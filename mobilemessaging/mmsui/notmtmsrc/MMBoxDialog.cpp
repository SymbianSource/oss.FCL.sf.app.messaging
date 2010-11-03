/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the CMMBoxDialog class
*
*/



// INCLUDES
#include <eikmenup.h>
#include <aknViewAppUi.h>               // iAvkonViewAppUi
#include <AknWaitDialog.h>              // CAknWaitDialog
#include <eikprogi.h>                   // CEikProgressInfo
#include <AknQueryDialog.h>             // CAknQueryDialog
#include <aknlists.h>
#include <StringLoader.h>               // StringLoader
#include <akntitle.h>                   // CAknTitlePane
#include <AknIconArray.h>               // CAknIconArray
// message includes
#include <msvapi.h>                     // for CMsvSession
#include <msvstd.h>                     // SelectionOrder
#include <akninputblock.h>              // CAknInputBlock
#include <MuiuMsvProgressReporterOperation.h> // MsvProgressReporterOperation
#include <MuiuMsgEmbeddedEditorWatchingOperation.h> // CMsgEmbeddedEditorWatchingOperation
#include <data_caging_path_literals.hrh> 
#include <muiumsvuiserviceutilitiesinternal.h>

#include <MuiuOperationWait.h>          // CMuiuOperationWait
#include <csxhelp/mce.hlp.hrh>
#include <hlplch.h>                     // For HlpLauncher 
#include <bldvariant.hrh>
#include <featmgr.h>                    // FeatureManager
#include <muiu.mbg>                     // bitmaps
#include <msvids.h>                     // for KMsvInboxIndexEntryId
#include <mtclreg.h>
#include <AknsUtils.h>                  // For skinned stuff
#include <eikmenub.h>                   // For CEikMenuBar
#include <aknnotewrappers.h>            // CAknInformationNote
#include <mmsnotificationclient.h>      // For Notification Client MTM
#include <aknnavi.h>                    // For navipane
#include <aknnavide.h>                  // CAknNavigationDecorator
#include <akntabgrp.h>                  // CAknTabGroup
#include <aknenv.h>                     // CAknEnv

#include <NotUi.rsg>
#include "NotUi.hrh"                    // for EMsgControlIdMMBoxListBox
#include "MMBoxDialog.h"
#include "NotMtmUi.h"
#include "NotMtmUiData.h"
#include "MmsMtmConst.h"

//  LOCAL CONSTANTS AND MACROS
const TInt KMMBoxBitmapArrayGranularity = 4;
const TInt KMMBoxMaxStatusInfoLength = 256;
const TInt KDateSize            = 30;
const TInt KMMBoxDialogFetchOperationsGranularity = 1;
const TInt KLocalKilo = 1024;
const TUid KUidMsvMessageListSortOrder={0x10005359};

_LIT( KMMBoxMarkReplacement, "1");
#define IS_PHONE_OFF_LINEL() MsvUiServiceUtilitiesInternal::IsPhoneOfflineL()
//#if defined ( _DEBUG )
// Enables notifications view testing without mmbox (to some extent).
// Uses Inbox as notifications folder instead of mmbox folder.
//#define MMS_NOTIFICATIONS_TESTING_WO_MMBOX 1
//#endif


//
//
// CMMBoxDialog
//
//

// ----------------------------------------------------
// CMMBoxDialog::CMMBoxDialog
// ----------------------------------------------------
CMMBoxDialog::CMMBoxDialog( CNotMtmUi&  aUi, 
                            CNotMtmUiData&  aUiData, 
                            CMsvSession& aSession, 
                            TInt& aExitCode ):
    iUi( aUi ),
    iUiData( aUiData ),
    iClientMtm( NULL ),
    iSession( aSession ),
    iNaviDecorator( NULL ),
    iMMBoxFolderId( KMsvGlobalInBoxIndexEntryId ),
    iFetchOperations( KMMBoxDialogFetchOperationsGranularity ),
    iDeleteSelection( NULL ),
    iExitCode( aExitCode ),
    iFlags( 0 ),
    iOriginalOrder ( KMsvNoGrouping, EMsvSortByDateReverse, EFalse )
    {
    iExitCode = KErrNone;
    iMarkReplacementText.Copy( KMMBoxMarkReplacement );
    }

// ----------------------------------------------------
// CMMBoxDialog::~CMMBoxDialog
// ----------------------------------------------------
CMMBoxDialog::~CMMBoxDialog()
    {
    LOGTEXT(_L8("MMBoxDialog: Destructor "));
    iSession.RemoveObserver( *this );
    delete iClientMtm;
    delete iMtmReg;
    // do not delete iNavipane - it's just a pointer
    delete iOldTitleText;
    delete iDeleteOperation;
    delete iOpenOperation;
    for ( TInt cc=iFetchOperations.Count()-1; cc>=0; cc-- )
        {
        delete iFetchOperations[cc];
        iFetchOperations.Delete(cc);
        }    
    iFetchOperations.ResetAndDestroy();
    delete iDeleteSelection;
    delete iUpdateOperation;
    delete iWaitDialog;
    delete iMessages;
    }

// ----------------------------------------------------
// CMMBoxDialog::NewL
// ----------------------------------------------------
CMMBoxDialog* CMMBoxDialog::NewL(   CNotMtmUi&  aUi, 
                                    CNotMtmUiData&  aUiData, 
                                    CMsvSession& aSession, 
                                    TInt& aExitCode )
    {
    CMMBoxDialog* self= new(ELeave) CMMBoxDialog( aUi, 
        aUiData, aSession, aExitCode );
    CleanupStack::PushL(self);
    self->ConstructL( );
    CleanupStack::Pop( );
    return self;
    }

// ----------------------------------------------------
// CMMBoxDialog::ConstructL
// ----------------------------------------------------
void CMMBoxDialog::ConstructL( )
    {
    LOGTEXT(_L8("MMBoxDialog: Construct start"));
    CAknDialog::ConstructL( R_NOT_MMBOX_MENUBAR );
    LOGTEXT(_L8("MMBoxDialog: Construct 2"));
    iMessages = CMMBoxArray::NewL( iSession, iUiData );
    LOGTEXT(_L8("MMBoxDialog: Construct 3"));
    iSession.AddObserverL( *this );

    // Prepare FeatureManager, read values to members and uninitialize FeatureManager
    FeatureManager::InitializeLibL( );
    if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        iFlags |= EHelpSupported;
        }
    FeatureManager::UnInitializeLib( );

    CEikStatusPane *sp = iEikonEnv->AppUiFactory( )->StatusPane( );
      
    if ( sp )
        {
        iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
        }
    LOGTEXT(_L8("MMBoxDialog: Constructor end "));
    }

// ----------------------------------------------------
// CMMBoxDialog::PreLayoutDynInitL
// ----------------------------------------------------
void CMMBoxDialog::PreLayoutDynInitL( )
    {
    LOGTEXT(_L8("MMBoxDialog: PreLayoutDynInitL start"));
    CEikColumnListBox* listBox = ListBox( );
    listBox->SetListBoxObserver( this );

    // Create scroll bar
    LOGTEXT(_L8("MMBoxDialog: PreLayoutDynInitL 2"));
    listBox->CreateScrollBarFrameL( );
    LOGTEXT(_L8("MMBoxDialog: PreLayoutDynInitL 3"));
    CAknAppUiBase* appUi = static_cast<CAknAppUiBase*>( iEikonEnv->EikAppUi() );
    if (AknLayoutUtils::DefaultScrollBarType(appUi) == CEikScrollBarFrame::EDoubleSpan)
        {
        // For EDoubleSpan type scrollbar
        listBox->ScrollBarFrame( )->CreateDoubleSpanScrollBarsL(EFalse, EFalse); // non-window owning scrollbar
        listBox->ScrollBarFrame( )->SetTypeOfVScrollBar(CEikScrollBarFrame::EDoubleSpan);
        }
    else
        {
        listBox->ScrollBarFrame( )->SetTypeOfVScrollBar(CEikScrollBarFrame::EArrowHead);
        }
    listBox->ScrollBarFrame( )->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    // Set data
    listBox->Model( )->SetItemTextArray( iMessages );
    listBox->Model( )->SetOwnershipType( ELbmDoesNotOwnItemArray );

    LOGTEXT(_L8("MMBoxDialog: PreLayoutDynInitL 4"));
    HBufC* emptyText = StringLoader::LoadLC( 
        R_NOT_MMBOX_EMPTY_STRING, 
        iCoeEnv );
    listBox->View( )->SetListEmptyTextL( *emptyText );
    CleanupStack::PopAndDestroy( emptyText );

    LOGTEXT(_L8("MMBoxDialog: PreLayoutDynInitL 5"));
    StoreTitleTextAndNavipaneAndSetNewL( );
    LOGTEXT(_L8("MMBoxDialog: PreLayoutDynInitL 6"));

    iMtmReg = CClientMtmRegistry::NewL( iSession );
    LOGTEXT(_L8("MMBoxDialog: PreLayoutDynInitL 7"));
	iClientMtm = (CMmsNotificationClientMtm*)iMtmReg->NewMtmL( KUidMsgMMSNotification );
    LOGTEXT(_L8("MMBoxDialog: PreLayoutDynInitL 8"));

#ifndef MMS_NOTIFICATIONS_TESTING_WO_MMBOX
    iMMBoxFolderId = iClientMtm->GetMmboxFolderL( );
#else
    LOGTEXT(_L8("MMBoxDialog: MMS_NOTIFICATIONS_TESTING_WO_MMBOX on"));
#endif
    LOGTEXT(_L8("MMBoxDialog: PreLayoutDynInitL 9"));

    CArrayPtrFlat<CGulIcon>* icons 
        = new( ELeave ) CAknIconArray( 
        KMMBoxBitmapArrayGranularity ); 

    CleanupStack::PushL( icons );

    // create icon array
    TFileName fileName;
    fileName.Copy( KAvkonBitmapFile );
    LOGTEXT(_L8("MMBoxDialog: PreLayoutDynInitL Append bitmaps"));
    // this must be first because of SetItemMarkReplacement index.
    AppendSkinnedIconL( fileName, EMbmAvkonQgn_prop_checkbox_on, icons );
    AppendSkinnedIconL( fileName, EMbmAvkonQgn_prop_checkbox_off, icons );
    
    // priority icons
    AppendSkinnedIconL( fileName, EMbmAvkonQgn_indi_mce_priority_high, icons );
    AppendSkinnedIconL( fileName, EMbmAvkonQgn_indi_mce_priority_low, icons );
    LOGTEXT(_L8("MMBoxDialog: PreLayoutDynInitL Append bitmaps end"));

    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( iEikonEnv->AppUiFactory()->StatusPane()->ControlL(
            TUid::Uid(EEikStatusPaneUidNavi) ) );
    iNaviPane->PushDefaultL();

    REINTERPRET_CAST( CAknDoubleGraphicStyleListBox*, listBox)->
        ItemDrawer( )->FormattedCellData()->SetIconArrayL( icons );
    listBox->ItemDrawer( )->SetItemMarkReplacement( iMarkReplacementText );
    CleanupStack::Pop( icons ); // icons

    LOGTEXT(_L8("MMBoxDialog: PreLayoutDynInitL 10"));
    UpdateMMBoxArrayL( );
    LOGTEXT(_L8("MMBoxDialog: PreLayoutDynInitL 11"));
    UpdateNavipaneL( );
    LOGTEXT(_L8("MMBoxDialog: PreLayoutDynInitL end"));
    }

