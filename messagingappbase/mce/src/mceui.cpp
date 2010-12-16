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
*     Defines methods for CMceUi
*
*/




// INCLUDE FILES
#include <bldvariant.hrh>
#include <featmgr.h>

#include <messagingvariant.hrh>
#include <centralrepository.h>
#include <messaginginternalcrkeys.h>
#include <CoreApplicationUIsSDKCRKeys.h>

// security data caging
#include <data_caging_path_literals.hrh>

#include <mtudreg.h>        // cmtmuidataregistry
#include <MTMStore.h>       // cmtmstore
#include <miutset.h>
#include <MuiuMsvProgressReporterOperation.h> // cmsvprogressreporteroperation
#include <mtmdef.h>         // KUidMtmQueryCanSendMsgValue
#include <msvids.h>
#include <msvuids.h>
#include <mtud.hrh>         // EMtudCommandTransferSend, EMtudCommandTransferSend, EMtudCommandTransferReceive

#include <MuiuMsgEmbeddedEditorWatchingOperation.h> // tmsgexitmode
#include <MuiuOperationWait.h>  // cmuiuoperationwait
#include <muiumsvuiserviceutilitiesinternal.h> // msvuiserviceutilitiesinternal
#include <MuiuMsvUiServiceUtilities.h> // msvuiserviceutilities
class CGulIcon;
#include <MsgFolderSelectionDialog.h>
#include <PushMtmCommands.hrh>    // epushmtmcmdcollectgarbage
#include <smutcmds.hrh>         // KMtmUiFunctionSmumRemoveSMSC
#include <MsgBioUids.h>     // kmsgbiouidpicturemsg
#include <mtuireg.h>        // CMtmUiRegistry
#include <mmsconst.h>
#include <obexclientmtm.h>
#include <MuiuOperationWait.h>  // cmuiuoperationwait
#include <muiu.mbg>
#include <muiu.rsg>
#include <MuiuMsvUiServiceUtilities.h>
#include <mtmuidef.hrh>

#include <SenduiMtmUids.h>      // mtm uids
#include <SendUiConsts.h>

// MMS.Content.Upload
#include <CSendingServiceInfo.h>
#include <sendui.h>
#include <CMessageData.h>

#include <layoutmetadata.cdl.h>//for layout id
#include <aknappui.h>
#include <StringLoader.h>   // stringloader
#include <akntabgrp.h>
#include <aknnavide.h>
#include <aknclearer.h>
#include <aknnotewrappers.h>
#include <akninputblock.h> // cakninputblock
#include <akntitle.h>
#include <ErrorUI.h>        // cerrorui
#include <aknnotedialog.h>
#include <akninputblock.h> // cakninputblock
#include <akncontext.h>

#include <gfxtranseffect/gfxtranseffect.h>     // for transition effects
#include <akntranseffect.h>                    // for transition effects

#include <DocumentHandler.h>


#include <RCustomerServiceProfileCache.h>


// MMS.Content.Upload
#include <muiumsguploadparam.h>
#include <CSendingServiceInfo.h>

#include <ImumInternalApi.h>                // cimuminternalapi
#include <ImumInMailboxServices.h>          // mimuminmailboxservices
#include <ImumInSettingsData.h>             // cimuminsettingsdata
#include <ImumInSettingsKeys.h>             // timuminsettings
#include <UiklafInternalCRKeys.h>

#include <MNcnInternalNotification.h>
#include <NcnNotificationDefs.h>

#include <BTSapDomainPSKeys.h>
#include <e32property.h>

#include <muiulock.h>

#include <messaginginternalpskeys.h>        // kpsuidmuiu, kmuiukeycurrentmsg
#include <e32property.h>                    // RProperty

#include <CPhCltUssd.h>

#include <RPhCltServer.h>

#include <hlplch.h>             // For HlpLauncher

// For Muiu split
#include <muiumsvuiserviceutilitiesinternal.h>

#include <ImumInternalApi.h>                // cimuminternalapi

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif // RD_MULTIPLE_DRIVE

#include <mce.rsg>
#include "MceCommands.hrh"
#include "mceui.h"
#include "MceSessionHolder.h"
#include "MceMessageListView.h"
//#include "MceMessageListbox.h"
#include "MceMainViewListView.h"
#include <mcesettingsdialog.h>
#include <mcesettingsarrays.h>
#include "MceMessageListContainerBase.h"
//#include "MceMessageListContainer.h"
#include "MceMainViewListContainer.h"
#include "MceBitmapResolver.h"
#include "MceConnectMailboxTimer.h"
#include "MceSendOperation.h"
#include "McePanic.h"
#include "MceLogEngine.h"
#include "MceDeliveryReportView.h"
//#include "MceMessageListItemArray.h"
#include "MceMainViewListItemArray.h"
#include "MceIdleMtmLoader.h"
#include "MceNaviPaneFolderIndicator.h"
//#include "MceEmailEditorResetTimer.h"
#include "MceCancelSendingOperation.h"
#include "MceChangeFlagOperation.h"     // cmceremovenewflag
#include "MceEmailNotifHandler.h"
#include "MceAttachmentAddition.h"
#include "mceiaupdateutils.h"


#include <miutset.h>
#include <mtmuids.h>
#include <MessagingDomainCRKeys.h>
#include <mcesettingsemailsel.h>
#include <mtmuids.h>
#include <MessagingDomainCRKeys.h>
//CR:422-271
#include <mmscodecclient.h>
#include "mmsclient.h"
#include "mmsconst.h"
#include "e32cmn.h"
// CONSTANTS

const TInt KMceArrayGranularity                     = 4;
const TUid KMceCBSApplication                       = {0x101F4CD3};
const TInt KMceCBSApplicationStartView              = 2;

_LIT( KMceDirAndFile,"muiu.mbm" );
// CONSTANTS

const TInt KMceAnswerToLifeUniverseAndEverything    = 42;
const TInt KMceMaxFolderLength                      = 35;
const TInt KMceTextLength                           = 120;
    // Estimate disk value for move or copy
const TInt KMceDiskSpaceForMoveOrCopy = 65536;
const TInt KMceMaxDateString(12);  // _mm/dd/yyyy_
const TInt KMceOneMMSNotificationFailed                 = 1;

_LIT8 ( KMceMMSNotifOne8, "1" ); // 8 bit one

#define KMessagingCentreMainViewUid TVwsViewId( \
    TUid::Uid( KMceApplicationUidValue ), \
    TUid::Uid( KMceMainViewListViewIdValue ) )

#ifdef _DEBUG
_LIT( KPanicText, "Mce" );
const TInt KCRepositorySettingFailure = 1;
#endif

const TInt KMceProgressReporterThreshold = 10;

const TUid KMailTechnologyTypeUid = { 0x10001671 };

//cmail update
#define KUidMsgTypeFsMtmVal               0x2001F406
//CR:422-271
_LIT( KMmsMessageDumpDirectory, "C:\\Private\\1000484b\\mmsvar");
_LIT( KRootPath, "C:\\" );
const TInt KMmsCodecClientChunkSize = 1024;

// LOCAL FUNCTION PROTOTYPES

//  ==================== LOCAL FUNCTIONS ====================
//

GLDEF_C void Panic(TMceUiPanic aPanic)
    // Panic function
    {
    User::Panic(_L("Mce"), aPanic);
    }


// ================= MEMBER FUNCTIONS =======================


CMceUi::CMceUi()
    :CAknViewAppUi(),
    iMsgTypesWritePopup( KMceArrayGranularity ),
    iMsgTypesWriteSubmenu( KMceArrayGranularity ),
    iMsgTypesSettings( KMceArrayGranularity ),
    iOperations( KMsvSingleOpWatcherArrayGranularity ),
    iMceIAUpdate( NULL ),
    iMoveOrCopyMailOperation(-1),
    iHandleNewMsgToInbox ( EFalse ),
    iAnchorId( NULL ),
    iEmailClientIntegration(EFalse),
    iSelectableEmail(EFalse),
    iEmailFramework(EFalse)
    {
    iMceUiFlags.SetMceFlag( EMceUiFlagsExitOnMsvMediaAvailableEvent );
    }

CMceUi::~CMceUi()
    {
    MCELOGGER_ENTERFN("~CMceUi()");
    iCoeEnv->RemoveForegroundObserver( *this ); // CR : 401-1806
    delete iBitmapResolver;
    if(iMtmLoader)
        {
        iMtmLoader->Cancel();
        delete iMtmLoader;
        }

#ifdef _DEBUG
    MCELOGGER_WRITE_FORMAT("iOperations.Count = %d", iOperations.Count() );
    const TInt count = iOperations.Count();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        MCELOGGER_WRITE_FORMAT("loop = %d", loop );
        MCELOGGER_WRITE_FORMAT("operation id = %d", iOperations[loop]->Operation().Id() );
        }
#endif
    iOperations.ResetAndDestroy();
    delete iMTMFunctionsArray;
    delete iUiRegistry;
    delete iEntry;
    delete iRootEntry;
    delete iMtmStore;
    delete iConnectMailboxTimer;
    if ( iSessionHolder )
        {
        // must not delete sessionholder, it will kill itself...
        iSessionHolder->RemoveClient();
        // must not delete iSession, session holder takes care of that
        }
    delete iMceLogEngine;

    delete iFolderIndicator;
    delete iTabsArray;
    delete iMMSNotifications;
    delete iDecoratedTabGroup;
    delete iMceErrorUi;
    delete iLocalScreenClearer;
    // iSession and iMceDeliveryView owned by iSessionHolder
    // don't delete iMessageTypesDlg, dialog will delete itself.
    delete iMailAccountItemArray;
    // MMS.Content.Upload
    iUploadServices.ResetAndDestroy();
    delete iEmailApi;
    delete iEmailNotifHandler;
    delete iCancelExitFlagOperation;
    
    if(iMceIAUpdate)
	    {
	    delete iMceIAUpdate;
	    }
    
    if ( iIdArray )
        {
        delete iIdArray;
        iIdArray = NULL;
        }
    // close connection monitor
    if ( iAlwaysOnline )
        {
        iConnectionMonitor.Close();
        }

    // delete KPSUidMuiu property
    RProperty::Delete( KPSUidMuiu, KMuiuKeyCurrentMsg );
    RProperty::Delete( KPSUidMuiu, KMuiuKeyNextMsg );
    if ( iFeatureMgrEnabled ) // CR : 401-1806
        {
    FeatureManager::UnInitializeLib();
		    iFeatureMgrEnabled = EFalse ; // CR : 401-1806
		    }
	if(iEncodeBuffer)
	{
		delete iEncodeBuffer;
	}
	iFs.Close();
		
    MCELOGGER_LEAVEFN("~CMceUi()");
    MCELOGGER_DELETE;
    }

void CMceUi::ConstructL()
    {
    MCELOGGER_CREATE
    MCELOGGER_ENTERFN("ConstructL()");
    MCELOGGER_WRITE_TIMESTAMP("Time: ");

    if ( AknLayoutUtils::MSKEnabled() )
        {
        BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );
        }
    else
        {
        BaseConstructL( EAknEnableSkin | EAknSingleClickCompatible );
        }

    // SIM access profiles
    CheckSIMAccessProfileL();

    // security data caging
    TParse fp;
    fp.Set( KMceDirAndFile, &KDC_APP_BITMAP_DIR , NULL );
    iFilename = fp.FullName();

    FeatureManager::InitializeLibL();
    iFeatureMgrEnabled = ETrue ; // CR : 401-1806

    //check if 'Memory in use' -option is enabled in mce settings
    iMemoryInUse = MceUtils::MemoryInUseOptionL();

    iAlwaysOnline = AlwaysOnlineL();

    iCancelExitFlagOperation = CIdle::NewL( CActive::EPriorityIdle );

    // Are CSP bits supported
    iCsp = CspBitsL();
    iSessionHolder = CMceSessionHolder::NewL( *this );
    iSession = iSessionHolder->Session();

    RFs& fs=iEikonEnv->FsSession();
    TBool storeChanged = EFalse;
    TBool storeOnPhoneMemory = ETrue;
#ifdef RD_MULTIPLE_DRIVE
    TInt phoneDrive;
    // At this point, try avoid using any predefined numbers E.G. EDriveC
    // in order to increase the safety and code flexibility
    User::LeaveIfError( DriveInfo::GetDefaultDrive( DriveInfo:: EDefaultPhoneMemory, phoneDrive ) );
    TInt currentDrive = TInt( iSession->CurrentDriveL() );
    if ( currentDrive != phoneDrive )
#else
    if ( TInt(iSession->CurrentDriveL()) == EDriveE )
#endif //RD_MULTIPLE_DRIVE
        {
        storeOnPhoneMemory = EFalse;
        TBool storeMounted = EFalse;
        TRAPD( error, storeMounted = (*iSession).MessageStoreDrivePresentL( ) );
        TBool driveContainsStore = EFalse;
#ifdef RD_MULTIPLE_DRIVE
        TRAPD( error2, driveContainsStore = iSession->DriveContainsStoreL( currentDrive ) );        
#else
        TRAPD( error2, driveContainsStore = iSession->DriveContainsStoreL( EDriveE ) );
#endif // RD_MULTIPLE_DRIVE
        if ( error || !storeMounted || error2 || !driveContainsStore )
            {
            //change the message store to phone
            storeChanged = ETrue;

            //Change message store to phones internal memory(EDriveC)
            MsvUiServiceUtilitiesInternal::ChangeMessageStoreToPhoneL( *iSession );
            iMceUiFlags.ClearMceFlag( EMceUiFlagsExitOnMsvMediaAvailableEvent );
            }
        }

    iMTMFunctionsArray=new (ELeave) CMsgFuncArray( KMceArrayGranularity );

    iBitmapResolver = CMceBitmapResolver::NewL( iSession, *this, ETrue );

    CMceMainViewListView* mainView = CMceMainViewListView::NewL(
        iSession,
        *iSessionHolder,
        *iBitmapResolver );
    CleanupStack::PushL( mainView );
    AddViewL(mainView);     // transfer ownership to CAknAppUi
    CleanupStack::Pop( mainView );
    iMceMainView = mainView;
    SetDefaultViewL( *iMceMainView );
    SetMceViewActive( EMceMainViewActive );

    iMceLogEngine = CMceLogEngine::NewL();

    CEikStatusPane* sp = StatusPane();
    iNaviPane = (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));

    // iMessageIterator created when opening the message

    // Create IMAP folder indicator

    CMceNaviPaneFolderIndicator* folder =
        CMceNaviPaneFolderIndicator::NewL( storeOnPhoneMemory, 0 );

    CAknNavigationDecorator* tmp = CAknNavigationDecorator::NewL( 
        iNaviPane, folder, CAknNavigationDecorator::ENotSpecified );

    CleanupStack::PushL( tmp );
    tmp->SetContainerWindowL( *iNaviPane );
    CleanupStack::Pop( tmp ); 
    tmp->MakeScrollButtonVisible( EFalse );
    iFolderIndicator = tmp;

    iMailAccountItemArray = new(ELeave) CMceMailAccountItemArray(
        KMceArrayGranularity );

    iMceIAUpdate = NULL;
    iIadUpdateVal  = EFalse;
    if(FeatureManager::FeatureSupported( KFeatureIdIAUpdate ))
        {
        iIadUpdateVal = ETrue;
        }
        
    if ( iAlwaysOnline )
        {
        iConnectionMonitor.ConnectL();
        iConnectionMonitor.NotifyEventL( *this );
        }

    iMceUiFlags.SetMceFlag( EMceUiFlagsFinishedConstruction );

    if ( storeChanged )
        {
        // message store switching ate EMsvServerReady so do it once again,
        // when all has been constructed
        storeChanged = EFalse;
        HandleSessionEventL( MMsvSessionObserver::EMsvServerReady, NULL, NULL, NULL );
        }

    // Contents of uploadservices
    if ( FeatureManager::FeatureSupported(KFeatureIdSenduiMmsUpload) )
        {
        CSendUi* sendui = CSendUi::NewLC( );
        sendui->AvailableServicesL( iUploadServices, KMmsDirectUpload );
        sendui->AvailableServicesL( iUploadServices, KMmsIndirectUpload );
        CleanupStack::PopAndDestroy( sendui );
        }
    
    iAudioMsgEnabled = EFalse ; // CR : 401-1806
    iPostcardEnabled = EFalse ; // CR : 401-1806
    if ( FeatureManager::FeatureSupported( KFeatureIdEmailMceIntegration ) )
        {
        iEmailClientIntegration = ETrue;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdSelectableEmail ) )
        {
        iSelectableEmail = ETrue;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdFfEmailFramework ) )
        {
        iEmailFramework = ETrue;
        }
    iMsgDeletedStatus = EFalse;
    iServerStarted = EFalse ;
    iEmailNotifHandler = NULL;
#ifndef __WINSCW__ 
    // Handling of NCN reset
    TRAPD( err, iEmailNotifHandler = CMceEmailNotifHandler::NewL() );
    MCELOGGER_WRITE_FORMAT("ConstructL iEmailNotifHandler err %d", err);
    if ( err )
        {
        iEmailNotifHandler = NULL;
        }
#endif
    ZoomLevelChangedL( ReadZoomLevelL() );

    iEmailApi = CreateEmailApiL( iSession );

    SetDiskSpaceForMoveOrCopyL();
    //CR:422-271
	//initializing the encodebuffer for MMS templates   
    iEncodeBuffer = CBufFlat::NewL( 1 );//To remove hardcoding
    User::LeaveIfError( iFs.Connect() );
    iFs.SetSessionPath( KRootPath );

    MCELOGGER_LEAVEFN("ConstructL()");
    }

// ----------------------------------------------------
// CMceUi::HandleSessionEventL
// ----------------------------------------------------
void CMceUi::HandleSessionEventL(
    TMsvSessionEvent aEvent,
    TAny* aArg1,
    TAny* aArg2,
    TAny* /*aArg3*/ )
    {
    MCELOGGER_WRITE_TIMESTAMP("CMceUi::HandleSessionEventL() start");
    MCELOGGER_WRITE_FORMAT("aEvent=%d", aEvent);

    if ( !iMceUiFlags.MceFlag( EMceUiFlagsFinishedConstruction ) )
        {
        return;
        }

    TMsvId folderId = KMsvNullIndexEntryId;

    // ConstructL must have left as we have returned to the active scheduler
    // before complete construction
    TInt err;
    switch (aEvent)
        {
    case EMsvServerReady:
    case EMsvCorruptedIndexRebuilt:
        iMceUiFlags.ClearMceFlag( EMceUiFlagsMediaUnavailable );
        TRAP(err, HandleServerStartupL());
        if (err!=KErrNone)
            {
            // close...
            MCELOGGER_WRITE_FORMAT("HandleServerStartupL: %d", err);
            iMceUiFlags.SetMceFlag( EMceUiFlagsWantToExit );
            iCoeEnv->HandleError( err );
            Exit();
            }
         // Messaging application is started to background in bootup.
         // However messaging main view is not activated in background startup.
         // Because of that startup of messaging application after background startup is not as fast as other startups.
         // This can be optimized by activating main view same way than when messaging application is hide to background.
         if ( !IsForeground() )
            {
            HideOrExit();
            }        
        break;
    case EMsvMediaChanged:
    case EMsvMtmGroupInstalled:
    case EMsvMtmGroupDeInstalled:
        iMceUiFlags.ClearMceFlag( EMceUiFlagsMediaUnavailable );
        HandleMTMChangeL();
        break;
    case EMsvEntriesCreated:
    case EMsvEntriesDeleted:
        iMceUiFlags.ClearMceFlag( EMceUiFlagsMediaUnavailable );
        folderId = (*(TMsvId*) (aArg2));
        if ( folderId == KMsvRootIndexEntryId )
            {
            if ( IsPresent( KSenduiMtmSyncMLEmailUid ) )
                {
                // check if syncml item should be added/removed
                // in new message dialog
                SyncMlNewMessageItemL( (CMsvEntrySelection*) aArg1, aEvent );
                }
            // something changed in root i.e. mailbox created or deleted...
            RemoveTabsAndUpdateArray();
            }
        // fall through 
    case EMsvEntriesChanged:
        folderId = (*(TMsvId*) (aArg2));
        if ( folderId == KMsvRootIndexEntryId && iMceUiFlags.MceFlag( EMceUiFlagsTabsActive ) )
            {
            CMsvEntrySelection* selection = (CMsvEntrySelection*) aArg1; // do not take ownership
            const TInt count = selection->Count();
            TInt selectedIndex = KErrNotFound;
            for ( TInt loop = 0; loop < count && selectedIndex == KErrNotFound; loop++ )
                {
                selectedIndex = iTabsArray->Find( selection->At( loop ) );
                }

            if ( selectedIndex != KErrNotFound )
                {
                CheckRemoteMailboxTabIconsL();
                }
            }
        // list update done by active view so do nothing here
        break;
    case EMsvServerFailedToStart:
    case EMsvServerTerminated:
    case EMsvCloseSession:
        iMceUiFlags.SetMceFlag( EMceUiFlagsWantToExit );
        iAvkonViewAppUi->ProcessCommandL( EAknCmdExit );
        break;
    case EMsvGeneralError:
        {
        const TInt leave=*STATIC_CAST(TInt*,aArg1);
        if (iMceUiFlags.MceFlag( EMceUiFlagsWantToExit ) && leave==KLeaveExit)
            {
            User::Leave(leave);
            }
        break;
        }
    case EMsvMediaUnavailable:
        iMceUiFlags.SetMceFlag( EMceUiFlagsMediaUnavailable );
        if ( iMceUiFlags.MceFlag( EMceUiFlagsExitOnMsvMediaAvailableEvent ) )
            {
            MCELOGGER_WRITE("CMceUi::HandleSessionEventL() EMsvMediaUnavailable EXIT");
            iMceUiFlags.SetMceFlag( EMceUiFlagsWantToExit );
            iAvkonViewAppUi->ProcessCommandL( EAknCmdExit );
            }
        else
            {
            MCELOGGER_WRITE("CMceUi::HandleSessionEventL() EMsvMediaUnavailable Clear flag!");
            iMceUiFlags.ClearMceFlag( EMceUiFlagsExitOnMsvMediaAvailableEvent );
            }
        break;
    case EMsvMediaAvailable:
        {
        iMceUiFlags.ClearMceFlag( EMceUiFlagsMediaUnavailable );
        iMceUiFlags.SetMceFlag( EMceUiFlagsExitOnMsvMediaAvailableEvent );
        // update folders tab
        CMceNaviPaneFolderIndicator* findicator =
            static_cast<CMceNaviPaneFolderIndicator*>( iFolderIndicator->DecoratedControl() );
        findicator->ChangeRootL( TInt(iSession->CurrentDriveL() == EDriveC) );
        }
        // fall through 
    default:
        break;
        }
    MCELOGGER_WRITE_TIMESTAMP("CMceUi::HandleSessionEventL() end");
    }

// ----------------------------------------------------
// CMceUi::HandleServerStartupL
// ----------------------------------------------------
void CMceUi::HandleServerStartupL()
    {
    iServerStarted = EFalse ;
    LoadContextIconL();

    if ( iMceUiFlags.MceFlag( EMceUiFlagsServerReady ) )
        {
        MCELOGGER_LEAVEFN("HandleServerStartupL(), server ready");

        if ( iMemoryInUse )
            {
            // when iSession->ChangeDriveL is called, also HandleSessionEvent is called
            // using EMsvServerReady

            if(iMtmLoader)
                {
                iMtmLoader->Cancel();
                delete iMtmLoader;
                iMtmLoader = NULL;
                }

            delete iUiRegistry;
            iUiRegistry=NULL;

            delete iEntry;
            iEntry = NULL;
            iEntry=iSession->GetEntryL(KMsvRootIndexEntryId);

            delete iRootEntry;
            iRootEntry = NULL;
            iRootEntry=CMsvEntry::NewL(
                *iSession,
            KMsvRootIndexEntryId,
            TMsvSelectionOrdering(KMsvNoGrouping, EMsvSortByNone, ETrue));

            delete iMtmStore;
            iMtmStore=NULL;

            iMtmStore=CMtmStore::NewL(*iSession);
            iUiRegistry=CMtmUiDataRegistry::NewL(*iSession);

            iRootEntry->SetEntryL(KMsvRootIndexEntryIdValue);

            // reconstruct main view
            iMceMainView->ConstructMainViewL();

            // reconstruct tabs
            TBool tabsActive = iMceUiFlags.MceFlag( EMceUiFlagsTabsActive );
            RemoveTabs();
            delete iDecoratedTabGroup;
            iDecoratedTabGroup = NULL;
            delete iTabsArray;
            iTabsArray = NULL;

            if ( MceViewActive( EMceMessageViewActive ) 
                && tabsActive 
                && ( !iMceListView->IsImapFolderOpenL( ) ) )
                {
                const TMceListItem& tempItem = iMceMainView
                    ->ListContainer()->CurrentItemListItem();
                TMsvId service;
                TMsvEntry child;
                User::LeaveIfError( iSession->GetEntry( tempItem.iMsvId, service, child ) );
                ShowTabsL( child.Id() );
                }

            // update folders tab
            CMceNaviPaneFolderIndicator* findicator =
                static_cast<CMceNaviPaneFolderIndicator*>( iFolderIndicator->DecoratedControl() );
            findicator->ChangeRootL( TInt(iSession->CurrentDriveL()) );

            iMceMainView->ListContainer()->DrawNow();
            }
        iServerStarted = ETrue ;
        return;
        }

    iEntry=iSession->GetEntryL(KMsvRootIndexEntryId);
    iRootEntry=CMsvEntry::NewL(
        *iSession,
        KMsvRootIndexEntryId,
        TMsvSelectionOrdering(KMsvNoGrouping, EMsvSortByNone, ETrue));
    iMtmStore=CMtmStore::NewL(*iSession);

    iUiRegistry=CMtmUiDataRegistry::NewL(*iSession);

   
    iMceMainView->HandleMsgServerStartupL();
    
    CMceDeliveryReportView* view3 = CMceDeliveryReportView::NewL( *iSessionHolder );
    CleanupStack::PushL( view3 );
    AddViewL( view3 );      // transfer ownership to CAknAppUi
    CleanupStack::Pop( view3 );

    iMceDeliveryView = view3;

     
    CMceMessageListView* mceListView = CMceMessageListView::NewL(
        iSession,
        KMsvLocalServiceIndexEntryId,
        *iSessionHolder,
        *iBitmapResolver,
        *iMceMainView );
    CleanupStack::PushL( mceListView );
    AddViewL( mceListView );      // transfer ownership to CAknAppUi
    CleanupStack::Pop( mceListView );
    iMceListView = mceListView;




    
    iAudioMsgEnabled = FeatureManager::FeatureSupported( KFeatureIdAudioMessaging ); // CR : 401-1806
    iPostcardEnabled = FeatureManager::FeatureSupported( KFeatureIdMmsPostcard ) ; // CR : 401-1806

    HandleMTMChangeL();

/*    CMceMessageListView* mceListView = CMceMessageListView::NewL(
        iSession,
        KMsvLocalServiceIndexEntryId,
        *iSessionHolder,
        *iBitmapResolver,
        *iMceMainView );
    CleanupStack::PushL( mceListView );
    AddViewL(mceListView);      // transfer ownership to CAknAppUi
    CleanupStack::Pop(); // mceListView
    iMceListView = mceListView;*/
    iMceListView->HandleMsgServerStartupL();



    iMceMainView->ListContainer()->ListItems()->SetAlwaysOnline( iAlwaysOnline );

    if ( iAlwaysOnline )
        {
        TRequestStatus status;
        TInt registrationStatus( 0 );

        //check network status
        iConnectionMonitor.GetIntAttribute(
            EBearerIdGSM, 0, KNetworkRegistration,
            registrationStatus, status );

        User::WaitForRequest( status );

        if ( status.Int() == KErrNone )
            {
            iMceMainView->ListContainer()->ListItems()->SetRoaming( 
                registrationStatus == ENetworkRegistrationRoaming );
            }
        }

    iMceUiFlags.ClearMceFlag( EMceUiFlagsWantToExit );
    iMceUiFlags.SetMceFlag( EMceUiFlagsServerReady );

    ToPhoneMemoryQueryL( iMceUiFlags.MceFlag( EMceUiFlagsMainViewActivated ) );

    iCoeEnv->AddForegroundObserverL( *this ); // CR : 401-1806
    //populate MMS templates in inbox which are preloaded KMmsMessageDumpDirectory.
    //CR:422-271
    PopulateMMSTemplates();
    
    iServerStarted = ETrue ;
    MCELOGGER_LEAVEFN("HandleServerStartupL()");
    }

// ----------------------------------------------------
// CMceUi::Session
// ----------------------------------------------------
CMsvSession& CMceUi::Session()
    {
    return *iSession;
    }

