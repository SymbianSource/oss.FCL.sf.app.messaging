/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     An HTML stripper
*
*/


// INCLUDE FILES
#include "MsgMailViewerHtmlConv.h"

#include <barsread.h>
#include <eikenv.h>
#include <f32file.h>
#include <aknnotewrappers.h>
#include <charconv.h>
#include <MailPlainView.rsg>
#include <MsgMailViewer.rsg>
#include "MailLog.h"



// LOCAL CONSTANTS AND MACROS
_LIT (KLineBreak, "<br>");
_LIT (KUnnumberedList, "<ul>");
_LIT (KUnnumberedListEnd, "</ul>");
_LIT (KOrderedList, "<ol>");
_LIT (KOrderedListEnd, "</ol>");
_LIT (KListItem, "<li>");
_LIT (KLessThan, "<");
_LIT (KGreaterThan, ">");
_LIT (KAmpersand, "&");
_LIT (KDefinitionList, "<dl>");
_LIT (KDefinitionListEnd, "</dl>");
_LIT (KDefinitionListTerm, "<dt>");
_LIT (KDefinitionListDef, "<dd>");
//_LIT (KDefinitionListDefEnd, "</dd>");
_LIT (KBodyTag, "<body>");
_LIT (KScript, "<script");
_LIT (KScriptEnd, "</script>");
_LIT (KNumberedItemSeparator, ". ");
_LIT (KNewParagraph, "<p>");
_LIT (KHeadStart, "<head>");
_LIT (KHeadEnd, "</head>");
_LIT (KHeading, "<h");
_LIT (KHeadingEnd, "</h");
_LIT (KStartHtml, "<html>");
_LIT (KEndHtml, "</html>");
_LIT (KHorizontalRuler, "<hr>");
_LIT (KImage, "<img");
_LIT (KImageAlt, "alt");
_LIT (KImageSrc, "src");
_LIT (KSpace, " ");
_LIT (KNewline, "\n");
_LIT (KListItemSymbol, "- ");
_LIT (KSemicolon, ";");
//_LIT (KQuotationMark, "\"");
_LIT (KBlock, "<div>");
_LIT (KBlockEnd, "</div>");
_LIT(KCharSet, "charset=");
_LIT( KCommentTagEnd, "-->");
_LIT( KOlId, "olid" );

const TInt KMaxOrdinalLength(5);
// "10000 is a reasonable size for a converted block, it has been
// tested that bigger size doesn't provide any faster conversion"
// Blocksize has been lowered to 1000 to make the UI more
// responsive during processing. Based on tests, the performance 
// is not noticeably changed by the smaller block size.
//
const TInt KMaxConvBlockLength( 1000 );

const TInt KSampleBufferSize = 256;
const TInt KMimimumConfidenceLevel = 50;
const TInt KMaxAmountOfSamples = KSampleBufferSize / 2;

enum TConversionState {
    EDeleteHeader = 1,
    EInitCharConverter,
    EConvert,
    ERemoveTags};

enum TStatusFlags {
    EForceUseCharacterSet = KBit0,
    EOwnsFileNameArray = KBit1
    };

// MODULE DATA STRUCTURES


// ================= MEMBER FUNCTIONS =======================

CStringPair::CStringPair()
    {
    }

CStringPair::~CStringPair()
    {
    delete iName;
    delete iValue;
    }

CStringPair* CStringPair::NewLC(const TDesC& aName,const TDesC& aValue)
    {
    CStringPair* temp = new(ELeave) CStringPair();
    CleanupStack::PushL(temp);
    temp->ConstructL(aName, aValue);
    return temp;
    }

void CStringPair::ConstructL(const TDesC& aName,const TDesC& aValue)
    {
    iName = aName.AllocL();
    iValue = aValue.AllocL();
    }

// C++ default constructor cannot contain any code that might leave
CMsgMailViewerHtmlConv::CMsgMailViewerHtmlConv()
    : CActive( CActive::EPriorityLow ),
    iPos(0), 
    iReturnValue(KErrNone),
    iErrorPos(KMaxTInt),
    iConversionState(EDeleteHeader),
    iCharacterSetId(0),
    iStatusFlags(EOwnsFileNameArray)
    {
    CActiveScheduler::Add( this );
    }

