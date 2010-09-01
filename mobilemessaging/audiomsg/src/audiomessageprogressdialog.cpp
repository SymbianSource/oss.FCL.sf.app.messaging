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
* Description:   Voice progress dialog. Progress bar for recording and playing.
*  				 Output routing feature for playing. 
*
*/



#include <StringLoader.h>
#include <eikprogi.h>   
#include <audiomessage.mbg>
#include <data_caging_path_literals.hrh>
#include <aknnotewrappers.h>   
#include <aknsoundsystem.h>
#include <akninputblock.h> 
#include <AknNaviObserver.h> 
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>
#include <aknconsts.h> // KAknStandardKeyboardRepeatRate
#include <featmgr.h>    
#include <PSVariables.h>   // Property values
#include <ctsydomainpskeys.h>
#include <e32property.h> 
#include <ScreensaverInternalPSKeys.h>
#include <audiomessage.rsg>
#include <AknVolumePopup.h>
#include "audiomessagerecorder.h"
#include "audiomessage.hrh"
#include "audiomessageprogressdialog.h"
#include "amsvolobserver.h"
#include "AudioMessageLogging.h"
 
#ifdef RD_SCALABLE_UI_V2
#include <cmsvmimeheaders.h>
#include "audiomessageappui.h"
#endif

_LIT( KAmsBitmapFileName, "audiomessage.mif" );

const TInt KVolumeFirstChangePeriod = KAknStandardKeyboardRepeatRate;
const TInt KVolumeChangePeriod = KAknStandardKeyboardRepeatRate;
const TInt KVolumeChangeUp = 1;      
const TInt KVolumeChangeDown = -1;   
const TInt KSecondAsMicroSeconds = 1000000;
#ifdef __SERIES60_30__
const TInt KMinVol = 1;
#else
const TInt KMinVol = 0;
#endif
const TInt KMaxVol = 10;
const TInt KFive = 5;
const TInt KSeed = 1;
const TUint KMaxMessageDurationInSeconds = 180;
const TUint KMaxMessageDurationInSecondsSmall = 60;
const TUint KBitsInByte = 8;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
// CAudioMessageProgressDialog::Constructor
// ---------------------------------------------------------
//
CAudioMessageProgressDialog::CAudioMessageProgressDialog( CAudioMessageDocument& aDocument ): 
	CAknProgressDialog( REINTERPRET_CAST( CEikDialog**, NULL ) ),
	iDocument( aDocument )
    {
    }

// ---------------------------------------------------------
// CAudioMessageProgressDialog::NewL
// ---------------------------------------------------------
//
CAudioMessageProgressDialog* CAudioMessageProgressDialog::NewL(
        const RFile& aFile,
		TInt aVolume,
		TBool aSpeakerEarpiece,
		TBool aRecorder,
		CAudioMessageDocument& aDocument )
	{
	CAudioMessageProgressDialog * dlg = 
		new( ELeave ) CAudioMessageProgressDialog( aDocument );
	CleanupStack::PushL( dlg );
	dlg->ConstructL( aFile, aVolume, aSpeakerEarpiece, aRecorder );
	CleanupStack::Pop( dlg );
	return dlg;	
	}
	
// ---------------------------------------------------------
// CAudioMessageProgressDialog::Destructor
// ---------------------------------------------------------
//
CAudioMessageProgressDialog::~CAudioMessageProgressDialog()
	{
    // Allow screensaver again
    RProperty::Set(KPSUidScreenSaver, KScreenSaverAllowScreenSaver, 0);

    if (iVolumeTimer)
    	{
        iVolumeTimer->Cancel();
        delete iVolumeTimer;
    	}
    if ( iVolumePopup )	
        {
        iVolumePopup->CloseVolumePopup();
        delete iVolumePopup;
        }
        
    delete iRecorder;
    delete iInterfaceSelector;     	 	
   	delete iTimeDuratBase;
  	delete iTextRecording;
  	delete iTextPlaying;
  	delete iTextPaused;
  	
  	delete iLengthMax;  
	delete iLengthNow;  
	}

