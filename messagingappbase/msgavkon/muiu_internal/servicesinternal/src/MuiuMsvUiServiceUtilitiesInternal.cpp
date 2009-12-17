/*
* Copyright (c) 2002 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*     Static class to help mtm service queries.
*
*/




// INCLUDE FILES
#include <e32base.h>
#include <bldvariant.hrh>               // For feature flags  
#include <msvuids.h>                    // KUidMsvServiceEntry
#include <data_caging_path_literals.hrh> // KDC_RESOURCE_FILES_DIR

#include <msvapi.h>
#include <msvstd.hrh>
#include <msvids.h>
#include <mtclreg.h>                    // CClientMtmRegistry
#include <muiu_internal.rsg>
#include <bautils.h>
#include <AknPhoneNumberGrouping.h>     // CAknPhoneNumberGrouping
#include "MsgOperationWait.h"           // CMsgOperationWait 
#include <muiumsvuiserviceutilitiesinternal.h>
#include "MuiuCallCmd.h"
#include "MuiuSendKeyAcceptingQueryDialog.h"
#include "MuiuListQueryDialog.h"        // CMuiuListQueryDialog
#include "MuiuEmailTools.h"
#include <centralrepository.h>          // link against centralrepository.lib
#include <messaginginternalcrkeys.h>    // for Central Repository keys
#include <CoreApplicationUIsSDKCRKeys.h>
#include <telconfigcrkeys.h>            // KCRUidTelConfiguration,KTelMatchDigits
#include <mmsgenutils.h>
#include <msgvoipextension.h>
#include <MuiuMsvUiServiceUtilities.h>  // MsvUiServiceUtilities
#include <featmgr.h>
#include <MessagingDomainCRKeys.h>
#include <SendUiConsts.h>
#include <mtudreg.h>
#include <miutset.h>
#include <SendUiConsts.h>

// CONSTANTS
const TInt KDefaultParamValueLength  = -1;   // Default aCompareLength parameter value
                                             // in ComparePhoneNumberL method
const TInt KDefaultCompareLength     = 7;    // Used in ComparePhoneNumberL method
                                             // if aCompareLength parameter has default value
const TInt KEqualCompareValue        = 0;    // CompareF returns 0 if phone numbers are equal
const TInt KFirstItemInList          = 0;
const TInt KHotswapWaitInterval      = 1000000;   // 1 second
const TInt KHotswapWaitMax           = 7;         // 7 number of turns(cycle time KHotswapWaitInterval)
_LIT( KDirAndMuiuResFileName,"muiu_internal.rsc" );



// ---------------------------------------------------------
// MsvUiServiceUtilitiesInternal::CallToSenderQueryL
//
// Used when asking from user if calling to sender of the message and dial
// if user select "Yes".  Check immediately if phone number is valid. If phone number
// is not valid then return EFalse and doesn't do nothing.
// If parameter "aDialWithoutQueries" is true then call immediately to sender of the message 
// and doesn't ask nothing from user before it.
// Returns:  Returns ETrue if the phone number was valid.
// 
// ---------------------------------------------------------
//
EXPORT_C TBool MsvUiServiceUtilitiesInternal::CallToSenderQueryL( const TDesC& aNumber, 
                                                          const TDesC& aName, 
                                                          TBool aDialWithoutQueries )
    {
    TBool validNumber( EFalse );	//Is phone number valid or not
    
    HBufC* buf = HBufC::NewLC( aNumber.Length() );
    TPtr   bufNumberPtr = buf->Des();
    bufNumberPtr = aNumber;
    validNumber = CommonPhoneParser::ParsePhoneNumber( bufNumberPtr, 
                                                       CommonPhoneParser::EPhoneClientNumber );
    if( validNumber )
        {
        if( aDialWithoutQueries )
            {
            //Start dialing and open Calling dialog
            CMuiuCallCmd* cMuiuCallCmdPtr = CMuiuCallCmd::NewL();
            cMuiuCallCmdPtr->ExecuteLD( bufNumberPtr, aName );
            }
        else
            {
            CMuiuSendKeyAcceptingQueryDialog* dlg = 
                                              CMuiuSendKeyAcceptingQueryDialog::NewL( aNumber, 
                                                                                      aName );
            if( dlg->ExecuteLD( R_MUIU_CALLBACK_QUERY ) )
                {
                //Start dialing and open Calling dialog
                CMuiuCallCmd* cMuiuCallCmdPtr = CMuiuCallCmd::NewL();
                cMuiuCallCmdPtr->ExecuteLD( bufNumberPtr, aName );
                }
            }
        }
    CleanupStack::PopAndDestroy( buf );
    return validNumber;
    }

