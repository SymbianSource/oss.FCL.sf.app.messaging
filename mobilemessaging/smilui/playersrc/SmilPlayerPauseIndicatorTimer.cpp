/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: SmilPlayerPauseIndicatorTimer implementation
*
*/



// INCLUDE FILES
#include "SmilPlayerPauseIndicatorTimer.h"
#include "SmilPlayerPauseObserver.h"

// CONSTANTS
const TInt KPauseIndicatorBlinkingTime = 1000000; // 1 seconds

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSmilPlayerPauseIndicatorTimer::CSmilPlayerPauseIndicatorTimer
// C++ constructor.
// ----------------------------------------------------------------------------
//
CSmilPlayerPauseIndicatorTimer::CSmilPlayerPauseIndicatorTimer( MSmilPlayerPauseObserver* aPauseObserver ) :
    CTimer( EPriorityStandard ),
    iPauseObserver( aPauseObserver )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPauseIndicatorTimer::ConstructL
// Symbian 2nd phase constructor. Creates timer.
// ----------------------------------------------------------------------------
// 
void CSmilPlayerPauseIndicatorTimer::ConstructL()
    {
    CTimer::ConstructL();
    }

// ---------------------------------------------------------
// CSmilPlayerPauseIndicatorTimer::NewL 
// Symbian two phased constructor
// ---------------------------------------------------------
// 
CSmilPlayerPauseIndicatorTimer* CSmilPlayerPauseIndicatorTimer::NewL( MSmilPlayerPauseObserver* aPauseObserver )
    {
    CSmilPlayerPauseIndicatorTimer* self = new(ELeave) CSmilPlayerPauseIndicatorTimer( aPauseObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPauseIndicatorTimer::~CSmilPlayerPauseIndicatorTimer
// Destructor.
// ----------------------------------------------------------------------------
//
CSmilPlayerPauseIndicatorTimer::~CSmilPlayerPauseIndicatorTimer()
    {
    iPauseObserver = NULL; // For LINT
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPauseIndicatorTimer::StartTimer
// Starts pause timer.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPauseIndicatorTimer::StartTimer()
    {
    Cancel();
    After( KPauseIndicatorBlinkingTime );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPauseIndicatorTimer::RunL
// Called when timer is triggered. Calls observer to toggle pause indicator and 
// sets timer active again.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPauseIndicatorTimer::RunL()
    {
    iPauseObserver->TogglePauseIndicator();
    After( KPauseIndicatorBlinkingTime );
    }

// End of File
