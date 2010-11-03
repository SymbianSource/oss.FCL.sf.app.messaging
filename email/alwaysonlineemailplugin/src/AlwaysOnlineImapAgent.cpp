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
*     Email imap agent
*
*/

#include <AlwaysOnlineStatusQueryInterface.h>
#include <SenduiMtmUids.h>
#include <es_sock.h>
#include <MuiuOperationWait.h>
#include <ImumInSettingsData.h>

#include "AlwaysOnlineEmailPluginTimer.h"
#include "AlwaysOnlineEmailPluginLogging.h"
#include "AlwaysOnlineEmailLoggingTools.h"
#include "AlwaysOnlineImapAgent.h"
#include "AlwaysOnlineImap4FolderPopulate.h"

const TInt KAlwaysOnlineMaxLimit = 30;  // if inbox sync rate is less than this and we are using gprs, then connection is kept open all the time.
const TInt KAlwaysOnlineTwoMinutes = 120;
const TInt KAlwaysOnlineMinuteToSecondsMultiplier = 60;


// ----------------------------------------------------------------------------
// CAlwaysOnlineImap4Agent()
// ----------------------------------------------------------------------------
CAlwaysOnlineImap4Agent::CAlwaysOnlineImap4Agent(
    CMsvSession& aSession,
    CClientMtmRegistry& aClientMtmRegistry,
    MAlwaysOnlineStatusQueryInterface& aAlwaysOnlineManager,
    CAlwaysOnlineEmailAgent& aEmailAgent )
    :
    CAlwaysOnlineEmailAgentBase(
        aSession,
        aClientMtmRegistry,
        aAlwaysOnlineManager,
        aEmailAgent ),
    iInboxId( 0 ),
    iImap4StateFlags()
    {
    }

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
CAlwaysOnlineImap4Agent* CAlwaysOnlineImap4Agent::NewL(
    CMsvSession& aSession,
    CClientMtmRegistry& aClientMtmRegistry,
    TMsvId aMailboxId,
    MAlwaysOnlineStatusQueryInterface& aAlwaysOnlineManager,
    CAlwaysOnlineEmailAgent& aEmailAgent )
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::IsReconnectAfterError" );
    CAlwaysOnlineImap4Agent* self = new(ELeave)CAlwaysOnlineImap4Agent(
        aSession,
        aClientMtmRegistry,
        aAlwaysOnlineManager,
        aEmailAgent );
    CleanupStack::PushL( self );
    self->ConstructL( aMailboxId );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ConstructL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::ConstructL( TMsvId aMailboxId )
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::ConstructL" );
    CAlwaysOnlineEmailAgentBase::ConstructL( aMailboxId );
    iImap4ClientMtm = (CImap4ClientMtm*) iClientMtmRegistry.NewMtmL(
        KSenduiMtmImap4Uid );
    iFolderArray = new (ELeave) CMsvEntrySelection();
    iFlags->SetFlag( EAOBFIsImap4 );
    }

// ----------------------------------------------------------------------------
// ~CAlwaysOnlineImap4Agent()
// ----------------------------------------------------------------------------
CAlwaysOnlineImap4Agent::~CAlwaysOnlineImap4Agent()
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::~CAlwaysOnlineImap4Agent" );
    // cancel ongoing operations
    CloseServices();
    delete iFolderArray;
    delete iFolderObserver;
    delete iImap4ClientMtm;
    }

// ----------------------------------------------------------------------------
// SetStayOnlineFlagL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::SetStayOnlineFlagL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::SetStayOnlineFlagL" );
    //first, check the mode we should be operating in.
    TBool isCsd = EFalse;
    TRAP_IGNORE( isCsd = IsBearerCSDL() );

    TInt inboxTime = RetrievalIntervalInMinutes( LoadSettingL<TInt>( 
            TImumDaSettings::EKeyAutoRetrievalInterval, EFalse ));
    if ( inboxTime <= KAlwaysOnlineMaxLimit && !isCsd )
        {
        iImap4StateFlags.SetFlag( EAlwaysOnlineImap4StayOnline );
        }
    else
        {
        iImap4StateFlags.ClearFlag( EAlwaysOnlineImap4StayOnline );
        }
    }

// ----------------------------------------------------------------------------
// StartL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::StartL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::StartL" );
    //once start delay op has been started we don't ever reset that id.
    if ( iStartDelayOpId == KErrNotFound )
        {
        //check offline & roaming statuses from AO server
        TBool suspended = EFalse;
        QueryAndHandleAOServerInfoL( suspended );

        if ( suspended )
            {
            //we got iState = EEmailAgentIdle from CAlwaysOnlineEmailAgentBase::Suspend
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::StartL() We have been suspended, not starting!");
            return;
            }

        iState = EEmailAgentInitialised;
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::StartL. Setting state: %d ",iState );


        TTime home;
        home.HomeTime();

        //Let's not start immediately on start. Small delay is safer
        TTimeIntervalSeconds startDelay( KInitWaitSeconds );
        home += startDelay;

        StartTimerOperationL( home, iStartDelayOpId );
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::StartL(). Activated startDelayWait 5sec Op Id: %d ",iStartDelayOpId );
        }//if
    else
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::StartL() called, but we're allready started. No changes to state machine");
        }
    }

// ----------------------------------------------------------------------------
// ResumeL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::ResumeL( const TBool aConnectNow )
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::ResumeL" );
    CAlwaysOnlineEmailAgentBase::ResumeL( aConnectNow );

	// just make sure that connection will not stay open
	iImap4StateFlags.ClearFlag( EAlwaysOnlineImap4StayOnline );
    }

// ----------------------------------------------------------------------------
// CreateImap4OperationL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::CreateImap4OperationL(
    TInt aFunctionId,
    TBool aCompletedOperation /*= EFalse*/ )
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::CreateImap4OperationL" );
    TMsvOp dummy;
    CreateImap4OperationL( dummy, aFunctionId, aCompletedOperation );
    }

