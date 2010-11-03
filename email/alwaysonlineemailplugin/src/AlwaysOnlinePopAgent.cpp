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
*     POP specific email agent
*
*/


//INCLUDES
#include <SenduiMtmUids.h>
#include <es_sock.h>
#include <MuiuOperationWait.h>
#include <ImumInSettingsKeys.h>
#include <ImumDaSettingsKeys.h>

#include "AlwaysOnlinePopAgent.h"
#include "AlwaysOnlineEmailPluginLogging.h"
#include "AlwaysOnlineEmailLoggingTools.h"
const TInt KEmailMaxCharsInLine = 75;
const TInt KEmailKB = 1024;

// ----------------------------------------------------------------------------
// CAlwaysOnlinePop3Agent()
// ----------------------------------------------------------------------------
CAlwaysOnlinePop3Agent::CAlwaysOnlinePop3Agent( 
    CMsvSession& aSession,
    CClientMtmRegistry& aClientMtmRegistry,
    MAlwaysOnlineStatusQueryInterface& aAlwaysOnlineManager,
    CAlwaysOnlineEmailAgent& aEmailAgent )
    :
    CAlwaysOnlineEmailAgentBase( 
        aSession, aClientMtmRegistry, 
        aAlwaysOnlineManager, aEmailAgent )
    {
    }

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
CAlwaysOnlinePop3Agent* CAlwaysOnlinePop3Agent::NewL( 
    CMsvSession& aSession,
    CClientMtmRegistry& aClientMtmRegistry,
    TMsvId aMailboxId,
    MAlwaysOnlineStatusQueryInterface& aAlwaysOnlineManager,
    CAlwaysOnlineEmailAgent& aEmailAgent )
    {
    AOLOG_IN( "CAlwaysOnlinePop3Agent::NewL" );
    CAlwaysOnlinePop3Agent* self = new(ELeave)CAlwaysOnlinePop3Agent( 
        aSession, aClientMtmRegistry, aAlwaysOnlineManager, aEmailAgent );
    CleanupStack::PushL( self );
    self->ConstructL( aMailboxId );
    CleanupStack::Pop( self );
    return self;
    }


// ----------------------------------------------------------------------------
// ConstructL()
// ----------------------------------------------------------------------------
void CAlwaysOnlinePop3Agent::ConstructL( TMsvId aMailboxId )
    {
    AOLOG_IN( "CAlwaysOnlinePop3Agent::ConstructL" );
    CAlwaysOnlineEmailAgentBase::ConstructL( aMailboxId );
    iPop3ClientMtm = (CPop3ClientMtm*) iClientMtmRegistry.NewMtmL( 
        KSenduiMtmPop3Uid );
    iFlags->ClearFlag( EAOBFIsImap4 );    
    }

// ----------------------------------------------------------------------------
// ~CAlwaysOnlinePop3Agent()
// ----------------------------------------------------------------------------
CAlwaysOnlinePop3Agent::~CAlwaysOnlinePop3Agent()
    {
    AOLOG_IN( "CAlwaysOnlinePop3Agent::~CAlwaysOnlinePop3Agent" );
    // cancel ongoing operations
    CloseServices();
    delete iPop3ClientMtm;
    }

// ----------------------------------------------------------------------------
// StartL()
// ----------------------------------------------------------------------------
void CAlwaysOnlinePop3Agent::StartL()
    {
    AOLOG_IN( "CAlwaysOnlinePop3Agent::StartL" );
    //once start delay op has been started we don't ever reset that id.
    if ( iStartDelayOpId == KErrNotFound )
        {
        //check offline & roaming statuses from AO server
        TBool suspended = EFalse;
        QueryAndHandleAOServerInfoL( suspended );
        
        if ( suspended )
            {
            //we got iState = EEmailAgentIdle from CAlwaysOnlineEmailAgentBase::Suspend
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlinePop3Agent::StartL() We have been suspended, not starting!");
            return;
            }

        iState = EEmailAgentInitialised;
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::StartL. Setting state: %d ",iState );

    
        TTime home;
        home.HomeTime();

        //Let's not start immediately on start. Small delay is safer
        TTimeIntervalSeconds startDelay( KInitWaitSeconds );
        home += startDelay;

        StartTimerOperationL( home, iStartDelayOpId );
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::StartL(). Activated startDelayWait 5sec Op Id: %d ",iStartDelayOpId );
        }//if
    else
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlinePop3Agent::StartL() called, but we're allready started. No changes to state machine");
        }
    }