// Symbian OS default constructor can leave.
void CMsgMailViewerHtmlConv::ConstructL(RFile& aHandle,
    TUint aCharSet)
    {
    iFileIdArray = new(ELeave) RPointerArray<HBufC>;
	LOG1("CMsgMailViewerHtmlConv::ConstructL:%08x", aCharSet);
    iCharacterSetId = aCharSet;

    // read HTML content
    TInt size;
    User::LeaveIfError(aHandle.Size(size));
    HBufC8 *buf = HBufC8::NewLC(size);
    TPtr8 ptr(buf->Des());
    User::LeaveIfError(aHandle.Read(ptr));

    iOrigText = HBufC::NewL(buf->Length());
    TPtr ptr16(iOrigText->Des());
    ptr16.Copy(ptr);
    CleanupStack::PopAndDestroy(); // buf
    }


// Two-phased constructor.
CMsgMailViewerHtmlConv* CMsgMailViewerHtmlConv::NewLC(
	RFile& aHandle, TUint aCharset, TBool aForced)
	{
    CMsgMailViewerHtmlConv* self = new (ELeave) CMsgMailViewerHtmlConv();

    CleanupStack::PushL( self );
    if (aForced)
    	{
    	self->iStatusFlags |= EForceUseCharacterSet;
    	}
    self->ConstructL(aHandle, aCharset);

    return self;
	}

CMsgMailViewerHtmlConv* CMsgMailViewerHtmlConv::NewL(
    RFile& aHandle, TUint aCharset, TBool aForced)
    {
    CMsgMailViewerHtmlConv* self = 
        CMsgMailViewerHtmlConv::NewLC( aHandle, aCharset, aForced );
    CleanupStack::Pop( self );
    return self;    
    }
    
// Destructor
CMsgMailViewerHtmlConv::~CMsgMailViewerHtmlConv()
    {
    Cancel();
    delete iOrigText;
    delete iCharSetName;
    if (iMacros)
        {
        iMacros->ResetAndDestroy();
        }
    delete iMacros;

    if (iFileIdArray && ( iStatusFlags & EOwnsFileNameArray ) )
        {
        iFileIdArray->ResetAndDestroy();
        delete iFileIdArray;
        }
    delete iCharConv;
    }

TInt CMsgMailViewerHtmlConv::Convert()
    {
    TRAP( iReturnValue, DoConversionL() );
    LOG1( "CMsgMailViewerHtmlConv::Convert return:%d", iReturnValue );
    return iReturnValue;
    }

void CMsgMailViewerHtmlConv::Convert( TRequestStatus* aStatus )
    {
    LOG( "CMsgMailViewerHtmlConv::Convert (Async)");
    ASSERT( aStatus );
    Cancel();
    iReqStatus = aStatus;
    *aStatus = KRequestPending;
    ContinueAsyncConvert();
    }

void CMsgMailViewerHtmlConv::DoConvertL()
    {
    if ( !iCharConv )
        {
        //missing charconv is considered to be a
        //corrupt message situtation, the html part
        //will be available in attachment view
        ShowErrorL();
        User::Leave( KErrCorrupt );
        }

    // Conversion "cursor" location, initially start of descriptor
    TInt toConvertStart( 0 );
    // Length of text to still convert
    TInt toConvertLength( iOrigText->Length() );
    // Conversion is done in blocks to control heap consumption,
    // size of a block is always <= KMaxConvBlockLength
    TInt blockLength( Min( toConvertLength, KMaxConvBlockLength ) );
    // Initially no block is partially converted -> remainder == 0
    TInt blockRemainder( 0 );
    // Converter's state variable, mustn't be tampered with during conversion
    TInt state( CCnvCharacterSetConverter::KStateDefault );

    // 8bit source buffer and 16bit target buffer for a block. We use
    // first block's size as buffer size, since the following blocks
    // are either of same size or smaller
    HBufC8* buf8 = HBufC8::NewLC( blockLength );
    HBufC* buf = HBufC::NewLC( blockLength );

    // Pointers to the source and target texts.
    TPtr origPtr( iOrigText->Des() );
    TPtr8 srcPtr( buf8->Des() );
    TPtr dstPtr( buf->Des() );

    while ( toConvertLength > 0 ) // conversion loop
        {
        // Create a block for this conversion round
        blockLength = Min( toConvertLength, KMaxConvBlockLength );
        srcPtr.Copy( iOrigText->Mid( toConvertStart, blockLength ) );

        // Convert a block, return number of characters that weren't converted
        blockRemainder = iCharConv->ConvertToUnicode( dstPtr, srcPtr, state );
        // Handle possible errors
        if ( blockRemainder < 0 )
            {
            iPos = toConvertStart;
            ShowErrorL();
            break;
            }

        // Store the contents of the output buffer
        // Here unicodeLength may be less than convertedLength, e.g. in
        // case of "&auml;" ->"ä", and that must be taken into account
        TInt convertedLength = srcPtr.Length() - blockRemainder;
        TInt unicodeLength = dstPtr.Length();
        // Replace convertedLength characters with unicodeLength characters
        origPtr.Replace( toConvertStart, convertedLength, dstPtr );
        // Update progress status, move starting point to next
        // unconverted character
        toConvertStart += unicodeLength;
        toConvertLength = iOrigText->Length() - toConvertStart;
        }

    CleanupStack::PopAndDestroy( 2 ); // buf8, buf
    }

