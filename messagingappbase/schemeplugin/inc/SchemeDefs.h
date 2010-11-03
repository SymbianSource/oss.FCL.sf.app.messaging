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
*      Constants used both in code of Scheme Handler Plugin.   
*
*/



#ifndef SCHEME_DEFS_H
#define SCHEME_DEFS_H

#include <msvids.h>

// CONSTANTS
// Identifiers for mms and sms
_LIT( KMmsto,"mmsto:");
_LIT( KSms,"sms:");
_LIT( KBody, "body=" );
_LIT( KSubject, "subject=" );
_LIT( KTo, "to=" );
_LIT( KCc, "cc=" );
_LIT( KComma, "," );

// For SendUi construction
LOCAL_C const TInt KCommand = 0;

// Identifiers for localapp
_LIT( KLocalApp, "localapp:" );
_LIT( KLocalAppSlash, "localapp://" );
_LIT( KMmsId, "mms" );
_LIT( KSmsId, "sms" );
_LIT( KContactsId, "contacts" );
_LIT( KCalendarId, "calendar" );
_LIT( KMessagingId,"messaging" );
_LIT( KToday, "?view=today" );
_LIT( KCompose, "/compose" );
_LIT( KComposer, "view=composer" );
_LIT( KInbox, "view=inbox" );
_LIT( KShow, "/show" );
_LIT( KJavaApp, "javaapp:" );
_LIT( KJamId, "jam/launch" );


// IDs for Calendar launching
const TUid KUidCalendar = { 0x10005901 };
_LIT( KDesToday, "Today" );
// IDs for Messaging launching
const TUid KUidMessaging = { 0x100058C5 };
const TUid KMceInboxViewId    ={0x02};    // Inbox view Uid
const TUid KMessageId = {KMsvGlobalInBoxIndexEntryIdValue};
// IDs for Contacts launching
const TUid KUidContacts = { 0x101f4cce };

#endif // def SCHEME_DEFS_H
