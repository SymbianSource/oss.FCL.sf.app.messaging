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
* Description: trpoint  declaration
*
*/


#ifndef TRPOINT_H
#define TRPOINT_H

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  Coord class with TReal iX and iY
*  
*
*  @lib smiltran.dll
*  @since S60 2.1
*/
NONSHARABLE_CLASS( TRPoint )
    {
    public:  // Constructors and destructor
    
        /**
        * C++ default contructor.
        */
    	TRPoint();
    	
    	/**
        * C++ contructor.
        */
    	TRPoint(TReal aX,TReal aY);
    	
    public: // New functions
    
        TInt operator==( const TRPoint& aPoint ) const;
    	TInt operator!=( const TRPoint& aPoint ) const;
    	
    	TRPoint& operator-=( const TRPoint& aPoint );
    	TRPoint& operator+=( const TRPoint& aPoint );
    	
    	TRPoint operator-( const TRPoint& aPoint ) const;
    	TRPoint operator+( const TRPoint& aPoint ) const;

    	TRPoint operator-() const;
    	
    	TRPoint& operator*=( const TReal aVal );
    	TRPoint& operator/=( const TReal aVal );
    	
    	TRPoint operator*( const TReal aVal ) const;
    	TRPoint operator/( const TReal aVal ) const;
    	
    	TRPoint& operator-=( const TReal aVal );
    	TRPoint& operator+=( const TReal aVal );
    	
    	TRPoint operator-( const TReal aVal ) const;
    	TRPoint operator+( const TReal aVal ) const;
    	
    	void SetXY( TInt aX, TInt aY );
    
    public: // Data
        
        // X - coordinate.
    	TReal iX;
    	
    	// Y - coordinate.
    	TReal iY;
    };

#endif // TRPOINT_H

// End of File
