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
* Description: iriswipetransitionfilter  declaration
*
*/


#ifndef CIRISWIPETRANSITIONFILTER_H
#define CIRISWIPETRANSITIONFILTER_H

//  INCLUDES
#include <e32base.h>
#include <smiltransitionfilter.h>
#include <smiltransitioncommondefs.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CFbsBitmap;
class TRPoint;


// CLASS DECLARATION

/**
*  Iris wipe transition filter.
*
*  @lib smiltran.lib
*  @since S60 2.1
*/
NONSHARABLE_CLASS( CIrisWipeTransitionFilter ) : public CSmilTransitionFilter, 
                                                 private TTransitionDefs
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CIrisWipeTransitionFilter* NewL( CSmilTransition* aTransition, 
                                                CFbsBitmap* aBuffer, 
                                                MSmilMedia* aMedia );
        /**
        * Destructor.
        */
        ~CIrisWipeTransitionFilter();

    public: // Functions from base classes
    
        /**
        * From CSmilTransitionFilter. See smiltransitionfilter.h
        * @since S60 2.1
        */     
        void Draw( CGraphicsContext& aGc, 
                   const TRect& aRect, 
                   const CFbsBitmap* aTargetBitmap, 
                   const TPoint& aTarget, 
                   const CFbsBitmap* aMaskBitmap = 0 );

        /**
        * From CSmilTransitionFilter. See smiltransitionfilter.h
        * @since S60 2.1
        */     
        TInt ResolutionMilliseconds() const;
        
    private:  // New functions
        
        /**
        * C++ default constructor.
        */
        CIrisWipeTransitionFilter();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CSmilTransition* aTransition, 
                         CFbsBitmap* aBuffer, 
                         MSmilMedia* aMedia );
        
    public:

        // Transition effect type.
        TTransitionType iType;
        
        // Transition effect subtype
        TTransitionSubtype iSubtype;

    private:
        
        // The iris shape vertices in an array
        RArray<TRPoint>* iShapeVertices; 
        
        // Maximum scale
        TReal iScaleMax;
        
        // Shape array.
        CArrayFixFlat<TPoint>* iPShape;
    };

#endif // CIRISWIPETRANSITIONFILTER_H

// End of File