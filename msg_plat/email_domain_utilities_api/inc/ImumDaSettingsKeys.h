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
* Description:  Collection of setting keys
 *
*/


#ifndef T_IMUMDASETTINGSKEYS_H
#define T_IMUMDASETTINGSKEYS_H

#include <msvstd.h>

/** Mailbox name max length */
const TInt KImumDaMailboxNameLength = 100;

/** Email address max length*/
const TInt KImumDaEmailSettingLength = 100;

/** Server address max length */
const TInt KImumDaServerSettingLength = 100;

/** Username max length */
const TInt KImumDaUserNameLength = 50;

/** Password max length */
const TInt KImumDaPasswordLength = 50;

/** User alias max length */
const TInt KImumDaUserAliasLength = 100;

/** Signature max length */
const TInt KImumDaSignatureLength = 1600;

/** Folder path max length*/
const TInt KImumDaFolderPathLength = 100;

/**
 * Keys for mailbox settings.
 *
 * @lib imumda.lib
 * @since S60 v3.2
 */
class TImumDaSettings
    {
    public:

        /** Mailbox name type (system's supported text format) */
        typedef TBuf<KImumDaMailboxNameLength> TTextMailboxName;

        /** Email address type (system's supported text format) */
        typedef TBuf<KImumDaEmailSettingLength> TTextEmailAddress;

        /** Server address type (system's supported text format) */
        typedef TBuf<KImumDaServerSettingLength> TTextServerAddress;

        /** User name (8-bit text format) */
        typedef TBuf8<KImumDaUserNameLength> TTextUserName;

        /** Password (8-bit text format) */
        typedef TBuf8<KImumDaPasswordLength> TTextPassword;

        /** User alias (system's supported text format) */
        typedef TBuf<KImumDaUserAliasLength> TTextUserAlias;

        /** Signature (system's supported text format) */
        typedef TBuf<KImumDaSignatureLength> TTextSignature;

        /** Folder path type (8-bit text format) */
        typedef TBuf8<KImumDaFolderPathLength> TTextFolderPath;


        /** KEY SET: Connection Settings
            First word  : Type of the setting
            Second word : Connection, key stored in connection settings
                          General, key stored in general settings  */
        enum TConnectionKeySet
            {
            /** TMsvId, Connection. */
            EKeyMailboxId = 0x01000000,

            /** TInt, Connection. */
            EKeyProtocol,

            /** TDesC, General */
            EKeyEmailAddress,

            /** TInt, Connection */
            EKeyAccessPoint,

            /** TDesC, Connection */
            EKeyServer,

            /** TDesC8, Connection */
            EKeyUsername,

            /** TDesC8, Connection */
            EKeyPassword,

            /** TInt, Connection */
            EKeyPort,

            /** TInt, Connection */
            EKeySecurity,

            /** <NOT USED> */
            EKeyLastSetting        // Keep this last!
            };

        /** KEY SET: User Preferences */
        enum TPreferencesKeySet
            {
            /** TDesC, General */
            EKeyMailboxName = 0x02000000,

            /** TDesC, General */
            EKeyUserAlias,

            /** TDesC8, General */
            EKeyReplyToAddress,

            /** TInt, General */
            EKeySendDelay,

            /** TInt, General */
            EKeySendCopyToSelf,

            /** TInt, General */
            EKeyIncludeSignature,

            /** TDesC, General */
            EKeySignature,

            /** TInt, General */
            EKeyEmailAlert,

            /** TInt32, General */
            EKeyInboxUpdateLimit,

            /** TInt32, General */
            EKeyFolderUpdateLimit,

            /** TInt, General */
            EKeyMailDeletion,

            /** <NOT USED> */
            ELastPreference     // Keep this last!
            };

        /** KEY SET: Always Online */
        enum TAutomaticRetrievalKeySet
            {
            /** TInt, General */
            EKeyAutoNotifications = 0x03000000,

            /** TInt, General */
            EKeyAutoRetrieval,

            /** TInt, General */
            EKeyAutoRetrievalDays,

            /** TInt64, General */
            EKeyAutoRetrievalStartHour,

            /** TInt64, General */
            EKeyAutoRetrievalEndHour,

            /** TInt, General */
            EKeyAutoRetrievalInterval,

            /** <NOT USED> */
            ELastAuto           // Keep this last!
            };

        /** KEY SET: Miscellaneous */
        enum TMiscKeySet
            {
            /** TInt, Connection */
            EKeyAPop = 0x04000000,

            /** TDesC8, Connection */
            EImapFolderPath,

            /** <NOT USED> */
            ELastMisc       // Keep this last!
            };

        // ======== CONNECTION SETTINGS ========



        /** Protocols */
        enum TProtocolValues
            {
            EValueProtocolUnset = 0,
            EValueProtocolImap4,
            EValueProtocolPop3,
            EValueProtocolSyncMl,   // Deprecated since TB9.1
            EValueProtocolSmtp,
            ELastProtocolValue
            };

        /** Access Point settings */
        enum TAccessPointValues
            {
            EValueApAlwaysAsk = -1,
            EValueApUserDefined
            };

        /** Security settings */
        enum TSecurityValues
            {
            EValueSecurityOff = 0,
            EValueSecurityTls,
            EValueSecuritySsl
            };

        /** Port values */
        enum TPortValues
            {
            EValuePortDefault = -1,
            EValuePortUserDefined
            };

        // ======== SETTINGS PREFERENCES ========



        /** Send copy to self */
        enum TSendCopyValues
            {
            EValueSendCopyOff = 0,
            EValueSendCopyAsTo,
            EValueSendCopyAsCc,
            EValueSendCopyAsBcc
            };

        enum TSendDelayValues
            {
            EValueSendImmediately = 0,
            EValueSendOnNextConnection,
            EValueSendOnRequest,
            };

        /** New mail indications */
        enum TEmailAlertFlags
            {
            EFlagAlertSoftNote = 0x00000001,
            };

        /** Inbox/Folder update limit */
        enum TUpdateLimitValues
            {
            EValueUpdateAll = 0
            };

        /** Email deletion */
        enum TMailDeletionValues
            {
            EValueDeletionAlwaysAsk = 0,
            EValueDeletionLocalOnly,
            EValueDeletionLocalAndServer
            };

        // ======== AUTOMATIC RETRIEVAL SETTINGS ========



        /** OMA EMN notifications */
        enum TNotificationsValues
            {
            EValueNotificationsOff = 0,
            EValueNotificationsOn,
            EValueNotificationsHome
            };

        /** Always Online settings */
        enum TAutoValues
            {
            // No automatic retrieval
            EValueAutoOff = 0,
            // Always Online turned on
            EValueAutoOn,
            // Always Online on only in home network
            EValueAutoHomeNetwork
            };

        /** Retrieval days */
        enum TAutoDaysKeyFlags
            {
            // Flags
            EFlagMonday     = 0x00000001,
            EFlagTuesday    = 0x00000002,
            EFlagWednesday  = 0x00000004,
            EFlagThursday   = 0x00000008,
            EFlagFriday     = 0x00000010,
            EFlagSaturday   = 0x00000020,
            EFlagSunday     = 0x00000040,
            // Default sets
            EFlagSetAllDays     = 0x0000007F
            };

        /** Retrieval interval */
        enum TAutoIntervalValues
            {
            EValue5Minutes = 0,
            EValue15Minutes,
            EValue30Minutes,
            EValue1Hour,
            EValue2Hours,
            EValue4Hours,
            EValue6Hours
            };

        /** Default values for ports */
        enum TDefaultPorts
            {
            EDefaultPortImap4            = 143,
            EDefaultSecurityPortImap4    = 993,
            EDefaultPortPop3             = 110,
            EDefaultSecurityPortPop3     = 995,
            EDefaultPortSmtp             = 25,
            EDefaultSecurityPortSmtp     = 465
            };

        // ======== PROTOCOL-SPECIFIC SETTINGS ========


    };

#endif // T_IMUMDASETTINGSKEYS_H