// ----------------------------------------------------------------------------
// ConnectAndUpdateHeadersL()
// ----------------------------------------------------------------------------
void CAlwaysOnlinePop3Agent::ConnectAndUpdateHeadersL()
    {
    AOLOG_IN( "CAlwaysOnlinePop3Agent::ConnectAndUpdateHeadersL" );
    KAOEMAIL_LOGGER_WRITE_FORMAT( "CAlwaysOnlinePop3Agent::ConnectAndUpdateHeadersL(). Starting to connect into: %d", iEntry->Entry().Id() );
    CreatePop3OperationL( iConnectOpId, KPOP3MTMConnect, EFalse );
    iState = EEmailAgentConnecting;
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::ConnectAndUpdateHeadersL(). Setting state:  %d ",iState );
    }

// ----------------------------------------------------------------------------
// DisconnectL()
// ----------------------------------------------------------------------------
void CAlwaysOnlinePop3Agent::DisconnectL( TBool aAutomatic )
    {
    AOLOG_IN( "CAlwaysOnlinePop3Agent::DisconnectL" );    
    TBuf8<1> dummyParams;
    dummyParams.Zero();

    CMsvSingleOpWatcher* watcher = CMsvSingleOpWatcher::NewL(*this);
    CleanupStack::PushL( watcher );
    CMsvOperation* op = NULL;

    if ( iEntry->Entry().Connected() )
        {
        CMsvEntrySelection* selection = new(ELeave) CMsvEntrySelection;
        CleanupStack::PushL( selection );
        selection->AppendL( iEntry->Entry().iServiceId );
        iPop3ClientMtm->SwitchCurrentEntryL( iEntry->Entry().iServiceId );
        op = iPop3ClientMtm->InvokeAsyncFunctionL( 
            KPOP3MTMDisconnect, *selection, dummyParams, watcher->iStatus);

        CleanupStack::PopAndDestroy( selection );//selection
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::DisconnectL: ADDING DISCONNECT OPERATION: %d", op->Id() );
        }
    else 
        {
        //already disconnected
        op = CMsvCompletedOperation::NewL(
                iSession, iEntry->Entry().iMtm, KNullDesC8, 
                KMsvLocalServiceIndexEntryId, 
                watcher->iStatus, KErrCancel);
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::DisconnectL: ADDING COMPLETED OPERATION: %d", op->Id() );
        }

    CleanupStack::PushL( op );
    AppendWatcherAndSetOperationL( watcher, op ); // takes immediately ownership
    CleanupStack::Pop( 2, watcher); // op // CSI: 12,47 # nothing wrong

    if ( aAutomatic )
        {
        iState = EEmailAgentAutoDisconnecting;
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::DisconnectL. Automatic, setting state:  %d ",iState );
        }
    else
        {
        iState = EEmailAgentUserDisconnecting;
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::DisconnectL. Setting state:  %d ",iState );
        }
    }