// ----------------------------------------------------------------------------
// CreateImap4OperationL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::CreateImap4OperationL(
    TMsvOp& aOpId,
    TInt aFunctionId,
    TBool aCompletedOperation /*= EFalse*/ )
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::CreateImap4OperationL" );
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::CreateImap4OperationL() aFunctionId %d", aFunctionId);

    CMsvEntrySelection* selection = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    CMsvSingleOpWatcher* watcher = CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( watcher );
    CMsvOperation* op = NULL;

    if ( aCompletedOperation )
        {
        op = CMsvCompletedOperation::NewL(  // CSI: 35 # cleanupstack is used
                iSession, iEntry->Entry().iMtm, KNullDesC8,
                KMsvLocalServiceIndexEntryId,
                watcher->iStatus, KErrCancel);

        aOpId = op->Id();

        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::CreateImap4OperationL: ADDING COMPLETED OPERATION. ID: %d", aOpId );
        }
    else
        {
        TPckg<MMsvImapConnectionObserver*> param( this );

        selection->AppendL( iEntry->Entry().iServiceId );
        iImap4ClientMtm->SwitchCurrentEntryL( iEntry->Entry().iServiceId );

        op = iImap4ClientMtm->InvokeAsyncFunctionL(
            aFunctionId, *selection, param, watcher->iStatus);

        aOpId = op->Id();
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::CreateImap4OperationL: ADDING CONNECT OPERATION. ID: %d", aOpId );

        }
    CleanupStack::PushL( op );
    AppendWatcherAndSetOperationL( watcher, op ); // takes immediately ownership
    CleanupStack::Pop( 2, watcher ); // op // CSI: 12,47 # nothing wrong
    CleanupStack::PopAndDestroy( selection );//selection
    }


// ----------------------------------------------------------------------------
// ConnectAndUpdateHeadersL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::ConnectAndUpdateHeadersL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::ConnectAndUpdateHeadersL" );
    KAOEMAIL_LOGGER_WRITE_FORMAT( "CAlwaysOnlineImap4Agent::ConnectAndUpdateHeadersL(). Starting to connect into: %d", iEntry->Entry().Id() );

    //Just connect. Whether to stay online or just poll, is decided elsewhere
    if ( iImap4StateFlags.Flag( EAlwaysOnlineImap4StayOnline ) && !IsEmn() )
        {
        KAOEMAIL_LOGGER_WRITE(" CAlwaysOnlineImap4Agent::ConnectAndUpdateHeadersL() Connecting to stay online");
        //do full sync, inbox and folders
        CreateImap4OperationL( iConnectAndStayOnlineOpId,
            KIMAP4MTMConnectAndSyncCompleteAfterDisconnect,
            EFalse );
        iState = EEmailAgentConnectingToStayOnline;

        //folder sync is started from HandleImapConnectionEvent after sync completes
        //mail partial fetch is also started from HandleImapConnectionEvent
        }
    else
        {
        KAOEMAIL_LOGGER_WRITE(" CAlwaysOnlineImap4Agent::ConnectAndUpdateHeadersL() Doing plaing connect");
        //just connect. After this completes, sync is invoked elsewhere
        CreateImap4OperationL( iConnectOpId, KIMAP4MTMConnect, EFalse );
        iState = EEmailAgentPlainConnecting;
        }


    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::ConnectAndUpdateHeadersL(). Setting state:  %d ",iState );
    }

// ----------------------------------------------------------------------------
// DisconnectL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::DisconnectL( TBool aAutomatic )
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::DisconnectL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineImap4Agent::DisconnectL");

    TBuf8<1> dummyParams;
    dummyParams.Zero();
    CMsvSingleOpWatcher* watcher = CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( watcher );
    CMsvOperation* op = NULL;

    if ( iEntry->Entry().Connected() )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::DisconnectL() Connected");
        CMsvEntrySelection* selection = new(ELeave) CMsvEntrySelection;
        CleanupStack::PushL( selection );
        selection->AppendL( iEntry->Entry().iServiceId );
        iImap4ClientMtm->SwitchCurrentEntryL( iEntry->Entry().iServiceId );
        op = iImap4ClientMtm->InvokeAsyncFunctionL(
            KIMAP4MTMDisconnect, *selection, dummyParams, watcher->iStatus);

        iDisconnectOpId = op->Id();
        CleanupStack::PopAndDestroy( selection );//selection
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::DisconnectL: ADDING DISCONNECT OPERATION: %d", iDisconnectOpId );
        }
    else
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::DisconnectL() Not connected");
        //already disconnected
        op = CMsvCompletedOperation::NewL(
                iSession, iEntry->Entry().iMtm, KNullDesC8,
                KMsvLocalServiceIndexEntryId,
                watcher->iStatus, KErrCancel);
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::DisconnectL: ADDING COMPLETED OPERATION: %d", op->Id() );
        }

    CleanupStack::PushL( op );
    AppendWatcherAndSetOperationL( watcher, op ); // takes immediately ownership
    CleanupStack::Pop( 2, watcher); // op // CSI: 12,47 # nothing wrong

    if ( aAutomatic )
        {
        iState = EEmailAgentAutoDisconnecting;
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::DisconnectL. Setting state:  %d ",iState );
        }
    else
        {
        iState = EEmailAgentUserDisconnecting;
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::DisconnectL. Setting state:  %d ",iState );
        }
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4Agent::DisconnectL");
    }

