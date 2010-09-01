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
*       CNotMtmUi - UI part of the Notification MTM type
*       to Symbian OS Messaging
*
*/



#ifndef NOTMTMUI_H
#define NOTMTMUI_H

//  INCLUDES
#include <AknWaitDialog.h>          //Wait note
#include <mtmuibas.h>
#include <MuiuMsgEditorLauncher.h> // for launch flags
#include <MsvPrgReporter.h> // MMsvProgressDecoder

#include <mmsconst.h>

#include "MmsSettingsHandler.h"
#include "MmsSettingsDialog.h"

// CONSTANTS

// MACROS

// DATA TYPES
// Editor types used to launch the correct application
enum TEditorType
    {
    EReadOnly,
    EEditExisting,
    ECreateNewMessage
    };

// FUNCTION PROTOTYPES


// FORWARD DECLARATIONS
class CMmsSettingsHandler;

// CLASS DECLARATION

/**
* CNotMtmUi - UI part of the Notification MTM type to Symbian OS Messaging
*
* @since 2.5
*/
class CNotMtmUi : public CBaseMtmUi,
                  public MMsvProgressDecoder 
    {
    public:  // Constructors and destructor
        
        /**
        * Symbian OS constructor
        * @param aBaseMtm client MTM
        * @param aRegisteredMtmDll registry dll
        * @return a new Notification MTM UI object.
        */
        static CNotMtmUi* NewL(
            CBaseMtm& aBaseMtm,
            CRegisteredMtmDll& aRegisteredMtmDll );
        
        /**
        * Destructor.
        */
        ~CNotMtmUi();
        
    public: // Functions from base classes
        
        /**
        * From CBaseMtmUi: Capability check.
        */
        TInt QueryCapability( TUid aCapability, TInt& aResponse );
        
        /**
        * From CBaseMtmUi: not supported in Series 60
        */
        CMsvOperation* DeleteServiceL(
            const TMsvEntry& aService,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi:
        * Just leaves with "not supported".
        */
        CMsvOperation* CreateL(
            const TMsvEntry& aEntry,
            CMsvEntry& aParent,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi: not supported in Series 60
        */
        CMsvOperation* DeleteFromL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi:
        * Suspends sending of the selected messages (if possible)
        */
        CMsvOperation* CancelL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );
        
        /**
        * From CBaseMtmUi: Handles open request to the message or service
        * entry having context. Asynchronous.
        * @param aStatus follows the operation progress
        * @return started operation
        */
        CMsvOperation* OpenL( TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi: As the other OpenL() but gets the first entry
        * in a selection and opens it.
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
        * From CBaseMtmUi: Views the message or service entry having context.
        * Asynchronous.
        * Opens Notification Viewer or Settings dialog depending of the entry.
        * @param aStatus follows the operation progress
        * @return started operation
        */
        CMsvOperation* ViewL( TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi: As the other ViewL(), but gets the first entry in
        * a selection ands views it.
        */
        CMsvOperation* ViewL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );
        
        /**
        * From CBaseMtmUi: Opens MmsEditor for message Reply.
        * @param aStatus follows the operation progress
        * @return started operation
        */
        CMsvOperation* ReplyL(
            TMsvId aDestination,
            TMsvPartList aPartlist,
            TRequestStatus& aCompletionStatus );
        
        /**
        * From CBaseMtmUi: Opens Notification Viewer for forwarding message.
        * @param aStatus follows the operation progress
        * @return started operation
        */
        CMsvOperation* ForwardL(
            TMsvId aDestination,
            TMsvPartList aPartList,
            TRequestStatus& aCompletionStatus );
        
        /**
        * From CBaseMtmUi: Copy a message from outbox to remote service (=send)
        * Context should be in MTM folder/service.
        * @param aSelection messages to be copied/sent
        * @param aStatus follows the operation progress
        * @return operation started
        */
        CMsvOperation* CopyToL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi: Move a message from outbox to remote service (=send)
        * Context should be in MTM folder/service.
        * @param aSelection messages to be moved/sent
        * @param aStatus follows the operation progress
        * @return operation started
        */
        CMsvOperation* MoveToL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi: Copy a message from remote service to inbox.
        * Just leaves with "not supported".
        */
        CMsvOperation* CopyFromL(
            const CMsvEntrySelection& aSelection,
            TMsvId aTargetId,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi: Move a message from remote service to inbox.
        * Just leaves with "not supported".
        */
        CMsvOperation* MoveFromL(
            const CMsvEntrySelection& aSelection,
            TMsvId aTargetId,
            TRequestStatus& aStatus );
        
        /**
        * From CBaseMtmUi: Perform an asynchronous operation
        */
        CMsvOperation* InvokeAsyncFunctionL(
            TInt aFunctionId,
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aCompletionStatus,
            TDes8& aParameter );
        
        /**
        * From CBaseMtmUi: Perform a synchronous operation
        */
        void InvokeSyncFunctionL(
            TInt aFunctionId,
            const CMsvEntrySelection& aSelection,
            TDes8& aParameter );
        
        /**
        * From CBaseMtmUi: Used for display operation summary (usually completed operations).
        * This does nothing, because MsgErrorWatcher shows all MMS error messages.
        * Furthermore currently MMS engine does not return any sensible progress to report.
        */
        TInt DisplayProgressSummary( const TDesC8& aProgress ) const;

        /**
        * From MMsvProgressDecoder
        * decode progress
        * @param aProgress: progress buffer
        * @param aReturnString: String to be displayed to the user usually in the CMsvWrappableProgressDialog dialog.
        * @param aTotalEntryCount: total entries to be processes
        * @param aEntriesDone: entries processes so far
        * @param aCurrentEntrySize: total size of the entries to be processes
        *           used to display progress bar (for example mail size to be fetched is 42000 bytes)
        *           If for example deleting messges then this should be number of messages to be deleted
        * @param aCurrentBytesTrans: number of bytes received so far
        *           used to display progress bar (for example received bytes is 21000 bytes, then progress bar half done)
        *           If for example deleting messges then this should be number of messages to deleted so far
        * @param aInternal: used only in CImapConnectionOp
        */
        TInt DecodeProgress (
            const TDesC8& aProgress,
            TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans, TBool aInternal );


    public: // New functions

        /**
        * Checks whether or not MMS service settings are OK. Open settings
        * dialog if not.
        * @param aExitCode settings dialog exit code
        * @return are settings ok or not
        */
        IMPORT_C TBool CheckSettingsL( CMmsSettingsDialog::TMmsExitCode& aExitCode ) const;

        /**
        * Opens settings dialog for the MMS service.
        * @param aCompletionStatus follows the operation progress
        * @return started operation
        */
        IMPORT_C CMsvOperation* OpenServiceSettingsDialogL(
            TRequestStatus& aCompletionStatus );
    
        /**
        * Checks if ap exists
        * @param aAp, access point id
        */
        IMPORT_C TBool ApExistsL( TInt32 aAp );

        /**
        * Creates completed operation after settings dialog closing with
        * appropriate parameters.
        * @param aCompletionStatus the operation progress
        * @param aExitCode settings dialog exit code
        * @return completed operation
        */
        IMPORT_C CMsvOperation* CompleteSettingsOperationL(
            TRequestStatus& aCompletionStatus,
            const CMmsSettingsDialog::TMmsExitCode& aExitCode = CMmsSettingsDialog::EMmsExternalInterrupt );
    
        /**
        * Actual settings dialog launcher
        * @param aExitCode out: the exit code causing dialog to close
        */
        IMPORT_C void LaunchSettingsDialogL(
            CMmsSettingsDialog::TMmsExitCode& aExitCode ) const;

        /**
        * Factory settings initializing function
        * @param aLevel normal or deep
        */
        IMPORT_C void HandleFactorySettingsL( TDes8& aLevel );

        /**
        * This returns ETrue, if the delete options flag is defined.
        */
        IMPORT_C TBool DeleteQueryRequiredByMtm();

        /*
        * Shows user a query where the notifications should be deleted
        * Options are Server only and Both server and local. Returns false
        * if the query was cancelled, so first check the return value.
        */
        TInt DeleteFromQueryL( TInt& result );

        /**
        * Checks if AP settings are OK. If they are not OK, shows user
        * AP define dialog. This function is meant to be used by MMBox only.
        * @result ETrue - if settings are ok or user created new ok settings
        * @result EFalse - if settings were ok and user did not create settings
        */
        TBool CheckSettingsL( );

    protected:  // New functions

        /**
        * Handles the actual launching of editor or viewer when viewing or editing a 
        * message.
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

        /**
        * Performs CopyToL or MoveToL operation
        * @param aSelection the entries to be sent
        * @param aStatus follows the operation progress
        * @param aCopyOnly defines is it copy or move
        * @return started operation
        */
        CMsvOperation* CopyMoveToL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus,
            TBool aCopyOnly );
    
    
    protected:  // Functions from base classes
    
        /**
        * From CBaseMtmUi: Returns resource file
        * @param aFileName Store resource file here
        */
        void GetResourceFileName( TFileName& aFileName ) const;
    
    private:

         enum TNotUiFlags
            {
            // set by mms local variation bit KMmsFeatureIdDeleteOptions
            EDeleteOptionsSupported                         = 0x0001,

            // set when delete operation is started
            EDeletingFromMMBox                              = 0x0002,

            // set when delete operation is started
            EDeletingMany                                   = 0x0004,

            // set by feature flag KFeatureIdOfflineMode
            EOffline                                        = 0x0008

            };

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
        CNotMtmUi( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll );
    
        /**
        * Default C++ constructor
        */
        CNotMtmUi();

        /**
        * Does nothing - done in engine by FactorySettings
        */
        void CreateDefaultSettingsL();

        /**
        * Opens message info popup.
        * @param aStatus follows the operation progress
        * @param aParameter contains message size from Editor, otherwise empty
        * @return started operation
        */
        CMsvOperation* OpenMessageInfoL(
            TRequestStatus& aCompletionStatus,
            TDes8& aParameter );

        /**
        * Opens MMBox dialog.
        * @param aStatus follows the operation progress
        * @param aParameter contains message size from Editor, otherwise empty
        * @return started operation
        */
        CMsvOperation* OpenMMBoxDialogL(
            TRequestStatus& aCompletionStatus );

        /**
        * Starts fetching procedure. This is called by InvokeAsyncfunction
        * @param aSelection Selection of entries to be fetched
        * @param aCompletionStatus Follows the progress of the operation
        * @return started operation
        */
        CMsvOperation* StartFetchingL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aCompletionStatus );

        /**
        * Starts deleting procedure. This is called by InvokeAsyncfunction
        * @param aSelection Selection of entries to be deleted
        * @param aCompletionStatus Follows the progress of the operation
        * @return started operation
        */
        CMsvOperation* StartDeletingL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aCompletionStatus );

            /**
        * Creates "To" recipient list for Message Info
        * @param aRecip     IN  Recipient array
        * @return New recipient string in CleanupStack
        */
        HBufC* TurnRecipientsArrayIntoStringLC( const CMsvRecipientList& aRecip ) const;
            
        /**
        * Message size - integer to string converter
        * Converts message size in bytes to a kilobyte string. Rounds the bytes up to the
        * next full kilo. I.e:
        * 0 -> 0KB
        * 1 -> 1KB
        * 1024 -> 1KB
        * 1025 -> 2KB
        * @param aTarget returned size string
        * @param aFileSize source size
        */
        void MessageSizeToStringL( TDes& aTarget, TInt aFileSize ) const; 
    
        /**
        * ShowConfirmationQueryL
        * Shows confirmation query with question aResourceId
        * @param IN aResourceId id of the confirmation resource
        */
        TInt ShowConfirmationQueryL( TInt aResourceId ) const;

        /**
        * Shows wait note using CAknWaitDialog.
        * @param aNoteTxtResourceId
        * @param aVisibilityDelayOff, ETrue show note immediately, 
        *                             EFalse after 1.5 sec
        */
        void ShowWaitNoteLC(
            TInt aNoteTxtResourceId, 
            TBool aVisibilityDelayOff);

        /**
        * Construct and prepares wait note using CAknWaitDialog.
        * This is called from ShowWaitNoteLC variants.
        * @param aVisibilityDelayOff, ETrue show note immediately, 
        *                             EFalse after 1.5 sec
        */
        void ConstructWaitNoteLC( TBool aVisibilityDelayOff );

        /**
        * To handle waitdialog PopAndDestroy destruction from cleanupstack.
        * @param 
        */ 
        static void CleanupWaitDialog(TAny* aAny);

        /**
        * Checks if query needed -> if there are more than one entry
        * or if one that has not been deleted from server yet.
        * This is used only locally by the MTM.
        */
        TBool DeleteQueryReallyNeededL( const CMsvEntrySelection& aSelection );

        /**
        * ShowInformationNoteL
        * Shows information note for user
        * @param aResourceID
        * @param aWaiting, is note waiting or not.
        */  
        void ShowInformationNoteL(TInt aResourceID, TBool aWaiting );


    private:    // Data
        CMmsSettingsHandler*    iSettingsHandler;

        // Used to show waiting dialog while deleting messages from server
        CAknWaitDialog*         iWaitDialog;

        TUint                   iNotUiFlags;

    };
    
#endif      // NOTMTMUI_H
    
    // End of File
    
