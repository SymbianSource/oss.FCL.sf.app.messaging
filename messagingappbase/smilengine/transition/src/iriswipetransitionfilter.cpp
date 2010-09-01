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
* Description: iriswipetransitionfilter implementation
*
*/



// INCLUDES
#include "iriswipetransitionfilter.h"

#include <gdi.h>
#include <fbs.h>
#include <w32std.h>
#include <trpoint.h>

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
// CIrisWipeTransitionFilter::CIrisWipeTransitionFilter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIrisWipeTransitionFilter::CIrisWipeTransitionFilter() :
    iType( EIrisWipe ),
    iSubtype( ERectangle )
    {
    }

// -----------------------------------------------------------------------------
// CIrisWipeTransitionFilter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIrisWipeTransitionFilter::ConstructL( CSmilTransition* aTransition,
                                            CFbsBitmap* /*aBuffer*/, 
                                            MSmilMedia* aMedia )
    {
    CSmilTransitionFilter::ConstructL( aMedia );

    iTransition = aTransition;

    iShapeVertices = new(ELeave) RArray<TRPoint>( 2 );
    iScaleMax = 0.0;
    
    if ( aTransition->Type() == KTriangleWipe ) 
        {
        iType = ETriangleWipe;

        if ( aTransition->Subtype() == KRight ) 
            {
            iShapeVertices->Append( TRPoint( -1, -1 ) );
            iShapeVertices->Append( TRPoint( 1, 0 ) );
            iShapeVertices->Append( TRPoint( -1, 1 ) );
            iSubtype = ERight;
            }
        else if ( aTransition->Subtype() == KDown ) 
            {
            iShapeVertices->Append( TRPoint( -1, -1 ) );
            iShapeVertices->Append( TRPoint( 1, -1 ) );
            iShapeVertices->Append( TRPoint( 0, 1 ) );
            iSubtype = EDown;
            }
        else if ( aTransition->Subtype() == KLeft ) 
            {
            iShapeVertices->Append( TRPoint( -1, 0 ) );
            iShapeVertices->Append( TRPoint( 1, -1 ) );
            iShapeVertices->Append( TRPoint( 1, 1 ) );
            iSubtype = ELeft;
            }
        else 
            {
            iShapeVertices->Append( TRPoint( 0, -1 ) );
            iShapeVertices->Append( TRPoint( 1, 1 ) );
            iShapeVertices->Append( TRPoint( -1, 1 ) );
            iSubtype = EUp;
            }
        }
    else if ( aTransition->Type() == KArrowHeadWipe ) 
        {
        iType = EArrowHeadWipe;

        if ( aTransition->Subtype() == KRight ) 
            {
            iShapeVertices->Append( TRPoint( -1, -1 ) );
            iShapeVertices->Append( TRPoint( 1, 0 ) );
            iShapeVertices->Append( TRPoint( -1, 1 ) );
            iShapeVertices->Append( TRPoint( -0.75, 0 ) );
            iSubtype = ERight;
            }
        else if ( aTransition->Subtype() == KDown ) 
            {
            iShapeVertices->Append( TRPoint( -1, -1 ) );
            iShapeVertices->Append( TRPoint( 0, -0.75 ) );
            iShapeVertices->Append( TRPoint( 1, -1 ) );
            iShapeVertices->Append(TRPoint( 0, 1 ) );
            iSubtype = EDown;
            }
        else if ( aTransition->Subtype() == KLeft ) 
            {
            iShapeVertices->Append( TRPoint( -1, 0 ) );
            iShapeVertices->Append( TRPoint( 1, -1 ) );
            iShapeVertices->Append( TRPoint( 0.75, 0 ) );
            iShapeVertices->Append( TRPoint( 1, 1 ) );
            iSubtype = ELeft;
            }
        else 
            {
            iShapeVertices->Append( TRPoint( 0, -1 ) );
            iShapeVertices->Append( TRPoint( 1, 1 ) );
            iShapeVertices->Append( TRPoint( 0, 0.75 ) );
            iShapeVertices->Append( TRPoint( -1, 1 ) );
            iSubtype = EUp;
            }
        }
    else if ( aTransition->Type() == KPentagonWipe ) 
        {
        iType = EPentagonWipe;
        
        if ( aTransition->Subtype() == KDown ) 
            {
            iShapeVertices->Append( TRPoint( 0, 2.5 ) );
            iShapeVertices->Append( TRPoint( 2.5, 0.5 ) );
            iShapeVertices->Append( TRPoint( 1.5, -2.5 ) );
            iShapeVertices->Append( TRPoint( -1.5, -2.5 ) );
            iShapeVertices->Append( TRPoint( -2.5, 0.5 ) );
            iSubtype = EDown;
            }
        else 
            {
            iShapeVertices->Append( TRPoint( 0, -2.5 ) );
            iShapeVertices->Append( TRPoint( 2.5, -0.5 ) );
            iShapeVertices->Append( TRPoint( 1.5, 2.5 ) );
            iShapeVertices->Append( TRPoint( -1.5, 2.5 ) );
            iShapeVertices->Append( TRPoint( -2.5, -0.5 ) );
            iSubtype = EUp;
            }
        }
    else if ( aTransition->Type() == KHexagonWipe ) 
        {
        iType = EHexagonWipe;

        if ( aTransition->Subtype() == KVertical ) 
            {
            TReal x,y;

            for ( TInt i = 0; i < 360; i += 60 ) 
                {
                Math::Cos( x, ( i + 30.0 ) * 2 * KPi / 360.0 );
                Math::Sin( y, ( i + 30.0 ) * 2 * KPi / 360.0 );
                iShapeVertices->Append( TRPoint( x, y ) );
                }

            iSubtype = EVertical;
            }
        else 
            {
            TReal x,y;

            for ( TInt i = 0; i < 360; i += 60 ) 
                {
                Math::Cos( x, i * 2 * KPi / 360.0 );
                Math::Sin( y, i * 2 * KPi / 360.0 );
                iShapeVertices->Append( TRPoint( x, y ) );
                }

            iSubtype = EHorizontal;
            }
        }
    else if ( aTransition->Type() == KEllipseWipe ) 
        {
        iType = EEllipseWipe;

        TReal a, b;

        if ( aTransition->Subtype() == KVertical ) 
            {
            a = 1.0;
            b = 1.5;
            iSubtype = EVertical;
            }
        else if ( aTransition->Subtype() == KHorizontal ) 
            {
            a = 1.5;
            b = 1.0;
            iSubtype = EHorizontal;
            }
        else 
            {
            a = 1.0;
            b = 1.0;
            iSubtype = ECircle;
            }

        for ( TReal i = 0; i < 360.0; i += 10.0) 
            {
            TReal x,y;
            Math::Cos( x, i * 2 * KPi / 360.0 );
            Math::Sin( y, i * 2 * KPi / 360.0 );
            iShapeVertices->Append( TRPoint( x * a, y * b ) );            
            }
        }
    else if ( aTransition->Type() == KEyeWipe ) 
        {
        iType = EEyeWipe;
        TReal w = 6.0 * 6.0;

        if ( aTransition->Subtype() == KVertical ) 
            {
            TReal x, y, b;

            Math::Sqrt( b, 1 + w );

            for ( y = -1.0001; y < 1; y += 0.1) 
                {
                Math::Sqrt( x, 1 + ( w * y * y ) );
                x = x * 0.1 - 0.1 * b;
                iShapeVertices->Append( TRPoint( x, y ) );
                }

            for ( y = 1.0001; y > -1; y -= 0.1) 
                {
                Math::Sqrt( x, 1 + ( w * y * y ) );
                x = x * 0.1 - 0.1 * b;
                iShapeVertices->Append( TRPoint( -x, y ) );
                }

            iSubtype = EVertical;
            }
        else 
            {
            TReal x, y, b;

            Math::Sqrt( b, 1 + w );

            for ( x = -1.0001; x < 1; x += 0.1 ) 
                {
                Math::Sqrt( y, 1 + ( w * x * x ) );
                y = y * 0.1 - 0.1 * b;
                iShapeVertices->Append( TRPoint( x, y ) );
                }

            for ( x = 1.0001; x > -1; x -= 0.1 ) 
                {
                Math::Sqrt( y, 1 + ( w * x * x ) );
                y = y * 0.1 - 0.1 * b;
                iShapeVertices->Append( TRPoint( x, -y ) );
                }

            iSubtype = EHorizontal;
            }
        }
    else if ( aTransition->Type() == KRoundRectWipe ) 
        {
        iType = ERoundRectWipe;
        
        if ( aTransition->Subtype() == KVertical ) 
            {
            TReal x, y;
            
            TInt i;
            for ( i = 0; i <= 90; i += 5 ) 
                {
                Math::Cos( x, ( i - 90.0 ) * 2 * KPi / 360.0 );
                Math::Sin( y, ( i - 90.0 ) * 2 * KPi / 360.0 );
                iShapeVertices->Append( TRPoint( x + 1, y - 2 ) );
                }

            for ( i = 0; i <= 90; i += 5 ) 
                {
                Math::Cos( x, (i) * 2 * KPi / 360.0 );
                Math::Sin( y, (i) * 2 * KPi / 360.0 );
                iShapeVertices->Append( TRPoint( x + 1, y + 2 ) );
                }

            for ( i = 0; i <= 90; i += 5 ) 
                {
                Math::Cos( x, ( i + 90.0 ) * 2 * KPi / 360.0 );
                Math::Sin( y, ( i + 90.0 ) * 2 * KPi / 360.0 );
                iShapeVertices->Append( TRPoint( x - 1, y + 2 ) );
                }

            for ( i = 0; i <= 90; i += 5 ) 
                {
                Math::Cos( x, ( i + 180.0 ) * 2 * KPi / 360.0 );
                Math::Sin( y, ( i + 180.0 ) * 2 * KPi / 360.0 );
                iShapeVertices->Append( TRPoint( x - 1, y - 2 ) );
                }

            iSubtype = EVertical;
            }
        else 
            {
            TReal x, y;
            TInt i;
            for ( i = 0; i <= 90; i += 5 ) 
                {
                Math::Cos( x, ( i - 90.0 ) * 2 * KPi / 360.0 );
                Math::Sin( y, ( i - 90.0 ) * 2 * KPi / 360.0 );
                iShapeVertices->Append( TRPoint( x + 2, y - 1 ) );
                }

            for ( i = 0; i <= 90; i += 5 ) 
                {
                Math::Cos( x, (i) * 2 * KPi / 360.0 );
                Math::Sin( y, (i) * 2 * KPi / 360.0 );
                iShapeVertices->Append( TRPoint( x + 2, y + 1 ) );
                }

            for ( i = 0; i <= 90; i += 5 ) 
                {
                Math::Cos( x, ( i + 90.0 ) * 2 * KPi / 360.0 );
                Math::Sin( y, ( i + 90.0 ) * 2 * KPi / 360.0);
                iShapeVertices->Append( TRPoint( x - 2, y + 1 ) );
                }

            for ( i = 0; i <= 90; i += 5 ) 
                {
                Math::Cos( x, ( i + 180.0 ) * 2 * KPi / 360.0 );
                Math::Sin( y, ( i + 180.0 ) * 2 * KPi / 360.0 );
                iShapeVertices->Append( TRPoint( x - 2, y - 1 ) );
                }

            iSubtype = EHorizontal;
            }
        }
    else if ( aTransition->Type() == KStarWipe ) 
        {
        iType = EStarWipe;
        
        if ( aTransition->Subtype() == KFivePoint ) 
            {
            TReal x,y;

            for ( TInt i = 0; i < 360; i += 36 ) 
                {
                Math::Cos( x, ( i + 90.0 ) * 2 * KPi / 360.0 );
                Math::Sin( y, ( i + 90.0 ) * 2 * KPi / 360.0 );
                TReal r = ( 1 + ( ( i / 36 ) & 1 ) ) * 0.5;
                iShapeVertices->Append( TRPoint( x * r, y * r ) );
                }

            iSubtype = EFivePoint;
            }
        else if ( aTransition->Subtype() == KSixPoint ) 
            {
            TReal x,y;

            for ( TInt i = 0; i < 360; i += 30 ) 
                {
                Math::Cos( x, ( i + 60.0 ) * 2 * KPi / 360.0 );
                Math::Sin( y, ( i + 60.0 ) * 2 * KPi / 360.0 );
                TReal r = ( 1 + ( ( i / 30 ) & 1 ) ) * 0.5;
                iShapeVertices->Append( TRPoint( x * r, y * r ) );
                }

            iSubtype = ESixPoint;
            }
        else 
            {
            TReal x,y;

            for ( TInt i = 0; i < 360; i += 45 ) 
                {
                Math::Cos( x, ( i + 45.0 ) * 2 * KPi / 360.0 );
                Math::Sin( y, ( i + 45.0 ) * 2 * KPi / 360.0 );
                TReal r = ( 1 + ( ( i / 45 ) & 1 ) ) * 0.5;
                iShapeVertices->Append( TRPoint( x * r, y * r ) );
                }

            iSubtype = EFourPoint;
            }
        }
    else if ( aTransition->Type() == KMiscShapeWipe ) 
        {
        iType = EMiscShapeWipe;

        if ( aTransition->Subtype() == KKeyhole ) 
            {
            for ( TReal i = -180 - 70; i <= 70; i += 10.0 ) 
                {
                TReal x,y;
                Math::Cos( x, i * 2 * KPi / 360.0 );
                Math::Sin( y, i * 2 * KPi / 360.0 );
                iShapeVertices->Append( TRPoint( x * 0.5, y * 0.5 - 1 ) );            
                }

            iShapeVertices->Append( TRPoint( 0.6, 1.0 ) );
            iShapeVertices->Append( TRPoint( -0.6, 1.0 ) );

            iSubtype = EKeyhole;
            }
        else 
            {
            TReal i;
            for ( i = -180; i < 0; i += 10.0 ) 
                {
                TReal x,y;
                Math::Cos( x, i * 2 * KPi / 360.0 );
                Math::Sin( y, i * 2 * KPi / 360.0 );
                iShapeVertices->Append( TRPoint( x - 1, y - 1 ) );            
                }

            for ( i = -180; i < 0; i += 10.0 ) 
                {
                TReal x,y;
                Math::Cos( x, i * 2 * KPi / 360.0 );
                Math::Sin( y, i * 2 * KPi / 360.0 );
                iShapeVertices->Append( TRPoint( x + 1, y - 1 ) );            
                }

            iShapeVertices->Append( TRPoint( 0.0, 3.0 ) );            

            iSubtype = EHeart;
            }
        }
    else 
        {
        iType = EIrisWipe;

        if ( aTransition->Subtype() == KDiamond ) 
            {
            iShapeVertices->Append( TRPoint( 0, -1 ) );
            iShapeVertices->Append( TRPoint( 1, 0 ) );
            iShapeVertices->Append( TRPoint( 0, 1 ) );
            iShapeVertices->Append( TRPoint( -1, 0 ) );
            iSubtype = EDiamond;
            }
        else 
            {
            iShapeVertices->Append( TRPoint( -1, -1 ) );
            iShapeVertices->Append( TRPoint( 1, -1 ) );
            iShapeVertices->Append( TRPoint( 1, 1 ) );
            iShapeVertices->Append( TRPoint( -1, 1 ) );
            iSubtype = ERectangle;
            }
        }

    // scale shape according to iPercent and iReverse
    iPShape = new(ELeave) CArrayFixFlat<TPoint>( iShapeVertices->Count() );
    }

