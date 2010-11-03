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
* Description:   Data types for Ncn Notification API methods
*
*/



#ifndef __NCNNOTIFICATIONDEFS_H__
#define __NCNNOTIFICATIONDEFS_H__

// Types of NcnList notification messages
enum TNcnNotifMessageType
	{
	EInboxMessage = 0,
	EMailMessage,
	EIMMessage
	};

// Types of "mark as unread" request makers
enum TNcnUnreadRequestType
	{
	EIndexInbox = 0,
	EIndexMCE,
	EIndexMailBox
	};

#endif // __NCNNOTIFICATIONDEFS_H__

// End of File