// ---------------------------------------------------------
// CAudioMessageProgressDialog::ConstructL
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::ConstructL( const RFile& aFile, 
				TInt aVolume ,TBool aSpeakerEarpiece, TBool aRecorder)
	{
	iFlags = 0;
   	iFinalValue = 0;
    iIncrement = KSeed;
    iVisibilityDelayOff = ETrue;
    iCurrentVolume = aVolume;
    if ( aRecorder )
    	{
		iFlags |= EAmsWorkingModeRecorder;
    	}
  	iTimeDuratBase = StringLoader::LoadL( R_QTN_TIME_DURAT_MIN_SEC );
  	iTextRecording = StringLoader::LoadL( R_AUDIOMESSAGE_PROGRESS_RECORDING );
  	iTextPlaying = StringLoader::LoadL( R_AUDIOMESSAGE_PROGRESS_PLAYING );
  	iTextPaused = StringLoader::LoadL( R_AUDIOMESSAGE_PROGRESS_PAUSED );

    // Get supported features from feature manager.
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdAccessoryFw ) )
		{
		iFlags |= EAmsSupportAudioOutput;
		}

   	FeatureManager::UnInitializeLib();
  
    iRecorder = CAudioMessageRecorder::NewL( aFile, ( iFlags &
     			EAmsSupportAudioOutput ) );
    iRecorder->SetObserver( this );
    iRecorder->SetVol( aVolume );
    
	if ( aSpeakerEarpiece )
		{
		iFlags |= EAmsAudioOutputEarpiece;
		}

    iInterfaceSelector = CRemConInterfaceSelector::NewL();

    // owned by iInterfaceSelector
    iCoreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector, *this ); 
   	iInterfaceSelector->OpenTargetL();

    // Remote control server command repeat timer.
    iVolumeTimer = CPeriodic::NewL( EPriorityNormal );
	
	

	
	// Do not allow screensaver
    RProperty::Set(KPSUidScreenSaver, KScreenSaverAllowScreenSaver, 1);
	}

// ---------------------------------------------------------
// CAudioMessageProgressDialog::PreLayoutDynInitL
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::PreLayoutDynInitL()
	{
AMSLOGGER_WRITE( "CAudioMessageProgressDialog::PreLayoutDynInitL()" );	
	//MakeVisible( EFalse );   
    RProperty property;  
    TInt callState;
	CAknProgressDialog::PreLayoutDynInitL();
 	TInt finalValue = 0;
	TInt64 interval = 0;
	iProgressInfo = GetProgressInfoL();
	iTickCount = 0;
	
	if ( iFlags & EAmsWorkingModeRecorder ) //rec
		{
	   	
	   	//Final value should be 3 min but it may be to big if message size is limited e.g. to 
	   	//100kB.
	    TInt dataRate(0);
	    dataRate=iRecorder->GetBitRateL();
	    if ( dataRate == 0)
	        {
	        User::Leave( KErrGeneral );
	        }
	    
	    iFinalValue = ( iMaxMessageSize*KBitsInByte ) / dataRate;
	   	if( iFinalValue > KMaxMessageDurationInSeconds )
	   	    {
	   	    iFinalValue = KMaxMessageDurationInSeconds;    
	   	    }
	   	else // iFinalValue < KMaxMessageDurationInSeconds
	   	    {
	   	    iFinalValue = KMaxMessageDurationInSecondsSmall;
	   	    }
	
	   	iProgressInfo->SetFinalValue( (( iFinalValue  ) * KFive)-KFive*3 );
		iMaxDurationMicroSec = (( iFinalValue ) * KSecondAsMicroSeconds );
 		SetProgressIconL( EMbmAudiomessageQgn_indi_mms_audio_record, 
 			EMbmAudiomessageQgn_indi_mms_audio_record_mask );
		UpdateProgressTextL( R_AUDIOMESSAGE_PROGRESS_RECORDING, 0 );
 
AMSLOGGER_WRITE( "CAudioMessageProgressDialog::PreLayoutDynInitL() done" );		
		}
	else //play
		{
        
		User::LeaveIfError( property.Attach(KPSUidCtsyCallInformation, KCTsyCallState) );
        property.Get( callState );
   		property.Close();
   		
		iMaxDurationMicroSec = iRecorder->GetDuration();
	    interval = iMaxDurationMicroSec.Int64();
		interval = interval / KSecondAsMicroSeconds;
	 	finalValue = ( ( I64INT( interval ) )  );
	 	iFinalValue = finalValue;
   		iProgressInfo->SetFinalValue(( ( finalValue ) * KFive));
       	// we don't want enable audiorouting if call in progress    
   		if ( ( iFlags & EAmsSupportAudioOutput ) && ( callState == EPSCTsyCallStateNone ) )
   			{
   			iRecorder->InitializeAudioRoutingL();
			if ( iFlags & EAmsAudioOutputEarpiece )
				{
				ToEarpiece();
				}
			else
				{
				ToLoudspeaker();
				}
   			}
   		else
   			{
			iNaviVolObserver->SetSpeakerOutput( iFlags & EAmsAudioOutputEarpiece  );
   			SetSoftkeys( R_SOFTKEYS_TO_EMPTY_STOP );
   			}
   		
	 	SetProgressIconL( EMbmAudiomessageQgn_indi_mms_audio_play, 
	 		EMbmAudiomessageQgn_indi_mms_audio_play_mask );
		UpdateProgressTextL( R_AUDIOMESSAGE_PROGRESS_PLAYING, 0 );
        
		
		}
	}

