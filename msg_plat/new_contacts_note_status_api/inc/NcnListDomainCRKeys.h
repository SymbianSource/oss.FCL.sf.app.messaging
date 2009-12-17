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
* Description:   New Contacts Note domain Central Repository keys.
*
*/




#ifndef NCNLISTDOMAINCRKEYS_H
#define NCNLISTDOMAINCRKEYS_H

// CONSTANTS
const TUid KCRUidNcnList = {0x101F87A8};

/**
 * Setting for Operator Indicator flags, stored in one 32-bit value.
 * Integer value.
 *
 * Default value: 0
 *
 * (Shared data key: KLogsOperatorIndicators)
 */
const TUint32 KNcnOperatorIndicators = 0x00000000;

/**
 * The number of active voice mail messages in line 1
 *
 * Possible values are 0 to to KMaxTInt 
 */
/* 107-18696: Handling of EF-CFIS and EF-MWIS in telephony area
 * These can be removed when etel mm api has been taken into use
 * successfully. 
*/ 
const TUint32 KNcnVoiceMailCountInLine1 = 0x00000001;

/**
 * The number of active voice mail messages in line 2
 *
 * Possible values are 0 to to KMaxTInt 
 */
/* 107-18696: Handling of EF-CFIS and EF-MWIS in telephony area
 * These can be removed when etel mm api has been taken into use
 * successfully.  
*/ 
const TUint32 KNcnVoiceMailCountInLine2 = 0x00000002;

/**
 * Local variation bitmask for NCN. See NcnLocalVariant.hrh for
 * bitmask values
 *
 * Default value: 0
 *
 */
const TUint32 KNcnLocalVariationFlags = 0x00000003;

/**
 * Constant that defines the desired notification when
 * "New Mail Notification" setting is on for a S60
 * mailbox. Possible values are the sames that
 * are defined in MNcnNotification.h.
 * Icon indication = 1
 * Tone and icon indication = 3
 * Icon, Tone and SoftNote indication = 7
 *         
 * Default value: 7
 *
 */
const TUint32 KNcnMailNotificationIndicatorOn = 0x00000004;

/**
 * Constant that defines the desired notification when
 * "New Mail Notification" setting is off for a S60
 * mailbox. Possible values are the sames that
 * are defined in MNcnNotification.h.
 * Icon indication = 1
 * Tone and icon indication = 3
 * Icon, Tone and SoftNote indication = 7
 *         
 * Default value: 1
 *
 */
const TUint32 KNcnMailNotificationIndicatorOff = 0x00000005;

/**
 * The number of new messages in Inbox
 *
 * Possible values are 0 to to KMaxTInt 
 */
const TUint32 KNcnNewMessageCountInInbox = 0x00000006;

/**
 * The number of messages in Outbox
 *
 * Possible values are 0 to to KMaxTInt 
 */
const TUint32 KNcnMessageCountInOutbox = 0x00000007;

/**
 * The number of new audio messages in Inbox
 *
 * Possible values are 0 to to KMaxTInt 
 */
const TUint32 KNcnNewAudioMessageCountInInbox = 0x00000008;

#endif      // NCNLISTDOMAINCRKEYS_H

// End of File
