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
*     Message centre's settings dialog.
*
*/



#ifndef MCESETTINGSDIALOG_H
#define MCESETTINGSDIALOG_H

//  INCLUDES

#include <ConeResLoader.h>
#include <msvapi.h>
#include "MceSettingsTitlePaneHandlerDialog.h"
#include <MsgArrays.h>


// FORWARD DECLARATIONS

class CMtmStore;
class CMtmUiDataRegistry;
class CMceSettingsSessionObserver;
class CMceUidNameArray;
class MMceSettingsAccountManager;

// CLASS DECLARATION

/**
*/
class CMceSettingsDialog :
    public CMceSettingsTitlePaneHandlerDialog

    {
    public:  // Constructors and destructor

        /**
        * Constructor.
        */
        IMPORT_C static CMceSettingsDialog* NewL();

        /**
        * Constructor for Mce
        */
        IMPORT_C static CMceSettingsDialog* NewL(
                CMceUidNameArray& aSettingsItemsArray,
                MMceSettingsAccountManager* aManager, 
                CMsvSession* aSession );       

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CMceSettingsDialog();

        /**
        * Executes CMceSettingsDialog
        */
        IMPORT_C TInt ExecuteLD( );
        

    public: // Functions from base classes

        /**
        * From CAknDialog
        */
        IMPORT_C void ProcessCommandL( TInt aCommandId );        

        /**
        * From MEikMenuObserver
        */
        IMPORT_C void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * From CoeControl        
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;

        /**
        * From MEikListBoxObserver
        */
        virtual void HandleListBoxEventL(
                CEikListBox* aListBox,
                TListBoxEvent aEventType );  

    protected:  // Functions from base classes

        /**
        * From CEikDialog
        */
        IMPORT_C virtual void PreLayoutDynInitL();

        /**
        * From CEikDialog
        */
        IMPORT_C virtual TBool OkToExitL( TInt aButtonId );             

    private:

        /**
        * Constructor.
        */
        CMceSettingsDialog();

        /**
        * Constructor.
        */
        CMceSettingsDialog( 
                CMceUidNameArray& aSettingsItemsArray,
                MMceSettingsAccountManager* aManager, 
                CMsvSession* aSession );      

        /**
        * Constructor.
        */
        void ConstructL();

        /**
        * Loads all mtms.
        */
        void LoadAllMtmsL();

        /**
        * Loads mtm
        * @param aMtmType: mtm type
        * @param aMtmStore: mtm store 
        * @param aUiRegistry: mtm ui data registry 
        */
        void LoadMtmL( const TUid aMtmType, 
                CMtmStore& aMtmStore, 
                const CMtmUiDataRegistry& aUiRegistry );

        /**
        * Open sub settings.
        */
        void OpenSubSettingsL();        

        /**
        * Changes array order: sms, mms, email, others...
        */
        void SortAndChangeSmsMmsEmailFirstL();

        /**
        * Places given mtm first in the array.
        * @param aArray: reference to array
        * @param aMsgType: mtm to changed to the first.
        */
        void ChangeMsgTypeTopL( CUidNameArray& aArray, TUid aMsgType ) const;

        /** 
        * Launches help application
        */
        void LaunchHelpL() const;

        /**
        * Checks CSP Cell Broadcast bit from SIM      
        */
        TBool CheckCspBitL() const;
    private:  // Data
    
        RConeResourceLoader     iResources;
        CMceUidNameArray*       iMsgTypesSettings;
        MMceSettingsAccountManager* iAccountManager;
        CMsvSession*            iSession; // not owed by this class.        
        TBool                   iLaunchedFromMce; 
        CMceSettingsSessionObserver* iSessionObserver;

        TBool                   iCbs;
        TBool 					iDialogOpen;// To verify dialog is already open
		TBool iProductIncludesSelectableEmail;
    };

#endif      // MCESETTINGSDIALOG_H

// End of File
