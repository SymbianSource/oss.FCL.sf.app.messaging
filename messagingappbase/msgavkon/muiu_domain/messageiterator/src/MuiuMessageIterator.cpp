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
* Description: MuiuMessageIterator implementation
*
*/



// INCLUDE FILES
#include <msvids.h>         // KMsvNullIndexEntryId
#include <mtclreg.h>        // CClientMtmRegistry
#include <mtuireg.h>        // CMtmUiRegistry
#include <mtmuibas.h>       // CBaseMtmUi
#include <mtmuidef.hrh>     // CBaseMtmUi
#include "MuiuMessageIterator.h"
#include "muiudomainpan.h"  // Panic codes


// CONSTANTS
const TUid KUidMsvMessageListSortOrder={0x10005359};



// ============================ MEMBER FUNCTIONS ===============================


// ---------------------------------------------------------
// CMessageIterator::NewL
// ---------------------------------------------------------
//
EXPORT_C CMessageIterator* CMessageIterator::NewL( CMsvSession& aSession, 
                                                   const TMsvEntry& aCurrentMessage )
    {   // static
    CMessageIterator* me = new( ELeave ) CMessageIterator;
    CleanupStack::PushL( me );
    me->ConstructL( aSession,aCurrentMessage );
    CleanupStack::Pop( me );
    return me;
    }

// ---------------------------------------------------------
// CMessageIterator::CMessageIterator
// ---------------------------------------------------------
//
CMessageIterator::CMessageIterator()
    {
    // Nothing to do.
    // Note iEntryObserver has been initialised to EFalse as this is a CBase derived class.
    }


// ---------------------------------------------------------
// CMessageIterator::ConstructL
// ---------------------------------------------------------
//
void CMessageIterator::ConstructL( CMsvSession& aSession, const TMsvEntry& aCurrentMessage )
    {
    // Assume only the root entry has no parent
    __ASSERT_DEBUG( aCurrentMessage.Id() != KMsvRootIndexEntryId, 
                    Panic( EMuiuMsgIteratorEntryHasNoParent ) );
    
    //
    // Get crrent sort settings used by Message Centre for displaying the message list
    // this assumes that the message list sort order changing doesn't 
    // affect the editors next/prev behaviour
    // i.e. the iterator uses the sort order that was being used when it was created.
    TMsvSelectionOrdering sort( KMsvGroupByType, EMsvSortByDateReverse, EFalse );
    CMsvEntry* entry=aSession.GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL( entry );
    if ( entry->HasStoreL() )
        {
        CMsvStore* store = entry->ReadStoreL();
        CleanupStack::PushL( store );
        if ( store->IsPresentL( KUidMsvMessageListSortOrder ) )
            {
            RMsvReadStream readStream;
            readStream.OpenLC( *store, KUidMsvMessageListSortOrder );
            sort.InternalizeL( readStream );
            CleanupStack::PopAndDestroy(); // readStream
            }
        CleanupStack::PopAndDestroy( store );
        }
    
    //
    // Setup iParentOfCurrent. Set sorting of parents children so
    // next/prev order matches the view the user had of the messages from the Email app.
    // Also become an observer of the parent to keep our copy of the children list updated.
    entry->SetEntryL( aCurrentMessage.Parent() );
    CleanupStack::Pop( entry ); // entry  (we're about to allocate it to instance data)
    iParentOfCurrent = entry;
    
    __ASSERT_ALWAYS( iParentOfCurrent,Panic( EMuiuMsgIteratorParentEntryNull ) );
    
    iParentOfCurrent->SetSortTypeL( sort );       // set sort to be same as Message Centre
    iParentOfCurrent->AddObserverL( *this );
    iEntryObserver = ETrue;    // To ensure we de-register, as an observer, in the destructor
    
    //
    // Create iChildrenOfParent and intialise iCurrentIndex
    iChildrenOfParent=new( ELeave ) CMsvEntrySelection;
    UpdateChildrenOfParentL( aCurrentMessage.Id() );
    }


