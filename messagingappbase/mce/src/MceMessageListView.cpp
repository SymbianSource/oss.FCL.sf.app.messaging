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




// INCLUDE FILES

#include <eikmenub.h>       // menu stuff
#include <apgcli.h>         // RApaLsSession
#include <msvuids.h>
#include <msvids.h>
#include <StringLoader.h>   // StringLoader
#include <aknlists.h>
#include <akntitle.h>       // CAknTitlePane
#include <aknclearer.h>     // CAknLocalScreenClearer
#include <NpdApi.h>         // CNotepadApi
#include <SenduiMtmUids.h>  // mtm uids

#include "MceMessageListContainer.h"
#include "MceOneRowMessageListContainer.h"
#include "MceMessageListbox.h"
#include "MceMessageListView.h"
#include "MceMainViewListView.h"
#include "MceSessionHolder.h"
#include "MceBitmapResolver.h"
#include "mceui.h"
#include "MceCommands.hrh"
#include <mce.rsg>

#include <CommonContentPolicy.h> // banned mime types
#include <MsgBioUids.h>     // KMsgBioUidPictureMsg

#include <bldvariant.hrh>
#include <featmgr.h>
#include <mmsconst.h>
#include <MtmExtendedCapabilities.hrh>

// MMS.Content.Upload
#include <SenduiMtmUids.h>

#include <centralrepository.h>
#include <messaginginternalcrkeys.h>
#include <MessagingDomainCRKeys.h>
#include <messagingvariant.hrh>

#include <AiwServiceHandler.h>
#include <AiwGenericParam.hrh>
#include <AiwVariant.h>
#include <msvuids.h>
#include <SendUiConsts.h>
#include <msvstd.h>

#include <akntoolbar.h>             //CAknToolbar
#include <eikcolib.h>               //CEikAppUiFactory
#include <MNcnInternalNotification.h>
#include <NcnNotificationDefs.h>

class CMsvStore;
#include <muiucontactservice.h> //CMuiuContactService
#include <miuthdr.h>            // CImHeader
#include <msvapi.h>             //TMsvEntry::ReadStoreL
#include <miutpars.h>           //TImMessageField::GetValidInternetEmailAddressFromString(),


#include <bacntf.h> // CEnvironmentChangeNotifier

#include <AknDlgShut.h>         // for AknDialogShutter
#include <messaginginternalpskeys.h> 
// CONSTANTS
const TInt KMceArraysGranularity = 4;

const TInt KMceOnlyInboxInImap = 1;
const TInt KMSKPosition = 3;

// Mail technology type
const TUid KMailTechnologyTypeUid = { 0x10001671 };

// These are for opening the viewer in inbox. 
// try to open viewer immediately if there is no other viewer is open
const TInt KMceLaunchViewerStartTime = 1; 
//0.3 sec delay is taken to ensure previously launched viewer is closed as to avoid the crashes and KErrCancel/KErrInUse errors due to launching of new viewer when currently running 
//viewer is still in the process of exiting
const TInt KMceLaunchViewerStartTimeWhenEditorOpen = 300000; 
const TInt KMceLaunchViewerRetryTime = 500000; // wait 0.5 secs for the next trial
const TInt KMceLaunchViewerRetryCounter = 20; // so editors have approx 10 secs to save...

// This is the paramater used to set mark as read/unread options menu
const TBool KIsEmail = ETrue;

// ================= MEMBER FUNCTIONS =======================


CMceMessageListView* CMceMessageListView::NewL(
    CMsvSessionPtr aSession,
    TMsvId aFolderId,
    CMceSessionHolder& aSessionHolder,
    CMceBitmapResolver& aBitmapResolver,
    CMceMainViewListView& aMainView )
    {
    CMceMessageListView* self = new (ELeave) CMceMessageListView(
        aSession, aFolderId, aSessionHolder, aBitmapResolver, aMainView );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

CMceMessageListView::CMceMessageListView(
    CMsvSessionPtr aSession,
    TMsvId aFolderId,
    CMceSessionHolder& aSessionHolder,
    CMceBitmapResolver& aBitmapResolver,
    CMceMainViewListView& aMainView )
    :
    iSession( aSession ),
    iFolderId( aFolderId ),
    iSessionHolder( aSessionHolder ),
    iBitmapResolver( aBitmapResolver ),
    iSelectedIndex( KErrNotFound ),
    iMainViewListView( aMainView ),
    iSortOrder( EMceCmdSortByDate ),
    iOrdering( EFalse ),
    iContextCommand(EFalse)
    {
    iSessionHolder.AddClient();
    iReadUnread = EFalse;
    }

CMceMessageListView::~CMceMessageListView()
    {
    delete iListboxMailTypeObserver;
    delete iListboxInboxTypeObserver;
    if (iMsgListContainer)
        {
        SetMskObserver(EFalse);
        AppUi()->RemoveFromStack(iMsgListContainer);
    delete iMsgListContainer;
        }
    delete iInboxName;
    delete iRemoteMailboxEmptyText;
    delete iEmptyText;
    delete iMessageViewerLauncher;    
    delete iLocalScreenClearer;
    delete iFolderItemArray;
    delete iDateTimeNotifier;
	delete iAiwServiceHandler;
	RProperty::Delete( KPSUidMuiu, KMceTemplatesDialog ); 	
	RProperty::Delete( KPSUidMuiu, KMuiuOneRowListPopulated );
    iSessionHolder.RemoveClient();
    }

void CMceMessageListView::ConstructL( )
    {
    MCELOGGER_ENTERFN("CMceMessageListView::ConstructL");   
    
    BaseConstructL( R_MCE_FOLDER_VIEW );
    iMceUi = STATIC_CAST( CMceUi*, AppUi() );

    iInboxName = StringLoader::LoadL( R_INBOX_NAME, iEikonEnv );
    iRemoteMailboxEmptyText = StringLoader::LoadL( R_MCE_TEXT_NO_EMAIL, iEikonEnv );
    iEmptyText = StringLoader::LoadL( R_MCE_TEXT_NO_ITEMS, iEikonEnv );
    iFolderItemArray = new(ELeave) CMceFolderItemArray( KMceArraysGranularity );    
    CRepository* repository = CRepository::NewL( KCRUidMuiuVariation );
    TInt featureBitmask = 0;
    TInt error = repository->Get( KMuiuEmailConfigFlags,featureBitmask );
    delete repository;
    repository = NULL;
    if ( error == KErrNone )
        {
        iIsUnread = featureBitmask & KEmailFeatureIdEmailUnreadFunction;
        }
    else
        {
        iIsUnread = EFalse;
        }        
    iListboxMailTypeObserver = CMceListboxTypeObserver::NewL( *this, KMuiuMailMessageListType );
    iListboxInboxTypeObserver = CMceListboxTypeObserver::NewL( *this, KMuiuInboxMessageListType );
    TInt r = RProperty::Define( KPSUidMuiu, KMuiuOneRowListPopulated, RProperty::EInt );
    if ( r != KErrAlreadyExists || r != KErrNone)
        {
        User::LeaveIfError( r );
        }
	// Initialisation		
     r = RProperty::Set( KPSUidMuiu, KMuiuOneRowListPopulated, EFalse );
     if ( r != KErrNone )
         {
         User::LeaveIfError( r );
         }          
    MCELOGGER_LEAVEFN("CMceMessageListView::ConstructL");   
    }

// ----------------------------------------------------
// CMceMessageListView::DoActivateL
// ----------------------------------------------------
void CMceMessageListView::DoActivateL(
    const TVwsViewId& aPrevViewId,
    TUid aCustomMessageId, /* Folder ID */
    const TDesC8& /*aCustomMessage*/)
    {

    MCELOGGER_ENTERFN("CMceMessageListView::DoActivateL()");

    // if new message is shown during disconnecting
    // prevent to exit from mce    
    iMceUi->ClearExitAfterDisconnectFlag();

    // Save the sort ordering in the message store
    // Set the default sort order
    iSortOrder = EMceCmdSortByDate;
    iOrdering = EFalse;
    if ( iMsgListContainer )
        {
        TMsvId fId = iMsgListContainer->CurrentFolderId();
        for( TInt loop = 0; loop < iFolderItemArray->Count() ; loop++ )
            {
            TMceFolderItem item = ((*iFolderItemArray)[loop]);
            if ( item.iFolderId == fId && aPrevViewId == TDRVIEWID)
                {        
                iSortOrder = item.iSortOrder;
                iOrdering = item.iOrdering;
                }
            }
        }
    if (aPrevViewId != TDRVIEWID ) 
        {
        // Reset the sort order
    	iFolderItemArray->Reset();
        }
	    
    if ( iMsgListContainer )
        {
        iMsgListContainer->SetSortTypeL( iSortOrder, iOrdering );
        }
        
    TMsvId id = 0;
    TBool editorLaunched = EFalse;
    TBool launchingFromOutside = EFalse;  
    TMsvId service = KMsvLocalServiceIndexEntryId;
    TMsvEntry entry;    
    TInt msgMtmUid = 0;
    
    if ( aCustomMessageId.iUid > KMsvRootIndexEntryId )
        {

        if ( iSession->GetEntry( aCustomMessageId.iUid, service, entry ) != KErrNone )
            {
            iSession->GetEntry( KMsvGlobalInBoxIndexEntryId, service, entry ); // this should always succeed!
            }

        // outside the app.
        launchingFromOutside = ETrue;
        // Set the forder containing the entry
        if ( entry.iType == KUidMsvMessageEntry )
            {
            // The aCustomMessageId is an ID of a message, set Inbox
            SetFolderL( entry.Parent() );
            id = aCustomMessageId.iUid;
            }
        else
            {
            // Set the folder based on the given ID
            iFolderId = aCustomMessageId.iUid;
            // this is performance optimization, start to launch viewer before container creation
            if ( !iMsgListContainer && entry.Id() == KMsvGlobalInBoxIndexEntryId )
                {
                if ( LaunchViewerWhenOneUnreadL( msgMtmUid ) > KErrNotFound )
                    {
                    editorLaunched = ETrue;
                    }
                }
            }
        iMceUi->SetDontExitOnNextOperationComplete();
        }
    
    if ( iMsgListContainer &&
            iCurrentListType != GetFolderListBoxType())
        {
        ListboxTypeChangedL();
        }
    else
        {
        CreateListboxL();        
        }           
    if ( iMsgListContainer && aPrevViewId == TDRVIEWID )
        {
        // Save the sort ordering in the message store
        // Set the default sort order
        iSortOrder = EMceCmdSortByDate;
        iOrdering = EFalse;
        TMsvId fId = iFolderId;
        for( TInt loop = 0; loop < iFolderItemArray->Count() ; loop++ )
            {
            TMceFolderItem item = ((*iFolderItemArray)[loop]);
            if ( item.iFolderId == fId )
                {        
                iSortOrder = item.iSortOrder;
                iOrdering = item.iOrdering;
                }
            }
     
        iMsgListContainer->SetSortTypeL( iSortOrder, iOrdering );
        }
    
    iMsgListContainer->SetFolderL( iFolderId );
            
    // Unset flag so one row list container knows that first subtitle is to be opened
    iMsgListContainer->SetContainerFlag( EMceOneRowFlagFirstSubtitleOpen, EFalse );         
    
    iMsgListContainer->ClearSelection();
    iMsgListContainer->SetRect(ClientRect());
        
    SetMskObserver( ETrue );
    AppUi()->RemoveFromStack( iMsgListContainer );
    AppUi()->AddToStackL( *this,iMsgListContainer );
    iMsgListContainer->ActivateL();
    iMsgListContainer->MakeVisible(ETrue);

    //const TMsvEntry& folderEntry = iMsgListContainer->FolderEntry(); 

    entry = iMsgListContainer->FolderEntry();
    
    if ( entry.Parent() == KMsvRootIndexEntryId &&
         entry.iType == KUidMsvServiceEntry &&
         entry.iMtm == KSenduiMtmImap4Uid &&
         iMsgListContainer->Count() )
        {        
        // TODO: container and array should handle this?
        FindInboxAndOpenItL();// special handling for imap4 Inbox, folderEntry -> empty
        }

    if ( IsSyncMl() && launchingFromOutside )
        {           
        SyncMlFindInboxAndOpenItL();
        }
        
    
    // Handle the resource change
    if ( iMceUi->ResourceChangeCalled() )
        {
        iMsgListContainer->HandleResourceChange( KEikDynamicLayoutVariantSwitch );
        iMceUi->ResetResourceChange();
        }

    SetEmptyTextL();

    iMsgListContainer->ResetCurrentItemL();
    
    // Fetch pointer to the default title pane control
    CAknTitlePane* title = iMceUi->TitlePaneL();
    TMsvEntry serviceEntry;
    TMsvId    serviceId;
    
    entry = iMsgListContainer->FolderEntry();
    
    if ( (entry.iMtm == KSenduiMtmImap4Uid || IsSyncMl() ) &&
         iSession->GetEntry( entry.iServiceId, serviceId, serviceEntry) == KErrNone )
        {
        title->SetTextL( serviceEntry.iDetails );
        }
    else
        {
        title->SetTextL( entry.iDetails );
        }

    // Set the MSK for the view
    SetMSKButtonL();

    iMceViewActivated = ETrue;

    TBool activateConnQueryTimer = ETrue;

    if ( launchingFromOutside )
        {
        iMceUi->RemoveTabs(); // just to make sure, doesn't do anything if no tabs
        const TMsvId entryId = entry.Id();

        if ( entry.Parent() == KMsvRootIndexEntryId ||
            ( entry.iMtm == KSenduiMtmImap4Uid && !IsImapFolderOpenL( ) ) ||
            entryId == KMsvGlobalInBoxIndexEntryId ||
            entryId == KMsvGlobalOutBoxIndexEntryId ||
            entryId == KMsvDraftEntryId ||
            entryId == KMsvSentEntryId ||
            entryId == KMceDocumentsEntryId ||
            ( IsSyncMl() && entry.iRelatedId == KMsvGlobalInBoxIndexEntryId ) ) 
            {
            if ( entry.iMtm == KSenduiMtmImap4Uid && entry.iDetails.CompareF( iInboxName->Des() ) == 0 )
                {
                iMceUi->ShowTabsL( serviceId ); // this was got above so this points to correct service.
                }
            else
                {
                iMceUi->ShowTabsL( aCustomMessageId.iUid );
                }
            }
    	else
    	    {
    	    ChangeFolderL(ETrue);
    	    }			
			
        if ( !editorLaunched )
            {
            if ( iFolderId == KMsvGlobalOutBoxIndexEntryId ||
                 iFolderId == KMsvGlobalInBoxIndexEntryId ||
                    ( entry.Parent() == KMsvRootIndexEntryId &&
                        ( iMceUi->CheckTechnologyTypeFromMtm( KMailTechnologyTypeUid, entry.iMtm ) ) &&
                        entry.iType == KUidMsvServiceEntry ) ||
                    ( SyncMlOutboxInbox( KMsvGlobalInBoxIndexEntryId ) &&
                        entry.iType == KUidMsvServiceEntry) )
                {
                if ( !FindUnreadMessageAndOpenItL( id ) )
                    {
                    activateConnQueryTimer = ETrue;
                    }
                else
                    {
                    activateConnQueryTimer = EFalse;
                    }
                }
            }
        }


    iMceUi->SetMceViewActive( EMceMessageViewActive );
    if ( editorLaunched &&
         ( msgMtmUid!=KSenduiMtmIrUidValue ) &&
         ( msgMtmUid!=KSenduiMtmBtUidValue ) &&
         ( msgMtmUid!=KSenduiMtmSmtpUidValue ) &&
         ( msgMtmUid!=KSenduiMtmImap4UidValue ) &&
         ( msgMtmUid!=KSenduiMtmPop3UidValue ))
        {
        // This is added to remove Inbox view flicker when new message is opened from home screen and 
        // forwarded
        delete iLocalScreenClearer;
        iLocalScreenClearer = NULL;
        iLocalScreenClearer = CAknLocalScreenClearer::NewL( EFalse ); 
        }
    
    if ( activateConnQueryTimer )
        {
        if ( entry.iMtm == KSenduiMtmImap4Uid || 
            entry.iMtm == KSenduiMtmPop3Uid )
            {
            iConnectionNote = ETrue;
            iMceUi->ActivateConnectionQueryTimerL( iFolderId );
            }        
        }

    iMsgListContainer->RefreshSelectionIndexesL ( EFalse );
    iMsgListContainer->RefreshListbox();

    // NCN reset, if the mail folder contains unread messages
	if ( iMceUi->CheckTechnologyTypeFromMtm( KMailTechnologyTypeUid, entry.iMtm ) &&
	    HasUnreadMessagesL() )
        {
        iMceUi->HandleNotif(iMsgListContainer->FolderEntry().iServiceId);
        }
				
    if ( !iDateTimeNotifier )
        {
        iDateTimeNotifier = CEnvironmentChangeNotifier::NewL( 
            CActive::EPriorityLow,
            TCallBack( EnvironmentChanged, this ) );
        }
        
    if ( !iDateTimeNotifier->IsActive() )
        {
        iDateTimeNotifier->Start();            
        }
     
    // if we are in marking mode then "press" rsk
    // cancel to return to normal mode
    if( iMarkingModeOn )
        {
        SetMarkingModeOff();
        }
    MCELOGGER_LEAVEFN("CMceMessageListView::DoActivateL()");
    }


// ----------------------------------------------------
// CMceMessageListView::DoDeactivate
// ----------------------------------------------------
void CMceMessageListView::DoDeactivate()
    {
    if (iMsgListContainer)
        {
        delete iDateTimeNotifier;
        iDateTimeNotifier = NULL;
        iMsgListContainer->MakeVisible(EFalse);
        AppUi()->RemoveFromStack(iMsgListContainer);
        delete iMsgListContainer;
        iMsgListContainer = NULL;
        }
    iMceViewActivated = EFalse;
    }


// ----------------------------------------------------
// CMceMessageListView::HandleCommandL
// ----------------------------------------------------
void CMceMessageListView::HandleCommandL( TInt aCommand )
    {
    MCELOGGER_WRITE_FORMAT("CMceMessageListView::HandleCommandL: aCommand: %d", aCommand);

    if ( !iMceViewActivated )
        {
        return;
        }

    switch (aCommand)
        {
        case EAknSoftkeyBack:
            if ( FolderOpenedL() )
                {
                ChangeFolderL( ETrue );
                }
            else
                {
                iConnectionNote = EFalse;
                HandleCloseL();
                }
            break;
        case EAknCmdOpen:
        case EAknSoftkeyContextOptions:
            HandleOpenL();
            break;
        case EAknCmdHideInBackground:
            // handled by app ui and sent directly to appui by avkon framework so ignore this by view.
            break;
        case EMceCmdConnect:
            {
            if ( iMsgListContainer->FolderEntry().iMtm == KSenduiMtmImap4Uid ||
                 iMsgListContainer->FolderEntry().iMtm == KSenduiMtmPop3Uid )
                {
                iMceUi->GoOnlineL( iMsgListContainer->FolderEntry().iServiceId );
                }
            break;
            }
        case EMceCmdCloseConnection:
            {
            if ( iMsgListContainer->FolderEntry().iMtm == KSenduiMtmImap4Uid ||
                 iMsgListContainer->FolderEntry().iMtm == KSenduiMtmPop3Uid )
                {
                iMceUi->GoOfflineL( iMsgListContainer->FolderEntry().iServiceId );
                }
            break;
            }          
        case  EAknCmdMarkReadMsgs:
            {
            FindAllReadMsgAndMarkL();
            break;
            }

        case EMceCmdSendViaIr:
            iMceUi->SendViaL( iMsgListContainer->CurrentItemId(), KSenduiMtmIrUid );
            break;

        case EMceCmdSendViaBt:
            iMceUi->SendViaL( iMsgListContainer->CurrentItemId(), KSenduiMtmBtUid );
            break;


        case EMceCmdSortByDate:
        case EMceCmdSortBySubject:
        case EMceCmdSortByRecipient:
        case EMceCmdSortBySender:
        case EMceCmdSortByType:
            HandleSortCommandL( aCommand );
            break;
            
        case EMceCommandMMSContentUpload:
            // MMS.Content.Upload
            iMceUi->ShowUploadQueryL();
            break;
        case EMceCmdDelete:
            // Cannot delete while the message viewer is being launched by the NCN
            if ( !iMessageViewerLauncher )
                {
                iMceUi->HandleCommandL( aCommand );
                }
            break;

        case EMceHCListCmdExpand:
        case EMceHCListCmdCollapse:
        case EMceHCListCmdCollapse1:
        case EMceHCListCmdCollapse2:
            HandleContainerOptionsCommandL( aCommand );
            break;

        case EMceCmdNewFolder:
        case EMceCmdRenameFolder:
            iMsgListContainer->ClearSelection();
            // drop through
        default:
        	if ( iAiwServiceHandler && FeatureManager::FeatureSupported(KFeatureIdSyncMlDsEmail) && 
        		 KAiwCmdSynchronize ==  iAiwServiceHandler->ServiceCmdByMenuCmd(aCommand)) 
        		{
        		TInt appId = EGenericParamMessageItemEMail;
        		const TMsvEntry entry = iMsgListContainer->FolderEntry();
        		CAiwGenericParamList* list = AiwSyncParamListLC( appId, entry.iDetails );
            	
            	iAiwServiceHandler->ExecuteMenuCmdL(aCommand, *list,
												iAiwServiceHandler->OutParamListL() );
				CleanupStack::PopAndDestroy(list);
        		}
        	// NSS Handling MessageReader AIW command
			else if( iAiwServiceHandler && KAiwCmdView ==  iAiwServiceHandler->ServiceCmdByMenuCmd(aCommand))
				{
				/** 
				 * @todo Fetching and packing message list is duplicated in 
				 * HandleCommandL and in DynInitMenuPaneL. should be extracted
				 * into a separate method. I don't dare to introduce a new 
				 * method. Let the mce team care about it
				 */
				CMsvEntrySelection* itemIds = iMsgListContainer->CurrentItemSelectionL();
				CleanupStack::PushL( itemIds );
				
				CAiwGenericParamList& inList = iAiwServiceHandler->InParamListL();
				
				for ( TInt i( 0 ); i < itemIds->Count(); i++ ) 
				    {
				    TMsvId itemId ( (*itemIds)[i] );
				    TMsvEntry currentEntry;
				    TMsvId serviceId; // not used here but needed by GetEntry function
				    
				    if ( iSession->GetEntry( itemId, serviceId, currentEntry ) == KErrNone ) 
                        {
                        // Pack the TMsvEntry into the TAiwVariant and add it to the param list
                        TPckgC<TMsvEntry> packedEntry ( currentEntry );
                        
                        // does not copy the data
                        TAiwVariant varEntry( packedEntry ); 
                        TAiwGenericParam parEntry( EGenericParamUnspecified, varEntry );
                        
                        // copies the data
                        inList.AppendL( parEntry );  
                        }  // if
                        // message reader wise ignore the error
                        // @todo log the error according to the mce team standards
				    }  // for
				
				CleanupStack::PopAndDestroy( itemIds );
				
				iAiwServiceHandler->ExecuteMenuCmdL( aCommand, inList, 
				                                     iAiwServiceHandler->OutParamListL() );
				}
			// NSS End of handling MessageReader AIW command
			else
				{
				iMceUi->HandleCommandL( aCommand );
				}
            break;
        }
    MCELOGGER_LEAVEFN("CMceMessageListView::HandleCommandL()");
    }

// ----------------------------------------------------
// CMceMessageListView::HandleSessionEventL
// ----------------------------------------------------
void CMceMessageListView::HandleSessionEventL(
    TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3)
    {
    MCELOGGER_WRITE_TIMESTAMP("CMceMessageListView::HandleSessionEventL() start");
    switch ( aEvent )
        {
    case EMsvEntriesDeleted:
        {
        // check that we don't have deleted folder open
        CMsvEntrySelection* selection = (CMsvEntrySelection*) aArg1;
        if ( iMceViewActivated && selection->Find( iFolderId ) != KErrNotFound )
            {
            // opened folder is deleted, must change to documents
            SetFolderL( KMceDocumentsEntryId );
            iMceUi->ShowTabsL( KMceDocumentsEntryId );
            CAknTitlePane* title=iMceUi->TitlePaneL();
            TMsvEntry documentsEntry;
            TMsvId serviceId;
            if ( iSession->GetEntry( KMceDocumentsEntryId, serviceId, documentsEntry)
                 == KErrNone )
                {
                title->SetTextL( documentsEntry.iDetails );
                }
            if ( iMsgListContainer )
                {
                iMsgListContainer->SetFolderL( KMceDocumentsEntryId );
                iMsgListContainer->ClearSelection();
                }
            }
            
        if ( /*!iMceUi->ViewUpdateSuppressionFlag() &&*/
             iMsgListContainer &&
             iFolderId == (*(TMsvId*) (aArg2)) )
            {
            iMsgListContainer->HandleMsvSessionEventL( aEvent, *selection, !iMceUi->ViewUpdateSuppressionFlag() );
            //HandleEntriesDeletedL();
            SetMSKButtonL();
            //this has been commented due to toolbar not showing up after opening a message and deleting and come back to main view
            //UpdateToolbarL();
            }

        }
        break;
    case EMsvEntriesCreated:    
    case EMsvEntriesChanged:
        {
        const TMsvId parentId = (*(TMsvId*) aArg2);
        CMsvEntrySelection* selection = (CMsvEntrySelection*) aArg1;
        MessageViewHandleEntriesCreatedOrChangedL( parentId, selection);
        if ( iMsgListContainer )
            {
            // Set the middle soft key
            SetMSKButtonL();
            
            if ( iFolderId == (*(TMsvId*) (aArg2)) )
                {
            	iMsgListContainer->HandleMsvSessionEventL( aEvent, *selection, !iMceUi->ViewUpdateSuppressionFlag() );
                }
            
            // Imap: Refresh list         
            if ( iMsgListContainer->FolderEntry().iMtm == KSenduiMtmImap4Uid )
                {
                iMsgListContainer->SetRect( ClientRect() );
                iMsgListContainer->DrawDeferred();
                }
            }
        }
        
        break;
    case EMsvEntriesMoved:
        {
        const TMsvId parentId = (*(TMsvId*) aArg2);
        CMsvEntrySelection* selection = (CMsvEntrySelection*) aArg1;
        if ( iMsgListContainer )
            {
            // Set the middle soft key
            SetMSKButtonL();
            if ( parentId == iFolderId )
                {
                iMsgListContainer->HandleMsvSessionEventL( EMsvEntriesCreated, *selection, !iMceUi->ViewUpdateSuppressionFlag() );
                }
            else if ( (*(TMsvId*) (aArg3)) == iFolderId )
                {
                iMsgListContainer->HandleMsvSessionEventL( EMsvEntriesDeleted, *selection, !iMceUi->ViewUpdateSuppressionFlag() );
                }

            // Folder string update for one row list
            if (iMsgListContainer->MceListId() == EMceListTypeOneRow )
                {
                TMsvId service;
                TMsvEntry toFolder;
                if ( iSession->GetEntry( parentId, service, toFolder ) == KErrNone )
                    {            
                    // Is target folder subfolder for current folder
                    if ( toFolder.Parent() == iFolderId )    
                        {
                        CMsvEntrySelection* targetSelection = new(ELeave) CMsvEntrySelection();
                        CleanupStack::PushL( targetSelection );
                        targetSelection->AppendL( parentId );
                        // Send EMsvEntriesChanged to one row message list container
                        // with move target folder information to get folder string updated
                        iMsgListContainer->HandleMsvSessionEventL( EMsvEntriesChanged, *targetSelection,
                                                             !iMceUi->ViewUpdateSuppressionFlag() );
                        CleanupStack::PopAndDestroy( targetSelection );
                        }
                    }
                }         
            }
        }
        
        break;

    case EMsvCloseSession:
        {        
        if (iMsgListContainer)
            {
//TODO
//            CMceMessageListItemArray* listArray = iMsgListContainer->ListItems();
//            listArray->HandleSessionEventL( aEvent, aArg1, aArg2, aArg3 );
            }

        iSession->RemoveObserver( *this );        
        }
        break;

    case EMsvMediaChanged:
        {
        if (iMceUi->MceViewActive(EMceMessageViewActive) )
            {            

            // this is because DoDeactivate saves current index and when returning to main view
            // we want to reset current index
            iMceUi->SetCustomControl(1);
            iSelectedIndex = 0;
            iMainViewListView.SetSelectedFolderId( iFolderId );
            iMceUi->ActivateLocalViewL( KMceMainViewListViewId );
            iMceUi->RemoveTabs();         
            }
        }
        break;
        
    default:
        break;

        };
    UpdateViewL();
    MCELOGGER_WRITE_TIMESTAMP("CMceMessageListView::HandleSessionEventL() end");
    }

// ----------------------------------------------------
// CMceMessageListView::ProcessCommandL
// ----------------------------------------------------
void CMceMessageListView::ProcessCommandL(TInt aCommand)
    {
    iMceUi->CancelMailboxTimer();
    MCELOGGER_WRITE("CMceMessageListView::ProcessCommandL");

    if ( ( !iMessageViewerLauncher || ( iMessageViewerLauncher && !iMessageViewerLauncher->IsActive() ) ) &&
         !iMceUi->IsEditorOpen() )
        {
        if ( aCommand == EAknSoftkeyContextOptions )
            {
            // Handle the context sensitive menu
            iFetchMenuSelectionKeyPressed = ETrue;
            iHideExitCommand = ETrue;
            aCommand = EAknSoftkeyOptions;
            if(iFolderId == KMsvGlobalOutBoxIndexEntryIdValue)
                iContextCommand = ETrue;
			MenuBar()->SetMenuType(CEikMenuBar::EMenuContext);
            }
        CAknView::ProcessCommandL( aCommand );
        MenuBar()->SetMenuType(CEikMenuBar::EMenuOptions);
        }
#ifdef _DEBUG
    else
        {
        MCELOGGER_WRITE("MCE: Editor is open so don't call CAknView::ProcessCommandL");
        }
#endif
    }


// ----------------------------------------------------
// CMceMessageListView::Id
// ----------------------------------------------------
TUid CMceMessageListView::Id() const
    {
    return KMceMessageListViewId;
    }

// ----------------------------------------------------
// CMceMessageListView::HandleListBoxEventL
// ----------------------------------------------------
void CMceMessageListView::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType )
    {
    switch(aEventType)
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            iMceUi->CancelMailboxTimer();
            HandleOpenL();
        break;
        default:
        break;
        }
    }

