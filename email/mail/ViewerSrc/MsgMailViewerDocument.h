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
* Description:  Declares document for application.
*
*/

#ifndef MSGMAILVIEWERDOCUMENT_H
#define MSGMAILVIEWERDOCUMENT_H

// INCLUDES
#include "MsgMailDocument.h"
#include <cmsvattachment.h>

// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;
class MsgMailDRMHandler;
class CSendUi;
class CMailOperation;
class MMsvProgressReporter;

// CLASS DECLARATION

/**
*  CMsgMailViewerDocument application class.
*/
class CMsgMailViewerDocument : public CMsgMailDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CMsgMailViewerDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CMsgMailViewerDocument();

    public: // New functions
    	/**
    	* Add Attachment to viewer attachment model.
    	* @param aAttachmentInfo attachment info.
    	* @param aCanBeRemoved ETrue if attachment can be removed
    	* via attachment view.
    	*/
    	void AddAttachmentL( 
    		CMsvAttachment& aAttachmentInfo, 
    		TBool aCanBeRemoved );
        
        /**
        * CanSaveFileL
        * @since Series 60 3.0        
        * @param aDataType type to be checked.
        * @return ETrue if system can save attachments with this mime type.
        */      	
    	TBool CanSaveFileL( TDataType aDataType );
    
        /// Returns character set
        TUint Charset() const;
        
        /**
        * DeleteMessageL
        * @since Series 60 3.0        
		* Delete message from server.
		* @return ETrue if operation was cancelled.
        */          
        TBool DeleteMessageL();
        
        /**
        * FetchAttachmentL
        * @since Series 60 3.0        
        * @param aSelection attachments to be fetched.
        * @param aCompletionStatus operation completion status.
        * @return CMsvOperation
        */          
        CMsvOperation* FetchAttachmentL( 
            CMsvEntrySelection& aSelection, 
            TRequestStatus& aCompletionStatus );
        
        /**
        * FetchAttachmentL
        * @since Series 60 3.0        
        * @param aSelection attachments to be fetched.
        * Operation is not completed immediately when this method returns.
        */         
        void FetchAttachmentL( CMsvEntrySelection& aSelection );

        /**
        * FetchAttachmentL
        * @since Series 60 3.0        
        * @param aAttachmentId attachment id.
        * Operation is not completed immediately when this method returns.        
        */         
        void FetchAttachmentL( TMsvAttachmentId aAttachmentId );

        /**
        * RemoveAttachmentL
        * @since Series 60 3.0 
        * @param aAttachmentId attachment id.
        */         
        void RemoveAttachmentL( TMsvAttachmentId aAttachmentId );
        
        /// Sets character set.
        void SetCharSet(const TUint aCharSet);

        /**
        * Check connection status.
        * @return ETrue if connected.
        **/
        TBool IsOnLineL() const;
        
        /**
        * RetrieveMessageL
        * @since Series 60 3.0
        * Retrieves current mail message from the server.
        */          
        void RetrieveMessageL();
        
        /**
        * SendUiL
        * Returns Send UI instance.
        * @since Series 60 3.0        
        */         
        CSendUi& SendUiL();
        
        /**
        * ShowMessageInfoL
        * Opens message info popup
        * @since Series 60 3.0        
        */         
        void ShowMessageInfoL();
               
        /**
        * OpenMessageEntryL
        * @since Series 60 3.0        
        * Opens entry if it is a message.
        * @param aEntryId message entry id.
        * @return ETrue if entry was opened.
        */      	
    	TBool OpenMessageEntryL( TMsvId aEntryId );
		        
        /**
        * @return mail message
        */
        CMailMessage& MailMessage();
        
        /**
        * MoveRemoteEntryL
        * Moves remote messages.
        * @since Series 60 3.0        
        */          
        void MoveRemoteEntryL( 
        	TMsvId aTo, 
        	TMsvId aService, 
        	TBool aDestinationIsRemote );
        
        /**
        * UpdateAttachmentModelFromMessageL
        * Updates attachment model.
        * @since Series 60 3.0        
        */         	
        void UpdateAttachmentModelFromMessageL();
        
        /**
        * ForwardMailMessageL
        * @param aReporter operation UI progress reporter.
        * @param aStatus operation status.
        * @param aSetSuspend ETrue if operation should be suspended.        
        * @return CMailOperation        
        * @since Series 60 3.1        
        */         	        
        CMailOperation* ForwardMailMessageL( 
            MMsvProgressReporter& aReporter,
            TRequestStatus& aStatus, 
            TBool aSetSuspend );

    private: // implementation

        CMsgMailViewerDocument(CEikApplication& aApp);
        void ConstructL();
        void DoInitDRMHandlerL();

    private:

		/// From CEikDocument, create CMsgMailViewerAppUi "App UI" object.
        CEikAppUi* CreateAppUiL();
        
        /// From MMsgAttachmentModelObserver
        void NotifyChanges(TMsgAttachmentCommand aCommand, 
            CMsgAttachmentInfo* aAttachmentInfo );
		/// From MMsgAttachmentModelObserver
		RFile GetAttachmentFileL( TMsvAttachmentId aId );
		
		// From CMsgEditorDocument
		void EntryChangedL();	
		
    private: // Member data
        /**
        * Character set used for the message.
        */
        TUint iCharSet;
        		
		// Own DRM handler
		MsgMailDRMHandler* iDRMHandler;
        
        /**
        * Own: Pointer to Send UI object
        */
        CSendUi* iSendUi;
    };

#include "MsgMailViewerDocument.inl"

#endif

// End of File

