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



// INCLUDE FILES
#include "SmilVideoRenderer.h"

#include <AknUtils.h>
#include <aknlayout.cdl.h>
#include <applayout.cdl.h> // LAF
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknStatuspaneUtils.h>
#include <mmf/common/mmcaf.h>
#include <mmferrors.h>
#include <AudioPreference.h>

#include <smilpresentation.h>

#ifdef VIDEO_DEBUG
    #include "SmilMediaLogging.h"
#endif

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TInt KProcessingDelayOffset = 1 * 1000 * 1000; // 1s

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::CSmilVideoRenderer
// ----------------------------------------------------------------------------
//
CSmilVideoRenderer::CSmilVideoRenderer( MSmilMedia* aMedia,
                                        DRMCommon& aDrmCommon,
                                        CDRMHelper& aDrmHelper ) :
    CSmilMediaRendererBase( EMsgMediaVideo, aMedia, aDrmCommon, aDrmHelper ),
    iState( ENotReady ),
    iError( KErrNone ),
    iStartTime( TInt64( 0 ) ),
    iParent( aMedia->Presentation()->GetPlayer()->GetControl( aMedia->Presentation() ) )
    {
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::ConstructL
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::ConstructL( RFile& aFileHandle )
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video, ConstructL" );
#endif
    
    BaseConstructL( aFileHandle );
    User::LeaveIfError( CheckDRMRights() );
    
    User::LeaveIfError( iMediaFile.Duplicate( aFileHandle ) );
    
    CreateWindowL( iParent );
    Window().SetBackgroundColor( KRgbBlack );
    Window().SetNonFading( ETrue );
    
    SetRect( ControlRect() );

#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, control rect: ( %d, %d) ( %d, %d)"),
        Rect().iTl.iX, Rect().iTl.iY, Rect().iBr.iX, Rect().iBr.iY );
    
    TPoint winPos = Window().Position();
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, window position: ( %d, %d)"),
        winPos.iX, winPos.iY );
#endif
    
    //Lets set the video mode according to the platform
    LoadVideoL();
    
    TInt volume( iMedia->Presentation()->Volume() );
    TRAP_IGNORE( DoSetVolumeL( volume ) );

    MakeVisible( EFalse );
    ActivateL();

#ifdef VIDEO_DEBUG
    SMILUILOGGER_LEAVEFN( "Video: ConstructL" );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::NewL
// ----------------------------------------------------------------------------
//
CSmilVideoRenderer* CSmilVideoRenderer::NewL( RFile& aFileHandle,
                                              MSmilMedia* aMedia,
                                              DRMCommon& aDrmCommon,
                                              CDRMHelper& aDrmHelper )
    {
    CSmilVideoRenderer* renderer = new(ELeave) CSmilVideoRenderer( aMedia,
                                                                   aDrmCommon,
                                                                   aDrmHelper );
    CleanupStack::PushL( renderer );
    renderer->ConstructL( aFileHandle );
    CleanupStack::Pop( renderer );
    return renderer;
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::~CSmilVideoRenderer
// ----------------------------------------------------------------------------
//
CSmilVideoRenderer::~CSmilVideoRenderer()
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video, ~CSmilVideoRenderer" );
#endif
    
    if ( iVideoPlayer )
        {
#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITE( "Video: iVideoPlayer->Close()" );
#endif
        iVideoPlayer->Close();
        delete iVideoPlayer;
        }
    
    iMediaFile.Close();
    iMedia = NULL; // For LINT

#ifdef VIDEO_DEBUG
    SMILUILOGGER_LEAVEFN( "Video: ~CSmilVideoRenderer" );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::LoadVideoL
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::LoadVideoL()
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: LoadVideoL" );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
#endif

    // Clipping rect and rect are always relative to screen coordinates!!
    TRect rect( VideoRect() );    
    TRect clipRect( rect );
    
#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("Video: LoadVideo rect before adjusting: (%d, %d) (%d, %d)"),
        rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY );
#endif

    AdjustRects( rect, clipRect );
    
    if ( !iVideoPlayer )
        {
#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITEF( _L("SMILUI: Video, rect: ( %d, %d) ( %d, %d), clipRect: ( %d, %d) ( %d, %d)"),
            rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY,
            clipRect.iTl.iX, clipRect.iTl.iY, clipRect.iBr.iX, clipRect.iBr.iY );
#endif
        
        // Create VideoPlayerUtility instance
        iVideoPlayer = CVideoPlayerUtility::NewL( *this,
                                                  TMdaPriority( KAudioPrioritySmilPlayer ),
                                                  TMdaPriorityPreference( KAudioPrefSmilPlayer ),
                                                  ControlEnv()->WsSession(),
                                                  *ControlEnv()->ScreenDevice(),
                                                  Window(),
                                                  rect,
                                                  clipRect );
        
        iCurrentClipRect = clipRect;
        
        // Register for Loading/rebuffering notices
        iVideoPlayer->RegisterForVideoLoadingNotification( *this );
        }
    else
        {
#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITE( "Video: iVideoPlayer->Close()" );
#endif
        iVideoPlayer->Close();
        
        if ( clipRect != iCurrentClipRect )
            {
            iVideoPlayer->SetDisplayWindowL( ControlEnv()->WsSession(),
                                             *ControlEnv()->ScreenDevice(),
                                             Window(),
                                             rect,
                                             clipRect );
            iCurrentClipRect = clipRect;
            }
        }
    
#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITE( "Video: iVideoPlayer->OpenFileL()" );
#endif
 
    TMMFileHandleSource fileHandleSource( iMediaFile, KDefaultContentObject, ContentAccess::EPeek );
    iVideoPlayer->OpenFileL( fileHandleSource );

    BeginActiveWait();
    
    MMMFDRMCustomCommand* customCommand = iVideoPlayer->GetDRMCustomCommand();
    if ( customCommand )
        {
        customCommand->DisableAutomaticIntent( ETrue );            
        }

#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, iError = %d"), iError );
#endif
    
    if ( iError != KErrNone )
        {
        User::Leave( iError );
        }

#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
    SMILUILOGGER_LEAVEFN( "Video: LoadVideoL" );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::DoSetVolumeL
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::DoSetVolumeL( TInt aVolume )
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: DoSetVolumeL" );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, aVolume = %d"), aVolume );
#endif

    if ( iVideoPlayer->AudioEnabledL() )
        {
#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITE( "Video: iVideoPlayer->SetVolumeL()" );
#endif
        iVideoPlayer->SetVolumeL( iVideoPlayer->MaxVolume() * aVolume / 100 );
        }

#ifdef VIDEO_DEBUG
    SMILUILOGGER_LEAVEFN( "Video: DoSetVolumeL" );
    SMILUILOGGER_WRITEF( _L(""));
#endif

    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::IsVisual
// ----------------------------------------------------------------------------
//
TBool CSmilVideoRenderer::IsVisual() const
    {
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::IsOpaque
// ----------------------------------------------------------------------------
//
TBool CSmilVideoRenderer::IsOpaque() const
    {
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::IntrinsicWidth
// ----------------------------------------------------------------------------
//
TInt CSmilVideoRenderer::IntrinsicWidth() const
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, IntrinsicWidth = %d"),
                         iMedia->GetRectangle().Width() );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    return iMedia->GetRectangle().Width();
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::IntrinsicHeight
// ----------------------------------------------------------------------------
//
TInt CSmilVideoRenderer::IntrinsicHeight() const
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, IntrinsicHeight = %d"),
                         iMedia->GetRectangle().Height() );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    return iMedia->GetRectangle().Height();
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::IntrinsicDuration
// ----------------------------------------------------------------------------
//
TSmilTime CSmilVideoRenderer::IntrinsicDuration() const
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: IntrinsicDuration" );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
#endif

    TTimeIntervalMicroSeconds duration( TInt64( 0 ) );
    
    TRAPD( error, duration = iVideoPlayer->DurationL() );
    if ( error == KErrNone )
        {
        duration = duration.Int64() + iProcessingDelay.Int64() + KProcessingDelayOffset;
        }

#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, IntrinsicDuration = %d"), I64INT( duration.Int64() ) );
    SMILUILOGGER_LEAVEFN( "Video: IntrinsicDuration" );
    SMILUILOGGER_WRITEF( _L(""));
