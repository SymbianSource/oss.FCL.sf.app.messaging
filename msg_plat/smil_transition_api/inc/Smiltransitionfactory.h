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
* Description: Smiltransitionfactory  declaration
*
*/


#ifndef CSMILTRANSITIONFACTORY_H
#define CSMILTRANSITIONFACTORY_H

//  INCLUDES
#include <e32base.h>
#include <smiltransitionfactoryinterface.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  SMIL transition factory. Factory class for instantiating transition filters.
*
*  @lib smiltran.lib
*  @since S60 2.1
*/
class CSmilTransitionFactory : public CBase, 
                               public MSmilTransitionFactory
    {
    public:  // Constructors and destructor

        /**
        * C++ constructor.
        */
        IMPORT_C CSmilTransitionFactory( CFbsBitmap* aBuffer );
        
        /**
        * Destructor.
        */
        ~CSmilTransitionFactory();

    public: // Functions from base classes

        /**
        * From MSmilTransitionFactory. See smiltransitionfactoryinterface.h
        * @since S60 2.1
        */
        CSmilTransitionFilter* CreateFilterL( CSmilTransition* aTransition, 
                                              MSmilMedia* aMedia );
        
        /**
        * From MSmilTransitionFactory. See smiltransitionfactoryinterface.h
        * @since S60 2.1
        */
        TBool IsSupportedType( const TDesC& aTransitionName ) const; 
        
        /**
        * From MSmilTransitionFactory. See smiltransitionfactoryinterface.h
        * @since S60 2.1
        */
        TBool IsSupportedSubtype( const TDesC& aTransitionName, 
                                  const TDesC& aSubtypeName ) const; 
    
    private: 
    
        /**
        * C++ default constructor.
        */
        CSmilTransitionFactory();
    
    public:
    
        // Transition buffer.
        CFbsBitmap* iBuf;
    };

#endif // CSMILTRANSITIONFACTORY_H

// End of File