// ----------------------------------------------------------------------------
// SynchroniseNewL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::SynchroniseNewL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::SynchroniseNewL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineImap4Agent::SynchroniseNewL");
    TBuf8<1> dummyParams;
    dummyParams.Zero();

    CMsvSingleOpWatcher* watcher = CMsvSingleOpWatcher::NewL(*this);
    CleanupStack::PushL( watcher );
    CMsvOperation* op = NULL;

    //we must be connected to use inboxnewsync
    if ( iEntry->Entry().Connected() )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::SynchroniseNewL() Connected");
        SetLastSuccessfulUpdate();
        CMsvEntrySelection* selection = new(ELeave) CMsvEntrySelection;
        CleanupStack::PushL( selection );
        selection->AppendL( iEntry->Entry().iServiceId );
        iImap4ClientMtm->SwitchCurrentEntryL( iEntry->Entry().iServiceId );
        op = iImap4ClientMtm->InvokeAsyncFunctionL(
            KIMAP4MTMFullSync, *selection, dummyParams, watcher->iStatus);

        iSyncOpId = op->Id();

        CleanupStack::PopAndDestroy( selection );//selection
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::SynchroniseNewL: Adding synchronise new operation. ID: %d", iSyncOpId );

        iState = EEmailAgentSynchronising;
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::SynchroniseNewL. Setting state:  %d ",iState );
        }
    else
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::SynchroniseNewL() Not connected");
        //for some reason we have been disconnected before we got to sync part. Invoke normal retry procedure
        iError = KErrDisconnected; //this error will cause us to retry
        op = CMsvCompletedOperation::NewL(
                iSession, iEntry->Entry().iMtm, KNullDesC8,
                KMsvLocalServiceIndexEntryId,
                watcher->iStatus, KErrCancel);
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::SynchroniseNewL: Connection was dropped, retry!" );
        }

    CleanupStack::PushL( op );
    AppendWatcherAndSetOperationL( watcher, op ); // takes immediately ownership
    CleanupStack::Pop( 2, watcher); // op // CSI: 12,47 # nothing wrong
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4Agent::SynchroniseNewL");
    }

// ----------------------------------------------------------------------------
// HandleImapConnectionEvent() from MMsvImapConnectionObserver
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::HandleImapConnectionEvent(
    TImapConnectionEvent aConnectionEvent )
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::HandleImapConnectionEvent" );
    TInt trapErr = KErrNone;
    switch ( aConnectionEvent )
        {
        case ESynchronisationComplete:
            //we can reset retrycounter when sync has occurred. Because it can be
            //taken as success, so it would not be retrying for same connection anymore.
            iRetryCounter = 0;

            //this event can be used also to set last successful update
            SetLastSuccessfulUpdate();

            if ( iImap4StateFlags.Flag( EAlwaysOnlineImap4StayOnline ) /*iStayOnline*/ )
                {
                if ( iFilteredPopulateOpId == KErrNotFound )
                    {
                    // start fetch new mail operation
                    TRAP( trapErr, FetchNewMailL() );
                    if ( trapErr != KErrNone )
                        {
                        // try to start folder sync timer not the folder sync itself, because
                        // they were synced on connect already
                        TRAP( trapErr, StartFolderSyncTimerL() );
                        }
                    }
                }
            break;
        case EDisconnecting:
            break;
        case EConnectionCompleted:
            break;
        default:
            break;
        }
    TRAP_IGNORE( CallNewMessagesL() );
    }


// ----------------------------------------------------------------------------
// ChangeNextStateL
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::ChangeNextStateL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::ChangeNextStateL" );
    AOLOG_WRMV( "start iState: ", EAoMailPluginStates, iState, EAoLogSt3 );
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::ChangeNextStateL() iState: %d", iState);

    if ( iState == EEmailAgentAutoDisconnecting &&
         iImap4StateFlags.Flag( EAlwaysOnlineImap4RemoveMeImmediately ) )
        {
        if ( !iImap4StateFlags.Flag( EAlwaysOnlineImap4DontRemoveOnDisconnect ) )
            {
            KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::ChangeNextStateL() RemoveMeL 0x%x", iEntry->Entry().Id() );
            RemoveMe();
            return;
            }
        else
            {
            KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::ChangeNextStateL() change 0x%x back to normal", iEntry->Entry().Id() );
            iImap4StateFlags.ClearFlag( EAlwaysOnlineImap4DontRemoveOnDisconnect );
            iImap4StateFlags.ClearFlag( EAlwaysOnlineImap4TemporaryConnectionObserver );
            iImap4StateFlags.ClearFlag( EAlwaysOnlineImap4DontReconnect );
            iImap4StateFlags.ClearFlag( EAlwaysOnlineImap4StayOnline );
            iImap4StateFlags.ClearFlag( EAlwaysOnlineImap4RemoveMeImmediately );
            }
        }


    //if we are waiting for schedule to start, we don't handle conn errors.
    //this should only occur if conn op has been started just before schedule ends.
    if ( iError != KErrNone && iState != EEmailAgentTimerWaitingForStart )
        {
        HandleOpErrorL();
        }

    switch ( iState )
        {
        // Agent just constructed ( or settings changed ) and ready. Should be
        // here only when settings have been saved with always online or mail
        // message notifications on.
        case EEmailAgentInitialised:
            //settings may have been changed, reset all pending operations
            //because their timings can be different compared to newly saved settings

            ResetAll();

            if ( !IsEmn() )
                {
                CheckAndHandleSchedulingL();//starts schedule timers and sets state accordingly
                if ( iWaitForStopOpId != KErrNotFound ||
                     ( iWaitForStopOpId == KErrNotFound &&
                       iWaitForStartOpId == KErrNotFound ) )//no start or stop waiters, all times
                    {
                    //CheckAndHandleScheduling started waitForStopTimer
                    StartWaitTimerL();
                    }
                //else, do nothing, we wait until waitForStartTimer expires and starts updates again
                }
            else
                {
                iState = EEmailAgentIdle;
                KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::ChangeNextStateL() EMN is on --> no scheduling or timers needed!");
                }
            break;

        //case EEmailAgentConnected:
        case EEmailAgentConnecting:
            DisconnectL( ETrue );
            break;
        case EEmailAgentPlainConnecting:
            SynchroniseNewL();
            break;
        case EEmailAgentSynchronising:
            FetchNewMailL(); //check filtering options and acts accordingly
            break;
        case EEmailAgentFetching:
            if ( IsTemporary() || ( iImap4StateFlags.Flag( EAlwaysOnlineImap4StayOnline ) &&
                !IsEmn() ) )
                {
                StartFolderSyncTimerL();
                }
            else
                {
                DisconnectL( ETrue );// Folder's not synchronized when EMN is on
                }
            break;
        case EEmailAgentConnectingToStayOnline:
            //if this has completed, we have to connect again
            ConnectIfAllowedL( iConnectAndStayOnlineOpId );
            break;
        case EEmailAgentConnectFailed:
        case EEmailAgentQueued:
        case EEmailAgentReconnecting:
        case EEmailAgentAutoDisconnecting:
        case EEmailAgentUserDisconnecting:
        case EEmailAgentConnTerminated://red key
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::ChangeNextStateL() StartWaitTimerL");
            if ( IsReconnectAfterError() )
                {
                StartWaitTimerL();
                }
            else
                {
                DisconnectL( ETrue );// Folder's not synchronized when EMN is on
                // disconnect if we are connected
                iState = EEmailAgentIdle;
                //give up, reset retry counter
                iRetryCounter = 0;
                CreateCompletedOpL();
                }
            break;
        case EEmailAgentTimerWaiting:
            // Just make sure that wait has completed before connecting
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::ChangeNextStateL() ConnectIfAllowedL");
            ConnectIfAllowedL( iIntervalWaitId );
            break;

        case EEmailAgentTimerWaitingForStart:
            // Wait end, can connect
            ConnectIfAllowedL( iWaitForStartOpId );
            // Else, do nothing, we are not going to change state until
            // waitForStart timer completes
            break;
        case EEmailAgentIdle:
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::ChangeNextStateL() Idling.... zZzZzZzZzzzzzzzzzzzzz");
            // No handling for idle, we just "hang around"
            break;
        case EEmailAgentFatalError:
            // Something is so wrong that connection cannot be made
            // without user intervention like incorrect settings or such
            DisplayGlobalErrorNoteL();
            SwitchAutoUpdateOffL();

            break;
        default:
            KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::ChangeNextStateL() Unknown state!: %d,resetting all!", iState);
            ResetAll();
            iState = EEmailAgentInitialised;
            CreateCompletedOpL();
            break;
        }
    AOLOG_WRMV( "end iState: ", EAoMailPluginStates, iState, EAoLogSt3 );
    }

