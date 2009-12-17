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
*     Main view of the Mce.
*
*/



// INCLUDE FILES

#include <eikclbd.h>
#include <msvuids.h>
#include <AknQueryDialog.h> // CAknQueryDialog
#include <StringLoader.h>   // StringLoader
#include <aknlists.h>
#include <akntitle.h>       // CAknTitlePane
#include <SenduiMtmUids.h>  // mtm uids
#include <eikmenub.h>
#include <msvids.h>
#include <featmgr.h>
#include <SchemeHandler.h>
#include <ConeResLoader.h>
#include <apgcli.h>
#include <msvstd.hrh>

#include <mce.rsg>
#include "mceui.h"
#include "MceSessionHolder.h"
#include "MceBitmapResolver.h"
#include "MceMainViewListContainer.h"
#include "MceMainViewEmptyListContainer.h"
#include "MceMainViewListView.h"
#include "MceMainViewListItemArray.h"
#include "MceCommands.hrh"
#include "McePanic.h"

#include <bldvariant.hrh>

#include <MNcnInternalNotification.h>
#include <NcnNotificationDefs.h>
#include "MceListItem.h"

#include <ImumInternalApi.h>
#include <ImumInHealthServices.h>
#include <muiumsvuiserviceutilitiesinternal.h>
#include <MessagingDomainCRKeys.h>
#include <miutset.h>
#include <mtudreg.h>
#include <muiuflags.h>
#include <ImumInternalApi.h>
#include <muiulock.h>
#include <messaginginternalcrkeys.h>
#include <aknnotedialog.h>

// CONSTANTS
const TInt   KMceWriteMessageAddIndex = 0;
// Add 'Write message' to the first
const TInt   KMceNoRemotemailboxesAddIndex = 3;
// Add 'No mailboxes' right after 'Documents'
const TInt   KMceShowMail = 1;
// Mce is activated from new contacts note for mail
const TInt KMceShowAudioMessage = 3;
// Mce is activated from new contacts note for audio message
#define KMessagingCentreMessageViewUid TVwsViewId( TUid::Uid( KMceApplicationUidValue ),TUid::Uid( KMceMessageListViewIdValue ) )
// Mail technology type
const TUid KMailTechnologyTypeUid = { 0x10001671 };
// The position of the middle soft key
const TInt KMSKPosition = 3;


// ================= MEMBER FUNCTIONS =======================

CMceMainViewListView* CMceMainViewListView::NewL(
    CMsvSessionPtr aSession,
    CMceSessionHolder& aSessionHolder,
    CMceBitmapResolver& aBitmapResolver )
    {
    MCELOGGER_ENTERFN("CMceMainViewListView::NewL()");
    CMceMainViewListView* self = new (ELeave) CMceMainViewListView(
        aSession, aSessionHolder, aBitmapResolver );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    MCELOGGER_LEAVEFN("CMceMainViewListView::NewL()");
    return self;
    }

CMceMainViewListView::CMceMainViewListView(
    CMsvSessionPtr aSession,
    CMceSessionHolder& aSessionHolder,
    CMceBitmapResolver& aBitmapResolver )
    :
/*    CMceMainViewListView(
        aSession,
        KMsvRootIndexEntryIdValue,
        aSessionHolder, aBitmapResolver ),*/
    iSession( aSession ),
    iSessionHolder( aSessionHolder ),
    iBitmapResolver( aBitmapResolver ),
    iSelectedIndex( KErrNotFound ),
    iTopItemIndex( 0 ),
    iSelectionFolderId( KMsvNullIndexEntryId ),
		iSelectableEmail(EFalse), 
		iEmailClientIntegration(EFalse),
		iEmailFramework(EFalse)
    {
    iSessionHolder.AddClient();
    }

CMceMainViewListView::~CMceMainViewListView()
    {

    if ( iEmptyListContainer )
        {
        AppUi()->RemoveFromStack(iEmptyListContainer);
        delete iEmptyListContainer;
        iEmptyListContainer = NULL;
        }

    if (iMsgListContainer)
        {
        CAknDoubleLargeStyleListBox* listBox = REINTERPRET_CAST( CAknDoubleLargeStyleListBox*,
            iMsgListContainer->ListBox() );

        listBox->ItemDrawer()->ColumnData()
            ->SetIconArray( NULL );
        // prevent icon array to be destroyed by column data
        if ( iListViewFlags.MceFlag( EMceUiFlagsContainerAddedToStack ) )
            {
            AppUi()->RemoveFromStack(iMsgListContainer);
            iListViewFlags.ClearMceFlag( EMceUiFlagsContainerAddedToStack );
            }
        }
    delete iMsgListContainer;
    
    delete iFolderEntry;
    // iSession->RemoveObserver( *this ) is called in HandleSessionEventL()
	delete iAiwServiceHandler;

    iSessionHolder.RemoveClient();

    }

