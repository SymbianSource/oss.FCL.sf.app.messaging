/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       SMIL Player media renderer for playing audio files
*
*/



#ifndef CSMILAUDIORENDERER_H
#define CSMILAUDIORENDERER_H

//  INCLUDES
#include <mdaaudiosampleplayer.h>
#include <f32file.h>
#include "SmilMediaRendererBase.h"

// FORWARD DECLARATIONS
class CMdaAudioPlayerUtility;

// CLASS DECLARATION

/**
* SMIL Player media renderer for playing audio files
*
* @lib smilmediarenderer.lib
* @since 2.0
*/
NONSHARABLE_CLASS(CSmilAudioRenderer) : public CSmilMediaRendererBase,
                                        public MMdaAudioPlayerCallback
    {

    public: // Constructors and destructor
        
        /**
        * Factory method that creates this object.
        *
        * @param    aFileHandle  IN Handle to the media file
        * @param    aMedia       IN SMIL engine side object representing
        *                           this media file
        * @param    aDrmCommon   IN DRM Common
        * @param    aDrmHelper   IN DRM Helper
        * @param    aPlayPreview IN Indicates whether only preview should be played. 
                                    Default is EFalse.
        *
        * @return   pointer to instance
        */
        static CSmilAudioRenderer* NewL( RFile& aFileHandle, 
                                         MSmilMedia* aMedia, 
                                         DRMCommon& aDrmCommon,
                                         CDRMHelper& aDrmHelper );
        
        /**
        * Destructor.
        */
        virtual ~CSmilAudioRenderer();
        
    public: // Functions from base classes
        
        /**
        * From MSmilMediaRenderer Returns the duration of the media object.
        *
        * @since 2.0
        *
        * @return Returns duration of audio clip.
        */
        TSmilTime IntrinsicDuration() const;
        
        /**
        * From MSmilMediaRenderer Called by the engine to signal that
        * the media is going to be played soon.
        *
        * @since 2.0
        *
        * @return void
        */
        void PrepareMediaL();       
        
        /**
        * From MSmilMediaRenderer Called by the engine to seek the media
        * to given position in its local timeline.
        *
        * @since 2.0
        * 
        * @param aTime  IN  Seek position.
        *
        * @return void
        */
        void SeekMediaL( const TSmilTime& aTime );
        
        /**
        * From MSmilMediaRenderer Called by the engine to start media
        * playback and to make visual media visible.
        *
        * @since 2.0
        * 
        * @return void
        */
        void ShowMediaL();
        
        /**
        * From MSmilMediaRenderer Called by the engine to stop media
        * playback and remove visual media from the screen.
        *
        * @since 2.0
        *
        * @return void
        */
        void HideMedia();
        
        /**
        * From MSmilMediaRenderer Called by the engine to pause media playback.
        *
        * @since 2.0
        *
        * @return void
        */
        void FreezeMedia();
        
        /**
        * From MSmilMediaRenderer Called by the engine to continue paused
        * media playback.
        *
        * @since 2.0
        *
        * @return void
        */
        void ResumeMedia();
        
        /**
        * From MSmilMediaRenderer Set volume of the audio media.
        *
        * @since 2.0
        *
        * @param aVolume    IN  A percentage value from maximum value that
        *                       the volume should be set.
        * @return void
        */
        void SetVolume( TInt aVolume );
        
        /**
        * From MMdaAudioPlayerCallback Called when audio clip initialization completes.
        *
        * @since 2.0
        *
        * @param aError    IN  Resulting error code.
        * @param aDuration IN Audio clip duration.
        *
        * @return void
        */
        void MapcInitComplete( TInt aError,
                               const TTimeIntervalMicroSeconds& aDuration );
        
        /**
        * From MMdaAudioPlayerCallback Called when audio clip playback completes.
        *
        * @since 2.0
        *
        * @param aError    IN  Resulting error code.
        *
        * @return void
        */
        void MapcPlayComplete( TInt aError );
        
    private:

        /**
        * C++ default constructor.
        */
        CSmilAudioRenderer();
        
        /**
        * Constructor.
        */
        CSmilAudioRenderer( MSmilMedia* aMedia, 
                            DRMCommon& aDrmCommon,
                            CDRMHelper& aDrmHelper );
        
        /**
        * 2nd phase constructor
        */
        void ConstructL( RFile& aFileHandle );
        
        
    private:   // Data

        // Internal states.
        enum TState
            {
            ENotReady = 0,
            EOpening,
            EOpen,
            EPlaying,
            EPaused,
            EHidden
            };

        // Duration.
        TSmilTime iIntrinsicDuration;
        
        // Audio player utility.
        CMdaAudioPlayerUtility* iAudioPlayer;
        
        // Internal state.
        TInt iState;
        
        // Operation result.
        TInt iError;
        
        // Indicates renderer end.
        TBool iRendererAtEnd;
        
        // Handle to media file. 
        // Duplicate made because of late preparation
        RFile iMediaFile;
    };
    
#endif // CSMILAUDIORENDERER_H
