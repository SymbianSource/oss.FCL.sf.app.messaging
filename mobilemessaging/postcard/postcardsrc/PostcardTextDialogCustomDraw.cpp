/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  PostcardTextDialogCustomDraw implementation
*
*/



// ========== INCLUDE FILES ================================
#include "PostcardTextDialogCustomDraw.h"
#include "PostcardTextDialog.h"
#include "PostcardLaf.h"

#include <gulicon.h>
#include <AknUtils.h>                      
#include <eikedwin.h>

#include <AknsUtils.h>
#include <applayout.cdl.h>                 
#include <aknlayoutscalable_apps.cdl.h>

#include <msgeditor.mbg>

#include <MsgEditorCommon.h>

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
// CPostcardTextDialogCustomDraw::NewL
// ---------------------------------------------------------
//
CPostcardTextDialogCustomDraw* CPostcardTextDialogCustomDraw::NewL(
        const MFormCustomDraw* aParentCustomDraw,
        const CPostcardTextDialog* aParentControl )
    {
    CPostcardTextDialogCustomDraw* self = new (ELeave) CPostcardTextDialogCustomDraw( aParentCustomDraw,
                                                                    aParentControl );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CPostcardTextDialogCustomDraw::ConstructL
// ---------------------------------------------------------
//
void CPostcardTextDialogCustomDraw::ConstructL()
    {
    iIcon = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(),
                                       KAknsIIDQgnGrafLineMessageHorizontal,
                                       KMsgEditorMbm,
                                       EMbmMsgeditorQgn_graf_line_message_horizontal,
                                       EMbmMsgeditorQgn_graf_line_message_horizontal_mask );
    
    ResolveLayouts();
    }


// ---------------------------------------------------------
// CPostcardTextDialogCustomDraw::CPostcardTextDialogCustomDraw
// ---------------------------------------------------------
//
CPostcardTextDialogCustomDraw::CPostcardTextDialogCustomDraw( 
        const MFormCustomDraw* aParentCustomDraw,
        const CPostcardTextDialog* aParentControl ) :
    iParentCustomDraw( aParentCustomDraw ),
    iParentControl( aParentControl )
    {
    }

// ---------------------------------------------------------
// CPostcardTextDialogCustomDraw::~CPostcardTextDialogCustomDraw
// ---------------------------------------------------------
//
CPostcardTextDialogCustomDraw::~CPostcardTextDialogCustomDraw()
    {
    delete iIcon;
    }

// ---------------------------------------------------------
// CPostcardTextDialogCustomDraw::ResolveLayouts
// ---------------------------------------------------------
//
void CPostcardTextDialogCustomDraw::ResolveLayouts()
    {
    TRect dataPane = MsgEditorCommons::MsgDataPane();

    TAknLayoutRect msgTextPane;
    msgTextPane.LayoutRect( dataPane,
                            AknLayoutScalable_Apps::msg_text_pane( 0 ).LayoutLine() );
        
    TAknLayoutRect msgBodyPane;
    msgBodyPane.LayoutRect( msgTextPane.Rect(),
                            AknLayoutScalable_Apps::msg_body_pane().LayoutLine() );
        
    TAknLayoutRect lineLayout;
    lineLayout.LayoutRect( msgBodyPane.Rect(),
                           AknLayoutScalable_Apps::msg_body_pane_g1().LayoutLine() );
    
    AknIconUtils::SetSize( iIcon->Bitmap(), 
                           lineLayout.Rect().Size(), 
                           EAspectRatioNotPreserved );
    
    iLineDelta = MsgEditorCommons::MsgBaseLineDelta();
    
    // Calculate first line rectangle's correct position.
    iFirstLineRect = TRect( TPoint( lineLayout.Rect().iTl.iX,
                                    iLineDelta - lineLayout.Rect().Height() ),
                                    lineLayout.Rect().Size() );
    }

// ---------------------------------------------------------
// CPostcardTextDialogCustomDraw::DrawBackground
// ---------------------------------------------------------
//
void CPostcardTextDialogCustomDraw::DrawBackground( const TParam& aParam,
                                           const TRgb& aRgb,
                                           TRect& aDrawn ) const 
    {
    iParentCustomDraw->DrawBackground( aParam, aRgb, aDrawn );
    // Draw lines only in editor. CustomDraw object is created 
    // only when a postcard is in editor state.
    DrawEditorLines( aParam );
    }

// ---------------------------------------------------------
// CPostcardTextDialogCustomDraw::DrawBackground
// ---------------------------------------------------------
//
void CPostcardTextDialogCustomDraw::DrawLineGraphics( const TParam& aParam, 
                                             const TLineInfo& aLineInfo ) const
    {
    iParentCustomDraw->DrawLineGraphics( aParam, aLineInfo );
    }

// ---------------------------------------------------------
// CPostcardTextDialogCustomDraw::DrawText
// ---------------------------------------------------------
//
void CPostcardTextDialogCustomDraw::DrawText( const TParam& aParam,
                                     const TLineInfo& aLineInfo,
                                     const TCharFormat& aFormat,
                                     const TDesC& aText,
                                     const TPoint& aTextOrigin,
                                     TInt aExtraPixels ) const
    {
    iParentCustomDraw->DrawText( aParam, 
                                 aLineInfo, 
                                 aFormat, 
                                 aText, 
                                 aTextOrigin, 
                                 aExtraPixels );
    }
    
// ---------------------------------------------------------
// CPostcardTextDialogCustomDraw::DrawText
// ---------------------------------------------------------
//
TRgb CPostcardTextDialogCustomDraw::SystemColor( TUint aColorIndex, TRgb aDefaultColor ) const
    {
    return iParentCustomDraw->SystemColor( aColorIndex, aDefaultColor );
    }

// ---------------------------------------------------------
// CPostcardTextDialogCustomDraw::DrawEditorLines
// ---------------------------------------------------------
//
void CPostcardTextDialogCustomDraw::DrawEditorLines( const TParam& aParam ) const
    {
    TRect currentRect( iFirstLineRect );
    
    while ( currentRect.iBr.iY <= aParam.iDrawRect.iBr.iY )
        {
        if ( currentRect.iTl.iY >= aParam.iDrawRect.iTl.iY  )
            {
            aParam.iGc.DrawBitmapMasked( currentRect,
                                         iIcon->Bitmap(), 
                                         TRect( TPoint( 0 ,0 ), currentRect.Size() ), 
                                         iIcon->Mask(),
                                         ETrue );
            }
            
        currentRect.Move( 0, iLineDelta );
        }
    } 

//  End of File
