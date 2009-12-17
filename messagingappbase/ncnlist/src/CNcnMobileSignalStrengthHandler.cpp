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
* Description:   Mobile signal strength handler implementation
*
*/



// INCLUDE FILES
#include <mmtsy_names.h>
#include "CNcnMobileSignalStrengthHandler.h"
#include "MNcnSignalStrengthObserver.h"
#include "NcnDebug.h"

// -----------------------------------------------------------------
// CNcnMobileSignalStrengthHandler::NewL
// -----------------------------------------------------------------
CNcnMobileSignalStrengthHandler* CNcnMobileSignalStrengthHandler::NewL( )
    {
    CNcnMobileSignalStrengthHandler* self = new (ELeave) CNcnMobileSignalStrengthHandler();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------
// CNcnMobileSignalStrengthHandler::~CNcnMobileSignalStrengthHandler
// -----------------------------------------------------------------
CNcnMobileSignalStrengthHandler::~CNcnMobileSignalStrengthHandler()
    {
    Cancel();
    
    // close session handles
    iMobilePhone.Close();
    iTelServer.Close();    
    }

// -----------------------------------------------------------------
// CNcnMobileSignalStrengthHandler::CNcnMobileSignalStrengthHandler
// -----------------------------------------------------------------
CNcnMobileSignalStrengthHandler::CNcnMobileSignalStrengthHandler() :
    CActive( EPriorityNormal )
    {
    // empty
    }

// -----------------------------------------------------------------
// CNcnMobileSignalStrengthHandler::ConstructL
// -----------------------------------------------------------------
void CNcnMobileSignalStrengthHandler::ConstructL()
    {
    // add AO to scheduler
    CActiveScheduler::Add( this );
    
    // initialize sessions
    InitializeSessionsL();
    }

// -----------------------------------------------------------------
// CNcnMobileSignalStrengthHandler::RunL
// -----------------------------------------------------------------
void CNcnMobileSignalStrengthHandler::RunL()
    {
    NCN_RDEBUG( _L("CNcnMobileSignalStrengthHandler::RunL") );

    // if operation completed successfully
    if( iStatus.Int() == KErrNone )
        {
        NCN_RDEBUG_INT( _L("CNcnMobileSignalStrengthHandler::RunL - Signal strength updated to %d"),
            iSignalStrength );
        
        NotifyObserver();
        }
        
    // if observing is still active reissue the notification request    
    if( iObserving )
        {
        NCN_RDEBUG( _L("CNcnMobileSignalStrengthHandler::RunL - NotifySignalStrengthChange") );
       // request
        iMobilePhone.NotifySignalStrengthChange( iStatus, iSignalStrength, iBars );
    
        // activate
        SetActive();
        }
    }

// -----------------------------------------------------------------
// CNcnMobileSignalStrengthHandler::RunError
// -----------------------------------------------------------------
TInt CNcnMobileSignalStrengthHandler::RunError( TInt aError )
    {
    return aError;
    }

// -----------------------------------------------------------------
// CNcnMobileSignalStrengthHandler::DoCancel
// -----------------------------------------------------------------
void CNcnMobileSignalStrengthHandler::DoCancel()
    {
    // cancel notification request
    // if we're observing
    if( iObserving )
        {
        iMobilePhone.CancelAsyncRequest( EMobilePhoneNotifySignalStrengthChange );
        }    
    }

// -----------------------------------------------------------------
// CNcnMobileSignalStrengthHandler::ObservingSupported
// -----------------------------------------------------------------
TBool CNcnMobileSignalStrengthHandler::ObservingSupported()
    {
    TUint32 caps;
    iMobilePhone.GetSignalCaps( caps );
    
    return ( caps & RMobilePhone::KCapsNotifySignalStrengthChange );
    }

// -----------------------------------------------------------------
// CNcnMobileSignalStrengthHandler::RequestsSupported
// -----------------------------------------------------------------    
TBool CNcnMobileSignalStrengthHandler::RequestsSupported()
    {
    TUint32 caps;
    iMobilePhone.GetSignalCaps( caps );
    
    return ( caps & RMobilePhone::KCapsGetSignalStrength );
    }

// -----------------------------------------------------------------
// CNcnMobileSignalStrengthHandler::SetSignalStrengthObserverL
// -----------------------------------------------------------------
void CNcnMobileSignalStrengthHandler::SetSignalStrengthObserverL(
    MNcnSignalStrengthObserver* aObserver )
    {
    // leave if observing is not supported
    if( !ObservingSupported() )
        {
        User::Leave( KErrNotSupported );
        }
        
    iObserver = aObserver;
    }

// -----------------------------------------------------------------
// CNcnMobileSignalStrengthHandler::StartObservingL
// -----------------------------------------------------------------
void CNcnMobileSignalStrengthHandler::StartObservingL()
    {
    // leave if observing is not supported
    if( !ObservingSupported() )
        {
        User::Leave( KErrNotSupported );
        }        
    
    // update signal str
    TRAPD( err, UpdateSignalStrengthL() );
    
    // check operation
    if( err != KErrNone )
        {        
        NCN_RDEBUG_INT( _L("CNcnMobileSignalStrengthHandler::StartObservingL - Failed to initialize signal strength (err = %d)"),
            err );
        iSignalStrength = 0;
        }
    else
        {
        NCN_RDEBUG_INT( _L("CNcnMobileSignalStrengthHandler::StartObservingL - Signal strength initialized to %d"),
            iSignalStrength );
        }
    
    // update state
    iObserving = ETrue;
    
    // Notify current observer about signal strength
    NotifyObserver();
    
    // request for notification    
    iMobilePhone.NotifySignalStrengthChange( iStatus, iSignalStrength, iBars );
    
    // activate
    SetActive();                            
    }

// -----------------------------------------------------------------
// CNcnMobileSignalStrengthHandler::StopObservingL
// -----------------------------------------------------------------
void CNcnMobileSignalStrengthHandler::StopObservingL()
    {
    if( !iObserving )
        {
        User::Leave( KErrGeneral );
        }
        
    // cancel the request    
    Cancel();
        
    // update state
    iObserving = EFalse;
    }

// -----------------------------------------------------------------
// CNcnMobileSignalStrengthHandler::SignalStrengthL
// -----------------------------------------------------------------
TInt CNcnMobileSignalStrengthHandler::SignalStrengthL()
    {
    // leave if requests are not supported
    if( !RequestsSupported() )
        {
        User::Leave( KErrNotSupported );
        }
        
    // if we're not observing we have to update the value manually,
    // otherwise value is up-to-date    
    if( !iObserving )
        {
        UpdateSignalStrengthL();
        }
    
    // return the current value
    return iSignalStrength;        
    }
    
// -----------------------------------------------------------------
// CNcnMobileSignalStrengthHandler::BarValueL
// -----------------------------------------------------------------
TInt CNcnMobileSignalStrengthHandler::BarValueL()
    {
    // leave if requests are not supported
    if( !RequestsSupported() )
        {
        User::Leave( KErrNotSupported );
        }
        
    // if we're not observing we have to update the value manually,
    // otherwise value is up-to-date    
    if( !iObserving )
        {
        UpdateSignalStrengthL();
        }
    
    // return the current value
    return iBars;      
    }
    
// -----------------------------------------------------------------
// CNcnMobileSignalStrengthHandler::InitializeSessionsL
// -----------------------------------------------------------------    
void CNcnMobileSignalStrengthHandler::InitializeSessionsL()
    {
    // connect to tel server
    User::LeaveIfError( iTelServer.Connect() );
    
    RTelServer::TPhoneInfo phoneInfo;
    TInt numPhone;
    
    // load tsy module and get number of phones
    User::LeaveIfError( iTelServer.LoadPhoneModule( KMmTsyModuleName ) );
    User::LeaveIfError( iTelServer.EnumeratePhones( numPhone) );

    // find the correct phone
    for (TInt i(0); i < numPhone; i++)
        {
        TName tsyName;
        User::LeaveIfError( iTelServer.GetPhoneInfo( i, phoneInfo ) );
        User::LeaveIfError( iTelServer.GetTsyName( i,tsyName ) );

        if ( tsyName.CompareF( KMmTsyModuleName ) == 0)
            {
            break;
            }
        }
    
    // open mobile phone session    
    User::LeaveIfError( iMobilePhone.Open( iTelServer, phoneInfo.iName ) );        
    }

// -----------------------------------------------------------------
// CNcnMobileSignalStrengthHandler::UpdateSignalStrengthL
// -----------------------------------------------------------------
void CNcnMobileSignalStrengthHandler::UpdateSignalStrengthL()
    {
    // only update the value if requests are supported
    if( RequestsSupported() )
        {
        TRequestStatus status;                
        
        // get signal strength
        iMobilePhone.GetSignalStrength( status, iSignalStrength, iBars );
        
        // Wait for request since we need to be able to supply
        // the signal strength from syncrhonous access method (we
        // have to wait somewhere anyway)
        User::WaitForRequest( status );
        
        NCN_RDEBUG_INT( _L("CNcnMobileSignalStrengthHandler::UpdateSignalStrengthL - GetSignalStrength returned with %d"),
            status.Int() );
        
        // leave if error was encountered
        User::LeaveIfError( status.Int() );
        }
    }

// -----------------------------------------------------------------
// CNcnMobileSignalStrengthHandler::NotifyObserver
// -----------------------------------------------------------------    
void CNcnMobileSignalStrengthHandler::NotifyObserver()
    {
    // notify observer, if set
    if( iObserver )
        {
        TRAP_IGNORE( iObserver->SignalStrengthAndBarUpdatedL( iSignalStrength, iBars ) );
        }
    }

//  End of File
