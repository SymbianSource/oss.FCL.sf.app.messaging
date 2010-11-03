/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides AudioMessage App UI methods.
*
*/



#include <apgwgnam.h>
#include <eikrted.h>
#include <data_caging_path_literals.hrh>
 
#include <akntabgrp.h>
#include <aknEditStateIndicator.h>
#include <akntitle.h>
#include <aknnotewrappers.h>
#include <AknWaitDialog.h>
#include <AknsUtils.h>
#include <akncontext.h>
#include <AknTaskList.h>
#include <AknGlobalNote.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <AknIndicatorContainer.h>
#include <AknIconArray.h>           // CAknIconArray
#include <aknlists.h>
#include <AknStatuspaneUtils.h>     // AknStatuspaneUtils
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <MuiuMessageIterator.h>
#ifdef RD_SCALABLE_UI_V2
#include <akntoolbar.h> 
#include <eikcolib.h>
#endif
    
#include <aknclearer.h>
#include <e32property.h>  
#include <ctsydomainpskeys.h>

// Common components
#include <commonphoneparser.h> //Common phone number validity checker
#include <PhCltTypes.h>                 // PhCltTypes
#include <CommonUiInternalCRKeys.h>
#include <DocumentHandler.h>
#include <featmgr.h>
#include <finditemmenu.h>
#include <finditem.hrh> //Place holder
#include <ItemFinder.h>
#include <StringLoader.h>  
#include <messagingvariant.hrh>

// Core UI
#include <CoreApplicationUIsSDKCRKeys.h>

// Help
#include <hlplch.h>
#include <csxhelp/msg.hlp.hrh>

// Profiles
#include <Profile.hrh>
#include <ProfileEngineSDKCRKeys.h>

// Messaging
#include <mmsvattachmentmanager.h>
#include <mtmuibas.h> 
#include <MtmExtendedCapabilities.hrh>

// MsgCommonUtils
#include <msgvoipextension.h>

// MUIU
#include <MuiuOperationWait.h>
#include <akninputblock.h>
#include <MuiuMsvUiServiceUtilities.h>
#include <muiumsvuiserviceutilitiesinternal.h>
#include <MsgFolderSelectionDialog.h> //Folder Selection dialog, DoMoveMessageL
#include <messaginginternalcrkeys.h>  //Inputmode, speaker setting
#include <MuiuMsgEditorLauncher.h>

// MsgEditor
#include <MsgCheckNames.h>
#include <MsgAttachmentUtils.h>
#include <MsgEditorAppUiExtension.h>// for iMsgEditorAppUiExtension

// Send UI
#include <sendui.h> 
#include <SenduiMtmUids.h> 
#include <CMessageData.h> //DoReplyViaL

// MMS Engine
#include <mmsgenutils.h> //PureAddress
#include <mmsclient.h>
#include <MsgMimeTypes.h>   
#include <MsgMediaResolver.h>
#include <MsgMediaInfo.h>
#include <msgmediacontrol.h>
#include <uniaddresshandler.h>

#include "AudioMessageLogging.h"
#include "audiomessageappui.h"
#include "audiomessageinsertoperation.h"
#include "audiomessagelaunchoperation.h"
#include "audiomessagesaveoperation.h"
#include "audiomessagesendoperation.h"
#include "audiomessagesendreadreportoperation.h"
#include "audiomessageprogressdialog.h"
#include <audiomessage.mbg>   
#include <audiomessage.rsg> 

#define KFSWApUid TUid::Uid( 0x10207218 )
#define KAknNfySrvUid TUid::Uid(0x10281EF2 )

#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
const TUint KNaviGranularity = 2;
#endif
const TMsvId KNewMessageFolder = KMsvDraftEntryIdValue;
const TSize KPriorityIconSize = TSize( 10,16 ); // width, height
const TInt  KDelayedExitDelay = 1500000;
const TUid KUidAudioMessageApplication = { 0x1020745A };
const TInt KAmsNotCaptured = -1;
const TInt KDefaultVolume = 5;
const TInt KAmsBytesInKilo = 1024;
const TInt KMaxAliasLength = 64;

_LIT( KAmrFileExt,".amr");
//#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
_LIT( KAmsBitmapFileName, "audiomessage.mif" );
//#endif

// ======== MEMBER FUNCTIONS ========
 

// ---------------------------------------------------------
// CAudioMessageAppUi::Constructor
// ---------------------------------------------------------
//
CAudioMessageAppUi::CAudioMessageAppUi()
    :
    iWaitResId( -1 ),
    iEditorFlags( 0 ),
    iProgressDlg( NULL ),
    iAlias ( NULL ),
    iCurrentVolume( KDefaultVolume ),
    iVoiceKeyHandle( KAmsNotCaptured ),
   	iSpeakerEarpiece( ETrue ),
    iInserting( EFalse ),
    iCurrentSkResId ( 0 )
    {
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::ConstructL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::ConstructL()
    {
AMSLOGGER_WRITE( "CAudioMessageAppUi::ConstructL" );
    if ( iEikonEnv->EikAppUi() )
        {
        iAbsorber = CAknInputBlock::NewLC();
        CleanupStack::Pop( iAbsorber );
        }

    CMsgEditorAppUi::ConstructL();
 
    // Get supported features from feature manager.
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        iSupportedFeatures |= EUniFeatureHelp;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdOfflineMode ) )
        {   
        iSupportedFeatures |= EUniFeatureOffline;      
        }        
    FeatureManager::UnInitializeLib();
    iMsgVoIPExtension = CMsgVoIPExtension::NewL();
    
    iSettingsRepository = CRepository::NewL( KCRUidMuiuSettings );
 
    TInt highlight = 0;
    // Automatic highlight setting storage and change notifications
    iCUiRepository = CRepository::NewL( KCRUidCommonUi );
    iCUiRepository->Get( KCuiAutomaticHighlight, highlight  );
    iNotifyHandler = CCenRepNotifyHandler::NewL( *this,
                                                 *iCUiRepository,
                                                 CCenRepNotifyHandler::EIntKey,
                                                 KCuiAutomaticHighlight );
    
    TInt featureBitmask = 0;
    CRepository* muiuRepository = CRepository::NewL( KCRUidMuiuVariation );
    muiuRepository->Get( KMuiuMmsFeatures, featureBitmask );

    if ( featureBitmask & KMmsFeatureIdPrioritySupport )
       {          
       iSupportedFeatures |= EUniFeaturePriority;     
       }
    if ( featureBitmask & KMmsFeatureIdDeliveryStatusDialog ) 
        {         
        iSupportedFeatures |= EUniFeatureDeliveryStatus;
        }


    delete muiuRepository;
    
    //Read speaker setting ( earpiece / loudspeaker )
    iAudioMesseageRepository = CRepository::NewL( KCRUidAudioMsg );
    iAudioMesseageRepository -> Get( KAudioMsgSpeaker, iSpeakerEarpiece );

    iNotifyHandler->StartListeningL();
    // Set up highlighting
    if ( highlight == 0 )
        {
        iEditorFlags &= ~EAutohighLightEnabled;
        }
    else
        {
        iEditorFlags |= EAutohighLightEnabled;
        }
        
        
    // Disable task swapper from options menu during launch
    MenuBar()->SetMenuType( CEikMenuBar::EMenuOptionsNoTaskSwapper );
        

    iTitlePane = static_cast<CAknTitlePane*>
        ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    if ( !iEikonEnv->StartedAsServerApp( ) )
        {
        Document()->PrepareToLaunchL( this );
        }

    Document()->SetEditorModelObserver( this );
   	iClipFileName = StringLoader::LoadL( R_QTN_VOREC_FIRST_MEMO_NAME );
   	iTimeDuratBase = StringLoader::LoadL( R_QTN_TIME_DURAT_MIN_SEC );


    CEikStatusPane* sp = StatusPane();
    CAknContextPane* contextPane = static_cast< CAknContextPane* >
       ( sp->ControlL( TUid::Uid( EEikStatusPaneUidContext ) ) );

      // Audiomsg window group id
    iMyWgId = iCoeEnv->RootWin().Identifier();
    
    CApaWindowGroupName::FindByAppUid(
        KFSWApUid,
        iCoeEnv->WsSession(),
        iFSWindowGroupId );
        
    CApaWindowGroupName::FindByAppUid(
        KAknNfySrvUid,
        iCoeEnv->WsSession(),
        iAknNfySrvUi );    

  	// Set path of bitmap file
  	TParse fileParse;
  	fileParse.Set( KAmsBitmapFileName, &KDC_APP_BITMAP_DIR, NULL );
    
    iAppIcon = AknsUtils::CreateGulIconL( 
        AknsUtils::SkinInstance(), 
        KAknsIIDQgnPropMceAudioTitle,
        fileParse.FullName(),
        EMbmAudiomessageQgn_prop_mce_audio_title,
        EMbmAudiomessageQgn_prop_mce_audio_title_mask );

    iCoeEnv->RootWin().EnableFocusChangeEvents();   // ignore error
    
    iScreenClearer = CAknLocalScreenClearer::NewLC( ETrue );
    CleanupStack::Pop( iScreenClearer );    
AMSLOGGER_WRITE( "CAudioMessageAppUi::ConstructL ends" );    	
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::Destructor
// ---------------------------------------------------------
//
CAudioMessageAppUi::~CAudioMessageAppUi()
    {
AMSLOGGER_WRITE( "CAudioMessageAppUi::~CAudioMessageAppUi" );    
    
    
    iEditorFlags |= EEditorExiting;
    //Write speaker setting ( earpiece / loudspeaker )
    if ( iAudioMesseageRepository)
    	{	
        iAudioMesseageRepository -> Set( KAudioMsgSpeaker, iSpeakerEarpiece );
        delete iAudioMesseageRepository;
    	}
    // Release voice key
    if ( iVoiceKeyHandle >= 0 )
        {
        iCoeEnv->RootWin().CancelCaptureKey( iVoiceKeyHandle );
        iVoiceKeyHandle = KAmsNotCaptured;
        }

    delete iAddressHandler;
  
  
    delete iAppIcon;
  
    // remember input mode:
    if ( iEditorFlags & ELaunchSuccessful )
        {
        TInt id( EMsgComponentIdNull );
        CMsgBaseControl* ctrl = iView->FocusedControl();  // ctrl can be NULL.
        if ( ctrl )
            {
            id = ctrl->ControlId();
            }
        if ( id == EMsgComponentIdNull )
            {
            id = EMsgComponentIdTo;
            }
        if ( id == EMsgComponentIdTo )
            {
            CMsgAddressControl* rec = iView ?
                static_cast< CMsgAddressControl* >( iView->ControlById( id ) )
                : NULL;
            if ( rec )
                {
                TInt inputMode = rec->Editor().AknEditorCurrentInputMode();
                iSettingsRepository->Set( KMuiuToInputMode, inputMode );
                }
            }
        }

    delete iSettingsRepository;
    delete iLaunchOperation;
    delete iSaveOperation;
    delete iSendOperation;
    delete iInsertOperation;
    if ( iWaitDialog )
    	{
		// iWaitDialog->ProcessFinishedL is for
		// cancellation\closing  of wait dialog is asynchronous operation. it never gets so
		// far that dialog would be really deleted before the application terminates.
		// so to avoid memory leak , deleting the iWaitDialog.
    	iWaitDialog->SetCallback( NULL );
    	TRAP_IGNORE( iWaitDialog->ProcessFinishedL() );
    	delete iWaitDialog;
		iWaitDialog = NULL;
    	}    
    delete iIdle;
    delete iAbsorber;
    delete iFindItemMenu;
    delete iNotifyHandler;
    delete iCUiRepository;
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT    
    delete iNavi_next_prev_msg_arrows;
#endif    
#ifdef RD_SCALABLE_UI_V2
    if ( iToolbar )
        {
        delete iToolbar;    
        }
#endif
    delete iMsgVoIPExtension;
    delete iClipFileName;
    delete iTimeDuratBase;
    delete iInsertingMedia;
    delete iAlias;	
    delete iScreenClearer;	
    delete iSendReadReportOperation;
AMSLOGGER_WRITE( "CAudioMessageAppUi::~CAudioMessageAppUi ends" );       
  }

// ---------------------------------------------------------
// CAudioMessageAppUi::LaunchViewL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::LaunchViewL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::LaunchViewL >>" ); 
    if ( iEditorFlags & EEditorExiting )
        {
        // Start the delayed exit here if exit command was given before application was
        // ready to handle it. Delayed exit is used as LauchViewL cannot 
        // make proper exit if Exit() is called here.
        //DoDelayedExitL( 0 );
        return;
        }
    Document()->SetClipStatus( EAmsClipNone );
    iMtm = &( Document()->Mtm() );
    const TMsvEntry& entry = Document()->Entry();
    if ( entry.iType.iUid != KUidMsvMessageEntryValue )
        {
        User::Leave( KErrGeneral );
        }

    TRAPD( error, iMtm->LoadMessageL() );
    if ( error )
        {
        if ( error != KErrNoMemory )
            {
            ShowErrorNoteL( R_AUDIOMESSAGE_ERROR_MSG_CORRUPT, ETrue );
            }
        User::Leave( error );
        }

    delete iAbsorber;
    iAbsorber = NULL;

    Document()->SetMessageType();

  	if ( !iMtm->Entry().Entry().ReadOnly() ) // editor
    	{
    	//SendKey disabled
    	iAvkonAppUi->SetKeyEventFlags( 
            CAknAppUiBase::EDisableSendKeyShort | 
            CAknAppUiBase::EDisableSendKeyLong ); 
               
    	SetFixedToolbarL( R_AMSEDITOR_APP_TOOLBAR );
    	iView = CMsgEditorView::NewL( *this,
        CMsgEditorView::EMsgDoNotUseDefaultBodyControl );

      	// Add TO-control to editor
      	iView->AddControlFromResourceL(
          	R_AUDIOMESSAGE_TO,
          	EMsgAddressControl,
          	EMsgAppendControl,
          	EMsgHeader );

	    // If editor opened from Phonebook or Logs details must be checked
	    // so we can add address/alias to Recipient -field
	    const TPtrC details = iMtm->Entry().Entry().iDetails;

	  	// Message in draft folder OR opening with data in address -field
	    if ( entry.iMtmData1 & KMmsMessageEditorOriented
	         || details.Length() 
	         || iMtm->Entry().Entry().Parent() == KMsvDraftEntryIdValue )
	    	{
	      	// Set name/number
	      	InsertRecipientL();
	    	}
	    // Restore AddressControl's input mode
	    CMsgAddressControl* to = ToCtrl();
	    TInt inputMode = EAknEditorNumericInputMode;
	  	if ( iSettingsRepository )
	        {
	        iSettingsRepository->Get( KMuiuToInputMode, inputMode );
	    	}
	    to->Editor().SetAknEditorInputMode( inputMode );
	    Document()->SetAppMode( EAmsEditor );   	
	   	InitEditorNaviPaneL();

	   	//End key should not close application in editor
	   	CMsgEditorAppUi::SetCloseWithEndKey( EFalse);
	   	}
	else // viewer
    	{
    	SetFixedToolbarL( R_AMSVIEWER_APP_TOOLBAR );
    	iView = CMsgEditorView::NewL( *this,
        	CMsgEditorView::EMsgReadOnly |
        	CMsgEditorView::EMsgDoNotUseDefaultBodyControl );

      	Document()->SetAppMode( EAmsViewer );
      	// Message in sent folder or moved from sent to documents	
        if ( entry.iMtmData1 & KMmsMessageEditorOriented  
             || entry.Parent() == KMsvSentEntryIdValue )
      		{
      		// Add TO -control to viewer and set name/number
      		InsertRecipientL();
      		}
    	else // Message in inbox
	      	{
	      	// Add FROM -control to viewer and set name/number
	      	SetSenderL();
	      	// Add SUBJECT -control to viewer and set subject
	      	SetSubjectL();
	      	}
	    Document()->SetInsertedStatus( EAmsInsideMessage );  	
    	InitViewerNaviPaneL();
    	}
    

    iView->SetEdwinObserverL( this );
    iLaunchOperation = CAudioMessageLaunchOperation::NewL(
        *this,
        *Document(),
        *iView );
    iLaunchOperation->Launch();
 
   AMSLOGGER_WRITE( "CAudioMessageAppUi::LaunchViewL <<" ); 
    }


