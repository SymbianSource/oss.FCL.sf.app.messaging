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
* Description:  IMSSettingsAccessPointItem.inl
*
*/


// ----------------------------------------------------------------------------
// CIMSSettingsAccessPointItem::Type()
// ----------------------------------------------------------------------------
//
inline TMuiuDynSettingsType CIMSSettingsAccessPointItem::Type() const
    {
    return EMuiuDynSetRadioButtonArray;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsAccessPointItem::Value()
// ----------------------------------------------------------------------------
//
inline TInt64 CIMSSettingsAccessPointItem::Value() const
    {
    return iIapRadioButton;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsAccessPointItem::Value()
// ----------------------------------------------------------------------------
//
inline void CIMSSettingsAccessPointItem::SetValue( const TInt64 aValue )
    {
    iIapRadioButton = aValue;
    }

// End of File
