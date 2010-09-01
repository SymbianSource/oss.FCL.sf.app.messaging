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
* Description: 
 *       Class implementation file
 *
*/


// INCLUDE FILES
#include "ComDbUtl.h"
#include "EmailFeatureUtils.h"
#include "IMSSettingsDialog.h"
#include "ImumUtilsLogging.h"
#include "ImumPanic.h"

#include <badesca.h>                // CDesCArrayFlat
#include <StringLoader.h>           // StringLoader
#include <imapset.h>                // KImapDefaultFetchSizeBytes
#include <messagingvariant.hrh>
#include <muiu_internal.rsg>
#include <ImumUtils.rsg>
#include <imum.rsg>
#include <cmconnectionmethoddef.h>
#include <cmdestination.h>

#include <cmapplicationsettingsui.h>
#include <cmpluginpacketdatadef.h>
#include <cmpluginwlandef.h>
#include <cmplugincsddef.h>
#include <cmpluginhscsddef.h>
#include <cmpluginlanbasedef.h>
#include <cmpluginvpndef.h>


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::CMsvCommDbUtilities()
// ----------------------------------------------------------------------------
//
CMsvCommDbUtilities::CMsvCommDbUtilities()
    {
    IMUM_CONTEXT( CMsvCommDbUtilities::CMsvCommDbUtilities, 0, KLogUi );
    }

// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::NewLC()
// ----------------------------------------------------------------------------
//
CMsvCommDbUtilities* CMsvCommDbUtilities::NewLC()
    {
    IMUM_STATIC_CONTEXT( CMsvCommDbUtilities::NewL, 0, utils, KLogUi );

    CMsvCommDbUtilities* self = new ( ELeave ) CMsvCommDbUtilities;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::NewL()
// ----------------------------------------------------------------------------
//
CMsvCommDbUtilities* CMsvCommDbUtilities::NewL()
    {
    IMUM_STATIC_CONTEXT( CMsvCommDbUtilities::NewL, 0, utils, KLogUi );

    CMsvCommDbUtilities* self = NewLC();
    CleanupStack::Pop(); // self
    return self;
    }

// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::~CMsvCommDbUtilities()
// ----------------------------------------------------------------------------
//
CMsvCommDbUtilities::~CMsvCommDbUtilities()
    {
    IMUM_CONTEXT( CMsvCommDbUtilities::~CMsvCommDbUtilities, 0, KLogUi );
    delete iFlags;
    iFilters.Close();
    iIapList.Close();
    iCmManager.Close();
    }

// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::ConstructL()
// ----------------------------------------------------------------------------
//
void CMsvCommDbUtilities::ConstructL()
    {
    IMUM_CONTEXT( CMsvCommDbUtilities::ConstructL, 0, KLogUi );

    iInitialized = EFalse;
    iFlags = MsvEmailMtmUiFeatureUtils::EmailFeaturesL( ETrue, ETrue );
    iFeatureAllowVPN = MsvEmailMtmUiFeatureUtils::LocalFeatureL(
            		   	KCRUidMuiuVariation, KMuiuEmailConfigFlags,
            		   	KEmailFeatureIdEmailVPNAllowed );

    // Fill IAP Filter array
    iFilters.Append( KUidPacketDataBearerType );
    iFilters.Append( KUidLanBearerType );
    if ( iFlags->GF( EMailFeatureCsd ) )
        {
        iFilters.Append( KUidCSDBearerType );
        iFilters.Append( KUidHSCSDBearerType );
        }
    if ( iFlags->GF( EMailFeatureProtocolWlan ) )
        {
        iFilters.Append( KUidWlanBearerType );
        }
    if( iFeatureAllowVPN )
    	{
    	iFilters.Append( KPluginVPNBearerTypeUid );
    	}

    iCmManager.OpenL();
    }


// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::IsApBearerCircuitSwitchedL()
// ----------------------------------------------------------------------------
//
TBool CMsvCommDbUtilities::IsApBearerCircuitSwitchedL( TUint32 aApId )
    {
    IMUM_CONTEXT( CMsvCommDbUtilities::IsApBearerCircuitSwitchedL, 0, KLogUi );

    TBool retVal = EFalse;

    // Get the connection method that's used, leaves if no
    // connection methods found
    RCmConnectionMethod connectionMethod = iCmManager.ConnectionMethodL( aApId );
    CleanupClosePushL( connectionMethod );

    // Check for CSD type
    TInt bearerType = connectionMethod.GetIntAttributeL( CMManager::ECmBearerType );
	if( bearerType == KCommDbBearerCSD )
		{
		retVal = ETrue;
		}
    // Check for VPN access point's home IAP
	else if( bearerType == KPluginVPNBearerTypeUid )
    	{
    	TInt homeIAP = connectionMethod.GetIntAttributeL( CMManager::EVpnIapId );

    	RCmConnectionMethod vpnMethod = iCmManager.ConnectionMethodL( homeIAP );
    	CleanupClosePushL( vpnMethod );
    	TInt vpnBearerType = vpnMethod.GetIntAttributeL( CMManager::ECmBearerType );

    	// Is the home AP a CSD access point?
    	if( vpnBearerType == KCommDbBearerCSD )
    		{
    		retVal = ETrue;
    		}
    	CleanupStack::PopAndDestroy(); // vpnMethod
    	}

    CleanupStack::PopAndDestroy(); // connectionMethod
    return retVal;
    }

// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::FillCustomIapArrayL()
// ----------------------------------------------------------------------------
//
void CMsvCommDbUtilities::FillCustomIapArrayL( CDesCArrayFlat& aFillArray )
    {
    IMUM_CONTEXT( CMsvCommDbUtilities::FillCustomIapArrayL, 0, KLogUi );

    // Set filters and get the accesspoint list
    InitializeSelectionL();

    // Create the current iap list
    const TInt count = iIapList.Count();

    // Make sure there are items in the array
    if ( count )
        {
        // Add all iap items to main array
        for ( TInt item = 0; item < count; item++ )
            {
            RCmConnectionMethod cm = iCmManager.ConnectionMethodL( iIapList[item] );
            CleanupClosePushL( cm );

            HBufC* apname = cm.GetStringAttributeL(CMManager::ECmName);
            CleanupStack::PushL(apname);
            // Insert to array and delete old item
            aFillArray.AppendL( apname->Des() );
            CleanupStack::PopAndDestroy(2); // CSI: 47 # cm, apname
            }
        }
    }


// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::InitializeSelectionL()
// ----------------------------------------------------------------------------
//
void CMsvCommDbUtilities::InitializeSelectionL()
    {
    IMUM_CONTEXT( CMsvCommDbUtilities::InitializeSelectionL, 0, KLogUi );

    if(!iInitialized)
        {
        TBool wlanSupported = EFalse;
        if ( iFlags->GF( EMailFeatureProtocolWlan ) )
            {
            wlanSupported = ETrue;
            }

        //fill the array of connection method id's
        iCmManager.ConnectionMethodL( iIapList, ETrue, EFalse, wlanSupported );

        TInt methodCount = iIapList.Count();
        TInt bearerCount = iFilters.Count();

        // Make sure there are items in the array
        if ( methodCount )
            {
            for ( TInt methodItem = 0; methodItem < methodCount; methodItem++ )
                {
                RCmConnectionMethod cm =
                	iCmManager.ConnectionMethodL( iIapList[methodItem] );
                CleanupClosePushL( cm );
                TUint methodBearer = cm.GetIntAttributeL( CMManager::ECmBearerType );
                CleanupStack::PopAndDestroy(1); //cm

                TBool found(EFalse);
                for ( TInt bearerItem = 0; bearerItem < bearerCount; bearerItem++ )
                    {
                    TUint bearerUid = iFilters[bearerItem];
                    if( bearerUid == methodBearer )
                        {
                        found = ETrue;
                        }
                    }
                if(!found)
                    {
                    iIapList.Remove(methodItem);
                    methodCount--;
                    }
                }
            }
        iInitialized = ETrue;
        }
    }


// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::StaticIAPCount()
// ----------------------------------------------------------------------------
//
TInt CMsvCommDbUtilities::StaticIAPCount()
	{
	IMUM_CONTEXT( CMsvCommDbUtilities::StaticIAPCount, 0, KLogUi );
	IMUM_IN();

	TInt staticCount = 1; // this is for default connection

    if ( iFlags->GF( EMailFeatureProtocolWlan ) )
        {
        // Add one for WLAN Access Point
        staticCount++;
        }

	IMUM_OUT();
	return staticCount;
	}


// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::InitItemAccessPointL()
// ----------------------------------------------------------------------------
//
void CMsvCommDbUtilities::InitItemAccessPointL(
    CIMSSettingsAccessPointItem& aIapItem,
    const TBool aIsWizard )
    {
    IMUM_CONTEXT( CMsvCommDbUtilities::InitItemAccessPointL, 0, KLogUi );

    // Initialize to first access point if called from wizard
    if( aIsWizard )
    	{
    	aIapItem.iIap.iResult = CMManager::EDefaultConnection;
    	aIapItem.iIap.iId = 0;
    	aIapItem.iIapRadioButton = 0;
    	}
    else
    	{
    	if( aIapItem.iIap.iId == 0 )
    		{
        	aIapItem.iIap.iResult = CMManager::EDefaultConnection;
        	aIapItem.iIapRadioButton = 0;
    		}
    	else
    		{
        	aIapItem.iIap.iResult = CMManager::EConnectionMethod;
        	aIapItem.iIapRadioButton = GetIapIndexL( aIapItem.iIap.iId );
    		}
    	}

    aIapItem.iIsWizard = aIsWizard;

    // Trap the leave but ignore the error, as the result is only seen in UI
    TRAP_IGNORE( SettingsItemIapStringL( aIapItem ) );
    }


// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::GetWizardRadioButton()
// ----------------------------------------------------------------------------
//
TInt CMsvCommDbUtilities::GetWizardRadioButton(
    CIMSSettingsAccessPointItem& aIapItem )
    {
    IMUM_CONTEXT( CMsvCommDbUtilities::GetWizardRadioButton, 0, KLogUi );

    return aIapItem.iIapRadioButton;
    }


// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::GetIapIndexL()
// ----------------------------------------------------------------------------
//
TInt CMsvCommDbUtilities::GetIapIndexL( const TUint32 aId )
	{
    IMUM_CONTEXT( CMsvCommDbUtilities::GetIapIndexL, 0, KLogUi );
    IMUM_IN();

    for ( TInt index = 0; index< iIapList.Count(); index++ )
        {
        TUint32 iapId( 0 );
        RCmConnectionMethod cm = iCmManager.ConnectionMethodL( iIapList[index] );
        CleanupClosePushL( cm );
        iapId = cm.GetIntAttributeL( CMManager::ECmId );
        CleanupStack::PopAndDestroy(1); // cm
        if( iapId == aId )
            {
            IMUM_OUT();
            return index;
            }
        }

    IMUM_OUT();
    // Default to 0 so the radio button points to the default connection
    return 0;
	}


// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::SettingsItemIapStringL()
// ----------------------------------------------------------------------------
//
const TDesC* CMsvCommDbUtilities::SettingsItemIapStringL(
    CIMSSettingsAccessPointItem& aIapItem )
    {
    IMUM_CONTEXT( CMsvCommDbUtilities::SettingsItemIapStringL, 0, KLogUi );

    // If the static access point is set, the name of the accesspoint can
    // be fetched from the list.
    if ( aIapItem.iIap.iResult != CMManager::EDefaultConnection )
        {
        RCmConnectionMethod cm;
        cm = iCmManager.ConnectionMethodL( aIapItem.iIap.iId );
        CleanupClosePushL( cm );
        HBufC* bearerName = cm.GetStringAttributeL(CMManager::ECmName);
        CleanupStack::PushL( bearerName );
        aIapItem.iItemSettingText->Copy( bearerName->Des() );
        CleanupStack::PopAndDestroy( 2, &cm ); // CSI: 47 # cm, bearername
        }
    else
        {
        TInt resourceId = 0;
        resourceId = R_NETW_CONSET_OPTIONS_DEFAULT_CONNECTION;
        ItemLoadBufferForItemL( aIapItem, resourceId );
        }

    return aIapItem.iItemSettingText;
    }

// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::ItemLoadBufferForItemL()
// ----------------------------------------------------------------------------
//
void CMsvCommDbUtilities::ItemLoadBufferForItemL(
    CIMSSettingsAccessPointItem& aIapItem,
    const TInt aResource )
    {
    IMUM_CONTEXT( CMsvCommDbUtilities::ItemLoadBufferForItemL, 0, KLogUi );

    // Load the string from the resource and store it into the setting item
    HBufC* text = StringLoader::LoadL( aResource );
    CleanupStack::PushL( text );

    aIapItem.iItemSettingText->Copy(
        text->Left( KMuiuDynMaxSettingsLongTextLength ) );

    CleanupStack::PopAndDestroy( text );
    text = NULL;
    }

// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::LaunchIapPageL()
// ----------------------------------------------------------------------------
//
TInt CMsvCommDbUtilities::LaunchIapPageL( CIMSSettingsAccessPointItem& aIapItem )
    {
    IMUM_CONTEXT( CMsvCommDbUtilities::LaunchIapPageL, 0, KLogUi );

    // This function handles the launching of the IAP setting page.
    // using the Connection Method Manager
    TInt result = KErrNotFound;

    // Show the access point list using Connection Method Manager
    CCmApplicationSettingsUi* settingsUi = CCmApplicationSettingsUi::NewLC();
    TUint apFilter = CMManager::EShowDefaultConnection |
    				 CMManager::EShowConnectionMethods;
    result = settingsUi->RunApplicationSettingsL( aIapItem.iIap,
    											  apFilter,
                                                  iFilters );
    CleanupStack::PopAndDestroy( settingsUi );

    // Return the quit method
    return result ? KErrNone : KErrCancel;
    }


// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::IsWlanAccessPointL()
// ----------------------------------------------------------------------------
//
TBool CMsvCommDbUtilities::IsWlanAccessPointL( TUint32 aIap ) const
    {
    IMUM_CONTEXT( CMsvCommDbUtilities::IsWlanAccessPointL, 0, KLogUi );

    TBool isWlan = EFalse;

    // First check if the AP is of wlan type
    TUint bearerType = BearerTypeL( aIap );
    if( bearerType == KUidWlanBearerType )
    	{
    	isWlan = ETrue;
    	}
    // Then make sure VPN doesn't have wlan IAP as home IAP
    else if( bearerType == KPluginVPNBearerTypeUid )
    	{
    	RCmConnectionMethod vpnCM = iCmManager.ConnectionMethodL( aIap );
    	CleanupClosePushL( vpnCM );
    	TInt homeIAP = vpnCM.GetIntAttributeL( CMManager::EVpnIapId );
    	CleanupStack::PopAndDestroy(); // vpnCM

    	isWlan = ( BearerTypeL( homeIAP ) == KUidWlanBearerType );
    	}
    	
    return isWlan;
    }

// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::DetermineImap4BufferSize()
// ----------------------------------------------------------------------------
//
TInt CMsvCommDbUtilities::DetermineImap4BufferSize(
    const TUint32 aIap,
    const CImumMboxDefaultData& aDefaultData ) const
    {
    IMUM_CONTEXT( CMsvCommDbUtilities::DetermineImap4BufferSize, 0, KLogUi );

	// If the following leaves, it really, really doesn't matter
    TBool isWlan = EFalse;
    TRAP_IGNORE( isWlan = IsWlanAccessPointL( aIap ) );

    TInt key = isWlan ?
        TImumInSettings:: EKeyDownloadBufferWlan:
        TImumInSettings:: EKeyDownloadBufferGprs;

    TInt value = 0;
    aDefaultData.GetAttr( key, value );
    return value;
    }

// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::BearerTypeL()
// ----------------------------------------------------------------------------
//
TUint32 CMsvCommDbUtilities::BearerTypeL( TUint32 aUid ) const
    {
    IMUM_CONTEXT( CMsvCommDbUtilities::BearerTypeL, 0, KLogUi );
    IMUM_IN();

    RCmConnectionMethod cm;
    cm = iCmManager.ConnectionMethodL( aUid );
    CleanupClosePushL( cm );
    TInt retVal = cm.GetIntAttributeL(CMManager::ECmBearerType);
    CleanupStack::PopAndDestroy(1); //cm

    IMUM_OUT();
    return retVal;
    }

// ----------------------------------------------------------------------------
// CMsvCommDbUtilities::GetIapIdL()
// ----------------------------------------------------------------------------
//
TInt CMsvCommDbUtilities::GetIapIdL( const TInt32 aSelectionIndex )
	{
    IMUM_CONTEXT( CMsvCommDbUtilities::GetIapIdL, 0, KLogUi );
    IMUM_IN();

    // The radiobutton id can't go over the count of access point list
    __ASSERT_ALWAYS( aSelectionIndex <= iIapList.Count(),
    		User::Panic( KImumCommsDbUtilPanic,
    		EPanicIAPsDoNotMatch ) );
    
    TUint32 iapId( 0 );
    RCmConnectionMethod cm = iCmManager.ConnectionMethodL( 
    		iIapList[ aSelectionIndex - 1 ] );
    CleanupClosePushL( cm );
    iapId = cm.GetIntAttributeL( CMManager::ECmId );
    CleanupStack::PopAndDestroy(); // cm
    IMUM_OUT();
    return iapId;

	}


// End of File
