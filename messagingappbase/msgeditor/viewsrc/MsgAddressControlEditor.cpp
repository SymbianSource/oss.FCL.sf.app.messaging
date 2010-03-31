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
* Description:  MsgAddressControlEditor implementation
*
*/



// ========== INCLUDE FILES ================================

#include <barsread.h>                      // for TResourceReader
#include <eikenv.h>                        // for CEikonEnv
#include <txtrich.h>                       // for CRichText
#include <AknUtils.h>                      // for AknUtils
#include <AknSettingCache.h>               // for SettingCache().InputLanguage()
#include <AknsUtils.h>                     // for AknsUtils::GetCachedColor()
#include <AknsConstants.h>                 // for KAknsIIDQsnHighlightColors and EAknsCIQsnHighlightColorsCG1

#include <aknlayout.cdl.h> // LAF
#include <txtetext.h>
#include <fldinfo.h>

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif 


#include "MsgEditorCommon.h"               //
#include "MsgAddressControlEditor.h"       // for CMsgAddressControlEditor
#include "MsgAddressControlEditorField.h"  // for CMsgAddressControlEditorField and TMsgAddressControlEditorFieldFactory
#include "MsgBaseControl.h"                // for TMsgEditPermissionFlags
#include "MsgBaseControlObserver.h"        // for MMsgBaseControlObserver
#include "MsgAddressControl.h"             // for permission flags
#include "MsgEditorPanic.h"                // for MsgEditor panics
#include "MsgEditor.hrh"                   // for EditPermission flags
#include "MsgEditorKeyCatcher.h"           // for CMsgEditorKeyCatcher

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// Highlighted field does not exist.
const TInt KMsgHighlightedFieldPosNone = -1;
// Current entry not defined.
const TInt KMsgCurrentEntryNone        = -1;