// ----------------------------------------------------
// CMceMessageListView::HandleCloseL
// ----------------------------------------------------
void CMceMessageListView::HandleCloseL( )
    {
    // this is because DoDeactivate saves current index and when returning to main view
    // we want to reset current index
    iSelectedIndex = 0;
    if ( iMsgListContainer->FolderEntry().iMtm == KUidMsvLocalServiceMtm )
        {
        iMainViewListView.SetSelectedFolderId( iFolderId );
        }
    else
        {
        // we are closing service and we should inform main view that
        // service id, not folder id.
        iMainViewListView.SetSelectedFolderId( iMsgListContainer->FolderEntry().iServiceId );
        }
    iMceUi->ActivateLocalViewL( KMceMainViewListViewId );
    iMceUi->RemoveTabs();
    }

// ----------------------------------------------------
// CMceMessageListView::ChangeFolderL
// ----------------------------------------------------
void CMceMessageListView::ChangeFolderL( TBool aOpenParent )
    {
    if ( aOpenParent && FolderOpenedL() )
        {
        iMceUi->RemoveFolderTabs();
        ChangeFolderAndRefreshListboxL( iMsgListContainer->FolderEntry().Parent() );
        const TMsvEntry& entry = iMsgListContainer->FolderEntry();
        const TMsvId entryId = entry.Id();
        if ( entry.Parent() == KMsvRootIndexEntryId ||
            ( entry.iMtm == KSenduiMtmImap4Uid && !IsImapFolderOpenL( ) ) ||
            entryId == KMsvGlobalInBoxIndexEntryId ||
            entryId == KMsvGlobalOutBoxIndexEntryId ||
            entryId == KMsvDraftEntryId ||
            entryId == KMsvSentEntryId ||
            entryId == KMceDocumentsEntryId ) 
            {
            iMceUi->ShowTabsL( entry.Id() );
            }  
        }
    else if ( !aOpenParent )
        {
        iMceUi->RemoveTabs();
        TMsvEntry child;
        TMsvId serviceId;
        TInt error = iSession->GetEntry( iMsgListContainer->CurrentItemId(), serviceId, child );
        if ( error == KErrNone )
            {
            if ( child.Id() == KMceTemplatesEntryId )
                {

				TInt r = RProperty::Define( KPSUidMuiu, KMceTemplatesDialog, RProperty::EInt );
				if ( r != KErrAlreadyExists )
				    {
				    User::LeaveIfError( r );
				    }
				if ( iCurrentListType == EMceListTypeOneRow)
				    {
				    r = RProperty::Set( KPSUidMuiu, KMceTemplatesDialog, MceTemplatesDialogOpen );
				    }
				
                if ( CNotepadApi::ExecTemplatesL() == EAknSoftkeyExit )
                    {
                    iAvkonViewAppUi->ProcessCommandL( EAknCmdExit );
                    }

				if ( iCurrentListType == EMceListTypeOneRow)
				    {
				    r = RProperty::Set( KPSUidMuiu, KMceTemplatesDialog, MceTemplatesDialogClose );
				    }
                if ( !(iMceUi->MceViewActive( EMceMainViewActive )))
                    iMceUi->ShowTabsL( iMsgListContainer->FolderEntry().Id() );
                ListContainer()->DrawDeferred();
                }
            else if ( child.iType == KUidMsvFolderEntry ||
                child.iType == KUidMsvServiceEntry ) // this should not be possible
                {
                ChangeFolderAndRefreshListboxL( child.Id() );
                }
            } // end if error == KErrNone
        } // else
    }

// ----------------------------------------------------
// CMceMessageListView::DynInitMenuPaneL
// ----------------------------------------------------

void CMceMessageListView::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    TInt listPopulated = 1;
    //Get the value, accordingly we can dim/undim mark/unmark for one row list only
    if ( iCurrentListType == EMceListTypeOneRow )
        {
        TInt r = RProperty::Get(KPSUidMuiu, KMuiuOneRowListPopulated,listPopulated);
        if ( r!= KErrNone )
            {
            User::LeaveIfError(r);
            }
        }
    if ( !iMceViewActivated )
        {
        return;
        }
        
    if ( !iAiwServiceHandler )
		{
		iAiwServiceHandler = CAiwServiceHandler::NewL();
		}
		
	if ( !iListViewFlags.MceFlag(EMceUiFlagsMainViewAiwCreated) )
		{
		iAiwServiceHandler->AttachMenuL( R_MCE_FOLDER_MENU, R_AIWMCE_INTEREST );
		iListViewFlags.SetMceFlag(EMceUiFlagsMainViewAiwCreated);
		}

    if (iAiwServiceHandler->HandleSubmenuL(*aMenuPane))
		{
		return;
		}
        
    switch ( aResourceId )
        {
        case R_MCE_FOLDER_MENU:
            aMenuPane->SetItemDimmed( EAknCmdHelp,
                !FeatureManager::FeatureSupported( KFeatureIdHelp ) );

            iMsgListContainer->MarkItemSelectionL();
            if ( !iHideExitCommand &&
                 !iMsgListContainer->DisplayOptionsMenuFromSelectionKey() &&
                 iMsgListContainer->CurrentItemType() == CMceMessageListContainerBase::EMessageListItemMsvItem
               )
                {
                iMceUi->DynInitMenuPaneL( aResourceId, aMenuPane );
                }
            // MMS.Content.Upload
            aMenuPane->SetItemDimmed( EMceCommandMMSContentUpload, !FeatureManager::FeatureSupported(KFeatureIdSenduiMmsUpload) );
            if ( !iMceUi->NumUploadServices() )
            	{
            	aMenuPane->SetItemDimmed( EMceCommandMMSContentUpload, ETrue );
            	}

            FolderMenuL( aMenuPane );
            
            //multiselection, handle MMMbox item
            iMceUi->AddMultiselectionMTMFunctionsL(*aMenuPane, EAknCmdOpen);

            // NSS Fetch the MessageReader menu
            if ( iAiwServiceHandler->IsAiwMenu( aResourceId ) )
                {
                /** 
				 * @todo Fetching and packing message list is duplicated in 
				 * HandleCommandL and in DynInitMenuPaneL. should be extracted
				 * into a separate method. I don't dare to introduce a new 
				 * method. Let the mce team care about it
				 */
				CMsvEntrySelection* itemIds = iMsgListContainer->CurrentItemSelectionL();
				CleanupStack::PushL( itemIds );
				
				CAiwGenericParamList& inList = iAiwServiceHandler->InParamListL();
				
				for ( TInt i( 0 ); i < itemIds->Count(); i++ ) 
				    {
				    TMsvId itemId ( (*itemIds)[i] );
				    TMsvEntry currentEntry;
				    TMsvId serviceId; // not used here but needed by GetEntry function
				    
				    if ( iSession->GetEntry( itemId, serviceId, currentEntry ) == KErrNone ) 
                        {
                        // Pack the TMsvEntry into the TAiwVariant and add it to the param list
                        TPckgC<TMsvEntry> packedEntry ( currentEntry );
                        
                        // does not copy the data
                        TAiwVariant varEntry( packedEntry ); 
                        TAiwGenericParam parEntry( EGenericParamUnspecified, varEntry );
                        
                        // copies the data
                        inList.AppendL( parEntry );  
                        }  // if
                        // message reader wise ignore the error
                        // @todo log the error according to the mce team standards
				    }  // for
				
				CleanupStack::PopAndDestroy( itemIds );
								
				iAiwServiceHandler->InitializeMenuPaneL( *aMenuPane, aResourceId, 
				                                         EMceCmdFirstMessageReaderAiwCommand,
				                                         inList,
				                                         EFalse,
				                                         ETrue);
                HandleContainerOptionsMenuL( aMenuPane );				                                         
                }
            
            break;

        case R_MCE_FETCH_MENU:
            FetchMenuL( aMenuPane );
            break;        

        case R_MCE_SENDVIA_MENU:
            aMenuPane->SetItemDimmed( EMceCmdSendViaIr,
                !FeatureManager::FeatureSupported( KFeatureIdIrda ) );
            aMenuPane->SetItemDimmed( EMceCmdSendViaBt,
                !FeatureManager::FeatureSupported( KFeatureIdBt ) ); 
            
            break;

        case R_MCE_SORT_BY_MENU:
            {
            if ( ( iFolderId != KMsvSentEntryId  ) &&
                 ( iFolderId != KMsvDraftEntryId ) &&
                 ( iFolderId != KMsvGlobalOutBoxIndexEntryId) )
                {
                aMenuPane->DeleteMenuItem(EMceCmdSortByRecipient);
                aMenuPane->SetItemDimmed( EMceCmdSortBySender, EFalse );
                }
            else
                {
                aMenuPane->DeleteMenuItem(EMceCmdSortBySender);
                aMenuPane->SetItemDimmed( EMceCmdSortByRecipient, EFalse );    
                }            

            if ( IsMSKMailBox() ) 
                {
                // Mail folder, only single message type
                aMenuPane->SetItemDimmed( EMceCmdSortByType, ETrue );
                }
            else
                {
                // Inbox etc.
                aMenuPane->SetItemDimmed( EMceCmdSortByType, EFalse );
                }
            aMenuPane->SetItemButtonState( iSortOrder, EEikMenuItemSymbolOn );
            break;
            }
        case R_AVKON_MENUPANE_MARK_MULTIPLE:
            {
            TInt count = iMsgListContainer->SelectionCount();
            TInt pos( 0 );
			// if Mark all exists and also total selection count and folder count is equal
			// Message list count meaning all items in the list are marked, hence dim mark all item
            if ( aMenuPane->MenuItemExists( EAknCmdMarkingModeMarkAll, pos ) &&
                    ( count + SubfolderCount() ) == iMsgListContainer->Count() )
               {
               //if all selected, disable mark all
               aMenuPane->SetItemDimmed( EAknCmdMarkingModeMarkAll, ETrue );
               } 
            break;
            }
        case R_MCE_NEW_MESSAGE_MENU:
        default:
            iMceUi->DynInitMenuPaneL( aResourceId, aMenuPane );
            iMsgListContainer->SetContainerFlag( EMceOneRowFlagOptionsMenuOpen, ETrue );
            break;
        }
    }
    
