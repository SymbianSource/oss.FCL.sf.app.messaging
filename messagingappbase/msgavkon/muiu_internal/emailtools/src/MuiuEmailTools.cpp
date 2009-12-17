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
* Description:  MuiuEmailTools implementation
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <cemailaccounts.h>         // CEmailAccounts
#include <msvstd.h>                 // TEntry
#include <msvids.h>                 // KMsvUnknownServiceIndexEntryId
#include <msvstd.hrh>               // KUidMsvServiceEntryValue
#include <miuthdr.h>                // TMsvEmailEntry
#include <MtmExtendedCapabilities.hrh> // Capabilities
#include <mtclreg.h>                // CClientMtmRegistry
#include <mtclbase.h>               // CBaseMtm
#include <mtuireg.h>
#include <muiuemailtools.h>

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// MuiuEmailTools::SetDefaultSendingMailboxL()
// ----------------------------------------------------------------------------
//
EXPORT_C void MuiuEmailTools::SetDefaultSendingMailboxL(    
    const TMsvId aSendingMailboxId )
    {
    CEmailAccounts* account = CEmailAccounts::NewLC();
    
    // Store the id
	TSmtpAccount sendingId;
	account->GetSmtpAccountL( aSendingMailboxId, sendingId );
	account->SetDefaultSmtpAccountL( sendingId );   		       
    
    CleanupStack::PopAndDestroy( account );        
    }
    
// ----------------------------------------------------------------------------
// MuiuEmailTools::DefaultSendingMailboxL()
// ----------------------------------------------------------------------------
//
EXPORT_C TMsvId MuiuEmailTools::DefaultSendingMailboxL( 
    CMsvSession& aMsvSession,
    TUid& aMtmId )
    {        
    // In case of basic mail mtm's, force the smtp as a searchable mtm
    TMsvId serviceId = KMsvUnknownServiceIndexEntryId;
    
    // If Imap4, Pop3 or Smtp mailbox, use CEmailAccounts to check the
    // default service
    if ( MuiuEmailTools::IsMailMtm( aMtmId ) )
        {
        TInt result = KErrNone;
        TSmtpAccount smtpId;
        aMtmId = KSenduiMtmSmtpUid;
        
        // Solve the mystery of the default mailbox
        CEmailAccounts* account = CEmailAccounts::NewLC();
        TRAPD( error, result = account->DefaultSmtpAccountL( smtpId ) );                    
        if ( error != KErrNone || result != KErrNone )
            {
            // Getting the default mailbox didn't succeed, 
            // return unknown service
            smtpId.iSmtpService = KMsvUnknownServiceIndexEntryId;
            }
        
        CleanupStack::PopAndDestroy( account );
        account = NULL;
        
        serviceId = smtpId.iSmtpService;
        }
    else
        {
        serviceId = QueryDefaultServiceL( aMsvSession, aMtmId );
        }
    
    // After the service has been specified, make sure it is valid    
    if ( !ValidateMailboxL( aMsvSession, aMtmId, serviceId ) )
        {
        serviceId = KMsvUnknownServiceIndexEntryId;
        }    
        
    return serviceId;
    }

// ----------------------------------------------------------------------------
// MuiuEmailTools::FindFirstValidMailboxL()
// ----------------------------------------------------------------------------
//
TMsvId MuiuEmailTools::FindFirstValidMailboxL( 
    CMsvSession& aMsvSession,
    const CMsvEntrySelection& aSelection,
    const TUid& aMtmId )
    {
    TMsvId serviceId = KMsvUnknownServiceIndexEntryId;
    
    // Prepare the mtm store and get the MtmUi of the desired mtmId
    CClientMtmRegistry* registry = CClientMtmRegistry::NewL( aMsvSession );
    CleanupStack::PushL( registry );
    CBaseMtm* mtm = registry->NewMtmL( aMtmId );
    CleanupStack::PushL( mtm );
    CMtmUiRegistry* uiRegistry = CMtmUiRegistry::NewL( aMsvSession );
    CleanupStack::PushL( uiRegistry );
    CBaseMtmUi* smtpMtmUi = uiRegistry->NewMtmUiL( *mtm );
    CleanupStack::PushL( smtpMtmUi );
    
    // Query from the mtm, if it is able to validate the service
    if ( QueryCapability( 
        KUidMsvMtmQuerySupportValidateSelection, *smtpMtmUi ) )
        {
        const TInt count = aSelection.Count();
        for ( TInt index = 0; 
              index < count && serviceId == KMsvUnknownServiceIndexEntryId; 
              index++ )
            {
            TMsvId mailbox = aSelection.At( index );
            serviceId = QueryMailboxValidityL( mailbox, *smtpMtmUi ) > 0 ?
                mailbox : KMsvUnknownServiceIndexEntryId;
            }        
        }
        
    CleanupStack::PopAndDestroy( 4, registry ); // registry, mtm, uiRegistry, smtpMtmUi
    
    return serviceId;
    }
        
// ----------------------------------------------------------------------------
// MuiuEmailTools::IsMailMtm()
// ----------------------------------------------------------------------------
//
EXPORT_C TBool MuiuEmailTools::IsMailMtm( 
    const TUid& aMtm,
    const TBool aAllowExtended )
    {                   
    // Basic mail mtm's   
    TBool basicMtms = 
        ( aMtm == KSenduiMtmSmtpUid ) || 
        ( aMtm == KSenduiMtmPop3Uid ) || 
        ( aMtm == KSenduiMtmImap4Uid );
    // extended mail mtm's
    TBool extendedMtms =
        ( aMtm == KSenduiMtmSyncMLEmailUid );
            
    // Returns ETrue, if the id is any of the following
    return basicMtms || ( aAllowExtended && extendedMtms );
    }     

