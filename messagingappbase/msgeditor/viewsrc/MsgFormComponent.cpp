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
* Description:  MsgFormComponent implementation
*
*/



// ========== INCLUDE FILES ================================
#include <AknUtils.h>                      // for AknUtils
#include <AknDef.h>

#include "MsgFormComponent.h"              // for CMsgFormComponent
#include "MsgControlArray.h"               // for CMsgControlArray
#include "MsgBaseControl.h"                // for CMsgBaseControl
#include "MsgEditorPanic.h"                // for CMsgEditor panics
#include "MsgEditorCommon.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

const TInt KComponentArrayGranularity = 5;
const TInt ENoFocus = -1;

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgFormComponent::CMsgFormComponent
//
// Constructor.
// ---------------------------------------------------------
//
CMsgFormComponent::CMsgFormComponent( const TMargins& aMargins ) : 
    iCurrentFocus( ENoFocus ),
    iMargins( aMargins )
    {
    SetComponentsToInheritVisibility( ETrue );
    }

// ---------------------------------------------------------
// CMsgFormComponent::~CMsgFormComponent
//
// Destructor.
// ---------------------------------------------------------
//
CMsgFormComponent::~CMsgFormComponent()
    {
    delete iControls;
    }

// ---------------------------------------------------------
// CMsgFormComponent::BaseConstructL
//
//
// ---------------------------------------------------------
//
void CMsgFormComponent::BaseConstructL( const CCoeControl& aParent )
    {
    SetContainerWindowL( aParent );
    iControls = new ( ELeave ) CMsgControlArray( KComponentArrayGranularity );
    }

// ---------------------------------------------------------
// CMsgFormComponent::Component
//
// Return a pointer to a control by given control id aControlId. If the
// control cannot be found returns NULL.
// ---------------------------------------------------------
//
CMsgBaseControl* CMsgFormComponent::Component( TInt aControlId ) const
    {
    TInt index = ComponentIndexFromId( aControlId );

    if ( index == KErrNotFound )
        {
        return NULL;
        }

    return (*iControls)[index];
    }

// ---------------------------------------------------------
// CMsgFormComponent::ComponentIndexFromId
//
// Returns control's array index by given control id aControlId.
// ---------------------------------------------------------
//
TInt CMsgFormComponent::ComponentIndexFromId( TInt aControlId ) const
    {
    return iControls->ComponentIndexFromId( aControlId );
    }

// ---------------------------------------------------------
// CMsgFormComponent::ControlFromPosition
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
CMsgBaseControl* CMsgFormComponent::ControlFromPosition( TPoint aPosition, TBool aEvaluateHitTest ) const
    {
    TInt componentCount = iControls->Count();

    for ( TInt index = 0; index < componentCount; index++ )
        {
        CMsgBaseControl* current = (*iControls)[index];
        if ( current->Rect().Contains( aPosition ) &&
             ( !aEvaluateHitTest ||
               ( !current->HitTest() ||
                 current->HitTest()->HitRegionContains( aPosition, *current ) ) ) )
            {
            return current;
            }
        }
    return NULL;
    }
#else
CMsgBaseControl* CMsgFormComponent::ControlFromPosition( TPoint /*aPosition*/, TBool /*aEvaluateHitTest*/ ) const
    {
    return NULL;
    }
#endif // RD_SCALABLE_UI_V2


// ---------------------------------------------------------
// CMsgFormComponent::AddControlL
//
// Adds a control aControl to the control array to position aIndex and sets
// control id for the control.
// ---------------------------------------------------------
//
void CMsgFormComponent::AddControlL(
    CMsgBaseControl* aControl,
    TInt aControlId,
    TInt aIndex /* = 0*/ )
    {
    aControl->SetControlId( aControlId );
    DoAddControlL( aControl, aIndex );
    }