// ----------------------------------------------------
// CMMBoxDialog::PostLayoutDynInitL
// ----------------------------------------------------
void CMMBoxDialog::PostLayoutDynInitL()
    {
    LOGTEXT(_L8("MMBoxDialog: PostLayoutDynInitL start"));

    if ( IS_PHONE_OFF_LINEL() )
        {   // We are in offline at the moment -> just return
        return;
        }

    if( !iUi.CheckSettingsL( ) )
        { // AP is not valid -> just return 
        return;
        }
    
    TMmboxInfo info;

    TInt err=0;
    TInt ret=0;
    TRAP( err, ret = iClientMtm->MmboxInfoL( info ) );

#ifdef MMS_NOTIFICATIONS_TESTING_WO_MMBOX
    ret = 1;
    info.error = 0;
    info.date.HomeTime();
#endif

    if( err )
        {
        LOGTEXT(_L8("MMBoxDialog: PostLayoutDynInitL info leave"));
        return;
        }
    else if( ret )
        {
        LOGTEXT(_L8("MMBoxDialog: PostLayoutDynInitL info found"));
        TInt startUpdate;
        if( info.error )
            {
            LOGTEXT(_L8("MMBoxDialog: PostLayoutDynInitL info update was not success"));
	        HBufC* buf;		// a buffer for strings
            buf = 
                StringLoader::LoadL( R_NOT_MMBOX_LAST_UPDATE_ERROR, iCoeEnv );
            CleanupStack::PushL( buf );

            startUpdate = ShowConfirmationQueryL( *buf );

            CleanupStack::PopAndDestroy( buf );
            }
        else
            {
            LOGTEXT(_L8("MMBoxDialog: PostLayoutDynInitL info update was success last"));

            TTime date( info.date );

            // Date is GMT so add offset + dst
    	    TLocale locale;
            date += locale.UniversalTimeOffset();
    	    if (locale.QueryHomeHasDaylightSavingOn())          
    		    {
    		    TTimeIntervalHours daylightSaving(1);          
    		    date += daylightSaving;
    		    }

	        HBufC* buf;		// a buffer for strings
            HBufC* dateFormat = StringLoader::LoadLC( R_QTN_DATE_SHORT, iCoeEnv );
            TBuf<KDateSize> dateString;
            date.FormatL( dateString, *dateFormat );
            CleanupStack::PopAndDestroy(); //dateFormat

            dateFormat = StringLoader::LoadLC( R_QTN_TIME_USUAL, iCoeEnv );
            TBuf<KDateSize> timeString;
            date.FormatL( timeString, *dateFormat );
            CleanupStack::PopAndDestroy(); //dateFormat

            CDesC16ArrayFlat* array = new(ELeave) CDesC16ArrayFlat( 2 );
            CleanupStack::PushL( array );
            array->AppendL( timeString );
            array->AppendL( dateString );
            buf = 
                StringLoader::LoadL( R_NOT_MMBOX_TIMED_QUERY, *array, iCoeEnv );
            CleanupStack::PopAndDestroy( array );
            CleanupStack::PushL( buf );

            startUpdate = ShowConfirmationQueryL( *buf );

            CleanupStack::PopAndDestroy( buf );
            }
        if( !startUpdate )
            { // User did not accept the "Update?" query
            return;
            }
        }
    else
        {
        if( !ShowConfirmationQueryL( R_NOT_MMBOX_TIMELESS_QUERY ) )
            {
            return;
            }
        }

    // If we end up here, user confirmed the update
    LOGTEXT(_L8("MMBoxDialog: PostLayoutDynInitL 2"));
    UpdateListL( );
    LOGTEXT(_L8("MMBoxDialog: PostLayoutDynInitL end"));

    return;
    }

// ----------------------------------------------------
// CMMBoxDialog::AppendSkinnedIconL
// ----------------------------------------------------
void CMMBoxDialog::AppendSkinnedIconL(    
    TFileName& aFileName, 
    TInt aIconIndex, 
    CArrayPtrFlat<CGulIcon>* aIcons)
    {
    LOGTEXT(_L8("MMBoxDialog: AppendSkinnedIconL start"));
    TAknsItemID skinId;
    switch ( aIconIndex )
        {
        case EMbmAvkonQgn_prop_checkbox_off:
            {
            skinId.Set( KAknsIIDQgnPropCheckboxOff );
            break;
            }
        case EMbmAvkonQgn_prop_checkbox_on:
            {
            skinId.Set( KAknsIIDQgnPropCheckboxOn );
            break;
            }
        case EMbmAvkonQgn_indi_mce_priority_high:
            {
            skinId.Set( KAknsIIDQgnIndiMcePriorityHigh );
            break;
            }
        case EMbmAvkonQgn_indi_mce_priority_low:
            {
            skinId.Set( KAknsIIDQgnIndiMcePriorityLow );
            break;
            }
        default:
            {
            User::Leave( KErrNotFound );
            break;
            }
        }

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    LOGTEXT(_L8("MMBoxDialog: AppendSkinnedIconL 2"));
    AknsUtils::CreateIconL(
        AknsUtils::SkinInstance(),
        skinId,
        bitmap,
        mask,
        aFileName,
        aIconIndex,
        aIconIndex + 1 ); //mask
    LOGTEXT(_L8("MMBoxDialog: AppendSkinnedIconL 3"));

    CleanupStack::PushL( bitmap );
    CleanupStack::PushL( mask );        

    CGulIcon* icon = NULL;
    icon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( mask );
    CleanupStack::Pop( bitmap );
    CleanupStack::PushL( icon );
    aIcons->AppendL( icon );
    CleanupStack::Pop( icon ); // icon    
    LOGTEXT(_L8("MMBoxDialog: AppendSkinnedIconL end"));
    }

// ----------------------------------------------------
// CMMBoxDialog::OkToExitL
// ----------------------------------------------------
TBool CMMBoxDialog::OkToExitL(TInt aButtonId)
    {
    LOGTEXT(_L8("MMBoxDialog: OkToExitL start"));
    CAknInputBlock::NewLC(  ); // Absorb keys
    TBool okToExit = CAknDialog::OkToExitL( aButtonId );

    if ( okToExit )
        {
        LOGTEXT(_L8("MMBoxDialog: OkToExitL RestoreTitleText.."));
        TRAP_IGNORE( RestoreTitleTextAndNavipaneL() );
        }

    CleanupStack::PopAndDestroy(); // CAknInputBlock

    if( iExitCode == EEikCmdExit )
        {
        LOGTEXT(_L8("MMBoxDialog: OkToExitL RunAppShutter"));
        CAknEnv::RunAppShutter();
        }

    LOGTEXT(_L8("MMBoxDialog: OkToExitL end"));
    return okToExit;
    }

