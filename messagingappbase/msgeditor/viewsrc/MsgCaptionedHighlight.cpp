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
* Description:  MsgCaptionedHighlight implementation
*
*/



// ========== INCLUDE FILES ================================

#include <barsread.h>                      // for TResourceReader
#include <eiklabel.h>                      // for CEikLabel
#include <eikenv.h>                        // for CEikonEnv
#include <eikedwin.h>                      // for TClipboardFunc
#include <txtrich.h>                       // for CRichText
#include <AknUtils.h>                      // for AknUtils
#include <aknenv.h>                        // for CAknEnv
#include <AknDef.h>
#include <AknLayoutFont.h>
#include <applayout.cdl.h> // LAF
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include <AknsDrawUtils.h>                 // for Skinned drawing
#include <AknsBasicBackgroundControlContext.h>					

#include "MsgEditorCommon.h"
#include "MsgBaseControl.h"
#include "MsgHeader.h"
#include "MsgCaptionedHighlight.h"
#include "MsgExpandableControl.h"

// ========== LOCAL CONSTANTS AND MACROS ===================

_LIT( KMsgEditIndicator, ":" );

// ---------------------------------------------------------
// CMsgCaptionedHighlight::CMsgCaptionedHighlight
// ---------------------------------------------------------
//
CMsgCaptionedHighlight* CMsgCaptionedHighlight::NewL( const CMsgHeader& aParent )
    {
    CMsgCaptionedHighlight* self = new ( ELeave )
        CMsgCaptionedHighlight( aParent );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CMsgCaptionedHighlight::HandleResourceChange
//
// Updates layout related parameter when layout is changed and
// skinned text color when skin is changed.
// ---------------------------------------------------------
//
void CMsgCaptionedHighlight::HandleResourceChange( TInt aType )
    {
	if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        ResolveLayouts();
        }
    else if ( aType == KAknsMessageSkinChange )
        {
        UpdateTextColor();
        }
        
    CCoeControl::HandleResourceChange( aType );
    }

// ---------------------------------------------------------
// CMsgCaptionedHighlight::CMsgCaptionedHighlight
// ---------------------------------------------------------
//
CMsgCaptionedHighlight::CMsgCaptionedHighlight( const CMsgHeader& aParent )
    : iParent( aParent )
    {
    }

// ---------------------------------------------------------
// CMsgCaptionedHighlight::ConstructL
//
// Initializes layout related parameters and skin text color.
// Connects the control to parent control and sets it ready
// to be drawn on the screen.
// ---------------------------------------------------------
//
void CMsgCaptionedHighlight::ConstructL()
    {
    ResolveLayouts();
    UpdateTextColor();
    SetContainerWindowL( iParent );
    ActivateL();
    }

// ---------------------------------------------------------
// CMsgCaptionedHighlight::ResolveLayouts
// ---------------------------------------------------------
//
void CMsgCaptionedHighlight::ResolveLayouts()
    {    
    TRect dataPane = MsgEditorCommons::MsgDataPane();
    TAknLayoutRect msgTextPane;
    msgTextPane.LayoutRect(
        dataPane,
        AknLayoutScalable_Apps::msg_text_pane( 0 ).LayoutLine() );
    TAknLayoutRect msgHeaderPane;
    msgHeaderPane.LayoutRect(
        msgTextPane.Rect(),
        AknLayoutScalable_Apps::msg_header_pane().LayoutLine() );
    iFrame.LayoutRect(
        msgHeaderPane.Rect(),
        AknLayoutScalable_Apps::input_focus_pane_cp07().LayoutLine() );
    iText.LayoutText(
        msgHeaderPane.Rect(),
        AknLayoutScalable_Apps::msg_header_pane_t1( 0 ).LayoutLine() );
    iVertBar.LayoutRect(
        dataPane,
        AknLayoutScalable_Avkon::bg_list_pane_g4( 2 ).LayoutLine() );
    
    iBaseLineOffset = MsgEditorCommons::MsgBaseLineOffset();
    
    const CAknLayoutFont* layoutFont = CAknLayoutFont::AsCAknLayoutFontOrNull( iText.Font() );
    
    if ( layoutFont )
        {
        iTextBaselineOffset =  layoutFont->TextPaneTopToBaseline();
        }
    else
        {
        iTextBaselineOffset = ( ( iText.Font()->HeightInPixels() + iBaseLineOffset ) / 2 ) - 
                                iText.Font()->DescentInPixels();
        }
    
    // Gap between the header control and text editor control.
    iTextGap = iText.TextRect().iTl.iY - msgHeaderPane.Rect().iTl.iY;
    }


