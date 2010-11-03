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
* Description:  ImumInSettingsDataAttribute.inl
*
*/


#include <e32cmn.h>

// ---------------------------------------------------------------------------
// CImumInSettingsDataAttribute::Value()
// ---------------------------------------------------------------------------
//
template<class T>
void CImumInSettingsDataAttribute::Value( T& aAttribute ) const
    {
    // Copy the value in the 
    if ( iType == ECmpNormal )
        {
        reinterpret_cast<TDes*>( &aAttribute )->Copy( 
            *reinterpret_cast<TDesC*>( iValue ) );
        }
    else if ( iType == ECmpNormal8 )
        {
        reinterpret_cast<TDes8*>( &aAttribute )->Copy( 
            *reinterpret_cast<TDesC8*>( iValue ) );
        }
    else
        {
        aAttribute = *reinterpret_cast<const T*>( iValue );
        }                    
    }       

// ---------------------------------------------------------------------------
// CImumInSettingsDataAttribute::DeleteNumber()
// ---------------------------------------------------------------------------
//
template<class T>
void CImumInSettingsDataAttribute::DeleteNumber()
    {
    delete reinterpret_cast<T*>( iValue );
    delete reinterpret_cast<T*>( iDefault );
    }


// ---------------------------------------------------------------------------
// CImumInSettingsDataAttribute::DoAllocL()
// ---------------------------------------------------------------------------
//
template<class T>
void CImumInSettingsDataAttribute::DoAllocL( 
    const TAny* aValue,
    const TAny* aDefault )
    {
    // First step is to reserve memory, so the value can be stored permanently
    T* value = new ( ELeave ) T();
    T* defaultValue = new ( ELeave ) T();
    
    // Get the content of the pointer and store it into the attribute
    *value = *reinterpret_cast<const T*>( aValue );
    *defaultValue = *reinterpret_cast<const T*>( aDefault );
    
    // Finally store the pointer of the attribute
    iValue = value;
    iDefault = defaultValue;
    }    

// End of File