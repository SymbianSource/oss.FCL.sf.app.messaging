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
* Description:  IMASAccountControl.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <SendUiConsts.h>
#include <centralrepository.h>

#include "IMASAccountControl.h"
#include "EmailUtils.H"
#include "ImumPrivateCRKeys.h"          // Predefined mailbox keys
#include "ImumPanic.h"
#include "ImumUtilsLogging.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
enum TIACErrors
    {
    EIACNoError = 0,
    EIACMboxToAccIdMtmError,
    EIACAccIdToExtIdMtmError,
    EIACExtIdToAccIdMtmError
    };

// Account Id conversion to Extension Id
enum TIACKeyShifts
    {
    EIACKeyShiftImap4  = 0,
    EIACKeyShiftPop3   = 8
    };

const TInt KImasShiftAccountId = 8;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// IMASAccountControl::CreateBaseKeyFromAccountId()
// ----------------------------------------------------------------------------
//
TUint32 IMASAccountControl::CreateBaseKeyFromAccountId(
    const TUint32 aAccountId,
    const TUid& aProtocol )
    {
    IMUM_STATIC_CONTEXT( IMASAccountControl::CreateBaseKeyFromAccountId, 0, utils, KLogData );

    TUint32 extendedAccountId =
        AccountIdToExtendedAccountId( aAccountId, aProtocol );
    return ( extendedAccountId << KImasShiftAccountId );
    }

// ----------------------------------------------------------------------------
// IMASAccountControl::CreateBaseKeyFromExtendedAccountId()
// ----------------------------------------------------------------------------
//
TUint32 IMASAccountControl::CreateBaseKeyFromExtendedAccountId(
    const TUint32 aExtendedAccountId )
    {
    IMUM_STATIC_CONTEXT( IMASAccountControl::CreateBaseKeyFromExtendedAccountId, 0, utils, KLogData );

    return ( aExtendedAccountId << KImasShiftAccountId );
    }

// ----------------------------------------------------------------------------
// IMASAccountControl::MailboxIdToAccountIdL()
// ----------------------------------------------------------------------------
//
TUint32 IMASAccountControl::MailboxIdToAccountIdL(
    const TMsvId aMailboxId,
    const TUid& aMtmId )
    {
    IMUM_STATIC_CONTEXT( IMASAccountControl::MailboxIdToAccountIdL, 0, utils, KLogData );

    TUint32 accountId = 0;

    switch ( aMtmId.iUid )
        {
        case KSenduiMtmSmtpUidValue:
            accountId = IMASAccountControl::GetSmtpAccountIdL(
                aMailboxId ).iSmtpAccountId;
            break;

        case KSenduiMtmPop3UidValue:
            accountId = IMASAccountControl::GetPopAccountIdL(
                aMailboxId ).iPopAccountId;
            break;

        case KSenduiMtmImap4UidValue:
            accountId = IMASAccountControl::GetImapAccountIdL(
                aMailboxId ).iImapAccountId;
            break;

        default:
            __ASSERT_DEBUG( EFalse, User::Panic(
                KIMASAccountControlPanic, EIACMboxToAccIdMtmError ) );
            User::Leave( KErrNotSupported );
            break;
        }

    return accountId;
    }

// ----------------------------------------------------------------------------
// IMASAccountControl::AccountIdToMailboxId()
// ----------------------------------------------------------------------------
//
TMsvId IMASAccountControl::AccountIdToMailboxId(
    CEmailAccounts& aAccounts,
    const TUint32 aAccountId,
    const TUid& aMtmId )
    {
    IMUM_STATIC_CONTEXT( IMASAccountControl::AccountIdToMailboxId, 0, utils, KLogData );

    TMsvId mailboxId = KErrNotFound;

    // Get the correct accounts according to the protocol
    if ( aMtmId.iUid == KSenduiMtmImap4UidValue )
        {
        // IMAP4
        TImapAccount imapAccount;
        RArray<TImapAccount> accountArray;
        TRAPD( error, aAccounts.GetImapAccountsL( accountArray ) );

        TInt account = ( error == KErrNone ) ?
            accountArray.Count() : KErrNotFound;

        while ( mailboxId < 0 && --account >= 0 )
            {
            imapAccount = accountArray[account];

            // Check if the id's match
            if ( imapAccount.iImapAccountId == aAccountId )
                {
                mailboxId = imapAccount.iImapService;
                }
            }
        }
    else
        {
        // POP3
        TPopAccount popAccount;
        RArray<TPopAccount> accountArray;
        TRAPD( error, aAccounts.GetPopAccountsL( accountArray ) );

        TInt account = ( error == KErrNone ) ?
            accountArray.Count() : KErrNotFound;

        while ( mailboxId < 0 && --account >= 0 )
            {
            popAccount = accountArray[account];

            // Check if the id's match
            if ( popAccount.iPopAccountId == aAccountId )
                {
                mailboxId = popAccount.iPopService;
                }
            }
        }

    return mailboxId;
    }


