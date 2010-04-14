/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "MsgVideoControl.h"

#include <AudioPreference.h>
#include <AknsDrawUtils.h>
#include <DRMCommon.h>
#include <mmf/common/mmferrors.h>              // KErrMMPartialPlayback

#include <MsgEditorCommon.h>

#include "MsgFrameControl.h"
#include "MsgIconControl.h"
#include "MsgVideoAreaControl.h"

#include "MsgMediaControlLogging.h"

// ==========================================================

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

//used to reset inactivity timer after one sec so that backlight will not
// be turned on during video playback.
const TInt KResetInactivityAfterOneSecond = 2000000;   // 2s

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgVideoControl::CMsgVideoControl
//
// Constructor. Maximum video area is calculated so that height comes from 
// mainpane height and width comes from data pane height. This video will
// use whole application area excluding scrollbar area.
// ---------------------------------------------------------
//
CMsgVideoControl::CMsgVideoControl( MMsgBaseControlObserver& aBaseControlObserver,
                                    TMdaPriority aPriority,
                                    TMdaPriorityPreference  aPreference,
                                    TInt aFlags ) :
    CMsgMediaControl( aBaseControlObserver, EMsgComponentIdVideo, EMsgVideoControl ),
    iMaxVideoArea( MsgEditorCommons::MsgMainPane() ),
    iMaxVolume( 1 ),
    iFlags( aFlags ),
    iHasVideo( ETrue ),
    iPriority( aPriority ),
    iPreference( aPreference )
    {
    if ( iFlags & ENoPlayback )
        {
        iControlType = EMsgImageControl;
        }
        
    iMaxVideoArea.SetWidth( MsgEditorCommons::MsgDataPane().Width() );
    }

// ---------------------------------------------------------
// CMsgVideoControl::ConstructL
// ---------------------------------------------------------
//
void CMsgVideoControl::ConstructL( CMsgEditorView& aParent, MMsgAsyncControlObserver* aObserver )
    {
    BaseConstructL( aParent, aObserver );
        
    SetRect( iMaxVideoArea );
    
    iTimer = CPeriodic::NewL( EPriorityLow );
    }