// ----------------------------------------------------
// CMceUi::HandleCommandL
// ----------------------------------------------------
void CMceUi::HandleCommandL( TInt aCommand )
    {
    MCELOGGER_WRITE_FORMAT("HandleCommandL: aCommand: %d", aCommand);

    if ( iMceUiFlags.MceFlag( EMceUiFlagsMediaUnavailable ) 
        && aCommand != EEikCmdExit 
        && aCommand != EMceCmdExit 
        && aCommand != EAknSoftkeyBack)
        {
        MCELOGGER_WRITE("HandleCommandL: media unavailable, leave");
        User::Leave( KMsvIndexBackup );
        }

    switch (aCommand)
        {
        case EEikCmdExit:
            Exit();
            break;
        case EMceCmdExit:
            CloseConnectionsBeforeExitL();
            if ( iMceUiFlags.MceFlag( EMceUiFlagsWantToExit ) )
                {
                HideOrExit();
                }
            break;
        case EAknCmdHideInBackground:
            HideOrExit();
            break;
        case EMceCmdNewFolder:
            CreateNewFolderL( );
            break;
        case EMceCmdRenameFolder:
            RenameFolderL();
            break;
        case EMceCmdSettings:
            SettingsDialogL();
            break;
        case EMceCmdDelete:
            HandleDeleteL();
            break;
        case EMceCmdUndelete:
            HandleUndeleteL();
            break;
        case EMceCmdCopy:
            MoveOrCopyEntriesL( ETrue );
            break;
        case EMceCmdMove:
            MoveOrCopyEntriesL( EFalse );
            break;
        case EMceCmdFetchNew:
            FetchNewL();
            break;
        case EMceCmdFetchSelected:
            FetchSelectedL();
            break;
        case EMceCmdFetchAll:
            FetchAllL();
            break;
        case EAknSoftkeyBack:
            RemoveTabs();
            ActivateLocalViewL( KMceMainViewListViewId );
            break;
        case EMceCmdOutboxStart:
            SendNowL();
            break;
        case EMceCmdOutboxSuspend:
            CancelSendingL();
            break;
        case EMceCmdOutboxMoveToDrafts:
            {
            if ( iMceListView->SyncMlOutboxInbox( KMsvGlobalOutBoxIndexEntryId ) )
                {
                SyncMlMoveFromOutboxToDraftsL();
                }
            else
                {
                MoveFromOutboxToDraftsL();
                }
            }
            break;
        case EMceCmdCBSApplication:
            LaunchCBSApplicationL();
            break;
        case EMceCmdUSSDEditor:
            LaunchServiceCommandEditorL();
            break;

        case EAknCmdHelp:
            LaunchHelpL();
            break;

        case EAknCmdOpen:
        case EMceCmdConnect:
        case EMceCmdCloseConnection:
        case EAknCmdUnmark:
        case EAknCmdMark:
        case EAknMarkAll:
        case EAknUnmarkAll:
            // these should be handled by view
            break;

        case EMceCmdReply:
            ReplyL(aCommand);
            break;

        case EMceCmdMarkAsRead:
        case EMceCmdMarkAsReadMsg:
        case EMceCmdMarkAsReadMsgs:
        case EMceCmdMarkAsReadEmails:
            MarkAsReadL();
            break;

        case EMceCmdMarkAsUnread:
        case EMceCmdMarkAsUnreadMsg:
        case EMceCmdMarkAsUnreadMsgs:
        case EMceCmdMarkAsUnreadEmails:
            MarkAsReadL( EFalse );
            break;

        case EMceCmdMailboxSettings:
            OpenMailboxSettingsL();
            break;

        case EMceCmdForward:
            ForwardL(aCommand);
            break;

        case EMceCmdZoomValueAutomatic:
        case EMceCmdZoomValueSmall:
        case EMceCmdZoomValueMedium:
        case EMceCmdZoomValueLarge:
            HandleZoomLevelChangeL( aCommand );
            break;
        case EMceCmdLanuchUniEditor:
            LaunchUniEditorL();
            break;
            
        case EMceCmdLaunchEmailEditor:
            CreateNewMessageL( GetToBeCreatedEmailType () );
            break;

        default:
        if ( aCommand >= EMceCmdFirstMTMFunction && aCommand < EMceCmdFirstMTMContextFunction )
            {
            const TMsgFunctionInfo* info=&iMTMFunctionsArray->At(aCommand-EMceCmdFirstMTMFunction);
            HandleMTMFunctionL(*info);
            }
        else if ( aCommand > EMceCmdCreateNewCommands && aCommand < EMceCmdFirstMTMFunction  )
                {
                const TInt messageIndex =
                    aCommand - EMceCmdCreateNewCommands - 1;
                __ASSERT_ALWAYS( messageIndex < iMsgTypesWriteSubmenu.Count(),
                    Panic( EMceUiErrCreateNewMessageCommand ) );
                CreateNewMessageL( iMsgTypesWriteSubmenu[messageIndex].iUid.iUid );
                }
        break;
        } // switch
    MCELOGGER_LEAVEFN("HandleCommandL()");
    }

// ----------------------------------------------------
// CMceUi::DynInitMenuPaneL
// ----------------------------------------------------
void CMceUi::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    ForceMtmLoaderFinish();
    TBool cbs = EFalse;
    if ( FeatureManager::FeatureSupported( KFeatureIdCellBroadcast ) )
        {
        cbs = ETrue;
        if ( iCsp )
            {
            cbs = CheckCspBitL();
            }
        }
    switch ( aResourceId )
        {
        case R_MCE_MAIN_VIEW_MENU:
            {
            // Run time Cell Broadcast variant.
            // Enable/disable CBS
            aMenuPane->SetItemDimmed( EMceCmdCBSApplication, !cbs );
            // Run time USSD variant.
            aMenuPane->SetItemDimmed( EMceCmdUSSDEditor,
                !FeatureManager::FeatureSupported( KFeatureIdUSSD ));

            aMenuPane->SetItemDimmed( EAknCmdHelp,
                !FeatureManager::FeatureSupported( KFeatureIdHelp ) );

            if ( iMsgTypesWriteSubmenu.Count() == 0 )
                {
                aMenuPane->SetItemDimmed( EMceCmdNewMessage, ETrue );
                }
            if ( MceViewActive( EMceMainViewActive ) )
                {
                AddMTMFunctionsL(*aMenuPane, EMceCmdCloseConnection);

                const CMceMainViewListContainer* listContainer =
                    iMceMainView->ListContainer();
                const TMceListItem listItem =
                    listContainer->CurrentItemListItem();
                if ( listItem.iExtraItem )
                    {
                    aMenuPane->SetItemDimmed( EMceCmdConnect, ETrue );
                    }
                else
                    {
                    const TUid  connectCapability =
                        { KUidMsvMtmUiQueryConnectionOrientedServices };
                    TInt rid;

                    iEntry->SetEntryL( listItem.iMsvId );
                    TUid uid = iEntry->Entry().iMtm;
                    CBaseMtmUiData* uiData = NULL;
                    if ( uid != KUidMsvLocalServiceMtm )
                        {
                        uiData=GetMtmUiDataL( uid );
                        }
                    if ( uiData && ( uid != KSenduiMtmSyncMLEmailUid ) 
                        && uiData->QueryCapability( connectCapability, rid )
                        == KErrNone )
                        {
                        // need to check if connected or not
                        if ( iEntry->Entry().Connected() )
                            {
                            aMenuPane->SetItemDimmed( EMceCmdConnect, ETrue );
                            }
                        }
                    else
                        {
                        aMenuPane->SetItemDimmed( EMceCmdConnect, ETrue );
                        }
                    }
                // if we have at least one connection open then do not delete EMceCmdCloseConnection
                CMsvEntrySelection* connectedAccounts = ConnectedServicesLC();
                if ( connectedAccounts->Count() == 0)
                    {
                    aMenuPane->SetItemDimmed( EMceCmdCloseConnection, ETrue );
                    }
                CleanupStack::PopAndDestroy( connectedAccounts );

                } // if ( MceViewActive( EMceMainViewActive ) )
            break;
            }
        case R_MCE_NEW_MESSAGE_MENU:
            {
            CEikMenuPaneItem::SData item;
            item.iCommandId = EMceCmdCreateNewCommands;
            item.iFlags = 0;
            item.iCascadeId = 0;
            const TInt count = iMsgTypesWriteSubmenu.Count();
            for(TInt loop = 0; loop < count; loop++)
                {
                item.iText = iMsgTypesWriteSubmenu[loop].iName;
                item.iCommandId++;
                aMenuPane->AddMenuItemL(item);
                }
            break;
            }
       case R_MCE_FOLDER_MENU:
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp,
                !FeatureManager::FeatureSupported( KFeatureIdHelp ) );

            AddMTMFunctionsL(*aMenuPane, EAknCmdOpen);
            }
            break;
       case R_MCE_LOCAL_ZOOM_VALUE:
            {
            HandleZoomSubMenu( aMenuPane );
            }
            break;
        default:
            break;
        }// end switch
    }

// ----------------------------------------------------
// CMceUi::SendViaL
// ----------------------------------------------------
void CMceUi::SendViaL( TMsvId aId, TUid aMtm )
    {
    __ASSERT_DEBUG( aMtm == KSenduiMtmIrUid || aMtm == KSenduiMtmBtUid,
    Panic(EMceUiErrCanSendViaOnlyBtOrIr) );

    LeaveIfDiskSpaceUnderCriticalLevelL();

    // Get the file name
    TFileName fileName;
    TInt fileSize;
    RFile fileHandle;
    MceUtils::GetIrFilePathL( *iSession, aId, fileName, fileHandle, fileSize );
    CleanupClosePushL( fileHandle );

    // Required capabilities for message types
    TSendingCapabilities capabilities(
        0,
        0,
        TSendingCapabilities::ESupportsAttachments );

    // Create the message
    CMessageData* messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );
    messageData->AppendAttachmentHandleL( fileHandle );

    // Send the message with SendUI
    CSendUi* sendui = CSendUi::NewLC();
    sendui->CreateAndSendMessageL( aMtm, messageData, KNullUid, ETrue );

    CleanupStack::PopAndDestroy( 3 ); // sendui, messageData, fileHandle 
    }

// ----------------------------------------------------
// CMceUi::HandleKeyEventL
// ----------------------------------------------------
TKeyResponse CMceUi::HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    if ( iEditorOperation )
        {
        MCELOGGER_WRITE_FORMAT("HandleKeyEventL: iEditorOperation set 0x%x so don't do anything", iEditorOperation );
        return EKeyWasConsumed;
        }

    if ( iMceUiFlags.MceFlag( EMceUiFlagsTabsActive ) && aType == EEventKey )
        {
        CAknTabGroup* tabGroup = NULL;
        if ( iDecoratedTabGroup )
            {
            tabGroup = ( CAknTabGroup* ) iDecoratedTabGroup->DecoratedControl();
            }

        if ( !tabGroup )
            {
            return EKeyWasNotConsumed;
            }

        return tabGroup->OfferKeyEventL( aKeyEvent, aType );
        }

    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CMceUi::LogEngine
// ----------------------------------------------------
CMceLogEngine* CMceUi::LogEngine()
    {
    return iMceLogEngine;
    }

// ----------------------------------------------------
// CMceUi::LaunchHelpL
// ----------------------------------------------------
void CMceUi::LaunchHelpL()
    {
    // activate Help application
    CArrayFix<TCoeHelpContext>* helpContext = AppHelpContextL();
    HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
    }

// ----------------------------------------------------
// CMceUi::HandleOpenNextPreviousL
// ----------------------------------------------------
void CMceUi::HandleOpenNextPreviousL( TBool /*aOpenNext*/ )
    {
    MCELOGGER_ENTERFN("HandleOpenNextPreviousL()");
     
    // Read the next message TMsvId using PS key 
    TInt nextEntryId = 0;
    TInt r = RProperty::Get( KPSUidMuiu, KMuiuKeyNextMsg, nextEntryId );
    if ( r != KErrNone )
        {
        nextEntryId = 0;
        }
    
    if ( nextEntryId > 0 )
        {
        // no need of following check because TMsvID is set properly
        //set from MsgEditorAppUI.
  
    
        TBool local = iMceListView->ListContainer()->FolderEntry().iServiceId == KMsvLocalServiceIndexEntryId;
        if ( local && !iLocalScreenClearer ) 
            {
            iLocalScreenClearer = CAknLocalScreenClearer::NewL( ETrue );
            }

        CMceMessageListContainerBase* container = iMceListView->ListContainer();
        
        TMsvId currentItemId = nextEntryId;
        if ( container )
            {
            container->SetCurrentItemIdL( currentItemId );
            container->DrawNow();
            }

        TMsvEntry currentEntry;
        TMsvId serviceId; // not used here but needed by GetEntry function
        if ( iSession->GetEntry( currentItemId, serviceId, currentEntry ) == KErrNone &&
             currentEntry.iType == KUidMsvMessageEntry )
            {
            TRAPD( err, EditMTMEntryL( currentEntry ) );
            if ( err )
                {
                if ( iLocalScreenClearer )
                    {
                    delete iLocalScreenClearer;
                    iLocalScreenClearer = NULL;
                    }
                User::Leave( err );
                }
            }
        } // end  
    MCELOGGER_LEAVEFN("HandleOpenNextPreviousL()");
    }

// ----------------------------------------------------
// CMceUi::LaunchCBSApplicationL
// ----------------------------------------------------
void CMceUi::LaunchCBSApplicationL()
    {
    TVwsViewId id;
    id.iAppUid = KMceCBSApplication;
    id.iViewUid.iUid = KMceCBSApplicationStartView;
    ActivateViewL( id  );
    }

// ----------------------------------------------------
// CMceUi::LaunchServiceCommandEditorL
// ----------------------------------------------------
void CMceUi::LaunchServiceCommandEditorL() const
    {
    CPhCltUssd* ussdClient = CPhCltUssd::NewL( EFalse );
    CleanupStack::PushL( ussdClient );
    ussdClient->StartUssdEditor();
    CleanupStack::PopAndDestroy( ussdClient );
    }

// ----------------------------------------------------
// CMceUi::HandleResourceChangeL
// ----------------------------------------------------
void CMceUi::HandleResourceChangeL( TInt aType )
    {
    if ( !iServerStarted )
        {
    	return ;
        }
        
    CAknViewAppUi::HandleResourceChangeL( aType );
	// this is fix for setting the correct status pane id, if AVKON changes the status pane ids then there will be a problem
    TBool landscape( Layout_Meta_Data::IsLandscapeOrientation() );
    if(!landscape)
    {
    CEikStatusPane* statusPane = StatusPane();
    if(statusPane->CurrentLayoutResId()== R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT)
    {
    statusPane->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL_EXT );
    }
    }
    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        if ( iLocalScreenClearer )
	          {
	          delete iLocalScreenClearer;
	          iLocalScreenClearer = NULL;	
	          iLocalScreenClearer = CAknLocalScreenClearer::NewL( ETrue );
	          }

        if ( MceViewActive( EMceDeliveryReportsViewActive ) )
            {
            iMceDeliveryView->HandleClientRectChange();
            }
        iMceMainView->HandleClientRectChange();
        
        CMceNaviPaneFolderIndicator* findicator =
            static_cast<CMceNaviPaneFolderIndicator*>( iFolderIndicator->DecoratedControl() );
        findicator->HandleResourceChange( aType );
        }

    if( aType == KAknsMessageSkinChange )
        {
        const TInt count = iUiRegistry->NumRegisteredMtmDlls();
        for ( TInt i = 0; i < count; i++ )
            {
            iMtmStore->ReleaseMtmUiData( iUiRegistry->MtmTypeUid(i) );
            }

        iBitmapResolver->ChangeBitmapInIconArrayL();
        if ( !MceViewActive( EMceMainViewActive ) && iMceMainView )
            {
            //update the list icons, when delivery reports view is open and skin changes
            iMceMainView->ListContainer()->HandleResourceChange( aType );
            }
        if ( !MceViewActive(EMceMessageViewActive) && iMceListView 
            && iMceListView->ListContainer() )
            {
            iMceListView->ListContainer()->HandleResourceChange( aType );
            }

        LoadContextIconL();
        TitlePaneL()->DrawDeferred();

        CMceNaviPaneFolderIndicator* findicator =
            static_cast<CMceNaviPaneFolderIndicator*>( iFolderIndicator->DecoratedControl() );
        findicator->HandleResourceChange( aType );

        CAknContextPane* contextPane = (CAknContextPane *)(iEikonEnv->AppUiFactory(*this))
            ->StatusPane()->ControlL(TUid::Uid(EEikStatusPaneUidContext));
        contextPane->DrawDeferred();
        }
    if (iDecoratedTabGroup)
        {
        CAknTabGroup* tab = static_cast<CAknTabGroup*>( iDecoratedTabGroup->DecoratedControl() );
        if ( tab )
            {
            // Inform (possibly off-screen) tab group about the change
            tab->HandleResourceChange( aType );
            }
        }

    iResourceChangeCalled = ETrue;
    }

// ----------------------------------------------------
// CMceUi::CloseEditorApp
// ----------------------------------------------------
void CMceUi::CloseEditorApp()
    {
    if ( iEditorOperation )
        {
        TBool foundItem = EFalse;
        TMsvOp opId = iEditorOperation->Operation().Id();

        for (TInt cc=iOperations.Count()-1; cc>=0 && !foundItem; cc--)
            {
            if (iOperations[cc]->Operation().Id()==opId)
                {
                delete iOperations[cc];
                iOperations.Delete(cc);
                foundItem = ETrue;
                }
            }
        iEditorOperation = NULL;

        delete iLocalScreenClearer;
        iLocalScreenClearer = NULL;
        }
    }

// ----------------------------------------------------
// CMceUi::LoadContextIconL
// ----------------------------------------------------
void CMceUi::LoadContextIconL( )
    {
    CEikStatusPane *sp = ( (CAknAppUi*)iEikonEnv->EikAppUi() )->StatusPane();
    CAknContextPane* contextPane = (CAknContextPane *)sp->ControlL( 
        TUid::Uid(EEikStatusPaneUidContext) );

    RFs& fs=iEikonEnv->FsSession();

    MAknsSkinInstance* skins = AknsUtils::SkinInstance();
    CFbsBitmap* bitmap;
    CFbsBitmap* bitmapMask;

#ifdef RD_MULTIPLE_DRIVE
    TInt phoneDrive;
    // At this point, try avoid using any predefined numbers E.G. EDriveC
    // in order to increase the safety and code flexibility
    User::LeaveIfError( DriveInfo::GetDefaultDrive( DriveInfo:: EDefaultPhoneMemory, phoneDrive ) );
    if ( TInt( iSession->CurrentDriveL() ) != phoneDrive )
#else
    if ( TInt( iSession->CurrentDriveL()) == EDriveE )
#endif //RD_MULTIPLE_DRIVE
        {
#ifndef __SCALABLE_ICONS
        AknsUtils::CreateIconL( skins, KAknsIIDQgnMenuMceMmcCxt, bitmap,
           bitmapMask, iFilename, EMbmMuiuQgn_menu_mce_mmc_cxt,
           EMbmMuiuQgn_menu_mce_mmc_cxt + 1 );
#else
        AknsUtils::CreateIconL( skins, KAknsIIDQgnMenuMceMmcCxt, bitmap,
           bitmapMask, iFilename, EMbmMuiuQgn_menu_mce_mmc,
           EMbmMuiuQgn_menu_mce_mmc + 1 );
#endif
        }
    else
        {
#ifndef __SCALABLE_ICONS
        AknsUtils::CreateIconL( skins, KAknsIIDQgnMenuMceCxt, bitmap,
           bitmapMask, iFilename, EMbmMuiuQgn_menu_mce_cxt,
           EMbmMuiuQgn_menu_mce_cxt + 1 );
#else
        AknsUtils::CreateAppIconLC( skins, KMceApplicationUid, 
            EAknsAppIconTypeContext, bitmap, bitmapMask );
        CleanupStack::Pop( 2 ); // Both bitmaps are left in the cleanup stack
#endif
        }
    contextPane->SetPicture( bitmap, bitmapMask );
    }

// ----------------------------------------------------
// CMceUi::ResourceChangeCalled
// ----------------------------------------------------
TBool CMceUi::ResourceChangeCalled()
    {
    return iResourceChangeCalled;
    }

// ----------------------------------------------------
// CMceUi::ResetResourceChange
// ----------------------------------------------------
void CMceUi::ResetResourceChange()
    {
    iResourceChangeCalled = EFalse;
    }

// ----------------------------------------------------
// CMceUi::CheckTechnologyTypeFromMtm
// ----------------------------------------------------
TBool CMceUi::CheckTechnologyTypeFromMtm( const TUid aTechnologyType, TUid aMtmType )
    {
    TBool isMailMtm = EFalse;
    if ( aMtmType.iUid != 0 && aMtmType != KUidMsvLocalServiceMtm &&
         iUiRegistry->IsPresent( aMtmType ) )
    {
    // get MTM technology type
    TUid technologyType = iUiRegistry->TechnologyTypeUid( aMtmType );
        isMailMtm = ( aTechnologyType == technologyType );
        }
    return isMailMtm;
    }





// ----------------------------------------------------
// CMceUi::HandleMTMChangeL
// ----------------------------------------------------
void CMceUi::HandleMTMChangeL()
    {
    MCELOGGER_ENTERFN("HandleMTMChangeL()");
    if( !iMtmLoader )
        {
        iMtmLoader = new(ELeave) CMceIdleMtmLoader(
            *iMtmStore,
            iMsgTypesWritePopup,
            iMsgTypesWriteSubmenu,
            iMsgTypesSettings,
            *iMTMFunctionsArray,
            *iUiRegistry );
        }
    else
        {
        iMtmLoader->Cancel();
        }

    iMtmLoader->StartL(*iSession, iAudioMsgEnabled, iPostcardEnabled); // CR : 401-1806
    MCELOGGER_LEAVEFN("HandleMTMChangeL()");
    }

// ----------------------------------------------------
// CMceUi::GetMtmUiDataL
// will return NULL if no uiData found for entryType
// ----------------------------------------------------
CBaseMtmUiData* CMceUi::GetMtmUiDataL(TUid aMtm)
    {
    MCELOGGER_ENTERFN("GetMtmUiDataL()");
    MCELOGGER_WRITE_FORMAT("GetMtmUiDataL: mtm = 0x%x", aMtm.iUid );

    CBaseMtmUiData* uiData=NULL;
    if (iMtmStore)
        {
        TRAPD(error, ( uiData=&(iMtmStore->MtmUiDataL(aMtm)) ) );
        MCELOGGER_WRITE_FORMAT("GetMtmUiDataL: error = %d", error );
        if ( iMceUiFlags.MceFlag( EMceUiFlagsServerReady )  && error != KErrNotFound )
            {
            // KErrNotFound cover case where MTM is not installed but messages exist
            User::LeaveIfError( error );
            }
        }
    MCELOGGER_LEAVEFN("GetMtmUiDataL()");
    return uiData;
    }

// ----------------------------------------------------
// CMceUi::CanCreateNewAccountL
// ----------------------------------------------------
TBool CMceUi::CanCreateNewAccountL(TUid aMtm)
    {
    TBool canCreateNewAccount = EFalse;
    TMsvEntry serviceEntry;
    serviceEntry.iType=KUidMsvServiceEntry;
    serviceEntry.iMtm=aMtm;

    TInt rid;
    CBaseMtmUiData* uiData=GetMtmUiDataL(aMtm);
    if ( uiData && uiData->CanCreateEntryL( iRootEntry->Entry(), serviceEntry, rid ) )
        {
        canCreateNewAccount = ETrue;
        }
    return canCreateNewAccount;
    }

// ----------------------------------------------------
// CMceUi::DeleteAccountL
// ----------------------------------------------------
void CMceUi::DeleteAccountL(TMsvId aId)
    {
    TMsvEntry   entry;
    GetEntryL( aId, entry );
    const TUid mtm = entry.iMtm;
    CBaseMtmUi* ui = &iMtmStore->ClaimMtmUiL( mtm );
    if (ui)
        {
        CMtmStoreMtmReleaser::CleanupReleaseMtmUiLC(*iMtmStore, mtm);
        }

    CMsvOperation* op = NULL;
    __ASSERT_DEBUG( !iDeleteOperation,
        Panic( EMceUiErrAlreadyDeletingAccount ) );

// Free disk space request for delete to be added

    CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( singleOpWatcher );

    if (ui)
        {
        ui->BaseMtm().SetCurrentEntryL(iSession->GetEntryL(KMsvRootIndexEntryId));
        CAknInputBlock::NewLC();
        op=ui->DeleteServiceL(entry, singleOpWatcher->iStatus);
        CleanupStack::PopAndDestroy(); // CAknInputBlock
        CleanupStack::PushL(op);
        }
    else
        {
        // could not load mtm, so delete as normal local entry
        iEntry->SetEntryL( KMsvRootIndexEntryId );

        CMsvProgressReporterOperation* progOp =
            CMsvProgressReporterOperation::NewL(
            *iSession, singleOpWatcher->iStatus, EMbmAvkonQgn_note_erased);
        CleanupStack::PushL( progOp );

        CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection();
        CleanupStack::PushL( selection );
        selection->AppendL( entry.Id() );
        CMsvOperation* subOp = iEntry->DeleteL( *selection, progOp->RequestStatus());
        CleanupStack::PopAndDestroy( selection );
        progOp->SetOperationL( subOp ); // this takes ownership immediately, so no cleanupstack needed.
        op = progOp;
        }

    iOperations.AppendL( singleOpWatcher );
    CleanupStack::Pop( 2 ); // singleOpWatcher, op
    singleOpWatcher->SetOperation( op );
    iDeleteOperation = singleOpWatcher;
    iMailboxDeleteOperation = op->Id();
    MCELOGGER_WRITE_FORMAT("free disk requested, iDeleteOperation set 0x%x", iDeleteOperation );
    MCELOGGER_WRITE_FORMAT("iOperations.AppendL(%d)", op->Id() );

// Cancel the free disk space request to be added

    if ( ui )
        {
        CleanupStack::PopAndDestroy();// release mtmUi
        }

    }

