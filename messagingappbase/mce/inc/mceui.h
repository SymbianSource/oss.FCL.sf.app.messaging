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
*     Defines the CMceUi class
*
*/



#ifndef MCEUI_H
#define MCEUI_H

//  INCLUDES
#include <aknViewAppUi.h>       // CAknViewAppUi
#include <MuiuMsvSingleOpWatcher.h>
#include "MtmRegistryObserver.h" // MMtmUiDataRegistryObserver
#include <MsgArrays.h>          // CUidNameArray
#include <gulicon.h>            // CGulIcon

#include "MceUtils.h"           // MMceAccountManager
#include "MsgFunctionInfo.h"

#include "MceIds.h"
#include "MceLogText.h"
#include <AknTabObserver.h>     // MAknTabObserver
#include <rconnmon.h>
#include "MceMailAccountItem.h"
#include <mcesettingsaccountmanager.h> // in MceSettings.dll
#include <bldvariant.hrh>
#include "MceEmailNotifHandler.h"
#include <AknQueryDialog.h>
#include <mmsclient.h>

// DATA TYPES
enum
    {
    EMceMainViewActive = 0x01,
    EMceMessageViewActive = 0x02,
    EMceDeliveryReportsViewActive = 0x04
    };
enum
    {
    EMceUiFlagsFinishedConstruction = 0x01,
    EMceUiFlagsServerReady          = 0x02,
    EMceUiFlagsTabsActive           = 0x04,
    EMceUiFlagsWantToExit           = 0x08,
    EMceUiFlagsExitAfterDisconnect  = 0x10,
    EMceUiFlagsDoNotExitAfterNextOperationComplete = 0x20,
    EMceUiFlagsMainViewActivated = 0x40,
    EMceUiFlagsMediaUnavailable = 0x80,
    EMceUiFlagsSupressViewUpdate = 0x100,
    EMceUiFlagsMainViewAiwCreated = 0x200,
    EMceUiFlagsMessageViewAiwCreated = 0x400,
    EMceUiFlagsSettingsDialogOpen = 0x800,
    EMceUiFlagsExitOnMsvMediaAvailableEvent = 0x1000,
    EMceUiFlagsContainerAddedToStack = 0x2000,
    
    };
#define KMsvMyFoldersEntryIdValue 0x1008
const TMsvId KMsvMyFoldersEntryId=KMsvMyFoldersEntryIdValue;

typedef CArrayFixFlat<TMceMailAccountItem> CMceMailAccountItemArray;

// FORWARD DECLARATIONS
class CMsvSession;
class CMtmStore;
class CMtmUiDataRegistry;
class CMceMessageListView;
class CMceMainViewListView;
class CMceSessionHolder;
class CMceBitmapResolver;
class CMceConnectMailboxTimer;
class CMceLogEngine;
class CMessageIterator;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CMceIdleMtmLoader;
class CAknLocalScreenClearer;
class CErrorUI;
class CAknTitlePane;
class CSendingServiceInfo;
class CMceDeliveryReportView;
class CImumInternalApi;
class CMceIAUpdateUtils;
class CMmsCodecClient;
// CLASS DECLARATION

