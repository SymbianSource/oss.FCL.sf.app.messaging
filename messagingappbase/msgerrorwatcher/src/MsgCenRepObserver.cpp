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
* Description:  
*          CMsgCenRepObserver implementation file
*
*/



// INCLUDE FILES
//#include    <es_sock.h>
#include <centralrepository.h>

#include "MsgErrorWatcher.h"
#include "MsgCenRepObserver.h"
 
#include "MmsEngineInternalCRKeys.h"

#ifdef USE_LOGGER
#include "MsgErrorWatcherLogging.h"
#endif
// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------
//
CMsgCenRepObserver::CMsgCenRepObserver(CMsgErrorWatcher* aWatcher)
    :CActive ( EPriorityStandard),
    iWatcher( aWatcher ),
    iRepositoryUid( KCRUidMmsEngine ),
    iKey( KMmsAccesspointArrayBase/*KMmsEngineAccessPointCount*/ )
    {
    }

// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CMsgCenRepObserver::ConstructL()
    {
#ifdef USE_LOGGER
        MEWLOGGER_WRITE( "CMsgCenRepObserver::ConstructL(" );
#endif  

    // Connect CenRep
    TRAPD( err, iRepository = CRepository::NewL( iRepositoryUid ) );
    if( err != KErrNone )
        {
#ifdef USE_LOGGER
        MEWLOGGER_WRITE( "Error in connecting to CenRep!" );
#endif
        return;
        }
    
    CActiveScheduler::Add(this);
        
#ifdef USE_LOGGER
        MEWLOGGER_WRITE( "Connected to CenRep CenRep!" );
#endif    
 
    }

// ---------------------------------------------------------
// Symbian two-phased constructor
// ---------------------------------------------------------
//
CMsgCenRepObserver* CMsgCenRepObserver::NewL( CMsgErrorWatcher* aWatcher )
    {
    CMsgCenRepObserver* self = new ( ELeave ) CMsgCenRepObserver( aWatcher );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CMsgCenRepObserver::~CMsgCenRepObserver()
    {
    Cancel();
    if( iRepository )
        {
        delete iRepository;
        }
    
#ifdef USE_LOGGER
    MEWLOGGER_WRITE( "CenRepObserver destroyed!" );
#endif
    }

// ---------------------------------------------------------
// SubscribeNotification
// ---------------------------------------------------------
//
void CMsgCenRepObserver::SubscribeNotification( )
    {
 #ifdef USE_LOGGER
    MEWLOGGER_WRITE( "CMsgCenRepObserver::SubscribeNotification" );
#endif 
    // Subscribe
    if (iRepository)
        {
        TInt err = iRepository->NotifyRequest(iKey, iStatus);

        if (err == KErrNone)
            {
            SetActive();
            }
        }
    }

// ---------------------------------------------------------
// RunL
// ---------------------------------------------------------
//
void CMsgCenRepObserver::RunL()
    {
 #ifdef USE_LOGGER
    MEWLOGGER_WRITE( "CMsgCenRepObserver::RunL" );
#endif 
        
    iWatcher -> HandleCenRepNotificationL();    
    }

TInt CMsgCenRepObserver::RunError( TInt /*aError*/ )
    {
    //In case RunL leaves
#ifdef USE_LOGGER
    MEWLOGGER_WRITE( "CMsgCenRepObserver::RunError" );
#endif 	
	return KErrNone;	
    }

// ---------------------------------------------------------
// DoCancel
// ---------------------------------------------------------
//
void CMsgCenRepObserver::DoCancel()
    {
 #ifdef USE_LOGGER
    MEWLOGGER_WRITE( "CMsgCenRepObserver::DoCancel" );
#endif 
    iRepository->NotifyCancel( iKey );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