// ----------------------------------------------------
// CMceUi::SettingsDialogL
// ----------------------------------------------------
void CMceUi::SettingsDialogL()
    {
    CMceUidNameArray* tempArray = CMceUidNameArray::NewL();

    if (( iSelectableEmail )&& (!iEmailFramework))
        {
        CleanupStack::PushL( tempArray );
        const TInt count = iMsgTypesSettings.Count();

        // get all email accounts (visible)
        CArrayFix<TUid>* listItemUids =
            new ( ELeave ) CArrayFixFlat<TUid>( KMceArrayGranularity );
        CleanupStack::PushL(listItemUids);
        CMsvEntry* entry = iSession->GetEntryL( KMsvRootIndexEntryId );
        CleanupStack::PushL( entry );

        TInt canCreateNew;   

        TInt cnt = entry->Count();
        if ( cnt != 0 )
            {
            entry->SetSortTypeL( TMsvSelectionOrdering(
                KMsvGroupByType | KMsvGroupByStandardFolders, EMsvSortByDetailsReverse, ETrue ) );

            // Add default item first
            for ( TInt cc = entry->Count(); --cc >= 0; )
                {
                TMsvEntry tentry = (*entry)[cc];                

                if ( tentry.iType.iUid == KUidMsvServiceEntryValue &&
                     tentry.Id() != KMsvLocalServiceIndexEntryIdValue )
                    {
                    const TBool noRelatedId = tentry.iRelatedId == KMsvNullIndexEntryId || tentry.iRelatedId == tentry.Id();

                    TBool appendEmail = EFalse;

                    if( tentry.Visible() )
                        {
                        appendEmail = ETrue;
                        }
                    else
                        {
                        CMtmUiDataRegistry* uiRegistry = CMtmUiDataRegistry::NewL( *iSession );                
                        CleanupStack::PushL(uiRegistry);
                        if ( uiRegistry->IsPresent( tentry.iMtm ) &&
                             uiRegistry->IsPresent( KUidMsgTypePOP3 ) )
                            {            
                            TUid mailMTMTechType = uiRegistry->TechnologyTypeUid( KUidMsgTypePOP3 );                                          
                            if( uiRegistry->TechnologyTypeUid( tentry.iMtm ) ==
                                mailMTMTechType )
                                {
                                appendEmail = ETrue;                    
                                }
                            }
                            CleanupStack::PopAndDestroy( uiRegistry );
                        } 

                    if ( appendEmail )
                        {                         
                        TBool append = ETrue; 
                        // query capability (new message)
                        TUid entryUid;

                        if ( tentry.iMtm == KSenduiMtmPop3Uid ||
                             tentry.iMtm == KSenduiMtmImap4Uid)
                            {                        
                            entryUid = KSenduiMtmSmtpUid;
                            }
                        else
                            {
                            entryUid = tentry.iMtm;     
                            }

                        CBaseMtmUiData* mtmUiData = NULL;

                        TRAPD( returnVal, mtmUiData =
                               &( iMtmStore->MtmUiDataL( entryUid ) ) );

                        if ( returnVal == KErrNone)
                            {
                            TInt ret = mtmUiData->QueryCapability( KUidMtmQueryCanSendMsg, canCreateNew );                                   

                            if ( ret == KErrNone ) 
                                {
                                append = ETrue;
                                }
                            else
                                {
                                append = EFalse;    
                                }                
                            }
                        else
                            {
                            append = EFalse;                                
                            }                            

                        iMtmStore->ReleaseMtmUiData( entryUid );                  

                        if( append )
                            {                     
                            listItemUids->AppendL( entryUid );
                            }
                        }
                    }
                }
            }
        TBool emailIncluded = EFalse;        
        TBool syncMLIncluded = EFalse;        
        TUid uidMsgValTypeEmailMtmVal = GetFsIntegratedEmailAppMtmPluginId(); 
        CDesCArrayFlat* items = new(ELeave)CDesCArrayFlat( KMceArrayGranularity );
        CleanupStack::PushL( items );
        for ( TInt loop = 0; loop < count ; loop++)
            {
            if ( FindMtmInVisibleListL( iMsgTypesSettings[loop].iUid, *listItemUids ) )
                {
                if ( !emailIncluded &&
                    ( iMsgTypesSettings[loop].iUid == KSenduiMtmSmtpUid ) ) 
                    {
                    tempArray->AppendL( iMsgTypesSettings[loop] );
                    emailIncluded = ETrue;
                    }                
                // Following is added because SyncML was missing from the list
                else if ( !syncMLIncluded &&
                          ( iMsgTypesSettings[loop].iUid == KSenduiMtmSyncMLEmailUid ) )
                    {
                    tempArray->AppendL( iMsgTypesSettings[loop] );
                    syncMLIncluded = ETrue;					
                    }
	        else if(iMsgTypesSettings[loop].iUid!=uidMsgValTypeEmailMtmVal)
                    {
                    tempArray->AppendL( iMsgTypesSettings[loop] );
                    }
                }
            else
                {
                tempArray->AppendL( iMsgTypesSettings[loop] );    
                }        
            }
            CleanupStack::PopAndDestroy( items ); 
            CleanupStack::PopAndDestroy( entry );
            CleanupStack::PopAndDestroy( listItemUids );
        }
    else if(iEmailFramework)  //Freestyle Feature is disabled and CMail Feature is enabled 
        {
        MCELOGGER_WRITE("CMceUi::SettingsDialogL: cmail ");
        CleanupStack::PushL( tempArray );
        const TInt count = iMsgTypesSettings.Count();
        for (TInt loop=0; loop < count; loop++)
            {
            if ( (iMsgTypesSettings[loop].iUid == KSenduiMtmSmtpUid ) ||
                    ( iMsgTypesSettings[loop].iUid == KSenduiMtmSyncMLEmailUid )||
                    (iMsgTypesSettings[loop].iUid == KUidMsgTypeFsMtmVal))
                {
                MCELOGGER_WRITE_FORMAT("not Loading %x", iMsgTypesSettings[loop].iUid );
                continue;
                }
            else
                {
                MCELOGGER_WRITE_FORMAT(" Loading %x", iMsgTypesSettings[loop].iUid);
                tempArray->AppendL( iMsgTypesSettings[loop] );
                }
            }
        }
    
    else    //Freestyle Feature is disabled and CMail Feature is disabled 
            
       {

    CleanupStack::PushL( tempArray );
    const TInt count = iMsgTypesSettings.Count();
    for (TInt loop=0; loop < count; loop++)
        {
        tempArray->AppendL( iMsgTypesSettings[loop] );
        }
		}

    TUid uid;
    uid.iUid = KMceAnswerToLifeUniverseAndEverything;
    // this is not used anywhere, two last items in the list are
    // ALWAYS cbs settings and mce general settings

    // Run time Cell Broadcast variant.
    TBool cbs = EFalse;
    if ( FeatureManager::FeatureSupported( KFeatureIdCellBroadcast ) )
        {
        cbs = ETrue;
        if ( iCsp )
            {
            cbs = CheckCspBitL();
            }
        }
    if ( cbs )
        {
        HBufC* text = StringLoader::LoadL( R_MCE_CBS_SETTINGS_TEXT, iEikonEnv );
        TUidNameInfo cbsSettings( uid, *text );
        delete text;
        tempArray->AppendL( cbsSettings );
        }

    HBufC* text = StringLoader::LoadL( R_MCE_GENERAL_SETTINGS_TEXT, iEikonEnv );
    TUidNameInfo generalSettings( uid, *text );
    delete text;
    tempArray->AppendL( generalSettings );

    // This is changed to automatic because local zoom should change only listbox zoom value
    // so settings dialog zoom value should come from general settings and so we need to change
    // local zoom to automatic before launching settings dialog
    // Is settings opened from General Settings
    CMuiuLock* repositoryLock = CMuiuLock::NewL( KMuiuLockSettings );
    CleanupStack::PushL(repositoryLock);
    TInt err = repositoryLock->Reserve();

    // Check that settings are not opened from General Settings
    if ( !err )
        {
        TAknUiZoom previous = ZoomLevelChangedL( EAknUiZoomAutomatic );
        iMceUiFlags.SetMceFlag( EMceUiFlagsSettingsDialogOpen );
        CMceSettingsDialog* dlg = CMceSettingsDialog::NewL( *tempArray, this, iSession);
        dlg->ExecuteLD();
        iMceUiFlags.ClearMceFlag( EMceUiFlagsSettingsDialogOpen );
        ZoomLevelChangedL( previous );
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
    CleanupStack::PopAndDestroy( 2 ); // tempArray, repositoryLock

    }

// ----------------------------------------------------
// CMceUi::CreateNewMessageL
// Create a new message of the given type, in the given location
// ----------------------------------------------------
void CMceUi::CreateNewMessageL(TInt aMessageType)
    {
    __ASSERT_DEBUG( aMessageType != KUidMsvLocalServiceMtmValue,
        Panic( EMceUiErrLocalMtm ) );
    LeaveIfDiskSpaceUnderCriticalLevelL();
    TUid messageTypeOrig;
    messageTypeOrig.iUid = aMessageType;
    TUid msgTypeUid; 
    msgTypeUid.iUid = 0;

    TBool tabsToCleanupStack = EFalse;
    TMsvId service = KMsvNullIndexEntryId;
    if ( MceViewActive( EMceMessageViewActive ) && iMceListView )
        {
        const TMsvEntry& entry = iMceListView->ListContainer()->FolderEntry();
        //cmail change
        if ( (iSelectableEmail) && (!iEmailFramework ))
            {
            if ( entry.iMtm == KSenduiMtmImap4Uid ||
                entry.iMtm == KSenduiMtmPop3Uid )
            {
            service = entry.iServiceId;
            }
            }
        else
            {
	        if ( aMessageType == entry.iMtm.iUid ||
	            ( aMessageType == KSenduiMtmSmtpUidValue
	               && (entry.iMtm == KSenduiMtmImap4Uid
	               || entry.iMtm == KSenduiMtmPop3Uid ) ) )
            {
            service = entry.iServiceId;
            }
        }
        }


    if ( service == KMsvNullIndexEntryId )
        {
        //cmail change
        if ( (iSelectableEmail) && (!iEmailFramework ) )
            {
            if ( GetDefaultServiceL( aMessageType, service, msgTypeUid )
        	     == EFalse )        
                {
                return;
                }
            }
        else
            {
        GetDefaultServiceL( aMessageType, service );
        }
        }    

    if ( ((iSelectableEmail) && (!iEmailFramework ))&& ( messageTypeOrig == KSenduiMtmSmtpUid ) )
        {	
        if( ( msgTypeUid == KSenduiMtmSmtpUid) ||
            ( msgTypeUid.iUid == KMsvNullIndexEntryId ) )
            {
    msgTypeUid.iUid = aMessageType;
            }        
        }
    else
        {
        msgTypeUid.iUid = aMessageType;            
        } 

    if ( iMceUiFlags.MceFlag( EMceUiFlagsTabsActive ) )
        {
        // must remove tabs because if no mailboxes created then
        // mailbox settings dialog is displayed and tabs should not be shown then.
        RemoveTabs();
        CleanupStack::PushL( TCleanupItem( ShowTabs, this ) );
        tabsToCleanupStack = ETrue;
        }

    CreateAndEditNewEntryL(
        KUidMsvMessageEntryValue,
        msgTypeUid,
        KMsvDraftEntryId,
        service );

    if ( tabsToCleanupStack )
        {
        CleanupStack::PopAndDestroy(); // TCleanupItem - will show tabs again
        }

        // NCN reset does not requied for new email creation
    

    }

// ----------------------------------------------------
// CMceUi::ShowTabs
// Static function.
// Used in CreateNewMessageL as TCleanupItem.
// ----------------------------------------------------
void CMceUi::ShowTabs(TAny* aAny)
    {
    CMceUi* mceUi = (CMceUi*) aAny;
    if ( mceUi && mceUi->MceViewActive( EMceMessageViewActive ) && mceUi->iMceListView )
        {
        TRAP_IGNORE( mceUi->ShowTabsL( mceUi->iMceListView->ListContainer()->CurrentFolderId() ) );
        }
    }

// ----------------------------------------------------
// CMceUi::ResetEditorOperation
// Static function.
// Used in CreateNewMessageL as TCleanupItem.
// ----------------------------------------------------
void CMceUi::ResetEditorOperation(TAny* aAny)
    {
    CMceUi* mceUi = (CMceUi*) aAny;
    MCELOGGER_WRITE_FORMAT("ResetEditorOperation iEditorOperation 0x%x set NULL", mceUi->iEditorOperation );
    mceUi->iEditorOperation = NULL;
    }

// ----------------------------------------------------
// CMceUi::CreateAndEditNewEntryL
// ----------------------------------------------------
void CMceUi::CreateAndEditNewEntryL(TMsvId aParentId, TMsvEntry& aNewEntry)
    {
    MCELOGGER_ENTERFN("CreateAndEditNewEntryL()");

    LeaveIfDiskSpaceUnderCriticalLevelL();

    __ASSERT_DEBUG(aNewEntry.iMtm != KUidMsvLocalServiceMtm,
        Panic( EMceUiErrLocalMtm ) );

    aNewEntry.iDate.HomeTime();

    // --- Get the MTM UI relevant to the message type ---
    CBaseMtmUi& mtmUi = iMtmStore->GetMtmUiLC( aNewEntry.iMtm );
    CMsvEntry* centry = iSession->GetEntryL( aParentId );
    // this allocates memory so cleanupstack is needed
    CleanupStack::PushL( centry );
    CAknInputBlock::NewLC();

    CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL(*this);
    CleanupStack::PushL( singleOpWatcher );

    CleanupStack::PushL( TCleanupItem( ResetEditorOperation, this ) );

    mtmUi.SetPreferences( mtmUi.Preferences() | EMtmUiFlagEditorPreferEmbedded );
    
    CMsvOperation* op = mtmUi.CreateL(
        aNewEntry,
        *centry,
        singleOpWatcher->iStatus );

    MCELOGGER_WRITE_FORMAT("CreateAndEditNewEntryL: iEditorOperation set 0x%x", singleOpWatcher );
    iEditorOperation = singleOpWatcher;

    CleanupStack::PushL( op );
    MCELOGGER_WRITE_FORMAT("iOperations.AppendL(%d)", op->Id() );
    iOperations.AppendL( singleOpWatcher );
    CleanupStack::Pop( op ); // singleOpWatcher, op
    CleanupStack::Pop(); // ResetEditorOperation everything ok, don't reset iEditorOperation
    CleanupStack::Pop( singleOpWatcher );

    singleOpWatcher->SetOperation( op );

    if ( aNewEntry.iType == KUidMsvMessageEntry &&
        iUiRegistry &&
        iUiRegistry->IsPresent( op->Mtm() ) )
        {
        iMtmStore->ClaimMtmUiL( op->Mtm() );            
        }

    CleanupStack::PopAndDestroy(3); // CAknInputBlock, centry, release MtmUi

    if ( MceViewActive( EMceMessageViewActive ) )
        {
        iMceListView->ListContainer()->ClearSelection();
        }

    MCELOGGER_LEAVEFN("CreateAndEditNewEntryL()");
    }

// ----------------------------------------------------
// CMceUi::CreateAndEditNewEntryL
// ----------------------------------------------------
void CMceUi::CreateAndEditNewEntryL(
    TInt aEntryType,
    TUid aMtm,
    TMsvId aParentId,
    TMsvId aServiceId )
    {
    __ASSERT_DEBUG(aMtm != KUidMsvLocalServiceMtm,
        Panic( EMceUiErrLocalMtm ) );
    TMsvEntry nentry;
    nentry.iMtm = aMtm;
    nentry.iType.iUid = aEntryType;
    nentry.iDate.HomeTime();
    nentry.iServiceId = aServiceId;
    CreateAndEditNewEntryL( aParentId, nentry );
    }

// ----------------------------------------------------
// CMceUi::GetDefaultServiceL
// when MceIntegration runtime flag is on
// ----------------------------------------------------
TBool CMceUi::GetDefaultServiceL( TInt aMtm, TMsvId& aService, TUid& aMtmUid )
    {
    TBool retVal = ETrue;
    TUid mtm = { aMtm };       

    aService = MsvUiServiceUtilities::DefaultServiceForMTML( *iSession,
                                                             mtm,
                                                             ETrue );

    if ( mtm == KSenduiMtmImap4Uid || mtm == KSenduiMtmPop3Uid
            || mtm == KSenduiMtmSmtpUid )
        {
        retVal = CMceSettingsEmailSel::ShowSelectEmailDlgL( aService,  aMtmUid );
        }

    return retVal;
    }

// ----------------------------------------------------
// CMceUi::GetDefaultServiceL
// ----------------------------------------------------
void CMceUi::GetDefaultServiceL(TInt aMtm, TMsvId& aService)
    {
    TUid mtm = { aMtm };

    aService = MsvUiServiceUtilitiesInternal::DefaultServiceForMTML(
        *iSession,
        mtm,
        ETrue );
    }

// ----------------------------------------------------
// CMceUi::OpCompleted
// ----------------------------------------------------
void CMceUi::OpCompleted(
    CMsvSingleOpWatcher& aOpWatcher,
    TInt aCompletionCode )
    {
    MCELOGGER_ENTERFN("OpCompleted()");
    CMsvOperation* op=&aOpWatcher.Operation();
    MCELOGGER_WRITE_FORMAT("operation id=%d completed", op->Id() );

    // Store the command ID
    TMsvOp tempOpID = op->Id();
    TUid tempOpMtm = op->Mtm();
    
    if ( &aOpWatcher == iEditorOperation )
        {
        MCELOGGER_WRITE_FORMAT("iEditorOperation 0x%x set NULL", iEditorOperation );
        iEditorOperation = NULL;
        if ( iMtmStore && //tempOpMtm != KUidMsvLocalServiceMtm && tempOpMtm != KUidMsvNullEntry &&
            iUiRegistry &&
            iUiRegistry->IsPresent( tempOpMtm ) )
            {
            iMtmStore->ReleaseMtmUi( tempOpMtm );                
            }
        }
    
#ifdef _DEBUG
    TRAPD(ignore, DoOperationCompletedL(op, aCompletionCode));
    MCELOGGER_WRITE_FORMAT("DoOperationCompletedL ignore %d", ignore );
#else
    TRAP_IGNORE(DoOperationCompletedL(op, aCompletionCode));
#endif

    ClearDontExitOnNextOperationComplete();
    
    TBool foundItem = EFalse;
    for (TInt cc = iOperations.Count()-1; cc >= 0 && !foundItem; cc--)
        {
        if (iOperations[cc]->Operation().Id()==op->Id())
            {
            delete iOperations[cc];
            iOperations.Delete(cc);
            foundItem = ETrue;
            }
        }

    if ( &aOpWatcher == iDeleteOperation )
        {
        MCELOGGER_WRITE_FORMAT("iDeleteOperation 0x%x set NULL", iDeleteOperation );
        iDeleteOperation = NULL;

        if ( iMMSNotifications && iMMSNotifications->Count() > 0 )
            {
            //delete MMS notifications
            TBuf8<1> params;
            params.Append( KMceMMSNotifOne8 );
            TRAPD( error, HandleMMSNotificationsDeleteL( params ) );
            if ( error )
                {
                if ( !iMceErrorUi )
                    {
                    TRAP_IGNORE( iMceErrorUi = CErrorUI::NewL(*iCoeEnv) );
                    }
                if ( iMceErrorUi )
                    {
                    TRAP_IGNORE( iMceErrorUi->ShowGlobalErrorNoteL( error ) );
                    }
                // ignore
                }
            }

// Cancel free disk space request to be added

        }

    else if ( iMMSNotifications
        && ( tempOpMtm == KUidMsgMMSNotification ) )
        {
        // check if MMS notification delete failed
        TRAP_IGNORE( CheckMMSNotifDelFailedL() );
        }
    else
        {
        //nothing special done
        }


    if ( &aOpWatcher == iFetchNewOperation )
        {        
        iFetchNewOperation = NULL;
    // TODO
        if ( ( iMceListView->ListContainer()->Count() )
            && MceViewActive( EMceMessageViewActive ) )
            {        
//            iMceListView->ListContainer()->ListBox()->SetCurrentItemIndex( 0 );
//            iMceListView->ListContainer()->ListBox()->DrawDeferred();
            }
        }

// Email messages cannot be copied or moved, show the note
    if ( ( iMoveOrCopyMailOperation != -1 ) && ( iMoveOrCopyMailOperation == tempOpID ) )
        {
        TRAP_IGNORE( InformationNoteCannotMoveCopyEMailL() );
        }

    if ( iHandleNewMsgToInbox )
        {
        TRAP_IGNORE( HandleNewMsgToInboxL( EFalse, NULL ) );
        }

    __ASSERT_DEBUG( foundItem, Panic( EMceUiErrOperationNotFound ) );
    MCELOGGER_LEAVEFN("OpCompleted()");
    }

// ----------------------------------------------------
// CMceUi::DoOperationCompletedL
// ----------------------------------------------------
void CMceUi::DoOperationCompletedL(
    CMsvOperation* aOperation,
    TInt aCompletionCode )
    {
    __ASSERT_DEBUG( aOperation, Panic( EMceUiErrOperationNull ) );
    if ( !aOperation )
        {
        return;
        }

    if ( iLocalScreenClearer && 
         aCompletionCode != CMsgEmbeddedEditorWatchingOperation::EMsgExitNext &&
         aCompletionCode != CMsgEmbeddedEditorWatchingOperation::EMsgExitPrevious )
        {
        
        delete iLocalScreenClearer;
        iLocalScreenClearer = NULL;
        
        }

    MCELOGGER_WRITE_FORMAT("CMceUi::DoOperationCompletedL: aCompletionCode: %d", aCompletionCode );
    
    if ( aOperation->Id() == iMailboxDeleteOperation )
        {
        CMceMainViewListItemArray* array = iMceMainView->ListContainer()->ListItems();
        if ( array )
            {
            array->HandleSessionEventL(EMsvEntriesDeleted, NULL, NULL, NULL );
            }
        }

    if ( ViewUpdateSuppressionFlag() && iBlockingOperation == aOperation )
        {
        SetViewUpdateSuppressionFlag( EFalse, NULL );
        iMceListView->UpdateViewL();
        }

    TInt closedEntryId = 0;
    // Retrieve value of KMuiuKeyCurrentMsg
    TInt r = RProperty::Get( KPSUidMuiu, KMuiuKeyCurrentMsg, closedEntryId );
    if ( r != KErrNone )
        {
        closedEntryId = 0;
        }
    if ( aCompletionCode == EEikCmdExit &&
        !iMceUiFlags.MceFlag( EMceUiFlagsDoNotExitAfterNextOperationComplete ) )
        {
        MCELOGGER_WRITE("CMceUi::DoOperationCompletedL: EXIT");
        iAvkonViewAppUi->ProcessCommandL( EAknCmdHideInBackground );
        return;
        }
    // viewer next/prev
    else if ( aCompletionCode == CMsgEmbeddedEditorWatchingOperation::EMsgExitNext ||
        aCompletionCode == CMsgEmbeddedEditorWatchingOperation::EMsgExitPrevious )
        {
        MCELOGGER_WRITE("CMceUi::DoOperationCompletedL: Opening next/previous");
        HandleOpenNextPreviousL( aCompletionCode ==
            CMsgEmbeddedEditorWatchingOperation::EMsgExitNext );
        if ( !IsForeground() )
            {
             // bring mce application to foreground
            iEikonEnv->RootWin().SetOrdinalPosition(0);
            }
        return;
        }
    else
        {
        // Viewer closed
        if ( closedEntryId > 0 )
            {
            CMceMessageListContainerBase* container = iMceListView->ListContainer();
            if ( container )
                {
                container->SetCurrentItemIdL( closedEntryId );
                }
            }
         }

    if ( !iMceErrorUi )
        {
        iMceErrorUi = CErrorUI::NewL(*iCoeEnv);
        }

    if ( aCompletionCode == KMsvIndexRestore )
        {
        iMceErrorUi->ShowGlobalErrorNoteL( aCompletionCode );
        }

    const TDesC8& progress = aOperation->ProgressL();

    if (aOperation->Mtm() != KUidMsvLocalServiceMtm )
        {
        MCELOGGER_WRITE_FORMAT("CMceUi::DoOperationCompletedL: calling mtm ui 0x%x DisplayProgressSummary", aOperation->Mtm().iUid );
        CAknInputBlock::NewLC();
        CMtmStore* mtmStore=CMtmStore::NewL(*iSession);
        CleanupStack::PushL( mtmStore );
        
        CBaseMtmUi& ui = mtmStore->GetMtmUiLC( aOperation->Mtm() );
        if ( aOperation->Service() > KFirstFreeEntryId
            && ( aOperation->Mtm() == KSenduiMtmImap4Uid
            || aOperation->Mtm() == KSenduiMtmPop3Uid ) )
            {
            ui.BaseMtm().SwitchCurrentEntryL( aOperation->Service() );
            }
        ui.DisplayProgressSummary( progress );
        CleanupStack::PopAndDestroy( 3 ); // CAknInputBlock, mtmStore, release mtmUi        
        MCELOGGER_WRITE("DisplayProgressSummary done");
        return;
        }

    if (progress.Length() == 0)
        {
        return;
        }

    TMsvLocalOperationProgress localprogress =
        McliUtils::GetLocalProgressL( *aOperation );

    if ( ( aCompletionCode == KErrNone
        || aCompletionCode == KErrCancel )
        && ( localprogress.iError == KErrNone
        || localprogress.iError == KErrCancel ) )
        {
        return;
        }

    MCELOGGER_WRITE_FORMAT("CMceUi::DoOperationCompletedL, localProgress.iType %d", localprogress.iType )
    MCELOGGER_WRITE_FORMAT("error %d", localprogress.iError?localprogress.iError:aCompletionCode )

    iMceErrorUi->ShowGlobalErrorNoteL( localprogress.iError?localprogress.iError:aCompletionCode );
    }

// ----------------------------------------------------
// CMceUi::CreateNewFolderL
// ----------------------------------------------------
void CMceUi::CreateNewFolderL()
    {
    LeaveIfDiskSpaceUnderCriticalLevelL();

    TMsvEntry newFolder;
    const TMsvId parentId=iMceListView->ListContainer()->CurrentFolderId();
    if ( parentId != KMceDocumentsEntryId )
        {
#if defined(_DEBUG)
        Panic( EMceUiErrCannotCreateFolder );
#endif
        return;
        }

    _LIT(KMceFormatFolderName, "%S(%02d)");
    TBuf<KMceMaxFolderLength> folderName;
    TInt num = 0;
    TBuf<KMceTextLength> defaultFolderName;
    StringLoader::Load( defaultFolderName, R_MCE_DEFAULT_FOLDER_NAME, iEikonEnv );
    folderName.Copy( defaultFolderName );
    while( iMceListView->IsFolderNameUsedL( folderName ) )
        {
        num++;
        folderName.Format( KMceFormatFolderName, &defaultFolderName, num );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( folderName );
        }

    if ( FolderNameQueryL( R_MCE_CREATE_FOLDER, folderName ) )
        {
        CAknInputBlock::NewLC();
        CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL( *this );
        CleanupStack::PushL( singleOpWatcher );

        iEntry->SetEntryL( parentId );
        newFolder.iDate.HomeTime();
        newFolder.iDetails.Set( folderName );
        newFolder.iType = KUidMsvFolderEntry;
        newFolder.iMtm = iEntry->Entry().iMtm;
        newFolder.iServiceId= iEntry->OwningService();

        CMsvOperation* op = iEntry->CreateL( newFolder, singleOpWatcher->iStatus );
        CleanupStack::PushL( op );
        iOperations.AppendL( singleOpWatcher );
        CleanupStack::Pop( 2 ); // singleOpWatcher, op
        singleOpWatcher->SetOperation( op );
        MCELOGGER_WRITE_FORMAT("iOperations.AppendL(%d)", op->Id() );
        CleanupStack::PopAndDestroy(); //CAknInputBlock
        }
    }

// ----------------------------------------------------
// CMceUi::RenameFolderL
// ----------------------------------------------------
void CMceUi::RenameFolderL()
    {
    const CMceMessageListContainerBase* container = iMceListView->ListContainer();
    TMsvId currentId = container->CurrentItemId();

    CMsvEntry* centry=iSession->GetEntryL( currentId );
    CleanupStack::PushL( centry );
    TMsvEntry entry = centry->Entry();

    __ASSERT_DEBUG(entry.iType == KUidMsvFolderEntry,
        Panic(EMceUiErrRenameNotFolder));
    __ASSERT_DEBUG(entry.iMtm == KUidMsvLocalServiceMtm,
        Panic(EMceUiErrRenameNotLocal));


    TBuf<KMceMaxFolderLength> buf = entry.iDetails.Left( KMceMaxFolderLength );
    if ( FolderNameQueryL( R_MCE_CREATE_FOLDER, buf, entry.Id() ) )
        {
        CAknInputBlock::NewLC();
        CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL( *this );
        CleanupStack::PushL(singleOpWatcher);
        iEntry->SetEntryL( entry.Id() );
        entry.iDetails.Set( buf );
        CMsvOperation* op = iEntry->ChangeL( entry, singleOpWatcher->iStatus );
        CleanupStack::PushL( op );
        iOperations.AppendL(singleOpWatcher);
        CleanupStack::Pop( 2 ); // singleOpWatcher, op
        singleOpWatcher->SetOperation(op);
        MCELOGGER_WRITE_FORMAT("iOperations.AppendL(%d)", op->Id() );
        CleanupStack::PopAndDestroy(); //CAknInputBlock
        }
    CleanupStack::PopAndDestroy( centry );
    }

// ----------------------------------------------------
// CMceUi::FolderNameQueryL
// ----------------------------------------------------
TBool CMceUi::FolderNameQueryL(
    const TInt aResourceId,
    TDes& aFolderName,
    const TMsvId aFolderId /*= KMsvNullIndexEntryId */ ) const
    {
    CAknTextQueryDialog* dlg = NULL;
    TInt button = 0;
    TBool okToStop = EFalse;
    while ( !okToStop )
        {
        dlg = CAknTextQueryDialog::NewL( aFolderName );
        dlg->SetMaxLength( KMceMaxFolderLength );
        button = dlg->ExecuteLD( aResourceId );
        if ( button )
            {
            MceUtils::ReplaceCharacters( aFolderName );
            if ( iMceListView->IsFolderNameUsedL( aFolderName, aFolderId ) )
                {
                HBufC* errorText = StringLoader::LoadLC(
                    R_MCE_DUPLICATE_FOLDER_TEXT, aFolderName, iEikonEnv );
                CAknInformationNote* note = new(ELeave)CAknInformationNote( ETrue );
                note->ExecuteLD(*errorText);
                CleanupStack::PopAndDestroy( errorText );
                }
            else
                {
                okToStop = ETrue;
                }
            }
        else
            {
            okToStop = ETrue;
            }
        }
    return button;
    }

// ----------------------------------------------------
// CMceUi::CloseConnectionsBeforeExitL
// ----------------------------------------------------
void CMceUi::CloseConnectionsBeforeExitL()
    {
    CMsvEntrySelection* connectedAccounts = ConnectedServicesLC();
    const TInt numberOfConnectedMailboxes = connectedAccounts->Count();
    TInt mceConnected = numberOfConnectedMailboxes;

    if ( iAlwaysOnline )
        {
        TMsvId account;
        TMsvEntry entry;
        TMsvId service;

        for ( TInt cc = 0; cc < numberOfConnectedMailboxes; cc++ )
            {
            account = connectedAccounts->At( cc );
            User::LeaveIfError( iSession->GetEntry(account,service,entry) );
// Get the allways online mail settings
            CImumInSettingsData* settings =
                iEmailApi->MailboxServicesL().LoadMailboxSettingsL( account );
            CleanupStack::PushL( settings );

            if ( !ServiceIdFound( account ) && 
                iMceMainView->ListContainer()->ListItems()->
                    AlwaysOnlineAccountL( account, *settings ) )
                {
                //this is a real always online account and not connected from mce
                mceConnected--;
                }

            CleanupStack::PopAndDestroy( settings ); // extendedMailSettings
            }
        }

    if ( mceConnected > 0 )
        {
        TBool disconnectMailboxes = ETrue;
        if( iEmailClientIntegration && iSelectableEmail )
            {
            //no need to go through the disconnection logic since the FS is enabled
            disconnectMailboxes = EFalse;
            iMceUiFlags.SetMceFlag( EMceUiFlagsWantToExit );
            }
        else
        if ( MceViewActive( EMceMainViewActive )
            || ( MceViewActive( EMceMessageViewActive )
            && iMceListView->IsMSKMailBox() ) )
            {
        CAknQueryDialog* confDialog = CAknQueryDialog::NewL();
        confDialog->PrepareLC( R_MCE_CONFIRMATION );
        HBufC* titleText = NULL;
        if ( numberOfConnectedMailboxes == 1 )
            {
            titleText = StringLoader::LoadLC(
                R_MCE_CONNECTION_ON_TEXT,
                iEikonEnv );
            }
        else
            {
            titleText = StringLoader::LoadLC(
                R_MCE_CONNECTION_ON_MANY_TEXT,
                iEikonEnv );
            }

        confDialog->SetPromptL( *titleText );
        CleanupStack::PopAndDestroy( titleText );

            if ( !confDialog->RunLD() )
                {
                disconnectMailboxes = EFalse;
                }
            }
        if ( disconnectMailboxes )
            {
            iMceUiFlags.SetMceFlag( EMceUiFlagsExitAfterDisconnect  );
            for ( TInt loop = 0; loop < numberOfConnectedMailboxes; loop++ )
                {
                GoOfflineSynchronouslyL( connectedAccounts->At( loop ) );
                }
            if (iMceUiFlags.MceFlag( EMceUiFlagsExitAfterDisconnect ) )
                {
                iMceUiFlags.SetMceFlag( EMceUiFlagsWantToExit );
                }
            }
        }
    else
        {
        iMceUiFlags.SetMceFlag( EMceUiFlagsWantToExit );
        }
    CleanupStack::PopAndDestroy(); // connectedAccounts
    }

// ----------------------------------------------------
// CMceUi::ConnectedServicesLC
// ----------------------------------------------------
CMsvEntrySelection* CMceUi::ConnectedServicesLC()
    {
    MCELOGGER_ENTERFN("ConnectedServicesLC()");

    CMsvEntrySelection* connectedAccounts = new(ELeave) CMsvEntrySelection();
    CleanupStack::PushL(connectedAccounts);

    CMsvEntrySelection* sel = MsvUiServiceUtilities::GetListOfAccountsL(
        *iSession );
    CleanupStack::PushL( sel );
    const TUid  connectCapability={KUidMsvMtmUiQueryConnectionOrientedServices};

    CBaseMtmUiData* uiData=NULL;
    TUid uid;
    const TInt numAccounts=sel->Count();
    for (TInt cc=0; cc<numAccounts; cc++)
        {
         uid.iUid=sel->At(cc);
         const TMsvEntry& tentry=iRootEntry->ChildDataL(uid.iUid);
         // if cmail  feature is enabled, no need to add cmail account in connectedAccounts.
         if ( !( iEmailFramework && 
               ( tentry.iMtm == KSenduiMtmImap4Uid || tentry.iMtm == KSenduiMtmPop3Uid ||
                 tentry.iMtm.iUid == KUidMsgTypeFsMtmVal )))
             {
              TRAPD(error, ( uiData=GetMtmUiDataL(tentry.iMtm) ) );
              if (error==KErrNone && uiData)
                 {
                  TInt rid;
                  if (uiData->QueryCapability(connectCapability, rid)==KErrNone)
                     {
                      if (tentry.Connected())
                         {
                          connectedAccounts->AppendL( sel->At(cc) );
                         }
                     }
                 }
             }  
        }
    CleanupStack::PopAndDestroy( sel );
    MCELOGGER_LEAVEFN("ConnectedServicesLC()");
    return connectedAccounts;
    }

// ----------------------------------------------------
// CMceUi::OpenFolderViewL
// ----------------------------------------------------
void CMceUi::OpenFolderViewL( TMsvId aFolderId )
    {
    iMceListView->SetFolderL( aFolderId );
    ActivateLocalViewL( KMceMessageListViewId );
    }

// ----------------------------------------------------
// CMceUi::OpenRemoteMailboxViewL
// ----------------------------------------------------
void CMceUi::OpenRemoteMailboxViewL( TMsvId aFolderId )
    {
        // NCN reset, if the mail folder contains unread messages
    TBool unreadMessages = EFalse;
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
	TRAPD(err, ( unreadMessages = iBitmapResolver->HasUnreadMessagesL(aFolderId) ) );
#else   
    TInt messageCount = 0;
    TInt unreadCount = 0;
    TRAPD(err, ( unreadMessages = iBitmapResolver->HasUnreadMessagesL(aFolderId, messageCount, unreadCount) ) );
#endif // RD_MSG_NAVIPANE_IMPROVEMENT     
    if (err == KErrNone && unreadMessages )
        {
        HandleNotif(aFolderId);
        }    
    OpenFolderViewL( aFolderId );
    ShowTabsL( aFolderId );
    }

// ----------------------------------------------------
// CMceUi::CancelMailboxTimer
// ----------------------------------------------------
void CMceUi::CancelMailboxTimer()
    {
    if ( iConnectMailboxTimer && iConnectMailboxTimer->IsActive() )
        {
        iConnectMailboxTimer->Cancel();
        }
    }

// ----------------------------------------------------
// CMceUi::EditMTMEntryL
// ----------------------------------------------------
void CMceUi::EditMTMEntryL( const TMsvEntry& aEntry /*, TBool aOpen*/)
    {
    // Define KPSUidMuiu property to be integer type
    TInt r = RProperty::Define( KPSUidMuiu, KMuiuKeyCurrentMsg, RProperty::EInt );
    if ( r != KErrAlreadyExists )
        {
        User::LeaveIfError( r );
        }
    
     r = RProperty::Define( KPSUidMuiu, KMuiuKeyNextMsg, RProperty::EInt );
     if ( r != KErrAlreadyExists )
         {
         User::LeaveIfError( r );
         }

    // Set entry Id value to property
    if ( aEntry.iType == KUidMsvMessageEntry )
        {
        r = RProperty::Set( KPSUidMuiu, KMuiuKeyCurrentMsg, aEntry.Id() );
        r = RProperty::Set( KPSUidMuiu, KMuiuKeyNextMsg, 0 );
        }

    TBool tabsToCleanupStack = EFalse;

    CBaseMtmUi& mtmUi=iMtmStore->GetMtmUiAndSetContextLC( aEntry );
    mtmUi.SetPreferences( mtmUi.Preferences() | EMtmUiFlagEditorPreferEmbedded );

    CMsvOperation* op = NULL;
    CAknInputBlock::NewLC();

    CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( singleOpWatcher );
    if (aEntry.iType == KUidMsvMessageEntry )
        {
        MCELOGGER_WRITE_FORMAT("EditMTMEntryL: iEditorOperation set 0x%x", singleOpWatcher );
        iEditorOperation = singleOpWatcher;
        }
    CleanupStack::PushL( TCleanupItem( ResetEditorOperation, this ) );
    

    if ( aEntry.iType == KUidMsvServiceEntry )
        {
        if ( iMceUiFlags.MceFlag( EMceUiFlagsTabsActive ) )
            {
            // must remove tabs because if no mailboxes created then
            // mailbox settings dialog is displayed and tabs should not be shown then.
            RemoveTabs();
            CleanupStack::PushL( TCleanupItem( ShowTabs, this ) );
            tabsToCleanupStack = ETrue;
            }
        TAknUiZoom previous = ZoomLevelChangedL( EAknUiZoomAutomatic );
        op=mtmUi.EditL( singleOpWatcher->iStatus );
        ZoomLevelChangedL( previous );

        if ( tabsToCleanupStack )
            {
            CleanupStack::PopAndDestroy(); // TCleanupItem - will show tabs again
            }
        if ( iMceUiFlags.MceFlag( EMceUiFlagsTabsActive )  && (!tabsToCleanupStack) )
            {
            ShowTabsL( iMceListView->ListContainer()->CurrentFolderId() );
            }
        }
    else
        {

        ReleaseMtmUiData();

        TRAPD( error, ( op=mtmUi.OpenL(singleOpWatcher->iStatus ) ) );
        if ( error )
            {
            // the returned error ofCBaseMtmUi code is defined in e32err.h instead of DocumentHandler.h
            // KMimeNotSupported(-12003) and KErrNotSupported(-5) has same meaning but different value
            // Solution: add the corresponding error code from e32err.h : (KErrUnknown, KErrAccessDenied ?)
            if ( error == KMimeNotSupported || error == KBadMimeType || error == KExecNotAllowed
                || error == KErrNotSupported || error == KErrUnknown || error == KErrAccessDenied )
                {
                //cannot be opened, mark read
                CMsvEntry* cEntry;
                cEntry = &(mtmUi.BaseMtm().Entry());
                TMsvEntry entry = cEntry->Entry();
                entry.SetUnread( EFalse );
                cEntry->ChangeL( entry );
                }

            // HandleOpenL is Trapped in MceOneRowListContainer to show error note we need to do it by ourselves
             delete op; // op should be null if leave occurs but make sure it is not leaked.
             HBufC* buf = 0;
             HandleErrorL(error, &buf);
             delete buf;
             CleanupStack::PopAndDestroy( 4 ); 
             if (error && iLocalScreenClearer )
                 {
                 delete iLocalScreenClearer;
                 iLocalScreenClearer = NULL;
                 }
             return;
                         
            //User::Leave( error );
            }
            
        CMceMessageListContainerBase* listContainer = iMceListView->ListContainer();
        if ( listContainer )
            {
            listContainer->UpdateIconArrayL();
            }
            
    	// the check has been added for email mailbox, need not to clear the screen
    	if ( !iLocalScreenClearer &&
             !CheckTechnologyTypeFromMtm( KMailTechnologyTypeUid, aEntry.iMtm ))       
	        {
	        iLocalScreenClearer = CAknLocalScreenClearer::NewL( ETrue );
	        }            
        }

    CleanupStack::PushL( op );
    iOperations.AppendL( singleOpWatcher );
    CleanupStack::Pop( 3 ); // singleOpWatcher, ResetEditorOperation op
    singleOpWatcher->SetOperation( op );
    MCELOGGER_WRITE_FORMAT("iOperations.AppendL(%d)", op->Id() );
    if ( aEntry.iType == KUidMsvMessageEntry &&
        iUiRegistry &&
        iUiRegistry->IsPresent( op->Mtm() ) )
        {
        iMtmStore->ClaimMtmUiL( aEntry.iMtm );            
        }
    CleanupStack::PopAndDestroy( 2 ); // CAknInputBlock, release mtmUi
    }

// ----------------------------------------------------
// CMceUi::CreateNewAccountL
// ----------------------------------------------------
TBool CMceUi::CreateNewAccountL(
    TUid aMessageType,
    TMsvId aOldServiceId /* = KMsvNullIndexEntryId */)
    {
    MCELOGGER_ENTERFN("CreateNewAccountL()");
    if ( aMessageType == KSenduiMtmSyncMLEmailUid )
    {
        aMessageType = KSenduiMtmSmtpUid;
    }

    iRootEntry->SetEntryL(KMsvRootIndexEntryIdValue);

    LeaveIfDiskSpaceUnderCriticalLevelL();

    TMsvEntry nentry;
    nentry.iMtm = aMessageType;
    nentry.iType.iUid = KUidMsvServiceEntryValue;
    nentry.iDate.HomeTime();
    nentry.iServiceId = aOldServiceId;

    // --- Get the MTM UI relevant to the message type ---
    CBaseMtmUi& mtmUi = iMtmStore->GetMtmUiLC( nentry.iMtm );
    CMsvEntry* centry = iSession->GetEntryL( KMsvRootIndexEntryId );
    CleanupStack::PushL( centry );
    CAknInputBlock::NewLC();

    CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL(*this);
    CleanupStack::PushL( singleOpWatcher );

    // Mailbox creation is ongoing, syncronic operation
    iMailboxCreationOn = ETrue;
    CMsvOperation* op = mtmUi.CreateL(
        nentry,
        *centry,
        singleOpWatcher->iStatus );
    // Mailbox creation finalized
    iMailboxCreationOn = EFalse;

    CleanupStack::PushL( op );
    iOperations.AppendL( singleOpWatcher );
    CleanupStack::Pop( 2 ); // singleOpWatcher, op
    singleOpWatcher->SetOperation( op );

    TBool accountCreated = (op->iStatus != KErrCancel);
    // if not created, imum returns completed operation with KErrCancel
    if ( accountCreated )
				{
        iMceMainView->ListContainer()->ListItems()->HandleSessionEventL( EMsvEntriesCreated, NULL, NULL, NULL );
				}

    CleanupStack::PopAndDestroy(); // CAknInputBlock
    CleanupStack::PopAndDestroy( centry );
    CleanupStack::PopAndDestroy(); // mtm ui release
    MCELOGGER_LEAVEFN("CreateNewAccountL()");
    return accountCreated;
    }

// ----------------------------------------------------
// CMceUi::EditAccountL
// ----------------------------------------------------
void CMceUi::EditAccountL(TMsvId aId)
    {

    iRootEntry->SetEntryL(KMsvRootIndexEntryIdValue);

    LeaveIfDiskSpaceUnderCriticalLevelL();

    TMsvEntry entry;

    CMsvEntrySelection* rootSel = iRootEntry->ChildrenL();
    CleanupStack::PushL( rootSel );
    const TInt index = rootSel->Find( aId );
    if (index > KErrNotFound)
        {
        entry = (*iRootEntry)[index];
        EditMTMEntryL( entry );
        }
    CleanupStack::PopAndDestroy( rootSel );
    }

// ----------------------------------------------------
// CMceUi::MtmAccountsL
// ----------------------------------------------------
CUidNameArray* CMceUi::MtmAccountsL( TUid aType )
    {

    iRootEntry->SetEntryL(KMsvRootIndexEntryIdValue);

    CUidNameArray* accounts = new (ELeave) CUidNameArray(
        KMceArrayGranularity );
    CleanupStack::PushL( accounts );

    CMsvEntrySelection* sel = NULL;

    if ( aType == KSenduiMtmSmtpUid )
        {
        sel = MsvUiServiceUtilities::GetListOfAccountsL(
            *iSession,
            ETrue );
        }
    else
        {
        sel = MsvUiServiceUtilities::GetListOfAccountsWithMTML(
            *iSession,
            aType,
            ETrue );
        }
    CleanupStack::PushL( sel );

    CMsvEntry* rootEntry = CMsvEntry::NewL(
        *iSession,
        KMsvRootIndexEntryId,
        TMsvSelectionOrdering(KMsvNoGrouping, EMsvSortByNone, ETrue));
    delete iRootEntry;
    iRootEntry = rootEntry;

    TUid uid;
    TUid techType = iUiRegistry->TechnologyTypeUid( aType );
    const TInt numAccounts = sel->Count();
    for (TInt cc=0; cc<numAccounts; cc++)
        {
        uid.iUid = sel->At(cc);
        TMsvEntry tentry;
        TRAPD( err, ( tentry = iRootEntry->ChildDataL( uid.iUid ) ) );
        if ( err == KErrNone &&
            iUiRegistry->IsPresent( tentry.iMtm ) && 
            iUiRegistry->TechnologyTypeUid( tentry.iMtm ) == techType )
            {
            const TInt count = accounts->Count();
            TBool foundService = EFalse;
            //check that related service is not already added to array
            for ( TInt loop = 0; loop < count; loop ++ )
                {
                if ( tentry.iRelatedId == (*accounts)[loop].iUid.iUid )
                    {
                    foundService = ETrue;
                    break;
                    }
                }
            if ( !foundService )
                {
                TUidNameInfo info( uid, tentry.iDetails.Left( KHumanReadableNameLength ) );
                MceUtils::ReplaceCharacters( info.iName );
                accounts->AppendL( info );
                }
            }
        }
    CleanupStack::PopAndDestroy( sel );
    CleanupStack::Pop( accounts );
    return accounts;
    }

// ----------------------------------------------------
// CMceUi::HandleDeleteL
// ----------------------------------------------------
void CMceUi::HandleDeleteL()
    {
    MCELOGGER_ENTERFN("HandleDeleteL()");

    __ASSERT_DEBUG( MceViewActive( EMceMessageViewActive ),
        Panic( EMceUiErrCannotDeleteFromMainView ) );
    if ( MceViewActive( EMceMainViewActive ) )
        {
        return;
        }

    CMsvEntrySelection * selection = iMceListView->ListContainer()
        ->CurrentItemSelectionL();
    CleanupStack::PushL( selection );
    __ASSERT_DEBUG( selection->Count() > 0,
        Panic( EMceUiErrCannotDeleteFromMainView ) );

    if (  selection->Count() <= 0 )
        {
        CleanupStack::PopAndDestroy( selection );
        return;
        }

    iMceListView->ListContainer()->SetAnchorItemIdL(
        CMceMessageListContainerBase::EMessageListOperationDelete );

    const TMsvId id = selection->At(0);
    iEntry->SetEntryL(id);
    const TInt type = iEntry->Entry().iType.iUid;

    if ( IsMessageSendingL( id ) )
        {
        CleanupStack::PopAndDestroy( selection );
        return;
        }

    __ASSERT_DEBUG( type == KUidMsvMessageEntryValue || type == KUidMsvFolderEntryValue,
        Panic(EMceUiErrCannotDeleteServiceFromMainView) );

    TBool local = iEntry->OwningService() == KMsvLocalServiceIndexEntryIdValue;

    if ( iMMSNotifications )
        {
        delete iMMSNotifications;
        iMMSNotifications = NULL;
        }

    if ( local )
        {
        CAknQueryDialog* confDialog = CAknQueryDialog::NewL();
        confDialog->PrepareLC( R_MCE_CONFIRMATION );
        // confDialog in CleanupStack now
        HBufC* titleText;
        if ( type == KUidMsvFolderEntryValue )
            {
            titleText = StringLoader::LoadLC( R_MCE_QUERY_COMMON_CONF_DELETE_FO,
                iEntry->Entry().iDetails.Left( iBitmapResolver->DescriptionLength() ), iEikonEnv );
            }
        else
            {
            if ( selection->Count() == 1 )
                {
                titleText = StringLoader::LoadLC(
                    R_MCE_QUERY_COMMON_CONF_DELETE_ME, iEikonEnv );
                }
            else
                {
                titleText = StringLoader::LoadLC(
                    R_MCE_QUERY_COMMON_CONF_DELETE_MS,
                    selection->Count(), iEikonEnv );
                }
            }
        confDialog->SetPromptL( *titleText );
        CleanupStack::PopAndDestroy( titleText );
        // RunLD removes confDialog from CleanupStack and deletes itself
        if ( !confDialog->RunLD() )
            {
            CleanupStack::PopAndDestroy( selection ); // selection
            return;
            }
        }
    
    if ( local && ( ( iEntry->Entry().Parent() == KMsvGlobalInBoxIndexEntryId ) ||
        ( iEntry->Entry().Parent() == KMsvGlobalOutBoxIndexEntryId ) ) ) 
        {
        CheckMMSNotificationsL( selection );
        if (  selection->Count() <= 0 )
            {
            // delete mms notifications, if any
            if ( iMMSNotifications->Count() > 0 )
                {
                //delete MMS notifications
                TBuf8<1> params;
                HandleMMSNotificationsDeleteL( params );
                }
            CleanupStack::PopAndDestroy( selection );
            return;
            }
        }

   
    iEntry->SetEntryL( iEntry->Entry().Parent() );

    CBaseMtmUi* ui = NULL;
    TUid mtm=iEntry->Entry().iMtm;
    if ( !local )
        {
        TRAPD(uiErr, (ui = &iMtmStore->ClaimMtmUiL( mtm )));
        if ( uiErr == KErrNotFound || uiErr == KErrNotSupported )
            {
            // MTM not present, so delete as normal local entry
            ui = NULL;
            }
        else
            {
            User::LeaveIfError(uiErr);
            }
        }
    if (ui)
        {
        CMtmStoreMtmReleaser::CleanupReleaseMtmUiLC(*iMtmStore, mtm);
        }

    CMsvOperation* op;

    if ( local )
        {
        __ASSERT_DEBUG( !iDeleteOperation,
            Panic( EMceUiErrAlreadyDeleting ) );

// Free disk space request for delete to be added
        }

    CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( singleOpWatcher );

    TBool resetSelection = ETrue;

    if (ui)
        {
        // reset ncn, if it is unread message
        TMsvEntry entry;
        TMsvId serviceId;
        User::LeaveIfError( iSession->GetEntry(selection->At(0),serviceId,entry) );
                
        if ( CheckTechnologyTypeFromMtm( KMailTechnologyTypeUid, 
                entry.iMtm )&& entry.Unread() )
            {
            HandleNotif(iMceListView->ListContainer()->FolderEntry().iServiceId);

            }
        
        ui->BaseMtm().SetCurrentEntryL(iSession->GetEntryL(
            iEntry->Entry().Id()));

        CAknInputBlock::NewLC();
        op = ui->DeleteFromL(*selection, singleOpWatcher->iStatus);
        SetViewUpdateSuppressionFlag( ETrue, op );
        CleanupStack::PopAndDestroy(); // CAknInputBlock
        CleanupStack::PushL( op );
        // if cancel was pressed then completed operation is returned with
        // local service and status KErrCancel
        if ( op->Service() == KMsvLocalServiceIndexEntryId &&
             op->iStatus == KErrCancel )
            {
            resetSelection = EFalse;
            SetViewUpdateSuppressionFlag( EFalse, NULL );
            }
        }
    else
        {
        if ( iEntry->EntryId() == KMsvGlobalOutBoxIndexEntryId )
            {
            CMsvProgressReporterOperation* progOp =
                CMsvProgressReporterOperation::NewL(
                *iSession, singleOpWatcher->iStatus, EMbmAvkonQgn_note_erased );
            CleanupStack::PushL( progOp );

            CMsvOperation* subOp = CMceCancelSendingOperation::NewL(
                *iSession,
                progOp->RequestStatus(),
                selection->At(0),
                CMceCancelSendingOperation::ECancelAndDelete,
                *iMtmStore );

            progOp->SetOperationL(subOp); // this takes ownership immediately, so no cleanupstack needed.

            HBufC* text = StringLoader::LoadLC(
                R_MUIU_LOCAL_PROGRESS_DELETING_1,
                iEikonEnv );
            progOp->SetTitleL( *text );
            CleanupStack::PopAndDestroy( text );
            op = progOp;
            }
        else
            {
            // SMS and MMS deletion
            if ( selection->Count() > KMceProgressReporterThreshold ||
                 type == KUidMsvFolderEntryValue )
                {
                // Show the progress note

                CMsvProgressReporterOperation* progOp =
                    CMsvProgressReporterOperation::NewL(
                    *iSession, singleOpWatcher->iStatus, EMbmAvkonQgn_note_erased );

                CleanupStack::PushL( progOp );

                // deleting folder
                HBufC* text = NULL;
                iEntry->SetEntryL(id);

                if( type == KUidMsvFolderEntryValue )
                    {
                    text = StringLoader::LoadLC( R_MCE_WAIT_DELETE_FOLDER,
                        iEntry->Entry().iDetails.Left( iBitmapResolver->DescriptionLength() ),
                        iEikonEnv );
                    }
                else // if message
                    {
                    text = StringLoader::LoadLC(
                        R_MUIU_LOCAL_PROGRESS_DELETING_1,
                        iEikonEnv );
                    }
                progOp->SetTitleL( *text );
                CleanupStack::PopAndDestroy( text );
                iEntry->SetEntryL( iEntry->Entry().Parent() );

                CMsvOperation* subOp=iEntry->DeleteL(
                    *selection, progOp->RequestStatus() );
                progOp->SetOperationL(subOp); // this takes ownership immediately, so no cleanup stack needed.
                op = progOp;
                }
            else
                {
                // Do not show the progress note
                iEntry->SetEntryL(id);
                iEntry->SetEntryL( iEntry->Entry().Parent() );
                op = iEntry->DeleteL( *selection, singleOpWatcher->iStatus );
                CleanupStack::PushL( op );
                }
            }
        }

    iOperations.AppendL( singleOpWatcher );
    CleanupStack::Pop( 2, singleOpWatcher ); // singleOpWatcher, op
    singleOpWatcher->SetOperation( op );
    MCELOGGER_WRITE_FORMAT("iOperations.AppendL(%d)", op->Id() );

    if ( ui )
        {
        CleanupStack::PopAndDestroy( );// release mtmUi
        }
    if ( local )
        {
        // if deleting from service, then it is mtm's responsibility to free disk...
        iDeleteOperation = singleOpWatcher;
        MCELOGGER_WRITE_FORMAT("free disk requested, iDeleteOperation set 0x%x", iDeleteOperation );

// Cancel the free disk space request to be added
        }
    CleanupStack::PopAndDestroy( selection );
    if ( resetSelection )
        {
        iMceListView->ListContainer()->ClearSelection();
        }
    iMsgDeletedStatus = ETrue;
    MCELOGGER_LEAVEFN("HandleDeleteL()");
    }

// ----------------------------------------------------
// CMceUi::HandleUndeleteL
// ----------------------------------------------------
void CMceUi::HandleUndeleteL()
    {
    MCELOGGER_ENTERFN("HandleUndeleteL()");

    __ASSERT_DEBUG( MceViewActive( EMceMessageViewActive ),
        Panic( EMceUiErrCannotUnDeleteFromMainView ) );
    if ( MceViewActive( EMceMainViewActive ) )
        {
        return;
        }

    CMsvEntrySelection* selection = iMceListView->ListContainer()
        ->CurrentItemSelectionRefreshL();
    CleanupStack::PushL( selection );

    const TInt count = selection->Count();
    __ASSERT_DEBUG( count != 0, Panic( EMceErrNothingToUnDelete ) );
    if ( count == 0 )
        {
        CleanupStack::PopAndDestroy( selection );
        return;
        }

    const TMsvId id = selection->At( 0 );
    TBool resetSelection = ETrue;

    TMsvEntry tentry;
    GetEntryL( id, tentry );
    TMsvEntry parentEntry;
    GetEntryL( tentry.Parent(), parentEntry );

    CBaseMtmUi& ui = iMtmStore->GetMtmUiAndSetContextLC( parentEntry );

    CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL(singleOpWatcher);

    CAknInputBlock::NewLC();
    CMsvOperation* op = ui.UnDeleteFromL(*selection, singleOpWatcher->iStatus);
    CleanupStack::PopAndDestroy(); // CAknInputBlock::NewLC()
    CleanupStack::PushL( op );
    // if cancel was pressed then completed operation is returned with
    // local service and status KErrCancel
    if ( op->Service() == KMsvLocalServiceIndexEntryId &&
         op->iStatus == KErrCancel )
        {
        resetSelection = EFalse;
        }

    iOperations.AppendL( singleOpWatcher );
    CleanupStack::Pop( 2 ); // singleOpWatcher, op
    singleOpWatcher->SetOperation( op );
    MCELOGGER_WRITE_FORMAT("iOperations.AppendL(%d)", op->Id() );

    CleanupStack::PopAndDestroy( 2 );// release mtmUi, selection
    if ( resetSelection )
        {
        iMceListView->ListContainer()->ClearSelection();
        }
    MCELOGGER_LEAVEFN("HandleUndeleteL()");
    }

// ----------------------------------------------------
// CMceUi::MoveOrCopyEntriesL
// ----------------------------------------------------
void CMceUi::MoveOrCopyEntriesL( TBool aCopy )
    {
    TInt count;
    TMsvId id;
    TMsvId service;
    TMsvEntry tEntry;
    TBool mailMessage = EFalse;
    if ( aCopy )
        {
        LeaveIfDiskSpaceUnderCriticalLevelL( iDiskSpaceForMoveOrCopy );
        }
    else
        {
        LeaveIfDiskSpaceUnderCriticalLevelL();
        }

    __ASSERT_DEBUG( MceViewActive( EMceMessageViewActive ),
        Panic( EMceUiErrCannotMoveFromMainView ) );
    if ( MceViewActive( EMceMainViewActive ) )
        {
        return;
        }

    const TMsvId sourceId = iMceListView->ListContainer()->CurrentFolderId();

    CMsvEntrySelection * selection = iMceListView->ListContainer()
        ->CurrentItemSelectionL();
    CleanupStack::PushL( selection );

    if ( !aCopy )
        {
        count = selection->Count();
        // MMS notifications cannot be moved
        for ( TInt cc = count; --cc >= 0; )
            {
            id = selection->At( cc );
            if ( iSession->GetEntry( id, service, tEntry ) == KErrNone  )
                {
                if ( tEntry.iMtm == KUidMsgMMSNotification )
                    {
                    selection->Delete( cc );
                    }
                }
            }
        if ( selection->Count() < count )
            {
            // MMS notifications have been deleted from selection
            CAknQueryDialog* note = CAknQueryDialog::NewL();
            HBufC* text = NULL;
            text = StringLoader::LoadLC( R_MMS_INFO_CANNOT_MOVE_NOTIF, CCoeEnv::Static() );
            note->SetPromptL( *text );
            CleanupStack::PopAndDestroy( text );
            note->ExecuteLD( R_MCE_MEMC_NOTE );
            }
        }

    // Email messages cannot be moved
    iMoveOrCopyMailOperation = -1;
    mailMessage = RemoveEmailMessagesFromSelection( selection );

    if (  selection->Count() <= 0 )
        {
        CleanupStack::PopAndDestroy( selection );
        if ( mailMessage )
            {
            InformationNoteCannotMoveCopyEMailL();
            }
        return;
        }
// Find an iAnchorId (=itemId) that should be focused after moving some items from the list
    iMceListView->ListContainer()->SetAnchorItemIdL(
        CMceMessageListContainerBase::EMessageListOperationGeneral );

    TMsvId destinationId = sourceId;
    const TInt titleResourceId = aCopy ?
        R_MCE_COPY_MESSAGES :
        R_MCE_MOVE_MESSAGES;
    HBufC* text =  StringLoader::LoadLC( titleResourceId, iEikonEnv );

    const TInt iconId = aCopy ?
        EMbmAvkonQgn_note_move :
        EMbmAvkonQgn_note_copy;

    if ( CMsgFolderSelectionDialog::SelectFolderL( destinationId, *text ) )
        {
        // Trap: NCNList cannot seperate New and Unread that comes to Inbox
        // So when movning Unread msgs to Inbox, we need to change them to
        // read, and then change them back after moving is completed
        if ( destinationId == KMsvGlobalInBoxIndexEntryId )
            {
            HandleNewMsgToInboxL( ETrue, selection );
            }

        TMsvEntry srcEntry;
        const TBool sourceIsRemote = KMsvLocalServiceIndexEntryId != GetEntryL( sourceId, srcEntry);

        CMsvOperation* op=NULL;

        CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL( *this );
        CleanupStack::PushL( singleOpWatcher );

        CAknInputBlock::NewLC();

        TMsvId service = KMsvLocalServiceIndexEntryId;
        TMsvEntry sourceServiceEntry;
        User::LeaveIfError( iSession->GetEntry( srcEntry.iServiceId, service, sourceServiceEntry ) );

        if ( ( sourceIsRemote && !( sourceServiceEntry.Connected() ) ) ||
            ( sourceServiceEntry.iMtm == KSenduiMtmSyncMLEmailUid ) )
            {
            CBaseMtmUi& ui=iMtmStore->GetMtmUiAndSetContextLC( srcEntry );
            op=ui.CopyFromL( *selection, destinationId, singleOpWatcher->iStatus);
            CleanupStack::PopAndDestroy(); // release ui
            }
        else
            {
            CMsvProgressReporterOperation* progOp =
                CMsvProgressReporterOperation::NewL( *iSession, singleOpWatcher->iStatus, iconId );
            CleanupStack::PushL(progOp);
            CMsvOperation* subOp = NULL;
            iEntry->SetEntryL( sourceId );

            if ( aCopy )
                {
                subOp = iEntry->CopyL( *selection, destinationId, progOp->RequestStatus());
                }
            else
                {
                subOp = iEntry->MoveL( *selection, destinationId, progOp->RequestStatus());
                }

            progOp->SetOperationL( subOp ); // this takes ownership immediately, so no cleanupstack needed.
            op = progOp;
            CleanupStack::Pop( progOp ); 
            }

        CleanupStack::PopAndDestroy(); // CAknInputBlock
        CleanupStack::PushL( op );
        iOperations.AppendL( singleOpWatcher );
        CleanupStack::Pop( 2 ); // singleOpWatcher, op
        singleOpWatcher->SetOperation( op );
        MCELOGGER_WRITE_FORMAT("iOperations.AppendL(%d)", op->Id() );

        if ( mailMessage )
            {
            // Mail messages cannot be copied or moved, save the command ID
            iMoveOrCopyMailOperation = op->Id();
            }

        iMceListView->ListContainer()->ClearSelection();

        } // end if
    CleanupStack::PopAndDestroy( 2 ); // text, selection
    }

// ----------------------------------------------------
// CMceUi::ActivateLocalViewL
// ----------------------------------------------------
void CMceUi::ActivateLocalViewL(TUid aViewId)
    {
    if ( aViewId == KMceMainViewListViewId )
        {
        iEditorOperation = NULL;
        CAknTitlePane* title=TitlePaneL();
        HBufC* text = StringLoader::LoadLC( R_MCE_MAIN_VIEW_TITLE, iEikonEnv );
        title->SetTextL( *text );
        CleanupStack::PopAndDestroy(); // text
        }
    CAknViewAppUi::ActivateLocalViewL( aViewId );
    }

// ----------------------------------------------------
// CMceUi::CreateTabsL
// ----------------------------------------------------
void CMceUi::CreateTabsL()
    {
    MCELOGGER_ENTERFN("CreateTabsL()");
    __ASSERT_DEBUG( !iTabsArray, Panic( EMceUiErrTabsArrayAlreadyExists ) );

    delete iDecoratedTabGroup;
    iDecoratedTabGroup = NULL;
    iDecoratedTabGroup = iNaviPane->CreateTabGroupL( this );
    iDecoratedTabGroup->SetControlType( CAknNavigationDecorator::ETabGroup );

    CAknTabGroup* tabGroup = ( CAknTabGroup* ) iDecoratedTabGroup->DecoratedControl();

    CMceMainViewListItemArray* array = iMceMainView->ListContainer()->ListItems();
    TInt count = array->Count();
    CMsvEntrySelection* tabsArray = new( ELeave ) CMsvEntrySelection();
    CleanupStack::PushL( tabsArray );
    TInt loop = 0;
    for ( loop = 0; loop < count; loop++ )
        {
        const TMceListItem& listItem = (*array)[loop];
        if ( !listItem.iExtraItem || listItem.iMsvId == KMceDeliveryReportsListIdValue )
            {
              if (( iEmailClientIntegration )&&(!iEmailFramework))
              {
                TMsvId service;
                TMsvEntry entry;
                     
                  
                CRepository* repository = NULL;
                TInt mtmPluginID = 0;
                TRAPD( ret, repository = CRepository::NewL(
                                         KCRUidSelectableDefaultEmailSettings ) );
                
                if ( ret == KErrNone )
                    {
                    // Get Email application mtm plugin ID
                    repository->Get( KIntegratedEmailAppMtmPluginId, mtmPluginID );
                    }
                delete repository;
                if ( iSession->GetEntry(listItem.iMsvId, service, entry) == KErrNone
                                                    &&  entry.iMtm.iUid != mtmPluginID)
                 {
                 tabsArray->AppendL( listItem.iMsvId );
                 }
				else if(listItem.iMsvId == KMceDeliveryReportsListIdValue)
                 {
                 	tabsArray->AppendL( listItem.iMsvId );
                 }
               }
               else
               {
                 tabsArray->AppendL( listItem.iMsvId );
               }
            
            }
        }
    CleanupStack::Pop( tabsArray );
    iTabsArray = tabsArray;

    CFbsBitmap* bitmap;
    CFbsBitmap* bitmapMask;

    TAknsItemID id;
    id.iMajor = 0;
    id.iMinor = 0;

    MAknsSkinInstance* skins = AknsUtils::SkinInstance();

    count = iTabsArray->Count();
    TInt bitmapIndex = EMbmMuiuQgn_prop_mce_inbox_tab4;
    for ( loop = 0; loop < count; loop++ )
        {
        switch ( (*iTabsArray)[loop] )
            {
            case KMsvGlobalInBoxIndexEntryId:
                bitmapIndex = EMbmMuiuQgn_prop_mce_inbox_tab4;
                id = KAknsIIDQgnPropMceInboxTab4;
                break;
            case KMceDocumentsEntryId:
                bitmapIndex = EMbmMuiuQgn_prop_mce_doc_tab4;
                id = KAknsIIDQgnPropMceDocTab4;
                break;
            case KMsvDraftEntryId:
                bitmapIndex = EMbmMuiuQgn_prop_mce_drafts_tab4;
                id = KAknsIIDQgnPropMceDraftsTab4;
                break;
            case KMsvSentEntryId:
                bitmapIndex = EMbmMuiuQgn_prop_mce_sent_tab4;
                id = KAknsIIDQgnPropMceSentTab4;
                break;
            case KMsvGlobalOutBoxIndexEntryId:
                bitmapIndex = EMbmMuiuQgn_prop_mce_outbox_tab4;
                id = KAknsIIDQgnPropMceOutboxTab4;
                break;
            case KMceDeliveryReportsListIdValue:
                bitmapIndex = EMbmMuiuQgn_prop_mce_dr_tab4;
                id = KAknsIIDQgnPropMceDrTab4;
                break;
            default:
                {
                TMsvId service;
                TMsvEntry entry;
                if ( iSession->GetEntry( (*iTabsArray)[loop], service, entry) == KErrNone
                    && ( entry.iMtm == KSenduiMtmImap4Uid || entry.iMtm == KSenduiMtmPop3Uid )
                    && entry.Connected() )
                    {
                    bitmapIndex = EMbmMuiuQgn_prop_mce_remote_on_tab4;
                    id = KAknsIIDQgnPropMceRemoteOnTab4;
                    }
                else
                    {
                    bitmapIndex = EMbmMuiuQgn_prop_mce_remote_tab4;
                    id = KAknsIIDQgnPropMceRemoteTab4;
                    }
                }
                break;
            }

        AknsUtils::CreateIconL( skins, id, bitmap,
           bitmapMask, iFilename, bitmapIndex,
           bitmapIndex + 1 );

        tabGroup->AddTabL(
            loop, bitmap, bitmapMask );
        }

    tabGroup->SetTabFixedWidthL(KTabWidthWithFourTabs);

    MCELOGGER_LEAVEFN("CreateTabsL()");
    }

// ----------------------------------------------------
// CMceUi::CheckRemoteMailboxTabIconsL
// ----------------------------------------------------
void CMceUi::CheckRemoteMailboxTabIconsL()
    {
    MCELOGGER_ENTERFN("CheckRemoteMailboxTabIconsL()");
    __ASSERT_DEBUG( iMceUiFlags.MceFlag( EMceUiFlagsTabsActive ),
        Panic( EMceUiErrTabsNotActivated ) );
    __ASSERT_DEBUG( iTabsArray, Panic( EMceUiErrTabsArrayDoesNotAlreadyExist ) );
    CAknTabGroup* tabGroup = ( CAknTabGroup* ) iDecoratedTabGroup->DecoratedControl();
    const TInt count = iTabsArray->Count();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        TMsvId service;
        TMsvEntry entry;
        if ( iSession->GetEntry( (*iTabsArray)[loop], service, entry) == KErrNone  )
            {
            TInt bitmapIndex = KErrNotFound;
            TAknsItemID id;
            id.iMajor = 0;
            id.iMinor = 0;
            if ( ( entry.iMtm == KSenduiMtmImap4Uid
            || entry.iMtm == KSenduiMtmPop3Uid ) )
            {
                bitmapIndex = entry.Connected() ?
                EMbmMuiuQgn_prop_mce_remote_on_tab4 : EMbmMuiuQgn_prop_mce_remote_tab4;
                id = entry.Connected() ?
                    KAknsIIDQgnPropMceRemoteOnTab4 : KAknsIIDQgnPropMceRemoteTab4;
                }
            else if ( entry.iMtm == KSenduiMtmSyncMLEmailUid )
                {
                bitmapIndex = EMbmMuiuQgn_prop_sml_remote_tab4;
                id = KAknsIIDQgnPropSmlRemoteTab4;
                }

            if ( bitmapIndex > KErrNotFound )
                {
                MAknsSkinInstance* skins = AknsUtils::SkinInstance();
                CFbsBitmap* bitmap;
                CFbsBitmap* bitmapMask;

                AknsUtils::CreateIconL( skins, id, bitmap,
                    bitmapMask, iFilename, bitmapIndex, bitmapIndex + 1 );

                tabGroup->ReplaceTabL(
                    loop, bitmap, bitmapMask );
                }
            }
        }

    MCELOGGER_LEAVEFN("CheckRemoteMailboxTabIconsL()");
    }

