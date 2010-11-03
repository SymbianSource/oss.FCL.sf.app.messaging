/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Message centre's mail dialog.
*     Includes "mtm mailboxes" and "mtm mailbox in use" setting items.
*
*/



#ifndef __MCESETTINGSMULTIPLESERVICESDIALOG_H__
#define __MCESETTINGSMULTIPLESERVICESDIALOG_H__

//  INCLUDES
#include <AiwServiceHandler.h>
#ifdef RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
#include <ConeResLoader.h>
#endif // RD_MESSAGING_GENERAL_SETTINGS_RENOVATION

#include <msvapi.h>
#include <ImumInHealthServices.h>
#include "MceSettingsTitlePaneHandlerDialog.h"


// FORWARD DECLARATIONS
class MMceSettingsAccountManager;
class CMceSettingsMultipleServicesDialog;
class CImumInternalApi;
class TUidNameInfo;
class CUidNameArray;

// CLASS DECLARATION

/**
*  Message centre's mail dialog.
*  Includes "remote mailboxes" and "Mailbox in use" setting items.
*/
class CMceSettingsMultipleServicesDialog :
    public CMceSettingsTitlePaneHandlerDialog,
    public MMsvSessionObserver

    {
    public:  // Constructors and destructor

        /**
        * Constructor.
        */
        CMceSettingsMultipleServicesDialog( 
            MMceSettingsAccountManager& aManager, TUid aMessageType, CMsvSession* aSession );

        /**
        * Destructor.
        */
        virtual ~CMceSettingsMultipleServicesDialog();

        /**
        *
        */
        IMPORT_C static TInt CreateAndExecuteL(
            MMceSettingsAccountManager& aManager,
            TUid aMessageType,
            CMsvSession* aSession );

    public: // new methods
       
        /**
         * Launches the dialog
         *
         * @since S60 v3.2
         * @param aManager         
         * @param aMessageType
         * @param aSession
         * @param aMenuResource
         * @param aDialogResource
         * @leave KErrNoMemory
         */
        static void LaunchDialogL( 
            MMceSettingsAccountManager& aManager, 
            const TUid& aMessageType, 
            CMsvSession& aSession,
            const TInt& aMenuResource,
            const TInt& aDialogResource );          
    
        /**
        * Launches new dialog for editing mailboxes.
        */
        void EditServicesL();        

        /**
        * Handles entries created session event
        * Called from HandleSessionEventL
        * @param aSelection: pointer to created entries
        */
        void HandleEntriesCreatedL( CMsvEntrySelection* aSelection );
        
        /*
        * Informs that subdialog is closed
        */
        void SubdialogClosed();

    public: // Functions from base classes
        /**
        * From CAknDialog
        */
        void ProcessCommandL( TInt aCommandId );

        /**
        * From MEikMenuObserver
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * MMsvSessionObserver
        */
        void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3 );

        /**
        * From CoeControl
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;        

        /**
        * Sets the flag to indicate that the MTM services subdialog
        * is not open
        */
        static void ResetMtmServicesDialog(TAny* aAny);

    protected:  // Functions from base classes

        /**
        * From CEikDialog
        */
        virtual void PreLayoutDynInitL();

        /**
        * From CEikDialog
        */
        virtual TBool OkToExitL(TInt aButtonId);

        /**
        * From MEikListBoxObserver
        */
        virtual void HandleListBoxEventL(
            CEikListBox* aListBox,
            TListBoxEvent aEventType );
        
        /**
         * Handles this application view's command keys. Forwards other keys.
         *
         * @return EKeyWasConsumed if the keyboard event was consumed or
         *         EKeyWasNotConsumed otherwise.
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

    private:
        /**
         * Symbian constructor
         *
         * @since S60 v3.2
         */
        void ConstructL(TInt aResource );
  
        /**
         * Symbian constructor
         *
         * @since S60 v3.2
         */
        void ConstructL();
        
        /**
         * Updates the internal mailbox data information
         *
         * @since S60 v3.2
         * @leave KErrNoMemory
         */
        void UpdateMailboxDataL(); 
    
        /**
        * Launches setting page "Mailbox in use"
        * to select mailbox to be used for sending by default.
        */
        void ShowListBoxSettingPageL();

        /**
        * Sets mailbox in use and sets up correct array
        * for this dialog to be displayed.
        */
        void SetAccountInUseL();

        /**
        * Should be called if no mailboxes defined.
        * Displayes query to create new mailbox and then creates one.
        * @return ETrue if user created mailbox, EFalse if they didn't.
        */
        TBool HandleNoMailboxesL();
        
        /** 
        * Launches help application
        */
        void LaunchHelpL() const;

        /** 
        * Sets the label of the MSK button for the dialog
        */
        void SetMSKButtonL();

        /** 
		* Lists email accounts
        * @return Returns array of email accounts
        */
		CMsvEntrySelection* GetEmailAccountsL( CMsvSession& aSession );
		
        /** 
		* Lists email accounts
        * @return Returns array of email accounts
        */
		CUidNameArray* MtmEmailAccountsL();

        /** 
		* Updates arrays
        */
		void UpdateServicesArrayL();
		
        /**
        * Creates new IMAP/POP mailbox.
        * @return ETrue if mailbox created,
        */		
		TBool CreateNewImapPopL();	
    
        /**
        * Launches a setup wizard.
        */
        TBool LaunchWizardL();
    
	private:
		enum EActiveMenuBar {
			EMultipleServices,
			EMtmServices
			};
    private:  // Data
        MMceSettingsAccountManager& iManager;
        TUid                    iMessageType;
        CDesCArrayFlat*         iSettingsItemArray;
        TBool                   iSessionObserverAdded;
        CMsvSession*            iSession;
        TBool                   iStartMailboxCreationWaiter;
        TBool                   iSubDialogForceClosed;
        // Owned: Pointer to email API object
        CImumInternalApi*       iEmailApi;
        // Array of mailbox ids
        MImumInHealthServices::RMailboxIdArray iMailboxArray;       
        TInt                    iNumberOfMailboxes;
#ifdef RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
        RConeResourceLoader     iResources;
#endif // RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
        TBool                   iSubDialogOpen;
        CUidNameArray*			iAccountArray;
        TInt					iActiveMenuBar;
        CAiwServiceHandler* iAiwServiceHandler;
        TBool iS60WizLaunched;
        TBool iProductIncludesSelectableEmail;
        TBool iMceIntegration;
		// Id of the email application (other than platform email application)
        TInt iMtmPluginId;
    };

#endif      // __MCESETTINGSMULTIPLESERVICESDIALOG_H__

// End of File
