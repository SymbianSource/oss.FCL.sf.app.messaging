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
* Description:  MsgBaseControl implementation
*
*/



// ========== INCLUDE FILES ================================

#include <eikenv.h>                        // for CEikonEnv
#include <AknUtils.h>                      // for AknUtils
#include <aknenv.h>                        // for CAknEnv

#include <applayout.cdl.h>

#include "MsgEditorCommon.h"               //
#include "MsgBaseControl.h"                // for CMsgBaseControl
#include "MsgBaseControlObserver.h"        // for MMsgBaseControlObserver
#include "MsgEditorPanic.h"                // for CMsgEditor panics

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CMsgBaseControl::CMsgBaseControl
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgBaseControl::CMsgBaseControl( MMsgBaseControlObserver& aBaseControlObserver ) : 
    iBaseControlObserver( &aBaseControlObserver ),
    iLineHeight( MsgEditorCommons::MsgBaseLineDelta() ),
    iMaxBodyHeight( MsgEditorCommons::MaxBodyHeight() )
    {
    }

// ---------------------------------------------------------
// CMsgBaseControl::CMsgBaseControl
//
// Constructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgBaseControl::CMsgBaseControl() :
    iLineHeight( MsgEditorCommons::MsgBaseLineDelta() ),
    iMaxBodyHeight( MsgEditorCommons::MaxBodyHeight() )
    {
    }

// ---------------------------------------------------------
// CMsgBaseControl::~CMsgBaseControl
//
// Destructor.
// ---------------------------------------------------------
//
EXPORT_C CMsgBaseControl::~CMsgBaseControl()
    {
    }

// ---------------------------------------------------------
// CMsgBaseControl::ConstructFromResourceL
//
// nop
// ---------------------------------------------------------
//
EXPORT_C void CMsgBaseControl::ConstructFromResourceL( TInt /*aResourceId*/ )
    {
    }

// ---------------------------------------------------------
// CMsgBaseControl::NotifyViewEvent
//
// nop
// ---------------------------------------------------------
//
EXPORT_C void CMsgBaseControl::NotifyViewEvent( TMsgViewEvent /*aEvent*/, 
                                                TInt /*aParam*/ )
    {
    }

// ---------------------------------------------------------
// CMsgBaseControl::Reset
//
// nop
// ---------------------------------------------------------
//
EXPORT_C void CMsgBaseControl::Reset()
    {
    }

// ---------------------------------------------------------
// CMsgBaseControl::SetModified
//
// Sets the modified flag of the control.
// ---------------------------------------------------------
//
EXPORT_C void CMsgBaseControl::SetModified( TBool aModified )
    {
    if ( aModified )
        {
        iControlModeFlags |= EMsgControlModeModified;
        }
    else
        {
        iControlModeFlags &= ~EMsgControlModeModified;
        }
    }

// ---------------------------------------------------------
// CMsgBaseControl::IsModified
//
// Return ETrue if contents of control has been modified.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgBaseControl::IsModified() const
    {
    return iControlModeFlags & EMsgControlModeModified;
    }

// ---------------------------------------------------------
// CMsgBaseControl::SetReadOnly
//
// Sets the control read only or not. Before setting the read only flag on or
// off, calls virtual PrepareForReadOnly function.
// ---------------------------------------------------------
//
EXPORT_C void CMsgBaseControl::SetReadOnly( TBool aReadOnly )
    {
    PrepareForReadOnly( aReadOnly );

    if ( aReadOnly )
        {
        iControlModeFlags |= EMsgControlModeReadOnly;
        }
    else
        {
        iControlModeFlags &= ~EMsgControlModeReadOnly;
        }
    }

// ---------------------------------------------------------
// CMsgBaseControl::IsReadOnly
//
// Returns the read only status of the control.
// ---------------------------------------------------------
//
EXPORT_C TBool CMsgBaseControl::IsReadOnly() const
    {
    return iControlModeFlags & EMsgControlModeReadOnly;
    }

// ---------------------------------------------------------
// CMsgBaseControl::ControlId
//
// Returns the control id.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgBaseControl::ControlId() const
    {
    return iControlId;
    }

// ---------------------------------------------------------
// CMsgBaseControl::SetControlId
//
// Sets the control id.
// ---------------------------------------------------------
//
EXPORT_C void CMsgBaseControl::SetControlId( TInt aControlId )
    {
    iControlId = aControlId;
    }

// ---------------------------------------------------------
// CMsgBaseControl::DistanceFromComponentAbove
//
// Returns a distance from above to this control in pixels.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgBaseControl::DistanceFromComponentAbove() const
    {
    return iDistanceFromComponentAbove;
    }

// ---------------------------------------------------------
// CMsgBaseControl::SetDistanceFromComponentAbove
//
// Sets a distance from above control in pixels.
// ---------------------------------------------------------
//
EXPORT_C void CMsgBaseControl::SetDistanceFromComponentAbove( TInt aDistance )
    {
    iDistanceFromComponentAbove = aDistance;
    }

// ---------------------------------------------------------
// CMsgBaseControl::Margins
//
// Returns the margins of the control.
// ---------------------------------------------------------
//
EXPORT_C TMargins CMsgBaseControl::Margins() const
    {
    return iMargins;
    }