// ---------------------------------------------------------
// MsvUiServiceUtilitiesInternal::CallServiceL
//
// ---------------------------------------------------------
//
EXPORT_C TBool MsvUiServiceUtilitiesInternal::CallServiceL( 
            const TDesC& aSenderNumber,
            const TDesC& aSenderName,
            const TDesC& aNumberInFocus,
            CEikonEnv* aEnv )
    {
    TBool validNumber( EFalse );

    // Is list query needed
    if ( aNumberInFocus.Length() && aSenderNumber.Length() )
        { 
        // Two numbers, list query is needed
        HBufC* buf = HBufC::NewLC( aNumberInFocus.Length() );
        TPtr   bufNumberPtr = buf->Des();
        bufNumberPtr = aNumberInFocus;
        CommonPhoneParser::ParsePhoneNumber( bufNumberPtr, 
                                             CommonPhoneParser::EPhoneClientNumber );
        HBufC* groupedNumber = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( bufNumberPtr );
        CleanupStack::PushL( groupedNumber );
        TPtr groupedNumberPtr = groupedNumber->Des();

        // Adding MUIU's resource-file
        TParse parse;
        parse.Set(KDirAndMuiuResFileName, &KDC_RESOURCE_FILES_DIR, NULL); 
        TFileName resourceFile(parse.FullName());

        if ( !aEnv )
            {
            aEnv  = CEikonEnv::Static();
            }
        BaflUtils::NearestLanguageFile( aEnv->FsSession(), resourceFile );
        TInt offset = aEnv->AddResourceFileL( resourceFile );

        // TRAPD to be sure that resource-file gets also deleted
        TInt index = 0;
        TInt commandId( 0 );
        TRAPD( err,commandId = LaunchDialogL( groupedNumberPtr, &index ) );
        // Removing MUIU's resource-file
        aEnv->DeleteResourceFile( offset );
        CleanupStack::PopAndDestroy( 2, buf ); //groupedNumber, buf
        User::LeaveIfError( err );
        if ( ( commandId == EAknSoftkeyCall ) || ( commandId == EEikBidOk ) )
            { 
            if ( index == 0 )
                {// number in focus
                validNumber = CallToSenderQueryL( aNumberInFocus, KNullDesC );
                }
            else
                {// back to sender
                validNumber = CallToSenderQueryL( aSenderNumber, aSenderName );
                }
            }
        }
    else if ( aSenderNumber.Length() )
        {
        // Sender only, list query is not needed

        // Launch the normal call query
        validNumber = CallToSenderQueryL( aSenderNumber, aSenderName, EFalse );
        }
    else if ( aNumberInFocus.Length() )
        {
        // Focused number only, list query is not needed

        // Launch the normal call query
        validNumber = CallToSenderQueryL( aNumberInFocus, KNullDesC(), EFalse );
        }
    else
        {
        // No numbers
        validNumber = EFalse;
        }

    return validNumber;
    }

// ---------------------------------------------------------
// MsvUiServiceUtilitiesInternal::InternetCallToSenderQueryL
//
// Used when asking from user if calling to sender of the message and dial
// if user select "Yes". Assumes allways that Internet call number is valid and 
// ETrue returned.
// If parameter "aDialWithoutQueries" is true then call immediately to sender of 
// the message and doesn't ask nothing from user before it.
// Returns:  Returns ETrue.
// 
// ---------------------------------------------------------
//
EXPORT_C TBool MsvUiServiceUtilitiesInternal::InternetCallToSenderQueryL( const TDesC& aNumber, 
                                                                  const TDesC& aName, 
                                                                  TBool aDialWithoutQueries )
    {
    //If the SIP URI is not valid then check is the selected number valid CS number.
    if( aDialWithoutQueries )
        {
        //Start dialing and open Calling dialog
        CMuiuCallCmd* cMuiuCallCmdPtr = CMuiuCallCmd::NewL();
        cMuiuCallCmdPtr->ExecuteLD( aNumber, aName, ETrue );
        }
    else
        {
        CMuiuSendKeyAcceptingQueryDialog* dlg = CMuiuSendKeyAcceptingQueryDialog::NewL( aNumber, 
                                                                                        aName );
        if( dlg->ExecuteLD( R_MUIU_CALLBACK_QUERY ) )
            {
            //Start dialing and open Calling dialog
            CMuiuCallCmd* cMuiuCallCmdPtr = CMuiuCallCmd::NewL();
            cMuiuCallCmdPtr->ExecuteLD( aNumber, aName, ETrue );
            }
        }
    return ETrue;
    }
    
