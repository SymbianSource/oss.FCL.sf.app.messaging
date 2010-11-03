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
* Description:  ImumInHealthServicesImpl.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <MuiuMsvUiServiceUtilities.h>  // MsvUiServiceUtilities
#include <cemailaccounts.h>             // CEmailAccounts
#include <etelmm.h>
#include <SendUiConsts.h>

#include "ImumInternalApiImpl.h"            // CImumInternalApiImpl
#include "ImumMboxManager.h"
#include "EmailUtils.H"
#include "ImumInHealthServicesImpl.h"
#include "ImumInMailboxUtilitiesImpl.h"
#include "ImumUtilsLogging.h"



// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
#ifdef __WINS__
_LIT( KIMAEmulatorImei, "123456789012345" );
#endif // __WINS__

// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::CImumInHealthServicesImpl()
// ----------------------------------------------------------------------------
//
CImumInHealthServicesImpl::CImumInHealthServicesImpl(
    CImumInternalApiImpl& aMailboxApi )
    :
    iInternalApi( aMailboxApi ),
    iExtendedSettings( NULL ),
    iRoot( NULL ),
    iUtils( NULL )
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::CImumInHealthServicesImpl, 0, KLogInApi );
    IMUM_IN();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::~CImumInHealthServicesImpl()
// ----------------------------------------------------------------------------
//
CImumInHealthServicesImpl::~CImumInHealthServicesImpl()
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::~CImumInHealthServicesImpl, 0, KLogInApi );
    IMUM_IN();

    delete iExtendedSettings;
    iExtendedSettings = NULL;
    delete iRoot;
    iRoot = NULL;
    iUtils = NULL;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumInHealthServicesImpl::ConstructL()
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::ConstructL, 0, KLogInApi );
    IMUM_IN();

    iExtendedSettings = CImumMboxDataExtension::NewL();
    GetIMEIFromThePhoneL();
    iRoot = iInternalApi.MsvSession().GetEntryL( KMsvRootIndexEntryIdValue );
    iRoot->SetSortTypeL( TMsvSelectionOrdering(
        KMsvGroupByType | KMsvGroupByStandardFolders,
        EMsvSortByDetailsReverse, ETrue ) );
    iUtils = const_cast<MImumInMailboxUtilities*>(
        &iInternalApi.MailboxUtilitiesL() );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::NewL()
