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
*     Allows a synchronous wait on a operation
*
*/




// INCLUDE FILES
#include <aknenv.h>
#include "MsgOperationWait.h"



// ============================ MEMBER FUNCTIONS ===============================


// ---------------------------------------------------------
// CMsgOperationWait::NewLC
//
// ---------------------------------------------------------
//
EXPORT_C CMsgOperationWait* CMsgOperationWait::NewLC( TInt aPriority )
    {
    CMsgOperationWait* self = new ( ELeave ) CMsgOperationWait( aPriority );
    CleanupStack::PushL( self );
    return self;
    }


// ---------------------------------------------------------
// CMsgOperationWait::CMsgOperationWait
//
// ---------------------------------------------------------
//
CMsgOperationWait::CMsgOperationWait( TInt aPriority )
: CActive( aPriority )
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------
// CMsgOperationWait::~CMsgOperationWait
//
// ---------------------------------------------------------
//
EXPORT_C CMsgOperationWait::~CMsgOperationWait()
    {
    Cancel();
    }


// ---------------------------------------------------------
// CMsgOperationWait::Start
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgOperationWait::Start()
    {
    SetActive();
    iWait.Start();
    }


// ---------------------------------------------------------
// CMsgOperationWait::RunL
//
// ---------------------------------------------------------
//
void CMsgOperationWait::RunL()
    {
    CAknEnv::StopSchedulerWaitWithBusyMessage( iWait );
    }


// ---------------------------------------------------------
// CMsgOperationWait::DoCancel
//
// ---------------------------------------------------------
//
void CMsgOperationWait::DoCancel()
    {
    if( iStatus == KRequestPending )
        {
        TRequestStatus* s=&iStatus;
        User::RequestComplete( s, KErrCancel );
        }

    CAknEnv::StopSchedulerWaitWithBusyMessage( iWait );
    }

// End of file
