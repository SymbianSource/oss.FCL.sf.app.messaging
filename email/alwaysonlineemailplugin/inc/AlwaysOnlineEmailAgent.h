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
*     Agent which holds an array of email protocol specific agents
*
*/


#ifndef __ALWAYSONLINEEMAILAGENT_H__
#define __ALWAYSONLINEEMAILAGENT_H__

//INCLUDES
#include <e32base.h>
#include <msvapi.h>
#include <MuiuMsvSingleOpWatcher.h>
#include <imapconnectionobserver.h>
#include <AlwaysOnlineManagerCommon.h>

#include "AlwaysOnlineEmailAgentBase.h"


//constants
const TInt KEmailAgentOperationsGranularity = 5;

//forward declarations
class MAlwaysOnlineStatusQueryInterface;
class CAlwaysOnlineEmailEMNResolver;

/**
* CAlwaysOnlineEmailAgent
*
*/
class CAlwaysOnlineEmailAgent : public CBase
    {
    public:

        /**
        * CAlwaysOnlineEmailAgent
        * @param CMsvSession&, CMsvSession reference
        * @param MAlwaysOnlineStatusQueryInterface&, status query interface reference
        */
        CAlwaysOnlineEmailAgent(
            CMsvSession& aMsvSession,
            MAlwaysOnlineStatusQueryInterface& aAlwaysOnlineManager );


        /**
        * ~CAlwaysOnlineEmailAgent()
        */
        ~CAlwaysOnlineEmailAgent();

        /**
        * NewL
        * @param CMsvSession&, CMsvSession reference
        * @param MAlwaysOnlineStatusQueryInterface&, status query interface reference
        * @return CAlwaysOnlineEmailAgent*, self pointer
        */
        static CAlwaysOnlineEmailAgent* NewL(
            CMsvSession&                        aMsvSession,
            MAlwaysOnlineStatusQueryInterface&  aAlwaysOnlineManager );

        /**
        * StartL
        */
        void StartL();

        /**
        * SuspendNWOperationsL
        */
        void SuspendNWOperationsL();

        /**
        * ResumeNWOperationsL
        */
        void ResumeNWOperationsL();

        /**
        * RoamingEventL
        */
        void RoamingEventL();

        /**
        * HomeNetworkEventL
        */
        void HomeNetworkEventL();

        /**
        * HandleOutOfDiskL
        */
        void HandleOutOfDiskL();

        /**
        * HandleEMNMessageL
        * @since Series60 3.0
        * @param aParameter
        */
        void HandleEMNMessageL( const TDesC8& aParameter );

        /**
        * ActivateOnlineMailboxesL
        */
        void ActivateOnlineMailboxesL();

        /**
        * GetMailboxesLC
        * @return CMsvEntrySelection*, all mailboxes
        */
        CMsvEntrySelection* GetMailboxesLC();

        /**
        * RefreshMailAgentArrayL
        */
        void RefreshMailAgentArrayL();

        /**
        * MailboxSettingsChangedL
        * @param TMsvId, mailbox id
        */
        void MailboxSettingsChangedL( TMsvId aMailboxId );

        /**
        * RemoveMailAgentL
        */
        void RemoveMailAgentL();

        /**
        * SwitchOffAllAgentsL()
        * Will turn off all mailbox agents.
        */
        void SwitchOffAllAgentsL();

        /**
        * Turns on/off the AlwaysOnline or set it to be removed
        * @since Series60 2.6
        * @param aCommand Command used to handle the AO
        * @param aParameter
        */
        void HandleAOStateL(
            const TInt aCommand,
            const TDesC8& aParameter );

    private:

        /**
        * ConstructL
        */
        void ConstructL();

        /**
        * FindMailbox
        * @param TMsvId, mailbox id
        * @return CAlwaysOnlineEmailAgentBase*, returns email agent for given mailbox. NULL if not found
        */
        CAlwaysOnlineEmailAgentBase* FindMailbox( TMsvId aMailboxId );

    public:

        /**
        * AddMailboxL
        * @param TMsvId, mailbox id
        * @return CAlwaysOnlineEmailAgentBase* returns email agent for given mailbox.
        * Can return NULL if mailbox not imap4 or pop3 or mailbox not found at all
        */
        CAlwaysOnlineEmailAgentBase* AddMailboxL( TMsvId aMailboxId );


        /**
        * DeleteMailbox
        * @param TMsvId, mailbox id
        * @return TBool, deletion successful
        */
        TBool DeleteMailbox( TMsvId aMailboxId );


    private:

        /**
        * IsMailAgentActiveL
        * @param TMsvId, mailbox id
        * @return TBool, ETrue if mailbox has been set to be auto update or emn
        */
        TBool IsMailAgentActiveL( TMsvId aMailboxId );

        /**
        * Turns Always online on or off
        * @since Series60 2.6
        * @param aMailboxId
        * @param aState
        */
        void MailboxSetActivityL(
            const TMsvId aMailboxId,
            const TBool  aState );

        /**
        * Dispatches MsvId from parameter
        * @since Series60 2.6
        * @param aParameters Parameters
        * @param aId Parameter to store the id
        * @return KErrNone, if dispatching is ok
        */
        TInt ParameterDispatchMsvId(
            const TDesC8& aParameters,
            TMsvId&       aMailboxId ) const;

        void MailboxUpdateMailWhileConnectedL( const TMsvId aMailboxId );

    //data
    private:
        MAlwaysOnlineStatusQueryInterface& iAlwaysOnlineManager;

        CClientMtmRegistry* iClientMtmRegistry;
        CMsvSession& iSession;
        CAOEmailAgentArray* iMailAgentArray;
        CAlwaysOnlineEmailEMNResolver* iEmnResolver;

        };


#endif

//EOF