// ---------------------------------------------------------
// CMsgVideoControl::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgVideoControl* CMsgVideoControl::NewL( CMsgEditorView& aParent,
                                                   MMsgAsyncControlObserver* aObserver,
                                                   TMdaPriority aPriority,
                                                   TMdaPriorityPreference  aPreference,
                                                   TInt aFlags )
    {
    CMsgVideoControl* self = new( ELeave ) CMsgVideoControl( aParent, aPriority, aPreference, aFlags );
    
    CleanupStack::PushL( self );
    self->ConstructL( aParent, aObserver );
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CMsgVideoControl::~CMsgVideoControl
//
// Destructor
// ---------------------------------------------------------
//
CMsgVideoControl::~CMsgVideoControl()
    {
    delete iVideoPlayer;
    delete iIdle;
    
    iVideoFile.Close();
    
    delete iTimer;
    delete iVideoAreaControl;
    }


// ---------------------------------------------------------
// CMsgVideoControl::HasAudio
//
// Check if video clip has audio in.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgVideoControl::HasAudioL( void ) const
    {
    if ( iVideoPlayer && Error() == KErrNone )
        {
        return iVideoPlayer->AudioEnabledL();
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CMsgVideoControl::SetVolume
//
// Set volume level.
// ---------------------------------------------------------
//
EXPORT_C void CMsgVideoControl::SetVolumeL( TInt aVolume )
    {
    MSGMEDIACONTROLLOGGER_WRITEF( _L("MsgVideoControl::SetVolumeL(): HasAudioL(): %d, iMaxVolume: %d, aVolume: %d"),  HasAudioL(), iMaxVolume, aVolume);
    if ( HasAudioL() )
        {
        iVideoPlayer->SetVolumeL( aVolume );
        iVolume = iVideoPlayer->Volume();
        }
    }
    
// ---------------------------------------------------------
// CMsgVideoControl::FocusChanged
//
// Called when focus is changing. Playing is paused or stopped
// depending on iContinueOnFocus flag when focus moves out of video control. 
// When focus moved back to control playing is started automatically
// if iContinueOnFocus flag is set.
// ---------------------------------------------------------
//
void CMsgVideoControl::FocusChanged( TDrawNow aDrawNow )
    {
    MSGMEDIACONTROLLOGGER_WRITEF( _L("MsgVideoControl : FocusChanged: IsFocused(): %d" ), IsFocused() );
    if ( IsFocused() )
        {
        if ( iState == EMsgAsyncControlStatePaused &&
             iContinueOnFocus )
            {
            MSGMEDIACONTROLLOGGER_WRITE( "MsgVideoControl : FocusChanged - Delayed start **" );
            // We must use CIdle to perform Play since
            // "SizeChanged" event comes only after "FocusChanged"
            // and thus the coordinates of the video control might
            // be false at this stage.
            delete iIdle;
            iIdle = NULL;
            TRAP_IGNORE( iIdle = CIdle::NewL( CActive::EPriorityIdle ) );
            if ( iIdle ) 
                {
                iIdle->Start( TCallBack( DelayedPlay, this ) );
                }
            else
                {
                MSGMEDIACONTROLLOGGER_WRITE( "MsgVideoControl : FocusChanged - Error in creating iIdle **" );
                Stop();
                }
            
            }
        }
    else
        {
        if ( iContinueOnFocus && 
             iState == EMsgAsyncControlStatePlaying )
            {
            MSGMEDIACONTROLLOGGER_WRITE( "MsgVideoControl : FocusChanged - Pausing video ..." );
            TRAPD( err, PauseL() );
            if ( err )
                {
                MSGMEDIACONTROLLOGGER_WRITE( "MsgVideoControl : FocusChanged - Error in pausing -> Stop it **" );
                Stop();
                }
            }
        else
            {
            MSGMEDIACONTROLLOGGER_WRITE( "MsgVideoControl : FocusChanged - Stopping video **" );
            Stop();
            }
        }
        
    CMsgMediaControl::FocusChanged( aDrawNow );
    }

// ---------------------------------------------------------
// CMsgVideoControl::SetAndGetSizeL
//
// Calculates and sets the size of the control and returns new size as
// reference aSize. Video control uses the whole application area excluding
// area that has been reserved for scrollbar when it is used for playback.
// Otherwise it uses the same width but height is calculated so that
// it will icon height + one line height and rounded up to the next whole line.
// ---------------------------------------------------------
//
void CMsgVideoControl::SetAndGetSizeL( TSize& aSize )
    {
    MSGMEDIACONTROLLOGGER_WRITEF( _L("MsgVideoControl:: SetAndGetSizeL(): Size: %dx%d"),  aSize.iWidth, aSize.iHeight);
    
    aSize.iWidth = iMaxVideoArea.Width();
    
    if ( iFlags & ENoPlayback )
        {
        aSize.iHeight = iFrame->FrameSize().iHeight;
        
        if ( aSize.iHeight % iLineHeight != 0 )
            {
            aSize.iHeight += iLineHeight - ( aSize.iHeight % iLineHeight );
            }
            
        aSize.iHeight += iLineHeight;
        }
    else
        {
        aSize.iHeight = iMaxVideoArea.Height();
        }
    
    MSGMEDIACONTROLLOGGER_WRITEF( _L("MsgVideoControl:: SetAndGetSizeL(): New size: %dx%d"),  aSize.iWidth, aSize.iHeight);
    SetSize( aSize );
    }

// ---------------------------------------------------------
// CMsgVideoControl::Reset
//
// Empties the control and sets icon visible.
// ---------------------------------------------------------
//
void CMsgVideoControl::Reset()
    {
    if( iVideoPlayer )
        {
        iVideoPlayer->Stop();
        iVideoPlayer->Close();
        
        iTimer->Cancel();
        
        delete iVideoPlayer;
        iVideoPlayer = NULL;
        }
        
    iVideoFile.Close();
    
    iVolume = 0;
    iMaxVolume = 1;

    SetIconVisible( ETrue );
    
    if ( iVideoAreaControl )
    	{
        iVideoAreaControl->MakeVisible( EFalse );
    	}
    
    iControlModeFlags |= EMsgControlModeModified;
    SetState( EMsgAsyncControlStateIdle );
    }

// ---------------------------------------------------------
// CMsgVideoControl::Draw
// Draw the skin background when appriate. Normal icon drawing
// is performed when icon is visible. Otherwise when video frame
// is visible we just draw the control area around the video frame.
// Video controller is responsible of drawing the video frame area.
// ---------------------------------------------------------
//
void CMsgVideoControl::Draw( const TRect& aRect ) const
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgVideoControl : Draw()" );
    if ( VisiblePlaceholder() )
        {    
        // Basic icon drawing from base class when icon is visible.
        CMsgMediaControl::Draw( aRect );    
        }
    else 
        {
        if ( !AknsDrawUtils::BackgroundBetweenRects( AknsUtils::SkinInstance(), 
                                                      AknsDrawUtils::ControlContext( this ), 
                                                      this, 
                                                      SystemGc(), 
                                                      aRect,
                                                      iVideoFrameArea ) )
            {
            SystemGc().SetBrushColor( AKN_LAF_COLOR( 0 ) );
            SystemGc().SetBrushStyle( CGraphicsContext::ESolidBrush );
            SystemGc().SetPenStyle( CGraphicsContext::ENullPen );
            SystemGc().DrawRect( aRect );
            }
            
        if ( !Window().IsFaded() )
            {
            // Should prevent race conditions between DSP ans window server.
            iCoeEnv->WsSession().Flush();
            TRAP_IGNORE( iVideoPlayer->RefreshFrameL() ) ;
            }
        }        
    }

// ---------------------------------------------------------
// CMsgVideoControl::HandleResourceChange
// ---------------------------------------------------------
//
void CMsgVideoControl::HandleResourceChange( TInt aType )
    {
    CMsgMediaControl::HandleResourceChange( aType ); 
    
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        Stop();
        TRAP_IGNORE( UpdateScaleFactorL() );
        
        iMaxVideoArea = MsgEditorCommons::MsgMainPane();
        iMaxVideoArea.SetWidth( MsgEditorCommons::MsgDataPane().Width() );
        }
    }

// ---------------------------------------------------------
// CMsgVideoControl::SizeChanged
//
// Called when size is changed.
// ---------------------------------------------------------
//
void CMsgVideoControl::SizeChanged()
    {
    MSGMEDIACONTROLLOGGER_WRITEF( _L("MsgVideoControl:: SizeChanged(): New size: %dx%d"),  Size().iWidth, Size().iHeight);
    
    CMsgMediaControl::SizeChanged();
    
    SetPosition( Rect().iTl );
    MSGMEDIACONTROLLOGGER_WRITEF( _L("MsgVideoControl:: SizeChanged(): Rect().iTl %dx%d"),  Rect().iTl.iX, Rect().iTl.iY);
    if ( iVideoPlayer && Error() == KErrNone )
        { 
        TRAP_IGNORE( SetVideoFrameSizeL() );
        }
    }

// ---------------------------------------------------------
// CMsgVideoControl::LoadL
// ---------------------------------------------------------
//
void CMsgVideoControl::LoadL( RFile& aFileHandle )
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgVideoControl : LoadL()" );

    iVideoFile.Close();
    User::LeaveIfError( iVideoFile.Duplicate( aFileHandle ) );
    
    if ( !iVideoAreaControl )
    	{
        iVideoAreaControl = CMsgVideoAreaControl::NewL( this );
    	}
    
    if ( !iVideoPlayer )
        {
        // Disable frame display as video is not yet visible
        TRect rect;
        TRect clipRect;

        // Create VideoPlayerUtility instance
        iVideoPlayer = CVideoPlayerUtility::NewL(
                        *this,
                        iPriority,
                        iPreference,
                        ControlEnv()->WsSession(),
                        *ControlEnv()->ScreenDevice(),    
                        *iVideoAreaControl->DrawableWindow(),
                        rect, 
                        clipRect );
        // Register for Loading/rebuffering notices
        iVideoPlayer->RegisterForVideoLoadingNotification( *this );
        }
    else
        {
        iVideoPlayer->Close();
        }

    TMMFileHandleSource mmSource( iVideoFile, iUniqueContentId );
    iVideoPlayer->OpenFileL( mmSource );        

    SetState( EMsgAsyncControlStateOpening );
    
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgVideoControl : LoadL()" );
    }

