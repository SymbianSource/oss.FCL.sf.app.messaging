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
* Description: barwipetransitionfilter implementation
*
*/



// INCLUDE FILES
#include "barwipetransitionfilter.h"

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
// CBarWipeTransitionFilter::CBarWipeTransitionFilter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CBarWipeTransitionFilter::CBarWipeTransitionFilter() :
    iType( EBarWipe ),
    iSubtype( ELeftToRight )
    {
    }

// -----------------------------------------------------------------------------
// CBarWipeTransitionFilter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBarWipeTransitionFilter::ConstructL( CSmilTransition* aTransition, 
                                           CFbsBitmap* /*aBuffer*/, 
                                           MSmilMedia* aMedia )
    {
	CSmilTransitionFilter::ConstructL( aMedia );

	iTransition = aTransition;

	if ( aTransition->Type() == KBarWipe )
	    {
		if ( aTransition->Subtype() == KTopToBottom )
		    {
		    iSubtype = ETopToBottom;
		    }
	    }
    }

// -----------------------------------------------------------------------------
// CBarWipeTransitionFilter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBarWipeTransitionFilter* CBarWipeTransitionFilter::NewL( CSmilTransition* aTransition,
    						                              CFbsBitmap* aBuffer, 
    						                              MSmilMedia* aMedia )
    {
	CBarWipeTransitionFilter* self = new(ELeave) CBarWipeTransitionFilter();
	
	CleanupStack::PushL( self );
	self->ConstructL( aTransition, aBuffer, aMedia );
	CleanupStack::Pop( self );
	
	return self;	
    }

// -----------------------------------------------------------------------------
// CBarWipeTransitionFilter::~CBarWipeTransitionFilter
// Destructor.
// -----------------------------------------------------------------------------
//
CBarWipeTransitionFilter::~CBarWipeTransitionFilter()
    {
    }  

// -----------------------------------------------------------------------------
// CBarWipeTransitionFilter::Draw
// Draw method, calculates move and draws bitmaps accordingly.
// -----------------------------------------------------------------------------
//
void CBarWipeTransitionFilter::Draw( CGraphicsContext& aGc, 
				                     const TRect& /*aRect*/,
				                     const CFbsBitmap* aTargetBitmap, 
				                     const TPoint& , 
				                     const CFbsBitmap* aMaskBitmap )
    {
	if (aTargetBitmap)
	    {
		if (iType == EBarWipe)
		    {
			// hacky: for transOuts, reverse percents, direction
			TInt percent = iTransOut ? 100 - iPercent : 
			                           iPercent;	
			TInt stype = iSubtype;
			if ( iTransOut ? !iTransition->iReverse : 
			                 iTransition->iReverse ) 
                {
				if ( iSubtype == ETopToBottom )
				    {
				    stype = EBottomToTop;
				    }
				else
				    {
				    stype = ERightToLeft;
				    }
			    }
			

			TInt bitmapW = aTargetBitmap->SizeInPixels().iWidth;
			TInt bitmapH = aTargetBitmap->SizeInPixels().iHeight;

			TRect region = Media()->GetRectangle();			

			TPoint targetTl = region.iTl;
			TRect copyRect;
			TInt move;

			switch ( stype )
			    {
			    case ETopToBottom:
			        {
			        move = bitmapH * percent / 100;
    				copyRect = TRect( TPoint( 0, 0 ), TSize( bitmapW, move ) );
	    			break;
			        }
			    case EBottomToTop:
			        {
			        move = bitmapH * ( 100 - percent ) / 100;
    				targetTl.iY = targetTl.iY + move;
	    			copyRect = TRect( TPoint( 0, move ), TSize( bitmapW, bitmapH ) );
		    		break;
			        }
			    case ERightToLeft:
			        {
			        move = bitmapW * ( 100 - percent ) / 100;
				    targetTl.iX = targetTl.iX + move;
				    copyRect = TRect( TPoint( move, 0 ), TSize( bitmapW, bitmapH ) );
				    break;
			        }
			    case ELeftToRight:
			    default:	
			        {
			        move = bitmapW * percent / 100;
				    copyRect = TRect( TPoint( 0, 0 ), TSize( move, bitmapH ) );
				    break;
			        }
			    }

			TRect regionSizeRect( TPoint( 0, 0 ), region.Size() );
			copyRect.Intersection( regionSizeRect );

			CBitmapContext* bgc = static_cast<CBitmapContext*>( &aGc );
			bgc->SetBrushStyle( CGraphicsContext::ENullBrush );
			bgc->SetPenStyle( CGraphicsContext::ENullPen );
	
			if ( aMaskBitmap )
			    {
			    bgc->BitBltMasked( targetTl, aTargetBitmap, copyRect, aMaskBitmap, EFalse );
			    }
			else
			    {
			    bgc->BitBlt( targetTl, aTargetBitmap, copyRect );
			    }
		    }
	    }
    }


// -----------------------------------------------------------------------------
// CBarWipeTransitionFilter::ResolutionMilliseconds
// -----------------------------------------------------------------------------
//
TInt CBarWipeTransitionFilter::ResolutionMilliseconds() const
    {
	return ESmilTransitionResolution;
    }

//  End of File  
