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
*     Pop agent implementation
*
*/

#ifndef __ALWAYSONLINEPOPAGENT_H__
#define __ALWAYSONLINEPOPAGENT_H__

#include "AlwaysOnlineEmailAgentBase.h"

#include <mtclreg.h>//ClientMtmRegistry
#include <popcmtm.h>//popclientmtm
/**
* CAlwaysOnlinePopAgent 
*
* Class takes care of POP mailbox "always" online logic
*/
class CAlwaysOnlinePop3Agent : public CAlwaysOnlineEmailAgentBase
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
        * @return CAlwaysOnlinePop3Agent*, self pointer
        */
        static CAlwaysOnlinePop3Agent* NewL( CMsvSession& aSession,
            CClientMtmRegistry& aClientMtmRegistry,
            TMsvId aMailboxId,
            MAlwaysOnlineStatusQueryInterface& aAlwaysOnlineManager,
            CAlwaysOnlineEmailAgent& aEmailAgent);

        /**
        * ~CAlwaysOnlinePop3Agent
        * Destructor
        */
        ~CAlwaysOnlinePop3Agent();

    private:
        /**
        * CAlwaysOnlinePop3Agent
        * @param CMsvSession&, session reference
        * @param CClientMtmRegistry&, mtm registry reference
        * @param TMsvId, mailbox id
        * @param MAlwaysOnlineStatusQueryInterface&, status query interface
        * @param CAlwaysOnlineEmailAgent&, email agent reference
        */
        CAlwaysOnlinePop3Agent( CMsvSession& aSession,
            CClientMtmRegistry& aClientMtmRegistry,
            MAlwaysOnlineStatusQueryInterface& aAlwaysOnlineManager,
            CAlwaysOnlineEmailAgent& aEmailAgent );

        /**
        * ConstructL
        * @param TMsvId, mailbox id
        * Second phase constructor
        */
        void ConstructL( TMsvId aMailboxId );

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
        * MailHasBeenSyncedL
        * Function to check has mailbox been synced before.
        * This is used to determine which sync method we should use.
        * @return TBool, has mailbox been synced
        */
        TBool MailHasBeenSyncedL();

        /**
         * 
         *
         * @since S60 v3.2 
         * @return 
         * @leave KErrNoMemory
         */
        TImPop3PopulateOptions FillMailOptionsL();
        
        
    private:

        /**
        * StartWaitTimerL
        * starts interval waiter. Does not handle schedule waits, only polling interval
        */
        void StartWaitTimerL();

        /**
        * CreatePop3OperationL
        * aFunctionId ignored and completed operation is created if aCompletedOperation is true.
        * @param TMsvOp&, this operation id is set in this function
        * @param TInt, function Id
        * @param TBool, ETrue if we want completed operation instead of real one
        */
        void CreatePop3OperationL( 
            TMsvOp& aOpId, TInt aFunctionId, TBool aCompletedOperation = EFalse );

        /**
        * aFunctionId ignored and completed operation is created if aCompletedOperation is true.
        * @param TInt, function Id
        * @param TBool, ETrue if we want completed operation instead of real one
        */
        void CreatePop3OperationL( 
            TInt aFunctionId, TBool aCompletedOperation = EFalse );

        /**
        *
        *
        */
        void DoFilteredPopulateL();

        //data
    private:
        CPop3ClientMtm* iPop3ClientMtm;
        TBool           iHasBeenSynced;

    };
    
    
#endif
//EOF