// ---------------------------------------------------------
// CMsgVideoControl::Cancel
// ---------------------------------------------------------
//
void CMsgVideoControl::Cancel()
    {
    if ( iState == EMsgAsyncControlStateOpening )
        {
        iVideoPlayer->Close();
        
        SetState( EMsgAsyncControlStateIdle );
        }
    }

// ---------------------------------------------------------
// CMsgVideoControl::VideoResolution
//
// Gives the resolution of the video in display (Symbian OS bitmap)
// ---------------------------------------------------------
//
EXPORT_C TSize CMsgVideoControl::VideoResolutionL() const
    {
    TSize retVal( 0, 0 );
    if ( iVideoPlayer && Error() == KErrNone )
        {
        iVideoPlayer->VideoFrameSizeL( retVal );
        }
    return retVal;
    }

// ---------------------------------------------------------
// DelayedExit
// ---------------------------------------------------------
//
TInt CMsgVideoControl::DelayedPlay( TAny* aThis )
    {
    TRAPD( error, static_cast<CMsgVideoControl*>( aThis )->PlayL() );
    return error;
    }

// ---------------------------------------------------------
// CMsgVideoControl::PlayL
//
// Starts video playback.
// ---------------------------------------------------------
//
void CMsgVideoControl::PlayL()
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgVideoControl : PlayL()" );

    // When DSP is used for audio/video playing, dsp resources are not always available.
    // It is handled by setting normal 'ready' icon. No error notes occur.
    if ( iVideoPlayer && 
         (  Error() == KErrNone ||
            (   iState == EMsgAsyncControlStateCorrupt
            &&  (   Error() == KErrAccessDenied
                ||  Error() == KErrSessionClosed ) ) ) && 
         !( iFlags & ENoPlayback ) )
        {
        if ( iHasVideo )
            {
            // Only hide icon if video file has video stream.
            if ( iIconControl->IsVisible() )
                {
                UpdateScaleFactorL();
                
                SetIconVisible( EFalse );
                iVideoAreaControl->MakeVisible( ETrue );
                
                // Enable frame display        	
                SetVideoFrameSizeL();
                }
            
            iTimer->Cancel(); //just in case
            
            // Disable backlight turn off during video playback when video contains
            // video stream.
            iTimer->Start( KResetInactivityAfterOneSecond,
                           KResetInactivityAfterOneSecond,
                           TCallBack( DoResetInactivityTimer, NULL ) );
            }
        	
        iVideoPlayer->Play();
        
        if ( iState == EMsgAsyncControlStatePaused )
            {
            SetState( EMsgAsyncControlStatePlaying );
            }
        else
            {
            if (iState == EMsgAsyncControlStateReady )
                {
                // First time
                SetState( EMsgAsyncControlStateAboutToPlay );
                }
            else
                {
                // State must be stop. Video has earlier played
                SetState( EMsgAsyncControlStatePlaying );
                }
            }
        }
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgVideoControl : PlayL()" );
    }

