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
* Description:  PostcardLaf implementation
*
*/




// INCLUDE FILES
#include <aknenv.h>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <aknlayout.cdl.h>
#include <applayout.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <MsgEditorCommon.h>
#include "PostcardLaf.h"

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// MainPostcardPane
// ---------------------------------------------------------
TRect PostcardLaf::MainPostcardPane( )
    {
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
    TRect mainPostcardLayout(mainPane);
    TAknLayoutRect mainPostcardLayoutRect;
    mainPostcardLayoutRect.LayoutRect(
        mainPane,
        AknLayoutScalable_Apps::main_postcard_pane( ).LayoutLine() );
    mainPostcardLayout = mainPostcardLayoutRect.Rect( );
    return mainPostcardLayout;
    }

// ---------------------------------------------------------
// RelativeMainPostcardPane
// ---------------------------------------------------------
TRect PostcardLaf::RelativeMainPostcardPane( )
    {
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
    TRect mainPostcardLayout(mainPane);
    TAknLayoutRect mainPostcardLayoutRect;
    mainPostcardLayoutRect.LayoutRect(
        mainPane,
        AknLayoutScalable_Apps::main_postcard_pane( ).LayoutLine() );
        mainPostcardLayout = mainPostcardLayoutRect.Rect( );
    // This is needed at least when touch pane is there.
	mainPostcardLayout.Move( -mainPostcardLayout.iTl.iX, -mainPostcardLayout.iTl.iY );
    return mainPostcardLayout;
    }

// ---------------------------------------------------------
// PostcardPane
// ---------------------------------------------------------
TRect PostcardLaf::PostcardPane( )
    {
    TRect mainPostcardPane = PostcardLaf::MainPostcardPane();
    TRect postcardPaneLayout(mainPostcardPane);
    TAknLayoutRect postcardPaneLayoutRect;
    TInt index = 0;
    if( Layout_Meta_Data::IsLandscapeOrientation( ) )
        {
        index = 1;
        }
    postcardPaneLayoutRect.LayoutRect(
        mainPostcardPane,
        AknLayoutScalable_Apps::postcard_pane( index ).LayoutLine() );
    postcardPaneLayout = postcardPaneLayoutRect.Rect( );
    return postcardPaneLayout;
    }

// ---------------------------------------------------------
// RelativePostcardPane
// ---------------------------------------------------------
TRect PostcardLaf::RelativePostcardPane( )
    {
    TRect mainPostcardPane = PostcardLaf::RelativeMainPostcardPane();
    TRect postcardPaneLayout(mainPostcardPane);
    TInt index = 0;
    if( Layout_Meta_Data::IsLandscapeOrientation( ) )
        {
        index = 1;
        }
    TAknLayoutRect postcardPaneLayoutRect;
    postcardPaneLayoutRect.LayoutRect(
        mainPostcardPane,
        AknLayoutScalable_Apps::postcard_pane( index ).LayoutLine() );
    postcardPaneLayout = postcardPaneLayoutRect.Rect( );
    return postcardPaneLayout;
    }

// ---------------------------------------------------------
// UpperArrow
// ---------------------------------------------------------
TRect PostcardLaf::UpperArrow( )
    {
    TRect mainPostcardPane = PostcardLaf::RelativeMainPostcardPane();
    TRect layout(mainPostcardPane);
    TInt index = 0;
    if( Layout_Meta_Data::IsLandscapeOrientation( ) )
        {
        index = 1;
        }
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(
        mainPostcardPane,
        AknLayoutScalable_Apps::main_postcard_pane_g5( index ).LayoutLine() );
    layout = layoutRect.Rect( );
    return layout;
    }

// ---------------------------------------------------------
// LowerArrow
// ---------------------------------------------------------
TRect PostcardLaf::LowerArrow( )
    {
    TRect mainPostcardPane = PostcardLaf::RelativeMainPostcardPane( );
    TRect layout(mainPostcardPane);
    TInt index = 0;
    if( Layout_Meta_Data::IsLandscapeOrientation( ) )
        {
        index = 1;
        }
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(
        mainPostcardPane,
        AknLayoutScalable_Apps::main_postcard_pane_g6( index ).LayoutLine() );
    layout = layoutRect.Rect( );
    return layout;
    }