#endif

    return TSmilTime::FromMicroSeconds( duration );
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::PrepareMediaL
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::PrepareMediaL()
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: PrepareMediaL" );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
#endif
    
    if ( iState == EReady )
        {
        // Parent control position might have changed so we need to
        // update our position & size.
        SetRect( ControlRect() );
        }
    
    // Reset processing delay.    
    iProcessingDelay = TInt64( 0 );
    
    // Reset pause position.
    iPausePosition = TInt64( 0 );
    
    if ( iLatePreparation )
        {
#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITEF( _L("SMILUI: Using late preparation") );
#endif
        DoLatePreparationL();
        
        if ( iError != KErrNone )
            {            
            iState = EError;
            }
        }
    
    if ( iState == EReady )
        {
        iState = EHidden;
        } 
       
#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
    SMILUILOGGER_LEAVEFN( "Video: PrepareMediaL" );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::SeekMediaL
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::SeekMediaL( const TSmilTime& aTime )
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: SeekMediaL" );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video: SeekMediaL, time=%d"), aTime.Value() );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
#endif
    
    TTimeIntervalMicroSeconds position = iVideoPlayer->PositionL();
    if ( position != aTime.ToMicroSeconds() )
        {
        if ( iState == EPlaying )
            {
#ifdef VIDEO_DEBUG
            SMILUILOGGER_WRITEF( _L("Video: iVideoPlayer->Stop()") );
#endif
            iVideoPlayer->Stop();
            
#ifdef VIDEO_DEBUG
            SMILUILOGGER_WRITEF( _L("Video: iVideoPlayer->SetPositionL()" ));
#endif
            iVideoPlayer->SetPositionL( aTime.ToMicroSeconds() );
            
#ifdef VIDEO_DEBUG
            SMILUILOGGER_WRITEF( _L("Video: iVideoPlayer->Play()") );
#endif
            iVideoPlayer->Play();
            }
        else if( iState == EHidden )
            {
#ifdef VIDEO_DEBUG
            SMILUILOGGER_WRITEF( _L("Video: iVideoPlayer->SetPositionL()") );
#endif
            
            iVideoPlayer->SetPositionL( aTime.ToMicroSeconds() );
            }
        }

    if( aTime == 0 &&  
        iState == EPaused &&
        PresentationPlaying() )
        {
#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITE( "SMILUI: Video, Auto play on SeekMediaL" );
#endif
        iState = EHidden;
        ShowMediaL();
        }

#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
    SMILUILOGGER_LEAVEFN( "Video: SeekMediaL" );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::ShowMediaL
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::ShowMediaL()
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: ShowMediaL" );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
#endif

    if ( iState == EHidden )
        {
        iState = ELoading;
        UpdateDrawingArea();
        MakeVisible( ETrue );

#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITEF( _L("Video: iCurrentClipRect: (%d, %d) (%d, %d)"),
            iCurrentClipRect.iTl.iX, iCurrentClipRect.iTl.iY, iCurrentClipRect.iBr.iX, iCurrentClipRect.iBr.iY );
        SMILUILOGGER_WRITE( "Video: iVideoPlayer->Play()" );
#endif
        iCoeEnv->WsSession().Flush();
        
        iStartTime.HomeTime();
        
        if ( iMedia->Presentation()->State() != CSmilPresentation::EPlaying )
			{
			iOldVolume = iVideoPlayer->Volume();	
			SetVolume( 0 );
			}       

        iVideoPlayer->Play();
        
        // To make sure we don't Pause before clip has been loaded.
        BeginActiveWait();
        
        if ( iState == ELoading )
            {
            iState = EPlaying;
            
            TTime stopTime( TInt64( 0 ) );
            stopTime.HomeTime();
    
            iProcessingDelay = iProcessingDelay.Int64() + stopTime.MicroSecondsFrom( iStartTime ).Int64();
            
            ConsumeDRMRightsL();
            }
        }