// ---------------------------------------------------------
// CMsgCaptionedHighlight::Draw
// ---------------------------------------------------------
//
EXPORT_C void CMsgCaptionedHighlight::Draw(const TRect& /*aRect*/) const
    {
    CWindowGc& gc = SystemGc();

    TRect rect( iParent.Rect() );
    TRect headerRect( iFrame.Rect().iTl.iX,
                      iParent.Rect().iTl.iY,
                      iFrame.Rect().iBr.iX,
                      iParent.Rect().iBr.iY );

    // clear top (0,0,176,6) area and draw top part of vertical bar.
    TRect clearRect( 0, 0, rect.iBr.iX, iBaseLineOffset );
    
    if( !AknsDrawUtils::Background( AknsUtils::SkinInstance(), 
                                    AknsDrawUtils::ControlContext( this ), 
                                    this, 
                                    gc, 
                                    clearRect ) )
        {        
    	gc.SetBrushColor( AKN_LAF_COLOR( 0 ) );
		gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
		gc.SetPenStyle( CGraphicsContext::ENullPen );
		gc.DrawRect( clearRect );
		}

    // Reset default brush & pen styles
    gc.SetBrushStyle( CGraphicsContext::ENullBrush );
    gc.SetPenStyle( CGraphicsContext::ESolidPen );

    CMsgBaseControl* focused = iParent.FocusedControl();
    if ( focused && 
         focused->IsFocused() && 
         !focused->IsReadOnly() &&
         focused->Rect().iBr.iY > clearRect.iBr.iY &&
         focused->Rect().iTl.iY < MsgEditorCommons::MsgHeaderPane().Height() )
        {
        TRect focusedRect(
            headerRect.iTl.iX,
            focused->Rect().iTl.iY,
            headerRect.iBr.iX,
            focused->Rect().iBr.iY );
            DrawFrame( gc, focusedRect );
        }    
    }

// ---------------------------------------------------------
// CMsgCaptionedHighlight::DrawFrame
//
// Draws the focus highlight around currently focused field in editor.
// Also performs colon drawing if button is not shown in the
// field instead of caption (as is the case when touch screen
// is present). Colon is not drawn either if the control's top 
// position is not fully shown (i.e. colon is not shown if tall
// control is scrolled so that first line is not shown). 
// Colon is drawn with the text color from current skin and it is
// drawn after focus highlight so that it would seem to appear
// above highlight. 
// ---------------------------------------------------------
//
void CMsgCaptionedHighlight::DrawFrame( CWindowGc& aGc, const TRect& aRect ) const
    {
    TRect drawRect( aRect );
    TBool drawColon = ETrue;
    
    CMsgExpandableControl* focusedControl = 
                        static_cast<CMsgExpandableControl*>( iParent.FocusedControl() );
                        
    if ( focusedControl &&
         focusedControl->Button() )
        {
        drawColon = EFalse;
        }
    
    if ( drawRect.iTl.iY < iBaseLineOffset )
        {
        drawRect.iTl.iY = iBaseLineOffset;
        drawColon = EFalse; // the control is not wholly in the screen
        }
        
    TInt maxBodyHeight( MsgEditorCommons::EditorViewHeigth() );
    if ( drawRect.iBr.iY > maxBodyHeight )
        {
        drawRect.iBr.iY = maxBodyHeight;
        }
        
    TAknLayoutRect tl;
    tl.LayoutRect( drawRect, AknLayoutScalable_Avkon::input_focus_pane_g2().LayoutLine() );
    
    TAknLayoutRect br;
    br.LayoutRect( drawRect, AknLayoutScalable_Avkon::input_focus_pane_g5().LayoutLine() );

    TRect innerRect = TRect(
        drawRect.iTl.iX + tl.Rect().Width(),
        drawRect.iTl.iY + tl.Rect().Height(),
        drawRect.iBr.iX - br.Rect().Width(),
        drawRect.iBr.iY - br.Rect().Height() );

  
        
    // Draw ':' for edit indicator.
    if( drawColon )
        {
        TPoint point( iVertBar.Rect().iTl.iX, drawRect.iTl.iY + iTextBaselineOffset + iTextGap );
        if ( point.iY > drawRect.iTl.iY )
            {
            aGc.SetPenColor( iTextColor );
            aGc.UseFont( iText.Font() );
            aGc.DrawText( KMsgEditIndicator, point );
            }
        }
    }


// ---------------------------------------------------------
// CMsgCaptionedHighlight::UpdateTextColor
//
// Text color used to draw colon is skinned. 
// It uses the same text color from skin as caption.
// ---------------------------------------------------------
//
void CMsgCaptionedHighlight::UpdateTextColor()
    {
    TRgb textColor = iText.Color();
    if ( AknsUtils::GetCachedColor( AknsUtils::SkinInstance(),
                                    textColor,
                                    KAknsIIDQsnTextColors,
                                    EAknsCIQsnTextColorsCG6 ) != KErrNone )
        {
        textColor = AKN_LAF_COLOR_STATIC( 215 );
        }

    iTextColor = textColor;
    }

//  End of File
