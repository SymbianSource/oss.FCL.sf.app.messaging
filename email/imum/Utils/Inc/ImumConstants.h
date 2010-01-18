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
* Description:  ImumConstants.h
*
*/


#ifndef T_IMUMCONSTANS_H
#define T_IMUMCONSTANS_H

#include <imapset.h>

enum TImumMboxDefaultDataIndex
    {
    EImumMboxDefaultDataIndexImap4 = 0,
    EImumMboxDefaultDataIndexPop3,
    EImumMboxDefaultDataIndexSmtp,
    EImumMboxDefaultDataIndexCount
    };

/** Hard defaults */
_LIT( KImumMboxDefaultMailboxName, "" );
_LIT( KImumMboxDefaultEmailAddress, "" );
const TInt KImumMboxDefaultAccessPoint = TImumDaSettings::EValueApAlwaysAsk;
const TInt KImumMboxDefaultAccessPointType = TImumInSettings::EValueApIsIapId;
_LIT8( KImumMboxDefaultUsername, "" );
_LIT8( KImumMboxDefaultPassword, "" );
const TInt KImumMboxDefaultPort = TImumDaSettings::EValuePortDefault;
const TInt KImumMboxDefaultSecurity = TImumDaSettings::EValueSecurityOff;
_LIT( KImumMboxDefaultUserAlias, "" );
_LIT( KImumMboxDefaultReplyToAddress, "" );
const TInt KImumMboxDefaultSendDelay = TImumDaSettings::EValueSendImmediately;
const TInt KImumMboxDefaultSendCopyToSelf = TImumDaSettings::EValueSendCopyOff;
const TInt KImumMboxDefaultIncludeSignature = EFalse;
_LIT( KImumMboxDefaultSignature, "" );
const TInt KImumMboxDefaultEmailAlert = TImumDaSettings::EFlagAlertSoftNote;
const TInt32 KImumMboxDefaultInboxUpdateLimit = 50;
const TInt32 KImumMboxDefaultFolderUpdateLimit = 30;
const TInt KImumMboxDefaultMailDeletion = TImumDaSettings::EValueDeletionAlwaysAsk;
const TInt KImumMboxDefaultAutoNotifications = TImumDaSettings::EValueNotificationsHome;
const TInt KImumMboxDefaultAutoRetrieval = TImumDaSettings::EValueAutoOff;
const TInt KImumMboxDefaultAutoRetrievalDays = TImumDaSettings::EFlagSetAllDays;
const TInt64 KImumMboxDefaultAutoRetrievalStartHour = 0;
const TInt64 KImumMboxDefaultAutoRetrievalEndHour = 0;
const TInt KImumMboxDefaultAutoRetrievalInterval = TImumDaSettings::EValue1Hour;
const TInt KImumMboxDefaultAPop = EFalse;
_LIT8( KImumMboxDefaultpFolderPath, "" );

_LIT( KImumMboxDefaultAccessPointName, "" );

// Download controls for protocols
const TInt32 KImumMboxDefaultSyncFlags[EImumMboxDefaultDataIndexCount] =
    { TImumInSettings::EFlagDownloadHeader,     // Imap4
      TImumInSettings::EFlagDownloadHeader };   // Pop3
const TInt32 KImumMboxDefaultDownloadFlags[EImumMboxDefaultDataIndexCount] =
    { TImumInSettings::EFlagDownloadBody,       // Imap4
      TImumInSettings::EFlagSetDownloadAll };   // Pop3
const TInt32 KImumMboxDefaultDownloadBodySize = KMaxTInt32;
const TInt32 KImumMboxDefaultDownloadAttachmentSize = KMaxTInt32;
const TInt KImumMboxDefaultDownloadMaxEmailSize = KMaxTInt32;

const TInt KImumMboxDefaultHideMsgs = 0;
const TInt KImumMboxDefaultOpenHtmlMail = 0;
_LIT8( KImumMboxDefaultSearchString, "" );
_LIT8( KImumMboxDefaulthPathSeparator, "" );
const TInt KImumMboxDefaultSynchronise = EUseLocal;
const TInt KImumMboxDefaultSubscribe = EUpdateNeither;
const TInt KImumMboxDefaultUpdatingSeenFlag = ETrue;
const TInt KImumMboxDefaultSyncRate = 300;
const TInt KImumMboxDefaultFetchSize = 0;
const TInt KImumMboxDefaultImapIdle = ETrue;
const TInt KImumMboxDefaultImapIdleTimeout = 1740;
_LIT( KImumMboxDefaultReceiptAddress, "" );
const TInt KImumMboxDefaultBodyEncoding = EMsgOutboxMIME;
const TInt32 KImumMboxDefaultDefaultMsgCharSet = 0;
const TInt KImumMboxDefaultAddVCardToEmail = EFalse;
const TInt KImumMboxDefaultRequestReceipts = EFalse;
const TInt KImumMboxDefaultSMTPAuth = ETrue;
const TInt KImumMboxDefaultInboxLoginDetails = 0; // ?
const TInt KImumMboxDefaultToCcIncludeLimit = 0; // ?
const TInt KImumMboxDefaultDisconnectedMode = ETrue;
const TInt KImumMboxDefaultAutoSendOnConnect = ETrue;
const TInt KImumMboxDefaultDeleteEmailsWhenDisconnecting = EFalse;
const TInt KImumMboxDefaultAcknowledgeReceipts = EFalse;
const TInt KImumMboxDefaultTemporaryUsername = 0;
const TInt KImumMboxDefaultTemporaryPassword = 0;
const TInt KImumMboxDefaultSettingsVersion = 3200;
const TInt KImumMboxDefaultAllowEmptyEmailAddress = ETrue; //Allow the predefined mailbox address to be empty
const TInt KImumMboxDefaultGenerateMailboxName = 0;
const TInt KImumMboxDefaultLastUpdateFailed = 0;
const TInt64 KImumMboxDefaultLastSuccessfulUpdate = 0;
const TInt KImumMboxUpdateSuccessfulWithCurSettings = 0;
const TInt KImumMboxDefaultDownloadBufferWlan = KImapDefaultFetchSizeBytes;
const TInt KImumMboxDefaultDownloadBufferGprs = KImapDefaultFetchSizeBytes * 6;
const TBool KImumMboxDefaultIsPredefinedMailbox = EFalse;

const TUint KImumUidValue = 0x100058EB;
const TUid KImumUid = { KImumUidValue };

// OTHER CONSTANTS

class ImumTextConsts
    {
    public:
        enum TTextLengths
            {
            EMaxTextLen = 2500
            };
    };



#endif // T_IMUMCONSTANS_H
