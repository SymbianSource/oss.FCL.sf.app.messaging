/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Methods for CNcnClass0Viewer class.
*
*/



// INCLUDE FILES
#include    "NcnDebug.h"
#include    "CNcnClass0Viewer.h"
#include    "CNcnMsvSessionHandler.h"
#include    <MuiuMsgEditorLauncher.h>

// ================= LOCAL CONSTANTS =======================
namespace
    {
    const TUid KUidMsgSmsViewer = {0x100058BD};
    const TUid KUidSmsMtmId = {0x1000102c};
    }

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
//  CNcnClass0Viewer::CNcnClass0Viewer
// ----------------------------------------------------
//
CNcnClass0Viewer::CNcnClass0Viewer(
    CNcnMsvSessionHandler& aMsvSessionHandler ) :
    CActive( EPriorityNormal ),
    iMsvSessionHandler( aMsvSessionHandler )
    {
    // empty
    }

// ----------------------------------------------------
//  CNcnClass0Viewer::NewL
// ----------------------------------------------------
//
CNcnClass0Viewer* CNcnClass0Viewer::NewL(
    CNcnMsvSessionHandler& aMsvSessionHandler )
    {
    CNcnClass0Viewer* self = new (ELeave) CNcnClass0Viewer( aMsvSessionHandler );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ----------------------------------------------------
//  CNcnClass0Viewer::ConstructL
// ----------------------------------------------------
//
void CNcnClass0Viewer::ConstructL()
    {
    // add AO to scheduler
    CActiveScheduler::Add( this );   
    }

// ----------------------------------------------------
//  CNcnClass0Viewer::~CNcnClass0Viewer
// ----------------------------------------------------
//    
CNcnClass0Viewer::~CNcnClass0Viewer()
    {
    Cancel();
    }

// ----------------------------------------------------
//  CNcnClass0Viewer::RunL
// ----------------------------------------------------
//
void CNcnClass0Viewer::RunL()
    {
    // just release operation instance, if any
    if( iMsvOperation )
        {
        delete iMsvOperation;
        iMsvOperation = 0;
        }
    }

// ----------------------------------------------------
//  CNcnClass0Viewer::RunError
// ----------------------------------------------------
//
TInt CNcnClass0Viewer::RunError( TInt aError )
    {
    return aError;
    }

// ----------------------------------------------------
//  CNcnClass0Viewer::DoCancel
// ----------------------------------------------------
//
void CNcnClass0Viewer::DoCancel()
    {
    // cancel & release operation instance, if any
    if( iMsvOperation )
        {
        iMsvOperation->Cancel();
        delete iMsvOperation;
        iMsvOperation = 0;
        }
    }

// ----------------------------------------------------
//  CNcnClass0Viewer::ShowClass0SmsL
// ----------------------------------------------------
//
void CNcnClass0Viewer::ShowClass0SmsL( const TMsvEntry& aEntry )
    {
    //Cancel outstanding requests
    if( IsActive() )
	    {
	    Cancel();
	    }
	
	//Create launch parameters        
    TEditorParameters launchParams;		
	launchParams.iId = aEntry.Id();
	launchParams.iFlags |= EMsgReadOnly;

    // get session reference
    CMsvSession& msvSession = 
        iMsvSessionHandler.MsvSessionL();
        
    // launch editor
	iMsvOperation =
	    MsgEditorLauncher::LaunchEditorApplicationL(
	        msvSession,
	        KUidSmsMtmId,
	        iStatus, 
		    launchParams,
		    *(MsvUiEditorUtilities::ResolveAppFileNameL( KUidMsgSmsViewer ) ) );

    // set status and activate AO	
    iStatus = KRequestPending;
    SetActive();
    }

//  End of File
