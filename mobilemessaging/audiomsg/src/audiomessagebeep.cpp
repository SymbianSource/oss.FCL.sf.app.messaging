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
* Description:   Class that handles playback of tones in AudioMessage applicationn
*
*/



#include <avkon.hrh>
#include <aknappui.h>
#include <AudioPreference.h>
#include <aknsoundsystem.h>
#include <aknSoundinfo.h>

#include "audiomessagebeep.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
// CAudioMessageBeep::Constructor
// ---------------------------------------------------------
//
CAudioMessageBeep::CAudioMessageBeep()
	: iState( EVRToneIdle )
    {
    }


// ---------------------------------------------------------
// CAudioMessageBeep::ConstructL
// ---------------------------------------------------------
//
void CAudioMessageBeep::ConstructL()
    {
    iPlayer = CMdaAudioToneUtility::NewL( *this );
    
    iSoundSystem = ( static_cast< CAknAppUi* >(
						CEikonEnv::Static()->EikAppUi())->KeySounds() );
    }


// ---------------------------------------------------------
// CAudioMessageBeep::NewL
// ---------------------------------------------------------
//
CAudioMessageBeep* CAudioMessageBeep::NewL()
    {
    CAudioMessageBeep* self = new ( ELeave ) CAudioMessageBeep;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------
// CAudioMessageBeep::Destructor
// ---------------------------------------------------------
//
CAudioMessageBeep::~CAudioMessageBeep()
    {
    if ( iPlayer )
        {
        switch ( iPlayer->State() )
            {
            case EMdaAudioToneUtilityNotReady:
                {
                iPlayer->CancelPrepare();
                break;
                }
            case EMdaAudioToneUtilityPlaying:
                {
                iPlayer->CancelPlay();
                break;
                }           
            }
        }
    delete iPlayer;
    }


// ---------------------------------------------------------
// CAudioMessageBeep::PrepareToneL
// Starts preparing the specified tone, preparing completes
// with call-back to MatoPrepareComplete
// ---------------------------------------------------------
//
void CAudioMessageBeep::PrepareToneL( TInt aToneId )
    {
	if ( iState != EVRToneIdle )
		{
		iQueuedTone = aToneId;
		return;
		}

	// Retrieve the tone
	CAknSoundInfo* info = CAknSoundInfo::NewL();
	CleanupStack::PushL( info );
	User::LeaveIfError( iSoundSystem->RequestSoundInfoL(
					                  aToneId, *info ) );
    
    iPreparedTone = aToneId;
    iState = EVRTonePreparing;
    
    iPlayer->PrepareToPlayDesSequence( *info->iSequence );
    CleanupStack::PopAndDestroy( info );
    }

// ---------------------------------------------------------
// CAudioMessageBeep::PrepareTone
// Non-leaving version of PrepareToneL
// ---------------------------------------------------------
//
void CAudioMessageBeep::PrepareTone( TInt aToneId )
    {
    TRAP_IGNORE( PrepareToneL( aToneId ) );
    }

// ---------------------------------------------------------
// CAudioMessageBeep::PlayTone
// Starts playback of specified tone. Completes with call-back to
// MatoPlayComplete
// ---------------------------------------------------------
//
void CAudioMessageBeep::PlayTone( TInt aToneId )
    {
    if ( iState != EVRToneIdle || iPreparedTone != aToneId || 
        iPlayer->State() == EMdaAudioToneUtilityPlaying )
    	{
    	// Preparing an unprepared tone synchronously is not supported,
    	// tone playback is skipped
    	return;
    	}
    
    // Preferences need to be set according to tone
    switch ( aToneId )
    	{
    	case EAvkonSIDVoiceRecordingStartTone:
    		{
    		iPlayer->SetPriority( KAudioPriorityRecording,
    			TMdaPriorityPreference( KAudioPrefVoiceRecStart ) );
			break;	    		
    		}    
    	case EAvkonSIDVoiceRecordingStopTone:
    		{
    		iPlayer->SetPriority( KAudioPriorityRecording,
    			TMdaPriorityPreference( KAudioPrefVoiceRecStop ) );    		
			break;    	    		    			
    		}
    	default:
    		{
    		break;
    		}
		}
 
    iState = EVRTonePlaying;
    
    iPlayer->Play();
    }

// ---------------------------------------------------------
// CAudioMessageBeep::MatoPrepareComplete
// ---------------------------------------------------------
//
void CAudioMessageBeep::MatoPrepareComplete(TInt aError)
	{
	if ( aError )
		{
		iPreparedTone = 0;
		}

	iState = EVRToneIdle;

	// Prepare the next tone in queue
	if ( iQueuedTone )
		{
		TRAP_IGNORE( PrepareToneL( iQueuedTone ) );
		iQueuedTone = 0;
		}


	}
	

// ---------------------------------------------------------
// CAudioMessageBeep::MatoPlayComplete
// ---------------------------------------------------------
//
#ifdef _DEBUG
void CAudioMessageBeep::MatoPlayComplete(TInt aError )
    {
	RDebug::Print( _L("AudioMessage: Tone %d played, error %d"), iPreparedTone, aError );
#else
void CAudioMessageBeep::MatoPlayComplete(TInt /*aError*/ )
    {
#endif    	

	iState = EVRToneIdle;

	// Prepare the next tone in queue
	if ( iQueuedTone )
		{
		TRAP_IGNORE( PrepareToneL( iQueuedTone ) );
		iQueuedTone = 0;
		}	
	}	
