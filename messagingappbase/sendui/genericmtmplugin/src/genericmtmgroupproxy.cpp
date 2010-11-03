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
* Description:   Table of plugins for ECom
*
*/




#include <e32std.h>
#include <implementationproxy.h>

#include "GenericMtmPluginUids.hrh"
#include "genericmtmplugin.h"

const TImplementationProxy KImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( KGenericMtmPluginUid,
        CGenericMtmPlugin::NewL)
    };


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// ImplementationGroupProxy for ECOM
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(KImplementationTable) / sizeof(TImplementationProxy);

    return KImplementationTable;
    }
