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
* Description:  Interface class of settings data
 *
*/



#ifndef C_IMUMDASETTINGSDATA_H
#define C_IMUMDASETTINGSDATA_H

#include <msvstd.h>                     // TMsvId
#include <ImumDaSettingsKeys.h>         // Settings data keys

class MImumDaSettingsDataCollection;

/**
 * Settings data class for all mailbox settings
 *
 * This is an interface to mailbox settings data. To be able to create
 * a working mailbox, this class has to be filled with valid settings.
 * Valid settings contains:
 *  - Connection settings for incoming (imap4/pop3) settings
 *      - Incoming protocol must be specified
 *      - Incoming server must be specified
 *  - Connection settings for outgoing (smtp) settings
 *      - Outgoing protocol must be specified
 *      - Outgoing server must be specified
 *      - Email address must be specified
 *  - Name for the mailbox
 *
 * @lib imumda.lib
 * @since S60 v3.2
 * @see from ImumDaSettingsKeys.h enum TConnectionKeySet for setting keys
 * @see from ImumDaErrorCodes.h enum TImumDaErrorCode for error codes
 * @see from ImumDaSettingsDataCollection.h class
 *           CImumDaSettingsDataCollection for connection data class
 */
NONSHARABLE_CLASS( CImumDaSettingsData ) : public CBase
    {
    public:

        /**
         * Creates connection data set to settings.
         *
         * @since S60 v3.2
         * @param aProtocol, The protocol of the connection settings
         * @return Reference to created connection settings.
         */
         virtual MImumDaSettingsDataCollection& AddSetL(
            const TUid& aProtocol ) = 0;

        /**
         * Removes the connection data set from the settings data.
         *
         * @since S60 v3.2
         * @param aIndex, Index to connection settings to be removed
         * @param aSettingsDataCollection, Connection settings to be removed
         * @leave EImumDaConnectionSetNotExist, if the item does not exist
         */
         virtual void DelSetL( const TUint aIndex ) = 0;

         virtual void DelSetL(
            const MImumDaSettingsDataCollection& aSettingsDataCollection ) = 0;

        /**
         * Gets an access to connection data at position.
         *
         * @since S60 v3.2
         * @param aIndex, Index to connection settings data.
         * @return Reference to connection settings at current index.
         */
         virtual MImumDaSettingsDataCollection& GetSetL(
            const TUint aIndex ) const = 0;

        /**
         * Validates the settings data.
         *
         * @since S60 v3.2
         * @return Result of the validation operation.
         */
         virtual TInt Validate() = 0;

        /**
         * Sets the given value/text to the key
         *
         * @since S60 v3.2
         * @param aAttributeKey, Id of the setting key
         * @param aValue/aText, Data to be stored
         * @return EImumDaNoError, when successful
         */
        virtual TInt SetAttr(
            const TUint aAttributeKey,
            const TInt aValue ) = 0;

        virtual TInt SetAttr(
            const TUint aAttributeKey,
            const TInt64 aValue ) = 0;

        virtual TInt SetAttr(
            const TUint aAttributeKey,
            const TMsvId aValue ) = 0;

        virtual TInt SetAttr(
            const TUint aAttributeKey,
            const TDesC& aText ) = 0;

        virtual TInt SetAttr(
            const TUint aAttributeKey,
            const TDesC8& aText ) = 0;

        /**
         * Gets the required value/text from the key
         *
         * @since S60 v3.2
         * @param aAttributeKey, Value of the setting key
         * @param aValue/aText, Data to be stored
         * @return EImumDaNoError, when successful
         */
        virtual TInt GetAttr(
            const TUint aAttributeKey,
            TInt& aValue ) const = 0;

        virtual TInt GetAttr(
            const TUint aAttributeKey,
            TInt64& aValue ) const = 0;

        virtual TInt GetAttr(
            const TUint aAttributeKey,
            TMsvId& aId ) const = 0;

        virtual TInt GetAttr(
            const TUint aAttributeKey,
            TDes& aText ) const = 0;

        virtual TInt GetAttr(
            const TUint aAttributeKey,
            TDes8& aText ) const = 0;


        /**
         * Compares the content of the datas
         *
         * @since S60 v3.2
         * @param aSettingsData
         * @return Result of the comparasion
         */
        virtual TBool operator!=(
            const CImumDaSettingsData& aSettingsData ) const = 0;

        virtual TBool operator==(
            const CImumDaSettingsData& aSettingsData ) const = 0;

        /**
         * Gets an access to connection data at position.
         *
         * @since S60 v3.2
         * @param aIndex, Index to connection settings data.
         * @return Reference to connection settings at current index.
         */
         virtual MImumDaSettingsDataCollection& operator[](
            const TUint aIndex ) const = 0;

        /**
         * Resets the default values to most of the settings
         *
         * @since S60 v3.2
         * This method sets the default value to all settings in the class
         */
        virtual void ResetAll() = 0;

        /**
         * Resets the default value of the single key
         *
         * @since S60 v3.2
         * @param aAttributeKey, Id of the setting key
         */
        virtual void Reset( const TUint aAttributeKey ) = 0;

        /**
         * Copies the content of the setting data
         *
         * @since S60 v3.2
         * @param aSettingsData Collection data object to be copied
         * @return EImumDaNoError, when successful
         */
        virtual TInt Copy(
            const CImumDaSettingsData& aSettingsData ) = 0;

    };

#endif // C_IMUMDASETTINGSDATA_H
