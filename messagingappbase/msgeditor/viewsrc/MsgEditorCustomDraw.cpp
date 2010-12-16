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
* Description:  MsgEditorCustomDraw implementation
*
*/



// ========== INCLUDE FILES ================================
#include "MsgEditorCustomDraw.h"

#include <gulicon.h>
#include <AknUtils.h>                      
#include <eikedwin.h>
#include "MsgEditor.hrh"                   // for typedefs

#include <AknsUtils.h>
#include <applayout.cdl.h>                 
#include <aknlayoutscalable_apps.cdl.h>

#include <msgeditor.mbg>

#include "MsgEditorCommon.h"

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
// CMsgEditorCustomDraw::NewL
// ---------------------------------------------------------
//
CMsgEditorCustomDraw* CMsgEditorCustomDraw::NewL( const MFormCustomDraw* aParentCustomDraw,
                                                  const CEikEdwin* aParentControl,const TInt aControlType  )
    {
    CMsgEditorCustomDraw* self = new (ELeave) CMsgEditorCustomDraw( aParentCustomDraw,
                                                                    aParentControl,aControlType );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CMsgEditorCustomDraw::ConstructL
// ---------------------------------------------------------
//
void CMsgEditorCustomDraw::ConstructL()
    {
    iIcon = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(),
                                       KAknsIIDQgnGrafLineMessageHorizontal,
                                       KMsgEditorMbm,
                                       EMbmMsgeditorQgn_graf_line_message_horizontal,
                                       EMbmMsgeditorQgn_graf_line_message_horizontal_mask );
    
    ResolveLayouts();
    }


// ---------------------------------------------------------
// CMsgEditorCustomDraw::CMsgEditorCustomDraw
// ---------------------------------------------------------
//
CMsgEditorCustomDraw::CMsgEditorCustomDraw( const MFormCustomDraw* aParentCustomDraw,
                                            const CEikEdwin* aParentControl,const TInt aControlType ) :
    iParentCustomDraw( aParentCustomDraw ),
    iParentControl( aParentControl ),
    iControlType(aControlType)
    {
    }

// ---------------------------------------------------------
// CMsgEditorCustomDraw::~CMsgEditorCustomDraw
// ---------------------------------------------------------
//
CMsgEditorCustomDraw::~CMsgEditorCustomDraw()
    {
    delete iIcon;
    }

// ---------------------------------------------------------
// CMsgEditorCustomDraw::ResolveLayouts
// ---------------------------------------------------------
//
void CMsgEditorCustomDraw::ResolveLayouts()
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

    TInt lineOffset = MsgEditorCommons::MsgBaseLineOffset();
    
    iLineDelta = MsgEditorCommons::MsgBaseLineDelta();
    
    // Calculate first line rectangle's correct position.
    iFirstLineRect = TRect( TPoint( lineLayout.Rect().iTl.iX,
                                    lineOffset + iLineDelta - lineLayout.Rect().Height() ),
                                    lineLayout.Rect().Size() );
    }

// ---------------------------------------------------------
// CMsgEditorCustomDraw::DrawBackground
// ---------------------------------------------------------
//
void CMsgEditorCustomDraw::DrawBackground( const TParam& aParam,
                                           const TRgb& aRgb,
                                           TRect& aDrawn ) const 
    {
    iParentCustomDraw->DrawBackground( aParam, aRgb, aDrawn );
    
    if ( !iParentControl->IsReadOnly() || iControlType == EMsgAttachmentControl)
        {
        DrawEditorLines( aParam );
        }
    }

// ---------------------------------------------------------
// CMsgEditorCustomDraw::DrawBackground
// ---------------------------------------------------------
//
void CMsgEditorCustomDraw::DrawLineGraphics( const TParam& aParam, 
                                             const TLineInfo& aLineInfo ) const
    {
    iParentCustomDraw->DrawLineGraphics( aParam, aLineInfo );
    }

// ---------------------------------------------------------
// CMsgEditorCustomDraw::DrawText
// ---------------------------------------------------------
//
void CMsgEditorCustomDraw::DrawText( const TParam& aParam,
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
    
// 107-24185 : Emoticon support for SMS and MMS

// -----------------------------------------------------------------------------
// CMsgEditorCustomDraw::DrawText
// -----------------------------------------------------------------------------
//
void CMsgEditorCustomDraw::DrawText( const TParam& aParam, 
    const TLineInfo& aLineInfo, const TCharFormat& aFormat, const TDesC& aText, 
    const TInt aStart, const TInt aEnd, const TPoint& aTextOrigin, 
    TInt aExtraPixels ) const
    {
    iParentCustomDraw->DrawText( aParam, aLineInfo, aFormat, 
        aText.Mid( aStart, aEnd - aStart ), aTextOrigin, aExtraPixels );
    }
    
// ---------------------------------------------------------
// CMsgEditorCustomDraw::DrawText
// ---------------------------------------------------------
//
TRgb CMsgEditorCustomDraw::SystemColor( TUint aColorIndex, TRgb aDefaultColor ) const
    {
    return iParentCustomDraw->SystemColor( aColorIndex, aDefaultColor );
    }

// ---------------------------------------------------------
// CMsgEditorCustomDraw::DrawEditorLines
// ---------------------------------------------------------
//
void CMsgEditorCustomDraw::DrawEditorLines( const TParam& aParam ) const
    {
    TRect currentRect( iFirstLineRect );
    
    if(iParentControl->IsReadOnly())
        {
		// Set the line to a correct position for scrolling.
		// But not just lineheight aligned.
        TInt offset = aParam.iDrawRect.iBr.iY - currentRect.iBr.iY;
        currentRect.Move(0, offset);
        }
    
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

// ---------------------------------------------------------
// CMsgEditorCustomDraw::SkinChanged
//
//
// ---------------------------------------------------------
//
void CMsgEditorCustomDraw::SkinChanged()
    {
    CGulIcon* newIcon = NULL;
    TRAP_IGNORE( newIcon = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(),
    												  KAknsIIDQgnGrafLineMessageHorizontal,
    												  KMsgEditorMbm,
    												  EMbmMsgeditorQgn_graf_line_message_horizontal,
    												  EMbmMsgeditorQgn_graf_line_message_horizontal_mask ) );
    
    if ( newIcon )
    	{
    	delete iIcon;
    	iIcon = newIcon;
    	
        ResolveLayouts();
    	}
    }

//  End of File