// ---------------------------------------------------------
// CAudioMessageProgressDialog::PostLayoutDynInitL
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::PostLayoutDynInitL()
   {
   	if ( iFlags & EAmsWorkingModeRecorder ) //rec
		{
        iRecorder->StartRecordingL();
		}
	else
	    {	    
        iRecorder->StartPlayingL();
	    //Select NULL as a owning control because the the popuu will be in correct place
        if ( !iVolumePopup )
            {
            iVolumePopup = CAknVolumePopup::NewL( NULL, ETrue);
            iVolumePopup->SetObserver(this);
            iVolumePopup->SetRange( KMinVol, KMaxVol);
            iVolumePopup->SetValue( iNaviVolObserver->GetVolume() );	
            }
	    iVolumePopup->ShowVolumePopupL();
	    }
   }
	
// ---------------------------------------------------------
// CAudioMessageProgressDialog::Stop
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::Stop()
	{
	if( !(iFlags & EAmsPause ) )
		{
		iFlags |= EAmsWsEventStop;
		}
	}

// ---------------------------------------------------------
// CAudioMessageProgressDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CAudioMessageProgressDialog::OkToExitL( TInt aButtonId )
	{
    if ( ( iTickCount < KSeed ) && ( !(iFlags & EAmsSetExit ) ) )
    	{
       	return EFalse;
    	}

	CAknInputBlock* commandAbs = CAknInputBlock::NewLC();
    	
   	TBool okToExit = EFalse;
      
    switch ( aButtonId )
    	{
    	case EAmsFileEnd:
    	    //Added delay(0.5 secs) to ensure the progress display(playTime:totalTime) is shown completely
    	    User::After(500000);
    		okToExit = ETrue;
    		break;
        case EAmsSoftkeyStop:
        	iRecorder->StopL();
        	okToExit = ETrue;
        	break;
      	case EAmsSoftkeyPause:
 			PauseL(); 
      		break;
		case EAmsSoftkeyContinue:
	        ContinueL();
			break;
		case EAmsSoftkeyToLoudspeaker:
			ToLoudspeaker();
			break;
		case EAmsSoftkeyToEarpiece:
			ToEarpiece();
			break;
    	case EEikBidCancel:
    		iRecorder->StopL();
    		okToExit = ETrue;
    		break;
    	case EKeyEnd:
    		iRecorder->StopL();
    		okToExit = ETrue;
			break;
       	default:
      		break;
      	}
      	
   CleanupStack::PopAndDestroy( commandAbs ); 
   return okToExit;
   }

