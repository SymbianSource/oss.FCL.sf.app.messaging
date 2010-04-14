/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Publish & Subscribe key identifiers for the S60 messaging UI
*                components
*
*/



#ifndef MESSAGINGINTERNALPSKEYS_H
#define MESSAGINGINTERNALPSKEYS_H

const TUid KPSUidMuiu = {0x101F87F1};

//
// The number of currently open message editors. The field is set as zero when 
// the user changes the message store between the phone and MMC. The message 
// editors should observe this and exit immediately.
//
const TUint32 KMuiuSysOpenMsgEditors = 0x00000001;

// Message viewer fast open previous/next message. Message id viewer had
// open when it exited. MCE uses this to focus/open correct message after
// viewer exit.
const TUint32 KMuiuKeyCurrentMsg = 0x00000002;

// The New PSK introduced from 5.0
// Tells whether Templates dialog is opened in the MCE main view
const TUint32 KMceTemplatesDialog = 0x00000003;

// The New PSK introduced from 5.0
// Tells whether Msg Sent as settings to be removed from Text settings view
const TUint32 KMuiuRemoveMsgSentSettings = 0x00000004;

// The New PSK introduced from 9.2
// Tells whether If one row list is populated or not, 1- list is populated, 0 - list population is in progress
const TUint32 KMuiuOneRowListPopulated = 0x00000005;

// The New PSK introduced from 5.0

const TUid KPSUidMsgEditor	=	{0x101F87FF};

//
// The number of currently standalone open message editors. The field is set to One by SendUI when we launch 
// a new standalone editor. When we try to launch another Standalone message editor when already one is existing, 
// The message editors should observe this and clsoe the previously launched standalone message editor. .
//

const TUint32 KMuiuStandAloneOpenMsgEditors = 0x00000001;
#endif // MESSAGINGINTERNALPSKEYS_H
