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



#ifndef MCEMAINVIEWLISTITEMARRAY_H
#define MCEMAINVIEWLISTITEMARRAY_H

//  INCLUDES

#include <bldvariant.hrh>
#include <msvapi.h>     // MMsvSessionObserver
#include <bamdesca.h>   // MDesCArray
#include <badesca.h>    // CDesCArrayFlat


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
class CMceLogEngine;
class CRepository;
class CCoeEnv;

/**
* Main view list item array observer.
* Called when main view list item texts are updated
* @since S60 5.0
*/
class MMceMainViewListItemArrayObserver
    {
    public:
        /**
        * Called when idle loader has changed main view text
        * TODO: add what has happened, add, remove, update etc.?
        */
        virtual void MceListItemArrayChangedL(
        ) = 0;
    };
    

// CLASS DECLARATION

/**
*  Item array for the message items to be displayed by mce.
*  Main view items are kept in memory (CArrayFixFlat<TMceListItem>) when created but
*  Message view list items are fetched from the msgs when needed in MdcaPoint function
*/
class CMceMainViewListItemArray :
    public CArrayFixFlat<TMceListItem>,
    public MMsvSessionObserver,
    public MDesCArray,
    public MMceUtilsIdleClassObserver   
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CMceMainViewListItemArray* NewL(
            CMsvSessionPtr aSession,
            TMsvId aFolderId,
            TMceListType aListType,
            CMceBitmapResolver& aBitmapResolver );

        /**
        * Destructor.
        */
        virtual ~CMceMainViewListItemArray();

    public:

        /**
        * Returns list index of the message
        * @param aItemId: item to be searched.
        * @return Index of the item.
        */
        TInt ItemIndex( TMsvId aItemId ) const;

        
        /**
        * Returns list index of the extra item:
        *   KMceWriteMessageListIdValue
        *   KMceNoMailboxesListIdValue
        *   KMceDeliveryReportsListIdValue
        * @param aItemId: item to be searched.
        * @return Index of the item.
        * @since S60 v5.0
        */
        TInt ExtraItemIndex( TInt aItemId ) const;

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
        
        /**
        * Returns main view's extra item command text.
        * See more details of main view extra items from
        * mceextraitems project. 
        * @param aCommandId: command id received from the menu (CMceMainViewListView::StartAppL)
        * @return: command text buffer. for example 'http://www.nokia.com'.
        *    if aCommandId not found, then empty buffer returned.
        * @since S60 v5.0
        */ 
        HBufC* ExtraItemCommandLC( TInt aCommandId );

private:        
        /**
        * Add item not in msgs (CBS, ...).
        * @param aNewItem: TMceListItem to be added to the list
        * @param aIndex: index in the list to be added (0 = first)
        */
        void AddExtraItemL( TMceListItem& aNewItem, TInt aIndex );

        /**
        * Add item not in msgs (CBS, ...).
        * @param aNewItem: TMceListItem to be added to the list
        * @param aIndex: index in the list to be added (0 = first)
        * @param aIconIndex: index of the icon in file MceExtraIcons
        * @param aApplicationUid: Related Application uid. This is used 
        *        when loading correct application icon. If this is not set, 
        *        then icon is loaded from MceExtraIcons.
        */
        void AddExtraItemWithIconL( 
            TMceListItem& aNewItem, 
            TInt aIndex, 
            TInt aIconIndex,
            TUid aApplicationUid );