// ----------------------------------------------------
// CMMBoxDialog::ProcessCommandL
// ----------------------------------------------------
void CMMBoxDialog::ProcessCommandL( TInt aCommandId )
    {
    LOGTEXT(_L8("MMBoxDialog: ProcessCommand start"));
    CAknDialog::ProcessCommandL( aCommandId ) ;

    switch ( aCommandId )
        {
        case EMMBoxOpen:
            LOGTEXT(_L8("MMBoxDialog: ProcessCommand OpenNotification start"));
            OpenNotificationL( );
            break;
        case EMMBoxRetrieveSelected:
            LOGTEXT(_L8("MMBoxDialog: ProcessCommand RetrieveL start"));
            FetchL( );
            break;
        case EMMBoxRetrieveAll:
            LOGTEXT(_L8("MMBoxDialog: ProcessCommand RetrieveAll start"));
            FetchL( ETrue );
            break;
        case EMMBoxDeleteSelected:
            LOGTEXT(_L8("MMBoxDialog: ProcessCommand Delete start"));
            DeleteL( );
            break;
        case EMMBoxDeleteAll:
            LOGTEXT(_L8("MMBoxDialog: ProcessCommand DeleteAll start"));
            DeleteL( ETrue );
            break;
        case EMMBoxUpdateList:
            LOGTEXT(_L8("MMBoxDialog: ProcessCommand UpdateList start"));
            UpdateListL( );
            break;
        case EMMBoxViewStatus:
            LOGTEXT(_L8("MMBoxDialog: ProcessCommand ViewStatus start"));
            ShowStatusL( );
            break;
        case EMMBoxSortMessagesByDate:
            LOGTEXT(_L8("MMBoxDialog: ProcessCommand SortByDate start"));
            SortMessagesByL( EMMBoxSortByDate );
            break;
        case EMMBoxSortMessagesBySender:
            LOGTEXT(_L8("MMBoxDialog: ProcessCommand SortBySender start"));
            SortMessagesByL( EMMBoxSortByName );
            break;
        case EMMBoxSortMessagesBySubject:
            LOGTEXT(_L8("MMBoxDialog: ProcessCommand SortBySubject start"));
            SortMessagesByL( EMMBoxSortBySubject );
            break;
        case EAknCmdHelp:
            LOGTEXT(_L8("MMBoxDialog: ProcessCommand Help start"));
            LaunchHelpL();                
            break;
        case EAknCmdExit:
            {
            LOGTEXT(_L8("MMBoxDialog: ProcessCommand Exit start"));
            iExitCode = EEikCmdExit;
            TryExitL( EAknCmdExit );
            }
            break;

        default :
            LOGTEXT(_L8("MMBoxDialog: ProcessCommand default break"));
            break;
        }
    }

// ----------------------------------------------------
// CMMBoxDialog::DynInitMenuPaneL
// ----------------------------------------------------
void CMMBoxDialog::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    LOGTEXT(_L8("MMBoxDialog: ProcessCommand DynInitMenuPaneL start"));
    if ( aResourceId == R_NOT_MMBOX_MENUPANE )
        {
        LOGTEXT(_L8("MMBoxDialog: ProcessCommand DynInitMenuPaneL menupane"));
        if( !( iFlags & EHelpSupported ) )
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
            }

        TMmboxInfo info;
        TInt ret = 0;
        TRAPD( err, ret = iClientMtm->MmboxInfoL( info ) );

#ifdef MMS_NOTIFICATIONS_TESTING_WO_MMBOX
        ret = 1;
#endif

        if( err || !ret )
            {// No info available
            aMenuPane->SetItemDimmed( EMMBoxViewStatus, ETrue ); 
            }

        if ( ListBox()->Model()->NumberOfItems() == 0 )
            {
            aMenuPane->SetItemDimmed( EMMBoxOpen, ETrue ); 
            aMenuPane->SetItemDimmed( EMMBoxRetrieveMenu, ETrue ); 
            aMenuPane->SetItemDimmed( EMMBoxDeleteMenu, ETrue ); 
            }
        else
            {
            TMsvEntry entry = 
                iMessages->Entry( ListBox( )->CurrentItemIndex( ) );

            if( ListBox()->Model()->NumberOfItems() == 1 )
                { // Operation support is only checked if there's only one entry
                  // Otherwise we let the user try to start the functions..
                if( iUiData.OperationSupportedL( KMtmUiFunctionFetchMMS, entry ) )
                    {
                    aMenuPane->SetItemDimmed( EMMBoxRetrieveMenu, ETrue ); 
                    }
                if( iUiData.OperationSupportedL( KMtmUiFunctionDeleteMessage, entry ) )
                    {
                    aMenuPane->SetItemDimmed( EMMBoxDeleteMenu, ETrue ); 
                    }
                }

            if( iUiData.OperationSupportedL( KMtmUiFunctionOpenMessage, entry ) )
                {
                aMenuPane->SetItemDimmed( EMMBoxOpen, ETrue ); 
                }
            }
        LOGTEXT(_L8("MMBoxDialog: ProcessCommand DynInitMenuPaneL end"));
        }
    }

// ----------------------------------------------------
// CMMBoxDialog::OfferKeyEventL
// ----------------------------------------------------
TKeyResponse CMMBoxDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {
    LOGTEXT(_L8("MMBoxDialog: ProcessCommand OfferKeyEvent"));
    CEikColumnListBox* listBox = ListBox();
    if ( listBox && aType == EEventKey && ( iUpdateOperation || iDeleteOperation ) )
        {   // When an operation is going on, we do not need keypresses
        LOGTEXT(_L8("MMBoxDialog: ProcessCommand OfferKeyEvent operation in progress"));
        return EKeyWasNotConsumed;
        }
    if ( listBox && aType == EEventKey && aKeyEvent.iCode == EKeyBackspace &&
         listBox->Model()->NumberOfItems() > 0 )
        { // C-key
        LOGTEXT(_L8("MMBoxDialog: ProcessCommand OfferKeyEvent Delete"));
        DeleteL( );
        return EKeyWasConsumed;
        }
    else if ( listBox && aType == EEventKey && aKeyEvent.iCode == EKeyOK )
        {   // Selection Key here!!!
        LOGTEXT(_L8("MMBoxDialog: ProcessCommand OfferKeyEvent Selection key"));
        listBox->View()->ToggleItemL( listBox->CurrentItemIndex() );
        return EKeyWasConsumed;
        }
    else if( aType == EEventKey && aKeyEvent.iCode == EKeyEscape )
        {
        if( !OkToExitL( -1 ) )
            {
            return EKeyWasConsumed;
            }
        }

    return CAknDialog::OfferKeyEventL( aKeyEvent, aType );
    }

// ----------------------------------------------------
// CMMBoxDialog::HandleListBoxEventL
// ----------------------------------------------------
void CMMBoxDialog::HandleListBoxEventL(
    CEikListBox* /*aListBox*/, 
    TListBoxEvent /*aEventType*/ )
    {
    }

// ----------------------------------------------------
// CMMBoxDialog::UpdateMMBoxArrayL
// ----------------------------------------------------
void CMMBoxDialog::UpdateMMBoxArrayL()
    {
    LOGTEXT(_L8("MMBoxDialog: UpdateMMBoxArray start"));

    iMessages->RefreshArrayL( iMMBoxFolderId );

    //	TInt newIndex = 
	ListBox()->ScrollToMakeItemVisible( ListBox()->CurrentItemIndex( ) );
    ListBox()->DrawNow( );
    
    SetMSKL();

    LOGTEXT(_L8("MMBoxDialog: UpdateMMBoxArray end"));
    }

// ----------------------------------------------------
// CMMBoxDialog::ListBox
// ----------------------------------------------------
CEikColumnListBox* CMMBoxDialog::ListBox() const
    {
    return((CEikColumnListBox*)Control(EMsgControlIdMMBoxListBox));
    }

// ----------------------------------------------------
// CMMBoxDialog::StoreTitleTextAndNavipaneAndSetNewL
// ----------------------------------------------------
void CMMBoxDialog::StoreTitleTextAndNavipaneAndSetNewL()
    {
    LOGTEXT(_L8("MMBoxDialog: StoreTitleTextAndNavipane.. start"));
    CEikStatusPane* sp = iEikonEnv->AppUiFactory( )->StatusPane( );
    // Fetch pointer to the default title pane control
    CAknTitlePane* title=(CAknTitlePane *)sp->ControlL(
        TUid::Uid(EEikStatusPaneUidTitle));

    delete iOldTitleText;
    iOldTitleText = NULL;
    iOldTitleText = HBufC::NewL( title->Text()->Length() + 1 );
    TPtr tempText = iOldTitleText->Des();
    tempText.Copy( (*title->Text()) );

    HBufC* newTitle = StringLoader::LoadLC( 
        R_NOT_MMBOX_TITLE_TEXT, 
        iCoeEnv );
    title->SetTextL( *newTitle );
    CleanupStack::PopAndDestroy( newTitle );

    iNaviOld = iNaviPane->Top( );

    CMsvEntry* entry=iSession.GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL( entry );
    if ( entry->HasStoreL() )
        {
        CMsvStore* store = entry->ReadStoreL();
        CleanupStack::PushL( store );
        if ( store->IsPresentL( KUidMsvMessageListSortOrder ) )
            { // Read it first
            RMsvReadStream readStream;
            readStream.OpenLC( *store, KUidMsvMessageListSortOrder );
            iOriginalOrder.InternalizeL( readStream );
            CleanupStack::PopAndDestroy( ); // readStream
            // Then remove it
/*            CMsvStore* store2 = entry->EditStoreL();
            CleanupStack::PushL( store2 );
            store->Remove(KUidMsvMessageListSortOrder);
            store->CommitL();
            CleanupStack::PopAndDestroy( store2 ); // store2
  */
            }
        CleanupStack::PopAndDestroy( store ); // store
        }
    CleanupStack::PopAndDestroy( entry );
    LOGTEXT(_L8("MMBoxDialog: StoreTitleTextAndNavipane.. end"));
    }