// ----------------------------------------------------------------------------
// IMASAccountControl::AccountIdToExtendedAccountId()
// ----------------------------------------------------------------------------
//
TUint32 IMASAccountControl::AccountIdToExtendedAccountId(
    const TUint32 aAccountId,
    const TUid& aMtmId )
    {
    IMUM_STATIC_CONTEXT( IMASAccountControl::AccountIdToExtendedAccountId, 0, utils, KLogData );

    // Imap4 accounts are 0x01, 0x02, 0x03, 0x04, 0x05, 0x06
    if ( aMtmId == KSenduiMtmImap4Uid )
        {
        return ( aAccountId << EIACKeyShiftImap4 );
        }
    // Pop3 accounts are 0x100, 0x200, 0x300, 0x400, 0x500, 0x600
    else if ( aMtmId == KSenduiMtmPop3Uid )
        {
        return ( aAccountId << EIACKeyShiftPop3 );
        }
    else
        {
        __ASSERT_DEBUG( EFalse, User::Panic(
            KIMASAccountControlPanic, EIACAccIdToExtIdMtmError ) );
        }

    return 0;
    }

// ----------------------------------------------------------------------------
// IMASAccountControl::ExtendedAccountIdToAccountId()
// ----------------------------------------------------------------------------
//
TUint32 IMASAccountControl::ExtendedAccountIdToAccountId(
    const TUint32 aExtendedAccountId,
    const TUid& aMtmId )
    {
    IMUM_STATIC_CONTEXT( IMASAccountControl::ExtendedAccountIdToAccountId, 0, utils, KLogData );

    // Imap4 accounts are 0x01, 0x02, 0x03, 0x04, 0x05, 0x06
    if ( aMtmId == KSenduiMtmImap4Uid )
        {
        return ( aExtendedAccountId >> EIACKeyShiftImap4 );
        }
    // Pop3 accounts are 0x100, 0x200, 0x300, 0x400, 0x500, 0x600
    else if ( aMtmId == KSenduiMtmPop3Uid )
        {
        return ( aExtendedAccountId >> EIACKeyShiftPop3 );
        }
    else
        {
        __ASSERT_DEBUG( EFalse, User::Panic(
            KIMASAccountControlPanic, EIACExtIdToAccIdMtmError ) );
        }

    return 0;
    }

// ----------------------------------------------------------------------------
// IMASAccountControl::GetPopAccountIdL()
// ----------------------------------------------------------------------------
//
TPopAccount IMASAccountControl::GetPopAccountIdL(
    const TMsvId aPopMailboxId )
    {
    IMUM_STATIC_CONTEXT( IMASAccountControl::GetPopAccountIdL, 0, utils, KLogData );

    CEmailAccounts* account = CEmailAccounts::NewLC();

    TPopAccount popAccountId;
    account->GetPopAccountL( aPopMailboxId, popAccountId );

    CleanupStack::PopAndDestroy( account );
    account = NULL;

    return popAccountId;
    }

// ----------------------------------------------------------------------------
// IMASAccountControl::GetImapAccountIdL()
// ----------------------------------------------------------------------------
//
TImapAccount IMASAccountControl::GetImapAccountIdL(
    const TMsvId aImapMailboxId )
    {
    IMUM_STATIC_CONTEXT( IMASAccountControl::GetImapAccountIdL, 0, utils, KLogData );

    CEmailAccounts* account = CEmailAccounts::NewLC();

    TImapAccount imapAccountId;
    account->GetImapAccountL( aImapMailboxId, imapAccountId );

    CleanupStack::PopAndDestroy( account );
    account = NULL;

    return imapAccountId;
    }

// ----------------------------------------------------------------------------
// IMASAccountControl::GetSmtpAccountIdL()
// ----------------------------------------------------------------------------
//
TSmtpAccount IMASAccountControl::GetSmtpAccountIdL(
    const TMsvId aSmtpMailboxId )
    {
    IMUM_STATIC_CONTEXT( IMASAccountControl::GetSmtpAccountIdL, 0, utils, KLogData );

    CEmailAccounts* account = CEmailAccounts::NewLC();

    TSmtpAccount smtpAccountId;
    account->GetSmtpAccountL( aSmtpMailboxId, smtpAccountId );

    CleanupStack::PopAndDestroy( account );
    account = NULL;

    return smtpAccountId;
    }

// ----------------------------------------------------------------------------
// IMASAccountControl::CreateSettingKey()
// ----------------------------------------------------------------------------
//
TUint32 IMASAccountControl::CreateSettingKey(
    const TUint32 aAccountId,
    const TUint32 aSetting,
    const TUid& aMtmId )
    {
    IMUM_STATIC_CONTEXT( IMASAccountControl::CreateSettingKey, 0, utils, KLogData );

    return CreateBaseKeyFromAccountId( aAccountId, aMtmId ) + aSetting;
    }

// End of File
