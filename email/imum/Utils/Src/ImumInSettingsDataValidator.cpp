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
* Description:  ImumInSettingsDataValidator.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <commdb.h>
#include <MuiuMsvUiServiceUtilities.h>

#include "ImumDaSettingsKeys.h"
#include "ImumInternalApiImpl.h"
#include "ImumInSettingsDataValidator.h"
#include "ImumDaErrorCodes.h"
#include "ImumInSettingsDataImpl.h"
#include "ImumInSettingsDataCollectionImpl.h"
#include "ImumDaSettingsKeys.h"
#include "ImumInSettingsKeys.h"
#include "EmailUtils.H"
#include "EmailFeatureUtils.h"
#include "ComDbUtl.h"                      // CMsvCommDbUtilities
#include "ImumUtilsLogging.h"


// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
const TInt KImumInValidatorMaxPortValue = 65536;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CImumInSettingsDataValidator::CImumInSettingsDataValidator()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataValidator::CImumInSettingsDataValidator(
    CImumInternalApiImpl& aInternalApi )
    :
    iInternalApi( aInternalApi ),
    iCommDbUtils( NULL )
    {
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataValidator::~CImumInSettingsDataValidator()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataValidator::~CImumInSettingsDataValidator()
    {
    IMUM_CONTEXT( CImumInSettingsDataValidator::~CImumInSettingsDataValidator, 0, KLogInApi );
    IMUM_IN();

    delete iCommDbUtils;
    iCommDbUtils = NULL;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataValidator::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumInSettingsDataValidator::ConstructL()
    {
    IMUM_CONTEXT( CImumInSettingsDataValidator::ConstructL, 0, KLogInApi );
    IMUM_IN();

    iCommDbUtils = CMsvCommDbUtilities::NewL();

	// Make sure that the filters are set in commdbutils
    iCommDbUtils->InitializeSelectionL();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataValidator::NewL()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataValidator* CImumInSettingsDataValidator::NewL(
    CImumInternalApiImpl& aInternalApi )
    {
    IMUM_STATIC_CONTEXT( CImumInSettingsDataValidator::NewL, 0, utils, KLogInApi );
    IMUM_IN();

    CImumInSettingsDataValidator* self = NewLC( aInternalApi );
    CleanupStack::Pop( self );

    IMUM_OUT();

    return self;
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataValidator::NewLC()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataValidator* CImumInSettingsDataValidator::NewLC(
    CImumInternalApiImpl& aInternalApi )
    {
    IMUM_STATIC_CONTEXT( CImumInSettingsDataValidator::NewLC, 0, utils, KLogInApi );
    IMUM_IN();

    CImumInSettingsDataValidator* self =
        new ( ELeave ) CImumInSettingsDataValidator( aInternalApi );
    CleanupStack::PushL( self );
    self->ConstructL();

    IMUM_OUT();

    return self;
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataValidator::Validate()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataValidator::Validate(
    CImumInSettingsDataImpl& aSettings )
    {
    IMUM_CONTEXT( CImumInSettingsDataValidator::Validate, 0, KLogInApi );
    IMUM_IN();

    TRAPD( error, ValidateL( aSettings ) );

    IMUM_OUT();

    return error;
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataValidator::ValidateL()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataValidator::ValidateL(
    CImumInSettingsDataImpl& aSettings )
    {
    IMUM_CONTEXT( CImumInSettingsDataValidator::ValidateL, 0, KLogInApi );
    IMUM_IN();

    // Settings must have 1 or more connection settings to be valid
    if ( aSettings.ConnectionSettingsCount() < 1 )
        {
        User::Leave( TImumDaErrorCode::NoConnectionSettings );
        }

    // Check that the protocols are valid
    CheckProtocolsL( aSettings );

    // Check all the connection settings
    for ( TInt sending = aSettings.ConnectionSettingsCount();
          --sending >= 0; )
        {
        ValidateL( aSettings.GetInSetL( sending ) );
        }

    // Check the email address
    CheckEmailAddressL( aSettings );

    // Check the mailbox name
    CheckMailboxNameL( aSettings );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataValidator::CheckProtocolsL()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataValidator::CheckProtocolsL(
    CImumInSettingsDataImpl& aSettings )
    {
    IMUM_CONTEXT( CImumInSettingsDataValidator::CheckProtocolsL, 0, KLogInApi );
    IMUM_IN();

    // Retrieve the first protocol
    TInt protocol = VerifyKeyL<TInt>(
        aSettings[0],
        TImumDaSettings::EKeyProtocol,
        TImumDaSettings::EValueProtocolImap4,
        TImumDaSettings::EValueProtocolSmtp,
        TImumDaErrorCode::EValueProtocolReceivingNotSet );

    // First setting must be receiving protocol
    if ( protocol == TImumDaSettings::EValueProtocolSmtp )
        {
        User::Leave( TImumDaErrorCode::EValueProtocolFirstNotReceiving );
        }

    // Check all the sending settings
    for ( TInt sending = aSettings.ConnectionSettingsCount();
          --sending > 0; )
        {
        protocol = VerifyKeyL<TInt>(
            aSettings[sending],
            TImumDaSettings::EKeyProtocol,
            TImumDaSettings::EValueProtocolImap4,
            TImumDaSettings::EValueProtocolSmtp,
            TImumDaErrorCode::EValueProtocolSendingNotSet );

        // Only smtp is supported sending protocol
        if ( protocol != TImumDaSettings::EValueProtocolSmtp )
            {
            User::Leave( TImumDaErrorCode::EValueProtocolTooManyReceiving );
            }
        }

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataValidator::Validate()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataValidator::Validate(
    MImumInSettingsDataCollection& aConnectionSettings )
    {
    IMUM_CONTEXT( CImumInSettingsDataValidator::Validate, 0, KLogInApi );
    IMUM_IN();

    TRAPD( error, ValidateL( aConnectionSettings ) );

    IMUM_OUT();
    return error;
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataValidator::ValidateL()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataValidator::ValidateL(
    MImumInSettingsDataCollection& aConnectionSettings )
    {
    IMUM_CONTEXT( CImumInSettingsDataValidator::ValidateL, 0, KLogInApi );
    IMUM_IN();

    // Get and check protocol
    TInt protocol = VerifyKeyL<TInt>(
        aConnectionSettings,
        TImumDaSettings::EKeyProtocol,
        TImumDaSettings::EValueProtocolImap4,
        TImumDaSettings::EValueProtocolSmtp,
        TImumDaErrorCode::EValueProtocolInvalid );

    // Check server address
    CheckServerAddressL( aConnectionSettings );

    // Check access point
    ValidateAccessPointL( aConnectionSettings );

    // Validate security settings
    ValidateSecurityL( aConnectionSettings );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataValidator::ValidateAccessPointL()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataValidator::ValidateAccessPointL(
    MImumInSettingsDataCollection& aConnectionSettings )
    {
    IMUM_CONTEXT( CImumInSettingsDataValidator::ValidateAccessPointL, 0, KLogInApi );
    IMUM_IN();

    // Access Point contains the id of the access point or always ask setting
    TInt apId = ValidateKey<TInt>(
        aConnectionSettings,
        TImumDaSettings::EKeyAccessPoint,
        TImumDaSettings::EValueApAlwaysAsk,
        KMaxTInt32 );

    TInt type = ValidateKey<TInt>(
        aConnectionSettings,
        TImumInSettings::EKeyAccessPointType,
        TImumInSettings::EValueApIsIapId,
        TImumInSettings::EValueApIsIapId );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataValidator::CheckEmailAddressL()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataValidator::CheckEmailAddressL(
    const CImumInSettingsDataImpl& aSettings )
    {
    IMUM_CONTEXT( CImumInSettingsDataValidator::CheckEmailAddressL, 0, KLogInApi );
    IMUM_IN();

    // Get the info if the email address can be empty
    TInt allowEmpty = EFalse;
    aSettings.GetAttr(
        TImumInSettings::EKeyAllowEmptyEmailAddress, allowEmpty );

    // Fetch the address
    TImumDaSettings::TTextEmailAddress address;
    if ( aSettings.GetAttr(
        TImumDaSettings::EKeyEmailAddress, address ) )
        {
        User::Leave( TImumDaErrorCode::EEmailAddressNotSet );
        }

    // If the empty mail address is not allowed and the length is zero, leave
    if ( !allowEmpty && !address.Length() )
        {
        User::Leave( TImumDaErrorCode::EEmailAddressEmpty );
        }

    if ( address.Length() > 0 )
    	{
    	// Validate email address if it has any characters
    	if ( !MsvUiServiceUtilities::IsValidEmailAddressL( address ) )
    		{
    		User::Leave( TImumDaErrorCode::EEmailAddressInvalid );
    		}
    	}

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataValidator::CheckServerAddressL()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataValidator::CheckServerAddressL(
    const MImumInSettingsDataCollection& aConnectionSettings )
    {
    IMUM_CONTEXT( CImumInSettingsDataValidator::CheckServerAddressL, 0, KLogInApi );
    IMUM_IN();

    // Fetch the address
    TImumDaSettings::TTextServerAddress address;
    if ( aConnectionSettings.GetAttr(
        TImumDaSettings::EKeyServer, address ) )
        {
        User::Leave( TImumDaErrorCode::EServerAddressNotSet );
        }

    // If the empty mail address is not allowed and the length is zero, leave
    if ( !address.Length() )
        {
        User::Leave( TImumDaErrorCode::EServerAddressEmpty );
        }

    // Validate server address
    if ( !MsvUiServiceUtilities::IsValidDomainL( address ) )
        {
        User::Leave( TImumDaErrorCode::EServerAddressInvalid );
        }

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataValidator::CheckMailboxNameL()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataValidator::CheckMailboxNameL(
    const CImumInSettingsDataImpl& aSettings )
    {
    IMUM_CONTEXT( CImumInSettingsDataValidator::CheckMailboxNameL, 0, KLogInApi );
    IMUM_IN();

    // Check if the name exists for the mailbox
    TImumDaSettings::TTextMailboxName name;
    if ( aSettings.GetAttr(
            TImumDaSettings::EKeyMailboxName, name ) )
        {
        User::Leave( TImumDaErrorCode::EMailboxNameNotSet );
        }
    else
        {
        TInt generate = 0;
        aSettings.GetAttr(
            TImumInSettings::EKeyGenerateMailboxName, generate );
        TMsvId mailboxId = 0;
        aSettings[0].GetAttr(
            TImumDaSettings::EKeyMailboxId, mailboxId );

        // Do the account check
        TRAPD( error, MsvEmailMtmUiUtils::CheckAccountNameL(
            iInternalApi, name, mailboxId, generate ) );
        if ( error == KErrAlreadyExists )
            {
            User::Leave( TImumDaErrorCode::EMailboxNameAlreadyExist );
            }
        else if ( error )
            {
            User::Leave( TImumDaErrorCode::EMailboxNameUnknownError );
            }
        else
            {
            // No errors
            }
        }

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataValidator::GetDefaultSecurityPortL()
// ----------------------------------------------------------------------------
//
TInt CImumInSettingsDataValidator::GetDefaultSecurityPortL(
    const TInt aSecurity,
    const TInt aProtocol )
    {
    IMUM_CONTEXT( CImumInSettingsDataValidator::GetDefaultSecurityPortL, 0, KLogInApi );
    IMUM_IN();

    TInt port( 0 );

    switch( aSecurity )
        {
        case TImumDaSettings::EValueSecurityTls:
        case TImumDaSettings::EValueSecurityOff:
            {
            switch ( aProtocol )
                {
                case TImumDaSettings::EValueProtocolImap4:
                    port = TImumDaSettings::EDefaultPortImap4;
                    break;

                case TImumDaSettings::EValueProtocolPop3:
                    port = TImumDaSettings::EDefaultPortPop3;
                    break;

                case TImumDaSettings::EValueProtocolSmtp:
                    port = TImumDaSettings::EDefaultPortSmtp;
                    break;

                default:
                    User::Leave( TImumDaErrorCode::ESecurityProtocolIllegal );
                }
            break;
            }
        case TImumDaSettings::EValueSecuritySsl:
            {
            switch ( aProtocol )
                {
                case TImumDaSettings::EValueProtocolImap4:
                    port = TImumDaSettings::EDefaultSecurityPortImap4;
                    break;

                case TImumDaSettings::EValueProtocolPop3:
                    port = TImumDaSettings::EDefaultSecurityPortPop3;
                    break;

                case TImumDaSettings::EValueProtocolSmtp:
                    port = TImumDaSettings::EDefaultSecurityPortSmtp;
                    break;

                default:
                    User::Leave( TImumDaErrorCode::ESecurityProtocolIllegal );
                }
            }
            break;
        }

    IMUM_OUT();

    return port;
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataValidator::ValidateSecurityL()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataValidator::ValidateSecurityL(
    MImumInSettingsDataCollection& aConnectionSettings )
    {
    IMUM_CONTEXT( CImumInSettingsDataValidator::ValidateSecurityL, 0, KLogInApi );
    IMUM_IN();

    // Validate security settings
    TInt security = ValidateKey<TInt>(
        aConnectionSettings,
        TImumDaSettings::EKeySecurity,
        TImumDaSettings::EValueSecurityOff,
        TImumDaSettings::EValueSecuritySsl );

    // Validate port settings
    TInt port = ValidateKey<TInt>(
        aConnectionSettings,
        TImumDaSettings::EKeyPort,
        TImumDaSettings::EValuePortDefault,
        KImumInValidatorMaxPortValue );

    // Set correct default port
    if ( port == TImumDaSettings::EValuePortDefault )
        {
        TInt protocol = 0;
        aConnectionSettings.GetAttr(
            TImumDaSettings::EKeyProtocol,
            protocol );
        port = GetDefaultSecurityPortL( security, protocol );
        aConnectionSettings.SetAttr(
            TImumDaSettings::EKeyPort, port );
        }

    IMUM_OUT();
    }

// End of File
