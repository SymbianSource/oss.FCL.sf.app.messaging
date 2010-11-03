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
* Description:  This dialog is used for Attachment handling.
*
*/


#ifndef MSGMAILVIEWERATTACHMENTS_H
#define MSGMAILVIEWERATTACHMENTS_H

//  INCLUDES
#include <MsgViewAttachmentsDialog.h>
#include <msvapi.h> // MMsvSessionObserver
#include "MMsgMailAttachmentDialogObserver.h"

// FORWARD DECLARATIONS
class CMsgMailViewerDocument;

// CLASS DECLARATION

/**
*  Dialog for attachments.
*  Mail attachments are handled from this dialog.
*/
class CMsgMailViewerAttachmentsDlg :public CMsgViewAttachmentsDialog, 
    public MMsvSessionObserver
    {
    public:  // Constructors and destructor
        /**
        * NewL
        * @param aTitle Dialog title text
        * @param aAttachmentModel a reference to attachment model
        * @param aDocument a reference to app document.
        */
        static CMsgMailViewerAttachmentsDlg* NewL(
        	TDesC& aTitle,
            CMsgAttachmentModel& aAttachmentModel, 
            CMsgMailViewerDocument& aDocument,
            MMsgMailAttachmentDialogObserver& aDlgObserver
            );

        /**
        * Destructor
        */
        ~CMsgMailViewerAttachmentsDlg();
        
        /**
        * NotifySizeChanged, calls SizeChanged() method of
        * dialog's listbox
        * 
        */
        void NotifySizeChanged();
        
        /**
        * Updates all attachments fetched flags
        * in attachment model.
        * 
        */
        void UpdateAttachmentsFetchFlags();
        
        /**
        * From MEikListBoxObserverClass
        */
        void HandleListBoxEventL( 
            CEikListBox* aListBox, TListBoxEvent aEventType );
        
        /**
        * Updates list of attatchments if attatchment 
        * dialog is opened before all attatchments 
        * are loaded.
        */
        void UpdateAttatchmentListL();

    private: // Implementation
        void UpdateChangedAttachmentsL(
            const CMsvEntrySelection& aChangedEntries );
		void FetchRemoteAttachmentL( TInt aIndex );
        void UpdateModelInfoL( TInt aIndex );
        void OpenAttachmentL();
        void DoFetchOrOpenL( TInt aIndex );
        void DoProcessCommandL( TInt aCommandId );
        void DoSendViaL();
        void PurgeAttachmentL( TInt aIndex );
        TBool CanRemoveAttachment( CMsgAttachmentInfo& aInfo );
        void SetMiddleSoftKeyLabelL( 
            TInt aResourceId, 
            TInt aCommandId );
        void UpdateMiddleSoftKeyL( TInt aIndex );   
        
         
    private: // Constructor
        CMsgMailViewerAttachmentsDlg(
            TDesC& aTitle,
            CMsgAttachmentModel& aAttachmentModel, 
            CMsgMailViewerDocument& aDocument,
            MMsgMailAttachmentDialogObserver& aDlgObserver );
        /**
        * 2nd phase constructor
        */
        void ConstructL();

    private: // Functions from base classes
        /// from CAknDialog
        void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);
        /// From CCoeControl
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);
        /// from CAknDialog
        void ProcessCommandL(TInt aCommandId);
        /// From CCoeControl
		void GetHelpContext( TCoeHelpContext& aContext) const;
        /// From MMsvSessionObserver
        void HandleSessionEventL(TMsvSessionEvent aEvent, 
            TAny* aArg1, TAny* aArg2, TAny* aArg3);
        //From CEikDialog
        TBool OkToExitL(TInt aButtonId);

    private:    // Data
        /**
        * A reference to the application Document.
        */
        CMsgMailViewerDocument& iDocument;
       
        /// Internal status flags
        TInt iFlags;

        /**
        * Ref: application UI object.
        */
        MMsgMailAttachmentDialogObserver& iDlgObserver;
    };

#endif      // MSGMAILVIEWATTACHMENTS_H

// End of File
