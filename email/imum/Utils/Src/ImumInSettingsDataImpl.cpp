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
* Description:  ImumInSettingsDataImpl.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <SendUiConsts.h>

#include "ImumInternalApiImpl.h"
#include "ImumInSettingsDataImpl.h"
#include "ImumInSettingsDataCollectionImpl.h"
#include "ImumInMailboxServicesImpl.h"
#include "ImumDaErrorCodes.h"
#include "ImumPanic.h"
#include "ImumMboxDefaultData.h"
#include "ImumInSettingsDataArray.h"
#include "ImumDaSettingsKeys.h"
#include "ImumInSettingsKeys.h"
#include "ImumInSettingsDataCtrl.h"
#include "ImumInSettingsDataValidator.h"
#include "ImumUtilsLogging.h"
#include "ImumConstants.h"

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
// CImumInSettingsDataImpl::CImumInSettingsDataImpl()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataImpl::CImumInSettingsDataImpl(
    CImumInternalApiImpl& aMailboxApi,
    const CImumMboxDefaultData* aDefaultData )
    :
    iConnectionSettings( NULL ),
    iSettings( NULL ),
    iValidator( NULL ),
    iDefaultData( aDefaultData ),
    iMailboxApi( aMailboxApi )
    {
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataImpl::~CImumInSettingsDataImpl()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataImpl::~CImumInSettingsDataImpl()
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::~CImumInSettingsDataImpl, 0, KLogInApi );
    IMUM_IN();

    iDefaultData = NULL;
    if ( iConnectionSettings )
        {
        iConnectionSettings->ResetAndDestroy();
        }

    delete iConnectionSettings;
    iConnectionSettings = NULL;
    delete iSettings;
    iSettings = NULL;
    delete iValidator;
    iValidator = NULL;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataImpl::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumInSettingsDataImpl::ConstructL(
    const TUid& aProtocol )
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::ConstructL, 0, KLogInApi );
    IMUM_IN();

    iSettings = iDefaultData->CloneSettingsL();
    iConnectionSettings =
        new ( ELeave ) CImumInConnSettings( KImumInConnSettingsGranularity );

    // Prepare the settings structure
    if ( aProtocol == KSenduiMtmImap4Uid )
        {
        AddInSetL( aProtocol );
        AddInSetL( KSenduiMtmSmtpUid );
        }
    else if ( aProtocol == KSenduiMtmPop3Uid )
        {
        AddInSetL( aProtocol );
        AddInSetL( KSenduiMtmSmtpUid );
        }
    else if ( aProtocol == KSenduiMtmSmtpUid )
        {
        SettingsDataExceptionL( TImumDaErrorCode::EParamNotReceivingProtocol );
        }
    else if ( aProtocol.iUid )
        {
        SettingsDataExceptionL( TImumDaErrorCode::EParamUnsupportedProtocol );
        }
    else
        {
        // Empty, will be defined later
        }

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataImpl::NewL()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataImpl* CImumInSettingsDataImpl::NewL(
    CImumInternalApiImpl& aMailboxApi,
    const TUid& aProtocol,
    const CImumMboxDefaultData* aDefaultData )
    {
    IMUM_STATIC_CONTEXT( CImumInSettingsDataImpl::NewL, 0, utils, KLogInApi );
    IMUM_IN();

    CImumInSettingsDataImpl* self =
        NewLC( aMailboxApi, aProtocol, aDefaultData );
    CleanupStack::Pop( self );

    IMUM_OUT();

    return self;
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataImpl::NewLC()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataImpl* CImumInSettingsDataImpl::NewLC(
    CImumInternalApiImpl& aMailboxApi,
    const TUid& aProtocol,
    const CImumMboxDefaultData* aDefaultData )
    {
    IMUM_STATIC_CONTEXT( CImumInSettingsDataImpl::NewLC, 0, utils, KLogInApi );
    IMUM_IN();

    CImumInSettingsDataImpl* self =
        new ( ELeave ) CImumInSettingsDataImpl( aMailboxApi, aDefaultData );
    CleanupStack::PushL( self );
    self->ConstructL( aProtocol );

    IMUM_OUT();

    return self;
    }

// ================ INTERFACE IMPLEMENTATION ================

// ---------------------------------------------------------------------------
// CImumInSettingsDataImpl::SelectDefaultData()
// ---------------------------------------------------------------------------
//
const CImumInSettingsDataCollectionImpl*
    CImumInSettingsDataImpl::SelectDefaultData(
        const TUid& aProtocol,
        const CImumMboxDefaultData* aDefaultData ) const
    {
    if ( aDefaultData )
        {
        switch ( aProtocol.iUid )
            {
            case KSenduiMtmImap4UidValue:
                return aDefaultData->DefaultConnectionData(
                    EImumMboxDefaultDataIndexImap4 );

            case KSenduiMtmPop3UidValue:
                return aDefaultData->DefaultConnectionData(
                    EImumMboxDefaultDataIndexPop3 );

            case KSenduiMtmSmtpUidValue:
                return aDefaultData->DefaultConnectionData(
                    EImumMboxDefaultDataIndexSmtp );

            default:
                break;
            }
        }

    return NULL;
    }


// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::AddSetL()
// ---------------------------------------------------------------------------
//
MImumDaSettingsDataCollection& CImumInSettingsDataImpl::AddSetL(
    const TUid& aProtocol )
    {
    return AddInSetL( aProtocol );
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::AddInSetL()
// ---------------------------------------------------------------------------
//
MImumInSettingsDataCollection& CImumInSettingsDataImpl::AddInSetL(
    const TUid& aProtocol )
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::AddInSetL, 0, KLogInApi );
    IMUM_IN();

    // Clone the connection settings from the default data object
    CImumInSettingsDataCollectionImpl* connSettings =
        iDefaultData->CloneConnectionSettingsL( aProtocol );
    CleanupStack::PushL( connSettings );
    iConnectionSettings->AppendL( connSettings );
    CleanupStack::Pop( connSettings );

    IMUM_OUT();

    return *connSettings;
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::DelSetL()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataImpl::DelSetL( const TUint aIndex )
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::DelSetL, 0, KLogInApi );
    IMUM_IN();

    // Check that index con
    if ( aIndex < iConnectionSettings->Count() )
        {
        delete ( *iConnectionSettings )[aIndex];
        iConnectionSettings->Delete( aIndex );
        }
    else
        {
        SettingsDataExceptionL( TImumDaErrorCode::EConnectionIndexOverflow );
        }

    IMUM_OUT();
    }


// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::DelSetL()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataImpl::DelSetL(
    const MImumDaSettingsDataCollection& aSettingsDataCollection )
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::DelSetL, 0, KLogInApi );
    IMUM_IN();

    TBool deleted = EFalse;

    for ( TInt setting = iConnectionSettings->Count();
        --setting >= 0 && !deleted; )
        {
        if ( *( *iConnectionSettings )[setting] == aSettingsDataCollection )
            {
            delete ( *iConnectionSettings )[setting];
            iConnectionSettings->Delete( setting );
            deleted = ETrue;
            }
        }

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::GetSet()
// ---------------------------------------------------------------------------
//
MImumDaSettingsDataCollection& CImumInSettingsDataImpl::GetSetL(
    const TUint aIndex ) const
    {
    return GetInSetL( aIndex );
    }

// ---------------------------------------------------------------------------
// From class CImumInSettingsData.
// CImumInSettingsDataImpl::GetSet()
// ---------------------------------------------------------------------------
//
MImumInSettingsDataCollection& CImumInSettingsDataImpl::GetInSetL(
    const TUint aIndex ) const
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::GetInSetL, 0, KLogInApi );
    IMUM_IN();

    MImumInSettingsDataCollection* settings = NULL;

    // Check that index con
    if ( aIndex < iConnectionSettings->Count() )
        {
        settings = iConnectionSettings->At( aIndex );
        }
    else
        {
        SettingsDataExceptionL( TImumDaErrorCode::EConnectionIndexOverflow );
        }

    IMUM_OUT();

    return *settings;
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::Validate()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataImpl::Validate()
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::Validate, 0, KLogInApi );
    IMUM_IN();

    TRAPD( error, ValidateL() );

    IMUM_OUT();

    return error;
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::SetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataImpl::SetAttr(
    const TUint aAttributeKey,
    const TInt aValue )
    {
    TInt defaultValue = 0;
    return ImumInSettingsDataCtrl::CreateToArray(
        *iSettings,
        iDefaultData,
        aAttributeKey,
        ECmpTInt32,
        aValue,
        defaultValue );
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::SetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataImpl::SetAttr(
    const TUint aAttributeKey,
    const TInt64 aValue )
    {
    TInt64 defaultValue = 0;
    return ImumInSettingsDataCtrl::CreateToArray(
        *iSettings,
        iDefaultData,
        aAttributeKey,
        ECmpTInt64,
        aValue,
        defaultValue );
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::SetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataImpl::SetAttr(
    const TUint aAttributeKey,
    const TMsvId aId )
    {
    TMsvId defaultValue = 0;
    return ImumInSettingsDataCtrl::CreateToArray(
        *iSettings,
        iDefaultData,
        aAttributeKey,
        ECmpTUint32,
        aId,
        defaultValue );
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::SetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataImpl::SetAttr(
    const TUint aAttributeKey,
    const TDesC& aText )
    {
    RBuf emptyDefault;
    TInt error = emptyDefault.Create( ImumTextConsts::EMaxTextLen );
    if ( error == KErrNone )
        {
        error = ImumInSettingsDataCtrl::CreateToArray(
            *iSettings,
            iDefaultData,
            aAttributeKey,
            ECmpNormal,
            aText,
            emptyDefault );
        }

    emptyDefault.Close();

    return error;
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::SetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataImpl::SetAttr(
    const TUint aAttributeKey,
    const TDesC8& aText )
    {
    RBuf8 emptyDefault;
    TInt error = emptyDefault.Create( ImumTextConsts::EMaxTextLen );
    if ( error == KErrNone )
        {
        error = ImumInSettingsDataCtrl::CreateToArray(
            *iSettings,
            iDefaultData,
            aAttributeKey,
            ECmpNormal8,
            aText,
            emptyDefault );
        }

    emptyDefault.Close();

    return error;
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::GetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataImpl::GetAttr(
    const TUint aAttributeKey,
    TInt& aValue ) const
    {
    return ImumInSettingsDataCtrl::RetrieveFromArray(
        *iSettings,
        aAttributeKey,
        ECmpTInt32,
        aValue );
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::GetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataImpl::GetAttr(
    const TUint aAttributeKey,
    TInt64& aValue ) const
    {
    return ImumInSettingsDataCtrl::RetrieveFromArray(
        *iSettings,
        aAttributeKey,
        ECmpTInt64,
        aValue );
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::GetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataImpl::GetAttr(
    const TUint aAttributeKey,
    TMsvId& aId ) const
    {
    return ImumInSettingsDataCtrl::RetrieveFromArray(
        *iSettings,
        aAttributeKey,
        ECmpTUint32,
        aId );
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::GetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataImpl::GetAttr(
    const TUint aAttributeKey,
    TDes& aText ) const
    {
    return ImumInSettingsDataCtrl::RetrieveFromArray(
        *iSettings,
        aAttributeKey,
        ECmpNormal,
        aText );
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::GetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataImpl::GetAttr(
    const TUint aAttributeKey,
    TDes8& aText ) const
    {
    return ImumInSettingsDataCtrl::RetrieveFromArray(
        *iSettings,
        aAttributeKey,
        ECmpNormal8,
        aText );
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::operator!=()
// ---------------------------------------------------------------------------
//
TBool CImumInSettingsDataImpl::operator!=(
    const CImumDaSettingsData& /* aSettingsData */ ) const
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::operator!=, 0, KLogInApi );
    IMUM_IN();

    __ASSERT_DEBUG( EFalse,
        User::Panic( KImumInSettingsDataImpl,
        TImumDaErrorCode::EUnsupportedOperation ) );

    IMUM_OUT();

    return EFalse;
    }

// ---------------------------------------------------------------------------
// From class CImumInSettingsData.
// CImumInSettingsDataImpl::operator!=()
// ---------------------------------------------------------------------------
//
TBool CImumInSettingsDataImpl::operator!=(
    const CImumInSettingsData& /* aSettingsData */ ) const
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::operator!=, 0, KLogInApi );
    IMUM_IN();

    __ASSERT_DEBUG( EFalse,
        User::Panic( KImumInSettingsDataImpl,
        TImumDaErrorCode::EUnsupportedOperation ) );

    IMUM_OUT();

    return EFalse;
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::operator==()
// ---------------------------------------------------------------------------
//
TBool CImumInSettingsDataImpl::operator==(
    const CImumDaSettingsData& /* aSettingsData */ ) const
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::operator==, 0, KLogInApi );
    IMUM_IN();

    __ASSERT_DEBUG( EFalse,
        User::Panic( KImumInSettingsDataImpl,
        TImumDaErrorCode::EUnsupportedOperation ) );

    IMUM_OUT();

    return EFalse;
    }

// ---------------------------------------------------------------------------
// From class CImumInSettingsData.
// CImumInSettingsDataImpl::operator==()
// ---------------------------------------------------------------------------
//
TBool CImumInSettingsDataImpl::operator==(
    const CImumInSettingsData& /* aSettingsData */ ) const
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::operator==, 0, KLogInApi );
    IMUM_IN();

    __ASSERT_DEBUG( EFalse,
        User::Panic( KImumInSettingsDataImpl,
        TImumDaErrorCode::EUnsupportedOperation ) );

    IMUM_OUT();

    return EFalse;
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::operator[]()
// ---------------------------------------------------------------------------
//
MImumDaSettingsDataCollection& CImumInSettingsDataImpl::operator[](
    const TUint aIndex ) const
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::operator, 0, KLogInApi );
    IMUM_IN();

    __ASSERT_DEBUG( aIndex < iConnectionSettings->Count(),
        User::Panic( KImumInSettingsDataImpl,
        TImumDaErrorCode::EConnectionIndexOverflow ) );

    IMUM_OUT();

    return *iConnectionSettings->At( aIndex );
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::Reset()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataImpl::Reset( const TUint aAttributeKey )
    {
    iSettings->ResetAttr( aAttributeKey );
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::ResetAll()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataImpl::ResetAll()
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::ResetAll, 0, KLogInApi );
    IMUM_IN();

    // Default data is up to date, so do the actual resetting of the
    // settings
    for ( TInt connSet = iConnectionSettings->Count(); --connSet >= 0; )
        {
        iConnectionSettings->At( connSet )->ResetAll();
        }

    RArray<TUint> excludeList;
    iSettings->ResetAll( excludeList );
    excludeList.Close();

    IMUM_OUT();
    }


// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::Copy()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataImpl::Copy( const CImumInSettingsData& aSettingsData )
    {
    return Copy( *static_cast<const CImumDaSettingsData*>( &aSettingsData ) );
    }

// ---------------------------------------------------------------------------
// From class CImumInSettingsData.
// CImumInSettingsDataImpl::Copy()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataImpl::Copy( const CImumDaSettingsData& aSettingsData )
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::Copy, 0, KLogInApi );
    IMUM_IN();

    __ASSERT_DEBUG( EFalse,
        User::Panic( KImumInSettingsDataImpl,
        TImumDaErrorCode::EUnsupportedOperation ) );

    TInt error = TImumDaErrorCode::ENoError;
    for ( TInt connSet = iConnectionSettings->Count();
          --connSet >= 0 && !error; )
        {
        error = iConnectionSettings->At( connSet )->Copy( aSettingsData[connSet] );
        }

    IMUM_OUT();

    return error;
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData.
// CImumInSettingsDataImpl::Log()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataImpl::Log() const
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::Log, 0, KLogInApi );
    IMUM_IN();

