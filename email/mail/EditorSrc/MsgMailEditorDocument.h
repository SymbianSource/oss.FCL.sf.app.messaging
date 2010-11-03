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
* Description:  Declares document class of Mail Editor.
*
*/


#ifndef MSGMAILEDITORDOCUMENT_H
#define MSGMAILEDITORDOCUMENT_H

// INCLUDES

#include "MsgMailDocument.h" // CMsgMailDocument

#include <miutset.h>  // KUidMsgTypeSMTP, KUidMsvServiceEntry
#include <miutmsg.h>  // for TImAttachmentInfo
#include <MsgAttachmentInfo.h>
#include <ImumInternalApi.h>
#include <ImumInHealthServices.h>


// FORWARD DECLARATIONS
class CEikAppUi;
class CMsgMailPreferences;
class MsgMailDRMHandler;

// CLASS DECLARATION

/**
*  CMsgMailEditorDocument document class.
*/
class CMsgMailEditorDocument
: public CMsgMailDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aApp a reference to an application
        * @return A pointer to created document object
        */
        static CMsgMailEditorDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CMsgMailEditorDocument();

    public: // New functions
            	    	
        /**
        * Check that are we creating a new entry or editing an old one.
        */
        TBool IsNewMessage() const;
        
        /**
        * Check if this is reply message.
        */
        TBool IsReplyMessage() const;
        
        /**
        * Check if this is a forward message.
        */
        TBool IsForwardMessage() const;
        
        /**
        * Read sending options.
        * If new message read default settings. 
        * If old message read settings from mail entry.
        */
        void ReadSendOptionsL();
        
        /**
        * @return ETrue if Online
        */
        TBool IsOnlineL();
        
        /**
         * Opens mail message entry
         * @since S60 3.1
         * @param aEntry Entry to be opened.
         */
        void OpenMailMessageEntryL( TMsvEntry aEntry );
        
        /**
         * Send mail message
         * @since S60 3.1
         * @param aMsgId Entry to be send.
         */        
        void SendImmediatelyL( const TMsvId aMsgId );
        
        /**
         * Get array of healty mailboxes
         * @since S60 3.1
         * @param aFlags see MIMAMailboxDoctor::GetHealthyMailboxList
         * Ownership not transferred.
         */         
        MImumInHealthServices::RMailboxIdArray GetHealthyMailboxListL(
            const TInt64& aFlags );
            
        MMsvAttachmentManager& GetAttachmentManagerL();

    private: // implementation

        CMsgMailEditorDocument(CEikApplication& aApp);
        void ConstructL();
                      
    private:    // New functions
        /**
        * This is called from NotifyChanges and possible leave is 
        * trapped there.
        * @param aCommand executed command
        * @param aAttachmentInfo affected attachment
        */
        void NotifyChangesL(TMsgAttachmentCommand aCommand, 
            CMsgAttachmentInfo* aAttachmentInfo);    

    private: // Functions from base classes
        
        /// From CEikDocument, create CMsgMailEditorAppUi "App UI" object.
        CEikAppUi* CreateAppUiL();
    
    public: 
        /// From MMsgAttachmentModelObserver
        void NotifyChanges(
        	TMsgAttachmentCommand aCommand, 
        	CMsgAttachmentInfo* aAttachmentInfo);
        	
        // From MMsgAttachmentModelObserver    
        RFile GetAttachmentFileL( TMsvAttachmentId aId );

    private: // data

        // Owned: Pointer to email API object
        CImumInternalApi*   iEmailApi;
        
        // Owned: Array of mailbox ids
        MImumInHealthServices::RMailboxIdArray iMailboxArray;
        
    };

#endif

// End of File

