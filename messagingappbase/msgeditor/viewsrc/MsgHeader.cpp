/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsgHeader implementation
*
*/



// ========== INCLUDE FILES ================================

#include <eikenv.h>                        // for CEikonEnv
#include <gulutil.h>                       // for ColorUtils
#include <AknUtils.h>                      // for AknUtils

#include <applayout.cdl.h>              // LAF
#include <AknsDrawUtils.h>                 // for Skinned drawing

#include "MsgEditorCommon.h"               //
#include "MsgHeader.h"                     // for CMsgHeader
#include "MsgCaptionedHighlight.h"
#include "MsgDummyHeaderCleaner.h"  
#include "MsgControlArray.h"               // for CMsgControlArray
#include "MsgExpandableControl.h"
#include "MsgEditorPanic.h"                // for CMsgEditor panics
#include "MsgEditorLogging.h"

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
// CMsgHeader::CMsgHeader
//
// Constructor.
// ---------------------------------------------------------
//
CMsgHeader::CMsgHeader( const TMargins& aMargins )
    : CMsgFormComponent( aMargins )
    {
    }

// ---------------------------------------------------------
// CMsgHeader::~CMsgHeader
//
// Destructor.
// ---------------------------------------------------------
//
CMsgHeader::~CMsgHeader()
    {
    AknsUtils::DeregisterControlPosition( iFrame );
    delete iFrame;
    delete iHeaderCleaner;
    AknsUtils::DeregisterControlPosition( this );
    }

