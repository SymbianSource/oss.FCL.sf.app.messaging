/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*       CSmumStartUpMonitor implementation file
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <startupdomainpskeys.h>
#include "MsgSimSCNumberDetector.h"
#include "SmumStartUpMonitor.h"

#include "SmumLogging.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSmumStartUpMonitor::CSmumStartUpMonitor
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CSmumStartUpMonitor::CSmumStartUpMonitor( CMsgSimOperation* aWatcher )
    : CActive( CActive::EPriorityStandard ),
    iWatcher( aWatcher )
    {
    SMUMLOGGER_WRITE("CSmumStartUpMonitor::CSmumStartUpMonitor");
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CSmumStartUpMonitor::ConstructL()
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CSmumStartUpMonitor::ConstructL()
    {
    SMUMLOGGER_ENTERFN("CSmumStartUpMonitor::ConstructL");
    User::LeaveIfError( iStartupProperty.Attach( KPSUidStartup, KPSGlobalSystemState ) );
    // Complete self
    iStatus = KRequestPending;
    TRequestStatus* pStatus = &iStatus;
    SetActive();
    User::RequestComplete( pStatus, KErrNone );
    SMUMLOGGER_LEAVEFN("CSmumStartUpMonitor::ConstructL");
    }

// ---------------------------------------------------------
// CSmumStartUpMonitor::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CSmumStartUpMonitor* CSmumStartUpMonitor::NewL( CMsgSimOperation* aWatcher )
    {
    SMUMLOGGER_ENTERFN("CSmumStartUpMonitor::NewL 2");
    CSmumStartUpMonitor* self = new ( ELeave )
        CSmumStartUpMonitor( aWatcher );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    SMUMLOGGER_LEAVEFN("CSmumStartUpMonitor::NewL 2");
    return self;
    }

// ---------------------------------------------------------
// CSmumStartUpMonitor::~CSmumStartUpMonitor
//
// Destructor
// ---------------------------------------------------------
//
CSmumStartUpMonitor::~CSmumStartUpMonitor()
    {
    Cancel();
    }

// ---------------------------------------------------------
// CSmumStartUpMonitor::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CSmumStartUpMonitor::DoCancel()
    {
    iStartupProperty.Cancel();
    }

// ---------------------------------------------------------
// CSmumStartUpMonitor::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CSmumStartUpMonitor::RunL()
    {
    SMUMLOGGER_ENTERFN("CSmumStartUpMonitor::RunL");
    TInt startupState = 0;
    iStartupProperty.Get( startupState );

    if ( startupState == ESwStateNormalRfOn ||
        startupState == ESwStateNormalRfOff ||
        startupState == ESwStateNormalBTSap )
        {
        iWatcher->HandleStartupReadyL();
        }
    else
        {
        iStatus = KRequestPending;
        iStartupProperty.Subscribe( iStatus );
        SetActive();
        }
    SMUMLOGGER_LEAVEFN("CSmumStartUpMonitor::RunL");

    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