// ---------------------------------------------------------
// CMsgVideoControl::Stop
//
// Stops video.
// ---------------------------------------------------------
//
void CMsgVideoControl::Stop()
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgVideoControl : Stop()" );

    if ( iState == EMsgAsyncControlStatePlaying ||
         iState == EMsgAsyncControlStatePaused )
        {
        iVideoPlayer->Stop();
        
        // Enable backlight to turn on after video playback has been stopped.
        iTimer->Cancel();
        
        SetState( EMsgAsyncControlStateStopped );
        }
    }

// ---------------------------------------------------------
// CMsgVideoControl::PauseL
//
// Pauses video.
// ---------------------------------------------------------
//
void CMsgVideoControl::PauseL()
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgVideoControl : PauseL()" );
    if ( iState == EMsgAsyncControlStatePlaying )
        {
        iVideoPlayer->PauseL();
        
        // Enable backlight to turn on after video playback has been paused.
        iTimer->Cancel();
        
        SetState( EMsgAsyncControlStatePaused );
        }
    }

// ---------------------------------------------------------
// CMsgVideoControl::IsOffScreen
//
// Returns ETrue if control is off screen.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgVideoControl::IsOffScreen() const
    {
    TRect rect( Rect() );
    if ( rect.iTl.iY < 0 || rect.iBr.iY > iMaxVideoArea.Height() )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }


