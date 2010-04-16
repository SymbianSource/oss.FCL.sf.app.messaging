/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *
 */
#include <telconfigcrkeys.h>        // KCRUidTelephonyConfiguration
#include <centralrepository.h>

#include "matchphnumberutil.h"

// Constants
const int KDefaultGsmNumberMatchLength = 7;  //matching unique ph numbers

int MatchPhNumberUtil::matchDigits()
    {
    // Read the amount of digits to be used in contact matching
    // The key is owned by PhoneApp    
    CRepository* repository = CRepository::NewLC(KCRUidTelConfiguration);
    int matchDigitCount = 0;
    if ( repository->Get(KTelMatchDigits, matchDigitCount) == KErrNone )
        {
    // Min is 7
    matchDigitCount = Max(matchDigitCount, KDefaultGsmNumberMatchLength);
        }
    CleanupStack::PopAndDestroy(); // repository

    return matchDigitCount;

    }
   


// EOF

