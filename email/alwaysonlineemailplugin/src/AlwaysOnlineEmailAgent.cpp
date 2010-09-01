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


#include <MuiuMsvUiServiceUtilities.h>
#include <SenduiMtmUids.h>
#include <ImumInternalApi.h>            // CImumInternalApi
#include <ImumInSettingsData.h>         // CImumInSettingsData
#include <AlwaysOnlineManagerCommon.h>

#include "AlwaysOnlineEmailAgent.h"
#include "AlwaysOnlineImapAgent.h"
#include "AlwaysOnlinePopAgent.h"
#include "AlwaysOnlineEmailPluginTimer.h"
#include "AlwaysOnlineEmailEMNResolver.h"
#include "AlwaysOnlineEmailPluginLogging.h"
#include "AlwaysOnlineEmailLoggingTools.h"

const TInt KAOHalfSecond = 500000;

const TInt KEmailAgentArrayGranularity = 2;
// How many times we try to load mail settings before giving up.
const TInt KMailSettingsRetryCount     = 3;

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::CAlwaysOnlineEmailAgent()
// ----------------------------------------------------------------------------
CAlwaysOnlineEmailAgent::CAlwaysOnlineEmailAgent(
    CMsvSession& aMsvSession,
    MAlwaysOnlineStatusQueryInterface& aAlwaysOnlineManager)
    :
    iAlwaysOnlineManager( aAlwaysOnlineManager ),
    iSession( aMsvSession ),
    iEmnResolver( NULL )
    {
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::~CAlwaysOnlineEmailAgent()
// ----------------------------------------------------------------------------
CAlwaysOnlineEmailAgent::~CAlwaysOnlineEmailAgent()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::~CAlwaysOnlineEmailAgent" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineEmailAgent::~CAlwaysOnlineEmailAgent");
    if ( iMailAgentArray )
        {
        iMailAgentArray->ResetAndDestroy();
        }
    delete iMailAgentArray;
    delete iClientMtmRegistry;
    delete iEmnResolver;
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineEmailAgent::~CAlwaysOnlineEmailAgent");
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::NewL()
// ----------------------------------------------------------------------------
CAlwaysOnlineEmailAgent* CAlwaysOnlineEmailAgent::NewL(
    CMsvSession& aMsvSession,
    MAlwaysOnlineStatusQueryInterface& aAlwaysOnlineManager )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::NewL" );
    CAlwaysOnlineEmailAgent* self = new (ELeave)CAlwaysOnlineEmailAgent(
        aMsvSession, aAlwaysOnlineManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::ConstructL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgent::ConstructL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::ConstructL" );
    iClientMtmRegistry = CClientMtmRegistry::NewL( iSession );
    iMailAgentArray = new (ELeave) CAOEmailAgentArray(
        KEmailAgentArrayGranularity );
    iEmnResolver = CAlwaysOnlineEmailEMNResolver::NewL();
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::StartL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgent::StartL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::StartL" );
    RefreshMailAgentArrayL();
    ActivateOnlineMailboxesL();
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::SuspendNWOperationsL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgent::SuspendNWOperationsL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::SuspendNWOperationsL" );
    const TInt count = iMailAgentArray->Count();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        iMailAgentArray->At( loop )->Suspend();
        }
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::ResumeNWOperationsL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgent::ResumeNWOperationsL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::ResumeNWOperationsL" );
    const TInt count = iMailAgentArray->Count();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        iMailAgentArray->At( loop )->ResumeL();
        }
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::RoamingEventL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgent::RoamingEventL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::RoamingEventL" );
    const TInt count = iMailAgentArray->Count();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        iMailAgentArray->At( loop )->HandleRoamingEventL();
        }
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::HomeNetworkEventL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgent::HomeNetworkEventL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::HomeNetworkEventL" );
    const TInt count = iMailAgentArray->Count();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        iMailAgentArray->At( loop )->HandleHomeNetworkEventL();
        }
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::HandleOutOfDiskL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgent::HandleOutOfDiskL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::HandleOutOfDiskL" );
    const TInt count = iMailAgentArray->Count();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        iMailAgentArray->At( loop )->HandleOutOfDiskEventL();
        }
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::HandleEMNMessageL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgent::HandleEMNMessageL(
    const TDesC8& aParameters )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::HandleEMNMessageL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineEmailAgent::HandleEMNL");
    TEMNElement elements;

    if ( iEmnResolver->ParameterDispatchTEMNElement(
        aParameters, elements ) == KErrNone )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgent::HandleEMNL(): Find EMN mailbox" );

        CAlwaysOnlineEmailAgentBase* mailbox =
            iEmnResolver->FindEMNMailbox( elements.mailbox, *iMailAgentArray );

        if ( mailbox )
            {
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgent::HandleEMNL(): EMN mailbox found" );
            mailbox->ConnectL();
            }
        else
            {
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgent::HandleEMNL(): EMN mailbox not found" );
            }
        }

    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineEmailAgent::HandleEMNL");
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::GetMailboxesLC()
// ----------------------------------------------------------------------------
CMsvEntrySelection* CAlwaysOnlineEmailAgent::GetMailboxesLC()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::GetMailboxesLC" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineEmailAgent::GetMailboxesLC");
    CMsvEntrySelection* mailboxSelection =
        MsvUiServiceUtilities::GetListOfAccountsWithMTML(
            iSession, KSenduiMtmImap4Uid );
	CleanupStack::PushL( mailboxSelection );

    CMsvEntrySelection* pop3Selection =
        MsvUiServiceUtilities::GetListOfAccountsWithMTML(
            iSession, KSenduiMtmPop3Uid );
	CleanupStack::PushL( pop3Selection );

    const TInt count = pop3Selection->Count();
    TInt loop = 0;
    for ( loop = 0; loop < count; loop++ )
        {
        mailboxSelection->AppendL( (*pop3Selection)[loop] );
        }
    CleanupStack::PopAndDestroy( pop3Selection );
    pop3Selection = NULL;

    // remove mailboxes from mailboxSelection which are already in iMailAgentArray
    for ( loop = mailboxSelection->Count() - 1; loop >= 0; loop-- )
        {
        if ( FindMailbox( (*mailboxSelection)[loop] ) )
            {
            // already in iMailAgentArray, delete it
            mailboxSelection->Delete( loop );
            }
        }
    // mailboxSelection in CleanupStack
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineEmailAgent::GetMailboxesLC");
    return mailboxSelection;
	}

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::RefreshMailAgentArrayLC
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgent::RefreshMailAgentArrayL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::RefreshMailAgentArrayL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineEmailAgent::RefreshMailAgentArrayL");
    // delete first mailboxes from the iMailAgentArray, which does not exists anymore.
    TInt cnt = iMailAgentArray->Count();
    TBool isHealthy = EFalse;
    TBool setActive = EFalse;
    TMsvId mailboxId;
    TMsvEntry tEntry;
    TMsvId service;

    CImumInternalApi* emailApi = CreateEmailApiLC( &iSession );
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgent::RefreshMailAgentArrayL() Mail Agents in array: %d", cnt);

    TInt i;
    for ( i = cnt; i > 0; i-- )
        {
        mailboxId = ( *iMailAgentArray )[i-1]->MailboxId();
        isHealthy = emailApi->HealthServicesL().IsMailboxHealthy( mailboxId );
        if ( isHealthy )
            {
            setActive = IsMailAgentActiveL( mailboxId );
            KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgent::RefreshMailAgentArrayL() setActive = %d", setActive);
            }

        //remove if mailbox has been deleted, AO setting disabled,
        //EMN turned off or mailbox is not healthy.
        if ( !setActive || !isHealthy ||
             iSession.GetEntry( mailboxId, service, tEntry ) != KErrNone )
            {
            KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgent::RefreshMailAgentArrayL() Deleting agent for mailbox: %d", mailboxId);
            delete iMailAgentArray->At( i-1 );
            iMailAgentArray->Delete( i-1 );
            }
        }

    // then add new mailboxes...
    CMsvEntrySelection* mailboxes = GetMailboxesLC();
    const TInt count = mailboxes->Count();
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgent::RefreshMailAgentArrayL() New mailboxes count: %d", count);
    for ( i = 0; i < count; i++ )
        {
        mailboxId = mailboxes->At( i );
        isHealthy = emailApi->HealthServicesL().IsMailboxHealthy( mailboxId );

        if ( isHealthy && IsMailAgentActiveL( mailboxId ) )
            {
            KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgent::RefreshMailAgentArrayL() Adding agent for mailbox: %d", (*mailboxes)[i]);
            AddMailboxL( ( *mailboxes )[i] );
            }
        }
    CleanupStack::PopAndDestroy( 2, emailApi ); // CSI: 12,47 # nothing wrong here
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineEmailAgent::RefreshMailAgentArrayL");
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::AddMailboxL
// ----------------------------------------------------------------------------
CAlwaysOnlineEmailAgentBase* CAlwaysOnlineEmailAgent::AddMailboxL(
    TMsvId aMailboxId )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::AddMailboxL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineEmailAgent::AddMailboxL");
    CAlwaysOnlineEmailAgentBase* mailAgent = NULL;
    if ( !FindMailbox( aMailboxId ) )
        {
        TMsvEntry tEntry;
        TMsvId service;
        if ( iSession.GetEntry( aMailboxId, service, tEntry ) == KErrNone &&
             tEntry.iType == KUidMsvServiceEntry )
            {
            if ( tEntry.iMtm == KSenduiMtmImap4Uid )
                {
                mailAgent = CAlwaysOnlineImap4Agent::NewL(
                    iSession, *iClientMtmRegistry, aMailboxId,
                    iAlwaysOnlineManager, *this );
                CleanupStack::PushL( mailAgent );
                iMailAgentArray->AppendL( mailAgent ); // takes ownership
                CleanupStack::Pop( mailAgent );
                }
            else if ( tEntry.iMtm == KSenduiMtmPop3Uid )
                {
                mailAgent = CAlwaysOnlinePop3Agent::NewL(
                    iSession, *iClientMtmRegistry, aMailboxId,
                    iAlwaysOnlineManager, *this );
                CleanupStack::PushL( mailAgent );
                iMailAgentArray->AppendL( mailAgent ); // takes ownership
                CleanupStack::Pop( mailAgent );
                }
            }
        }
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineEmailAgent::AddMailboxL");
    return mailAgent;
    }


// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::ActivateOnlineMailboxesL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgent::ActivateOnlineMailboxesL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::ActivateOnlineMailboxesL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineEmailAgent::ActivateOnlineMailboxesL");
    const TInt count = iMailAgentArray->Count();

    for ( TInt loop = 0; loop < count; loop++ )
        {
        (*iMailAgentArray)[loop]->StartL();
        }//for
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineEmailAgent::ActivateOnlineMailboxesL");
    }


// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::FindMailbox
// ----------------------------------------------------------------------------
CAlwaysOnlineEmailAgentBase* CAlwaysOnlineEmailAgent::FindMailbox(
    TMsvId aMailboxId )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::FindMailbox" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineEmailAgent::FindMailbox");
    // Get number of agents
    const TInt count = iMailAgentArray->Count();
    CAlwaysOnlineEmailAgentBase* mailAgent = NULL;

    // Search through the agents, until correct is found
    for ( TInt loop = 0; loop < count && !mailAgent; loop++ )
        {
        if ( aMailboxId == (*iMailAgentArray)[loop]->MailboxId() )
            {
            mailAgent = (*iMailAgentArray)[loop];
            }

        }

    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineEmailAgent::FindMailbox");
    // Return the pointer
    return mailAgent;
    }

// ----------------------------------------------------------------------------
// DeleteMailbox
// ----------------------------------------------------------------------------
TBool CAlwaysOnlineEmailAgent::DeleteMailbox( TMsvId aMailboxId )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::DeleteMailbox" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineEmailAgent::DeleteMailbox");
    const TInt count = iMailAgentArray->Count();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        if ( aMailboxId == (*iMailAgentArray)[loop]->MailboxId() )
            {
            delete iMailAgentArray->At( loop );
            iMailAgentArray->Delete( loop );
            return ETrue;
            }

        }
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineEmailAgent::DeleteMailbox");
    return EFalse;
    }

