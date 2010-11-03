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
* Description: msgarrays implementation
*
*/



// INCLUDE FILES
#include "MsgArrays.h"


// CONSTANTS
const TInt KGranularity = 4;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TUidNameInfo::TUidNameInfo
// -----------------------------------------------------------------------------
//	
EXPORT_C TUidNameInfo::TUidNameInfo( TUid aUid, const TDesC& aName )
:iUid( aUid ), 
iName( aName )
    {
    }


// -----------------------------------------------------------------------------
// TUidNameKey::TUidNameKey
// -----------------------------------------------------------------------------
//
EXPORT_C TUidNameKey::TUidNameKey( TKeyCmpText aType ) : TKeyArrayFix( KGranularity, aType, 0 ) 
    {
    }


// -----------------------------------------------------------------------------
// TUidNameKey::Compare
// -----------------------------------------------------------------------------
//
TInt TUidNameKey::Compare( TInt aLeft, TInt aRight ) const
    {
    TDesC* left=( ( TDesC* )At( aLeft ) );
    TDesC* right=( ( TDesC* )At( aRight ) );
    switch ( iCmpType )
        {
        case ECmpFolded:
            return( left->CompareF( *right ) );
        case ECmpCollated:
            return( left->CompareC( *right ) );
        default:
            return( left->Compare( *right ) );
        }
    }


// -----------------------------------------------------------------------------
// CUidNameArray::CUidNameArray
// -----------------------------------------------------------------------------
//
EXPORT_C CUidNameArray::CUidNameArray( TInt aGranularity )
:CArrayFixFlat<TUidNameInfo>( aGranularity ) 
    {
    }


// -----------------------------------------------------------------------------
// CUidNameArray::~CUidNameArray
// -----------------------------------------------------------------------------
//	
EXPORT_C CUidNameArray::~CUidNameArray()
    {
    }

// -----------------------------------------------------------------------------
// CUidNameArray::MdcaCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUidNameArray::MdcaCount() const
    {
    return Count();
    }

// -----------------------------------------------------------------------------
// CUidNameArray::MdcaPoint
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CUidNameArray::MdcaPoint( TInt aIndex ) const 
    { 
    return At( aIndex ).iName;
    }

// -----------------------------------------------------------------------------
// CUidNameArray::Sort
// -----------------------------------------------------------------------------
//
EXPORT_C void CUidNameArray::Sort( TKeyCmpText aTextComparisonType )
    {
    TUidNameKey key( aTextComparisonType );
    CArrayFixBase::Sort( key );
    }


// End of File
