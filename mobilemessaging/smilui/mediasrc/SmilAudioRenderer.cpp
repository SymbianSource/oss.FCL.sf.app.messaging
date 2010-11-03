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



#include <mdaaudiosampleplayer.h>
#include <AudioPreference.h>
#include <smilpresentation.h>
#include <mmf/common/mmcaf.h>

#include "SmilAudioRenderer.h"

#ifdef AUDIO_DEBUG
    #include "SmilMediaLogging.h"
#endif

// ----------------------------------------------------------------------------
// CSmilAudioRenderer::CSmilAudioRenderer
// Constructor
// ----------------------------------------------------------------------------
CSmilAudioRenderer::CSmilAudioRenderer( MSmilMedia* aMedia,
                                        DRMCommon& aDrmCommon,
                                        CDRMHelper& aDrmHelper ) : 
    CSmilMediaRendererBase( EMsgMediaAudio, aMedia, aDrmCommon, aDrmHelper ),
    iIntrinsicDuration( TSmilTime::KUnresolved ),
    iState( ENotReady ),
    iError( KErrNone ),
    iRendererAtEnd( EFalse )
    {
    }

// ----------------------------------------------------------------------------
// CSmilAudioRenderer::ConstructL
// ----------------------------------------------------------------------------
void CSmilAudioRenderer::ConstructL( RFile& aFileHandle )
    {
#ifdef AUDIO_DEBUG
    SMILUILOGGER_ENTERFN( "[SMILUI] Audio: ConstructL" );
#endif

    BaseConstructL( aFileHandle );
    User::LeaveIfError( CheckDRMRights() );
    
    User::LeaveIfError( iMediaFile.Duplicate( aFileHandle ) );
    
    iAudioPlayer = CMdaAudioPlayerUtility::NewL( *this,
                                                 KAudioPrioritySmilPlayer,
                                                 TMdaPriorityPreference( KAudioPrefSmilPlayer ) );
    iState = EOpening;
     
    TMMFileHandleSource fileHandleSource( aFileHandle, KDefaultContentObject, ContentAccess::EPeek );
    iAudioPlayer->OpenFileL( fileHandleSource );

    //start active wait
    BeginActiveWait();

#ifdef AUDIO_DEBUG
    SMILUILOGGER_WRITEF( _L("[SMILUI] Audio: iError=%d"), iError );
#endif

    User::LeaveIfError( iError );

    MMMFDRMCustomCommand* customCommand = iAudioPlayer->GetDRMCustomCommand();
    if ( customCommand )
        {
        customCommand->DisableAutomaticIntent( ETrue );            
        }

    if ( iMedia && iMedia->Presentation() )
        {
        // this is a persentage value, [0..100]
        TInt volume = iMedia->Presentation()->Volume();
        
#ifdef AUDIO_DEBUG
        SMILUILOGGER_WRITEF( _L("Audio: Presentation volume, volume=%d"), volume );
        SMILUILOGGER_WRITEF( _L("Audio: iAudioPlayer->SetVolume(), volume=%d"),
                                ( ( iAudioPlayer->MaxVolume() * volume ) / 100 ) );
#endif

        iAudioPlayer->SetVolume( ( iAudioPlayer->MaxVolume() * volume ) / 100 );    
        }
        
#ifdef AUDIO_DEBUG
    SMILUILOGGER_LEAVEFN( "[SMILUI] Audio: ConstructL" );
#endif
    }


// ----------------------------------------------------------------------------
// CSmilAudioRenderer::NewL
// ----------------------------------------------------------------------------
CSmilAudioRenderer* CSmilAudioRenderer::NewL( RFile& aFileHandle,
                                              MSmilMedia* aMedia,
                                              DRMCommon& aDrmCommon,
                                              CDRMHelper& aDrmHelper )
    {
    CSmilAudioRenderer* renderer = new(ELeave) CSmilAudioRenderer( aMedia, 
                                                                   aDrmCommon, 
                                                                   aDrmHelper );       
    CleanupStack::PushL( renderer );
    renderer->ConstructL( aFileHandle );
    CleanupStack::Pop( renderer );
    return renderer;
    }

// ----------------------------------------------------------------------------
// CSmilAudioRenderer::~CSmilAudioRenderer
// ----------------------------------------------------------------------------
CSmilAudioRenderer::~CSmilAudioRenderer()
    {
#ifdef AUDIO_DEBUG
    SMILUILOGGER_ENTERFN( "[SMILUI] Audio: destructor" );
#endif
    if ( iAudioPlayer )
        {
        iAudioPlayer->Close();
        delete iAudioPlayer;
        }
    
    iMediaFile.Close();    
    iMedia = NULL; // For LINT
    }