void CMsgMailViewerHtmlConv::DoConversionL()
    {
    switch (iConversionState)
        {
        case EDeleteHeader:
            {
            LOG("EDeleteHeader");

            // Try to find charset name if not set already
            if ( !(iStatusFlags & EForceUseCharacterSet) )
            	{
	            TInt charSetPos(iOrigText->FindF(KCharSet()));
	            if ( charSetPos != KErrNotFound)
	                {
                	// Begin of charset name
                	charSetPos += KCharSet().Length();
	                TInt maxIdLength(charSetPos+20); // Give up if id end not found at this point.
	                for (TInt i = charSetPos; i<maxIdLength; ++i)
	                    {
	                    if((*iOrigText)[i] == '"')
	                        {
	                        iCharSetName = iOrigText->Mid(
	                            charSetPos, i-charSetPos).AllocL();
	                        break;
	                        }
	                    }
	                }
            	}

            // 5 for <body
            TInt bodyPos(iOrigText->FindF(KBodyTag().Left(5)));
            if ( bodyPos != KErrNotFound)
                {
                // Delete from start to body tag
                Delete(bodyPos);
                // Delete body tag
                RemoveTagL();
                }
            else // no body tag, invalid HTML, search for <html> then
                {
                bodyPos = iOrigText->FindF(KStartHtml().Left(5));
                if (bodyPos != KErrNotFound)
                    {
                    Delete(bodyPos);
                    RemoveTagL();
                    }
                }

            iConversionState++;
            break;
            }

        case EInitCharConverter:
            {
            RFs fs = CEikonEnv::Static()->FsSession();
            iCharConv = CCnvCharacterSetConverter::NewL();
            // Try to find character set id from the HTML header
            if ( iCharSetName && iCharSetName->Length() )
                {
                HBufC8* buf8 = HBufC8::NewLC( iCharSetName->Length() );
                    buf8->Des().Copy( *iCharSetName );

                iCharacterSetId = iCharConv->
                    ConvertStandardNameOfCharacterSetToIdentifierL(
                    *buf8, fs);

                CleanupStack::PopAndDestroy(); // buf8
                }

            if (iCharacterSetId > 0)
                {
                LOG1("CMsgMailViewerHtmlConv::EInitCharConverter:%08x",
                	iCharacterSetId);
                const TInt ret(iCharConv->PrepareToConvertToOrFromL(
                    iCharacterSetId,
                    fs));

                if (ret == CCnvCharacterSetConverter::ENotAvailable)
                    {
					LOG( "CMsgMailViewerHtmlConv::EInitCharConverter: ENotAvailable" );
                    iCharacterSetId = 0;
                    delete iCharConv;
                    iCharConv = NULL;
                    }

                }
            else
                {
                //regular method of getting charset id failed.
                //Try autodetect instead.

                TInt confidence=0;
                const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* availableSets =
                CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC( fs );

                HBufC8* sample = HBufC8::NewLC( KSampleBufferSize );
	            TPtr8 sampleDes = sample->Des();

	            sampleDes.Copy(iOrigText->Left( iOrigText->Length() > KMaxAmountOfSamples ?
	                                                KMaxAmountOfSamples : iOrigText->Length()));

                iCharConv->AutoDetectCharSetL(confidence, iCharacterSetId, *availableSets, sampleDes);

                if(confidence > KMimimumConfidenceLevel)
                    {
                    const TInt ret(iCharConv->PrepareToConvertToOrFromL(
                    iCharacterSetId,
                    fs));
                    }
                else
                    {
                    //autodetect not reliable enough.
                    delete iCharConv;
                    iCharConv = NULL;
                    }

                CleanupStack::PopAndDestroy(2); // availableSets, sample
                }

            iConversionState++;
            break;
            }

        case EConvert:
            {
            LOG("EConvert");
            DoConvertL();

            // reset position to beginning before step to next state.
            iPos = 0;
			iConversionState++;
            break;
            }

        case ERemoveTags:
            {
            LOG("ERemoveTags");
            /**
            * This operation is done inside the CAknWaitnoteWrapper StepL
            * (CActive::RunL) and thus it should not take too much time.
            * We use maxloops to break out from time consuming operation,
            * so that wait note can be updated.
            */
            TInt maxloops(10000);
            while (iReturnValue == KErrNone && iPos < iOrigText->Length())
                {
                if (maxloops < 0)
                    {
                    LOG("maxloops");
                    // return to update wait note.
                    return;
                    }
                TPtrC character = iOrigText->Mid(iPos, 1);
                if (character == KAmpersand)
                    {
                    // Replace
                    ReplaceMacroL();
                    // takes more time, so decrease by ten
                    maxloops -= 10;
                    }
                else if (character == KLessThan)
                    {
                    // Remove possible tag
                    HandleTagL(0);
                    // takes more time, so decrease by ten
                    maxloops -= 10;
                    }
                // replace linefeeds with one space
                else if (character[0] == 0x0A || character[0] == 0x0D)
                	{
                	// +1 peek next character
                	if (iPos+1 < iOrigText->Length() &&
                		(iOrigText->Mid(iPos+1, 1)[0] == 0x0A ||
                		iOrigText->Mid(iPos+1, 1)[0] == 0x0D))
                		{
                		Delete(1); // delete character
                   		}
                	else
                		{
                		// insert space
                		TPtr ptr(iOrigText->Des());
                		ptr.Replace(iPos, 1, KSpace);
                	    // move to next char
                        iPos++;
                		}
                	}
                else
                    {
                    // +1 peek next char
                    if (character == KSpace &&
                        iPos+1 < iOrigText->Length() &&
                        iOrigText->Mid(iPos+1, 1) == KSpace)
                        {
                        // delete multible spaces
                        Delete(1);
                        }
                    else
                        {
                        // move to next char
                        iPos++;
                        }
                    maxloops--;
                    }
                }
            iConversionState++;
            // conversion ended
            if (iReturnValue == KErrNone)
                {
                iReturnValue = KErrEof;
                }
            break;
            }

        default:
            LOG("default");
            // conversion ended
            iReturnValue = KErrEof;
            break;
        }
    }


