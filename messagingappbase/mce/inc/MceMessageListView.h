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
*     This view is created when user opens folder.
*
*/



#ifndef MCEMESSAGELISTVIEW_H
#define MCEMESSAGELISTVIEW_H

//  INCLUDES
//#include "MceBaseListView.h"
#include <aknview.h>
#include <msvapi.h>
#include <AiwServiceHandler.h>
#include <coemain.h>

#include "MceMessageListContainer.h" 

#include "mcetemplate.h"
#include "MceFolderItem.h"
#include "MceListboxTypeObserver.h"

#include <bldvariant.hrh>

#include <AiwServiceHandler.h>

// CONSTANTS

#define TDRVIEWID TVwsViewId( \
    TUid::Uid( KMceApplicationUidValue ), \
    TUid::Uid( KMceDeliveryReportViewIdValue ) )

enum TMessageFolderMessageStatus
    {
    MessageFolderAllRead,
    MessageFolderAllUnread,
    MessageFolderReadUnread
    };

enum TMessageFolderDeleteMessageStatus
    {
    MessageFolderAllDelete,
    MessageFolderAllUnDelete,
    MessageFolderDeleteUnDelete
    };

enum TMceTemplatesDialogStatus
    {
    MceTemplatesDialogClose,
    MceTemplatesDialogOpen
    };
// DATA TYPES
typedef CArrayFixFlat<TMceFolderItem> CMceFolderItemArray;

// FORWARD DECLARATIONS
class CMsvSession;
class CMceBitmapResolver;
class CMceMainViewListView;
class CAknLocalScreenClearer;
class CMceSessionHolder;
class CMceUi;
class CMceMessageListContainer;
class CMceMessageListContainerBase;
class CEnvironmentChangeNotifier;

class CMceListboxTypeObserver;

// CLASS DECLARATION