// ---------------------------------------------------------
// CMsgFormComponent::RemoveControlL
//
// Removes a control from array by given control id aControlId and returns
// pointer to it. If the control cannot be found retuns NULL.
// ---------------------------------------------------------
//
CMsgBaseControl* CMsgFormComponent::RemoveControlL( TInt aControlId )
    {
    CMsgBaseControl* control = NULL;
    TInt index = ComponentIndexFromId( aControlId );

    if ( index != KErrNotFound )
        {
        control = (*iControls)[index];
        control->SetFocus( EFalse );
        iControls->Delete( index );

        if ( index <= iCurrentFocus )
            {
            iCurrentFocus--;
            }
        
        if ( iControls->Count() == 0 )
            {
            iCurrentFocus = ENoFocus;
            }
        else if ( iCurrentFocus < 0 )
            {
            iCurrentFocus = 0;
            }
        }

    return control;
    }

// ---------------------------------------------------------
// CMsgFormComponent::Margins
//
// Returns margins.
// ---------------------------------------------------------
//
TMargins CMsgFormComponent::Margins() const
    {
    return iMargins;
    }

// ---------------------------------------------------------
// CMsgFormComponent::CurrentLineRect
//
// Returns the current control rect.
// ---------------------------------------------------------
//
TRect CMsgFormComponent::CurrentLineRect()
    {
    __ASSERT_DEBUG( ( iCurrentFocus < iControls->Count() ) &&
        ( iCurrentFocus != ENoFocus ), Panic( EMsgFocusLost ) );

    return (*iControls)[iCurrentFocus]->CurrentLineRect();
    }

// ---------------------------------------------------------
// CMsgFormComponent::ChangeFocusTo
//
// Changes focus to a control whose index is aNewFocus. Returns EFalse if focus
// cannot be changed.
// ---------------------------------------------------------
//
TBool CMsgFormComponent::ChangeFocusTo( TInt aNewFocus, TDrawNow aDrawNow /*= EDrawNow*/ )
    {
    __ASSERT_DEBUG( ( aNewFocus < iControls->Count() ) &&
        ( aNewFocus >= 0 ), Panic( EMsgFocusLost ) );

    if ( (*iControls)[aNewFocus]->IsNonFocusing() )
        {
        return EFalse;
        }
        
    if ( aNewFocus != iCurrentFocus )
        {
        // Take focus off from the currently focused component
        SetFocus( EFalse, aDrawNow );
        iCurrentFocus = aNewFocus;
        }

    SetFocus( ETrue, aDrawNow );

    return ETrue;
    }

// ---------------------------------------------------------
// CMsgFormComponent::FocusedControl
//
// Returns a pointer to the focused control. If focus not set returns NULL.
// ---------------------------------------------------------
//
CMsgBaseControl* CMsgFormComponent::FocusedControl() const
    {
    TInt numberOfControls = iControls->Count();

    if ( ( iCurrentFocus != ENoFocus ) && ( iCurrentFocus < numberOfControls ) )
        {
        return (*iControls)[iCurrentFocus];
        }
    return NULL;
    }

// ---------------------------------------------------------
// CMsgFormComponent::FirstFocusableControl
//
// Finds the first focusable control and returns index of the found control
// or KErrNotFound if the focusable control cannot be found.
// ---------------------------------------------------------
//
TInt CMsgFormComponent::FirstFocusableControl( TInt aStart, TMsgFocusDirection aDirection )
    {
    TInt components = iControls->Count();
    TInt newFocus = aStart;

    while ( ( newFocus < components ) && ( newFocus >= 0 ) )
        {
        if ( (*iControls)[newFocus]->IsNonFocusing() )
            {
            newFocus += aDirection;
            }
        else
            {
            return newFocus;
            }
        }

    return KErrNotFound;
    }

// ---------------------------------------------------------
// CMsgFormComponent::NotifyControlsForEvent
//
// Notifies all the controls about a view event.
// ---------------------------------------------------------
//
void CMsgFormComponent::NotifyControlsForEvent( TMsgViewEvent aEvent, TInt aParam )
    {
    TInt componentCount = iControls->Count();

    for ( TInt i = 0; i < componentCount; i++ )
        {
        (*iControls)[i]->NotifyViewEvent( aEvent, aParam );
        }
    }

// ---------------------------------------------------------
// CMsgFormComponent::CurrentFocus
//
// Returns index of currently focused control.
// ---------------------------------------------------------
//
TInt CMsgFormComponent::CurrentFocus() const
    {
    return iCurrentFocus;
    }

