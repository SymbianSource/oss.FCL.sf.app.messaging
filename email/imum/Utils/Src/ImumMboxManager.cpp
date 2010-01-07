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
* Description: 
*       Encapsulates saving and removing email accounts
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <msvapi.h>                     // CMsvEntry
#include <cemailaccounts.h>             // CEmailAccounts
#include <smtpset.h>                    // CImSmtpSettings
#include <pop3set.h>                    // CImPop3Settings
#include <imapset.h>                    // CImImap4Settings
#include <iapprefs.h>                   // CImIAPPreferences
#include <muiumsvuiserviceutilitiesinternal.h>
#include <SendUiConsts.h>

#include "ImumInternalApiImpl.h"
#include "ImumMboxSymbianDataConverter.h"
#include "ImumMboxInternalDataConverter.h"
#include "ImumMboxManager.h"
#include "ImumInHealthServicesImpl.h"
#include "ImumMboxData.h"
#include "ImumMboxSettingsCtrl.h"
#include "IMSSettingsNoteUi.h"
#include "ImumUtilsLogging.h"
#include "EmailUtils.H"
#include "ImumPanic.h"

#include <mtudreg.h>
#include <muiumsvuiserviceutilitiesinternal.h>
#include <featmgr.h>

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
const TInt KImumMboxQueryTextLength    = 100;
const TInt KImumMboxArrayGranularity   = 4;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================