#ifdef __DEBUG
    // First, validate the connection settings
    for ( TInt connSet = iConnectionSettings->Count(); --connSet >= 0; )
        {
        iConnectionSettings->At( connSet )->Log();
        }
#endif // __DEBUG

    IMUM_OUT();
    }

// ================ GENERAL IMPLEMENTATION ================

// ---------------------------------------------------------------------------
// CImumInSettingsDataImpl::SettingsDataExceptionL()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataImpl::SettingsDataExceptionL(
    const TInt aErrorCode ) const
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::SettingsDataExceptionL, 0, KLogInApi );
    IMUM_IN();

    User::Leave( aErrorCode );
    __ASSERT_DEBUG( EFalse, User::Panic(
        KImumInSettingsDataImpl, aErrorCode ) );
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataImpl::ValidatorL()
// ---------------------------------------------------------------------------
//
CImumInSettingsDataValidator& CImumInSettingsDataImpl::ValidatorL()
    {
    if ( !iValidator )
        {
        // Create settings validator
        iValidator = CImumInSettingsDataValidator::NewL( iMailboxApi );
        }

    return *iValidator;
    }


// ---------------------------------------------------------------------------
// CImumInSettingsDataImpl::ValidateL()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataImpl::ValidateL()
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::ValidateL, 0, KLogInApi );
    IMUM_IN();

    ValidatorL().ValidateL( *this );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataImpl::ConnectionSettingsCount()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataImpl::ConnectionSettingsCount()
    {
    return iConnectionSettings->Count();
    }

// ---------------------------------------------------------------------------
// From class CImumInSettingsData.
// CImumInSettingsDataImpl::CloneL()
// ---------------------------------------------------------------------------
//
CImumInSettingsData* CImumInSettingsDataImpl::CloneL() const
    {
    IMUM_CONTEXT( CImumInSettingsDataImpl::CloneL, 0, KLogInApi );
    IMUM_IN();

    // When cloning, the default data object must be added to the new data.
    // This guarrantees also, that there will be only one default data object.
    CImumInSettingsDataImpl* clone = NewLC(
        iMailboxApi,
        TUid::Uid( 0 ),
        iMailboxApi.MailboxServicesImplL().DefaultDataObjectL() );

    for ( TInt connection = KErrNotFound;
        ++connection < iConnectionSettings->Count(); )
        {
        clone->iConnectionSettings->AppendL(
            iConnectionSettings->At( connection )->DoCloneL() );
        }

    // Before doing anything, the previous array must be delete to prevent
    // any extra data to remain in the memory.
    delete clone->iSettings;
    clone->iSettings = iSettings->CloneL();

    CleanupStack::Pop( clone );

    IMUM_OUT();

    return clone;
    }

