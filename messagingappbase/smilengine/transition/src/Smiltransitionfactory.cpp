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
* Description: Smiltransitionfactory implementation
*
*/



//  INCLUDES
#include "smiltransitionfactory.h"

#include <gdi.h>
#include <fbs.h>
#include <w32std.h>

#include "slidewipetransitionfilter.h"
#include "pushwipetransitionfilter.h"
#include "fadetransitionfilter.h"
#include "barwipetransitionfilter.h"
#include "clockwipetransitionfilter.h"
#include "iriswipetransitionfilter.h"
#include "matrixwipetransitionfilter.h"

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
// CSmilTransitionFactory::CSmilTransitionFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSmilTransitionFactory::CSmilTransitionFactory( CFbsBitmap* aBuffer )
    {
    iBuf = aBuffer;
    }

// -----------------------------------------------------------------------------
// CSmilTransitionFactory::~CSmilTransitionFactory
// Destructor.
// -----------------------------------------------------------------------------
//
CSmilTransitionFactory::~CSmilTransitionFactory()
    {
    }

// -----------------------------------------------------------------------------
// CSmilTransitionFactory::CreateFilterL
// Create correct filter, can leave.
// -----------------------------------------------------------------------------
//
CSmilTransitionFilter* CSmilTransitionFactory::CreateFilterL( CSmilTransition* aTransition, 
                                                                       MSmilMedia* aMedia)
    {
    if ( aTransition && iBuf )
        {
        if ( aTransition->Type() == KFade )
            {
            // create a FadeTransitionFilter
            return CFadeTransitionFilter::NewL( aTransition, iBuf, aMedia );
            }
        else if ( aTransition->Type() == KSlideWipe )
            {
            // create a SlideWipeTransitionFilter
            return CSlideWipeTransitionFilter::NewL( aTransition, iBuf, aMedia );
            }
        else if ( aTransition->Type() == KPushWipe )
            {
            // create a PushWipeTransitionFilter
            return  CPushWipeTransitionFilter::NewL( aTransition, iBuf, aMedia );
            }
        else if ( aTransition->Type() == KBarWipe )
            {    
            // create BarWipeTransitionFilter
            return CBarWipeTransitionFilter::NewL( aTransition, iBuf, aMedia );
            }
        else if ( aTransition->Type() == KClockWipe )
            {    
            // create ClockWipeTransitionFilter
            return CClockWipeTransitionFilter::NewL( aTransition, iBuf, aMedia );
            }
        else if ( aTransition->Type()==KIrisWipe ||
                  aTransition->Type()==KTriangleWipe ||
                  aTransition->Type()==KArrowHeadWipe ||
                  aTransition->Type()==KEllipseWipe ||
                  aTransition->Type()==KMiscShapeWipe ||
                  aTransition->Type()==KStarWipe ||
                  aTransition->Type()==KEyeWipe ||
                  aTransition->Type()==KHexagonWipe ||
                  aTransition->Type()==KRoundRectWipe ||
                  aTransition->Type()==KPentagonWipe )
            {
            return CIrisWipeTransitionFilter::NewL( aTransition, iBuf, aMedia );
            }
        else if ( aTransition->Type() == KSnakeWipe ||
                  aTransition->Type() == KSpiralWipe) 
            {
            return CMatrixWipeTransitionFilter::NewL( aTransition, iBuf, aMedia );
            }
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CSmilTransitionFactory::IsSupportedType
// Check if transition is supported, returns ETrue if so.
// -----------------------------------------------------------------------------
//
TBool CSmilTransitionFactory::IsSupportedType( const TDesC& aTransitionName ) const
    {
    if ( aTransitionName == KFade || 
         aTransitionName == KSlideWipe || 
         aTransitionName == KPushWipe ||
         aTransitionName == KBarWipe ||
         aTransitionName == KIrisWipe ||
         aTransitionName == KTriangleWipe ||
         aTransitionName == KArrowHeadWipe ||
         aTransitionName == KPentagonWipe ||
         aTransitionName == KEllipseWipe ||
         aTransitionName == KMiscShapeWipe ||
         aTransitionName == KStarWipe ||
         aTransitionName == KEyeWipe ||
         aTransitionName == KRoundRectWipe ||
         aTransitionName == KHexagonWipe ||
         aTransitionName == KSnakeWipe ||
         aTransitionName == KSpiralWipe ||
         aTransitionName == KClockWipe ) 
           {
           return ETrue;
           }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSmilTransitionFactory::IsSupportedType
// Check if transition subtype is supported, returns ETrue if so.
// -----------------------------------------------------------------------------
//
TBool CSmilTransitionFactory::IsSupportedSubtype( const TDesC& aTransitionName, 
                                                           const TDesC& aSubtypeName ) const
    {
    if ( aTransitionName == KFade )
        {
        if ( aSubtypeName == KFadeToColor || 
             aSubtypeName == KFadeFromColor || 
             aSubtypeName == KCrossfade )
            {
            return ETrue;
            }
        }
    else if ( aTransitionName == KSlideWipe )
        {
        if ( aSubtypeName == KFromRight || 
             aSubtypeName == KFromLeft ||
             aSubtypeName == KFromTop || 
             aSubtypeName == KFromBottom )
            {
            return ETrue;
            }
        }
    else if ( aTransitionName == KPushWipe ) 
        {
        if ( aSubtypeName == KFromRight || 
             aSubtypeName == KFromLeft ||
             aSubtypeName == KFromTop || 
             aSubtypeName==KFromBottom )
            {
            return ETrue;
            }
        }
    else if ( aTransitionName == KBarWipe )
        {
        if( aSubtypeName == KTopToBottom || 
            aSubtypeName == KLeftToRight )
            {
            return ETrue;
            }
        }
    else if ( aTransitionName == KIrisWipe )
        {
        if ( aSubtypeName == KDiamond || 
             aSubtypeName == KRectangle )
            {
            return ETrue;
            }
        }
     else if ( aTransitionName == KTriangleWipe )
        {
        if ( aSubtypeName == KRight || 
             aSubtypeName == KDown ||
             aSubtypeName == KLeft || 
             aSubtypeName == KUp )
            {
            return ETrue;
            }
        }
    else if ( aTransitionName == KArrowHeadWipe )
        {
        if ( aSubtypeName == KRight || 
             aSubtypeName == KDown ||
             aSubtypeName == KLeft || 
             aSubtypeName == KUp )
            {
            return ETrue;
            }
        }
    else if ( aTransitionName == KPentagonWipe )
        {
        if ( aSubtypeName == KUp || 
             aSubtypeName == KDown )
            {
            return ETrue;
            }
        }
    else if ( aTransitionName == KHexagonWipe )
        {    
        if ( aSubtypeName == KVertical || 
             aSubtypeName == KHorizontal )
            {
            return ETrue;
            }
        }
    else if ( aTransitionName == KEllipseWipe )
        {
        if ( aSubtypeName == KVertical || 
             aSubtypeName == KHorizontal || 
             aSubtypeName == KCircle )
            {
            return ETrue;
            }
        }
    else if ( aTransitionName == KEyeWipe )
        {
        if ( aSubtypeName == KVertical || 
             aSubtypeName == KHorizontal )
            {
            return ETrue;
            }
        }
    else if ( aTransitionName == KRoundRectWipe )
        {
        if ( aSubtypeName == KVertical || 
             aSubtypeName == KHorizontal )
            {
            return ETrue;
            }
        }
    else if ( aTransitionName == KStarWipe )
        {
        if ( aSubtypeName == KFivePoint || 
             aSubtypeName == KSixPoint || 
             aSubtypeName == KFourPoint )
            {
            return ETrue;
            }
        }
    else if ( aTransitionName == KMiscShapeWipe )
        {
        if ( aSubtypeName == KKeyhole || 
             aSubtypeName == KHeart )
            {
            return ETrue;
            }
        }
    else if ( aTransitionName == KSpiralWipe ) 
        {
        if ( aSubtypeName == KTopRightClockwise || 
             aSubtypeName == KBottomRightClockwise ||
             aSubtypeName == KBottomLeftClockwise || 
             aSubtypeName == KTopLeftCounterClockwise ||
             aSubtypeName == KTopRightCounterClockwise || 
             aSubtypeName == KBottomLeftCounterClockwise ||
             aSubtypeName == KBottomRightCounterClockwise || 
             aSubtypeName == KTopLeftClockwise )
            {
            return ETrue;
            }
        }
    else if ( aTransitionName == KSnakeWipe )
        {    
        if ( aSubtypeName == KTopLeftVertical  || 
             aSubtypeName == KTopLeftDiagonal ||
             aSubtypeName == KTopRightDiagonal || 
             aSubtypeName == KBottomLeftDiagonal ||
             aSubtypeName ==KBottomRightDiagonal || 
             aSubtypeName == KTopLeftHorizontal )
            {
            return ETrue;
            }
        }
    else if ( aTransitionName == KClockWipe )
        {
        if ( aSubtypeName == KClockwiseThree || 
             aSubtypeName == KClockwiseSix || 
             aSubtypeName == KClockwiseNine )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// End of file
