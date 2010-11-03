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
*       CMsgErrorStartupObserver implementation file
*
*/



// INCLUDE FILES
#include <e32base.h>

#include <startupdomainpskeys.h>

#include "MsgErrorWatcher.h"
#include "MsgErrorStartupObserver.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgErrorStartupObserver::CMsgErrorStartupObserver
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgErrorStartupObserver::CMsgErrorStartupObserver( CMsgErrorWatcher* aWatcher )
    : CActive( CActive::EPriorityStandard ),
    iWatcher( aWatcher )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMsgErrorStartupObserver::ConstructL()
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgErrorStartupObserver::ConstructL()
    {
    User::LeaveIfError( iStartupProperty.Attach( KPSUidStartup, KPSGlobalSystemState ) );
    // Complete self
    iStatus = KRequestPending;
    TRequestStatus* pStatus = &iStatus;
    SetActive();
    User::RequestComplete( pStatus, KErrNone );
    }

// ---------------------------------------------------------
// CMsgErrorStartupObserver::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgErrorStartupObserver* CMsgErrorStartupObserver::NewL( CMsgErrorWatcher* aWatcher )
    {
    CMsgErrorStartupObserver* self = new ( ELeave )
        CMsgErrorStartupObserver( aWatcher );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CMsgErrorStartupObserver::~CMsgErrorStartupObserver
//
// Destructor
// ---------------------------------------------------------
//
CMsgErrorStartupObserver::~CMsgErrorStartupObserver()
    {
    Cancel();
    iStartupProperty.Close();
    }

// ---------------------------------------------------------
// CMsgErrorStartupObserver::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorStartupObserver::DoCancel()
    {
    iStartupProperty.Cancel();
    }

// ---------------------------------------------------------
// CMsgErrorStartupObserver::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorStartupObserver::RunL()
    {
    TInt startupState = 0;
    iStartupProperty.Get( startupState );

    if ( startupState == ESwStateNormalRfOn ||
        startupState == ESwStateNormalRfOff ||
        startupState == ESwStateNormalBTSap )
        {
        TRAP_IGNORE( iWatcher->HandleStartupReadyL() );
        }
    else
        {
        iStatus = KRequestPending;
        iStartupProperty.Subscribe( iStatus );
        SetActive();
        }
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
