/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*		folder subscription dialog
*
*/


// INCLUDES
#include <impicmds.h>
#include <imum.rsg>
#include <MtmStore.h>
#include <msvuids.h>
#include <mtudcbas.h>
#include <mtmuibas.h>
#include <miuthdr.h>
#include <imapcmds.h>
#include <gulicon.h>
#include <GULUTIL.H>
#include <EIKCLB.H>
#include <EIKCLBD.H>
#include <EIKTXLBM.H>
#include <AknLists.h>
#include <AknIconArray.h>                   // CAknIconArray
#include <StringLoader.h>
#include <eikmenup.h>
#include <FeatMgr.h>                        // FeatureManager
#include <hlplch.h>                         // For HlpLauncher
#include <muiu.mbg>
#include <coemain.h>
#include <muiuoperationwait.h>
#include <aknquerydialog.h>
#include <AknInputBlock.h>					// CAknInputBlock
#include <aknsutils.h>
#include <aknsconstants.h>                  // Skinned icon ids
#include <mtudcbas.h>
#include "imapfoldersubscription.h"
#include "ImumPanic.h"
#include "imas.hrh"
#include "EmailUtils.H"
#include <data_caging_path_literals.hrh>

#include <csxhelp/mbxs.hlp.hrh>

// CONSTANTS
// Correct path is added to literal when it is used.
_LIT(KMuiuBitmapFile, "z:muiu.MBM");
const TInt KImumFolderArrayVisibleItems = 5;
const TInt KImumFolderIconArrayGranularity = 6;
const TInt KImumITextLength = 100;


// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::NewL
// ----------------------------------------------------------------------------
CImapSubscriptionDialog* CImapSubscriptionDialog::NewL(
    TMsvId aServiceId,
    CMsvSession& aSession,
    TBool& aShouldExit )
	{
	CImapSubscriptionDialog* self=new(ELeave) CImapSubscriptionDialog(
        aServiceId,
        aSession,
        aShouldExit );

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::~CImapSubscriptionDialog
// ----------------------------------------------------------------------------
CImapSubscriptionDialog::~CImapSubscriptionDialog()
	{
	iSession.RemoveObserver(*this);
	if (iMtmStore)
		{
		iMtmStore->ReleaseMtmUi(KUidMsgTypeIMAP4);
		delete iMtmStore;
		}
	delete iContextEntry;
    delete iFolderListArray;
	}

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::CImapSubscriptionDialog
// ----------------------------------------------------------------------------
CImapSubscriptionDialog::CImapSubscriptionDialog(
    TMsvId aServiceId,
    CMsvSession& aSession,
    TBool& aShouldExit )
    :
    iServiceId(aServiceId),
	iSession(aSession),
    iShouldExit( aShouldExit )
	{
	}

    // ----------------------------------------------------------------------------
// CImapSubscriptionDialog::ConstructL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::ConstructL()
	{
    FeatureManager::InitializeLibL();

    CAknDialog::ConstructL( R_IMAS_SETTINGS_SUBSCRIBE_MENU );
    iSubscriptionsChanged = EFalse;

	iSession.AddObserverL(*this);
	iMtmStore=CMtmStore::NewL(iSession);

	iMtmUiData=&(iMtmStore->MtmUiDataL(KUidMsgTypeIMAP4));

	iMtmUi=&(iMtmStore->ClaimMtmUiL(KUidMsgTypeIMAP4));
	iMtmUi->BaseMtm().SwitchCurrentEntryL(iServiceId);

	TMsvSelectionOrdering selectionOrdering(KMsvGroupByType, EMsvSortByDetails, ETrue);
	iContextEntry=CMsvEntry::NewL(iSession, iServiceId, selectionOrdering);
    iFolderListArray = CImapFolderListArray::NewL( iSession, iServiceId );
    iOpenedFolderId = iServiceId;

	}

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::PreLayoutDynInitL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::PreLayoutDynInitL()
	{
    TBool refreshed = EFalse;
    CAknQueryDialog* confDialog = CAknQueryDialog::NewL();
	if( confDialog->ExecuteLD( R_IMUM_CONNECT_FOR_UPDATE ) )
        {
        refreshed = ETrue;
        CreateOperationL( ERefreshFolderList );
        }

    if( refreshed )
        {
        iFolderListArray->RefreshFolderListArrayL();
        }

    CEikColumnListBox* listBox =
        static_cast< CEikColumnListBox* >( Control( EImasCIDFolderListBox ) );

    listBox->SetListBoxObserver( this );

    // Create scroll bar
    listBox->CreateScrollBarFrameL();
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);


    CTextListBoxModel* model = listBox->Model();
    model->SetItemTextArray( iFolderListArray );
    listBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

    HBufC* emptyText =
        StringLoader::LoadLC( R_IMAS_SETTINGS_DIALOG_SUBS_EMPTY_TEXT );

    listBox->View()->SetListEmptyTextL( emptyText->Des() );

    CArrayPtrFlat<CGulIcon>* icons
        = new(ELeave) CAknIconArray( KImumFolderIconArrayGranularity );

    CleanupStack::PushL( icons );

    // create icon array
    TFileName fileName;

    TParse tp;
    tp.Set( KMuiuBitmapFile, &KDC_APP_BITMAP_DIR, NULL );
    fileName.Copy( tp.FullName() );

    AppendIconsL( fileName, icons );

    reinterpret_cast<CAknSingleGraphicStyleListBox*>( listBox )->
        ItemDrawer()->ColumnData()->SetIconArray( icons );


    CleanupStack::Pop( ); // icons
    CleanupStack::PopAndDestroy();// emptyText

	}

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::PostLayoutDynInitL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::PostLayoutDynInitL()
	{
    //nothing here
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::SynchroniseFoldersL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::SynchroniseFoldersL()
	{
	CreateOperationL(ERefreshFolderList);
	}

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::CreateOperationL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::CreateOperationL(TInt aButtonId)
	{
	CMsvEntrySelection* mySelection=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(mySelection);
	TBuf8<1> dummyParam;

    CMsvOperation* op = NULL;
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();

	switch(aButtonId)
		{
	case EFolderSubscribe:
	case EFolderUnsubscribe:
        {
        //check for child / parent folders for question "want to subscribe/unsub whole tree branch?"
        //eat softkey commands. If user would leave while subscription process in progress, it will crash.
        //Only for subscribe operations. We need to be able to cancel folder list update operation
        CAknInputBlock::NewLC();

        HandleSubscriptionL( aButtonId, *mySelection );

        TInt cnt = mySelection->Count();
        if( cnt > 1 )
            {
            //if more than one folder, we need to do a batch of subscribe commands
            op = iMtmUi->InvokeAsyncFunctionL( KIMAP4MTMStartBatch,
			*mySelection, wait->iStatus, dummyParam);
            wait->Start();
            delete op;
            op = NULL;

            TInt i;
            for( i=0; i<cnt; i++ )
                {
                op = iMtmUi->InvokeAsyncFunctionL(
			        (aButtonId==EFolderSubscribe?KIMAP4MTMLocalSubscribe:KIMAP4MTMLocalUnsubscribe),
			        *mySelection, wait->iStatus, dummyParam);
                wait->Start();
                delete op;
                op = NULL;


                if( mySelection->Count() > 1 )
                    {
                    //don't delete the last one, otherwise next invokeasyncfunction crashes, there has to
                    //be something in the passed entry selection.
                    mySelection->Delete(0);//always delete the first item, so next will be the first
                    }

                }
            op = iMtmUi->InvokeAsyncFunctionL( KIMAP4MTMEndBatch,
			*mySelection, wait->iStatus, dummyParam);
            }//if
        else
            {
            //for only selected folder, no batch needed
            op = iMtmUi->InvokeAsyncFunctionL(
			    (aButtonId==EFolderSubscribe?KIMAP4MTMLocalSubscribe:KIMAP4MTMLocalUnsubscribe),
			    *mySelection, wait->iStatus, dummyParam);
            }

        }
		break;

	case ERefreshFolderList:

        mySelection->AppendL(iServiceId);
		op = iMtmUi->InvokeAsyncFunctionL(
			KImpiMTMRefreshFolderList, *mySelection, wait->iStatus ,dummyParam);

		break;

	default:
		break;
		};

    wait->Start();
    delete op;
    op = NULL;


    if( aButtonId == EFolderSubscribe || aButtonId == EFolderUnsubscribe )
        {
        CleanupStack::PopAndDestroy(); //CAknInputBlock
        }

	CleanupStack::PopAndDestroy(2);	// CSI: 47 # wait, mySelection
	}

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::OkToExitL
// ----------------------------------------------------------------------------
TBool CImapSubscriptionDialog::OkToExitL(TInt aButtonId)
	{
	TBool exit=EFalse;
    CEikColumnListBox* listBox = 
        static_cast<CEikColumnListBox*>( Control( EImasCIDFolderListBox ) );
    const TInt oldCount = listBox->Model()->NumberOfItems();
	switch(aButtonId)
		{
        case EAknSoftkeyOptions:
            return CAknDialog::OkToExitL( EAknSoftkeyOptions );

		case EFolderSubscribe:
		case EFolderUnsubscribe:
            iSubscriptionsChanged = ETrue;
            CreateOperationL( aButtonId );
            break;

        case EAknSoftkeyClose:
            //invokaa folder update jos ollaan online, tai promptaa connectia?
            if( iSubscriptionsChanged )
                {
                //HandleRefreshL();
                }
            exit = ETrue;
            break;

        case EFolderOpenFolder:
            // open folder which has subfolders...
	        iContextEntry->SetEntryL(CurrentFolderL());
            if ( ContextHasChildFolders() )
                {
                iOpenedFolderId = CurrentFolderL();
                iFolderListArray->ChangeFolderL( CurrentFolderL() );
                if ( listBox->Model()->NumberOfItems() )
                    {
                    listBox->SetTopItemIndex( 0 );
                    listBox->SetCurrentItemIndex( 0 );
                    }
                if ( oldCount > listBox->Model()->NumberOfItems() )
                    {
                    listBox->HandleItemRemovalL();
                    }
                else
                    {
                    listBox->HandleItemAdditionL();
                    }
                }
            break;

        case EAknSoftkeyBack:
            {
            if( iContextEntry->Entry().iType == KUidMsvServiceEntry )
                {
                //Only exit if current folder is service
                exit = ETrue;
                break;
                }

            TMsvId folderToBeSelected = iOpenedFolderId;
	        iContextEntry->SetEntryL( iOpenedFolderId );

            iOpenedFolderId = iContextEntry->Entry().Parent();
            iFolderListArray->ChangeFolderL( iOpenedFolderId );
	        iContextEntry->SetEntryL( iOpenedFolderId );

            if ( oldCount > listBox->Model()->NumberOfItems() )
                {
                listBox->HandleItemRemovalL();
                }
            else
                {
                listBox->HandleItemAdditionL();
                }

            TInt loop;
            for ( loop = iContextEntry->Count()-1; loop >= 0; loop-- )
                {
                if ( folderToBeSelected == (*iContextEntry)[loop].Id() )
                    {
                    break;
                    }
                }
            if ( loop < 0 )
                {
                loop = 0;
                }
            listBox->SetTopItemIndex( ( loop > KImumFolderArrayVisibleItems ? loop - KImumFolderArrayVisibleItems : 0 ) );
            listBox->SetCurrentItemIndex( loop );
            }
            break;

        case EEikCmdExit:
            exit = ETrue;
            iShouldExit = ETrue;//pass exit to settings dialog
            break;

	default:
		break;
		};
	listBox->DrawNow();
	return exit;
	}


// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::ContextHasChildFolders
// ----------------------------------------------------------------------------
TBool CImapSubscriptionDialog::ContextHasChildFolders() const
	{
	TInt index=iContextEntry->Count()-1;
	while (index>=0 && (*iContextEntry)[index].iType!=KUidMsvFolderEntry)
		index--;

	if(index!=-1)
		return ETrue;

	return EFalse;
	}

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::HandleSessionEventL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	if(aEvent!=EMsvEntriesChanged)
		return;

    CMsvEntrySelection& sel=*static_cast<CMsvEntrySelection*>(aArg1);
	if(sel.Find(CurrentFolderL())!=KErrNotFound)
        {
		CEikColumnListBox* listBox = static_cast<CEikColumnListBox*>(Control(EImasCIDFolderListBox));
        listBox->DrawNow();
        }
	}


// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::DisplayFinalProgressDialog
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::DisplayFinalProgressDialog(CMsvOperation& aCompletedOperation) const
	{
	__ASSERT_DEBUG(!aCompletedOperation.IsActive(), Panic(EIMAP4MtmUiOperationStillActive));
	iMtmUi->DisplayProgressSummary(aCompletedOperation.FinalProgress());
	}

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::AppendIconsL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::AppendIconsL(
    TFileName& aFileName,
    CArrayPtrFlat<CGulIcon>* aIcons)
    {

    MAknsSkinInstance* skins = AknsUtils::SkinInstance();
    TAknsItemID id;
    CFbsBitmap* bitmap;
    CFbsBitmap* bitmapMask;
    TBool found = ETrue;


    for( TInt i=EMbmMuiuQgn_prop_folder_small; i<EMbmMuiuQgn_prop_folder_sub_unsubs_new+1; i++ )
        {
        CGulIcon* icon = NULL;
        found = ETrue;
        switch( i )
            {
            case EMbmMuiuQgn_prop_folder_small:
                id.Set( KAknsIIDQgnPropFolderSmall );
                break;
            case EMbmMuiuQgn_prop_folder_subs:
                id.Set( KAknsIIDQgnPropFolderSubs );
                break;
            case EMbmMuiuQgn_prop_folder_subs_new:
                id.Set( KAknsIIDQgnPropFolderSubsNew );
                break;
            case EMbmMuiuQgn_prop_folder_sub_small:
                id.Set( KAknsIIDQgnPropFolderSubSmall );
                break;
            case EMbmMuiuQgn_prop_folder_sub_subs:
                id.Set( KAknsIIDQgnPropFolderSubSubs );
                break;
            case EMbmMuiuQgn_prop_folder_sub_subs_new:
                id.Set( KAknsIIDQgnPropFolderSubSubsNew );
                break;
            case EMbmMuiuQgn_prop_folder_unsubs_new:
                id.Set( KAknsIIDQgnPropFolderUnsubsNew );
                break;
            case EMbmMuiuQgn_prop_folder_sub_unsubs_new:
                id.Set( KAknsIIDQgnPropFolderSubUnsubsNew );
                break;
            default:
                found = EFalse;
                break;
            }


        if( found )
            {
            AknsUtils::CreateIconL( skins, id, bitmap,
                bitmapMask, aFileName, i, i + 1 );

            icon = CGulIcon::NewL( bitmap, bitmapMask );

            // ownership of bitmap and mask transfered to icon
            CleanupStack::PushL( icon );
            aIcons->AppendL( icon );
            CleanupStack::Pop( icon ); // icon
            }
        }

    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::HandleListBoxEventL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemDoubleClicked:
            {
            TMsvId prevEntry = iContextEntry->Entry().Id();

            CEikColumnListBox* listBox =
                static_cast<CEikColumnListBox*>(
                    Control( EImasCIDFolderListBox ) );
            iContextEntry->SetEntryL( CurrentFolderL() );

            if ( listBox->Model()->NumberOfItems() &&
                 ContextHasChildFolders() )
                {
                //if has childs, open folder
                OkToExitL( EFolderOpenFolder );
                }
            else
                {
                //if not, change subs state of the folder
                const TMsvEmailEntry emailEntry( iContextEntry->Entry() );

                if ( MuiuEmailTools::IsInbox( iSession, emailEntry ) )
                    {
                    //inbox cannot be changed
                    iContextEntry->SetEntryL( prevEntry );
                    return;
                    }

                if ( emailEntry.LocalSubscription() )
                    {
                    iSubscriptionsChanged = ETrue;
        		    CreateOperationL( EFolderUnsubscribe );
                    }
                else
                    {
                    iSubscriptionsChanged = ETrue;
        		    CreateOperationL( EFolderSubscribe );
                    }
                listBox->DrawNow();
                iContextEntry->SetEntryL( prevEntry );
                }
            // Don't set prev entry here because we're opening it,
            // so it will be valid
            }

        break;
            default:
        break;
        }
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::CurrentFolderL
// ----------------------------------------------------------------------------
TMsvId CImapSubscriptionDialog::CurrentFolderL()
    {
    CEikColumnListBox* listBox = static_cast<CEikColumnListBox*>(Control(EImasCIDFolderListBox));
    TInt curIndex = listBox->CurrentItemIndex();

    return ( curIndex == KErrNotFound ? curIndex : iFolderListArray->Id( curIndex ) );
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::OfferKeyEventL
// ----------------------------------------------------------------------------
TKeyResponse CImapSubscriptionDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    return CAknDialog::OfferKeyEventL( aKeyEvent, aType );
    }



// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::DynInitMenuPaneL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    TMsvId folderId = CurrentFolderL();
    if ( folderId == KErrNotFound )
        {
        if ( aResourceId == R_IMAS_SETTINGS_SUBSCRIBE_MENUPANE )
            {
            aMenuPane->SetItemDimmed( EFolderOpenFolder, ETrue );
            aMenuPane->SetItemDimmed( EFolderSubscribe, ETrue );
            aMenuPane->SetItemDimmed( EFolderUnsubscribe, ETrue );
            }
        }
    else
        {
        TMsvId prevEntry = iContextEntry->Entry().Id();
        iContextEntry->SetEntryL( folderId );
	    const TMsvEmailEntry emailEntry( iContextEntry->Entry() );

        if ( aResourceId == R_IMAS_SETTINGS_SUBSCRIBE_MENUPANE )
            {
            if ( !ContextHasChildFolders() )
                {
                aMenuPane->SetItemDimmed( EFolderOpenFolder, ETrue );
                }
            if ( MuiuEmailTools::IsInbox( iSession, emailEntry ) )
                {
                //inbox subscription cannot be changed
                aMenuPane->SetItemDimmed( EFolderSubscribe, ETrue );
                aMenuPane->SetItemDimmed( EFolderUnsubscribe, ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed(
                    EFolderSubscribe, emailEntry.LocalSubscription() );
                aMenuPane->SetItemDimmed(
                    EFolderUnsubscribe, !emailEntry.LocalSubscription() );
                }
            aMenuPane->SetItemDimmed( EAknCmdHelp,
                !FeatureManager::FeatureSupported( KFeatureIdHelp ) );
            }

        iContextEntry->SetEntryL( prevEntry );
        }

    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::ProcessCommandL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::ProcessCommandL( TInt aCommandId )
    {
    if ( MenuShowing() )
        {
        HideMenu();
        }

    CAknDialog::ProcessCommandL(aCommandId);
    switch( aCommandId )
        {
        case EFolderOpenFolder:
            OkToExitL( EFolderOpenFolder );
            break;
        case EFolderSubscribe:
		case EFolderUnsubscribe:
            {
            TMsvEntry tEntry;
            TMsvId service;
            iSession.GetEntry( CurrentFolderL(), service, tEntry );
            const TMsvEmailEntry emailEntry( tEntry );


            iSubscriptionsChanged = ETrue;
            CreateOperationL( aCommandId );
            }
            break;
        case ERefreshFolderList:
            SynchroniseFoldersL();
            iFolderListArray->RefreshFolderListArrayL();
            break;

        case EAknCmdHelp:
            {
            LaunchHelpL();
            break;
            }

        case EEikCmdExit:
            TryExitL( aCommandId );
            break;
        default:
            iEikonEnv->EikAppUi()->HandleCommandL(aCommandId);
            break;
        }
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::LaunchHelpL
// Launch help using context
//
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::LaunchHelpL()
    {
    CCoeAppUi* appUi = static_cast<CCoeAppUi*>( ControlEnv()->AppUi() );
    HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), appUi->AppHelpContextL() );
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::GetHelpContext
// Returns helpcontext as aContext
//
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::GetHelpContext
        (TCoeHelpContext& aContext) const
    {
    const TUid KUidMce ={0x100058C5};
    aContext.iMajor = KUidMce;
    aContext.iContext = KMCE_HLP_SETT_MAILFOLDER_UPD;
    }                   

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::HandleSubscriptionL( TInt aSubType )
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::HandleSubscriptionL( TInt aSubType, CMsvEntrySelection& aSelection )
    {
    TMsvId current = CurrentFolderL();

    if( iFolderListArray->ContextHasChildren( current ) )
        {
        TMsvSelectionOrdering selectionOrdering(KMsvGroupByType, EMsvSortByDetails, ETrue);

        //CMsvEntry is used this way because GetContextChildrenL is recursive. Otherwise
        //CMsvEntry objects would be created several times before the first one is deleted
        //which consumes lots of memory. Now we just set new CMsvEntry over this one several times.
        CMsvEntry* entry = CMsvEntry::NewL(iSession, current, selectionOrdering);
        CleanupStack::PushL( entry );
        CMsvEntrySelection* sel = iFolderListArray->GetContextChildrenL( current, *entry );
        CleanupStack::PopAndDestroy();//entry

        CleanupStack::PushL( sel );

        TBool allFolders = CheckAreAllFoldersIncludedL( aSubType, sel );

        CAknQueryDialog* subsQuery = new(ELeave) CAknQueryDialog;

        switch( aSubType )
            {
            case EFolderSubscribe:
                {
                //subscribe also all sub folders?
                subsQuery->PrepareLC( R_IMUM_DO_SUBS_FOR_SUBFOLDERS );
                }
                break;
	        case EFolderUnsubscribe:
                {
                //unsubscribe also all sub folders?
                subsQuery->PrepareLC( R_IMUM_DO_UNSUBS_FOR_SUBFOLDERS );
                }
                break;
            default:
                User::Leave( KErrNotSupported );
                break;
            }//switch


        TInt ret = 1;//by default do all subfolders

        //if all subfolders already subbed / unsubbed, no need for query
        if( !allFolders )
            {
            ret = subsQuery->RunLD();
            }
        else
            {
            CleanupStack::PopAndDestroy();//subsQuery
            }

        if( ret )
            {
            //append all sub folders

            TInt count = sel->Count();

            //first append current
            aSelection.AppendL( current );

            //then the children
            TInt i;
            for( i=0; i<count; i++ )
                {
                aSelection.AppendL( sel->At(i) );
                }
            }
        else
            {
            //only selected
            aSelection.AppendL( current );
            }

        CleanupStack::PopAndDestroy();//sel
        }//if
    else
        {
        //no children, append only current
        aSelection.AppendL( current );
        }
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::CheckAreAllFoldersIncludedL
// ----------------------------------------------------------------------------
TBool CImapSubscriptionDialog::CheckAreAllFoldersIncludedL( TInt aSubType, CMsvEntrySelection* sel )
    {
    TMsvEntry tEntry;
    TMsvId service;

    TInt count = sel->Count();
    TInt i;
    TInt result = 0;

    for( i=0; i<count; i++ )
        {
        iSession.GetEntry( sel->At(i), service, tEntry );
        TMsvEmailEntry emailEntry( tEntry );

        if( aSubType == EFolderSubscribe )
            {
            //count all subscribed entries
            if( emailEntry.LocalSubscription() )
                {
                result++;
                }
            }
        else
            {
            //count all unsubscribed entries
            if( !emailEntry.LocalSubscription() )
                {
                result++;
                }
            }
        }

    if( count == result )
        {
        return ETrue;
        }

    return EFalse;

    }

///////////////////////////////////////

// ----------------------------------------------------------------------------
// CImapFolderListArray::NewL
// ----------------------------------------------------------------------------
CImapFolderListArray* CImapFolderListArray::NewL(
    CImumInternalApi& aMailboxApi,
    TMsvId aMailboxId )
	{
	CImapFolderListArray* self=new (ELeave) CImapFolderListArray(
        aMsvSession, aMailboxId );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// CImapFolderListArray::~CImapFolderListArray
// ----------------------------------------------------------------------------
CImapFolderListArray::~CImapFolderListArray()
    {
    FeatureManager::UnInitializeLib();

    delete iText;
    delete iEntry;
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::ConstructL
// ----------------------------------------------------------------------------
void CImapFolderListArray::ConstructL()
    {
    RefreshFolderListArrayL();

    iText = HBufC::NewL( KImumITextLength );
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::CImapFolderListArray
// ----------------------------------------------------------------------------
CImapFolderListArray::CImapFolderListArray(
    CImumInternalApi& aMailboxApi,
    TMsvId aMailboxId)
    :
    iSession( aMsvSession ),
    iMailboxId( aMailboxId )
    {
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::MdcaPoint
// ----------------------------------------------------------------------------
TPtrC CImapFolderListArray::MdcaPoint( TInt aIndex ) const
    {
	const TMsvEmailEntry emailEntry( (*iEntry)[aIndex] );

    TPtr tempText = iText->Des();
    tempText.Zero();

    TInt iconIndex = 0;
    if ( ContextHasChildFolders( emailEntry.Id() ) )
        {
        iconIndex = ( emailEntry.LocalSubscription() ? EImumFolderSubfoldersSubbed : EImumFolderSubfoldersUnsubbed );
        }
    else
        {
        iconIndex = ( emailEntry.LocalSubscription() ? EImumFolderSingleSubbed : EImumFolderSingleUnsubbed );
        }
    tempText.AppendNum( iconIndex );
    tempText.Append( KColumnListSeparator );
    tempText.Append( emailEntry.iDetails.Left( KImasImailServiceNameLength ) );

    return tempText;
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::MdcaCount
// ----------------------------------------------------------------------------
TInt CImapFolderListArray::MdcaCount() const
    {
    return iCount;
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::Id
// ----------------------------------------------------------------------------
TMsvId CImapFolderListArray::Id( TInt aIndex )
    {
    return (*iEntry)[aIndex].Id();
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::ChangeFolderL
// ----------------------------------------------------------------------------
void CImapFolderListArray::ChangeFolderL( TMsvId aFolderId )
    {
    iEntry->SetEntryL( aFolderId );

    CMsvEntrySelection* sel = iEntry->ChildrenWithTypeL( KUidMsvFolderEntry );
    iCount = sel->Count();
    delete sel;
    sel = NULL;

    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::ContextHasChildren
// ----------------------------------------------------------------------------
TBool CImapFolderListArray::ContextHasChildren( TMsvId aId ) const
    {
    return ContextHasChildFolders( aId );
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::ContextHasChildFolders
// ----------------------------------------------------------------------------
TBool CImapFolderListArray::ContextHasChildFolders( TMsvId aId ) const
	{
    TBool hasFolders = EFalse;
    TRAPD( error, hasFolders = DoContextHasChildFoldersL( aId ) );
    if ( error != KErrNone )
        {
        return EFalse;
        }
    return hasFolders;
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::DoContextHasChildFoldersL
// ----------------------------------------------------------------------------
TBool CImapFolderListArray::DoContextHasChildFoldersL( TMsvId aId ) const
	{
	TMsvSelectionOrdering selectionOrdering(KMsvGroupByType, EMsvSortByDetails, ETrue);
	CMsvEntry* entry = CMsvEntry::NewL(iSession, aId, selectionOrdering);
    CleanupStack::PushL( entry );
	TInt index=entry->Count()-1;
	while (index>=0 && (*entry)[index].iType!=KUidMsvFolderEntry)
        {
		index--;
        }

    CleanupStack::PopAndDestroy( entry );

	if(index!=-1)
        {
		return ETrue;
        }

	return EFalse;
	}

// ----------------------------------------------------------------------------
// CImapFolderListArray::GetContextChildrenL
// ----------------------------------------------------------------------------
CMsvEntrySelection* CImapFolderListArray::GetContextChildrenL( TMsvId aId, CMsvEntry& aEntry ) const
    {
    TMsvSelectionOrdering selectionOrdering(KMsvGroupByType, EMsvSortByDetails, ETrue);
    aEntry.SetEntryL( aId );// = CMsvEntry::NewL(iSession, aId, selectionOrdering);
    aEntry.SetSortTypeL( selectionOrdering );
    CMsvEntrySelection* sel = aEntry.ChildrenWithTypeL( KUidMsvFolderEntry );
    CleanupStack::PushL( sel );

    TInt i;
    TInt cnt = sel->Count();

    for( i=0; i<cnt; i++ )
        {
        if( ContextHasChildren( sel->At(i) ) )
            {
            //Note! recursion
            CMsvEntrySelection* selection = GetContextChildrenL( sel->At(i), aEntry );
            CleanupStack::PushL( selection );
            TInt c = selection->Count();

            TInt j;
            for( j=0; j<c; j++ )
                {
                sel->AppendL( selection->At(j) );
                }
            CleanupStack::PopAndDestroy();//selection
            }
        }


    CleanupStack::Pop();//sel
    return sel;
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::FoldersUpdated
// ----------------------------------------------------------------------------
TBool CImapFolderListArray::FoldersUpdated() const
    {
    return ( iEntry->Count() > 0 ? ETrue : EFalse );
    }

// ----------------------------------------------------------------------------
// CImapFolderListArray::RefreshFolderListArrayL
// ----------------------------------------------------------------------------
void CImapFolderListArray::RefreshFolderListArrayL()
    {
    if( iEntry )
        {
        delete iEntry;
        iEntry = NULL;
        }

    iEntry = iSession.GetEntryL( iMailboxId );
  	const TMsvSelectionOrdering originalOrdering=iEntry->SortType();
	TMsvSelectionOrdering newOrdering=originalOrdering;
	newOrdering.SetShowInvisibleEntries(ETrue);
    newOrdering.SetGroupByType(ETrue);
    newOrdering.SetSorting( EMsvSortByDetails );
	iEntry->SetSortTypeL(newOrdering);

    CMsvEntrySelection* sel = iEntry->ChildrenWithTypeL( KUidMsvFolderEntry );
    iCount = sel->Count();
    delete sel;
    sel = NULL;
    }

// End of File
