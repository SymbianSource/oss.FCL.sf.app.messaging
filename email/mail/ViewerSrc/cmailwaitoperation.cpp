/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wait operatin
*
*/

#include "cmailwaitoperation.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMailWaitOperation()
// ---------------------------------------------------------------------------
//
CMailWaitOperation::CMailWaitOperation(
	CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus ) : 
	CMsvOperation( 
	aMsvSession, 
	CActive::EPriorityStandard, 
	aObserverRequestStatus )
    {
    // Just wait and do nothing.
    CActiveScheduler::Add( this );
    iStatus = KRequestPending;
    SetActive();
    iObserverRequestStatus = KRequestPending;    
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CMailWaitOperation* CMailWaitOperation::NewL(
   	CMsvSession& aMsvSession, 
   	TRequestStatus& aObserverRequestStatus)
    {
    return new( ELeave) CMailWaitOperation(
        aMsvSession, 
        aObserverRequestStatus );
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMailWaitOperation::~CMailWaitOperation()
    {
    Cancel();
    }


// ---------------------------------------------------------------------------
// From class CMsvOperation.
// ---------------------------------------------------------------------------
//
const TDesC8& CMailWaitOperation::ProgressL()
    {
    // No need to report progress
    return KNullDesC8();
    }


// ---------------------------------------------------------------------------
// From class CActive.
// ---------------------------------------------------------------------------
//    
void CMailWaitOperation::DoCancel()
    {
    // Stop waiting. No need to report cancelation
    TRequestStatus* ownstatus = &iStatus;
    User::RequestComplete( ownstatus, KErrCancel );    
    
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, KErrNone );//iStatus.Int() );    
    }


// ---------------------------------------------------------------------------
// From class CActive.
// ---------------------------------------------------------------------------
//  
void CMailWaitOperation::RunL()
    {
    // Never come here
    ASSERT( NULL );
    }

// End Of File
