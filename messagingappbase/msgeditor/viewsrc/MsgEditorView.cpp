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
* Description:   MsgEditorView implementation*
*/



// ========== INCLUDE FILES ================================

#include <eikenv.h>                        // for CEikonEnv
#include <eikappui.h>                      // for CEikAppUi
#include <barsread.h>                      // for TResourceReader
#include <eikrted.h>                       // for CEikRichTextEditor
#include <eikscrlb.h>
#include <AknUtils.h>                      // for AknUtils
#include <txtrich.h>                       // for automatic highlight
#include <aknappui.h>
#include <applayout.cdl.h>                  // LAF
#include <aknlayoutscalable_apps.cdl.h>

#include <bautils.h>                        // NearestLanguageFile
#include <data_caging_path_literals.hrh>

#include <aknnavi.h>
#include <aknnavide.h>
#include <AknStatuspaneUtils.h>

#include <AknsBasicBackgroundControlContext.h>  // for skins support    
#include <AknsDrawUtils.h>

#include <MsgEditorAppUi.rsg>
#include <StringLoader.h>

#include "MsgEditorCommon.h"               //
#include "MsgEditorView.h"                 // for CMsgEditorView
#include "MsgEditorObserver.h"             // for MMsgEditorObserver
#include "MsgHeader.h"                     // for CMsgHeader
#include "MsgBody.h"                       // for CMsgBody
#include "MsgExpandableControl.h"          // for MsgExpandableControl
#include "MsgBodyControl.h"                // for MsgBodyControl
#include "MsgAddressControl.h"             // for MsgAddressControl
#include "MsgAttachmentControl.h"          // for MsgAttachmentControl
#include "MsgAddressControlEditor.h"       // for MsgAddressControlEditor
#include "MsgEditorPanic.h"                // for MsgEditor panics
#include "MsgEditorLogging.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

const TInt KMsgControlIndexHeader = 0;
const TInt KMsgControlIndexBody = 1;
const TInt KMsgNumberOfControls = 2;

const TInt KMsgMaximumScrollPartLength = 64;
_LIT( KMsgEditorAppUiResourceFileName, "msgeditorappui.rsc" );

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgEditorView::CMsgEditorView
//
// Constructor.
// ---------------------------------------------------------
//
CMsgEditorView::CMsgEditorView( MMsgEditorObserver& aObserver,
                                TUint32 aEditorModeFlags ): 
    iEditorObserver( aObserver ),
    iCurrentFocus( EMsgNoneFocused ),
    iEditorModeFlags( aEditorModeFlags ),
    iLineHeight( MsgEditorCommons::MsgBaseLineDelta() ),
    iBaseLineOffset( MsgEditorCommons::MsgBaseLineOffset() ),
    iResourceLoader( *iCoeEnv )
    {
    }

// ---------------------------------------------------------
// CMsgEditorView::ConstructL
//
// Constructs the editor view by creating the window, the header and the body.
// ---------------------------------------------------------
//
void CMsgEditorView::ConstructL()
    { 
    CreateWindowL();
    
    if ( AknStatuspaneUtils::FlatLayoutActive() )
        {
        CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
        if ( statusPane )
            {
            CAknNavigationControlContainer* naviPane = 
                static_cast<CAknNavigationControlContainer*>( statusPane->ControlL( 
                                                              TUid::Uid( EEikStatusPaneUidNavi ) ) );
            
            if ( naviPane )
                {
                naviPane->SetPreferredNaviDecoratorLayoutStyle( 
                                CAknNavigationDecorator::ENaviControlLayoutNormal );
                }
            }
        }
    
    iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMainMessage, 
                                                           Rect(), 
                                                           EFalse );

    // T9: Add control to stack to make FEP work. View does not accept key
    //     events from CT9FepControl::HandleChangeInFocus(). Key events are
    //     handled first by editor app ui which routes them to view.
    iEikonEnv->EikAppUi()->AddToStackL( this, 
                                        ECoeStackPriorityDefault, 
                                        ECoeStackFlagRefusesAllKeys );

    Window().SetShadowDisabled( ETrue );
    
    // Disables automatic background color drawing
    // on window server.
    Window().SetBackgroundColor();    
    
    CreateScrollBarL();
    CreateHeaderL();
    CreateBodyL();
    }

// ---------------------------------------------------------
// CMsgEditorView::~CMsgEditorView
//
// Destructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgEditorView::~CMsgEditorView()
    {
    iEikonEnv->EikAppUi()->RemoveFromStack( this );

    delete iHeader;
    delete iBody;
    delete iScrollBar;
    delete iBgContext;
    
    iResourceLoader.Close();
           
    delete iScrollPopText;
    }

// ---------------------------------------------------------
// CMsgEditorView::NewL
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CMsgEditorView* CMsgEditorView::NewL( MMsgEditorObserver& aObserver,
                                               TUint32 aEditorModeFlags )
    {
    CMsgEditorView* self = new( ELeave ) CMsgEditorView( aObserver, aEditorModeFlags );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CMsgEditorView::MopNext
//
// From MObjectProvider.
// ---------------------------------------------------------
//
EXPORT_C MObjectProvider* CMsgEditorView::MopNext()
    {
    return iEikonEnv->EikAppUi();
    }

// ---------------------------------------------------------
// CMsgEditorView::ExecuteL
//
// Prepares the editor view for showing it on the screen.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorView::ExecuteL( const TRect& aRect,
                                        TInt aControlIdForFocus )
    {
    TRect rect( aRect );
    rect.iBr.iY = rect.iTl.iY + MsgEditorCommons::EditorViewHeigth();

    AdjustComponentDistances();

    SetViewRect( rect );
    iSize = rect.Size();
    
    // Set initial size for controls.
    TSize editorSize( rect.Size() );
    SetAndGetSizeL( editorSize, ETrue ); 
    
    // Enable here so that size affecting events are processed 
    // from header & body controls after this point.
    iStateFlags |= EMsgStateInitializing;
    
    SizeChanged();
    ActivateL();
    
    iScrollParts = 1;

    iEditorObserver.EditorObserver( MMsgEditorObserver::EMsgScrollParts,
                                    &iScrollParts,
                                    NULL,
                                    NULL );
    
    if ( aControlIdForFocus != EMsgComponentIdNull )
        {
        SetFocusByControlIdL( aControlIdForFocus );
        }

    if ( aControlIdForFocus == EMsgComponentIdNull ||
         iCurrentFocus == EMsgNoneFocused )
        {
        iCurrentFocus = EMsgHeaderFocused;
        
        TInt newFocus = iHeader->FirstFocusableControl( 0, EMsgFocusDown );
        if ( newFocus == KErrNotFound )
            {
            iCurrentFocus = EMsgBodyFocused;
            newFocus = iBody->FirstFocusableControl( 0, EMsgFocusDown );
            
            if ( newFocus == KErrNotFound )
                {
                iCurrentFocus = EMsgNoneFocused;
                }
            else
                {
                iBody->ChangeFocusTo( newFocus, ENoDrawNow );
                }
            }
        else
            {
            iHeader->ChangeFocusTo( newFocus, ENoDrawNow );
            }
        }

    UpdateScrollBarL();

    SetComponentsInitialized();
    
    iStateFlags &= ~EMsgStateInitializing;
    iStateFlags |= EMsgStateInitialized;
    
    // Has to be called after EMsgStateInitialized flag is set on since
    // otherwise control that have suppress text formatting are not 
    // resized correctly.
    NotifyControlsForEvent( EMsgViewEventPrepareForViewing, 0 );
    
    DrawNow();
    iCoeEnv->WsSession().Flush();
    }

// ---------------------------------------------------------
// CMsgEditorView::ControlById
//
// Finds a control from the header and the body by id and returns a pointer to it.
// ---------------------------------------------------------
//
EXPORT_C CMsgBaseControl* CMsgEditorView::ControlById( TInt aControlId ) const
    {
    __ASSERT_DEBUG( iHeader != NULL, Panic( EMsgHeaderNotExists ) );
    __ASSERT_DEBUG( iBody != NULL, Panic( EMsgBodyNotExists ) );

    CMsgBaseControl* component = iHeader->Component( aControlId );

    if ( !component )
        {
        component = iBody->Component( aControlId );
        }

    return component;
    }

// ---------------------------------------------------------
// CMsgEditorView::FocusedControl
//
// Returns a pointer to a focused control. If no control is focused, returns NULL.
// ---------------------------------------------------------
//
EXPORT_C CMsgBaseControl* CMsgEditorView::FocusedControl() const
    {
    __ASSERT_DEBUG( iHeader != NULL, Panic( EMsgHeaderNotExists ) );
    __ASSERT_DEBUG( iBody != NULL, Panic( EMsgBodyNotExists ) );

    if ( iCurrentFocus == EMsgHeaderFocused )
        {
        return iHeader->FocusedControl();
        }
    else if ( iCurrentFocus == EMsgBodyFocused )
        {
        return iBody->FocusedControl();
        }
    else
        {
        return NULL;
        }
    }

// ---------------------------------------------------------
// CMsgEditorView::SetFocus
//
// Sets focus to a control aControlId.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorView::SetFocus( TInt aControlId )
    {
    __ASSERT_DEBUG( iStateFlags & EMsgStateInitialized,
                    Panic( EMsgFunctionCalledBeforeInitialization ) );

    TRAP_IGNORE( 
        {
        SetFocusByControlIdL( aControlId );
        UpdateScrollBarL();
        } );
    }

// ---------------------------------------------------------
// CMsgEditorView::AddControlFromResourceL
//
// Constructs a control of type aControlType from resource and adds it to
// a form component aFormComponent. The control is added to a position aIndex.
//
// Leaves with KErrNotFound if aControlType is incorrect.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgEditorView::AddControlFromResourceL( TInt aResourceId,
                                                       TInt aControlType,
                                                       TInt aIndex,
                                                       TMsgFormComponent aFormComponent )
    {
    CMsgBaseControl* control = NULL;

    switch (aControlType)
        {
        case EMsgExpandableControl:
            {
            control = new ( ELeave ) CMsgExpandableControl( *this );
            break;
            }
        case EMsgAddressControl:
            {
            control = new ( ELeave ) CMsgAddressControl( *this );
            break;
            }
        case EMsgAttachmentControl:
            {
            control = new ( ELeave ) CMsgAttachmentControl( *this, *this );
            break;
            }
        default:
            {
            control = iEditorObserver.CreateCustomControlL( aControlType );
            if ( control == NULL )
                {
                User::Leave( KErrNotSupported );
                }
            break;
            }
        }

    CleanupStack::PushL( control );
    control->SetControlType( aControlType );
    control->SetMopParent( this );

    control->ConstructFromResourceL( aResourceId );

    TInt controlId = control->ControlId();
    DoAddControlL( control, controlId, aIndex, aFormComponent );
    CleanupStack::Pop();  //control

    return controlId;
    }

// ---------------------------------------------------------
// CMsgEditorView::AddControlL
//
// Adds a control given by aControl to a form component aFormComponent.
// The control is added to a position aIndex with control id aControlId.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorView::AddControlL( CMsgBaseControl* aControl,
                                           TInt aControlId,
                                           TInt aIndex,
                                           TMsgFormComponent aFormComponent )
    {
    DoAddControlL( aControl, aControlId, aIndex, aFormComponent );
    }

// ---------------------------------------------------------
// CMsgEditorView::RemoveControlL
//
// Removes the control aControlId from the header or the body and returns pointer
// to it. If a control cannot be found, returns NULL.
// ---------------------------------------------------------
//
EXPORT_C CMsgBaseControl* CMsgEditorView::RemoveControlL( TInt aControlId )
    {
    __ASSERT_DEBUG( iHeader != NULL, Panic( EMsgHeaderNotExists ) );
    __ASSERT_DEBUG( iBody != NULL, Panic( EMsgBodyNotExists ) );

    if ( iStateFlags & EMsgStateInitialized ) // Control is removed on run-time.
        {
        if ( FocusedControl() == ControlById( aControlId ) &&
             iHeader->CountMsgControls() + iBody->CountMsgControls() > 1 )
            {
            // Try to move focus to different control as focused control is removed and
            // there is more than one control loaded.
            MMsgEditorObserver::TMsgFocusEvent focusEvent = MMsgEditorObserver::EMsgFocusNone;

            if ( !RotateFocusL( EMsgFocusUp, focusEvent ) )
                {
                if ( !RotateFocusL( EMsgFocusDown, focusEvent ) )
                    {
                    //__ASSERT_DEBUG(EFalse, Panic(EMsgFocusLost));
                    // There are no focusing component left. Remove the focus from current control.
                    FocusedControl()->SetFocus( EFalse );
                    }
                }
            
            iCoeEnv->SyncNotifyFocusObserversOfChangeInFocus();
            }
        }

    // Remove a component from the header.
    CMsgBaseControl* control = iHeader->RemoveControlL( aControlId );

    if ( control == NULL )
        {
        // If a component could not be found in the header, remove it from the body.
        control = iBody->RemoveControlL( aControlId );
        }

    if ( iStateFlags & EMsgStateInitialized ) // Control is removed on run-time.
        {
        if ( control != NULL )
            {
            CleanupStack::PushL( control );
            RefreshViewL();
            EnsureCorrectViewPosition();
            CleanupStack::Pop();  // control
            }
        }
    else if ( iHeader->CountMsgControls() == 0 )
        {
        iFormOffset = 0;
        }
    
    if ( control )
        {
        // This is needed atleast for japanese pictographs to not
        // draw into current control area.
        control->MakeVisible( EFalse );
        }
    
    return control;
    }

// ---------------------------------------------------------
// CMsgEditorView::DeleteControlL
//
// Removes the control aControlId from the header or the body and returns pointer
// to it. If a control cannot be found, returns NULL.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorView::DeleteControlL( TInt aControlId )
    {
    CMsgBaseControl* baseControl = RemoveControlL( aControlId );
    delete baseControl;
    }

