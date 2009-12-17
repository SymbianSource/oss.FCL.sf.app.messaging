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
*     Picture Message model and converter.
*
*/



// INCLUDE FILES
#include "gmsModel.h"

#include <txtrich.h>                //for CRichText
#include <s32file.h>

// LOCAL CONSTANTS AND MACROS

// size constants
const TInt KGmsVersionLength          = 1;
const TInt KGmsItemLengthFieldLength  = 2;
const TInt KGmsItemHeaderLength       = 3;

// tokens
const TUint8 KGmsVersionAsciiZero     = 48; // "0"
const TUint8 KGmsItemTypeLatin1       = 0;  // "0"
const TUint8 KGmsItemTypeUnicode      = 1;  // "1"
const TUint8 KGmsItemTypeOTABitmap    = 2;  // "2"

const TUint8 KGmsLF = 10;
_LIT(KGmsModelDll,"GmsModel.dll");

//  MEMBER FUNCTIONS


EXPORT_C CGmsModel* CGmsModel::NewL(RFs& aFs, TInt aLeaveCodeForCorrupt)
    {
    CGmsModel* self = new (ELeave) CGmsModel(aFs, aLeaveCodeForCorrupt);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CGmsModel::ConstructL()
    {
    }

EXPORT_C CGmsModel::~CGmsModel()
    {
    delete iText;
    delete iOTABitmap;
    }

EXPORT_C void CGmsModel::ImportGMSL(const TDesC8& aInput)
    {
    Reset();
    TInt index = 0;
    CheckVersionL(aInput, index); //this leaves if version is invalid

    TInt length = aInput.Length();
    while (index < length)
        {
        //either text, a picture or something unsupported
        ExtractGMSSubItemL(aInput, index);
        }
    __ASSERT_ALWAYS(HasPicture(), User::Leave(iLeaveCorrupt));
    }

EXPORT_C void CGmsModel::ImportGMSL(const CRichText& aRichText)
    {
    Reset();

    // Create a temporary HBufC16 for extracting from CRichText.
    // (CRichText has no interface for extracting to 8 bit descriptor)
    TInt docLength = aRichText.DocumentLength();
    HBufC16* buf = HBufC16::NewLC(docLength);
    TPtr16 ptr16(buf->Des());
    aRichText.Extract(ptr16, 0, docLength);

    //Create a temporary HBufC8 for importing to converter.
    HBufC8* buf8 = HBufC8::NewLC(docLength);
    TPtr8 ptr8 = buf8->Des();

    //Copy from 16 to 8 bit descriptor.
    ptr8.Copy(ptr16);

    ImportGMSL(*buf8);
    CleanupStack::PopAndDestroy(2); //buf8 buf16
    }

EXPORT_C HBufC8* CGmsModel::ExportGMS8LC()
    {
    // There has to be at least picture data.
    __ASSERT_ALWAYS(HasPicture(), Panic(EGmsMPanicNoPicData1));

    HBufC8* buf = NULL;
    buf = HBufC8::NewLC(TotalSize());
    TPtr8 gms(buf->Des());
    AppendVersionL(gms);        // <version>
    AppendTextSubItemL(gms);    // <item>
    AppendPictureSubItemL(gms); // <item>
    return buf;
    }

EXPORT_C HBufC16* CGmsModel::ExportGMS16LC()
    {
    HBufC16* buf16 = HBufC16::NewLC(TotalSize());
    TPtr ptr16(buf16->Des());
    HBufC8* buf8 = ExportGMS8LC();
    TPtr8 ptr8(buf8->Des());
    ptr16.Copy(ptr8);
    CleanupStack::PopAndDestroy(); //buf8
    return buf16;
    }

EXPORT_C HBufC* CGmsModel::TextAsHBufC16LC() const
    {
    if (iText)
        {
        return iText->AllocLC();
        }
    return KNullDesC().AllocLC();
    }

EXPORT_C void CGmsModel::SetTextL(const TDesC& aSource)
    {
    if (ContainsUnicodeL(aSource))
        {
        SetTextUnicodeL(aSource);
        }
    else
        {
        SetTextLatinL(aSource);
        }
    }

EXPORT_C void CGmsModel::SetTextLatinL(const TDesC& aSource)
    {
    HBufC* text = aSource.AllocL();
    delete iText;
    iText  = text;
    iMode = EGmsModeLatin;
    }

EXPORT_C void CGmsModel::SetTextUnicodeL(const TDesC& aSource)
    {
    HBufC* text = aSource.AllocL();
    delete iText;
    iText  = text;
    iMode = EGmsModeUnicode;
    }

TBool CGmsModel::ContainsUnicodeL(const TDesC& aInputString) const
    {
    TInt length = aInputString.Length();
    const TUint KGmsHighestLatin1 = 255;
    for (TInt n = 0; n < length; n++)
        {
        TUint16 ch = aInputString[n];
        // Latin-1 occupies dec. values 0 to 255 of Unicode. (U+0000..U+00FF)
        if (ch > KGmsHighestLatin1 && ch != CEditableText::EParagraphDelimiter)
            {        
            return ETrue;
            }
        }
    return EFalse;
    }

EXPORT_C void CGmsModel::ExportOTABitmapL(RWriteStream& aOutStream)
    {
    __ASSERT_ALWAYS(HasPicture(), Panic(EGmsMPanicNoPictureToExport));
    aOutStream.WriteL(*iOTABitmap);
    }

/// Assumes an 8 bit data stream
EXPORT_C void CGmsModel::ImportOTABitmapL(RReadStream& aReadStream)
    {
    MStreamBuf* streamBuf = NULL;
    streamBuf = aReadStream.Source();
    User::LeaveIfNull(streamBuf);
    TInt size = streamBuf->SizeL();
    __ASSERT_ALWAYS(size > 0,
        Panic(EGmsMPanicStreamIsEmpty));
    HBufC8* bitmap = HBufC8::NewLC(size);
    TPtr8 bitmapPtr(bitmap->Des());
    aReadStream.ReadL(bitmapPtr, size);
    // This leaves with iLeaveCorrupt if the bitmap is bad.
    HBufC8* buf8 = CheckedAndFixedOtaBitmapL(*bitmap);
    CleanupStack::PopAndDestroy(bitmap);
    delete iOTABitmap;
    iOTABitmap = buf8;
    }

EXPORT_C TInt CGmsModel::SizeOfCompleteMsgExcludingTextAndPic() const
    {
    return (KGmsVersionLength + KGmsItemHeaderLength*2);
    }

EXPORT_C TBool CGmsModel::HasText() const
    {
    if (!iText || iText->Length() <= 0)
        {
        return EFalse;
        }
    // length > 0
    return ETrue;
    }

// This does not verify that the data is valid
EXPORT_C TBool CGmsModel::HasPicture() const
    {
    if (!iOTABitmap || iOTABitmap->Length() <= 0)
        {
        return EFalse;
        }
    // length > 0
    return ETrue;
    }

EXPORT_C TBool CGmsModel::IsTextLatin() const
    {
    return (iMode == EGmsModeLatin);
    }

EXPORT_C TInt CGmsModel::TotalSizeExcludingText() const
    {
    TInt bitmapSize = 0;
    if (iOTABitmap)
        {
        bitmapSize = iOTABitmap->Size();
        }
    return bitmapSize + SizeOfCompleteMsgExcludingTextAndPic();
    }

EXPORT_C TInt CGmsModel::TotalSize() const
    {
    TInt textSize(0);
    
    textSize = Textlength();
    if (iMode == EGmsModeUnicode)
        {
        textSize *= 2; // (two bytes per char)
        }

    return TotalSizeExcludingText() + textSize;
    }

EXPORT_C TInt CGmsModel::Textlength() const
    {
    if (iText)
        {
        return iText->Length();
        }
    return 0;
    }

EXPORT_C TInt CGmsModel::PictureSize() const
    {
    if (iOTABitmap)
        {
        return iOTABitmap->Size();
        }
    return 0;
    }

EXPORT_C void CGmsModel::Reset()
    {
    delete iText;
    iText = NULL;
    delete iOTABitmap;
    iOTABitmap = NULL;
    }

EXPORT_C TInt CGmsModel::_TestingL(TInt /*aCode1*/, TInt& /*aCode2*/, const TDesC8& /*aDes1*/, const TDesC16& /*aDes2*/)
    {
    // Nothing here.
    return 0;
    }

// Deprecated. Do not use.
//
EXPORT_C HBufC* CGmsModel::ExtractTextFromMsgLC(const TDesC& /*aMsg*/, TInt /*aAmount*/)
    {
    return NULL; // for compiler
    }

EXPORT_C TBool CGmsModel::IsPictureValidL()
    {
    // Deprecated
    return ETrue;
    }

TBool CGmsModel::ImportOtaBitmapL(const TDesC& aFileName)
    {
    RFileReadStream readStream;
    User::LeaveIfError(readStream.Open(
        iFs,
        aFileName,
        EFileStream));
    readStream.PushL();
    MStreamBuf* buf = readStream.Source();
    User::LeaveIfNull(buf);
    TInt size = buf->SizeL();
    __ASSERT_ALWAYS(size > 0,
        Panic(EGmsNull1));
    HBufC8* data = HBufC8::NewLC(size);
    TPtr8 dataPtr(data->Des());
    readStream.ReadL(dataPtr, size);
    TBool success;
    HBufC8* buf8 = NULL;
    buf8 = CheckedAndFixedOtaBitmapL(*data, success);
    CleanupStack::PopAndDestroy(data);
    CleanupStack::PopAndDestroy(); //(closes readStream)
    if (success)
        {
        delete iOTABitmap;
        iOTABitmap = buf8;
        return ETrue;
        }
    else
        {
        __ASSERT_DEBUG(!buf8, Panic(EGmsProblem1));
        return EFalse;
        }
    }

// Call this as the first extraction method.
void CGmsModel::CheckVersionL(const TDesC8& aInput, TInt& aIndex)
    {
    __ASSERT_ALWAYS(aInput.Length() >= KGmsVersionLength,
        User::Leave(iLeaveCorrupt));
    // To be a valid msg, its version identifier must be zero
    if (aInput[0] != KGmsVersionAsciiZero)
        {
        User::Leave(iLeaveCorrupt);
        }
    aIndex++;
    }

void CGmsModel::ExtractGMSSubItemL(const TDesC8& aInput, TInt& aIndex)
    {
    ExtractGMSSubItemL(aInput, aIndex, EFalse);
    }

void CGmsModel::ExtractGMSSubItemL(const TDesC8& aInput, TInt& aIndex,
    TBool aSkipBitmap)
    {
    __ASSERT_ALWAYS(aInput.Length() >= aIndex + KGmsItemHeaderLength,
        User::Leave(iLeaveCorrupt));

    //Get GMS item type. It should be one of the following:
    //  "00" <GMS-item-length> <ISO-8859-1-char>*     = LATIN TEXT ITEM
    //  "01" <GMS-item-length> <UNICODE-char>*        = UNICODE TEXT ITEM
    //  "02" <GMS-item-length> <OTA-bitmap>           = PICTURE ITEM

    TBuf8<1> msgItemType;
    msgItemType = aInput.Mid(aIndex, 1); //pos, length
    aIndex += 1;

    if (KGmsItemTypeLatin1 == msgItemType[0])       // "00"
        {
        ExtractLatin1L(aInput, aIndex);
        }
    else if (KGmsItemTypeUnicode == msgItemType[0]) // "01"
        {
        ExtractUnicodeL(aInput, aIndex);
        }
    else if (KGmsItemTypeOTABitmap == msgItemType[0]) // "02"
        {
        if (aSkipBitmap)
            {
            ExtractUnknownL(aInput, aIndex); //just skips over it
            }
        else
            {
            ExtractOTABitmapL(aInput, aIndex);
            }
        }
    else
        {
        ExtractUnknownL(aInput, aIndex); //just skips over the part
        }
    }

void CGmsModel::ExtractLatin1L(const TDesC8& aInput, TInt& aIndex)
    {
    TInt length = ExtractLengthL(aInput, aIndex);

    __ASSERT_ALWAYS(aInput.Length() >= aIndex + length,
        User::Leave(iLeaveCorrupt));

    delete iText;
    iText = NULL;
    iText = HBufC::NewL(length);
    iText->Des().Copy(aInput.Mid(aIndex, length));
    aIndex += length;
    iMode = EGmsModeLatin;
    }

void CGmsModel::ExtractUnicodeL(const TDesC8& aInput, TInt& aIndex)
    {
    TInt length = ExtractLengthL(aInput, aIndex);
    __ASSERT_ALWAYS(aInput.Length() >= aIndex + length,
        User::Leave(iLeaveCorrupt));
    __ASSERT_ALWAYS((length % 2) == 0, User::Leave(iLeaveCorrupt));//even nr.
    delete iText;
    iText = NULL;
    iText = HBufC::NewL(length / 2);

    //
    // The data has to be reassembled from UCS-2 to 16 bit Unicode.
    //
    TPtr ptr(iText->Des());
    for (TInt n = aIndex; n < aIndex + length; n += 2)
        {
        TUint16 lower = aInput[n + 1];
        TUint16 upper = aInput[n];
        upper = TUint16(upper << 8);
        TUint16 ch = 0;
        ch = TUint16(upper | lower);
        ptr.Append(ch);
        }
    aIndex += length;
    iMode = EGmsModeUnicode;
    }

void CGmsModel::ExtractOTABitmapL(const TDesC8& aInput, TInt& aIndex)
    {
    TInt length = ExtractLengthL(aInput, aIndex);
    __ASSERT_ALWAYS(aInput.Length() >= aIndex + length,
        User::Leave(iLeaveCorrupt));
    delete iOTABitmap;
    iOTABitmap = NULL;
    // This leaves with iLeaveCorrupt if the bitmap is bad.
    iOTABitmap = CheckedAndFixedOtaBitmapL(aInput.Mid(aIndex, length));
    aIndex += length;
    }

void CGmsModel::ExtractUnknownL(const TDesC8& aInput, TInt& aIndex)
    {
    TInt length = ExtractLengthL(aInput, aIndex);
    __ASSERT_ALWAYS(aInput.Length() >= aIndex + length,
        User::Leave(iLeaveCorrupt));
    aIndex += length; // Skip the unknown item.
    }

TInt CGmsModel::ExtractLengthL(const TDesC8& aInput, TInt& aIndex)
    {
    __ASSERT_ALWAYS(aInput.Length() >= aIndex + KGmsItemLengthFieldLength,
        User::Leave(iLeaveCorrupt));

    // (KGmsItemLengthFieldLength = 2)
    TBuf8<KGmsItemLengthFieldLength> itemLength;
    itemLength.Copy(aInput.Mid(aIndex, KGmsItemLengthFieldLength));
    aIndex += KGmsItemLengthFieldLength;
    TUint16 length = 0;
    length = TUint16 ((itemLength[0]&0x000000FF));
    length = TUint16 (length<<8);
    length |= TUint16 ((itemLength[1]&0x000000FF));
    return length;
    }

CGmsModel::CGmsModel(RFs& aFs, TInt aLeaveCodeForCorrupt) :
    iFs(aFs),
    iLeaveCorrupt(aLeaveCodeForCorrupt)
    {
    }

void CGmsModel::AppendVersionL(TDes8& aGms)
    {
    TBuf8<1> gmsVersion;
    _LIT(KEmpty1, "0");
    gmsVersion.Copy(KEmpty1);
    gmsVersion[0] = KGmsVersionAsciiZero;
    aGms.Append(gmsVersion);
    }

void CGmsModel::AppendTextSubItemL(TDes8& aGms)
    {
    TBuf8<3> itemHeader;
    _LIT(KEmpty3, "000");
    itemHeader.Copy(KEmpty3);

    if (iMode == EGmsModeLatin)
        {
        itemHeader[0] = KGmsItemTypeLatin1; // entity type identifier.
        }
    else
        {
        // Unicode
        itemHeader[0] = KGmsItemTypeUnicode; // entity type identifier.
        }

    TInt sizeT = 0;
    if (iText)
        {
        sizeT = iText->Length();
        if (iMode == EGmsModeUnicode)
            {
            sizeT *= 2;
            }
        }

    // The text header is added even if there is no text.
    TInt lowerT = sizeT;
    lowerT &= TInt(0x000000FF);

    TUint upperT = sizeT;
    upperT = TInt(upperT >> 8);
    upperT &= TInt(0x000000FF);

    itemHeader[1] = TInt8(upperT); //First byte of <Item-length>
    itemHeader[2] = TInt8(lowerT); //Second byte of <Item-length>

    aGms.Append(itemHeader);
    if (iText)
        {
        if (iMode == EGmsModeLatin)
            {
            AppendLatin1TextL(aGms, *iText);
            }
        else
            {
            // Have to encode Unicode text to UCS2.
            AppendUCS2TextL(aGms, *iText);
            }
        }
    }

void CGmsModel::AppendPictureSubItemL(TDes8& aGms)
    {
    __ASSERT_DEBUG(HasPicture(), Panic(EGmsMPanicNoPicData2));

    TBuf8<3> itemHeader;
    _LIT(KEmpty3, "000");
    itemHeader.Copy(KEmpty3);

    itemHeader[0] = KGmsItemTypeOTABitmap; // entity type identifier.

    TInt sizeB = iOTABitmap->Size();
    TInt lowerB = sizeB;
    lowerB &= TInt(0x000000FF);

    TUint upperB = sizeB;
    upperB = TInt(upperB >> 8);
    upperB &= TInt(0x000000FF);

    itemHeader[1] = TInt8(upperB); //First byte of <Item-length>
    itemHeader[2] = TInt8(lowerB); //Second byte of <Item-length>

    aGms.Append(itemHeader);
    aGms.Append(*iOTABitmap);
    }

void CGmsModel::AppendUCS2TextL(TDes8& aGms, const TDesC& aText)
    {
    TInt length = aText.Length();
    for (TInt n = 0; n < length; n++)
        {
        TUint16 i = aText[n];

        TUint16 lower = i;
        lower &= TUint16(0x000000FF);

        TUint16 upper = i;
        upper = TUint16(upper >> 8);
        upper &= TUint16(0x000000FF);
        TChar ch = upper;
        aGms.Append(ch);
        ch = lower;
        aGms.Append(ch);
        }
    }

void CGmsModel::AppendLatin1TextL(TDes8& aGms, const TDesC& aText)
    {
    TInt length = aText.Length();
    for (TInt n = 0; n < length; n++)
        {
        TUint16 ch = aText[n];
        if (ch == CEditableText::EParagraphDelimiter)
            {
            aGms.Append(KGmsLF);
            }
        else
            {
            aGms.Append(ch);
            }
        }
    }

HBufC8* CGmsModel::CheckedAndFixedOtaBitmapL(const TDesC8& aOta) const
    {
    TBool success;
    HBufC8* buf = CheckedAndFixedOtaBitmapL(aOta, success);
    if (success)
        {
        return buf;
        }
    else
        {
        __ASSERT_DEBUG(!buf, Panic(EGmsProblem2));
        User::Leave(iLeaveCorrupt);
        return NULL; //for compiler
        }
    }

HBufC8* CGmsModel::CheckedAndFixedOtaBitmapL(const TDesC8& aOta, TBool& aSuccess) const
    {
    // From Smart Messaging Specification 3.0.0:
    // <ota-bitmap> ::= <header> <image- data> [<palette>]
    // <header> ::= <infofield> [<extfield>]* <width><height><depth>

    __ASSERT_DEBUG(aOta.Length(), Panic(EGmsNoPic1));
    TInt length = aOta.Length();

    // <header> should be four bytes
    const TInt KHeaderBytes = 4;
    if (length < KHeaderBytes)
        {
        aSuccess = EFalse;
        return NULL;
        }

    // <infofield> byte should be zero, because the following should hold:
    // Concatenation flag = 0 (no)
    // Compression = 0        (no)
    // External palette = 0   (not used)
    // Icon max size fld len. = 0 (8 bit)
    // Number of animated icons = 0
    if (aOta[0] != 0)
        {
        aSuccess = EFalse;
        return NULL;
        }

    // <extfield> not allowed, and it was ruled out by requiring 0 for
    // the concatenation flag of <infofield>.

    TInt width = aOta[1];
    TInt height = aOta[2];
    TInt depth = aOta[3];
    if (!Rng(KGmsMinPictureWidthPixels, width, KGmsMaxPictureWidthPixels)
        || !Rng(KGmsMinPictureHeightPixels, height,KGmsMaxPictureHeightPixels)
        || depth != 1)
        {
        aSuccess = EFalse;
        return NULL;
        }

    // Check that there is the right amount of picture data.
    TInt pixels = width * height;
    TInt bytesNeededForPicData = pixels / 8 + (pixels%8 == 0 ? 0 : 1);
    TInt bytesNeeded = bytesNeededForPicData + KHeaderBytes;
    TInt tooManyBytes = length - bytesNeeded;
    if (tooManyBytes == 0)
        {
        aSuccess = ETrue;
        return aOta.AllocL();
        }
    else if (tooManyBytes < 0)
        {
        // not enough bytes
        aSuccess = EFalse;
        return NULL;
        }
    else
        {
        // tooManyBytes > 0
        // Discard the excess bytes.
        aSuccess = ETrue;
        return aOta.Left(bytesNeeded).AllocL();
        }
    }

void CGmsModel::Panic(TGmsModelPanic aCode)
    {
    User::Panic(KGmsModelDll, aCode);
    }

//end of file
