/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
*     Changes message flag.
*
*/



// INCLUDE FILES
#include "MceChangeFlagOperation.h"   // header

#include <msvids.h>         // KMsvRootIndexEntryId

// CONSTANTS

_LIT8 ( KMceSpace8, " " ); // 8 bit space


// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CMceRemoveNewFlag::NewL
// Two-phased constructor.
// ----------------------------------------------------
CMceRemoveNewFlag* CMceRemoveNewFlag::NewL(
    TUint32 aRemoveFlags,
    CMsvSession& aMsvSession, 
    TRequestStatus& aObserverRequestStatus, 
    CMsvEntrySelection* aSelection )
    {
    CMceRemoveNewFlag* self=new (ELeave) CMceRemoveNewFlag(
        aRemoveFlags, aMsvSession, aObserverRequestStatus, KMsvRootIndexEntryId, aSelection );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------
// CMceRemoveNewFlag::CMceRemoveNewFlag
// Constructor.
// ----------------------------------------------------
CMceRemoveNewFlag::CMceRemoveNewFlag(
    TUint32 aRemoveFlags,
    CMsvSession& aMsvSession, 
    TRequestStatus& aObserverRequestStatus, 
    TMsvId aFolderId,
    CMsvEntrySelection* aSelection )
    :
    CMsvOperation(aMsvSession, CActive::EPriorityStandard, aObserverRequestStatus), 
    iFolderId( aFolderId ),
    iEntrySelection( aSelection ),
    iBlank( KMceSpace8 )
    {
    CActiveScheduler::Add( this );
    iRemoveFlags.SetMceFlag( aRemoveFlags );
    }

// ----------------------------------------------------
// CMceRemoveNewFlag::~CMceRemoveNewFlag
// Destructor.
// ----------------------------------------------------
CMceRemoveNewFlag::~CMceRemoveNewFlag()
    {
    Cancel();
    delete iOperation;    
    delete iEntry;
    delete iEntrySelection;
    }

// ----------------------------------------------------
// CMceRemoveNewFlag::ConstructL
// Constructor..
// ----------------------------------------------------
void CMceRemoveNewFlag::ConstructL()
    {
    iCount = iEntrySelection->Count();
    iEntry = CMsvEntry::NewL( iMsvSession, iFolderId, TMsvSelectionOrdering() );
       
    if ( iEntrySelection->Count() == 0 )
        {
        iStatus=KRequestPending;
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }    
    else
        {
        TMsvId serviceId; // not used
        TMsvEntry entry;
        for ( TInt loop=iCount-1; loop>=0; loop-- )
            {            
            const TMsvId id = iEntrySelection->At( loop );
            User::LeaveIfError( iMsvSession.GetEntry( id, serviceId, entry ) );
            DeleteFromSelection( loop, entry.Unread() );
            }
         if ( iEntrySelection->Count() > 0 )
            {
            RemoveNextNewFlagL();
            }
         else
            {
            //all were read
            iStatus=KRequestPending;
            SetActive();
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, KErrNone );
            }
        }
    }

//
// ----------------------------------------------------------------------------
// CMceRemoveNewFlag::DeleteFromSelection()
// ----------------------------------------------------------------------------
//
void CMceRemoveNewFlag::DeleteFromSelection( TInt aIndex, TBool aUnread )
    {
    // Remove from selection, 
    // when read and setting to read or
    // when unread and setting to unread
    if ( iRemoveFlags.MceFlag( 
            EMceRemoveEntryFlagUnread ) && !aUnread || 
         iRemoveFlags.MceFlag( 
            EMceRemoveEntryFlagNew ) && aUnread ) //Changed from EMceRestoreEntryFlagRead
        {
        //remove read entries from selection
        iEntrySelection->Delete( aIndex );                
        }   
    }

// ----------------------------------------------------
// CMceRemoveNewFlag::RemoveNextNewFlagL
// Starts handling next message to mark it read.
// ----------------------------------------------------
void CMceRemoveNewFlag::RemoveNextNewFlagL()
    {   
    delete iOperation;
    iOperation = NULL;      

    TMsvId serviceId; // not used
    TMsvEntry entry;
    const TMsvId id = iEntrySelection->At( 0 );   
    User::LeaveIfError( iMsvSession.GetEntry( id, serviceId, entry ) );
    
    TRAPD( err, MakeNextRemoveL() );

    // whether error or not, remove the data for this operation
    iEntrySelection->Delete( 0 );
    // and set active
    iStatus=KRequestPending;
    SetActive();
    // if error then complete this pass with the error code
    if ( err )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    }


// ----------------------------------------------------
// CMceRemoveNewFlag::MakeNextRemoveL
// Marks message read.
// ----------------------------------------------------
void CMceRemoveNewFlag::MakeNextRemoveL()
    {
    iEntry->SetEntryL( (*iEntrySelection)[0] );
    iNewEntry = iEntry->Entry();
    
    // In no case that mce should set a message new
    // it used to be "!iRemoveFlags.MceFlag( EMceRemoveEntryFlagNew )" for some unknown reason
    iNewEntry.SetNew(  EFalse ); 
    iNewEntry.SetUnread( !iRemoveFlags.MceFlag( EMceRemoveEntryFlagUnread ) );        
    iOperation=iEntry->ChangeL( iNewEntry, iStatus );    
    }

// ----------------------------------------------------
// CMceRemoveNewFlag::DoCancel
// Cancels active object.
// ----------------------------------------------------
void CMceRemoveNewFlag::DoCancel()
    {
    if ( iOperation )
        {
        iOperation->Cancel();
        }
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, iStatus.Int() );
    }

// ----------------------------------------------------
// CMceRemoveNewFlag::RunL
// Runs active object.
// ----------------------------------------------------
void CMceRemoveNewFlag::RunL()
    {
    if ( iEntrySelection->Count()==0 )
        {
        // nothing left to process, so complete the observer
        TRequestStatus* status = &iObserverRequestStatus;
        User::RequestComplete( status, iStatus.Int() );
        }
    else 
        {
        // start the next pass
        RemoveNextNewFlagL();
        }   
    }

// ----------------------------------------------------
// CMceRemoveNewFlag::ProgressL
// Progressing of active object.
// ----------------------------------------------------
const TDesC8& CMceRemoveNewFlag::ProgressL()
    {
    if ( iOperation )
        {
        return iOperation->ProgressL();
        }
    return iBlank;
    }

// ---------------------------------------------------------
// CMceRemoveNewFlag::DecodeProgress
//
// ---------------------------------------------------------
//
TInt CMceRemoveNewFlag::DecodeProgress(
    const TDesC8& /*aProgress*/, 
    TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString, 
    TInt& aTotalEntryCount, 
    TInt& aEntriesDone,
    TInt& aCurrentEntrySize, 
    TInt& aCurrentBytesTrans, 
    TBool /*aInternal*/ )
    {
    aCurrentEntrySize = aTotalEntryCount = iCount;
    aCurrentBytesTrans = aEntriesDone = iCount - (iEntrySelection->Count());    
    aReturnString.Copy( _L("") );  
    return KErrNone;
    }

// ----------------------------------------------------
// CMceRemoveNewFlag::RunError
// Handles a leave occurring in the request completion event handler RunL().
// ----------------------------------------------------
TInt CMceRemoveNewFlag::RunError( TInt /*aError*/ )
    {
    // Error occured while processing, so complete the observer with current status
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, iStatus.Int() );
        
        // Just ignore any error and continue without
    // any handling to allow smooth execution.
    return KErrNone;    
    }

// End of file
