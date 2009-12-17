/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       CMsgGarbageCollection implementation file
*
*/



// INCLUDE FILES
#include <watcher.h>
#include <msvids.h>
#include <mtclreg.h>
#include <mtclbase.h>
#include <SendUiConsts.h>
#include <PushMtmCommands.hrh>    // EPushMtmCmdCollectGarbage

#include "MsgGarbageCollection.h"

#ifdef USE_LOGGER
#include "MsgErrorWatcherLogging.h"
#endif

// CONSTANTS
const TUint KGarbageCollectionInterval = 1; // In hours

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgGarbageCollection::NewL
// ---------------------------------------------------------
//
CMsgGarbageCollection* CMsgGarbageCollection::NewL( 
        CClientMtmRegistry& aMtmRegistry )
    {
    CMsgGarbageCollection* self = new ( ELeave ) CMsgGarbageCollection( aMtmRegistry );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMsgGarbageCollection::CMsgGarbageCollection
// ---------------------------------------------------------
//
CMsgGarbageCollection::CMsgGarbageCollection( CClientMtmRegistry& aMtmRegistry )
    : CActive( EPriorityStandard ),
    iMtmRegistry( aMtmRegistry )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMsgGarbageCollection::~CMsgGarbageCollection
// ---------------------------------------------------------
//
CMsgGarbageCollection::~CMsgGarbageCollection()
    {
    Cancel();
    iTimer.Close();
    delete iOp;
    delete iWapMtm;
    }

// ---------------------------------------------------------
// CMsgGarbageCollection::ConstructL
// ---------------------------------------------------------
//
void CMsgGarbageCollection::ConstructL()
    {
#ifdef USE_LOGGER
    MEWLOGGER_ENTERFN( "GarbageCollection: ConstructL" );
#endif
    User::LeaveIfError( iTimer.CreateLocal() );
    iWapMtm = iMtmRegistry.NewMtmL( KSenduiMtmPushMtmUid );

    StartWaiting();

#ifdef USE_LOGGER
    MEWLOGGER_LEAVEFN( "GarbageCollection: ConstructL" );
#endif
    }

// ---------------------------------------------------------
// CMsgGarbageCollection::RunL
// ---------------------------------------------------------
//
void CMsgGarbageCollection::RunL()
    {
#ifdef USE_LOGGER
    MEWLOGGER_ENTERFN( "GarbageCollection: RunL" );
#endif
    if ( iState == EGarbageCollectionWaiting )
        {
        WapPushGarbageCollectionL();
        }
    else
        {
        StartWaiting();
        }
#ifdef USE_LOGGER
    MEWLOGGER_LEAVEFN( "GarbageCollection: RunL" );
#endif
    }

// ---------------------------------------------------------
// CMsgGarbageCollection::DoCancel
// ---------------------------------------------------------
//
void CMsgGarbageCollection::DoCancel()
    {
    iTimer.Cancel();
    if ( iOp )
        {
        iOp->Cancel();
        }
    }

// ---------------------------------------------------------
// CMsgGarbageCollection::StartWaiting
// ---------------------------------------------------------
//
void CMsgGarbageCollection::StartWaiting()
    {
#ifdef USE_LOGGER
    MEWLOGGER_ENTERFN( "GarbageCollection: StartWaiting" );
#endif
    iState = EGarbageCollectionWaiting;
    iStatus = KRequestPending;
    TTime time;
    time.UniversalTime();
    time += TTimeIntervalHours( KGarbageCollectionInterval );
    iTimer.AtUTC( iStatus, time );
    SetActive();
#ifdef USE_LOGGER
    MEWLOGGER_LEAVEFN( "GarbageCollection: StartWaiting" );
#endif
    }

// ---------------------------------------------------------
// CMsgGarbageCollection::WapPushGarbageCollectionL
// ---------------------------------------------------------
//
void CMsgGarbageCollection::WapPushGarbageCollectionL()
    {
#ifdef USE_LOGGER
    MEWLOGGER_ENTERFN( "GarbageCollection: WapPushGarbageCollectionL" );
#endif
    // Delete just in case...
    iState = EGarbageCollectionWapPush;
    CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection();
    CleanupStack::PushL( selection );
    TBuf8<1> blankParams;
    iStatus = KRequestPending;
    delete iOp;
    iOp = NULL;
    iOp = iWapMtm->InvokeAsyncFunctionL(
        EPushMtmCmdCollectGarbage,
        *selection,
        blankParams,
        iStatus );
    CleanupStack::PopAndDestroy( selection );
    SetActive();
#ifdef USE_LOGGER
    MEWLOGGER_LEAVEFN( "GarbageCollection: WapPushGarbageCollectionL" );
#endif
    }

//  End of File  