// ---------------------------------------------------------
// CAudioMessageProgressDialog::DoTickL
// Periodical timer tick
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::DoTickL( TTimeIntervalMicroSeconds aCurrValue )
    {
    //We must prevent screen saver.
    User::ResetInactivityTime();
    //if ( ( aCurrValue.Int64() ) <= ( iFinalValue *  KSecondAsMicroSeconds ) )
    if ( ( aCurrValue.Int64() ) <= iMaxDurationMicroSec.Int64() )
        {
       	if ( !( iFlags & EAmsWorkingModeRecorder) 
       	   || ( ( iFlags & EAmsWorkingModeRecorder)  && ( iTickCount != 0 ) ) ) //rec
       		{
  			if ( !IsVisible() )
        		{
   				MakeVisible( ETrue );
    			}
			//Let's do some corrective actions if the tick count jumps
			//It may occur if there is frequent UI action.
			//e.g. extremely continuos volume adjusting during play
			//The tick occurs at every 200000 microseconds	
			//You could also use on SetAndDraw and not IncrementAndDraw but then
			//the progress bar does not update smoothly
			TInt temp ( aCurrValue.Int64()/KAmsDisplayUpdateDelay );
			if ( temp > iIncrement+2 )
			    {
			    iProgressInfo->SetAndDraw( temp );
			    }
			else
			    {
			    iProgressInfo->IncrementAndDraw( 1 );
			    }
			iIncrement=temp;
    		}
   		if ( ( !( iFlags & EAmsWorkingModeRecorder) ) && ( iTickCount == 0 ))
        	{
        	SetNaviVolL();
        	}
        iTickCount++;
        }
    else
        {
        if ( !( iFlags & EAmsWorkingModeRecorder) )
            {
            //completely fill the progress bar before exiting after play.
            while( iTickCount <= ( iFinalValue * KFive ) )
                {
                iProgressInfo->IncrementAndDraw( 1 );
                iTickCount++;
                }            
            }
       	iFlags |= EAmsSetExit;
        }
        
    if ( iFlags & EAmsSetExit )
        {
    	iRecorder->StopL();
      	if ( iFlags & EAmsWorkingModeRecorder)
      		{
      		//ESCN-7MP97X: Application crashed when audio message was recorded upto max length
      		// and user pressed some key on toolbar during screen transition.
            CEikButtonGroupContainer* cba;
            cba = CEikButtonGroupContainer::Current();
            cba->MakeVisible(EFalse);
            this->MakeVisible(EFalse);      		
      	    ShowInformationNoteL( R_AUDIOMESSAGE_INFO_MAX_REC_TIME, ETrue );
      		}
        // Try to exit dialog cleanly. If that doesn't work,
    	// then terminate anyway.
        TRAPD( err, TryExitL( EAmsFileEnd/*EAmsSoftkeyStop*/ ) );
    	if ( err != KErrNone )
    		{
        	delete this;
        	}

        }
   	else if ( iFlags & EAmsWsEventStop )
           {
    	iRecorder->StopL();
    	TRAPD( err, TryExitL( EAmsSoftkeyStop ) );
    	if ( err != KErrNone )
    		{
        	delete this;
        	}
 		iFlags &= ~EAmsWsEventStop;
        }
    }
    
// ---------------------------------------------------------
// CAudioMessageProgressDialog::SetNaviVolL
// Updates Navipane's volume state via AudioMessageAppUi
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::SetNaviVolL()
  	{
   	iCurrentVolume = iRecorder->GetCurrentVolume();
	iNaviVolObserver->UpdateNaviVol( iCurrentVolume );
  	}
    
// ---------------------------------------------------------
// CAudioMessageProgressDialog::ShowInformationNoteL
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::ShowInformationNoteL( TInt aResourceID, TBool aWaiting )
    {
    HBufC* prompt = NULL;
    prompt = StringLoader::LoadLC( aResourceID, iCoeEnv );
    CAknInformationNote* note = new ( ELeave )
    CAknInformationNote( aWaiting );
    note->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    } 
       
