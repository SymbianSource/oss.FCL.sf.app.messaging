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
*       MsgEditor Audio media control - a Message Editor Base control.
*
*/



// INCLUDE FILES
#include "MsgAudioControl.h"

#include <AudioPreference.h>
#include <DRMCommon.h>
#include <mmf/common/mmcaf.h>                      // TMMSource

#include <MsgEditorCommon.h>

#include "MsgFrameControl.h"            // for FrameControl

#include "MsgMediaControlLogging.h"

// ==========================================================

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgAudioControl::CMsgAudioControl
//
// Constructor.
// ---------------------------------------------------------
//
CMsgAudioControl::CMsgAudioControl( MMsgBaseControlObserver& aBaseControlObserver,
                                    TMdaPriority aPriority,
                                    TMdaPriorityPreference aPreference ) :
    CMsgMediaControl( aBaseControlObserver, EMsgComponentIdAudio, EMsgAudioControl ),
    iPriority( aPriority ),
    iPreference( aPreference )
    {
    }

// ---------------------------------------------------------
// CMsgAudioControl::ConstructL
//
//
// ---------------------------------------------------------
//
void CMsgAudioControl::ConstructL( CMsgEditorView& aParent, MMsgAsyncControlObserver* aObserver )
    {
    BaseConstructL( aParent, aObserver );
    
    SetRect( MsgEditorCommons::MsgDataPane() );
    }


// ---------------------------------------------------------
// CMsgAudioControl::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgAudioControl* CMsgAudioControl::NewL( CMsgEditorView& aParent,
                                                   MMsgAsyncControlObserver* aObserver,
                                                   TMdaPriority aPriority,
                                                   TMdaPriorityPreference aPreference )
    {
    CMsgAudioControl* self = new( ELeave ) CMsgAudioControl( aParent, aPriority, aPreference );
    
    CleanupStack::PushL( self );
    self->ConstructL( aParent, aObserver );
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CMsgAudioControl::~CMsgAudioControl
//
// Destructor
// ---------------------------------------------------------
//
CMsgAudioControl::~CMsgAudioControl()
    {
    delete iAudioPlayer;
    
    iAudioFile.Close();
    }

// ---------------------------------------------------------
// CMsgAudioControl::SetAndGetSizeL
//
// Calculates and sets the size of the control and returns new size as
// reference aSize.
// ---------------------------------------------------------
//
void CMsgAudioControl::SetAndGetSizeL( TSize& aSize )
    {
    // No need to change width
    // Frame size is always larger than icon size
    aSize.iHeight = iFrame->FrameSize().iHeight;
    aSize.iHeight = iBaseLine *
        ( ( aSize.iHeight + iBaseLine - 1 ) / iBaseLine );

    SetSize( aSize );
    }

// ---------------------------------------------------------
// CMsgAudioControl::Reset
//
// Empties the control.
// ---------------------------------------------------------
//
void CMsgAudioControl::Reset()
    {
    Close();
    
    CMsgMediaControl::Reset();
    }

// ---------------------------------------------------------
// CMsgAudioControl::SizeChanged
//
// Called when size is changed.
// ---------------------------------------------------------
//
void CMsgAudioControl::SizeChanged()
    {
    CMsgMediaControl::SizeChanged();
    
    SetPosition( Rect().iTl );
    }


// ---------------------------------------------------------
// CMsgAudioControl::LoadL
// ---------------------------------------------------------
//
void CMsgAudioControl::LoadL( RFile& aFileHandle )
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgAudioControl::LoadL()" );
    
    iAudioFile.Close();
    User::LeaveIfError( iAudioFile.Duplicate( aFileHandle ) );
        
    if ( !iAudioPlayer )
        {
        iAudioPlayer = CMdaAudioPlayerUtility::NewL( *this,
                                                     iPriority,
                                                     iPreference );
        }
    else
        {
        iAudioPlayer->Close();
        }

    TMMFileHandleSource mmSource( iAudioFile, iUniqueContentId );
    iAudioPlayer->OpenFileL( mmSource );
    
    SetState( EMsgAsyncControlStateOpening );
    
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgAudioControl::LoadL()" );
    }

// ---------------------------------------------------------
// CMsgAudioControl::Cancel
// ---------------------------------------------------------
//
void CMsgAudioControl::Cancel()
    {
    if ( iState == EMsgAsyncControlStateOpening )
        {
        iAudioPlayer->Close();
        SetState( EMsgAsyncControlStateIdle );
        }
    }

// ---------------------------------------------------------
// CMsgAudioControl::Play
// ---------------------------------------------------------
//
void CMsgAudioControl::PlayL()
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgAudioControl::PlayL()" );
    
    if ( iAudioPlayer )
        {           
        iAudioPlayer->Play();
        SetState( EMsgAsyncControlStatePlaying );
        }
    
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgAudioControl::PlayL()" );
    }

