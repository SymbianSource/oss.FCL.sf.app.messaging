/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IMSPageResourceControl.h
*
*/


#ifndef CIMSPageResourceControl_H
#define CIMSPageResourceControl_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
// MACROS
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// DATA TYPES
enum TIMSPageResource
    {
    EIPRRadioButton = 0,
    EIPRCheckbox,
    EIPRText,
    EIPRNumber,
    EIPRScNumber,
    EIPRSecret,
    EIPRMultiLine,
    EIPRLastPage
    };

typedef CArrayFixFlat<TInt> CResourceStack;
typedef CArrayPtrFlat<CResourceStack> CResourceStackArray;

// CLASS DECLARATION

/**
*
*  @lib
*  @since Series60 3.0
*/
NONSHARABLE_CLASS( CIMSPageResourceControl ): public CBase
    {
    public:  // Constructors and destructor

        /**
        * Creates object from CIMSPageResourceControl and leaves it to cleanup stack
        * @since S60 3.1
        * @return, Constructed object
        */
        static CIMSPageResourceControl* NewLC();

        /**
        * Create object from CIMSPageResourceControl
        * @since S60 3.1
        * @return, Constructed object
        */
        static CIMSPageResourceControl* NewL();

        /**
        * Destructor
        * @since S60 3.1
        */
        virtual ~CIMSPageResourceControl();

    public: // New functions

        /**
        *
        * @since S60 3.1
        */
        void PushResourceL(
            const TInt aType,
            const TInt aPageResource );

        /**
        *
        * @since S60 3.1
        */
        TInt Resource(
            const TInt aType,
            const TBool aRemove = ETrue );

        /**
        *
        * @since S60 3.1
        */
        TInt Count( const TInt aType );

        /**
        *
        * @since S60 3.1
        */
        CResourceStack* CreateStackL( const TInt aPageResource );

        /**
        *
        * @since S60 3.1
        */
        TInt operator[]( const TInt aType );

    public: // Functions from base classes

    protected:  // Constructors

        /**
        * Default constructor for classCIMSPageResourceControl
        * @since S60 3.1
        * @return, Constructed object
        */
        CIMSPageResourceControl();

        /**
        * Symbian 2-phase constructor
        * @since S60 3.1
        */
        void ConstructL();

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions

        // TOOLS

        /**
        *
        * @since S60 3.1
        */
        CResourceStack& Stack( const TInt aType );

    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data
        CResourceStackArray*    iPageResources;
    };

#endif //  CIMSPageResourceControl_H

// End of File
