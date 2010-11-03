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
* Description:  ImumInSettingsDataAttribute.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>

#include "ImumInSettingsDataAttribute.h"
#include "ImumInSettingsDataCtrl.h"
#include "ImumUtilsLogging.h"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CImumInSettingsDataAttribute::NewL()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataAttribute* CImumInSettingsDataAttribute::NewL(
    const TUint aAttributeId,
    const TInt aAttributeType,
    const TUint aAttributeSize,
    const TAny* aAttributeValue,
    const TAny* aAttributeDefault )
    {
    CImumInSettingsDataAttribute* self = NewLC(
        aAttributeId,
        aAttributeType,
        aAttributeSize,
        aAttributeValue,
        aAttributeDefault );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CImumInSetting::NewLC()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataAttribute* CImumInSettingsDataAttribute::NewLC(
    const TUint aAttributeId,
    const TInt aAttributeType,
    const TUint aAttributeSize,
    const TAny* aAttributeValue,
    const TAny* aAttributeDefault )
    {
    CImumInSettingsDataAttribute* self =
        new ( ELeave ) CImumInSettingsDataAttribute();
    CleanupStack::PushL( self );
    self->ConstructL(
        aAttributeId,
        aAttributeType,
        aAttributeSize,
        aAttributeValue,
        aAttributeDefault );

    return self;
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataAttribute::CImumInSettingsDataAttribute()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataAttribute::CImumInSettingsDataAttribute()
    :
    iType( 0 ),
    iSize( 0 ),
    iValue( NULL ),
    iDefault( NULL )
    {
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataAttribute::~CImumInSettingsDataAttribute()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataAttribute::~CImumInSettingsDataAttribute()
    {
    Cleanup();
    iValue = NULL;
    iDefault = NULL;
    }


// ----------------------------------------------------------------------------
// CImumInSettingsDataAttribute::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumInSettingsDataAttribute::ConstructL(
    const TUint aAttributeId,
    const TInt aAttributeType,
    const TUint aAttributeSize,
    const TAny* aAttributeValue,
    const TAny* aAttributeDefault )
    {
    SetAttributeL(
        aAttributeId,
        aAttributeType,
        aAttributeSize,
        aAttributeValue,
        aAttributeDefault );
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataAttribute::Cleanup()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataAttribute::Cleanup()
    {
    // Delete attribute data accoding to its type
    if ( ImumInSettingsDataCtrl::IsText( iType ) )
        {
        DeleteText();
        }
    else if ( iType == ECmpTInt64 )
        {
        DeleteNumber<TInt64>();
        }
    else
        {
        DeleteNumber<TInt>();
        }
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataAttribute::DeleteText()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataAttribute::DeleteText()
    {
    // Doing the deletion. Both value and default must be deleted from
    // the memory as both of the items are objects.
    if ( iType == ECmpNormal )
        {
        delete reinterpret_cast<HBufC*>( iValue );
        delete reinterpret_cast<HBufC*>( iDefault );
        }
    else if ( iType == ECmpNormal8 )
        {
        delete reinterpret_cast<HBufC8*>( iValue );
        delete reinterpret_cast<HBufC8*>( iDefault );
        }
    else
        {
        // This just can't simply happen, since it would be a number
        }

    iValue = NULL;
    iDefault = NULL;
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataAttribute::SetAttributeL()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataAttribute::SetAttributeL(
    const TUint aAttributeId,
    const TInt aAttributeType,
    const TUint aAttributeSize,
    const TAny* aAttributeValue,
    const TAny* aAttributeDefault )
    {
    // Make sure that previous item is cleaned up
    Cleanup();

    iId     = aAttributeId;
    iType   = aAttributeType;
    iSize   = aAttributeSize;

    AllocL( aAttributeValue, aAttributeDefault );
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataAttribute::AllocL()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataAttribute::AllocL(
    const TAny* aValue,
    const TAny* aDefault )
    {
    // Allocate data only if the values contains valid information
    if ( aValue && aDefault )
        {
        // Create copy of the text
        if ( ImumInSettingsDataCtrl::IsText( iType ) )
            {
            iValue = reinterpret_cast<const TDes*>( aValue )->AllocL();
            iDefault = reinterpret_cast<const TDes*>( aDefault )->AllocL();
            }
        else if ( iType == ECmpTInt64 )
            {
            DoAllocL<TInt64>( aValue, aDefault );
            }
        else
            {
            DoAllocL<TInt>( aValue, aDefault );
            }
        }
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataAttribute::CloneL()
// ---------------------------------------------------------------------------
//
CImumInSettingsDataAttribute* CImumInSettingsDataAttribute::CloneL()
    {
    return NewL( iId, iType, iSize, iValue, iDefault );
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataAttribute::Copy()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataAttribute::Copy(
    const CImumInSettingsDataAttribute& aAttribute )
    {
    TRAP_IGNORE( SetAttributeL(
        aAttribute.iId,
        aAttribute.iType,
        aAttribute.iSize,
        aAttribute.iValue,
        aAttribute.iDefault ) );
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataAttribute::Reset()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataAttribute::Reset()
    {
    // Copy the value in the
    if ( iType == ECmpNormal )
        {
        // Only copy if the descriptor contents differ
        if( reinterpret_cast<TDes*>( iValue )->Compare(
        		*reinterpret_cast<TDesC*>( iDefault ) ) )
        	reinterpret_cast<TDes*>( iValue )->Copy(
        			*reinterpret_cast<TDesC*>( iDefault ) );
        }
    else if ( iType == ECmpNormal8 )
        {
        // Only copy if the descriptor contents differ
        if( reinterpret_cast<TDes8*>( iValue )->Compare(
        		*reinterpret_cast<TDesC8*>( iDefault ) ) )
        	reinterpret_cast<TDes8*>( iValue )->Copy(
        			*reinterpret_cast<TDesC8*>( iDefault ) );
        }
    else
        {
        memmove( iValue, iDefault, iSize );
        }
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataAttribute::operator==()
// ---------------------------------------------------------------------------
//
TBool CImumInSettingsDataAttribute::operator==(
    const CImumInSettingsDataAttribute& aAttribute )
    {
    return ( iValue == aAttribute.iValue );
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataAttribute::operator!=()
// ---------------------------------------------------------------------------
//
TBool CImumInSettingsDataAttribute::operator!=(
    const CImumInSettingsDataAttribute& aAttribute )
    {
    return ( iValue != aAttribute.iValue );
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataAttribute::SetId()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataAttribute::SetId( const TUint aId )
    {
    iId = aId;
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataAttribute::IdCompare()
// ---------------------------------------------------------------------------
//
TBool CImumInSettingsDataAttribute::IdCompare(
    const CImumInSettingsDataAttribute& aLeft,
    const CImumInSettingsDataAttribute& aRight )
    {
    return ( aLeft.Id() == aRight.Id() );
    }


// ---------------------------------------------------------------------------
// CImumInSettingsDataAttribute::Id()
// ---------------------------------------------------------------------------
//
TUint CImumInSettingsDataAttribute::Id() const
    {
    return iId;
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataAttribute::Type()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataAttribute::Type() const
    {
    return iType;
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataAttribute::Log()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataAttribute::Log() const
    {
    IMUM_CONTEXT( CImumInSettingsDataAttribute::Log, 0, KLogDataAttr );
    IMUM_IN();

    IMUM0( 0, "------------------------------------------------------" );
    IMUM1( 0, "Attribute: 0x%x", this );
    IMUM1( 0, "Attribute ID: 0x%x", iId );
    IMUM1( 0, "Attribute type: 0x%x", iType );
    IMUM1( 0, "Attribute size: 0x%x", iSize );
    IMUM1( 0, "Attribute iValue; addr: 0x%x", iValue );
    IMUM1( 0, "Attribute iDefault; addr: 0x%x", iDefault );
    IMUM0( 0, "------------------------------------------------------" );

    IMUM_OUT();
    }


// End of File
