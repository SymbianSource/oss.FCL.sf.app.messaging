/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains the application launch oper. functions. 
*  				 Prepare body icon. Inserts smil if aplication 
*				 is launched from the gallery.
*
*/



#include <AknsConstants.h>
#include <data_caging_path_literals.hrh> // KDC_APP_BITMAP_DIR
#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <cmsvmimeheaders.h>
#include <StringLoader.h>	
#include <MsgEditorCommon.h>
#include <AknUtils.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <msgimagecontrol.h> 
#include <MsgMimeTypes.h>
#include <MsgMediaResolver.h>
#include <MsgMediaInfo.h>
#include "msgtextutils.h"

#include <audiomessage.rsg>
#include <audiomessage.mbg> 
#include "audiomessagelaunchoperation.h"
#include "audiomessagerecorder.h"
#include "audiomessage.hrh"

_LIT( KAmsMbmFile, "audiomessage.mif" );

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------
// CAudioMessageLaunchOperation::NewL
// ---------------------------------------------------------
//
CAudioMessageLaunchOperation* CAudioMessageLaunchOperation::NewL(
        MAmsOperationObserver& aOperationObserver,
        CAudioMessageDocument& aDocument,
        CMsgEditorView& aView )
    {
    CAudioMessageLaunchOperation* self = 
        new ( ELeave ) CAudioMessageLaunchOperation( aOperationObserver, aDocument, aView );
    return self;
    }

// ---------------------------------------------------------
// CAudioMessageLaunchOperation::Constructor
// ---------------------------------------------------------
//
CAudioMessageLaunchOperation::CAudioMessageLaunchOperation(
        MAmsOperationObserver& aOperationObserver,
        CAudioMessageDocument& aDocument,
        CMsgEditorView& aView ) :
    CAudioMessageOperation( aOperationObserver, aDocument ),
    iView( aView )
    {
    }

// ---------------------------------------------------------
// CAudioMessageLaunchOperation::Destructor
// ---------------------------------------------------------
//
CAudioMessageLaunchOperation::~CAudioMessageLaunchOperation()
    {
    Cancel();
    }

// ---------------------------------------------------------
// CAudioMessageLaunchOperation::Launch
// ---------------------------------------------------------
//
void CAudioMessageLaunchOperation::Launch()
    {
    ResetError();
    iLaunchState = EAmsLaunchInitialize;
    CompleteSelf( KErrNone );
    }
 
// ---------------------------------------------------------
// CAudioMessageLaunchOperation::DoLaunchStepL
// ---------------------------------------------------------
//
void CAudioMessageLaunchOperation::DoLaunchStepL()
    {
    switch ( iLaunchState )
        {
        case EAmsLaunchInitialize:
            DoInitializeL();
            break;
        case EAmsCheckAttach:
   			CheckAttasL( );
			CompleteSelf( KErrNone );
			break;
        case EAmsHandleAudio:
        	DoHandleAudioL();
        	break;
        case EAmsLaunchPrepareContent:
           	DoPrepareContentL();
          	CompleteSelf( KErrNone );
            break;
        case EAmsLaunchAddNeededSmil:
           	if ( !SmilExists() )
           		{
           		AddSmilL();
           		}
        	CompleteSelf( KErrNone );
            break;
        case EAmsLaunchUnInitialize:
            UnInit();
    		CompleteSelf( KErrNone );
            break;
        case EAmsLaunchPrepareBody:
            DoPrepareBodyL();
            break;
        case EAmsLaunchEnd:
            iObserver.EditorOperationEvent(
                EAmsOperationLaunch,
                EAmsOperationComplete ); 
            break;
		case EAmsLaunchErrorReport:
			UnInit();
			iObserver.EditorOperationEvent(
				EAmsOperationLaunch,
				EAmsOperationError );
			break;
        default:
            iObserver.EditorOperationEvent(
                EAmsOperationLaunch,
                EAmsOperationError ); 
            break;
        }
    iLaunchState++;
    }

