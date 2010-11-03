/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       SMIL Player media renderer for displaying text files
*
*/



#include "SmilTextRenderer.h"

#include <txtrich.h>
#include <frmtview.h>

#include <eikenv.h>
#include <fbs.h>
#include <gdi.h>

// LAF
#include <aknlayout.cdl.h>
#include <applayout.cdl.h> 
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include <AknUtils.h>               // AknLayoutUtils
#include <AknsUtils.h>              // AknsUtils
#include <AknStatuspaneUtils.h>
#include <AknLayoutFont.h>

// pictograph support
#include <AknPictographInterface.h>
#include <AknPictographDrawerInterface.h>

#include <msgtextutils.h>

#include <smiltransitionfilter.h>
#include <smilmediainterface.h>
#include <smilparser.h>

#ifdef TEXT_DEBUG
    #include "SmilMediaLogging.h"
#endif

//These are from LAF:
const TInt KMarginSize = KFocusSize + 1;

_LIT( KParamColor1, "foreground-color" );
_LIT( KParamColor2, "color" );
_LIT( KParamColor3, "ForegroundColor" );

// ---------------------------------------------------------
// CSmilTextRenderer::CSmilTextRenderer
// ---------------------------------------------------------
//
CSmilTextRenderer::CSmilTextRenderer( MSmilMedia* aMedia,
                                      DRMCommon& aDrmCommon,
                                      CDRMHelper& aDrmHelper,
                                      TUint aCharset,
                                      TMsgMediaType aMediaType 
                                      ) :
    CSmilMediaRendererBase( aMediaType, aMedia, aDrmCommon, aDrmHelper ),                                     
    iCharset( aCharset ),
#ifdef RD_SCALABLE_UI_V2 
    iNotYetDrawn( ETrue )
#endif
    {
    }

// ---------------------------------------------------------
// CSmilTextRenderer::ConstructL
// ---------------------------------------------------------
//
void CSmilTextRenderer::ConstructL( RFile& aFileHandle, CGraphicsContext& aGc )
    {
    iCustomDraw = CSmilTextRendererCustomDraw::NewL( *this, aGc );
#ifdef RD_MSG_XHTML_SUPPORT
    if ( iMediaType == EMsgMediaXhtml )
    	{
	    iXhtmlParser = CXhtmlParser::NewL( this );	
       	iXhtmlParser->SetMode( EFalse, ETrue );
    	}
#endif
    CreateLayoutL();
    OpenTextL( aFileHandle );
    FormatTextL( aGc );
#ifdef RD_SCALABLE_UI_V2    
    CCoeControl* parentControl = iMedia->Presentation()->GetPlayer()->GetControl( iMedia->Presentation() );
    SetContainerWindowL( *parentControl );
    TRect controlRect = iMedia->GetRegion()->GetRectangle();
    controlRect.Move( parentControl->Position() );
    SetRect( controlRect );
#endif    
    }

// ---------------------------------------------------------
// CSmilTextRenderer::NewL
// ---------------------------------------------------------
//
CSmilTextRenderer* CSmilTextRenderer::NewL( RFile& aFileHandle,
                                            MSmilMedia* aMedia,
                                            DRMCommon& aDrmCommon,
                                            CDRMHelper& aDrmHelper,
                                            TUint aCharset,
                                            CGraphicsContext& aGc,
                                            TMsgMediaType aMediaType 
											)
    {
    CSmilTextRenderer* renderer = new(ELeave) CSmilTextRenderer( aMedia, 
                                                                 aDrmCommon,
                                                                 aDrmHelper,
                                                                 aCharset,
                                                                 aMediaType 
                                                                 );
    CleanupStack::PushL( renderer );
    renderer->ConstructL( aFileHandle, aGc );
    CleanupStack::Pop( renderer );
    return renderer;
    }

// ---------------------------------------------------------
// CSmilTextRenderer::~CSmilTextRenderer
// ---------------------------------------------------------
//
CSmilTextRenderer::~CSmilTextRenderer()
    {
    delete iLayout; // text layout
    delete iRichText; // contained text object
    delete iCharFormatLayer; // char format layer
    delete iParaFormatLayer; // para format layer
    delete iCustomDraw;
    delete iBufferDevice;
    delete iBufferBitmap;
    delete iMaskDevice;
    delete iMaskBitmap;
#ifdef RD_SCALABLE_UI_V2    
    delete iSBFrame;
#endif    
#ifdef RD_MSG_XHTML_SUPPORT
	delete iXhtmlParser;
#endif    
    iMedia = NULL; // For LINT
    }

