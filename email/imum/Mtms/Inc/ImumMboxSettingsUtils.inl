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
* Description: ImumMboxSettingsUtils.inl
*
*/


#include <ImumInternalApi.h>
#include <ImumInMailboxServices.h>
#include <ImumInMailboxUtilities.h>

// ---------------------------------------------------------------------------
// ImumMboxSettingsUtils::QuickIntL()
// ---------------------------------------------------------------------------
//
template<class T>
T ImumMboxSettingsUtils::QuickLoadL( 
    CImumInternalApi& aEmailApi,
    const TMsvId aMailboxId,
    const TUint aAttributeKey,
    const TInt aConnectionIndex )
    {
    CImumInSettingsData* settings = 
        aEmailApi.MailboxServicesL().LoadMailboxSettingsL( aMailboxId );
    CleanupStack::PushL( settings );

    T type = QuickGetL<T>( *settings, aAttributeKey, aConnectionIndex );
    
    CleanupStack::PopAndDestroy( settings );
    settings = NULL;
        
    return type;
    }

// ---------------------------------------------------------------------------
// ImumMboxSettingsUtils::QuickIntL()
// ---------------------------------------------------------------------------
//
template<class T>
T ImumMboxSettingsUtils::QuickGetL( 
    const CImumInSettingsData& aSettings,
    const TUint aAttributeKey,
    const TInt aConnectionIndex )
    {
    T type;
    TInt error = KErrNone;
    
    if ( aConnectionIndex == KErrNotFound )
        {        
        error = aSettings.GetAttr( aAttributeKey, type );
        }
    else
        {
        error = aSettings.GetInSetL( aConnectionIndex ).GetAttr( aAttributeKey, type );
        }
        
    User::LeaveIfError( error );    
        
    return type;
    }

// End of File
