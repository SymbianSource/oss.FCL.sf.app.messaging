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
* Description: barwipetransitionfilter  declaration
*
*/


#ifndef CBARWIPETRANSITIONFILTER_H
#define CBARWIPETRANSITIONFILTER_H

//  INCLUDES
#include <e32base.h>
#include <smiltransitionfilter.h>
#include <smiltransitioncommondefs.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Bar wipe transition filter.
*
*  @lib smiltran.lib
*  @since S60 2.1
*/
NONSHARABLE_CLASS( CBarWipeTransitionFilter ) : public CSmilTransitionFilter, 
                                                private TTransitionDefs
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CBarWipeTransitionFilter* NewL( CSmilTransition* aTransition, 
                                               CFbsBitmap* aBuffer, 
                                               MSmilMedia* aMedia );
        /**
        * Destructor.
        */
        ~CBarWipeTransitionFilter();

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
        CBarWipeTransitionFilter();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CSmilTransition* aTransition, 
                         CFbsBitmap* aBuffer, 
                         MSmilMedia* aMedia );        
        
    public: // Data
        
        // Transition effect type.
        TTransitionType iType;
        
        // Transition effect subtype.
        TTransitionSubtype iSubtype;
    };

#endif // CBARWIPETRANSITIONFILTER_H

// End of File