#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
    SMILUILOGGER_LEAVEFN( "Video: ShowMediaL" );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::HideMedia
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::HideMedia()
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: HideMedia" );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
#endif

    if ( iState == EPlaying || iState == EPaused )
        {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("Video: iVideoPlayer->Stop()") );
#endif
        iVideoPlayer->Stop();
        iState = EHidden;
        
        // Do not call MakeVisible if presentation has ended and
        // video is played from begin of the presentation until the
        // end of presentation and current time is within one second
        // from the end.
        // This optimization is to keep the last frame visible.
        if ( !PresentationPlaying() ||
             !PlayedForWholePresentation() ||
             ( iMedia->Presentation()->CurrentTime() < 
               ( iMedia->MediaEnd() - TSmilTime::FromMicroSeconds( KProcessingDelayOffset ) ) ) )
            {
            MakeVisible( EFalse );
            UpdateDrawingArea();
            }
        }
        
#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
    SMILUILOGGER_LEAVEFN( "Video: HideMedia" );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::FreezeMedia
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::FreezeMedia()
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: FreezeMedia" );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
#endif

    if ( iState == EPlaying )
        {
#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITE( "Video: iVideoPlayer->PauseL()" );
#endif
        // Do the waiting only if presentation is ending. Otherwise performs "normal"
        // pausing
        if ( PresentationPlaying() &&
             iMedia->Presentation()->CurrentTime() >= iMedia->Presentation()->Duration() )
            {
            // Clip is not paused if
            // clip end is < 1 second. In this case we wait
            // until clip ends.
            TBool clipEnding( EFalse );
            
            TRAP_IGNORE( clipEnding = IsClipEndingL() );
            
            if ( iState != EPaused )
                {
                if ( clipEnding )
                    {
                    BeginActiveWait();
                    }
                else
                    {
                    DoPause();
                    }
                }
            }
        else
            {
            DoPause();
            }
        }
#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
    SMILUILOGGER_LEAVEFN( "Video: FreezeMedia" );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::Draw
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::Draw( CGraphicsContext& /*aGc*/,
                              const TRect& /*aRect*/,
                              CSmilTransitionFilter* /*aTransitionFilter*/,
                              const MSmilFocus* /*aFocus*/ )
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: Draw" );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
    SMILUILOGGER_LEAVEFN( "Video: Draw" );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::Draw
// ----------------------------------------------------------------------------
void CSmilVideoRenderer::Draw( const TRect& aRect ) const
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: Draw" );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
#endif

    if ( iError != KErrNone ||
         ( iState != EPlaying && 
           iState != EPaused &&
           iState != ELoading ) )
        {
#ifdef VIDEO_DEBUG
            SMILUILOGGER_ENTERFN( "Video: Drawn with black" );
#endif
            
        CWindowGc& gc = SystemGc();
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc.SetPenStyle( CGraphicsContext::ENullPen );
        gc.SetBrushColor( KRgbBlack );
        gc.DrawRect( aRect ); 
        }    
    else if ( Window().DisplayMode() == EColor16MA )
		{
		CWindowGc& gc = SystemGc();
		gc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
        gc.SetBrushColor( TRgb( 0, 0 ) );
		gc.Clear();  		
		}
    
#ifdef VIDEO_DEBUG
    SMILUILOGGER_LEAVEFN( "Video: Draw" );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::ResumeMedia
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::ResumeMedia()
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: ResumeMedia" );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
#endif
    
    TRAPD( error, DoResumeL() );
    if ( error != KErrNone )    
        {
        iError = error;
        }    
        
