/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   DLL entry point and ECOM implementation proxy.
*
*/




// INCLUDE FILES
#include <e32base.h>
#include <ecom/implementationproxy.h>
#include "RNcnNotification.h"
#include "NcnNotificationUIDs.hrh"

// ==================== LOCAL FUNCTIONS ====================

/**
*
* ImplementationTable
*
*/
const TImplementationProxy ImplementationTable[] =
	{
	    IMPLEMENTATION_PROXY_ENTRY( KNcnNotifImplUID, RNcnNotification::NewL )
	};

/**
* ImplementationGroupProxy
* @param aTableCount
* @returns "TImplementationProxy*"
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount )
	{
	aTableCount =
	    sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	return ImplementationTable;
	}

// End of file
