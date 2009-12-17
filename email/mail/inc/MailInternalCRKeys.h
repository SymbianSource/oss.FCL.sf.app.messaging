/*
* Copyright (c) 2001 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Mail internal Central Repository keys.
*
*/



#ifndef MAILINTERNALCRKEYS_H
#define MAILINTERNALCRKEYS_H

// CONSTANTS
const TUid KCRUidMail = {0x101F8850};

/**
 * Setting for Mail additional headers, stored in one 32-bit value.
 * Integer value.
 *
 * Default value: 5
 */
const TUint32 KMailAdditionalHeaderSettings = 0x00000000;

/**
 * Setting for Mail LV flags, stored in one 32-bit value.
 * Integer bitmask.
 *
 * Default value: 0
 */
const TUint32 KMailLocalVariationFlags = 0x00000001;

#endif      // MAILINTERNALCRKEYS_H

// End of File