// ---------------------------------------------------------
// CMsgVideoControl::SetVideoFrameSizeL
//
// ---------------------------------------------------------
//
void CMsgVideoControl::SetVideoFrameSizeL()
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgVideoControl : DoSizeChangedL()" );
    
    TRect rect;
    TRect clipRect;
    
	if ( VisiblePlaceholder() )
		{
		// Disable frame display if icon is visible
		iVideoFrameArea = TRect();
		}
	else
	    {
	    TSize videoSize( iOriginalSize.iWidth * iScaleFactor,
                         iOriginalSize.iHeight * iScaleFactor );
        
        TPoint videoPosition( Rect().Center() );
        
        videoPosition.iX -= videoSize.iWidth / 2;
        videoPosition.iY -= videoSize.iHeight / 2;
        
        // Rect & clipRect are relative to screen coordinates.
        rect =  TRect( videoPosition, videoSize );
        rect.Move( 0, iMaxVideoArea.iTl.iY );

        clipRect = rect;
        clipRect.iTl.iY = Max( clipRect.iTl.iY, iMaxVideoArea.iTl.iY );
        clipRect.iTl.iY = Min( clipRect.iTl.iY, iMaxVideoArea.iBr.iY );
        clipRect.iBr.iY = Min( clipRect.iBr.iY, iMaxVideoArea.iBr.iY );
        clipRect.iBr.iY = Max( clipRect.iBr.iY, iMaxVideoArea.iTl.iY );

        // iVideoFrameArea is relative to window coordinates.
        iVideoFrameArea = clipRect;
        iVideoFrameArea.Move( 0, -iBaseControlObserver->ViewRect().iTl.iY );

        MSGMEDIACONTROLLOGGER_WRITE( "MsgVideoControl : DoSizeChangedL() - Calling  SetDisplayWindowL" );
        MSGMEDIACONTROLLOGGER_WRITEF( _L("MsgVideoControl:: SizeChanged(): rect (%d,%d),( %d,%d)"),  rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY);
        MSGMEDIACONTROLLOGGER_WRITEF( _L("MsgVideoControl:: SizeChanged(): cliRect (%d,%d),( %d,%d)"),  clipRect.iTl.iX, clipRect.iTl.iY, clipRect.iBr.iX, clipRect.iBr.iY);
	    }
	
    iVideoAreaControl->SetRect( iVideoFrameArea );

	// iVideoAreaControl is relative to screen coordinates.
    iVideoPlayer->SetDisplayWindowL(
        ControlEnv()->WsSession(),
        *ControlEnv()->ScreenDevice(),
        *iVideoAreaControl->DrawableWindow(),
        rect,
        clipRect );
    
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgVideoControl : DoSizeChangedL()" );
    }


