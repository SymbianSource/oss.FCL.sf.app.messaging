/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MuiuDynamicSettingsItemFinder.cpp
*
*/



// INCLUDE FILES
#include <e32base.h>
#include "MuiuDynamicSettingsArray.hrh"
#include "MuiuDynamicSettingsArray.h"

#include "MuiuDynamicSettingsItemFinder.h"


// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::CMuiuDynamicSettingsItemFinder()
// ----------------------------------------------------------------------------
CMuiuDynamicSettingsItemFinder::CMuiuDynamicSettingsItemFinder(
    MMuiuDynamicSettingsItemFinderInterface& aObserver )
    :
    iObserver( aObserver ),
    iSubArrays( NULL ),
    iTempItem( NULL ),
    iSearchItem( 0 ),
    iCurrentItem( 0 ),
    iFinderArray( NULL )
    {
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::ConstructL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsItemFinder::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::~CMuiuDynamicSettingsItemFinder()
// ----------------------------------------------------------------------------
//
CMuiuDynamicSettingsItemFinder::~CMuiuDynamicSettingsItemFinder()
    {
    // Shouldn't exist but remove anyway
    if ( iSubArrays )
        {
        iSubArrays->Reset();
        }

    delete iSubArrays;
    iSubArrays = NULL;

    iTempItem = NULL;

    // Clean and remove the array
    if ( iFinderArray )
        {
        iFinderArray->Reset();
        }

    delete iFinderArray;
    iFinderArray = NULL;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::NewL()
// ----------------------------------------------------------------------------
//
CMuiuDynamicSettingsItemFinder* CMuiuDynamicSettingsItemFinder::NewL(
    MMuiuDynamicSettingsItemFinderInterface& aObserver )
    {
    CMuiuDynamicSettingsItemFinder* self = NewLC( aObserver );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::NewLC()
// ----------------------------------------------------------------------------
//
CMuiuDynamicSettingsItemFinder* CMuiuDynamicSettingsItemFinder::NewLC(
    MMuiuDynamicSettingsItemFinderInterface& aObserver )
    {
    CMuiuDynamicSettingsItemFinder* self =
        new ( ELeave ) CMuiuDynamicSettingsItemFinder( aObserver );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }


// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::SetSearchFlags()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsItemFinder::SetSearchFlags(
    const TMuiuFlags& aSearchFlags )
    {
    iSearchFlags = aSearchFlags;
    }


// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::SearchFlags()
// ----------------------------------------------------------------------------
//
TMuiuFlags CMuiuDynamicSettingsItemFinder::SearchFlags() const
    {
    return iSearchFlags;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::StartSearchL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsItemFinder::StartSearchL(
    CMuiuDynSetItemArray& aItemArray,
    const TUid& aId )
    {
    InitializeSearchL( KErrUnknown );
    SearchIdFromTreeL( aItemArray, aId );
    FinalizeSearchL();
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::StartSearchL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsItemFinder::StartSearchL(
    CMuiuDynSetItemArray& aItemArray,
    const TInt aNth )
    {
    InitializeSearchL( aNth );
    SearchIndexFromTreeL( aItemArray );
    FinalizeSearchL();
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::InitializeSearchL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsItemFinder::InitializeSearchL(
    const TInt aItem )
    {
    iSearchItem = aItem;
    iTempItem = NULL;

    // Clear flags
    iSearchFlags.ClearFlag( EFinderItemFound );
    iSearchFlags.ClearFlag( EFinderItemFindError );
    iSearchFlags.ClearFlag( EFinderExit );

    iSubArrays =
        new ( ELeave ) CMuiuDynSetItemArray( KMuiuDynArrayGranularity );

    // Create arrays
    if ( !iFinderArray )
        {
        iFinderArray =
            new ( ELeave ) CMuiuDynFinderItemArray( KMuiuDynArrayGranularity );
        }
    // Clean and remove the array
    else if ( !iSearchFlags.Flag( EFinderKeepLastResult ) )
        {
        iFinderArray->Reset();
        }
    else
        {
        // make lint to keep quiet
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::FinalizeSearchL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsItemFinder::FinalizeSearchL()
    {
    if ( iSubArrays )
        {
        iSubArrays->Reset();
        }

    delete iSubArrays;
    iSubArrays = NULL;

    iTempItem = NULL;

    // Clear flags
    iSearchFlags.ClearFlag( EFinderItemFound );
    iSearchFlags.ClearFlag( EFinderItemFindError );
    iSearchFlags.ClearFlag( EFinderExit );

    iSearchItem = 0;
    iCurrentItem = 0;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::ContinueSearch()
// ----------------------------------------------------------------------------
//
TBool CMuiuDynamicSettingsItemFinder::ContinueSearch(
    const TInt aCurrentItem,
    const TInt aMaxItems )
    {
    // First check if the search should be cancelled
    if ( iSearchFlags.Flag( EFinderExit ) )
        {
        return EFalse;
        }

    // Check if the item is found, and that it's enough
    if ( iSearchFlags.Flag( EFinderItemFound ) &&
         !iSearchFlags.Flag( EFinderSearchAll ) &&
         !iSearchFlags.Flag( EFinderResourceSearch ) )
        {
        return EFalse;
        }

    // Set the current item
    iCurrentItem = aCurrentItem;

    // Check if the last item is in progress
    if ( iCurrentItem >= aMaxItems )
        {
        if ( !iSearchFlags.Flag( EFinderItemFound ) )
            {
            iSearchFlags.SetFlag( EFinderItemFindError );
            }

        iSearchFlags.SetFlag( EFinderExit );
        return EFalse;
        }

    // Do custom checks if needed
    if ( iSearchFlags.Flag( EFinderCustomCheck ) &&
         !iObserver.SearchDoContinuationCheck( *iTempItem, iCurrentItem ) )
        {
        iSearchFlags.SetFlag( EFinderExit );
        return EFalse;
        }

    // New round is about to start, clear the item found flag
    iSearchFlags.ClearFlag( EFinderItemFound );

    // All checks done, continue searching
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::SearchingItem()
// ----------------------------------------------------------------------------
//
TBool CMuiuDynamicSettingsItemFinder::SearchingItem()
    {
    TBool result = EFalse;

    // If resource search, compare against resource id of the
    // current item
    if ( iSearchFlags.Flag( EFinderResourceSearch ) )
        {
        result = ( iTempItem->iItemResourceId == iSearchItem );
        }
    // When going through all items in the list, set found
    // item as true
    else if ( iSearchFlags.Flag( EFinderSearchAll ) )
        {
        result = ETrue;
        }
    // Index search is on, compare against the item index
    else
        {
        result = !iSearchItem--;
        }

    // Turn the flag according the search mode
    result ^= iSearchFlags.Flag( EFinderExlusiveSearch );

    return result;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::ItemToArrayL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsItemFinder::ItemToArrayL()
    {
    TMuiuFinderItem item;

    item.iIndex = iCurrentItem;
    item.iItem = iTempItem;
    iFinderArray->AppendL( item );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::ItemCheck()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsItemFinder::ItemCheckL( const TUid& aId )
    {
    // Make custom check for item, EXIT search
    if ( iSearchFlags.Flag( EFinderCustomCheck ) &&
         !iObserver.SearchDoItemCheck( *iTempItem ) )
        {
        return;
        }

    // If user has chosen to ingore rest of the checks, CONTINUE the search
    if ( iSearchFlags.Flag( EFinderOverrideChecks ) )
        {
        // Remove flags
        iSearchFlags.ClearFlag( EFinderItemFound );
        iSearchFlags.ClearFlag( EFinderExit );

        return;
        }

    // Id check
    if ( iTempItem->iItemId == aId )
        {
        // Id's of both items matches, so quit the search
        iSearchFlags.SetFlag( EFinderItemFound );
        ItemToArrayL();
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::ItemCheckL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsItemFinder::ItemCheckL()
    {
    // Make custom check for item, EXIT search
    if ( iSearchFlags.Flag( EFinderCustomCheck ) &&
         !iObserver.SearchDoItemCheck( *iTempItem ) )
        {
        // It's decided that item has matched, add to array
        iSearchFlags.SetFlag( EFinderItemFound );
        ItemToArrayL();
        return;
        }

    // If user has chosen to ingore rest of the checks, CONTINUE the search
    if ( iSearchFlags.Flag( EFinderOverrideChecks ) )
        {
        // Remove flags
        iSearchFlags.ClearFlag( EFinderItemFound );
        iSearchFlags.ClearFlag( EFinderExit );
        iSearchFlags.ClearFlag( EFinderOverrideChecks );

        return;
        }

    // Make hidden item check
    // If EFinderDoHideCheck is turned on, hidden items are not allowed
    // to be included into the search. When off, hidden items, except
    // permanently hidden items, are included into search
    // The item is found, when iSearchItem reaches zero.
    if ( !IsHidden() && SearchingItem() )
        {
        // Id's of both items matches, so quit the search
        iSearchFlags.SetFlag( EFinderItemFound );
        ItemToArrayL();
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::ItemSubArrayCheck()
// ----------------------------------------------------------------------------
//
TBool CMuiuDynamicSettingsItemFinder::ItemSubArrayCheck()
    {
    // Check if item quit is issued. The search shouldn't be cancelled even
    // if the item is found
    if ( iSearchFlags.Flag( EFinderExit ) )
        {
        return EFalse;
        }

    // Check if the subfolders need to be searched
    if ( !iSearchFlags.Flag( EFinderSearchSubFolders ) )
        {
        return EFalse;
        }

    // Check if the item is subarray item
    return iTempItem->HasLinkArray();

    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::ContinueSubSearch()
// ----------------------------------------------------------------------------
//
TBool CMuiuDynamicSettingsItemFinder::ContinueSubSearch()
    {
    // If search is finished, don't continue the search
    if ( iSearchFlags.Flag( EFinderExit ) )
        {
        return EFalse;
        }

    // If items left in subarray list
    return iSubArrays->Count() > 0;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::IsHidden()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsItemFinder::IsHidden()
    {
    // Permanently hidden items are shown only, if the ownership of the
    // array is moved to client. As then the client will get full access
    // to items in the array
    if ( iTempItem->iItemFlags.Flag( EMuiuDynItemPermanentlyHidden ) &&
         !iSearchFlags.Flag( EFinderMoveOwnership ) &&
         iSearchFlags.Flag( EFinderDoHideCheck ) )
        {
        return KErrNotSupported;
        }
    // Hidden items are available only, if flag EFinderDoHideCheck
    // is turned off.
    else if ( iSearchFlags.Flag( EFinderDoHideCheck ) &&
        iTempItem->iItemFlags.Flag( EMuiuDynItemHidden ) )
        {
        return KErrNotFound;
        }
    // The item is included into the search
    else
        {
        return KErrNone;
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::DoIndexSearchL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsItemFinder::DoIndexSearchL(
    CMuiuDynSetItemArray& aItemArray )
    {
    // Search for the item in the current array before moving inside
    // the subarrays
    TInt currentItem = 0;
    TInt max = aItemArray.Count();
    for ( ; ContinueSearch( currentItem, max ); currentItem++ )
        {
        // Store the item in array to member
        iTempItem = aItemArray[currentItem];

        // Check if the item matches and set quit flag if the id of the item
        // matches with the one that is searched
        ItemCheckL();

        // If the item wasn't the searched one, check if it contains subarray
        // and append it to stack, for later searching
        if ( ItemSubArrayCheck() )
            {
            iSubArrays->AppendL( iTempItem );
            }
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::DoIdSearchL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsItemFinder::DoIdSearchL(
    CMuiuDynSetItemArray& aItemArray,
    const TUid& aId )
    {
    // Search for the item in the current array before moving inside
    // the subarrays
    TInt currentItem = 0;
    TInt max = aItemArray.Count();
    for ( ; ContinueSearch( currentItem, max ); currentItem++ )
        {
        // Store the item in array to member
        iTempItem = aItemArray[iCurrentItem];

        // Check if the item matches and set quit flag if the id of the item
        // matches with the one that is searched
        ItemCheckL( aId );

        // If the item wasn't the searched one, check if it contains subarray
        // and append it to stack, for later searching
        if ( ItemSubArrayCheck() )
            {
            iSubArrays->AppendL( iTempItem );
            }
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::SearchSubArraysL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsItemFinder::DoIdSearchSubArraysL(
    const TUid& aId )
    {
    // If the correct item couldn't be found from the array,
    // search from the subarrays
    while ( ContinueSubSearch() )
        {
        // Get the first item in the array and remove it from there
        CMuiuSettingsLink* item =
            static_cast<CMuiuSettingsLink*>( iSubArrays->At( 0 ) );
        iSubArrays->Delete( 0 );

        // Search through the new tree
        if ( item->HasLinkArray() )
            {
            SearchIdFromTreeL( *item->iItemLinkArray, aId );
            }
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::SearchSubArraysL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsItemFinder::DoIndexSearchSubArraysL()
    {
    // If the correct item couldn't be found from the array,
    // search from the subarrays
    while ( ContinueSubSearch() )
        {
        // Get the first item in the array and remove it from there
        CMuiuSettingsLink* item =
            static_cast<CMuiuSettingsLink*>( iSubArrays->At( 0 ) );
        iSubArrays->Delete( 0 );

        // Search through the new tree
        if ( item->HasLinkArray() )
            {
            SearchIndexFromTreeL( *item->iItemLinkArray );
            }
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::SearchIdFromTreeL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsItemFinder::SearchIdFromTreeL(
    CMuiuDynSetItemArray& aItemArray,
    const TUid& aId )
    {
    // The search is made in following pattern:
    // First: The given array is searched through until EFinderExit flag is
    //        set or the end has been reached. The flag is set when given
    //        item(s) is (are) found. Each matching item and its index will
    //        be stored into array.
    //        Each item is checked for subarrays (during the first search).
    //        If the item has subarray, the item will be stored for
    //        searching.
    // Second: After the given array has been searched, each subarray will
    //         be searched, until conditions mentioned above have met, or
    //         each of the subarray's has been searched through.
    // The function is recursive

    // Search for the item in the current array
    DoIdSearchL( aItemArray, aId );

    // Before searching subarrays, check if subarray search is actually
    // need at all
    SearchShouldStop();

    // Search through the subarrays, if any exists
    DoIdSearchSubArraysL( aId );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::SearchIndexFromTreeL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsItemFinder::SearchIndexFromTreeL(
    CMuiuDynSetItemArray& aItemArray )
    {
    // Search for the index from the existing array
    DoIndexSearchL( aItemArray );

    // Before searching subarrays, check if subarray search is actually
    // need at all
    SearchShouldStop();

    // Check the subarrays for the existing index
    DoIndexSearchSubArraysL();
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::SearchShouldStop()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsItemFinder::SearchShouldStop()
    {
    // Check if searching has been issued to be stopped
    if ( iSearchFlags.Flag( EFinderExit ) )
        {
        // Store flags to variables to keep things readable
        TBool found = iSearchFlags.Flag( EFinderItemFound );
        TBool seekAll =
            iSearchFlags.Flag( EFinderResourceSearch ) ||
            iSearchFlags.Flag( EFinderSearchAll );

        // If item has been found and only single item is searched
        // allow quitting, otherwise the search must continue
        iSearchFlags.ChangeFlag( EFinderExit, found && !seekAll );
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::FinderArray()
// ----------------------------------------------------------------------------
//
CMuiuDynFinderItemArray* CMuiuDynamicSettingsItemFinder::FinderArray() const
    {
    return iFinderArray;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsItemFinder::CopyFinderArrayLC()
// ----------------------------------------------------------------------------
//
CMuiuDynFinderItemArray* CMuiuDynamicSettingsItemFinder::FinderArrayLC() const
    {
    // Create new array object
    CMuiuDynFinderItemArray* finderArray =
        new ( ELeave ) CMuiuDynFinderItemArray( KMuiuDynArrayGranularity );
    CleanupStack::PushL( finderArray );

    // Duplicate the array
    TInt items = iFinderArray->Count();
    for ( TInt item = 0; item < items; item++ )
        {
        finderArray->AppendL( iFinderArray->At( item ) );
        }

    return finderArray;
    }



//  End of File
