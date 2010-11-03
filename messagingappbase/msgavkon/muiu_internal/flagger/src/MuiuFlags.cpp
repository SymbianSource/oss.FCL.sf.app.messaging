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
* Description:   This file contains definitions to flag functions
*
*/



// INCLUDE FILES
#include <e32base.h>        
#include "MuiuFlags.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
const TInt KMuiuFlagLast = 64;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ----------------------------------------------------------------------------
// TMuiuFlags::TMuiuFlags()
// ----------------------------------------------------------------------------
//
EXPORT_C TMuiuFlags::TMuiuFlags( const TUint64 aFlags ) 
    : 
    iFlags( aFlags )
    {
    }

// ----------------------------------------------------------------------------
// TMuiuFlags::operator=()
// ----------------------------------------------------------------------------
//
EXPORT_C TMuiuFlags& TMuiuFlags::operator=( const TMuiuFlags& aFlags )
    {
    // Copy the flags
    for ( TInt flag = KMuiuFlagLast; --flag >= 0; )
        {
        ChangeFlag( flag, aFlags.Flag( flag ) );
        }
        
    return *this;
    }
    
// ----------------------------------------------------------------------------
// TMuiuFlags::operator=()
// ----------------------------------------------------------------------------
//
EXPORT_C TMuiuFlags& TMuiuFlags::operator=( const TUint64 aFlags )
    {
    // Copy the flags
    for ( TInt flag = KMuiuFlagLast; --flag >= 0; )
        {
        ChangeFlag( flag, ( aFlags >> flag ) & 0x01 );
        }
        
    return *this;
    }    


// End of File
