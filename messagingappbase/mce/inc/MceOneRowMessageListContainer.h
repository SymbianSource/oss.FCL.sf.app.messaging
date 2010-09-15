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
*     Container for main view.
*     Derived from CMceListContainer.
*
*/



#ifndef MCEONEROWMESSAGELISTCONTAINER_H
#define MCEONEROWMESSAGELISTCONTAINER_H

//  INCLUDES
#include <coecntrl.h> // CCoeControl
#include <bldvariant.hrh>
#include <akntreelistobserver.h>
#include <msvstd.h>

#include "MceMessageListContainerBase.h"
#include "MceMessageListContTimer.h"
#include "MceOneRowMessageListArray.h"

// FORWARD DECLARATIONS
class CMceOneRowListItemArray;
class CAknSingleColumnStyleTreeList;
class CAknsBasicBackgroundControlContext;
class CPtiEngine;
class CAknTreeList;
class CMtmUiDataRegistry;
// CLASS DECLARATION


/**
*  Container for main view.
*  Derived from CMceListContainer.
*/
class  CMceOneRowMessageListContainer :
    public CMceMessageListContainerBase,
    public MAknTreeListObserver,
    public MMceOneRowListItemArrayObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMceOneRowMessageListContainer* NewL(
            CMceMessageListView& aParentView,
            const TRect& aRect,
            CMsvSessionPtr aSession,
            MMtmUiDataRegistryObserver& aRegObserver );

        /**
        * Destructor.
        */
        virtual ~CMceOneRowMessageListContainer();

        /**
        * From MAknTreeListObserver.
        * Handles tree list events.
        */
        TBool HandleTreeListEvent( CAknTreeList& aList, TAknTreeItemID aItem,
            TEvent aEvent );

        /**
        * Resets tree list
        */
        void ResetListbox();

        /**
        * Returns ETrue if empty list or there are items selected.
        */
        TBool DisplayOptionsMenuFromSelectionKey() const;

        /**
        * From MMceOneRowListItemArrayObserver
        */
        void MceListItemArrayChangedL(
            CArrayFixFlat<TInt>& aChangedItems );

        /**
        * From MMceOneRowListItemArrayObserver
        */
        void HandleMarkCommandL( TInt aCommand );

        /**
        * From CMceMessageListContainerBase
        */
        TInt MceListId() const;

        /**
        * From CMceMessageListContainerBase
        */
        virtual TInt Count() const;

        /**
        * From CMceMessageListContainerBase
        */
        virtual TInt MessageCount() const;

        /**
        * From CMceMessageListContainerBase
        */
        virtual TInt FolderCount() const;

        /**
        * From CMceMessageListContainerBase
        */
        virtual TBool SetCurrentItemIdL( TMsvId aEntryId );

        /**
        * From CMceMessageListContainerBase
        */
        virtual void ResetCurrentItemL();

        /**
        * From CMceMessageListContainerBase
        */
        TMsvId FindEntry( const TDesC& aMatchString ) const;

        /**
        * From CMceMessageListContainerBase
        */
        virtual void ItemCountChangedL( TBool aItemsAdded,
                CArrayFix<TInt>* aAddedIndexes = 0 );

        /**
        * From CMceMessageListContainerBase
        */
        virtual void RefreshListbox();

        /**
        * From CMceMessageListContainerBase
        */
        virtual TBool FindFirstUnreadMessageL( TMsvId& aEntryId ) const;

        /**
        * From CMceMessageListContainerBase
        */
        virtual TMessageListItemType CurrentItemType() const;

        /**
        * From CMceMessageListContainerBase
        */
        void SetFolderL( TMsvId aFolderId );

        /**
        * From CMceMessageListContainerBase
        */
        TMsvId CurrentFolderId() const;

        /**
        * From CMceMessageListContainerBase
        */
        const TMsvEntry& FolderEntry() const;

        /**
        * From CMceMessageListContainerBase
        */
        TInt CurrentItemIndex() const;

        /**
        * From CMceMessageListContainerBase
        */
        TMsvId  CurrentItemId() const;

        /**
        * From CMceMessageListContainerBase
        */
        CMsvEntrySelection* CurrentItemSelectionL();

        /**
        * From CMceMessageListContainerBase
        */
        CMsvEntrySelection* CurrentItemSelectionRefreshL();

        /**
        * From CMceMessageListContainerBase
        */
        TInt CurrentItemSelectionCount() const;

        /**
        * From CMceMessageListContainerBase
        */
        TInt SelectionCount() const;

        /**
        * From CMceMessageListContainerBase
        */
        virtual TBool IsItemSelected( TMsvId aItemId );

        /**
        * From CMceMessageListContainerBase
        */
        void ClearSelection();

        /**
        * From CMceMessageListContainerBase
        */
        void MarkItemSelectionL();

        /**
        * From CMceMessageListContainerBase
        */
        void AddCurrentItemToSelectionL();

        /**
        * From CMceMessageListContainerBase
        */
        void RemoveCurrentItemFromSelection();

        /**
        * From CMceMessageListContainerBase
        */
        void RefreshSelectionIndexesL( TBool aForceUpdate = EFalse );

        /**
        * From CMceMessageListContainerBase
        */
        void AddAllToSelectionL();
        
        /**
        * From CMceMessageListContainerBase
        */
        void MarkAllReadMessagesL();

        /**
        * From CMceMessageListContainerBase
        */
        TInt MarkedItemsCount();

        /**
        * From CMceMessageListContainerBase 
        */
        TBool IsCurrentItemFolder() const;

        /**
        * From CMceMessageListContainerBase
        */
        TBool IsItemFolder( TMsvId aItemId ) const;

        /**
        * From CMceMessageListContainerBase
        */
        TBool IsItemFolder( TInt aIndex ) const;

        /**
        * From CMceMessageListContainerBase
        */
        TBool IsItemRemote( TMsvId aItemId ) const;

        /**
        * From CMceMessageListContainerBase
        */
        TBool IsOpenedFolder( TMsvId aFolderId ) const;

        /**
        * From CMceMessageListContainerBase
        */
        virtual void SetSortTypeL( TInt aSortType, TBool aOrdering );

        /**
        * From CMceMessageListContainerBase
        */
        virtual void SetMtmUiDataRegistry( MMtmUiDataRegistryObserver* aRegistryObserver );

        /**
        * From CMceMessageListContainerBase
        */
        void SetListEmptyTextL(const TDesC& aText);

        /**
        * From CCoeControl
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;

        /**
        * From CMceMessageListContainerBase
        */
        void SetAnchorItemIdL(
            TMessageListOperationType aOperationType );

        /**
        * From CMceMessageListContainerBase
        */
        void HandleMsvSessionEventL(
            MMsvSessionObserver::TMsvSessionEvent aEvent,
            CMsvEntrySelection& aSelection,
            TBool aDraw );
        
        /**
        * From CMceMessageListContainerBase 
        */
        TUint OptionsCommandSupported() const;

        /**
        * From CMceMessageListContainerBase 
        */
        void HandleOptionsCommandL( TInt aCommand );
        
        /**
        * From CMceMessageListContainerBase 
        */
        void DateTimeFormatChangedL( TInt aChangeFlag );

        /**
        * From CMceMessageListContainerBase 
        */
        TInt SetSpecialMskL();
            
        /**
        * Expands focused node
        */
        void ExpandFocusedNode();

        /**
        * Collapses focused node
        */
        void CollapseFocusedNode();
        
        /**
        * From CMceMessageListContainerBase 
        */
        void UpdateIconArrayL( );
		
        /**
        * From CMceMessageListContainerBase 
        */
        void SetMarkingModeOff();

    public:     // Functions from base classes

        /**
        * From CCoeControl
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * From CCoeControl
        */
        virtual TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );

        /**
        * From CCoeControl
        */
        void SizeChanged();

        /**
        * From CAknAppUi
        */
        void HandleResourceChange( TInt aType );

        /**
        * From MEikCommandObserver
        */
        virtual void ProcessCommandL(TInt aCommand);

        /**
        * Retrieves an object of the same type as that encapsulated in aId.
        *
        * @param aId An encapsulated object type ID.
        */
        TTypeUid::Ptr MopSupplyObject( TTypeUid aId );

        /**
        * Tries to find item from the currently opened list and retuns
        * list index if found. KErrNotFound if not found.
        * @param aId: item id to be searched
        * @return List index of the item.
        */
        TInt ItemIndex( TMsvId aId ) const;

        /**
        * From CCoeControl.
        * Draws this CHierarchicalColumnListAppView to the screen.
        *
        * @param aRect the rectangle of this view that needs updating.
        */
        void Draw( const TRect& aRect ) const;

        /**
        * Sets MSK command
        * @param aLabelResourceId Resource Id for MSK
        */
        void ChangeMskCommand( TInt aLabelResourceId );

        /**
        * From CMceMessageListContainerBase 
        */
        void SetContainerFlag( TMceContainerFlags aFlag, TBool aState );

    protected:  // Functions from base classes
        // From CCoeControl
        void FocusChanged(TDrawNow aDrawNow);

    private:

        /**
        * Symbian OS constructor.
        */
        void ConstructL(
            const TRect& aRect,
            CMsvSessionPtr aSession,
            MMtmUiDataRegistryObserver& aRegObserver );

        /**
        * Constructor is private.
        */
        CMceOneRowMessageListContainer( CMceMessageListView& aParentView );

        /**
         * Function that check if a key can be search
         * @param aPointerEvent The pointer event
         * @return ETrue if it can
         * since 3.2
         */
        TBool IsValidSearchKey(const TKeyEvent& aKeyEvent);

        /**
        * Add children items to specified node
        * @param aParentId Parent node Id
        */
        void AddChildrenL( TAknTreeItemID aParentId );

        /**
        * Removes children from specified node
        * @param aParentId Parent node Id
        */
        void RemoveChildren( TAknTreeItemID aParentId );

        /**
        * Gets number of marked items
        * @return number of marked items
        */
        TInt GetMarkedItemsCountL() const;
        
        /**
        * Adds default icons
        */
        void AddDefaultIconsL();
        
        /**
        * Adds an entry to tree list
        * @param aEntryIndex index of new entry
        */
        void AddEntryL( TInt aEntryIndex );
        
        /**
        * Updates an entry
        * @param aEntryId TMsvId of the entry to be changed
        */
        void UpdateEntryL( TMsvId aEntryId );
        
        /**
        * Updates tree list when new entry created
        * @param aSelection Items created
        * @param aDraw Will list be drawed immediality
        */
        void EntryCreatedL( 
            CMsvEntrySelection& aSelection,
            TBool aDraw );
        
        /**
        * Updates tree list when entry deleted
        * @param aSelection Items deleted
        * @param aDraw Will list be drawed immediality
        */
        void EntryDeletedL( 
            CMsvEntrySelection& aSelection,
            TBool aDraw );
        
        /**
         * Handles shortcut for search
         * Since 5.0
         */
        void HandleShortcutForSearchL( const TKeyEvent& aKeyEvent );

        /*
        * Function that check if * key can change msglist state
        * @return ETrue if it can
        * Since 5.0
        */
        TBool StateCanBeChangedByStarKeyL() const;
        
        /*
        *  Check if * key has been handled
        *  @return ETrue if it's handled
        *  Since 5.0
        */
        TBool HasHandleStarKeyDoneL();
        
        /*
        *  mark item read/unread
        * Since 5.0
        */
        void MarkReadUnreadWithStarKeyL();

        /*
        * Check if message list is sort by date
        *  @return ETrue if it is
        *  Since 5.0
        */
        TBool IsSortByDate();

        /*
        * Get icon id for subtitle icon
        * @param aSortType used sorting 
        * @param aSetUnread ETrue if unread type of icon
        * @return Icon id 
        */
        TInt GetSubtitleIcon ( TBool aSetUnread );

        /*
        * Updates subtitle icon
        * @param aSubtitleID: subtitle ID
        * @param aReadStatusKnown: ETrue if there is known unread item under subtitle
        */
        void UpdateSubtitleIconL( TAknTreeItemID aSubtitleID, TBool aReadStatusKnown );

        /**
        * From CCoeControl.
        * Handles pointer events.
        * @param aPointerEvent The pointer event. 
        */
        virtual void HandlePointerEventL( const TPointerEvent& aPointerEvent );

        /**
        * Saves marked items
        */
        void SaveMarkingL();
        /**
        * Handles focus when operation is completed. 
        * This is called by SetAnchorItemIdL when it gets
        * EMessageListOperationCompleted.
        */
        void HandleOperationCompletedL();

        /**
         * Check the MTM technology type for Mail Technology 
         * @param aMtm: Mtm Uid
         * @return : ETrue if Mtm Technology is Mail
         *           otherwsie EFalse
         */
        TBool IsMailMtmTechnology( TUid aMtm )const;
        
    private:

        CAknSingleColumnStyleTreeList*      iTreeListBox;
        CMceOneRowListItemArray*            iListItems;
        CMceOneRowBitmapResolver*           iBitmapResolver;
        CMsvEntrySelection*                 iSelectedEntries;
        TMsvId                              iAnchorItemId;
        TAknTreeItemID                      iAnchorItemIndex;
        TMessageListOperationType           iLastOperationType;
        TMsvId                              iSelectedItemWaitingIndex;

        /**
        * Background control context.
        * Own.
        */
        CAknsBasicBackgroundControlContext* iBgContext;

        TAknTreeItemID              iNodeId;
        TBool                       iMarkingModeEnabled;
        CPtiEngine*                 iT9Interface;
        CMceMessageListContainerKeyTimer* iT9Timer;        
        TBool                       iPreviousInputMethod;
        TBool                       iInputMethodQwerty;
        TMceFlags                   iContainerFlags;
        TDay                        iStartOfWeek;
        TBool 			            iDialerEvent;
        /**
         * CMail Changes, it will be ETrue if 
         * KFeatureIdFfEmailFramework is enabled
         */
        TBool                       iEmailFramework;
        CMtmUiDataRegistry*         iUiRegistry;
        /**
         * For mass deletion of messages in onerowlist 
         * iDeletedMessges will keeptrack of no of messages got actually 
         *                 deleted and we got the call back from m/w
         * iSelectedSubTitles will keep list of all the subtitles from where
         *                 messages got deleted but not the subtitle.  
         */
        TInt                        iDeletedMessges;
        RArray<TAknTreeItemID>      iSelectedSubTitles;

    };

#endif

// End of file