// ----------------------------------------------------------------------------
// MuiuEmailTools::GetMailboxServiceId()
// ----------------------------------------------------------------------------
//
EXPORT_C TInt MuiuEmailTools::GetMailboxServiceId(
    CMsvSession& aMsvSession,
    TMsvEntry& aMailbox,
    const TMsvId aMailboxId,    
    const TBool aGetSendingService )
    {
    // Get the entry based on mailbox id    
    TMsvId serviceId;            
    TInt error = aMsvSession.GetEntry( aMailboxId, serviceId, aMailbox );
    
    TInt32 mtm = aMailbox.iMtm.iUid;
    
    // Continue handling, if successful
    if ( error == KErrNone && 
         aMailbox.iType.iUid == KUidMsvServiceEntryValue && 
         IsMailMtm( aMailbox.iMtm ) )  
        {        
        // Get related entry, if:
        // Mailbox type is smtp and pop3 or imap4 is wanted OR
        // Mailbox type is pop3 or imap4 and smtp is wanted
        if ( aGetSendingService && mtm != KSenduiMtmSmtpUidValue || 
            !aGetSendingService && mtm == KSenduiMtmSmtpUidValue )
            {
            error = aMsvSession.GetEntry( aMailbox.iRelatedId, serviceId, aMailbox );
            }
        }

    return error;
    }    

// ----------------------------------------------------------------------------
// MuiuEmailTools::ValidateMailboxL()
// ----------------------------------------------------------------------------
//
TBool MuiuEmailTools::ValidateMailboxL( 
    CMsvSession& aMsvSession,
    const TUid& aMtmId,
    const TMsvId aMailboxId )
    {
    // Prepare the mtm store and get the MtmUi of the desired mtmId
    TBool validity = EFalse;

    CClientMtmRegistry* registry = CClientMtmRegistry::NewL( aMsvSession );
    CleanupStack::PushL( registry );
    CBaseMtm* mtm = registry->NewMtmL( aMtmId );
    CleanupStack::PushL( mtm );
    CMtmUiRegistry* uiRegistry = CMtmUiRegistry::NewL( aMsvSession );
    CleanupStack::PushL( uiRegistry );
    CBaseMtmUi* smtpMtmUi = uiRegistry->NewMtmUiL( *mtm );
    CleanupStack::PushL( smtpMtmUi );

    // Query from the mtm, if it is able to validate the service
    if ( QueryCapability( 
        KUidMsvMtmQuerySupportValidateSelection, *smtpMtmUi ) )
        {
        validity = QueryMailboxValidityL( aMailboxId, *smtpMtmUi ) > 0;
        }
        
    CleanupStack::PopAndDestroy( 4, registry ); // registry, mtm, uiRegistry, smtpMtmUi
    
    return validity;
    }
    
// ----------------------------------------------------------------------------
// MuiuEmailTools::QueryCapability()
// ----------------------------------------------------------------------------
//
TBool MuiuEmailTools::QueryCapability( const TInt aQuery, CBaseMtmUi& aMtmUi )
    {
    TInt response = EFalse;
    TUid capabilityId = TUid::Uid( aQuery ); 
    
    // Query the capability from the Mtm
    TInt error = aMtmUi.QueryCapability( capabilityId, response );
    
    // Result is ok, when no error has occurred
    return !error && response;
    }    

// ----------------------------------------------------------------------------
// MuiuEmailTools::QueryMailboxValidityL()
// ----------------------------------------------------------------------------
//
TInt MuiuEmailTools::QueryMailboxValidityL( 
    const TMsvId aMailboxId,
    CBaseMtmUi& aMtmUi )
    {
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection();
    CleanupStack::PushL( selection );
    selection->AppendL( aMailboxId );
    TPckgBuf<TInt> parameter = EFalse;    
    
    // Invoke mtm's sync function
    aMtmUi.InvokeSyncFunctionL( 
        KUidMsvMtmQuerySupportValidateSelection, *selection, parameter );

    CleanupStack::PopAndDestroy( selection );
        
    return ( parameter() == KErrNone );        
    }  
    
    
// ----------------------------------------------------------------------------
// MuiuEmailTools::QueryDefaultServiceL()
// ----------------------------------------------------------------------------
//
TMsvId MuiuEmailTools::QueryDefaultServiceL( 
    CMsvSession& aMsvSession, 
    const TUid& aMtmId )
    {
    TMsvId serviceId = KMsvUnknownServiceIndexEntryId;
    CClientMtmRegistry* registry = CClientMtmRegistry::NewL( aMsvSession );
    CleanupStack::PushL( registry );
    CBaseMtm* mtm = registry->NewMtmL( aMtmId );
    CleanupStack::PushL( mtm );
    TRAPD( error, serviceId = mtm->DefaultServiceL() );
    if( error != KErrNone )
        {
        serviceId = KMsvUnknownServiceIndexEntryId;
        }
        
    CleanupStack::PopAndDestroy( 2, registry ); 
    registry = NULL;
    mtm = NULL;
    
    return serviceId;
    }           
            
// End of File