// ----------------------------------------------------
// CMceUi::ShowTabsL
// ----------------------------------------------------
void CMceUi::ShowTabsL( TMsvId aOpenedId, TBool aOpeningDeliveryReports /* = EFalse */ )
    {
    MCELOGGER_ENTERFN("ShowTabsL()");

    if ( !iDecoratedTabGroup )
        {
        CreateTabsL();
        }

    TInt selectedIndex;
    if ( aOpeningDeliveryReports )
        {
        selectedIndex = iTabsArray->Find( KMceDeliveryReportsListIdValue );
        }
    else
        {
        selectedIndex = iTabsArray->Find( aOpenedId );
        }

    CAknTabGroup* tabGroup = ( CAknTabGroup* ) iDecoratedTabGroup->DecoratedControl();
    tabGroup->SetActiveTabByIndex( selectedIndex );
    iNaviPane->PushL(*iDecoratedTabGroup);
    iMceUiFlags.SetMceFlag( EMceUiFlagsTabsActive );
    CheckRemoteMailboxTabIconsL();
    MCELOGGER_LEAVEFN("ShowTabsL()");
    }

// ----------------------------------------------------
// CMceUi::RemoveTabs
// ----------------------------------------------------
void CMceUi::RemoveTabs()
    {
    MCELOGGER_ENTERFN("RemoveTabs()");
    if ( iMceUiFlags.MceFlag( EMceUiFlagsTabsActive ) )
        {
        iNaviPane->Pop();
        iMceUiFlags.ClearMceFlag( EMceUiFlagsTabsActive );
        }
    MCELOGGER_LEAVEFN("RemoveTabs()");
    }

