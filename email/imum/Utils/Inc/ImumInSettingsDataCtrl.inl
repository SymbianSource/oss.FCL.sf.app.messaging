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
* Description: ImumInSettingsDataCtrl.inl
*
*/


#include "ImumDaErrorCodes.h"
#include "ImumPanic.h"
#include "ImumInSettingsDataAttribute.h"
#include "ImumInSettingsDataImpl.h"             // CImumInSettingsDataImpl
#include "ImumInSettingsDataCollectionImpl.h"   // CImumInSettingsDataCollectionImpl
#include "ImumInSettingsDataArray.h"    

// ---------------------------------------------------------------------------
// ImumInSettingsDataCtrl::CreateToArray()
// ---------------------------------------------------------------------------
//
template<class TDefaultDataType, class TAttributeType, class TDefaultType>
TInt ImumInSettingsDataCtrl::CreateToArray(
    CImumInSettingsDataArray& aArray,
    const TDefaultDataType* aDefaultData,
    const TUint aAttributeKey,
    const TInt aAttributeType,
    const TAttributeType& aAttributeValue,
    TDefaultType& aEmptyDefaultValue )
    {
    CImumInSettingsDataAttribute* attribute = NULL;
    TInt error = KErrNone;
    
    // In case the default data is not provided, it means the default data
    // itself is being constructed, thus this part can be skipped and 
    // the object itself shall be filled with correct data. 
    if ( aDefaultData )
        {        
        error = aDefaultData->GetAttr( aAttributeKey, aEmptyDefaultValue );
        }
    // The default data attribute must contain the default data in itself,
    // so it will be possible to clone the attribute later on.        
    else
        {
        // As this template method can be executed for both number and text
        // attributes, the copy part must be different here and executed 
        // seperately for descriptors and built-in types.
        if ( aAttributeType == ECmpNormal )
            {
            reinterpret_cast<TDes*>( &aEmptyDefaultValue )->Copy( 
                *reinterpret_cast<const TDesC*>( &aAttributeValue ) );
            }
        else  if ( aAttributeType == ECmpNormal8 )
            {
            reinterpret_cast<TDes8*>( &aEmptyDefaultValue )->Copy( 
                *reinterpret_cast<const TDesC8*>( &aAttributeValue ) );
            }
        else
            {
            aEmptyDefaultValue = aAttributeValue;
            }
        }
        
    if ( error == KErrNone )
        {        
        TRAP( error, attribute = CreateAttributeL( 
            aAttributeKey,
            aAttributeType,
            aAttributeValue,
            aEmptyDefaultValue ) );
        }
        
    if ( error == KErrNone )
        {        
        aArray.SetAttr( *attribute );
        }
    
    return error;
    }

// ---------------------------------------------------------------------------
// ImumInSettingsDataCtrl::CreateToArray()
// ---------------------------------------------------------------------------
//
template<class TAttributeType>
TInt ImumInSettingsDataCtrl::RetrieveFromArray(
    CImumInSettingsDataArray& aArray,
    const TUint aAttributeKey,
    const TInt /* aAttributeType */,
    TAttributeType& aAttributeValue )
    {
    CImumInSettingsDataAttribute* attribute = NULL;
 
    TInt error = aArray.GetAttr( aAttributeKey, attribute );
    
    if ( error == KErrNone )
        {        
        attribute->Value( aAttributeValue );   
        }
 
    return error;
    }
    
