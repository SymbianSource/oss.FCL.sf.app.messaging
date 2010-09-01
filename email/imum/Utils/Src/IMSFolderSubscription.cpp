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
*       folder subscription dialog
*
*/


// INCLUDES
#include <impicmds.h>
#include <ImumUtils.rsg>
#include <MTMStore.h>
#include <msvuids.h>
#include <mtudcbas.h>
#include <mtmuibas.h>
#include <miuthdr.h>
#include <imapcmds.h>
#include <gulicon.h>
#include <gulutil.h>
#include <eikclb.h>
#include <eikclbd.h>
#include <eiktxlbm.h>
#include <aknlists.h>
#include <AknIconArray.h>                   // CAknIconArray
#include <StringLoader.h>
#include <eikmenup.h>
#include <hlplch.h>                         // For HlpLauncher
#include <muiu.mbg>
#include <coemain.h>
#include <MuiuOperationWait.h>
#include <AknQueryDialog.h>
#include <akninputblock.h> 					// CAknInputBlock
#include <AknsUtils.h>
#include <AknsConstants.h>                  // Skinned icon ids
#include <mtudcbas.h>
#include "IMSFolderSubscription.h"
#include "ImumPanic.h"
#include "EmailUtils.H"
#include "EmailFeatureUtils.h"
#include "IMSSettingsDialog.h"              // EDialogLastFlag
#include "ImumInternalApiImpl.h"
#include "ImumInMailboxUtilitiesImpl.h"
#include "ImumInSettingsDataCollection.h"
#include "ImumInSettingsData.h"
#include <data_caging_path_literals.hrh>
#include "ImumUtilsLogging.h"
#include <csxhelp/mbxs.hlp.hrh>

#include <aknnavide.h>
#include "ImumNaviPaneFolderIndicator.h"

// CONSTANTS
enum TIFSFlags
    {
    EIFSPreviousFolderOpenVisible = EDialogLastFlag,
    EIFSPreviousSubscriptionUndimmed,
    EIFSSubscriptionsChanged,
    EIFSLastFlag
    };

// Correct path is added to literal when it is used.
_LIT(KMuiuBitmapFile, "z:muiu.MBM");
const TInt KImumFolderArrayVisibleItems = 5;
const TInt KImumFolderIconArrayGranularity = 6;