// ---------------------------------------------------------
// CAudioMessageAppUi::FinalizeLaunchL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::FinalizeLaunchL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::FinalizeLaunchL >>" ); 
    if ( iEditorFlags & EEditorExiting )
        {
        // Start the delayed exit here if exit command was given before application was
        // ready to handle it. Delayed exit is used as LauchViewL cannot 
        // make proper exit if Exit() is called here.
        //DoDelayedExitL( 0 );
        return;
        }
    
    SetAddressSize();
    
    if ( Document()->GetClipStatus() == EAmsClipAttachment )
      	{
      	FetchAudioFileDataL() ;
      	}

    if ( !iMtm->Entry().Entry().ReadOnly() )//editor
      	{
      	iView->ExecuteL( ClientRect(), EMsgComponentIdImage );
      	if ( Document()->GetInsertedStatus() == EAmsSmilAdded ) // from gallery and smil added
      		{
      	    iView->SetFocus( EMsgComponentIdTo );
      	    ShowInformationNoteL( R_AUDIOMESSAGE_INSERTED, EFalse );
      		}
      	else
      	    {
      	    iView->SetFocus( EMsgComponentIdImage );
      	    }	
        }
    else //viewer
      	{
      	iView->ExecuteL( ClientRect(), EMsgComponentIdNull );
      	iView->SetFocus( EMsgComponentIdImage );
      	SetFindMode( iEditorFlags & EAutohighLightEnabled ) ;
        SetViewerToolBarItemVisibilities();
        }
    SetTitleIconL();
    AMSLOGGER_WRITE( "CAudioMessageAppUi::FinalizeLaunchL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::HandleCommandL
// Error: EGWG-7HWE2P : Purushotham Reddy K
// ---------------------------------------------------------
//
void CAudioMessageAppUi::HandleCommandL( TInt aCommand )
    {
    TRAPD( error, DoHandleCommandL( aCommand ) );
    if ( error != KErrNone )    
        {
        AMSLOGGER_WRITEF( _L("iHandleCommandL: LEAVE (%d) "), error ); 
        iEditorFlags &= ~EProgressInUse;
        delete iAbsorber;
        iAbsorber = NULL;
        
        //See the code where the following has been set EFalse
        iAvkonAppUi->Cba()->MakeVisible( ETrue );
        iInserting = EFalse;
        iView->MakeVisible( ETrue );
        //EGWG-7HWE2P
        if(error == KErrInUse)
            {
            //If this error is due to record is already in use in another message editor,
            //then de-highlight the record button.
            iToolbar->SetItemDimmed(EAmsEditorToolbarRecord, EFalse, ETrue);
            }
        User::Leave( error );
        }
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::SetFixedToolbarL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::SetFixedToolbarL( const TInt aResId )
    {
#ifdef RD_SCALABLE_UI_V2
    if ( AknLayoutUtils::PenEnabled() && !iToolbar)
    	{
    	CEikAppUiFactory* appUiFactory 
    	    = static_cast<CEikAppUiFactory*>( iEikonEnv->AppUiFactory() );
        CAknToolbar* oldFixedToolbar = appUiFactory->CurrentFixedToolbar();
        iToolbar = CAknToolbar::NewL( aResId );
        iToolbar->SetToolbarObserver( this );
        
        if ( oldFixedToolbar != iToolbar )
            {
            oldFixedToolbar->SetToolbarVisibility( EFalse );    
            }
        appUiFactory->SetViewFixedToolbar( iToolbar );    	
        iToolbar->SetToolbarVisibility( ETrue, EFalse ); 
    	}
    else if ( AknLayoutUtils::PenEnabled() && iToolbar )
        {
        CEikAppUiFactory* appUiFactory = static_cast<CEikAppUiFactory*>( iEikonEnv->AppUiFactory() );
        CAknToolbar* oldFixedToolbar = appUiFactory->CurrentFixedToolbar();
        if ( oldFixedToolbar != iToolbar )
            {
            oldFixedToolbar->SetToolbarVisibility( EFalse );
            appUiFactory->SetViewFixedToolbar( iToolbar );     
            }
        }

    #endif
    return;
    }


// ---------------------------------------------------------
// CAudioMessageAppUi::HandleCommandL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoHandleCommandL(TInt aCommand)
    {
AMSLOGGER_WRITE( "CAudioMessageAppUi::HandleCommandL >>" ); 
    if ( iEditorFlags & EEditorExiting )
        {
        // Do not handle any event if we are exiting.
        return;
        }
    
    if ( !( iEditorFlags & ELaunchSuccessful ) )
        {
        if ( aCommand == EEikCmdExit )
            {
            if ( !Document()->IsLaunched() )
                {
                //We come here e.g. when editor is started in very low memory
                iEditorFlags |= EEditorExiting;
                DoDelayedExitL( 0 );
                return;
                }   
            }
        else
            {
            // Do not handle other than exit command if application has not
            // finished launching.
            return;
            }    
        }

  	if ( iFindItemMenu && iFindItemMenu->CommandIsValidL( aCommand ) )
    	{
    	iFindItemMenu->HandleItemFinderCommandL( aCommand );
    	return;
    	}
    switch ( aCommand )
        {
        case EAmsEditorCmdSend:
            DoUserSendL();
            break;
        case EAmsEditorCmdAddRecipient:
            DoUserAddRecipientL();
          	break;
        case EAmsViewerCmdSaveAudio:
        case EAmsEditorCmdSaveAudio:
          	SaveAudioClipL();
          	break;
        case EAmsEditorCmdPlayAudio:
            PlayAudioL();
            break;
        case EAmsViewerCmdForward:
          	DoForwardL();
          	break;
        case EAmsViewerCmdRemoveAudio:
          	DeleteMessageL();
          	break;
        case EAmsEditorCmdMessageInfo:
            DoUserMessageInfoL();
            break;
        case EAmsViewerCmdDeliveryStatus:
            OpenDeliveryPopupL();
            break;
        case EAmsViewerCmdMoveToFolder:
            DoMoveMessageL();
            break;
        case EAknCmdHelp:
            LaunchHelpL();
            break;
    	case EAknCmdExit:
        case EEikCmdExit:
            ExitAndSaveL( );
            break;
        // Reply submenu commands
        case EAmsViewerCmdReplyToSender:
          	DoReplyViaL( EReplyToSender );
          	break;
        case EAmsViewerCmdReplyToAll:
          	DoReplyViaL( EReplyToAll );
          	break;       
        case EAmsViewerCmdReplyViaMessage:
          	DoReplyViaL( EReplyViaMessage );
          	break;          	
        case EAmsEditorCmdRecordAudio:
           	InsertAudioL( ETrue );
            break;
        case EAmsEditorCmdFromGallery:
            InsertAudioL( EFalse );
            break;
        case EAmsEditorCmdReRecordAudio:
            InsertAudioL( ETrue );
            break;
        case EAmsEditorCmdFromGallery2:
            InsertAudioL( EFalse );
          	break;
        // Button commands
        case EAknSoftkeyClose:
            DoBackSaveL();
            break;
        case EAmsSoftkeyAdd:
            // Call this function so that check name functionality is called first
        	DoSelectionKeyL( );
        	break;
        case EAmsSoftkeyPlay:
        	PlayAudioL();
        	break;
        case EAmsSoftkeyRecord:
        	InsertAudioL( ETrue );
        	break;
        case EAmsEditorCmdChangePriority:
            DoEditAmsPriorityL( );
            break;
        case EAmsDispSizeAutomatic:
        case EAmsDispSizeLarge:
        case EAmsDispSizeNormal:
        case EAmsDispSizeSmall:
            DoHandleLocalZoomChangeL( aCommand );
            break;        	
        default:
            CMsgEditorAppUi::HandleCommandL( aCommand );
            break;
        }
    AMSLOGGER_WRITE( "CAudioMessageAppUi::HandleCommandL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::EditorObserver
// ---------------------------------------------------------
//
void CAudioMessageAppUi::EditorObserver( TMsgEditorObserverFunc aFunc, TAny* aArg1,
  TAny* aArg2, TAny* aArg3 )
    {
    TRAP_IGNORE( DoEditorObserverL( aFunc,aArg1,aArg2,aArg3 ) );
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::DoEditorObserverL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoEditorObserverL( TMsgEditorObserverFunc aFunc, TAny* aArg1,
  TAny* aArg2, TAny*/* aArg3*/ )
    {
    
    switch ( aFunc )
        {
        case MMsgEditorObserver::EMsgHandleFocusChange:   
	        {
AMSLOGGER_WRITE( "DoEditorObserverL: EMsgHandleFocusChange" ); 	        
	        TMsgFocusEvent event = *( static_cast<TMsgFocusEvent*>( aArg1 ) );  
					// Handling of SendKey in Viewer mode
					if ( Document()->GetAppMode() == EAmsViewer )
						{
						if((event == EMsgFocusToHeader) && (iSenderType == ESenderTypePhoneNumber))
							{
							//disable dialer
							iAvkonAppUi->SetKeyEventFlags( 
			    				CAknAppUiBase::EDisableSendKeyShort | 
			    				CAknAppUiBase::EDisableSendKeyLong ); 	
							}
							else if(event == EMsgFocusToBody)
							{
							//enable dialer
							iAvkonAppUi->SetKeyEventFlags( 0x00); 
							}	
						}
	        if( event == EMsgFocusMovedTo )
                {
                SetNaviPaneL( );
                UpdateSoftKeysL(); 
                }
	        }
	        break;
	    
	    
#ifdef RD_SCALABLE_UI_V2	    
	    case MMsgEditorObserver::EMsgControlPointerEvent:
            {
            TPointerEvent* ptrEvent = static_cast<TPointerEvent*>( aArg2 );    
            if ( Document()->GetAppMode() == EAmsViewer )
            	{
            	CMsgBaseControl* control = static_cast<CMsgBaseControl*>( aArg1 );
            	if(control)
	            	{
	            	TInt id = control->ControlId();	            
								if(id == EMsgComponentIdFrom && iSenderType == ESenderTypePhoneNumber)
									{     
									//disable dialer
									iAvkonAppUi->SetKeyEventFlags( 
				    					CAknAppUiBase::EDisableSendKeyShort | 
				    					CAknAppUiBase::EDisableSendKeyLong ); 								
									}
								else            
									{
	                	//enable dialer
									iAvkonAppUi->SetKeyEventFlags( 0x00);
									}
	            	}
            	}           
     	    if ( ptrEvent->iType == TPointerEvent::EButton1Down )
    	        {
        	        TPoint point = ptrEvent->iPosition;
        	        
        	        TRect bodyRect = iView->FormComponent( EMsgBody ).Rect();
        	        TRect focusedRect = iView->FocusedControl()->Rect();
        	        //the bodyrect was already focused i.e not the first press
        	        if ( bodyRect == focusedRect )
        	            {
        	            TSize frameSize = iLaunchOperation->FrameSize();
                        bodyRect.Shrink((bodyRect.Width()-frameSize.iWidth)/2,
                            (bodyRect.Height()-frameSize.iHeight)/2);
                        
            	        if ( bodyRect.Contains( point ) )
                            {//the image is clicked - the nth time
                   	        if ( Document()->GetClipStatus() != EAmsClipNone )
                			    {
                			    HandleCommandL( EAmsEditorCmdPlayAudio );
                			    }
                    		//there is no any clip, so try to record new.
                	    	else
                	    		{
                	    		HandleCommandL(EAmsEditorCmdRecordAudio);
                            	}
                            }
        	            }
    	        }
                
            break;
            }
 #endif //RD_SCALABLE_UI_V2           
  
	    case MMsgEditorObserver::EMsgButtonEvent:
            {
AMSLOGGER_WRITE( "DoEditorObserverL: EMsgButtonEvent" );            
            CMsgAddressControl* toControl = ToCtrl();
			if( toControl == static_cast<CMsgAddressControl*>( aArg1 ) )
				{
                TBool modified = EFalse;
                if ( toControl->GetRecipientsL()->Count() )
                    {
                    // Recipients found. Verify addresses.
                    if ( !VerifyAddressesL( modified ) )
                        {
                        // Illegal address found.
                        modified = ETrue;
                        }
                    }
                if ( !modified )//else
                    {
                    // Nothing changed on the UI. Open PhoneBook.
                   HandleCommandL( EAmsEditorCmdAddRecipient ); 
                    }
				}
            }
            break;
        default:
            break;
        } 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::FetchFileL
// ---------------------------------------------------------
//
TBool CAudioMessageAppUi::FetchFileL( TFileName &aFileName )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::FetchFileL >>" ); 
    CDesCArrayFlat* dummy = new ( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( dummy );
    //TFileName fileName;

    TBool fetchOK = MsgAttachmentUtils::FetchFileL(
        MsgAttachmentUtils::EAudio,
        //fileName,
        aFileName,
        *dummy,
        ETrue,
        EFalse,
        this );
    //aFileName = fileName;
    CleanupStack::PopAndDestroy( dummy );
    AMSLOGGER_WRITE( "CAudioMessageAppUi::FetchFileL <<" ); 
    return fetchOK;
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::VerifySelectionL
// ---------------------------------------------------------
//
TBool CAudioMessageAppUi::VerifySelectionL( const MDesCArray* aSelectedFiles )
    {
   	AMSLOGGER_WRITE( "CAudioMessageAppUi::VerifySelectionL >>" ); 
	CAknInputBlock::NewLC();
	
   	TBool ret = EFalse;
    if ( aSelectedFiles->MdcaCount() == 1 )
        {
        TPtrC ptr = aSelectedFiles->MdcaPoint( 0 );
        if ( CreateMediaInfoForInsertL( ptr ) )
            {
            ret = ETrue;
            }
        }

    CleanupStack::PopAndDestroy(); //input blocker
    AMSLOGGER_WRITE( "CAudioMessageAppUi::VerifySelectionL <<" ); 
    return ret;
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::HandleEdwinEventL
//
// Performs character counter updating if neccessary. This is needed
// for supporting on-screen keyboard. Normally character input
// is handled already on OfferKeyEventL function but it does no
// harm to do it here again.
// ---------------------------------------------------------
//
void CAudioMessageAppUi::HandleEdwinEventL( CEikEdwin* /*aEdwin*/, TEdwinEvent aEventType )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::HandleEdwinEventL >>" ); 
    if ( aEventType == MEikEdwinObserver::EEventTextUpdate )
        {
        HandleCharInputL();
        }
    AMSLOGGER_WRITE( "CAudioMessageAppUi::HandleEdwinEventL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::HandleKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CAudioMessageAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::HandleKeyEventL >>" ); 
    if(iEditorFlags & EProgressInUse)
    	{
        return EKeyWasNotConsumed;
    	}
    
    if (!(iEditorFlags & ELaunchSuccessful))
    	{
        return EKeyWasNotConsumed;
    	}
    // still launching
    if ( !Document()->IsLaunched() )
        {
        return EKeyWasNotConsumed;
        }
   	if ( iInserting )
      	{
        return EKeyWasNotConsumed;
      	}

    if ( iView && !IsDisplayingMenuOrDialog() )
        {
        switch ( aKeyEvent.iCode )
        	{
            case EKeyLeftArrow:
#ifdef RD_MSG_NAVIPANE_IMPROVEMENT            
               	if ( Document()->GetAppMode() == EAmsViewer &&
               			IsNextMessageAvailableL( EFalse ))
                	{
                   	NextMessageL( EFalse );
                	}
#else
                if ( Document()->GetAppMode() == EAmsViewer &&
               			iNaviPane->Top() == iNavi_next_prev_msg_arrows &&
               			IsNextMessageAvailableL( EFalse ))
                	{
                   	NextMessageL( EFalse );
                	}
#endif            
    	
                break;
          	case EKeyRightArrow:
#ifdef RD_MSG_NAVIPANE_IMPROVEMENT          	
                if ( Document()->GetAppMode() == EAmsViewer &&
              			IsNextMessageAvailableL( ETrue ))
                	{
                   	NextMessageL( ETrue );
                	}
#else
                if ( Document()->GetAppMode() == EAmsViewer &&
              			iNaviPane->Top() == iNavi_next_prev_msg_arrows &&
              			IsNextMessageAvailableL( ETrue ))
                	{
                   	NextMessageL( ETrue );
                	}
#endif                	
                break;
           	case EKeyOK: // Selection key: Show context menus OR play/record clip
                {    
                // Check if find item highlight needs to be enabled
                if ( CheckFindItemHighlightL( aKeyEvent, aType ) )
                    {
                    return EKeyWasConsumed;
                    }
                
                if ( Document()->GetAppMode() == EAmsEditor )
                  	{
                  	HandleCommandL( EAmsSoftkeyAdd );
                  	return EKeyWasConsumed;
                  	}
                else
                  	{
                  	CMsgBaseControl* ctrl = iView->FocusedControl();
                  	if (  ctrl->ControlId() == EMsgComponentIdImage)
                    	{
                    	HandleCommandL( EAmsSoftkeyAdd );
                    	}
                  	else
                    	{
                    	MenuBar()->SetMenuTitleResourceId( R_AUDIOMESSAGE_VIEWER_SELECTMENU );
                    	MenuBar()->SetMenuType(CEikMenuBar::EMenuContext);
                    	TRAP_IGNORE(MenuBar()->TryDisplayMenuBarL());
                    	MenuBar()->SetMenuTitleResourceId( R_AUDIOMESSAGE_MENUBAR );
                    	MenuBar()->SetMenuType(CEikMenuBar::EMenuOptions);
                    	return EKeyWasConsumed;
                    	}

                  	}
                return EKeyWasConsumed;
                }
           	case EKeyYes:
                {
                if ( Document()->GetAppMode() == EAmsEditor )
                  	{
                  	// CallCreationKey: Send message if recipients and audio clip exists.
                  	//                  Otherwise fetch recipients
                  	if ( !IsHeaderEmpty() &&
                     		( !IsBodyEmpty() ||
                       		Document()->GetClipStatus() != EAmsClipNone ) )
                      	{
                      	// has address/es and audio clip inserted/recorded -> Send
                      	HandleCommandL( EAmsEditorCmdSend );
                      	}
                  	else if ( IsHeaderEmpty() )
                      	{
                      	HandleCommandL( EAmsEditorCmdAddRecipient ); 
                      	}
                  	else if ( IsBodyEmpty() )
                    	{
                    	ShowInformationNoteL( R_AUDIOMESSAGE_INFO_CANNOT_SEND, EFalse );
                    	}
                  	}
                else
                  	{
                  	// if from field not having a valid phone number, show dialer
                  	CMsgBaseControl* ctrl = iView->FocusedControl();
                  	if(	iSenderType == ESenderTypePhoneNumber && 
                  		ctrl && ctrl->ControlId() == EMsgComponentIdFrom)
	             				{
	             				DoCallToSenderL();									
                  		}
                	else
											{
                  		iAvkonAppUi->SetKeyEventFlags( 0x00);
											}
                  	}
                return EKeyWasConsumed;
                }
            case EKeyBackspace:
            case EKeyDelete:
                {
                CMsgBaseControl* ctrl = iView->FocusedControl();
                if ( ctrl && ctrl->ControlId() == EMsgComponentIdImage &&
                   		Document()->GetAppMode() == EAmsEditor  &&
                   		!IsBodyEmpty()  )
                	{
                    RemoveAudioL();
                    return EKeyWasConsumed;
                    }
                 if ( Document()->GetAppMode() == EAmsViewer )
                 	{
                    HandleCommandL( EAmsViewerCmdRemoveAudio );
                    return EKeyWasConsumed;
                 	}
                }
            	break;
            case EKeyEnter:
            	{
            	// Check if find item highlight needs to be enabled
                if ( CheckFindItemHighlightL( aKeyEvent, aType ) )
                    {
                    return EKeyWasConsumed;
                    }
              	if (DoEnterKeyL() )
                	{
                  	return EKeyWasConsumed;
                  	}
              	break;
              	}
        	case EKeyNull:
        		if( aKeyEvent.iScanCode == 0xA5 /*EStdKeySpace*/ )
        			{
       				SetNaviPaneL( );
        			}
        		break;
            default:
                break;
            }

        switch ( aKeyEvent.iScanCode )
            {
            case EStdKeyUpArrow:
                // FALLTHROUGH
            case EStdKeyDownArrow:
                // FALLTHROUGH
            case EStdKeyLeftArrow:
                // FALLTHROUGH
            case EStdKeyRightArrow:
                // FALLTHROUGH
            case EStdKeyDevice1: // Close key
                // FALLTHROUGH
            case EStdKeyRightShift: // Shift
                // FALLTHROUGH
            case EStdKeyApplication0: // Task switching
    AMSLOGGER_WRITE( "CAudioMessageAppUi::HandleKeyEventL <<" ); 
                return CMsgEditorAppUi::HandleKeyEventL( aKeyEvent, aType );
            default:
                break;
            }

        TKeyResponse resp = CMsgEditorAppUi::HandleKeyEventL( aKeyEvent, aType );

        if ( aType == EEventKeyUp )
            {
            // Check if character inserted to controls
            HandleCharInputL();
            }
AMSLOGGER_WRITE( "CAudioMessageAppUi::HandleKeyEventL <<" ); 

        return resp;
        }
    else
        {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::HandleKeyEventL <<" ); 
        return CMsgEditorAppUi::HandleKeyEventL( aKeyEvent, aType );
        }
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::HandleCharInputL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::HandleCharInputL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::HandleCharInputL >>" ); 
    if ( Document()->IsLaunched() )
        {
        CMsgBaseControl* ctrl = iView->FocusedControl();
        TBool changed = EFalse;

        if ( ctrl )
            {
            switch ( ctrl->ControlId() )
                {
                case EMsgComponentIdTo:
                    if ( ctrl->IsModified() )
                        {
                  		Document()->SetHeaderModified( ETrue );
                        changed = ETrue;
                        }
                    SetAddressSize();
                    break;
                case EMsgComponentIdFirstFreeEditorSpecificId:
                  	if ( ctrl->IsModified() )
                    	{
                    	Document()->SetBodyModified( ETrue );
                    	changed = ETrue;
                    	}
                  	break;
                default:
                    break;
                }
            }
        if ( changed )
            {
      		SetNaviPaneL( );
            }
        }
    AMSLOGGER_WRITE( "CAudioMessageAppUi::HandleCharInputL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DynInitMenuPaneL >>" ); 
    // Confirm app is running properly 
    if ( !Document()->IsLaunched() )
        {
        // If not, hide everything and return
        TInt amountOfItems = aMenuPane->NumberOfItemsInPane();
        if ( amountOfItems )
            {
            aMenuPane->DeleteBetweenMenuItems( 0, amountOfItems-1 );
            return;
            }
        }

  	if ( !iFindItemMenu )
        {
        iFindItemMenu = CFindItemMenu::NewL( EFindItemMenuPlaceHolder );
        if ( SenderType() == ESenderTypeMailAddress )
            {
            // Set sender type to find menu
            iFindItemMenu->SetSenderDescriptorType( CItemFinder::EEmailAddress );
            }
    else
        {
            // Set sender type to find menu
            iFindItemMenu->SetSenderDescriptorType( CItemFinder::EPhoneNumber );
            }
        }

    switch ( aResourceId )
        {
        case R_AUDIOMESSAGE_MAIN_MENU:
            {
            if ( !iMtm->Entry().Entry().ReadOnly() ) // editor
              	{
                /*if ( Document()->GetClipStatus() == EAmsClipNone )
              		{
              		aMenuPane->SetItemDimmed( EAmsEditorCmdSend, ETrue );
              		}*/
              	if ( Document()->GetClipStatus() == EAmsClipNone )
	                {
	                aMenuPane->SetItemDimmed( EAmsEditorSubmenuReplaceAudioCommands, ETrue );
	                aMenuPane->SetItemDimmed( EAmsEditorCmdPlayAudio, ETrue );
	                aMenuPane->SetItemDimmed( EAmsEditorCmdSaveAudio, ETrue );
	                }
	            else
                	{
                	aMenuPane->SetItemDimmed( EAmsEditorSubmenuInsertAudioCommands, ETrue );
                	}
              	if ( Document()->GetClipStatus() == EAmsClipFromGallery )
                	{
                	aMenuPane->SetItemDimmed( EAmsEditorCmdSaveAudio, ETrue );
                	}
              	if ( !( iSupportedFeatures & EUniFeatureHelp ) )
                  	{
                  	aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
                  	}
              	aMenuPane->SetItemDimmed( EAmsViewerCmdSaveAudio, ETrue );
	            aMenuPane->SetItemDimmed( EAmsViewerCmdSubmenuReply, ETrue );
	            aMenuPane->SetItemDimmed( EAmsViewerCmdForward, ETrue );
	            aMenuPane->SetItemDimmed( EFindItemMenuPlaceHolder, ETrue );
	            aMenuPane->SetItemDimmed( EAmsViewerCmdDeliveryStatus, ETrue );
	            aMenuPane->SetItemDimmed( EAmsViewerCmdMoveToFolder, ETrue );
	            aMenuPane->SetItemDimmed( EAmsViewerCmdRemoveAudio, ETrue );              	
              	if ( !(iSupportedFeatures & EUniFeaturePriority) )              	
				    {
				    aMenuPane->SetItemDimmed( EAmsEditorCmdChangePriority, ETrue );
				   	}
              	}
            else // viewer 
              	{
            	const TPtrC details = iMtm->Entry().Entry().iDetails;

                TInt focusedControl = FocusedControlId();                         
                TBool senderHighlighted = EFalse;
                
                if ( focusedControl == EMsgComponentIdFrom )
                    {
                    CMsgAddressControl* address = static_cast<CMsgAddressControl*>(
                        iView->ControlById( EMsgComponentIdFrom ) );
                    
                    if ( address && address->Editor().SelectionLength() 
                            == address->Editor().TextLength() )
                        {
                        senderHighlighted = ETrue;
                        }
                    }
            	   	
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
              	iFindItemMenu->AddItemFindMenuL (
                  	0,
                  	aMenuPane,
                  	EFindItemMenuPlaceHolder,
                  	senderHighlighted ? 
                  	TMmsGenUtils::PureAddress( iMtm->Sender( ) ) : KNullDesC(),
          			( details.Length() == 0 ),
                  	EFalse );
          		//in sent folder reply is dimmed
               	if ( iMtm->Entry().Entry().Parent() == KMsvSentEntryIdValue )
                	{
                    aMenuPane->SetItemDimmed( EAmsViewerCmdSubmenuReply, ETrue );
                	}
        		if ( iMtm->Sender().Length() <= 0 )
        			{
                  	aMenuPane->SetItemDimmed( EAmsViewerCmdSubmenuReply, ETrue );
        			}
	            aMenuPane->SetItemDimmed( EAmsEditorCmdSaveAudio, ETrue );
	            aMenuPane->SetItemDimmed( EAmsEditorCmdSend, ETrue );
	            aMenuPane->SetItemDimmed( EAmsEditorCmdAddRecipient, ETrue );
	            aMenuPane->SetItemDimmed( EAmsEditorSubmenuInsertAudioCommands, ETrue );
	            aMenuPane->SetItemDimmed( EAmsEditorSubmenuReplaceAudioCommands, ETrue );
	            aMenuPane->SetItemDimmed( EAmsEditorCmdChangePriority, ETrue );          
                if (! (     iSupportedFeatures & EUniFeatureDeliveryStatus  
                        && ( ( Document( )->Entry( ).iMtmData2 & KMmsDeliveryStatusMask) 
                        != KMmsDeliveryStatusNotRequested ) ) )
                    {
    		        aMenuPane->SetItemDimmed( EAmsViewerCmdDeliveryStatus, ETrue );
                    }
              	if ( !( iSupportedFeatures & EUniFeatureHelp ) )
                  	{
                  	aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
                  	}

              	}
            }
            break;
        case R_AUDIOMESSAGE_VIEWER_CONTEXT_MENU://call, reply fw, delete
          	{
          	TPhCltTelephoneNumber  number;          // phoneclient
          	number.Zero();
          	const TPtrC details = iMtm->Entry().Entry().iDetails;
          	
          	if(CommonPhoneParser::IsValidPhoneNumber( 
          	                  details, CommonPhoneParser::ESMSNumber ))
                {
             number = details;
                }   
            if( (FocusedControlId( ) == EMsgComponentIdFrom) && ( ( iMtm->Sender( ) ).Length() ) )
                {
                iFindItemMenu->SetSenderHighlightStatus( ETrue );
                iFindItemMenu->SetSenderDisplayText( TMmsGenUtils::PureAddress( iMtm->Sender( ) ) );
                }
            else
     	        {
                iFindItemMenu->SetSenderHighlightStatus( EFalse );
	            }
          	iFindItemMenu->AddItemFindMenuL (
            	0,
        		aMenuPane,
              	EFindItemContextMenuPlaceHolder,
              	TMmsGenUtils::PureAddress( iMtm->Sender() ),
        		( KErrNotFound == number.Match( details ) ), //"Is sender known"
            	ETrue );
            // no items dimmed for now
          	if ( iMtm->Sender().Length() <= 0 )
          		{
              	aMenuPane->SetItemDimmed( EAmsViewerCmdSubmenuReply, ETrue );
          		}
          	break;
          	}
        case R_AUDIOMESSAGE_ZOOM_SUBMENU:
            {
            TInt zoomLevel = KErrGeneral;
            iMsgEditorAppUiExtension->iSettingCachePlugin.GetValue( KAknLocalZoomLayoutSwitch, 
                                                                    zoomLevel );
            switch ( zoomLevel )
                {
                case EAknUiZoomAutomatic:
                    {
                    aMenuPane->SetItemButtonState( EAmsDispSizeAutomatic,
                                                   EEikMenuItemSymbolOn );
                    break;
                    }
                case EAknUiZoomNormal:
                    {
                    aMenuPane->SetItemButtonState( EAmsDispSizeNormal,
                                                   EEikMenuItemSymbolOn );
                    break;
                    }
                case EAknUiZoomSmall:
                    {
                    aMenuPane->SetItemButtonState( EAmsDispSizeSmall,
                                                   EEikMenuItemSymbolOn );
                    break;
                    }
                case EAknUiZoomLarge:
                    {
                    aMenuPane->SetItemButtonState( EAmsDispSizeLarge,
                                                   EEikMenuItemSymbolOn );
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            break;
            }  	
       	case R_AUDIOMESSAGE_REPLY_SUBMENU:
        	{
            if ( iMtm->AddresseeList().Count() < 2  )
            	{
                aMenuPane->SetItemDimmed( EAmsViewerCmdReplyToAll, ETrue );
            	}
        	break;
        	}
        default:
          	if ( iFindItemMenu )
            	{
        		iFindItemMenu->UpdateItemFinderMenuL( aResourceId, aMenuPane );
            	}
          	break;
        }
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DynInitMenuPaneL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::IsMessageEmpty
// ---------------------------------------------------------
//
TBool CAudioMessageAppUi::IsMessageEmpty() const
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::IsMessageEmpty >>" ); 
    return ( IsHeaderEmpty() && IsBodyEmpty() );
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::DoUserSendL
// ---------------------------------------------------------
// 
void CAudioMessageAppUi::DoUserSendL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoUserSendL >>" ); 
    //Send command may be issued without recipient
    //in this case the phonebook is opened
	if(iInserting)
		{
		return;
		}
    if ( IsHeaderEmpty() )
        {
        HandleCommandL( EAmsEditorCmdAddRecipient ); 
        return;
        }
    else if ( IsBodyEmpty() )
        {
        ShowInformationNoteL( R_AUDIOMESSAGE_INFO_CANNOT_SEND, EFalse );
        return;
        }

    iCoeEnv->SyncNotifyFocusObserversOfChangeInFocus();
    // Check message size
    if ( TUint( MessageSizeInBytes() ) > Document()->MaxMessageSize() )
        {
        TInt maxInKilos = ( Document()->MaxMessageSize() + KAmsBytesInKilo - 1 ) / KAmsBytesInKilo;
        HBufC* string = StringLoader::LoadLC(
            R_AUDIOMESSAGE_INFO_OBJECT_TOO_BIG,
            maxInKilos,
            iCoeEnv );
        CAknInformationNote* note = new ( ELeave ) CAknInformationNote();
        note->ExecuteLD( *string );
        CleanupStack::PopAndDestroy( string );  // string
        return;
        }
    TBool dummy;
    if ( !CheckNamesL( dummy ) )
        {
        return;
        }

    TInt resourceId = IsPhoneOfflineL()
        ? R_QTN_WAIT_MSG_SAVED_OUTBOX
        : R_QTN_AUDIOMESSAGE_WAIT_SEND_MESSAGE;

    if ( !iAddressHandler )
        {
        iAddressHandler = CUniAddressHandler::NewL(
            *iMtm,
            *ToCtrl(),
            *iCoeEnv );
        }
    iSendOperation = CAudioMessageSendOperation::NewL(
        *ToCtrl(),
        *iAddressHandler,
        *this,
        *Document(),
        *iView );       
        
    iEditorFlags |= EEditorClosing;

    iAbsorber = CAknInputBlock::NewLC();
    CleanupStack::Pop( iAbsorber ); //iAbsorber

    iSendOperation->Send();
    ShowWaitNoteL( resourceId );
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoUserSendL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::DoMsgSaveExitL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoMsgSaveExitL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoMsgSaveExitL >>" ); 
    if ( CAknEnv::AppWithShutterRunning() )
        {
        iEditorFlags |= ERunAppShutterAtExit;
        }
    if ( IsMessageEmpty() )
        {
        DeleteCurrentEntryL();
        }
   	else if ( CanSaveMessageL())
      	{
     	if (  Document()->Modified() )
         	{
          	// Needs saving
          	if ( IsForeground() )
              	{
              	TInt resId =
                  	Document()->Saved() ?
                  	R_QTN_AUDIOMESSAGE_WAIT_SAVING_MESSAGE :
                  	R_QTN_AUDIOMESSAGE_WAIT_SAVING_MESSAGE_NEW;
		       	// Cannot trap call below.
		        iEditorFlags |= EEditorClosing;
		        DoSaveL();
		        ShowWaitNoteL( resId );
              	}
          	else
              	{
              	TInt err(KErrNone);
              	TRAP( err, DoSaveL() );
              	}
          	BeginActiveWait();
          	if ( iEditorFlags & ERunAppShutterAtExit )
              	{
              	CAknEnv::RunAppShutter();
              	}
            }
      	}
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoMsgSaveExitL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::DeleteCurrentEntryL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DeleteCurrentEntryL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DeleteCurrentEntryL >>" ); 
    CAudioMessageDocument& doc = *Document();

    TInt msgStoreDrive = Document()->Session( ).CurrentDriveL( );
    FsSession().GetReserveAccess( msgStoreDrive );

    if ( doc.CurrentEntry().OwningService() == KMsvLocalServiceIndexEntryId )
        {
        const TMsvId id = doc.Entry().Id();
        TRAPD(
                err, 
                {
                doc.CurrentEntry().SetEntryL( doc.Entry().Parent() );
                doc.CurrentEntry().DeleteL( id );
                }
            );
        if ( err != KErrNone )
            {
            doc.Session().RemoveEntry( id );
            }
        }
    FsSession().ReleaseReserveAccess( msgStoreDrive );
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DeleteCurrentEntryL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::DeleteAndExitL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DeleteAndExitL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DeleteAndExitL >>" ); 
    DeleteCurrentEntryL();
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DeleteAndExitL <<" ); 
    Exit( EAknSoftkeyBack );
    }


// ---------------------------------------------------------
// CAudioMessageAppUi::ExitAndSaveL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::ExitAndSaveL( )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::ExitAndSaveL >>" ); 
    if ( iEditorFlags & EEditorClosing )
        {
        Exit( EAknSoftkeyClose );
        return;
        }
    if ( CAknEnv::AppWithShutterRunning() )
        {
        iEditorFlags |= ERunAppShutterAtExit;
        }
    if (    iEditorFlags & ELaunchSuccessful
        &&  Document()->MediaAvailable() )
        {
        TInt err(KErrNone);
        TRAP( err, DoMsgSaveExitL() );
        }
    AMSLOGGER_WRITE( "CAudioMessageAppUi::ExitAndSaveL <<" ); 
    Exit();
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::DeleteMessageL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DeleteMessageL()
  	{
  	AMSLOGGER_WRITE( "CAudioMessageAppUi::DeleteMessageL >>" ); 
  	if ( !ShowConfirmationQueryL( R_AUDIOMESSAGE_QUEST_DELETE_MESSAGE ) )
  		{
	    CMsgBaseControl* ctrl = iView->FocusedControl();  // ctrl can be NULL.
		SetNaviPaneL();
    	return;
    	}
  	DeleteAndExitL();
  	AMSLOGGER_WRITE( "CAudioMessageAppUi::DeleteMessageL <<" ); 
  	}

// ---------------------------------------------------------
// CAudioMessageAppUi::DoBackSaveL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoBackSaveL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoBackSaveL >>" ); 
    //Make sure this function is not run in case of very fast press
    //of close key after play soft key	
	if ( iEditorFlags & EProgressInUse)
		{
		return;
		}

    if ( Document()->GetAppMode() == EAmsViewer )
      	{
      	Exit( EAknSoftkeyClose );
      	}
    if ( IsMessageEmpty() && Document()->GetClipStatus() == EAmsClipNone )
        {
        if ( iMtm->Entry().Entry().Visible() )
            {
            ShowInformationNoteL( R_AUDIOMESSAGE_INFO_DELETED, ETrue );
            }
        DeleteAndExitL();
        }
    else
        {
        TInt closeVal = ShowCloseQueryL();
        if ( closeVal == EMsgCloseCancel )
            {
            return;
            }
        else if ( closeVal == EMsgCloseDelete )
            {
            DeleteCurrentEntryL();
        	Exit( EAknSoftkeyBack );
            }
        else // closeVal == EMsgCloseSave
            {
            // Message has data or there is a recipient
            if ( Document()->Modified() || ToCtrl() )
                {
                if ( CanSaveMessageL() )
                    {
                    TInt resId =
                        Document()->Saved() ?
                        R_QTN_AUDIOMESSAGE_WAIT_SAVING_MESSAGE :
                        R_QTN_AUDIOMESSAGE_WAIT_SAVING_MESSAGE_NEW;
                    iEditorFlags |= EEditorClosing;

                    DoSaveL();
                    ShowWaitNoteL( resId );
                    BeginActiveWait();
                    }
                else
                    {
                    if ( ShowConfirmationQueryL( R_AUDIOMESSAGE_QUEST_CLOSE_OOD ) )
                        {
                        // Exit without saving.
                        Exit( EAknSoftkeyClose );
                        }
                    }
                }
            else
               {
                // No changes -> just go away.
               Exit( EAknSoftkeyClose );
               }
            }
        }
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoBackSaveL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::CanSaveMessageL
// ---------------------------------------------------------
//
TBool CAudioMessageAppUi::CanSaveMessageL() const
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::CanSaveMessageL" ); 
   	TInt bytes =
       	Document()->AddressSize() +
        Document()->AudioFileSize();
    return !( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL(Document()->Session(),
        bytes ) );
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::DoSaveL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoSaveL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoSaveL >>" ); 
    if ( !CanSaveMessageL() )
        {
        User::Leave( KErrDiskFull );
        }
    iAbsorber = CAknInputBlock::NewLC();
    CleanupStack::Pop( iAbsorber );
 
    delete iSaveOperation;
    iSaveOperation = NULL;
    iSaveOperation = CAudioMessageSaveOperation::NewL( *ToCtrl(),
        *this,
        *Document(),
        *iView );
    iSaveOperation->Save();
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoSaveL <<" ); 
    }


// ---------------------------------------------------------
// CAudioMessageAppUi::CheckNamesL
// ---------------------------------------------------------
//
TBool CAudioMessageAppUi::CheckNamesL( TBool& aModified )
    {	
    AMSLOGGER_WRITE( "CAudioMessageAppUi::CheckNamesL >>" ); 
    iCoeEnv->SyncNotifyFocusObserversOfChangeInFocus();
    CMsgAddressControl* headerField = ToCtrl();
    aModified = EFalse;
    TBool retVal = VerifyAddressesL( aModified );

    if ( aModified )
        {
        Document()->SetHeaderModified( ETrue );
        SetAddressSize();
    	SetNaviPaneL( );
        }

    if ( !retVal )
        {
        //We'll get here if there's illegal addresses
        if ( headerField )
            {
            // Search first invalid address
            CMsgRecipientArray* recipients = headerField->GetRecipientsL();
            TInt addrCnt = recipients->Count();

            for ( TInt k = 0; k < addrCnt ; k++ )
                {
                CMsgRecipientItem* addrItem = recipients->At( k );
                if ( addrItem && !addrItem->IsValidated() )
                    {
                    TInt ret = headerField->HighlightUnvalidatedStringL();
                    if ( !ret )
                        {
                        // highlight succeeded
                        break;
                        }
                    }
                }
            }
        }
AMSLOGGER_WRITE( "CAudioMessageAppUi::CheckNamesL <<" ); 
    return retVal;
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::VerifyAddressesL
// ---------------------------------------------------------
//
TBool CAudioMessageAppUi::VerifyAddressesL( TBool& aModified )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::VerifyAddressesL >>" ); 
    if ( !iAddressHandler )
        {
        iAddressHandler = CUniAddressHandler::NewL(
            *iMtm,
            *ToCtrl(),
            *iCoeEnv );
        }
    AMSLOGGER_WRITE( "CAudioMessageAppUi::VerifyAddressesL ... after return <<" ); 

    return iAddressHandler->VerifyAddressesL( aModified );
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::DoUserAddRecipientL
// ---------------------------------------------------------
//JANI-7H3EVA :: fix:: Reddy
//
void CAudioMessageAppUi::DoUserAddRecipientL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoUserAddRecipientL >>" ); 
    
    if(iToolbar)
        {
        iToolbar->SetToolbarVisibility(EFalse, EFalse);
        }
 
    if ( !iAddressHandler )
        {
        iAddressHandler = CUniAddressHandler::NewL(
            *iMtm,
            *ToCtrl(),
            *iCoeEnv );
        }

    TBool dummy( EFalse );
    TBool recipientAdded( EFalse );
    recipientAdded =  iAddressHandler->AddRecipientL( dummy ); 
    if (recipientAdded)
        {
        Document()->SetHeaderModified( ETrue );
        SetAddressSize();
        }
    
    if(iToolbar)
        {
        iToolbar->SetToolbarVisibility(ETrue, EFalse);
        }
        
    if( recipientAdded)
        {
        iView->SetFocus( EMsgComponentIdTo );
        }

    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoUserAddRecipientL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::SetSubjectL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::SetSubjectL()
  	{
  	AMSLOGGER_WRITE( "CAudioMessageAppUi::SetSubjectL >>" ); 
  	HBufC* subject = iMtm->SubjectL().AllocLC();
  	TPtr subjectPtr = subject->Des();
  	subjectPtr.Trim();
  	if ( subjectPtr.Length() )
    	{
        iView->AddControlFromResourceL( R_AUDIOMESSAGE_SUBJECT, EMsgExpandableControl,
          	EMsgAppendControl, EMsgHeader );
      	CMsgExpandableControl* subjectControl =
      		static_cast<CMsgExpandableControl*> ( iView->ControlById( EMsgComponentIdSubject ) );
        subjectControl->Editor().SetTextL( subject );
      	}
  	CleanupStack::PopAndDestroy( subject ); //subject
  	AMSLOGGER_WRITE( "CAudioMessageAppUi::SetSubjectL <<" ); 
  	}

// ---------------------------------------------------------
// CAudioMessageAppUi::SetSenderL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::SetSenderL()
  	{
  	AMSLOGGER_WRITE( "CAudioMessageAppUi::SetSenderL >>" ); 
  	iView->AddControlFromResourceL( R_AUDIOMESSAGE_VIEWER_FROM,
                  EMsgAddressControl,
                  EMsgAppendControl,
                  EMsgHeader );

  	CMsgAddressControl* fromCtrl = FromCtrl();
    const TPtrC sender = iMtm->Sender();
    const TPtrC details = iMtm->Entry().Entry().iDetails;

    if ( sender.Length() )
        {
        if ( CommonPhoneParser::IsValidPhoneNumber(
            TMmsGenUtils::PureAddress( sender ),
            CommonPhoneParser::ESMSNumber ) )
            {
            iSenderType = ESenderTypePhoneNumber;
            }
        else
            {
            //We trust on that there won't be illegal addresses
            //in received messages
            iSenderType = ESenderTypeMailAddress;
            }

        //Check if MMS engine has put an alias into iDetails field
        if ( sender.Compare( details ) != 0 )
            {
            TPtrC alias = KNullDesC();
            
            if ( details.Length() && !iAlias )
                {
                iAlias = details.AllocL( );
                alias.Set( *iAlias );
                }
            MVPbkContactLink* link = NULL;
            fromCtrl->AddRecipientL( alias,
                          TMmsGenUtils::PureAddress( sender ),
                          ETrue,
                          link );

            }
        else
            {
            MVPbkContactLink* link = NULL;
            fromCtrl->AddRecipientL( TMmsGenUtils::Alias( sender ),
                          TMmsGenUtils::PureAddress( sender ),
                          ETrue,
                          link );

            }
        }
        else
        	{
        	iSenderType = ESenderTypeNone;       	
        	}
  	AMSLOGGER_WRITE( "CAudioMessageAppUi::SetSenderL <<" ); 
  	}

// ---------------------------------------------------------
// CAudioMessageAppUi::InsertRecipientL
// EKLU-7LGCPT :: Filter Invalid contact details
// ---------------------------------------------------------
void CAudioMessageAppUi::InsertRecipientL()
  	{
  	AMSLOGGER_WRITE( "CAudioMessageAppUi::InsertRecipientL >>" ); 

  	if ( Document()->GetAppMode() == EAmsViewer )
    	{
    	iView->AddControlFromResourceL(  R_AUDIOMESSAGE_VIEWER_TO,
               EMsgAddressControl,
               EMsgAppendControl,
               EMsgHeader );
    	}
    
    TBool addInvalidRecipient = EFalse;
    if ( !iAddressHandler )
        {
        iAddressHandler = CUniAddressHandler::NewL( *iMtm, *ToCtrl(), *iCoeEnv );
        }
    
    if( Document()->Entry().iMtmData1 & KMmsMessageEditorOriented )
        {
        //if opened from draft folders, then simply open the message as saved, without any cntact filtering
        addInvalidRecipient = ETrue;
        }
    iAddressHandler->CopyAddressesFromMtmL( EMsvRecipientTo, addInvalidRecipient );
    iAddressHandler->ShowInvalidRecipientInfoNotesL();

  	AMSLOGGER_WRITE( "CAudioMessageAppUi::InsertRecipientL <<" ); 
  	}

// ---------------------------------------------------------
// CAudioMessageAppUi::DoUserMessageInfoL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoUserMessageInfoL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoUserMessageInfoL >>" ); 
    CBaseMtmUi& uiMtm = Document()->MtmUi();
    CAknInputBlock::NewLC();
  	if ( Document()->HeaderModified() )
    	{
     	const CMsvRecipientList& addresses = iMtm->AddresseeList();
    	// Reset addresses in clientMtm
      	for ( TInt i = iMtm->AddresseeList().Count(); --i >=  0; )
        	{
            iMtm->RemoveAddressee( i );
        	}

      	const CMsgRecipientArray* recipients = ToCtrl()->GetRecipientsL();
      	// inserts addresses from address control to clientMtm
      	TInt addrCnt = recipients->Count();
      	for ( TInt ii = 0; ii < addrCnt ; ii++ )
      	    {
      	    CMsgRecipientItem* addrItem = recipients->At(ii);
      	    TPtr realAddress = addrItem->Address()->Des();
      	    TPtr alias = addrItem->Name()->Des();
      	    iMtm->AddAddresseeL( EMsvRecipientTo, realAddress, alias );
      	    }
      	iMtm->SaveMessageL();
      	Document()->SetHeaderModified( EFalse );
    	}

  	TMmsMsvEntry tEntry = static_cast<TMmsMsvEntry>( Document()->Entry() );
    tEntry.iBioType = KUidMsgSubTypeMmsAudioMsg.iUid;
    // Save made because user launched Message Info, Preview or Objects view
    // Message should stay invisible and in preparation if it was.
    if ( !tEntry.Visible() )
     	{
        // Guarantee that when Visible is ETrue InPreparation is EFalse
        tEntry.SetInPreparation( ETrue );
        }
    Document()->CurrentEntry().ChangeL(tEntry);
    // Pack message size
    TInt sizeInKilos = 0;
  	if ( EAmsClipAttachment == Document()->GetClipStatus() )
  		{
        sizeInKilos = ( Document()->AudioFileSize() + KAmsBytesInKilo - 1 ) / KAmsBytesInKilo;
  		}
    else
    	{
      	sizeInKilos=( MessageSizeInBytes() + KAmsBytesInKilo - 1 ) / KAmsBytesInKilo;
    	}

    TPckgBuf<TInt> param( sizeInKilos );

    //These are not used. They just have to be there
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );

    // Change to "sync" operation
    CMsvSingleOpWatcher* watch = CMsvSingleOpWatcher::NewLC( *Document() );
    CMsvOperation* op = uiMtm.InvokeAsyncFunctionL(
        KMtmUiFunctionMessageInfo,
        *selection,
        watch->iStatus,
        param );
    CleanupStack::Pop( watch ); // watch
    Document()->AddSingleOperationL( op, watch );

    CleanupStack::PopAndDestroy( 2 ); // selection, CAknInputBlock
	SetNaviPaneL();
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoUserMessageInfoL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::SetAudioClipIconL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::SetAudioClipIconL( TAmsRecordIconStatus aStatus )
  	{
  	AMSLOGGER_WRITE( "CAudioMessageAppUi::SetAudioClipIconL >>" ); 
    iLaunchOperation->DoPrepareBodyL( aStatus );
    AMSLOGGER_WRITE( "CAudioMessageAppUi::SetAudioClipIconL <<" ); 
  	}

// ---------------------------------------------------------
// CAudioMessageAppUi::PlayAudioL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::PlayAudioL()
	{
	AMSLOGGER_WRITE( "CAudioMessageAppUi::PlayAudioL >>" ); 
	//Make sure this function is not run in case of very fast double press
	//of play softkey
	if ( iEditorFlags & EProgressInUse)
		{
		return;
		}
	
	iEditorFlags |= EProgressInUse;
  	if ( IsSilentModeL()
        && !ShowConfirmationQueryL( R_QTN_AUDIOMESSAGE_OBEY_SILENT_MODE ) )
  		{
  		iEditorFlags &= ~EProgressInUse;	
    	return;
    	}
  	CMsvStore* store = Document()->Mtm().Entry().ReadStoreL();
  	CleanupStack::PushL( store );
  	MMsvAttachmentManager& manager = store->AttachmentManagerL();
    RFile rFile = manager.GetAttachmentFileL( Document()->iAudio );
    CleanupClosePushL( rFile );

    // Capture voice key while recording
    if ( iVoiceKeyHandle <= 0 )
        {
        iVoiceKeyHandle = iCoeEnv->RootWin().CaptureKey(
            EKeyDevice6, 0, 0 );
        }

	iProgressDlg = CAudioMessageProgressDialog::NewL( rFile, 
				iCurrentVolume, iSpeakerEarpiece, EFalse, *Document() );
	iProgressDlg->SetNaviObserver( this );	

    TRAPD( err, iProgressDlg->ExecuteLD( R_AUDIOMESSAGE_VOICE_SPEAK_DIALOG ) );
    if (err == KErrNotSupported)
    	{
    	Document()->SetClipStatus( EAmsClipCorrupted ); 
    	SetAudioClipIconL( EAudioCorrupted );
    	ShowErrorNoteL( R_AUDIOMESSAGE_ERROR_MSG_CORRUPT, ETrue );
    	User::Leave( KErrNotSupported );
    	}
    else if (err != KErrNone )
    	{
    	User::Leave( err );
    	}
    

    SetNaviPaneL();	

	iProgressDlg = NULL;
	
    CleanupStack::PopAndDestroy( &rFile );
    CleanupStack::PopAndDestroy( store );

  	// Release voice key
  	if ( iVoiceKeyHandle >= 0 )
      	{
      	iCoeEnv->RootWin().CancelCaptureKey( iVoiceKeyHandle );
      	iVoiceKeyHandle = KAmsNotCaptured;
      	}
   	iEditorFlags &= ~EProgressInUse;
AMSLOGGER_WRITE( "CAudioMessageAppUi::PlayAudioL <<" ); 
	}

// ---------------------------------------------------------
// CAudioMessageAppUi::InsertAudioL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::InsertAudioL( TBool aNew )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::InsertAudioL >>" ); 

   	iEditorFlags |= EProgressInUse;

    if ( aNew )
     	{
     	if ( !iAbsorber )
            {
            iAbsorber = CAknInputBlock::NewLC();
        	CleanupStack::Pop( iAbsorber );
            }
            
        if ( UnsupportedCallTypeOngoingL() )
        	{
            delete iAbsorber;
            iAbsorber = NULL;
            iEditorFlags &= ~EProgressInUse;
            return;
        	}

        SetNaviPaneL( );// Capture voice key while recording
      	if ( iVoiceKeyHandle <= 0 )
              {
              iVoiceKeyHandle = iCoeEnv->RootWin().CaptureKey(
                  EKeyDevice6, 0, 0 );
              }

      	TFileName* cmdFileName = new( ELeave ) TFileName;
    	CleanupStack::PushL( cmdFileName );

        MsgAttachmentUtils::GetMsgEditorTempPath( *cmdFileName );  // It's cleaned by base editor
        // Create temp directory if needed
        TInt err = iEikonEnv->FsSession( ).MkDirAll( *cmdFileName );
      	if ( err != KErrNone && err != KErrAlreadyExists )
        	{
            User::Leave( err );
            }
      	// prepare cmdFile name
      	cmdFileName->Append( *iClipFileName );
      	cmdFileName->Append( KAmrFileExt );
      	RFile rFile;
        User::LeaveIfError( rFile.Replace( FsSession(), *cmdFileName, EFileWrite ) ); 	
    	CleanupClosePushL( rFile );
    	
    	iProgressDlg 
    	    = CAudioMessageProgressDialog::NewL( rFile, iCurrentVolume, iSpeakerEarpiece, ETrue, *Document());
    	iProgressDlg->SetMaxMessageSize( Document()->MaxMessageSize() );
    	iProgressDlg->SetNaviObserver( this );
	
        delete iAbsorber;
        iAbsorber = NULL;
        
        iProgressDlg->ExecuteLD( R_AUDIOMESSAGE_VOICE_SPEAK_DIALOG );
		iAvkonAppUi->Cba()->MakeVisible( EFalse );
	
        if ( !iAbsorber )
         	{
            iAbsorber = CAknInputBlock::NewLC();
        	CleanupStack::Pop( iAbsorber );
          	}
        iProgressDlg=NULL;
           	
      	CleanupStack::PopAndDestroy( &rFile );
      	if(!CheckIncomingCall())
      	    {
      	    ShowWaitNoteL( R_QTN_AUDIOMESSAGE_WAIT_INSERTING);
      	    }
      	DoInsertAudioL( *cmdFileName  );
        	
         // Release voice key
        if ( iVoiceKeyHandle >= 0 )
          	{
          	iCoeEnv->RootWin().CancelCaptureKey( iVoiceKeyHandle );
          	iVoiceKeyHandle = KAmsNotCaptured;
          	}
      	}
    else
        {
        //must update navi here because if you got clip already and play it and after that
        //you insert a new clip from gallery the navi field is not correct
        SetNaviPaneL( );
      	TFileName tmpFileName;
      	if ( FetchFileL( tmpFileName ) )
          	{
          	if(!CheckIncomingCall())
      	    	{
      	   		 ShowWaitNoteL( R_QTN_AUDIOMESSAGE_WAIT_INSERTING);
      	   	 	}
        	if ( !iInsertOperation )
                {
                iInsertOperation = CAudioMessageInsertOperation::NewL(
                    *this,
                    *Document(),
                    *iView );
                }

            iInserting = ETrue;
            iAbsorber = CAknInputBlock::NewLC();
            CleanupStack::Pop( iAbsorber );
            iInsertOperation->Insert( iInsertingMedia, EFalse, EFalse  );
            Document()->SetClipStatus( EAmsClipFromGallery );	
          	}
        }
    
	iEditorFlags &= ~EProgressInUse;
AMSLOGGER_WRITE( "CAudioMessageAppUi::InsertAudioL <<" ); 
    }


// ---------------------------------------------------------
// CAudioMessageAppUi::DoInsertAudioL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoInsertAudioL( TFileName &aCmdFileName )
    {
    if ( iInsertingMedia )
	    {
	    delete iInsertingMedia;
	    iInsertingMedia = NULL;
	    }
    RFile fileHandle = Document()->MediaResolver()->FileHandleL( aCmdFileName );
    CleanupClosePushL( fileHandle );
    iInsertingMedia = Document()->MediaResolver()->CreateMediaInfoL( fileHandle );
    // No need to parse info details
    CleanupStack::PopAndDestroy( &fileHandle );

    if ( !iInsertOperation )
     	{
        		iInsertOperation = CAudioMessageInsertOperation::NewL(
               	 	*this,
                	*Document(),
                	*iView );
        }
    iInserting = ETrue;
    iInsertOperation->Insert( iInsertingMedia, EFalse ,ETrue );
    Document()->SetClipStatus( EAmsClipFromRecorder );
    CleanupStack::PopAndDestroy( &aCmdFileName );
    }


// ---------------------------------------------------------
// CAudioMessageAppUi::CreateMediaInfoForInsertL
// ---------------------------------------------------------
//
TBool CAudioMessageAppUi::CreateMediaInfoForInsertL( const TDesC& aFileName )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::CreateMediaInfoForInsertL >>" ); 
    if ( iInsertingMedia )
      	{
      	delete iInsertingMedia;
      	iInsertingMedia = NULL;
      	}

    RFile fileHandle = Document()->MediaResolver()->FileHandleL( aFileName );
    CleanupClosePushL( fileHandle );
    iInsertingMedia = Document()->MediaResolver()->CreateMediaInfoL( fileHandle );
    Document()->MediaResolver()->ParseInfoDetailsL( iInsertingMedia, fileHandle );
    CleanupStack::PopAndDestroy( &fileHandle );
    
    TInt errResId = 0;
    if ( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL( 
            Document()->Mtm().Session(),
         	iInsertingMedia->FileSize() ) )
    	{
        User::Leave( KErrDiskFull );
        }
  	if ( iInsertingMedia->FileSize() > Document()->MaxMessageSize() )
    	{
    	errResId = R_AUDIOMESSAGE_INFO_SIZE_EXCEEDED;
    	}
  	else if ( iInsertingMedia->MimeType().CompareF( KMsgMimeAudioAmr ) != 0 )
    	{
    	errResId = R_AUDIOMESSAGE_INFO_INVALID_OBJECT;
    	}
    	// Supported type check
    else if ( iInsertingMedia->Protection() )
        {
        errResId = R_AUDIOMESSAGE_INFO_UNABLE_TO_INSERT_DRM;
        }
    else if ( iInsertingMedia->Corrupt() )
        {
        // Corrupt is set to only supported media types
        errResId = R_AUDIOMESSAGE_INFO_CORRUPTED_OBJECT;
        }
    if ( errResId != 0 )
        {
        delete iInsertingMedia;
        iInsertingMedia = NULL;
        ShowInformationNoteL( errResId, EFalse );
AMSLOGGER_WRITE( "CAudioMessageAppUi::CreateMediaInfoForInsertL <<" ); 
        return EFalse;
        }
AMSLOGGER_WRITE( "CAudioMessageAppUi::CreateMediaInfoForInsertL <<" ); 
    return ETrue;

    }
 

// ---------------------------------------------------------
// CAudioMessageAppUi::InitViewerNaviPaneL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::InitViewerNaviPaneL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::InitNaviPaneL >>" ); 
#ifdef RD_MSG_NAVIPANE_IMPROVEMENT
    //main navi control container
    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( StatusPane( )->ControlL(  TUid::Uid( EEikStatusPaneUidNavi ) ) );

    
    //length
    MAknEditingStateIndicator* editIndi = iAvkonEnv->EditingStateIndicator();
    if( editIndi )
    	{
    	iNavi_length = editIndi->IndicatorContainer();	
    	}    

    TMsgEditorMsgPriority priority = EMsgEditorMsgPriorityNormal;
    
    if ( iMtm->MessagePriority() == EMmsPriorityHigh )
        {
        priority = EMsgEditorMsgPriorityHigh;
        }
    else if ( iMtm->MessagePriority() == EMmsPriorityLow )
        {
        priority = EMsgEditorMsgPriorityLow;
        }
    
    TBool utcTime = ETrue;    
    CreateViewerNaviPaneL( Document()->Entry().iDate, priority, utcTime  );

    iNaviPane->PushL( *iNaviDecorator );  // <- This has nothing to do with cleanup-stack
     
#else    
    //main navi control container
    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( StatusPane( )->ControlL(  TUid::Uid( EEikStatusPaneUidNavi ) ) ); 

#endif    
    AMSLOGGER_WRITE( "CAudioMessageAppUi::InitNaviPaneL <<" ); 
    }
   
// ---------------------------------------------------------
// CAudioMessageAppUi::InitViewerNaviPaneL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::InitEditorNaviPaneL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::InitEditorNaviPaneL >>" ); 

    //main navi control container
    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( StatusPane( )->ControlL(  TUid::Uid( EEikStatusPaneUidNavi ) ) ); 
    
    //length
    MAknEditingStateIndicator* editIndi = iAvkonEnv->EditingStateIndicator();
    if ( editIndi )
      	{
      	iNavi_length = editIndi->IndicatorContainer();
      	} 
    iNaviDecorator = iNaviPane->CreateEditorIndicatorContainerL();    
    AMSLOGGER_WRITE( "CAudioMessageAppUi::InitEditorNaviPaneL <<" ); 
    }     


// ---------------------------------------------------------
// CAudioMessageAppUi::UpdateViewerNaviFieldL 
// ---------------------------------------------------------
//
void CAudioMessageAppUi::UpdateViewerNaviFieldL()
  	{	
AMSLOGGER_WRITE( "CAudioMessageAppUi::UpdateViewerNaviFieldL >>" );   	
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT	
    TInt indexInFolder = 0;
    TInt msgsInFolder = 0;
    MessageIndexInFolderL( indexInFolder, msgsInFolder );
 
    CArrayFixFlat<TInt>* array = new ( ELeave ) CArrayFixFlat<TInt>(KNaviGranularity);
    CleanupStack::PushL( array );
    array->AppendL( indexInFolder + 1 );
    array->AppendL( msgsInFolder );

    HBufC* buf = StringLoader::LoadLC( R_AUDIOMESSAGE_VIEWER_ORDINAL_NUMBER, *array,
      iCoeEnv );
    //Pushed into CleanupStack
    TBuf<20> finalText;
    finalText.Append(*buf);
	AknTextUtils::LanguageSpecificNumberConversion( finalText );
    
    //Creates iNavi_next_prev_msg_arrows
    CreateTabGroupL( finalText );
    CleanupStack::PopAndDestroy( 2 ); //buf, array
    iNavi_next_prev_msg_arrows->MakeScrollButtonVisible( ETrue );
    //NaviButtons are dimmed by default
    if ( IsNextMessageAvailableL( ETrue ) )
        {
        iNavi_next_prev_msg_arrows->SetScrollButtonDimmed( CAknNavigationDecorator::
          ERightButton, EFalse );
        }
    if ( IsNextMessageAvailableL( EFalse ) )
        {
        iNavi_next_prev_msg_arrows->SetScrollButtonDimmed( CAknNavigationDecorator::
          ELeftButton, EFalse );
        }
  	iNaviPane->PushL( *iNavi_next_prev_msg_arrows );	
#endif
	AMSLOGGER_WRITE( "CAudioMessageAppUi::UpdateViewerNaviFieldL <<" ); 	
 		}


// ---------------------------------------------------------
// CAudioMessageAppUi::UpdateEditorNaviFieldL 
// ---------------------------------------------------------
//
void CAudioMessageAppUi::UpdateEditorNaviFieldL( TAmsNaviType aNaviType)
  	{	
  	AMSLOGGER_WRITE( "CAudioMessageAppUi::UpdateEditorNaviFieldL >>" ); 
  	TTimeIntervalMicroSeconds duration = Document()->GetDuration();
    TTime lengthAsTime( duration.Int64() );
	TInt strLength = iTimeDuratBase->Length();	
	HBufC* lengthNow2 = HBufC::NewLC( strLength );
 	TPtr lengthNow(lengthNow2->Des());
	lengthAsTime.FormatL( lengthNow, *iTimeDuratBase);
 
    TInt priorityHigh;
    TInt priorityLow;

    if ( iMtm->MessagePriority() == EMmsPriorityLow )
        {
        priorityHigh = EAknIndicatorStateOff;
        priorityLow = EAknIndicatorStateOn;
        }
    else if ( iMtm->MessagePriority() == EMmsPriorityHigh )
        {
        priorityHigh = EAknIndicatorStateOn;
        priorityLow = EAknIndicatorStateOff;
        }
    else
        {
        priorityHigh = EAknIndicatorStateOff;
        priorityLow = EAknIndicatorStateOff;
        }
   	AknTextUtils::LanguageSpecificNumberConversion( lengthNow );

 	switch ( aNaviType )
 		{
    	case EAmsOnlyLength:
    		{
         	CAknIndicatorContainer* indiContainer =
          		static_cast<CAknIndicatorContainer*>( iNaviDecorator->DecoratedControl() );
        	if ( indiContainer )
        		{
            	indiContainer->
                	SetIndicatorValueL(TUid::Uid(EAknNaviPaneEditorIndicatorMessageLength),
                	lengthNow);
            	
            	indiContainer->
              		SetIndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorMessageLength),
                	EAknIndicatorStateOn);
            	
      		    indiContainer->
              	    SetIndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorMcePriorityHigh),
              	    priorityHigh);
                
                indiContainer->
              	    SetIndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorMcePriorityLow),
                    priorityLow);
      		    
      		    iNaviPane->PushL( *iNaviDecorator);
        		}
    		}
      		break;
    	case EAmsTextEditorAndLength:
    		{
          	if ( !iNavi_length )
          		{
			  	MAknEditingStateIndicator* editIndi = iAvkonEnv->EditingStateIndicator();
			   	if ( editIndi )
			      	{
			      	iNavi_length = editIndi->IndicatorContainer();
			      	}

          		}
          	if ( iNavi_length )
              	{
              	iNavi_length->
                	SetIndicatorValueL(TUid::Uid(EAknNaviPaneEditorIndicatorMessageLength),
                	lengthNow);
              	iNavi_length->
                	SetIndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorMessageLength),
                  	EAknIndicatorStateOn);
              	}
      		break;
    		}
    	case EAmsMsgArrows:
 		    {	
            //Not in editor mode
	      	break;
    		}
    	default:
      		break;
  		}
  		CleanupStack::PopAndDestroy( lengthNow2 );	
	AMSLOGGER_WRITE( "CAudioMessageAppUi::UpdateEditorNaviFieldL <<" ); 	
 		}
