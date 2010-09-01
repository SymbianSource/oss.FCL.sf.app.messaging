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
* Description:  ImumMboxDefaultData.inl
*
*/


#include "IMASCenRepControl.h"

// ---------------------------------------------------------------------------
// CImumMboxDefaultData::UpdateData()
// ---------------------------------------------------------------------------
//
template<class TDataType, class TSettingType>
void CImumMboxDefaultData::UpdateData(
    TDataType& aDefaultData, 
    const TUint aSettingsKey, 
    const TUint32 aCRKey, 
    const TSettingType& aDefaultValue )
    {    
    TSettingType cenrepValue;       
    
    // Begin filling the value
    TInt error = iCenRepControl->GetFromAccount( 
        aCRKey, cenrepValue );
    
    aDefaultData.SetAttr( 
        aSettingsKey, 
        !error ? cenrepValue : aDefaultValue );
    }

// ---------------------------------------------------------------------------
// CImumMboxDefaultData::UpdateData()
// ---------------------------------------------------------------------------
//
template<class TDataType>
void CImumMboxDefaultData::UpdateData(
    TDataType& aDefaultData, 
    const TUint aSettingsKey, 
    const TUint32 aCRKeyLow, 
    const TUint32 aCRKeyHigh, 
    const TInt64& aDefaultValue )
    {    
    TInt64 cenrepValue = 0;   
    
    // Begin filling the value
    TInt error = iCenRepControl->GetFromAccount( 
        aCRKeyLow, aCRKeyHigh, cenrepValue );
    
    aDefaultData.SetAttr( 
        aSettingsKey, 
        !error ? cenrepValue : aDefaultValue );
    }