// ----------------------------------------------------------------------------
// ChangeNextStateL
// ----------------------------------------------------------------------------
void CAlwaysOnlinePop3Agent::ChangeNextStateL()
    {
    AOLOG_IN( "CAlwaysOnlinePop3Agent::ChangeNextStateL" ); 
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::ChangeNextStateL() iState: %d", iState);
    
    //if we are waiting for schedule to start, we don't handle conn errors.
    //this should only occur if conn op has been started just before schedule ends.
    if ( iError != KErrNone && iState != EEmailAgentTimerWaitingForStart )
        {
        HandleOpErrorL();
        }

    switch ( iState )
        {
        //agent just constructed ( or settings changed ) and ready. Should be here only when settings have be saved
        //with always / home settings
        case EEmailAgentInitialised:
            //settings may have been changed, reset all pending operations
            //because their timings can be different compared to newly saved settings

            ResetAll();
            
            if ( !IsEmn() )
                {
                CheckAndHandleSchedulingL();//starts schedule timers and sets state accordingly
                if( iWaitForStopOpId != KErrNotFound || 
                    ( iWaitForStopOpId == KErrNotFound && 
                      iWaitForStartOpId == KErrNotFound ) )//no start or stop waiters, all times
                    {
                    //CheckAndHandleCheduling started waitForStopTimer
                    StartWaitTimerL();
                    }
                //else, do nothing, we wait until waitForStartTimer expires and starts updates again
                }
            else
                {
                iState = EEmailAgentIdle;
                CreateCompletedOpL();
                KAOEMAIL_LOGGER_WRITE("CAlwaysOnlinePop3Agent::ChangeNextStateL() EMN is on --> no scheduling or timers needed!");
                }
            break;

        //case EEmailAgentConnected:
        case EEmailAgentConnecting:
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlinePop3Agent::ChangeNextStateL() DoFilteredPopulateL");
            //if filter not used, just moves to next state - disconnect
            DoFilteredPopulateL();
            break;
        case EEmailAgentDoingFilteredPopulate:
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlinePop3Agent::ChangeNextStateL() DisconnectL");
            DisconnectL( ETrue );
            break;
        case EEmailAgentConnectFailed:
        case EEmailAgentQueued:
        case EEmailAgentReconnecting:
        case EEmailAgentAutoDisconnecting:
        case EEmailAgentUserDisconnecting:
        case EEmailAgentConnTerminated:
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlinePop3Agent::ChangeNextStateL() StartWaitTimerL");
            if ( IsReconnectAfterError() )
                {
                StartWaitTimerL();
                }
            else
                {
                DisconnectL( ETrue );
                // disconnect if we are connected
                iState = EEmailAgentIdle;
                //give up, reset retry counter
                iRetryCounter = 0;
                CreateCompletedOpL();
                }
            break;
        case EEmailAgentTimerWaiting:
            //just make sure that wait has completed before connecting
            ConnectIfAllowedL( iIntervalWaitId );
            break;

        case EEmailAgentTimerWaitingForStart://wait end, can connect
            ConnectIfAllowedL( iWaitForStartOpId );
            //else, do nothing, we are not going to change state until waitForStart timer completes
            break;
        case EEmailAgentIdle:
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlinePop3Agent::ChangeNextStateL() Idling.... zZzZzZzZzzzzzzzzzzzzz");
            //No handling for idle, we just "hang around"
            break;
        case EEmailAgentFatalError:
            //something is so wrong that connection cannot be made without user intervention
            //like incorrect settings or such
            DisplayGlobalErrorNoteL();
            SwitchAutoUpdateOffL();
            break;
        default:
            KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::ChangeNextStateL() Unknown state!: %d,resetting all!", iState);
            ResetAll();
            iState = EEmailAgentInitialised;
            CreateCompletedOpL();
            break;
        }
    }

