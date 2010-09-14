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
*       MsgEditor image UI control - a Message Editor Base control.
*
*/



// INCLUDE FILES
#include "MsgImageControl.h"

#include <DRMCommon.h>
#include <AknUtils.h>               // AknsUtils
#include <AknsDrawUtils.h>          // AknsDrawUtils
#include <aknlayoutscalable_apps.cdl.h>

#include <MsgEditorView.h>          // CMsgEditorView
#include <MsgEditorCommon.h>

#include <IHLImageFactory.h>        // IHLImageFactory
#include <IHLViewerFactory.h>       // IHLViewerFactory
#include <MIHLFileImage.h>          // MIHLFileImage
#include <MIHLBitmap.h>             // MIHLBitmap
#include <MIHLImageViewer.h>        // MIHLImageViewer
#include <icl/imagecodecdata.h>     // KImageTypeBMPUid

#include "MsgFrameControl.h"
#include "MsgIconControl.h"
#include "MsgBitmapControl.h"

#include "MsgMediaControlLogging.h"

// ==========================================================

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
const TInt KAnimationStartDelay = 500000; // 0,5s
const TInt KAnimationDelayBetweenLoops = 1000000; // 1s

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgImageControl::CMsgImageControl
//
// Constructor.
// ---------------------------------------------------------
//
CMsgImageControl::CMsgImageControl( MMsgBaseControlObserver& aBaseControlObserver ) : 
    CMsgMediaControl( aBaseControlObserver, EMsgComponentIdImage, EMsgImageControl )
    {
    }

// ---------------------------------------------------------
// CMsgImageControl::ConstructL
// ---------------------------------------------------------
//
void CMsgImageControl::ConstructL( CMsgEditorView& aParent, MMsgAsyncControlObserver* aObserver )
    {
    BaseConstructL( aParent, aObserver );    
    
    iBitmapControl = new( ELeave ) CMsgBitmapControl;
    iBitmapControl->SetContainerWindowL( *this );
    
    CalculateMaxSize();

    TRect rect( aParent.Rect() );
    if ( rect.Height() == 0 || rect.Width() == 0 )
        {
        // We have been called before view construction.
        // Set rect to screen size.
        rect.SetRect( TPoint(), iMaxSize );
        }

    SetRect( rect );
    
    iLoopTimer = CPeriodic::NewL( EPriorityNormal );
    }

