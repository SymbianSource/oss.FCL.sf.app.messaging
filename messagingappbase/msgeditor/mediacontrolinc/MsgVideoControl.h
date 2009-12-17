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
* Description:  
*       MsgEditor video media control - a Message Editor Base control.
*
*/



#ifndef MSGVIDEOCONTROL_H
#define MSGVIDEOCONTROL_H

// INCLUDES
#include <msgmediacontrol.h>
#include <videoplayer.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CVideoPlayerUtility;
class CPeriodic;
class CMsgVideoAreaControl;

// CLASS DECLARATION

// ==========================================================

/**
* Message Editor Base control handling images in MMS
*/
class CMsgVideoControl : public CMsgMediaControl,
                         public MVideoPlayerUtilityObserver,
                         public MVideoLoadingObserver,                         
                         public MCoeControlHitTest
    {
    public:  // Constructor and destructor
        
        
        enum TVideoFlags
            {
            ENoPlayback = 0x1 // Video control is not used for video playback
            };
        
        /**
        * Two-phased constructor.
        * @param aParent parent control
        * @param aFlags display flags - by default editor state
        * @param aPlaybackSupported Indicates whether control 
        * @return new object
        */
        IMPORT_C static CMsgVideoControl* NewL( CMsgEditorView& aParent,
                                                MMsgAsyncControlObserver* aObserver,
                                                TMdaPriority aPriority,
                                                TMdaPriorityPreference  aPreference,
                                                TInt aFlags = 0 );

        /**
        * Destructor.
        */
        virtual ~CMsgVideoControl();

    public:

        /**
        * From MMsgAsyncControl
        *
        * Loads the video
        *
        * @param aFileHandle image file handle - expected to be valid videoclip
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
        * Close video.
        */
        void Close();
        
        /**
        * From CMsgMediaControl    
        *
        * Start video.
        * Video must be loaded AND prepared before calling this!
        */
        void PlayL();

        /**
        * From CMsgMediaControl    
        *
        * Stop video.
        */
        void Stop();

        /**
        * From CMsgMediaControl    
        *
        * Pause video.
        */
        void PauseL();
        
        /**
        * Gives the resolution of the video in display
        * @return scaled resolution
        */
        IMPORT_C TSize VideoResolutionL() const;
        
        /**
        * Returns ETrue if control is off screen.
        */
        IMPORT_C TBool IsOffScreen() const;

        /**
        * Check if video clip has audio in.
        * @return ETrue if there is audio.
        */
        IMPORT_C TBool HasAudioL() const;

        /**
        * Set volume level
        * @param aVolume    Volume level wanted range 0..10.
        * @return           Volume level after adjustment.
        */
        IMPORT_C void SetVolumeL( TInt aVolume );

        /**
        * Get current volume level
        * @return   Current volume level
        */
        inline TInt Volume() const;

        /**
        * Get maximum volume level
        * @return   Maximum volume level
        */
        inline TInt MaxVolume() const;

        /**
        * Set continuation property for focus movement.
        * @param aContinue  Boolean value to decide whether playing
        *                   is continued when focus comes back to region.
        */
        inline void ContinueOnFocus( TBool aContinue );
        
        /**
        * Returns ETrue if video file contains video stream.
        * @return ETrue if video stream is present on video file.
        */
        inline TBool HasVideoStream() const;

    public: // From CCoeControl

        /**
        * Draw Control
        * From CCoeControl
        */
        void Draw( const TRect& aRect ) const;
        
        /**
        * Handles resource change events.
        * From CCoeControl
        */
        void HandleResourceChange( TInt aType );

    public: // Functions from MVideoPlayerUtilityObserver
    
        /** 
        * From MVideoPlayerUtilityObserver 
        */
        void MvpuoOpenComplete( TInt aError );

        /** 
        * From MVideoPlayerUtilityObserver 
        */
        void MvpuoPrepareComplete( TInt aError );

        /** 
        * From MVideoPlayerUtilityObserver 
        */
        void MvpuoFrameReady( CFbsBitmap& aFrame ,TInt aError );

        /** 
        * From MVideoPlayerUtilityObserver 
        */
        void MvpuoPlayComplete( TInt aError );
        
        /** 
        * From MVideoPlayerUtilityObserver 
        */
        void MvpuoEvent( const TMMFEvent& aEvent );
    
        /** 
        * From MVideoPlayerUtilityObserver 
        */
        void MvloLoadingStarted();
    
        /** 
        * From MVideoPlayerUtilityObserver 
        */
        void MvloLoadingComplete();

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

        /**
        * Called when size is changed.
        * From CMsgBaseControl
        */
        void SizeChanged();
    
    public: // from MCoeControlHitTest
    
        /**
        * Evaluates whether control is "hit" by touch event.
        */
        TBool HitRegionContains( const TPoint& aPoint, const CCoeControl& aControl ) const;
        
    public:

        /**
        * Callback for CIdle
        */
        static TInt DelayedPlay( TAny* aThis );

    protected: // from CCoeControl

        /**
        * Called when focus is changing
        * From CCoeControl
        */
        void FocusChanged( TDrawNow aDrawNow );

    private:
        
        /**
        * C++ default constructor.
        */
        CMsgVideoControl();
        
        /**
        * constructor
        * @param aFlags
        */
        CMsgVideoControl( MMsgBaseControlObserver& aBaseControlObserver,
                          TMdaPriority aPriority,
                          TMdaPriorityPreference  aPreference,
                          TInt aFlags );
            
        /**
        * By default Symbian constructor is private.
        */
        void ConstructL( CMsgEditorView& aParent, MMsgAsyncControlObserver* aObserver );

        /**
        * Sets the correct size for video frame.
        */
        void SetVideoFrameSizeL();

        /**
        * Handles callback or other completion of an operation.
        * Calls DoHandleCallbackL
        * @param aError     error code
        * @param aNewState  new state assigned to iState, if aError is KErrNone:
        *                   if aError indicates real error, state is not assigned
        */
        void HandleCallback( TInt aError, 
                             TMsgAsyncControlState aNewState = EMsgAsyncControlStateReady);
        
        /**
        * Callback function to do inactivity timer resetting.
        */
        static TInt DoResetInactivityTimer( TAny* aObject );
        
        /**
        * Updates scaling factor
        */
        void UpdateScaleFactorL();
        
    private: //Data

        CVideoPlayerUtility*    iVideoPlayer;
        TRect                   iMaxVideoArea;
        TBool                   iContinueOnFocus;
        TInt                    iVolume;
        TInt                    iMaxVolume;
        
        CIdle*                  iIdle;
        
        RFile                   iVideoFile;
        TSize                   iOriginalSize;
        
        TInt                    iFlags;
        
        CPeriodic*              iTimer;        
        TBool                   iHasVideo;
        
        TRect                   iVideoFrameArea;
        
        TMdaPriority            iPriority;
        TMdaPriorityPreference  iPreference;
        
        TReal                   iScaleFactor;
        CMsgVideoAreaControl*	iVideoAreaControl;
    };


#include <msgvideocontrol.inl>

#endif // MSGVIDEOCONTROL_H
