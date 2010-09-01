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
*       MsgEditor audio media control.
*
*/



#ifndef MSGAUDIOCONTROL_H
#define MSGAUDIOCONTROL_H

// INCLUDES

#include <msgmediacontrol.h>
#include <mdaaudiosampleplayer.h>

// CONSTANTS

// MACROS
// TODO simulate video playing in the emulator
// TODO If enabled play preview visible in the options menu
//#define _SIMULATE_EMBEDDED_PLAY 1

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CMsgEditorView;

// CLASS DECLARATION

// ==========================================================

/**
* Message Editor Base control handling audio
*/
class CMsgAudioControl : public CMsgMediaControl,
                         public MMdaAudioPlayerCallback
    {
    public:  // Constructor and destructor
        
        /**
        * Two-phased constructor.
        * @param aParent parent control
        * @param aFlags display flags - by default editor state
        * @return new object
        */
        IMPORT_C static CMsgAudioControl* NewL( CMsgEditorView& aParent,
                                                MMsgAsyncControlObserver* aObserver,
                                                TMdaPriority aPriority = EMdaPriorityNormal,
                                                TMdaPriorityPreference aPreference = EMdaPriorityPreferenceNone );
        
        /**
        * Destructor.
        */
        virtual ~CMsgAudioControl();

    public:

        /**
        * From MMsgAsyncControl
        *
        * Loads the audio
        *
        * @param aAudioFile image file name - expected to be valid Audioclip
        */
        void LoadL( RFile& aFileHandle );
        
        /**
        * From MMsgAsyncControl
        *
        * Cancels the asyncronous operation if it is pending.
        */         
        void Cancel();
        
        /**
        * From MMsgAsyncControl
        *
        * Close audio.
        */
        void Close();
        
        /**
        * From CMsgMediaControl
        *
        * Start audio playback.
        * Audio must be loaded AND prepared before calling this!
        */
        void PlayL();

        /**
        * From CMsgMediaControl
        *
        * Stop audio.
        */
        void Stop();

        /**
        * From CMsgMediaControl
        *
        * Pause audio.
        */
        void PauseL();

        /**
        * Get current volume level
        * @return   Current volume level
        */
        inline TInt Volume();

        /**
        * Get maximum volume level
        * @return   Maximum volume level
        */
        inline TInt MaxVolume();

        /**
        * Set volume level
        * @param aVolume    Volume level wanted range 0..10.
        * @return           Volume level after adjustment.
        */
        inline void SetVolume( TInt aVolume );

    public: // Functions from MMdaAudioPlayerCallback

        /**
        * From MMdaAudioPlayerCallback
        */
        void MapcInitComplete( TInt aError, const TTimeIntervalMicroSeconds& aDuration );

        /**
        * The actual implementation of MapcInitComplete
        */
        void DoMapcInitCompleteL( TInt aError, const TTimeIntervalMicroSeconds& aDuration );

        /**
        * From MMdaAudioPlayerCallback
        */
        void MapcPlayComplete( TInt aError );

        /**
        * The actual implementation of MapcPlayComplete
        */
        void DoMapcPlayCompleteL( TInt aError );

    public: // from CMsgBaseControl

        /**
        * Calculates and sets the size of the control and returns new size as
        * reference aSize.
        * From CMsgBaseControl
        */
        void SetAndGetSizeL( TSize& aSize );

        /**
        * Resets (= clears) contents of the control.
        * From CMsgBaseControl
        */
        void Reset();

    public:

        /**
        * Called when size is changed.
        * From CMsgBaseControl
        */
        void SizeChanged();

    private: // New functions

        /**
        * Default C++ constructor
        */
        CMsgAudioControl();
                          
        /**
        * constructor
        * @param aFlags
        */
        CMsgAudioControl( MMsgBaseControlObserver& aBaseControlObserver,
                          TMdaPriority aPriority,
                          TMdaPriorityPreference aPreference );
        
        /**
        * By default Symbian constructor is private.
        */
        void ConstructL( CMsgEditorView& aParent, MMsgAsyncControlObserver* aObserver );

        /**
        * Handles callback or other completion of an operation
        * @param aError     error code
        * @param aNewState  new state assigned to iState, if aError is KErrNone:
        *                   if aError indicates real error, state is not assigned
        */
        void HandleCallback( TInt    aError, 
                             TBool   aDefaultStop,
                             TMsgAsyncControlState aNewState = EMsgAsyncControlStateReady );

    private: //Data

        CMdaAudioPlayerUtility* iAudioPlayer;
        RFile                   iAudioFile;
        
        TMdaPriority iPriority;
        TMdaPriorityPreference iPreference;
    };


#include <msgaudiocontrol.inl>

#endif // MSGAUDIOCONTROL_H
