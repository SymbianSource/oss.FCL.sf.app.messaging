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
* Description:  Interface class of settings data collection
 *
*/

 
#ifndef M_IMUMDASETTINGSDATACOLLECTION_H 
#define M_IMUMDASETTINGSDATACOLLECTION_H 

#include <msvstd.h>                     // TMsvId
#include <ImumDaSettingsKeys.h>         // Settings data keys

class MImumDaSettingsDataCollection;

/**
 * Settings data collection for connection settings
 *
 * This is an interface to settings data collection. This class is used with
 * CImumDaSettingsData interface, which always owns any instance of this class.
 *
 * @lib imumda.lib
 * @since S60 v3.2
 * @see from ImumDaSettingsKeys.h enum TConnectionKeySet for setting keys
 * @see from ImumDaErrorCodes.h enum TImumDaErrorCode for error codes
 * @see from ImumDaSettingsData.h class CImumDaSettingsData for data class
 */
NONSHARABLE_CLASS( MImumDaSettingsDataCollection )
    {
    public:
    
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
            const MImumDaSettingsDataCollection& aSettingsDataCollection ) 
                const = 0;
            
        virtual TBool operator==( 
            const MImumDaSettingsDataCollection& aSettingsDataCollection ) 
                const = 0;
        
        /**
         * Resets the default value of the single key
         *
         * @since S60 v3.2
         * @param aAttributeKey, Id of the setting key
         * @return EImumDaNoError, when successful
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
            const MImumDaSettingsDataCollection& aSettingsDataCollection ) = 0;
    };

#endif // M_IMUMDASETTINGSDATACOLLECTION_H

