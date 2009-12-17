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


// INCLUDES
#include <AlwaysOnlineManagerCommon.h>
#include <AlwaysOnlineStatusQueryInterface.h>
#include <pop3set.h>
#include <imapset.h>
#include <SenduiMtmUids.h>
#include <StringLoader.h>
#include <AknGlobalNote.h>
#include <stringresourcereader.h>
#include <avkon.rsg>                            // Resouce identifiers
#include <iapprefs.h>
#include <rconnmon.h>
#include <cdbcols.h>							// for some constants
#include <MuiuMsvUiServiceUtilities.h>
#include <messagingvariant.hrh>                 // Local variation flags
#include <data_caging_path_literals.hrh>
#include <messaginginternalcrkeys.h>            // Messaging keys
#include <muiuemailtools.h>                     // MuiuEmailTools
#include <ImumInternalApi.h>
#include <ImumInSettingsData.h>
#include <ImumInSettingsKeys.h>

#include <cmconnectionmethoddef.h>
#include <cmpluginvpndef.h>
#include <ImumDaSettingsKeys.h>

#include "AlwaysOnlineEmailPluginLogging.h"
#include "AlwaysOnlineEmailLoggingTools.h"
#include "AlwaysOnlineEmailAgentBase.h"
#include "AlwaysOnlineEmailPluginTimer.h"
#include "AlwaysOnlineEmailAgent.h"
#include <AlwaysOnlineEmailPluginData.rsg>

// CONSTANTS
// Correct path is added to literal when it is used.
_LIT(KEmailPluginResource, "AlwaysOnlineEmailPluginData.rsc");
const TInt KEmailAgentBaseOperationsGranularity = 2;
const TInt KFileLoggerCounter = 100;
const TInt KMailSettingsIapPreferenceNumber     = 0;
const TInt KEmailPluginQueryTextLength    = 150;

const TInt KAoInterval5Min   = 5;
const TInt KAoInterval15Min  = 15;
const TInt KAoInterval30Min  = 30;
const TInt KAoInterval1Hour  = 60;
const TInt KAoInterval2Hours = 120;
const TInt KAoInterval4Hours = 240;
const TInt KAoInterval6Hours = 360;

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase()
// ----------------------------------------------------------------------------
CAlwaysOnlineEmailAgentBase::CAlwaysOnlineEmailAgentBase(
    CMsvSession& aSession, CClientMtmRegistry& aClientMtmRegistry,
    MAlwaysOnlineStatusQueryInterface& aAlwaysOnlineManager,
    CAlwaysOnlineEmailAgent& aEmailAgent )
    :
    iClientMtmRegistry( aClientMtmRegistry ),
    iSession( aSession ),
    iEntry ( NULL ),
    iOperations( KEmailAgentBaseOperationsGranularity ),
    iState( EEmailAgentInitialised ),
    iRetryCounter( 0 ),
    iLoggerFileCounter( KFileLoggerCounter ),
    iMailboxApi( NULL ),
    iMailboxSettings( NULL ),
    iError( KErrNone ),
    iNcnNotification ( NULL ),
    iStatusQueryInterface( aAlwaysOnlineManager ),
    iEmailAgent( aEmailAgent ),
    iFlags( NULL )
    {
    }

// ----------------------------------------------------------------------------
// ~CAlwaysOnlineEmailAgentBase ()
// ----------------------------------------------------------------------------
CAlwaysOnlineEmailAgentBase::~CAlwaysOnlineEmailAgentBase()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::~CAlwaysOnlineEmailAgentBase" );
    delete iMailboxApi;
    iMailboxApi = NULL;
    delete iMailboxSettings;
    iMailboxSettings = NULL;

    delete iNcnNotification;
    delete iEntry;

    iCmManager.Close();
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::ConstructL( TMsvId aMailboxId )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::ConstructL" );
    iIntervalWaitId = KErrNotFound;
    iConnectOpId = KErrNotFound;
    iConnectSyncOpId = KErrNotFound;
    iSyncOpId = KErrNotFound;
    iWaitForStartOpId = KErrNotFound;
    iWaitForStopOpId = KErrNotFound;
    iStartDelayOpId = KErrNotFound;
    iConnectAndStayOnlineOpId = KErrNotFound;
    iDisconnectOpId = KErrNotFound;
    iFolderUpdateTimerOpId = KErrNotFound;
    iFolderSyncOpId = KErrNotFound;
    iFilteredPopulateOpId = KErrNotFound;

    iEntry = iSession.GetEntryL( aMailboxId );
    // Handle flags, including variation
    AlwaysOnlineFlagsL();

    iMailboxApi = CreateEmailApiL( &iSession );
    iMailboxSettings =
        iMailboxApi->MailboxServicesL().LoadMailboxSettingsL( aMailboxId );


    iRetryCounter = 0;

    iCmManager.OpenL();
    }

// ----------------------------------------------------------------------------
// OpCompleted
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::OpCompleted(
    CMsvSingleOpWatcher& aOpWatcher,
    TInt /*aCompletionCode*/ )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::OpCompleted" );
    CMsvOperation* op=&aOpWatcher.Operation();
    TMsvOp opId = op->Id();
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgentBase::OpCompleted() Op Id: %d", opId);
    const TInt count = iOperations.Count();

    for ( TInt i = count - 1; i >= 0; i-- )
        {
        // We need to check the id of a completed operation that it
        // matches to what we expect, because there may be several
        // simultaneous operations and there is no guarantee that
        // they comlete in the order we think they do.
        CMsvOperation& oper = iOperations[i]->Operation();

        if ( oper.Id() == opId )
            {
            CMsvSingleOpWatcher* opWatcher = iOperations[i];
            KAOEMAIL_LOGGER_WRITE_FORMAT(">>>> iOperations count before : %d", iOperations.Count());
            iOperations.Delete( i );
            // The operations matches, handle it
            HandleCompletingOperation( opId, *op, op->Mtm() );
            KAOEMAIL_LOGGER_WRITE_FORMAT(">>>> iOperations count after : %d", iOperations.Count());
            delete opWatcher;

            break;
            }
        }

    // Change the state
    TRAPD( err, ChangeNextStateL() );
    if ( err != KErrNone )
        {
        //not much to do if this fails for some reason. Just reset all and restart
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgentBase::OpCompleted() ChangeNextStateL returned error: %d , resetting all!", err );
        ResetAll();
        iState = EEmailAgentInitialised;

        // Just catch the possible leaving and continue execution
        TRAP( err, CreateCompletedOpL() );
        }

    }