// ----------------------------------------------------
// CMceUi::RemoveTabsAndUpdateArray
// ----------------------------------------------------
void CMceUi::RemoveTabsAndUpdateArray()
    {
    MCELOGGER_ENTERFN("RemoveTabsAndUpdateArray()");
    TBool tabsActive = iMceUiFlags.MceFlag( EMceUiFlagsTabsActive );
    RemoveTabs();
    delete iDecoratedTabGroup;
    iDecoratedTabGroup = NULL;
    delete iTabsArray;
    iTabsArray = NULL;
    
    if ( tabsActive && MceViewActive( EMceMessageViewActive ) && iMceListView )
        {
        TRAP_IGNORE( ShowTabsL( iMceListView->ListContainer()->CurrentFolderId() ) );
        }
    MCELOGGER_LEAVEFN("RemoveTabsAndUpdateArray()");
    }

// ----------------------------------------------------
// CMceUi::CheckIAUpdate
// ----------------------------------------------------
void CMceUi::CheckIAUpdate()
    {
    TRAP_IGNORE(iMceIAUpdate->StartL( TUid::Uid( KMceApplicationUidValue ) ));
    }

// ----------------------------------------------------
// CMceUi::GoOnlineWithQueryL
// ----------------------------------------------------
void CMceUi::GoOnlineWithQueryL( TMsvId aAccount )
    {
    if ( AlwaysOnlineLastConnectionL( aAccount ) )
        {
        CAknQueryDialog* confDialog = CAknQueryDialog::NewL();
        if ( confDialog->ExecuteLD( R_MCE_GO_ONLINE_CONFIRMATION ) )
            {
            ChangeServiceConnectionStateL( aAccount, ETrue);
            }
        }
    }

// ----------------------------------------------------
// CMceUi::GoOnlineL
// ----------------------------------------------------
void CMceUi::GoOnlineL( TMsvId aAccount )
    {
    MCELOGGER_ENTERFN("GoOnlineL()");
    ChangeServiceConnectionStateL( aAccount, ETrue);
    MCELOGGER_LEAVEFN("GoOnlineL()");
    }

// ----------------------------------------------------
// CMceUi::CloseConnectionWithListQueryL
// ----------------------------------------------------
void CMceUi::CloseConnectionWithListQueryL()
    {
    CMsvEntrySelection* connectedAccounts = ConnectedServicesLC();
    const TInt numberOfConnectedMailboxes = connectedAccounts->Count();
    TInt closeConnection = KErrNotFound;

    if ( numberOfConnectedMailboxes > 0 )
        {
        if ( numberOfConnectedMailboxes == 1 )
            {
            closeConnection = 0;
            }
        else
            {
            CDesCArrayFlat* items = new(ELeave)CDesCArrayFlat( KMceArrayGranularity );
            CleanupStack::PushL( items );
            for (TInt loop = 0; loop < numberOfConnectedMailboxes; loop++)
                {
                const TMsvEntry& tentry=iRootEntry->ChildDataL( connectedAccounts->At( loop ) );
                items->AppendL( tentry.iDetails.Left( iBitmapResolver->DescriptionLength() ) );
                }

            CAknListQueryDialog* dlg = new (ELeave) CAknListQueryDialog(&closeConnection);
            dlg->PrepareLC( R_MCE_LIST_QUERY );
            dlg->SetItemTextArray( items );
            dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );
            if( !dlg->RunLD() )
                {
                closeConnection = KErrNotFound;
                }
            CleanupStack::PopAndDestroy( items );
            }

        if ( closeConnection != KErrNotFound )
            {
            GoOfflineL( connectedAccounts->At( closeConnection ) );
            }
        }
    CleanupStack::PopAndDestroy(); // connectedAccounts

    }

// ----------------------------------------------------
// CMceUi::GoOfflineL
// ----------------------------------------------------
void CMceUi::GoOfflineL( TMsvId aAccount )
    {
    MCELOGGER_ENTERFN("GoOfflineL()");
    TMsvEntry tentry;
    GetEntryL( aAccount, tentry );
    if ( tentry.Connected() )
        {
        ChangeServiceConnectionStateL( aAccount, EFalse );
        }
    MCELOGGER_LEAVEFN("GoOfflineL()");
    }

// ----------------------------------------------------
// CMceUi::ChangeServiceConnectionStateL
// ----------------------------------------------------
void CMceUi::ChangeServiceConnectionStateL(TMsvId aServiceId, TBool aConnect)
    {
    MCELOGGER_ENTERFN("ChangeServiceConnectionStateL()");

    if ( aConnect )
        {
        LeaveIfDiskSpaceUnderCriticalLevelL();
        }

    TMsvEntry tentry;
    (void)GetEntryL(aServiceId, tentry);
    CBaseMtmUi& ui = iMtmStore->GetMtmUiAndSetContextLC(tentry);
    CMsvOperation* op;
    CAknInputBlock::NewLC();

    CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( singleOpWatcher );

    if ( aConnect )
        {
        op = ui.OpenL( singleOpWatcher->iStatus );
        if ( tentry.iMtm != KSenduiMtmImap4Uid )
            {
            SetViewUpdateSuppressionFlag( ETrue, op );
            }
        if ( iAlwaysOnline && op )
            {
            AddOperationIdL( op->Id(), aServiceId );
            }
        }
    else
        {
        op = ui.CloseL( singleOpWatcher->iStatus );
        if ( iAlwaysOnline && op )
           {
           RemoveOperationId( op->Id(), aServiceId );
           }
        }

    CleanupStack::PushL( op );

    iOperations.AppendL( singleOpWatcher );
    CleanupStack::Pop( 2 ); // singleOpWatcher, op
    singleOpWatcher->SetOperation( op );
    MCELOGGER_WRITE_FORMAT("iOperations.AppendL(%d)", op->Id() );

    CleanupStack::PopAndDestroy( 2 ); // CAknInputBlock, release mtmUi
    if ( iMceListView->ListContainer() && 
        ( MceViewActive(EMceMessageViewActive ) ) )
        {
        if ( aConnect )
            {
            iMceListView->ListContainer()->SetAnchorItemIdL(
                CMceMessageListContainerBase::EMessageListOperationConnect );
            }
        else
            {
            iMceListView->ListContainer()->SetAnchorItemIdL(
                CMceMessageListContainerBase::EMessageListOperationGeneral );
            }
        }

    MCELOGGER_LEAVEFN("ChangeServiceConnectionStateL()");
    }

// ----------------------------------------------------
// CMceUi::GetEntryL
// ----------------------------------------------------
TMsvId CMceUi::GetEntryL(TMsvId aId, TMsvEntry& aEntry) const
    {
    TMsvId service;
    User::LeaveIfError(iSession->GetEntry(aId, service, aEntry));
    return service;
    }

// ----------------------------------------------------
// CMceUi::SendSelectionL
// ----------------------------------------------------
void CMceUi::SendSelectionL(CMsvEntrySelection* aSel)
    {
    CleanupStack::PushL(aSel);
    CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::Pop( aSel ); 
    CleanupStack::PushL( singleOpWatcher ); // guaranteed not to leave because of previous pop

    CMsvOperation* op = CMceSendOperation::NewL(
        *iSession, singleOpWatcher->iStatus, *iMtmStore, aSel );
        // aSel will be deleted!

    CleanupStack::PushL( op );
    iOperations.AppendL( singleOpWatcher );
    CleanupStack::Pop( 2 ); // singleOpWatcher, op
    singleOpWatcher->SetOperation( op );
    MCELOGGER_WRITE_FORMAT("iOperations.AppendL(%d)", op->Id() );
    }

// ----------------------------------------------------
// CMceUi::SendNowL
// ----------------------------------------------------
void CMceUi::SendNowL()
    {

    CMsvEntrySelection * selection = iMceListView->ListContainer()
        ->CurrentItemSelectionL();
    if ( selection->Count() == 0 )
        {
        delete selection;
        return;
        }

    CleanupStack::PushL( selection );

    // if we are sending email just ignore offline checks
    iEntry->SetEntryL( (*selection)[0] );
    const TMsvEntry& entry = iEntry->Entry();

    if ( entry.iMtm != KSenduiMtmSmtpUid &&
        FeatureManager::FeatureSupported( KFeatureIdOfflineMode ))
        {
        // check if offline mode has been set
        TInt offline = 1;

        CRepository* repository = NULL;
        TRAPD( ret, repository = CRepository::NewL(KCRUidCoreApplicationUIs) );
        CleanupStack::PushL( repository );

        if ( ret == KErrNone )
            {
            if ( repository->Get(KCoreAppUIsNetworkConnectionAllowed,offline) != KErrNone )
                {
                offline = 1;
                }
            }

        CleanupStack::Pop( repository );
        delete repository;

        if ( !offline )
            {
            // offline mode has been set
            HBufC* text = StringLoader::LoadLC( R_MCE_OFFLINE_NOT_POSSIBLE, CCoeEnv::Static() );
            CAknInformationNote* note = new (ELeave) CAknInformationNote();
            note->ExecuteLD(*text);
            CleanupStack::PopAndDestroy(2); //text, selection
            return;
            }

        }

    CleanupStack::Pop( selection ); // this is because SendSelectionL pushes selection immediately
    SendSelectionL( selection );
    }

// ----------------------------------------------------
// CMceUi::CancelSendingL
// ----------------------------------------------------
void CMceUi::CancelSendingL()
    {
    CMsvEntrySelection * selection = iMceListView->ListContainer()
        ->CurrentItemSelectionL();
    CleanupStack::PushL( selection );

    if ( selection->Count() == 0 ||
         IsMessageSendingL( selection->At(0) ) )
        {
        CleanupStack::PopAndDestroy( selection );
        return;
        }

    CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( singleOpWatcher );

    CMsvOperation* op = CMceCancelSendingOperation::NewL(
        *iSession,
        singleOpWatcher->iStatus,
        selection->At( 0 ),
        CMceCancelSendingOperation::ECancelOnly,
        *iMtmStore );

    CleanupStack::PushL( op );
    iOperations.AppendL( singleOpWatcher );
    CleanupStack::Pop( 2 ); // singleOpWatcher, op
    singleOpWatcher->SetOperation( op );
    MCELOGGER_WRITE_FORMAT("iOperations.AppendL(%d)", op->Id() );

    CleanupStack::PopAndDestroy( selection );
    }

// ----------------------------------------------------
// CMceUi::FetchNewL
// ----------------------------------------------------
void CMceUi::FetchNewL()
    {
    DoAsyncFunctionOnCurrentContextL( KMtmUiFunctionFetchNew );
    iMceListView->ListContainer()->SetAnchorItemIdL(
        CMceMessageListContainerBase::EMessageListOperationFetchNew );
    }

// ----------------------------------------------------
// CMceUi::FetchSelectedL
// ----------------------------------------------------
void CMceUi::FetchSelectedL()
    {
    __ASSERT_DEBUG( MceViewActive( EMceMessageViewActive ),
        Panic( EMceUiErrCannotFetchFromMainView ) );
    if ( MceViewActive( EMceMainViewActive ) )
        {
        return;
        }

    CMsvEntrySelection* sel = iMceListView->ListContainer()
        ->CurrentItemSelectionL();

    if ( sel->Count() == 0 )
        {
        delete sel;
        return;
        }

    CleanupStack::PushL( sel );

    iMceListView->ListContainer()->SetAnchorItemIdL(
        CMceMessageListContainerBase::EMessageListOperationFetchSelected );
    DoAsyncFunctionOnCurrentContextL(KMtmUiFunctionFetchSelected, *sel);
    CleanupStack::PopAndDestroy( sel );
    }

// ----------------------------------------------------
// CMceUi::FetchAllL
// ----------------------------------------------------
void CMceUi::FetchAllL()
    {
    DoAsyncFunctionOnCurrentContextL(KMtmUiFunctionFetchAll);
    iMceListView->ListContainer()->SetAnchorItemIdL(
        CMceMessageListContainerBase::EMessageListOperationFetchSelected );
    }

// ----------------------------------------------------
// CMceUi::DoAsyncFunctionOnCurrentContextL
// ----------------------------------------------------
void CMceUi::DoAsyncFunctionOnCurrentContextL(TInt aFunctionId)
    {
    CMsvEntrySelection* sel = new (ELeave) CMsvEntrySelection();
    CleanupStack::PushL( sel );

    DoAsyncFunctionOnCurrentContextL(aFunctionId, *sel);

    CleanupStack::PopAndDestroy( sel );
    }

// ----------------------------------------------------
// CMceUi::DoAsyncFunctionOnCurrentContextL
// ----------------------------------------------------
void CMceUi::DoAsyncFunctionOnCurrentContextL(TInt aFunctionId, CMsvEntrySelection& aSelection)
    {
    DoAsyncFunctionL(aFunctionId, aSelection, iMceListView->ListContainer()->FolderEntry());
    }

// ----------------------------------------------------
// CMceUi::DoAsyncFunctionL
// ----------------------------------------------------
void CMceUi::DoAsyncFunctionL(TInt aFunctionId, const TMsvEntry& aContext)
    {
    CMsvEntrySelection* sel= new (ELeave) CMsvEntrySelection();
    CleanupStack::PushL( sel );
    DoAsyncFunctionL(aFunctionId, *sel, aContext);
    CleanupStack::PopAndDestroy( sel );
    }

// ----------------------------------------------------
// CMceUi::DoAsyncFunctionL
// ----------------------------------------------------
void CMceUi::DoAsyncFunctionL( TInt aFunctionId, CMsvEntrySelection& aSelection,
    const TMsvEntry& aContext)
    {
    LeaveIfDiskSpaceUnderCriticalLevelL();

    CBaseMtmUi& mtmUi = iMtmStore->GetMtmUiAndSetContextLC( aContext );

    TBuf8<1> blankParams;
    CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( singleOpWatcher );

    CMsvOperation* op = mtmUi.InvokeAsyncFunctionL( aFunctionId, aSelection,
        singleOpWatcher->iStatus, blankParams );

    if ( CheckTechnologyTypeFromMtm( KMailTechnologyTypeUid, aContext.iMtm ) &&
         (aFunctionId == KMtmUiFunctionFetchNew ||
          aFunctionId == KMtmUiFunctionFetchAll 
        ))
        {
        SetViewUpdateSuppressionFlag( ETrue, op );   
        }

    if ( iAlwaysOnline && op )
        {
        TMsvEntry tEntry;
        TMsvId serviceId;
        User::LeaveIfError( iSession->GetEntry( aContext.iServiceId, serviceId, tEntry ) );
        if ( !tEntry.Connected() )
            {
            AddOperationIdL( op->Id(), serviceId );
            }
        }

    CleanupStack::PushL( op );
    iOperations.AppendL( singleOpWatcher );
    CleanupStack::Pop( 2 ); // singleOpWatcher, op
    singleOpWatcher->SetOperation( op );

    if ( aFunctionId == KMtmUiFunctionFetchNew )
        {
        iFetchNewOperation = singleOpWatcher;
        }

    MCELOGGER_WRITE_FORMAT("iOperations.AppendL(%d)", op->Id() );

    CleanupStack::PopAndDestroy(); // release mtmUI
    iMceListView->ListContainer()->ClearSelection();
    }

// ----------------------------------------------------
// CMceUi::AddMTMFunctionsL
// ----------------------------------------------------
void CMceUi::AddMTMFunctionsL(CEikMenuPane& aMenuPane, TInt aMenuCommandId)
    {
    MCELOGGER_ENTERFN("AddMTMFunctionsL()");
    TMsvEntry selectedEntry;
    TBool foundSelectedEntry = GetCurrentEntryL( selectedEntry );
    TMsvEntry openedEntry;
    TBool foundOpenedEntry = GetCurrentEntryL( openedEntry, ETrue );

    CEikMenuPaneItem::SData data;
    data.iCascadeId=0;
    data.iFlags=0;
    TInt cc;
    TInt sendCmd = EMceCmdFirstMTMFunction;
    const TInt count = iMTMFunctionsArray->Count();
    for ( cc = 0; cc < count; cc++ )
        {
        TMsgFunctionInfo& functionInfo = iMTMFunctionsArray->At(cc);
        data.iText = functionInfo.iCaption;
        data.iCommandId = sendCmd;
        if ( functionInfo.iFlags & EMtudContextSpecific )
            {
            if ( ( foundSelectedEntry && !CheckCommandAvailableL( sendCmd, selectedEntry ) ) ||
                 ( foundOpenedEntry && !CheckCommandAvailableL( sendCmd, openedEntry ) )
                )
                {
                // Enable Item Specific Flag
                if(functionInfo.iFlags & EMtudCommandItemSpecific)
		    {
                    data.iFlags |= EEikMenuItemSpecific;
		    }
                
                if ( functionInfo.iFuncId == KMtmUiFunctionMessageInfo )
                    {
					sendCmd++;
					continue;
                    }

                else if ( functionInfo.iFuncId == KMtmUiFunctionDeliveryStatus )
                    {
                    data.iFlags |= EEikMenuItemSpecific;               
                    aMenuPane.AddMenuItemL( data, EMceCmdUndelete );
                    }

                else if ( functionInfo.iFuncId == KMtmUiFunctionFetchMMS ) //MMS notification
                    {
                    data.iFlags |= EEikMenuItemSpecific; 
                    aMenuPane.AddMenuItemL( data, EAknCmdOpen );
                    }

                else if ( functionInfo.iFuncId == KMtmUiFunctionFetchSyncML ) //SyncML Retrieve
                    {
                    if ( selectedEntry.iMtm == KSenduiMtmSyncMLEmailUid )
                        {
                        aMenuPane.AddMenuItemL( data, EAknCmdOpen );
                        }
                    else
                        {
                        // nothing is added
                        }
                    }

                else if ( functionInfo.iFuncId == KMtmUiFunctionMMBox )
                    {
                    aMenuPane.AddMenuItemL( data, EAknCmdOpen );
                    }

                else
                    {
                    // Following comparison added to hide MCE's Mark as read/Unread 
                    // as duplicate copy of same options was provided by Visto MTM
                    CCoeEnv* ownEikonEnvmak = CEikonEnv::Static();
                    HBufC* markasRead = ownEikonEnvmak->AllocReadResourceLC( R_QTN_MCE_MARKAS_READ );
                    TBufC<30> bufread(markasRead->Des());
                    HBufC* markasUnRead = ownEikonEnvmak->AllocReadResourceLC( R_QTN_MCE_MARKAS_UNREAD );
                    TBufC<30> bufUnread(markasUnRead->Des());
                    CleanupStack::PopAndDestroy(2); //markasUnRead, markasRead

                    // Compare the function Caption with buffered value
                    if ( functionInfo.iCaption.FindF(bufread) == KErrNone 
                       || functionInfo.iCaption.FindF(bufUnread) == KErrNone )
                        {
                        iMceListView->SetMarkReadUnread( ETrue );
                        }

                    aMenuPane.AddMenuItemL( data, aMenuCommandId );
                    }
                }
            }
        else
            {
            if ( MceViewActive( EMceMainViewActive ) && !CheckMTMFunctionL( functionInfo, selectedEntry ) )
                {
                // Enable Item Specific Flag
                if(functionInfo.iFlags & EMtudCommandItemSpecific)
	            {
                    data.iFlags |= EEikMenuItemSpecific;
                    }
                aMenuPane.AddMenuItemL( data, aMenuCommandId );
                }
            }
        sendCmd++;
        }
    MCELOGGER_LEAVEFN("AddMTMFunctionsL()");
    }

// ----------------------------------------------------
// CMceUi::CheckMTMFunctionL
// ----------------------------------------------------
TInt CMceUi::CheckMTMFunctionL( const TMsgFunctionInfo& aFunction, TMsvEntry& aEntryToCheck )
    {
    TInt functionAvailable = KErrNotFound;

    CBaseMtmUiData* uiData = NULL;

    if ( MceViewActive( EMceMainViewActive ) )
        {
        if ( !( aFunction.iFlags & EMtudContextSpecific ) )
            {
            uiData = GetMtmUiDataL( aFunction.iMtmUid );
            if ( uiData )
                {
                functionAvailable = uiData->OperationSupportedL(aFunction.iFuncId, aEntryToCheck);
                }
            }
        }
    else
        {
        if ( aFunction.iMtmUid == aEntryToCheck.iMtm )
            {
            uiData = GetMtmUiDataL( aFunction.iMtmUid );
            }

        if ( aFunction.iFlags & EMtudContextSpecific )
            {
            if ( uiData )
                {
                functionAvailable =
                    uiData->OperationSupportedL(aFunction.iFuncId, aEntryToCheck);
                }
            }
        else if ( aFunction.iFlags & EMtudRemoteOnly )
            {
            if ( uiData &&
                 aEntryToCheck.iMtm != KUidMsvLocalServiceMtm )
                {
                functionAvailable =
                    uiData->OperationSupportedL(aFunction.iFuncId, aEntryToCheck);
                }
            }
        }

    return functionAvailable;
    }

// ----------------------------------------------------
// CMceUi::GetCurrentEntryL
// ----------------------------------------------------
TBool CMceUi::GetCurrentEntryL(TMsvEntry& aEntry, TBool aOpenedFolder /*= EFalse*/) const
    {
    MCELOGGER_ENTERFN("GetCurrentEntryL()");

    TMsvId cursorId = KErrNotFound;

    if ( MceViewActive( EMceMainViewActive ) )
        {
        cursorId = iMceMainView->ListContainer()->CurrentItemId();
        }
    else
        {
        if ( aOpenedFolder )
            {
            cursorId = iMceListView->ListContainer()->CurrentFolderId();
            }
        else
            {
            if ( iMceListView->ListContainer()->CurrentItemSelectionCount() == 1 )
                {
                CMsvEntrySelection* selection =
                    iMceListView->ListContainer()->CurrentItemSelectionL();
                cursorId = selection->At(0);
                delete selection;
                }
            else
                {
                cursorId = KErrNotFound;
                }
            }
        }

    if ( cursorId == KErrNotFound )
        {
        MCELOGGER_LEAVEFN("GetCurrentEntryL() selected entry not found");
        return EFalse;
        }

    TMsvId service;
    const TInt err = iSession->GetEntry(cursorId, service, aEntry);

    if ( err != KErrNone && err != KErrNotFound )
        {
        User::Leave( err );
        }

    MCELOGGER_LEAVEFN("GetCurrentEntryL()");
    return ( err == KErrNone );
    }