// ---------------------------------------------------------
// CMsgEditorView::FormComponent
//
// Returns a reference to a form component.
// ---------------------------------------------------------
//
EXPORT_C CCoeControl& CMsgEditorView::FormComponent( TMsgFormComponent aFormComponent ) const
    {
    __ASSERT_DEBUG( iHeader != NULL, Panic( EMsgHeaderNotExists ) );
    __ASSERT_DEBUG( iBody != NULL, Panic( EMsgBodyNotExists ) );

    switch ( aFormComponent )
        {
        case EMsgHeader:
            {
            return *iHeader;
            }
        case EMsgBody:
            {
            return *iBody;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EMsgIncorrectFormComponent ) );
            // Just to make compiler happy.
            return *iBody;
            }
        }
    }

// ---------------------------------------------------------
// CMsgEditorView::HandleScreenSizeChangeL
//
// Prepares the editor view for viewing it on the screen after screen size change.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorView::HandleScreenSizeChangeL( const TRect& aRect )
    {
    __ASSERT_DEBUG( iStateFlags & EMsgStateInitialized,
        Panic( EMsgFunctionCalledBeforeInitialization ) );

    SetViewRect( aRect );
    RefreshViewL();
    }


// ---------------------------------------------------------
// CMsgEditorView::ResetControls
//
// Reset all controls.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorView::ResetControls()
    {
    __ASSERT_DEBUG( iHeader != NULL, Panic( EMsgHeaderNotExists ) );
    __ASSERT_DEBUG( iBody != NULL, Panic( EMsgBodyNotExists ) );

    iHeader->ResetControls();
    iBody->ResetControls();
    }

// ---------------------------------------------------------
// CMsgEditorView::ResetControls
//
// Reset controls from header or body.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorView::ResetControls( TMsgFormComponent aFormComponent )
    {
    __ASSERT_DEBUG( iHeader != NULL, Panic( EMsgHeaderNotExists ) );
    __ASSERT_DEBUG( iBody != NULL, Panic( EMsgBodyNotExists ) );

    switch ( aFormComponent )
        {
        case EMsgHeader:
            {
            iHeader->ResetControls();
            break;
            }
        case EMsgBody:
            {
            iBody->ResetControls();
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EMsgIncorrectFormComponent ) );
            }
        }
    }

// ---------------------------------------------------------
// CMsgEditorView::IsAnyControlModified
//
// Checks if any control owned by view is modified.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgEditorView::IsAnyControlModified() const
    {
    TInt i;
    CMsgBaseControl* ctrl;
    TInt countHeader = iHeader->CountMsgControls();
    TInt countBody = iBody->CountMsgControls();

    for ( i = 0; i < countHeader; i++ )
        {
        ctrl = iHeader->MsgControl( i );
        if ( ctrl && ctrl->IsModified() )
            {
            return ETrue;
            }
        }

    for ( i = 0; i < countBody; i++ )
        {
        ctrl = iBody->MsgControl( i );
        if ( ctrl && ctrl->IsModified() )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// ---------------------------------------------------------
// CMsgEditorView::SetControlsModified
//
// Updates modified flag of controls owned by view.
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorView::SetControlsModified( TBool aFlag )
    {
    TInt i;
    CMsgBaseControl* ctrl;
    TInt countHeader = iHeader->CountMsgControls();
    TInt countBody = iBody->CountMsgControls();

    for ( i = 0; i < countHeader; i++ )
        {
        ctrl = iHeader->MsgControl( i );
        if ( ctrl )
            {
            ctrl->SetModified( aFlag );
            }
        }

    for ( i = 0; i < countBody; i++ )
        {
        ctrl = iBody->MsgControl( i );
        if ( ctrl )
            {
            ctrl->SetModified( aFlag );
            }
        }
    }

// ---------------------------------------------------------
// CMsgEditorView::OfferKeyEventL
//
// Handles key events.
// ---------------------------------------------------------
//
EXPORT_C TKeyResponse CMsgEditorView::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                      TEventCode aType )
    {
    CMsgBaseControl* ctrl = FocusedControl();

    // Is this needed?
    if ( ctrl && ctrl->ControlType() == EMsgAddressControl && aType == EEventKeyUp )
        {
        return ctrl->OfferKeyEventL( aKeyEvent, aType );
        }    
    
    if ( ( !( iStateFlags & EMsgStateInitialized ) ) || ( aType != EEventKey ) )
        {
        return EKeyWasNotConsumed;
        }

    TKeyResponse keyResp( EKeyWasNotConsumed );
    TBool focusRotated = EFalse;
    TBool forceScrollUp = EFalse;
    MMsgEditorObserver::TMsgFocusEvent focusEvent = MMsgEditorObserver::EMsgFocusNone;

    switch ( aKeyEvent.iCode )
        {
        case EKeyUpArrow:
            {
            if ( ctrl )
                {
                // No focus change allowed if selection is ongoing.
                if ( !( aKeyEvent.iModifiers & EModifierShift ) &&
                     ctrl->IsFocusChangePossible( EMsgFocusUp ) )
                    {
                    if ( RotateFocusL( EMsgFocusUp, focusEvent ) )
                        {
                        focusRotated = ETrue;
                        keyResp = EKeyWasConsumed; // focus changed.
                        }
                    else
                        {
                        // the cursor is in the topmost control.
                        TInt delta = -iFormOffset;
                        ScrollForm( delta, ETrue );
                        
                        keyResp = EKeyWasNotConsumed;
                        }
                    }
                }
            else
                {
                // no focused control: set focus event if many parts.
                if ( iScrollParts > 1 )
                    {
                    focusEvent = MMsgEditorObserver::EMsgFocusAtTop;
                    }
                }
            break;
            }
        case EKeyDownArrow:
            {
            if ( ctrl )
                {
                // No focus change allowed if selection is ongoing.
                if ( !( aKeyEvent.iModifiers & EModifierShift ) &&
                     ctrl->IsFocusChangePossible( EMsgFocusDown ) )
                    {
                    if ( RotateFocusL( EMsgFocusDown, focusEvent ) )
                        {
                        focusRotated = ETrue;
                        keyResp = EKeyWasConsumed;
                        }
                    else
                        {
                        keyResp = EKeyWasNotConsumed;
                        }
                    }
                }
            else
                {
                // no focused control: set focus event if many parts.
                if ( iScrollParts > 1 )
                    {
                    focusEvent = MMsgEditorObserver::EMsgFocusAtBottom;
                    }
                }
            break;
            }
        default:
            {
            break;
            }
        }

    if ( ctrl && keyResp == EKeyWasNotConsumed )
        {
        keyResp = ctrl->OfferKeyEventL( aKeyEvent, aType );
        }

    if ( focusEvent != MMsgEditorObserver::EMsgFocusNone )
        {
        MMsgEditorObserver::TMsgAfterFocusEventFunc after =
            MMsgEditorObserver::EMsgAfterFocusNone;

#ifdef RD_SCALABLE_UI_V2
        if ( focusEvent == MMsgEditorObserver::EMsgFocusAtTop )
            {
            iVisiblePart--;
            }
        else if ( focusEvent == MMsgEditorObserver::EMsgFocusAtBottom )
            {
            iVisiblePart++;
            }
        
        iVisiblePart = Max( 0, Min( iScrollParts - 1, iVisiblePart ) );
#endif
        iEditorObserver.EditorObserver( MMsgEditorObserver::EMsgHandleFocusChange,
                                        &focusEvent,
                                        &after,
                                        &iVisiblePart );
        
        if ( after != MMsgEditorObserver::EMsgAfterFocusNone )
            {
            forceScrollUp = SetAfterFocusL( after );
            }
        }
    
    EnsureCorrectFormPosition( ( aKeyEvent.iCode == EKeyDownArrow ) && focusRotated, forceScrollUp );

    UpdateScrollBarL();

    return keyResp;
    }

// ---------------------------------------------------------
// CMsgEditorView::HandlePointerEventL
//
// Handles pointer event processing and propagation.
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CMsgEditorView::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        TBool handled( EFalse );

	if ( IsFocused() )
	{           
        TPointerEvent pointerEvent( aPointerEvent );
            
        iEditorObserver.EditorObserver( MMsgEditorObserver::EMsgControlPointerEvent, 
            ControlFromPosition( aPointerEvent.iPosition, ETrue ),
            &pointerEvent, 
            &handled );
	}

        if ( !handled )
            {
            CCoeControl::HandlePointerEventL( aPointerEvent );
            }
        }
    }
#else
void CMsgEditorView::HandlePointerEventL( const TPointerEvent& /*aPointerEvent*/ )
    {
    }
#endif // RD_SCALABLE_UI_V2


// ---------------------------------------------------------
// CMsgEditorView::ViewInitialized
//
// Returns whether view is initialized.
// ---------------------------------------------------------
//
TBool CMsgEditorView::ViewInitialized() const
    {
    return iStateFlags & EMsgStateInitialized;
    }

// ---------------------------------------------------------
// CMsgEditorView::ViewRect
//
// Returns the current view rectangle.
// ---------------------------------------------------------
//
TRect CMsgEditorView::ViewRect() const
    {
    return iViewRect;
    }

// ---------------------------------------------------------
// CMsgEditorView::HandleBaseControlEventRequestL
//
// Handles an event from a control.
// ---------------------------------------------------------
//
TBool CMsgEditorView::HandleBaseControlEventRequestL( CMsgBaseControl* aControl,
                                                      TMsgControlEventRequest aRequest,
                                                      TInt aDelta )
    {
    if ( !( iStateFlags & EMsgStateInitialized ) )
        {
        // Allow processing of possible height & position changing
        // events when view is currently being initialized.
        if ( !( iStateFlags & EMsgStateInitializing ) ||
             !( aRequest == EMsgEnsureCorrectFormPosition ||
                aRequest == EMsgHeightChanged ) )
            {
            return ETrue;
            }
        }

    TBool done( EFalse );
    MMsgEditorObserver::TMsgFocusEvent focusEvent = MMsgEditorObserver::EMsgFocusNone;

    switch ( aRequest )
        {
        case EMsgCheckIfFocusIsAboveThis:
            {
            TBool isAbove = EFalse;
            TInt componentIndex = iHeader->ComponentIndexFromId( aControl->ControlId() );
            if ( componentIndex != KErrNotFound )
                {
                // control found in header.
                isAbove = iCurrentFocus == EMsgHeaderFocused
                    ? iHeader->CurrentFocus() < componentIndex
                    : EFalse;
                }
            else
                {
                // control found in body.
                componentIndex = iBody->ComponentIndexFromId( aControl->ControlId() );
                isAbove = iCurrentFocus == EMsgBodyFocused
                    ? iBody->CurrentFocus() < componentIndex
                    : ETrue;
                }
            return isAbove;
            }

        case EMsgEnsureCorrectFormPosition:
            {
            if ( iStateFlags & EMsgEnsureCorrectFormPositionRequestIssued )
                {
                // prevents recursion.
                return EFalse;
                }
            break;
            }
        case EMsgRotateFocusUp:
            {
            done = RotateFocusL( EMsgFocusUp, focusEvent );
            break;
            }
        case EMsgRotateFocusDown:
            {
            done = RotateFocusL( EMsgFocusDown, focusEvent );
            break;
            }
        case EMsgHeightChanged:
            {
            if ( iStateFlags & EMsgStateRefreshing )
                {
                done = ETrue;
                }
            else if ( aControl &&
                      aControl->ControlModeFlags() & EMsgControlModeBodyMaxHeight )
                {
                // Special handling is needed when maximum height body control's height is changed.
                // Currently this is happening when control is reseted. Special handling is
                // needed as last line of body control's text should be shown on the
                // last visible editor line if editor total height permits it. If editor total
                // does not permit this then editor should show it's topmost line.
                TInt bottomYPos( aControl->Position().iY + aControl->VirtualHeight() );
                TInt distanceFromBottom( Rect().iBr.iY - bottomYPos );
    
                if ( distanceFromBottom > 0 )
                    {
                    TInt delta = Abs( iFormOffset ) < distanceFromBottom ? -iFormOffset :
                                                                           distanceFromBottom;
                    
                    if ( delta )
                        {
                        ScrollForm( delta, ETrue );
                        }
                    }
                
                if ( ViewInitialized() )
                    {
                    UpdateScrollBarL();
                    }
                
                done = ETrue;
                }
            else
                {
                done = HandleHeightChangedL( aControl, aDelta );
                
                if ( ViewInitialized() )
                    {
                    UpdateScrollBarL();
                    }
                }
            return done;
            }
        case EMsgScrollForm:
            {
            done = ScrollForm( aDelta, ETrue );
            break;
            }
        case EMsgUpdateScrollbar:
            {
            UpdateScrollBarL();
            return ETrue;
            }
        default:
            {
            break;
            }
        }

    done = EnsureCorrectFormPosition( EFalse );
    
    if ( iStateFlags & EMsgStateRefreshing ||
         iStateFlags & EMsgStateInitializing )
        {
        return done;
        }

    UpdateScrollBarL();

    return done;
    }

// ---------------------------------------------------------
// CMsgEditorView::HandleBaseControlEventRequestL
//
// Handles an event from a control.
// ---------------------------------------------------------
//
TBool CMsgEditorView::HandleBaseControlEventRequestL( CMsgBaseControl* aControl,
                                                      TMsgControlEventRequest aRequest )
    {
    return HandleBaseControlEventRequestL( aControl, aRequest, 0 );
    }

