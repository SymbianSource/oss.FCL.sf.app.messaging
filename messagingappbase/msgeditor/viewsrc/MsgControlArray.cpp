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
* Description:  MsgControlArray implementation
*
*/



// ========== INCLUDE FILES ================================

#include "MsgControlArray.h"               // for CMsgControlArray

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgControlArray::CMsgControlArray
//
// Constructor.
// ---------------------------------------------------------
//
CMsgControlArray::CMsgControlArray( TInt aGranularity )
    : CArrayPtrFlat<CMsgBaseControl>( aGranularity )
    {
    }

// ---------------------------------------------------------
// CMsgControlArray::~CMsgControlArray
//
// Destructor.
// ---------------------------------------------------------
//
CMsgControlArray::~CMsgControlArray()
    {
    ResetAndDestroy();
    }

// ---------------------------------------------------------
// CMsgControlArray::ComponentIndexFromId
//
// Finds a control from the array based on control id aControlId and returns
// its array index.
// ---------------------------------------------------------
//
TInt CMsgControlArray::ComponentIndexFromId( TInt aControlId ) const
    {
    const TInt count = Count();
    TInt ii = 0;

    FOREVER
        {
        if ( ii == count )
            {
            return ( KErrNotFound );
            }

        if ( (*this)[ii]->ControlId() == aControlId )
            {
            break;
            }
        ii++;
        }

    return ( ii );
    }

// ---------------------------------------------------------
// CMsgControlArray::DeleteComponent
//
// Deletes a control from the array.
// ---------------------------------------------------------
//
void CMsgControlArray::DeleteComponent( TInt aIndex )
    {
    delete( (*this)[aIndex] );
    Delete( aIndex );
    }

//  End of File
