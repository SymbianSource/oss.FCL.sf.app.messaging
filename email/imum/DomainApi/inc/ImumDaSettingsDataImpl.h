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
* Description:  ImumDaSettingsDataImpl.h
 *
*/


#ifndef C_IMUMDASETTINGSDATAIMPL_H
#define C_IMUMDASETTINGSDATAIMPL_H

#include "ImumDaSettingsData.h"
#include "ImumInternalApi.h"
#include "ImumDaSettingsDataCollectionImpl.h"

class CImumDaMailboxServicesImpl;
class MImumInSettingsDataCollection;
class CImumInSettingsData;

typedef CArrayPtrFlat<CImumDaSettingsDataCollectionImpl> CImumDaConnSettings;

NONSHARABLE_CLASS( CImumDaSettingsDataImpl ) :
    public CImumDaSettingsData
    {
    public:  // Constructors and destructor

        /**
         * Create object from CImumDaSettingsDataImpl
         *
         * @since S60 3.2
         * @param aMailboxApi, Reference to internal api.
         * @param aProtocol, Any receiving protocol: Imap4 or Pop3.
         * @return, Constructed object
         */
        static CImumDaSettingsDataImpl* NewL(
            CImumInternalApi& aMailboxApi,
            const TUid& aProtocol );

        /**
         * Creates object from CImumDaSettingsDataImpl and leaves it
         * to cleanup stack
         *
         * @since S60 3.2
         * @param aMailboxApi, Reference to internal api.
         * @param aProtocol, Any receiving protocol: Imap4 or Pop3.
         * @return, Constructed object
         */
        static CImumDaSettingsDataImpl* NewLC(
            CImumInternalApi& aMailboxApi,
            const TUid& aProtocol );

        /**
         * Destructor
         * @since S60 3.2
         */
        virtual ~CImumDaSettingsDataImpl();

    protected:  // Constructors

        /**
         * Default constructor for CImumDaSettingsDataImpl
         *
         * @since S60 3.2
         * @param aMailboxApi, Reference to internal api.
         * @return, Constructed object
         */
        CImumDaSettingsDataImpl( CImumInternalApi& aMailboxApi );

        /**
        * Symbian 2-phase constructor
        * @since S60 3.2
        */
        void ConstructL( const TUid& aProtocol );

    private:

        /**
         * Throws an exception as leave (release) or panic (debug)
         *
         * @since S60 v3.2
         * @param aErrorCode Error code that is thrown.
         * @leave aErrorCode
         */
        void SettingsDataExceptionL( const TInt aErrorCode ) const;

    public:  // From class CImumDaSettingsData

        /**
         * Creates connection data set to settings.
         *
         * @since S60 v3.2
         * @param aProtocol, The protocol of the connection settings
         * @return Reference to created connection settings.
         */
        MImumDaSettingsDataCollection& AddSetL( const TUid& aProtocol );

        /**
         * Removes the connection data set from the settings data.
         *
         * @since S60 v3.2
         * @param aIndex, Index to connection settings to be removed
         * @param aSettingsDataCollection, Connection settings to be removed
         * @leave EImumDaConnectionSetNotExist, if the item does not exist
         */
        void DelSetL( const TUint aIndex ) ;
        void DelSetL( const MImumDaSettingsDataCollection& aSettingsDataCollection );

        /**
         * Gets an access to connection data at position.
         *
         * @since S60 v3.2
         * @param aIndex, Index to connection settings data.
         * @return Reference to connection settings at current index.
         */
        MImumDaSettingsDataCollection& GetSetL( const TUint aIndex ) const;

        /**
         * Validates the settings data.
         *
         * @since S60 v3.2
         * @return Result of the validation operation.
         */
        TInt Validate();

        /**
         * Sets the given value/text to the key
         *
         * @since S60 v3.2
         * @param aAttributeKey, Id of the setting key
         * @param aValue/aText, Data to be stored
         * @return EImumDaNoError, when successful
         */
        TInt SetAttr( const TUint aAttributeKey, const TInt aValue );
        TInt SetAttr( const TUint aAttributeKey, const TInt64 aValue );
        TInt SetAttr( const TUint aAttributeKey, const TMsvId aValue );
        TInt SetAttr( const TUint aAttributeKey, const TDesC& aText );
        TInt SetAttr( const TUint aAttributeKey, const TDesC8& aText );

        /**
         * Gets the required value/text from the key
         *
         * @since S60 v3.2
         * @param aAttributeKey, Value of the setting key
         * @param aValue/aText, Data to be stored
         * @return EImumDaNoError, when successful
         */
        TInt GetAttr( const TUint aAttributeKey, TInt& aValue ) const;
        TInt GetAttr( const TUint aAttributeKey, TInt64& aValue ) const;
        TInt GetAttr( const TUint aAttributeKey, TMsvId& aId ) const;
        TInt GetAttr( const TUint aAttributeKey, TDes& aText ) const;
        TInt GetAttr( const TUint aAttributeKey, TDes8& aText ) const;

        /**
         * Compares the content of the datas
         *
         * @since S60 v3.2
         * @param aSettingsData
         * @return Result of the comparasion
         */
        TBool operator!=(
            const CImumDaSettingsData& aSettingsData ) const;
        TBool operator==(
            const CImumDaSettingsData& aSettingsData ) const;

        /**
         * Gets an access to connection data at position.
         *
         * @since S60 v3.2
         * @param aIndex, Index to connection settings data.
         * @return Reference to connection settings at current index.
         */
        MImumDaSettingsDataCollection& operator[]( const TUint aIndex ) const;

        /**
         * Resets the default values to most of the settings
         *
         * This method sets the default value to all settings in the class,
         * except those, that would invalidate the settings.
         * Settings that are not resetted are:
         *      Server address, email address, mailbox name and protocol
         *
         * @since S60 v3.2
         */
        void ResetAll();

        /**
         * Resets the default value of the single key
         *
         * @since S60 v3.2
         * @param aAttributeKey, Id of the setting key
         * @return EImumDaNoError, when successful
         */
        void Reset( const TUint aAttributeKey );

        /**
         * Copies the content of the setting data
         *
         * @since S60 v3.2
         * @param aSettingsData Collection data object to be copied
         * @return EImumDaNoError, when successful
         */
        TInt Copy( const CImumDaSettingsData& aSettingsData );

        /**
         *
         * @since S60 3.2
         * @param
         * @return reference to internal data settigns.
         * @leave &(leavedesc)s
         */
        CImumInSettingsData& GetInData();

    private:

        // Reference to internal Api
        CImumInternalApi& iMailboxApi;
        // owned
        CImumInSettingsData* iData;
        // Owned: Domain api connection settings
        CImumDaConnSettings* iDaConnectionSettings;
    };



#endif /* C_IMUMDASETTINGSDATAIMPL_H */