// ---------------------------------------------------------
// CMsgEditorView::HandleEditObserverEventRequestL
//
// Handles an event from a control.
// ---------------------------------------------------------
//
TInt CMsgEditorView::HandleEditObserverEventRequestL( const CCoeControl* /*aControl*/,
                                                      TMsgControlEventRequest aRequest,
                                                      TAny* aArg1,
                                                      TAny* aArg2,
                                                      TAny* aArg3 )
    {
    switch ( aRequest )
        {
        case EMsgDenyCut:
            {
            CEikEdwin::TClipboardFunc cb = CEikEdwin::ECut;
            TInt arg = 0;

            iEditorObserver.EditorObserver( MMsgEditorObserver::EMsgDenyClipboardOperation,
                                            &cb,
                                            &arg,
                                            NULL );
            return arg;
            }
        case EMsgDenyCopy:
            {
            CEikEdwin::TClipboardFunc cb = CEikEdwin::ECopy;
            TInt arg = 0;

            iEditorObserver.EditorObserver( MMsgEditorObserver::EMsgDenyClipboardOperation,
                                            &cb,
                                            &arg,
                                            NULL );
            return arg;
            }
        case EMsgDenyPaste:
            {
            CEikEdwin::TClipboardFunc cb = CEikEdwin::EPaste;
            TInt arg = 0;

            iEditorObserver.EditorObserver( MMsgEditorObserver::EMsgDenyClipboardOperation,
                                            &cb,
                                            &arg,
                                            NULL );
            return arg;
            }
        case EMsgStateFlags:
            {
            return iStateFlags;
            }
        case EMsgGetNaviIndicators:
            {
            iEditorObserver.EditorObserver( MMsgEditorObserver::EMsgGetNaviIndicators,
                                            aArg1,
                                            aArg2,
                                            aArg3 );
            return 0;
            }
        case EMsgButtonPressed:
            {
            iEditorObserver.EditorObserver( MMsgEditorObserver::EMsgButtonEvent,
                                            aArg1,
                                            aArg2,
                                            aArg3 );
            return 0;
            }
        default:
            {
            break;
            }
        }
    return 0;
    }

// ---------------------------------------------------------
// CMsgEditorView::CountComponentControls
//
// Returns a number of controls.
// ---------------------------------------------------------
//
TInt CMsgEditorView::CountComponentControls() const
    {
    TInt countScrollBarComponents( 0 );
    
    if ( iScrollBar )
        {
        countScrollBarComponents = iScrollBar->CountComponentControls();
        }
        
    return KMsgNumberOfControls + countScrollBarComponents;
    }

// ---------------------------------------------------------
// CMsgEditorView::ComponentControl
//
// Returns a control of index aIndex.
// ---------------------------------------------------------
//
CCoeControl* CMsgEditorView::ComponentControl( TInt aIndex ) const
    {
    __ASSERT_DEBUG( iHeader != NULL, Panic( EMsgHeaderNotExists ) );
    __ASSERT_DEBUG( iBody != NULL, Panic( EMsgBodyNotExists ) );

    TInt countScrollBarComponents( 0 );
    
    if ( iScrollBar )
        {
        countScrollBarComponents = iScrollBar->CountComponentControls();
        }
        
    switch ( aIndex )
        {
        case KMsgControlIndexHeader:
            {
            return iHeader;
            }
        case KMsgControlIndexBody:
            {
            return iBody;
            }
        default:
            {
            if ( iScrollBar &&  
                 aIndex >= KMsgNumberOfControls &&  
                 aIndex < countScrollBarComponents + KMsgNumberOfControls )
                {
                return iScrollBar->ComponentControl( aIndex - KMsgNumberOfControls );
                }
            else
                {
                __ASSERT_DEBUG( EFalse, Panic( EMsgIncorrectComponentIndex ) );
                }
            return NULL;
            }
        }
    }

// ---------------------------------------------------------
// CMsgEditorView::SizeChanged
//
// Sets new position for all the controls.
// ---------------------------------------------------------
//
void CMsgEditorView::SizeChanged()
    { 
    MEBLOGGER_ENTERFN( "CMsgEditorView::SizeChanged" );

    if ( iBgContext )
        {
        iBgContext->SetRect( Rect() );
        iBgContext->SetParentPos( PositionRelativeToScreen() );
        }

    __ASSERT_DEBUG( iHeader != NULL, Panic( EMsgHeaderNotExists ) );
    __ASSERT_DEBUG( iBody != NULL, Panic( EMsgBodyNotExists ) );

    TPoint bodyPosition( MsgEditorCommons::MsgBodyPane().iTl.iX, 0 );
    TPoint headerPosition( MsgEditorCommons::MsgHeaderPane().iTl.iX, iFormOffset );

    //MEBLOGGER_WRITEF(_L("MEB: CMsgEditorView::SizeChanged: header height %d "), iHeader->Size().iHeight);
    //MEBLOGGER_WRITEF(_L("MEB: CMsgEditorView::SizeChanged: body   height %d "), iBody->Size().iHeight);

    iHeader->SetExtent( headerPosition, iHeader->Size() );
    bodyPosition.iY = iHeader->Rect().iBr.iY /* + 1 */;
    iBody->SetExtent( bodyPosition, iBody->Size() );

    MEBLOGGER_LEAVEFN( "CMsgEditorView::SizeChanged" );
    }

// ---------------------------------------------------------
// CMsgEditorView::FocusChanged
//
// Sets focus off or on from focused control. This is called
// when e.g. options menu is shown. Removed e.g. cursor from
// text fields.
// ---------------------------------------------------------
//
void CMsgEditorView::FocusChanged( TDrawNow /*aDrawNow*/ )
    {
    if ( iHeader && iBody )
        {
        CMsgBaseControl* ctrl = FocusedControl();

        if ( ctrl )
            {
            if ( !( ctrl->IsNonFocusing() ) )
                {
                ctrl->SetFocus( IsFocused(), IsFocused() ? EDrawNow: ENoDrawNow );
                }
            }
        
        if ( IsFocused() )
            {
            NotifyControlsForEvent( EMsgViewEventViewFocusGain, 0 );
            }
        else
            {
            NotifyControlsForEvent( EMsgViewEventViewFocusLost, 0 );
            }
        }
    }

// ---------------------------------------------------------
// CMsgEditorView::HandleResourceChange
//
//
// ---------------------------------------------------------
//
void CMsgEditorView::HandleResourceChange( TInt aType )
    {
    // Updates child control layouts. Must be called before
    // layout switch is handled here as AdjustComponentDistances 
    // expects controls to have correct height. Child control
    // height may vary if layout is using different font.
    CCoeControl::HandleResourceChange( aType ); 
    
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        iScrollBar->SetScrollBarFrameObserver( this );
        
        TInt invisibleLines( iFormOffset / iLineHeight );
        
        iLineHeight = MsgEditorCommons::MsgBaseLineDelta();
        iBaseLineOffset = MsgEditorCommons::MsgBaseLineOffset();
        
        // Correct the form offset to be multiple of current line height
        iFormOffset = invisibleLines * iLineHeight;
        
        TRect mainPane = MsgEditorCommons::MsgMainPane();
        SetViewRect( mainPane );
        iSize = mainPane.Size();
        
        AknLayoutUtils::LayoutVerticalScrollBar(
            iScrollBar,
            TRect( TPoint( 0, 0 ), mainPane.Size() ),
            AknLayoutScalable_Apps::scroll_pane_cp017().LayoutLine() );
            
        TRAP_IGNORE( RefreshViewL() );
        
        EnsureCorrectViewPosition();
        }
    }

// ---------------------------------------------------------
// CMsgEditorView::HandleControlEventL
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CMsgEditorView::HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType )
    {
    if ( aControl != iHeader &&
         aControl != iBody )
        {
        switch( aEventType )
            {
            case MCoeControlObserver::EEventPrepareFocusTransition:
                {
                // These things must be done before pointer event is handled.
                CMsgBaseControl* baseControl = static_cast<CMsgBaseControl*>( aControl );
    
                if ( baseControl && 
                     ( baseControl->ControlType() == EMsgBodyControl ||
                       baseControl->ControlType() == EMsgXhtmlBodyControl ) &&
                     baseControl->ItemFinder() && 
                     static_cast<CMsgBodyControl*>( baseControl )->Editor().TextView() )
                    {
                    TInt componentIndex = iBody->ComponentIndexFromId( baseControl->ControlId() );
                    baseControl->SetupAutomaticFindAfterFocusChangeL( iBody->CurrentFocus() <= componentIndex );
                    }
                    
                break;
                }
            case MCoeControlObserver::EEventRequestFocus:
                {
                // These things are done when pointer event is handled and focus should be changed.
                // NOTE: CMsgAddressControlEditor is relying on focus not to set before
                //       pointer event is processed. Otherwise it is impossible to 
                //       determine whether control has been focused before or not.
                CMsgBaseControl* baseControl = static_cast<CMsgBaseControl*>( aControl );
                SetFocusByControlIdL( baseControl->ControlId(), ETrue, EFalse );                    
                
                UpdateScrollBarL();
                }
            default:
                {
                break;
                }
            }
        }
    //MEBLOGGER_WRITEF(_L("MEB: CMsgEditorView::HandleControlEventL"));
    }
#else
void CMsgEditorView::HandleControlEventL( CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/ )
    {
    //MEBLOGGER_WRITEF(_L("MEB: CMsgEditorView::HandleControlEventL"));
    }
#endif // RD_SCALABLE_UI_V2

// ---------------------------------------------------------
// CMsgEditorView::CreateHeaderL
//
// Creates the header.
// ---------------------------------------------------------
//
void CMsgEditorView::CreateHeaderL()
    {
    __ASSERT_DEBUG( iHeader == NULL, Panic( EMsgHeaderAlreadyExists ) );

    TMargins margins;

    margins.iLeft = 0;
    margins.iRight = 0;
    margins.iTop = 0;
    margins.iBottom = 0;

    iHeader = CMsgHeader::NewL( *this, margins );
    iHeader->SetObserver( this );
    }

// ---------------------------------------------------------
// CMsgEditorView::CreateBodyL
//
// Creates the body. Creates also the default body control if requested.
// ---------------------------------------------------------
//
void CMsgEditorView::CreateBodyL()
    {
    __ASSERT_DEBUG( iBody == NULL, Panic( EMsgBodyAlreadyExists ) );

    TMargins margins;

    margins.iLeft = 0;
    margins.iRight = 0;
    margins.iTop = 0;
    margins.iBottom = 0;

    iBody = CMsgBody::NewL( *this, margins );
    iBody->SetObserver( this );

    if ( !( iEditorModeFlags & EMsgDoNotUseDefaultBodyControl ) )
        {
        CMsgBaseControl* bodyControl = CMsgBodyControl::NewL( this );
        bodyControl->SetControlType( EMsgBodyControl );
        DoAddControlL( bodyControl, EMsgComponentIdBody, EMsgFirstControl, EMsgBody );
        }
    }

// ---------------------------------------------------------
// CMsgEditorView::CreateScrollBarL
//
// Creates the scroll bar.
// ---------------------------------------------------------
//
void CMsgEditorView::CreateScrollBarL()
    {
    __ASSERT_DEBUG( iScrollBar == NULL, Panic( EMsgScrollBarAlreadyExists ) );

    iScrollBar = new ( ELeave ) CEikScrollBarFrame(
        this,  // CCoeControl* aParentWindow
        this  // MEikScrollBarObserver* aObserver
        );

    // Check which type of scrollbar is to be shown    
    if ( AknLayoutUtils::DefaultScrollBarType( iAvkonAppUi ) == CEikScrollBarFrame::EDoubleSpan )
        {
        // For EDoubleSpan type scrollbar
        // non-window owning scrollbar
        iScrollBar->CreateDoubleSpanScrollBarsL( EFalse, EFalse, ETrue, EFalse );
        iScrollBar->SetTypeOfVScrollBar( CEikScrollBarFrame::EDoubleSpan );
        
        AknLayoutUtils::LayoutVerticalScrollBar(
            iScrollBar,
            TRect( TPoint( 0, 0 ), 
                   MsgEditorCommons::MsgMainPane().Size() ),
            AknLayoutScalable_Apps::scroll_pane_cp017().LayoutLine() );        
        }
    else
        {
        // For EArrowHead type scrollbar
        iScrollBar->SetTypeOfVScrollBar( CEikScrollBarFrame::EArrowHead );
        }

    iScrollBar->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, 
                                         CEikScrollBarFrame::EAuto );
    }

// ---------------------------------------------------------
// CMsgEditorView::UpdateScrollBarL
//
// Updates the scroll bar.
// ---------------------------------------------------------
//
void CMsgEditorView::UpdateScrollBarL()
    {   
    TInt height( 0 );
    TInt pos( 0 );
    TInt windowSize( iViewRect.Height() - iBaseLineOffset );
    
    // Round height up to the next full line height.
    MsgEditorCommons::RoundToNextLine( windowSize, iLineHeight );    
    
    GetVirtualFormHeightAndPos( height, pos );
    
    // Round height up to the next full line height.
    MsgEditorCommons::RoundToNextLine( height, iLineHeight );    
    
    if ( pos < iLineHeight )
        {
        pos = 0;
        }
    
    // Round position up to the next full line height.
    MsgEditorCommons::RoundToNextLine( pos, iLineHeight );
    
#ifdef _DEBUG
    // CAknScrollIndicator::SetPosition has an __ASSERT_DEBUG
    // for range check even if the control handles out-of-range
    // values properly.
    if ( pos > height ) pos = height;
    if ( pos < -1 )     pos = -1;
    
#endif

    if ( iScrollBar->TypeOfVScrollBar() == CEikScrollBarFrame::EDoubleSpan )
        {
        TAknDoubleSpanScrollBarModel vDsSbarModel;
        vDsSbarModel.SetScrollSpan( height );
        vDsSbarModel.SetWindowSize( windowSize );       
        
        pos = Max( 0, Min( height - windowSize, pos ) );  
        
        vDsSbarModel.SetFocusPosition( pos );
        
        TEikScrollBarFrameLayout layout;
        layout.iTilingMode = TEikScrollBarFrameLayout::EInclusiveRectConstant;
        
        // It seems to be important that we have separate
        // variable for "inclusiveRect" and "clientRect"
        TRect inclusiveRect( Rect() );
        TRect clientRect( Rect() );
        
        iScrollBar->TileL( NULL, &vDsSbarModel, clientRect, inclusiveRect, layout );
        iScrollBar->SetVFocusPosToThumbPos( vDsSbarModel.FocusPosition() );
        
        iViewFocusPosition = vDsSbarModel.FocusPosition();
        }
    else
        {
        TEikScrollBarModel vertModel;
        vertModel.iScrollSpan = height;
        vertModel.iThumbSpan  = windowSize;
        vertModel.iThumbPosition = pos;
        
        TEikScrollBarFrameLayout layout;
    
        TRect rect( Rect() );
        iScrollBar->TileL( NULL, &vertModel, rect, rect, layout );
        iScrollBar->SetVFocusPosToThumbPos( vertModel.iThumbPosition );
        
        iViewFocusPosition = vertModel.iThumbPosition;
        }
    
    MsgEditorCommons::RoundToNextLine( iViewFocusPosition, iLineHeight );    
    }


