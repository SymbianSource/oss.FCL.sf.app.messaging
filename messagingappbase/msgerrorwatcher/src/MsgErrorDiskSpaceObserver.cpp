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
*     CMsgErrorDiskSpaceObserver implementation file
*
*/



// INCLUDE FILES
#include    <e32base.h>

#include    "MsgErrorWatcher.h"
#include    "MsgErrorDiskSpaceObserver.h"

#ifdef USE_LOGGER
#include "MsgErrorWatcherLogging.h"
#endif

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgErrorDiskSpaceObserver::CMsgErrorDiskSpaceObserver
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgErrorDiskSpaceObserver::CMsgErrorDiskSpaceObserver( 
        CMsgErrorWatcher* aWatcher, 
        CMsvSession& aSession, 
        RFs& aFs )
    : CActive( CActive::EPriorityStandard ),
    iWatcher( aWatcher ),
    iSession( aSession ),
    iFs( aFs ),
    iLimit( 0 )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMsgErrorDiskSpaceObserver::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgErrorDiskSpaceObserver* CMsgErrorDiskSpaceObserver::NewL( 
        CMsgErrorWatcher* aWatcher, 
        CMsvSession& aSession, 
        RFs& aFs )
    {
    CMsgErrorDiskSpaceObserver* self = new ( ELeave )
        CMsgErrorDiskSpaceObserver( aWatcher, aSession, aFs );
    return self;
    }

    
// ---------------------------------------------------------
// CMsgErrorDiskSpaceObserver::~CMsgErrorDiskSpaceObserver
//
// Destructor
// ---------------------------------------------------------
//
CMsgErrorDiskSpaceObserver::~CMsgErrorDiskSpaceObserver()
    {
    Cancel();
    }


// ---------------------------------------------------------
// CMsgErrorDiskSpaceObserver::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorDiskSpaceObserver::DoCancel()
    {
    iFs.NotifyDiskSpaceCancel( iStatus );
    }

// ---------------------------------------------------------
// CMsgErrorDiskSpaceObserver::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorDiskSpaceObserver::RunL()
    {
#ifdef USE_LOGGER
    MEWLOGGER_WRITEF( _L("DiskSpace: RunL, iStatus: %d"), iStatus.Int() );
#endif
    TInt status = iStatus.Int();
    if ( status < 0 )
        {
        //Reset the observer.
        TRAP_IGNORE ( SetLimitAndActivateL( iLimit ) );
        return;
        }
    
    //We shouldn't have to check whether we're above the limit,
    //because we were below when we activated and the event comes
    //when we cross the limit

    TRAP_IGNORE( iWatcher->HandleDiskSpaceEventL() );
    }

// ---------------------------------------------------------
// CMsgErrorDiskSpaceObserver::SetLimitAndActivateL
//
// Sets the disk space trigger level for active object
// and activates it.
// ---------------------------------------------------------
//
void CMsgErrorDiskSpaceObserver::SetLimitAndActivateL( TInt32 aLimit )
    {
#ifdef USE_LOGGER
    MEWLOGGER_ENTERFN( "DiskSpace: SetLimitAndActivateL" );
    MEWLOGGER_WRITEF( _L("DiskSpace: aLimit %d"), aLimit );
#endif
    __ASSERT_DEBUG( aLimit >= 0, User::Leave( KErrArgument ) );
    if ( aLimit < 0 )
        {
        return;
        }
    if ( iLimit == 0 || iLimit > aLimit )
        {
        Cancel();
        iLimit = aLimit;

        TInt currentDrive = TInt( iSession.CurrentDriveL() );
        TVolumeInfo volInfo;
        User::LeaveIfError( iFs.Volume( volInfo, currentDrive ) );
#ifdef USE_LOGGER
        MEWLOGGER_WRITEF( _L("DiskSpace: free/hi %d"), I64HIGH( volInfo.iFree ) );
        MEWLOGGER_WRITEF( _L("DiskSpace: free/lo %d"), I64LOW( volInfo.iFree ) );
#endif
        //check if we're already above the limit
        if ( I64HIGH( volInfo.iFree ) != 0 ||
            I64LOW( volInfo.iFree ) > (TUint)iLimit ) //iLimit is always positive.
            {
            iWatcher->HandleDiskSpaceEventL();
            return;
            }
        else
            {
#ifdef USE_LOGGER
            MEWLOGGER_WRITE( "DiskSpace: notify activated" );
#endif
            TInt64 treshold = iLimit;
            iFs.NotifyDiskSpace( treshold, currentDrive, iStatus );
            // No need to check IsActive since object is cancelled 
            // anyway above
            SetActive();
            }
        }
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

