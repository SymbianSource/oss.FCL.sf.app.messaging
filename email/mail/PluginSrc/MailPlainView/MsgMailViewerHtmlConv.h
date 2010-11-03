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
*     Utility class for HTML converter
*
*/


#ifndef CMSGMAILVIEWERHTMLCONV_H
#define CMSGMAILVIEWERHTMLCONV_H

//  INCLUDES
#include <e32base.h>
#include <txtrich.h>

// FORWARD DECLARATIONS
class CMsgBodyControl;
class CCnvCharacterSetConverter;

// CLASS DECLARATION

/**
*  Utility class for HTML converter. 
*  A pair of two strings.
*/
class CStringPair : public CBase
    {
    public:
        /**
        * A constructor.
        * @param aName a HTML macro, e.g. lt
        * @param aValue a character that is represented by a macro e.g. <
        * @return created object.
        */
        static CStringPair* NewLC(const TDesC& aName, const TDesC& aValue);

        /**
        * Destructor
        */
        ~CStringPair();

        /**
        * @return a name of the macro
        */
        HBufC* GetName() const;
        
        /**
        * @return a value of the macro
        */
        HBufC* GetValue() const;
        
        /**
        * Compares if two pairs are same. Names are compared.
        * @return as in TBufC::Compare.
        */
        static TInt Compare(const CStringPair& aPair1, 
            const CStringPair& aPair2);

    private:
        /**
        * A constructor        
        */
        CStringPair();

        /**
        * A 2nd phase constructor
        * @param aName a HTML macro, e.g. lt
        * @param aValue a character that is represented by a macro e.g. <
        */
        void ConstructL(const TDesC& aName, const TDesC& aValue);

    private:
        /// Name of the macro. Owns.
        HBufC* iName;
        
        /// Value of the macro. Owns.
        HBufC* iValue;
    };


