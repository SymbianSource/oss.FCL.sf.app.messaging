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
*     Imap agent implementation
*
*/

#ifndef __ALWAYSONLINEIMAPAGENT_H__
#define __ALWAYSONLINEIMAPAGENT_H__

#include "AlwaysOnlineEmailAgentBase.h"
#include "AlwaysOnlineImap4FolderObserver.h"
#include "AlwaysOnlineEmailFlags.h"

#include <mtclreg.h>//ClientMtmRegistry
#include <impcmtm.h>//imapclientmtm
#include <imapconnectionobserver.h>



enum
    {
    EAlwaysOnlineImap4ThisHasBeenSynced = 0x01,
    EAlwaysOnlineImap4StayOnline = 0x02,
    EAlwaysOnlineImap4StartFetchAfterFolderSync = 0x04,
    EAlwaysOnlineImap4DontReconnect = 0x08,
    EAlwaysOnlineImap4RemoveMeImmediately = 0x10,
    EAlwaysOnlineImap4TemporaryConnectionObserver = 0x20,
    EAlwaysOnlineImap4DontRemoveOnDisconnect = 0x40
    };


/**
* CAlwaysOnlineImap4Agent 
*
* Class takes care of IMAP mailbox "always" online logic
*/
class CAlwaysOnlineImap4Agent : 
    public CAlwaysOnlineEmailAgentBase,
    public MMsvImapConnectionObserver,
    public MAlwaysOnlineImap4FolderObserver
    {
    public:

    
        /**
        * NewL
        * Two phased constructor
        * @param CMsvSession&, session reference
        * @param CClientMtmRegistry&, mtm registry reference
        * @param TMsvId, mailbox id
        * @param MAlwaysOnlineStatusQueryInterface&, status query interface
        * @param CAlwaysOnlineEmailAgent&, email agent reference
        * @return CAlwaysOnlineImap4Agent*, self pointer
        */
        static CAlwaysOnlineImap4Agent* NewL( CMsvSession& aSession,
            CClientMtmRegistry& aClientMtmRegistry,
            TMsvId aMailboxId,
            MAlwaysOnlineStatusQueryInterface& aAlwaysOnlineManager,
            CAlwaysOnlineEmailAgent& aEmailAgent );

        /**
        * ~CAlwaysOnlineImap4Agent()
        * Destructor
        */
        ~CAlwaysOnlineImap4Agent();

        //from CAlwaysOnlineEmailAgentBase
    public:

        /**
        * StartL
        * from CAlwaysOnlineEmailAgentBase
        */
        void StartL();

        /**
        * ConnectAndUpdateHeadersL
        * from CAlwaysOnlineEmailAgentBase
        */
        void ConnectAndUpdateHeadersL( );

        
        /**
        * DisconnectL
        * from CAlwaysOnlineEmailAgentBase
        * @param TBool, automatic or manual disconnection
        */
        void DisconnectL( TBool aAutomatic );


        /**
        * ResumeL
        * Need to override this to update EAlwaysOnlineImap4StayOnline flag
        * from CAlwaysOnlineEmailAgentBase
        */
        void ResumeL( const TBool aConnectNow = EFalse );
        

        /**
        * HandleImapConnectionEvent
        * From MMsvImapConnectionObserver
        * @param TImapConnectionEvent, imap connection event
        */
        virtual void HandleImapConnectionEvent(
           TImapConnectionEvent aConnectionEvent);

        /**
        * From MAlwaysOnlineImap4FolderObserver
        */
    	virtual void HandleFolderEntryL( const CMsvEntrySelection& aFolderArray );

        /**
        *
        */
        void SetUpdateMailWhileConnectedL( TBool aAgentAlreadyCreated );

    protected:
        /**
        * ChangeNextStateL
        * from CAlwaysOnlineEmailAgentBase
        */
        void ChangeNextStateL();

        /**
        * HandleOpErrorL
        * from CAlwaysOnlineEmailAgentBase
        */
        void HandleOpErrorL();

       /**
        * DoSyncDisconnectL(), from emailagentbase
        * Does sync disconnect. Meant to be called from ResetAll().
        * This is needed because this must not have imapact on state machine.
        */
        void DoSyncDisconnectL();

        /**
        * HandleOpCompleted
        * from CAlwaysOnlineEmailAgentBase
        * @param TMsvOp, id of completed operation
        * @param, TInt, completion code
        */
        void HandleOpCompleted( TMsvOp opId, TInt aCompletionCode );

        /**
        * CreateCompletedOpL
        * from CAlwaysOnlineEmailAgentBase
        */
        void CreateCompletedOpL();

        /**
        * SynchroniseNewL()
        */
        void SynchroniseNewL();

        
    private:

        /**
        * CAlwaysOnlineImap4Agent
        * C++ constructor
        * @param CMsvSession&, session reference
        * @param CClientMtmRegistry&, mtm registry reference
        * @param TMsvId, mailbox id
        * @param MAlwaysOnlineStatusQueryInterface&, status query interface
        * @param CAlwaysOnlineEmailAgent&, email agent reference
        */
        CAlwaysOnlineImap4Agent( CMsvSession& aSession,
            CClientMtmRegistry& aClientMtmRegistry,
            MAlwaysOnlineStatusQueryInterface& aAlwaysOnlineManager,
            CAlwaysOnlineEmailAgent& aEmailAgent);

        /**
        * ConstructL
        * @param TMsvId, mailbox id
        */
        void ConstructL( TMsvId aMailboxId );

        /**
        * StartWaitTimerL
        * starts interval waiter. Does not handle schedule waits, only polling interval
        */
        void StartWaitTimerL();

        /**
        * CreateImap4OperationL
        * aFunctionId ignored and completed operation is created if aCompletedOperation is true.
        * @param TMsvOp&, this operation id is set in this function
        * @param TInt, function Id
        * @param TBool, ETrue if we want completed operation instead of real one
        */
        void CreateImap4OperationL( 
            TMsvOp& aOpId, TInt aFunctionId, TBool aCompletedOperation = EFalse );

        /**
        * CreateImap4OperationL
        * aFunctionId ignored and completed operation is created if aCompletedOperation is true.
        * @param TInt, function Id
        * @param TBool, ETrue if we want completed operation instead of real one
        */
        void CreateImap4OperationL( 
            TInt aFunctionId, TBool aCompletedOperation = EFalse );

        /**
        * FetchNewMailL
        */
        TMsvOp FetchNewMailL();

        /**
        * FindInboxL
        * @return TMsvId, folder if of inbox
        */
        TMsvId FindInboxL();

        /**
        * StartFolderSyncL()
        * Starts imap folder sync and sets timer to repeat
        */
        void DoFolderSyncL();

        
        /**
        * StartFolderSyncTimerL()
        * Starts timer for folder sync.
        */
        void StartFolderSyncTimerL();

        /**
        * Updates iFolderArray member
        * @since 2.6
        */
        void UpdateSubscribedFoldersL( 
            TMsvId aServiceId );

        /**
        * Updates iFolderArray member
        * @since 2.6
        */
        void DoUpdateSubscribedFoldersL( 
            TMsvId aFolderId );

        /**
        * Appends iFolderArray (except aInboxId) to given folder selection
        * @since 2.6
        * @param aFolders:
        * @param aInboxId
        */
        void AppendFolderArrayL( CMsvEntrySelection& aFolders, TMsvId aInboxId );

        /**
        *
        * @since 2.6
        */
        void StartFolderObserverL();

        /**
        *
        * @since 2.6
        */
        void StopFolderObserver();

        /**
        * Starts folder populate operation for the given folders 
        * @since 2.6
        * @param aFolder: folders to be populated
        * @return operation id
        */
        TMsvOp FetchNewMailFromSelectedFoldersL( const CMsvEntrySelection& aFolders );


        /**
        * Sets flag for whether to stay online or disconnect.
        */
        void SetStayOnlineFlagL();

        /**
        * Is agent temporary?
        * @since S60 3.1
        * @return ETrue, if EAlwaysOnlineImap4TemporaryConnectionObserver
        * flag is set.
        */
        TBool IsTemporary() const;

        /**
         * Fills mail fetching options
         *
         * @since S60 v3.2 
         * @return  mail fetching options
         */
        TImImap4GetPartialMailInfo FillMailOptionsL();
        
        //data
    private:
        CImap4ClientMtm*                    iImap4ClientMtm;
        CAlwaysOnlineImap4FolderObserver*   iFolderObserver; // at the moment only inbox, should we add others also?
        CMsvEntrySelection*                 iFolderArray; // subscribed folders and last fetched mail
        TMsvId                              iInboxId;
        TAlwaysOnlineEmailFlags             iImap4StateFlags;
    };


#endif
//EOF