// ----------------------------------------------------
// CMceUi::CheckCommandAvailableL
// ----------------------------------------------------
TInt CMceUi::CheckCommandAvailableL( TInt aCommand, TMsvEntry& aEntryToCheck )
    {
    TInt rid = 0; // command is available

    const TBool remoteContext =
        aEntryToCheck.iServiceId != KMsvLocalServiceIndexEntryId;

    switch ( aCommand )
        {
        case EMceCmdUndelete:
            if ( remoteContext )
                {
                CBaseMtmUiData* uiData = GetMtmUiDataL( aEntryToCheck.iMtm );
                if ( !uiData || !uiData->CanUnDeleteFromEntryL(aEntryToCheck, rid) && !rid )
                    {
                    rid = KErrNotSupported;
                    }
                }
            break;

        case EMceCmdReply:
            {
            CBaseMtmUiData* uiData = GetMtmUiDataL( aEntryToCheck.iMtm );
            if ( !uiData || !uiData->CanReplyToEntryL(aEntryToCheck, rid) )
                {
                rid = KErrNotSupported;
                }
            else
                {
                if ( aEntryToCheck.iMtm == KSenduiMtmSmsUid || aEntryToCheck.iMtm == KSenduiMtmBioUid 
                     || aEntryToCheck.iMtm == KSenduiMtmMmsUid || aEntryToCheck.iMtm == KSenduiMMSNotificationUid )
                    {
                    if ( !MceUtils::ValidPhoneNumberL( aEntryToCheck.Id(), *iSession ) )
                        {
                        rid = KErrNotSupported;
                        }
                    }
                
                }
            }
            break;
         
        case EMceCmdForward:
            {
            CBaseMtmUiData* uiData = GetMtmUiDataL( aEntryToCheck.iMtm );
            if ( !uiData || !uiData->CanForwardEntryL( aEntryToCheck, rid ) )
                {
                rid = KErrNotSupported;
                }
            }
            break;   
            
        default:
            if (aCommand >= EMceCmdFirstMTMFunction)
                {
                rid = CheckMTMFunctionL(
                    iMTMFunctionsArray->At( aCommand-EMceCmdFirstMTMFunction ), aEntryToCheck );
                }
            else
                {
                rid = KErrNotSupported;
                }
            break;
        };

    return rid;
    }

// ----------------------------------------------------
// CMceUi::CheckCommandAvailableL
// ----------------------------------------------------
TInt CMceUi::CheckCommandAvailableL( TInt aCommand, CMsvEntrySelection* aEntriesToCheck )
    {
    __ASSERT_DEBUG( aEntriesToCheck, Panic( EMceUiErrArrayEmpty ) );
    TInt rid = 0; // command is available
    if ( !aEntriesToCheck || aEntriesToCheck->Count() == 0 )
        {
#if defined(_DEBUG)
        Panic(EMceUiErrArrayEmpty);
#endif
        return KErrNotSupported;
        }

    TMsvEntry entry;
    TMsvId serviceId;
    TMsvId id = (*aEntriesToCheck)[0];

    if ( iSession->GetEntry( id, serviceId, entry ) != KErrNone )
        {
        return KErrNotSupported;
        }

    if (aCommand >= EMceCmdFirstMTMFunction)
        {
        rid = KErrNotSupported;
        }
    else if ( aCommand == EMceCmdUndelete )
        {
        if ( entry.iServiceId != KMsvLocalServiceIndexEntryId )
            {
            CBaseMtmUiData* uiData = GetMtmUiDataL( entry.iMtm );
            TBool canUndelete = EFalse;
            if ( uiData )
                {
                const TInt count = aEntriesToCheck->Count();
                for ( TInt loop = 0; loop < count && !canUndelete; loop++ )
                    {
                    id = (*aEntriesToCheck)[loop];
                    if ( iSession->GetEntry( id, serviceId, entry ) == KErrNone &&
                         uiData->CanUnDeleteFromEntryL(entry, rid) )
                        {
                        canUndelete = ETrue;
                        }
                    }
                }
            rid = canUndelete;
            }
        else
            {
            rid = KErrNotSupported;
            }
        }

    return rid;
    }


// ----------------------------------------------------
// CMceUi::HandleMTMFunctionL
// ----------------------------------------------------
void CMceUi::HandleMTMFunctionL(const TMsgFunctionInfo& aFunction)
    {
    MCELOGGER_ENTERFN("HandleMTMFunctionL()");

    CMsvEntrySelection* sel = NULL;
    if ( MceViewActive( EMceMainViewActive ) )
        {
        sel = new ( ELeave ) CMsvEntrySelection();
        CleanupStack::PushL( sel );
        sel->AppendL( iMceMainView->ListContainer()->CurrentItemId() );
        }
    else
        {
        sel = iMceListView->ListContainer()->CurrentItemSelectionRefreshL();
        CleanupStack::PushL( sel );
        }


    TMsvId contextId = KMsvNullIndexEntryId;
    if ( aFunction.iFlags&EMtudContextSpecific )
        {
        if ( sel->Count() != 0 )
            {
            contextId = (*sel)[0];
            TMsvId service;
            TMsvEntry entry;
            if ( iSession->GetEntry( contextId, service, entry) != KErrNone ||
                entry.iMtm != aFunction.iMtmUid )
                {
                contextId = KMsvNullIndexEntryId;
                }
            }
        if ( contextId == KMsvNullIndexEntryId )
            {
            CleanupStack::PopAndDestroy( sel );
            return;
            }
        }

    CAknInputBlock::NewLC();

    CBaseMtmUi& mtmUi=iMtmStore->GetMtmUiLC(aFunction.iMtmUid);
    CBaseMtm&   mtm=mtmUi.BaseMtm();
    if (aFunction.iFlags&EMtudContextSpecific)
        {
        if (mtm.HasContext())
            {
            mtm.SwitchCurrentEntryL(contextId);
            }
        else
            {
            CMsvEntry* centry=iSession->GetEntryL(contextId);
            mtm.SetCurrentEntryL(centry);
            }
        }

    TBool tabsToCleanupStack = EFalse;

    if ( iMceUiFlags.MceFlag( EMceUiFlagsTabsActive )
        && aFunction.iFuncId != KMtmUiFunctionMessageInfo )
        {
        // must remove tabs because if no mailboxes created then
        // mailbox settings dialog is displayed and tabs should not be shown then.
        RemoveTabs();
        CleanupStack::PushL( TCleanupItem( ShowTabs, this ) );
        tabsToCleanupStack = ETrue;
        }

    TBuf8<1> buf;
    if (!(aFunction.iFlags&EMtudAsynchronous))
        {
        mtmUi.InvokeSyncFunctionL(aFunction.iFuncId, *sel, buf);
        }
    else
        {
        CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL( *this );
        CleanupStack::PushL( singleOpWatcher );

        CMsvOperation* op=mtmUi.InvokeAsyncFunctionL( aFunction.iFuncId, *sel,
            singleOpWatcher->iStatus, buf );

        if ( op )
            {
            CleanupStack::PushL( op );
            iOperations.AppendL( singleOpWatcher );
            CleanupStack::Pop( 2 ); // singleOpWatcher, op
            singleOpWatcher->SetOperation( op );
            MCELOGGER_WRITE_FORMAT("iOperations.AppendL(%d)", op->Id() );
            }
        else
            {
            CleanupStack::PopAndDestroy( singleOpWatcher );
            }
        }

    if ( tabsToCleanupStack )
        {
        CleanupStack::PopAndDestroy(); // TCleanupItem - will show tabs again
        }

    CleanupStack::PopAndDestroy(3, sel);// sel, CAknInputBlock, release mtmUi

    MCELOGGER_LEAVEFN("HandleMTMFunctionL()");
    }

// ----------------------------------------------------
// CMceUi::SetMceViewActive
// ----------------------------------------------------
void CMceUi::SetMceViewActive( TUint aActiveView )
    {
    iMceActiveView = aActiveView;
    if ( aActiveView & EMceDeliveryReportsViewActive )
        {
        iMceMainView->SetDeliveryReportsSelected();
        }
    }

// ----------------------------------------------------
// CMceUi::MceViewActive
// ----------------------------------------------------
TBool CMceUi::MceViewActive( TInt aActiveView ) const
    {
    return iMceActiveView & aActiveView;
    }

// ----------------------------------------------------
// CMceUi::MoveFromOutboxToDraftsL
// ----------------------------------------------------
void CMceUi::MoveFromOutboxToDraftsL()
    {
    MCELOGGER_ENTERFN("MoveFromOutboxToDraftsL()");
    __ASSERT_DEBUG( MceViewActive( EMceMessageViewActive ),
        Panic( EMceUiErrCannotMoveFromMainView ) );

    LeaveIfDiskSpaceUnderCriticalLevelL();

    CMsvEntrySelection* selection = iMceListView->ListContainer()
        ->CurrentItemSelectionL();
    CleanupStack::PushL( selection );


    if ( selection->Count() == 0 ||
         IsMessageSendingL( selection->At(0) ) )
        {
        CleanupStack::PopAndDestroy( selection );
        MCELOGGER_LEAVEFN("MoveFromOutboxToDraftsL1()");
        return;
        }

    CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( singleOpWatcher );
    CAknInputBlock::NewLC();

    CMsvProgressReporterOperation* op =
        CMsvProgressReporterOperation::NewL(
        *iSession, singleOpWatcher->iStatus, EMbmAvkonQgn_note_move );
    CleanupStack::PushL( op );

    CMsvOperation* subOp = CMceCancelSendingOperation::NewL(
         *iSession,
         op->RequestStatus(),
         selection->At(0),
         CMceCancelSendingOperation::ECancelAndMoveToDrafts,
         *iMtmStore );

    op->SetOperationL(subOp); // this takes ownership immediately, so no cleanupstack needed.

    HBufC* text = StringLoader::LoadLC(
        R_MUIU_LOCAL_PROGRESS_MOVING_1,
        iEikonEnv );
    op->SetTitleL( *text );
    CleanupStack::PopAndDestroy( text );
    CleanupStack::Pop( op );
    singleOpWatcher->SetOperation( op );
    CleanupStack::PopAndDestroy(); // CAknInputBlock
    CleanupStack::PushL( op );
    iOperations.AppendL( singleOpWatcher );
    CleanupStack::Pop(2, singleOpWatcher); // singleOpWatcher, op


    MCELOGGER_WRITE_FORMAT("iOperations.AppendL(%d)", op->Id() );
    iMceListView->ListContainer()->ClearSelection();
    CleanupStack::PopAndDestroy( selection ); 
    MCELOGGER_LEAVEFN("MoveFromOutboxToDraftsL2()");
    }

// ----------------------------------------------------
// CMceUi::ForceMtmLoaderFinish
// ----------------------------------------------------
void CMceUi::ForceMtmLoaderFinish()
    {
    MCELOGGER_ENTERFN("ForceMtmLoaderFinish()");
    if ( iMtmLoader )
        {
        TRAP_IGNORE(iMtmLoader->FinishL(iAudioMsgEnabled, iPostcardEnabled)); // CR : 401-1806
        delete iMtmLoader;
        iMtmLoader = NULL;
        }
    MCELOGGER_LEAVEFN("ForceMtmLoaderFinish()");
    }

// ----------------------------------------------------
// CMceUi::GoOfflineSynchronouslyL
// ----------------------------------------------------
void CMceUi::GoOfflineSynchronouslyL( TMsvId aAccount )
    {
    MCELOGGER_ENTERFN("GoOfflineSynchronouslyL()");
    TMsvEntry tentry;
    (void)GetEntryL(aAccount, tentry);

    if (  iAlwaysOnline && ( tentry.iMtm == KSenduiMtmImap4Uid
        || tentry.iMtm == KSenduiMtmPop3Uid ) )
        {
        if ( !ServiceIdFound( tentry.Id()) )
            {
            // do not close always online connection
            return;
            }
        }

    CBaseMtmUi& ui=iMtmStore->GetMtmUiAndSetContextLC(tentry);
    CAknInputBlock::NewLC();

    CMuiuOperationWait* wait =
        CMuiuOperationWait::NewLC(EActivePriorityWsEvents+10);
    CMsvOperation* op=ui.CloseL(wait->iStatus);
    wait->Start();
    CleanupStack::PopAndDestroy( wait );
    delete op;
    CleanupStack::PopAndDestroy( 2 ); // CAknInputBlock, release mtmui
    MCELOGGER_LEAVEFN("GoOfflineSynchronouslyL()");
    }

// ----------------------------------------------------
// CMceUi::IsEditorOpen
// ----------------------------------------------------
TBool CMceUi::IsEditorOpen( ) const
    {
    return ( iEditorOperation ? ETrue : EFalse );
    }

// ----------------------------------------------------
// CMceUi::LeaveIfDiskSpaceUnderCriticalLevelL
// Static function.
// Used in CreateNewMessageL as TCleanupItem.
// ----------------------------------------------------
void CMceUi::LeaveIfDiskSpaceUnderCriticalLevelL( TInt aBytesToWrite )
    {
    MCELOGGER_WRITE_FORMAT("LeaveIfDiskSpaceUnderCriticalLevelL: %d", aBytesToWrite );


    if ( iMceUiFlags.MceFlag( EMceUiFlagsMediaUnavailable ) )
        {
        MCELOGGER_WRITE("LeaveIfDiskSpaceUnderCriticalLevelL: media unavailable, leave");
        User::Leave( KMsvIndexBackup );
        }

    if ( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL(*iSession, aBytesToWrite) )
        {
        User::Leave( KErrDiskFull );
        }
    MCELOGGER_WRITE("LeaveIfDiskSpaceUnderCriticalLevelL: OK to proceed");
    }

// ----------------------------------------------------
// CMceUi::CancelFreeDiskSpaceRequest
// Static function.
// Used in CreateNewMessageL as TCleanupItem.
// ----------------------------------------------------
void CMceUi::CancelFreeDiskSpaceRequest(TAny* /*aAny*/)
    {
    MCELOGGER_ENTERFN("CancelFreeDiskSpaceRequest()");

// Cancel the free disk space request to be added

    MCELOGGER_LEAVEFN("CancelFreeDiskSpaceRequest()");
    }

// ----------------------------------------------------
// CMceUi::TitlePaneL
// Returns mce ui's title pane
// ----------------------------------------------------
CAknTitlePane* CMceUi::TitlePaneL() const
    {
    return (CAknTitlePane *)(iEikonEnv->AppUiFactory(*this))->StatusPane()->ControlL(
        TUid::Uid(EEikStatusPaneUidTitle));
    }

// ----------------------------------------------------
// CMceUi::IsMessageSendingL
//
// ----------------------------------------------------
TBool CMceUi::IsMessageSendingL( TMsvId aEntryId ) const
    {
    MCELOGGER_ENTERFN("IsMessageSendingL()");
    MCELOGGER_WRITE_FORMAT( "Check entry 0x%x", aEntryId );

    TBool sending = EFalse;
    iEntry->SetEntryL( aEntryId );
    const TMsvEntry& entry = iEntry->Entry();
    if ( entry.Parent() == KMsvGlobalOutBoxIndexEntryId &&
         entry.SendingState() == KMsvSendStateSending &&
         ( entry.iMtm == KSenduiMtmSmsUid ||
         entry.iMtm == KSenduiMtmMmsUid )
       )
        {
                sending = ETrue;
        }
    MCELOGGER_WRITE_FORMAT( "Entry sending: %d", sending);
    MCELOGGER_LEAVEFN("IsMessageSendingL()");
    return sending;
    }

// ----------------------------------------------------
// CMceUi::ActivateConnectionQueryTimerL
//
// ----------------------------------------------------
void CMceUi::ActivateConnectionQueryTimerL( TMsvId aEntryId )
    {
    MCELOGGER_ENTERFN("ActivateConnectionQueryTimerL()");
    if ( !iConnectMailboxTimer )
        {
        iConnectMailboxTimer = CMceConnectMailboxTimer::NewL( *this );
        }

    TMsvEntry tentry;
    TMsvId serviceId = GetEntryL( aEntryId, tentry );
    if ( tentry.iMtm != KSenduiMtmImap4Uid &&
         tentry.iMtm != KSenduiMtmPop3Uid )
        {
        // not mailbox, do nothing.
        return;
        }

    if ( serviceId != aEntryId )
        {
        // lets find out that service...
        GetEntryL( serviceId, tentry );
        }
    if ( !tentry.Connected() )
        {
        // this cancels possible old timer
        iConnectMailboxTimer->SetTimer( serviceId );
        }
    MCELOGGER_LEAVEFN("ActivateConnectionQueryTimerL()");
    }

// ----------------------------------------------------
// CMceUi::ReplyL
//
// ----------------------------------------------------
void CMceUi::ReplyL(TInt /*aReplyCommand*/)
    {
    MCELOGGER_ENTERFN("ReplyL()");
    LeaveIfDiskSpaceUnderCriticalLevelL( iDiskSpaceForMoveOrCopy );

    if ( MceViewActive( EMceMainViewActive ) )
        {
        return;
        }

    CMsvEntrySelection * selection = iMceListView->ListContainer()->CurrentItemSelectionL();
    CleanupStack::PushL( selection );
    if ( selection->Count() <= 0)
        {
        CleanupStack::PopAndDestroy( selection );
        return;
        }

    TMsvEntry entry;
    TMsvId serviceId;
    User::LeaveIfError( iSession->GetEntry(selection->At(0),serviceId,entry) ); // original message?

    // NCN reset, received mail has been replied before opening it
    if ( ((entry.iMtm == KSenduiMtmImap4Uid)
        || (entry.iMtm == KSenduiMtmPop3Uid)
        || (entry.iMtm == KSenduiMtmSyncMLEmailUid)
        || CheckTechnologyTypeFromMtm( KMailTechnologyTypeUid, entry.iMtm ))
        && entry.Unread() )
        {
        
        HandleNotif(iMceListView->ListContainer()->FolderEntry().iServiceId);

        }

    CleanupStack::PopAndDestroy( selection ); 

    TMsvPartList parts=KMsvMessagePartBody|KMsvMessagePartOriginator|KMsvMessagePartDescription;

    // Mark local entry as read before replying, remote entries should be handled by mtm ui
    if ( entry.Unread() && entry.iServiceId == KMsvLocalServiceIndexEntryId )
        {
        iEntry->SetEntryL( entry.Id() );
        TMsvEntry entry = iEntry->Entry();
        entry.SetUnread( EFalse );
        iEntry->ChangeL( entry );
        }

    if ( entry.iBioType == KMsgBioUidPictureMsg.iUid)
        {
        //picture message
        TMsvId origMsgId = entry.Id();

        // Change the MTM type of msg temporarily

        CMsvEntry* cEntry = iSession->GetEntryL( origMsgId );
        CleanupStack::PushL( cEntry );
        TMsvEntry tentry( cEntry->Entry() );
        const TUid origMtmUid = tentry.iMtm;
        tentry.iMtm = KSenduiMtmSmsUid;

        // Create registry instances
        CClientMtmRegistry* mtmReg = CClientMtmRegistry::NewL( *iSession );
        CleanupStack::PushL(mtmReg);
        CMtmUiRegistry* mtmUiReg = CMtmUiRegistry::NewL( *iSession );
        CleanupStack::PushL(mtmUiReg);

        // Create the SMS MTM
        CBaseMtm* mtm = mtmReg->NewMtmL( KSenduiMtmSmsUid );
        CleanupStack::PushL(mtm);
        CBaseMtmUi* mtmUi = mtmUiReg->NewMtmUiL( *mtm );
        CleanupStack::PushL(mtmUi);

        // Trap errors, so we can try to restore the original MTM Uid

        TRAPD(err,
            {

            cEntry->ChangeL( tentry );

            // Set its context
            mtm->SwitchCurrentEntryL( origMsgId );

            mtmUi->SetPreferences( mtmUi->Preferences() | EMtmUiFlagEditorPreferEmbedded );
            CAknInputBlock::NewLC();

            CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL( *this );
            CleanupStack::PushL( singleOpWatcher );

            CMsvOperation* oper = mtmUi->ReplyL(
                KMsvDraftEntryIdValue, parts, singleOpWatcher->iStatus );
            iEditorOperation = singleOpWatcher;

            CleanupStack::PushL( oper );
            iOperations.AppendL( singleOpWatcher );
            CleanupStack::Pop( 2, singleOpWatcher ); // oper
            singleOpWatcher->SetOperation( oper );
            CleanupStack::PopAndDestroy(  ); // CAknInputBlock

            });

        // Restore original Mtm Uid
        tentry = cEntry->Entry();
        tentry.iMtm = origMtmUid;
        cEntry->ChangeL(tentry);
        mtm->SwitchCurrentEntryL( origMsgId );

        User::LeaveIfError(err);
        CleanupStack::PopAndDestroy( 5 ); // mtmUi, mtmReg, mtmUiReg, mtm, cEntry
        }

    else
        {
        CBaseMtmUi& ui = iMtmStore->GetMtmUiAndSetContextLC( entry );
        ui.SetPreferences( ui.Preferences() | EMtmUiFlagEditorPreferEmbedded );
        CAknInputBlock::NewLC();
        CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL( *this );
        CleanupStack::PushL( singleOpWatcher );

        CMsvOperation* oper = ui.ReplyL(
            KMsvDraftEntryIdValue, parts, singleOpWatcher->iStatus );
        iEditorOperation = singleOpWatcher;

        CleanupStack::PushL( oper );
        iOperations.AppendL( singleOpWatcher );
        CleanupStack::Pop( 2, singleOpWatcher ); // oper
        singleOpWatcher->SetOperation( oper );
        MCELOGGER_WRITE_FORMAT("iOperations.AppendL(%d)", oper->Id() );
        CleanupStack::PopAndDestroy( 2 ); // CAknInputBlock, ui
        }
    MCELOGGER_LEAVEFN("ReplyL()");

    }

// ----------------------------------------------------
// CMceUi::MarkAsReadL
// ----------------------------------------------------
void CMceUi::MarkAsReadL( TBool aRead )
    {
    MCELOGGER_ENTERFN("MarkAsReadL()");
    CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL(singleOpWatcher);

    CMsvEntrySelection* sel = iMceListView->ListContainer()
        ->CurrentItemSelectionL();
    CleanupStack::PushL( sel );
    TInt selCount = sel->Count();
    if ( selCount <= 0 )
        {
        CleanupStack::PopAndDestroy( 2 ); // singleOpWatcher, sel
        return;
        }

    TMsvId serviceId;
    TMsvEntry entry;
    User::LeaveIfError( iSession->GetEntry( sel->At(0), serviceId, entry ) );
    CBaseMtmUi& mtmUi = iMtmStore->GetMtmUiLC( entry.iMtm );
    mtmUi.SetPreferences( mtmUi.Preferences() | EMtmUiFlagEditorPreferEmbedded );
    CleanupStack::PopAndDestroy( ); // mtmUi

    CMsvProgressReporterOperation* op =
        CMsvProgressReporterOperation::NewL(
            *iSession, singleOpWatcher->iStatus );
    CleanupStack::PushL( op );
    HBufC* text = StringLoader::LoadLC( R_MCE_MARK_AS_READ_PROGRESS, iEikonEnv );
    op->SetTitleL( *text );
    CleanupStack::PopAndDestroy( text );

    // Determine the action for unread flag
    TInt type =
            aRead ? CMceRemoveNewFlag::EMceRemoveEntryFlagUnread :
                    CMceRemoveNewFlag::EMceRemoveEntryFlagNew;
    CMceRemoveNewFlag* subOp = CMceRemoveNewFlag::NewL(
    type, *iSession, op->RequestStatus(), sel );

    op->SetOperationL( subOp ); // this takes ownership immediately, so no cleanupstack needed.
    if ( selCount ==  1 )
        {
        op->MakeProgressVisibleL(EFalse);
        }
    op->SetProgressDecoder( *subOp );
    CleanupStack::Pop( op );

    iOperations.AppendL( singleOpWatcher );
    CleanupStack::Pop( 2, singleOpWatcher ); // singleOpWatcher, sel
    singleOpWatcher->SetOperation( op );
    iMceListView->ListContainer()->ClearSelection();
    if ( CheckTechnologyTypeFromMtm( KMailTechnologyTypeUid, entry.iMtm )&& aRead )
        {
        HandleNotif(iMceListView->ListContainer()->FolderEntry().iServiceId);
        }
    MCELOGGER_WRITE_FORMAT( "iOperations.AppendL(%d)", op->Id() );
    MCELOGGER_LEAVEFN("MarkAsReadL()");
    }

// ----------------------------------------------------
// CMceUi::TabChangedL
//
// ----------------------------------------------------
void CMceUi::TabChangedL(TInt aIndex)
    {
    TBool bOBoxOrMBox = EFalse ;
    if ( !MceViewActive( EMceMessageViewActive )
        && !MceViewActive( EMceDeliveryReportsViewActive ) )
        {
        return;
        }
    TInt count = iTabsArray->Count();
    TMsvId newId = KMsvGlobalInBoxIndexEntryId;
    if ( aIndex < count )
        {
        newId = (*iTabsArray)[aIndex];
        }

    CancelMailboxTimer();

    if ( MceViewActive( EMceDeliveryReportsViewActive ) )
        {
        OpenFolderViewL( newId );
        }
    else if ( !MceViewActive( EMceDeliveryReportsViewActive )
            && newId == KMceDeliveryReportsListIdValue )
        {
        // activate delivery reports view
        ActivateLocalViewL( KMceDeliveryReportViewId );
        }
    else
        {
        TBool bIsFromMailbox = EFalse ;
        TBool bIsToMailbox = EFalse ;
        TUid mtm = iMceListView->ListContainer()->FolderEntry().iMtm ;
        bIsFromMailbox = CheckTechnologyTypeFromMtm( KMailTechnologyTypeUid, mtm );
        
        TMsvEntry currentEntry;
        TMsvId serviceId; // not used here but needed by GetEntry function
        if ( iSession->GetEntry( newId, serviceId, currentEntry )
        	== KErrNone )
	        {
	        bIsToMailbox = CheckTechnologyTypeFromMtm( KMailTechnologyTypeUid, currentEntry.iMtm ); 	
	        }
        
        TMsvId oldId = iMceListView->ListContainer()->FolderEntry().Id();
        if (oldId == KMsvGlobalOutBoxIndexEntryId || newId == KMsvGlobalOutBoxIndexEntryId
            || bIsFromMailbox || bIsToMailbox)
	        {
		        if (bIsFromMailbox || bIsToMailbox || 
		            oldId == KMsvGlobalOutBoxIndexEntryId || iMceListView->ListContainer()->MceListId() != EMceListTypeTwoRow)
		        {
		        bOBoxOrMBox = ETrue ;
		        iLocalScreenClearer = CAknLocalScreenClearer::NewL( EFalse );	
		        }
	        }
        iMceListView->ChangeFolderAndRefreshListboxL( newId );
        }
     
     if (bOBoxOrMBox)
	     {
	     delete iLocalScreenClearer;
	     iLocalScreenClearer = NULL;
	     }
    }

// ----------------------------------------------------
// CMceUi::IsPresent
// ----------------------------------------------------
TBool CMceUi::IsPresent( TUid aMtm ) const
    {
    return iUiRegistry->IsPresent( aMtm );
    }

// ----------------------------------------------------
// CMceUi::ClearExitAfterDisconnectFlag
//
// ----------------------------------------------------
void CMceUi::ClearExitAfterDisconnectFlag( )
    {
    iMceUiFlags.ClearMceFlag( EMceUiFlagsExitAfterDisconnect  );
    }

// ----------------------------------------------------
// CMceUi::SetDontExitOnNextOperationComplete
//
// ----------------------------------------------------
void CMceUi::SetDontExitOnNextOperationComplete( )
    {
    MCELOGGER_WRITE("CMceUi::SetDontExitOnNextOperationComplete");
    iMceUiFlags.SetMceFlag( EMceUiFlagsDoNotExitAfterNextOperationComplete );
    if ( iCancelExitFlagOperation && !( iCancelExitFlagOperation->IsActive() ) )
        {
        iCancelExitFlagOperation->Start( 
            TCallBack( ClearDontExitOnNextOperationComplete, this ) );
        }
    }

// ----------------------------------------------------
// CMceUi::ClearDontExitOnNextOperationComplete
//
// ----------------------------------------------------
void CMceUi::ClearDontExitOnNextOperationComplete( )
    {
    MCELOGGER_WRITE("CMceUi::ClearDontExitOnNextOperationComplete");
    iMceUiFlags.ClearMceFlag( EMceUiFlagsDoNotExitAfterNextOperationComplete );
    }

// ----------------------------------------------------
// CMceUi::ClearDontExitOnNextOperationComplete
//
// ----------------------------------------------------
TInt CMceUi::ClearDontExitOnNextOperationComplete( TAny* aSelf )
    {
    reinterpret_cast<CMceUi*>(aSelf)->ClearDontExitOnNextOperationComplete();
    return KErrNone;
    }

// ----------------------------------------------------
// CMceUi::SyncMlNewMessageItemL
// ----------------------------------------------------
void CMceUi::SyncMlNewMessageItemL( const CMsvEntrySelection* aSelection, TInt aEvent )
    {
    TMsvId serviceId;
    TMsvEntry entry;
    if ( aEvent == EMsvEntriesCreated )
        {
        if ( iSession->GetEntry( aSelection->At(0), serviceId, entry ) == KErrNone )
            {
            if ( entry.iMtm == KSenduiMtmSyncMLEmailUid )
                {
                HandleMTMChangeL();
                }
            }
        }
    else
        {
        if ( aEvent == EMsvEntriesDeleted )
            {
            CMsvEntrySelection* sel=iRootEntry->ChildrenWithTypeL( KUidMsvServiceEntry );
            CleanupStack::PushL( sel );
            const TMsvEntry* tentry=NULL;
            TInt syncMl = 0;
            for ( TInt cc=sel->Count(); --cc>=0 && !syncMl; )
                {
                tentry=&(iRootEntry->ChildDataL((*sel)[cc]));
                if ( tentry->iMtm == KSenduiMtmSyncMLEmailUid )
                    {
                    syncMl++;
                    }
                }
            if ( !syncMl )
                {
                HandleMTMChangeL();
                }

            CleanupStack::PopAndDestroy( sel );
            }
        }
    }

