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
* Description: trpoint implementation
*
*/



// INCLUDE FILES
#include "trpoint.h"

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
// TRPoint::TRPoint
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TRPoint::TRPoint() :
    iX( 0.0 ),
    iY( 0.0 )
    {
    }

// -----------------------------------------------------------------------------
// TRPoint::TRPoint
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TRPoint::TRPoint( TReal aX, TReal aY ) : 
    iX( aX ), 
    iY( aY )
    {
    }
// -----------------------------------------------------------------------------
// TRPoint::operator==
// -----------------------------------------------------------------------------
//
TInt TRPoint::operator==( const TRPoint& aPoint ) const
    {
    if ( iX == aPoint.iX && iY == aPoint.iY )
        {
        return 0;
        }
        
    return -1;
    }

// -----------------------------------------------------------------------------
// TRPoint::operator!=
// -----------------------------------------------------------------------------
//
TInt TRPoint::operator!=( const TRPoint& aPoint ) const
    {
    if ( iX != aPoint.iX || iY != aPoint.iY )
        {
        return 0;
        }
        
    return -1;
    }

// -----------------------------------------------------------------------------
// TRPoint::operator-=
// -----------------------------------------------------------------------------
//
TRPoint& TRPoint::operator-=( const TRPoint& aPoint )
    {
    iX -= aPoint.iX;
    iY -= aPoint.iY;
    
    return *this;
    }

// -----------------------------------------------------------------------------
// TRPoint::operator+=
// -----------------------------------------------------------------------------
//
TRPoint& TRPoint::operator+=( const TRPoint& aPoint )
    {
    iX += aPoint.iX;
    iY += aPoint.iY;

    return *this;
    }

// -----------------------------------------------------------------------------
// TRPoint::operator-
// -----------------------------------------------------------------------------
//
TRPoint TRPoint::operator-( const TRPoint& aPoint ) const
    {
    return TRPoint( iX - aPoint.iX, iY - aPoint.iY );
    }

// -----------------------------------------------------------------------------
// TRPoint::operator+
// -----------------------------------------------------------------------------
//
TRPoint TRPoint::operator+( const TRPoint& aPoint ) const
    {
    return TRPoint( iX + aPoint.iX, iY + aPoint.iY );
    }

// -----------------------------------------------------------------------------
// TRPoint::operator-
// -----------------------------------------------------------------------------
//
TRPoint TRPoint::operator-() const
    {
    return TRPoint( -iX, -iY );
    }

// -----------------------------------------------------------------------------
// TRPoint::SetXY
// -----------------------------------------------------------------------------
//
void TRPoint::SetXY( TInt aX, TInt aY )
    {
    iX = aX;
    iY = aY;
    }

// -----------------------------------------------------------------------------
// TRPoint::operator*=
// -----------------------------------------------------------------------------
//
TRPoint& TRPoint::operator*=( const TReal aVal )
    {
    iX *= aVal;
    iY *= aVal;
    return *this;
    }

// -----------------------------------------------------------------------------
// TRPoint::operator/=
// -----------------------------------------------------------------------------
//
TRPoint& TRPoint::operator/=( const TReal aVal )
    {
    iX /= aVal;
    iY /= aVal;
    return *this;
    }

// -----------------------------------------------------------------------------
// TRPoint::operator*
// -----------------------------------------------------------------------------
//
TRPoint TRPoint::operator*( const TReal aVal ) const
    {
    return TRPoint( iX * aVal, iY * aVal );
    }

// -----------------------------------------------------------------------------
// TRPoint::operator/
// -----------------------------------------------------------------------------
//
TRPoint TRPoint::operator/( const TReal aVal ) const
    {
    return TRPoint( iX / aVal, iY / aVal );
    }

// -----------------------------------------------------------------------------
// TRPoint::operator+=
// -----------------------------------------------------------------------------
//
TRPoint& TRPoint::operator+=( const TReal aVal )
    {
    iX += aVal;
    iY += aVal;
    return *this;
    }

// -----------------------------------------------------------------------------
// TRPoint::operator-=
// -----------------------------------------------------------------------------
//
TRPoint& TRPoint::operator-=( const TReal aVal )
    {
    iX -= aVal;
    iY -= aVal;
    return *this;
    }

// -----------------------------------------------------------------------------
// TRPoint::operator+
// -----------------------------------------------------------------------------
//
TRPoint TRPoint::operator+( const TReal aVal ) const
    {
    return TRPoint( iX + aVal, iY + aVal );
    }

// -----------------------------------------------------------------------------
// TRPoint::operator-
// -----------------------------------------------------------------------------
//
TRPoint TRPoint::operator-( const TReal aVal ) const
    {
    return TRPoint( iX - aVal, iY - aVal );
    }

//  End of File  
