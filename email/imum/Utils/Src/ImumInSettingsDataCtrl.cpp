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
* Description:  ImumInSettingsDataCtrl.cpp
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <SendUiConsts.h>

#include "ImumInSettingsDataCtrl.h"
#include "ImumInSettingsDataArray.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// ImumInSettingsDataCtrl::IsText()
// ---------------------------------------------------------------------------
//
TBool ImumInSettingsDataCtrl::IsText( const TInt aAttributeType )
    {
    return ( aAttributeType == ECmpNormal || aAttributeType == ECmpNormal8 );
    }

// ---------------------------------------------------------------------------
// ImumInSettingsDataCtrl::IsNumber()
// ---------------------------------------------------------------------------
//
TBool ImumInSettingsDataCtrl::IsNumber( const TInt aAttributeType )
    {
    return ( aAttributeType >= ECmpTInt8 && aAttributeType <= ECmpTInt64 );
    }

// ---------------------------------------------------------------------------
// ImumInSettingsDataCtrl::MatchKey()
// ---------------------------------------------------------------------------
//
TInt ImumInSettingsDataCtrl::MatchKey(
    const TUint aAttributeKey,
    const TInt aRequiredKey )
    {
    TInt error = TImumDaErrorCode::ENoError;
    TInt type = GetKeyAttributeType( aAttributeKey );

    // Requested key does not match with exisiting key, return error
    if ( type != aRequiredKey )
        {
        switch ( type )
            {
            case ECmpNormal:
                error = TImumDaErrorCode::ERequestedKeyIsTDesC;
                break;
            case ECmpNormal8:
                error = TImumDaErrorCode::ERequestedKeyIsTDesC8;
                break;
            case ECmpTInt32:
                error = TImumDaErrorCode::ERequestedKeyIsTInt;
                break;
            case ECmpTInt64:
                error = TImumDaErrorCode::ERequestedKeyIsTInt64;
                break;
            case ECmpTUint32:
                error = TImumDaErrorCode::ERequestedKeyIsTMsvId;
                break;
            default:
                error = TImumDaErrorCode::ERequestedKeyIsUnknown;
                break;
            }
        }

    // Panic, when key type is wrong
    __ASSERT_DEBUG( !error, User::Panic(
        KImumInSettingsDataArray, error ) );

    return error;
    }