// ---------------------------------------------------------
// CMessageIterator::HandleEntryEventL
//
// HandleEntryEvent from MMsvEntryObserver
// If the parent of current entry changes then we may need to re-build our local children list.
// ---------------------------------------------------------
//
void CMessageIterator::HandleEntryEventL(TMsvEntryEvent aEvent, 
                                         TAny* /*aArg1*/, 
                                         TAny* /*aArg2*/, 
                                         TAny* /*aArg3*/)
    {
    //
    // Ensure we're ready to handle the event
    if( !iParentOfCurrent )
        {
        return;
        }
    
    // React according to event type
    switch( aEvent )
        {
        case EMsvEntryDeleted:
            // This should not occur as the editor should have locked the entry
            // Panic( EMuiuMsgIteratorParentEntryDeleted );
            break;
            
        case EMsvEntryMoved:
            // This should not occur as the editor should have locked the entry
            Panic( EMuiuMsgIteratorParentEntryMoved );
            break;
            
        case EMsvChildrenChanged:
            // No need to do anything as a child has changed, but not the children list
            break;
            
        case EMsvContextInvalid:
        case EMsvChildrenMissing:
        case EMsvChildrenInvalid:
            // Have to call SetEntryL before re-builiding children list
            iParentOfCurrent->SetEntryL( iParentOfCurrent->EntryId() );
            // drop thru to rebuild children list code
            
        default:
        case EMsvEntryChanged:
        case EMsvNewChildren:
        case EMsvDeletedChildren:
            //
            // Re-create child list and refresh the index of our current entry
            
            //if a class 0 sms is received into empty inbox we have a situation
            //where Count == 0. This causes a crash when another class 0 arrives.
            if( iChildrenOfParent->Count() != 0 )
                {
                TInt originalCount = iChildrenOfParent->Count();
                UpdateChildrenOfParentL( ( *iChildrenOfParent )[iCurrentIndex] );
                
                if ( iMessageIteratorObserver && ( iChildrenOfParent->Count() != originalCount ))
                    {
                    iMessageIteratorObserver->HandleIteratorEventL( 
                                            MMessageIteratorObserver::EFolderCountChanged );
                    }
                }
            
            break;
        }
    }


// ---------------------------------------------------------
// CMessageIterator::~CMessageIterator
// ---------------------------------------------------------
//
EXPORT_C CMessageIterator::~CMessageIterator()
    {
    delete iChildrenOfParent;
    if( iParentOfCurrent )
        {
        // Only de-register our observer-ness if our original registration did not leave
        if( iEntryObserver )
            {
            iParentOfCurrent->RemoveObserver( *this );
            }
        delete iParentOfCurrent;
        }
    }


// ---------------------------------------------------------
// CMessageIterator::CurrentMessage
//
// Query
// ---------------------------------------------------------
//
EXPORT_C const TMsvEntry& CMessageIterator::CurrentMessage() const
    {
    __ASSERT_ALWAYS(iParentOfCurrent,Panic(EMuiuMsgIteratorParentEntryNull));
    __ASSERT_ALWAYS(iCurrentIndex<iChildrenOfParent->Count(),
                    Panic(EMuiuMsgIteratorInvalidCurrentIndex));

    TInt foundIndex = KErrNotFound;
    TMsvId entryId = ( *iChildrenOfParent )[iCurrentIndex];
    const TInt count = iParentOfCurrent->Count();

    for ( TInt loop = 0; loop < count && foundIndex == KErrNotFound; loop++ )
        {
        if ( ( *iParentOfCurrent )[ loop ].Id() == entryId )
            {
            foundIndex = loop;
            }

        }
    
    __ASSERT_ALWAYS( foundIndex != KErrNotFound,Panic( EMuiuMsgIteratorChildEntryNotFound ) );   
    return ( *iParentOfCurrent )[foundIndex];
    }

// ---------------------------------------------------------
// CMessageIterator::CurrentParent
//
// Query
// ---------------------------------------------------------
//
EXPORT_C const TMsvEntry& CMessageIterator::CurrentParent() const
    {
    __ASSERT_ALWAYS( iParentOfCurrent, Panic( EMuiuMsgIteratorParentEntryNull ) );
    return iParentOfCurrent->Entry();
    }

// ---------------------------------------------------------
// CMessageIterator::NextMessageExists
//
// Query
// ---------------------------------------------------------
//
EXPORT_C TBool CMessageIterator::NextMessageExists() const
    {
    //
    // Next message exists if current is not last in list
    if( iChildrenOfParent->Count() <= 0 )
        {
        return EFalse;
        }
    else
        {
        return( iChildrenOfParent->Count()-1 != iCurrentIndex );
        }
    }

// ---------------------------------------------------------
// CMessageIterator::PreviousMessageExists
//
// Query
// ---------------------------------------------------------
//
EXPORT_C TBool CMessageIterator::PreviousMessageExists() const
    {
    //
    // Previous message if current is not first in list
    return( 0 != iCurrentIndex );
    }

// ---------------------------------------------------------
// CMessageIterator::CurrentMessageIndex
//
// Query
// ---------------------------------------------------------
//
EXPORT_C TInt CMessageIterator::CurrentMessageIndex() const
    {
    return iCurrentIndex;
    }

// ---------------------------------------------------------
// CMessageIterator::MessagesInFolder
//
// Query
// ---------------------------------------------------------
//
EXPORT_C TInt CMessageIterator::MessagesInFolder() const
    {
    return iChildrenOfParent->Count();
    }


// ---------------------------------------------------------
// CMessageIterator::SetCurrentMessageL
//
// State changer
// ---------------------------------------------------------
//
EXPORT_C void CMessageIterator::SetCurrentMessageL( const TMsvEntry& aCurrentMessage )
    {
    // Assume only the root entry has no parent
    __ASSERT_DEBUG( aCurrentMessage.Id() != KMsvRootIndexEntryId, 
                    Panic( EMuiuMsgIteratorEntryHasNoParent ) );
    
    //
    // Update list of children
    iParentOfCurrent->SetEntryL( aCurrentMessage.Parent() );
    UpdateChildrenOfParentL( aCurrentMessage.Id() );
    }


