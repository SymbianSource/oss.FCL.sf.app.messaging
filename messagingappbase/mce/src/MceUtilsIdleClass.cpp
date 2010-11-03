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
*   Active object to count messages background for mainview.
*
*/




// INCLUDE FILES


#include "MceUtilsIdleClass.h"

#include "MceLogText.h"


// CONSTANTS




// ================= MEMBER FUNCTIONS =======================


CMceUtilsIdleClass* CMceUtilsIdleClass::NewL(
    MMceUtilsIdleClassObserver& aObserver )
    {
    CMceUtilsIdleClass* self = new (ELeave) CMceUtilsIdleClass( aObserver );
    return self;
    }


CMceUtilsIdleClass::CMceUtilsIdleClass(
    MMceUtilsIdleClassObserver& aObserver )
    :   
    CActive( CActive::EPriorityIdle ),
    iObserver( aObserver )
    {
    CActiveScheduler::Add(this);
    }

// destructor
CMceUtilsIdleClass::~CMceUtilsIdleClass()
    {
    Cancel();
    }

// ---------------------------------------------------------
// CMceUtilsIdleClass::StartL
// ---------------------------------------------------------
//
void CMceUtilsIdleClass::StartL()
    {
    MCELOGGER_ENTERFN("CMceUtilsIdleClass::StartL()");
    if(!IsActive())
        {
        TRequestStatus *s = &iStatus;
        User::RequestComplete(s, KErrNone);
        SetActive();
        MCELOGGER_WRITE_TIMESTAMP("Started at: ");
        }
#ifdef _DEBUG
    else
        {
        MCELOGGER_WRITE_TIMESTAMP("Already running, time: ");
        }        
#endif
        MCELOGGER_LEAVEFN("CMceUtilsIdleClass::StartL()");
    }


// ---------------------------------------------------------
// CMceUtilsIdleClass::FinishL
// ---------------------------------------------------------
//
void CMceUtilsIdleClass::ForceFinishL()
    {
    MCELOGGER_ENTERFN("CMceUtilsIdleClass::ForceFinishL()");
    if(IsActive())
        {
        // First consume the outstanding completion.
        Cancel();
        // Now try to do remaining steps synchronously.
        TBool startAgain = EFalse;
        TInt i = 0;
        MCELOGGER_WRITE_TIMESTAMP("start time: ");
        startAgain = iObserver.MMceUtilsIdleClassStepL();
        while( startAgain && i < 100 /* safety */)
            {
            i++;
            startAgain = iObserver.MMceUtilsIdleClassStepL();
            }
        MCELOGGER_WRITE_TIMESTAMP("stop time: ");
        }
    MCELOGGER_LEAVEFN("CMceUtilsIdleClass::ForceFinishL()");
    }


// ---------------------------------------------------------
// CMceUtilsIdleClass::DoCancel
// ---------------------------------------------------------
//
void CMceUtilsIdleClass::DoCancel()
    {
    }


// ---------------------------------------------------------
// CMceUtilsIdleClass::RunL
// ---------------------------------------------------------
//
void CMceUtilsIdleClass::RunL()
    {
    MCELOGGER_ENTERFN("CMceUtilsIdleClass::RunL()");
    QueueNextL();
#ifdef _DEBUG    
    if(!IsActive())
        {
        MCELOGGER_WRITE("CMceUtilsIdleClass: finished");
        MCELOGGER_WRITE_TIMESTAMP("CMceUtilsIdleClass Time: ");
        }
#endif        
    MCELOGGER_LEAVEFN("CMceUtilsIdleClass::RunL()");
    }

// ---------------------------------------------------------
// CMceUtilsIdleClass::RunError
// ---------------------------------------------------------
//
TInt CMceUtilsIdleClass::RunError(TInt /*aError*/)
    {
    // ignore all the errors
    return KErrNone;
    }

// ---------------------------------------------------------
// CMceUtilsIdleClass::QueueLoad
// ---------------------------------------------------------
//
void CMceUtilsIdleClass::QueueNextL()
    {
    MCELOGGER_ENTERFN("CMceUtilsIdleClass::QueueNextL()");
    TBool startAgain = EFalse;
    MCELOGGER_WRITE_TIMESTAMP("About to call MMceUtilsIdleClassStepL: ");
    startAgain = iObserver.MMceUtilsIdleClassStepL();
    MCELOGGER_WRITE_TIMESTAMP("<--- MMceUtilsIdleClassStepL done ");
    // OPEN: should we start again in case of leave?
    // OPEN: Is there need for some safety counter???
    if( startAgain )
        {
        TRequestStatus *s = &iStatus;
        User::RequestComplete(s, KErrNone);
        SetActive();
        MCELOGGER_WRITE_TIMESTAMP("Started again at: ");
        }
    MCELOGGER_LEAVEFN("CMceUtilsIdleClass::QueueNextL()");
    }


//  End of File
