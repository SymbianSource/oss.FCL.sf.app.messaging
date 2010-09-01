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
* Description:  MuiuDynamicSettingItemEditValue.inl
*
*/


// ----------------------------------------------------------------------------
// CMuiuSettingsEditValue::Value()
// ----------------------------------------------------------------------------
//
inline TInt64 CMuiuSettingsEditValue::Value() const
    {
    return iUserValue;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsEditValue::Value()
// ----------------------------------------------------------------------------
//
inline void CMuiuSettingsEditValue::SetValue( const TInt64 aValue )
    {
    iUserValue = aValue;
    }

// End of file