// ---------------------------------------------------------
// CMsgFormComponent::ResetControls
//
// Reset all controls in this component.
// ---------------------------------------------------------
//
void CMsgFormComponent::ResetControls()
    {
    TInt componentCount = iControls->Count();

    for (TInt i = 0; i < componentCount; i++)
        {
        (*iControls)[i]->Reset();
        }
    }

// ---------------------------------------------------------
// CMsgFormComponent::VirtualHeight
//
// Return virtual height of controls.
// ---------------------------------------------------------
//
TInt CMsgFormComponent::VirtualHeight()
    {
    TInt numberOfControls = iControls->Count();
    TInt totalHeight = 0;
    CMsgBaseControl* ctrl;

    for ( TInt i = 0; i < numberOfControls; i++ )
        {
        ctrl = (*iControls)[i];
        totalHeight += ctrl->VirtualHeight();
        totalHeight += ctrl->DistanceFromComponentAbove();
        }

    return totalHeight + iMargins.iTop + iMargins.iBottom;
    }

// ---------------------------------------------------------
// CMsgFormComponent::VirtualExtension
//
// Returns total of pixels above each control' band. This function is
// needed for the scroll bar for estimating message form's position
// relative to screen.
// ---------------------------------------------------------
//
TInt CMsgFormComponent::VirtualExtension()
    {
    TInt numberOfControls = iControls->Count();
    TInt pos = 0;

    if ( iCurrentFocus < numberOfControls && 
         iCurrentFocus != ENoFocus )
        {
        TInt offset = MsgEditorCommons::MsgBaseLineOffset();
        
        CMsgBaseControl* ctrl;
        for ( TInt i = 0; i <= iCurrentFocus; i++ )
            {
            ctrl = (*iControls)[i];
            
            if ( ctrl->Position().iY > offset )
                {
                break;
                }

            pos += ctrl->DistanceFromComponentAbove();
            
            if ( ctrl->IsFocused() )
                {
                pos += ctrl->VirtualVisibleTop();
                
                TInt visiblePixels( ctrl->Position().iY + ctrl->Size().iHeight );
                visiblePixels -= offset;
                
                if ( ctrl->Position().iY < 0 &&
                     visiblePixels > 0 )
                    {
                    pos += ( ctrl->Size().iHeight - visiblePixels );
                    }
                }
            else
                {
                pos += ctrl->VirtualHeight();
                
                TInt visiblePixels( 0 );
                
                if ( ctrl->Position().iY < 0 )
                    {
                    visiblePixels = ctrl->Position().iY + ctrl->Size().iHeight - offset;
                    }
                else
                    {
                    visiblePixels = ctrl->Size().iHeight;
                    }
                
                // If control is partially visible on the screen remove
                // these pixels.
                if ( visiblePixels > 0 )
                    {
                    pos -= visiblePixels;
                    }
                }
            }
        }
    return pos;
    }

// ---------------------------------------------------------
// CMsgFormComponent::OfferKeyEventL
//
// Handles key event by passing the event to a focused control.
// ---------------------------------------------------------
//
TKeyResponse CMsgFormComponent::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    __ASSERT_DEBUG( (iCurrentFocus < iControls->Count() ) &&
        ( iCurrentFocus != ENoFocus ), Panic( EMsgFocusLost ) );

    return static_cast<CCoeControl*>( (*iControls)[iCurrentFocus] )
        ->OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// CMsgFormComponent::CountComponentControls
//
// Returns a number of controls.
// ---------------------------------------------------------
//
TInt CMsgFormComponent::CountComponentControls() const
    {
    return iControls->Count();
    }

// ---------------------------------------------------------
// CMsgFormComponent::ComponentControl
//
// Returns a control of index aIndex.
// ---------------------------------------------------------
//
CCoeControl* CMsgFormComponent::ComponentControl( TInt aIndex ) const
    {
    if ( aIndex < iControls->Count() )
        {
        return ( (*iControls)[aIndex] );
        }

    __ASSERT_DEBUG(EFalse, Panic(EMsgIncorrectComponentIndex));

    return NULL;
    }

