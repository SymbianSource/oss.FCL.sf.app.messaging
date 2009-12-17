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
* Description:   Folder navigation decorator for navipane
*
*/



// INCLUDES
#include "MceNaviPaneFolderIndicator.h"
#include <StringLoader.h>
#include <muiu.mbg>
#include <mce.rsg>
#include <AknIconArray.h>   // CAknIconArray

// security data caging
#include <data_caging_path_literals.hrh>
#include <bldvariant.hrh>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknDef.h>
#include "MceSessionHolder.h"


// CONSTANTS

const TUint KMceRoot = 0;
const TUint KMceFirstLevel = 1;
_LIT( KMceDirAndFile,"muiu.mbm" );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMceNaviPaneFolderIndicator::CMceNaviPaneFolderIndicator
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMceNaviPaneFolderIndicator::CMceNaviPaneFolderIndicator( 
    TBool aPhoneMemory,
    const TInt aDepth )
    :                                                         
    iMaxDepth( KMceNaviPaneMaxDepth - 1 ),
    iPhoneMemory( aPhoneMemory )
    {
    // ensure that iDepth will be positive
    aDepth < 0 ? iDepth = 0 : iDepth = aDepth;
    }

// -----------------------------------------------------------------------------
// CMceNaviPaneFolderIndicator::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMceNaviPaneFolderIndicator::ConstructL()
    {
    // security data caging
    TParse fp;
    fp.Set( KMceDirAndFile, &KDC_APP_BITMAP_DIR , NULL );
    iFilename = fp.FullName();

    iSeparator = StringLoader::LoadL( R_MEMC_MAIN_SEPARATOR, iEikonEnv );
    iEndChar = StringLoader::LoadL( R_MEMC_MAIN_END, iEikonEnv );     
   
    LoadFolderBitmapL();
    // Set the default root bitmap
    ChangeRootL( iPhoneMemory );
    }

