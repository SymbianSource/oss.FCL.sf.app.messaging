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
*     Declares character converter for application.
*
*/


#ifndef MSGMAILVIEWERCHARCONV_H
#define MSGMAILVIEWERCHARCONV_H

// INCLUDES
#include <AknWaitNoteWrapper.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CImConvertCharconv;
class CCnvCharacterSetConverter;
// CLASS DECLARATION


/**
*  CMsgMailViewerDocument application class.
*/
class CMsgMailViewerCharConv : public CBase, public MAknBackgroundProcess
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CMsgMailViewerCharConv* NewL( TUint aCharset );

        /**
        * Destructor.
        */
        virtual ~CMsgMailViewerCharConv();

    public: // from MAknBackgroundProcess

        /**
         * Completes one cycle of the process.
         */
        void StepL();

        /**
         * Return true when the process is done.
         */
        TBool IsProcessDone() const;

    private:

        /**
        * Default constructor.
        */
        CMsgMailViewerCharConv();
        
        /**
        * Symbian OS default constructor.
        */
        void ConstructL( TUint aCharset );

    public: // new methods
        /**
        * Prepares to convert from Unicode.
        * @param aSource  The text to be converted.
        */
        void PrepareToConvertFromUnicodeL( const TDesC16& aSource );

        /**
        * Prepares to convert to Unicode.
        * @param aSource  The text to be converted.
        */
        void PrepareToConvertToUnicodeL( const TDesC8& aSource );

        /**
        * Returns the result of the conversion to Unicode. The ownership is
        * transferred to the caller.
        * @return       The result of conversion.
        */
        HBufC16* GetUnicodeText();

        /**
        * Returns the result of the conversion from Unicode. The ownership is
        * transferred to the caller.
        * @return       The result of conversion.
        */
        HBufC8* GetForeignText();

        /**
        * Returns the number of failed characters for Unicode->foreign conversion.
        * @return       The number of failed characters.
        */
        TInt GetFailedCount() const;

    private: // new methods
        /**
        * Converts one block from Unicode.
        */
        void StepFromUnicodeL();

        /**
        * Converts one block to Unicode.
        */
        void StepToUnicodeL();

        /**
        * Appends data to iTarget16 possibly extending it.
        * @param aData      Data to be appended.
        */
        void AppendTextL( const TDesC16& aData );

        /**
        * Appends data to iTarget8 possibly extending it.
        * @param aData      Data to be appended.
        */
        void AppendTextL( const TDesC8& aData );

    private: // Member data
        /// Owns: Character converter instance.
        CCnvCharacterSetConverter* iCharConv;
        /// Own
        CImConvertCharconv* iImCharConv;

        /// Own: The text in target character set if converting from Unicode.
        HBufC8* iTarget8;

        /// Own: The text in target character set if converting to Unicode
        HBufC16* iTarget16;

        /// Refs: The text in source character set if converting from Unicode.
        TPtrC16 iSource16;

        /// Refs: The text in source character set if converting to Unicode.
        TPtrC8 iSource8;

        /// ETrue, if converting from Unicode
        TBool iFromUnicode;

        /// State when converting to Unicode
        TInt iState;

        /// Number of failed characters when converting from Unicode.
        TInt iFailCount;
    };

#endif

// End of File