// ---------------------------------------------------------
// CAudioMessageProgressDialog::UpdateL
// Updates Progress texts ("current sec / maximum sec" rec/play)
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::UpdateL( TTimeIntervalMicroSeconds aCurrentDuration, TInt aErrorCode )
	{
//The following  check is preventing a case where aCurrentDuration suddenly
//drops to zero. There must be error in lower levels of software. There is nothing to do but
//quit. It occurs e.g. in low memory situation.
//If this check is not done the recording may continue undefined amount of time	and the recording time
//in the progress dialog stays zero.
	
	if ( aErrorCode != KErrNone
	 || ( iTickCount > 1 && aCurrentDuration == 0  ) )
	    {
AMSLOGGER_WRITE( "CAudioMessageProgressDialog::UpdateL() ERROR" );		
	    
	    TRAPD( err, TryExitL( EAmsFileEnd ) );
        if ( err != KErrNone )
    	    {
AMSLOGGER_WRITE( "TryExitL FAILS" );    	    
            delete this;
            }
	    }
	else
	    {
	    if ( iFlags & EAmsWorkingModeRecorder ) //rec
		    {
	        iDocument.SetDuration( aCurrentDuration );
		    }
	    }    
	
	
	if ( !(iFlags & EAmsSetExit) )
		{
		if ( iFlags & EAmsWorkingModeRecorder )//recording in progress
			{
			UpdateProgressTextL( R_AUDIOMESSAGE_PROGRESS_RECORDING, aCurrentDuration );
	 		}
		else//playing
			{
			UpdateProgressTextL( R_AUDIOMESSAGE_PROGRESS_PLAYING, aCurrentDuration );
			}	
		DoTickL( aCurrentDuration);
		}	
	}

// ---------------------------------------------------------
// CAudioMessageProgressDialog::UpdateProgressTextL
// Parses text strings to progress dialog
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::UpdateProgressTextL( TInt aText, 
	TTimeIntervalMicroSeconds aMicroSec )
	{
	TInt strLength = 0;
  	TTime currentLengthAsTime( aMicroSec.Int64() );
  	TTime maxLengthAsTime( iMaxDurationMicroSec.Int64() );
	strLength = iTimeDuratBase->Length() ;

	if ( !iLengthNow  )
		{
		iLengthNow = HBufC::NewL( strLength );
		}
 
 	TPtr lnow(iLengthNow->Des());
	currentLengthAsTime.FormatL( lnow, *iTimeDuratBase);

	if ( !iLengthMax)
		{
	    iLengthMax = HBufC::NewL( strLength );		
		}
	
 	TPtr lmax( iLengthMax->Des() );
	maxLengthAsTime.FormatL( lmax, *iTimeDuratBase);

	HBufC* tempBuf = NULL;	
	switch ( aText )
		{
		case R_AUDIOMESSAGE_PROGRESS_PLAYING:
		tempBuf = iTextPlaying;
		break;
		case R_AUDIOMESSAGE_PROGRESS_RECORDING:
		tempBuf = iTextRecording;
		break;
		case R_AUDIOMESSAGE_PROGRESS_PAUSED:
		tempBuf = iTextPaused;
		break;
		// Coverty fix, Forward NULL; http://ousrv057/cov.cgi?cid=35851
		default:
		tempBuf = iTextPlaying;
		break;		
		}
	
 	strLength += tempBuf->Length();
	TPtr tempBuff( tempBuf->Des() );
	HBufC* baseText = HBufC::NewLC( strLength );
	TPtr baset( baseText->Des() );
	HBufC* texts = HBufC::NewLC( strLength );
	TPtr pText( texts->Des() );

	StringLoader::Format( baset,tempBuff,  1, lmax );
	StringLoader::Format( pText,baset,  0, lnow );
  
  	AknTextUtils::LanguageSpecificNumberConversion( pText );

   	this->SetTextL( pText ); 
   	
    CleanupStack::PopAndDestroy( texts );
    CleanupStack::PopAndDestroy( baseText );
	}

