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
* Description:  ImumInSettingsDataValidator.h
*
*/


#ifndef CIMUMINSETTINGSDATAVALIDATOR_H
#define CIMUMINSETTINGSDATAVALIDATOR_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CImumInternalApiImpl;
class CImumInSettingsDataImpl;
class MImumInSettingsDataCollection;
class CMsvCommDbUtilities;

// CLASS DECLARATION

/**
 *  Class to analyze and correct settings data objects.
 *
 *  @lib ImumUtils.lib
 *  @since S60 v3.2
 */
class CImumInSettingsDataValidator : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Creates object from CImumInSettingsDataValidator and leaves it to cleanup stack
        * @since S60 v3.2
        * @return, Constructed object
        */
        static CImumInSettingsDataValidator* NewLC(
            CImumInternalApiImpl& aInternalApi );

        /**
        * Create object from CImumInSettingsDataValidator
        * @since S60 v3.2
        * @return, Constructed object
        */
        static CImumInSettingsDataValidator* NewL(
            CImumInternalApiImpl& aInternalApi );

        /**
        * Destructor
        * @since S60 v3.2
        */
        virtual ~CImumInSettingsDataValidator();

    public: // New functions

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        TInt Validate(
            CImumInSettingsDataImpl& aSettings );

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        TInt Validate(
            MImumInSettingsDataCollection& aConnectionSettings );

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @leave
         */
        void ValidateL(
            CImumInSettingsDataImpl& aSettings );

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @leave
         */
        void ValidateL(
            MImumInSettingsDataCollection& aConnectionSettings );

    public: // Functions from base classes

    protected:  // Constructors

        /**
        * Default constructor for classCImumInSettingsDataValidator
        * @since S60 v3.2
        * @return, Constructed object
        */
        CImumInSettingsDataValidator(
            CImumInternalApiImpl& aInternalApi );

        /**
        * Symbian 2-phase constructor
        * @since S60 v3.2
        */
        void ConstructL();

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions

        /**
         *
         *
         * @since S60 v3.2
         * @param
         */
        void CheckProtocolsL(
            CImumInSettingsDataImpl& aSettings );

        /**
         * Makes sure that access point has working values set
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        void ValidateAccessPointL(
            MImumInSettingsDataCollection& aConnectionSettings );

        /**
         *
         *
         * @since S60 v3.2
         */
        void CheckEmailAddressL(
            const CImumInSettingsDataImpl& aSettings );

        /**
         *
         *
         * @since S60 v3.2
         */
        void CheckServerAddressL(
            const MImumInSettingsDataCollection& aSettings );

        /**
         *
         *
         * @since S60 v3.2
         * @param
         */
        void CheckMailboxNameL(
            const CImumInSettingsDataImpl& aSettings );

        /**
         * Check and validate security settings
         *
         * @since S60 v3.2
         * @param
         */
        void ValidateSecurityL(
            MImumInSettingsDataCollection& aConnectionSettings );

        /**
         * Returns default port of security setting for the protocol.
         *
         * @since S60 v3.2
         * @param aSecurity Selected security setting
         * @param aProtocol Current protocol
         * @return Default port based on the protocol and security setting
         */
        TInt GetDefaultSecurityPortL(
            const TInt aSecurity,
            const TInt aProtocol );

        /**
         * This method verifies that the key contains valid information
         *
         * @since S60 v3.2
         * @param aSettings Either API's SettingsData or ConnectionSettings
         * @param aAttributeKey Unique id of the key.
         * @param aMinValue Minimum possible value for the setting
         * @param aMaxValue Maximum possible value for the setting
         * @param aErrorCode
         * @return Verified value of the attribute
         * @leave aErrorCode, when setting doesn't fulfill the requirements
         */
        template<class TType, class TSettings>
        inline TType VerifyKeyL(
            TSettings& aSettings,
            const TUint aAttributeKey,
            const TInt aMinValue,
            const TInt aMaxValue,
            const TInt aErrorCode ) const;

        /**
         * This method validates that the key contains valid information
         *
         * @since S60 v3.2
         * @param aSettings Either API's SettingsData or ConnectionSettings
         * @param aAttributeKey Unique id of the key.
         * @param aMinValue Minimum possible value for the setting
         * @param aMaxValue Maximum possible value for the setting
         * @return Validated value of the attribute.
         * @rtype Type of the value.
         */
        template<class TType, class TSettings>
        inline TType ValidateKey(
            TSettings& aSettings,
            const TUint aAttributeKey,
            const TInt aMinValue,
            const TInt aMaxValue );

    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data

        /** Control flags */
        enum TFlags
            {
            // Validating has caused settings to be changed
            ESettingsHaveChanged = 0
            };

        //
        CImumInternalApiImpl&   iInternalApi;
        // Owned: For access point validation
        CMsvCommDbUtilities*    iCommDbUtils;
        // Flags to control the behaviour
        TMuiuFlags              iFlags;
    };

#include "ImumInSettingsDataValidator.inl"

#endif //  CIMUMINSETTINGSDATAVALIDATOR_H

// End of File