// ----------------------------------------------------
// CMMBoxDialog::RestoreTitleTextL
// ----------------------------------------------------
void CMMBoxDialog::RestoreTitleTextAndNavipaneL()
    {
    LOGTEXT(_L8("MMBoxDialog: RestoreTitleTextAndNavipane.. start"));
    CEikStatusPane* sp = iEikonEnv->AppUiFactory( )->StatusPane( );
    // Fetch pointer to the default title pane control
    CAknTitlePane* title=(CAknTitlePane *)sp->ControlL(
        TUid::Uid(EEikStatusPaneUidTitle));

    title->SetTextL( iOldTitleText->Des() );
    delete iOldTitleText;
    iOldTitleText = NULL;

    iNaviPane->Pop( iNaviDecorator );
    delete iNaviDecorator;
    iNaviDecorator = NULL;

    if( iNaviOld )
        {
        iNaviPane->PushL( *iNaviOld );
        }
    iNaviOld = NULL;

    if( iFlags & ESortTypeChanged )
        {
        // Remove the one set by MMBoxList
        CMsvEntry* entry=iSession.GetEntryL( KMsvRootIndexEntryId );
        CleanupStack::PushL( entry );
        CMsvStore* store = entry->EditStoreL();
        CleanupStack::PushL( store );
        store->Remove(KUidMsvMessageListSortOrder);
        store->CommitL();
        CleanupStack::PopAndDestroy( 2 ); // store, entry

        // If the original sory type was not the default one -> write it..
        TMsvSelectionOrdering def( KMsvNoGrouping, EMsvSortByDateReverse, EFalse );
        if( !(iOriginalOrder == def) )
            {
            CMsvEntry* entry=iSession.GetEntryL( KMsvRootIndexEntryId );
            CleanupStack::PushL( entry );
 	        CMsvStore* store = entry->EditStoreL();
 	        CleanupStack::PushL( store );
 	        RMsvWriteStream writeStream;
 	        writeStream.AssignLC( *store, KUidMsvMessageListSortOrder );
 	        iOriginalOrder.ExternalizeL( writeStream );
 	        writeStream.CommitL();
 	        store->CommitL();
 	        CleanupStack::PopAndDestroy( 3 ); // store, writestream, entry
            }   
        }    
    LOGTEXT(_L8("MMBoxDialog: RestoreTitleTextAndNavipane.. end"));
    }

// ----------------------------------------------------
// CMMBoxDialog::OpCompleted
// ----------------------------------------------------
void CMMBoxDialog::OpCompleted(
    CMsvSingleOpWatcher& aOpWatcher,
    TInt aCompletionCode )
    {
    LOGTEXT(_L8("MMBoxDialog: OpCompleted start"));
    TBool tryToExit = EFalse;
    CMsvOperation* op=&aOpWatcher.Operation();
    TRAP_IGNORE( tryToExit = DoOperationCompletedL(op, aCompletionCode ));  
    if ( tryToExit )
        {
        LOGTEXT(_L8("MMBoxDialog: OpCompleted tryToExit"));
        return;
        }

    if ( iDeleteOperation && iDeleteOperation->Operation().Id()==op->Id() )
        {
        LOGTEXT(_L8("MMBoxDialog: OpCompleted DeleteOperation"));
        TInt error = op->iStatus.Int( );
        // Check the entries one by one
        TInt failedOnes = 0;
        TInt totalOnes = 0;
        if( iDeleteSelection )
            { // This means user did not answer "No" to "Delete message" question
            for( TInt loop = 0; loop < iDeleteSelection->Count( ); loop++ )
                {
                TMsvEntry entry;
                TMsvId service = -1;
                totalOnes++;            
                if ( iSession.GetEntry(iDeleteSelection->At(loop), service, entry ) != KErrNotFound )
                	{
                    LOGTEXT(_L8("MMBoxDialog: OpCompleted DeleteOp Entry found after delete"));
                    failedOnes++;
                	}
                }
            }
        if( error || failedOnes )
            {
            LOGTEXT2(_L16("MMBoxDialog: OpCompleted DeleteOperation Error: %d"), error);
            if( failedOnes == 1)
                {
                LOGTEXT(_L8("MMBoxDialog: OpCompleted DeleteOp 1 failed"));
                TRAP_IGNORE( ShowConfirmationQueryL( R_NOT_MMBOX_INFO_DELETE_FAILED, ETrue ) );
                }
            else
                {
                LOGTEXT(_L8("MMBoxDialog: OpCompleted DeleteOp many failed"));
                TRAP_IGNORE ( ShowConfirmationQueryL(   failedOnes,
                                                        totalOnes,                          
                                                        R_NOT_MMBOX_INFO_DELETE_FAILED_MANY ) );
                }
            }
        delete iDeleteSelection;
        iDeleteSelection = NULL;
        delete iDeleteOperation;
        iDeleteOperation = NULL;
        }

    else if ( iOpenOperation && iOpenOperation->Operation().Id()==op->Id() )
        {
        if( !( iFlags & EDoNotDeleteOpen ) )
            {
            LOGTEXT(_L8("MMBoxDialog: OpCompleted OpenOperation"));
            delete iOpenOperation;
            iOpenOperation = NULL;
            }
        iFlags &= ~EDoNotDeleteOpen;    // Reset this anyway
        }

    else if ( iUpdateOperation && iUpdateOperation->Operation().Id()==op->Id() )
        {
        LOGTEXT(_L8("MMBoxDialog: OpCompleted UpdateOperation"));

        if( iWaitDialog )   // Remove the wait note from the screen
            {
            delete iWaitDialog;
            iWaitDialog=NULL;
            }
                
        TInt error = op->iStatus.Int( );        

        delete iUpdateOperation;
        iUpdateOperation = NULL;

        if( error )
            {
            LOGTEXT2(_L16("MMBoxDialog: OpCompleted DeleteOperation Error: %d"), error);
            TRAP_IGNORE( ShowInformationNoteL( R_NOT_MMBOX_INFO_UNABLE_UPDATE, EFalse ) );
            }
        else
            {
            LOGTEXT(_L8("MMBoxDialog: OpCompleted UpdateOperation Success"));
            TMmboxInfo info;
            TBool infoExists=EFalse;
            TRAPD( err, infoExists=iClientMtm->MmboxInfoL( info ) );

#ifdef MMS_NOTIFICATIONS_TESTING_WO_MMBOX
            infoExists = 1;
            info.mmboxQuotaInBytes = 100500;
            info.mmboxTotalInBytes = 98000;
#endif

            if( infoExists && !err )
                {
                if( info.mmboxTotalInBytes == 0 )
                    {
                    TRAP_IGNORE( ShowInformationNoteL( R_NOT_MMBOX_INFO_NO_MSGS, EFalse ) );
                    }
                else 
	               {
                   if( info.mmboxQuotaInBytes > 0 )
                       { // Make sure there will be no "division by zero"
    	               TReal res = (TReal)info.mmboxTotalInBytes/(TReal)info.mmboxQuotaInBytes;
	                   if( res > 0.8 )
	                        {
	                        TRAP_IGNORE( ShowConfirmationQueryL( R_NOT_MMBOX_QUERY_80_USED, ETrue ) );
	                        }
                       }
	                }
                }
            LOGTEXT(_L8("MMBoxDialog: OpCompleted UpdateOperation 2"));
            TRAP_IGNORE( UpdateNavipaneL( ));  
            // We just need to trap this, we can ignore the error code
            }
        }
    else
        { // Let's save the fetch operations last as there can be many..
        for ( TInt cc=iFetchOperations.Count()-1; cc>=0; cc-- )
            {
            if ( iFetchOperations[cc]->Operation( ).Id( ) == op->Id( ) )
                {
                LOGTEXT(_L8("MMBoxDialog: OpCompleted FetchOperation"));
                delete iFetchOperations[cc];
                iFetchOperations.Delete(cc);
                }
            }
        }
    LOGTEXT(_L8("MMBoxDialog: OpCompleted End"));
    ListBox()->DrawDeferred();
    }

// ----------------------------------------------------
// CMMBoxDialog::DoOperationCompletedL
// ----------------------------------------------------
TBool CMMBoxDialog::DoOperationCompletedL(
    CMsvOperation* aOperation, 
    TInt aCompletionCode )
    {

    if ( !aOperation )
        {
        return EFalse;
        }

    if( iOpenOperation && iOpenOperation->Operation( ).Id( ) == aOperation->Id( ) )
        {
        iFlags &= ~EDoNotDeleteOpen;
        LOGTEXT(_L8("MMBoxDialog: DoOpCompleted Open"));
        if ( aCompletionCode == CMsgEmbeddedEditorWatchingOperation::EMsgExitNext ||
            aCompletionCode == CMsgEmbeddedEditorWatchingOperation::EMsgExitPrevious )
            {
            LOGTEXT(_L8("MMBoxDialog: DoOpCompleted Next/Prev Message"));
            HandleOpenNextPreviousL( aCompletionCode == CMsgEmbeddedEditorWatchingOperation::EMsgExitNext );
            iFlags |= EDoNotDeleteOpen;
            }
        else if ( aCompletionCode == EEikCmdExit )
            {
            LOGTEXT(_L8("MMBoxDialog: DoOpCompleted Exit"));
            delete iOpenOperation;
            iOpenOperation = NULL;
            iExitCode = EEikCmdExit;
            return ETrue;
            }
        }
    else if( iUpdateOperation && iUpdateOperation->Operation( ).Id( ) == aOperation->Id( ) )
        {
        LOGTEXT(_L8("MMBoxDialog: DoOpCompleted Update"));
        UpdateMMBoxArrayL();
        }
    else if ( iDeleteOperation && iDeleteOperation->Operation( ).Id( ) == aOperation->Id( ) )
        {
        LOGTEXT(_L8("MMBoxDialog: DoOpCompleted Delete"));
        UpdateMMBoxArrayL();
        }
    else
        {
        // Nothing special to be done (for example for fetch operations)
        }
    return EFalse;
    }