const TUint KImumUidValue = 0x100058EB;
const TUid KImumUid = { KImumUidValue };

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::NewL
// ----------------------------------------------------------------------------
CImapSubscriptionDialog* CImapSubscriptionDialog::NewL(
    CMuiuFlags& aFlags,
    TMsvId aServiceId,
    TMsvId aMailboxId,
    TBool& aLoginChanged,
    const TDesC8& aUserName,
    const TDesC8& aPassword,
    CImumInternalApiImpl& aMailboxApi )
    {
    IMUM_STATIC_CONTEXT( CImapSubscriptionDialog::NewL, 0, utils, KLogUi );

    CImapSubscriptionDialog* self =
        new ( ELeave ) CImapSubscriptionDialog( aFlags, aServiceId, aMailboxId,
            aLoginChanged, aUserName, aPassword, aMailboxApi );

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
    IMUM_CONTEXT( CImapSubscriptionDialog::~CImapSubscriptionDialog, 0, KLogUi );

    iMailboxApi.MsvSession().RemoveObserver(*this);
    if (iMtmStore)
        {
        iMtmStore->ReleaseMtmUi(KUidMsgTypeIMAP4);
        delete iMtmStore;
        }
    delete iContextEntry;
    delete iFolderListArray;
    delete iSoftkeys;
    iFolderIdArray.Reset();
    iSoftkeys = NULL;
    iUtils = NULL;


	if( iNaviPane )
		{
		iNaviPane->Pop( iFolderIndicator );
		}
    delete iFolderIndicator;
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::CImapSubscriptionDialog
// ----------------------------------------------------------------------------
CImapSubscriptionDialog::CImapSubscriptionDialog(
    CMuiuFlags& aFlags,
    TMsvId aServiceId,
    TMsvId aMailboxId,
    TBool& aLoginChanged,
    const TDesC8& aUserName,
    const TDesC8& aPassword,
    CImumInternalApiImpl& aMailboxApi )
    :
    iMailboxApi( aMailboxApi ),
    iFlags( aFlags ),
    iServiceId( aServiceId ),
    iMailboxId( aMailboxId ),
    iLoginChanged( aLoginChanged ),
    iUserName( aUserName ),
    iPassword( aPassword )
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::CImapSubscriptionDialog, 0, KLogUi );

    // If login info has not changed, it need not be saved before refreshing
    // folder list from server.
    iLoginSaved = !aLoginChanged;
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::ConstructL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::ConstructL()
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::ConstructL, 0, KLogUi );

    CAknDialog::ConstructL( R_IMAS_SETTINGS_SUBSCRIBE_MENU );
    iFlags.ClearFlag( EIFSSubscriptionsChanged );

    iMailboxApi.MsvSession().AddObserverL(*this);
    iMtmStore=CMtmStore::NewL(iMailboxApi.MsvSession());

    iMtmUiData=&(iMtmStore->MtmUiDataL(KUidMsgTypeIMAP4));

    iMtmUi=&(iMtmStore->ClaimMtmUiL(KUidMsgTypeIMAP4));
    iMtmUi->BaseMtm().SwitchCurrentEntryL(iServiceId);

    TMsvSelectionOrdering selectionOrdering(KMsvGroupByType, EMsvSortByDetails, ETrue);
    iContextEntry=CMsvEntry::NewL(iMailboxApi.MsvSession(), iServiceId, selectionOrdering);
    iFolderListArray = CImapFolderListArray::NewL( iMailboxApi.MsvSession(), iServiceId );
    iOpenedFolderId = iServiceId;
    iFolderIdArray.Append(iOpenedFolderId);
    iUtils = const_cast<MImumInMailboxUtilities*>(
        &iMailboxApi.MailboxUtilitiesL() );

    CEikStatusPane* sp = ( (CAknAppUi*)iEikonEnv->EikAppUi() )->StatusPane();
    iNaviPane = (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));

    CImumNaviPaneFolderIndicator* folder =
        CImumNaviPaneFolderIndicator::NewL( ETrue, 1 );

    CAknNavigationDecorator* tmp = CAknNavigationDecorator::NewL(
        iNaviPane, folder, CAknNavigationDecorator::ENotSpecified );

    CleanupStack::PushL( tmp );
    tmp->SetContainerWindowL( *iNaviPane );
    CleanupStack::Pop( tmp );
    tmp->MakeScrollButtonVisible( EFalse );
    iFolderIndicator = tmp;

    // draw the folder icons on navi pane
    // iDepth is 0 at this point, so only
    // the root icon is displayed
    SetFolderDepthL( iDepth );
    }

/******************************************************************************

    Init

******************************************************************************/

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::PreLayoutDynInitL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::PreLayoutDynInitL()
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::PreLayoutDynInitL, 0, KLogUi );

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
    IMUM_CONTEXT( CImapSubscriptionDialog::PostLayoutDynInitL, 0, KLogUi );

    iSoftkeys = CIMSSoftkeyControl::NewL( ButtonGroupContainer() );
    iSoftkeys->SetMSKLabelling( ETrue );
    SetMskL();
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::DynInitMenuPaneL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::DynInitMenuPaneL, 0, KLogUi );

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
            if ( iUtils->IsInbox( emailEntry ) )
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
                !iFlags.GF( EMailFeatureHelp ) );
            }

        iContextEntry->SetEntryL( prevEntry );
        }

    }