// ---------------------------------------------------------
// BackBackground
// ---------------------------------------------------------
TRect PostcardLaf::BackBackground( )
    {
    TRect postcardPane = PostcardLaf::RelativePostcardPane( );
    TRect layout(postcardPane);
    TInt index = 0;
    if( Layout_Meta_Data::IsLandscapeOrientation( ) )
        {
        index = 1;
        }
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(
        postcardPane,
        AknLayoutScalable_Apps::postcard_pane_g1( index ).LayoutLine() );
    layout = layoutRect.Rect( );
    return layout;
    }

// ---------------------------------------------------------
// FrontBackground
// ---------------------------------------------------------
TRect PostcardLaf::FrontBackground( )
    {
    TRect postcardPane = PostcardLaf::RelativePostcardPane( );
    TRect layout(postcardPane);
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(
        postcardPane,
        AknLayoutScalable_Apps::postcard_pane_g7( 0 ).LayoutLine() );
    layout = layoutRect.Rect( );
    return layout;
    }

// ---------------------------------------------------------
// FrontBackgroundWithoutImage
// ---------------------------------------------------------
TRect PostcardLaf::FrontBackgroundWithoutImage( )
    {
    TRect postcardPane = PostcardLaf::RelativePostcardPane( );
    TRect layout(postcardPane);
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(
        postcardPane,
        AknLayoutScalable_Apps::postcard_pane_g7( 1 ).LayoutLine() );
    layout = layoutRect.Rect( );
    return layout;
    }

// ---------------------------------------------------------
// Stamp
// ---------------------------------------------------------
TRect PostcardLaf::Stamp( )
    {
    TRect postcardPane = PostcardLaf::RelativePostcardPane( );
    TRect layout(postcardPane);
    TInt index = 0;
    if( Layout_Meta_Data::IsLandscapeOrientation( ) )
        {
        index = 1;
        }
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(
        postcardPane,
        AknLayoutScalable_Apps::postcard_pane_g2( index ).LayoutLine() );
    layout = layoutRect.Rect( );
    return layout;
    }

// ---------------------------------------------------------
// InsertImageIcon
// ---------------------------------------------------------
TRect PostcardLaf::InsertImageIcon( )
    {
    TRect postcardPane = PostcardLaf::RelativePostcardPane( );
    TRect layout(postcardPane);
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(
        postcardPane,
        AknLayoutScalable_Apps::postcard_pane_g3( 0 ).LayoutLine() );
    layout = layoutRect.Rect( );
    return layout;
    }

// ---------------------------------------------------------
// Image
// ---------------------------------------------------------
TRect PostcardLaf::Image( )
    {
    TRect postcardPane = PostcardLaf::RelativePostcardPane( );
    TRect layout(postcardPane);
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(
        postcardPane,
        AknLayoutScalable_Apps::postcard_pane_g4( 0 ).LayoutLine() );
    layout = layoutRect.Rect( );
    return layout;
    }

// ---------------------------------------------------------
// Address
// ---------------------------------------------------------
TRect PostcardLaf::Address( )
    {
    TRect postcardPane = PostcardLaf::RelativePostcardPane( );
    TRect layout(postcardPane);
    TInt index = 0;
    if( Layout_Meta_Data::IsLandscapeOrientation( ) )
        {
        index = 1;
        }
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(
        postcardPane,
        AknLayoutScalable_Apps::postcard_pane_g5( index ).LayoutLine() );
    layout = layoutRect.Rect( );
    return layout;
    }

// ---------------------------------------------------------
// Address
// ---------------------------------------------------------
TRect PostcardLaf::AddressFocus( )
    {
    TRect postcardPane = PostcardLaf::RelativePostcardPane( );
    TRect layout(postcardPane);
    TInt index = 0;
    if( Layout_Meta_Data::IsLandscapeOrientation( ) )
        {
        index = 1;
        }
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(
        postcardPane,
        AknLayoutScalable_Apps::postcard_address2_pane( index ).LayoutLine() );
    layout = layoutRect.Rect( );
    return layout;
    }

// ---------------------------------------------------------
// GreetingText
// ---------------------------------------------------------
TRect PostcardLaf::GreetingText( )
    {
    TRect postcardPane = PostcardLaf::RelativePostcardPane( );
    TRect layout(postcardPane);
    TInt index = 0;
    if( Layout_Meta_Data::IsLandscapeOrientation( ) )
        {
        index = 1;
        }
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(
        postcardPane,
        AknLayoutScalable_Apps::postcard_pane_g6( index ).LayoutLine() );
    layout = layoutRect.Rect( );
    return layout;
    }

