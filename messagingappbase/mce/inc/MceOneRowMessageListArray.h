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
*     Item array for the message items to be displayed by mce.
*     Main view items are kept in memory (CArrayFixFlat<TMceListItem>) when created but
*     Message view list items are fetched from the msgs when needed in MdcaPoint function
*
*/



#ifndef MCEONEROWLISTITEMARRAY_H
#define MCEONEROWLISTITEMARRAY_H

//  INCLUDES

#include "mcetemplate.h"
#include <badesca.h>    // CDesCArrayFlat
#include <msvapi.h>     // MMsvSessionObserver
#include <bldvariant.hrh>
#include <PhCltTypes.h>
#include <akntreelistconstants.h>
#include <akncustomtreeordering.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CMsvSession;
class CMceOneRowBitmapResolver;
class MMtmUiDataRegistryObserver;
class CMceOneRowListItemArrayLoader;
class CCoeEnv;
class THCListListItem;


enum THCListListType
    {
    EHCListTypeNone = 0,
    EHCListTypeTime,
    EHCListTypeSize,
    EHCListTypeSender,
    EHCListTypeSubject,
    EHCListTypeType
    };
    
const TInt KOneRowListColumnTextSize = 30;


// CLASS DECLARATION
class MMceOneRowListItemArrayObserver 
    {
    public:
        virtual void MceListItemArrayChangedL(
            CArrayFixFlat<TInt>& aChangedItems ) = 0;
    };


/**
*  Item array for the message items to be displayed by mce.
*  Main view items are kept in memory (CArrayFixFlat<TMceListItem>) when created but
*  Message view list items are fetched from the msgs when needed in MdcaPoint function
*/
class CMceOneRowListItemArray :
    public CBase,
    public MMsvSessionObserver,
    public MAknCustomTreeOrdering
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CMceOneRowListItemArray* NewL(
            CMsvSessionPtr aSession,
            TMsvId aFolderId );

        /**
        * Destructor.
        */
        virtual ~CMceOneRowListItemArray();

    public:

        /**
        * Sets MMceListItemArrayObserver
        * Needed to get notify when MsgStore has changed
        * @param aObserver: Listitem observer
        */
        void SetListItemArrayObserver( MMceOneRowListItemArrayObserver* aObserver );

        /**
        * Changes folder of the list.
        * @param aFolderId: folder id to be changed.
        */
        void SetFolderL( TMsvId aFolderId );

        /**
        * Currently open folder
        */
        TMsvId FolderId();
        const TMsvEntry& FolderEntry() const;

        /**
        * Returns message id of the list item.
        * @param aIndex: index of the item to be returned
        * @return message id (TMsvId)
        */
        TMsvId ItemId( TInt aIndex );

        /**
        * Returns list index of the message
        * @param aItemId: item to be searched.
        * @return Index of the item.
        */
        TInt ItemIndex( TMsvId aItemId ) const;

        /**
        * Sets sort order 
        * @param aOrdering: 
        */
        void SetOrderingL( TInt aType, TBool aOrdering );

        /**
        * Checks, if Delete should be discarded in outbox
        * @param aItemId: id of the item to be checked
        * @return ETrue, if Delete should be discarded
        */
        TBool DeleteDiscard( TMsvId aItemId ) const;

        /**
        * returns sort type
        */
        TMsvSelectionOrdering SortType();

        /**
        * Checks valid phone number
        * @param aEntryId: message id
        * @param aEntry: message entry
        * @param aNumber: phone number
        * @return ETrue, if valid phone number, in other case return EFalse
        */
        TBool CheckValidNumberL( const TMsvId aEntryId, TMsvEntry& aEntry, 
            TPhCltTelephoneNumber& aNumber ) const;
            
        /**
        * Get entry
        */
        TInt GetEntry( TMsvId aEntryId, TMsvEntry& aEntry );

        /**
        * Get entry
        */
        TInt GetEntry( TInt aIndex, TMsvEntry& aEntry );

        /**
        * Get subtitle text
        */
        TInt GetSubtitleTextL( TInt aIndex, TDes& aTitleText);

        /**
        * Get column text
        */
        TInt GetColumnTextL( TInt aIndex, TInt aColumn, TDes& aTitleText);

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
        * Reset one tree item from the array
        * @param aTreeItemId: id of the item to be reseted
        */
        void ResetTreeItemId( TAknTreeItemID aTreeItemId );

        /**
        * Reset all tree items from the array
        */
        void ResetAllTreeItemId();

        /**
        * Finds first unread message from the opened folder
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
        * Returns message count
        * @return: Count
        */
        TInt MessageCount();

        /**
        * Sets MtmUiDataRegistryObserver
        * Needed only by Outbox list to get status of the message
        * @param aRegistryObserver: registry observer (usually object of CMceUi)
        * @return none
        */
        void SetRegistryObserver( MMtmUiDataRegistryObserver* aRegistryObserver );


        /**
        * Check if folder is remote
        */
        TBool IsFolderRemote() const;


        /**
        * Check if item is remote
        */
        TBool IsItemRemote( TMsvId aItemId ) const;


        /**
        * Check connetion status
        * @return: connection status
        */
        TBool IsConnected() const;

        /**
        * Finds index of entry
        */
        TInt FindEntryIndex( const TDesC& aMatchString ) const;

        /**
        * Rebuild array
        */
        void RefreshArrayL();

        /**
        * Returns how many items created.
        */
        TInt EntryCreatedL( TMsvId aEntryId );
        
        /**
        * 
        * @param aEntryId: entry that was deleted.
        * @param aTreeItemId: deleted entry tree id, 0 if not found
        * @param aParentTreeItemId: parent id, 0 if parent was not deleted.
        * @return: Is subtitle deleted
        */
        TBool EntryDeletedL( 
            TMsvId aEntryId, 
            TAknTreeItemID& aTreeItemId, 
            TAknTreeItemID& aParentTreeItemId );


    public:     // Functions from base classes


        /**
        * From MDesCArray
        */
        virtual TInt MdcaCount() const;

        /**
        * Compares two items
        */
        virtual TInt Compare( TAknTreeItemID aFirst, TAknTreeItemID aSecond );

        /**
        * Returns index of the item in the list
        */
        TInt ItemIndexTreeId( TAknTreeItemID aTreeItemId );

        /**
        * Set item mark
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
        * Returns item type 
        * @return: Item type 
        */        
        TInt HCItemType( TInt aIndex );
