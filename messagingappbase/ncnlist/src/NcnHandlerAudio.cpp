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
* Description:  NcnHandlerAudio implementation
*
*/



// INCLUDE FILES
#include    <DRMCommon.h>
#include    "NcnHandlerAudio.h"
#include    "NcnModelBase.h"
#include    "NcnModel.h"

#include    "NcnCRHandler.h"
#include    <ProfileEngineDomainCRKeys.h>

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
namespace
    {
    
    const TInt KNcnMaxProfileVolume( 10 );    
    
    // sampled beep sequence
    _LIT8( KNcnSampledBeepSequence,
        "\x2\x4a\x3a\x51\x9\x95\x95\xc0\x4\x0\xb\x1c\x41\x8d\x51\xa8\x0\x0" );
    }

// MACROS

#define SET_VOLUME( t, v )  t->SetVolume( Max( 0, Min( t->MaxVolume() /KNcnMaxProfileVolume *v, t->MaxVolume() ) ) )

// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNcnHandlerAudio::CNcnHandlerAudio
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CNcnHandlerAudio::CNcnHandlerAudio( CNcnModelBase* aModel ) :
    iNcnModel( aModel ), iDefaultTonePlayed( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CNcnHandlerAudio::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNcnHandlerAudio::ConstructL()
    {    
    iProfileEngine = CreateProfileEngineL(); 
    }

// -----------------------------------------------------------------------------
// CNcnHandlerAudio::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNcnHandlerAudio* CNcnHandlerAudio::NewL( CNcnModelBase* aModel )
    {
    // Create instance of 
    CNcnHandlerAudio* self = 
        new( ELeave ) CNcnHandlerAudio( aModel );
    
    // Push into cleanup stack and construct
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    // Return the pointer
    return self;
    }

    
// Destructor
CNcnHandlerAudio::~CNcnHandlerAudio()
    {
    if( iProfileEngine )
        {
        iProfileEngine->Release();
        }

    RemoveAudioPlayer();
    iToneQueue.Close();
    iAudioPlayer = NULL;
    iNcnModel = NULL;
    iProfileEngine = NULL;
    }
    


// -----------------------------------------------------------------------------
// CNcnHandlerAudio::MdapcInitComplete
// -----------------------------------------------------------------------------
//
void CNcnHandlerAudio::StartPlayToneL()
{
	// Update volume
	MProfile* profile = NULL;
	TRAPD( err,  profile = iProfileEngine->ActiveProfileL() );
	if( err == KErrNone && profile != NULL )
	   {
	    CleanupReleasePushL( *profile ); // CSI: 42 # Should not leave
	    
	    // Set volume
		NCN_RDEBUG_INT( _L("CNcnHandlerAudio::MapcInitComplete: Audio player's max volume is %d"), iAudioPlayer->MaxVolume() );        
		NCN_RDEBUG_INT( _L("CNcnHandlerAudio::MapcInitComplete: Profile volume is %d"), profile->ProfileTones().ToneSettings().iRingingVolume );
	 	SET_VOLUME( iAudioPlayer, profile->ProfileTones().ToneSettings().iRingingVolume );     	
	 	    
	    // Set time
	    TTimeIntervalMicroSeconds time = TTimeIntervalMicroSeconds( 0 );

	    // Set repeats and start playing
	    iAudioPlayer->SetRepeats( 0, time );
	    iAudioPlayer->Play();

	    // Set playing state and inform shared date
	    iAlertTonePlaying = iToneQueue[0];
	    iNcnModel->MsgReceivedTonePlaying( iAlertTonePlaying );
	    
		CleanupStack::PopAndDestroy( profile );    	
	   }              
}

// -----------------------------------------------------------------------------
// CNcnHandlerAudio::MdapcInitComplete
// -----------------------------------------------------------------------------
//
void CNcnHandlerAudio::MapcInitComplete(
    TInt                                aError, 
    const TTimeIntervalMicroSeconds&    /* aDuration */ )
    {
	NCN_RDEBUG_INT(_L("CNcnHandlerAudio::MapcInitComplete:  %d"), aError );
    // Audio player is initialised, so if there is no error, 
    // we can start playing the tone.
    if( aError == KErrNone )
        {
      	 TRAP_IGNORE(StartPlayToneL());
        
        }
    // Otherwise, play default tone and if there is an error in initialisation then
	// go to next tone in quenue.
    else
        {
		if ( iDefaultTonePlayed )
			{
			// Play next tone from queue
			iDefaultTonePlayed = EFalse;
			MapcPlayComplete( aError );
			}
		else
			{
			// Leave should not cancel the whole operation, 
            // so the error is trapped.
			iDefaultTonePlayed = ETrue;
            TRAP_IGNORE( PlayDefaultAlertToneL() );
			}
        }
    }

// -----------------------------------------------------------------------------
// CNcnHandlerAudio::PlayDefaultAlertToneL
// -----------------------------------------------------------------------------
//
TInt CNcnHandlerAudio::PlayDefaultAlertToneL()
	{
	// This method plays default tone if there was a problem in loading original tone.
	TInt err = KErrNone;

	// Remove current audioplayer.
	delete iAudioPlayer;
    iAudioPlayer = NULL;

   // Get soundfile according to received message
	if( iToneQueue[0] == CNcnModelBase::EIndexNewEmails )
        {
		// Default Email tone not defined (?), use the default message alert
		err = iNcnModel->GetCRString(
			KCRUidProfileEngine,
			KProEngDefaultMessageTone,
			iSelectedTone );
		}
	else
		{
		err = iNcnModel->GetCRString(
			KCRUidProfileEngine,
			KProEngDefaultMessageTone,
			iSelectedTone );
		}


	if( err )
		{
		return err;
		}

	iAudioPlayer = CMdaAudioPlayerUtility::NewFilePlayerL( 
		iSelectedTone,
		*this, 
		KAudioPriorityRecvMsg, 
		static_cast<TMdaPriorityPreference>( KAudioPrefNewSMS ) );
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CNcnHandlerAudio::MdapcPlayComplete
// -----------------------------------------------------------------------------
//
void CNcnHandlerAudio::MapcPlayComplete(TInt aError)
    {
	NCN_RDEBUG_INT(_L("CNcnHandlerAudio::MapcPlayComplete: %d"), aError );

	if ( aError == DRMCommon::ENoRights ||
         aError == DRMCommon::ERightsDBCorrupted ||
         aError == DRMCommon::ERightsExpired ||
         aError == DRMCommon::EInvalidRights ||
         aError == KErrArgument )
		{
		// Leave should not cancel the whole operation, 
        // so the error is trapped.
		iDefaultTonePlayed = ETrue;
        TRAP_IGNORE( PlayDefaultAlertToneL() );
		}
	else
		{
		// When playing is completed, audioplayer can be deleted
		RemoveAudioPlayer();

		// Remove the played tone from queue
		iToneQueue.Remove( 0 );

		// Check if another sound needs to be played and play it
		TRAPD( err, PlayAlertToneFromQueueL() );

		// Check if there was error trying to play alert tone and try once again
		if( err )
		    {
			NCN_RDEBUG_INT(_L("CNcnHandlerAudio::MapcPlayComplete: PlayAlertToneFromQueueL err %d"), err );
		    TRAP( err, PlayAlertToneFromQueueL() );
		    }
		}
    }

// -----------------------------------------------------------------------------
// CNcnHandlerAudio::StopMsgReceivedTonePlaying
// -----------------------------------------------------------------------------
//
void CNcnHandlerAudio::StopMsgReceivedTonePlaying()
    {
    // This method is called when the user has pressed some key during the playing...
    // Called from the model.

    // So check if the audioplayer is on and it is playing
    if( iAudioPlayer && iAlertTonePlaying )
        {
        // Stop playing and remove sound from queue
        iAudioPlayer->Stop();
        iToneQueue.Reset();        
        RemoveAudioPlayer();
        }
    }

// -----------------------------------------------------------------------------
// CNcnHandlerAudio::RemoveAudioPlayer
// -----------------------------------------------------------------------------
//
void CNcnHandlerAudio::RemoveAudioPlayer()
    {
	NCN_RDEBUG(_L("CNcnHandlerAudio::RemoveAudioPlayer"));

    // If other sound is waiting in queue, remove it
    iAlertTonePlaying = 0;

    // Delete player and remove address
    delete iAudioPlayer;
    iAudioPlayer = NULL;

    // Inform shared data that nothing is going to be played
    iNcnModel->MsgReceivedTonePlaying( iAlertTonePlaying );
    }

// -----------------------------------------------------------------------------
// CNcnHandlerAudio::PlayAlertToneL
// -----------------------------------------------------------------------------
//
TInt CNcnHandlerAudio::PlayAlertToneL( const TUint& aMessageType )
    {
    // First check if the tone is already playing
    if ( iToneQueue.Find( aMessageType ) != KErrNotFound )
        {
		NCN_RDEBUG(_L("CNcnHandlerAudio::PlayAlertToneL msg type found"));
        return KErrAlreadyExists;
        }

    // Set alert tone waiting
    iToneQueue.Append( aMessageType );
    
    // If the audioplayer already exists we don't play sound as it
    // will be played, after the other sound is played
    if( iAudioPlayer )
        {
		NCN_RDEBUG(_L("CNcnHandlerAudio::PlayAlertToneL iAudioPlayer already exists"));
        return KErrAlreadyExists;
        }

    // Play the alert tone
    return PlayAlertToneFromQueueL();
    }
    
// -----------------------------------------------------------------------------
// CNcnHandlerAudio::PlayAlertToneFromQueueL
// -----------------------------------------------------------------------------
//
TInt CNcnHandlerAudio::PlayAlertToneFromQueueL()
    {
    // Check if there is alert tone that needs to be played
    if( iToneQueue.Count() )
        {   
		NCN_RDEBUG_INT(_L("CNcnHandlerAudio::PlayAlertToneFromQueueL iToneQueue.Count() is %d"), iToneQueue.Count());
		
        // Get active profile
        MProfile* profile = iProfileEngine->ActiveProfileL();
        CleanupReleasePushL( *profile );
    
		// if beep-once is set instantiate player for audio sequence
		if( IsBeepOnceSetL( *profile ) )
		    {
            //Debug message    
  		    NCN_RDEBUG(_L("CNcnHandlerAudio::PlayAlertToneFromQueueL : Beep-once set, playing sequence.."));		      	    
	        
	        // create new descriptor audio player	    
		    iAudioPlayer = CMdaAudioPlayerUtility::NewDesPlayerReadOnlyL(
		        KNcnSampledBeepSequence(), *this, KAudioPriorityRecvMsg, 
                static_cast<TMdaPriorityPreference>( KAudioPrefNewSMS ) );
            iDefaultTonePlayed = EFalse;
		    }
		// otherwise load tone from file
		else
		    {		  		    
            // Get correct sound and volume
            TInt err = GetSoundFileL(
                *profile, iSelectedTone, iToneQueue[0] );
 
            //Debug message    
         	NCN_RDEBUG_INT(_L("CNcnHandlerAudio::PlayAlertToneFromQueueL : Beep-once not set, playing tone: %S"), &iSelectedTone);
         	
            // If fetching went fine, play the files
            if( !err )
                {
                // Create the audio player
                iAudioPlayer =  CMdaAudioPlayerUtility::NewFilePlayerL( 
                    iSelectedTone, *this, KAudioPriorityRecvMsg,
                    static_cast<TMdaPriorityPreference>( KAudioPrefNewSMS ) );
    			iDefaultTonePlayed = EFalse;
                }
    		else
    			{
    			NCN_RDEBUG_INT(_L("CNcnHandlerAudio::PlayAlertToneFromQueueL: GetSoundFileAndVolumeL err %d"), err );
    			}
		    }
		    
		// Release profile
        CleanupStack::PopAndDestroy( profile );
        }
	else
		{
		NCN_RDEBUG(_L("CNcnHandlerAudio::PlayAlertToneFromQueueL iToneQueue is empty"));
		}
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CNcnHandlerAudio::GetSoundFileAndVolumeL
// -----------------------------------------------------------------------------
//
TInt CNcnHandlerAudio::GetSoundFileL(
    const MProfile& aProfile,
    TFileName&      aSound, 
    const TUint&    aMessageType ) const
    {
    // Get soundfile according to received message
    if( aMessageType == CNcnModelBase::EIndexNewEmails )
        {        
        // Sound for email
        aSound = aProfile.ProfileExtraTones().EmailAlertTone();
        }
    else
        {      
        // Sound for other type of messages
        aSound = aProfile.ProfileTones().MessageAlertTone();
        }
        
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CNcnHandlerAudio::IsBeepOnceSetL
// -----------------------------------------------------------------------------
//
TBool CNcnHandlerAudio::IsBeepOnceSetL( const MProfile& aProfile ) const
    {
    // default to false
    TBool ret = EFalse;
    
    // get tone settings    
    const TProfileToneSettings& toneSettings = aProfile.ProfileTones().ToneSettings();
    
    // if beep-once is set, set return value to ETrue
    if( toneSettings.iRingingType == EProfileRingingTypeBeepOnce )
        {
        ret = ETrue;
        }    
    
    return ret;
    }
    

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