// -----------------------------------------------------------------------------
// CMceNaviPaneFolderIndicator::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMceNaviPaneFolderIndicator* CMceNaviPaneFolderIndicator::NewL(
    TBool aPhoneMemory,
    const TInt aDepth )
    {
    CMceNaviPaneFolderIndicator* self = CMceNaviPaneFolderIndicator::NewLC( 
        aPhoneMemory, aDepth );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMceNaviPaneFolderIndicator::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMceNaviPaneFolderIndicator* CMceNaviPaneFolderIndicator::NewLC( 
    TBool aPhoneMemory,
    const TInt aDepth )
    {
    CMceNaviPaneFolderIndicator* self=new(ELeave) CMceNaviPaneFolderIndicator( 
        aPhoneMemory, aDepth );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
CMceNaviPaneFolderIndicator::~CMceNaviPaneFolderIndicator()
    {
    delete iFolderBitmap;
    delete iFolderMask;

    delete iRootBitmap;
    delete iRootMask;

    delete iSeparator;
    delete iEndChar;
    }
// -----------------------------------------------------------------------------
// CMceNaviPaneFolderIndicator::SetFolderDepth
// Sets the current folder navigation depth
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMceNaviPaneFolderIndicator::SetFolderDepth( const TInt aDepth )
    {
    aDepth < 0 ? iDepth = 0 : iDepth = aDepth;

    // ReportChange() is used to notify CoeControl to redraw pane.
    ReportChange();
    }

// -----------------------------------------------------------------------------
// CMceNaviPaneFolderIndicator::SizeChanged
// Handles size changes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMceNaviPaneFolderIndicator::SizeChanged()
    {
    TSize size(  TSize::EUninitialized );
    AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::ENaviPane, size );
    TRect parentRect( size );

    // Get pane icon and text layouts
    TAknWindowLineLayout paneIconLayout(
        AknLayoutScalable_Avkon::navi_icon_text_pane_g1().LayoutLine() );
    TAknTextLineLayout paneTextLayout(
        AknLayoutScalable_Avkon::navi_icon_text_pane_t1().LayoutLine() );
    TAknLayoutRect layoutRect;
    TAknLayoutText layoutText;
    layoutRect.LayoutRect( parentRect, paneIconLayout );
    layoutText.LayoutText( parentRect, paneTextLayout );
    // Setup pane items
    TRect itemsRect( layoutRect.Rect() );
    const CFont* font( layoutText.Font() ); // Not owned
    TInt bsWidth( font->MeasureText( *iSeparator ) );
    TInt dotsWidth( font->MeasureText( *iEndChar ) );
    TInt textWidth( KMceNaviPaneMaxDepth * bsWidth + dotsWidth );
    TInt iconWidth( ( itemsRect.Width() - textWidth )
        / KMceNaviPaneMaxDepth );
    TInt i( 0 );

    if ( AknLayoutUtils::LayoutMirrored() )
        {
        TInt offs( itemsRect.Width() );
        for ( i = 0 ; i < KMceNaviPaneMaxDepth; i++ )
            {
        // Root and Folder Icons
            offs -= iconWidth;
            iBitmapLayout[ i ].LayoutRect(
                parentRect,
                paneIconLayout.iC,
                paneIconLayout.il + offs,
                paneIconLayout.it,
                paneIconLayout.ir,
                paneIconLayout.ib,
                iconWidth,
                paneIconLayout.iH
                );
        // Backslashes
            offs -= bsWidth;
            iTextLayout[ i ].LayoutText(
                parentRect,
                paneTextLayout.iFont,
                paneTextLayout.iC,
                paneTextLayout.il + offs,
                paneTextLayout.ir,
                paneTextLayout.iB,
                paneTextLayout.iW,
                paneTextLayout.iJ
                );
            }
        // Dots
        offs -= dotsWidth;
        iTextLayout[ i ].LayoutText(
            parentRect,
            paneTextLayout.iFont,
            paneTextLayout.iC,
            paneTextLayout.il + offs,
            paneTextLayout.ir,
            paneTextLayout.iB,
            paneTextLayout.iW,
            paneTextLayout.iJ
            );
        }
    else
        {
        TInt offs( 0 );
        for ( i = 0 ; i < KMceNaviPaneMaxDepth; i++ )
            {
        // Root and Folder Icons
            iBitmapLayout[ i ].LayoutRect(
                parentRect,
                paneIconLayout.iC,
                paneIconLayout.il + offs,
                paneIconLayout.it,
                paneIconLayout.ir,
                paneIconLayout.ib,
                iconWidth,
                paneIconLayout.iH
                );
            offs += iconWidth;
        // Backslashes
            iTextLayout[ i ].LayoutText(
                parentRect,
                paneTextLayout.iFont,
                paneTextLayout.iC,
                paneTextLayout.il + offs,
                paneTextLayout.ir,
                paneTextLayout.iB,
                paneTextLayout.iW,
                paneTextLayout.iJ
                );
            offs += bsWidth;
            }
        // Dots
        iTextLayout[ i ].LayoutText(
            parentRect,
            paneTextLayout.iFont,
            paneTextLayout.iC,
            paneTextLayout.il + offs,
            paneTextLayout.ir,
            paneTextLayout.iB,
            paneTextLayout.iW,
            paneTextLayout.iJ
            );
        }
    }
// -----------------------------------------------------------------------------
// CMceNaviPaneFolderIndicator::Draw
// Handles drawing of the decorator
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMceNaviPaneFolderIndicator::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();    
    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();    
    
    CFbsBitmap* bmp = NULL;
    TRgb color( KRgbWhite ); // Default never used
    bmp = AknsUtils::GetCachedBitmap( skin, KAknsIIDQsnIconColors );
    if (!IsLandscapeScreenOrientation()) 
		{
		AknsUtils::GetCachedColor( skin, color, 
			KAknsIIDQsnIconColors, EAknsCIQsnIconColorsCG7 );
		}
	else
		{
    AknsUtils::GetCachedColor( skin, color, 
			KAknsIIDQsnIconColors, EAknsCIQsnIconColorsCG25 );
		}
   
    AknIconUtils::SetSize( iRootMask, iBitmapLayout[ KMceRoot ].Rect().Size() );
    if( bmp )
        {
        iBitmapLayout[ KMceRoot ].DrawImage( gc, bmp, iRootMask );
        }
    else
        {
        AknIconUtils::SetSize( iRootBitmap, iBitmapLayout[ KMceRoot ].Rect().Size() );
        iBitmapLayout[ KMceRoot ].DrawImage( gc, iRootBitmap, iRootMask );
        }

    if( !bmp )
        {
        AknIconUtils::SetSize( iFolderBitmap, iBitmapLayout[ KMceFirstLevel ].Rect().Size() );
        bmp = iFolderBitmap;
        }


    // draw folder bitmaps depending on current folderdepth
    TInt count( iDepth > iMaxDepth ? iMaxDepth : iDepth );
    AknIconUtils::SetSize( iFolderMask, iBitmapLayout[ KMceFirstLevel ].Rect().Size() );
    for( TInt i( KMceFirstLevel ); i <= count; i++ )
        {
        iBitmapLayout[ i ].DrawImage( gc, bmp, iFolderMask );
        }

    // draw backslashes between folderbitmaps depending on current folderdepth
    TInt tc( iDepth > ( iMaxDepth + 1 ) ? ( iMaxDepth + 1 ) : iDepth );
    for( TInt j( 0 ); j < tc; j++ )
        {
        const CFont* font = iTextLayout[j].Font(); // Not owned
        gc.UseFont( font );
        gc.SetPenColor( color );
        gc.DrawText( *iSeparator, iTextLayout[j].TextRect(),
                     font->AscentInPixels(), CGraphicsContext::ELeft, 0 );
        }

    // draw "..." at the end if necessary
    if ( iDepth > iMaxDepth )
        {
        const CFont* font = iTextLayout[iMaxDepth+1].Font(); // Not owned
        gc.UseFont( font );
        gc.SetPenColor( color );
        gc.DrawText( *iEndChar, iTextLayout[iMaxDepth+1].TextRect(),
                     font->AscentInPixels(), CGraphicsContext::ELeft, 0 );
        }
    }

