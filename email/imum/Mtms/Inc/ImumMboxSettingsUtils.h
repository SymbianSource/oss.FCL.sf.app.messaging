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
* Description: ImumMboxSettingsUtils.h
*
*/


#ifndef CIMUMMBOXSETTINGSUTILS_H
#define CIMUMMBOXSETTINGSUTILS_H

// INCLUDES
#include <e32base.h>
#include <ImumInternalApi.h>        // CImumInternalApi
#include <ImumInSettingsData.h>     // CImumInSettingsData
#include <ImumInSettingsDataCollection.h> // CImumInSettingsDataCollection
#include <miutset.h>                // CImBaseEmailSettings
#include <imapset.h>                // CImImap4Settings
#include <pop3set.h>                // CImPop3Settings
#include <smtpset.h>                // CImSmtpSettings

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*
*  @lib
*  @since Series60 3.0
*/
class ImumMboxSettingsUtils
    {
    public:  // Constructors and destructor
    public: // New functions

        /**
         *
         * @since S60 3.2
         */
        static void SetLoginInformationL(
            CImumInternalApi& aMailboxApi,
            const TDesC8& aUserName,
            const TDesC8& aPassword,
            const TBool aPasswordTemporary,
            const TMsvId aServiceId );

        /**
         *
         * @since S60 3.2
         */
        static void GetLoginInformationL(
            CImumInternalApi& aMailboxApi,
            TDes8& aUserName,
            TDes8& aPassword,
            TBool& aPasswordTemporary,
            const TMsvId aServiceId );

        /**
         * Get login information from settings. This is a performance optimization
         * to avoid loading settings if they are available already.
         * @since S60 3.2
         */
        static void GetLoginInformationL(
            const CImumInSettingsData& aAccountSettings,
            TUid aMtmUid,
            TDes8& aUserName,
            TDes8& aPassword,
            TBool& aPasswordTemporary );

        /**
         *
         *
         * @since S60 3.2
         * @param aEmailApi
         * @param aMailboxId
         * @param aAttributeKey
         * @param aConnectionIndex
         * @return
         */
        template<class T>
        inline static T QuickLoadL(
            CImumInternalApi& aEmailApi,
            const TMsvId aMailboxId,
            const TUint aAttributeKey,
            const TInt aConnectionIndex = KErrNotFound );

        /**
         *
         * @since S60 3.2
         * @param aSettings
         * @param aAttributeKey
         * @return
         */
        template<class T>
        inline static T QuickGetL(
            const CImumInSettingsData& aSettings,
            const TUint aAttributeKey,
            const TInt aConnectionIndex = KErrNotFound );

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        static TBool IsImap4(
            const CImumInSettingsData& aSettings );

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        static TBool ValidateSettingsL(
            CImumInternalApi& aMailboxApi,
            const TMsvId aMailboxId );

        /**
         * Updates last connect info for mailbox
         *
         * @since S60 v3.2
         * @param aMailboxId: id of the mailbox
         * @param aSuccessful: ETrue, if successfully connected, EFalse otherwise
         */
        static void SetLastUpdateInfoL(
            CImumInternalApi& aEmailApi,
            TMsvId aMailboxId,
            const TBool& aSuccessful );

    public: // Functions from base classes

    protected:  // Constructors
    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions
    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data
    };

#include "ImumMboxSettingsUtils.inl"

#endif //  CIMUMMBOXSETTINGSUTILS_H

// End of File
