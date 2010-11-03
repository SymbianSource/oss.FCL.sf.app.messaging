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
*     Main plugin class
*
*/

#include <msvids.h>
#include "AlwaysOnlineEmailPlugin.h"
#include <AlwaysOnlineManagerCommon.h>
#include <msvuids.h>
#include <SenduiMtmUids.h>
#include <msvapi.h>
#include <AlwaysOnlineManagerClient.h>

#include "AlwaysOnlineEmailAgent.h"
#include "AlwaysOnlineEmailPluginLogging.h"
#include "AlwaysOnlineEmailLoggingTools.h"

const TInt KAOEmailLaunchStartTime = 3000000;
const TInt KAOEmailLaunchRetryTime = 3000000;
typedef TTimeIntervalMicroSeconds32 TAOEmailMs;

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
CEComEmailPlugin::CEComEmailPlugin( ) : CAlwaysOnlineEComInterface()
    {
    AOLOG_IN( "CEComEmailPlugin::CEComEmailPlugin" );
    iPluginStarted = EFalse;
    }

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
CEComEmailPlugin* CEComEmailPlugin::NewL( )
    {
    AOLOG_IN( "CEComEmailPlugin::NewL" );
    CEComEmailPlugin* self = new (ELeave) CEComEmailPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
void CEComEmailPlugin::ConstructL()
    {
    AOLOG_IN( "CEComEmailPlugin::ConstructL" );
    iSession = CMsvSession::OpenSyncL( *this );
    }

// ----------------------------------------------------------------------------
//  ~CEComEmailPlugin()
// ----------------------------------------------------------------------------
CEComEmailPlugin::~CEComEmailPlugin()
    {
    AOLOG_IN( "CEComEmailPlugin::~CEComEmailPlugin" );
    KAOEMAIL_LOGGER_WRITE("CEComEmailPlugin::~CEComEmailPlugin()");
    delete iSessionStarter;
    delete iEmailAgent;
    delete iSession;
    }

// ----------------------------------------------------------------------------
// HandleSessionEventL() from MMsvSessionObserver
// ----------------------------------------------------------------------------
void CEComEmailPlugin::HandleSessionEventL(
    TMsvSessionEvent    aEvent,
    TAny*               /* aArg1 */,
    TAny*               /* aArg2 */,
    TAny*               /* aArg3 */)
    {
    AOLOG_IN( "CEComEmailPlugin::HandleSessionEventL" );
    AOLOG_WRMV( "aEvent: ", EAoMsvSessionEvent, aEvent, EAoLogSt3 );
    KAOEMAIL_LOGGER_WRITE_FORMAT("CEComEmailPlugin::HandleSessionEventL() aEvent %d", aEvent);
    //we can receive session events even before plugin is started..we MUST NOT try to handle those.
    //they would just result in access violation because iEmailAgent is NULL at that time.
    if ( iPluginStarted )
        {
        switch (aEvent)
            {
            case EMsvMediaUnavailable:
                iEmailAgent->SwitchOffAllAgentsL();
                break;

            case EMsvMediaChanged:
            case EMsvMediaAvailable:
                iEmailAgent->RefreshMailAgentArrayL();
                iEmailAgent->ActivateOnlineMailboxesL();
                break;

            case EMsvCloseSession:
            case EMsvServerTerminated:
                HandleSessionTerminatedL();
                break;

                //let these fall through.
            case EMsvEntriesCreated:
            case EMsvEntriesDeleted:
            case EMsvEntriesChanged:
            case EMsvServerReady:
            case EMsvMediaIncorrect:
            case EMsvIndexLoaded:
            case EMsvIndexFailedToLoad:
            case EMsvCorruptedIndexRebuilding:
            case EMsvMtmGroupInstalled:
            case EMsvMtmGroupDeInstalled:
            case EMsvGeneralError:
            default:
                //no handling for these now
                break;
            }//switch
        }//if
    }

// ----------------------------------------------------------------------------
//  HandleServerCommandL
// ----------------------------------------------------------------------------
TAny* CEComEmailPlugin::HandleServerCommandL(
    TInt aCommand,
    TDesC8* aParameters )
    {
    AOLOG_IN( "CEComEmailPlugin::HandleServerCommandL" );
    AOLOG_WRMV( "aCommand: ", EAoManagerServerCommands, aCommand, EAoLogSt3 );
    KAOEMAIL_LOGGER_WRITE_FORMAT("CEComEmailPlugin::HandleServerCommandL() aCommand %d", aCommand);

    TInt result = KErrNone;

    // Make system check
    VerifyPluginHealthL();

    switch ( aCommand )
        {
        case EAOManagerPluginStart:
            ActivateOnlineMailboxesL();
        
            iPluginStarted = ETrue;
            break;
            
        case EAOManagerPluginStop:
            // Return value does not matter, plugin will be stopped anyway.
            result = KErrNotSupported;
            break;

        case EAOManagerNWOpsNotAllowed:
            iEmailAgent->SuspendNWOperationsL();
            break;
            
        case EAOManagerNWOpsAllowed:
            iEmailAgent->ResumeNWOperationsL();
            break;
            
        case EAOManagerStartedRoaming:
            iEmailAgent->RoamingEventL();
            break;
            
        case EAOManagerStoppedRoaming:
            iEmailAgent->HomeNetworkEventL();
            break;
            
        case EAOManagerDiskSpaceAboveCritical:
            // Not handled at the moment because 
            // switching off at below critical
            result = KErrNotSupported;
            break;
            
        case EAOManagerDiskSpaceBelowCritical:
            iEmailAgent->HandleOutOfDiskL();
            break;
            
        case EAOManagerSuicideQuery:
            // Always tell to server, that we don't 
            // want to make a suicide.
            result = EFalse;
            break;

        // Mailbox handling
        case EAOManagerMailboxAgentSuspend:
        case EAOManagerMailboxAgentResume:
        case EAOManagerMailboxAgentRemove:
        case EAOManagerEMNReceived:
        case EAOManagerMailboxAgentUpdateMailWhileConnected:
            HandleCmdMailboxL( aCommand, aParameters );
            break;
        
        // EAOManagerAOSchdulerError not handled because 
        // there's no badly behaving active objects
        case EAOManagerAOSchdulerError:
            KAOEMAIL_LOGGER_WRITE("CEComEmailPlugin::HandleServerCommandL() EAOManagerAOSchdulerError recieved");
        default:
            result = KErrNotSupported;
            break;
        }

    KAOEMAIL_LOGGER_WRITE_FORMAT("CEComEmailPlugin::HandleServerCommandL() result %d", result);

    return reinterpret_cast<TAny*>( result );
    }

// ----------------------------------------------------------------------------
//  CEComEmailPlugin::ActivateOnlineMailboxesL
// ----------------------------------------------------------------------------
void CEComEmailPlugin::ActivateOnlineMailboxesL()
    {
    AOLOG_IN( "CEComEmailPlugin::ActivateOnlineMailboxesL" );
    // Start the agent, if everything is ok
    VerifyPluginHealthL();
    iEmailAgent->StartL();
    }

// ----------------------------------------------------------------------------
//  CEComEmailPlugin::HandleCmdMailboxL
// ----------------------------------------------------------------------------
void CEComEmailPlugin::HandleCmdMailboxL(
    const TInt    aCommand,
    const TDesC8* aParameters )
    {
    AOLOG_IN( "CEComEmailPlugin::HandleCmdMailboxL" );
    // Check for NULL pointer and return
    if ( !aParameters )
        {
        // Just don't care about the false alarm,
        // ignore them to keep things running
        // smoothly.
        return;
        }

    switch( aCommand )
        {
        // Suspend Agent
        case EAOManagerMailboxAgentSuspend:
        case EAOManagerMailboxAgentResume:
        case EAOManagerMailboxAgentRemove:
        case EAOManagerMailboxAgentUpdateMailWhileConnected:
            iEmailAgent->HandleAOStateL( aCommand, *aParameters );
            break;

        // Mail notification
        case EAOManagerEMNReceived:
            iEmailAgent->HandleEMNMessageL( *aParameters );
            break;

        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
//  CEComEmailPlugin::LaunchMsvSession
// ----------------------------------------------------------------------------
TInt CEComEmailPlugin::LaunchMsvSession( TAny* aSelf )
    {
    AOLOG_IN( "CEComEmailPlugin::LaunchMsvSession" );
    return reinterpret_cast< CEComEmailPlugin* >( aSelf )->LaunchSession();
    }

// ----------------------------------------------------------------------------
//  CEComEmailPlugin::LaunchSession
// ----------------------------------------------------------------------------
TInt CEComEmailPlugin::LaunchSession()
    {
    AOLOG_IN( "CEComEmailPlugin::LaunchSession" );
    KAOEMAIL_LOGGER_FN1("CEComEmailPlugin::LaunchSession");
    const TInt KAOEmailRetryCounter = 20;
    TInt error = KErrNone;
    iLaunchCounter++;

    // Try activating mailboxex
    if ( iLaunchCounter < KAOEmailRetryCounter )
        {
        TRAP( error, ActivateOnlineMailboxesL() );
        }
    // Else just give up for trying and hang around for miracle to happen...

    // Remove the timer
    if ( error == KErrNone )
        {
        delete iSessionStarter;
        iSessionStarter = NULL;
        iPluginStarted = ETrue;
        }
    KAOEMAIL_LOGGER_WRITE_FORMAT( "LaunchSession, error = %d", error);
    KAOEMAIL_LOGGER_FN2("CEComEmailPlugin::LaunchSession");
    return error;
    }

// ----------------------------------------------------------------------------
//  CEComEmailPlugin::HandleSessionTerminatedL
// ----------------------------------------------------------------------------
void CEComEmailPlugin::HandleSessionTerminatedL()
    {
    AOLOG_IN( "CEComEmailPlugin::HandleSessionTerminatedL" );
    KAOEMAIL_LOGGER_FN1("CEComEmailPlugin::HandleSessionTerminatedL");

    // Messaging Server has been terminated, recreate connection to it
    delete iEmailAgent;
    iEmailAgent = NULL;
    iPluginStarted = EFalse;
    delete iSession;
    iSession = NULL;

    // Check if timer already exist for recreating the
    if ( !iSessionStarter )
        {
        iSessionStarter = CPeriodic::NewL( CActive::EPriorityIdle );
        }

    // If the timer isn't active, start it
    if ( !( iSessionStarter->IsActive() ) )
        {
        // Determine the intervals
        const TAOEmailMs startTimeInterval = KAOEmailLaunchStartTime;
        const TAOEmailMs retryTimeInterval = KAOEmailLaunchRetryTime;
        iLaunchCounter = 0;

        // Start the timer
        iSessionStarter->Start(
            startTimeInterval,
            retryTimeInterval,
            TCallBack( LaunchMsvSession, this ) );
        }
    KAOEMAIL_LOGGER_FN2("CEComEmailPlugin::HandleSessionTerminatedL");
    }

// ----------------------------------------------------------------------------
// CEComEmailPlugin::VerifyPluginHealthL()
// ----------------------------------------------------------------------------
void CEComEmailPlugin::VerifyPluginHealthL()
    {
    AOLOG_IN( "CEComEmailPlugin::VerifyPluginHealthL" );

    // Create new session, if it doesn't exist yet
    if ( !iSession )
        {
        KAOEMAIL_LOGGER_WRITE("iSession is NULL");
        iSession = CMsvSession::OpenSyncL( *this );
        KAOEMAIL_LOGGER_WRITE_FORMAT("Created iSession 0x%x", iSession);
        }

    // Create new agent, if there isn't such yet
    if ( !iEmailAgent && iSession )
        {
        KAOEMAIL_LOGGER_WRITE("iEmailAgent is NULL");
        iEmailAgent = CAlwaysOnlineEmailAgent::NewL(
            *iSession, *iStatusQueryObject );
        }
    }


//EOF
