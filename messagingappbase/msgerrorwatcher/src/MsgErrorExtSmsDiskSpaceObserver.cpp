/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*       CMsgErrorExtSmsDiskSpaceObserver implementation file
*
*
*/


// INCLUDE FILES
#include <e32base.h>

#include <e32property.h>

#include <smsuaddr.h>
#include "MsgErrorWatcher.h"
#include "MsgErrorExtSmsDiskSpaceObserver.h"
#include "MsgErrorWatcherLogging.h"
// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgErrorExtSmsDiskSpaceObserver::CMsgErrorExtSmsDiskSpaceObserver
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgErrorExtSmsDiskSpaceObserver::CMsgErrorExtSmsDiskSpaceObserver( CMsgErrorWatcher* aWatcher )
    : CActive( CActive::EPriorityStandard ),
    iWatcher( aWatcher )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMsgErrorExtSmsDiskSpaceObserver::ConstructL()
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgErrorExtSmsDiskSpaceObserver::ConstructL()
    {
    MEWLOGGER_WRITE( "CMsgErrorExtSmsDiskSpaceObserver::ConstructL: BEFORE ATTACH" );
            
    TInt success = iSmsDiskSpaceProperty.Attach( KUidSystemCategory, KUidPSDiskSpaceMonitorKeyType );
    MEWLOGGER_WRITEF( _L("iSmsDiskSpaceProperty.Attach: %d") ,success );
    User::LeaveIfError(success);    
        
    
    
    MEWLOGGER_WRITE( "CMsgErrorExtSmsDiskSpaceObserver::ConstructL: AFTER ATTACH" );
    
    iStatus = KRequestPending;
    iSmsDiskSpaceProperty.Subscribe( iStatus );
    SetActive();
    }

// ---------------------------------------------------------
// CMsgErrorExtSmsDiskSpaceObserver::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgErrorExtSmsDiskSpaceObserver* CMsgErrorExtSmsDiskSpaceObserver::NewL( CMsgErrorWatcher* aWatcher)
    {
    CMsgErrorExtSmsDiskSpaceObserver* self = new ( ELeave )
        CMsgErrorExtSmsDiskSpaceObserver( aWatcher);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CMsgErrorExtSmsDiskSpaceObserver::~CMsgErrorExtSmsDiskSpaceObserver
//
// Destructor
// ---------------------------------------------------------
//
CMsgErrorExtSmsDiskSpaceObserver::~CMsgErrorExtSmsDiskSpaceObserver()
    {
    Cancel();
    iSmsDiskSpaceProperty.Close();
    }

// ---------------------------------------------------------
// CMsgErrorExtSmsDiskSpaceObserver::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorExtSmsDiskSpaceObserver::DoCancel()
    {
    iSmsDiskSpaceProperty.Cancel();
    }

// ---------------------------------------------------------
// CMsgErrorExtSmsDiskSpaceObserver::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorExtSmsDiskSpaceObserver::RunL()
    {
    MEWLOGGER_WRITE( "CMsgErrorExtSmsDiskSpaceObserver::RunL" );
    TInt messagesComingInLowDiskSpace(ESmsDiskSpaceUnknown);
    TInt success = iSmsDiskSpaceProperty.Get( messagesComingInLowDiskSpace );
    
    MEWLOGGER_WRITEF( _L("iSmsDiskSpaceProperty.Get returns: %d") ,success );
    MEWLOGGER_WRITEF( _L("iSmsDiskSpaceProperty.Get( %d ) ") ,messagesComingInLowDiskSpace );
        
    
    if ( messagesComingInLowDiskSpace == ESmsDiskSpaceFull )
        {
        MEWLOGGER_WRITE( "CMsgErrorExtSmsDiskSpaceObserver::RunL: ESmsDiskSpaceFull" );
        TRAP_IGNORE( iWatcher->HandleDiskSpaceEvent2L() );
        }

    iStatus = KRequestPending;
    iSmsDiskSpaceProperty.Subscribe( iStatus );
    SetActive();
    }



// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