// ---------------------------------------------------------
// CSmilTextRenderer::CreateLayoutL
// ---------------------------------------------------------
//
void CSmilTextRenderer::CreateLayoutL()
    {
    TRect appWindowRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EApplicationWindow, appWindowRect );
    
    TAknLayoutRect mainPane;
    
    if ( AknStatuspaneUtils::StaconPaneActive() )
        {
        mainPane.LayoutRect( appWindowRect, AknLayoutScalable_Avkon::main_pane( 4 ) );   
        }
    else
        {
        mainPane.LayoutRect( appWindowRect, AknLayoutScalable_Avkon::main_pane( 0 ) );   
        }
        
    TAknLayoutRect mainSmilPane;
    mainSmilPane.LayoutRect( mainPane.Rect(), AknLayoutScalable_Apps::main_smil_pane() );
    
    TAknLayoutRect smilTextPane;
    smilTextPane.LayoutRect( mainSmilPane.Rect(), AknLayoutScalable_Apps::smil_text_pane() );
    
    TAknLayoutRect smilListTextPane;
    smilListTextPane.LayoutRect( smilTextPane.Rect(), AknLayoutScalable_Apps::list_smil_text_pane() );
    
    TAknLayoutText smiltextPane;    
    smiltextPane.LayoutText( smilListTextPane.Rect(), 
                             AknLayoutScalable_Apps::smil_text_list_pane_t1( 0, 0 ) );
    
#ifdef RD_MSG_XHTML_SUPPORT
    if ( iMediaType == EMsgMediaXhtml )
    	{
    	TRgb textColor = smiltextPane.Color();

    	// Set default textColor for xhtml text
    	iXhtmlParser->SetDefaultTextColor( textColor );
    	
    	TAknLayoutText smilSmalltextPane;    
        smilSmalltextPane.LayoutText( smilListTextPane.Rect(), 
                                      AknLayoutScalable_Apps::smil_text_list_pane_t1( 0, 1 ) );
        
        TAknLayoutText smilLargetextPane;    
        smilLargetextPane.LayoutText( smilListTextPane.Rect(), 
                                      AknLayoutScalable_Apps::smil_text_list_pane_t1( 0, 2 ) );
    	
    	iXhtmlParser->SetFonts( smilLargetextPane.Font(),
	                            smiltextPane.Font(),
	                            smilSmalltextPane.Font(),
	                            smiltextPane.Font() );   
    	}          
#endif

    CParaFormat::TAlignment alignment( CParaFormat::ELeftAlign );
    switch ( smiltextPane.Align() )
        {
        case CGraphicsContext::ELeft:
            {
            alignment = CParaFormat::ELeftAlign;
            break;
            }
        case CGraphicsContext::ECenter:
            {
            alignment = CParaFormat::ECenterAlign;
            break;
            }
        case CGraphicsContext::ERight:
            {
            alignment = CParaFormat::ERightAlign;
            break;
            }
        default:
            {
            break;
            }
        }
    
    iLineHeight = smiltextPane.TextRect().Height();
    
    // Creates paragraph formatting parameter container.
    CParaFormat paraFormat;
    TParaFormatMask paraFormatMask;
    
    // Set up correct line spacing control parameters.
    paraFormatMask.SetAttrib( EAttLineSpacingControl );
    paraFormat.iLineSpacingControl = CParaFormat::ELineSpacingExactlyInPixels;
    
    paraFormatMask.SetAttrib( EAttLineSpacing );
    paraFormat.iLineSpacingInTwips = iLineHeight;
    
    // Set up correct horizontal alignment parameters.
    paraFormatMask.SetAttrib( EAttAlignment );
    paraFormat.iHorizontalAlignment = alignment;

    // Creates character formatting parameter container.
    TCharFormat charFormat;
    TCharFormatMask formatMask;
    
    // Set up correct character font.
    formatMask.SetAttrib( EAttFontTypeface );
    formatMask.SetAttrib( EAttFontHeight );
    formatMask.SetAttrib( EAttFontPosture );
    formatMask.SetAttrib( EAttFontStrokeWeight );
    
    charFormat.iFontSpec = smiltextPane.Font()->FontSpecInTwips();

    // Set up correct character color.
    formatMask.SetAttrib( EAttColor );
    charFormat.iFontPresentation.iTextColor = ResolveTextColorL( smiltextPane.Color() );

    iParaFormatLayer = CParaFormatLayer::NewL( &paraFormat, paraFormatMask );
    iCharFormatLayer = CCharFormatLayer::NewL( charFormat, formatMask );
    
    iRichText = CRichText::NewL( iParaFormatLayer, iCharFormatLayer );
    
    iLayout = CTextLayout::NewL( iRichText, iMedia->GetRectangle().Width() );
    iLayout->SetCustomDraw( iCustomDraw );
    
    // Do not automatically inflate the ascents inside Form 
    iLayout->SetFontHeightIncreaseFactor( 0 ); 
    
    // Allow minimum descent of 0
    iLayout->SetMinimumLineDescent( 0 );
    
    const CAknLayoutFont* layoutFont = CAknLayoutFont::AsCAknLayoutFontOrNull( smiltextPane.Font() );
    
    // Font must always be an CAknLayoutFont otherwise we cannot continue.
    if ( !layoutFont )
        {
        return;
        }
    
    const CFbsFont* fbsFont = NULL;

    if ( layoutFont->TypeUid() == KCFbsFontUid )
        {
        fbsFont = static_cast<const CFbsFont*>( layoutFont );
        }
    
    // Font maximum height & width.
    TInt maxHeight( 0 );
    
    if ( fbsFont && fbsFont->IsOpenFont() )
        {
        // FORM itself uses an OpenFont MaxHeight it if can get it. Account for this:
        TOpenFontMetrics metrics;
        fbsFont->GetFontMetrics( metrics );
        
        maxHeight = metrics.MaxHeight();
        }
    else
        {
        maxHeight = layoutFont->AscentInPixels();
        }
    
    // This space around the glyph areas is for cursor and highlight extensions but it 
    // might also be used by font.
    TAknTextDecorationMetrics decorationMetrics( layoutFont );
    
    TInt topMargin( 0 );
    TInt bottomMargin( 0 );
    decorationMetrics.GetTopAndBottomMargins( topMargin, bottomMargin );
   
    // This is the difference between what "ascent" I need, and the prediction of what FORM will use:
    TInt extraRoomForHighGlyphs = layoutFont->TextPaneTopToBaseline() - maxHeight;
    
    // Need to create dummy textView object as SetExcessHeightRequired is not exported for
    // CTextLayout. CTextView will not take ownership of iLayout.
    TRect empty;
    CTextView* texView = CTextView::NewL( iLayout, 
                                          empty, 
                                          NULL, 
                                          NULL, 
                                          &Window(), 
                                          NULL,
                                          &iCoeEnv->WsSession() );
    
    // Have to convince Form to draw into the ascent area.
    TInt excessHeightRequired = extraRoomForHighGlyphs + topMargin;
    texView->SetExcessHeightRequired( excessHeightRequired );
    
    delete texView; // Will not delete layout.
    }


