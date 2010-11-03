/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*      Implementation of Scheme handler proxy table
*
*/



#include "SmsHandler.h"
#include "MmsToHandler.h"
#include "LocalAppHandler.h"
#include <e32std.h>
#include "SchemePluginDefs.hrh"
#include <implementationproxy.h>

// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(SCHEME_MMS_TO_IMP, CMmsToHandler::NewL),
	IMPLEMENTATION_PROXY_ENTRY(SCHEME_SMS_IMP, CSmsHandler::NewL),
	IMPLEMENTATION_PROXY_ENTRY(SCHEME_LOCALAPP_IMP, CLocalAppHandler::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