// ----------------------------------------------------------------------------
// StartWaitTimerL
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::StartWaitTimerL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::StartWaitTimerL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineImap4Agent::StartWaitTimerL");

    if ( iIntervalWaitId > KErrNotFound )
        {
        KAOEMAIL_LOGGER_WRITE_FORMAT("already waiting, try to find wait operation: %d", iIntervalWaitId );
        const TInt count = iOperations.Count();
        for ( TInt i = count - 1; i >= 0; i-- )
            {
            CMsvOperation& oper = iOperations[i]->Operation();
            if ( oper.Id() == iIntervalWaitId )
                {
                CMsvSingleOpWatcher* opWatcher = iOperations[i];
                KAOEMAIL_LOGGER_WRITE("wait operation found, delete it");
                iOperations.Delete( i );
                delete opWatcher;
                iIntervalWaitId = KErrNotFound;
                break;
                }
            }

        }

    //waiting already
    if ( iState == EEmailAgentTimerWaiting )
        {
        KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4Agent::StartWaitTimerL, already waiting");
        return;
        }

    TTime time;
    time.HomeTime();

    //if just exited settings we should connect, only when AO is on (not EMN).
    if ( iFlags->Flag( EAOBFConnectNow ) && !IsEmn() )
        {
        // Set to false, one time only
        iFlags->ClearFlag( EAOBFConnectNow );
        TTimeIntervalSeconds conn = KInitWaitSeconds;
        time += conn;

        StartTimerOperationL( time, iIntervalWaitId );
        iState = EEmailAgentTimerWaiting;
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::StartWaitTimerL. Connecting in 5 seconds. Op Id:  %d ",iIntervalWaitId );
        KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4Agent::StartWaitTimerL");
        return;
        }


    //if we are retrying a connection. n quick retries and then qive up
    if ( ( iRetryCounter % KAOMaxRetries ) != 0 )
        {
        TTimeIntervalSeconds seconds( 0 );
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::StartWaitTimerL() iRetryCounter %d", iRetryCounter);

        switch ( (iRetryCounter % KAOMaxRetries) )
            {
            //values for these retry times configured in AlwaysOnlineEmailAgentBase.h
            case 1:
            case 2: // CSI: 47 # see comment above
                seconds = EEMailAgentReconnectThree;
                break;
            default:
                seconds = EEMailAgentReconnectFive;
                break;
            }//switch

        time += seconds;
        }//if
    else if ( iState == EEmailAgentQueued )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::StartWaitTimerL() EEmailAgentQueued");
        TTimeIntervalSeconds seconds( 0 );
        seconds = EEMailAgentReconnectFour;
        time += seconds;
        }
    else if ( !IsEmn() )
        {
        TInt minutes = RetrievalIntervalInMinutes( LoadSettingL<TInt>( 
            TImumDaSettings::EKeyAutoRetrievalInterval, EFalse ));
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::StartWaitTimerL() Minutes to wait: %d", minutes);

        TTimeIntervalMinutes intervalMinutes( minutes );
        time += intervalMinutes;
        }

    StartTimerOperationL( time, iIntervalWaitId );
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::StartWaitTimerL. Interval timer started. Op Id:  %d ",iIntervalWaitId );

    iState = EEmailAgentTimerWaiting;
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::StartWaitTimerL. Setting state:  %d ",iState );
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4Agent::StartWaitTimerL");
    }



// ----------------------------------------------------------------------------
// FetchNewMailL
// ----------------------------------------------------------------------------
TMsvOp CAlwaysOnlineImap4Agent::FetchNewMailL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::FetchNewMail" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineImap4Agent::FetchNewMailL");
    // This is completed because Always Online is supposed to only synchronize the headers.
    TMsvOp opId = KErrNotFound;
    iState = EEmailAgentFetching;
    CreateImap4OperationL( opId, 0, ETrue );
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4Agent::FetchNewMailL");
    return opId;
    }

// ----------------------------------------------------------------------------
// FindInboxL
// ----------------------------------------------------------------------------
TMsvId CAlwaysOnlineImap4Agent::FindInboxL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::FindInboxL" );
    TMsvId inboxId = KErrNotFound;
    TMsvEntry child;
    const TInt count = iEntry->Count();
    _LIT( KInboxName, "Inbox");

    for ( TInt loop = 0; loop < count && inboxId == KErrNotFound; loop++ )
        {
        child = (*iEntry)[loop];
        if ( child.iType == KUidMsvFolderEntry  &&
             child.iDetails.CompareF( KInboxName ) == 0 )
            {
            inboxId = child.Id();
            }
        }
    return inboxId;
    }

