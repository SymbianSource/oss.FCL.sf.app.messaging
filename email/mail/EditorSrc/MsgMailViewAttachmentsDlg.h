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
* Description:  This dialog is used for attachment handling.
*
*/


#ifndef MSGMAILVIEWATTACHMENTS_H
#define MSGMAILVIEWATTACHMENTS_H

//  INCLUDES
#include <MsgViewAttachmentsDialog.h>

// FORWARD DECLARATIONS
class CMsgMailEditorDocument;
class CMsgMailEditorAppUi;

// CLASS DECLARATION

/**
*  Dialog for attachments.
*  Mail Editor attachments are handled from this dialog.
*/
class CMsgMailViewAttachmentsDlg :
	public CMsgViewAttachmentsDialog
    {
    public:  // Constructors and destructor

        /**
        * NewL()
        * @param aTitle Dialog title text
        * @param aAttachmentModel a reference to attachment model
        * @param aAppUi a reference to MsgMailEditorAppUi
        * @param aHelpSupported ETrue if help supported 
        */
        static CMsgMailViewAttachmentsDlg* NewL(TDesC& aTitle,
            CMsgAttachmentModel& aAttachmentModel, CMsgMailEditorAppUi& aAppUi,
            TBool aHelpSupported);
        
        /**
        * Destructor.
        */
        ~CMsgMailViewAttachmentsDlg();
        
    private: // Constructor
        /**
        * Constructor.
        * @params see NewL()
        * @param aMenuId Dialog menu resource Id
        */
        CMsgMailViewAttachmentsDlg(
            TDesC& aTitle, TInt aMenuId,
            CMsgAttachmentModel& aAttachmentModel, CMsgMailEditorAppUi& aAppUi);


    private: // Functions from base classes
        /**
        * From CAknDialog
        */
        void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);

        /**
        * From CCoeControl
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

        /**
        * From CAknDialog
        */
        void ProcessCommandL(TInt aCommandId);

		/**
		* @param aContext Help context reference for returning current context
		*/
		void GetHelpContext( TCoeHelpContext& aContext) const;

        /**
        * From MEikListBoxObserver
        */
        void HandleListBoxEventL( CEikListBox* aListBox, 
            TListBoxEvent aEventType );

    private: // implementation

        void OpenAttachmentL();
        void DoShowSelectionListL();

    private:    // Data
        /**
        * Ref: application UI object.
        */
        CMsgMailEditorAppUi& iAppUi;
        
        /**
        * Feature flag
        */
        TBool iHelpSupported;
        
    };

#endif      // MSGMAILVIEWATTACHMENTS_H

// End of File