// ---------------------------------------------------------
// CMessageIterator::SetNextMessage
//
// State changer.
// Returns KErrNotFound on 'no next message'
// ---------------------------------------------------------
//
EXPORT_C TInt CMessageIterator::SetNextMessage()
    {
    // Check if next message exists
    if( !NextMessageExists() )
        {
        return KErrNotFound;
        }
    else
        {
        // Increment index
        ++iCurrentIndex;
        return KErrNone;
        }
    }


// ---------------------------------------------------------
// CMessageIterator::SetPreviousMessage
//
// State changer.
// Returns KErrNotFound on 'no previous message'
// ---------------------------------------------------------
//
EXPORT_C TInt CMessageIterator::SetPreviousMessage()
    {
    // Check if next message exists
    if( !PreviousMessageExists() )
        {
        return KErrNotFound;
        }
    else
        {
        // Decrement index
        --iCurrentIndex;
        return KErrNone;
        }
    }


// ---------------------------------------------------------
// CMessageIterator::OpenCurrentMessageL
//
// Editor/viewer launcher
// ---------------------------------------------------------
//
EXPORT_C CMsvOperation* CMessageIterator::OpenCurrentMessageL( TRequestStatus& aStatus,
                                                               TBool aEmbed )
    {
    __ASSERT_ALWAYS( iParentOfCurrent, Panic( EMuiuMsgIteratorParentEntryNull ) );
    
    // Get mtm ui layer
    CClientMtmRegistry* mtmReg = CClientMtmRegistry::NewL( iParentOfCurrent->Session() );
    CleanupStack::PushL( mtmReg );
    CBaseMtm* baseMtm = mtmReg->NewMtmL( CurrentMessage().iMtm );
    CleanupStack::PushL( baseMtm );
    CMtmUiRegistry* mtmUiReg = CMtmUiRegistry::NewL( iParentOfCurrent->Session() );
    CleanupStack::PushL( mtmUiReg );
    CBaseMtmUi* mtmUi = mtmUiReg->NewMtmUiL( *baseMtm );
    CleanupStack::PushL( mtmUi );
    
    // Open the message
    baseMtm->SwitchCurrentEntryL( CurrentMessage().Id() );
    if( aEmbed )
        {
        mtmUi->SetPreferences( mtmUi->Preferences() | EMtmUiFlagEditorPreferEmbedded );
        }
    CMsvOperation* op = mtmUi->OpenL( aStatus );
    
    CleanupStack::PopAndDestroy( 4, mtmReg ); // mtmUi, mtmUiReg, baseMtm, mtmReg
    return op;
    }


// ---------------------------------------------------------
// CMessageIterator::SetMessageIteratorObserver
//
// Set iterator observer.
// ---------------------------------------------------------
//
EXPORT_C void CMessageIterator::SetMessageIteratorObserver( MMessageIteratorObserver* aObserver )
    {
    iMessageIteratorObserver = aObserver;
    }


// ---------------------------------------------------------
// CMessageIterator::UpdateChildrenOfParentL
//
// Intialise/update iChildrenOfParent & iCurrentIndex
// ---------------------------------------------------------
//
void CMessageIterator::UpdateChildrenOfParentL( TMsvId aCurrentMessageId )
    {
    // We assume that iParentOfCurrent has already had it's context set correctly
    // Get child list
    CMsvEntrySelection* tempSel = iParentOfCurrent->ChildrenL();
    CleanupStack::PushL( tempSel );
    
    TMsvId parentId = iParentOfCurrent->EntryId();
    // there is no need to check sub folder entry , if
    // parent ID is Inbox/Draft/sent/outbox
    if(parentId  != KMsvGlobalOutBoxIndexEntryId &&
            parentId  != KMsvGlobalInBoxIndexEntryId &&
            parentId  != KMsvDraftEntryId &&
            parentId  != KMsvSentEntryId )
        {
        // remove all except messages from the list.
        for ( TInt loop = tempSel->Count() - 1; loop >= 0; loop-- )
            {
            const TMsvEntry& childEntry = iParentOfCurrent->ChildDataL( ( *tempSel )[loop] );
    
            if ( childEntry.iType.iUid != KUidMsvMessageEntryValue )
                {
                tempSel->Delete( loop );
                }
            }
        }
    // Use the new child list
    delete iChildrenOfParent;
    CleanupStack::Pop( tempSel );
    iChildrenOfParent = tempSel;
    
    // Update current index
    iCurrentIndex = iChildrenOfParent->Find( aCurrentMessageId );
    if ( iCurrentIndex < 0 )
        {
        iCurrentIndex = 0;
        }
    }

// End of file