void CMceMainViewListView::ConstructL()
    {
    iMceUi = STATIC_CAST( CMceUi*, AppUi() );
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdEmailMceIntegration ) )
        {
        iEmailClientIntegration = ETrue;
        CRepository* repository = NULL;
        TRAPD( ret, repository = CRepository::NewL(
                                 KCRUidSelectableDefaultEmailSettings ) );

        if ( ret == KErrNone )
            {
            TInt err = repository->Get( KIntegratedEmailAppMtmPluginId,
                       iMtmPluginId );
            if ( err != KErrNone )
                {
                iMtmPluginId = 0;
                }
            }
        delete repository;
        }

    if ( FeatureManager::FeatureSupported( KFeatureIdSelectableEmail ) )
        {
        iSelectableEmail = ETrue;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdFfEmailFramework ) )
        {
        iEmailFramework = ETrue;
        }
    FeatureManager::UnInitializeLib();

    BaseConstructL( R_MCE_MAIN_VIEW );
//    CMceMainViewListView::ConstructL( EMceListTypeMainView );
    }

// ----------------------------------------------------
// CMceMainViewListView::Id
// ----------------------------------------------------
TUid CMceMainViewListView::Id() const
    {
    return KMceMainViewListViewId;
    }

// ----------------------------------------------------
// CMceMainViewListView::DoActivateL
// ----------------------------------------------------
void CMceMainViewListView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/,
    TUid aCustomMessageId,
    const TDesC8& /*aCustomMessage*/)
    {
    MCELOGGER_ENTERFN("CMceMainViewListView::DoActivateL");   
    
    if ( aCustomMessageId.iUid == KMceHideInBackground )
    	{
        iSelectedIndex = KMceWriteMessageAddIndex;
        ConstructMainViewL();
        iMceUi->SetCustomControl(0); // Enable bring-to-foreground on view activation
    	}   
    
    iDrawListbox = ETrue;
    iPerformOperationAfterMsgStart = KErrNone;
    
    iMceUi->SetMceViewActive( EMceMainViewActive );
    iMceUi->SetMainViewActivatedFlag( );
    if ( !iMsvSessionReady )
        {
        if ( !iEmptyListContainer  )
            {
            iEmptyListContainer = CMceMainViewEmptyListContainer::NewL( ClientRect() );
            AppUi()->AddToStackL(*this,iEmptyListContainer);
            MCELOGGER_WRITE_TIMESTAMP("Main view: Empty container activated at ");
            }
        if ( aCustomMessageId.iUid == KMceShowMail )
            {
            iPerformOperationAfterMsgStart = KMceShowMail;
            }
    	else if ( aCustomMessageId.iUid == KMceShowAudioMessage )
    	    {
            iPerformOperationAfterMsgStart = KMceShowAudioMessage;
    	    }
        MCELOGGER_LEAVEFN("CMceMainViewListView::DoActivateL2");   
        return;
        }

    TMsvId mailboxId = NULL; // the first mailbox id, in which there is unread mail
    TInt mailboxCount = 0; // the count of mailboxes, in which there are unread mail  

    if ( iMceUi->IsEditorOpen() )
        {
        // something is embedded in mce, wait for that to be closed
        iMceUi->CloseEditorApp();
        }
        
    if ( aCustomMessageId.iUid == KMceShowMail )
        {
        MCELOGGER_WRITE("CMceMainViewListView::DoActivateL ClearDontExitOnNextOperationComplete");
        iMceUi->SetDontExitOnNextOperationComplete();
        
        mailboxCount = MailboxWithUnreadMessagesL( mailboxId );
        MCELOGGER_WRITE_FORMAT("CMceMainViewListView::DoActivateL mailboxCount %d", mailboxCount);   
        MCELOGGER_WRITE_FORMAT("CMceMainViewListView::DoActivateL mailboxId 0x%x", mailboxId );   

        if ( mailboxId && mailboxCount == 1 )
            {
            //activate remote folder view using mailboxId
            ActivateViewL ( KMessagingCentreMessageViewUid, TUid::Uid( mailboxId ),KNullDesC8() );           
            MCELOGGER_LEAVEFN("CMceMainViewListView::DoActivateL3");   
            return;
            }        
        }
	else if ( aCustomMessageId.iUid == KMceShowAudioMessage )
        {
        //activate remote folder view using inboxId
        ActivateViewL ( KMessagingCentreMessageViewUid, TUid::Uid( KMsvGlobalInBoxIndexEntryIdValue ),KNullDesC8() );      
        MCELOGGER_LEAVEFN("CMceMainViewListView::DoActivateL4");   
        return;
        }
        
    if ( iMsvSessionReady )
        {
        if ( iEmptyListContainer )
            {
            AppUi()->RemoveFromStack(iEmptyListContainer);
            delete iEmptyListContainer;
            iEmptyListContainer = NULL;
            MCELOGGER_WRITE_TIMESTAMP("Main view: Empty container deleted at ");
            }
        CreateContainerAndActivateL();
        //folders navi
        iMceUi->RemoveFolderTabs();

        iMceUi->RemoveTabs();
        SetSpecialMSKButtonL(R_MCE_MSK_BUTTON_OPEN);
        }
/*    else if ( !iEmptyListContainer  )
        {
        iEmptyListContainer = CMceMainViewEmptyListContainer::NewL( ClientRect() );
        AppUi()->AddToStackL(*this,iEmptyListContainer);
        MCELOGGER_WRITE_TIMESTAMP("Main view: Empty container activated at ");
        }*/
    iMceUi->SetMceViewActive( EMceMainViewActive );

    if ( mailboxId && mailboxCount > 1 )
        {
        // launched from outside:
        // in several mailboxes unread mail, activate the first mailbox
        TInt mailboxIndex =  iMsgListContainer->ItemIndex( mailboxId );
        iMsgListContainer->ListBox()->SetCurrentItemIndex( mailboxIndex );
        iMsgListContainer->ListBox()->DrawDeferred();        
        }

    iMceUi->ToPhoneMemoryQueryL( iMsvSessionReady );
    MCELOGGER_LEAVEFN("CMceMainViewListView::DoActivateL1");   
    
    }

