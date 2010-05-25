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
* Description:   This class acts as a high level wrapper around the 
*                CMdaAudioRecorderUtility playback and recording 
*                functionalities. Functons for output route handling.
*
*/



#include <AudioPreference.h>
#include <AudioOutput.h>
#include <AudioInput.h>
#include <avkon.hrh>
#include "audiomessagebeep.h"
#include "audiomessagerecorder.h"
#include "amsvoiceobserver.h"
#include "AudioMessageLogging.h"

const TInt KAmsVolumeSteps = 10;

const TUint KAmsDefaultAmrBitrate( 12200 );  //Voice Recorder uses this value for MMS
                                             //optimized voice

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
// CAudioMessageRecorder::NewL
// ---------------------------------------------------------
//
CAudioMessageRecorder* CAudioMessageRecorder::NewL( const RFile& aFile,  const TBool aAudioOutput )
	{
	CAudioMessageRecorder * recorder = new( ELeave ) CAudioMessageRecorder( );
	CleanupStack::PushL( recorder );
	recorder->ConstructL( aFile, aAudioOutput );
	CleanupStack::Pop( recorder );
	return recorder;	
	}

// ---------------------------------------------------------
// CAudioMessageRecorder::ConstructL
// ---------------------------------------------------------
//
void CAudioMessageRecorder::ConstructL( const RFile& aFile, const TBool aAudioOutput )
	{
AMSLOGGER_WRITE( "CAudioMessageRecorder::ConstructL" );	
    iRecUtility = CMdaAudioRecorderUtility::NewL(
       	*this,
        NULL,
        KAudioPriorityRecording,
        TMdaPriorityPreference( KAudioPrefRealOneLocalPlayback ) );

	iRecUtility->OpenFileL( aFile );
	iBeepPlayer = CAudioMessageBeep::NewL();
	iBeepPlayer->PrepareToneL( EAvkonSIDVoiceRecordingStartTone );
	iPause = EFalse;
	iSupportAudioOutput = aAudioOutput;
	iTimer = CPeriodic::NewL( EPriorityNormal + 1 ); 
	
   	if ( !iActiveWait.IsStarted() )
		{
		iActiveWait.Start();
		}
AMSLOGGER_WRITE( "CAudioMessageRecorder::ConstructL DONE!" );		
	}


// ---------------------------------------------------------
// CAudioMessageRecorder::MoscoStateChangeEvent
// ---------------------------------------------------------
void CAudioMessageRecorder::MoscoStateChangeEvent( CBase* /*aObject*/, TInt aPreviousState, 
	TInt aCurrentState, TInt  aErrorCode)
	{
	AMSLOGGER_WRITEF( _L( "CAudioMessageRecorder: MoscoStateChangeEvent() prev: %d, current: %d, error: %d" ), aPreviousState,
			aCurrentState, aErrorCode );

			
	if ( iActiveWait.IsStarted() )
		{
		iActiveWait.AsyncStop();
		}

    iErrorCode = aErrorCode;
    
    //Let's not go here in case there has been an error in opening the file.
    //That's why the error code has been checked here
    if( aCurrentState == CMdaAudioClipUtility::EOpen 
         && aPreviousState == CMdaAudioClipUtility::ENotReady 
         && !iAudioInput
         && iErrorCode == KErrNone )
        {
        TRAPD(error, iAudioInput = CAudioInput::NewL( *iRecUtility ); 
            ResetAudioInputL();
            iRecUtility->SetDestinationBitRateL( KAmsDefaultAmrBitrate ));
        
        //KErrNotSupported is allowed because it works OK even if CAudioOutput
        //is not supported
        if( error != KErrNone && error != KErrNotSupported )
            {
            iErrorCode = error;
            }
        
        }
    else if ( aCurrentState == CMdaAudioClipUtility::EOpen && aPreviousState ==
    	CMdaAudioClipUtility::EPlaying )
   		{ 
   		iObserver->PlayingStops();   
 		}   
    else if ( aCurrentState == CMdaAudioClipUtility::EOpen && aPreviousState == 
    	CMdaAudioClipUtility::ERecording )
   		{ 
   		if ( !iPause )
	   		{
	   		iObserver->PlayingStops();   
	   		}
   		}
 	 else if ( aCurrentState == CMdaAudioClipUtility::ERecording && 
 	    aPreviousState == CMdaAudioClipUtility::EOpen )
 	    {
 	    iBeepPlayer->PrepareTone( EAvkonSIDVoiceRecordingStopTone );    
 	    }	  
	}