// ---------------------------------------------------------
// CMsgEditorView::GetVirtualFormHeightAndPos
//
// Return virtual height and virtual Y position of the form.
// This function assumes that only body can contain controls which virtual
// height must be calculated. Also this funtion assumes that only the
// currently focused control in the body can contain text above visible
// text view.
// ---------------------------------------------------------
//
void CMsgEditorView::GetVirtualFormHeightAndPos( TInt& aHeight, TInt& aPos )
    {
    TInt screenHeight = MsgEditorCommons::EditorViewHeigth() - iBaseLineOffset;
    
    TInt scrollPartsHeight( 0 );
    TInt abovePartHeight( 0 );
    
    TInt bodyHeight( iBody->VirtualHeight() );
    
    TInt headerHeight( 0 );
    
    TBool headerLoaded( iHeader->CountMsgControls() );
    if ( headerLoaded )
        {
        // Header loaded so get header height and subtract base line offset
        // from it as header first control contains it and it is not scrollable
        // area.
        headerHeight = iHeader->VirtualHeight() - iBaseLineOffset;
        }
    else
        {
        // No header so baseline offset is added to the first body control's distance.
        // Baseline offset is not scrollable area so it must be subtracted from body height.
        bodyHeight -= iBaseLineOffset;
        }
    
    iVisiblePartHeight = bodyHeight + headerHeight;
    
    // Visible height is always at least one screen height.
    iVisiblePartHeight = Max( screenHeight, iVisiblePartHeight );
    
    // Calculate total scroll parts high excluding visible part and 
    // height of the scroll parts above visible part.
    TInt singlePartHeight = Max( screenHeight, iVisiblePartHeight / 10 );
    for ( TInt current = 0; current < iScrollParts; current++ )
        {
        if ( current != iVisiblePart )
            {
            scrollPartsHeight += singlePartHeight;
            
            if ( current < iVisiblePart )
                {
                abovePartHeight = scrollPartsHeight;
                }
            }
        }
    
    aHeight = iVisiblePartHeight + scrollPartsHeight;

    if ( iCurrentFocus == EMsgBodyFocused )
        {
        TInt bodyExt = abovePartHeight + iBody->VirtualExtension();
        
        if ( headerLoaded )
            {
            // Remove visible header pixels from header height.
            TInt visiblePixels( iHeader->Position().iY + iHeader->Size().iHeight - iBaseLineOffset );
            if (  visiblePixels > 0 )
                {
                headerHeight -= visiblePixels;
                }
            }
        else
            {
            // No header so baseline offset is added to the first body control's distance.
            // Baseline offset is not scrollable area so it must be subtracted from body extension.
            bodyExt -= iBaseLineOffset;
            }
        
        aPos = headerHeight + bodyExt;

        if ( iVisiblePart == ( iScrollParts - 1 ) )
            {
            // This ensures that if we at the end of the scroll area then scroll position is also
            // at the end.
            CMsgBaseControl* ctrl = FocusedControl();
            
            if ( ctrl && 
                 ctrl->IsCursorLocation( EMsgBottom ) &&
                 NextFocusableFormControl( iBody, 
                         iBody->CurrentFocus() + 1, 
                         EMsgFocusDown ) == KErrNotFound )
                {
                // Further scrolling (down) not possible.
                aPos = aHeight;
                
                if ( iScrollBar->TypeOfVScrollBar() == CEikScrollBarFrame::EDoubleSpan )
                    {
                    // Focus position is top of the view.
                    aPos -= screenHeight;
                    }
                }
            }
        }
    else
        {
        aPos = abovePartHeight + iHeader->VirtualExtension() - iBaseLineOffset;
        }
    }

// ---------------------------------------------------------
// CMsgEditorView::SetAndGetSizeL
//
// Sets sizes for the header and the body and returns their total size aSize
// as a reference. The function does not set new sizes for the controls if
// the aInit argument is EFalse. If aInit == ETrue, this function sets the size
// so that only a total height can change.
// ---------------------------------------------------------
//
void CMsgEditorView::SetAndGetSizeL( TSize& aSize, TBool aInit )
    {
    __ASSERT_DEBUG( iHeader != NULL, Panic( EMsgHeaderNotExists ) );
    __ASSERT_DEBUG( iBody != NULL, Panic( EMsgBodyNotExists ) );

    if ( aInit && iStateFlags & EMsgStateInitialized )
        {
        // Size changing at run time. Set intializing flag on.
        iStateFlags |= EMsgStateInitializing;
        }

    TSize editorSize( aSize );
    TSize headerSize( 0, 0 );
    TSize bodySize( 0, 0 );

    headerSize = aSize;
    iHeader->SetAndGetSizeL( headerSize, aInit );

    bodySize = aSize;
    iBody->SetAndGetSizeL( bodySize, aInit );

    editorSize.iHeight = headerSize.iHeight + bodySize.iHeight;

    // View size must always be the size of client rect. Don't change "iSize"!
    aSize = editorSize;
    
    if ( aInit && iStateFlags & EMsgStateInitialized )
        {
        iStateFlags &= ~EMsgStateInitializing;
        }
    }

// ---------------------------------------------------------
// CMsgEditorView::RefreshViewL
//
// Sets the positions for the controls and re-draws the view.
// ---------------------------------------------------------
//
void CMsgEditorView::RefreshViewL()
    {
    iStateFlags |= EMsgStateRefreshing;
    
    // Component must know their correct height by now.
    AdjustComponentDistances();

    TSize editorSize( iViewRect.Size() );
    
    // Called after AdjustComponentDistances because that
    // information is used on some components to calculate
    // their sizes
    SetAndGetSizeL( editorSize, ETrue );
    SizeChanged();
    
    UpdateScrollBarL();
    
    NotifyControlsForEvent( EMsgViewEventPrepareForViewing, 0 );
    DrawDeferred();

    iStateFlags &= ~EMsgStateRefreshing;

    EnsureCorrectFormPosition( EFalse );
    }

// ---------------------------------------------------------
// CMsgEditorView::NotifyControlsForEvent
//
// Notifies controls for an event.
// ---------------------------------------------------------
//
void CMsgEditorView::NotifyControlsForEvent( TMsgViewEvent aEvent, TInt aParam )
    {
    __ASSERT_DEBUG( iHeader != NULL, Panic( EMsgHeaderNotExists ) );
    __ASSERT_DEBUG( iBody != NULL, Panic( EMsgBodyNotExists ) );

    iHeader->NotifyControlsForEvent( aEvent, aParam );
    iBody->NotifyControlsForEvent( aEvent, aParam );
    }

// ---------------------------------------------------------
// CMsgEditorView::SetFocusByControlIdL
//
// Sets focus to a control aControlId. aSetCursorPos specified
// whether cursor position should be set or not.
// ---------------------------------------------------------
//
void CMsgEditorView::SetFocusByControlIdL( TInt aControlId, 
                                           TBool aCorrectFormPosition,
                                           TBool aSetCursorPos )
    {
    __ASSERT_DEBUG( iHeader != NULL, Panic( EMsgHeaderNotExists ) );
    __ASSERT_DEBUG( iBody != NULL, Panic( EMsgBodyNotExists ) );
    __ASSERT_DEBUG( aControlId >= 0, Panic( EMsgIncorrectControlId ) );

    NotifyControlsForEvent( EMsgViewEventFocusMoveStarting, 0 ); 
    
    ReportFocusMovement( MMsgEditorObserver::EMsgFocusMovingFrom );
    
    TInt componentIndex = iHeader->ComponentIndexFromId( aControlId );

    if ( componentIndex != KErrNotFound )
        {
        // Control where focus is wanted to be moved was found from header.
        if ( iCurrentFocus == EMsgBodyFocused )
            {
            if (IsReadOnly()) // viewer
                {
                 iBody->NotifyControlsForEvent( EMsgViewEventPrepareFocusTransitionUp, 0 );
                 iBody->SetFocus( EFalse, 
                             iStateFlags & EMsgStateInitialized ? EDrawNow :
                                                                  ENoDrawNow );                            
                }
            else
                {
                iBody->SetFocus( EFalse, 
                             iStateFlags & EMsgStateInitialized ? EDrawNow :
                                                                  ENoDrawNow );                            
                iBody->NotifyControlsForEvent( EMsgViewEventPrepareFocusTransitionUp, 0 ); 
                }
            }
        
        TMsgFocus previousMsgPart = iCurrentFocus;
        iCurrentFocus = EMsgHeaderFocused;
        
        TInt delta = componentIndex - iHeader->CurrentFocus();
        
        if ( delta != 0 )
            {
            iHeader->SetFocus( EFalse, 
                               iStateFlags & EMsgStateInitialized ? EDrawNow :
                                                                    ENoDrawNow );
            iHeader->NotifyControlsForEvent( delta > 0 ? EMsgViewEventPrepareFocusTransitionDown : 
                                                         EMsgViewEventPrepareFocusTransitionUp,
                                             0 );
            }
        
        // Do real focus movement when focus is moved between
        // message parts or when focus is moved inside message part
        // to different component. Do it also if currently focused 
        // control does not have focus really.
        if ( componentIndex != iHeader->CurrentFocus() ||
             previousMsgPart != iCurrentFocus ||
             !iHeader->FocusedControl()->IsFocused() )
            {
            if ( !iHeader->ChangeFocusTo( componentIndex, 
                                          iStateFlags & EMsgStateInitialized ? EDrawNow :
                                                                               ENoDrawNow ) )
                {
                __ASSERT_DEBUG( EFalse, Panic( EMsgNonFocusingControl ) );
                }
            }
        }
    else
        {
        componentIndex = iBody->ComponentIndexFromId( aControlId );
        
        if ( componentIndex != KErrNotFound )
            {
            // Control where focus is wanted to be moved was found from body.
            if ( iCurrentFocus == EMsgHeaderFocused )
                {
                 if (IsReadOnly())
                    {
                    iHeader->NotifyControlsForEvent( EMsgViewEventPrepareFocusTransitionDown, 0 ); 
                    iHeader->SetFocus( EFalse, 
                                   iStateFlags & EMsgStateInitialized ? EDrawNow :
                                                                        ENoDrawNow );
                    }
                else
                    {

                    iHeader->SetFocus( EFalse, 
                                   iStateFlags & EMsgStateInitialized ? EDrawNow :
                                                                        ENoDrawNow );
                    iHeader->NotifyControlsForEvent( EMsgViewEventPrepareFocusTransitionDown, 0 ); 
                    }
                }
            TMsgFocus previousMsgPart = iCurrentFocus;    
            iCurrentFocus = EMsgBodyFocused;
            
            TInt delta = componentIndex - iBody->CurrentFocus();
            
            if ( delta != 0 )
                {
                iBody->SetFocus( EFalse, 
                                 iStateFlags & EMsgStateInitialized ? EDrawNow :
                                                                      ENoDrawNow );            
                iBody->NotifyControlsForEvent( delta ? EMsgViewEventPrepareFocusTransitionDown : 
                                                       EMsgViewEventPrepareFocusTransitionUp,
                                               0 );
                }
            
            // Do real focus movement when focus is moved between
            // message parts or when focus is moved inside message part
            // to different component. Do it also if currently focused 
            // control does not have focus really.
            if ( componentIndex != iBody->CurrentFocus() ||
                 previousMsgPart != iCurrentFocus ||
                 !iBody->FocusedControl()->IsFocused() )
                {
                CMsgBaseControl* ctrl = iBody->MsgControl( componentIndex );
    
                if ( aSetCursorPos &&
                     ctrl && 
                     ( ctrl->ControlType() == EMsgBodyControl ||
                       ctrl->ControlType() == EMsgXhtmlBodyControl ) &&
                     ctrl->ItemFinder() && 
                     static_cast<CMsgBodyControl*>( ctrl )->Editor().TextView() )
                    {
                    ctrl->SetupAutomaticFindAfterFocusChangeL( iBody->CurrentFocus() <= componentIndex );
                    }
        
                if ( !iBody->ChangeFocusTo( componentIndex, 
                                            iStateFlags & EMsgStateInitialized ? EDrawNow :
                                                                                 ENoDrawNow ) )
                    {
                    __ASSERT_DEBUG( EFalse, Panic( EMsgNonFocusingControl ) );
                    }
                }
            }
        else
            {
            __ASSERT_DEBUG( EFalse, Panic( EMsgNoFocusingControlExist ) );
            }
        }
        
    NotifyControlsForEvent( EMsgViewEventFocusMoveFinished, 0 );
    
    ReportFocusMovement( MMsgEditorObserver::EMsgFocusMovedTo );

    if ( aCorrectFormPosition )
        {
        EnsureCorrectFormPosition( IsReadOnly() );
        }

    if ( aSetCursorPos && 
         IsReadOnly() && 
         FocusedControl() )
        {
        FocusedControl()->NotifyViewEvent( EMsgViewEventSetCursorLastPos, 0 );
        }
    }