/**
*  Mce's UI class.
*/
class CMceUi :
    public CAknViewAppUi,
    public MMsvSessionObserver,
    public MMtmUiDataRegistryObserver,
    public MMsvSingleOpWatcher,
    public MMceSettingsAccountManager,
    public MAknTabObserver,
    public MConnectionMonitorObserver,
    public MCoeForegroundObserver // CR : 401-1806
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CMceUi();

        /**
        * Destructor.
        */
        virtual ~CMceUi();

    public:

        /**
        * Opens message folder and activates message view
        * @param aFolderId id of the folder to be opened
        */
        void OpenFolderViewL( TMsvId aFolderId );

        /**
        * Opens remote mailbox and activates message view
        * @param aFolderId id of the folder to be opened
        */
        void OpenRemoteMailboxViewL( TMsvId aFolderId );

        /**
        * Cancels ConnectMailboxTimer.
        */
        void CancelMailboxTimer();

        /**
        * Calls corresponding mtm to edit entry
        * @param aEntry: entry to be edited.
        */
        void EditMTMEntryL( const TMsvEntry& aEntry );

        /**
        * Used to launch unieditor
        */
        void LaunchUniEditorL();


        /**
        * Opens connection to account
        * @param aAccount account id to be connected.
        */
        void GoOnlineL( TMsvId aAccount );

        /**
        * Opens connection to account with confirmation query.
        * @param aAccount account id to be connected.
        */
        void GoOnlineWithQueryL( TMsvId aAccount );

        /**
        * Closes connection to mailbox.
        * @param aAccount account id to be disconnected.
        */
        void GoOfflineL( TMsvId aAccount );

        /**
        * Connects or disconnects
        * @param aServiceId id of the account to be connected
        * or disconnected.
        * @param aConnect ETrue if connected, EFalse if disconnected.
        */
        void ChangeServiceConnectionStateL(TMsvId aServiceId, TBool aConnect);

        /**
        * Fills TMsvEntry structure of the message.
        * @param aId id of the message.
        * @param aEntry structure to be filled.
        */
        TMsvId GetEntryL(TMsvId aId, TMsvEntry& aEntry) const;

        /**
        * Closes connection if only one online. If more than one connection on
        * displayes query to disconnect.
        */
        void CloseConnectionWithListQueryL();

        /**
        * Returns pointer to log engine.
        */
        CMceLogEngine* LogEngine();

        /**
        * Launches help application
        */
        void LaunchHelpL();

        /**
        * Checks if command is currently available
        * @param aCommand command id
        * @param aEntryToCheck current entry selected
        * @return 0 if available. != 0 otherwise
        */
        TInt CheckCommandAvailableL( TInt aCommand, TMsvEntry& aEntryToCheck );

        /**
        * Checks if command is currently available
        * @param aCommand command id
        * @param aEntriesToCheck currently selected entries
        * @return 0 if available. != 0 otherwise
        */
        TInt CheckCommandAvailableL( TInt aCommand, CMsvEntrySelection* aEntriesToCheck );

        /**
        * Called when UI receives HandleSessionEventL( EMsvServerReady, .... )
        * from msgs.
        * Creates needed messaging related objects, collects info of the
        * installed MTMs, creates icon array and finally activates main view.
        */
        void HandleServerStartupL();

        /**
        * Displayes tabs (creates tabs if not already created).
        * Should be called when opening folder.
        * @param: aOpenedId: id of the folder to be opened (used to set active tab)
        * @param: aOpeningDeliveryReports: ETrue when opening delivery reports view and aOpenedId is ignored.
        *                                  EFalse if opening folder from msgs.
        */
        void ShowTabsL( TMsvId aOpenedId, TBool aOpeningDeliveryReports = EFalse );

        /**
        * Removes tabs.
        */
        void RemoveTabs();

        /**
        * Should be called by view's DoActivate to inform mceui what view is active.
        * Sets view flag.
        * @param aActiveView: EMceMainViewActive or EMceMessageViewActive, etc
        */
        void SetMceViewActive( TUint aActiveView );

        /**
        * Query function to query is certain view active
        * For example MceViewActive( EMceMainViewActive ).
        * @param aActiveView: EMceMainViewActive or EMceMessageViewActive, etc
        * @return ETrue if view is active, EFalse otherwise
        */
        TBool MceViewActive( TInt aActiveView ) const;

        /**
        * Sends entry via BT or IR.
        * @param aId: id of the message to be sent
        * @param aMtm: mtm to be used to send
        */
        void SendViaL( TMsvId aId, TUid aMtm );

        /**
        * Returns ETrue if mce is performing editor opening operation
        */
        TBool IsEditorOpen( ) const;

        /**
        * Gets status pane of the mce application
        * @return pointer to status pane (to the object of CAknTitlePane)
        */
        CAknTitlePane* TitlePaneL() const;

        /**
        * Activates timer for mailbox connection query
        * @param aEntryId: folder to be opened
        */
        void ActivateConnectionQueryTimerL( TMsvId aEntryId );

        /**
        * Clears EMceUiFlagsExitAfterDisconnect flag
        * 
        */
        void ClearExitAfterDisconnectFlag();
        
        void SetViewUpdateSuppressionFlag( TBool aValue, CMsvOperation* aOp );
        TBool ViewUpdateSuppressionFlag();

        /**
        * Sets EMceUiFlagsDoNotExitAfterNextOperationComplete flag
        * 
        */
        void SetDontExitOnNextOperationComplete();

        /**
        * Clears EMceUiFlagsDoNotExitAfterNextOperationComplete flag
        * 
        */
        void ClearDontExitOnNextOperationComplete();

        /**
        * Call back function, clears EMceUiFlagsDoNotExitAfterNextOperationComplete flag
        * 
        */
        static TInt ClearDontExitOnNextOperationComplete( TAny* aAny );

        /**
        * Checks if SyncML mail synchronizing going on
        * Returns ETrue, if synchronizing is going on 
        */
        TBool IsSyncronizingL( const TMsvEntry& entry );

        /**
        * Checks when last always online connection succeeded
        * @param aAccount: service id
        * Returns EFalse, if last connection failed 
        */
        TBool AlwaysOnlineLastConnectionL( const TMsvId aAccount );
        
        /**
        * Checks if always online feature is supported       
        * Returns ETrue, if always online is supported
        */
        TBool AlwaysOnline( );

        /**
        * Checks if remote mailbox connection was made in mce
        * @param aServiceId: service id
        * Returns ETrue, if connection was made in Mce 
        */
        TBool ServiceIdFound( const TMsvId& aServiceId );

        /**
        * If connection is made in mce, adds operation id/service in iMailAccountItemArray
        * @param aOp: operation id
        * @param aServiceId: service id  
        */
        void AddOperationIdL( const TMsvOp& aOp, const TMsvId& aServiceId );
       
        /**
        * Removes operation id from iMailAccountItemArray 
        * @param TMsvId: operation id
        * @param aServiceId: service id  
        */
        void RemoveOperationId( const TMsvId& aOpId, const TMsvId& aServiceId );
        
        /**
        * Shows a query in mce startup,
        * if message store was automatically changed to phone from MMC
        * @param aShowQuery: if ETrue, query can be shown
        */
        void ToPhoneMemoryQueryL( TBool aShowQuery );       

        /**
        * Sets EMceUiFlagsMainViewActivated flag
        *        
        */
        void SetMainViewActivatedFlag( );

        /**
        * Removes folder tabs.
        */
        void RemoveFolderTabs();

        /**
        * Shows folder tabs.
        * @param aDepth: folder path depth
        */
        void ShowFolderTabsL( const TInt aDepth );

        /**
        * Adds mtm specific functions to the menu for multiselection
        * @param aMenuPane
        * @param aMenucommandId id of the command after mtm specific functions are added
        */
        void AddMultiselectionMTMFunctionsL( CEikMenuPane& aMenuPane, TInt aMenuCommandId );

        /**
        * Check mtm specific tecnology uid
        * @param aTechnologyType
        * @param aMtmType
        * Returns ETrue, if tecnologyUid match with mtm.
        */
        TBool CheckTechnologyTypeFromMtm( const TUid aTechnologyType, TUid aMtmType );

        /**
        * Checks if the message is set to be deleted from server
        * Returns ETrue, if the message is set to be deleted from server, EFalse otherwise
        */        
        TBool IsMailSetToBeDeletedFromServerL( TMsvId aItemId );
        
        /**
        * Defines item to be anchored when entries are deleted or moved from entry list.
        */
        void DefineAnchorIdL( const CMsvEntrySelection* aSelection );
        
        /**
        * Return itemId that should be focused after deleting or moving some entries.
        */
        TMsvId AnchoredItem();

        /**
        * Set anchorId.
        * @param aAnchorId itemId that should be focused after deleting or moving some entries.
        */
        void SetAnchorId( TMsvId aAnchorId );

    public:     // Functions from base classes
        /**
        * From CEikAppUi
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CEikAppUi
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * From MMsvSessionObserver
        */
        virtual void HandleSessionEventL(
            TMsvSessionEvent aEvent,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3);

        /**
        * From CAknViewAppUi
        */
        void ActivateLocalViewL( TUid aViewId );

        /**
        * From MMceAccountManager
        */
        TBool CreateNewAccountL(
            TUid aMessageType,
            TMsvId aOldServiceId );

        /**
        * From MMceAccountManager
        */
        void EditAccountL( TMsvId aId );

        /**
        * From MMceAccountManager
        */
        CUidNameArray* MtmAccountsL( TUid aType );

        /**
        * From MMceAccountManager
        */
        TBool CanCreateNewAccountL(TUid aMtm);

        /**
        * From MMceAccountManager
        */
        void DeleteAccountL(TMsvId aId);

        /**
        * From MMceAccountManager
        */
        CMsvSession& Session();

        /**
        * From MMceAccountManager
        */
        TBool IsPresent( TUid aMtm ) const;

        /**
        * From MMceAccountManager
        */
        THumanReadableName MtmName( TUid aType ) const;
        
        /**
        * From MMceAccountManager
        */
        void SetChangeMessageStore( TBool aChangeEnded );

        /**
        * From MMtmUiDataRegistryObserver
        */
        virtual CBaseMtmUiData* GetMtmUiDataL( TUid aEntryType );

        /**
        * From MMsvSingleOpWatcher
        */
        virtual void OpCompleted(
            CMsvSingleOpWatcher& aOpWatcher,
            TInt aCompletionCode );

        /**
        * From MAknTabObserver
        */		
        void TabChangedL( TInt aIndex );

        /**
        * From CEikAppUi
        */
        void HandleResourceChangeL( TInt aType );

        /**
        * From MConnectionMonitorObserver
        */
        void EventL( const CConnMonEventBase &aConnMonEvent );

        /**
         * From MCoeForegroundObserver
         */
        void HandleGainingForeground(); // CR : 401-1806
        
         /**
         * From MCoeForegroundObserver
         */
        void HandleLosingForeground(); // CR : 401-1806
        
        /**
        * MMS.Content.Upload
        */
        void DoUploadL( TInt aCommand );
        
        /**
        * MMS.Content.Upload query
        */
        void ShowUploadQueryL( );

		/**
        * MMS.Content.Upload query
        */
		TInt NumUploadServices();

		void HandleNotif();
		
		void HandleNotif(const TMsvId& aMailbox);
		/**
		 * Checks if the SIM access profile is connected or not.
		 */
		void CheckSIMAccessProfileL();

		/**
		* Closes the open editors/viewers
		*/
		void CloseEditorApp();

        /**
        * Checks if there has been a change in the screen resolution
        */
        TBool ResourceChangeCalled();
        
        /**
        * Resets the flag indicating that there has been a change in
        * the screen resolution
        */
        void ResetResourceChange();

        /**
        * Checks if the SyncML mailbox is being created
        */
        TBool MailboxCreationOn();        
        
    private:

        /**
        * From CCoeAppUi
        * Observes left / right arrow keys to change active tab.
        */
        TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

        /**
        * Symbian OS constructor.
        */
        void ConstructL();

        /**
        * User requested to create new folder under local.
        * Data query is displayed and folder created.
        */
        void CreateNewFolderL();

        /**
        * User requested to rename new folder under local.
        * Data query is displayed and folder renamed.
        */
        void RenameFolderL();

        /**
        * Displays query for foldername and also possible "Duplicate folder" error note.
        * @param aResourceId resource for the folder name query
        * @param aFolderName buffer for the folder name. If renaming then should include old name
        * @param aFolderId id of the folder if renaming.
        */
        TBool FolderNameQueryL(
            const TInt aResourceId,
            TDes& aFolderName,
            const TMsvId aFolderId = KMsvNullIndexEntryIdValue ) const;

        /**
        * Calls UpdateMessageTypesArrayL
        */
        void HandleMTMChangeL();

        /**
        * Adds mtm specific functions to the menu
        * @param aMenuPane
        * @param aMenucommandId id of the command after mtm specific functions are added
        */
        void AddMTMFunctionsL(CEikMenuPane& aMenuPane, TInt aMenuCommandId);

        /**
        * Gets the default service for mtm.
        * @param aMtm
        * @param aService will be filled to default account's id.
        */
        TBool GetDefaultServiceL( TInt aMtm, TMsvId& aService, TUid& aMtmUid );
        void GetDefaultServiceL( TInt aMtm, TMsvId& aService );

        /**
        * Creates new message of type aMessageType.
        * @param aMessageType type of the message to be created.
        */
        void CreateNewMessageL( TInt aMessageType );

        /**
        * Launches settings dialog.
        */
        void SettingsDialogL();

        /**
        * Creates new entry
        * @param aParentId
        */
        void CreateAndEditNewEntryL( TMsvId aParentId, TMsvEntry& aNewEntry );

        /**
        * Creates new entry
        * @param aEntryType
        * @param aMtm
        * @param aParentId
        * @param aServiceId
        */
        void CreateAndEditNewEntryL(
            TInt aEntryType,
            TUid aMtm,
            TMsvId aParentId,
            TMsvId aServiceId );

        /**
        * Called by OpCompleted.
        * @param aOperation
        * @param aCompletionCode
        */
        void DoOperationCompletedL(
            CMsvOperation* aOperation,
            TInt aCompletionCode );

        /**
        * User requested to delete items.
        * Launches query and if accepted deletes items.
        */
        void HandleDeleteL();

        /**
        * User requested to undelete items.
        */
        void HandleUndeleteL();

        /**
        * User requested to move or copy message items.
        * Launches folder selection query and if accepted moves items.
        * @param aCopy: set ETrue if copying instead of moving.
        */
        void MoveOrCopyEntriesL( TBool aCopy = EFalse );

        /**
        * Sends selection immediately.
        * @param aSel Array of items to be sent.
        */
        void SendSelectionL(CMsvEntrySelection* aSel);

        /**
        * Tries to send currently selected item immediately.
        */
        void SendNowL();

        /**
        * Cancels sending
        */
        void CancelSendingL();

        /**
        * Fetch new messages.
        */
        void FetchNewL();

        /**
        * Fetch selected messages.
        */
        void FetchSelectedL();

        /**
        * Fetch all the messages.
        */
        void FetchAllL();

        /**
        * @param aFunctionId
        */
        void DoAsyncFunctionOnCurrentContextL(TInt aFunctionId);

        /**
        * @param aFunctionId
        * @param aSelection
        */
        void DoAsyncFunctionOnCurrentContextL(
            TInt aFunctionId, CMsvEntrySelection& aSelection);

        /**
        * @param aFunctionId
        * @param aContext
        */
        void DoAsyncFunctionL(TInt aFunctionId, const TMsvEntry& aContext);

        /**
        * @param aFunctionId
        * @param aSelection
        * @param aContext
        */
        void DoAsyncFunctionL(
            TInt aFunctionId,
            CMsvEntrySelection& aSelection,
            const TMsvEntry& aContext );

        /**
        * Checks if any connections are on. If there are connections on then
        * displays query to User to disconnect. Disconnects if user selects yes.
        */
        void CloseConnectionsBeforeExitL();

       /**
        * Returns array of the connected accounts.
        * Places array to the clean up stack. Calling function must delete the array.
        */
        CMsvEntrySelection* ConnectedServicesLC();

        /**
        * Fills TMsvEntry structure of the currently selected entry.
        * @param aEntry structure to be filled.
        * @param aOpenedFolder ETrue if opened folder info wanted
        * EFalse if currently selected (entry under 'cursor')
        * wanted.
        */
        TBool GetCurrentEntryL(TMsvEntry& aEntry, TBool aOpenedFolder = EFalse) const;

        /**
        * Checks if mtm function is currently available
        * @param aFunction
        * @param aEntryToCheck
        */
        TInt CheckMTMFunctionL( const TMsgFunctionInfo& aFunction, TMsvEntry& aEntryToCheck );

        /**
        * Perform mtm function.
        * @param aFunction function to be executed.
        * @return none
        */
        void HandleMTMFunctionL(const TMsgFunctionInfo& aFunction);

        /**
         * Opens next or previous message.
         */
        void HandleOpenNextPreviousL( TBool aOpenNext = ETrue );

        /**
        * Starts USSD editor application
        */
        void LaunchServiceCommandEditorL() const;

        /**
        * Starts CBS application
        */
        void LaunchCBSApplicationL();

        /**
        * Performs 'Move back to Drafts' operation
        */
        void MoveFromOutboxToDraftsL();

        /**
        * Creates tabs. Should be called when opening folder.
        */
        void CreateTabsL();

        /**
        * Calls and traps ShowTabsL, ignores errors.
        * Used in CreateNewMessageL as TCleanupItem.
        * @param aAny: pointer to object of CMceUi.
        */
        static void ShowTabs( TAny* aAny );

        /**
        * Checks remote mailbox states and updates remote mailbox icons if connected
        * or not
        */
        void CheckRemoteMailboxTabIconsL();

        /**
        * Forces mtm loader to finish loading.
        */
        void ForceMtmLoaderFinish();

        /**
        * Closes connection to mailbox synchronously. Used when trying to exit application
        * when connected to one or more mailboxes.
        * @param aAccount account id to be disconnected.
        */
        void GoOfflineSynchronouslyL( TMsvId aAccount );

        /**
        * Resets iEditorOperation if leave occurs.
        * @param aAny: pointer to object of CMceUi.
        */
        static void ResetEditorOperation( TAny* aAny );

        /**
        * Check if free disk space goes under critical level, and leaves if it does
        */
        void LeaveIfDiskSpaceUnderCriticalLevelL( TInt aBytesToWrite = 0 );

        /**
        * Cancels RequestFreeDiskSpace if leave happens
        * on DeleteAccountL
        * @param aAny: pointer to object
        */
        static void CancelFreeDiskSpaceRequest( TAny* aAny );

        /**
        * returns ETrue if sms or mms message entry is connected or in sending state
        * @param aEntryId: id of the entry to be checked.
        */
        TBool IsMessageSendingL( TMsvId aEntryId ) const;
       
        /**
        * Replies to sender/all
        * @param aReplyCommand: reply to sender or reply to all       
        */
        void ReplyL(TInt aReplyCommand);

        /**
        * Marks selected mail to read        
        */
        void MarkAsReadL( TBool aRead = ETrue );

        /**
        * If mtm type is SyncMl, updates new message -dialog.
        * @param aSelection: entry selection
        * @param aEvent: event type  
        */
        void SyncMlNewMessageItemL( const CMsvEntrySelection* aSelection, TInt aEvent );

        /**        
        * Performs 'Move back to Drafts' operation to SyncMl outbox entry
        */        
        void SyncMlMoveFromOutboxToDraftsL( );

        /**
        * Returns ETrue if always online is supported               
        */
        TBool AlwaysOnlineL( );

        /**
        * Handles MMS notification delete
        * @param aParameter: parameter value
        */
        void HandleMMSNotificationsDeleteL( TDes8& aParameter );
        
        /**
        * Separates MMS notifications from selection       
        * @param aSelection: pointer to selected entries
        */
        void CheckMMSNotificationsL( CMsvEntrySelection* aSelection );       

        /**
        * Opens remote mailbox settings              
        */
        void OpenMailboxSettingsL( );

        /**
        * Marks selected SyncML mail to read        
        */
        void SyncMlMarkAsReadL( );

        /**
        * Checks if MMS notification delete failed        
        */
        void CheckMMSNotifDelFailedL();

        /**
        * Checks CSP Cell Broadcast bit from SIM      
        */
        TBool CheckCspBitL() const;
        /**
        * Checks if csp is supported      
        */
        TBool CspBitsL( );

        /**
        * Updates the application icon in the context pane      
        */        
        void LoadContextIconL();
        
        /**
        * Releases other that built-in mtm ui data objects.
        * So that it could be possible to install new version of some mtm
        * from inbox.
        */
        void ReleaseMtmUiData();

        /**
        * Shows the information note, when mail messages cannot be copied or moved.
        */
		void InformationNoteCannotMoveCopyEMailL();

        /**
        * Removes mail messages from the selection
        */
        TBool RemoveEmailMessagesFromSelection( CMsvEntrySelection *aSelection );
        
        /**
        * Forwards message to receiver
        * @param aForwardCommand: Forwards message to receiver 
        * Since 3.2      
        */
        void ForwardL(TInt aForwardCommand);

        /**
        * Changes local zoom level.
        * @param aZoomLevel: new local zoom value
        * @return Returns previous local zoom value
        * @since 3.2
        */
        TAknUiZoom ZoomLevelChangedL( TAknUiZoom aZoomLevel );
        
        /**
        * Reads zoom level value from
        * Central Repository: KCRUidMuiuSettings, key: KMuiuZoomValue
        * @return Local zoom level saved in cenrep.
        * @since 3.2
        */
        TAknUiZoom ReadZoomLevelL();
        
        /**
        * Saves zoom level value to
        * Central Repository: KCRUidMuiuSettings, key: KMuiuZoomValue
        * @param aZoomLevel: new local zoom value
        * @since 3.2
        */
        void WriteZoomLevelL( TAknUiZoom aZoomLevel );
        
        /**
        * Called from HandleCommandL, changes zoom level and saves it to cen rep.
        * @param aCommand: command id (EMceCmdZoomValueAutomatic - EMceCmdZoomValueLarge)
        * @since 3.2
        */
        void HandleZoomLevelChangeL( TInt aCommand );
        
        /**
        * Sets correct sub menu item selected when zoom submenu opened.
        * @param aMenuPane: menupane from DynInitMenuPaneL
        * @since 3.2
        */
        void HandleZoomSubMenu( CEikMenuPane* aMenuPane );
        
