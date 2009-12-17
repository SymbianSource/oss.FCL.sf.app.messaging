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
*     Item array for the message items to be displayed by mce.
*     Main view items are kept in memory (CArrayFixFlat<TMceListItem>) when created but
*     Message view list items are fetched from the msgs when needed in MdcaPoint function
*
*/



#ifndef MCEONEROWLISTITEMARRAYLOADER_H
#define MCEONEROWLISTITEMARRAYLOADER_H

//  INCLUDES

#include "mcetemplate.h"
#include <msvapi.h>     // MMsvSessionObserver
#include <bldvariant.hrh>
#include <PhCltTypes.h>
#include <akntreelistconstants.h>
#include <akncustomtreeordering.h>

// CONSTANTS

const TInt KMceOneRowListDaySubtitleCount = 15;

// FORWARD DECLARATIONS
class CMsvSession;
class CMceOneRowBitmapResolver;
class MMtmUiDataRegistryObserver;
class CMceOneRowListItemArray;
class CCoeEnv;

// CLASS DECLARATION

/**
*  CMceOneRowListItemArrayLoader
*
*
*/
class CMceOneRowListItemArrayLoader :
    public CActive,
    public MMsvSessionObserver
    {
    public:

    /**
    *  THCListListItemType
    */
    enum THListItemType
        {
        EHListItemEntry = 0,    // Entry item
        EHListItemSubtitle      // Subtitle item
        };

    /**
    *  THCListListItem
    */
    class THCListListItem
        {
        public:
            THListItemType  iItemType;
            TInt            iSubtitleType;
            TMsvId          iEntryId;   // first item in the selection
            TAknTreeItemID  iTreeItemId;
            TBool           iMarked;    // Marking status - list is destroyed for saving memory when collapsing
        };

    typedef RArray<THCListListItem> RHCListListItemArray;


    /** Array building status flags */
    enum TMceArrayItemFlags
        {
        EItemAdded          = 0x01,
        EFoldersAdded       = 0x02,
        ESubFolderLastWeek  = 0x04,
        ESubFolderOlder = 0x08,
        };

    public:  // Constructors and destructor

        static CMceOneRowListItemArrayLoader* NewL(
            CMsvSessionPtr aSession );

        static CMceOneRowListItemArrayLoader* NewL(
            TMsvId aFolderId,
            CMsvSessionPtr aSession );

    private:
        /**
        * C++ constructor.
        */
        CMceOneRowListItemArrayLoader(
            CMsvSessionPtr aSession );

        /**
        * 2nd phase constructor
        */
        void ConstructL( TMsvId aFolderId );

    public:

        /**
        * Destructor.
        */
        virtual ~CMceOneRowListItemArrayLoader();

    public: // New functions

        /**
        * Starts loading of MTMs
        * @param aSession: session to message server
        */
        void StartL();

        /**
        * Complete synchronously. Loads rest of the MTMs immediately.
        */
        void FinishL();

        /**
        * Returns list index of the message
        * @param aItemId: item to be searched.
        * @return Index of the item.
        */
        TInt ItemIndex( TMsvId aItemId ) const;

        /**
        * Returns message id of the list item.
        * @param aIndex: index of the item to be returned
        * @return message id (TMsvId)
        */
        TMsvId ItemId( TInt aIndex ) const;

        /**
        * Changes folder of the list.
        * @param aFolderId: folder id to be changed.
        */
        void SetEntryL( TMsvId aFolderId );

        /**
        * Returns item count in the array
        * @return count of items
        */
        TInt Count();

        /**
        * Get folder entry
        */
        const TMsvEntry& FolderEntry() const;

        /**
        * Get entry
        */
        TInt GetEntry( TMsvId aEntryId, TMsvEntry& aEntry );

        /**
        * Get entry
        */
        TInt GetEntry( TInt aIndex, TMsvEntry& aEntry );
        
        /**
        * Get subtitle type (time based)
        */
        TInt GetTimeBasedSubtitle( TInt aIndex );

        /**
        * Get opened folder entry
        */
        void GetCurrentEntry( TMsvEntry& aEntry );

        /**
        * Get opened folder entry
        */
        TMsvId CurrentEntryId();

        /**
        * Sets MMceListItemArrayObserver
        * Needed to get notify when MsgStore has changed
        * @param aObserver: Listitem observer
        */
        void SetListItemArrayObserver( MMceOneRowListItemArrayObserver* aObserver );

        /**
        * Set list sorting
        */
        void SetSortTypeL( THCListListType aType );

        /**
        * Sets sort order in folder entry
        * @param aOrdering: the sort order
        */

        /**
        * Sets sort order
        * @param aSortType: sort type
        * @param aOrdering: sort ordering(ascending/descending)  
        */
        void SetOrderingL( TInt aType, TBool aOrdering );

        /**
        * Saves new orderding to root entry
        * @param aNewOrdering
        */
        void SaveOrderingL( const TMsvSelectionOrdering& aNewOrdering ) const;

        /**
        * returns sort type
        */
        TMsvSelectionOrdering SortType();

        /**
        * Checks, if Delete should be discarded in outbox
        * @param aItemId: id of the item to be checked
        * @return ETrue, if Delete should be discarded
        */
        TBool DeleteDiscard( TMsvId aItemId ) const;

        /**
        * Set sort type
        */
        THCListListType SortTypeHCList();

        /**
        * returns folder's parent folder Id
        */
        TMsvId GetParentFolderId() const;

        /**
        * Check if item is folder
        * @param aItemId: id of the item to be checked
        * @return ETrue if item is folder
        */
        TBool IsItemFolder( TMsvId aItemId ) const;

        /**
        * @return Id of the tree list item
        */
        TAknTreeItemID TreeItemId( TInt aIndex );

        /**
        * Gets the MsvId of the entry
        * @param aTreeItemId: Id of item in tree list
        */
        TMsvId MsvItemId( TAknTreeItemID aTreeItemId );

        /**
        * @return Id of the parent
        */
        TAknTreeItemID ParentItemId( TInt aIndex );
        
        /**
        * @return index of the parent in array, KErrNotFound if item has no parent.
        */
        TInt ParentItemIndex( TInt aIndex );

        /**
        * Set the item id
        * @param aIndex: Index of the item
        * @param aTreeItemId: Id of item in tree list
        */
        void SetTreeItemId( TInt aIndex, TAknTreeItemID aTreeItemId );

        /**
        * Reset all tree items from the array
        */
        void ResetAllTreeItemId();

        /**
        * Reset one tree item from the array
        * @param aTreeItemId: id of the item to be reseted
        */
        void ResetTreeItemId( TAknTreeItemID aTreeItemId );

        /**
        * Compares two items
        * @param aFirst: id of the first item to compare
        * @param aSecond: id of the second item to compare
        */
        TInt Compare( TAknTreeItemID aFirst, TAknTreeItemID aSecond );

        /**
        * Finds first unread message from the opened folder
        * @param aEntryId: Entry Id 
        */
        TBool FindFirstUnreadMessageL( TMsvId& aEntryId ) const;

        /**
        * Search unread messages under subtitle
        * @param aSubtitleID: Subtitle Id 
        * @return ETrue if unread entry is found
        */
        TBool FindUnreadMessageUnderSubtitleL( TAknTreeItemID aSubtitleID );

        /**
        * Returns folder count
        */
        TInt FolderCount();

        /**
        * Get connection status
        */
        TBool IsConnected() const;

        /**
        * Finds entry index in message list
        * @param aMatchString: the string which is searched
        */
        TInt FindEntryIndex( const TDesC& aMatchString ) const;

        /**
        * Removes the prefixes from the given text string
        * @param aTextString: Text string
        * @return: start position after skipped prefix
        */
        TInt SkipPrefix( TPtrC aTextString ) const;

        /**
        * Returns index of the item in the list
        * @param aTreeItemId: Id of the item
        * @return: Index
        */
        TInt ItemIndexTreeId( TAknTreeItemID aTreeItemId );

        /**
        * Set item mark. Needed when collapsing subtitle and not keeping items in treelist node
        * @param aFocused: Current item id
        * @param aState: Set mark status
        */
        void SetItemMarked( TAknTreeItemID aFocused, TBool aState );

        /**
        * Gets item mark status
        * @param aFocused: Current item id
        * @return: Mark status of the item
        */
        TBool GetItemMarkStatusL( TMsvId aEntryId );

        /**
        * Returns subfolder item count
        * @param aParent: parent Id
        * @return: Subfolder item count
        */
        TInt GetSubfolderItemCount( TAknTreeItemID aParent );

        /**
        * Returns message count
        * @return: Count
        */
        TInt MessageCount();
        
        /**
        * Returns item type 
        * @return: Item type 
        */        
        TInt HCItemType( TInt aIndex );
//        THListItemType HCItemType( TInt aIndex );        
        
        /**
        * Returns number of created items.
        * @param aEntryId: TMsvId of the first item
        * @return: amount of created entries
        */
        TInt EntryCreatedL( TMsvId aEntryId );

        /**
        * Inserts an entry
        * @param
        */
        TInt InsertEntryL( TInt aEntryIndex, TMsvId aEntryId );
        
        /**
        * 
        * @param aEntryId: entry that was deleted.
        * @param aTreeItemId: deleted entry tree id, 0 if not found
        * @param aParentTreeItemId: parent id, 0 if parent was not deleted.
        * @return: is subtitle deleted
        */
        TBool EntryDeletedL( 
            TMsvId aEntryId, 
            TAknTreeItemID& aTreeItemId, 
            TAknTreeItemID& aParentTreeItemId );
        
        /**
        * Saves marked items
        * @param: aMarked Array of marked tree items id's
        */
        void SaveMarkingL( RArray<TAknTreeItemID>& aMarked );
        
        /**
        * Get today's time 
        */        
        TTime TodayTime () ;
        
        /**
         * This function Populates the iCmailSelectionList with all cmail entries.
         * @param
         * @return
         * @since S60 v5.0
         */
        void CreateCmailEntryIndexL() ;
        
        /**
         * This function returns true if iCmailSelectionList does not contain aId i.e. aId is not a cmail entry.
         * @param: aId: Entry to be searched in iCmailSelectionList
         * @return: True if Entry is not present in iCmailSelectionList
         * @since S60 v5.0
         */        
        TBool CheckCmailEntryIndexL (TMsvId aId);
    
    private:  // Functions from CActive
        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */
        virtual void RunL();

        /**
        * From MMsvSessionObserver
        */
        virtual void HandleSessionEventL(
            TMsvSessionEvent aEvent,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3);

    private: // New functions

        /**
        * Requests next mtm to be loaded.
        */
        void QueueLoad();

        /**
        * Load one mtm
        * @param aMtmType: mtm to load.
        */
        void LoadMessagesL();

        /**
        * Initialises array constuction process
        */
        void InitialiseSearch();

        /**
        * Appends messages to array
        */
        void AppendMessagesL();

        /**
        * Creates message time based array
        */
        void CreateTimeBasedArrayL();

        /**
        * Creates message sender based array
        */
        void CreateSenderBasedArrayL();

        /**
        * Creates message subject based array
        */
        void CreateSubjectBasedArrayL();

        /**
        * Creates message type based array
        */
        void CreateTypeBasedArrayL();

        /**        
        * return ETrue if items have same subtitle
        */
        TBool SameSubtitle( TInt aIndex1, TInt aIndex2 );

        /**
        * Insert folder entry to correct location in iHCListItemArray
        */        
        TInt InsertFolderEntryL( TInt aEntryIndex, TMsvId aEntryId );
        
        /**
        * Get timezone correction to entry time 
        * @param aEntryTime: 
        */
        void GetTime( TTime& aEntryTime ) const;

        /**
        * Get current time 
        * @param aCurrentTime: 
        */
        void GetCurrentTime( TTime& aCurrentTime ) const;
        
        /**
        * Find preceding entry from array
        * @param aId: Entry 
        * @return: index of found entry or KErrNotFound
        */
        TInt FindPreviousFromArrayL( TMsvId aId );

        /**
        * Find following entry from array
        * @param aId: Entry 
        * @return: index of found entry or KErrNotFound
        */
        TInt FindNextFromArrayL( TMsvId aId );

        /**
        * Get subtitle type for given time
        * @param aTime: Time
        * @param aUpdateCurrentTime: 
        * @return: Subtitle type
        */
        TInt TimeBasedSubtitleType( TTime& aTime, TBool aUpdateCurrentTime );

        /**
        * Updates time variables
        */
        void UpdateCurrentTime();

    private: //Data
        THCListListType                 iListType;
        CMsvSessionPtr                  iSession;
        CMsvEntry*                      iFolderEntry;
        CMsvEntrySelection*             iEntriesToBeAdded;
        TMsvId                          iEntryToBeAdded;
        CMsvEntrySelection*             iEntriesReceivedDuringListCreation;
        MMceOneRowListItemArrayObserver*      iListItemArrayObserver;
        RHCListListItemArray            iHCListItemArray;
        TInt                            iCounter;
        TInt                            iStep;
        TBool                           iSubtitleAdded[KMceOneRowListDaySubtitleCount];
        TInt                            iSubtitleType;
        TTime                           iToday;
        TTime                           iYesterday;
        TTime                           iOneWeekAgo;
        TTime                           iTwoWeeksAgo;
        TBool                           iAddSubtitle;
        TMsvSelectionOrdering           iPreviousSelectionOrdering;

        /**
         * CMail Changes, it will be ETrue if 
         * KFeatureIdFfEmailFramework is enabled
         */
        TBool                           iEmailFramework;          
        TBool                           iDuplicate;
        RArray<TMsvId>                  iMarkedArray;
        /**
         * iCmailSelectionList is the List of Cmail entries which, 
         * we are not willing to show in messaging folders.
         */        
        CMsvEntrySelection*             iCmailSelectionList ;
    public:
        // These should be in same order as in iNumberOfMessagesStrings array
        // in class CMceOneRowListItemArray
        enum TOneRowSubtitles
            {
            EMondayAdded = 0,
            ETuesdayAdded,
            EWednesdayAdded,
            EThursdayAdded,
            EFridayAdded,
            ESaturdayAdded,
            ESundayAdded,
            ETodayAdded,
            EYesterdayAdded,
            ELastWeekAdded,
            EOlderAdded
            };
    };


#endif      // MCEONEROWLISTITEMARRAYLOADER_H

// End of File
