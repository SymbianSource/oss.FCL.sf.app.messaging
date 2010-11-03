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
*       Class implementation file
*
*/


// INCLUDES
#include <smtpset.h>                // CImSmtpSettings
#include <pop3set.h>                // CImPop3Settings
#include <imapset.h>                // CImImap4Settings
#include <iapprefs.h>               // CImIAPPreferences
#include <txtrich.h>

#include "ImumMboxData.h"
#include "ImumMboxDataExtension.h"
#include "ImumUtilsLogging.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
const TUint32 KServiceIdNoServiceSpecified = 0U;
_LIT( KImasPanicString, "IMAS" );

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// ImasPanic()
// ----------------------------------------------------------------------------
GLDEF_C void ImasPanic(TImumImasPanic aCode)
    {
    User::Panic( KImasPanicString, aCode );
    }

// ----------------------------------------------------------------------------
// CMsvImailSignature::CMsvImailSignature()
// ----------------------------------------------------------------------------
CMsvImailSignature::CMsvImailSignature()
    {
    IMUM_CONTEXT( CMsvImailSignature::CMsvImailSignature, 0, KLogData );
    
    }

// ----------------------------------------------------------------------------
// CMsvImailSignature::CMsvImailSignature()
// ----------------------------------------------------------------------------
CMsvImailSignature::CMsvImailSignature(
    CRichText*        aRichText,
    CParaFormatLayer* aParaFormat,
    CCharFormatLayer* aCharFormat )
    :
    iRichText( aRichText ),
    iParaFormatLayer( aParaFormat ),
    iCharFormatLayer( aCharFormat )
    {
    IMUM_CONTEXT( CMsvImailSignature::CMsvImailSignature, 0, KLogData );
    
    }

// ----------------------------------------------------------------------------
// CMsvImailSignature::~CMsvImailSignature()
// ----------------------------------------------------------------------------
CMsvImailSignature::~CMsvImailSignature()
    {
    IMUM_CONTEXT( CMsvImailSignature::~CMsvImailSignature, 0, KLogData );
    
    delete iRichText;
    delete iParaFormatLayer;
    delete iCharFormatLayer;
    }

// ----------------------------------------------------------------------------
// CMsvImailSignature::CopyL()
// ----------------------------------------------------------------------------
void CMsvImailSignature::CopyL( const CMsvImailSignature& aSignature )
    {
    IMUM_CONTEXT( CMsvImailSignature::CopyL, 0, KLogData );
    
    delete iRichText;
    iRichText = NULL;

    delete iParaFormatLayer;
    iParaFormatLayer = NULL;

    delete iCharFormatLayer;
    iCharFormatLayer = NULL;

    if ( aSignature.iParaFormatLayer )
        {
        iParaFormatLayer = aSignature.iParaFormatLayer->CloneL();
        }
    if ( aSignature.iCharFormatLayer )
        {
        iCharFormatLayer = aSignature.iCharFormatLayer->CloneL();
        }
    if ( aSignature.iRichText )
        {
        __ASSERT_DEBUG(
            iParaFormatLayer && iCharFormatLayer,
            ImasPanic( EImasPanicInvalidSignatureSource ) );
        iRichText = CRichText::NewL( iParaFormatLayer, iCharFormatLayer );
        iRichText->AppendTakingSolePictureOwnershipL( *aSignature.iRichText );
        }
    }

// ----------------------------------------------------------------------------
// CMsvImailSignature::CreateEmptyRichTextL()
// ----------------------------------------------------------------------------
void CMsvImailSignature::CreateEmptyRichTextL()
    {
    IMUM_CONTEXT( CMsvImailSignature::CreateEmptyRichTextL, 0, KLogData );
    
    delete iRichText;
    iRichText = NULL;

    delete iParaFormatLayer;
    iParaFormatLayer = NULL;

    delete iCharFormatLayer;
    iCharFormatLayer = NULL;

    iParaFormatLayer = CParaFormatLayer::NewL();
    iCharFormatLayer = CCharFormatLayer::NewL();
    iRichText = CRichText::NewL( iParaFormatLayer, iCharFormatLayer );
    }