TInt CMsgMailViewerHtmlConv::HandleListL(const TInt aIndent,
                                         const TBool aNumbered)
    {
    TInt item(1);
    TInt len(RemoveTagL()); // remove <ol>
    TInt remaining(len - iPos);
    // length of the buf is 4, so we can find and distinguish <li> and </ol
    TBuf<4> buf(iOrigText->Mid(iPos, Min(4, remaining)));
    buf.LowerCase();
    const TInt currPos(iPos);

    while (iPos < len && buf != (aNumbered ? KOrderedListEnd().Left(4) :
        KUnnumberedListEnd().Left(4)))
        {
        if (buf == KListItem)
            {
            RemoveTagL();

            HBufC *fill = HBufC::NewLC(aIndent + KMaxOrdinalLength);
            TPtr fillPtr = fill->Des();

            fillPtr.AppendFill(' ', aIndent);
            if (aNumbered)
                {
                fillPtr.AppendNum(item);
                fillPtr.Append(KNumberedItemSeparator);
                }
            else
                {
                fillPtr.Append(KListItemSymbol);
                }

            InsertTextL(fillPtr);
            CleanupStack::PopAndDestroy(); // fill

            InsertLinefeedL();

            len = iOrigText->Length();
            iPos += aIndent + (aNumbered ? 3 : 2); // iPos is always moved at least by 1
            item++;
            }
        else if ( buf.Left(1)[0] == 0x0A || buf.Left(1)[0] == 0x0D )
            {
            Delete(1); // delete linefeeds
            }
        else if (buf.Left(1) == KLessThan)
            {
            len = HandleTagL(aIndent + 2);
            }
        else if (buf.Left(1) == KAmpersand)
            {
            len = ReplaceMacroL();
            }
        else
            {
            iPos++;
            }
        remaining = len - iPos;
        buf = iOrigText->Mid(iPos, Min(4, remaining));
        buf.LowerCase();
        }

    if ( iPos != len)
        {
        InsertLinefeedL();
        len = RemoveTagL(); // delete </ol>
        }
    else
        {
        iPos = currPos;
        ShowErrorL();
        len = iOrigText->Length();
        }

    return len;
    }

