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
* Description:  ImumDaSettingsDataCollectionImpl.h
 *
*/


#ifndef C_IMUMDASETTINGSDATACOLLECTIONIMPL_H
#define C_IMUMDASETTINGSDATACOLLECTIONIMPL_H

#include <e32base.h>
#include <msvstd.h>
#include "ImumDaSettingsDataCollection.h"

class CImumInternalApi;
class MImumInSettingsDataCollection;

NONSHARABLE_CLASS( CImumDaSettingsDataCollectionImpl ) :
    public MImumDaSettingsDataCollection
    {
    public: // Constructors and destructor

        /**
         * Create object from CImumDaSettingsDataCollectionImpl
         *
         * @since S60 3.2
         * @param aInDataCollection, Reference to internal api data collection.
         * @return, Constructed object
         */
        static CImumDaSettingsDataCollectionImpl* NewL(
            MImumInSettingsDataCollection& aInDataCollection );

        /**
         * Creates object from CImumDaSettingsDataCollectionImpl and
         * leaves it to cleanup stack
         *
         * @since S60 3.2
         * @param aInDataCollection, Reference to internal api data collection.
         * @return, Constructed object
         */
        static CImumDaSettingsDataCollectionImpl* NewLC(
            MImumInSettingsDataCollection& aInDataCollection );

        /**
         * Destructor
         * @since S60 3.2
         */
        virtual ~CImumDaSettingsDataCollectionImpl();

    private: // Constructors

        /**
         * Default constructor for CImumDaSettingsDataCollectionImpl
         *
         * @since S60 3.2
         * @param aInDataCollection, Reference to internal api data collection.
         * @return, Constructed object
         */
        CImumDaSettingsDataCollectionImpl(
            MImumInSettingsDataCollection& aInDataCollection );

        /**
         * Symbian 2-phase constructor
         * @since S60 3.2
         */
        void ConstructL();

    public:  //From class MImumDaSettingsDataCollection

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
            const MImumDaSettingsDataCollection& aSettingsDataCollection )
                const;

        TBool operator==(
            const MImumDaSettingsDataCollection& aSettingsDataCollection )
                const;

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
        TInt Copy(
            const MImumDaSettingsDataCollection& aSettingsDataCollection );

    public:
        /**
         *
         * @since S60 3.2
         * @param aAttributeKey, Attribute key to be tested.
         * @param aReadOperation, Chooses how attribute is tested. read(1) or write(0).
         * @return
         */
        static TInt IsLegalKey(
            const TUint aAttributeKey,
            TBool aReadOperation );

    private:

        //Reference to internal api data collection
        MImumInSettingsDataCollection& iInDataCollection;
    };



#endif /* C_IMUMDASETTINGSDATACOLLECTIONIMPL_H */