// ----------------------------------------------------------------------------
//  CAlwaysOnlineEmailAgent::MailboxSetActivityL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgent::MailboxSetActivityL(
    const TMsvId aMailboxId,
    const TBool  aState )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::MailboxSetActivityL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineEmailAgent::MailboxSetActiviytL");
    // Mailbox settings changed, update the array
    MailboxSettingsChangedL( aMailboxId );

    // Find the agent from array
    CAlwaysOnlineEmailAgentBase* agent = FindMailbox( aMailboxId );

    // Check that the agent is valid
    if ( agent )
        {
        if ( !aState )
            {
            // Suspend the agent
            agent->Suspend();
            }
        else
            {
            agent->ResumeL( ETrue );
            }
        }
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineEmailAgent::MailboxSetActiviytL");
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::MailboxSettingsChangedL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgent::MailboxSettingsChangedL( TMsvId aMailboxId )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::MailboxSettingsChangedL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineEmailAgent::MailboxSettingsChangedL");
    RefreshMailAgentArrayL();
    CAlwaysOnlineEmailAgentBase* mailAgent = FindMailbox( aMailboxId );
    if ( mailAgent )
        {
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgent::MailboxSettingsChangedL() Calling StartL for: %d", aMailboxId);
        mailAgent->StartL();
        }
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineEmailAgent::MailboxSettingsChangedL");
    }


// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::RemoveMailAgentL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgent::RemoveMailAgentL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::RemoveMailAgentL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineEmailAgent::RemoveMailAgentL");
    RefreshMailAgentArrayL();
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineEmailAgent::RemoveMailAgentL");
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::SwitchOffAllAgentsL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgent::SwitchOffAllAgentsL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::SwitchOffAllAgentsL" );
    KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgent::SwitchOffAllAgentsL() Deleting all agents from agent array!");
    while ( iMailAgentArray->Count() )
        {
        delete iMailAgentArray->At( 0 );
        iMailAgentArray->Delete( 0 );
        }

    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailAgent::IsMailAgentActiveL
// ----------------------------------------------------------------------------
TBool CAlwaysOnlineEmailAgent::IsMailAgentActiveL( TMsvId aMailboxId )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::IsMailAgentActiveL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineEmailAgent::IsMailAgentActiveL");
    TMsvEntry entry;
    TMsvId service;

    TInt err = iSession.GetEntry( aMailboxId, service, entry );
    if ( err != KErrNone )
        {
        return EFalse;
        }

    TInt cnt = 0;
    CImumInternalApi* emailApi = CreateEmailApiLC( &iSession );
    CImumInSettingsData* settings = NULL;

    err = KErrNotFound;
    while ( err && cnt < KMailSettingsRetryCount )
        {
        // No need to push, since no leaving method called after
        TRAP( err, settings =
            emailApi->MailboxServicesL().LoadMailboxSettingsL( aMailboxId ) );
        if ( err || !settings )
            {
            // wait for half a second and try again
            RTimer timer;
            TRequestStatus timerStatus;
            timer.CreateLocal();
            timer.After( timerStatus, KAOHalfSecond );
            User::WaitForRequest(timerStatus); // CSI: 94 # not UI code
            cnt++;
            }
        }


    // Did we got settings or did we fail 3 times?
    if ( err && cnt == KMailSettingsRetryCount )
        {
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgent::IsMailAgentActiveL: failed to load settings, leaving. Err = %d", err );
        User::Leave( err );
        }

    TBool result = EFalse;

    TInt emnState = 0;
    settings->GetAttr( TImumDaSettings::EKeyAutoNotifications, emnState );
    TInt aoState = 0;
    settings->GetAttr( TImumDaSettings::EKeyAutoRetrieval, aoState );

    if ( emnState != TImumDaSettings::EValueNotificationsOff )
        {
        result = ETrue;
        }
    else if ( aoState != TImumDaSettings::EValueAutoOff )
        {
        result = ETrue;
        }
    else
        {
        // There might be temporary agent running.
        CAlwaysOnlineEmailAgentBase* mailAgent = FindMailbox( aMailboxId );
        result = mailAgent != NULL; // CSI: 64 # conversion to TBool
        }

    CleanupStack::PopAndDestroy( emailApi );
    emailApi = NULL;
    delete settings;
    settings = NULL;

    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineEmailAgent::IsMailAgentActiveL");

    return result;
    }

