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
* Description: matrixwipetransitionfilter  declaration
*
*/


#ifndef CMATRIXWIPETRANSITIONFILTER_H
#define CMATRIXWIPETRANSITIONFILTER_H

//  INCLUDES
#include <e32base.h>
#include <smiltransitionfilter.h>
#include <smiltransitioncommondefs.h>
#include "trpoint.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CFbsBitmap;
class CFbsDevice;
class TRPoint;
class CMatrixSnake;

// CLASS DECLARATION

/**
*  Matrix wipe transition filter.
*
*  @lib smiltran.lib
*  @since S60 2.1
*/
NONSHARABLE_CLASS( CMatrixWipeTransitionFilter ) : public CSmilTransitionFilter, 
                                                   private TTransitionDefs
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMatrixWipeTransitionFilter* NewL( CSmilTransition* aTransition, 
                                                  CFbsBitmap* aBuffer, 
                                                  MSmilMedia* aMedia );
        
        /**
        * Destructor.
        */
        ~CMatrixWipeTransitionFilter();

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
        CMatrixWipeTransitionFilter();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CSmilTransition* aTransition, 
                         CFbsBitmap* aBuffer, 
                         MSmilMedia* aMedia );        
        
    public: // Data
        
        // Transition effect type.
        TTransitionType iType;
        
        // Transition effect subtype
        TTransitionSubtype iSubtype;

    private: // Data
        
        // Buffer bitmap.
        CFbsBitmap* iBufferBitmap;

        // The start and end times + positions.
        RArray<CMatrixSnake*>* iMatrixSnake;
        
        // The shape that is drawn in matrix steps.
        RArray<TRPoint>* iShapeVertices; 
        
        // Number of steps in the wipe.
        TReal iNumSteps; 
        
        // Rectangle or diamond doing the wipe.
        TBool iIsRect; 
        
        // Shape array.
        CArrayFixFlat<TPoint>* iPShape;
    };

/**
*  Helper class for matrix wipe.
*
*  @lib smiltran.lib
*  @since S60 2.1
*/
NONSHARABLE_CLASS( CMatrixSnake ) : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMatrixSnake* NewL( TReal abx, 
                                   TReal aby, 
                                   TReal aex, 
                                   TReal aey, 
                                   TInt abt, 
                                   TInt aet );
        
        /**
        * Destructor.
        */
        ~CMatrixSnake();
        
    private:  // New functions
        
        /**
        * C++ default constructor.
        */
        CMatrixSnake();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TReal abx, 
                         TReal aby, 
                         TReal aex, 
                         TReal aey, 
                         TInt abt, 
                         TInt aet );
    
    public: // Data
        
        // Begin position.
        TRPoint iBeginPos;
        
        // End position.
        TRPoint iEndPos;
        
        // Begin time.
        TInt iBeginTime;
        
        // End time.
        TInt iEndTime;
    };

#endif // CMATRIXWIPETRANSITIONFILTER_H

// End of File