/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ImumInSettingsDataArray.h
*
*/


#ifndef CIMUMINDATAKEYARRAY_H
#define CIMUMINDATAKEYARRAY_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CImumInSettingsDataAttribute;
class CImumMboxDefaultData;

// CLASS DECLARATION

/**
 *  Class to store and handle settings data attributes.
 *
 *  @lib
 *  @since S60 v3.2
 *  @see ImumInSettingsDataAttribute.h
 */
class CImumInSettingsDataArray : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Creates object from CImumInSettingsDataArray and leaves it to cleanup stack
         *
         * @since S60 v3.2
         * @return, Constructed object
         */
        static CImumInSettingsDataArray* NewLC();

        /**
         * Create object from CImumInSettingsDataArray
         *
         * @since S60 v3.2
         * @return, Constructed object
         */
        static CImumInSettingsDataArray* NewL();

        /**
         * Destructor
         *
         * @since S60 v3.2
         */
        virtual ~CImumInSettingsDataArray();

    public: // New functions

        /**
         * General method to retrieve the key from the settings
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        TInt GetAttr(
            const TUint aAttributeKey,
            CImumInSettingsDataAttribute*& aAttribute ) const;

        /**
         * General method to retrieve the key from the settings
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        TInt SetAttr(
            CImumInSettingsDataAttribute& aAttribute );

        /**
         *
         *
         * @since S60 v3.2
         * @param
         */
        void ResetAttr( const TUint aAttributeKey );

        /**
         * Sets the default value to all item excluding the id's in the list
         *
         * @since S60 v3.2
         * @param aExcludeList
         */
        void ResetAll( const RArray<TUint>& aExcludeList );

        /**
         * Creates an identical copy of the object
         *
         * @since S60 v3.2
         * @return
         * @leave
         */
        CImumInSettingsDataArray* CloneL() const;

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         * @leave
         */
        TInt Count() const;

    public: // Functions from base classes

    protected:  // Constructors

        /**
         * Default constructor for classCImumInSettingsDataArray
         *
         * @since S60 v3.2
         * @return, Constructed object
         */
        CImumInSettingsDataArray();

        /**
         * Symbian 2-phase constructor
         *
         * @since S60 v3.2
         */
        void ConstructL();

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions

        /**
         * Check from the list if the item exists
         *
         * @since S60 v3.2
         * @param
         * @param
         * @return
         */
        TBool HasItem(
            const TUint aAttributeKey,
            TInt& aIndex ) const;

    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data

        /** */
        typedef RPointerArray<CImumInSettingsDataAttribute> RArraySettings;

        // Owned: Array of settings
        RArraySettings                  iSettings;
        // Owned: Item for searching
        CImumInSettingsDataAttribute*   iSearchItem;
    };

#endif //  CIMUMINDATAKEYARRAY_H

// End of File
