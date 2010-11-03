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
*     Email agent base class. common functions and data
*
*/


#ifndef __ALWAYSONLINEEMAILAGENTBASE_H__
#define __ALWAYSONLINEEMAILAGENTBASE_H__

#include <e32base.h>
#include <msvapi.h>
#include <MuiuMsvSingleOpWatcher.h>
#include <miutset.h>
#include <AlwaysOnlineManagerCommon.h>
#include <MNcnInternalNotification.h>
#include <muiuflagger.h>
#include <cmconnectionmethod.h>
#include <cmmanager.h>

#include "AlwaysOnlineEmailLoggingTools.h"
const TInt KAOMaxRetries = 3;

const TInt KAOMaxRetriesSwitchOff = 5*KAOMaxRetries;

const TInt KAOUsernameLength = 50;

//reconnect intervals in seconds
enum TAgentReconnectIntervals
    {
    EEMailAgentReconnectOne = 10, //10 seconds
    EEMailAgentReconnectTwo = 20, //20
    EEMailAgentReconnectThree = 30, //30
    EEMailAgentReconnectFour = 40, //40 secs
    EEMailAgentReconnectFive = 60, //one minute
    EEMailAgentReconnectSix = 300 //five minutes
    };

enum TAgentScheduleWaitStates
    {
    EWaitForStop = 0,
    EWaitForStart
    };

enum TAlwaysOnlineEmailIAPNotes
    {
    EEmailAgentCSDIAP,
    EEmailAgentInvalidInterval,
    EEmailAgentOutOfDisk,
    EEmailAgentEMNFatalError,
    EEmailAgentOtherInvalidity
    };

enum TMailSettingType
    {
    EMailboxSettings = 0,
    EAlwaysonlineSettings
    };

// Always Online features
enum TAlwaysOnlineLocalFeatures
    {
    // KEmailFeatureIdAlwaysOnlineCSD
    EAOLFAlwaysOnlineCSD = 0,
    // KEmailFeatureIdEmailVPNAllowed
    EAOLFAlwaysOnlineVPN
    };


// Always Online flags
enum TAlwaysOnlineBaseFlags
    {
    // Flag to indicate when the connection should be made
    EAOBFConnectNow = 0,
    // Flag to indicate the protocol of the current mailbox
    EAOBFIsImap4,
    // Last Base Flag
    EAOBFLastFlag
    };

const TInt KInitWaitSeconds = 5;
const TInt KErrAoServerNotFound = -5120;

//constants
class CClientMtmRegistry;
class CMsvSession;
class MAlwaysOnlineStatusQueryInterface;
class CAlwaysOnlineEmailAgent;
class CImumInternalApi;
class CImumInSettingsData;

