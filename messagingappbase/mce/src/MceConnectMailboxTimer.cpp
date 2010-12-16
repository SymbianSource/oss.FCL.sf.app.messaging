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
*     Active object for timing "Connect" query for Remote mailbox.
*
*/



// INCLUDE FILES
#include <msvids.h> // KMsvNullIndexEntryId
#include "MceConnectMailboxTimer.h"
#include "mceui.h"

// LOCAL CONSTANTS AND MACROS
const TInt KMceRemoteMailboxConnectionTimeout = 3000000; // 3 seconds

// ================= MEMBER FUNCTIONS =======================

// C++ constructor can NOT contain any code that
// might leave.
//
CMceConnectMailboxTimer::CMceConnectMailboxTimer( CMceUi& aMceUi ) :
    CActive( EPriorityStandard ),
    iMceUi( aMceUi ),
    iAccountId( KMsvNullIndexEntryId )
    {
    }


// Symbian OS default constructor can leave.
void CMceConnectMailboxTimer::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    }

// Two-phased constructor.
CMceConnectMailboxTimer* CMceConnectMailboxTimer::NewL(
            CMceUi& aMceUi )
    {
    CMceConnectMailboxTimer* self =
        new (ELeave) CMceConnectMailboxTimer(
        aMceUi );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CMceConnectMailboxTimer::~CMceConnectMailboxTimer()
    {
    Cancel();
    iTimer.Close();
    }

// ----------------------------------------------------
// CMceConnectMailboxTimer::SetTimer
// ----------------------------------------------------
void CMceConnectMailboxTimer::SetTimer( TMsvId aAccountId )
    {
    const TTimeIntervalMicroSeconds32 timeInterval = KMceRemoteMailboxConnectionTimeout;
    iAccountId = aAccountId;
    Cancel();
    iTimer.After( iStatus, timeInterval );
    SetActive();
    }

// ----------------------------------------------------
// CMceConnectMailboxTimer::RunL
// ----------------------------------------------------
void CMceConnectMailboxTimer::RunL()
    {
    iMceUi.GoOnlineWithQueryL( iAccountId );
    }

// ----------------------------------------------------
// CMceConnectMailboxTimer::DoCancel
// ----------------------------------------------------
void CMceConnectMailboxTimer::DoCancel()
    {
    iTimer.Cancel();
    }

// ----------------------------------------------------
// CMceConnectMailboxTimer::RunError
// ----------------------------------------------------
TInt CMceConnectMailboxTimer::RunError( TInt /*aError*/ )
    {
    // Just ignore any error and continue without
    // any handling to allow smooth execution. 
    return KErrNone; 
    }

//  End of File