// ----------------------------------------------------------------------------
//  CAlwaysOnlineEmailAgent::ParameterDispatchMsvId
// ----------------------------------------------------------------------------
TInt CAlwaysOnlineEmailAgent::ParameterDispatchMsvId(
    const TDesC8& aParameters,
    TMsvId& aMailboxId ) const
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::ParameterDispatchMsvId" );
    KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgent::ParameterDispatchMsvId Dispatch the message");

    // Presume that wrong type of parameter is given
    TInt err = KErrNotSupported;

    // Unpack parameters to see to which mailbox command is for
    TPckgBuf<TMsvId> paramPack;

    // Make sure that the parameter length matches Id length
    if ( aParameters.Length() == sizeof( TMsvId ) )
        {
        paramPack.Copy( aParameters );

        // Get the mailbox id from the packet
        aMailboxId = paramPack();
        err = KErrNone;
        }

    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailAgent::ParameterDispatchMsvId Mailbox receiving message: 0x%x", aMailboxId );
    return err;
    }

// ----------------------------------------------------------------------------
//  CAlwaysOnlineEmailAgent::HandleAOStateL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgent::HandleAOStateL(
    const TInt    aCommand,
    const TDesC8& aParameters )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::HandleAOStateL" );
    // Get the msvid from parameter
    TMsvId mailboxId;

    // Make sure parameter has correct type of information in
    if ( ParameterDispatchMsvId( aParameters, mailboxId ) != KErrNone )
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgent::HandleAOState -> Invalid parameter!");
        return;
        }

    // Handle the command
    switch ( aCommand )
        {
        case EAOManagerMailboxAgentSuspend:
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgent::HandleAOState -> Command: Suspend agent");
            MailboxSetActivityL( mailboxId, EFalse );
            break;

        case EAOManagerMailboxAgentResume:
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgent::HandleAOState -> Command: Resume agent");
            MailboxSetActivityL( mailboxId, ETrue );
            break;

        case EAOManagerMailboxAgentRemove:
            KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailAgent::HandleAOState -> Command: Remove agent");
            DeleteMailbox( mailboxId );
            break;

        case EAOManagerMailboxAgentUpdateMailWhileConnected:
            MailboxUpdateMailWhileConnectedL( mailboxId ); // fetch new mail but do not reconnect if connection is dropped!
            break;

        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
//  CAlwaysOnlineEmailAgent::MailboxSetActivityL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailAgent::MailboxUpdateMailWhileConnectedL(
    const TMsvId aMailboxId )
    {
    AOLOG_IN( "CAlwaysOnlineEmailAgent::MailboxUpdateMailWhileConnectedL" );
    TMsvEntry tEntry;
    TMsvId service;

    // this is valid only for imap4 mailboxes which is already connected
    if ( iSession.GetEntry( aMailboxId, service, tEntry ) == KErrNone
        && tEntry.iMtm == KSenduiMtmImap4Uid
        && tEntry.iType == KUidMsvServiceEntry
        && tEntry.Connected() )

        {
        // Find the agent from array
        CAlwaysOnlineEmailAgentBase* agent = FindMailbox( aMailboxId );

        // Check that the agent is valid
        if ( agent )
            {
            // do nothing, this is handled by default
            CAlwaysOnlineImap4Agent* mailAgent =
                static_cast<CAlwaysOnlineImap4Agent*>( agent );
            mailAgent->SetUpdateMailWhileConnectedL( ETrue );
            }
        else
            {
            // create temporary agent...
            CAlwaysOnlineImap4Agent* mailAgent =
                CAlwaysOnlineImap4Agent::NewL(
                    iSession, *iClientMtmRegistry, aMailboxId,
                    iAlwaysOnlineManager, *this );
            CleanupStack::PushL( mailAgent );
            iMailAgentArray->AppendL( mailAgent ); // takes ownership
            CleanupStack::Pop( mailAgent );
            mailAgent->SetUpdateMailWhileConnectedL( EFalse );
            }
        }
    }



//EOF