// ----------------------------------------------------
// CMceMainViewListView::CreateContainerAndActivateL
// ----------------------------------------------------
void CMceMainViewListView::CreateContainerAndActivateL()
    {
    MCELOGGER_ENTERFN("CreateContainerAndActivateL()");
    if ( !iMsgListContainer )
        {
        iMsgListContainer = CMceMainViewListContainer::NewL(
            ClientRect(),
            iSession,
            iBitmapResolver,
            KMsvRootIndexEntryIdValue,
            EMceListTypeMainView );
        iMsgListContainer->SetMopParent( this );
        iMsgListContainer->ListItems()->SetListItemArrayObserver( this );
        ResetBitmapsL();
        iMsgListContainer->SetMskL();
        }

    if ( !iDrawListbox )
        {
        return;
        }



    CAknDoubleLargeStyleListBox* listBox = REINTERPRET_CAST( CAknDoubleLargeStyleListBox*,
        iMsgListContainer->ListBox() );

    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    listBox->ItemDrawer()->ColumnData()
        ->SetIconArray( iBitmapResolver.IconArray() );
    iMsgListContainer->SetRect(ClientRect());
    // Handle the layout switch iMceUi->TitlePaneL()
    if ( iMceUi->ResourceChangeCalled() )
        {
        iMsgListContainer->HandleResourceChange( KEikDynamicLayoutVariantSwitch );
        iMceUi->ResetResourceChange();
        }
    listBox->SetListBoxObserver(this);

    iMsgListContainer->ActivateL();
    if ( !iListViewFlags.MceFlag( EMceUiFlagsContainerAddedToStack ) )
        {
        AppUi()->AddToStackL(*this,iMsgListContainer);
        iListViewFlags.SetMceFlag( EMceUiFlagsContainerAddedToStack );
        }

    if ( iEmptyListContainer )
        {
        AppUi()->RemoveFromStack(iEmptyListContainer);
        delete iEmptyListContainer;
        iEmptyListContainer = NULL;
        }

    listBox->SetTopItemIndex( iTopItemIndex );
    if ( iSelectionFolderId > KMsvRootIndexEntryId )
        {
        TInt index = iMsgListContainer->ItemIndex( iSelectionFolderId );
        if ( index > KErrNotFound )
            {
            listBox->SetCurrentItemIndex( index );
            listBox->UpdateScrollBarsL();
            }
        iSelectionFolderId = KMsvNullIndexEntryId;
        }
    else if ( iSelectedIndex >= 0 )
        {
        listBox->SetCurrentItemIndex( iSelectedIndex );
        listBox->UpdateScrollBarsL();
        }
    listBox->DrawNow();

    // Fetch pointer to the default title pane control

    CAknTitlePane* title=iMceUi->TitlePaneL();
    HBufC* text = StringLoader::LoadLC( R_MCE_MAIN_VIEW_TITLE, iEikonEnv );
    title->SetTextL( *text );
    CleanupStack::PopAndDestroy(); // text

    iMceViewActivated = ETrue;
    MCELOGGER_LEAVEFN("CreateContainerAndActivateL()");
    MCELOGGER_WRITE_TIMESTAMP("Main view activated at ");
    }


// ----------------------------------------------------
// CMceMainViewListView::DoDeactivate
// ----------------------------------------------------
void CMceMainViewListView::DoDeactivate()
    {
    if ( iEmptyListContainer )
        {
        AppUi()->RemoveFromStack(iEmptyListContainer);
        delete iEmptyListContainer;
        iEmptyListContainer = NULL;
        }

    if ( iMsgListContainer )
        {
        iSelectedIndex = iMsgListContainer->CurrentItemIndex();
        iTopItemIndex = iMsgListContainer->ListBox()->TopItemIndex();
        if ( iListViewFlags.MceFlag( EMceUiFlagsContainerAddedToStack ) )
            {
            AppUi()->RemoveFromStack(iMsgListContainer);
            iListViewFlags.ClearMceFlag( EMceUiFlagsContainerAddedToStack );
            }
        }
    iMceViewActivated = EFalse;
    }