// ---------------------------------------------------------
// CMsgImageControl::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgImageControl* CMsgImageControl::NewL( CMsgEditorView& aParent,
                                                   MMsgAsyncControlObserver* aObserver )
    {
    CMsgImageControl* self = new( ELeave ) CMsgImageControl( aParent );
    
    CleanupStack::PushL( self );
    self->ConstructL( aParent, aObserver );
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CMsgImageControl::~CMsgImageControl
//
// Destructor
// ---------------------------------------------------------
//
CMsgImageControl::~CMsgImageControl()
    {
    delete iEngine;
    delete iDestinationBitmap;
    delete iSourceImage;
    delete iBitmapControl;
    delete iLoopTimer;
    }
    
// ---------------------------------------------------------
// CMsgImageControl::LoadL
// ---------------------------------------------------------
//
void CMsgImageControl::LoadL( RFile& aFileHandle )
    {
    Reset();
    
    iSourceImage = IHLImageFactory::OpenFileImageL( aFileHandle, 
                                                    0,  // image index
                                                    MIHLFileImage::EOptionNoDRMConsume );
    
    SetState( EMsgAsyncControlStateOpening );
    
    iDestinationBitmap = IHLBitmap::CreateL();
            
    TSize sourceSize( iSourceImage->Size() );
    TSize targetSize = TSize(
    	Min( sourceSize.iWidth, iMaxSize.iWidth - iFrame->FrameBorderSize().iWidth ),
		Min( sourceSize.iHeight, iMaxSize.iHeight - iFrame->FrameBorderSize().iHeight ) );

    // In case of KImageTypePNGUid and KImageTypeBMPUid 
    // with and without IsAnimation, call CreateImageViewerL with
    // Flags zero. This to avoid image corruption  when IsAnimation is flase and 
    // Image resize happened based on control size.

    TUint32 flagOptions = 0;
    
    if ( !iSourceImage->IsAnimation() &&
           iSourceImage->ImageType() == KImageTypeGIFUid )
        {
        	flagOptions |= MIHLImageViewer::EOptionUseBilinearInterpolation;
      	}
 	  iEngine = IHLViewerFactory::CreateImageViewerL( targetSize,
                                                    *iSourceImage, 
                                                    *iDestinationBitmap, 
                                                    *this, 
                                                    flagOptions);                        	
    }

// ---------------------------------------------------------
// CMsgImageControl::Cancel
// ---------------------------------------------------------
//
void CMsgImageControl::Cancel()
    {
    if ( iState == EMsgAsyncControlStateOpening )
        {
        Reset();
        
        SetState( EMsgAsyncControlStateIdle );
        }
    }

// ---------------------------------------------------------
// CMsgImageControl::PlayL
// ---------------------------------------------------------
//
void CMsgImageControl::PlayL()
    {
    if ( IsAnimation() &&
         iState >= EMsgAsyncControlStateOpening )
        {
        DoPlay( 0 );
        }
    }

// ---------------------------------------------------------
// CMsgImageControl::PauseL
// ---------------------------------------------------------
//
void CMsgImageControl::PauseL()
    {
    if ( IsAnimation() && 
         iState >= EMsgAsyncControlStateReady )
        {
        DoPause();
        }
    }
     
// ---------------------------------------------------------
// CMsgImageControl::Stop
// ---------------------------------------------------------
//
void CMsgImageControl::Stop()
    {    
    if ( IsAnimation() && 
         iState >= EMsgAsyncControlStateReady )
        {
        StopAnimation();
    
        SetState( EMsgAsyncControlStateStopped );
        }
    }

// ---------------------------------------------------------
// CMsgImageControl::Close
// ---------------------------------------------------------
//
void CMsgImageControl::Close()
    {
    Stop();
    SetState( EMsgAsyncControlStateIdle );
    }

// ---------------------------------------------------------
// CMsgImageControl::IsAnimation
//
// Return ETrue if loaded image is animation.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgImageControl::IsAnimation() const
    {
    TBool result( EFalse );
    
    if ( iSourceImage )
        {
        result = iSourceImage->IsAnimation();
        }
        
    return result;
    }

// ---------------------------------------------------------
// CMsgImageControl::SetAnimationLoopCount
//
// Sets animation loop count (i.e. how many times animation
// is played fully through). -1 means that it is looped forever.
// ---------------------------------------------------------
//
EXPORT_C void CMsgImageControl::SetAnimationLoopCount( TInt aCount )
    {
    iLoopCount = aCount;
    }

// ---------------------------------------------------------
// CMsgImageControl::SetImageFileOpenOrClose
//
// Sets image file open or close.
// ---------------------------------------------------------
//
EXPORT_C void CMsgImageControl::SetImageFileClosed( )
    {
    if ( iSourceImage )
        {
        Stop();
        
        delete iSourceImage;
        iSourceImage = NULL;
        
        delete iEngine;
        iEngine = NULL;
        
        SetState( EMsgAsyncControlStateIdle );
        }
    }

// ---------------------------------------------------------
// CMsgImageControl::SetAndGetSizeL
//
// Calculates and sets the size of the control and returns new size as
// reference aSize.
// ---------------------------------------------------------
//
void CMsgImageControl::SetAndGetSizeL( TSize& aSize )
    {
    // Adjust max height if needed
    if ( aSize.iHeight > iMaxSize.iHeight )
        {
        aSize.iHeight = iMaxSize.iHeight;
        }

    // Do not adjust the width. The width of the control must always
    // be the same as the width of the client rect.
    aSize = CalculateControlSize( aSize );

    SetSize( aSize );
    }


// ---------------------------------------------------------
// CMsgImageControl::NotifyViewEvent
//
// Animation playback is paused if view looses focus and resumed
// when focus is again gained. Resuming happens only if animation is
// still at pause state and control is on playing state.
// ---------------------------------------------------------
//
void CMsgImageControl::NotifyViewEvent( TMsgViewEvent aEvent, TInt aParam )
    {
    switch ( aEvent )
        {
        case EMsgViewEventViewFocusLost:
            {
            if ( iAnimationState == EAnimationPlaying )
                {
                PauseAnimation();
                }
            break;
            }
        case EMsgViewEventViewFocusGain:
            {
            if ( iAnimationState == EAnimationPaused &&
                 iState == EMsgAsyncControlStatePlaying )
                {
                StartAnimation( KAnimationStartDelay );
                }
            break;
            }
        default:
            {
            break;
            }
        }
        
    CMsgMediaControl::NotifyViewEvent( aEvent, aParam );
    }

// ---------------------------------------------------------
// CMsgImageControl::Reset
//
// Empties the control.
// ---------------------------------------------------------
//
void CMsgImageControl::Reset()
    {
    delete iEngine;
    iEngine = NULL;
    
    iAnimationState = EAnimationStopped;
    
    delete iDestinationBitmap;
    iDestinationBitmap = NULL;
    
    iBitmapControl->SetBitmap( NULL );
    
    delete iSourceImage;
    iSourceImage = NULL;
    
    ResetIconId();
    }

// ---------------------------------------------------------
// CMsgImageControl::FocusChanged
//
// Updates the focus (i.e. set frame control visible/invisible).
// Calls base class implementation if icon is visible. Otherwise
// performs it's own handling.
// Frame control is redraw immediatelly if that 
// is requested and frame control state is changed. Otherwise
// normal deferred drawing is done. This is activated at 
// CCoeControl::MakeVisible if needed.
// ---------------------------------------------------------
//
void CMsgImageControl::FocusChanged( TDrawNow aDrawNow )
    {
    if ( VisiblePlaceholder() )
        {
        CMsgMediaControl::FocusChanged( aDrawNow );
        }
    else 
        {
        TBool oldVisibility( iFrame->IsVisible() );
    
        iFrame->MakeVisible( IsFocused() );
        
        if ( aDrawNow == EDrawNow &&
             iFrame->IsVisible() != oldVisibility )
            {
            DrawNow();
            }
        }
    }

// ---------------------------------------------------------
// CMsgImageControl::SizeChanged
//
// Called when size is changed.
// ---------------------------------------------------------
//
void CMsgImageControl::SizeChanged()
    {
    if ( iEngine && 
         Size() != iEngine->ViewerSize() )
        {
        TSize bitmapSize = iEngine->SourceSize() ;
        TSize controlSize = iSize;
        controlSize.iWidth -= iFrame->FrameBorderSize().iWidth;
        controlSize.iHeight -= iFrame->FrameBorderSize().iHeight;
        
        TReal widthRatio = static_cast<TReal>( controlSize.iWidth ) / 
                           static_cast<TReal>( bitmapSize.iWidth );
        TReal heightRatio = static_cast<TReal>( controlSize.iHeight ) / 
                            static_cast<TReal>( bitmapSize.iHeight );
        TReal zoomRatio = ( widthRatio < heightRatio ) ? widthRatio : heightRatio;
        
        iEngine->SetViewerSize( Size() );
        iEngine->SetZoomRatio( zoomRatio );
        }
    
    // Size updated at the ViewerBitmapChanged when image scaling is done.
    iBitmapControl->SetPosition( Position() + CalculateImagePosition() );
        
    if ( iDestinationBitmap && 
         iDestinationBitmap->IsCreated() )
        {
        iFrame->SetImageSize( iDestinationBitmap->Bitmap().SizeInPixels() );
        }
    
    if ( iState == EMsgAsyncControlStatePlaying )
        {
        // Start animation if needed on playing state.
        StartAnimation( 0 );
        }
        
    CMsgMediaControl::SizeChanged();
    }

// ---------------------------------------------------------
// CMsgImageControl::Draw
//
// Draws the basic skin background for given rectangle. 
// Image is drawn on CBitmapControl::Draw function.
// ---------------------------------------------------------
//
void CMsgImageControl::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    
    if ( !VisiblePlaceholder() )
        {
        if ( !AknsDrawUtils::BackgroundBetweenRects( AknsUtils::SkinInstance(), 
                                                 AknsDrawUtils::ControlContext( this ), 
                                                 this, 
                                                 gc, 
                                                 Rect(),
                                                 iBitmapControl->Rect(),
                                                 KAknsDrawParamNoClearUnderImage ) )
            {
            gc.SetBrushColor( AKN_LAF_COLOR( 0 ) );
            gc.SetPenStyle( CGraphicsContext::ENullPen );
    		gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        	gc.DrawRect( aRect );
            }
        }
    else 
        {
        // Basic icon drawing from base class when icon is visible.
        CMsgMediaControl::Draw( aRect );
        }
    }