// ---------------------------------------------------------
// CAudioMessageRecorder::SetVol
// ---------------------------------------------------------
//
void CAudioMessageRecorder::SetVol( TInt aVal )
	{
	TInt max = iRecUtility->MaxVolume();
	TInt step = max / KAmsVolumeSteps;
	iCurrentVolume = aVal * step;
	
    if ( iCurrentVolume >= max )
        iRecUtility->SetVolume( max );
    else if ( iCurrentVolume <= 0)
        iRecUtility->SetVolume( 0 );
    else
        iRecUtility->SetVolume( iCurrentVolume );

	}

// ---------------------------------------------------------
// CAudioMessageRecorder::StartPlayingL
// ---------------------------------------------------------
//
void CAudioMessageRecorder::StartPlayingL()
	{  
	if( iErrorCode != KErrNone )
	    {
AMSLOGGER_WRITEF( _L( "CAudioMessageRecorder: StartPlayingL(): Leave ( %d ) occured." ), iErrorCode );
	    User::Leave( iErrorCode);
	    return;
	    }	 
	iRecUtility->SetPlaybackBalance( KMMFBalanceCenter );
	iRecUtility->SetVolume( iCurrentVolume ) ;
    iRecUtility->PlayL();
 	if ( !iActiveWait.IsStarted() )
		{
		iActiveWait.Start();
		}
	StartTimer();
	}

// ---------------------------------------------------------
// CAudioMessageRecorder::StartRecordingL
// ---------------------------------------------------------
//
void CAudioMessageRecorder::StartRecordingL()
	{ 
	if( iErrorCode != KErrNone )
	    {
AMSLOGGER_WRITEF( _L( "CAudioMessageRecorder: StartRecordingL(): Leave ( %d ) occured." ), iErrorCode );
	    User::Leave( iErrorCode);
	    return;
	    }
	
    iRecUtility->SetAudioDeviceMode( CMdaAudioRecorderUtility::ELocal );
    iRecUtility->SetPriority( KAudioPriorityRecording, 
        TMdaPriorityPreference( KAudioPrefVoiceRec ) );	
    iRecUtility->SetGain( iRecUtility->MaxGain() );
    iRecUtility->SetPosition( TTimeIntervalMicroSeconds( 0 ) );

	iBeepPlayer->PlayTone( EAvkonSIDVoiceRecordingStartTone );
	//iBeepPlayer->PrepareToneL( EAvkonSIDVoiceRecordingStopTone );    

    iRecUtility->RecordL();
 	if ( !iActiveWait.IsStarted() )
		{
		iActiveWait.Start();
		}
	StartTimer();
	}

// ---------------------------------------------------------
// CAudioMessageRecorder::Pause
// ---------------------------------------------------------
//
void CAudioMessageRecorder::Pause()
	{
	iPause = ETrue;
    iRecUtility->Stop();
    StopTimer();
	iRecUtility->SetPosition( TTimeIntervalMicroSeconds( 0 ) );
    iRecUtility->SetPriority( KAudioPriorityRecording, 
    	TMdaPriorityPreference( KAudioPrefRealOneLocalPlayback ) );	
	}

