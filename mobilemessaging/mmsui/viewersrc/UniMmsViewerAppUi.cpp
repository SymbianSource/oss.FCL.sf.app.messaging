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
*       Provides CMmsViewerAppUi class methods.
*
*/



// INCLUDE FILES

#include <gmxmlconstants.h>

#include <e32keys.h>                // EStdKey*

#include <eikmenup.h>               // CEikMenuPane
#include <eikmenub.h>               // CEikMenuBar
#include <eikrted.h>
#include <MsgBodyControl.h>         // CMsgBodyControl
#include <MsgEditorView.h>          // CMsgEditorView
#include <MsgEditor.hrh>            // Base editor commands & enums
#include <MsgEditorCommon.h>
#include <MsgRecipientItem.h>
#include <msgeditor.mbg>            // bitmap identifiers
#include <MmsViewer.rsg>            // resouce identifiers
#include <mmsui.mbg>
#include <data_caging_path_literals.hrh> 

#include <apgwgnam.h>                // CApaWindowGroupName

#include <msvuids.h>
#include <MsgAddressControl.h>
#include <mtclbase.h>
#include <MsgFolderSelectionDialog.h>   //Folder Selection dialog
#include <MuiuMsvUiServiceUtilities.h>
#include <muiumsvuiserviceutilitiesinternal.h>

#include <MuiuOperationWait.h>          // CMuiuOperationWait
#include <MuiuMsgEmbeddedEditorWatchingOperation.h>
#include <akninputblock.h>              // CAknInputBlock
#include <muiumsguploadparam.h>

#include <avkon.hrh>
#include <avkon.rsg>                // R_AVKON_SOFTKEYS_OPTIONS_BACK, R_AVKON_NAVI_PANE_VOLUME_INDICATOR
#include <aknnavide.h>              // CAknNavigationDecorator
#include <akntitle.h>               // CAknTitlePane
#include <Aknvolumecontrol.h>       // CAknVolumeControl
#include <AknQueryDialog.h>
#include <AknWaitDialog.h>
#include <aknnotewrappers.h>
#include <aknlists.h>               // CAknSinglePopupMenuStyleListBox
#include <aknPopup.h>               //
#include <akntabgrp.h>              // CAknTabGroup
#include <aknclearer.h>             // CAknLocalScreenClearer
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <AknStatuspaneUtils.h>

#include <AiwGenericParam.h>           // CAiwGenericParamList

#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include <cntdef.h>                         // KNullContactId

#include <centralrepository.h>
#include <cenrepnotifyhandler.h>
#include <messaginginternalcrkeys.h>
#include <CommonUiInternalCRKeys.h>
#include <MusicPlayerInternalCRKeys.h>
#include <ProfileEngineSDKCRKeys.h>   // silence mode status check
#include <settingsinternalcrkeys.h>    // KCRUidRichCallSettings
#include <messagingvariant.hrh>
 
#include <Profile.hrh>              // silence mode status check

#include <DocumentHandler.h>        // CDocumentHandler
#include <commonphoneparser.h>      // CommonPhoneParser

#include <FindItemmenu.rsg>         // Find item stuff
#include <finditemmenu.h>           //
#include <finditemdialog.h>         //
#include <finditem.hrh>             //
#include <ItemFinder.h>          //

#include <StringLoader.h>           // StringLoader
#include <ErrorUI.h>                // Default error notes

#include <charconv.h>               // Character sets

#include <basched.h>                // KErrExtended, KLeaveExit

#include <eikbtgpc.h>
#include <baclipb.h>
#include <txtetext.h>
#include <txtrich.h>

#include <msvstd.h>                     // Message entries
#include <msvapi.h>                     // MsvOperation
#include <msvstore.h>                   // CMsvStore
#include <MtmExtendedCapabilities.hrh>  // function id of MessageInfo

#include <CSendingServiceInfo.h>
#include <sendui.h>                 // SendUI (for Reply via SMS)
#include <SenduiMtmUids.h>          // MTM Uids
#include <CMessageData.h>

#include <mdaaudiosampleplayer.h>
#include <AudioPreference.h>
#include <NpdApi.h>                 // CNotePadApi

// DRM
#include <DRMCommon.h>

// SMIL Player
#include <SmilPlayerDialog.h>

// Features
#include <featmgr.h>	
#include <bldvariant.hrh>

// Help
#include <csxhelp/mms.hlp.hrh>
#include <hlplch.h>                 // For HlpLauncher 

#include "MmsMtmUi.h"               // Mms Mtm Ui

#include "MsgAttachmentUtils.h"     // enumerations for objects view

#include <msgtextutils.h>
#include <MsgMedia.hrh>
#include <MsgMediaInfo.h>
#include <MsgVideoInfo.h>
#include <MsgAttachmentInfo.h>

#include <MmsConformance.h>
#include <fileprotectionresolver.h>
#include <msgmediacontrol.h>
#include <msgimagecontrol.h>
#include <msgaudiocontrol.h>
#include <msgvideocontrol.h>
#ifdef RD_SVGT_IN_MESSAGING
#include <msgsvgcontrol.h>
#endif
#include <unidatamodel.h>           // CUniDataModel
#include <unismilmodel.h>           // CUniSmilModel
#include <unidatautils.h>           // enumerations
#include <uniobject.h>
#include <unimimeinfo.h>

#include <unislideloader.h>

#include <uniobjectsviewdialog.h>   // Common objects view implementation
#include <uniobjectsmodel.h>        // Model part of the objects view
#include <uniobjectlist.h>
#include <unismillist.h>
#include <uniobjectsinfo.h>

#include <mmsclient.h>              // CMmsClientMtm, mmsconst.h
#include <mmsgenutils.h>
#include <mmsmsventry.h>
#include <mmsversion.h>

#include "MmsViewer.hrh"            // application specific commands & enums
#include "UniMmsViewerDocument.h"      // CMmsViewerDocument
#include "UniMmsViewerAppUi.h"         // CMmsViewerAppUi
#include "MmsViewerHeader.h"
#include "MmsViewerLaunchOperation.h"
#include "MmsViewerChangeSlideOperation.h"
#include "MmsViewerSendReadReportOperation.h"

#include "MmsMtmConst.h"            // File logger
#include <msgvoipextension.h>

#ifdef RD_SCALABLE_UI_V2
#include <aknnavide.h>              // pen support on navi pane
#include <AknNaviObserver.h>        // pen support of volume control of navi pane
#endif
#include <akntoolbar.h>             
#include <MsgEditorAppUiExtension.h>// for iMsgEditorAppUiExtension
#include <MsgEditorSettingCacheUpdatePlugin.h>
#include    <e32property.h>
#include  <ctsydomainpskeys.h>
#include <aknstyluspopupmenu.h>
// CONSTANTS

// following is minimum value for player
const TInt KVolumeControlMinValuePlayer = 0;      
// following are minimum values for avkon volume indicator component
const TInt KVolumeControlMinValue = 0;      //from "AKNVOLUMECONTROL.CPP"
const TInt KVolumeControlMaxValue = 10;     //from "AKNVOLUMECONTROL.CPP"
const TInt KVolumeDefault = 8;
const TInt KVolumeLevels = 10;
const TInt KMmsSizeWaitDialogDisabled = 204800; 

#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
const TUint KNaviGranularity = 2;
const TInt KIconDefaultColor = 0;
#endif
_LIT( KMmsUiMbmFile, "mmsui.mbm" );

const TInt KReplyToSenderSize = 2048;
const TInt KReplyToAllSize = 2048;
const TMsvId KNewMessageFolder = KMsvDraftEntryIdValue;
const TInt KInvalidResourceId = -1;

// Fast swap window group
#define KFastSwapAppUid TUid::Uid( 0x10207218 )
#define KAknNotifyServerAppUid TUid::Uid( 0x10281ef2 )
#define KScreenSaverAppUid TUid::Uid( 0x100056CF )

_LIT( KMmsViewerSpace, " " );

const TUid KNotepadUID3 = { KTextHandler };   // Notepad viever

// Exiting in launch. Let this time to show error notes
//const TInt  KDelayedExitShort = 100000;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMmsViewerAppUi::CMmsViewerAppUi( ) :
        iPendingNoteResource( KInvalidResourceId ),
        iViewerFlags( EInitialFlags ),
        iViewerState( ENormal )
    {
    LOGTIMESTAMP("CMmsViewerAppUi::CMmsViewerAppUi ");
    }

// ---------------------------------------------------------
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMmsViewerAppUi::ConstructL( )
    {
    CMsgEditorAppUi::ConstructL( );
    
    // Disable task swapper from options menu during launch
    MenuBar()->SetMenuType( CEikMenuBar::EMenuOptionsNoTaskSwapper );
    
    LOGTEXT(_L8("CMmsViewerAppUi::ConstructL starting") );
	FeatureManager::InitializeLibL( );
	if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
		{
		iSupportedFeatures |= EMmsFeatureHelp;
		}
    if ( FeatureManager::FeatureSupported( KFeatureIdDrmFull ) )
        {
        iSupportedFeatures |= EMmsFeatureDrmFull;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdSideVolumeKeys ) )
        {
        iSupportedFeatures |= EMmsFeatureSVK;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdSenduiMmsUpload ) )
        {
        iSupportedFeatures |= EMmsFeatureUpload;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdAudioMessaging ) )
        {
        iSupportedFeatures |= EMmsFeatureAudioMessaging;
        }
    FeatureManager::UnInitializeLib( );

#ifdef RD_SCALABLE_UI_V2
    // Creation of CMsgEditorView must be after toolbar is set visible otherwise layout things can go wrong. 
    // Toolbar itself seems to set itself invisible temporary at some startup time. Toolbar is set visible here, too.   
    if ( AknLayoutUtils::PenEnabled() )
        {
        iToolbar = CurrentFixedToolbar();  
        if ( iToolbar )
            {
            iToolbar->SetToolbarObserver( this );
            iToolbar->SetToolbarVisibility( ETrue, EFalse );
            }
        }
