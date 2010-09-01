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
*       Static functions for validating account settings prior to connection
*
*/


#include <eikenv.h>
#include <eikbtgpc.h>
#include <eikseced.h>
#include <iapprefs.h>
#include <smtpset.h>
#include <pop3set.h>
#include <imapset.h>
#include <MsvPrgReporter.h>
#include <imum.rsg>                 // R_MBXS_SETTINGS_MAIL_SAVE_PSWRD_QRY
#include <AknQueryDialog.h>
#include <SenduiMtmUids.h>
#include <ImumInternalApi.h>        // CImumInternalApi
#include "ImumPanic.h"
#include "EmailUtils.H"
#include "ComDbUtl.h"
#include "MsvConnectionValidation.h"
#include "ImumUsernameAndPasswordDialog.h"
#include "ImumMtmLogging.h"
#include "IMSSettingsNoteUi.h"
#include "ImumMboxSettingsUtils.h"
#include <ImumInMailboxServices.h>
#include <ImumInMailboxUtilities.h>
#include <ImumInSettingsKeys.h>
#include "EmailFeatureUtils.h"
#include "ImumDaErrorCodes.h"

// CONSTANTS
const TInt KImumSettingsIapPreferenceNumber     = 0;

// ----------------------------------------------------------------------------
// MsvConnectionValidation::RunAndHandleLoginDialogL()
// ----------------------------------------------------------------------------
//
TIMASPasswordSave MsvConnectionValidation::RunAndHandleLoginDialogL(
    TDes8& aUserName,
    TDes8& aPassword,
    TBool& aPasswordTemporary )
    {
    IMUM_STATIC_CONTEXT( MsvConnectionValidation::RunAndHandleLoginDialogL, 0, mtm, KImumMtmLog );
    IMUM_IN();
            
    TIMASPasswordSave ret = EIMASPasswordNoSave;
    // Store the username and password temporarily to prevent losing them
    TMuiuPasswordText tempUsername;
    TMuiuPasswordText tempPassword;
    tempUsername.Copy( aUserName );
    tempPassword.Copy( aPassword );

    // New values are accepted, but the user's permission is required to store
    // the password to the settings
    if ( RunUsernameAndPasswordDialogL( tempUsername, tempPassword ) )
        {
         // Store settings.
        aUserName.Copy( tempUsername );
        aPassword.Copy( tempPassword );

        // If the user answers no, set the password temporary
        if ( !CIMSSettingsNoteUi::ShowQueryL(
            R_MBXS_SETTINGS_MAIL_SAVE_PSWRD_QRY, R_EMAIL_CONFIRMATION_QUERY ) )
            {
            aPasswordTemporary = ETrue;
            ret = EIMASPasswordTempSave;
            }
        else
            {
            aPasswordTemporary = EFalse;
            ret = EIMASPasswordPermanentSave;
            }
        }
    else
        {
        // Leave everything untouched
        ret = EIMASPasswordCancelSave;
        }
    IMUM_OUT();

    return ret;
    }