// ---------------------------------------------------------
// CMmsImageControl::MakeVisible
//
// Sets same visibility to bitmap control as image control has.
// ---------------------------------------------------------
//
void CMsgImageControl::MakeVisible( TBool aVisible )
    {
    iBitmapControl->MakeVisible( aVisible );
    
    CMsgMediaControl::MakeVisible( aVisible );
    }

// ---------------------------------------------------------
// CMmsImageControl::PositionChanged
//
// Calculates new position to bitmap control as image control position
// has changed.
// ---------------------------------------------------------
//
void CMsgImageControl::PositionChanged()
    {
    iBitmapControl->SetPosition( Position() + CalculateImagePosition() );
    
    CMsgMediaControl::PositionChanged();
    }

// ---------------------------------------------------------
// CMsgImageControl::HandleResourceChange
// ---------------------------------------------------------
//
void CMsgImageControl::HandleResourceChange( TInt aType )
    {
    CMsgMediaControl::HandleResourceChange( aType ); 
    
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        CalculateMaxSize();
        
        if ( iEngine )
            {
            TSize sourceSize( iSourceImage->Size() );
            TSize targetSize = TSize(
                    Min( sourceSize.iWidth, iMaxSize.iWidth - iFrame->FrameBorderSize().iWidth ),
                    Min( sourceSize.iHeight, iMaxSize.iHeight - iFrame->FrameBorderSize().iHeight ) );
            
            iBitmapControl->SetSize( TSize( 0, 0 ) );
            SetSize( targetSize );
            }
        }
    }

