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
* Description:  ImumInSettingsDataImpl.h
*
*/


#ifndef CIMUMINSETTINGSDATAIMPL_H
#define CIMUMINSETTINGSDATAIMPL_H

// INCLUDES
#include <e32base.h>
#include <ImumInSettingsData.h>
#include "ImumInSettingsDataCollectionImpl.h"
#include "ImumInSettingsKeys.h"
#include "ImumDaSettingsKeys.h"

// CONSTANTS
const TInt KImumInConnSettingsGranularity = 2;

/** */
typedef CArrayPtrFlat<CImumInSettingsDataCollectionImpl> CImumInConnSettings;

class CImumMboxDefaultData;
class CImumInSettingsDataArray;
class CImumInternalApiImpl;
class CImumInSettingsDataValidator;
class CImumMboxDefaultDataCtrl;

/**
*
*  @lib
*  @since S60 3.2
*/
class CImumInSettingsDataImpl : public CImumInSettingsData
    {
    public:  // Constructors and destructor

        /**
         * Creates object from CImumInSettingsDataImpl and leaves it to cleanup stack
         *
         * @since S60 3.2
         * @return, Constructed object
         */
        static CImumInSettingsDataImpl* NewLC(
            CImumInternalApiImpl& aMailboxApi,
            const TUid& aProtocol,
            const CImumMboxDefaultData* aDefaultData = NULL );

        /**
         * Create object from CImumInSettingsDataImpl
         *
         * @since S60 3.2
         * @return, Constructed object
         */
        static CImumInSettingsDataImpl* NewL(
            CImumInternalApiImpl& aMailboxApi,
            const TUid& aProtocol,
            const CImumMboxDefaultData* aDefaultData = NULL );

        /**
         * Destructor
         *
         * @since S60 3.2
         */
        virtual ~CImumInSettingsDataImpl();

    public: // New functions

        /**
         * Returns the number of connection settings objects.
         *
         * @since S60 v3.2
         * @return Current number of settings data objects.
         */
        TInt ConnectionSettingsCount();

    public: // Functions from base classes

        /**
         * Adds new connection settings data object.
         *
         * @since S60 3.2
         * @param
         * @return
         */
        virtual MImumDaSettingsDataCollection& AddSetL(
            const TUid& aProtocol );

        /**
         * Adds new connection settings data object.
         *
         * @since S60 3.2
         * @param
         * @return
         */
        virtual MImumInSettingsDataCollection& AddInSetL(
            const TUid& aProtocol );

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        void DelSetL( const TUint aIndex );

        /**
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        void DelSetL(
            const MImumDaSettingsDataCollection& aSettingsDataCollection );

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        MImumDaSettingsDataCollection& GetSetL( const TUint aIndex ) const;

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        MImumInSettingsDataCollection& GetInSetL( const TUint aIndex ) const;

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        virtual TInt Validate();

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        TInt SetAttr( const TUint aAttributeKey, const TInt aValue );

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        TInt SetAttr( const TUint aAttributeKey, const TInt64 aValue );

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        TInt SetAttr( const TUint aAttributeKey, const TMsvId aId );

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        TInt SetAttr( const TUint aAttributeKey, const TDesC& aText );

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        TInt SetAttr( const TUint aAttributeKey, const TDesC8& aText );

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        TInt GetAttr( const TUint aAttributeKey, TInt& aValue ) const;

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        TInt GetAttr( const TUint aAttributeKey, TInt64& aValue ) const;

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        TInt GetAttr( const TUint aAttributeKey, TMsvId& aId ) const;

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        TInt GetAttr( const TUint aAttributeKey, TDes& aText ) const;

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        TInt GetAttr( const TUint aAttributeKey, TDes8& aText ) const;

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        TBool operator!=( const CImumDaSettingsData& aSettingsData ) const;

        /**
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        TBool operator!=( const CImumInSettingsData& aSettingsData ) const;

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        TBool operator==( const CImumDaSettingsData& aSettingsData ) const;

        /**
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        TBool operator==( const CImumInSettingsData& aSettingsData ) const;

        /**
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        MImumDaSettingsDataCollection& operator[](
            const TUint aIndex ) const;

        /**
         *
         * @since S60 3.2
         * @return
         */
        virtual void Reset( const TUint aAttributeKey );

        /**
         *
         * @since S60 3.2
         */
        virtual void ResetAll();

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        TInt Copy( const CImumDaSettingsData& aSettingsData );

        /**
         *
         * @since S60 3.2
         * @param
         * @return
         */
        TInt Copy( const CImumInSettingsData& aSettingsData );

        /**
         * Prints out the data in the object. Debug Only.
         *
         * @since S60 3.2
         */
        virtual void Log() const;

    protected:  // Constructors

        /**
         * Default constructor for classCImumInSettingsDataImpl
         *
         * @since S60 3.2
         * @return, Constructed object
         */
        CImumInSettingsDataImpl(
            CImumInternalApiImpl& aMailboxApi,
            const CImumMboxDefaultData* aDefaultData = NULL );

        /**
         * Symbian 2-phase constructor
         *
         * @since S60 3.2
         */
        virtual void ConstructL(
            const TUid& aProtocol );

    protected:  // New virtual functions
    protected:  // New functions

        /**
         * Returns protocol specific default data
         *
         * @since S60 v3.2
         * @param aProtocol Protocol, of which default settings are requested
         * @return Object to default data
         */
        const CImumInSettingsDataCollectionImpl* SelectDefaultData(
            const TUid& aProtocol,
            const CImumMboxDefaultData* aDefaultData ) const;

        /**
         * Throws an exception as leave (release) or panic (debug)
         *
         * @since S60 v3.2
         * @param aErrorCode Error code that is thrown.
         * @leave aErrorCode
         */
        void SettingsDataExceptionL( const TInt aErrorCode ) const;

    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions

        /**
         * Creates validator class and returns reference to it.
         *
         * @since S60 v3.2
         * @return
         */
        CImumInSettingsDataValidator& ValidatorL();

        /**
         * Analyzes and corrects problems in the data.
         *
         * @since S60 v3.2
         */
        void ValidateL();

    private:  // Functions from base classes

        /**
         * Creates an identical object of the item.
         *
         * @since S60 v3.2
         * @return A new cloned object
         */
        virtual CImumInSettingsData* CloneL() const;

    public:     // Data
    protected:  // Data

        // Owned: Connection settings
        CImumInConnSettings*            iConnectionSettings;
        // Owned: Settings
        CImumInSettingsDataArray*       iSettings;
        // Owned: Settings validator
        CImumInSettingsDataValidator*   iValidator;
        // Not owned: Default settings data object
        const CImumMboxDefaultData*     iDefaultData;
        // Not owned: Mailbox API reference
        CImumInternalApiImpl&           iMailboxApi;

    private:    // Data
    };

#endif //  CIMUMINSETTINGSDATAIMPL_H

// End of File


