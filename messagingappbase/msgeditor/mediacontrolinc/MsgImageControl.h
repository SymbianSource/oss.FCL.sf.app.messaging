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
*       MsgEditor Image media control - a Message Editor Base control
*
*/



#ifndef MSGIMAGECONTROL_H
#define MSGIMAGECONTROL_H

// INCLUDES
#include <msgmediacontrol.h>
#include <MIHLViewerObserver.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMsgEditorView;
class MIHLFileImage;
class MIHLBitmap;
class MIHLImageViewer;
class TAknsItemID;
class CMsgBitmapControl;

// CLASS DECLARATION

// ==========================================================

/**
* Message Editor Base control for handling images
*/
class CMsgImageControl : public CMsgMediaControl,
                         public MIHLViewerObserver,
                         public MCoeControlHitTest
    {

    public:  // Constructor and destructor

        /**
        * Two-phased constructor.
        * @param aObserver Control state observer.
        * @param aParent   Parent control.
        *
        * @return new object
        */
        IMPORT_C static CMsgImageControl* NewL( CMsgEditorView& aParent, 
                                                MMsgAsyncControlObserver* aObserver );

        /**
        * Destructor.
        */
        virtual ~CMsgImageControl();

    public:
        
        /**
        * From MMsgAsyncControl
        *
        * Load media to from given file handle to media control.
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
        * Closes control. Closing means freeing file
        * specific resources that other application can use the file.
        */         
        void Close();
        
        /**
        * From CMsgMediaControl    
        *
        * Shows the image in the viewer
        */
        void PlayL();
        
        /**
        * From CMsgMediaControl    
        *
        * Pauses playing media including animations. 
        */         
        void PauseL();
        
        /**
        * From CMsgMediaControl    
        *
        * Stops playing media including animations. 
        */         
        void Stop();
        
        /**
        * Return ETrue if loaded image is animation.
        */
        IMPORT_C TBool IsAnimation() const;
        
        /**
        * Sets how many time animation should loop.
        * 
        * @param aCount Animation loop count (-1 == indefinite).
        */
        IMPORT_C void SetAnimationLoopCount( TInt aCount );
        
        /**
        *
        */
        IMPORT_C void SetImageFileClosed( );
        
        /**
        * Loads the image into display.
        * @param aImageFile image file name - expected to be valid image
        * @param aMaxHeight maximum height of the displayed image - default is
        *                   0 = {max display size or max thumbnail size}.
        */
        inline void SetMaxHeight( TInt aMaxHeight );

        /**
        * Loads the image into display.
        * @param aImageFile image file name - expected to be valid image
        * @param aMaxSize maximum height and width of the displayed image
        */
        inline void SetMaxSize( TSize aMaxSize );
        
        /**
        * returns the framesize
        */
        IMPORT_C TSize FrameSize();
        
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
        * Handles control events from view.
        *
        * @param aEvent     IN      Event type
        * @param aParam     IN      Event related parameters.
        */
        void NotifyViewEvent( TMsgViewEvent aEvent, TInt aParam );
    
    public: // from CCoeControl
        
        /**
        * This is called when the focus of the control is changed.
        * @param aDrawNow
        */
        void FocusChanged( TDrawNow aDrawNow );
        
        /**
        * Called when size is changed.
        * From CCoeControl
        */
        void SizeChanged();
        
        /**
        * From CCoeControl,Draw.
        * @param aRect draw rect
        */
        void Draw(const TRect& aRect) const;
        
        /**
        * From CCoeControl. MakeVisible
        * @param aVisible
        */
        void MakeVisible( TBool aVisible );
        
        /**
        * From CCoeControl. PositionChanged
        * @param aVisible
        */
        void PositionChanged();
        
        /**
        * From CCoeControl. Handles resource change events.
        * @param aType
        */
        void HandleResourceChange( TInt aType );
        
    public: // From MIHLViewerObserver
        
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
        * @since 3.0
        *
        * @param aError     IN  System wide error code.
        *
        * @return void
		*/
		void ViewerError( TInt aError );
    
    public: // from MCoeControlHitTest
    
        /**
        * Evaluates whether control is "hit" by touch event.
        */
        TBool HitRegionContains( const TPoint& aPoint, const CCoeControl& aControl ) const;
        
    protected: // from CCoeControl
        
        /**
        * Return count of controls be included in this component
        * From CCoeControl
        */
        TInt CountComponentControls() const;

        /**
        * Return pointer to component in question.
        * From CCoeControl
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;
        
    private:
        
        /**
        * Calculates correct centered image position.
        */
        TPoint CalculateImagePosition() const;
        
        /**
        * Calculates the size for the image control.
        * @param aProposedSize size given by the framework. Our size is either this or smaller.
        * @return new size for the control
        */
        TSize CalculateControlSize( const TSize& aProposedSize ) const;
        
        /**
        * Returns ETrue if control is off screen.
        */
        TBool IsOffScreen() const;

        /**
        * Original size of image/icon bitmap
        */
        TSize BitmapSize() const;
        
        /**
        * Performs icon loading steps.
        */
        void DoIconLoadingL( const TAknsItemID& aId,
                             const TDesC& aFileName,
                             const TInt aFileBitmapId,
                             const TInt aFileMaskId );

    private: // Constructors

        /**
        * C++ default constructor.
        */
        CMsgImageControl();
                          
        /**
        * constructor
        * @param aFlags - CMsgImageControl::TFlags
        * @param aBaseControlObserver - observer to notify about changes.
        */
        CMsgImageControl( MMsgBaseControlObserver& aBaseControlObserver );
                          
        /**
        * By default Symbian constructor is private.
        * @param aParent - parent control.
        */
        void ConstructL( CMsgEditorView& aParent,
                         MMsgAsyncControlObserver* aObserver );

        /**
        * Reopens every media. Restarts possible animation.
        * @param aMainContent is ignored 
        */
        void DoReloadL( TBool aMainContent = ETrue );
        
        /**
        * Performs play functionality.
        */
        void DoPlay( TInt aAnimationStartDelay ); 
        
        /**
        * Performs pause functionality.
        */
        void DoPause();
        
        /**
        * Calculates maximum size for image control.
        */
        void CalculateMaxSize(); 
        
        /**
        * Starts animation playback.
        */
        void StartAnimation( TInt aAnimationStartDelay );
        
        /**
        * Stops animation playback.
        */
        void StopAnimation();
        
        /**
        * Pauses animation playback.
        */
        void PauseAnimation();
        
        /**
        * Callback function to start animation loop
        */
        static TInt DoStartLoop( TAny* aObject );
        
        /**
        * Starts new animation loop
        */
        void StartLoop();
        
    private: //Data

        enum TAnimationState
            {
            EAnimationNotReady,
            EAnimationStopped,
            EAnimationPlaying,
            EAnimationPaused
            };
        
        // Source image
        MIHLFileImage*      iSourceImage;
        
        // Destination bitmap
        MIHLBitmap*         iDestinationBitmap;

        // Image viewer used as engine
        MIHLImageViewer*    iEngine;
                
        // Maximum control size.
        TSize               iMaxSize;

        // Internal animation state.
        TAnimationState     iAnimationState;
        
        // Maximum loop count for animations.
        TInt                iLoopCount;
        
        // Current loop number.
        TInt                iCurrentLoop;

        // Control that contains the image bitmap.
        CMsgBitmapControl*  iBitmapControl;
        
        // Loop delay timer.
        CPeriodic*          iLoopTimer;
        
        TInt                iMsgMainPaneHeight;
    };
    
#include <msgimagecontrol.inl>

#endif // MSGIMAGECONTROL_H
