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
* Description:  ImumMtmBaseMtmUi.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <ImumInMailboxServices.h>
#include <ConeResLoader.h>

#include "EmailFeatureUtils.h"
#include "ImumMtmBaseMtmUi.h"
#include "ImumMboxSettingsUtils.h"
#include "ImumInSettingsKeys.h"
#include "ImumPanic.h"
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

// ================ CONSTRUCTING ================

// ----------------------------------------------------------------------------
// CImumMtmBaseMtmUi::CImumMtmBaseMtmUi()
// ----------------------------------------------------------------------------
//
CImumMtmBaseMtmUi::CImumMtmBaseMtmUi(
    CBaseMtm& aBaseMtm,
    CRegisteredMtmDll& aRegisteredMtmDll )
    :
    CBaseMtmUi( aBaseMtm, aRegisteredMtmDll ),
    iMailboxApi( NULL ),
    iUtils( NULL ),
    iMsvSession( NULL ),
    iErrorResolver( NULL ),
    iFeatureFlags( NULL )
    {
    IMUM_CONTEXT( CImumMtmBaseMtmUi::CImumMtmBaseMtmUi, 0, KImumMtmLog );
    IMUM_IN();

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMtmBaseMtmUi::~CImumMtmBaseMtmUi()
// ----------------------------------------------------------------------------
//
CImumMtmBaseMtmUi::~CImumMtmBaseMtmUi()
    {
    IMUM_CONTEXT( CImumMtmBaseMtmUi::~CImumMtmBaseMtmUi, 0, KImumMtmLog );
    IMUM_IN();

    delete iMailboxApi;
    iMailboxApi = NULL;
    delete iErrorResolver;
    iErrorResolver = NULL;
    delete iFeatureFlags;
    iFeatureFlags = NULL;
    iMsvSession = NULL;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMtmBaseMtmUi::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumMtmBaseMtmUi::ConstructL()
    {
    IMUM_CONTEXT( CImumMtmBaseMtmUi::ConstructL, 0, KImumMtmLog );
    IMUM_IN();

    // --- Make sure base class correctly constructed ---
    CBaseMtmUi::ConstructL();

    // Construct own base MTM UI
    iMsvSession = &Session();
    iMailboxApi = CreateEmailApiL( iMsvSession );
    iErrorResolver = CErrorUI::NewL(*iCoeEnv);
    iFeatureFlags = MsvEmailMtmUiFeatureUtils::EmailFeaturesL( ETrue, ETrue );
    iUtils = const_cast<MImumInMailboxUtilities*>(
        &iMailboxApi->MailboxUtilitiesL() );

    IMUM_OUT();
    }

// ================ GENERAL MTM UI FUNCTIONALITY ================

// ----------------------------------------------------------------------------
// CImumMtmBaseMtmUi::AcknowledgeReceiptsL()
// ----------------------------------------------------------------------------
//
TBool CImumMtmBaseMtmUi::AcknowledgeReceiptsL(
    const TMsvId aMessageId,
    const MImumInMailboxUtilities::TImumInMboxRequest& aRequestType ) const
    {
    IMUM_CONTEXT( CImumMtmBaseMtmUi::AcknowledgeReceiptsL, 0, KImumMtmLog );
    IMUM_IN();

    // The Id provided into this function is the id of the email message.
    // To get the settings, the owner box of the email needs to be found,
    // using the service id. After the mailbox is found, the settings can
    // be retrieved to get the setting

    // First, get the entry of the email
    TMsvEntry email;
    TMsvId service;
    User::LeaveIfError( iMsvSession->GetEntry( aMessageId, service, email ) );

    // Next, get the entry of the owner mailbox
    TMsvEntry mailbox = iUtils->GetMailboxEntryL(
        email.iServiceId,
        aRequestType );

    // Then read the settings of the mailbox
    CImumInSettingsData* accountSettings =
        iMailboxApi->MailboxServicesL().LoadMailboxSettingsL( mailbox.Id() );
    CleanupStack::PushL( accountSettings );

    // Finally, retrieve the setting
    TInt ackReceipts = ImumMboxSettingsUtils::QuickGetL<TInt>(
        *accountSettings,
        TImumInSettings::EKeyAcknowledgeReceipts );

    CleanupStack::PopAndDestroy( accountSettings );
    accountSettings = NULL;
    IMUM_OUT();

    return static_cast<TBool>( ackReceipts );
    }


// ----------------------------------------------------------------------------
// CImumMtmBaseMtmUi::GetMailDeletionModeL()
// ----------------------------------------------------------------------------
//
TInt CImumMtmBaseMtmUi::GetMailDeletionModeL() const
    {
    IMUM_CONTEXT( CImumMtmBaseMtmUi::GetMailDeletionModeL, 0, KImumMtmLog );
    IMUM_IN();

    // Settings are stored in the CenRep, read the data from the system
    TMsvId mboxId = BaseMtm().Entry().Entry().iServiceId;
    IMUM2(0, "Loading mailbox settings from iMailboxApi (0x%x) with Id 0x%x", iMailboxApi, mboxId );

    CImumInSettingsData* settings =
        iMailboxApi->MailboxServicesL().LoadMailboxSettingsL( mboxId );
    CleanupStack::PushL( settings );

    IMUM1( 0, "Settings data object at 0x%x", settings );

    // Mailbox settings contain the deletion mode, so read the data
    TInt mode = TImumDaSettings::EValueDeletionAlwaysAsk;
    settings->GetAttr( TImumDaSettings::EKeyMailDeletion, mode );

    IMUM1(0, "TInt mode( %d )", mode );

    CleanupStack::PopAndDestroy( settings );
    settings = NULL;

    IMUM_OUT();
    return mode;
    }


// ---------------------------------------------------------------------------
// From class CBaseMtmUi.
// CImumMtmBaseMtmUi::DeleteServiceL()
// ---------------------------------------------------------------------------
//
CMsvOperation* CImumMtmBaseMtmUi::DeleteServiceL(
    const TMsvEntry& aService,
    TRequestStatus& aStatus )
    {
    IMUM_CONTEXT( CImumMtmBaseMtmUi::DeleteServiceL, 0, KImumMtmLog );
    IMUM_IN();

    __ASSERT_DEBUG( aService.iMtm == Type(),
        User::Panic( KImumMtmUiPanic, ESmtpMtmUiWrongMtm ) );

    CImumInternalApi* emailApi = CreateEmailApiL( &Session() );
    CleanupStack::PushL( emailApi );
    CMsvOperation* op = emailApi->MailboxServicesL().RemoveMailboxL( aService.Id(), aStatus );
    CleanupStack::PopAndDestroy( emailApi );
    emailApi = NULL;

    IMUM_OUT();

    return op;
    }

// End of File
