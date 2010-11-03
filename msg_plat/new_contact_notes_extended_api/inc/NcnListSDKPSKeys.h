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
* Description:   New Contacts Note Publish & Subscribe keys.
*
*/



#ifndef NCNLISTSDKPSKEYS_H
#define NCNLISTSDKPSKEYS_H

// Use TUid KUidSystemCategory = { 0x101f75b6 };

const TInt KNcnEnumerationFirstValue = 0;

/**
 * Outbox status PS key. Indicates the state of the outbox (ie. empty or contains documents).
 */
const TInt KPSNcnOutboxStatusValue = 0x100052CE;
const TUid KPSNcnOutboxStatus = { KPSNcnOutboxStatusValue };

/**
 * Outbox status value enumeration.
 */
enum TNcnOutboxStatus
    {
    ENcnOutboxEmpty,
    ENcnDocumentsInOutbox
    };

/**
 * Voice Mail status PS key. Indicates the state of voice mail.
 */
const TInt KPSNcnVoiceMailStatusValue = 0x100052E3;
const TUid KPSNcnVoiceMailStatus = { KPSNcnVoiceMailStatusValue };

/**
 * Voice Mail status value enumeration.
 */
enum TNcnVoiceMailStatus
    {
    ENcnVoiceMailStatusStatusUninitialized = KNcnEnumerationFirstValue,
    ENcnNoVoiceMails,
    ENcnVoiceMailWaiting,
    ENcnVoiceMailWaitingOnLine1,
    ENcnVoiceMailWaitingOnLine2,
    ENcnVoiceMailWaitingOnBothLines
    };

#endif      // NCNLISTSDKPSKEYS_H

// End of File

