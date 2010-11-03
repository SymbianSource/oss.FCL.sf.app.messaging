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
* Description:  ImumDaErrorCodes.h
 *
*/


#ifndef T_IMUMDAERRORCODE_H
#define T_IMUMDAERRORCODE_H

class TImumDaErrorCode
    {
    public:
        enum TErrorCode
            {
            ENoError = 0,

            // Settings errors: 0 > -1000
            EReadFailure = -1000,
            EWriteFailure,
            EConnectionSetNotExist,
            EParamNotReceivingProtocol,
            EParamUnsupportedProtocol,
            EParamEmptyProtocol,
            EConnectionIndexUnderflow,
            EConnectionIndexOverflow,
            ERequestedKeyIsTInt,        // -992
            ERequestedKeyIsTInt64,      // -991
            ERequestedKeyIsTMsvId,      // -990
            ERequestedKeyIsTDesC,       // -989
            ERequestedKeyIsTDesC8,      // -988
            ERequestedKeyIsUnknown,     // -987
            ECopyFailureOfProtocol,
            ECopyFailureOfEmailAddress,
            ECopyFailureOfAccessPoint,
            ECopyFailureOfServer,
            ECopyFailureOfUsername,
            ECopyFailureOfPassword,
            ECopyFailureOfPort,         // -980
            ECopyFailureOfSecurity,
            EUnsupportedOperation,
            EItemCreationFailure,
            EItemSetFailure,
            EKeyAndDataArrayInbalanced,
            ESettingNotFound,
            EResetUnrecognizedKey,
            EResetKeyNotFound,
            EDefaultCopyKeyTypeUnknown,
            EDefaultCopyKeyNotFound,    // -970
            EKeyCannotBeAssigned,
            EAttributeNull,
            EConnectionSettingsNull,

            // Validation errors: -1001 > -2000
            ESettingEmpty = -2000,
            NoConnectionSettings,
            EValueProtocolInvalid,
            EValueProtocolNotFound,
            EValueProtocolSendingNotSet,
            EValueProtocolReceivingNotSet,
            EValueProtocolFirstNotReceiving,
            EValueProtocolTooManyReceiving,
            EEmailAddressNotSet,
            EEmailAddressInvalid,
            EEmailAddressEmpty,     // -1990
            EServerAddressNotSet,
            EServerAddressInvalid,
            EServerAddressEmpty,
            EAccessPointNotSet,
            EAccessPointIdNotFound,
            EMailboxNameNotSet,
            EMailboxNameAlreadyExist,
            EMailboxNameUnknownError,
            ESecuritySettingNotFound,
            ESecuritySettingInvalid,    // -1980
            ESecurityProtocolIllegal,
            ESecurityPortNotFound,
            ESecurityPortInvalid,

            // Service errors: -2001 > -3000
            EUnsupportedProtocol = -3000,
            EIdNotRecongnized,

            // Utils errors: -3001 > -4000
            EEntryNotFound = -4000,
            EEntryNotMailbox,

            // Api errors
            EServicesNotReady = -5000
            };
    };

#endif // T_IMUMDAERRORCODE_H