/******************************************************************************

    Events

******************************************************************************/

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::HandleSessionEventL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::HandleSessionEventL, 0, KLogUi );

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
// CImapSubscriptionDialog::OkToExitL
// ----------------------------------------------------------------------------
TBool CImapSubscriptionDialog::OkToExitL(TInt aButtonId)
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::OkToExitL, 0, KLogUi );

    TBool exit=EFalse;
    CEikColumnListBox* listBox = static_cast<CEikColumnListBox*>(Control(EImasCIDFolderListBox));
    const TInt oldCount = listBox->Model()->NumberOfItems();
    switch(aButtonId)
        {
        case EAknSoftkeyOptions:
            return CAknDialog::OkToExitL( EAknSoftkeyOptions );

        case EFolderSubscribe:
        case EFolderUnsubscribe:
            iFlags.SetFlag( EIFSSubscriptionsChanged );
            CreateOperationL( aButtonId );
            break;

        case EAknSoftkeyClose:
            exit = ETrue;
            break;

        case EFolderOpenFolder:
            // open folder which has subfolders...
            iContextEntry->SetEntryL( CurrentFolderL() );

            // set the folder depth to 1 more for the
            // navi pane folder icons
			iDepth++;
            if ( ContextHasChildFolders() )
                {
                iOpenedFolderId = CurrentFolderL();
                iFolderIdArray.Append(iOpenedFolderId);
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
            // set the folder depth to 1 less for the
            // navi pane folder icons
			iDepth--;
            if( iContextEntry->Entry().iType == KUidMsvServiceEntry )
                {
                //Only exit if current folder is service
                exit = ETrue;
                break;
                }

            TMsvId folderToBeSelected = iOpenedFolderId;
            iContextEntry->SetEntryL( iOpenedFolderId );

            iOpenedFolderId = iContextEntry->Entry().Parent();
            iFolderIdArray.Remove(iFolderIdArray.Count()-1);
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
            // Issue exit to the dialog
            iFlags.SetFlag( EImumSettingShouldExit );
            break;

    default:
        break;
        };

    // draw the folder icons to navi pane
    SetFolderDepthL( iDepth );

    listBox->DrawNow();
    return exit;
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::SetFolderDepthL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::SetFolderDepthL( TInt aDepth )
	{
	CImumNaviPaneFolderIndicator* findicator =
		static_cast<CImumNaviPaneFolderIndicator*>( iFolderIndicator->DecoratedControl() );

	findicator->SetFolderDepth( aDepth );
	iNaviPane->PushL( *iFolderIndicator );
	}


// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::HandleListBoxEventL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType )
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::HandleListBoxEventL, 0, KLogUi );

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

                if ( iUtils->IsInbox( emailEntry ) )
                    {
                    //inbox cannot be changed
                    iContextEntry->SetEntryL( prevEntry );
                    return;
                    }

                if ( emailEntry.LocalSubscription() )
                    {
                    iFlags.SetFlag( EIFSSubscriptionsChanged );
                    CreateOperationL( EFolderUnsubscribe );
                    }
                else
                    {
                    iFlags.SetFlag( EIFSSubscriptionsChanged );
                    CreateOperationL( EFolderSubscribe );
                    }
                listBox->DrawNow();
                iContextEntry->SetEntryL( prevEntry );
                }
            // Don't set prev entry here because we're opening it,
            // so it will be valid
            SetMskL();
            }
            break;
        case EEventItemClicked:
        case EEventItemDraggingActioned:
            SetMskL();
            break;
        default:
            break;
        }
    }


// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::OfferKeyEventL
// ----------------------------------------------------------------------------
TKeyResponse CImapSubscriptionDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::OfferKeyEventL, 0, KLogUi );

    // For key up events, update the middle softkey label
    if ( aType == EEventKeyUp )
        {
        SetMskL();
        }

    return CAknDialog::OfferKeyEventL( aKeyEvent, aType );
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::ProcessCommandL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::ProcessCommandL( TInt aCommandId )
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::ProcessCommandL, 0, KLogUi );

    if ( MenuShowing() )
        {
        HideMenu();
        }

    CAknDialog::ProcessCommandL(aCommandId);
    switch( aCommandId )
        {
        case EAknSoftkeyOk:
            HandleListBoxEventL( NULL, EEventEnterKeyPressed );
            break;

        case EFolderOpenFolder:
            OkToExitL( EFolderOpenFolder );
            break;
        case EFolderSubscribe:
        case EFolderUnsubscribe:
            {
            TMsvEntry tEntry;
            TMsvId service;
            iMailboxApi.MsvSession().GetEntry( CurrentFolderL(), service, tEntry );
            const TMsvEmailEntry emailEntry( tEntry );


            iFlags.SetFlag( EIFSSubscriptionsChanged );
            CreateOperationL( aCommandId );

            SetMskL();
            }
            break;
        case ERefreshFolderList:
            RefreshFolderListL();
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
// CImapSubscriptionDialog::ActivateL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::ActivateL()
    {
    CAknDialog::ActivateL();

    // Query user if folder list should be updated from server when "subscribe
    // folders" setting item is opened.
    CAknQueryDialog* confDialog = CAknQueryDialog::NewL();
    if( confDialog->ExecuteLD( R_IMUM_CONNECT_FOR_UPDATE ) )
        {
        RefreshFolderListL();
        }
    }

/******************************************************************************

    Tools

******************************************************************************/

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::SynchroniseFoldersL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::SynchroniseFoldersL()
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::SynchroniseFoldersL, 0, KLogUi );

    CreateOperationL(ERefreshFolderList);
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::CreateOperationL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::CreateOperationL(TInt aButtonId)
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::CreateOperationL, 0, KLogUi );

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
        CAknInputBlock::NewLC( );

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
        CleanupStack::PopAndDestroy();//CAknInputBlock
        }

    CleanupStack::PopAndDestroy(2); // CSI: 47 # wait, mySelection
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::ContextHasChildFolders
// ----------------------------------------------------------------------------
TBool CImapSubscriptionDialog::ContextHasChildFolders() const
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::ContextHasChildFolders, 0, KLogUi );

    TInt index=iContextEntry->Count()-1;
    while (index>=0 && (*iContextEntry)[index].iType!=KUidMsvFolderEntry)
        index--;

    if(index!=-1)
        return ETrue;

    return EFalse;
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::DisplayFinalProgressDialog
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::DisplayFinalProgressDialog(
    CMsvOperation& aCompletedOperation) const
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::DisplayFinalProgressDialog, 0, KLogUi );

    __ASSERT_DEBUG( !aCompletedOperation.IsActive(),
        User::Panic( KImumMtmUiPanic, EIMAP4MtmUiOperationStillActive ) );
    iMtmUi->DisplayProgressSummary(aCompletedOperation.FinalProgress());
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::AppendIconsL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::AppendIconsL(
    TFileName& aFileName,
    CArrayPtrFlat<CGulIcon>* aIcons)
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::AppendIconsL, 0, KLogUi );


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
// CImapSubscriptionDialog::CurrentFolderL
// ----------------------------------------------------------------------------
TMsvId CImapSubscriptionDialog::CurrentFolderL()
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::CurrentFolderL, 0, KLogUi );

    CEikColumnListBox* listBox = static_cast<CEikColumnListBox*>(Control(EImasCIDFolderListBox));
    TInt curIndex = listBox->CurrentItemIndex();

    return ( curIndex == KErrNotFound ? curIndex : iFolderListArray->Id( curIndex ) );
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::GetResourceForMiddlekeyL()
// ----------------------------------------------------------------------------
//
TInt CImapSubscriptionDialog::GetResourceForMiddlekeyL()
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::GetResourceForMiddlekeyL, 0, KLogUi );

    TInt resource = 0;
    TMsvId folderId = CurrentFolderL();

    if ( folderId == KErrNotFound )
        {
        resource = R_IMAS_MSK_EMPTY;
        }
    else
        {
        TMsvId prevEntry = iContextEntry->Entry().Id();
        iContextEntry->SetEntryL( folderId );
        const TMsvEmailEntry emailEntry( iContextEntry->Entry() );

        // First check for subfolders
        if ( ContextHasChildFolders() )
            {
            // Text "OPEN" when subfolders
            resource = R_IMAS_MSK_OPEN;
            }
        else if ( iUtils->IsInbox( emailEntry ) )
            {
            // No text for inbox
            resource = R_IMAS_MSK_EMPTY;
            }
        else if ( !emailEntry.LocalSubscription() )
            {
            resource = R_IMAS_MSK_MARK;
            }
        else
            {
            resource = R_IMAS_MSK_UNMARK;
            }

        iContextEntry->SetEntryL( prevEntry );
        }

    return resource;
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::LaunchHelpL
// Launch help using context
//
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::LaunchHelpL()
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::LaunchHelpL, 0, KLogUi );

    TCoeHelpContext context;
    context.iContext = KMCE_HLP_SETT_MAILFOLDER_UPD;
    context.iMajor = KImumUid;
    
    CArrayFixFlat<TCoeHelpContext>* array = new(ELeave) CArrayFixFlat<TCoeHelpContext>( 1 );
    CleanupStack::PushL ( array );
    array->AppendL( context );
    CleanupStack::Pop( array );
    HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), array );
       
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::HandleSubscriptionL( TInt aSubType )
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::HandleSubscriptionL( TInt aSubType, CMsvEntrySelection& aSelection )
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::HandleSubscriptionL, 0, KLogUi );

    TMsvId current = CurrentFolderL();

    if( iFolderListArray->ContextHasChildren( current ) )
        {
        TMsvSelectionOrdering selectionOrdering(KMsvGroupByType, EMsvSortByDetails, ETrue);

        //CMsvEntry is used this way because GetContextChildrenL is recursive. Otherwise
        //CMsvEntry objects would be created several times before the first one is deleted
        //which consumes lots of memory. Now we just set new CMsvEntry over this one several times.
        CMsvEntry* entry = CMsvEntry::NewL(iMailboxApi.MsvSession(), current, selectionOrdering);
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
    IMUM_CONTEXT( CImapSubscriptionDialog::CheckAreAllFoldersIncludedL, 0, KLogUi );

    TMsvEntry tEntry;
    TMsvId service;

    TInt count = sel->Count();
    TInt i;
    TInt result = 0;

    for( i=0; i<count; i++ )
        {
        iMailboxApi.MsvSession().GetEntry( sel->At(i), service, tEntry );
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

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::RefreshFolderListL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::RefreshFolderListL()
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::RefreshFolderListL, 0, KLogUi );

    // If user has modified username/password setting we need to save them
    // before connecting into IMAP server.
    if ( !iLoginSaved )
        {
        SetLoginInformationL();
        iLoginSaved = ETrue;
        }

    CEikColumnListBox* listBox =
            static_cast<CEikColumnListBox*>( Control(EImasCIDFolderListBox) );
    const TInt oldCount = listBox->Model()->NumberOfItems();

    //get new folder structure from server
    SynchroniseFoldersL();

    // Login into IMAP server took place. Inform caller of this dialog that
    // username/password may have changed.
    iLoginChanged = ETrue;

    iFolderListArray->RefreshFolderListArrayL();

    iOpenedFolderId = iServiceId;
    iContextEntry->SetEntryL( iOpenedFolderId );

    SetBackToOldFolderL();


    //Refresh listBox
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
    SetMskL();
    }


// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::SetBackToOldFolderL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::SetBackToOldFolderL()
    {
    IMUM_CONTEXT( CImapSubscriptionDialog::SetBackToOldFolderL, 0, KLogUi );

    CMsvEntrySelection* sel = NULL;

    //set nextFolder index to root
    TInt nextFolder = 0;
    //if count is greater than 1 user is in some subfolder
    const TInt count = iFolderIdArray.Count();

    //found == true if first index in iFolderIdArray is new root folder
    TBool found = iFolderIdArray[nextFolder] == iOpenedFolderId;

    //loop as long as we found iFolderIdArray indexes from new folder structure
    //or loop go thru whole iFolderIdArray. ++nextFolder set index to subfolder
    while ( found && ++nextFolder < count &&
            iContextEntry->Count() > 0 )
        {

        sel = iContextEntry->ChildrenL();

        //test that subfolder found from selection
        found = sel->Find( iFolderIdArray[nextFolder] ) != KErrNotFound;

        if ( found )
            {

            iContextEntry->SetEntryL( iFolderIdArray[nextFolder] );
            iOpenedFolderId = iContextEntry->EntryId();
            }

        delete sel;
        sel = NULL;
        }


    // nextFolder is now index to first
    // FolderId that not found in previous loop
    if ( nextFolder > 0 )
        {
        //update iFolderListArray to new opened folder
        iFolderListArray->ChangeFolderL( iOpenedFolderId );

        //remove indexes that are not valid anymore
        for ( TInt i = count; --i >= nextFolder; )
            {
            iFolderIdArray.Remove(i);
            }
        }
    // if we did not found any subfolder indexes,
    //set context to root and clear the array
    else
        {
        iFolderIdArray.Reset();
        iFolderIdArray.AppendL( iOpenedFolderId );
        }

    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::SetLoginInformationL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::SetLoginInformationL()
    {
    // Retrieve the settings of the mailbox
    const MImumInMailboxUtilities& utils = iMailboxApi.MailboxUtilitiesL();
    TMsvEntry entry = utils.GetMailboxEntryL( iMailboxId );

    CImumInSettingsData* accountsettings =
        iMailboxApi.MailboxServicesL().LoadMailboxSettingsL( iMailboxId );
    CleanupStack::PushL( accountsettings );
    // Incoming settings
    MImumInSettingsDataCollection* connection = &accountsettings->GetInSetL( 0 );

    // Set login data to settings
    User::LeaveIfError( connection->SetAttr(
        TImumDaSettings::EKeyUsername, iUserName ) );
    User::LeaveIfError( connection->SetAttr(
        TImumDaSettings::EKeyPassword, iPassword ) );
    User::LeaveIfError( connection->SetAttr(
        TImumInSettings::EKeyTemporaryUsername, EFalse ) );
    User::LeaveIfError( connection->SetAttr(
        TImumInSettings::EKeyTemporaryPassword, EFalse ) );

    // Save mailbox data
    iMailboxApi.MailboxServicesL().SaveMailboxSettingsL( *accountsettings );
    CleanupStack::PopAndDestroy( accountsettings );
    }

// ----------------------------------------------------------------------------
// CImapSubscriptionDialog::SetMskL
// ----------------------------------------------------------------------------
void CImapSubscriptionDialog::SetMskL()
    {
    iSoftkeys->UpdateMiddleSoftkeyLabelL( GetResourceForMiddlekeyL(),
        EAknSoftkeyOk, this );
    }

// End of File
