/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains an implementation of CSmilPlayerDialog class.
*
*/



// INCLUDE FILES
#include <AknUtils.h>                //for AknUtils
#include <eikmenub.h>               // for CEikMenuBar
#include <featmgr.h>                // for Feature Manager
#include <StringLoader.h>           // for StringLoader (load and format strings from resources)
#include <aknnotewrappers.h>        // for Avkon Note Wrappers
#include <AknDef.h>                 // for KEikDynamicLayoutVariantSwitch
#include <aknconsts.h>
#include <AknStatuspaneUtils.h>     // for AknStatuspaneUtils

#include <aknappui.h>
#include <aknlayoutscalable_apps.cdl.h>  // for application LAF
#include <aknlayoutscalable_avkon.cdl.h> // for AVKON LAF
#include <data_caging_path_literals.hrh> 

// Find item stuff
#include <FindItemmenu.rsg>         
#include <finditemmenu.h>           
#include <finditemdialog.h>
#include <finditemengine.h> 

// Help stuff
#include <csxhelp/mms.hlp.hrh> 
#include <hlplch.h>

#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>

#include <SmilPlayer.rsg>
#include "SmilPlayerDialog.h"
#include "SmilPlayerPresentationController.h"
#include "SmilPlayerPanic.h"

#ifdef DIALOG_DEBUG
#include "SMilPlayerLogging.h"
#endif

// CONSTANTS
_LIT( KResFileName, "smilplayer.rsc"); // Resource file

//used to show time control one sec longer after stop
const TInt KStopAfterOneSecond = 1000000;   
//used to wait until 'corrupted presentation' note is shown
const TInt KClosePlayerAfter = 0;  

const TUid KUidMmsEditor = {0x100058DF};

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::CSmilPlayerDialog
// C++ constructor. Initializes class member variables.
// ----------------------------------------------------------------------------
//
CSmilPlayerDialog::CSmilPlayerDialog( TBool aPreview, TBool aAudioOn ) :   
    iResLoader( *iCoeEnv ),
    iPresentationState( ESmilPlayerStateNotReady )
    {
    if ( aPreview )
        {
        iPlayerFlags |= EPreview;
        }
    if ( aAudioOn )
        {
        iPlayerFlags |= EAudioOn;
        }
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::ConstructL
// Symbian 2nd phase constructor. Creates only presentation controller since this
// is the only thing that differentiates the DOM and file handle dialog construction
// "methods". All other 2nd phase constrcution is done on CommonConstructL that should
// be called before this function.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::ConstructL( const TDesC& aBaseUrl, 
                                    MMediaFactoryFileInfo* aFileInfo,
                                    CMDXMLDocument* aDOM )
    {
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_ENTERFN( "[SMILPLAYER] Dialog: ConstructL" );
#endif
    if ( !aDOM || !aFileInfo )
        {
        User::Leave( KErrArgument );
        }
        
    iPresentationController = CSmilPlayerPresentationController::NewL( PresentationRect(), 
                                                                       aBaseUrl, 
                                                                       aFileInfo,
                                                                       this , 
                                                                       aDOM,
                                                                       AudioEnabled() );
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_LEAVEFN( "[SMILPLAYER] Dialog: ConstructL" );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::ConstructL
// Symbian 2nd phase constructor. Creates only presentation controller since this
// is the only thing that differentiates the DOM and file handle dialog construction
// "methods". All other 2nd phase constrcution is done on CommonConstructL that should
// be called before this function.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::ConstructL( const TDesC& aBaseUrl, 
                                    MMediaFactoryFileInfo* aFileInfo,
                                    RFile& aFileHandle )
    {
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_ENTERFN( "[SMILPLAYER] Dialog: ConstructL" );
#endif
    if ( aFileHandle.SubSessionHandle() == KNullHandle ||
         !aFileInfo )
        {
        User::Leave( KErrBadHandle );
        }
        
    iPresentationController = CSmilPlayerPresentationController::NewL( PresentationRect(), 
                                                                       aBaseUrl, 
                                                                       aFileInfo,
                                                                       this , 
                                                                       aFileHandle,
                                                                       AudioEnabled() );
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_LEAVEFN( "[SMILPLAYER] Dialog: ConstructL" );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::CommonConstructL
// Common 2nd phase constructor. Should be called before any ConstructL overloads
// are called.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::CommonConstructL()
    {
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_ENTERFN( "[SMILPLAYER] Dialog: CommonConstructL" );
#endif

    // Load dialog's resource file
    TParse fileParse;
    fileParse.Set( KResFileName, &KDC_RESOURCE_FILES_DIR, NULL );    
    TFileName fileName( fileParse.FullName() );
    iResLoader.OpenL( fileName );
    
    // Call the base class' two-phased constructor
    CAknDialog::ConstructL( R_SMILPLAYER_MENUBAR ); 
    
    iCoeEnv->AddForegroundObserverL( *this );

    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        iPlayerFlags |= EHelpSupported;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdSideVolumeKeys ) )
        {
        iPlayerFlags |= EFeatureSideVolumeKeys;
        }
    FeatureManager::UnInitializeLib();

    iInterfaceSelector = CRemConInterfaceSelector::NewL();

    // owned by iInterfaceSelector
    iCoreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector, *this ); 

    iInterfaceSelector->OpenTargetL();

    // Remote control server command repeat timer.
    iRCSTimer = CPeriodic::NewL( EPriorityNormal );

    // Stop/Close timer
    iTimer = CPeriodic::NewL( EPriorityLow );
	
	iFindItemMenu = CFindItemMenu::NewL( ESmilPlayerFindItemMenu );
    
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_LEAVEFN( "[SMILPLAYER] Dialog: CommonConstructL" );
#endif
    }