// ----------------------------------------------------------------------------
// AppendWatcherAndSetOperationL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::AppendWatcherAndSetOperationL(
    CMsvSingleOpWatcher* aWatcher,
    CMsvOperation* aOperation )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::AppendWatcherAndSetOperationL" );
    iOperations.AppendL( aWatcher );
    aWatcher->SetOperation( aOperation ); // takes immediately ownership

    KAOEMAIL_LOGGER_WRITE( "-------------------<APPEND: OpInfo>-----------------" );
    KAOEMAIL_LOGGER_WRITE_FORMAT( "Completing operation: aOpId: %d", aOperation->Id() );
    KAOEMAIL_LOGGER_WRITE_FORMAT( "Completing operation: aUid: %d", aOperation->Mtm().iUid );
    KAOEMAIL_LOGGER_WRITE_FORMAT( "Completing operation: Op Count: %d", iOperations.Count() );
    KAOEMAIL_LOGGER_WRITE( "-------------------<APPEND: OpInfo>-----------------" );
    }

// ----------------------------------------------------------------------------
// MailboxId
// ----------------------------------------------------------------------------
TMsvId CAlwaysOnlineEmailAgentBase::MailboxId() const
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::MailboxId" );
    return iEntry->Entry().Id();
    }

//-----------------------------------------------------------------------------
// EmailAddress
//-----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::EmailAddress( TDes& aEmailAddress )
    {
    iMailboxSettings->GetAttr(
        TImumDaSettings::EKeyEmailAddress, aEmailAddress );
    }

//-----------------------------------------------------------------------------
// Username
//-----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::Username( TDes8& aUsername )
    {
    // trap ignored, only leave descriptor empty in case of error
    TRAP_IGNORE( aUsername = LoadSettingL<TImumDaSettings::TTextUserName>( // CSI: 59 # trap has leaving code
                 TImumDaSettings::EKeyUsername, ETrue); );
    }


//-----------------------------------------------------------------------------
// ServerAddress
//-----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::ServerAddress( TDes& aServer )
    {
    // leave descriptor empty in case of error
    TRAP_IGNORE( aServer = LoadSettingL<TImumDaSettings::TTextServerAddress>( // CSI: 59 # trap has leaving code
                 TImumDaSettings::EKeyServer, ETrue); );
    }

//-----------------------------------------------------------------------------
// StartTimerOperationL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::StartTimerOperationL(
    const TTime& aTime,
    TMsvOp& aOpId )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::StartTimerOperationL" );
    CMsvSingleOpWatcher* watcher = CMsvSingleOpWatcher::NewL(*this);
    CleanupStack::PushL( watcher );

    CAlwaysOnlineEmailPluginTimer* timer = 
        CAlwaysOnlineEmailPluginTimer::NewL( iSession, watcher->iStatus );
    CleanupStack::PushL( timer );
    KAOEMAIL_LOGGER_WRITE_DATETIME("CAlwaysOnlineEmailAgentBase::StartTimerOperationL() Expires at: ", aTime);
    timer->At( aTime );  // CSI: 1 # not an array

    aOpId = timer->Id();

    AppendWatcherAndSetOperationL( watcher, timer ); // takes immediately ownership
    CleanupStack::Pop( 2, watcher ); // timer // CSI: 12,47 # nothing worng
    }


// ---------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::AoState()
// ---------------------------------------------------------------------------
//
TInt CAlwaysOnlineEmailAgentBase::AoState() const
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::AoState" );
    TInt state = 0;
    iMailboxSettings->GetAttr(
        TImumDaSettings::EKeyAutoRetrieval, state );
    return state;
    }

// ---------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::EmnState()
// ---------------------------------------------------------------------------
//
TInt CAlwaysOnlineEmailAgentBase::EmnState() const
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::EmnState" );
    TInt state = 0;
    iMailboxSettings->GetAttr(
        TImumDaSettings::EKeyAutoNotifications, state );
    return state;
    }


