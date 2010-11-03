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
* Description:   Methods for Ncn subsystem base class.
*
*/



// Include Files
#include "NcnDebug.h"
#include "NcnModelBase.h"
#include "Ncn.h"
#include "CNcnSession.h"
#include "NcnCommsDefs.h"
#include "MNcnNotificationObserver.h"
#include <defaultcaps.hrh>
#include <e32svr.h>
#include <e32base.h>
#include <e32math.h>
#include <e32uid.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

    // thread name with secure api
    _LIT( KNcnThreadName, "ncnlist" );

    const TUint myRangeCount = 3;
    const TInt myRanges[myRangeCount] = 
        {
        0,                 //range is all s60-internal API requests
        ENewMessages1,     //range is all public API requests
        EMarkUnread1 + 1  //range is non-supported requests
        };
    const TUint8 myElementsIndex[myRangeCount] = 
        {
        0,                              //applies to 0th range
        1,                              //applies to 1st range    
        CPolicyServer::ENotSupported    //applies to 2nd range
        };
    const CPolicyServer::TPolicyElement myElements[] = 
        {
        {
        _INIT_SECURITY_POLICY_V1( VID_DEFAULT, ECapabilityWriteUserData ),
        CPolicyServer::EFailClient
        },
        {
        _INIT_SECURITY_POLICY_C1( ECapabilityWriteUserData ),
        CPolicyServer::EFailClient
        }        
        };
    const CPolicyServer::TPolicy myPolicy =
        {
        1, //connect attempt checks write user data capability
        myRangeCount,                   
        myRanges,
        myElementsIndex,
        myElements
        };

}  // namespace

// ================= MEMBER FUNCTIONS =======================

CNcn::CNcn( TInt aPriority, const TPolicy& aPolicy )
    : CPolicyServer( aPriority, aPolicy )
    {
    }
    
CNcn::~CNcn()
    {
    delete iNcnModel;
    }    

// Create and start a new server.
void CNcn::NewLC()
    {
    NCN_RDEBUG(_L("CREATING NCNLIST"));    
    CNcn* server = new CNcn( CActive::EPriorityHigh, myPolicy );
    CleanupStack::PushL( server );
    server->ConstructL();
    server->StartL( KNcnServerName );
    }

// Symbian OS default constructor can leave.
void CNcn::ConstructL()
    {
    // Create an instance of CNcnModel (which initialises the Ncnlist environment)
    iNcnModel = CNcnModelBase::NewL();    
    }

// Create a new server session.
CSession2* CNcn::NewSessionL(
    const TVersion& aVersion,
    const RMessage2& /*aMessage*/ ) const
    {
    // check we're the right version
    TVersion v( KNcnNotificationServerMajor,
                KNcnNotificationServerMinor,
                KNcnNotificationServerBuild );
    if( !User::QueryVersionSupported( v, aVersion ) )
        User::Leave( KErrNotSupported );
    // make new session
    return CNcnSession::NewL( iNcnModel->NotificationObserver() );
    }

TInt CNcn::RunServer()
    {
    __UHEAP_MARK;
    //
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt ret = KErrNoMemory;
    if( cleanup )
        {
        TRAP( ret, CNcn::RunServerL() );
        delete cleanup;
        }
    //
    __UHEAP_MARKEND;
    if( ret != KErrNone )
        {
        // Signal the client that server creation failed
        RProcess::Rendezvous( ret );
        }
    return ret;
    }

void CNcn::RunServerL()
    {
    // rename thread if secure api is defined
    TInt err = User::RenameThread( KNcnThreadName );
    
    if( err != KErrNone )
        {
        User::Leave( err );
        }
    
    // Create and install the active scheduler we need
    CActiveScheduler *as = new ( ELeave ) CActiveScheduler;
    CleanupStack::PushL( as );
    CActiveScheduler::Install( as );

    // Create server
    CNcn::NewLC();

    // Initialisation complete, now signal the client
    RProcess::Rendezvous( KErrNone );

    // Ready to run
    CActiveScheduler::Start();

    // Cleanup the server and scheduler
    CleanupStack::PopAndDestroy( 2, as );
    }

TInt E32Main()
    {
    TInt error( KErrNone );
    error = CNcn::RunServer();
    return error;
    }

//  End of File