// ----------------------------------------------------------------------------
// StartWaitTimerL
// ----------------------------------------------------------------------------
void CAlwaysOnlinePop3Agent::StartWaitTimerL( )
    {
    AOLOG_IN( "CAlwaysOnlinePop3Agent::StartWaitTimerL" );
    //waiting already
    if ( iState == EEmailAgentTimerWaiting )
        {
        KAOEMAIL_LOGGER_FN1("CAlwaysOnlinePop3Agent::StartWaitTimerL(), already waiting");
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
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::StartWaitTimerL. Connecting in 5 seconds. Op Id:  %d ",iIntervalWaitId );
        return;
        }

    //if we are retrying a connection. 3 quick retries and then qive up
    if ( (iRetryCounter % KAOMaxRetries) != 0 )
        {
        TTimeIntervalSeconds seconds( 0 );
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::StartWaitTimerL() iRetryCounter %d", iRetryCounter);

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
    else if( iState == EEmailAgentQueued )
        {
        TTimeIntervalSeconds seconds( 0 );
        seconds = EEMailAgentReconnectFour;
        time += seconds;
        }
    else if ( !IsEmn() )
        {
        TInt minutes = RetrievalIntervalInMinutes( LoadSettingL<TInt>( 
            TImumDaSettings::EKeyAutoRetrievalInterval, EFalse ));
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::StartWaitTimerL(). Minutes to wait: %d", minutes);

        TTimeIntervalMinutes intervalMinutes( minutes );
        time += intervalMinutes;
        }

    StartTimerOperationL( time, iIntervalWaitId );
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::StartWaitTimerL. Interval timer started. Op Id:  %d ",iIntervalWaitId );

    iState = EEmailAgentTimerWaiting;
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::StartWaitTimerL. Setting state:  %d ",iState );
    }

// ----------------------------------------------------------------------------
// CreatePop3OperationL()
// ----------------------------------------------------------------------------
void CAlwaysOnlinePop3Agent::CreatePop3OperationL( 
    TInt aFunctionId, 
    TBool aCompletedOperation /*= EFalse*/ )
    {
    AOLOG_IN( "CAlwaysOnlinePop3Agent::CreatePop3OperationL" );
    TMsvOp dummy;
    CreatePop3OperationL( dummy, aFunctionId, aCompletedOperation );
    }

// ---------------------------------------------------------------------------
// CAlwaysOnlinePop3Agent::FillMailOptionsL()
// ---------------------------------------------------------------------------
//
TImPop3PopulateOptions CAlwaysOnlinePop3Agent::FillMailOptionsL()
    {
    AOLOG_IN( "CAlwaysOnlinePop3Agent::FillMailOptionsL" );
    TImPop3PopulateOptions pop3GetMailInfo;

    TInt32 sync = LoadSettingL<TInt32>( 
        TImumInSettings::EKeySyncFlags, ETrue );

    if ( sync == TImumInSettings::EFlagDownloadPartialCumulative )
        {
        // Population limit is set only if user defined limit has been defined
        TInt32 limit = LoadSettingL<TInt32>(
            TImumInSettings::EKeyDownloadBodySize, ETrue );
        pop3GetMailInfo.SetPopulationLimit(
            ( limit * KEmailKB ) / KEmailMaxCharsInLine );
        }
        
    return pop3GetMailInfo;
    }

// ----------------------------------------------------------------------------
// CreatePop3OperationL
// ----------------------------------------------------------------------------
void CAlwaysOnlinePop3Agent::CreatePop3OperationL( 
    TMsvOp& aOpId, 
    TInt aFunctionId, 
    TBool aCompletedOperation /*= EFalse*/ )
    {
    AOLOG_IN( "CAlwaysOnlinePop3Agent::CreatePop3OperationL" );
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::CreatePop3OperationL() aFunctionId %d", aFunctionId);

    CMsvEntrySelection* selection = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    CMsvSingleOpWatcher* watcher = CMsvSingleOpWatcher::NewL(*this);
    CleanupStack::PushL( watcher );
    CMsvOperation* op = NULL;

    if ( aCompletedOperation )
        {
        op = CMsvCompletedOperation::NewL(  // CSI: 35 # cleanupstack is used
                iSession, iEntry->Entry().iMtm, KNullDesC8, 
                KMsvLocalServiceIndexEntryId, 
                watcher->iStatus, KErrCancel);

        aOpId = op->Id();

        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::CreatePop3OperationL: ADDING COMPLETED OPERATION. ID: %d", aOpId );
        }
    else
        {
        selection->AppendL( iEntry->Entry().iServiceId );
        iPop3ClientMtm->SwitchCurrentEntryL( iEntry->Entry().iServiceId );

        TImPop3PopulateOptions pop3GetMailInfo = FillMailOptionsL();
        // Add the population limit to packet for fetch operation
        TPckg<TImPop3PopulateOptions> params(pop3GetMailInfo);
        
        // Begin the fetching operation
        op = iPop3ClientMtm->InvokeAsyncFunctionL( 
            aFunctionId, *selection, params, watcher->iStatus);

        aOpId = op->Id();
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::CreatePop3OperationL: ADDING CONNECT OPERATION. ID: %d", aOpId );
        }

    CleanupStack::PushL( op );
    AppendWatcherAndSetOperationL( watcher, op ); // takes immediately ownership
    CleanupStack::Pop( 2, watcher); // op // CSI: 12,47 # nothing wrong
    CleanupStack::PopAndDestroy( selection );//selection
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlinePop3Agent::CreatePop3OperationL");
    }


// ----------------------------------------------------------------------------
// DoFilteredPopulateL
// ----------------------------------------------------------------------------
void CAlwaysOnlinePop3Agent::DoFilteredPopulateL()
    {
    AOLOG_IN( "CAlwaysOnlinePop3Agent::DoFilteredPopulateL" );
    TInt32 sync = LoadSettingL<TInt32>( 
        TImumInSettings::EKeySyncFlags, ETrue );

    if ( sync == TImumInSettings::EFlagDownloadPartialCumulative ||
         sync == TImumInSettings::EFlagSetDownloadAll )
        {
        // Either user defined limit or full download is selected
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::DoFilteredPopulateL(): Doing filtered populate with setting: %d", sync );
        iState = EEmailAgentDoingFilteredPopulate;
        CreatePop3OperationL( 
            iFilteredPopulateOpId, 
            KPOP3MTMPopulateNew,
            EFalse );
        }
    else
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlinePop3Agent::DoFilteredPopulateL(): Filters not set. Will not populate." );
        //set state and do completed op to step to next state
        iState = EEmailAgentDoingFilteredPopulate;
        CreateCompletedOpL();
        }
    }

