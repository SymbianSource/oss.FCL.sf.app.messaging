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
* Description:   This file contains function definitions to class
*
*/



// ----------------------------------------------------------------------------
// TMuiuFlags::SetFlag()
// ----------------------------------------------------------------------------
//
inline void TMuiuFlags::SetFlag( const TUint aFlag )
    {
    iFlags |= ( static_cast<TUint64>( 0x01 ) << aFlag );
    }


// ----------------------------------------------------------------------------
// TMuiuFlags::ClearFlag()
// ----------------------------------------------------------------------------
//
inline void TMuiuFlags::ClearFlag( const TUint aFlag )
    {
    iFlags &= ~( static_cast<TUint64>( 0x01 ) << aFlag );
    }

// ----------------------------------------------------------------------------
// TMuiuFlags::Flag()
// ----------------------------------------------------------------------------
//
inline TBool TMuiuFlags::Flag( const TUint aFlag ) const
    {
    return ( iFlags >> aFlag ) & 0x01;
    }
    
// ----------------------------------------------------------------------------
// TMuiuFlags::ChangeFlag()
// ----------------------------------------------------------------------------
//
inline void TMuiuFlags::ChangeFlag(
    const TUint aFlag,
    const TBool aNewState )
    {
    if ( aNewState )
        {
        SetFlag( aFlag );
        }
    else
        {
        ClearFlag( aFlag );
        }
    }    
    

// End of File
