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
* Description:  ImumMboxSettingsCtrl.cpp
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <centralrepository.h>          // CRepository
#include <cemailaccounts.h>             // CEmailAccounts
#include <miutset.h>                    // KUidMsgTypeSMTP
#include <smtpset.h>                    // CImSmtpSettings
#include <pop3set.h>                    // CImPop3Settings
#include <imapset.h>                    // CImImap4Settings
#include <iapprefs.h>                   // CImIAPPreferences
#include <messaginginternalcrkeys.h>    // KCRUidAOSettings
#include <commdb.h>                     // CCommsDatabase
#include "MuiuDynamicSettingItemBase.h" // CMuiuDynamicSettingsItemBase
#include <SendUiConsts.h>
#include <featmgr.h>

#include "ImumMboxSettingsCtrl.h"
#include "ImumMboxDefaultData.h"
#include "ImumMboxManager.h"
#include "ImumPrivateCRKeys.h"          // Predefined mailbox keys
#include "ImumMboxDataExtension.h"
#include "IMASCenRepControl.h"
#include "ImumUtilsLogging.h"
#include "EmailUtils.H"
#include "ImumInternalApiImpl.h"
#include "ImumInMailboxUtilitiesImpl.h"
#include "ImumPanic.h"
#include "ImumInSettingsKeys.h"
#include "ImumConstants.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
const TUint64 KIMASLastFlag = 64;

enum TISPErrors
    {
    EISPNoError = 0,
    EISPMailboxIdUnderflowFailure,
    EISPAccountIdUnderflowFailure,
    EISPAccountIdOverflowFailure,
    EISPMtmIdParameterFailure
    };

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// TIMAStorerParams::TIMAStorerParams()
// ----------------------------------------------------------------------------
//
TIMAStorerParams::TIMAStorerParams(
    const TUid& aMtmId,
    const TMsvId aMailboxId )
    :
    iMailboxId( aMailboxId ),
    iAccountId( 0 ),
    iMtmId( aMtmId )
    {
    __ASSERT_DEBUG( aMailboxId > 0,
        User::Panic( KIMSStorerParamsPanic, EISPMailboxIdUnderflowFailure ) );
    __ASSERT_DEBUG(
        ( aMtmId == KSenduiMtmImap4Uid ||
          aMtmId == KSenduiMtmPop3Uid ),
        User::Panic( KIMSStorerParamsPanic, EISPMtmIdParameterFailure ) );
    }

// ----------------------------------------------------------------------------
// TIMAStorerParams::TIMAStorerParams()
// ----------------------------------------------------------------------------
//
TIMAStorerParams::TIMAStorerParams(
    const TUint32 aAccountId,
    const TUid& aMtmId )
    :
    iMailboxId( 0 ),
    iAccountId( aAccountId ),
    iMtmId( aMtmId )
    {
    __ASSERT_DEBUG( iAccountId <= KIMASMaxAccounts,
        User::Panic( KIMSStorerParamsPanic, EISPAccountIdOverflowFailure ) );
    __ASSERT_DEBUG(
        ( aMtmId == KSenduiMtmImap4Uid ||
          aMtmId == KSenduiMtmPop3Uid ),
        User::Panic( KIMSStorerParamsPanic, EISPMtmIdParameterFailure ) );

    }

// ----------------------------------------------------------------------------
// TIMAStorerParams::TIMAStorerParams()
// ----------------------------------------------------------------------------
//
TIMAStorerParams::TIMAStorerParams(
    const TMsvEntry& aEntry )
    :
    iMailboxId( aEntry.Id() ),
    iAccountId( aEntry.iMtmData2 ),
    iMtmId( aEntry.iMtm )
    {
    __ASSERT_DEBUG( iAccountId > 0,
        User::Panic( KIMSStorerParamsPanic, EISPAccountIdUnderflowFailure ) );
    __ASSERT_DEBUG( iAccountId <= KIMASMaxAccounts,
        User::Panic( KIMSStorerParamsPanic, EISPAccountIdOverflowFailure ) );
    __ASSERT_DEBUG(
        ( aEntry.iMtm == KSenduiMtmImap4Uid ||
          aEntry.iMtm == KSenduiMtmPop3Uid ),
        User::Panic( KIMSStorerParamsPanic, EISPMtmIdParameterFailure ) );

    }

// ----------------------------------------------------------------------------
// TIMAStorerParams::MailboxId()
// ----------------------------------------------------------------------------
//
TMsvId TIMAStorerParams::MailboxId() const
    {
    return iMailboxId;
    }

// ----------------------------------------------------------------------------
// TIMAStorerParams::AccountId()
// ----------------------------------------------------------------------------
//
TUint32 TIMAStorerParams::AccountId() const
    {
    return iAccountId;
    }

// ----------------------------------------------------------------------------
// TIMAStorerParams::MtmId()
// ----------------------------------------------------------------------------
//
TUid TIMAStorerParams::MtmId() const
    {
    return iMtmId;
    }

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::CImumMboxSettingsCtrl()
// ----------------------------------------------------------------------------
CImumMboxSettingsCtrl::CImumMboxSettingsCtrl(
    CImumInternalApiImpl& aMailboxApi )
    :
    iMailboxApi( aMailboxApi ),
    iUtils( NULL ),
    iCenRepSmtp( NULL ),
    iCenRepExtMail( NULL ),
    iCenRepAlwaysOnline( NULL ),
    iCenRepDefaultData( NULL ),
    iAccounts( NULL ),
    iCenRepControl( NULL )
    {
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::ConstructL()
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::ConstructL, 0, KLogData );
    IMUM_IN();

    iCenRepControl = CIMASCenRepControl::NewL();
    iUtils = const_cast<MImumInMailboxUtilities*>(
        &iMailboxApi.MailboxUtilitiesL() );

    FeatureManager::InitializeLibL();
    iFFEmailVariation = 
        FeatureManager::FeatureSupported( KFeatureIdFfEmailFramework );
    FeatureManager::UnInitializeLib();
    
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::~CImumMboxSettingsCtrl()
// ----------------------------------------------------------------------------
//
CImumMboxSettingsCtrl::~CImumMboxSettingsCtrl()
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::~CImumMboxSettingsCtrl, 0, KLogData );
    IMUM_IN();

    delete iCenRepSmtp;
    iCenRepSmtp = NULL;
    delete iCenRepExtMail;
    iCenRepExtMail = NULL;
    delete iCenRepAlwaysOnline;
    iCenRepAlwaysOnline = NULL;
    delete iCenRepDefaultData;
    iCenRepDefaultData = NULL;
    delete iAccounts;
    iAccounts = NULL;
    delete iCenRepControl;
    iCenRepControl = NULL;
    iUtils = NULL;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::NewL()
