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
* Description: slidewipetransitionfilter  declaration
*
*/


#ifndef CSLIDEWIPETRANSITIONRENDERER_H
#define CSLIDEWIPETRANSITIONRENDERER_H

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
class CFbsDevice;

// CLASS DECLARATION

/**
*  Slide wipe transition filter.
*
*  @lib smiltran.lib
*  @since S60 2.1
*/
NONSHARABLE_CLASS( CSlideWipeTransitionFilter ) : public CSmilTransitionFilter, 
                                                  private TTransitionDefs
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSlideWipeTransitionFilter* NewL( CSmilTransition* iTransition,
                                                 CFbsBitmap* aBuffer, 
                                                 MSmilMedia* aMedia );
                                                          
        /**
        * Destructor.
        */
        ~CSlideWipeTransitionFilter();
        
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
        CSlideWipeTransitionFilter();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CSmilTransition* aTransition, 
                         CFbsBitmap* aBuffer, 
                         MSmilMedia* aMedia );

    public:
        
        // Transition effect type.
        TTransitionType iType;
        
        // Transition effect subtype.
        TTransitionSubtype iSubtype;

    private:
        
        // Buffer bitmap.
        CFbsBitmap* iBufferBitmap;
        
        // Buffer device.
        CFbsDevice* iBufferDevice;
    };

#endif // CSLIDEWIPETRANSITIONRENDERER_H

// End of File