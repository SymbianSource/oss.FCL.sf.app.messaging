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



#ifndef MCEMESSAGELISTITEMARRAY_H
#define MCEMESSAGELISTITEMARRAY_H

//  INCLUDES

#include <msvapi.h>     // MMsvSessionObserver
#include <bldvariant.hrh>
#include "MceListItem.h"
#include "MceUtils.h"
#include "MceUtilsIdleClass.h"

#include "mcetemplate.h"
#include <PhCltTypes.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CMsvSession;
class CMceBitmapResolver;
class MMtmUiDataRegistryObserver;
class CMtmUiDataRegistry;
class CImumInternalApi;
class CImumInSettingsData;

// CLASS DECLARATION

/**
*  Item array for the message items to be displayed by mce.
*  Main view items are kept in memory (CArrayFixFlat<TMceListItem>) when created but
*  Message view list items are fetched from the msgs when needed in MdcaPoint function
*/
class CMceMessageListItemArray :
    public CBase, //CArrayFixFlat<TMceListItem>,
    public MMsvSessionObserver,
    public MDesCArray
//    public MMceUtilsIdleClassObserver
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CMceMessageListItemArray* NewL(
            CMsvSessionPtr aSession,
            TMsvId aFolderId,
            TMceListType aListType,
            CMceBitmapResolver& aBitmapResolver );

        /**
        * Destructor.
        */
        virtual ~CMceMessageListItemArray();

    public:

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
        * @param aIndex
        * @return TMceListItem
        */
        const TMceListItem Item( TInt aIndex ) const;

        /**
        * Changes folder of the list.
        * @param aFolderId: folder id to be changed.
        */
        void SetFolderL( TMsvId aFolderId );
        
        
        TInt MessageCount();
        TInt FolderCount();
        
        /**
        *
        */
        TInt GetEntry( TMsvId aEntryId, TMsvEntry& aEntry );

        /**
        * Add item not in msgs (CBS, ...).
        * @param aNewItem: TMceListItem to be added to the list
        * @param aIndex: index in the list to be added (0 = first)
        */
//        void AddExtraItemL( TMceListItem& aNewItem, TInt aIndex );

        /**
        * Add item not in msgs (CBS, ...).
        * @param aNewItem: TMceListItem to be added to the list
        * @param aIndex: index in the list to be added (0 = first)
        * @param aIconIndex: index of the icon in file MceExtraIcons
        * @param aApplicationUid: Related Application uid. This is used 
        *        when loading correct application icon. If this is not set, 
        *        then icon is loaded from MceExtraIcons.
        */
/*        void AddExtraItemWithIconL( 
            TMceListItem& aNewItem, 
            TInt aIndex, 
            TInt aIconIndex,
            TUid aApplicationUid );
*/
      /**
        * Check if item is folder
        * @param aItemId: id of the item to be checked
        * @return ETrue if item is folder
        */
        TBool IsItemFolder( TMsvId aItemId ) const;

      /**
        * Sets MtmUiDataRegistryObserver
        * Needed only by Outbox list to get status of the message
        * @param aRegistryObserver: registry observer (usually object of CMceUi)
        * @return none
        */
        void SetRegistryObserver( MMtmUiDataRegistryObserver* aRegistryObserver );

        /**
        * Finds entry index in message list
        * @param aMatchString: the string which is searched
        */
        TInt FindEntryIndex( const TDesC& aMatchString ) const;

        /**
        * Sets sort order in folder entry
        * @param aOrdering: the sort order
        */
        void SetOrderingL( TInt aOrdering );

        /**
        * Help needs info, if remote folder or own folder
        */
        TBool IsFolderRemote() const;        
        TBool IsItemRemote( TMsvId aItemId ) const;

        /**
        * Checks, if Delete should be discarded in outbox
        * @param aItemId: id of the item to be checked
        * @return ETrue, if Delete should be discarded
        */
        TBool DeleteDiscard( TMsvId aItemId ) const;

        /**
        * Sets roaming flag
        * @param aRoaming: roaming flag         
        */
//        void SetRoaming( TBool aRoaming );
       
        /**
        * Gets roaming flag            
        */
//        TBool Roaming() const;

        /**
        * Sets always online flag
        * @param aAlwaysOnline: always online flag         
        */
//        void SetAlwaysOnline( TBool aAlwaysOnline );

        /**
        * Sets sort order
        * @param aSortType: sort type
        * @param aOrdering: sort ordering(ascending/descending)  
        */
        void SetOrderingL( TInt aSortType, TBool aOrdering );

        /**
        * returns sort type
        */
        TMsvSelectionOrdering SortType();