#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d, iError = %d"), iState, iError );
    SMILUILOGGER_LEAVEFN( "Video: ResumeMedia" );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::DoResumeL
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::DoResumeL()
    {
    if ( iState == EPaused )
        {
#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITE( "Video: iVideoPlayer->Play() in ResumeMedia()" );
#endif
        if ( iLatePreparation )
            {
#ifdef VIDEO_DEBUG
            SMILUILOGGER_WRITE( "Video: late preparation in ResumeMedia()" );
#endif

            DoLatePreparationL();
            
            if ( iError == KErrNone )
                {
#ifdef VIDEO_DEBUG
                SMILUILOGGER_WRITE( "Video: late preparation succeeded" );
#endif
            
                iState = EPlaying;
                
                UpdateDrawingArea();
                iVideoPlayer->Play( iPausePosition, iVideoPlayer->DurationL() );
                
                iPausePosition = 0;
                }
            }
        else
            {
            iState = EPlaying;
            iPausePosition =  0;
            
            UpdateDrawingArea();
            iVideoPlayer->Play();            
            }    
            
        }
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::SetVolume
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::SetVolume( TInt aVolume )
    {
    TRAP_IGNORE( DoSetVolumeL( aVolume ) );
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::MvpuoFrameReady
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::MvpuoFrameReady( CFbsBitmap& /*aFrame*/,
                                         TInt /*aError*/ )
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("Frame ready") );
#endif
    return;
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::MvpuoPlayComplete
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::MvpuoPlayComplete( TInt aError )
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: PlayComplete" );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, aError = %d"), aError );
#endif
    
    if ( aError != KErrNone )
        {
        iError = aError;
        iLatePreparation = ETrue;
            
        if ( iState != ENotReady &&
             iState != EReady &&
             iState != EHidden )
            {
            if ( iState == EPaused &&
                 ( iError == KErrSessionClosed ||
                   iError == KErrDied ) )
                {
#ifdef VIDEO_DEBUG
                SMILUILOGGER_WRITEF( _L("SMILUI: Video, pause position: %d"), I64INT( iPausePosition.Int64() ) );
#endif
                if ( iPausePosition == 0 )
                    {
                    // No pause position. Cannot show continue on menu. Notifying UI that presentation
                    // should go to end state.
                    iState = EError;
#ifdef VIDEO_DEBUG
                    SMILUILOGGER_WRITE( "SMILUI: Video, Critical error. Notified UI about presentation end" );
#endif
                    iMedia->Presentation()->GetPlayer()->PresentationEvent( MSmilPlayer::EEndReached, 
                                                                            iMedia->Presentation(), 
                                                                            KNullDesC() );
                    }
                // Otherwise try to do late preparation on ResumeMedia and continue from the current position.
                }
            else 
                {
                iState = EError;
                
                TRAP_IGNORE( iMedia->RendererAtEndL() );
                }
            }
        
        EndActiveWait();
        }
    else if ( iState == EPlaying )
        {        
        TRAP_IGNORE( iMedia->RendererAtEndL() );
        iState = EPaused;
        
        EndActiveWait();
        }
    
#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
    SMILUILOGGER_LEAVEFN( "Video: PlayComplete" );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::MvpuoOpenComplete
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::MvpuoOpenComplete( TInt aError )
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: OpenComplete" );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, aError = %d"), aError );
#endif
    if ( aError != KErrNone )
        {
        iError = aError;
        
        EndActiveWait();
        }
    else
        {
        // Prepare clip for playing and data queries
#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITE( "Video: iVideoPlayer->Prepare()" );
#endif
        iVideoPlayer->Prepare();
        }
#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
    SMILUILOGGER_LEAVEFN( "Video: OpenComplete" );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::MvpuoPrepareComplete
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::MvpuoPrepareComplete( TInt aError )
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: PrepareComplete" );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, aError = %d"), aError );
#endif

    if ( aError != KErrNone &&
         aError != KErrMMPartialPlayback )
        {
        iError = aError;
        iLatePreparation = ETrue;
        }
    else
        {
        if ( iLatePreparation )
            {
            // Calculate processing delay taken by the late preparation.
            TTime stopTime( TInt64( 0 ) );
            stopTime.HomeTime();
            
            TTimeIntervalMicroSeconds duration( TInt64( 0 ) );
            duration = stopTime.MicroSecondsFrom( iStartTime ).Int64();
            iProcessingDelay = duration.Int64() + KProcessingDelayOffset;
            }
            
        iState = EReady;
        iLatePreparation = EFalse;
        }
    
#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, Processing delay = %d"),
                         I64INT( iProcessingDelay.Int64() ) );
#endif
                         
    EndActiveWait();

#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
    SMILUILOGGER_LEAVEFN( "Video: PrepareComplete" );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::MvpuoEvent
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::MvpuoEvent( const TMMFEvent& /*aEvent*/ )
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: MvpuoEvent" );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
#endif