TInt CMsgMailViewerHtmlConv::HandleDListL(TInt aIndent)
    {
    TInt len(Delete(4)); // remove <dl>
    TInt remaining(len - iPos);
    // length of the buf is 4, so we can find and distinguish <li> and </dl
    TBuf<4> buf(iOrigText->Mid(iPos, Min(4, remaining)));
    buf.LowerCase();
    const TInt currPos(iPos);

    while (iPos < len && buf != KDefinitionListEnd().Left(4))
        {
        if (buf == KDefinitionListTerm)
            {
            len = RemoveTagL();
            HBufC *fill = HBufC::NewLC(aIndent);
            TPtr fillPtr = fill->Des();

            fillPtr.AppendFill(' ', aIndent);
            InsertTextL(fillPtr);
            CleanupStack::PopAndDestroy(); // fill

            InsertLinefeedL();
            iPos += aIndent; // skip spaces
            len = iOrigText->Length();
            }
        else if (buf == KDefinitionListDef)
            {
            len = RemoveTagL();
            InsertTextL(KSpace);
            }
        else if (buf.Left(1) == KLessThan)
            {
            len = HandleTagL(aIndent + 2);
            }
        else if (buf.Left(1) == KAmpersand)
            {
            len = ReplaceMacroL();
            }
        else
            {
            iPos++;
            }
        remaining = len - iPos;
        buf = iOrigText->Mid(iPos, Min(4, remaining));
        buf.LowerCase();
        }

    if ( iPos != len)
        {
        len = RemoveTagL(); // delete </dl>
        }
    else // </dl> missing
        {
        iPos = currPos;
        ShowErrorL();
        len = iOrigText->Length();
        }

    return len;
    }