// ---------------------------------------------------------
// CAudioMessageAppUi::CreateTabGroupL
// ---------------------------------------------------------
//
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT 
void CAudioMessageAppUi::CreateTabGroupL( const TDesC& aText )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::CreateTabGroupL >>" ); 
    CFbsBitmap* bitmapPriority = NULL;
   	CFbsBitmap* maskPriority = NULL;
    //Delete (possibly) existing navi decorator
    delete iNavi_next_prev_msg_arrows;
    iNavi_next_prev_msg_arrows = NULL;

    //Create TabGroup
    iNavi_next_prev_msg_arrows = iNaviPane->CreateTabGroupL();
    CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>( iNavi_next_prev_msg_arrows->
     	DecoratedControl() );
  	// Set path of bitmap file
  	TParse fileParse;
  	fileParse.Set( KAmsBitmapFileName, &KDC_APP_BITMAP_DIR, NULL );

  	if ( ( iMtm->MessagePriority() == EMmsPriorityHigh ) || (iMtm->MessagePriority() ==
    	EMmsPriorityLow) )
    	{
    	if ( iMtm->MessagePriority() == EMmsPriorityHigh )
      		{
      		// Create high priority bitmap
          	AknsUtils::CreateIconL( AknsUtils::SkinInstance(), KAknsIIDQgnIndiMcePriorityHigh,
              	bitmapPriority, maskPriority, fileParse.FullName(),
              	EMbmAudiomessageQgn_indi_mce_priority_high,
              	EMbmAudiomessageQgn_indi_mce_priority_high_mask );
      		}
    	else
      		{
      		// Create low priority bitmap
          	AknsUtils::CreateIconL( AknsUtils::SkinInstance(), KAknsIIDQgnIndiMcePriorityLow,
              	bitmapPriority, maskPriority, fileParse.FullName(),
              	EMbmAudiomessageQgn_indi_mce_priority_low,
              	EMbmAudiomessageQgn_indi_mce_priority_low_mask );
      		}

    	if ( !bitmapPriority )
      		{
      		User::Leave( KErrNotFound );
      		}
   		CleanupStack::PushL( bitmapPriority );
        CleanupStack::PushL( maskPriority );
        // Priority bitmap has been created, set size
        User::LeaveIfError( AknIconUtils::SetSize( bitmapPriority, KPriorityIconSize ) );
    	}

  	if (bitmapPriority)
    	{
    	tabGroup->AddTabL( 0, aText, bitmapPriority, maskPriority );
      	CleanupStack::Pop( maskPriority ); // ownership change
      	CleanupStack::Pop( bitmapPriority ); // ownership change
    	}
  	else
    	{
    	tabGroup->AddTabL( 0, aText );  // No icons required
    	}
    tabGroup->SetActiveTabById( 0 );
    tabGroup->SetTabFixedWidthL( EAknTabWidthWithOneTab );
