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
* Description:   
*       This class contains all audio the handling for NcnList
*
*/




#ifndef NCNHANDLERAUDIO_H
#define NCNHANDLERAUDIO_H

//  INCLUDES
#include <mdaaudiosampleplayer.h>   // For playing the alert tone
#include <AudioPreference.h>

#include    <MProfile.h>
#include    <MProfileEngine.h>
#include    <MProfileTones.h>
#include    <MProfileExtraTones.h>
#include    <TProfileToneSettings.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CNcnModelBase;

// CLASS DECLARATION

/**
*  This class contains all the audio handling in NcnList
*/
class CNcnHandlerAudio : public CBase, 
                         public MMdaAudioPlayerCallback
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CNcnHandlerAudio* NewL( CNcnModelBase* aModel );
        
        /**
        * Destructor.
        */
        virtual ~CNcnHandlerAudio();

    public: // New functions
        
        /**
        * Start playing the tone
        * @since    Series60 2.1
        * @param    aMessageType   Type of received message
        * @return   KErrNone or error number
        */
        TInt PlayAlertToneL( const TUint& aMessageType ); 

        /**
        * From MMdaAudioPlayerCallback 
        *   Stops playing the audio
        * @since    Series60 2.1
        */
        void StopMsgReceivedTonePlaying();

    public: // Functions from base classes

        /**
        * From MMdaAudioPlayerCallback 
        *   Completes playing the file
        * @since    Series60 2.1
        * @param    aError      Errorcode for failed cases
        * @return   aDuration   Duration of audiosample
        */
        void MapcInitComplete(
            TInt                                aError, 
            const TTimeIntervalMicroSeconds&    aDuration);

        /**
        * From MMdaAudioPlayerCallback 
        *   Stops playing file
        * @since    Series60 2.1
        * @param    aError      Errorcode for failed cases
        * @return ?description
        */
        void MapcPlayComplete( TInt aError );
        
    protected:  // New functions        
    protected:  // Functions from base classes
    private:

        /**
        * C++ default constructor.
        */
        CNcnHandlerAudio( CNcnModelBase* aModel );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:

        /**
        * Fetches the sound and volume for received messagetype
        * @since    Series60 2.1
        * @param    aProfile Current profile.
        * @param    aVolume     The variable to store the volume
        * @param    aMessageType Type of received message (sms or email)
        * @return   KErrNone when no problems found
        */
        TInt GetSoundFileL(
            const MProfile& aProfile, 
            TFileName&      aSound, 
            const TUint&    aMessageType ) const;
        
        /**
        * Removes the audio player and sends required information to shareddata
        * @since    Series60 2.1
        */
        void RemoveAudioPlayer();

        /**
        * Plays next alerttone from queue
        * @since    Series60 2.1
        * @return   KErrNone, when no errors.
        */
        TInt PlayAlertToneFromQueueL();

	    /**
        * Handles audio initialisation errors.
        * @since    Series60 2.6
        * @return   KErrNone, when no errors.
        */
		TInt PlayDefaultAlertToneL();
		
		/**
		 * Returns ETrue if beep-once is set in profile. If it is,
		 * beep sequence should be played instead of message alert tone.
		 * @param Active profile.
		 * @return ETrue if beep-once is set in profile.
		 */
		TBool IsBeepOnceSetL( const MProfile& aProfile ) const;	
		 /* Plays Alert tone */
		void CNcnHandlerAudio::StartPlayToneL();	
		
    public:     // Data   
    protected:  // Data
    private:    // Data

        // A pointer to the CNcnModel instance
        CNcnModelBase* iNcnModel;
        // A pointer to the audio player instance
        CMdaAudioPlayerUtility* iAudioPlayer;
        
        // A pointer to the MProfileEngine instance
        MProfileEngine* iProfileEngine;               
        // Tone to be played
        RArray<TUint> iToneQueue;
        // State of audioplayer
        TUint iAlertTonePlaying;
		// Play default tone
		TBool iDefaultTonePlayed;
		//The tone that is selected to be played
		TFileName iSelectedTone;
    };

#endif      // NCNHANDLERAUDIO_H   
            
// End of File