// ---------------------------------------------------------
// CMmsImageControl::CountComponentControls
//
// Return count of controls be included in this component.
// ---------------------------------------------------------
//
TInt CMsgImageControl::CountComponentControls() const
    {    
    return 2;
    }

// ---------------------------------------------------------
// CMmsImageControl::ComponentControl
//
// Return pointer to component in question.
// Note! Image and Icon cannot exist at a same time.
// ---------------------------------------------------------
//
CCoeControl* CMsgImageControl::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* result = NULL;
    switch ( aIndex )
        {
        case 0:
            {
            result = iFrame;                
            break;
            }
        case 1:
            {
            CCoeControl* visiblePlaceholder = VisiblePlaceholder();
            if ( visiblePlaceholder )
                {
                result = visiblePlaceholder;
                }
            else
                {
                result = iBitmapControl;
                }
            break;
            }
        default:
            {
            break;
            }
        }
        
    return result;
    }

// ---------------------------------------------------------
// CMsgImageControl::IsOffScreen
//
// Returns ETrue if image is off screen.
// ---------------------------------------------------------
//
TBool CMsgImageControl::IsOffScreen() const
    {
    TRect rect( Position() + CalculateImagePosition(),
                iDestinationBitmap->Bitmap().SizeInPixels() );
    
    TBool result( EFalse );
    
    if ( rect.iTl.iY < 0 || 
         rect.iBr.iY >  MsgEditorCommons::MsgMainPane().Height() )
        {
        result = ETrue;
        }
        
    return result;
    }