// -----------------------------------------------------------------------------
// CIrisWipeTransitionFilter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIrisWipeTransitionFilter* CIrisWipeTransitionFilter::NewL( CSmilTransition* aTransition,
                                                            CFbsBitmap* aBuffer, 
                                                            MSmilMedia* aMedia )
    {
    CIrisWipeTransitionFilter* self = new(ELeave) CIrisWipeTransitionFilter();
    
    CleanupStack::PushL( self );
    self->ConstructL( aTransition, aBuffer, aMedia );
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// CIrisWipeTransitionFilter::~CIrisWipeTransitionFilter
// Destructor.
// -----------------------------------------------------------------------------
//
CIrisWipeTransitionFilter::~CIrisWipeTransitionFilter()
    {
    if ( iShapeVertices ) 
        {
        iShapeVertices->Reset();
        delete iShapeVertices;
        }
        
    if ( iPShape )
        {
        delete iPShape;
        }
    }

// -----------------------------------------------------------------------------
// CIrisWipeTransitionFilter::Draw
// Draw method, calculate shapes and draw bitmaps accordingly.
// -----------------------------------------------------------------------------
//
void CIrisWipeTransitionFilter::Draw( CGraphicsContext& aGc,
                                      const TRect& /*aRect*/,
                                      const CFbsBitmap* aTargetBitmap,
                                      const TPoint& aTarget,
                                      const CFbsBitmap* aMaskBitmap )
    {
    if ( !aTargetBitmap )
        {
        return;
        }

    if ( iType == EIrisWipe ||
         iType == ETriangleWipe ||
         iType == EArrowHeadWipe ||
         iType == EEllipseWipe ||
         iType == EMiscShapeWipe ||
         iType == EStarWipe ||
         iType == EEyeWipe ||
         iType == EHexagonWipe ||
         iType == ERoundRectWipe ||
         iType == EPentagonWipe )
        {
        TInt i;
        TInt w = aTargetBitmap->SizeInPixels().iWidth;
        TInt h = aTargetBitmap->SizeInPixels().iHeight;
        TRect brect( 0, 0, w, h );
        TPoint tpIn = aTarget;

        iPShape->Reset();

        // hacky: for transOuts, reverse percents and the reverse flag
        TInt percent = iTransOut ? 100 - iPercent :
                                   iPercent;
        TBool reverse = iTransOut? !iTransition->iReverse : 
                                   iTransition->iReverse;

        if ( iScaleMax == 0.0 ) 
            {
            TReal minr = w * w + h * h;

            for ( i = 0; i < iShapeVertices->Count() - 1; i++ ) 
                {
                TRPoint sp = (*iShapeVertices)[i];
                TRPoint ep = (*iShapeVertices)[i + 1];

                for ( TInt j = 0; j < 5; j++ ) 
                    {
                    TReal x = ( sp.iX * ( 4.0 - j ) + ep.iX * j ) / 4.0;
                    TReal y = ( sp.iY * ( 4.0 - j ) + ep.iY * j ) / 4.0;
                    TReal r = x * x + y * y;
                    if ( r < minr )
                        {
                        minr = r;
                        }
                    }
            }

            TRPoint sp = (*iShapeVertices)[iShapeVertices->Count() - 1];
            TRPoint ep = (*iShapeVertices)[0];

            for ( TInt j = 0; j < 5; j++ ) 
                {
                TReal x = ( sp.iX * ( 4.0 - j ) + ep.iX * j ) / 4.0;
                TReal y = ( sp.iY * ( 4.0 - j ) + ep.iY * j ) / 4.0;
                TReal r = x * x + y * y;
                if ( r < minr )
                    {
                    minr = r;
                    }
                }

            Math::Sqrt( iScaleMax, 1.1 * ( w * w + h * h ) / ( 4 * minr ) );
            }


        CFbsBitmap* pMaskBitmap = NULL;
        
        TInt err;
        pMaskBitmap = new CFbsBitmap();
        if ( pMaskBitmap == NULL )
            {
            return;
            }
        
        err = pMaskBitmap->Create( aTargetBitmap->SizeInPixels(), EGray2 );
        if ( err != KErrNone )
            {
            delete pMaskBitmap;
            return;
            }

        CBitmapContext* bgc = static_cast<CBitmapContext*>( &aGc );
        bgc->SetBrushStyle( CGraphicsContext::ENullBrush );
        bgc->SetPenStyle( CGraphicsContext::ENullPen );

        CFbsBitmapDevice* mbdev = NULL;
        CFbsBitGc* mbgc = NULL;
        TRAP( err,   // cannot leave in draw()
              mbdev = CFbsBitmapDevice::NewL( ( CFbsBitmap* ) pMaskBitmap );
              mbgc = CFbsBitGc::NewL(); 
            );
            
        if ( err != KErrNone )
            {
            delete pMaskBitmap;
            delete mbdev;
            delete mbgc;
            return;
            }

        mbgc->Activate( mbdev );
        mbgc->SetBrushStyle( CGraphicsContext::ESolidBrush );
        mbgc->SetPenStyle( CGraphicsContext::ESolidPen );

        // find out how much the shape must be scaled so the incoming image fits in it

        TReal scale = iScaleMax * percent / 100.0;

        CFbsBitmap* pInMap = const_cast<CFbsBitmap*>( aTargetBitmap );

        if ( reverse ) 
            {
            scale = iScaleMax * ( 100 - percent ) / 100.0;
            mbgc->SetBrushColor( TRgb() );
            mbgc->SetPenColor( TRgb() );
            mbgc->DrawRect( brect );
            mbgc->SetBrushColor( TRgb( 0, 0, 0 ) );
            mbgc->SetPenColor( TRgb( 0, 0, 0 ) );
            }
        else 
            {
            mbgc->SetBrushColor( TRgb( 0, 0, 0 ) );
            mbgc->SetPenColor( TRgb( 0, 0, 0 ) );
            mbgc->DrawRect( brect );
            mbgc->SetBrushColor( TRgb() );
            mbgc->SetPenColor( TRgb() );
            }

        if ( scale < 0.0 )
            {
            scale = 0.0;
            }

        for ( i = 0; i < iShapeVertices->Count(); i++ ) 
            {
            TRAP( err, 
                  iPShape->AppendL( TPoint( (TInt)((*iShapeVertices)[i].iX * scale + w / 2 ), 
                                            (TInt)((*iShapeVertices)[i].iY * scale + h / 2 ) ) ) );
            }

        // draw shape polygon on the mask bitmap
        if ( scale > 0.0 )
            {
            err = mbgc->DrawPolygon( iPShape, CGraphicsContext::EWinding );
            }
        
        if ( aMaskBitmap )
            {
            mbgc->SetDrawMode( CGraphicsContext::EDrawModeAND );
            mbgc->BitBlt( TPoint( 0, 0 ), aMaskBitmap );
            }
        
        if ( err == KErrNone )
            {
            bgc->BitBltMasked( tpIn, pInMap, brect, pMaskBitmap, EFalse );
            }
            
        delete mbgc;
        delete mbdev;

        iPShape->Reset();

        delete pMaskBitmap;
        }
    }

// -----------------------------------------------------------------------------
// CIrisWipeTransitionFilter::ResolutionMilliseconds
// -----------------------------------------------------------------------------
//
TInt CIrisWipeTransitionFilter::ResolutionMilliseconds() const
    {
    return ESmilTransitionResolution;
    }

//  End of File  