// ----------------------------------------------------
// CMceMessageListView::HandleContainerOptionsMenuL
// ----------------------------------------------------
void CMceMessageListView::HandleContainerOptionsMenuL( CEikMenuPane* aMenuPane )
    {
    TUint validCommands = iMsgListContainer->OptionsCommandSupported();
    
    aMenuPane->SetItemDimmed( EMceHCListCmdExpand, !(validCommands & CMceMessageListContainerBase::TMessageListCommandExpand ));
    aMenuPane->SetItemDimmed( EMceHCListCmdCollapse1, !(validCommands & CMceMessageListContainerBase::TMessageListCommandCollapse1 ));
    aMenuPane->SetItemDimmed( EMceHCListCmdCollapse2, !(validCommands & CMceMessageListContainerBase::TMessageListCommandCollapse2 ));
    
    }
    
// ----------------------------------------------------
// CMceMessageListView::HandleContainerOptionsCommandL
// ----------------------------------------------------
void CMceMessageListView::HandleContainerOptionsCommandL( TInt aCommand )
    {
    TInt containerCommand = 0;
    switch ( aCommand )
        {
        case EMceHCListCmdExpand:
            containerCommand = CMceMessageListContainerBase::TMessageListCommandExpand;
            break;
        case EMceHCListCmdCollapse:
        case EMceHCListCmdCollapse1:
        case EMceHCListCmdCollapse2:
            containerCommand = CMceMessageListContainerBase::TMessageListCommandCollapse1;
            break;
        default: 
            break;
        }
    if ( containerCommand > 0 )
        {
        iMsgListContainer->HandleOptionsCommandL( containerCommand );
        }
    }
    

// ----------------------------------------------------
// CMceMessageListView::EditEntryL
// ----------------------------------------------------
void CMceMessageListView::EditEntryL()
    {
    if ( iMceUi->IsEditorOpen() )
        {
        MCELOGGER_WRITE("MCE: EditEntry: do not open because already editing one");
        return;
        }

    TMsvId id = iMsgListContainer->CurrentItemId();
    if ( id < KMsvRootIndexEntryId )
        {
        return;
        }
    TMsvEntry currentEntry;
    TMsvId serviceId; // not used here but needed by GetEntry function
    if ( iSession->GetEntry( id, serviceId, currentEntry ) == KErrNone &&
         currentEntry.iType == KUidMsvMessageEntry )
        {
    	if ( (currentEntry.iMtm==KSenduiMtmImap4Uid) || (currentEntry.iMtm==KSenduiMtmPop3Uid) ||
    	 	 (currentEntry.iMtm==KSenduiMtmSyncMLEmailUid)||
    	 	 iMceUi->CheckTechnologyTypeFromMtm( KMailTechnologyTypeUid, currentEntry.iMtm ) )
    		{
    		// Mailbox entry
    		if ( currentEntry.Unread() )
    			{
    			// NCN reset, opening the unread entry
    			
    			iMceUi->HandleNotif(iMsgListContainer->FolderEntry().iServiceId);
    			}
    		}

        // just to make sure...
        iMceUi->CancelMailboxTimer();
        iRunningEditFunction = ETrue;
        TRAPD( error, iMceUi->EditMTMEntryL( currentEntry ) );        
        iRunningEditFunction = EFalse;
        User::LeaveIfError( error );
        }

    }


// ----------------------------------------------------
// CMceMessageListView::HandleOpenL
// ----------------------------------------------------
void CMceMessageListView::HandleOpenL()
    {
    if ( !iMsgListContainer->DisplayOptionsMenuFromSelectionKey() )
        {
        CMsvEntrySelection* selection = iMsgListContainer->CurrentItemSelectionRefreshL();
        CleanupStack::PushL( selection );
        iHideExitCommand = EFalse;
        TMsvEntry currentEntry;
        TMsvId serviceId; // not used here but needed by GetEntry function
        if ( selection->Count() &&
            iSession->GetEntry( selection->At(0), serviceId, currentEntry ) == KErrNone )
            {
            switch ( currentEntry.iType.iUid )
                {
                case KUidMsvMessageEntryValue:
                    if (( currentEntry.Parent() != KMsvGlobalOutBoxIndexEntryId) && (iFolderId != KMsvGlobalOutBoxIndexEntryId))
                        {
                        EditEntryL();
                        }
                    else // Open context sensitive menu
                        {
                        ProcessCommandL( EAknSoftkeyContextOptions );
                        }                        
                break;
                case KUidMsvFolderEntryValue:
                    ChangeFolderL();
                break;
                default:
                break;
                }
            }
        CleanupStack::PopAndDestroy( selection );
        }
    else
        {
        iHideExitCommand = ETrue;
        ProcessCommandL( EAknSoftkeyContextOptions );
        }
    }

// ----------------------------------------------------
// CMceMessageListView::FetchMenuL
// ----------------------------------------------------
void CMceMessageListView::FetchMenuL( CEikMenuPane* aMenuPane ) const
    {
    if ( iMsgListContainer->CurrentItemSelectionCount() == 0 )
        {
        aMenuPane->SetItemDimmed( EMceCmdFetchSelected, ETrue );
        }
    else if ( iFetchMenuSelectionKeyPressed )
        {
        aMenuPane->SetItemDimmed( EMceCmdFetchNew, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdFetchAll, ETrue );
        }
    }

//
// ----------------------------------------------------------------------------
// CMceMessageListView::GetSetRead()
// ----------------------------------------------------------------------------
//
TInt CMceMessageListView::GetSetRead( const CMsvEntrySelection& aEntries )
    {
    TBool allRead = ETrue;
    TBool allUnread = ETrue;
    TMsvId service;
    TMsvEntry tEntry;

    for ( TInt cc=aEntries.Count(); --cc>=0; )
        {
        if ( iSession->GetEntry( 
                aEntries.At( cc ), service, tEntry ) == KErrNone  )
            {
            if ( tEntry.Unread() )
                {
                allRead = EFalse;
                }
            else
                {
                allUnread = EFalse;
                }
            }
        }

    if ( allRead && !allUnread )
        {
        return MessageFolderAllRead;
        }
    else if ( !allRead && allUnread )
        {
        // all unread
        return MessageFolderAllUnread;
        }
    else if ( !allRead && !allUnread )
        {
        // both read and unread
        return MessageFolderReadUnread;
        }
    else
        {
        /* never */
        return MessageFolderReadUnread;
        }
    
    }
    
// ----------------------------------------------------
// CMceMessageListView::FolderMenuL
// ----------------------------------------------------
void CMceMessageListView::FolderMenuL( CEikMenuPane* aMenuPane )
    {
    if(iFolderId == KMsvGlobalOutBoxIndexEntryIdValue )
        {
        if(iContextCommand)
            {
            aMenuPane->SetItemDimmed( EMceCmdOutboxSuspend, EFalse );
            aMenuPane->SetItemDimmed( EMceCmdOutboxStart, EFalse );
            aMenuPane->SetItemDimmed( EMceCmdOutboxMoveToDrafts, EFalse );
            aMenuPane->SetItemDimmed( EMceCmdDelete, EFalse );
            iContextCommand = EFalse;
            }
        else
            {
            aMenuPane->SetItemDimmed( EMceCmdOutboxSuspend, ETrue );
            aMenuPane->SetItemDimmed( EMceCmdOutboxStart, ETrue );
            aMenuPane->SetItemDimmed( EMceCmdOutboxMoveToDrafts, ETrue );
            aMenuPane->SetItemDimmed( EMceCmdDelete, ETrue );       
            }     
        }
    const TInt count = iMsgListContainer->CurrentItemSelectionCount();
    TBool remotemailboxOpened = ( iMsgListContainer->FolderEntry().iMtm == KSenduiMtmImap4Uid
                               || iMsgListContainer->FolderEntry().iMtm == KSenduiMtmPop3Uid );
    
    TBool thirdPartEmail = ( !remotemailboxOpened && 
                             iMceUi->CheckTechnologyTypeFromMtm( 
                             KMailTechnologyTypeUid, 
                             iMsgListContainer->FolderEntry().iMtm ) );

    //Forward is disabled in all folder menus, except is enabled in Sent Items seperately
    aMenuPane->SetItemDimmed( EMceCmdForward, ETrue );
    if ( remotemailboxOpened )
        {
        aMenuPane->SetItemDimmed( EMceCmdMailboxSettings, IsImapFolderOpenL() );
        if ( !IsImapFolderOpenL() )
            {
            aMenuPane->SetItemDimmed( EMceCmdMailboxSettings, iHideExitCommand );
            }
        aMenuPane->SetItemDimmed( EMceCmdMarkAsRead, iHideExitCommand );
        aMenuPane->SetItemDimmed( EMceCmdMarkAsUnread, iHideExitCommand );
        aMenuPane->SetItemDimmed( EMceCmdSort, iHideExitCommand );        
        aMenuPane->SetItemDimmed( EMceCmdReply, iHideExitCommand );
        TMsvEntry tempEntry;
        TMsvId serviceId;
        if ( iSession->GetEntry( iMsgListContainer->FolderEntry().iServiceId, serviceId, tempEntry )
            == KErrNone )
            {
            if ( tempEntry.Connected() )
                {
                aMenuPane->SetItemDimmed( EMceCmdConnect, ETrue );               
                aMenuPane->SetItemDimmed( EMceCmdCloseConnection, (iHideExitCommand && count > 0) );
                }
            else
                {
                aMenuPane->SetItemDimmed( EMceCmdCloseConnection, ETrue );
                aMenuPane->SetItemDimmed( EMceCmdConnect, (iHideExitCommand && count > 0) );
                }
            }
        // MMS.Content.Upload
        aMenuPane->SetItemDimmed( EMceCommandMMSContentUpload, ETrue );
        // No move and copy commands for mail messages
        aMenuPane->SetItemDimmed( EMceCmdMove, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdCopy, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdOutboxMoveToDrafts, ETrue );
        }
    else
        {
        aMenuPane->SetItemDimmed( EMceCmdCloseConnection, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdConnect, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdMailboxSettings, ETrue );
   	    aMenuPane->SetItemDimmed( EMceCmdMarkAsUnread, !iIsUnread );
        }

    if ( iFolderId != KMceDocumentsEntryId )
        {
        // only documents can have create, rename folder menu items
        aMenuPane->SetItemDimmed( EMceCmdNewFolder, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdRenameFolder, ETrue );
        }
    else
        {
        aMenuPane->SetItemDimmed( EMceCmdNewFolder, iHideExitCommand );
        aMenuPane->SetItemDimmed( EMceCmdRenameFolder, iHideExitCommand );
        }
    TInt totalCount = iMsgListContainer->Count();
    TInt subfolderCount = SubfolderCount();
    // TODO: I don't understand this if...
	if ( totalCount != subfolderCount && totalCount - subfolderCount == 1)
		{
		aMenuPane->SetItemDimmed( EMceCmdSort, ETrue ); 	
		}
    if ( iFolderId == KMsvGlobalOutBoxIndexEntryId )
        {
        aMenuPane->SetItemDimmed( EAknCmdMarkingModeEnter, ETrue );
        }
    else if ( count > 0 && totalCount == subfolderCount )
        {
        // only folders in the list, then "Edit" submenu would be empty so delete it
        aMenuPane->SetItemDimmed( EAknCmdMarkingModeEnter, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdSort, ETrue );  
        }

    aMenuPane->SetItemDimmed( EMceCmdExit, iHideExitCommand );
    aMenuPane->SetItemDimmed( EAknCmdOpen, iHideExitCommand );
    aMenuPane->SetItemDimmed( EMceCmdSort, iHideExitCommand );

	if ( IsSyncMl() )
		{
		if ( iMsgListContainer->FolderEntry().iType == KUidMsvServiceEntry )
			{
			// Prepare the menu for the SyncML main folder
			FolderMenuSyncMLMain( aMenuPane );
			}
		else
			{
			// Prepare the menu for the message lists
			FolderMenuSyncMLFolderL( aMenuPane );
			}
		}
	else
		{
		// Show the SyncML command only for the SyncML folder, no other folders
		aMenuPane->SetItemDimmed( EMceCmdAiwPlaceholder, ETrue );
		}
	
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        aMenuPane->SetItemDimmed( EAknCmdHelp, iHideExitCommand );
        }

    TBool hideNewMessageCommand = iHideExitCommand;
    if ( iHideExitCommand && iFolderId == KMsvDraftEntryId )
        {
        hideNewMessageCommand = count > 0;
        }
    if ( !IsSyncMl() )        
        {
        aMenuPane->SetItemDimmed( EMceCmdNewMessage, hideNewMessageCommand );
        }
    if ( count == 0 ) // No items in the list
        {
        // MMS.Content.Upload
        aMenuPane->SetItemDimmed( EMceCommandMMSContentUpload, ETrue );
        FolderMenuNoItemsL( aMenuPane );
        }
    else if ( count == 1 ) // 1 selected from the list
        {
        FolderMenuOneItemL( aMenuPane, remotemailboxOpened, thirdPartEmail );
        }
    else // More than 1 selected from the list
        {
        // MMS.Content.Upload
        aMenuPane->SetItemDimmed( EMceCommandMMSContentUpload, ETrue );

        CMsvEntrySelection* entries = ListContainer()->CurrentItemSelectionL();
        CleanupStack::PushL( entries );

        TInt setRead = GetSetRead( *entries );
        FolderMenuManyItemsL( aMenuPane, remotemailboxOpened, thirdPartEmail, setRead );

        if ( remotemailboxOpened || 
                SyncMlOutboxInbox( KMsvGlobalInBoxIndexEntryId ) || 
                SyncMlOutboxInbox( KMsvDraftEntryId ) || 
                SyncMlOutboxInbox( KMsvSentEntryId  ) )
            {
            switch ( GetDeleteFlagStatusL( entries ) )
                {
                case MessageFolderAllDelete:
                    aMenuPane->SetItemDimmed( EMceCmdUndelete, ETrue );
                    aMenuPane->SetItemDimmed( EMceCmdDelete, EFalse );
                    break;
                case MessageFolderAllUnDelete:
                    aMenuPane->SetItemDimmed( EMceCmdDelete, ETrue );
                    aMenuPane->SetItemDimmed( EMceCmdUndelete, EFalse );
                    break;
                case MessageFolderDeleteUnDelete:
                    aMenuPane->SetItemDimmed( EMceCmdUndelete, EFalse );
                    aMenuPane->SetItemDimmed( EMceCmdDelete, EFalse );
                    break;
                default:
                    break;
                }
            }

        // MMS notifications cannot be moved            
        CBaseMtmUiData* uiData = NULL;
        uiData = iMceUi->GetMtmUiDataL( KUidMsgMMSNotification );// NULL check
        TMsvId id;
        TMsvId service;
        TMsvEntry tEntry; 
        TInt mmsCount = 0;
        TInt mmsNotDeleteCount = 0;
            
        for ( TInt cc=entries->Count(); --cc>=0; )
            {
            id = entries->At(cc);
            if ( iSession->GetEntry( id, service, tEntry ) == KErrNone  )
                {
                if ( tEntry.iMtm == KUidMsgMMSNotification ) 
                    {
                    mmsCount++;
                    //Coverty fix,NULL RETURN, http://ousrv057/cov.cgi?cid=101801 
                    if ( uiData )
                        {
                        if ( uiData->OperationSupportedL( KMtmUiFunctionDeleteMessage, tEntry ) )
                            {
                            //MMS notification delete this entry not supported
                            mmsNotDeleteCount ++;
                            }
                        }
                     }
                }
            }
        if ( mmsCount == count )
            {
            //if only mms notifications, then move cannot be done
            aMenuPane->SetItemDimmed( EMceCmdMove, ETrue );
            if ( mmsCount == mmsNotDeleteCount )
                {
                aMenuPane->SetItemDimmed( EMceCmdDelete, ETrue );
                }                
            }

        CleanupStack::PopAndDestroy( entries );
        }

    if( iReadUnread )    
    {
    aMenuPane->SetItemDimmed( EMceCmdMarkAsRead, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdMarkAsUnread, ETrue );
    }
    
    iFetchMenuSelectionKeyPressed = iHideExitCommand;
    iHideExitCommand = EFalse;
    }

// ----------------------------------------------------
// CMceMessageListView::FolderMenuNoItemsL
// ----------------------------------------------------
void CMceMessageListView::FolderMenuNoItemsL( CEikMenuPane* aMenuPane ) const
    {
    aMenuPane->SetItemDimmed( EAknCmdOpen, ETrue );
    aMenuPane->SetItemDimmed( EAknCmdMarkingModeEnter, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdDelete, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdUndelete, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdMove, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdOutboxMoveToDrafts, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdCopy, ETrue );
    
    // "Write New Message" enabled for options and context sensitive menu
    // in case of empty list in Mail For Exchange mailbox
    if ( iMsgListContainer->FolderEntry().iMtm == KMceMtmMailForExchangeUid )
        {
        aMenuPane->SetItemDimmed( EMceCmdNewMessage, EFalse );
        }
    
    if ( !( iMsgListContainer->FolderEntry().iMtm == KSenduiMtmImap4Uid || 
            iMsgListContainer->FolderEntry().iMtm == KSenduiMtmPop3Uid ) )
        {
        aMenuPane->SetItemDimmed( EMceCmdFetch, ETrue );
        }
    else
        {
        TMsvEntry tempEntry;
        TMsvId serviceId;
        if ( iSession->GetEntry( iMsgListContainer->FolderEntry().iServiceId, serviceId, tempEntry ) == KErrNone )
            {
        	if ( tempEntry.Connected() )
        	    {
        		aMenuPane->SetItemDimmed( EMceCmdFetch, ETrue );               
        	    }
            }
        }
    aMenuPane->SetItemDimmed( EMceCmdOutboxSuspend, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdOutboxStart, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdSendVia, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdReply, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdMarkAsRead, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdMarkAsUnread, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdMarkAsReadMsg, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdMarkAsUnreadMsg, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdMarkAsReadMsgs, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdMarkAsUnreadMsgs, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdMarkAsReadEmails, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdMarkAsUnreadEmails, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdSort, ETrue );
    }

