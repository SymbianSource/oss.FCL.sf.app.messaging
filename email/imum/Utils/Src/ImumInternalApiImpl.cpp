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
* Description:  ImumInternalApiImpl.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <msvapi.h>                     // CMsvSession
#include <MuiuMsvUiServiceUtilities.h>  // MsvUiServiceUtilities
#include <cemailaccounts.h>             // CEmailAccounts
#include <ImumInternalApi.h>

#include "ImumDaErrorCodes.h"
#include "EmailUtils.H"
#include "IMSSettingsNoteUi.h"
#include "ImumInternalApiImpl.h"
#include "ImumInHealthServicesImpl.h"
#include "ImumInMailboxUtilitiesImpl.h"
#include "ImumInMailboxServicesImpl.h"
#include "ImumMboxManager.h"
#include "EmailFeatureUtils.h"
#include "ImumUtilsLogging.h"
#include "ComDbUtl.h"                   // CMsvCommDbUtilities
#include "ImumPanic.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ LOCAL FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CreateEmailApiL()
// ----------------------------------------------------------------------------
//
EXPORT_C CImumInternalApi* CreateEmailApiL( CMsvSession* aMsvSession )
    {
    IMUM_STATIC_CONTEXT( CreateEmailApiL, 0, utils, KLogInApi );
    IMUM1_IN( "aMsvSession: 0x%x", aMsvSession );
    IMUM_OUT();

    return CImumInternalApiImpl::NewL( aMsvSession );
    }

// ----------------------------------------------------------------------------
// CreateEmailApiLC()
// ----------------------------------------------------------------------------
//
EXPORT_C CImumInternalApi* CreateEmailApiLC( CMsvSession* aMsvSession )
    {
    IMUM_STATIC_CONTEXT( CreateEmailApiLC, 0, utils, KLogInApi );
    IMUM1_IN( "aMsvSession: 0x%x", aMsvSession );
    IMUM_OUT();

    return CImumInternalApiImpl::NewLC( aMsvSession );
    }

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CImumInternalApiImpl::NewL()
// ----------------------------------------------------------------------------
//
CImumInternalApiImpl* CImumInternalApiImpl::NewL( CMsvSession* aMsvSession )
    {
    IMUM_STATIC_CONTEXT( CImumInternalApiImpl::NewL, 0, utils, KLogInApi );
    IMUM1_IN( "aMsvSession: 0x%x", aMsvSession );

    CImumInternalApiImpl* self = NewLC( aMsvSession );
    CleanupStack::Pop( self );

    IMUM_OUT();

    return self;
    }

// ----------------------------------------------------------------------------
// CImumInternalApiImpl::NewLC()
// ----------------------------------------------------------------------------
//
CImumInternalApiImpl* CImumInternalApiImpl::NewLC( CMsvSession* aMsvSession )
    {
    IMUM_STATIC_CONTEXT( CImumInternalApiImpl::NewLC, 0, utils, KLogInApi );
    IMUM1_IN( "aMsvSession: 0x%x", aMsvSession );

    CImumInternalApiImpl* self =
        new ( ELeave ) CImumInternalApiImpl( aMsvSession );
    CleanupStack::PushL( self );
    self->ConstructL();

    IMUM_OUT();

    return self;
    }

