/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides MMS Settings.
*
*/



// INCLUDE FILES
#include <coemain.h>
#include <eikenv.h>
#include <ApUtils.h> //CApUtils, CCommsDatabase, EDatabaseTypeIAP
#include <AknQueryDialog.h>
#include <AknWaitDialog.h>
#include <akninputblock.h>  //CAknInputBlock
#include <bautils.h>
#include <ConeResLoader.h>          // for RConeResourceLoader
#include <StringLoader.h>
#include <data_caging_path_literals.hrh> 

#include <msvids.h>

#include <MmsSettings.rsg>
#include <mmsclient.h>
#include <mmserrors.h>
#include <MtmExtendedCapabilities.hrh> // for TMscFactorySettingsLevel

#include "MmsSettingsHandler.h"
#include "MmsSettingsPanic.h"
#include "MmsMtmConst.h"

_LIT( KMmsSettingsResourceFileName, "mmssettings.rsc" );

GLREF_C void Panic(TMmsSettingsPanic aPanic)
    {
    _LIT( KPanicText,"MmsSettings.dll" );
    User::Panic( KPanicText, aPanic );
    }   

// ---------------------------------------------------------
// CMmsSettingsHandler::CMmsSettingsHandler()
// ---------------------------------------------------------
//
CMmsSettingsHandler::CMmsSettingsHandler( CMmsClientMtm& aMmsClient ) :
    iMmsClient( aMmsClient ), iResourceOffset( 0 )
    {
    }

// ---------------------------------------------------------
// CMsgInfoMessageInfoDialog::ConstructL
// ---------------------------------------------------------
//
void CMmsSettingsHandler::ConstructL()
    {    
    iMmsSettings = CMmsSettings::NewL();    
    iMmsClient.RestoreSettingsL();
    iMmsSettings->CopyL( iMmsClient.MmsSettings() );
    
    TParse fileParse;
    fileParse.Set( KMmsSettingsResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
    TFileName fileName( fileParse.FullName() );
    CEikonEnv* env = CEikonEnv::Static( );
    BaflUtils::NearestLanguageFile( env->FsSession(), fileName );
    iResourceOffset = env->AddResourceFileL( fileName );
    }

// ---------------------------------------------------------
// CMmsSettingsHandler::NewL()
// ---------------------------------------------------------
//
EXPORT_C CMmsSettingsHandler* CMmsSettingsHandler::NewL( CMmsClientMtm& aMmsClient )
    {
    LOGTEXT( _L8( "CMmsSettingsHandler::NewL, start" ) );
    CMmsSettingsHandler* self = new ( ELeave ) CMmsSettingsHandler( aMmsClient );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); //self
    return self;
    }

// ---------------------------------------------------------
// CMmsSettingsHandler::~CMmsSettingsHandler
// Destructor
// ---------------------------------------------------------
//
EXPORT_C CMmsSettingsHandler::~CMmsSettingsHandler()
    {
    if( iResourceOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );
        }
    delete iMmsSettings;
    if ( iWaitDialog )
        {
        TRAP_IGNORE( iWaitDialog->ProcessFinishedL() );
        }
    }