#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
    SMILUILOGGER_LEAVEFN( "Video: MvpuoEvent" );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::MvloLoadingStarted
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::MvloLoadingStarted()
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: MvloLoadingStarted" );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
#endif

#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
    SMILUILOGGER_LEAVEFN( "Video: MvloLoadingStarted" );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::MvloLoadingComplete
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::MvloLoadingComplete()
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: MvloLoadingComplete" );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
#endif
            
    // To make sure we don't Pause before clip loading completed.
    EndActiveWait();

#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, State = %d"), iState );
    SMILUILOGGER_LEAVEFN( "Video: MvloLoadingComplete" );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::AdjustRects
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::AdjustRects( TRect& aRect, TRect& aClipRect ) const
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, aRect: ( %d, %d) ( %d, %d), aClipRect: ( %d, %d) ( %d, %d)"),
        aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY,
        aClipRect.iTl.iX, aClipRect.iTl.iY, aClipRect.iBr.iX, aClipRect.iBr.iY );
    SMILUILOGGER_ENTERFN( "Video: AdjustRects" );
#endif
    
    TRect appWindowRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EApplicationWindow, appWindowRect );
    
    TAknLayoutRect mainPane;
    
    if ( AknStatuspaneUtils::StaconPaneActive() )
        {
        mainPane.LayoutRect( appWindowRect, AknLayoutScalable_Avkon::main_pane( 4 ) );
        }
    else
        {
        mainPane.LayoutRect( appWindowRect, AknLayoutScalable_Avkon::main_pane( 0 ) );
        }
    
    TAknLayoutRect mainSmilPane;
    mainSmilPane.LayoutRect( mainPane.Rect(), AknLayoutScalable_Apps::main_smil_pane() );
    
    TRect mainSmilPaneRect;    
    mainSmilPaneRect = mainSmilPane.Rect();
    
    TRect controlPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EControlPane, controlPaneRect );
    
    // region to height of screen
    if ( aRect.iBr.iY > mainSmilPaneRect.iBr.iY )
        {                
#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITEF( _L("SMILUI: Video, region to height of screen ") );
#endif
        aClipRect.iBr.iY = mainSmilPaneRect.iBr.iY;
        }

    // region to width of screen
    if ( aRect.iBr.iX > mainSmilPaneRect.iBr.iX )
        {
#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITEF( _L("SMILUI: Video, region to width of screen ") );
#endif
        aClipRect.iBr.iX = mainSmilPaneRect.iBr.iX;
        }

    // softkeys in pause/end state
    if ( iState != ENotReady &&  
         iState != EHidden &&
         iState != ELoading &&
         iState != EPlaying )
        {
        if ( !AknStatuspaneUtils::StaconPaneActive() &&
             aRect.iBr.iY > controlPaneRect.iTl.iY )
            {
#ifdef VIDEO_DEBUG
            SMILUILOGGER_WRITEF( _L("SMILUI: Video, region to above control pane ") );
#endif
            aClipRect.iBr.iY = controlPaneRect.iTl.iY;
            }
        }

    if ( aRect.iTl.iX < 0 )
        {
#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITEF( _L("SMILUI: Video, region iTl horizontally out of screen ") );
#endif
        aRect.iTl.iX = 0;
        }
    
    if ( aRect.iTl.iY < 0 )
        {
#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITEF( _L("SMILUI: Video, region iTl vertically out of screen ") );
#endif
        aRect.iTl.iY = 0;
        }
        
    if ( aClipRect.iTl.iX < 0 )
        {
#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITEF( _L("SMILUI: Video, clip region iTl horizontally out of screen ") );
#endif
        aClipRect.iTl.iX = 0;
        }
    
    if ( aClipRect.iTl.iY < 0 )
        {
#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITEF( _L("SMILUI: Video, clip region iTl vertically out of screen ") );
#endif
        aClipRect.iTl.iY = 0;
        }
    
    // Disable rendering if video region is still hidden.
    if ( iState == ENotReady ||
    	 iState == EReady ||
    	 iState == EHidden )
    	{
    	aRect = TRect();
    	aClipRect = TRect();
    	}
    
