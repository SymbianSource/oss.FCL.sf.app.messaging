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
*        Implementation of SmilMediaRenderer, a image renderer.
*
*/



#ifndef IMAGE_RENDERER_H
#define IMAGE_RENDERER_H

//  INCLUDES
#include <smilmediarendererinterface.h>
#include <smiltime.h>
#include <smilactiveinterface.h>
#include <MIHLViewerObserver.h>
#include <eiksbfrm.h>

#include "SmilMediaRendererBase.h"


// FORWARD DECLARATIONS
class MSmilFocus;
class MIHLFileImage;
class MIHLBitmap;
class MIHLImageViewer;

// CLASS DECLARATION

/**
* CSmilImageRenderer provides SMIL media renderer interface for
* image source files.
* This class is used by SMIL Engine to perform the image rending.
*
* @lib smilmediarenderer.lib
* @since 2.0
*/
NONSHARABLE_CLASS(CSmilImageRenderer) : public CSmilMediaRendererBase,
                                        public MIHLViewerObserver,
                                        public MSmilActive,
                                        public MEikScrollBarObserver
    {
    public:  // Methods

    // Constructors and destructor

        /**
        * Static constructor.
        *
        * @since 2.0
        * @param aFileHandle IN     Source file handle.
        * @param aMedia      IN     SMIL media object.
        * @param aDrmCommon  IN     DRM Common
        * @param aDrmHelper  IN     DRM Helper
        *
        * @return Initialized image renderer object.
        */
        static CSmilImageRenderer* NewL( RFile& aFileHandle,
                                         MSmilMedia* aMedia,
                                         DRMCommon& aDrmCommon,
                                         CDRMHelper& aDrmHelper );

        /**
        * Destructor.
        *
        * @since 2.0
        */
        virtual ~CSmilImageRenderer();
        
    public: // From MSmilMediaRenderer

       /**
        * From MSmilMediaRenderer Called by the SMIL engine to signal
        * that the engine is no more referencing renderer.
        *
        * @since 2.0
        *
        * @return void
        */
        void Close();

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
        * @return Returns EFalse if source image is non-transparent
        * otherwise returns ETrue.
        */
        TBool IsOpaque() const;

        /**
        * From MSmilMediaRenderer Returns if renderer is scrollable.
        *
        * @since 2.0
        *
        * @return Returns ETrue if image is scrollable and EFalse if
        * it is not.
        */
        TBool IsScrollable() const;

        /**
        * From MSmilMediaRenderer Returns if renderer is Symbian control.
        *
        * @since 2.0
        *
        * @return Returns ETrue since all the renderers are implemented
        * as controls.
        */
        TBool IsControl() const;

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
        * @return Returns duration of image (Note! zero duration means still
        * image).
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
                   const MSmilFocus* aFocus );

        /**
        * From MSmilMediaRenderer Move scrollable media to given position.
        *
        * @since 2.0
        *
        * @param aDirX  IN  Amount of pixels to move on X direction.
        * @param aDirY  IN  Amount of pixels to move on Y direction.
        *
        * @return void
        */
        void Scroll( TInt aDirX, TInt aDirY );
   
        /**
        * from MEikScrollBarObserver
        *   Handles the scrollevents
        */
        void HandleScrollEventL( CEikScrollBar* aScrollBar, 
                        TEikScrollEvent aEventType );
              
    protected: // From MSmilActive

        /**
        * Called when the requested time has been reached. 
        * 
        * @since 3.0
        *
        * @param aTime      IN Current time, relative to the requesting media obect.
        */
        void ActivateL( const TSmilTime& aTime );
   
    protected: // From MIHLViewerObserver
        
        /**
        * From MIHLViewerObserver Notifies client when viewer bitmap content is changed.
        * Leave situation is handled in ViewerError() callback.
        *
        * @since 3.0
        *
        * @return void
        */
        void ViewerBitmapChangedL();

        /**
        * From MIHLViewerObserver Notifies client if error occurs in viewer.
        * 
        * @since 2.0
        *
        * @param aError     IN  System wide error code.
        *
        * @return void
        */
        void ViewerError( TInt aError );

    private: //Types
        
        // Scrolling directions.
        enum TDirection
            {
            EScrollUp,
            EScrollDown,
            EScrollLeft,
            EScrollRight
            };
        
        // Internal states.
        enum TState
            {
            ENotReady = 0,
            EReady,
            EPlaying,
            EPaused,
            EHidden,
            EError
            };
            
    private: //Methods

        /**
        * C++ default constructor
        */
        CSmilImageRenderer();

        /**
        * Constructor.
        */
        CSmilImageRenderer( MSmilMedia* aMedia,
                            DRMCommon& aDrmCommon,
                            CDRMHelper& aDrmHelper );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( RFile& aFileHandle );

        /**
        * Creates viewer and configures it.
        */
        void InitializeViewerL();
        
        /**
        * Converts the specified frame.
        */
        void ConvertFrameL( TInt aFrame );

        /**
        * Initializes the scroll bars.
        */
        void SetupScrollingL();
        
        /**
        * Calculate zoom ratio for meet fit attribute.
        */
        TReal CalculateMeetRatio( const TSize& aRegion, const TSize& aImage ) const;
    
        /**
        * Calculate zoom ratio for slice fit attribute.
        */
        TReal CalculateSliceRatio( const TSize& aRegion, const TSize& aImage ) const;
    
        /**
        * Sets internal state variables
        */
        void SetState( TInt aNewState );
        
        /**
        * Checks if scrolling to specified direction is possible.
        */
        TBool IsScrollingPossible( TDirection aDirection ) const;
        
        /**
        * Calculates single scrolling step size to specified direction.
        */
        void CalculateScrollStepSize( TDirection aDirection,
                    TSize& aScrollingStep, TInt aStepSize = 1 ) const;

        /**
        * Draws the scrollbar(s)
        */                        
        void DrawScrollBarL();
        
        /**
        * Scrolls without redrawing the thumbs on the scrollbar
        * 
        * @param aDirX  IN  Amount of pixels to move on X direction.
        * @param aDirY  IN  Amount of pixels to move on Y direction.
        *
        * @return void
        */
        void ScrollWithOutThumbUpdate( TInt aDirX, TInt aDirY );
        
    private:    // Data
        
        // Source image
        MIHLFileImage* iSourceImage;
        
        // Destination bitmap
        MIHLBitmap* iDestinationBitmap;

        // Image viewer used as engine
        MIHLImageViewer* iEngine;

        // Total duration of the animation.
        TSmilTime iTotalLength;

        // Error flag
        TInt iError;

        // Specifies whether renderer is scrollable.
        TBool iIsScrollable;
        
        // Scrolling rectangle.
        TRect iImageScrollRect;

        // Previous internal state.
        TInt iPrevState;
        
        // Current internal state.
        TInt iCurrState;
        
        // Specifies the time taken by some operation.
        TTimeIntervalMicroSeconds iProcessingDelay;
        
        // Image bitmap size
        TSize iSourceSize;
        
        // Indicates whether image is animation. Value is
        // stored because we lose this information when
        // iEngine and iSourceImage are deleted for memory
        // optimization.
        TBool iAnimation;
        
#ifdef RD_SCALABLE_UI_V2
        // Scrollbar frame
        CEikScrollBarFrame* iSBFrame;
        
        //indicator whether or not the scrollbar needs to be
        // initialised and drawn
        TBool iNotYetDrawn;
        
        //save the thumb position if replay is needed
        TInt iThumbPosX;
        
        //save the thumb position if replay is needed
        TInt iThumbPosY;
        
        //from scrollbar, member because maxvalue of scrollbar 
        // is uninitialised 
        TInt iScrollSpanY;
        TInt iScrollSpanX;
#endif // RD_SCALABLE_UI_V2      
    };

#endif // IMAGE_RENDERER_H
            
// End of File
