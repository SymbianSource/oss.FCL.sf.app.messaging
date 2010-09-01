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
* Description:  Implements storage class for mail preferences
*
*/


// INCLUDE FILES
#include <msvapi.h>                     // MsvSession, MsvEntry
#include <miutset.h>                    // KUidMsgTypeSMTP
#include <smtpset.h>                    // CImSmtpSettings
#include <featmgr.h>
#include <bldvariant.hrh>
#include <charconv.h>
#include <cemailaccounts.h>				// CEmailAccounts
#include <SendUiConsts.h>
#include "MsgMailPreferences.h"         // TMsgMailPreferences


// LOCAL CONSTANTS
const TInt KFirstBit = 1;
const TInt KSecondBit = 2;
const TInt KFourthBit = 4;


// ================= MEMBER FUNCTIONS =======================

// Constructor
// Sets default values.
//
EXPORT_C CMsgMailPreferences::CMsgMailPreferences():
    iScheduling(CMsgMailPreferences::EMsgMailSchedulingNextConn),     
    iServiceId(KMsvUnknownServiceIndexEntryId)
    {
    iHeaderResults = 5;				// CSI: 47 # See a comment above.
    // if these defaults above are changed, DefaultAccountSettings() should be 
    // changed accordingly
    }

// ----------------------------------------------------------------------------
// CMsgMailPreferences::DefaultAccountSettingsL gets all default values 
// for all preferences from specified remote mailbox service
// Returns: none (updates fields)
// ----------------------------------------------------------------------------
//
EXPORT_C void CMsgMailPreferences::DefaultAccountSettingsL(
    const TMsvId aServiceId, // service whose settings are to be used
    CMsvSession& aSession) // used to get the settings
    {
    SetServiceId( aServiceId );

    if ( aServiceId != KMsvUnknownServiceIndexEntryId )        
        {            
        // Resolve account id
        TMsvEntry smtpEntry;
        TMsvId serviceId;
        User::LeaveIfError( 
        	aSession.GetEntry( aServiceId, serviceId, smtpEntry ) );
        
        // Set "Send on next connection" and Utf 8 as defaults
        TImSMTPSendMessageOption sendingOption = ESendMessageOnNextConnection;
        iSendingCharacterSet = TUid::Uid(KCharacterSetIdentifierUtf8);
        
            
		CEmailAccounts* smtpAccount = CEmailAccounts::NewLC();
		CImSmtpSettings* smtpSet = new ( ELeave ) CImSmtpSettings();
		CleanupStack::PushL( smtpSet );
	
		TSmtpAccount accountParams;
		accountParams.iSmtpService = aServiceId;
		accountParams.iSmtpAccountId = smtpEntry.iMtmData2;
		
		smtpAccount->LoadSmtpSettingsL( accountParams, *smtpSet );
		sendingOption = smtpSet->SendMessageOption();
		iSendingCharacterSet = SolveCharacterSet( *smtpSet );
		
		CleanupStack::PopAndDestroy( 2, smtpAccount );			// CSI: 47 # smtpSet, smtpAccount
				
		
        SetMessageScheduling(
        	( sendingOption == ESendMessageImmediately ) ?
            EMsgMailSchedulingNow : EMsgMailSchedulingNextConn );        	                      	
        }     
    else 
        {
        // no default account, restore defaults (see constructor)
        SetMessageScheduling(
        	CMsgMailPreferences::EMsgMailSchedulingNextConn);
        }
    }

// ----------------------------------------------------------------------------
// CMsgMailPreferences::ExportSendOptionsL exports preferences from TMsvEntry
// Returns: none (updates fields from TMsvEntry)
// ----------------------------------------------------------------------------
//
EXPORT_C void CMsgMailPreferences::ExportSendOptionsL(
    const TMsvEmailEntry& aEntry) // TMsvEntry from where the values are got
    {
    // Send options: "Mail account"
    SetServiceId(aEntry.iServiceId);

    // Send options: "Sending"
    SetMessageScheduling(aEntry.SendingState() == KMsvSendStateWaiting ?
        EMsgMailSchedulingNow : EMsgMailSchedulingNextConn);
    }

// ----------------------------------------------------------------------------
// CMsgMailPreferences::ImportSendOptionsL imports preferences into TMsvEntry
// ----------------------------------------------------------------------------
//
EXPORT_C void CMsgMailPreferences::ImportSendOptionsL(TMsvEmailEntry& aEntry)
    {
    aEntry.iServiceId = ServiceId();

    // set Send options: "Sending" in entry
    // for entry, there is no difference between now and next connection
    if ( MessageScheduling() == 
        CMsgMailPreferences::EMsgMailSchedulingNow )
        {
        aEntry.SetSendingState(KMsvSendStateWaiting);
        }
    else 
        {
        aEntry.SetSendingState(KMsvSendStateScheduled);
        }    
    }

//
// ---------------------------------------------------------
// CMsgMailPreferences::GetAdditionalHeaderVisibility gets header visibility
// Returns: header visibility status 
// ---------------------------------------------------------
//
EXPORT_C EHeaderStatus CMsgMailPreferences::GetAdditionalHeaderVisibility( TMsgControlId aHeader)
    {
    TInt results(0);

    if(aHeader == EMsgComponentIdCc)
    	{
    	results = iHeaderResults ^ KFirstBit;
    	}
    else if(aHeader == EMsgComponentIdBcc)
    	{
    	results = iHeaderResults ^ KSecondBit;				// CSI: 47 # <insert a comment here>
    	}
    else if(aHeader == EMsgComponentIdSubject)
    	{
    	results = iHeaderResults ^ KFourthBit;				// CSI: 47 # <insert a comment here>
    	}

    EHeaderStatus returnVal;

    if (results < iHeaderResults)
    	returnVal = EHeaderVisible;
    else
    	returnVal = EHeaderHidden;

    return returnVal;
    }

// ---------------------------------------------------------
// CMsgMailPreferences::SetAdditionalHeaders sets user configurable headers
// Returns: none
// ----------------------------------------------------
//
EXPORT_C void CMsgMailPreferences::SetAdditionalHeaders(
    TInt aValue) // new additional headers settings
    {
    iHeaderResults = aValue;
    }

// ----------------------------------------------------------------------------
// CMsgMailPreferences::SolveCharacterSet()
// ----------------------------------------------------------------------------
//
TUid CMsgMailPreferences::SolveCharacterSet( 
    const CImSmtpSettings& aSmtpSettings ) const
    {
    TUid characterSet = TUid::Uid(KCharacterSetIdentifierUtf8);
    // encoding supported, so read charset id from smtp settings.
    if ( iEncodingSupported )
        {
        characterSet = aSmtpSettings.DefaultMsgCharSet();             
        }
    else
        {                                  
	    if ( FeatureManager::FeatureSupported( KFeatureIdJapanese ) )
	        {
	        characterSet = TUid::Uid(
	            KCharacterSetIdentifierIso2022Jp );
	        }
        }
        
    return characterSet;        
    }    

//  End of File  

