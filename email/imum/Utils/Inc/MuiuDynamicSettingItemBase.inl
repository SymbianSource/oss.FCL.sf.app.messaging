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
* Description:  MuiuDynamicSettingItemBase.h
*
*/


// ----------------------------------------------------------------------------
// CMuiuSettingBase::Type()
// ----------------------------------------------------------------------------
//
inline TMuiuDynSettingsType CMuiuSettingBase::Type() const
    {
    return iItemType;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingBase::Value()
// ----------------------------------------------------------------------------
//
inline TInt64 CMuiuSettingBase::Value() const
    {
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingBase::Value()
// ----------------------------------------------------------------------------
//
inline void CMuiuSettingBase::SetValue( const TInt64 /* aValue */ )
    {
    // Do nothing
    }

// ----------------------------------------------------------------------------
// CMuiuSettingBase::Text()
// ----------------------------------------------------------------------------
//
inline const TMuiuSettingsText* CMuiuSettingBase::Text() const
    {
    return NULL;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingBase::VisibleText()
// ----------------------------------------------------------------------------
//
inline const TDesC& CMuiuSettingBase::VisibleText() const
    {
    return KNullDesC();
    }

// ----------------------------------------------------------------------------
// CMuiuSettingBase::SetText()
// ----------------------------------------------------------------------------
//
inline void CMuiuSettingBase::SetText(
    const TMuiuSettingsText* /* aText */ )
    {
    }

// ----------------------------------------------------------------------------
// CMuiuSettingBase::HasLinkArray()
// ----------------------------------------------------------------------------
//
inline TBool CMuiuSettingBase::HasLinkArray() const
    {
    return EFalse;
    }

// End of file
