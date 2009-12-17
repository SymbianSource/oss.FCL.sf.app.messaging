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
* Description:  ImumDaSettingsDataImpl.cpp
 *
*/


#include <ImumInMailboxServices.h>

#include "ImumDaSettingsDataImpl.h"
#include "ImumInSettingsData.h"
#include "ImumInternalApi.h"
#include "ImumDaErrorCodes.h"
#include "ImumPanic.h"

// CONSTANTS
const TInt KImumDaConnSettingsGranularity = 2;

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CImumDaSettingsDataImpl::CImumDaSettingsDataImpl()
// ----------------------------------------------------------------------------
//
CImumDaSettingsDataImpl::CImumDaSettingsDataImpl(
    CImumInternalApi &aMailboxApi )
    :
    iMailboxApi( aMailboxApi ),
    iData( NULL ),
    iDaConnectionSettings( NULL )
    {
    }

// ----------------------------------------------------------------------------
// CImumDaSettingsDataImpl::~CImumDaSettingsDataImpl()
// ----------------------------------------------------------------------------
//
CImumDaSettingsDataImpl::~CImumDaSettingsDataImpl()
    {
    if (iDaConnectionSettings != NULL)
        iDaConnectionSettings->ResetAndDestroy();
    delete iDaConnectionSettings;
    iDaConnectionSettings = NULL;

    delete iData;
    iData = NULL;
    }