/**
*  This view is created when user opens folder.
*/
class CMceMessageListView :
    public CAknView,
    public MEikListBoxObserver,
    public MMsvSessionObserver,    
    public MMceListboxTypeObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMceMessageListView* NewL(
            CMsvSessionPtr aSession,
            TMsvId aFolderId,
            CMceSessionHolder& aSessionHolder,
            CMceBitmapResolver& aBitmapResolver,
            CMceMainViewListView& aMainView );

        /**
        * Destructor.
        */
        virtual ~CMceMessageListView();

    public: // New functions
    
    
        /**
        * Sets folder to be watched
        * @param aFolderId: folder id
        */
        void SetFolderL( TMsvId aFolderId );

        /**
        * Returns listbox type of the current folder.
        * @return Folder listbox type
        */
        TInt GetFolderListBoxType();


        /**
        * Returns pointer to list container.
        * @return Pointer to list container
        */
        CMceMessageListContainerBase* ListContainer() const;

        /**
        * Should be called by mce after message server startup.
        * This adds view to session observer list and creates needed CMsvEntries.
        */
        virtual void HandleMsgServerStartupL();
        
        /**
        * Checks if there are unread messages in the given folder
        */
        TBool HasUnreadMessagesL();

        /**
         * Cancels remote mailbox timer.
         */
        void CancelMailboxTimer() const;

        /**
         * Checks if folder aName already exists
         * @param aName folder name to be searched.
         * @param aId id of the folder. Should be set when renaming.
         * @return ETrue if name used.
         */
        TBool IsFolderNameUsedL(const TDesC& aName, TMsvId aId = KErrNotFound) const;

        /**
         * Changes currently opened folder and redraws the list.
         * @param aNewFolderId: id of the folder name to be opened
         */
        void ChangeFolderAndRefreshListboxL( TMsvId aNewFolderId );

        /**
        * Finds and selects message entry
        * @param aMatchString: the matched string
        */
        void FindAndSelectEntryL( const TDesC& aMatchString ) const;

        /**
        * Checks, if SyncMl folder
        * Returns ETrue, if SyncMl folder              
        */
        TBool IsSyncMl() const;

        /**
        * Checks, if SyncMl Outbox or Inbox folder
        * Returns ETrue, if SyncMl Outbox or Inbox folder
        * @param aFolder: id of the folder        
        */
        TBool SyncMlOutboxInbox( TMsvId aFolder ) const;

        /**
        * Checks, if MMS notification can be opened using selection key         
        */
        TBool CheckMMSNotificationOpenL( ) const;

        /**
        * Opens remote mailbox settings              
        */
        void OpenMailboxSettingsL();

        /**
        * Is IMAP folder open        
        */
        TBool IsImapFolderOpenL( ) const;

        /**
        * From CAknView: Sets the new size of Rect
        */
        void HandleStatusPaneSizeChange();

        inline void UpdateViewL();

    public: // Functions from base classes

        /**
        * From CAknView
        */
        TUid Id() const;

        /**
        * From CAknView
        */
        void DoActivateL(
            const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );

        /**
        * From CAknView
        */
        void DoDeactivate();

        /**
        * From CAknView
        */
        void DynInitMenuPaneL(
            TInt aResourceId,
            CEikMenuPane* aMenuPane );

        /**
        * From MEikCommandObserver
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From MMsvSessionObserver
        */
        virtual void HandleSessionEventL(
            TMsvSessionEvent aEvent,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3 );

        /**
        * From MEikCommandObserver
        */
        virtual void ProcessCommandL(TInt aCommand);
        
        /**
        * From CAknView ?member_description
        */
        void HandleClientRectChange();        

        /**
        * Sets the label for the middle soft key in the folder view
        * @since S60 3.1
        */
        void SetMSKButtonL();
        
        /**
        * Checks if current folder is emailmailbox for the middle soft key.
        * @since S60 3.1
        * @return Etrue if mailbox
        */
        TBool IsMSKMailBox();  
        
        /**
        * Checks if current folder is syncMLmailbox for the middle soft key.
        * @since S60 3.2
        * @return Etrue if syncMLmailbox
        */
        TBool IsMSKSyncMlMailBox();
        
        /**
        * Store current item id for mailbox focus
        * @since S60 3.0
        */
        void SetCurrentItemId();

        /**
        * Should be called by container when selection button is pressed so that
        * exit command is not showed in context sensitive menu. iHideExitCommand set
        * EFalse at the end of DynInitMenuPaneL function.
        * @param aHideExitCommand: ETrue if exit not shown next time option menu displayed
        */
        void SetHideExitCommand( TBool aHideExitCommand );
        
        /**
        * Check if the item is Unread
        * @para aId: the item id
        * @return ETrue if the item is unread, return EFalse if the item is read
        * @since Taco
        */
       TBool IsItemUnread( TMsvId aId ); 

        /**
        * Handle call by sendkey in email list 
        * @para aEntryId: current entry id
        * @Since S60 5.0
        */
        void HandleSendkeyInMailboxL( TMsvId aEntryId );

        /**
        * Call-back function for CEnvironmentChangeNotifier class.
        * @param object of CMceMessageListView.
        */        
        static TInt EnvironmentChanged( TAny* aObj );
        
        /**
        * From MMceListboxTypeObserver
        */
        virtual void ListboxTypeChangedL();
		
		/**
        * For setting the ContextCommandFlag when clicked to get the context menu
        */
        void SetContextCommandFlag(TBool aContextCommandFlag);

    protected:  // Functions from base classes

        /**
        * From MEikListBoxObserver
        */
        void HandleListBoxEventL(
            CEikListBox* aListBox,
            TListBoxEvent aEventType );

    private:

        /**
        * Constructor is private.
        */
        CMceMessageListView(
            CMsvSessionPtr aSession,
            TMsvId aFolderId,
            CMceSessionHolder& aSessionHolder,
            CMceBitmapResolver& aBitmapResolver,
            CMceMainViewListView& aMainView );

        /**
        * Symbian OS constructor
        */
        void ConstructL();

        /**
        * Handles different 'open's: Open folder or open message
        */
        void HandleOpenL();

        /**
        * Called when user pressed Back and main view should be opened
        */
        void HandleCloseL();

        /**
        * Changes currently opened folder.
        * @param aOpenParent: ETrue if "CD .." wanted.
        */
        void ChangeFolderL( TBool aOpenParent = EFalse );

        /**
        * User wants to open message and this call's Ui's EditMtmEntry function.
        */
        void EditEntryL();

        /**
        * Called by DynInitMenuPaneL to delete unneccessary menu items.
        * @param aMenuPane menu pane
        */
        void FetchMenuL( CEikMenuPane* aMenuPane ) const;

        /**
        * Called by DynInitMenuPaneL to delete unneccessary menu items.
        * @param aMenuPane menu pane
        */
        void FolderMenuL( CEikMenuPane* aMenuPane );

        /**
        * Called by FolderMenuL to delete unneccessary menu items.
        * @param aMenuPane menu pane
        */
        void FolderMenuNoItemsL( CEikMenuPane* aMenuPane ) const;
        /**
        * Called by FolderMenuL to delete unneccessary menu items.
        * @param aMenuPane menu pane
        */
        void FolderMenuOneItemL( 
            CEikMenuPane* aMenuPane, 
            TBool aRemoteMailboxOpened,
            TBool aThirdPartEmail  );
        /**
        * Called by FolderMenuL to delete unneccessary menu items.
        * @param aMenuPane menu pane
        */
        void FolderMenuManyItemsL(
            CEikMenuPane* aMenuPane, 
            TBool aRemoteMailboxOpened,
            TBool aThirdPartEmail,
            TBool aSetRead = EFalse );

        /**
        * Called by FolderMenuL to delete unneccessary menu items.
        * @param aMenuPane menu pane, SyncML folder
        */
		void FolderMenuSyncMLMain( CEikMenuPane *aMenuPane );

        /**
        * Called by FolderMenuL to delete unneccessary menu items.
        * @param aMenuPane menu pane, SyncML folder
        */
		void FolderMenuSyncMLFolderL( CEikMenuPane *aMenuPane );

        /**
        * Called by DynInitMenuPaneL to delete unneccessary menu items.
        * @param aMenuPane menu pane
        */
        void EditMenuL( CEikMenuPane* aMenuPane ) const;

        /**
        * Called by DynInitMenuPaneL to delete unneccessary menu items.
        * @param aMenuPane menu pane
        */
        void OrganizeMenuL( CEikMenuPane* aMenuPane );

        /**
        * Returns ETrue if folder is opened by this view and
        * back does not return to main view.
        * If EFalse returned then this view needs to be closed
        * and main view is activated.
        */
        TBool FolderOpenedL();

        /**
        * Returns folder count in currently opened folder
        */
        TInt SubfolderCount() const;

        /**
         * Checks how many unread messages folder has. If only one then asks
         * MceUi to open it. If more than one, select first unread. If none select first
         * item of the folder
         * @return ETrue if there is one message in inbox/mailbox folder and trying to open it,
         * EFalse when just opening inbox/mailbox folder
         * @param aId: message id
         */
        TBool FindUnreadMessageAndOpenItL( const TMsvId aId );

        /**
         * Finds folder called 'Inbox' from current (IMAP4 remote mailbox) folder and opens it.
         * Do not confuse with local service inbox!
         */
        void FindInboxAndOpenItL();

        /**
        * Sets empty text of the list box.
        * Empty text is different in Remote mailboxes than other folders.
        */
        void SetEmptyTextL() const;

        /**
        * Function for the CIdle class to be called, calls LaunchMceViewer.
        * @param aSelf: pointer to CMceMessageListView object.
        * @return see LaunchMceViewer.
        */
        static TInt LaunchViewer( TAny* aSelf );

        /**
        * Tries to launch viewer (by calling Mce's EditMtmEntryL)
        * for the unread message if there is only one unread
        * message in the folder. Otherwise does not do nothing
        * @return KErrNone if launch ok
        */
        TInt LaunchMceViewer( );

        /**
        * Checks if selected entry can be opened and dims 'send via' menu if it can
        * @param aEntryId: currently selected entry
        * @param aMenuPane: menupane
        */
        void HandleSendViaMenuItemL( TMsvId aEntryId, CEikMenuPane* aMenuPane ) const;

        /**
        * Called by HandleSessionEventL if entries created or changed.
        * @param aParentId: id of the parent
        * @param aSelection: pointer to selection of messages changed.
        */
        void MessageViewHandleEntriesCreatedOrChangedL(
            TMsvId aParentId,
            CMsvEntrySelection* aSelection );

        /**
        * Handles EMceCmdSortByDate,EMceCmdSortBySubject and EMceCmdSortBySender
        * @param aCommand: id of the command
        */
        void HandleSortCommandL( const TInt aCommand );

        /**
        * Open SyncMl Inbox folder               
        */
        void SyncMlFindInboxAndOpenItL();   
        
        /**
        *
        */
        TInt GetSetRead( const CMsvEntrySelection& aEntries );
        
        /**
        * Checks are the selected messages deleted or undeleted
        * @since 3.2
        * @return Index of the selected message deletetion status
        * otherwise KErrNotFound
        */
        TInt GetDeleteFlagStatusL( const CMsvEntrySelection* aEntries );
        
        /**
        * Checks if there is only one unread message in current folder
        * and starts viewer for it
        * @since 3.2
        * @param aMsgMtmUid: filled with Message MTM Uid
        * @return Index of the message if only one unread, 
        * otherwise KErrNotFound
        */
        TInt LaunchViewerWhenOneUnreadL( TInt &aMsgMtmUid );
        
        /**
        * registers MceMessageListContainer to Msk observer
        * @param aObserver: True if set MskObserver and False to remove it.
        * @since 3.2
        */
        void SetMskObserver( TBool aObserver );


		/**
		* Input parameter list for AIW plugin
		*/
		CAiwGenericParamList* AiwSyncParamListLC(TInt aId, const TDesC& aText);

        /**
        *
        */
        void HandleEntriesCreatedChangedL( const CMsvEntrySelection* aSelection );

        /**
        *
        */
        void HandleEntriesDeletedL( );

        /**
        *
        */
        void HandleEntriesMovedL( TMsvId aSourceFolderId, TMsvId aTargetFolderId );
        
        
        /**
        * Creates either one row or two row listbox and sets it to iMsgListContainer
        */
        void CreateListboxL();

        /**
        * Find all read messages and mark 
        * 
        * @since Taco
        */
        void FindAllReadMsgAndMarkL();
        
        /**
        * Check if there is any selected item visible in current view 
        * @ msgIndexArray: the array of the index of selected item
        * @since S60 5.0
        */
        TBool AnySelectedItemsVisible( CArrayFixFlat<TInt>* msgIndexArray );

        /**
        * Check if it is in Inbox, Documents and user's own foldermark, 
        * if it is, enable mark read msg option from menu
        * @currentEntry: current entry
        * @currentfolderId: current Folder Id
        * @aMenuPane: Menu pane to show the mark all read command
        * @since S60 5.0
        */
        void EnableMarkAllReadMsgInInboxAndDocument( 
            TMsvEntry currentEntry, 
            TMsvId currentFolderId, 
            CEikMenuPane* aMenuPane );
            
            
        /**
        * Set correct menu items for expand/collapse options
        * @param aMenuPane: 
        * @since S60 5.0
        */    
        void HandleContainerOptionsMenuL( CEikMenuPane* aMenuPane );  
        
        /**
        *
        */
        void HandleContainerOptionsCommandL( TInt aCommand );
        
        /**
        * called from EnvironmentChanged.
        */
        TInt HandleEnvironmentChanged();
         
        /*
        * Check and Set Forward command for the selected item
        * Forward is currently only available in Sent Item folder
        * Since 5.0
        */
        void SetForwardMenuL( CEikMenuPane* aMenuPane, TMsvEntry& aEntry );            
         
        /*
        * Dim Read/Unread command from options menu, prepare for further settings
        * Since 5.0 
        */
        void DimReadUnreadOptionsMenu( CEikMenuPane *aMenuPane );
        
        /*
        * Set Read/Unread command for options menu, when multiple msg/emails are selected
        * Since 5.0
        */
        void SetReadUnreadOptionsMenu( CEikMenuPane *aMenuPane, TBool aIsEmail, TBool aSetRead );