public:
        /**
        * Help needs info, if remote folder or own folder
        */
        TBool IsFolderRemote() const;        

        /**
        * Checks if mail account is always online
        * @param aAccount: service id
        * @param aAlwaysOnlineSettings: CIMASettingsDataExtension reference
        * Returns ETrue, if always online account 
        */
        TBool AlwaysOnlineAccountL( 
            const TMsvId aAccount, 
            const CImumInSettingsData& aAlwaysOnlineSettings ) const;
        
        /**
        * Sets roaming flag
        * @param aRoaming: roaming flag         
        */
        void SetRoaming( TBool aRoaming );
       
        /**
        * Gets roaming flag            
        */
        TBool Roaming() const;

        /**
        * Sets always online flag
        * @param aAlwaysOnline: always online flag         
        */
        void SetAlwaysOnline( TBool aAlwaysOnline );

        /* 
        * Gets results from MceUtils::GetHealthyMailboxListL
        * @para aCount:  iMailboxArray.Count()
        * @return err from MceUtils::GetHealthyMailBoxL()
        * from 3.2
        */
        TInt GetHealthyMailboxListResult( TInt& aCount );


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
        void ConstructMainViewL();

        /**
        * Help needs folder id
        */
        TMsvId FolderId() const;        

        /**
        * Help needs info, if remote account is connected
        */
        TBool IsConnected() const;
        
        /**
         * Returns the pointer to Imum's internal email API        
         * 
         * @since S60 v3.2
         * @return pointer to email API
         */
        CImumInternalApi* GetEmailApi();

        /**
        *
        */
        void ResetIconIndexL( TInt aIndex );
        
        /**
        *
        */
        void StartListUpdateL();
        
        TBool MMceUtilsIdleClassStepL();
        void SetListItemArrayObserver( MMceMainViewListItemArrayObserver* aObserver );
    
    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        *
        */
        CMceMainViewListItemArray(
            CMsvSessionPtr aSession,
            TMsvId aFolderId,
            TMceListType aListType,
            CMceBitmapResolver& aBitmapResolver );

        /**
        * Adds TMsvEntry to the array (used only in Main view).
        * @param aEntry TMsvEntry of the item to be added.
        * @return nothing
        */
        void AddItemL( const TMsvEntry& aEntry );

        /**
        * Adds main view folders to the array (used only in Main view)
        */
        void AddFoldersL();

        /**
        * Adds folders Inbox and Documents to the array
        * (used only in Main view).
        */
        void AddInboxDocumentsL();

        /**
        * Adds folders Draft, Sent, Outbox to the array
        * (used only in Main view).
        */
        void AddDraftSentOutboxL();

        /**
        * Appends aSourceString to aTargetString and changes KColumnListSeparator
        * characters to space characters.
        * @param aTargetString buffer to append aSource string.
        * @param aSourceString source buffer.
        */
        void StripAndAppendString(TDes& aTargetString, const TDesC& aSourceString) const;

        /**
        * Creates string for main view listbox. Called by MdcaPoint.
        * @param aIndex: index of the message to be drawn
        */
        TPtrC MakeStringForMainView( TInt aIndex ) const;

        /**
        * Checks if always online account and always online date/time valid
        * @param aAccount: service id
        * @return ETrue,if always online account and always online date/time valid
        */
        TBool AlwaysOnlineDateTimeValidL( const TMsvId aAccount, TBool &aoDefined ) const;


        /**
        * Handles folders, messages string
        * @param aBuffer: buffer to be append
        * @param aEntry TMsvEntry of the item.
        */
        void HandleFolderMessagesStringL( TDes& aBuffer, const TMsvEntry& aEntry ) const; 
    
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
        void MakeItemL( 
            TMceListItem& aListItem, 
            const TMsvEntry& aEntry ) const;

        /**
         * 
         *
         * @since S60 v3.1
         * @param 
         * @return 
         * @leave 
         */
        void InsertItemL( 
            const TMsvEntry& aEntry, 
            const TInt aIndex );

        /**
         * Checks if the entry can be added to list
         *
         * @since S60 v3.1
         * @param aEntry Entry to be checked
         * @param aId Id to be checked
         * @return ETrue when the entry can be added to list
         * @return EFalse when the entry can NOT be added to list
         */
        TBool ShouldAddToListL( 
            const TMsvEntry& aEntry,
            const TMsvId aId );


        /**
         * Gathers all the services that needs to be shown to the list
         *
         * @since S60 v3.1
         * @leave KErrNoMemory when failed to add item to list
         * @return Index of the draft folder
         */
        TInt CollectServicesL( const TMsvId aId );

        /**
         * 
         *
         * @since S60 v3.1
         * @param 
         * @return 
         * @leave 
         */
        void AddMailboxesL( const TInt aMailboxIndex );
        
        /**
        *
        */
        void AppendMessageCountString( TDes& aBuffer, TInt aMessageCount) const;

        
        
        /**
        * Adds items 'Write message', 'Mailbox' and 'Delivery reports'
        * from the string resource. Note that this is different
        * than iExtraItemsArray despite naming...
        * @since S60 v5.0
        */
        void AddExtraItemFromResourceL( TInt aResourceId, TInt aCommandId );

    
        /**
        * Adds items from mceextraitems resource file to iExtraItemsArray.
        * That is done only on construct. If main view items needs to be refreshed
        * then items are read from iExtraItemsArray and no need to 
        * open resource again.
        * @since S60 v5.0
        */
        void ReadExtraItemsResourceL();
        
        /**
        * Adds mceextraitem from iExtraItemsArray to actual main view array.
        * @param: aIndex: index of the item in iExtraItemsArray.
        * @since S60 v5.0
        */         
        void AddExtraItemCommandL( TInt aIndex );

        /**
        * Adds TMsvEntry item from message store to main view array
        * @param: aEntryId: id of the entry to be added.
        * @since S60 v5.0
        */
        void AddFolderItemL( TMsvId aEntryId );
        
        /**
        * Reads main view item order from the central repository.
        * Order of the items are stored in iMainViewItems so
        * when refresh is needed, no need to open cenrep file again.
        * @since S60 v5.0
        */
        void ReadMainViewItemsFromRepositoryL();
    
        //Calculates the visible count excluding FS email stuff
        TInt FindVisibleCountL(TMsvId serviceId) const;
    private:    // Data
        CMsvSessionPtr                  iSession;
        CMsvEntry*                      iFolderEntry;
        CMceBitmapResolver&             iBitmapResolver;
        MMtmUiDataRegistryObserver*     iRegistryObserver;
        HBufC*                          iListBoxText;
        CDesCArrayFlat*                 iNumberOfMessagesStrings;
        TBool                           iAlwaysOnline;
        TBool                           iRoaming;
        // Array of mailbox Ids
        MImumInHealthServices::RMailboxIdArray iMailboxArray;        
        // Owned: Pointer to email API object
        CImumInternalApi*               iEmailApi;
        TInt                            iErrGetHealthyMailBoxList;
        TInt                            iMaiboxArrayCount;
        
        CMceUtilsIdleClass*             iIdleLoader;
        TInt                            iIdleCounter;
        MMceMainViewListItemArrayObserver*      iListItemArrayObserver;
        
        CCoeEnv*                        iCoeEnv;
        CMtmUiDataRegistry*             iUiRegistry;
        CMsvEntrySelection*             iMainViewItems;
        CMceMainViewExtraItemArray*     iExtraItemsArray;
        
        TInt                            iDescriptionLength;
        //FS Email stuff
        TBool                           iSelectableEmail;
        
        /**
         * CMail Changes, it will be True if 
         * KFeatureIdFfEmailFramework is enabled
         */
        TBool                           iEmailFramework;        
    };

#endif      // MCEMAINVIEWLISTITEMARRAY_H

// End of File
