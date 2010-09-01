/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation for DllMain.
*
*/



#include "msgindicatorpluginimplementation.h"

#include <implementationproxy.h>

// Define the interface UIDs
// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x10275096, 
                                CMsgIndicatorPluginImplementation::NewL )
	};
	
// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy
        (TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(ImplementationTable[0]);
    return ImplementationTable;
    }
    
// End of file