// ----------------------------------------------------------------------------
//
CImumInHealthServicesImpl* CImumInHealthServicesImpl::NewL(
    CImumInternalApiImpl& aMailboxApi )
    {
    IMUM_STATIC_CONTEXT( CImumInHealthServicesImpl::NewL, 0, utils, KLogInApi );
    IMUM_IN();

    CImumInHealthServicesImpl* self = NewLC( aMailboxApi );
    CleanupStack::Pop( self );

    IMUM_OUT();

    return self;
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::NewLC()
// ----------------------------------------------------------------------------
//
CImumInHealthServicesImpl* CImumInHealthServicesImpl::NewLC(
    CImumInternalApiImpl& aMailboxApi )
    {
    IMUM_STATIC_CONTEXT( CImumInHealthServicesImpl::NewLC, 0, utils, KLogInApi );
    IMUM_IN();

    CImumInHealthServicesImpl* self = new ( ELeave ) CImumInHealthServicesImpl( aMailboxApi );

    CleanupStack::PushL( self );
    self->ConstructL();

    IMUM_OUT();

    return self;
    }

/******************************************************************************

    Cleanup operations

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::GetIMEIFromTheEntry()
// ----------------------------------------------------------------------------
//
TIMDErrors CImumInHealthServicesImpl::GetIMEIFromTheEntry(
    const TMsvId aEntryId,
    TDes& aIMEI ) const
    {
    TMsvEntry mailbox;
    TIMDErrors result = GetMailboxEntry( aEntryId, mailbox );

    if ( result == EIMDNoError )
        {
        aIMEI.Copy( mailbox.iDescription.Left( KIMAMaxPhoneIdLength ) );
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::GetIMEIFromTheEntry()
// ----------------------------------------------------------------------------
//
TIMDErrors CImumInHealthServicesImpl::GetIMEIFromTheEntry(
    const TMsvEntry& aEntry,
    TDes& aIMEI ) const
    {
    // IMEI is stored into description field
    aIMEI.Copy( aEntry.iDescription.Left( KIMAMaxPhoneIdLength ) );

    return EIMDNoError;
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::GetIMEIFromThePhone()
// ----------------------------------------------------------------------------
//
void CImumInHealthServicesImpl::GetIMEIFromThePhoneL()
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::GetIMEIFromThePhoneL, 0, KLogInApi );
    IMUM_IN();

    // In Emulator version, the imei cannot be fetched, so the basic number
    // is used instead
#ifndef __WINS__

    RTelServer telServer;
    User::LeaveIfError( telServer.Connect() );
    CleanupClosePushL(telServer);

    TInt numPhones = 0;
    User::LeaveIfError( telServer.EnumeratePhones( numPhones ) );
    if ( numPhones < 1 )
        {
        User::Leave( KErrNotFound );
        }
    RTelServer::TPhoneInfo info;
    User::LeaveIfError( telServer.GetPhoneInfo( 0, info ) );
    RMobilePhone mobilePhone;
    User::LeaveIfError( mobilePhone.Open( telServer, info.iName ) );
    CleanupClosePushL( mobilePhone );

    //mobilePhone.Initialise();
    TUint32 identityCaps;
    User::LeaveIfError( mobilePhone.GetIdentityCaps( identityCaps ) );

    if ( identityCaps & RMobilePhone::KCapsGetSerialNumber )
        {
        TRequestStatus status;
        RMobilePhone::TMobilePhoneIdentityV1 mobilePhoneIdentity;

        mobilePhone.GetPhoneId( status, mobilePhoneIdentity );

        User::WaitForRequest( status );
        User::LeaveIfError( status.Int() );

        iIMEI.Copy( mobilePhoneIdentity.iSerialNumber );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }

    CleanupStack::PopAndDestroy( &mobilePhone );
    CleanupStack::PopAndDestroy( &telServer );
#else
    iIMEI.Copy( KIMAEmulatorImei );
#endif // __WINS__

    IMUM1(0, "Phones IMEI code: %S", &iIMEI);

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::StoreIMEIToMailbox()
// ----------------------------------------------------------------------------
//
TIMDErrors CImumInHealthServicesImpl::StoreIMEIToMailbox(
    const TMsvId aMailboxId ) const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::StoreIMEIToMailbox, 0, KLogInApi );
    IMUM1_IN( "aMailboxId = 0x%x", aMailboxId );

    // Get available mailbox entries
    TMsvEntry entry;
    TIMDErrors result = GetMailboxEntry( aMailboxId, entry );

    // Imap4 & Pop3 & Smtp protocols contain two entries
    if ( result == EIMDNoError )
        {
        // Attempt to store the IMEI code to the entry
        result = StoreIMEIToEntry( entry.Id() );
        }

    // Imap4 / Pop3 protocols have two different entries
    if ( iUtils->IsMailMtm( entry.iMtm ) )
        {
        TMsvEntry secondaryEntry;
        result = GetMailboxEntry( entry.iRelatedId, entry );

        if ( result == EIMDNoError )
            {
            // Attempt to store the IMEI code to the entry
            result = StoreIMEIToEntry( entry.Id() );
            }
        }

    IMUM_OUT();

    return result;
    }


// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::StoreIMEIToEntry()
// ----------------------------------------------------------------------------
//
TIMDErrors CImumInHealthServicesImpl::StoreIMEIToEntry(
    const TMsvId aMailboxId ) const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::StoreIMEIToEntry, 0, KLogInApi );
    IMUM1_IN( "aMailboxId = 0x%x", aMailboxId );

    TRAPD( error, StoreIMEIToEntryL( aMailboxId ) );

    IMUM_OUT();
    return ( error == KErrNone ) ? EIMDNoError : EIMDStoringFailureWithIMEI;
    }


// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::StoreIMEIToEntryL()
// ----------------------------------------------------------------------------
//
void CImumInHealthServicesImpl::StoreIMEIToEntryL(
    const TMsvId aMailboxId ) const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::StoreIMEIToEntryL, 0, KLogInApi );
    IMUM1_IN( "aMailboxId = 0x%x", aMailboxId );

    CMsvEntry* centry = iInternalApi.MsvSession().GetEntryL( aMailboxId );
    CleanupStack::PushL( centry );

    TMsvEntry tentry = centry->Entry();
    tentry.iDescription.Set( iIMEI );
    centry->ChangeL( tentry );

    CleanupStack::PopAndDestroy( centry );
    centry = NULL;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::GetMailboxEntry()
// ----------------------------------------------------------------------------
//
TIMDErrors CImumInHealthServicesImpl::GetMailboxEntry(
    const TMsvId aMailboxId,
    TMsvEntry& aMailboxEntry ) const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::GetMailboxEntry, 0, KLogInApi );
    IMUM1_IN( "aMailboxId = 0x%x", aMailboxId );

    // Get both receiving and sending entries of the mailbox
    TIMDErrors result = EIMDNoError;

    TMsvId service;
    TInt error = iInternalApi.MsvSession().GetEntry(
        aMailboxId, service, aMailboxEntry );

    // Make sure getting of the entry was ok
    if ( error == KErrNone )
        {
        // Verify that the entry has mail mtm
        if ( !iUtils->IsMailMtm( aMailboxEntry.iMtm, ETrue ) )
            {
            result = EIMDNotMailMtmId;
            }
        // Mailbox, log the details
        else
            {
            IMUM0( 0, "************ Entry Data START ******************" );
            IMUM1( 0, "MailboxId    = 0x%x", aMailboxEntry.Id() );
            IMUM1( 0, "RelatedId    = 0x%x", aMailboxEntry.iRelatedId );
            IMUM1( 0, "MTM type     = 0x%x", aMailboxEntry.iMtm.iUid );
            IMUM1( 0, "Visible      = %d", aMailboxEntry.Visible() );
            IMUM1( 0, "iMtmData2    = 0x%x", aMailboxEntry.iMtmData2 );
            IMUM1( 0, "iDescription = %S", &aMailboxEntry.iDescription );
            IMUM1( 0, "iDetails     = %S", &aMailboxEntry.iDetails );
            IMUM0( 0, "************* Entry Data END *******************" );

         	//Make sure that all settings are ok
            if ( aMailboxEntry.iDetails.Length() > KIMASMaxMailboxNameLength  )
                {
            	if ( aMailboxEntry.iMtm == KUidMsgTypeIMAP4 )
            		{
            		result = EIMDGetEntryFailure;
            		TRAP_IGNORE( result = iInternalApi.SettingsStorerL().CheckMailboxNameImap( aMailboxEntry ) );
            		// Should SettingsStorerL leave, result == EIMDGetEntryFailure
            		}
            	else if ( aMailboxEntry.iMtm == KUidMsgTypePOP3 )
            		{
            		result = EIMDGetEntryFailure;
            		TRAP_IGNORE( result = iInternalApi.SettingsStorerL().CheckMailboxNamePop( aMailboxEntry ) );
            		// Should SettingsStorerL leave, result == EIMDGetEntryFailure
            		}
            	else if ( aMailboxEntry.iMtm == KUidMsgTypeSMTP )
            		{
            		result = EIMDGetEntryFailure;
            		TRAP_IGNORE( result = iInternalApi.SettingsStorerL().CheckMailboxNameSmtp( aMailboxEntry ) );
            		// Should SettingsStorerL leave, result == EIMDGetEntryFailure
            		}
            	else
            		{
        			result = EIMDGetEntryFailure;
            		}
                }
            }
        }
    else
        {
        result = EIMDGetEntryFailure;
        }

    IMUM1( 0, "Result of getting the entry: %d", result );
    IMUM_OUT();
    return result;
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::VerifyMailboxHealth()
// ----------------------------------------------------------------------------
//
TIMDErrors CImumInHealthServicesImpl::VerifyMailboxHealth(
    const TMsvId aMailboxId ) const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::VerifyMailboxHealth, 0, KLogInApi );
    IMUM1_IN( "aMailboxId = 0x%x", aMailboxId );

    TMsvEntry primaryEntry;
    TMsvEntry secondaryEntry;
    TMsvEntry* secondaryEntryPtr = NULL;
    TIMDErrors verification = GetMailboxEntry( aMailboxId, primaryEntry );

    // Imap4 & Pop3 & Smtp protocols contain two entries
    if ( verification == EIMDNoError &&
         iUtils->IsMailMtm( primaryEntry.iMtm ) )
        {
        verification = GetMailboxEntry(
            primaryEntry.iRelatedId, secondaryEntry );

        if ( verification == EIMDNoError )
            {
            secondaryEntryPtr = &secondaryEntry;
            }
        }

    // Make sure
    if ( verification == EIMDNoError )
        {
        verification = VerifyMailboxHealth( primaryEntry, secondaryEntryPtr );
        }

    IMUM_OUT();
    return verification;
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::VerifyMailboxHealth()
// ----------------------------------------------------------------------------
//
TIMDErrors CImumInHealthServicesImpl::VerifyMailboxHealth(
    const TMsvEntry& aPrimaryEntry,
    const TMsvEntry* aSecondaryEntry ) const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::VerifyMailboxHealth, 0, KLogInApi );
    IMUM_IN();

    // Verify that SMTP part of the mailbox has got the settings
    // Verify that SMTP account id's matches with the settings
    // Verify that SMTP mailbox id's matches with the settings
    TIMDErrors verification = MatchEntryWithSettings( aPrimaryEntry );
    IMUM1( 0, "Mailbox primary entry IMEI settings matching result: %d", verification );

    // Verify that related part of the mailbox has got the settings
    // Verify that related account id's matches with the settings
    // Verify that related mailbox id's matches with the settings
    if ( !verification && aSecondaryEntry )
        {
        verification = MatchEntryWithSettings( *aSecondaryEntry );
        IMUM1( 0, "Mailbox secondary entry IMEI settings matching result: %d", verification );
        }

    // Verify that primary entry has IMEI code stored
    if ( !verification )
        {
        verification = VerifyEntryIMEI( aPrimaryEntry );
        IMUM1( 0, "Mailbox primary entry IMEI verification result: %d", verification );
        if( verification == EIMDMissingIMEI )
            {
            // Mailbox has no IMEI code stored. This Mailbox is most likely created
            // with CEmailAccounts. Lock the account to current Phone.
            verification = StoreIMEIToMailbox( aPrimaryEntry.Id() );
            IMUM1( 0, "IMEI missing. Lock to current device: %d", verification );
            }
        }

    // Verify that related entry has IMEI code stored
    if ( !verification && aSecondaryEntry )
        {
        verification = VerifyEntryIMEI( *aSecondaryEntry );
        IMUM1( 0, "Mailbox secondary entry IMEI verification result: %d", verification );
        if( verification == EIMDMissingIMEI )
            {
            // Mailbox has no IMEI code stored. This Mailbox is most likely created
            // with CEmailAccounts. Lock the account to current Phone.
            verification = StoreIMEIToMailbox( aSecondaryEntry->Id() );
            IMUM1( 0, "IMEI missing. Lock to current device: %d", verification );
            }
        }

    IMUM_OUT();

    return verification;
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::MatchEntryWithSettings()
// ----------------------------------------------------------------------------
//
TIMDErrors CImumInHealthServicesImpl::MatchEntryWithSettings(
    const TMsvEntry& aMailbox ) const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::MatchEntryWithSettings, 0, KLogInApi );
    IMUM1_IN( "aMailbox.Id = 0x%x", aMailbox.Id() );
    IMUM_OUT();

    switch ( aMailbox.iMtm.iUid )
        {
        case KSenduiMtmImap4UidValue:
            return VerifyImap4EntryWithSettings( aMailbox );

        case KSenduiMtmPop3UidValue:
            return VerifyPop3EntryWithSettings( aMailbox );

        case KSenduiMtmSmtpUidValue:
            return VerifySmtpEntryWithSettings( aMailbox );

        default:
        	// 3rd party mailbox
        	// MTM UID is unknown -> check technology type and return
        	// EIMDNoError if it is mailbox
        	if( iUtils->IsMailbox( aMailbox.Id() ) )
        		{
        		return EIMDNoError;
        		}
            break;
        }

    return EIMDUnknownMailbox;
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::VerifyImap4EntryWithSettings()
// ----------------------------------------------------------------------------
//
TIMDErrors CImumInHealthServicesImpl::VerifyImap4EntryWithSettings(
    const TMsvEntry& aMailbox ) const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::VerifyImap4EntryWithSettings, 0, KLogInApi );
    IMUM1_IN( "aMailbox.Id = 0x%x", aMailbox.Id() );

    // Get IMAP4 account information from the repository
    TIMDErrors verification = EIMDNoError;
    TImapAccount imapAccount;
    TRAPD( error, iInternalApi.EmailAccountsL().GetImapAccountL(
        aMailbox.Id(), imapAccount ) );

    // Continue only, if successful
    if ( error == KErrNone )
        {
        // Verify that the entry's account id matches with repository
        if ( aMailbox.iMtmData2 != imapAccount.iImapAccountId )
            {
            verification = EIMDImap4AccountIdNotMatching;
            }

        // Verify that the related id matches with the one in the repository
        if ( verification == EIMDNoError &&
             aMailbox.iRelatedId != imapAccount.iSmtpService )
            {
            verification = EIMDImap4RelatedEntryIdNotMatching;
            }
        }
    else
        {
        verification = EIMDImap4AccountFailure;
        }

    IMUM_OUT();
    return verification;
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::VerifyPop3EntryWithSettings()
// ----------------------------------------------------------------------------
//
TIMDErrors CImumInHealthServicesImpl::VerifyPop3EntryWithSettings(
    const TMsvEntry& aMailbox ) const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::VerifyPop3EntryWithSettings, 0, KLogInApi );
    IMUM1_IN( "aMailbox.Id = 0x%x", aMailbox.Id() );

    // Get POP3 account information from the repository
    TIMDErrors verification = EIMDNoError;
    TPopAccount popAccount;
    TRAPD( error, iInternalApi.EmailAccountsL().GetPopAccountL(
        aMailbox.Id(), popAccount ) );

    // Continue only, if successful
    if ( error == KErrNone )
        {
        // Verify that the entry's account id matches with repository
        if ( aMailbox.iMtmData2 != popAccount.iPopAccountId )
            {
            verification = EIMDPop3AccountIdNotMatching;
            }

        // Verify that the related id matches with the one in the repository
        if ( verification == EIMDNoError &&
             aMailbox.iRelatedId != popAccount.iSmtpService )
            {
            verification = EIMDPop3RelatedEntryIdNotMatching;
            }
        }
    else
        {
        verification = EIMDPop3AccountFailure;
        }

    IMUM_OUT();
    return verification;
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::VerifySmtpEntryWithSettings()
// ----------------------------------------------------------------------------
//
TIMDErrors CImumInHealthServicesImpl::VerifySmtpEntryWithSettings(
    const TMsvEntry& aMailbox ) const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::VerifySmtpEntryWithSettings, 0, KLogInApi );
    IMUM1_IN( "aMailbox.Id = 0x%x", aMailbox.Id() );

    // Get SMTP account information from the repository
    TIMDErrors verification = EIMDNoError;
    TSmtpAccount smtpAccount;
    TRAPD( error, iInternalApi.EmailAccountsL().GetSmtpAccountL(
        aMailbox.Id(), smtpAccount ) );

    // Continue only, if successful
    if ( error == KErrNone )
        {
        // Verify that the entry's account id matches with repository
        if ( aMailbox.iMtmData2 != smtpAccount.iSmtpAccountId )
            {
            verification = EIMDSmtpAccountIdNotMatching;
            }

        // Verify that the related id matches with the one in the repository
        if ( verification == EIMDNoError &&
             aMailbox.iRelatedId != smtpAccount.iRelatedService )
            {
            verification = EIMDSmtpRelatedEntryIdNotMatching;
            }
        }
    else
        {
        verification = EIMDSmtpAccountFailure;
        }

    IMUM_OUT();
    return verification;
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::VerifyEntryIMEI()
// ----------------------------------------------------------------------------
//
TIMDErrors CImumInHealthServicesImpl::VerifyEntryIMEI(
    const TMsvEntry& aMailbox ) const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::VerifyEntryIMEI, 0, KLogInApi );
    IMUM1_IN( "aMailbox.Id = 0x%x", aMailbox.Id() );

    TIMDErrors result = EIMDNoError;

    // In emulator, always return true for the IMEI verification,
    // so that mismatching IMEI codes doesn't prevent debugging

    // Make sure the field contains at least something
    if ( aMailbox.iDescription.Length() )
        {
        if ( iIMEI.Match( aMailbox.iDescription ) )
            {
            TDriveUnit driveUnit = EDriveC;
            TRAPD( err, driveUnit =
                iInternalApi.MsvSession().CurrentDriveL() );

            // When memory card restored to another phone emailbox should be visible
            if ( err == KErrNone && driveUnit == EDriveC )
            	{
            	result = StoreIMEIToMailbox( aMailbox.Id() );
            	}
            else
            	{
            	result = EIMDMismatchingIMEI;
            	}
            }
        }
    else
        {
        result = EIMDMissingIMEI;
        }

    IMUM_OUT();
    return result;
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::IsRequiredMailbox()
// ----------------------------------------------------------------------------
//
TBool CImumInHealthServicesImpl::IsRequiredMailbox(
    const TMsvEntry& aEntry,
    const TMuiuFlags& aFlags ) const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::IsRequiredMailbox, 0, KLogInApi );
    IMUM1_IN( "aEntry.Id() = 0x%x", aEntry.Id() );

    IMUM1( 0, "ServiceEntry: %d", aEntry.iType.iUid == KUidMsvServiceEntryValue );
    IMUM1( 0, "Entry ID not Local: %d", aEntry.Id() != KMsvLocalServiceIndexEntryIdValue );

    // Accept only services.
    TBool result = ( aEntry.iType.iUid == KUidMsvServiceEntryValue ) &&
                   ( aEntry.Id() != KMsvLocalServiceIndexEntryIdValue );

    IMUM1( 0, "Result after service test: %d", result );

    if ( result )
        {
        IMUM1( 0, "Get healthy mailboxes: %d", !aFlags.Flag( EFlagHealthy ) );
        IMUM1( 0, "Entry is visible: %d", aEntry.Visible() );
        IMUM1( 0, "Entry is SMTP: %d", aEntry.iMtm == KSenduiMtmSmtpUid );

        // For valid mailboxes, only visible entries are allowed. For cleanup,
        // all entries must be allowed.
        result = !aFlags.Flag( EFlagHealthy ) | aEntry.Visible();

        // Smtp services are invisible, so the result must be acceptable
        // for them.
        result |= !aEntry.Visible() && aEntry.iMtm == KSenduiMtmSmtpUid;

        IMUM1( 0, "Result after entry test: %d", result );
        }

    if ( result )
        {
        // Entry is valid service entry. Now, check if the mailbox is...

        // ...Imap4 and it is required...
        IMUM2( 0, "IMAP4 required: %d AND entry is IMAP4 %d",
            aFlags.Flag( EFlagImap4 ),
            aEntry.iMtm == KSenduiMtmImap4Uid );

        result =
            aFlags.Flag( EFlagImap4 ) &&
            aEntry.iMtm == KSenduiMtmImap4Uid;

        // ...or Pop3 and it is required...
        IMUM2( 0, "POP3 required: %d AND entry is POP3 %d",
            aFlags.Flag( EFlagPop3 ),
            aEntry.iMtm == KSenduiMtmPop3Uid );

        result |=
            aFlags.Flag( EFlagPop3 ) &&
            aEntry.iMtm == KSenduiMtmPop3Uid;

        // ...or Smtp and it is required...
        IMUM2( 0, "SMTP required: %d AND entry is SMTP %d",
            aFlags.Flag( EFlagSmtp ),
            aEntry.iMtm == KSenduiMtmSmtpUid );

        result |=
            aFlags.Flag( EFlagSmtp ) &&
            aEntry.iMtm == KSenduiMtmSmtpUid;

        // ...or other MTM and it is required
        IMUM2( 0, "Other required: %d AND entry is other %d",
            aFlags.Flag( EFlagOtherService ),
            aEntry.iMtm != KSenduiMtmSmtpUid &&
            aEntry.iMtm != KSenduiMtmPop3Uid &&
            aEntry.iMtm != KSenduiMtmImap4Uid );

        result |=
            aFlags.Flag( EFlagOtherService ) &&
            aEntry.iMtm != KSenduiMtmSmtpUid &&
            aEntry.iMtm != KSenduiMtmPop3Uid &&
            aEntry.iMtm != KSenduiMtmImap4Uid;

        IMUM1( 0, "Result of service test: %d", result );
        }

    IMUM_OUT();
    return result;
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::DoGetMailboxList()
// ----------------------------------------------------------------------------
//
void CImumInHealthServicesImpl::DoGetMailboxList(
    RMailboxIdArray& aMailboxArray,
    const TMuiuFlags& aFlags ) const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::DoGetMailboxList, 0, KLogInApi );
    IMUM_IN();

    TBool getHealthy = aFlags.Flag( EFlagHealthy );

    for ( TInt item = iRoot->Count(); --item >= 0; )
        {
        // Check if the item is mailbox flagged to be retrieved
        const TMsvEntry& candidate = ( *iRoot )[item];
        if ( IsRequiredMailbox( candidate, aFlags ) )
            {
            // Append to array, if healthy and healthy required OR
            // unhealthy and unhealthy required
            if ( IsMailboxHealthy( candidate.Id() ) == getHealthy )
                {
                IMUM1( 0, "Adding candidate 0x%x to list", candidate.Id() );
                aMailboxArray.Append( candidate.Id() );
                }
            }
        }

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::DoCleanup()
// ----------------------------------------------------------------------------
//
void CImumInHealthServicesImpl::DoCleanup(
    const RMailboxIdArray& aMailboxArray ) const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::DoCleanup, 0, KLogInApi );
    IMUM_IN();

    for ( TInt mailbox = aMailboxArray.Count(); --mailbox >= 0; )
        {
        TMsvId serviceId;
        TMsvEntry entry;
        TInt error = iInternalApi.MsvSession().GetEntry(
            aMailboxArray[mailbox], serviceId, entry );

        // We detect only missing settings, so remove the entries only if
        // created on this phone
        if ( error == KErrNone && VerifyEntryIMEI( entry ) == EIMDNoError )
            {

            iInternalApi.MsvSession().RemoveEntry( entry.Id() );
            iInternalApi.MsvSession().RemoveEntry( entry.iRelatedId );
            }
        }

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::PrepareFlags()
// ----------------------------------------------------------------------------
//
TMuiuFlags CImumInHealthServicesImpl::PrepareFlags(
    const TInt64 aFlags ) const
    {
    TMuiuFlags flags = aFlags;

    return flags;
    }

/******************************************************************************

    Functions from the base classes

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::GetMailboxList()
// ----------------------------------------------------------------------------
//
TInt CImumInHealthServicesImpl::GetMailboxList(
    RMailboxIdArray& aHealthyMailboxArray,
    const TInt64 aFlags,
    const TBool aClearArray ) const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::GetMailboxList, 0, KLogInApi );
    IMUM2_IN( "aFlags = 0x%x, aClearArray = %d", aFlags, aClearArray );

    TInt error = KErrNone;

    if ( aClearArray )
        {
        aHealthyMailboxArray.Reset();
        }

    DoGetMailboxList( aHealthyMailboxArray, PrepareFlags( aFlags ) );

    IMUM_OUT();

    return error;
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::GetHealthyMailboxList()
// ----------------------------------------------------------------------------
//
TInt CImumInHealthServicesImpl::GetHealthyMailboxList(
    RMailboxIdArray& aHealthyMailboxArray,
    const TInt64 aFlags,
    const TBool aClearArray ) const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::GetHealthyMailboxList, 0, KLogInApi );
    IMUM_IN();

    IMUM_OUT();
    return GetMailboxList( aHealthyMailboxArray, aFlags, aClearArray );
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::IsMailboxHealthy()
// ----------------------------------------------------------------------------
//
TBool CImumInHealthServicesImpl::IsMailboxHealthy(
    const TMsvId aMailboxId ) const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::IsMailboxHealthy, 0, KLogInApi );
    IMUM1_IN( "aMailboxId = 0x%x", aMailboxId );

    TIMDErrors verification = VerifyMailboxHealth( aMailboxId );

    IMUM_OUT();
    return !verification;
    }

// ----------------------------------------------------------------------------
// CImumInHealthServicesImpl::CleanupUnhealthyMailboxes()
// ----------------------------------------------------------------------------
//
void CImumInHealthServicesImpl::CleanupUnhealthyMailboxes() const
    {
    IMUM_CONTEXT( CImumInHealthServicesImpl::CleanupUnhealthyMailboxes, 0, KLogInApi );
    IMUM_IN();

    RMailboxIdArray unhealthyMailboxes;
    TMuiuFlags flags;

	flags.SetFlag( EFlagImap4 );
	flags.SetFlag( EFlagPop3 );

    DoGetMailboxList( unhealthyMailboxes, flags );
    DoCleanup( unhealthyMailboxes );

    unhealthyMailboxes.Reset();

    IMUM_OUT();
    }


// End of File

