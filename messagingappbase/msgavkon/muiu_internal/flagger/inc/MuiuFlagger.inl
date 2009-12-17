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



// INCLUDES
#include <featmgr.h>            // FeatureManager

// ----------------------------------------------------------------------------
// CMuiuFlags::SetFlag()
// ----------------------------------------------------------------------------
//
inline void CMuiuFlags::SetFlag( 
    const TUint aFlag )
    {
    HandleSetFlag( iFlags, aFlag );
    }

// ----------------------------------------------------------------------------
// CMuiuFlags::ClearFlag()
// ----------------------------------------------------------------------------
//
inline void CMuiuFlags::ClearFlag( 
    const TUint aFlag )
    {
    HandleClearFlag( iFlags, aFlag );
    }

// ----------------------------------------------------------------------------
// CMuiuFlags::Flag()
// ----------------------------------------------------------------------------
//
inline TBool CMuiuFlags::Flag( 
    const TUint aFlag ) const
    {
    return HandleFlag( iFlags, aFlag );
    }            
    
// ----------------------------------------------------------------------------
// CMuiuFlags::ChangeFlag()
// ----------------------------------------------------------------------------
//
inline void CMuiuFlags::ChangeFlag(
    const TUint aFlag,
    const TBool aNewState )
    {
    HandleChangeFlag( iFlags, aFlag, aNewState );
    }
    
// ----------------------------------------------------------------------------
// CMuiuFlags::ChangeGF()
// ----------------------------------------------------------------------------
//
inline void CMuiuFlags::ChangeGF( 
    const TUint aGlobalFlag,
    const TBool aNewState )
    {
    HandleChangeFlag( iGlobalFeatures, aGlobalFlag, aNewState );
    }

// ----------------------------------------------------------------------------
// CMuiuFlags::GF()
// ----------------------------------------------------------------------------
//
inline TBool CMuiuFlags::GF( const TUint aGlobalFeature ) const
    {
    return HandleFlag( iGlobalFeatures, aGlobalFeature );
    }

// ----------------------------------------------------------------------------
// CMuiuFlags::ChangeLF()
// ----------------------------------------------------------------------------
//
inline void CMuiuFlags::ChangeLF( 
    const TUint aLocalFlag,
    const TBool aNewState )
    {
    HandleChangeFlag( iLocalFeatures, aLocalFlag, aNewState );
    }

// ----------------------------------------------------------------------------
// CMuiuFlags::LF()
// ----------------------------------------------------------------------------
//
inline TBool CMuiuFlags::LF( const TUint aLocalFeature ) const
    {
    return HandleFlag( iLocalFeatures, aLocalFeature );
    }

// ----------------------------------------------------------------------------
// CMuiuFlags::HandleSetFlag()
// ----------------------------------------------------------------------------
//
inline void CMuiuFlags::HandleSetFlag( 
    TMuiuFlags& aFlags, 
    const TUint aFlag ) const
    {
    aFlags.SetFlag( aFlag );
    }

// ----------------------------------------------------------------------------
// CMuiuFlags::HandleClearFlag()
// ----------------------------------------------------------------------------
//
inline void CMuiuFlags::HandleClearFlag( 
    TMuiuFlags& aFlags, 
    const TUint aFlag ) const
    {
    aFlags.ClearFlag( aFlag );
    }

// ----------------------------------------------------------------------------
// CMuiuFlags::HandleFlag()
// ----------------------------------------------------------------------------
//
inline TBool CMuiuFlags::HandleFlag( 
    const TMuiuFlags& aFlags, 
    const TUint aFlag ) const
    {
    return aFlags.Flag( aFlag );
    }            
    
// ----------------------------------------------------------------------------
// CMuiuFlags::HandleChangeFlag()
// ----------------------------------------------------------------------------
//
inline void CMuiuFlags::HandleChangeFlag(
    TMuiuFlags& aFlags,
    const TUint aFlag,
    const TBool aNewState ) const
    {
    aFlags.ChangeFlag( aFlag, aNewState );
    }

// End of File