// ----------------------------------------------------------------------------
// CSmilPlayerDialog::NewL
// Three-phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CSmilPlayerDialog* CSmilPlayerDialog::NewL( CMDXMLDocument* aDOM, 
                                                     MMediaFactoryFileInfo* aFileInfo,
                                                     const TDesC& aBaseUrl, 
                                                     TBool aPreview,
                                                     TBool aAudioOn )
    {
    CSmilPlayerDialog* self = new(ELeave) CSmilPlayerDialog( aPreview, aAudioOn );
    
    CleanupStack::PushL( self );
    self->CommonConstructL();
    self->ConstructL( aBaseUrl, aFileInfo, aDOM );
    CleanupStack::Pop( self );
    
    return self;
    }
                                                 
// ----------------------------------------------------------------------------
// CSmilPlayerDialog::NewL
// Three-phased constructor
// ----------------------------------------------------------------------------
//
EXPORT_C CSmilPlayerDialog* CSmilPlayerDialog::NewL( RFile& aFileHandle,
                                                     MMediaFactoryFileInfo* aFileInfo,
                                                     const TDesC& aBaseUrl, 
                                                     TBool aPreview,
                                                     TBool aAudioOn )
    {
    CSmilPlayerDialog* self = new(ELeave) CSmilPlayerDialog( aPreview, aAudioOn );
    
    CleanupStack::PushL( self );
    self->CommonConstructL();
    self->ConstructL( aBaseUrl, aFileInfo, aFileHandle );
    CleanupStack::Pop( self );
    
    return self;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::~CSmilPlayerDialog
// Destructor
// ----------------------------------------------------------------------------
//
CSmilPlayerDialog::~CSmilPlayerDialog()
    {
    if ( iAvkonAppUi && iAvkonAppUi->Cba() )
        {
        iAvkonAppUi->Cba()->MakeVisible( ETrue );
        }
    
    // Remove from stack
    iCoeEnv->RemoveForegroundObserver( *this );
    
    // Unload the resourcefile
    iResLoader.Close();
    
    delete iTimer;
    delete iRCSTimer;
    
    delete iInterfaceSelector;
    iCoreTarget = NULL; // For LINT. Owned by iInterfaceSelector
    
    delete iFindItemMenu;
    delete iPresentationController;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::ExecuteLD
// Calls CAknDialog's  ExecuteLD() with correct resourceId
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CSmilPlayerDialog::ExecuteLD()
    { 
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_ENTERFN( "[SMILPLAYER] Dialog: ExecuteLD" );
#endif

    return CAknDialog::ExecuteLD( R_SMILPLAYER_DIALOG );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::PresentationStoppedL
// Called by the presentation controller when presentation stop event is received.
// Changes the current state to stopped..
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::PresentationStoppedL()
    {
    ChangeStateL( ESmilPlayerStateStop );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::HandleGainingForeground
// Updates the presentation to screen.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::HandleGainingForeground()
    {
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_ENTERFN( "[SMILPLAYER] Dialog: HandleGainingForeground" );
#endif

    if ( iPresentationController )
        {  
        iPresentationController->DrawNow();
        }
        
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_LEAVEFN( "[SMILPLAYER] Dialog: HandleGainingForeground" );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::HandleLosingForeground
// Pauses the presentation if it is playing.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::HandleLosingForeground()
    {
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_ENTERFN( "[SMILPLAYER] Dialog: HandleLosingForeground" );
#endif

    //Check if player is playing then goes to paused
    if ( iPresentationState == ESmilPlayerStatePlaying )
        {        
        TRAP_IGNORE( ChangeStateL( ESmilPlayerStatePause ) );
        }
        
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_LEAVEFN( "[SMILPLAYER] Dialog: HandleLosingForeground" );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::ProcessCommandL
// Prosesses menu commands
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::ProcessCommandL( TInt aCommandId )
    {
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_ENTERFN( "[SMILPLAYER] Dialog: ProcessCommandL" );
    SMILPLAYERLOGGER_WRITEF( _L("[SMILPLAYER] Dialog: aCommandId=%d"), aCommandId );
#endif

    if ( iFindItemMenu->CommandIsValidL( aCommandId ) )
        {
        CAknDialog::HideMenu();
        DoFindItemL( aCommandId );
        return;
        }
        
    switch ( aCommandId )
        {
        case ESmilPlayerCmdOpenLink:
            {
            CAknDialog::HideMenu();
            
            TBool internalLink( iPresentationController->AnchorIsInternalLink() );
            
            if ( internalLink )
                {
                // Deactivate scrolling before opening the internal link
                DeactivateScrollingL();
                }
            
            iPresentationController->OpenLinkL();

            if ( internalLink )
                {
                // Opening an internal link pauses the presentation to speficied position,
                ChangeStateL( ESmilPlayerStatePlaying );
                }
                
            break;
            }
        case ESmilPlayerCmdPlaySVG:
            {
            CAknDialog::HideMenu();
            iPresentationController->ActivateFocusedL();
            break;
            }
        case ESmilPlayerCmdActivateScroll:
            {
            if ( 
#ifdef RD_SCALABLE_UI_V2
                 (!(iPlayerFlags & EScrollingActivated)) && 
#endif
                ( iPresentationState == ESmilPlayerStatePause ||
                 iPresentationState == ESmilPlayerStateStop ) )
                {
                ActivateScrollingL();
                }
            break;
            }
        case ESmilPlayerCmdDeactivateScroll:
            {
            if ( iPresentationState == ESmilPlayerStatePause ||
                 iPresentationState == ESmilPlayerStateStop )
                {
                DeactivateScrollingL();
                }
            break;
            }
        case ESmilPlayerCmdReplay:
            {
            DoReplayL();
            break;
            }
        case ESmilPlayerCmdPause:
            {
            ChangeStateL( ESmilPlayerStatePause );    
            break;
            }
        case ESmilPlayerCmdResume:
            {
            ChangeStateL( ESmilPlayerStatePlaying );
            break;
            }
        case EAknCmdHelp:
            {
            LaunchHelpL();
            break;
            }
        case EAknCmdExit:
        case EEikCmdExit:
            {
            MEikCommandObserver* commandObserver = 
                            static_cast<MEikCommandObserver*>( iEikonEnv->EikAppUi() );
            commandObserver->ProcessCommandL( EAknCmdExit );
            break;
            }
        default :
            {
            break;
            }
        }
        
    CAknDialog::ProcessCommandL( aCommandId );
    
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_LEAVEFN( "[SMILPLAYER] Dialog: ProcessCommandL" );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::OfferKeyEventL
// Handles key events. This dialog handles only event keys. Uses CAknDialog as
// a default handler. 
// ----------------------------------------------------------------------------
//
TKeyResponse CSmilPlayerDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                TEventCode aType )
    {
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_WRITE_TIMESTAMP( " --------------------------------" );
    SMILPLAYERLOGGER_ENTERFN( "[SMILPLAYER] Dialog: OfferKeyEventL" );
    SMILPLAYERLOGGER_WRITEF( _L("[SMILPLAYER] Dialog: aKeyEvent.iCode=%d"), aKeyEvent.iCode );
    SMILPLAYERLOGGER_WRITEF( _L("[SMILPLAYER] Dialog: aType=%d"), aType );
#endif
    
    TKeyResponse result( EKeyWasNotConsumed );
    
    if ( CAknDialog::MenuShowing() )
        {
        // If a menu is showing offer key events to it.
        result = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
        }
    else if ( aType == EEventKey )
        {
        // Only interested in standard key events
        result = HandleKeyEventL( aKeyEvent, aType );
        }
    
    if ( result == EKeyWasNotConsumed )
        {
        // Default handler
        result = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
        }

#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_WRITEF( _L("[SMILPLAYER] Dialog: result=%d"), result );
    SMILPLAYERLOGGER_LEAVEFN( "[SMILPLAYER] Dialog: OfferKeyEventL" );
#endif

    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::MrccatoCommand
// Handles side volume key events.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::MrccatoCommand( TRemConCoreApiOperationId aOperationId, 
                                        TRemConCoreApiButtonAction aButtonAct )
    {
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_WRITE_TIMESTAMP( " --------------------------------" );
    SMILPLAYERLOGGER_ENTERFN( "[SMILPLAYER] Dialog: MrccatoCommand" );
    SMILPLAYERLOGGER_WRITEF( _L("[SMILPLAYER] Dialog: aOperationId=%d"), aOperationId );
    SMILPLAYERLOGGER_WRITEF( _L("[SMILPLAYER] Dialog: aButtonAct=%d"), aButtonAct );
#endif
    
    TRequestStatus status;
    
    switch ( aOperationId )
        {
        case ERemConCoreApiVolumeUp: //  Volume Up
        case ERemConCoreApiVolumeDown: //  Volume Down
            {
            switch ( aButtonAct )
                {
                case ERemConCoreApiButtonPress:
                    {
                    // This is called after system default repeat time from button press 
                    // if button has not been released
                    if ( iPresentationController )
                        {
                        iPresentationController->HandleRCSOperation( aOperationId );
                        }
                    
                    // Volume key pressed
                    iRCSTimer->Cancel();
                    
                    iRepeatId = aOperationId;
                    iRCSTimer->Start( KAknStandardKeyboardRepeatRate,
                                      KAknStandardKeyboardRepeatRate, 
                                      TCallBack( DoVolumeRepeat, this ) );
                    
                    break;
                    }
                case ERemConCoreApiButtonRelease:
                    {
                    // Volume key released
                    iRCSTimer->Cancel();
                    break;
                    }
                case ERemConCoreApiButtonClick:
                    {
                    // Volume key clicked
                    if ( iPresentationController )
                        {
                        iPresentationController->HandleRCSOperation( aOperationId );
                        }
                    break;
                    }
                default:
                    {
                    // Never hits this
                    break;
                    }
                }
                
            if ( aOperationId == ERemConCoreApiVolumeUp )
                {
                iCoreTarget->VolumeUpResponse( status, KErrNone );
                }
            else
                {
                iCoreTarget->VolumeDownResponse( status, KErrNone );
                }
            
            User::WaitForRequest( status );
            break;
            }
        default :
            {
            iCoreTarget->SendResponse( status, aOperationId, KErrNone );
            
            User::WaitForRequest( status );
            break;
            }
        }

#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_LEAVEFN( "[SMILPLAYER] Dialog: MrccatoCommand" );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::DynInitMenuPaneL
// Initializes menu items
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane )
    {
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_ENTERFN( "[SMILPLAYER] Dialog: DynInitMenuPaneL" );
#endif

    switch ( aMenuId )
        {
        case R_SMILPLAYER_MENU_OPTIONS:
            {
            // in pause state with left softkey "Options"
            aMenuPane->SetItemDimmed( ESmilPlayerCmdOpenLink,
                                      !iPresentationController->AnchorExist() );
            aMenuPane->SetItemDimmed( ESmilPlayerCmdPlaySVG,
                                      !iPresentationController->IsFocused( EMsgMediaSvg ) );
            aMenuPane->SetItemDimmed( ESmilPlayerCmdActivateScroll,
                                      !iPresentationController->ScrollingActivationEnabled() );
            aMenuPane->SetItemDimmed( ESmilPlayerCmdDeactivateScroll, 
                                      !iPresentationController->ScrollingDeactivationEnabled() );
            aMenuPane->SetItemDimmed( ESmilPlayerCmdResume,
                                      iPresentationState == ESmilPlayerStateStop );
            if ( iPresentationController->PresentationContains( EMsgMediaText ) ||
                 iPresentationController->PresentationContains( EMsgMediaXhtml ) )
                {
                iFindItemMenu->DisplayFindItemMenuItemL( *aMenuPane, 
                                                         ESmilPlayerCmdReplay );
                }
            aMenuPane->SetItemDimmed( EAknCmdHelp, !( iPlayerFlags & EHelpSupported ) );
            break;
            }
        case R_FINDITEMMENU_MENU:
            {
            iFindItemMenu->DisplayFindItemCascadeMenuL( *aMenuPane );
            break;
            }
        case R_SMILPLAYER_SELECT_MENUPANE_IN_PLAYING_STATE:
            {
            aMenuPane->SetItemDimmed( ESmilPlayerCmdOpenLink,
                                      !iPresentationController->AnchorExist() );
            aMenuPane->SetItemDimmed( ESmilPlayerCmdActivateScroll,
                                      !iPresentationController->ScrollingActivationEnabled() );
            aMenuPane->SetItemDimmed( ESmilPlayerCmdDeactivateScroll, 
                                      !iPresentationController->ScrollingDeactivationEnabled() );
            aMenuPane->SetItemDimmed( ESmilPlayerCmdPause, 
                                      !iPresentationController->AnchorExist() && 
                                      !iPresentationController->ScrollingActivationEnabled() &&
                                      !iPresentationController->ScrollingDeactivationEnabled() );
            break;
            }
        case R_SMILPLAYER_SELECT_MENUPANE_IN_PAUSE_STATE:
            {
            aMenuPane->SetItemDimmed( ESmilPlayerCmdOpenLink,
                                      !iPresentationController->AnchorExist() );
            aMenuPane->SetItemDimmed( ESmilPlayerCmdPlaySVG,
                                      !iPresentationController->IsFocused( EMsgMediaSvg ) );
            aMenuPane->SetItemDimmed( ESmilPlayerCmdActivateScroll,
                                      !iPresentationController->ScrollingActivationEnabled() );
            aMenuPane->SetItemDimmed( ESmilPlayerCmdDeactivateScroll, 
                                      !iPresentationController->ScrollingDeactivationEnabled() );
            break;
            }
        case R_SMILPLAYER_SELECT_MENUPANE_IN_STOP_STATE:
            {
            aMenuPane->SetItemDimmed( ESmilPlayerCmdOpenLink,
                                      !iPresentationController->AnchorExist() );
            aMenuPane->SetItemDimmed( ESmilPlayerCmdPlaySVG,
                                      !iPresentationController->IsFocused( EMsgMediaSvg ) );
            aMenuPane->SetItemDimmed( ESmilPlayerCmdActivateScroll,
                                      !iPresentationController->ScrollingActivationEnabled() );
            aMenuPane->SetItemDimmed( ESmilPlayerCmdDeactivateScroll, 
                                      !iPresentationController->ScrollingDeactivationEnabled() );
            break;
            }
        default:
            {
            break;
            }
        }
        
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_LEAVEFN( "[SMILPLAYER] Dialog: DynInitMenuPaneL" );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::OkToExitL
// Called by framework when the OK button is pressed
// ----------------------------------------------------------------------------
//
TBool CSmilPlayerDialog::OkToExitL( TInt aButtonId )
    {
    switch ( aButtonId )
        {
        case ESmilPlayerCmdReplay:
        case ESmilPlayerCmdPause:
        case ESmilPlayerCmdResume:
            {
            ProcessCommandL( aButtonId );
            return EFalse;
            }
        case EEikBidOk:
            {
            return EFalse;
            }
        case EAknSoftkeyContextOptions:
            {
            HandleSelectionKeyL();
            return EFalse;
            }
        default:
            {
            TBool value = CAknDialog::OkToExitL( aButtonId );
            if ( value )
                {
                //switch layout
                iPresentationController->PrepareExitL();
                }
            return value;
            }
        }
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::GetHelpContext
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    if ( iPlayerFlags & EHelpSupported )
        {
        aContext.iMajor = KUidMmsEditor;
        aContext.iContext = KMMS_HLP_PRESENTAT_VIEW_VIEWER;
        }
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::ActivateL
// Initializes and activates the dialog. Also updated presentation dialog layout
// if there has been dynamic layout change during dialog construction. This is only
// done if client application does not have "locked" orientation.
// If there is an error on presentation controller initialization error 
// note is shown and dialog is closed.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::ActivateL()
    {
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_ENTERFN( "[SMILPLAYER] Dialog: ActivateL" );
#endif
    
    iAvkonAppUi->Cba()->MakeVisible( EFalse );
    
    if ( !AknStatuspaneUtils::StaconPaneActive() )
        {  
        ButtonGroupContainer().MakeVisible( EFalse );
        }
    
    if ( iPresentationController->InitializeL() )
        {
        if ( iPresentationController->Rect().Size() != PresentationRect().Size() &&
             ( iAvkonAppUiBase->Orientation() == CAknAppUiBase::EAppUiOrientationAutomatic  ||
               iAvkonAppUiBase->Orientation() == CAknAppUiBase::EAppUiOrientationUnspecified ) )
            {
            LayoutPresentationL();
            }
            
        if ( iPresentationState == ESmilPlayerStateNotReady ||
             iPresentationState == ESmilPlayerStateStop )
            {
            ChangeStateL( ESmilPlayerStatePlaying );
            }        
        }
    else
        {
        ButtonGroupContainer().MakeVisible( ETrue );
        
        ShowInformationNoteL( R_SMILPLAYER_UNSUPPORTED_PRESENTATION, ETrue );
        
        iTimer->Cancel(); //just in case
        iTimer->Start( KClosePlayerAfter,
                       KClosePlayerAfter,
                       TCallBack( DoClosePlayer, this ) );
        }    
        
#ifdef DIALOG_DEBUG
    SMILPLAYERLOGGER_LEAVEFN( "[SMILPLAYER] Dialog: ActivateL" );
#endif
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::DoReplayL
// Performs replay operation. Makes internal state change to stopped state and
// then changes state to play.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::DoReplayL()
    {
    DeactivateScrollingL();
    
    iPresentationState = ESmilPlayerStateNotReady;

    iTimer->Cancel();

    iPresentationController->ChangeStateL( CSmilPlayerPresentationController::EStateStop );
    iPresentationState = ESmilPlayerStateStop;

    ChangeStateL( ESmilPlayerStatePlaying );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::ChangeStateL
// Performs necessary steps needed for state change.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::ChangeStateL( TSmilPlayerPresentationStates aState )
    {
    if ( iPresentationState == aState )
        {
        // Prevents needless function calls
        return;
        }
                        
    switch ( aState )
        {
        case ESmilPlayerStatePlaying:
            {
            iTimer->Cancel();
            DeactivateScrollingL();
            
            CEikButtonGroupContainer& cba = ButtonGroupContainer();
            cba.SetCommandSetL( R_SMILPLAYER_SOFTKEYS_PLAYING );            

            if ( CSmilPlayerPresentationController::UseWidescreenStatusPane() )
                {  
                TInt dummy;
                iPresentationController->SwitchStatusPaneL( R_AVKON_WIDESCREEN_PANE_LAYOUT_USUAL_FLAT_NO_SOFTKEYS, 
                                                            ETrue,
                                                            dummy );
                }
            
            if ( !AknStatuspaneUtils::StaconPaneActive() )
                {   
                cba.MakeVisible( EFalse );
                }
            else
                {
                cba.MakeVisible( ETrue );
                }
                        
            iPresentationController->DrawNow();
            
            if ( iPresentationState == ESmilPlayerStatePause )
                {
                iPresentationState = aState;
                iPresentationController->ChangeStateL( CSmilPlayerPresentationController::EStateResume );
                }
            else
                {
                iPresentationState = aState;
                iPresentationController->ChangeStateL( CSmilPlayerPresentationController::EStatePlay );
                }

            break;
            }
        case ESmilPlayerStatePause:
            {
            iPresentationState = aState;
            iPresentationController->ChangeStateL( CSmilPlayerPresentationController::EStatePause );
            
            if ( CSmilPlayerPresentationController::UseWidescreenStatusPane() )
                {  
                TInt dummy;
                iPresentationController->SwitchStatusPaneL( R_AVKON_WIDESCREEN_PANE_LAYOUT_USUAL_FLAT, 
                                                            ETrue,
                                                            dummy );
                }
            
            CEikButtonGroupContainer& cba = ButtonGroupContainer();
            
            if( iPlayerFlags & EPreview )
                {
                cba.SetCommandSetL( R_SMILPLAYER_SOFTKEYS_PAUSE_IN_PREVIEW );
                }
            else
                {
                cba.SetCommandSetL( R_SMILPLAYER_SOFTKEYS_PAUSE_IN_NORMAL );
                }
            
            cba.MakeVisible( ETrue );
            cba.DrawNow();
            break;
            }
        case ESmilPlayerStateStop:
            {
            iPresentationState = aState;
            
            //Used to show timer one second longer, before getting back to beginning
            // of the presentation
            iTimer->Cancel();
            iTimer->Start( KStopAfterOneSecond,
                           KStopAfterOneSecond,
                           TCallBack( DoStopTiming, this ) );
            if ( CSmilPlayerPresentationController::UseWidescreenStatusPane() )
                {  
                TInt dummy;
                iPresentationController->SwitchStatusPaneL( R_AVKON_WIDESCREEN_PANE_LAYOUT_USUAL_FLAT, 
                                                            ETrue,
                                                            dummy );
                }

            CEikButtonGroupContainer& cba = ButtonGroupContainer();
            if( iPlayerFlags & EPreview )
                {
                cba.SetCommandSetL( R_SMILPLAYER_SOFTKEYS_STOP_IN_PREVIEW );
                }
            else
                {
                cba.SetCommandSetL( R_SMILPLAYER_SOFTKEYS_STOP_IN_NORMAL );
                }
    
            cba.MakeVisible( ETrue );                
            cba.DrawNow();
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( ESmilPlayerUnknownState ) );
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::DoStopTiming
// Performs StopTiming function when timer is triggered.
// ----------------------------------------------------------------------------
//
TInt CSmilPlayerDialog::DoStopTiming( TAny* aObject )
    {
    // cast, and call non-static function
    CSmilPlayerDialog* dialog = static_cast<CSmilPlayerDialog*>( aObject );
    dialog->StopTiming();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::DoClosePlayer
// Performs ClosePlayer function when timer is triggered.
// ----------------------------------------------------------------------------
//
TInt CSmilPlayerDialog::DoClosePlayer( TAny* aObject )
    {
    // cast, and call non-static function
    CSmilPlayerDialog* dialog = static_cast<CSmilPlayerDialog*>( aObject );
    dialog->ClosePlayer();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::DoVolumeRepeat
// Performs ClosePlayer function when timer is triggered.
// ----------------------------------------------------------------------------
//
TInt CSmilPlayerDialog::DoVolumeRepeat( TAny* aObject )
    {
    // cast, and call non-static function
    CSmilPlayerDialog* dialog = static_cast<CSmilPlayerDialog*>( aObject );
    dialog->VolumeRepeat();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::StopTiming
// Calls presentation controller to stop time indicator.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::StopTiming()
    {
    iPresentationController->StopTimer();
    iTimer->Cancel();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::ClosePlayer
// Deletes dialog
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::ClosePlayer()
    {
    delete this;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::ClosePlayer
// Deletes dialog
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::VolumeRepeat()
    {
    iPresentationController->HandleRCSOperation( iRepeatId );
    }

// ----------------------------------------------------------------------------
// CSmilPlayer::ShowInformationNoteL
// Shows information note with specified string to the user.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::ShowInformationNoteL( TInt aResourceID, TBool aWaiting )
    {
    HBufC* string = StringLoader::LoadLC( aResourceID, iCoeEnv );
    
    CAknInformationNote* note = new(ELeave) CAknInformationNote( aWaiting );
    note->ExecuteLD( *string );
    
    CleanupStack::PopAndDestroy( string );
    }
    
// ----------------------------------------------------------------------------
// CSmilPlayerDialog::LaunchHelpL
// Launches help.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::LaunchHelpL() const
    {
    if ( iPlayerFlags & EHelpSupported )
        {
        CCoeAppUi* appUi = static_cast<CCoeAppUi*>( iCoeEnv->AppUi() );
        CArrayFix<TCoeHelpContext>* helpContext = appUi->AppHelpContextL();
        
        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
        }
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::DoFindItemL
// Performs find item functionality. First text buffer containing all text included
// on presentation so that each individual text is separated with two paragraph separators
// is retrieved from presentation controller. Then this text buffer is given to the 
// find item dialog that performs find operation and shows result to user. Statuspane
// is changed to "normal" during find item dialog execution so that find item dialog
// is shown correctly. After execution returns from find item dialog statuspane is
// switched to the one used by SMIL player.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::DoFindItemL( TInt aCommandId )
    {
    HBufC* textBuffer = iPresentationController->PresentationTextL();
    if ( textBuffer )
        {
        CleanupStack::PushL( textBuffer );
        
        CFindItemDialog* dialog = CFindItemDialog::NewL( *textBuffer, iFindItemMenu->SearchCase( aCommandId ) );
        CleanupStack::PushL( dialog );
        
        // Set the presentation overlap flag to true.
        iPresentationController->SetPresentationOverlapped( ETrue );
        iPresentationController->SetStatusPaneVisibilityL( ETrue );
        LayoutOverlappedPresentation( );
        
        //need to shed some player-dialog pixels from top. This unique condition
        //occurs because dialog with usual-statuspane overlaps the player-dialog
        //which has a widescreen-usual-flat-statuspane.
        TRect statusPaneWindow;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EStatusPane, statusPaneWindow );
        TRect smilPlayerWindow = PresentationRect();
        smilPlayerWindow.iTl.iY = statusPaneWindow.iBr.iY;
        iPresentationController->SetRect( smilPlayerWindow );
        
        CleanupStack::Pop( dialog );
        
        iAvkonAppUi->Cba()->MakeVisible( ETrue );
        TRAPD(error, dialog->ExecuteLD() );
        iAvkonAppUi->Cba()->MakeVisible( EFalse );
        
        // Reset the presentation overlap flag to false.
        iPresentationController->SetPresentationOverlapped( EFalse );
        iPresentationController->SetStatusPaneVisibilityL( EFalse );
        LayoutOverlappedPresentation( );

        User::LeaveIfError( error );
        
        CleanupStack::PopAndDestroy( textBuffer );
        }
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::LayoutOverlappedPresentation
// Sets the correct player layout, when a dialog (e.g. FindUi)
// is overlapping the presentation.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::LayoutOverlappedPresentation()
    {
    iPresentationController->SetRect( PresentationRect() );
    iPresentationController->LayoutOverlappedPresentation();
    }


// ----------------------------------------------------------------------------
// CSmilPlayerDialog::HandleResourceChange
// Calls LayoutPresentationL to handle dynamic layout switching.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::HandleResourceChange( TInt aType )
	{
	CAknDialog::HandleResourceChange( aType );
	
	if ( aType == KEikDynamicLayoutVariantSwitch )
		{		
	    TRAP_IGNORE( LayoutPresentationL() );
		}
	}

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::LayoutPresentationL
// Sets correct layout for the presentation controller and calls presentation
// control to change it's child control layouts. Also force change dialog state to 
// stopped. 
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::LayoutPresentationL()
    {
    iPresentationState = ESmilPlayerStateNotReady;
    
    iPresentationController->SetRect( PresentationRect() );
    
    if ( iPresentationController->OwnStatusPaneVisible() )
        {
        iPresentationController->SetStatusPaneVisibilityL( EFalse );
        }

    if ( iPresentationController->IsPresentationOverlapped() )
        {
        // reset setups to layout the presentation correctly
        iAvkonAppUi->Cba()->MakeVisible( EFalse );
        iPresentationController->SetPresentationOverlapped( EFalse );        
        LayoutOverlappedPresentation( );
        // set the settings back
        iPresentationController->SetPresentationOverlapped( ETrue );
        }
    
    iPresentationController->LayoutPresentationL();
    
    if ( iPresentationController->IsPresentationOverlapped() )
        {
        LayoutOverlappedPresentation( );
        TRect statusPaneWindow;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EStatusPane, statusPaneWindow );
        TRect smilPlayerWindow = PresentationRect();
        smilPlayerWindow.iTl.iY = statusPaneWindow.iBr.iY;
        iPresentationController->SetRect( smilPlayerWindow );
        iAvkonAppUi->Cba()->MakeVisible( ETrue );
        }

    if ( !iPresentationController->OwnStatusPaneVisible() )
        {
        iPresentationController->SetStatusPaneVisibilityL( ETrue );
        }
    
    ChangeStateL( ESmilPlayerStateStop );
    
    DrawDeferred();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::PresentationRect
// Calculates presentation controller rectangle according to current LAF information.
// ----------------------------------------------------------------------------
//
TRect CSmilPlayerDialog::PresentationRect() const
    {
    TRect appWindowRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EApplicationWindow, appWindowRect );
    
    TAknLayoutRect mainPane;
    
    if ( CSmilPlayerPresentationController::UseWidescreenStatusPane() )
        {
        // select appropriate main_pane variety based on presentation overlap flag
        if ( iPresentationController && iPresentationController->IsPresentationOverlapped() )
            mainPane.LayoutRect( appWindowRect, AknLayoutScalable_Avkon::main_pane( 4 ) );
        else
            mainPane.LayoutRect( appWindowRect, AknLayoutScalable_Avkon::main_pane( 22 ) );
        }
    else if ( AknStatuspaneUtils::StaconPaneActive() )
        {
        mainPane.LayoutRect( appWindowRect, AknLayoutScalable_Avkon::main_pane( 4 ) );   
        }
    else
        {
        mainPane.LayoutRect( appWindowRect, AknLayoutScalable_Avkon::main_pane( 0 ) );   
        }
    
    TAknLayoutRect mainSmilPane;
    mainSmilPane.LayoutRect( mainPane.Rect(), AknLayoutScalable_Apps::main_smil_pane() );
    
    return mainSmilPane.Rect();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::DeactivateScrolling
// Removes scrolling flag and calls presentation control to deactivate scrolling
// if scrolling deactivationg is enabled.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::DeactivateScrollingL()
    {
    if ( iPresentationController->ScrollingDeactivationEnabled() )
        {
        iPlayerFlags &= ~EScrollingActivated;
        iPresentationController->DeactivateScrollingL();
        }
    }
        
// ----------------------------------------------------------------------------
// CSmilPlayerDialog::ActivateScrolling
// Sets scrolling flag and calls presentation control to activate scrolling.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::ActivateScrollingL()
    {
    iPlayerFlags |= EScrollingActivated;
    iPresentationController->ActivateScrollingL();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::HandleKeyEventL
// Performs event key handling. Each key code is mapped to correct operation and
// each operation is performed on separate function. When side volume keys are
// present all the left/right events are consumed without volume change.
// ----------------------------------------------------------------------------
//
TKeyResponse CSmilPlayerDialog::HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse result( EKeyWasNotConsumed );
    
    switch ( aKeyEvent.iCode )
        {
        case EKeyRightArrow:
        case EKeyLeftArrow:
            {
            result = HandleScrollKeyL( aKeyEvent, aType );
            if ( result == EKeyWasNotConsumed )
                {
                if ( iPlayerFlags & EFeatureSideVolumeKeys )
                    {
                    result = EKeyWasConsumed;
                    }
                else
                    {
                    result = HandleVolumeKeyL( aKeyEvent, aType );
                    }
                }
            break;
            }
        case EKeyDownArrow:
        case EKeyUpArrow:
            {
            result = HandleScrollKeyL( aKeyEvent, aType );
            break;
            }
        case EKeyEnter:
        case EKeyOK:
            {
            result = HandleSelectionKeyL();
            break;
            }
        case EKeyApplication0: // apps key 
            {
            // Do this already here to avoid video
            // rendering on top of fast swap window
            HandleLosingForeground();
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
// CSmilPlayerDialog::HandleScrollKeyL
// Performs scrolling operation. Calls presentation controller to handle scroll
// key event.
// ----------------------------------------------------------------------------
//
TKeyResponse CSmilPlayerDialog::HandleScrollKeyL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    return iPresentationController->HandleScrollKeyEventL( aKeyEvent, aType );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::HandleVolumeKeyL
// Performs volume change operation. 
// ----------------------------------------------------------------------------
//
TKeyResponse CSmilPlayerDialog::HandleVolumeKeyL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse result( EKeyWasNotConsumed );
    
    if ( AudioEnabled() )
        {            
        result = iPresentationController->HandleVolumeKeyEventL( aKeyEvent, aType );
        }
        
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::HandleSelectionKeyL
// Performs needed operation when user has pressed selection key.
// ----------------------------------------------------------------------------
//
TKeyResponse CSmilPlayerDialog::HandleSelectionKeyL()
    {
    TKeyResponse result( EKeyWasNotConsumed );
    
    if ( iPresentationState == ESmilPlayerStatePlaying && 
         iPresentationController->ObjectFocused() )
        {
        if ( iPresentationController->ScrollingActivationEnabled() )
            {
            // Selection key pressed on playing state when scrollable object is focused.
            ChangeStateL( ESmilPlayerStatePause );
            ActivateScrollingL();
            }
        else if ( iPresentationController->AnchorExist() &&
                  iPresentationController->AnchorIsInternalLink() )
            {
            // Selection key pressed on playing state when internal link is focused.
            iPresentationController->OpenLinkL();
            }
        else if ( iPresentationController->IsFocused( EMsgMediaSvg ) )
            {
            ChangeStateL( ESmilPlayerStatePause );
            iPresentationController->ActivateFocusedL();
            }
        else
            {
            // Otherwise activate object
            iPresentationController->ActivateFocusedMediaL();
            }
            
        result = EKeyWasConsumed;
        }
    else if ( iPresentationState == ESmilPlayerStatePause )
        {
        // in pause state of the presentation
        ShowMenubarL( R_SMILPLAYER_SELECT_MENUBAR_IN_PAUSE_STATE );
        result = EKeyWasConsumed;
        }
    else if ( iPresentationState == ESmilPlayerStateStop )
        {
        // in stop state of the presentation
        ShowMenubarL( R_SMILPLAYER_SELECT_MENUBAR_IN_STOP_STATE );
        result = EKeyWasConsumed;
        }
    else if ( iPresentationState == ESmilPlayerStateNotReady )
        {
        result = EKeyWasConsumed;
        }
    
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::ShowMenubarL
// Shows given menubar (i.e. context menu) to user and changes the menubar to
// default one after that.
// ----------------------------------------------------------------------------
//
void CSmilPlayerDialog::ShowMenubarL( TInt aResourceId )
    {
    iMenuBar->SetMenuTitleResourceId( aResourceId );
    iMenuBar->SetMenuType(CEikMenuBar::EMenuContext);
    iMenuBar->TryDisplayMenuBarL();
    iMenuBar->SetMenuTitleResourceId( R_SMILPLAYER_MENUBAR );
    iMenuBar->SetMenuType(CEikMenuBar::EMenuOptions);
    }

// ----------------------------------------------------------------------------
// CSmilPlayerDialog::AudioEnabled
// ----------------------------------------------------------------------------
TBool CSmilPlayerDialog::AudioEnabled() const
    {
    return iPlayerFlags & EAudioOn;
    }
    
// ----------------------------------------------------------------------------
// CSmilPlayerDialog::PlayTappedL
// ----------------------------------------------------------------------------
void CSmilPlayerDialog::PlayTappedL()
    {
#ifdef RD_SCALABLE_UI_V2   
    ChangeStateL( ESmilPlayerStatePlaying );
#endif
    }    
    
// ----------------------------------------------------------------------------
// CSmilPlayerDialog::PauseTappedL
// ----------------------------------------------------------------------------
void CSmilPlayerDialog::PauseTappedL()
    {
#ifdef RD_SCALABLE_UI_V2    
    ChangeStateL( ESmilPlayerStatePause );
#endif    
    }        
    

// End of File
