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
* Description:  
*     Panic codes for calendar bio control.
*
*/



#ifndef _VCALBCPANICS_H_
#define _VCALBCPANICS_H_

#include <e32def.h>

// CONSTANTS

enum TVCalBcPanic
    {
    EUnacceptableCalEntryType,
    ERepeatNull,
    EIllegalRepeatType,
    ECalenImpProblem,
    EPanic1,
    EPanic2,
    EUnsupportedEntryType,
    EPanicUndefinedRepeat,          //Not used
    ENullEntryGivenToFiller,
    ENullViewer,
    EInvalidConfid,
    EIllegalDailyRepeatInterval,
    EIllegalWeeklyRepeatInterval,
    EIllegalMonthlyByDatesRepeatInterval,
    EIllegalYearlyByDateRepeatInterval,
    EVCalBCPanicUndefinedRepeatIndex
    };

GLREF_C void Panic(TVCalBcPanic aPanic);

#endif //_VCALBCPANICS_H_

// end of file
