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
*    CMsgErrorDisconnectDlg implementation file
*
*/



#include <e32base.h>
#include <DisconnectDlgClient.h>

#include "MsgErrorDisconnectDlg.h"

// LOCAL CONSTANTS AND MACROS

// ---------------------------------------------------------
// CMsgErrorDisconnectDlg::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgErrorDisconnectDlg* CMsgErrorDisconnectDlg::NewL()
    {
    CMsgErrorDisconnectDlg* self = new ( ELeave ) CMsgErrorDisconnectDlg();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMsgErrorDisconnectDlg::ConstructL
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgErrorDisconnectDlg::ConstructL()
    {
    User::LeaveIfError( iDialogServer.Connect() );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMsgErrorDisconnectDlg::CMsgErrorDisconnectDlg
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgErrorDisconnectDlg::CMsgErrorDisconnectDlg() 
    : CActive( CActive::EPriorityStandard )
    {
    }

// ---------------------------------------------------------
// CMsgErrorDisconnectDlg::~CMsgErrorDisconnectDlg
//
// Destructor
// ---------------------------------------------------------
//
CMsgErrorDisconnectDlg::~CMsgErrorDisconnectDlg()
    {
    Cancel();
    iDialogServer.Close();
    }

// ---------------------------------------------------------
// CMsgErrorDisconnectDlg::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorDisconnectDlg::RunL() 
    {
    //We're not really interested in what happened.
    //We are not informing MsgErrorWatcher that the dialog
    //      is dismissed and MsgErrorWatcher could then 
    //      delete this CMsgErrorDisconnectDlg.
    }

// ---------------------------------------------------------
// CMsgErrorDisconnectDlg::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorDisconnectDlg::DoCancel()
    {
    iDialogServer.CancelDisconnectDlg();
    }

// ---------------------------------------------------------
// CMsgErrorDisconnectDlg::Start
//
// Starts the active object
// ---------------------------------------------------------
//
void CMsgErrorDisconnectDlg::Start()
    {
    if ( !IsActive() ) 
        {
        iDialogServer.DisconnectDlg( iStatus );
        SetActive();
        }
    }


// End of File