TInt CMsgMailViewerHtmlConv::HandleTagL(TInt aIndent)
    {
    TInt length(iOrigText->Length());
    const TInt remaining(length - iPos);

    if (!remaining)
        {
        // Nothing to remove
        iReturnValue = KErrEof;
        return iOrigText->Length();
        }

    // Longest HTML tag is 6 characters long, thus six char buffer
    TBuf<6> buf(iOrigText->Mid(iPos+1, Min(6, remaining - 1)));
    buf.LowerCase();
    buf.TrimAll();

    if (buf.Left(2) == KLineBreak().Mid(1,2) ||
        buf.Left(1) == KNewParagraph().Mid(1,1) ||
        buf.Left(2) == KHorizontalRuler().Mid(1,2) ||
		buf.Left(3) == KBlock().Mid(1,3))
        {
        length = RemoveTagL(); // Delete <br>, <p>, <hr> or <div>
        InsertLinefeedL();
        return length;
        }
	else if (buf.Left(4) == KBlockEnd().Mid(1,4))
		{
        length = RemoveTagL(); // Delete </div>
	    if ( (length - iPos) > 6 ) // still data to peek to?
			{
			// peek the next tag and if it's <div>, don't add newline
			TPtrC peekPtr(iOrigText->Mid(iPos));
			TInt found = peekPtr.Find(KLessThan);
			// found + 3 there needs to be at least 3 characters for "div" check
			if ( found != KErrNotFound && iPos+found+3 < length )
				{
				// +1 step over "<"
				TBuf<6> peekbuf( iOrigText->Mid(
				    iPos+found+1, Min(6, length - iPos - 1)) );
			    peekbuf.LowerCase();
				peekbuf.TrimAll();
				// compare "div" part
				if ( peekbuf.Length() > 3 &&
				    peekbuf.Left(3) != KBlock().Mid(1,3) )
				    {
					// next tag is not <div>, add newline
					InsertLinefeedL();
				    }
				}
			}
        return length;
		}
    else if (buf.Left(2) == KUnnumberedList().Mid(1,2))
        {
        return HandleUListL(aIndent);
        }
    else if (buf.Left(2) == KOrderedList().Mid(1,2))
        {
		// check for Outlook's olid tag, not to be confused
		// with ordered list tag <ol>
		if( buf.Left( 4 ).Match( KOlId ) == 0 )
			{
			return RemoveTagL(); // Delete <olid ....>
			}
		return HandleOListL(aIndent);
        }
    else if (buf.Left(2) == KDefinitionList().Mid(1,2))
        {
        return HandleDListL(aIndent);
        }
    else if (buf.Left(4) == KHeadStart().Mid(1,4))
        {
        const TInt currPos(iPos);
        length = RemoveTagL(); // Remove <head ...>
        TPtrC tagPos(iOrigText->Mid(iPos));
        TBool found(EFalse);
        TInt endTag(tagPos.Find(KLessThan));
        while (!found && endTag != KErrNotFound
            && iOrigText->Length() >= endTag+5 )
            {
            buf = iOrigText->Mid(endTag+1, 4);
            buf.TrimAll();
            buf.LowerCase();
            if (buf == KHeadEnd().Mid(1, 4))
                {
                found = ETrue;
                Delete(endTag - iPos);
                return RemoveTagL();
                }
            else
                {
                TPtrC tmpPos(iOrigText->Mid(endTag+1));
                const TInt p(tmpPos.Find(KLessThan));
                endTag = p == KErrNotFound ? KErrNotFound : endTag + p + 1;
                }
            }
        if (endTag == KErrNotFound)
            {
            iPos = currPos;
            ShowErrorL();
            return iOrigText->Length();
            }
        }
    else if (buf.Left(1) == KHeading().Mid(1,1))
        {
        CDesCArrayFlat *headArray = new(ELeave) CDesCArrayFlat(6);
        CleanupStack::PushL(headArray);
        for (TInt i = 1; i < 7; i++)
            {
            TBuf<3> tag = KHeading().Mid(1);
            tag.AppendNum(i);
            tag.Append(KGreaterThan);
            headArray->AppendL(tag);
            }
        TInt foundInPos;
        if (headArray->Find(buf.Left(3), foundInPos) == 0)
            {
            length = RemoveTagL(); // remove <h?>
            InsertLinefeedL();
            CleanupStack::PopAndDestroy(); // headArray
            return length;
            }
        CleanupStack::PopAndDestroy(); // headArray
        }
    else if (buf.Left(2) == KHeadingEnd().Mid(1,2) &&
             buf != KEndHtml().Mid(1, 6))
        {
        CDesCArrayFlat *headArray = new(ELeave) CDesCArrayFlat(6);
        CleanupStack::PushL(headArray);
        for (TInt i = 1; i < 7; i++)
            {
            TBuf<4> tag = KHeadingEnd().Mid(1);
            tag.AppendNum(i);
            tag.Append(KGreaterThan);
            headArray->AppendL(tag);
            }
        TInt foundInPos;
        if (headArray->Find(buf.Left(4), foundInPos) == 0)
            {
            length = RemoveTagL(); // remove </h?>
            InsertLinefeedL();
            CleanupStack::PopAndDestroy(); // headArray
            return length;
            }
        CleanupStack::PopAndDestroy(); // headArray
        }
    else if (buf == KScript().Mid(1))
        {
        const TInt currPos(iPos);
        length = RemoveTagL(); // Remove <script ...>
        TPtrC tagPos(iOrigText->Mid(iPos));
        TBool found(EFalse);
        TInt endTag(tagPos.Find(KLessThan));
        while (!found && endTag != KErrNotFound
            && iOrigText->Length() >= endTag+7 )
            {
            buf = iOrigText->Mid(endTag+1, 6);
            buf.TrimAll();
            buf.LowerCase();
            if (buf == KScriptEnd().Mid(1, 6))
                {
                found = ETrue;
                Delete(endTag - iPos);
                return RemoveTagL();
                }
            else
                {
                TPtrC tmpPos(iOrigText->Mid(endTag+1));
                const TInt p(tmpPos.Find(KLessThan));
                endTag = p == KErrNotFound ? KErrNotFound : endTag + p + 1;
                }
            }
        if (endTag == KErrNotFound)
            {
            iPos = currPos;
            ShowErrorL();
            return iOrigText->Length();
            }
        }
    else if (buf.Left(3) == KImage().Mid(1,3))
        {
        TBool inQuoted(EFalse); // in quoted string
        TBool inAltPart(EFalse);
        TBool inAltQuotedPart(EFalse);
        TBool inSrcPart(EFalse);
        const TInt KInitialSize(50);
        HBufC* inlineImage = HBufC::NewLC(KInitialSize);

        while ((*iOrigText)[iPos] != '>' && iPos < iOrigText->Length())
            {
            TChar ch((*iOrigText)[iPos]);
            const TInt remaining(iOrigText->Length() - iPos);
            HBufC* lowerCaseCopy = iOrigText->Mid(iPos,
                remaining > 3 ? 3 : remaining).AllocLC();
            TPtr lowerCasePtr(lowerCaseCopy->Des());
            lowerCasePtr.LowerCase();

            if (inQuoted && ch != '"')
                {
                if ( inlineImage->Length() == inlineImage->Des().MaxLength() )
                    {
                    HBufC* temp = inlineImage->ReAllocL(
                        inlineImage->Length() + KInitialSize);
                    CleanupStack::Pop(2); //lowerCaseCopy, inlineImage
                    CleanupStack::PushL( inlineImage = temp );
                    CleanupStack::PushL( lowerCaseCopy );
                    }

                // Leave alt quoted to the body
                if ( inAltQuotedPart )
                    {
                    iPos++;
                    }
                else if ( inSrcPart )
                    {
                    inlineImage->Des().Append(ch);
                    Delete(1);
                    }
                else
                    {
                    // other queted parts are deleted
                    Delete(1);
                    }

                }
            else if (ch == '"')
                {
                Delete(1);
                inQuoted = !inQuoted;
                if (inAltPart)
                    {
                    inAltQuotedPart = ETrue;
                    inAltPart = EFalse;
                    }
                else if (inAltQuotedPart)
                    {
                    inAltQuotedPart = EFalse;
                    }
                else if (inSrcPart && !inQuoted)
                    {
                    // end of Src Quoted
                    inSrcPart = EFalse;
                    }
                }
            else if (!lowerCaseCopy->Compare(KImageAlt))
                {
                Delete(KImageAlt().Length()); // delete alt
                inAltPart = ETrue;
                inSrcPart = EFalse;
                }
            else if (!lowerCaseCopy->Compare(KImageSrc))
                {
                Delete(KImageSrc().Length()); // delete src
                inAltPart = EFalse;
                inSrcPart = ETrue;
				}
            else
                {
                Delete(1);
                }
            CleanupStack::PopAndDestroy(); // lowerCaseCopy
            }
        if ( inlineImage->Length() )
            {
            User::LeaveIfError( iFileIdArray->Append(inlineImage) );
            CleanupStack::Pop(); // inlineImage
            }
        else
            {
            CleanupStack::PopAndDestroy(); // inlineImage
            }

        if (iPos < iOrigText->Length())
            {
            Delete(1); // delete >
            }
        return iOrigText->Length();
        }

    return RemoveTagL();
    }