// ---------------------------------------------------------
// CAudioMessageProgressDialog::PauseL
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::PauseL() 
	{
	iFlags |= EAmsPause;
	iPausePosMicroSec = iRecorder->GetPosition().Int64();
	iRecorder->Pause();
	UpdateProgressTextL( R_AUDIOMESSAGE_PROGRESS_PAUSED, iPausePosMicroSec );
	SetProgressIconL( EMbmAudiomessageQgn_indi_mms_audio_pause, 
		EMbmAudiomessageQgn_indi_mms_audio_pause_mask );
	SetSoftkeys( R_SOFTKEYS_CONTINUE_STOP );
	}

// ---------------------------------------------------------
// CAudioMessageProgressDialog::ContinueL
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::ContinueL() 
	{
	iFlags &= ~EAmsPause;
	iRecorder->ContinueL();
	UpdateProgressTextL( R_AUDIOMESSAGE_PROGRESS_PAUSED, iPausePosMicroSec );
	SetProgressIconL( EMbmAudiomessageQgn_indi_mms_audio_record, 
		EMbmAudiomessageQgn_indi_mms_audio_record_mask );
	SetSoftkeys( R_SOFTKEYS_PAUSE_STOP );
	}
	
// ---------------------------------------------------------
// CAudioMessageProgressDialog::SetSoftkeys
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::SetSoftkeys( TInt aSk )
	{
   	CEikButtonGroupContainer* cba;
    cba = CEikButtonGroupContainer::Current();
    TRAP_IGNORE( cba->SetCommandSetL( aSk ) );
    cba->DrawNow();
	}

// ---------------------------------------------------------
// CAudioMessageProgressDialog::SetProgressIconL
// Sets play, rec and pause icon
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::SetProgressIconL( TInt aIconId, TInt aIconMask )
	{
   	CEikImage* icon = NULL;      
   	icon = new ( ELeave ) CEikImage;
   	CleanupStack::PushL( icon );
    TParse fileParse;
    fileParse.Set( KAmsBitmapFileName, &KDC_APP_BITMAP_DIR, NULL );
   	icon->CreatePictureFromFileL( fileParse.FullName(), aIconId, aIconMask );
   	this->SetIconL( icon );
   	CleanupStack::Pop( icon );
	}

// ---------------------------------------------------------
// CAudioMessageProgressDialog::ToLoudspeaker
// Sets audio route loudspeaker
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::ToLoudspeaker()
	{
	iFlags &= ~EAmsAudioOutputEarpiece;
    if ( iRecorder->SetAudioOutput( ETrue ) == KErrNone )
    	{
	 	iNaviVolObserver->SetSpeakerOutput( iFlags & EAmsAudioOutputEarpiece );
		SetSoftkeys( R_SOFTKEYS_TO_EARPIECE_STOP );
    	}
    else
    	{
		iFlags |= EAmsAudioOutputEarpiece;
    	}
    	
	}

// ---------------------------------------------------------
// CAudioMessageProgressDialog::ToEarpiece
// Sets audio route earpiece
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::ToEarpiece()
	{
	iFlags |= EAmsAudioOutputEarpiece;

	if ( iRecorder->SetAudioOutput( EFalse ) == KErrNone)
		{
		iNaviVolObserver->SetSpeakerOutput( iFlags & EAmsAudioOutputEarpiece  );
		SetSoftkeys( R_SOFTKEYS_TO_LOUDSPEAKER_STOP );
		}
    else
    	{
		iFlags &= ~EAmsAudioOutputEarpiece;

    	}
	}

// ---------------------------------------------------------
// CAudioMessageProgressDialog::PlayingStops
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::PlayingStops()
	{
  	iFlags |= EAmsSetExit;
    TRAP_IGNORE( iRecorder->StopL());
    // Try to exit dialog cleanly. If that doesn't work,
    // then terminate anyway.
    TRAPD( err, TryExitL( EAmsFileEnd ) );
    if ( err != KErrNone )
    	{
        delete this;
        }
	}

