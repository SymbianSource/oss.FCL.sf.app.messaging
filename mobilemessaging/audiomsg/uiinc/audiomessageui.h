/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   UI part of the AudioMessage MTM type to Symbian OS Messaging
*
*/



#ifndef AUDIOMESSAGEUI_H
#define AUDIOMESSAGEUI_H

#include <mtmuibas.h>
#include <MuiuMsgEditorLauncher.h>
#include <mmsconst.h>
#include "mmssettingshandler.h"

enum TEditorType
    {
    EReadOnly,
    EEditExisting,
    ECreateNewMessage,
    ESpecialEditor
    };

/**
 *
 *  UI part of the AudioMessage MTM type to Symbian OS Messaging
 *
 *  @lib audiomessageui.lib
 *  @since S60 v3.1
 */
class CAudioMessageUi : public CBaseMtmUi
    {
    public:  // Constructors and destructor
        
        /**
         * Symbian OS constructor
         * @param aBaseMtm client MTM
         * @param aRegisteredMtmDll registry dll
         * @return a new AMS MTM UI object.
         */
        static CAudioMessageUi* NewL(
            CBaseMtm& aBaseMtm,
            CRegisteredMtmDll& aRegisteredMtmDll );
        
        /**
         * Destructor.
         */
        ~CAudioMessageUi();
        
    public: // Functions from base classes
        
        /**
         * From CBaseMtmUi: Create a new entry. Launches MMS editor.
         * Asynchronous.
         * @param aEntry entry to be created
         * @param aParent parent entry
         * @param aStatus follows the operation progress
         * @return started operation
         */
        CMsvOperation* CreateL(
            const TMsvEntry& aEntry,
            CMsvEntry& aParent,
            TRequestStatus& aStatus );
        
        /**
         * From CBaseMtmUi:
         * Not supported (leaves with KErrNotSupported)
         */
        CMsvOperation* CancelL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );
        
        /**
         * From CBaseMtmUi: 
         * Not supported (leaves with KErrNotSupported)
         */
        CMsvOperation* OpenL( TRequestStatus& aStatus );
        
        /**
         * From CBaseMtmUi:
         * Not supported (leaves with KErrNotSupported)
         */
        CMsvOperation* OpenL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );
        
        /**
         * From CBaseMtmUi: not supported in Series 60
         */
        CMsvOperation* CloseL( TRequestStatus& aStatus );
        
        /**
         * From CBaseMtmUi: not supported in Series 60
         */
        CMsvOperation* CloseL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );
        
        /**
         * From CBaseMtmUi: Opens MmsEditor for editing a message. If
         * service entry opens MMS Settings dialog for editing MMS settings.
         * @param aStatus follows the operation progress
         * @return started operation
         */
        CMsvOperation* EditL( TRequestStatus& aStatus );
        
        /**
         * From CBaseMtmUi: As the other EditL(), but gets the first entry in
         * a selection and edits it.
         */
        CMsvOperation* EditL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );
        
        /**
         * From CBaseMtmUi: 
         * Not supported (leaves with KErrNotSupported)
         */
        CMsvOperation* ViewL( TRequestStatus& aStatus );
        
        /**
         * From CBaseMtmUi:
         * Not supported (leaves with KErrNotSupported)
         */
        CMsvOperation* ViewL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );
        
        /**
         * From CBaseMtmUi:
         * Not supported (leaves with KErrNotSupported)
         */
        CMsvOperation* ReplyL(
            TMsvId aDestination,
            TMsvPartList aPartlist,
            TRequestStatus& aCompletionStatus );
        
        /**
         * From CBaseMtmUi: 
         * Not supported (leaves with KErrNotSupported)
         */
        CMsvOperation* ForwardL(
            TMsvId aDestination,
            TMsvPartList aPartList,
            TRequestStatus& aCompletionStatus );
        
    protected:  // New functions
    
        /**
         * Handles the actual launching of editor when editing a message.
         * @param aStatus follows the operation progress.
         * @param aSession Message server session to be used.
         * @param aEditorType Editor type used to launch the correct application. 
         *                    Defaults to EEditExisting.
         * @return started operation
         */        
        CMsvOperation* LaunchEditorApplicationL(
            TRequestStatus& aStatus,
            CMsvSession& aSession,
            TEditorType aEditorType = EEditExisting );
    
    protected:  // Functions from base classes
    
        /**
         * From CBaseMtmUi: Returns resource file
         * @param OUT aFileName Store resource file here
         */
        void GetResourceFileName( TFileName& aFileName ) const;
    
    private:
    
        /**
         * From CBaseMtmUi: construction.
         * @param
         * @return
         */
        void ConstructL();
    
        /**
         * From CBaseMtmUi: constructor.
         * @param aBaseMtm client MTM reference
         * @param aRegisteredMtmDll Registry DLL
         * @return
         */
        CAudioMessageUi( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll );
    
        /**
         * Opens settings dialog for the MMS service.
         * @param aCompletionStatus follows the operation progress
         * @return started operation
         */
        CMsvOperation* OpenServiceSettingsDialogL(
            TRequestStatus& aCompletionStatus );
    
        /**
         * Checks whether or not MMS service settings are OK. Open settings
         * dialog if not.
         * @param aExitCode settings dialog exit code
         * @return are settings ok or not
         */
        TBool CheckSettingsL( CMmsSettingsDialog::TMmsExitCode& aExitCode ) const;
    
        /**
         * Creates completed operation after settings dialog closing with
         * appropriate parameters.
         * @param aCompletionStatus the operation progress
         * @param aExitCode settings dialog exit code
         * @return completed operation
         */
        CMsvOperation* CompleteSettingsOperationL(
            TRequestStatus& aCompletionStatus,
            const CMmsSettingsDialog::TMmsExitCode& aExitCode = CMmsSettingsDialog::EMmsExternalInterrupt );
    
        /**
         * Actual settings dialog launcher
         * @param aExitCode out: the exit code causing dialog to close
         */
        void LaunchSettingsDialogL(
            CMmsSettingsDialog::TMmsExitCode& aExitCode ) const;

        /**
         * Checks if ap exists
         * @param aAp, access point id
         */
        TBool ApExistsL( TUint32 aAp );

        /**
         * Checks if there's enough diskspace
         * If there is, just returns;
         * If there is not, leaves with KErrDiskFull
         */
        void CheckDiskSpaceL( );

    private:    // Data
        /**
         * Pointer to settingshandler
         * Own.  
		 */
        CMmsSettingsHandler*    iSettingsHandler;
        TUid                    iAmsAppId;
    };
    
#endif      // AUDIOMESSAGEUI_H
    
   