// ----------------------------------------------------------------------------
// HandleOpErrorL
// ----------------------------------------------------------------------------
void CAlwaysOnlinePop3Agent::HandleOpErrorL()
    {
    AOLOG_IN( "CAlwaysOnlinePop3Agent::HandleOpErrorL" );
    AOLOG_WRMV( "iError: ",  EAoNormalError, iError, EAoLogSt3 );
    KAOEMAIL_LOGGER_WRITE("CAlwaysOnlinePop3Agent::HandleOpErrorL() Error Handling started" );
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::HandleOpErrorL() Error ID: %d", iError );
    
    switch ( iError )
        {
        case KErrNone:
            //shouldn't come here if KErrNone
            break;
        case KErrAbort:
        case KErrCancel:
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlinePop3Agent::HandleOpErrorL(): KErrCancel");
            //sometimes we seem to get these errors with connection operations.
            //so go to initialised state -> reset all
            iError = KErrNone;
            iState = EEmailAgentInitialised; //this will be changed where those timers were cancelled.
            break;
        
        case KErrGeneral://seems that this is what is given when phone/csd is in use
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlinePop3Agent::HandleOpErrorL(): KErrGeneral");
            iState = EEmailAgentQueued;
            KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::HandleOpErrorL. Setting state:  %d ",iState );
            break;
        case KErrInUse:
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlinePop3Agent::HandleOpErrorL(): KErrInUse");
            iState = EEmailAgentQueued;
            KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::HandleOpErrorL. Setting state:  %d ",iState );
            break;
        //fatal errors, cannot recover without user intervention
        case KPop3InvalidUser:
        case KPop3InvalidLogin:
        case KPop3InvalidApopLogin:
        case KErrPop3TLSNegotiateFailed:
        case KImskSSLTLSNegotiateFailed:
        case KErrCorrupt:
            {
            iState = EEmailAgentFatalError;
            iRetryCounter = 0;
            KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlinePop3Agent::HandleOpErrorL. Setting state:  %d ",iState );
            }
            break;
        //This error occurs sometimes when network is reserverd or there is a dns problems
        case KErrAoServerNotFound:
            HandleDefaultError();
            break;
            
        case KErrConnectionTerminated://red key
        case KErrTimedOut:
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlinePop3Agent::HandleOpErrorL. KErrConnectionTimedOut(-33) OR KErrConnectionTerminated");
            iState = EEmailAgentConnTerminated;
            break;
        //let these go to default retry mechanism
        case KImskErrorDNSNotFound:
        case KImskErrorControlPanelLocked:
        case KImskErrorISPOrIAPRecordNotFound:
        case KImskErrorActiveSettingIsDifferent:
        case KImskSecuritySettingsFailed:
        case KPop3CannotCreateApopLogonString:            
        case KPop3ProblemWithRemotePopServer:            
        case KPop3CannotOpenServiceEntry:            
        case KPop3CannotSetRequiredFolderContext:
        case KPop3CannotConnect:
        default:
            HandleDefaultError();
            break;
        };
    //reset error
    iError = KErrNone;
    }

