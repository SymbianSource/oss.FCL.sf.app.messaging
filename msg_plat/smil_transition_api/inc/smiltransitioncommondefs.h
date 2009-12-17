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
* Description: smiltransitioncommondefs  declaration
*
*/



#ifndef SMILTRANSITIONCOMMONDEFS_H
#define SMILTRANSITIONCOMMONDEFS_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
_LIT( KBarWipe, "barWipe" );
_LIT( KBoxWipe, "boxWipe" );
_LIT( KFourBoxWipe, "fourBoxWipe" );
_LIT( KBarnDoorWipe, "barnDoorWipe" );
_LIT( KDiagonalWipe, "diagonalWipe" );
_LIT( KBowTieWipe, "bowTieWipe" );
_LIT( KMiscDiagonalWipe, "miscDiagonalWipe" );
_LIT( KVeeWipe, "veeWipe" );
_LIT( KBarnVeeWipe, "barnVeeWipe" );
_LIT( KZigZagWipe, "zigZagWipe" );
_LIT( KBarnZigZagWipe, "barnZigZagWipe" );
_LIT( KTriangleWipe, "triangleWipe" );
_LIT( KIrisWipe, "irisWipe" );
_LIT( KArrowHeadWipe, "arrowHeadWipe" );
_LIT( KPentagonWipe, "pentagonWipe" );
_LIT( KHexagonWipe, "hexagonWipe" );
_LIT( KEllipseWipe, "ellipseWipe" );
_LIT( KEyeWipe, "eyeWipe" );
_LIT( KRoundRectWipe, "roundRectWipe" );
_LIT( KStarWipe, "starWipe" );
_LIT( KMiscShapeWipe, "miscShapeWipe" );
_LIT( KClockWipe, "clockWipe" );
_LIT( KPinWheelWipe, "pinWheelWipe" );
_LIT( KSingleSweepWipe, "singleSweepWipe" );
_LIT( KFanWipe, "fanWipe" );
_LIT( KDoubleFanWipe, "doubleFanWipe" );
_LIT( KDoubleSweepWipe, "doubleSweepWipe" );
_LIT( KSaloonDoorWipe, "saloonDoorWipe" );
_LIT( KWindshieldWipe, "windshieldWipe" );
_LIT( KSnakeWipe, "snakeWipe" );
_LIT( KSpiralWipe, "spiralWipe" );
_LIT( KParallelSnakesWipe, "parallelSnakesWipe" );
_LIT( KBoxSnakesWipe, "boxSnakesWipe" );
_LIT( KWaterfallWipe, "waterfallWipe" );
_LIT( KPushWipe, "pushWipe" );
_LIT( KSlideWipe, "slideWipe" );
_LIT( KFade, "fade" );