// ---------------------------------------------------------
// MsvUiServiceUtilitiesInternal::InternetCallServiceL
//
// ---------------------------------------------------------
//
EXPORT_C TBool MsvUiServiceUtilitiesInternal::InternetCallServiceL( 
            const TDesC& aSenderNumber,
            const TDesC& aSenderName,
            const TDesC& aNumberInFocus,
            CEikonEnv* aEnv )
    {
        TBool validNumber( EFalse );

    // Is list query needed
    if ( aNumberInFocus.Length() && aSenderNumber.Length() )
        { 
        // Two numbers, list query is needed
        HBufC* buf = HBufC::NewLC( aNumberInFocus.Length() );
        TPtr   bufNumberPtr = buf->Des();
        bufNumberPtr = aNumberInFocus;
        CommonPhoneParser::ParsePhoneNumber( bufNumberPtr, 
                                             CommonPhoneParser::EPhoneClientNumber );
        HBufC* groupedNumber = CAknPhoneNumberGrouping::CreateGroupedPhoneNumberL( bufNumberPtr );
        CleanupStack::PushL( groupedNumber );
        TPtr groupedNumberPtr = groupedNumber->Des();

        // Adding MUIU's resource-file
        TParse parse;
        parse.Set(KDirAndMuiuResFileName, &KDC_RESOURCE_FILES_DIR, NULL); 
        TFileName resourceFile(parse.FullName());

        if ( !aEnv )
            {
            aEnv  = CEikonEnv::Static();
            }
        BaflUtils::NearestLanguageFile( aEnv->FsSession(), resourceFile );
        TInt offset = aEnv->AddResourceFileL( resourceFile );

        // TRAPD to be sure that resource-file gets also deleted
        TInt index = 0;
        TInt commandId( 0 );
        TRAPD( err,commandId = LaunchDialogL( groupedNumberPtr, &index ) );
        // Removing MUIU's resource-file
        aEnv->DeleteResourceFile( offset );
        CleanupStack::PopAndDestroy( 2, buf ); //groupedNumber, buf
        User::LeaveIfError( err );
        if ( ( commandId == EAknSoftkeyCall ) || ( commandId == EEikBidOk ) )
            { 
            if ( index == 0 )
                {// number in focus
                validNumber = InternetCallToSenderQueryL( aNumberInFocus, KNullDesC );
                }
            else
                {               
                // call CS call to sender              
                validNumber = CallToSenderQueryL( aSenderNumber, aSenderName );  
                }
            }
        }
    else if ( aSenderNumber.Length() )
        {
        // Sender only, list query is not needed

        // Launch the normal call query
        validNumber = InternetCallToSenderQueryL( aSenderNumber, aSenderName, EFalse );
        }
    else if ( aNumberInFocus.Length() )
        {
        // Focused number only, list query is not needed

        // Launch the normal call query
        validNumber = InternetCallToSenderQueryL( aNumberInFocus, KNullDesC(), EFalse );
        }
    else
        {
        // No numbers
        validNumber = EFalse;
        }

    return validNumber;
    }


