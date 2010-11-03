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
* Description:  Base class for document classes of the 
*                 mail viewer and editor applications
*
*/


#ifndef MSGMAILDOCUMENT_H
#define MSGMAILDOCUMENT_H

//  INCLUDES
#include <MsgEditorDocument.h>          // CMsgEditorDocument
#include <apgcli.h>

// FORWARD DECLARATIONS
class CImHeader;
class CMsgMailViewerSettings;
class TMsvEmailEntry;
class CImEmailMessage;
class CEikMenuPane;
class CMsgMailPreferences;
class CMailMessage;
class CMailCRHandler;

// CLASS DECLARATION

/**
*   CMsgMailDocument, base class for editor and viewer application document 
*   classes.
*   Defines CMsgMailDocument -class, which is a base class for document 
*   classes of the mail viewer and editor applications. The document is 
*   part of the Symbian OS application framework.
*/
class CMsgMailDocument :public CMsgEditorDocument
    {
    public:  // Constructors and destructor
    
	// Types of messages to keep count
	enum
		{
	    EIndexAddHeaders = 0,
	    EIndexLast
	    };

        /**
        * Destructor.
        */
        IMPORT_C ~CMsgMailDocument();

    public: // New functions
        
        /**
        * Sets preferences of the message..
        * @param aModel model.
        */
        void SetPreferences(CMsgMailPreferences* aPrefs);

        /**
        * Returns header of the message.
        * @return CImHeader, header of the message.
        */
        CImHeader& HeaderL() const;
        
		/**
        * Get entry of the message.
        * @return TMsvEmailEntry entry of the message.
        */
        IMPORT_C const TMsvEmailEntry& TMsvEMailEntryL();

        
        /**
        * Returns reference to message object.
        * @return CImEmailMessage reference to message.
        */
        IMPORT_C CImEmailMessage& MessageL();               
        
        /**
        * inline
        * Creates new message.
        * @see CreateNewL(TMsvId aServiceType, TMsvId aDestination, 
        *    TInt aTypeList, TUid aMtm)
        * 
        *   calls  CreateNewL(aService, aTarget, aTypeList, KUidMsgTypeSMTP);
        */
        TMsvId CreateNewL(TMsvId aService, TMsvId aTarget, 
            TInt aTypeList);

        /**
        * inline
        * @return a pointer to Mail Central Repository handler.
        * Ownership not transferred.
        */
        CMailCRHandler* MailCRHandler() const;
        
        /**
        * Creates new message.
        * A new message of given type is created in a destination folder.
        * @param aServiceType service used for sending message
        * @param aTarget folder in which message will be saved eg. 
        * KMsvDraftEntryIdValue
        * @param aTypeList type of message to be created. 
        * @see TMsvEmailTypeList
        * @param aMtm, message type to be created e.g. KUidMsgTypeSMTP
        * @return ID of mail operation. 
        */
        IMPORT_C TMsvId CreateNewL(TMsvId aServiceType, TMsvId aTarget, 
            TInt aTypeList, TUid aMtm);
        
        /**
        * Creates new message entry.
        * @return new message ID.
        */        
        TMsvId CreateNewEntryL( TMsvEmailEntry& aEntry );
        
        /**
        * Returns a preferences object of the document.
        * @return A preference object.
        */
        CMsgMailPreferences& SendOptions() const;

        /**
        * Sets iConfNote flag according to parameter.
        * @param aConfNote value to set.
        */
        void SetConfirmation(TBool aConfNote);

        /**
        * Checks if confirmation note needs to be shown to user.
        * @return ETrue if note is needed. 
        */
        TBool ShowConfirmation() const;

        /**
        * Save iHeader information to current store.
        */
        void SaveHeaderToStoreL();

        /**
        * Check if we are dealing with a SyncML mail
        * @deprecated
        * @returns EFalse.
        */
        IMPORT_C TBool IsSyncMLMail();
        
        /**
        * Check file mime type
        * @param aFile file to be checked.
        * @return TDataType mime type of a file.
        */        
        IMPORT_C TDataType ResolveMimeType( RFile aFile );
        
        /**
        * Informs the model of updated central repository value
        * Integer override
        * @param aMessageType The index of entry.
        * @param aValue Received value
        */
        TInt SetReceivedIntL( const TUint aMessageType, const TInt aValue );
        
        /**
        * Informs the model of updated central repository value
        * Real override
        * @param aMessageType The index of entry.
        * @param aValue Received value
        */
        TInt SetReceivedRealL( 
            const TUint aMessageType, const TReal aValue );
        
        /**
        * Informs the model of updated central repository value
        * Binary override
        * @param aMessageType The index of entry.
        * @param aValue Received value
        */
        TInt SetReceivedBinaryL( 
            const TUint aMessageType, const TDesC8 aValue );
        
        /**
        * Informs the model of updated central repository value
        * String override
        * @param aMessageType The index of entry.
        * @param aValue Received value
        */
        TInt SetReceivedStringL( 
            const TUint aMessageType, 
            const TDesC16 aValue );
        
        /**
        * Check if we are below critical level.
        * @param aBytesToWrite bytes to be written.
        * @retur ETrue if critical level is not reached.
        */        
        TBool DiskSpaceBelowCriticalLevelL( TInt aBytesToWrite );
        
    protected:  // New functions
        /**
        * Constructor.
        * Initializes variables and calls parent class constructor.
        * @param aApp application object.
        */
        IMPORT_C CMsgMailDocument(CEikApplication& aApp);
    
    public: // Functions from base classes 
        
        /// From CMsgEditorDocument
        TMsvId DefaultMsgService() const;
        TMsvId DefaultMsgFolder() const;
        TMsvId CreateNewL(TMsvId aService, TMsvId aTarget);       
    
    protected:  // Functions from base classes 
        
        /// From CMsgEditorDocument
        IMPORT_C void EntryChangedL();
        IMPORT_C CMsvOperation* CreateReplyL(
            TMsvId aDest, 
            TMsvPartList aParts, 
            TRequestStatus& aStatus );
        IMPORT_C CMsvOperation* CreateForwardL(
            TMsvId aDest, 
            TMsvPartList aParts, 
            TRequestStatus& aStatus );

    
    private: // implementation
        void AddAttachmentNamesToMessageBodyL( const TDesC& aString );
        HBufC* GetAttachmentNamesFromMessageL( 
            CImEmailMessage* aMailMessage );
        TBool IsPlainHTMLMessageL( CImEmailMessage* aMailMessage );
        static TInt ShowHtmlNoteCallBackL( TAny* aCaller );
        
    protected:  // Data
        /// Own: Message proferences
        CMsgMailPreferences* iPrefs;
               
        // Ref: Mail message
		CMailMessage* iViewedMessage;
		
		// Own: message centry
		CMsvEntry* iMessageCEntry;
		
		// original message id
		TMsvId iMessageID;
		
		// Ref: Mail CR handler         
        CMailCRHandler* iMailCRHandler;
        
    private:    // Data
        /// Own: Mail message
        CImEmailMessage* iMessage;
        
        /// Own: Message header
        CImHeader* iHeader;            
        
        /**
        * iConfNote flag telling if user needs to be informed
        * when attachment added.
        */
        TBool iConfNote;
		
		// AP session
		RApaLsSession iApaSession;
		
		//own
		CIdle* iIdle;
		
		/**
		* iShowNoteCallBack is used to show
		* html note to user with some delay.
		*/
		TCallBack iShowNoteCallBack;
        
		
    };

#include "MsgMailDocument.inl" // Inline functions

#endif      // MSGMAILDOCUMENT_H
            
// End of File

