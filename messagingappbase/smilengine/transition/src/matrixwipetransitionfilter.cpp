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
* Description: matrixwipetransitionfilter implementation
*
*/



//  INCLUDES
#include "matrixwipetransitionfilter.h"

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
// CMatrixWipeTransitionFilter::CMatrixWipeTransitionFilter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMatrixWipeTransitionFilter::CMatrixWipeTransitionFilter() :
    iType( ESnakeWipe ),
    iSubtype( ETopLeftHorizontal )
    {
    }

// -----------------------------------------------------------------------------
// CMatrixWipeTransitionFilter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMatrixWipeTransitionFilter::ConstructL( CSmilTransition* aTransition,
                                              CFbsBitmap* aBuffer, 
                                              MSmilMedia* aMedia )
    {
    CSmilTransitionFilter::ConstructL( aMedia );

    iTransition = aTransition;

    iMatrixSnake = new( ELeave ) RArray<CMatrixSnake*>( 2 );
    iShapeVertices = new( ELeave ) RArray<TRPoint>( 2 );
    
    iIsRect = ETrue; // rectangle or a diamond

    if ( aTransition->Type() == KSpiralWipe )
        {
        iType = ESpiralWipe;
    
        if ( aTransition->Subtype() == KTopRightClockwise ) 
            {
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 0.0/7.0, 6.0/7.0, 6.0/7.0, 1, 7 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 6.0/7.0, 0.0/7.0, 6.0/7.0, 8, 13 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 5.0/7.0, 0.0/7.0, 0.0/7.0, 14, 19 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 0.0/7.0, 5.0/7.0, 0.0/7.0, 20, 24 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 1.0/7.0, 5.0/7.0, 5.0/7.0, 25, 29 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 5.0/7.0, 1.0/7.0, 5.0/7.0, 30, 33 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 4.0/7.0, 1.0/7.0, 1.0/7.0, 34, 37 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 1.0/7.0, 4.0/7.0, 1.0/7.0, 38, 40 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 2.0/7.0, 4.0/7.0, 4.0/7.0, 41, 43 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 3.0/7.0, 4.0/7.0, 2.0/7.0, 4.0/7.0, 44, 45 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 3.0/7.0, 2.0/7.0, 2.0/7.0, 46, 47 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 3.0/7.0, 2.0/7.0, 3.0/7.0, 3.0/7.0, 48, 49 ) );

            iNumSteps = 51.0;
            iIsRect = ETrue;
            iSubtype = ETopRightClockwise;
            }
        else if ( aTransition->Subtype() == KBottomRightClockwise ) 
            {
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 6.0/7.0, 0.0/7.0, 6.0/7.0, 1, 7 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 5.0/7.0, 0.0/7.0, 0.0/7.0, 8, 13 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 0.0/7.0, 6.0/7.0, 0.0/7.0, 14, 19 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 1.0/7.0, 6.0/7.0, 5.0/7.0, 20, 24 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 5.0/7.0, 1.0/7.0, 5.0/7.0, 25, 29 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 4.0/7.0, 1.0/7.0, 1.0/7.0, 30, 33 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 1.0/7.0, 5.0/7.0, 1.0/7.0, 34, 37 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 2.0/7.0, 5.0/7.0, 4.0/7.0, 38, 40 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 4.0/7.0, 2.0/7.0, 4.0/7.0, 41, 43 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 3.0/7.0, 2.0/7.0, 2.0/7.0, 44, 45 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 3.0/7.0, 2.0/7.0, 4.0/7.0, 2.0/7.0, 46, 47 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 3.0/7.0, 3.0/7.0, 3.0/7.0, 48, 49 ) );

            iNumSteps = 51.0;
            iIsRect = ETrue;
            iSubtype = EBottomRightClockwise;
            }
        else if ( aTransition->Subtype() == KBottomLeftClockwise ) 
            {
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 6.0/7.0, 0.0/7.0, 0.0/7.0, 1, 7 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 0.0/7.0, 6.0/7.0, 0.0/7.0, 8, 13 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 1.0/7.0, 6.0/7.0, 6.0/7.0, 14, 19 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 6.0/7.0, 1.0/7.0, 6.0/7.0, 20, 24 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 5.0/7.0, 1.0/7.0, 1.0/7.0, 25, 29 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 1.0/7.0, 5.0/7.0, 1.0/7.0, 30, 33 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 2.0/7.0, 5.0/7.0, 5.0/7.0, 34, 37 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 5.0/7.0, 2.0/7.0, 5.0/7.0, 38, 40 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 4.0/7.0, 2.0/7.0, 2.0/7.0, 41, 43 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 3.0/7.0, 2.0/7.0, 4.0/7.0, 2.0/7.0, 44, 45 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 3.0/7.0, 4.0/7.0, 4.0/7.0, 46, 47 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 3.0/7.0, 4.0/7.0, 3.0/7.0, 3.0/7.0, 48, 49 ) );

            iNumSteps = 51.0;
            iIsRect = ETrue;
            iSubtype = EBottomLeftClockwise;
            }
        else if ( aTransition->Subtype() == KTopLeftCounterClockwise ) 
            {
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 0.0/7.0, 0.0/7.0, 6.0/7.0, 1, 7 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 6.0/7.0, 6.0/7.0, 6.0/7.0, 8, 13 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 5.0/7.0, 6.0/7.0, 0.0/7.0, 14, 19 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 0.0/7.0, 1.0/7.0, 0.0/7.0, 20, 24 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 1.0/7.0, 1.0/7.0, 5.0/7.0, 25, 29 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 5.0/7.0, 5.0/7.0, 5.0/7.0, 30, 33 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 1.0/7.0, 2.0/7.0, 1.0/7.0, 38, 40 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 2.0/7.0, 2.0/7.0, 4.0/7.0, 41, 43 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 3.0/7.0, 4.0/7.0, 4.0/7.0, 4.0/7.0, 44, 45 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 3.0/7.0, 4.0/7.0, 2.0/7.0, 46, 47 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 3.0/7.0, 2.0/7.0, 3.0/7.0, 3.0/7.0, 48, 49 ) );
            
            iNumSteps = 51.0;
            iIsRect = ETrue;
            iSubtype = ETopLeftCounterClockwise;
            }
        else if ( aTransition->Subtype() == KTopRightCounterClockwise ) 
            {
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 0.0/7.0, 0.0/7.0, 0.0/7.0, 1, 7 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 1.0/7.0, 0.0/7.0, 6.0/7.0, 8, 13 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 6.0/7.0, 6.0/7.0, 6.0/7.0, 14, 19 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 5.0/7.0, 6.0/7.0, 1.0/7.0, 20, 24 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 1.0/7.0, 1.0/7.0, 1.0/7.0, 25, 29 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 2.0/7.0, 1.0/7.0, 5.0/7.0, 30, 33 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 5.0/7.0, 5.0/7.0, 5.0/7.0, 34, 37 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 4.0/7.0, 5.0/7.0, 2.0/7.0, 38, 40 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 2.0/7.0, 2.0/7.0, 2.0/7.0, 41, 43 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 3.0/7.0, 2.0/7.0, 4.0/7.0, 44, 45 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 3.0/7.0, 4.0/7.0, 4.0/7.0, 4.0/7.0, 46, 47 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 3.0/7.0, 3.0/7.0, 3.0/7.0, 48, 49 ) );
            
            iNumSteps = 51.0;
            iIsRect = ETrue;
            iSubtype = ETopRightCounterClockwise;
            }
        else if ( aTransition->Subtype() == KBottomLeftCounterClockwise ) 
            {
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 6.0/7.0, 6.0/7.0, 6.0/7.0, 1, 7 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 5.0/7.0, 6.0/7.0, 0.0/7.0, 8, 13 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 0.0/7.0, 0.0/7.0, 0.0/7.0, 14, 19 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 1.0/7.0, 0.0/7.0, 5.0/7.0, 20, 24 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 5.0/7.0, 5.0/7.0, 5.0/7.0, 25, 29 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 4.0/7.0, 5.0/7.0, 1.0/7.0, 30, 33 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 1.0/7.0, 1.0/7.0, 1.0/7.0, 34, 37 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 2.0/7.0, 1.0/7.0, 4.0/7.0, 38, 40 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 4.0/7.0, 4.0/7.0, 4.0/7.0, 41, 43 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 3.0/7.0, 4.0/7.0, 2.0/7.0, 44, 45 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 3.0/7.0, 2.0/7.0, 2.0/7.0, 2.0/7.0, 46, 47 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 3.0/7.0, 3.0/7.0, 3.0/7.0, 48, 49 ) );

            iNumSteps = 51.0;
            iIsRect = ETrue;
            iSubtype = EBottomLeftCounterClockwise;
            }
        else if ( aTransition->Subtype() == KBottomRightCounterClockwise ) 
            {
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 6.0/7.0, 6.0/7.0, 0.0/7.0, 1, 7 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 0.0/7.0, 0.0/7.0, 0.0/7.0, 8, 13 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 1.0/7.0, 0.0/7.0, 6.0/7.0, 14, 19 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 6.0/7.0, 5.0/7.0, 6.0/7.0, 20, 24 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 5.0/7.0, 5.0/7.0, 1.0/7.0, 25, 29 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 1.0/7.0, 1.0/7.0, 1.0/7.0, 30, 33 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 2.0/7.0, 1.0/7.0, 5.0/7.0, 34, 37 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 5.0/7.0, 4.0/7.0, 5.0/7.0, 38, 40 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 4.0/7.0, 4.0/7.0, 2.0/7.0, 41, 43 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 3.0/7.0, 2.0/7.0, 2.0/7.0, 2.0/7.0, 44, 45 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 3.0/7.0, 2.0/7.0, 4.0/7.0, 46, 47 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 3.0/7.0, 4.0/7.0, 3.0/7.0, 3.0/7.0, 48, 49 ) );

            iNumSteps = 51.0;
            iIsRect = ETrue;
            iSubtype = EBottomRightCounterClockwise;
            }
        else
            {
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 0.0/7.0, 6.0/7.0, 0.0/7.0, 1, 7 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 1.0/7.0, 6.0/7.0, 6.0/7.0, 8, 13 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 6.0/7.0, 0.0/7.0, 6.0/7.0, 14, 19 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 5.0/7.0, 0.0/7.0, 1.0/7.0, 20, 24 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 1.0/7.0, 5.0/7.0, 1.0/7.0, 25, 29 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 2.0/7.0, 5.0/7.0, 5.0/7.0, 30, 33 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 5.0/7.0, 1.0/7.0, 5.0/7.0, 34, 37 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 4.0/7.0, 1.0/7.0, 2.0/7.0, 38, 40 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 2.0/7.0, 4.0/7.0, 2.0/7.0, 41, 43 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 3.0/7.0, 4.0/7.0, 4.0/7.0, 44, 45 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 3.0/7.0, 4.0/7.0, 2.0/7.0, 4.0/7.0, 46, 47 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 3.0/7.0, 3.0/7.0, 3.0/7.0, 48, 49 ) );

            iNumSteps = 51.0;
            iIsRect = ETrue;
            iSubtype = ETopLeftClockwise;
            }
        }
    else if ( aTransition->Type() == KParallelSnakesWipe ) 
        {
        iType = EParallelSnakesWipe;
        
        if ( aTransition->Subtype() == KVerticalBottomSame ) 
            {
            iNumSteps = 1.0;
            iIsRect = ETrue;
            iSubtype = EVerticalBottomSame;
            }
        else 
            {
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 0.0/7.0, 0.0/7.0, 6.0/7.0, 1, 7 ) );

            iNumSteps = 1.0;
            iIsRect = ETrue;
            iSubtype = EVerticalTopSame;
            }
        }
    else
        {
        iType = ESnakeWipe;
    
        if ( aTransition->Subtype() == KTopLeftVertical ) 
            {
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 0.0/7.0, 0.0/7.0, 6.0/7.0, 1, 7 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 6.0/7.0, 1.0/7.0, 0.0/7.0, 8, 14 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 0.0/7.0, 2.0/7.0, 6.0/7.0, 15, 21 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 3.0/7.0, 6.0/7.0, 3.0/7.0, 0.0/7.0, 22, 28 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 0.0/7.0, 4.0/7.0, 6.0/7.0, 29, 35 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 6.0/7.0, 5.0/7.0, 0.0/7.0, 36, 42 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 0.0/7.0, 6.0/7.0, 6.0/7.0, 43, 49 ) );
     
            iNumSteps = 49.0;
            iIsRect = ETrue;
            iSubtype = ETopLeftVertical;
            }
        else if ( aTransition->Subtype() == KTopLeftDiagonal ) 
            {
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 0.0/7.0, 0.0/7.0, 0.0/7.0, 0, 1 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 1.0/7.0, 1.0/7.0, 0.0/7.0, 1, 2 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 0.0/7.0, 0.0/7.0, 2.0/7.0, 3, 5 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 3.0/7.0, 3.0/7.0, 0.0/7.0, 6, 9 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 0.0/7.0, 0.0/7.0, 4.0/7.0, 10, 14 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 5.0/7.0, 5.0/7.0, 0.0/7.0, 15, 20 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 0.0/7.0, 0.0/7.0, 6.0/7.0, 21, 27 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 7.0/7.0, 7.0/7.0, 0.0/7.0, 28, 35 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 7.0/7.0, 1.0/7.0, 1.0/7.0, 7.0/7.0, 36, 42 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 7.0/7.0, 7.0/7.0, 2.0/7.0, 43, 48 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 7.0/7.0, 3.0/7.0, 3.0/7.0, 7.0/7.0, 49, 53 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 7.0/7.0, 7.0/7.0, 4.0/7.0, 54, 57 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 7.0/7.0, 5.0/7.0, 5.0/7.0, 7.0/7.0, 58, 60 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 7.0/7.0, 7.0/7.0, 6.0/7.0, 61, 62 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 7.0/7.0, 7.0/7.0, 7.0/7.0, 7.0/7.0, 63, 64 ) );

            iNumSteps = 64.0;
            iIsRect = EFalse;
            iSubtype = ETopLeftDiagonal;
            }
        else if ( aTransition->Subtype() == KTopRightDiagonal ) 
            {
            iMatrixSnake->Append( CMatrixSnake::NewL( 7.0/7.0, 0.0/7.0, 7.0/7.0, 0.0/7.0, 0, 1 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 7.0/7.0, 1.0/7.0, 6.0/7.0, 0.0/7.0, 1, 2 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 0.0/7.0, 7.0/7.0, 2.0/7.0, 3, 5 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 7.0/7.0, 3.0/7.0, 4.0/7.0, 0.0/7.0, 6, 9 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 3.0/7.0, 0.0/7.0, 7.0/7.0, 4.0/7.0, 10, 14 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 7.0/7.0, 5.0/7.0, 2.0/7.0, 0.0/7.0, 15, 20 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 1.0/7.0, 0.0/7.0, 7.0/7.0, 6.0/7.0, 21, 27 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 7.0/7.0, 7.0/7.0, 0.0/7.0, 0.0/7.0, 28, 35 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 1.0/7.0, 6.0/7.0, 7.0/7.0, 36, 42 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 5.0/7.0, 7.0/7.0, 0.0/7.0, 2.0/7.0, 43, 48 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 3.0/7.0, 4.0/7.0, 7.0/7.0, 49, 53 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 3.0/7.0, 7.0/7.0, 0.0/7.0, 4.0/7.0, 54, 57 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 5.0/7.0, 2.0/7.0, 7.0/7.0, 58, 60 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 3.0/7.0, 7.0/7.0, 0.0/7.0, 6.0/7.0, 61, 62 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 7.0/7.0, 0.0/7.0, 7.0/7.0, 63, 64 ) );

            iNumSteps = 64.0;
            iIsRect = EFalse;
            iSubtype = ETopRightDiagonal;
            }
        else if ( aTransition->Subtype() == KBottomLeftDiagonal ) 
            {
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 7.0/7.0, 0.0/7.0, 7.0/7.0, 0, 1 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 6.0/7.0, 1.0/7.0, 7.0/7.0, 1, 2 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 7.0/7.0, 0.0/7.0, 5.0/7.0, 3, 5 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 4.0/7.0, 3.0/7.0, 7.0/7.0, 6, 9 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 7.0/7.0, 0.0/7.0, 3.0/7.0, 10, 14 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 2.0/7.0, 5.0/7.0, 7.0/7.0, 15, 20 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 7.0/7.0, 0.0/7.0, 1.0/7.0, 21, 27 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 0.0/7.0, 7.0/7.0, 7.0/7.0, 28, 35 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 7.0/7.0, 6.0/7.0, 1.0/7.0, 0.0/7.0, 36, 42 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 0.0/7.0, 7.0/7.0, 5.0/7.0, 43, 48 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 7.0/7.0, 4.0/7.0, 3.0/7.0, 0.0/7.0, 49, 53 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 0.0/7.0, 7.0/7.0, 3.0/7.0, 54, 57 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 7.0/7.0, 2.0/7.0, 5.0/7.0, 0.0/7.0, 58, 60 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 0.0/7.0, 7.0/7.0, 1.0/7.0, 61, 62 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 7.0/7.0, 0.0/7.0, 7.0/7.0, 0.0/7.0, 63, 64 ) );
        
            iNumSteps = 64.0;
            iIsRect = EFalse;
            iSubtype = EBottomLeftDiagonal;
            }
        else if ( aTransition->Subtype() == KBottomRightDiagonal ) 
            {
            iMatrixSnake->Append( CMatrixSnake::NewL( 7.0/7.0, 7.0/7.0, 7.0/7.0, 7.0/7.0, 0, 1 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 7.0/7.0, 7.0/7.0, 6.0/7.0, 1, 2 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 7.0/7.0, 5.0/7.0, 5.0/7.0, 7.0/7.0, 3, 5 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 7.0/7.0, 7.0/7.0, 4.0/7.0, 6, 9 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 7.0/7.0, 3.0/7.0, 3.0/7.0, 7.0/7.0, 10, 14 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 7.0/7.0, 7.0/7.0, 2.0/7.0, 15, 20 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 7.0/7.0, 1.0/7.0, 1.0/7.0, 7.0/7.0, 21, 27 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 7.0/7.0, 7.0/7.0, 0.0/7.0, 28, 35 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 0.0/7.0, 0.0/7.0, 6.0/7.0, 36, 42 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 5.0/7.0, 5.0/7.0, 0.0/7.0, 43, 48 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 4.0/7.0, 0.0/7.0, 0.0/7.0, 4.0/7.0, 49, 53 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 3.0/7.0, 3.0/7.0, 0.0/7.0, 54, 57 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 2.0/7.0, 0.0/7.0, 0.0/7.0, 2.0/7.0, 58, 60 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 1.0/7.0, 1.0/7.0, 0.0/7.0, 61, 62 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 0.0/7.0, 0.0/7.0, 0.0/7.0, 63, 64 ) );
        
            iNumSteps = 64.0;
            iIsRect = EFalse;
            iSubtype = EBottomRightDiagonal;
            }
        else 
            {
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 0.0/7.0, 6.0/7.0, 0.0/7.0, 1, 7 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 1.0/7.0, 0.0/7.0, 1.0/7.0, 8, 14 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 2.0/7.0, 6.0/7.0, 2.0/7.0, 15, 21 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 3.0/7.0, 0.0/7.0, 3.0/7.0, 22, 28 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 4.0/7.0, 6.0/7.0, 4.0/7.0, 29, 35 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 6.0/7.0, 5.0/7.0, 0.0/7.0, 5.0/7.0, 36, 42 ) );
            iMatrixSnake->Append( CMatrixSnake::NewL( 0.0/7.0, 6.0/7.0, 6.0/7.0, 6.0/7.0, 43, 49 ) );

            iNumSteps = 49.0;
            iIsRect = ETrue;
            iSubtype = ETopLeftHorizontal;
            }
        }

    if ( aTransition->iReverse ) 
        {
        for ( TInt i = 0; i < (*iMatrixSnake).Count(); i++ ) 
            {
            // create reverse snake
            TReal bt = ( iNumSteps + 1 ) - (*iMatrixSnake)[i]->iEndTime;
            TRPoint bp = (*iMatrixSnake)[i]->iEndPos;
            (*iMatrixSnake)[i]->iEndTime = (TInt)(iNumSteps + 1) - (*iMatrixSnake)[i]->iBeginTime;
            (*iMatrixSnake)[i]->iEndPos = (*iMatrixSnake)[i]->iBeginPos;
            (*iMatrixSnake)[i]->iBeginTime = (TInt)bt;
            (*iMatrixSnake)[i]->iBeginPos = bp;
            }
        }

    if ( iIsRect ) 
        {
        // rectangle shape
        iShapeVertices->Append( TRPoint( -0.5, -0.5 ) );
        iShapeVertices->Append( TRPoint( 0.5, -0.5 ) );
        iShapeVertices->Append( TRPoint( 0.5, 0.5 ) );
        iShapeVertices->Append( TRPoint( -0.5, 0.5 ) );
        }
    else 
        {
        // diamond shape
        iShapeVertices->Append( TRPoint( 0, -1.0 ) );
        iShapeVertices->Append( TRPoint( 1.0, 0 ) );
        iShapeVertices->Append( TRPoint( 0, 1.0 ) );
        iShapeVertices->Append( TRPoint( -1.0, 0 ) );
        }

    iBufferBitmap = aBuffer;

    iPShape = new(ELeave) CArrayFixFlat<TPoint>(iShapeVertices->Count());
    }

