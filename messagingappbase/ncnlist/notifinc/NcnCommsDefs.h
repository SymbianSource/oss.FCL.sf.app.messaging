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
* Description:   Data types for NcnNotification API C/S comms
*
*/



#ifndef __NCNCOMMSDEFS_H__
#define __NCNCOMMSDEFS_H__

#include <e32base.h>

// Name of NcnList server
_LIT( KNcnServerName, "NcnServer" );

const TInt KNcnNotificationServerMajor = 1;
const TInt KNcnNotificationServerMinor = 1;
const TInt KNcnNotificationServerBuild = 1;

// List of internal API methods transmitted by IPC
enum TNcnInternalApiFunc
	{
	ENewInternalMessages1     = 0,
	ENewInternalMessages2     = 1,
	EInternalMarkUnread1      = 2,
	EInternalMarkUnread2      = 3,
	EInternalCancelMarkUnread = 4
	};

// List of API methods transmitted by IPC, continue numbering from
// internal API functions
enum TNcnApiFunc
	{
	ENewMessages1           = 5,
	EMarkUnread1            = 6	
	};

#endif // __NCNCOMMSDEFS_H__

// End of File
