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
* Description:  IMSSettingsBaseUI.inl
*
*/



// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::ToUid()
// ----------------------------------------------------------------------------
//
inline TUid CIMSSettingsBaseUI::ToUid( const TUint32 aId ) const
    {
    return TUid::Uid( aId );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsBaseUI::SafeStringLoad()
// ----------------------------------------------------------------------------
//
inline TInt CIMSSettingsBaseUI::SafeStringLoad(
    const TInt aResource,
    HBufC*& aString ) const
    {
    TRAPD( error, aString = StringLoader::LoadL( aResource ) );

    return error;
    }

// End of File