// ----------------------------------------------------
// CMMBoxDialog::LaunchHelpL
// launch help using context
// 
// ----------------------------------------------------
void CMMBoxDialog::LaunchHelpL()
    {   
    if ( iFlags & EHelpSupported )
        {
        LOGTEXT(_L8("MMBoxDialog: LaunchHelpL"));
        CCoeAppUi* appUi = ControlEnv()->AppUi();
        if ( appUi )
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), appUi->AppHelpContextL() );
            }
        LOGTEXT(_L8("MMBoxDialog: LaunchHelpL End"));
        }
    }

// ----------------------------------------------------
// CMMBoxDialog::GetHelpContext
// returns helpcontext as aContext
// 
// ----------------------------------------------------
void CMMBoxDialog::GetHelpContext
        ( TCoeHelpContext& aContext ) const
    {  
    const TUid KUidMce ={0x100058C5};      
    aContext.iMajor = KUidMce;
    aContext.iContext = KMCE_HLP_MMS_NOTIFICATIONS;
    }

// ----------------------------------------------------
// CMMBoxDialog::
// ----------------------------------------------------
TInt  CMMBoxDialog::DecodeProgress (
            const TDesC8& /*aProgress*/,
            TBuf<CBaseMtmUi::EProgressStringMaxLen>& /*aReturnString*/,
            TInt& /*aTotalEntryCount*/,
            TInt& /*aEntriesDone*/,
            TInt& /*aCurrentEntrySize*/,
            TInt& /*aCurrentBytesTrans*/, TBool /*aInternal*/ )
    {
    // At the moment there are no operations who would need this function.
    // Only delete operation uses ProgressDecoder, but that progress is
    // asked from NotMtmUi object.
    return KErrNone;
    }

// ----------------------------------------------------
// CMMBoxDialog::
// ----------------------------------------------------
void CMMBoxDialog::SortMessagesByL( TMMBoxSortType aType )
    {
    // Lets memorize the id of the current focused one
   LOGTEXT(_L8("MMBoxDialog: SortMessageByL start"));
    TMsvId focused = KErrNotFound;
    
    if( iMessages->MdcaCount() > 0 )
        {
        focused= iMessages->EntryId( ListBox( )->CurrentItemIndex( ) );
        }
    
    // Lets set the sorting type
    TMsvSelectionOrdering newType = iMessages->SetSortType( aType );

    // Lets find the focused one again
    TInt index = 0;
    TInt newFocused = KErrNotFound;
    while( index < iMessages->MdcaCount( ) )
        {
        if( iMessages->EntryId( index ) == focused )
            {
            newFocused = index;
            break;
            }
        index++;
        }
    
    //Let's set the new focused item and draw the listbox
    LOGTEXT(_L8("MMBoxDialog: SortMessageByL 2"));
    if( newFocused != KErrNotFound )
        {
        LOGTEXT(_L8("MMBoxDialog: SortMessageByL 3"));
        ListBox( )->SetCurrentItemIndexAndDraw( newFocused );
        }

    iFlags |= ESortTypeChanged;

    LOGTEXT(_L8("MMBoxDialog: SortMessageByL 4"));
    CMsvEntry* entry=iSession.GetEntryL( KMsvRootIndexEntryId );
    CleanupStack::PushL( entry );
    CMsvStore* store = entry->EditStoreL();
    LOGTEXT(_L8("MMBoxDialog: SortMessageByL 5"));
    CleanupStack::PushL( store );
    store->Remove(KUidMsvMessageListSortOrder);
    store->CommitL();
    CleanupStack::PopAndDestroy( 1 ); // store

    LOGTEXT(_L8("MMBoxDialog: SortMessageByL 6"));
 	CMsvStore* store2 = entry->EditStoreL();
 	CleanupStack::PushL( store2 );
    LOGTEXT(_L8("MMBoxDialog: SortMessageByL 7"));
 	RMsvWriteStream writeStream;
 	writeStream.AssignLC( *store2, KUidMsvMessageListSortOrder );
    LOGTEXT(_L8("MMBoxDialog: SortMessageByL 8"));
 	newType.ExternalizeL( writeStream );
 	writeStream.CommitL();
 	store2->CommitL();
 	CleanupStack::PopAndDestroy( 3 ); // store, writestream, entry

    LOGTEXT(_L8("MMBoxDialog: SortMessagesByL End"));
    }

// ----------------------------------------------------
// CMMBoxDialog::
// ----------------------------------------------------
void CMMBoxDialog::OpenNotificationL( )
    {
    LOGTEXT(_L8("MMBoxDialog: OpenNotificationL start"));

    CMsvOperation* op=NULL;
    CAknInputBlock::NewLC(  );

    CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL(singleOpWatcher);

    TMsvEntry entry = 
        iMessages->Entry( ListBox( )->CurrentItemIndex( ) );

    if ( entry.iType != KUidMsvMessageEntry )
        { // No message -> wrong kind of entry
        CleanupStack::PopAndDestroy( 2 ); // singleOpWatcher, CAknInputBlock
        return;
        }
    else
        {
        iOpenOperation = singleOpWatcher;
        }

    iUi.BaseMtm().SwitchCurrentEntryL( entry.Id( ) );
    iUi.SetPreferences( iUi.Preferences() | EMtmUiFlagEditorPreferEmbedded );

    LOGTEXT(_L8("MMBoxDialog: OpenNotificationL 2"));
    op = iUi.OpenL(singleOpWatcher->iStatus );
    LOGTEXT(_L8("MMBoxDialog: OpenNotificationL 3"));

    CleanupStack::Pop( singleOpWatcher );

    singleOpWatcher->SetOperation(op);

    CleanupStack::PopAndDestroy( ); // CAknInputBlock::NewLC(  );
    LOGTEXT(_L8("MMBoxDialog: OpenL End"));
    }