// ----------------------------------------------------------------------------
// HandleOpErrorL
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::HandleOpErrorL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::HandleOpErrorL" );
    AOLOG_WRMV("iError: ", EAoNormalError, iError, EAoLogSt3);
    AOLOG_WRMV( "start iState: ", EAoMailPluginStates, iState, EAoLogSt3 );
    KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::HandleOpErrorL() Error Handling started" );
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::HandleOpErrorL() Error ID: %d", iError );
    switch ( iError )
        {
        case KErrNone:
            //shouldn't come here if KErrNone
            break;
        case KErrAbort:
        case KErrCancel:
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::HandleOpErrorL(): KErrCancel");
            //sometimes we seem to get these errors with connection operations.
            //so go to initialised state -> reset all
            iError = KErrNone;
            iState = EEmailAgentInitialised; //this will be changed where those timers were cancelled.
            break;

        case KErrGeneral://seems that this is what is given when phone/csd is in use
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::HandleOpErrorL(): KErrGeneral");
            iState = EEmailAgentQueued;
            KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::HandleOpErrorL. Setting state:  %d ",iState );
            break;
        case KErrInUse:
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::HandleOpErrorL(): KErrInUse");
            iState = EEmailAgentQueued;
            KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::HandleOpErrorL. Setting state:  %d ",iState );
            break;
            //fatal errors, cannot recover without user intervention
        case KErrImapServerNoSecurity:
        case KErrImapTLSNegotiateFailed:
        case KImskSSLTLSNegotiateFailed:
        case KErrImapBadLogon:
        case KErrCorrupt:
            {
            iState = EEmailAgentFatalError;
            iRetryCounter = 0;
            KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::HandleOpErrorL. Fatal Error: %d.",iError );
            KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::HandleOpErrorL. Setting state:  %d ",iState );
            }
            break;

        case KErrAoServerNotFound:
            HandleDefaultError();
            break;
        case KErrConnectionTerminated:
        case KErrTimedOut:
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::HandleOpErrorL. KErrConnectionTimedOut(-33) OR KErrConnectionTerminated");
            iState = EEmailAgentConnTerminated;
            break;

        //just let these go to default retry mechanism

        case KErrImapServerLoginDisabled:
        case KErrImapServerParse:
        case KErrImapServerBusy:
        case KErrImapServerVersion:
        case KErrImapSendFail:
        case KErrImapSelectFail:
        case KErrImapWrongFolder:
        case KErrImapCantDeleteFolder:
        case KErrImapInvalidServerResponse:
        case KImskErrorDNSNotFound:
        case KImskErrorControlPanelLocked:
        case KImskErrorISPOrIAPRecordNotFound:
        case KImskErrorActiveSettingIsDifferent:
        case KImskSecuritySettingsFailed:
        case KErrImapConnectFail:
        case KErrImapServerFail://how fatal is this?
        case KErrDisconnected:

        default:
            HandleDefaultError();
            break;
        };
    //reset error
    iError = KErrNone;
    AOLOG_WRMV( "end iState: ", EAoMailPluginStates, iState, EAoLogSt3 );
    KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::HandleOpErrorL() iError Has been reset to KErrNone");
    }

// ----------------------------------------------------------------------------
// DoSyncDisconnectL
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::DoSyncDisconnectL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::DoSyncDisconnectL" );
    if( iEntry->Entry().Connected() )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::DoSyncDisconnectL() Doing SyncDisconnect! ");

		TBuf8<1> dummyParams;
	    dummyParams.Zero();
		// Needed to change next operation to synchronized
        CMsvOperationActiveSchedulerWait* wait =
        	CMsvOperationActiveSchedulerWait::NewLC();

        CMsvEntrySelection* selection = new(ELeave) CMsvEntrySelection;
        CleanupStack::PushL( selection );

        selection->AppendL( iEntry->Entry().iServiceId );
        iImap4ClientMtm->SwitchCurrentEntryL( iEntry->Entry().iServiceId );
        CMsvOperation* op = iImap4ClientMtm->InvokeAsyncFunctionL(
            KIMAP4MTMDisconnect, *selection, dummyParams, wait->iStatus );

		// Start wait object
       	wait->Start();
       	
        delete op;
        CleanupStack::PopAndDestroy( 2, wait ); // selection // CSI: 12,47 # nothing wrong
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::DoSyncDisconnectL() Should be disconnected now! ");
        }
    }

