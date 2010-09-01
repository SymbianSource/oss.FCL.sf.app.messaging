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
* Description:   
*       This class defines functionality for small timer
*
*/



// INCLUDE FILES
#include "NcnBaseTimer.h"
#include "NcnTimer.h"
#include "NcnModel.h"


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
CNcnTimer::CNcnTimer( MNcnTimer& aObserver ) 
    :
    CTimer( EPriorityStandard ),
    iObserver( aObserver )
    {
    CActiveScheduler::Add( this );    
    }


// Symbian OS default constructor can leave.
void CNcnTimer::ConstructL()
    {
    CTimer::ConstructL();
    }

// Two-phased constructor.
CNcnTimer* CNcnTimer::NewL( MNcnTimer& aInterface )
    {
    // Create object
    CNcnTimer* self =
        new ( ELeave ) CNcnTimer( aInterface );

    // Push to cleanupstack while constructing
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CNcnTimer::~CNcnTimer()
    {
    Cancel();
    }

// ----------------------------------------------------
// CNcnTimer::RunL
// ----------------------------------------------------
void CNcnTimer::RunL()
    {
    iObserver.NcnTimerCompleted();
    }


//  End of File
