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
* Description: SmilPlayerPresentationController implementation
*
*/


// INCLUDE FILES
#include "SmilPlayerPresentationController.h"

#include <eikenv.h>
#include <avkon.hrh>
#include <AknWaitDialog.h>          //Wait note
#include <AknQueryDialog.h>         // Confirmation dialog
#include <AknStatuspaneUtils.h>     // AknStatuspaneUtils

#include <applayout.cdl.h> // LAF
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <StringLoader.h>

#include <gmxmldocument.h>
#include <gmxmlnode.h>
#include <gmxmlelement.h>

#include <smilmedia.h>
#include <smilparser.h>             //SmilParser
#include <smilpresentation.h>       //SmilEngine
#include <smiltransitionfactory.h>
#include <smilobject.h>

#include <aiwdialdata.h>
#include <AiwCommon.h>
#include <AiwServiceHandler.h>

#include <commonphoneparser.h>      // Phonenumber parser
#include <sendui.h>                 // for CSendUi
#include <CMessageData.h>           // CMessageData
#include <SenduiMtmUids.h>          // Mtm uids
#include <apgcli.h>                 // RApaLsSession for WMLBrowser launch

#include <aknnavi.h>
#include <aknnavide.h>

#include <browserlauncher.h>

#include <txtetext.h>           // for CPlainText
#include "SmilTextRenderer.h"
#include "SmilPlayerPanic.h"
#include "SmilPlayerPresentationControl.h"
#include "SmilPlayerDialog.h"
#include "SmilPlayerLinkParser.h"
#include "SmilPlayerIndicatorController.h"
#include "SmilPlayerSynchXmlParser.h"
#include <SmilPlayer.rsg>

#ifdef PRESCONTROLLER_DEBUG
#include "SMilPlayerLogging.h"
#endif

// CONSTANTS
const TReal KLayoutScaleDown = 0.20;
const TReal KLayoutScaleUp = 2.00;

const TInt KScrollModifierCode = 32769;
const TInt KScrollScanCode = 3;

_LIT( KInternalLinkMark, "#" );
_LIT( KSpace, " " ); // Space for WML Browser's parameter

_LIT( KActivate, "activateEvent" );
_LIT( KWmlBrowserParameter, "4" ); // magic number for Wml browser to open the URL



// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::CSmilPlayerPresentationController
// C++ constructor. Initializes class member variables.
// ----------------------------------------------------------------------------
//
CSmilPlayerPresentationController::CSmilPlayerPresentationController( CMDXMLDocument* aDOM,
                                                                      MMediaFactoryFileInfo* aFileInfo,
                                                                      CSmilPlayerDialog* aParent,
                                                                      RFile& aFileHandle ) :
    iDOM( aDOM ),
    iFileInfo( aFileInfo ),
    iParent( aParent ),
    iFileHandle( aFileHandle )
    {
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::ConstructL
// Symbian 2nd phase constructor. Creates indicator controller and tranfers it's ownership
// to indicator container. Also creates buffer bitmap and context for it that
// are used as a double buffer for drawing the presentation (i.e. presentation
// is first drawn to buffer and then buffer is drawn to screen). After this media
// factory (component that creates media renderers) and transition effect factory
// (component that creates transition effect filters) are created. Also presentation
// control that performs presentation drawing is created and initialized. Presentation
// parsing is done next and if everything went ok all the controls are layed out correctly.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::ConstructL( const TRect& aRect, 
                                                    const TDesC& aBaseUrl, 
                                                    TBool aVolumeEnabled )
    {
#ifdef PRESCONTROLLER_DEBUG
    SMILPLAYERLOGGER_ENTERFN( "[SMILPLAYER] PresController: ConstructL" );
    SMILPLAYERLOGGER_WRITEF( _L("[SMILPLAYER] PresController: aRect=(%d, %d) (%d, %d)"), 
        aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY );
#endif
 
    CreateWindowL();
    SetRect( aRect );
    
    // Overlap flag set to EFalse at construction time
    SetPresentationOverlapped( EFalse );
            
    CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();
    CAknNavigationControlContainer* naviContainer = 
        static_cast<CAknNavigationControlContainer*>( sp->ControlL( 
                                                            TUid::Uid( EEikStatusPaneUidNavi ) ) );
    
    // Not owned by presentation controller.
    iIndicatorController = CSmilPlayerIndicatorController::NewL( naviContainer, 
                                                                 this,
                                                                 aVolumeEnabled );
    
    // iIndicatorController ownership transferred to iIndicatorContainer.
    iIndicatorContainer = CAknNavigationDecorator::NewL( naviContainer, iIndicatorController );
    iIndicatorContainer->SetContainerWindowL( *naviContainer );
    iIndicatorContainer->MakeScrollButtonVisible( EFalse );
    iIndicatorContainer->SetComponentsToInheritVisibility( ETrue );
    
    InitializeDoubleBufferL();
    
    iMediaFactory = CSmilPlayerMediaFactory::NewL( *iCoeEnv,
                                                   iBufferContext,
                                                   iFileInfo );

    iTransitionFactory = new(ELeave) CSmilTransitionFactory( iBufferBitmap );

    iPresentationControl = CSmilPlayerPresentationControl::NewL( this );
    iPresentationControl->SetRect( Rect() );

    iBaseUrl = aBaseUrl.AllocL();
    
    TRAPD( error, DoParsingL() );
    if ( error == KErrNone )
        {
        LayoutPresentationControl();
        
        SetComponentsToInheritVisibility( ETrue );
        MakeVisible( EFalse );
        }
    else 
        {
#ifdef PRESCONTROLLER_DEBUG
        SMILPLAYERLOGGER_WRITEF( _L("[SMILPLAYER] PresController: Presentation creation failed=%d"), error );
#endif
        delete iWaitDialog;
        iWaitDialog = NULL;
        
        // All other error cases are handled on CSmilDialog::ActivateL function
        if ( error == KErrNoMemory )
            {
            User::Leave( error );
            }
        }

#ifdef PRESCONTROLLER_DEBUG
    SMILPLAYERLOGGER_LEAVEFN( "[SMILPLAYER] PresController: ConstructL" );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::NewL
// Symbian two phased constructor
// ----------------------------------------------------------------------------
//
CSmilPlayerPresentationController* CSmilPlayerPresentationController::NewL( const TRect& aRect,
                                                                            const TDesC& aBaseUrl,
                                                                            MMediaFactoryFileInfo* aFileInfo,
                                                                            CSmilPlayerDialog* aParent,
                                                                            CMDXMLDocument* aDOM,
                                                                            TBool aVolumeEnabled )
    {
    RFile tempHandle;
    CSmilPlayerPresentationController* self = new(ELeave) CSmilPlayerPresentationController( aDOM,
                                                                                             aFileInfo,
                                                                                             aParent,
                                                                                             tempHandle );
    
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aBaseUrl, aVolumeEnabled );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::NewL
// Symbian two phased constructor
// ----------------------------------------------------------------------------
//
CSmilPlayerPresentationController* CSmilPlayerPresentationController::NewL( const TRect& aRect,
                                                                            const TDesC& aBaseUrl,
                                                                            MMediaFactoryFileInfo* aFileInfo,
                                                                            CSmilPlayerDialog* aParent,
                                                                            RFile& aFileHandle,
                                                                            TBool aVolumeEnabled )
    {
    CSmilPlayerPresentationController* self = new(ELeave) CSmilPlayerPresentationController( NULL,
                                                                                             aFileInfo,
                                                                                             aParent,
                                                                                             aFileHandle );
    
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aBaseUrl, aVolumeEnabled );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::~CSmilPlayerPresentationController
// Destructor
// ----------------------------------------------------------------------------
//
CSmilPlayerPresentationController::~CSmilPlayerPresentationController()
    {
    delete iPresentation;
    delete iMediaFactory;   
    delete iTransitionFactory;   
    delete iBufferContext;
    delete iBufferDevice;
    delete iBufferBitmap;
    delete iPresentationControl;
    delete iSendUi;
    delete iIndicatorContainer;
    delete iWaitDialog;

    delete iServiceHandler;

    delete iBaseUrl;
    if ( iDOMInternallyOwned )
        {
        delete iDOM;
        }
    
    iFileInfo = NULL; // For Lint
    iParent = NULL;   // For Lint
    iIndicatorController = NULL; // For Lint. Ownership transferred to decorator.
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::ChangeStateL
// Changes controller state to specified one. Indicator state is change here or
// on PresentationEvent function depending which one is more approriate for this state.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::ChangeStateL( TSmilPresControllerStates aState )
    {
#ifdef PRESCONTROLLER_DEBUG
    SMILPLAYERLOGGER_ENTERFN( "[SMILPLAYER] PresController: ChangeStateL" );
    SMILPLAYERLOGGER_WRITEF( _L("[SMILPLAYER] PresController: aState=%d"), aState );
#endif
    
    if ( !iPresentation )
        {
        __ASSERT_DEBUG( EFalse, Panic( ESmilPlayerPresentationNotReady ) );
        return;
        }
        
    switch ( aState )
        {
        case EStatePlay:
            {
            iIndicatorController->Start();
            
            if ( iPresentation->State() != CSmilPresentation::EPlaying )
                {
                iPresentation->PlayL();
                }
            break;
            }
        case EStateStop:
            {
            if ( iPresentation->State() != CSmilPresentation::EStopped )
                {
                iPresentation->StopL();
                }
            break;
            }
        case EStatePause:
            {
            if ( iPresentation->State() == CSmilPresentation::EPlaying )
                {
                iPresentation->Pause();                   
                }
            break;
            }
        case EStateResume:
            {
            if ( iPresentation->State() != CSmilPresentation::EPlaying )
                {
                iPresentation->Resume();
                }
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( ESmilPlayerUnknownState ) );
            break;
            }
        }
        
#ifdef PRESCONTROLLER_DEBUG
    SMILPLAYERLOGGER_LEAVEFN( "[SMILPLAYER] PresController: ChangeStateL" );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::StopTimer
// Stops timer indicator.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::StopTimer()
    {
    iIndicatorController->Stop();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::RedrawArea
// Called by SMIL Engine. Calls presentation control to redraw given area.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::RedrawArea( const TRect& aArea ) const
    {
    iPresentationControl->RedrawArea( aArea );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::SetStatusPaneVisibilityL
// Controls statuspane layout. aValue is ETrue "normal" statuspane is used and
// if aValue is EFalse SMIL Player own statuspane is used.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::SetStatusPaneVisibilityL( TBool aValue )
    {
    TRect newRect;
    
    TRect applicationWindow;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EApplicationWindow, applicationWindow );

    TAknLayoutRect mainPane;
    
    if ( CSmilPlayerPresentationController::UseWidescreenStatusPane() )
        {
        // select appropriate main_pane variety based on presentation overlap flag
        if ( IsPresentationOverlapped() )
            mainPane.LayoutRect( applicationWindow, AknLayoutScalable_Avkon::main_pane( 4 ) );
        else
            mainPane.LayoutRect( applicationWindow, AknLayoutScalable_Avkon::main_pane( 22 ) );
        }
    else if ( AknStatuspaneUtils::StaconPaneActive() )
        {
        mainPane.LayoutRect( applicationWindow, AknLayoutScalable_Avkon::main_pane( 4 ) );   
        }
    else
        {
        if ( aValue )
            {
            mainPane.LayoutRect( applicationWindow, AknLayoutScalable_Avkon::main_pane( 5 ) );
            }
        else
            {
            mainPane.LayoutRect( applicationWindow, AknLayoutScalable_Avkon::main_pane( 0 ) );
            }
        }

    TAknLayoutRect mainSmilPane;
    mainSmilPane.LayoutRect( mainPane.Rect(), AknLayoutScalable_Apps::main_smil_pane() );
        
    newRect = mainSmilPane.Rect();
        
    TInt notUsed( 0 );
    
    if ( aValue )
        {
        SwitchStatusPaneL( R_AVKON_STATUS_PANE_LAYOUT_USUAL, EFalse, notUsed );
        SetRect( newRect );
        }
    else
        {
        SetRect( newRect );
        
        TInt newStatusPaneResId = R_AVKON_STATUS_PANE_LAYOUT_SMALL;
        if ( CSmilPlayerPresentationController::UseWidescreenStatusPane() )
            {
            newStatusPaneResId = R_AVKON_WIDESCREEN_PANE_LAYOUT_USUAL_FLAT;
            }

        SwitchStatusPaneL( newStatusPaneResId, ETrue, notUsed );
        }
    
    iOwnStatusPaneVisible = !aValue;
    
    DrawNow();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::AnchorExist
// Returns if anchor exists on presentation. Anchors are links pointing to 
// internal place inside presentation or external place outside presentation 
// (such as HTTP, mailto and callto links).
// ----------------------------------------------------------------------------
//
TBool CSmilPlayerPresentationController::AnchorExist() const
    {
    TBool result( EFalse );
    CSmilObject* focused = iPresentation->FocusedObject();
    
    if ( focused )
        {
        CSmilAnchor* anchor = static_cast<CSmilMedia*>( focused )->Anchor();
        if ( anchor && anchor->Href().Length() > 0 )
            {
            result = ETrue;
            }
        }
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::AnchorIsInternalLink
// Retuns whether anchor (i.e. link) is internal (i.e. points inside
// presentation).
// ----------------------------------------------------------------------------
//
TBool CSmilPlayerPresentationController::AnchorIsInternalLink() const
    {
    TBool result( EFalse );
    CSmilObject* focused = iPresentation->FocusedObject();
    
    if ( focused )
        {
        CSmilAnchor* anchor = static_cast<CSmilMedia*>( focused )->Anchor();
        if ( anchor ) 
            {
            TPtrC href = anchor->Href();

            if( href.Length() >= KInternalLinkMark().Length() ) 
                {
                if( href.Left( KInternalLinkMark().Length() ).MatchF( 
                                KInternalLinkMark ) != KErrNotFound )
                    {
                    result = ETrue;
                    }
                }
            }
        }
    return result;
    }
// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::ScrollingActivationEnabled
// Returns if scrolling can be activated for given focused object. Checks
// if object is focused and it is scrollable and not already activated. 
// ----------------------------------------------------------------------------
//
TBool CSmilPlayerPresentationController::ScrollingActivationEnabled() const
    {
    TBool result( EFalse );
    #ifdef RD_SCALABLE_UI_V2
    if ( AknLayoutUtils::PenEnabled() )
    return result;
    #endif
    CSmilObject* focused = iPresentation->FocusedObject();
    
    if ( focused )
        {
        result = ( static_cast<CSmilMedia*>( focused )->IsScrollable() &&
                   !iPresentation->MediaSelected() );
        }
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::FocusedObjectScrollable
// Returns if focused object is scrollable. Checks if object is focused and it is
// scrollable.
// ----------------------------------------------------------------------------
//
TBool CSmilPlayerPresentationController::FocusedObjectScrollable() const
    {
    TBool result( EFalse );
    CSmilObject* focused = iPresentation->FocusedObject();
    
    if ( focused )
        {
        result = static_cast<CSmilMedia*>( focused )->IsScrollable();
        }
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::PrepareExitL
// Switches statuspane to the one that was used using resource ID that was
// stored on InitializeL. Sets controller also invisible and refreshes the screen
// for better drawing result.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::PrepareExitL()
    {
    //set layout back 
    TInt notUsed;
    SwitchStatusPaneL( iLayoutResId, EFalse, notUsed );
    iOwnStatusPaneVisible = EFalse;
    
    MakeVisible( EFalse );
    DrawNow();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::Initialize
// Initializes and activates the dialog. Switches statuspane to small statuspane.
// Also stores old statuspane resource ID. Also activates controller and sets
// it visible and refreshes the screen. This is done for better drawing result.
// Returns EFalse if presentation parsing was not successful.
// ----------------------------------------------------------------------------
//
TBool CSmilPlayerPresentationController::InitializeL()
    {
    TBool result( EFalse );
    
    if ( iPresentation )
        {
        MakeVisible( ETrue );
        ActivateL();  
        
        TInt newStatusPaneResId = R_AVKON_STATUS_PANE_LAYOUT_SMALL;
        if ( CSmilPlayerPresentationController::UseWidescreenStatusPane() )
            {
            newStatusPaneResId = R_AVKON_WIDESCREEN_PANE_LAYOUT_USUAL_FLAT_NO_SOFTKEYS;
            }
        
        SwitchStatusPaneL( newStatusPaneResId, 
                           ETrue,
                           iLayoutResId );
        iOwnStatusPaneVisible = ETrue;
        
        DrawNow();
        
        result = ETrue;
        }
        
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::ActivateScrolling
// Activates scolling. This is done by giving EKeyEnter event to the presentation.
// Also if non-internal anchor (link) exists flag is raised so that open document
// cannot be called (can panic).
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::ActivateScrollingL()
    {
    TKeyEvent keyEvent;
    keyEvent.iCode = EKeyEnter;
    keyEvent.iScanCode = KScrollScanCode;
    keyEvent.iModifiers = KScrollModifierCode;
    keyEvent.iRepeats = 0;
    TEventCode type = EEventKey;

    if( AnchorExist() &&
        !AnchorIsInternalLink() )
        {
        iScrollingTogglePerformedWhileLinkExist = ETrue;
        }

    iPresentation->HandleKeyEventL( keyEvent, type);
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::DeactivateScrollingL
// Deactivates scolling (same implementation as activate scrolling)
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::DeactivateScrollingL()
    {
    // The implemention is exactly as activateScrolling()
    ActivateScrollingL();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::HandleVolumeKeyEventL
// Calls DoHandleVolumeKeyL to handle volume key event.
// ----------------------------------------------------------------------------
//
TKeyResponse CSmilPlayerPresentationController::HandleVolumeKeyEventL( const TKeyEvent& aKeyEvent, 
                                                                       TEventCode aType )
    {
    return DoHandleVolumeKeyL( aKeyEvent, aType );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::HandleScrollKeyEventL
// Calls presentation to handle scroll key event if scrolling deactivation is not ongoing.
// ----------------------------------------------------------------------------
//
TKeyResponse CSmilPlayerPresentationController::HandleScrollKeyEventL( const TKeyEvent& aKeyEvent, 
                                                                       TEventCode aType )
    {
    TKeyResponse result( EKeyWasNotConsumed );
    if ( iScrollingTogglePerformedWhileLinkExist )
        {
#ifdef PRESCONTROLLER_DEBUG
        SMILPLAYERLOGGER_WRITEF( _L("[SMILPLAYER] PresController: Changing scrolling mode") );
#endif
        // scrolling deactivation ongoing...
        // should not do any key handling, otherwise can panic
        result = EKeyWasConsumed;
        }
    else
        {
        result = iPresentation->HandleKeyEventL( aKeyEvent, aType );
        }
    return result;
    }


// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::HandleRCSOperation
// Call internal function to handle specified operation. Silently ignores errors.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::HandleRCSOperation( TRemConCoreApiOperationId aOperation )
    {
    TRAP_IGNORE( DoHandleRCSOperationL( aOperation ) );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::DoHandleRCSOperationL
// Performs Remote Control Server operation handling. VolumeUp and VolumeDown
// events are interpreted as right/left key events. 
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::DoHandleRCSOperationL( TRemConCoreApiOperationId aOperation )
    {
    switch ( aOperation )
        {
        case ERemConCoreApiVolumeUp:
            {
            TKeyEvent keyEvent;
            keyEvent.iCode = EKeyRightArrow;
            TEventCode type = EEventKey;
            
            DoHandleVolumeKeyL( keyEvent, type );
            break;
            }
        case ERemConCoreApiVolumeDown:
            {
            TKeyEvent keyEvent;
            keyEvent.iCode = EKeyLeftArrow;
            TEventCode type = EEventKey;
            
            DoHandleVolumeKeyL( keyEvent, type );
            
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::DoHandleVolumeKeyL
// Performs volume key handling. Forwards volume key event to indicator controller
// and if key event is consumed there changes presentation volume accordingly.
// ----------------------------------------------------------------------------
//
TKeyResponse CSmilPlayerPresentationController::DoHandleVolumeKeyL( const TKeyEvent& aKeyEvent, 
                                                                    TEventCode aType )
    {
    TKeyResponse result( EKeyWasNotConsumed);
    
    if ( iPresentation )
        {
        result = iIndicatorController->OfferKeyEventL( aKeyEvent, aType );
        if ( result ==  EKeyWasConsumed )
            {
            iPresentation->SetVolume( iIndicatorController->VolumeValue() * 10 );
            }    
        }
    
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::OpenDocumentL
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::OpenDocumentL( const TDesC& aUrl,
                                                       const CSmilAnchor* /*aAnchor*/,
                                                       const CSmilPresentation* /*aPres*/ )
    {
    if ( iScrollingTogglePerformedWhileLinkExist )
        {
        // used when activating/deactivating scrolling while link exists also
        iScrollingTogglePerformedWhileLinkExist = EFalse;
        return;
        }

    SmilPlayerLinkParser::TSmilPlayerLinkType linkResult = 
                    SmilPlayerLinkParser::LinkType( aUrl );

    switch( linkResult )
        {
        case SmilPlayerLinkParser::ESmilPlayerNotSupported:
            {
            // nothing to do in this case
            break;
            }
        case SmilPlayerLinkParser::ESmilPlayerPhoneNumber:
            {
            // use phone client to make a call
            if ( LoadTextAndConfirmL( R_SMILPLAYER_CONFIRM_CALLTO,
                                 aUrl.Right( aUrl.Length() - KPhoneToString().Length() ) ) )
                {
                CallL( aUrl.Right( aUrl.Length() - KPhoneToString().Length() ) );
                }
            break;
            }
        case SmilPlayerLinkParser::ESmilPlayerMailAddress:
            {
            // open mail editor
            if ( LoadTextAndConfirmL( R_SMILPLAYER_CONFIRM_MAILTO,
                                 aUrl.Right( aUrl.Length() - KMailToString().Length() ) ) )
                {
                SetStatusPaneVisibilityL( ETrue );

                // TRAPD used to catch the case where access point not defined
                // and user doesn't want to define it, and when getting back
                // the StatusPane should be set invisible again
                TRAP_IGNORE( SendMessageL( KSenduiMtmSmtpUid,
                                           aUrl.Right( aUrl.Length() - KMailToString().Length() ) ) );
                SetStatusPaneVisibilityL( EFalse );
                }
            break;
            }
        case SmilPlayerLinkParser::ESmilPlayerUrlAddress:
            {
            // open browser
            if ( LoadTextAndConfirmL( R_SMILPLAYER_CONFIRM_OPEN, aUrl ) )
                {
                GoToUrlL( aUrl );
                }
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::PresentationEvent
// Handles presentation event notifications from SMIL Engine. Indicator
// state is changes here or on ChangeStateL function depending which one
// is more approriate for that state. On end reached state event notifies
// SMIL Player dialog about the presetation end.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::PresentationEvent( TInt aEvent,
                                                           const CSmilPresentation* /*aPres*/,
                                                           const TDesC& /*aUrl*/ )
    {   
#ifdef PRESCONTROLLER_DEBUG
    SMILPLAYERLOGGER_ENTERFN( "[SMILPLAYER] PresController: PresentationEvent" );
    SMILPLAYERLOGGER_WRITEF( _L("[SMILPLAYER] PresController: aEvent=%d"), aEvent );
#endif
  
    switch( aEvent )
        {
        case MSmilPlayer::EStarted:            
        case MSmilPlayer::EStopped:
            {
            // Do not stop indicator controller here because presentation
            // end might follow that requires special handling.
            break;
            }
        case MSmilPlayer::EPaused:
            {
            iIndicatorController->Pause();
            break;
            }
        case MSmilPlayer::EResumed:
            {
            iIndicatorController->Resume();
            break;
            }
        case MSmilPlayer::ESeeked:
        case MSmilPlayer::EStalled:
        case MSmilPlayer::EMediaNotFound:
            {
            break;
            }
        case MSmilPlayer::EEndReached:
            {
            iIndicatorController->EndReached();
            TRAP_IGNORE( iParent->PresentationStoppedL() );
            break;
            }
        default:
            {
            break;
            }
        }

#ifdef PRESCONTROLLER_DEBUG
    SMILPLAYERLOGGER_LEAVEFN( "[SMILPLAYER] PresController: PresentationEvent" );
#endif
    }


// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::LayoutPresentationControl
// Sets correct layout for presentation control. Retrieves presentation root-region
// size (this is always starting from 0,0). After this calculates the top left corner
// so that presentation is centralized.
// ----------------------------------------------------------------------------
//
 void CSmilPlayerPresentationController::LayoutPresentationControl()
    {
    TSize presentationSize = iPresentation->RootRegionRect().Size();

    TPoint newtopleft;
    newtopleft.iX = ( Rect().Width() - presentationSize.iWidth ) / 2;
    newtopleft.iY = ( Rect().Height() - presentationSize.iHeight ) / 2;
    
    iPresentationControl->SetRect( TRect( newtopleft, presentationSize ) );
    iPresentationRect = iPresentationControl->Rect();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::DoParsingL
// Performs SMIL Presentation parsing. If client has not give valid DOM it is 
// parsed from given file handle using XML parser. After valid DOM is present
// SMIL presentation is parsed from it. During SMIL presentation parsing
// wait note is shown if approriate. When presentation is parsed it is initialized
// with correct parameters.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::DoParsingL()
    {
#ifdef PRESCONTROLLER_DEBUG
    SMILPLAYERLOGGER_ENTERFN( "[SMILPLAYER] PresController: DoParsingL" );
#endif

    iWaitDialog = new(ELeave) CAknWaitDialog( ( REINTERPRET_CAST( CEikDialog**, &iWaitDialog ) ) ); 
    iWaitDialog->SetTone( CAknNoteDialog::ENoTone );
    iWaitDialog->ExecuteLD( R_WAIT_NOTE ); 

#ifdef PRESCONTROLLER_DEBUG
    SMILPLAYERLOGGER_WRITEF( _L("[SMILPLAYER] PresController: Wait dialog created") );
#endif

    //dom parsing needed?
    if ( !iDOM )
        {
        CSmilPlayerSynchXmlParser* util = new(ELeave) CSmilPlayerSynchXmlParser;
        CleanupStack::PushL( util );
        
        iDOM = util->ParseL( iFileHandle );
        iDOMInternallyOwned = ETrue;
        
        CleanupStack::PopAndDestroy( util );
        }

    CSmilParser* parser = CSmilParser::NewL( this );
    CleanupStack::PushL( parser );

#ifdef PRESCONTROLLER_DEBUG
    SMILPLAYERLOGGER_WRITEF( _L("[SMILPLAYER] PresController: Parser created") );
#endif
    
    //Set layout scaling mode
    parser->SetMaxDownUpScaling( static_cast<TReal32>( KLayoutScaleDown ),
                                 static_cast<TReal32>( KLayoutScaleUp ) );
    
    iPresentation = parser->ParseL( iDOM, *iBaseUrl );
    
#ifdef PRESCONTROLLER_DEBUG
    SMILPLAYERLOGGER_WRITEF( _L("[SMILPLAYER] PresController: Presentation parsed") );
#endif

    delete iWaitDialog;
    iWaitDialog = NULL;
    
    iPresentation->SetEndAction( CSmilPresentation::EStopAtEnd );
    iPresentation->SetStartAction( CSmilPresentation::EShowFirstFrame );
    iPresentation->SetVolume( iIndicatorController->VolumeValue() * 10 );

    CleanupStack::PopAndDestroy( parser );
    
#ifdef PRESCONTROLLER_DEBUG
    SMILPLAYERLOGGER_LEAVEFN( "[SMILPLAYER] PresController: DoParsingL" );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::CallL
// Creates a call to speficied number. First phone client is created if none exists.
// Then phone number is parsed from the given string and phone call is created
// using phone client.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::CallL( const TDesC& aString )
    {

    if ( !iServiceHandler )
        {
        iServiceHandler = CAiwServiceHandler::NewL();
        }
        
    // Attach to AIW interest
    iServiceHandler->AttachL ( R_SMILPLAYER_CALL_INTEREST );
                
    CAiwDialData* dialData = CAiwDialData::NewLC();
    
    // Set test parameters  
    dialData->SetPhoneNumberL( aString );
    dialData->SetCallType( CAiwDialData::EAIWVoice );
    dialData->SetWindowGroup( iCoeEnv->RootWin().WindowGroupId() );

    CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
    dialData->FillInParamListL( paramList );
 
    // Execute AIW command with notification callback
    iServiceHandler->ExecuteServiceCmdL( KAiwCmdCall, 
                                         paramList,
                                         iServiceHandler->OutParamListL(),
                                         0, 
                                         NULL );
            
    iServiceHandler->DetachL( R_SMILPLAYER_CALL_INTEREST );
        
    CleanupStack::PopAndDestroy( dialData );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::SendMessageL
// Sends message (SMS, MMS or E-mail) referenced e-mail address
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::SendMessageL( const TUid& aMtmUid,
                                                      const TDesC& aString )
    {
    if ( !iSendUi )
        {
        iSendUi = CSendUi::NewL();
        }

    CMessageData* messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );
    
    messageData->AppendToAddressL( aString );
    
    iSendUi->CreateAndSendMessageL( aMtmUid, 
                                    messageData );
                                       
    CleanupStack::PopAndDestroy( messageData );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::GoToUrlL
// Opens browser with given address,
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::GoToUrlL( const TDesC& aString )
    {
    HBufC* parameters = HBufC::NewLC( aString.Length() + 2 ); 
                        // 2 = length of the first parameter + space

    TPtr paramPtr = parameters->Des();
    paramPtr.Copy( KWmlBrowserParameter );
    paramPtr.Append( KSpace );
    paramPtr.Append( aString );

    CBrowserLauncher* launcher = CBrowserLauncher::NewLC();
    
    User::LeaveIfError( launcher->LaunchBrowserSyncEmbeddedL( *parameters ,NULL, NULL ) );
    
    CleanupStack::PopAndDestroy( launcher );
    
    CleanupStack::PopAndDestroy( parameters );

    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::LoadTextAndConfirmL
// Loads correct text from given resource and shown confirmation query dialog
// by using ShowConfirmQueryL. Also forwards user answer to confirmation query
// dialog to caller.
// ----------------------------------------------------------------------------
//
TBool CSmilPlayerPresentationController::LoadTextAndConfirmL( TInt aResourceId, 
                                                             const TDesC& aString ) const
    {
    HBufC* dlgText = StringLoader::LoadLC( aResourceId, aString, iEikonEnv );
    
    TBool result( ShowConfirmQueryL( R_SMILPLAYER_CONFIRM_QUERY, *dlgText ) );
    
    CleanupStack::PopAndDestroy( dlgText );
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::ShowConfirmQueryL
// Shows confirmation query dialog to user by using CAknQueryDialog. Returns 
// ETrue if user selected "Yes" and EFalse if "No".
// ----------------------------------------------------------------------------
//
TBool CSmilPlayerPresentationController::ShowConfirmQueryL( TInt aResource,
                                                            const TDesC& aText ) const
    {
    TBool result( EFalse );
    
    CAknQueryDialog* dialog = CAknQueryDialog::NewL();
    TInt ret = dialog->ExecuteLD( aResource, aText );
    User::LeaveIfError( ret );
    
    if ( ret == EAknSoftkeyYes )
        {
        result = ETrue;
        }    
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::CountComponentControls
// Returns number of presentation controller child controls.
// ----------------------------------------------------------------------------
//
TInt CSmilPlayerPresentationController::CountComponentControls() const
    {
    // return nbr of controls inside this container
    return 1; 
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::ComponentControl
// Returns presentation control child control.
// ----------------------------------------------------------------------------
//
CCoeControl* CSmilPlayerPresentationController::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* result = NULL;
    
    switch ( aIndex )
        {
        case 0: 
            {
            result = iPresentationControl;
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
// CSmilPlayerPresentationController::Draw
// Draws the "borders" of the presentation. By "borders" here is meant
// areas the under statuspane that are not used by presentation. These areas
// are produced when original presentation is so small that even with maximum
// up-scaling presentation does not fill the whole available area or when
// original presentation uses different aspect ratio than playing device.
// To prevent flickering only border areas are drawn instead of whole presentation
// control rectangle.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::Draw( const TRect& /*aRect*/ ) const
    {    
    TRect controllerRect( Rect() );
    TRect presentationRect( iPresentationControl->Rect() );
    
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.SetBrushColor( KRgbWhite );
    
    TRect borderRect;
    
    // Draw the top border if neccessary
    if ( controllerRect.iTl.iY < presentationRect.iTl.iY )
        {
        borderRect.SetRect( controllerRect.iTl, 
                            TPoint( controllerRect.iBr.iX, presentationRect.iTl.iY ) );
        
        gc.DrawRect( borderRect );
        }
    
    // Draw the left border if neccessary
    if ( controllerRect.iTl.iX < presentationRect.iTl.iX )
        {
        borderRect.SetRect( controllerRect.iTl, 
                            TPoint( presentationRect.iTl.iX, controllerRect.iBr.iY ) );
        
        gc.DrawRect( borderRect );
        }
    
    // Draw the right border if neccessary
    if ( controllerRect.iBr.iX > presentationRect.iBr.iX )
        {
        borderRect.SetRect( TPoint( presentationRect.iBr.iX, controllerRect.iTl.iY ),
                            controllerRect.iBr );
        
        gc.DrawRect( borderRect );
        }
        
    // Draw the bottom border if neccessary
    if ( controllerRect.iBr.iY > presentationRect.iBr.iY )
        {
        borderRect.SetRect( TPoint( controllerRect.iTl.iX, presentationRect.iBr.iY ),
                            controllerRect.iBr );
        
        gc.DrawRect( borderRect );
        }
    
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::LayoutPresentationL
// Sets correct layout for presentation control according to current LAF values.
// First flag for layout change is set and presentation existence is checked. If
// presentation does not exist then we are in the middle of presentation creation
// and layout switching is restarted when current presentation creation finishes.
// When performing the real layout change code first indicators are stopped 
// if presentation is playing or paused. After this presentation controller state is 
// reseted and new buffer bitmap & buffer context is created. 
// This new buffer bitmap context is also updated for media factory 
// so that newly created text renderers use correct buffer. Then presentation
// is parsed again so that all media regions and renderers use new corrected
// (this is currently the only way to fit SMIL engine for new layout).
// After this is done presentation control and indicator are adjusted to new
// layout and screen is updated.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::LayoutPresentationL()
    {
    iChangeLayout = ETrue;
    
    if ( iPresentation )
        {
        while ( iChangeLayout )
            {
            CSmilPresentation::TPresentationState presentationState( iPresentation->State() );
    
            iIndicatorController->MakeVisible( EFalse );
            
            if ( presentationState == CSmilPresentation::EPlaying ||
                 presentationState == CSmilPresentation::EPaused )
        	    {
        	    iIndicatorController->Stop();
        	    }
    	
            Reset();
            
            InitializeDoubleBufferL();
            
            iMediaFactory = CSmilPlayerMediaFactory::NewL( *iCoeEnv,
                                                           iBufferContext,
                                                           iFileInfo );

            iTransitionFactory = new( ELeave ) CSmilTransitionFactory( iBufferBitmap );
            
            iPresentationControl->SetRect( Rect() );
            
            iChangeLayout = EFalse;
        	DoParsingL();
        	
        	if ( iChangeLayout )
        	    {
        	    continue;
        	    }
        	    
        	LayoutPresentationControl();
        	
        	iIndicatorController->SetRect( iIndicatorContainer->Rect() );
        	iIndicatorController->LayoutIndicators();
        	
        	iIndicatorController->MakeVisible( ETrue );
        	
        	iPresentation->PlayL();
        	iPresentation->StopL(); 
        	
        	DrawDeferred();
            }
        }
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::IsPresentationOverlapped
// Returns presentation overlap flag status
// ----------------------------------------------------------------------------
//
TBool CSmilPlayerPresentationController::IsPresentationOverlapped()
    {
    return iPresentationOverlapped;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::SetPresentationOverlapped
// Sets flag to indicate that the presentation is overlapped.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::SetPresentationOverlapped( TBool aOverlapped )
    {
    iPresentationOverlapped = aOverlapped;
    return;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::LayoutOverlappedPresentation
// Sets the correct presentation layout, when a dialog (e.g. FindUi)
// is overlapping the presentation.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::LayoutOverlappedPresentation()
    {
    iPresentationControl->SetRect( Rect() );
    LayoutPresentationControl();
    DrawDeferred();
    }


// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::Reset
// Resets the presentation controller internal state.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::Reset()
    {
    delete iPresentation;
    iPresentation = NULL;
    
    delete iMediaFactory;
    iMediaFactory = NULL;
    
    delete iTransitionFactory;
    iTransitionFactory = NULL;
    
    delete iBufferBitmap;
    iBufferBitmap = NULL;
    
    delete iBufferDevice;
    iBufferDevice = NULL;
    
    delete iBufferContext;
    iBufferContext = NULL;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::SwitchStatusPaneL
// Changes statuspane to new one and possibly sets SMIL player own indicator
// for the statuspane. First checks that navigation pane is present on current
// statuspane layout and that it is application owned. These are global settings
// to all statuspane layouts and without these we cannot make any changes
// to navigation pane. If navigation pane is present and it is application owned
// it is first checked that it does not already have our own indicators as top most
// indicators. If this is true remove our own indicators. Then if our own indicators
// are wanted to be set to navigation pane this is done and after this statuspane layout
// is switched. Then last thing is to check that does this layout really contain
// navigation pane. If it does not and we have set our own indicators they are removed.
// NOTE! This is extremely sensitive function and changes to this might have really
// bad effects to visual appearance of statuspane switch. 
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::SwitchStatusPaneL( TInt aNewPaneResourceId, 
                                                           TBool aSetOwnIndicators,
                                                           TInt& aOldPaneResourceId )
    {    
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    
    aOldPaneResourceId = statusPane->CurrentLayoutResId();
    
    CEikStatusPaneBase::TPaneCapabilities naviCapabilities = 
        statusPane->PaneCapabilities( TUid::Uid( EEikStatusPaneUidNavi ) );
    
    if ( naviCapabilities.IsPresent() && 
         naviCapabilities.IsAppOwned() )
        {
        CAknNavigationControlContainer* naviContainer = 
          static_cast<CAknNavigationControlContainer*>( statusPane->ControlL( 
                                                            TUid::Uid( EEikStatusPaneUidNavi ) ) );
                                                                  
        if ( naviContainer->Top() == iIndicatorContainer )
            {
            naviContainer->Pop( iIndicatorContainer );
            iIndicatorController->MakeVisible( EFalse );
            }
            
        if ( aSetOwnIndicators )
            {
            naviContainer->PushL( *iIndicatorContainer );
            naviContainer->MakeVisible( EFalse );
            iIndicatorController->MakeVisible( ETrue );
            }
        
        statusPane->SwitchLayoutL( aNewPaneResourceId );
        
        naviCapabilities = statusPane->PaneCapabilities( TUid::Uid( EEikStatusPaneUidNavi ) );
        
        if ( !naviCapabilities.IsInCurrentLayout() &&
             aSetOwnIndicators )
            {
            naviContainer->Pop( iIndicatorContainer );
            iIndicatorController->MakeVisible( EFalse );
            }
        else if ( aSetOwnIndicators )
            {
            naviContainer->MakeVisible( ETrue );
            iIndicatorController->SetRect( iIndicatorContainer->Rect() );
            iIndicatorController->LayoutIndicators();
            }
            
        statusPane->ApplyCurrentSettingsL();
        statusPane->DrawNow();
        iCoeEnv->WsSession().Flush();
        }
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::ActivateFocusedMediaL
// Sends an activation event to focused media object.
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::ActivateFocusedMediaL()
    {
    iPresentation->SubmitEventL( KActivate, iPresentation->FocusedObject() );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::PresentationTextL
// All texts inside presentation are concatenated into
// one text so that they are sepatated with two paragraph characters from each
// others. 
// ----------------------------------------------------------------------------
//
HBufC* CSmilPlayerPresentationController::PresentationTextL()
    {
    CRendererArray* renderers = iMediaFactory->GetRenderers();

    if ( !renderers )
        {
        return NULL;
        }

    TChar PS( CEditableText::EParagraphDelimiter );  // Unicode paragraph separator

    TInt totalSize = 0;
    CPlainText* textTemp;
    TInt count = renderers->Count();
    
    // Count the total length of all text renderers
    for ( TInt current = 0; current < count ; current++ ) 
        {     
        if ( renderers->At( current )->MediaType() == EMsgMediaText ||
             renderers->At( current )->MediaType() == EMsgMediaXhtml )
            {
            textTemp = static_cast<CSmilTextRenderer*>( renderers->At( current ) )->GetText();
            TInt textSize = textTemp->DocumentLength();
            // + 2 is for two Unicode paragraph separator
            totalSize = totalSize + textSize + 2;
            }
        }

    HBufC* resultHeap = NULL;
    if ( totalSize > 0 )
        {
        // Create buffer for total size of all texts
        resultHeap = HBufC::NewLC( totalSize + 1 );
        HBufC* tempHeap = HBufC::NewLC( totalSize );
        TPtr ptrResultHeap = resultHeap->Des();
        TPtr ptrTempHeap = tempHeap->Des();
        
        for ( TInt current = 0; current < count; current++ )
            {     
            if ( renderers->At( current )->MediaType() == EMsgMediaText ||
                 renderers->At( current )->MediaType() == EMsgMediaXhtml )
                {
                textTemp = static_cast<CSmilTextRenderer*>( renderers->At( current ) )->GetText();
                
                TInt position = textTemp->DocumentLength();

                // Adding 2 paragragh separators and current text renderer's text.
                textTemp->InsertL( position, PS );
                textTemp->InsertL( position, PS );

                textTemp->Extract( ptrTempHeap );
                ptrResultHeap.Append( ptrTempHeap );
                }
            }
        
        CleanupStack::PopAndDestroy( tempHeap );
        CleanupStack::Pop( resultHeap );
        }
        
    return resultHeap;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::PresentationContains
// Performs a check whether presentation contains specified type of renderer.
// ----------------------------------------------------------------------------
//
TBool CSmilPlayerPresentationController::PresentationContains( const TMsgMediaType& aType ) const
    {
    CRendererArray* renderers = iMediaFactory->GetRenderers();
    
    if ( !renderers )
        {
        return EFalse;
        }

    for ( TInt current = 0; current < renderers->Count(); current++ )
        {     
        if ( renderers->At( current )->MediaType() == aType )
            {
            return ETrue;
            }
        }
    return EFalse;
    }


// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::PlayPauseTappedL
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::PlayPauseTappedL()
    {
#ifdef RD_SCALABLE_UI_V2  
    TInt state = iPresentation->State();  
    if  (  state == CSmilPresentation::EPlaying )
        {
        iParent->PauseTappedL();     
        }
    else if ( state == CSmilPresentation::EPaused )
        {
        iParent->PlayTappedL();
        }        
#endif        
    }
  
    
#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------
// CSmilPlayerPresentationController::HandlePointerEventL
// ---------------------------------------------------------
//
void CSmilPlayerPresentationController::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        switch ( aPointerEvent.iType )
            {
            case TPointerEvent::EButton1Up:
            case TPointerEvent::EButton1Down:
                {
                // The check needs to be here as CSmilObject is null on this state
                if ( iPresentation->FocusedObject() )
                    {
                    TRect focusedRect;
                    if ( iPresentation->FocusedObject()->GetRegion() )
                        {
                        focusedRect = iPresentation->FocusedObject()->GetRegion()->GetRectangle();
                        // Returned rect is relative to presentation rect, we need window relative rect
                        focusedRect.Move( iPresentationRect.iTl );
                        }
                    
                    if ( !focusedRect.IsEmpty() &&
                         !focusedRect.Contains( aPointerEvent.iPosition ) )
                        {
                        // First tap on the region
                        // pointerevent is window related, we have to give smilengine presentation related
                        TPointerEvent pointerEvent( aPointerEvent );
                        pointerEvent.iPosition -= iPresentationRect.iTl;
                        iPresentation->HandlePointerEventL( pointerEvent );
                        iParent->DeactivateScrollingL();
                        iTappedObject = NULL;
                        }
                    else if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
                        {
                        iTappedObject = iPresentation->FocusedObject(); 
                        }
                    else if ( aPointerEvent.iType == TPointerEvent::EButton1Up && iTappedObject == iPresentation->FocusedObject() )
                        {
                        iParent->HandleSelectionKeyL();
                        }
                    }
                break;
                }
            default:
                break;
            }
        }
    }  


#else
void CSmilPlayerPresentationController::HandlePointerEventL( const TPointerEvent& /*aPointerEvent*/ )
    {
    }
#endif // RD_SCALABLE_UI_V2

// ---------------------------------------------------------
// CSmilPlayerPresentationController::InitializeDoubleBufferL
//
// Initializes double buffer bitmap, device and context.
// ---------------------------------------------------------
//
void CSmilPlayerPresentationController::InitializeDoubleBufferL()
    {
    iBufferBitmap = new( ELeave ) CWsBitmap();
    
    if ( iEikonEnv->DefaultDisplayMode() < EColor16MU )
        {
        User::LeaveIfError( iBufferBitmap->Create( Size(),  EColor16M ) );
        }
    else
        {
        User::LeaveIfError( iBufferBitmap->Create( Size(), SystemGc().Device()->DisplayMode() ) );
        }
    
    // Sets the twip-size of the bitmap by converting the bitmaps pixel-size from pixels to twips, 
    // using the conversion functions in the specified graphics device map. This is needed to retrieve
    // correct font from FBS server.
    iBufferBitmap->SetSizeInTwips( SystemGc().Device() );
        
    iBufferDevice = CFbsBitmapDevice::NewL( iBufferBitmap );
    User::LeaveIfError( iBufferDevice->CreateContext( iBufferContext ) );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::IsFocused
// ----------------------------------------------------------------------------
//
TBool CSmilPlayerPresentationController::IsFocused( const TMsgMediaType& aMediaType ) const
    {
    TBool result( EFalse );
    CSmilObject* focused = iPresentation->FocusedObject();
    
    if ( focused && focused->IsMedia() )
        {
        CSmilMedia* media = static_cast<CSmilMedia*>( focused );
        if ( media )
        	{
            CSmilMediaRendererBase* renderer = static_cast<CSmilMediaRendererBase*>( media->GetRenderer() );
            
            if ( renderer && 
            	 renderer->MediaType() == aMediaType )
                {
                // Media object type is the same as parameter.
                result = ETrue;
                }        	
        	}
        }
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::ActivateFocusedL
// ----------------------------------------------------------------------------
//
void CSmilPlayerPresentationController::ActivateFocusedL()
    {
    CSmilObject* focused = iPresentation->FocusedObject();
    
    if ( focused )
        {
        CSmilMedia* media = static_cast<CSmilMedia*>( focused );
        CSmilMediaRendererBase* renderer = static_cast<CSmilMediaRendererBase*>( media->GetRenderer() );
        
        SetStatusPaneVisibilityL( ETrue );
        
        TRAPD( error, renderer->ActivateRendererL() );
        
        SetStatusPaneVisibilityL( EFalse );
        
        User::LeaveIfError( error );
        }
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::OwnStatusPaneVisible
// ----------------------------------------------------------------------------
//
TBool CSmilPlayerPresentationController::OwnStatusPaneVisible()
    {
    return iOwnStatusPaneVisible;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerPresentationController::UseWidescreenStatusPane
// ----------------------------------------------------------------------------
TBool CSmilPlayerPresentationController::UseWidescreenStatusPane()
    {
    return Layout_Meta_Data::IsLandscapeOrientation() && AknLayoutUtils::PenEnabled();
    }

// End of File  