// ----------------------------------------------------------------------------
//  Called from OpCompleted
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::HandleOpCompleted(
    TMsvOp opId,
    TInt aCompletionCode )
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::HandleOpCompleted" );
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::HandleOpCompleted(). Completion code %d",aCompletionCode );
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::HandleOpCompleted(). opId = %d",opId );

    TInt trapErr = KErrNone;
    //save error code so we can handle errors elsewhere
    if ( opId == iConnectAndStayOnlineOpId )
        {
        //if this completes, reconnect. KErrDisconnected causes us to invoke retry mechanism
        iError = aCompletionCode;

        if ( iDisconnectOpId == KErrNotFound )
            {
            //if we ourselves disconnected, we don't want to reconnect
            if ( iError == KErrNone )
                {
                //if no real error, set this so we will reconnect
                iError = KErrDisconnected;
                }
            }
        else
            {
            iDisconnectOpId = KErrNotFound;
            }

        iConnectAndStayOnlineOpId = KErrNotFound;
        }
    else if( ( opId == iConnectSyncOpId || 
               opId == iConnectOpId || 
               opId == iSyncOpId ) && 
             aCompletionCode != KErrNone )
        {
        iError = aCompletionCode;
        SetLastUpdateFailed();
        }
    else if ( opId == iConnectSyncOpId )
        {
        iError = KErrNone;
        iConnectSyncOpId = KErrNotFound;
        iRetryCounter = 0; //reset retry counter
        }
    else if ( opId == iConnectOpId )
        {
        iError = KErrNone;
        iConnectOpId = KErrNotFound;
        iRetryCounter = 0; //reset retry counter
        }
    else if ( opId == iSyncOpId )
        {
        iError = KErrNone;
        iSyncOpId = KErrNotFound;
        iRetryCounter = 0; //reset retry counter
        }
    else if ( opId == iIntervalWaitId )
        {
        iIntervalWaitId = KErrNotFound;
        }
    else if ( opId == iWaitForStopOpId )
        {
        if ( aCompletionCode == KErrNone )
            {
            iWaitForStopOpId = KErrNotFound;
            TRAP( trapErr, CheckAndHandleSchedulingL() );
            if ( trapErr != KErrNone )
                {
                //can't know why leaved. just reset all and restart
                ResetAll();
                iState = EEmailAgentInitialised;
                }
            }
        }
    else if ( opId == iWaitForStartOpId )
        {
        if ( aCompletionCode == KErrNone )
            {
            iWaitForStartOpId = KErrNotFound;
            TRAP( trapErr, CheckAndHandleSchedulingL() );
            if ( trapErr != KErrNone )
                {
                //can't know why leaved. just reset all and restart
                ResetAll();
                iState = EEmailAgentInitialised;
                }
            }
        }
    else if ( opId == iStartDelayOpId )
        {
        iFlags->SetFlag( EAOBFConnectNow );
        //leave id untouched. We need this only once, on bootup, or when agent is activated via settings
        //we use this to check is it used already
        }
    else if ( opId == iFolderUpdateTimerOpId )
        {
        //timer is restarted when BatchEnd operation completes
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::HandleOpCompleted DoFolderSyncL() called, iFolderUpdateTimerOpId reseted");
        iFolderUpdateTimerOpId = KErrNotFound;
        TRAP( trapErr, DoFolderSyncL() );
        if( trapErr != KErrNone )
                {
                //folder sync failed for some reason. If it's timer isn't restarted
                //do it now
                if( iFolderUpdateTimerOpId == KErrNotFound )
                    {
                    TRAP( trapErr, StartFolderSyncTimerL() );
                    //no handling for leave. Just give up with folder sync
                    }
                }
        }
    else if ( opId == iFolderSyncOpId )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::HandleOpCompleted iFolderSyncOpId completed");
        //restart folder update timer
        iFolderSyncOpId = KErrNotFound;
        if ( iImap4StateFlags.Flag(
            EAlwaysOnlineImap4StartFetchAfterFolderSync ) )
            {
            TRAP( trapErr, FetchNewMailL() );
            iImap4StateFlags.ClearFlag(
                EAlwaysOnlineImap4StartFetchAfterFolderSync );
            }
        else
            {
            TRAP( trapErr, StartFolderSyncTimerL() );
            }
        //no handling for leave. Just give up with folder sync
        }
    else if ( opId == iFilteredPopulateOpId )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::HandleOpCompleted iFilteredPopulateOpId completed");
        //restart folder update timer
        iFilteredPopulateOpId = KErrNotFound;
        TRAP( trapErr, StartFolderSyncTimerL() );
        //no handling for leave. Just give up with folder sync
        }

    TRAP_IGNORE( CallNewMessagesL() );
    }


// ----------------------------------------------------------------------------
//  CreateCompletedOpL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::CreateCompletedOpL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::CreateCompletedOpL" );
    CreateImap4OperationL( 0, ETrue );//creates completed operation
    }

// ----------------------------------------------------------------------------
//  DoFolderSyncL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::DoFolderSyncL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::DoFolderSyncL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineImap4Agent::DoFolderSyncL");
    //do folder sync using fullsync function. No point in using several separate foldersync functions because
    //there is no performance benefit and it is also far more complex to implement. Also data traffic is higher
    //if separate folders are synced

    if ( iEntry->Entry().Connected() )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::DoFolderSyncL() Connected");
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4Agent::DoFolderSyncL() Folder count %d", iFolderArray->Count() );
        if ( iFolderArray->Count() > 1 &&
             iFilteredPopulateOpId == KErrNotFound )
            {
            //only if connected. We shouldn't get here if not connected, but to be sure...
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::DoFolderSyncL() Folder sync with KIMAP4MTMFullSync started");
            CreateImap4OperationL( iFolderSyncOpId,
                KIMAP4MTMFullSync, EFalse );

            // it *can* happen so that fetching of the email is still in progress while this is started, it could be handled so that
            // folder sync timer is re-started in that case instead of starting full sync.
            // Added iFilteredPopulateOpId == KErrNotFound because of that, maybe that is enough?

            }
        else
            {
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::DoFolderSyncL() just create compeleted operation");
            CreateImap4OperationL( iFolderSyncOpId, 0, ETrue );
            }
        }
    else
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::DoFolderSyncL() Connection closed, let's try to connect again!");
        iState = EEmailAgentConnectingToStayOnline; // let's try to connect again
        }
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4Agent::DoFolderSyncL");
    }