// ---------------------------------------------------------
// CAudioMessageProgressDialog::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CAudioMessageProgressDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
	TEventCode aType )
	{
	if ( !(iFlags & EAmsWorkingModeRecorder ))//playing in progress
		{
		if ( ( aKeyEvent.iCode == EKeyLeftArrow ) )
		  	{
            iChange = KVolumeChangeDown;
			DoChangeVolumeL();
		    return EKeyWasConsumed;
		  	}
		if ( ( aKeyEvent.iCode  == EKeyRightArrow))
		  	{
            iChange = KVolumeChangeUp;
			DoChangeVolumeL();
		    return EKeyWasConsumed;
		  	}
		if (  aKeyEvent.iCode == EKeyUpArrow )
			{
		    return EKeyWasConsumed;
			}
		if (  aKeyEvent.iCode  == EKeyDownArrow)
			{
		    return EKeyWasConsumed;
			}
		if (  aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode  == EKeyOK )
			{
			if ( !(iFlags & EAmsSupportAudioOutput) )
				{
				iFlags |= EAmsWsEventStop;
				}			
			else
				{
				if ( iFlags & EAmsAudioOutputEarpiece )
					{
					ToLoudspeaker();
					}
				else
					{
					ToEarpiece();
					}
				}
			return EKeyWasConsumed;
			}
  
   		}
   	else if (  aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode  == EKeyOK )
		{
		if ( iFlags & EAmsPause )
			{
			ContinueL();
			}
		else
			{
			PauseL();
			}
		return EKeyWasConsumed;
		}
  
	if (  aKeyEvent.iCode == EKeyUpArrow )
		{
	    return EKeyWasConsumed;
		}
	if (  aKeyEvent.iCode  == EKeyDownArrow )
		{
	    return EKeyWasConsumed;
		}
		
  	return CAknProgressDialog::OfferKeyEventL( aKeyEvent,aType );
	}

// ---------------------------------------------------------
// CAudioMessageProgressDialog::SetNaviObserver
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::SetNaviObserver( MAmsVolumeObserver* aObserver )
	{
	iNaviVolObserver = aObserver;
	}

// ---------------------------------------------------------
// CAudioMessageProgressDialog::SetMaxMessageSize
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::SetMaxMessageSize( TUint aSize )
	{
	iMaxMessageSize = aSize;
	}

// ---------------------------------------------------------
// CAudioMessageProgressDialog::MrccatoCommand
// Handles side volume key events.
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::MrccatoCommand( TRemConCoreApiOperationId   aOperationId, 
                                    TRemConCoreApiButtonAction  aButtonAct )
    {
    if ( iFlags & EAmsWorkingModeRecorder )
        {
        return;
        }
	
    TRequestStatus stat;
 
    switch ( aOperationId )
        {
        case ERemConCoreApiVolumeDown: //  Volume Down
            switch ( aButtonAct )
                {
                case ERemConCoreApiButtonPress:
                    // Volume up hold down for 0,6 seconds
                    iChange = KVolumeChangeDown;
                    iVolumeTimer->Cancel();
                    iVolumeTimer->Start(    KVolumeFirstChangePeriod,
                                            KVolumeChangePeriod, 
                                            TCallBack( ChangeVolume, this ) );
                    break;
                case ERemConCoreApiButtonRelease:
                    iVolumeTimer->Cancel();
                    break;
                case ERemConCoreApiButtonClick:
                    iChange = KVolumeChangeDown;
					TRAP_IGNORE( DoChangeVolumeL() );

                    break;
                default:
                    // Never hits this
                    break;
                }
           	iCoreTarget->VolumeDownResponse(stat, KErrNone);
            break;
        case ERemConCoreApiVolumeUp: //  Volume Up
            switch ( aButtonAct )
                {
                case ERemConCoreApiButtonPress:
                    // Volume down hold for 0,6 seconds
                    iChange = KVolumeChangeUp;
                    iVolumeTimer->Cancel();
                    iVolumeTimer->Start(    KVolumeFirstChangePeriod,
                                            KVolumeChangePeriod, 
                                            TCallBack( ChangeVolume, this ) );

                    break;
                case ERemConCoreApiButtonRelease:
                   	iVolumeTimer->Cancel();
                    break;
                case ERemConCoreApiButtonClick:
                    iChange = KVolumeChangeUp;
					TRAP_IGNORE( DoChangeVolumeL() );
                    break;
                default:
                    // Never hits this
                    break;
                }
            iCoreTarget->VolumeUpResponse(stat, KErrNone);
            break;
        default :
           	iCoreTarget->SendResponse( stat, aOperationId, KErrNone );
            break;
        }
        User::WaitForRequest( stat );

    }
    
