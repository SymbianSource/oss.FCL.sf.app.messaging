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
* Description:  ImumInSettingsDataCtrl.h
*
*/


#ifndef IMUMINDATAKEYCTRL_H
#define IMUMINDATAKEYCTRL_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class MImumDaSettingsDataCollection;
class CImumInSettingsDataArray;
class CImumInSettingsDataAttribute;

// CLASS DECLARATION

/**
 *  Static class to control settings data object handling.
 *
 *  @lib ImumUtils.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( ImumInSettingsDataCtrl )
    {
    public: // Constructors and destructor
    public: // New functions

        /**
         * Creates an attribute with specific parameters into the array
         *
         * @since S60 v3.2
         * @param aArray The settings array, the attribute will be created
         * @param aDefaultData Default data container
         * @param aAttributeKey Unique key Id
         * @param aAttributeType Type of the key
         * @param aAttributeValue The value of the attribute
         * @param aEmptyDefaultValue The default value of the attribute
         * @return KErrNone, when ok
         */
        template<class TDefaultDataType, class TAttributeType, class TDefaultType>
        static inline TInt CreateToArray(
            CImumInSettingsDataArray& aArray,
            const TDefaultDataType* aDefaultData,
            const TUint aAttributeKey,
            const TInt aAttributeType,
            const TAttributeType& aAttributeValue,
            TDefaultType& aEmptyDefaultValue );

        /**
         * Gets attribute value from the array
         *
         * @since S60 v3.2
         * @param aArray Array which contains the attribute
         * @param aAttributeKey Unique id of the key to be retrieved
         * @param aAttributeType Type of the attribute
         * @param aAttributeValue The return parameter for the value
         * @return KErrNone, when successful
         */
        template<class TAttributeType>
        static inline TInt RetrieveFromArray(
            CImumInSettingsDataArray& aArray,
            const TUint aAttributeKey,
            const TInt aAttributeType,
            TAttributeType& aAttributeValue );

        /**
         * Determines, if the attribute is text type
         *
         * @since S60 v3.2
         * @param aAttributeType Type of the attribute
         * @return ETrue, when attribute is a text type
         */
        static TBool IsText( const TInt aAttributeType );

        /**
         * Determines, if the attribute is number type
         *
         * @since S60 v3.2
         * @param aAttributeType Type of the attribute
         * @return ETrue, when attribute is a text type
         */
        static TBool IsNumber( const TInt aAttributeType );

        /**
         * Gets next available key in the setting list.
         *
         * @deprecated
         * @since S60 v3.2
         */
        static TBool GetNextKey(
            TUint& aKey,
            const TBool& aIncludeCompulsory );

        /**
         * Checks if the item is compulsory
         *
         * @deprecated
         * @since S60 v3.2
         */
        static TBool IsCompulsory( TUint& aKey );


        /**
         * Copies the content of source to destination
         *
         * @deprecated
         * @since S60 v3.2
         * @param
         * @return
         */
        template<class T>
        static inline TInt Copy(
            MImumDaSettingsDataCollection& aDestination,
            const MImumDaSettingsDataCollection& aSource,
            const TUint aAttributeKey,
            T& aAttribute,
            const TInt aErrorCode );

        /**
         *
         *
         * @deprecated
         * @since S60 v3.2
         * @param
         * @return
         */
        template<TInt L>
        static inline TBool CompareText(
            const MImumDaSettingsDataCollection& aDestinationSettings,
            const MImumDaSettingsDataCollection& aSourceSettings,
            const TUint aAttributeKey );

        /**
         *
         *
         * @deprecated
         * @since S60 v3.2
         * @param
         * @return
         */
        template<TInt L>
        static inline TBool CompareText8(
            const MImumDaSettingsDataCollection& aDestinationSettings,
            const MImumDaSettingsDataCollection& aSourceSettings,
            const TUint aAttributeKey );

        /**
         *
         *
         * @deprecated
         * @since S60 v3.2
         * @param
         * @return
         */
        template<class T>
        static inline TBool CompareNum(
            const MImumDaSettingsDataCollection& aDestinationSettings,
            const MImumDaSettingsDataCollection& aSourceSettings,
            const TUint aAttributeKey );

    public: // Functions from base classes

    protected:  // Constructors
    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions

        /**
         * Creates attribute object
         *
         * @since S60 v3.2
         * @param aAttributeKey Unique key ID
         * @param aAttributeType Type of the key
         * @param aAttributeValue The value of the attribute
         * @param aEmptyDefaultValue The default value of the attribute
         * @return New attribute object. Ownership to caller.
         */
        template<class TValueType, class TDefaultType>
        static inline CImumInSettingsDataAttribute* CreateAttributeL(
            const TUint aAttributeId,
            const TInt aAttributeType,
            const TValueType& aAttributeValue,
            const TDefaultType& aAttributeDefault );

        /**
         * Gets the type of the attribute
         *
         * @since S60 v3.2
         * @param aAttributeKey Unique key ID
         * @return Type of the attribute
         */
        static TInt GetKeyAttributeType( const TUint aAttributeKey );

        /**
         *
         *
         * @deprected
         * @since S60 v3.2
         * @param
         * @return
         */
        static TInt MatchKey(
            const TUint aAttributeKey,
            const TInt aRequiredKey );

    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data
    };

#include "ImumInSettingsDataCtrl.inl"

#endif //  IMUMINDATAKEYCTRL_H

// End of File
