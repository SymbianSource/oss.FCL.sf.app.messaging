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
* Description:  ImumInSettingsKeys.h
 *
*/


#ifndef TIMUMINSETTINGSKEYS_H
#define TIMUMINSETTINGSKEYS_H

// Domain API contains the basic settings keys, so include header
// automatically here.
#include "ImumDaSettingsKeys.h"

/** */
const TInt KImumInSearchStringLength = 100;
/** */
const TInt KImumInSeparatorLength = 5;

class TImumInSettings
    {
    public:

        /** */
        typedef TBuf8<KImumInSearchStringLength> TTextSearchString;


        // =========== KEY SETS ===========

        /** KEY SET: Connection settings */
        enum TInConnectionKeySet
            {
            /** TDesC */
            EKeyAccessPointName = 0x01010000,

            /** TInt */
            ELastInSettingsKey
            };

        /** KEY SET: Preferences */
        enum TPreferencesKeySet
            {
            /** Flags to control synchronization.

                @type TInt
                @see MImumInConnectionSettings
                @see TDownloadSize */
            EKeySyncFlags = 0x02010000,

            /** Flags to control downloading.

                @type TInt
                @see MImumInConnectionSettings
                @see TDownloadSize */
            EKeyDownloadFlags,

            /** Body size limit.

                @type TInt32
                @see MImumInConnectionSettings
                @see TDownloadSize */
            EKeyDownloadBodySize,

            /** Attachment size limit.

                @type TInt32
                @see MImumInConnectionSettings
                @see TDownloadSize */
            EKeyDownloadAttachmentSize,

            /** Maximum size of email that will be downloaded.

                @type TInt
                @see MImumInConnectionSettings
                @see  */
            EKeyDownloadMaxEmailSize,

            /** TInt <NOT USED> */
            EKeyHideMsgs,

            /** TInt <NOT USED> */
            EKeyOpenHtmlMail,


            /** <NOT USED> */
            ELastInPreferenceKey
            };

        /** KEY SET: Miscellaneous */
        enum TMiscKeySet
            {

            /** IMAP4 related settings */

          	/** TDesC8 */
            EKeySearchString = 0x04010000,

          	/** TDesC8 */
            EKeyPathSeparator,

          	/** TInt */
            EKeySynchronise,

          	/** TInt */
            EKeySubscribe,

          	/** TInt */
            EKeyUpdatingSeenFlag,

          	/** TInt */
            EKeySyncRate,

          	/** TInt */
            EKeyFetchSize,

          	/** TInt */
            EKeyImapIdle,

          	/** TInt */
            EKeyImapIdleTimeout,

            /** TInt */
            EKeyDownloadBufferWlan,

            /** TInt */
            EKeyDownloadBufferGprs,

            /** SMTP related settings */

            /** TDesC */
            EKeyReceiptAddress,

            /** TInt */
            EKeyBodyEncoding,

            /** TInt32 */
            EKeyDefaultMsgCharSet,

            /** TInt */
            EKeyAddVCardToEmail,

            /** TInt */
            EKeyRequestReceipts,

            /** TInt */
            EKeySMTPAuth,

            /** TInt */
            EKeyInboxLoginDetails,

            /** TInt */
            EKeyToCcIncludeLimit,

            /** <NOT USED> */
            ELastInMiscKey       // Keep this last!
            };

        /** KEY SET: Email behaviour controlling keys */
        enum TEmailControlingKeySet
            {
            /** TInt */
            EKeyDisconnectedMode = 0x05010000,

			/** TInt */
            EKeyAutoSendOnConnect,

			/** TInt */
            EKeyDeleteEmailsWhenDisconnecting,

			/** TInt */
            EKeyAcknowledgeReceipts,


            /** <NOT USED> */
            ELastEmailKey
            };

        /** KEY SET: Settings behaviour controlling keys */
        enum TControllingKeySet
            {
            /** TInt */
            EKeyTemporaryUsername = 0x0F000000,
            EKeyTemporaryPassword,

            /** TInt */
            EKeySettingsVersion,

            /** TInt */
            EKeyAllowEmptyEmailAddress,

            /** TInt */
            EKeyGenerateMailboxName,

            /** TInt */
            EKeyAccessPointType,

            /** TInt */
            EKeyInfoLastUpdateFailed,

            /** TInt64 */
            EKeyInfoLastSuccessfulUpdate,

            /** TInt */
            EKeyAoUpdateSuccessfulWithCurSettings,

            /** TBool */
            EKeyIsPredefinedMailbox,

            /** <NOT USED> */
            ELastControlKey
            };

        // =========== KEY VALUES ===========

        /** */
        enum TDownloadFlags
            {
            /** FLAGS */
            EFlagDownloadHeader             = 0x00000001,
            EFlagDownloadBody               = 0x00000002,
            EFlagDownloadBodyPartial        = 0x00000004,
            EFlagDownloadAttachment         = 0x00000008,
            EFlagDownloadAttachmentPartial  = 0x00000010,
            EFlagDownloadPartialCumulative  = 0x00000020,

            /** SETS */
            EFlagSetDownloadHeaderBody      = 0x00000003,
            EFlagSetDownloadAll             = 0x0000000B
            };

        /** */
        enum TDownloadSize
            {
            EValueDownloadCompletely     = -1,
            EValueDownloadUserDefined
            };

        /** */
        enum TBodyEncodingValues
            {
            EValueEncodingDefault = 0,
            EValueEncodingOff,
            EValueEncodingMIME,
            EValueEncodingMHtmlAsMime,
            EValueEncodingMHtmlAltAsMime
            };

        /** Access point types */
        enum TAccessPointControlValues
            {
            /** Index to the access point in the list */
            EValueApIsIapId = 0
            };
    };

#endif // TIMUMINSETTINGSKEYS_H