// ----------------------------------------------------
// CMceMainViewListView::HandleListBoxEventL
// ----------------------------------------------------
void CMceMainViewListView::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType )
    {
    switch(aEventType)
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            ChangeViewL();
        break;
        case EEventItemDraggingActioned:
            if ( iMsgListContainer )
                {
                iMsgListContainer->SetMskL();                
                }
        break;
        default:
        break;

        }
    }

// ----------------------------------------------------
// CMceMainViewListView::HandleCommandL
// ----------------------------------------------------
void CMceMainViewListView::HandleCommandL( TInt aCommand )
    {
    MCELOGGER_WRITE_FORMAT("CMceMainViewListView::HandleCommandL: aCommand: %d", aCommand);
    if ( !iMceViewActivated )
        {
        return;
        }

    switch (aCommand)
        {
        case EAknSoftkeyExit:
            iMceUi->HandleCommandL( EEikCmdExit );
            break;
        case EAknCmdOpen:
        case EMceCmdSelect:
            ChangeViewL();
            break;
        case EMceCmdConnect:
            {
            iFolderEntry->SetEntryL(KMsvRootIndexEntryIdValue);

            const TMsvEntry entry = iFolderEntry->ChildDataL( iMsgListContainer->CurrentItemId() );
            if ( entry.iType.iUid == KUidMsvServiceEntryValue )
                {
                iMceUi->GoOnlineL( entry.Id() );
                }
            }
            break;
        case EMceCmdCloseConnection:
            {
            iMceUi->CloseConnectionWithListQueryL();
            }
            break;
        case EAknCmdHideInBackground:
            break;
        case EMceCmdSettings:
		    {
		    if (( iEmailClientIntegration )&&(!iEmailFramework))
		        {
		        // these have to be updated here because when a mailbox setup wizard is launched
		        // from MCE Settings dialog this view's DoDeactivate will not be entered (because of 
		        // CAknInputBlock used used in wizard launching).
		        iSelectedIndex = iMsgListContainer->CurrentItemIndex();
		        iTopItemIndex = iMsgListContainer->ListBox()->TopItemIndex();

		        }
                        iMceUi->HandleCommandL( aCommand );
                        break;
                
		    }
        default:
        	if ( iAiwServiceHandler && FeatureManager::FeatureSupported(KFeatureIdSyncMlDsEmail) &&
        		 KAiwCmdSynchronize ==  iAiwServiceHandler->ServiceCmdByMenuCmd(aCommand) )
        		{
        		TInt appId = EGenericParamMessageItemEMail;
				const TMceListItem& tempItem = iMsgListContainer->CurrentItemListItem();
				TMsvEntry entry;
				TRAP_IGNORE( iMceUi->GetEntryL(tempItem.iMsvId,entry) );
        		CAiwGenericParamList* list = AiwSyncParamListLC( appId, entry.iDetails );
				iAiwServiceHandler->ExecuteMenuCmdL(aCommand, *list,
												iAiwServiceHandler->OutParamListL());
				CleanupStack::PopAndDestroy(list);
        		}
			else
				{
				iMceUi->HandleCommandL( aCommand );
				}
            break;
        }
    MCELOGGER_LEAVEFN("CMceMainViewListView::HandleCommandL()");
    }

// ----------------------------------------------------
// CMceMainViewListView::ProcessCommandL
// ----------------------------------------------------
void CMceMainViewListView::ProcessCommandL(TInt aCommand)
    {
    MCELOGGER_ENTERFN("CMceMainViewListView::ProcessCommandL");
    if ( !iMceUi->IsEditorOpen() && !iEmptyListContainer )
        {
        CAknView::ProcessCommandL( aCommand );
        }
#ifdef _DEBUG
    else
        {
        MCELOGGER_WRITE("CMceMainViewListView::ProcessCommandL: Editor is open so don't call CAknView::ProcessCommandL");
        }
#endif
    MCELOGGER_LEAVEFN("CMceMainViewListView::ProcessCommandL");
    }

