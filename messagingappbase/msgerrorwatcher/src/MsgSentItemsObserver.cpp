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
*       CMsgSentItemsObserver implementation file
*
*/



// INCLUDE FILES
#include <watcher.h>
#include <msvids.h>

#include <centralrepository.h>    // link against centralrepository.lib
#include <cenrepnotifyhandler.h>  // link against CenRepNotifHandler.lib
#include <messaginginternalcrkeys.h> // for Central Repository keys

#include "MsgSentItemsObserver.h"

#ifdef USE_LOGGER
#include "MsgErrorWatcherLogging.h"
#endif

// CONSTANTS
//const TUid KUidMceApp = { 0x100058C5 }; //needed to request shared data notifications
const TMsvId KWatcherSentFolderId = KMsvSentEntryIdValue;
const TUint KMaxRetries = 3;
const TInt KMaxNumOfItems = 20; //Deafult value in case of error reading CommsDb
//const TInt KMsgErrorDiskSpaceForDelete = ( 5 * 1024 ); // minimum disk space needed when deleting messages
//_LIT( KSentItemsInUseOff, "0" );


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgSentItemsObserver::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgSentItemsObserver* CMsgSentItemsObserver::NewL( 
        CMsgErrorWatcher* aWatcher, 
        CMsvSession* aSession )
    {
    CMsgSentItemsObserver* self = new ( ELeave )
        CMsgSentItemsObserver( aWatcher, aSession );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMsgSentItemsObserver::CMsgSentItemsObserver
//
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgSentItemsObserver::CMsgSentItemsObserver( CMsgErrorWatcher* aWatcher, CMsvSession* aSession )
    : CActive( EPriorityStandard ),
    iWatcher( aWatcher ),
    iSession( aSession )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMsgSentItemsObserver::~CMsgSentItemsObserver
//
// Destructor
// ---------------------------------------------------------
//
CMsgSentItemsObserver::~CMsgSentItemsObserver()
    {
    Cancel();
    if ( iNotifyHandler )
        {        
        iNotifyHandler->StopListening();
        }
    delete iNotifyHandler;
    delete iRepository;
    if( iCleanupFlags & ESentItemsFolderObserverAdded )
        {
        iSentItemsFolder->RemoveObserver( *this );
        }
    delete iSentItemsFolder;
    delete iEntry;
    delete iOp;
    }

// ---------------------------------------------------------
// CMsgSentItemsObserver::ConstructL
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgSentItemsObserver::ConstructL()
    {
#ifdef USE_LOGGER
    MEWLOGGER_ENTERFN( "SentItems: ConstructL" );
#endif
    //iMsgStoreDrive = MessageServer::CurrentDriveL( iSession->FileSession() );
    //register as observer of sent items folder
    TMsvSelectionOrdering sort( KMsvNoGrouping, EMsvSortByDateReverse );
    iSentItemsFolder = CMsvEntry::NewL( *iSession, KWatcherSentFolderId, sort );
    iSentItemsFolder->AddObserverL( *this );
    iCleanupFlags |= ESentItemsFolderObserverAdded;
    
    //register as observer of user settings for sent folder

    // Central Repository
    iRepository = CRepository::NewL( KCRUidMuiuSettings );
    iNotifyHandler = CCenRepNotifyHandler::NewL( *this, *iRepository );
    iNotifyHandler->StartListeningL();

#ifdef USE_LOGGER
    MEWLOGGER_LEAVEFN( "SentItems: ConstructL" );
#endif
    }

// ---------------------------------------------------------
// CMsgSentItemsObserver::RunL
//
// Will only run when messages have been deleted
// ---------------------------------------------------------
//
void CMsgSentItemsObserver::RunL()
    {
    //iSession->FileSession().ReleaseReserveAccess( iMsgStoreDrive );
#ifdef USE_LOGGER
    if( iStatus==KErrNone )
        {
        MEWLOGGER_WRITE( "SentItems: deletion successful" );
        }
    else
        {
        MEWLOGGER_WRITEF( _L("SentItems: deletion error %d"), iStatus.Int() );
        } 
#endif
    //Check if new messages have appeared into Sent Items while deleting
    //the previous one(s).
#ifdef USE_LOGGER
    MEWLOGGER_WRITEF( _L("SentItems: Retries: %d"), iRetryCounter );
#endif
    if ( iRetryCounter < KMaxRetries )
        {
        iRetryCounter++;
        TRAP_IGNORE( DeleteOldMessagesFromSentFolderL() );
        }
    }

// ---------------------------------------------------------
// CMsgSentItemsObserver::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgSentItemsObserver::DoCancel()
    {
    //iSession->FileSession().ReleaseReserveAccess( iMsgStoreDrive );
    if ( iOp )
        {
        iOp->Cancel();
        }
    }

// ---------------------------------------------------------
// CMsgSentItemsObserver::HandleNotifyInt
//
// Notification from central repository
// ---------------------------------------------------------
//
void CMsgSentItemsObserver::HandleNotifyInt( TUint32 aId, TInt aNewValue )
    {    
    if ( ( aId == KMuiuSentItemsInUse && aNewValue != 0 ) ||
         ( aId == KMuiuSentItemsCount && SentFolderIsBeingUsed() ) )
        {
        iRetryCounter = 0;
        //This is non-leaving function...
        TRAPD( dummy, DeleteOldMessagesFromSentFolderL() );
        dummy=dummy; //Prevent arm5 build warning
        }
    }

// ---------------------------------------------------------
// CMsgSentItemsObserver::HandleNotifyGeneric
// ---------------------------------------------------------
//
void CMsgSentItemsObserver::HandleNotifyGeneric( TUint32 /*aId*/ )
    {
    //Nothing.
    }
    
// ---------------------------------------------------------
// CMsgSentItemsObserver::HandleNotifyError
// ---------------------------------------------------------
//
void CMsgSentItemsObserver::HandleNotifyError( TUint32 /*aId*/, TInt /*error*/, CCenRepNotifyHandler* /*aHandler*/ )
    {
    //Nothing.
    }
  
// ---------------------------------------------------------
// CMsgSentItemsObserver::HandleEntryEventL
//
// Call back from msg that the sent items folder has changed
// so check if messages need to be deleted.
// ---------------------------------------------------------
//
void CMsgSentItemsObserver::HandleEntryEventL( TMsvEntryEvent aEvent, TAny* aArg1, TAny* /*aArg2*/, TAny* /*aArg3*/ )
    {   
    switch( aEvent )
        {
        case EMsvNewChildren:
            {
            if( !SentFolderIsBeingUsed() )
                {
                //not taking ownership
                CMsvEntrySelection* selection =
                    static_cast<CMsvEntrySelection*>( aArg1 );
                iRetryCounter = 0;
                DeleteMessagesFromSentFolderL( selection );
                return;
                }
            if( iSentItemsFolder->Count() > SentFolderUpperLimit() )
                {
                iRetryCounter = 0;
                DeleteOldMessagesFromSentFolderL();
                return;
                }
            }
            break;
        case EMsvEntryChanged:
        case EMsvDeletedChildren:
        case EMsvChildrenChanged:
        case EMsvEntryDeleted:
        case EMsvContextInvalid:
        case EMsvChildrenMissing:
        case EMsvChildrenInvalid:
        case EMsvEntryMoved:
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CMsgSentItemsObserver::SentFolderIsBeingUsed
//
// Returns the user setting for use of the sent folder. If there
// is an error then the default value of ON (i.e ETrue) will
// be returned.
// ---------------------------------------------------------
//
TBool CMsgSentItemsObserver::SentFolderIsBeingUsed()
    {
    TInt isUsed = 1;
   	iRepository->Get( KMuiuSentItemsInUse, isUsed );
    return isUsed;
    }

// ---------------------------------------------------------
// CMsgSentItemsObserver::SentFolderUpperLimit
//
// Returns the user setting for max limit to the number of messages
// in the sent folder. If there is an error then the default value of
// KMaxNumOfItems will be returned
// ---------------------------------------------------------
//
TInt CMsgSentItemsObserver::SentFolderUpperLimit()
    {
    TInt numItems = KMaxNumOfItems;
   	iRepository->Get( KMuiuSentItemsCount, numItems );
    return numItems;
    }

// ---------------------------------------------------------
// CMsgSentItemsObserver::DeleteOldMessagesFromSentFolderL
//
// If there are more messages in the sent folder than the upper
// limit set by the user then delete the oldest messages.
// ---------------------------------------------------------
//
void CMsgSentItemsObserver::DeleteOldMessagesFromSentFolderL()
    {
    CMsvEntrySelection* children = iSentItemsFolder->ChildrenL();
    CleanupStack::PushL( children );
    TInt limit = SentFolderUpperLimit();
    TInt sentMessageCount = children->Count();
    if( sentMessageCount <= limit )
        {
        CleanupStack::PopAndDestroy( children );
        return;
        }
    CMsvEntrySelection* messagesToBeDeleted = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( messagesToBeDeleted );
    //oldest messages are at the end of the array
    for( TInt index = sentMessageCount - 1; index >= limit; index-- )
        {
        messagesToBeDeleted->AppendL( children->At( index ) );
        }
    DeleteMessagesFromSentFolderL( messagesToBeDeleted );
    CleanupStack::PopAndDestroy( messagesToBeDeleted  ); 
    CleanupStack::PopAndDestroy( children );
    }

// ---------------------------------------------------------
// CMsgSentItemsObserver::DeleteMessagesFromSentFolderL
//
// Delete the messages specified by aSelection
// ---------------------------------------------------------
//
void CMsgSentItemsObserver::DeleteMessagesFromSentFolderL( CMsvEntrySelection* aSelection )
    {
    if ( !IsActive() )
        {
#ifdef USE_LOGGER
        MEWLOGGER_WRITEF( _L("SentItems: deleting %d messages from sent folder"), aSelection->Count() );
#endif
        const TMsvId id = aSelection->At( 0 );
        delete iEntry;
        iEntry = 0;
        iEntry = iSession->GetEntryL( id );
        iEntry->SetEntryL( iEntry->Entry().Parent() );

        //if ( iSession->FileSession().ReserveDriveSpace( iMsgStoreDrive, KMsgErrorDiskSpaceForDelete ) == KErrNone )
        //    {
        //    iSession->FileSession().GetReserveAccess( iMsgStoreDrive );
        //    }

        delete iOp;
        iOp = 0;
        //TRAPD( err,
        //    {
            iOp = iEntry->DeleteL( *aSelection, iStatus ); //delete acts on returned CMsvOperation*
        //    } );
        //if ( err )
        //    {
        //    iSession->FileSession().ReleaseReserveAccess( iMsgStoreDrive );
        //    User::Leave( err );
        //    }
        //else
        //    {
            SetActive();
        //    }
        }
    }

//  End of File  

