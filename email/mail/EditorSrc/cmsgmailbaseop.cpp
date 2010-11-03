/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MMsgMailOpObserver
*
*/



// INCLUDE FILES
#include "cmsgmailbaseop.h"
#include "mmsgmailappuiopdelegate.h"
#include "MailLog.h"
#include "MsgMailEditorDocument.h"
#include <StringLoader.h>
#include <eikenv.h>
#include <MsgEditorAppUi.rsg>
#include <MsgMailEditor.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMsgMailBaseOp::CMsgMailBaseOp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMsgMailBaseOp::CMsgMailBaseOp(
    CMsgMailEditorDocument& aDocument )
    : CActive( CActive::EPriorityStandard ),
      iDocument( aDocument ),
      iState( EIdleState ),
      iWaitingError( KMaxTInt )
    {
    CActiveScheduler::Add( this );
    }

// Destructor
CMsgMailBaseOp::~CMsgMailBaseOp()
    {
    LOG( "CMsgMailBaseOp::~CMsgMailBaseOp" );
    }

// -----------------------------------------------------------------------------
// CMsgMailBaseOp::DoCancel
// -----------------------------------------------------------------------------
//
void CMsgMailBaseOp::DoCancel()
    {
    LOG1( "CMsgMailBaseOp::DoCancel @ iState:%d", iState );
    HandleOperationCancel(); // let subclass cancel outstanding request
    CompleteOperation( KErrCancel );
    }
    
