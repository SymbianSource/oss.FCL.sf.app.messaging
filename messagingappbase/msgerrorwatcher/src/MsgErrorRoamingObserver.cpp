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
*       CMsgErrorRoamingObserver implementation file
*
*/



// INCLUDE FILES
#include    <e32base.h>
#include    <etel.h>
#include    <etelmm.h>
#include    <mmtsy_names.h>

#include    "MsgErrorWatcher.h"
#include    "MsgErrorRoamingObserver.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/

// ---------------------------------------------------------
// CMsgErrorRoamingObserver::CMsgErrorRoamingObserver
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgErrorRoamingObserver::CMsgErrorRoamingObserver( CMsgErrorWatcher* aWatcher )
    : CActive( CActive::EPriorityStandard ),
    iWatcher( aWatcher ),
    iRoaming( EFalse ),
    iRegStatus( RMobilePhone::ERegistrationUnknown ),
    iRequestId( 0 )
    {
    CActiveScheduler::Add( this );
    }

/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/

/*****************************************************
*   Series 60 Customer / TSY
*   Needs customer TSY implementation
*****************************************************/

// ---------------------------------------------------------
// CMsgErrorRoamingObserver::ConstructL()
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgErrorRoamingObserver::ConstructL()
    {

    User::LeaveIfError( iETelServer.Connect() );
    User::LeaveIfError( iETelServer.LoadPhoneModule( KMmTsyModuleName ) );
    User::LeaveIfError( iMobilePhone.Open( iETelServer, KMmTsyPhoneName ) );
    iMobilePhone.GetNetworkRegistrationStatus( iStatus, iRegStatus );
    iRequestId = EMobilePhoneGetNetworkRegistrationStatus;
    SetActive();
    }

// ---------------------------------------------------------
// CMsgErrorRoamingObserver::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgErrorRoamingObserver* CMsgErrorRoamingObserver::NewL( CMsgErrorWatcher* aWatcher )
    {
    CMsgErrorRoamingObserver* self = new ( ELeave )
        CMsgErrorRoamingObserver( aWatcher );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/

/*****************************************************
*   Series 60 Customer / TSY
*   Needs customer TSY implementation
*****************************************************/

// ---------------------------------------------------------
// CMsgErrorRoamingObserver::~CMsgErrorRoamingObserver
//
// Destructor
// ---------------------------------------------------------
//
CMsgErrorRoamingObserver::~CMsgErrorRoamingObserver()
    {
    Cancel();
    iMobilePhone.Close();
    iETelServer.UnloadPhoneModule( KMmTsyModuleName );
    iETelServer.Close();    
    }

/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/

// ---------------------------------------------------------
// CMsgErrorRoamingObserver::UpdateRoamingStatusL
//
// Checks whether roaming status has actually changed
// ---------------------------------------------------------
//
void CMsgErrorRoamingObserver::UpdateRoamingStatusL()
    {
    TBool changed = EFalse;
    if ( iRoaming && iRegStatus == RMobilePhone::ERegisteredOnHomeNetwork )
        {
        iRoaming = EFalse;
        changed = ETrue;
        }
    else if ( !iRoaming && iRegStatus == RMobilePhone::ERegisteredRoaming )
        {
        iRoaming = ETrue;
        changed = ETrue;
        }
    else
        {
        //Nothing to do
        }    
    if ( changed )
        {
        //Inform watcher only when roaming status has changed
        iWatcher->HandleRoamingEventL( iRoaming );
        }
    }

/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/

// ---------------------------------------------------------
// CMsgErrorRoamingObserver::Start
//
// Starts the active object
// ---------------------------------------------------------
//
void CMsgErrorRoamingObserver::Start()
    {
    if ( !IsActive() ) 
        {
        iMobilePhone.NotifyNetworkRegistrationStatusChange( iStatus, iRegStatus );
        iRequestId = EMobilePhoneNotifyNetworkRegistrationStatusChange;
        SetActive();
        }
    }

/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/

// ---------------------------------------------------------
// CMsgErrorRoamingObserver::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorRoamingObserver::DoCancel()
    {
    if ( iRequestId )
        {
        iMobilePhone.CancelAsyncRequest( iRequestId );
        iRequestId = 0;
        }
    }

/*****************************************************
*   Series 60 Customer / ETel
*   Series 60  ETel API
*****************************************************/

// ---------------------------------------------------------
// CMsgErrorRoamingObserver::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgErrorRoamingObserver::RunL()
    {
    iRequestId = 0;
    TInt status = iStatus.Int();
    if ( status < 0 )
        {
        iRegStatus = RMobilePhone::ERegistrationUnknown;
        }
    TRAP_IGNORE( UpdateRoamingStatusL() );
    Start();
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