// ----------------------------------------------------
// CMceMessageListView::FolderMenuOneItemL
// ----------------------------------------------------
void CMceMessageListView::FolderMenuOneItemL( CEikMenuPane* aMenuPane, 
    TBool aRemoteMailboxOpened, 
    TBool aThirdPartEmail  )
    {
    if ( iMsgListContainer->CurrentItemType() != CMceMessageListContainerBase::EMessageListItemMsvItem )
        {
        aMenuPane->SetItemDimmed( EAknCmdOpen, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdMove, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdRenameFolder, ETrue );               
        aMenuPane->SetItemDimmed( EMceCmdCopy, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdOutboxMoveToDrafts, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdOutboxSuspend, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdOutboxStart, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdSendVia, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdReply, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdFetch, !aRemoteMailboxOpened );
        DimReadUnreadOptionsMenu( aMenuPane );

        if ( iMsgListContainer->CurrentItemType() == CMceMessageListContainerBase::EMessageListItemSubtitle )
            {
            CMsvEntrySelection* entriesInNode = iMsgListContainer->CurrentItemSelectionL();
            CleanupStack::PushL( entriesInNode );
            if ( entriesInNode->Count() > 0 )
                {
                switch ( GetDeleteFlagStatusL( entriesInNode ) )
                    {
                    case MessageFolderAllUnDelete:
                        aMenuPane->SetItemDimmed( EMceCmdDelete, ETrue );
                        aMenuPane->SetItemDimmed( EMceCmdUndelete, EFalse );
                        break;
                    case MessageFolderAllDelete:
                    case MessageFolderDeleteUnDelete:
                        aMenuPane->SetItemDimmed( EMceCmdUndelete, ETrue );
                        aMenuPane->SetItemDimmed( EMceCmdDelete, EFalse );
                        break;
                    default:
                        break;
                    }

                TInt setRead = GetSetRead( *entriesInNode );
                TMsvId currentFolderId = iFolderId;
            
                // Set Mark as Read/Unread Options menu 
                if ( ( currentFolderId == KMceDocumentsEntryId  ||
                       currentFolderId == KMsvGlobalInBoxIndexEntryId || 
                       iMsgListContainer->FolderEntry().Parent() == KMceDocumentsEntryId ) )
                    {
                    SetReadUnreadOptionsMenu( aMenuPane, !KIsEmail, setRead );
                    }
                else if( aRemoteMailboxOpened || aThirdPartEmail )
                    {
                    SetReadUnreadOptionsMenu( aMenuPane, KIsEmail, setRead );
                    }        
                }
            CleanupStack::PopAndDestroy( entriesInNode );
            }
        else
            {
            aMenuPane->SetItemDimmed( EMceCmdUndelete, !aRemoteMailboxOpened  );        
            }

        // Disable Mark as read/unread if focus on subtitle
        if ( iMsgListContainer->CurrentItemType() != CMceMessageListContainerBase::EMessageListItemSubtitle )
            {
            aMenuPane->SetItemDimmed( EMceCmdMarkAsRead, !aRemoteMailboxOpened  );
            aMenuPane->SetItemDimmed( EMceCmdMarkAsUnread, !aRemoteMailboxOpened  );
            }
        TMsvId currentFolderId = iFolderId;
        // Drafts does not have sort
        if ( currentFolderId == KMsvDraftEntryId )
            {
            aMenuPane->SetItemDimmed( EMceCmdSort, ETrue );
            }
        return;
        }
        
    // CurrentItemSelectionL() collects currently focused item if there is no marked one
    // For marked items, check iMsgListContainer->DisplayOptionsMenuFromSelectionKey()
    CMsvEntrySelection* selection = iMsgListContainer->CurrentItemSelectionL();
    CleanupStack::PushL( selection );
    TMsvEntry currentEntry;
    TMsvId serviceId;
    if ( iSession->GetEntry( selection->At(0), serviceId, currentEntry )
        == KErrNone || iMceUi->GetMsgDeletedStatus())
        {        
        DimReadUnreadOptionsMenu ( aMenuPane );
        if ( currentEntry.iType == KUidMsvFolderEntry )
            {
            aMenuPane->SetItemDimmed( EMceCmdMove, ETrue );
                
            if ( MceUtils::IsEntryFixed( currentEntry.Id() ) )
                {
                aMenuPane->SetItemDimmed( EMceCmdRenameFolder, ETrue );               
                }
            if ( aRemoteMailboxOpened )
                {
                aMenuPane->SetItemDimmed( EMceCmdFetch, ETrue );
                aMenuPane->SetItemDimmed( EMceCmdUndelete, ETrue );
                aMenuPane->SetItemDimmed( EMceCmdDelete, ETrue );
                aMenuPane->SetItemDimmed( EMceCmdSort, ETrue );
                aMenuPane->SetItemDimmed( EMceCmdCopy, ETrue );
                }
            // MMS.Content.Upload
            aMenuPane->SetItemDimmed( EMceCommandMMSContentUpload, ETrue );
            } // end folder
        else
            {
            // not folder so delete "Rename"
            aMenuPane->SetItemDimmed( EMceCmdRenameFolder, ETrue );

            // MMS.Content.Upload
            if ( currentEntry.iMtm.iUid != KSenduiMtmMmsUidValue )
                {
                aMenuPane->SetItemDimmed( EMceCommandMMSContentUpload, ETrue );
                }
            }

        if ( IsSyncMl() )
            {
            if ( iMceUi->CheckCommandAvailableL( EMceCmdUndelete, currentEntry ) )
                {
                aMenuPane->SetItemDimmed( EMceCmdUndelete, ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed( EMceCmdDelete, ETrue );
                }

            // Sync: remove New Message, Open, Reply and Sort from menu if item marked
            if ( iMsgListContainer->DisplayOptionsMenuFromSelectionKey() )
                {
                aMenuPane->SetItemDimmed( EMceCmdReply, ETrue );
                aMenuPane->SetItemDimmed( EAknCmdOpen, ETrue );
                aMenuPane->SetItemDimmed( EMceCmdSort, ETrue );                
                aMenuPane->SetItemDimmed( EMceCmdNewMessage, ETrue );
                }
            else
                {
                if ( SyncMlOutboxInbox( KMsvGlobalInBoxIndexEntryId ) )
                    {
                    aMenuPane->SetItemDimmed( EMceCmdReply, 
                         iMceUi->CheckCommandAvailableL( EMceCmdReply, currentEntry ) );
                    aMenuPane->SetItemDimmed( EMceCmdMarkAsRead, !currentEntry.Unread() );
                    aMenuPane->SetItemDimmed( EMceCmdMarkAsUnread, currentEntry.Unread() );
                    }                
                }               
            
            CleanupStack::PopAndDestroy( selection );
            return;
            }  

        TMsvId currentFolderId = iFolderId;
                               
        // Forward is enabled in sent item folder.
        if ( currentFolderId == KMsvSentEntryId )               
        	{
        	SetForwardMenuL( aMenuPane, currentEntry );	    	
        	}        
        	
        //reply is enabled in Inbox,Remote mailbox and in Document folder
        if ( currentFolderId == KMsvDraftEntryId ||
            currentFolderId == KMsvGlobalOutBoxIndexEntryId ||
            currentFolderId == KMsvSentEntryId ||
            currentFolderId == KMceTemplatesEntryId )
            {
            //reply is disabled in drafts, sent, outbox and templates folder
            aMenuPane->SetItemDimmed( EMceCmdReply, ETrue );
            
            if ( currentEntry.iMtm == KUidMsgMMSNotification )
                {
                aMenuPane->SetItemDimmed( EMceCmdOutboxMoveToDrafts, ETrue );
                }
            else if ( currentEntry.iMtm.iUid==KSenduiMtmImap4UidValue ||
				 	  currentEntry.iMtm.iUid==KSenduiMtmPop3UidValue ||
				 	  currentEntry.iMtm.iUid==KSenduiMtmSmtpUidValue )
            	{
            	// No copy or move commands for the mail messages
				aMenuPane->SetItemDimmed( EMceCmdMove, ETrue );
				aMenuPane->SetItemDimmed( EMceCmdCopy, ETrue );				
            	}
            }
        else
            {
            if ( iMsgListContainer->IsItemFolder( currentEntry.Id() ) 
                || iMceUi->IsMailSetToBeDeletedFromServerL( currentEntry.Id() )
                || iMsgListContainer->MarkedItemsCount() > 0 ) 
                { 
                // Reply is disabled for folder or 
                // when mailbox is not connected and the message is set to be deleted from the server
                aMenuPane->SetItemDimmed( EMceCmdReply, ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed( EMceCmdReply, 
                     iMceUi->CheckCommandAvailableL( EMceCmdReply, currentEntry ) );
                }
            }

        if ( currentFolderId != KMsvGlobalOutBoxIndexEntryId )
            {
            TBool selectionKeyPressed = iMsgListContainer->DisplayOptionsMenuFromSelectionKey();

            if ( iMceUi->IsMailSetToBeDeletedFromServerL( currentEntry.Id() ) )
                { 
                // Open, Fetch and Delete is disabled 
                // when mailbox is not connected and the message is set to be deleted from the server
                aMenuPane->SetItemDimmed( EAknCmdOpen, ETrue );
                aMenuPane->SetItemDimmed( EMceCmdFetch, ETrue );
                aMenuPane->SetItemDimmed( EMceCmdDelete, ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed( EAknCmdOpen, selectionKeyPressed );
                }

            if ( ( currentFolderId == KMsvGlobalInBoxIndexEntryId ) &&
                ( currentEntry.iMtm == KUidMsgMMSNotification ) )  
                {
                aMenuPane->SetItemDimmed( EMceCmdMove, ETrue );
                //check also open               
                CBaseMtmUiData* uiData = iMceUi->GetMtmUiDataL( KUidMsgMMSNotification );
                // Coverty change, NULL Return, http://ousrv057/cov.cgi?cid=101800
                if ( uiData )
                    {
                    aMenuPane->SetItemDimmed( EMceCmdDelete, 
                     uiData->OperationSupportedL( KMtmUiFunctionDeleteMessage, currentEntry ) );
                    }                 
                }

            aMenuPane->SetItemDimmed( EMceCmdOutboxSuspend, ETrue );
            aMenuPane->SetItemDimmed( EMceCmdOutboxStart, ETrue );
            aMenuPane->SetItemDimmed( EMceCmdOutboxMoveToDrafts, ETrue );
            if ( currentEntry.iMtm != KSenduiMtmIrUid &&
                 currentEntry.iMtm != KSenduiMtmBtUid )
                {
                aMenuPane->SetItemDimmed( EMceCmdSendVia, ETrue );
                }
            else
                {
                if ( selectionKeyPressed )
                    {
                    aMenuPane->SetItemDimmed( EMceCmdSendVia, ETrue );
                    }
                else
                    {
                    TRAPD( error, HandleSendViaMenuItemL( currentEntry.Id(), aMenuPane ) );
                    if ( error != KErrNone )
                        {
                        aMenuPane->SetItemDimmed( EMceCmdSendVia, ETrue );
                        }
                    }
                }
            }
        else
            {
            // Outbox, MMS.Content.Upload
            if ( currentEntry.iBioType == KUidMsgSubTypeMmsUpload.iUid )
                {
                aMenuPane->SetItemDimmed( EMceCommandMMSContentUpload, ETrue );
                }

            // outbox does not have open
            aMenuPane->SetItemDimmed( EAknCmdOpen, ETrue );
            TBool deleteStart = EFalse;
            TBool deleteSuspend = EFalse;
            switch ( currentEntry.SendingState() )
                {                
                case KMsvSendStateResend:                    
                    if ( currentEntry.iMtm == KUidMsgMMSNotification )
                        {                        
                        deleteStart = ETrue;
                        deleteSuspend = ETrue;
                        }
                    //go through
                case KMsvSendStateScheduled:                
                    break;

                case KMsvSendStateWaiting:
                    {
                    if ( currentEntry.iMtm == KSenduiMtmSmsUid ||
                        currentEntry.iMtm == KSenduiMtmMmsUid )
                        {
                        // disable 'Start' option to waiting SMS and 
                        // waiting MMS
                        aMenuPane->SetItemDimmed( EMceCmdOutboxStart, ETrue );
                        }

                    if ( currentEntry.iMtm == KUidMsgMMSNotification )
                        {   
                        aMenuPane->SetItemDimmed( EMceCmdOutboxSuspend, ETrue );
                        aMenuPane->SetItemDimmed( EMceCmdOutboxStart, ETrue );
                        }
                    break;
                    }

                case KMsvSendStateFailed:
                    if ( currentEntry.iMtm == KUidMsgMMSNotification )
                        {                         
                        aMenuPane->SetItemDimmed( EMceCmdOutboxStart, ETrue );
                        }
                    //go through
                case KMsvSendStateSuspended:
                    if ( currentEntry.iMtm == KUidMsgMMSNotification )
                        {                        
                        deleteStart = ETrue;                        
                        }
                    //go through
                case KMsvSendStateUponRequest:
                    deleteSuspend = ETrue;
                    break;

                case KMsvSendStateSending:
                    aMenuPane->SetItemDimmed( EMceCmdDelete, ETrue );
                    aMenuPane->SetItemDimmed( EMceCmdOutboxMoveToDrafts, ETrue );
                    // go through
                case KMsvSendStateSent:
                    deleteStart = ETrue;
                    break;
                case KMsvSendStateUnknown:
                case KMsvSendStateNotApplicable:
                default:
                    deleteSuspend = ETrue;
                    deleteStart = ETrue;
                    break;
                }

            if ( currentEntry.Connected() ||
                 deleteStart )
                {
                aMenuPane->SetItemDimmed( EMceCmdOutboxStart, ETrue );
                if ( currentEntry.iMtm == KSenduiMtmSmsUid ||
                     currentEntry.iMtm == KSenduiMtmMmsUid ||
                     currentEntry.iMtm == KSenduiMtmIrUid||
                     currentEntry.iMtm == KSenduiMtmBtUid||
                     currentEntry.iMtm == KUidMsgMMSNotification )
                    {
                    aMenuPane->SetItemDimmed( EMceCmdOutboxSuspend, ETrue );

                    //if message has no status, do not disable 'move to drafts'
                    if ( currentEntry.SendingState() != KMsvSendStateUnknown)
                        {		
                        aMenuPane->SetItemDimmed( EMceCmdOutboxMoveToDrafts, ETrue );
                        }
                    
                    if ( ( currentEntry.SendingState() != KMsvSendStateSent ) &&
                        ( ( currentEntry.iMtm == KSenduiMtmSmsUid ) ||
                        ( currentEntry.iMtm == KSenduiMtmMmsUid ) ) )
                        {
                        aMenuPane->SetItemDimmed( EMceCmdDelete, ETrue );
                        }
                    }
                }
            if ( deleteSuspend )
                {
                aMenuPane->SetItemDimmed( EMceCmdOutboxSuspend, ETrue );
                }
            aMenuPane->SetItemDimmed( EMceCmdSendVia, ETrue );
            if ( iMsgListContainer->Count() > 1 )
                {
                aMenuPane->SetItemDimmed( EMceCmdSort, iHideExitCommand  );
                }
            else
                {
                aMenuPane->SetItemDimmed( EMceCmdSort, ETrue );
                }                
            } // end outbox handling

        // Drafts and Outbox does not have move
        if ( currentFolderId == KMsvDraftEntryId ||
            currentFolderId == KMsvGlobalOutBoxIndexEntryId ||
            aRemoteMailboxOpened )
            {
            aMenuPane->SetItemDimmed( EMceCmdMove, ETrue );
            }
        // Drafts does not have sort
        if ( currentFolderId == KMsvDraftEntryId )
            {
            aMenuPane->SetItemDimmed( EMceCmdSort, ETrue );
            }
        if ( !aRemoteMailboxOpened )
            {
            if ( iMsgListContainer->FolderEntry().iServiceId == KMsvLocalServiceIndexEntryId )
                {
                aMenuPane->SetItemDimmed( EMceCmdCopy, ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed( EMceCmdMove, ETrue );
                }
            aMenuPane->SetItemDimmed( EMceCmdFetch, ETrue );
            aMenuPane->SetItemDimmed( EMceCmdUndelete, ETrue );
            
            // enable mark as read/unread in Inbox and Document folder
            EnableMarkAllReadMsgInInboxAndDocument( currentEntry, currentFolderId, aMenuPane );
            }
        if ( aRemoteMailboxOpened || aThirdPartEmail )             
            {
            if ( iMceUi->CheckCommandAvailableL( EMceCmdUndelete, currentEntry ) )
                {
                aMenuPane->SetItemDimmed( EMceCmdUndelete, ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed( EMceCmdDelete, ETrue );
                }
            if ( aRemoteMailboxOpened || aThirdPartEmail )
                {
                if ( currentEntry.iType != KUidMsvFolderEntry &&
                     !iMceUi->IsMailSetToBeDeletedFromServerL( currentEntry.Id() ) )
                    { // MarkAsUnread and MarkAsRead is disabled when the message is set to be deleted from the server
                    aMenuPane->SetItemDimmed( EMceCmdMarkAsRead, !currentEntry.Unread() );
                    aMenuPane->SetItemDimmed( EMceCmdMarkAsUnread, currentEntry.Unread() );
                    }
                }
            }

        if ( currentEntry.iType == KUidMsvFolderEntry )
            {
            if ( MceUtils::IsEntryFixed( currentEntry.Id() ) )
                {
                aMenuPane->SetItemDimmed( EMceCmdDelete, ETrue );
                }
            } // end folder
        iMceUi->SetMsgDeletedStatus(EFalse);
        } // end if ( iSession->GetEntry == KErrNone )
    CleanupStack::PopAndDestroy( selection ); // selection
    }

// ----------------------------------------------------
// CMceMessageListView::FolderMenuManyItemsL
// ----------------------------------------------------
void CMceMessageListView::FolderMenuManyItemsL(
    CEikMenuPane* aMenuPane,
    TBool aRemoteMailboxOpened,
    TBool aThirdPartEmail,
    TBool aSetRead )
    {
    aMenuPane->SetItemDimmed( EMceCmdRenameFolder, ETrue );
    aMenuPane->SetItemDimmed( EAknCmdOpen, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdOutboxSuspend, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdOutboxStart, ETrue );    
    aMenuPane->SetItemDimmed( EMceCmdReply, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdSendVia, ETrue );
    DimReadUnreadOptionsMenu( aMenuPane );

    if ( IsSyncMl() )
        {
        if( SyncMlOutboxInbox ( KMsvGlobalInBoxIndexEntryId ) )
            {
            SetReadUnreadOptionsMenu( aMenuPane, KIsEmail, aSetRead );
            }       

        aMenuPane->SetItemDimmed( EMceCmdNewMessage, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdSort, ETrue );
        if ( iHideExitCommand )
            {
            aMenuPane->SetItemDimmed( EMceCmdMarkAsReadEmails, iHideExitCommand ); 
            aMenuPane->SetItemDimmed( EMceCmdMarkAsUnreadEmails, iHideExitCommand );
            }
    	// SyncML Sent
        if ( SyncMlOutboxInbox( KMsvSentEntryId ) || SyncMlOutboxInbox( KMsvDraftEntryId ) )
            {
            aMenuPane->SetItemDimmed( EMceCmdNewMessage, ETrue );
            aMenuPane->SetItemDimmed( EMceCmdSort, ETrue );    
            aMenuPane->SetItemDimmed( EMceCmdUndelete, ETrue );	    
            }
        return;
        } 

    TMsvId serviceId;
    CMsvEntrySelection* selection = iMsgListContainer->CurrentItemSelectionL();
    CleanupStack::PushL( selection );
    TMsvEntry currentEntry;

    TMsvId currentFolderId = iFolderId;

    if ( currentFolderId != KMsvGlobalOutBoxIndexEntryId )
        {
        aMenuPane->SetItemDimmed( EMceCmdOutboxMoveToDrafts, ETrue );
        }
    
    if ( currentFolderId == KMsvGlobalOutBoxIndexEntryId )
        {
        aMenuPane->SetItemDimmed( EMceCmdMove, ETrue );
        }
    if ( currentFolderId == KMsvSentEntryId )
        {
        TInt selectionCount = selection->Count();
        TBool mailMessage = EFalse;
        TBool otherMessage = EFalse;
        
        aMenuPane->SetItemDimmed( EMceCmdSort, EFalse );

        // Check type of message
        for ( TInt loop = 0; loop < selectionCount; loop++ )
            {
            if ( iSession->GetEntry( selection->At(loop), serviceId, currentEntry ) 
                == KErrNone || iMceUi->GetMsgDeletedStatus())
                {
                if ( currentEntry.iMtm.iUid==KSenduiMtmImap4UidValue ||
		             currentEntry.iMtm.iUid==KSenduiMtmPop3UidValue ||
                     currentEntry.iMtm.iUid==KSenduiMtmSmtpUidValue )
                    {    
                    mailMessage = ETrue;
                    }
                else
                    {
                    otherMessage = ETrue;
                    }
                iMceUi->SetMsgDeletedStatus(EFalse);
                }
            }
        if ( otherMessage && mailMessage ) 
            {
            aMenuPane->SetItemDimmed( EMceCmdMove, EFalse );
            }
        else if ( mailMessage && !otherMessage ) // Only mailmessages
            {
            aMenuPane->SetItemDimmed( EMceCmdMove, ETrue );
            }            
        else // Only other messages
            {
            aMenuPane->SetItemDimmed( EMceCmdMove, EFalse );
            }            
        }
    // Drafts does not have move and sort
    if ( currentFolderId == KMsvDraftEntryId)
        {
        aMenuPane->SetItemDimmed( EMceCmdMove, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdSort, ETrue );
        }

    if (aRemoteMailboxOpened )
        {  
        // Incase of mailbox, sort should be enabled for all defined folder inside (e.g. inbox. outbox, draft, sent etc) 
         aMenuPane->SetItemDimmed( EMceCmdSort, iHideExitCommand );  
        }

    if ( !aRemoteMailboxOpened )
        {
        if ( iMsgListContainer->FolderEntry().iServiceId == KMsvLocalServiceIndexEntryId )
            {
            aMenuPane->SetItemDimmed( EMceCmdCopy, ETrue );
            }
        else
            {
            aMenuPane->SetItemDimmed( EMceCmdMove, ETrue );
            }
        aMenuPane->SetItemDimmed( EMceCmdFetch, ETrue );
        aMenuPane->SetItemDimmed( EMceCmdUndelete, ETrue );
        }

    if ( iMsgListContainer->CurrentItemType() !=
             CMceMessageListContainerBase::EMessageListItemSubtitle
             || selection->Count()>0 )
        {
        // Set Mark as Read/Unread Options menu 
        if ( ( currentFolderId == KMceDocumentsEntryId  ||
               currentFolderId == KMsvGlobalInBoxIndexEntryId || 
               iMsgListContainer->FolderEntry().Parent() == KMceDocumentsEntryId ) )
            {
            SetReadUnreadOptionsMenu( aMenuPane, !KIsEmail, aSetRead );
            }
        else if( aRemoteMailboxOpened || aThirdPartEmail )
            {
            SetReadUnreadOptionsMenu( aMenuPane, KIsEmail, aSetRead );
            }        
        }

    CleanupStack::PopAndDestroy( selection );
    }


// ----------------------------------------------------
// CMceMessageListView::FolderMenuSyncMLMain
// ----------------------------------------------------
void CMceMessageListView::FolderMenuSyncMLMain( CEikMenuPane *aMenuPane )
	{
	aMenuPane->SetItemDimmed( EMceCmdSendVia, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdConnect, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdCloseConnection, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdReply, ETrue );
	aMenuPane->SetItemDimmed( EMceCommandMMSContentUpload, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdOutboxSuspend, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdFetch, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdDelete, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdUndelete, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdMarkAsRead, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdMarkAsUnread, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdSort, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdMove, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdOutboxStart, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdOutboxMoveToDrafts, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdNewFolder, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdRenameFolder, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdCopy, ETrue );
	aMenuPane->SetItemDimmed( EAknCmdMarkingModeEnter, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdMailboxSettings, ETrue );
	// + MessageDetails
	}

// ----------------------------------------------------
// CMceMessageListView::FolderMenuSyncMLFolderL
// ----------------------------------------------------
void CMceMessageListView::FolderMenuSyncMLFolderL( CEikMenuPane *aMenuPane )
	{
	CMsvEntrySelection* selection = iMsgListContainer->CurrentItemSelectionL();
    CleanupStack::PushL( selection );
    TMsvEntry currentEntry;
    TMsvId serviceId;
	
	// All SyncML folders: Inbox, Sent items, Drafts, Outbox
	aMenuPane->SetItemDimmed( EMceCmdAiwPlaceholder, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdSendVia, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdConnect, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdCloseConnection, ETrue );
	aMenuPane->SetItemDimmed( EMceCommandMMSContentUpload, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdOutboxSuspend, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdMove, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdOutboxStart, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdNewFolder, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdRenameFolder, ETrue );
	aMenuPane->SetItemDimmed( EMceCmdMailboxSettings, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdFetch, ETrue ); 
    aMenuPane->SetItemDimmed( EMceCmdCopy, ETrue ); 
    aMenuPane->SetItemDimmed( EMceCmdMarkAsRead, ETrue ); 
	aMenuPane->SetItemDimmed( EMceCmdMarkAsUnread, ETrue ); 

    TInt numberOfEntries = iMsgListContainer->Count();

	// All except SyncML Outbox
	if ( SyncMlOutboxInbox( KMsvGlobalInBoxIndexEntryId  ) )
		{
		aMenuPane->SetItemDimmed( EMceCmdOutboxMoveToDrafts, ETrue );
        if ( numberOfEntries > 0 && 
            iSession->GetEntry( selection->At(0), serviceId, currentEntry ) == KErrNone &&
            selection->Count() == 1 )
            {
            aMenuPane->SetItemDimmed( EMceCmdMarkAsRead, !currentEntry.Unread() );
            aMenuPane->SetItemDimmed( EMceCmdMarkAsUnread, currentEntry.Unread() );
            }
		}

	// SyncML Drafts
	if ( SyncMlOutboxInbox( KMsvDraftEntryId ) )
		{
		aMenuPane->SetItemDimmed( EMceCmdReply, ETrue );
		aMenuPane->SetItemDimmed( EMceCmdOutboxMoveToDrafts, ETrue );
		}
		
	// SyncML Sent
	if ( SyncMlOutboxInbox( KMsvSentEntryId ) )
		{
		aMenuPane->SetItemDimmed( EMceCmdOutboxMoveToDrafts, ETrue );
		aMenuPane->SetItemDimmed( EMceCmdReply, ETrue );
		}

	// SyncML Outbox
	if ( SyncMlOutboxInbox( KMsvGlobalOutBoxIndexEntryIdValue ) )
		{		
		aMenuPane->SetItemDimmed( EAknCmdMarkingModeEnter, ETrue );
		aMenuPane->SetItemDimmed( EAknCmdOpen, ETrue );
		aMenuPane->SetItemDimmed( EMceCmdFetch, ETrue );
		aMenuPane->SetItemDimmed( EMceCmdMarkAsRead, ETrue );
		aMenuPane->SetItemDimmed( EMceCmdMarkAsUnread, ETrue );
		aMenuPane->SetItemDimmed( EMceCmdSort, ETrue );
		aMenuPane->SetItemDimmed( EMceCmdCopy, ETrue );
		aMenuPane->SetItemDimmed( EMceCmdReply, ETrue ); 
		aMenuPane->SetItemDimmed( EMceCmdNewMessage, iHideExitCommand );
		}
    if ( numberOfEntries == 0 )
        {
        aMenuPane->SetItemDimmed( EMceCmdNewMessage, EFalse );
		}
    CleanupStack::PopAndDestroy( selection ); 
	}

// ----------------------------------------------------
// CMceMessageListView::EditMenuL
// ----------------------------------------------------
void CMceMessageListView::EditMenuL( CEikMenuPane* aMenuPane ) const
    {
    // This function is not called if empty folder/list is yet to be populated
    // because in that case EditMenu is hidden
    aMenuPane->SetItemDimmed( EAknCmdMarkReadMsgs, ETrue );

    TInt count = iMsgListContainer->SelectionCount();

    if ( count == 0 )
        {
        //if no selection, disable unmarkall
        aMenuPane->SetItemDimmed( EAknUnmarkAll, ETrue );
        TBool remoteMailboxOpened = ( iMsgListContainer->FolderEntry().iMtm == KSenduiMtmImap4Uid || 
                                      iMsgListContainer->FolderEntry().iMtm == KSenduiMtmPop3Uid );
        if ( remoteMailboxOpened || // Mailbox
            IsSyncMl() ||   //SyncMailbox
            iMsgListContainer->FolderEntry().Id() == KMsvGlobalInBoxIndexEntryId || //Inbox
            iMsgListContainer->FolderEntry().Id() == KMceDocumentsEntryId || //Document
            iMsgListContainer->FolderEntry().Parent() == KMceDocumentsEntryId ) //Subfolder of Document
            {
            aMenuPane->SetItemDimmed( EAknCmdMarkReadMsgs, EFalse );
            }
       }   

    if ( ( count + SubfolderCount() ) == iMsgListContainer->Count())
       {
       //if all selected, disable markall
       aMenuPane->SetItemDimmed( EAknMarkAll, ETrue );
       } 

    if ( iMsgListContainer->IsItemFolder( iMsgListContainer->CurrentItemId() ) )
        {
        aMenuPane->SetItemDimmed( EAknCmdMark, ETrue );
        aMenuPane->SetItemDimmed( EAknCmdUnmark, ETrue );
        }
    else
        {
        if (iMsgListContainer->IsItemSelected( iMsgListContainer->CurrentItemId() ) )
            {
            aMenuPane->SetItemDimmed( EAknCmdMark, ETrue );
            }
        else
            {
            aMenuPane->SetItemDimmed( EAknCmdUnmark, ETrue );
            }
        } // end else

    // Mark All and Unmark All are always visible
    }

// ----------------------------------------------------
// CMceMessageListView::FolderOpenedL
// ----------------------------------------------------
TBool CMceMessageListView::FolderOpenedL()
    {
    // return EFalse if cd.. returns to main view, ETrue if returns to Parent in folder view.

    TBool folderOpened = ETrue;

    const TMsvEntry& entry = iMsgListContainer->FolderEntry();
    const TMsvId entryId = entry.Id();

    if ( entry.Parent() == KMsvRootIndexEntryId ||
        ( entry.iMtm == KSenduiMtmImap4Uid && !IsImapFolderOpenL( ) ) ||
         entryId == KMsvGlobalInBoxIndexEntryId ||
         entryId == KMsvGlobalOutBoxIndexEntryId ||
         entryId == KMsvDraftEntryId ||
         entryId == KMsvSentEntryId ||
         entryId == KMceDocumentsEntryId ) 
        {
        folderOpened = EFalse;
        }
    
    return folderOpened;
    }

// ----------------------------------------------------
// CMceMessageListView::SubfolderCount
// ----------------------------------------------------
TInt CMceMessageListView::SubfolderCount() const
    {
    return iMsgListContainer->FolderCount();
    }


// ----------------------------------------------------
// CMceMessageListView::CancelMailboxTimer
// ----------------------------------------------------
void CMceMessageListView::CancelMailboxTimer() const
    {
    iMceUi->CancelMailboxTimer();
    }

// ----------------------------------------------------
// CMceMessageListView::IsFolderNameUsedL
// ----------------------------------------------------
TBool CMceMessageListView::IsFolderNameUsedL(const TDesC& aName, TMsvId aId /* = KErrNotFound */) const
//
// Return ETrue if a service of name 'aName' already exists.
//
    {
    CMsvEntry* entry = CMsvEntry::NewL(
        *iSession,
        iMsgListContainer->CurrentFolderId(),
        TMsvSelectionOrdering(
            KMsvGroupByStandardFolders, EMsvSortByDateReverse) );
    CleanupStack::PushL( entry );
    TBool ret = EFalse;
    const TInt total = entry->Count();
    for(TInt cc = 0; cc < total; ++cc)
        {
        const TMsvEntry& tEntry = (*entry)[cc];
        if( tEntry.iType == KUidMsvFolderEntry &&
            tEntry.Id() != aId )
            {
            // This entry is a service, so ensure the passed name isn't used
            if(!aName.CompareF(tEntry.iDetails))
                {
                ret = ETrue;
                break;
                }
            }
        }
    CleanupStack::PopAndDestroy( entry );
    return ret;
    }

// ----------------------------------------------------
// CMceMessageListView::FindUnreadMessageAndOpenItL
// ----------------------------------------------------
TBool CMceMessageListView::FindUnreadMessageAndOpenItL( const TMsvId aId )
    {
    TInt launchViewerStartTime = KMceLaunchViewerStartTime;
    if(iMceUi->IsEditorOpen())
        {
        launchViewerStartTime = KMceLaunchViewerStartTimeWhenEditorOpen;
        }
    
// TODO: how to simplify this ?
    TMsvId firstUnreadMessageId = KErrNotFound;
    TBool oneUnreadMessage = EFalse;
    if ( aId )
        {
        //
        oneUnreadMessage = ETrue;
        firstUnreadMessageId = aId;
        }
    else
        {
        oneUnreadMessage = iMsgListContainer->FindFirstUnreadMessageL( firstUnreadMessageId );
        }

    if ( firstUnreadMessageId > KErrNotFound )
        {
        iMsgListContainer->SetCurrentItemIdL(
            firstUnreadMessageId );
        }
        
    if ( !iRunningEditFunction )
        {
        iMceUi->CloseEditorApp();
        }

    TMsvEntry firstEntry;
    TMsvId serviceId;
    if ( oneUnreadMessage && firstUnreadMessageId > KErrNotFound && 
        iSession->GetEntry( firstUnreadMessageId, serviceId, firstEntry ) == KErrNone )
        {
        TUid messageMtm = firstEntry.iMtm;
        
    	if ( (messageMtm!=KSenduiMtmIrUidValue) &&
    	     (messageMtm!=KSenduiMtmBtUidValue) &&
    	     (messageMtm!=KSenduiMtmSmtpUidValue) &&
    	     (messageMtm!=KSenduiMtmImap4UidValue) &&
    	     (messageMtm!=KSenduiMtmPop3UidValue)
    	   )
    	    {

            if ( !iMessageViewerLauncher )
                {
                iMessageViewerLauncher = CPeriodic::NewL( CActive::EPriorityIdle );
                }

            if ( !(iMessageViewerLauncher->IsActive()) )
                {
                delete iLocalScreenClearer;
                iLocalScreenClearer = NULL;
                iLocalScreenClearer = CAknLocalScreenClearer::NewL( EFalse );
                MCELOGGER_WRITE("FindUnreadMessageAndOpenItL: start viewer launcher");
                TTimeIntervalMicroSeconds32 startTimeInterval = launchViewerStartTime;
                const TTimeIntervalMicroSeconds32 retryTimeInterval = KMceLaunchViewerRetryTime;
                iLaunchViewerCounter = 0;
                iMessageViewerLauncher->Start( 
                    startTimeInterval, 
                    retryTimeInterval, 
                    TCallBack(LaunchViewer, this ) );
                }
    	    }
        return ETrue;
        }
    return EFalse;
    }

// ----------------------------------------------------
// CMceMessageListView::FindInboxAndOpenItL
// ----------------------------------------------------
void CMceMessageListView::FindInboxAndOpenItL()
    {    
    //imapfolders
    //first count subscribed folders. If only one ( inbox ) open it
    //otherwise show folderlist.
    TMsvId inboxId = KErrNotFound;
    TMsvEntry child;
    TInt cnt = 0;
    CMsvEntry* entry = CMsvEntry::NewL(
        *iSession,
        iMsgListContainer->CurrentFolderId(),
        TMsvSelectionOrdering(
            KMsvGroupByStandardFolders, EMsvSortByDateReverse) );
    CleanupStack::PushL( entry );
    
    const TInt count = entry->Count();
    
    for ( TInt loop = 0; loop < count; loop++ )
        {
        child = (*entry)[loop];
        if( child.iType == KUidMsvFolderEntry )
            {
            cnt++;
            inboxId = child.Id();
            }
        }

    //if only one subscribed, it must be inbox, but check it also to be sure
    if( cnt == KMceOnlyInboxInImap && child.iDetails.CompareF( iInboxName->Des() ) == 0 )
        {
        SetFolderL( inboxId );
        }    
    CleanupStack::PopAndDestroy( entry );
    }

// ----------------------------------------------------
// CMceMessageListView::ChangeFolderAndRefreshListboxL
// ----------------------------------------------------
void CMceMessageListView::ChangeFolderAndRefreshListboxL( TMsvId aNewFolderId )
    {
    MCELOGGER_ENTERFN("CMceMessageListView::ChangeFolderAndRefreshListboxL");
    
     TInt oldCount = iMsgListContainer->Count(); // how many items in current folder
    TMsvId oldId = iMsgListContainer->FolderEntry().Id();
    TMsvEntry currentEntry;
    TMsvId serviceId; // not used here but needed by GetEntry function
    if ( iSession->GetEntry( aNewFolderId, serviceId, currentEntry )
        == KErrNone )
        {
        iMsgListContainer->ClearSelection();
        
        if ((oldId == KMsvGlobalOutBoxIndexEntryId) || (aNewFolderId == KMsvGlobalOutBoxIndexEntryId))
            {
            iFolderId = aNewFolderId;    
            iMsgListContainer->MakeVisible(EFalse);
            AppUi()->RemoveFromStack(iMsgListContainer);
            delete iMsgListContainer;
            iMsgListContainer = NULL;
            CreateListboxL();
            iMsgListContainer->SetRect(ClientRect());
            AppUi()->AddToStackL( *this,iMsgListContainer );
            iMsgListContainer->ActivateL();
            iMsgListContainer->MakeVisible(ETrue);
            iMsgListContainer->SetFolderL( iFolderId );
            }
        else
            {
            SetFolderL( aNewFolderId );
            }
        
        TUid mtm = iMsgListContainer->FolderEntry().iMtm;
        if ( mtm == KSenduiMtmImap4Uid )
            {
            FindInboxAndOpenItL();
            }

        // NCN reset, if the mail folder contains unread messages
    	if ( iMceUi->CheckTechnologyTypeFromMtm( KMailTechnologyTypeUid, mtm ) &&
    	     HasUnreadMessagesL() )
            {
           
            iMceUi->HandleNotif(iMsgListContainer->FolderEntry().iServiceId);
            }

        if ( ( mtm == KSenduiMtmImap4Uid || 
            mtm == KSenduiMtmPop3Uid ) && !iConnectionNote )
            {
            iMceUi->ActivateConnectionQueryTimerL( iFolderId );
            }
        
        SetEmptyTextL();

        if ( IsImapFolderOpenL( ) )        
            {
            //show folder tabs
            TInt depth = 1;            
            const TMsvEntry child = iMsgListContainer->FolderEntry();           
            TMsvId id = child.Parent(); 
            TMsvId service = KMsvLocalServiceIndexEntryId;
            TMsvEntry entry;
            TInt err = iSession->GetEntry( id, service, entry );
               
            while ( err == KErrNone && entry.iType == KUidMsvFolderEntry)
                {
                err = iSession->GetEntry( entry.Parent(), service, entry );
                depth++;
                }
            iMceUi->ShowFolderTabsL( depth );
            }
        }

    const TMsvEntry entry = iMsgListContainer->FolderEntry();
    // Fetch pointer to the default title pane control
    CAknTitlePane* title=iMceUi->TitlePaneL();
    TMsvEntry serviceEntry;
    if ( ( entry.iMtm == KSenduiMtmImap4Uid || IsSyncMl() ) &&
         iSession->GetEntry( entry.iServiceId, serviceId, serviceEntry)
         == KErrNone )
        {
        title->SetTextL( serviceEntry.iDetails );
        }
    else
        {
        title->SetTextL( entry.iDetails );
        }

    TInt sorting = EMceCmdSortByDate;
    TBool ordering = EFalse;
    TMsvId id = iFolderId;       
    for( TInt loop = 0; loop < iFolderItemArray->Count() ; loop++ )
        {
        TMceFolderItem item = ((*iFolderItemArray)[loop]);
        if ( item.iFolderId == id )
            {        
            sorting = item.iSortOrder;
            ordering = item.iOrdering;
            }
        }

    // MSK
    SetMSKButtonL();

    iSortOrder = sorting;
    iOrdering = ordering;

    // Unset flag so one row list container knows that first subtitle is to be opened
    iMsgListContainer->SetContainerFlag( EMceOneRowFlagFirstSubtitleOpen, EFalse );

    iMsgListContainer->SetSortTypeL( iSortOrder, iOrdering );

    if ( !iMsgListContainer->SetCurrentItemIdL( oldId ) )
        {
        iMsgListContainer->ResetCurrentItemL( );
        }

    iMsgListContainer->RefreshSelectionIndexesL( EFalse );
    iMsgListContainer->RefreshListbox();
	                
    MCELOGGER_LEAVEFN("CMceMessageListView::ChangeFolderAndRefreshListboxL");
    }

// ----------------------------------------------------
// CMceMessageListView::SetHideExitCommand
// ----------------------------------------------------
void CMceMessageListView::SetHideExitCommand( TBool aHideExitCommand )
    {
    iHideExitCommand = aHideExitCommand;
    }

// ----------------------------------------------------
// CMceMessageListView::SetEmptyTextL
// now we need to set correct empty string because in remote mailbox it is
// different than in other folders
// ----------------------------------------------------
void CMceMessageListView::SetEmptyTextL() const
    {
    TBool folderIsRemote = iMsgListContainer->IsItemRemote( iMsgListContainer->CurrentFolderId() );
    if ( folderIsRemote && !IsSyncMl() )
        {
        iMsgListContainer->SetListEmptyTextL( *iRemoteMailboxEmptyText );
        }   
    else
        {       
        iMsgListContainer->SetListEmptyTextL( *iEmptyText);
        }
    }

// ----------------------------------------------------
// CMceMessageListView::LaunchViewer
// ----------------------------------------------------
TInt CMceMessageListView::LaunchViewer( TAny* aSelf )
    {
    return REINTERPRET_CAST(CMceMessageListView*,aSelf)->LaunchMceViewer();
    }

// ----------------------------------------------------
// CMceMessageListView::LaunchMceViewer
// ----------------------------------------------------
TInt CMceMessageListView::LaunchMceViewer()
    {
    if ( iMceUi->IsEditorOpen() )
        // something is embedded in mce, wait for that to be closed
        {
        if ( iRunningEditFunction )
            {
            return KErrNone;
            }
        iMceUi->CloseEditorApp();
        if ( iLaunchViewerCounter++ < KMceLaunchViewerRetryCounter )
            {
            MCELOGGER_WRITE_FORMAT("LaunchMceViewer: wait editor to close...iLaunchViewerCounter = %d", iLaunchViewerCounter);
            return KErrNotReady;
            }
        else
            {
            MCELOGGER_WRITE("LaunchMceViewer: editor did not close quickly enough, give up waiting...");
            delete iLocalScreenClearer;
            iLocalScreenClearer = NULL;
            delete iMessageViewerLauncher;
            iMessageViewerLauncher = NULL;
            return KErrNone;
            }
        }

    if ( iMceUi->IsEditorOpen() )
        {
        MCELOGGER_WRITE("LaunchMceViewer: do not open because already editing one");
        delete iLocalScreenClearer;
        iLocalScreenClearer = NULL;
        delete iMessageViewerLauncher;
        iMessageViewerLauncher = NULL;
        return KErrNone;
        }
        
    TMsvId firstUnreadMessageId = KErrNotFound;
    TBool oneUnreadMessage = EFalse;
    TRAPD( error, oneUnreadMessage = iMsgListContainer->FindFirstUnreadMessageL( firstUnreadMessageId ));
    if ( error )
        {
        oneUnreadMessage = EFalse;
        }
    TMsvEntry firstEntry;
    TMsvId serviceId;

    if ( oneUnreadMessage && firstUnreadMessageId > KErrNotFound && 
        iSession->GetEntry( firstUnreadMessageId, serviceId, firstEntry ) == KErrNone )
        {
        TUid messageMtm = firstEntry.iMtm;
        
    	if ( (messageMtm!=KSenduiMtmIrUidValue) &&
    	     (messageMtm!=KSenduiMtmBtUidValue) &&
    	     (messageMtm!=KSenduiMtmSmtpUidValue) &&
    	     (messageMtm!=KSenduiMtmImap4UidValue) &&
    	     (messageMtm!=KSenduiMtmPop3UidValue)
    	   )
    	    {
            MCELOGGER_WRITE("LaunchMceViewer: launch editor");
            iRunningEditFunction = ETrue;
            TRAP_IGNORE( iMceUi->EditMTMEntryL( firstEntry ) );
            iRunningEditFunction = EFalse;
    	    }
        }
    delete iLocalScreenClearer;
    iLocalScreenClearer = NULL;
    delete iMessageViewerLauncher;
    iMessageViewerLauncher = NULL;
    MCELOGGER_WRITE("LaunchMceViewer: done");
    return KErrNone; 
    }

// ----------------------------------------------------
// CMceMessageListView::HandleSendViaMenuItemL
//
// ----------------------------------------------------
void CMceMessageListView::HandleSendViaMenuItemL( TMsvId aEntryId, CEikMenuPane* aMenuPane ) const
    {
    TBool dimSendVia = ETrue;
    TFileName fileName;
    RApaLsSession apaLs;
    TBool         isProgram = EFalse;
    TUid          appUid(KNullUid);

    MCELOGGER_WRITE_FORMAT("CMceMessageListView::HandleSendViaMenuItemL: aEntryId: 0x%x", aEntryId );
	TInt fileSize;
    RFile fileHandle;
	MceUtils::GetIrFilePathL( *iSession, aEntryId, fileName, fileHandle, fileSize );
	CleanupClosePushL( fileHandle );

    TBuf<KMaxDataTypeLength> mimeType;
    mimeType.Zero();
    if (apaLs.Connect() == KErrNone)
        {
        TDataType datatype;
        apaLs.SetAcceptedConfidence( CApaDataRecognizerType::ECertain );
        apaLs.AppForDocument( fileName, appUid, datatype );
        apaLs.IsProgram( fileName, isProgram );
        mimeType = datatype.Des();
        apaLs.Close();
        }

#ifdef _DEBUG
    // make sure buffer does not get too long for logging.
    TBuf<KMcePrintableTextLength> debugMimeType = mimeType.Left( KMcePrintableTextLength );
    MCELOGGER_WRITE_FORMAT("mime type: \"%s\"", debugMimeType.PtrZ());
#endif
    if (appUid.iUid == 0 || isProgram)
        {
        MCELOGGER_WRITE( "application not found" );
        dimSendVia = EFalse;
        }
#ifdef _DEBUG
    else
        {
        MCELOGGER_WRITE_FORMAT("application found: uid 0x%x", appUid.iUid );
        MCELOGGER_WRITE_FORMAT("is program: %d", isProgram );
        }
#endif

    CCommonContentPolicy* contentPolicy = CCommonContentPolicy::NewLC();
    TBool banned = ( contentPolicy->IsClosedFileL( fileHandle ) );
    if ( banned )
        {
        dimSendVia = banned;
        }	
    CleanupStack::PopAndDestroy(2); // contentPolicy, fileHandle

    MCELOGGER_WRITE_FORMAT("dim EMceCmdSendVia menu: %d", dimSendVia );
    aMenuPane->SetItemDimmed( EMceCmdSendVia, dimSendVia );
    }

// ----------------------------------------------------
// CMceMessageListView::MessageViewHandleEntriesCreatedOrChangedL
//
// ----------------------------------------------------
void CMceMessageListView::MessageViewHandleEntriesCreatedOrChangedL(
    TMsvId aParentId,
    CMsvEntrySelection* aSelection )
    {
    if ( !iMsgListContainer )
        {
        return;
        }
    TMsvEntry entry = iMsgListContainer->FolderEntry();
    MCELOGGER_WRITE_FORMAT("parentId 0x%x", aParentId );
    MCELOGGER_WRITE_FORMAT("currentId 0x%x", entry.Id() );
    if ( aParentId != entry.Id() )
        {
        MCELOGGER_WRITE("Not happening in our folder, just skip");
        return;
        }
    if ( entry.Parent() == KMsvRootIndexEntryId &&
         entry.iMtm == KSenduiMtmImap4Uid &&
         entry.iType == KUidMsvServiceEntry &&
         iMsgListContainer->Count() )
        {
        FindInboxAndOpenItL();
        if ( iMsgListContainer )
            {
            iMsgListContainer->SetFolderL( iMsgListContainer->FolderEntry().Id() );
            }
        }

    if ( aSelection->Count() && iMsgListContainer )
        {
        iMsgListContainer->RefreshSelectionIndexesL( EFalse );
        iMsgListContainer->RefreshListbox();
        }

    }

// ----------------------------------------------------
// CMceMessageListView::FindAndSelectEntryL
// Sets found item in message list
// ----------------------------------------------------
void CMceMessageListView::FindAndSelectEntryL( const TDesC& aMatchString ) const
    {
    TMsvId foundEntry = iMsgListContainer->FindEntry( aMatchString );
    if ( foundEntry > KErrNotFound )
        {
        iMsgListContainer->SetCurrentItemIdL( foundEntry );
        iMsgListContainer->RefreshListbox();
        }
    }

// ----------------------------------------------------
// CMceMessageListView::HandleSortCommandL
// 
// ----------------------------------------------------
void CMceMessageListView::HandleSortCommandL( const TInt aCommand )
    {
    // set sort order in folder array
    TMsvId id = iFolderId; 
    TBool folderFound = EFalse;

    if ( aCommand == iSortOrder )
        {
    // Toggle
        iOrdering = !iOrdering;
        }
    else
        {
        iOrdering = EFalse;
        }

    for( TInt loop = 0; loop < iFolderItemArray->Count() ; loop++ )
        {
        TMceFolderItem item = ((*iFolderItemArray)[loop]);
        if ( item.iFolderId == id/*service*/ )
            {
                ((*iFolderItemArray)[loop]).iSortOrder = aCommand;
                ((*iFolderItemArray)[loop]).iOrdering = iOrdering;
                folderFound = ETrue;
            }
        }
    // add folder item to folder array
    if (!folderFound)
        {
        TMceFolderItem item;
        item.iFolderId = id/*service*/;
        item.iSortOrder = aCommand;
        item.iOrdering = iOrdering;

        iFolderItemArray->AppendL( item );
        }

    // Switch the sort ordering if needed
    iSortOrder = aCommand;

    // Update the selection in the listbox
    TMsvId currentItemId = iMsgListContainer->CurrentItemId();

    // If no focused item in one row list or focused item is folder
    // first subtitle is opened
    if ( currentItemId == KErrNotFound ||
        iMsgListContainer->IsItemFolder( currentItemId ) )
        {
        iMsgListContainer->SetContainerFlag( 
                EMceOneRowFlagFirstSubtitleOpen, EFalse );    
        }

    // Sort the items of the listbox
    iMsgListContainer->SetSortTypeL( aCommand, iOrdering );   
    iMsgListContainer->RefreshSelectionIndexesL( EFalse );

    // Update the selection in the listbox
    iMsgListContainer->SetCurrentItemIdL( currentItemId );

    }

// ----------------------------------------------------
// CMceMessageListView::SyncMlOutboxInbox
// 
// ----------------------------------------------------
TBool CMceMessageListView::SyncMlOutboxInbox( TMsvId aFolder ) const
    {
    TBool syncMl = EFalse;
    if ( IsSyncMl() )
        {
        TMsvId id = iMsgListContainer->FolderEntry().Parent();        
        TMsvEntry parentEntry;
        TMsvId serviceId;
        if ( iSession->GetEntry( id, serviceId, parentEntry ) == KErrNone )
            {
            if ( aFolder == KMsvGlobalInBoxIndexEntryId )
                {                
                if ( ( iMsgListContainer->FolderEntry().Id() == parentEntry.iMtmData1 ) ||
                     ( iMsgListContainer->FolderEntry().iRelatedId == KMsvGlobalInBoxIndexEntryId ) )
                    {                   
                    syncMl = ETrue;
                    }
                }
            else if ( aFolder == KMsvGlobalOutBoxIndexEntryId )
                {                
                if ( ( iMsgListContainer->FolderEntry().Id() == parentEntry.iMtmData2 ) ||
                    ( iMsgListContainer->FolderEntry().iRelatedId == KMsvGlobalOutBoxIndexEntryId ) )
                    {
                    syncMl = ETrue;
                    }
                }
            else if ( aFolder == KMsvDraftEntryId )
                {                
                if ( ( iMsgListContainer->FolderEntry().Id() == parentEntry.iMtmData1 ) ||
                     ( iMsgListContainer->FolderEntry().iRelatedId == KMsvDraftEntryId ) )
                    {                   
                    syncMl = ETrue;
                    }
                }
            else if ( aFolder == KMsvSentEntryId )
                {                
                if ( ( iMsgListContainer->FolderEntry().Id() == parentEntry.iMtmData1 ) ||
                     ( iMsgListContainer->FolderEntry().iRelatedId == KMsvSentEntryId ) )
                    {                   
                    syncMl = ETrue;
                    }
                }                                
            }
        }
   
    return syncMl;
    }

// ----------------------------------------------------
// CMceMessageListView::IsSyncMl
// 
// Checks if the folder is a SyncML folder
// ----------------------------------------------------
TBool CMceMessageListView::IsSyncMl() const
    {
    return ( iMceUi->IsPresent( KSenduiMtmSyncMLEmailUid ) &&
        ( iMsgListContainer->FolderEntry().iMtm == KSenduiMtmSyncMLEmailUid ) );       
    }

// ----------------------------------------------------
// CMceMessageListView::SyncMlFindInboxAndOpenItL
// ----------------------------------------------------
void CMceMessageListView::SyncMlFindInboxAndOpenItL()
    {
    TMsvId inboxId = KErrNotFound;
    TMsvEntry child;
    CMsvEntry* entry = CMsvEntry::NewL(
        *iSession,
        iMsgListContainer->CurrentFolderId(),
        TMsvSelectionOrdering(
            KMsvGroupByStandardFolders, EMsvSortByDateReverse) );
    CleanupStack::PushL( entry );
    const TInt count = entry->Count();
    for (TInt loop = 0; loop < count && inboxId == KErrNotFound; loop++)
        {
        child = (*entry)[loop];
        if ( child.iType == KUidMsvFolderEntry  &&
            child.iRelatedId == KMsvGlobalInBoxIndexEntryId )
            {
            inboxId = child.Id();
            }
        }

    if ( inboxId != KErrNotFound )
        {
        SetFolderL( inboxId );
        }
    CleanupStack::PopAndDestroy( entry );
    }
// ----------------------------------------------------
// CMceMessageListView::IsImapFolderOpenL
// ----------------------------------------------------
TBool CMceMessageListView::IsImapFolderOpenL( ) const
    {
    //imapfolders
    //we can't have imap folder if mtm isn't imap
    if( iMsgListContainer->FolderEntry().iMtm != KSenduiMtmImap4Uid )
        {
        return EFalse;
        }
    
    TMsvEntry child;    
    TInt cnt = 0;    

    CMsvEntry *parent = iSession->GetEntryL( iMsgListContainer->FolderEntry().Parent() );
    CleanupStack::PushL( parent );
    if( parent->Entry().iType != KUidMsvFolderEntry )
        {
        const TInt count = parent->Count();

        for ( TInt loop = 0; loop < count; loop++ )
            {
            child = (*parent)[loop];
            if( child.iType == KUidMsvFolderEntry )
                {
                cnt++;
                //entry = child;
                }
            }
        }

    CleanupStack::PopAndDestroy( parent );

    //if type is folder, then we must have a folder open.
    if( iMsgListContainer->FolderEntry().iType == KUidMsvFolderEntry && cnt != KMceOnlyInboxInImap )
        {
        return ETrue;
        }

    return EFalse;
    }

// ----------------------------------------------------
// CMceMessageListView::CheckMMSNotificationOpenL
// ----------------------------------------------------
TBool CMceMessageListView::CheckMMSNotificationOpenL( ) const
    {
    TBool skip = EFalse;
    TMsvId currentFolderId = iFolderId;
    const TInt count = iMsgListContainer->CurrentItemSelectionCount();
    TMsvId currentId = KErrNotFound;
    if ( count == 1 )
        {
        // one selected
        currentId = iMsgListContainer->CurrentItemId();
        }   

    if ( ( currentId != KErrNotFound ) 
        && ( currentFolderId == KMsvGlobalInBoxIndexEntryId )  )
        {        
        TMsvEntry currentEntry;
        TMsvId serviceId;
        
        if ( iSession->GetEntry( currentId, serviceId, currentEntry )
            == KErrNone )
            {            
            if ( ( currentEntry.iType == KUidMsvMessageEntry ) &&
                ( currentEntry.iMtm == KUidMsgMMSNotification ) )
                {
                CBaseMtmUiData* uiData = iMceUi->GetMtmUiDataL( KUidMsgMMSNotification );
                // Coverty change, NULL Return, http://ousrv057/cov.cgi?cid=101803
                if ( uiData )
                    {
                	skip = uiData->OperationSupportedL( KMtmUiFunctionOpenMessage, currentEntry );
                	}	
                }
            }
        }    
    return skip;
    }

// ----------------------------------------------------
// CMceMessageListView::OpenMailboxSettingsL()
// 
// ----------------------------------------------------
void CMceMessageListView::OpenMailboxSettingsL()
    {
    TInt currentIndex = 0;
    TMsvId currentId = 0;
    TInt currentCount = iMsgListContainer->Count();
    if ( currentCount > 0 )
        {
        currentId = iMsgListContainer->CurrentItemId();
        }
	    
    const TMsvEntry& entry = iMsgListContainer->FolderEntry();
    
    CAknTitlePane* title = iMceUi->TitlePaneL();
    TMsvEntry tEntry;
    TMsvId serviceId;
    User::LeaveIfError( iSession->GetEntry( entry.iServiceId, serviceId, tEntry ) );


    iMceUi->EditAccountL( tEntry.iRelatedId );
    User::LeaveIfError( iSession->GetEntry( serviceId, serviceId, tEntry ) );
    
    //set title even if inbox was opened directly
    if ( iMsgListContainer->FolderEntry().Id() == tEntry.Id() ||
            iMsgListContainer->FolderEntry().Parent() == tEntry.Id() )
        {
        title->SetTextL( tEntry.iDetails );
        }
	
    if ( iMsgListContainer->FolderEntry().Id() == tEntry.Id() )
        {
        TInt newCount = iMsgListContainer->Count();	

        if ( newCount > 0 )
            {
            if ( currentIndex >= newCount )
                {
                currentIndex = newCount - 1;		
                }
            else
                {		
                iMsgListContainer->SetCurrentItemIdL( currentId );
                }
            }
        }
    }

// ----------------------------------------------------
// CMceMessageListView::HandleStatusPaneSizeChange()
// 
// ----------------------------------------------------
void CMceMessageListView::HandleStatusPaneSizeChange()
    {
    if ( iMsgListContainer )
        {
        iMsgListContainer->SetRect( ClientRect() );
        }
    CAknView::HandleStatusPaneSizeChange();    
    }
    
//// ----------------------------------------------------
// CMceMessageListView::LaunchViewerWhenOneUnreadL( TInt &aMsgMtmUid )
// 
// ----------------------------------------------------
TInt CMceMessageListView::LaunchViewerWhenOneUnreadL( TInt &aMsgMtmUid )
    {
    if ( iRunningEditFunction || iMsgListContainer || iMceUi->MailboxCreationOn() || iMceUi->IsEditorOpen() )
        {
        return KErrNotFound;
        }
        
    // TODO: change this so that FindFirstUnreadMessageL is not called so many times!
    // TODO: this is performance optimisation: we should launch viewer before creating container!!
    // so container is not created yet, need to calculate unread message count some other way...

    TInt unreadMessageMtmUid = KErrNotFound;
    TInt numberOfUnreadMessages = 0;          
    TInt firstUnreadMessage = KErrNotFound;

    CMsvEntry* entry = CMsvEntry::NewL(
        *iSession,
        iFolderId,
        TMsvSelectionOrdering(
            KMsvGroupByStandardFolders, EMsvSortByDateReverse) );
    CleanupStack::PushL( entry );
    
    const TInt itemCount = entry->Count();    
    for ( TInt loop = 0; loop < itemCount; loop++ )
        {
        if ( (*entry)[loop].iType == KUidMsvMessageEntry &&
             (*entry)[loop].Unread() )
            {
            if ( firstUnreadMessage == KErrNotFound )
                {
                firstUnreadMessage = loop;
                unreadMessageMtmUid = (*entry)[firstUnreadMessage].iMtm.iUid;
                }
            numberOfUnreadMessages++;
            if ( numberOfUnreadMessages > 1 )
                {
                // we are only interested of first unread message and info that if
                // there are more than one unread messages.
                // So we can stop if there are more than one.
                break;
                }
            }
        }

    if ( (numberOfUnreadMessages==1) && (firstUnreadMessage!=KErrNotFound) &&
    	 (unreadMessageMtmUid!=KSenduiMtmIrUidValue) &&
    	 (unreadMessageMtmUid!=KSenduiMtmBtUidValue) &&
    	 (unreadMessageMtmUid!=KSenduiMtmSmtpUidValue) &&
    	 (unreadMessageMtmUid!=KSenduiMtmImap4UidValue) &&
    	 (unreadMessageMtmUid!=KSenduiMtmPop3UidValue)
    	 )
        {
        MCELOGGER_WRITE("LaunchMceViewer: launch editor");
        iRunningEditFunction = ETrue;
        iMceUi->EditMTMEntryL( (*entry)[firstUnreadMessage] );
        iRunningEditFunction = EFalse;
        }
    aMsgMtmUid = unreadMessageMtmUid ;
    CleanupStack::PopAndDestroy( entry );
    return ( numberOfUnreadMessages==1 ? firstUnreadMessage : KErrNotFound );
    }
	
// ----------------------------------------------------
// CMceMessageListView::SetMskObserver
// 
// ----------------------------------------------------	
void CMceMessageListView::SetMskObserver( TBool aObserver )
    {
    CEikButtonGroupContainer* bgc = Cba();
    if ( bgc )
        {
        CEikCba* cba = static_cast< CEikCba* >( bgc->ButtonGroup() );
        if ( aObserver )
            {
            cba->SetMSKCommandObserver( iMsgListContainer );
            }
        else
            {
            cba->SetMSKCommandObserver( NULL );
            }                
        }
    }

// ----------------------------------------------------
// CMceMessageListView::GetDeleteFlagStatusL
// 
// ----------------------------------------------------	
TInt CMceMessageListView::GetDeleteFlagStatusL( const CMsvEntrySelection* aEntries )
    {
    TBool allDelete = ETrue;
    TBool allUnDelete = ETrue;
    
    TMsvEntry entry;
    TMsvId serviceId;
    TMsvId id = (*aEntries)[0];
    TInt rid = 0;
    if ( iSession->GetEntry( id, serviceId, entry ) != KErrNone )
        {
        return KErrNotSupported;
        }
    if ( entry.iServiceId != KMsvLocalServiceIndexEntryId )
        {
        CBaseMtmUiData* uiData = iMceUi->GetMtmUiDataL( entry.iMtm );
        if ( uiData )
            {
            const TInt count = aEntries->Count();
            for ( TInt loop = 0; loop < count; loop++ )
                {                                 
                id = (*aEntries)[loop];
                if ( iSession->GetEntry( id, serviceId, entry ) == KErrNone )
                    {
                    if ( uiData->CanDeleteFromEntryL(entry, rid) )
                        {
                        allUnDelete = EFalse;
                        }
                    else if ( uiData->CanUnDeleteFromEntryL(entry, rid) )                    
                        {
                        allDelete = EFalse;
                        }
                    }
                }
            }
        }
        
    if ( allDelete && !allUnDelete )
        {
        // all delete
        return MessageFolderAllDelete;
        }
    else if ( !allDelete && allUnDelete )
        {
        // all undelete
        return MessageFolderAllUnDelete;
        }
    else if ( !allDelete && !allUnDelete )
        {
        // both delete and undelete
        return MessageFolderDeleteUnDelete;
        }
    else
        {
        // never
        return MessageFolderDeleteUnDelete;
        }        
    }

// ----------------------------------------------------
// CMceMessageListView::FindAllReadMsgAndMarkL()
// Private
// ----------------------------------------------------	
void CMceMessageListView::FindAllReadMsgAndMarkL()
    {
    if ( iMsgListContainer )
        {
        iMsgListContainer->MarkAllReadMessagesL();
        }
    }
    
// ----------------------------------------------------
// CMceMessageListView::IsItemUnread( TMsvId aId )  
// Public
// ----------------------------------------------------	
TBool CMceMessageListView::IsItemUnread(TMsvId aId)
    {
    TMsvEntry aEntry;
    TMsvId serviceId;
    if ( iSession->GetEntry( aId, serviceId, aEntry ) == KErrNone )
        {
        if ( aEntry.Unread() )
            {
            return ETrue;
            }
        }
        return EFalse;
    }
// ----------------------------------------------------
// CMceMessageListView::AnySelectedItemsVisible(  CArrayFixFlat<TInt>* msgIndexArray )  
// private
// ----------------------------------------------------	
TBool CMceMessageListView::AnySelectedItemsVisible ( CArrayFixFlat<TInt>* /*msgIndexArray */)
    {        
    return EFalse;
    }
    
// ----------------------------------------------------
// CMceMessageListView::EnableMarkAllReadMsgInInboxAndDocument( TMsvEntry currentEntry, 
//                                                          TMsvId currentFolderId, 
//                                                          CEikMenuPane* aMenuPane )
// private
// ----------------------------------------------------	    
void CMceMessageListView::EnableMarkAllReadMsgInInboxAndDocument( 
    TMsvEntry currentEntry, 
    TMsvId currentFolderId, 
    CEikMenuPane* aMenuPane )
    {
    if ( currentFolderId == KMsvGlobalInBoxIndexEntryId ||
        currentFolderId == KMceDocumentsEntryId ||
        iMsgListContainer->FolderEntry().Parent() == KMceDocumentsEntryId ) // subfolder of Document
        {
        if ( iMsgListContainer->IsItemFolder( currentEntry.Id() ) )
            {
            // folder item does not have mark as read/unread
            }
        else
            {
            aMenuPane->SetItemDimmed( 
                EMceCmdMarkAsReadMsg, !currentEntry.Unread() );
            aMenuPane->SetItemDimmed( 
                EMceCmdMarkAsUnreadMsg, currentEntry.Unread() );
            }                
        }
    }

// ----------------------------------------------------
// void CMceMessageListView::HandleSendkeyInMailboxL( TMsvId aEntryId )
// public
// ----------------------------------------------------
void CMceMessageListView::HandleSendkeyInMailboxL( TMsvId aEntryId )
    {
    TPtrC addressString; // mail address string 
    TBool tempDoMoreCleanup = EFalse;
    
    CMsvEntry* entry = iSession->GetEntryL( aEntryId );
    CleanupStack::PushL( entry );                 //<<<---------------------------entry
    
    if ( entry->Entry().iType != KUidMsvMessageEntry )
       {
       // do not try to call if entry is not message
       CleanupStack::PopAndDestroy( entry );
       return;
       }
    if (  entry->HasStoreL()  )  // only mails that have been retrivaled have stores
        {
        tempDoMoreCleanup = ETrue;
        CMsvStore* store = entry->ReadStoreL();
        if ( store ==  NULL)   // just in case no crash if something wrong with store
            {
            CleanupStack::PopAndDestroy( entry ); //entry, store, 
            return;
            }
        CleanupStack::PushL( store );              //<<<--------------------------- store
        CImHeader* iHeader = CImHeader::NewLC();  //<<<-----------------iHeader into stack
        iHeader->RestoreL( *store );
        TImMessageField email;
        addressString.Set( email.GetValidInternetEmailAddressFromString( iHeader->From() ) );          
        }
                     
     else  // Only mails that are not retrivaled should use iDetails instead.
        {
        TImMessageField email;
        addressString.Set( email.GetValidInternetEmailAddressFromString( entry->Entry().iDetails ) );  
        }                 
      
    CMuiuContactService* contactService = CMuiuContactService::NewL( *iEikonEnv );
    CleanupStack::PushL( contactService );             //<<<--------------- contactService
    contactService->SetAddress( addressString );
    // This LINE is commented for the new send key behaviour in mailboxes 
    // there shall be no VOIP calls but only dialer shall be opened 
    // contactService->CallL();
    
    if ( tempDoMoreCleanup )
        {
        CleanupStack::PopAndDestroy( 4, entry ); //iHeader store contactService entry
        }
    else
        {
        CleanupStack::PopAndDestroy( 2, entry ); // contactService entry
        }
    }

    
// ----------------------------------------------------
// CMceMessageListView::HandleClientRectChange
// ----------------------------------------------------
void CMceMessageListView::HandleClientRectChange()
    {
    if (iMsgListContainer)
        {
        iMsgListContainer->SetRect(ClientRect());
        }
    }
    
// ----------------------------------------------------
// CMceMessageListView::HandleEntriesCreatedChangedL
// ----------------------------------------------------
void CMceMessageListView::HandleEntriesCreatedChangedL( const CMsvEntrySelection* aSelection )
    {
    iMsgListContainer->ItemCountChangedL( ETrue ); 
    
    if ( IsMSKMailBox() ) // focus setting only for email
        {
        iMsgListContainer->SetCurrentItemIdL( iCurrentItemId );

        }
    else
        {
        TMsvEntry currentEntry;
	    TMsvId serviceId; // not used here but needed by GetEntry function
	    if ( iSession->GetEntry( aSelection->At(0), serviceId, currentEntry ) == KErrNone &&
	         serviceId == KMsvLocalServiceIndexEntryId )
	        {
            iMsgListContainer->SetCurrentItemIdL( aSelection->At(0) );
	        }
        }   
    	
    if ( iMceViewActivated )
        {
        iMsgListContainer->RefreshListbox();
        }
    }

// ----------------------------------------------------
// CMceMessageListView::HandleEntriesDeletedL
// ----------------------------------------------------
void CMceMessageListView::HandleEntriesDeletedL( )
    {
    TInt currentItem = KErrNotFound;
    TMsvId currentSelectedItemId = 0;
    if ( IsMSKMailBox() ) // focus setting only for email
        {
        if ( iMceUi->IsMailSetToBeDeletedFromServerL( iCurrentItemId ) )
            {
            currentSelectedItemId = iMceUi->AnchoredItem();
            }
        else 
            {
            currentSelectedItemId = iCurrentItemId;
            }      
        }      
    else    	
        {
        currentSelectedItemId = iMceUi->AnchoredItem();
        }

    iMsgListContainer->ItemCountChangedL( EFalse ); 
    if ( currentItem != KErrNotFound )
        {
        iMsgListContainer->RefreshSelectionIndexesL( ETrue );
        }
    iMsgListContainer->SetCurrentItemIdL( currentSelectedItemId );         
	SetCurrentItemId();
    if ( iMceViewActivated )
        {
        iMsgListContainer->RefreshListbox();
        SetMSKButtonL();
        }
    }

// ----------------------------------------------------
// CMceMessageListView::HandleEntriesMovedL
// ----------------------------------------------------
void CMceMessageListView::HandleEntriesMovedL( TMsvId aSourceFolderId, TMsvId aTargetFolderId )
    {
    if ( iFolderId == aTargetFolderId )
        {
        iMsgListContainer->ItemCountChangedL( ETrue ); 
        iMsgListContainer->RefreshSelectionIndexesL( EFalse );
        iMsgListContainer->SetCurrentItemIdL( iMceUi->AnchoredItem() );         
        if ( iMceViewActivated )
            {
            iMsgListContainer->RefreshListbox();
            }
        }
    else if ( iFolderId == aSourceFolderId )
        {
        iMsgListContainer->ItemCountChangedL( EFalse ); 
        iMsgListContainer->SetCurrentItemIdL( iMceUi->AnchoredItem() );         
        iMsgListContainer->RefreshSelectionIndexesL( EFalse );
        if ( iMceViewActivated )
            {
            iMsgListContainer->RefreshListbox();
            }
        }
    }

// ----------------------------------------------------
// CMceMessageListView::SetFolderL
// ----------------------------------------------------
void CMceMessageListView::SetFolderL( TMsvId aFolderId )
    {
    if ( iFolderId != aFolderId && iMsgListContainer )
        {
        iFolderId = aFolderId;
        if ( GetFolderListBoxType() != iMsgListContainer->MceListId() ) 
            {
            ListboxTypeChangedL();
            }
        }

    iFolderId = aFolderId;
    if ( iMsgListContainer )
        {
        iMsgListContainer->SetFolderL( aFolderId );
        }
    }

// ----------------------------------------------------
// CMceMessageListView::GetFolderListBoxType
// ----------------------------------------------------
TInt CMceMessageListView::GetFolderListBoxType()
    {
    TInt listType = EMceListTypeTwoRow; 
    TMsvEntry serviceEntry;
    TMsvId serviceId;

    if ( iSession->GetEntry( iFolderId, serviceId, serviceEntry ) == KErrNone )
        {
        const TMsvId entryId = serviceEntry.Id();
        if ( entryId == KMsvGlobalInBoxIndexEntryId 
            || entryId == KMsvDraftEntryId
            || entryId == KMsvSentEntryId 
            || entryId == KMceDocumentsEntryId
            || serviceEntry.Parent() == KMceDocumentsEntryId )
            {
            listType = iListboxInboxTypeObserver->ListboxType();
            }  
        else if ( iMceUi->CheckTechnologyTypeFromMtm( KMailTechnologyTypeUid, serviceEntry.iMtm ))
            {
            listType = iListboxMailTypeObserver->ListboxType();
            }
    }
    return listType;
    }

// ----------------------------------------------------
// CMceMessageListView::ListContainer
// ----------------------------------------------------
CMceMessageListContainerBase* CMceMessageListView::ListContainer() const
    {
    return iMsgListContainer;
    }


// ----------------------------------------------------
// CMceMessageListView::HandleMsgServerStartupL
// ----------------------------------------------------
void CMceMessageListView::HandleMsgServerStartupL()
    {
    iSession->AddObserverL( *this );
    iMsvSessionReady = ETrue;
    }

// ----------------------------------------------------
// CMceMessageListView::AiwSyncParamListLC()
// 
// ----------------------------------------------------
CAiwGenericParamList* CMceMessageListView::AiwSyncParamListLC(TInt /*aId*/, const TDesC& aText)
    {
	TAiwVariant variant;
    variant.Set(KNullDesC);
	TAiwGenericParam param(EGenericParamMessageItemEMail, variant);

	TAiwVariant variant2;
    variant2.Set(aText);
	TAiwGenericParam param2(EGenericParamMessageItemMbox, variant2);
            
	CAiwGenericParamList* list = CAiwGenericParamList::NewLC();
	list->AppendL(param);
	list->AppendL(param2);
	
	return list;
    }

// ----------------------------------------------------
// CMceMessageListView::CheckUnreadMessages
//
// ----------------------------------------------------
TBool CMceMessageListView::HasUnreadMessagesL()
	{
	// TODO: to list array or container???
    CMsvEntry* entry = CMsvEntry::NewL(
        *iSession,
        iMsgListContainer->CurrentFolderId(),
        TMsvSelectionOrdering(
            KMsvGroupByStandardFolders, EMsvSortByDateReverse) );
    CleanupStack::PushL( entry );
    
	const TInt count = entry->Count();
	
	for ( TInt i=0; i<count; ++i )
		{
		if ( (*entry)[i].Unread() )
			{
            CleanupStack::PopAndDestroy( entry );
			// Current folder has unread/new message
			return ETrue;
			}
		}	
    CleanupStack::PopAndDestroy( entry );
	return EFalse;
	}

// ----------------------------------------------------
// CMceMessageListView::SetMSKButtonL()
// 
// ----------------------------------------------------
void CMceMessageListView::SetMSKButtonL()
    {
    
    if ( AknLayoutUtils::MSKEnabled() )
        {
        TMsvId newFolderId = iFolderId;
        TMsvId syncFolderId = 0;
        TInt resourceId = R_MCE_MSK_BUTTON_OPEN;
        TInt numberOfEntries = iMsgListContainer->Count();
        TInt currentItemSelectionCount = iMsgListContainer->SelectionCount();
        
        if ( IsMSKSyncMlMailBox() )
            {
            syncFolderId = iMsgListContainer->FolderEntry().iRelatedId;
            if ( syncFolderId > 0 && newFolderId != syncFolderId )
                {
                newFolderId = syncFolderId;
                }
            }
        
        if ( IsMSKMailBox() && ( !IsMSKSyncMlMailBox() ) )
            {
            TInt rid = 0; // command is available
            TMsvId id = iMsgListContainer->CurrentItemId();
            if ( id < 0 )
                {
                if ( CMceMessageListContainerBase::EMessageListItemSubtitle 
                    != iMsgListContainer->CurrentItemType())
                    {
                    resourceId = R_MCE_MSK_BUTTON_CONTEXT_OPTIONS;                        
                    }
                else
                    {
                    resourceId = iMsgListContainer->SetSpecialMskL();
                    }
                }
            else
                {
                TMsvEntry currentEntry;
                TMsvId serviceId;
                iSession->GetEntry( id, serviceId, currentEntry );

                TMsvEntry serviceEntry;
                iSession->GetEntry( currentEntry.iServiceId, serviceId, serviceEntry );
                        
                CBaseMtmUiData* mtmUiData = iMceUi->GetMtmUiDataL( currentEntry.iMtm );

                if ( !mtmUiData)
                    {
                    resourceId = R_MCE_MSK_BUTTON_EMPTY;
                    }
                else if ( mtmUiData->CanUnDeleteFromEntryL( currentEntry, rid ) &&
                     !serviceEntry.Connected() && currentItemSelectionCount == 0  ) 
                    {
                    // Offline and deleted item in focus
                    resourceId = R_MCE_MSK_BUTTON_UNDELETE;
                    }
                else
                    {
                    if ( currentItemSelectionCount >= 1 || numberOfEntries == 0 )
                        {
                        resourceId = R_MCE_MSK_BUTTON_CONTEXT_OPTIONS;
                        }
                    else
                        {
                        if ( CMceMessageListContainerBase::EMessageListItemSubtitle == iMsgListContainer->CurrentItemType())
                            {
                            resourceId = iMsgListContainer->SetSpecialMskL();
                            }
                        else
                            {
                            resourceId = R_MCE_MSK_BUTTON_OPEN;        
                            }
                        }
                    }
                }
            }
        else if ( newFolderId == KMceDeliveryReportsId )
            {
            // Delivery reports
            if ( numberOfEntries > 0 )
                {
                resourceId = R_MCE_MSK_BUTTON_CONTEXT_OPTIONS;
                }
            else
                {
                resourceId = R_MCE_MSK_BUTTON_EMPTY;
                }        
            }
        else if ( newFolderId == KMsvGlobalOutBoxIndexEntryId )
            {
            // Outbox 
            if ( numberOfEntries > 0 )
                {
                resourceId = R_MCE_MSK_BUTTON_CONTEXT_OPTIONS;
                }
            else if ( numberOfEntries == 0 )
                {
                if ( IsMSKSyncMlMailBox() )
                    {
                    resourceId = R_MCE_MSK_BUTTON_CONTEXT_OPTIONS;
                    }
                else
                    {
                    resourceId = R_MCE_MSK_BUTTON_EMPTY;
                    }                    
                }
            }
        else if ( newFolderId == KMceDocumentsEntryId )
            {
            // Documents, user's own folder
            if ( currentItemSelectionCount >= 1 )
                {
                resourceId = R_MCE_MSK_BUTTON_CONTEXT_OPTIONS;
                }
            else
                {
                if ( CMceMessageListContainerBase::EMessageListItemSubtitle == iMsgListContainer->CurrentItemType())
                    {
                    resourceId = iMsgListContainer->SetSpecialMskL();
                    }
                else
                    {
                    resourceId = R_MCE_MSK_BUTTON_OPEN;        
                    }
                }
            }
        else if ( newFolderId == KMsvGlobalInBoxIndexEntryId ||
                  newFolderId == KMsvSentEntryId ||
                  newFolderId == KMceTemplatesEntryId )
            {
            // Inbox, Sent Items, Templates
            if ( currentItemSelectionCount >= 1 ) 
                {
                resourceId = R_MCE_MSK_BUTTON_CONTEXT_OPTIONS;
                }
            else if ( numberOfEntries == 0 )
                {
                if ( IsMSKSyncMlMailBox() )
                    {
                    resourceId = R_MCE_MSK_BUTTON_CONTEXT_OPTIONS;
                    }
                else
                    {
                    resourceId = R_MCE_MSK_BUTTON_EMPTY;
                    }
                }
            else
                {
                // One row list handles expand/collape MSK setting
                if ( CMceMessageListContainerBase::EMessageListItemSubtitle != iMsgListContainer->CurrentItemType())
                    {                        
                    TMsvId id = iMsgListContainer->CurrentItemId();
                    TMsvEntry currentEntry;
                    TMsvId serviceId;
                    iSession->GetEntry( id, serviceId, currentEntry );
                           
                    CBaseMtmUiData* mtmUiData = iMceUi->GetMtmUiDataL( currentEntry.iMtm );
                    // Coverty fix NULL Return, http://ousrv057/cov.cgi?cid=101802
                    if ( currentEntry.iMtm == KUidMsgMMSNotification && mtmUiData &&
                        mtmUiData->OperationSupportedL(KMtmUiFunctionOpenMessage, currentEntry ) )
                        {
                        // when MMSNotification is in retrieving status, the hide open for MSK
                        resourceId = R_MCE_MSK_BUTTON_EMPTY;
                        }
                    else 
                        {
                        resourceId = R_MCE_MSK_BUTTON_OPEN;
                        } 
                    }
                else
                    {
                    if ( CMceMessageListContainerBase::EMessageListItemSubtitle == iMsgListContainer->CurrentItemType())
                        {
                        resourceId = iMsgListContainer->SetSpecialMskL();
                        }
                    else
                        {
                        resourceId = R_MCE_MSK_BUTTON_OPEN;        
                        }
                    }
                }
            }
        else if ( newFolderId == KMsvDraftEntryId )
            {
            // Drafts
            if ( currentItemSelectionCount >= 1 ) 
                {
                resourceId = R_MCE_MSK_BUTTON_CONTEXT_OPTIONS;
                }
            else if ( numberOfEntries == 0 )
            	{
            	if ( IsMSKSyncMlMailBox() )
                    {
                    resourceId = R_MCE_MSK_BUTTON_CONTEXT_OPTIONS;
                    }
               else
               	{
               	resourceId = R_MCE_MSK_BUTTON_EMPTY;
               	}            	
            	}
            else
                {
                if ( CMceMessageListContainerBase::EMessageListItemSubtitle == iMsgListContainer->CurrentItemType())
                    {
                    resourceId = iMsgListContainer->SetSpecialMskL();
                    }
                else
                    {
                    resourceId = R_MCE_MSK_BUTTON_OPEN;        
                    }
                }
            }
        else
            {
            // User's own folders
            if ( currentItemSelectionCount >= 1 )
                {
                resourceId = R_MCE_MSK_BUTTON_CONTEXT_OPTIONS;
                }
            else if ( numberOfEntries == 0 )
                {
                resourceId = R_MCE_MSK_BUTTON_EMPTY;
                }
            else
                {
                // One row list handles expand/collape MSK setting
                if ( CMceMessageListContainerBase::EMessageListItemSubtitle != iMsgListContainer->CurrentItemType())
                    {                        
                    TMsvId id = iMsgListContainer->CurrentItemId();
                    TMsvEntry currentEntry;
                    TMsvId serviceId;
                    iSession->GetEntry( id, serviceId, currentEntry );
                           
                    CBaseMtmUiData* mtmUiData = iMceUi->GetMtmUiDataL( currentEntry.iMtm );
                    // Coverty fix, NULL Return, http://ousrv057/cov.cgi?cid=101802
                    if ( currentEntry.iMtm == KUidMsgMMSNotification && mtmUiData &&
                        mtmUiData->OperationSupportedL(KMtmUiFunctionOpenMessage, currentEntry ) )
                        {
                        // when MMSNotification is in retrieving status, the hide open for MSK
                        resourceId = R_MCE_MSK_BUTTON_EMPTY;
                        }
                    else 
                        {
                        resourceId = R_MCE_MSK_BUTTON_OPEN;
                        } 
                    }
                else
                    {
                        resourceId = iMsgListContainer->SetSpecialMskL();
                    }
                }
            }     
        
        CEikButtonGroupContainer* cba = Cba();
        cba->SetCommandL( KMSKPosition, resourceId );
        cba->DrawDeferred();
        
        }
    }
    
    
// ----------------------------------------------------
// CMceMessageListView::IsMSKMailBox()
// 
// ----------------------------------------------------    
TBool CMceMessageListView::IsMSKMailBox()
    {
    TBool mailBox = EFalse;
    const TMsvEntry& entry = iMsgListContainer->FolderEntry();
    if ( iMceUi->CheckTechnologyTypeFromMtm( KMailTechnologyTypeUid, entry.iMtm ) )
        {
        mailBox = ETrue;
        }
    return mailBox;
    }
    
// ----------------------------------------------------
// CMceMessageListView::IsMSKSyncMlMailBox()
// 
// ----------------------------------------------------    
TBool CMceMessageListView::IsMSKSyncMlMailBox()
    {
    TBool syncMailBox = EFalse;
    const TMsvEntry& entry = iMsgListContainer->FolderEntry();
    if ( iMceUi->IsPresent( entry.iMtm ) &&
         iMsgListContainer->FolderEntry().iMtm == KSenduiMtmSyncMLEmailUid ) 
        {
        syncMailBox = ETrue;
        }
    return syncMailBox;
    }

 // ----------------------------------------------------
// CMceMessageListView::SetCurrentItemId
// ----------------------------------------------------
void CMceMessageListView::SetCurrentItemId()
    {
    if ( iMsgListContainer && IsMSKMailBox() )
        {
        iCurrentItemId = iMsgListContainer->CurrentItemId();
        }
    else
        {
        iCurrentItemId = KErrNotFound;        
        }
    }
    
    
// ----------------------------------------------------
// CMceMessageListView::CreateListboxL
// ----------------------------------------------------
void CMceMessageListView::CreateListboxL()
    {
    TInt newListType = EMceListTypeTwoRow;
    
    newListType = GetFolderListBoxType();

    if ( !iMsgListContainer )
        {
        if ( newListType != EMceListTypeOneRow )
            {
            iMsgListContainer = CMceMessageListContainer::NewL(
                ClientRect(),
                iSession,
                iFolderId,
                iBitmapResolver,
                *this );
            iCurrentListType = EMceListTypeTwoRow;
            }
        else
            {
            iMsgListContainer = CMceOneRowMessageListContainer::NewL(
                *this,
                ClientRect(),
                iSession,
                *iMceUi );
            iCurrentListType = EMceListTypeOneRow;
            }

        iMsgListContainer->SetMopParent( this );
        iMsgListContainer->SetMtmUiDataRegistry( iMceUi );
        }

    iCurrentListboxId = iMsgListContainer->MceListId();
    }
    

// ----------------------------------------------------
// CMceMessageListView::EnvironmentChanged
// ----------------------------------------------------
TInt CMceMessageListView::EnvironmentChanged( TAny* aSelf )
    {
    TInt err = KErrArgument;
    CMceMessageListView* self = static_cast<CMceMessageListView*>(aSelf);
    if ( self )
        {
        err = self->HandleEnvironmentChanged();
        }
    return err;        
    
    }
    
// ----------------------------------------------------
// CMceMessageListView::HandleEnvironmentChanged
// ----------------------------------------------------
TInt CMceMessageListView::HandleEnvironmentChanged()
    {
    TInt err = KErrNone;
    if ( iMsgListContainer &&
        iDateTimeNotifier ) 
        {
        TInt change = iDateTimeNotifier->Change();
        if ( change & EChangesMidnightCrossover ||
             change & EChangesLocale ||
             change & EChangesSystemTime )
            {
            // inform container that date/time format has been changed
            TRAP( err, iMsgListContainer->DateTimeFormatChangedL( change ) );
            }
        }
    return err;
    }

    
// ----------------------------------------------------
// CMceMessageListView::ListboxTypeChangedL
// ----------------------------------------------------
void CMceMessageListView::ListboxTypeChangedL()
    {
    if (iMsgListContainer)
        {
        CEikonEnv* eikEnv = CEikonEnv::Static();
        if( eikEnv )
            {
            // New listbox will overlap any remaining dialog, 
            // so we'll close them here.
            AknDialogShutter::ShutDialogsL( *eikEnv );
            }
        
        iMsgListContainer->MakeVisible(EFalse);
        AppUi()->RemoveFromStack(iMsgListContainer);
        delete iMsgListContainer;
        iMsgListContainer = NULL;
        iLocalScreenClearer = CAknLocalScreenClearer::NewL( EFalse );
        CreateListboxL();
        iMsgListContainer->SetFolderL( iFolderId );
        iMsgListContainer->ClearSelection();
        iMsgListContainer->SetRect(ClientRect());
        SetMskObserver( ETrue );
        AppUi()->AddToStackL( *this,iMsgListContainer );
        iMsgListContainer->ActivateL();
        iMsgListContainer->MakeVisible(ETrue);
        delete iLocalScreenClearer;
        iLocalScreenClearer = NULL;
        }
    }

// ----------------------------------------------------
// CMceMessageListView::SetForwardMenuL
// ----------------------------------------------------
void CMceMessageListView::SetForwardMenuL( CEikMenuPane* aMenuPane, TMsvEntry& aEntry )
    {
    TBool canForward = EFalse;
    switch( aEntry.iMtm.iUid ) 
        {
        case KSenduiMtmIrUidValue:
        case KSenduiMtmBtUidValue:
        case KSenduiMtmBioUidValue:
            {
            canForward = EFalse;
            break;
            }
        case KSenduiMtmSmsUidValue:
            {
            canForward = !( aEntry.iBioType ) && 
                iMceUi->CheckCommandAvailableL( EMceCmdForward, aEntry ) ;
            break;
            }
        default:
            {
            canForward = iMceUi->CheckCommandAvailableL( EMceCmdForward, aEntry );
            }
        }
    aMenuPane->SetItemDimmed( EMceCmdForward, !canForward );
    }
            



// ---------------------------------------------------------
// CMceMessageListView::DimReadUnreadOptionsMenu
// ---------------------------------------------------------
//    
void CMceMessageListView::DimReadUnreadOptionsMenu( CEikMenuPane *aMenuPane )
    {
    aMenuPane->SetItemDimmed( EMceCmdMarkAsRead, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdMarkAsUnread, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdMarkAsReadMsg, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdMarkAsUnreadMsg, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdMarkAsReadMsgs, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdMarkAsUnreadMsgs, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdMarkAsReadEmails, ETrue );
    aMenuPane->SetItemDimmed( EMceCmdMarkAsUnreadEmails, ETrue );
    }

// ---------------------------------------------------------
// CMceMessageListView::SetReadUnreadOptionsMenu
// ---------------------------------------------------------
//    
void CMceMessageListView::SetReadUnreadOptionsMenu( CEikMenuPane *aMenuPane, TBool aIsEmail, TBool aSetRead )
    {
    if ( aIsEmail )
        {// emails
        switch ( aSetRead )
            {
            case MessageFolderAllRead:
                aMenuPane->SetItemDimmed( EMceCmdMarkAsReadEmails, ETrue );
                aMenuPane->SetItemDimmed( EMceCmdMarkAsUnreadEmails, EFalse );
                break;
            case MessageFolderAllUnread:
                aMenuPane->SetItemDimmed( EMceCmdMarkAsReadEmails, EFalse );
                aMenuPane->SetItemDimmed( EMceCmdMarkAsUnreadEmails, ETrue );
                break;
            case MessageFolderReadUnread:
                aMenuPane->SetItemDimmed( EMceCmdMarkAsReadEmails, EFalse );
                aMenuPane->SetItemDimmed( EMceCmdMarkAsUnreadEmails, EFalse );
                break;
            default:
                // never
                break;
            }
        }
    else
        {// messages 
        switch ( aSetRead )
            {
            case MessageFolderAllRead:
                aMenuPane->SetItemDimmed( EMceCmdMarkAsReadMsgs, ETrue );
                aMenuPane->SetItemDimmed( EMceCmdMarkAsUnreadMsgs, EFalse );
                break;
            case MessageFolderAllUnread:
                aMenuPane->SetItemDimmed( EMceCmdMarkAsReadMsgs, EFalse );
                aMenuPane->SetItemDimmed( EMceCmdMarkAsUnreadMsgs, ETrue );
                break;
            case MessageFolderReadUnread:
                aMenuPane->SetItemDimmed( EMceCmdMarkAsReadMsgs, EFalse );
                aMenuPane->SetItemDimmed( EMceCmdMarkAsUnreadMsgs, EFalse );
                break;
            default:
                // never
                break;
            }
        }
    }
      
// ----------------------------------------------------
// CMceMessageListView::SetMarkReadUnread( TBool aValue )
// ----------------------------------------------------
void CMceMessageListView::SetMarkReadUnread( TBool aValue )
    {
    iReadUnread = aValue;
    }

// ----------------------------------------------------
// CMceMessageListView::SetContextCommandFlag( TBool aContextCommandFlag )
// ----------------------------------------------------	
void CMceMessageListView::SetContextCommandFlag(TBool aContextCommandFlag)
    {
    iContextCommand = aContextCommandFlag;
    }

// ----------------------------------------------------
// CMceMessageListView::GetLocalScreenClearer( CAknLocalScreenClearer** &aClearer )
// ---------------------------------------------------- 
void CMceMessageListView::GetLocalScreenClearer( CAknLocalScreenClearer** &aClearer )        
    {
    aClearer = &iLocalScreenClearer;
    }
// ----------------------------------------------------
// CMceMessageListView::SetMarkingMode( TBool aMarkingModeOn )
// ---------------------------------------------------- 

void CMceMessageListView::SetMarkingMode( TBool aMarkingModeOn )
    {
    iMarkingModeOn = aMarkingModeOn;
    }
// ----------------------------------------------------
// CMceMessageListView::MarkingMode() const
// ---------------------------------------------------- 

TBool CMceMessageListView::MarkingMode() const
    {
    return iMarkingModeOn;  
    }	
// ----------------------------------------------------
// CMceMessageListView::MessageCount() const
// ---------------------------------------------------- 

TInt CMceMessageListView::MessageCount() const
    {
    if ( iMsgListContainer )
        {
        return iMsgListContainer->MessageCount();
        }
    return 0;
    }
// ----------------------------------------------------
// CMceMessageListView::SetMarkingModeOff()
// ---------------------------------------------------- 

void CMceMessageListView::SetMarkingModeOff()
    {
    if( iMsgListContainer )
        {
        iMsgListContainer->SetMarkingModeOff();
        }
    iMarkingModeOn = EFalse;
    }

//  End of File