// ---------------------------------------------------------
// CMsgBaseControl::SetMargins
//
// Sets margins for the control.
// ---------------------------------------------------------
//
EXPORT_C void CMsgBaseControl::SetMargins( const TMargins& aMargins )
    {
    iMargins = aMargins;
    }

// ---------------------------------------------------------
// CMsgBaseControl::SetBaseControlObserver
//
// Sets base control observer.
// ---------------------------------------------------------
//
EXPORT_C void CMsgBaseControl::SetBaseControlObserver( 
                                    MMsgBaseControlObserver& aBaseControlObserver )
    {
    iBaseControlObserver = &aBaseControlObserver;
    }

// ---------------------------------------------------------
// CMsgBaseControl::VirtualHeight
//
// Returns approximate height of the control.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgBaseControl::VirtualHeight()
    {
    return iSize.iHeight;
    }

// ---------------------------------------------------------
// CMsgBaseControl::VirtualVisibleTop
//
// Returns a topmost visible text position.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgBaseControl::VirtualVisibleTop()
    {
    return 0;
    }

// ---------------------------------------------------------
// CMsgBaseControl::BaseConstructL
//
// Sets default margins for the control.
// ---------------------------------------------------------
//
EXPORT_C void CMsgBaseControl::BaseConstructL()
    {
    iMargins.iLeft = 0;
    iMargins.iRight = 0;
    iMargins.iTop = 0;
    iMargins.iBottom = 0;
    }

// ---------------------------------------------------------
// CMsgBaseControl::NormalTextFontL
//
// Returns a pointer to the normal text font.
// ---------------------------------------------------------
//
EXPORT_C const CFont* CMsgBaseControl::NormalTextFontL() const
    {
    //TODO: Should this be read from LAF?
    const CFont* font = iEikonEnv->NormalFont();
    return font;
    }

// ---------------------------------------------------------
// CMsgBaseControl::LabelFontL
//
// Returns a pointer to the normal label font.
// ---------------------------------------------------------
//
EXPORT_C const CFont* CMsgBaseControl::LabelFontL() const
    {
    TRect msgPane = MsgEditorCommons::MsgMainPane();
    TAknLayoutText labelLayout;
    labelLayout.LayoutText(
        msgPane,
        AppLayout::Message_writing_texts_Line_1( 0 ) );
    return labelLayout.Font();
    }

// ---------------------------------------------------------
// CMsgBaseControl::CountComponentControls
//
// Returns a number of controls.
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgBaseControl::CountComponentControls() const
    {
    __ASSERT_DEBUG( EFalse, Panic( EMsgInheritedFunctionIsMissing ) );
    return 0;
    }

// ---------------------------------------------------------
// CMsgBaseControl::ComponentControl
//
// Returns a control of index aIndex.
// ---------------------------------------------------------
//
EXPORT_C CCoeControl* CMsgBaseControl::ComponentControl( TInt /*aIndex*/ ) const
    {
    __ASSERT_DEBUG( EFalse, Panic( EMsgInheritedFunctionIsMissing ) );
    return NULL;
    }

// ---------------------------------------------------------
// CMsgBaseControl::SizeChanged
//
// nop
// ---------------------------------------------------------
//
EXPORT_C void CMsgBaseControl::SizeChanged()
    {
    __ASSERT_DEBUG( EFalse, Panic( EMsgInheritedFunctionIsMissing ) );
    }

// ---------------------------------------------------------
// CMsgBaseControl::FocusChanged
//
// nop
// ---------------------------------------------------------
//
EXPORT_C void CMsgBaseControl::FocusChanged( TDrawNow /*aDrawNow*/ )
    {
    __ASSERT_DEBUG( EFalse, Panic( EMsgInheritedFunctionIsMissing ) );
    }

// ---------------------------------------------------------
// CMsgBaseControl::ItemFinder
//
// nop
// ---------------------------------------------------------
//
EXPORT_C CItemFinder* CMsgBaseControl::ItemFinder()
	{
	return NULL;
	}
	
// ---------------------------------------------------------
// CMsgBaseControl::SetupAutomaticFindAfterFocusChangeL
//
// nop
// ---------------------------------------------------------
//
EXPORT_C void CMsgBaseControl::SetupAutomaticFindAfterFocusChangeL( TBool /*aBeginning */ )
    {
    }

// ---------------------------------------------------------
// CMsgBaseControl::ScrollL
//
// nop
// ---------------------------------------------------------
//
EXPORT_C TInt CMsgBaseControl::ScrollL( TInt /*aPixelsToScroll*/, 
                                        TMsgScrollDirection /*aDirection*/ )
    {
    return 0;
    }

// ---------------------------------------------------------
// CMsgBaseControl::HandleResourceChange
//
// nop
// ---------------------------------------------------------
//
EXPORT_C void CMsgBaseControl::HandleResourceChange( TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        iLineHeight = MsgEditorCommons::MsgBaseLineDelta();
        iMaxBodyHeight = MsgEditorCommons::MaxBodyHeight();
        iSize.iWidth++; // for point out this control's size has changed
        }
    
    CCoeControl::HandleResourceChange( aType );
    }

//  End of File