// ----------------------------------------------------------------------------
// ConnectL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::ConnectL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::ConnectL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineEmailAgentBase::ConnectL");

    if ( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelWithOverheadL(
            iSession, 0, KAOSafetyMargin ) && !IsTemporary() )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::ConnectL(): Disk full, setting AO of.");
        DisplayGlobalErrorNoteL( EEmailAgentOutOfDisk );
        SwitchAutoUpdateOffL();
        return;
        }

    TBool allowed = ETrue;

    TBool offline =
        reinterpret_cast<TBool>( iStatusQueryInterface.QueryStatusL(
            EAOManagerStatusQueryOffline ) );

    if ( offline )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::ConnectL(). Trying to connect while offline mode! Not allowing!");
        allowed = EFalse;
        }

    TAlwaysOnlineEmailIAPNotes invalidity;

    if ( allowed && IsIAPInvalidL( invalidity ) )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::ConnectL(). Accesspoint faulty or prompting password, switching off!");
        DisplayGlobalErrorNoteL( invalidity );
        SwitchAutoUpdateOffL();
        return;
        }


    if ( allowed && AoState() == TImumDaSettings::EValueAutoOff &&
        EmnState() == TImumDaSettings::EValueNotificationsOff )
        {
        // User is propably in settings, and we're turned off.
        // Wait timer event just has been running and completed while
        // user launched settings. Don't connect.
        allowed = EFalse;
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::ConnectL(). Settings dialog seems to be open. Won't connect.");
        }

    // UI takes care that either AO or EMN is turned on.
    if ( allowed && (AoState() == TImumDaSettings::EValueAutoHomeNetwork ||
        EmnState() == TImumDaSettings::EValueNotificationsHome) )
        {
        TInt networkStatus =
            reinterpret_cast< TInt >( iStatusQueryInterface.QueryStatusL(
                EAOManagerStatusQueryNetwork ) );

        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgentBase::ConnectL() Network status: %d", networkStatus );

        if( networkStatus != ENetworkRegistrationHomeNetwork )
            {
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::ConnectL(). We're not on home network. Won't connect");
            allowed = EFalse;
            }
        }

    if ( !allowed )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::ConnectL(). Connect not allowed -> idle");
        iState = EEmailAgentIdle;
        CreateCompletedOpL();
        return;
        }

    ConnectAndUpdateHeadersL();
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineEmailAgentBase::ConnectL");
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::CleanOperation()
// ----------------------------------------------------------------------------
//
void CAlwaysOnlineEmailAgentBase::CleanOperation( TMsvOp& aOpId )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::CleanOperation" );
    const TInt count = iOperations.Count();

    for ( TInt i=0; i < count; i++ )
        {
        if ( iOperations[i]->Operation().Id() == aOpId )
            {
            KAOEMAIL_LOGGER_WRITE( "-------------------<REMOVE: OpInfo>-----------------" );
            KAOEMAIL_LOGGER_WRITE_FORMAT( "Completing operation: aOpId: %d", aOpId );
            KAOEMAIL_LOGGER_WRITE_FORMAT( "Completing operation: aUid: %d", iOperations[i]->Operation().Mtm().iUid );

            // First set id to KErrNotFound so OpCompleted won't
            // take any action because of this
            aOpId = KErrNotFound;
            delete iOperations[i];
            iOperations[i] = NULL;
            iOperations.Delete(i);

            KAOEMAIL_LOGGER_WRITE_FORMAT( "Completing operation: Op Count: %d", iOperations.Count() );
            KAOEMAIL_LOGGER_WRITE( "-------------------<REMOVE: OpInfo>-----------------" );
            break;
            }//if
        }//for
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::LaunchStartTimerL()
// ----------------------------------------------------------------------------
//
void CAlwaysOnlineEmailAgentBase::LaunchStartTimerL(
    TTime& aClock,
    const TTimeIntervalSeconds& aSeconds )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::LaunchStartTimerL" );
    //wait for start
    KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::LaunchStartTimerL() Checking for wait for START schedule");

    // The connection is on, and the connection is required, this means
    // the mailbox in online during the time it should not be
    if ( iConnectAndStayOnlineOpId != KErrNotFound )
        {
        DisconnectL( ETrue );
        }

    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgentBase::LaunchStartTimerL. Minutes to wait ( time ): %d ",( aSeconds.Int()/60 ) ); // CSI: 47 # seconds to minutes

    aClock += aSeconds;
    StartTimerOperationL( aClock, iWaitForStartOpId );

    KAOEMAIL_LOGGER_WRITE_DATETIME("Started waitForStartTimer. Expiring at: ", aClock);

    iState = EEmailAgentTimerWaitingForStart;
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgentBase::LaunchStartTimerL. Setting state:  %d ",iState );

    CleanOperation( iWaitForStopOpId );
    iWaitForStopOpId = KErrNotFound;//set to KErrNotFound

    // If waiting for start, we don't need to connect after saving settings,
    // thus set this to false.
    iFlags->ClearFlag( EAOBFConnectNow );
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::LaunchStopTimerL()
// ----------------------------------------------------------------------------
//
void CAlwaysOnlineEmailAgentBase::LaunchStopTimerL(
    TTime& aClock,
    const TTimeIntervalSeconds& aSeconds )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::LaunchStopTimerL" );
    //wait for stop
    KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::LaunchStopTimerL() Checking for wait for STOP schedule");

    if( aSeconds.Int() == 0)
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::LaunchStopTimerL() All days and all hours scheduled");
        iState = EEmailAgentTimerWaitingForStart;//causes us to connect
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgentBase::LaunchStopTimerL(). Setting state:  %d ",iState );
        CreateCompletedOpL();//creates completed operation
        return;
        }
    else
        {
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgentBase::LaunchStopTimerL. Minutes to wait ( time ): %d ",( aSeconds.Int()/60 + 0.5 ) ); // CSI: 47 # seconds to minutes
        aClock += aSeconds;
        StartTimerOperationL( aClock, iWaitForStopOpId );

        CleanOperation( iWaitForStartOpId );
        iWaitForStartOpId = KErrNotFound;//set to KErrNotFound

        KAOEMAIL_LOGGER_WRITE_DATETIME("Started waitForStopTimer. Expiring at: ", aClock);
        }
    }

// ----------------------------------------------------------------------------
// CheckAndHandleSchedulingL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::CheckAndHandleSchedulingL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::CheckAndHandleSchedulingL" );
    TTime clock;
    clock.HomeTime();

    TTimeIntervalSeconds secondsInterval;
    TInt64 connectionState = 0x00;
    iMailboxApi->MailboxUtilitiesL().NextAlwaysOnlineIntervalL(
        iEntry->Entry().Id(),
        connectionState, secondsInterval );

    KAOEMAIL_LOGGER_WRITE_FORMAT( "Stop Timer: %d", iWaitForStopOpId );
    KAOEMAIL_LOGGER_WRITE_FORMAT( "Start Timer: %d", iWaitForStartOpId );
    KAOEMAIL_LOGGER_WRITE_FORMAT( "Interval Timer: %d", iIntervalWaitId );

    if ( iWaitForStopOpId != KErrNotFound )
        {
        KAOEMAIL_LOGGER_WRITE( "* Cleaning stop timer *" );
        CleanOperation( iWaitForStopOpId );
        }

    if ( iWaitForStartOpId != KErrNotFound )
        {
        KAOEMAIL_LOGGER_WRITE( "* Cleaning start timer *" );
        CleanOperation( iWaitForStartOpId );
        }

    if( iIntervalWaitId != KErrNotFound )
        {
        KAOEMAIL_LOGGER_WRITE( "* Cleaning interval timer *" );
        CleanOperation( iIntervalWaitId );
        }

    // Not connected and should connect
    if ( connectionState & MImumInMailboxUtilities::EFlagWaitingToConnect )
        {
        LaunchStartTimerL( clock, secondsInterval );
        }
    // Connected and should disconnect
    else
        {
        LaunchStopTimerL( clock, secondsInterval );
        }
    }