// =========================================================
// Callbacks from VideoPlayerutility
// =========================================================

// ---------------------------------------------------------
// see MVideoPlayerUtilityObserver::MvpuoOpenComplete
// ---------------------------------------------------------
//
void CMsgVideoControl::MvpuoOpenComplete( TInt aError )
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgVideoControl : MvpuoOpenComplete()" );
    
    // Close the handle now
    iVideoFile.Close();
    
    iMaxVolume = iVideoPlayer->MaxVolume();
    iVolume = iVideoPlayer->Volume();

    // Needed so that video controller won't consume DRM rights automatically.    
    MMMFDRMCustomCommand* customCommand = iVideoPlayer->GetDRMCustomCommand();
    if ( customCommand )
        {
        customCommand->DisableAutomaticIntent( ETrue );            
        }
    
    HandleCallback( aError, iState );
    // Remember that MvpuoPrepareComplete() follows
    
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgVideoControl : MvpuoOpenComplete()" );
    }

// ---------------------------------------------------------
// see MVideoPlayerUtilityObserver::MvpuoPrepareComplete
// ---------------------------------------------------------
//
void CMsgVideoControl::MvpuoPrepareComplete( TInt aError )
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgVideoControl : MvpuoPrepareComplete()" );
    
    // Determine whether video file contains video stream (i.e. it has something
    // to show on the screen).
	TRAP_IGNORE( 
	    {
        if ( iVideoPlayer->VideoFrameRateL() == 0 && iVideoPlayer->VideoBitRateL() == 0 )
            {
            MSGMEDIACONTROLLOGGER_WRITE( "MsgVideoControl : no video stream" );
            
            // If both video frame rate and bit rate are zeros then we can be pretty
            // sure that there is no video stream on the file.
            iHasVideo = EFalse;
            }
                                                 
        iVideoPlayer->VideoFrameSizeL( iOriginalSize );
        } );
    
    HandleCallback( aError, EMsgAsyncControlStateReady );    
    
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgVideoControl : MvpuoPrepareComplete()" );
    }

// ---------------------------------------------------------
// see MVideoPlayerUtilityObserver::MvpuoFrameReady
// ---------------------------------------------------------
//
void CMsgVideoControl::MvpuoFrameReady( CFbsBitmap& /*aFrame*/,TInt /*aError*/ )
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgVideoControl : MvpuoFrameReady()" );
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgVideoControl : MvpuoFrameReady()" );
    }

// ---------------------------------------------------------
// see MVideoPlayerUtilityObserver::MvpuoPlayComplete
// Trapping callback as leaving not allowed.
// ---------------------------------------------------------
//
void CMsgVideoControl::MvpuoPlayComplete( TInt aError )
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgVideoControl : MvpuoPlayComplete()" );
    
    // Enable backlight to turn on after video playback has ended.
    iTimer->Cancel();
    
    HandleCallback( aError, EMsgAsyncControlStateStopped );
    
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgVideoControl : MvpuoPlayComplete()" );
    }
        
// ---------------------------------------------------------
// see MVideoPlayerUtilityObserver::MvpuoEvent
// Trapping callback as leaving not allowed.
// ---------------------------------------------------------
//
void CMsgVideoControl::MvpuoEvent( const TMMFEvent& /*aEvent*/ )
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgVideoControl : MvpuoEvent()" );
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgVideoControl : MvpuoEvent()" );
    }
    