// ----------------------------------------------------
// CMMBoxDialog::
// ----------------------------------------------------
void CMMBoxDialog::FetchL( TBool aAll/*=EFalse*/ )
    {
    if ( IS_PHONE_OFF_LINEL() )
        {   // We are in offline at the moment -> Infonote
        ShowInformationNoteL( R_NOTMTM_OFFLINE_NOT_POSSIBLE, EFalse );
        return;
        }

    CMsvEntrySelection* sel = SelectionL( aAll );
    CleanupStack::PushL( sel );

    // Lets see which ones are already deleted from server.
    TInt all = 0; // includes all notifications in the selection
    TInt bad = 0; // includes notifications that are not stored in mmbox
    for( TInt i = 0; i<sel->Count( ); i++ )
        {
        TMsvEntry entry;
        TMsvId service=-1;
        iSession.GetEntry( sel->At( i ), service, entry );
        if( ( ( entry.iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageMNotificationInd ) &&
              ( entry.iMtmData2 & KMmsStoredInMMBox ) )
            {
            all++;
            }
        else
            {
            if( ( entry.iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageMNotificationInd )
                {
                bad++;
                all++;
                }
            }
        }

    if( bad == 1 )
        {
        // One already deleted
        ShowInformationNoteL( R_NOT_MMBOX_INFO_FETCH_FAILED_ONE, EFalse );
        if( all == bad )
            { // Just one notification -> leave from the function
            CleanupStack::PopAndDestroy( sel );
            return;
            }
        }
    else if (bad > 0)
        {
        if( all == bad )
            { // All notifications were bad
            ShowInformationNoteL( R_NOT_MMBOX_INFO_FETCH_FAILED_ANY, EFalse );
            LOGTEXT(_L8("MMBoxDialog: FetchL All bad - just return"));
            CleanupStack::PopAndDestroy( sel );
            return;
            }
        else
            { // Some were bad, so one can continue fetching
            ShowInformationNoteL( R_NOT_MMBOX_INFO_FETCH_FAILED_SOME, EFalse );
            LOGTEXT(_L8("MMBoxDialog: FetchL Some bad - warn"));
            }
        }
    else // bad == 0
        {
        if( all == 0 )
            { // No notifications at all -> no note and just leave
            LOGTEXT(_L8("MMBoxDialog: FetchL Selection empty - just return"));
            CleanupStack::PopAndDestroy( sel );
            return;
            }
        // else just continue as there were no bad notifications
        }

    TMsvId origId = iClientMtm->Entry().Entry().Id();

    if( sel->Count( ) == 1 )
        {
        iClientMtm->SwitchCurrentEntryL( sel->At( 0 ) );
        iClientMtm->LoadMessageL( );
        TTime currentTime;
        currentTime.HomeTime( );
        TTime expiryTime = iClientMtm->ExpiryDate( );
        iClientMtm->SwitchCurrentEntryL( origId );
        iClientMtm->LoadMessageL( );
	    TLocale locale;
        expiryTime += locale.UniversalTimeOffset();
	    if (locale.QueryHomeHasDaylightSavingOn())          
		    {
		    TTimeIntervalHours daylightSaving(1);          
		    expiryTime += daylightSaving;
		    }

        if( currentTime > expiryTime )
            {   // Message is expired
            TBool res = EFalse;
            TRAPD( err, res=ShowConfirmationQueryL( R_NOT_MMBOX_QUEST_MSG_EXPIRED ) );
            if ( err || !res )
                {   // User did not want to fetch expired message or the function leaved
            CleanupStack::PopAndDestroy( sel );
                return;
                } // else we continue
            }
        }

    CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL(*this);
    CleanupStack::PushL(singleOpWatcher);

    TBuf8<1> buf;

    CMsvOperation* op = NULL;

    LOGTEXT(_L8("MMBoxDialog: FetchL 2"));
    TRAPD( err, 
            op= iUi.InvokeAsyncFunctionL( 
            KMtmUiFunctionFetchMMS, 
            *sel, 
            singleOpWatcher->iStatus, 
            buf) );
    LOGTEXT(_L8("MMBoxDialog: FetchL 3"));

    if( op == NULL || err )
        {
        LOGTEXT(_L8("MMBoxDialog: FetchL NULL op or left"));
        CleanupStack::PopAndDestroy( singleOpWatcher );
        CleanupStack::PopAndDestroy( sel );
        return;
        }

    CleanupStack::Pop( singleOpWatcher );
   
    singleOpWatcher->SetOperation( op );
    iFetchOperations.AppendL( singleOpWatcher );

    CleanupStack::PopAndDestroy( sel );

    LOGTEXT(_L8("MMBoxDialog: FetchL End"));
    return;
    }

// ----------------------------------------------------
// CMMBoxDialog::DeleteL
// ----------------------------------------------------
void CMMBoxDialog::DeleteL( TBool aAll/*=EFalse*/ )
    {
    LOGTEXT(_L8("MMBoxDialog: DeleteL start"));

    if ( IS_PHONE_OFF_LINEL() )
        {   // We are in offline at the moment -> Infonote
        ShowInformationNoteL( R_NOTMTM_OFFLINE_NOT_POSSIBLE, EFalse );
        return;
        }

    CAknInputBlock::NewLC( );

    if( iDeleteSelection )
        {
        // We should never end up in here because delete op has a wait note but just in case
        delete iDeleteSelection;
        iDeleteSelection = NULL;
        }

    iDeleteSelection = SelectionL( aAll );
    LOGTEXT(_L8("MMBoxDialog: DeleteL 2"));

    CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL(*this);
    CleanupStack::PushL(singleOpWatcher);

    TBuf8<1> buf;

    LOGTEXT(_L8("MMBoxDialog: DeleteL 3"));
    CMsvOperation* op= iUi.InvokeAsyncFunctionL( 
        KMtmUiFunctionDeleteMessage, 
        *iDeleteSelection, 
        singleOpWatcher->iStatus, 
        buf);

    LOGTEXT(_L8("MMBoxDialog: DeleteL 4"));
    if( op == NULL )
        {
        // Lets just make a completed operation so
        // the progress dialog is not shown
        LOGTEXT(_L8("MMBoxDialog: Delete NULL op"));
        delete iDeleteSelection;
        iDeleteSelection = NULL;
        TPckgC < TMsvId > progress = 0;
        op = CMsvCompletedOperation::NewL( iSession, 
            KUidMsgMMSNotification,
            progress,
            KMsvLocalServiceIndexEntryId, singleOpWatcher->iStatus );
        }
    LOGTEXT(_L8("MMBoxDialog: DeleteL 5"));

    CleanupStack::Pop( singleOpWatcher );
    
    singleOpWatcher->SetOperation( op );
    iDeleteOperation = singleOpWatcher;

    CleanupStack::PopAndDestroy( ); // CAknInputBlock

    LOGTEXT(_L8("MMBoxDialog: DeleteL End"));
    return;
    }

// ----------------------------------------------------
// CMMBoxDialog::SelectionL
// ----------------------------------------------------
CMsvEntrySelection* CMMBoxDialog::SelectionL( TBool aAll/*=EFalse*/ )
    {
    LOGTEXT(_L8("MMBoxDialog: Selection Start"));
    CMsvEntrySelection* sel = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( sel );

    if( aAll )
        {
	    LOGTEXT(_L8("MMBoxDialog: Selection: Use all"));
        TInt index = 0;
        while( index < iMessages->MdcaCount( ) )
            {
	        LOGTEXT(_L8("MMBoxDialog: Selection: Use all: append"));
            sel->AppendL( iMessages->EntryId( index ) );
            index++;
            }
        }
    else
        {
        const CListBoxView::CSelectionIndexArray* indexes = 
            ListBox( )->SelectionIndexes( );
        TInt index = 0;
        
        if( indexes->Count( ) < 1 )
            { // Lets use the current one as there are no selected ones
	        LOGTEXT(_L8("MMBoxDialog: Selection: No selected so just use current"));
            sel->AppendL( iMessages->EntryId( ListBox( )->CurrentItemIndex( ) ) );
            }
        else 
	        {
            LOGTEXT(_L8("MMBoxDialog: Selection: Selected items"));
	        while( index < indexes->Count( ) )
            	{
	            LOGTEXT(_L8("MMBoxDialog: Selection: Selected items added one"));
            	TInt lbIndex = indexes->At(index);
            	sel->AppendL( iMessages->EntryId( lbIndex ) );
            	index++;
            	}
	        }
        }

    LOGTEXT(_L8("MMBoxDialog: Selection Done"));
    CleanupStack::Pop( sel );
    LOGTEXT(_L8("MMBoxDialog: Selection End"));
    return sel;
    }

// ----------------------------------------------------
// CMMBoxDialog::
// ----------------------------------------------------
void CMMBoxDialog::ShowStatusL( )
    {
    LOGTEXT(_L8("MMBoxDialog: ShowStatus Start"));
	HBufC* status;	// holds the text
	HBufC* buf;		// a buffer for strings

    TMmboxInfo info;
    TInt ret = 0;
    TRAPD( err, ret=iClientMtm->MmboxInfoL( info ) )

#ifdef MMS_NOTIFICATIONS_TESTING_WO_MMBOX
    ret = 1;
    info.date.HomeTime();
#endif

    if( err || !ret )
        {
        return; // No quota data available -> just return;
        }
    TTime messageDate = info.date;
    
    // messageDate is GMT so add time offset
    TLocale locale;
    messageDate += locale.UniversalTimeOffset();
    if (locale.QueryHomeHasDaylightSavingOn())          
	    {
	    TTimeIntervalHours daylightSaving(1);          
	    messageDate += daylightSaving;
	    }
    
    TInt usedQuota = info.mmboxTotalInBytes/KLocalKilo;
    if (  info.mmboxTotalInBytes % KLocalKilo )
            {
            usedQuota++;
            }
    TInt fullQuota = info.mmboxQuotaInBytes/KLocalKilo;
    if (  info.mmboxQuotaInBytes % KLocalKilo )
            {
            fullQuota++;
            }

	CEikFormattedCellListBox* listBox = new ( ELeave ) CAknDoublePopupMenuStyleListBox;
	CleanupStack::PushL( listBox );

	CAknPopupList* popupList;
    popupList = CAknPopupList::NewL(listBox, R_AVKON_SOFTKEYS_OK_EMPTY__OK, 
									AknPopupLayouts::EMenuDoubleWindow);
	CleanupStack::PushL( popupList );

    LOGTEXT(_L8("MMBoxDialog: ShowStatus 2"));
    TInt flags = CEikListBox::EDisableHighlight;
	listBox->ConstructL( popupList, flags );
	listBox->CreateScrollBarFrameL( ETrue );
	listBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, 
															CEikScrollBarFrame::EAuto );
    // Lets add the heading

	status = StringLoader::LoadLC( R_NOT_MMBOX_STATUS_TITLE );
	popupList->SetTitleL( *status );
	CleanupStack::PopAndDestroy( status );	// status
	status = NULL;

	MDesCArray* itemList = listBox->Model()->ItemTextArray();
	CDesCArray* itemArray = ( CDesCArray* ) itemList;

    // Lets add the first label "Updated"

    LOGTEXT(_L8("MMBoxDialog: ShowStatus 3"));
	status = HBufC::NewLC( KMMBoxMaxStatusInfoLength );
	buf = StringLoader::LoadLC( R_NOT_MMBOX_UPDATE_LABEL );	// first line - bold text
	status->Des().Append( *buf );
	status->Des().Append( TChar( '\t' ) );

    // Lets add the update time

    LOGTEXT(_L8("MMBoxDialog: ShowStatus 4"));
    HBufC* dateFormat = StringLoader::LoadLC( R_QTN_DATE_USUAL_WITH_ZERO, iCoeEnv );
    TBuf<KDateSize> dateString;
    messageDate.FormatL( dateString, *dateFormat );
    CleanupStack::PopAndDestroy(); //dateFormat

    dateFormat = StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO, iCoeEnv );
    TBuf<KDateSize> timeString;
    messageDate.FormatL( timeString, *dateFormat );
    CleanupStack::PopAndDestroy(); //dateFormat

    LOGTEXT(_L8("MMBoxDialog: ShowStatus 5"));
    CDesC16ArrayFlat* array = new(ELeave) CDesC16ArrayFlat( 2 );
    CleanupStack::PushL( array );
    array->AppendL( timeString );
    array->AppendL( dateString );
    buf = 
        StringLoader::LoadL( R_NOT_MMBOX_INFO_TIME, *array, iCoeEnv );
    status->Des().Append( *buf );
    CleanupStack::PopAndDestroy( array );
    CleanupStack::PushL( buf );

    itemArray->AppendL( *status );
	CleanupStack::PopAndDestroy(3);	// buf, buf, status
	buf = NULL;
	status = NULL;

    // Lets add the 2nd label "Storage in use"

    LOGTEXT(_L8("MMBoxDialog: ShowStatus 6"));
	status = HBufC::NewLC( KMMBoxMaxStatusInfoLength );
	buf = StringLoader::LoadLC( R_NOT_MMBOX_STORAGE_LABEL );	// first line - bold text
	status->Des().Append( *buf );
	status->Des().Append( TChar( '\t' ) );

    // Lets add the "X %" stuff

	TInt percentage = 100;

	if( fullQuota > 0 )
		{
		percentage = TInt((((TReal)usedQuota/(TReal)fullQuota)+TReal(0.005))*100);
		}

	buf = StringLoader::LoadLC( R_NOT_MMBOX_STORAGE_DATA, percentage, iCoeEnv );

    status->Des().Append( *buf );
    itemArray->AppendL( *status );

    CleanupStack::PopAndDestroy( 3 );	// buf, buf, status
	buf = NULL;
	status = NULL;

    // Lets add the 3rd label "Quota used"

	status = HBufC::NewLC( KMMBoxMaxStatusInfoLength );
	buf = StringLoader::LoadLC( R_NOT_MMBOX_QUOTA_LABEL );	// first line - bold text
	status->Des().Append( *buf );
	status->Des().Append( TChar( '\t' ) );

    // Lets add the " 5 / 5" stuff

    LOGTEXT(_L8("MMBoxDialog: ShowStatus 7"));
    CArrayFixFlat<TInt>* array3 = new(ELeave) CArrayFixFlat<TInt>( 2 );
    CleanupStack::PushL( array3 );
    array3->AppendL( usedQuota );
    array3->AppendL( fullQuota );
    buf = 
        StringLoader::LoadL( R_NOT_MMBOX_QUOTA_DATA, *array3, iCoeEnv );
    CleanupStack::PopAndDestroy( array3 ); //array3
    CleanupStack::PushL( buf );

    status->Des().Append( *buf );
    itemArray->AppendL( *status );

    CleanupStack::PopAndDestroy( 3 );	// buf, buf, status
	buf = NULL;
	status = NULL;

	CleanupStack::Pop( popupList );	// popupList 

	// Show Dialog
    LOGTEXT(_L8("MMBoxDialog: ShowStatus 8"));
	popupList->ExecuteLD( );
	// cleanup
	CleanupStack::PopAndDestroy( listBox );	//listBox
	listBox = NULL;
    LOGTEXT(_L8("MMBoxDialog: ShowStatus End"));
    }