// ---------------------------------------------------------
// CMsgImageControl::ViewerBitmapChangedL
//
// Handle bitmap change notifications. State is changed accordingly
// if this is the first frame. GIF animation is stopped if control
// goes off screen and started again when it is fully visible and control
// is on playing state. On editor mode animation is looped only once. 
// Animation is also stopped if animation loop count is reached.
// New animation loop is started after KAnimationDelayBetweenLoops
// delay.
// ---------------------------------------------------------
//
void CMsgImageControl::ViewerBitmapChangedL()
    {
    if ( iState == EMsgAsyncControlStateOpening )   
        {
        SetState( EMsgAsyncControlStateReady );        
        
        if ( iAnimationState == EAnimationPlaying )
            {
            SetState( EMsgAsyncControlStatePlaying );
            }
        }
        
    if ( iSourceImage->IsAnimation() )
        {
        switch ( iAnimationState )
            {
            case EAnimationPlaying:
                {   
                if ( IsOffScreen() )
                    {
                    // Stop the playback if animation is not fully visible
                    StopAnimation();
                    }
                else if ( iEngine->AnimationFrame() == 
                          iSourceImage->AnimationFrameCount() - 1 )
                    {
                    // Last frame of the animation has been loaded.
                    iCurrentLoop++;
                    
                    if ( iCurrentLoop == iLoopCount )
                        {
                        // Maximum loop count reached. Do not stop yet as first
                        // frame is wanted to be displayed. Stopping is done on 
                        // EAnimationStopped state.
                        iAnimationState = EAnimationStopped;
                        }
                    else
                        {
                        iEngine->Stop();
                        
                        iLoopTimer->Cancel();
                        iLoopTimer->Start( KAnimationDelayBetweenLoops,
                                           KAnimationDelayBetweenLoops,
                                           TCallBack( DoStartLoop, this ) );
                        }
                    }
                break;
                }
            case EAnimationStopped:
                {
                if( iState == EMsgAsyncControlStatePlaying &&
                    !IsOffScreen() &&
                    iCurrentLoop != iLoopCount )
                    {
                    StartAnimation( 0 );
                    }
                else
                    {
                    StopAnimation();
                    }
                break;
                }
            case EAnimationNotReady:
            default:
                {
                break;
                }
            }
        }
    
    iBitmapControl->SetBitmap( iDestinationBitmap );
    
    TRect imageRect( Position() + CalculateImagePosition(),
                     iDestinationBitmap->Bitmap().SizeInPixels() );
    iBitmapControl->SetRect( imageRect );
    
    if( IsVisible() )
        {
        // Sometimes iDestinationBitmap is not ready when in SizeChanged()
        if ( iDestinationBitmap &&  
             iDestinationBitmap->IsCreated() )
            {
            iFrame->SetImageSize( imageRect.Size() );
            }  

        DrawNow();
        }
    }