// ----------------------------------------------------------------------------
// DoSyncDisconnectL
// ----------------------------------------------------------------------------
void CAlwaysOnlinePop3Agent::DoSyncDisconnectL()
    {
    AOLOG_IN( "CAlwaysOnlinePop3Agent::DoSyncDisconnectL" );
    if ( iEntry->Entry().Connected() )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlinePop3Agent::DoSyncDisconnectL() Doing SyncDisconnect! ");

    	TBuf8<1> dummyParams;
    	dummyParams.Zero();

		// Needed to change next operation to synchronized
        CMsvOperationActiveSchedulerWait* wait = 
        	CMsvOperationActiveSchedulerWait::NewLC();
        
        CMsvEntrySelection* selection = new(ELeave) CMsvEntrySelection;
        CleanupStack::PushL( selection );

        selection->AppendL( iEntry->Entry().iServiceId );
        iPop3ClientMtm->SwitchCurrentEntryL( iEntry->Entry().iServiceId );
        CMsvOperation* op = iPop3ClientMtm->InvokeAsyncFunctionL( 
            KPOP3MTMDisconnect, *selection, dummyParams, wait->iStatus);

		// Start wait object
        wait->Start();
        
        delete op;
        CleanupStack::PopAndDestroy( 2, wait ); // selection // CSI: 12,47 # nothing wrong

        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlinePop3Agent::DoSyncDisconnectL() Should be disconnected now! ");
        }
    }

// ----------------------------------------------------------------------------
//  HandleOpCompleted
// ----------------------------------------------------------------------------
void CAlwaysOnlinePop3Agent::HandleOpCompleted( 
    TMsvOp opId, 
    TInt aCompletionCode )
    {
    AOLOG_IN( "CAlwaysOnlinePop3Agent::HandleOpCompleted" );
    TInt trapErr;
    
    //save error code so we can handle errors elsewhere
    if ( opId == iConnectOpId && aCompletionCode != KErrNone )
        {
        iError = aCompletionCode;
        SetLastUpdateFailed();
        }
    else if ( opId == iConnectOpId )
        {
        iError = KErrNone;
        iConnectOpId = KErrNotFound;
        iRetryCounter = 0; //reset retry counter

        //set last successful update date and time to settings
        SetLastSuccessfulUpdate();
        
        }
    else if ( opId == iFilteredPopulateOpId )
        {
        iFilteredPopulateOpId = KErrNotFound;
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
            }
        }
    else if ( opId == iWaitForStartOpId )
        {
        if ( aCompletionCode == KErrNone )
            {
            iWaitForStartOpId = KErrNotFound;
            TRAP( trapErr, CheckAndHandleSchedulingL() );
            }
        }
    else if ( opId == iStartDelayOpId )
        {
        iFlags->SetFlag( EAOBFConnectNow );        
        //leave id untouched. We need this only once, on bootup, or when agent is activated via settings
        //we use this to check is it used already
        //iStartDelayOpId;
        }
    
    TRAP_IGNORE( CallNewMessagesL() );
    }

// ----------------------------------------------------------------------------
//  CreateCompletedOpL
// ----------------------------------------------------------------------------
void CAlwaysOnlinePop3Agent::CreateCompletedOpL()
    {
    AOLOG_IN( "CAlwaysOnlinePop3Agent::CreateCompletedOpL" );
    CreatePop3OperationL( 0, ETrue );//creates completed operation
    }

// ----------------------------------------------------------------------------
//  MailHasBeenSyncedL()
// ----------------------------------------------------------------------------
TBool CAlwaysOnlinePop3Agent::MailHasBeenSyncedL()
    {
    AOLOG_IN( "CAlwaysOnlinePop3Agent::MailHasBeenSyncedL" );

    //we need to set this only once during the lifetime of ImapAgent
    if ( iHasBeenSynced )
        {
        return iHasBeenSynced;
        }

    iHasBeenSynced = EFalse;
    
    CMsvEntrySelection* sel = iEntry->ChildrenL();
    CleanupStack::PushL( sel );
    if ( sel->Count() )
        {
        iHasBeenSynced = ETrue;
        }
    CleanupStack::PopAndDestroy( sel );

    return iHasBeenSynced;
    }
