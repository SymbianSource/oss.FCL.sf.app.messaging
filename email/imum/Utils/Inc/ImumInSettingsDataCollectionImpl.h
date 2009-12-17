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
* Description:  ImumInSettingsDataCollectionImpl.h
*
*/


#ifndef CIMUMINSETTINGSDATACOLLECTIONIMPL_H
#define CIMUMINSETTINGSDATACOLLECTIONIMPL_H

// INCLUDES
#include <e32base.h>
#include <ImumInSettingsDataCollection.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
class CImumInSettingsDataArray;
class CImumMboxDefaultData;
class CImumInSettingsDataValidator;

// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*
*  @lib
*  @since S60 3.2
*/
class CImumInSettingsDataCollectionImpl :
    public CBase,
    public MImumInSettingsDataCollection
    {
    public:  // Constructors and destructor

        /**
        * Creates object from CImumInSettingsDataCollectionImpl and leaves it to cleanup stack
        * @since S60 3.2
        * @return, Constructed object
        */
        static CImumInSettingsDataCollectionImpl* NewLC(
            const TUid& aProtocol,
            const MImumInSettingsDataCollection* aDefaultData,
            CImumInSettingsDataValidator& aValidator );

        /**
        * Create object from CImumInSettingsDataCollectionImpl
        * @since S60 3.2
        * @return, Constructed object
        */
        static CImumInSettingsDataCollectionImpl* NewL(
            const TUid& aProtocol,
            const MImumInSettingsDataCollection* aDefaultData,
            CImumInSettingsDataValidator& aValidator );

        /**
        * Destructor
        * @since S60 3.2
        */
        virtual ~CImumInSettingsDataCollectionImpl();

    public: // New functions

        /**
         *
         *
         * @since S60 v3.2
         * @leave &(leavedesc)s
         */
        void ValidateL();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        void ResetAll();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         */
        void Log();

        /**
         * Makes identical copy of the object and returns the implementation
         *
         * @since S60 v3.2
         * @return Clone of implementation object
         * @leave
         */
        CImumInSettingsDataCollectionImpl* DoCloneL() const;

    public: // Functions from base classes

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
         * @param aSettingsData
         * @return
         */
        TBool operator!=( const MImumDaSettingsDataCollection& aConnectionSettings ) const;
        TBool operator!=( const MImumInSettingsDataCollection& aConnectionSettings ) const;

        /**
         *
         * @since S60 3.2
         * @param aSettingsData
         * @return
         */
        TBool operator==( const MImumDaSettingsDataCollection& aConnectionSettings ) const;
        TBool operator==( const MImumInSettingsDataCollection& aConnectionSettings ) const;

        /**
         *
         * @since S60 3.2
         * @param aAttributeKey
         * @return
         */
        void Reset( const TUint aAttributeKey );

        /**
         *
         * @since S60 3.2
         * @param aSettingsData
         * @return
         */
        TInt Copy( const MImumDaSettingsDataCollection& aConnectionSettings );
        TInt Copy( const MImumInSettingsDataCollection& aConnectionSettings );

        /**
         * Makes identical copy of the object and returns the API class.
         *
         * @since S60 v3.2
         * @return Clone of implementation object
         * @leave
         */
        MImumInSettingsDataCollection* CloneL() const;

    protected:  // Constructors

        /**
        * Default constructor for classCImumInSettingsDataCollectionImpl
        * @since S60 3.2
        * @return, Constructed object
        */
        CImumInSettingsDataCollectionImpl(
            const TUid& aProtocol,
            const MImumInSettingsDataCollection* aDefaultData,
            CImumInSettingsDataValidator& aValidator );

        /**
        * Symbian 2-phase constructor
        * @since S60 3.2
        */
        void ConstructL(
            const TUid& aProtocol );

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
         * @leave &(leavedesc)s
         */
        void SettingsDataCollectionExceptionL(
            const TInt aErrorCode ) const;

    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data

        //
        const MImumInSettingsDataCollection* iDefaultData;
        //
        TUid                            iProtocol;
        //
        CImumInSettingsDataValidator&   iValidator;
        // Owned:
        CImumInSettingsDataArray*       iSettings;
    };

#include "ImumInSettingsDataCollectionImpl.inl"

#endif //  CIMUMINSETTINGSDATACOLLECTIONIMPL_H

// End of File
