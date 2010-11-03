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
*     CMsgErrorConnectionObserver implementation file
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <rconnmon.h>

#include "MsgErrorWatcher.h"
#include "MsgErrorConnectionObserver.h"

#ifdef USE_LOGGER
#include "MsgErrorWatcherLogging.h"
#endif

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgErrorConnectionObserver::CMsgErrorConnectionObserver
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgErrorConnectionObserver::CMsgErrorConnectionObserver( CMsgErrorWatcher* aWatcher )
    : iWatcher( aWatcher ),
    iStarted( EFalse )
    {
    }

// ---------------------------------------------------------
// CMsgErrorConnectionObserver::ConstructL
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgErrorConnectionObserver::ConstructL()
    {
    User::LeaveIfError( iConnMon.ConnectL() ); 
    }

// ---------------------------------------------------------
// CMsgErrorConnectionObserver::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgErrorConnectionObserver* CMsgErrorConnectionObserver::NewL( CMsgErrorWatcher* aWatcher )
    {
    CMsgErrorConnectionObserver* self = new ( ELeave )
        CMsgErrorConnectionObserver( aWatcher );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// ---------------------------------------------------------
// CMsgErrorConnectionObserver::~CMsgErrorConnectionObserver
//
// Destructor
// ---------------------------------------------------------
//
CMsgErrorConnectionObserver::~CMsgErrorConnectionObserver()
    {
#ifdef USE_LOGGER
    MEWLOGGER_ENTERFN( "CMsgErrorConnectionObserver destructor" );
#endif
    iConnMon.CancelNotifications();
    iConnMon.Close();
#ifdef USE_LOGGER
    MEWLOGGER_LEAVEFN( "CMsgErrorConnectionObserver destructor" );
#endif
    }

// ---------------------------------------------------------
// CMsgErrorConnectionObserver::ConnectionsOpen
// ---------------------------------------------------------
//
TUint CMsgErrorConnectionObserver::ConnectionsOpen()
    {
    // Get number of active (packet data) connections
#ifdef USE_LOGGER
    MEWLOGGER_ENTERFN( "CMsgErrorConnectionObserver::ConnectionsOpen" );
#endif
    TUint conns( 0 );
    TUint subConns( 0 );
    TUint packetDataConns( 0 );
    TUint connId( 0 );    
    TInt bearer( 0 );

    TRequestStatus status;
    iConnMon.GetConnectionCount( conns, status );
    User::WaitForRequest( status );
#ifdef USE_LOGGER
    MEWLOGGER_WRITEF( _L("Connections open: %d"), conns );
#endif

    while ( conns-- )
        {
        // Connection indexing starts from 1
        // --> Add one to index
        iConnMon.GetConnectionInfo( conns + 1, connId, subConns );
        iConnMon.GetIntAttribute( connId, 0, KBearer, bearer, status );
        User::WaitForRequest( status );
        switch ( bearer )
            {
            case EBearerGPRS:
            case EBearerEdgeGPRS:
            case EBearerExternalGPRS:
            case EBearerExternalEdgeGPRS:
            case EBearerWCDMA:
            case EBearerExternalWCDMA:
//#ifdef __WINS__
//            case EBearerLAN:
//#endif // WINS
                {
                packetDataConns++;
                break;
                }
            default :
                {
                break;
                }                
            }
        }
#ifdef USE_LOGGER
    MEWLOGGER_WRITEF( _L("Packet data connections open: %d"), packetDataConns );
#endif
    return packetDataConns;
    }


// ---------------------------------------------------------
// CMsgErrorConnectionObserver::Start
// ---------------------------------------------------------
//
void CMsgErrorConnectionObserver::StartL()
    {
    if ( !iStarted )
        {
        iConnectionsOpenWhenStarted = ConnectionsOpen();
        iConnMon.NotifyEventL( *this );
        iStarted = ETrue;
        }
    }


// ---------------------------------------------------------
// CMsgErrorConnectionObserver::EventL
//
// From MConnectionMonitorObserver
// ---------------------------------------------------------
//
void CMsgErrorConnectionObserver::EventL( const CConnMonEventBase& aConnMonEvent )
    {
#ifdef USE_LOGGER
    MEWLOGGER_ENTERFN( "CMsgErrorConnectionObserver::EventL" );
    MEWLOGGER_WRITEF( _L("EventType: %d"), aConnMonEvent.EventType() );
#endif
    switch( aConnMonEvent.EventType() )
        {
        case EConnMonDeleteConnection:
            {
            if ( ConnectionsOpen() < iConnectionsOpenWhenStarted )
                {
                iWatcher->HandleConnectionEvent();
                }
            //const CConnMonDeleteConnection *delEvent =
            //    static_cast<const CConnMonDeleteConnection*>( &aConnMonEvent );
            break;
            }
        default:
            break;
        }
#ifdef USE_LOGGER
    MEWLOGGER_LEAVEFN( "CMsgErrorConnectionObserver::EventL" );
#endif
    }


// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