// ---------------------------------------------------------------------------
// ImumInSettingsDataCtrl::GetKeyAttributeType()
// ---------------------------------------------------------------------------
//
TInt ImumInSettingsDataCtrl::GetKeyAttributeType(
    const TUint aAttributeKey )
    {
    TInt type;

    switch ( aAttributeKey )
        {
        // 16-bit text
        case TImumDaSettings::EKeyEmailAddress:
        case TImumDaSettings::EKeyServer:
        case TImumDaSettings::EKeyMailboxName:
        case TImumDaSettings::EKeyUserAlias:
        case TImumDaSettings::EKeyReplyToAddress:
        case TImumDaSettings::EKeySignature:
        case TImumInSettings::EKeyAccessPointName:
        case TImumInSettings::EKeyReceiptAddress:
            type = ECmpNormal;
            break;

        // 32-bit values
        case TImumDaSettings::EKeyProtocol:
        case TImumDaSettings::EKeyAccessPoint:
        case TImumDaSettings::EKeyPort:
        case TImumDaSettings::EKeySecurity:
        case TImumDaSettings::EKeySendDelay:
        case TImumDaSettings::EKeySendCopyToSelf:
        case TImumDaSettings::EKeyIncludeSignature:
        case TImumDaSettings::EKeyEmailAlert:
        case TImumDaSettings::EKeyMailDeletion:
        case TImumDaSettings::EKeyAPop:
        case TImumDaSettings::EKeyAutoNotifications:
        case TImumDaSettings::EKeyAutoRetrieval:
        case TImumDaSettings::EKeyAutoRetrievalDays:
        case TImumDaSettings::EKeyAutoRetrievalInterval:
        case TImumInSettings::EKeyDownloadMaxEmailSize:
        case TImumInSettings::EKeyHideMsgs:
        case TImumInSettings::EKeyOpenHtmlMail:
        case TImumInSettings::EKeySynchronise:
        case TImumInSettings::EKeySubscribe:
        case TImumInSettings::EKeyUpdatingSeenFlag:
        case TImumInSettings::EKeySyncRate:
        case TImumInSettings::EKeyFetchSize:
        case TImumInSettings::EKeyImapIdle:
        case TImumInSettings::EKeyImapIdleTimeout:
        case TImumInSettings::EKeyBodyEncoding:
        case TImumInSettings::EKeyAddVCardToEmail:
        case TImumInSettings::EKeyRequestReceipts:
        case TImumInSettings::EKeySMTPAuth:
        case TImumInSettings::EKeyInboxLoginDetails:
        case TImumInSettings::EKeyToCcIncludeLimit:
        case TImumInSettings::EKeyDisconnectedMode:
        case TImumInSettings::EKeyAutoSendOnConnect:
        case TImumInSettings::EKeyDeleteEmailsWhenDisconnecting:
        case TImumInSettings::EKeyAcknowledgeReceipts:
        case TImumInSettings::EKeyTemporaryUsername:
        case TImumInSettings::EKeyTemporaryPassword:
        case TImumInSettings::EKeySettingsVersion:
        case TImumInSettings::EKeyAllowEmptyEmailAddress:
        case TImumInSettings::EKeyGenerateMailboxName:
        case TImumInSettings::EKeyAccessPointType:
        case TImumInSettings::EKeyInfoLastUpdateFailed:
        case TImumInSettings::EKeyDownloadBufferWlan:
        case TImumInSettings::EKeyDownloadBufferGprs:
        case TImumInSettings::EKeyIsPredefinedMailbox:
        case TImumInSettings::EKeyAoUpdateSuccessfulWithCurSettings:
            type = ECmpTInt32;
            break;

        // 8-bit text
        case TImumDaSettings::EKeyUsername:
        case TImumDaSettings::EKeyPassword:
        case TImumInSettings::EKeyPathSeparator:
        case TImumDaSettings::EImapFolderPath:
        case TImumInSettings::EKeySearchString:
            type = ECmpNormal8;
            break;

        // 64-bit values
        case TImumDaSettings::EKeyAutoRetrievalStartHour:
        case TImumDaSettings::EKeyAutoRetrievalEndHour:
        case TImumInSettings::EKeyInfoLastSuccessfulUpdate:
            type = ECmpTInt64;
            break;

        // 32-bit unsigned values
        case TImumDaSettings::EKeyMailboxId:
        case TImumDaSettings::EKeyInboxUpdateLimit:
        case TImumDaSettings::EKeyFolderUpdateLimit:
        case TImumInSettings::EKeyDownloadBodySize:
        case TImumInSettings::EKeyDownloadAttachmentSize:
        case TImumInSettings::EKeyDefaultMsgCharSet:
        case TImumInSettings::EKeySyncFlags:
        case TImumInSettings::EKeyDownloadFlags:
            type = ECmpTUint32;
            break;

        // Not too good if executing this part of the code...
        default:
            type = KErrNotFound;
            break;
        }

    return type;
    }

// ---------------------------------------------------------------------------
// ImumInSettingsDataCtrl::GetNextKey()
// ---------------------------------------------------------------------------
//
TBool ImumInSettingsDataCtrl::GetNextKey(
    TUint& aKey,
    const TBool& aIncludeCompulsory )
    {
    TBool isLast = EFalse;

    do
        {
        switch ( aKey )
            {
            case 0:
                aKey = TImumDaSettings::EKeyMailboxId;
                break;

            case TImumDaSettings::EKeyLastSetting:
                aKey = TImumInSettings::EKeyAccessPointName;
                break;

            case TImumInSettings::ELastInSettingsKey:
                aKey = TImumDaSettings::EKeyMailboxName;
                break;

            case TImumDaSettings::ELastPreference:
                aKey = TImumInSettings::EKeySyncFlags;
                break;

            case TImumInSettings::ELastInPreferenceKey:
                aKey = TImumDaSettings::EKeyAutoNotifications;
                break;

            case TImumDaSettings::ELastAuto:
                aKey = TImumDaSettings::EKeyAPop;
                break;

            case TImumDaSettings::ELastMisc:
                aKey = TImumInSettings::EKeySearchString;
                break;

            case TImumInSettings::ELastInMiscKey:
                aKey = TImumInSettings::EKeyDisconnectedMode;
                break;

            case TImumInSettings::ELastEmailKey:
                aKey = TImumInSettings::EKeyTemporaryUsername;
                break;

            case TImumInSettings::ELastControlKey:
                isLast = ETrue;
                break;

            default:
                break;
            }
        }
    while ( !aIncludeCompulsory && IsCompulsory( aKey ) );

    return isLast;
    }

// ---------------------------------------------------------------------------
// ImumInSettingsDataCtrl::IsCompulsory()
// ---------------------------------------------------------------------------
//
TBool ImumInSettingsDataCtrl::IsCompulsory( TUint& aKey )
    {
    switch ( aKey )
        {
        case TImumDaSettings::EKeyProtocol:
        case TImumDaSettings::EKeyEmailAddress:
        case TImumDaSettings::EKeyMailboxName:
        case TImumDaSettings::EKeyServer:
            aKey++;
            return ETrue;

        default:
            break;
        }

    return EFalse;
    }






// End of File
