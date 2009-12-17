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
*     Message centre's general settings dialog.
*
*/



#ifndef MCESETTINGSGENERALSETTINGSDIALOG_H
#define MCESETTINGSGENERALSETTINGSDIALOG_H

//  INCLUDES
#include <ConeResLoader.h>
#include <StringLoader.h>   // StringLoader
#include "MceSettingsTitlePaneHandlerDialog.h"


// FORWARD DECLARATIONS
class CMuiuSettingsArray;
class MMceSettingsAccountManager;


// CLASS DECLARATION

/**
*  Message centre's general settings dialog.
*/
class CMceGeneralSettingsDialog :
    public CMceSettingsTitlePaneHandlerDialog
    {
    public:  // Constructors and destructor

        /**
        * Constructor.
        */        
        CMceGeneralSettingsDialog( 
            CMsvSession* aSession, 
            MMsvSessionObserver& aObserver,
            MMceSettingsAccountManager* aManager
         );       

        /**
        * Constructor.
        */
        void ConstructL(TInt aResource );

        /**
        * Destructor.
        */
        virtual ~CMceGeneralSettingsDialog();

        /**
        * 
        */
        IMPORT_C static TInt CreateAndExecuteL(
            CMsvSession* aSession, MMsvSessionObserver& aObserver );

        /**
        * From CAknDialog
        */
        void ProcessCommandL( TInt aCommandId );

        /**
        * From CoeControl        
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

        /**
        * From MEikMenuObserver
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * Starts copying/moving message store functionality
        */
        void MoveMessageStoreL( TInt currentDrive, TInt driveNumber, TBool aDeleteCopiedStore );
        
        /**
        * Set error when error happens during msg store copying/moving process
        */
        void SetTransferError( TInt aError );

        /**
        * Checks, if there is connection
        */
        TBool CheckIsServerBusyL();

        /**
        * Gives message store path in aPath, when drive number is got
        *  @param aDrive                 drive number
        *  @param aPath                  message store path in source
        */
        void FindSourcePathL( TInt aDrive, TFileName& aPath ) const;

        /**
        * Gives drive letter and drive delimiter in aPath, when drive number is got
        *  @param aDrive                 drive number
        *  @param aPath                  message store path in target
        */
        void GetMessageDriveL( TInt aDrive, TDes& aPath ) const;

        /**
        * Sets correct string in General settings memory in use -option
        */
        void UpdateMemorySelectionL();

        /**
        * Closes sms, mms and mail applications before changing message store
        */
        TBool ShutdownAppsL() const;

        /**
        * Handles Memory in Use option
        */
        void HandleMemoryInUseDialogL(); 
        
        /**
        * Changes the drive of the message store
        */
#ifdef RD_MULTIPLE_DRIVE
        void ChangeMessageStoreL(TInt aTargetDrive );
#else
        void ChangeMessageStoreL();
#endif //RD_MULTIPLE_DRIVE

    protected:  // Functions from base classes

        /**
        * From CEikDialog
        */
        virtual void PreLayoutDynInitL();

        /**
        * From CEikDialog
        */
        virtual TBool OkToExitL( TInt aButtonId );

        /**
        * From MEikListBoxObserver
        */
        void HandleListBoxEventL(
            CEikListBox* aListBox,
            TListBoxEvent aEventType );

    private:

        /**
        * Launches help application
        */
        void LaunchHelpL() const;

        /**
        * Sets the label for the middle soft key
        */
        void SetMSKButtonL();

#ifdef RD_MULTIPLE_DRIVE
        /*
        * Sets the string for result of drive selection
        */
        HBufC* MakeDriveNameStringLC( TInt aDriveNumber );
#endif // RD_MULTIPLE_DRIVE

    private:  // Data
        CMuiuSettingsArray*     iMceSettingsArray;      
        CMsvSession*            iSession;       
        MMsvSessionObserver&    iObserver;

        TFileName               iSourcePath;
        TFileName               iTargetPath;
        TBool                   iMemoryInUse;
        TBool                   iNewMailIndicator;
        friend class CMceMoveProgress;
#ifdef RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
        RConeResourceLoader     iResources;
#endif // RD_MESSAGING_GENERAL_SETTINGS_RENOVATION
        MMceSettingsAccountManager* iAccountManager;
        TInt                     iTransferError;
    };

#endif      // MCESETTINGSGENERALSETTINGSDIALOG_H

// End of File