// ----------------------------------------------------------------------------
// CAlwaysOnlineImap4Agent::StartFolderSyncTimerL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::StartFolderSyncTimerL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::StartFolderSyncTimerL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineImap4Agent::StartFolderSyncTimerL");
    KAOEMAIL_LOGGER_WRITE_FORMAT2("iFolderUpdateTimerOpId: %d, iFilteredPopulateOpId: %d", iFolderUpdateTimerOpId, iFilteredPopulateOpId);
    KAOEMAIL_LOGGER_WRITE_FORMAT("iFolderSyncOpId: %d", iFolderSyncOpId );
    if ( iImap4StateFlags.Flag( EAlwaysOnlineImap4StayOnline ) &&
         iFolderUpdateTimerOpId == KErrNotFound &&
         iFilteredPopulateOpId == KErrNotFound &&
         iFolderSyncOpId == KErrNotFound )
        {
        TTimeIntervalSeconds folderWait;
        TTime home;
        home.HomeTime();

        // If there is a temporary agent created (==EMailAoOff), which means
        // user has taken connection manually. It is agreed, that in this case
        // we must use value returned by CImImap4Settings::SyncRate function.
        // Otherwise user would not know what interval is used for updating
        // folders.
        if ( LoadSettingL<TInt>(
            TImumDaSettings::EKeyAutoRetrieval, EFalse ) !=
                TImumDaSettings::EValueAutoOff )
            {
            //refresh time must be less than 30 min, because we are in
            // iStayOnline -mode take off 2 minutes if limit is set to 30min
            // because of IMAP protocoll 29 minute timeout. 28 is safe enough
            TInt interval = RetrievalIntervalInMinutes( LoadSettingL<TInt>( 
                TImumDaSettings::EKeyAutoRetrievalInterval, EFalse ));
            folderWait = interval * KAlwaysOnlineMinuteToSecondsMultiplier;
            if ( interval == KAlwaysOnlineMaxLimit )
                {
                folderWait = ( folderWait.Int() - KAlwaysOnlineTwoMinutes );
                }
            }
        else
            {
            folderWait = LoadSettingL<TInt>( TImumInSettings::EKeySyncRate, ETrue );
            }

        // Add wait time to current time and start timer.
        home += folderWait;

        KAOEMAIL_LOGGER_WRITE_DATETIME("CAlwaysOnlineImap4Agent::StartFolderSyncTimerL(): Folder timer started, will complete at: ", home );
        StartTimerOperationL( home, iFolderUpdateTimerOpId );
        }//if

    // Start inbox folder observer here...
    StartFolderObserverL();
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4Agent::StartFolderSyncTimerL");

    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineImap4Agent::AddSubscribedFoldersL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::UpdateSubscribedFoldersL(
    TMsvId aFolderId )
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::UpdateSubscribedFoldersL" );
    iFolderArray->Reset();
    DoUpdateSubscribedFoldersL( aFolderId );
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineImap4Agent::DoAddSubscribedFoldersL()
// NOTE: This is recursive!
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::DoUpdateSubscribedFoldersL(
    TMsvId aFolderId )
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::DoUpdateSubscribedFoldersL" );
    CMsvEntry* entry = iSession.GetEntryL( aFolderId );
    CleanupStack::PushL( entry );

    const TInt count = entry->Count();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        TMsvEmailEntry mailEntry = (*entry)[loop];
        if ( mailEntry.iType.iUid == KUidMsvFolderEntryValue )
            {
            if ( mailEntry.LocalSubscription() )
                {
                iFolderArray->AppendL( mailEntry.Id() );
                }
            DoUpdateSubscribedFoldersL( mailEntry.Id() );
            }
        }

    CleanupStack::PopAndDestroy( entry );

    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineImap4Agent::AppendFolderArrayL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::AppendFolderArrayL(
    CMsvEntrySelection& aFolders,
    TMsvId aInboxId )
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::AppendFolderArrayL" );
    const TInt count = iFolderArray->Count();
    TInt i = 0;
    for ( i=0; i < count; i++ )
        {
        if ( (*iFolderArray)[i] != aInboxId )
            {
            aFolders.AppendL( (*iFolderArray)[i] );
            }
        }

    }


// ----------------------------------------------------------------------------
// CAlwaysOnlineImap4Agent::StartFolderObserverL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::StartFolderObserverL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::StartFolderObserverL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineImap4Agent::StartFolderObserverL");
    // Start observer only if staying online
    if ( iImap4StateFlags.Flag( EAlwaysOnlineImap4StayOnline ) )
        {
        if ( !iFolderObserver )
            {
            iFolderObserver = CAlwaysOnlineImap4FolderObserver::NewL(
                iSession,
                NULL, // this is set below
                *this );
            }
        UpdateSubscribedFoldersL( iEntry->Entry().Id() );
        iFolderObserver->ResetFoldersL( iFolderArray );
        if ( iImap4StateFlags.Flag( EAlwaysOnlineImap4DontReconnect ) )
            {
            iFolderObserver->SetMailbox( iEntry->Entry().Id() );
            }
        else
            {
            iFolderObserver->SetMailbox( 0 );
            }
        iFolderObserver->Start();
        KAOEMAIL_LOGGER_WRITE("Observer started");
        }
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4Agent::StartFolderObserverL");
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineImap4Agent::StopFolderObserver()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::StopFolderObserver()
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::StopFolderObserver" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineImap4Agent::StopFolderObserver");
    if ( iFolderObserver )
        {
        iFolderObserver->Stop();
        }
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4Agent::StopFolderObserver");
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineImap4Agent::HandleFolderEntryL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::HandleFolderEntryL(
    const CMsvEntrySelection& aFolderArray  )
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::HandleFolderEntryL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineImap4Agent::HandleFolderEntryL");
#ifdef _DEBUG
    const TInt count = aFolderArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        KAOEMAIL_LOGGER_WRITE_FORMAT( "folderId 0x%x", aFolderArray[i] );
        }
#endif
    TInt index = aFolderArray.Find( iEntry->Entry().Id() );
    if ( index > KErrNotFound )
        {
        // disconnected? Remove myself.
        if( !iEntry->Entry().Connected() )
            {
            // this is a bit dirty way. This agent is removed after disconnect is completed
            KAOEMAIL_LOGGER_WRITE( "Mailbox disconnected, kill myself!");
            iImap4StateFlags.SetFlag( EAlwaysOnlineImap4RemoveMeImmediately );
            DisconnectL( ETrue );
            return;
            }
        }


    // Start populate operation
    // Change this so that fetch new started after folder sync, if it is going, if not,
    // then just start fetch new immediately
    if ( iFolderSyncOpId > KErrNotFound )
        {
        StopFolderObserver();
        KAOEMAIL_LOGGER_WRITE( "Wait folder sync operation to complete");
        iImap4StateFlags.SetFlag( EAlwaysOnlineImap4StartFetchAfterFolderSync );
        }
    // NOTE: Always Online is supposed to get only the headers, no need to fetch the bodies for mails
    // in subscribed folders!!!
    else
        {
        TMsvOp opId;
        iState = EEmailAgentFetching;
        CreateImap4OperationL( opId, 0, ETrue );
        KAOEMAIL_LOGGER_WRITE( "Compliting operation in HandleFolderEntryL");
        }
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4Agent::HandleFolderEntryL");
    }

