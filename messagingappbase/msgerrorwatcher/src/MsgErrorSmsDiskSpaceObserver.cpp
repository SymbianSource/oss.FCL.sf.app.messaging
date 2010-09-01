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
*       CMsgErrorSmsDiskSpaceObserver implementation file
*
*/



// INCLUDE FILES
#include <e32base.h>

#include <e32property.h>

#include <smsuaddr.h>
#include "MsgErrorWatcher.h"
#include "MsgErrorSmsDiskSpaceObserver.h"
#include "MsgErrorWatcherLogging.h"
// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgErrorSmsDiskSpaceObserver::CMsgErrorSmsDiskSpaceObserver
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgErrorSmsDiskSpaceObserver::CMsgErrorSmsDiskSpaceObserver( CMsgErrorWatcher* aWatcher )
    : CActive( CActive::EPriorityStandard ),
    iWatcher( aWatcher )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMsgErrorSmsDiskSpaceObserver::ConstructL()
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgErrorSmsDiskSpaceObserver::ConstructL()
    {
    MEWLOGGER_WRITE( "CMsgErrorSmsDiskSpaceObserver::ConstructL: BEFORE ATTACH" );
    
    TInt success = iSmsDiskSpaceProperty.Attach( KUidPSSMSStackCategory, KUidPSSMSStackDiskSpaceMonitorKey );
    MEWLOGGER_WRITEF( _L("iSmsDiskSpaceProperty.Attach: %d") ,success );
    User::LeaveIfError(success);    
        
    
    
    MEWLOGGER_WRITE( "CMsgErrorSmsDiskSpaceObserver::ConstructL: AFTER ATTACH" );
    
    iStatus = KRequestPending;
    iSmsDiskSpaceProperty.Subscribe( iStatus );
    SetActive();
    }

// ---------------------------------------------------------
// CMsgErrorSmsDiskSpaceObserver::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgErrorSmsDiskSpaceObserver* CMsgErrorSmsDiskSpaceObserver::NewL( CMsgErrorWatcher* aWatcher)
    {
    CMsgErrorSmsDiskSpaceObserver* self = new ( ELeave )
        CMsgErrorSmsDiskSpaceObserver( aWatcher);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CMsgErrorSmsDiskSpaceObserver::~CMsgErrorSmsDiskSpaceObserver
//
// Destructor
// ---------------------------------------------------------
//
CMsgErrorSmsDiskSpaceObserver::~CMsgErrorSmsDiskSpaceObserver()
    {
    Cancel();
    iSmsDiskSpaceProperty.Close();
    }

// ---------------------------------------------------------
// CMsgErrorSmsDiskSpaceObserver::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorSmsDiskSpaceObserver::DoCancel()
    {
    iSmsDiskSpaceProperty.Cancel();
    }

// ---------------------------------------------------------
// CMsgErrorSmsDiskSpaceObserver::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorSmsDiskSpaceObserver::RunL()
    {
    MEWLOGGER_WRITE( "CMsgErrorSmsDiskSpaceObserver::RunL" );
    TInt messagesComingInLowDiskSpace(ESmsDiskSpaceUnknown);
    TInt success = iSmsDiskSpaceProperty.Get( messagesComingInLowDiskSpace );
    
    MEWLOGGER_WRITEF( _L("iSmsDiskSpaceProperty.Get returns: %d") ,success );
    MEWLOGGER_WRITEF( _L("iSmsDiskSpaceProperty.Get( %d ) ") ,messagesComingInLowDiskSpace );
        
    
    if ( messagesComingInLowDiskSpace == ESmsDiskSpaceFull )
        {
        MEWLOGGER_WRITE( "CMsgErrorSmsDiskSpaceObserver::RunL: ESmsDiskSpaceFull" );
        TRAP_IGNORE( iWatcher->HandleDiskSpaceEvent2L() );
        }

    iStatus = KRequestPending;
    iSmsDiskSpaceProperty.Subscribe( iStatus );
    SetActive();
    }



// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