_LIT( KBottom, "bottom" );
_LIT( KBottomCenter, "bottomCenter" );
_LIT( KBottomLeft, "bottomLeft" );
_LIT( KBottomLeftClockwise, "bottomLeftClockwise" );
_LIT( KBottomLeftCounterClockwise, "bottomLeftCounterClockwise" );
_LIT( KBottomLeftDiagonal, "bottomLeftDiagonal" );
_LIT( KBottomRight, "bottomRight" );
_LIT( KBottomRightClockwise, "bottomRightClockwise" );
_LIT( KBottomRightCounterClockwise, "bottomRightCounterClockwise" );
_LIT( KBottomRightDiagonal, "bottomRightDiagonal" );
_LIT( KCenterRight, "centerRight" );
_LIT( KCenterTop, "centerTop" );
_LIT( KCircle, "circle" );
_LIT( KClockwiseBottom, "clockwiseBottom" );
_LIT( KClockwiseBottomRight, "clockwiseBottomRight" );
_LIT( KClockwiseLeft, "clockwiseLeft" );
_LIT( KClockwiseNine, "clockwiseNine" );
_LIT( KClockwiseRight, "clockwiseRight" );
_LIT( KClockwiseSix, "clockwiseSix" );
_LIT( KClockwiseThree, "clockwiseThree" );
_LIT( KClockwiseTop, "clockwiseTop" );
_LIT( KClockwiseTopLeft, "clockwiseTopLeft" );
_LIT( KClockwiseTwelve, "clockwiseTwelve" );
_LIT( KCornersIn, "cornersIn" );
_LIT( KCornersOut, "cornersOut" );
_LIT( KCounterClockwiseBottomLeft, "counterClockwiseBottomLeft" );
_LIT( KCounterClockwiseTopRight, "counterClockwiseTopRight" );
_LIT( KCrossfade, "crossfade" );
_LIT( KDiagonalBottomLeft, "diagonalBottomLeft" );
_LIT( KDiagonalBottomLeftOpposite, "diagonalBottomLeftOpposite" );
_LIT( KDiagonalTopLeft, "diagonalTopLeft" );
_LIT( KDiagonalTopLeftOpposite, "diagonalTopLeftOpposite" );
_LIT( KDiamond, "diamond" );
_LIT( KDoubleBarnDoor, "doubleBarnDoor" );
_LIT( KDoubleDiamond, "doubleDiamond" );
_LIT( KDown, "down" );
_LIT( KFadeFromColor, "fadeFromColor" );
_LIT( KFadeToColor, "fadeToColor" );
_LIT( KFanInHorizontal, "fanInHorizontal" );
_LIT( KFanInVertical, "fanInVertical" );
_LIT( KFanOutHorizontal, "fanOutHorizontal" );
_LIT( KFanOutVertical, "fanOutVertical" );
_LIT( KFivePoint, "fivePoint" );
_LIT( KFourBlade, "fourBlade" );
_LIT( KFourBoxHorizontal, "fourBoxHorizontal" );
_LIT( KFourBoxVertical, "fourBoxVertical" );
_LIT( KFourPoint, "fourPoint" );
_LIT( KFromBottom, "fromBottom" );
_LIT( KFromLeft, "fromLeft" );
_LIT( KFromRight, "fromRight" );
_LIT( KFromTop, "fromTop" );
_LIT( KHeart, "heart" );
_LIT( KHorizontal, "horizontal" );
_LIT( KHorizontalLeft, "horizontalLeft" );
_LIT( KHorizontalLeftSame, "horizontalLeftSame" );
_LIT( KHorizontalRight, "horizontalRight" );
_LIT( KHorizontalRightSame, "horizontalRightSame" );
_LIT( KHorizontalTopLeftOpposite, "horizontalTopLeftOpposite" );
_LIT( KHorizontalTopRightOpposite, "horizontalTopRightOpposite" );
_LIT( KKeyhole, "keyhole" );
_LIT( KLeft, "left" );
_LIT( KLeftCenter, "leftCenter" );
_LIT( KLeftToRight, "leftToRight" );
_LIT( KOppositeHorizontal, "oppositeHorizontal" );
_LIT( KOppositeVertical, "oppositeVertical" );
_LIT( KParallelDiagonal, "parallelDiagonal" );
_LIT( KParallelDiagonalBottomLeft, "parallelDiagonalBottomLeft" );
_LIT( KParallelDiagonalTopLeft, "parallelDiagonalTopLeft" );
_LIT( KParallelVertical, "parallelVertical" );
_LIT( KRectangle, "rectangle" );
_LIT( KRight, "right" );
_LIT( KRightCenter, "rightCenter" );
_LIT( KSixPoint, "sixPoint" );
_LIT( KTop, "top" );
_LIT( KTopCenter, "topCenter" );
_LIT( KTopLeft, "topLeft" );
_LIT( KTopLeftClockwise, "topLeftClockwise" );
_LIT( KTopLeftCounterClockwise, "topLeftCounterClockwise" );
_LIT( KTopLeftDiagonal, "topLeftDiagonal" );
_LIT( KTopLeftHorizontal, "topLeftHorizontal" );
_LIT( KTopLeftVertical, "topLeftVertical" );
_LIT( KTopRight, "topRight" );
_LIT( KTopRightClockwise, "topRightClockwise" );
_LIT( KTopRightCounterClockwise, "topRightCounterClockwise" );
_LIT( KTopRightDiagonal, "topRightDiagonal" );
_LIT( KTopToBottom, "topToBottom" );
_LIT( KTwoBladeHorizontal, "twoBladeHorizontal" );
_LIT( KTwoBladeVertical, "twoBladeVertical" );
_LIT( KTwoBoxBottom, "twoBoxBottom" );
_LIT( KTwoBoxLeft, "twoBoxLeft" );
_LIT( KTwoBoxRight, "twoBoxRight" );
_LIT( KTwoBoxTop, "twoBoxTop" );
_LIT( KUp, "up" );
_LIT( KVertical, "vertical" );
_LIT( KVerticalBottomLeftOpposite, "verticalBottomLeftOpposite" );
_LIT( KVerticalBottomSame, "verticalBottomSame" );
_LIT( KVerticalLeft, "verticalLeft" );
_LIT( KVerticalRight, "verticalRight" );
_LIT( KVerticalTopLeftOpposite, "verticalTopLeftOpposite" );
_LIT( KVerticalTopSame, "verticalTopSame" );