// ---------------------------------------------------------
// CMsgEditorView::DoAddControlL
//
// Does an actual add operation for the control by setting all the necessary
// observers etc.
// ---------------------------------------------------------
//
void CMsgEditorView::DoAddControlL( CMsgBaseControl* aControl,
                                    TInt aControlId,
                                    TInt aIndex,
                                    TMsgFormComponent aFormComponent )
    {
    __ASSERT_DEBUG( iHeader != NULL, Panic( EMsgHeaderNotExists ) );
    __ASSERT_DEBUG( iBody != NULL, Panic( EMsgBodyNotExists ) );

    // set readonly for control here because readonly flag
    // may be needed before ActivateL call.
    TUint32 flags = aControl->ControlModeFlags();
    if ( IsReadOnly() ||  
         flags & EMsgControlModeReadOnly )
        {
        flags |= EMsgControlModeReadOnly;
        }
    else
        {
        flags &= ~EMsgControlModeReadOnly;
        }
    aControl->SetControlModeFlags( flags );
    
    if ( !aControl->OwnsWindow() )
        {
        aControl->SetContainerWindowL( *this );
        }
        
    aControl->SetObserver( this );
    aControl->SetBaseControlObserver( *this );
    
    if ( iEdwinObserver )
        {
        CEikRichTextEditor* editor = NULL;
    
        if ( aControl->ControlType() == EMsgExpandableControl ||
             aControl->ControlType() == EMsgAddressControl )
            {
            editor = &static_cast<CMsgExpandableControl*>( aControl )->Editor();
            }
        else if ( aControl->ControlType() == EMsgBodyControl ||
                  aControl->ControlType() == EMsgXhtmlBodyControl )
            {
            editor = &static_cast<CMsgBodyControl*>( aControl )->Editor();
            }
        
        if ( editor )
            {
            editor->AddEdwinObserverL( iEdwinObserver );
            }
        }
    
    aControl->MakeVisible( ETrue );
    aControl->ActivateL();
    
    // this calls PrepareForReadOnly and sets flags again
    // but it doesn't matter.
    aControl->SetReadOnly( flags & EMsgControlModeReadOnly );

    if ( aFormComponent == EMsgHeader )
        {
        iHeader->AddControlL( aControl, aControlId, aIndex );
        }
    else if ( aFormComponent == EMsgBody )
        {
        iBody->AddControlL( aControl, aControlId, aIndex );

        if ( iHeader->CountMsgControls() == 0 )
            {
            iCurrentFocus = EMsgBodyFocused;
            }
        }
    else
        {
        __ASSERT_DEBUG( EFalse, Panic( EMsgIncorrectFormComponent ) );
        }
    
    if ( aControl->UniqueHandle() == KErrNotFound )
        {
        // Control added first time. Set unique handle.
        aControl->SetUniqueHandle( iUniqueHandlePool );
        iUniqueHandlePool++;
        }
    
    if ( iStateFlags & EMsgStateInitialized )  // Control is added on run-time.
        {
        TRAPD( err, RefreshViewL() );

        if ( err != KErrNone )
            {
            // if RefreshViewL fails, remove the added control from internal
            // structures, i.e. transfer ownership back to caller.
            if ( aFormComponent == EMsgHeader )
                {
                iHeader->RemoveControlL( aControlId ); // does not leave.
                }
            else if ( aFormComponent == EMsgBody )
                {
                iBody->RemoveControlL( aControlId ); // does not leave.
                }

            User::Leave( err );
            }
        else
            {
            // Set control initialized as it has been successfully added & activated.
            flags = aControl->ControlModeFlags();
            flags |= EMsgControlModeInitialized;
            aControl->SetControlModeFlags( flags );
            }
        }
    }

// ---------------------------------------------------------
// CMsgEditorView::RotateFocusL
//
// Rotates focus up or down depending aDirection argument. Returns ETrue if
// operation can be done.
// ---------------------------------------------------------
//
TBool CMsgEditorView::RotateFocusL( TMsgFocusDirection aDirection,
                                    MMsgEditorObserver::TMsgFocusEvent& aFocusEvent )
    {
    __ASSERT_DEBUG( iHeader != NULL, Panic( EMsgHeaderNotExists ) );
    __ASSERT_DEBUG( iBody != NULL, Panic( EMsgBodyNotExists ) );

    TBool startAutoFind = EFalse;
    TBool setMaxHeight = EFalse;
    
    TInt newFocus = KErrNotFound;
    TInt controlIndex = 0;
    
    CMsgFormComponent* currForm = NULL;
    CMsgFormComponent* nextForm = NULL;
    
    if ( iCurrentFocus == EMsgHeaderFocused )
        {
        currForm = iHeader;
        controlIndex = iHeader->CurrentFocus();

        if ( aDirection == EMsgFocusDown )
            {
            newFocus = NextFocusableFormControl( iHeader, controlIndex + 1, EMsgFocusDown );

            if ( newFocus != KErrNotFound )
                {
                nextForm = iHeader;
                }
            else
                {
                newFocus = NextFocusableFormControl( iBody, 0, EMsgFocusDown );

                if ( newFocus != KErrNotFound )
                    {
                    startAutoFind = ETrue;
                    nextForm = iBody;
                    aFocusEvent = MMsgEditorObserver::EMsgFocusToBody;
                    }
                else
                    {
                    TInt currentlyFocused = iBody->CurrentFocus();
                    
                    if ( IsFocusable( iBody->MsgControl( currentlyFocused ), EMsgFocusDown ) )
                        {
                        // Refocus the currently focused control.
                        nextForm = iBody;
                        startAutoFind = ETrue;
                        newFocus = currentlyFocused;
                        }
                        
                    aFocusEvent = MMsgEditorObserver::EMsgFocusAtBottom;
                    }
                }
            }
        else if ( aDirection == EMsgFocusUp )
            {
            newFocus = NextFocusableFormControl( iHeader, controlIndex - 1, EMsgFocusUp );

            if ( newFocus != KErrNotFound )
                {
                nextForm = iHeader;
                }
            else
                {
                aFocusEvent = MMsgEditorObserver::EMsgFocusAtTop;
                }
            }
        }
    else if ( iCurrentFocus == EMsgBodyFocused )
        {
        currForm = iBody;
        controlIndex = iBody->CurrentFocus();

        if ( aDirection == EMsgFocusDown )
            {
            newFocus = NextFocusableFormControl( iBody, controlIndex + 1, EMsgFocusDown );

            if ( newFocus != KErrNotFound )
                {
                nextForm = iBody;
                }
            else
                {
                aFocusEvent = MMsgEditorObserver::EMsgFocusAtBottom;
                }
            }
        else if ( aDirection == EMsgFocusUp )
            {
            newFocus = NextFocusableFormControl( iBody, controlIndex - 1, EMsgFocusUp );

            if ( newFocus != KErrNotFound )
                {
                nextForm = iBody;
                }
            else
                {
                newFocus = NextFocusableFormControl( iHeader, iHeader->CountMsgControls() - 1, EMsgFocusUp );
                setMaxHeight = ETrue;
                
                if ( newFocus != KErrNotFound )
                    {
                    nextForm = iHeader;
                    aFocusEvent = MMsgEditorObserver::EMsgFocusToHeader;
                    }
                else
                    {
                    TInt currentlyFocused = iHeader->CurrentFocus();
                    if ( IsFocusable( iHeader->MsgControl( currentlyFocused ), EMsgFocusUp ) )
                        {
                        // Refocus the currently focused control.
                        nextForm = iHeader;
                        newFocus = currentlyFocused;
                        }
                        
                    aFocusEvent = MMsgEditorObserver::EMsgFocusAtTop;
                    }
                }
            }

        if ( ( aFocusEvent == MMsgEditorObserver::EMsgFocusNone || 
               aFocusEvent == MMsgEditorObserver::EMsgFocusToBody ) && 
             ( newFocus != controlIndex && 
               newFocus > KErrNotFound ) ) // for automatic highlight
            { 
            // we assume that editor size is correct in this point
            startAutoFind = ETrue;
            }

        }
    
    if ( setMaxHeight )
        {
        CMsgBaseControl* ctrl = FocusedControl(); // for automatic highlight
        
        if ( ctrl && 
             ( ctrl->ControlType() == EMsgBodyControl ||
               ctrl->ControlType() == EMsgXhtmlBodyControl ) && 
             ctrl->ItemFinder() ) 
            {
            static_cast<CMsgBodyControl*>( ctrl )->Editor().SetMaximumHeight( 
                                                            MsgEditorCommons::MaxBodyHeight() );
            } // for automatic highlight
        }

    if ( nextForm != NULL )
        {                                              
        NotifyControlsForEvent( aDirection == EMsgFocusUp ? EMsgViewEventPrepareFocusTransitionUp : 
                                                            EMsgViewEventPrepareFocusTransitionDown, 
                                0 );
        
        NotifyControlsForEvent( EMsgViewEventFocusMoveStarting, 0 );
            
        ReportFocusMovement( MMsgEditorObserver::EMsgFocusMovingFrom );
        
        if ( nextForm == currForm )
            {
            currForm->ChangeFocusTo( newFocus, 
                                     iStateFlags & EMsgStateInitialized ? EDrawNow :
                                                                          ENoDrawNow );
            }
        else
            {
            currForm->SetFocus( EFalse, 
                                iStateFlags & EMsgStateInitialized ? EDrawNow :
                                                                     ENoDrawNow );
            iCurrentFocus = ( nextForm == iBody ) ? EMsgBodyFocused : 
                                                    EMsgHeaderFocused;
            
            nextForm->ChangeFocusTo( newFocus,
                                     iStateFlags & EMsgStateInitialized ? EDrawNow :
                                                                          ENoDrawNow );
            }
        
        CMsgBaseControl* focusedControl = FocusedControl();
        
        if ( focusedControl )
            {
            if ( aDirection == EMsgFocusDown )
                {
                focusedControl->NotifyViewEvent( EMsgViewEventSetCursorFirstPos, 0 );
                }
            else if ( aDirection == EMsgFocusUp )
                {
                focusedControl->NotifyViewEvent( EMsgViewEventSetCursorLastPos, 0 );
                }
            }
        
        NotifyControlsForEvent( EMsgViewEventFocusMoveFinished, 0 );
            
        ReportFocusMovement( MMsgEditorObserver::EMsgFocusMovedTo );
        }
    else
        {
        CMsgBaseControl* focusedControl = FocusedControl();
        
        if ( !IsReadOnly() && focusedControl && !startAutoFind )
            {
            // Focus is tried to move to direction that does not cause any focus
            // changes (either up when already at the topmost position or down when 
            // already at the bottommost position) on editor. 
            // => Set focused cursor position to either last or first position if we
            // are at last or first scroll part. 
            if ( aDirection == EMsgFocusDown && 
                 iVisiblePart == iScrollParts - 1 )
                {
                if ( focusedControl->ControlType() == EMsgBodyControl ||
                     focusedControl->ControlType() == EMsgXhtmlBodyControl )
                    {
                    CEikRichTextEditor& editor = static_cast<CMsgBodyControl*>( focusedControl )->Editor();
                    editor.SetCursorPosL( editor.TextLength(), EFalse );
                    }
                else if ( focusedControl->ControlType() == EMsgExpandableControl ||
                          focusedControl->ControlType() == EMsgAddressControl )
                    {
                    CEikRichTextEditor& editor = 
                                static_cast<CMsgExpandableControl*>( focusedControl )->Editor();
                    editor.SetCursorPosL( editor.TextLength(), EFalse );
                    
                    if ( focusedControl->ControlType() == EMsgAddressControl )
                        {
                        // Update the address field highlighting
                        static_cast<CMsgAddressControlEditor&>( editor ).CheckHighlightingL();
                        }
                    }
                }
            else if ( aDirection == EMsgFocusUp &&
                      iVisiblePart == 0)
                {
                if ( focusedControl->ControlType() == EMsgBodyControl ||
                     focusedControl->ControlType() == EMsgXhtmlBodyControl )
                    {
                    CEikRichTextEditor& editor = static_cast<CMsgBodyControl*>( focusedControl )->Editor();
                    editor.SetCursorPosL( 0, EFalse );
                    }
                else if ( focusedControl->ControlType() == EMsgExpandableControl ||
                          focusedControl->ControlType() == EMsgAddressControl )
                    {
                    CEikRichTextEditor& editor = 
                            static_cast<CMsgExpandableControl*>( focusedControl )->Editor();
                    editor.SetCursorPosL( 0, EFalse );
                    
                    if ( focusedControl->ControlType() == EMsgAddressControl )
                        {
                        // Update the address field highlighting
                        static_cast<CMsgAddressControlEditor&>( editor ).CheckHighlightingL();
                        }
                    }
                }
            }
        }
    
    if ( startAutoFind ) 
        { 
        CMsgBaseControl* ctrl = (CMsgBaseControl*)iBody->MsgControl( newFocus );
    
        if ( ctrl && 
             ( ctrl->ControlType() == EMsgBodyControl ||
               ctrl->ControlType() == EMsgXhtmlBodyControl ) &&
             ctrl->ItemFinder() && 
             static_cast<CMsgBodyControl*>( ctrl )->Editor().TextView() )
            {
            ctrl->SetupAutomaticFindAfterFocusChangeL( aDirection == EMsgFocusDown );
            }
        }
        
    return newFocus != KErrNotFound;
    }

// ---------------------------------------------------------
// CMsgEditorView::EnsureCorrectFormPosition
//
// Ensures that the cursor is visible on the view and that the form position
// is correct.
// ---------------------------------------------------------
//
TBool CMsgEditorView::EnsureCorrectFormPosition( TBool /*aScrollDown*/, TBool /*aScrollUp*/ )
    {
    CMsgBaseControl* ctrl = FocusedControl();
    if ( !ctrl )
        {
        return EFalse;
        }

    iStateFlags |= EMsgEnsureCorrectFormPositionRequestIssued;

    TInt delta = 0;
    TBool viewScrolled = ETrue;
    TRect lineRect = ctrl->IsReadOnly() ? ctrl->Rect() : 
                                          ctrl->CurrentLineRect();

    if ( lineRect.Height() )
        {
        if ( lineRect.iBr.iY > iViewRect.Height() )
            {
            // the view must be scrolled up.
            delta = iViewRect.Height() - lineRect.iBr.iY;
            }
        else if ( lineRect.iTl.iY < 0 && 
                  lineRect.Height() <= iViewRect.Height() )
            {
            // the view must be scrolled down.
            delta = -lineRect.iTl.iY;
            }
        
        // Adjust to next(/previous) baseline
        if ( delta > 0 )
            {
            MsgEditorCommons::RoundToNextLine( delta, iLineHeight );            
            }
        else
            {
            MsgEditorCommons::RoundToPreviousLine( delta, iLineHeight );                        
            }
        }

    if ( delta )
        {
        EnsureCorrectCursorPosition();
        
        viewScrolled = EFalse;
        ScrollForm( delta, ETrue );
        }

    iStateFlags &= ~EMsgEnsureCorrectFormPositionRequestIssued;

    return viewScrolled;
    }