// ----------------------------------------------------------------------------
// CImumInternalApiImpl::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumInternalApiImpl::ConstructL()
    {
    IMUM_CONTEXT( CImumInternalApiImpl::ConstructL, 0, KLogInApi );
    IMUM_IN();

    // Retrieve all the feature flags
    iFlags = MsvEmailMtmUiFeatureUtils::EmailFeaturesL( ETrue, ETrue );

    // Make sure the message server session is open
    CreateMsvSession();

    CCoeEnv* env = CCoeEnv::Static();

    //Resourceloader won't be needed if CCoeEnv not present
    if( env )
        {
        iFlags->SetFlag( EHasUi );
        iResourceLoader = new (ELeave) RConeResourceLoader( *env );
        MsvEmailMtmUiUtils::LoadResourceFileL( *iResourceLoader );
        }

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInternalApiImpl::CImumInternalApiImpl()
// ----------------------------------------------------------------------------
//
CImumInternalApiImpl::CImumInternalApiImpl( CMsvSession* aMsvSession )
    :
    iFlags( NULL ),
    iMsvSession( aMsvSession ),
    iMailboxManager( NULL ),
    iHealthServices( NULL ),
    iMboxSettingsCtrl( NULL ),
    iAccounts( NULL ),
    iMailboxUtilities( NULL ),
    iMailboxServices( NULL ),
    iResourceLoader( NULL )
    {
    IMUM_CONTEXT( CImumInternalApiImpl::CImumInternalApiImpl, 0, KLogInApi );
    IMUM1_IN( "aMsvSession: 0x%x", aMsvSession );
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInternalApiImpl::
// ----------------------------------------------------------------------------
//
CImumInternalApiImpl::~CImumInternalApiImpl()
    {
    IMUM_CONTEXT( CImumInternalApiImpl::~CImumInternalApiImpl, 0, KLogInApi );
    IMUM_IN();

    // Remove the observer here since incoming events are not interesting at
    // this point
    if ( iMsvSession )
        {
        iMsvSession->RemoveObserver( *this );
        }

    if ( iResourceLoader )
        {
        iResourceLoader->Close();
        delete iResourceLoader;
        }
    iResourceLoader = NULL;

    delete iMailboxManager;
    delete iHealthServices;
    delete iMboxSettingsCtrl;
    delete iAccounts;
    delete iMailboxUtilities;
    delete iMailboxServices;
    delete iCommDbUtils;

    // Destroy the session here since there is a chance that some of the
    // above objects may have used this session to create objects, so now
    // they can do cleanup in peace.
    if ( iFlags && iFlags->Flag( ESessionOwned ) )
        {
        delete iMsvSession;
        }
    delete iFlags;

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumInternalApiImpl::ServiceExceptionL()
// ---------------------------------------------------------------------------
//
void CImumInternalApiImpl::ServiceExceptionL(
    const TInt aErrorCode ) const
    {
    IMUM_CONTEXT( CImumInternalApiImpl::ServiceExceptionL, 0, KLogInApi );
    IMUM_IN();

    IMUM1( 0, "*!!!!!* EXCEPTION %d *!!!!!*", aErrorCode );

    __ASSERT_DEBUG( EFalse,
        User::Panic( KImumInternalApi, aErrorCode ) );

    IMUM_OUT();
    User::Leave( aErrorCode );
    }

/******************************************************************************

    Get Methods

******************************************************************************/

void CImumInternalApiImpl::Reset()
    {
    IMUM_CONTEXT( CImumInternalApiImpl::Reset, 0, KLogInApi );
    IMUM_IN();

    delete iMailboxManager;
    iMailboxManager = NULL;
    delete iHealthServices;
    iHealthServices = NULL;
    delete iMboxSettingsCtrl;
    iMboxSettingsCtrl = NULL;
    delete iAccounts;
    iAccounts = NULL;
    delete iMailboxUtilities;
    iMailboxUtilities = NULL;

    // Delete the session when owned, but don't touch it, when delivered
    // from outside. The component outside should take care of updating
    // the session.
    if ( iFlags && iFlags->Flag( ESessionOwned ) )
        {
        iFlags->ClearFlag( EObserverSet );
        delete iMsvSession;
        iMsvSession = NULL;
        }

    iFlags->ClearFlag( EServicesReady );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInternalApiImpl::EmailAccountsL()
// ----------------------------------------------------------------------------
//
CEmailAccounts& CImumInternalApiImpl::EmailAccountsL()
    {
    if ( !iAccounts )
        {
        iAccounts = CEmailAccounts::NewL();
        }

    return *iAccounts;
    }

// ----------------------------------------------------------------------------
// CImumInternalApiImpl::MailboxDoctorL()
// ----------------------------------------------------------------------------
//
CImumInHealthServicesImpl& CImumInternalApiImpl::MailboxDoctorL()
    {
    if ( !iHealthServices )
        {
        iHealthServices = CImumInHealthServicesImpl::NewL( *this );
        }

    return *iHealthServices;
    }

// ----------------------------------------------------------------------------
// CImumInternalApiImpl::SettingsManagerL()
// ----------------------------------------------------------------------------
//
CImumMboxManager& CImumInternalApiImpl::SettingsManagerL()
    {
    if ( !iMailboxManager )
        {
        iMailboxManager = CImumMboxManager::NewL( *this );
        }

    return *iMailboxManager;
    }

// ----------------------------------------------------------------------------
// CImumInternalApiImpl::SettingsStorerL()
// ----------------------------------------------------------------------------
//
CImumMboxSettingsCtrl& CImumInternalApiImpl::SettingsStorerL()
    {
    if ( !iMboxSettingsCtrl )
        {
        iMboxSettingsCtrl = CImumMboxSettingsCtrl::NewL( *this );
        }

    return *iMboxSettingsCtrl;
    }

// ---------------------------------------------------------------------------
// CImumInternalApiImpl::MailboxServicesImplL()
// ---------------------------------------------------------------------------
//
CImumInMailboxServicesImpl& CImumInternalApiImpl::MailboxServicesImplL()
    {
    if ( !iMailboxServices )
        {
        iMailboxServices = CImumInMailboxServicesImpl::NewL( *this );
        }

    return *iMailboxServices;
    }

// ---------------------------------------------------------------------------
// CImumInternalApiImpl::Flags()
// ---------------------------------------------------------------------------
//
const CMuiuFlags& CImumInternalApiImpl::Flags()
    {
    return *iFlags;
    }

// ---------------------------------------------------------------------------
// CImumInternalApiImpl::CommDbUtils2L()
// ---------------------------------------------------------------------------
//
CMsvCommDbUtilities& CImumInternalApiImpl::CommDbUtilsL()
    {
    if ( !iCommDbUtils )
        {
        iCommDbUtils = CMsvCommDbUtilities::NewL();
        iCommDbUtils->InitializeSelectionL();
        }

    return *iCommDbUtils;
    }


/******************************************************************************

    Message Server functionality

******************************************************************************/

// ---------------------------------------------------------------------------
// CImumInternalApiImpl::HandleSessionEventL()
// ---------------------------------------------------------------------------
//
void CImumInternalApiImpl::HandleSessionEventL(
    TMsvSessionEvent aEvent,
    TAny* /*aArg1*/,
    TAny* /*aArg2*/,
    TAny* /*aArg3*/ )
    {
    IMUM_CONTEXT( CImumInternalApiImpl::HandleSessionEventL, 0, KLogInApi );
    IMUM1_IN( "aEvent: %d", aEvent );

    // Handle the events
    switch ( aEvent )
        {
        // On media changed and server termination, remove reset all of the
        // API's objects.
        case EMsvServerTerminated:
            Reset();
            break;

        // When ready, recreate the API objects
        case EMsvMediaAvailable:
        case EMsvServerReady:
        case EMsvMediaChanged:
            ReloadL();
            break;
        }

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumInternalApiImpl::ReloadL()
// ---------------------------------------------------------------------------
//
void CImumInternalApiImpl::ReloadL()
    {
    IMUM_CONTEXT( CImumInternalApiImpl::ReloadL, 0, KLogInApi );
    IMUM_IN();

    Reset();
    DoCreateMsvSessionL();

    // Make sure that if for some reason default mailbox information
    // is lost, find a valid mailbox from the system and set it as a default.
    // This kind of situation may occur after swapping the message storage
    // between phone and memory card.
    if ( iFlags->Flag( EHasUi ) )
        {
        MailboxUtilitiesL().DefaultMailboxId( ETrue );
        }

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumInternalApiImpl::CreateMsvSession()
// ---------------------------------------------------------------------------
//
CMsvSession* CImumInternalApiImpl::CreateMsvSession()
    {
    IMUM_CONTEXT( CImumInternalApiImpl::CreateMsvSession, 0, KLogInApi );
    IMUM_IN();

    // Catch leaves. The method should not leave and there just
    // is no possibility that this would fail...
    TRAP_IGNORE( iMsvSession = DoCreateMsvSessionL() );

    IMUM_OUT();

    return iMsvSession;
    }

// ---------------------------------------------------------------------------
// CImumInternalApiImpl::DoCreateMsvSessionL()
// ---------------------------------------------------------------------------
//
CMsvSession* CImumInternalApiImpl::DoCreateMsvSessionL()
    {
    IMUM_CONTEXT( CImumInternalApiImpl::DoCreateMsvSessionL, 0, KLogInApi );
    IMUM_IN();

    if ( !iMsvSession )
        {
        IMUM0( 0, "No session, create new" );

        iFlags->SetFlag( ESessionOwned );
        iMsvSession = CMsvSession::OpenSyncL( *this );
        iFlags->ClearFlag( EObserverSet );
        }

    // To prevent adding multiple observers to list, check if
    // already assigned
    if ( !iFlags->Flag( EObserverSet ) )
        {
        IMUM0( 0, "Not an observer, make it so" );

        iMsvSession->AddObserverL( *this );
        iFlags->SetFlag( EObserverSet );
        }

    iFlags->SetFlag( EServicesReady );

    IMUM_OUT();

    return iMsvSession;
    }

/******************************************************************************

    Interface Methods

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumInternalApiImpl::HealthServicesL()
// ----------------------------------------------------------------------------
//
CMsvSession& CImumInternalApiImpl::MsvSession()
    {
    IMUM_CONTEXT( CImumInternalApiImpl::MsvSession, 0, KLogInApi );
    IMUM_IN();

    // It is possible, that session is destroyed outside, but to the API.
    // Here it must be made sure, that working session is provided.
    if ( !iMsvSession )
        {
        // It should be made sure that session exists... wait if have to
        CreateMsvSession();
        }

    IMUM_OUT();

    return *iMsvSession;
    }

// ----------------------------------------------------------------------------
// CImumInternalApiImpl::HealthServicesL()
// ----------------------------------------------------------------------------
//
const MImumInHealthServices& CImumInternalApiImpl::HealthServicesL()
    {
    IMUM_CONTEXT( CImumInternalApiImpl::HealthServicesL, 0, KLogInApi );
    IMUM_IN();

    if ( !iFlags->Flag( EServicesReady ) )
        {
        ServiceExceptionL( TImumDaErrorCode::EServicesNotReady );
        }

    IMUM_OUT();

    return MailboxDoctorL();
    }

// ----------------------------------------------------------------------------
// CImumInternalApiImpl::HealthApiL()
// ----------------------------------------------------------------------------
//
const MImumInHealthServices& CImumInternalApiImpl::HealthApiL()
    {
    IMUM_CONTEXT( CImumInternalApiImpl::HealthApiL, 0, KLogInApi );
    IMUM_IN();

    IMUM0( 0, "THIS IS DEPRECATED METHOD" );

    IMUM_OUT();
    return HealthServicesL();
    }

// ---------------------------------------------------------------------------
// CImumInternalApiImpl::MailboxUtilities()
// ---------------------------------------------------------------------------
//
const MImumInMailboxUtilities& CImumInternalApiImpl::MailboxUtilitiesL()
    {
    IMUM_CONTEXT( CImumInternalApiImpl::MailboxUtilitiesL, 0, KLogInApi );
    IMUM_IN();

    if ( !iFlags->Flag( EServicesReady ) )
        {
        ServiceExceptionL( TImumDaErrorCode::EServicesNotReady );
        }

    if ( !iMailboxUtilities )
        {
        iMailboxUtilities = CImumInMailboxUtilitiesImpl::NewL( *this );
        }

    IMUM_OUT();

    return *iMailboxUtilities;
    }

// ---------------------------------------------------------------------------
// CImumInternalApiImpl::MailboxUtilities()
// ---------------------------------------------------------------------------
//
const MImumInMailboxServices& CImumInternalApiImpl::MailboxServicesL()
    {
    IMUM_CONTEXT( CImumInternalApiImpl::MailboxServicesL, 0, KLogInApi );
    IMUM_IN();

    if ( !iFlags->Flag( EServicesReady ) )
        {
        ServiceExceptionL( TImumDaErrorCode::EServicesNotReady );
        }

    IMUM_OUT();

    return MailboxServicesImplL();
    }

// ---------------------------------------------------------------------------
// CImumInternalApiImpl::IsEmailFeatureSupported()
// ---------------------------------------------------------------------------
//
TBool CImumInternalApiImpl::IsEmailFeatureSupportedL(
    const TEmailFeatures aFlag,
    const TUint aFeatureOnResource,
    const TUint aFeatureOffResource )
    {
    IMUM_CONTEXT( CImumInternalApiImpl::IsEmailFeatureSupportedL, 0, KLogInApi );
    IMUM3_IN( "aFlag = %d, aFeatureOnResource = 0x%x, aFeatureOffResource = 0x%x",
        aFlag, aFeatureOnResource, aFeatureOffResource );
    TBool ret = EFalse;

    switch ( aFlag )
        {
        case EMailLockedSettings :
            ret = iFlags->GF( EMailFeatureIdSapPolicyManagement );
            break;
        default :
            break;
        }

    if( ret && aFeatureOnResource )
        {
        CIMSSettingsNoteUi::ShowNoteL(
            aFeatureOnResource,
            EIMSInformationNote );
        }
    else if( ret == EFalse && aFeatureOffResource )
        {
        CIMSSettingsNoteUi::ShowNoteL(
            aFeatureOffResource,
            EIMSInformationNote );
        }
    else
        {
        //Do not show notes.
        }

    IMUM_OUT();

    return ret;
    }

// End of File
