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
*     Base class for implementing asyncronous item saving.
*
*/



// INCLUDE FILES

#include "MsgAsyncItemSaver.h"          // Own header
#include "MsgEditorUtils.pan"           // MsgEditorUtils panics
#include <e32svr.h>

// ================= MEMBER FUNCTIONS =======================

EXPORT_C CMsgAsyncItemSaver::CMsgAsyncItemSaver() : 
CActive( EPriorityStandard ) // superclass constructor
    {
    }

EXPORT_C CMsgAsyncItemSaver::~CMsgAsyncItemSaver()
    {
    Cancel();
    }

EXPORT_C void CMsgAsyncItemSaver::StartOperation(
    MMsgAsyncItemSaver* aItemSaver )
    {
    __ASSERT_DEBUG( aItemSaver, Panic( EMEUAsynItemSaverNULL ) );
    iItemSaver = aItemSaver;
	if (!IsAdded())
		{
	    CActiveScheduler::Add( this );
		}
    NextStep();
    }

EXPORT_C void CMsgAsyncItemSaver::Pause( TInt aMicroSeconds )
    {
    __ASSERT_DEBUG( iItemSaver, Panic( EMEUAsynItemSaverNULL ) );
	iPaused = ETrue;
	iPauseDelay = aMicroSeconds;
    }

EXPORT_C void CMsgAsyncItemSaver::Continue()
    {
    __ASSERT_DEBUG( iItemSaver, Panic( EMEUAsynItemSaverNULL ) );
	if ( iPaused )
		{
		iPaused = EFalse;
		NextStep();
		}
    }

void CMsgAsyncItemSaver::RunL()
    {
    __ASSERT_DEBUG( iItemSaver, Panic( EMEUAsynItemSaverNULL ) );
	if ( iPaused )
		{
		iTimer.Close();
		iPaused = EFalse;
		}

	if ( !iItemSaver->IsComplete() )
        {
        iItemSaver->ContinueOperationL();
        NextStep();
        }
    else
        {
        iItemSaver->CompleteOperationL();
        }
    }

void CMsgAsyncItemSaver::DoCancel()
    {
    // Nothing to cancel
    }

TInt CMsgAsyncItemSaver::RunError(TInt aError)
    {
    TInt error = iItemSaver->HandleError( aError );
    if ( error == KErrNone )
        {
        aError = error;
        NextStep();
        }
    return aError;
    }

void CMsgAsyncItemSaver::NextStep()
    {
	if ( iPaused )
		{
		TInt err = iTimer.CreateLocal(); // created for this thread
		if ( err == KErrNone )
			{
			iTimer.After( iStatus, iPauseDelay );
			}
		}
	else
		{
		TRequestStatus* sp = &iStatus;
		User::RequestComplete(sp, KErrNone);
		}
	SetActive();
    }

//  End of File