// ---------------------------------------------------------
// CMsgEditorView::HandleHeightChangedL
//
// Handles the form's height change. Gets pointer aControl to a control which
// height is changing. If aDeltaHeight > 0 the height is increasing.
// Assumes that control's size has already been changed.
// ---------------------------------------------------------
//
TBool CMsgEditorView::HandleHeightChangedL( CMsgBaseControl* /*aControl*/, 
                                            TInt /*aDeltaHeight*/ )
    {
    MEBLOGGER_ENTERFN( "CMsgEditorView::HandleHeightChangedL" );

    TSize headerSize = iHeader->Size();
    
    TSize size( headerSize + iBody->Size() );
    TSize thisSize( iViewRect.Size() );

    SetAndGetSizeL( thisSize, EFalse );
    
    if ( size.iHeight != thisSize.iHeight )
        {
        SizeChanged();
        }
    
    if ( ViewInitialized() )
        {
        if ( headerSize != iHeader->Size() )
            {
            DrawNow();
            }
        else
            {
            DrawDeferred();
            }
        }

    MEBLOGGER_ENTERFN( "CMsgEditorView::HandleHeightChangedL" );

    return ETrue;
    }

// ---------------------------------------------------------
// CMsgEditorView::ScrollForm
//
// Scrolls the form up or down depending on aDelta. If aDelta > 0, the form
// scrolls down. Form offset should also be less or equal to 0. 
// ---------------------------------------------------------
//
TBool CMsgEditorView::ScrollForm( TInt aDelta, TBool aUpdateScrollBarPos )
    {
    TBool result( ETrue );
    
    if ( iFormOffset + aDelta > 0 )
        {
        // Scrolling form given amount of pixels would result form to go above
        // it's uppermost position.
        aDelta = -iFormOffset;
        
        result = EFalse;
        }
        
    if ( aDelta != 0 )
        {    
        iFormOffset += aDelta;
        
        // Real scrolling is performed here.
        SizeChanged();
        
        // Do not redraw the screen if we are in the middle of state refreshing. 
        if ( !( iStateFlags & EMsgStateRefreshing ) )
            {
            DrawNow();
            }
            
        NotifyControlsForEvent( EMsgViewEventFormScrolled, aDelta );
        
        if ( aUpdateScrollBarPos )
            {
            iViewFocusPosition -= aDelta;
            
            const TAknDoubleSpanScrollBarModel* model = AknScrollBarModel();
           
            if ( model )
                {
                iScrollBar->MoveThumbsBy( 0, -aDelta );
                }
            }
        }
    
    return result;
    }

// ---------------------------------------------------------
// CMsgEditorView::SetViewRect
//
// Sets view rect.
// ---------------------------------------------------------
//
void CMsgEditorView::SetViewRect( const TRect& /*aRect*/ )
    {
    iViewRect = MsgEditorCommons::MsgMainPane();
    
    Window().SetExtent( iViewRect.iTl, iViewRect.Size() );
    }

// ---------------------------------------------------------
// CMsgEditorView::AdjustComponentDistances
//
// Set proper distances between controls to obey LAF coordinates.
//
// Note: Components must know their correct height when this function
//       is called.
// ---------------------------------------------------------
//
void CMsgEditorView::AdjustComponentDistances()
    {
    TInt i;
    CMsgBaseControl* ctrl;
    TInt controlType;
    TInt countHeader = iHeader->CountMsgControls();
    TInt countBody = iBody->CountMsgControls();    

    TInt baseLine = iLineHeight;
    TInt firstComponentOffset = iBaseLineOffset;

    // beforeOffset + control height + afterOffset
    // must be multiple of baseLine!
    TInt beforeOffset = 0;
    TInt afterOffset = 0;
    TInt distance = 0;

    for ( i = 0; i < countHeader; i++ )
        {
        ctrl = iHeader->MsgControl( i );
        if ( ctrl )
            {
            controlType = ctrl->ControlType();

            if ( controlType == EMsgAddressControl ||
                 controlType == EMsgExpandableControl ||
                 controlType == EMsgAttachmentControl )
                {
                distance = ( i == 0 ) ? firstComponentOffset : 
                                        0;
                distance += afterOffset;

                // At least in APAC there's offset in the header
                // fields. A header field is 21 pixels high whereas
                // the baseline is 19 pixels.
                TInt offset = -( ctrl->Size().iHeight % baseLine );
                // TODO: Minor MAGIC
                // header frames take 3 pixels so it's ok to
                // have offset of "-3".
                if ( offset < -3 )
                    {
                    offset += baseLine;
                    }
                // divide offset evenly between before and after offset
                //beforeOffset = offset / 2;
                //afterOffset = offset - beforeOffset;
                //distance += beforeOffset;
                distance += offset;

                ctrl->SetDistanceFromComponentAbove( distance );
                }
            }
        }

    for ( i = 0; i < countBody; i++ )
        {
        ctrl = iBody->MsgControl( i );
        
        if ( ctrl )
            {
            if ( i == 0 )
                {
                distance = ( countHeader == 0 ) ? firstComponentOffset : 
                                                  0;
                }
            else
                {
                distance = baseLine;
                }
                
            distance += afterOffset;

            controlType = ctrl->ControlType();
            
            switch ( controlType )
                {
                case EMsgImageControl:
                case EMsgAudioControl:
                    {
                    // Image control height is always
                    // multiple of baseLine
                    beforeOffset = 0;
                    afterOffset = 0;
                    break;
                    }
                case EMsgVideoControl:
                    {
                    // Video must be precisely at the top of the screen.
                    beforeOffset = -firstComponentOffset;
                    // Video control height is not multiple of baseLine
                    // -> calculate offset
                    afterOffset = firstComponentOffset -
                        ( ctrl->Size().iHeight % baseLine );
                    break;
                    }
                default:
                    {
                    beforeOffset = 0;
                    afterOffset = 0;
                    break;
                    }
                }
            distance += beforeOffset;
           
            ctrl->SetDistanceFromComponentAbove( distance );

            // TODO: Is this the correct place to check this?!
            if ( !IsReadOnly() && 
                 ( controlType == EMsgBodyControl ||
                   controlType == EMsgXhtmlBodyControl ) )
                {
                TUint32 flags = ctrl->ControlModeFlags();
                if ( iBody->ComponentIndexFromId( EMsgComponentIdBody ) == 
                     countBody - 1 )
                    {
                    // In editor mode bodycontrol must have max height 
                    // if it's the last control in msg body even if it
                    // has no text so that all editor lines are drawn.
                    flags |= EMsgControlModeBodyMaxHeight;
                    }
                else
                    {
                    flags &= ~EMsgControlModeBodyMaxHeight;
                    }
                    
                ctrl->SetControlModeFlags( flags );
                }
            }
        }
    }

// ---------------------------------------------------------
// CMsgEditorView::SetComponentsInitialized
//
// Set all controls initialized.
// ---------------------------------------------------------
//
void CMsgEditorView::SetComponentsInitialized()
    {
    TInt i;
    CMsgBaseControl* ctrl;
    TInt countHeader = iHeader->CountMsgControls();
    TInt countBody = iBody->CountMsgControls();

    TUint32 flags;

    for ( i = 0; i < countHeader; i++ )
        {
        ctrl = iHeader->MsgControl( i );
        
        if ( ctrl )
            {
            flags = ctrl->ControlModeFlags();
            flags |= EMsgControlModeInitialized;
            ctrl->SetControlModeFlags( flags );
            }
        }

    for ( i = 0; i < countBody; i++ )
        {
        ctrl = iBody->MsgControl( i );
        
        if ( ctrl )
            {
            flags = ctrl->ControlModeFlags();
            flags |= EMsgControlModeInitialized;
            ctrl->SetControlModeFlags( flags );
            }
        }
    }


// ---------------------------------------------------------
// CMsgEditorView::SetAfterFocusL
//
//
//
// ---------------------------------------------------------
//
TBool CMsgEditorView::SetAfterFocusL( MMsgEditorObserver::TMsgAfterFocusEventFunc aAfterFocus )
    {
    TBool forceScrollUp = EFalse;
    CMsgBaseControl* ctrl;
    TInt newFocus;
    CMsgBodyControl* body = static_cast<CMsgBodyControl*>( ControlById( EMsgComponentIdBody ) );
    CEikRichTextEditor* bodyEditor = NULL;

    if ( body )
        {
        bodyEditor = &body->Editor();
        body->SetupAutomaticFindAfterFocusChangeL(
                            aAfterFocus == MMsgEditorObserver::EMsgCursorToBodyBeginning );
        }

    if ( aAfterFocus == MMsgEditorObserver::EMsgCursorToBodyBeginning )
        {
        newFocus = iBody->FirstFocusableControl( 0, EMsgFocusDown );
        
        if ( newFocus != KErrNotFound )
            {
            ctrl = iBody->MsgControl( newFocus );
            SetFocusByControlIdL( ctrl->ControlId() );

            if ( bodyEditor )
                {
                bodyEditor->SetCursorPosL( 0, EFalse );
                }
            }
        }
    else if ( aAfterFocus == MMsgEditorObserver::EMsgCursorToBodyEnd )
        {
        newFocus = iBody->FirstFocusableControl( iBody->CountMsgControls() - 1, EMsgFocusUp );

        if ( newFocus == KErrNotFound )
            {
            // empty body perhaps...
            newFocus = iHeader->FirstFocusableControl( iHeader->CountMsgControls() - 1, EMsgFocusUp );

            if ( newFocus != KErrNotFound )
                {
                ctrl = iHeader->MsgControl( newFocus );
                SetFocusByControlIdL( ctrl->ControlId(), EFalse );
                }
            }
        else
            {
            ctrl = iBody->MsgControl( newFocus );
            SetFocusByControlIdL( ctrl->ControlId(), EFalse );

            if ( bodyEditor )
                {               
                bodyEditor->SetCursorPosL( bodyEditor->TextLength(), EFalse );
                }
                
            forceScrollUp = ETrue;
            }
        }
    return forceScrollUp;
    }

// ---------------------------------------------------------
// CMsgEditorView::ItemFinder
//
// ---------------------------------------------------------
//
EXPORT_C CItemFinder* CMsgEditorView::ItemFinder()
    {   
    if ( iBody )
        {
        CMsgBaseControl* ctrl;
        TInt countBody = iBody->CountMsgControls();

        for ( TInt i = 0; i < countBody; i++ )
            {
            ctrl = iBody->MsgControl( i );
            if ( ctrl &&
                 ( ctrl->ControlType() == EMsgBodyControl ||
                   ctrl->ControlType() == EMsgXhtmlBodyControl ) )
                {
                return ctrl->ItemFinder();
                }
            }
        }
    return 0;
    }

// ---------------------------------------------------------
// CMsgEditorView::SetEdwinObserverL
//
// ---------------------------------------------------------
//
EXPORT_C void CMsgEditorView::SetEdwinObserverL( MEikEdwinObserver* aObserver )
    {
    iEdwinObserver = aObserver;
    
    TInt headerControlCount = iHeader->CountMsgControls();
    TInt controlCount = headerControlCount + iBody->CountMsgControls();
    
    for ( TInt controlIndex = 0; controlIndex < controlCount; controlIndex++ )
        {
        CMsgBaseControl* control = NULL;
        if ( controlIndex < headerControlCount )
            {
            control = iHeader->MsgControl( controlIndex );
            }
        else
            {
            control = iBody->MsgControl( controlIndex - headerControlCount );
            }
        
        CEikRichTextEditor* editor = NULL;
        if ( control->ControlType() == EMsgExpandableControl ||
             control->ControlType() == EMsgAddressControl )
            {
            editor = &static_cast<CMsgExpandableControl*>( control )->Editor();
            }
        else if ( control->ControlType() == EMsgBodyControl ||
                  control->ControlType() == EMsgXhtmlBodyControl )
            {
            editor = &static_cast<CMsgBodyControl*>( control )->Editor();
            }
        
        if ( editor )
            {
            editor->RemoveEdwinObserver( iEdwinObserver );
            
            if ( iEdwinObserver )
                {
                editor->AddEdwinObserverL( iEdwinObserver );
                }
            }
        }
    }

// ---------------------------------------------------------
// CMsgEditorView::ControlFullyVisible
//
// If control's:
// - top Y-coordinate is below or equal messaging main pane top Y-coordinate and 
// - bottom Y-coordinate is above or equal messaging main pane bottom Y-coordinate 
// then control is fully visible. Currently all controls are expected
// to have equal width as messaging data pane.
// ---------------------------------------------------------
//
TBool CMsgEditorView::ControlFullyVisible( CMsgBaseControl* aControl ) const
    {
    TBool retVal = EFalse;
    
    if ( aControl )
        {
        TRect rect = aControl->Rect();
        if ( rect.iTl.iY >= 0 && 
             rect.iBr.iY <= MsgEditorCommons::EditorViewHeigth() )
            {
            retVal = ETrue;
            }
        }
        
    return retVal;
    }