// ----------------------------------------------------------------------------
// CImumMboxManager::CImumMboxManager()
// ----------------------------------------------------------------------------
//
CImumMboxManager::CImumMboxManager(
    CImumInternalApiImpl& aMailboxApi )
    :
    iMailboxApi( aMailboxApi ),
    iMboxSettingsCtrl( NULL ),
    iAccounts( NULL ),
    iCleaner( NULL )
    {
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::ConstructL()
    {
    IMUM_CONTEXT( CImumMboxManager::ConstructL, 0, KLogData );
    IMUM_IN();

    FeatureManager::InitializeLibL();

    iAccounts = CEmailAccounts::NewL();
    iMboxSettingsCtrl = CImumMboxSettingsCtrl::NewL( iMailboxApi );
    iCleaner = CImumInHealthServicesImpl::NewL( iMailboxApi );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::~CImumMboxManager()
// ----------------------------------------------------------------------------
//
CImumMboxManager::~CImumMboxManager()
    {
    IMUM_CONTEXT( CImumMboxManager::~CImumMboxManager, 0, KLogData );
    IMUM_IN();

    FeatureManager::UnInitializeLib();
    delete iMboxSettingsCtrl;
    iMboxSettingsCtrl = NULL;
    delete iAccounts;
    iAccounts = NULL;
    delete iCleaner;
    iCleaner = NULL;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::NewL()
// ----------------------------------------------------------------------------
//
CImumMboxManager* CImumMboxManager::NewL(
    CImumInternalApiImpl& aMailboxApi )
    {
    IMUM_STATIC_CONTEXT( CImumMboxManager::NewL, 0, utils, KLogData );
    IMUM_IN();

    CImumMboxManager* self = NewLC( aMailboxApi );
    CleanupStack::Pop( self );

    IMUM_OUT();

    return self;
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::NewLC()
// ----------------------------------------------------------------------------
//
CImumMboxManager* CImumMboxManager::NewLC(
    CImumInternalApiImpl& aMailboxApi )
    {
    IMUM_STATIC_CONTEXT( CImumMboxManager::NewLC, 0, utils, KLogData );
    IMUM_IN();

    CImumMboxManager* self =
        new ( ELeave ) CImumMboxManager( aMailboxApi );
    CleanupStack::PushL( self );
    self->ConstructL();

    IMUM_OUT();

    return self;
    }

/******************************************************************************

    Static Functions

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxManager::GetIapPreferencesL()
// ----------------------------------------------------------------------------
//
TInt CImumMboxManager::GetIapPreferencesL(
    const TMsvId aMailboxId,
    CEmailAccounts& aAccounts,
    CImIAPPreferences& aIapPrefs,
    CImumInternalApiImpl& aMailboxApi,
    const TBool aSmtp )
    {
    IMUM_STATIC_CONTEXT( CImumMboxManager::GetIapPreferencesL, 0, utils, KLogData );
    IMUM_IN();

    TMsvEntry entry;

    // Get the correct part of the mailbox
    if ( aSmtp )
        {
        // Get SMTP part of the mailbox
        entry = aMailboxApi.MailboxUtilitiesL().GetMailboxEntryL(
            aMailboxId, MImumInMailboxUtilities::ERequestSending );

        // Define the account id
        TSmtpAccount smtpAccountId;
        smtpAccountId.iSmtpAccountId = entry.iMtmData2;

        // Load preferences
        aAccounts.LoadSmtpIapSettingsL( smtpAccountId, aIapPrefs );
        }
    else
        {
        // Get POP3 or IMAP4 part of the mailbox
        entry = aMailboxApi.MailboxUtilitiesL().GetMailboxEntryL(
            aMailboxId, MImumInMailboxUtilities::ERequestReceiving );

        // Load settings based on the mtm type
        if ( entry.iMtm.iUid == KSenduiMtmImap4UidValue )
            {
            // Define imap4 account id
            TImapAccount imapAccountId;
            imapAccountId.iImapAccountId = entry.iMtmData2;

            // Load preferences
            aAccounts.LoadImapIapSettingsL( imapAccountId, aIapPrefs );
            }
        else
            {
            // Define pop3 account id
            TPopAccount popAccountId;
            popAccountId.iPopAccountId = entry.iMtmData2;

            // Load preferences
            aAccounts.LoadPopIapSettingsL( popAccountId, aIapPrefs );
            }
        }

    IMUM_OUT();

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::SetIapPreferencesL()
// ----------------------------------------------------------------------------
//
TInt CImumMboxManager::SetIapPreferencesL(
    const TMsvId aMailboxId,
    CEmailAccounts& aAccounts,
    const CImIAPPreferences& aIapPrefs,
    CImumInternalApiImpl& aMailboxApi,
    const TBool aSmtp )
    {
    IMUM_STATIC_CONTEXT( CImumMboxManager::SetIapPreferencesL, 0, utils, KLogData );
    IMUM_IN();

    TMsvEntry entry;
    TInt error = KErrNone;

    // Get the correct part of the mailbox
    if ( aSmtp )
        {
        // Get SMTP part of the mailbox
        entry = aMailboxApi.MailboxUtilitiesL().GetMailboxEntryL(
            aMailboxId, MImumInMailboxUtilities::ERequestSending );

        // Define the account id
        TSmtpAccount smtpAccountId;
        smtpAccountId.iSmtpAccountId = entry.iMtmData2;

        // Load preferences
        aAccounts.SaveSmtpIapSettingsL( smtpAccountId, aIapPrefs );
        }
    else
        {
        // Get POP3 or IMAP4 part of the mailbox
        entry = aMailboxApi.MailboxUtilitiesL().GetMailboxEntryL(
            aMailboxId, MImumInMailboxUtilities::ERequestReceiving );

        // Load settings based on the mtm type
        if ( entry.iMtm.iUid == KSenduiMtmImap4UidValue )
            {
            // Define imap4 account id
            TImapAccount imapAccountId;
            imapAccountId.iImapAccountId = entry.iMtmData2;

            // Load preferences
            aAccounts.SaveImapIapSettingsL( imapAccountId, aIapPrefs );
            }
        else
            {
            // Define pop3 account id
            TPopAccount popAccountId;
            popAccountId.iPopAccountId = entry.iMtmData2;

            // Load preferences
            aAccounts.SavePopIapSettingsL( popAccountId, aIapPrefs );
            }
        }

    IMUM_OUT();

    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::ForceAlwaysAskL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::ForceAlwaysAskL(
    const TMsvId aMailboxId,
    CEmailAccounts& aAccounts,
    CImIAPPreferences& aIapPrefs,
    CImumInternalApiImpl& aMailboxApi,
    const TBool aSmtp )
    {
    IMUM_STATIC_CONTEXT( CImumMboxManager::ForceAlwaysAskL, 0, utils, KLogData );
    IMUM_IN();

    aIapPrefs.RemoveIAPL( 0 );
    TImIAPChoice iap;
    iap.iDialogPref = ECommDbDialogPrefPrompt;
    iap.iIAP = 0;
    aIapPrefs.AddIAPL( iap );
    User::LeaveIfError( CImumMboxManager::SetIapPreferencesL(
        aMailboxId, aAccounts, aIapPrefs, aMailboxApi, aSmtp ) );

    IMUM_OUT();
    }

/******************************************************************************

    Manager tools

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxManager::CreateAccountFillIapL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::CreateAccountFillIapL(
    CImumMboxData& aSettings )
    {
    IMUM_CONTEXT( CImumMboxManager::CreateAccountFillIapL, 0, KLogData );
    IMUM_IN();

    // This is not ideal choice, but current situation forces to do this,
    // until Reset() funcion is exported from class CImIAPPreferences
    delete aSettings.iIncomingIapPref;
    aSettings.iIncomingIapPref = NULL;
    delete aSettings.iOutgoingIapPref;
    aSettings.iOutgoingIapPref = NULL;

    // Make sure the preferences are made
    if ( !aSettings.iIncomingIapPref )
        {
        aSettings.iIncomingIapPref = CImIAPPreferences::NewLC();
        CleanupStack::Pop( aSettings.iIncomingIapPref );
        }

    if ( !aSettings.iOutgoingIapPref )
        {
        aSettings.iOutgoingIapPref = CImIAPPreferences::NewLC();
        CleanupStack::Pop( aSettings.iOutgoingIapPref );
        }

    // Set incoming internet access point
    TImIAPChoice incomingIapChoice;
    incomingIapChoice.iIAP = aSettings.iIncomingIap;
    incomingIapChoice.iDialogPref = ECommDbDialogPrefDoNotPrompt;
    aSettings.iIncomingIapPref->AddIAPL( incomingIapChoice );

    // Set outgoing internet access point
    TImIAPChoice outgoingIapChoice;
    outgoingIapChoice.iIAP = aSettings.iOutgoingIap;
    outgoingIapChoice.iDialogPref = ECommDbDialogPrefDoNotPrompt;
    aSettings.iOutgoingIapPref->AddIAPL( outgoingIapChoice );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::LoadAccountFillIapL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::LoadAccountFillIapL(
    CImumMboxData& aSettings )
    {
    IMUM_CONTEXT( CImumMboxManager::LoadAccountFillIapL, 0, KLogData );
    IMUM_IN();
	
	//check IAP number 
    if ( aSettings.iIncomingIapPref->NumberOfIAPs() > 0 )
    	{
    // Set internet access point (incoming IAP)
    TImIAPChoice incomingIapChoice =
        aSettings.iIncomingIapPref->IAPPreference( 0 );
    aSettings.iIncomingIap = incomingIapChoice.iIAP;

    // Set internet access point (Outgoing IAP)
    TImIAPChoice outgoingIapChoice =
        aSettings.iOutgoingIapPref->IAPPreference( 0 );
    aSettings.iOutgoingIap = outgoingIapChoice.iIAP;
    	}
		
	//check SNAP define	
    else if( aSettings.iIncomingIapPref->SNAPDefined() )
    	{
    	aSettings.iIncomingIap = aSettings.iIncomingIapPref->SNAPPreference();
    	aSettings.iOutgoingIap = aSettings.iOutgoingIapPref->SNAPPreference();
    	}	
    else 
    	{
    	aSettings.iIncomingIap = 0;
    	aSettings.iOutgoingIap = 0;
    	}
    	
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::DetermineDefaultAccountL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::DetermineDefaultAccountL(
    CImumMboxData& aSettings )
    {
    IMUM_CONTEXT( CImumMboxManager::DetermineDefaultAccountL, 0, KLogData );
    IMUM_IN();

    // First check if default account id is set
    aSettings.iDefaultAccountId =
        MsvUiServiceUtilitiesInternal::DefaultServiceForMTML(
            iMailboxApi.MsvSession(), KSenduiMtmSmtpUid, EFalse );

    // Set the newly created one as a default account
    if ( aSettings.iDefaultAccountId == KMsvUnknownServiceIndexEntryId )
        {
		if( !FeatureManager::FeatureSupported( KFeatureIdSelectableEmail ) ||
			!MsvUiServiceUtilitiesInternal::OtherEmailMTMExistL( 
			iMailboxApi.MsvSession(), aSettings.iMailboxId ) )
			{
            TSmtpAccount smtpAccountId;
            iAccounts->GetSmtpAccountL( aSettings.iMailboxId, smtpAccountId );
            iAccounts->SetDefaultSmtpAccountL( smtpAccountId );

			MsvUiServiceUtilitiesInternal::SetDefaultServiceForMTML(
					iMailboxApi.MsvSession(), KSenduiMtmSmtpUid, 
					aSettings.iMailboxId );
			aSettings.iDefaultAccountId = aSettings.iMailboxId;
			}
		}
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::RemoveAccountL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::RemoveAccountL(
    const CImumMboxData& aSettings )
    {
    IMUM_CONTEXT( CImumMboxManager::RemoveAccountL, 0, KLogData );
    IMUM_IN();

    // Delete Imap4 or Pop3 account
    if ( aSettings.iIsImap4 )
        {
        DeleteExtendedSettings( TIMAStorerParams(
            aSettings.iImap4AccountId.iImapAccountId, KSenduiMtmImap4Uid ) );

        DeleteBaseSettingsL( aSettings );
        }
    else
        {
        DeleteExtendedSettings( TIMAStorerParams(
            aSettings.iPop3AccountId.iPopAccountId, KSenduiMtmPop3Uid ) );

        DeleteBaseSettingsL( aSettings );
        }

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::RemoveAccountL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::RemoveAccountL(
    const TMsvEntry& aMailboxEntry, TBool aSilent )
    {
    IMUM_CONTEXT( CImumMboxManager::RemoveAccountL, 0, KLogData );
    IMUM_IN();

    if ( aSilent || AccountDeletionConditionsOkL( aMailboxEntry.Id() ) )
        {
        // Delete Imap4 or Pop3 account
        if ( aMailboxEntry.iMtm == KSenduiMtmImap4Uid )
            {
            DeleteExtendedSettings( TIMAStorerParams(
                aMailboxEntry.iMtmData2, KSenduiMtmImap4Uid ) );

            DeleteBaseSettingsL( aMailboxEntry.iMtmData2, ETrue );
            }
        else
            {
            DeleteExtendedSettings( TIMAStorerParams(
                aMailboxEntry.iMtmData2, KSenduiMtmPop3Uid ) );

            DeleteBaseSettingsL( aMailboxEntry.iMtmData2, EFalse );
            }
        }

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::RemoveAccountL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::RemoveAccountL(
    const TUint32 aRelatedAccountId,
    const TUid& aMtmId )
    {
    IMUM_CONTEXT( CImumMboxManager::RemoveAccountL, 0, KLogData );
    IMUM_IN();

    // First remove the extended settings, as this cannot leave
    DeleteExtendedSettings(
        TIMAStorerParams( aRelatedAccountId, aMtmId ) );

    // Delete Imap4 or Pop3 account
    DeleteBaseSettingsL(
        aRelatedAccountId, aMtmId == KSenduiMtmImap4Uid );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumMboxManager::RemoveAccount()
// ---------------------------------------------------------------------------
//
void CImumMboxManager::RemoveAccount( const TMsvId aMailboxId, TBool aSilent )
    {
    IMUM_CONTEXT( CImumMboxManager::RemoveAccount, 0, KLogData );
    IMUM_IN();

    TMsvEntry entry;
    TRAPD( error, entry = iMailboxApi.MailboxUtilitiesL().GetMailboxEntryL(
        aMailboxId,
        MImumInMailboxUtilities::ERequestReceiving ) );

    if ( !error )
        {
        TRAP_IGNORE( RemoveAccountL( entry, aSilent ) );
        }

    IMUM_OUT();
    }


/******************************************************************************

    Account creation

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxManager::CreateEmailAccount()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::CreateEmailAccount(
    CImumMboxData& aSettings,
    const TBool aPopulateDefaults )
    {
    IMUM_CONTEXT( CImumMboxManager::CreateEmailAccount, 0, KLogData );
    IMUM_IN();

    // If settings not ready, don't try to create
    if ( aSettings.IsOk() )
        {
        // Attempt to create the mailaccount
        TRAPD( error, CreateEmailAccountL( aSettings, aPopulateDefaults ) );

        // Creating mailbox has failed, delete entries and cenrep settings
        if ( error != KErrNone )
            {
            TRAP_IGNORE( RemoveAccountL( aSettings ) );
            __ASSERT_DEBUG( EFalse, User::Panic( KImumMtmUiPanic, error ) );
            }
        }

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumMboxManager::CreateEmailAccountL()
// ---------------------------------------------------------------------------
//
TMsvId CImumMboxManager::CreateEmailAccountL(
    const CImumInSettingsData& aSettingsData )
    {
    IMUM_CONTEXT( CImumMboxManager::CreateEmailAccountL, 0, KLogData );
    IMUM_IN();

    CImumMboxData* settings =
        CImumMboxSymbianDataConverter::ConvertToSymbianMboxDataLC(
            iMailboxApi, aSettingsData );

    CreateEmailAccountL( *settings, EFalse );

    TMsvId mboxId = settings->iMailboxId;
    CleanupStack::PopAndDestroy( settings );
    settings = NULL;

    IMUM_OUT();

    return mboxId;
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::CreateEmailAccountL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::CreateEmailAccountL(
    CImumMboxData& aSettings,
    const TBool aPopulateDefaults )
    {
    IMUM_CONTEXT( CImumMboxManager::CreateEmailAccountL, 0, KLogData );
    IMUM_IN();

    TUint32 id = 0;
    TUid mtmId;
    TMsvId serviceId = 0;
    IMUM0(0, " ### Starting mailbox creation processs");

    // Check for imap4
    if ( aSettings.iIsImap4 )
        {
        IMUM0( 0, " # STEP1 : Create IMAP4 account" );
        // Catch all the problems with account creation
        id = CreateImap4AccountL( aSettings, aPopulateDefaults );
        serviceId = aSettings.iImap4AccountId.iImapService;
        mtmId = KSenduiMtmImap4Uid;
        }
    else
        {
        IMUM0( 0, " # STEP1 : Create POP3 account" );

        id = CreatePop3AccountL( aSettings, aPopulateDefaults );
        serviceId = aSettings.iPop3AccountId.iPopService;
        mtmId = KSenduiMtmPop3Uid;
        }

    IMUM0( 0, " # STEP1.5 : Store IMEI" );
    iCleaner->StoreIMEIToMailbox( aSettings.iMailboxId );

    IMUM0( 0, " # STEP2 : Create extended settings" );
    CreateExtendedSettingsL( aSettings.iMailboxId, id, mtmId, aSettings, aPopulateDefaults );

    IMUM0( 0, " # STEP3 : Store account ID to entry" );
    StoreAccountIdToEntryL( aSettings );

    IMUM0( 0, " # STEP4 : Choose default account" );
    DetermineDefaultAccountL( aSettings );

    IMUM0( 0, " # STEP5 : Store signature" );
    TRAP_IGNORE( StoreSignatureL( aSettings ) );
    
    IMUM0( 0, " # STEP6 : Activate AO Email plugin if needed" );
    if( serviceId && ( 
        EMailAoOff  != aSettings.iExtendedSettings->AlwaysOnlineState() ||
        EMailEmnOff != aSettings.iExtendedSettings->EmailNotificationState() ) )
        {
        TRAPD( error, MsvEmailMtmUiUtils::SendAOCommandL( EServerAPIEmailTurnOn, serviceId ) );
		if ( error != KErrNotFound )
			{
			User::LeaveIfError( error );
			}
        }
    IMUM0(0, " ### Finishing mailbox creation processs");
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::CreateImap4AccountL()
// ----------------------------------------------------------------------------
//
TUint32 CImumMboxManager::CreateImap4AccountL(
    CImumMboxData& aSettings,
    const TBool aPopulateDefaults,
    const TBool aReadOnly )
    {
    IMUM_CONTEXT( CImumMboxManager::CreateImap4AccountL, 0, KLogData );
    IMUM_IN();

    // Populate defaults
    if ( aPopulateDefaults )
        {
        iAccounts->PopulateDefaultImapSettingsL(
            *aSettings.iImap4Settings, *aSettings.iIncomingIapPref );
        iAccounts->PopulateDefaultSmtpSettingsL(
            *aSettings.iSmtpSettings, *aSettings.iOutgoingIapPref );
        }

    // Fill iap preferences
    CreateAccountFillIapL( aSettings );

    // Create the account
    aSettings.iImap4AccountId = iAccounts->CreateImapAccountL(
        aSettings.iName, *aSettings.iImap4Settings,
        *aSettings.iIncomingIapPref, aReadOnly );
    iAccounts->CreateSmtpAccountL( aSettings.iImap4AccountId,
        *aSettings.iSmtpSettings, *aSettings.iOutgoingIapPref, aReadOnly );

    aSettings.iMailboxId = aSettings.iImap4AccountId.iSmtpService;

    IMUM_OUT();

    return aSettings.iImap4AccountId.iImapAccountId;
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::CreatePop3AccountL()
// ----------------------------------------------------------------------------
//
TUint32 CImumMboxManager::CreatePop3AccountL(
    CImumMboxData& aSettings,
    const TBool aPopulateDefaults,
    const TBool aReadOnly )
    {
    IMUM_CONTEXT( CImumMboxManager::CreatePop3AccountL, 0, KLogData );
    IMUM_IN();

    // Populate defaults
    if ( aPopulateDefaults )
        {
        iAccounts->PopulateDefaultPopSettingsL(
            *aSettings.iPop3Settings, *aSettings.iIncomingIapPref );
        iAccounts->PopulateDefaultSmtpSettingsL(
            *aSettings.iSmtpSettings, *aSettings.iOutgoingIapPref );
        }

    // Fill iap preferences
    CreateAccountFillIapL( aSettings );

    // Create the account
    aSettings.iPop3AccountId = iAccounts->CreatePopAccountL(
        aSettings.iName,
        *aSettings.iPop3Settings, *aSettings.iIncomingIapPref, aReadOnly );
    iAccounts->CreateSmtpAccountL( aSettings.iPop3AccountId,
        *aSettings.iSmtpSettings, *aSettings.iOutgoingIapPref, aReadOnly );

    aSettings.iMailboxId = aSettings.iPop3AccountId.iSmtpService;

    IMUM_OUT();

    return aSettings.iPop3AccountId.iPopAccountId;
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::CreateExtendedSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::CreateExtendedSettingsL(
    const TMsvId aMailboxId,
    const TUint32 aAccountId,
    const TUid& aProtocol,
    CImumMboxData& aSettings,
    const TBool aPopulateDefaults )
    {
    IMUM_CONTEXT( CImumMboxManager::CreateExtendedSettingsL, 0, KLogData );
    IMUM_IN();

    // If populating has been successful
    User::LeaveIfError( iMboxSettingsCtrl->CreateSettings(
        aMailboxId, aAccountId, aProtocol,
        *aSettings.iExtendedSettings, aPopulateDefaults ) );

    IMUM_OUT();
    }

/******************************************************************************

    Account Deletion

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxManager::DeleteExtendedSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxManager::DeleteExtendedSettings(
    const TIMAStorerParams& aParams )
    {
    // Delete the settings
    return iMboxSettingsCtrl->DeleteExtendedSettings( aParams );
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::DeleteRelatedSmtpAccountsL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::DeleteRelatedSmtpAccountsL(
    const CImumMboxData& aSettings )
    {
    IMUM_CONTEXT( CImumMboxManager::DeleteRelatedSmtpAccountsL, 0, KLogData );
    IMUM_IN();

    // Get the available smtp accounts
    RArray<TSmtpAccount> smtpAccountArray;
    iAccounts->GetSmtpAccountsL( smtpAccountArray );

    // Get the related service id
    TMsvId relatedServiceId = aSettings.iIsImap4 ?
        aSettings.iImap4AccountId.iImapService :
        aSettings.iPop3AccountId.iPopService;

    // Delete the accounts
    DeleteSmtpAccountsL( relatedServiceId, smtpAccountArray );

    smtpAccountArray.Reset();

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::DeleteRelatedSmtpAccountsL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::DeleteRelatedSmtpAccountsL(
    const TUint32 aRelatedAccountId,
    const TBool aIsImap4 )
    {
    IMUM_CONTEXT( CImumMboxManager::DeleteRelatedSmtpAccountsL, 0, KLogData );
    IMUM_IN();

    // Get the available imap accounts
    TMsvId relatedService = KMsvNullIndexEntryId;

    if ( aIsImap4 )
        {
        // Get Imap account
        RIMASImap4IdArray imapAccountArray;
        iAccounts->GetImapAccountsL( imapAccountArray );

        // Find the account from imap accounts
        for ( TInt mailbox = imapAccountArray.Count();
            --mailbox >= 0 && relatedService == KMsvNullIndexEntryId; )
            {
            TUint32 imapId = imapAccountArray[mailbox].iImapAccountId;
            if ( imapId == aRelatedAccountId )
                {
                relatedService = imapAccountArray[mailbox].iImapService;
                }
            }

        imapAccountArray.Reset();
        }
    else
        {
        // Get pop account
        RIMASPop3IdArray popAccountArray;
        iAccounts->GetPopAccountsL( popAccountArray );

        // Find the account from pop accounts
        for ( TInt mailbox = popAccountArray.Count();
            --mailbox >= 0 && relatedService == KMsvNullIndexEntryId; )
            {
            TUint32 popId = popAccountArray[mailbox].iPopAccountId;
            if ( popId == aRelatedAccountId )
                {
                relatedService = popAccountArray[mailbox].iPopService;
                }
            }

        popAccountArray.Reset();
        }

    // If found, remove the accounts
    if ( relatedService != KMsvNullIndexEntryId )
        {
        RIMASSmtpIdArray smtpAccountArray;
        iAccounts->GetSmtpAccountsL( smtpAccountArray );

        // Delete the accounts
        DeleteSmtpAccountsL( relatedService, smtpAccountArray );
        smtpAccountArray.Reset();
        }

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::DeleteSmtpAccountsL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::DeleteSmtpAccountsL(
    const TMsvId aRelatedMailboxId,
    const RIMASSmtpIdArray& aSmtpAccountArray )
    {
    IMUM_CONTEXT( CImumMboxManager::DeleteSmtpAccountsL, 0, KLogData );
    IMUM_IN();

    TSmtpAccount smtpId;

    // Start checking the mailboxes
    for ( TInt mailbox = aSmtpAccountArray.Count(); --mailbox >= 0; )
        {
        smtpId = aSmtpAccountArray[mailbox];

        // If the related id matches with the smtp accounts related service
        // id, the account can be deleted.
        if ( aRelatedMailboxId == smtpId.iRelatedService )
            {
            iAccounts->DeleteSmtpAccountL( smtpId );
            }
        }

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::DeleteBaseSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::DeleteBaseSettingsL(
    const TUint32 aRelatedAccountId,
    const TBool aIsImap4 )
    {
    IMUM_CONTEXT( CImumMboxManager::DeleteBaseSettingsL, 0, KLogData );

    // Remove the related smtp accounts
    DeleteRelatedSmtpAccountsL( aRelatedAccountId, aIsImap4 );

    // Determine the account type
    if ( aIsImap4 )
        {
        // Remove the Imap4 account
        TImapAccount imapId;
        imapId.iImapAccountId = aRelatedAccountId;
        iAccounts->DeleteImapAccountL( imapId );
        }
    else
        {
        // Remove the Pop3 account
        TPopAccount popId;
        popId.iPopAccountId = aRelatedAccountId;
        iAccounts->DeletePopAccountL( popId );
        }

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::DeleteBaseSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::DeleteBaseSettingsL(
    const CImumMboxData& aSettings )
    {
    IMUM_CONTEXT( CImumMboxManager::DeleteBaseSettingsL, 0, KLogData );
    IMUM_IN();

    // Remove the related smtp accounts
    DeleteRelatedSmtpAccountsL( aSettings );

    // Determine the account type
    if ( aSettings.iIsImap4 )
        {
        // Remove the Imap4 account
        iAccounts->DeleteImapAccountL( aSettings.iImap4AccountId );
        }
    else
        {
        // Remove the Pop3 account
        iAccounts->DeletePopAccountL( aSettings.iPop3AccountId );
        }

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::DeleteAccountSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxManager::DeleteAccountSettings(
    const TUint32 aAccountId,
    const TUid& aMtmId )
    {
    IMUM_CONTEXT( CImumMboxManager::DeleteAccountSettings, 0, KLogData );
    IMUM_IN();

    TInt error = KErrNone;

    // Delete the related extended settings
    if ( aMtmId == KSenduiMtmImap4Uid )
        {
        // Delete Imap4 or Pop3 account
        TImapAccount imapId;
        imapId.iImapAccountId = aAccountId;
        TRAP( error, iAccounts->DeleteImapAccountL( imapId ) );
        }
    else if ( aMtmId == KSenduiMtmPop3Uid )
        {
        // Remove the Pop3 account
        TPopAccount popId;
        popId.iPopAccountId = aAccountId;
        TRAP( error, iAccounts->DeletePopAccountL( popId ) );
        }
    else
        {
        // Remove the Smtp account
        TSmtpAccount smtpId;
        smtpId.iSmtpAccountId = aAccountId;
        TRAP( error, iAccounts->DeleteSmtpAccountL( smtpId ) );
        }

    if ( aMtmId != KSenduiMtmSmtpUid )
        {
        DeleteExtendedSettings(
            TIMAStorerParams( aAccountId, aMtmId ) );
        }

    IMUM_OUT();

    return error;
    }

// ---------------------------------------------------------------------------
// CImumMboxManager::CheckAccountDeletionConditionsL()
// ---------------------------------------------------------------------------
//
TBool CImumMboxManager::CheckAccountDeletionConditionsL(
    const TMsvId aMailboxId )
    {
    return AccountDeletionConditionsOkL( aMailboxId );
    }

/******************************************************************************

    Account Saving

******************************************************************************/

// ---------------------------------------------------------------------------
// CImumMboxManager::SaveEmailSettingsL()
// ---------------------------------------------------------------------------
//
void CImumMboxManager::SaveEmailSettingsL(
    const CImumInSettingsData& aSettingsData )
    {
    IMUM_CONTEXT( CImumMboxManager::SaveEmailSettingsL, 0, KLogData );
    IMUM_IN();

    CImumMboxData* settings =
        CImumMboxSymbianDataConverter::ConvertToSymbianMboxDataLC(
            iMailboxApi, aSettingsData );

    SaveEmailSettingsL( *settings );

    CleanupStack::PopAndDestroy( settings );
    settings = NULL;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::SaveEmailSettingsL()
// ----------------------------------------------------------------------------
//
TInt CImumMboxManager::SaveEmailSettingsL(
    CImumMboxData& aSettings )
    {
    IMUM_CONTEXT( CImumMboxManager::SaveEmailSettingsL, 0, KLogData );
    IMUM_IN();

    // If settings not ready, don't try to create
    if ( !aSettings.IsOk() )
        {
        return KErrNotFound;
        }

    TInt error = KErrNone;
    CreateAccountFillIapL( aSettings );

    // Check for imap4
    if ( aSettings.iIsImap4 )
        {
        // Catch all the problems with account creation
        TRAP( error, SaveImap4SettingsL( aSettings ) );

        // Attempt to save the extended settings, even if other settings have
        // failed to save
        iMboxSettingsCtrl->SaveSettings( *aSettings.iExtendedSettings );
        TRAP_IGNORE( StoreSignatureL( aSettings ) );
        }
    // It's a pop
    else
        {
        // Catch all the problems with account creation
        TRAP( error, SavePop3SettingsL( aSettings ) );

        // Attempt to save the extended settings, even if other settings have
        // failed to save
        iMboxSettingsCtrl->SaveSettings( *aSettings.iExtendedSettings );
        TRAP_IGNORE( StoreSignatureL( aSettings ) );
        }

    IMUM_OUT();

    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::SavePop3SettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::SavePop3SettingsL(
    CImumMboxData& aSettings )
    {
    IMUM_CONTEXT( CImumMboxManager::SavePop3SettingsL, 0, KLogData );
    IMUM_IN();

    TSmtpAccount smtpAccountId;
    iAccounts->GetSmtpAccountL(
        aSettings.iPop3AccountId.iSmtpService, smtpAccountId );

    // Store the mailbox name
    aSettings.iPop3AccountId.iPopAccountName.Copy(
        aSettings.iName );
    smtpAccountId.iSmtpAccountName.Copy(
        aSettings.iName );

    // Load mailbox settings
    iAccounts->SavePopSettingsL(
        aSettings.iPop3AccountId, *aSettings.iPop3Settings );
    iAccounts->SavePopIapSettingsL(
        aSettings.iPop3AccountId, *aSettings.iIncomingIapPref );
    iAccounts->SaveSmtpSettingsL(
        smtpAccountId, *aSettings.iSmtpSettings );
    iAccounts->SaveSmtpIapSettingsL(
        smtpAccountId, *aSettings.iOutgoingIapPref );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::SaveImap4SettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::SaveImap4SettingsL(
    CImumMboxData& aSettings )
    {
    IMUM_CONTEXT( CImumMboxManager::SaveImap4SettingsL, 0, KLogData );
    IMUM_IN();

    TSmtpAccount smtpAccountId;
    iAccounts->GetSmtpAccountL(
        aSettings.iImap4AccountId.iSmtpService, smtpAccountId );

    // Store the mailbox name
    aSettings.iImap4AccountId.iImapAccountName.Copy(
        aSettings.iName );
    smtpAccountId.iSmtpAccountName.Copy(
        aSettings.iName );

    // Load mailbox settings
    iAccounts->SaveImapSettingsL(
        aSettings.iImap4AccountId, *aSettings.iImap4Settings );
    iAccounts->SaveImapIapSettingsL(
        aSettings.iImap4AccountId, *aSettings.iIncomingIapPref );
    iAccounts->SaveSmtpSettingsL(
        smtpAccountId, *aSettings.iSmtpSettings );
    iAccounts->SaveSmtpIapSettingsL(
        smtpAccountId, *aSettings.iOutgoingIapPref );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::StoreAccountIdToEntryL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::StoreAccountIdToEntryL(
    const TMsvId aMailboxId,
    const TUint32 aAccountId )
    {
    IMUM_CONTEXT( CImumMboxManager::StoreAccountIdToEntryL, 0, KLogData );
    IMUM_IN();

    // Get the entry
    CMsvEntry* cEntry = iMailboxApi.MsvSession().GetEntryL( aMailboxId );
    CleanupStack::PushL( cEntry );

    // Store the value to the entry
    TMsvEntry tEntry( cEntry->Entry() );
    tEntry.iMtmData2 = aAccountId;
    cEntry->ChangeL( tEntry );

    CleanupStack::PopAndDestroy( cEntry );
    cEntry = NULL;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::GetMailboxAccountIdsL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::GetMailboxAccountIdsL(
    const TMsvId aMailboxId,
    TMsvId& aReceivingMailboxId,
    TUint32& aReceivingAccountId,
    TMsvId& aSendingMailboxId,
    TUint32& aSendingAccountId )
    {
    IMUM_CONTEXT( CImumMboxManager::GetMailboxAccountIdsL, 0, KLogData );
    IMUM_IN();

    MImumInMailboxUtilities::RMsvEntryArray entries;
    iMailboxApi.MailboxUtilitiesL().GetMailboxEntriesL(
        aMailboxId, entries );
    TMsvEntry receiving = entries[0];
    TMsvEntry sending = entries[1];
    entries.Reset();

    if ( receiving.iMtm == KSenduiMtmImap4Uid )
        {
        // Get Imap account id
        TImapAccount imapAccountId;
        iAccounts->GetImapAccountL( receiving.Id(), imapAccountId );

        aReceivingMailboxId = receiving.Id();
        aReceivingAccountId = imapAccountId.iImapAccountId;
        aSendingMailboxId = sending.Id();
        }
    else if ( receiving.iMtm == KSenduiMtmPop3Uid )
        {
        // Get Pop account id
        TPopAccount popAccountId;
        iAccounts->GetPopAccountL( receiving.Id(), popAccountId );

        aReceivingMailboxId = receiving.Id();
        aReceivingAccountId = popAccountId.iPopAccountId;
        aSendingMailboxId = sending.Id();
        }
    else
        {
        // This should not happen ever, but if it does for some reason
        // then leave, as the leave will be trapped and the mailbox
        // shall be removed after it
        User::Leave( KErrUnknown );
        }

    // Get smtp account id
    TSmtpAccount smtpAccountId;
    iAccounts->GetSmtpAccountL( aSendingMailboxId, smtpAccountId );

    aSendingAccountId = smtpAccountId.iSmtpAccountId;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::StoreAccountIdToEntryL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::StoreAccountIdToEntryL(
    CImumMboxData& aSettings )
    {
    IMUM_CONTEXT( CImumMboxManager::StoreAccountIdToEntryL, 0, KLogData );
    IMUM_IN();

    TMsvId sendingMailboxId;
    TMsvId receivingMailboxId;
    TUint32 sendingAccountId;
    TUint32 receivingAccountId;
    GetMailboxAccountIdsL( aSettings.iMailboxId,
        receivingMailboxId, receivingAccountId,
        sendingMailboxId, sendingAccountId );

    StoreAccountIdToEntryL( receivingMailboxId, receivingAccountId );
    StoreAccountIdToEntryL( sendingMailboxId, sendingAccountId );

    IMUM_OUT();
    }

/******************************************************************************

    Account Loading

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxManager::LoadEmailSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::LoadEmailSettingsL(
    const TMsvEntry& aEntry,
    CImumMboxData& aSettings )
    {
    IMUM_CONTEXT( CImumMboxManager::LoadEmailSettingsL, 0, KLogData );
    IMUM_IN();

    // Get both of the entries
    TMsvEntry receiving = iMailboxApi.MailboxUtilitiesL().GetMailboxEntryL(
        aEntry.Id(), MImumInMailboxUtilities::ERequestReceiving );

    // Store the id and protocol
    aSettings.iMailboxId = receiving.iRelatedId;
    aSettings.iIsImap4 = ( receiving.iMtm == KSenduiMtmImap4Uid );

    // Load settings based on the protocol
    if ( aSettings.iIsImap4 )
        {
        iAccounts->GetImapAccountL(
            receiving.Id(), aSettings.iImap4AccountId );
        LoadImap4SettingsL( aSettings );

        // Load extended settings
        TUint32 accountId = aSettings.iImap4AccountId.iImapAccountId;
        iMboxSettingsCtrl->LoadSettings(
            TIMAStorerParams( accountId, receiving.iMtm ),
            *aSettings.iExtendedSettings );

        // Attempt to read the signature
        TRAP_IGNORE( RestoreSignatureL( aSettings ) );
        }
    else // pop3
        {
        iAccounts->GetPopAccountL(
            receiving.Id(), aSettings.iPop3AccountId );
        LoadPop3SettingsL( aSettings );

        // Load extended settings
        TUint32 accountId = aSettings.iPop3AccountId.iPopAccountId;
        iMboxSettingsCtrl->LoadSettings(
            TIMAStorerParams( accountId, receiving.iMtm ),
            *aSettings.iExtendedSettings );

        // Attempt to read the signature
        TRAP_IGNORE( RestoreSignatureL( aSettings ) );
        }

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumMboxManager::LoadEmailSettingsL()
// ---------------------------------------------------------------------------
//
CImumInSettingsData* CImumMboxManager::LoadEmailSettingsL(
    const TMsvId aMailboxId )
    {
    IMUM_CONTEXT( CImumMboxManager::LoadEmailSettingsL, 0, KLogData );
    IMUM_IN();

    CImumMboxData* oldData = CImumMboxData::NewLC();
    TMsvEntry mailbox =
        iMailboxApi.MailboxUtilitiesL().GetMailboxEntryL( aMailboxId );
    LoadEmailSettingsL( mailbox, *oldData );

    CImumInSettingsData* internalData =
        CImumMboxInternalDataConverter::ConvertToInternalMboxDataLC(
            iMailboxApi, *oldData );
    CleanupStack::Pop( internalData );
    CleanupStack::PopAndDestroy( oldData );
    oldData = NULL;

    IMUM_OUT();

    return internalData;
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::LoadPop3SettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::LoadPop3SettingsL(
    CImumMboxData& aSettings )
    {
    IMUM_CONTEXT( CImumMboxManager::LoadPop3SettingsL, 0, KLogData );
    IMUM_IN();

    TSmtpAccount smtpAccountId;
    iAccounts->GetSmtpAccountL(
        aSettings.iPop3AccountId.iSmtpService, smtpAccountId );

    // Load mailbox settings
    iAccounts->LoadPopSettingsL(
        aSettings.iPop3AccountId, *aSettings.iPop3Settings );
    iAccounts->LoadPopIapSettingsL(
        aSettings.iPop3AccountId, *aSettings.iIncomingIapPref );
    iAccounts->LoadSmtpSettingsL(
        smtpAccountId, *aSettings.iSmtpSettings );
    iAccounts->LoadSmtpIapSettingsL(
        smtpAccountId, *aSettings.iOutgoingIapPref );

    LoadAccountFillIapL( aSettings );
    aSettings.iName.Copy( aSettings.iPop3AccountId.iPopAccountName );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::LoadImap4SettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::LoadImap4SettingsL(
    CImumMboxData& aSettings )
    {
    IMUM_CONTEXT( CImumMboxManager::LoadImap4SettingsL, 0, KLogData );
    IMUM_IN();

    TSmtpAccount smtpAccountId;
    iAccounts->GetSmtpAccountL(
        aSettings.iImap4AccountId.iSmtpService, smtpAccountId );

    // Load mailbox settings
    iAccounts->LoadImapSettingsL(
        aSettings.iImap4AccountId, *aSettings.iImap4Settings );
    iAccounts->LoadImapIapSettingsL(
        aSettings.iImap4AccountId, *aSettings.iIncomingIapPref );
    iAccounts->LoadSmtpSettingsL(
        smtpAccountId, *aSettings.iSmtpSettings );
    iAccounts->LoadSmtpIapSettingsL(
        smtpAccountId, *aSettings.iOutgoingIapPref );

    LoadAccountFillIapL( aSettings );
    aSettings.iName.Copy( aSettings.iImap4AccountId.iImapAccountName );

    IMUM_OUT();
    }

/******************************************************************************

    MISC TOOLS

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxManager::StoreSignatureL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::StoreSignatureL(
    const CImumMboxData& aSettings )
    {
    IMUM_CONTEXT( CImumMboxManager::StoreSignatureL, 0, KLogData );
    IMUM_IN();

    // Get Signature
    CMsvEntry* entry = iMailboxApi.MsvSession().GetEntryL( aSettings.iMailboxId );
    CleanupStack::PushL( entry );
    CMsvStore* msvStore = entry->EditStoreL();
    CleanupStack::PushL( msvStore );

    msvStore->StoreBodyTextL( *( aSettings.iSignature->iRichText ) );
    msvStore->CommitL();

    CleanupStack::PopAndDestroy( msvStore );
    msvStore = NULL;
    CleanupStack::PopAndDestroy( entry );
    entry = NULL;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::RestoreSignatureL()
// ----------------------------------------------------------------------------
//
void CImumMboxManager::RestoreSignatureL(
    CImumMboxData& aSettings )
    {
    IMUM_CONTEXT( CImumMboxManager::RestoreSignatureL, 0, KLogData );
    IMUM_IN();

    // Get Signature
    CMsvEntry* entry = iMailboxApi.MsvSession().GetEntryL( aSettings.iMailboxId );
    CleanupStack::PushL( entry );
    CMsvStore* msvStore = entry->ReadStoreL();
    CleanupStack::PushL( msvStore );

    aSettings.iSignature->CreateEmptyRichTextL();
    msvStore->RestoreBodyTextL( *( aSettings.iSignature->iRichText ) );

    CleanupStack::PopAndDestroy( msvStore );
    msvStore = NULL;
    CleanupStack::PopAndDestroy( entry );
    entry = NULL;

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumMboxManager::IsConnectedL()
// ---------------------------------------------------------------------------
//
TBool CImumMboxManager::IsConnectedL( const TMsvId aMailboxId ) const
    {
    IMUM_CONTEXT( CImumMboxManager::IsConnectedL, 0, KLogData );
    IMUM_IN();

    TBool connected = EFalse;

    // Make sure that none of the entries is in connected state
    MImumInMailboxUtilities::RMsvEntryArray entries;
    iMailboxApi.MailboxUtilitiesL().GetMailboxEntriesL(
        aMailboxId, entries );
    CleanupClosePushL( entries );

    for ( TInt mbox = entries.Count(); --mbox >= 0 && !connected; )
        {
        connected = entries[mbox].Connected();
        }

    CleanupStack::PopAndDestroy( &entries );

    IMUM_OUT();

    return connected;
    }

// ---------------------------------------------------------------------------
// CImumMboxManager::IsLockedL()
// ---------------------------------------------------------------------------
//
TBool CImumMboxManager::IsLockedL( const TMsvId aMailboxId ) const
    {
    IMUM_CONTEXT( CImumMboxManager::IsLockedL, 0, KLogData );
    IMUM_IN();

    TBool isLocked = EFalse;
    CMsvEntry* centry =
        iMailboxApi.MsvSession().GetEntryL( aMailboxId );
    CleanupStack::PushL( centry );

    // If the entry is locked, deletion should not be possible
    TRAPD( err, centry->ChangeL( centry->Entry() ) );
    if( err != KErrNone )
        {
        isLocked = ETrue;
        }

    CleanupStack::PopAndDestroy( centry );
    centry = NULL;

    IMUM_OUT();

    return isLocked;
    }

// ----------------------------------------------------------------------------
// CImumMboxManager::HandleMailboxInUseDeleteL()
// ----------------------------------------------------------------------------
//
TBool CImumMboxManager::HandleMailboxInUseDeleteL(
    MImumInHealthServices::RMailboxIdArray& aMailboxes,
    const TMsvEntry& aSmtpEntry ) const
    {
    IMUM_CONTEXT( CImumMboxManager::HandleMailboxInUseDeleteL, 0, KLogData );
    IMUM_IN();
    TBool continueDeletion(ETrue);
    TMsvId newDefaultId = 0;
    TInt mailboxCount = aMailboxes.Count();
    if ( mailboxCount == 2 ) // CSI: 47 # Mailbox count.
        {
        // set default to be the other one
        TInt newDefaultIndex =
            ( aSmtpEntry.Id() == aMailboxes[0] ) ? 1 : 0;
        newDefaultId = aMailboxes[newDefaultIndex];
        }
    else
        {
        // To show mailbox name list, the array must be constructed here
        CDesCArrayFlat* items =
            new ( ELeave ) CDesCArrayFlat( KImumMboxArrayGranularity );
        CleanupStack::PushL( items );

        TMsvEntry tempEntry;
        TInt delIndex=-1;
        for (TInt loop = 0; loop < mailboxCount; loop++)
            {
            TMsvId tempServiceId;
            User::LeaveIfError( iMailboxApi.MsvSession().GetEntry(
                aMailboxes[loop],
                tempServiceId,
                tempEntry ) );
            if ( aSmtpEntry.Id() != tempEntry.Id() )
                {
                items->AppendL( tempEntry.iDetails );
                }
            else
                {
                delIndex = loop;
                }
            }
        if ( delIndex>=0 )
            {
            //take the mailbox to be deleted off from the array
            aMailboxes.Remove(delIndex);
            }

        TInt index = 0;
        CAknListQueryDialog* dlg = new (ELeave) CAknListQueryDialog(&index);
        dlg->PrepareLC( R_IMUM_NEW_DEFAULT_MAILBOX_LIST_QUERY );
        dlg->SetItemTextArray( items );
        dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );
        if ( dlg->RunLD( ) == EEikBidOk )
            {
            newDefaultId = aMailboxes[index];
            }
        else
            {
            continueDeletion = EFalse;
            }

        CleanupStack::PopAndDestroy( items );  // items
        items = NULL;
        }

    if ( newDefaultId )
        {
        // Set default
        MsvUiServiceUtilitiesInternal::SetDefaultServiceForMTML(
                iMailboxApi.MsvSession(),
                KSenduiMtmSmtpUid,
                newDefaultId );
        }

    IMUM_OUT();
    return continueDeletion;
    }

// ---------------------------------------------------------------------------
// CImumMboxManager::AccountDeletionConditionsOkL()
// ---------------------------------------------------------------------------
//
TBool CImumMboxManager::AccountDeletionConditionsOkL(
    const TMsvId aMailboxId )
    {
    IMUM_CONTEXT( CImumMboxManager::AccountDeletionConditionsOkL, 0, KLogData );
    IMUM_IN();

    TBool conditionsOk = EFalse;

    // Entry is required to get the name of the mailbox Id
    TMsvEntry mailbox = iMailboxApi.MailboxUtilitiesL().GetMailboxEntryL(
        aMailboxId, MImumInMailboxUtilities::ERequestSending );
    TBuf<KImumMboxQueryTextLength> name;
    name.Copy( mailbox.iDetails.Left( KImumMboxQueryTextLength ) );
    TUid mtm = KSenduiMtmSmtpUid;

    TMsvId defaultService = MsvUiServiceUtilitiesInternal::
            DefaultServiceForMTML(
                iMailboxApi.MsvSession(), mtm, EFalse );

    // Get the list of available mailboxes
    MImumInHealthServices::RMailboxIdArray mailboxes;
    iMailboxApi.HealthServicesL().GetMailboxList( mailboxes, 
        MImumInHealthServices::EFlagGetHealthy | 
        MImumInHealthServices::EFlagIncludeSmtp );
    CleanupClosePushL( mailboxes );

    // Don't allow locked or connected mailboxes to be deleted
    if ( IsConnectedL( aMailboxId ) ||
         IsLockedL( mailbox.Id() ) )
        {
        CIMSSettingsNoteUi::ShowNoteL(
            R_IMUM_SETTINGS_MAIL_NO_DELETE, EIMSInformationNote, ETrue );
        }
    //
    else if ( mailboxes.Count() == 1 )
        {
        conditionsOk =
            CIMSSettingsNoteUi::ShowQueryL<KImumMboxQueryTextLength>(
                R_IMUM_SETTINGS_MAIL_QUEST_DEL,
                mailbox.iDetails );

        }
    // If the current mailbox is not default, it can be deleted
    // immediately.
    else if ( mailbox.Id() != defaultService )
        {
        conditionsOk =
            CIMSSettingsNoteUi::ShowQueryL<KImumMboxQueryTextLength>(
                R_IMUM_COMMON_CONF_DELETE,
                mailbox.iDetails );
        }
    else
        {
        conditionsOk =
            CIMSSettingsNoteUi::ShowQueryL<KImumMboxQueryTextLength>(
                R_IMUM_SETTINGS_MAIL_QUEST_D,
                mailbox.iDetails );

        if ( conditionsOk )
            {
            conditionsOk = HandleMailboxInUseDeleteL( mailboxes, mailbox );
            }
        }

    CleanupStack::PopAndDestroy( &mailboxes );

    IMUM_OUT();
    return conditionsOk;
    }


// End Of File