TInt CMsgMailViewerHtmlConv::RemoveTagL()
    {
    const TInt currPos(iPos);
    TPtrC tagPos(iOrigText->Mid(iPos));
    const TInt tagLength(tagPos.Find(KGreaterThan));
    if (tagLength == KErrNotFound)
        {
        iReturnValue = KErrEof; //  ">" not found at the end
        iPos = 0;
        return 0;
        }

    if (tagPos[1] == '!') // start of the comment
        {
        const TInt commentLength( tagPos.Find( KCommentTagEnd ) );
        if (commentLength != KErrNotFound)
            return Delete(commentLength + 3); // 3 for -->

        return Delete(tagLength+1);// not comment delete whole tag;
        }
    // Check that > really belongs to this tag
    TPtrC tmpPos(iOrigText->Mid(iPos+1));
    const TInt firstTagStart(tmpPos.Find(KLessThan));
    if (tagLength != KErrNotFound &&
        ( firstTagStart == KErrNotFound ||
          tagLength <= firstTagStart ) )
        {
        return Delete(tagLength+1);
        }
    else
        {
        // check that found < really belongs to tag which end was found
        // and it's not only wrongly encoded
        if (firstTagStart < tagLength)
            {
            iPos++;
            return iOrigText->Length();
            }
        else
            {
            iPos = currPos;
            ShowErrorL();
            return iOrigText->Length();
            }
        }
    }

RPointerArray<HBufC>* CMsgMailViewerHtmlConv::FileIdArray()
    {
    iStatusFlags &= ~EOwnsFileNameArray;
    return iFileIdArray;
    }

TBool CMsgMailViewerHtmlConv::AutoParsedCharSetNameAndIdentifier() const
    {
    TBool autoParsed = EFalse;
    //
    if ( !(iStatusFlags & EForceUseCharacterSet) )
        {
        autoParsed = ( iCharSetName != NULL ) && ( iCharacterSetId != 0);
        }
    //
    return autoParsed;
    }

TUint CMsgMailViewerHtmlConv::AutoParsedCharSetIdentifier() const
    {
    return iCharacterSetId;
    }



TInt CMsgMailViewerHtmlConv::Delete(const TInt aLen)
    {
    TPtr ptr(iOrigText->Des());
    ptr.Delete(iPos, aLen);
    return iOrigText->Length();
    }

// InsertTextL()
// It is checked that text to be inserted fits into a descriptor and
// a descriptor's size is doubled if not. In practice this should never
// happen because insertions are usually one character to the place where
// there used to be i.e. a 4-char macro.
//
TInt CMsgMailViewerHtmlConv::InsertTextL(const TDesC& aChars)
    {
    if (iOrigText->Des().MaxLength() < iOrigText->Length() + aChars.Length())
        {
        iOrigText = iOrigText->ReAllocL((
            iOrigText->Length() + aChars.Length()) * 2);
        }

    TPtr ptr(iOrigText->Des());
    ptr.Insert(iPos, aChars);
    return iOrigText->Length();
    }

