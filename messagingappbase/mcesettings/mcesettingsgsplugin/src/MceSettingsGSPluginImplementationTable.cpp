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
* Description:  
*        ECOM proxy table for this plugin
*
*/



// System includes
#include <e32std.h>
#include <implementationproxy.h>

// User includes
#include "MceSettingsGSPlugin.h"

// Constants

// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    // UPDATE THIS - Uid for plugin implementation:
    IMPLEMENTATION_PROXY_ENTRY( 0x10282EE0, CMceSettingsGSPlugin::NewL )
    };

// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// Gate/factory function
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// End of File