// ----------------------------------------------------
// CMceUi::SyncMlMoveFromOutboxToDraftsL
// ----------------------------------------------------
void CMceUi::SyncMlMoveFromOutboxToDraftsL( )
    {
    LeaveIfDiskSpaceUnderCriticalLevelL( iDiskSpaceForMoveOrCopy );

    __ASSERT_DEBUG( MceViewActive( EMceMessageViewActive ),
        Panic( EMceUiErrCannotMoveFromMainView ) );
    if ( MceViewActive( EMceMainViewActive ) )
        {
        return;
        }

    const TMsvId sourceId = iMceListView->ListContainer()->CurrentFolderId();
    TMsvEntry srcEntry;
    GetEntryL( sourceId, srcEntry );

    CMsvEntrySelection * selection = iMceListView->ListContainer()
        ->CurrentItemSelectionL();
    CleanupStack::PushL( selection );

    if (  selection->Count() <= 0 )
        {
        CleanupStack::PopAndDestroy( selection );
        return;
        }

    CMsvOperation* op=NULL;

    CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( singleOpWatcher );

    CAknInputBlock::NewLC();

    CBaseMtmUi& ui=iMtmStore->GetMtmUiAndSetContextLC( srcEntry );
    op=ui.MoveFromL( *selection, KMsvDraftEntryId, singleOpWatcher->iStatus );
    CleanupStack::PopAndDestroy(); // release ui

    CleanupStack::PopAndDestroy(); // CAknInputBlock::NewLC()
    CleanupStack::PushL(op);
    iOperations.AppendL( singleOpWatcher );
    CleanupStack::Pop( 2 ); // singleOpWatcher, op
    singleOpWatcher->SetOperation( op );

    iMceListView->ListContainer()->ClearSelection();
    CleanupStack::PopAndDestroy( selection ); 
    }

// ----------------------------------------------------
// CMceUi::MtmName
//
// ----------------------------------------------------
THumanReadableName CMceUi::MtmName( TUid aMtm ) const
    {
    THumanReadableName data;
    const TInt count = iMsgTypesSettings.Count();
    for ( TInt cc = 0; cc < count; cc++ )
        {
        TUidNameInfo info = iMsgTypesSettings.At(cc);
        if ( info.iUid == aMtm )
            {
            data = info.iName;
            }
        }
    return data;
    }

// ----------------------------------------------------
// CMceUi::SetChangeMessageStore
//
// ----------------------------------------------------
void CMceUi::SetChangeMessageStore( TBool aChangeEnded )
    {
    if ( aChangeEnded )
        {
        iMceUiFlags.SetMceFlag( EMceUiFlagsExitOnMsvMediaAvailableEvent );
        }
    else
        {
        iMceUiFlags.ClearMceFlag( EMceUiFlagsExitOnMsvMediaAvailableEvent );
        }
    }


// ----------------------------------------------------
// CMceUi::IsSyncronizingL
//
// ----------------------------------------------------
TBool CMceUi::IsSyncronizingL( const TMsvEntry& entry )
    {
    TBool isSyncronizing = EFalse;

    if ( IsPresent( KSenduiMtmSyncMLEmailUid ) &&
            entry.iMtm == KSenduiMtmSyncMLEmailUid )
        {
        CBaseMtmUiData* uiData= GetMtmUiDataL( KSenduiMtmSyncMLEmailUid );
        if ( uiData )
            {
            TInt reason = 0;
            TBool syncronizing = uiData->CanOpenEntryL( entry, reason );
            if ( !syncronizing )
                {
                //cannot open during syncronizing
                HBufC* text = StringLoader::LoadLC( reason, iEikonEnv );
                CAknInformationNote* note = new (ELeave) CAknInformationNote();
                note->ExecuteLD(*text);
                CleanupStack::PopAndDestroy(); //text
                isSyncronizing = ETrue;
                }
            }
        }

    return isSyncronizing;
    }

// ----------------------------------------------------
// CMceUi::AlwaysOnlineLastConnectionL
//
// ----------------------------------------------------
TBool CMceUi::AlwaysOnlineLastConnectionL( const TMsvId aAccount )
    {
    TBool connectionMade = ETrue;

    CImumInSettingsData* settings =
        iEmailApi->MailboxServicesL().LoadMailboxSettingsL( aAccount );
    CleanupStack::PushL( settings );

    if ( iMceMainView->ListContainer()->ListItems()->
        AlwaysOnlineAccountL( aAccount, *settings ) )
        {
        TInt lastUpdateFailed = 0;
        settings->GetAttr(
            TImumInSettings::EKeyInfoLastUpdateFailed, lastUpdateFailed );
        if ( lastUpdateFailed )
            {
            TInt64 lastTime = 0;
            settings->GetAttr(
                TImumInSettings::EKeyInfoLastSuccessfulUpdate, lastTime );
            TTime settingsTime( lastTime );
            TBuf<KMceMaxDateString> time;
            TBuf<KMceMaxDateString> dateTime;

            HBufC* timeFormat = iCoeEnv->
                AllocReadResourceLC( R_QTN_TIME_USUAL_WITH_ZERO );
            HBufC* dateFormat = iCoeEnv->
                AllocReadResourceLC( R_QTN_DATE_USUAL_WITH_ZERO );

            TTime currentTime;
            currentTime.HomeTime();
            TDateTime currentDate( currentTime.DateTime() ); //current date
            TDateTime currentDateSettings( settingsTime.DateTime() ); //settings date

            TBool connBeforeToday = EFalse;
            if ( currentDate.Year() > currentDateSettings.Year() )
                {
                connBeforeToday = ETrue;
                }
            else
                if ( currentDate.Month() > currentDateSettings.Month() )
                    {
                    connBeforeToday = ETrue;
                    }
            else
                if ( currentDate.Day() > currentDateSettings.Day() )
                    {
                    connBeforeToday = ETrue;
                    }

            settingsTime.FormatL( time, *timeFormat );
            AknTextUtils::LanguageSpecificNumberConversion( time );

            settingsTime.FormatL( dateTime, *dateFormat );
            AknTextUtils::LanguageSpecificNumberConversion( dateTime );
            CleanupStack::PopAndDestroy( 2, timeFormat ); // timeFormat, dateFormat

            CAknQueryDialog* confDialog = CAknQueryDialog::NewL();
            HBufC* text = NULL;
            if ( connBeforeToday ) //last connection failed before today
                {
                CDesCArrayFlat* string=new(ELeave)CDesCArrayFlat( KMceArrayGranularity );
                CleanupStack::PushL( string );
                string->AppendL( dateTime );
                string->AppendL( time );
                text = StringLoader::LoadLC(
                    R_MCE_SETTINGS_ALWAYS_LASTTIME2, *string, iEikonEnv );
                }
            else
                {
                text = StringLoader::LoadLC(
                    R_MCE_SETTINGS_ALWAYS_LASTTIME, time, iEikonEnv );
                }

            confDialog->SetPromptL( *text );
            CleanupStack::PopAndDestroy( text );
            if ( connBeforeToday ) //last connection failed before today
                {
                CleanupStack::PopAndDestroy(); 
                }

            connectionMade = EFalse;

            if ( confDialog->ExecuteLD( R_MCE_CONFIRMATION ) )
                {
                GoOnlineL( aAccount );
                }

            }
        }

    CleanupStack::PopAndDestroy( 1 ); // extendedMailSettings
    return connectionMade;
    }

// ----------------------------------------------------
// CMceUi::AddOperationIdL
//
// ----------------------------------------------------
void CMceUi::AddOperationIdL( const TMsvOp& aOp, const TMsvId& aServiceId )
    {
    if ( !iMailAccountItemArray )
        {
        return;
        }

    TInt count = iMailAccountItemArray->Count();
    TInt found = EFalse;
    for ( TInt cc = 0; cc < count && !found; cc++ )
        {
        const TMceMailAccountItem& item = ((*iMailAccountItemArray)[cc]);
        if ( item.iServiceId == aServiceId )
            {
            ((*iMailAccountItemArray)[cc]).iMceConnection = ETrue;
            ((*iMailAccountItemArray)[cc]).iOp = aOp;
            found = ETrue;
            }
        }
    if ( !found )
        {
        TMceMailAccountItem item;
        item.iServiceId = aServiceId;
        item.iMceConnection = ETrue;
        item.iOp = aOp;
        iMailAccountItemArray->AppendL( item );
        }
    }

// ----------------------------------------------------
// CMceUi::RemoveOperationId
//
// ----------------------------------------------------
void CMceUi::RemoveOperationId( const TMsvId& /*aOpId*/, const TMsvId& aServiceId )
    {
    if ( !iMailAccountItemArray )
        {
        return;
        }

    TInt count = iMailAccountItemArray->Count();

    for ( TInt loop=count-1; loop>=0; loop-- )
        {
        TMceMailAccountItem item = ((*iMailAccountItemArray)[loop]);
        if ( item.iServiceId == aServiceId )
            {
            iMailAccountItemArray->Delete( loop );
            break;
            }
        }
    }

// ----------------------------------------------------
// CMceUi::ServiceIdFound
//
// ----------------------------------------------------
TBool CMceUi::ServiceIdFound( const TMsvId& aServiceId )
    {
    TInt found = EFalse;

    TInt count = iMailAccountItemArray->Count();

    for ( TInt cc = 0; cc < count && !found; cc++ )
        {
        TMceMailAccountItem item = ((*iMailAccountItemArray)[cc]);
        if ( item.iServiceId == aServiceId && item.iMceConnection  )
            {
            found = ETrue;
            }
        }
    return found;
    }

// ----------------------------------------------------
// CMceUi::AlwaysOnline
//
// ----------------------------------------------------
TBool CMceUi::AlwaysOnline( )
    {    
    return iAlwaysOnline;
    }

// ----------------------------------------------------
// CMceUi::IsMessageSetToBeDeletedFromServer
//
// ----------------------------------------------------
TBool CMceUi::IsMailSetToBeDeletedFromServerL( TMsvId aItemId )
    {
    TInt rid = 0;
    TBool ret = EFalse;
    TMsvId serviceId;
    TMsvEntry currentEntry;

    iSession->GetEntry( aItemId, serviceId, currentEntry );

    CBaseMtmUiData* mtmUiData = GetMtmUiDataL( currentEntry.iMtm );

    if ( mtmUiData && mtmUiData->CanUnDeleteFromEntryL( currentEntry, rid ) )
        {
        ret = ETrue;
        }

    return ret;
    }

// ----------------------------------------------------
// CMceUi::AnchoredItem
//
// ----------------------------------------------------
//
void CMceUi::DefineAnchorIdL( const CMsvEntrySelection* /*aSelection*/ )
    {
#if 0    
    if ( IsMailSetToBeDeletedFromServerL() )
        {
        SetAnchorId( iMceListView->ListContainer()->CurrentItemId() );
        return;
        }
    // TODO
    TInt currentItem = iMceListView->ListContainer()->CurrentItemIndex();
    CMceMessageListItemArray* items = iMceListView->ListContainer()->ListItems();
    iAnchorId = NULL;
    
// Do not change the focus if mailbox is offline and currently selected item is set to be deleted from server
    if ( IsMailSetToBeDeletedFromServerL() )
        {
        SetAnchorId( iMceListView->ListContainer()->CurrentItemId() );
        return;
        }
    
    for ( TInt i = currentItem; i < items->MdcaCount(); i++ ) // Find forwards
        {
        if ( aSelection->Find( items->ItemId( i ) ) == KErrNotFound )
            {
            SetAnchorId( items->ItemId( i ) );
            break;
            }
        }

    if ( !iAnchorId ) // If not found in forward search, find backwards
        {
        for ( TInt i = currentItem; i >= 0; i-- )
            {
            if ( aSelection->Find( items->ItemId( i ) ) == KErrNotFound )
                {
                SetAnchorId( items->ItemId( i ) );
                break;
                }
            }
        }
#endif        
    }

// ----------------------------------------------------
// CMceUi::AnchoredItem
//
// ----------------------------------------------------
TMsvId CMceUi::AnchoredItem()
    {
    return iAnchorId;
    }

// ----------------------------------------------------
// CMceUi::SetAnchorId
//
// ----------------------------------------------------
void CMceUi::SetAnchorId( TMsvId aAnchorId )
    {
    iAnchorId = aAnchorId;
    }

// ----------------------------------------------------
// CMceUi::EventL
//
// ----------------------------------------------------
void CMceUi::EventL( const CConnMonEventBase &aConnMonEvent )
    {
    switch ( aConnMonEvent.EventType() )
        {
        case EConnMonNetworkRegistrationChange:
            {
            CConnMonNetworkRegistrationChange* event =
                ( CConnMonNetworkRegistrationChange* ) &aConnMonEvent;                
            
            if ( iMceMainView->ListContainer() ) // Cannot set roaming if ListContainer is empty
                {
                CMceMainViewListItemArray* array = iMceMainView->ListContainer()->ListItems();
                if ( array && (array->Count() > 0) ) // List item array should not be empty
                    {
                    // Set roaming status
                    array->SetRoaming( event->RegistrationStatus() == ENetworkRegistrationRoaming );
                    
                    if ( MceViewActive( EMceMainViewActive ) )
                        {
                        //update icon only if main view is open
                        iMceMainView->ListContainer()->DrawDeferred();
                        }
                    }
                }
            }
            break;

        default:
            break;
        }
    }
// ----------------------------------------------------
// CMceUi::HandleGainingForeground
//
// ----------------------------------------------------
void CMceUi::HandleGainingForeground() // CR : 401-1806
    {
   
    // This code is added to remove screen clearer when returning from viewer.
    if ( iMceListView )
        {
        CAknLocalScreenClearer** localScreenClearer = NULL; 
        iMceListView->GetLocalScreenClearer( localScreenClearer );
        delete *localScreenClearer;
        *localScreenClearer = NULL;// this will assign null to iLocalScreenClearer in messagelistview.
        }
    if ( !iFeatureMgrEnabled )
        {
  	    TRAP_IGNORE( FeatureManager::InitializeLibL() );
  	    iFeatureMgrEnabled = ETrue ;
        }
  	TBool newPostcardVal = EFalse;
    TBool newAudioMsgVal = EFalse;
    newAudioMsgVal = FeatureManager::FeatureSupported( KFeatureIdAudioMessaging );
    newPostcardVal = FeatureManager::FeatureSupported( KFeatureIdMmsPostcard );
    
    if ( newAudioMsgVal != iAudioMsgEnabled || newPostcardVal != iPostcardEnabled )
        {
        TRAP_IGNORE( HandleMTMChangeL() );
        }
    
    if ( MceViewActive( EMceMainViewActive ) && iIadUpdateVal)
        {      
        if(!iMceIAUpdate) // first time messaging view is activated
            {
            TRAP_IGNORE(iMceIAUpdate = CMceIAUpdateUtils::NewL(*this));     
            if(iMceIAUpdate)
                {
                CheckIAUpdate();
                }
            }
        else
            {
            if(iMceIAUpdate->IsUpdateRequired())
                {
                CheckIAUpdate();
                }
            }
        }
    }

// ----------------------------------------------------
// CMceUi::HandleLosingForeground
//
// ----------------------------------------------------
void CMceUi::HandleLosingForeground() // CR : 401-1806
    {
    iAudioMsgEnabled = FeatureManager::FeatureSupported( KFeatureIdAudioMessaging );
    iPostcardEnabled = FeatureManager::FeatureSupported( KFeatureIdMmsPostcard ) ;
    if ( iFeatureMgrEnabled ) 
        {
  	    FeatureManager::UnInitializeLib() ;
  	    iFeatureMgrEnabled = EFalse ;
  	    }
  	}
    
// ----------------------------------------------------
// CMceUi::ToPhoneMemoryQueryL
//
// ----------------------------------------------------
void CMceUi::ToPhoneMemoryQueryL( TBool aShowQuery )
    {

    if ( iPhoneMemoryQuery )
        {
        // If the dialog for whitching the message store is allready visible,
        // dismiss it
        delete iPhoneMemoryQuery;
        iPhoneMemoryQuery = NULL;
        return;
        }

    //this method is run when message centre is started
    if ( iMemoryInUse && !iMessageStoreExist && aShowQuery )
        {
        // check, if message store was in MMC but message server has
        // automatically changed message store to phone memory
        RFs& fs=iEikonEnv->FsSession();
        iMessageStoreExist = ETrue;
        TInt currentDrive = EDriveC;
        currentDrive = TInt(iSession->CurrentDriveL());

        TInt i = EDriveC;

        CRepository* repository = NULL;
        TRAPD( ret, repository = CRepository::NewL(KCRUidMuiuSettings) );

        if ( ret == KErrNone )
            {
            CleanupStack::PushL( repository );
            if ( repository->Get(KMuiuMemoryInUse,i) != KErrNone )
                {
                i = EDriveC;
                }
            if ( i != EDriveC && currentDrive == EDriveC )
                {
                // message server has changed the store automatically
                iMessageStoreExist = EFalse;
                }
            }

        if ( !iMessageStoreExist )
            {
            // message store was in MMC, but now there is no MMC
            iPhoneMemoryQuery = CAknQueryDialog::NewL();
            iPhoneMemoryQuery->ExecuteLD( R_MCE_MEMC_NOTE );
            iPhoneMemoryQuery = NULL; // ExecuteLD deletes the object
            }
        iMessageStoreExist = ETrue;

        if ( ret == KErrNone )
            {
            ret = repository->Set( KMuiuMemoryInUse, currentDrive );
            __ASSERT_DEBUG( !ret, User::Panic(KPanicText,KCRepositorySettingFailure) );
            CleanupStack::PopAndDestroy( repository );
            }

        }
    }

// ----------------------------------------------------
// CMceUi::SetMainViewActivatedFlag
//
// ----------------------------------------------------
void CMceUi::SetMainViewActivatedFlag( )
    {
    iMceUiFlags.SetMceFlag( EMceUiFlagsMainViewActivated );
    }

// ---------------------------------------------------------
// CMceUi::AlwaysOnlineL
// This is static function
// Returns ETrue if always online is supported
// ---------------------------------------------------------
TBool CMceUi::AlwaysOnlineL( )
    {
    TBool alwaysOnline = EFalse;
     if ( FeatureManager::FeatureSupported( KFeatureIdAlwaysOnLine )
        && FeatureManager::FeatureSupported( KFeatureIdAlwaysOnLineEmail ) )
        {
        TInt featureBitmask = 0;

        CRepository* repository = NULL;
        TRAPD( ret, repository = CRepository::NewL(KCRUidMuiuVariation) );
        CleanupStack::PushL( repository );

        if ( ret == KErrNone )
            {
            if ( repository->Get(KMuiuEmailConfigFlags,featureBitmask) != KErrNone )
                {
                alwaysOnline = EFalse;
                }
            else
                {
                alwaysOnline = featureBitmask & KEmailFeatureIdAlwaysOnline;
                }
            }

        CleanupStack::Pop( repository );
        delete repository;

        }

    return alwaysOnline;
    }

// ----------------------------------------------------
// CMceUi::RemoveFolderTabs
// ----------------------------------------------------
void CMceUi::RemoveFolderTabs()
    {
    MCELOGGER_ENTERFN("RemoveFolderTabs()");

    iNaviPane->Pop( iFolderIndicator );

    MCELOGGER_LEAVEFN("RemoveFolderTabs()");
    }

// ----------------------------------------------------
// CMceUi::ShowFolderTabsL
// ----------------------------------------------------
void CMceUi::ShowFolderTabsL( const TInt aDepth )
    {
    MCELOGGER_ENTERFN("ShowFolderTabsL()");

    CMceNaviPaneFolderIndicator* findicator =
        static_cast<CMceNaviPaneFolderIndicator*>( iFolderIndicator->DecoratedControl() );

    findicator->SetFolderDepth( aDepth );
    iNaviPane->PushL( *iFolderIndicator );

    MCELOGGER_LEAVEFN("ShowFolderTabsL()");
    }

// ----------------------------------------------------
// CMceUi::HandleMMSNotificationsDeleteL
//
// ----------------------------------------------------
void CMceUi::HandleMMSNotificationsDeleteL( TDes8& aParameter )
    {
    CBaseMtmUiData* uiData = NULL;
    uiData = GetMtmUiDataL( KUidMsgMMSNotification );

    TMsvId selId = iMMSNotifications->At(0);
    TMsvId mmsService;
    TMsvEntry mmsEntry;
    if ( uiData && iSession->GetEntry( selId, mmsService, mmsEntry ) == KErrNone  )
        {
        CBaseMtmUi& mtmUi=iMtmStore->GetMtmUiAndSetContextLC( mmsEntry );
        CMsvSingleOpWatcher* tempSingleOpWatcher = CMsvSingleOpWatcher::NewL( *this );
        CleanupStack::PushL( tempSingleOpWatcher );

        CMsvOperation* tempOp=mtmUi.InvokeAsyncFunctionL( KMtmUiFunctionDeleteMessage, *iMMSNotifications,
            tempSingleOpWatcher->iStatus, aParameter );

        if ( tempOp )
            {
            CleanupStack::PushL( tempOp );
            iOperations.AppendL( tempSingleOpWatcher );
            CleanupStack::Pop(2); // tempSingleOpWatcher, tempOp
            tempSingleOpWatcher->SetOperation( tempOp );
            }
        else
            {
            // User answered 'No' to the confirmation query and
            // NULL operation was returned
            CleanupStack::PopAndDestroy( tempSingleOpWatcher );
            if( iMMSNotifications )
                {
                delete iMMSNotifications;
                iMMSNotifications = NULL;
                }
            }
        CleanupStack::PopAndDestroy( ); // mtmUi
        }

    }

// ----------------------------------------------------
// CMceUi::CheckMMSNotificationsL
//
// ----------------------------------------------------
void CMceUi::CheckMMSNotificationsL( CMsvEntrySelection* aSelection )
    {
    TMsvId id;
    TMsvId service;
    TMsvEntry tEntry;
    CBaseMtmUiData* uiData = NULL;
    uiData = GetMtmUiDataL( KUidMsgMMSNotification );

    if ( iMMSNotifications )
        {
        delete iMMSNotifications;
        iMMSNotifications = NULL;
        }
    iMMSNotifications = new( ELeave ) CMsvEntrySelection();

    for ( TInt cc=aSelection->Count(); --cc>=0; )
        {
        id = aSelection->At(cc);
        if ( iSession->GetEntry( id, service, tEntry ) == KErrNone  )
            {
            if ( tEntry.iMtm == KUidMsgMMSNotification )
                {
                if ( uiData && !uiData->OperationSupportedL( KMtmUiFunctionDeleteMessage, tEntry ) )
                    {
                    iMMSNotifications->AppendL( tEntry.Id() );
                    aSelection->Delete( cc );
                    }
                else
                    {
                    aSelection->Delete( cc );
                    }
                }
            }
        }

    }

// ----------------------------------------------------
// CMceUi::OpenMailboxSettingsL
// ----------------------------------------------------
void CMceUi::OpenMailboxSettingsL()
    {
    MCELOGGER_ENTERFN("OpenMailboxSettingsL()");

    if ( MceViewActive( EMceMainViewActive ) )
        {
        return;
        }

    // Is settings opened from General Settings
    CMuiuLock* repositoryLock = CMuiuLock::NewL( KMuiuLockSettings );
    CleanupStack::PushL(repositoryLock);
    TInt err = repositoryLock->Reserve();
    // Check that settings are not opened from General Settings
    if ( !err )
        {
        iMceListView->OpenMailboxSettingsL();
        }
    else
        {
        // Show information note: General Settings has opened settings
        CAknNoteDialog* dlg = new (ELeave) CAknNoteDialog(
            CAknNoteDialog::ENoTone, CAknNoteDialog::ELongTimeout);
        dlg->ExecuteLD(R_MCE_NOTE_SETTINGS_OPEN_GS);
        }
    repositoryLock->Release();
    CleanupStack::PopAndDestroy( repositoryLock );

    MCELOGGER_LEAVEFN("OpenMailboxSettingsL");
    }

// ----------------------------------------------------
// CMceUi::SyncMlMarkAsReadL
// ----------------------------------------------------
void CMceUi::SyncMlMarkAsReadL( )
    {
    if ( MceViewActive( EMceMainViewActive ) )
        {
        return;
        }

    CMsvEntrySelection * selection = iMceListView->ListContainer()
        ->CurrentItemSelectionL();
    CleanupStack::PushL( selection );

    if (  selection->Count() <= 0 )
        {
        CleanupStack::PopAndDestroy( selection );
        return;
        }

    CBaseMtmUi& mtmUi = iMtmStore->GetMtmUiLC( KSenduiMtmSyncMLEmailUid );

    CMsvSingleOpWatcher* tempSingleOpWatcher=CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( tempSingleOpWatcher );
    TBuf8<1> blankParams;

    CMsvOperation* tempOp = mtmUi.InvokeAsyncFunctionL( KMtmUiFunctionMarkAsRead, *selection,
        tempSingleOpWatcher->iStatus, blankParams );

    CleanupStack::PushL( tempOp );
    iOperations.AppendL( tempSingleOpWatcher );
    CleanupStack::Pop( 2 ); // tempSingleOpWatcher, tempOp
    tempSingleOpWatcher->SetOperation( tempOp );
    CleanupStack::PopAndDestroy( 2 ); // mtmUi, selection
    }

// ----------------------------------------------------
// CMceUi::CheckMMSNotifDelFailedL
// ----------------------------------------------------
void CMceUi::CheckMMSNotifDelFailedL()
    {
    MCELOGGER_ENTERFN("CheckMMSNotifDelFailedL()");

    CBaseMtmUiData* data = GetMtmUiDataL( KUidMsgMMSNotification );
    if ( data )
        {
        TInt count = iMMSNotifications->Count();
        TInt failed = 0;
        TMsvId serviceId;
        for(TInt loop = 0; loop < count; loop++)
            {
            TMsvEntry entry;
            if ( iSession->GetEntry(iMMSNotifications->At(loop),serviceId,entry) == KErrNone )
            	{
            	if ( !( entry.iMtmData2 & KMmsNewOperationForbidden ) &&
                	!( entry.iMtmData2 & KMmsOperationOngoing ) &&
                	( entry.iMtmData2 & KMmsOperationFinished ) &&
                	( entry.iMtmData2 & KMmsOperationResult ) )
                	{
                	failed++;               
                	}
            	}
            } // end loop


        if ( failed > 0 )
            {
            CAknQueryDialog* note = CAknQueryDialog::NewL();
            HBufC* text = NULL;
            if ( failed == KMceOneMMSNotificationFailed )
                {
                text = StringLoader::LoadLC(
                    R_MMS_INFO_REMOTE_DEL_FAILED, iEikonEnv );
                note->SetPromptL( *text );
                note->ExecuteLD( R_MCE_MEMC_NOTE );
                CleanupStack::PopAndDestroy(  ); // text
                }
            else
                {
                CArrayFix<TInt>* indexArray =
                     new( ELeave ) CArrayFixFlat<TInt>( KMceArrayGranularity );
                CleanupStack::PushL( indexArray );
                indexArray->AppendL( failed );
                indexArray->AppendL( count );
                text = StringLoader::LoadLC(
                    R_MMS_INFO_REMOTE_DEL_FAILED_MANY, *indexArray, iEikonEnv );

                note->SetPromptL( *text );
                note->ExecuteLD( R_MCE_MEMC_NOTE );
                CleanupStack::PopAndDestroy( 2 ); // text, indexArray
                }
           }

       delete iMMSNotifications;
       iMMSNotifications = NULL;
       }

    MCELOGGER_LEAVEFN("CheckMMSNotifDelFailedL");
    }

// ----------------------------------------------------
// CMceUi::AddMultiselectionMTMFunctionsL
// ----------------------------------------------------
void CMceUi::AddMultiselectionMTMFunctionsL(CEikMenuPane& aMenuPane, TInt /*aMenuCommandId*/)
    {
    MCELOGGER_ENTERFN("AddMultiselectionMTMFunctionsL()");
    
    CEikMenuPaneItem::SData data;
    data.iCascadeId = 0;
    data.iFlags = 0;
    TInt cc;
    TInt sendCmd = EMceCmdFirstMTMFunction;
    const TInt count = iMTMFunctionsArray->Count();

    for ( cc = 0; cc < count; cc++ )
        {
        TMsgFunctionInfo& functionInfo = iMTMFunctionsArray->At(cc);
        data.iText = functionInfo.iCaption;
        data.iCommandId = sendCmd;

        if ( ( functionInfo.iFuncId == KMtmUiFunctionMMBox ) &&
            ( MceViewActive( EMceMessageViewActive ) && 
            iMceListView->ListContainer()->CurrentFolderId() 
            == KMsvGlobalInBoxIndexEntryId ) ) //MMS notification
            {
            TInt pos;
            if ( !aMenuPane.MenuItemExists ( sendCmd, pos ) )
                {
                aMenuPane.AddMenuItemL( data, EAknCmdOpen );
                }
            }

        sendCmd++;
        }
    MCELOGGER_LEAVEFN("AddMultiselectionMTMFunctionsL()");
    }

// ----------------------------------------------------
// CMceUi::CheckCspBitL
// ----------------------------------------------------
TBool CMceUi::CheckCspBitL( ) const
    {
    MCELOGGER_ENTERFN("CheckCspBitL()");
    RCustomerServiceProfileCache csp;
    TInt error = csp.Open();
    if ( error )
        {
        return ETrue;
        }

    RMobilePhone::TCspTeleservices params;
    TInt retVal = csp.CspTeleServices( params );
    csp.Close();

    if ( retVal == KErrNone )
        {
        if ( ( params&RMobilePhone::KCspSMCB ) != 0 )
            {
            MCELOGGER_LEAVEFN("CheckCspBitL()");
            // Cell Broadcast CSP bit is on
            return ETrue;
            }
        else
            {
            MCELOGGER_LEAVEFN("CheckCspBitL()");
            // Cell Broadcast CSP bit is off
            return EFalse;
            }
        }
    else
        {
        MCELOGGER_LEAVEFN("CheckCspBitL()");
        // Error: By default show the CBS service
        return ETrue;
        }
    }