#endif
        
    iView = CMsgEditorView::NewL( *this,
        CMsgEditorView::EMsgReadOnly |
        CMsgEditorView::EMsgDoNotUseDefaultBodyControl );

    iMtm = &(Document( )->Mtm( ));
    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( StatusPane( )->ControlL(  TUid::Uid( EEikStatusPaneUidNavi ) ) );
    iTitlePane = static_cast<CAknTitlePane*>
        ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    iButtonGroup = Cba( );
    iSvkEvents = CMmsSvkEvents::NewL( *this );

    if ( !iButtonGroup || !iNaviPane || !iMtm )
        {
        User::Leave( KErrBadHandle );
        }

    TInt highlight = 0;
    TInt drmPlayback = 0;    
    // Volume level storage
    iMmsRepository = CRepository::NewL( KCRUidMmsui );

    // Automatic highlight setting storage and change notifications
    iCUiRepository = CRepository::NewL( KCRUidCommonUi );
    iCUiRepository->Get( KCuiAutomaticHighlight, highlight  );
    iNotifyHandler = CCenRepNotifyHandler::NewL( *this, 
                                                 *iCUiRepository, 
                                                 CCenRepNotifyHandler::EIntKey, 
                                                 KCuiAutomaticHighlight );
    iNotifyHandler->StartListeningL( );

    CRepository* repository = CRepository::NewLC( KCRUidMusicPlayerFeatures );
    repository->Get( KRequireDRMInPlayback, drmPlayback  );    
    
    if ( drmPlayback )
        {
        TBuf<1> tmp; // Magic: 1 char to get length of actual value
        TInt realLen = 0;
        TInt err = repository->Get( KPlaybackRestrictedMimeTypes,
                                    tmp,
                                    realLen );
                                    
        if ( err == KErrOverflow )
            {
            // Prepare list of blocked MIME types
            iProhibitMimeTypeBuffer = HBufC::NewL( realLen + KMmsViewerSpace().Length() );
            TPtr bufferPtr = iProhibitMimeTypeBuffer->Des();
            
            User::LeaveIfError( repository->Get( KPlaybackRestrictedMimeTypes, bufferPtr ) );
            bufferPtr.Append( KMmsViewerSpace );
            }
        else
            {
            User::LeaveIfError( err );
            }
        }
    
    CleanupStack::PopAndDestroy( repository );
    repository = NULL;

    TInt features(0);
    repository = CRepository::NewL( KCRUidMuiuVariation );
    
    repository->Get( KMuiuMmsFeatures, features );
    if ( features & KMmsFeatureIdDeliveryStatusDialog ) 
        {
        iSupportedFeatures |= EMmsFeatureDeliveryStatus;
        }
        
    repository->Get( KMuiuUniEditorFeatures, features );
    if ( features & KUniEditorFeatureIdRestrictedReplyMms )
        {
        iSupportedFeatures |= EMmsFeatureRestrictedReply;
        }
    delete repository;
        
    repository = CRepository::NewL( KCRUidUniEditor );
    repository->Get( KUniEditorMaxSlideCount, iMaxForwardSlideCount );    
    delete repository;
        
    // Set up highlighting
    if ( highlight == 0 )
        {
        iViewerFlags &= ~EAutohighLightEnabled;
        }
    else
        {
        iViewerFlags |= EAutohighLightEnabled;
        }

    if ( drmPlayback )
        {
        iViewerFlags |= EProhibitNonDrmMusic;
        }

    if ( Document( )->CreationMode( ) == EMmsCreationModeWarning )
        {
        iViewerFlags |= EShowGuidedConf;
        }

    iMmsConformance = CMmsConformance::NewL( );

  	// Set path of bitmap file
  	TParse fileParse;
  	fileParse.Set( KMmsUiMbmFile, &KDC_APP_BITMAP_DIR, NULL );
    
    iAppIcon = AknsUtils::CreateGulIconL( 
        AknsUtils::SkinInstance(), 
        KAknsIIDQgnPropMceMmsTitle,
        fileParse.FullName(),
        EMbmMmsuiQgn_prop_mce_mms_title,
        EMbmMmsuiQgn_prop_mce_mms_title_mask );

    // Get important window group ids
    GetNonStoppingWgIds( );

    // Priority must be lower than the priority of iWrappedWaitDialog
    // Constructed here to ensure exit on low memory situations.
    iIdle = CPeriodic::NewL( EPriorityNormal - 1);

	if( !iEikonEnv->StartedAsServerApp( ) )
		{
	    Document( )->PrepareToLaunchL( this );
		}
    LOGTEXT2(_L16("CMmsViewerAppUi::ConstructL read. iSupportedFeatures 0x%x"), iSupportedFeatures );
    iLongTapDetector = CAknLongTapDetector::NewL( this );
    iLongTapDetector->SetTimeDelayBeforeAnimation( KUniLongTapStartDelay );
    iLongTapDetector->SetLongTapDelay( KUniLongTapTimeDelay );
    iTapConsumed = EFalse;
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CMmsViewerAppUi::~CMmsViewerAppUi( )
    {
    LOGTEXT(_L8("CMmsViewerAppUi::~CMmsViewerAppUi starting") );
    iViewerFlags |= EViewerExiting;
    iViewerState = EReseted;
    EndActiveWait();
    // Input blocker is also deleted
    DeactivateInputBlocker();
    if ( iNotifyHandler )
        {        
        iNotifyHandler->StopListening( );
        }
    delete iNotifyHandler;
    delete iCUiRepository;
    delete iMmsRepository;

    //iMtm not owned - just a reference pointer
    //iSmilModel not owned - just a reference pointer
    //iNaviPane not owned - just a reference pointer
    //iButtonGroup not owned - just a reference pointer

#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
    delete iNaviDecorator;
#endif
    delete iVolumeDecorator;
    delete iWaitDialog;
    delete iFindItemMenu;
    delete iDocHandler;

    delete iHeader;
    delete iSlideLoader;
    delete iLaunchOperation;
    delete iChangeSlideOperation;
    delete iScreenClearer;
    iUploadServices.ResetAndDestroy( );
    delete iMmsVoIPExtension;
    delete iSvkEvents;
    delete iMmsConformance;
    delete iTitleIcon;
    
    delete iProhibitMimeTypeBuffer;
    delete iSendReadReportOperation;
    delete iIdle;
    delete iAppIcon;
    if(iLongTapDetector)
    {
    delete iLongTapDetector;
    }
    if(iEmbeddedObjectStylusPopup)
    {
    delete iEmbeddedObjectStylusPopup;
    iEmbeddedObjectStylusPopup = NULL;
    }

    LOGTEXT(_L8("CMmsViewerAppUi::~CMmsViewerAppUi end") );
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::Document
// ---------------------------------------------------------
//
CMmsViewerDocument* CMmsViewerAppUi::Document( ) const
    {
    return static_cast<CMmsViewerDocument*>( CMsgEditorAppUi::Document( ) );
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::MsgSaveExitL
// framework function
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoMsgSaveExitL( )
    {
    MediaStop( );
    iExitMode = MApaEmbeddedDocObserver::TExitMode(CMsgEmbeddedEditorWatchingOperation::EMsgExternalExit);
    Exit( );
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::LaunchViewL
// ---------------------------------------------------------
//

void CMmsViewerAppUi::LaunchViewL( )
    {
    __ASSERT_DEBUG( Document( )->Entry( ).Id( ) != KMsvNullIndexEntryId, Panic( EMmsViewerNoMessage ) );
    TBool exitViewer( EFalse );
    if ( iViewerFlags & EViewerExiting )
        {
        exitViewer = ETrue;
        }
    else
        {
        TRAPD( error, DoLaunchViewL() );
        
        if ( error != KErrNone )
            {
            iEikonEnv->HandleError( error );
            exitViewer = ETrue;
            }
        }
    
    if ( exitViewer )
        {
        // Start the delayed exit here if exit command was given before application was
        // ready to handle it or error happened when performing lauch view. 
        // Delayed exit is used as LauchViewL cannot make proper exit if Exit() is called here.
        DoDelayedExit( 0 );
        }
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::DoLaunchViewL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoLaunchViewL()
    {
    // Exiting here leaves incomplete mms viewer view visible
    // Let the launch happen until ViewerOperationEvent()
    TRAPD( error, iMtm->LoadMessageL( ) );
    LOGTEXT2(_L16("CMmsViewerAppUi::LaunchViewL error %d"), error );
    if ( error )
        {
        if ( error != KErrNoMemory )
            {
            ShowErrorNoteL( R_MMSVIEWER_ERROR_MSG_CORRUPT );
            }
        User::Leave( error );
        }

    iViewerState = EBusy;
    iOpeningState = EOpeningMessage;
    
    // Disabling Wait Dialog for messages less than 200kB 
    if ( iMtm->MessageSize() > KMmsSizeWaitDialogDisabled )
        {
        LaunchWaitDialogL( 0 );
        }

    // iWaitDialog was deleted in case of error. Is this needed ?
    iLaunchOperation = new ( ELeave ) CMmsViewerLaunchOperation(
                                        this,
                                        *this,
                                        *this,
                                        *Document( ),
                                        *iView,
                                        iCoeEnv->FsSession( ) );
    ActivateInputBlockerL( iLaunchOperation );
    iLaunchOperation ? iLaunchOperation->Launch( ) : User::Leave( KErrNoMemory );
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::ChangeSlideL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::ChangeSlideL( TInt aSlideNumber, TBool aScreenClearer )
    {
    if ( aSlideNumber < 0 || aSlideNumber >= iSmilModel->SlideCount( ) )
        {
        User::Leave( KErrArgument );
        }
    MediaStop( );
    
    iViewerState = EBusy;
    iOpeningState = EOpeningSlide;
    iViewerFlags |= EVideoHasAudio;
    iVideo = NULL;
    iAudio = NULL;
    iImage = NULL;
#ifdef RD_SVGT_IN_MESSAGING
    iSvgt = NULL;
#endif    
    iPendingNoteResource = KInvalidResourceId;
    iPendingNotePriority = 0;
    iPendingNoteObject = NULL;
    iPendingNoteError = KErrNone;
    
    // use case: 
    // precondition: text on two slide and from field empty on the emulator
    // crash when: 
    // - goto slide 2 and take selection menu on text
    // - goto slide 1 and take selection menu on from field (which is empty) => crash
    // iFindItem contains 'deleted' objects
    delete iFindItemMenu;
    iFindItemMenu = NULL;

    UpdateMskL();


    if ( !iChangeSlideOperation )
        {
        iChangeSlideOperation = new ( ELeave )CMmsViewerChangeSlideOperation (
            *this,
            *( Document( ) ),
            *iHeader,
            *iSlideLoader,
            *iView,
            iCoeEnv->FsSession( ) );
        }
        
    iChangeSlideOperation ? iChangeSlideOperation->ChangeSlide( aSlideNumber ) : User::Leave( KErrNoMemory );
    
    if ( aScreenClearer )
        {
        iScreenClearer = CAknLocalScreenClearer::NewLC( EFalse );
        CleanupStack::Pop( iScreenClearer );
        iScreenClearer->SetExtent( iScreenClearer->Position(), ClientRect().Size() );
        }
    
    if ( aScreenClearer )
        {
        LaunchWaitDialogL( aSlideNumber );
        }
    ActivateInputBlockerL( iChangeSlideOperation );
    
    // TODO: Get rid of active wait!
    BeginActiveWait( iChangeSlideOperation );
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::LaunchWaitDialogL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::LaunchWaitDialogL( TInt aParam )
    {
    if ( iWaitDialog )
        {
        delete iWaitDialog;
        iWaitDialog = NULL;
        }
 
    if ( iOpeningState == EOpeningMessage )
        {
        iWaitDialog = new ( ELeave ) CAknWaitDialog(
                ( reinterpret_cast<CEikDialog**>( &iWaitDialog ) ),
                ETrue );
        iWaitDialog->ExecuteLD( R_MMSVIEWER_WAIT_LOADING );
        }
    else if ( iOpeningState == EOpeningSlide )
        {
        iWaitDialog = new ( ELeave ) CAknWaitDialog(
                ( reinterpret_cast<CEikDialog**>( &iWaitDialog ) ),
                EFalse );
        CArrayFixFlat<TInt>* array = new ( ELeave ) CArrayFixFlat<TInt>(2);
        CleanupStack::PushL( array );
        array->AppendL( aParam + 1 );
        array->AppendL( iSmilModel->SlideCount( ) );
        HBufC* string = StringLoader::LoadLC(
            R_MMSVIEWER_OPENING_PAGE,
            *array,
            iCoeEnv );
        iWaitDialog->SetTextL( *string );
        iWaitDialog->ExecuteLD( R_MMSVIEWER_WAIT_OPENING_PAGE );
        CleanupStack::PopAndDestroy( 2, array ); // + string
        }
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::SetPendingNote
// ---------------------------------------------------------
//
void CMmsViewerAppUi::SetPendingNote(   TInt                aResourceId, 
                                        CUniObject*         aObject,
                                        TInt                aError )
    {    
    TInt priority = 0;
    switch ( aResourceId )
        {
        case R_MMSVIEWER_DRM_ERROR:
            {
                // do not show note during launch or slide change except for image
            if (   !aObject
                || !aObject->MediaInfo( )
                ||  (   (   iOpeningState == EOpeningMessage
                        ||  iOpeningState == EOpeningSlide )
                    &&  aObject->MediaType( ) != EMsgMediaImage ) )
                {
                return;
                }                
            else if (   aObject->MediaType( ) == EMsgMediaImage
#ifdef RD_SVGT_IN_MESSAGING
                    ||  aObject->MediaType( ) == EMsgMediaSvg
#endif
                    )
                {
                if ( aObject->DrmInfo( ) )
                    {
                    TBool consumed = EFalse;
                    aObject->DrmInfo( )->EvaluateRights( consumed );
                    if ( consumed )
                        {
                        return;
                        }
                    }
                else
                    {
                    // can't do anything without drm info
                    return;
                    }
                }
            priority = 5;
            break;
            }
        case R_MMSVIEWER_DRM_PREVIEW_RIGHTS:
            priority = 4;  
            break;          
        case R_MMSVIEWER_DRM_OBJECT_REMOVED:
            if (!priority)
                {
                priority = 3;
                }
            break;
        case R_MMSVIEWER_CANNOT_DISPLAY_OBJECTS:
            {
                    
                // during launch only 
            if ( iOpeningState != EOpeningMessage )
                {
                return;
                }
            if (!priority)
                {
                priority = 2;
                }
            break;
            }
        case R_QTN_MMS_CANNOT_OPEN_CORRUPTED:
            {   
                // during play only
            if ( iOpeningState != EOpeningNone )
                {
                return;
                }
            priority = 1;
            break;
            }
        default:
            break;
        }
    if ( priority > iPendingNotePriority )
        {
        iPendingNotePriority = priority;
        iPendingNoteResource = aResourceId;
        iPendingNoteObject = aObject;
        iPendingNoteError = aError;
        }
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::ShowPendingNoteL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::ShowPendingNoteL( )
    {
        // show note only once and after every control has been initialised
        // in order to show the highest priority note
    if ( !AreControlsReadyAndWaitNoteDismissedL() )
        {
        return;
        }
          
    iOpeningState = EOpeningNone;

    // It may occur that this code point is entered twice when control state changes take place
    // Temp variable is second action to avoid it. Resetting iPending* variables earlier in ReloadControlL
    // and ChangeSlide is first one
    TInt pendingNoteResource = iPendingNoteResource;
    iPendingNoteResource = KInvalidResourceId;
    switch ( pendingNoteResource )
        {
        case R_MMSVIEWER_DRM_ERROR:
            {
            if (    iPendingNoteObject 
                &&  iPendingNoteObject->DrmInfo( ) )
                {
                TRAP_IGNORE( iPendingNoteObject->DrmInfo( )->HandleDrmErrorL( iPendingNoteError ) );
                }
            break;
            }
        case R_MMSVIEWER_DRM_PREVIEW_RIGHTS:
        case R_MMSVIEWER_DRM_OBJECT_REMOVED:
        case R_MMSVIEWER_CANNOT_DISPLAY_OBJECTS:
        case R_QTN_MMS_CANNOT_OPEN_CORRUPTED:
            {
            ShowInformationNoteL( pendingNoteResource );
            }
            break;
        case KInvalidResourceId:
        default:
            break;
        }    
    iPendingNotePriority = 0;
    iPendingNoteObject = NULL;
    iPendingNoteError = KErrNone;
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::InitNavipaneL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::InitNavipaneL( )
    {
#ifdef RD_MSG_NAVIPANE_IMPROVEMENT
    if ( !iNaviDecorator )
        {
        TMsgEditorMsgPriority priority = EMsgEditorMsgPriorityNormal;
    
        if ( iMtm->MessagePriority() == EMmsPriorityHigh )
            {
            priority = EMsgEditorMsgPriorityHigh;
            }
        else if ( iMtm->MessagePriority() == EMmsPriorityLow )
            {
            priority = EMsgEditorMsgPriorityLow;
            }
        
        CreateViewerNaviPaneL( Document()->Entry().iDate, priority, ETrue );
        }
        
    if ( MediaIsRunning( iAudio ) ||
         MediaIsRunning( iVideo ) )
        {
        iNaviPane->PushL( *iVolumeDecorator );  
        }
    else
        {
        iNaviPane->PushL( *iNaviDecorator );  
        }
#else
    TInt indexInFolder = 0;
    TInt msgsInFolder = 0;
    MessageIndexInFolderL( indexInFolder, msgsInFolder );

    if (    indexInFolder < 0
        ||  msgsInFolder == 0 )
        {
        // this may occur when viewer is used in cover ui. Viewer is started as 
        // stand alone without current folder and '1/0' would be visible. 
        iNaviPane->PushDefaultL( );
        return;
        }

    CArrayFixFlat<TInt>* array = new ( ELeave ) CArrayFixFlat<TInt>(KNaviGranularity);
    CleanupStack::PushL( array );
    array->AppendL( indexInFolder + 1 );
    array->AppendL( msgsInFolder );

    HBufC* buf = StringLoader::LoadLC( R_MMSVIEWER_ORDINAL_NUMBER, *array, iCoeEnv );
    //Pushed into CleanupStack

    //Creates iNaviDecorator
    CreateTabGroupL( *buf );
    CleanupStack::PopAndDestroy( 2, array ); // + buf

    iNaviDecorator->MakeScrollButtonVisible( ETrue );

    //NaviButtons are dimmed by default
    if ( IsNextMessageAvailableL( ETrue ) )
        {
        iNaviDecorator->SetScrollButtonDimmed( CAknNavigationDecorator::ERightButton, EFalse );
        }
    if ( IsNextMessageAvailableL( EFalse ) )
        {
        iNaviDecorator->SetScrollButtonDimmed( CAknNavigationDecorator::ELeftButton, EFalse );
        }

    if (    !MediaIsRunning( iAudio ) 
        &&  !MediaIsRunning( iVideo ) )
        {
        iNaviPane->PushL( *iNaviDecorator );  
        }
#endif
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::UpdatePanesL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::UpdatePanesL( TBool aAudioPlaying, TBool aVideoPlaying )
    {
    if ( aAudioPlaying || aVideoPlaying )
        {
        // Silent mode is handled in DoChangeVolumeL
        TInt newVolume = DoChangeVolumeL( 0, ETrue, aAudioPlaying ); 
        if ( newVolume >= KVolumeControlMinValue )
            {
            // Show the volume
            ShowVolumeL( newVolume, aAudioPlaying );
            }
        if ( aAudioPlaying )
            {
            iButtonGroup->SetCommandSetL( R_MMSVIEWER_SOFTKEYS_EMPTY_STOP );
            }
        else if ( aVideoPlaying )
            {
            iButtonGroup->SetCommandSetL( R_MMSVIEWER_SOFTKEYS_EMPTY_STOPVIDEO );
            }
        else
            {
            iButtonGroup->SetCommandSetL( R_MMSVIEWER_SOFTKEYS_OPTIONS_BACK_EMPTY );
            }

        if (    iNaviDecorator
            &&  iNaviPane->Top( ) == iNaviDecorator )
            {
            // Silent mode is ON or video has not audio
            // Hide navi pane during playback
            iNaviPane->PushDefaultL( );
            } 
        }
    else 
        {
        iButtonGroup->SetCommandSetL( R_MMSVIEWER_SOFTKEYS_OPTIONS_BACK_EMPTY );
        if ( iNaviDecorator )
            {
            iNaviPane->PushL( *iNaviDecorator );
            }
        else
            {
            iNaviPane->PushDefaultL( );
            }
        }
    // forces to update MSK, which was overridden here
    iMskId = 0;
    iButtonGroup->DrawNow( );
#ifdef RD_SCALABLE_UI_V2
    SetToolBarItemVisibilities();
#endif
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::CreateTabGroupL
// ---------------------------------------------------------
//
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
void CMmsViewerAppUi::CreateTabGroupL( const TDesC& aText )
    {
    if ( !iNaviDecorator )
        {
        // Create TabGroup
        iNaviDecorator = iNaviPane->CreateTabGroupL();
        }
    
#ifdef RD_SCALABLE_UI_V2
    if ( AknLayoutUtils::PenEnabled() )
        {
	    iNaviDecorator->SetNaviDecoratorObserver( this );
        }
#endif
    
    CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>( iNaviDecorator->DecoratedControl() );
    // Create message priority bitmap
    if ( iMtm->MessagePriority() == EMmsPriorityHigh || 
         iMtm->MessagePriority() == EMmsPriorityLow )
        {
        TRect naviPane;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::ENaviPane, naviPane );
        
        TAknLayoutRect naviTabsPane;
        naviTabsPane.LayoutRect( naviPane,
                                 AknLayoutScalable_Avkon::navi_navi_tabs_pane().LayoutLine() );
        
        TAknLayoutRect navi2TabsPane;
        navi2TabsPane.LayoutRect( naviTabsPane.Rect(),
                                  AknLayoutScalable_Avkon::navi_tabs_2_pane().LayoutLine() );

        TAknLayoutRect tabs2ActivePane;
        tabs2ActivePane.LayoutRect( navi2TabsPane.Rect(),
                                    AknLayoutScalable_Avkon::tabs_2_active_pane( 0 ).LayoutLine() );

        TAknLayoutRect tabs2ActivePaneIcon;
        tabs2ActivePaneIcon.LayoutRect( tabs2ActivePane.Rect(),
                                        AknLayoutScalable_Avkon::tabs_2_active_pane_g1().LayoutLine() );

        TSize indicatorIconSize( tabs2ActivePaneIcon.Rect().Size() );
    
        // Set path of bitmap file
        TParse fileParse;
        fileParse.Set( KMmsUiMbmFile, &KDC_APP_BITMAP_DIR, NULL );
        
        CFbsBitmap* bitmapPriority = NULL;
        CFbsBitmap* maskPriority = NULL;
    
        if ( iMtm->MessagePriority() == EMmsPriorityHigh )
            {
            // Create high priority bitmap
            AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
                                         KAknsIIDQgnIndiMcePriorityHigh,
                                         KAknsIIDQsnIconColors,
                                         EAknsCIQsnIconColorsCG7,
                                         bitmapPriority,
                                         maskPriority,
                                         fileParse.FullName(),
                                         EMbmMmsuiQgn_indi_mce_priority_high,
                                         EMbmMmsuiQgn_indi_mce_priority_high_mask,
                                         AKN_LAF_COLOR( KIconDefaultColor ),
                                         indicatorIconSize,
                                         EAspectRatioPreserved );    
            }
        else
            {
            // Create low priority bitmap
            AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
                                         KAknsIIDQgnIndiMcePriorityLow,
                                         KAknsIIDQsnIconColors,
                                         EAknsCIQsnIconColorsCG7,
                                         bitmapPriority,
                                         maskPriority,
                                         fileParse.FullName(),
                                         EMbmMmsuiQgn_indi_mce_priority_low,
                                         EMbmMmsuiQgn_indi_mce_priority_low_mask,
                                         AKN_LAF_COLOR( KIconDefaultColor ),
                                         indicatorIconSize,                                         
                                         EAspectRatioPreserved ); 
            }
            
        if ( !bitmapPriority )            
            {
            User::Leave( KErrNotFound );
            }
            
        CleanupStack::PushL( bitmapPriority );
        CleanupStack::PushL( maskPriority );
        
        if ( tabGroup->TabCount() == 0 )
            {
            tabGroup->AddTabL( 0, aText, bitmapPriority, maskPriority );
            }
        else
            {
            tabGroup->ReplaceTabL( 0, aText, bitmapPriority, maskPriority );
            }
            
        CleanupStack::Pop( maskPriority ); // ownership tranferred to tabgroup
        CleanupStack::Pop( bitmapPriority ); // ownership tranferred to tabgroup
        }
    else
        {
        // Set <message-index>/<message count>
        if ( tabGroup->TabCount() == 0 )
            {
            tabGroup->AddTabL( 0, aText ); 
            }
        else
            {
            tabGroup->ReplaceTabL( 0, aText );
            }
        }

    tabGroup->SetActiveTabById( 0 );
    tabGroup->SetTabFixedWidthL( KTabWidthWithOneTab );
    }
#endif // !RD_MSG_NAVIPANE_IMPROVEMENT

// ---------------------------------------------------------
// CMmsViewerAppUi::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane )
    {
    TInt amountOfItems( aMenuPane->NumberOfItemsInPane( ) );
    if ( !IsLaunched( )  )
        {
        if ( amountOfItems )
            {
            aMenuPane->DeleteBetweenMenuItems( 0, amountOfItems - 1 );
            }
        return;
        }

    if ( !iFindItemMenu )
        {
        iFindItemMenu = CFindItemMenu::NewL( EFindItemMenuPlaceHolder );
        iFindItemMenu->SetCallSubMenuVisibility ( EFalse ); // Click-To-Call
        if ( iHeader->SenderType( ) == EMuiuAddressTypeEmail )
            {
            // Set sender type to find menu
            iFindItemMenu->SetSenderDescriptorType( CItemFinder::EEmailAddress );
            }
        }

    if ( iSupportedFeatures & EMmsFeatureUpload &&
        !( iViewerFlags & EUploadInitialized ) )
        {
        iViewerFlags |= EUploadInitialized;
        CSendUi* sendui = CSendUi::NewLC( );
        sendui->AvailableServicesL( iUploadServices, KMmsDirectUpload );
        sendui->AvailableServicesL( iUploadServices, KMmsIndirectUpload );
        CleanupStack::PopAndDestroy( sendui );
        }


    switch ( aMenuId )
        {
        case R_MMSVIEWER_SELECTIONMENU:
            {
            // No find item for recipient fields in Sent folder
            if ( !IsOwnMessage( ) )
                {    
                if( (FocusedControlId( ) == EMsgComponentIdFrom) && ( ( iMtm->Sender( ) ).Length() ) )
                    {
                    iFindItemMenu->SetSenderHighlightStatus( ETrue );
                    iFindItemMenu->SetSenderDisplayText( TMmsGenUtils::PureAddress( iMtm->Sender( ) ) );
                    }
	            else
	     	        {
                    iFindItemMenu->SetSenderHighlightStatus( EFalse );
		            }
                }
			iFindItemMenu->AddItemFindMenuL( 
                ( FocusedControlId( ) == EMsgComponentIdBody ) ? iView->ItemFinder( ) : 0,
				aMenuPane, 
                EFindItemContextMenuPlaceHolder,
                TMmsGenUtils::PureAddress( iMtm->Sender( ) ),
                iHeader->Alias( ) && !iHeader->IsRemoteAlias( ) ? ETrue : EFalse, //"Is sender known"
				ETrue );

           
            break;
            }
        case R_MMSVIEWER_OPTIONSMENU:
            {
            
            if ( iViewerState == EReseted )
                {
                //Exit is the last item... leave it there.
                aMenuPane->DeleteBetweenMenuItems( 0, amountOfItems - 2 );
                return;
                }
                    
            TInt focusedControl = FocusedControlId( );
                      
            TBool senderHighlighted = EFalse;
            if ( focusedControl == EMsgComponentIdFrom )
                {
                CMsgAddressControl* address = static_cast<CMsgAddressControl*>(
                    iView->ControlById( EMsgComponentIdFrom ) );
                
                if ( address && address->Editor().SelectionLength() )
                    {
                    senderHighlighted = ETrue;
                    }
                }
            
            if ( !IsOwnMessage( ) )
                {    
                if( ( focusedControl == EMsgComponentIdFrom ) && ( ( iMtm->Sender( ) ).Length() ) 
                      && senderHighlighted )
                    {
                    iFindItemMenu->SetSenderHighlightStatus( ETrue );
                    iFindItemMenu->SetSenderDisplayText( TMmsGenUtils::PureAddress( iMtm->Sender( ) ) );                
                    }
	            else
	     	        {
                    iFindItemMenu->SetSenderHighlightStatus( EFalse );
		            }
                }
            
            iFindItemMenu->AddItemFindMenuL( 
                ( iView->ItemFinder() && 
                  iView->ItemFinder()->CurrentSelection().Length() ) 
                ? iView->ItemFinder() : 0,
                aMenuPane, 
                EFindItemMenuPlaceHolder,
                senderHighlighted ? 
                TMmsGenUtils::PureAddress( iMtm->Sender( ) ) : KNullDesC(),
                iHeader->Alias( ) && !iHeader->IsRemoteAlias( ) ? ETrue : EFalse, //"Is sender known"
                EFalse );
                
            const TMsvEntry& context = Document( )->Entry( );
            // local variation for MMS Delivery Status dialog feature
            if (! (     iSupportedFeatures & EMmsFeatureDeliveryStatus  
                    && ( ( context.iMtmData2 & KMmsDeliveryStatusMask) != KMmsDeliveryStatusNotRequested ) ) )
                {
                aMenuPane->DeleteMenuItem(EMmsViewerDeliveryStatus);
                }

            if ( iSmilModel->SlideCount( ) > iMaxForwardSlideCount )
                {
                aMenuPane->SetItemDimmed( EMmsViewerForward, ETrue );
                }
                                
   
                
            if (    ! ( iSupportedFeatures & EMmsFeatureUpload )
                ||  !iUploadServices.Count( ) )
                {
                aMenuPane->SetItemDimmed( EMmsViewerUpload, ETrue );
                }            
                            
            // Set help menu-item visibility according featureManager
            if ( !( iSupportedFeatures & EMmsFeatureHelp ) )
                {
                aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
                }
            if ( !Document( )->SmilList( )->Count( ) )
                {
                aMenuPane->SetItemDimmed( EMmsViewerShowMessage, ETrue );

                }
            else 
                {
                if ( Document( )->SmilType( ) != EMmsSmil )
                    {
                    aMenuPane->SetItemDimmed( EMmsViewerShowMessage, ETrue );
                    }

				}

            if ( !ShowReplyMenu( ) )
                {
                aMenuPane->SetItemDimmed( EMmsViewerReplyMenu, ETrue );
                }
                
            if ( Document( )->ObjectList( )->Count( ) == 0 &&
                Document( )->AttachmentList( )->Count( ) == 0 )
                {
                aMenuPane->SetItemDimmed( EMmsViewerObjects, ETrue );
                }    
            break;
            }
        case R_MMSVIEWER_REPLY_SUBMENU:
            {
            if (   IsOwnMessage( )
                || !HasSender( ) )
                {
                aMenuPane->SetItemDimmed( EMmsViewerReplyToSender, ETrue );
                }
            if ( !ShowReplyToAll( ) )
                {
                aMenuPane->SetItemDimmed( EMmsViewerReplyToAll, ETrue );
                }
            if (   !( iSupportedFeatures & EMmsFeatureAudioMessaging )
                || IsOwnMessage( )
                || !HasSender( )
                ||  iSupportedFeatures & EMmsFeatureRestrictedReply )
                {
                aMenuPane->SetItemDimmed( EMmsViewerReplyViaAudio, ETrue );
                }
                
            if ( IsOwnMessage( ) || 
                 iHeader->SenderType( ) != EMuiuAddressTypeEmail ||
                iSupportedFeatures & EMmsFeatureRestrictedReply ||
                 MsvUiServiceUtilitiesInternal::DefaultServiceForMTML(
                    Document( )->Session( ),
                    KSenduiMtmSmtpUid,
                    ETrue )
                    == KMsvUnknownServiceIndexEntryId )
                {
                // "Reply via e-mail" feature
                aMenuPane->SetItemDimmed( EMmsViewerReplyViaMail, ETrue );
                }
            break;
            }
        case R_MMSVIEWER_ZOOM_SUBMENU:
            {
            TInt zoomLevel = KErrGeneral;
            iMsgEditorAppUiExtension->
                iSettingCachePlugin.GetValue( KAknLocalZoomLayoutSwitch, zoomLevel );
            switch ( zoomLevel )
                {
                case EAknUiZoomAutomatic:
                    aMenuPane->SetItemButtonState( EMsgDispSizeAutomatic,
                                     EEikMenuItemSymbolOn );
                    break;
                case EAknUiZoomNormal:
                    aMenuPane->SetItemButtonState( EMsgDispSizeNormal,
                                     EEikMenuItemSymbolOn );
                    break;
                case EAknUiZoomSmall:
                    aMenuPane->SetItemButtonState( EMsgDispSizeSmall,
                                     EEikMenuItemSymbolOn );
                    break;
                case EAknUiZoomLarge:
                    aMenuPane->SetItemButtonState( EMsgDispSizeLarge,
                                     EEikMenuItemSymbolOn );
                    break;
                default:
                    break;
                }
            }
            break;
        default: // we are in FindItem submenu
            {
            // this does nothing if we're not, so this is safe
			iFindItemMenu->UpdateItemFinderMenuL( aMenuId, aMenuPane );
            break;
            }
        }
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::UploadL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::UploadL( )
    {
    TInt count = iUploadServices.Count( );
    
    TInt selectedIndex = 0;
    CAknListQueryDialog* dlg = new ( ELeave )
        CAknListQueryDialog( &selectedIndex );
    dlg->PrepareLC( R_MMSVIEWER_UPLOAD_LIST_QUERY );

    // Populate list query array
    CDesCArrayFlat* array = new ( ELeave ) CDesCArrayFlat( 4 );
    CleanupStack::PushL( array );
    for ( TInt i = 0; i < count; i++ )
        {
        array->AppendL( iUploadServices[ i ]->ServiceMenuName( ) );
        }
    dlg->SetItemTextArray( array );
    CleanupStack::Pop( array ); 

    if ( dlg->RunLD( ) )
	    {
        CSendingServiceInfo* info = iUploadServices[ selectedIndex ];
        TMsgUploadParameters uploadParams;
        uploadParams.iDirect = ( info->ServiceProviderId( ) == KMmsDirectUpload );
        uploadParams.iRealAddress = info->ServiceAddress( );
        uploadParams.iAlias = info->ServiceName( );

        // Pack upload parameters
        TPckgBuf<TMsgUploadParameters> param( uploadParams );

        MediaStop( );
        ResetViewL( );

        //These are not used! They just have to be there.
        CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
        CleanupStack::PushL( selection );

        CAknInputBlock::NewLC( );
        CMuiuOperationWait* wait =
            CMuiuOperationWait::NewLC( EActivePriorityWsEvents + 10 );

        CMsvOperation* oper = Document( )->MtmUiL( ).InvokeAsyncFunctionL(
            KMtmUiFunctionUpload,
            *selection,
            wait->iStatus,
            param );

        CleanupStack::PushL( oper );

        wait->Start( );

        CleanupStack::PopAndDestroy( 4, selection );  // absorber, wait, oper, selection
        Exit( EAknSoftkeyClose );
	    }
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::ProcessCommandL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::ProcessCommandL( TInt aCommand )
    {    
    if (    aCommand == EAknSoftkeyOptions
        &&  iViewerState == EBusy)
        {
        LOGTEXT(_L8("CMmsViewerAppUi::ProcessCommandL returns immediately..."));
        return;
        }            
    CAknAppUi::ProcessCommandL( aCommand );
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::HandleCommandL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::HandleCommandL( TInt aCommand )
    {
#ifdef USE_LOGGER
    LOGTEXT2(_L16("CMmsViewerAppUi::HandleCommandL in. Command %d"), aCommand );
    TRAPD(error, DoHandleCommandL( aCommand ) );
    LOGTEXT2(_L16("CMmsViewerAppUi::HandleCommandL %d"), error );
    User::LeaveIfError( error );
#else
    DoHandleCommandL( aCommand );
#endif
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::DoHandleCommandL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoHandleCommandL( TInt aCommand )
    {
    if ( !IsLaunched() )
        {
        if ( aCommand == EEikCmdExit )
            {
            LOGTEXT(_L8("CMmsViewerAppUi::DoHandleCommandL Setting iViewerFlags |= EViewerExiting"));
            iViewerFlags |= EViewerExiting;
            }
        return;
        }

    if ( iFindItemMenu && iFindItemMenu->CommandIsValidL( aCommand ) )
		{
		iFindItemMenu->HandleItemFinderCommandL( aCommand );
		return;
		}		
    //MediaStop( );
    switch ( aCommand )
        {
        case EMmsViewerShowPresentation:
        case EMmsViewerShowMessage:
            DoShowPresentationL( );
            break;
        case EMmsViewerForward:
            if ( !ShowOutOfDiskNoteIfBelowCriticalLevelL(
                 Document( )->MessageSize( ) ) )
                {
                DoForwardL( );
                }
            break;
        case EMmsViewerDelete:
            DoDeleteAndExitL( );
            break;
        case EMmsViewerViewImage:
            MediaPlayL( iImage );
            break;
        case EMmsViewerPlayAudio:
            MediaPlayL( iAudio );
            break;
        case EMmsViewerPlayVideo:
            MediaPlayL( iVideo );
            break;
#ifdef RD_SVGT_IN_MESSAGING
        case EMmsViewerPlaySvg:
            MediaPlayL( iSvgt );            
            break;
#endif            
        case EMmsViewerObjects:
            DoObjectsL( );
            break;
        case EMmsViewerMessageInfo:
            DoMessageInfoL( );
            break;
        case EMmsViewerDeliveryStatus:
            OpenDeliveryPopupL( );    
            break;
        case EMmsViewerMove:
            DoMoveMessageL( );
            break;
        case EMmsViewerReplyToSender:
            if ( !ShowOutOfDiskNoteIfBelowCriticalLevelL(
                 KReplyToSenderSize ) )
                {
                DoReplyL( ETrue );
                }
            break;
        case EMmsViewerReplyToAll:
            if ( !ShowOutOfDiskNoteIfBelowCriticalLevelL(
                 KReplyToAllSize ) )
                {
                DoReplyL( EFalse );
                }
            break;
        case EMmsViewerReplyViaAudio:
        case EMmsViewerReplyViaMail:
            if ( !ShowOutOfDiskNoteIfBelowCriticalLevelL(
                 KReplyToAllSize ) )
                {
                DoReplyViaL( aCommand );
                }
            break;

        case EMmsViewerSoftStopVideo:
            MediaStop(iVideo);
            break;
#ifdef RD_SVGT_IN_MESSAGING            
        case EMmsViewerSoftStopSvg:
            MediaStop( iSvgt );
            break;
#endif            
                        
        case EMmsViewerSaveObject:
            SaveObjectL( );
            break;   
                     
        case EMmsViewerUpload:
            {
            UploadL( );
            break;
            }

        case EMmsViewerSoftStopAudio:
            MediaStop(iAudio);
            break;
        case EMsgDispSizeAutomatic:
        case EMsgDispSizeLarge:
        case EMsgDispSizeNormal:
        case EMsgDispSizeSmall:
            HandleLocalZoomChangeL( (TMsgCommonCommands)aCommand );
            break;
        case EAknCmdHelp:
            LaunchHelpL( );
            break;

        case EAknSoftkeyBack:
            MediaStop( );
            Exit( EAknSoftkeyClose );                
            break;
        case EEikCmdExit:
            {
            DoMsgSaveExitL( );
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::HandleKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CMmsViewerAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    if ( !IsLaunched( ) )
        {
        return EKeyWasNotConsumed;
        }
    if ( aType != EEventKey )
        {
        return EKeyWasNotConsumed;
        }
                
    if (    iViewerState == EBusy
        ||  iViewerState == EReseted )
        {
        return EKeyWasConsumed;
        }
    switch ( aKeyEvent.iCode )
        {
        case EKeyLeftArrow:
            {
            if (    iVolumeDecorator
                &&  iNaviPane->Top( ) == iVolumeDecorator )
                {
                if( !( iSupportedFeatures & EMmsFeatureSVK ) )
                    {
                    ChangeVolumeL( -1, ETrue );                
                    }
                else if ( IsNextMessageAvailableL( EFalse ) )
                    {
                    // playing audio/video is stopped by destructor
                    NextMessageL( EFalse );
                    }	
                }
            else if (   iNaviDecorator
                    &&  iNaviPane->Top( ) == iNaviDecorator )
                {
                if ( IsNextMessageAvailableL( EFalse ) )
                    {
                    NextMessageL( EFalse );
                    }
                }
            }
            break;
        case EKeyRightArrow:
            {
            if (    iVolumeDecorator
                &&  iNaviPane->Top( ) == iVolumeDecorator )
                {
                if( !( iSupportedFeatures & EMmsFeatureSVK ) )
                    {
                    ChangeVolumeL( +1, ETrue );
                    }
                else if ( IsNextMessageAvailableL( ETrue ) )
                    {
                    // playing audio/video is stopped by destructor
                    NextMessageL( ETrue );
                    }
                }
            else if (   iNaviDecorator
                    &&  iNaviPane->Top( ) == iNaviDecorator )
                {
                if ( IsNextMessageAvailableL( ETrue ) )
                    {
                    NextMessageL( ETrue );
                    }
                }
            }
            break;
        case EKeyDevice3:       //Selection key
        case EKeyEnter:         //Enter Key
            {                    
            CMsgExpandableControl* ctrl = NULL;
            TBool checkHighlight = EFalse;
            
            if( iView->FocusedControl() )
                {
                ctrl = static_cast<CMsgExpandableControl*>( iView->FocusedControl() );
                if ( ctrl->ControlType() == EMsgAddressControl
                        && !ctrl->Editor().SelectionLength()  ) 
                    {
                    checkHighlight = ETrue;
                    }
                else if ( ctrl->ControlType() == EMsgBodyControl )
                    {       
                    CItemFinder* itemFinder = iView->ItemFinder();
                    if ( FocusedControlId() == EMsgComponentIdBody && itemFinder
                            && !itemFinder->CurrentSelection().Length() )
                        {                                                              
                        checkHighlight = ETrue;
                        }
                    }
                }
            if ( ctrl && checkHighlight )
                {
                // Check if highlight needs to be restored to editor,
                // address/body editor offerkeyevent will handle it
                if ( ctrl->Editor().OfferKeyEventL( aKeyEvent, aType ) 
                        == EKeyWasConsumed )
                    {
                    // Highlight was restored, just return, no cs menu needed
                    return EKeyWasConsumed;
                    }
                }

            TInt focusedControl = FocusedControlId( );
            CMsgMediaControl* control = MediaControlById(focusedControl);
            if (control)
                {
                if ( MediaCanStop( control ) )
                    {
                    MediaStop(control);
                    }
                else
                    {
                    MediaOpenL(control);
                    }
                }
            else if( focusedControl == EMsgComponentIdAttachment ) 
                {
            	DoObjectsL( );
                }
            else if (    Document( )->SmilType( ) != EMmsSmil
                    &&  focusedControl == EMsgComponentIdImage )
                {
                DoShowPresentationL( );
                }
            else if ( iViewerState == ENormal )
                {
                MenuBar( )->SetMenuTitleResourceId( R_MMSVIEWER_SELECTMENU );
                MenuBar()->SetMenuType(CEikMenuBar::EMenuContext);
                TRAP_IGNORE( MenuBar( )->TryDisplayMenuBarL( ) );
                MenuBar( )->SetMenuTitleResourceId( R_MMSVIEWER_MENU );
                MenuBar()->SetMenuType(CEikMenuBar::EMenuOptions);              
                }
            break;
            }
        case EKeyYes:   //Send key
            DoHandleSendKeyL( );
            break;
        case EKeyBackspace: // clear key
            if ( !MediaIsRunning( iAudio ) &&
                 !MediaIsRunning( iVideo ) )
                {
                DoDeleteAndExitL( );
                }
            break;
        case EKeyNo: // End key
            {
            LOGTEXT(_L8("CMmsViewerAppUi::HandleKeyEventL EKeyNo"));
            if ( MediaIsRunning( iAudio ) )
                {
                MediaStop(iAudio);
                }
            else if ( MediaIsRunning( iVideo ) ) 
                {
                MediaStop(iVideo);
                }
            // fall through
            }
        default:
            // - Audio countdown & audio play cancelled on
            //   HandleViewDeactivation event!!!
            //
            //Power key
            //Apps key
            //End key
            //Up key
            //Down key
            //Numeric keys
            //Clear key
            //Shift key
            return iView->OfferKeyEventL( aKeyEvent, aType );
        }
    return EKeyWasConsumed;
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::HandleWsEventL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::HandleWsEventL(const TWsEvent& aEvent,CCoeControl* aDestination)
    {
  	TInt type = aEvent.Type( );
    switch ( type )
        {
        case EEventFocusGroupChanged:
            {
            if ( !iIsStopPlaybackWgId )
                {
                ResolveCurrentWgIdL( );
                
                // Stop only if temporary change e.g. a waiting note etc.
                if ( iIsStopPlaybackWgId )
              	    {
                    MediaStop(iAudio);
                    MediaStop(iVideo);
                    StopAnimation( );
              	    }
                }
          	// else - no need to stop
            break;
            }
        case EEventFocusGained:
            {
            if ( iIsStopPlaybackWgId )
                {
                iIsStopPlaybackWgId = EFalse;
                StartAnimationL( );
                };
            break;
            }
        // case EEventFocusLost:
        default:
            break;
        }
    CMsgEditorAppUi::HandleWsEventL(aEvent, aDestination);
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::CreateCustomControlL
// ---------------------------------------------------------
//
CMsgBaseControl* CMmsViewerAppUi::CreateCustomControlL(
    TInt /*aControlType*/ )
    {
    return NULL;
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::ShowInformationNoteL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::ShowInformationNoteL( TInt aResourceId )
    {
    HBufC* string = StringLoader::LoadLC( aResourceId, iCoeEnv );
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *string );
    CleanupStack::PopAndDestroy( string ); 
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::ShowErrorNoteL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::ShowErrorNoteL( TInt aResourceId )
    {
    HBufC* string = StringLoader::LoadLC( aResourceId, iCoeEnv );
    CAknErrorNote* note = new ( ELeave ) CAknErrorNote( ETrue );
    note->ExecuteLD( *string );
    CleanupStack::PopAndDestroy( string ); 
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::ShowConfirmationQueryL
// ---------------------------------------------------------
//
TInt CMmsViewerAppUi::ShowConfirmationQueryL(   TInt aResourceId ) const
    {
    HBufC* queryString = StringLoader::LoadLC(
        aResourceId,
        iCoeEnv );
    CAknQueryDialog* dlg = CAknQueryDialog::NewL( );
    TInt retVal = dlg->ExecuteLD( R_MMSVIEWER_CONFIRMATION_QUERY, *queryString );
    CleanupStack::PopAndDestroy( queryString );
    return retVal;
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::ShowConfirmationQueryL
// ---------------------------------------------------------
//
TInt CMmsViewerAppUi::ShowConfirmationQueryL(   TInt            aResourceId,
                                                const TDesC&    aString) const
    {
    HBufC* queryString = StringLoader::LoadLC(
        aResourceId,
        aString,
        iCoeEnv );
    CAknQueryDialog* dlg = CAknQueryDialog::NewL( );
    TInt retVal = dlg->ExecuteLD( R_MMSVIEWER_CONFIRMATION_QUERY, *queryString );
    CleanupStack::PopAndDestroy( queryString );
    return retVal;
    }

// ---------------------------------------------------------
// ShowGuidedModeConfirmationQueryL
// ---------------------------------------------------------
//
TInt CMmsViewerAppUi::ShowGuidedModeConfirmationQueryL( TInt aResourceID )
    {
    TInt retVal = EFalse;
    if ( iViewerFlags & EShowGuidedConf )
        {
        retVal = ShowConfirmationQueryL( aResourceID );
        if ( retVal )
            {
            iViewerFlags &= ~EShowGuidedConf;
            }

        }
    else
        {
        retVal = ( Document( )->CreationMode( ) != EMmsCreationModeRestricted );
        }
    return retVal;
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::ShowOutOfDiskNoteIfBelowCriticalLevelL
// ---------------------------------------------------------
//
TBool CMmsViewerAppUi::ShowOutOfDiskNoteIfBelowCriticalLevelL( TInt aSize )
    {
    //Check whether there is enough free disk
    //to create a copy of the message.
    if ( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL(
        Document( )->Session( ),
        aSize ) )
        {
        CErrorUI* errorUI = CErrorUI::NewLC( *iCoeEnv );
        errorUI->ShowGlobalErrorNoteL( KErrDiskFull );
        //Returns TInt! (ETrue if note was displayed, EFalse otherwise)
        //...ignored...
        CleanupStack::PopAndDestroy( errorUI ); 
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::DoReplyL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoReplyL( TBool aReplyToSender )
    {
    iViewerState = EBusy;
    TRAPD ( error,
        {
        if ( !(iEditorBaseFeatures & EStayInViewerAfterReply )) 
            {
        	MediaStop( );
            ResetViewL( );
            }

        if ( iToolbar )
            {
            iToolbar->HideItemsAndDrawOnlyBackground( ETrue );
            }
        
        // Enable Dialer as MMS Settings dialog may be launched
        iAvkonAppUi->SetKeyEventFlags( 0x00 );	    
    
        if ( aReplyToSender )
            {
            ReplyL( KNewMessageFolder );  //Draft
            }
        else
            {
            ReplyToAllL( KNewMessageFolder );  //Draft
            }

        if (!(iEditorBaseFeatures & EStayInViewerAfterReply))
            {
            Exit( EAknSoftkeyClose );
            }

        if ( iToolbar )
            {
            iToolbar->HideItemsAndDrawOnlyBackground( EFalse );
            }
        iViewerState = ENormal;
        }); //end TRAP
    if ( error )
        {
        LOGTEXT2(_L16("CMmsViewerAppUi::DoReplyL error %d"), error );
        iViewerState = EReseted;
        User::Leave( error );
        }
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::DoForwardL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoForwardL( )
    {
    TInt resourceId = 0;
    Document()->DataModel().FinalizeMediaParse();
    if ( !CanForwardL( resourceId ) )
        {
        // This if cannot be combined with the above
        // one. -> Else would not work correctly.
        if ( resourceId )
            {
            ShowInformationNoteL( resourceId );
            }
        }
    else
        {
        iViewerState = EBusy;
        TRAPD ( error,
            {
            if ( !(iEditorBaseFeatures & EStayInViewerAfterReply )) 
	            {
	            MediaStop( );
	            ResetViewL( );
	            }
            if ( iToolbar )
                {
                iToolbar->HideItemsAndDrawOnlyBackground( ETrue );
                }

            // Enable Dialer as MMS Settings dialog may be launched
            iAvkonAppUi->SetKeyEventFlags( 0x00 );	    
            
            if(iFindItemMenu)
			{
				delete iFindItemMenu;
				iFindItemMenu = NULL;
			}   
    
            ForwardL( KNewMessageFolder );  //Draft
            
            if ( !(iEditorBaseFeatures & EStayInViewerAfterReply )) 
	            {
	        	Exit( EAknSoftkeyClose );
	            }

            if ( iToolbar )
                {
                iToolbar->HideItemsAndDrawOnlyBackground( EFalse );
                }
	        iViewerState = ENormal;    
            } ); //end TRAP
        if ( error )
            {
            LOGTEXT2(_L16("CMmsViewerAppUi::DoForwardL error %d"), error );
            iViewerState = EReseted;
            User::Leave( error );
            }
        }
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::CanForward
// ---------------------------------------------------------
//
TBool CMmsViewerAppUi::CanForwardL( TInt& aResourceId )
    {

    TInt msgSize = Document( )->MessageSize( );
    if ( msgSize > TInt32( Document( )->MaxMessageSize( ) ) )
        {
        // This could happen if MaxReceiveSize > MaxSendSize
        aResourceId = R_MMSVIEWER_CANNOT_FW_BIG;
        return EFalse;
        }

    if ( Document( )->CreationMode( ) == EMmsCreationModeRestricted )
        {
        if ( Document( )->SmilType( ) != EMmsSmil ||
             iViewerFlags & ESuspiciousSmil )
            {
            aResourceId = ( iMmsConformance->CreationModeUserChangeable( ) &&
                            !iMmsConformance->CreationMode( ) ) ?
                        R_MMSVIEWER_RMODE_CANNOT_FW_PRES:
                        R_MMSVIEWER_CANNOT_FW_PRES;
            return EFalse;
            }
        else if ( msgSize > KClassMaxSizeVideoRich )
            {
            aResourceId = R_MMSVIEWER_CANNOT_FW_BIG;
            return EFalse;
            }
        }
    TBool objectsOk = CanForwardObjectsL( aResourceId );
	TInt conformanceSize = KClassMaxSizeVideoRich;
	if(iMmsConformance->ConformanceVersion() > KMmsVersion12)
	{
		conformanceSize = KClassMaxSizeMegapixel;
	}
		
    if ( objectsOk && Document( )->CreationMode( ) == EMmsCreationModeWarning )
        {
        if ( Document( )->SmilType( ) != EMmsSmil ||
             iViewerFlags & ESuspiciousSmil )
            {
            aResourceId = 0;
            return ShowGuidedModeConfirmationQueryL( R_MMSVIEWER_QUEST_GUIDED_PRESENTATION );
            }
        else if ( msgSize > conformanceSize )
            {
            aResourceId = 0;
            return ShowGuidedModeConfirmationQueryL( R_MMSVIEWER_QUEST_GUIDED_INC_OBJ );
            }
        }
    return objectsOk;
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::CanForwardObjectsInListL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::CanForwardObjectsInListL( 
                                                CUniObjectList* aObjectList, 
                                                TInt& aSdCount, 
                                                TInt& aFlCount, 
                                                TInt& aNonConfCount, 
                                                TInt& aNormalCount,
                                                TInt& aResourceId ) const
    {
    __ASSERT_ALWAYS( aObjectList, Panic( EMmsViewerNullPointer ) );
    for ( TInt i = aObjectList->Count( ); --i >= 0; )
        {
        TUint32 confStatus( EMmsConfOk );

        CUniObject* obj = aObjectList->GetByIndex( i );
        if (    obj
            &&  obj->MediaInfo( ) )
            {
            TMmsConformance conf = iMmsConformance->MediaConformance( * ( obj->MediaInfo( ) ) );
            confStatus = conf.iConfStatus;
            }
        if ( confStatus & EMmsConfNokTooBig )
            {
            aFlCount++;
            continue;
            }

        if ( confStatus == EMmsConfOk )
            {
            aNormalCount++;
            continue;
            }

        if ( iSupportedFeatures & EMmsFeatureDrmFull &&
            obj->MediaInfo( )->Protection( ) & EFileProtSuperDistributable )
            {
            aSdCount++;
            continue;
            }

        if ( confStatus & EMmsConfNokDRM )
            {
            
            
            aResourceId = ( iSupportedFeatures & EMmsFeatureDrmFull )
                ? R_MMSVIEWER_CANNOT_FW_DRM
                : R_MMSVIEWER_CANNOT_FW_OBJECTS;

            if (    aResourceId == R_MMSVIEWER_CANNOT_FW_OBJECTS
                &&  (   iMmsConformance->CreationModeUserChangeable( ) &&
                        !iMmsConformance->CreationMode( ) ) )
                {
                aResourceId = R_MMSVIEWER_RMODE_CANNOT_FW_OBJECTS;
                }
            
            aFlCount++;
            continue;
            }

        if ( confStatus != EMmsConfOk )
            {
            aNonConfCount++;
            }
        }
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::CanForwardObjects
// ---------------------------------------------------------
//
TBool CMmsViewerAppUi::CanForwardObjectsL( TInt& aResourceId )
    {

    TInt normalCount = 0;
    TInt sdCount = 0;
    TInt deniedCount = 0;
    TInt nonConfCount = 0;

    CanForwardObjectsInListL( Document( )->ObjectList( ), 
                             sdCount, 
                             deniedCount, 
                             nonConfCount, 
                             normalCount,
                             aResourceId );
    CanForwardObjectsInListL( Document( )->AttachmentList( ), 
                             sdCount, 
                             deniedCount, 
                             nonConfCount, 
                             normalCount,
                             aResourceId );

    if ( deniedCount )
        {
        // resource ID set by CanForwardObjectsInListL
        return EFalse;
        }

    switch ( Document( )->CreationMode( ) )
        {
        case EMmsCreationModeRestricted:
            if ( sdCount || nonConfCount )
                {
                aResourceId = ( iMmsConformance->CreationModeUserChangeable( ) &&
                                !iMmsConformance->CreationMode( ) ) ?
                                R_MMSVIEWER_RMODE_CANNOT_FW_OBJECTS:
                                R_MMSVIEWER_CANNOT_FW_OBJECTS;
                return EFalse;
                }
            break;
        case EMmsCreationModeWarning:
            {
            if ( nonConfCount == 1 )
                {
                aResourceId = 0;
                return ShowGuidedModeConfirmationQueryL( R_MMSVIEWER_QUEST_GUIDED_INC_OBJ );
                }
            else if ( nonConfCount )
                {
                aResourceId = 0;
                return ShowGuidedModeConfirmationQueryL( R_MMSVIEWER_QUEST_GUIDED_INC_OBJS );
                }
            }
            // Fallthrough
        case EMmsCreationModeFree:
        default:
            {
            if ( sdCount )
                {
                // default singular 
                TInt everyWoRights = R_MMSVIEWER_QUEST_SEND_ONE_WO_RIGHTS;
                if ( sdCount > 1)
                    {
                    // plural
                    everyWoRights = R_MMSVIEWER_QUEST_SEND_EVERY_WO_RIGHTS;
                    }
                
                aResourceId = 0; //Make sure resource id is zero.
                return ( ShowConfirmationQueryL( normalCount
                    ? R_MMSVIEWER_QUEST_SEND_SOME_WO_RIGHTS 
                    :  everyWoRights ) );
                }
            }
            break;
        }

    return ETrue;
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::DoReplyViaL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoReplyViaL( TInt aCase )
    {
    if ( iHeader->SenderType( ) != EMuiuAddressTypeNone )
        {
        TInt cleanupCount = 0;

	    CMessageData* msgData = CMessageData::NewLC( );
        cleanupCount++;

	    // Add address and alias
	    // Basically alias is taken from TMsvEntry.iDetails (see MmsViewerHeader.cpp)
	    // Alias can be one of the following in precedence order
	    // 1) local alias
	    // 2) remote alias. Remote alias maybe available when message arrives from 
	    //    email address
	    // 3) sender's address 
	    const TPtrC sender = iMtm->Sender( );
	    
	    if ( sender != KNullDesC( ) )
	        {
	        if ( iHeader->Alias( ) )
	            {
	            //const TPtrC alias = iHeader->Alias( );
	            msgData->AppendToAddressL( TMmsGenUtils::PureAddress( sender ), *iHeader->Alias( ) );
	            }
	        else
	            {
	            msgData->AppendToAddressL( TMmsGenUtils::PureAddress( sender ) );
	            }
	        }

		TUid mtmUid( KSenduiMtmAudioMessageUid );
    	switch( aCase )
    		{
    		case EMmsViewerReplyViaAudio:
    		    // default value OK
                break;
    		case EMmsViewerReplyViaMail:
    		    if ( iHeader->SenderType( ) == EMuiuAddressTypePhoneNumber )
                    {
                    __ASSERT_DEBUG( EFalse, Panic( EMmsViewerUnexpectedReplyEvent ) );
                    }
                mtmUid = KSenduiMtmSmtpUid;
                break;
            default:
                __ASSERT_DEBUG( EFalse, Panic( EMmsViewerUnexpectedReplyEvent ) );
                break;
    		}

        // audio message has not subject field
        if ( mtmUid == KSenduiMtmSmtpUid )
            {
            HBufC* prefixSubject = CreateSubjectPrefixStringL( iMtm->SubjectL( ), ETrue );
            CleanupStack::PushL( prefixSubject ); 
            if ( prefixSubject )
                {
                msgData->SetSubjectL( prefixSubject );
                }
            else
                {
	            TPtrC subjectDes = iMtm->SubjectL( );
	            msgData->SetSubjectL( &subjectDes );
                }
            CleanupStack::PopAndDestroy( prefixSubject ); 
            }

        CSendUi* sendUi = CSendUi::NewLC( );
        cleanupCount++;

        iViewerState = EBusy;
        if ( iToolbar )
            {
            iToolbar->HideItemsAndDrawOnlyBackground( ETrue );
            }

        // Enable Dialer as MMS Settings dialog may be launched
        iAvkonAppUi->SetKeyEventFlags( 0x00 );	    
         
        TRAPD ( error,
            {
            if ( !(iEditorBaseFeatures & EStayInViewerAfterReply ))
            	{
	            MediaStop( );
	            ResetViewL( );	
            	}
            
            sendUi->CreateAndSendMessageL(
	            mtmUid,
	            msgData);
            
            } ); //TRAPD
		CleanupStack::PopAndDestroy( cleanupCount );

        if ( error )
            {
            LOGTEXT2(_L16("CMmsViewerAppUi::DoReplyViaL error %d"), error );
            iViewerState = EReseted;
            User::Leave( error );
            }
		else if ( !(iEditorBaseFeatures & EStayInViewerAfterReply )) 
            {
        	Exit( EAknSoftkeyClose );
            }
        if ( iToolbar )
            {
            iToolbar->HideItemsAndDrawOnlyBackground( EFalse );
            }
        iViewerState = ENormal;
        }
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::DoObjectsL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoObjectsL( )
    {
    MediaClose( iVideo );
    CUniObjectsModel& objectsViewModel =
        static_cast<CUniObjectsModel&>( Document( )->AttachmentModel( ) );

    iViewerState = EBusy;

    if ( iToolbar )
        {
        iToolbar->HideItemsAndDrawOnlyBackground( ETrue );
        }
    // Enable Dialer 
    iAvkonAppUi->SetKeyEventFlags( 0x00 );	    
       
    TUniObjectsDialogExitCmd dlgRet = EUniObjectsViewBack;
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
    iNoTitlePaneUpdate = ETrue;
#endif

    iViewerFlags |= EInObjectsView;
    TRAP_IGNORE (  CUniObjectsViewDialog::ObjectsViewL (
                        CUniObjectsViewDialog::EViewer, 
                    objectsViewModel,
                        iEikonEnv,
                        dlgRet, 
                        Document()->DataModel( ) ) );

    iViewerFlags &= ~EInObjectsView;
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
    iNoTitlePaneUpdate = EFalse;
#endif
    objectsViewModel.Reset();

    if ( !IsAppShutterRunning() )
        {
        if ( iToolbar )
            {
            iToolbar->HideItemsAndDrawOnlyBackground( EFalse );
            }
        iViewerState = ENormal;
        if ( dlgRet == EUniObjectsViewBack )
            {
            if ( iVideo )
                {
                CUniObject* videoObject = ObjectByBaseControl( *iVideo );
                if ( videoObject )
                    {
                    ReloadControlL( iVideo, *videoObject );
                    }
                }
            }
               
            InitNavipaneL( );
           
        }
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::DoShowPresentationL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoShowPresentationL( )
    {
    TMsvAttachmentId smilId = Document( )->SmilList( )->GetSmilAttachmentByIndex( 0 );

    if ( smilId )
        {
        MediaClose();

        // Finish parsing media objects
        Document()->DataModel().FinalizeMediaParse();

        if ( Document( )->SmilType( ) == EMmsSmil &&
            !( iViewerFlags & ESuspiciousSmil ) )
            {
            if ( !iSmilModel->ChangeLayoutL( Document( )->DataModel().Dom( ) ) )
                {
                Document( )->DataModel().SetDom( iSmilModel->ComposeL( ) );
                }

            if ( !iSmilModel->CorrectTimingL( Document( )->DataModel().Dom( ) ) )
                {
                Document( )->DataModel().SetDom( iSmilModel->ComposeL( ) );
                }
            }
                    
        CUniObjectList* objectList = Document( )->ObjectList( );            
        TInt expiredCount = 0;
        TInt norightsCount = 0;
        
        iViewerFlags &= (~EPresentationHasAudio);
        for ( TInt index = 0; index < objectList->Count( ); index++ )
            {
            CUniObject* current = objectList->GetByIndex( index );
            TBool ignoreConsumed (EFalse );
            TInt result = KErrNone;
            if (    current
                &&  current->DrmInfo( ) )
                {
                result = current->DrmInfo( )->EvaluateRights( ignoreConsumed );
                }
            
            LOGTEXT2(_L16("CMmsViewerAppUi::DoShowPresentationL result %d"), result );
            if ( result == DRMCommon::ENoRights )
                {
                norightsCount++;
                }
            else if ( result == DRMCommon::ERightsExpired )
                {
                expiredCount++;
                }
            else if ( current )
                {
                // Rights exist
                // Smil player needs info whether audio clip is present or (one of the) video clip(s)
                // has audio stream
        		TMsgMediaType media = current->MediaType( );
        		if ( media == EMsgMediaAudio )
        			{
        			iViewerFlags |= EPresentationHasAudio;
        			}
        		else if ( media == EMsgMediaVideo &&
        				  static_cast<CMsgVideoInfo*>( current->MediaInfo() )->IsAudio() )
        			{
                        // existence of audio is checked after initialisation
        			iViewerFlags |= EPresentationHasAudio;
        			}
                }
            }

        if ( norightsCount > 0 )
            {
            ShowInformationNoteL( R_QTN_DRM_NO_RIGHTS_FOR_SOME );
            }
        else if ( expiredCount > 0 )
            {
            ShowInformationNoteL( R_QTN_DRM_GEN_RIGHTS_EXP );
            }
            
        delete iSvkEvents;
        iSvkEvents = NULL;
        //EJJN-7MY3SM::MMS viewer, ReTe, RM505_MeDo_ivalo_rnd: Panic with note 
        //"Application closed: Multimedia" occurs after some operation.
        
        CEikButtonGroupContainer* cba1 = CEikButtonGroupContainer::Current();
        if (cba1)
            {
             cba1->MakeVisible(EFalse);
            }
       
        if ( iToolbar )
            {
            iToolbar->SetToolbarVisibility(EFalse, EFalse);
            }
        // Enable Dialer 
        iAvkonAppUi->SetKeyEventFlags( 0x00 );	    
 
        CSmilPlayerDialog* dialog = CSmilPlayerDialog::NewL(
                            Document( )->DataModel().Dom( ),
                            Document( )->ObjectList( ),
			                KNullDesC( ),
			                EFalse,
			                iViewerFlags & EPresentationHasAudio );
                            
        iViewerState = EBusy;
        TRAPD( error, dialog->ExecuteLD( ) );

        CAknAppUiBase* appUi = static_cast<CAknAppUiBase*>( iEikonEnv->EikAppUi( ) );
        TRAP_IGNORE( appUi->SetOrientationL( CAknAppUiBase::EAppUiOrientationAutomatic ) );            
        
        if ( iToolbar )
            {
            iToolbar->SetToolbarVisibility(ETrue, EFalse);
            }

        iViewerState = ENormal;
        
        // re-calculate all pos and dimensions of layout's widgets, after
        // status pane is set back to usual and fixed-toolbar is made visible.
        HandleResourceChangeL(KEikDynamicLayoutVariantSwitch);
        
        if ( !IsAppShutterRunning() )
            {
            ReloadSlideL( );
            }
        else
            {    
            InitNavipaneL( );
            }

        iSvkEvents = CMmsSvkEvents::NewL( *this );
        
        User::LeaveIfError( error );
        }
    }
    
// ---------------------------------------------------------
// CMmsViewerAppUi::DoHandleSendKeyL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoHandleSendKeyL( )
    {
    if ( MediaIsRunning( iAudio ) ||
         MediaIsRunning( iVideo ) )
        {
        //-> inactive
        return;
        }

    if ( !iMmsVoIPExtension )
        {
        iMmsVoIPExtension = CMsgVoIPExtension::NewL( );    
        }
    
    TPtrC senderAddr = KNullDesC( );
    TPtrC senderAlias = KNullDesC( );
    TBool dialerDisabled = EFalse;

    senderAddr.Set( TMmsGenUtils::PureAddress( iMtm->Sender() ) );
    if ( iHeader->Alias( ) )
        {
        senderAlias.Set( *( iHeader->Alias( ) ) );
        }

    TPtrC focusedAddr = KNullDesC( );
    if ( FocusedControlId( ) == EMsgComponentIdBody &&
        iView->ItemFinder( ) )
        {
        const CItemFinder::CFindItemExt& item =
            iView->ItemFinder( )->CurrentItemExt( );
        if (    item.iItemDescriptor
            &&  (   item.iItemType == CItemFinder::EPhoneNumber
                ||  item.iItemType == CItemFinder::EEmailAddress ) )
            {
            	 //Dialer is opened when creating a VoIP call from MMS via Call creation key
            	 //Disabling the dialer when send is pressed
                iAvkonAppUi->SetKeyEventFlags( 
                    CAknAppUiBase::EDisableSendKeyShort | 
                    CAknAppUiBase::EDisableSendKeyLong );		
                dialerDisabled = ETrue;    
                focusedAddr.Set( *(item.iItemDescriptor) );
            }
        }

    if ( FocusedControlId( ) == EMsgComponentIdFrom && 
         senderAddr.Length() && 
         iHeader->SenderType( ) == EMuiuAddressTypePhoneNumber )    
        {
        // Disable dialer
        iAvkonAppUi->SetKeyEventFlags( 
            CAknAppUiBase::EDisableSendKeyShort | 
            CAknAppUiBase::EDisableSendKeyLong );	
        dialerDisabled = ETrue;            
        }
    // Only when dialer is disabled try placing the call    
    if ( dialerDisabled )
        {        
        MsvUiServiceUtilitiesInternal::InternetOrVoiceCallServiceL( 
            *iMmsVoIPExtension,
            senderAddr,
            senderAlias,
            focusedAddr,
            ETrue,
            iEikonEnv );
        }
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::DoMessageInfoL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoMessageInfoL( )
    {
    TPckgBuf<TInt> nullParam( 0 );
    
    //These are not used! They just have to be there.
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );

    CMsvSingleOpWatcher* watch = CMsvSingleOpWatcher::NewLC( *Document( ) );
    CMsvOperation* op = Document( )->MtmUiL( ).InvokeAsyncFunctionL(
        KMtmUiFunctionMessageInfo,
        *selection,
        watch->iStatus,
        nullParam );
    CleanupStack::Pop( watch ); 
    Document( )->AddSingleOperationL( op, watch );

    CleanupStack::PopAndDestroy( selection ); 
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::DoDeleteAndExitL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoDeleteAndExitL( )
    {
    // Confirm from user deletion of message and delete if needed.
    if ( ShowConfirmationQueryL( R_MMSVIEWER_QUEST_DELETE_MESSAGE ) )
        {
        MediaStop( );
        //Reset view (i.e. close all files!)
        //If image is open, extra delay needs to be added that
        //server side components can finish their tasks 
        iViewerState = EReseted;
        ResetViewL( );
        // create delay, because every media object is not necessary closed immediately
        Document( )->DeleteModel( );
        User::After(50000);
        DeleteAndExitL( );
        }
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::DoMoveMessageL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoMoveMessageL( )
    {
    //ask folder with a dialog (from muiu)
    TMsvId target = Document( )->Entry( ).Parent( );
    HBufC* title = StringLoader::LoadLC( R_MMSVIEWER_MOVE_TEXT, iCoeEnv );
    TBool success = CMsgFolderSelectionDialog::SelectFolderL(target, *title);
    CleanupStack::PopAndDestroy( title ); 
    if (success)
        {
        MediaStop( );
        iViewerState = EReseted;
        ResetViewL( );
        iViewerState = EBusy;
        TInt error(KErrNone);
        TRAP(error, MoveMessageEntryL( target ) );
        iViewerState = EReseted;
        
        LOGTEXT2(_L16("CMmsViewerAppUi::DoMoveMessageL %d"), error );
        User::LeaveIfError( error );
        Exit( EAknSoftkeyClose );
        }
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::IsOwnMessage
// ---------------------------------------------------------
//
TBool CMmsViewerAppUi::IsOwnMessage( ) const
    {
    const TMmsMsvEntry* mmsEntry =
        static_cast<const TMmsMsvEntry*>( &Document( )->Entry( ) );
    return ( !(mmsEntry->IsMobileTerminated( ) ) );
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::HasSender
// ---------------------------------------------------------
//
TBool CMmsViewerAppUi::HasSender( ) const
    {
    return ( iMtm->Sender( ).Length( ) > 0 );
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::ShowReplyToAll
// ---------------------------------------------------------
//
TBool CMmsViewerAppUi::ShowReplyToAll( ) const
    {
    return ( iMtm->AddresseeList( ).Count( ) > 
        ( IsOwnMessage( ) ? 0 : 1 ) );
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::ShowReplyMenu
// ---------------------------------------------------------
//
TBool CMmsViewerAppUi::ShowReplyMenu( ) const
    {
    if ( Document( )->IsUpload( ) )
        {
        return EFalse;
        }
    else
        {
        return ( HasSender( ) || ShowReplyToAll( ) );
        }
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::ResetViewL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::ResetViewL( )
    {
    iVideo = NULL;
    iAudio = NULL;
    iImage = NULL;
#ifdef RD_SVGT_IN_MESSAGING
    iSvgt = NULL;
#endif    

    if ( iSlideLoader )
        {
        iSlideLoader->ResetViewL( );
        }
    // When attachments view is visible but not focused, eye striking focus movement takes
    // place to attachments field e.g durign forwarding. 
    iHeader->RemoveAttachmentL(); 
    iHeader->RemoveFromViewL( );
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::EditorObserver
// ---------------------------------------------------------
//
void CMmsViewerAppUi::EditorObserver(TMsgEditorObserverFunc aFunc, TAny* aArg1, TAny* aArg2, TAny* aArg3)
    {
    TRAP_IGNORE(DoEditorObserverL(aFunc,aArg1,aArg2,aArg3 ) );
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::EditorObserver
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoEditorObserverL(TMsgEditorObserverFunc aFunc, TAny* aArg1, TAny* aArg2, TAny* aArg3)
    {
    TInt slides = iSmilModel->SlideCount( );
    TBool multiSlide =
        ( Document( )->SmilType( ) == EMmsSmil && slides > 1 );
    switch ( aFunc )
        {
        case EMsgScrollParts:
            {
            if ( multiSlide )
                {
                TInt* parts = static_cast<TInt*>( aArg1 );
                *parts = slides;
                }
            }
            break;
        case EMsgHandleFocusChange:
            {
            // Enable Dialer 
            iAvkonAppUi->SetKeyEventFlags( 0x00 );
            
            TMsgFocusEvent event =
                *( static_cast<TMsgFocusEvent*>( aArg1 ) );
            TMsgAfterFocusEventFunc* after =
                static_cast<TMsgAfterFocusEventFunc*>( aArg2 );
            TInt* currPart =
                static_cast<TInt*>( aArg3 );

            switch (event)
                {
                case EMsgFocusAtBottom:
#ifdef RD_SCALABLE_UI_V2
                    // check the legality of slide change
                    if ( multiSlide && *currPart > Document( )->CurrentSlide( ) )
                        {
                        TInt moveToSlide( *currPart );
                        if ( moveToSlide >= slides )
                            {
                            moveToSlide = slides - 1;
                            }
                        
                        ChangeSlideL( moveToSlide, ETrue );
                        *after = EMsgCursorToBodyBeginning;
                        }
                    else
                        {
                        *after = EMsgAfterFocusNone;
                        }
                    *currPart = Document( )->CurrentSlide( );
#else
                    if ( multiSlide && Document( )->CurrentSlide( ) + 1 < slides )
                        {
                        ChangeSlideL( Document( )->CurrentSlide( ) + 1, ETrue);
                        *after = EMsgCursorToBodyBeginning;
                        *currPart = Document( )->CurrentSlide( );
                        }
#endif
                    break;
                case EMsgFocusAtTop:
#ifdef RD_SCALABLE_UI_V2
                    // check the legality of slide change
                    if ( multiSlide && *currPart < Document( )->CurrentSlide( ) )
                        {
                        TInt moveToSlide( *currPart );
                        if ( moveToSlide < 0 )
                            {
                            moveToSlide = 0;
                            }
                        
                        ChangeSlideL( moveToSlide, ETrue);
                        *after = EMsgCursorToBodyEnd;
                        }
                    else
                        {
                        *after = EMsgAfterFocusNone;
                        }
                    *currPart = Document( )->CurrentSlide( );
#else
                    if ( multiSlide && Document( )->CurrentSlide( ) > 0 )
                        {
                        ChangeSlideL( Document( )->CurrentSlide( ) - 1, ETrue);
                        *after = EMsgCursorToBodyEnd;
                        *currPart = Document( )->CurrentSlide( );
                        }
#endif
                    break;
                case EMsgFocusMovingFrom:
                    {
                    if ( MediaIsRunning( iVideo ) )
                        {
                        MediaStop( iVideo );
                        }
                    break;
                    }
                case EMsgFocusMovedTo:
                    {
                    StartAnimationL( );
                    UpdateMskL( );
                    break;
                    }
                case EMsgFocusToBody:
                case EMsgFocusToHeader:
                default:
                    break;
                }
            }
            break;
        case EMsgControlPointerEvent:
            {
#ifdef RD_SCALABLE_UI_V2
            if ( AknLayoutUtils::PenEnabled() )
                {
                TPointerEvent* pointerEvent = static_cast<TPointerEvent*>( aArg2 );
                CMsgBaseControl* baseControl = static_cast<CMsgBaseControl*>( aArg1 );
                TWsEvent* wsEvent = static_cast<TWsEvent*>(aArg2);
                
                if (    pointerEvent
                    &&  pointerEvent->iType == TPointerEvent::EButton1Down )
                    {
                    // We are looking for a valid tap (button down and up)
                    // on a focused object.
                    iPointerTarget = baseControl;
                    
                    if ( baseControl && 
                             ( baseControl->ControlId() == EMsgComponentIdAudio ||
                               baseControl->ControlId() == EMsgComponentIdImage ||
                               baseControl->ControlId() == EMsgComponentIdVideo ||
                               baseControl->ControlId() == EMsgComponentIdSvg )&&
                                     ((Document()->SmilType()!=ETemplateSmil)&&(Document()->SmilType()!=E3GPPSmil) )) 
                        {
                        if(iLongTapDetector)
                            {
                            iLongTapDetector->EnableLongTapAnimation(ETrue);
                            iLongTapDetector->PointerEventL( *pointerEvent );
                            }
                        }
                    iTapConsumed = EFalse;
                    }
                else if ( (!iTapConsumed) && (pointerEvent
                        &&  pointerEvent->iType == TPointerEvent::EButton1Up) )
                    {   
                    iLongTapDetector->MonitorWsMessage(*wsEvent);
                    iTapConsumed = ETrue;
                    CMsgBaseControl* focusedControl = iView->FocusedControl();
                    if ( baseControl && iPointerTarget == baseControl )
                        {
                        switch( iMskId )
                            {
                            // Tapping on selected object functions as a
                            // selection key was pressed. But command set
                            // is limited to ones below.
                            case R_MMSVIEWER_MSK_BUTTON_PLAY_PRESENTATION: // fallthrough
                            case R_MMSVIEWER_MSK_BUTTON_PLAY_AUDIO: // fallthrough
                            case R_MMSVIEWER_MSK_BUTTON_OPEN_IMAGE: // fallthrough
                            case R_MMSVIEWER_MSK_BUTTON_PLAY_VIDEO: // fallthrough
                            case R_MMSVIEWER_MSK_BUTTON_STOP_AUDIO:
                            case R_MMSVIEWER_MSK_BUTTON_STOP_VIDEO:
                            case R_MMSVIEWER_MSK_BUTTON_OPEN_OBJECTS:
#ifdef RD_SVGT_IN_MESSAGING                 	
                            case R_MMSVIEWER_MSK_BUTTON_PLAY_SVG:
#endif                
                                {
                                HandleSelectionKeyL();
                                *static_cast<TBool*>( aArg3 ) = ETrue;  // handled
                                }
                            default:
                                ;
                            }
                        }
                    else if ( focusedControl && iPointerTarget == focusedControl )
                        {
                            if ( iMskId == R_MMSVIEWER_MSK_BUTTON_PLAY_VIDEO ||
                                 iMskId == R_MMSVIEWER_MSK_BUTTON_STOP_VIDEO )
                                {
                                HandleSelectionKeyL();
                                *static_cast<TBool*>( aArg3 ) = ETrue;  // handled
                                }
                        }

                    iPointerTarget = NULL;
                    }
                }
#endif // RD_SCALABLE_UI_V2
            break;
            }
        case EMsgButtonEvent:
            {    
#ifdef RD_SCALABLE_UI_V2
            if ( AknLayoutUtils::PenEnabled() )
                {                    
                if ( iHeader->AttachmentControl( ) == static_cast<CMsgBaseControl*>( aArg1 ) )
                    {
                    DoObjectsL();
                    }
                }
#endif // RD_SCALABLE_UI_V2
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::HandleNotifyInt
//
// Notification from central repository
// ---------------------------------------------------------
//
void CMmsViewerAppUi::HandleNotifyInt( TUint32 /*aId*/, TInt aNewValue )
    {    
    if ( aNewValue == 0 )
        {
        iViewerFlags &= ~EAutohighLightEnabled;
        }
    else
        {
        iViewerFlags |= EAutohighLightEnabled;
        }
    TRAP_IGNORE( SetFindModeL( iViewerFlags & EAutohighLightEnabled ) );
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::HandleNotifyGeneric
// ---------------------------------------------------------
//
void CMmsViewerAppUi::HandleNotifyGeneric( TUint32 /*aId*/ )
    {
    //Nothing.
    }
    
// ---------------------------------------------------------
// CMmsViewerAppUi::HandleNotifyError
// ---------------------------------------------------------
//
void CMmsViewerAppUi::HandleNotifyError( TUint32 /*aId*/, TInt /*error*/, CCenRepNotifyHandler* /*aHandler*/ )
    {
    //Nothing.
    }
  
//  
// NOTICE:
//
// Startup observer moved to own class because Publish & Subscribe
// requires an active object for event handling.
//

// ---------------------------------------------------------
// CMmsViewerAppUi::HandleIteratorEventL
// ---------------------------------------------------------
//
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
void CMmsViewerAppUi::HandleIteratorEventL( TMessageIteratorEvent aEvent )
    {
    if (    aEvent == EFolderCountChanged 
        &&  iViewerState == ENormal 
        &&  !iNoTitlePaneUpdate )
        {
        InitNavipaneL( );
        }
    }
#endif

// ---------------------------------------------------------
// MmsSvkChangeVolumeL
// ---------------------------------------------------------
// Callback function
void CMmsViewerAppUi::MmsSvkChangeVolumeL( TInt aVolumeChange )
    {    
    ChangeVolumeL( aVolumeChange, ETrue );
    }

// ---------------------------------------------------------
// ChangeVolumeL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::ChangeVolumeL( TInt aVolume, TBool aChange )
    {    
    TBool audioPlaying ( MediaCanStop( iAudio ) );
    TBool videoPlaying =
        ( ( MediaCanStop( iVideo ) ) &&
        ( FocusedControlId( ) == EMsgComponentIdVideo ) );

    if ( audioPlaying || videoPlaying )
        {
        TInt newVolume = DoChangeVolumeL( aVolume, aChange, audioPlaying );
        if ( newVolume >= KVolumeControlMinValue )
            {
            // Finally show the volume
            ShowVolumeL( newVolume, audioPlaying );
            }
        }
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::DoChangeVolumeL
// ---------------------------------------------------------
// 
TInt CMmsViewerAppUi::DoChangeVolumeL( TInt aVolume, TBool aChange, TBool aAudioPlaying )
    {
#ifdef _DEBUG
    if ( aAudioPlaying )
        {
        __ASSERT_DEBUG( iAudio, Panic( EMmsViewerNullPointer ) );
        }
    else
        {
        __ASSERT_DEBUG( iVideo, Panic( EMmsViewerNullPointer ) );
        }
#endif
    TInt newVolume = aVolume;
    // In practise the variable indicates whether volume indicator is shown in the navi pane
    // and whether audio can be heard
    TBool updateIntoCenRepAllowed( aAudioPlaying );

    // Changes the volume level into the CenRep, if:
    // 1) audio is playing or
    // 2) video is playing and video has audio and silent mode is not On
    // 
    // Sets the volume level into audio/video player if 
    // 1) as above
    // 2) video is playing and silent mode is On
    // 
    if (    aAudioPlaying 
        ||  (   !aAudioPlaying 
            &&  iViewerFlags & EVideoHasAudio ) )
        {
        if (    !aAudioPlaying
            &&  iViewerFlags & EVideoHasAudio
            &&  !IsSilenceL( ) )
            {
            updateIntoCenRepAllowed = ETrue;
            }
        else if ( !aAudioPlaying )
            {
            updateIntoCenRepAllowed = EFalse;
            }
        
        // First determine new value to be written into Cenrep
        TInt currVolume( KVolumeDefault );
        if ( aChange )
            {
            iMmsRepository->Get( KMmsuiHandsfreeVolume, currVolume );
            newVolume = currVolume+aVolume;
            }
        else
            {
            // force change
            currVolume = KErrNotFound;
            }
        // else - newVolume has correct value and currVolume forces always change

        newVolume = Max( KVolumeControlMinValue,
                        Min( KVolumeControlMaxValue, newVolume ) );
        if (    updateIntoCenRepAllowed 
            &&  newVolume != currVolume )
            {
            iMmsRepository->Set( KMmsuiHandsfreeVolume, newVolume );
            }

        // Then write the value into control
        if ( aAudioPlaying )
            {
            if (iAudio)
                {            
                TInt maxVolume = reinterpret_cast<CMsgAudioControl*>(iAudio)->MaxVolume( );
                reinterpret_cast<CMsgAudioControl*>(iAudio)->
                    SetVolume( ( newVolume * maxVolume ) / KVolumeLevels );
                }
            }
        else
            {
            if ( iVideo )
                {
                if ( !updateIntoCenRepAllowed )
                    {
                    newVolume = KVolumeControlMinValuePlayer;
                    }
                
                TInt maxVolume = reinterpret_cast<CMsgVideoControl*>( iVideo )->MaxVolume( );
                TInt dummy(KErrNone);
                TRAP( dummy, 
                    reinterpret_cast<CMsgVideoControl*>(iVideo)->
                        SetVolumeL( ( newVolume * maxVolume ) / KVolumeLevels ) );
                }
            }
        }
    if ( !updateIntoCenRepAllowed )
        {
        newVolume = KErrNotFound;
        }
    return newVolume;
    }

// ---------------------------------------------------------
// ShowVolumeL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::ShowVolumeL(  TInt  aNewVolume,
                                    TBool aAudioPlaying )
    {
    // Shows volume control, if:
    // 1) audio is playing or
    // 2) video is playing and (either or both of the below conditions is ETrue )
    //   - silent mode is not On or 
    //   - video has audio
    if (    aAudioPlaying 
        ||  (   !aAudioPlaying 
            &&  (    iViewerFlags & EVideoHasAudio )
                ||  !IsSilenceL( ) ) )
        {
        if ( !iVolumeDecorator )
            {
            iVolumeDecorator = iNaviPane->CreateVolumeIndicatorL( R_AVKON_NAVI_PANE_VOLUME_INDICATOR );

#ifdef RD_SCALABLE_UI_V2
            if ( AknLayoutUtils::PenEnabled() )
                {
                static_cast<CAknVolumeControl*>
                    ( iVolumeDecorator->DecoratedControl() )->SetObserver( this );
                }
#endif
            }

        if ( ! ( iViewerFlags & EVolumeControlTriggered ) )
            {
            // Set new volume and show it
            static_cast<CAknVolumeControl*>
                ( iVolumeDecorator->DecoratedControl() )->SetValue( aNewVolume );
            }
        iNaviPane->PushL( *iVolumeDecorator );
        }
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::MsgAsyncControlStateChanged
// ---------------------------------------------------------
//
void CMmsViewerAppUi::MsgAsyncControlStateChanged(   
                                    CMsgBaseControl& aControl,
                                    TMsgAsyncControlState aNewState,
                                    TMsgAsyncControlState aOldState )

    {
    switch ( aControl.ControlId( ) )
        {
        case EMsgComponentIdAudio:
        case EMsgComponentIdVideo:
        case EMsgComponentIdImage:
        case EMsgComponentIdSvg:
            // Must be derived from CMsgMediaControl
            TRAP_IGNORE( DoMsgMediaControlStateChangedL( 
                            static_cast<CMsgMediaControl&>( aControl ), 
                            aNewState, 
                            aOldState ) );
            break;
        default:
            // not derived from CMsgMediaControl
            break;
        }
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::MsgAsyncControlStateChanged
// ---------------------------------------------------------
void CMmsViewerAppUi::MsgAsyncControlResourceChanged( CMsgBaseControl& aControl, TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch &&
         aControl.ControlType() == EMsgImageControl )
        {
        CMsgImageControl& imageControl = static_cast<CMsgImageControl&>( aControl );
        
        if ( imageControl.IconBitmapId() == EMbmMmsuiQgn_graf_mms_play )
            {
            // 3GPP icons needs to be handled by ourself
            TAknLayoutRect iconLayout;
            iconLayout.LayoutRect( MsgEditorCommons::MsgDataPane(),
                                   AknLayoutScalable_Apps::msg_data_pane_g4().LayoutLine() );
    
            TRAP_IGNORE( imageControl.SetIconSizeL( iconLayout.Rect().Size() ) );
            }
        }
    }
    
// ---------------------------------------------------------
// CMmsViewerAppUi::DoMsgMediaControlStateChangingL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoMsgMediaControlStateChangedL(   
                                    CMsgMediaControl&        aControl,
                                    TMsgAsyncControlState   aNewState,
                                    TMsgAsyncControlState   aOldState )
    {
#ifdef USE_LOGGER
    switch ( aControl.ControlId( ) )
        {
        case EMsgComponentIdAudio:
            LOGTEXT2(_L16("CMmsViewerAppUi::MsgMediaControlStateChanged. Audio: Old state %d"), aOldState );
            LOGTEXT2(_L16("                                              Audio: new state %d"), aNewState );
            LOGTEXT2(_L16("                                              Audio: error %d"), aControl.Error() );
            if ( aNewState == EMsgAsyncControlStateReady )
                {
                LOGTIMESTAMP("CMmsViewerAppUi::DoMsgMediaControlStateChangedL Audio is ready ");
                }
            break;
        case EMsgComponentIdVideo:
            LOGTEXT2(_L16("CMmsViewerAppUi::MsgMediaControlStateChanged. Video: Old state %d"), aOldState );
            LOGTEXT2(_L16("                                              Video: new state %d"), aNewState );
            LOGTEXT2(_L16("                                              Video: error %d"), aControl.Error() );
            if ( aNewState == EMsgAsyncControlStateReady )
                {
                LOGTIMESTAMP("CMmsViewerAppUi::DoMsgMediaControlStateChangedL Video is ready ");
                }
            break;
        case EMsgComponentIdImage:
            LOGTEXT2(_L16("CMmsViewerAppUi::MsgMediaControlStateChanged. Image: Old state %d"), aOldState );
            LOGTEXT2(_L16("                                              Image: new state %d"), aNewState );
            LOGTEXT2(_L16("                                              Image: error %d"), aControl.Error() );
            if ( aNewState == EMsgAsyncControlStateReady )
                {
                LOGTIMESTAMP("CMmsViewerAppUi::DoMsgMediaControlStateChangedL Image is ready ");
                }
            break;
        case EMsgComponentIdSvg:
            LOGTEXT2(_L16("CMmsViewerAppUi::MsgMediaControlStateChanged. Svg: Old state %d"), aOldState );
            LOGTEXT2(_L16("                                              Svg: new state %d"), aNewState );
            LOGTEXT2(_L16("                                              Svg: error %d"), aControl.Error() );
            if ( aNewState == EMsgAsyncControlStateReady )
                {
                LOGTIMESTAMP("CMmsViewerAppUi::DoMsgMediaControlStateChangedL Svg is ready ");
                }
            break;
        default:
            // not derived from CMsgMediaControl
            break;
        }
#endif

    // Resets iOpeningState
    ShowPendingNoteL( );

    // component independent
    switch ( aNewState )
        {
        case EMsgAsyncControlStatePlaying:
            UpdatePanesL( MediaIsRunning( iAudio ) ,MediaIsRunning( iVideo ) );
            break;

        case EMsgAsyncControlStateStopped:
        case EMsgAsyncControlStateNoRights:
        case EMsgAsyncControlStateCorrupt:
            UpdatePanesL( EFalse, EFalse );
            break;

        default:
            break;
        }

    // Keep UpdateMskL() after UpdatePanesL( )
    UpdateMskL( );    

    // component specific
    switch ( aControl.ControlId( ) )
        {
        case EMsgComponentIdVideo:
            {
            if ( aNewState == EMsgAsyncControlStateReady &&
                 !static_cast<CMsgVideoControl&>( aControl ).HasAudioL() )
                {
                iViewerFlags &= ~EVideoHasAudio;
                }
            else if ( aNewState == EMsgAsyncControlStateReady )
                {
                // Lie that video is playing to set initial volume level.
                DoChangeVolumeL( 0, ETrue, EFalse ); 
                }
#ifdef USE_LOGGER
			if ( aNewState == EMsgAsyncControlStateReady )
				{
				LOGTEXT2(_L16("CMmsViewerAppUi::DoMsgMediaControlStateChangedL. Video: HasAudioL()? %d"), static_cast<CMsgVideoControl&>( aControl ).HasAudioL() );
				}
#endif
            if (    aNewState == EMsgAsyncControlStateStopped
                ||  aNewState == EMsgAsyncControlStateNoRights
                ||  aNewState == EMsgAsyncControlStateCorrupt  )
                {
                CUniObject* object = ObjectByMediaControl( aControl );
                if (    object
                    &&  object->DrmInfo( ) )
                    {
                    TInt err = object->DrmInfo( )->ReleaseRights( );
                    }
                }
            break;
            }
            // fall through
        case EMsgComponentIdAudio:
            {
            if ( aNewState == EMsgAsyncControlStateReady )
                {
                // Lie that audio is playing to set initial volume level.
                DoChangeVolumeL( 0, ETrue, ETrue ); 
                }
            if (    aNewState == EMsgAsyncControlStateStopped
                ||  aNewState == EMsgAsyncControlStateNoRights
                ||  aNewState == EMsgAsyncControlStateCorrupt )
                {
                CUniObject* object = ObjectByMediaControl( aControl );
                if (    object
                    &&  object->DrmInfo( ) )
                    {
                    TInt err = object->DrmInfo( )->ReleaseRights( );
                    }
                }
            break;
            }
        case EMsgComponentIdImage:
            StartAnimationL();
            // fall through
        case EMsgComponentIdSvg:
            if ( aNewState == EMsgAsyncControlStateIdle )
                {
                CUniObject* object = ObjectByMediaControl( aControl );
                if (    object
                    &&  object->DrmInfo( ) )
                    {
                    TInt err = object->DrmInfo( )->ReleaseRights( );
                    }
                }
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::LaunchHelpL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::LaunchHelpL( )
    {
    // activate Help application
    
	if ( iSupportedFeatures & EMmsFeatureHelp )
		{
	    CArrayFix<TCoeHelpContext>* helpContext = AppHelpContextL( );
		HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession( ), helpContext );
		}
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::HelpContextL
// ---------------------------------------------------------
//
CArrayFix<TCoeHelpContext>* CMmsViewerAppUi::HelpContextL( ) const
    {
	if ( iSupportedFeatures & EMmsFeatureHelp )
		{
	    CArrayFix<TCoeHelpContext>* r = new ( ELeave ) CArrayFixFlat<TCoeHelpContext>(1);
		CleanupStack::PushL( r );     
	    r->AppendL( TCoeHelpContext( KUidMmsViewer, KMMS_HLP_MMS_VIEWER( ) ) );
		CleanupStack::Pop( r );
		return r;
		}
	return NULL;
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::SetFindModeL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::SetFindModeL( TBool aEnable )
    {
    if ( iView )
        {
        if ( iView->ItemFinder( ) )
            {
            TInt findMode = aEnable ?
                CItemFinder::EPhoneNumber |
                CItemFinder::EUrlAddress |
                CItemFinder::EEmailAddress :
                CItemFinder::ENoneSelected;
            iView->ItemFinder( )->SetFindModeL( findMode );
            }
        CMsgBaseControl* fromControl = iView->ControlById( EMsgComponentIdFrom );
        if ( fromControl && iMtm->Sender( ).Length( ) )
            {
            static_cast<CMsgAddressControl*>( fromControl )
                ->SetAddressFieldAutoHighlight( aEnable );
            }
        }
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::MediaOpenL
// for single control
// ---------------------------------------------------------
// This function is entered when selection key is pressed.
// Actions allowed are play, embedded play, activate and get rights.
void CMmsViewerAppUi::MediaOpenL( CMsgMediaControl* aMedia )
    {
    if ( aMedia )
        {
        if ( MediaInitializedL( aMedia ) )
            {
            MediaPlayL( aMedia );
            }
        }
    } 


// ---------------------------------------------------------
// CMmsViewerAppUi::MediaPlayL
// for single control
// ---------------------------------------------------------
void CMmsViewerAppUi::MediaPlayL(CMsgMediaControl* aMedia)
    {
    if ( aMedia )
        {        
        CUniObject* currObject = ObjectByMediaControl( *aMedia );
        if (    currObject
            &&  currObject->Corrupted() )
            {
            UpdatePanesL( MediaIsRunning( iAudio ) ,MediaIsRunning( iVideo ) );
            UpdateMskL( );
            ShowInformationNoteL( R_QTN_MMS_CANNOT_OPEN_CORRUPTED );
            return;
            }
#ifdef RD_SVGT_IN_MESSAGING
        if ( aMedia == iSvgt || aMedia == iImage )
            {
            ViewImageL( *aMedia );
            }
#else        
        if (aMedia == iImage)
            {
            ViewImageL( reinterpret_cast< CMsgImageControl& >( *aMedia ) );
            }
#endif            
        else 
            {
            if (aMedia == iVideo)
                {
                if(IsVideoCall())
                {
                	ShowInformationNoteL( R_MMSVIEWER_CANNOT_PLAY_VIDEOCALL_ONGOING );
                	return;                
                }
                reinterpret_cast<CMsgVideoControl*>(aMedia)->ContinueOnFocus( ETrue );
                }
            else if (   aMedia == iAudio )
                {
                __ASSERT_ALWAYS(    ObjectByMediaControl( *iAudio ) || ObjectByMediaControl( *iAudio )->MediaInfo( ), 
                                    Panic( EMmsViewerNullPointer ) );
                CUniObject* obj = ObjectByMediaControl( *iAudio );
                // Coverty fix, Null pointer return, http://ousrv057/cov.cgi?cid=37099
                if (obj)
                	{
                    if (    iViewerFlags & EProhibitNonDrmMusic 
                    &&  !( obj->MediaInfo( )->Protection( ) &
                        ( EFileProtSuperDistributable | EFileProtForwardLocked ) ) )
                        {
                        // Prepare buffer for aMimeType
                        HBufC* mimeBuffer = HBufC::NewLC( obj->MimeType().Length() + KMmsViewerSpace().Length() );
                        TPtr mimeBufferPtr = mimeBuffer->Des();
                        mimeBufferPtr.Copy( obj->MimeType() );

                        // FindF() would find "audio/3gpp" in "audio/3gpp2" without
                        // the added space.
                        mimeBufferPtr.Append( KMmsViewerSpace );

                        // If result is not KErrNotFound, this MIME-type is indeed on blocked list.
                        if ( iProhibitMimeTypeBuffer->FindF( mimeBufferPtr ) >= 0 ) 
                           {
                           ShowInformationNoteL( R_MMSVIEWER_PLAIN_MUSIC_PROHIBITED );
                           /* Allocated buffer must be destroyed and pop'd !!! */
                           CleanupStack::PopAndDestroy( mimeBuffer );
                            return;
                           }
                         /* Allocated buffer must be destroyed and pop'd !!! */
                        CleanupStack::PopAndDestroy( mimeBuffer );
                        }
                     }
                if (    IsSilenceL( ) 
                    &&  !ShowConfirmationQueryL( R_QTN_MMS_OBEY_SILENT_MODE ) )
                    {
                    return;
                    }
                }
                
            if ( !HandleConsumeRightsL( *aMedia ) )
                {
                UpdatePanesL( MediaIsRunning( iAudio ) ,MediaIsRunning( iVideo ) );
                UpdateMskL( );
                return;
                }
            aMedia->PlayL( );
            }
        }
    } 

// ---------------------------------------------------------
// CMmsViewerAppUi::MediaStop
// for every control
// ---------------------------------------------------------
//
void CMmsViewerAppUi::MediaStop( )
    {
    MediaStop( iImage );
    MediaStop( iAudio );
    MediaStop( iVideo );
#ifdef RD_SVGT_IN_MESSAGING
    MediaStop( iSvgt );
#endif
    } 


// ---------------------------------------------------------
// CMmsViewerAppUi::MediaStop
// for single control
// ---------------------------------------------------------
//
void CMmsViewerAppUi::MediaStop(CMsgMediaControl* aMedia )
    {
    if ( MediaCanStop( aMedia ) )
        {
        if (aMedia == iVideo)
            {
            reinterpret_cast<CMsgVideoControl*>( aMedia )->ContinueOnFocus( EFalse );
            }
        CUniObject* object = ObjectByMediaControl( *aMedia );
        if (    object
            &&  object->DrmInfo( ) )
            {
            TInt err = object->DrmInfo( )->ReleaseRights( );
            }
        aMedia->Stop( );
        }    
    } 


// ---------------------------------------------------------
// CMmsViewerAppUi::MediaClose
// for every control
// ---------------------------------------------------------
//
void CMmsViewerAppUi::MediaClose( )
    {
    MediaClose( iImage );
    MediaClose( iAudio );
    MediaClose( iVideo );
#ifdef RD_SVGT_IN_MESSAGING
    MediaClose( iSvgt );
#endif    
    } 

// ---------------------------------------------------------
// CMmsViewerAppUi::MediaClose
// for single control
// ---------------------------------------------------------
//
void CMmsViewerAppUi::MediaClose(CMsgMediaControl* aMedia)
    {    
    if ( aMedia )
        {
        if ( MediaCanStop ( aMedia ) )
            {
            aMedia->Stop( );
            }    
        
        // Release rights - image only
        if (    aMedia
            &&  iImage == aMedia )
            {                    
            CUniObject* object = ObjectByMediaControl( *aMedia );
            if (    object
                &&  object->DrmInfo( ) )
                {
                TInt err = object->DrmInfo( )->ReleaseRights( );
                }
            }
        aMedia->Close( );
        }    
    } 

// ---------------------------------------------------------
// CMmsViewerAppUi::ReloadSlideL
// for every control
// ---------------------------------------------------------
//
void CMmsViewerAppUi::ReloadSlideL( )
    {
    LOGTEXT(_L8("CMmsViewerAppUi::ReloadSlideL"));
    // Get focused control to restore focus after other view
    if ( iView->FocusedControl( ) )
        {                
        iFocusedControlId = iView->FocusedControl( )->ControlId();  
        if ( iFocusedControlId )
            {
            // Disable autofind temporarily, if On, because when slide is reloaded
            // focus moves to From: field temporarily.
            // Autohighlight is disabled temporarily
            if (    !IsOwnMessage( )
                &&  iFocusedControlId != EMsgComponentIdFrom )
                {
                // Get highlight status        
                TInt highlight = 0;
                iCUiRepository->Get( KCuiAutomaticHighlight, highlight  );
                if ( highlight )
                    {
                    SetFindModeL( EFalse );
                    }
                }  
            }
        }

    if ( Document( )->SmilType( ) == EMmsSmil )
        {
        ChangeSlideL( Document( )->CurrentSlide( ), EFalse );
        }
        SetFindModeL( iViewerFlags & EAutohighLightEnabled );

    InitNavipaneL( );
    } 

// ---------------------------------------------------------
// CMmsViewerAppUi::ReloadControlL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::ReloadControlL( CMsgMediaControl* aControl, CUniObject& aObject )
    {
    LOGTEXT(_L8("CMmsViewerAppUi::ReloadControlL"));
    if (    aControl
        &&  Document( )->SmilType( ) == EMmsSmil )
        {
        // Get focused control to restore focus after other view
        if ( iView->FocusedControl( ) )
            {                
            iFocusedControlId = iView->FocusedControl( )->ControlId();  
            if ( iFocusedControlId )
                {
                // Disable autofind temporarily, if On, because when slide is reloaded
                // focus moves to From: field temporarily.
                // Autohighlight is disabled temporarily
                if (    !IsOwnMessage( )
                    &&  iFocusedControlId != EMsgComponentIdFrom )
                    {
                    // Get highlight status        
                    TInt highlight = 0;
                    iCUiRepository->Get( KCuiAutomaticHighlight, highlight  );
                    if ( highlight )
                        {
                        SetFindModeL( EFalse );
                        }
                    }  
                }
            }

        MediaStop( );

        if (    iVideo 
            &&  iVideo == aControl )
            {
            iScreenClearer = CAknLocalScreenClearer::NewLC( EFalse );
            CleanupStack::Pop( iScreenClearer );
            }
        
        iViewerState = EBusy;
        iOpeningState = EOpeningSlide;
        iViewerFlags |= EVideoHasAudio;
        iVideo = NULL;
        iAudio = NULL;
        iImage = NULL;
    #ifdef RD_SVGT_IN_MESSAGING
        iSvgt = NULL;
    #endif    
        iPendingNoteResource = KInvalidResourceId;
        iPendingNotePriority = 0;
        iPendingNoteObject = NULL;
        iPendingNoteError = KErrNone;

        delete iFindItemMenu;
        iFindItemMenu = NULL;

        UpdateMskL();

        if ( !iChangeSlideOperation )
            {
            iChangeSlideOperation = new ( ELeave )CMmsViewerChangeSlideOperation (
                *this,
                *( Document( ) ),
                *iHeader,
                *iSlideLoader,
                *iView,
                iCoeEnv->FsSession( ) );
            }
            
        ActivateInputBlockerL( iChangeSlideOperation );
        iChangeSlideOperation->ReLoadControlL( aControl, &aObject );
        
        // TODO: Get rid of active wait!
        BeginActiveWait( iChangeSlideOperation );
        InitNavipaneL( );
        }
    } 


// ---------------------------------------------------------
// CMmsViewerAppUi::StopAnimation
// ---------------------------------------------------------
//
void CMmsViewerAppUi::StopAnimation( )
    {
    if (    iImage
        &&  iImage->IsAnimation( ) )
        {        
        iImage->Stop( );
        }    
    } 

// ---------------------------------------------------------
// CMmsViewerAppUi::StartAnimationL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::StartAnimationL( )
    {
    if (    iImage
        &&  iImage->IsAnimation( ) )
        {            
        TMsgAsyncControlState state = iImage->State( );        
        if (    state == EMsgAsyncControlStateReady 
            ||  state == EMsgAsyncControlStatePaused
            ||  state == EMsgAsyncControlStateStopped )
            {
            iImage->PlayL( );
            }    
        }
    } 

// ---------------------------------------------------------
// CMmsViewerAppUi::ViewerOperationEvent
// ---------------------------------------------------------
//
void CMmsViewerAppUi::ViewerOperationEvent(
        TMmsViewerOperationType     aOperation,
        TMmsViewerOperationEvent    aEvent,
        TInt                        aError)
    {
    if ( aEvent == EMmsViewerOperationCancel )
        {
        LOGTEXT(_L8("CMmsViewerAppUi::ViewerOperationEvent EMmsViewerOperationCancel") );
        // Operation by operation should be considered what is proper action 
        // in Cancel situation
        ResetOperationState();
        return;
        }  

    if (    aOperation == EMmsViewerOperationReadReport
        &&  aEvent == EMmsViewerOperationError )
        {
        // Not worth exiting
        aError = KErrNone;
        }

    TInt error(aError);
    if ( !error )
        {

        TRAP( error, DoViewerOperationEventL( aOperation, aEvent ) );
        if ( aEvent == EMmsViewerOperationCancel )
            {
            // ignore errors
            error = KErrNone;
            }
        }
    else
        {
        ResetOperationState();
        if ( error == KErrCorrupt )
            {
            TRAP_IGNORE( ShowErrorNoteL( R_MMSVIEWER_ERROR_MSG_CORRUPT ) );
            }
        }
    
    if ( error )
        {
        MediaStop( );
        Exit( error );
        }
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::ViewerOperationEvent
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoViewerOperationEventL(
        TMmsViewerOperationType     aOperation,
        TMmsViewerOperationEvent    aEvent )
    {
    TInt error( KErrNone );
    switch ( aOperation )
        {
        case EMmsViewerOperationChangeSlide:
            {
            EndActiveWait( );
            // restore focus
            if ( iFocusedControlId > EMsgComponentIdNull )
                {
                if ( FocusedControlId( ) != iFocusedControlId )
                    {
                    CMsgBaseControl* control = iView->ControlById( iFocusedControlId ) ;
                    if ( control )
                        {
                        iView->SetFocus( iFocusedControlId );
                        }
                    }
                iFocusedControlId = EMsgComponentIdNull;
                }
            iImage = reinterpret_cast< CMsgImageControl* > 
                        ( iView->ControlById( EMsgComponentIdImage ) );
            HandleImageNoRightsInLaunch( );
            StartAnimationL( );
            iAudio = reinterpret_cast< CMsgAudioControl* > 
                        ( iView->ControlById( EMsgComponentIdAudio ) );
            iVideo = reinterpret_cast< CMsgVideoControl* > 
                        ( iView->ControlById( EMsgComponentIdVideo ) );
#ifdef RD_SVGT_IN_MESSAGING
            iSvgt =  reinterpret_cast< CMsgSvgControl* > 
                        ( iView->ControlById( EMsgComponentIdSvg ) );                        
#endif                        
            break;
            }
        case EMmsViewerOperationLaunch:
            {
            if ( aEvent == EMmsViewerOperationCancel )
                {
                return;
                }
            else
                {       
                LOGTEXT(_L8("CMmsViewerAppUi::DoViewerOperationEventL DoViewerOperationEventLaunchL in "));
                TRAP( error, DoViewerOperationEventLaunchL( ) );
                LOGTEXT(_L8("CMmsViewerAppUi::DoViewerOperationEventL DoViewerOperationEventLaunchL out "));
                }
            break;
            }
        case EMmsViewerOperationReadReport:
            // free resources
            DeactivateInputBlocker();
            delete iSendReadReportOperation;
            iSendReadReportOperation = NULL;
            // Viewer is already running normally
            return;            
            
        default:
            break;
        }
    
    ResetOperationState();
    User::LeaveIfError( error );

    SetFindModeL( iViewerFlags & EAutohighLightEnabled );
    ShowPendingNoteL( );

    // iViewer state must be ENormal.
    // UpdateMskL() must be after UpdatePanesL()
    UpdatePanesL( EFalse, EFalse );
    UpdateMskL( );             

    // Call of SendReadReportL cannot be in DoViewerOperationEventLaunchL, because
    // ResetOperationState() deletes the input blocker launched from SendReadReportL
    if (    aOperation == EMmsViewerOperationLaunch
        &&  aEvent == EMmsViewerOperationComplete )
        {
        // Less important stuff follows.
        // It is OK, that flags and state indicate successful launch
#ifndef USE_LOGGER
        TRAP_IGNORE( SendReadReportL( aOperation, aEvent ) );
#else
        TRAPD( error, SendReadReportL( aOperation, aEvent  ) );
        if ( error )
            {
            LOGTEXT2(_L16("CMmsViewerAppUi::DoViewerOperationEventLaunchL SendReadReportL error %d"), error );
            }
#endif  // USE_LOGGER
        }
    }

// ---------------------------------------------------------
// DoViewerOperationEventLaunchL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoViewerOperationEventLaunchL( )
    {
    if ( Document( )->ParseResult( ) &
        ( ERegionMediaTypeMismatch | EBadlyReferencedObjects ) )
        {
        iViewerFlags |= ESuspiciousSmil;
        }
    
    iSmilModel = Document( )->SmilModel( );
    iHeader = iLaunchOperation->DetachHeader( );
    iSlideLoader = iLaunchOperation->DetachSlideLoader( );
   
    TMsvEntry entry = iMtm->Entry( ).Entry( );
    if ( entry.iMtmData1 & EMmsDrmCorruptedAttachment )
        {
        SetPendingNote( R_MMSVIEWER_DRM_OBJECT_REMOVED );
        }
    
    // Condition was EUnsupportedObjects, but region may be resolved even
    // if it is set. 
    if ( Document( )->ParseResult( ) & EBadlyReferencedObjects  )
        {
        SetPendingNote( R_MMSVIEWER_CANNOT_DISPLAY_OBJECTS );
        }

    if ( Document( )->AttachmentList( )->Count( ) )
        {
        SetPendingNote( R_MMSVIEWER_CANNOT_DISPLAY_OBJECTS );
        }
    
    ResolveDrmWaitingNotes( );   // e.g. R_MMSVIEWER_DRM_PREVIEW_RIGHTS

    TInt focusedControl = ( Document( )->SmilType( ) != EMmsSmil )
        ? EMsgComponentIdImage
        : EMsgComponentIdNull;
    iView->ExecuteL( ClientRect( ), focusedControl );

    SetTitleIconL();
    InitNavipaneL( );    
    TRAP_IGNORE( CheckAndSetDescriptionL( ) );

    if (focusedControl != EMsgComponentIdImage )
        {
        // 3GPP icon is not assigned
        iImage = reinterpret_cast< CMsgImageControl* > 
                    ( iView->ControlById( EMsgComponentIdImage ) );
        HandleImageNoRightsInLaunch( );
        StartAnimationL( );
        }
    iAudio = reinterpret_cast< CMsgAudioControl* > 
                ( iView->ControlById( EMsgComponentIdAudio ) );
    iVideo = reinterpret_cast< CMsgVideoControl* > 
                ( iView->ControlById( EMsgComponentIdVideo ) );
#ifdef RD_SVGT_IN_MESSAGING             
    iSvgt = reinterpret_cast< CMsgSvgControl* > 
                ( iView->ControlById( EMsgComponentIdSvg ) );
#endif                    
    // Enable task swapper to options menu after launch has been completed.
    MenuBar()->SetMenuType( CEikMenuBar::EMenuOptions );
    
#ifdef RD_SCALABLE_UI_V2
    if ( iToolbar )
        { 
        SetToolBarItemVisibilities();
        }
#endif
    }

// ---------------------------------------------------------
// BeginActiveWait
// ---------------------------------------------------------
//
void CMmsViewerAppUi::BeginActiveWait( CMmsViewerOperation* aOperation )
    {
    if( iWait.IsStarted() )
        {
        if ( iActiveOperation )
            {
            iActiveOperation->Cancel();
            }
        else
            {
            return;
            }                
        }
    iActiveOperation = aOperation;
    iWait.Start();
    }

// ---------------------------------------------------------
// EndActiveWait
// ---------------------------------------------------------
//
void CMmsViewerAppUi::EndActiveWait( )
    {
    if( !iWait.IsStarted( ) )
        {
        return;
        }
    iWait.AsyncStop( );
    iActiveOperation = NULL;
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::FocusedControlId
// ---------------------------------------------------------
//
TInt CMmsViewerAppUi::FocusedControlId( )
    {
    TInt controlType = EMsgComponentIdNull;
    if ( iView && iView->FocusedControl( ) )
        {
        controlType = iView->FocusedControl( )->ControlId( );
        }
    return controlType;
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::MediaControlById
// ---------------------------------------------------------
//
CMsgMediaControl* CMmsViewerAppUi::MediaControlById(TInt aControl)
    {
    CMsgMediaControl* control = NULL;
    switch (aControl)
        {
        case EMsgComponentIdVideo:
            control = iVideo;
            break;
        case EMsgComponentIdAudio:
            control = iAudio;
            break;
        case EMsgComponentIdImage:
            control = iImage;
            break;
#ifdef RD_SVGT_IN_MESSAGING
        case EMsgComponentIdSvg:
            control = iSvgt;
            break;
#endif            
        default:
            break;
        }
    return control;
    } 
    
// ---------------------------------------------------------
// CMmsViewerAppUi::MediaInitializedL
// ---------------------------------------------------------
//
TBool CMmsViewerAppUi::MediaInitializedL( CMsgMediaControl* aMedia )
    {
    if ( aMedia )
        {
        TBool noRights( EFalse );        
        CUniObject* currObject = ObjectByMediaControl( *aMedia );
        // test has two purpose
        // If object has not rights, assume it is initialised.
        // Image: If rights have not been consumed, object had not rights.
        // Other objects: DRM rights do not matter, because they are handled separately  during
        // 'launch'.
        if (    currObject 
            &&  currObject->DrmInfo() )
            {
            if (    aMedia == iImage )
                {
                noRights = !IsRightsConsumed( *currObject );
                }
            else
                {
                noRights =  (   !IsRightsConsumed( *currObject ) 
                            &&  !IsRightsNow( *currObject ) );
                }
            }

        if ( noRights || MediaCanPlay( aMedia, currObject ) )
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }
    // else - no media - assume initialized
    return ETrue;
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::MediaCanPlay( )
// ---------------------------------------------------------
//
TBool CMmsViewerAppUi::MediaCanPlay(const CMsgMediaControl* aMedia,
                                    const CUniObject* aMmsObject ) const
    {
    TBool corrupted( EFalse );
    if (    aMmsObject
        &&  aMmsObject->Corrupted() )
        {
        corrupted = ETrue;
        }
    
    if ( aMedia )
        {        
        TMsgAsyncControlState state = aMedia->State( );
        if (    state == EMsgAsyncControlStateReady 
            ||  state == EMsgAsyncControlStatePaused
            ||  state == EMsgAsyncControlStateStopped
            ||  state == EMsgAsyncControlStateNoRights
            ||  state == EMsgAsyncControlStateCorrupt
            ||  corrupted     
            ||  (   aMedia == iImage
                &&  (   state == EMsgAsyncControlStatePlaying
                    ||  state == EMsgAsyncControlStatePaused )
                &&  iImage->IsAnimation() ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::MediaCanStop( )
// ---------------------------------------------------------
//
TBool CMmsViewerAppUi::MediaCanStop(CMsgMediaControl* aMedia)
    {
    if ( aMedia )
        {
        TMsgAsyncControlState state = aMedia->State( );
        if (    state == EMsgAsyncControlStateAboutToPlay
            ||  state == EMsgAsyncControlStateBuffering
            ||  state == EMsgAsyncControlStatePlaying
            ||  state == EMsgAsyncControlStatePaused )
            {
            return ETrue;
            }
        }
    return EFalse;
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::MediaIsRunning( )
// ---------------------------------------------------------
//
TBool CMmsViewerAppUi::MediaIsRunning(CMsgMediaControl* aMedia)
    {
    if ( aMedia )
        {
        TMsgAsyncControlState state = aMedia->State( );
        if ( state == EMsgAsyncControlStatePlaying )
            {
            return ETrue;
            }
        }
    return EFalse;
    }



// ---------------------------------------------------------
// CMmsViewerAppUi::ResolveDrmWaitingNotes( )
// ---------------------------------------------------------
//
void CMmsViewerAppUi::ResolveDrmWaitingNotes( )
    {
        // resolve need of R_MMSVIEWER_DRM_PREVIEW_RIGHTS
    TBool jumpOut(EFalse);
    
        // go through all recipients
    CUniObjectList* objectList = Document( )->ObjectList( );
    TInt objectCount = 0;
    if (objectList)
        {
        objectCount = objectList->Count( );
        }
        
    for ( TInt i = 0; i < objectCount && !jumpOut; i++ )
        {
        CUniObject* obj = objectList->GetByIndex( i );
        if ( obj )
            {
            switch ( obj->Region( ) )
                {
                case EUniRegionImage:
                    // fall through
                case EUniRegionAudio:
                    if (    obj->DrmInfo( )
                        &&  obj->DrmInfo( )->IsDrmWithOnePreview( ) )
                        {
                        SetPendingNote( R_MMSVIEWER_DRM_PREVIEW_RIGHTS, obj );
                        jumpOut = ETrue;
                        }
                    break;
                case EUniRegionText:
                case EUniRegionUnresolved:
                default:
                    // not interested
                    break;
                }
            }
        }
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::SaveObjectL( )
// ---------------------------------------------------------
//
void CMmsViewerAppUi::SaveObjectL( )
    {
    // returns only image, audio, video and svg controls
    CMsgMediaControl* mediaControl = MediaControlById( FocusedControlId( ) );
    CMsgBaseControl* control = mediaControl;
    if ( !control )
        {
        if (    iView->FocusedControl()
            &&  iView->FocusedControl()->ControlType() == EMsgXhtmlBodyControl )
            {
            control = iView->FocusedControl();
            }
        }

    if ( control )
        {
        // if-branch intended for multimedia and xHTML
        CUniObject* obj = ObjectByBaseControl( *control );
        if ( obj )
            {   
            DoSaveObjectL( *obj );
            }
        }
    else if (FocusedControlId( ) == EMsgComponentIdBody )
        {
        CUniObject* textObj = NULL;
        TUint objectCount = iSmilModel->SlideObjectCount( Document( )->CurrentSlide( ) );
        for ( TUint i = 0; i < objectCount; i++ )
            {
            CUniObject* obj = iSmilModel->GetObjectByIndex( Document( )->CurrentSlide( ), i );
            if ( obj->Region( ) == EUniRegionText )
                {
                // found
                textObj = obj;
                break;
                }
            }

        if (    textObj
            &&  textObj->MediaType( ) == EMsgMediaText )
            {
            // Save text files directly to Notepad.
            // This is needed because automatic character set recognation
            // is not too reliable.
            TUint mibCharset = textObj->MimeInfo( )->Charset( );
            if ( !mibCharset )
                {
                //assume US-ASCII - mandated by RFC 2046
                mibCharset = KCharacterSetMIBEnumUsAscii;
                }

            TUint charconvCharsetID = Document( )->DataUtils( )->MibIdToCharconvIdL( mibCharset );
            RFile textFile = Document( )->GetAttachmentFileL( *textObj );

            CleanupClosePushL( textFile );
            
            CNotepadApi::SaveFileAsMemoL(
                textFile, 
                charconvCharsetID );

            CleanupStack::PopAndDestroy(&textFile);

            // Show confirmation note
            RApaLsSession appList;
            if ( appList.Connect( ) == KErrNone )
                {
                TApaAppInfo appInfo;
                if ( appList.GetAppInfo( appInfo, KNotepadUID3 ) == KErrNone )
                    {
                    textObj->SetSaved( ETrue );
                    HBufC* text = StringLoader::LoadLC( R_MMS_FILE_SAVED_TO, appInfo.iCaption );
                    CAknConfirmationNote* note = new ( ELeave ) CAknConfirmationNote( ETrue );
                    note->ExecuteLD( *text );
                    CleanupStack::PopAndDestroy( text );
                    }
                appList.Close( );
                }
            }
        }
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::DoSaveObjectL( )
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoSaveObjectL(CUniObject& aObject )
    {
    CMsgMediaInfo* media = aObject.MediaInfo( );
    if ( media )
        {                
        TParsePtrC plainFileName( media->FullFilePath( ) );
        const TPtrC newName = plainFileName.NameAndExt( );
        TDataType dataType( aObject.MimeType( ) );
  
        CDocumentHandler* docHandler = CDocumentHandler::NewL( );
        CleanupStack::PushL(docHandler);

        RFile mediaFile = Document( )->GetAttachmentFileL( aObject );
        CleanupClosePushL( mediaFile );
        
        TInt ret = docHandler->CopyL(   mediaFile, 
                                        newName, 
                                        dataType, 
                                        KEntryAttNormal );
        if ( ret != KErrNone && ret != KUserCancel )
            {
            User::LeaveIfError( ret );
            }
        else if (   ret == KErrNone ) 
            {
            aObject.SetSaved( ETrue );
            }

        CleanupStack::PopAndDestroy(2, docHandler); // + mediaFile
        }
    }

        
// ----------------------------------------------------
// IsSilenceL
// 
// ----------------------------------------------------
TBool CMmsViewerAppUi:: IsSilenceL( )
    {
    TBool result( EFalse );
    
    CRepository* repository = CRepository::NewLC( KCRUidProfileEngine );
    
    TInt ringType( EProfileRingingTypeRinging );
    User::LeaveIfError( repository->Get( KProEngActiveRingingType, ringType ) );
    CleanupStack::PopAndDestroy( repository );
    
    if ( ringType == EProfileRingingTypeSilent )
        {
        result = ETrue;
        }
    return result;
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::OpenDeliveryPopupL( )
// Forwards the request to the MMS UI 
// ---------------------------------------------------------
//
void CMmsViewerAppUi::OpenDeliveryPopupL( )
    {
   
   //These are not used! They just have to be there.
    TPckgBuf<TInt> param( 0 ); 
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );

    CMsvSingleOpWatcher* watch = CMsvSingleOpWatcher::NewLC( *Document( ) );
    CMsvOperation* op = Document( )->MtmUiL( ).InvokeAsyncFunctionL( 
        KMtmUiFunctionDeliveryStatus,
        *selection,
        watch->iStatus,
        param ); // launch the dialog through MMSUI
    CleanupStack::Pop( watch ); 
    Document( )->AddSingleOperationL( op, watch );

    CleanupStack::PopAndDestroy( selection );
    }


// ---------------------------------------------------------
// CMmsViewerAppUi::HandleResourceChangeL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::HandleResourceChangeL(TInt aType)
    {
    // Base class call must be first
    CMsgEditorAppUi::HandleResourceChangeL(aType);

    if ( iHeader )
        {
        iHeader->HandleResourceChange(aType);
        }

    if ( aType == KAknsMessageSkinChange )
        {
        if ( iAppIcon )
            {
            delete iAppIcon;
            iAppIcon = NULL;
            }
    
      	// Set path of bitmap file
      	TParse fileParse;
      	fileParse.Set( KMmsUiMbmFile, &KDC_APP_BITMAP_DIR, NULL );
        
        iAppIcon = AknsUtils::CreateGulIconL( 
            AknsUtils::SkinInstance(), 
            KAknsIIDQgnPropMceMmsTitle,
            fileParse.FullName(),
            EMbmMmsuiQgn_prop_mce_mms_title,
            EMbmMmsuiQgn_prop_mce_mms_title_mask );
        SetTitleIconL();   
        }
    else if ( aType == KEikDynamicLayoutVariantSwitch && IsLaunched() )
        {
        SetTitleIconL();   
        }
    }

// ---------------------------------------------------------
// IsLaunched
// ---------------------------------------------------------
inline TBool CMmsViewerAppUi::IsLaunched( ) const
    {
    return ( iViewerFlags & ELaunchSuccessful );
    }


// ---------------------------------------------------------
// PrepareToExit
// ---------------------------------------------------------
void  CMmsViewerAppUi::PrepareToExit( )
    {
    CEikAppUi::PrepareToExit( );
    }
    
// ---------------------------------------------------------
// ObjectByMediaControl
// ---------------------------------------------------------
CUniObject* CMmsViewerAppUi::ObjectByMediaControl( CMsgMediaControl& aControl )
    {  
    TInt controlId = aControl.ControlId( );
    if (    controlId == EMsgComponentIdImage
        ||  controlId == EMsgComponentIdAudio
        ||  controlId == EMsgComponentIdVideo 
#ifdef RD_SVGT_IN_MESSAGING
        ||  controlId == EMsgComponentIdSvg
#endif            
        )
        {
        return ObjectByBaseControl( aControl );
        }
    else
        {
        return NULL;
        }
    
    }

// ---------------------------------------------------------
// ObjectByBaseControl
// ---------------------------------------------------------
CUniObject* CMmsViewerAppUi::ObjectByBaseControl( CMsgBaseControl& aControl )
    {    
    TInt controlId = aControl.ControlId( );
    TUniRegion region( EUniRegionUnresolved );
    switch ( controlId )
        {
        case EMsgComponentIdImage:
            region = EUniRegionImage;
            break;
        case EMsgComponentIdAudio:
            region = EUniRegionAudio;
            break;
        case EMsgComponentIdVideo:
            region = EUniRegionImage;
            break;
#ifdef RD_SVGT_IN_MESSAGING
        case EMsgComponentIdSvg:
            region = EUniRegionImage;
            break;
#endif            
        case EMsgComponentIdBody:
            region = EUniRegionText;
            break;
        default:
            break;
        }
    if ( region == EUniRegionUnresolved )
        {
        return NULL;
        }
              
    if ( iSmilModel )
        {         
        TInt countObjectsOnSlide( 
                iSmilModel->SlideObjectCount( Document( )->CurrentSlide( ) ) );
        for ( TInt i = 0;  i < countObjectsOnSlide; i++ )
            {
            CUniObject* object = iSmilModel->GetObject( 
                                    Document( )->CurrentSlide( ), region );
            if ( object )
                {
                return object;
                }
            }
        }
    return NULL;
    }

    
// ---------------------------------------------------------
// CMmsViewerAppUi::SetAndSaveDescriptionL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::CheckAndSetDescriptionL( )
    {    
    TMsvEntry tEntry = Document( )->Entry( );
    if( !tEntry.iDescription.Length( ) )
        {
        // is it on current slide
        CMsgBodyControl* bodyControl = static_cast<CMsgBodyControl*>
            ( iView->ControlById( EMsgComponentIdBody ) );
        if ( bodyControl )
            {
            SetAndSaveDescriptionL(bodyControl->TextContent( ));
            }
        else 
            {
            // Continue searching from other slides
            // Take first text attachment     
            CUniSmilModel* smilModel = Document( )->SmilModel( );
            if ( smilModel )
                {
                // search for first text attachment
                TInt slideCount = smilModel->SlideCount( );
                for (TInt slideI = 1; slideI < slideCount ; slideI++ )
                    {
                    TInt objectCountOnSlide = smilModel->SlideObjectCount( slideI );
                    for (TInt objectI = 0; objectI < objectCountOnSlide ; objectI++ )
                        {
                        CUniObject* obj = smilModel->GetObjectByIndex( slideI, objectI );
                        if (    obj 
                            &&  obj->Region( ) == EUniRegionText )
                            {
                            TUint attaCharset = obj->MimeInfo( )->Charset( );
                            if ( !attaCharset )
                                {
                                //assume US-ASCII - mandated by RFC 2046
                                attaCharset = KMmsUsAscii;
                                }

                            TUint charconvCharsetID = 0;
                            charconvCharsetID = Document( )->DataUtils()->MibIdToCharconvIdL( attaCharset );
                            RFile file = Document( )->GetAttachmentFileL( *obj );
                            CleanupClosePushL( file );
                            
                            RFileReadStream stream( file );
                            CleanupClosePushL( stream );
                            
                            CPlainText::TImportExportParam param;
                            param.iForeignEncoding = charconvCharsetID;
                            param.iOrganisation = CPlainText::EOrganiseByParagraph; 
                            
                            CPlainText::TImportExportResult result;
                                                
                            CPlainText* plainText = CPlainText::NewL( );
                            CleanupStack::PushL( plainText );
                            
                            plainText->ImportTextL( 0, stream, param, result );
                            SetAndSaveDescriptionL( *plainText );
                            
                            CleanupStack::PopAndDestroy( 3, &file ); // plainText, file, stream
                            break;
                            } // else - no object or wrong type of object
                        } // for - objects of one slide
                    } // for - slides
                }// else - no smil model
            } // else - no body text on first slide
        } // else - description exists
    }
    
// ---------------------------------------------------------
// CMmsViewerAppUi::SetAndSaveDescriptionL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::SetAndSaveDescriptionL(const CPlainText& aText)
    {
    TBuf<KMmsMaxDescription> description;
    description.Zero( );
    aText.Extract( description, 0, KMmsMaxDescription );
    TInt length( description.Length( ) );
    while( length-- )
        {

        if (    description[length] == CEditableText::ETabCharacter
            ||  description[length] == CEditableText::EPageBreak
            ||  description[length] == CEditableText::ENonBreakingSpace
            ||  description[length] == CEditableText::EHyphen
            ||  description[length] == CEditableText::ENonBreakingHyphen
            ||  description[length] == CEditableText::ELeftSingleQuote
            ||  description[length] == CEditableText::ERightSingleQuote
            ||  description[length] == CEditableText::ELeftDoubleQuote
            ||  description[length] == CEditableText::ERightDoubleQuote
            ||  description[length] == CEditableText::EBullet
            ||  description[length] == CEditableText::EEllipsis
            ||  description[length] == CEditableText::ELineBreak
            ||  description[length] == CEditableText::EParagraphDelimiter 
            ||  description[length] == CEditableText::EPictureCharacter
            ||  description[length] == CEditableText::EZeroWidthNoBreakSpace
            ||  description[length] == CEditableText::EByteOrderMark
            ||  description[length] == CEditableText::EReversedByteOrderMark            
            ||  description[length] == '\n' )
            {
            description[length] = ' ';
            }
        }
    description.Trim( );

    if ( description.Length( ) > 0 )
        {
        TMsvEntry tEntry = Document( )->Entry( );        
        tEntry.iDescription.Set( description );
        Document( )->CurrentEntry( ).ChangeL( tEntry );
        }
    }



// ---------------------------------------------------------
// CMmsViewerAppUi::ViewImageL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::ViewImageL( CMsgMediaControl& aMediaCtrl )
    {
    CUniObject* launchObject = ObjectByMediaControl( aMediaCtrl );
	// check if valid object is selected and check if the image is already being viewed
	// as to avoid doing it again.
    if ( !launchObject || iViewerFlags & EImageBeingViewed )
        {
        return;
        }
      
    if ( !iDocHandler )
        {
        iDocHandler = CDocumentHandler::NewL( );
        iDocHandler->SetExitObserver( this );
        }
    
    if ( &aMediaCtrl == iImage && 
        ( reinterpret_cast< CMsgImageControl& >( aMediaCtrl ).IsAnimation() ) )       
        {
        aMediaCtrl.Stop( );
        }
    
    if ( launchObject->DrmInfo() )
        {
        launchObject->DrmInfo()->ReleaseRights();
        }
        
    TDataType mimeDataType( launchObject->MimeType() );

    RFile sharedFile = Document( )->GetAttachmentFileL( *launchObject );
    CleanupClosePushL( sharedFile );
    
    CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC();   
    if ( !launchObject->IsSaved() )
        {
        TAiwGenericParam genericParamAllowSave
            ( EGenericParamAllowSave, ETrue);
        paramList->AppendL( genericParamAllowSave );
        }

    TInt err( KErrNone );
    TRAP( err, err = iDocHandler->OpenFileEmbeddedL(    sharedFile,
                                                        mimeDataType,
                                                        *paramList ) );
    if( err == KErrNone )
		{
        iViewerFlags |= EImageBeingViewed;
		}
    
    LOGTEXT2(_L16("CMmsViewerAppUi::ViewImageL error %d"), err );
    CleanupStack::PopAndDestroy( 2, &sharedFile );  // + paramList
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::HandleServerAppExit
// ---------------------------------------------------------
//
void CMmsViewerAppUi::HandleServerAppExit(TInt /*aReason*/)
    {
    if ( iViewerFlags & EImageBeingViewed )
        {
        iViewerFlags &= ( ~EImageBeingViewed );
        }
    
    // Currently only image is possible
    if (    iImage 
        &&  iImage->IsAnimation() )
        {
        TRAP_IGNORE( iImage->PlayL( ) );
        }
    }
    
// ---------------------------------------------------------
// CMmsViewerAppUi::HandleConsumeRightsL
// ---------------------------------------------------------
//
TBool CMmsViewerAppUi::HandleConsumeRightsL( CMsgMediaControl& aMedia ) 
    {        
    TBool result( ETrue );

    CUniObject* object = ObjectByMediaControl( aMedia);
    TBuf<ContentAccess::KMaxCafUniqueId> defaultContent( ContentAccess::KDefaultContentObject( ) );
    if (    object
        &&  object->DrmInfo( )
        &&  defaultContent == aMedia.UniqueContentId( ) )
        {
        TInt err = object->DrmInfo( )->ConsumeRights( );
        switch ( err )
            {
            case KErrNone:
                break;
            case DRMCommon::EGeneralError:
            case DRMCommon::EUnknownMIME:
            case DRMCommon::EVersionNotSupported:
            case DRMCommon::ESessionError:
            case DRMCommon::ENoRights:
            case DRMCommon::ERightsDBCorrupted:
            case DRMCommon::EUnsupported:
            case DRMCommon::ERightsExpired:
            case DRMCommon::EInvalidRights:
                SetPendingNote( R_MMSVIEWER_DRM_ERROR, 
                                object, 
                                err );
                result = EFalse;
                break;
            default: 
                SetPendingNote(R_QTN_MMS_CANNOT_OPEN_CORRUPTED);
                object->SetCorrupted( ETrue );            
                result = EFalse;
                break;
            }
        }

    if ( !result )
        {
        iSlideLoader->UpdateControlIconL( aMedia, object );
        aMedia.DrawDeferred();
        ShowPendingNoteL( );
        }
    return result; 
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::HandleImageNoRightsInLaunch
// ---------------------------------------------------------
//
void CMmsViewerAppUi::HandleImageNoRightsInLaunch( ) 
    {        
    if ( iImage )
        {
        CUniObject* object = ObjectByMediaControl( *iImage );
        if (    object
            &&  object->DrmInfo( ) )
            {
            TBool ignoreConsumed ( EFalse );
            TInt err = object->DrmInfo( )->EvaluateRights( ignoreConsumed );
            switch ( err )
                {
                case KErrNone:
                    break;
                case DRMCommon::EGeneralError:
                case DRMCommon::EUnknownMIME:
                case DRMCommon::EVersionNotSupported:
                case DRMCommon::ESessionError:
                case DRMCommon::ENoRights:
                case DRMCommon::ERightsDBCorrupted:
                case DRMCommon::EUnsupported:
                case DRMCommon::ERightsExpired:
                case DRMCommon::EInvalidRights:
                    SetPendingNote( R_MMSVIEWER_DRM_ERROR, 
                                    object, 
                                    err );
                    break;
                default: 
                    break;
                }
            }
        }
    }

// ---------------------------------------------------------
// CMmsViewerAppUi::UpdateMskL
// ---------------------------------------------------------
void CMmsViewerAppUi::UpdateMskL( )
    {
	TInt resId = (-1);
    if ( iView && iViewerState == ENormal ) 
        {
        CMsgBaseControl* ctrl = iView->FocusedControl();
        if ( ctrl )
            {
            // If entered here, some command must be set even empty
            switch ( ctrl->ControlId() )
                {
                case EMsgComponentIdAttachment:
                    resId = R_MMSVIEWER_MSK_BUTTON_OPEN_OBJECTS;
                    break;
                case EMsgComponentIdImage:
                    if ( Document( )->SmilType( ) != EMmsSmil )
                        {
                        resId = R_MMSVIEWER_MSK_BUTTON_PLAY_PRESENTATION;
                        }
                    else if (   MediaInitializedL( iImage ) )
                        {    
                        resId = R_MMSVIEWER_MSK_BUTTON_OPEN_IMAGE;
                        }
                    else
                        {
                        resId = R_MMSVIEWER_MSK_NO_COMMAND_EMPTY_TEXT;
                        }
                    break;
                case EMsgComponentIdAudio:
                    if ( MediaIsRunning( iAudio ) )
                        {
                        resId = R_MMSVIEWER_MSK_BUTTON_STOP_AUDIO;
                        }                        
                    else if (   MediaInitializedL( iAudio ) )
                        {
                        resId = R_MMSVIEWER_MSK_BUTTON_PLAY_AUDIO;
                        }
                    else
                        {
                        resId = R_MMSVIEWER_MSK_NO_COMMAND_EMPTY_TEXT;
                        }
                	break;
                case EMsgComponentIdVideo:
                    if ( MediaIsRunning( iVideo ) )
                        {
                        resId = R_MMSVIEWER_MSK_BUTTON_STOP_VIDEO;
                        }
                    else if (   MediaInitializedL( iVideo ) )
                        {
                        resId = R_MMSVIEWER_MSK_BUTTON_PLAY_VIDEO;
                        }
                    else
                        {
                        resId = R_MMSVIEWER_MSK_NO_COMMAND_EMPTY_TEXT;
                        }
                	break;
#ifdef RD_SVGT_IN_MESSAGING                 	
                case EMsgComponentIdSvg:
                    if ( MediaIsRunning( iSvgt ) )
                        {
                        resId = R_MMSVIEWER_MSK_BUTTON_STOP_SVG;
                        }
                    else if (   MediaInitializedL( iSvgt ) )
                        {
                        resId = R_MMSVIEWER_MSK_BUTTON_PLAY_SVG;
                        }
                    else
                        {
                        resId = R_MMSVIEWER_MSK_NO_COMMAND_EMPTY_TEXT;
                        }
                	break;                
#endif                
                default: 
                    {
                    
                    if (    iWaitDialog
                        &&  (   iOpeningState == EOpeningMessage
                            ||  iOpeningState == EOpeningSlide ) )
                        {
                        resId = R_MMSVIEWER_MSK_NO_COMMAND_EMPTY_TEXT;
                        }
                    else
                        {                                                    
                        resId = R_MMSVIEWER_MSK_BUTTON_CONTEXT_MENU;
                        if ( resId != iMskId )
                            { 
        		            MenuBar()->SetContextMenuTitleResourceId(
        		                R_MMSVIEWER_SELECTMENU );
                            }
                        }
                    break;
                    }
                }
            }
        }
    else if ( iView ) 
        {
        resId = R_MMSVIEWER_MSK_NO_COMMAND_EMPTY_TEXT;
        }

    if (    resId != iMskId 
        &&  resId != (-1) )
        {
        const TInt KMskPosition = 3;
        CEikButtonGroupContainer* cba = Cba();
        if ( cba )
            {            
            if( resId != 0 )
            	{
    	        cba->SetCommandL( KMskPosition, resId );
            	}
            else
            	{ // nullify the command
    	        cba->SetCommandL( KMskPosition, 0, KNullDesC( ) );
            	}
            cba->DrawDeferred( );
            iMskId = resId;
            }
        }
    return;
    }

// -----------------------------------------------------------------------------
// MediaHasNoDrmRights
// -----------------------------------------------------------------------------
//
TInt CMmsViewerAppUi::MediaHasNoDrmRights(CMsgMediaControl* aMedia)
    {
    TInt result = KErrNone;
    if ( aMedia )
        {
        CUniObject* object = ObjectByMediaControl( *aMedia);
        TBuf<ContentAccess::KMaxCafUniqueId> defaultContent( ContentAccess::KDefaultContentObject( ) );

        TBool ignoreConsumed (EFalse );
        if (    object
            &&  object->DrmInfo( ) )
            {
            result = object->DrmInfo( )->EvaluateRights( ignoreConsumed );
            switch ( result )
                {
                case DRMCommon::EGeneralError:
                case DRMCommon::EUnknownMIME:
                case DRMCommon::EVersionNotSupported:
                case DRMCommon::ESessionError:
                case DRMCommon::ENoRights:
                case DRMCommon::ERightsDBCorrupted:
                case DRMCommon::EUnsupported:
                case DRMCommon::ERightsExpired:
                case DRMCommon::EInvalidRights:
                    break;
                default:
                    result = KErrNone;
                    break;
                }
            }
        }
    return result;
    }

// -----------------------------------------------------------------------------
// handles the touch-ui related control events for next/previous message
// -----------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
void CMmsViewerAppUi::HandleNaviDecoratorEventL( TInt aEventID )     
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        if (    iNaviDecorator
            &&  iNaviPane->Top( ) == iNaviDecorator )
            {
            if( IsNextMessageAvailableL( aEventID == EAknNaviDecoratorEventRightTabArrow ) )
                {
                /* no need for separate checks for right and left arrows
                    because IsNextMessageAvailableL() and NextMessageL
                    are called with the truth-value of the same comparison */
                NextMessageL( aEventID == EAknNaviDecoratorEventRightTabArrow );
                }
            }
        }
    }
#endif // !RD_MSG_NAVIPANE_IMPROVEMENT
#endif // RD_SCALABLE_UI_V2

// -----------------------------------------------------------------------------
// SendReadReportL
// -----------------------------------------------------------------------------
//
void CMmsViewerAppUi::SendReadReportL(  TMmsViewerOperationType     aOperation,
                                        TMmsViewerOperationEvent    aEvent )
    {
    // send read report
    if (    aOperation == EMmsViewerOperationLaunch
        &&  aEvent == EMmsViewerOperationComplete 
        &&  Document( )->LaunchFlags() & EMsgUnreadMessage )
        {
        iSendReadReportOperation = new (ELeave ) CMmsViewerSendReadReportOperation(
                                                    *this,
                                                    *Document( ),
                                                    iCoeEnv->FsSession( ) );
        ActivateInputBlockerL( iSendReadReportOperation );
        iSendReadReportOperation->SendReadReportL( );
        }
    }

// -----------------------------------------------------------------------------
// HandleControlEventL
// -----------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CMmsViewerAppUi::HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType )
#else
void CMmsViewerAppUi::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/)
#endif
    {
#ifdef RD_SCALABLE_UI_V2
    if ( AknLayoutUtils::PenEnabled() )
        {
        if (    iVolumeDecorator
            &&  iNaviPane->Top( ) == iVolumeDecorator 
            &&  aControl == iVolumeDecorator->DecoratedControl()
            &&  aEventType >= MAknNavigationObserver::ENaviEventHandleNavigation
            &&  aEventType <= MAknNavigationObserver::ENaviEventAlreadyRightmostItem )
            {
            iViewerFlags |= EVolumeControlTriggered;
            TRAP_IGNORE( ChangeVolumeL(   
                            static_cast<CAknVolumeControl*>
                                ( iVolumeDecorator->DecoratedControl() )->Value( ), 
                            EFalse ) );
            iViewerFlags &= ( ~EVolumeControlTriggered );
            }
        }

#endif
    }

// -----------------------------------------------------------------------------
// GetNonStoppingWGIdsL
// -----------------------------------------------------------------------------
//
void CMmsViewerAppUi::GetNonStoppingWgIds( )
    {
    iMmsViewerWgId = iCoeEnv->RootWin().Identifier();
    CApaWindowGroupName::FindByAppUid(
        KFastSwapAppUid,
        iCoeEnv->WsSession(),
        iFastSwapWgId );
    CApaWindowGroupName::FindByAppUid(
        KAknNotifyServerAppUid,
        iCoeEnv->WsSession(),
        iAknNotifyServerWgId );
    CApaWindowGroupName::FindByAppUid(
        KScreenSaverAppUid,
        iCoeEnv->WsSession(),
        iScreenSaverWgId );        
    // start receivingn events
    iCoeEnv->RootWin().EnableFocusChangeEvents();
    }

// -----------------------------------------------------------------------------
// ResolveCurrentWgIdL
// -----------------------------------------------------------------------------
//
void CMmsViewerAppUi::ResolveCurrentWgIdL( )
    {
    // magic constant
    const TInt KSystemAppWgId( 4 );
  	TInt windowGroupId = iEikonEnv->WsSession().GetFocusWindowGroup();

/*#ifdef USE_LOGGER
    CApaWindowGroupName* winGroupInfo = CApaWindowGroupName::NewL(iCoeEnv->WsSession() );
    CleanupStack::PushL( winGroupInfo );
    winGroupInfo->ConstructFromWgIdL( windowGroupId );
    TUid uid = winGroupInfo->AppUid( );
    TPtrC caption = winGroupInfo->Caption( );

    LOGTEXT2(_L16("HandleWsEventL EEventFocusLost winGroup: %d"), windowGroupId );
    LOGTEXT2(_L16("HandleWsEventL EEventFocusLost appUid: 0x%x"), uid );
    LOGTEXT2(_L16("HandleWsEventL EEventFocusLost caption: %S"), &caption );

    CleanupStack::PopAndDestroy( winGroupInfo );
#endif*/

    // Stop only if temporary change e.g. a waiting note etc.
    if ( !  (   windowGroupId == iFastSwapWgId 
            ||  windowGroupId == KSystemAppWgId 
            ||  windowGroupId == iAknNotifyServerWgId 
            ||  windowGroupId == iMmsViewerWgId 
            ||  windowGroupId == iScreenSaverWgId ) )
  	    {
        iIsStopPlaybackWgId = ETrue;
  	    }
    }

// ---------------------------------------------------------
// ActivateInputBlockerL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::ActivateInputBlockerL( CActive* aActiveObjectToCancel )
    {
    LOGTEXT(_L8("CMmsViewerAppUi::ActivateInputBlockerL"));
    delete iInputBlocker;
    iInputBlocker = NULL;
    
    iInputBlocker = CAknInputBlock::NewLC();    
    CleanupStack::Pop( iInputBlocker );
    
    if ( aActiveObjectToCancel )
        {
        iInputBlocker->SetCancelActive( aActiveObjectToCancel );
        }
    }
 
// ---------------------------------------------------------
// DeactivateInputBlocker
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DeactivateInputBlocker()
    {
    if ( iInputBlocker )
        {
        LOGTEXT(_L8("CMmsViewerAppUi::DeactivateInputBlockerL"));
        iInputBlocker->SetCancelActive( NULL );    
        delete iInputBlocker;
        iInputBlocker = NULL;
        }
    }

// ---------------------------------------------------------
// ResetOperationState
// ---------------------------------------------------------
//
void CMmsViewerAppUi::ResetOperationState()
    {
    // Operation by operation should be considered what is proper action 
    // in Cancel situation
    DeactivateInputBlocker();

    EndActiveWait();
    delete iWaitDialog;
    iWaitDialog = NULL;
         
    delete iScreenClearer;
    iScreenClearer = NULL;

    // Following takes place also in Cancel event to enable 
    // exit through HandleCommandL()
    iViewerState = ENormal;
    iOpeningState = EOpeningNone;
    iViewerFlags |= ELaunchSuccessful;
    }

// ---------------------------------------------------------
// DoDelayedExit
// ---------------------------------------------------------
//
void CMmsViewerAppUi::DoDelayedExit( TInt aDelayTime )
    {
    LOGTEXT(_L8("CMmsViewerAppUi::DoDelayedExit in ") );
    iIdle->Start( aDelayTime,
                  aDelayTime, 
                  TCallBack( DelayedExitL, this ));
    }

// ---------------------------------------------------------
// DelayedExit
// ---------------------------------------------------------
//
TInt CMmsViewerAppUi::DelayedExitL( TAny* aThis )
    {
    CMmsViewerAppUi* viewer = static_cast<CMmsViewerAppUi*>( aThis );
    viewer->DoMsgSaveExitL();
    return KErrNone;
    }
// ---------------------------------------------------------
// IsVideoCall
// ---------------------------------------------------------
//
 TBool CMmsViewerAppUi::IsVideoCall()
 {
    TInt callType;

    RProperty::Get( KPSUidCtsyCallInformation,
                     KCTsyCallType,
                     callType );// Ignore errors
 
     TBool videoCall = ( callType == EPSCTsyCallTypeH324Multimedia );   
      
     return videoCall;
 }
// ---------------------------------------------------------
// DynInitToolbarL
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CMmsViewerAppUi::DynInitToolbarL(  TInt            /*aResourceId*/, 
                                        CAknToolbar*    /*aToolbar*/ )
    {
    }
#endif        
// ---------------------------------------------------------
// OfferToolbarEventL
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CMmsViewerAppUi::OfferToolbarEventL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EMmsViewerToolbarReply:
            if ( IsOwnMessage( ) )
                {
                HandleCommandL( EMmsViewerReplyToAll );
                }
            else
                {
                HandleCommandL( EMmsViewerReplyToSender );
                }
            
            break;

        case EMmsViewerToolbarForward:
            HandleCommandL( EMmsViewerForward );
            break;

        case EMmsViewerToolbarDelete:
            HandleCommandL( EMmsViewerDelete );
            break;

        default:
            break;
        }
    }

// ---------------------------------------------------------
// SetToolBarItemVisibilities
// ---------------------------------------------------------
//
void CMmsViewerAppUi::SetToolBarItemVisibilities()
    {
    TBool replyDimming( EFalse );
    TBool forwardDimming( EFalse );
    TBool deleteDimming( EFalse );
    
    if ( iToolbar )
        {                
        CEikButtonGroupContainer* cba = Cba();
        if (    cba
            &&  !cba->ControlOrNull( EAknSoftkeyOptions ) )
            {
            // dim every item            
            replyDimming = ETrue;
            forwardDimming = ETrue;
            deleteDimming = ETrue;
            }
        else
            {
            if ( IsOwnMessage( ) )
                {
                if (    !ShowReplyToAll( ) 
                    ||  Document( )->IsUpload( ) )
                    {
                    replyDimming = ETrue;
                    }
                }
            else if ( !HasSender( ) )
                {
                replyDimming = ETrue;
                }
            if ( iSmilModel->SlideCount( ) > iMaxForwardSlideCount )
                {
                forwardDimming = ETrue;
                }
            }
        iToolbar->SetItemDimmed(    EMmsViewerToolbarReply,
                                    replyDimming,
                                    ETrue );
        iToolbar->SetItemDimmed(    EMmsViewerToolbarForward,
                                    forwardDimming,
                                    ETrue );
        iToolbar->SetItemDimmed(    EMmsViewerToolbarDelete,
                                    deleteDimming,
                                    ETrue );
        }
    }
#endif

// ---------------------------------------------------------
// AreControlsReadyAndWaitNoteDismissedL
// ---------------------------------------------------------
//
TBool CMmsViewerAppUi::AreControlsReadyAndWaitNoteDismissedL( )
    {
    if (    
            MediaInitializedL( iImage ) 
        &&  MediaInitializedL( iAudio ) 
        &&  MediaInitializedL( iVideo )
#ifdef RD_SVGT_IN_MESSAGING
        &&  MediaInitializedL( iSvgt )        
#endif        
        &&  !iWaitDialog )
        {
        return ETrue;
        }
    return EFalse;
    }


// ---------------------------------------------------------
// IsRightsConsumed
// ---------------------------------------------------------
//
TBool CMmsViewerAppUi::IsRightsConsumed( CUniObject& aObject ) const
    {
    TBool consumed( EFalse );
    if (  aObject.DrmInfo( ) )
        {
        aObject.DrmInfo( )->EvaluateRights( consumed );
        }
    LOGTEXT2(_L16("CMmsViewerAppUi::IsRightsConsumed consumed %d"), consumed );
    return consumed;
    }

// ---------------------------------------------------------
// IsRightsNow
// ---------------------------------------------------------
//
TBool CMmsViewerAppUi::IsRightsNow( CUniObject& aObject ) const
    {
    TBool rightsNow( EFalse );
    if (  aObject.DrmInfo( ) )
        {
        TInt consumed( EFalse );
        // KErrNone: rights exist
        rightsNow = !aObject.DrmInfo( )->EvaluateRights( consumed );

        /*TBool rightsNow2( EFalse );
        rightsNow2 = aObject.DrmInfo( )->RightsValidL( ETrue );
        TInt j;
        j++;*/
        }
    LOGTEXT2(_L16("CMmsViewerAppUi::IsRightsNow rightsNow %d"), rightsNow );


    return rightsNow;
    }
    
// ---------------------------------------------------------
// IsDrmRightsWhenLoaded
// ---------------------------------------------------------
//
TBool CMmsViewerAppUi::IsDrmRightsWhenLoaded( CUniObject& aObject ) const
    {
    // Separate function just because of LOG traces
    TBool rightsWhenLoaded = aObject.IsDrmRightsWhenLoaded();
    LOGTEXT2(_L16("CMmsViewerAppUi::IsDrmRightsWhenLoaded rightsWhenLoaded %d"), rightsWhenLoaded );
    return rightsWhenLoaded;
    }
           
// ---------------------------------------------------------
// HandleUniObjectEvent from MUniObjectObserver
// ---------------------------------------------------------
//
void  CMmsViewerAppUi::HandleUniObjectEvent(    CUniObject&    aUniObject,
                                                TUniObjectEvent aUniEvent )
    {
    LOGTEXT(_L8("CMmsViewerAppUi::HandleUniObjectEvent in ") );
    // SVG is different. Drm rights are not consumed by us at all but external viewer
    CMsgMediaControl* control = MediaControlByObject( aUniObject );
    if (        aUniEvent == EUniObjectEventDrmRightsChanged
            &&  control
            &&  !(   control == iVideo 
                &&  iViewerFlags & EInObjectsView )
            &&  aUniObject.DrmInfo( )
            &&  (   
                    (   IsDrmRightsWhenLoaded( aUniObject )
                    &&  !IsRightsConsumed( aUniObject )
                    &&  !IsRightsNow( aUniObject ) )
                ||  (   !IsDrmRightsWhenLoaded( aUniObject )
                    &&  !IsRightsConsumed( aUniObject )
                    &&  IsRightsNow( aUniObject ) )
                ) 
        )
        {
#ifdef RD_SVGT_IN_MESSAGING
        if ( control == iSvgt )
            {
            // SVG is different. DRM rights are not consumed ever by us but thumbnail may be visible
            // Thus, if SVG thumbnail becomes ever visible, don't remove it
            if (    IsDrmRightsWhenLoaded( aUniObject )
                &&  !IsRightsConsumed( aUniObject )
                &&  !IsRightsNow( aUniObject ) )
                {
                return;
                }
            }
#endif

        //          - Rights were initially when slide was loaded 
        //      &&  - they were not consumed by us
        //      &&  - they were consumed by someone else
        // or       - Rights were not initially when slide was loaded 
        //      &&  - they are not consumed by us
        //      &&  - now there are rights
        if (    !iViewerFlags & EInObjectsView
            &&  IsDisplayingDialog() )
            {
            TKeyEvent keyEvent;
            keyEvent.iCode = EKeyEscape;
            keyEvent.iScanCode = EStdKeyEscape;
            keyEvent.iModifiers = 0;
            keyEvent.iRepeats = 0;
            TRAP_IGNORE( iEikonEnv->SimulateKeyEventL(keyEvent,EEventKey) );
            }
        TRAP_IGNORE( ReloadControlL( control, aUniObject ) );
        }
    LOGTEXT(_L8("CMmsViewerAppUi::HandleUniObjectEvent out ") );
    }

// ---------------------------------------------------------
// SimulateUniObjectEventL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::SimulateUniObjectEventL( )
    {
    iIdle->Cancel();
    TInt j = 4;
    CMsgMediaControl* control = NULL;
    if (    j == 1
        &&  iImage )
        control = iImage;
    if (    j == 2
        &&  iAudio )
        control = iAudio;
    if (    j == 3
        &&  iVideo )
        control = iVideo;
#ifdef RD_SVGT_IN_MESSAGING              
    if (    j == 4
        &&  iSvgt )
        control = iSvgt;
#endif        
    if ( control )
        {
        CUniObject* uniObject = ObjectByBaseControl( *control );
        if ( uniObject )
            {
            ReloadControlL( control, *uniObject );
            }
        }
    }


// ---------------------------------------------------------
// MediaControlByObject
// ---------------------------------------------------------
CMsgMediaControl* CMmsViewerAppUi::MediaControlByObject( CUniObject&    aUniObject )
    {    
    CMsgMediaControl* control = NULL;
    switch ( aUniObject.MediaType( ) )
        {
        case EMsgMediaImage:
            control = iImage;
            break;
        case EMsgMediaAudio:
            control = iAudio;
            break;
        case EMsgMediaVideo:
            control = iVideo;
            break;
#ifdef RD_SVGT_IN_MESSAGING
        case EMsgMediaSvg:
            control = iSvgt;
            break;
#endif
        default:
            break;
        }
    return control;
    
    }


// ---------------------------------------------------------
// SetTitleIconL
// ---------------------------------------------------------
//
void CMmsViewerAppUi::SetTitleIconL()
    {
    SetTitleIconSizeL( iAppIcon->Bitmap() );
    // Create duplicates of the icon to be used
    CFbsBitmap* bitmap = new( ELeave ) CFbsBitmap();
    CleanupStack::PushL( bitmap );
    
    CFbsBitmap* bitmapMask = new( ELeave ) CFbsBitmap();
    CleanupStack::PushL( bitmapMask );
    
    User::LeaveIfError( bitmap->Duplicate( iAppIcon->Bitmap()->Handle() ) );
    User::LeaveIfError( bitmapMask->Duplicate( iAppIcon->Mask()->Handle() ) );

    iTitlePane->SetSmallPicture( bitmap, bitmapMask, ETrue );
    iTitlePane->DrawNow();
    
    CleanupStack::Pop( bitmapMask );
    CleanupStack::Pop( bitmap );
    }
// ---------------------------------------------------------
// CMmsViewerAppUi::HandleLongTapEventL
//  Function for handling the long tap events
// ---------------------------------------------------------
//
void CMmsViewerAppUi::HandleLongTapEventL(const TPoint& aPenEventLocation, 
                                          const TPoint& aPenEventScreenLocation )
    {
    CMsgBaseControl* ctrl = iView->FocusedControl(); // ctrl can be NULL.
    if ( ctrl && 
         ( ctrl->ControlId() == EMsgComponentIdAudio ||
                 ctrl->ControlId() == EMsgComponentIdImage ||
                 ctrl->ControlId() == EMsgComponentIdVideo ||
                 ctrl->ControlId() == EMsgComponentIdSvg )&&
                 ((Document()->SmilType()!=ETemplateSmil)&&(Document()->SmilType()!=E3GPPSmil) )) 
        {
        TRect rect = ctrl->Rect();
       
        
     
        if ((!iTapConsumed)&&rect.Contains(aPenEventLocation))
            {
            if (iEmbeddedObjectStylusPopup)
                {
                delete iEmbeddedObjectStylusPopup;
                iEmbeddedObjectStylusPopup = NULL;
                }
            iEmbeddedObjectStylusPopup = CAknStylusPopUpMenu::NewL(this,aPenEventLocation);
            TResourceReader reader;
            iCoeEnv->CreateResourceReaderLC(reader,R_MMSVIEWER_EMBEDDED_OBJECT_STYLUS_MENU );
            iEmbeddedObjectStylusPopup->ConstructFromResourceL(reader);
            CleanupStack::PopAndDestroy();
            iEmbeddedObjectStylusPopup->SetPosition(aPenEventLocation);
            iEmbeddedObjectStylusPopup->ShowMenu();
            iTapConsumed = ETrue;
            }
        }   
    } 
// ---------------------------------------------------------
// CMmsViewerAppUi::HandleSelectionKeyL
//  Function for handling the selection key
// ---------------------------------------------------------
void CMmsViewerAppUi::HandleSelectionKeyL()
    {
    // Read current MSK resource to get a command id
    // to execute.
    TResourceReader reader;
    iEikonEnv->CreateResourceReaderLC(
        reader, iMskId );
    reader.ReadInt8(); // version
    HandleCommandL( reader.ReadUint16() );
    CleanupStack::PopAndDestroy(); // resource buffer
    }
//  End of File