///////////////////////////////////////////////////////////////////////////////
// CImumMboxData
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// CImumMboxData::NewL()
// ----------------------------------------------------------------------------
//
CImumMboxData* CImumMboxData::NewL()
    {
    IMUM_STATIC_CONTEXT( CImumMboxData::NewL, 0, utils, KLogData );
    
    CImumMboxData* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CImumMboxData::NewLC()
// ----------------------------------------------------------------------------
//
CImumMboxData* CImumMboxData::NewLC()
    {
    IMUM_STATIC_CONTEXT( CImumMboxData::NewLC, 0, utils, KLogData );
    
    CImumMboxData* self =
       new ( ELeave ) CImumMboxData();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CImumMboxData::NewL()
// ----------------------------------------------------------------------------
//
CImumMboxData* CImumMboxData::NewL(
    const TDesC& aName,
    CImSmtpSettings* aSmtpSettings,
    CImPop3Settings* aPop3Settings,
    CImumMboxDataExtension* aExtendedSettings )
    {
    IMUM_STATIC_CONTEXT( CImumMboxData::NewL, 0, utils, KLogData );
    
    CImumMboxData* self = NewLC(
        aName, aSmtpSettings, aPop3Settings, aExtendedSettings );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CImumMboxData::NewLC()
// ----------------------------------------------------------------------------
//
CImumMboxData* CImumMboxData::NewLC(
    const TDesC& aName,
    CImSmtpSettings* aSmtpSettings,
    CImPop3Settings* aPop3Settings,
    CImumMboxDataExtension* aExtendedSettings )
    {
    IMUM_STATIC_CONTEXT( CImumMboxData::NewLC, 0, utils, KLogData );
    
    CImumMboxData* self =
        new ( ELeave ) CImumMboxData(
            aName, aSmtpSettings, aPop3Settings, NULL, aExtendedSettings );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CImumMboxData::NewL()
// ----------------------------------------------------------------------------
//
CImumMboxData* CImumMboxData::NewL(
    const TDesC& aName,
    CImSmtpSettings* aSmtpSettings,
    CImImap4Settings* aImap4Settings,
    CImumMboxDataExtension* aExtendedSettings )
    {
    IMUM_STATIC_CONTEXT( CImumMboxData::NewL, 0, utils, KLogData );
    
    CImumMboxData* self = NewLC(
        aName, aSmtpSettings, aImap4Settings, aExtendedSettings );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CImumMboxData::NewLC()
// ----------------------------------------------------------------------------
//
CImumMboxData* CImumMboxData::NewLC(
    const TDesC& aName,
    CImSmtpSettings* aSmtpSettings,
    CImImap4Settings* aImap4Settings,
    CImumMboxDataExtension* aExtendedSettings )
    {
    IMUM_STATIC_CONTEXT( CImumMboxData::NewLC, 0, utils, KLogData );
    
    CImumMboxData* self =
        new ( ELeave ) CImumMboxData(
            aName, aSmtpSettings, NULL, aImap4Settings, aExtendedSettings );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CImumMboxData::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumMboxData::ConstructL()
    {
    IMUM_CONTEXT( CImumMboxData::ConstructL, 0, KLogData );
    
    InitializeL();
    }

// ----------------------------------------------------------------------------
// CImumMboxData::InitializeL()
// ----------------------------------------------------------------------------
//
void CImumMboxData::InitializeL()
    {
    IMUM_CONTEXT( CImumMboxData::InitializeL, 0, KLogData );
    
        // Create signature
    if ( !iSignature )
        {
        iSignature = new ( ELeave ) CMsvImailSignature;
        iSignature->CreateEmptyRichTextL();
        }

    // Create the base settings
    if ( !iSmtpSettings )
        {
        iSmtpSettings = new ( ELeave ) CImSmtpSettings;
        }

    if ( !iPop3Settings )
        {
        iPop3Settings = new ( ELeave ) CImPop3Settings;
        }

    if ( !iImap4Settings )
        {
        iImap4Settings = new ( ELeave ) CImImap4Settings;
        }

    // Create IAP preferences separately to both
    // incoming and outgoing protocols
    if ( !iIncomingIapPref )
        {
        iIncomingIapPref = CImIAPPreferences::NewLC();
        CleanupStack::Pop( iIncomingIapPref );
        }

    if ( !iOutgoingIapPref )
        {
        iOutgoingIapPref = CImIAPPreferences::NewLC();
        CleanupStack::Pop( iOutgoingIapPref );
        }

    if ( !iExtendedSettings  )
        {
        iExtendedSettings = CImumMboxDataExtension::NewL();
        }
    }

// ----------------------------------------------------------------------------
// CImumMboxData::CImumMboxData()
// ----------------------------------------------------------------------------
CImumMboxData::CImumMboxData()
    :
    iIsImap4( EFalse ),
    iSignature( NULL ),
    iMailboxId( KMsvNullIndexEntryId ),
    iDefaultAccountId( KMsvNullIndexEntryIdValue ),
    iImap4Settings( NULL ),
    iPop3Settings( NULL ),
    iSmtpSettings( NULL ),
    iExtendedSettings( NULL ),
    iIncomingIap( KServiceIdNoServiceSpecified ),
    iIncomingDialogPref( ECommDbDialogPrefDoNotPrompt ),
    iIncomingIapPref( NULL ),
    iOutgoingIap( KServiceIdNoServiceSpecified ),
    iOutgoingDialogPref( ECommDbDialogPrefDoNotPrompt ),
    iOutgoingIapPref( NULL )
    {
    IMUM_CONTEXT( CImumMboxData::CImumMboxData, 0, KLogData );
    
    }

// ----------------------------------------------------------------------------
// CImumMboxData::CImumMboxData()
// ----------------------------------------------------------------------------
CImumMboxData::CImumMboxData(
    const TDesC& aName,
    CImSmtpSettings* aSmtpSettings,
    CImPop3Settings* aPop3Settings,
    CImImap4Settings* aImap4Settings,
    CImumMboxDataExtension* aExtendedSettings )
    :
    iIsImap4( EFalse ),
    iSignature( NULL ),
    iMailboxId( KMsvNullIndexEntryId ),
    iDefaultAccountId( KMsvNullIndexEntryIdValue ),
    iImap4Settings( aImap4Settings ),
    iPop3Settings( aPop3Settings ),
    iSmtpSettings( aSmtpSettings ),
    iExtendedSettings( aExtendedSettings ),
    iIncomingIap ( KServiceIdNoServiceSpecified ),
    iIncomingIapPref( NULL ),
    iOutgoingIap ( KServiceIdNoServiceSpecified ),
    iOutgoingIapPref( NULL )
    {
    IMUM_CONTEXT( CImumMboxData::CImumMboxData, 0, KLogData );
    
    iName.Copy( aName );
    iImap4AccountId.iImapAccountId = KErrNotFound;
    iPop3AccountId.iPopAccountId = KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CImumMboxData::~CImumMboxData()
// ----------------------------------------------------------------------------
CImumMboxData::~CImumMboxData()
    {
    IMUM_CONTEXT( CImumMboxData::~CImumMboxData, 0, KLogData );
    
    delete iSignature;
    iSignature = NULL;
    delete iSmtpSettings;
    iSmtpSettings = NULL;
    delete iPop3Settings;
    iPop3Settings = NULL;
    delete iImap4Settings;
    iImap4Settings = NULL;
    delete iIncomingIapPref;
    iIncomingIapPref = NULL;
    delete iOutgoingIapPref;
    iOutgoingIapPref = NULL;
    delete iExtendedSettings;
    iExtendedSettings = NULL;
    }

// ----------------------------------------------------------------------------
// CImumMboxData::CopyL()
// ----------------------------------------------------------------------------
void CImumMboxData::CopyL(
    const CImumMboxData& aSettings )
    {
    IMUM_CONTEXT( CImumMboxData::CopyL, 0, KLogData );
    
    iName.Copy( aSettings.iName );
    iIsImap4 = aSettings.iIsImap4;
    iMailboxId = aSettings.iMailboxId;
    iDefaultAccountId = aSettings.iDefaultAccountId;

    // Remove any old settings
    delete iSignature;
    iSignature = NULL;
    delete iSmtpSettings;
    iSmtpSettings = NULL;
    delete iPop3Settings;
    iPop3Settings = NULL;
    delete iImap4Settings;
    iImap4Settings = NULL;
    delete iIncomingIapPref;
    iIncomingIapPref = NULL;
    delete iOutgoingIapPref;
    iOutgoingIapPref = NULL;
    delete iExtendedSettings;
    iExtendedSettings = NULL;

    // Mailbox settings
    iSmtpSettings = new ( ELeave ) CImSmtpSettings;
    iSmtpSettings->CopyL( *aSettings.iSmtpSettings );

    iPop3Settings = new ( ELeave ) CImPop3Settings;
    iPop3Settings->CopyL( *aSettings.iPop3Settings );

    iImap4Settings = new ( ELeave ) CImImap4Settings;
    iImap4Settings->CopyL( *aSettings.iImap4Settings );

    iExtendedSettings = CImumMboxDataExtension::NewL();
    iExtendedSettings->CopyL( *aSettings.iExtendedSettings );

    iIncomingIapPref = CImIAPPreferences::NewLC();
    CleanupStack::Pop( iIncomingIapPref );

    iOutgoingIapPref = CImIAPPreferences::NewLC();
    CleanupStack::Pop( iOutgoingIapPref );

    iSignature = new ( ELeave ) CMsvImailSignature;
    iSignature->CopyL( *aSettings.iSignature );

    // Incoming IAP preferences
    iIncomingIap = aSettings.iIncomingIap;

    TInt count = aSettings.iIncomingIapPref->NumberOfIAPs();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        iIncomingIapPref->AddIAPL(
            aSettings.iIncomingIapPref->IAPPreference( loop ) );
        }

    // Outgoing IAP preferences
    iOutgoingIap = aSettings.iOutgoingIap;

    count = aSettings.iOutgoingIapPref->NumberOfIAPs();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        iOutgoingIapPref->AddIAPL(
            aSettings.iOutgoingIapPref->IAPPreference( loop ) );
        }
    }

// ----------------------------------------------------------------------------
// CImumMboxData::RelatedMtmSettings()
// ----------------------------------------------------------------------------
CImBaseEmailSettings& CImumMboxData::RelatedMtmSettings() const
    {
    IMUM_CONTEXT( CImumMboxData::RelatedMtmSettings, 0, KLogData );
    
    CImBaseEmailSettings* settings = NULL;

    if( iIsImap4 )
        {
        settings = iImap4Settings;
        }
    else
        {
        settings = iPop3Settings;
        }

    __ASSERT_DEBUG( settings, ImasPanic( EImasPanicNoSettings ) );
    return *settings;
    }

// ----------------------------------------------------------------------------
// CImumMboxData::Clean()
// ----------------------------------------------------------------------------
//
void CImumMboxData::Clean()
    {
    IMUM_CONTEXT( CImumMboxData::Clean, 0, KLogData );
    
    delete iSignature;
    iSignature = NULL;
    delete iSmtpSettings;
    iSmtpSettings = NULL;
    delete iPop3Settings;
    iPop3Settings = NULL;
    delete iImap4Settings;
    iImap4Settings = NULL;
    delete iIncomingIapPref;
    iIncomingIapPref = NULL;
    delete iOutgoingIapPref;
    iOutgoingIapPref = NULL;
    delete iExtendedSettings;
    iExtendedSettings = NULL;
    
    iIsImap4 = EFalse;
    iName.Zero();
    iMailboxId = KMsvNullIndexEntryId;
    }

// ----------------------------------------------------------------------------
// CImumMboxData::IsOk()
// ----------------------------------------------------------------------------
//
TBool CImumMboxData::IsOk() const
    {
    IMUM_CONTEXT( CImumMboxData::IsOk, 0, KLogData );
    
    if ( iSignature && iSmtpSettings && iIncomingIapPref &&
         iOutgoingIapPref && iExtendedSettings &&
         ( iPop3Settings || iImap4Settings ) )
        {
        return ETrue;
        }

    return EFalse;
    }

// End of File
