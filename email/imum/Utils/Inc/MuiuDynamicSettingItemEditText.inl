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
* Description:  MuiuDynamicSettingItemEditText.inl
*
*/


// ----------------------------------------------------------------------------
// CMuiuSettingsEditText::Text()
// ----------------------------------------------------------------------------
//
inline const TMuiuSettingsText* CMuiuSettingsEditText::Text() const
    {
    return iUserText;
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsEditText::SetText()
// ----------------------------------------------------------------------------
//
inline void CMuiuSettingsEditText::SetText(
    const TMuiuSettingsText* aText )
    {
    if ( aText )
        {
        iUserText->Copy( *aText );
        }
    }

// ----------------------------------------------------------------------------
// CMuiuSettingsEditText::SetEmptyText()
// ----------------------------------------------------------------------------
//
inline void CMuiuSettingsEditText::SetEmptyText( const TDesC& aEmptyText )
    {
    iEmptyText->Copy( aEmptyText );
    }

// End of file