// ----------------------------------------------------
// CMceMainViewListView::ChangeViewL
// ----------------------------------------------------
void CMceMainViewListView::ChangeViewL()
    {
    if ( iMceUi->IsEditorOpen() )
        {
        MCELOGGER_WRITE("CMceMainViewListView::ChangeViewL: do nothing because already editing one");
        return;
        }
    const TMceListItem& tempItem = iMsgListContainer->CurrentItemListItem();
    if ( !tempItem.iExtraItem )
        {
        TMsvId service;
        TMsvEntry child;
        User::LeaveIfError( iSession->GetEntry( tempItem.iMsvId, service, child ) );
        __ASSERT_DEBUG( (child.iType == KUidMsvFolderEntry || child.iType == KUidMsvServiceEntry), Panic( EMceMainViewCannotChangeView ) );
        if ( child.iType == KUidMsvFolderEntry )
            {
            iMceUi->OpenFolderViewL( child.Id() );
            iMceUi->ShowTabsL( child.Id() );
            }
        else // if ( child.iType == KUidMsvServiceEntry )
            {
            if ( iMceUi->IsSyncronizingL( child ) )
                {
                //cannot open during suncML mail syncronizing
                return;
                } 
            // Explanation: This part of code will launch Custom email application for 
            // configured mtm plugin. For the other accounts behavior will not change.

            // If MTM plugin ID matches the one in the repository we'll let Mtm plugin
            // launch the mailbox view
            if ( ( iEmailClientIntegration && (!iEmailFramework)) && ( iMtmPluginId != 0 ) &&
                    ( child.iMtm.iUid == iMtmPluginId ) )
                {
                iMceUi->OpenMtmMailboxViewL( child );
                }
            else
                {
                iMceUi->OpenRemoteMailboxViewL( child.Id() );
                }
            }

        }
    else if ( tempItem.iMsvId == KMceWriteMessageId )
        {
        iMceUi->LaunchUniEditorL();        
        }
    else if ( tempItem.iMsvId == KMceNoMailboxesListId )
        {
        CMuiuLock* repositoryLock = CMuiuLock::NewL( KCRUidMuiuVariation );
        CleanupStack::PushL( repositoryLock );
        TInt err = repositoryLock->Reserve();
        if ( !err )
            {
			if ( iEmailClientIntegration )
			    {
			    // LAUNCH S60 wizard (with confirmation query)
			    CAknQueryDialog* confDialog = CAknQueryDialog::NewL();
			    if ( confDialog->ExecuteLD( R_MCE_CREATE_MBOX_CONFIRMATION ) )
			        {
			        iMceUi->CreateNewAccountL( KSenduiMtmSmtpUid,
			                KMsvNullIndexEntryId );
			        }
	            // Release repository
        		repositoryLock->Release();
        		CleanupStack::PopAndDestroy( repositoryLock );
			    return;
			    } // iEmailClientIntegration
        CImumInternalApi* emailApiPtr = 
            iMsgListContainer->ListItems()->GetEmailApi();

        //Deny creating new mail account if TARM is active.
        if( !emailApiPtr->IsEmailFeatureSupportedL( 
            CImumInternalApi::EMailLockedSettings, R_QTN_SELEC_PROTECTED_SETTING ) )
            {
            CAknQueryDialog* confDialog = CAknQueryDialog::NewL();
            if ( confDialog->ExecuteLD( R_MCE_CREATE_MBOX_CONFIRMATION ) )
                {
                iMceUi->CreateNewAccountL( KSenduiMtmSmtpUid, KMsvNullIndexEntryId );
                }            
                }
            }
        else
            {
            // Show information note: General Settings has opened settings
            CAknNoteDialog* dlg = new (ELeave) CAknNoteDialog(
                CAknNoteDialog::ENoTone, CAknNoteDialog::ELongTimeout);
            dlg->ExecuteLD(R_MCE_NOTE_SETTINGS_OPEN_GS);
            }
                
        // Release repository
        repositoryLock->Release();
        CleanupStack::PopAndDestroy( repositoryLock );


        }
    else if ( tempItem.iMsvId == KMceDeliveryReportsId )
        {
        iSelectedIndex = iMsgListContainer->CurrentItemIndex();
        iTopItemIndex = iMsgListContainer->ListBox()->TopItemIndex();
        iSelectionFolderId = KMsvNullIndexEntryId;
        iMceUi->ActivateLocalViewL( KMceDeliveryReportViewId );
        iMceUi->ShowTabsL( 0, ETrue );
        // first parameter ignored when opening delivery reports view
        }
    else
        {
        StartAppL( tempItem.iMsvId );
        }
    }