// ---------------------------------------------------------
// CAudioMessageLaunchOperation::DoInitializeL
// ---------------------------------------------------------
//
void CAudioMessageLaunchOperation::DoInitializeL()
    {
    if ( iDocument.GetAppMode() == EAmsViewer )
    	{
    	DoReadInitL();
    	}
    else
    	{
    	DoEditInitL();
    	}
   	iDocument.InitializeL( );
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CAudioMessageLaunchOperation::DoPrepareBodyL
// ---------------------------------------------------------
//
void CAudioMessageLaunchOperation::DoPrepareBodyL()
    {
   	if ( iDocument.GetClipStatus() == EAmsClipAttachment )
   		{
       	DoPrepareBodyL( EAudioInserted );
   		}
   	else if ( iDocument.GetClipStatus() == EAmsClipCorrupted )
   		{
   		DoPrepareBodyL( EAudioCorrupted );
   		}
   	else
   		{
   		DoPrepareBodyL( EAudioInsert );
   		}
    CompleteSelf( KErrNone );
    }
    
// ---------------------------------------------------------------------------
// CAudioMessageLaunchOperation::DoPrepareBodyL
// Prepares voice clip icon (insert/inserted) according to status of the clip
// ---------------------------------------------------------------------------
//
void CAudioMessageLaunchOperation::DoPrepareBodyL( TAmsRecordIconStatus aType )
    {
  	TParse fileParse;
    fileParse.Set( KAmsMbmFile, &KDC_APP_BITMAP_DIR, NULL );
    TAknsItemID id;
    TInt icon;
    TInt mask;
    iDocument.SetRecordIconStatus( aType );
    if ( aType == EAudioInserted && iDocument.Entry().iError != KErrCorrupt )
    	{
    	id = KAknsIIDQgnGrafMmsAudioInserted;
        icon = EMbmAudiomessageQgn_graf_mms_audio_inserted;
        mask = EMbmAudiomessageQgn_graf_mms_audio_inserted_mask;
    	}
    else if ( aType == EAudioInsert && iDocument.Entry().iError != KErrCorrupt )
    	{
    	id = KAknsIIDQgnGrafMmsAudioInsert;
        icon = EMbmAudiomessageQgn_graf_mms_audio_insert;
        mask = EMbmAudiomessageQgn_graf_mms_audio_insert_mask;
    	}
    else if ( aType == EAudioCorrupted && iDocument.Entry().iError != KErrCorrupt  )
    	{
    	//Create a copy TmsvEntry based on returned const ref
    	TMsvEntry modifiableEntry = iDocument.Entry();
    	modifiableEntry.iError = KErrCorrupt; 
    	iDocument.CurrentEntry().ChangeL( modifiableEntry );
    	
    	id = KAknsIIDQgnGrafMmsAudioCorrupted;
    	icon = EMbmAudiomessageQgn_graf_mms_audio_corrupted;
        mask = EMbmAudiomessageQgn_graf_mms_audio_corrupted_mask; 
    	}
    else  
    	{
    	id = KAknsIIDQgnGrafMmsAudioCorrupted;
    	icon = EMbmAudiomessageQgn_graf_mms_audio_corrupted;
        mask = EMbmAudiomessageQgn_graf_mms_audio_corrupted_mask; 
    	}
    	

    TAknLayoutRect iconLayout;
    iconLayout.LayoutRect(
        MsgEditorCommons::MsgDataPane(),
        AknLayoutScalable_Apps::msg_data_pane_g7().LayoutLine() );

    if ( !iImageControl )
    	{
    	iImageControl = CMsgImageControl::NewL( iView, NULL );
    	iImageControl->LoadIconL( fileParse.FullName(), id, icon, mask );
    	iImageControl->SetControlId( EMsgComponentIdImage );
        iImageControl->SetIconSizeL( iconLayout.Rect().Size() );
        iImageControl->SetIconVisible( ETrue );
    	iView.AddControlL( iImageControl, EMsgComponentIdImage, EMsgFirstControl, EMsgBody );
  		}
  	else
    	{
    	iImageControl->LoadIconL( fileParse.FullName(), id, icon, mask );
        iImageControl->SetIconSizeL( iconLayout.Rect().Size() );
        iImageControl->SetIconVisible( ETrue );
        iImageControl->DrawDeferred();
        }
    }

// ---------------------------------------------------------
// CAudioMessageLaunchOperation::DoCancel
// ---------------------------------------------------------
//
void CAudioMessageLaunchOperation::DoCancel()
    {
    }

// ---------------------------------------------------------
// CAudioMessageLaunchOperation::RunL
// ---------------------------------------------------------
//
void CAudioMessageLaunchOperation::RunL()
    {
    if ( iStatus.Int() != KErrNone )
        {
        // if EAmsExit, next error code is reason to exit
        SetError( iStatus.Int() );
        iObserver.EditorOperationEvent(
            EAmsOperationLaunch,
            EAmsOperationError );
        }
    else
	    {
	    DoLaunchStepL();
	    }
    }

// ---------------------------------------------------------
// CAudioMessageLaunchOperation::DoPrepareContentL
// ---------------------------------------------------------
//
void CAudioMessageLaunchOperation::DoPrepareContentL()
	{
 	SetSmilExists(ETrue);
    TInt attaCount = iManager->AttachmentCount();
    if ( attaCount == 1 )
    	if ( iDocument.GetAppMode() != EAmsViewer )
    	{
 			SetSmilExists(EFalse);
    		iDocument.SetInsertedStatus( EAmsSmilAdded );
    	}

	if( attaCount > 0 )
    	{
		RFile attaFile = iManager->GetAttachmentFileL( iDocument.iAudio );
   		CleanupClosePushL( attaFile ); // RFile must be closed
   		//We must get the duration for the SMIL
   		CAudioMessageRecorder *recorder = CAudioMessageRecorder::NewL( attaFile, EFalse );
        iDocument.SetDuration( recorder->GetDuration() );
        delete recorder;
  	    recorder = NULL;
	
       	if ( !SmilExists() ) 
       		{
  		    attaFile.Name( iContentLocationFileName );
  			TParsePtrC newName( iContentLocationFileName );
  			CMsvAttachment* atta = iManager->GetAttachmentInfoL( iDocument.iAudio );
    		CleanupStack::PushL( atta );	
    		CMsvMimeHeaders* msvMime = CMsvMimeHeaders::NewLC();
    		msvMime->RestoreL( *atta );
	        //Now the filename still may contains дце etc
	        msvMime->SetSuggestedFilenameL( newName.NameAndExt()  );
	        
            //Replace дце to _ , real name not needed anymore
	        CMsgTextUtils::TrimAndRemoveNonAlphaDigit( iContentLocationFileName );
	        
	        msvMime->SetContentLocationL( iContentLocationFileName );
	        
	        msvMime->StoreL( *atta );
            CleanupStack::Pop( msvMime );
            CleanupStack::Pop( atta );
            CleanupStack::PushL( msvMime );
            iManagerSync->ModifyAttachmentInfoL( atta );
	        MakeCommitL( );
	        CleanupStack::PopAndDestroy( msvMime ); 
       		} 
		iDocument.SetClipStatus( EAmsClipAttachment );
    	CleanupStack::PopAndDestroy( &attaFile );
		}
	}



// ---------------------------------------------------------
// CAudioMessageLaunchOperation::SmilExists
// ---------------------------------------------------------
//
TBool CAudioMessageLaunchOperation::SmilExists()
	{
    return iSmilExist;
    }
    
// ---------------------------------------------------------
// CAudioMessageLaunchOperation::SetSmilExists
// ---------------------------------------------------------
//
void CAudioMessageLaunchOperation::SetSmilExists( TBool aSel )
    {
    iSmilExist = aSel;
    }


// ---------------------------------------------------------
// CAudioMessageLaunchOperation::DoHandleAudioL
// ---------------------------------------------------------
//
void CAudioMessageLaunchOperation::DoHandleAudioL( )
    {
    if ( iDocument.iAudio != 0 )
        {
        if( iDocument.MessageType( ) == EAmsSendAs )
        	{ // SendAs -> Lets see what's inside
			RFile audioFile = iManager->GetAttachmentFileL( iDocument.iAudio );
            CleanupClosePushL( audioFile );

		 	CMsgMediaInfo* attaInfo = iDocument.MediaResolver()->CreateMediaInfoL( audioFile );
            iDocument.MediaResolver()->ParseInfoDetailsL( attaInfo, audioFile );
            
            if ( attaInfo->Protection() )
                {
                CleanupStack::PopAndDestroy( &audioFile );
		        SetError( EAmsProtectedObject );
		        iLaunchState = EAmsLaunchErrorReport;
				CompleteSelf( -1 );
				return;
                }
            else if ( ( attaInfo->FileSize( ) > iDocument.MaxMessageSize() ) ||
                 ( attaInfo->MimeType( ).CompareF( KMsgMimeAudioAmr ) != 0  ) )
                {
                CleanupStack::PopAndDestroy( &audioFile );
		        SetError( EAmsSendUiUnsupported );
		        iLaunchState = EAmsLaunchErrorReport;
				CompleteSelf( -1 );
				return;		    	    
                }
            else if ( attaInfo->Corrupt() )
                {
                CleanupStack::PopAndDestroy( &audioFile );
		        SetError( EAmsSendUiCorrupted );
		        iLaunchState = EAmsLaunchErrorReport;
				CompleteSelf( -1 );
				return;
                }
            
            CleanupStack::PopAndDestroy( &audioFile );
		    delete attaInfo;
        	}
    	}
        CompleteSelf( KErrNone );
	}
	
/**
* gets the framsize
*/	
TSize CAudioMessageLaunchOperation::FrameSize()
    {
    return iImageControl->FrameSize();
    }
    

