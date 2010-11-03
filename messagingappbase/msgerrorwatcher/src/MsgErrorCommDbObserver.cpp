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
*       CMsgErrorCommDbObserver implementation file
*
*/



// INCLUDE FILES
#include    <e32base.h>
#include    <commdb.h>

#include    "MsgErrorWatcher.h"
#include    "MsgErrorCommDbObserver.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgErrorCommDbObserver::CMsgErrorCommDbObserver
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgErrorCommDbObserver::CMsgErrorCommDbObserver( CMsgErrorWatcher* aWatcher )
    : CActive( CActive::EPriorityStandard ),
    iWatcher( aWatcher )
    {
    }

// ---------------------------------------------------------
// CMsgErrorCommDbObserver::ConstructL
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgErrorCommDbObserver::ConstructL()
    {
    iDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CActiveScheduler::Add( this );
    Start();
    }

// ---------------------------------------------------------
// CMsgErrorCommDbObserver::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgErrorCommDbObserver* CMsgErrorCommDbObserver::NewL( CMsgErrorWatcher* aWatcher )
    {
    CMsgErrorCommDbObserver* self = new ( ELeave )
        CMsgErrorCommDbObserver( aWatcher );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// ---------------------------------------------------------
// CMsgErrorCommDbObserver::~CMsgErrorCommDbObserver
//
// Destructor
// ---------------------------------------------------------
//
CMsgErrorCommDbObserver::~CMsgErrorCommDbObserver()
    {
    Cancel();
    delete iDb;
    }

// ---------------------------------------------------------
// CMsgErrorCommDbObserver::Restart
//
// (Re)starts the active object (public)
// ---------------------------------------------------------
//
void CMsgErrorCommDbObserver::Restart()
    {
    Start();
    }

// ---------------------------------------------------------
// CMsgErrorCommDbObserver::Start
//
// Starts the active object
// ---------------------------------------------------------
//
void CMsgErrorCommDbObserver::Start()
    {
    if ( !IsActive() ) 
        {
        iDb->RequestNotification( iStatus );
        SetActive();       
        }
    }

// ---------------------------------------------------------
// CMsgErrorCommDbObserver::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorCommDbObserver::DoCancel()
    {
    if ( iDb )
        {
        iDb->CancelRequestNotification();
        }
    }

// ---------------------------------------------------------
// CMsgErrorCommDbObserver::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorCommDbObserver::RunL()
    {
    if ( iStatus == RDbNotifier::ECommit )
        {
        TRAP_IGNORE( iWatcher->HandleCommDbEventL() );
        }
    else
        {
        Start();
        }
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