#ifdef RD_READ_MESSAGE_DELETION 
        /**
        * returns folder's parent folder Id
        */
        TMsvId GetParentFolderId() const; 
#endif //RD_READ_MESSAGE_DELETION 

    public:     // Functions from base classes

        /**
        * From MDesCArray
        */
        virtual TInt MdcaCount() const;

        /**
        * From MDesCArray
        */
        virtual TPtrC MdcaPoint(TInt aIndex) const;

    public:    // Functions from base classes
        /**
        * From MMsvSessionObserver
        */
        virtual void HandleSessionEventL(
            TMsvSessionEvent aEvent,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3);

        /**
        * Reconstructs main view folder list
        */
//        void ConstructMainViewL();

        /**
        * Currently opened folder id
        */
        TMsvId FolderId() const;        
        
        
        /**
        *
        */
        const TMsvEntry& FolderEntry() const;

        /**
        * Help needs info, if remote account is connected
        */
        TBool IsConnected() const;
        
        /**
        *
        */
//        void ResetIconIndexL( TInt aIndex );
        
        /**
        *
        */
//        void StartListUpdateL();
        
//        TBool MMceUtilsIdleClassStepL();
//        void SetListItemArrayObserver( MMceListItemArrayObserver* aObserver );


        /*
        *
        */
        TBool FindFirstUnreadMessageL( TMsvId& aEntryId ) const;
    
    
    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        *
        */
        CMceMessageListItemArray(
            CMsvSessionPtr aSession,
            TMsvId aFolderId,
            TMceListType aListType,
            CMceBitmapResolver& aBitmapResolver );

        /**
        * Adds TMsvEntry to the array (used only in Main view).
        * @param aEntry TMsvEntry of the item to be added.
        * @return nothing
        */
//        void AddItemL( const TMsvEntry& aEntry );

        /**
        * Adds folders to the array (used only in Main view)
        * @param aId id of the folder
        */
//        void AddFoldersL( TMsvId aId );

        /**
        * Adds folders Inbox and Documents to the array
        * (used only in Main view).
        */
//        void AddInboxDocumentsL();

        /**
        * Adds folders Draft, Sent, Outbox to the array
        * (used only in Main view).
        */
//        void AddDraftSentOutboxL();

        /**
        * Creates send status string.
        * @param aText buffer status string is put.
        * @param aEntry TMsvEntry of the item.
        */
        void MakeSendStatusStringL(TDes& aText, const TMsvEntry& aEntry) const;

        /**
        * Appends aSourceString to aTargetString and changes KColumnListSeparator
        * characters to space characters.
        * @param aTargetString buffer to append aSource string.
        * @param aSourceString source buffer.
        */
        void StripAndAppendString(TDes& aTargetString, const TDesC& aSourceString) const;

        /**
        * Creates string for message list view listbox. Called by MdcaPoint.
        * @param aIndex: index of the message to be drawn
        */
        TPtrC MakeStringForMessageView( TInt aIndex ) const;

        /**
        * Creates string for main view listbox. Called by MdcaPoint.
        * @param aIndex: index of the message to be drawn
        */
//        TPtrC MakeStringForMainView( TInt aIndex ) const;

        /**
        * Appends folder entry string to the buffer.
        * @param aBuffer: buffer to be append
        * @param aEntry: entry to be append. Must be folder.
        */
        void AppendFolderString( TDes& aBuffer, const TMsvEntry& aEntry ) const;

        /**
        * Appends message entry string to the buffer.
        * @param aBuffer: buffer to be append
        * @param aEntry: entry to be append. Must be message.
        */
        void AppendMessageString( TDes& aBuffer, const TMsvEntry& aEntry ) const;


        /**
        * Appends recipient string to the buffer.
        * @param aBuffer: buffer to be append
        * @param aEntry: entry to be append. Must be message.
        */
        void AppendRecipientStringL( TDes& aBuffer, const TMsvEntry& aEntry ) const;
        
        /**
        * Checks if always online account and always online date/time valid
        * @param aAccount: service id
        * @return ETrue,if always online account and always online date/time valid
        */