// ---------------------------------------------------------
// CMmsSettingsHandler::ApExistsL
// Checks the existence of given access point
// according to id.
// ---------------------------------------------------------
//
EXPORT_C TBool CMmsSettingsHandler::ApExistsL( TInt32 aAp )
    {
    if ( aAp == KErrNotFound )
        {
        return EFalse;
        }
    CCommsDatabase* commsDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( commsDb );
    CApUtils* apUtils = CApUtils::NewLC( *commsDb );
   
    TBool exists = EFalse;
    TRAPD( error, ( exists = apUtils->WapApExistsL( aAp ) ) );

    CleanupStack::PopAndDestroy( 2 ); //apUtils, commsDb

    if ( exists )
        {
        return ETrue;
        }
    
    if ( error != KErrNotFound )
        {
        User::LeaveIfError( error );
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CMmsSettingsHandler::CheckSettingsL() const
// Check MMS service settings
// ---------------------------------------------------------
//
EXPORT_C TBool CMmsSettingsHandler::CheckSettingsL( 
        CMmsSettingsDialog::TMmsExitCode& aExitCode )
    {
    TInt error = iMmsSettings->ValidateSettings();
    if ( error == KMmsErrorInvalidSettings )
        {
        //Settings corrupted. Restore factory settings.
        RestoreDefaultSettingsL( EMmsFactorySettingsLevelNormal );
        //Re-check the service.
        //error = iMmsClient.ValidateService( iMmsClient.DefaultSettingsL() );
        error = iMmsSettings->ValidateSettings();
        }
    if ( error != KErrNone )
        { // something wrong in service settings
        CAknQueryDialog* settingsNotOkQuery = CAknQueryDialog::NewL();

        if ( settingsNotOkQuery->ExecuteLD( R_MMSUI_QUERY_AP_ERROR ) )
            {
            // user wants to correct the settings
            CEikStatusPane* sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
            TInt previous = sp->CurrentLayoutResId();
            sp->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );
            sp->MakeVisible( ETrue );
            sp->DrawNow();

            LaunchSettingsDialogL( aExitCode );

            sp->SwitchLayoutL( previous );
            sp->MakeVisible( ETrue );
            sp->DrawNow();
            }
        return EFalse;
        }
    return ETrue;
    }

// ---------------------------------------------------------
// CMsvOperation* CMmsSettingsHandler::CompleteSettingsOperationL(
//    TRequestStatus& aCompletionStatus, const CMmsSettingsDialog::TMmsExitCode& aExitCode)
// Creates completed operation after settings dialog closing with appropriate parameters.
// ---------------------------------------------------------
//
EXPORT_C CMsvOperation* CMmsSettingsHandler::CompleteSettingsOperationL(
    TRequestStatus& aCompletionStatus,
    const CMmsSettingsDialog::TMmsExitCode& aExitCode
    /* = CMmsSettingsDialog::EMmsExternalInterrupt */)
    {
    TInt err = KErrNone;
    if ( aExitCode == CMmsSettingsDialog::EMmsExit )
        {
        LOGTEXT( _L8( "MmsMtmUi Service Settings dialog exiting" ) );
        err = EEikCmdExit;
        }
    // Nothing to do, so return a completed operation.
    return CMsvCompletedOperation::NewL( iMmsClient.Session(), 
        KUidMsvLocalServiceMtm, 
        KNullDesC8, 
        KMsvLocalServiceIndexEntryId, 
        aCompletionStatus, 
        err );
    }

// ---------------------------------------------------------
// CMmsSettingsHandler::HandleFactorySettingsL
// Reset Factory Settings support
// ---------------------------------------------------------
EXPORT_C void CMmsSettingsHandler::HandleFactorySettingsL( TDes8& aLevel )
    {
    TPckgBuf <TMsvFactorySettingsLevel> paramPack;
    const TInt paramSize = sizeof( TMsvFactorySettingsLevel );

    __ASSERT_DEBUG( aLevel.Length() > 0, Panic( EMmsSettingsWrongParameters ) );

    if ( aLevel.Length() > 0 )
        {
        paramPack.Copy( aLevel.Ptr(), paramSize );
        const TMsvFactorySettingsLevel& level = paramPack();

        LOGTEXT2( _L16( "CMmsSettingsHandler calling RestoreFactorySettings with level %d"), level );
        if ( level == EMsvFactorySettingsLevelNormal )
            {
            RestoreDefaultSettingsL( EMmsFactorySettingsLevelNormal );
            }
        else if ( level == EMsvFactorySettingsLevelDeep )
            {
            RestoreDefaultSettingsL( EMmsFactorySettingsLevelDeep );
            }
        else
            {
            __ASSERT_DEBUG( EFalse, Panic( EMmsSettingsWrongParameters ) );
            }
        }
    }

// ---------------------------------------------------------
// void CMmsSettingsHandler::LaunchSettingsDialogL(
//      CMmsSettingsDialog::TMmsExitCode& aExitCode )
// Opens either MMS Service Settings or MMS Sending Settings dialog.
// ---------------------------------------------------------
//
EXPORT_C void CMmsSettingsHandler::LaunchSettingsDialogL(
    CMmsSettingsDialog::TMmsExitCode& aExitCode )
    {
    // Reload settings, because mce does not create CMmsSettingsHandler from scratch
    RefreshSettingsL();
    CMmsSettingsDialog* dlg = new ( ELeave ) CMmsSettingsDialog( iMmsSettings, aExitCode );
    
    LOGTEXT( _L8( "MmsMtmUi Opening Settings dialog" ) );
    dlg->ConstructL( R_SETTINGSDIALOG_MENUBAR );
    dlg->ExecuteLD( R_MMSUI_MMSSETTINGS );
    //return value ignored.
    
    // Save changes only if Back or ExitWithSave is selected
    if ( aExitCode==CMmsSettingsDialog::EMmsBack 
        ||  aExitCode==CMmsSettingsDialog::EMmsExitWithSave )
        {
        iWaitDialog =  new ( ELeave ) CAknWaitDialog( NULL );
        iWaitDialog->PrepareLC( SAVING_WAIT_NOTE );
        HBufC* text = StringLoader::LoadLC( R_MMS_SETTINGS_PROCESSING );
        iWaitDialog->SetTextL( *text );
        CleanupStack::PopAndDestroy(); // text

        iWaitDialog->SetCallback(this);
        iWaitDialog->RunLD();
        //Adding InputBlocker to avoid handling of user keypresses until the mmsclient operations have completed
        CAknInputBlock* inputBlock = CAknInputBlock::NewLC();
        iMmsClient.SetSettingsL( *iMmsSettings );
        iMmsClient.StoreSettingsL();
        CleanupStack::PopAndDestroy(inputBlock);                    
        
        if ( iWaitDialog )
            {
            iWaitDialog->ProcessFinishedL();
            iWaitDialog = NULL;
            }

        if ( aExitCode==CMmsSettingsDialog::EMmsExitWithSave )
            { // Change exit code if ExitWithSave is selected
            aExitCode = CMmsSettingsDialog::EMmsExit;
            }
        }
    }

// ---------------------------------------------------------
// CMsvOperation* CMmsSettingsHandler::OpenServiceSettingsDialogL(TRequestStatus& aCompletionStatus)
// Handles MMS service settings
// ---------------------------------------------------------
//
EXPORT_C CMsvOperation* CMmsSettingsHandler::OpenServiceSettingsDialogL( TRequestStatus& aCompletionStatus )
    {
    CMmsSettingsDialog::TMmsExitCode exitCode;
    LaunchSettingsDialogL( exitCode );
    return CompleteSettingsOperationL( aCompletionStatus, exitCode );
    }

// ---------------------------------------------------------
// CMmsSettingsHandler::CreateDefaultServiceL
// ---------------------------------------------------------
//
void CMmsSettingsHandler::RestoreDefaultSettingsL( TMmsFactorySettingsLevel aLevel ) const
    {
    iMmsSettings->RestoreFactorySettingsL( iMmsClient.Session(), aLevel );
    iMmsClient.SetSettingsL( *iMmsSettings );
    iMmsClient.StoreSettingsL();
    }

// ---------------------------------------------------------
// DialogDismissedL
// ---------------------------------------------------------
//
void CMmsSettingsHandler::DialogDismissedL( TInt /*aButtonId*/ ) 
    {
    iWaitDialog = NULL;
    }
// End of File