// ----------------------------------------------------
// CMceMainViewListView::StartAppL
// ----------------------------------------------------
void CMceMainViewListView::StartAppL( TInt aCommandId )
    {
    HBufC* commandText = iMsgListContainer->ListItems()->ExtraItemCommandLC( aCommandId );
    TPtr tempText = commandText->Des();
    if ( tempText.Length() )
        {
        TLex lex( tempText );
        TUid uid;
        uid.iUid = 0;
        if ( lex.Val( uid.iUid ) == KErrNone && uid.iUid != 0 )
            {
            RWsSession wsServer;
            User::LeaveIfError( wsServer.Connect() );
            CleanupClosePushL( wsServer );
            TApaTaskList taskList( wsServer );
            TApaTask task(taskList.FindApp( uid ));
            if ( task.Exists() )
                {
                task.BringToForeground();   
                }
            else
                {
                TApaAppInfo appInfo;
                TApaAppCapabilityBuf capabilityBuf;
                RApaLsSession lsSession;
                User::LeaveIfError( lsSession.Connect() );
                CleanupClosePushL( lsSession );
                User::LeaveIfError(lsSession.GetAppInfo(appInfo, uid ));
                User::LeaveIfError(lsSession.GetAppCapability(capabilityBuf, uid ));
                TApaAppCapability& caps = capabilityBuf();

                TFileName appName = appInfo.iFullName;
                CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
                cmdLine->SetExecutableNameL(appName);
                
                if (caps.iLaunchInBackground)
                    {
                    cmdLine->SetCommandL(EApaCommandBackground);
                    }
                else
                    {
                    cmdLine->SetCommandL(EApaCommandRun);
                    }

                User::LeaveIfError(lsSession.StartApp(*cmdLine));
                
                CleanupStack::PopAndDestroy(cmdLine);
                CleanupStack::PopAndDestroy(); // lsSession   
                } // else
            CleanupStack::PopAndDestroy(); // wsServer   
            }
        else
            {
            CSchemeHandler* handler = CSchemeHandler::NewL( tempText );
            CleanupStack::PushL( handler );
            handler->HandleUrlStandaloneL();
            CleanupStack::PopAndDestroy( handler );
            }
        }
    CleanupStack::PopAndDestroy( commandText );
    }


// ----------------------------------------------------
// CMceMainViewListView::DynInitMenuPaneL
// ----------------------------------------------------
void CMceMainViewListView::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    if ( !iMceViewActivated )
        {
        return;
        }

	if ( FeatureManager::FeatureSupported(KFeatureIdSyncMlDsEmail) )
		{		
		if ( !iAiwServiceHandler )
		    {
		    iAiwServiceHandler = CAiwServiceHandler::NewL();
		    }
		if ( !iListViewFlags.MceFlag(EMceUiFlagsMainViewAiwCreated) )
		    {
		    iAiwServiceHandler->AttachMenuL( R_MCE_MAIN_VIEW_MENU, R_AIWMCE_INTEREST ); // execute this only once
		    iListViewFlags.SetMceFlag(EMceUiFlagsMainViewAiwCreated);
		    }
		if (iAiwServiceHandler->HandleSubmenuL(*aMenuPane))
			{
			return;
			}
		}

    const TMceListItem& tempItem = iMsgListContainer->CurrentItemListItem();
    if ( aResourceId == R_MCE_MAIN_VIEW_MENU  &&
        tempItem.iExtraItem && tempItem.iMsvId == KMceWriteMessageId )
        {
        aMenuPane->SetItemDimmed( EAknCmdOpen, ETrue );
        }
        
    if ( aResourceId == R_MCE_MAIN_VIEW_MENU )
    	{
    	aMenuPane->SetItemDimmed( EMceCmdAiwPlaceholder, ETrue );
    	if ( FeatureManager::FeatureSupported(KFeatureIdSyncMlDsEmail) )
    		{
    		TMsvEntry entry;
    		TRAP_IGNORE( iMceUi->GetEntryL(tempItem.iMsvId,entry) );
    		if ( entry.iMtm == KSenduiMtmSyncMLEmailUid )
    			{
    			// Focused item is a syncML mailbox, show the SyncML AIW plugin menu
    			aMenuPane->SetItemDimmed( EMceCmdAiwPlaceholder, EFalse );
				iAiwServiceHandler->InitializeMenuPaneL( *aMenuPane, aResourceId,
				EMceCmdFirstSyncmlAiwCommand, iAiwServiceHandler->InParamListL());
    			}
    		}
    	}
        
    iMceUi->DynInitMenuPaneL( aResourceId, aMenuPane );
    }


// ----------------------------------------------------
// CMceMainViewListView::HandleSessionEventL
// ----------------------------------------------------
void CMceMainViewListView::HandleSessionEventL(
    TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3)
    {
    MCELOGGER_WRITE_TIMESTAMP("CMceMainViewListView::HandleSessionEventL() start");
    switch (aEvent)
        {
    case EMsvEntriesChanged:
    case EMsvEntriesCreated:
    case EMsvEntriesDeleted:
        {
        TMsvId changedFolderId = (*(TMsvId*) (aArg2));
        if ( iMsgListContainer &&
             changedFolderId == KMsvRootIndexEntryIdValue
           )
            {
            // something changed to root, update main view list, if
            // the mailbox creation is not ongoing
            if ( !(iMceUi->MailboxCreationOn()) )
                {
                iMsgListContainer->ListItems()->HandleSessionEventL( aEvent, aArg1, aArg2, aArg3 );
		        iMsgListContainer->ListBox()->DrawDeferred();        
                }
            }

        CheckMainViewBitmapL( changedFolderId );
        break;
        }

    case EMsvEntriesMoved:
        {
        TMsvId changedFolderId = (*(TMsvId*) (aArg2));
        CheckMainViewBitmapL( changedFolderId );
        changedFolderId = (*(TMsvId*) (aArg3));
        CheckMainViewBitmapL( changedFolderId );
        break;
        }

    case EMsvCloseSession:
        {        
        iMsgListContainer->ListItems()->HandleSessionEventL( aEvent, aArg1, aArg2, aArg3 );
        iSession->RemoveObserver( *this );        
        }
        break;

    default:
        break;
        }
//    CMceMainViewListView::HandleSessionEventL( aEvent, aArg1, aArg2, aArg3 );
    MCELOGGER_WRITE_TIMESTAMP("CMceMainViewListView::HandleSessionEventL() end");

    }

