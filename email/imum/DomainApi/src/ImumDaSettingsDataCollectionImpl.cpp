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
* Description:  ImumDaSettingsDataCollectionImpl.cpp
 *
*/


#include <e32base.h>
#include <msvstd.h>
#include "ImumDaSettingsKeys.h"
#include "ImumDaErrorCodes.h"
#include "ImumDaSettingsDataCollectionImpl.h"
#include "ImumDaMailboxServicesImpl.h"
#include "ImumInSettingsDataCollection.h"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CImumDaSettingsDataCollectionImpl::CImumDaSettingsDataCollectionImpl()
// ----------------------------------------------------------------------------
//
CImumDaSettingsDataCollectionImpl::CImumDaSettingsDataCollectionImpl(
    MImumInSettingsDataCollection& aInDataCollection )
    :
    iInDataCollection( aInDataCollection )
    {
    }

// ----------------------------------------------------------------------------
// CImumDaSettingsDataCollectionImpl::~CImumDaSettingsDataCollectionImpl()
// ----------------------------------------------------------------------------
//
CImumDaSettingsDataCollectionImpl::~CImumDaSettingsDataCollectionImpl()
    {
    }

// ---------------------------------------------------------------------------
// CImumDaSettingsDataCollectionImpl::NewL()
// ---------------------------------------------------------------------------
//
CImumDaSettingsDataCollectionImpl* CImumDaSettingsDataCollectionImpl::NewL(
    MImumInSettingsDataCollection& aInDataCollection )
    {
    CImumDaSettingsDataCollectionImpl* self = NewLC( aInDataCollection );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CImumDaSettingsDataCollectionImpl::NewLC()
// ---------------------------------------------------------------------------
//
CImumDaSettingsDataCollectionImpl* CImumDaSettingsDataCollectionImpl::NewLC(
    MImumInSettingsDataCollection& aInDataCollection )
    {
    CImumDaSettingsDataCollectionImpl* self =
        new ( ELeave ) CImumDaSettingsDataCollectionImpl( aInDataCollection );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// CImumDaMailboxServicesImpl::ConstructL()
// ---------------------------------------------------------------------------
//
void CImumDaSettingsDataCollectionImpl::ConstructL()
    {
    }

// ======================= INTERFACE IMPLEMENTATION ===========================

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection
// CImumDaSettingsDataCollectionImpl::SetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumDaSettingsDataCollectionImpl::SetAttr(
    const TUint aAttributeKey,
    const TInt aValue )
    {
    if ( IsLegalKey( aAttributeKey, EFalse ) )
        {
        return iInDataCollection.SetAttr( aAttributeKey, aValue );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection
// CImumDaSettingsDataCollectionImpl::SetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumDaSettingsDataCollectionImpl::SetAttr(
    const TUint aAttributeKey,
    const TInt64 aValue )
    {
    if ( IsLegalKey( aAttributeKey, EFalse ) )
        {
        return iInDataCollection.SetAttr( aAttributeKey, aValue );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection
// CImumDaSettingsDataCollectionImpl::SetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumDaSettingsDataCollectionImpl::SetAttr(
    const TUint aAttributeKey,
    const TMsvId aValue )
    {
    if ( IsLegalKey( aAttributeKey, EFalse ) )
        {
        return iInDataCollection.SetAttr( aAttributeKey, aValue );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }


// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection
// CImumDaSettingsDataCollectionImpl::SetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumDaSettingsDataCollectionImpl::SetAttr(
    const TUint aAttributeKey,
    const TDesC& aText )
    {
    if ( IsLegalKey( aAttributeKey, EFalse ) )
        {
        return iInDataCollection.SetAttr( aAttributeKey, aText );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection
// CImumDaSettingsDataCollectionImpl::SetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumDaSettingsDataCollectionImpl::SetAttr(
    const TUint aAttributeKey,
    const TDesC8& aText )
    {
    if ( IsLegalKey( aAttributeKey, EFalse ) )
        {
        return iInDataCollection.SetAttr( aAttributeKey, aText );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection
// CImumDaSettingsDataCollectionImpl::GetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumDaSettingsDataCollectionImpl::GetAttr(
    const TUint aAttributeKey,
    TInt& aValue ) const
    {
    if ( IsLegalKey( aAttributeKey, ETrue ) )
        {
        return iInDataCollection.GetAttr( aAttributeKey, aValue );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection
// CImumDaSettingsDataCollectionImpl::GetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumDaSettingsDataCollectionImpl::GetAttr(
    const TUint aAttributeKey,
    TInt64& aValue ) const
    {
    if ( IsLegalKey( aAttributeKey, ETrue ) )
        {
        return iInDataCollection.GetAttr( aAttributeKey, aValue );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection
// CImumDaSettingsDataCollectionImpl::GetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumDaSettingsDataCollectionImpl::GetAttr(
    const TUint aAttributeKey,
    TMsvId& aId ) const
    {
    if ( IsLegalKey( aAttributeKey, ETrue ) )
        {
        return iInDataCollection.GetAttr( aAttributeKey, aId );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection
// CImumDaSettingsDataCollectionImpl::GetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumDaSettingsDataCollectionImpl::GetAttr(
    const TUint aAttributeKey,
    TDes& aText ) const
    {
    if ( IsLegalKey( aAttributeKey, ETrue ) )
        {
        return iInDataCollection.GetAttr( aAttributeKey, aText );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection
// CImumDaSettingsDataCollectionImpl::GetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumDaSettingsDataCollectionImpl::GetAttr(
    const TUint aAttributeKey,
    TDes8& aText ) const
    {
    if ( IsLegalKey( aAttributeKey, ETrue ) )
        {
        return iInDataCollection.GetAttr( aAttributeKey, aText );
        }
    else
        {
        return TImumDaErrorCode::EKeyCannotBeAssigned;
        }
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection
// CImumDaSettingsDataCollectionImpl::operator!=()
// ---------------------------------------------------------------------------
//
TBool CImumDaSettingsDataCollectionImpl::operator!=(
    const MImumDaSettingsDataCollection& aSettingsDataCollection ) const
    {
    const MImumInSettingsDataCollection* InSettingsDataCollection =
        static_cast<const MImumInSettingsDataCollection*>
            ( &aSettingsDataCollection );

    return iInDataCollection.operator!=( *InSettingsDataCollection );
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection
// CImumDaSettingsDataCollectionImpl::operator==()
// ---------------------------------------------------------------------------
//
TBool CImumDaSettingsDataCollectionImpl::operator==(
    const MImumDaSettingsDataCollection& aSettingsDataCollection ) const
    {
    const MImumInSettingsDataCollection* InSettingsDataCollection =
        static_cast<const MImumInSettingsDataCollection*>
            ( &aSettingsDataCollection );

    return iInDataCollection.operator==( *InSettingsDataCollection );
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection
// CImumDaSettingsDataCollectionImpl::Reset()
// ---------------------------------------------------------------------------
//
void CImumDaSettingsDataCollectionImpl::Reset(
    const TUint aAttributeKey )
    {
    return iInDataCollection.Reset( aAttributeKey );
    }

// ---------------------------------------------------------------------------
// From class MImumDaSettingsDataCollection
// CImumDaSettingsDataCollectionImpl::Copy()
// ---------------------------------------------------------------------------
//
TInt CImumDaSettingsDataCollectionImpl::Copy(
    const MImumDaSettingsDataCollection& aSettingsDataCollection )
    {
    const MImumInSettingsDataCollection* InSettingsDataCollection =
        static_cast<const MImumInSettingsDataCollection*>
            ( &aSettingsDataCollection );

    return iInDataCollection.Copy( *InSettingsDataCollection );
    }

// ======================= INTERNAL FUNCTIONALITY ============================

// ---------------------------------------------------------------------------
// CImumDaSettingsDataCollectionImpl::IsLegalKey()
// ---------------------------------------------------------------------------
//
TInt CImumDaSettingsDataCollectionImpl::IsLegalKey(
    const TUint aAttributeKey,
    TBool aReadOperation )
    {
    TBool isLegal = EFalse;

    //TConnectionKeySet
    if( aReadOperation )    //read
        {
        if( aAttributeKey >= TImumDaSettings::EKeyMailboxId &&
        aAttributeKey < TImumDaSettings::EKeyLastSetting )
            {
            isLegal = ETrue;
            }
        }
    else                    //write (EKeyMailboxId cannot be set)
        {
        if( aAttributeKey > TImumDaSettings::EKeyMailboxId &&
        aAttributeKey < TImumDaSettings::EKeyLastSetting )
            {
            isLegal = ETrue;
            }
        }
    //TPreferencesKeySet
    if( aAttributeKey >= TImumDaSettings::EKeyMailboxName &&
        aAttributeKey < TImumDaSettings::ELastPreference )
        {
        isLegal = ETrue;
        }
    //TAutomaticRetrievalKeySet
    if( aAttributeKey >= TImumDaSettings::EKeyAutoNotifications &&
        aAttributeKey < TImumDaSettings::ELastAuto )
        {
        isLegal = ETrue;
        }
    //TMiscKeySet
    if( aAttributeKey >= TImumDaSettings::EKeyAPop &&
        aAttributeKey < TImumDaSettings::ELastMisc )
        {
        isLegal = ETrue;
        }

    return isLegal;
    }