// ---------------------------------------------------------
// CMsgHeader::NewL
//
// Factory method that creates this control.
// ---------------------------------------------------------
//
CMsgHeader* CMsgHeader::NewL( const CCoeControl& aParent, const TMargins& aMargins )
    {
    CMsgHeader* self = new ( ELeave ) CMsgHeader( aMargins );

    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// CMsgHeader::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CMsgHeader::ConstructL( const CCoeControl& aParent )
    {
    CMsgFormComponent::BaseConstructL( aParent );
    
    iFrame = CMsgCaptionedHighlight::NewL( *this );
    
    iHeaderCleaner = new (ELeave) CMsgDummyHeaderCleaner(this);
    iHeaderCleaner->ConstructL();
    
#ifdef RD_SCALABLE_UI_V2
    User::LeaveIfError( iFrame->SetHitTest( this ) );
    User::LeaveIfError( iHeaderCleaner->SetHitTest( this ) );
#endif // RD_SCALABLE_UI_V2
    }

// ---------------------------------------------------------
// CMsgHeader::SetAndGetSizeL
//
// Calculates and sets the size for the header and returns new size as reference
// aSize. If aInit == ETrue, sets also size for the controls by calling their
// SetAndGetSizeL functions.
// ---------------------------------------------------------
//
void CMsgHeader::SetAndGetSizeL( TSize& aSize, TBool aInit )
    {
    aSize = MsgEditorCommons::MsgHeaderPane().Size();

    TInt numberOfControls( iControls->Count() );
    iMargins.iTop = 0;
    iMargins.iBottom = numberOfControls ? MsgEditorCommons::MsgBaseLineDelta() : 
                                          0;
                                          
    TInt marginsDeltaHeight = iMargins.iTop + iMargins.iBottom;
    TInt marginsDeltaWidth = iMargins.iLeft + iMargins.iRight;
    TSize headerSize( aSize.iWidth, marginsDeltaHeight );
    TSize controlSize( 0, 0 );

    controlSize.iWidth = aSize.iWidth - marginsDeltaWidth;
    CMsgBaseControl* control = NULL;
    TInt minimumHeigth = marginsDeltaHeight;

    for ( TInt cc = 0; cc < numberOfControls; cc++ )
        {
        controlSize.iHeight = aSize.iHeight;
        control = (*iControls)[cc];

        if ( aInit )
            {
            control->SetAndGetSizeL( controlSize );
            }
        else
            {
            controlSize = control->Size();
            }

        headerSize.iHeight += control->DistanceFromComponentAbove() + controlSize.iHeight /* + 1 */;
        minimumHeigth += control->DistanceFromComponentAbove() + control->MinimumSize().iHeight /* + 1 */;

        //MEBLOGGER_WRITEF(_L("MEB: CMsgHeader::SetAndGetSizeL: headerSize.iHeight %d "), headerSize.iHeight);
        }

    if ( minimumHeigth < marginsDeltaHeight )
        // make top margin higher if there is no space for the icon
        {
        TInt additionalTopMargin = marginsDeltaHeight - minimumHeigth;
        headerSize.iHeight += additionalTopMargin;
        iMargins.iTop += additionalTopMargin;
        }

    SetSizeWithoutNotification( headerSize );
    aSize = headerSize;
    }

// ---------------------------------------------------------
// CMsgHeader::CountComponentControls
//
// Returns a number of controls.
// ---------------------------------------------------------
//
TInt CMsgHeader::CountComponentControls() const
    {
    // Component controls + iFrame + iHeaderCleaner
    return ( CMsgFormComponent::CountComponentControls() + 1 + 1);
    }

// ---------------------------------------------------------
// CMsgHeader::ComponentControl
//
// Returns a control of index aIndex.
// ---------------------------------------------------------
//
CCoeControl* CMsgHeader::ComponentControl( TInt aIndex ) const
    {
    // The order of component is important as now the frame gets drawn
    // before the editor edwins. In the end iHeaderCleaner 
    // cleans the top of the display
    TInt compCount = CMsgFormComponent::CountComponentControls();
    // Component controls + iFrame 
    __ASSERT_DEBUG( aIndex < compCount + 1 + 1,
        Panic( EMsgIncorrectComponentIndex ) );

    if ( aIndex == 0 )
        {
        return iFrame;
        }

    if ( aIndex <= compCount )
        {
        return CMsgFormComponent::ComponentControl( aIndex-1 );
        }
        
    if ( aIndex == compCount +1 )
        {
        return iHeaderCleaner;
        }

    return NULL;
    }

// ---------------------------------------------------------
// CMsgHeader::HitRegionContains
// 
// Note! CONE will do final verification if control is really hit so
//       not need to do it here.
// ---------------------------------------------------------
//
TBool CMsgHeader::HitRegionContains( const TPoint& /*aPoint*/, 
                                     const CCoeControl& /*aControl*/ ) const
    {
    return EFalse;
    }

// ---------------------------------------------------------
// CMsgHeader::SizeChanged
//
// Sets new position for all the controls.
// ---------------------------------------------------------
//
void CMsgHeader::SizeChanged()
    {
    MEBLOGGER_ENTERFN( "CMsgHeader::SizeChanged" );

    CMsgFormComponent::SizeChanged();
    TRect rect = Rect( );
	iFrame->SetRect( rect );
	iHeaderCleaner->SetRect( rect );
    AknsUtils::RegisterControlPosition( iFrame );
    AknsUtils::RegisterControlPosition( this );

    // Resize also the frame control around the focused editor
    CMsgBaseControl* ctrl = FocusedControl();
    if( ctrl )
        {
        switch( ctrl->ControlType( ) )
            { // HandleFrameSizeChange should be called only for expandable and address controls
            case EMsgExpandableControl:
            case EMsgAddressControl:
            case EMsgAttachmentControl:
                {
                (static_cast<CMsgExpandableControl*>(ctrl))->HandleFrameSizeChange( );    
                break;
                }
            default: // don't call it for example for body control as there's no frame
                break;
            }
        }

    MEBLOGGER_LEAVEFN( "CMsgHeader::SizeChanged" );
    }

// ---------------------------------------------------------
// CMsgFormComponent::FocusChanged
//
// Sets focus off or on if this form component lost focus or gets focused.
// ---------------------------------------------------------
//
void CMsgHeader::FocusChanged( TDrawNow /*aDrawNow*/ )
    {
    CMsgFormComponent::FocusChanged( ENoDrawNow );
    DrawNow();
    }


// ---------------------------------------------------------
// CMsgHeader::Draw
//
// Draws the header.
// ---------------------------------------------------------
//
void CMsgHeader::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    TRect rect( Rect() );

    if ( !AknsDrawUtils::Background( AknsUtils::SkinInstance(), 
                                     AknsDrawUtils::ControlContext( this ), 
                                     this, 
                                     gc, 
                                     rect ) )
        {
	    gc.SetBrushColor( AKN_LAF_COLOR( 0 ) );
		gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
		gc.SetPenStyle( CGraphicsContext::ENullPen );
		gc.DrawRect( rect );
		}
    }

//  End of File
