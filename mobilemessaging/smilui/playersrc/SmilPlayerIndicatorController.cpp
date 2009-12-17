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
* Description: SmilPlayerIndicatorController implementation
*
*/



// INCLUDE FILES
#include <gulicon.h>                // CGulIcon
#include <AknUtils.h>               // AknLayoutUtils
#include <AknsUtils.h>              // AknsUtils
#include <AknStatuspaneUtils.h>     // AknStatuspaneUtils

#include <applayout.cdl.h>
#include <smilplayer.mbg>
#include <AknsConstants.h>

#include <data_caging_path_literals.hrh> 
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include <eiklabel.h>
#include <eikimage.h>

#include "SmilPlayerIndicatorController.h"
#include "SmilPlayerPresentationController.h"
#include "SmilPlayerVolumeIndicatorController.h"
#include "SmilPlayerPauseIndicatorTimer.h"
#include "SmilPlayerTimeIndicatorTimer.h"

// CONSTANTS
_LIT(KSmilPlayerBitmapFile, "smilplayer.mbm");

const TInt KMilliSecondsToSecondsCoefficient = 1000;
const TInt KColorWhite = 0;

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::CSmilIndicatorControllerler
// C++ constructor 
// ----------------------------------------------------------------------------
//
CSmilPlayerIndicatorController::CSmilPlayerIndicatorController( 
                                                    CSmilPlayerPresentationController* aPresController ) :
    iPresController( aPresController ),
    iShowTimeIndicator( ETrue )
    {
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::ConstructL
// Symbian second phase constructor. Sets indicator controller as foreground
// observer so that event about foreground/background changes are received.
// This is needed in order to stop pause timer when SMIL Player is set to 
// background and start it again when SMIL Player is set foreground. Creates
// volume controller if volume is enabled and all other indicator controls and
// their timers.
// ----------------------------------------------------------------------------
//
void CSmilPlayerIndicatorController::ConstructL( const CCoeControl* aParent, 
                                                 TBool aVolumeEnabled  )
    {
    iCoeEnv->AddForegroundObserverL( *this );
    
    SetContainerWindowL( *aParent );
    
    if ( aVolumeEnabled )
        {
        iVolumeController = CSmilPlayerVolumeIndicatorController::NewL( this );
        iVolumeController->SetObserver( this );
        }
    
    iTimeIndicator = new(ELeave) CEikLabel;

    iTimeIndicator->SetTextL( KNullDesC );
    iTimeIndicator->SetContainerWindowL( *this );
    iTimeIndicator->MakeVisible( EFalse );
    
    iTimeModel = CSmilPlayerTimeIndicatorTimer::NewL( this, iCoeEnv );
    
    InitializeIconIndicatorL( iPauseIndicator, 
                              KAknsIIDQgnIndiMmsPause,
                              EMbmSmilplayerQgn_indi_mms_pause,
                              EMbmSmilplayerQgn_indi_mms_pause_mask );
    
    InitializeIconIndicatorL( iPlayIndicator, 
                              KAknsIIDQgnIndiMmsPlay,
                              EMbmSmilplayerQgn_indi_mms_play,
                              EMbmSmilplayerQgn_indi_mms_play_mask );

    iPauseTimer = CSmilPlayerPauseIndicatorTimer::NewL( this );
    
    MakeVisible( EFalse );
    }


// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::NewL
// Symbian two phased constructor
// ----------------------------------------------------------------------------
//
CSmilPlayerIndicatorController*
CSmilPlayerIndicatorController::NewL( const CCoeControl* aParent, 
                                      CSmilPlayerPresentationController* aPresController,
                                      TBool aVolumeEnabled )
    {
    CSmilPlayerIndicatorController* self = new(ELeave) CSmilPlayerIndicatorController( aPresController );

    CleanupStack::PushL( self );
    self->ConstructL( aParent, aVolumeEnabled );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::~CSmilPlayerIndicatorController
// Destructor
// ----------------------------------------------------------------------------
//
CSmilPlayerIndicatorController::~CSmilPlayerIndicatorController()
    {
    iCoeEnv->RemoveForegroundObserver( *this );
    
    delete iVolumeController;
    delete iTimeIndicator;
        
    delete iTimeModel;
    delete iPauseIndicator;
    delete iPauseTimer;

    delete iPlayIndicator;

    iPresController = NULL; // For LINT
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::OfferKeyEventL
// Forwards key event to volume controller if present.
// ----------------------------------------------------------------------------
//
TKeyResponse CSmilPlayerIndicatorController::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                             TEventCode aType )
    {
    TKeyResponse result( EKeyWasNotConsumed );
    
    if ( iVolumeController )
        {
        result = iVolumeController->OfferKeyEventL( aKeyEvent, aType );
        }
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::CountComponentControls
// Returns number of child controls.
// ----------------------------------------------------------------------------
//
TInt CSmilPlayerIndicatorController::CountComponentControls() const
    {
    // return nbr of controls inside this container
    TInt result( 3 );

    if ( iVolumeController )
        {
        result++;
        }
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::ComponentControl
// Returns specified child controls.
// ----------------------------------------------------------------------------
//
CCoeControl* CSmilPlayerIndicatorController::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* result = NULL;

    switch( aIndex )
        {
        case 0:
            {
            result = iPauseIndicator;
            break;
            }
        case 1:
            {
            result = iPlayIndicator;
            break;
            }
        case 2:
            {
            result = iTimeIndicator;
            break;
            }
        case 3:
            {
            result = iVolumeController;
            break;
            }
        default:
            {
            break;
            }
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::Stop
// Indicates that presentation has been stopped. Stops time indicator refreshing.
// ----------------------------------------------------------------------------
//
void CSmilPlayerIndicatorController::Stop()
    {
    iPauseTimer->Cancel();    
    iPauseIndicator->MakeVisible( EFalse );
    
    iPlayIndicator->MakeVisible( EFalse );
        
    iTimeModel->Stop();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::Start
// Indicates that presentation has been started. Starts time indicator refreshing.
// ----------------------------------------------------------------------------
//
void CSmilPlayerIndicatorController::Start()
    {
    iPauseTimer->Cancel();    
    iPauseIndicator->MakeVisible( EFalse );

    iPlayIndicator->MakeVisible( ETrue );
        
    iTimeModel->Start();
    }
        
// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::Pause
// Indicates that presentation has been paused. Pauses time indicator refreshing
// and play indicator is set hidden.
// ----------------------------------------------------------------------------
//
void CSmilPlayerIndicatorController::Pause()
    {
    iPlayIndicator->MakeVisible( EFalse );
    
    SetPauseIndicatorBlinking( ETrue );
    
    iTimeModel->Pause();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::Resume
// Indicates that presentation has been resumed. Starts time indicator refreshing.
// And prevents time indicator from showing if duration is not finite. Also sets
// play indicator visible.
// ----------------------------------------------------------------------------
//
void CSmilPlayerIndicatorController::Resume()
    {
    iTimeModel->Resume();
    SetPauseIndicatorBlinking( EFalse );
    
    iPlayIndicator->MakeVisible( ETrue );    
    }

        
// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::EndReached
// Indicates the presentation end has been reached. This notification from presentation
// controller is forwarded to time indicator timer. Also hides play indicator.
// ----------------------------------------------------------------------------
//
void CSmilPlayerIndicatorController::EndReached()
    {
    iPlayIndicator->MakeVisible( EFalse );
    
    iTimeModel->EndReached();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::SetPauseIndicatorBlinking
// Controls pause indicator blinking. State changes are notified to volume
// controller (if present) so that volume indicator's left arrow state can
// be changed accordingly if needed.
// ----------------------------------------------------------------------------
//
void CSmilPlayerIndicatorController::SetPauseIndicatorBlinking( TBool aValue )
    {        
    iPauseIndicator->MakeVisible( aValue );
    
    if( aValue )
        {        
        //start blinking timer
        iPauseTimer->StartTimer();
        }
    else
        {
        //stop blinking timer
        iPauseTimer->Cancel();
        }
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::VolumeValue
// Asks current volume level from volume controller if present and returns it
// caller. Otherwise return zero.
// ----------------------------------------------------------------------------
//
TInt CSmilPlayerIndicatorController::VolumeValue() const
    {
    TInt result( 0 );
    if ( iVolumeController )
        {
        result = iVolumeController->VolumeValue();
        }
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::TimeChangedL
// Called by timer indicator timer. Updates time indicator with new time string.
// On landscape mode time indicator is not shown if volume indicator is visible
// ----------------------------------------------------------------------------
//
void CSmilPlayerIndicatorController::TimeChangedL( const TDesC& aTimeString )
    {
    if ( !iTimeIndicator->IsVisible() )
        {
        if ( iShowTimeIndicator ||
             !iVolumeController || 
             !iVolumeController->IsVisible() )
            {
            iTimeIndicator->MakeVisible( ETrue );
            }
        }       
        
    iTimeIndicator->SetTextL( aTimeString );
    iTimeIndicator->DrawDeferred();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::CurrentTime
// Returns current presentation time.
// ----------------------------------------------------------------------------
//
TInt CSmilPlayerIndicatorController::CurrentTime() const
    {
    TInt result( 0 );
    if ( iPresController->Presentation() )
        {
        result = iPresController->Presentation()->CurrentTime().Value() / 
                 KMilliSecondsToSecondsCoefficient;
        }
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::PresentationDuration
// Returns presentation duration.
// ----------------------------------------------------------------------------
//
TInt CSmilPlayerIndicatorController::PresentationDuration() const
    {
    TInt result( 0 );
    if ( iPresController->Presentation() )
        {
        result = iPresController->Presentation()->Duration().Value() /
                 KMilliSecondsToSecondsCoefficient;
        }
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::IsDurationFinite
// Returns whether presentation duration is finite (i.e. not infinite).
// ----------------------------------------------------------------------------
//
TBool CSmilPlayerIndicatorController::IsDurationFinite() const
    {
    TBool result( EFalse );
    if ( iPresController->Presentation() )
        {
        result = iPresController->Presentation()->Duration().IsFinite();
        }
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::TogglePauseIndicator
// Toggles pause indicator (i.e. sets it visible if it was previous invisible and
// other way around.
// ----------------------------------------------------------------------------
//
void CSmilPlayerIndicatorController::TogglePauseIndicator() const
    {
    iPauseIndicator->MakeVisible( !iPauseIndicator->IsVisible() );
    iPauseIndicator->DrawDeferred();    
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::HandleGainingForeground
// Starts pause indicator blinking if presentation is on the paused state. 
// Pause indicator blinking was stopped when player was sent to background so
// that timer would not waste processing time unnecessarely.
// ----------------------------------------------------------------------------
//
void CSmilPlayerIndicatorController::HandleGainingForeground()
    {
    if ( iPresController->Presentation() &&
         iPresController->Presentation()->State() == CSmilPresentation::EPaused )
        {
        SetPauseIndicatorBlinking( ETrue );
        }
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::HandleLosingForeground
// Stops the pause indicator blinking if presentation is on paused state.
// ----------------------------------------------------------------------------
//
void CSmilPlayerIndicatorController::HandleLosingForeground()
    {
    if ( iPresController->Presentation() &&
         iPresController->Presentation()->State() == CSmilPresentation::EPaused )
        {
        SetPauseIndicatorBlinking( EFalse );
        }
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::LayoutIndicators
// Sets correct layout for indicator controls according current LAF.
// On landscape mode only time indicator or volume indicator is shown at once.
// Both are shown on the same place and time indicator is hidden if volume indicator
// shown.
// ----------------------------------------------------------------------------
//
void CSmilPlayerIndicatorController::LayoutIndicators()
    {
    TAknLayoutRect smilstatusVolumePane;
    if ( AknStatuspaneUtils::StaconPaneActive() )
        {
		smilstatusVolumePane.LayoutRect( Rect(), AknLayout::Navi_pane_tab_elements_Line_1() );
        }
    else
        {
		smilstatusVolumePane.LayoutRect( Rect(), 
                                     AknLayoutScalable_Apps::smil_status_volume_pane() );
        }
    
    TAknLayoutRect stateIndicatorIcon;
    TAknLayoutText timeLayout;
    if( iPresController->UseWidescreenStatusPane() )
        {            
        TAknLayoutRect naviSmilPane;
         naviSmilPane.LayoutRect(Rect(), AknLayoutScalable_Avkon::navi_smil_pane( 0 ));
        stateIndicatorIcon.LayoutRect( naviSmilPane.Rect(),
                                   AknLayoutScalable_Avkon::navi_smil_pane_g1( 0 ) );
        timeLayout.LayoutText(  naviSmilPane.Rect(),
                            AknLayoutScalable_Avkon::navi_smil_pane_t1() );
        }
    else
        {
    stateIndicatorIcon.LayoutRect( Rect(),
                                   AknLayoutScalable_Apps::smil_status_pane_g1() );
        timeLayout.LayoutText(  Rect(),
                             AknLayoutScalable_Apps::smil_status_pane_t1() );
        }
    
    if ( iVolumeController )
        {
        iVolumeController->SetRect( smilstatusVolumePane.Rect() );
        }
    
    
    TRect timeRect;
    
    // TODO: Remove hack when changes have been made to LAF.
    if ( AknStatuspaneUtils::StaconPaneActive() )
        {
        timeRect = smilstatusVolumePane.Rect();
        
        iShowTimeIndicator = EFalse;
        iTimeIndicator->MakeVisible( EFalse );
        }
    else
        {
        timeRect = timeLayout.TextRect();
        
        iShowTimeIndicator = ETrue;
        }
        
    iTimeIndicator->SetRect( timeRect );
    iTimeIndicator->SetFont( timeLayout.Font() );
    
    TRAP_IGNORE( UpdateTextColorL() );
    
    TInt align( ELayoutAlignNone );
    switch( timeLayout.Align() )
        {
        case CGraphicsContext::ELeft:
            {
            align = ELayoutAlignLeft;
            break;
            }
        case CGraphicsContext::ECenter:
            {
            align = ELayoutAlignCenter;
            break;
            }
        case CGraphicsContext::ERight:
            {
            align = ELayoutAlignRight;
            break;
            }
        default:
            {
            break;
            }
        }
    iTimeIndicator->SetLabelAlignment( align );
    
    SetIconIndicatorExtent( iPauseIndicator, stateIndicatorIcon.Rect() );
    SetIconIndicatorExtent( iPlayIndicator, stateIndicatorIcon.Rect() );
 
    DrawDeferred();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::InitializeIconIndicatorL
// Creates and initializes given icon control with specified bitmap and mask.
// ----------------------------------------------------------------------------
//
void CSmilPlayerIndicatorController::InitializeIconIndicatorL( CEikImage*& aIndicator, 
                                                               const TAknsItemID& aItem, 
                                                               const TInt aBitmapIndex,
                                                               const TInt aMaskIndex ) const
    {
    aIndicator = new(ELeave) CEikImage;
    aIndicator->SetContainerWindowL( *this );
    aIndicator->MakeVisible( EFalse );
    
    SetIconIndicatorBitmapL( aIndicator, aItem, aBitmapIndex, aMaskIndex );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::HandleControlEventL
// Handles control events from volume indicator controller. 
// Sets new volume to presentation if it has been changed.
// On landscape mode toggles time and volume indicators.
// ----------------------------------------------------------------------------
//
void CSmilPlayerIndicatorController::HandleControlEventL( CCoeControl* aControl, 
                                                          TCoeEvent aEventType )
    {
    if ( aControl == iVolumeController &&
         aEventType == MCoeControlObserver::EEventStateChanged )
        {
        TInt currentVolume( VolumeValue() * 10 );
        if ( iPresController->Presentation() )
            {
            if ( currentVolume != iPresController->Presentation()->Volume() )
                {
                iPresController->Presentation()->SetVolume( currentVolume );
                }
            }
        }
    }
    
// ---------------------------------------------------------
// CSmilPlayerIndicatorController::HandlePointerEventL
// Process pointer events. Forwards corresponding pointer event 
// to volume controller if present. Otherwise performs play pausing
// on tapping.
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CSmilPlayerIndicatorController::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        if ( iVolumeController &&
             iVolumeController->Rect().Contains( aPointerEvent.iPosition ) )
            {
            iVolumeController->HandlePointerEventL( aPointerEvent );                    
            }
        else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
            {
            iPresController->PlayPauseTappedL();    
            }
        }            
    }  

#else
void CSmilPlayerIndicatorController::HandlePointerEventL( const TPointerEvent& /*aPointerEvent*/ )
    {  
    }
#endif // RD_SCALABLE_UI_V2

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::HandleResourceChange
//
// Handles skin (theme) changing.
// ----------------------------------------------------------------------------
//
void CSmilPlayerIndicatorController::HandleResourceChange( TInt aType )
	{
	CCoeControl::HandleResourceChange( aType );
	
	if ( aType == KAknsMessageSkinChange )
		{		
	    TRAP_IGNORE( DoHandleSkinChangeL() );
		}
	}

// ---------------------------------------------------------
// CSmilPlayerIndicatorController::DoHandleSkinChangeL
// ---------------------------------------------------------
//
void CSmilPlayerIndicatorController::DoHandleSkinChangeL()
    {
    UpdateTextColorL();
    SetIconIndicatorBitmapL( iPauseIndicator, 
                             KAknsIIDQgnIndiMmsPause,
                             EMbmSmilplayerQgn_indi_mms_pause,
                             EMbmSmilplayerQgn_indi_mms_pause_mask );
    
    SetIconIndicatorBitmapL( iPlayIndicator, 
                             KAknsIIDQgnIndiMmsPlay,
                             EMbmSmilplayerQgn_indi_mms_play,
                             EMbmSmilplayerQgn_indi_mms_play_mask );
    
    DrawDeferred();
    }

// ---------------------------------------------------------
// CSmilPlayerIndicatorController::UpdateTextColorL
//
// Sets the correct text color for time indicator from currently 
// used theme. 
// ---------------------------------------------------------
//
void CSmilPlayerIndicatorController::UpdateTextColorL()
    {
    TRgb timeColor( AKN_LAF_COLOR( KColorWhite ) );
    
    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), 
                               timeColor, 
                               KAknsIIDQsnIconColors,
                               EAknsCIQsnIconColorsCG7 );
    
    iTimeIndicator->OverrideColorL( EColorLabelText, timeColor );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::LayoutArrowIcons
//
// Sets correct size for arrow icons.
// ----------------------------------------------------------------------------
//
void CSmilPlayerIndicatorController::SetIconIndicatorBitmapL( CEikImage*& aIndicator, 
                                                              const TAknsItemID& aItem, 
                                                              const TInt aBitmapIndex,
                                                              const TInt aMaskIndex ) const
    {
    TParse fileParse;
    User::LeaveIfError( fileParse.Set( KSmilPlayerBitmapFile, &KDC_APP_BITMAP_DIR, NULL ) );
    
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    
    AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(), 
                                 aItem,
                                 KAknsIIDQsnIconColors,
                                 EAknsCIQsnIconColorsCG7,
                                 bitmap, 
                                 mask,
                                 fileParse.FullName(), 
                                 aBitmapIndex,
                                 aMaskIndex,
                                 AKN_LAF_COLOR( KColorWhite ) ); 
                                     
    
    aIndicator->SetNewBitmaps( bitmap, mask );
    
    if ( aIndicator->Size() != TSize() )
        {
        SetIconIndicatorExtent( aIndicator, aIndicator->Rect() );
        }
    }

// ----------------------------------------------------------------------------
// CSmilPlayerIndicatorController::SetIconIndicatorExtent
//
// Sets given icon extent to specified one.
// ----------------------------------------------------------------------------
//
void CSmilPlayerIndicatorController::SetIconIndicatorExtent( CEikImage* aIconIndicator,
                                                             TRect aExtent ) const
    {
    TInt result = AknIconUtils::SetSize( const_cast<CFbsBitmap*>( aIconIndicator->Bitmap() ), 
                                         aExtent.Size() );
    
    if ( result == KErrNone )
        {
        if ( aExtent != aIconIndicator->Rect() )
            {
            aIconIndicator->SetRect( aExtent );
            }
        }
    else
        {
        aIconIndicator->SetRect( TRect() );
        }
    }

// End of File  
