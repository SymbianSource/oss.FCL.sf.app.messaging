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



#ifndef _GMSMODEL_
#define _GMSMODEL_

// INCLUDES

#include<e32base.h>

// CONSTANTS

const TInt KGmsMaxPictureWidthPixels  = 72; //this is also in appl. LAF
const TInt KGmsMaxPictureHeightPixels = 28; //this is also in appl. LAF
const TInt KGmsMinPictureWidthPixels  = 1;
const TInt KGmsMinPictureHeightPixels = 1;

// MACROS
// DATA TYPES

/**
 * After an import or after setting the text, the model will be in one of the
 * two modes.
 */
enum TGmsMode
    {
    EGmsModeUnicode,
    EGmsModeLatin
    };

enum TGmsModelPanic
    {
    EGmsMPanicNoPicData1,
    EGmsMPanicNoPicData2,
    EGmsMPanicNoPictureToExport,
    EGmsMPanicStreamIsEmpty,
    EGmsMPanicNotSupported,
    EGmsNullParameter1,
    EGmsNoPic1,
    EGmsNull1,
    EGmsProblem1,
    EGmsProblem2
    };

// FORWARD DECLARATIONS

class RReadStream;
class RWriteStream;
class CRichText;
class RFs;

// CLASS DECLARATION

/**
 * Class for managing the data of a Picture Message.
 * It supports message text of type Unicode and ISO-8859-1.
 * See Smart Messaging Specification 3.0.0 for info on picture messaging.
 *
 * Note that the failure (leaving) of the import operation might
 * put the model in an invalid state. The model should be reset after a
 * failed import.
 */