// ----------------------------------------------------
// CMMBoxDialog::UpdateListL
// ----------------------------------------------------
void CMMBoxDialog::UpdateListL( )
    {
    LOGTEXT(_L8("MMBoxDialog: UpdateListL start"));
    if ( IS_PHONE_OFF_LINEL() )
        {   // We are in offline at the moment -> Infonote
        ShowInformationNoteL( R_NOTMTM_OFFLINE_NOT_POSSIBLE, EFalse );
        return;
        }
    LOGTEXT(_L8("MMBoxDialog: UpdateListL 2"));

    if( !iUi.CheckSettingsL( ) )
        { // AP is not valid so just return ->
        return;
        }
    LOGTEXT(_L8("MMBoxDialog: UpdateListL 3"));

    CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL(*this);
    CleanupStack::PushL(singleOpWatcher);

    CMsvOperation* updateOp = iClientMtm->UpdateMmBoxListL(
                        singleOpWatcher->iStatus );

    if( updateOp == NULL )
        {
        // Unable to update the list -> show error
        LOGTEXT(_L8("MMBoxDialog: UpdateList NULL op"));
        CleanupStack::PopAndDestroy( singleOpWatcher );
        ShowInformationNoteL( R_NOT_MMBOX_INFO_UNABLE_UPDATE, EFalse );
        return;
        }

    CleanupStack::Pop( singleOpWatcher );    
    iUpdateOperation = singleOpWatcher;
    iUpdateOperation->SetOperation( updateOp );

    ShowWaitNoteL( R_NOT_MMBOX_WAIT_UPDATING_NOTIF, ETrue );

    LOGTEXT(_L8("MMBoxDialog: UpdateList End"));
    return;
    }

// ---------------------------------------------------------
// CMMBoxDialog::ShowInformationNoteL
// ---------------------------------------------------------
//
void CMMBoxDialog::ShowInformationNoteL( TInt aResourceID, TBool aWaiting )
    {
    HBufC* string = NULL;
    string = StringLoader::LoadLC( aResourceID, iCoeEnv );

    CAknInformationNote* note = new ( ELeave )
        CAknInformationNote( aWaiting );
    note->ExecuteLD( *string );

    CleanupStack::PopAndDestroy(); //string
    }

// ---------------------------------------------------------
// CMMBoxDialog::ShowConfirmationQueryL
// ---------------------------------------------------------
//
TInt CMMBoxDialog::ShowConfirmationQueryL( TInt aResourceId, TBool aOnlyOK/*=EFalse*/ ) const
    {
    HBufC* prompt = StringLoader::LoadLC( aResourceId, iCoeEnv );
    TInt retVal = ShowConfirmationQueryL( *prompt, aOnlyOK );
    CleanupStack::PopAndDestroy( prompt );
    return retVal;
    }

// ---------------------------------------------------------
// CMMBoxDialog::ShowConfirmationQueryL
// ---------------------------------------------------------
//
TInt CMMBoxDialog::ShowConfirmationQueryL( const TDesC& aText, TBool aOnlyOK/*=EFalse*/ ) const
    {
    TInt dialogType=aOnlyOK?R_NOTUI_CONFIRMABLE_QUERY:R_NOTUI_CONFIRMATION_QUERY;
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    TInt retVal = dlg->ExecuteLD( dialogType, aText );
    return retVal;    
    }

// ---------------------------------------------------------
// CMMBoxDialog::HandleSessionEventL
// ---------------------------------------------------------
//
void CMMBoxDialog::HandleSessionEventL(TMsvSessionEvent aEvent, 
        TAny* /*aArg1*/, TAny* aArg2, TAny* /*aArg3*/)
    {
    LOGTEXT(_L8("MMBoxDialog: HandleSessionEvent Start"));
    switch( aEvent )
        {
        case EMsvEntriesCreated:
        case EMsvEntriesDeleted:
        case EMsvEntriesChanged:
			{
			TMsvId parentId = KMsvNullIndexEntryId;
			if ( aArg2 != NULL )
				{	
				parentId = *(TMsvId*)aArg2;
				}
			if( parentId == iMMBoxFolderId )
				{ // in MMBoxFolder
				ListBox( )->ClearSelection( );
                if( !iOpenOperation && !iUpdateOperation && !iDeleteOperation )
                    { // Lets close the menubar because something has been changed..
				    LOGTEXT(_L8("MMBoxDialog: HandleSessionEvent Close menubar"));
                    iMenuBar->StopDisplayingMenuBar( );
				    LOGTEXT(_L8("MMBoxDialog: HandleSessionEvent Close menubar END"));
                    }
				if ( aEvent == EMsvEntriesCreated )
					{
				    LOGTEXT(_L8("MMBoxDialog: HandleSessionEvent Handle Item Addition"));
				    ListBox()->HandleItemAdditionL();
					ListBox()->ScrollToMakeItemVisible( ListBox()->CurrentItemIndex( ) );
					ListBox()->DrawNow();
				    LOGTEXT(_L8("MMBoxDialog: HandleSessionEvent Handle Item Addition End"));
					}
				else if ( aEvent == EMsvEntriesDeleted )
					{
					LOGTEXT(_L8("MMBoxDialog: HandleSessionEvent Handle Item Removal"));
					// Store the current selected item
                    TInt index = ListBox()->CurrentItemIndex();
                    // Refresh is needed to get the entry updated after the sessionevent
                    iMessages->RefreshArrayL( iMMBoxFolderId );
                    // Let's tell the listbox an item was removed
					LOGTEXT(_L8("MMBoxDialog: HandleSessionEvent Handle Item Removal 2"));
					ListBox()->HandleItemRemovalL();
                    // If current entry is out of boundaries, move the position
                    TInt count = iMessages->MdcaCount( );
                    if( index >= count || index < 0 )
                        {
    					LOGTEXT(_L8("MMBoxDialog: HandleSessionEvent Handle Item Removal Change the index"));
                        index = count - 1;
                        }
                    if( count > 0 )
                        { // Lets make sure when don't highlight in an empty listbox
    					LOGTEXT(_L8("MMBoxDialog: HandleSessionEvent Handle Item Removal Entries left"));
					    ListBox()->SetCurrentItemIndex( index );
    					LOGTEXT(_L8("MMBoxDialog: HandleSessionEvent Handle Item Removal Entries Update SB"));
                        ListBox()->UpdateScrollBarsL( );
                        }
                    else 
                        {
                        SetMSKL();
                        }
					LOGTEXT(_L8("MMBoxDialog: HandleSessionEvent Handle Item Removal DrawNow"));
                    DrawNow();
					ListBox()->DrawNow();
					LOGTEXT(_L8("MMBoxDialog: HandleSessionEvent Handle Item Removal End"));
                    }
				else
					{
					LOGTEXT(_L8("MMBoxDialog: HandleSessionEvent Handle Entry Change"));
                    // Refresh is needed to get the entry updated after the sessionevent
                    iMessages->RefreshArrayL( iMMBoxFolderId );
					ListBox()->DrawNow();
					LOGTEXT(_L8("MMBoxDialog: HandleSessionEvent Handle Entry Change END"));
					}
				}
			break;
			}

        case EMsvCloseSession:
            {
            iSession.RemoveObserver( *this );
            break;
            }

        default:
            {
            break;
            }
        }
    LOGTEXT(_L8("MMBoxDialog: HandleSessionEvent End"));
    }