// ----------------------------------------------------------------------------
// ResetAll
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::ResetAll()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::ResetAll" );
    KAOEMAIL_LOGGER_WRITE_FORMAT("ResetAll() Reset All called. Operation count: %d", iOperations.Count() );

    iOperations.ResetAndDestroy();

    //if we're still connected, do sync disconnet. Connect check is inside this function
    //can leave, so trap. ResetAll() is not leaving function
    TRAP_IGNORE( DoSyncDisconnectL() );

    iIntervalWaitId = KErrNotFound;
    iConnectOpId = KErrNotFound;
    iConnectSyncOpId = KErrNotFound;
    iSyncOpId = KErrNotFound;
    iWaitForStartOpId = KErrNotFound;
    iWaitForStopOpId = KErrNotFound;
    iConnectAndStayOnlineOpId = KErrNotFound;
    iDisconnectOpId = KErrNotFound;
    iFolderUpdateTimerOpId = KErrNotFound;
    iFolderSyncOpId = KErrNotFound;
    iFilteredPopulateOpId = KErrNotFound;
    }


// ----------------------------------------------------------------------------
// Suspend
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::Suspend()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::Suspend" );
    KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::SuspendL() Suspend called. Resetting and going to idle");
    ResetAll();
    iState = EEmailAgentIdle;
    // We may start up suspended, so set this to false.
    iFlags->ClearFlag( EAOBFConnectNow );
    }

// ----------------------------------------------------------------------------
// ResumeL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::ResumeL( const TBool aConnectNow )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::ResumeL" );
    // We are here, because agent got resume command, so we need to refresh our
    // settings objects by deleting current settings. New settings objects are
    // created and loaded when certain setting is used first time after resume.
    // iAlwaysOnlineEmailScheduler's settings reference is also updated, this
    // happens in ExtensionSettings() function.

    // Update the settings here
    delete iMailboxSettings;
    iMailboxSettings = NULL;

    iMailboxSettings =
        iMailboxApi->MailboxServicesL().LoadMailboxSettingsL(
            iEntry->Entry().Id() );

    // Set connection status, so the connection can be made, if needed
    iFlags->ChangeFlag( EAOBFConnectNow, aConnectNow );

    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgentBase::ResumeL() iState: %d", iState );
    if ( iState == EEmailAgentIdle )
        {
        if ( AoState() != TImumDaSettings::EValueAutoOn )
            {
            //check does roaming setting allow us to resume
            TInt networkStatus =
                reinterpret_cast<TInt>( iStatusQueryInterface.QueryStatusL(
                    EAOManagerStatusQueryNetwork ) );

            KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgentBase::ResumeL() Network status: %d", networkStatus );
            if ( networkStatus != ENetworkRegistrationHomeNetwork )
                {
                //not going to resume, stay idle
                KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::ResumeL() Resume called. Home only setting won't allow us to resume. Still idling");
                return;
                }//if
            }//if
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::ResumeL() Resume called. Settings state to Initialised ( 0 )");
        iState = EEmailAgentInitialised;
        CreateCompletedOpL();
        }
    }

// ----------------------------------------------------------------------------
// HandleRoamingEventL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::HandleRoamingEventL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::HandleRoamingEventL" );
    // UI takes care that either AO or EMN is turned on.
    if ( AoState() != TImumDaSettings::EValueAutoOn ||
         EmnState() != TImumDaSettings::EValueNotificationsOn )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::HandleRoamingEventL() Roaming started and we're not set to update during roaming, going to idle");
        ResetAll();
        iState = EEmailAgentIdle;
        }
    }


// ----------------------------------------------------------------------------
// HandleOutOfDiskEventL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::HandleOutOfDiskEventL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::HandleOutOfDiskEventL" );
    KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::HandleOutOfDiskEventL() Ran out of disk! Switching off!");
    // Check if agent is temporary, which means that we are not allowed to show
    // any notes. Only IMAP mailbox can have temporary agent.
    if ( !IsTemporary() )
        {
        DisplayGlobalErrorNoteL(
            TAlwaysOnlineEmailIAPNotes( EEmailAgentOutOfDisk ) );
        SwitchAutoUpdateOffL();
        }
    }

// ----------------------------------------------------------------------------
// SwitchOffL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::SwitchOffL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::SwitchOffL" );
    KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::SwitchOffL() ORDERED TO SWITCH OFF, COMPLYING!");
    SwitchAutoUpdateOffL();
    }

// ----------------------------------------------------------------------------
// HandleHomeNetworkEventL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::HandleHomeNetworkEventL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::HandleHomeNetworkEventL" );
    // This might be true when there is a temporary mail agent.
    if ( AoState() != TImumDaSettings::EValueAutoOn ||
         EmnState() != TImumDaSettings::EValueNotificationsOn )
        {
        if( iState == EEmailAgentIdle )
            {
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::HandleHomeNetworkEventL() HomeNetwork event received, starting up");
            iState = EEmailAgentInitialised;
            CreateCompletedOpL();
            }
        else
            {
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::HandleHomeNetworkEventL() HomeNetwork event received But we're running already...ignore");
            }
        }
    }

