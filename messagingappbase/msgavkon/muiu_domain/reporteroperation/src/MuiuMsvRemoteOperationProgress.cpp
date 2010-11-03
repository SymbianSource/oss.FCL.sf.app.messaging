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
*     Messaging progress watching classes
*
*/




// INCLUDE FILES
#include <msvapi.h>
#include "MuiuMsvRemoteOperationProgress.h"


// CONSTANTS
const TInt KRemoteOpProgressPriority=0;


// ============================ MEMBER FUNCTIONS ===============================


// ---------------------------------------------------------
// CMsvRemoteOperationProgress::NewL
//
// ---------------------------------------------------------
//
EXPORT_C CMsvRemoteOperationProgress* CMsvRemoteOperationProgress::NewL( 
                                      MMsvRemoteOperationProgressObserver& aObserver, 
                                      TInt aPeriod )
    {// static
    CMsvRemoteOperationProgress* self = new ( ELeave ) CMsvRemoteOperationProgress( aObserver, 
                                                       KRemoteOpProgressPriority, aPeriod );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); // self
    return self;
    }


// ---------------------------------------------------------
// CMsvRemoteOperationProgress::NewL
//
// ---------------------------------------------------------
//
EXPORT_C CMsvRemoteOperationProgress* CMsvRemoteOperationProgress::NewL( 
                                      MMsvRemoteOperationProgressObserver& aObserver, 
                                      TInt aPriority, TInt aPeriod )
    {// static
    CMsvRemoteOperationProgress* self = new (ELeave) CMsvRemoteOperationProgress( aObserver, 
                                                                                  aPriority, 
                                                                                  aPeriod );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); // self
    return self;
    }


// ---------------------------------------------------------
// CMsvRemoteOperationProgress::CMsvRemoteOperationProgress
//
// ---------------------------------------------------------
//
CMsvRemoteOperationProgress::CMsvRemoteOperationProgress( 
                             MMsvRemoteOperationProgressObserver& aObserver, 
                             TInt aPriority, TInt aPeriod )
    :   CTimer( aPriority ),
        iObserver( aObserver ),
        iPeriod( aPeriod )
    { __DECLARE_NAME( _S( "CMsvRemoteOperationProgress" ) ); }


// ---------------------------------------------------------
// CMsvRemoteOperationProgress::ConstructL
//
// ---------------------------------------------------------
//
void CMsvRemoteOperationProgress::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    IssueRequest();
    }


// ---------------------------------------------------------
// CMsvRemoteOperationProgress::IssueRequest
//
// ---------------------------------------------------------
//
void CMsvRemoteOperationProgress::IssueRequest()
    {
    TTimeIntervalMicroSeconds32 period( iPeriod );
    After( period );
    }


// ---------------------------------------------------------
// CMsvRemoteOperationProgress::RunL
//
// ---------------------------------------------------------
//
void CMsvRemoteOperationProgress::RunL()
    {
    IssueRequest();
    iObserver.UpdateRemoteOpProgressL();
    }


// ---------------------------------------------------------
// CMsvRemoteOperationProgress::~CMsvRemoteOperationProgress
//
// ---------------------------------------------------------
//
CMsvRemoteOperationProgress::~CMsvRemoteOperationProgress()
    {
    }

// End of file
