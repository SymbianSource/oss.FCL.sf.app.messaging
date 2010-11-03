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
*     Msv operation for server app watching
*
*/



#include "MuiuMsgEditorServerWatchingOperation.h"
#include "MuiuMsgEditorService.h"
#include "muiudomainpan.h"

// CONSTANTS
const TInt KiProgressBufferSize = 1;


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CMsgEditorServerWatchingOperation::NewL
// ---------------------------------------------------------
//
EXPORT_C CMsgEditorServerWatchingOperation* CMsgEditorServerWatchingOperation::NewL( 
	CMsvSession& aMsvSession,
    TInt aPriority,
    TRequestStatus& aObserverRequestStatus,
    TUid aMtm,
    const TEditorParameters aParams )
	{
	CMsgEditorServerWatchingOperation* self = 
		new(ELeave) CMsgEditorServerWatchingOperation( 
			aMsvSession,
			aPriority,
			aObserverRequestStatus,
			aMtm );
	CleanupStack::PushL(self);
    self->ConstructL( aParams, aMtm );
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------
// CMsgEditorServerWatchingOperation::ConstructL
// ---------------------------------------------------------
//
void CMsgEditorServerWatchingOperation::ConstructL( const TEditorParameters aParams, 
                                                    TUid aMtmType )
    {
	iService = CMuiuMsgEditorService::NewL( aParams, aMtmType, this );
    iObserverRequestStatus = KRequestPending;
    Start( );
    }

// ---------------------------------------------------------
// CMsgEditorServerWatchingOperation::CMsgEditorServerWatchingOperation
// ---------------------------------------------------------
//
EXPORT_C CMsgEditorServerWatchingOperation::CMsgEditorServerWatchingOperation
	(
    CMsvSession& aMsvSession, 
    TInt aPriority, 
    TRequestStatus& aObserverRequestStatus, 
    TUid aMtm 
    )
    :   
    CMsvOperation(
    aMsvSession, 
    aPriority, 
    aObserverRequestStatus )
    {
    iMtm = aMtm;
    }

// ---------------------------------------------------------
// CMsgEditorServerWatchingOperation::~CMsgEditorServerWatchingOperation
// ---------------------------------------------------------
//
EXPORT_C CMsgEditorServerWatchingOperation::~CMsgEditorServerWatchingOperation()
    {
    Cancel();
    delete iService;
    delete iProgress;
    }


// -----------------------------------------------------------------------------
// CMsgEditorServerWatchingOperation::ProgressL
// From CMsvOperation
// -----------------------------------------------------------------------------
//
const TDesC8& CMsgEditorServerWatchingOperation::ProgressL()
    {
    if ( !iProgress )
        {
        iProgress = HBufC8::NewL( KiProgressBufferSize );
        }
    return *iProgress;
    }


// -----------------------------------------------------------------------------
// CMsgEditorServerWatchingOperation::FinalProgress
// From CMsvOperation
// -----------------------------------------------------------------------------
//
const TDesC8& CMsgEditorServerWatchingOperation::FinalProgress()
    {
    __ASSERT_ALWAYS( !IsActive(), Panic( EMuiuEditorWatcherActiveInFinalProgress ) );
    return *iProgress;
    }


// -----------------------------------------------------------------------------
// CMsgEditorServerWatchingOperation::DoCancel
// From CActive
// -----------------------------------------------------------------------------
//
void CMsgEditorServerWatchingOperation::DoCancel()
    {
    if ( iStatus == KRequestPending )
        {
        TRequestStatus* pstat = &iStatus;
        User::RequestComplete( pstat, KErrCancel );
        }
    CompleteObserver( KErrCancel );
    }


// -----------------------------------------------------------------------------
// CMsgEditorServerWatchingOperation::RunL
// From CActive
// -----------------------------------------------------------------------------
//
void CMsgEditorServerWatchingOperation::RunL()
    {
    CompleteObserver( iStatus.Int( ) );
    }


// -----------------------------------------------------------------------------
// CMsgEditorServerWatchingOperation::Start
//
// -----------------------------------------------------------------------------
//
void CMsgEditorServerWatchingOperation::Start()
    {
    CActiveScheduler::Add( this );
    iStatus = KRequestPending;
    SetActive();
    }


// -----------------------------------------------------------------------------
// CMsgEditorServerWatchingOperation::CompleteObserver
// Completes observer with the completion code
// -----------------------------------------------------------------------------
//
void CMsgEditorServerWatchingOperation::CompleteObserver( TInt aCode )
    {
    TRequestStatus* status = &iObserverRequestStatus;
    
    if( ( *status ) == KRequestPending )
        {
        User::RequestComplete( status, aCode );
        }
    }



// ---------------------------------------------------------
// CMsgEditorServerWatchingOperation::HandleServerAppExit
// ---------------------------------------------------------
//
void CMsgEditorServerWatchingOperation::HandleServerAppExit( TInt aReason )
	{
    TInt exitMode = aReason;
    if ( iStatus == KRequestPending )
    	{
    	TRequestStatus* pstat = &iStatus;
    	User::RequestComplete( pstat, exitMode );
    	}
	MAknServerAppExitObserver::HandleServerAppExit( aReason );	
	}

// End of file
