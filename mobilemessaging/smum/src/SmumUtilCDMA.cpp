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
#include <smum.rsg>                     // resource identifiers
#include <stringresourcereader.h>       // CStringResourceReader
#include <StringLoader.h>               // StringLoader
#include <aknnotewrappers.h>            // CAknInformationNote
#include "SmumUtil.h"                   // SmumUtil
#include "SmumLogging.h"
#include "smsui.pan"                    // Panic codes
#include <data_caging_path_literals.hrh>
#include <CentralRepository.h>          // CRepository
#include <MessagingInternalCRKeys.h>    // Keys

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
    CDesCArrayFlat* /*aSCNamesList*/)
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
    TDes& /*aSmsc*/, 
    TDes& /*aDestinationAddress*/,
    TBool& /*aModifiable*/ )
    {
    SMUMLOGGER_ENTERFN("SmumUtil::ReadEmailOverSmsSettingsL")
    SMUMLOGGER_LEAVEFN("SmumUtil::ReadEmailOverSmsSettingsL")
    return KErrNone;
    }

// ----------------------------------------------------
// SmumUtil::WriteEmailOverSmsSettingsL
// ----------------------------------------------------
//
EXPORT_C TInt SmumUtil::WriteEmailOverSmsSettingsL( 
    const TDes& /*aSmsc*/, 
    const TDes& /*aDestinationAddress*/,
    const TBool& /*aModifiable*/ )
    {
    SMUMLOGGER_ENTERFN("SmumUtil::WriteEmailOverSmsSettingsL")
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
    SMUMLOGGER_LEAVEFN("SmumUtil::CheckEmailOverSmsSupportL")
    return EFalse;
    }

// ---------------------------------------------------------------------------
// SmumUtil::ReadEmailSupportFromVASL
// 
// ---------------------------------------------------------------------------
TBool SmumUtil::ReadEmailSupportFromVASL()
    {
    SMUMLOGGER_ENTERFN("CSmumMainSettingsDialogGSM::ReadEmailSupportFromVASL")
    SMUMLOGGER_LEAVEFN("CSmumMainSettingsDialogGSM::ReadEmailSupportFromVASL")
    return EFalse;
    }

//  End of File  
