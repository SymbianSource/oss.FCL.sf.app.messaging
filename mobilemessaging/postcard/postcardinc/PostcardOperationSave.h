/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       CPostcardOperationSave, asynchronous operation for postcard saving
*
*/



#ifndef __POSTCARDOPERATIONSAVE_H
#define __POSTCARDOPERATIONSAVE_H

// INCLUDES

#include <e32base.h>
#include <e32std.h>

#include "PostcardOperation.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CMsvAttachment;
class CPlainText;
class CContactCard;
class CCnvCharacterSetConverter;
class CContactItemFieldSet;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CPostcardOperationSave 
* @lib postcardeditor.exe
* @since 3.0
*/
class CPostcardOperationSave : public CPostcardOperation
    {
    public:  // New methods

        /**
        * Factory method that creates this object.
        * @since    3.0
        * @return   Pointer to instance
        */
        static CPostcardOperationSave* NewL(
            MPostcardOperationObserver& aObserver,
            CPostcardDocument& aDocument,
            CPostcardAppUi& aAppUi,
            RFs& aFs,
			CContactCard& aContactItem,
            CPlainText& aPlainText );

        /**
        * Destructor
        */
        virtual ~CPostcardOperationSave( );

        /**
        * Checks the argument to determine if the operation is save or send
        * Initializes iLaunchState and completes itself.
        * param aArgument If 0 - this is Save, otherwise Send
        */
		void Start( TInt aArgument );

    private:

        /**
        * Called by RunL.
        * Calls different functions according to iLaunchStep
        */
        void DoLaunchStepL( );

        /**
        * Removes the old text and recipient attachments.
        */
		void RemoveOldAttasL( );

        /**
        * Calls StartCreatingEmptyAttachmentL with greeting text file name
        */
		void CreateNewTextAttaL( );

        /**
        * Creates CharConverter.
        * Create new mime headers.
        * Sets mime types and possible OMA X header for text file.
        * If special format is used, handles the adding of the recipient address here
        * Converts user added separator characters to replacement characters.
        * Then writes the whole text into message store buffer by buffer.
        */
		void HandleTextL( );

        /**
        * Creates a special format text from postcard greeting and address
        */
        HBufC* MakeSpecialFormatTextL( );

        /**
        * Stores the mime headers.
        * In case special format was used -> skips the following recipient handling.
        * and smil adding and moves directly to finalizing the message
        */
		void FinalizeTextL( );

        /**
        * Calls StartCreatingEmptyAttachmentL with recipient VCard file name
        */
		void CreateNewRecipientAttaL( );

        /**
        * Creates CharConverter.
        * Create new mime headers.
        * Sets mime types VCard file.
        * Creates an instance of CParserVCard.
        * Adds the recipient address fields into the parser.
        * Parser externalizes itself into the message store.
        */
		void HandleRecipientL( );

        /**
        * Stores the mime headers.
        */
		void FinalizeRecipientL( );

        /**
        * In case Postcard app supports OMA 1.3 Postcard format a SMIL file is created.
        * Calls StartCreatingEmptyAttachmentL with smil file name
        */
		void CreateNewSmilAttaL( );

        /**
        * Creates and sets mime headers.
        * Reads the SMIL from resource file (R_POSTCARD_OMA_SMIL)
        * Writes the smil into the file.
        */
		void HandleSmilL( );

        /**
        * Stores the mime headers.
        */
		void FinalizeSmilL( );

        /**
        * In case no recipient is set, reads the address of the service provider
        * from the resource file and sets it as a recipient (of the Postcard MMS itself, not the postcard)        
        * Updates the TMsvEntry (details, description, visibility, editor-orientation, biotype)
        */
		void FinalizeMessageL( );

        /**
        * Call ClientMtm's Send function. Called if this operation was started as "Send"
        */
		void StartSendingL( );

        /**
        * From CActive
        */
        void DoCancel( );

        /**
        * From CActive
        * Calls DoLaunchStepL
        */
        void RunL( );
    
        /**
        * From CActive
        */
        TInt RunError( TInt aError );

    protected: 

        /**
        * Constructor.
        */
        CPostcardOperationSave(
            MPostcardOperationObserver& aObserver,
            CPostcardDocument& aDocument,
            CPostcardAppUi& aAppUi,
            RFs& aFs,
			CContactCard& aContactItem,
            CPlainText& aPlainText );

        /**
        * 2nd phase constructor.
        */
        void ConstructL( );

    private:

        /**
        * Creates a new empty attachment and asks AttachmentManager
        * to add it into message store. 
        */
        void StartCreatingEmptyAttachmentL( const TFileName& aFileName );

        /**
        * Returns ETrue if any of the address fields has at least one character
        */
		TBool HasContact( );
		
        /**
        * Reads the address fields from aSet and adds them into an array.
        * The order is so that the strings match to the indexes used in
        * special format string. If operation is in "Send" mode and
        * special format is used -> looks for separator characters in
        * the field values and calls ReplaceWithL to replace them with
        * replace characters.
        * Returns the array to the caller.
        */
		CDesCArrayFlat* AddressArrayLC( CContactItemFieldSet& aSet );
		
        /**
        * Reads the address fields from aSet and adds them into an array.
        * The order is so that the strings match the order used in VCard address field
        * If operation is in "Send" mode and special format is used ->
        * looks for separator characters in the field values and calls
        * ReplaceWithL to replace them with replace characters.
        * Returns the array to the caller.
        */
		CDesCArrayFlat* VCardAddressArrayL( CContactItemFieldSet& aSet );

        /**
        * Looks for aWhat's in descriptor aOrig. Replaces aWhat's with aWiths.
        * param aOrig IN/OUT Function modifies this descriptor so that aWhat is replaced with aWith
        * param aWhat IN Replace this string
        * param aWith IN Replace with this string
        */
		void ReplaceWith( TDes& aOrig, const TDesC& aWhat, const TDesC& aWith );
		
        /**
        * Finds the image attachment and gets its name.
        * returns the the image file name buffer
        */	
		HBufC* GetImageFileNameL( );
		
    protected: // data

        enum TPostcardSaveState
            {
            EPostcardSaveInitializing = 0,
            EPostcardSaveCheckAttas,
            EPostcardSaveRemoveOldAttas,
            EPostcardSaveCreateTextAtta,
            EPostcardSaveSaveTextAtta,
            EPostcardSaveFinalizeTextAtta,
            EPostcardSaveCreateRecipientAtta,
            EPostcardSaveSaveRecipientAtta,
            EPostcardSaveFinalizeRecipientAtta,
            EPostcardSaveCreateSmilAtta,
            EPostcardSaveSaveSmilAtta,
            EPostcardFinalizeSmilAtta,
            EPostcardSaveUninit,
            EPostcardSaveFinalizeMessage,
            EPostcardSaveStartSend,
            EPostcardSaveFinished,
            EPostcardSaveError
            };

		enum TPostcardSaveFlags
			{
			EPostcardSaveSend = 1,
			EPostcardSaveSpecialFormat = 2,
			EPostcardSaveHasContact = 4,
			EPostcardSaveRequestActive = 8
			};
			
    private:
        CMsvAttachment* 	iTempAtta;
        RFile*          	iTempFile;
        CContactCard& 		iContact;
		CCnvCharacterSetConverter* iCharConv;
        
        CPlainText&			iPlainText;
        CMsvOperation*		iSendOperation;
        HBufC*             	iRecipientName;
        TInt 				iFlags;
    };

#endif // __PostcardOperationSave_H
