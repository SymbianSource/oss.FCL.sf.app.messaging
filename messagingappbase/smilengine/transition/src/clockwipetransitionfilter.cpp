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
* Description: clockwipetransitionfilter implementation
*
*/




// INCLUDE FILES
#include "clockwipetransitionfilter.h"

#include <e32math.h>
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
// CClockWipeTransitionFilter::CClockWipeTransitionFilter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CClockWipeTransitionFilter::CClockWipeTransitionFilter() :
    iType( EClockWipe ),
    iSubtype( EClockwiseTwelve )
    {
    }

// -----------------------------------------------------------------------------
// CClockWipeTransitionFilter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CClockWipeTransitionFilter::ConstructL( CSmilTransition* aTransition, 
                                             CFbsBitmap* aBuffer, 
                                             MSmilMedia* aMedia )
    {
    CSmilTransitionFilter::ConstructL( aMedia );

    iTransition = aTransition;

    if ( aTransition->Type() == KClockWipe )
        {
        if ( aTransition->Subtype() == KClockwiseThree )
            {
            iSubtype = EClockwiseThree;
            }
        else if ( aTransition->Subtype() == KClockwiseSix )
            {
            iSubtype = EClockwiseSix;
            }
        else if ( aTransition->Subtype() == KClockwiseNine )
            {
            iSubtype = EClockwiseNine;
            }
        }
        
    iBufferBitmap = aBuffer;
    }  

// -----------------------------------------------------------------------------
// CClockWipeTransitionFilter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CClockWipeTransitionFilter* CClockWipeTransitionFilter::NewL( CSmilTransition* aTransition, 
                                                              CFbsBitmap* aBuffer, 
                                                              MSmilMedia* aMedia )
    {
    CClockWipeTransitionFilter* self = new(ELeave) CClockWipeTransitionFilter;
    
    CleanupStack::PushL( self );
    self->ConstructL( aTransition, aBuffer, aMedia );
    CleanupStack::Pop( self );
    
    return self;    
    }  

// -----------------------------------------------------------------------------
// CClockWipeTransitionFilter::~CClockWipeTransitionFilter
// Destructor
// -----------------------------------------------------------------------------
//
CClockWipeTransitionFilter::~CClockWipeTransitionFilter()
    {
    }