// ---------------------------------------------------------
// CAudioMessageProgressDialog::ChangeVolume
// ---------------------------------------------------------
//
TInt CAudioMessageProgressDialog::ChangeVolume( TAny* aObject )
    {
    // cast, and call non-static function
    TRAP_IGNORE( static_cast<CAudioMessageProgressDialog*>( aObject )->DoChangeVolumeL( ));
    return KErrNone;
    }

// ---------------------------------------------------------
// CAudioMessageProgressDialog::DoChangeVolumeL
// ---------------------------------------------------------
//
void CAudioMessageProgressDialog::DoChangeVolumeL( )
    {
    if ( iChange == KVolumeChangeUp )
	    {
        if ( iCurrentVolume + KSeed > KMaxVol )
        	{
			iAvkonAppUi->KeySounds()->PlaySound( EAvkonSIDVolumeMaxTone );
        	}
		else
			{
			iCurrentVolume++;
			}
		iNaviVolObserver->UpdateNaviVol( iCurrentVolume );
		iRecorder->IncrementVol();
	    }
    else
	    {
       	if ( iCurrentVolume - KSeed < KMinVol )
       		{
			iAvkonAppUi->KeySounds()->PlaySound( EAvkonSIDVolumeMinTone );
       		}
		else
			{
			iCurrentVolume--;
			}
		iNaviVolObserver->UpdateNaviVol( iCurrentVolume );
		iRecorder->DecrementVol();
	    }
    iVolumePopup->SetValue( iCurrentVolume );
    iVolumePopup->ShowVolumePopupL();
    }


#ifdef RD_SCALABLE_UI_V2
// -----------------------------------------------------------------------------
// handles the touch-ui related control events for the volume arrows
// -----------------------------------------------------------------------------
//
void CAudioMessageProgressDialog::HandleNaviDecoratorEventL( TInt aEventID )     
    {
    if( aEventID == EAknNaviDecoratorEventRightTabArrow )
        {
        iChange = KVolumeChangeUp;
		DoChangeVolumeL();
		}
    else // LeftTabArrow
        {
        iChange = KVolumeChangeDown;
		DoChangeVolumeL();
        }
    }
     
// -----------------------------------------------------------------------------
// handles the touch-ui related control events coming from the volume bar
// -----------------------------------------------------------------------------
//
void CAudioMessageProgressDialog::HandleControlEventL( 
    CCoeControl* aControl,
    TCoeEvent aEventType )
    {
    if((aControl == iVolumePopup) 
        && ( aEventType == MCoeControlObserver::EEventStateChanged)
         )
        {
        iCurrentVolume = iVolumePopup->Value();          
        iRecorder->SetVol( iCurrentVolume );
        iNaviVolObserver->UpdateNaviVol( iCurrentVolume );            		    
        }
    }

void CAudioMessageProgressDialog::SetVolCtrlObserver( MCoeControlObserver& aObserver )    
    {
    iVolCtrlObserver = &aObserver;
    }

#else
void CAudioMessageProgressDialog::HandleNaviDecoratorEventL( TInt /* aEventID */ )     
    {
    }
    
void CAudioMessageProgressDialog::HandleControlEventL( CCoeControl* /*aControl*/, 
                                                        TCoeEvent /* aEventType */ )
    {
    }
void CAudioMessageProgressDialog::SetVolCtrlObserver( MCoeControlObserver& 
                                                            /*aObserver*/ )    
    {
    }

#endif    