// ----------------------------------------------------------------------------
// SetLastSuccessfulUpdate
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::SetLastSuccessfulUpdate()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::SetLastSuccessfulUpdate" );
    TBool lastUpdateFailed = EFalse;
    TTime time = TTime();
    TBool updSuccess = ETrue;
    time.HomeTime();
    iMailboxSettings->SetAttr(
        TImumInSettings::EKeyInfoLastUpdateFailed,
        lastUpdateFailed );
    iMailboxSettings->SetAttr(
        TImumInSettings::EKeyInfoLastSuccessfulUpdate,
        time.Int64() );
    iMailboxSettings->SetAttr(
        TImumInSettings::EKeyAoUpdateSuccessfulWithCurSettings, updSuccess );

    TRAP_IGNORE( iMailboxApi->MailboxServicesL().SaveMailboxSettingsL( *iMailboxSettings ) );
    }

// ----------------------------------------------------------------------------
// SetLastUpdateFailedL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::SetLastUpdateFailed()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::SetLastUpdateFailed" );
    TBool updateFailed = LoadSettingL<TInt>(
        TImumInSettings::EKeyInfoLastUpdateFailed, EFalse );

    if ( !updateFailed )
        {
        iMailboxSettings->SetAttr(
            TImumInSettings::EKeyInfoLastUpdateFailed,
            ETrue );
        TRAP_IGNORE( iMailboxApi->MailboxServicesL().SaveMailboxSettingsL(
            *iMailboxSettings ) );
        }

    }

// ----------------------------------------------------------------------------
// DisplayGlobalErrorNoteL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::DisplayGlobalErrorNoteL(
        TAlwaysOnlineEmailIAPNotes aInvalidity )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::DisplayGlobalErrorNoteL" );
    //NOTE: WHEN USING THIS FUNCTION TO SHOW ERROR NOTES,
    //STRINGS MUST CONTAIN %U PARAMETER FOR MAILBOX NAME

    TFileName resourceFile;
    TParse tp;
    tp.Set( KEmailPluginResource, &KDC_RESOURCE_FILES_DIR, NULL );
    resourceFile.Copy( tp.FullName() );
    CStringResourceReader* resourceReader = NULL;
    TRAPD( err, resourceReader = CStringResourceReader::NewL( resourceFile ) );
    if ( err != KErrNone )
        {
        //if resource not present for some reason, just return.
        //User will not see error note, but no other harm done.
        return;
        }
    CleanupStack::PushL( resourceReader );//can't use LC in TRAPD

    TBuf<KEmailPluginQueryTextLength> tempText;

    switch ( aInvalidity )
        {
        case EEmailAgentCSDIAP:
            tempText = resourceReader->ReadResourceString(
                R_EMAIL_PLUGIN_INVALID_AP_BEARER );
            break;
        case EEmailAgentOutOfDisk:
            tempText = resourceReader->ReadResourceString(
                R_EMAIL_PLUGIN_OUT_OF_DISK );
            break;
        case EEmailAgentInvalidInterval:
        case EEmailAgentEMNFatalError:
        case EEmailAgentOtherInvalidity:
        default:
            tempText = resourceReader->ReadResourceString(
                R_EMAIL_PLUGIN_ALWAYS_INCORRECT );
            break;
        }

    TBuf<KEmailPluginQueryTextLength> noteText;
    StringLoader::Format(
        noteText,
        tempText,
        -1,
        iEntry->Entry().iDetails.Left(
                    KEmailPluginQueryTextLength - tempText.Length() )
        );

    CAknGlobalNote* note = CAknGlobalNote::NewL();
    CleanupStack::PushL( note );
    note->SetSoftkeys( R_AVKON_SOFTKEYS_OK_EMPTY__OK );
    note->ShowNoteL( EAknGlobalErrorNote, noteText );
    CleanupStack::PopAndDestroy( 2, resourceReader ); // note // CSI: 12,47 # nothing wrong
    }


// ----------------------------------------------------------------------------
// QueryAndHandleAOServerInfoL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::QueryAndHandleAOServerInfoL( TBool& aSuspended )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::QueryAndHandleAOServerInfoL" );
    TInt networkStatus =
        reinterpret_cast<TInt>( iStatusQueryInterface.QueryStatusL(
            EAOManagerStatusQueryNetwork ) );

    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgentBase::QueryAndHandleAOServerInfoL() Network status: %d", networkStatus );

    TBool offline =
        reinterpret_cast<TBool>( iStatusQueryInterface.QueryStatusL(
            EAOManagerStatusQueryOffline ) );

    aSuspended = EFalse;

    if ( offline )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::QueryAndHandleAOServerInfoL() We are in offline mode. Going into susped");
        Suspend();
        aSuspended = ETrue;
        }

    if ( !aSuspended &&
         ( CheckIfAgentHomeOnly() &&
           networkStatus != ENetworkRegistrationHomeNetwork ) )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::QueryAndHandleAOServerInfoL() We are not in home network and HomeNetworkOnly is selected. Going into suspend");
        Suspend();
        aSuspended = ETrue;
        }
    }

// ----------------------------------------------------------------------------
// CheckAgentHomeNetwork
// ----------------------------------------------------------------------------
TBool CAlwaysOnlineEmailAgentBase::CheckIfAgentHomeOnly()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::CheckIfAgentHomeOnly" );
    if ( AoState() == TImumDaSettings::EValueAutoHomeNetwork ||
         EmnState() == TImumDaSettings::EValueNotificationsHome )
        {
        return ETrue;
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// SwitchAutoUpdateOffL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgentBase::SwitchAutoUpdateOffL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::SwitchAutoUpdateOffL" );
    KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::SwitchAutoUpdateOffL() Switching Auto Update OFF!");

    //first call suspend so all timer operations will be deleted
    Suspend();

    if ( IsEmn() )
        {
        iMailboxSettings->SetAttr(
            TImumDaSettings::EKeyAutoNotifications,
            TImumDaSettings::EValueNotificationsOff );
        }
    else
        {
        iMailboxSettings->SetAttr(
            TImumDaSettings::EKeyAutoRetrieval,
            TImumDaSettings::EValueAutoOff );
        }

    iMailboxApi->MailboxServicesL().SaveMailboxSettingsL(
        *iMailboxSettings );


    RemoveMe();//completely removes us
    }