TInt CMsgMailViewerHtmlConv::ReplaceMacroL()
    {
    if (!iMacros)
        {
        InitMacroArrayL();
        }

    TInt len(iOrigText->Length());
    const TPtrC macroPos(iOrigText->Mid(iPos+1));
    const TInt macroLength(macroPos.Find(KSemicolon));
    if (macroLength > 0)
        {
        const TPtrC macroPtr(iOrigText->Mid(iPos+1, macroLength));

        TInt pos;
        // A dummy string pair for Find-method
        TDesC tmpDesC(KSpace);
        CStringPair* tmpPair = CStringPair::NewLC(macroPtr, tmpDesC);
        if (iMacros->FindInOrder(tmpPair, pos,
            TLinearOrder<CStringPair>(CStringPair::Compare)) != KErrNotFound)
            {
            // + 2 for '&' + ';'
            Delete( ((*iMacros)[pos])->GetName()->Length() + 2);
            len = InsertTextL( *((*iMacros)[pos]->GetValue()) );
            iPos++;
            CleanupStack::PopAndDestroy(); // tmpPair
            return len;
            }
        CleanupStack::PopAndDestroy(); // tmpPair
        if (macroPtr[0] == '#') // numeric form
            {
            const TPtrC ptr(iOrigText->Mid(iPos+2, macroLength - 1)); // skip #
            TLex lex(ptr);
            TUint value(0);
            TInt err(KErrNone);
            if (User::LowerCase(lex.Peek()) == 'x')
                { // character in hex
                lex.Inc(1); // skip x in &#x20AC
                err = lex.Val(value, EHex);
                }
            else
                { // character in decimal
                err = lex.Val(value);
                }
            if (err == KErrNone)
                {
                // Check how many chars we have actually consumed.
                TInt offset = lex.Offset(); 
                // NCR could be missing the semicolon.
                if( lex.Peek() == ';' )
                    {
                    offset++;
                    }

                // + 2 for # & ;
                Delete( offset + 2 );
                TBuf<1> buf;
                buf.Append(value);
                len = InsertTextL(buf);
                iPos++;
                return len;
                }
            }
        }

    // this isn't correctly encoded macro, but let's skip it instead of
    // showing an error, so user can see as much of the message as possible
    iPos++;
    return iOrigText->Length();
    }

void CMsgMailViewerHtmlConv::InitMacroArrayL()
    {
    TResourceReader reader;
    CEikonEnv::Static()->CreateResourceReaderLC(reader, R_MAIL_HTML_MACRO);
    const TInt count(reader.ReadInt16());
    iMacros = new(ELeave) RPointerArray<CStringPair>(count);
    for (TInt i=0; i<count; i++)
        {
        HBufC* macro = reader.ReadHBufCL();
        CleanupStack::PushL(macro);
        HBufC* realText = reader.ReadHBufCL();
        CleanupStack::PushL(realText);
        CStringPair* tmp = CStringPair::NewLC(*macro, *realText);
        // this should never fail, because we allocated enough memory in
        // construction
        User::LeaveIfError(iMacros->InsertInOrder(
            tmp, TLinearOrder<CStringPair>(CStringPair::Compare)));
        CleanupStack::Pop(); // tmp
        CleanupStack::PopAndDestroy(2); // macro, realText
        }
    CleanupStack::PopAndDestroy(); // reader
    iMacros->Compress();
    }

void CMsgMailViewerHtmlConv::ShowErrorL()
    {
    iErrorPos = iPos;
    TInt deleteRest(iOrigText->Length() - iPos);
    if (deleteRest > 0)
        {
        Delete(deleteRest);
        }
    InsertLinefeedL();
    HBufC* text =
        CEikonEnv::Static()->AllocReadResourceLC(R_ERROR_IN_HTML_TEXT);
    InsertTextL(*text);
    CleanupStack::PopAndDestroy(); // text
    iReturnValue = KErrCorrupt;
    iPos = iOrigText->Length();
    }

void CMsgMailViewerHtmlConv::InsertLinefeedL()
    {
    if (iPos > 0 && iPos < iOrigText->Length() )
        {
        TPtr ptr(iOrigText->Des());
        ptr.Insert(iPos, KNewline);
        iPos++;
        }
    }

void CMsgMailViewerHtmlConv::RunL()
    {
    TInt err = Convert();
    if( !err ) // KErrNone
        {
        // still data to convert. Continue work on next loop.
        ContinueAsyncConvert();
        }
    else if( err == KErrEof )
        {
        // success
        User::RequestComplete( iReqStatus, KErrNone );
        }
    else
        {
        // error occured
        User::RequestComplete( iReqStatus, err );
        }
    }

void CMsgMailViewerHtmlConv::DoCancel()
    {
    User::RequestComplete( iReqStatus, KErrCancel );
    }

void CMsgMailViewerHtmlConv::ContinueAsyncConvert()
    {
    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// ================= OTHER EXPORTED FUNCTIONS ==============


//  End of File