// -----------------------------------------------------------------------------
// CClockWipeTransitionFilter::Draw
// Draw method, calculate move and draw bitmaps accordingly. Basic concept of
// this transition is four quarters to which target bitmap is devided. Only one
// of there quarters can active simultaneously. All other quarters are either
// fully visible or invisible depending whether they have been already active
// or not. Fully invisible means that old bitmap that was showing before transition
// is still visible and fully visible means that new bitmap that is visible after
// transition ends is visible.Transition effect is always happening on active quarter.
// Quarters are normally indexed as below diagram shows.
//
//                       |---------------------|
//                       |          |          |
//                       |    3     |    0     |
//                       |          |          |
//                       |---------------------|
//                       |          |          |
//                       |    2     |    1     |
//                       |          |          |
//                       |---------------------|
//
// -----------------------------------------------------------------------------
//
void CClockWipeTransitionFilter::Draw( CGraphicsContext& aGc, 
                                       const TRect& /*aRect*/,
                                       const CFbsBitmap* aTargetBitmap, 
                                       const TPoint& aTarget, 
                                       const CFbsBitmap* aMaskBitmap )
    {
    if ( aTargetBitmap && iType == EClockWipe )
        {
        //Common index used on loops
        TInt index( 0 );
        
        // Target bitmap dimensions
        TInt width( aTargetBitmap->SizeInPixels().iWidth );
        TInt height( aTargetBitmap->SizeInPixels().iHeight );

        // hacky: for transOuts, reverse percents and the reverse flag
        TInt percent = iTransOut ? 100 - iPercent : iPercent;
        
        // Indicates whether transition is proceeding reversed (i.e. non-clockwise)
        TBool reverse = iTransOut ? !iTransition->iReverse : iTransition->iReverse;

        // Specifies the rectangles for different quarters. See function comments diagram
        // for specific position of each quarter.
        TRect quart[4] = { TRect( width / 2, 0, width, height / 2 ),
                           TRect( width / 2, height / 2, width, height ),
                           TRect( 0, height / 2, width / 2, height ),
                           TRect( 0, 0, width / 2, height / 2 ) };

        // Bitmap that was visible before transitions began.
        CFbsBitmap* outBitmap = const_cast<CFbsBitmap*>( iBufferBitmap );
        
        // Bitmap that is visible when transition has ended.
        CFbsBitmap* inBitmap = const_cast<CFbsBitmap*>( aTargetBitmap );
        
        // Quarter shift of the algorithm used to calculate
        // current "transition point" on a circumference around whole
        // transition area. This variable adjusts our algorithm to
        // give different values depending on the subtype.
        TReal quartShift( 0.0 );
        
        TInt quartBeginTime[4] = { 0, 25, 50, 75 };
        
        // Fill in different bitmap quarters begin times
        // depending on the transition effect subtype. Begin
        // times define the order of which transition is proceeding
        // through different quarters.Different subtypes define 
        // on what "position" the clock transition begins.
        switch ( iSubtype )
            {
            case EClockwiseThree:
                {
                quartBeginTime[0] = 75;
                quartBeginTime[1] = 0;
                quartBeginTime[2] = 25;
                quartBeginTime[3] = 50;
                quartShift = KPi / 2.0; // Shift one quarter
                break;
                }
            case EClockwiseSix:
                {
                quartBeginTime[0] = 50;
                quartBeginTime[1] = 75;
                quartBeginTime[2] = 0;
                quartBeginTime[3] = 25;
                quartShift = KPi * 2.0 / 2.0; // Shift two quarters
                break;
                }
            case EClockwiseNine:
                {
                quartBeginTime[0] = 25;
                quartBeginTime[1] = 50;
                quartBeginTime[2] = 75;
                quartBeginTime[3] = 0;
                quartShift = KPi * 3.0 / 2.0; // Shift three quarters
                break;
                }
            case EClockwiseTwelve:
            default:
                {
                break; // No shifting required
                }
            }
        
        // Indicates currently active quarter index. -1 = no active quarter.
        TInt activeQuart( -1 ); 
        
        // Current "transition point" coordinates. Transition point tell
        // us our current place on circumference.
        TReal transitionX( 0.0 );
        TReal transitionY( 0.0 );
        
        // Hypotenuusa used to calculate current transition point.
        TReal radius( 0.0 );
        Math::Sqrt( radius, width * width + height * height );
        
        CFbsBitmap* fillQuartWith[4] = { NULL, NULL, NULL, NULL };
        
        // Fill in the map indicating what bitmap should be used
        // to fill that quarter. Currently active quarter is not
        // filled this way but separately processed.
        if ( reverse )
            {
            // Reversed (i.e. proceeding non-clockwise).
            for ( index = 0; index < 4; index++ )
                {
                if ( ( 100 - percent ) <= quartBeginTime[index] )
                    {
                    // Fully visible quarter
                    fillQuartWith[index] = inBitmap;
                    }
                else if ( ( 100 - percent ) < ( quartBeginTime[index] + 25 ) ) 
                    {
                    // Currently active quarter.
                    fillQuartWith[index] = NULL;
                    activeQuart = index;
                    
                    // With reverse quarter shifting is done back
                    Math::Cos( transitionX, 2.0 * KPi * ( percent / 100.0 ) + KPi / 2.0 - quartShift );
                    transitionX *= radius;
                    
                    Math::Sin( transitionY, 2.0 * KPi * ( percent / 100.0 ) + KPi / 2.0 - quartShift );
                    transitionY *= -radius;
                    }
                else
                    {
                    // Fully invisible quarter
                    fillQuartWith[index] = outBitmap;
                    }
                }
            }
        else 
            {
            for ( index = 0; index < 4; index++ ) 
                {
                if ( percent <= quartBeginTime[index] ) 
                    {
                    // Fully invisible quarter. Begin time greater than current percentage.
                    fillQuartWith[index] = outBitmap;
                    }
                else if ( percent < quartBeginTime[index] + 25 ) 
                    {
                    // Currently active quarter
                    fillQuartWith[index] = NULL;
                    activeQuart = index;
                    
                    // Quarter shifting is done forth
                    Math::Cos( transitionX, 2.0 * KPi * ( percent / 100.0 ) + KPi / 2.0 + quartShift );
                    transitionX *= -radius;
                    
                    Math::Sin( transitionY, 2.0 * KPi * ( percent / 100.0 ) + KPi / 2.0 + quartShift );
                    transitionY *= -radius;
                    }
                else 
                    {
                    // Fully visible quarter. Begin time less than current percentage.
                    fillQuartWith[index] = inBitmap;
                    }
                }
            }

        CBitmapContext* bitmapContext = static_cast<CBitmapContext*>( &aGc );
        
        bitmapContext->SetBrushStyle( CGraphicsContext::ENullBrush );
        bitmapContext->SetPenStyle( CGraphicsContext::ENullPen );
        
        // Fill the full quarters with bitmaps specified on fillQuartWith table. Only
        // draws the inBitmap quarters (i.e. fully visible quarters).
        for ( index = 0; index < 4; index++ ) 
            {
            if ( fillQuartWith[index] != NULL && 
                 fillQuartWith[index] == inBitmap )  
                {
                TPoint bitmapTlc( quart[index].iTl );
                bitmapTlc += aTarget;
                
                if ( aMaskBitmap ) 
                    {
                    bitmapContext->BitBltMasked( bitmapTlc, 
                                                 fillQuartWith[index], 
                                                 quart[index], 
                                                 aMaskBitmap, 
                                                 EFalse );
                    }
                else
                    {
                    bitmapContext->BitBlt( bitmapTlc, 
                                           fillQuartWith[index], 
                                           quart[index] );
                    }
                }
            }
        
        if ( activeQuart != -1 ) 
            {
            // Fill the active quarter
            
            // Top coordinates for active quarter
            TReal topX( 0.0 );
            TReal topY( 0.0 );
        
            // Bottom coordinates for active quarter
            TReal bottomX( 0.0 );
            TReal bottomY( 0.0 );
            
            // Top left corner coordinates for active quarter
            TInt tlX = quart[activeQuart].iTl.iX;
            TInt tlY = quart[activeQuart].iTl.iY;
            
            // Bottom right corner coordinates for active quarter
            TInt brX = quart[activeQuart].iBr.iX;
            TInt brY = quart[activeQuart].iBr.iY;

            // Clip transition point coordinates
            if ( transitionX < -width / 2 ) 
                {
                transitionY = transitionY * ( ( -width / 2 ) / transitionX );
                transitionX = -width / 2;
                }
            
            if ( transitionX > ( width - width / 2 - 1 ) ) 
                {
                transitionY = transitionY * ( ( width - width / 2 - 1 ) / transitionX );
                transitionX = width - width/2;
                }
            
            if ( transitionY < -height / 2 ) 
                {
                transitionX = transitionX * ( ( -height / 2 ) / transitionY );
                transitionY = -height / 2;
                }
            
            if ( transitionY >= ( height - height / 2 - 1 ) ) 
                {
                transitionX = transitionX * ( ( height - height / 2 - 1 ) / transitionY );
                transitionY = height - height / 2 - 1;
                }

            transitionX = transitionX + width / 2;
            transitionY = transitionY + height / 2;

            switch ( activeQuart ) 
                {
                case 0:
                default:
                    {
                    topX = transitionX;
                    topY = transitionY;
                    bottomX = width / 2;
                    bottomY = height / 2;
                    break;
                    }
                case 1:
                    {
                    topX = width / 2;
                    topY = height / 2;
                    bottomX = transitionX;
                    bottomY = transitionY;
                    break;
                    }
                case 2:
                    {
                    topX = width / 2;
                    topY = height / 2;
                    bottomX = transitionX;
                    bottomY = transitionY;
                    break;
                    }
                case 3:
                    {
                    topX = transitionX;
                    topY = transitionY;
                    bottomX = width / 2;
                    bottomY = height / 2;                
                    break;
                    }
                }
            
            TReal dx( bottomX - topX );
            TReal dy( bottomY - topY );
            
            TReal ix( ( dy > 0 ) ? dx / dy : dx / ( -dy ) );

            TBitmapUtil buTarget( const_cast<CFbsBitmap*>( aTargetBitmap ) );
            TBitmapUtil buBuffer( iBufferBitmap );
            
            TBitmapUtil* leftMap = NULL;
            TBitmapUtil* rightMap = NULL;
            
            if ( activeQuart == 0 || activeQuart == 3 ) 
                {
                if( reverse ) 
                    {
                    leftMap = &buBuffer;
                    rightMap = &buTarget;
                    }
                else 
                    {
                    leftMap = &buTarget;
                    rightMap = &buBuffer;
                    }
                }
            else 
                {
                if ( !reverse ) 
                    {
                    leftMap = &buBuffer;
                    rightMap = &buTarget;
                    }
                else 
                    {
                    leftMap = &buTarget;
                    rightMap = &buBuffer;
                    }
                }

            TBitmapUtil* buMask = NULL;

            if ( aMaskBitmap )
                {    
                buMask = new TBitmapUtil( const_cast<CFbsBitmap*>( aMaskBitmap ) );
                // buMask is checked before every use if it is NULL or not 
                }

            buTarget.Begin( TPoint( 0, 0 ) );

            if ( aTarget.iX < 0 || aTarget.iY < 0 )
                {
                buBuffer.Begin( TPoint( 0, 0 ), buTarget );
                }
            
            buBuffer.Begin( aTarget, buTarget );

            if ( buMask )
                {
                buMask->Begin( TPoint( 0, 0 ), buTarget);
                }
                
            // these are the end limits
            TInt regX = Media()->GetRectangle().iBr.iX;
            TInt regY = Media()->GetRectangle().iBr.iY;
            TSize bufbitmapsize =  iBufferBitmap->SizeInPixels();

            for ( TInt y = tlY; 
                  ( y < brY ) && ( y <= regY ) && ( y < bufbitmapsize.iHeight ); 
                  y++ )
                {
                buBuffer.SetPos( TPoint( tlX + aTarget.iX, aTarget.iY + y ) );
                buTarget.SetPos( TPoint( tlX, y ) );

                if ( buMask )
                    {
                    buMask->SetPos( TPoint( tlX, y ) );
                    }

                TInt x;
                TInt txInt = (TInt)topX;
                
                // left map
                for ( x = tlX; 
                      ( x < txInt ) && ( x <= regX ) && ( x < bufbitmapsize.iWidth ); 
                      x++ )
                    {                                        
                    if ( buMask &&
                        !buMask->GetPixel() )
                        {
                        buBuffer.IncXPos();                        
                        buTarget.IncXPos();                        
                        buMask->IncXPos();
                        continue;
                        }
                    
                    // Fix to EABK-6E2HKK
                    TRgb rgb( leftMap->GetPixel() );
                    rgb.SetAlpha( 0 );
                    
                    buBuffer.SetPixel( rgb.Value() );

                    buBuffer.IncXPos();        
                    buTarget.IncXPos();
                    
                    if ( buMask )
                        {
                        buMask->IncXPos();
                        }
                    } // for left map

                // right map
                for ( x = txInt; 
                      ( x < brX ) && ( x <= regX ) && ( x < bufbitmapsize.iWidth ); 
                      x++ )
                    {                                                            
                    if ( buMask &&
                         !buMask->GetPixel() )
                        {
                        buBuffer.IncXPos();                        
                        buTarget.IncXPos();                        
                        buMask->IncXPos();
                        continue;
                        }
                    
                    // Fix to EABK-6E2HKK
                    TRgb rgb( rightMap->GetPixel() );
                    rgb.SetAlpha( 0 );
                    
                    buBuffer.SetPixel( rgb.Value() );

                    buBuffer.IncXPos();                        
                    buTarget.IncXPos();                        
                    
                    if ( buMask )
                        {
                        buMask->IncXPos();
                        }
                    } //for right map

                if ( y < topY ) 
                    {
                    topX = topX;
                    }
                else if ( y > bottomY ) 
                    {
                    topX = bottomX;
                    }
                else 
                    {
                    topX += ix;
                    topY++;            

                    if ( topX <= tlX ) 
                        {
                        topX = tlX;
                        ix = 0;
                        }
                    else if ( topX >= brX ) 
                        {
                        transitionX = brX;
                        ix = 0;
                        }
                    }   
                }
            
            // free bitmapUtils
            if ( buMask )
                {
                buMask->End();
                delete buMask;
                }
                
            buBuffer.End();    
            buTarget.End();
            }
        }
    }

// -----------------------------------------------------------------------------
// CClockWipeTransitionFilter::ResolutionMilliseconds
// -----------------------------------------------------------------------------
//
TInt CClockWipeTransitionFilter::ResolutionMilliseconds() const
    {
    return ESmilTransitionResolution;
    }

//  End of File  