// ---------------------------------------------------------
// CMsgFormComponent::SizeChanged
//
// Sets new position for all the controls.
//
// SetExtent function must be used for setting positions because when this
// is called the first time, positions are not set yet. Hence, SetPosition
// cannot be used because it assumes that positions are already set.
// ---------------------------------------------------------
//
void CMsgFormComponent::SizeChanged()
    {
    TPoint componentPosition(
        Position().iX + iMargins.iLeft,
        Position().iY + iMargins.iTop );
    CMsgBaseControl* component;
    TInt components( iControls->Count() );
    TSize componentSize;

    for ( TInt cc = 0; cc < components; cc++ )
        {
        component = (*iControls)[cc];
        componentPosition.iY += component->DistanceFromComponentAbove();
        componentSize = component->Size();
        // SetPosition cannot be used here
        component->SetExtent( componentPosition, componentSize );
        componentSize = component->Size();
        componentPosition.iY += componentSize.iHeight;
        }
    }

// ---------------------------------------------------------
// CMsgFormComponent::HandleResourceChange
// Handles a resource relative event
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CMsgFormComponent::HandleResourceChange( TInt aType )
	{
    CEikBorderedControl::HandleResourceChange( aType );
	if( aType == KEikDynamicLayoutVariantSwitch )
		{
        SizeChanged();
        }
	}

// ---------------------------------------------------------
// CMsgFormComponent::FocusChanged
//
// Sets focus off or on if this form component lost focus or gets focused.
// ---------------------------------------------------------
//
void CMsgFormComponent::FocusChanged( TDrawNow aDrawNow )
    {
    if ( iControls->Count() > 0 )
        {
        CMsgBaseControl* control = (*iControls)[iCurrentFocus];

        if ( !( control->IsNonFocusing() ) )
            {
            control->SetFocus( IsFocused(), aDrawNow );
            }
        }
    }

// ---------------------------------------------------------
// CMsgFormComponent::DoAddControlL
//
// Adds control aControl to the component array to position aIndex. If aIndex =
// the number components in the array or EMsgAppendControl, appends the control
// to end of the array. Panics is aIndex is incorrect.
// ---------------------------------------------------------
//
TInt CMsgFormComponent::DoAddControlL( CMsgBaseControl* aControl, TInt aIndex )
    {
    TInt controlId = aControl->ControlId();
    TInt componentCount = iControls->Count();

    __ASSERT_DEBUG( controlId != 0,
        Panic( EMsgControlIdNotSet ) );
    __ASSERT_DEBUG( iControls->ComponentIndexFromId( controlId ) == KErrNotFound,
        Panic( EMsgControlIdNotUnique ) );
    __ASSERT_DEBUG( ( aIndex >= EMsgAppendControl ) && ( aIndex <= componentCount ),
        Panic( EMsgIncorrectComponentIndex ) );

    TInt index = ( aIndex == EMsgAppendControl )
        ? componentCount
        : aIndex;

    if ( componentCount == 0 )
        {
        iCurrentFocus = 0;
        }
    else if ( index <= iCurrentFocus )
        {
        iCurrentFocus++;
        }

    if ( index == componentCount )
        {
        iControls->AppendL( aControl );
        }
    else if ( ( index >= EMsgFirstControl ) && ( index < componentCount ) )
        {
        iControls->InsertL( index, aControl );
        }
    else
        {
        __ASSERT_DEBUG( EFalse, Panic( EMsgIncorrectComponentIndex ) );
        }

    return controlId;
    }

// ---------------------------------------------------------
// CMsgFormComponent::CountMsgControls
//
// Returns the number of components.
// ---------------------------------------------------------
//
TInt CMsgFormComponent::CountMsgControls( ) const
    {
    return iControls->Count();    
    }
    
// ---------------------------------------------------------
// CMsgFormComponent::MsgControl
//
// Return a pointer to a control by given control id aControlId. If the
// control cannot be found returns NULL.
// ---------------------------------------------------------
//
CMsgBaseControl* CMsgFormComponent::MsgControl( TInt aIndex ) const
    {
    
    if( aIndex >= iControls->Count( ) || aIndex < 0 )
        {
        return NULL;
        }
    
    return (*iControls)[aIndex];
    }


//  End of File
