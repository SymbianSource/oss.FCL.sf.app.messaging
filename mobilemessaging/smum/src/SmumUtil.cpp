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
* Description:  
*       Finds default name for Service Centre and adds ordinal
*       number in the end if it is needed.
*
*/



// INCLUDE FILES
#include <coemain.h>                    // CCoeEnv::Static()
#include <SMUM.rsg>                     // resource identifiers
#include <stringresourcereader.h>       // CStringResourceReader
#include <StringLoader.h>               // StringLoader
#include <aknnotewrappers.h>			// CAknInformationNote

#include <smutset.h>                    // CSmsSettings
#include "SmumUtil.h"                   // SmumUtil
#include "SmumLogging.h"
#include <messagingvariant.hrh>
#include <RCustomerServiceProfileCache.h>
#include "smsui.pan"                    // Panic codes
#include <data_caging_path_literals.hrh>
#include <centralrepository.h>          // CRepository
#include <messaginginternalcrkeys.h>    // Keys

// CONSTANTS
_LIT(KFmtSpec, "%S(%02d)");
_LIT(KSmumResourceFileName, "smum.rsc");
const TInt KTestStringLength = 100;

// ----------------------------------------------------
// FindDefaultNameL
// Finds default name for Service Centre and adds ordinal
// number in the end if it is needed.
// ----------------------------------------------------
EXPORT_C void SmumUtil::FindDefaultNameForSCL(
    TDes& aName,
    TBool aNameIsForSCentreFromSIM,
    CDesCArrayFlat* aSCNamesList)
    { 
    // Parse the filename
    TFileName myFileName;
    TParse tp;
    tp.Set( KSmumResourceFileName, &KDC_MTM_RESOURCE_DIR, NULL );
    myFileName.Copy( tp.FullName() );

    // Initialise string resource reader
    CStringResourceReader* reader = CStringResourceReader::NewLC( myFileName );
 
    // Load default name without ordinals
    HBufC* buf = reader->ReadResourceString( aNameIsForSCentreFromSIM ? 
        R_QTN_MCE_SETTINGS_SIM_CENTRE : R_QTN_MCE_SETTINGS_SMS_SC_D_NAME ).AllocLC();
  

    TInt nameMaxLength = aName.MaxLength();
    if ( nameMaxLength < buf->Length())
        {
        User::Leave(KErrOverflow);
        }
    aName.Append(buf->Des());

    if (aSCNamesList)
        {
        // Launched from CSmsServiceCentresDialog, so we need
        // to check the list and use ordinals if needed
        const TInt lBoxCount = aSCNamesList->Count();
        TInt defOrdinal = 0;
        TBool nameAlreadyInUse = EFalse;
        TBool canExit = EFalse;
        
        if (lBoxCount)
            {
            while (!canExit)
                {
                // tried name already in use, let's increase ordinal
                if (nameAlreadyInUse)
                    {                
                    TBuf<KTestStringLength> testBuf;
                    testBuf.Format( KFmtSpec, buf, defOrdinal );
                    if ( nameMaxLength < testBuf.Length() ) 
                        {
                        User::Leave( KErrOverflow );
                        }
                    aName.Format( KFmtSpec, buf, defOrdinal );
                    AknTextUtils::LanguageSpecificNumberConversion( aName );
                    nameAlreadyInUse = EFalse;
                    }        
                // do we already have this name?
                for ( TInt loop = 0; loop < lBoxCount && !nameAlreadyInUse; loop++ )
                    {
                    // yes
                    if ( KErrNotFound != aName.Match((*aSCNamesList)[loop]) )
                        {
                        nameAlreadyInUse = ETrue;
                        defOrdinal++;
                        }
                    // no, let's use it
                    else if ( loop == lBoxCount-1 )
                        {
                        canExit = ETrue;
                        }
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy( 2 ); // buf, reader 
    }

// ----------------------------------------------------
// SmumUtil::::ShowInformationNoteL
//
// ----------------------------------------------------
void SmumUtil::ShowInformationNoteL( TInt aResource )
    {
    CCoeEnv* coeEnv = CEikonEnv::Static();
    __ASSERT_DEBUG( coeEnv, Panic( ESmumUtilNullPointer ) );
    if ( !coeEnv )
        {
        User::Leave( KErrGeneral );        
        }
    HBufC* text = StringLoader::LoadLC( aResource, coeEnv );
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy(); // text
    }
    
// ----------------------------------------------------------------------------
// SmumUtil::CheckVariationFlagsL()
// Function to check and set variation according to 
// flags set in Central Repository
// ----------------------------------------------------------------------------
TInt SmumUtil::CheckVariationFlagsL( const TUid aUid, const TUint32 aKey )
    {
    TInt flags = KErrNotFound;
    // Create storage
    CRepository* storage = CRepository::NewLC( aUid );
    TInt error = storage->Get( aKey, flags ); 
    // Leave on error
    User::LeaveIfError( error );
    CleanupStack::PopAndDestroy(); // storage
    return flags;
    }    

// ----------------------------------------------------
// SmumUtil::ReadEmailOverSmsSettingsL
// ----------------------------------------------------
//
EXPORT_C TInt SmumUtil::ReadEmailOverSmsSettingsL( 
    TDes& aSmsc, 
    TDes& aDestinationAddress,
    TBool& aModifiable )
    {
    SMUMLOGGER_ENTERFN("SmumUtil::ReadEmailOverSmsSettingsL")
	// Create storage
    CRepository* storage = CRepository::NewLC( KCRUidSmum );
	storage->Get( KSumEmailSC, aSmsc );
	SMUMLOGGER_WRITE_FORMAT(
        "ReadEmailOverSmsSettingsL - SmsC: %s", aSmsc.Ptr() )
	storage->Get( KSumEmailGateway, aDestinationAddress );
	SMUMLOGGER_WRITE_FORMAT(
        "ReadEmailOverSmsSettingsL - Gateway: %s", aDestinationAddress.Ptr() )
	storage->Get( KSumEmailModifiable, aModifiable );
	SMUMLOGGER_WRITE_FORMAT("ReadEmailOverSmsSettingsL - Modifiable: %d", aModifiable)
    CleanupStack::PopAndDestroy(); // storage
    SMUMLOGGER_LEAVEFN("SmumUtil::ReadEmailOverSmsSettingsL")
    return KErrNone;
    }

// ----------------------------------------------------
// SmumUtil::WriteEmailOverSmsSettingsL
// ----------------------------------------------------
//
EXPORT_C TInt SmumUtil::WriteEmailOverSmsSettingsL( 
    const TDes& aSmsc, 
    const TDes& aDestinationAddress,
    const TBool& aModifiable )
    {
    SMUMLOGGER_ENTERFN("SmumUtil::WriteEmailOverSmsSettingsL")
    // Create storage
    CRepository* storage = CRepository::NewLC( KCRUidSmum );
	storage->Set( KSumEmailSC, aSmsc );
	SMUMLOGGER_WRITE_FORMAT(
        "WriteEmailOverSmsSettingsL - SmsC: %s", aSmsc.Ptr() )
	storage->Set( KSumEmailGateway, aDestinationAddress );
	SMUMLOGGER_WRITE_FORMAT(
    	"WriteEmailOverSmsSettingsL - Gateway: %s", aDestinationAddress.Ptr() )
	storage->Set( KSumEmailModifiable, aModifiable );
	SMUMLOGGER_WRITE_FORMAT("WriteEmailOverSmsSettingsL - Modifiable: #%d", aModifiable)
    CleanupStack::PopAndDestroy(); // storage
    SMUMLOGGER_LEAVEFN("SmumUtil::WriteEmailOverSmsSettingsL")
    return KErrNone;
    }

// ----------------------------------------------------
// SmumUtil::CheckEmailOverSmsSupportL
//
// ----------------------------------------------------
EXPORT_C TBool SmumUtil::CheckEmailOverSmsSupportL()
    {
    SMUMLOGGER_ENTERFN("SmumUtil::CheckEmailOverSmsSupportL")
    TBool emailSupported( EFalse );
    TInt tmpInt( KErrNotFound );
    tmpInt = CheckVariationFlagsL( KCRUidMuiuVariation, KMuiuSmsFeatures );
    if ( tmpInt & KSmsFeatureIdEmailOverSms ) 
        { // Feature is supported by S60 local variation
        SMUMLOGGER_WRITE_FORMAT(
            "CheckEmailOverSmsSupportL - KSmsFeatureIdEmailOverSms :%d",
            tmpInt )
        // Now we must check if the feature is supported by 
        // Value Added Services, (U)SIM bit
        emailSupported = ReadEmailSupportFromVASL();
        SMUMLOGGER_WRITE_FORMAT(
            "CheckEmailOverSmsSupportL - ReadEmailSupportFromVASL returned :%d",
            emailSupported )
        }
    SMUMLOGGER_WRITE_FORMAT(
                "CheckEmailOverSmsSupportL - EmailOverSmsSupport :%d",
                emailSupported )
    SMUMLOGGER_LEAVEFN("SmumUtil::CheckEmailOverSmsSupportL")
    return emailSupported;
    }

// ---------------------------------------------------------------------------
// SmumUtil::ReadEmailSupportFromVASL
// 
// ---------------------------------------------------------------------------
TBool SmumUtil::ReadEmailSupportFromVASL()
    {
    SMUMLOGGER_ENTERFN("CSmumMainSettingsDialogGSM::ReadEmailSupportFromVASL")
    TBool featureSupport( EFalse );
    RCustomerServiceProfileCache* cspProfile = 
        new (ELeave) RCustomerServiceProfileCache;    
    __ASSERT_DEBUG( cspProfile, Panic( ESmsuNullPointer ) );
    TInt error = cspProfile->Open();
    if( KErrNone == error )
        {
        // Get tele services flags from CSP
        RMobilePhone::TCspValueAdded params;
        // If not supported (-5) or any other error
        // we assume all settings are supported
        error = cspProfile->CspCPHSValueAddedServices( params );
        if ( KErrNone == error ) 
            {
            SMUMLOGGER_WRITE_FORMAT(
                "ReadEmailSupportFromVASL - RMobilePhone::TCspTeleservices #%d", params )
            if( 0 != ( params&RMobilePhone::KCspSMMOEmail ) ) 
                {
                SMUMLOGGER_WRITE(
                    "ReadEmailSupportFromVASL - KCspSMMOEmail supported" )
                featureSupport = ETrue;
                }
            }
        else
            {
            SMUMLOGGER_WRITE_FORMAT(
                "ReadEmailSupportFromVASL - cspProfile->CspTeleServices() #%d", error )
            }
        cspProfile->Close();
        }
    else
        {
        SMUMLOGGER_WRITE_FORMAT(
                "ReadEmailSupportFromVASL - cspProfile->Open() #%d", error )
        }
    delete cspProfile;    
    SMUMLOGGER_LEAVEFN("CSmumMainSettingsDialogGSM::ReadEmailSupportFromVASL")
    return featureSupport;
    }

//  End of File  
