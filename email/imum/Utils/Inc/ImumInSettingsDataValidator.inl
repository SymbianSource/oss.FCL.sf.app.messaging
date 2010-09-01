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
* Description:  ImumInSettingsDataValidator.inl
*
*/



// ---------------------------------------------------------------------------
// CImumInSettingsDataValidator::VerifyKeyL()
// ---------------------------------------------------------------------------
//
template<class TType, class TSettings>
TType CImumInSettingsDataValidator::VerifyKeyL( 
    TSettings& aSettings,
    const TUint aAttributeKey,
    const TInt aMinValue,
    const TInt aMaxValue,
    const TInt aErrorCode ) const
    {
    TType value( 0 );
    if ( !( !aSettings.GetAttr( aAttributeKey, value ) &&
         value >= aMinValue &&
         value <= aMaxValue ) )
        {
        User::Leave( aErrorCode );
        }     
        
    return value;        
    }


// ---------------------------------------------------------------------------
// CImumInSettingsDataValidator::ValidateKey()
// ---------------------------------------------------------------------------
//
template<class TType, class TSettings>
TType CImumInSettingsDataValidator::ValidateKey( 
    TSettings& aSettings,
    const TUint aAttributeKey,
    const TInt aMinValue,
    const TInt aMaxValue )
    {
    TType value( 0 );
    if ( !( !aSettings.GetAttr( aAttributeKey, value ) &&
         value >= aMinValue &&
         value <= aMaxValue ) )
        {
        aSettings.Reset( aAttributeKey );        
        aSettings.GetAttr( aAttributeKey, value );
        iFlags.SetFlag( ESettingsHaveChanged );        
        }
        
    return value;        
    }