// ---------------------------------------------------------------------------
// ImumInSettingsDataCtrl::CreateAttributeL()
// ---------------------------------------------------------------------------
//
template<class TValueType, class TDefaultType>
CImumInSettingsDataAttribute* ImumInSettingsDataCtrl::CreateAttributeL( 
    const TUint aAttributeId,
    const TInt aAttributeType,             
    const TValueType& aAttributeValue,
    const TDefaultType& aAttributeDefault )
    {
    // Size does matter!
    TUint size( 0 );

    // Ok, this may look a bit ugly, but at this point it is certain
    // that attribute is going to be descriptor, so the conversion
    // is safe.   
    if ( aAttributeType == ECmpNormal )
        {             
        size = reinterpret_cast<const TDesC*>( &aAttributeValue )->Length();
        }
    else if ( aAttributeType == ECmpNormal8 )
        {             
        size = reinterpret_cast<const TDesC8*>( &aAttributeValue )->Length();
        }
    else
        {
        size = sizeof( aAttributeValue );
        }
        
    // After calculating the size, create the actual container object
    return CImumInSettingsDataAttribute::NewL(    
        aAttributeId, 
        aAttributeType, 
        size, 
        &aAttributeValue, 
        &aAttributeDefault );          
    }

    
    
    
// ---------------------------------------------------------------------------
// ImumInSettingsDataCtrl::Copy()
// ---------------------------------------------------------------------------
//
template<class T>
TInt ImumInSettingsDataCtrl::Copy( 
    MImumDaSettingsDataCollection& aDestination,
    const MImumDaSettingsDataCollection& aSource, 
    const TUint aAttributeKey, 
    T& aAttribute, 
    const TInt aErrorCode )
    {
    TInt error = aSource.GetAttr( 
        aAttributeKey, aAttribute );
    if ( !error )
        {
        error = aDestination.SetAttr( 
            aAttributeKey, aAttribute ); 
        }

    if ( error )
        {
        error = aErrorCode;
        }
        
    return error;        
    }
    
// ---------------------------------------------------------------------------
// ImumInSettingsDataCtrl::CompareText8()
// ---------------------------------------------------------------------------
//
template<TInt L>
TBool ImumInSettingsDataCtrl::CompareText8( 
    const MImumDaSettingsDataCollection& aDestinationSettings,
    const MImumDaSettingsDataCollection& aSourceSettings, 
    const TUint aAttributeKey )
    {
    TBuf8<L> destination;
    TBuf8<L> source;
    TBool result = EFalse;
    
    TInt error = aDestinationSettings.GetAttr( 
        aAttributeKey, destination );
        
    if ( !error )
        {        
        error = aSourceSettings.GetAttr( 
            aAttributeKey, source );
        }
    
    if ( !error )
        {
        result = !source.CompareC( destination ); 
        }

    return result;        
    }
    
// ---------------------------------------------------------------------------
// ImumInSettingsDataCtrl::CompareText()
// ---------------------------------------------------------------------------
//
template<TInt L>
TBool ImumInSettingsDataCtrl::CompareText( 
    const MImumDaSettingsDataCollection& aDestinationSettings,
    const MImumDaSettingsDataCollection& aSourceSettings, 
    const TUint aAttributeKey )
    {
    TBuf<L> destination;
    TBuf<L> source;
    TBool result = EFalse;
    TInt error = aDestinationSettings.GetAttr( 
        aAttributeKey, destination );
        
    if ( !error )
        {        
        error = aSourceSettings.GetAttr( 
            aAttributeKey, source );
        }
    
    if ( !error )
        {
        result = !source.CompareC( destination ); 
        }

    return result;        
    }    
    
// ---------------------------------------------------------------------------
// ImumInSettingsDataCtrl::CompareNum()
// ---------------------------------------------------------------------------
//
template<class T>
TBool ImumInSettingsDataCtrl::CompareNum( 
    const MImumDaSettingsDataCollection& aDestinationSettings,
    const MImumDaSettingsDataCollection& aSourceSettings, 
    const TUint aAttributeKey )
    {
    T dest;
    T source;
    TBool result = EFalse;
    TInt error = aDestinationSettings.GetAttr( 
        aAttributeKey, dest );
        
    if ( !error )
        {        
        error = aSourceSettings.GetAttr( 
            aAttributeKey, source );
        }
        
    if ( !error )
        {
        result = ( dest == source ); 
        }

    return result; 
    }    


