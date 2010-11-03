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
* Description:   New Contacts Note domain Publish & Subscribe keys.
*
*/




#ifndef NCNLISTDOMAINPSKEYS_H
#define NCNLISTDOMAINPSKEYS_H

// CONSTANTS
const TUid KPSUidNcnList = {0x101F87A9};

/**
 * For notifying NcnList to play alert tone when needed.
 * Possible integer values:
 * 0 = No audio, 1 = Email alert tone, 2 = SMS alert tone.
 *
 * Default value: 0
 *
 * (Shared data key: KNcnPlayAlertTone)
 */
const TUint32 KNcnPlayAlertTone = 0x00000001;

#endif      // NCNLISTDOMAINPSKEYS_H

// End of File

