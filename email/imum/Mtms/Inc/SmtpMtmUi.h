/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       SMTP MTM UI
*
*/

// MODULE - SMTP MTM UI

#ifndef __SMTPMTMUI_H__
#define __SMTPMTMUI_H__

#include <smtpset.h>
#include <AlwaysOnlineManagerClient.h>
#include "IMSSettingsUi.h"
#include "PROGTYPE.H"
#include "ImumMtmBaseMtmUi.h"   // CImumMtmBaseMtmUi
#include <msvapi.h>

// Forward references
class CErrorUI;
class CEmailPreCreation;
class CClientMtmRegistry;
class TEditorParameters;

/**
* CSmtpMtmUi - SMTP MTM UI
*/
class CSmtpMtmUi : public CImumMtmBaseMtmUi
    {
    public:
        enum TSmtpMtmUiOperations
            {
            ESmtpMtmUiEditing=EUiProgTypeEditing
            };

    private:
        enum TSmtpMtmUiCreateNewServiceParameters
            {
            ESmtpMtmUiCreateNewServiceSave,
            ESmtpMtmUiCreateNewServiceExit,
            ESmtpMtmUiCreateNewServiceSaveAndExit,
            ESmtpMtmUiCreateNewServiceNotCreated
            };

    public:
        /**
        *
        */
        static CSmtpMtmUi* NewL(
            CBaseMtm& aBaseMtm,
            CRegisteredMtmDll& aRegisteredMtmDll );

        /**
        * Destructor
        */
        virtual ~CSmtpMtmUi();

        /**
        * From CBaseMtmUi
        * Function NOT dependent on the current context
        */
        virtual CMsvOperation* CreateL(
            const TMsvEntry& aEntry,
            CMsvEntry& aParent,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi
        * Context-specific
        */
        virtual CMsvOperation* OpenL( TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi
        * Context-specific
        */
        virtual CMsvOperation* CloseL( TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi
        * Context-specific
        * Launches editor/settings dialog as appropriate
        */
        virtual CMsvOperation* EditL(TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context-specific
        * Launches viewer/settings dialog as appropriate
        */
        virtual CMsvOperation* ViewL(TRequestStatus& aStatus);

        /**
        * From CBaseMtmUi
        * Context-specific
        * Takes ownership of the passed entry (which the context is switched to )
        */
        virtual CMsvOperation* ReplyL(
            TMsvId aDestination,
            TMsvPartList aPartlist,
            TRequestStatus& aCompletionStatus );

        /**
        * From CBaseMtmUi
        * Context-specific
        * Takes ownership of the passed entry (which the context is switched to )
        */
        virtual CMsvOperation* ForwardL(
            TMsvId aDestination,
            TMsvPartList aPartList,
            TRequestStatus& aCompletionStatus );

        /**
        * From CBaseMtmUi
        * Context-specific
        * Selections must be in same folder and all of the correct MTM type
        * Context may change after calling these functions
        */
        virtual CMsvOperation* OpenL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection);

        /**
        * From CBaseMtmUi
        * Context-specific
        * Selections must be in same folder and all of the correct MTM type
        * Context may change after calling these functions
        */
        virtual CMsvOperation* CloseL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );

        /**
        * From CBaseMtmUi
        * Context-specific
        * Selections must be in same folder and all of the correct MTM type
        * Context may change after calling these functions
        * Calls EditL for the first item of the selection.
        */
        virtual CMsvOperation* EditL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );

        /**
        * From CBaseMtmUi
        * Context-specific
        * Selections must be in same folder and all of the correct MTM type
        * Context may change after calling these functions
        * Calls ViewL for the first item of the selection.
        */
        virtual CMsvOperation* ViewL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );

        /**
        * From CBaseMtmUi
        * Context-specific
        * Selections must be in same folder and all of the correct MTM type
        * Context may change after calling these functions
        * Cancels sending of the selection by creating object of
        * CSmtpCancelSendingOp class
        */
        virtual CMsvOperation* CancelL(
            TRequestStatus& aStatus,
            const CMsvEntrySelection& aSelection );

        /**
        * From CBaseMtmUi
        * Context-specific
        * Context should be MTM folder/service to copy to
        */
        virtual CMsvOperation* CopyToL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi
        * Context-specific
        * Context should be MTM folder/service to move to
        */
        virtual CMsvOperation* MoveToL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi
        * Context-specific
        * Context should be MTM folder/service to copy from
        */
        virtual CMsvOperation* CopyFromL(
            const CMsvEntrySelection& aSelection,
            TMsvId aTargetId,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi
        * Context-specific
        * Context should be MTM folder/service to move from
        */
        virtual CMsvOperation* MoveFromL(
            const CMsvEntrySelection& aSelection,
            TMsvId aTargetId,
            TRequestStatus& aStatus );

        /**
        * From CBaseMtmUi
        * Interpret transfer progress
        */
        virtual TInt DisplayProgressSummary(
            const TDesC8& aProgress ) const;

        /**
        * From CBaseMtmUi
        * Interpret transfer progress
        */
        virtual TInt GetProgress(
            const TDesC8& aProgress,
            TBuf<EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans ) const;

        /**
        * From CBaseMtmUi
        */
        virtual TInt QueryCapability( TUid aCapability, TInt& aResponse );

        /**
        * From CBaseMtmUi
        */
        virtual void InvokeSyncFunctionL(
            TInt aFunctionId,
            const CMsvEntrySelection& aSelection,
            TDes8& aParameter );

        /**
        * From CBaseMtmUi
        */
        virtual CMsvOperation* InvokeAsyncFunctionL(
            TInt aFunctionId,
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aCompletionStatus,
            TDes8& aParameter );

    protected:

        /**
        * From CBaseMtmUi
        */
        virtual void ConstructL();

        /**
        * From CBaseMtmUi
        */
        virtual void GetResourceFileName( TFileName& aFileName ) const;

    private:
        /**
        * C++ constructor
        */
        CSmtpMtmUi(CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll);

        /**
        *
        */
        CMsvOperation* LaunchSettingsDialogL( TRequestStatus& aStatus );

        /**
        *
        */
        CMsvOperation* LaunchEditorApplicationL(
            TRequestStatus& aStatus,
            TBool aReadOnly,
            TBool aNewMsg,
            CMsvSession& aSession,
            TMsvId aNewMsgServiceId = KMsvNullIndexEntryIdValue );

        /**
        * Explicit send of a message.
        */
        CMsvOperation* DoCopyMoveToL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aStatus,
            TBool aMoving );

        /**
        *
        */
        TMsvId CreateNewServiceL( const TMsvEntry& aEntry, TSmtpMtmUiCreateNewServiceParameters& aSaveParameters );

        /**
        *
        */
        TMsvId HandleNoAccountsL( TSmtpMtmUiCreateNewServiceParameters& aSaveParameters );

        /**
        *
        */
        void ResolveEditorFileNameL();

        /**
        *
        */
        void ResolveViewerFileNameL();

        /**
        * Finds out name of the remote mailbox
        */
        HBufC* GetMailboxNameL( TMsvId aServiceId ) const;

        /**
        *
        */
        TInt GetConnectionProgress(
            const TDesC8& aProgress,
            TBuf<EProgressStringMaxLen>& aReturnString ) const;

        /**
        * Draft implementation. Not used yet.
        * Creates predefined mailboxes on startup or restore factorysettings.
        */
        void HandlePredefinedMailboxCreationL( );

        /**
        * Shows message info dialog
        * Context should be the message which info is displayed.
        */
        CMsvOperation* ShowMessageInfoL(
            TRequestStatus& aCompletionStatus,
            TDes8& aParameter );

        /**
        * Checks if aServiceId exists. If not then checks if there are any default
        * services and if no services asks user to create one.
        * This is used before launching mail editor.
        * @param aServiceId: service id to be checked.
        *    Fills new service id if that service does not exists or new service is created.
        * @return ETrue if service exists, EFalse if no mail services
        */
        TBool CheckMailboxesBeforeEditL(
            TMsvId& aServiceId,
            TSmtpMtmUiCreateNewServiceParameters& aSaveParameters );

        /**
        * smtp auth fix 23092002
        * Checks if SMTP has username and password set. If not, they are copied from
        * mailbox ( pop or imap )
        * This function should be used only with smtp auth implementation where we use
        * the same username and password as in mailbox settings.
        * If and when SMTP has it's own u&p in settings, this function SHOULD be removed.
        */
        void CheckAndRestoreSmtpUserAndPasswordL( TMsvId aSmtpService );

        /**
        * Validate service function to be used by InvokeAsyncFunction
        * Checks that a default mailbox exists and that it has access point set.
        * @param TDes8& result of validation will be set into this parameter. KErrNone
        * or and error which occurred, usually KErrNotFound
        */
        void ValidateServiceL( TDes8& aParameter );

        /**
         * 
         * @since S60 3.2
         * @param aSelection
         * @param aParameter
         */
        void ValidateServiceL( 
            const CMsvEntrySelection& aSelection, 
            TDes8& aParameter );
            
        /**
         * 
         * @since S60 3.2
         * @param aMailboxId
         * @param aParameter
         */
        void DoValidateServiceL( 
            const TMsvId aMailboxId, 
            TDes8& aParameter );

        /**
        *
        */
        CMsvOperation* DoExitL(
            TRequestStatus& aStatus,
            const TBool aIssueQuit = ETrue );

        /**
        *
        * @since S60 3.0
        */
        TBool SettingsDialogExitL(
            CImumInSettingsData& aSettings,
            const TImumUiExitCodes& aExitCode );

        /**
        *
        * @since S60 3.0
        */
        void SettingsDialogExitSaveAndExitL(
            CImumInSettingsData& aSettings );

        /**
        *
        * @since S60 3.0
        */
        CMsvOperation* PrepareCommandL(
            TEditorParameters& aParameters,
            TMsvId aDestination,
            TMsvPartList aPartlist,
            TRequestStatus& aCompletionStatus );

        /**
        *
        * @since S60 3.0
        */
        CMsvOperation* CreateExecutePreCreationL(
            const TMsvId aSmtpService, 
            TRequestStatus& aStatus ) const;

        /**
        *
        * @since S60 3.0
        */
        CMsvOperation* CreateEmailServiceL(
            const TMsvEntry& aEntry,
            TRequestStatus& aStatus );

        /**
        *
        * @since S60 3.0
        */
        void ChangeUnreadFlagL(
            const TMsvId aEmailId,
            const TBool aNewState );
            
        /**
         * 
         * @since S60 3.2
         * @param 
         * @return 
         * @leave &(leavedesc)s
         */
        TSmtpMtmUiCreateNewServiceParameters SaveParams(
            const TImumUiExitCodes& aUiExitCode );
        
        /**
         * Activates rigth view after mailbox creation
         *
         * @since S60 3.2
         * @param aExitCode: how wizard exited and what view should be active
         * @param aId: the id of the mailbox that should be opened
         */
        void ActivateViewL(
            TImumUiExitCodes aExitCode,
            TMsvId aId );
        
        /**
         * Removes a screen clearer from the stack if a leave occurs.
         * @param aScreenClearer ScreenClearer pointer.
         */
        static void ScreenClearerCleanupOperation( TAny* aScreenClearer );

    private:
        // --- Data ---
        HBufC*                      iEditorFilename;
        HBufC*                      iViewerFilename;
        CEmailPreCreation*          iPreCreate;        
        TBool                       iNewSecureSettingOn;
        RAlwaysOnlineClientSession  iAOClient;        
    };

#endif // __SMTPMTMUI_H__