// -----------------------------------------------------------------------------
// CMatrixWipeTransitionFilter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMatrixWipeTransitionFilter* CMatrixWipeTransitionFilter::NewL( CSmilTransition* aTransition,
                                                                CFbsBitmap* aBuffer, 
                                                                MSmilMedia* aMedia )
    {
    CMatrixWipeTransitionFilter* self = new(ELeave) CMatrixWipeTransitionFilter();
    
    CleanupStack::PushL( self );
    self->ConstructL( aTransition, aBuffer, aMedia );
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// CMatrixWipeTransitionFilter::~CMatrixWipeTransitionFilter
// Destructor.
// -----------------------------------------------------------------------------
//
CMatrixWipeTransitionFilter::~CMatrixWipeTransitionFilter()
    {
    if ( iMatrixSnake ) 
        {
        for ( TInt i = 0; i < iMatrixSnake->Count(); i++ )
            {
            delete (*iMatrixSnake)[i];
            }

        iMatrixSnake->Reset();
        delete iMatrixSnake;
        }

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
// CMatrixWipeTransitionFilter::Draw
// Draw method, calculate shapes and draw bitmaps accordingly.
// -----------------------------------------------------------------------------
//
void CMatrixWipeTransitionFilter::Draw( CGraphicsContext& aGc,
                                        const TRect& /*aRect*/,
                                        const CFbsBitmap* aTargetBitmap,
                                        const TPoint& aTarget,
                                        const CFbsBitmap* aMaskBitmap )
    {
    if ( !aTargetBitmap )
        {
        return;
        }

    if ( iType == ESnakeWipe ||
         iType == ESpiralWipe)
        {
        TInt i;
        TReal w = aTargetBitmap->SizeInPixels().iWidth;
        TReal h = aTargetBitmap->SizeInPixels().iHeight;
        TRect brect( 0, 0, (TInt)w, (TInt)h );
        TPoint tpIn = aTarget;

        CFbsBitmap* pMaskBitmap = NULL;
        
        TInt leaveValue;
        pMaskBitmap = new CFbsBitmap();
        if ( pMaskBitmap == NULL )
            {
            return;
            }


        // create new bitmap for squares mask
        TInt err = pMaskBitmap->Create( aTargetBitmap->SizeInPixels(), EGray2 );
        if ( err != KErrNone )
            {
            delete pMaskBitmap;
            return;
            }
            
        CBitmapContext* bgc = static_cast<CBitmapContext*>( &aGc );
        CFbsBitmap* pInMap = const_cast<CFbsBitmap*>( aTargetBitmap );
        bgc->SetBrushStyle( CGraphicsContext::ENullBrush );
        bgc->SetPenStyle( CGraphicsContext::ENullPen );

        CFbsBitmapDevice* mbdev = NULL;
        CFbsBitGc* mbgc = NULL;
        TRAP( leaveValue, // cannot leave in draw()
              mbdev = CFbsBitmapDevice::NewL( (CFbsBitmap*)pMaskBitmap );
              mbgc = CFbsBitGc::NewL();
             );
             
        if ( leaveValue )
            {    // clean up
            delete pMaskBitmap;
            delete mbdev;
            delete mbgc;
            return;
            }

        mbgc->Activate( mbdev );
        mbgc->SetBrushStyle( CGraphicsContext::ESolidBrush );
        mbgc->SetPenStyle( CGraphicsContext::ESolidPen );

        if (iTransOut )
            {
            mbgc->SetBrushColor( TRgb() );
            mbgc->SetPenColor( TRgb() );
            mbgc->DrawRect( brect );
            mbgc->SetBrushColor( TRgb( 0, 0, 0 ) );
            mbgc->SetPenColor( TRgb(0, 0, 0 ) );
            }
        else
            {
            mbgc->SetBrushColor( TRgb( 0, 0, 0 ) );
            mbgc->SetPenColor( TRgb( 0, 0, 0 ) );
            mbgc->DrawRect( brect );
            mbgc->SetBrushColor( TRgb() );
            mbgc->SetPenColor( TRgb() );
            }

        TReal scalew = w / 7.0 + 1.0;
        TReal scaleh = h / 7.0 + 1.0;

        TRPoint shiftxy = TRPoint( scalew / 2.0, scaleh / 2.0 );
        if ( !iIsRect )
            {
            shiftxy = TRPoint( 0, 0 );
            }

        TReal percent = ( iNumSteps + 2 ) * iPercent / 100.0;

        for ( i = 0; i < (*iMatrixSnake).Count(); i++ ) 
            {
            if ( (*iMatrixSnake)[i]->iBeginTime < percent ) 
                {
                TRPoint bp = (*iMatrixSnake)[i]->iBeginPos;
                TRPoint ep = (*iMatrixSnake)[i]->iEndPos;
                TReal bt = (*iMatrixSnake)[i]->iBeginTime;
                TReal et = (*iMatrixSnake)[i]->iEndTime;
                TReal dt = et - bt;
                TReal steps = dt;
                TRPoint dp = (ep - bp) / steps;

                if ( et > percent )
                    {
                    et = percent; 
                    }

                for ( TReal s = bt; s <= et; s++ ) 
                    {
                    iPShape->Reset();
                    for ( TInt k = 0; k < iShapeVertices->Count(); k++ ) 
                        {
                        TRAP( err, iPShape->AppendL( 
                            TPoint( (TInt)( (*iShapeVertices)[k].iX * scalew + bp.iX * w + shiftxy.iX ), 
                                    (TInt)( (*iShapeVertices)[k].iY * scaleh + bp.iY * h + shiftxy.iY ) ) ) );
                        }
                        
                    err = mbgc->DrawPolygon( iPShape );
                    if ( err != KErrNone )
                        {
                        break;
                        }
                        
                    bp = bp + dp;
                    }
                }
            }

        if ( aMaskBitmap )
            {
            mbgc->SetDrawMode( CGraphicsContext::EDrawModeAND );
            mbgc->BitBlt( TPoint( 0, 0 ), aMaskBitmap );
            }

        bgc->BitBltMasked( tpIn, pInMap, brect, pMaskBitmap, EFalse );

        delete mbgc;
        delete mbdev;

        iPShape->Reset();
        
        delete pMaskBitmap;
        }
    }

// -----------------------------------------------------------------------------
// CMatrixWipeTransitionFilter::ResolutionMilliseconds
// -----------------------------------------------------------------------------
//
TInt CMatrixWipeTransitionFilter::ResolutionMilliseconds() const
    {
    return ESmilTransitionResolution;
    }

// ============================ CMatrixSnake ===============================

// -----------------------------------------------------------------------------
// CMatrixSnake::CMatrixSnake
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMatrixSnake::CMatrixSnake()
    {
    }

// -----------------------------------------------------------------------------
// CMatrixSnake::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMatrixSnake::ConstructL( TReal abx, TReal aby, TReal aex, TReal aey, TInt abt, TInt aet )
    {
    iBeginPos = TRPoint( abx, aby );
    iEndPos = TRPoint( aex, aey );
    iBeginTime = abt;
    iEndTime = aet;
    }

// -----------------------------------------------------------------------------
// CMatrixSnake::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMatrixSnake* CMatrixSnake::NewL( TReal abx,TReal aby,TReal aex,TReal aey,TInt abt,TInt aet )
    {
    CMatrixSnake* self = new(ELeave) CMatrixSnake();
 
    CleanupStack::PushL( self );
    self->ConstructL( abx, aby, aex, aey, abt, aet );
    CleanupStack::Pop( self );
    
    return self;
    }
    
// -----------------------------------------------------------------------------
// CMatrixSnake::~CMatrixSnake
// Destructor.
// -----------------------------------------------------------------------------
//
CMatrixSnake::~CMatrixSnake()
    {
    }
    
//  End of File  

