/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       CMsgEditorShutter implementation file
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <aknenv.h>
#include <messaginginternalpskeys.h>
#include <MuiuMsgEditorLauncher.h>
#include "MsgEditorShutter.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgEditorShutter::CMsgEditorShutter
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgEditorShutter::CMsgEditorShutter(CMsgEditorDocument& aDoc)
    : CActive( CActive::EPriorityStandard ),
    iDoc( aDoc )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMsgEditorShutter::ConstructL()
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgEditorShutter::ConstructL()
    {
    User::LeaveIfError( iOpenAppProperty.Attach( KPSUidMuiu, KMuiuSysOpenMsgEditors ) );
    
    // Complete self
    TInt openEditors = 0;
    TInt err = iOpenAppProperty.Get( openEditors );
    
    switch ( err )
        {
        case KErrPermissionDenied:
        case KErrArgument:
            {
            User::Leave( err );
            }
            break;
        case KErrNotFound:
            {
            User::LeaveIfError ( 
                RProperty::Define( 
                    KPSUidMuiu,
                    KMuiuSysOpenMsgEditors,
                    RProperty::EInt,
                    ECapabilityReadDeviceData,
                    ECapabilityWriteDeviceData,
                    0 ) );
            }
            break;
        case KErrNone:
        default:
            break;
        }
    err = iOpenAppStandAloneProperty.Attach(KPSUidMsgEditor,KMuiuStandAloneOpenMsgEditors);
    TInt newValue = 0; // since only one stand alone editor can be launched
    err = iOpenAppStandAloneProperty.Get(KPSUidMsgEditor, KMuiuStandAloneOpenMsgEditors, newValue);
     switch ( err )
        {
        case KErrPermissionDenied:
        case KErrArgument:
            {
            User::Leave( err );
            }
            break;
        case KErrNotFound:
            {
            User::LeaveIfError ( 
                RProperty::Define( 
                    KPSUidMsgEditor,
                    KMuiuStandAloneOpenMsgEditors,
                    RProperty::EInt,
                    ECapabilityReadDeviceData,
                    ECapabilityWriteDeviceData,
                    0 ) );
            }
            break;
        case KErrNone:
        default:
            break;
        }
    openEditors++;
    iOpenAppProperty.Set( openEditors );
    iStatus = KRequestPending;
    iOpenAppProperty.Subscribe( iStatus );
    SetActive();
    }

// ---------------------------------------------------------
// CMsgEditorShutter::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgEditorShutter* CMsgEditorShutter::NewL(CMsgEditorDocument& aAppUi)
    {
    CMsgEditorShutter* self = new ( ELeave ) CMsgEditorShutter(aAppUi);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CMsgEditorShutter::~CMsgEditorShutter
//
// Destructor
// ---------------------------------------------------------
//
CMsgEditorShutter::~CMsgEditorShutter()
    {
    Cancel();
    
    TInt openEditors = 0;
    iOpenAppProperty.Get( openEditors );
    
        // Do not make open editor count negative
    
    if ( openEditors > 0 )
        {
        openEditors--;    
        iOpenAppProperty.Set( openEditors );
        }
    
    TInt standAloneEditorCnt = 0; // since only one stand alone editor can be launched
    iOpenAppStandAloneProperty.Get(KPSUidMsgEditor, KMuiuStandAloneOpenMsgEditors, standAloneEditorCnt);

    if(standAloneEditorCnt > 0)
        {
        standAloneEditorCnt--;
        iOpenAppStandAloneProperty.Set(KPSUidMsgEditor, KMuiuStandAloneOpenMsgEditors, standAloneEditorCnt);
        }

    iOpenAppProperty.Close();
    iOpenAppStandAloneProperty.Close();
    }

// ---------------------------------------------------------
// CMsgEditorShutter::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgEditorShutter::DoCancel()
    {
    iOpenAppProperty.Cancel();
    }

// ---------------------------------------------------------
// CMsgEditorShutter::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgEditorShutter::RunL()
    {
    TInt openEditors = 0;
    iOpenAppProperty.Get( openEditors );

	TInt launchtype = iDoc.LaunchFlags();

    TInt StandAloneEditorCnt = 0; // since only one stand alone editor can be launched
    TInt err = iOpenAppStandAloneProperty.Get(KPSUidMsgEditor, KMuiuStandAloneOpenMsgEditors, StandAloneEditorCnt);
                 
    if((launchtype & EMsgLaunchEditorStandAlone ) && StandAloneEditorCnt > 1 )    
        {
        // Run appshutter
        CAknEnv::RunAppShutter();
        }
    else if ( openEditors  )
        {
        iStatus = KRequestPending;
        iOpenAppProperty.Subscribe( iStatus );
        SetActive();
        }
    else
        {
        // Run appshutter
        CAknEnv::RunAppShutter();
        }
    }

//  End of File  
