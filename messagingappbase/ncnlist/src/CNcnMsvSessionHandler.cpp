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
* Description:   Methods for CNcnMsvSessionHandler class.
*
*/



// INCLUDE FILES
#include    "NcnDebug.h"
#include    "CNcnMsvSessionHandler.h"
#include    "MNcnMsvSessionHandlerObserver.h"
#include    <msvapi.h>                          // For CMsvSession

// ================= LOCAL CONSTANTS =======================
namespace
    {
    }

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
//  CNcnMsvSessionHandler::CNcnMsvSessionHandler
// ----------------------------------------------------
//
CNcnMsvSessionHandler::CNcnMsvSessionHandler( ) :
    iObservers( 2 )
    {
    // empty
    }

// ----------------------------------------------------
//  CNcnMsvSessionHandler::ConstructL
// ----------------------------------------------------
//
void CNcnMsvSessionHandler::ConstructL()
    {
    // empty
    }
    
// ----------------------------------------------------
//  CNcnMsvSessionHandler::NewL
// ----------------------------------------------------
//
CNcnMsvSessionHandler* CNcnMsvSessionHandler::NewL( )
    {
    CNcnMsvSessionHandler* self = new (ELeave) CNcnMsvSessionHandler();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ----------------------------------------------------
//  CNcnMsvSessionHandler::~CNcnMsvSessionHandler
// ----------------------------------------------------
//
CNcnMsvSessionHandler::~CNcnMsvSessionHandler()
    {
    // stop sessino handler
    StopSessionHandler();
    }

// ----------------------------------------------------
//  CNcnMsvSessionHandler::HandleSessionEventL
// ----------------------------------------------------
//
void CNcnMsvSessionHandler::HandleSessionEventL(
            TMsvSessionEvent aEvent,
            TAny* aArg1,
            TAny* /*aArg2*/,
            TAny* /*aArg3*/ )
    {
    // determine event type
    switch ( aEvent )
        {

        case EMsvMediaUnavailable:
            {
            // If the Unavailable Memory is External Memory then set it back to Phone Memory .
            NCN_RDEBUG( _L( "CNcnMsvSessionHandler::HandleSessionEventL - EMsvMediaUnavailable." ));
           	HandleMsvMediaUnAvailableL();
            break;
            }

    	case EMsvServerReady:
            {
            // Ignored. Session was initialized already in StartSessionHandlerL .
            // If message store medium changes, then "old" messages are deleted from the CMsvEntry index
            // and the index is kept valid without re-creating it.
            NCN_RDEBUG( _L( "CNcnMsvSessionHandler::HandleSessionEventL - EMsvSessionReady, ignored" ));
            NotifyMsvSessionReady();
            break;
            }
        // Msv server terminated
        case EMsvServerTerminated:
            {            
            HandleMsvTerminatedL();
            break;
            }
        // Msv session close event
        case EMsvCloseSession:
            {
            HandleMsvClosed();
            break;
            }
        // Entries created
        case EMsvEntriesCreated:
            {
            CMsvEntrySelection* selection =
                static_cast<CMsvEntrySelection*>( aArg1 );
                
            HandleMsvEntriesCreatedL( *selection );
            break;
            }
        // Entries deleted
        case EMsvEntriesDeleted:
            {
            CMsvEntrySelection* selection =
                static_cast<CMsvEntrySelection*>( aArg1 );
                
            HandleMsvEntriesDeletedL( *selection );
            break;
            }
        // Messages are moved to MMC
        case EMsvMediaChanged:
            {
            TDriveNumber* newDriveNumber = static_cast<TDriveNumber*>( aArg1 );
            HandleMsvMediaChangedL( *newDriveNumber );
            break;
            }
 		// Message server is again available    
        case EMsvMediaAvailable:
        	{
        	TDriveNumber* driveNumber = static_cast<TDriveNumber*>( aArg1 );
            HandleMsvMediaAvailableL( *driveNumber );
            break;            		
        	}
        // other events, ignore
        default:
            {
           	NCN_RDEBUG_INT( _L( "CNcnMsvSessionHandler::HandleSessionEventL - unknown event %d!" ), aEvent );
            break;
            }
        }
    }
    
// ----------------------------------------------------
//  CNcnMsvSessionHandler::AddObserver
// ----------------------------------------------------
//
void CNcnMsvSessionHandler::AddObserverL(
    MNcnMsvSessionHandlerObserver* aObserver )
    {
    // append observer to array
    iObservers.AppendL( aObserver );    
    }

// ----------------------------------------------------
//  CNcnMsvSessionHandler::RemoveObserver
// ----------------------------------------------------
//
void CNcnMsvSessionHandler::RemoveObserver(
    MNcnMsvSessionHandlerObserver* aObserver )
    {
    TInt count = iObservers.Count();
    TInt index = -1;
    
    // search for observer
    while( count-- )
        {
        MNcnMsvSessionHandlerObserver* observer =
            iObservers.At( count );
            
        // if pointer found    
        if( observer == aObserver )
            {
            index = count;
            break;
            }
        }
    
    // if observer was found delete it    
    if( index != -1 )
        {
        iObservers.Delete( index );
        iObservers.Compress();
        }
    }

// ----------------------------------------------------
//  CNcnMsvSessionHandler::IsMsvSessionReady
// ----------------------------------------------------
//
TBool CNcnMsvSessionHandler::IsMsvSessionReady() const
    {
    return ( iMsvSession != 0 );
    }

// ----------------------------------------------------
//  CNcnMsvSessionHandler::MsvSessionL
// ----------------------------------------------------
//
CMsvSession& CNcnMsvSessionHandler::MsvSessionL()
    {
    // leave if session is not ready
    if( !IsMsvSessionReady() )
        {
        User::Leave( KErrGeneral );
        }
        
    // return the session instance    
    return *iMsvSession;
    }

// ----------------------------------------------------
//  CNcnMsvSessionHandler::StartSessionHandlerL
// ----------------------------------------------------
//        
void CNcnMsvSessionHandler::StartSessionHandlerL()
    {
    if( !iMsvSession )
        {
        // open msv session
        iMsvSession = CMsvSession::OpenSyncL( *this );
        
        // notity that msv session is ready
        NotifyMsvSessionReady();
        }
    }
        

// ----------------------------------------------------
//  CNcnMsvSessionHandler::StopSessionHandler
// ----------------------------------------------------
//    
void CNcnMsvSessionHandler::StopSessionHandler()
    {
    // just delegate to close handler
    HandleMsvClosed();
    }

// ----------------------------------------------------
//  CNcnMsvSessionHandler::HandleMsvTerminatedL
// ----------------------------------------------------
//    
void CNcnMsvSessionHandler::HandleMsvTerminatedL()
    {
    // handle msv close normally
    HandleMsvClosed();
    
    // re-start session handler
    StartSessionHandlerL();
    }
    
// ----------------------------------------------------
//  CNcnMsvSessionHandler::HandleMsvClosed
// ----------------------------------------------------
//
void CNcnMsvSessionHandler::HandleMsvClosed()
    {
    // only handle close if session is ready
    if( iMsvSession )
        {
        // set temporary pointer
        CMsvSession* tmpMsvSession = iMsvSession;
        
        // clear instance variable since it's used
        // for tracking instance state
        iMsvSession = 0;        
        
        // notify observers that session was closed
        NotifyMsvSessionClosed();
        
        // finally delete the session
        delete tmpMsvSession;
        }    
    }

// ----------------------------------------------------
//  CNcnMsvSessionHandler::HandleEntriesCreatedL
// ----------------------------------------------------
//    
void CNcnMsvSessionHandler::HandleMsvEntriesCreatedL(
    const CMsvEntrySelection& aSelection )
    {
    TInt count = aSelection.Count();
    
    while( count-- )
        {
        TMsvId id = aSelection.At( count );
        NotifyMsvEntryCreated( id );
        }
    }
    
// ----------------------------------------------------
//  CNcnMsvSessionHandler::HandleEntriesDeletedL
// ----------------------------------------------------
//    
void CNcnMsvSessionHandler::HandleMsvEntriesDeletedL(
    const CMsvEntrySelection& aSelection )
    {
    TInt count = aSelection.Count();
    
    while( count-- )
        {
        TMsvId id = aSelection.At( count );
        NotifyMsvEntryDeleted( id );
        }
    }

// ----------------------------------------------------
//  CNcnMsvSessionHandler::NotifyMsvSessionReady
// ----------------------------------------------------
//    
void CNcnMsvSessionHandler::NotifyMsvSessionReady()
    {
    TInt count = iObservers.Count();
    
    // process all observers
    while( count-- )
        {        
        MNcnMsvSessionHandlerObserver* observer =
            iObservers.At( count );
        
        TRAP_IGNORE( observer->HandleMsvSessionReadyL( *iMsvSession ) );
        }
    }
  
// ----------------------------------------------------
//  CNcnMsvSessionHandler::NotifyMsvSessionClosed
// ----------------------------------------------------
//        
void CNcnMsvSessionHandler::NotifyMsvSessionClosed()
    {
    TInt count = iObservers.Count();
    
    // process all observers
    while( count-- )
        {        
        MNcnMsvSessionHandlerObserver* observer =
            iObservers.At( count );
        
        TRAP_IGNORE( observer->HandleMsvSessionClosedL() );
        }
    }

// ----------------------------------------------------
//  CNcnMsvSessionHandler::NotifyMsvEntryCreated
// ----------------------------------------------------
//    
void CNcnMsvSessionHandler::NotifyMsvEntryCreated( const TMsvId& aMsvId )
    {
    TInt count = iObservers.Count();
    
    // process all observers
    while( count-- )
        {        
        MNcnMsvSessionHandlerObserver* observer =
            iObservers.At( count );
        
        TRAP_IGNORE( observer->HandleMsvEntryCreatedL( aMsvId ) );
        }
    }
  
// ----------------------------------------------------
//  CNcnMsvSessionHandler::NotifyMsvSessionClosed
// ----------------------------------------------------
//        
void CNcnMsvSessionHandler::NotifyMsvEntryDeleted( const TMsvId& aMsvId )
    {
    TInt count = iObservers.Count();
    
    // process all observers
    while( count-- )
        {        
        MNcnMsvSessionHandlerObserver* observer =
            iObservers.At( count );
        
        TRAP_IGNORE( observer->HandleMsvEntryDeletedL( aMsvId ) );
        }
    }
    
// ----------------------------------------------------
//  CNcnMsvSessionHandler::HandleMsvMediaChangedL
// ----------------------------------------------------
//  
void CNcnMsvSessionHandler::HandleMsvMediaChangedL( const TDriveNumber& aDriveNumber )
	{
    TInt count = iObservers.Count();
    
    // process all observers
    while( count-- )
        {        
        MNcnMsvSessionHandlerObserver* observer =
            iObservers.At( count );
        
        TRAP_IGNORE( observer->HandleMsvMediaChangedL( aDriveNumber ) );
        }		
	}
	
// ----------------------------------------------------
//  CNcnMsvSessionHandler::HandleMsvMediaAvailableL
// ----------------------------------------------------
//  
void CNcnMsvSessionHandler::HandleMsvMediaAvailableL( const TDriveNumber& aDriveNumber )
	{
    TInt count = iObservers.Count();
    
    // process all observers
    while( count-- )
        {        
        MNcnMsvSessionHandlerObserver* observer =
            iObservers.At( count );
        
        TRAP_IGNORE( observer->HandleMsvMediaAvailableL( aDriveNumber ) );
        }		
	}
	
// ----------------------------------------------------
//  CNcnMsvSessionHandler::HandleMsvMediaUnAvailableL
//  Call the corresponding observer to handle MediaUnAvailable event
// ----------------------------------------------------
//  
void CNcnMsvSessionHandler::HandleMsvMediaUnAvailableL( )
	{
    TInt count = iObservers.Count();
    
    // process all observers
    while( count != 0 )
        {     
        count--;   
        MNcnMsvSessionHandlerObserver* observer =
            iObservers.At( count );
        
        TRAP_IGNORE( observer->HandleMsvMediaUnavailableL() );
        }		
	}
//  End of File