// ---------------------------------------------------------
// CMsgAudioControl::Close
// ---------------------------------------------------------
//
void CMsgAudioControl::Close()
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgAudioControl::Close()" );
    
    if ( iAudioPlayer )
        {
        iAudioPlayer->Close();
        delete iAudioPlayer;
        iAudioPlayer = NULL;
        }
    
    iAudioFile.Close();
    
    SetState( EMsgAsyncControlStateIdle );
    
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgAudioControl::Close()" );
    }

// ---------------------------------------------------------
// CMsgAudioControl::Stop
// ---------------------------------------------------------
//
void CMsgAudioControl::Stop()
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgAudioControl::Stop()" );
    
    if ( iAudioPlayer )
        {
        iAudioPlayer->Stop();
        SetState( EMsgAsyncControlStateStopped );
        }
    
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgAudioControl::Stop()" );
    }

// ---------------------------------------------------------
// CMsgAudioControl::PauseL
// ---------------------------------------------------------
//
void CMsgAudioControl::PauseL()
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgAudioControl::PauseL()" );
    
    if ( iAudioPlayer )
        {
        User::LeaveIfError( iAudioPlayer->Pause() );
        SetState( EMsgAsyncControlStatePaused );
        }
    
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgAudioControl::PauseL()" );
    }

// =========================================================
// Callbacks from CMdaAudioPlayerUtility
// =========================================================

// ---------------------------------------------------------
// CMsgAudioControl::MapcInitComplete
// ---------------------------------------------------------
//
void CMsgAudioControl::MapcInitComplete( TInt aError, 
                                         const TTimeIntervalMicroSeconds& /*aDuration*/ )
    {
    // Close the handle now
    iAudioFile.Close();
    
    // Needed so that audio controller won't consume DRM rights automatically.
    MMMFDRMCustomCommand* customCommand = iAudioPlayer->GetDRMCustomCommand();
    if ( customCommand )
        {
        customCommand->DisableAutomaticIntent( ETrue );            
        }

    HandleCallback( aError, EFalse, EMsgAsyncControlStateReady );    
    }

// ---------------------------------------------------------
// CMsgAudioControl::MapcPlayComplete
// ---------------------------------------------------------
//
void CMsgAudioControl::MapcPlayComplete( TInt aError )
    {
    HandleCallback( aError, ETrue , EMsgAsyncControlStateStopped );
    }


// ---------------------------------------------------------
// CMsgAudioControl::HandleCallback
// ---------------------------------------------------------
//
void CMsgAudioControl::HandleCallback( TInt aError, 
                                       TBool aDefaultStop,
                                       TMsgAsyncControlState aNewState )
    {
    MSGMEDIACONTROLLOGGER_ENTERFN( "MsgAudioControl::HandleCallback()" );
    MSGMEDIACONTROLLOGGER_WRITEF_ERROR_STATE( _L("CMsgAudioControl::HandleCallbackL: **aError: %d, CurrentState: %S"),aError, iState);
    MSGMEDIACONTROLLOGGER_WRITEF_ERROR_STATE( _L("CMsgAudioControl::HandleCallbackL: **aError: %d, aNewState: %S"),aError, aNewState);
    
    iError = aError;

    // To simulate drm error, use e.g. -30100
    switch ( aError )
        {
        case KErrNone:
            {
            SetState( aNewState );
            break;
            }
        case DRMCommon::EGeneralError:
        case DRMCommon::EUnknownMIME:
        case DRMCommon::EVersionNotSupported:
        case DRMCommon::ESessionError:
        // const TInt KErrCANoRights      =-17452:
        case DRMCommon::ENoRights:
        case DRMCommon::ERightsDBCorrupted:
        case DRMCommon::EUnsupported:
        case DRMCommon::EInvalidRights:
        // const TInt KErrCANoPermission  =-17451: 
        case DRMCommon::ERightsExpired:
            {
            SetState( EMsgAsyncControlStateNoRights );
            break;
            }
		case KErrInUse: // To handle if head phone is removed from the jack while playing audio.
            {
            Stop();
            break;
            }
        case KErrNoMemory:
        case KErrNotSupported:
        case KErrCorrupt:
            {
            // These errors are always "fatal".
            aDefaultStop = EFalse;
            // Fall-through...
            }
        default:
            {
            if ( aDefaultStop )
                {
                Stop();
                }
                
            SetState( EMsgAsyncControlStateCorrupt );
            }
        }
    
    MSGMEDIACONTROLLOGGER_LEAVEFN( "MsgAudioControl::HandleCallback()" );
    }

//  End of File