// ----------------------------------------------------------------------------
// MsvConnectionValidation::CheckAndPromptForPasswordL
// Check that there is a password specified, if not, prompt user to enter one
// and store it in the settings.
// ----------------------------------------------------------------------------
TBool MsvConnectionValidation::CheckAndPromptForPasswordL(
    CImumInternalApi& aMailboxApi,
    TMsvId aServiceId,
    TMsvLoginData aLoginData,
    TInt& aLoginRetryCounter )
    {
    IMUM_STATIC_CONTEXT( MsvConnectionValidation::CheckAndPromptForPasswordL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    TBool ret = EFalse;

    // Temporary information is not accepted as valid data
    if ( aLoginData.iIsPasswordTemporary )
        {
        // Don't initialize temp password into a username/password query dialog
        aLoginData.iPassword.Zero();
        }

    if ( aLoginData.iUsername.Length() && aLoginData.iPassword.Length() )
        {
        // Have a valid password.
        ret = ETrue;
        }
    else
        {
        aLoginRetryCounter++;

        // Run dialog
        TIMASPasswordSave save = EIMASPasswordCancelSave;

        if( aMailboxApi.IsEmailFeatureSupportedL( 
            CImumInternalApi::EMailLockedSettings ) )
            {
            CIMSSettingsNoteUi::ShowNoteL( 
                R_IMUM_ERROR_LOCKED_SETTIGNS, 
                EIMSErrorNote );  
            }
        else
            {
            save = RunAndHandleLoginDialogL(
                aLoginData.iUsername,
                aLoginData.iPassword,
                aLoginData.iIsPasswordTemporary );
            }

        switch ( save )
            {
            case EIMASPasswordTempSave:
            case EIMASPasswordPermanentSave:
                // Save settings (login info) into data base.
                ImumMboxSettingsUtils::SetLoginInformationL( aMailboxApi,
                    aLoginData.iUsername, aLoginData.iPassword,
                    aLoginData.iIsPasswordTemporary, aServiceId );
                ret = ETrue;
                break;
            case EIMASPasswordCancelSave:
            case EIMASPasswordNoSave:
            default:
                ret = EFalse;
            }
        }
    IMUM_OUT();
    return ret;
    }

// ----------------------------------------------------------------------------
// MsvConnectionValidation::ShowLoginDetailsRejectionDlgL
//
// Prompt the user to check the login details for aRcvServiceId, allowing them to change the
// details. Returns EFalse if user cancels.
//
// ----------------------------------------------------------------------------
TBool MsvConnectionValidation::ShowLoginDetailsRejectionDlgL(
    CImumInternalApi& aMailboxApi,
    TMsvId aServiceId )
    {
    IMUM_STATIC_CONTEXT( MsvConnectionValidation::ShowLoginDetailsRejectionDlgL, 0, mtm, KImumMtmLog );
    IMUM_IN();

    // Get settings.
    TBuf8<KImasLoginLength> username8;
    TBuf8<KImasPasswordLength> password8;
    TBool isPasswordTemporary = EFalse;

    // Load login information
    ImumMboxSettingsUtils::GetLoginInformationL(
        aMailboxApi, 
        username8, 
        password8, 
        isPasswordTemporary,
        aServiceId );

    TInt ret = EIMASPasswordCancelSave;

    // Run dialog if email settings lock is not active.
    if( aMailboxApi.IsEmailFeatureSupportedL( 
        CImumInternalApi::EMailLockedSettings ) )
        {
        CIMSSettingsNoteUi::ShowNoteL( 
            R_IMUM_ERROR_LOCKED_SETTIGNS, 
            EIMSErrorNote );      
        }
    else
        {
        ret = RunAndHandleLoginDialogL(
            username8, 
            password8, 
            isPasswordTemporary );    
        }  
   
    // Save login information
    ImumMboxSettingsUtils::SetLoginInformationL(
        aMailboxApi, 
        username8, 
        password8, 
        isPasswordTemporary,        
        aServiceId );
    IMUM_OUT();

    return ( ret == EIMASPasswordCancelSave ? EFalse : ETrue );
    }

// ----------------------------------------------------------------------------
// MsvConnectionValidation::IsAlwaysAskSet()
// ----------------------------------------------------------------------------
//
TBool MsvConnectionValidation::IsAlwaysAskSet(
    const CImIAPPreferences& iapPref )
    {
    IMUM_STATIC_CONTEXT( MsvConnectionValidation::IsAlwaysAskSet, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    TImIAPChoice iapChoice =
        iapPref.IAPPreference( KImumSettingsIapPreferenceNumber );

    // Always ask is set, when prompting is set and iap is set to 0
    if ( iapChoice.iDialogPref == ECommDbDialogPrefPrompt &&
         iapChoice.iIAP == 0 )
        {
        IMUM_OUT();
        return ETrue;
        }
    IMUM_OUT();

    return EFalse;
    }

// ----------------------------------------------------------------------------
// MsvConnectionValidation::InstantiateSettingsObjectLC
//
// Create the appropriate type of settings object for aEntry.
//
// ----------------------------------------------------------------------------
CImBaseEmailSettings* MsvConnectionValidation::InstantiateSettingsObjectLC(
    const TMsvEntry& aEntry,
    TUid& aMtmType)
    {
    IMUM_STATIC_CONTEXT( MsvConnectionValidation::InstantiateSettingsObjectLC, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CImBaseEmailSettings* ret = NULL;
    aMtmType = aEntry.iMtm;
    switch(aMtmType.iUid)
        {
        case KSenduiMtmSmtpUidValue:
            ret = new(ELeave) CImSmtpSettings;
            break;
        case KSenduiMtmPop3UidValue:
            ret = new(ELeave) CImPop3Settings;
            break;
        case KSenduiMtmImap4UidValue:
            ret = new(ELeave) CImImap4Settings;
            break;
        default:
            User::Panic(KImumMtmUiPanic,EPanicUnknownMtmType);
            break;
        }
    CleanupStack::PushL(ret);
    IMUM_OUT();
    return ret;
    }

// ----------------------------------------------------------------------------
// MsvConnectionValidation::RunUsernameAndPasswordDialogL
//
// ----------------------------------------------------------------------------
TInt MsvConnectionValidation::RunUsernameAndPasswordDialogL(
    TDes& aUserName,
    TDes& aPassword )
    {
    IMUM_STATIC_CONTEXT( MsvConnectionValidation::RunUsernameAndPasswordDialogL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CImumUsernameAndPasswordDialog* dlg =
        CImumUsernameAndPasswordDialog::NewL( aUserName, aPassword );
    IMUM_OUT();
    return dlg->ExecuteLD(R_CONUTL_LOGINDETAILS_DLG);
    }


// ----------------------------------------------------------------------------
// MsvConnectionValidation::GetServiceNameL
//
// ----------------------------------------------------------------------------
void MsvConnectionValidation::GetServiceNameL(
    CMsvSession& aSession,
    TMsvId aServiceId,
    TDes& aServiceName )
    {
    IMUM_STATIC_CONTEXT( MsvConnectionValidation::GetServiceNameL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CMsvEntry* centry = aSession.GetEntryL(aServiceId);
    aServiceName = centry->Entry().iDetails;
    delete centry;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// MsvConnectionValidation::GetServiceIapNameL
//
// ----------------------------------------------------------------------------
void MsvConnectionValidation::GetServiceIapNameL(
    CImumInSettingsData& aAccountSettings, TUid aMtmUid, TDes& aIapName)
    {
    IMUM_STATIC_CONTEXT( MsvConnectionValidation::GetServiceIapNameL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    TInt error = aAccountSettings.Validate();
      
    if ( !error )
        {                    
        TInt connection = ( aMtmUid == KSenduiMtmSmtpUid ) ? 1 : 0;
        
        aAccountSettings[connection].GetAttr( 
            TImumInSettings::EKeyAccessPointName, 
            aIapName );
        }
    // handle the error caused by invalid email address
    else if (( error == TImumDaErrorCode::EEmailAddressNotSet ) || 
             ( error == TImumDaErrorCode::EEmailAddressEmpty ) ||
             ( error == TImumDaErrorCode::EEmailAddressInvalid ))
        {
        CIMSSettingsNoteUi::ShowNoteL( R_IMUM_EDIT_OWN_EMAIL_ADDRESS_ERROR_TEXT,
            EIMSInformationNote, ETrue );
        User::Leave( KErrNotReady );
        }    
    else
        {
        User::Leave( KErrNotReady );
        }
    IMUM_OUT();
    }


// End of File