#ifdef RD_SCALABLE_UI_V2
    iNavi_next_prev_msg_arrows->SetNaviDecoratorObserver( this );
#endif    
    AMSLOGGER_WRITE( "CAudioMessageAppUi::CreateTabGroupL <<" ); 
    }
#endif //!RD_MSG_NAVIPANE_IMPROVEMENT     


// ---------------------------------------------------------
// CAudioMessageAppUi::UpdateNaviVol
// ---------------------------------------------------------
//
void CAudioMessageAppUi::UpdateNaviVol( TInt aCurrentVolumeValue )
  	{
  	AMSLOGGER_WRITE( "CAudioMessageAppUi::UpdateNaviVol >>" ); 
    iCurrentVolume=aCurrentVolumeValue;
    AMSLOGGER_WRITE( "CAudioMessageAppUi::UpdateNaviVol <<" ); 
  	}

// ---------------------------------------------------------
// CAudioMessageAppUi::GetVolume
// ---------------------------------------------------------
//
TInt CAudioMessageAppUi::GetVolume()
    {
    return iCurrentVolume;
    }
// ---------------------------------------------------------
// CAudioMessageAppUi::SetSpeakerOutput
// ---------------------------------------------------------
//
void CAudioMessageAppUi::SetSpeakerOutput(TBool aEarpiece)
  	{
  	iSpeakerEarpiece = aEarpiece;
  	}

