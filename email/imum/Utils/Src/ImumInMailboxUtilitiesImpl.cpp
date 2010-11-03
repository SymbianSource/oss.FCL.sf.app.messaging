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
* Description:  ImumInMailboxUtilitiesImpl.cpp
*
*/


#include <e32base.h>
#include <SendUiConsts.h>
#include <msvapi.h>                         // CMsvSession
#include <msvstd.hrh>                       // Constants
#include <mtudreg.h>                        // CMtmUiDataRegistry
#include <cemailaccounts.h>                 // CEmailAccounts
#include <iapprefs.h>                       // CImIAPPreferences

#include "ImumInMailboxUtilitiesImpl.h"
#include "ImumInMailboxServicesImpl.h"
#include "ImumInternalApiImpl.h"
#include "ImumMboxManager.h"                // CImumMboxManager
#include "ImumMboxScheduler.h"              // CImumMboxSchdeuler
#include "ImumUtilsLogging.h"
#include "muiuemailtools.h"
#include "ComDbUtl.h"

// CONSTANTS
_LIT( KImumInImapInboxName,"INBOX");

// ================ CLASS CONSTRUCTION ================

// ---------------------------------------------------------------------------
// CImumInMailboxUtilitiesImpl::CImumInMailboxUtilitiesImpl()
// ---------------------------------------------------------------------------
//
CImumInMailboxUtilitiesImpl::CImumInMailboxUtilitiesImpl(
    CImumInternalApiImpl& aMailboxApi )
    :
    iMailboxApi( aMailboxApi )
    {
    IMUM_CONTEXT( CImumInMailboxUtilitiesImpl::CImumInMailboxUtilitiesImpl, 0, KLogInApi );
    IMUM_IN();
    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumInMailboxUtilitiesImpl::~CImumInMailboxUtilitiesImpl()
// ---------------------------------------------------------------------------
//
CImumInMailboxUtilitiesImpl::~CImumInMailboxUtilitiesImpl()
    {
    IMUM_CONTEXT( CImumInMailboxUtilitiesImpl::~CImumInMailboxUtilitiesImpl, 0, KLogInApi );
    IMUM_IN();

    delete iMtmUiDataRegistry;
    iMtmUiDataRegistry = NULL;

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumInMailboxUtilitiesImpl::NewL()
// ---------------------------------------------------------------------------
//
CImumInMailboxUtilitiesImpl* CImumInMailboxUtilitiesImpl::NewL(
    CImumInternalApiImpl& aMailboxApi )
    {
    IMUM_STATIC_CONTEXT( CImumInMailboxUtilitiesImpl::NewL, 0, utils, KLogInApi );
    IMUM_IN();

    CImumInMailboxUtilitiesImpl* self = NewLC( aMailboxApi );
    CleanupStack::Pop( self );

    IMUM_OUT();

    return self;
    }

// ---------------------------------------------------------------------------
// CImumInMailboxUtilitiesImpl::NewLC()
// ---------------------------------------------------------------------------
//
CImumInMailboxUtilitiesImpl* CImumInMailboxUtilitiesImpl::NewLC(
    CImumInternalApiImpl& aMailboxApi )
    {
    IMUM_STATIC_CONTEXT( CImumInMailboxUtilitiesImpl::NewLC, 0, utils, KLogInApi );
    IMUM_IN();

    CImumInMailboxUtilitiesImpl* self =
        new ( ELeave ) CImumInMailboxUtilitiesImpl( aMailboxApi );
    CleanupStack::PushL( self );

    self->ConstructL();

    IMUM_OUT();

    return self;
    }

// ---------------------------------------------------------------------------
// CImumInMailboxUtilitiesImpl::ConstructL()
// ---------------------------------------------------------------------------
//
void CImumInMailboxUtilitiesImpl::ConstructL()
    {
    IMUM_CONTEXT( CImumInMailboxUtilitiesImpl::ConstructL, 0, KLogInApi );
    IMUM_IN();

    iMtmUiDataRegistry = CMtmUiDataRegistry::NewL( iMailboxApi.MsvSession() );

    IMUM_OUT();
    }

// ================ INTERFACE IMPLEMENTATION ================

// ---------------------------------------------------------------------------
// From class MImumInMailboxUtilities.
// CImumInMailboxUtilitiesImpl::DefaultMailbox()
// ---------------------------------------------------------------------------
//
TMsvId CImumInMailboxUtilitiesImpl::DefaultMailboxId(
    const TBool aForceGet ) const
    {
    IMUM_CONTEXT( CImumInMailboxUtilitiesImpl::DefaultMailboxId, 0, KLogInApi );
    IMUM_IN();

    TMsvId defaultId = KMsvUnknownServiceIndexEntryId;

    //Should not leave, defaultId is just unknown
    TRAP_IGNORE( defaultId = DefaultMailboxIdL( aForceGet ) );

    IMUM1(0,"Default mailbox: 0x%x", defaultId );
    IMUM_OUT();

    return defaultId;
    }

// ---------------------------------------------------------------------------
// From class MImumInMailboxUtilities.
// CImumInMailboxUtilitiesImpl::IsMailMtm()
// ---------------------------------------------------------------------------
//
TBool CImumInMailboxUtilitiesImpl::IsMailMtm(
    const TUid& aMtm,
    const TBool& aAllowExtended ) const
    {
    IMUM_CONTEXT( CImumInMailboxUtilitiesImpl::IsMailMtm, 0, KLogInApi );
    IMUM_IN();

    // Basic mail mtm's
    TBool basicMtms =
        ( aMtm == KSenduiMtmSmtpUid ) ||
        ( aMtm == KSenduiMtmPop3Uid ) ||
        ( aMtm == KSenduiMtmImap4Uid );

    // Extended mail mtm's
    // Check if mailbox is 3rd party mailbox.
    TBool extendedMtms = EFalse;
    
    if( aAllowExtended && !basicMtms )
    	{
        // We don´t know 3rd party mailbox uid values so technology type
        // is compared instead.
    	if( iMtmUiDataRegistry->IsPresent( aMtm ) )
    	    {
            const TUid& technologyType =
                iMtmUiDataRegistry->TechnologyTypeUid( aMtm );
            extendedMtms = ( KSenduiTechnologyMailUid == technologyType );
    	    }
    	}

    IMUM_OUT();

    // Returns ETrue, if the id is any of the following
    return basicMtms || ( aAllowExtended && extendedMtms );
    }

// ---------------------------------------------------------------------------
// From class MImumInMailboxUtilities.
// CImumInMailboxUtilitiesImpl::IsMailbox()
// ---------------------------------------------------------------------------
//
TBool CImumInMailboxUtilitiesImpl::IsMailbox( const TMsvId aMailboxId ) const
    {
    IMUM_CONTEXT( CImumInMailboxUtilitiesImpl::IsMailbox, 0, KLogInApi );
    IMUM_IN();

    // First get the entry and make sure it is a service
    TBool isMailbox = EFalse;
    TMsvId serviceId;
    TMsvEntry entry;
    if ( !iMailboxApi.MsvSession().GetEntry( aMailboxId, serviceId, entry ) &&
          entry.iType.iUid == KUidMsvServiceEntryValue )
        {
        // Get the technology type of the entry
        const TUid& technologyType =
            iMtmUiDataRegistry->TechnologyTypeUid( entry.iMtm );

        // Match with mail technology type
        isMailbox = ( KSenduiTechnologyMailUid == technologyType );
        }

    IMUM_OUT();

    return isMailbox;
    }

// ---------------------------------------------------------------------------
// From class MImumInMailboxUtilities.
// CImumInMailboxUtilitiesImpl::IsMailbox()
// ---------------------------------------------------------------------------
//
TBool CImumInMailboxUtilitiesImpl::IsMailbox(
    const TMsvEntry& aEntry ) const
    {
    IMUM_CONTEXT( CImumInMailboxUtilitiesImpl::IsMailbox, 0, KLogInApi );
    IMUM_IN();

    // At first, make sure the entry is a service
    if ( aEntry.iType.iUid == KUidMsvServiceEntryValue )
        {
        // Get the technology type of the entry
        const TUid& technologyType =
            iMtmUiDataRegistry->TechnologyTypeUid( aEntry.iMtm );

        // Match with mail technology type
        return ( KSenduiTechnologyMailUid == technologyType );
        }

    IMUM_OUT();

    return EFalse;
    }

// ---------------------------------------------------------------------------
// From class MImumInMailboxUtilities.
// CImumInMailboxUtilitiesImpl::GetMailboxL()
// ---------------------------------------------------------------------------
//
TMsvEntry CImumInMailboxUtilitiesImpl::GetMailboxEntryL(
    const TMsvId aMailboxId,
    const TImumInMboxRequest& aType,
    const TBool aServiceCheck ) const
    {
    IMUM_CONTEXT( CImumInMailboxUtilitiesImpl::GetMailboxEntryL, 0, KLogInApi );
    IMUM_IN();

    // Get the entry based on mailbox id
    TMsvId serviceId;
    TMsvEntry entry;
    User::LeaveIfError( iMailboxApi.MsvSession().GetEntry(
        aMailboxId, serviceId, entry ) );

    // Make sure the id is mailbox
    if ( !( aServiceCheck && !IsMailbox( entry ) ) )
        {
        // Continue handling, if successful
        if ( aType != ERequestCurrent )
            {
            TInt32 mtm = entry.iMtm.iUid;

            // Get related entry, if:
            // Mailbox type is smtp and pop3 or imap4 is wanted OR
            // Mailbox type is pop3 or imap4 and smtp is wanted
            if ( aType == ERequestSending && mtm != KSenduiMtmSmtpUidValue ||
                 aType == ERequestReceiving && mtm == KSenduiMtmSmtpUidValue )
                {
                User::LeaveIfError( iMailboxApi.MsvSession().GetEntry(
                    entry.iRelatedId, serviceId, entry ) );
                }
            }
        }
    else
        {
        User::Leave( KErrUnknown );
        }

    IMUM_OUT();

    return entry;
    }

// ---------------------------------------------------------------------------
// From class MImumInMailboxUtilities.
// CImumInMailboxUtilitiesImpl::GetMailboxEntriesL()
// ---------------------------------------------------------------------------
//
const TUid& CImumInMailboxUtilitiesImpl::GetMailboxEntriesL(
    const TMsvId aMailboxId,
    RMsvEntryArray& aEntries,
    const TBool aResetArray ) const
    {
    IMUM_CONTEXT( CImumInMailboxUtilitiesImpl::GetMailboxEntriesL, 0, KLogInApi );
    IMUM_IN();

    // Reset the array on request
    if ( aResetArray )
        {
        aEntries.Reset();
        }

    // Get the entries
    aEntries.Append( GetMailboxEntryL( aMailboxId, ERequestReceiving ) );
    aEntries.Append( GetMailboxEntryL( aMailboxId, ERequestSending ) );

    IMUM_OUT();

    // return the mtm id
    return aEntries[0].iMtm;
    }

// ---------------------------------------------------------------------------
// From class MImumInMailboxUtilities.
// CImumInMailboxUtilitiesImpl::IsInbox()
// ---------------------------------------------------------------------------
//
TBool CImumInMailboxUtilitiesImpl::IsInbox(
    const TMsvEntry& aFolderEntry ) const
    {
    IMUM_CONTEXT( CImumInMailboxUtilitiesImpl::IsInbox, 0, KLogInApi );
    IMUM_IN();

    TBool isInbox( EFalse );
    if ( aFolderEntry.iDetails.CompareF( KImumInImapInboxName ) == 0 )
        {
        // If parent is Mailbox service, then this is IMAP4 protocol inbox,
        // otherwise it is something else...
        TMsvId serviceId;
        TMsvEntry entry;
        TInt error = iMailboxApi.MsvSession().GetEntry(
            aFolderEntry.Parent(), serviceId, entry );
        if ( ( error == KErrNone ) && ( entry.Id() == serviceId ) )
            {
            isInbox = ETrue;
            }
        }

    IMUM_OUT();

    return isInbox;
    }

// ---------------------------------------------------------------------------
// From class MImumInMailboxUtilities.
// CImumInMailboxUtilitiesImpl::HasWlanConnectionL()
// ---------------------------------------------------------------------------
//
TBool CImumInMailboxUtilitiesImpl::HasWlanConnectionL(
    const TMsvId aMailboxId ) const
    {
    IMUM_CONTEXT( CImumInMailboxUtilitiesImpl::HasWlanConnectionL, 0, KLogInApi );
    IMUM_IN();

    // Connecting to WLAN accesspoint is allowed to offline mode also,
    // so return ETrue if WLAN access point
    TBool wlanIap = EFalse;

    CEmailAccounts* account = CEmailAccounts::NewLC();
    CImIAPPreferences* iapPreferences = CImIAPPreferences::NewLC();

    TMsvId serviceId;
    TMsvEntry mailbox;
    TInt error = iMailboxApi.MsvSession().GetEntry( aMailboxId,
    												serviceId,
    												mailbox );

    // In some cases the mailId is passed to this method instead of the actual mailboxId so we
    // need to check the owningService as well.
    CMsvEntry* owningEntry = iMailboxApi.MsvSession().GetEntryL(aMailboxId);
    CleanupStack::PushL( owningEntry );
    TMsvId owningService = owningEntry->OwningService();
    CleanupStack::PopAndDestroy( owningEntry );

    // Make sure that the entry belongs to mailbox
    if ( !IsMailbox( mailbox ) )
    	{
    	if ( !IsMailbox( owningService ) )
        	{
        	IMUM0(0, "Not a mailbox");
        	error = KErrNotSupported;
        	}
        }

    if ( error == KErrNone )
        {
        IMUM0(0, "Retrieving preferences");
        error = CImumMboxManager::GetIapPreferencesL(
            serviceId,
            *account,
            *iapPreferences,
            iMailboxApi,
            ( mailbox.iMtm == KSenduiMtmSmtpUid ) );
        }

    if ( error == KErrNone && iapPreferences->NumberOfIAPs() )
        {
        IMUM0(0, "Accesspoints exist");
        TImIAPChoice iap = iapPreferences->IAPPreference( 0 );

        // Do not allow always ask
        if ( iap.iIAP > 0 )
            {
            CMsvCommDbUtilities* dbUtils = CMsvCommDbUtilities::NewLC();

            TRAPD( error, wlanIap = dbUtils->IsWlanAccessPointL( iap.iIAP ) );
            // If access point item is not found, force always ask
            if ( error )
                {
                CImumMboxManager::ForceAlwaysAskL(
                    serviceId,
                    *account,
                    *iapPreferences,
                    iMailboxApi,
                    EFalse );
                wlanIap = ETrue;
                }
            CleanupStack::PopAndDestroy(); // dbUtils
            }
        else
            {
            // Return ETrue for the always ask
            wlanIap = ETrue;
            }
        }
    CleanupStack::PopAndDestroy( 2, account ); // CSI: 47 # iapPreferences, account.

    IMUM_OUT();
    return wlanIap;
    }

// ----------------------------------------------------------------------------
// From class MImumInMailboxUtilities.
// CImumInMailboxUtilitiesImpl::HasSubscribedFoldersL()
// ----------------------------------------------------------------------------
//
TBool CImumInMailboxUtilitiesImpl::HasSubscribedFoldersL(
    const TMsvId aFolderId ) const
    {
    IMUM_CONTEXT( CImumInMailboxUtilitiesImpl::HasSubscribedFoldersL, 0, KLogInApi );
    IMUM_IN();

    // Make sure the id is receiving
    TMsvEntry rcv = iMailboxApi.MailboxUtilitiesL().GetMailboxEntryL(
        aFolderId,
        MImumInMailboxUtilities::ERequestReceiving,
        EFalse );

    // Get the folder entry
    CMsvEntry* folderEntry = iMailboxApi.MsvSession().GetEntryL( rcv.Id() );
    CleanupStack::PushL( folderEntry );

    // Subscribed folders are hidden entries before the first sync
    TMsvSelectionOrdering order = folderEntry->SortType();
    order.SetShowInvisibleEntries( ETrue );
    folderEntry->SetSortTypeL( order );

    TBool found( EFalse );

    // Search through all the folders and locate the subscribed ones
    const TInt count = folderEntry->Count();
    for ( TInt loop = 0; !found && loop < count; loop++ )
        {
        TMsvEmailEntry mailEntry = ( *folderEntry )[ loop ];
        if ( mailEntry.iType.iUid == KUidMsvFolderEntryValue )
            {
            if ( mailEntry.LocalSubscription() && !IsInbox( mailEntry ) )
                {
                found = ETrue;
                }
            else
                {
                // Go into the folder to search the new entry
                found = HasSubscribedFoldersL( mailEntry.Id() );
                }
            }
        }

    CleanupStack::PopAndDestroy( folderEntry );

    IMUM_OUT();

    return found;
    }

// ---------------------------------------------------------------------------
// From class MImumInMailboxUtilities.
// CImumInMailboxUtilitiesImpl::IsAlwaysOnlineOnL()
// ---------------------------------------------------------------------------
//
void CImumInMailboxUtilitiesImpl::QueryAlwaysOnlineStateL(
    const TMsvId aMailboxId,
    TInt64& aAlwaysOnlineStatus ) const
    {
    IMUM_CONTEXT( CImumInMailboxUtilitiesImpl::QueryAlwaysOnlineStateL, 0, KLogInApi );
    IMUM_IN();

    // Query the result from the scheduler utility
    CImumMboxScheduler* scheduler = CImumMboxScheduler::NewLC(
        iMailboxApi, aMailboxId );
    aAlwaysOnlineStatus = scheduler->QueryAlwaysOnlineStateL();

    // Cleanup
    CleanupStack::PopAndDestroy( scheduler );
    scheduler = NULL;

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// From class MImumInMailboxUtilities.
// CImumInMailboxUtilitiesImpl::NextAlwaysOnlineIntervalL()
// ---------------------------------------------------------------------------
//
void CImumInMailboxUtilitiesImpl::NextAlwaysOnlineIntervalL(
    const TMsvId aMailboxId,
    TInt64& aAlwaysOnlineStatus,
    TTimeIntervalSeconds& aSeconds ) const
    {
    IMUM_CONTEXT( CImumInMailboxUtilitiesImpl::NextAlwaysOnlineIntervalL, 0, KLogInApi );
    IMUM_IN();

    // Query the result from the scheduler utility
    CImumMboxScheduler* scheduler = CImumMboxScheduler::NewLC(
        iMailboxApi, aMailboxId );
    aAlwaysOnlineStatus = scheduler->SecondsToNextMark( aSeconds );

    // Cleanup
    CleanupStack::PopAndDestroy( scheduler );
    scheduler = NULL;

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// From class MImumInMailboxUtilities.
// CImumInMailboxUtilitiesImpl::DefaultMailboxIdL()
// ---------------------------------------------------------------------------
//
TMsvId CImumInMailboxUtilitiesImpl::DefaultMailboxIdL(
    const TBool aForceGet ) const
    {
    IMUM_CONTEXT( CImumInMailboxUtilitiesImpl::DefaultMailboxIdL, 0, KLogInApi );
    IMUM_IN();

    // The system must have default mtm for mailboxes, so find one
    TUid defaultMtm = KSenduiMtmSmtpUid;

    TMsvId defaultId = MuiuEmailTools::DefaultSendingMailboxL(
        iMailboxApi.MsvSession(), defaultMtm );

    // There's no such of thing, so force find any.
    // This may be true in case the message store has been swapped between
    // the phone and memorycard. Forcing default mailbox makes it possible
    // to write and send emails without problems.
    if ( aForceGet && defaultId == KMsvUnknownServiceIndexEntryId )
        {
        // Find first valid mailbox
        iMailboxApi.MailboxServicesL().SetDefaultMailboxL( 0 );
        defaultId = MuiuEmailTools::DefaultSendingMailboxL(
            iMailboxApi.MsvSession(), defaultMtm );
        }

    IMUM_OUT();
    return defaultId;
    }

