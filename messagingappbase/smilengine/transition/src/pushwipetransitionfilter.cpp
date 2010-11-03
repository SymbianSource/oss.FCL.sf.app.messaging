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
* Description: pushwipetransitionfilter implementation
*
*/



//  INCLUDES
#include "pushwipetransitionfilter.h"

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
// CPushWipeTransitionFilter::CPushWipeTransitionFilter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPushWipeTransitionFilter::CPushWipeTransitionFilter() :
    iType( EPushWipe ),
    iSubtype( EFromLeft )
    {
    }

// -----------------------------------------------------------------------------
// CPushWipeTransitionFilter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPushWipeTransitionFilter::ConstructL( CSmilTransition* aTransition, 
                                            CFbsBitmap* aBuffer, 
                                            MSmilMedia* aMedia )
    {
    CSmilTransitionFilter::ConstructL( aMedia );

    iTransition = aTransition;
    
    if ( aTransition->Type() == KPushWipe )
        {
        iType = EPushWipe;
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

    // Need buffered bitmap but there's no guarantie that the bitmap provided at construct time
    // is the right one. Should get it at draw time - but for now...
    iBackBuffer = aBuffer;
    }

// -----------------------------------------------------------------------------
// CPushWipeTransitionFilter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPushWipeTransitionFilter* CPushWipeTransitionFilter::NewL( CSmilTransition* aTransition, 
                                                            CFbsBitmap* aBuffer, 
                                                            MSmilMedia* aMedia )
    {
    CPushWipeTransitionFilter* self =new(ELeave) CPushWipeTransitionFilter();
    
    CleanupStack::PushL( self );
    self->ConstructL( aTransition, aBuffer, aMedia );
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// CPushWipeTransitionFilter::~CPushWipeTransitionFilter
// Destructor.
// -----------------------------------------------------------------------------
//
CPushWipeTransitionFilter::~CPushWipeTransitionFilter()
    {
    delete iBufferBitmap;
    delete iBitmapContext;
    delete iBufferDevice;
    }

// -----------------------------------------------------------------------------
// CPushWipeTransitionFilter::Draw
// Draw method, calculate move and draw bitmaps accordingly.
// -----------------------------------------------------------------------------
//
void CPushWipeTransitionFilter::Draw( CGraphicsContext& aGc, 
                                      const TRect& /*aRect*/,
                                      const CFbsBitmap* aTargetBitmap, 
                                      const TPoint& /*aTarget*/, 
                                      const CFbsBitmap* aMaskBitmap )
    {
    if ( aTargetBitmap )
        {
        if ( !iBitmapContext )
            {
            return;
            }

        if ( iType==EPushWipe )
            {
            // ### inefficient!            
            iBitmapContext->BitBlt( TPoint(0,0), iBackBuffer, Media()->GetRectangle() );

            // hacky: for transOuts, reverse percents, direction
            TInt percent = iTransOut? 100-iPercent: 
                                      iPercent;    
            TInt stype = iSubtype;
            if ( iTransOut ? !iTransition->iReverse :
                             iTransition->iReverse ) 
                {
                switch ( iSubtype )
                    {
                    case EFromRight:
                        {
                        stype = EFromLeft;
                        break;
                        }
                    case EFromLeft:
                        {
                        stype = EFromRight;
                        break;
                        }
                    case EFromBottom:
                        {
                        stype = EFromTop;
                        break;
                        }
                    case EFromTop:
                    default:
                        {
                        stype = EFromBottom;
                        break;
                        }
                    }
                }            

            TInt bitmapW = aTargetBitmap->SizeInPixels().iWidth;
            TInt bitmapH = aTargetBitmap->SizeInPixels().iHeight;

            TRect region = Media()->GetRectangle();            

            TPoint targetTl1 = region.iTl;
            TPoint targetTl2 = region.iTl;
            TRect copyRect1;
            TRect copyRect2;
            TInt move;

            switch ( stype )
                {
                case EFromTop:
                    {
                    move = bitmapH * percent / 100;
                    copyRect1 = TRect( TPoint( 0, bitmapH - move ), TSize( bitmapW, move ) );
                    copyRect2 = TRect( TPoint( 0, 0 ), TSize( bitmapW, bitmapH - move ) );
                    targetTl2.iY = targetTl2.iY + move;
                    break;
                    }
                case EFromBottom:
                    {
                    move = bitmapH * percent / 100;
                    copyRect1 = TRect( TPoint( 0, 0 ), TSize( bitmapW, move ) );
                    targetTl1.iY = targetTl2.iY + bitmapH - move;
                    copyRect2 = TRect( TPoint( 0, move ), TSize( bitmapW, bitmapH - move ) );                
                    break;
                    }
                case EFromRight:
                    {
                    move = bitmapW * percent / 100;
                    copyRect1 = TRect( TPoint( 0, 0 ), TSize( move, bitmapH ) );
                    targetTl1.iX = targetTl2.iX + bitmapW - move;
                    copyRect2 = TRect( TPoint( move, 0 ), TSize( bitmapW - move, bitmapH ) );                
                    break;
                    }
                case EFromLeft:
                default:    
                    {
                    move = bitmapW * percent / 100;
                    copyRect1 = TRect( TPoint( bitmapW - move, 0 ), TSize( move, bitmapH ) );
                    copyRect2 = TRect( TPoint( 0, 0 ), TSize( bitmapW - move, bitmapH ) );
                    targetTl2.iX = targetTl2.iX + move;
                    break;
                    }
                }

            TRect regionSizeRect( TPoint( 0, 0 ), region.Size() );
            copyRect1.Intersection( regionSizeRect );
            copyRect2.Intersection( regionSizeRect );

            CBitmapContext* bgc = static_cast<CBitmapContext*>( &aGc );
            bgc->SetBrushStyle( CGraphicsContext::ENullBrush );
            bgc->SetPenStyle( CGraphicsContext::ENullPen );

            if ( aMaskBitmap )
                {
                bgc->BitBltMasked( targetTl1, aTargetBitmap, copyRect1, aMaskBitmap, EFalse );
                bgc->BitBltMasked( targetTl2, iBufferBitmap, copyRect2, aMaskBitmap, EFalse );
                }
            else
                {
                bgc->BitBlt( targetTl1, aTargetBitmap, copyRect1 );
                bgc->BitBlt( targetTl2, iBufferBitmap, copyRect2 );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CPushWipeTransitionFilter::ResolutionMilliseconds
// -----------------------------------------------------------------------------
//
TInt CPushWipeTransitionFilter::ResolutionMilliseconds() const
    {
    return ESmilTransitionResolution;
    }

// -----------------------------------------------------------------------------
// CPushWipeTransitionFilter::TransitionStartingL
// Called when transition is to start, can allocate & leave.
// -----------------------------------------------------------------------------
//
void CPushWipeTransitionFilter::TransitionStartingL()
    {
    if ( iBackBuffer && !iBufferBitmap )
        {
        iBufferBitmap = new(ELeave) CFbsBitmap;
        User::LeaveIfError( iBufferBitmap->Create( Media()->GetRectangle().Size(),
                                                   iBackBuffer->DisplayMode() ) );

        iBufferDevice = CFbsBitmapDevice::NewL( iBufferBitmap );
        iBitmapContext = NULL;
        User::LeaveIfError( iBufferDevice->CreateContext( iBitmapContext ) );
        }
    }

// -----------------------------------------------------------------------------
// CPushWipeTransitionFilter::TransitionEnding
// Called when transition is to end, clean up.
// -----------------------------------------------------------------------------
//
void CPushWipeTransitionFilter::TransitionEnding()
    {    
    delete iBufferBitmap;
    iBufferBitmap = NULL;
    
    delete iBitmapContext;
    iBitmapContext = NULL;
    
    delete iBufferDevice;
    iBufferDevice = NULL;
    }    

//  End of File  