// ----------------------------------------------------
// CMceMainViewListView::SetSelectedFolderId
// ----------------------------------------------------
void CMceMainViewListView::SetSelectedFolderId( TMsvId aFolderId )
    {
    iSelectionFolderId = aFolderId;
    iSelectedIndex = KErrNotFound;
    }

// ----------------------------------------------------
// CMceMainViewListView::SetDeliveryReportsSelected
// ----------------------------------------------------
void CMceMainViewListView::SetDeliveryReportsSelected( )
    {
    iSelectionFolderId = KMsvNullIndexEntryId;
    iSelectedIndex = iMsgListContainer->ListItems()->ExtraItemIndex( KMceDeliveryReportsListIdValue );
    // delivery reports always last!
    }

// ----------------------------------------------------
// CMceMainViewListView::HandleMsgServerStartupL
// ----------------------------------------------------
void CMceMainViewListView::HandleMsgServerStartupL()
    {
    iSession->AddObserverL( *this );
    iMsvSessionReady = ETrue;
    delete iFolderEntry;
    iFolderEntry = NULL;
    iFolderEntry = CMsvEntry::NewL(
        *iSession,
        KMsvRootIndexEntryIdValue,
        TMsvSelectionOrdering(
            KMsvGroupByStandardFolders, EMsvSortByDateReverse) );
            
    CreateContainerAndActivateL();
    
    TMsvId mailboxId = NULL; // the first mailbox id, in which there is unread mail
    TInt mailboxCount = 0; // the count of mailboxes, in which there are unread mail  
    
    if ( iPerformOperationAfterMsgStart == KMceShowMail )
        {
        iPerformOperationAfterMsgStart = 0;        
        mailboxCount = MailboxWithUnreadMessagesL( mailboxId );

        if ( mailboxId )
            {
             if ( mailboxCount == 1 )
                {
                //activate remote folder view using mailboxId
                ActivateViewL ( KMessagingCentreMessageViewUid, TUid::Uid( mailboxId ),KNullDesC8() );           
                }
            else if ( mailboxCount > 1 )
                {
                // launched from outside:
                // in several mailboxes unread mail, activate the first mailbox
                TInt mailboxIndex =  iMsgListContainer->ItemIndex( mailboxId );
                iMsgListContainer->ListBox()->SetCurrentItemIndex( mailboxIndex );
                iMsgListContainer->ListBox()->DrawDeferred();        
                }
            }
        }
	else if ( iPerformOperationAfterMsgStart ==  KMceShowAudioMessage )
        {
        iPerformOperationAfterMsgStart = 0;        
        //activate remote folder view using inboxId
        ActivateViewL ( KMessagingCentreMessageViewUid, TUid::Uid( KMsvGlobalInBoxIndexEntryIdValue ),KNullDesC8() );      
        }
    }


// ----------------------------------------------------
// CMceMainViewListView::ConstructMainViewL
// resets main view and constructs it again
// ----------------------------------------------------
void CMceMainViewListView::ConstructMainViewL()
    {    
    TInt origIndex = iMsgListContainer->CurrentItemIndex();
    TInt origCount = iMsgListContainer->ListItems()->Count();    
    TMsvId folderid =  iMsgListContainer->CurrentItemId();

    CMceMainViewListItemArray* listArray = iMsgListContainer->ListItems();
    listArray->Reset();
    listArray->ConstructMainViewL();
  
    TInt newIndex = KMceWriteMessageAddIndex;
    if ( origIndex == ( origCount - 1))
        {
        //delivery reports
        newIndex = listArray->Count() - 1;
        }
    else if ( origIndex == KMceWriteMessageAddIndex )
        {
        //new message
        newIndex = KMceWriteMessageAddIndex;
        }  
    else
        {
        //base folders and remote mailboxes
        newIndex = iMsgListContainer->ItemIndex( folderid );
        if ( newIndex < KMceWriteMessageAddIndex || newIndex > 
            ( listArray->Count() - 1 ) )
            {
            newIndex = KMceNoRemotemailboxesAddIndex;
            }
        }
    iMsgListContainer->ListBox()->SetCurrentItemIndex( newIndex );
    }