/**
* CAlwaysOnlineEmailAgentBase
*
* Base class for IMAP and POP mailbox agents.
* Agents contain the logic to keep mailboxes up to date.
* One agent per mailbox.
*/
class CAlwaysOnlineEmailAgentBase : public CBase, public MMsvSingleOpWatcher
    {
    public:

        /**
        * ~CAlwaysOnlineEmailAgentBase
        * Destructor
        */
        virtual ~CAlwaysOnlineEmailAgentBase();

        /**
        * StartL
        */
        virtual void StartL() = 0;

        /**
        * ConnectL
        * Calls ConnectAndUpdateHeadersL if mailbox not connected, if already connected then calls UpdateHeadersWhenAlreadyConnectedL
        */
        virtual void ConnectL();

        /**
        * ConnectAndUpdateHeadersL
        *
        */
        virtual void ConnectAndUpdateHeadersL( ) = 0;


        /**
        * DisconnectL
        * @param TBool, is disconnection automatic or user initiated.
        */
        virtual void DisconnectL( TBool aAutomatic ) = 0;

        /**
        * MailboxId
        * @return TMsvId, mailbox id
        */
        TMsvId MailboxId() const;

        /**
        * Mailbox type
        * @since S60 v3.1
        * @return TBool
        */
        inline TBool IsImap4() const;

        /**
        * Gets email address
        * @param aEmailAddress will point to email address after function call
        * @since S60 v3.1
        */
        void EmailAddress( TDes& aEmailAddress );

        /**
        * Gets username
        * @param aUsername will point to username after function call
        * @since S60 v3.1
        */
        void Username( TDes8& aUsername );

        /**
        * Gets incoming server
        * @param aServer will point to incoming server after function call
        * @since S60 v3.1
        */
        void ServerAddress( TDes& aServer );

        /**
         * Loads a value of single setting
         *
         * @since S60 v3.2
         * @param
         * @leave Any Imum Internal API leave code
         */
        template<class T>
        inline T LoadSettingL(
            const TUint aKey,
            const TBool aConnectionSetting ) const;


        /**
        * Is mail agent EMN or not?
        * @since S60 v3.1
        * @return TBool if EMN is in automatic or in home network only states.
        */
        inline TBool IsEmn() const;

        /**
        * Suspend
        * Called when plugin receives offline event ( network operations not allowed )
        */
        virtual void Suspend();

        /**
        * ResumeL
        * Called when plugin receives offline event ( network operations allowed )
        * @since Series60 2.6
        * @param aConnectNow When true, connection is started to mailbox
        */
        virtual void ResumeL( const TBool aConnectNow = EFalse );

        /**
        * HandleHomeNetworkEventL
        * Called when plugin receives HomeNetwork event
        */
        void HandleHomeNetworkEventL();

        /**
        * HandleRoamingEventL
        * Called when plugin receives Roaming event
        */
        void HandleRoamingEventL();

        /**
        * HandleOutOfDiskEventL
        * Called when plugin receives out of disk event
        */
        void HandleOutOfDiskEventL();

        /**
        * SwitchOffL
        * Will switch off this mailagent
        */
        void SwitchOffL();

    public:
        /**
        * OpCompleted
        * From MMsvSingleOpWatcher
        * @param CMsvSingleOpWatcher&, opwathcer, which just completed
        * @param TInt, completion code
        */
        virtual void OpCompleted(
            CMsvSingleOpWatcher& aOpWatcher,
            TInt aCompletionCode );

    protected:

        /**
        * CAlwaysOnlineEmailAgentBase
        * C++ Constructor
        * @param CMsvSession&, session reference
        * @param CClientMtmRegistry&, mtm registry reference
        * @param TMsvId, mailbox id
        * @param MAlwaysOnlineStatusQueryInterface&, status query interface reference
        * @param CAlwaysOnlineEmailAgent&, email agent reference
        */
        CAlwaysOnlineEmailAgentBase( CMsvSession& aSession,
            CClientMtmRegistry& aClientMtmRegistry,
            MAlwaysOnlineStatusQueryInterface& aAlwaysOnlineManager,
            CAlwaysOnlineEmailAgent& aEmailAgent );

        /**
        * ConstructL
        * @param TMsvId, mailbox id
        */
        void ConstructL( TMsvId aMailboxId );

        /**
        * AppendWatcherAndSetOperationL
        * Appends aWatcher to iOperations array and sets watcher operation to aOperation.
        * NOTE: Takes ownership of aWatcher and aOperation so those MUST be removed from CleanupStack
        * immediately after this function call
        * @param CMsvSingleOpWatcher*, opwatcher to be appended
        * @param CMsvOperation*, operation to be wathced
        */
        void AppendWatcherAndSetOperationL( 
            CMsvSingleOpWatcher* aWatcher, CMsvOperation* aOperation );

        /**
        * ChangeNextStateL
        * Called by OpCompleted, need to decide what to do next...
        * The core of our state machine
        */
        virtual void ChangeNextStateL() = 0;

        /**
        * HandleOpErrorL
        * Called in ChangeNextStateL() if error is detected in OpCompleted()
        */
        virtual void HandleOpErrorL() = 0;

        /**
        * StartTimerOperationL
        * @param TTime&, time reference
        * @param TMsvOp& reference of operation id which will be set in this function
        */
        virtual void StartTimerOperationL( const TTime& aTime, TMsvOp& aOpId );

        /**
        * DoSyncDisconnectL()
        * Does sync disconnect. Meant to be called from ResetAll().
        * This is needed because this must not have imapact on state machine.
        */
        virtual void DoSyncDisconnectL() = 0;

        /**
        * Is agent temporary?
        * @since S60 3.1
        * @return ETrue, if temporary (default is ETrue )
        */
        virtual TBool IsTemporary() const;

        /**
        * CheckAndHandleSchedulingL
        * Checks schedules and starts timers accordingly
        */
        void CheckAndHandleSchedulingL();

        /**
        * ResetAll
        * Resets all operations and their saved IDs
        */
        void ResetAll();

        /**
        * StartScheduleWaiterL
        * Does the actual starting of timers and does the wait calculating
        * @param TBool, ETrue if we're waiting for stop ( chedule is ok )
        */
        void StartScheduleWaiterL( const TBool aShouldConnect );

        /**
        * HandleOpCompleted
        * If completed operation requires special handling, it is taken care of in here
        * @param TMsvOp, id of the completed operation
        * @param TInt, completion code
        */
        virtual void HandleOpCompleted( TMsvOp opId, TInt aCompletionCode ) = 0;

        /**
        * CreateCompletedOpL
        * Function to create a completed operation. Needed to switch state when no other operation needs to be created.
        */
        virtual void CreateCompletedOpL() = 0;

        /**
        * SetLastSuccessfulUpdate
        * Function to set last successful update time.
        */
        void SetLastSuccessfulUpdate();

        /**
        * SetLastUpdateFailed
        * Function to set last update failed.
        */
        void SetLastUpdateFailed();

        /**
        * DisplayGlobalErrorNoteL
        * Shows global error note in case of an fatal error
        * @param TAlwaysOnlineEmailIAPNotes, id of fail type
        */
        void DisplayGlobalErrorNoteL( 
            TAlwaysOnlineEmailIAPNotes aInvalidity=EEmailAgentOtherInvalidity );

        /**
        * IsBearerCSDL
        * Checks if our bearer is CSD or not
        * @return TBool, ETrue if bearer is circuit switched
        */
        TBool IsBearerCSDL();


        /**
        * Creates SingleOpWatcher operation and places it to cleanup stack
        */
        CMsvSingleOpWatcher* CreateSingleOpWatcherLC();

        /**
        * Handles default errors
        * @since S60 v3.0
        */

        void HandleDefaultError();

        /**
        * Handles Email notification connection related errors
        * @since S60 v3.1
        * @return TBool, ETrue if we should reconnect
        */
        TBool IsReconnectAfterError();
        
        
        /**
         * Converts TImumDaSettings::TAutoIntervalValues into minutes.
         * @since S60 3.2
         * @param aIntervalIndex TAutoIntervalValues Interval
         * @return Interval in minutes
         */
        TInt RetrievalIntervalInMinutes( TInt aIntervalIndex );

    protected:


        /**
        * QueryAndHandleAOServerInfoL
        * Queries offline and roaming info from AO server
        * and takes action accordingly
        * @param TBool&, parameter is set in this function
        */
        void QueryAndHandleAOServerInfoL( TBool& aSuspended );

        /**
        * @since S60 v3.1
        * @return ETrue if agent has home network only selected
        */
        TBool CheckIfAgentHomeOnly();

        /**
        * SwitchAutoUpdateOffL()
        **/
        void SwitchAutoUpdateOffL();

        /**
        * IsIAPInvalidL
        * Checks accesspoint settings. Password prompt and IAP type and other validity
        * @param TAlwaysOnlineEmailIAPNotes&, id of invalidity. Set in this function
        * @return TBool, ETrue if AP is invalid
        */
        TBool IsIAPInvalidL( TAlwaysOnlineEmailIAPNotes& aInvalidity );

        /**
        * GetConnectionMethodLC
        * Leaves the Connection Method into cleanupstack if found
        * @return RCmConnectionMethod The used connection method
        * @leave System wide leave codes
        */
        RCmConnectionMethod GetConnectionMethodLC();

        /**
        * RemoveMe
        * Tells email agent to remove this plugin
        */
        void RemoveMe();

        /**
        * Make an connection check and determine scheduling
        * @since Series60 2.6
        * @param aOperation, Operation that must exist
        */
        void ConnectIfAllowedL( const TMsvOp& aOperation );

        /**
        * Tell to Ncn that there might be new messages to show
        * @since Series60 3.0
        */
        void CallNewMessagesL();

        /**
        *
        * @since Series60 3.0
        */
        void CleanOperation( TMsvOp& aOpId );

        /**
        * Cancels the ongoing operations. The method should be called in the beginning
        * of the destructors of the subclasses. No errors returned, because the method
        * is called at the shutdown phase where no reasonable error handling is possible
        * anymore.
        * @since S60 3.0
        */
        void CloseServices();

        /**
         * Gets the current state of the Always Online
         *
         * @since S60 v3.2
         * @param Key to be used
         * @return
         */
        TInt AoState() const;

        /**
         * Gets the current state of the Always Online
         *
         * @since S60 v3.2
         * @param Key to be used
         * @return
         */
        TInt EmnState() const;

    private:

        /**
        * Try to create completed operation
        * @since Series60 2.6
        * @param aOpId Id of the given operation
        * @param aOp Actual operation
        * @param aUid Used MTM Uid
        */
        void HandleCompletingOperation(
            const TMsvOp& aOpId,
            CMsvOperation& aOp,
            const TUid& aUid );

        /**
        *
        * @since Series60 3.0
        */
        void LaunchStartTimerL(
            TTime& aClock,
            const TTimeIntervalSeconds& aSeconds );

        /**
        *
        * @since Series60 3.0
        */
        void LaunchStopTimerL(
            TTime& aClock,
            const TTimeIntervalSeconds& aSeconds );

        /**
        *
        * @since Series60 3.0
        */
        CMuiuFlags* AlwaysOnlineFlagsL();

    //data
    protected:
        CClientMtmRegistry& iClientMtmRegistry;
        CMsvSession& iSession;
        CMsvEntry* iEntry; // Entry of the mailbox we are polling
        CMsvSingleOpWatcherArray iOperations;

        enum TAlwaysOnlineEmailAgentStates
            {
            EEmailAgentInitialised = 0,
            EEmailAgentIdle, //( 1 )
            EEmailAgentConnecting, //( 2 )
            EEmailAgentDoingFilteredPopulate, //( 3 )
            EEmailAgentPlainConnecting, //( 4 )
            EEmailAgentConnectingToStayOnline, //( 5 )
            EEmailAgentSynchronising, //( 6 )
            EEmailAgentFetching, //( 7 )
            EEmailAgentAutoDisconnecting, //( 8 )
            EEmailAgentUserDisconnecting, //( 9 )
            EEmailAgentQueued, //( 10 )
            EEmailAgentTimerWaiting, //( 11 )
            EEmailAgentTimerWaitingForStart, //( 12 )
            EEmailAgentTimerWaitingForStop, //( 13 )
            EEmailAgentReconnecting, //( 14 )
            EEmailAgentConnectFailed, //( 15 )
            EEmailAgentFatalError,//( 16 )
            EEmailAgentConnTerminated //( 17 )
            };


        TAlwaysOnlineEmailAgentStates iState;

        TInt iRetryCounter;

        TInt iLoggerFileCounter; // delete logger file time to time so that it does not grow too much...
        // Owned: Pointer to email API object
        CImumInternalApi*               iMailboxApi;
        // Owned: Pointer to email settings object
        CImumInSettingsData*            iMailboxSettings;

        //These ids are needed by our state machine. We need to know which operation is completing
        //to take appropriate action
        TMsvOp iIntervalWaitId;//operation id for interval timer
        TMsvOp iConnectSyncOpId;//operation id for connect and sync operation

        TMsvOp iConnectOpId;//operation id for plain connection operation
        TMsvOp iSyncOpId;//operation id for separate header sync
        TMsvOp iWaitForStartOpId;//operation id for schedule wait timer ( waiting for start )
        TMsvOp iWaitForStopOpId;//operation id for schedule wait timer ( waiting for stop )
        TMsvOp iStartDelayOpId;//operation id for start delay timer. Once set, never reset.
        TMsvOp iConnectAndStayOnlineOpId; //operation id for connection after which we stay online.
        TMsvOp iDisconnectOpId;//operation id for disconnection
        TMsvOp iFolderUpdateTimerOpId;//operation id for imap folder update timer.
        TMsvOp iFolderSyncOpId;//op id for folder sync operation
        TMsvOp iFilteredPopulateOpId;//op id for filtered email population ( partial fetch )

        TInt iError;

        MNcnInternalNotification* iNcnNotification;

        MAlwaysOnlineStatusQueryInterface& iStatusQueryInterface;

        CAlwaysOnlineEmailAgent& iEmailAgent;
        // Object to handle and store the flags
        CMuiuFlags*         iFlags;

        // For handling access point settings
        RCmManager			iCmManager;
    };

#include "AlwaysOnlineEmailAgentBase.inl"

typedef CArrayPtrFlat<CAlwaysOnlineEmailAgentBase> CAOEmailAgentArray;

#endif
//EOF
