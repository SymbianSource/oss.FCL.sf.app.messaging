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
* Description: fadetransitionfilter implementation
*
*/



// INCLUDES
#include "fadetransitionfilter.h"

#include <gdi.h>
#include <fbs.h>
#include <w32std.h>

#include "smiltransitioncommondefs.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFadeTransitionFilter::CBarWipeTransitionFilter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFadeTransitionFilter::CFadeTransitionFilter() :
    iType( EFade ),
    iSubtype( ECrossfade )
    {
    }

// -----------------------------------------------------------------------------
// CFadeTransitionFilter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFadeTransitionFilter::ConstructL( CSmilTransition* aTransition, 
                                        CFbsBitmap* aBuffer, 
                                        MSmilMedia* aMedia )
    {
    CSmilTransitionFilter::ConstructL( aMedia );

    iTransition = aTransition;

    if ( aTransition->Type() == KFade )
        {
        iType = EFade;
        
        if ( aTransition->Subtype() == KFadeToColor )
            {
            iSubtype = EFadeToColor;
            }
        else if ( aTransition->Subtype() == KFadeFromColor )
            {
            iSubtype = EFadeFromColor;
            }
        else
            {
            iSubtype = ECrossfade;
            }
        }

    iBufferBitmap = aBuffer;
    }

// -----------------------------------------------------------------------------
// CFadeTransitionFilter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFadeTransitionFilter* CFadeTransitionFilter::NewL( CSmilTransition* aTransition, 
                                                    CFbsBitmap* aBuffer, 
                                                    MSmilMedia* aMedia )
    {
    CFadeTransitionFilter* self = new(ELeave) CFadeTransitionFilter();
    
    CleanupStack::PushL( self );
    self->ConstructL( aTransition, aBuffer, aMedia );
    CleanupStack::Pop( self );
    
    return self;        
    }

// -----------------------------------------------------------------------------
// CFadeTransitionFilter::~CFadeTransitionFilter
// Destructor
// -----------------------------------------------------------------------------
//
CFadeTransitionFilter::~CFadeTransitionFilter()
    {
    }

// -----------------------------------------------------------------------------
// CFadeTransitionFilter::Draw
// Draw method, calculate progession and draw bitmaps accordingly.
// -----------------------------------------------------------------------------
//
void CFadeTransitionFilter::Draw( CGraphicsContext& /*aGc*/, 
                                  const TRect& /*aRect*/, 
                                  const CFbsBitmap* aTargetBitmap, 
                                  const TPoint& aTarget, 
                                  const CFbsBitmap* aMaskBitmap )
    {
    if ( aTargetBitmap )
        {
        if ( iType == EFade )
            {            
            TSize size =  aTargetBitmap->SizeInPixels();
            TRect region = Media()->GetRectangle();

            TSize bsize =  iBufferBitmap->SizeInPixels();    
            
            if ( size.iWidth > region.Width() )
                {
                size.iWidth = region.Width();
                }
                
            if ( size.iHeight > region.Height() )
                {
                size.iHeight = region.Height();
                }
            
            TBitmapUtil bu ( const_cast<CFbsBitmap*>( aTargetBitmap ) );
            TBitmapUtil bub( iBufferBitmap );

            //bum is checked before every use if it is NULL or not 
            TBitmapUtil* bum = NULL;

            if ( aMaskBitmap )
                {
                bum = new TBitmapUtil( const_cast<CFbsBitmap*>( aMaskBitmap ) );
                }
            
            // lock the heap
            bu.Begin( TPoint( 0,0 ) );
            bub.Begin( TPoint( 0 + aTarget.iX, 0 + aTarget.iY ), bu );
            
            if ( bum )
                {
                bum->Begin( TPoint( 0, 0 ), bu );
                }
            
            TRgb rgb;
            TRgb rgb2;
            TInt alpha;
            TInt32 px;
            
            TInt red;
            TInt green;
            TInt blue;
            
            TInt factor1 = ( iPercent << 7 ) / 100;    
            TInt factor2 = ( ( 100 - iPercent ) << 7 ) / 100;
            
            for ( TInt y = 0; ( y < size.iHeight ) && ( aTarget.iY + y ) < bsize.iHeight; y++ )
                {            
                bub.SetPos( TPoint( aTarget.iX , aTarget.iY + y ) );
                bu.SetPos( TPoint ( 0 , y ) );

                if ( bum )
                    {
                    bum->SetPos( TPoint( 0 , y ) );
                    }

                for ( TInt x = 0; x < size.iWidth && ( aTarget.iX + x ) < bsize.iWidth ; x++ )
                    {                                                                            
                    px = bu.GetPixel();

                    if ( bum )
                        {
                        // test transparency mask
                        if ( !bum->GetPixel() )
                            {
                            bub.IncXPos();                        
                            bu.IncXPos();                        
                            bum->IncXPos();
                            continue;
                            }
                        }

                    switch ( iSubtype )
                        {
                        /* according to SMIL errata E38: "The "fadeToColor" and 
                        "fadeFromColor" subtypes are equivalent. The fade direction 
                         is determined by whether it is used as transIn or transOut."*/
                        case EFadeFromColor:
                        case EFadeToColor:
                            {
                            if ( iTransOut )
                                {
                                rgb = TRgb( px );    
                                rgb2 = iTransition->iFadeColor;
                                }
                            else
                                {                                        
                                rgb = iTransition->iFadeColor;
                                rgb2 = TRgb( px );
                                }
                            break;
                            }
                        case ECrossfade:
                        default:
                            {
                            if ( iTransOut )
                                {
                                rgb = TRgb( px );
                                rgb2 = TRgb( bub.GetPixel() );
                                }
                            else
                                {
                                rgb = TRgb( bub.GetPixel() );
                                rgb2 = TRgb( px );
                                }
                            break;
                            }
                        }
                    alpha = 0;
                    red = ( ( rgb.Red() * factor2 ) >> 7 ) + ( ( rgb2.Red() * factor1 ) >> 7 );
                    green = ( ( rgb.Green() * factor2 ) >> 7 ) + ( ( rgb2.Green() * factor1 ) >> 7 );
                    blue = ( ( rgb.Blue() * factor2 ) >> 7 ) + ( ( rgb2.Blue() * factor1 ) >> 7 );
                    
                    bub.SetPixel( TRgb( red, green, blue, alpha ).Value() );
                        
                    bub.IncXPos();                        
                    bu.IncXPos();
                    
                    if ( bum )
                        {
                        bum->IncXPos();
                        }
                    }
                }

            bu.End();
            bub.End();    

            if ( bum )
                {
                bum->End();
                delete bum;
                }
            
            }
        }
    }

// -----------------------------------------------------------------------------
// CFadeTransitionFilter::ResolutionMilliseconds
// -----------------------------------------------------------------------------
//
TInt CFadeTransitionFilter::ResolutionMilliseconds() const
    {
    return ESmilTransitionResolution;
    }

//  End of File  