// ----------------------------------------------------------------------------
// CSmilAudioRenderer::IntrinsicDuration
// ----------------------------------------------------------------------------
TSmilTime CSmilAudioRenderer::IntrinsicDuration() const
    {
#ifdef AUDIO_DEBUG
    SMILUILOGGER_ENTERFN( "[SMILUI] Audio: IntrinsicDuration()" );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Audio: IntrinsicDuration, dur=%d"),
                         iIntrinsicDuration.Value() );
#endif
    return iIntrinsicDuration;
    }

// ----------------------------------------------------------------------------
// CSmilAudioRenderer::PrepareMediaL
// ----------------------------------------------------------------------------
void CSmilAudioRenderer::PrepareMediaL()
    {
#ifdef AUDIO_DEBUG
    SMILUILOGGER_ENTERFN( "[SMILUI] Audio: PrepareMediaL" );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Audio: iState=%d"), iState );
#endif
    
    if ( iState == ENotReady )
        {
#ifdef AUDIO_DEBUG
        SMILUILOGGER_WRITEF( _L("SMILUI: Using late preparation") );
#endif
        
        iError = KErrNone;
        
        iAudioPlayer->OpenFileL( iMediaFile );
        BeginActiveWait();
        }
       
#ifdef AUDIO_DEBUG
    SMILUILOGGER_WRITEF( _L("SMILUI: Audio, State = %d"), iState );
    SMILUILOGGER_LEAVEFN( "Audio: PrepareMediaL" );
    SMILUILOGGER_WRITEF( _L(""));
#endif
    }

// ----------------------------------------------------------------------------
// CSmilAudioRenderer::SeekMediaL
// ----------------------------------------------------------------------------
void CSmilAudioRenderer::SeekMediaL( const TSmilTime& aTime )
    {
#ifdef AUDIO_DEBUG
    SMILUILOGGER_ENTERFN( "[SMILUI] Audio: SeekMediaL" );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Audio: SeekMediaL, time=%d, iState=%d"),
                         aTime.Value(), iState );
#endif
    iAudioPlayer->SetPosition( aTime.ToMicroSeconds() );

    if( aTime == 0 && 
        iRendererAtEnd && 
        iState == EPlaying )
        {
#ifdef AUDIO_DEBUG
        SMILUILOGGER_WRITE( "[SMILUI] Audio: iAudioPlayer->Play() in SeekMediaL()" );
#endif
        iAudioPlayer->Play();
        iRendererAtEnd = EFalse;
        }
    }

// ----------------------------------------------------------------------------
// CSmilAudioRenderer::ShowMediaL
// ----------------------------------------------------------------------------
void CSmilAudioRenderer::ShowMediaL()
    {
#ifdef AUDIO_DEBUG
    SMILUILOGGER_ENTERFN( "[SMILUI] Audio: ShowMedia" );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Audio: iState=%d"), iState );
#endif
    if( iState == EOpen ||
        iState == EHidden )
        {       
#ifdef AUDIO_DEBUG
        SMILUILOGGER_WRITE( "[SMILUI] Audio: iAudioPlayer->Play() in ShowMedia" );
#endif
        ConsumeDRMRightsL();
        if ( iMedia->Presentation()->State() == CSmilPresentation::EPlaying )
            {
            iAudioPlayer->Play();
            iState = EPlaying;
            }
        else
            {
            iState = EPaused;
            }
        }
    }

// ----------------------------------------------------------------------------
// CSmilAudioRenderer::HideMedia
// ----------------------------------------------------------------------------
void CSmilAudioRenderer::HideMedia()
    {
#ifdef AUDIO_DEBUG
    SMILUILOGGER_ENTERFN( "[SMILUI] Audio: HideMedia" );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Audio: iState=%d"), iState );
#endif
    if( iState == EPlaying ||
        iState == EPaused )
        {
#ifdef AUDIO_DEBUG
        SMILUILOGGER_WRITE( "[SMILUI] Audio: iAudioPlayer->Stop()" );
#endif
        iAudioPlayer->Stop();
        iState = EHidden;
        }
    else
        {
#ifdef AUDIO_DEBUG
        SMILUILOGGER_WRITE( "[SMILUI] Audio: Not EPlaying/EPaused -> No Stop()!" );
#endif
        }
    }