// ---------------------------------------------------------
// CSmilTextRenderer::OpenTextL
// ---------------------------------------------------------
//
void CSmilTextRenderer::OpenTextL( RFile& aFileHandle )
    {
    if ( iMediaType == EMsgMediaXhtml )
    	{
#ifdef RD_MSG_XHTML_SUPPORT
	    RFile tempHandle;
	    tempHandle.Duplicate( aFileHandle );
    	iXhtmlParser->CreateDomL( tempHandle );
    	iError = 0;
    	BeginActiveWait();
    	User::LeaveIfError( iError );
#endif    	
    	}
    else
    	{
	    CMsgTextUtils* textUtils = CMsgTextUtils::NewL( iCoeEnv->FsSession() );
	    CleanupStack::PushL( textUtils );
	    
	    TUint charset = textUtils->MibIdToCharconvIdL( iCharset );
	    
	    CleanupStack::PopAndDestroy( textUtils );
	    
	    RFile tempHandle;
	    tempHandle.Duplicate( aFileHandle );
	    
	    RFileReadStream inputStream( tempHandle ); // Owns tempHandle
	    CleanupClosePushL( inputStream );
	    
	    CPlainText::TImportExportParam param;
	    param.iForeignEncoding = charset;
	    CPlainText::TImportExportResult result;
	    iRichText->ImportTextL( 0, inputStream, param, result );
	    
	    CleanupStack::PopAndDestroy( &inputStream );
    	}
    }

// ---------------------------------------------------------
// CSmilTextRenderer::ResolveTextColorL
// ---------------------------------------------------------
//
TRgb CSmilTextRenderer::ResolveTextColorL( TRgb aDefaultColor ) const
    {
    TRgb textColor = aDefaultColor;
    TPtrC fgColor = iMedia->GetParam( KParamColor1 );
    if ( !fgColor.Length() )
        {
        fgColor.Set( iMedia->GetParam( KParamColor2 ) );
        }
    if ( !fgColor.Length() )
        {
        fgColor.Set( iMedia->GetParam( KParamColor3 ) );
        }
    if ( fgColor.Length() )
        {
        TBool transparent = EFalse;
        textColor = CSmilParser::ParseColor( fgColor, transparent );
        }
    return textColor;
    }