// ---------------------------------------------------------
// CMMBoxDialog::UpdateNavipaneL
// ---------------------------------------------------------
//
void CMMBoxDialog::UpdateNavipaneL( )
    {
    LOGTEXT(_L8("MMBoxDialog: UpdateNavipaneL start"));
	HBufC* buf;		// a buffer for strings

    TMmboxInfo info;
    TInt ret = 0;
    TRAPD( err, ret=iClientMtm->MmboxInfoL( info ) );
    
#ifdef MMS_NOTIFICATIONS_TESTING_WO_MMBOX
    ret = 1;
    info.date.HomeTime();
    User::After( 1000000 );
#endif
    
    if( !err && ret )
        {
        TTime updateDate = info.date;

	    TLocale locale;
        updateDate += locale.UniversalTimeOffset();
	    if (locale.QueryHomeHasDaylightSavingOn())          
		    {
		    TTimeIntervalHours daylightSaving(1);          
		    updateDate += daylightSaving;
		    }

        TTime currentDate;
        currentDate.HomeTime( );

        TDateTime update = updateDate.DateTime( );
        TDateTime current = currentDate.DateTime( );
        if( update.Day( ) == current.Day( ) &&            
            update.Month( ) == current.Month( ) &&
            update.Year( ) == current.Year( ) )
            {   // It's the same day

            HBufC* dateFormat = StringLoader::LoadLC( R_QTN_TIME_USUAL, iCoeEnv );
            TBuf<KDateSize> timeString;
            updateDate.FormatL( timeString, *dateFormat );
            CleanupStack::PopAndDestroy(); //dateFormat

            CDesC16ArrayFlat* array = new(ELeave) CDesC16ArrayFlat( 1 );
            CleanupStack::PushL( array );
            array->AppendL( timeString );
            buf = 
                StringLoader::LoadL( R_NOT_MMBOX_NAVI_TIME, *array, iCoeEnv );
            CleanupStack::PopAndDestroy( array );
            CleanupStack::PushL( buf );
            
            }
        else
            {   // It's not the same day
            HBufC* dateFormat = StringLoader::LoadLC( R_QTN_DATE_SHORT, iCoeEnv );
            TBuf<KDateSize> dateString;
            updateDate.FormatL( dateString, *dateFormat );
            CleanupStack::PopAndDestroy(); //dateFormat

            CDesC16ArrayFlat* array = new(ELeave) CDesC16ArrayFlat( 1 );
            CleanupStack::PushL( array );
            array->AppendL( dateString );
            buf = 
                StringLoader::LoadL( R_NOT_MMBOX_NAVI_DATE, *array, iCoeEnv );
            CleanupStack::PopAndDestroy( array );
            CleanupStack::PushL( buf );
            }
        }
    else
        {
        buf = HBufC::NewL(0);
        CleanupStack::PushL( buf );
        }

    delete iNaviDecorator;
    iNaviDecorator = NULL;

    iNaviDecorator = iNaviPane->CreateTabGroupL();
    CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>
        ( iNaviDecorator->DecoratedControl() );

    tabGroup->AddTabL( 0, *buf );

    tabGroup->SetActiveTabById( 0 );
    tabGroup->SetTabFixedWidthL( KTabWidthWithOneTab );

	CleanupStack::PopAndDestroy( buf );	// buf, buf, status
    
    iNaviPane->PushL( *iNaviDecorator );  // Make it visible
    LOGTEXT(_L8("MMBoxDialog: UpdateNavipaneL End"));
    }

// ----------------------------------------------------
// CMMBoxDialog::HandleOpenNextPreviousL
// ----------------------------------------------------
void CMMBoxDialog::HandleOpenNextPreviousL( TBool aOpenNext )
    {
    LOGTEXT(_L8("MMBoxDialog: Open Next/Prev Start"));
    TInt newIndex = ListBox( )->CurrentItemIndex( );

    if( iOpenOperation )
        {
        delete iOpenOperation;
        iOpenOperation = NULL;
        }

    newIndex += aOpenNext?1:-1;

    if( newIndex < 0 || newIndex >= iMessages->MdcaCount( ) )
        { // Index out of array -> lets just ignore this..
        return;
        }

    ListBox()->SetCurrentItemIndex( newIndex );
    ListBox()->DrawDeferred();

    TMsvEntry entry = iMessages->Entry( newIndex );

    if( iUiData.OperationSupportedL( KMtmUiFunctionOpenMessage, entry ) )
        {
        return;
        }

    OpenNotificationL( );    
    }

// ---------------------------------------------------------
// ShowWaitNoteLC
// ---------------------------------------------------------

void CMMBoxDialog::ShowWaitNoteLC(
    TInt aNoteTxtResourceId, 
    TBool aVisibilityDelayOff )
    {
    ConstructWaitNoteLC( aVisibilityDelayOff );
    HBufC* text = StringLoader::LoadLC( aNoteTxtResourceId, iCoeEnv );
    iWaitDialog->SetTextL( *text );
    CleanupStack::PopAndDestroy( ); // text
    iWaitDialog->RunLD( );
    }

// ---------------------------------------------------------
// ConstructWaitNoteLC
// ---------------------------------------------------------

void CMMBoxDialog::ConstructWaitNoteLC(
    TBool aVisibilityDelayOff )
    {
    delete iWaitDialog;
    iWaitDialog = NULL;
    iWaitDialog = new (ELeave) CAknWaitDialog( 
        reinterpret_cast<CEikDialog**>( &iWaitDialog ), 
        aVisibilityDelayOff );
    CleanupStack::PushL( TCleanupItem( CleanupWaitDialog, (TAny**)&iWaitDialog ) ); 
    iWaitDialog->PrepareLC( R_NOTUI_WAIT_NOTE );
    }

// ----------------------------------------------------
// CMMBoxDialog::CleanupWaitDialog
// ----------------------------------------------------
void CMMBoxDialog::CleanupWaitDialog(TAny* aAny)
    {
    CAknWaitDialog** dialog = (CAknWaitDialog**) aAny;
    if ( dialog && *dialog )
        {
        delete *dialog;
        *dialog = NULL;
        }
    }

// ---------------------------------------------------------
// ShowWaitNoteL
// ---------------------------------------------------------

void CMMBoxDialog::ShowWaitNoteL(
    TInt aNoteTxtResourceId, 
    TBool aVisibilityDelayOff )
    {
    HBufC* text = StringLoader::LoadLC( aNoteTxtResourceId, iCoeEnv );
    delete iWaitDialog;
    iWaitDialog = NULL;
    iWaitDialog = new (ELeave) CAknWaitDialog( 
        reinterpret_cast<CEikDialog**>( &iWaitDialog ), 
        aVisibilityDelayOff );
    iWaitDialog->SetTextL( *text );
    CleanupStack::PopAndDestroy( ); // text
    iWaitDialog->PrepareLC( R_NOTUI_WAIT_NOTE );
    iWaitDialog->RunLD( );
    }

// ---------------------------------------------------------
// SetMSKL
// ---------------------------------------------------------
void CMMBoxDialog::SetMSKL()
    {
    const TInt KMskPosition = 3;
    CEikColumnListBox* listBox = ListBox();
    if ( listBox->Model()->NumberOfItems() > 0 )
        {
        TInt resourceId;
        if ( listBox->View()->ItemIsSelected( listBox->CurrentItemIndex() ) )
            {
            resourceId = R_NOT_MMBOX_MSK_BUTTON_UNMARK;
            }
        else
            {
            resourceId = R_NOT_MMBOX_MSK_BUTTON_MARK;
            }
            
        CEikButtonGroupContainer &cba = ButtonGroupContainer();
        cba.SetCommandL( KMskPosition, resourceId );
        cba.DrawDeferred();
        }
    else
        {
        CEikButtonGroupContainer &cba = ButtonGroupContainer();
        cba.SetCommandL( KMskPosition, R_NOT_MMBOX_MSK_BUTTON_EMPTY );
        cba.DrawDeferred();
        }
    }

// ---------------------------------------------------------
// ShowConfirmationQueryL
// ---------------------------------------------------------
TInt CMMBoxDialog::ShowConfirmationQueryL(  TInt    aCount,
                                            TInt    aTotalOnes,                          
                                            TInt    aResource )
    {
    CArrayFixFlat<TInt>* arr = new(ELeave) CArrayFixFlat<TInt>( 2 );
    CleanupStack::PushL( arr );
    arr->AppendL( aCount );
    arr->AppendL( aTotalOnes );
    HBufC* buf = 
        StringLoader::LoadL( aResource, *arr, iCoeEnv );
    CleanupStack::PopAndDestroy( arr );
    CleanupStack::PushL( buf );

    TInt ret = ShowConfirmationQueryL( *buf, ETrue );

    CleanupStack::PopAndDestroy( buf );
    return ret;
    }
//  End of File