// ---------------------------------------------------------
// CMsgEditorView::NextFocusableFormControl
// 
// Searches then next focusable form control from given form component
// starting from certain control and proceeding into given direction.
// 
// Stops when either focusable control is found or there is no more
// controls left. See CMsgEditorView::IsFocusable function for
// description when control is focusable.
// ---------------------------------------------------------
//
TInt CMsgEditorView::NextFocusableFormControl( CMsgFormComponent* aFormComponent, 
                                               TInt aStart, 
                                               TMsgFocusDirection aDirection )
    {
    TInt newFocus = aFormComponent->FirstFocusableControl( aStart, aDirection );
    TBool found = EFalse;

    while ( newFocus != KErrNotFound && 
            !found )
        {
        found = IsFocusable( aFormComponent->MsgControl( newFocus ), aDirection );
            
        if ( !found )
            {
            newFocus = aFormComponent->FirstFocusableControl( 
                                aDirection == EMsgFocusDown ? newFocus + 1 : 
                                                              newFocus - 1, 
                                aDirection );
            }        
        }
        
    return newFocus;
    }

// ---------------------------------------------------------
// CMsgEditorView::MopSupplyObject
//
// ---------------------------------------------------------
//
EXPORT_C TTypeUid::Ptr CMsgEditorView::MopSupplyObject( TTypeUid aId )
    {   
    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }

    return CCoeControl::MopSupplyObject( aId );
    }

// ---------------------------------------------------------
// CMsgEditorView::HandleScrollEventL
//
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CMsgEditorView::HandleScrollEventL( CEikScrollBar* aScrollBar, 
                                         TEikScrollEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEikScrollUp:
            {
            ScrollViewL( iLineHeight, EMsgScrollUp, ETrue );
            EnsureCorrectScrollPartL( AknScrollBarModel()->FocusPosition() );
            break;
            }
        case EEikScrollDown:
            {
            ScrollViewL( iLineHeight, EMsgScrollDown, ETrue );
            EnsureCorrectScrollPartL( AknScrollBarModel()->FocusPosition() );
            break;
            }
        case EEikScrollTop:
            {
            // Not supported yet.
            break;
            }
        case EEikScrollBottom:
            {
            // Not supported yet.
            break;
            }
        case EEikScrollThumbReleaseVert:
            {
            iScrollBar->DrawBackground( ETrue, ETrue );
            
            EnsureCorrectScrollPartL( AknScrollBarModel()->FocusPosition() );
            iPopUpPart = -1;
            break;
            }
        case EEikScrollPageUp:
        case EEikScrollPageDown:
            {
            TInt scrolledPixels = iViewFocusPosition - AknScrollBarModel()->FocusPosition();
            
            // Round to the previous full line.
            MsgEditorCommons::RoundToPreviousLine( scrolledPixels, iLineHeight );
        
            if ( scrolledPixels != 0 )
                {
                ScrollViewL( Abs( scrolledPixels ), 
                             scrolledPixels > 0 ? EMsgScrollUp :
                                                  EMsgScrollDown,
                             EFalse );
                
                EnsureCorrectScrollPartL( AknScrollBarModel()->FocusPosition() );
                
                iScrollBar->DrawScrollBarsNow();
                }
            break;
            }
        case EEikScrollThumbDragVert:
            {
            TInt currentPart( CurrentScrollPart( AknScrollBarModel()->FocusPosition() ) );
            
            if ( currentPart == iVisiblePart )
                {
                if ( iPopUpPart != -1 )
                    {
                    // Hide the popup if visible
                    static_cast<CAknDoubleSpanScrollBar*>( aScrollBar )->SetScrollPopupInfoTextL( KNullDesC );
                    iPopUpPart = -1;
                    }    
                }
            
            if ( iPopUpPart == -1 )
                {
                TInt scrolledPixels = iViewFocusPosition - AknScrollBarModel()->FocusPosition();
            
                // Round to the previous full line.
                MsgEditorCommons::RoundToPreviousLine( scrolledPixels, iLineHeight );
            
                if ( scrolledPixels != 0 )
                    {
                    ScrollViewL( Abs( scrolledPixels ), 
                                 scrolledPixels > 0 ? EMsgScrollUp :
                                                      EMsgScrollDown,
                                 EFalse );
                    }
                }
            
            TInt pixelsScrolled = iViewFocusPosition - AknScrollBarModel()->FocusPosition();
            TMsgScrollDirection scrollDirection = pixelsScrolled > 0 ? EMsgScrollUp : EMsgScrollDown;

            // If scrolled position is outside of visible part, then there is no need to 
            // reset thumb position and no page number pop-up need be shown.
            if ( ( currentPart != iVisiblePart ) &&
                 EnsureVisiblePartScrollComplete ( AknScrollBarModel()->FocusPosition(), scrollDirection ) )
                {
                // Thumb position is forced to the top most position of currently scrolled slide.
                TInt currentHeight( 0 );
                TInt screenHeight = iViewRect.Height() - iBaseLineOffset;

                for ( TInt current = 0; current < currentPart; current++ )
                    {
                    if ( current == iVisiblePart )
                        {
                        currentHeight += iVisiblePartHeight;
                        }
                    else
                        {
                        currentHeight += screenHeight;
                        }
                    }
                
                iScrollBar->DrawBackground( ETrue, EFalse );
                iScrollBar->SetVFocusPosToThumbPos( currentHeight );
                
                if ( currentPart != iPopUpPart )
                    {
                    ShowScrollPopupInfoTextL( static_cast<CAknDoubleSpanScrollBar*>( aScrollBar ), 
                                              currentPart + 1 );
                    iPopUpPart = currentPart;
                    }
                }
            else
                {
                iScrollBar->DrawScrollBarsNow();
                }
            break;
            }
        default:
            {
            break;
            }
        }
    }
#else
void CMsgEditorView::HandleScrollEventL( CEikScrollBar* /*aScrollBar*/, 
                                         TEikScrollEvent /*aEventType*/ )
    {
    }
#endif // RD_SCALABLE_UI_V2

// ---------------------------------------------------------
// CMsgEditorView::Draw
// Draws skin background for the view area. With video control
// this should not be done since we might accidentally draw over
// the video frame. In this case we only draw around the video control
// and let the video control draw its own background when approriate.
// ---------------------------------------------------------
//
void CMsgEditorView::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    
    CMsgBaseControl* videoControl = iBody->Component( EMsgComponentIdVideo );
    
    TBool backgroundDrawn( EFalse );
    if ( !videoControl )
        {
        backgroundDrawn = AknsDrawUtils::Background( 
                                     AknsUtils::SkinInstance(), 
                                     AknsDrawUtils::ControlContext( this ), 
                                     this, 
                                     gc, 
                                     aRect );
        }
    else
        {
        backgroundDrawn = AknsDrawUtils::BackgroundBetweenRects( 
                                                     AknsUtils::SkinInstance(), 
                                                     AknsDrawUtils::ControlContext( this ), 
                                                     this, 
                                                     gc, 
                                                     aRect,
                                                     videoControl->Rect() );
        }

        
    if ( !backgroundDrawn )
        {
        gc.SetBrushColor( AKN_LAF_COLOR( 0 ) );
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc.SetPenStyle( CGraphicsContext::ENullPen );
        gc.DrawRect( aRect );
        }
    }
    
// ---------------------------------------------------------
// CMsgEditorView::ReportFocusMovement
//
// ---------------------------------------------------------
//
void CMsgEditorView::ReportFocusMovement( TInt aFocusEvent )
    {
    CMsgBaseControl* ctrl = FocusedControl();
    
    TInt controlId = ctrl ? ctrl->ControlId() : 
                     EMsgComponentIdNull;
                     
    iEditorObserver.EditorObserver( MMsgEditorObserver::EMsgHandleFocusChange,
                                    &aFocusEvent,
                                    &controlId,
                                    NULL );
    }

// ---------------------------------------------------------
// CMsgEditorView::ControlFromPosition
//
// ---------------------------------------------------------
//
CMsgBaseControl* CMsgEditorView::ControlFromPosition( TPoint aPosition, 
                                                      TBool aEvaluateHitTest ) const
    {
    CMsgBaseControl* control = NULL;
    
    if ( iHeader->Rect().Contains( aPosition ) )
        {
        control = iHeader->ControlFromPosition( aPosition, aEvaluateHitTest );
        }
    else
        {
        control = iBody->ControlFromPosition( aPosition, aEvaluateHitTest );
        }
        
    return control;
    }

// ---------------------------------------------------------
// CMsgEditorView::EnsureVisiblePartScrollComplete
//
// Description
// ---------------------------------------------------------

TBool CMsgEditorView::EnsureVisiblePartScrollComplete( TInt aFocusPosition,
                                                       TMsgScrollDirection aDirection )
    {
    MEBLOGGER_ENTERFN( "CMsgEditorView::EnsureVisiblePartScrollComplete" );
    TInt scrollTillPosition( 0 );
    TBool result = ETrue;
    
    TInt screenHeight = MsgEditorCommons::EditorViewHeigth() - iBaseLineOffset;

    for ( TInt current = 0; current < iVisiblePart ; current++ )
        scrollTillPosition += screenHeight;
    
    if ( aDirection == EMsgScrollDown )
        {
        scrollTillPosition += iVisiblePartHeight;

        if ( aFocusPosition < scrollTillPosition )
            result = EFalse;
        }
    else
        {
        if ( aFocusPosition > scrollTillPosition )
            result = EFalse;
        }
    
    MEBLOGGER_LEAVEFN( "CMsgEditorView::EnsureVisiblePartScrollComplete" );
    return result;
    }

// ---------------------------------------------------------
// CMsgEditorView::ScrollViewL
//
// First bound check is made so that no scrolling below minimum &
// above maximum is performed. This might happen with out check
// when using scrollbar arrows. DoScrollViewL function is called
// to perform the real scrolling. After all scrolling is done we 
// move the thumb position if this is requested by the caller.
// ---------------------------------------------------------
//
void CMsgEditorView::ScrollViewL( TInt aPixelsToScroll, 
                                  TMsgScrollDirection aDirection, 
                                  TBool aMoveThumb )
    {
    MEBLOGGER_ENTERFN( "CMsgEditorView::ScrollViewL" );
    
    const TAknDoubleSpanScrollBarModel* model = AknScrollBarModel();
    
    // Maximum value for thumb [0..scrollSpan]
    TInt scrollSpan = model->ScrollSpan();
    
    // Position of the topmost part of the thumb
    TInt thumbTopPos = iViewFocusPosition;
    
    // Position of the downmost part of the thumb
    TInt thumbDownPos = thumbTopPos + model->WindowSize();
    
    // Bound check
    if ( ( aDirection == EMsgScrollUp && thumbTopPos <= 0 ) ||
         ( aDirection == EMsgScrollDown && thumbDownPos >= scrollSpan ) )
        {
        return;
        }
    
    if ( CurrentScrollPart( iViewFocusPosition ) != iVisiblePart )
        {
        // No need to perform scrolling, if the visible part is completely
        // scrolled and the new scroll position is outside the visible part
        if ( EnsureVisiblePartScrollComplete ( iViewFocusPosition, aDirection ) )
            return;
        }
    
    DoScrollViewL( aPixelsToScroll, aDirection );
            
    if ( aMoveThumb )
        {
        // Scroll bar thumb is moved only if caller requests it. Scroll bar seems to
        // add/subtract 1 pixel depending of direction.
        TInt directionMultiplier( -1 );
        if ( aDirection == EMsgScrollDown )
            {
            directionMultiplier = 1;
            }
        
        iScrollBar->SetVFocusPosToThumbPos( model->FocusPosition() - 
                                                directionMultiplier * 1 + 
                                                directionMultiplier * aPixelsToScroll );
        }
    
    MEBLOGGER_LEAVEFN( "CMsgEditorView::ScrollViewL" );
    }

// ---------------------------------------------------------
// CMsgEditorView::EnsureCorrectCursorPosition
//
// Ensures that cursor position relative to focus position
// on all controls is on correct position.
// ---------------------------------------------------------
//
void CMsgEditorView::EnsureCorrectCursorPosition()
    {
    CMsgFormComponent* currentForm = NULL;
    if ( iCurrentFocus == EMsgHeaderFocused )
        {
        iBody->NotifyControlsForEvent( EMsgViewEventSetCursorFirstPos, 0 );
        currentForm = iHeader;
        }
    else
        {
        iHeader->NotifyControlsForEvent( EMsgViewEventSetCursorLastPos, 0 );
        currentForm = iBody;
        }
    
    TInt componentIndex = currentForm->ComponentIndexFromId( currentForm->FocusedControl()->ControlId() );
    
    for ( TInt current = 0; current < currentForm->CountMsgControls(); current++ )
        { 
        CMsgBaseControl* control = currentForm->MsgControl( current );
        TInt currentIndex = currentForm->ComponentIndexFromId( control->ControlId() );
        if (  currentIndex < componentIndex )
            {
            control->NotifyViewEvent( EMsgViewEventSetCursorLastPos, 0 );
            }
        else if ( currentIndex > componentIndex )
            {
            control->NotifyViewEvent( EMsgViewEventSetCursorFirstPos, 0 );
            }
        }
    }

// ---------------------------------------------------------
// CMsgEditorView::EnsureCorrectScrollPartL
//
// Ensures that correct scroll part is visible. Performs scroll
// part change if focus position is outside of currently visible scroll
// part.
// ---------------------------------------------------------
//
void CMsgEditorView::EnsureCorrectScrollPartL( TInt aFocusPosition )
    {
    TInt currentPart( CurrentScrollPart( aFocusPosition ) );
    if ( currentPart != iVisiblePart )
        {
        MMsgEditorObserver::TMsgFocusEvent focusEvent = MMsgEditorObserver::EMsgFocusNone;
        
        if ( currentPart > iVisiblePart )
            {
            focusEvent = MMsgEditorObserver::EMsgFocusAtBottom;
            }
        else
            {
            focusEvent = MMsgEditorObserver::EMsgFocusAtTop;
            }
        
        MMsgEditorObserver::TMsgAfterFocusEventFunc after = 
                                        MMsgEditorObserver::EMsgAfterFocusNone;
        
        iVisiblePart = currentPart;
        iEditorObserver.EditorObserver( MMsgEditorObserver::EMsgHandleFocusChange,
                                        &focusEvent,
                                        &after,
                                        &iVisiblePart );
        
        if ( after != MMsgEditorObserver::EMsgAfterFocusNone )
            {
            SetAfterFocusL( after );
            }
    
        EnsureCorrectFormPosition( EFalse, EFalse );

        UpdateScrollBarL();
        }
    }