// ----------------------------------------------------
// MsvUiServiceUtilitiesInternal::ChangeMessageStoreToPhoneL
// ----------------------------------------------------
EXPORT_C void MsvUiServiceUtilitiesInternal::ChangeMessageStoreToPhoneL( CMsvSession& aSession )
    {
    //check if message server is busy
    TInt outstanding = 1;
    for ( TInt jj = 0;  jj < KHotswapWaitMax  && outstanding; jj++ )
        {
        outstanding = aSession.OutstandingOperationsL();
        if ( outstanding == 0 )
            {
            // check to see if any mailboxes are open - 
            // otherwise we could miss client side operations
            CMsvEntry* root=CMsvEntry::NewL( aSession, KMsvRootIndexEntryIdValue, 
                TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
            CleanupStack::PushL( root );
            CMsvEntrySelection* sel=root->ChildrenWithTypeL( KUidMsvServiceEntry );
            CleanupStack::PushL( sel );
            const TMsvEntry* tentry=NULL;
            for ( TInt cc=sel->Count(); --cc>=0 && !outstanding; )
                {
                tentry=&( root->ChildDataL( ( *sel )[cc]) );
                if ( tentry->Connected() )
                    {
                    outstanding++;
                    }
                }

            CleanupStack::PopAndDestroy( 2, root ); //sel, root                
            }
        if ( outstanding > 0 )
            {
            User::After( KHotswapWaitInterval ); //wait one second
            }
        }
        
    CMsgOperationWait* waiter = CMsgOperationWait::NewLC();
    CMsvOperation* op = aSession.ChangeDriveL( EDriveC, waiter->iStatus );
    CleanupStack::PushL( op );
    waiter->Start();        
    CleanupStack::PopAndDestroy( 2, waiter ); //waiter, op
    }


// ----------------------------------------------------
// MsvUiServiceUtilitiesInternal::MmcDriveInfoL
// ----------------------------------------------------
EXPORT_C TBool MsvUiServiceUtilitiesInternal::MmcDriveInfoL( CMsvSession& aSession )
    {
    return aSession.MessageStoreDrivePresentL();
    }

// ---------------------------------------------------------
// MsvUiServiceUtilitiesInternal::DefaultServiceForMTML
// ---------------------------------------------------------
//
EXPORT_C TMsvId MsvUiServiceUtilitiesInternal::DefaultServiceForMTML( 
    CMsvSession& aSession, 
    TUid aMtm, 
    TBool aFindFirstServiceIfNoDefault )
    {
    FeatureManager::InitializeLibL();
    TBool selectableEmail =
        FeatureManager::FeatureSupported( KFeatureIdSelectableEmail );
    TBool emailFramework =
        FeatureManager::FeatureSupported( KFeatureIdFfEmailFramework );
    FeatureManager::UnInitializeLib();
    TMsvId serviceId = KMsvUnknownServiceIndexEntryId;
    
    // Default mailbox is fetched seperately by using the API provided by symbian
    // If selectableEmail feature is on, get the default email 
    // account from the central repository  
    if ( selectableEmail && !emailFramework && MuiuEmailTools::IsMailMtm( aMtm ) )
    	{
    	serviceId = MuiuEmailTools::DefaultSendingMailboxL( aSession, aMtm );

    	if ( serviceId == KMsvUnknownServiceIndexEntryId )
    		{
    		CRepository* repository = NULL;
    		TRAPD( ret, repository = CRepository::NewL(
    				                KCRUidSelectableDefaultEmailSettings ) );
    		CleanupStack::PushL( repository );

    		if ( ret == KErrNone )
    			{
    			TInt entryId;
    				if ( repository->Get( KSelectableDefaultMailAccount ,
    						              entryId ) != KErrNone )
    				{
    				serviceId = KMsvUnknownServiceIndexEntryId;    		        
    				}
    			else
    				{
    				serviceId = entryId;    
    				}    		        
    			}

    		CleanupStack::PopAndDestroy();            
    		}

    	// Bug workaround, validate that the mailbox exist
    	TMsvEntry mailbox;
    	TMsvId dummyId;

    	if ( aSession.GetEntry( serviceId, dummyId, mailbox ) != KErrNone )
    		{
    		serviceId = KMsvUnknownServiceIndexEntryId;
    		aFindFirstServiceIfNoDefault = ETrue;
    		}
    	}
    // if selectable email feature is off, Default mailbox is fetched 
    // seperately by using the API provided by symbian
    else if ( !selectableEmail && MuiuEmailTools::IsMailMtm( aMtm, ETrue ) )
    	{                
    	// Get service id, which will always be validated
    	serviceId = MuiuEmailTools::DefaultSendingMailboxL( aSession, aMtm );

    	aFindFirstServiceIfNoDefault &=             
    	( serviceId == KMsvUnknownServiceIndexEntryId );
    	}	
    // Other mtm's use this path
    else
    	{
    	CClientMtmRegistry* registry = CClientMtmRegistry::NewL( aSession );
    	CleanupStack::PushL( registry );
    	CBaseMtm* mtm = registry->NewMtmL( aMtm );
    	CleanupStack::PushL( mtm );
    	TRAPD( error, serviceId = mtm->DefaultServiceL() );
    	if( error != KErrNone )
    		{
    		serviceId = KMsvUnknownServiceIndexEntryId;
    		}
        CleanupStack::PopAndDestroy( 2 ); // registry, mtm
        registry = NULL;
        mtm = NULL;
        }    

    if ( aFindFirstServiceIfNoDefault && 
    	 serviceId == KMsvUnknownServiceIndexEntryId ) 
    	{
    	CMsvEntry* entry = aSession.GetEntryL( KMsvRootIndexEntryIdValue );
    	CleanupStack::PushL( entry );
    	CMsvEntrySelection* sel = entry->ChildrenWithMtmL( aMtm );
    	CleanupStack::PopAndDestroy( entry );
    	CleanupStack::PushL( sel );
    	entry = NULL;

    	// Check if mail mtm
    		if ( MuiuEmailTools::IsMailMtm( aMtm, ETrue ) )
    			{
    			// Mail services needs to be validated, provide the array 
    			// of services for validation
    			serviceId = MuiuEmailTools::FindFirstValidMailboxL(
    					    aSession, *sel, aMtm );
    			}
    		else
    			{
    			// Just take the first available service from the list
    			if ( sel->Count() > 0 )
    				{
    				serviceId = sel->At( 0 );
    				}            
    			}            

    		CleanupStack::PopAndDestroy( sel );
    		sel = NULL;
    		}
    
	return serviceId;    
    }


// ---------------------------------------------------------
// MsvUiServiceUtilitiesInternal::SetDefaultServiceForMTML
// ---------------------------------------------------------
//
EXPORT_C void MsvUiServiceUtilitiesInternal::SetDefaultServiceForMTML( 
    CMsvSession& aSession, 
    TUid aMtm, 
    TMsvId aServiceId )
    {
    FeatureManager::InitializeLibL();
    if ( MuiuEmailTools::IsMailMtm( aMtm ) )
        {
        TMsvEntry mailbox;
        TInt error = MuiuEmailTools::GetMailboxServiceId(
            aSession, mailbox, aServiceId, ETrue );
            
        if ( error == KErrNone )
            {            
            MuiuEmailTools::SetDefaultSendingMailboxL( 
                mailbox.Id() );
            }
        }
    else if ( FeatureManager::FeatureSupported( KFeatureIdSelectableEmail )
            && aMtm != KSenduiMtmSyncMLEmailUid )
    	{
    	TMsvEntry entry;
    	TMsvId serviceId;

    	User::LeaveIfError( aSession.GetEntry(
    			                            aServiceId, serviceId, entry ) );

    	TBool appendEmail = EFalse;						            

    	if( entry.Visible() )
    		{
    		appendEmail = ETrue;
    		}
    	else
    		{
    		CMtmUiDataRegistry* uiRegistry =
    		                            CMtmUiDataRegistry::NewL( aSession );                
    		CleanupStack::PushL( uiRegistry );
    		if ( uiRegistry->IsPresent( entry.iMtm ) &&
    				               uiRegistry->IsPresent( KUidMsgTypePOP3 ) )
    			{                                                      
    			if ( uiRegistry->TechnologyTypeUid( entry.iMtm ) ==
    			                                   KSenduiTechnologyMailUid )
    				{
    				appendEmail = ETrue;                    
    				}
    			}
    		CleanupStack::PopAndDestroy( uiRegistry );
    		} 

    	if ( appendEmail || (MuiuEmailTools::IsMailMtm( aMtm ) ) )
    		{        
    		CRepository* repository = NULL;
    		TRAPD( ret, repository = CRepository::NewL(
    				                KCRUidSelectableDefaultEmailSettings ) );
    		CleanupStack::PushL( repository );

    		if ( ret == KErrNone )
    			{
    			TInt entryId = entry.Id(); 
    			if ( entry.iMtm == KSenduiMtmPop3Uid ||
    			     entry.iMtm == KSenduiMtmImap4Uid )		               
    				{
    				entryId = entry.iRelatedId;
    				}
    			repository->Set( KSelectableDefaultMailAccount , entryId );
    			}
    		CleanupStack::PopAndDestroy(); // repository         
    		}
    	}
    else
    	{
    	CClientMtmRegistry* registry = CClientMtmRegistry::NewL( aSession );
    	CleanupStack::PushL( registry );
    	CBaseMtm* mtm = registry->NewMtmL( aMtm );
    	CleanupStack::PushL( mtm );
    	mtm->ChangeDefaultServiceL( aServiceId );
        CleanupStack::PopAndDestroy( mtm );
    	CleanupStack::PopAndDestroy( registry );        
    	}

    FeatureManager::UnInitializeLib();    
    }
    
    
// ---------------------------------------------------------
// MsvUiServiceUtilitiesInternal::IsPhoneOfflineL
// ---------------------------------------------------------
//
EXPORT_C TBool MsvUiServiceUtilitiesInternal::IsPhoneOfflineL()
    {
    TInt connAllowed ( 1 );
    CRepository* repository ( CRepository::NewL( KCRUidCoreApplicationUIs ) );
    TInt err = repository->Get( KCoreAppUIsNetworkConnectionAllowed, connAllowed );
    delete repository;
    repository = NULL;
    if ( !err && !connAllowed )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------
// MsvUiServiceUtilitiesInternal::InternetOrVoiceCallServiceL
// ---------------------------------------------------------
//
EXPORT_C TBool MsvUiServiceUtilitiesInternal::InternetOrVoiceCallServiceL( 
    const CMsgVoIPExtension&    aVoIPExtension,
    const TDesC&                aSenderAddress,
    const TDesC&                aSenderName,
    const TDesC&                aAddressInFocus,
    TBool                       aDialWithoutQueries,
    CEikonEnv*                  aEnv )
    {    
    if ( !aEnv )
        {
        aEnv  = CEikonEnv::Static();
        }
    return InternetOrVoiceCallServiceL( *aEnv,
                                        aVoIPExtension,
                                        aSenderAddress,
                                        aSenderName,
                                        ResolveAddressTypeL( aSenderAddress ),   
                                        aAddressInFocus,
                                        ResolveAddressTypeL( aAddressInFocus ),  
                                        aDialWithoutQueries );
    }

// ---------------------------------------------------------
// MsvUiServiceUtilitiesInternal::InternetOrVoiceCallServiceL
// ---------------------------------------------------------
//
EXPORT_C TBool MsvUiServiceUtilitiesInternal::InternetOrVoiceCallServiceL( 
    CEikonEnv&                  aEnv,
    const CMsgVoIPExtension&    aVoIPExtension,
    const TDesC&                aSenderAddress,
    const TDesC&                aSenderName,
    TMuiuAddressType            aSenderAddressType,
    const TDesC&                aAddressInFocus,
    TMuiuAddressType            aAddressInFocusTypeType,
    TBool                       aDialWithoutQueries )
    {                
    FeatureManager::InitializeLibL();
    TBool VoIPFeatureSupport = EFalse;
    //checking If VOIP is enabled
    if( FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
        {
        VoIPFeatureSupport = ETrue;
        }
    FeatureManager::UnInitializeLib();

    if ( aSenderAddressType || aAddressInFocusTypeType )
        {
            // - if voip number is focused then voip call should be possible 
            //   even though preferred telephony is set to CS
            // - Preferred tel not supported in Sawfish
            if((VoIPFeatureSupport)
               &&( aVoIPExtension.VoIPProfilesExistL() 
               && aAddressInFocusTypeType == EMuiuAddressTypeEmail) )
               {
                if ( aAddressInFocusTypeType )
                {
                return MsvUiServiceUtilitiesInternal::InternetCallServiceL( 
                    aSenderAddress, 
                    aSenderName,
                    aAddressInFocus,
                    &aEnv );
                }
                else if ( aSenderAddressType )
                        {
                        return MsvUiServiceUtilitiesInternal::InternetCallToSenderQueryL(
                            aSenderAddress,
                            aSenderName,
                            aDialWithoutQueries );
                        }
                }
            if((!VoIPFeatureSupport)
                &&(aVoIPExtension.IsPreferredTelephonyVoIP()
                &&  aVoIPExtension.VoIPProfilesExistL()) )
                {
                if ( aAddressInFocusTypeType )
                    {
                return MsvUiServiceUtilitiesInternal::InternetCallServiceL( 
                    aSenderAddress, 
                    aSenderName,
                    aAddressInFocus,
                    &aEnv );
                }
            else if ( aSenderAddressType )
                {
                return MsvUiServiceUtilitiesInternal::InternetCallToSenderQueryL(
                    aSenderAddress,
                    aSenderName,
                    aDialWithoutQueries );
                }
            }
        else // as first choice try voice call 
            {
            // Voice call is not possible with email address
            TPtrC senderAddress = aSenderAddress;
            TPtrC senderName = aSenderName;
            TPtrC focusAddress = aAddressInFocus;

            if ( aSenderAddressType != EMuiuAddressTypePhoneNumber )
                {
                senderAddress.Set( KNullDesC( ) );
                senderName.Set( KNullDesC( ) );
                }
            if ( aAddressInFocusTypeType != EMuiuAddressTypePhoneNumber )
                {
                focusAddress.Set( KNullDesC( ) );
                }
            
            if ( aAddressInFocusTypeType == EMuiuAddressTypePhoneNumber )
                {
                return MsvUiServiceUtilitiesInternal::CallServiceL( 
                    senderAddress, 
                    senderName,
                    focusAddress,
                    &aEnv );
                }
            else if ( aSenderAddressType == EMuiuAddressTypePhoneNumber )
                {
                return MsvUiServiceUtilitiesInternal::CallToSenderQueryL(
                    senderAddress,
                    senderName,
                    aDialWithoutQueries );
                }
            // else - voice call was not possible. VOIP could still be possible
            // but it is not tried. After future feature 'contact matching' the code 
            // would change anyway
            }
        }
    return EFalse;
    }


// ---------------------------------------------------------
// MsvUiServiceUtilitiesInternal::ResolveAddressTypeL
// ---------------------------------------------------------
//
EXPORT_C TMuiuAddressType MsvUiServiceUtilitiesInternal::ResolveAddressTypeL(
                             const TDesC&                        aAddress,
                             CommonPhoneParser::TPhoneNumberType aDefaultPhoneNumberType )
    {
    TMuiuAddressType addressType = CommonPhoneParser::IsValidPhoneNumber( 
                     aAddress, 
                     aDefaultPhoneNumberType ) ? EMuiuAddressTypePhoneNumber : EMuiuAddressTypeNone;
    if ( addressType == EMuiuAddressTypeNone )
        {
        addressType = MsvUiServiceUtilities::IsValidEmailAddressL( aAddress ) ?
                            EMuiuAddressTypeEmail : EMuiuAddressTypeNone;
        }
    return addressType;
    }
    
    
// ---------------------------------------------------------
// MsvUiServiceUtilitiesInternal::LaunchDialogL
//
// ---------------------------------------------------------
//
TInt MsvUiServiceUtilitiesInternal::LaunchDialogL( const TPtr aGroupedNumberPtr, TInt* aIndex )
    {
    TInt commandId( 0 );
    CMuiuListQueryDialog* dlg = new (ELeave) CMuiuListQueryDialog( aIndex );
    dlg->PrepareLC( R_CALL_TO_SENDER_LIST_QUERY );
    (( CDesCArrayFlat* )(( CTextListBoxModel* )dlg->ListBox()->Model())->ItemTextArray())->InsertL( KFirstItemInList, aGroupedNumberPtr );
    commandId = dlg->RunLD();
    return commandId;
    }

// ---------------------------------------------------------
// MsvUiServiceUtilitiesInternal::ConvertUtcToLocalTime
//
// ---------------------------------------------------------
//
EXPORT_C void MsvUiServiceUtilitiesInternal::ConvertUtcToLocalTime( TTime& aTime )
    {
    TLocale locale;
    
    // Add time difference
    aTime += locale.UniversalTimeOffset();
    if ( locale.QueryHomeHasDaylightSavingOn() )
        { 
        // and possible daylight saving time
        TTimeIntervalHours daylightSaving(1);          
        aTime += daylightSaving;
        }
    }
    
    
// ---------------------------------------------------------
// MsvUiServiceUtilitiesInternal::ComparePhoneNumberL
//
// ---------------------------------------------------------
//
EXPORT_C TBool MsvUiServiceUtilitiesInternal::ComparePhoneNumberL(
    const TDesC& aNumber1, const TDesC& aNumber2, TInt aCompareLength )
    {
    if ( aCompareLength == KDefaultParamValueLength )
        {
        // default to 7
        aCompareLength = KDefaultCompareLength;
        CRepository* repository = CRepository::NewL( KCRUidTelConfiguration );
       	repository->Get( KTelMatchDigits, aCompareLength );
        delete repository;
        }
    HBufC* number1C = aNumber1.AllocLC();
    HBufC* number2C = aNumber2.AllocLC();
    TPtr number1 = number1C->Des();
    TPtr number2 = number2C->Des();

    // Remove "()- " from numbers.
    if ( !CommonPhoneParser::ParsePhoneNumber(
        number1, CommonPhoneParser::EPlainPhoneNumber ) )
        {
        CleanupStack::PopAndDestroy( 2 ); // number1C, number2C
        return EFalse;
        }

    if ( !CommonPhoneParser::ParsePhoneNumber(
        number2, CommonPhoneParser::EPlainPhoneNumber ) )
        {
        CleanupStack::PopAndDestroy( 2 ); // number1C, number2C
        return EFalse;
        }

    // Check if first char is "+" and remove it.
    // NOTE: Does not handle "#*pPwW" chars, these should cause
    // "incorrect recipient"-note already earlier.
    const TChar plus = '+';

    TInt res = number1.Locate( plus );
    if ( res != KErrNotFound )
        {
        number1.Delete( res, 1 );
        }

    res = number2.Locate( plus );
    if ( res != KErrNotFound )
        {
        number2.Delete( res, 1 );
        }

    TInt number1Length = number1.Length();
    TInt number2Length = number2.Length();

    // Check length.

    // If both numbers are same length, do not truncate, but compare as is.
    // case: 040 1234567 and 050 1234567 are different!
    if ( number1Length != number2Length )
        {
        // If longer than aCompareLength (default = 7) truncate from the beginning.
        // case: +358 50 1234567 and 050 1234567 are same!
        // TODO: +358 50 1234567 and 040 1234567 are still different!
        if ( number1Length > aCompareLength )
            {
            number1.Delete( 0, number1Length - aCompareLength );
            }

        if ( number2Length > aCompareLength )
            {
            number2.Delete( 0, number2Length - aCompareLength );
            }
        }

    TInt ret = number1.CompareF( number2 );

    CleanupStack::PopAndDestroy( 2, number1C ); // number1C, number2C

    return ( ret == KEqualCompareValue );
    }

// ----------------------------------------------------------------------------
// MsvUiServiceUtilitiesInternal::OtherEmailMTMExistL()
// ----------------------------------------------------------------------------
//
EXPORT_C TBool MsvUiServiceUtilitiesInternal::OtherEmailMTMExistL( CMsvSession& aSession, TInt aMailbox )
    {
    CMsvEntry* entry = aSession.GetEntryL( KMsvRootIndexEntryId );
    CleanupStack::PushL( entry );

    TInt cnt = entry->Count();
    if ( cnt != 0 )
        {
        entry->SetSortTypeL( TMsvSelectionOrdering( KMsvGroupByType | KMsvGroupByStandardFolders, EMsvSortByDetailsReverse, EFalse ) );

        // Add default item first
        for (TInt cc = entry->Count(); --cc >= 0; )
            {
            TMsvEntry tentry=(*entry)[cc];                

            if ( tentry.iType.iUid == KUidMsvServiceEntryValue && tentry.Id() != KMsvLocalServiceIndexEntryIdValue )
                {
                const TBool KnoRelatedId = tentry.iRelatedId == KMsvNullIndexEntryId || tentry.iRelatedId == tentry.Id();

                TBool appendEmail = EFalse;

                if( tentry.Visible() )
                    {
                    appendEmail = ETrue;
                    }
                else
                    {
                    CMtmUiDataRegistry* uiRegistry = CMtmUiDataRegistry::NewL(aSession);                
                    CleanupStack::PushL(uiRegistry);
                    if ( uiRegistry->IsPresent( tentry.iMtm ) && uiRegistry->IsPresent( KUidMsgTypePOP3 ) )
                        {            
                        if( uiRegistry->TechnologyTypeUid( tentry.iMtm ) == KSenduiTechnologyMailUid )
                            {
                            appendEmail = ETrue;                    
                            }
                        }
                    CleanupStack::PopAndDestroy(uiRegistry);
                    }  

                if ( appendEmail || KnoRelatedId )
                    {
                    if (aMailbox != 0)
                        {
                        if( aMailbox != tentry.Id() && aMailbox != tentry.iRelatedId )
                            {                        
                            CleanupStack::PopAndDestroy( entry );
                            return ETrue;                            
                            }
                        }
                    else 
                        {
                        if( tentry.iMtm != KUidMsgTypePOP3 && tentry.iMtm != KUidMsgTypeIMAP4
                                && tentry.iMtm != KUidMsgTypeSMTP )
                            {
                            CleanupStack::PopAndDestroy( entry );                        
                            return ETrue;
                            }
                        }
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy( entry );        
    return EFalse;        
    }



// End of File
