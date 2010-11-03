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
* Description:  ImumInSettingsDataArray.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>

#include "ImumInSettingsDataArray.h"
#include "ImumDaErrorCodes.h"
#include "ImumDaSettingsKeys.h"
#include "ImumInSettingsKeys.h"
#include "ImumInSettingsDataAttribute.h"
#include "ImumMboxDefaultData.h"
#include "ImumPanic.h"
#include "ImumUtilsLogging.h"

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
// CImumInSettingsDataArray::CImumInSettingsDataArray()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataArray::CImumInSettingsDataArray()
    {
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataArray::~CImumInSettingsDataArray()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataArray::~CImumInSettingsDataArray()
    {
    iSettings.ResetAndDestroy();
    iSettings.Close();

    delete iSearchItem;
    iSearchItem = NULL;
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataArray::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumInSettingsDataArray::ConstructL()
    {
    // This item is need to search the array for the specific id. Only required
    // value is the id of the item, other can be ignored. The id is specified
    // just before the search begins.
    iSearchItem = CImumInSettingsDataAttribute::NewL(
        0, ECmpTInt32, 0, NULL, NULL );
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataArray::NewL()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataArray* CImumInSettingsDataArray::NewL()
    {
    CImumInSettingsDataArray* self = NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CImumInSettingsDataArray::NewLC()
// ----------------------------------------------------------------------------
//
CImumInSettingsDataArray* CImumInSettingsDataArray::NewLC()
    {
    CImumInSettingsDataArray* self = new ( ELeave ) CImumInSettingsDataArray();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataArray::HasItem()
// ---------------------------------------------------------------------------
//
TBool CImumInSettingsDataArray::HasItem(
    const TUint aAttributeKey,
    TInt& aIndex ) const
    {
    iSearchItem->SetId( aAttributeKey );
    TIdentityRelation<CImumInSettingsDataAttribute> relation(
        CImumInSettingsDataAttribute::IdCompare );

    aIndex = iSettings.Find( iSearchItem, relation );

    return ( aIndex >= 0 );
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataArray::GetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataArray::GetAttr(
    const TUint aAttributeKey,
    CImumInSettingsDataAttribute*& aAttribute ) const
    {
    // Try to find the key from the settings key list
    TInt index = KErrNotFound;
    TInt error = KErrNone;

    // Find the item from the list
    if ( HasItem( aAttributeKey, index ) )
        {
        aAttribute = iSettings[index];
        }
    else
        {
        error = TImumDaErrorCode::ESettingNotFound;
        }

    return error;
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataArray::SetAttr()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataArray::SetAttr(
    CImumInSettingsDataAttribute& aAttribute )
    {
    TInt error = KErrNone;

    // Try to find the key from the settings key list
    TInt index = KErrNotFound;

    // Find the item from the list
    if ( !HasItem( aAttribute.Id(), index ) )
        {
        TRAP( error, iSettings.AppendL( &aAttribute ) );
        }
    // Key exists, replace the old value with the new one
    else
        {
        delete iSettings[index];
        iSettings.Remove( index );
        iSettings.Insert( &aAttribute, index );
        }

    return error;
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataArray::ResetAttr()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataArray::ResetAttr( const TUint aAttributeKey )
    {
    CImumInSettingsDataAttribute* attr = NULL;
    if ( GetAttr( aAttributeKey, attr ) == KErrNone )
        {
        attr->Reset();
        }
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataArray::ResetAll()
// ---------------------------------------------------------------------------
//
void CImumInSettingsDataArray::ResetAll( const RArray<TUint>& /* aExcludeList */ )
    {
	// Start from the last item
	TInt settingsCount = iSettings.Count() - 1;
    for ( TInt setting = settingsCount; setting >= 0; --setting )
        {
        iSettings[setting]->Reset();
        }
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataArray::CloneL()
// ---------------------------------------------------------------------------
//
CImumInSettingsDataArray* CImumInSettingsDataArray::CloneL() const
    {
    CImumInSettingsDataArray* clone = NewLC();

    for ( TInt setting = iSettings.Count(); --setting >= 0; )
        {
        clone->iSettings.Append( iSettings[setting]->CloneL() );
        }

    CleanupStack::Pop( clone );

    return clone;
    }

// ---------------------------------------------------------------------------
// CImumInSettingsDataArray::Count()
// ---------------------------------------------------------------------------
//
TInt CImumInSettingsDataArray::Count() const
    {
    return iSettings.Count();
    }



// End of File