const TText KZeroWidthJoiner = 0x200D;
const TText KLRMarker = 0x200E;
const TText KRLMarker = 0x200F;

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgAddressControlEditor::CMsgAddressControlEditor
//
// Constructor.
// ---------------------------------------------------------
//
CMsgAddressControlEditor::CMsgAddressControlEditor(
    const CCoeControl* aParent,
    TUint32& aFlags,
    MMsgBaseControlObserver* aBaseControlObserver )
    : CMsgExpandableControlEditor( aParent, aFlags, aBaseControlObserver ),
      iHighLightedFieldPos( KMsgHighlightedFieldPosNone ),
      iCurrentEntry( KMsgCurrentEntryNone )
    {
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::~CMsgAddressControlEditor
//
// Destructor.
// ---------------------------------------------------------
//
CMsgAddressControlEditor::~CMsgAddressControlEditor()
    {
    delete iKeyCatcher;
    delete iAddressControlEditorFieldFactory;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::ConstructL()
    {
    CMsgExpandableControlEditor::ConstructL();
    
    //To Dis-able Smiley support for address fields
    RemoveFlagFromUserFlags(EAvkonEnableSmileySupport);
    
    iFlags |= EMsgControlModeDoNotDrawFrameBgContext; 
    iKeyCatcher = new ( ELeave ) CMsgEditorKeyCatcher();
    iKeyCatcher->ConstructL( this );

    SetAllowUndo( EFalse );
    // Variation is done inside edwin
    SetPhoneNumberGrouping( ETrue );
    iCusPos = 0;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::ExtractEntry
//
// Extracts an entry according to aEntryNumber and returns it in aEntryString.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::ExtractEntry(
    TDes& aEntryString, TInt aEntryNumber )
    {
    __ASSERT_DEBUG( ( aEntryNumber >= 0 ) &&
        ( aEntryNumber < iRichText->ParagraphCount() ),
        Panic( EMsgInvalidEntryNumber ) );

    TInt entryLength = 0;
    TInt entryStart = iRichText->CharPosOfParagraph( entryLength, aEntryNumber );
    iRichText->Extract( aEntryString, entryStart, entryLength - 1 );
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::ReadEntry
//
// Reads an entry aEntryNumber and returns pointer to it.
// ---------------------------------------------------------
//
TPtrC CMsgAddressControlEditor::ReadEntry( TInt aEntryNumber ) const
    {
    __ASSERT_DEBUG( ( aEntryNumber >= 0 ) &&
        ( aEntryNumber < iRichText->ParagraphCount() ),
        Panic( EMsgInvalidEntryNumber ) );

    TInt entryLength = 0;
    TInt entryStart = iRichText->CharPosOfParagraph( entryLength, aEntryNumber );
    return iRichText->Read( entryStart, entryLength - 1 );
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::EntryCount
//
// Returns number of entries.
// ---------------------------------------------------------
//
TInt CMsgAddressControlEditor::EntryCount() const
    {
    return iRichText->ParagraphCount();
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::DeleteEntryL
//
// Deletes an entry aEntryNumber.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::DeleteEntryL( TInt aEntryNumber )
    {
    __ASSERT_DEBUG( ( aEntryNumber >= 0 ) &&
        ( aEntryNumber < iRichText->ParagraphCount() ),
        Panic( EMsgInvalidEntryNumber ) );

    CancelFepTransaction();
    TBool setcursorpos(EFalse);
    TInt numOfEntries = iRichText->ParagraphCount();
    TInt entryLength = 0;
    TInt entryStart = iRichText->CharPosOfParagraph( entryLength, aEntryNumber );
    if ( numOfEntries == aEntryNumber + 1 ) //entry to be deleted is the lowest one
        {
        if ( aEntryNumber != 0 ) // but not the only one
            {
            entryStart--;
            }
        else
            {
            entryLength--;
            }
        }
    
    if ( TextView() )
        {
        // Cursor position must not go beyond text end. Otherwise
        // panic will happen on text formatting.
        TInt newTextLength = TextLength() - entryLength;
        
        if ( CursorPos() > newTextLength )
            {
            SetCursorPosL( newTextLength, EFalse );
            iCusPos = newTextLength;
            setcursorpos = ETrue;
            }
        }
    
    iRichText->CancelInsertCharFormat();
    iRichText->DelSetInsertCharFormatL( entryStart, entryLength );
    if ( iTextView )
       {
        iTextView->HandleInsertDeleteL(
            TCursorSelection( entryStart, entryStart + entryLength ), entryLength, EFalse );
       }
     numOfEntries = iRichText->ParagraphCount();
     if( numOfEntries > 0 && numOfEntries != aEntryNumber && !setcursorpos)
        {
         TInt  txtlen = TextLength();
         TInt  curpos = CursorPos();        
         if ( aEntryNumber ) 
             {
             entryStart = iRichText->CharPosOfParagraph( entryLength, aEntryNumber - 1 );         
             curpos = entryStart + --entryLength ;
             }
         else 
             {
             entryStart = iRichText->CharPosOfParagraph( entryLength, aEntryNumber );         
             curpos+= --entryLength;
             }
         
         if ( TextView() )
            {
                if(curpos < txtlen)
                {
                    iCusPos = curpos;
                    SetCursorPosL( curpos, EFalse );   
                }
              else
                {
                    iCusPos =entryStart ;
                    SetCursorPosL( iCusPos, EFalse );   
                    ReportEdwinEventL( MEikEdwinObserver::EEventTextUpdate );
                }        
            }
        }
 }
 // ---------------------------------------------------------
// CMsgAddressControlEditor::GetCursorpos
//
// returns the current cursor position.
// ---------------------------------------------------------
//

TInt CMsgAddressControlEditor::GetCursorpos() const
    {
        return iCusPos;
    }
    
// ---------------------------------------------------------
// CMsgAddressControlEditor::ClearEntryL
//
// Clears all the text of an entry aEntryNumber but not the paragraph delimiter.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::ClearEntryL( TInt aEntryNumber, TBool aUpdate )
    {
    __ASSERT_DEBUG( ( aEntryNumber >= 0 ) &&
        ( aEntryNumber < iRichText->ParagraphCount() ),
        Panic( EMsgInvalidEntryNumber ) );

    TInt entryLength = 0;
    TInt entryStart = iRichText->CharPosOfParagraph( entryLength, aEntryNumber );
    DeleteEntryCharsL( aEntryNumber, 0, entryLength );
    if ( iTextView && aUpdate )
        {
        iTextView->HandleInsertDeleteL(
            TCursorSelection( entryStart, entryStart ), --entryLength, EFalse );
        SetCursorPosAndCancelSelectionL( entryStart );
        }
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::DeleteAllL
//
// Clears the content of the editor. Updates also the scroll bars.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::DeleteAllL()
    {
    CancelFepTransaction();
    iRichText->CancelInsertCharFormat();
    iRichText->Reset();
    if (iTextView)
        {
        SetAmountToFormatL( ETrue );
        SetCursorPosAndCancelSelectionL( 0 );
        }
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::DeleteEntryCharsL
//
// Deletes characters from aStartPos to aEndPos at entry aEntryNumber. Never
// removes the paragraph delimiter.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::DeleteEntryCharsL(
    TInt aEntryNumber, TInt aStartPos, TInt aEndPos)
    {
    __ASSERT_DEBUG( ( aEntryNumber >= 0 ) &&
        ( aEntryNumber < iRichText->ParagraphCount() ),
        Panic( EMsgInvalidEntryNumber ) );
    __ASSERT_DEBUG( ( aStartPos >= 0 ) && ( aStartPos <= aEndPos ),
        Panic( EMsgInvalidStartPos ) );

    CancelFepTransaction();
    TInt entryLength = 0;
    TInt entryStart = iRichText->CharPosOfParagraph( entryLength, aEntryNumber );
    entryLength--;
    entryLength = ( aEndPos < entryLength )
        ? aEndPos - aStartPos
        : entryLength - aStartPos;
    entryStart += aStartPos;
    
    if ( TextView() )
        {
        // Cursor position must not go beyond text end. Otherwise
        // panic will happen on text formatting.
        TInt newTextLength = TextLength() - entryLength;
        
        if ( CursorPos() > newTextLength )
            {
            SetCursorPosL( newTextLength, EFalse );
            }
        }
    
    iRichText->CancelInsertCharFormat();
    iRichText->DelSetInsertCharFormatL( entryStart, entryLength );
    //clear the iTextView cache too
    if ( iTextView )
        {
        iTextView->HandleInsertDeleteL(
            TCursorSelection( entryStart, entryStart + entryLength ), entryLength, EFalse );
        SetCursorPosAndCancelSelectionL( entryStart );
        }
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::SetCursorPosAndCancelSelectionL
//
// Sets cursor position to aPos and cancels selection.
// This is needed because Edwin/TextView does not do it as expected.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::SetCursorPosAndCancelSelectionL( TInt aPos )
    {
    if ( iTextView )
        {
        CancelFepTransaction();

        iTextView->SetDocPosL( aPos, EFalse );
        ForceScrollBarUpdateL();
        }
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::InsertEntryL
//
// Inserts an entry to the location aEntryNumber with strings a text aName
// and aAddress. Makes an entry as verified if aVerified = ETrue and validated
// if aValidated = ETrue.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::InsertEntryL(
    TInt aEntryNumber,
    const TDesC& aName,
    const TDesC& aAddress,
    TBool aVerified,
    TBool aValidated,
    const MVPbkContactLink* aContactLink )
    {
    __ASSERT_DEBUG( aEntryNumber >= 0 && 
                    aEntryNumber <= iRichText->ParagraphCount(),
                    Panic( EMsgInvalidEntryNumber ) );

    TChar semicolon = KSemicolon;
    TInt entryLength = 0;
    TInt numOfEntries = iRichText->ParagraphCount();

    TInt pos = TextLength();
    TChar ch = CharAtPos( pos - 1 );

    if ( ch == CEditableText::EParagraphDelimiter )
        {
        // last character is parag.delim. => decrease entry num.
        aEntryNumber--;
        }
    
    /*
     * Fix the split contact name issue::ECYU-7FPC93.
     * Replace any enter-key characters with space characters.
     */

    TBufC<KMaxContactLength> parsedName(aName.Left(KMaxContactLength));
    
    TInt enterKeyPos;
    TChar enterChar( CEditableText::EParagraphDelimiter );
    TChar spaceChar( CEditableText::ESpace );
    TPtr namePtr = parsedName.Des();
    TInt nameLength = parsedName.Length();
    
    enterKeyPos = namePtr.Locate(enterChar);
    while( enterKeyPos != KErrNotFound && enterKeyPos < nameLength )
        {
        namePtr[enterKeyPos] = spaceChar;
        enterKeyPos = namePtr.Locate(enterChar);
        }

    if ( aEntryNumber >= numOfEntries )
        {
        // entry appended to the end.
        iRichText->CancelInsertCharFormat();
        iRichText->InsertL( pos, CEditableText::EParagraphDelimiter );
        }
    else
        {
        // entry appended to the middle or beginning.
        iRichText->CharPosOfParagraph( entryLength, aEntryNumber );
        
        if ( entryLength - 1 )
            {
            InsertEntryBreakL( aEntryNumber, 0 );
            }
        }

    TInt entryStart = iRichText->CharPosOfParagraph( entryLength, aEntryNumber );
    
    //ECYU-7FPC93:: Fix
    HBufC* string = CreateEntryStringLC( parsedName, aAddress, aVerified );
    //HBufC* string = CreateEntryStringLC( aName, aAddress, aVerified );

    if ( aVerified || aValidated )
        {
        // Setting up the field factory.
        iRichText->SetFieldFactory( iAddressControlEditorFieldFactory );
        
        CMsgAddressControlEditorField* field = 
            static_cast<CMsgAddressControlEditorField*>( 
                                        iRichText->NewTextFieldL( KMsgAddressControlEditorFieldUid ) );

        if ( field )
            {
            field->SetEntryStringL( *string );
            
            //ECYU-7FPC93:: Fix
            field->SetNameL( parsedName );
            //field->SetNameL( aName );
           
            field->SetAddressL( aAddress );
            field->SetVerified( aVerified );
            field->SetValidated( aValidated );
            field->SetContactLink( aContactLink );

            iRichText->InsertFieldL( entryStart, 
                                     field, 
                                     KMsgAddressControlEditorFieldUid );
                                     
            iRichText->UpdateFieldL( entryStart );
            }
        }
    else
        {
        if ( string->Length() > 0 )
            {
            iRichText->InsertL( entryStart, *string );
            }
        else
            {
            // This should not happen. Zero lenght contact name is being added. 
            // Insert semicolon only.
            if ( iAvkonEnv->SettingCache().InputLanguage() == ELangArabic )
                {
                semicolon = KArabicSemicolon;
                }
                
            iRichText->InsertL( entryStart, semicolon );
            }
        }

    if ( aEntryNumber >= numOfEntries )
        {
        if ( ch != semicolon &&
             ch != CEditableText::EParagraphDelimiter &&
             ch != KArabicSemicolon )
            {
            if ( iAvkonEnv->SettingCache().InputLanguage() == ELangArabic )
                {
                semicolon = KArabicSemicolon;
                }
                
            iRichText->InsertL( pos, semicolon );
            }
        }
        
    CleanupStack::PopAndDestroy();  // string
    }
    
// ---------------------------------------------------------
// CMsgAddressControlEditor::InsertEntryBreakL
//
// Inserts the paragraph delimiter at aPos on entry aEntryNumber.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::InsertEntryBreakL( TInt aEntryNumber, TInt aPos )
    {
    __ASSERT_DEBUG( aEntryNumber >= 0 &&
                    aEntryNumber < iRichText->ParagraphCount(),
                    Panic( EMsgInvalidEntryNumber ) );

    TInt entryLength = 0;
    TInt insertPos = iRichText->CharPosOfParagraph( entryLength, aEntryNumber );
    insertPos += aPos;
    iRichText->CancelInsertCharFormat();
    iRichText->InsertL( insertPos, CEditableText::EParagraphDelimiter );
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::EntryNumber
//
// Returs an entry number where aPos is on.
// ---------------------------------------------------------
//
TInt CMsgAddressControlEditor::EntryNumber( TInt aPos ) const
    {
    __ASSERT_DEBUG( ( aPos >= 0 ) && ( aPos <= TextLength() ),
        Panic( EMsgDocPosOutOfRange ) );

    // first paragraph==0
    TInt paragraphNumber = iRichText->ParagraphNumberForPos( aPos );
    return paragraphNumber;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::EntryLength
//
// Returns length of the entry aEntryNumber. Does not count paragraph delimiter.
// ---------------------------------------------------------
//
TInt CMsgAddressControlEditor::EntryLength( TInt aEntryNumber ) const
    {
    __ASSERT_DEBUG( ( aEntryNumber >= 0 ) &&
        ( aEntryNumber < iRichText->ParagraphCount() ),
        Panic( EMsgInvalidEntryNumber ) );

    TInt entryLength = 0;
    iRichText->CharPosOfParagraph( entryLength, aEntryNumber );
    return entryLength - 1;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::EntryStartPos
//
// Returns first character position on the entry aEntryNumber.
// ---------------------------------------------------------
//
TInt CMsgAddressControlEditor::EntryStartPos( TInt aEntryNumber ) const
    {
    __ASSERT_DEBUG( ( aEntryNumber >= 0 ) &&
        ( aEntryNumber < iRichText->ParagraphCount() ),
        Panic( EMsgInvalidEntryNumber ) );

    TInt entryLength = 0;
    return iRichText->CharPosOfParagraph( entryLength, aEntryNumber );
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::FirstFieldPos
//
//
// ---------------------------------------------------------
//
TInt CMsgAddressControlEditor::FirstFieldPos( TInt aCursorPos ) const
    {
    TFindFieldInfo fieldInfo;

    /*TBool ret =*/ iRichText->FindFields( fieldInfo, aCursorPos, 0 );

    return fieldInfo.iFirstFieldPos;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::MoveCursorToEntryPosL
//
// Moves the cursor on an entry aEntryNumber to a position aPos. 0 == entry start.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::MoveCursorToEntryPosL( TInt aEntryNumber, TInt aPos )
    {
    __ASSERT_DEBUG( ( aEntryNumber >= 0 ) &&
        ( aEntryNumber < iRichText->ParagraphCount() ),
        Panic( EMsgInvalidEntryNumber ) );

    TInt entryLength = 0;
    TInt entryStart = iRichText->CharPosOfParagraph( entryLength, aEntryNumber );
    if ( entryLength - 1 < aPos )
        {
        aPos = entryLength - 1; // adjust the aPos if it happens to be too big
        }
    SetCursorPosAndCancelSelectionL( entryStart + aPos );
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::IsPosAtEntryBeginning
//
// Checks whether the cursor is at the beginning of the entry and returns ETrue
// if it is, otherwise EFalse.
// ---------------------------------------------------------
//
TBool CMsgAddressControlEditor::IsPosAtEntryBeginning( TInt aPos ) const
    {
    __ASSERT_DEBUG( ( aPos >= 0 ) && ( aPos <= TextLength() ),
        Panic( EMsgDocPosOutOfRange ) );

    TInt entryLength = 0;
    return ( iRichText->CharPosOfParagraph(
        entryLength, EntryNumber( aPos ) ) == aPos );
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::IsEntryVerified
//
// Check whether an entry aEntryNumber is verified or not and returns ETrue
// if it is, otherwise EFalse.
// ---------------------------------------------------------
//
TBool CMsgAddressControlEditor::IsEntryVerified( TInt aEntryNumber ) const
    {
    const CMsgAddressControlEditorField* field = Field( aEntryNumber );

    return ( field ? field->IsVerified() : EFalse );
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::IsPriorEntryVerified
//
// Checks whether an entry aEntryNumber-1 is verified and returns ETrue if
// it is, otherwise EFalse.
// ---------------------------------------------------------
//
TBool CMsgAddressControlEditor::IsPriorEntryVerified( TInt aEntryNumber ) const
    {
    __ASSERT_DEBUG( ( aEntryNumber >= 0 ) &&
        ( aEntryNumber < iRichText->ParagraphCount() ),
        Panic( EMsgInvalidEntryNumber ) );

    if ( aEntryNumber > 0 )
        {
        return IsEntryVerified( aEntryNumber - 1 );
        }

    return EFalse;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::IsEntryValidated
//
// Check whether an entry aEntryNumber is validated or not and returns ETrue
// if it is, otherwise EFalse.
// ---------------------------------------------------------
//
TBool CMsgAddressControlEditor::IsEntryValidated( TInt aEntryNumber ) const
    {
    const CMsgAddressControlEditorField* field = Field( aEntryNumber );

    return ( field ? field->IsValidated() : EFalse );
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::IsFieldOnLeft
//
// Checks if there is a text field on the left hand side of the cursor at
// position aPos and returns ETrue if it is, otherwise EFalse.
// ---------------------------------------------------------
//
TBool CMsgAddressControlEditor::IsFieldOnLeft( TInt aPos ) const
    {
    if ( aPos > 0 && aPos <= TextLength() )
        {
        const CMsgAddressControlEditorField* field = FieldFromPos( aPos - 1 );

        return ( field ? field->IsVerified() : EFalse );
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::IsFieldOnRight
//
// Checks if there is a text field on the right hand side of the cursor at position
// aPos and returns ETrue if it is, otherwise EFalse.
// ---------------------------------------------------------
//
TBool CMsgAddressControlEditor::IsFieldOnRight( TInt aPos ) const
    {
    if ( aPos >= 0 && aPos < TextLength() )
        {
        // Fixing Fixing "ESPI-63C45M - Messags: The contact named with one
        // characteronly cannot be deleted from ‘To’ field by ‘Clear’ key."

        const CMsgAddressControlEditorField* field = FieldFromPos( aPos );

        return ( field ? field->IsVerified() : EFalse );
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::IsFieldOnPos
//
// Checks if there is a text field under the cursor at position
// aPos and returns ETrue if it is, otherwise EFalse.
// ---------------------------------------------------------
//
TBool CMsgAddressControlEditor::IsFieldOnPos( TInt aPos ) const
    {
    if ( aPos >= 0 && aPos < TextLength() )
        {
        const CMsgAddressControlEditorField* field = FieldFromPos( aPos );

        return ( field ? field->IsVerified() : EFalse );
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::IsEntryEmpty
//
// Checks whether an entry aEntryNumber is empty.
// ---------------------------------------------------------
//
TBool CMsgAddressControlEditor::IsEntryEmpty( TInt aEntryNumber ) const
    {
    __ASSERT_DEBUG( ( aEntryNumber >= 0 ) &&
        ( aEntryNumber < iRichText->ParagraphCount() ),
        Panic( EMsgInvalidEntryNumber ) );

    TInt entryLength = 0;
    iRichText->CharPosOfParagraph( entryLength, aEntryNumber );

    return ( entryLength == 1 );
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::IsEntryRightToLeft
//
// ---------------------------------------------------------
//
TBool CMsgAddressControlEditor::IsEntryRightToLeft( TInt aEntryNumber ) const
    {
    TBool isRightToLeft( EFalse );

    const CMsgAddressControlEditorField* field = Field( aEntryNumber );
    if ( field )
        {
        TPtrC entryText = ReadEntry( aEntryNumber );
        if ( entryText.Length() > 0 )
            {
            if ( TBidiText::TextDirectionality( entryText ) ==
                TBidiText::ERightToLeft ) 
                {
                isRightToLeft = ETrue;
                }
            }
        } 
    return isRightToLeft;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::CheckHighlightingL
//
// Turns entry as highlighted if the cursor is on the top of the verified
// entry, otherwise turns highlighting off if there were any somewhere else.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::CheckHighlightingL( TBool aCancelFep /*= ETrue*/ )
    {
    if ( !iTextView )
        {
        return ;
        }

    if ( aCancelFep && IsFocused())
        {
        CancelFepTransaction();
        }

    TInt cursorPos = CursorPos();

    if ( !IsReadOnly() && IsFieldOnPos( cursorPos ) )
        {
        TFindFieldInfo fieldInfo;

        if ( iRichText->FindFields( fieldInfo, cursorPos, 0 ) )
            {
            TCharFormat charFormat;
            TCharFormatMask applyMask;
            
            if ( iHighLightedFieldPos != fieldInfo.iFirstFieldPos )
                {
                if ( !( iFlags & EMsgControlModeShiftPressed ) )
                    {
                    TRgb highlightColor;
                    
                    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), 
                            highlightColor, 
                            KAknsIIDQsnHighlightColors,
                            EAknsCIQsnHighlightColorsCG1 );

                    charFormat.iFontPresentation.iHighlightColor = highlightColor;
                    applyMask.SetAttrib( EAttFontHighlightColor );

                    charFormat.iFontPresentation.iHighlightStyle =
                        TFontPresentation::EFontHighlightNormal;
                    applyMask.SetAttrib( EAttFontHighlightStyle );
                    
                    // Sets only the attributes specified to be set on mask.
                    iRichText->ApplyCharFormatL(
                        charFormat,
                        applyMask,
                        fieldInfo.iFirstFieldPos,
                        fieldInfo.iFirstFieldLen );
                    }

                if ( iTextView )
                    { 
                    iTextView->HandleRangeFormatChangeL(
                        TCursorSelection(
                            fieldInfo.iFirstFieldPos,
                            fieldInfo.iFirstFieldPos + fieldInfo.iFirstFieldLen ),
                        EFalse );
                    }

                if ( cursorPos != fieldInfo.iFirstFieldPos && 
                     !iHandlingPointerEvent )
                    {
                    MoveCursorToEntryPosL(EntryNumber( cursorPos ), 0 );
                    }

                TurnHighlightingOffL( aCancelFep );
                iHighLightedFieldPos = fieldInfo.iFirstFieldPos;
                }
            }
        }
    else
        {
        if ( IsReadOnly() && iValidHighlightable )
			{
            // Use "link highlight"
			SetHighlightStyleL( EEikEdwinHighlightLink );
            SelectAllL();
            }
		else
            {
            TurnHighlightingOffL( aCancelFep );
            if ( IsReadOnly() )
                {
                iTextView->SetDocPosL( cursorPos, ETrue );                    
                }
            }
        }
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::TurnHighlightingOffL
//
// Turns highlighting off if it happens to be on aSelection.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::TurnHighlightingOffL( TCursorSelection aSelection )
    {
    if ( !Selection().Length() ||
        ( iHighLightedFieldPos == KMsgHighlightedFieldPosNone ) )
        {
        return;
        }

    if ( IsFieldOnPos( iHighLightedFieldPos ) )
        {
        CancelFepTransaction();

        TInt selLowerPos = aSelection.LowerPos();
        // The higherPos is more close to the end of the text
        TInt selHigherPos = aSelection.HigherPos();

        TFindFieldInfo fieldInfo;
        iRichText->FindFields( fieldInfo, iHighLightedFieldPos, 0 );

        if ( ( selLowerPos >= iHighLightedFieldPos ) &&
            ( iHighLightedFieldPos + fieldInfo.iFirstFieldLen <= selHigherPos ) )
            {
            TurnHighlightingOffL();
            }
        }
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::TurnHighlightingOffL
//
// Turns highlighting off if such a highlighted text field is somewhere.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::TurnHighlightingOffL( TBool aCancelFep /*= ETrue*/ )
    {
    TInt textLength = TextLength();

    if ( ( iHighLightedFieldPos == KMsgHighlightedFieldPosNone ) ||
        ( iHighLightedFieldPos > textLength ) )
        {
        return;
        }

    if ( aCancelFep )
        {
        CancelFepTransaction();
        }

    TInt cursorPos = CursorPos();

    if ( cursorPos == textLength )
        {
        if ( IsFieldOnPos( cursorPos - 1 ) )
            {
            // Remove the highlight from last cursor position only if
            // there is highlightable field. 
            TurnHighlightingOffFromPosL( cursorPos - 1 );
            }
        }

    TurnHighlightingOffFromPosL( iHighLightedFieldPos );
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::TurnHighlightingOffFromPosL
//
// Turns highlighting off from position aPos.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::TurnHighlightingOffFromPosL( TInt aPos )
    {
    TInt textLength = TextLength();

    if ( iHighLightedFieldPos == KMsgHighlightedFieldPosNone ||
         aPos < 0 ||        
         aPos > textLength )
        {
        return;
        }

    TFindFieldInfo fieldInfo;

    if ( !iRichText->FindFields( fieldInfo, aPos, 0 ) )
        {
        // iHighLightedFieldPos is not updated correctly and hence, clear whole text
        fieldInfo.iFirstFieldPos = 0;
        fieldInfo.iFirstFieldLen = textLength;
        __ASSERT_DEBUG( EFalse, Panic( EMsgInvalidStartPos ) );
        }
    
    TCharFormat charFormat;
    TCharFormatMask applyMask;    

    // background color.
    charFormat.iFontPresentation.iHighlightColor = AKN_LAF_COLOR( 0 );
    applyMask.SetAttrib( EAttFontHighlightColor );
    
    charFormat.iFontPresentation.iHighlightStyle = TFontPresentation::EFontHighlightNone;
    applyMask.SetAttrib( EAttFontHighlightStyle );
    
    // Sets only the attributes specified to be set on mask.
    iRichText->ApplyCharFormatL( charFormat,
                                 applyMask,
                                 fieldInfo.iFirstFieldPos,
                                 fieldInfo.iFirstFieldLen );

    if ( iTextView )
        {
        TCursorSelection selection( fieldInfo.iFirstFieldPos,
                                    fieldInfo.iFirstFieldPos + fieldInfo.iFirstFieldLen );
        iTextView->HandleRangeFormatChangeL( selection, EFalse );
        }
    
    if ( iHighLightedFieldPos == aPos )
        {
        // If highlight was removed from currently highlighted position 
        // reset highlighted field position.
        iHighLightedFieldPos = KMsgHighlightedFieldPosNone;
        }
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::CreateEntryStringLC
//
// Creates a suitable text string for the text field using strings aName and
// aAddress and return pointer to it. If aFormat == EFalse, this function does
// not format the string but takes only aAddess. If text string does not fit to
// view it clips it and adds ... to end of it. If text layout does not exist
// this function does not attempt to clip text string.
// ---------------------------------------------------------
//

HBufC* CMsgAddressControlEditor::CreateEntryStringLC(
    const TDesC& aName,
    const TDesC& aAddress,
    TBool aVerified /*= ETrue*/ )
    {
    HBufC* addressCopy = aAddress.AllocLC();
    TPtr addrTPtr = addressCopy->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( addrTPtr );
    
    if ( aVerified || IsReadOnly() )
        {
        TInt len = aName.Length();
        if ( len == 0 )
            {
            len = addrTPtr.Length();
            }

        // +2 for KZeroWidthJoiner and direction marker
        HBufC* string = HBufC::NewLC( len + 2 );
        TPtr strPtr = string->Des();

        if ( aName.Length() == 0 )
            {
            strPtr.Append( addrTPtr );
            }
        else
            {
            strPtr.Append( aName );
            }

        TInt maxWidth = 0;

        if ( iTextView )
            {
            // We must have iTextView because LayoutWidth calls CursorPos.
            maxWidth = LayoutWidth();
            }

        if ( maxWidth > 0 )
            {
            const CFont* font = TextFont();

            if ( strPtr.Length() )
                {
                // Clip a little bit more than maxWidth to prevent wrapping.
                TInt clipWidth = Max( 0, maxWidth - 4 );
                TInt ellipsisWidth = font->CharWidthInPixels( KEllipsis );

                TInt fits = font->TextCount( strPtr, clipWidth );

                if ( fits < strPtr.Length() )
                    {
                    fits = font->TextCount( strPtr, clipWidth - ellipsisWidth );
                    
                    // Work out the text directionality before the truncation point.
                    // First put the text in reverse order and then call
                    // TBidiText::TextDirectionality. This effectively tells the
                    // directionality of the last strongly directional character
                    // in the text.
                    HBufC* temp = strPtr.Left( fits ).AllocL();
                    TPtr tempPtr = temp->Des();

                    TInt i = 0;
                    TInt j = tempPtr.Length() - 1;
                    while ( i < j )
                        {
                        TText t = tempPtr[i];
                        tempPtr[i++] = tempPtr[j];
                        tempPtr[j--] = t;
                        }

                    TBidiText::TDirectionality dir = 
                        TBidiText::TextDirectionality( tempPtr );

                    delete temp;

                    TText dirMarker = ( dir == TBidiText::ERightToLeft )
                        ? KRLMarker
                        : KLRMarker;

                    // Insert zero width joiner if necessary to get the correct glyph form
                    // before truncation.

                    TText lastChar = strPtr[fits - 1];
                    TText next = strPtr[fits];
                    
                    TInt ellipsisPos = fits;
                    
                    if ( CFont::CharactersJoin( lastChar, KZeroWidthJoiner ) &&
                         CFont::CharactersJoin( lastChar, next ) )
                        {
                        strPtr[fits] = KZeroWidthJoiner;
                        ellipsisPos++;
                        }
                    strPtr.SetMax();
                    strPtr[ellipsisPos] = KEllipsis;
                    // Inserting direction marker after the ellipsis ensures that
                    // the ellipsis is shown on the correct side of the text
                    // before it.
                    strPtr[ellipsisPos + 1] = dirMarker;

                    strPtr.SetLength( ellipsisPos + 2 );
                    }
                }
            }

        HBufC* newString = strPtr.AllocL();
        CleanupStack::PopAndDestroy( 2 );  // string, addressCopy

        CleanupStack::PushL(newString);
        return newString;
        }
    else
        {
        HBufC* string = addrTPtr.AllocL();
        CleanupStack::PopAndDestroy();  // addressCopy
        CleanupStack::PushL( string );
        return string;
        }
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::ReBuildEntryStringsL
//
// Re-builds all the text strings. Goes through all the fields in the
// text and assigns a new string into them. After this we call UpdateAllFieldsL 
// to update all the fields and after this restores to cursor position
// to old position if it is valid or to the end of the text.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::ReBuildEntryStringsL()
    {
    const TInt textLength = TextLength();
    TInt length = textLength;
    TInt docPos = 0;
    TFindFieldInfo fieldInfo;

    iCurrentEntry = EntryNumber( CursorPos() );

    while ( length > 0 &&
            docPos < textLength &&
            iRichText->FindFields( fieldInfo, docPos, length ) )
        {
        CMsgAddressControlEditorField* field =
            const_cast<CMsgAddressControlEditorField*>( FieldFromPos( fieldInfo.iFirstFieldPos ) );

        HBufC* string = CreateEntryStringLC( *field->Name(), 
                                             *field->Address(), 
                                             field->IsVerified() );

        field->SetEntryStringL( *string );

        CleanupStack::PopAndDestroy( string );

        if ( fieldInfo.iFieldCountInRange == 0 ) // no more fields left within a range
            {
            break;
            }

        docPos = fieldInfo.iFirstFieldPos + fieldInfo.iFirstFieldLen;
        length = textLength - docPos;
        }
    
    if ( IsFocused() )
        {
        CancelFepTransaction();
        }
    
    TInt oldCursorPos( CursorPos() );

    // Sets cursor pos always to beginning
    UpdateAllFieldsL();
    
    oldCursorPos = Min( oldCursorPos, TextLength() );
    
    // Restore the original cursor position if it was
    // not out of the limit.
    SetCursorPosL( oldCursorPos, EFalse );
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::Field
//
// Returns a field of the entry aEntryNumber. If there is no field on that entry,
// returns NULL.
// ---------------------------------------------------------
//
const CMsgAddressControlEditorField* CMsgAddressControlEditor::Field(
    TInt aEntryNumber ) const
    {
    __ASSERT_DEBUG( ( aEntryNumber >= 0 ) &&
        ( aEntryNumber < iRichText->ParagraphCount() ),
        Panic( EMsgInvalidEntryNumber ) );

    TInt entryLength = 0;
    TInt entryStart = iRichText->CharPosOfParagraph( entryLength, aEntryNumber );

    return FieldFromPos( entryStart );
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::CharAtPos
//
//
// ---------------------------------------------------------
//
TChar CMsgAddressControlEditor::CharAtPos( TInt aPos ) const
    {
    if ( aPos >= 0 && aPos < TextLength() )
        {
        TBuf<1> buf;
        iText->Extract( buf, aPos, 1 );
        return buf[0];
        }
    else
        {
        return 0;
        }
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::PrepareForViewing
//
// Prepares control for viewing.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::PrepareForViewing()
    {
    if ( iCurrentEntry >= 0 )
        {
        TRAP_IGNORE( MoveCursorToEntryPosL(
            iCurrentEntry, EntryLength( iCurrentEntry ) ) );

        iCurrentEntry = KMsgCurrentEntryNone;
        }
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::PreInsertEditorFormattingL
//
//
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::PreInsertEditorFormattingL( const TKeyEvent& aKeyEvent )
    {
    if ( IsFocused() && !IsReadOnly() )
        {    
        TBool modified = EFalse;
        TInt editPos = CursorPos();
        
        // No preinserting is done in a case edit position is over
        // text length (should not be possible) or if delete key is pressed.
        if ( editPos > TextLength() ||
             aKeyEvent.iCode == EKeyDelete )
            {
            return;
            }
        
        TInt entryNumber = EntryNumber( editPos );
        TInt startPos = FirstFieldPos( editPos );
        TInt endPos = startPos + EntryLength( entryNumber );
        
        if ( startPos < editPos && editPos < endPos && 
             IsEntryVerified( entryNumber ) )
            {
            // Cursor is in the middle of verified entry, se inserting not possible.
            return;
            }
            
        TChar currentChar = CharAtPos( editPos );
        TChar previousChar = CharAtPos( editPos - 1 );
        
        if ( IsFieldOnLeft( editPos + 1 ) ) // looking on the right, but before the key is inserted
            {
            // user is trying to add character to left of field:
            // insert paragraph delimiter in front of the field,
            // and in front of that a semicolon.
            iTextView->SetDocPosL( editPos, EFalse );
            iRichText->CancelInsertCharFormat();
            InsertParagraphL( editPos );
            InsertSemicolonL( editPos );
            modified = ETrue;
            }
        else if ( IsFieldOnLeft( editPos ) )
            {
            // user is trying to add character to right of field:
            // insert semicolon after the field if there isn't one already,
            // and after that a paragraph delimiter.
            iTextView->SetDocPosL( editPos, EFalse );
            iRichText->CancelInsertCharFormat();
            if ( currentChar != KSemicolon && currentChar != KArabicSemicolon )
                {
                InsertSemicolonL( editPos );
                }
            InsertParagraphL( ++editPos );
            editPos++;
            modified = ETrue;
            }
        else if ( ( previousChar == KSemicolon || 
                    previousChar == KArabicSemicolon  ) &&
                  IsFieldOnLeft( editPos - 1 ) )
            {
            // user is trying to add character to right of semicolon
            // and there is field right next to it:
            // insert a paragraph delimiter after semicolon.
            iTextView->SetDocPosL( editPos, EFalse );
            iRichText->CancelInsertCharFormat();
            InsertParagraphL( editPos );
            editPos++;
            modified = ETrue;
            }
            
        if ( modified )
            {
            CheckHighlightingL( EFalse );

            TInt len = TextLength();
            if ( editPos > len )
                {
                editPos = len;
                }

            iTextView->SetDocPosL( editPos, EFalse );

            TViewYPosQualifier yPosQualifier;
            yPosQualifier.SetFillScreen();
            yPosQualifier.SetMakeLineFullyVisible();
            iTextView->HandleGlobalChangeL( yPosQualifier );

            iHighLightedFieldPos = KMsgHighlightedFieldPosNone;
            }
        }
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::PostInsertEditorFormattingL
//
//
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::PostInsertEditorFormattingL( TBool aDuplicateEvent )
    {
    if ( IsFocused() && !IsReadOnly() )
        {
        TInt editPos = CursorPos();
        if ( editPos > TextLength() )
            {
            return;
            }
        TChar prevChar = CharAtPos( editPos - 1 );

        if ( !aDuplicateEvent && 
             ( prevChar == KSemicolon || prevChar == KArabicSemicolon  ) )
            {
            // user is at the end of a line, after a semicolon:
            // paragraph delimiter after semicolon.
            iTextView->SetDocPosL( editPos, EFalse );
            iRichText->CancelInsertCharFormat();
            InsertParagraphL( editPos++ );
                
            TInt len = TextLength();
            if ( editPos > len )
                {
                editPos = len;
                }

            iTextView->SetDocPosL( editPos, EFalse );

            TViewYPosQualifier yPosQualifier;
            yPosQualifier.SetFillScreen();
            yPosQualifier.SetMakeLineFullyVisible();
            iTextView->HandleGlobalChangeL( yPosQualifier );

            iHighLightedFieldPos = KMsgHighlightedFieldPosNone;
            }
        }
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::CheckEditPermission
//
// Does edit permission check and returns its result.
// ---------------------------------------------------------
//
TUint32 CMsgAddressControlEditor::CheckEditPermission() const
    {
    TUint32 editPermission = CMsgBaseControl::EMsgEditNone;
    TInt textLength = TextLength();
    TCursorSelection selection = iTextView->Selection();

    if ( selection.Length() )
        {
        TInt selLowerPos = selection.LowerPos();
        // The higherPos is more close to the end of the text
        TInt selHigherPos = selection.HigherPos();
        TFindFieldInfo fieldInfoLower;
        TFindFieldInfo fieldInfoHigher;
        TBool lowerPosOnField(
            iRichText->FindFields( fieldInfoLower, selLowerPos, 0 ) );
        TBool higherPosOnField(
            iRichText->FindFields( fieldInfoHigher, selHigherPos, 0 ) );

        if ( lowerPosOnField && higherPosOnField )
            {
            // Both the lowerPos and the higherPos are on a field
            if ( IsPosAtEntryBeginning( selLowerPos ) &&
                IsPosAtEntryBeginning( selHigherPos ) )
                {
                // Both positions are at the beginning of the fields
                editPermission |= CMsgAddressControl::EMsgEditParagraphDelimiter
                    | CMsgAddressControl::EMsgEditBackspace
                    | CMsgAddressControl::EMsgEditDelete
                    | CMsgBaseControl::EMsgEditCut;
                }
            }
        else if ( lowerPosOnField )
            {
            if ( IsPosAtEntryBeginning( selLowerPos ) )
                {
                editPermission |= CMsgAddressControl::EMsgEditParagraphDelimiter
                    | CMsgAddressControl::EMsgEditBackspace
                    | CMsgAddressControl::EMsgEditCharInsert
                    | CMsgAddressControl::EMsgEditDelete
                    | CMsgBaseControl::EMsgEditCut
                    | CMsgBaseControl::EMsgEditPaste;
                }
            }
        else if ( higherPosOnField )
            {
            if ( IsPosAtEntryBeginning( selHigherPos ) )
                {
                editPermission |= CMsgAddressControl::EMsgEditParagraphDelimiter;
                }
            }
        else if ( IsFieldOnLeft( selLowerPos ) )
            {
            // Neither position are on a field but the lower pos is
            // just next to a field on the right side
            editPermission |= CMsgAddressControl::EMsgEditParagraphDelimiter;
            }
        else
            {
            // Neither position are on a field
            editPermission |= CMsgAddressControl::EMsgEditCharInsert
                | CMsgAddressControl::EMsgEditParagraphDelimiter
                | CMsgAddressControl::EMsgEditBackspace
                | CMsgAddressControl::EMsgEditDelete
                | CMsgBaseControl::EMsgEditCut
                | CMsgBaseControl::EMsgEditPaste;
            }

        // Copy is always possible
        editPermission |= CMsgBaseControl::EMsgEditCopy;

        // BackspaceMove and RemoveEntry are never possible when selection is on
        editPermission &= ~CMsgAddressControl::EMsgEditBackspaceMove;
        editPermission &= ~CMsgAddressControl::EMsgEditRemoveEntry;

        editPermission |=
            (selHigherPos - selLowerPos) == textLength ?
            CMsgBaseControl::EMsgEditUnSelectAll : 0;
        }
    else
        {
        // Selection is off
        TInt cursorPos = CursorPos();
        TBool posAtEntryBeginning = IsPosAtEntryBeginning( cursorPos );
        TChar ch = CharAtPos( cursorPos - 1 );
        TChar nextCh = CharAtPos( cursorPos );

        if ( ch == CEditableText::EParagraphDelimiter )
            {
            ch = CharAtPos( cursorPos - 2 );
            }

        TInt entryAtPos = EntryNumber( cursorPos );

        if ( IsFieldOnPos( cursorPos ) && !posAtEntryBeginning )
            {
            // The cursor is in a field. No edit actions are allowed.
            editPermission = (TUint32) CMsgBaseControl::EMsgEditNone;
            }
        else if ( IsFieldOnLeft( cursorPos ) && !posAtEntryBeginning )
            {
            // The cursor is not on a field but just next to it on the right
            // side. Only the paragraph delimiter can be inserted or the field
            // can be removed by the backspace.
            editPermission |= CMsgAddressControl::EMsgEditParagraphDelimiter;
            editPermission |= CMsgAddressControl::EMsgEditRemoveEntry;
            editPermission |= CMsgAddressControl::EMsgEditCharInsert;
            // This should check also a situation when next entry is empty
            // and when delete is also possible.
            }
        else if ( ( ch == KSemicolon || ch == KArabicSemicolon 
                || IsPriorEntryVerified( entryAtPos ) ) && posAtEntryBeginning )
            {
            // The cursor is not in a field but just below it at the
            // beginning of the line.
            if ( IsEntryEmpty( entryAtPos ) )
                {
                // Current entry is empty. RemoveEntry and BackspaceMove
                // the edit actions are not allowed.
                editPermission = (TUint32) CMsgBaseControl::EMsgEditAll;
                editPermission &= ~CMsgAddressControl::EMsgEditBackspaceMove;
                editPermission &= ~CMsgAddressControl::EMsgEditRemoveEntry;
                }
            else
                {
                // Pressing the backspace button is allowed but no char
                // removal i.e the BackspaceMove is allowed.
                editPermission = (TUint32) CMsgBaseControl::EMsgEditAll;
                editPermission &= ~CMsgAddressControl::EMsgEditBackspace;
                editPermission &= ~CMsgAddressControl::EMsgEditRemoveEntry;
                }
            }
        else
            {
            // The cursor is somewhere else but in a field or next to it on
            // the right side. All but the RemoveEntry and BackspaceMove the
            // edit actions are allowed.
            editPermission = (TUint32) CMsgBaseControl::EMsgEditAll;
            editPermission &= ~CMsgAddressControl::EMsgEditBackspaceMove;
            editPermission &= ~CMsgAddressControl::EMsgEditRemoveEntry;
            }

        if ( IsFieldOnRight( cursorPos ) )
            {
            // Field immediatelly on right side of curser. Delete and RemoveEntry
            // are allowed.
            editPermission |= CMsgAddressControl::EMsgEditDelete;
            editPermission |= CMsgAddressControl::EMsgEditRemoveEntry;
            }
        else if ( IsFieldOnRight( cursorPos + 1) && 
                  nextCh == CEditableText::EParagraphDelimiter )
            {
            // The cursor is not in a field but just above it at the
            // end of the line.
            if ( IsEntryEmpty( entryAtPos ) )
                {
                // Current entry is empty. RemoveEntry are not allowed.
                editPermission |= CMsgAddressControl::EMsgEditDelete;
                editPermission &= ~CMsgAddressControl::EMsgEditRemoveEntry;
                }
            else
                {
                // Current entry is not empty. Delete is not allowed.
                editPermission &= ~CMsgAddressControl::EMsgEditDelete;
                }
            }

        // Copy is never allowed when selection is off.
        editPermission &= ~CMsgBaseControl::EMsgEditCopy;
        // Cut is never allowed when selection is off.
        editPermission &= ~CMsgBaseControl::EMsgEditCut;
        // UnSelectAll is never allowed when selection is off.
        editPermission &= ~CMsgBaseControl::EMsgEditUnSelectAll;
        }

    editPermission &= ~CMsgBaseControl::EMsgEditUndo; // Undo is never allowed.

    if ( textLength )
        {
        editPermission |= CMsgBaseControl::EMsgEditSelectAll;
        }
    else
        {
        editPermission &= ~CMsgBaseControl::EMsgEditSelectAll;
        }

    if ( IsReadOnly() )
        {
        editPermission &= ~CMsgBaseControl::EMsgEditPaste;
        editPermission &= ~CMsgBaseControl::EMsgEditCut;
        editPermission &= ~CMsgAddressControl::EMsgEditRemoveEntry;
        editPermission &= ~CMsgAddressControl::EMsgEditParagraphDelimiter;
        editPermission &= ~CMsgAddressControl::EMsgEditCharInsert;
        }

    return editPermission;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::ConstructFromResourceL
//
// Creates this control from resource.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::ConstructFromResourceL( TResourceReader& aReader )
    {
    // This used to be font id. Not used anymore. Have to read it, though.
    /*TInt fontId =*/ aReader.ReadInt32();

    ConstructL();

    iAddressControlEditorFieldFactory =
        new ( ELeave ) TMsgAddressControlEditorFieldFactory();

    if ( !IsReadOnly() )
        {
        ReadAknResourceL( aReader );
        }
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::SetAndGetSizeL
//
// Calculates and sets the size of the control and returns new size as
// reference aSize. Sets maximum height for editor to be maximum
// body height. If our height has already been set then uses the current
// one since edwin will decide internally what is the correct height and
// report this through HandleEdwinSizeEventL observer interface. 
// After all this finally set the size to 
// edwin. This might cause reformatting so after this we need to check
// whether we are band formatting and set the size to maximum body height
// if we are since edwin will not report height change through HandleEdwinSizeEventL
// when band formatting is enabled.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::SetAndGetSizeL( TSize& aSize )
    {
    TSize controlSize( aSize );
    
    if ( iSize.iHeight > 0 )
        {
        controlSize.iHeight = iSize.iHeight;
        }

    CMsgExpandableControlEditor::SetAndGetSizeL( aSize );
    
    // Make sure that if we are band formatting editor size is set
    // to maximum body height. Will not do any harm resetting this
    // even if we already are at maximum body height.
    if ( TextLayout() && TextLayout()->IsFormattingBand() )
        {
        controlSize.iHeight = MaximumHeight();
        SetSizeWithoutNotification( controlSize );
        }
    
    aSize = iSize;
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::OfferKeyEventL
//
// Handles key event.
// ---------------------------------------------------------
//
TKeyResponse CMsgAddressControlEditor::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if ( IsReadOnly() && 
        aKeyEvent.iCode == EKeyUpArrow &&
        iValidHighlightable )
        {
        return EKeyWasConsumed;
        }
    return CMsgExpandableControlEditor::OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::HandlePointerEventL
//
// Handles pointer events. Sets handling pointer event flag on during
// edwin pointer event handling so that callbacks from edwin can
// use this information. 
// In readonly mode:
// On button down event when automatic find is
// activated stores the previously focused field if there is one. 
// On button up event checks whether same field is still focused. 
// If user has tapped already focused field again selection key menu
// is launched by simulating key event into it.
// In non-readonly mode:
// If user taps in the middle of validated recipient field cursor
// is moved to the beginning of that field if it is not already located
// there. Also highlight is checked.
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CMsgAddressControlEditor::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    iHandlingPointerEvent = ETrue;
    
    TBool forwardRequest( ETrue );
    
    if ( IsReadOnly() )
        {
        if ( iValidHighlightable && 
             ( aPointerEvent.iType == TPointerEvent::EButton1Down ||
               aPointerEvent.iType == TPointerEvent::EButton1Up ) )
            {
            TPoint tapPoint( aPointerEvent.iPosition );
            TInt docPos( TextView()->XyPosToDocPosL( tapPoint ) );
            
            const CMsgAddressControlEditorField* currentField = FieldFromPos( docPos );
                
            if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
                {
                iPreviousField = currentField;
                            
                if ( iPreviousField &&
                     !iPreviousField->IsVerified() )
                    {
                    iPreviousField = NULL;
                    }
                else
                    {
                    forwardRequest = EFalse;
                    }
                }
            else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
                {
                if ( currentField &&
                     iPreviousField == currentField )
                    {
                    TKeyEvent event;
                    event.iCode = EKeyDevice3;
                    event.iScanCode = EStdKeyDevice3;
                    event.iModifiers = 0;
                    event.iRepeats = 0;
                    
                    iCoeEnv->WsSession().SimulateKeyEvent( event );
                    
                    forwardRequest = EFalse;
                    }
                }
            }
        }
    else
        {
        if ( aPointerEvent.iType == TPointerEvent::EButton1Down ||
             aPointerEvent.iType == TPointerEvent::EButton1Up )
            {
            TPoint tapPoint( aPointerEvent.iPosition );
            TInt docPos( TextView()->XyPosToDocPosL( tapPoint ) );
            
            const CMsgAddressControlEditorField* currentField = FieldFromPos( docPos );
            
            if ( currentField &&
                 currentField->IsVerified() )
                {
              //  forwardRequest = EFalse;
                TInt entryStartPos = EntryStartPos( EntryNumber( docPos ) );
                if ( entryStartPos != CursorPos() )
                    {
                    SetCursorPosL( entryStartPos, EFalse );
                    CheckHighlightingL();
                    }                    
                }
            }
        }
        
    if ( forwardRequest )
        {
        CMsgExpandableControlEditor::HandlePointerEventL( aPointerEvent );
        }
#ifdef RD_TACTILE_FEEDBACK         
    else if(aPointerEvent.iType == TPointerEvent::EButton1Down)
        {                                
        MTouchFeedback* feedback = MTouchFeedback::Instance();
        if ( feedback )
            {
            feedback->InstantFeedback( this, ETouchFeedbackBasic );
            }
        }
#endif        
            
    iHandlingPointerEvent = EFalse;
    }
#else
void CMsgAddressControlEditor::HandlePointerEventL( const TPointerEvent& /*aPointerEvent*/ )
    {
    }
#endif // RD_SCALABLE_UI_V2

// ---------------------------------------------------------
// CMsgAddressControlEditor::HandleResourceChange
//
// During layout switching if editor contains text highlight is
// turned off and entry string are rebuild as width of the editor
// might have changed. Also underlining is checked.
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::HandleResourceChange( TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        CMsgExpandableControlEditor::HandleResourceChange( aType );
    
	    //SKIM-7NKC4B
        /*if ( TextLength() > 0 )
            {
            TRAP_IGNORE( 
                {
                TurnHighlightingOffL();
                ReBuildEntryStringsL();
            
                CheckUnderliningL();
                
                if ( IsFocused() )
                    {
                    CheckHighlightingL( EFalse );
                    }
                } );
            }
	    */
        }
    else
        {
        CMsgExpandableControlEditor::HandleResourceChange( aType );
        }
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::InsertParagraphL
//
//
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::InsertParagraphL( TInt aPos )
    {
    iRichText->InsertL( aPos, CEditableText::EParagraphDelimiter );
    iTextView->HandleCharEditL( CTextLayout::EFParagraphDelimiter );
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::InsertSemicolonL
//
//
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::InsertSemicolonL( TInt aPos )
    {
    TInt langcode = CAknEnv::Static()->SettingCache().InputLanguage();
    TChar semicolon = ( langcode == ELangArabic )
        ? KArabicSemicolon
        : KSemicolon; 

    iRichText->InsertL( aPos, semicolon );
    iTextView->HandleCharEditL( CTextLayout::EFCharacterInsert );
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::CcpuPasteL
//
//
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::CcpuPasteL()
    {
    TInt cursorPos = CursorPos();
    TChar ch = CharAtPos ( cursorPos );

    if ( ch == KSemicolon || ch == KArabicSemicolon )
        {
        iRichText->CancelInsertCharFormat();
        iRichText->InsertL(cursorPos, CEditableText::EParagraphDelimiter);
        iTextView->HandleCharEditL( CTextLayout::EFParagraphDelimiter );
        }

    CMsgExpandableControlEditor::CcpuPasteL();
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::SetAddressFieldAutoHighlight
//
//
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::SetAddressFieldAutoHighlight( TBool aValidHighlightable )
    {
    if ( iValidHighlightable != aValidHighlightable && !aValidHighlightable )
        {
        // Automatic find turned off -> cancel selection
        TRAP_IGNORE( ClearSelectionL() );
        }
    iValidHighlightable = aValidHighlightable;
    
    TRAP_IGNORE( CheckUnderliningL() );
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::CheckUnderliningL
//
//
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::CheckUnderliningL()
    {
    TInt textLength = TextLength();
    TInt length = textLength;
    TInt docPos = 0;
    TFindFieldInfo fieldInfo;
        
    while ( length > 0 &&
            docPos < textLength &&
            iRichText->FindFields( fieldInfo, docPos, length ) )
        {
        TCharFormat currentCharFormat;
        TCharFormatMask currentCharMask;
        
        TCharFormat newCharFormat;
        TCharFormatMask newCharMask;
        
        iRichText->GetCharFormat( currentCharFormat,
                                  currentCharMask,
                                  fieldInfo.iFirstFieldPos,
                                  fieldInfo.iFirstFieldLen );
        
        if ( iValidHighlightable )
            {
            if ( currentCharFormat.iFontPresentation.iUnderline != EUnderlineOn )
                {
                newCharFormat.iFontPresentation.iUnderline = EUnderlineOn;
                newCharMask.SetAttrib( EAttFontUnderline );
                }
            }
        else
            {
            if ( currentCharFormat.iFontPresentation.iUnderline != EUnderlineOff )
                {
                newCharFormat.iFontPresentation.iUnderline = EUnderlineOff;
                newCharMask.SetAttrib( EAttFontUnderline );
                }
            }
        
        if ( newCharMask.AttribIsSet( EAttFontUnderline ) )
            {
            iRichText->ApplyCharFormatL( newCharFormat,
                                         newCharMask,
                                         fieldInfo.iFirstFieldPos,
                                         fieldInfo.iFirstFieldLen );
            }
        
        if ( fieldInfo.iFieldCountInRange == 0 ) // no more fields left within a range
            {
            break;
            }
        
        docPos = fieldInfo.iFirstFieldPos + fieldInfo.iFirstFieldLen;
        length = textLength - docPos;
        }
    }

// ---------------------------------------------------------
// CMsgAddressControlEditor::PrepareForReadOnlyL
// ---------------------------------------------------------
//
void CMsgAddressControlEditor::PrepareForReadOnlyL( TBool aReadOnly )
    {
    if ( IsReadOnly() == aReadOnly )
        {
        return;
        }

    if ( aReadOnly )
        {
#ifdef RD_SCALABLE_UI_V2
        SetHighlightStyleL( EEikEdwinHighlightLink );
#endif   
        
        SetWordWrapL( EFalse );
        if ( AddressFieldAutoHighlight() )
            {
            SelectAllL(); // for automatic highlight
            }
        }
    
    CMsgExpandableControlEditor::PrepareForReadOnlyL( aReadOnly );
    }

//  End of File
