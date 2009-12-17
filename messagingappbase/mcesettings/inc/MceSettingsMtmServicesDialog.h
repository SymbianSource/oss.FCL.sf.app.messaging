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
*     Dialog for handling services of certain mtm (if possible to create services).
*
*/



#ifndef __MCESETTINGSMTMSERVICESDIALOG_H__
#define __MCESETTINGSMTMSERVICESDIALOG_H__

//  INCLUDES

#include "MceSettingsTitlePaneHandlerDialog.h"
#include <ImumInternalApi.h>
#include <ImumInHealthServices.h>
class CMceSettingsMultipleServicesDialog;

// CONSTANTS
const TInt KMceMaxNumberOfMailboxes = 6;

// FORWARD DECLARATIONS

class MMceSettingsAccountManager;
class CMceUidNameArray;

// CLASS DECLARATION

class CMceSettingsMtmServicesDialog :
    public CMceSettingsTitlePaneHandlerDialog,
    public MMsvSessionObserver
    {
    public:

        /**
        * Constructor.
        */
        CMceSettingsMtmServicesDialog (
            TUid aMessageType,
            MMceSettingsAccountManager& aManager,
            CMsvSession* aSession,
            CMceSettingsMultipleServicesDialog& aParent );

        /**
        * Destructor.
        */
        virtual ~CMceSettingsMtmServicesDialog();

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
        * From CAknDialog
        */
        void ProcessCommandL( TInt aCommandId );

        /**
        * From CAknDialog
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );               

        /**
        * From CEikDialog
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,TEventCode aType );

        /**
        * MMsvSessionObserver
        */
        void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3 );

        /**
        * From CoeControl
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;        

        /**
        * Updates services array
        */
        virtual void UpdateServicesArrayL(); 

    protected:

        /**
        * From CEikDialog
        */
        virtual void PreLayoutDynInitL();

    private:

        /** 
        * Launches help application
        */
        void LaunchHelpL() const;

        /**
        * Set special MSK button
        * @since S60 3.2
        */
        void SetSpecialMSKButtonL(TInt aResourceId);        
  
        /**
         * Updates the internal mailbox data information
         *
         * @since S60 v3.2
         * @leave KErrNoMemory
         */
        void UpdateMailboxDataL(); 
        
    private: // Data
        TUid                    iMtmType;     
        MMceSettingsAccountManager& iAccountManager;
        CMsvSession*            iSession;       

        CMceUidNameArray*       iAccountArray;
        TBool                   iSelectionKeyPressedToOpenOptionMenu;
        TBool                   iSessionObserverAdded;
        // Owned: Pointer to email API object
        CImumInternalApi*       iEmailApi;
        // Array of mailbox id's
        MImumInHealthServices::RMailboxIdArray iMailboxArray;      
        TBool                   iMailboxCreationOn;
        TInt                    iNumberOfMailboxes;
        
        // Not owned: Only to call that we are closed
        CMceSettingsMultipleServicesDialog&  iParentDialog;
        TMsvId                  iCurrentMailAccountId;
    };

#endif      // MCESETTINGSMTMSERVICESDIALOG_H

// End of File