// ---------------------------------------------------------
// CSmilTextRenderer::FormatTextL
// ---------------------------------------------------------
//
void CSmilTextRenderer::FormatTextL( const CGraphicsContext& aGc )
    {
    iLayout->SetImageDeviceMap( aGc.Device() );
    iLayout->SetFormatMode( CLayoutData::EFScreenMode,
                            iMedia->GetRectangle().Width(),
                            NULL );
                            
    MSmilRegion* region = iMedia->GetRegion();
    if ( region )
        {
        TInt maxWidth = region->GetRectangle().Width();
        maxWidth = Max( 0, maxWidth - ( 2 * KMarginSize ) );
        
        iLayout->SetBandHeight( CLayoutData::EFHeightForFormattingAllText );
        iLayout->SetWrapWidth( maxWidth );
        iLayout->FormatBandL();
        
        if ( iLayout->FormattedHeightInPixels() >
             region->GetRectangle().Height() - ( 2 * KMarginSize) )
            {
            //scrolling needed
            iScrollBar = ETrue;
            
            if ( !AknStatuspaneUtils::StaconPaneActive() )
                {
                // Calculates how much text is hidden under control pane and
                // adds so many line breaks to the end of the text as is neccesary
                // for all the text to become visible when it is scrolled.
                TRect invisibleRect( region->GetRectangle() );
            
                CCoeControl* parent = iMedia->Presentation()->GetPlayer()->GetControl( iMedia->Presentation() );
                invisibleRect.Move( parent->PositionRelativeToScreen() );
                
                TRect controlPaneRect;
                AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EControlPane, controlPaneRect );
                
                invisibleRect.Intersection( controlPaneRect );
                if ( invisibleRect.Height() > 0 && 
                     iLineHeight > 0 )
                    {
                    TInt linesToAdd = invisibleRect.Height() / iLineHeight;
                    linesToAdd++;
                    
                    while ( linesToAdd > 0 )
                        {
                        iRichText->InsertL( iRichText->DocumentLength(), CEditableText::ELineBreak);
                        linesToAdd--;
                        }
                    }
                }

#ifdef RD_SCALABLE_UI_V2           
            
            TRect mainpaneRect;
            AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainpaneRect );
    
            TAknLayoutRect scrollBarRect;
            scrollBarRect.LayoutRect( mainpaneRect, AknLayoutScalable_Avkon::scroll_pane(0) );
    
            iLayout->SetWrapWidth( Max( 0, ( maxWidth - scrollBarRect.Rect().Width() ) ) );
#else
            iLayout->SetWrapWidth( Max( 0, ( maxWidth - KScrollBarSize ) ) );
#endif            
            iLayout->FormatBandL();
            }
        }
    else
        {
        iLayout->SetWrapWidth( IntrinsicWidth() );
        iLayout->FormatBandL();
        }
    }


// ---------------------------------------------------------
// CSmilTextRenderer::IsVisual
// ---------------------------------------------------------
//
TBool CSmilTextRenderer::IsVisual() const
    {
    return ETrue;
    }

// ---------------------------------------------------------
// CSmilTextRenderer::IsScrollable
// ---------------------------------------------------------
//
TBool CSmilTextRenderer::IsScrollable() const
    {
    return iScrollBar;
    }

// ---------------------------------------------------------
// CSmilTextRenderer::IntrinsicWidth
// ---------------------------------------------------------
//
TInt CSmilTextRenderer::IntrinsicWidth() const
    {
    return iMedia->GetRectangle().Width();
    }

// ---------------------------------------------------------
// CSmilTextRenderer::IntrinsicHeight
// ---------------------------------------------------------
//
TInt CSmilTextRenderer::IntrinsicHeight() const
    {
    return iMedia->GetRectangle().Height();
    }

// ---------------------------------------------------------
// CSmilTextRenderer::SeekMediaL
// ---------------------------------------------------------
//
void CSmilTextRenderer::SeekMediaL( const TSmilTime& /*aTime*/ )
    {
    }


// ---------------------------------------------------------
// CSmilTextRenderer::PrepareMediaL
// ---------------------------------------------------------
//
void CSmilTextRenderer::PrepareMediaL()
    {
#ifdef TEXT_DEBUG
    SMILUILOGGER_ENTERFN( "Text: PrepareMediaL" );
#endif
#ifdef RD_SCALABLE_UI_V2    
    CCoeControl* parentControl = iMedia->Presentation()->GetPlayer()->GetControl( iMedia->Presentation() );
    TRect controlRect( iMedia->GetRegion()->GetRectangle() );
    controlRect.Move( parentControl->Position() );
    SetRect( controlRect );
#endif    
    }

// ---------------------------------------------------------
// CSmilTextRenderer::ShowMediaL
// ---------------------------------------------------------
//
void CSmilTextRenderer::ShowMediaL()
    {
#ifdef TEXT_DEBUG
    SMILUILOGGER_ENTERFN( "Text: ShowMediaL" );
#endif
    iShowing = ETrue;    
    iMedia->Redraw();
    }