// ----------------------------------------------------------------------------
// GetConnectionMethodL
// ----------------------------------------------------------------------------
RCmConnectionMethod CAlwaysOnlineEmailAgentBase::GetConnectionMethodLC()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::GetIapIdL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineEmailAgentBase::GetIapIdL");

    CImIAPPreferences* apPrefs = CImIAPPreferences::NewLC();
    CEmailAccounts* accounts = CEmailAccounts::NewLC();

    // Load account settings according to used protocol
    if ( iFlags->Flag( EAOBFIsImap4 ) )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::GetIapIdL(): Load Imap IAP settings");
        TImapAccount account;
        accounts->GetImapAccountL( iEntry->Entry().Id(), account );
        accounts->LoadImapIapSettingsL( account, *apPrefs );
        }
    else // POP3
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::GetIapIdL(): Load Pop IAP settings");
        TPopAccount account;
        accounts->GetPopAccountL( iEntry->Entry().Id(), account );
        accounts->LoadPopIapSettingsL( account, *apPrefs );
        }
    CleanupStack::PopAndDestroy( accounts ); // accounts

    // Check ap preferences
    TImIAPChoice iapChoice = apPrefs->IAPPreference( KMailSettingsIapPreferenceNumber );
    TInt iapId = iapChoice.iIAP;

    // if iapId is 0, assume default connection
    // else the iapId points to the actual iap
    if( iapId == 0 )
    	{
    	// Default Connection is in use
        TCmDefConnValue defConSetting;
        iCmManager.ReadDefConnL( defConSetting );
        if( defConSetting.iType == ECmDefConnAlwaysAsk ||
        	defConSetting.iType == ECmDefConnAskOnce )
        	{
        	// always ask not supported in always online plugin
        	User::Leave( KErrNotSupported );
        	}
        else
        	{
        	iapId = defConSetting.iId;
        	}
    	}

    RCmConnectionMethod connMethod = iCmManager.ConnectionMethodL( iapId );
    CleanupStack::PopAndDestroy( apPrefs ); // apPrefs
    CleanupClosePushL( connMethod );

    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineEmailAgentBase::GetIapIdL");
    return connMethod;
    }

// ----------------------------------------------------------------------------
// IsIAPInvalidL
// ----------------------------------------------------------------------------
TBool CAlwaysOnlineEmailAgentBase::IsIAPInvalidL( TAlwaysOnlineEmailIAPNotes& aInvalidity )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::IsIAPInvalidL" );
    TBool retVal = EFalse;

    // Get the connection method that's used, leaves if no
    // connection methods found
    RCmConnectionMethod connectionMethod = GetConnectionMethodLC();

    // << VARIATED FEATURE START
    // Check for CSD type
    // Always online can be activated for CSD only if EAOLFAlwaysOnlineCSD
    // flag is on
    TInt bearerType = connectionMethod.GetIntAttributeL( CMManager::ECmBearerType );
    if ( !iFlags->LF( EAOLFAlwaysOnlineCSD ) )
    	{
    	if( bearerType == KCommDbBearerCSD )
    		{
    		// IAP is invalid, feature flag is off but bearer type is
    		// still CSD
    		CleanupStack::PopAndDestroy(); // connMethod // CSI: 12 # RClass not checked 
    		aInvalidity = EEmailAgentCSDIAP;
    		return ETrue;
    		}
    	}
    // CSD VARIATED FEATURE END >>

    // VPN APs should make the same checks, for example CSD
    if( bearerType == KPluginVPNBearerTypeUid )
    	{
    	TInt homeIAP = connectionMethod.GetIntAttributeL( CMManager::EVpnIapId );

    	RCmManager connMan;
    	connMan.OpenLC();
    	RCmConnectionMethod vpnMethod = connMan.ConnectionMethodL( homeIAP );
    	CleanupClosePushL( vpnMethod );
    	TInt vpnBearerType = vpnMethod.GetIntAttributeL( CMManager::ECmBearerType );

        if ( !iFlags->LF( EAOLFAlwaysOnlineCSD ) )
        	{
        	if( vpnBearerType == KCommDbBearerCSD )
        		{
        		// IAP is invalid, feature flag is off but the VPN home
        		// IAP's bearer type is still CSD
        		CleanupStack::PopAndDestroy( 2 ); // vpnMethod, connectionMethod // CSI: 12,47 # RClass not checked 
        		aInvalidity = EEmailAgentCSDIAP;
        		return ETrue;
        		}
        	}
    	CleanupStack::PopAndDestroy(); // vpnMethod // CSI: 12 # RClass not checked 
    	}

    CleanupStack::PopAndDestroy(); // connectionMethod // CSI: 12 # RClass not checked 
    return retVal;
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::IsBearerCSDL()
// ----------------------------------------------------------------------------
TBool CAlwaysOnlineEmailAgentBase::IsBearerCSDL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::IsBearerCSDL" );
    TBool csdIAP = EFalse;

    // Get the connection method that's used, leaves if no
    // connection methods found
    RCmConnectionMethod connectionMethod = GetConnectionMethodLC();
    TInt bearerType = connectionMethod.GetIntAttributeL( CMManager::ECmBearerType );

	if( bearerType == KCommDbBearerCSD )
		{
		// IAP is invalid, feature flag is off but bearer type is
		// still CSD
		csdIAP = ETrue;
		}

    CleanupStack::PopAndDestroy(); // connectionMethod // CSI: 12 # RClass not checked

    return csdIAP;
    }


// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::RemoveMe()
// ----------------------------------------------------------------------------
//
void CAlwaysOnlineEmailAgentBase::RemoveMe()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::RemoveMe" );
    KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::RemoveMe() EmailAgent signalled to remove us!");
    //refresh will see that our setting is "not enabled" and removes us
    iEmailAgent.DeleteMailbox( iEntry->Entry().Id() );
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::CreateSingleOpWatcherLC()
// ----------------------------------------------------------------------------
//
CMsvSingleOpWatcher* CAlwaysOnlineEmailAgentBase::CreateSingleOpWatcherLC()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::CreateSingleOpWatcherLC" );
    CMsvSingleOpWatcher* watcher = CMsvSingleOpWatcher::NewL(*this);
    CleanupStack::PushL( watcher );
    return watcher;
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::HandleCompletingOperation()
// ----------------------------------------------------------------------------
//
void CAlwaysOnlineEmailAgentBase::HandleCompletingOperation(
    const TMsvOp& aOpId,
    CMsvOperation& aOp,
    const TUid& aUid )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::HandleCompletingOperation" );
    TInt error = aOp.iStatus.Int();

    KAOEMAIL_LOGGER_WRITE( "-------------------<COMPLETE: OpInfo>-----------------" );
    KAOEMAIL_LOGGER_WRITE_FORMAT( "Completing operation: aOpId: %d", aOpId );
    KAOEMAIL_LOGGER_WRITE_FORMAT( "Completing operation: aUid: %d", aUid.iUid );
    KAOEMAIL_LOGGER_WRITE_FORMAT( "Completing operation: error: %d", error );
    KAOEMAIL_LOGGER_WRITE( "-------------------<COMPLETE: OpInfo>-----------------" );

    // Check if we are using IMAP4
    if( error == KErrNone )
        {
        if ( aUid == KSenduiMtmImap4Uid )
            {
            TPckgBuf<TImap4CompoundProgress> prgBuf;
            TRAP( error, prgBuf.Copy( aOp.ProgressL() ) );

            // Check that we have the descriptor with us
            if ( error == KErrNone )
                {
                const TImap4CompoundProgress& opPrg = prgBuf();
                const TImap4GenericProgress& genProg =
                opPrg.iGenericProgress;
                error = genProg.iErrorCode;
                }
            }
        // Check if we are using POP3
        else if( aUid == KSenduiMtmPop3Uid )
            {
            TPckgBuf<TPop3Progress> paramPack;
            TRAP( error, paramPack.Copy( aOp.ProgressL() ) );

            // Check that we have the descriptor with us
            if ( error == KErrNone )
                {
                const TPop3Progress& progress = paramPack();
                error = progress.iErrorCode;
                }
            }
        }

    // Handle operation completion
    HandleOpCompleted( aOpId, error );
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::ConnectIfAllowed()
// ----------------------------------------------------------------------------
//
void CAlwaysOnlineEmailAgentBase::ConnectIfAllowedL( const TMsvOp& aOperation )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::ConnectIfAllowedL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineEmailAgentBase::ConnectIfAllowedL");
    if( aOperation == KErrNotFound )
        {
        if ( !IsEmn() )
            {
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::ConnectIfAllowedL(): Not EMN -> Check scheduling");
            // Starts schedule timers and sets state accordingly
            CheckAndHandleSchedulingL();

            // No start or stop waiters, all times
            if( iWaitForStopOpId != KErrNotFound ||
                  ( iWaitForStopOpId == KErrNotFound &&
                  iWaitForStartOpId == KErrNotFound ) )
                {
                ConnectL();
                }
            }
        else
            {
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::ConnectIfAllowedL(): EMN -> ConnectL");
            // When EMN used, we don't have to care about scheduling,
            // just connect
            ConnectL();
            }
        }

    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineEmailAgentBase::ConnectIfAllowedL");
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::CallNewMessagesL()
// ----------------------------------------------------------------------------
//
void CAlwaysOnlineEmailAgentBase::CallNewMessagesL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::CallNewMessagesL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineEmailAgentBase::CallNewMessagesL");

    // Create a dummy array. It is not currently used to anything.
    CDesCArrayFlat* dummyArray = new (ELeave) CDesCArrayFlat( 1 );
    CleanupStack::PushL( dummyArray );

    if ( !iNcnNotification )
        {
        // If notification haven't been created let's do it now!
        TRAPD(error, iNcnNotification =
                MNcnInternalNotification::CreateMNcnInternalNotificationL() );
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::CallNewMessagesL(): NCN notification connection retry");
        if ( error != KErrNone )
            {
            // if construction wasn't fully successful
            delete iNcnNotification;
            iNcnNotification = NULL;
            }
        }

    if ( iNcnNotification )
        {
        TInt result = iNcnNotification->NewMessages(
            EMailMessage, iEntry->Entry().Id(), *dummyArray );
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgentBase::CallNewMessagesL(): NewMessages result = %d", result );
        }
    else
        {
        // write log if we couldn't create notification
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::CallNewMessagesL(): NewMessages not called!, retry faild");
        }

    CleanupStack::PopAndDestroy( dummyArray );
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineEmailAgentBase::CallNewMessagesL");
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::AlwaysOnlineFlagsL()
// ----------------------------------------------------------------------------
//
CMuiuFlags* CAlwaysOnlineEmailAgentBase::AlwaysOnlineFlagsL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::AlwaysOnlineFlagsL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineEmailAgentBase::AlwaysOnlineFlagsL()");
    TMuiuLocalFeatureArray locals;

    TMuiuGlobalFeatureArray globals;

    // EAOLFAlwaysOnlineCSD
    TMuiuLocalFeatureItem temp = {
        KCRUidMuiuVariation, KMuiuEmailConfigFlags,
        KEmailFeatureIdAlwaysOnlineCSD, ETrue };
    locals.Append( temp );

    // EAOLFAlwaysOnlineVPN
    temp.iFlag = KEmailFeatureIdEmailVPNAllowed;
    locals.Append( temp );

    iFlags = CMuiuFlags::NewL( &globals, &locals );
    globals.Reset();
    locals.Reset();

    KAOEMAIL_LOGGER_WRITE( "********************* FEATURES *******************" );
    KAOEMAIL_LOGGER_WRITE_FORMAT( "CSD support: %d", iFlags->LF( EAOLFAlwaysOnlineCSD ) );

    KAOEMAIL_LOGGER_WRITE_FORMAT( "VPN support: %d", iFlags->LF( EAOLFAlwaysOnlineVPN )  );
    KAOEMAIL_LOGGER_WRITE( "********************* FEATURES *******************" );

    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineEmailAgentBase::AlwaysOnlineFlagsL()");
    return iFlags;
    }


// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::AlwaysOnlineDynamicFlags()
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::CloseServices()
// ----------------------------------------------------------------------------
//
void CAlwaysOnlineEmailAgentBase::CloseServices()
	{
	AOLOG_IN( "CAlwaysOnlineEmailAgentBase::CloseServices" );
    iOperations.ResetAndDestroy();
	}

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::IsTemporary()
// ----------------------------------------------------------------------------
//
TBool CAlwaysOnlineEmailAgentBase::IsTemporary() const
	{
	AOLOG_IN( "CAlwaysOnlineEmailAgentBase::IsTemporary" );
	KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::IsTemporary: Default EFalse" );
    return EFalse;
	}



// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::HandleDefaultError()
// ----------------------------------------------------------------------------
//
void CAlwaysOnlineEmailAgentBase::HandleDefaultError()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::HandleDefaultError" );

    if ( iError == KErrAoServerNotFound )
        {
        // there have been one connection attempt, incement iRetryCounter
        ++iRetryCounter;
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::HandleDefaultError. KErrAoServerNotFound catched" );
        // if connection have been successful with current settings keep trying forever
        // else ao is shut down immediatelly

        TBool wasSuccess = LoadSettingL<TInt>(
            TImumInSettings::EKeyAoUpdateSuccessfulWithCurSettings, EFalse );

        if ( wasSuccess && !IsEmn() )
            {
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::HandleDefaultError. KErrAoServerNotFound, but keep trying" );
            iState = EEmailAgentConnectFailed;
            }
        else
            {
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::HandleDefaultError. KErrAoServerNotFound, ao will go off" );
            iState = EEmailAgentFatalError;
            iRetryCounter = 0;
            }

        }
    //try to reconnect only if we were connecting
    else if ( iState == EEmailAgentConnecting ||
              iState == EEmailAgentPlainConnecting ||
              iState == EEmailAgentConnectingToStayOnline )
        {
        iRetryCounter++;
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgentBase::HandleDefaultError. Default error handling, retry counter: %d .",iRetryCounter );

        // if connection have been tried three times whitout success
        if ( (iRetryCounter % KAOMaxRetries)==0 )
            {

            // always online should switch off if no successful update in last 15 attempts,
            // but if ao have before updated succesfully, then keep trying forever

        TBool wasSuccess = LoadSettingL<TInt>(
            TImumInSettings::EKeyAoUpdateSuccessfulWithCurSettings, EFalse );

            if ( iRetryCounter >= KAOMaxRetriesSwitchOff && !wasSuccess )
                {
                //ao will be shutted down immediatelly
                iState = EEmailAgentFatalError;
                iRetryCounter = 0;
                KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::HandleDefaultError. EEailAgentFatalError Ao is goingt to shut down " );

                }
            else
                {
                // keep trying
                iState = EEmailAgentConnectFailed;
                KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::HandleDefaultError. Ao connection failed, but keep trying" );
                }
            }
        else
            {
            // reconnect in few seconds
            iState = EEmailAgentReconnecting;
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::HandleDefaultError. State EEmailAgentReconnecting " );
            }
        }
    else
        {
        //make sure that something is done in case if() didn't catch
        iState = EEmailAgentConnTerminated;
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgentBase::HandleDefaultError. If did not catch, setting state: EEmailAgentConnTerminated ");
        }
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::IsReconnectAfterError()
// ----------------------------------------------------------------------------
//
TBool CAlwaysOnlineEmailAgentBase::IsReconnectAfterError()
{
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::IsReconnectAfterError" );
    // switch - case set this boolean true if we should disconnect
    TBool isReconnect = ETrue;

    //if manually connected or emn is not on, then always reconnect
    if ( IsEmn() && !IsTemporary() )
        {
        switch ( iState )
            {
            case EEmailAgentQueued:
            case EEmailAgentReconnecting:
                // if allready tried KAOMaxRetries time, give up
                isReconnect = iRetryCounter < KAOMaxRetries;
                break;
            case EEmailAgentAutoDisconnecting:
            case EEmailAgentConnectFailed:
            case EEmailAgentUserDisconnecting:
            case EEmailAgentConnTerminated:
            default:
                isReconnect = EFalse;
                break;
            }
        }

    return isReconnect;
}

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgentBase::RetrievalIntervalInMinutes()
// ----------------------------------------------------------------------------
//
TInt CAlwaysOnlineEmailAgentBase::RetrievalIntervalInMinutes( TInt aIntervalIndex )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgentBase::RetrievalIntervalInMinutes" );
    
    switch( aIntervalIndex )
        {
        case TImumDaSettings::EValue5Minutes:
            {
            return KAoInterval5Min;
            }

        case TImumDaSettings::EValue15Minutes:
            {
            return KAoInterval15Min;
            }
        
        case TImumDaSettings::EValue30Minutes:
            {
            return KAoInterval30Min;
            }
        
        case TImumDaSettings::EValue1Hour:
            {
            return KAoInterval1Hour;
            }
        
        case TImumDaSettings::EValue2Hours:
            {
            return KAoInterval2Hours;
            }
        
        case TImumDaSettings::EValue4Hours:
            {
            return KAoInterval4Hours;
            }
        
        case TImumDaSettings::EValue6Hours:
            {
            return KAoInterval6Hours;
            }
        }
    
    // default
    return KAoInterval1Hour;
    }

//EOF