// ---------------------------------------------------------
// see MVideoLoadingObserver::MvloLoadingStarted
// Trapping callback as leaving not allowed.
// ---------------------------------------------------------
//
void CMsgVideoControl::MvloLoadingStarted()
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgVideoControl : MvloLoadingStarted()" );

    SetState( EMsgAsyncControlStateBuffering );
    
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgVideoControl : MvloLoadingStarted()" );
    }
    
// ---------------------------------------------------------
// see MVideoLoadingObserver::MvloLoadingComplete
// Trapping callback as leaving not allowed.
// ---------------------------------------------------------
//
void CMsgVideoControl::MvloLoadingComplete()
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgVideoControl : MvloLoadingComplete()" );
    
    SetState( EMsgAsyncControlStatePlaying );
    
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgVideoControl : MvloLoadingComplete()" );
    }

// ---------------------------------------------------------
// CMsgVideoControl::HandleCallback
// ---------------------------------------------------------
//
void CMsgVideoControl::HandleCallback( TInt aError,
                                       TMsgAsyncControlState aNewState )
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgVideoControl : HandleCallback()" );
    MSGMEDIACONTROLLOGGER_WRITEF_ERROR_STATE( _L("CMsgVideoControl::HandleCallback: **aError: %d, CurrentState: %S"), aError, iState);
    MSGMEDIACONTROLLOGGER_WRITEF_ERROR_STATE( _L("CMsgVideoControl::HandleCallback: **aError: %d, aNewState: %S"), aError, aNewState);
    
    if ( aError == KErrMMPartialPlayback )
        {
        aError = KErrNone;
        }
    
    iError = aError;
    switch ( aError )
        {
        case KErrNone:    
            {
            SetState( aNewState );
            
            if ( iState == EMsgAsyncControlStateOpening )
                {
                if ( iVideoPlayer )
                    {
                    iVideoPlayer->Prepare();
                    }
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
        case KErrNoMemory:
        default:
            {
            SetState( EMsgAsyncControlStateCorrupt );
            }
        } 
    
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgVideoControl : HandleCallback()" );   
    }

// ---------------------------------------------------------
// CMsgVideoControl::Reset
//
// Empties the control.
// ---------------------------------------------------------
//
void CMsgVideoControl::Close()
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgVideoControl : Close()" );
    
    Reset();
    
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgVideoControl : Close()" );   
    }


// ---------------------------------------------------------
// CMsgImageControl::HitRegionContains
// 
// Note! CONE will do final verification if control is really hit so
//       not need to do it here.
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
TBool CMsgVideoControl::HitRegionContains( const TPoint& aPoint, 
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
        result = iVideoFrameArea.Contains( aPoint );;
        }
        
    return result;
    }
#else
TBool CMsgVideoControl::HitRegionContains( const TPoint& /*aPoint*/, 
                                           const CCoeControl& /*aControl*/ ) const
    {
    return EFalse;
    }

#endif // RD_SCALABLE_UI_V2

// ---------------------------------------------------------
// CMsgVideoControl::HandleResourceChange
//
// Resets inactivity timer and prevents backlight from turning
// off during video playback.
// ---------------------------------------------------------
//
TInt CMsgVideoControl::DoResetInactivityTimer( TAny* /*aObject*/ )
    {
    User::ResetInactivityTime();
    return KErrNone;
    }

// ---------------------------------------------------------
// CMsgVideoControl::UpdateScaleFactorL
// ---------------------------------------------------------
//
void CMsgVideoControl::UpdateScaleFactorL()
    {
    if ( iVideoPlayer )
        {
        TReal widthRatio = static_cast<TReal>( iSize.iWidth ) / 
                           static_cast<TReal>( iOriginalSize.iWidth );
        TReal heightRatio = static_cast<TReal>( iSize.iHeight ) / 
                            static_cast<TReal>( iOriginalSize.iHeight );
                        
        iScaleFactor = widthRatio < heightRatio ? widthRatio : 
                                                  heightRatio;
        }
    }

//  End of File
