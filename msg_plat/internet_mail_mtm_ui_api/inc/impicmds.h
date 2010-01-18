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
*     Commands which are used by the imap mtm, but only have meaning to the UI layer
*
*
*/


#if !defined (__IMPICMDS_H__)
#define __IMPICMDS_H__

#include <MtmExtendedCapabilities.hrh>

/**
*   TIMAP4Cmds:
*   (i.e.   KImpiMTMConnectAndSyncCompleteAfterConnection = Connect and complete when disconnected, sync not reported
*           KImpiMTMConnectAndSyncCompleteAfterFolderSync = Connect, sync folders then complete
*           KImpiMTMConnectAndSyncCompleteAfterConnection = Connect and synchronise, and complete when disconnected
*           KImpiMTMRefreshFolderList =  NOT supported
*           KImpiMTMSubscribedFolders = NOT supported
*           KImpiMTMPurgeAttachment = NOT supported, use CImumDeleteMessagesLocally instead
*           KImpiMTMFetchAttachments = Fetch attachments
*/

enum TIMAP4Cmds
    {
    KImpiMTMConnect = KMtmUiFirstFreePlatformMTMFunctionId,
    KImpiMTMConnectAndSyncCompleteAfterConnection,
    KImpiMTMConnectAndSyncCompleteAfterFolderSync,
    KImpiMTMConnectAndSyncCompleteAfterDisconnection,
    KImpiMTMRefreshFolderList,
    KImpiMTMSubscribedFolders,
    KImpiMTMPurgeAttachment,
    KImpiMTMFetchAttachments,
    KImpiMTMChangeFolderSubscribe,//imapfolders
    KImpiMTMUpdateChangeFolderHeaders//imapfolders
    };

#endif