// ---------------------------------------------------------------------------
// CAlwaysOnlineImap4Agent::FillMailOptionsL()
// ---------------------------------------------------------------------------
//
TImImap4GetPartialMailInfo CAlwaysOnlineImap4Agent::FillMailOptionsL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::FillMailOptionsL" );
    TImImap4GetPartialMailInfo getMailInfo;

    TInt syncFlags = LoadSettingL<TInt>(
        TImumInSettings::EKeySyncFlags, ETrue );

    if ( syncFlags == TImumInSettings::EFlagDownloadHeader )
        {
        getMailInfo.iGetMailBodyParts = EGetImap4EmailHeaders;
        getMailInfo.iPartialMailOptions = ENoSizeLimits;
        }
    else
        {
        // Currently, other than headers can be retrieved
        User::Leave( KErrNotSupported );
        }

    // Set size definitions
    getMailInfo.iBodyTextSizeLimit =
        LoadSettingL<TInt32>( TImumInSettings::EKeyDownloadBodySize, ETrue );
    getMailInfo.iAttachmentSizeLimit =
        LoadSettingL<TInt32>( TImumInSettings::EKeyDownloadAttachmentSize, ETrue );
    getMailInfo.iTotalSizeLimit =
        getMailInfo.iAttachmentSizeLimit + getMailInfo.iBodyTextSizeLimit;
    getMailInfo.iMaxEmailSize = getMailInfo.iTotalSizeLimit;

    if ( getMailInfo.iMaxEmailSize == 0 )
        {
        getMailInfo.iGetMailBodyParts = EGetImap4EmailHeaders;
        }
    getMailInfo.iDestinationFolder = KMsvNullIndexEntryId; // No destination for populate.

    return getMailInfo;
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineImap4Agent::FetchNewMailFromSelectedFoldersL()
// ----------------------------------------------------------------------------
TMsvOp CAlwaysOnlineImap4Agent::FetchNewMailFromSelectedFoldersL(
    const CMsvEntrySelection& aFolders )
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::FetchNewMailFromSelectedFoldersL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineImap4Agent::FetchNewMailFromSelectedFoldersL");
    TMsvOp opId = 0;

    TImImap4GetPartialMailInfo getMailInfo = FillMailOptionsL();

    KAOEMAIL_LOGGER_WRITE_FORMAT("Fetching size limit: %d", getMailInfo.iBodyTextSizeLimit);

    iState = EEmailAgentFetching;
    if ( getMailInfo.iGetMailBodyParts == EGetImap4EmailHeaders )
        {
        // Just complete since filtering is not in use
        KAOEMAIL_LOGGER_WRITE_FORMAT("Setting is just headers, create completed operation opId = %d", opId );
        CreateImap4OperationL( opId, 0, ETrue );
        }
    else
        {
        // read imap filtering settings
        KAOEMAIL_LOGGER_WRITE_FORMAT("folder count = %d", aFolders.Count());

        CMsvSingleOpWatcher* watcher = CMsvSingleOpWatcher::NewL(*this);
        CleanupStack::PushL( watcher );
        CMsvOperation* op = CAlwaysOnlineImap4FolderPopulate::NewL(
            iSession,
            *iImap4ClientMtm,
            iEntry->Entry().Id(),
            iInboxId,
            getMailInfo,
            &aFolders,
            watcher->iStatus );

        CleanupStack::PushL( op );
        opId = op->Id();
        AppendWatcherAndSetOperationL( watcher, op ); // takes immediately ownership

        CleanupStack::Pop( 2, watcher); // op // CSI: 12,47 # nothing wrong
        }
    iFilteredPopulateOpId = opId;
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4Agent::FetchNewMailFromSelectedFoldersL");
    return opId;
    }

// ----------------------------------------------------------------------------
// SetUpdateMailWhileConnectedL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4Agent::SetUpdateMailWhileConnectedL(
    TBool aAgentAlreadyCreated )
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::SetUpdateMailWhileConnectedL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineImap4Agent::SetUpdateMailWhileConnectedL");

    if ( iImap4StateFlags.Flag( EAlwaysOnlineImap4StayOnline ) )
        {
        // we are already observing...
        KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4Agent::SetUpdateMailWhileConnectedL ignore");
        return;
        }

    if ( aAgentAlreadyCreated )
        {
        iImap4StateFlags.SetFlag( EAlwaysOnlineImap4DontRemoveOnDisconnect );
        }

    iImap4StateFlags.SetFlag( EAlwaysOnlineImap4TemporaryConnectionObserver );
    iImap4StateFlags.SetFlag( EAlwaysOnlineImap4DontReconnect );
    iImap4ClientMtm->SwitchCurrentEntryL( iEntry->Entry().Id() );

    if( iImap4ClientMtm->Entry().Entry().Connected() )
        {
        iImap4StateFlags.SetFlag( EAlwaysOnlineImap4StayOnline );
        iState = EEmailAgentIdle;
        StartFolderSyncTimerL();
        KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4Agent::SetUpdateMailWhileConnectedL 1");
        return;
        }

    // this is a bit dirty way. This agent is removed after disconnect is completed
    KAOEMAIL_LOGGER_WRITE( "Mailbox disconnected even we should be connected, kill myself!");
    iImap4StateFlags.SetFlag( EAlwaysOnlineImap4RemoveMeImmediately );
    DisconnectL( ETrue ); // this creates just completed operation if service is not connected.
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4Agent::SetUpdateMailWhileConnectedL 2");
    }

// ----------------------------------------------------------------------------
// IsTemporary()
// ----------------------------------------------------------------------------
TBool CAlwaysOnlineImap4Agent::IsTemporary() const
    {
    AOLOG_IN( "CAlwaysOnlineImap4Agent::IsTemporary" );
    // If temporary agent is running, then flag EAlwaysOnlineImap4TemporaryConnectionObserver
    // is set. At this stage, showing of notes are not allowed, since user may get
    // confused with the disk full note!
    TBool result =
        iImap4StateFlags.Flag( EAlwaysOnlineImap4TemporaryConnectionObserver );

    if ( result )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::IsTemporary: Yes, I am temporary! No notes, plz!" );
        return ETrue;
        }
    else
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4Agent::IsTemporary: No, I am not temporary! Go ahead and show notes, plz!" );
        return EFalse;
        }
    }


//EOF