// -----------------------------------------------------------------------------
// CMceNaviPaneFolderIndicator::HandleResourceChange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMceNaviPaneFolderIndicator::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        SizeChanged();
        }
    else if( aType == KAknsMessageSkinChange )
            {
            TRAP_IGNORE( LoadFolderBitmapL() );
            // Set the default root bitmap
            TRAP_IGNORE( ChangeRootL( iPhoneMemory ) );
            }
    }

// -----------------------------------------------------------------------------
// CMceNaviPaneFolderIndicator::ReportChange
// Reports changes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMceNaviPaneFolderIndicator::ReportChange()
    {
    // Leave is trapped because it's not critical to functionality.
    // If leave occurs, navipane just may not be redrawn at that very moment.
    TRAP_IGNORE( ReportEventL( MCoeControlObserver::EEventStateChanged ) );
    }

// -----------------------------------------------------------------------------
// CMceNaviPaneFolderIndicator::ChangeRootL
// Changes the root bitmap
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMceNaviPaneFolderIndicator::ChangeRootL( TBool aPhoneMemory )
        {
    iPhoneMemory = aPhoneMemory;
        delete iRootBitmap;
        iRootBitmap = NULL;
        delete iRootMask;
        iRootMask = NULL;

    if ( iPhoneMemory )
        {
        AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
            KAknsIIDQgnPropMemcPhoneTab,
            KAknsIIDQsnIconColors,
            EAknsCIQsnIconColorsCG7,
            iRootBitmap, 
            iRootMask,
            iFilename,
            EMbmMuiuQgn_prop_memc_phone_tab, 
            EMbmMuiuQgn_prop_memc_phone_tab+1,
            KRgbBlack );
        }
    else
        {
        AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
            KAknsIIDQgnPropMemcMmcTab,
            KAknsIIDQsnIconColors,
            EAknsCIQsnIconColorsCG7,
            iRootBitmap, 
            iRootMask,
            iFilename,
            EMbmMuiuQgn_prop_memc_mmc_tab, 
            EMbmMuiuQgn_prop_memc_mmc_tab+1,
            KRgbBlack );
        }
    }

// -----------------------------------------------------------------------------
// CMceNaviPaneFolderIndicator::LoadFolderBitmapL
// Loads Folder Bitmaps
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMceNaviPaneFolderIndicator::LoadFolderBitmapL()
    {

    delete iFolderBitmap;
    iFolderBitmap = NULL;
    delete iFolderMask;
    iFolderMask = NULL;
               
    AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
        KAknsIIDQgnPropFolderTab,
        KAknsIIDQsnIconColors,
        EAknsCIQsnIconColorsCG7,
        iFolderBitmap, 
        iFolderMask,
        iFilename,
        EMbmMuiuQgn_prop_folder_tab, 
        EMbmMuiuQgn_prop_folder_tab+1,
        KRgbBlack );
        }
  
// -----------------------------------------------------------------------------
// CMceNaviPaneFolderIndicator::IsLandscapeScreenOrientation
// Checks phone screen mode Landscape or Portrait
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CMceNaviPaneFolderIndicator::IsLandscapeScreenOrientation() const	
	{
	TRect rect;
	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, rect);
	return rect.Width() > rect.Height();
	}  
  
// End of File
