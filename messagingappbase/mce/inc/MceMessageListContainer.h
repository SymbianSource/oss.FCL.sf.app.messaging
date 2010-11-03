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
*     List container user by message view.
*     Derived from CMceListContainer.
*
*/



#ifndef MCEMESSAGELISTCONTAINER_H
#define MCEMESSAGELISTCONTAINER_H

//  INCLUDES
#include <coecntrl.h> // CCoeControl
#include <msvapi.h>
//#include "MceListContainer.h"
#include "MceMessageListContainerBase.h"
#include "MceMessageListContTimer.h"

#include "mcetemplate.h"
#include "MceUtils.h"
#include "MceListItem.h"
#include <centralrepository.h> //CRepository
#include <bldvariant.hrh>
#include <eikcmobs.h>
#include <e32property.h>

// FORWARD DECLARATIONS
class CMceMessageListView;
class CPtiEngine;
class CMcePropertySubscriber;
class CMsvSession;
class CMceBitmapResolver;
class CMceMessageListItemArray;
class CMceMessageListBox;
class MEikListBoxObserver;

//CONSTS
const TInt KMceSelectionReplacementSize = 5;


// CLASS DECLARATION

/**
*  List container user by message view.
*  Derived from CMceListContainer.
*/
class  CMceMessageListContainer :
    public CMceMessageListContainerBase
//    public CCoeControl,
//    public MEikCommandObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMceMessageListContainer* NewL(
            const TRect& aRect,
            CMsvSessionPtr aSession,
            TMsvId aFolderId,
            CMceBitmapResolver& aBitmapResolver,
            CMceMessageListView& aParentView );

        /**
        * Destructor.
        */
        virtual ~CMceMessageListContainer();

    public:
    
        /**
        * Returns list index of the current item
        * @return Index of the current item
        */
        TInt    CurrentItemIndex() const;

        /**
        * Returns mce's info of the current message
        * @return info
        */
        TMceListItem CurrentItemListItem() const;

        /**
        * Tries to find item from the currently opened list and retuns
        * list index if found. KErrNotFound if not found.
        * @param aId: item id to be searched
        * @return List index of the item.
        */
        TInt ItemIndex( TMsvId aId ) const;

        /**
        * Sets MSK label according to a focused item
        * Since 3.2
        */
        void SetMskL();

        /**
        * Is Outbox open?
        */
        TBool IsOutboxOpened() const;

        /**
        * Returns ETrue if Syncml outbox opened
        */
        TBool SyncMlOutboxOpened() const;


    public:     // Functions from base classes
    
        /**
        * From CMceMessageListContainerBase 
        */
        virtual TInt MceListId() const;
    
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
        virtual void SetSortTypeL( TInt aSortType, TBool aOrdering );

        /**
        * From CMceMessageListContainerBase 
        */
        virtual void SetMtmUiDataRegistry( MMtmUiDataRegistryObserver* aRegistryObserver );

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
        * Updates selection indexes by reading selected message's TMsvIds from iSelectedEntries
        * and creates new CSelectionIndexArray.
        * @param aForceUpdate: set ETrue if index update is wanted to be done even when
        *   listbox's selection does not include any items.
        *   Usually this is set true only when calling from CMceBaseListView::HandleEntriesDeletedL
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
        TBool IsItemRemote( TMsvId aItemId ) const;

        /**
        * Checks is item folder.
        * @param aIndex index of the item to be checked.
        * @return ETrue if item is folder.
        */
        TBool IsItemFolder(TInt aIndex) const;

        /**
        * From CMceMessageListContainerBase 
        */
        TBool IsOpenedFolder( TMsvId aFolderId ) const;
        
        /**
        * From CMceMessageListContainerBase 
        */
        void SetListEmptyTextL(const TDesC& aText);
        
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
        TBool DisplayOptionsMenuFromSelectionKey() const;
        
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
        * From CMceMessageListContainerBase 
        */
        void UpdateIconArrayL();

        /**
        * From CCoeControl
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;

        /**
        * From CMceMessageListContainerBase 
        */
        void SetContainerFlag( TMceContainerFlags aFlag, TBool aState );
		
        /**
        * From CMceMessageListContainerBase 
        */
        void SetMarkingModeOff();

    public:     // Functions from base classes

        /**
        * From CCoeControl
        */
        virtual TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType);        

            
        /**
        * From CCoeControl
        */
        void SizeChanged();
            
        /**
        * From 
        */
        void HandleResourceChange( TInt aType );
            
         /**
         * From MEikCommandObserver
         */
        virtual void ProcessCommandL(TInt aCommand);
         
        /**
        * From CCoeControl.
        * Handles pointer events.
        * @param aPointerEvent The pointer event. 
        */
        virtual void HandlePointerEventL( const TPointerEvent& aPointerEvent );
        
        /**
        * From CCoeControl
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        
    protected:  // Functions from base classes

        /**
        * From CMceListContainer
        */
//        virtual CEikTextListBox* CreateListBoxL( );        

        // From CCoeControl
        void FocusChanged(TDrawNow aDrawNow);
        
    private:

        /**
        * Symbian OS constructor.
        */
        void ConstructL(
            const TRect& aRect,
            CMsvSessionPtr aSession,
            CMceBitmapResolver& aBitmapResolver,
            TMsvId aFolderId );

        /**
        * Constructor is private.
        */
        CMceMessageListContainer(
            CMceMessageListView& aParentView );

            
        /**
         * Function that check if a key can be search
         * @param aPointerEvent The pointer event
         * @return ETrue if it can
         * since 3.2
         */
        TBool IsValidSearchKey(const TKeyEvent& aKeyEvent);

        /**
         * Handles a change in qwerty input mode setting.
         * @return KErrNone if ok.
         */
        TInt HandleQwertyModeChangeNotification();

        /**
         * Handles a change in qwerty input mode setting.
         * @return KErrNone if ok.
         */
        static TInt QwertyModeChangeNotification( TAny* aObj );


        /**
        * Adds item to iSelectedEntries
        * @param aEntryId: id of the entry to be added.
        */
        void AddItemToSelectionL( TMsvId aEntryId );

        /**
        * Removes item from iSelectedEntries
        * @param aEntryId: id of the entry to be removed.
        */
        void RemoveItemFromSelection( TMsvId aEntryId );

        /**
        * Handles focus when operation is completed. 
        * This is called by SetAnchorItemIdL when it gets
        * EMessageListOperationCompleted.
        */
        void HandleOperationCompletedL();
        
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



    private:
        CMceMessageListBox*            iListBox;
        CMceMessageListItemArray*          iListItems;
//        TMceListType                iListType;
        CMsvEntrySelection*         iSelectedEntries;
        TMsvId                      iAnchorItemId;
        TInt                        iAnchorItemIndex;
        
        TInt                        iCurrMskResource;

//        CMceMessageListView&            iOwningView;        
        CPtiEngine*                     iT9Interface;
        CMceMessageListContainerKeyTimer* iT9Timer;        

        CMcePropertySubscriber*     iQwertyModeStatusSubscriber;
        RProperty                   iQwertyModeStatusProperty;        
        TBool                       iPreviousInputMethod;
        TBool                       iInputMethodQwerty;
        
        TMessageListOperationType   iLastOperationType;
        TBool 			            iDialerEvent ;
        
        CMsvEntrySelection* 		iAddedMsvIds;
        
    };
    
#endif

// End of file