class CGmsModel : public CBase
    {
    public: //construction and destruction
        /**
         * Two-phased constructor
         * @param aFs Reference to an open file session.
         * @param aLeaveCodeForCorrupt The leave code that is to be used for
         * signalling about corrupted data when importing. It is kept in
         * iLeaveCorrupt.
         * @return The newly created object.
         */
        IMPORT_C static CGmsModel* NewL(RFs& aFs, TInt aLeaveCodeForCorrupt);

        /// Destructor
        IMPORT_C ~CGmsModel();

    public:

        /**
         * Reads the GMS data from a descriptor, parses it and stores the
         * parsed data in members. The text and bitmap is extracted at this
         * stage. The bitmap is validated.
         * @param aInput The input.
         * @exception If data is corrupt or picture invalid, leaves with
         * iLeaveCorrupt.
         */
        IMPORT_C void ImportGMSL(const TDesC8& aInput);

        /**
         * Imports the GMS data from a rich text object, and parses the data.
         * This is used primarily with rich text obtained from a CMsvStore.
         * Extracts text and bitmap.
         * Leaving happens if the extraction fails.
         * @param aRichText The source of extraction.
         * @exception If data is corrupt, leaves with iLeaveCorrupt.
         */
        IMPORT_C void ImportGMSL(const CRichText& aRichText);

        /**
         * Returns the GMS message as a HBufC8 which is on the cleanup stack.
         *
         * The text part is exported as UCS-2 format if the model is in
         * Unicode mode. If the model is in Latin mode, the text is exported
         * Latin text. (see the S.M. specification) You can use the function
         * IsTextLatin() to find out the current text mode. (the model can
         * only be in Latin or Unicode mode)
         *
         * @return The GMS message in a HBufC8.
         * @exception Panics if there is no picture data.
         */
        IMPORT_C HBufC8* ExportGMS8LC();

        /**
         * Returns the GMS message as a HBufC16 which is on the cleanup stack.
         * Each data byte occupies a 16 bit slot.
         *
         * The text part is exported as UCS-2 format if the model is in
         * Unicode mode. If the model is in Latin mode, the text is exported
         * Latin text. (see the S.M. specification) You can use the function
         * IsTextLatin() to find out the current text mode. (the model can
         * only be in Latin or Unicode mode)
         *
         * @return The GMS message in a HBufC16.
         * @exception Panics if there is no picture data.
         */
        IMPORT_C HBufC16* ExportGMS16LC();

        /**
         * Gets text part as a HBufC, which is placed on the cleanup stack.
         * @return Text part of message as HBufC in cleanup stack.
         */
        IMPORT_C HBufC* TextAsHBufC16LC() const;

        /**
         * Sets the text for this model. It is able to handle Unicode also.
         * Afterwards you can use the method IsTextLatin() for finding out if
         * the model is in Unicode mode or Latin mode.
         * If you set the text again, the earlier text is replaced.
         * There is no restriction on the length of the text.
         * @param aSource The source text.
         */
        IMPORT_C void SetTextL(const TDesC& aSource);

        /**
         * Sets the message text with latin type of text.
         * Note that this function does not check the character set.
         * It sets the model to Latin mode. The mode can be queried
         * using IsTextLatin(). (the modes are Latin and Unicode)
         * If you set the text again, the earlier text is replaced.
         * There is no restriction on the length of the text.
         * @param aSource The source text.
         */
        IMPORT_C void SetTextLatinL(const TDesC& aSource);

        /**
         * Sets the message text with Unicode type of text.
         * There is no restriction on the length of the text.
         * Note that this function does not check the character set.
         * It sets the model to Unicode mode. The mode can be queried
         * using IsTextLatin(). (the modes are Latin and Unicode)
         * If you set the text again, the earlier text is replaced.
         * UCS-2 encoding is used, so each char occupies two bytes.
         * @param aSource The source text.
         */
        IMPORT_C void SetTextUnicodeL(const TDesC& aSource);

        /**
         * Exports only the OTA Bitmap.
         * @param aOutStream Reference to an output stream which is open.
         * @exception Panics with EGmsNoPictureToExport if there is no picture
         * data to export.
         */
        IMPORT_C void ExportOTABitmapL(RWriteStream& aOutStream);

        /**
         * Imports an OTA Bitmap.
         * Leaves with iLeaveCorrupt if the data is invalid.
         * If you import again, the earlier data is replaced.
         *
         * @param aReadStream Reference to the input stream which is open.
         * @exception Panics if the stream buffer is empty.
         */
        IMPORT_C void ImportOTABitmapL(RReadStream& aReadStream);

        /**
         * Size of complete message excluding text and pic.
         * @return Number of bytes.
         */
        IMPORT_C TInt SizeOfCompleteMsgExcludingTextAndPic() const;

        /**
         * Does the converter have text data.
         * @return ETrue if there is text data.
         */
        IMPORT_C TBool HasText() const;

        /**
         * Is there picture data? Note that the validity of the picture data
         * is not checked.
         * @return ETrue if there is picture data.
         */
        IMPORT_C TBool HasPicture() const;

        /**
         * Tells if the text data is of type Latin or Unicode.
         * @return ETrue if the type of text is Latin, and EFalse if Unicode.
         */
        IMPORT_C TBool IsTextLatin() const;

        /**
         * Returns the total message size but not taking into account the text
         * size.
         * @return Message size in bytes.
         */
        IMPORT_C TInt TotalSizeExcludingText() const;

        /**
         * The total size of the message data in bytes.
         * @return Message size in bytes.
         */
        IMPORT_C TInt TotalSize() const;

        /**
         * Length of the text.
         * It is not the same thing as size of text. You can use IsTextLatin()
         * to find out if it is Unicode or Latin.
         */
        IMPORT_C TInt Textlength() const;

        /**
         * Size of picture in bytes.
         * @return Size of picture in bytes.
         */
        IMPORT_C TInt PictureSize() const;

        /// Resets this model.
        IMPORT_C void Reset();

        /**
         * Deprecated. Do not use this method.
         */
        IMPORT_C TInt _TestingL(TInt aCode1, TInt& aCode2, const TDesC8& aDes1, const TDesC16& aDes2);

        /// Deprecated. Do not use this method.
        IMPORT_C HBufC* ExtractTextFromMsgLC(const TDesC& aMsg, TInt aAmount);

        /// Deprecated. Do not use this method.
        IMPORT_C TBool IsPictureValidL();

    public: //not exported

        /**
         * Imports the OTA Bitmat and validates it. Does not leave if bitmap
         * is invalid. The validation result is returned by the method.
         * @param aFileName Source file name.
         * @return Validation result.
         */
        TBool ImportOtaBitmapL(const TDesC& aFileName);

    private: //message data extraction functions

        /**
         * Checks if version of Picture Message is valid. Leaves with
         * iLeaveCorrupt if invalid.
         * This function is the first extraction function called.
         * @param aInput The source of extraction.
         * @param aIndex Reference to position of extraction. (not const)
         * @exception If data is corrupt, leaves with iLeaveCorrupt.
         */
        void CheckVersionL(const TDesC8& aInput, TInt& aIndex);

        /**
         * Extracts a sub item which is either a picture, text or something
         * unknown: <item>
         *
         * @param aInput The source of extraction.
         * @param aIndex Reference to position of extraction. (not const)
         * @exception If data is corrupt, leaves with iLeaveCorrupt.
         */
        void ExtractGMSSubItemL(
            const TDesC8& aInput,
            TInt& aIndex);

        /**
         * Extracts a sub item which is either a picture, text or something
         * unknown: <item>
         *
         * @param aInput The source of extraction.
         * @param aIndex Reference to position of extraction. (not const)
         * @param aSkipBitmap Whether to skip the OTA Bitmap.
         * @exception If data is corrupt, leaves with iLeaveCorrupt.
         */
        void ExtractGMSSubItemL(
            const TDesC8& aInput,
            TInt& aIndex,
            TBool aSkipBitmap);

        /**
         * Extracts a Latin text entity: <item-length><ISO-8859-1-char>*
         *
         * @param aInput The source of extraction.
         * @param aIndex Reference to position of extraction. (not const)
         * @exception If data is corrupt, leaves with iLeaveCorrupt.
         */
        void ExtractLatin1L(const TDesC8& aInput, TInt& aIndex);

        /**
         * Extracts a Unicode text entity: <item-length><UNICODE-char>*
         *
         * @param aInput The source of extraction.
         * @param aIndex Reference to position of extraction. (not const)
         * @exception If data is corrupt, leaves with iLeaveCorrupt.
         */
        void ExtractUnicodeL(const TDesC8& aInput, TInt& aIndex);

        /**
         * Extracts OTA Bitmap data: <OTA-bitmap>
         * It also checks the OTA Bitmap syntax and leaves if it is bad or too
         * big.
         * @param aInput The source of extraction.
         * @param aIndex Reference to position of extraction. (not const)
         * @exception If there is less data than what the length field
         *   specifies, it leaves with iLeaveCorrupt.
         * @exception Leaves with iLeaveCorrupt in case of syntax error.
         */
        void ExtractOTABitmapL(const TDesC8& aInput, TInt& aIndex);

        /**
         * Skips an entity which is of unknown type.
         * @param aInput The source of extraction.
         * @param aIndex Reference to position of extraction. (not const)
         * @exception If there is less data than what the length field
         *   specifies, it leaves with iLeaveCorrupt.
         */
        void ExtractUnknownL(const TDesC8& aInput, TInt& aIndex);

        /**
         * Extracts the entity length header: <item-length>
         * @param aInput The source of extraction.
         * @param aIndex Reference to position of extraction. (not const)
         * @exception If data is corrupt, leaves with iLeaveCorrupt.
         */
        TInt ExtractLengthL(const TDesC8& aInput, TInt& aIndex);

    private: // other private functions

        /**
         * Checks if the text contains any Unicode characters.
         * Unicode line breaks are ignored. They are converted to LF if
         * exporting in Latin-1 mode.
         * @param aText The text that is to be scanned.
         * @return ETrue if text contains Unicode characters.
         */
        TBool ContainsUnicodeL(const TDesC& aText) const;

        /**
         * Appends <version>
         * @param aGms The descriptor to which the data is appended.
         */
        void AppendVersionL(TDes8& aGms);

        /**
         * Appends "00" <Item-length> <ISO-8859-1-char>*
         *       | "01" <Item-length> <UNICODE-char>*
         * @param aGms The descriptor to which the data is appended.
         */
        void AppendTextSubItemL(TDes8& aGms);

        /**
         * Appends "02" <Item-length> <OTA-bitmap>
         * @param aGms The descriptor to which the data is appended.
         */
        void AppendPictureSubItemL(TDes8& aGms);

        /**
         * Encodes Unicode text into UCS2 and appends it to a descriptor.
         * @param aGms The descriptor to which the data is appended.
         * @param aText The Unicode text.
         */
        void AppendUCS2TextL(TDes8& aGms, const TDesC& aText);

        /**
         * Apppends the text to aGms, but replaces Unicode line breaks with
         * CR.
         * @param aGms The descriptor to which the data is appended.
         * @param aText The Latin1 text (which may contain Unicode line
         * breaks).
         */
        void AppendLatin1TextL(TDes8& aGms, const TDesC& aText);

        /**
         * Checks the OTA syntax and size. It leaves with iLeaveCorrupt in
         * case the bitmap did not pass the tests.
         * @param aOta The data that is to be checked.
         * @return The checked and fixed bitmap.
         */
        HBufC8* CheckedAndFixedOtaBitmapL(const TDesC8& aOta) const;

        /**
         * Checks the OTA syntax and size. Does not leave if the bitmap is
         * bad. The result is obtained using a TBool reference parameter.
         * @param aOta The data that is to be checked.
         * @param aSuccess Reference to a TBool that gets the result of the
         * validation.
         * @return The checked and fixed bitmap, or NULL if the validation
         * failed.
         */
        HBufC8* CheckedAndFixedOtaBitmapL(const TDesC8& aOta, TBool& aSuccess) const;

        /**
         * Panic wrapper.
         * @param aCode The panic code.
         */
        static void Panic(TGmsModelPanic aCode);

    private: // construction related

        /// constructor
        CGmsModel(RFs& aFs, TInt aLeaveCodeForCorrupt);

        /// Second phase constructor.
        void ConstructL();

    private: // prohibited

        /// The default constructor is prohibited
        CGmsModel();

        /// Copy contructor prohibited.
        CGmsModel(const CGmsModel& aSource);

        /// Assignment operator prohibited.
        const CGmsModel& operator=(const CGmsModel& aSource);

    private: //data

        /// The message text in Unicode (native Symbian OS) format.
        HBufC16* iText;

        /**
         * The OTA Bitmap data.
         */
        HBufC8* iOTABitmap;

        /**
         * Flag which tells if the model is in Latin or Unicode mode
         * The mode is changed by either using the text setting functions, or
         * by importing a complete message.
         */
        TGmsMode iMode;

        mutable RFs& iFs;

        /**
         * This leave code is used if the importing fails because of corrupt
         * data. It is set in the constructor.
         */
        const TInt iLeaveCorrupt;
    };



#endif // _GMSMODEL_

// end of file
