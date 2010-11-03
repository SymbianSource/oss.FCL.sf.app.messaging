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
*       MsgEditor Frame control.
*
*/



// INCLUDE FILES
#include "MsgFrameControl.h"

#include <coeutils.h>               // for ConeUtils::FileExists
#include <MsgEditorCommon.h>

#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>

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
// CMsgFrameControl::CMsgFrameControl
//
// Constructor
// ---------------------------------------------------------
//
CMsgFrameControl::CMsgFrameControl()
    {
    }


// ---------------------------------------------------------
// CMsgFrameControl::NewL
//
// Constructor
// ---------------------------------------------------------
//
CMsgFrameControl* CMsgFrameControl::NewL( const CCoeControl& aParent )
    {
    CMsgFrameControl* self = new( ELeave ) CMsgFrameControl;
    
    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CMsgFrameControl::ConstructL
//
// Constructor
// ---------------------------------------------------------
//
void CMsgFrameControl::ConstructL( const CCoeControl& aParent )
    {
    SetContainerWindowL( aParent );
        
    SetFrameSize();
    
    MakeVisible( EFalse );
    }

// ---------------------------------------------------------
// CMsgFrameControl::~CMsgFrameControl
//
// Destructor
// ---------------------------------------------------------
//
CMsgFrameControl::~CMsgFrameControl()
    {
    }

// ---------------------------------------------------------
// CMsgFrameControl::SetImageSize
//
// ---------------------------------------------------------
//
void CMsgFrameControl::SetImageSize( const TSize& aSize )
    {
    iFrameSize = aSize + iFrameBorderSize;
    iImageSizeSet = ETrue;
    }

// ---------------------------------------------------------
// CMsgFrameControl::FrameSize
//
// ---------------------------------------------------------
//
TSize CMsgFrameControl::FrameSize() const
    {
    return iFrameSize;
    }

// ---------------------------------------------------------
// CMsgFrameControl::Draw
//
// ---------------------------------------------------------
//
void CMsgFrameControl::Draw( const TRect& aRect ) const
    {
    if ( !iImageSizeSet )
        {
        return;
        }

    CWindowGc& gc = SystemGc();
    gc.SetClippingRect( aRect );

    // Center the actual "frameRect" inside the "Rect()"
    TRect frameRect( Rect().iTl, iFrameSize );
    frameRect.Move( ( Rect().Width() - iFrameSize.iWidth ) / 2,
                    ( Rect().Height() - iFrameSize.iHeight ) / 2 );

    TAknLayoutRect tl;
    tl.LayoutRect( frameRect, AknLayoutScalable_Avkon::input_focus_pane_g2().LayoutLine() );
    
    TAknLayoutRect br;
    br.LayoutRect( frameRect, AknLayoutScalable_Avkon::input_focus_pane_g5().LayoutLine() );
    
    TRect innerRect = TRect( frameRect.iTl.iX + tl.Rect().Width(),
                             frameRect.iTl.iY + tl.Rect().Height(),
                             frameRect.iBr.iX - br.Rect().Width(),
                             frameRect.iBr.iY - br.Rect().Height() );

    TBool drawn = AknsDrawUtils::DrawFrame( AknsUtils::SkinInstance(),
                                            gc,
                                            frameRect,
                                            innerRect, 
                                            KAknsIIDQsnFrInput, 
                                            KAknsIIDNone );        
    if ( !drawn )
        {
        gc.SetPenColor( KRgbGray );
        TRect rect = frameRect;
        rect.iTl.iX += 1;
        rect.iTl.iY += 1;
        gc.DrawRect( rect );
        gc.SetPenColor( KRgbBlack );
        rect.iTl.iX -= 1;
        rect.iTl.iY -= 1;
        rect.iBr.iX -= 1;
        rect.iBr.iY -= 1;
        gc.DrawRect( rect );
        }
    }

// ---------------------------------------------------------
// CMsgFrameControl::HandleResourceChange
// ---------------------------------------------------------
//
void CMsgFrameControl::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType ); 
    
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetFrameSize();
        }
    }

// ---------------------------------------------------------
// CMsgFrameControl::SetFrameSize
// ---------------------------------------------------------
//
void CMsgFrameControl::SetFrameSize()
    {
    TRect dataPane = MsgEditorCommons::MsgDataPane();
    TAknLayoutRect iconLayout;
    iconLayout.LayoutRect( dataPane,
                           AknLayoutScalable_Apps::msg_data_pane_g4().LayoutLine() );
                           
    TAknLayoutRect frameLayout;
    frameLayout.LayoutRect( dataPane,
                            AknLayoutScalable_Apps::qrid_highlight_pane_cp011( 0 ).LayoutLine() );
    iFrameBorderSize = TPoint( iconLayout.Rect().iTl - frameLayout.Rect().iTl ).AsSize();
    // "Multiply" by two:
    iFrameBorderSize += iFrameBorderSize;
    }

//  End of File