// ---------------------------------------------------------
// GreetingText
// ---------------------------------------------------------
TRect PostcardLaf::GreetingTextFocus( )
    {
    TRect postcardPane = PostcardLaf::RelativePostcardPane( );
    TRect layout(postcardPane);
    TInt index = 0;
    if( Layout_Meta_Data::IsLandscapeOrientation( ) )
        {
        index = 1;
        }
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(
        postcardPane,
        AknLayoutScalable_Apps::postcard_message2_pane( index ).LayoutLine() );
    layout = layoutRect.Rect( );
    return layout;
    }

// ---------------------------------------------------------
// WholeMainPaneForDialogs
// ---------------------------------------------------------
TRect PostcardLaf::WholeMainPaneForDialogs( )
	{
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
    mainPane.Move( -mainPane.iTl.iX, -mainPane.iTl.iY );
	return mainPane;	
	}

// ---------------------------------------------------------
// ActiveFont
// ---------------------------------------------------------
void PostcardLaf::ActiveFont( TCharFormat& aChar, TCharFormatMask& aCharMask,
						CParaFormat& aPara, TParaFormatMask& aParaMask )
	{
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
    TAknLayoutText textLayout;
    textLayout.LayoutText(
        mainPane,
        AknLayoutScalable_Apps::msg_body_pane_t1( 0 ) );
        
	aPara.iLineSpacingControl = CParaFormat::ELineSpacingExactlyInPixels;
	aParaMask.SetAttrib( EAttLineSpacingControl );
	aPara.iLineSpacingInTwips = MsgEditorCommons::MsgBaseLineDelta();
	aParaMask.SetAttrib( EAttLineSpacing );
	aPara.iHorizontalAlignment = CParaFormat::ELeftAlign;
	aParaMask.SetAttrib( EAttAlignment );
	
	aChar.iFontSpec = textLayout.Font( )->FontSpecInTwips();
    aCharMask.SetAttrib( EAttFontTypeface );
    aCharMask.SetAttrib( EAttFontHeight );

    TRgb textColor;
    if ( AknsUtils::GetCachedColor(
	    AknsUtils::SkinInstance(),
        textColor,
        KAknsIIDQsnTextColors,
        EAknsCIQsnTextColorsCG6 ) != KErrNone )
	    {
	    textColor = AKN_LAF_COLOR_STATIC( 215 );
	    }

	aChar.iFontPresentation.iTextColor = textColor;
    aCharMask.SetAttrib( EAttColor );	
	}

// ---------------------------------------------------------
// MiniatureFont
// ---------------------------------------------------------
void PostcardLaf::MiniatureFont( TCharFormat& aChar, TCharFormatMask& aCharMask,
						CParaFormat& aPara, TParaFormatMask& aParaMask )
	{
	TRect postcardPane = PostcardLaf::GreetingText( );
    
    // 1st row
    TAknLayoutRect rectLayout;
    rectLayout.LayoutRect(
        postcardPane,
        AknLayoutScalable_Apps::message2_row_pane( 1 ).LayoutLine() );

    TRect rowPane = rectLayout.Rect();

    TAknLayoutText textLayout;
    textLayout.LayoutText(
        rowPane,
        AknLayoutScalable_Apps::postcard_message2_row_pane_t1() );
    
	aPara.iLineSpacingControl = CParaFormat::ELineSpacingExactlyInPixels;
	aParaMask.SetAttrib( EAttLineSpacingControl );
	aPara.iLineSpacingInTwips = PostcardLaf::BaselineDelta( );
	aParaMask.SetAttrib( EAttLineSpacing );
	aPara.iHorizontalAlignment = CParaFormat::ELeftAlign;
	aParaMask.SetAttrib( EAttAlignment );
	
	aChar.iFontSpec = textLayout.Font( )->FontSpecInTwips();
    aCharMask.SetAttrib( EAttFontTypeface );
    aCharMask.SetAttrib( EAttFontHeight );
	aChar.iFontPresentation.iTextColor = AKN_LAF_COLOR_STATIC( 215 );
    aCharMask.SetAttrib( EAttColor );	
	}

