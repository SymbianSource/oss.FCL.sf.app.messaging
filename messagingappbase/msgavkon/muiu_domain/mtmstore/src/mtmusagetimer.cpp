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
* Description: mtmusagetimer implementation
*
*/



// INCLUDE FILES
#include "mtmusagetimer.h"
#include "MTMStore.h" // for MMtmUsageTimerObserver


// CONSTANTS
const TInt KUsageTimerTimeoutValue = 5*60*1000*1000; // 5 minutes


// ============================ MEMBER FUNCTIONS ===============================


// ---------------------------------------------------------
// CMtmUsageTimer::NewLC
// ---------------------------------------------------------
//
CMtmUsageTimer* CMtmUsageTimer::NewLC( MMtmUsageTimerObserver& aObserver, TUid aMtm )
    { // static
    CMtmUsageTimer* self = new( ELeave ) CMtmUsageTimer( aObserver, aMtm );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CMtmUsageTimer::CMtmUsageTimer
// ---------------------------------------------------------
//
CMtmUsageTimer::CMtmUsageTimer( MMtmUsageTimerObserver& aObserver, TUid aMtm )
:CTimer( EPriorityLow ), 
iObserver( aObserver ), 
iMtm( aMtm )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMtmUsageTimer::RunL
// ---------------------------------------------------------
//
void CMtmUsageTimer::RunL()
    {
    iObserver.MtmUsageTimedOut( iMtm );
    }


// ---------------------------------------------------------
// CMtmUsageTimer::Start
// ---------------------------------------------------------
//
void CMtmUsageTimer::Start()
    {
    Cancel();
    After(KUsageTimerTimeoutValue);
    }


// End of file