// ---------------------------------------------------------
// CMsgEditorView::CurrentScrollPart
// 
// Calculates the correct scroll part of the given focus position.
// Current scroll part is evaluated based on position of top & bottom
// of the view rectangle. If either one of the borders are on the
// different scroll part than visible part then that part is returned
// as current part.
// ---------------------------------------------------------
//
TInt CMsgEditorView::CurrentScrollPart( TInt aFocusPosition )
    {
    TInt result( 0 );
    
    TInt currentHeight( 0 );
    TInt topPart( -1 );
    TInt bottomPart( -1 );
    
    TInt topPosition( aFocusPosition );
    TInt bottomPosition( topPosition + iViewRect.Height() - iBaseLineOffset );
    
    TInt screenHeight = MsgEditorCommons::EditorViewHeigth() - iBaseLineOffset;
    
    for ( TInt current = 0; current < iScrollParts; current++ )
        {
        if ( current == iVisiblePart )
            {
            currentHeight += iVisiblePartHeight;
            }
        else
            {
            currentHeight += screenHeight;
            }
        
        if ( topPart == -1 &&
             topPosition < currentHeight )
            {
            topPart = current;
            }
        
        if ( bottomPart == -1 &&
             bottomPosition <= currentHeight )
            {
            bottomPart = current;
            }
        }
    
    if ( topPart == -1 )
        {
        topPart = iScrollParts - 1;
        }
    
    if ( bottomPart == -1 )
        {
        bottomPart = iScrollParts - 1;
        }
    
    if ( topPart == bottomPart )
        {
        result = topPart;
        }
    else
        {
        if ( topPart != iVisiblePart )
            {
            result = topPart;
            }
        else
            {
            result = bottomPart;
            }
        }
    return result;
    }  

// ---------------------------------------------------------
// CMsgEditorView::IsFocusable
// 
// Determines if given control should receive focus when navigating
// into given direction.
//
// On editor mode (i.e. non read-only) every control is focusable.
//
// On viewer mode (i.e. read-only) control is focusable if:
// - force focus stop flag is set to control, 
// - control is not fully visible on the screen,
// - focus change to given direction is not allowed or
// - there is currently focused (i.e. currently shown and
//   highlighted) automatic finder item.
// ---------------------------------------------------------
//
TBool CMsgEditorView::IsFocusable( CMsgBaseControl* aControl,
                                   TMsgFocusDirection aDirection ) const
    {
    if ( !aControl )
        {
        return EFalse;
        }
        
    if ( aControl->IsReadOnly() )
        {
        if ( aControl->ControlModeFlags() & EMsgControlModeForceFocusStop ||
             !ControlFullyVisible( aControl ) ||
             !( aControl->IsFocusChangePossible( aDirection ) ) )
            {
            return ETrue;
            }
        else if ( aControl->ItemFinder() ) 
            {
            const CItemFinder::CFindItemExt& item = aControl->ItemFinder()->CurrentItemExt();
            
            if ( item.iItemType != CItemFinder::ENoneSelected )
                {
                return ETrue;
                }
            }
            
        return EFalse;
        }
        
    return ETrue;
    }

// ---------------------------------------------------------
// CMsgEditorView::ShowScrollPopupInfoTextL
//
// Loads the info text resouce if not yet loaded. Creates correct
// text string based on this resource and given part number.
// After this sets the created info text for the scroll bar.
// ---------------------------------------------------------
//
void CMsgEditorView::ShowScrollPopupInfoTextL( CAknDoubleSpanScrollBar* aScrollBar, TInt aPartNumber )
    {
    if ( !iScrollPopText )
        {
        if ( !iCoeEnv->IsResourceAvailableL( R_QTN_MSG_PAGE_NUMBER_POPUP ) )
            {
	        // Load the resource file containing page number popup if it has not yet
	        // been loaded. This should not happen but better to be prepared for that also.
	        TParse parse;
            User::LeaveIfError( parse.Set( KMsgEditorAppUiResourceFileName, 
                                           &KDC_RESOURCE_FILES_DIR, 
                                           NULL ) );
        
            TFileName* fileName = new( ELeave ) TFileName( parse.FullName() );
            CleanupStack::PushL( fileName );
        
            iResourceLoader.OpenL( *fileName );
        
            CleanupStack::PopAndDestroy( fileName );
	        }
    
        iScrollPopText = StringLoader::LoadL( R_QTN_MSG_PAGE_NUMBER_POPUP, iCoeEnv );
        }
    
    TBuf<KMsgMaximumScrollPartLength> buffer;
    StringLoader::Format( buffer, *iScrollPopText, KErrNotFound, aPartNumber );

    aScrollBar->SetScrollPopupInfoTextL( buffer );
    }

// ---------------------------------------------------------
// CMsgEditorView::EnsureCorrectViewPosition
// ---------------------------------------------------------
//
void CMsgEditorView::EnsureCorrectViewPosition()
    {
    CMsgBaseControl* firstHeaderControl = iHeader->CountMsgControls() > 0 ? iHeader->MsgControl( 0 ) : NULL;

    CMsgBaseControl* lastControlBody = iBody->CountMsgControls() > 0 ? 
                                                    iBody->MsgControl( iBody->CountMsgControls() - 1 ) : NULL;
    
    if ( firstHeaderControl &&
         firstHeaderControl->Position().iY > iBaseLineOffset )
        {
        TInt pixelsToScroll( firstHeaderControl->Position().iY - iBaseLineOffset );
        TRAP_IGNORE( DoScrollViewL( pixelsToScroll, 
                                    EMsgScrollDown ) );
        }
    else if ( lastControlBody && 
              lastControlBody->Rect().iBr.iY <= iViewRect.Height() - iLineHeight )
        {
        TInt pixelsToScroll( iViewRect.Height() - lastControlBody->Rect().iBr.iY );
        TRAP_IGNORE( DoScrollViewL( pixelsToScroll, 
                                    EMsgScrollUp ) );
        }
    else
        {         
        
      	CMsgBaseControl* ctrl = FocusedControl();
        if (( ctrl && ctrl->ControlModeFlags() & EMsgControlModeBodyMaxHeight) &&  ctrl->VirtualHeight() < MsgEditorCommons::MaxBodyHeight() )
            {
             // Special handling is required when we rotate the phone from 
             // landscape to portrait or vice versa when the Editor is placed last 
             TInt bottomYPos( ctrl->Position().iY + ctrl->VirtualHeight() );          
             TInt distanceFromBottom( Rect().iBr.iY - bottomYPos );
    
             if ( distanceFromBottom > 0 )
                {
                TInt delta = Abs( iFormOffset ) < distanceFromBottom ? -iFormOffset :
                                                                           distanceFromBottom;                    
                if ( delta )
                    {
                     ScrollForm( delta, ETrue );
                    }
                 }               
                 if ( ViewInitialized() )
                    {
               		TRAP_IGNORE(UpdateScrollBarL());
                    }
                    
            }
        else if( ctrl )
        	{
        	TInt height( 0 );
        	TInt pos( 0 );   
    
        	GetVirtualFormHeightAndPos( height, pos );
        	if(!ControlFullyVisible( ctrl ) &&  height > iViewRect.Height() )
            	{
               	TInt controlPixels(  Abs(ctrl->Position().iY  ));                                                  
               	MsgEditorCommons::RoundToPreviousLine( controlPixels, iLineHeight );
               	TRAP_IGNORE(DoScrollViewL(controlPixels,EMsgScrollDown));  
           		}	
        	}                      
       	}     
    }

// ---------------------------------------------------------
// CMsgEditorView::DoScrollViewL
//
// First & last loaded control is
// needed in order to do bound checking so that empty space above
// or below loaded controls isn't shown. Actual scrolling is performed
// so that control from first or last visible (depends about the
// scrolling direction) line is retrieved. If this control isn't fully
// visible on the scroll we try to move it as much visible as allowed
// by the scrollable pixels. When the control is fully visible it can
// perform internal scrolling if needed. When it has finished internal
// scrolling then we move the view one line up or down. This is continued
// as long as there is pixels left. After all scrolling is done we 
// update our internal view focus position
// that keeps count where the focus position on the view is really
// located. Finally we draw the screen if needed.
// ---------------------------------------------------------
//
void CMsgEditorView::DoScrollViewL( TInt& aPixelsToScroll, 
                                    TMsgScrollDirection aDirection )
    {
    MEBLOGGER_ENTERFN( "CMsgEditorView::DoScrollViewL" );
    
    TInt pixelsLeftToScroll( aPixelsToScroll );
    
    TInt directionMultiplier( -1 );
    if ( aDirection == EMsgScrollDown )
        {
        directionMultiplier = 1;
        }
    
    CMsgBaseControl* firstControl = iHeader->CountMsgControls() > 0 ? iHeader->MsgControl( 0 ) :
                                                                      iBody->MsgControl( 0 );
    
    CMsgBaseControl* lastControl = iBody->CountMsgControls() > 0 ? 
                                        iBody->MsgControl( iBody->CountMsgControls() - 1 ) :
                                        iHeader->MsgControl( iHeader->CountMsgControls() - 1 );
    
    while ( pixelsLeftToScroll > 0 )
        {
        MEBLOGGER_WRITEF(_L("MEB: CMsgEditorView::DoScrollViewL: pixelsLeftToScroll %d "), pixelsLeftToScroll );
        
        CMsgBaseControl* scrolledControl = NULL;
       
        if ( aDirection == EMsgScrollUp )
            {
            // Control located at first visible line
            scrolledControl = ControlFromPosition( TPoint( iViewRect.Width() / 2, iLineHeight / 2 + iBaseLineOffset ), EFalse );
            }
        else
            {
            // Control located at last visible line
            scrolledControl = ControlFromPosition( TPoint( iViewRect.Width() / 2, iViewRect.Height() - iLineHeight / 2 ), EFalse );
            }
    
        if ( scrolledControl )
            {
            // Assumption: Control will make internal scrolling only if
            // it's height is greater or equal to view height.
            if ( !ControlFullyVisible( scrolledControl ) )
                {   
                // Calculate how many pixels needs to be scrolled in order
                // to control be fully visible.
                TInt controlPixels( Abs( scrolledControl->Position().iY ) );
                
                if ( aDirection == EMsgScrollDown )
                    {
                    // Round to the previous full line.
                    MsgEditorCommons::RoundToPreviousLine( controlPixels, iLineHeight );
                    }
                else 
                    {
                    // Round to the next full line.
                    MsgEditorCommons::RoundToNextLine( controlPixels, iLineHeight );
                    }
                
                // Scroll form so that control is fully visible or
                // at minimum one line height and at maximum pixels left to scroll.
                TInt scrollFormByPixels = Max( iLineHeight, Min( pixelsLeftToScroll, 
                                                                 controlPixels ) );                    
                
                TInt oldYPos( scrolledControl->Position().iY );
                
                // To disable drawing in the middle of scrolling.
                iStateFlags |= EMsgStateRefreshing;
                
                if ( CurrentScrollPart( iViewFocusPosition - aPixelsToScroll + pixelsLeftToScroll + directionMultiplier * scrollFormByPixels ) != iVisiblePart )
                    {
                    // Do not scroll form if scrolling would make scroll part change.
                    break;
                    }
                
                ScrollForm( -directionMultiplier * scrollFormByPixels, EFalse );
                
                // Substract the pixels really moved.
                pixelsLeftToScroll -= Abs( oldYPos - scrolledControl->Position().iY );
                
                MEBLOGGER_WRITEF(_L("MEB: CMsgEditorView::DoScrollViewL: pixelsLeftToScroll after fully visible %d "), pixelsLeftToScroll );
                }
            
            if ( pixelsLeftToScroll > 0 )
                {
                // Perform internal scrolling for the control if there is still pixels left to scroll.
                pixelsLeftToScroll -= scrolledControl->ScrollL( pixelsLeftToScroll, aDirection );
                
                MEBLOGGER_WRITEF(_L("MEB: CMsgEditorView::DoScrollViewL: pixelsLeftToScroll after internal scrolling %d "), pixelsLeftToScroll );
                }
            }
        
        // After component's internal scrolling there is still some pixels left to scroll
        // move the view to correction direction one line.
        if ( pixelsLeftToScroll > 0 )
            {
            // Boundary check for form scrolling. Do not scroll up (down) if first (last) control is
            // fully visible. Also do not scroll form if scrolling would make scroll part change.
            if ( aDirection == EMsgScrollDown ? lastControl->Rect().iBr.iY > iViewRect.Height() :
                                                firstControl->Rect().iTl.iY < iViewRect.Height() &&
                 CurrentScrollPart( iViewFocusPosition - aPixelsToScroll + pixelsLeftToScroll + directionMultiplier * iLineHeight ) == iVisiblePart )
                {
                
                if ( pixelsLeftToScroll > 0 )
                    {
                    // To disable drawing in the middle of scrolling.
                    iStateFlags |= EMsgStateRefreshing;
                    
                    ScrollForm( -directionMultiplier * iLineHeight, EFalse );
                    pixelsLeftToScroll -= iLineHeight;
                    
                    MEBLOGGER_WRITEF(_L("MEB: CMsgEditorView::DoScrollViewL: pixelsLeftToScroll after single scrolling %d "), pixelsLeftToScroll );
                    
                    }
                }
            else                
                {
                break;                    
                }
            }
        }
        
    iViewFocusPosition += directionMultiplier * ( aPixelsToScroll - pixelsLeftToScroll );
    
    if ( iStateFlags & EMsgStateRefreshing )
        {
        iStateFlags &= ~EMsgStateRefreshing;
        DrawNow();
        }
    
    MEBLOGGER_LEAVEFN( "CMsgEditorView::ScrollViewL" );
    }


// End of File
