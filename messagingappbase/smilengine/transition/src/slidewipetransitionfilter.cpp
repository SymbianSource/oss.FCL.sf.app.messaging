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
* Description: slidewipetransitionfilter implementation
*
*/



//  INCLUDES
#include "slidewipetransitionfilter.h"

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
// CSlideWipeTransitionFilter::CSlideWipeTransitionFilter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSlideWipeTransitionFilter::CSlideWipeTransitionFilter() :
    iType( ESlideWipe ),
    iSubtype( EFromLeft )
    {
    }

// -----------------------------------------------------------------------------
// CSlideWipeTransitionFilter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSlideWipeTransitionFilter::ConstructL( CSmilTransition* aTransition, 
                                             CFbsBitmap* /*aBuffer*/, 
                                             MSmilMedia* aMedia )
    {
    CSmilTransitionFilter::ConstructL( aMedia );

    iTransition = aTransition;
    
    if ( aTransition->Type() == KSlideWipe )
        {
        iType = ESlideWipe;
        if( aTransition->iReverse ) 
            {
            if ( aTransition->Subtype() == KFromRight )
                {
                iSubtype = EFromLeft;
                }
            else if ( aTransition->Subtype() == KFromBottom )
                {
                iSubtype = EFromTop;
                }
            else if ( aTransition->Subtype() == KFromTop )
                {
                iSubtype = EFromBottom;
                }
            else
                {
                iSubtype = EFromRight;
                }
            }
        else 
            {
            if ( aTransition->Subtype() == KFromRight )
                {
                iSubtype = EFromRight;
                }
            else if ( aTransition->Subtype() == KFromBottom )
                {
                iSubtype = EFromBottom;
                }
            else if ( aTransition->Subtype() == KFromTop )
                {
                iSubtype = EFromTop;
                }
            else
                {
                iSubtype = EFromLeft;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CSlideWipeTransitionFilter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSlideWipeTransitionFilter* CSlideWipeTransitionFilter::NewL( CSmilTransition* aTransition, 
                                                                       CFbsBitmap* aBuffer, 
                                                                       MSmilMedia* aMedia )
    {
    CSlideWipeTransitionFilter* self = new( ELeave ) CSlideWipeTransitionFilter();
    
    CleanupStack::PushL( self );
    self->ConstructL( aTransition, aBuffer, aMedia );
    CleanupStack::Pop( self );
    
    return self;        
    }

// -----------------------------------------------------------------------------
// CSlideWipeTransitionFilter::~CSlideWipeTransitionFilter
// Destructor.
// -----------------------------------------------------------------------------
//
CSlideWipeTransitionFilter::~CSlideWipeTransitionFilter()
    {
    }

// -----------------------------------------------------------------------------
// CSlideWipeTransitionFilter::Draw
// Draw method, calculate move and draw bitmaps accordingly.
// -----------------------------------------------------------------------------
//
void CSlideWipeTransitionFilter::Draw( CGraphicsContext& aGc, 
                                       const TRect& /*aRect*/, 
                                       const CFbsBitmap* aTargetBitmap, 
                                       const TPoint& aTarget, 
                                       const CFbsBitmap* aMaskBitmap )
    {
    if ( aTargetBitmap )
        {
        if ( iType == ESlideWipe )
            {
            TRect region = Media()->GetRectangle();

            TInt w = region.Width();
            TInt h = region.Height();

            TRect brect( 0, 0, w, h );                

            TInt wdiff = 0;
            TInt hdiff = 0;

            TInt tlY = region.iTl.iY;
            TInt brX = region.iBr.iX;
            TInt brY = region.iBr.iY;
    
            TPoint tp = aTarget; 

            if ( iTransOut )
                {
                switch ( iSubtype )
                    {
                    case EFromTop:
                        {
                        hdiff = h * iPercent / 100;    
                        region.iTl = TPoint( 0,0 );
                        region.iBr = TPoint( brX, brY - hdiff );        
                        tp = TPoint( aTarget.iX, tlY + hdiff ); 
                        break;                        
                        }
                    case EFromBottom:
                        {
                        hdiff = h * iPercent / 100;
                        region.iTl = TPoint( 0, hdiff );    
                        region.iBr = TPoint( brX, brY );
                        break;        
                        }
                    case EFromRight:
                        {
                        wdiff = w * iPercent / 100;
                        region.iTl = TPoint( wdiff, 0 );    
                        region.iBr = TPoint( brX, brY );
                        break;
                        }
                    case EFromLeft:
                    default:
                        {
                        wdiff = w * iPercent / 100;
                        region.iTl = TPoint( 0, 0 );        
                        region.iBr = TPoint( w - wdiff, brY );            
                        tp = TPoint( aTarget.iX + wdiff, aTarget.iY ); 
                        break;
                        }
                    }    
                }
            else  // iTransIn
                {
                switch (iSubtype)
                    {
                    case EFromTop:
                        {
                        region.iTl = TPoint( 0, h - h * iPercent / 100 );
                        break;
                        }
                    case EFromBottom:
                        {
                        hdiff = h * iPercent / 100;    
                        region.iTl = TPoint( 0, 0 );    
                        region.iBr = TPoint( brX, hdiff );
                        tp = TPoint( aTarget.iX, brY - hdiff ); 
                        break;
                        }
                    case EFromRight:
                        {
                        wdiff = w * iPercent / 100;
                        region.iTl = TPoint( 0, 0 );    
                        region.iBr = TPoint( wdiff, brY );        
                        tp = TPoint( brX - wdiff, aTarget.iY );
                        break;
                        }
                    case EFromLeft:
                    default:
                        {
                        region.iTl = TPoint( w - w * iPercent / 100, 0 );        
                        }
                    }

                region.Intersection( brect );
                }

            CBitmapContext* bgc = static_cast<CBitmapContext*>( &aGc );

            bgc->SetBrushStyle( CGraphicsContext::ENullBrush );
            bgc->SetPenStyle( CGraphicsContext::ENullPen );
            if ( aMaskBitmap )
                {
                bgc->BitBltMasked( tp, aTargetBitmap, region, aMaskBitmap, EFalse );
                }
            else
                {
                bgc->BitBlt( tp, aTargetBitmap, region );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CSlideWipeTransitionFilter::ResolutionMilliseconds
// -----------------------------------------------------------------------------
//
TInt CSlideWipeTransitionFilter::ResolutionMilliseconds() const
    {
    return ESmilTransitionResolution;
    }

//  End of File  
