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
*     Timer implementation designed for always online timed operations
*
*/


#include "AlwaysOnlineEmailPluginLogging.h"
#include "AlwaysOnlineEmailLoggingTools.h"
#include "AlwaysOnlineEmailPluginTimer.h"

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailPluginTimer::NewL
// ----------------------------------------------------------------------------
CAlwaysOnlineEmailPluginTimer* CAlwaysOnlineEmailPluginTimer::NewL( 
    CMsvSession& aMsvSession, TRequestStatus& aStatus )
    {
    AOLOG_IN( "CAlwaysOnlineEmailPluginTimer::NewL" );
    CAlwaysOnlineEmailPluginTimer* self =
        new( ELeave ) CAlwaysOnlineEmailPluginTimer( 
            aMsvSession, CActive::EPriorityIdle, aStatus );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailPluginTimer::CAlwaysOnlineEmailPluginTimer
// ----------------------------------------------------------------------------
CAlwaysOnlineEmailPluginTimer::CAlwaysOnlineEmailPluginTimer(
    CMsvSession& aMsvSession, 
    TInt aPriority, 
    TRequestStatus& aObserverRequestStatus )
    : CMsvOperation(aMsvSession, aPriority, aObserverRequestStatus)
    {
    }


// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailPluginTimer::~CAlwaysOnlineEmailPluginTimer()
// ----------------------------------------------------------------------------
CAlwaysOnlineEmailPluginTimer::~CAlwaysOnlineEmailPluginTimer()
    {
    AOLOG_IN( "CAlwaysOnlineEmailPluginTimer::~CAlwaysOnlineEmailPluginTimer" );
    iTimer.Close();
    }


// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailPluginTimer::ConstructL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailPluginTimer::ConstructL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailPluginTimer::ConstructL" );
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add(this);
    }


// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailPluginTimer::ProgressL
// ----------------------------------------------------------------------------
const TDesC8& CAlwaysOnlineEmailPluginTimer::ProgressL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailPluginTimer::ProgressL" );
    return iProgress;
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailPluginTimer::At
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailPluginTimer::At(const TTime& aTime)
    {
    AOLOG_IN( "CAlwaysOnlineEmailPluginTimer::At" );
    iCompletionTime = aTime;
    iTimer.Cancel();
    iTimer.At(iStatus, aTime); // CSI: 1 # not an array
    SetActive();
    }

// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailPluginTimer::DoCancel
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailPluginTimer::DoCancel()
    {
    AOLOG_IN( "CAlwaysOnlineEmailPluginTimer::DoCancel" );
    iTimer.Cancel();
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, KErrCancel );
    }


// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailPluginTimer::RunL
// ----------------------------------------------------------------------------
void CAlwaysOnlineEmailPluginTimer::RunL()
    {
    AOLOG_IN( "CAlwaysOnlineEmailPluginTimer::RunL" );
    KAOEMAIL_LOGGER_WRITE_FORMAT2("CAlwaysOnlineEmailPluginTimer::RunL(). OpId: %d RequestStatus: %d ",Id(), iObserverRequestStatus.Int() );
    TTime home;
    home.HomeTime();

    //get rid of microseconds, we don't need them. Second is accurate enough. Microseconds can and will change during execution anyway.
    TDateTime dtHome = home.DateTime();
    TDateTime dtCompletion = iCompletionTime.DateTime();
    dtHome.SetMicroSecond(0);
    dtCompletion.SetMicroSecond(0);

    home = dtHome;
    iCompletionTime = dtCompletion;

    
    KAOEMAIL_LOGGER_WRITE_DATETIME("CAlwaysOnlineEmailPluginTimer::RunL() HomeTime: ", home );
    KAOEMAIL_LOGGER_WRITE_DATETIME("CAlwaysOnlineEmailPluginTimer::RunL() CompletionTime should be: ", iCompletionTime );
   
    if( home >= iCompletionTime)
        {
        TRequestStatus* status = &iObserverRequestStatus;
        User::RequestComplete(status, KErrNone);
        }
    else
        {
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineEmailPluginTimer::RunL() Timer stopped prematurely! Not going to complete, restarting timer");
        KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineEmailPluginTimer::RunL() iStatus: %d", iStatus.Int());
        Cancel();
        iTimer.At(iStatus, iCompletionTime);  // CSI: 1 # not an array
        SetActive();
        }
    }
    
// ----------------------------------------------------------------------------
// CAlwaysOnlineEmailPluginTimer::RunError()
// ----------------------------------------------------------------------------
//
TInt CAlwaysOnlineEmailPluginTimer::RunError( TInt /* aError */ )
    {
    AOLOG_IN( "CAlwaysOnlineEmailPluginTimer::RunError" );
    // Just ignore any error and continue without
    // any handling to allow smooth execution. 
    return KErrNone;            
    }
    
//EOF