// ---------------------------------------------------------
// CAudioMessageAppUi::ShowInformationNoteL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::ShowInformationNoteL( TInt aResourceID, TBool aWaiting )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::ShowInformationNoteL >>" ); 
    HBufC* prompt = NULL;
    switch ( aResourceID )
        {
        case R_AUDIOMESSAGE_INFO_OBJECT_TOO_BIG:
        	{
            TInt maxInKilos 
                = ( Document()->MaxMessageSize() + KAmsBytesInKilo - 1 ) / KAmsBytesInKilo;
            prompt = StringLoader::LoadLC( aResourceID, maxInKilos, iCoeEnv  );
            break;
        	}
        default:
            prompt = StringLoader::LoadLC( aResourceID, iCoeEnv );
            break;
        }

    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( aWaiting );
    note->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    AMSLOGGER_WRITE( "CAudioMessageAppUi::ShowInformationNoteL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::ShowErrorNoteL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::ShowErrorNoteL( TInt aResourceID, TBool aWaiting )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::ShowErrorNoteL >>" ); 
    HBufC* prompt = StringLoader::LoadLC( aResourceID, iCoeEnv );
    CAknErrorNote* note = new ( ELeave ) CAknErrorNote( aWaiting );
    note->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    AMSLOGGER_WRITE( "CAudioMessageAppUi::ShowErrorNoteL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::ShowConfirmationQueryL
// ---------------------------------------------------------
//
TInt CAudioMessageAppUi::ShowConfirmationQueryL( TInt aResourceID )
    {
    HBufC* prompt = StringLoader::LoadLC( aResourceID, iCoeEnv );
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    TInt retVal = dlg->ExecuteLD( R_AUDIOMESSAGE_CONFIRMATION_QUERY, *prompt );
    CleanupStack::PopAndDestroy( prompt );
    AMSLOGGER_WRITE( "CAudioMessageAppUi::ShowConfirmationQueryL <<" ); 
    return retVal;
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::ShowWaitNoteL
// ---------------------------------------------------------
//
TBool CAudioMessageAppUi::ShowWaitNoteL( TInt aResourceId )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::ShowWaitNoteL >>" ); 
    TInt waitNoteResource = R_AUDIOMESSAGE_WAIT_NOTE;
    HBufC* string = NULL;

    switch ( aResourceId )
        {
        case R_QTN_AUDIOMESSAGE_WAIT_SEND_MESSAGE:
            {
            waitNoteResource = R_AUDIOMESSAGE_WAIT_NOTE_ICON;//
            }
            //fallthroug
        case R_QTN_WAIT_MSG_SAVED_OUTBOX:
        case R_QTN_AUDIOMESSAGE_WAIT_OPENING_EDITOR:
        case R_QTN_AUDIOMESSAGE_WAIT_SAVING_MESSAGE:
        case R_QTN_AUDIOMESSAGE_WAIT_SAVING_MESSAGE_NEW:
        case R_QTN_AUDIOMESSAGE_WAIT_INSERTING:
            {
            string = StringLoader::LoadLC(
                aResourceId,
                iCoeEnv );
            }
            break;
        default:
            break;
        }
    
    if ( aResourceId == R_QTN_WAIT_MSG_SAVED_OUTBOX
        || aResourceId == R_QTN_AUDIOMESSAGE_WAIT_SEND_MESSAGE
        //|| aResourceId == R_QTN_AUDIOMESSAGE_WAIT_SAVING_MESSAGE
        || aResourceId == R_QTN_AUDIOMESSAGE_WAIT_SAVING_MESSAGE_NEW )
    	{
    	if (!iWaitDialog)
    	iWaitDialog = new( ELeave )CAknWaitDialog(
            reinterpret_cast<CEikDialog**>( &iWaitDialog ), ETrue);
    	}
    else
    	{
    	if (!iWaitDialog)
    	iWaitDialog = new( ELeave )CAknWaitDialog(
            reinterpret_cast<CEikDialog**>( &iWaitDialog ));
    	}
    	
    iWaitDialog->SetCallback( this );	
    // Coverty fix, Forward NULL , http://ousrv057/cov.cgi?cid=35709 
    if (string)
        {
        iWaitDialog->SetTextL( *string );
        CleanupStack::PopAndDestroy( string );
        }
    iWaitResId = aResourceId;
    iWaitDialog->PrepareLC( waitNoteResource );

    AMSLOGGER_WRITE( "CAudioMessageAppUi::ShowWaitNoteL <<" );
    return iWaitDialog->RunLD();
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::RemoveWaitNote
// ---------------------------------------------------------
//
void CAudioMessageAppUi::RemoveWaitNote()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::RemoveWaitNote >>" ); 
    //This also deletes the dialog
    if ( iWaitDialog)
    	{
    	TRAP_IGNORE( iWaitDialog->ProcessFinishedL() );
    	}
    AMSLOGGER_WRITE( "CAudioMessageAppUi::RemoveWaitNote <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::SetAddressSize
// ---------------------------------------------------------
//
void CAudioMessageAppUi::SetAddressSize()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::SetAddressSize >>" ); 
    TInt entryCnt( 0 );
    TInt sizeInBytes( 0 );
    TInt tempBytes( 0 );

    if ( !iMtm->Entry().Entry().ReadOnly() )//not viewer
      	{
      	CMsgAddressControl* to = ToCtrl();
      	to->GetSizeOfAddresses( entryCnt, tempBytes );
      	sizeInBytes += tempBytes;
      	Document()->SetAddressSize( sizeInBytes );
      	}
    AMSLOGGER_WRITE( "CAudioMessageAppUi::SetAddressSize <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::SetBodySize
// ---------------------------------------------------------
//
void CAudioMessageAppUi::SetBodySize()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::SetBodySize >>" ); 
    TInt sizeInBytes( 0 );

    if (iInsertingMedia)
      	{
      	sizeInBytes = iInsertingMedia->FileSize();
      	}
    Document()->SetAudioFileSize( sizeInBytes );
    AMSLOGGER_WRITE( "CAudioMessageAppUi::SetBodySize <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::IsForward
// ---------------------------------------------------------
//
TBool CAudioMessageAppUi::IsForward() const
    {
    TMmsMsvEntry entry = static_cast<TMmsMsvEntry>( Document()->Entry() );
    return entry.IsForwardedMessage();
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::RemoveAudioL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::RemoveAudioL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::RemoveAudioL >>" ); 
  	if ( ShowConfirmationQueryL( R_AUDIOMESSAGE_QUEST_REMOVE_AUDIO ) )
    	{
    	if ( !iInsertOperation )
      		{
      		iInsertOperation = CAudioMessageInsertOperation::NewL(
            	*this,
            	*Document(),
            	*iView );
      		}
      	//In a case when phone is in silent mode and try to remove audio clip...
      	//Press msk twice extremely fast. It will take the second press as play
      	//audio and the UI is messed. Following will prevent it.
      	//Flag released when remove operation returns.		
    	iEditorFlags |= EProgressInUse;
    	iInsertOperation->Insert( iInsertingMedia, ETrue );
    	}
    AMSLOGGER_WRITE( "CAudioMessageAppUi::RemoveAudioL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::MessageSizeInBytes
// ---------------------------------------------------------
//
TInt CAudioMessageAppUi::MessageSizeInBytes()
    {
    TInt mSize = 0;
    if ( iInsertingMedia )
    	{
    	mSize = iInsertingMedia->FileSize();
    	}
    else
    	{
      	mSize = 0;
    	}
    return mSize;
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::DoSelectionKeyL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoSelectionKeyL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoSelectionKeyL >>" ); 
    CMsgBaseControl* ctrl = iView->FocusedControl();
    if ( ctrl )
     	{
        switch ( ctrl->ControlId() )
            {
            case EMsgComponentIdTo:
                if ( Document()->GetAppMode() == EAmsEditor )
                	{
                  	TBool modified = EFalse;
                  	if ( !VerifyAddressesL( modified ) )
                      	{
                      	// Illegal address found.
                      	modified = ETrue;
                      	}
                  	if ( !modified )//else
                      	{
            			// Nothing changed on the UI. Open PhoneBook.
                    	HandleCommandL( EAmsEditorCmdAddRecipient ); 
            			}
                	}
            	else
              		{
                  	MenuBar()->SetMenuTitleResourceId( R_AUDIOMESSAGE_VIEWER_SELECTMENU );
                  	MenuBar()->SetMenuType(CEikMenuBar::EMenuContext);
                  	TRAP_IGNORE(MenuBar()->TryDisplayMenuBarL());
                  	MenuBar()->SetMenuTitleResourceId( R_AUDIOMESSAGE_MENUBAR );
                  	MenuBar()->SetMenuType(CEikMenuBar::EMenuOptions);
              		}
                break;
      		default:
        		//if audio clip is inserted, just play it
        		if ( Document()->GetClipStatus() != EAmsClipNone )
        			{
        			HandleCommandL( EAmsEditorCmdPlayAudio );
        			}
        		//there is no any clip, so try to record new.
        		else
        			{
                	HandleCommandL( EAmsEditorCmdRecordAudio );
                	}
                break;
        	}
    	}
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoSelectionKeyL <<" ); 
    }
    
// ---------------------------------------------------------
// CAudioMessageAppUi::IsPhoneOfflineL
// ---------------------------------------------------------
//
TBool CAudioMessageAppUi::IsPhoneOfflineL() const
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::IsPhoneOfflineL >>" ); 
    if ( iSupportedFeatures & EUniFeatureOffline )
        {
        TInt connAllowed = 1;
        CRepository* repository = CRepository::NewL( KCRUidCoreApplicationUIs );
        TInt err = repository->Get( KCoreAppUIsNetworkConnectionAllowed, connAllowed );
        delete repository;
        repository = NULL;
        if ( !err && !connAllowed )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::LaunchHelpL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::LaunchHelpL()
    { 
    AMSLOGGER_WRITE( "CAudioMessageAppUi::LaunchHelpL >>" ); 
    // activate Help application
    if ( iSupportedFeatures & EUniFeatureHelp )
        {
        //CArrayFix<TCoeHelpContext>* helpContext = AppHelpContextL();
        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession()
            , iEikonEnv->EikAppUi()->AppHelpContextL()/* helpContext*/ ); 
        }
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::HelpContextL
// ---------------------------------------------------------
//
CArrayFix<TCoeHelpContext>* CAudioMessageAppUi::HelpContextL() const
    {
    CArrayFix<TCoeHelpContext>* r = new ( ELeave ) CArrayFixFlat<TCoeHelpContext>( 1 );
    CleanupStack::PushL( r );
    if ( Document()->GetAppMode() == EAmsViewer )
    	{
      	r->AppendL( TCoeHelpContext( KUidAudioMessageApplication, KAUDIO_MSG_HLP_VIEWER() ) );
    	}
    else // Document()->GetAppMode() == EAmsEditor 
    	{
      	r->AppendL( TCoeHelpContext( KUidAudioMessageApplication, KAUDIO_MSG_HLP_EDITOR() ) );
    	}
	
    CleanupStack::Pop( r );
    return r;
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::NumberConversionLC
// ---------------------------------------------------------
//
HBufC* CAudioMessageAppUi::NumberConversionLC( const TDesC& aOrigNumber, TBool aDirection )
    {
    HBufC* addressCopy = aOrigNumber.AllocLC();
    TPtr tempTPtr = addressCopy->Des();
    if ( aDirection )
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( tempTPtr );
        }
    else
        {
        AknTextUtils::ConvertDigitsTo( tempTPtr, EDigitTypeWestern );
        }
    return addressCopy;
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::DoInsertCompleteL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoInsertCompleteL( TBool aErrorNone )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoInsertCompleteL >>" ); 
	if( !CheckIncomingCall() )
	    {
	    RemoveWaitNote();
	    }
	else
	    {
        //There may have been fatal error in inserting
        if ( iInsertOperation && iInsertOperation->GetError()<0 )
            {
           	;//Do nothing, error note is shown elsewhere and application is closed
            }
        else
            {
            HBufC* labell = StringLoader::LoadLC( R_AUDIOMESSAGE_INSERTED );
  		    CAknGlobalNote *globalNote = CAknGlobalNote::NewLC();
	        globalNote->ShowNoteL( EAknGlobalInformationNote, *labell );
	        CleanupStack::PopAndDestroy( globalNote );
            CleanupStack::PopAndDestroy( labell );
            }     
	    }
	
   	CAknInputBlock::NewLC(); // Absorb keys
   	iAvkonAppUi->Cba()->MakeVisible( ETrue );
    iInserting = EFalse;
  	if ( aErrorNone )
    	{
	    SetBodySize();
	    Document()->SetBodyModified( ETrue );
	    Document()->SetInsertedStatus( EAmsRecorded );
	    SetAudioClipIconL( EAudioInserted );
       	iView->SetFocus( EMsgComponentIdImage );       
    	}
  	else
    	{
      	ShowInsertErrorsL();
    	}

    UpdateSoftKeysL();	
  	FetchAudioFileDataL( ) ;
   	CleanupStack::PopAndDestroy();// Absorb keys
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoInsertCompleteL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::EditorOperationEvent
// ---------------------------------------------------------
//
void CAudioMessageAppUi::EditorOperationEvent( TAmsOperationType aOperation,
  TAmsOperationEvent aEvent )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::EditorOperationEvent >>" ); 
    if ( iEditorFlags & EEditorExiting )
        {
        // Do not handle any event if we are exiting.
        return;
        }
    
    delete iAbsorber;
    iAbsorber = NULL;

    TRAPD( error, DoEditorOperationEventL( aOperation, aEvent ) );
	if (error != KErrNone)
		{
		Exit ( error );
		}
    AMSLOGGER_WRITE( "CAudioMessageAppUi::EditorOperationEvent <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::DoEditorOperationEventL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoEditorOperationEventL( TAmsOperationType aOperation,
  TAmsOperationEvent aEvent )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoEditorOperationEventL >>" ); 
    switch ( aOperation )
        {
      	case EAmsOperationInsert:
            if ( aEvent == EAmsOperationProcessing )
                {
                ShowWaitNoteL( R_QTN_AUDIOMESSAGE_WAIT_INSERTING );
                }
            else if ( aEvent == EAmsOperationError )
              	{
              	DoInsertCompleteL( EFalse );
              	}
            else
                {
                DoInsertCompleteL( ETrue );
                }
            break;
        case EAmsOperationLaunch:
            DoLaunchCompleteL( aEvent );
            break;
        case EAmsOperationSave:
            DoSaveCompleteL();
            break;
        case EAmsOperationSend:
            DoSendCompleteL();
            break;
        case EAmsOperationRemove:
            DoRemoveCompleteL();
            iEditorFlags &= ~EProgressInUse;
            break;
        case EAmsOperationSendReadReport:
            // free resources
            delete iSendReadReportOperation;
            iSendReadReportOperation = NULL;
            break;
        default:
            {
            // nothing to do
            break;
            }
        }   
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoEditorOperationEventL <<" ); 
    }  

// ---------------------------------------------------------
// CAudioMessageAppUi::DoRemoveCompleteL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoRemoveCompleteL()
  	{
  	AMSLOGGER_WRITE( "CAudioMessageAppUi::DoRemoveCompleteL >>" ); 
    CAknInputBlock::NewLC(); // Absorb keys
    Document()->SetAudioFileSize( 0 );
    Document()->SetDuration( 0 );
 	SetNaviPaneL();
    Document()->SetBodyModified( ETrue );
    Document()->SetClipStatus( EAmsClipNone );
    SetAudioClipIconL( EAudioInsert );   
  	UpdateSoftKeysL();
    ShowInformationNoteL( R_AUDIOMESSAGE_INFO_CLIP_DELETED, EFalse );
  	CleanupStack::PopAndDestroy();
  	AMSLOGGER_WRITE( "CAudioMessageAppUi::DoRemoveCompleteL <<" ); 
  	}

// ---------------------------------------------------------
// CAudioMessageAppUi::DoLaunchCompleteL
// EKLU-7LGCPT :: Filter Invalid contact details
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoLaunchCompleteL( TAmsOperationEvent aEvent )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoLaunchCompleteL >>" ); 
    TInt error = iLaunchOperation->GetError();
    RemoveWaitNote();
    
    if ( error == EAmsExit )
      	{
        DoDelayedExitL( 0 );
        }
    else if ( error != KErrNone )
    	{
    	if ( !ShowLaunchErrorsL( error  ) )
    		{
    		//In this case the error not is shown
    		DoDelayedExitL( KDelayedExitDelay );
    		}
    	else
    		{
    		DoDelayedExitL( 0 );
    		}
    	}

    else
        {
       	FinalizeLaunchL();
       	
       	// Enable task swapper to options menu after launch has been completed.
        MenuBar()->SetMenuType( CEikMenuBar::EMenuOptions );
       	
       	delete iScreenClearer;
        iScreenClearer = NULL;
        
        iEditorFlags |= ELaunchSuccessful;
        //Set softkeys

        UpdateSoftKeysL();
        //SetNaviPaneL();		
        	
        TRAP_IGNORE( SendReadReportL( aEvent ) );
        }
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoLaunchCompleteL <<" );    
    }
 
// ---------------------------------------------------------   
// CAudioMessageAppUi::DoDelayedExitL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoDelayedExitL( TInt aDelayTime )
    {
AMSLOGGER_WRITE( "CAudioMessageAppUi::DoDelayedExitL" );      
    delete iIdle;
    iIdle = 0;
    iIdle = CPeriodic::NewL( EPriorityNormal /*- 1*/);
    iIdle->Start(   aDelayTime,
                    aDelayTime,
                    TCallBack( DelayedExit, this ));
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::DelayedExit
// ---------------------------------------------------------
//
TInt CAudioMessageAppUi::DelayedExit( TAny* aThis )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DelayedExit" ); 
    CAudioMessageAppUi* editor = static_cast<CAudioMessageAppUi*>( aThis );
    editor->DoExternalExit();
  	CAknEnv::RunAppShutter( );
    return KErrNone;
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::DoExternalExit
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoExternalExit()
    {
    delete iAbsorber;
    iAbsorber=NULL;
    Exit( EAknSoftkeyClose );
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::DoSaveCompleteL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoSaveCompleteL()
    {
    EndActiveWait();
    if ( iWaitDialog )
        {
        // Processing continued at DialogDismissedL when wait note
        // has dismissed itself.
        iWaitDialog->ProcessFinishedL();
        }    
    else
        {
        DoOpComplete2ndPhaseL();
        }
    
    }
    
// ---------------------------------------------------------
// CAudioMessageAppUi::DoOpComplete2ndPhaseL
//
// Performs the real send operation completing code.
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoOpComplete2ndPhaseL()
    { 
    DoDelayedExitL( 0 );
    }    

// ---------------------------------------------------------
// CAudioMessageAppUi::DoSendCompleteL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoSendCompleteL()
    {
    if ( iWaitDialog )
        {
        // Processing continued at DialogDismissedL when wait note
        // has dismissed itself.
        iWaitDialog->ProcessFinishedL();
        }    
    else
        {
        DoOpComplete2ndPhaseL();
        }    
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::BeginActiveWait
// ---------------------------------------------------------
//
void CAudioMessageAppUi::BeginActiveWait()
    {
    if ( iWait.IsStarted() )
        {
        return;
        }
    iWait.Start();
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::EndActiveWait
// ---------------------------------------------------------
//
void CAudioMessageAppUi::EndActiveWait()
    {
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

		
// ---------------------------------------------------------
// CAudioMessageAppUi::ShowLaunchErrorsL
// ---------------------------------------------------------
//
TBool CAudioMessageAppUi::ShowLaunchErrorsL( TInt aError )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::ShowLaunchErrorsL >>" ); 
    TInt errRes = -1;
    switch ( aError )
        {
        case EAmsSendUiUnsupported:
            SetErrorResource( errRes, R_AUDIOMESSAGE_INFO_UNSUPPORTED_OBJECT );
          break;
        case EAmsSendUiCorrupted:
            SetErrorResource( errRes, R_AUDIOMESSAGE_INFO_CORRUPTED_OBJECT );
            break;
        case EAmsProtectedObject:
            SetErrorResource( errRes, R_AUDIOMESSAGE_INFO_SEND_FORBID );
            break;
        default:
            // e.g leave in startup
            break;
        }

    if ( errRes != -1 )
        {
        ShowInformationNoteL( errRes, EFalse );
    AMSLOGGER_WRITE( "CAudioMessageAppUi::ShowLaunchErrorsL, false <<" ); 
        return EFalse;
        }

    //else nothing
   AMSLOGGER_WRITE( "CAudioMessageAppUi::ShowLaunchErrorsL, true <<" ); 
    return ETrue;
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::SetErrorResource
// ---------------------------------------------------------
//
void CAudioMessageAppUi::SetErrorResource( TInt& aStoreId, TInt aNewId )
    {
    if ( ErrorPriority( aNewId ) >= ErrorPriority( aStoreId ) )
        {
        if ( aStoreId == R_AUDIOMESSAGE_INFO_CORRUPTED_OBJECT &&
            ( aNewId == R_AUDIOMESSAGE_INFO_CORRUPTED_OBJECT  ) )
            {
            //set plural
            aStoreId = R_AUDIOMESSAGE_INFO_CORRUPTED_OBJECT;
            }
        else if ( aStoreId == R_AUDIOMESSAGE_INFO_UNSUPPORTED_OBJECT &&
            ( aNewId == R_AUDIOMESSAGE_INFO_UNSUPPORTED_OBJECT ) )
            {
            //set plural
            aStoreId = R_AUDIOMESSAGE_INFO_UNSUPPORTED_OBJECT;
            }
        else if ( aStoreId != -1 && aStoreId != aNewId )
            {
            // Set "general error"
            aStoreId = aNewId;
            }
        else
            {
            aStoreId = aNewId;
            }
        }
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::ErrorPriority
// ---------------------------------------------------------
//
TInt CAudioMessageAppUi::ErrorPriority( TInt aErrorId )
    {
    TInt priority = 0;
    switch ( aErrorId )
        {
        case R_AUDIOMESSAGE_INFO_SEND_FORBID:
            priority++;
        case R_AUDIOMESSAGE_INFO_OBJECT_TOO_BIG:
            priority++;
        case R_AUDIOMESSAGE_ERROR_CANNOT_OPEN:
             priority++;
        case R_AUDIOMESSAGE_INFO_UNSUPPORTED_OBJECT:
            priority++;
        default:
            break;
        }
    return priority;
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::ShowInsertErrorsL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::ShowInsertErrorsL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::ShowInsertErrorsL >>" ); 
    TBool abortedByUser = EFalse;
    TInt errRes( -1 );
    TInt error = iInsertOperation->GetError();
    
    //Let's first check if there is degative error value.
    //In that case the runL has been leaved e.g. in case low memory
    if ( error < 0 )
        {
        iEikonEnv->HandleError( error );
        return;
        }
    
    
    switch ( error )
        {
        case EAmsInsertTooBig:
            SetErrorResource( errRes, R_AUDIOMESSAGE_INFO_OBJECT_TOO_BIG );
            break;
       	case EAmsInsertNotSupported:
            SetErrorResource( errRes, R_AUDIOMESSAGE_INFO_UNSUPPORTED_OBJECT );
            break;
    	default:
            break;
    	}
    if ( !abortedByUser && errRes != -1 )
        {
        ShowInformationNoteL( errRes, EFalse );
        }
    }


// ---------------------------------------------------------
// CAudioMessageAppUi::OpenDeliveryPopupL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::OpenDeliveryPopupL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::OpenDeliveryPopupL >>" ); 
    //These are not used. They just have to be there.
    TPckgBuf<TInt> param( 0 ); 
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );

    CMsvSingleOpWatcher* watch = CMsvSingleOpWatcher::NewLC( *Document() );
    CMsvOperation* op = Document()->MtmUi().InvokeAsyncFunctionL(
        KMtmUiFunctionDeliveryStatus,
        *selection,
        watch->iStatus,
        param ); // launch the dialog through MMSUI
    CleanupStack::Pop( watch ); 
    Document()->AddSingleOperationL( op, watch );
    CleanupStack::PopAndDestroy( selection ); // selection
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::DoMoveMessageL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoMoveMessageL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoMoveMessageL >>" ); 
    //ask folder with a dialog (from muiu)
    TMsvId target = Document()->Entry().Parent();
    HBufC* title = StringLoader::LoadLC( R_AMSVIEWER_MOVE_TEXT, iCoeEnv );
    TBool success = CMsgFolderSelectionDialog::SelectFolderL(target, *title);
    CleanupStack::PopAndDestroy( title ); // title
    if ( success )
        {
        MoveMessageEntryL( target );
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoMoveMessageL <<" ); 
        Exit( EAknSoftkeyClose );
        }
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoMoveMessageL <<" ); 
    }



// ---------------------------------------------------------
// CAudioMessageAppUi::DoReplyViaL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoReplyViaL( TInt aCase  )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoDoReplyViaL >>" ); 
     
    if ( iToolbar ) 
        {
        iToolbar->SetToolbarVisibility( EFalse, EFalse );     
        }

    //to reduce flickering during closing
	if (!(iEditorBaseFeatures & EStayInViewerAfterReply ))
        {
        iView->MakeVisible( EFalse );
        }

    switch ( aCase )
            {
            case EReplyToSender:
                ReplyL( KNewMessageFolder );  //Draft
                break;
            case EReplyToAll:
                ReplyToAllL( KNewMessageFolder );
                break;                         
            case EReplyViaMessage:
                DoDoReplyViaMessageL();
                break;  
    
            default:
                break;
            }      
            
    if (!(iEditorBaseFeatures & EStayInViewerAfterReply)) 
        {
        Exit( EAknSoftkeyClose );
        }
    if ( iToolbar ) 
        {
        iToolbar->SetToolbarVisibility( ETrue, EFalse );     
        }  
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoDoReplyViaL <<" );    
    }
    
// ---------------------------------------------------------
// CAudioMessageAppUi::DoDoReplyViaMessageL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoDoReplyViaMessageL(  )
    {
    CMessageData* msgData = CMessageData::NewLC();
    TPtrC alias = KNullDesC();

    const TPtrC details = iMtm->Entry().Entry().iDetails;

    if ( details.Length() && !iAlias )
        {
        iAlias = details.AllocL( );
        alias.Set( *iAlias );
        }
        
     const TPtrC description = iMtm->Entry().Entry().iDescription;
     TPtrC subject = KNullDesC();       

	// Add address
	const TPtrC sender = iMtm->Sender();
   	if ( sender != KNullDesC() )
		{
	    if ( iAlias )
	    	{
            msgData->AppendToAddressL( TMmsGenUtils::PureAddress ( sender ), *iAlias );
            }
        else
            {
            msgData->AppendToAddressL( TMmsGenUtils::PureAddress ( sender ) );
         	}
	 	}
	 	
	//Add Subject Header	
 	HBufC* prefixSubject = CreateSubjectPrefixStringL( iMtm->SubjectL(), ETrue );
    CleanupStack::PushL( prefixSubject ); 
    if ( prefixSubject )
        {
        msgData->SetSubjectL( prefixSubject );
        }
    else
        {
        TPtrC subjectDes = iMtm->SubjectL();
        msgData->SetSubjectL( &subjectDes );
        }
    CleanupStack::PopAndDestroy( prefixSubject ); 

    CSendUi* sendUi = CSendUi::NewLC();
    sendUi->CreateAndSendMessageL(
            	KSenduiMtmUniMessageUid,
               	msgData);
    CleanupStack::PopAndDestroy( sendUi );
    CleanupStack::PopAndDestroy( msgData );
    }    
 
// ---------------------------------------------------------
// CAudioMessageAppUi::DoForwardL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoForwardL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoForwardL >>" ); 
    if ( iToolbar ) 
        {
        iToolbar->SetToolbarVisibility( EFalse, EFalse );     
        }
    
    if (!(iEditorBaseFeatures & EStayInViewerAfterReply ))
        {
        iView->MakeVisible( EFalse );
        }
    
    ForwardL( KNewMessageFolder );
    if (!(iEditorBaseFeatures & EStayInViewerAfterReply))
	    {
	    Exit( EAknSoftkeyClose );
	    }
    if ( iToolbar ) 
        {
        iToolbar->SetToolbarVisibility( ETrue, EFalse );     
        } 
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoForwardL <<" ); 
    }


// ---------------------------------------------------------
// CAudioMessageAppUi::FetchAudioFileDataL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::FetchAudioFileDataL( )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::FetchAudioFileDataL >>" ); 
    CMsvStore* store = Document()->Mtm().Entry().ReadStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& manager = store->AttachmentManagerL();
	RFile rFile = manager.GetAttachmentFileL( Document()->iAudio );

    CleanupClosePushL( rFile );

    TInt currentSize = 0;
    rFile.Size( currentSize );
    Document()->SetAudioFileSize( currentSize );

    TDataType testType( KMsgMimeAudioAmr );
    if ( Document()->GetAppMode() == EAmsViewer && !( Document()->IsAmrL( rFile, testType ) ))
        {
        Document()->SetClipStatus( EAmsClipCorrupted ); 
    	SetAudioClipIconL( EAudioCorrupted );
        UpdateSoftKeysL(); 
        ShowInformationNoteL( R_AUDIOMESSAGE_ERROR_MSG_CORRUPT, EFalse );
        }
    
    CleanupStack::PopAndDestroy( &rFile );
    CleanupStack::PopAndDestroy( store );
    AMSLOGGER_WRITE( "CAudioMessageAppUi::FetchAudioFileDataL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::FocusedControlId
// ---------------------------------------------------------
//
TInt CAudioMessageAppUi::FocusedControlId()
    {
    TInt controlType = EMsgComponentIdNull;
    if ( iView && iView->FocusedControl() )
        {
        controlType = iView->FocusedControl()->ControlId();
        }
    return controlType;
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::IsSilentModeL
// ---------------------------------------------------------
//
TBool CAudioMessageAppUi::IsSilentModeL()
    {
    TBool result( EFalse );

    CRepository* repository = CRepository::NewLC( KCRUidProfileEngine );
    TInt defaultType( EProfileGeneralId );
    User::LeaveIfError( repository->Get( KProEngActiveProfile, defaultType ) );
    CleanupStack::PopAndDestroy( repository );

    if ( defaultType == EProfileSilentId )
        {
        result = ETrue;
        }
    return result;
    }


// ---------------------------------------------------------
// CAudioMessageAppUi::HandleWsEventL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::HandleWsEventL(const TWsEvent& aEvent,CCoeControl* aDestination)
  	{
  	const TInt systemAppWgId( 4 );
  	switch ( aEvent.Type() )
    	{
        case EEventFocusGroupChanged:
        	{
AMSLOGGER_WRITE( "CAudioMessageAppUi::HandleWsEventL: EEventFocusGroupChanged " );           	
          	TInt windowGroupId = iEikonEnv->WsSession().GetFocusWindowGroup();

          	if ( ( windowGroupId != iMyWgId  ) && ( windowGroupId != iFSWindowGroupId ) &&
              	( windowGroupId != systemAppWgId ) && (windowGroupId != iAknNfySrvUi ) )
            	if ( iProgressDlg )
            		{                                       		
AMSLOGGER_WRITE( "HandleWsEventL(): Event caused  iProgressDlg->Stop() " );            		                                      		
AMSLOGGER_WRITEF( _L("iEikonEnv->WsSession().GetFocusWindowGroup() = : %d"), windowGroupId );
              		iProgressDlg->Stop();
              		
             		// Release voice key
          			if ( iVoiceKeyHandle >= 0 )
              			{
              		    iCoeEnv->RootWin().CancelCaptureKey( iVoiceKeyHandle );
              			iVoiceKeyHandle = KAmsNotCaptured;
              			}
            		}	
          	break;
        	}

        default:
            {
            break;
            }
    	}
    CMsgEditorAppUi::HandleWsEventL( aEvent, aDestination );      	
  	}


// ---------------------------------------------------------
// CAudioMessageAppUi::SaveAudioClipL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::SaveAudioClipL()
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::SaveAudioClipL >>" ); 
    TFileName fileName;
    TDataType dataType( KMsgMimeAudioAmr );

    CMsvStore* store = Document()->Mtm().Entry().ReadStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& manager = store->AttachmentManagerL();
 
	RFile rFile = manager.GetAttachmentFileL( Document()->iAudio );

    rFile.Name(fileName);
    const TPtrC newName = fileName;
    
    
    CDocumentHandler* docHandler = CDocumentHandler::NewL( );
    CleanupStack::PushL(docHandler);
    CleanupClosePushL( rFile );

    TInt ret = docHandler->CopyL( rFile,
                            	newName,
                                dataType,
                                KEntryAttNormal );
    if ( ret != KErrNone && ret != KUserCancel )
        {
        User::LeaveIfError( ret );
        }
    CleanupStack::PopAndDestroy(3, store);
    AMSLOGGER_WRITE( "CAudioMessageAppUi::SaveAudioClipL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::ShowAddressInfoNoteL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::ShowAddressInfoNoteL( const TDesC& aAddress, const TDesC& aAlias )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::ShowAddressInfoNoteL >>" ); 
    TInt cleanupCount = 0;
    HBufC* string = NULL;
    HBufC* convertedAddress = NumberConversionLC( aAddress, ETrue );
    cleanupCount++;

    if ( aAlias.Length() == 0 )
        {
        string = StringLoader::LoadLC(
            R_AUDIOMESSAGE_INFO_FAIL_RECIPIENT_NO_ALIAS,
            *convertedAddress,
            iCoeEnv );
        cleanupCount++;
        }
    else
        {
        CDesCArrayFlat* stringArr = new ( ELeave ) CDesCArrayFlat( 2 );
        CleanupStack::PushL( stringArr );
        cleanupCount++;
        stringArr->AppendL( aAlias );    //First string
        stringArr->AppendL( *convertedAddress );  //Second string
        string = StringLoader::LoadLC(
            R_AUDIOMESSAGE_INFO_FAIL_RECIPIENT,
            *stringArr,
            iCoeEnv );
        cleanupCount++;
        }

    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *string );
    CleanupStack::PopAndDestroy( cleanupCount );  // string, (stringArr)
    AMSLOGGER_WRITE( "CAudioMessageAppUi::ShowAddressInfoNoteL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::DoCallToSenderL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DoCallToSenderL()
  	{
  	AMSLOGGER_WRITE( "CAudioMessageAppUi::DoCallToSenderL >>" ); 
    if ( !iMsgVoIPExtension )
        {
        return;
        }
  	
    TPtrC senderAddr = KNullDesC( );
    TPtrC alias = KNullDesC();
    TPtrC focusedAddr = KNullDesC( );

    senderAddr.Set( TMmsGenUtils::PureAddress( iMtm->Sender() ) );
      
    HBufC* ali = HBufC::NewL( KMaxAliasLength );
    CleanupStack::PushL( ali ); 
    TPtr aliPtr ( ali->Des() );
    
    TMmsGenUtils::GetAlias( 
        iMtm->Sender() ,
        aliPtr,
        KMaxAliasLength,
        FsSession()   ); 
    alias.Set( aliPtr );

    // find item not possible as there is no message body

    MsvUiServiceUtilitiesInternal::InternetOrVoiceCallServiceL( 
            *iMsgVoIPExtension,
            senderAddr,
            alias,
            focusedAddr,
            ETrue,
            iEikonEnv );
   CleanupStack::PopAndDestroy( ali ); 
AMSLOGGER_WRITE( "CAudioMessageAppUi::DoCallToSenderL <<" ); 
  	}

// ---------------------------------------------------------
// CAudioMessageAppUi::SetFindMode
// ---------------------------------------------------------
//
void CAudioMessageAppUi::SetFindMode( TBool aEnable )
    {
    if ( iView )
        {
        CMsgBaseControl* fromControl = iView->ControlById( EMsgComponentIdFrom );
        if ( fromControl && iMtm->Sender().Length() )
            {
            static_cast<CMsgAddressControl*>( fromControl )
                ->SetAddressFieldAutoHighlight( aEnable );
            }
        }
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::HandleNotifyInt
// ---------------------------------------------------------
//
void CAudioMessageAppUi::HandleNotifyInt( TUint32 /*aId*/, TInt aNewValue )
    {
    if ( aNewValue == 0 )
        {
        iEditorFlags &= ~EAutohighLightEnabled;
        }
    else
        {
        iEditorFlags |= EAutohighLightEnabled;
        }
    SetFindMode( iEditorFlags & EAutohighLightEnabled );
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::SetNaviPaneL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::SetNaviPaneL()
	{
   	CMsgBaseControl* ctrl = iView->FocusedControl();
    if ( Document()->GetAppMode() == EAmsEditor )
    	{
	   	if (  ctrl->ControlId() == EMsgComponentIdImage )
	       	{
	  		UpdateEditorNaviFieldL( EAmsOnlyLength );
	       	}
	   	else
	       	{
	  		UpdateEditorNaviFieldL( EAmsTextEditorAndLength );
	       	}
    	}
    else
    	{
	  	UpdateViewerNaviFieldL(  );
    	}
	}

#ifdef RD_SCALABLE_UI_V2

// -----------------------------------------------------------------------------
// handles the touch-ui related control events for next/previous message
// -----------------------------------------------------------------------------
//
void CAudioMessageAppUi::HandleNaviDecoratorEventL( TInt aEventID )     
    {
    if( IsNextMessageAvailableL( aEventID == EAknNaviDecoratorEventRightTabArrow ) )
        {
        /* no need for separate checks for right and left arrows
            because IsNextMessageAvailableL() and NextMessageL
            are called with the truth-value of the same comparison */
        NextMessageL( aEventID == EAknNaviDecoratorEventRightTabArrow );
        }
    }

#else
void CAudioMessageAppUi::HandleNaviDecoratorEventL( TInt /* aEventID */)
    {}
#endif


// ---------------------------------------------------------
// CAudioMessageAppUi::DialogDismissedL
//
// This is a bit tricky. DialogDismissedL is called when wait dialog
// is really dismissed from the screen. Wait dialog is not neccessary
// dismmissed at once the ProcessFinishedL function is called. This 
// is because of some minimum delayes etc. tied to the visibility of
// wait dialog. But this more complex than that as wait dialog can be dismissed
// by end key or escape key from the screen before the operation has really been
// completed or cancelled. This needs to be taken into account here.
// ---------------------------------------------------------
//
void CAudioMessageAppUi::DialogDismissedL( TInt dismissed )
    {
    if ( dismissed == EAknSoftkeyDone )
        {
        switch ( iWaitResId )
            {
            case R_QTN_AUDIOMESSAGE_WAIT_SAVING_MESSAGE :
            case R_QTN_AUDIOMESSAGE_WAIT_SAVING_MESSAGE_NEW:
                {
                //We are still in saving phase. Prevents opening the options menu
                if ( !iAbsorber )
                    {
                    iAbsorber = CAknInputBlock::NewLC();
        	        CleanupStack::Pop( iAbsorber );
                    }
                DoOpComplete2ndPhaseL();
                break;
                }
            case R_QTN_AUDIOMESSAGE_WAIT_INSERTING:
                {
                //There may have been fatal error in inserting
                if ( iInsertOperation && iInsertOperation->GetError()<0 )
                    {
                    ;//Do nothing, error note is shown elsewhere and application is closed
                    }
                else
                    {
                    HBufC* labell = StringLoader::LoadLC( R_AUDIOMESSAGE_INSERTED );
  		            CAknGlobalNote *globalNote = CAknGlobalNote::NewLC();
	                globalNote->ShowNoteL( EAknGlobalInformationNote, *labell );
	                CleanupStack::PopAndDestroy( globalNote );
                    CleanupStack::PopAndDestroy( labell );
                    }
                  	 break;                 
                }
            case R_QTN_AUDIOMESSAGE_WAIT_SEND_MESSAGE:
            case R_QTN_WAIT_MSG_SAVED_OUTBOX:
                {
                //We are still in sending/saving phase. Prevents opening the options menu
                if ( !iAbsorber )
                    {
                    iAbsorber = CAknInputBlock::NewLC();
        	        CleanupStack::Pop( iAbsorber );
                    }
                DoOpComplete2ndPhaseL();
                break;
                }   
            case R_QTN_AUDIOMESSAGE_WAIT_OPENING_EDITOR:
                {
                break;
                }    
            default:
                {
                break;
                }
            }
        }
    else if ( dismissed == EEikBidCancel )
        {
        iWaitDialog = NULL;
        }
    
    iWaitDialog = NULL;
    iWaitResId = -1;
    }

// -----------------------------------------------------------------------------
// Update softkeys
// Err ID: EJJN-7J3BBY :: 05/09/2008 :: Purushotham Reddy K
// -----------------------------------------------------------------------------
//
void CAudioMessageAppUi::DoUpdateSoftKeysL( TInt aSk )
	{   
    if ( iCurrentSkResId != aSk )
    	{
        CEikButtonGroupContainer* cba  = CEikButtonGroupContainer::Current();     
        if(cba)
            {
            cba->SetCommandSetL( aSk  );
            iCurrentSkResId = aSk;	
    	    cba->DrawNow();
            }
    	}
	}

// -----------------------------------------------------------------------------
// Update softkeys second version
// This can be called without knowing the application state or focused item
// -----------------------------------------------------------------------------
//
void CAudioMessageAppUi::UpdateSoftKeysL( )
	{
	CMsgBaseControl* ctrl = iView->FocusedControl();
       	        
    if ( Document()->GetAppMode() == EAmsViewer )
		{
        if ( ctrl->ControlId() == EMsgComponentIdImage)
	        {
            DoUpdateSoftKeysL( R_AUDIOMESSAGE_OPTIONS_PLAY_BACK);
	        }
	    else
	        {   
		    MenuBar()->SetContextMenuTitleResourceId(
		        R_AUDIOMESSAGE_VIEWER_SELECTMENU );

            DoUpdateSoftKeysL( R_AUDIOMESSAGE_OPTIONS_CONTEXTOPTIONS_BACK ); 	
	        }
		}
	 else  //AppMode == EAmsEditor
	    {
	    if ( ctrl->ControlId() == EMsgComponentIdImage )
	        {
            //Now we are in the body we must check if the clip has already been recorded
            if( Document()->GetClipStatus() != EAmsClipNone )
            	{
                DoUpdateSoftKeysL( R_AUDIOMESSAGE_OPTIONS_PLAY_CLOSE );
                }
            else 
             	{
                DoUpdateSoftKeysL( R_AUDIOMESSAGE_OPTIONS_RECORD_CLOSE );
                }
	        }
	    else
	        {
	        DoUpdateSoftKeysL( R_AUDIOMESSAGE_OPTIONS_ADD_CLOSE );
	        }
	    }
	}
// ---------------------------------------------------------
// CAudioMessageAppUi::DoEditMmsPriorityL
// ---------------------------------------------------------
void CAudioMessageAppUi::DoEditAmsPriorityL( )
    {
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoEditAmsPriorityL >>" ); 
    TInt currentlySelected = 0;
    switch( Document()->Mtm().MessagePriority(  ) )
        {
        case EMmsPriorityNormal:
            currentlySelected = 1;
            break;
        case EMmsPriorityLow:
            currentlySelected = 2;
            break;
        default:
            break;
        }
     
	// Create listbox    
    CAknSingleGraphicPopupMenuStyleListBox* listBox 
        = new (ELeave) CAknSingleGraphicPopupMenuStyleListBox;
    CleanupStack::PushL( listBox );
    
    // Create popup
    CAknPopupList* popup = CAknPopupList::NewL( 
                listBox, 
                R_AVKON_SOFTKEYS_SELECT_CANCEL, 
                AknPopupLayouts::EMenuGraphicWindow );
    CleanupStack::PushL( popup );

	// Construct listbox
    listBox->ConstructL( popup, CEikListBox::ELeftDownInViewRect );
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame( )->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                                      CEikScrollBarFrame::EAuto );
  
	// Set title    
    HBufC* title = StringLoader::LoadLC( R_AUDIOMESSAGE_PRIORITY_TITLE, iCoeEnv );
    popup->SetTitleL( title->Des( ) );
    CleanupStack::PopAndDestroy( title );

	CAknIconArray* iconArray = RadioButtonArrayL( );
    listBox->ItemDrawer( )->FormattedCellData( )->SetIconArray( iconArray ); // FormattedCellData owns

    listBox->HandleItemAdditionL( );

    TResourceReader reader;
    iEikonEnv->CreateResourceReaderLC( reader, R_AUDIOMESSAGE_PRIORITY_LIST );
    
    const TInt KSettingsGranularity = 4;
    CDesCArrayFlat* items = new ( ELeave ) CDesCArrayFlat( KSettingsGranularity );
    CleanupStack::PushL( items );

    // Get the labels from resources
    const TInt count = reader.ReadInt16();

    for ( TInt loop = 0; loop < count; loop++ )
        {
        HBufC* label = reader.ReadHBufCL();        
        CleanupStack::PushL( label );
        TPtr pLabel = label->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( pLabel );
        
        TBuf<20> itemString;
        if( currentlySelected == loop )
            { // This one is selected
            itemString.AppendNum( 1 );
            }
        else
            {
            itemString.AppendNum( 0 );
            }

        _LIT( KT,"\t");
        itemString.Append( KT );
        itemString.Append( pLabel );
        items->AppendL( itemString );
        
        CleanupStack::PopAndDestroy( label );
        label = NULL;
        }

    CTextListBoxModel* model = listBox->Model( );
    model->SetItemTextArray( items );
    model->SetOwnershipType( ELbmOwnsItemArray );
	CleanupStack::Pop( items ); // model owns it now  
	CleanupStack::PopAndDestroy( ); // reader 
    listBox->SetCurrentItemIndexAndDraw( currentlySelected );

    TInt result = popup->ExecuteLD();
 
    if( result )
        {
        // OK, user chose the priority bit
        switch( listBox->CurrentItemIndex( ) )
            {
            case 0:
                Document()->Mtm().SetMessagePriority( EMmsPriorityHigh );
                break;
            case 1:
                Document()->Mtm().SetMessagePriority( EMmsPriorityNormal );
                break;
            default:
                Document()->Mtm().SetMessagePriority( EMmsPriorityLow );
                break;
            }   
        }
    UpdateIndicatorIcons( (TMmsMessagePriority)Document()->Mtm().MessagePriority() );    
    Document()->SetHeaderModified( ETrue ); //This makes us save headers in case of 
                                            //Message details menu shown
    CleanupStack::Pop( popup ); 
    CleanupStack::PopAndDestroy( listBox );
    AMSLOGGER_WRITE( "CAudioMessageAppUi::DoEditAmsPriorityL <<" ); 
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::RadioButtonArrayL
// ---------------------------------------------------------
CAknIconArray* CAudioMessageAppUi::RadioButtonArrayL( )
	{
    TFileName fileName;
    fileName.Copy( KAvkonBitmapFile );

    CAknIconArray* iconArray = new (ELeave) CAknIconArray( 2 );
    CleanupStack::PushL( iconArray ); 

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    CGulIcon* icon = NULL;

    TAknsItemID skinId = KAknsIIDQgnPropRadiobuttOff;
    TInt bitmapId = EMbmAvkonQgn_prop_radiobutt_off;

    AknsUtils::CreateIconL(
        AknsUtils::SkinInstance(),
        skinId,
        bitmap,
        mask,
        fileName,
        bitmapId,
        bitmapId+1
        );
    CleanupStack::PushL( bitmap );
    CleanupStack::PushL( mask );
    icon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( mask );
    CleanupStack::Pop( bitmap );
    CleanupStack::PushL( icon ); 
    iconArray->AppendL( icon );
    CleanupStack::Pop( icon );   

    skinId = KAknsIIDQgnPropRadiobuttOn;
    bitmapId = EMbmAvkonQgn_prop_radiobutt_on;

    AknsUtils::CreateIconL(
        AknsUtils::SkinInstance(),
        skinId,
        bitmap,
        mask,
        fileName,
        bitmapId,
        bitmapId+1
        );
    CleanupStack::PushL( bitmap );
    CleanupStack::PushL( mask );
    icon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( mask );//popup mask first
    CleanupStack::Pop( bitmap );//then bitmap
    CleanupStack::PushL( icon ); 
    iconArray->AppendL( icon );
    CleanupStack::Pop( icon );   

    CleanupStack::Pop( iconArray );
    
    return iconArray;
	}

// ---------------------------------------------------------
// CAudioMessageAppUi::UpdateIndicatorIcons
//
// ---------------------------------------------------------
//
void CAudioMessageAppUi::UpdateIndicatorIcons( TMmsMessagePriority aPriority )
    {
    TInt priorityHigh;
    TInt priorityLow;

	if( aPriority == EMmsPriorityLow )
	    {
	    priorityHigh = EAknIndicatorStateOff;
	    priorityLow = EAknIndicatorStateOn;
	    }
	else if( aPriority == EMmsPriorityHigh )
	    {
	    priorityHigh = EAknIndicatorStateOn;
	    priorityLow = EAknIndicatorStateOff;
	    }
	else
	    {
	    priorityHigh = EAknIndicatorStateOff;
	    priorityLow = EAknIndicatorStateOff;
	    }

    MAknEditingStateIndicator* editIndi = iAvkonEnv->EditingStateIndicator();
        
    CAknIndicatorContainer* naviIndi = 
        static_cast<CAknIndicatorContainer*>( iNaviDecorator->DecoratedControl() );
    
    // Update the Edwin indicator pane
    if( editIndi )
        {
        CAknIndicatorContainer* indiContainer = editIndi->IndicatorContainer();
        if ( indiContainer )
            {
            indiContainer->SetIndicatorState(
                        TUid::Uid(EAknNaviPaneEditorIndicatorMcePriorityHigh), 
                        priorityHigh);
            indiContainer->SetIndicatorState(
                        TUid::Uid(EAknNaviPaneEditorIndicatorMcePriorityLow), 
                        priorityLow);
            }
        }
    // Update the navi indicator pane
    if( naviIndi )
        {
        naviIndi->SetIndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorMcePriorityHigh), 
                    priorityHigh);
        naviIndi->SetIndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorMcePriorityLow), 
                    priorityLow);
        }
    }

// -----------------------------------------------------------------------------
// SendReadReportL
// -----------------------------------------------------------------------------
//
void CAudioMessageAppUi::SendReadReportL( TAmsOperationEvent aEvent )
    {
    // send read report
    if (    aEvent == 	EAmsOperationComplete
        &&  Document( )->LaunchFlags() & EMsgUnreadMessage )
        {
        // Do not leave because of read report
        iSendReadReportOperation = new (ELeave ) CAudioMessageSendReadReportOperation(
                                                    *this,
                                                    *Document( ) );
        iSendReadReportOperation->SendReadReportL( );
        }
    }

// -----------------------------------------------------------------------------
// DoHandleLocalZoomChangeL
// -----------------------------------------------------------------------------
//
void CAudioMessageAppUi::DoHandleLocalZoomChangeL( TInt aCommand )
	{
	switch ( aCommand )
		{	
	    case EAmsDispSizeAutomatic:
		    {
		    HandleLocalZoomChangeL( EMsgDispSizeAutomatic );
		    break;
		    }
        case EAmsDispSizeLarge:
            {
            HandleLocalZoomChangeL( EMsgDispSizeLarge );
		    break;
		    }
        case EAmsDispSizeSmall:
            {
            HandleLocalZoomChangeL( EMsgDispSizeSmall );
		    break;
		    }
	    default: //EAmsDispSizeNormal
            {
            HandleLocalZoomChangeL(  EMsgDispSizeNormal );
		    break;
		    }	    
		}
	}

// -----------------------------------------------------------------------------
// UnsupportedCallTypeOngoing
// -----------------------------------------------------------------------------
//
TBool CAudioMessageAppUi::UnsupportedCallTypeOngoingL(  )
	{
	TBool unsupported( EFalse );
	TInt value( 0 );

	TInt err( 0 );
    RProperty property;
    err = property.Attach( KPSUidCtsyCallInformation, KCTsyCallState );
	if ( err )
		{
		return EFalse;
		}

    property.Get( value );

    switch ( value )
	    {
    	case EPSCTsyCallStateUninitialized:
    	case EPSCTsyCallStateNone:
    		{ // No any kind of call in progress
    		return EFalse;
    		}
		default: // Call in progress, check if it is videocall
		    {
		    err = property.Attach( KPSUidCtsyCallInformation,
        		KCTsyCallType );
        	if ( err )
        		{
        		return EFalse;
        		}
        	value = 0;
    		property.Get( value );
			break;
		    }
    	}

	switch( value )
		{
		// Video call ongoing
		case EPSCTsyCallTypeH324Multimedia:
			{
			unsupported = ETrue;
			ShowUnsupportedCallTypeNoteL( R_VR_VIDEO_CALL_INFONOTE_LABEL );
			break;			
			}
		// Voip call ongoing
		case EPSCTsyCallTypeVoIP:
			{
			unsupported = ETrue;
			ShowUnsupportedCallTypeNoteL( R_VR_INTERNET_CALL_INFONOTE_LABEL );
			break;			
			}			
		}
	
	return unsupported;
	}
	
// ---------------------------------------------------------------------------
// CAudioMessageAppUi::ShowUnsupportedCallTypeNoteL
// 
// ---------------------------------------------------------------------------
//
void CAudioMessageAppUi::ShowUnsupportedCallTypeNoteL( TInt aResourceId )
    {
    CAknInformationNote* infoNote;
    HBufC* noteText;
    
    noteText = CEikonEnv::Static()->
        AllocReadResourceLC( aResourceId );
    
    infoNote = new( ELeave ) CAknInformationNote( ETrue );
    infoNote->ExecuteLD( *noteText );

    CleanupStack::PopAndDestroy( noteText );
    }

// ---------------------------------------------------------------------------
// CAudioMessageAppUi::CheckIncomingCall
// 
// ---------------------------------------------------------------------------
//
TBool CAudioMessageAppUi::CheckIncomingCall(  )
    {
	TInt value( 0 );
	TInt err( 0 );
	RProperty property;
	err  = property.Attach( KPSUidCtsyCallInformation, KCTsyCallState );
	if ( err )
		{
		return EFalse;
		}
	property.Get( value );

    switch ( value )
        {
		case EPSCTsyCallStateUninitialized:
		case EPSCTsyCallStateNone:
			{ // No any kind of call in progress
			return EFalse;
			}
		default: // Call in progress, 
	    	{
			//we can return from here, as we found call in progress
			return ETrue;
	    	}
        }
    }
// ---------------------------------------------------------
// CAudioMessageAppUi::HandleIteratorEventL
// ---------------------------------------------------------
//
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
void CAudioMessageAppUi::HandleIteratorEventL( TMessageIteratorEvent aEvent )
    {
    if ( aEvent == EFolderCountChanged  )
        {
        SetNaviPaneL();
        }
    }
#endif // RD_MSG_NAVIPANE_IMPROVEMENT


// ---------------------------------------------------------
// DynInitToolbarL
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CAudioMessageAppUi::DynInitToolbarL(  TInt            /*aResourceId*/, 
                                        CAknToolbar*    /*aToolbar*/ )
    {
    }
#endif        
// ---------------------------------------------------------
// OfferToolbarEventL
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CAudioMessageAppUi::OfferToolbarEventL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAmsViewerToolbarReply:
            if ( IsOwnMessage( ) )
                {
                HandleCommandL( EAmsViewerCmdReplyToAll );
                }
            else
                {
                HandleCommandL( EAmsViewerCmdReplyToSender );
                }
            
            break;

        case EAmsViewerToolbarForward:
            HandleCommandL( EAmsViewerCmdForward );
            break;

        case EAmsViewerToolbarDelete:
            HandleCommandL( EAmsViewerCmdRemoveAudio );
            break;
            
        case EAmsEditorToolbarSend:
            HandleCommandL( EAmsEditorCmdSend );
            break;
            
        case EAmsEditorToolbarRecord:
            HandleCommandL( EAmsEditorCmdRecordAudio );
            break;
            
        case EAmsEditorToolbarAddRecipient:
            HandleCommandL( EAmsEditorCmdAddRecipient );
            break; 
                   
        default:
            break;

       
        }
    } 

// ---------------------------------------------------------
// SetEditorToolBarItemVisibilities
// ---------------------------------------------------------
//
void CAudioMessageAppUi::SetEditorToolBarItemVisibilities()
    {
    /*TBool sendDimming( EFalse );
    TBool recordDimming( EFalse );
    TBool AddRecipientDimming( EFalse );*/
    }

// ---------------------------------------------------------
// SetViewerToolBarItemVisibilities
// ---------------------------------------------------------
//
void CAudioMessageAppUi::SetViewerToolBarItemVisibilities()
    {
    TBool replyDimming( EFalse );
    
    if ( iToolbar )
        {                
        if ( IsOwnMessage( ) || !HasSender( ) )
            {
            replyDimming = ETrue;
            }

        iToolbar->SetItemDimmed(    EAmsViewerToolbarReply,
                                    replyDimming,
                                    ETrue );
        }
    }
    
// ---------------------------------------------------------
// CAudioMessageAppUi::IsOwnMessage
// ---------------------------------------------------------
//
TBool CAudioMessageAppUi::IsOwnMessage( ) const
    {
    const TMmsMsvEntry* mmsEntry =
        static_cast<const TMmsMsvEntry*>( &Document( )->Entry( ) );
    return ( !(mmsEntry->IsMobileTerminated( ) ) );
    }    
    
// ---------------------------------------------------------
// CAudioMessageAppUi::HasSender
// ---------------------------------------------------------
//
TBool CAudioMessageAppUi::HasSender( ) const
    {
    return ( iMtm->Sender( ).Length( ) > 0 );
    }    
#endif

// ---------------------------------------------------------
// CAudioMessageAppUi::HandleResourceChangeL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::HandleResourceChangeL( TInt aType )
    {
     // Base class call must be first
    CMsgEditorAppUi::HandleResourceChangeL(aType);
    
    if ( aType == KAknsMessageSkinChange )
        {
        if ( iAppIcon )
            {
            delete iAppIcon;
            iAppIcon = NULL;
            }
    
        // Set path of bitmap file
  	    TParse fileParse;
  	    fileParse.Set( KAmsBitmapFileName, &KDC_APP_BITMAP_DIR, NULL );
    
        iAppIcon = AknsUtils::CreateGulIconL( 
            AknsUtils::SkinInstance(), 
            KAknsIIDQgnPropMceAudioTitle,
            fileParse.FullName(),
            EMbmAudiomessageQgn_prop_mce_audio_title,
            EMbmAudiomessageQgn_prop_mce_audio_title_mask );
        SetTitleIconL();   
        }
    else if ( aType == KEikDynamicLayoutVariantSwitch && Document()->IsLaunched() )
        {
        //The following does not change the icon but scales it
        SetAudioClipIconL( Document()->GetRecordIconStatus() );  
        
        if ( !iMtm->Entry().Entry().ReadOnly() ) // editor
    	    {
    	    SetFixedToolbarL( R_AMSEDITOR_APP_TOOLBAR );      	        
    	    }
    	else
    	    {
    	    SetFixedToolbarL( R_AMSVIEWER_APP_TOOLBAR );
    	    SetViewerToolBarItemVisibilities();
    	    }    
        SetTitleIconL();   
        }
     
    }


// ---------------------------------------------------------
// CAudioMessageAppUi::SetTitleIconL
// ---------------------------------------------------------
//
void CAudioMessageAppUi::SetTitleIconL()
    {
    SetTitleIconSize();
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
// CAudioMessageAppUi::SetTitleIconSize
//
// Sets the correct size from LAF for title icon
// ---------------------------------------------------------
//
void CAudioMessageAppUi::SetTitleIconSize()
    {
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::ETitlePane, mainPane );
    TAknLayoutRect titleIconPaneLayoutRect;
    
    if ( AknStatuspaneUtils::StaconPaneActive() )
        {
        titleIconPaneLayoutRect.LayoutRect( 
            mainPane,
            AknLayoutScalable_Avkon::title_pane_stacon_g1( 0 ).LayoutLine() );
        }
    else
        {
        titleIconPaneLayoutRect.LayoutRect( 
            mainPane,
            AknLayoutScalable_Avkon::title_pane_g2( 0 ).LayoutLine() );
        }
    
    TSize iconSize = titleIconPaneLayoutRect.Rect().Size();
    AknIconUtils::SetSize( iAppIcon->Bitmap(), iconSize, EAspectRatioPreserved );
    }    
    
// ---------------------------------------------------------
// CAudioMessageAppUi::DoEnterKeyL
// ---------------------------------------------------------
//
TBool CAudioMessageAppUi::DoEnterKeyL()
    {
    TBool result( EFalse );
    CMsgBaseControl* ctrl = iView->FocusedControl();

    if ( ctrl )
        {
        switch ( ctrl->ControlId() )
            {
            case EMsgComponentIdAudio:
            case EMsgComponentIdImage:
                {
                DoSelectionKeyL();
                result = ETrue;
                break;
                }
                
            case EMsgComponentIdFrom:
                {
                MenuBar()->SetMenuTitleResourceId( R_AUDIOMESSAGE_VIEWER_SELECTMENU );
                MenuBar()->SetMenuType(CEikMenuBar::EMenuContext);
                TRAP_IGNORE(MenuBar()->TryDisplayMenuBarL());
                MenuBar()->SetMenuTitleResourceId( R_AUDIOMESSAGE_MENUBAR );
                MenuBar()->SetMenuType(CEikMenuBar::EMenuOptions);
                result = ETrue;
                break;
                }
                
            default:
                {
                break;
                }
            }
        }
    
    return result;
    }

// ---------------------------------------------------------
// CAudioMessageAppUi::CheckFindItemHighlightL
// ---------------------------------------------------------
//
TBool CAudioMessageAppUi::CheckFindItemHighlightL( 
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {   
    TKeyResponse keyResp = EKeyWasNotConsumed;
    CMsgExpandableControl* ctrl = NULL;
    TBool checkHighlight = EFalse;
    
    if( iView->FocusedControl() )
        {
        ctrl = static_cast<CMsgExpandableControl*>( iView->FocusedControl() );
        if ( ctrl )
        	{	
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
        }
    
    if ( ctrl && checkHighlight )
        {
        // Check if highlight needs to be restored to editor,
        // address/body editor offerkeyevent will handle it
        keyResp = ctrl->Editor().OfferKeyEventL( aKeyEvent, aType );
        }    

    return ( keyResp == EKeyWasConsumed ? ETrue : EFalse );  
    }