// ----------------------------------------------------------------------------
// CImumDaSettingsDataImpl::NewL()
// ----------------------------------------------------------------------------
//
CImumDaSettingsDataImpl* CImumDaSettingsDataImpl::NewL(
    CImumInternalApi &aMailboxApi,
    const TUid& aProtocol )
    {
    CImumDaSettingsDataImpl* self = NewLC( aMailboxApi, aProtocol );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CImumDaSettingsDataImpl::NewLC()
// ----------------------------------------------------------------------------
//
CImumDaSettingsDataImpl* CImumDaSettingsDataImpl::NewLC(
    CImumInternalApi &aMailboxApi,
    const TUid& aProtocol )
    {
    CImumDaSettingsDataImpl* self =
        new ( ELeave ) CImumDaSettingsDataImpl( aMailboxApi );
    CleanupStack::PushL( self );
    self->ConstructL( aProtocol );

    return self;
    }

// ----------------------------------------------------------------------------
// CImumDaSettingsDataImpl::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumDaSettingsDataImpl::ConstructL( const TUid& aProtocol )
    {
    iData = iMailboxApi.MailboxServicesL().CreateSettingsDataL( aProtocol );

    iDaConnectionSettings =
        new ( ELeave ) CImumDaConnSettings( KImumDaConnSettingsGranularity );
    }

// ======================= INTERFACE IMPLEMENTATION ===========================

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::AddSetL()
// ----------------------------------------------------------------------------
//
MImumDaSettingsDataCollection& CImumDaSettingsDataImpl::AddSetL(
    const TUid& aProtocol )
    {
    CImumDaSettingsDataCollectionImpl* daDataCollection;

    MImumInSettingsDataCollection* inDataCollection =
        &iData->AddInSetL( aProtocol );

    daDataCollection = CImumDaSettingsDataCollectionImpl::NewL(
        *inDataCollection );

    CleanupStack::PushL( daDataCollection );
    iDaConnectionSettings->AppendL( daDataCollection );
    CleanupStack::Pop( daDataCollection );

    inDataCollection = NULL;

    return *daDataCollection;
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::DelSetL()
// ----------------------------------------------------------------------------
//
void CImumDaSettingsDataImpl::DelSetL( const TUint aIndex )
    {
    iData->DelSetL( aIndex );

    delete ( *iDaConnectionSettings )[aIndex];
    iDaConnectionSettings->Delete( aIndex );
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::DelSetL()
// ----------------------------------------------------------------------------
//
void CImumDaSettingsDataImpl::DelSetL(
    const MImumDaSettingsDataCollection& /* aSettingsDataCollection */ )
    {
    // Empty, will be defined later
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::GetSetL()
// ----------------------------------------------------------------------------
//
MImumDaSettingsDataCollection& CImumDaSettingsDataImpl::GetSetL(
    const TUint aIndex ) const
    {
    MImumDaSettingsDataCollection* DaSettings = NULL;

    if ( aIndex < iDaConnectionSettings->Count() )
        {
        DaSettings = iDaConnectionSettings->At( aIndex );
        }
    else
        {
        SettingsDataExceptionL( TImumDaErrorCode::EConnectionIndexOverflow );
        }

    return *DaSettings;
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::Validate()
// ----------------------------------------------------------------------------
//
TInt CImumDaSettingsDataImpl::Validate()
    {
    return iData->Validate();
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::SetAttr()
// ----------------------------------------------------------------------------
//
TInt CImumDaSettingsDataImpl::SetAttr(
    const TUint aAttributeKey,
    const TInt aValue )
    {
    if ( CImumDaSettingsDataCollectionImpl::IsLegalKey( aAttributeKey, EFalse ) )
        {
        return iData->SetAttr( aAttributeKey, aValue );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::SetAttr()
// ----------------------------------------------------------------------------
//
TInt CImumDaSettingsDataImpl::SetAttr(
    const TUint aAttributeKey,
    const TInt64 aValue )
    {
    if ( CImumDaSettingsDataCollectionImpl::IsLegalKey( aAttributeKey, EFalse ) )
        {
        return iData->SetAttr( aAttributeKey, aValue );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::SetAttr()
// ----------------------------------------------------------------------------
//
TInt CImumDaSettingsDataImpl::SetAttr(
    const TUint aAttributeKey,
    const TMsvId aValue )
    {
    if ( CImumDaSettingsDataCollectionImpl::IsLegalKey( aAttributeKey, EFalse ) )
        {
        return iData->SetAttr( aAttributeKey, aValue );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::SetAttr()
// ----------------------------------------------------------------------------
//
TInt CImumDaSettingsDataImpl::SetAttr(
    const TUint aAttributeKey,
    const TDesC& aText )
    {
    if ( CImumDaSettingsDataCollectionImpl::IsLegalKey( aAttributeKey, EFalse ) )
        {
        return iData->SetAttr( aAttributeKey, aText );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::SetAttr()
// ----------------------------------------------------------------------------
//
TInt CImumDaSettingsDataImpl::SetAttr(
    const TUint aAttributeKey,
    const TDesC8& aText )
    {
    if ( CImumDaSettingsDataCollectionImpl::IsLegalKey( aAttributeKey, EFalse ) )
        {
        return iData->SetAttr( aAttributeKey, aText );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::GetAttr()
// ----------------------------------------------------------------------------
//
TInt CImumDaSettingsDataImpl::GetAttr(
    const TUint aAttributeKey,
    TInt& aValue ) const
    {
    if ( CImumDaSettingsDataCollectionImpl::IsLegalKey( aAttributeKey, ETrue ) )
        {
        return iData->GetAttr( aAttributeKey, aValue );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::GetAttr()
// ----------------------------------------------------------------------------
//
TInt CImumDaSettingsDataImpl::GetAttr(
    const TUint aAttributeKey,
    TInt64& aValue ) const
    {
    if ( CImumDaSettingsDataCollectionImpl::IsLegalKey( aAttributeKey, ETrue ) )
        {
        return iData->GetAttr( aAttributeKey, aValue );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::GetAttr()
// ----------------------------------------------------------------------------
//
TInt CImumDaSettingsDataImpl::GetAttr(
    const TUint aAttributeKey,
    TMsvId& aId ) const
    {
    if ( CImumDaSettingsDataCollectionImpl::IsLegalKey( aAttributeKey, ETrue ) )
        {
        return iData->GetAttr( aAttributeKey, aId );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::GetAttr()
// ----------------------------------------------------------------------------
//
TInt CImumDaSettingsDataImpl::GetAttr(
    const TUint aAttributeKey,
    TDes& aText ) const
    {
    if ( CImumDaSettingsDataCollectionImpl::IsLegalKey( aAttributeKey, ETrue ) )
        {
        return iData->GetAttr( aAttributeKey, aText );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::GetAttr()
// ----------------------------------------------------------------------------
//
TInt CImumDaSettingsDataImpl::GetAttr(
    const TUint aAttributeKey,
    TDes8& aText ) const
    {
    if ( CImumDaSettingsDataCollectionImpl::IsLegalKey( aAttributeKey, ETrue ) )
        {
        return iData->GetAttr( aAttributeKey, aText );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::operator!=()
// ----------------------------------------------------------------------------
//
TBool CImumDaSettingsDataImpl::operator!=(
    const CImumDaSettingsData& aSettingsData ) const
    {
    const CImumInSettingsData* inSettingsData =
        static_cast<const CImumInSettingsData*>( &aSettingsData );

    return iData->operator!=( *inSettingsData );

    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::operator==()
// ----------------------------------------------------------------------------
//
TBool CImumDaSettingsDataImpl::operator==(
    const CImumDaSettingsData& aSettingsData ) const
    {
    const CImumInSettingsData* inSettingsData =
        static_cast<const CImumInSettingsData*>( &aSettingsData );

    return iData->operator==( *inSettingsData );

    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::operator[]()
// ----------------------------------------------------------------------------
//
MImumDaSettingsDataCollection& CImumDaSettingsDataImpl::operator[](
    const TUint aIndex ) const
    {
    MImumDaSettingsDataCollection* daDataCollection =
        &iData->operator[]( aIndex );

    return *daDataCollection;
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::ResetAll()
// ----------------------------------------------------------------------------
//
void CImumDaSettingsDataImpl::ResetAll()
    {
    iData->ResetAll();
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::Reset()
// ----------------------------------------------------------------------------
//
void CImumDaSettingsDataImpl::Reset(
    const TUint aAttributeKey )
    {
    iData->Reset( aAttributeKey );
    }

// ----------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::Copy()
// ----------------------------------------------------------------------------
//
TInt CImumDaSettingsDataImpl::Copy(
    const CImumDaSettingsData& aSettingsData )
    {
    const CImumInSettingsData* inSettingsData =
        static_cast<const CImumInSettingsData*>( &aSettingsData );

    return iData->Copy( *inSettingsData );
    }

// ---------------------------------------------------------------------------
// From class CImumDaSettingsData
// CImumDaSettingsDataImpl::GetData()
// ---------------------------------------------------------------------------
//
CImumInSettingsData& CImumDaSettingsDataImpl::GetInData()
    {
    return *iData;
    }

// ======================= INTERNAL FUNCTIONALITY ============================

// ---------------------------------------------------------------------------
// CImumDaSettingsDataImpl::SettingsDataExceptionL()
// ---------------------------------------------------------------------------
//
void CImumDaSettingsDataImpl::SettingsDataExceptionL(
    const TInt aErrorCode ) const
    {
    User::Leave( aErrorCode );
    __ASSERT_DEBUG( EFalse, User::Panic(
        KImumInSettingsDataImpl, aErrorCode ) );
    }