// ---------------------------------------------------------
// CMceUi::CspBitsL
// Returns ETrue if csp bits is supported
// ---------------------------------------------------------
//
TBool CMceUi::CspBitsL( )
    {
    TBool csp = EFalse;
    TInt featureBitmask = 0;

    CRepository* repository = NULL;
    TRAPD( ret, repository = CRepository::NewL(KCRUidMuiuVariation) );
    CleanupStack::PushL( repository );

    if ( ret == KErrNone )
        {
        if ( repository->Get(KMuiuMceFeatures,featureBitmask) != KErrNone )
            {
            csp = EFalse;
            }
        else
            {
            csp = featureBitmask & KMceFeatureIdCSPSupport;
            }
        }

    CleanupStack::Pop( repository );
    delete repository;

    return csp;
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::DoUploadL
// ---------------------------------------------------------
void CMceUi::DoUploadL( TInt aIndex )
    {
    TInt i = aIndex;
    CSendingServiceInfo* info = iUploadServices[ i ];
    TMsgUploadParameters uploadParams;
    uploadParams.iDirect = ( info->ServiceProviderId() == KMmsDirectUpload );
    uploadParams.iRealAddress = info->ServiceAddress();
    uploadParams.iAlias = info->ServiceName();

    // Pack upload parameters
    TPckgBuf<TMsgUploadParameters> param( uploadParams );

    // Current selection
    CMsvEntrySelection* selection = iMceListView->ListContainer()->CurrentItemSelectionL();
    CleanupStack::PushL( selection );

    CAknInputBlock::NewLC();
    CMuiuOperationWait* wait =
        CMuiuOperationWait::NewLC( EActivePriorityWsEvents + 10 );

// Get a handle of the MMS MTM
    CBaseMtmUi& mtmUi = iMtmStore->GetMtmUiLC( KSenduiMtmMmsUid );

    CMsvOperation* oper = mtmUi.InvokeAsyncFunctionL(
        KMtmUiFunctionUpload,
        *selection,
        wait->iStatus,
        param );
    CleanupStack::PushL( oper );

    wait->Start();

    CleanupStack::PopAndDestroy( 5, selection ); // selection, CAknInputBlock, wait, oper
    }

// ---------------------------------------------------------
// CMceUi::ShowUploadQueryL
// ---------------------------------------------------------
void CMceUi::ShowUploadQueryL()
    {
    TInt count = iUploadServices.Count();

    TInt selectedIndex = 0;
    CAknListQueryDialog* dlg = new ( ELeave )
        CAknListQueryDialog( &selectedIndex );
    dlg->PrepareLC( R_MCE_UPLOAD_LIST_QUERY );

    // Populate list query array
    CDesCArrayFlat* array = new ( ELeave ) CDesCArrayFlat( 4 );
    CleanupStack::PushL( array );
    for ( TInt i = 0; i < count; i++ )
        {
        array->AppendL( iUploadServices[ i ]->ServiceMenuName() );
        }
    dlg->SetItemTextArray( array );
    CleanupStack::Pop( array );

    if ( dlg->RunLD() )
        {
        DoUploadL( selectedIndex );
        }

    }

// MMS.Content.Upload
// ---------------------------------------------------------
// CMceUi::NumUploadServices
// ---------------------------------------------------------
TInt CMceUi::NumUploadServices()
    {
    return iUploadServices.Count();
    }

// ---------------------------------------------------------
// CMceUi::HandleNotifL
// ---------------------------------------------------------
void CMceUi::HandleNotif()
    {
    if ( !iEmailNotifHandler )
        {
        // Handling of NCN reset
        TRAPD( err, iEmailNotifHandler = CMceEmailNotifHandler::NewL() );
        MCELOGGER_WRITE_FORMAT("ConstructL iEmailNotifHandler err %d", err);
        if ( err )
            {
            iEmailNotifHandler = NULL;
            }
        }
    if (iEmailNotifHandler)
        iEmailNotifHandler->HandleNotif();
    }

// ---------------------------------------------------------
// CMceUi::HandleNotifL
// ---------------------------------------------------------
void CMceUi::HandleNotif(const TMsvId& aMailbox)
    {
    if ( !iEmailNotifHandler )
        {
        // Handling of NCN reset
        TRAPD( err, iEmailNotifHandler = CMceEmailNotifHandler::NewL() );
        if ( err )
            {
            iEmailNotifHandler = NULL;
            }
        }
    if ( iEmailNotifHandler )
        {
        iEmailNotifHandler->HandleNotif(aMailbox);
        }
    }

// ---------------------------------------------------------
// CMceUi::CheckSIMAccessProfileL
// ---------------------------------------------------------
void CMceUi::CheckSIMAccessProfileL()
    {
    // P&S key of the SIM access profile
    RProperty property;
    TInt sapState( 0 );
    User::LeaveIfError( property.Attach( KPSUidBluetoothSapConnectionState, KBTSapConnectionState ) );
    CleanupClosePushL( property );
    
    property.Get( sapState );

    if ( (sapState==EBTSapConnecting) || (sapState==EBTSapConnected) )
        {
        // SIM access profile is connecting/connected, MCE is not started
        HBufC* text = StringLoader::LoadLC( R_QTN_OFFLINE_NOT_POSSIBLE_SAP, iEikonEnv );
        // ETrue waiting/non-waiting dialoog
        CAknInformationNote* note = new(ELeave) CAknInformationNote( ETrue );
        note->SetTimeout( CAknNoteDialog::ELongTimeout ); // ELongTimeout
        note->ExecuteLD( *text );
        CleanupStack::PopAndDestroy( text );
        // Exit the application
        CleanupStack::PopAndDestroy( &property );
        Exit();
        }
    else
        {
        CleanupStack::PopAndDestroy( &property );       
        }
    }

// ---------------------------------------------------------
// CMceUi::SetViewUpdateSuppressionFlag
// ---------------------------------------------------------
void CMceUi::SetViewUpdateSuppressionFlag( TBool aValue, CMsvOperation* aOp )
    {
    if ( aValue )
        {
        iMceUiFlags.SetMceFlag( EMceUiFlagsSupressViewUpdate );
        iBlockingOperation = aOp;
        }
    else
        {
        iMceUiFlags.ClearMceFlag( EMceUiFlagsSupressViewUpdate );
        iBlockingOperation = NULL;
        }

    }

// ---------------------------------------------------------
// CMceUi::ViewUpdateSuppressionFlag
// ---------------------------------------------------------
TBool CMceUi::ViewUpdateSuppressionFlag()
    {
    return iMceUiFlags.MceFlag( EMceUiFlagsSupressViewUpdate );
    }

// ---------------------------------------------------------
// CMceUi::ReleaseMtmUiData
// ---------------------------------------------------------
void CMceUi::ReleaseMtmUiData()
    {
    MCELOGGER_ENTERFN("ReleaseMtmUiData()");
    const TInt count = iUiRegistry->NumRegisteredMtmDlls();
    MCELOGGER_WRITE_FORMAT("Mtm cound %d", count);

    for ( TInt i = 0; i < count; i++ )
        {
        TUid uid = iUiRegistry->MtmTypeUid(i);
        if ( !MceUtils::IsMtmBuiltIn(uid) )
            {
            iMtmStore->ReleaseMtmUiData( uid );
            }
        }
    MCELOGGER_LEAVEFN("ReleaseMtmUiData()");
    }

// ---------------------------------------------------------
// CMceUi::InformationNoteCannotMoveCopyEMailL
// ---------------------------------------------------------
void CMceUi::InformationNoteCannotMoveCopyEMailL()
    {
    HBufC* errorText = StringLoader::LoadL( R_MCE_INFO_CANNOT_MOVE, iEikonEnv );
    CleanupStack::PushL( errorText );
    CAknInformationNote* note = new(ELeave)CAknInformationNote( ETrue );
    note->ExecuteLD(*errorText);
    CleanupStack::PopAndDestroy( errorText );
    }

// ---------------------------------------------------------
// CMceUi::RemoveEmailMessagesFromSelection
// ---------------------------------------------------------
TBool CMceUi::RemoveEmailMessagesFromSelection( CMsvEntrySelection *aSelection )
    {
    TBool mailMessage = EFalse;
    TInt count = aSelection->Count();
    TMsvId service;
    TMsvEntry tEntry;

    for ( TInt cc=count; --cc>=0; )
        {
        TMsvId id = aSelection->At(cc);
        if ( iSession->GetEntry( id, service, tEntry ) == KErrNone  )
            {
            if ( tEntry.iMtm==KSenduiMtmImap4Uid || tEntry.iMtm==KSenduiMtmPop3Uid ||
                 tEntry.iMtm==KSenduiMtmSmtpUid )
                {
                aSelection->Delete( cc );
                mailMessage = ETrue;
                }
            }
        }

    return mailMessage;
    }

// ---------------------------------------------------------
// CMceUi::MailboxCreationOn
// ---------------------------------------------------------
TBool CMceUi::MailboxCreationOn()
    {
    return iMailboxCreationOn;
    }

// ---------------------------------------------------------
// CMceUi::ZoomLevelChangedL
// ---------------------------------------------------------
TAknUiZoom CMceUi::ZoomLevelChangedL( TAknUiZoom aZoomLevel )
    {
    TAknUiZoom previous = LocalUiZoom();
    if ( aZoomLevel != previous &&
        !iMceUiFlags.MceFlag( EMceUiFlagsSettingsDialogOpen ) )
        {
        SetLocalUiZoom( aZoomLevel );
        ApplyLayoutChangeL( ETrue );
        }
    return previous;
    }

// ---------------------------------------------------------
// CMceUi::ReadZoomLevelL
// ---------------------------------------------------------
TAknUiZoom CMceUi::ReadZoomLevelL()
    {
    TInt zoomValue = EAknUiZoomAutomatic;
    CRepository* repository = NULL;
    TRAPD( ret, repository = CRepository::NewL(KCRUidMuiuSettings) );
    if ( ret == KErrNone )
        {
        CleanupStack::PushL( repository );
        if ( repository->Get(KMuiuZoomValue, zoomValue) != KErrNone )
            {
            zoomValue = EAknUiZoomAutomatic;
            }
        CleanupStack::PopAndDestroy( repository );
        }
    return (TAknUiZoom) zoomValue;
    }

// ---------------------------------------------------------
// CMceUi::WriteZoomLevelL
// ---------------------------------------------------------
void CMceUi::WriteZoomLevelL( TAknUiZoom aZoomLevel )
    {
    CRepository* repository = NULL;
    TRAPD( ret, repository = CRepository::NewL(KCRUidMuiuSettings) );
    if ( ret == KErrNone )
        {
        CleanupStack::PushL( repository );
        if ( repository->Set(KMuiuZoomValue, aZoomLevel) != KErrNone )
            {
            // hmm, just ignore?
            }
        CleanupStack::PopAndDestroy( repository );
        }
    }

// ---------------------------------------------------------
// CMceUi::HandleZoomLevelChangeL
// ---------------------------------------------------------
void CMceUi::HandleZoomLevelChangeL( TInt aCommand )
    {
    TAknUiZoom uiZoom;
    switch ( aCommand )
        {
        default:
        case EMceCmdZoomValueAutomatic:
           uiZoom = EAknUiZoomAutomatic;
           break;
        case EMceCmdZoomValueLarge:
           uiZoom = EAknUiZoomLarge;
           break;
        case EMceCmdZoomValueMedium:
           uiZoom = EAknUiZoomNormal;
           break;
        case EMceCmdZoomValueSmall:
           uiZoom = EAknUiZoomSmall;
           break;
        }

    TAknUiZoom previousZoom = ZoomLevelChangedL( uiZoom );
    if ( previousZoom != uiZoom )
        {
        WriteZoomLevelL( uiZoom );
        }
    }

// ---------------------------------------------------------
// CMceUi::HandleZoomSubMenu
// ---------------------------------------------------------
void CMceUi::HandleZoomSubMenu( CEikMenuPane* aMenuPane )
    {
    TAknUiZoom currentLevel = LocalUiZoom();
    TInt buttonId = EMceCmdZoomValueAutomatic;
    switch ( currentLevel )
        {
        default:
        case EAknUiZoomAutomatic:
            buttonId = EMceCmdZoomValueAutomatic;
            break;
        case EAknUiZoomLarge:
            buttonId = EMceCmdZoomValueLarge;
            break;
        case EAknUiZoomNormal:
            buttonId = EMceCmdZoomValueMedium;
            break;
        case EAknUiZoomSmall:
            buttonId = EMceCmdZoomValueSmall;
            break;
        }
    aMenuPane->SetItemButtonState( buttonId, EEikMenuItemSymbolOn );
    }

// ----------------------------------------------------
// CMceUi::ForwardL
//
// ----------------------------------------------------
void CMceUi::ForwardL(TInt /*aForwardCommand*/)
    {
    MCELOGGER_ENTERFN( "MceUi::ForwardL()----Start>>>>" );

    LeaveIfDiskSpaceUnderCriticalLevelL( iDiskSpaceForMoveOrCopy );

    if ( MceViewActive( EMceMainViewActive ) )
        {
        return;
        }

    CMsvEntrySelection * selection = iMceListView->ListContainer()->CurrentItemSelectionL();
    CleanupStack::PushL( selection );
    if ( selection->Count() <= 0)
        {
        CleanupStack::PopAndDestroy( selection );
        return;
        }

    TMsvEntry entry;
    TMsvId serviceId;
    User::LeaveIfError( iSession->GetEntry(selection->At(0),serviceId,entry) );
    CleanupStack::PopAndDestroy( selection );

    TMsvPartList parts = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartDescription | KMsvMessagePartAttachments;

    CBaseMtmUi& ui=iMtmStore->GetMtmUiAndSetContextLC( entry );
    ui.SetPreferences( ui.Preferences() | EMtmUiFlagEditorPreferEmbedded );
    CAknInputBlock::NewLC();
    CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( singleOpWatcher );

    CMsvOperation* oper = ui.ForwardL(
        KMsvDraftEntryIdValue, parts, singleOpWatcher->iStatus );
    iEditorOperation = singleOpWatcher;

    CleanupStack::PushL( oper );
    iOperations.AppendL( singleOpWatcher );
    CleanupStack::Pop( 2, singleOpWatcher ); // oper
    singleOpWatcher->SetOperation( oper );
    MCELOGGER_WRITE_FORMAT( "iOperations.AppendL(%d)", oper->Id() );
    CleanupStack::PopAndDestroy( 2 ); // CAknInputBlock, ui

    MCELOGGER_LEAVEFN( "MceUi::ForwardL()----End<<<<" );

    }

// ----------------------------------------------------
// CMceUi::HandleNewMsgToInboxL
// Remove New Flag from selected message(s) if there is any
// From 5.0
// ----------------------------------------------------
void CMceUi::HandleNewMsgToInboxL( TBool removeNewFlag, CMsvEntrySelection* aSelection )
    {
    TMsvEntry aEntry;

    if ( !removeNewFlag && ( !iIdArray ) )
        {
        return;
        }
    else if( removeNewFlag )
        {
        if ( iIdArray )
            {
            delete iIdArray;
            iIdArray = NULL;
            }
        iIdArray = new ( ELeave ) CMsvEntrySelection;
        for (TInt i = 0; i < aSelection->Count(); ++i )
            {
            TMsvId selectedId = aSelection->At(i);
            iEntry->SetEntryL( selectedId );
            aEntry = iEntry->Entry();
            if ( aEntry.Unread() )
                {
                iIdArray->AppendL( selectedId );
                aEntry.SetUnread( EFalse );
                iEntry->ChangeL( aEntry );
                }
            }
        if ( iIdArray->Count() )
            {
            iHandleNewMsgToInbox = ETrue;
            }
        }
    else
        {
        for ( TInt i = 0; i < iIdArray->Count(); ++i )
            {
            iEntry->SetEntryL( iIdArray->At(i) );
            aEntry = iEntry->Entry();
            aEntry.SetUnread( ETrue );
            iEntry->ChangeL( aEntry );
            }
        delete iIdArray;
        iIdArray = NULL;
        iHandleNewMsgToInbox = EFalse;
        }
    }


// ----------------------------------------------------
// TBool CMceUi::HasNewEmailL
// Check if there is new email the the folder
// public
// ----------------------------------------------------
TBool CMceUi::HasNewEmailL( TMsvId aFolderId )
    {
    CMsvEntry* entry = iSession->GetEntryL( aFolderId );
    CleanupStack::PushL( entry );

    TBool newEmail = EFalse;
    const TInt count = entry->Count();
    for ( TInt loop = 0; loop < count ; loop++ )
        {
        if ( (*entry)[loop].iType.iUid != KUidMsvFolderEntryValue )
            {
            if ( (*entry)[loop].New() )
                {
                newEmail = ETrue;
                break;
                }
            }
        }
    CleanupStack::PopAndDestroy( entry );
    return newEmail;
    }

// ----------------------------------------------------
// void CMceUi::SetDiskSpaceForMoveOrCopyL
// Reads critical and warning levels from cenrep
// ----------------------------------------------------
void CMceUi::SetDiskSpaceForMoveOrCopyL( )
    {
    // If can't open cenrep use default value
    iDiskSpaceForMoveOrCopy = KMceDiskSpaceForMoveOrCopy;
    TInt diskWarningThreshold = 0;
    TInt diskCriticalThreshold = 0;
    
    CRepository* repository = NULL;
    TRAPD( ret, repository = CRepository::NewL( KCRUidUiklaf ) );

    if ( ret == KErrNone )
        {
        CleanupStack::PushL( repository );
        if ( repository->Get( KUikOODDiskWarningThreshold, 
             diskWarningThreshold ) == KErrNone && 
             repository->Get( KUikOODDiskCriticalThreshold,
             diskCriticalThreshold ) == KErrNone )
            {
            iDiskSpaceForMoveOrCopy = diskWarningThreshold 
            - diskCriticalThreshold;        
            }
        CleanupStack::PopAndDestroy( repository );
        }

    }
// ----------------------------------------------------
// CMceUi::LaunchUniEditorL
// ----------------------------------------------------
//    
void CMceUi::LaunchUniEditorL()
    {
    //KUidUniMtm 0x102072D6
    CreateNewMessageL( KSenduiMtmUniMessageUidValue ); 
    }    
    
// ----------------------------------------------------
// CMceUi::GetToBeCreatedEmailType
// ----------------------------------------------------
//
 TInt CMceUi::GetToBeCreatedEmailType ()
    {
    // not sure if it's save
    TInt messageType = KErrNone;
    if ( MceViewActive( EMceMessageViewActive ) && iMceListView )
        {
        const TMsvEntry& entry = iMceListView->ListContainer()->FolderEntry();
        if ( entry.iMtm == KSenduiMtmImap4Uid
            || entry.iMtm == KSenduiMtmPop3Uid ) 
            {
            messageType = KSenduiMtmSmtpUidValue;
            }
        else
            {
             messageType = entry.iMtm.iUid;
            }
        }
    return messageType;
    }
    
// ----------------------------------------------------
// CMceUi::HideOrExit
// ----------------------------------------------------
void CMceUi::HideOrExit()
    {
    SetDontExitOnNextOperationComplete();
    iMceUiFlags.ClearMceFlag( EMceUiFlagsWantToExit );
    if (ExitHidesInBackground())
        {
        // Always use exit effect even if this is actually application switch
		// Note: Not allowed to call GfxTransEffect::EndFullScreen() as AVKON takes care of that when
		// EApplicationExit context is used!  
		// Set effect begin point
		GfxTransEffect::BeginFullScreen( AknTransEffect::EApplicationExit, TRect(), 
			AknTransEffect::EParameterType, AknTransEffect::GfxTransParam(TUid::Uid( KMceApplicationUidValue )) );       

        ResetAndHide();
        }
    else
        {
        CAknEnv::RunAppShutter();
        }
    }

// ----------------------------------------------------
// CMceUi::ResetAndHide
// ----------------------------------------------------
void CMceUi::ResetAndHide()
    {
    // Messaging was not exiting properly when "exit" is pressed from settings dialog.
    // iMceUiFlags.MceFlag( EMceUiFlagsSettingsDialogOpen ) will be true 
    // when we exit from any of the settings Dialog. 
    // Closing of Settings dialogs will be taken care by AVKON. 
    if( !(MceViewActive( EMceMainViewActive ) && IsForeground() ) ||  
	      iMceUiFlags.MceFlag( EMceUiFlagsSettingsDialogOpen ) )
        {
        SetCustomControl(1);    // Disable bring-to-foreground on view activation
        TRAP_IGNORE( CAknViewAppUi::CreateActivateViewEventL( \
            KMessagingCentreMainViewUid, \
            TUid::Uid(KMceHideInBackground), \
            KNullDesC8 ) ) ;
        }
    else
        {
        SetCustomControl(0); // Enable bring-to-foreground on view activation
        }
    HideInBackground();
    }

// ----------------------------------------------------
// CMceUi::OpenMtmMailboxViewL
// ----------------------------------------------------
void CMceUi::OpenMtmMailboxViewL( const TMsvEntry& aEntry )
	{
    CBaseMtmUi& mtmUi=iMtmStore->GetMtmUiAndSetContextLC( aEntry );

    CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( singleOpWatcher );

    CMsvOperation* op = NULL;
    TRAP_IGNORE(  op=mtmUi.OpenL(singleOpWatcher->iStatus )  );
    
    CleanupStack::PushL( op );
    iOperations.AppendL( singleOpWatcher );
    CleanupStack::Pop( op );
    CleanupStack::Pop( singleOpWatcher );
    
    singleOpWatcher->SetOperation( op );
    
    // Destroys mtmUi object insted of PopAndDestroy
    iMtmStore->ReleaseMtmUi( mtmUi.Type() );
	}

// ----------------------------------------------------
// CMceUi::FindMtmInVisibleListL
// ----------------------------------------------------
TBool CMceUi::FindMtmInVisibleListL( TUid aMtm, CArrayFix<TUid>& aListItemUids )
    {
    for ( TInt i = 0 ; i < aListItemUids.Count() ; i++)
        {
        if( aMtm == aListItemUids[i] )
            {
            return ETrue;
            }
        }
    return EFalse;
    }
// ----------------------------------------------------
// CMceUi::GetMsgDeletedStatus
// ----------------------------------------------------    
TInt CMceUi::GetMsgDeletedStatus()
    {
    return iMsgDeletedStatus;	
    }
// ----------------------------------------------------
// CMceUi::SetMsgDeletedStatus
// ----------------------------------------------------
void CMceUi::SetMsgDeletedStatus(TBool aStatus)
    {
    iMsgDeletedStatus = aStatus;	
    }

// ----------------------------------------------------
// CMceUi::GetFsIntegratedEmailAppMtmPluginId
// ----------------------------------------------------
TUid CMceUi::GetFsIntegratedEmailAppMtmPluginId()
    {
    TUid uidMsgValTypeEmailMtmVal = TUid::Uid(0);
     if ( (iEmailClientIntegration )&& (!iEmailFramework))
        {
        CRepository* repository = NULL;
        TInt mtmPluginID = 0;
        TRAPD( ret, repository = CRepository::NewL(
                                 KCRUidSelectableDefaultEmailSettings ) );
        
        if ( ret == KErrNone )
            {
            // Get Email application mtm plugin ID
            repository->Get( KIntegratedEmailAppMtmPluginId, mtmPluginID );
            }
        delete repository;
        uidMsgValTypeEmailMtmVal = TUid::Uid( mtmPluginID );       
        }
     return uidMsgValTypeEmailMtmVal;
    }
//CR:422-271
// ----------------------------------------------------
// CMceUi::PopulateMMSTemplates
// ----------------------------------------------------
TInt CMceUi::PopulateMMSTemplates()
    {

    TInt error = KErrNone;
    TInt numErrors = 0;
    TInt errorIncrement = 0;
    
    //first try if the directory of mms dump exists or not
    _LIT(KTopDir,"C:\\Private\\1000484b\\");
      
    _LIT( KWild, "mmsvar" );

    TFindFile finder( iFs );
    error = finder.FindByDir(KWild,KTopDir);

    if ( error !=KErrNone )
        {
        return -1;
        }
    // Reset inactivity timer to keep viewServer from crashing  
    User::ResetInactivityTime();
    
    TFileName CurrentPath;
    CurrentPath.Copy( KMmsMessageDumpDirectory );
    
    TRAP( error, errorIncrement = DecodeAllMMSFromDirectoryL( CurrentPath,KMsvGlobalInBoxIndexEntryId) );
    numErrors += errorIncrement;
    if ( error != KErrNone )
        {
      //"left with error "
        	numErrors++;
        	error = KErrNone;
        }
    if ( errorIncrement > 0 )
        {
        //("- returned errors")
        }
	User::ResetInactivityTime();
	return numErrors;
}
// ----------------------------------------------------
// CMceUi::DecodeAllMMSFromDirectoryL
// ----------------------------------------------------
TInt CMceUi::DecodeAllMMSFromDirectoryL( TFileName& aFilePath, TMsvId aBoxid /*= KMsvGlobalInBoxIndexEntryId*/ )
    {
    TInt numErrors = 0;
    TInt error = KErrNone;
    _LIT( KWild, "*" );

    CDir* fileList = NULL;
    TInt i = 0;  // general counter
    TFileName Filename;
    TFileName CurrentPath = aFilePath;
    if ( CurrentPath[CurrentPath.Length()-1] != '\\' )
    CurrentPath.Append( '\\' );
    iFs.SetSessionPath( CurrentPath );

    TFindFile finder( iFs );
    error = finder.FindWildByPath( KWild, NULL, fileList );
    CleanupStack::PushL( fileList );
    TInt fileCounter = 0;

    if ( error == KErrNone )
        {
        fileCounter = fileList->Count();
        }
    TEntry entry;

    if ( error == KErrNone )
        {
        for ( i = 0; i < fileCounter; i++ )
            {
            // Reset inactivity timer to keep viewServer from crashing  
            User::ResetInactivityTime();
            entry = (*fileList)[i]; // name is entry.iName
            Filename.Copy( entry.iName );
            if (!entry.IsDir())
                {
                TRAP( error, DecodeMMSFromFileL( Filename, aBoxid ));
                if ( error != KErrNone )
                    {
                        //There are errors in decoding the files
                    }
                iFs.Delete(entry.iName);
                }

             }
            
        }

    CleanupStack::PopAndDestroy(); // fileList
    fileList = NULL;

    return numErrors;
    }
// ----------------------------------------------------
// CMceUi::DecodeFromFileL
// ----------------------------------------------------
TInt CMceUi::DecodeMMSFromFileL( TFileName& aFilePath, TMsvId aBoxId /* KMsvGlobalInBoxIndexEntryId*/, TUint32 aFlags /*= 0*/ )
    {
    TInt numErrors = 0;
    TInt error = KErrNone;
    TMsvId id;
        
    FillBufferFromFileL( aFilePath, iFs, iEncodeBuffer );
        
    CMmsCodecClient* codecClient = CMmsCodecClient::NewL( *iSession );
    CleanupStack::PushL( codecClient );
    
    TBool unread = EFalse;
    
    TRAP( error, codecClient->InitializeChunkedAddingL( aBoxId, id, aFlags, unread ) ); 
    if ( error != KErrNone )
        {
        numErrors++;
        }
    
    if ( error == KErrNone )
        {
        error = FeedDataInChunks( codecClient );
        }
        
    if ( error != KErrNone )
        {
        numErrors++;
        }
    
    // Not needed can be cleaned up
    CMsvEntry* cEntry = iSession->GetEntryL( id );
    TMsvEntry entry = cEntry->Entry();

    entry.SetReadOnly(ETrue);
    entry.iServiceId = KMsvLocalServiceIndexEntryId;
    entry.iMtmData1 = KMmsMessageMRetrieveConf | KMmsMessageMobileTerminated;
    entry.SetUnread(EFalse);
    
    
    cEntry->ChangeL(entry); 
    CleanupStack::PopAndDestroy( codecClient );
       
    return numErrors;
    }
// ----------------------------------------------------
// CMceUi::FeedDataInChunks
// ----------------------------------------------------
TInt CMceUi::FeedDataInChunks( CMmsCodecClient* aCodecClient )
    {
    
    TInt error = KErrNone;
    TInt chunkSize = KMmsCodecClientChunkSize;
    TInt size = iEncodeBuffer->Size();
    
    // Number of full chunks
    TInt chunkNumber = size / chunkSize;
    TInt remainder = size % chunkSize;
    
    TInt pos = 0;    
    TPtrC8 messagePtr( iEncodeBuffer->Ptr( 0 ) );
    
    TBool lastChunk = EFalse;
    TInt i = 0;
    for ( i = 0; i < chunkNumber && error == KErrNone; i++ )
        {
        pos = i * chunkSize;
        TPtrC8 ptr( iEncodeBuffer->Ptr( 0 ).Mid( pos, chunkSize ) );
        error = aCodecClient->NextDataPart( ptr, lastChunk );
        }
        
    // last chunk
    if ( remainder > 0 && error == KErrNone )
        {
        TPtrC8 ptr2( iEncodeBuffer->Ptr( size - remainder ) );
        if ( ptr2.Length() > 0 )
            {
            error = aCodecClient->NextDataPart( ptr2, lastChunk );
            }
        }
        
    lastChunk = ETrue;
    TPtrC8 ptr3;
    if ( error == KErrNone )
        {
        aCodecClient->NextDataPart( ptr3, lastChunk );
        }
    return error;    
    
    }
// ----------------------------------------------------
// CMceUi::FillBufferFromFileL
// ----------------------------------------------------
void CMceUi::FillBufferFromFileL(
    const TDesC& aFilePath,
    RFs& aFs,
    CBufFlat* aEncodeBuffer )
    {
    TInt error = KErrNone;
    TEntry orgEntry;
    error = aFs.Entry( aFilePath, orgEntry );
    TInt size = orgEntry.iSize;
    RFile inFile;
    if ( aEncodeBuffer == NULL )
        {
        aEncodeBuffer = CBufFlat::NewL( size );
        }
    else
        {
        aEncodeBuffer->ResizeL( 0 );
        aEncodeBuffer->ResizeL( size );
        }

    error = inFile.Open( aFs, aFilePath, EFileShareReadersOnly );
    
    TPtr8 ptr = aEncodeBuffer->Ptr( 0 );
    if ( error == KErrNone )
        {
        error = inFile.Read( ptr, size );
        inFile.Close();
        }
    else
        {
        //"*** - can't read file"
        }
    User::LeaveIfError( error );    
    
    }
//CR:422-271
// end of file

// end of file
