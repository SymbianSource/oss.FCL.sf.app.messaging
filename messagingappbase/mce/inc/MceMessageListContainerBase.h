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
*     Pure virtua class for message view to handle listbox.
*
*/



#ifndef MCEMESSAGELISTCONTAINERBASE_H
#define MCEMESSAGELISTCONTAINERBASE_H

//  INCLUDES
#include <coecntrl.h> // CCoeControl
#include <msvapi.h>
//#include "MceListContainer.h"
//#include "MceMessageListContTimer.h"

#include "mcetemplate.h"
#include "MceUtils.h"
#include "MceListItem.h"
#include <centralrepository.h> //CRepository
#include <bldvariant.hrh>
#include <eikcmobs.h>
#include <e32property.h>

// FORWARD DECLARATIONS
class CMceMessageListView;
class MMtmUiDataRegistryObserver;

//CONSTS
//const TInt KMceSelectionReplacementSize = 5;
enum TMceContainerFlags
    {
    EMceOneRowFlagOptionsMenuOpen   = 0x01, // options menu open status
    EMceOneRowFlagFirstSubtitleOpen = 0x02
    };


// CLASS DECLARATION

/**
* Base class for all the message list containers. 
* This class is used by message list view.
* @since S60 5.0
*/
class  CMceMessageListContainerBase :
    public CCoeControl,
    public MEikCommandObserver
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~CMceMessageListContainerBase();

    public:
    
        /**
        * TODO: Needed?
        * 1 = two row
        * 2 = hierachical list
        */
        virtual TInt MceListId() const = 0;
    
        /**
        * @return number of items in list (total list item count, including ALL the items with one row list)
        */
        virtual TInt Count() const = 0;
        
        /**
        * @return message count
        */
        virtual TInt MessageCount() const = 0;
        
        /**
        * @return folder count
        * TODO: some other count also needed? Subtitle row count?
        */
        virtual TInt FolderCount() const = 0;
        
        /**
        * Changes currently opened folder
        * @param aFolderId: id of the folder to be opened.
        * TODO: should this open inbox in case of imap?
        */
        virtual void SetFolderL( TMsvId aFolderId ) = 0;
        
        /**
        * Returns id of the currently opened folder
        * @return folder id
        */
        virtual TMsvId CurrentFolderId() const = 0;
        
        /**
        * Returns the currently opened folder entry.
        * @return TMsvEntry of the opened folder.
        */
        virtual const TMsvEntry& FolderEntry() const = 0;

        
        /**
        * Returns message id of the currently focused item
        * @return TMsvId of the entry
        * TODO: should this be TInt so that it could be used to subtitles?
        */
        virtual TMsvId  CurrentItemId() const = 0;

        /**
        * TMessageListItemType
        * different list item types, CurrentItemType
        */
        enum TMessageListItemType
            {
            /**
            * unknown item (list is empty?)
            */
            EMessageListItemUnknown = 0,
            
            /**
            * Item can be found from message store (CurrentItemId() returns TMsvId)
            */
            EMessageListItemMsvItem,
            
            /**
            * something else ( one row list; subtitle?)
            */
            EMessageListItemSubtitle,
            
            /**
            * Tree list root
            */
            EMessageListItemRoot
            };
            
        /**
        * @return Type of the currently focused item
        */
        virtual TMessageListItemType CurrentItemType() const = 0;
        
        /**
        * Changes focus to the entry of the given id.
        * @return ETrue if aEntryId found otherwise EFalse.
        */
        virtual TBool SetCurrentItemIdL( TMsvId aEntryId ) = 0;
        
        /**
        * Changes focus to the first entry.
        */
        virtual void ResetCurrentItemL() = 0;
        
        
        /**
        * Searches aMatchString from the list and returns first found entry id.
        * Returns KErrNotFound if not found.
        * TODO: container should move focus automatically?
        */
        virtual TMsvId FindEntry( const TDesC& aMatchString ) const = 0;
        
        /**
        * Finds first unread message from the opened folder. 
        * aEntry is set KErrNotFound if no unread messages in the folder,
        * otherwise id of the first unread message.
        * @return ETrue if exactly one unread message in current folder
        * EFalse otherwise.
        */
        virtual TBool FindFirstUnreadMessageL( TMsvId& aEntryId ) const = 0;
        
        /**
        * TODO: that is not needed if container would be msv session observer
        */
        virtual void ItemCountChangedL( TBool aItemsAdded ) = 0;
        
        /**
        * Redraw the listbox.
        */
        virtual void RefreshListbox() = 0;
        
        /**
        * Returns array of currently selected (marked) messages.
        * (Focus is added, if no other markings)
        * Caller's responsibility to destroy array!
        * @return Array of selected items.
        */
        virtual CMsvEntrySelection* CurrentItemSelectionL() = 0;

        /**
        * Refreshes selection by calling MarkItemSelectionL and
        * then returns selection by calling CurrentItemSelectionL
        * Caller's responsibility to destroy array!
        * @return Array of selected items.
        */
        virtual CMsvEntrySelection* CurrentItemSelectionRefreshL() = 0;

        /**
        * Returns number of marked items including focus if no other selections.
        * @return Number of items marked, if no marked items then 1 is returned (focus).
        * 0 is returned only if list is empty.
        */
        virtual TInt CurrentItemSelectionCount() const = 0;
        
        /**
        * Returns number of selected/marked messages, focus is not included.
        * @return Number of items selected (0 or more)
        */
        virtual TInt SelectionCount() const = 0;
        
        /**
        * Returns ETrue if aItemId is marked.
        */
        virtual TBool IsItemSelected( TMsvId aItemId ) = 0;

        /**
        * Clears selection.
        */
        virtual void ClearSelection() = 0;

        /**
        * Reads selected item indexes from the listbox and then stores
        * selected items TMsvId's to iSelectedEntries array.
        * If no items marked then currently selected item is added to array
        *
        * This can be called when for example opening options menu. Options menu
        * can overlap whole list box and if then new message is received it is selected
        * on background. Now if user selected 'Delete' then message which was selected
        * before opening options menu is deleted, not just received message because
        * it is stored by this function.
        */
        virtual void MarkItemSelectionL() = 0;

        /**
        * Adds currently focused item to marked array.
        */
        virtual void AddCurrentItemToSelectionL() = 0;

        /**
        * Removes currently selected item (== 'cursor') from selected array
        */
        virtual void RemoveCurrentItemFromSelection() = 0;

        /**
        * TODO: update description for more generic. Now it is only for old list:
        * Updates selection indexes by reading selected message's TMsvIds from iSelectedEntries
        * and creates new CSelectionIndexArray.
        * @param aForceUpdate: set ETrue if index update is wanted to be done even when
        *   listbox's selection does not include any items.
        *   Usually this is set true only when calling from CMceBaseListView::HandleEntriesDeletedL
        */
        virtual void RefreshSelectionIndexesL( TBool aForceUpdate ) = 0 ;

        /**
        * Adds all the messages to listbox selection index array
        */
        virtual void AddAllToSelectionL() = 0;
        
        /**
        * Mark all read messages.
        */
        virtual void MarkAllReadMessagesL() = 0;
        
        /**
        * Return the count of marked items. if there is no marked item, returns 0;
        */
        virtual TInt MarkedItemsCount() = 0;

        /**
        * Checks if currently selected item is folder.
        * @return ETrue if item aItemId is folder.
        * NOTE: aItemId must be found from msg store. 
        *       if item not found, returns EFalse;
        */
        virtual TBool IsItemFolder( TMsvId aItemId ) const = 0;
        
        /**
        * Returns ETrue if aItemId is under remote service
        */
        virtual TBool IsItemRemote( TMsvId aItemId ) const = 0;

        /**
        * checks if aFolderId is currently opened
        * Return ETrue if currently opened folder is aFolderId
        * For example if needs to be checked if outbox is opened call:
        *   IsOpenedFolder( KMsvGlobalOutBoxIndexEntryId )
        */
        virtual TBool IsOpenedFolder( TMsvId aFolderId ) const = 0;

        /**
        * Sets sort order
        * @param aSortType: from MceCommands.hrh
        * @param aOrdering: sort ordering(ascending/descending)  
        */
        virtual void SetSortTypeL( TInt aSortType, TBool aOrdering ) = 0;
        
        /**
        *
        */
        virtual void SetMtmUiDataRegistry( MMtmUiDataRegistryObserver* aRegistryObserver ) = 0;

        /**
        * Sets empty text for the listbox.
        * @param aText: empty text
        */
        virtual void SetListEmptyTextL(const TDesC& aText) = 0;
        
        /**
        * TMessageListOperationType
        * different mailbox operation types to handle cursor 
        */
        enum TMessageListOperationType
            {
            /**
            *
            */
            EMessageListOperationGeneral = 0,
            
            /**
            *
            */
            EMessageListOperationConnect,
            
            /**
            *
            */
            EMessageListOperationFetchSelected,
            
            /**
            *
            */
            EMessageListOperationFetchNew,
            
            /**
            *
            */
            EMessageListOperationDelete,
            
            /**
            * Called when operation is completed
            * NOTE: this is not 100% to be called when operation is ready.
            *   For example imap4 create connection operation is not completed
            *   when connection is created, onle when connection is closed!
            */
            EMessageListOperationCompleted
            };

        
        /**
        * Called by application when operation is started.
        * List container can set anchor item or what ever it wants to keep 
        * focus in sensible place during the operation.
        * OPEN:
        * Problem is that operation completion is not informed so
        * listcontainer should handle that somehow...?
        */
        virtual void SetAnchorItemIdL( 
            TMessageListOperationType aOperationType ) = 0;
            
        /**
        * TODO: that is not needed if container would be msv session observer
        * TODO: remove ItemCountChangedL
        * @param aEvent: notified events: 
        *   EMsvEntriesDeleted, EMsvEntriesCreated, EMsvEntriesChanged
        * @param aSelection: CMsvEntrySelection of the entries changed/deleted/created.
        * @param aDrawNow: EFalse means CMceUi has ViewUpdateSuppressionFlag ON
        *   so listbox should not redraw itself, then only update anchor item etc.
        *   When operation is completed then container will get 
        *   SetAnchorItemIdL call with parameter EMessageListOperationCompleted.
        *   ETrue means that listbox can redraw itself if needed.
        * See also SetAnchorItemIdL
        */
        virtual void HandleMsvSessionEventL( 
            MMsvSessionObserver::TMsvSessionEvent aEvent,
            CMsvEntrySelection& aSelection,
            TBool aDraw ) = 0;
            
        /**
        * Returns ETrue if empty list or there are items selected.
        */
        virtual TBool DisplayOptionsMenuFromSelectionKey() const = 0;

        
        /**
        * Used to show different menu command offered by message list view
        * At the moment there are only Expand and Collapse options menu items 
        * but there can be more...
        */
        enum 
            {
            /**
            * Return TMessageListCommandNone if all the menu items dimmed,
            * otherwise corresponding flag value. 
            */
            TMessageListCommandNone = 0,
            /**
            * TMessageListCommandExpand: return this if expand command is visible.
            */
            TMessageListCommandExpand = 0x01,
            /**
            * TMessageListCommandCollapse1: return this if collapse, which is 
            *    immediately after expand command, is visible.
            */
            TMessageListCommandCollapse1 = 0x02,
            /**
            * TMessageListCommandCollapse2: return this if collapse, which is 
            *    after reply command, is visible.
            */
            TMessageListCommandCollapse2 = 0x04
            };
            
        /**
        * Return correct flag to set command visible in message 
        * list view's options menu. 
        * if expand valid command, then return TMessageListCommandExpand,
        * if collapse valid command, then return either TMessageListCommandCollapse1 
        *    or TMessageListCommandCollapse2 but NOT both.
        */
        virtual TUint OptionsCommandSupported() const = 0;
        
        /**
        *
        */
        virtual void HandleOptionsCommandL( TInt aCommand ) = 0;
        
        /**
        * Called when time or date format has been changed or 
        *   midnight is passed. 
        * @param aChangeFlag: flag defined in TChanges in e32const.h.
        *   Notifies only the following events:
        *   - EChangesLocale
        *   - EChangesMidnightCrossover
        *   - EChangesSystemTime
        */
        virtual void DateTimeFormatChangedL( TInt aChangeFlag ) = 0;

        /**
        * Returns container Msk
        */
        virtual TInt SetSpecialMskL() = 0;

        /**
        * Sets message list container flags
        * @param aFlag: Name of the flag
        * @param aState: State for the flag
        */
        virtual void SetContainerFlag( TMceContainerFlags aFlag, TBool aState ) = 0;

        /**
        *  Updates icon array in case MTM is released (one row list)
        */
        virtual void UpdateIconArrayL() = 0;
        
    protected:
    
        /**
        *
        */
        CMceMessageListContainerBase( CMceMessageListView& aOwningView );
        
    protected:

        CMceMessageListView&            iOwningView;        

    };
    

#endif

// End of file