// ---------------------------------------------------------
// CSmilTextRenderer::HideMedia
// ---------------------------------------------------------
//
void CSmilTextRenderer::HideMedia()
    {
#ifdef TEXT_DEBUG
    SMILUILOGGER_ENTERFN( "Text: HideMedia" );
#endif
    iShowing = EFalse;
    iMedia->Redraw();
#ifdef RD_SCALABLE_UI_V2    
    iNotYetDrawn = ETrue;
    if ( iSBFrame )
        {
        TRAP_IGNORE( iSBFrame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, 
                                                        CEikScrollBarFrame::EOff ) );
        }
#endif        
    }

// ---------------------------------------------------------
// CSmilTextRenderer::FreezeMedia
// ---------------------------------------------------------
//
void CSmilTextRenderer::FreezeMedia()
    {
#ifdef TEXT_DEBUG
    SMILUILOGGER_ENTERFN( "Text: FreezeMedia" );
#endif
    }

// ---------------------------------------------------------
// CSmilTextRenderer::ResumeMedia
// ---------------------------------------------------------
//
void CSmilTextRenderer::ResumeMedia()
    {
#ifdef TEXT_DEBUG
    SMILUILOGGER_ENTERFN( "Text: ResumeMedia" );
#endif
    }

// ---------------------------------------------------------
// CSmilTextRenderer::Draw
// ---------------------------------------------------------
//
void CSmilTextRenderer::Draw( CGraphicsContext& aGc,
                          const TRect& aRect,
                          CSmilTransitionFilter* aTransitionFilter,
                          const MSmilFocus* aFocus)
    {
    if (iShowing)
        {
        TRect overallRect( iMedia->GetRectangle() );
        overallRect.Shrink( KMarginSize, KMarginSize );
              
        TBool drawLayout( ETrue );
        
        // Move the text to start after scroll bar on mirrored
        // layout when scroll bar is present.
        if ( AknLayoutUtils::LayoutMirrored() && iScrollBar )
            {
#ifdef RD_SCALABLE_UI_V2
            TRect mainpaneRect;
            AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainpaneRect );
            
            TAknLayoutRect scrollBarRect;
            scrollBarRect.LayoutRect( mainpaneRect, AknLayoutScalable_Avkon::scroll_pane(0) );
            
            overallRect.iTl.iX = overallRect.iTl.iX + scrollBarRect.Rect().Width() + 1;
#else
            overallRect.iTl.iX = overallRect.iTl.iX + KScrollBarSize + 1;
#endif
            if ( overallRect.iTl.iX > overallRect.iBr.iX )
                {
                overallRect.iTl.iX = overallRect.iBr.iX;
                }
            }
        
        if ( aTransitionFilter )
            {
            TRAPD( err, DrawTransitionL( aGc, aTransitionFilter, overallRect ) );
            if ( err != KErrNone )
                {
                delete iBufferBitmap;
                iBufferBitmap = NULL;
                
                delete iBufferDevice;
                iBufferDevice = NULL;
                
                delete iMaskBitmap;
                iMaskBitmap = NULL;
                
                delete iMaskDevice;
                iMaskDevice = NULL;
                }
            else
                {
                //transition was properly drawn
                //->Don't draw "normal" layout
                drawLayout = EFalse;
                }
            }
       
        if ( drawLayout )
            {
            TRect updateRect( aRect );
            updateRect.Intersection( overallRect );
            
            iDrawContext.SetGc( &aGc );
            iDrawContext.iViewRect = overallRect;
            iDrawContext.SetDrawToEveryPixel( EFalse );
        
            TRAP_IGNORE( 
                {
                iLayout->DrawL( updateRect, &iDrawContext );
                
                if ( iScrollBar )
                    {           
                    DrawScrollBarL( aGc, updateRect );
                    } 
                } );
            }
        }

    if ( aFocus )
        {
        DrawFocus( aGc, aFocus );
        }
    }

// ---------------------------------------------------------
// CSmilTextRenderer::GetText
// ---------------------------------------------------------
//
EXPORT_C CPlainText* CSmilTextRenderer::GetText()
    {
    return static_cast<CPlainText*>( iRichText );
    }

// ---------------------------------------------------------
// CSmilTextRenderer::DrawPictoCallback
// ---------------------------------------------------------
//
void CSmilTextRenderer::DrawPictoCallback() const
    {
    iMedia->Redraw();
    }