public:
        /**
        * Set the iReadUnread variable if Mark as read/unread appears twice
        * if it is, enable mark read msg option from menu
        * @aValue: The value to be set to iReadUnread variable
        * Since 3.2
        */
        void SetMarkReadUnread( TBool aValue );
		
        /**
         *  This function sets aClearer to iLocalScreenClearer which contains Inbox view
         *  screen. aClearer is used in CMceUi::HandleGainingForeground() when returning from viewers.
         */
        void  GetLocalScreenClearer( CAknLocalScreenClearer** &aClearer );
            
    private:    // Data

        CMceUi*                 iMceUi;
        CMceMessageListContainerBase*  iMsgListContainer;
        
        TInt                    iCurrentListboxId;
        CMsvSessionPtr          iSession;
        TMsvId                  iFolderId;
        CMceSessionHolder&      iSessionHolder;
        CMceBitmapResolver&     iBitmapResolver;
        TBool                   iMceViewActivated;
        TBool                   iMsvSessionReady;
        CAiwServiceHandler*		iAiwServiceHandler;
        TMceFlags               iListViewFlags;
        TMsvId 					iCurrentItemId;

        TInt    iSelectedIndex; // index of the cursor when deactivated
   
        HBufC*  iInboxName;
        HBufC*  iRemoteMailboxEmptyText;
        HBufC*  iEmptyText;
        CMceMainViewListView& iMainViewListView;
        TBool   iHideExitCommand;
        TBool   iFetchMenuSelectionKeyPressed;
        CPeriodic*  iMessageViewerLauncher;        
        CAknLocalScreenClearer* iLocalScreenClearer;
        TInt    iLaunchViewerCounter;
        CMceFolderItemArray* iFolderItemArray;
        TBool   iConnectionNote;

        TInt    iSortOrder;
        TBool   iOrdering;        
        TBool   iRunningEditFunction;
        TBool   iIsUnread;
        
        TInt                     iCurrentListType;
        CMceListboxTypeObserver* iListboxMailTypeObserver;
        CMceListboxTypeObserver* iListboxInboxTypeObserver;
        
        CEnvironmentChangeNotifier* iDateTimeNotifier;
        //Variable that get set when Mark as read/unread appears twice
		TBool 						iReadUnread; 
		//Variable for context command
		TBool                       iContextCommand;
    };

inline void CMceMessageListView::UpdateViewL()
	{
	if ( iMsgListContainer )
	    {
        iMsgListContainer->SetAnchorItemIdL(
            CMceMessageListContainerBase::EMessageListOperationCompleted );
        iMsgListContainer->RefreshListbox();
	    }
	}



#endif

// End of file
