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
* Description:  ImumMboxSettingsUtils.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <SendUiConsts.h>                       // Constants
#include <ImumInSettingsDataCollection.h>       // MImumInSettingsDataCollection
#include <ImumInMailboxServices.h>
#include <ImumInMailboxUtilities.h>

#include "ImumMboxSettingsUtils.h"
#include "ImumMtmLogging.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================


// ----------------------------------------------------------------------------
// ImumMboxSettingsUtils::SetLoginInformationL()
// ----------------------------------------------------------------------------
//
void ImumMboxSettingsUtils::SetLoginInformationL(
    CImumInternalApi& aMailboxApi,
    const TDesC8& aUserName,
    const TDesC8& aPassword,
    const TBool aPasswordTemporary,
    const TMsvId aServiceId )
    {
    IMUM_STATIC_CONTEXT( ImumMboxSettingsUtils::SetLoginInformationL, 0, mtm, KImumMtmLog );
    IMUM_IN();

    // Retrieve the settings of the mailbox
    const MImumInMailboxUtilities& utils = aMailboxApi.MailboxUtilitiesL();
    TMsvEntry entry = utils.GetMailboxEntryL( aServiceId );
    CImumInSettingsData* accountsettings =
        aMailboxApi.MailboxServicesL().LoadMailboxSettingsL( aServiceId );
    CleanupStack::PushL( accountsettings );
    MImumInSettingsDataCollection* connection = NULL;

    // Get the correct connection settings
    switch( entry.iMtm.iUid )
        {
        case KSenduiMtmPop3UidValue:
        case KSenduiMtmImap4UidValue:
            connection = &accountsettings->GetInSetL( 0 );
            break;

        case KSenduiMtmSmtpUidValue:
            connection = &accountsettings->GetInSetL( 1 );
            break;

        // Default, just leave
        default:
            User::Leave( KErrUnknown );
            break;
        }

    // Set login data to settings
    User::LeaveIfError( connection->SetAttr(
        TImumDaSettings::EKeyUsername, aUserName ) );
    User::LeaveIfError( connection->SetAttr(
        TImumDaSettings::EKeyPassword, aPassword ) );
    User::LeaveIfError( connection->SetAttr(
        TImumInSettings::EKeyTemporaryUsername, EFalse ) );
    User::LeaveIfError( connection->SetAttr(
        TImumInSettings::EKeyTemporaryPassword, aPasswordTemporary ) );

    // Save mailbox data
    aMailboxApi.MailboxServicesL().SaveMailboxSettingsL( *accountsettings );
    CleanupStack::PopAndDestroy( accountsettings );
    accountsettings = NULL;
    connection = NULL;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// ImumMboxSettingsUtils::GetLoginInformationL()
// ----------------------------------------------------------------------------
//
void ImumMboxSettingsUtils::GetLoginInformationL(
    CImumInternalApi& aMailboxApi,
    TDes8& aUserName,
    TDes8& aPassword,
    TBool& aPasswordTemporary,
    const TMsvId aServiceId )
    {
    IMUM_STATIC_CONTEXT( ImumMboxSettingsUtils::GetLoginInformationL, 0, mtm, KImumMtmLog );
    IMUM_IN();

    // Retrieve the settings of the mailbox
    CImumInSettingsData* accountsettings =
        aMailboxApi.MailboxServicesL().LoadMailboxSettingsL( aServiceId );
    CleanupStack::PushL( accountsettings );

    TMsvEntry entry =
        aMailboxApi.MailboxUtilitiesL().GetMailboxEntryL( aServiceId );
    GetLoginInformationL( *accountsettings, entry.iMtm, aUserName, aPassword,
        aPasswordTemporary );

    CleanupStack::PopAndDestroy( accountsettings );
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// ImumMboxSettingsUtils::GetLoginInformationL()
// ----------------------------------------------------------------------------
//
void ImumMboxSettingsUtils::GetLoginInformationL(
    const CImumInSettingsData& aAccountSettings,
    TUid aMtmUid,
    TDes8& aUserName,
    TDes8& aPassword,
    TBool& aPasswordTemporary )
    {
    // Get login information. Avoid loading settings data. Use caller's data
    // set and execute faster.

    IMUM_STATIC_CONTEXT( ImumMboxSettingsUtils::GetLoginInformationL, 0, mtm, KImumMtmLog );
    IMUM_IN();

    TInt dataIndex = 0;
    switch( aMtmUid.iUid )
        {
        case KSenduiMtmPop3UidValue:
        case KSenduiMtmImap4UidValue:
            break;
        case KSenduiMtmSmtpUidValue:
            dataIndex = 1;
            break;
        // Default, just leave
        default:
            User::Leave( KErrUnknown );
        }
    MImumInSettingsDataCollection& connection = aAccountSettings.GetInSetL( dataIndex );

    // Read the data
    User::LeaveIfError( connection.GetAttr(
        TImumDaSettings::EKeyUsername, aUserName ) );
    User::LeaveIfError( connection.GetAttr(
        TImumDaSettings::EKeyPassword, aPassword ) );
    User::LeaveIfError( connection.GetAttr(
        TImumInSettings::EKeyTemporaryPassword, aPasswordTemporary ) );
    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// ImumMboxSettingsUtils::IsImap4()
// ---------------------------------------------------------------------------
//
TBool ImumMboxSettingsUtils::IsImap4( const CImumInSettingsData& aSettings )
    {
    IMUM_STATIC_CONTEXT( ImumMboxSettingsUtils::IsImap4, 0, mtm, KImumMtmLog );
    IMUM_IN();

    TInt protocol = TImumDaSettings::EValueProtocolUnset;
    TRAP_IGNORE( aSettings.GetInSetL( 0 ).GetAttr( TImumDaSettings::EKeyProtocol, protocol ) );
    // protocol == TImumDaSettings::EValueProtocolUnset if leave should happen

    IMUM_OUT();
    return ( protocol == TImumDaSettings::EValueProtocolImap4 );
    }

// ---------------------------------------------------------------------------
// ImumMboxSettingsUtils::ValidateSettingsL()
// ---------------------------------------------------------------------------
//
TBool ImumMboxSettingsUtils::ValidateSettingsL(
    CImumInternalApi& aMailboxApi,
    const TMsvId aMailboxId )
    {
    IMUM_STATIC_CONTEXT( ImumMboxSettingsUtils::ValidateSettingsL, 0, mtm, KImumMtmLog );
    IMUM_IN();

    CImumInSettingsData* accountsettings =
        aMailboxApi.MailboxServicesL().LoadMailboxSettingsL( aMailboxId );
    CleanupStack::PushL( accountsettings );

    TBool ok = !accountsettings->Validate();

    CleanupStack::PopAndDestroy( accountsettings );
    accountsettings = NULL;
    IMUM_OUT();

    return ok;
    }

// ----------------------------------------------------------------------------
// ImumMboxSettingsUtils::SetLastUpdateInfoL()
// ----------------------------------------------------------------------------
//
void ImumMboxSettingsUtils::SetLastUpdateInfoL(
    CImumInternalApi& aEmailApi,
    TMsvId aMailboxId,
    const TBool& aSuccessful )
    {
    IMUM_STATIC_CONTEXT( ImumMboxSettingsUtils::SetLastUpdateInfoL, 0, mtm, KImumMtmLog );
    IMUM_IN();

    if ( aSuccessful )
        {
        TTime hometime = TTime();
        hometime.HomeTime();

        aEmailApi.MailboxServicesL().SetLastUpdateInfoL( aMailboxId,
            aSuccessful, hometime );
        }
    IMUM_OUT();
    }
