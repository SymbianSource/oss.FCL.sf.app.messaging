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
* Description: 
*     Main class. Dll entry point
*
*/



#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "AlwaysOnlineEmailPlugin.h"

//--------------------------------
// Exported proxy for instantiation method resolution
// Use the interface UID and Implementation factory method
// as a pair for ECom instantiation.
//--------------------------------
const TImplementationProxy ImplementationTable[] = 
	{
		{{0x101F85ED},	(TProxyNewLPtr)CEComEmailPlugin::NewL}
	};


//--------------------------------
// ImplementationGroupProxy()
// Give the table to the caller
//--------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}


