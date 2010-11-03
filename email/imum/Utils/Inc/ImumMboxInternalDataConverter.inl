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
* Description:  ImumMboxInternalDataConverter.inl
*
*/


#include "ImumMboxData.h"
#include "ImumInSettingsDataImpl.h"
#include "ImumInSettingsDataCollectionImpl.h"

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::FillText8InternalL()
// ---------------------------------------------------------------------------
//
template<TInt TAttrSize, class TAttrSettings>
void CImumMboxInternalDataConverter::FillText8InternalL(
    TAttrSettings& aSettings,
    const TUint aAttributeKey,
    const TDesC8& aText )
    {
    TBuf8<TAttrSize> text;         
    text.Copy( aText.Left( TAttrSize ) );
    User::LeaveIfError( aSettings.SetAttr( aAttributeKey, text ) );
    }

// ---------------------------------------------------------------------------
// CImumMboxInternalDataConverter::FillTextInternalL()
// ---------------------------------------------------------------------------
//
template<TInt TAttrSize, class TAttrSettings>
void CImumMboxInternalDataConverter::FillTextInternalL(
    TAttrSettings& aSettings,
    const TUint aAttributeKey,
    const TDesC& aText )
    {
    TBuf<TAttrSize> text;         
    text.Copy( aText.Left( TAttrSize ) );
    User::LeaveIfError( aSettings.SetAttr( aAttributeKey, text ) );
    }      
   
// End of File