//        THListItemType HCItemType( TInt aIndex );

        /**
        * Returns subfolder item count
        * @param aParent: subfolder Id
        * @return: number of items
        */
        TInt GetSubfolderItemCount( TAknTreeItemID aParent );

        /**
        * Saves marked items
        * @param: aMarked Array of marked tree items id's
        */
        void SaveMarkingL(  RArray<TAknTreeItemID>& aMarked );
        
        /**
        * Checkes whether entry belongs to today or not
        * @param: aEntry TMsvEntry of the item (entry to be checked)
        */
        TBool IsTodaysEntry(const TMsvEntry& aEntry) const;

    public:    // Functions from base classes
        /**
        * From MMsvSessionObserver
        */
        virtual void HandleSessionEventL(
            TMsvSessionEvent aEvent,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3);

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        *
        */
        CMceOneRowListItemArray(
            CMsvSessionPtr aSession,
            TMsvId aFolderId );

        /**
        * Appends aSourceString to aTargetString and changes KColumnListSeparator
        * characters to space characters.
        * @param aTargetString buffer to append aSource string.
        * @param aSourceString source buffer.
        */
        void StripAndAppendString(TDes& aTargetString, const TDesC& aSourceString) const;

        /**
        * Appends folder entry string to the buffer.
        * @param aBuffer: buffer to be append
        * @param aEntry: entry to be append. Must be folder.
        */
        void AppendFolderString( TDes& aBuffer, const TMsvEntry& aEntry ) const;

        /**
        * Appends recipient string to the buffer.
        * @param aBuffer: buffer to be append
        * @param aEntry: entry to be append. Must be message.
        */
        void AppendRecipientStringL( TDes& aBuffer, const TMsvEntry& aEntry ) const;

        /**
        * Handles folders, messages string
        * @param aBuffer: buffer to be append
        * @param aEntry TMsvEntry of the item.
        */
        void HandleFolderMessagesStringL( TDes& aBuffer, const TMsvEntry& aEntry ) const;

        /**
        * Appends Bt/IrDa message entry string to the buffer.
        * @param aBuffer: buffer to be append
        * @param aEntry: entry to be append. Must be message.
        */
        void AppendBtIrMessageStringL( TDes& aBuffer, const TMsvEntry& aEntry ) const;
        
        /**
        * Get timezone correction to entry time 
        * @param aEntryTime: 
        */
        void GetTime( TTime& aEntryTime, TTime& aCurrentTime ) const;

        /**
        * Creates time or date string for message view listbox.
        * @param aText: buffer to be append
        * @param aEntry: message entry
        * @param aSubtitle: title is present or not
        * @param aShowTime: time to be shown or not
        */
        void AppendTimeStringL( TDes& aText, const TMsvEntry& aEntry, TBool aSubtitle, TBool aShowTime = ETrue ) const;
        
        /**
        * Returns whether the entry is Template folder (ETrue) or not (EFalse).
        * @param aEntry: message entry
        * @param aText: buffer to be append
        * @param aColumn: cloumn to be checked
        */
        TBool IsFolderTemplate( const TMsvEntry& aEntry, TDes& aText, TInt aColumn );

    private:    // Data
        CMsvSessionPtr                  iSession;
        TMsvId                          iFolderId;
        CDesCArrayFlat*                 iNumberOfMessagesStrings;
        CMceOneRowListItemArrayLoader*  iMceListItemArrayLoader;
        CCoeEnv*                        iOwnEikonEnv;

        // String array        
        CDesCArrayFlat*                 iSubtitleStrings;
        MMtmUiDataRegistryObserver*     iRegistryObserver;
        TBool                           iShowDate;
        TInt                            iDescriptionLength;

    };

#endif      // MCEONEROWLISTITEMARRAY_H


// End of File