#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, aRect: ( %d, %d) ( %d, %d), aClipRect: ( %d, %d) ( %d, %d)"),
        aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY,
        aClipRect.iTl.iX, aClipRect.iTl.iY, aClipRect.iBr.iX, aClipRect.iBr.iY );
    SMILUILOGGER_LEAVEFN( "Video: AdjustRects" );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::UpdateDrawingArea
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::UpdateDrawingArea()
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: UpdateDrawingArea" );
#endif
    
    // Clipping rect and rect are always relative to screen coordinates!!
    TRect rect( VideoRect() );

#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("Video: UpdateDrawingArea rect before moving rect: (%d, %d) (%d, %d)"),
        rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY );
    
    TPoint winPos = Window().Position();
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, window position: ( %d, %d)"),
        winPos.iX, winPos.iY );
#endif
    
    TRect mediaRect( iMedia->GetRectangle() );
    mediaRect.Move( iParent->PositionRelativeToScreen() );    
    
    rect.iBr.iY = mediaRect.iBr.iY;
    
    TRect clipRect( rect );

    AdjustRects( rect, clipRect );

#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, iCurrentClipRect: ( %d, %d) ( %d, %d), clipRect: ( %d, %d) ( %d, %d)"),
        iCurrentClipRect.iTl.iX, iCurrentClipRect.iTl.iY, iCurrentClipRect.iBr.iX, iCurrentClipRect.iBr.iY,
        clipRect.iTl.iX, clipRect.iTl.iY, clipRect.iBr.iX, clipRect.iBr.iY );
#endif

    if ( iCurrentClipRect != clipRect )
        {
        iCurrentClipRect = clipRect;

#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITEF( _L("SMILUI: Video, iCurrentClipRect: ( %d, %d) ( %d, %d), clipRect: ( %d, %d) ( %d, %d)"),
            iCurrentClipRect.iTl.iX, iCurrentClipRect.iTl.iY, iCurrentClipRect.iBr.iX, iCurrentClipRect.iBr.iY,
            clipRect.iTl.iX, clipRect.iTl.iY, clipRect.iBr.iX, clipRect.iBr.iY );
        SMILUILOGGER_WRITE( "Video: iVideoPlayer->SetDisplayWindowL()" );
#endif
        TRAP_IGNORE( iVideoPlayer->SetDisplayWindowL( ControlEnv()->WsSession(),
                                                      *ControlEnv()->ScreenDevice(),
                                                      Window(),
                                                      rect,
                                                      clipRect ) );
        }
        
    UpdateSoftKeyVisibility();
        
#ifdef VIDEO_DEBUG
    SMILUILOGGER_LEAVEFN( "Video: UpdateDrawingArea" );
#endif
    }
    
// ----------------------------------------------------------------------------
// CSmilVideoRenderer::IsClipEndingL
// Determines if clip is ending < 1 second.
// ----------------------------------------------------------------------------
//
TBool CSmilVideoRenderer::IsClipEndingL() const
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: IsClipEndingL" );
#endif

    TBool result( EFalse );
    TTimeIntervalMicroSeconds position( iVideoPlayer->PositionL() );
    TTimeIntervalMicroSeconds duration( iVideoPlayer->DurationL() );

#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, position: %d"), I64INT( position.Int64() ) );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, duration: %d"), I64INT( duration.Int64() ) );
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, iProcessingDelay: %d"), I64INT( iProcessingDelay.Int64() ) );
#endif        
    
    // Sanity check.
    if ( position <= duration )
        {
        position = position.Int64() + iProcessingDelay.Int64() + KProcessingDelayOffset;
    
        if ( duration < position )
            {
            result = ETrue;
            }
        }

#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, result: %d"), result );
    SMILUILOGGER_LEAVEFN( "Video: IsClipEndingL" );