/**
*  HTML converter.
*  This class is used in Mail viewer application to convert
*  messages with HTML-formatting to plain text.
*/
class CMsgMailViewerHtmlConv : public CActive
    {
    public:  // Constructors and destructor                

        /**
        * Two-phased constructor.
        * @param aHandle a handle to HTML-file.
        * @param aCharset Characterset ID
        * @param aForced forse converter to use passed Character set id
        * otherwise converter tries to use character set defined in HTML content
        * If zero character set id is tried to find out from the HTML content.
        */            
        static CMsgMailViewerHtmlConv* NewLC(
        	RFile& aHandle, TUint aCharset, TBool aForced);
        static CMsgMailViewerHtmlConv* NewL(
            RFile& aHandle, TUint aCharset, TBool aForced);
        
        /**
        * Destructor.
        */
        virtual ~CMsgMailViewerHtmlConv();

    public: // New functions
        
        /**
        * Removes HTML-tags and replaces macros with real characters.
        * @return KErrNone if part of the conversion has been successful.
        * Returns KErrEof when conversion is done.
        */
        TInt Convert();

        /**
        * Removes HTML-tags and replaces macros with real characters.  
        * Does the conversion asynchronously in low priority.
        * 
        * @param aStatus Generic request status.
        *        KErrNone when the convertion is succesful. 
        *        Other error codes are used to indicate an error during conversion. 
        */
        void Convert( TRequestStatus* aStatus );

        
        /**
        * Returns a stripped text.
        * @return stripped text object.
        */
        HBufC* GetText() const;

        /**
        * Return file id array of inline images.
        * @return Reference to array.
        * Ownership is transferred to caller
        **/
        RPointerArray<HBufC>* FileIdArray();       
        /**
        * Checks if the charset name and identifiers were autoparsed
        */
        TBool AutoParsedCharSetNameAndIdentifier() const;
        
        /**
        * Returns the auto parsed charset identifier
        */
        TUint AutoParsedCharSetIdentifier() const;    

    private:
        /**
        * C++ default constructor.
        */
        CMsgMailViewerHtmlConv();

        /**
        * By default Symbian OS constructor is private.
        * @param aHandle file handle to HTML content
        * @param aCharSet characterset id
        */
        void ConstructL(RFile& aHandle, TUint aCharSet);
        
        /**
        * Deletes aLen characters beginning from aPos in iStrippedText.
        * @param aLen how many characters will be deleted.
        * @return New length of the message.
        */
        TInt Delete(const TInt aLen);

        /**
        * Inserts characters beginning from aPos in iStrippedText.
        * @param aChars characters to be inserted.
        * @return New length of the message.
        */
        TInt InsertTextL(const TDesC& aChars);

        /**
        * Replaces macro with real text
        * @return New length of the message.
        */
        TInt ReplaceMacroL();

        /**
        * Fills a map of macros with name, value pairs.
        */
        void InitMacroArrayL();

        /**
        * Converts unnumbered list to a text.
        * @param Indentation of the items.
        * @return New length of the message.
        */
        TInt HandleUListL(const TInt aIndent);

        /**
        * Converts ordinal list to a text.
        * @param Indentation of the items.
        * @return New length of the message.
        */
        TInt HandleOListL(const TInt aIndent);
        
        /**
        * Converts definition list to a text.
        * @param Indentation of the items.
        * @return New length of the message.
        */
        TInt HandleDListL(const TInt aIndent);

        /**
        * Converts lists into a text.
        * @param aIndent Indentation of the items
        * @param aNumbered, ETrue if ordinal list, EFalse otherwise
        * @return New length of the message
        */
        TInt HandleListL(const TInt aIndent, const TBool aNumbered);

        /**
        * Converts HTML tags to plain text.
        * @param indentation of the lists.
        * @return New length of the message.
        */
        TInt HandleTagL(const TInt aIndent);

        /**
        * Removes characters from iPos to >-character. Shows an error
        * and interrupts conversion if it is not found.
        * @return New length of the message.
        */
        TInt RemoveTagL();

        /**
        * Adds an error message to end of the message 
        * and shows an information note.
        */
        void ShowErrorL();

        /**
        * Adds a linefeed into a text.
        */
        void InsertLinefeedL();

        /**
        * Performs a character conversion for area started with 
        * BeginConvertL();
        */
        void DoConvertL();
        
        /**
        * Main loop
        */
        void DoConversionL();

        /**
         * Continue asynchronous conversion in next scheduler loop
         */
        void ContinueAsyncConvert();
        
        
    protected: // From CActive
        void RunL();
        void DoCancel();

        
    private:    // Data
        /**
        * Contains a text read from file which is stripped (Owned).
        */
        HBufC *iOrigText;

        /**
        * A position in text.
        */
        TInt iPos;

        /**
        * An array which contains all HTML macro definitions which
        * are read from a resource file.
        */
        RPointerArray<CStringPair> *iMacros;
        
        /**
        * An array which contains places for line feeds. These will 
        * be inserted to a RTF object.
        */
        RArray<TInt> *iNewLines;

        /**
        * Status of the conversion
        */
        TInt iReturnValue;

        /**
        * Position where an error occured
        */
        TInt iErrorPos;

        /**
        * Own: Character converter instance.
        */
        CCnvCharacterSetConverter* iCharConv;

        /**
        * State conversion is in.
        */
        TInt iConversionState;

        /**
        * An array containing inline image names
        */
        RPointerArray<HBufC>* iFileIdArray;

        /**
        * Characterset name
        */
        HBufC* iCharSetName;

        /**
        * Characterset id
        */
        TUint iCharacterSetId;
        
        /**
        * Status flags
        */
        TUint iStatusFlags;
        
        /**
         * Asynchronous convert request status 
         */
        TRequestStatus* iReqStatus;

    };

#include "MsgMailViewerHtmlConv.inl" // inline functions

#endif      // CMSGMAILVIEWERHTMLCONV_H
            
// End of File
