/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*           Implements MMS service and sending settings handler
*
*/



#ifndef MMSSETTINGSHANDLER_H
#define MMSSETTINGSHANDLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <barsc.h>

#include <AknProgressDialog.h>  // MProgressDialogCallback

#include <mmssettingsdialog.h>
#include <mmssettings.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CMmsClientMtm;
class CAknWaitDialog;

// CLASS DECLARATIONS
/**
 * Class CMmsSettingsHandler
 * Creates a handler class for handling mms settings. This
 * class is used from mtmui-classes to get access to mms settings.
 *
 * Note: It is expected that the context is set to MMS service
 * object before calling!
 *
 * @since 2.5
 */
class CMmsSettingsHandler : public CBase, 
                            public MProgressDialogCallback
    {
    public:

        /**
         * Constructor
         */
        IMPORT_C static CMmsSettingsHandler* NewL( CMmsClientMtm& aMmsClient );

        /**
         * Destructor
         */
        IMPORT_C ~CMmsSettingsHandler();

        /**
        * Checks if ap exists
        * @param aAp, access point id
        */
        IMPORT_C TBool ApExistsL( TInt32 aAp );
        
        /**
        * Checks whether or not MMS service settings are OK. Open settings
        * dialog if not.
        * @param aExitCode settings dialog exit code
        * @return are settings ok or not
        */
        IMPORT_C TBool CheckSettingsL( CMmsSettingsDialog::TMmsExitCode& aExitCode );

        /**
        * Creates completed operation after settings dialog closing with
        * appropriate parameters.
        * @param aCompletionStatus the operation progress
        * @param aExitCode settings dialog exit code
        * @return completed operation
        */
        IMPORT_C CMsvOperation* CompleteSettingsOperationL(
            TRequestStatus& aCompletionStatus,
            const CMmsSettingsDialog::TMmsExitCode& aExitCode );

        /**
        * Factory settings initializing function
        * @param aLevel normal or deep
        */
        IMPORT_C void HandleFactorySettingsL( TDes8& aLevel );

        /**
        * Actual settings dialog launcher
        * @param aExitCode out: the exit code causing dialog to close
        */
        IMPORT_C void LaunchSettingsDialogL(
            CMmsSettingsDialog::TMmsExitCode& aExitCode );

        /**
        * Opens settings dialog for the MMS service.
        * @param aCompletionStatus follows the operation progress
        * @return started operation
        */
        IMPORT_C CMsvOperation* OpenServiceSettingsDialogL(
            TRequestStatus& aCompletionStatus );
            
        inline CMmsSettings* MmsSettings();

        /**
        * Reloads the settings from Central Repository
        */
        inline void RefreshSettingsL( );

        /**
        * From MProgressDialogCallback
        * Wait dialog has died
        */
        void DialogDismissedL( TInt aButtonId );

    private:
        
        /**
        * Handles restoring MMS service default values
        * @param aLevel, level of restore: normal or deep
        */
        void RestoreDefaultSettingsL( TMmsFactorySettingsLevel aLevel ) const;

        CMmsSettingsHandler( CMmsClientMtm& aMmsClient );

        /**
        * Symbian OS constructor.
        */
        void ConstructL();

    private:

        CMmsClientMtm&              iMmsClient;
        CMmsSettings*               iMmsSettings;
        TInt                        iResourceOffset;
        CAknWaitDialog*             iWaitDialog;
        
    };

inline CMmsSettings* CMmsSettingsHandler::MmsSettings()
    {
    return iMmsSettings;
    }

inline void CMmsSettingsHandler::RefreshSettingsL( )
    {
    iMmsSettings->LoadSettingsL( );
    }
    
#endif      // MMSSETTINGSHANDLER_H

// End of File
