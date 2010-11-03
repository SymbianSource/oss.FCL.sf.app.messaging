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
* Description:  ImumInSettingsDataCollectionImpl.cpp
*
*/


// INCLUDE FILES
#include <msvstd.h>
#include <MuiuMsvUiServiceUtilities.h>
#include <SendUiConsts.h>                       // Message type constants

#include "ImumInSettingsDataCollectionImpl.h"
#include "ImumDaErrorCodes.h"
#include "ImumPanic.h"
#include "ImumInSettingsDataArray.h"
#include "ImumDaSettingsKeys.h"
#include "ImumInSettingsKeys.h"
#include "EmailUtils.H"
#include "ImumInSettingsDataCtrl.h"
#include "ImumInSettingsDataValidator.h"
#include "ImumUtilsLogging.h"
#include "ImumConstants.h"
#include "ImumMboxDefaultData.h"

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
// CImumInSettingsDataCollectionImpl::CImumInSettingsDataCollectionImpl()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataCollectionImpl::CImumInSettingsDataCollectionImpl(
    const TUid& aProtocol,
    const MImumInSettingsDataCollection* aDefaultData,
    CImumInSettingsDataValidator& aValidator )
    :
    iDefaultData( aDefaultData ),
    iProtocol( aProtocol ),
    iValidator( aValidator ),
    iSettings( NULL )
    {
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataCollectionImpl::~CImumInSettingsDataCollectionImpl()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataCollectionImpl::~CImumInSettingsDataCollectionImpl()
    {
    IMUM_CONTEXT( CImumInSettingsDataCollectionImpl::~CImumInSettingsDataCollectionImpl, 0, KLogInApi );
    IMUM_IN();

    delete iSettings;
    iSettings = NULL;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataCollectionImpl::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumInSettingsDataCollectionImpl::ConstructL(
    const TUid& aProtocol )
    {
    IMUM_CONTEXT( CImumInSettingsDataCollectionImpl::ConstructL, 0, KLogInApi );
    IMUM_IN();

    iSettings = CImumInSettingsDataArray::NewL();

    if ( aProtocol == KSenduiMtmImap4Uid )
        {
        SetAttr(
            TImumDaSettings::EKeyProtocol,
            TImumDaSettings::EValueProtocolImap4 );
        }
    else if ( aProtocol == KSenduiMtmPop3Uid )
        {
        SetAttr(
            TImumDaSettings::EKeyProtocol,
            TImumDaSettings::EValueProtocolPop3 );
        }
    else if ( aProtocol == KSenduiMtmSmtpUid )
        {
        SetAttr(
            TImumDaSettings::EKeyProtocol,
            TImumDaSettings::EValueProtocolSmtp );
        }
    else if ( !aProtocol.iUid )
        {
        SettingsDataCollectionExceptionL(
            TImumDaErrorCode::EParamUnsupportedProtocol );
        }
    else
        {
        SettingsDataCollectionExceptionL(
            TImumDaErrorCode::EParamEmptyProtocol );
        }


    // It's possible to reset the settings, if the settings class would
    // contain the default data. If the default data is not set, it means
    // that the current object is the default data, so the resetting should
    // be skipped here and the actual resetting happens in the real default
    // data class.
    if ( iDefaultData )
        {
        ResetAll();
        }

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataCollectionImpl::NewL()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataCollectionImpl* CImumInSettingsDataCollectionImpl::NewL(
    const TUid& aProtocol,
    const MImumInSettingsDataCollection* aDefaultData,
    CImumInSettingsDataValidator& aValidator )
    {
    IMUM_STATIC_CONTEXT( CImumInSettingsDataCollectionImpl::NewL, 0, utils, KLogInApi );
    IMUM_IN();

    CImumInSettingsDataCollectionImpl* self = NewLC(
        aProtocol, aDefaultData, aValidator );
    CleanupStack::Pop( self );

    IMUM_OUT();

    return self;
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataCollectionImpl::NewLC()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataCollectionImpl* CImumInSettingsDataCollectionImpl::NewLC(
    const TUid& aProtocol,
    const MImumInSettingsDataCollection* aDefaultData,
    CImumInSettingsDataValidator& aValidator )
    {
    IMUM_STATIC_CONTEXT( CImumInSettingsDataCollectionImpl::NewLC, 0, utils, KLogInApi );
    IMUM_IN();

    CImumInSettingsDataCollectionImpl* self =
        new ( ELeave ) CImumInSettingsDataCollectionImpl(
            aProtocol, aDefaultData, aValidator );
    CleanupStack::PushL( self );
    self->ConstructL( aProtocol );

    IMUM_OUT();

    return self;
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection.
// CImumInSettingsDataCollectionImpl::SetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataCollectionImpl::SetAttr(
    const TUint aAttributeKey,
    const TInt aValue )
    {
    TInt emptyDefault = 0;
    return ImumInSettingsDataCtrl::CreateToArray(
        *iSettings,
        iDefaultData,
        aAttributeKey,
        ECmpTInt32,
        aValue,
        emptyDefault );
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection.
// CImumInSettingsDataCollectionImpl::SetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataCollectionImpl::SetAttr(
    const TUint aAttributeKey,
    const TInt64 aValue )
    {
    TInt64 emptyDefault = 0;
    return ImumInSettingsDataCtrl::CreateToArray(
        *iSettings,
        iDefaultData,
        aAttributeKey,
        ECmpTInt64,
        aValue,
        emptyDefault );
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection.
// CImumInSettingsDataCollectionImpl::SetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataCollectionImpl::SetAttr(
    const TUint aAttributeKey,
    const TMsvId aId )
    {
    TMsvId emptyDefault = 0;
    return ImumInSettingsDataCtrl::CreateToArray(
        *iSettings,
        iDefaultData,
        aAttributeKey,
        ECmpTUint32,
        aId,
        emptyDefault );
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection.
// CImumInSettingsDataCollectionImpl::SetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataCollectionImpl::SetAttr(
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
// From class MImumDaSettingsDataCollection.
// CImumInSettingsDataCollectionImpl::SetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataCollectionImpl::SetAttr(
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
// From class MImumDaSettingsDataCollection.
// CImumInSettingsDataCollectionImpl::GetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataCollectionImpl::GetAttr(
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
// From class MImumDaSettingsDataCollection.
// CImumInSettingsDataCollectionImpl::GetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataCollectionImpl::GetAttr(
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
// From class MImumDaSettingsDataCollection.
// CImumInSettingsDataCollectionImpl::GetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataCollectionImpl::GetAttr(
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
// From class MImumDaSettingsDataCollection.
// CImumInSettingsDataCollectionImpl::GetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataCollectionImpl::GetAttr(
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
// From class MImumDaSettingsDataCollection.
// CImumInSettingsDataCollectionImpl::GetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataCollectionImpl::GetAttr(
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
// From class MImumDaSettingsDataCollection.
// CImumInSettingsDataCollectionImpl::operator!=()
// ---------------------------------------------------------------------------
//
TBool CImumInSettingsDataCollectionImpl::operator!=(
    const MImumDaSettingsDataCollection& aSettingsData ) const
    {
    IMUM_CONTEXT( CImumInSettingsDataCollectionImpl::operator!=, 0, KLogInApi );
    IMUM_IN();
    IMUM_OUT();

    return !( *this == aSettingsData );
    }

// ---------------------------------------------------------------------------
// From class MImumInSettingsDataCollection.
// CImumInSettingsDataCollectionImpl::operator!=()
// ---------------------------------------------------------------------------
//
TBool CImumInSettingsDataCollectionImpl::operator!=(
    const MImumInSettingsDataCollection& aSettingsData ) const
    {
    IMUM_CONTEXT( CImumInSettingsDataCollectionImpl::operator!=, 0, KLogInApi );
    IMUM_IN();
    IMUM_OUT();

    return !( *this == aSettingsData );
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection.
// CImumInSettingsDataCollectionImpl::operator==()
// ---------------------------------------------------------------------------
//
TBool CImumInSettingsDataCollectionImpl::operator==(
    const MImumDaSettingsDataCollection& /* aSettingsData */ ) const
    {
    IMUM_CONTEXT( CImumInSettingsDataCollectionImpl::operator==, 0, KLogInApi );
    IMUM_IN();

    IMUM_OUT();

    return ETrue;
    }

// ---------------------------------------------------------------------------
// From class MImumInSettingsDataCollection.
// CImumInSettingsDataCollectionImpl::operator==()
// ---------------------------------------------------------------------------
//
TBool CImumInSettingsDataCollectionImpl::operator==(
    const MImumInSettingsDataCollection& aSettingsData ) const
    {
    IMUM_CONTEXT( CImumInSettingsDataCollectionImpl::operator==, 0, KLogInApi );
    IMUM_IN();

    IMUM_OUT();
    return ( *this == *static_cast<const MImumDaSettingsDataCollection*>(
        &aSettingsData ) );
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection.
// CImumInSettingsDataCollectionImpl::Reset()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataCollectionImpl::Reset( const TUint aAttributeKey )
    {
    IMUM_CONTEXT( CImumInSettingsDataCollectionImpl::Reset, 0, KLogInApi );
    IMUM_IN();

    iSettings->ResetAttr( aAttributeKey );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection.
// CImumInSettingsDataCollectionImpl::Copy()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataCollectionImpl::Copy(
    const MImumDaSettingsDataCollection& /* aSettingsData */ )
    {
    IMUM_CONTEXT( CImumInSettingsDataCollectionImpl::Copy, 0, KLogInApi );
    IMUM_IN();

    // Currently no support for copy operation. Cloning should be used instead.
    __ASSERT_DEBUG( EFalse, User::Panic(
        KImumInSettingsDataCollectionImpl, KErrNotSupported ) );

    IMUM_OUT();

    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection.
// CImumInSettingsDataCollectionImpl::Copy()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataCollectionImpl::Copy(
    const MImumInSettingsDataCollection& aSettingsData )
    {
    return Copy( *static_cast<const MImumDaSettingsDataCollection*>(
        &aSettingsData ) );
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataCollectionImpl::SettingsDataCollectionExceptionL()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataCollectionImpl::SettingsDataCollectionExceptionL(
    const TInt aErrorCode ) const
    {
    IMUM_CONTEXT( CImumInSettingsDataCollectionImpl::SettingsDataCollectionExceptionL, 0, KLogInApi );
    IMUM_IN();

    User::Leave( aErrorCode );
    __ASSERT_DEBUG( EFalse, User::Panic(
        KImumInSettingsDataCollectionImpl, aErrorCode ) );
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataCollectionImpl::ValidateL()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataCollectionImpl::ValidateL()
    {
    IMUM_CONTEXT( CImumInSettingsDataCollectionImpl::ValidateL, 0, KLogInApi );
    IMUM_IN();

    iValidator.ValidateL( *this );

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataCollectionImpl::ResetAll()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataCollectionImpl::ResetAll()
    {
    IMUM_CONTEXT( CImumInSettingsDataCollectionImpl::ResetAll, 0, KLogInApi );
    IMUM_IN();

    RArray<TUint> excludeList;
    iSettings->ResetAll( excludeList );
    excludeList.Close();

    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataCollectionImpl::Log()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataCollectionImpl::Log()
    {
    IMUM_CONTEXT( CImumInSettingsDataCollectionImpl::Log, 0, KLogInApi );
    IMUM_IN();
    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// From class .
// CImumInSettingsDataCollectionImpl::CloneL()
// ---------------------------------------------------------------------------
//
MImumInSettingsDataCollection* CImumInSettingsDataCollectionImpl::CloneL() const
    {
    IMUM_CONTEXT( CImumInSettingsDataCollectionImpl::Log, 0, KLogInApi );
    IMUM_IN();
    IMUM_OUT();

    return DoCloneL();
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataCollectionImpl::DoCloneL()
// ---------------------------------------------------------------------------
//
CImumInSettingsDataCollectionImpl*
    CImumInSettingsDataCollectionImpl::DoCloneL() const
    {
    IMUM_CONTEXT( CImumInSettingsDataCollectionImpl::Log, 0, KLogInApi );
    IMUM_IN();

    CImumInSettingsDataCollectionImpl* clone = NewLC(
        iProtocol, iDefaultData, iValidator );

    // Before doing anything, the previous array must be deleted to prevent
    // any extra data to remain in the memory.
    delete clone->iSettings;
    clone->iSettings = iSettings->CloneL();

    CleanupStack::Pop( clone );

    IMUM_OUT();

    return clone;
    }


// End of File