// -----------------------------------------------------------------------------
// CMsgMailBaseOp::RunL
// -----------------------------------------------------------------------------
//
void CMsgMailBaseOp::RunL()
    {
    LOG1( "CMsgMailBaseOp::RunL @ iState:%d", iState );
    TInt completionCode = iStatus.Int();
    if ( completionCode != KErrNone )
        { // entering this branch means that latest request (iteration) was
          // completed with an error code
        LOG1( "CMsgMailBaseOp::RunL, completed with error: %d",
              completionCode );
        if ( !HandleStateActionError( completionCode ) )
            { // error wasn't handled -> complete operation
            LOG( "CMsgMailBaseOp::RunL, error wasn't handled -> stopping" );
            CompleteOperation( completionCode );
            return;
            }        
        }

    if( iWaitingError <= KErrNone && 
        iState == EIdleState )
        {
        // Wait note was dismissed and we can complete the operation.
        LOG1( "CMsgMailBaseOp::RunL, wait note dismissed -> operation done: %d", 
            iWaitingError );
        NotifyObservers( iWaitingError );
        }
    else
        {
        // Start handling next state
        SetNextState();
        
        if ( iState == EIdleState )
            {
            CompleteOperation( KErrNone );
            }
        else
            {
            HandleStateActionL(); // call subclass to handle the action
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CMsgMailBaseOp::RunError
// -----------------------------------------------------------------------------
//
TInt CMsgMailBaseOp::RunError( TInt aError )
    {   
    LOG2( "CMsgMailBaseOp::RunError @ iState: %d, aError:%d", iState, aError );

    // Usually we must return KErrNone, but with KLeaveExit,KErrDiskFull and KErrNoMemory
    // we must return error code.  
    // We have to return KLeaveExit to get appui deleted. We also don't want
    // to notify observer anymore in that case.
    // KErrDiskFull and KErrNoMemory are returned because otherwise error note is not shown.
    if ( aError == KLeaveExit )
        {        
        LOG1("ERROR %d", aError);
        return aError;
        }
    else if(aError == KErrDiskFull || aError == KErrNoMemory)
        {
        LOG1("ERROR KErrDiskFull or KErrNoMemory: %d", aError);
        CompleteOperation( KErrCancel );
        return aError;
        }    
    else if ( HandleStateActionError( aError ) )
        { // error was handled -> we can continue with next step
        LOG( "CMsgMailBaseOp::RunError, error was handled" );
        return KErrNone;
        }
    else
        { // error wasn't handled -> complete operation
        LOG( "CMsgMailBaseOp::RunError, error wasn't handled -> stopping" );
        CompleteOperation( aError );
        return KErrNone;
        }
    }


// -----------------------------------------------------------------------------
// CMsgMailBaseOp::DialogDismissedL
// -----------------------------------------------------------------------------
//    
void CMsgMailBaseOp::DialogDismissedL( TInt /*aButtonId*/ )
    {
    LOG( "CMsgMailBaseOp::DialogDismissedL" );    
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CMsgMailBaseOp::HandleStateActionError
// -----------------------------------------------------------------------------
//    
TBool CMsgMailBaseOp::HandleStateActionError( TInt /*aError*/ )
    {
    // By default errors are not handled or tolerated
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CMsgMailBaseOp::StartOp
// -----------------------------------------------------------------------------
//    
void CMsgMailBaseOp::StartOp( MMsgMailOpObserver& aObserver )
    {
    LOG( "CMsgMailBaseOp::StartOp" );
    Cancel();
    ASSERT( iState == EIdleState );
    iObserver = &( aObserver );
    // start iterating through operation states
    CompleteStateAction();    
    }

// -----------------------------------------------------------------------------
// CMsgMailBaseOp::StartOp
// -----------------------------------------------------------------------------
//    
void CMsgMailBaseOp::StartOp( TRequestStatus& aStatus )
    {
    LOG( "CMsgMailBaseOp::StartOp" );
    Cancel();
    ASSERT( iState == EIdleState );
    iExtRequestStatus = &( aStatus );
    *iExtRequestStatus = KRequestPending;
    // start iterating through operation states
    CompleteStateAction();    
    }

// -----------------------------------------------------------------------------
// CMsgMailBaseOp::CompleteStateAction
// -----------------------------------------------------------------------------
//    
void CMsgMailBaseOp::CompleteStateAction( TInt aError )
    {
    LOG1( "CMsgMailBaseOp::CompleteStateAction @ aError:%d", aError );
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, aError );         
    SetActive();
    }

// -----------------------------------------------------------------------------
// CMsgMailBaseOp::CompleteOperation
// -----------------------------------------------------------------------------
//    
void CMsgMailBaseOp::CompleteOperation( TInt aError )
    {
    LOG1( "CMsgMailBaseOp::CompleteOperation @ aError:%d", aError );
    iState = EIdleState;
    StopWaitNote(); // not necessarily started
    
    if( iWaitDialog )
        {
        // wait note is still active. we have to wait for it before 
        // completing the operation.
        iWaitDialog->SetCallback( this );
        iWaitingError = aError;
        }
    else
        {
        // Nothing to wait, we can complete now.
        NotifyObservers( aError );
        }
    }

// -----------------------------------------------------------------------------
// CMsgMailBaseOp::NotifyObservers
// -----------------------------------------------------------------------------
//    
void CMsgMailBaseOp::NotifyObservers( TInt aError )
    {
    LOG1( "CMsgMailBaseOp::NotifyObservers @ aError:%d", aError );
    if ( iObserver )
        {        
        iObserver->HandleOpCompleted( *this, aError );
        iObserver =  NULL; // mustn't reuse observer
        }
    if ( iExtRequestStatus )
        {
        LOG( "NotifyObservers, completing external request" );
        ASSERT( iExtRequestStatus );
        User::RequestComplete( iExtRequestStatus, aError );
        iExtRequestStatus = NULL; // mustn't reuse request status
        }
    }

// ----------------------------------------------------------------------------
// CMsgMailBaseOp::StartWaitNoteL
// ----------------------------------------------------------------------------
//
void CMsgMailBaseOp::StartWaitNoteL(
    TInt aTextResourceId,
    TBool aDelayOff,
    TInt aResouceID )
    {
    LOG( "CMsgMailBaseOp::StartWaitNoteL" );
    StopWaitNote();
    iWaitDialog = new( ELeave ) CAknWaitDialog(
        reinterpret_cast<CEikDialog**>( &iWaitDialog ), aDelayOff );
    iWaitDialog->PrepareLC( aResouceID );
    HBufC* text = StringLoader::LoadLC( aTextResourceId, CEikonEnv::Static() );		// CSI: 27 # Must be used because of 
    																				// iEikEnv is not accessible.
    iWaitDialog->SetTextL( *text );
    CleanupStack::PopAndDestroy( text ); // text    

	// safe to call LD even as a member variable, since sets itself to NULL when deleting
    iWaitDialog->RunLD(); // CSI: 50 # see comment above
    }
  
// ----------------------------------------------------------------------------
// CMsgMailBaseOp::StopWaitNote
// ----------------------------------------------------------------------------
//  
void CMsgMailBaseOp::StopWaitNote()
    {
    LOG( "CMsgMailBaseOp::StopWaitNote" );
    if ( iWaitDialog )
        {
        TRAP_IGNORE( iWaitDialog->ProcessFinishedL() );
        LOG1( "StopWaitNote - process finished @ iWaitDialog:%x",  iWaitDialog );
        }
    }
    
// End Of File