// ----------------------------------------------------------------------------
//
CImumMboxSettingsCtrl* CImumMboxSettingsCtrl::NewL(
    CImumInternalApiImpl& aMailboxApi )
    {
    IMUM_STATIC_CONTEXT( CImumMboxSettingsCtrl::NewL, 0, utils, KLogData );
    IMUM_IN();

    CImumMboxSettingsCtrl* self = NewLC( aMailboxApi );
    CleanupStack::Pop( self );

    IMUM_OUT();
    return self;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::NewLC()
// ----------------------------------------------------------------------------
//
CImumMboxSettingsCtrl* CImumMboxSettingsCtrl::NewLC(
    CImumInternalApiImpl& aMailboxApi )
    {
    IMUM_STATIC_CONTEXT( CImumMboxSettingsCtrl::NewLC, 0, utils, KLogData );

    CImumMboxSettingsCtrl* self =
        new ( ELeave ) CImumMboxSettingsCtrl( aMailboxApi );
    CleanupStack::PushL( self );
    self->ConstructL();

    IMUM_OUT();
    return self;
    }

/******************************************************************************

    PUBLIC: CREATING NEW ACCOUNT

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::CreateSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::CreateSettings(
    const TMsvId aMailboxId,
    const TUint32 aAccountId,
    const TUid& aProtocol,
    CImumMboxDataExtension& aExtendedMailSettings,
    const TBool aPopulateDefaults )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::CreateSettings, 0, KLogData );
    IMUM_IN();

    // Initialize the settings
    PreInitializeExtendedSettings(
        aMailboxId, aAccountId, aProtocol, aExtendedMailSettings );

    TInt error = KErrNone;

    // Populate defaults
    if ( aPopulateDefaults )
        {
        error = PopulateDefaultExtSettings( aExtendedMailSettings );
        }

    // If populating has not failed, continue with creating the settings
    if ( error == KErrNone )
        {
        // Create the settings
        TRAP( error, CreateSettingsL( aExtendedMailSettings ) );
        }

    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::CreateSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::CreateSettings(
    const CImumMboxDataExtension& aExtendedMailSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::CreateSettings, 0, KLogData );
    IMUM_IN();

    TInt error = KErrNotReady;

    if ( aExtendedMailSettings.IsSettingsValid() )
        {
        TRAP( error, CreateSettingsL( aExtendedMailSettings ) );
        }

    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::PreInitializeExtendedSettings()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::PreInitializeExtendedSettings(
    const TMsvId aMailboxId,
    const TUint32 aAccountId,
    const TUid& aProtocol,
    CImumMboxDataExtension& aExtendedMailSettings ) const
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::PreInitializeExtendedSettings, 0, KLogData );
    IMUM_IN();

    // Pre-initialize the extended settings
    aExtendedMailSettings.SetMailboxId( aMailboxId );
    aExtendedMailSettings.SetAccountId( aAccountId );
    aExtendedMailSettings.SetProtocol( aProtocol );
    aExtendedMailSettings.SetExtensionId(
        IMASAccountControl::AccountIdToExtendedAccountId(
            aAccountId, aProtocol ) );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::PopulateDefaultExtSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::PopulateDefaultExtSettings(
    CImumMboxDataExtension& aExtendedMailSettings ) const
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::PopulateDefaultExtSettings, 0, KLogData );
    IMUM_IN();

    // Before setting default settings, make sure the settings is properly
    // initialized
    if ( !aExtendedMailSettings.IsSettingsValid() )
        {
        // The setting information is not stored properly, using
        // the settings cannot be continued safely
        // PreInitializeExtendedSettings should be called before this
        IMUM0( 0, " *** MAILBOX EXTENDED SETTINGS FAILURE *** " );

        __ASSERT_DEBUG( EFalse,
            User::Panic( KImumMtmUiPanic, KErrNotReady ) );
        return KErrNotReady;
        }

    //emailnotificationstate is not set to default since its already done
    aExtendedMailSettings.SetNewMailIndicators( ETrue );
    aExtendedMailSettings.SetHideMsgs( EFalse );
    aExtendedMailSettings.SetOpenHtmlMail( EFalse );
    aExtendedMailSettings.SetAlwaysOnlineState( EMailAoHomeOnly );
    aExtendedMailSettings.SetSelectedWeekDays( KImumMboxDefaultAutoRetrievalDays );
    aExtendedMailSettings.SetSelectedTimeStart( 0 );
    aExtendedMailSettings.SetSelectedTimeStop( 0 );
    aExtendedMailSettings.SetInboxRefreshTime( KIMASAoInterval1Hour );
    aExtendedMailSettings.SetUpdateMode( EMailAoHeadersOnly );
    aExtendedMailSettings.SetMailDeletion( EIMASMailDeletionAlwaysAsk );
    TAOInfo emptyInfo = { EFalse, Time::NullTTime(), EFalse };
    aExtendedMailSettings.SetLastUpdateInfo( emptyInfo );
    aExtendedMailSettings.SetIsPredefined( KImumMboxDefaultIsPredefinedMailbox );

    IMUM_OUT();
    return KErrNone;
    }

/******************************************************************************

    PUBLIC: DELETING EXISTING ACCOUNT

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::DeleteExtendedSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::DeleteExtendedSettings(
    const TIMAStorerParams& aParams )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::DeleteExtendedSettings, 0, KLogData );
    IMUM_IN();
    // Make sure the mtm is valid email mtm
    TBool ok = iUtils->IsMailMtm( aParams.MtmId(), ETrue );
    TInt error = KErrNotSupported;

    // Choose the right way to Create the settings
    if ( ok && aParams.AccountId() != KErrNone )
        {
        TRAP( error, DeleteExtendedSettingsL(
            aParams.AccountId(), aParams.MtmId() ) );
        }
    else if ( ok && aParams.MailboxId() != KErrNone )
        {
        TRAP( error, DeleteExtendedSettingsL(
            aParams.MailboxId(), aParams.MtmId() ) );
        }

    IMUM_OUT();
    return error;
    }


/******************************************************************************

    PUBLIC: SAVING CURRENT ACCOUNT

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::SaveSettings(
    const CImumMboxDataExtension& aExtendedMailSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveSettings, 0, KLogData );
    IMUM_IN();
    TInt error = KErrNotReady;

    // Here it is good idea to make sure, that the settings are valid, so
    // no corrupted data is going in.
    if ( aExtendedMailSettings.IsSettingsValid() )
        {
        TRAP( error, SaveSettingsL( aExtendedMailSettings ) );
        }
    else
        {
        __ASSERT_DEBUG( EFalse,
            User::Panic( KImumMtmUiPanic, KErrNotReady ) );
        }

    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::SaveSettings(
    const TUint32 aAccountId,
    const CImImap4Settings& aImap4Settings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveSettings, 0, KLogData );
    IMUM_IN();

    TRAPD( error, SaveSettingsL( aAccountId, aImap4Settings ) );

    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::SaveSettings(
    const TUint32 aAccountId,
    const CImPop3Settings& aPop3Settings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveSettings, 0, KLogData );
    IMUM_IN();

    TRAPD( error, SaveSettingsL( aAccountId, aPop3Settings ) );

    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::SaveSettings(
    const TUint32 aAccountId,
    const CImSmtpSettings& aSmtpSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveSettings, 0, KLogData );
    IMUM_IN();

    TRAPD( error, SaveSettingsL( aAccountId, aSmtpSettings ) );

    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::SaveSettings(
    const TMsvId aMailboxId,
    const CImImap4Settings& aImap4Settings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveSettings, 0, KLogData );
    IMUM_IN();

    TRAPD( error, SaveSettingsL( aMailboxId, aImap4Settings ) );

    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::SaveSettings(
    const TMsvId aMailboxId,
    const CImPop3Settings& aPop3Settings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveSettings, 0, KLogData );
    IMUM_IN();

    TRAPD( error, SaveSettingsL( aMailboxId, aPop3Settings ) );

    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::SaveSettings(
    const TMsvId aMailboxId,
    const CImSmtpSettings& aSmtpSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveSettings, 0, KLogData );
    IMUM_IN();

    TRAPD( error, SaveSettingsL( aMailboxId, aSmtpSettings ) );

    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::SaveSettings(
    const TMsvEntry& aEntry,
    CImBaseEmailSettings& aBaseSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveSettings, 0, KLogData );
    IMUM_IN();

    TRAPD( error, SaveSettingsL( aEntry, aBaseSettings ) );

    IMUM_OUT();
    return error;
    }

/******************************************************************************

    PUBLIC: LOADING EXISTING ACCOUNT

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::LoadSettings(
    const TIMAStorerParams& aParams,
    CImumMboxDataExtension& aExtendedMailSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadSettings, 0, KLogData );
    IMUM3_IN( "aParams.iMtmId = 0x%x, aParams.iAccountId = 0x%x, aParams.iMailboxId = 0x%x",
        aParams.MtmId(), aParams.AccountId(), aParams.MailboxId() );

    // Make sure the mtm is valid email mtm
    TBool ok = iUtils->IsMailMtm( aParams.MtmId(), ETrue );

    // Choose the right way to Load the settings
    if ( ok && aParams.AccountId() != KErrNone )
        {
        TRAPD( error, LoadSettingsL(
            aParams.AccountId(), aParams.MtmId(), aExtendedMailSettings ) );
    	IMUM_OUT();
        return error;
        }
    else if ( ok && aParams.MailboxId() != KErrNone )
        {
        TRAPD( error, LoadSettingsL(
            aParams.MailboxId(), aParams.MtmId(), aExtendedMailSettings ) );
    	IMUM_OUT();
        return error;
        }
    else
        {
    	IMUM_OUT();
        return KErrNotSupported;
        }

    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::LoadSettings(
    const TUint32 aAccountId,
    CImImap4Settings& aImap4Settings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadSettings, 0, KLogData );
    IMUM_IN();

    TRAPD( error, LoadSettingsL( aAccountId, aImap4Settings ) );

    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::LoadSettings(
    const TUint32 aAccountId,
    CImPop3Settings& aPop3Settings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadSettings, 0, KLogData );
    IMUM_IN();

    TRAPD( error, LoadSettingsL( aAccountId, aPop3Settings ) );

    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::LoadSettings(
    const TUint32 aAccountId,
    CImSmtpSettings& aSmtpSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadSettings, 0, KLogData );
    IMUM_IN();

    TRAPD( error, LoadSettingsL( aAccountId, aSmtpSettings ) );

    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::LoadSettings(
    const TMsvId aMailboxId,
    CImImap4Settings& aImap4Settings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadSettings, 0, KLogData );
    IMUM_IN();

    TRAPD( error, LoadSettingsL( aMailboxId, aImap4Settings ) );

    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::LoadSettings(
    const TMsvId aMailboxId,
    CImPop3Settings& aPop3Settings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadSettings, 0, KLogData );
    IMUM_IN();

    TRAPD( error, LoadSettingsL( aMailboxId, aPop3Settings ) );

    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::LoadSettings(
    const TMsvId aMailboxId,
    CImSmtpSettings& aSmtpSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadSettings, 0, KLogData );
    IMUM_IN();

    TRAPD( error, LoadSettingsL( aMailboxId, aSmtpSettings ) );

    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadSettings()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::LoadSettings(
    const TMsvEntry& aEntry,
    CImBaseEmailSettings& aBaseSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadSettings, 0, KLogData );
    IMUM_IN();

    TRAPD( error, LoadSettingsL( aEntry, aBaseSettings ) );

    IMUM_OUT();
    return error;
    }
// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::AccountId2ExtensionId()
// ----------------------------------------------------------------------------
//
TUint32 CImumMboxSettingsCtrl::AccountId2ExtensionId(
    const TUint32 aAccountId,
    const TUid& aMtmId ) const
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::AccountId2ExtensionId, 0, KLogData );
    IMUM_IN();

    IMUM_OUT();
    return IMASAccountControl::AccountIdToExtendedAccountId(
        aAccountId, aMtmId );
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::ExtensionId2AccountId()
// ----------------------------------------------------------------------------
//
TUint32 CImumMboxSettingsCtrl::ExtensionId2AccountId(
    const TUint32 aExtension,
    const TUid& aMtmId ) const
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::ExtensionId2AccountId, 0, KLogData );
    IMUM_IN();

    IMUM_OUT();
    return IMASAccountControl::ExtendedAccountIdToAccountId(
        aExtension, aMtmId );
    }

/******************************************************************************

    PUBLIC: RESTORE DEFAULT DATA

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::RestoreDefaultSettingsDataL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::RestoreDefaultSettingsDataL(
    CImumMboxDefaultData& aDefaultData )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::RestoreDefaultSettingsDataL, 0, KLogData );
    IMUM_IN();

    // Make sure there is connection to repository
    MsvEmailMtmUiUtils::CreateCenRepConnectionL(
        iCenRepDefaultData, KCRUidImum );

    // Prepare default data reading
    iCenRepControl->SetActiveCentralRepository( *iCenRepDefaultData );
    iCenRepControl->SetActiveDefaultDataKey();

    // Begin filling the value
    TInt value = iCenRepControl->GetFromAccountL(
        KImumDownloadBufferSizeOther );
    aDefaultData.SetAttr(
        TImumInSettings::EKeyDownloadBufferGprs, value );

    value = iCenRepControl->GetFromAccountL(
        KImumDownloadBufferSizeWlan );
    aDefaultData.SetAttr(
        TImumInSettings::EKeyDownloadBufferWlan, value );

    IMUM_OUT();
    }

/******************************************************************************

    PRIVATE: CREATING NEW ACCOUNT

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::CreateSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::CreateSettingsL(
    const CImumMboxDataExtension& aExtendedMailSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::CreateSettingsL, 0, KLogData );
    IMUM_IN();

    // Make sure there is connection to repository
    MsvEmailMtmUiUtils::CreateCenRepConnectionL(
        iCenRepExtMail, KCRUidImumSettings );

    // Prepare account control
    iCenRepControl->SetActiveCentralRepository( *iCenRepExtMail );
    iCenRepControl->SetActiveBaseKey( aExtendedMailSettings.ExtensionId() );

    // Create the settings
    // Common Settings
    CreateExtendedSettingsStatusFlagsL( aExtendedMailSettings );
    iCenRepControl->CreateToAccountL( EImaCommonSetMailboxId,
        aExtendedMailSettings.MailboxId() );
    iCenRepControl->CreateToAccountL( EImaCommonSetAccountId,
        static_cast<TInt>( aExtendedMailSettings.AccountId() ) );
    iCenRepControl->CreateToAccountL( EImaCommonSetExtensionId,
        static_cast<TInt>( aExtendedMailSettings.ExtensionId() ) );
    iCenRepControl->CreateToAccountL( EImaCommonSetProtocol,
        aExtendedMailSettings.Protocol().iUid );
    iCenRepControl->CreateToAccountL( EImaCommonSetEmailAddress,
        *aExtendedMailSettings.EmailAddress() );

    // Extended mailbox settings
    iCenRepControl->CreateToAccountL( EImaExtSetEmailNotif,
        static_cast<TInt>( aExtendedMailSettings.EmailNotificationState() ) );
    iCenRepControl->CreateToAccountL( EImaExtSetIndicators,
        static_cast<TBool>( aExtendedMailSettings.NewMailIndicators() ) );
    iCenRepControl->CreateToAccountL( EImaExtSetHideMsgs,
        static_cast<TInt>( aExtendedMailSettings.HideMsgs() ) );
    iCenRepControl->CreateToAccountL( EImaExtSetOpenHtmlMail,
        static_cast<TInt>( aExtendedMailSettings.OpenHtmlMail() ) );
    iCenRepControl->CreateToAccountL( EImaExtSetMailDeletion,
        static_cast<TInt>( aExtendedMailSettings.MailDeletion() ) );
    iCenRepControl->CreateToAccountL( EImaExtIsPredefined,
        static_cast<TInt>( aExtendedMailSettings.IsPredefined() ) );

    // Always Online Settings
    iCenRepControl->CreateToAccountL( EImaAoVersion,
        aExtendedMailSettings.Version() );
    iCenRepControl->CreateToAccountL( EImaAoOnlineState,
        aExtendedMailSettings.AlwaysOnlineState() );
    iCenRepControl->CreateToAccountL( EImaAoWeekDays,
        aExtendedMailSettings.SelectedWeekDays() );
    iCenRepControl->CreateToAccountL( EImaAoTimeStartH, EImaAoTimeStartL,
        aExtendedMailSettings.SelectedTimeStart().Int64() );
    iCenRepControl->CreateToAccountL( EImaAoTimeStopH, EImaAoTimeStopL,
        aExtendedMailSettings.SelectedTimeStop().Int64() );
    iCenRepControl->CreateToAccountL( EImaAoInboxRefreshTime,
        aExtendedMailSettings.InboxRefreshTime() );
    iCenRepControl->CreateToAccountL( EImaAoUpdateMode,
        aExtendedMailSettings.UpdateMode() );

    CreateLastUpdateInfoL( aExtendedMailSettings.LastUpdateInfo() );

    IMUM_OUT();
    }

/******************************************************************************

    PRIVATE: DELETING EXISTING ACCOUNT

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::DeleteExtendedSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::DeleteExtendedSettingsL(
    const TMsvId aMailboxId,
    const TUid& aMtmId )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::DeleteExtendedSettingsL, 0, KLogData );
    IMUM_IN();

    // Get the correct accountId and continue opening the settings
    TUint32 accountId =
        IMASAccountControl::MailboxIdToAccountIdL( aMailboxId, aMtmId );
    DeleteExtendedSettingsL( accountId, aMtmId );
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::DeleteExtendedSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::DeleteExtendedSettingsL(
    const TUint32 aAccountId,
    const TUid& aMtmId )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::DeleteExtendedSettingsL, 0, KLogData );
    IMUM_IN();

    // Make sure there is connection to repository
    MsvEmailMtmUiUtils::CreateCenRepConnectionL(
        iCenRepExtMail, KCRUidImumSettings );

    // Prepare account control
    iCenRepControl->SetActiveCentralRepository( *iCenRepExtMail );
    iCenRepControl->SetActiveBaseKey( aAccountId, aMtmId );

    // Create the settings
    // Common Settings
    iCenRepControl->DeleteFromAccountL(
        EImaCommonSetStatusFlagsH, EImaCommonSetStatusFlagsL );
    iCenRepControl->DeleteFromAccountL( EImaCommonSetMailboxId );
    iCenRepControl->DeleteFromAccountL( EImaCommonSetAccountId );
    iCenRepControl->DeleteFromAccountL( EImaCommonSetExtensionId );
    iCenRepControl->DeleteFromAccountL( EImaCommonSetProtocol );
    iCenRepControl->DeleteFromAccountL( EImaCommonSetEmailAddress );

    // Extended mailbox settings
    iCenRepControl->DeleteFromAccountL( EImaExtSetEmailNotif );
    iCenRepControl->DeleteFromAccountL( EImaExtSetIndicators );
    iCenRepControl->DeleteFromAccountL( EImaExtSetHideMsgs );
    iCenRepControl->DeleteFromAccountL( EImaExtSetOpenHtmlMail );
    iCenRepControl->DeleteFromAccountL( EImaExtSetMailDeletion );
    iCenRepControl->DeleteFromAccountL( EImaExtIsPredefined );

    // Always Online Settings
    iCenRepControl->DeleteFromAccountL( EImaAoVersion );
    iCenRepControl->DeleteFromAccountL( EImaAoOnlineState );
    iCenRepControl->DeleteFromAccountL( EImaAoWeekDays );
    iCenRepControl->DeleteFromAccountL( EImaAoTimeStartH, EImaAoTimeStartL );
    iCenRepControl->DeleteFromAccountL( EImaAoTimeStopH, EImaAoTimeStopL );
    iCenRepControl->DeleteFromAccountL( EImaAoInboxRefreshTime );
    iCenRepControl->DeleteFromAccountL( EImaAoUpdateMode );
    DeleteLastUpdateInfo();

    IMUM_OUT();
    }

/******************************************************************************

    PRIVATE: SAVING CURRENT ACCOUNT

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::SaveSettingsL(
    const CImumMboxDataExtension& aExtendedMailSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveSettingsL, 0, KLogData );
    IMUM_IN();

    // Make sure there is connection to repository
    MsvEmailMtmUiUtils::CreateCenRepConnectionL(
        iCenRepExtMail, KCRUidImumSettings );

    // Prepare account control
    iCenRepControl->SetActiveCentralRepository( *iCenRepExtMail );
    iCenRepControl->SetActiveBaseKey( aExtendedMailSettings.ExtensionId() );

    // Store the settings
    // Common Settings
    SaveExtendedSettingsStatusFlagsL( aExtendedMailSettings );
    iCenRepControl->SetToAccountL( EImaCommonSetMailboxId,
        aExtendedMailSettings.MailboxId() );
    iCenRepControl->SetToAccountL( EImaCommonSetAccountId,
        static_cast<TInt>( aExtendedMailSettings.AccountId() ) );
    iCenRepControl->SetToAccountL( EImaCommonSetExtensionId,
        static_cast<TInt>( aExtendedMailSettings.ExtensionId() ) );
    iCenRepControl->SetToAccountL( EImaCommonSetProtocol,
        aExtendedMailSettings.Protocol().iUid );
    iCenRepControl->SetToAccountL( EImaCommonSetEmailAddress,
        *aExtendedMailSettings.EmailAddress() );

    // Extended mailbox settings
    iCenRepControl->SetToAccountL( EImaExtSetEmailNotif,
        static_cast<TInt>( aExtendedMailSettings.EmailNotificationState() ) );
    iCenRepControl->SetToAccountL( EImaExtSetIndicators,
        static_cast<TBool>( aExtendedMailSettings.NewMailIndicators() ) );
    iCenRepControl->SetToAccountL( EImaExtSetHideMsgs,
        static_cast<TInt>( aExtendedMailSettings.HideMsgs() ) );
    iCenRepControl->SetToAccountL( EImaExtSetOpenHtmlMail,
        static_cast<TBool>( aExtendedMailSettings.OpenHtmlMail() ) );
    iCenRepControl->SetToAccountL( EImaExtSetMailDeletion,
        static_cast<TInt>( aExtendedMailSettings.MailDeletion() ) );
    iCenRepControl->SetToAccountL( EImaExtIsPredefined,
        static_cast<TInt>( aExtendedMailSettings.IsPredefined() ) );

    // Always Online Settings
    iCenRepControl->SetToAccountL( EImaAoVersion,
        aExtendedMailSettings.Version() );
    iCenRepControl->SetToAccountL( EImaAoOnlineState,
        aExtendedMailSettings.AlwaysOnlineState() );
    iCenRepControl->SetToAccountL( EImaAoWeekDays,
        aExtendedMailSettings.SelectedWeekDays() );
    iCenRepControl->SetToAccountL( EImaAoTimeStartH, EImaAoTimeStartL,
        aExtendedMailSettings.SelectedTimeStart().Int64() );
    iCenRepControl->SetToAccountL( EImaAoTimeStopH, EImaAoTimeStopL,
        aExtendedMailSettings.SelectedTimeStop().Int64() );
    iCenRepControl->SetToAccountL( EImaAoInboxRefreshTime,
        aExtendedMailSettings.InboxRefreshTime() );
    iCenRepControl->SetToAccountL( EImaAoUpdateMode,
    aExtendedMailSettings.UpdateMode() );

    SaveLastUpdateInfo( aExtendedMailSettings.LastUpdateInfo() );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::SaveSettingsL(
    const TUint32 aAccountId,
    const CImImap4Settings& aImap4Settings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveSettingsL, 0, KLogData );
    IMUM_IN();

    TImapAccount imapAccountId;
    imapAccountId.iImapAccountId = aAccountId;

    AccountsL()->SaveImapSettingsL( imapAccountId, aImap4Settings );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::SaveSettingsL(
    const TUint32 aAccountId,
    const CImPop3Settings& aPop3Settings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveSettingsL, 0, KLogData );
    IMUM_IN();

    TPopAccount popAccountId;
    popAccountId.iPopAccountId = aAccountId;

    AccountsL()->SavePopSettingsL( popAccountId, aPop3Settings );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::SaveSettingsL(
    const TUint32 aAccountId,
    const CImSmtpSettings& aSmtpSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveSettingsL, 0, KLogData );
    IMUM_IN();

    TSmtpAccount smtpAccountId;
    smtpAccountId.iSmtpAccountId = aAccountId;

    AccountsL()->SaveSmtpSettingsL( smtpAccountId, aSmtpSettings );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::SaveSettingsL(
    const TMsvId aMailboxId,
    const CImImap4Settings& aImap4Settings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveSettingsL, 0, KLogData );
    IMUM_IN();

    TImapAccount imapAccountId;
    AccountsL()->GetImapAccountL( aMailboxId, imapAccountId );
    iAccounts->SaveImapSettingsL( imapAccountId, aImap4Settings );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::SaveSettingsL(
    const TMsvId aMailboxId,
    const CImPop3Settings& aPop3Settings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveSettingsL, 0, KLogData );
    IMUM_IN();

    TPopAccount popAccountId;
    AccountsL()->GetPopAccountL( aMailboxId, popAccountId );
    iAccounts->SavePopSettingsL( popAccountId, aPop3Settings );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::SaveSettingsL(
    const TMsvId aMailboxId,
    const CImSmtpSettings& aSmtpSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveSettingsL, 0, KLogData );
    IMUM_IN();

    TSmtpAccount smtpAccountId;
    AccountsL()->GetSmtpAccountL( aMailboxId, smtpAccountId );
    iAccounts->SaveSmtpSettingsL( smtpAccountId, aSmtpSettings );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::SaveSettingsL(
    const TMsvEntry& aEntry,
    CImBaseEmailSettings& aBaseSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveSettingsL, 0, KLogData );
    IMUM_IN();

    // Save the correct settings based on the MTM uid
    switch( aEntry.iMtm.iUid )
        {
        // SMTP
        case KSenduiMtmSmtpUidValue:
            SaveSettingsL( aEntry.Id(),
                *static_cast<CImSmtpSettings*>( &aBaseSettings ) );
            break;

        // POP3
        case KSenduiMtmPop3UidValue:
            SaveSettingsL( aEntry.Id(),
                *static_cast<CImPop3Settings*>( &aBaseSettings )  );
            break;

        // IMAP4
        case KSenduiMtmImap4UidValue:
            SaveSettingsL( aEntry.Id(),
                *static_cast<CImImap4Settings*>( &aBaseSettings )  );
            break;

        default:
            User::Leave( KErrUnknown );
            break;
        }
    IMUM_OUT();
    }

/******************************************************************************

    PRIVATE: LOADING EXISTING ACCOUNT

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadSettings()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::LoadSettingsL(
    const TUint32 aAccountId,
    const TUid& aMtmId,
    CImumMboxDataExtension& aExtendedMailSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadSettingsL, 0, KLogData );
    IMUM_IN();

    // Try to open the settings
    TRAPD( error, OpenExtendedSettingsL(
        TIMAStorerParams( aAccountId, aMtmId ), aExtendedMailSettings ) );

    // It's possible to create the email settings without defining the
    // extended settings. If that is the case, create the extended mailsettings
    // for the mailbox. CMail will handle initialization of extended settings
	// if it's installed.
    if ( !iFFEmailVariation && error == KErrNotFound )
        {
        TMsvId mailboxId = IMASAccountControl::AccountIdToMailboxId(
            *AccountsL(), aAccountId, aMtmId );
        error = CreateSettings(
            mailboxId, aAccountId, aMtmId, aExtendedMailSettings );
        //If AlwaysOnline or EmailNotification is switched on we need to activate it
        if ( error == KErrNone && ( 
        	EMailAoOff  != aExtendedMailSettings.AlwaysOnlineState() || 
            EMailEmnOff != aExtendedMailSettings.EmailNotificationState() ))
			{
			MsvEmailMtmUiUtils::SendAOCommandL( EServerAPIEmailTurnOn, mailboxId );
			}
        }

    User::LeaveIfError( error );
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadSettings()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::LoadSettingsL(
    const TMsvId aMailboxId,
    const TUid& aMtmId,
    CImumMboxDataExtension& aExtendedMailSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadSettingsL, 0, KLogData );
    IMUM_IN();

    // Get the account id and continue opening
    TUint32 accountId =
        IMASAccountControl::MailboxIdToAccountIdL( aMailboxId, aMtmId );
    LoadSettingsL( accountId, aMtmId, aExtendedMailSettings );
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::LoadSettingsL(
    const TUint32 aAccountId,
    CImImap4Settings& aImap4Settings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadSettingsL, 0, KLogData );
    IMUM_IN();

    TImapAccount imapAccountId;
    imapAccountId.iImapAccountId = aAccountId;

    AccountsL()->LoadImapSettingsL( imapAccountId, aImap4Settings );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::LoadSettingsL(
    const TUint32 aAccountId,
    CImPop3Settings& aPop3Settings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadSettingsL, 0, KLogData );
    IMUM_IN();

    TPopAccount popAccountId;
    popAccountId.iPopAccountId = aAccountId;

    AccountsL()->LoadPopSettingsL( popAccountId, aPop3Settings );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadSettingsSmtpL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::LoadSettingsL(
    const TUint32 aAccountId,
    CImSmtpSettings& aSmtpSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadSettingsL, 0, KLogData );
    IMUM_IN();

    TSmtpAccount smtpAccountId;
    smtpAccountId.iSmtpAccountId = aAccountId;

    AccountsL()->LoadSmtpSettingsL( smtpAccountId, aSmtpSettings );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::LoadSettingsL(
    const TMsvId aServiceId,
    CImImap4Settings& aImap4Settings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadSettingsL, 0, KLogData );
    IMUM_IN();

    TImapAccount imapAccountId;
    AccountsL()->GetImapAccountL( aServiceId, imapAccountId );
    iAccounts->LoadImapSettingsL( imapAccountId, aImap4Settings );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::LoadSettingsL(
    const TMsvId aServiceId,
    CImPop3Settings& aPop3Settings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadSettingsL, 0, KLogData );
    IMUM_IN();

    TPopAccount popAccountId;
    AccountsL()->GetPopAccountL( aServiceId, popAccountId );
    iAccounts->LoadPopSettingsL( popAccountId, aPop3Settings );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::LoadSettingsL(
    const TMsvId aServiceId,
    CImSmtpSettings& aSmtpSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadSettingsL, 0, KLogData );
    IMUM_IN();

    TSmtpAccount smtpAccountId;
    AccountsL()->GetSmtpAccountL( aServiceId, smtpAccountId );
    iAccounts->LoadSmtpSettingsL( smtpAccountId, aSmtpSettings );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::LoadSettingsL(
    const TMsvEntry& aEntry,
    CImBaseEmailSettings& aBaseSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadSettingsL, 0, KLogData );
    IMUM_IN();

    // Load the correct settings based on the MTM uid
    switch( aEntry.iMtm.iUid )
        {
        // SMTP
        case KSenduiMtmSmtpUidValue:
            LoadSettingsL( aEntry.Id(),
                *static_cast<CImSmtpSettings*>( &aBaseSettings ) );
            break;

        // POP3
        case KSenduiMtmPop3UidValue:
            LoadSettingsL( aEntry.Id(),
                *static_cast<CImPop3Settings*>( &aBaseSettings )  );
            break;

        // IMAP4
        case KSenduiMtmImap4UidValue:
            LoadSettingsL( aEntry.Id(),
                *static_cast<CImImap4Settings*>( &aBaseSettings )  );
            break;

        default:
            User::Leave( KErrUnknown );
            break;
        }
    IMUM_OUT();
    }

/******************************************************************************

    Special storers

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::CreateLastUpdateInfoL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::CreateLastUpdateInfoL(
    const TAOInfo& aAoInfo )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::CreateLastUpdateInfoL, 0, KLogData );
    IMUM_IN();

    // Create the last update info
    iCenRepControl->CreateToAccountL(
        EImaAoLastSuccessfulUpdateH, EImaAoLastSuccessfulUpdateL,
        aAoInfo.iLastSuccessfulUpdate.Int64() );
    iCenRepControl->CreateToAccountL(
        EImaAoLastUpdateFailed, aAoInfo.iLastUpdateFailed );
    iCenRepControl->CreateToAccountL( EImaAoUpdateSuccessfulWithCurSettings,
        aAoInfo.iUpdateSuccessfulWithCurSettings );
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::DeleteLastUpdateInfo()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::DeleteLastUpdateInfo()
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::DeleteLastUpdateInfo, 0, KLogData );
    IMUM_IN();

    // Save the last update info

    TRAPD( error,
        iCenRepControl->DeleteFromAccountL(
            EImaAoLastSuccessfulUpdateH, EImaAoLastSuccessfulUpdateL );
        iCenRepControl->DeleteFromAccountL( EImaAoLastUpdateFailed );
        iCenRepControl->DeleteFromAccountL(
            EImaAoUpdateSuccessfulWithCurSettings ); );

    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveLastUpdateInfo()
// ----------------------------------------------------------------------------
//
TInt CImumMboxSettingsCtrl::SaveLastUpdateInfo(
    const TAOInfo& aAoInfo )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveLastUpdateInfo, 0, KLogData );
    IMUM_IN();

    // Save the last update info and ignore any error.
    TRAPD( error,
        iCenRepControl->SetToAccountL(
            EImaAoLastSuccessfulUpdateH, EImaAoLastSuccessfulUpdateL,
            aAoInfo.iLastSuccessfulUpdate.Int64() );
        iCenRepControl->SetToAccountL(
            EImaAoLastUpdateFailed, aAoInfo.iLastUpdateFailed  );
        iCenRepControl->SetToAccountL( EImaAoUpdateSuccessfulWithCurSettings,
            aAoInfo.iUpdateSuccessfulWithCurSettings  ); );


    IMUM_OUT();
    return error;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadLastUpdateInfo()
// ----------------------------------------------------------------------------
//
TAOInfo CImumMboxSettingsCtrl::LoadLastUpdateInfo()
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadLastUpdateInfo, 0, KLogData );
    IMUM_IN();

    TAOInfo aoInfo;

    // Failing to load the info can be ignored
    TRAP_IGNORE(
        // Load the last update info
        aoInfo.iLastSuccessfulUpdate = iCenRepControl->GetFromAccountL(
            EImaAoLastSuccessfulUpdateH, EImaAoLastSuccessfulUpdateL );
        aoInfo.iLastUpdateFailed = iCenRepControl->GetFromAccountL(
            EImaAoLastUpdateFailed );
        aoInfo.iUpdateSuccessfulWithCurSettings = iCenRepControl->GetFromAccountL(
            EImaAoUpdateSuccessfulWithCurSettings ); );

    IMUM_OUT();
    return aoInfo;
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::OpenExtendedSettingsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::OpenExtendedSettingsL(
    const TIMAStorerParams& aParams,
    CImumMboxDataExtension& aExtendedMailSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::OpenExtendedSettingsL, 0, KLogData );
    IMUM3_IN( "aParams.iMtmId = 0x%x, aParams.iAccountId = 0x%x, aParams.iMailboxId = 0x%x",
        aParams.MtmId(), aParams.AccountId(), aParams.MailboxId() );

    // Make sure there is connection to repository
    MsvEmailMtmUiUtils::CreateCenRepConnectionL(
        iCenRepExtMail, KCRUidImumSettings );

    // Prepare account control
    iCenRepControl->SetActiveCentralRepository( *iCenRepExtMail );
    iCenRepControl->SetActiveBaseKey( aParams.AccountId(), aParams.MtmId() );

    // Load the settings
    // Common Settings
    LoadExtendedSettingsStatusFlagsL( aExtendedMailSettings );
    TInt value = iCenRepControl->GetFromAccountL( EImaCommonSetMailboxId );
    aExtendedMailSettings.SetMailboxId( static_cast<TMsvId>( value ) );
    value = iCenRepControl->GetFromAccountL( EImaCommonSetAccountId );
    aExtendedMailSettings.SetAccountId( value );
    value = iCenRepControl->GetFromAccountL( EImaCommonSetExtensionId );
    aExtendedMailSettings.SetExtensionId( value );
    value = iCenRepControl->GetFromAccountL( EImaCommonSetProtocol );
    aExtendedMailSettings.SetProtocol( TUid::Uid( value ) );
    TMuiuSettingsText text;
    iCenRepControl->GetTextFromAccountL( text, EImaCommonSetEmailAddress );
    aExtendedMailSettings.SetEmailAddress( text );

    // Common settings are now loaded. If the following settings are found to be 
    // missing, they are populated with the default values during the load 
    // process.
    PopulateDefaultExtSettings( aExtendedMailSettings );
    
    // Extended mailbox settings
    //
    value = iCenRepControl->GetFromAccountOrCreateL( 
        EImaExtSetEmailNotif, aExtendedMailSettings.EmailNotificationState() );
    aExtendedMailSettings.SetEmailNotificationState(
        static_cast<TMailEmnStates>( value ) );
    
    TBool result = iCenRepControl->GetFromAccountOrCreateL( 
        EImaExtSetIndicators, aExtendedMailSettings.NewMailIndicators() );
    aExtendedMailSettings.SetNewMailIndicators( result );
    
    result = iCenRepControl->GetFromAccountOrCreateL( 
        EImaExtSetHideMsgs, aExtendedMailSettings.HideMsgs() );
    aExtendedMailSettings.SetHideMsgs( result );
    
    value = iCenRepControl->GetFromAccountOrCreateL( 
        EImaExtSetOpenHtmlMail, aExtendedMailSettings.OpenHtmlMail() );
    aExtendedMailSettings.SetOpenHtmlMail( value );
    
    value = iCenRepControl->GetFromAccountOrCreateL( 
        EImaExtSetMailDeletion, aExtendedMailSettings.MailDeletion() );
    aExtendedMailSettings.SetMailDeletion(
        static_cast<TIMASMailDeletionMode>( value ) );
    
    result = iCenRepControl->GetFromAccountOrCreateL( 
        EImaExtIsPredefined, aExtendedMailSettings.IsPredefined() );
    aExtendedMailSettings.SetIsPredefined( result );

    // Always Online Settings
    //
    value = iCenRepControl->GetFromAccountOrCreateL( 
        EImaAoOnlineState, aExtendedMailSettings.AlwaysOnlineState() );
    aExtendedMailSettings.SetAlwaysOnlineState(
        static_cast<TMailAoStates>( value ) );

    value = iCenRepControl->GetFromAccountOrCreateL( 
        EImaAoWeekDays, aExtendedMailSettings.SelectedWeekDays() );
    aExtendedMailSettings.SetSelectedWeekDays( value );
    
    TInt64 value64 = iCenRepControl->GetFromAccountOrCreateL(
        EImaAoTimeStartH, EImaAoTimeStartL, 
        aExtendedMailSettings.SelectedTimeStart().Int64() );
    aExtendedMailSettings.SetSelectedTimeStart( value64 );
    
    value64 = iCenRepControl->GetFromAccountOrCreateL(
        EImaAoTimeStopH, EImaAoTimeStopL,
        aExtendedMailSettings.SelectedTimeStop().Int64() );
    aExtendedMailSettings.SetSelectedTimeStop( value64 );
    
    value = iCenRepControl->GetFromAccountOrCreateL( 
        EImaAoInboxRefreshTime, aExtendedMailSettings.InboxRefreshTime() );
    aExtendedMailSettings.SetInboxRefreshTime( value );
    
    value = iCenRepControl->GetFromAccountOrCreateL(
        EImaAoUpdateMode, aExtendedMailSettings.UpdateMode() );
    aExtendedMailSettings.SetUpdateMode(
        static_cast<TMailAoUpdateModes>( value ) );

    aExtendedMailSettings.SetLastUpdateInfo( LoadLastUpdateInfo() );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::AccountsL()
// ----------------------------------------------------------------------------
//
CEmailAccounts* CImumMboxSettingsCtrl::AccountsL()
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::AccountsL, 0, KLogData );
    IMUM_IN();

    if ( !iAccounts )
        {
        iAccounts = CEmailAccounts::NewL();
        }

    IMUM_OUT();
    return iAccounts;
    }


// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadExtendedSettingsStatusFlagsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::LoadExtendedSettingsStatusFlagsL(
    CImumMboxDataExtension& aExtendedSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadExtendedSettingsStatusFlagsL, 0, KLogData );
    IMUM_IN();

    TMuiuFlags flags;
    flags = iCenRepControl->GetFromAccountL(
         EImaCommonSetStatusFlagsH, EImaCommonSetStatusFlagsL );
    aExtendedSettings.SetStatusFlags( flags );
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SaveExtendedSettingsStatusFlagsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::SaveExtendedSettingsStatusFlagsL(
    const CImumMboxDataExtension& aExtendedSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SaveExtendedSettingsStatusFlagsL, 0, KLogData );
    IMUM_IN();

    // Get the flags in correct order from flag class and store to settings
    TInt64 flags = 0;
    const TMuiuFlags muiuFlags = aExtendedSettings.StatusFlags();

    for ( TInt flag = KIMASLastFlag; --flag >= 0; )
        {
        flags |= muiuFlags.Flag( flag ) << flag;
        }

    iCenRepControl->SetToAccountL( EImaCommonSetStatusFlagsH,
        EImaCommonSetStatusFlagsL, flags );
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMboxSettingsCtrl::CreateExtendedSettingsStatusFlagsL()
// ----------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::CreateExtendedSettingsStatusFlagsL(
    const CImumMboxDataExtension& aExtendedSettings )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::CreateExtendedSettingsStatusFlagsL, 0, KLogData );
    IMUM_IN();

    // Get the flags in correct order from flag class and store to settings
    TInt64 flags = 0;
    const TMuiuFlags muiuFlags = aExtendedSettings.StatusFlags();

    for ( TInt flag = KIMASLastFlag; --flag >= 0; )
        {
        flags |= muiuFlags.Flag( flag ) << flag;
        }

    iCenRepControl->CreateToAccountL( EImaCommonSetStatusFlagsH,
        EImaCommonSetStatusFlagsL, flags );
    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumMboxSettingsCtrl::LoadAlwaysOnlineSettingsL()
// ---------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::LoadAlwaysOnlineSettingsL(
    const TUint32 aAccountId,
    const TUid& aMtmId,
    TInt64& aDays,
    TInt64& aStartTime,
    TInt64& aStopTime,
    TInt& aActive )
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::LoadAlwaysOnlineSettingsL, 0, KLogData );
    IMUM_IN();

    // Always Online Settings

    // Make sure there is connection to repository
    MsvEmailMtmUiUtils::CreateCenRepConnectionL(
        iCenRepExtMail, KCRUidImumSettings );

    // Prepare account control
    iCenRepControl->SetActiveCentralRepository( *iCenRepExtMail );
    iCenRepControl->SetActiveBaseKey( aAccountId, aMtmId );

    aActive = iCenRepControl->GetFromAccountL( EImaAoOnlineState );
    aDays = iCenRepControl->GetFromAccountL( EImaAoWeekDays );
    aStartTime = iCenRepControl->GetFromAccountL(
        EImaAoTimeStartH, EImaAoTimeStartL );
    aStopTime = iCenRepControl->GetFromAccountL(
        EImaAoTimeStopH, EImaAoTimeStopL );
    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumMboxSettingsCtrl::CheckMailboxNameImap()
// ---------------------------------------------------------------------------
//
TIMDErrors CImumMboxSettingsCtrl::CheckMailboxNameImap(
	TMsvEntry& aMailboxEntry ) const
	{
	IMUM_CONTEXT( CImumMboxSettingsCtrl::CheckMailboxNameImap, 0, KLogData );
    IMUM_IN();

    //Mailboxname is located in the iDetails variable and can't be over 30
    //This method checks the length of mailboxname, if it is over 30
    //we cut off the mailboxname
    TIMDErrors result = EIMDNoError;
	if( aMailboxEntry.iDetails.Length() > KIMASMaxMailboxNameLength )
		{
		TRAPD( error, FixMailboxNameImapL( aMailboxEntry ) );
        if( error )
            {
            // all errors cause account failure
            result = EIMDImap4AccountFailure;
            }
		}

	IMUM_OUT();
    return result;
	}

// ---------------------------------------------------------------------------
// CImumMboxSettingsCtrl::CheckMailboxNamePop()
// ---------------------------------------------------------------------------
//
TIMDErrors CImumMboxSettingsCtrl::CheckMailboxNamePop(
	TMsvEntry& aMailboxEntry ) const
	{
	IMUM_CONTEXT( CImumMboxSettingsCtrl::CheckMailboxNamePop, 0, KLogData );
    IMUM_IN();

	//Mailboxname is located in the iDetails variable and can't be over 30
    //This method checks the length of mailboxname, if it is over 30
    //we cut off the mailboxname
    TIMDErrors result = EIMDNoError;
	if( aMailboxEntry.iDetails.Length() > KIMASMaxMailboxNameLength )
		{
	    TRAPD( error, FixMailboxNamePopL( aMailboxEntry ));
	    if( error )
	        {
            // all errors cause account failure
	        result = EIMDPop3AccountFailure;
	        }
	    }
	IMUM_OUT();
    return result;
	}

// ---------------------------------------------------------------------------
// CImumMboxSettingsCtrl::CheckMailboxNameSmtp()
// ---------------------------------------------------------------------------
//
TIMDErrors CImumMboxSettingsCtrl::CheckMailboxNameSmtp(
	TMsvEntry& aMailboxEntry ) const
	{
	IMUM_CONTEXT( CImumMboxSettingsCtrl::CheckMailboxNameSmtp, 0, KLogData );
    IMUM_IN();

    //Mailboxname is located in the iDetails variable and can't be over 30
    //This method checks the length of mailboxname, if it is over 30
    //we cut off the mailboxname
    TIMDErrors result = EIMDNoError;
	if ( aMailboxEntry.iDetails.Length() > KIMASMaxMailboxNameLength )
	    {
    	TRAPD( error, FixMailboxNameSmtpL( aMailboxEntry ) )
    	if( error )
    	    {
    	    // all errors cause account failure
    	    result = EIMDSmtpAccountFailure;
    	    }
	    }

	IMUM_OUT();
	return result;
	}

// ---------------------------------------------------------------------------
// CImumMboxSettingsCtrl::SetLastUpdateInfoL()
// ---------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::SetLastUpdateInfoL( TUint32 aAccountId,
    const TUid& aMtmId, TBool aSuccess, const TTime& aTime )
    {
    // This function is performance optimization as settings are
    // not converted to CImumInSettingsData format.
    IMUM_CONTEXT( CImumMboxSettingsCtrl::SetLastUpdateInfoL, 0, KLogData );

    // Make sure there is connection to repository
    MsvEmailMtmUiUtils::CreateCenRepConnectionL(
        iCenRepExtMail, KCRUidImumSettings );

    // Prepare account control
    iCenRepControl->SetActiveCentralRepository( *iCenRepExtMail );
    iCenRepControl->SetActiveBaseKey( aAccountId, aMtmId );

    TAOInfo taoInfo;
    taoInfo.iLastUpdateFailed = aSuccess;
    taoInfo.iLastSuccessfulUpdate = aTime;
    taoInfo.iUpdateSuccessfulWithCurSettings = aSuccess;
    SaveLastUpdateInfo( taoInfo );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumMboxSettingsCtrl::FixMailboxNameSmtpL()
// ---------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::FixMailboxNameSmtpL(
    TMsvEntry& aMailboxEntry ) const
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::FixMailboxNameStmpL, 0, KLogData );

    // get settings
    TSmtpAccount smtpAccount;
    iMailboxApi.EmailAccountsL().GetSmtpAccountL(
        aMailboxEntry.Id(), smtpAccount );
    CImSmtpSettings *smtpSet = new (ELeave) CImSmtpSettings;
    CleanupStack::PushL( smtpSet );
    iMailboxApi.EmailAccountsL().LoadSmtpSettingsL( smtpAccount, *smtpSet );

    // shorten mailbox name
    TPtrC16 ptrSmtp = smtpAccount.iSmtpAccountName.Left(
        KIMASMaxMailboxNameLength );
    smtpAccount.iSmtpAccountName.Copy( ptrSmtp );

    // save settings
    iMailboxApi.EmailAccountsL().SaveSmtpSettingsL( smtpAccount, *smtpSet );
    CleanupStack::PopAndDestroy( smtpSet );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumMboxSettingsCtrl::FixMailboxNameImapL()
// ---------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::FixMailboxNameImapL(
    TMsvEntry& aMailboxEntry ) const
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::FixMailboxNameImapL, 0, KLogData );

    // get settings
    TImapAccount imapAccount;
    iMailboxApi.EmailAccountsL().GetImapAccountL(
        aMailboxEntry.Id(), imapAccount );
    CImImap4Settings *imap4Set = new (ELeave) CImImap4Settings;
    CleanupStack::PushL( imap4Set );
    iMailboxApi.EmailAccountsL().LoadImapSettingsL( imapAccount, *imap4Set );

    // shorten mailbox name
    TPtrC16 ptrImap = imapAccount.iImapAccountName.Left(
        KIMASMaxMailboxNameLength);
    imapAccount.iImapAccountName.Copy( ptrImap );

    // store changes
    iMailboxApi.EmailAccountsL().SaveImapSettingsL( imapAccount, *imap4Set );
    CleanupStack::PopAndDestroy(imap4Set);

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumMboxSettingsCtrl::FixMailboxNamePopL()
// ---------------------------------------------------------------------------
//
void CImumMboxSettingsCtrl::FixMailboxNamePopL(
    TMsvEntry& aMailboxEntry ) const
    {
    IMUM_CONTEXT( CImumMboxSettingsCtrl::FixMailboxNamePopL, 0, KLogData );

    // get settings
    TPopAccount popAccount;
    iMailboxApi.EmailAccountsL().GetPopAccountL(
        aMailboxEntry.Id(), popAccount );
    CImPop3Settings *popSet = new (ELeave) CImPop3Settings;
    CleanupStack::PushL( popSet );
    iMailboxApi.EmailAccountsL().LoadPopSettingsL( popAccount, *popSet );

    // shorten mailbox name
    TPtrC16 ptrPop = popAccount.iPopAccountName.Left(
        KIMASMaxMailboxNameLength);
    popAccount.iPopAccountName.Copy( ptrPop );

    // store changes
    iMailboxApi.EmailAccountsL().SavePopSettingsL( popAccount, *popSet );
    CleanupStack::PopAndDestroy( popSet );

    IMUM_OUT();
    }

//  End of File

