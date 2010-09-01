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
* Description:  Contains functions to show notes in UI
*
*
*/


#include <aknclearer.h>         // CAknLocalScreenClearer

// ----------------------------------------------------------------------------
// CIMSSettingsNoteUi::ShowQuery
// ----------------------------------------------------------------------------
//
template<TInt TLength>
TInt CIMSSettingsNoteUi::ShowQueryL(
    const TInt aTextResource,
    const TDesC& aAdditionalText,    
    const TInt aDialogResource,
    const TBool& aClearBackground,
    const CAknQueryDialog::TTone& aTone )
    {                                   
    // Combine text with resource
    TBuf<TLength> text;
    CIMSSettingsNoteUi::MakeStringL(
        text,
        aTextResource, 
        aAdditionalText );                    
                            
    // Create dialog and execute the dialog
    return ShowQueryL( text, aDialogResource, aClearBackground, aTone );        
    }