// ----------------------------------------------------
// CMceMainViewListView::CheckMainViewBitmapL
// ----------------------------------------------------
void CMceMainViewListView::CheckMainViewBitmapL( TMsvId aChangedFolderId ) const
    {
    TMsvId service;
    TMsvEntry child;
    if ( iSession->GetEntry( aChangedFolderId, service, child ) == KErrNone )
        {
        TInt changedIndex = KErrNotFound;
        if ( child.iMtm == KSenduiMtmImap4Uid )
            {
            changedIndex = iMsgListContainer->ItemIndex( service );
            }
        else if ( child.iMtm == KUidMsvLocalServiceMtm )
            {
            if ( child.Parent() == KMceDocumentsEntryId )
                {
                changedIndex = iMsgListContainer->ItemIndex( KMceDocumentsEntryId );
                }
            else
                {
                changedIndex = iMsgListContainer->ItemIndex( child.Id() );
                }
            }
        else
            {
            changedIndex = iMsgListContainer->ItemIndex( aChangedFolderId );
            }

        if ( changedIndex != KErrNotFound )
            {
            CMceMainViewListItemArray* array = static_cast<CMceMainViewListItemArray*>(iMsgListContainer->ListItems());
            array->ResetIconIndexL( changedIndex );
            }
        }
    }

// ----------------------------------------------------
// CMceMainViewListView::ResetBitmaps
// ----------------------------------------------------
void CMceMainViewListView::ResetBitmapsL( ) const
    {
    CMceMainViewListItemArray* array = static_cast<CMceMainViewListItemArray*>(iMsgListContainer->ListItems());
    array->StartListUpdateL();
    }

// ----------------------------------------------------
// CMceMainViewListView::SetSpecialMSKButtonL
// ----------------------------------------------------
void CMceMainViewListView::SetSpecialMSKButtonL(TInt aResourceId)
    { 
    if ( iDrawListbox )
        {
        CEikButtonGroupContainer* cba = Cba();
        cba->SetCommandL( KMSKPosition, aResourceId );
        cba->DrawDeferred();
        }
    }

// ----------------------------------------------------
// CMceMainViewListView::MailboxWithUnreadMessagesL
// ----------------------------------------------------
TInt CMceMainViewListView::MailboxWithUnreadMessagesL( TMsvId& aFirstMailboxId )
    {
    TInt mailboxCount = 0;
    aFirstMailboxId = 0;
    
    // launched from outside:
    // check mailboxes, also syncml mailboxes
    iFolderEntry->SetEntryL( KMsvRootIndexEntryIdValue );
    CMsvEntrySelection* sel=iFolderEntry->ChildrenWithTypeL( KUidMsvServiceEntry );
    
    CleanupStack::PushL( sel );
    const TMsvEntry* tentry=NULL;
    TBool read = EFalse;
 
    for ( TInt cc = 0; cc < sel->Count(); cc++ )
        {
        tentry=&( iFolderEntry->ChildDataL( (*sel)[cc] ) );
        if ( iMceUi->CheckTechnologyTypeFromMtm( KMailTechnologyTypeUid, tentry->iMtm ) )
            {
            TInt messageCount = 0;
            TInt unreadCount = 0;
            read = iBitmapResolver.HasUnreadMessagesL( tentry->Id(), messageCount, unreadCount );
            if ( read )
                {
                mailboxCount++; 
                if ( !aFirstMailboxId )
                    {                      
                    aFirstMailboxId = tentry->Id();
                    }
                }                
            }
        }

    CleanupStack::PopAndDestroy( sel );
    return mailboxCount;
    }

// ----------------------------------------------------
// CMceMainViewListView::MceListItemArrayChangedL
// ----------------------------------------------------
void CMceMainViewListView::MceListItemArrayChangedL()
    {
    if ( iMceViewActivated && iMsgListContainer )
        {
        CAknDoubleLargeStyleListBox* listBox = REINTERPRET_CAST( CAknDoubleLargeStyleListBox*,
            iMsgListContainer->ListBox() );
        if ( listBox )
            {
            listBox->DrawDeferred();
            }
        }
    }

// ----------------------------------------------------
// CMceMainViewListView::AiwSyncParamListLC
// ----------------------------------------------------
CAiwGenericParamList* CMceMainViewListView::AiwSyncParamListLC(TInt /*aId*/, const TDesC& aText)
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
// CMceMainViewListView::ListContainer
// ----------------------------------------------------
CMceMainViewListContainer* CMceMainViewListView::ListContainer() const
    {
    return iMsgListContainer;
    }
    
// ----------------------------------------------------
// CMceMainViewListView::HandleClientRectChange
// ----------------------------------------------------
void CMceMainViewListView::HandleClientRectChange()
    {
    if (iMsgListContainer)
        {
        iMsgListContainer->SetRect(ClientRect());
        }
    }

//  End of File