// ---------------------------------------------------------
// CMsgImageControl::ViewerError
//
// Handles engine errors.
// ---------------------------------------------------------
//
void CMsgImageControl::ViewerError( TInt aError )
    {
    MSGMEDIACONTROLLOGGER_WRITEF_ERROR_STATE( _L("CMsgImageControl::HandleCallback: **aError: %d, CurrentState: %S"),aError, iState);
    
    iError = aError;
            
    switch ( aError )
        {
        case KErrNone:
            {
            if ( iState == EMsgAsyncControlStateOpening )
                {
                // image shown in the viewer and image had rights
                SetState( EMsgAsyncControlStateReady );
                DoPlay( 0 );
                }
            else if (  iState == EMsgAsyncControlStatePlaying )
                {
                SetState( EMsgAsyncControlStateReady );
                DoPlay( 0 );
                }
            break;
            }
        case DRMCommon::EGeneralError:
        case DRMCommon::EUnknownMIME:
        case DRMCommon::EVersionNotSupported:
        case DRMCommon::ESessionError:
        case DRMCommon::ENoRights:
        case DRMCommon::ERightsDBCorrupted:
        case DRMCommon::EUnsupported:
        case DRMCommon::ERightsExpired:
        case DRMCommon::EInvalidRights:
            {
            SetState( EMsgAsyncControlStateNoRights );
            break;
            }
        case KErrNoMemory:  // cannot recover
        default:
            {
            SetState( EMsgAsyncControlStateCorrupt );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMsgImageControl::CalculateImagePosition
//
// Calculates image position so that it is centered to 
// control extent.
// ---------------------------------------------------------
//
TPoint CMsgImageControl::CalculateImagePosition() const
    {
    TPoint result;
    
    TSize controlSize( Size() );
    TSize imageSize;
    
    if ( iEngine )
        {
        imageSize = iEngine->SourceSize();
        imageSize.iWidth *= iEngine->ZoomRatio();
        imageSize.iHeight *= iEngine->ZoomRatio();
        }
    else
        {
        // Engine has been unloaded. Use current bitmap control size
        // as imageSize.
        imageSize = iBitmapControl->Size();
        }
    
    result.iX = ( controlSize.iWidth - imageSize.iWidth ) / 2;
    result.iY = ( controlSize.iHeight - imageSize.iHeight ) / 2;
    
    if ( result.iX < 0 )
        {
        result.iX = 0;
        }
    
    if ( result.iY < 0 )
        {
        result.iY = 0;
        }
        
    return result;
    }

// ---------------------------------------------------------
// CMsgImageControl::CalculateControlSize
//
// Calculates the correct size for the image control.
// ---------------------------------------------------------
//
TSize CMsgImageControl::CalculateControlSize( const TSize& aProposedSize ) const
    {
    TSize result( aProposedSize.iWidth, 0 );
    TSize bitmapSize = BitmapSize();
    
    if ( !bitmapSize.iHeight || !bitmapSize.iWidth )
        {
        result.iHeight = aProposedSize.iHeight;
        return result;
        }
    else if ( bitmapSize.iHeight < aProposedSize.iHeight )
        {
        result.iHeight = bitmapSize.iHeight;
        }
    else
        {
        result.iHeight = aProposedSize.iHeight;
        }

    TInt scaledHeight( 
        ( static_cast<TReal>( bitmapSize.iHeight ) /
          static_cast<TReal>( bitmapSize.iWidth ) ) *
          aProposedSize.iWidth );
    if ( scaledHeight < result.iHeight )
        {
        result.iHeight = scaledHeight;
        }

    // Height should be multiple of baseline.
    // Round up to the next full multiple if needed.
    TInt remainder = result.iHeight % iBaseLine;
    if ( remainder )
        {
        result.iHeight += ( iBaseLine - remainder );
        }
        
    if ( result.iHeight > iMaxSize.iHeight )
        {
        result.iHeight = iMaxSize.iHeight;
        }

    return result;
    }

// ---------------------------------------------------------
// CMsgImageControl::BitmapSize
// 
// Returns correct size for bitmap.
// ---------------------------------------------------------
//
TSize CMsgImageControl::BitmapSize() const
    {
    TSize bitmapSize( 0, 0 );
    if ( !iIconControl->IsVisible() &&
         ( iBitmapControl->Size().iWidth == 0 ||
           iBitmapControl->Size().iHeight == 0 ) )
        {
        if ( iEngine )
            {
            bitmapSize = iEngine->SourceSize() ;
            bitmapSize.iWidth += iFrame->FrameBorderSize().iWidth;
            bitmapSize.iHeight += iFrame->FrameBorderSize().iHeight;
            }
        }
    else 
        {
        // Bitmap or icon visible. 
        bitmapSize = iFrame->FrameSize();
        }

    return bitmapSize;
    }

// ---------------------------------------------------------
// CMsgImageControl::DoPlay
//
// Starts animation if possible and sets control to correct state.
// ---------------------------------------------------------
//
void CMsgImageControl::DoPlay( TInt aAnimationStartDelay )
    {
    StartAnimation( aAnimationStartDelay );
    
    if ( iState != EMsgAsyncControlStateOpening )
        {
        SetState( EMsgAsyncControlStatePlaying );
        }
    }

// ---------------------------------------------------------
// CMsgImageControl::DoPause
//
// Stops animation and sets control to correct state.
// ---------------------------------------------------------
//
void CMsgImageControl::DoPause()
    {
    StopAnimation();
    
    SetState( EMsgAsyncControlStatePaused );
    }

// ---------------------------------------------------------
// CMsgImageControl::CalculateMaxSize
// ---------------------------------------------------------
//
void CMsgImageControl::CalculateMaxSize()
    {
    TAknLayoutRect maxSizeLayout;
    maxSizeLayout.LayoutRect( MsgEditorCommons::MsgDataPane(),
                              AknLayoutScalable_Apps::msg_data_pane_g1().LayoutLine() );
    
    iMaxSize = maxSizeLayout.Rect().Size();
    
    // Make sure "iMaxSize" is multiple of "iBaseLine"
    iMaxSize.iHeight = iBaseLine * ( iMaxSize.iHeight / iBaseLine );
    }

// ---------------------------------------------------------
// CMsgImageControl::HitRegionContains
// 
// Note! CONE will do final verification if control is really hit so
//       not need to do it here.
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
TBool CMsgImageControl::HitRegionContains( const TPoint& aPoint, 
                                     const CCoeControl& /*aControl*/ ) const
    {
    TBool result( EFalse );
    
    CCoeControl* visiblePlaceholder = VisiblePlaceholder();
    
    if ( visiblePlaceholder )
        {
        result = visiblePlaceholder->Rect().Contains( aPoint );
        }
    else
        {
        result = iBitmapControl->Rect().Contains( aPoint );
        }
        
    return result;
    }
#else
TBool CMsgImageControl::HitRegionContains( const TPoint& /*aPoint*/, 
                                     const CCoeControl& /*aControl*/ ) const
    {
    return EFalse;
    }
#endif


// ---------------------------------------------------------
// CMsgImageControl::FrameSize
//
// Returns the framesize
// ---------------------------------------------------------
//
EXPORT_C TSize CMsgImageControl::FrameSize()
    {
    return iFrame->FrameSize();
    }

// ---------------------------------------------------------
// CMsgImageControl::StartAnimation
//
// Starts animation if possible.
// ---------------------------------------------------------
//
void CMsgImageControl::StartAnimation( TInt aAnimationStartDelay )
    {
    iLoopTimer->Cancel();
    
    if ( iEngine && 
         IsAnimation() && 
         !IsOffScreen() &&
         iLoopCount != iCurrentLoop )
        {    
        iAnimationState = EAnimationPlaying;
            
        if ( aAnimationStartDelay == 0 )
            {
            iEngine->Play();
            }
        else 
            {
            iLoopTimer->Start( aAnimationStartDelay,
                               aAnimationStartDelay,
                               TCallBack( DoStartLoop, this ) );
            }
        }
    }

// ---------------------------------------------------------
// CMsgImageControl::StopAnimation
//
// Stops animation if possible.
// ---------------------------------------------------------
//
void CMsgImageControl::StopAnimation()
    {
    iLoopTimer->Cancel();
    
    if ( iEngine &&  
         IsAnimation() )
        {
        iEngine->Stop();
        iAnimationState = EAnimationStopped;
        }
    }

// ---------------------------------------------------------
// CMsgImageControl::PauseAnimation
//
// Pauses animation if possible.
// ---------------------------------------------------------
//
void CMsgImageControl::PauseAnimation()
    {
    StopAnimation();
    iAnimationState = EAnimationPaused;
    }

// ---------------------------------------------------------
// CMsgImageControl::DoStartLoop
//
// Start new animation loop.
// ---------------------------------------------------------
//
TInt CMsgImageControl::DoStartLoop( TAny* aObject )
    {
    // cast, and call non-static function
    CMsgImageControl* control = static_cast<CMsgImageControl*>( aObject );
    control->StartLoop();
    
    return KErrNone;
    }

// ---------------------------------------------------------
// CMsgImageControl::StartLoop
//
// Start new animation loop.
// ---------------------------------------------------------
//
void CMsgImageControl::StartLoop()
    {
    if ( iAnimationState == EAnimationPlaying )
        {
        StartAnimation( 0 );
        }
    else
        {
        iLoopTimer->Cancel();
        }
    }

//  End of File