// ----------------------------------------------------------------------------
// CSmilAudioRenderer::FreezeMedia
// ----------------------------------------------------------------------------
void CSmilAudioRenderer::FreezeMedia()
    {
#ifdef AUDIO_DEBUG
    SMILUILOGGER_ENTERFN( "[SMILUI] Audio: FreezeMedia" );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Audio: iState=%d"), iState );
#endif
    if ( iState == EPlaying )
        {
#ifdef AUDIO_DEBUG
        SMILUILOGGER_WRITE( "[SMILUI] Audio: iAudioPlayer->Pause()" );
#endif
        TInt err = iAudioPlayer->Pause();
        if ( !err )
            {
            iState = EPaused;
            }
        }          
#ifdef AUDIO_DEBUG
    SMILUILOGGER_LEAVEFN( "[SMILUI] Audio: FreezeMedia" );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilAudioRenderer::ResumeMedia
// ---------------------------------------------------------------------------
void CSmilAudioRenderer::ResumeMedia()
    {
#ifdef AUDIO_DEBUG
    SMILUILOGGER_ENTERFN( "[SMILUI] Audio: ResumeMedia" );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Audio: iState=%d"), iState );
#endif
    if( iState == EPaused )
        {
#ifdef AUDIO_DEBUG
    SMILUILOGGER_WRITE( "[SMILUI] Audio: iAudioPlayer->Play() in ResumeMedia" );
#endif 
        iAudioPlayer->Play();
        iState = EPlaying;
        }
#ifdef AUDIO_DEBUG
    SMILUILOGGER_LEAVEFN( "[SMILUI] Audio: ResumeMedia" );
#endif

    }

// ----------------------------------------------------------------------------
// CSmilAudioRenderer::SetVolume
// ----------------------------------------------------------------------------
void CSmilAudioRenderer::SetVolume( TInt aVolume )
    {
#ifdef AUDIO_DEBUG
    SMILUILOGGER_WRITEF( _L("[SMILUI] Audio: SetVolume, volume=%d"), aVolume );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Audio: iAudioPlayer->SetVolume(), volume=%d"),
                            ( ( iAudioPlayer->MaxVolume() * aVolume ) / 100 ) );
#endif
    // Presentation uses range [0..100]
    iAudioPlayer->SetVolume( ( iAudioPlayer->MaxVolume() * aVolume ) / 100 ); 
    }

// ----------------------------------------------------------------------------
// CSmilAudioRenderer::MapcInitComplete
// ----------------------------------------------------------------------------
void CSmilAudioRenderer::MapcInitComplete(
                            TInt aError,
                            const TTimeIntervalMicroSeconds& aDuration )
    {
#ifdef AUDIO_DEBUG
    SMILUILOGGER_ENTERFN( "[SMILUI] Audio: MapcInitComplete" );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Audio: error=%d"), aError );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Audio: iState=%d"), iState );
#endif

    if ( aError )
        {
        iError = aError;
        iState = ENotReady;                
        }
    else
        {
        iState = EOpen;
        iIntrinsicDuration = TSmilTime::FromMicroSeconds( aDuration );            

#ifdef AUDIO_DEBUG
        SMILUILOGGER_WRITE( "[SMILUI] Audio: iMedia->RendererDurationChangedL()" );
#endif
        TRAPD( error, iMedia->RendererDurationChangedL() );
        if ( error != KErrNone )
            {
#ifdef AUDIO_DEBUG
            SMILUILOGGER_WRITEF( _L("[SMILUI] Audio: RendererDurationChangedL=%d"), error );
#endif
            iError = error;
            iState = ENotReady;
            }
        }

    EndActiveWait();

#ifdef AUDIO_DEBUG
    SMILUILOGGER_LEAVEFN( "[SMILUI] Audio: MapcInitComplete" );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilAudioRenderer::MapcPlayComplete
// ----------------------------------------------------------------------------
void CSmilAudioRenderer::MapcPlayComplete( TInt aError )
    {
#ifdef AUDIO_DEBUG
    SMILUILOGGER_ENTERFN( "[SMILUI] Audio: MapcPlayComplete()" );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Audio: MapcPlayComplete, error=%d"),
                         aError );
    SMILUILOGGER_WRITEF( _L("[SMILUI] Audio: iState=%d"), iState );
#endif
    if ( aError != KErrNone )
        {
        iError = aError;
        //if error goto end
        iRendererAtEnd = ETrue;
        TRAPD( error, iMedia->RendererAtEndL() );
        if ( error != KErrNone )
            {
            iError = error;
            }
        }
    else
        {

#ifdef AUDIO_DEBUG
        SMILUILOGGER_WRITE( "[SMILUI] Audio: iMedia->RendererAtEndL()" );
#endif
        TRAPD( error, iMedia->RendererAtEndL() );
        if( !error )
            {
            iRendererAtEnd = ETrue;
            }
        }
#ifdef AUDIO_DEBUG
    SMILUILOGGER_LEAVEFN( "[SMILUI] Audio: MapcPlayComplete" );
#endif
    }

// End of File