// ---------------------------------------------------------
// BaselineDelta
// ---------------------------------------------------------
TInt PostcardLaf::BaselineDelta( )
    {
    TRect postcardPane = PostcardLaf::GreetingText( );
    TRect layout(postcardPane);
    TRect layout2(postcardPane);
    
    // 1st row
    TAknLayoutRect rectLayout;
    rectLayout.LayoutRect(
        postcardPane,
        AknLayoutScalable_Apps::message2_row_pane( 1 ).LayoutLine() );

    TRect rowPane = rectLayout.Rect();
    
    TAknLayoutRect textLayout;
    textLayout.LayoutRect(
        rowPane,
        AknLayoutScalable_Apps::postcard_message2_row_pane_g1() );
    layout = textLayout.Rect();
    
    // 2nd row
    TAknLayoutRect rectLayout2;
    rectLayout2.LayoutRect(
        postcardPane,
        AknLayoutScalable_Apps::message2_row_pane( 2 ).LayoutLine() );

    TRect rowPane2 = rectLayout2.Rect();

    TAknLayoutRect textLayout2;
    textLayout2.LayoutRect(
        rowPane2,
        AknLayoutScalable_Apps::postcard_message2_row_pane_g1() );
    layout2 = textLayout2.Rect();
    
    return layout2.iTl.iY - layout.iTl.iY;
    }

// ---------------------------------------------------------
// BaselineTop
// ---------------------------------------------------------
TInt PostcardLaf::BaselineTop( )
    {
    TRect postcardPane = PostcardLaf::GreetingText( );
    TRect layout(postcardPane);
    
    // 1st row
    TAknLayoutRect rectLayout;
    rectLayout.LayoutRect(
        postcardPane,
        AknLayoutScalable_Apps::message2_row_pane( 1 ).LayoutLine() );

    TRect rowPane = rectLayout.Rect();

    TAknLayoutText textLayout;
    textLayout.LayoutText(
        rowPane,
        AknLayoutScalable_Apps::postcard_message2_row_pane_t1() );
    layout = textLayout.TextRect( );
    
    return layout.iTl.iY - postcardPane.iTl.iY;
    }

// ---------------------------------------------------------
// LeftMargin
// ---------------------------------------------------------
TInt PostcardLaf::LeftMargin( )
    {
    TRect postcardPane = PostcardLaf::GreetingText( );
    TRect layout(postcardPane);

    // 1st row
    TAknLayoutRect rectLayout;
    rectLayout.LayoutRect(
        postcardPane,
        AknLayoutScalable_Apps::message2_row_pane( 1 ).LayoutLine() );

    TRect rowPane = rectLayout.Rect();

    TAknLayoutText textLayout;
    textLayout.LayoutText(
        rowPane,
        AknLayoutScalable_Apps::postcard_message2_row_pane_t1() );
    layout = textLayout.TextRect( );

    return layout.iTl.iX - postcardPane.iTl.iX;
    }

// ---------------------------------------------------------
// ScrollBarWidth
// ---------------------------------------------------------
TInt PostcardLaf::ScrollBarWidth( )
	{
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );

    TRect listScrollGenPaneLayout(mainPane);
    if ( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
        {
	    TAknLayoutRect listScrollGenPaneLayoutRect;
	    listScrollGenPaneLayoutRect.LayoutRect(
	        mainPane,
	        AknLayoutScalable_Avkon::main_pane( 0 ).LayoutLine() );
	    listScrollGenPaneLayout = listScrollGenPaneLayoutRect.Rect( );
        }

    TRect scrollPaneLayout(listScrollGenPaneLayout);
    if ( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
        {
	    TAknLayoutRect scrollPaneLayoutRect;
	    scrollPaneLayoutRect.LayoutRect(
	        scrollPaneLayout,
	        AknLayoutScalable_Avkon::scroll_pane( ).LayoutLine() );
	    scrollPaneLayout = scrollPaneLayoutRect.Rect( );
        }

    TRect bgScrollPaneLayout(scrollPaneLayout);
    if ( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
        {
	    TAknLayoutRect bgScrollPaneLayoutRect;
	    bgScrollPaneLayoutRect.LayoutRect(
	        bgScrollPaneLayout,
	        AknLayoutScalable_Avkon::bg_scroll_pane( ).LayoutLine() );
	    bgScrollPaneLayout = bgScrollPaneLayoutRect.Rect( );
        }

	return bgScrollPaneLayout.Width( );
	}

// ---------------------------------------------------------
// GreetingFocusLineCount
// ---------------------------------------------------------
TInt PostcardLaf::GreetingFocusLineCount()
    {
    TAknLayoutScalableParameterLimits limits = AknLayoutScalable_Apps::message2_row_pane_ParamLimits();
    return limits.LastRow();
    }
//  End of File  