#ifdef RD_READ_MESSAGE_DELETION
        /**
        * CMceUi::HandleNewMsgToInboxL( removeNewFlag, selection, &flagChanged );
        * Remove New Flag from selected message(s) if there is any
        * From 4.0
        */
        void HandleNewMsgToInboxL( TBool removeNewFlag,
                                  CMsvEntrySelection* aSelection );
#endif //RD_READ_MESSAGE_DELETION

        /**
        * TBool NewComingEmailL( TMsvId aFolderId )
        * Check if there is new email in the folder
        * Returns ETrue if There is new email in the folder, otherwise returns EFalse
        * @para: Id of folder entry
        * Since 3.2  
        */
        TBool HasNewEmailL( TMsvId aFolderId );
        
        /** 
        * Set disk space for move or copy messages
        * Since 3.1
        **/
        void SetDiskSpaceForMoveOrCopyL( );
        
        /*
        * Used for Fixed toolbar to get email type to launch email editor
        */
        TInt GetToBeCreatedEmailType();

        /**
        * Checks if 'real' exit or just hiding needs to be done when performing user's exit command.
        * @since S60 5.0
        */        
        void HideOrExit();
        
        /**
        * Resets mce main view and then hides application from the ui.
        * @since S60 5.0
        */
        void ResetAndHide();
        
        /**
        * Removes tabs and updates tabs array if those were active.
        * Called when something changes in msg store root.
        * @since S60 5.0
        */
        void RemoveTabsAndUpdateArray();
        
        /**
         * Checks for any IA updates.
         */
        void CheckIAUpdate();
        /**
        * CR:422-271
        * Set of Functions for decoding the .mms files from the hardcoded path
        * @since S60 5.0
        */
        TInt PopulateMMSTemplates();

        TInt DecodeAllMMSFromDirectoryL( TFileName& aFilePath, TMsvId aBoxid /*= KMsvDraftEntryId*/ );

        TInt DecodeMMSFromFileL( TFileName& aFilePath, TMsvId aBoxId /*= KMsvDraftEntryId*/, TUint32 aFlags = 0 );
        
        TInt FeedDataInChunks( CMmsCodecClient* aCodecClient );
        
        void FillBufferFromFileL( const TDesC& aFilePath, RFs& aFs, CBufFlat* aEncodeBuffer );

    public:
   void OpenMtmMailboxViewL(const TMsvEntry& aEntry); 
   TBool FindMtmInVisibleListL( TUid aMtm, CArrayFix<TUid>& aListItemUids );   
        /**
        * Gets the status of iMsgDeletedStatus flag.
        * @since S60 5.0
        */
        TInt GetMsgDeletedStatus();
        /**
        * Sets iMsgDeletedStatus flag.
        * @since S60 5.0
        */
        void SetMsgDeletedStatus(TBool aStatus);
        TUid GetFsIntegratedEmailAppMtmPluginId();
        
        
    private:    // Data
        CMsvSession*                    iSession;
        CMtmUiDataRegistry*             iUiRegistry;
        CMsvEntry*                      iEntry;
        CMsvEntry*                      iRootEntry;

        CMtmStore*                      iMtmStore;

        CUidNameArray                   iMsgTypesWritePopup;
        CUidNameArray                   iMsgTypesWriteSubmenu;
        CUidNameArray                   iMsgTypesSettings;
        CMsgFuncArray*                  iMTMFunctionsArray;

        CMsvSingleOpWatcherArray        iOperations;
        CMsvSingleOpWatcher*            iEditorOperation;

        CMceMessageListView*            iMceListView;
        CMceDeliveryReportView*         iMceDeliveryView;
        CMceMainViewListView*           iMceMainView;
        TUint                           iMceActiveView;

        CMceSessionHolder*              iSessionHolder;

        CMceBitmapResolver*             iBitmapResolver;

        CMceConnectMailboxTimer*        iConnectMailboxTimer;
   
        CMceLogEngine*                  iMceLogEngine;
        CAknNavigationControlContainer* iNaviPane;
        CAknNavigationDecorator*        iDecoratedTabGroup;
        CMsvEntrySelection*             iTabsArray;
        CMceIdleMtmLoader*              iMtmLoader;
        TMceFlags                       iMceUiFlags;
        CErrorUI*                       iMceErrorUi;
        CMsvSingleOpWatcher*            iDeleteOperation;
        CMsvSingleOpWatcher*            iFetchNewOperation;
        CMsvOperation*		            iBlockingOperation;
        CAknLocalScreenClearer*         iLocalScreenClearer;
        CMceMailAccountItemArray*       iMailAccountItemArray;
        CAknNavigationDecorator*        iFolderIndicator;
        CMsvEntrySelection*             iMMSNotifications;
        CMceIAUpdateUtils*              iMceIAUpdate;

        TBool                           iMessageStoreExist;
        TBool                           iCancelFreeDiskSpaceRequest;
        TBool                           iMemoryInUse;
        TBool                           iAlwaysOnline;        
        RConnectionMonitor              iConnectionMonitor;
        // security data caging
        TFileName                       iFilename;
        TBool                           iCsp;
        // MMS.Content.Upload
        RPointerArray<CSendingServiceInfo> iUploadServices;
        // NCN reset
        CMceEmailNotifHandler			*iEmailNotifHandler;
        CAknQueryDialog*                iPhoneMemoryQuery;
        
        TBool                           iResourceChangeCalled;
        TMsvOp							iMoveOrCopyMailOperation;
        TBool                           iMailboxCreationOn;
        TMsvOp 							iMailboxDeleteOperation; 
        // Owned: Pointer to email API object
        CImumInternalApi*               iEmailApi;