// MACROS
#define ESmilTransitionResolution 100

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Transition definitions.
*
*  @lib smiltran.lib
*  @since S60 2.1
*/
class TTransitionDefs
    {
    public: // Enumerations.
    
        enum TTransitionType // Transitions types.
            {
            EBarWipe,
            EBoxWipe,
            EFourBoxWipe,
            EBarnDoorWipe,
            EDiagonalWipe,
            EBowTieWipe,
            EMiscDiagonalWipe,
            EVeeWipe,
            EBarnVeeWipe,
            EZigZagWipe,
            EBarnZigZagWipe,
            ETriangleWipe,
            EIrisWipe,
            EArrowHeadWipe,
            EPentagonWipe,
            EHexagonWipe,
            EEllipseWipe,
            EEyeWipe,
            ERoundRectWipe,
            EStarWipe,
            EMiscShapeWipe,
            EClockWipe,
            EPinWheelWipe,
            ESingleSweepWipe,
            EFanWipe,
            EDoubleFanWipe,
            EDoubleSweepWipe,
            ESaloonDoorWipe,
            EWindshieldWipe,
            ESnakeWipe,
            ESpiralWipe,
            EParallelSnakesWipe,
            EBoxSnakesWipe,
            EWaterfallWipe,
            EPushWipe,
            ESlideWipe,
            EFade
            };

        //
        // Subtypes
        enum TTransitionSubtype
            {
            EBottom,
            EBottomCenter,
            EBottomLeft,
            EBottomLeftClockwise,
            EBottomLeftCounterClockwise,
            EBottomLeftDiagonal,
            EBottomRight,
            EBottomRightClockwise,
            EBottomRightCounterClockwise,
            EBottomRightDiagonal,
            ECenterRight,
            ECenterTop,
            ECircle,
            EClockwiseBottom,
            EClockwiseBottomRight,
            EClockwiseLeft,
            EClockwiseNine,
            EClockwiseRight,
            EClockwiseSix,
            EClockwiseThree,
            EClockwiseTop,
            EClockwiseTopLeft,
            EClockwiseTwelve,
            ECornersIn,
            ECornersOut,
            ECounterClockwiseBottomLeft,
            ECounterClockwiseTopRight,
            ECrossfade,
            EDiagonalBottomLeft,
            EDiagonalBottomLeftOpposite,
            EDiagonalTopLeft,
            EDiagonalTopLeftOpposite,
            EDiamond,
            EDoubleBarnDoor,
            EDoubleDiamond,
            EDown,
            EFadeFromColor,
            EFadeToColor,
            EFanInHorizontal,
            EFanInVertical,
            EFanOutHorizontal,
            EFanOutVertical,
            EFivePoint,
            EFourBlade,
            EFourBoxHorizontal,
            EFourBoxVertical,
            EFourPoint,
            EFromBottom,
            EFromLeft,
            EFromRight,
            EFromTop,
            EHeart,
            EHorizontal,
            EHorizontalLeft,
            EHorizontalLeftSame,
            EHorizontalRight,
            EHorizontalRightSame,
            EHorizontalTopLeftOpposite,
            EHorizontalTopRightOpposite,
            EKeyhole,
            ELeft,
            ELeftCenter,
            ELeftToRight,
            EOppositeHorizontal,
            EOppositeVertical,
            EParallelDiagonal,
            EParallelDiagonalBottomLeft,
            EParallelDiagonalTopLeft,
            EParallelVertical,
            ERectangle,
            ERight,
            ERightCenter,
            ESixPoint,
            ETop,
            ETopCenter,
            ETopLeft,
            ETopLeftClockwise,
            ETopLeftCounterClockwise, 
            ETopLeftDiagonal,
            ETopLeftHorizontal,
            ETopLeftVertical,
            ETopRight,
            ETopRightClockwise,
            ETopRightCounterClockwise,
            ETopRightDiagonal,
            ETopToBottom,
            ETwoBladeHorizontal,
            ETwoBladeVertical,
            ETwoBoxBottom,
            ETwoBoxLeft,
            ETwoBoxRight,
            ETwoBoxTop,
            EUp,
            EVertical,
            EVerticalBottomLeftOpposite,
            EVerticalBottomSame,
            EVerticalLeft,
            EVerticalRight,
            EVerticalTopLeftOpposite,
            EVerticalTopSame,

            // some subtype reverse enumeration
            EBottomToTop,
            ERightToLeft
            };
    };

#endif // SMILTRANSITIONCOMMONDEFS_H