// ---------------------------------------------------------
// CSmilTextRenderer::Scroll
// ---------------------------------------------------------
//
void CSmilTextRenderer::Scroll(TInt /*aDirX*/, TInt aDirY)
    {
#ifdef TEXT_DEBUG
    SMILUILOGGER_WRITEF( _L("Text: Scroll, deltaY=%d"), aDirY );
#endif
    ScrollWithOutThumbUpdate( aDirY * iLineHeight );
#ifdef RD_SCALABLE_UI_V2    
    if ( iThumbPos > 0 && aDirY < 0 ||
        iThumbPos < iScrollSpan - 
            (iMedia->GetRegion()->GetRectangle().Height() )/ iLineHeight
         && aDirY > 0 )
    //the thumbspan needs to be substracted from the scrollspan          
        {
        iThumbPos += aDirY;
        iSBFrame->MoveVertThumbTo( iThumbPos );        
        }
#endif        
    }

// ---------------------------------------------------------
// CSmilTextRenderer::ScrollWithOutThumbUpdate
// This function is for scrolling when no thumbupdate is needed
// ---------------------------------------------------------
//
void CSmilTextRenderer::ScrollWithOutThumbUpdate( TInt aDirY )
    {
#ifdef TEXT_DEBUG
    SMILUILOGGER_WRITEF( _L("Text: ScrollWithOutThumbUpdate, aDirY=%d"), aDirY );
#endif
    TInt remaining = 
        iLayout->FormattedHeightInPixels() - 
        ( iLayout->PixelsAboveBand() + 
        iMedia->GetRegion()->GetRectangle().Height() - ( 2 * KMarginSize ) );
    if ( remaining <= 0 && -aDirY < 0 )
        {
        return;
        }
    if ( aDirY > remaining )
        {
        aDirY = remaining;
        }
    TInt scrollValue = - aDirY;        
    TRAPD( error, iLayout->ChangeBandTopL( scrollValue, 
            CTextLayout::EFAllowScrollingBlankSpace ) );
    if ( error == KErrNone )
        {
        iMedia->Redraw();
        }
    }
    

// ---------------------------------------------------------
// CSmilTextRenderer::DrawScrollBarL
// ---------------------------------------------------------
//
void CSmilTextRenderer::DrawScrollBarL( 
#ifdef RD_SCALABLE_UI_V2
                                     CGraphicsContext& /*aGc*/,
#else
                                     CGraphicsContext& aGc,
#endif
                                      const TRect& /*aRect*/ ) 
    {
#ifdef RD_SCALABLE_UI_V2
    if ( iNotYetDrawn )
        {
        if ( !iSBFrame )
            {
            iSBFrame = new (ELeave) CEikScrollBarFrame( this, this, ETrue );
            iSBFrame->CreateDoubleSpanScrollBarsL(ETrue, EFalse, ETrue, EFalse );
            iSBFrame->SetTypeOfVScrollBar( CEikScrollBarFrame::EDoubleSpan );
            iScrollSpan = (iLayout->FormattedHeightInPixels()+ ( 2 * KMarginSize )) / iLineHeight;
            if ( iScrollSpan *iLineHeight != 
                    (iLayout->FormattedHeightInPixels()+ ( 2 * KMarginSize ))   )
                {
                iScrollSpan += 1;
                }
            }
       //these steps need to be redone as most likely 
       // TileL-function needs to be recalled also when only reactivating the existing bar
        iSBFrame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,CEikScrollBarFrame::EOn );    
         
        TEikScrollBarModel hSbarModel;
        TEikScrollBarModel vSbarModel;
        vSbarModel.iThumbPosition = iThumbPos; 
        vSbarModel.iScrollSpan = iScrollSpan;
        vSbarModel.iThumbSpan = iMedia->GetRegion()->GetRectangle().Height() / iLineHeight;

        // For EDoubleSpan type scrollbar
        TAknDoubleSpanScrollBarModel hDsSbarModel(hSbarModel);
        TAknDoubleSpanScrollBarModel vDsSbarModel(vSbarModel);

        TEikScrollBarFrameLayout layout;
        layout.iTilingMode = TEikScrollBarFrameLayout::EInclusiveRectConstant;

        TRect rect( Rect( ) ); 
        rect.Shrink( KMarginSize,KMarginSize );

        //Should have separate variable for "inclusiveRect" and "clientRect"
        TRect inclusiveRect( rect );
        TRect clientRect( rect );
                   
        iSBFrame->TileL( &hDsSbarModel, &vDsSbarModel, clientRect, inclusiveRect, layout );
   
        iSBFrame->SetVFocusPosToThumbPos( iThumbPos );
           
        iSBFrame->GetScrollBarHandle( CEikScrollBar::EVertical )->DrawDeferred();
        iNotYetDrawn = EFalse;
        Window().Invalidate();
        }
#else
    TRect regionRect = iMedia->GetRegion()->GetRectangle();
    TRect textRect( regionRect );
    textRect.iTl.iY -= iLayout->PixelsAboveBand();
    textRect.iBr.iY = textRect.iTl.iY + iLayout->FormattedHeightInPixels();

    // from CSmilMediaRendererBase:
    DrawScrollBars( aGc, regionRect, textRect );
        
