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
*     List container key timer.
*
*/



// INCLUDE FILES

#include <aknkeys.h>  // EKeyOk
#include "MceMessageListContTimer.h"

#include <PtiEngine.h>


// CONSTANTS

const TInt KMceMultiTapTimeout    = 1000000; // 1.0 s


// ================= MEMBER FUNCTIONS =======================



// ----------------------------------------------------
// CMceMessageListContainerKeyTimer::NewL
// Constructor
// ----------------------------------------------------
CMceMessageListContainerKeyTimer* CMceMessageListContainerKeyTimer::NewL( CPtiEngine& aT9Interface )
    {
    CMceMessageListContainerKeyTimer* self =
        new (ELeave) CMceMessageListContainerKeyTimer(
        aT9Interface );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------
// CMceMessageListContainerKeyTimer::~CMceMessageListContainerKeyTimer
// Destructor
// ----------------------------------------------------
CMceMessageListContainerKeyTimer::~CMceMessageListContainerKeyTimer()
    {
    Cancel();
    iTimer.Close();
    }

// ----------------------------------------------------
// CMceMessageListContainerKeyTimer::CMceMessageListContainerKeyTimer
// Constructor
// ----------------------------------------------------
CMceMessageListContainerKeyTimer::CMceMessageListContainerKeyTimer(
    CPtiEngine& aT9Interface )
    :
    CActive( EPriorityStandard ),
    iT9Interface( aT9Interface )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------
// CMceMessageListContainerKeyTimer::ConstructL
// Creates key click timer
// ----------------------------------------------------
void CMceMessageListContainerKeyTimer::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    }

// ----------------------------------------------------
// CMceMessageListContainerKeyTimer::Start
// Starts key click timer
// ----------------------------------------------------
void CMceMessageListContainerKeyTimer::Start()
    {
    const TTimeIntervalMicroSeconds32 timeInterval = KMceMultiTapTimeout;
    Cancel();
    iTimer.After( iStatus, timeInterval );
    SetActive();
    }

// ----------------------------------------------------
// CMceMessageListContainerKeyTimer::RunL
// Runs key click timer
// ----------------------------------------------------
void CMceMessageListContainerKeyTimer::RunL()
    {
    iT9Interface.ClearCurrentWord();
    }

// ----------------------------------------------------
// CMceMessageListContainerKeyTimer::DoCancel
// Cancels key click timer
// ----------------------------------------------------
void CMceMessageListContainerKeyTimer::DoCancel()
    {
    iTimer.Cancel();
    }

//  End of File
