/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       SMIL Player media renderer for playing video files
*
*/



#ifndef SMILVIDEORENDER_H
#define SMILVIDEORENDER_H

//  INCLUDES
#include <videoplayer.h>
#include "SmilMediaRendererBase.h"

// FORWARD DECLARATIONS
class CFbsBitmap;

// CLASS DECLARATION

/**
* SMIL Player media renderer for playing video files
*
* @lib smilmediarenderer.lib
* @since 2.0
*/
NONSHARABLE_CLASS(CSmilVideoRenderer) : public CSmilMediaRendererBase,
                                        public MVideoPlayerUtilityObserver,
                                        public MVideoLoadingObserver
    {
    public: // Constructors and destructor

        /**
        * Factory method that creates this object.
        *
        * @param    aFileHandle    IN File handle to the media file
        * @param    aMedia         IN SMIL engine side object representing
        *                             this media file
        * @param    aDrmCommon     IN DRM Common
        * @param    aDrmHelper     IN DRM Helper
        *
        * @return   pointer to instance
        */
        static CSmilVideoRenderer* NewL( RFile& aFileHandle,
                                         MSmilMedia* aMedia,
                                         DRMCommon& aDrmCommon,
                                         CDRMHelper& aDrmHelper );

        /**
        * Destructor.
        */
        virtual ~CSmilVideoRenderer();

    public: // Functions from base classes

        /**
        * From MSmilMediaRenderer Returns if renderer is visual 
        * (i.e. draws something to the screen).
        *
        * @since 2.0
        * 
        * @return Returns ETrue since the media type is visual.
        */
        TBool IsVisual() const;

        /**
        * From MSmilMediaRenderer Returns if renderer is non-transparent
        * (i.e. fills all the pixels on specied region).
        *
        * @since 2.0
        *
        * @return Returns ETrue.
        *
        */
        TBool IsOpaque() const;

        /**
        * From MSmilMediaRenderer Returns the unscalable size of the
        * visual media object.
        *
        * @since 2.0
        * 
        * @return Width in pixels.
        */
        TInt IntrinsicWidth() const;

        /**
        * From MSmilMediaRenderer Returns the unscalable size of the
        * visual media object.
        *
        * @since 2.0
        * 
        * @return Height in pixels.
        */
        TInt IntrinsicHeight() const;

        /**
        * From MSmilMediaRenderer Returns the duration of the media object.
        *
        * @since 2.0
        *
        * @return Returns duration of video.
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
        * From MSmilMediaRenderer Called by the engine to make a media
        * renderer draw its content.
        *
        * @since 2.0
        *
        * @param aGc    IN      Graphical context performing the drawing.
        * @param aRect  IN      Target rectangle.
        * @param aTransitionFilter IN Transition effects if present.
        * @param aFocus IN      Focus indication if focussed.
        *
        * @return void
        */
        void Draw( CGraphicsContext& aGc,
                   const TRect& aRect,
                   CSmilTransitionFilter* aTransitionFilter,
                   const MSmilFocus* aFocus);
        
        /**
        * From MVideoPlayerUtilityObserver
        *
        * @since 2.0
        *
        * @param aError IN
        *
        * @return void
        */
        void MvpuoOpenComplete( TInt aError );

        /**
        * From MVideoPlayerUtilityObserver
        *
        * @since 2.0
        *
        * @param aError IN
        *
        * @return void
        */
        void MvpuoPrepareComplete( TInt aError );

        /**
        * From MVideoPlayerUtilityObserver
        *
        * @since 2.0
        *
        * @param aFrame IN
        * @param aError IN
        *
        * @return void
        */
        void MvpuoFrameReady( CFbsBitmap& aFrame, TInt aError );

        /**
        * From MVideoPlayerUtilityObserver
        *
        * @since 2.0
        *
        * @param aError IN
        *
        * @return void
        */
        void MvpuoPlayComplete(TInt aError);
            
        /**
        * From MVideoPlayerUtilityObserver
        *
        * @since 2.0
        *
        * @param aEvent IN
        *
        * @return void
        */
        void MvpuoEvent( const TMMFEvent& aEvent );
        
        /**
        * From MVideoLoadingObserver
        *
        * @since 2.0
        *
        * @return void
        */
        void MvloLoadingStarted();
    
        /**
        * From MVideoLoadingObserver
        *
        * @since 2.0
        *
        * @return void
        */
        void MvloLoadingComplete();
    
    private: // From CCoeControl
    
        /**
        * Draws the control.
        */
        void Draw( const TRect& aRect ) const;
        
    private:

        /**
        * C++ default constructor.
        */
        CSmilVideoRenderer();
        
        /**
        * Constructor.
        */
        CSmilVideoRenderer( MSmilMedia* aMedia,
                            DRMCommon& aDrmCommon,
                            CDRMHelper& aDrmHelper );

        /**
        * 2nd phase constructor
        */
        void ConstructL( RFile& aFileHandle );    

        /**
        * LoadVideoL
        */
        void LoadVideoL();

        /**
        * DoSetVolumeL
        *
        * @param aVolume
        */
        void DoSetVolumeL( TInt aVolume );
        
        /**
        * AdjustRects
        */
         void AdjustRects( TRect& aRect, TRect& aClipRect ) const;

        /**
        * UpdateDrawingArea
        */
        void UpdateDrawingArea();
    
        /**
        * IsClipEndingL
        */
        TBool IsClipEndingL() const;
    
        /**
        * UpdateSoftKeyVisibility
        */
        void UpdateSoftKeyVisibility();
        
        /**
        * DoPause
        */
        void DoPause();
        
        /**
        * Returns control rectangle relative to parent control rectangle.
        */
        TRect ControlRect() const;
         
        /**
        * Returns video rectangle relative to screen.
        */ 
        TRect VideoRect() const; 
        
        /**
        * Returns ETrue if presentation is playing.
        */
        TBool PresentationPlaying() const;
        
        /**
        * Performs reopening steps.
        */
        void DoLatePreparationL();

        /**
        * Returns ETrue if video is played for the whole presentation
        * from it's start to end.
        */
        TBool PlayedForWholePresentation() const;
        
        /**
        * Performs resuming.
        */
        void DoResumeL();
        
    private: // data

        enum TState
            {
            ENotReady = 0,
            EReady,
            ELoading,
            EPlaying,
            EPaused,
            EHidden,
            EError
            };

        // Playback engine.
        CVideoPlayerUtility* iVideoPlayer;
        
        // Duration.
        TSmilTime iIntrinsicDuration;

        // Internal state.
        TInt iState;
        
        // Internal error code.
        TInt iError;

        // Current clipping rectangle.
        TRect iCurrentClipRect;
        
        // Operation start time.
        TTime iStartTime;
    
        // Determines whether to use late preparation.
        TBool iLatePreparation;
    
        // Specifies the time taken by some operation.
        TTimeIntervalMicroSeconds iProcessingDelay;
        
        // Handle to media file. 
        // Duplicate made because of late preparation
        RFile iMediaFile;
        
        // Parent control for video control
        CCoeControl* iParent;

        // Position where pausing happened.
        TTimeIntervalMicroSeconds iPausePosition;
        
        // Old volume to remember 
        TInt iOldVolume;
    };

#endif // SMILVIDEORENDER_H