#endif            
    }

// ---------------------------------------------------------
// CSmilTextRenderer::DrawTransitionL
// ---------------------------------------------------------
//
void CSmilTextRenderer::DrawTransitionL( CGraphicsContext& aGc,
                                        CSmilTransitionFilter* aTransitionFilter,
                                        const TRect& aRect )
    {
    if ( !iBufferBitmap )
        {
        iBufferBitmap = new (ELeave) CFbsBitmap();
        User::LeaveIfError( iBufferBitmap->Create( aRect.Size(), EColor4K ) );
        iBufferDevice = CFbsBitmapDevice::NewL( iBufferBitmap );
    
        iMaskBitmap = new (ELeave) CFbsBitmap();
        User::LeaveIfError( iMaskBitmap->Create( aRect.Size(), EGray2 ) );
        iMaskDevice = CFbsBitmapDevice::NewL( iMaskBitmap );
        }

    CBitmapContext* bitmapContext = NULL;
    CBitmapContext* maskContext = NULL;

    User::LeaveIfError( iBufferDevice->CreateBitmapContext( bitmapContext ) );
    User::LeaveIfError( iMaskDevice->CreateBitmapContext( maskContext ) );

    TRect bitmapRect( TPoint(0,0), aRect.Size() );

    maskContext->SetBrushStyle( CGraphicsContext::ESolidBrush );
    maskContext->SetPenStyle( CGraphicsContext::ENullPen );
    maskContext->SetBrushColor( TRgb(0,0,0) );
    maskContext->DrawRect( bitmapRect );

    iDrawContext.SetGc( bitmapContext );
    iDrawContext.iViewRect = bitmapRect;
    
    TInt err = KErrNone;
    TRAP( err, iLayout->DrawL( bitmapRect, &iDrawContext ) );

    iDrawContext.SetGc( maskContext );                
    iCustomDraw->SetWhite( ETrue );
    TRAP( err, iLayout->DrawL( bitmapRect, &iDrawContext ) );
    iCustomDraw->SetWhite( EFalse );

    aTransitionFilter->Draw(
        aGc,
        bitmapRect,
        iBufferBitmap,
        aRect.iTl,
        iMaskBitmap );

    delete bitmapContext;
    delete maskContext;
    }