//        TBool AlwaysOnlineDateTimeValidL( const TMsvId aAccount, TBool &aoDefined ) const;


        /**
        * This performs number grouping to the given phone number if required.
        *
        * @param aPhoneNumber (in/out): A valid phone number to be reformated
        * with number grouping. If number grouping feature is turned on and
        * the give phone number does not already exceed the maximum displayable
        * length for number grouping, the reformated phone number with number
        * grouping is returned. Otherwise, the phone number remains unchanged.
        *
        * NOTE: Caller must validate the phone number and pass the phone
        *       number if it's valid.
        * @return none
        * @since 2.7
        */
        void DoNumberGroupingL( TDes& aPhoneNumber ) const;

        /**
        * Creates inbox status string for MMS notification
        * @param aText buffer status string is put.
        * @param aEntry TMsvEntry of the item.
        */
        void MakeInboxStatusStringL( TDes& aText, const TMsvEntry& aEntry ) const;

        /**
        * Handles folders, messages string
        * @param aBuffer: buffer to be append
        * @param aEntry TMsvEntry of the item.
        */
        void HandleFolderMessagesStringL( TDes& aBuffer, const TMsvEntry& aEntry ) const; 
    
        /**
        * Removes the prefixes from the given text string
        * @param aTextString: Text string
        */
        TInt SkipPrefix( TPtrC aTextString ) const;
        
        /**
        * Saves new orderding to root entry
        * @param aNewOrdering
        * @since 3.0
        */
        void SaveOrderingL( const TMsvSelectionOrdering& aNewOrdering ) const;

        /**
        * Appends Bt/IrDa message entry string to the buffer.
        * @param aBuffer: buffer to be append
        * @param aEntry: entry to be append. Must be message.
        */
        void AppendBtIrMessageStringL( TDes& aBuffer, const TMsvEntry& aEntry ) const;

        /**
         * 
         *
         * @since S60 v3.2
         * @param 
         * @return 
         * @leave &(leavedesc)s
         */
        void StoreIndex( const TInt aIndex );
    
        /**
         * 
         *
         * @since S60 v3.1
         * @param 
         * @return 
         * @leave 
         */
/*        void MakeItemL( 
            TMceListItem& aListItem, 
            const TMsvEntry& aEntry ) const;
*/
        /**
         * 
         *
         * @since S60 v3.1
         * @param 
         * @return 
         * @leave 
         */
/*        void InsertItemL( 
            const TMsvEntry& aEntry, 
            const TInt aIndex );
*/
        /**
         * Checks if the entry can be added to list
         *
         * @since S60 v3.1
         * @param aEntry Entry to be checked
         * @param aId Id to be checked
         * @return ETrue when the entry can be added to list
         * @return EFalse when the entry can NOT be added to list
         */
/*        TBool ShouldAddToList( 
            const TMsvEntry& aEntry,
            const TMsvId aId );
*/

        /**
         * Gathers all the services that needs to be shown to the list
         *
         * @since S60 v3.1
         * @leave KErrNoMemory when failed to add item to list
         * @return Index of the draft folder
         */
//        TInt CollectServicesL( const TMsvId aId );

        /**
         * 
         *
         * @since S60 v3.1
         * @param 
         * @return 
         * @leave 
         */
//        void AddMailboxesL( const TInt aMailboxIndex );
        
        /**
        *
        */
        void AppendMessageCountString( TDes& aBuffer, TInt aMessageCount) const;
        
        /**
        *
        */
        void AppendTimeStringL( TDes& aText, const TMsvEntry& aEntry ) const;

        //FS Email stuff
        TInt CreateEntryIndexL() const;
        TInt GetEntryIndex( TInt aIndex ) const;
        
        /**
         * Check the MTM technology type for Mail Technology 
         * @param aMtm: Mtm Uid
         * @return : ETrue if Mtm Technology is Mail
         *           otherwsie EFalse
         */
        TBool IsMailMtmTechnology(TUid aMtm ) const ;
    
    private:    // Data
        CMsvSessionPtr                  iSession;
        CMsvEntry*                      iFolderEntry;
        TMsvId                          iFolderId;
        TMceListType                    iListType;
        CMceBitmapResolver&             iBitmapResolver;
        MMtmUiDataRegistryObserver*     iRegistryObserver;
        HBufC*                          iListBoxText;
        CDesCArrayFlat*                 iNumberOfMessagesStrings;
        TBool                           iAlwaysOnline;
        TBool                           iRoaming;
        TBool                           iNumberGroupingEnabledFlag;
        CMtmUiDataRegistry*             iUiRegistry;
        TMsvSelectionOrdering           iPreviousSelectionOrdering;
        TInt                            iDescriptionLength;
        // ETrue if MCE integration feature is ON. In practice, this means
        // that S60 mailboxes are hided from MCE main view
        TBool iEmailClientIntegration;
        
        // ETrue if there is other than platform email application registered
        // to handle S60 mailboxes
        TBool iIntegratedEmailAppExcist;
        CArrayFixFlat<TInt>*            iMsgIndex;
        
        /**
         * CMail Changes, it will be ETrue if 
         * KFeatureIdFfEmailFramework is enabled
         */
        TBool  iEmailFramework;          
    };

#endif      // MCEMESSAGELISTITEMARRAY_H

// End of File