// ---------------------------------------------------------
// CAudioMessageRecorder::Stop
// ---------------------------------------------------------
//
void CAudioMessageRecorder::StopL()
	{
	iPause = EFalse;
    iRecUtility->Stop();
    StopTimer();
	iRecUtility->SetPosition( TTimeIntervalMicroSeconds( 0 ) );
    iRecUtility->SetPriority( KAudioPriorityRecording, 
    	TMdaPriorityPreference( KAudioPrefRealOneLocalPlayback) );	
	iBeepPlayer->PlayTone( EAvkonSIDVoiceRecordingStopTone );
	//iBeepPlayer->PrepareToneL( EAvkonSIDVoiceRecordingStartTone );		
	}

// ---------------------------------------------------------
// CAudioMessageRecorder::ContinueL
// ---------------------------------------------------------
//
void CAudioMessageRecorder::ContinueL()
	{
	if( iErrorCode != KErrNone )
	    {
AMSLOGGER_WRITEF( _L( "CAudioMessageRecorder: ContinueL(): Leave ( %d ) occured." ), iErrorCode );
	    User::Leave( iErrorCode);
	    return;
	    }
	
	iPause = EFalse;
	iRecUtility->SetPriority( KAudioPriorityRecording, TMdaPriorityPreference( KAudioPrefVoiceRec ) );
        iRecUtility->RecordL(); 
 	if ( !iActiveWait.IsStarted() )
		{
		iActiveWait.Start();
		}
	StartTimer();
	}

// ---------------------------------------------------------
// CAudioMessageRecorder::Constructor
// ---------------------------------------------------------
//
CAudioMessageRecorder::CAudioMessageRecorder() 
	{
  	}

// ---------------------------------------------------------
// CAudioMessageRecorder::Destructor
// ---------------------------------------------------------
//
CAudioMessageRecorder::~CAudioMessageRecorder()
	{ 
	delete iAudioOutput;
  	delete iAudioInput;
    delete iRecUtility;
	delete iTimer;
   	delete iBeepPlayer;
	}

// ---------------------------------------------------------
// CAudioMessageRecorder::SetObserver
// ---------------------------------------------------------
//
void CAudioMessageRecorder::SetObserver(MAmsVoiceObserver* aObserver )
	{
	iObserver = aObserver;
	}
	
// ---------------------------------------------------------
// CAudioMessageRecorder::StartTimer
// ---------------------------------------------------------
//
void CAudioMessageRecorder::StartTimer()
	{
	StopTimer();	// stop if already running
	TCallBack cb( TimerCallBack, this );
	iTimer->Start( 0, KAmsDisplayUpdateDelay, cb );
	}


// ---------------------------------------------------------
// CAudioMessageRecorder::StopTimer
// ---------------------------------------------------------
//
void CAudioMessageRecorder::StopTimer()
	{
	iTimer->Cancel();
	}

// ---------------------------------------------------------
// CAudioMessageRecorder::TimerCallBack
// ---------------------------------------------------------
//
TInt CAudioMessageRecorder::TimerCallBack( TAny* aRecorder )
	{
	CAudioMessageRecorder* recorder = reinterpret_cast< CAudioMessageRecorder* >( aRecorder );
	recorder->HandleTick();
	return ETrue;
	}

// ---------------------------------------------------------
// CAudioMessageRecorder::HandleTick
// ---------------------------------------------------------
//
void CAudioMessageRecorder::HandleTick()
	{	
	TRAPD( error, iObserver->UpdateL( iRecUtility->Position(), iErrorCode ) );
	if ( error != KErrNone )
	    {
	    iErrorCode = error;
	    }
	}

// ---------------------------------------------------------
// CAudioMessageRecorder::GetPosition
// ---------------------------------------------------------
//
TTimeIntervalMicroSeconds CAudioMessageRecorder::GetPosition()
	{
	return iRecUtility->Position();
	}

// ---------------------------------------------------------
// CAudioMessageRecorder::GetDuration
// ---------------------------------------------------------
//
TTimeIntervalMicroSeconds CAudioMessageRecorder::GetDuration()
	{
	return iRecUtility->Duration();
	}
	