#ifdef RD_READ_MESSAGE_DELETION
        CMsvEntrySelection*             iIdArray;
        TBool                           iHandleNewMsgToInbox;
#endif // RD_READ_MESSAGE_DELETION
        TMsvId                          iAnchorId;
        TInt                            iDiskSpaceForMoveOrCopy;
        CIdle*                          iCancelExitFlagOperation;
        TBool                           iAudioMsgEnabled ;   // CR : 401-1806
        TBool                           iPostcardEnabled ;   // CR : 401-1806
        TBool                           iFeatureMgrEnabled ; // CR : 401-1806
        // ETrue if MCE integration feature is ON. In practice, this means
        // that S60 mailboxes are hided from MCE main view
        TBool iEmailClientIntegration;
        
        // ETrue if Selectable Default Email feature is on        
        TBool iSelectableEmail;
        TBool                           iMsgDeletedStatus; 
        TBool 							iServerStarted ;
        
        /**
         * CMail Changes, it will be ETrue if 
         * KFeatureIdFfEmailFramework is enabled
         */
        TBool                           iEmailFramework;        
        //CR:422-271
        //iFs is needed for opening a .mms file from hardcoded path for mms templates
        RFs                             iFs;
        CBufFlat*                       iEncodeBuffer;
        CMmsClientMtm*                  iMmsMtm;
        /**
         * Iad Update Feature Flag.
         * It will be ETrue, if feature is enabled,
         * otherwise EFalse
         */
        TBool                           iIadUpdateVal;
    };

#endif

// End of file
