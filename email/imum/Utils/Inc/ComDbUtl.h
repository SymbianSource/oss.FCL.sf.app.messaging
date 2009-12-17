/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ComDbUtl.h
*
*
*/




#ifndef __COMDBUTL_H__
#define __COMDBUTL_H__

// INCLUDE FILES
#include "ImumMboxDefaultData.h"			// CImumMboxDefaultData
#include "IMSSettingsItemAccessPoint.h"     // CIMSSettingsAccessPointItem


#include <badesca.h>
#include <coemain.h>
#include <muiuflagger.h>                    // CMuiuFlags
#include <cmmanager.h>
#include <cmapplicationsettingsui.h>


const TUint32 KServiceIdNoServiceSpecified = 0U;

// CONSTANTS
enum TIMASCommsDbUtilsIapArray
	{
    EIMASCommsDbIapAlwaysAsk = 0
	};


/**
*  CommsDb related utility functions
*
*  @lib imumutils.lib
*  @since S60 3.0
*/
class CMsvCommDbUtilities : public CBase
    {
    public: // Constructors & destructor

        /**
        * NewLC
        * @return CMsvCommDbUtilities*, self pointer
        */
        static CMsvCommDbUtilities* NewLC();

        /**
        * NewL
        * @return CMsvCommDbUtilities*, self pointer
        */
        static CMsvCommDbUtilities* NewL();

        /**
        * ~CMsvCommDbUtilities
        * Destructor
        */
        ~CMsvCommDbUtilities();

    public: // New Functions

        /**
        * IsApBearerCircuitSwitchedL
        * Checks if AP bearer is circuit switched
        * @param TUint32, AP id
        * @return TBool, ETrue if AP bearer is circuit switched
        */
        TBool IsApBearerCircuitSwitchedL( TUint32 aApId );

        /**
         * Fills IAP Array using the given bearer filter
         *
         * @param aFillArray Array where IAPs are filled into
         * @since S60 3.0
         */
        void FillCustomIapArrayL( CDesCArrayFlat& aFillArray );


        /**
        * Gets the radiobutton value
        *
        * @param aIapItem IAP item of which radio button
        * 				  index is wanted
        * @return TInt The index of the IAP in IAP list
        * @since S60 3.0
        */
        TInt GetWizardRadioButton( CIMSSettingsAccessPointItem& aIapItem );

        /**
        * Returns the name string of the given IAP
        *
        * @param aIapItem IAP item of which name string is wanted
        * @return TDesC* A pointer to the name
        * @since S60 3.0
        */
        const TDesC* SettingsItemIapStringL( CIMSSettingsAccessPointItem& aIapItem );

        /**
         * Gets an access points index in IAP array
         *
         * @param aId IAP item of which radio button
         * 			  index is wanted
         * @return TInt The index of IAP
         * @since S60 5.0
         */
        TInt GetIapIndexL( const TUint32 aId );


        /**
        * Launches the IAP selection page using Connection
        * Method Manager.
        *
        * @param aIapItem On success the selected iap information
        * 				  is set in this
        * @return TInt Return value of IAP selection, KErrNone or
        * 			   KErrCancel if selection was cancelled.
        * @since S60 3.0
        */
        TInt LaunchIapPageL( CIMSSettingsAccessPointItem& aIapItem );

        /**
        * Initializes the IAP array
        *
        * @since S60 3.0
        */
        void InitializeSelectionL();

        /**
        * Initializes access point item
        *
        * @param aIapItem Reference to where iap information is filled
        * @param aIapId
        * @since S60 3.0
        */
        void InitItemAccessPointL( CIMSSettingsAccessPointItem& aIapItem,
        						   const TBool aIsWizard );


        /**
         * Loads resource string for IAP item
         *
         * @param aIapItem IAP item for which resource is to be loaded
         * @param aResource Id of the resouce to be loaded
         * @since S60 3.0
         */
        void ItemLoadBufferForItemL( CIMSSettingsAccessPointItem& aIapItem,
        							 const TInt aResource );

        /**
        * Checks if the given IAP is an WLAN access point
        *
        * @param aIap An IAP id
        * @return TBool ETrue if given IAP is of WLAN type
        * @since S60 3.0
        */
        TBool IsWlanAccessPointL( TUint32 aIap ) const;

        /**
        * Determines IMAP4 buffer size for given IAP
        *
        * @param aIap An IAP id
        * @param aDefaultData Data from where buffer size
        * 					  is gotten
        * @return TInt The buffer size
        * @since S60 3.0
        */
        TInt DetermineImap4BufferSize( const TUint32 aIap,
        							   const CImumMboxDefaultData& aDefaultData ) const;

        /**
        * Gets the bearer type of a given access point
        * @param aUid The UID of the access point.
        * @return The type of the bearer.
        * @since S60 3.2
        */
        TUint32 BearerTypeL( TUint32 aUid ) const;


    	/*
    	 * Returns the amount of static IAPs, such as
    	 * Default Connection and WLAN access point
    	 */
    	TInt StaticIAPCount();

        /**
         * Gets an access point id from IAP array
         *
         * @param aSelectionIndex a radiobutton of the IAP that is selected
         * @return TInt The ID of IAP
         * @since S60 5.0
         */
        TInt GetIapIdL( const TInt32 aSelectionIndex );



    private:

        /**
        * ConstructL
        * Second phase constructor
        */
        void ConstructL();

        /**
        * Constructor
        * @since S60 3.0
        */
        CMsvCommDbUtilities();


    private:

        RCmManager      iCmManager;
        TBool           iFeatureAllowVPN;
        CMuiuFlags*     iFlags;
        RArray<TUint32> iIapList;
        TBearerFilterArray iFilters;
        TBool           iInitialized;
    };


#endif