// ---------------------------------------------------------
// CAudioMessageRecorder::SetAudioOutput
// ---------------------------------------------------------
//
TInt CAudioMessageRecorder::SetAudioOutput( TBool aOutputPublic )
	{ 
	TInt err ( KErrNone );
	CAudioOutput::TAudioOutputPreference outputPckg;

	if ( iAudioOutput )
		{
		if ( aOutputPublic )
			{
			outputPckg = CAudioOutput::EPublic;
			}
		else
			{
			outputPckg = CAudioOutput::EPrivate;
			}
		TRAP( err, iAudioOutput->SetAudioOutputL( outputPckg ) );
		}
	return err;
	}
	
// ---------------------------------------------------------
// CAudioMessageRecorder::ResetAudioInputL
// ---------------------------------------------------------
//
void CAudioMessageRecorder::ResetAudioInputL(  )
	{ 
    if( iAudioInput )
        {
AMSLOGGER_WRITE( "CAudioMessageRecorder::ResetAudioInput, iAudioInput EXISTS" );        
        RArray<CAudioInput::TAudioInputPreference> inputArray(4);
        CleanupClosePushL( inputArray );
        inputArray.Append( CAudioInput::EDefaultMic );
        iAudioInput->SetAudioInputL( inputArray.Array() );   
        CleanupStack::PopAndDestroy( &inputArray );
        }
#ifdef _DEBUG	
    else
        {
AMSLOGGER_WRITE( "CAudioMessageRecorder::ResetAudioInput, iAudioInput == NULL!" );
        }
#endif
	}

// ---------------------------------------------------------
// CAudioMessageRecorder::GetCurrentVolume
// ---------------------------------------------------------
//
TInt CAudioMessageRecorder::GetCurrentVolume()
	{
	TInt current;
	iRecUtility->GetVolume( current );
	TInt stepsize = iRecUtility->MaxVolume() / KAmsVolumeSteps;
	return ( current / stepsize );	
	}

// ---------------------------------------------------------
// CAudioMessageRecorder::GetBitRateL
// ---------------------------------------------------------
//
TUint CAudioMessageRecorder::GetBitRateL()
	{
	if( iErrorCode != KErrNone )
	    {
AMSLOGGER_WRITEF( _L( "CAudioMessageRecorder::GetBitRateL(): Leave ( %d ) occured." ), iErrorCode );
	    User::Leave( iErrorCode);
	    return 0;
	    }	
	return iRecUtility->DestinationBitRateL();
	}

// ---------------------------------------------------------
// CAudioMessageRecorder::IncrementVol
// ---------------------------------------------------------
//
void CAudioMessageRecorder::IncrementVol()
    {
    TInt maxVolume ( iRecUtility->MaxVolume() );
    TInt currentVolume ( 0 );
    iRecUtility->GetVolume( currentVolume );
    TInt volumeValue = currentVolume + (maxVolume /KAmsVolumeSteps);
    
    if ( volumeValue >= maxVolume )
        iRecUtility->SetVolume( maxVolume );
    else
        iRecUtility->SetVolume( volumeValue );
    
    }

// ---------------------------------------------------------
// CAudioMessageRecorder::DecrementVol
// ---------------------------------------------------------
//
void CAudioMessageRecorder::DecrementVol()
    {
    TInt maxVolume ( iRecUtility->MaxVolume() );
    TInt currentVolume ( 0 );
    iRecUtility->GetVolume( currentVolume );
    TInt volumeValue = currentVolume - (maxVolume/KAmsVolumeSteps);
    
    if ( volumeValue <= 0 )
        iRecUtility->SetVolume( 0 );
    else
        iRecUtility->SetVolume( volumeValue );
    }
    
// ---------------------------------------------------------
// CAudioMessageRecorder::InitializeAudioRoutingL
// ---------------------------------------------------------
//
void CAudioMessageRecorder::InitializeAudioRoutingL()
	{
	if ( iSupportAudioOutput )
		{
		if ( !iAudioOutput )
			{
		    TRAPD( err, iAudioOutput = CAudioOutput::NewL( *iRecUtility, EFalse ) );
			}
		}
	}