// ---------------------------------------------------------
// CSmilTextRendererCustomDraw::NewL
// ---------------------------------------------------------
//
CSmilTextRendererCustomDraw* CSmilTextRendererCustomDraw::NewL(
    const CSmilTextRenderer& aParent,
    CGraphicsContext& aGc )
    {
    CSmilTextRendererCustomDraw* self = 
        new (ELeave) CSmilTextRendererCustomDraw( aParent, aGc );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CSmilTextRendererCustomDraw::ConstructL
// ---------------------------------------------------------
//
void CSmilTextRendererCustomDraw::ConstructL()
    {
    iPictographDrawer = CAknPictographInterface::NewL(
        static_cast<CCoeControl&>( const_cast<CSmilTextRenderer&>( iParent ) ),
        *static_cast<MAknPictographAnimatorCallBack*>( this ) );
    }

// ---------------------------------------------------------
// CSmilTextRendererCustomDraw::CSmilTextRendererCustomDraw
// ---------------------------------------------------------
//
CSmilTextRendererCustomDraw::CSmilTextRendererCustomDraw(
        const CSmilTextRenderer& aParent,
        CGraphicsContext& aGc ) :
    iParent( aParent ),
    iGc( aGc )
    {
    }

// ---------------------------------------------------------
// CSmilTextRendererCustomDraw::~CSmilTextRendererCustomDraw
// ---------------------------------------------------------
//
CSmilTextRendererCustomDraw::~CSmilTextRendererCustomDraw()
    {
    delete iPictographDrawer;
    }

// ---------------------------------------------------------
// CSmilTextRendererCustomDraw::DrawBackground
// ---------------------------------------------------------
//
void CSmilTextRendererCustomDraw::DrawBackground( const TParam& aParam,
                                                 const TRgb& /*aRgb*/,
                                                 TRect& aDrawn ) const 
    {
    aDrawn = aParam.iDrawRect;
    }


// ---------------------------------------------------------
// CSmilTextRendererCustomDraw::DrawText
// ---------------------------------------------------------
//
void CSmilTextRendererCustomDraw::DrawText( const TParam& aParam,
              const TLineInfo& aLineInfo,
              const TCharFormat& aFormat,
              const TDesC& aText,
              const TPoint& aTextOrigin,
              TInt aExtraPixels ) const
    {
    if ( iWhite )
        {
        aParam.iGc.SetPenColor( KRgbWhite );
        }

    MFormCustomDraw::DrawText(
        aParam,
        aLineInfo,
        aFormat,
        aText,
        aTextOrigin,
        aExtraPixels );

    // Draw pictographs if the feature is supported.
    // Character justification is not supported.
    if ( iPictographDrawer && !aExtraPixels )
        {
        CBitmapContext* bitmapGc = NULL;
        bitmapGc = static_cast<CBitmapContext*>(&iGc);
        
        const TText* text = aText.Ptr();
        TInt length( aText.Length() );

        MAknPictographDrawer* drawer =
            iPictographDrawer->Interface();

        CFont* font = NULL;
        aParam.iMap.GetNearestFontInTwips( font, aFormat.iFontSpec );

        if ( font )
            {
            for ( TInt i = 0 ; i < length ; i++ )
                {
                if ( drawer->IsPictograph( text[i] ) )
                    {
                    TPoint point = aParam.iDrawRect.iTl;            
                    point.iX += font->TextWidthInPixels( aText.Left( i ) );
                    point.iY += font->AscentInPixels();

                    drawer->DrawPictographsInText(
                        *bitmapGc,
                        *font,
                        aText.Mid( i, 1 ),
                        point );
                    }
                }

            aParam.iMap.ReleaseFont( font );
            }
        }
    }

// ---------------------------------------------------------
// CSmilTextRendererCustomDraw::SetWhite
// ---------------------------------------------------------
//
void CSmilTextRendererCustomDraw::SetWhite( TBool aWhite )
    {
    iWhite = aWhite;
    }

// ---------------------------------------------------------
// CSmilTextRendererCustomDraw::DrawPictographArea
// ---------------------------------------------------------
//
void CSmilTextRendererCustomDraw::DrawPictographArea()
    {
    iParent.DrawPictoCallback();
    }
#ifdef RD_SCALABLE_UI_V2   
// -----------------------------------------------------------------------------
// CSmilTextRenderer::HandleScrollEventL
// -----------------------------------------------------------------------------
// 
void CSmilTextRenderer::HandleScrollEventL( 
                    CEikScrollBar* aScrollBar, TEikScrollEvent aEventType )
    {
    const TInt KScrollValue = iLineHeight;
    const TInt KScrollPageValue = iMedia->GetRegion()->GetRectangle().Height();
    if ( AknLayoutUtils::PenEnabled() )
        {
        TInt oldThumbPos = iThumbPos;
        iThumbPos = aScrollBar->ThumbPosition();
    	switch ( aEventType )
    		{ 
       		case EEikScrollUp:
       		    ScrollWithOutThumbUpdate( -KScrollValue );
       		    break;
       		case EEikScrollDown:
       		    ScrollWithOutThumbUpdate( KScrollValue );
       		    break;
   		    case EEikScrollPageUp:
   		        {
   		        ScrollWithOutThumbUpdate( -KScrollPageValue );
   		        break;
   		        }
   		    case EEikScrollPageDown:
   		        {
   		        ScrollWithOutThumbUpdate( KScrollPageValue );
   		        break;
   		        }
        	case EEikScrollThumbDragVert:
        	    {
   		        //update the shown page to match with the thumb position
                ScrollWithOutThumbUpdate( (iThumbPos - oldThumbPos)*KScrollValue );    
        	    break;
        	    }
        	   		        
       		case EEikScrollLeft:
    		case EEikScrollRight:
        	case EEikScrollPageLeft:
        	case EEikScrollPageRight:
        	case EEikScrollThumbDragHoriz:
        	case EEikScrollThumbReleaseHoriz:
        	case EEikScrollThumbReleaseVert:	
            case EEikScrollHome:
        	case EEikScrollTop:
        	case EEikScrollEnd:
        	case EEikScrollBottom:
       		default:
       		    // Do nothing
       			break;
       			}
        }
    } 
#else
void CSmilTextRenderer::HandleScrollEventL( 
                    CEikScrollBar* /*aScrollBar*/, TEikScrollEvent /*aEventType*/ )
    {  }
#endif

// -----------------------------------------------------------------------------
// CSmilTextRenderer::CompleteL
// Call back function used to inform a client of the Parser
// when a parsing operation completes.
// -----------------------------------------------------------------------------
//	
void CSmilTextRenderer::ParseCompleteL()
	{
#ifdef RD_MSG_XHTML_SUPPORT
	CRichText& text = *iRichText;
    iXhtmlParser->ParseL( text );
    EndActiveWait();
#endif	
	}
	
// -----------------------------------------------------------------------------
// CSmilTextRenderer::ParseError
// Call back function used to inform a client of the Parser
// about error.
// -----------------------------------------------------------------------------
//	
void CSmilTextRenderer::ParseError( TInt aError )
	{
	iError = aError;
#ifdef RD_MSG_XHTML_SUPPORT
	EndActiveWait();
#endif	
	}

// End of file