#endif        

    return result;
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::UpdateSoftKeyVisibility
// Sets softkeys visible or invisible depending if video window is blocking soft
// keys and what the current state of the presentation is.
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::UpdateSoftKeyVisibility()
    {
    TRect rect( VideoRect() );
    
    if ( !AknStatuspaneUtils::StaconPaneActive() &&
         ( iState == EReady ||
           iState == EPaused ||
           iState == EHidden ) )
        {
        // Evaluate need to move change the control size because of control pane if
        // stacon is not active and we are at EReady, EPaused or EHidden state.
        
        TRect controlPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EControlPane, controlPaneRect );
        
        if ( rect.iBr.iY > controlPaneRect.iTl.iY )
            {
            // Sofkeys as visible and video window is shown on control pane area
            // => adjust video window so that it is not blocking softkeys
            rect.iBr.iY = controlPaneRect.iTl.iY;
            }
        }
    
    // Use SetSize instead of SetRect as SetRect causes
    // window to be cleared.
    if ( rect.Size() != Size() )
        {
        SetSize( rect.Size() );
        }

#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Video, UpdateSoftKeyVisibility rect: ( %d, %d) ( %d, %d)"),
        rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::DoPause
// Performs the pausing
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::DoPause()
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: DoPause" );
#endif

    TRAPD( error, { 
                  iVideoPlayer->PauseL();
                  iPausePosition = iVideoPlayer->PositionL();
                  } );
                  
    if ( error == KErrNone )
        {
#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITEF( _L("SMILUI: Video, pause position: %d"), I64INT( iPausePosition.Int64() ) );
        SMILUILOGGER_WRITE( "Video: Pausing succeeded!" );
#endif
        iState = EPaused;
        UpdateDrawingArea();
        }
    else
        {
#ifdef VIDEO_DEBUG
        SMILUILOGGER_WRITEF( _L("SMILUI: Video, pausing failed = %d"), error );
#endif
        // Reset the pause position if error was received.
        iPausePosition = 0;
        }

	// Reset volume to previous setting
    if ( iOldVolume )
    	{
    	SetVolume( iOldVolume );
    	iOldVolume = 0;	
    	}	

#ifdef VIDEO_DEBUG
    SMILUILOGGER_LEAVEFN( "Video: DoPause" );
#endif        
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::ControlRect
// Returns media rectangle relative to parent control's rectangle. This is
// basically rectangle for video control.
// ----------------------------------------------------------------------------
//
TRect CSmilVideoRenderer::ControlRect() const
    {
    TRect rect( iMedia->GetRectangle() );
    
    if ( iParent )
        {
        rect.Move( iParent->Position() );
        }
    
    return rect;
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::VideoRect
// Returns video rectangle relative to screen. This is
// basically rectangle given to the video controller.
// ----------------------------------------------------------------------------
//
TRect CSmilVideoRenderer::VideoRect() const
    {
    return TRect( PositionRelativeToScreen(), iMedia->GetRectangle().Size() );    ;
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::PresentationPlaying
// ----------------------------------------------------------------------------
//
TBool CSmilVideoRenderer::PresentationPlaying() const
    {
    return iMedia->Presentation()->State() == CSmilPresentation::EPlaying;
    }
    
// ----------------------------------------------------------------------------
// CSmilVideoRenderer::PlayedForWholePresentation
// ----------------------------------------------------------------------------
//
TBool CSmilVideoRenderer::PlayedForWholePresentation() const
    {
    return iMedia->MediaEnd() >= iMedia->Presentation()->Duration() &&
           iMedia->MediaBegin() <= 0;
    }

// ----------------------------------------------------------------------------
// CSmilVideoRenderer::DoLatePreparationL
// ----------------------------------------------------------------------------
//
void CSmilVideoRenderer::DoLatePreparationL()
    {
#ifdef VIDEO_DEBUG
    SMILUILOGGER_ENTERFN( "Video: DoLatePreparationL" );
#endif

    iError = KErrNone;
    
    iVideoPlayer->OpenFileL( iMediaFile );
        
    iStartTime.HomeTime();
    BeginActiveWait();
            
    if ( iError == KErrNone )
        {            
        DoSetVolumeL( iMedia->Presentation()->Volume() );

        iMedia->RendererDurationChangedL();        
        }
        
#ifdef VIDEO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: iError: %d"), iError );
    SMILUILOGGER_LEAVEFN( "Video: DoLatePreparationL" );
#endif 
    }
    
// End of file
