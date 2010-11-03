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
*     An active object class which takes care of reading the possible new
*     service centres from SIM and adds them to pda-side Sms Settings.
*
*/



// INCLUDE FILES
#include <mtuireg.h>    // for CMtmUiRegistry
#include <etelmm.h>
#include <mmlist.h>
#include <smutset.h>    //CSmsSettings
#include <exterror.h>   // KErrGsmSMSSimBusy
#include <csmsaccount.h>
#include <centralrepository.h>
#include <messagingvariant.hrh>
#include <messaginginternalcrkeys.h>    // Keys

#include "SmumStartUpMonitor.h"
#include "SmumUtil.h"   // SmumUtil
#include "MsgSimSCNumberDetector.h"
#include "SmumLogging.h"
#include "smsui.pan"    // for panics

// CONSTANTS
const TInt KMaxNameLength = 60;
const TInt KSmumGranularity = 4;
const TInt KSmumRetryCount = 20;
const TInt KSmumRetryDelay = 5000000; // 5 secs
const TInt KSmumSMSPid = 50;
// ================= MEMBER FUNCTIONS =======================

/*
* Symbian OS 2 phase construction and initialization. Note that operation that is returned is already active.
* A client will be notified via aClientStatus when the operation terminates.
*/
EXPORT_C CMsgSimOperation* CMsgSimOperation::NewL( TRequestStatus& aClientStatus )
    {
    SMUMLOGGER_ENTERFN("CMsgSimOperation::NewL")

    CMsgSimOperation* self = new ( ELeave ) CMsgSimOperation( aClientStatus );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    SMUMLOGGER_LEAVEFN("CMsgSimOperation::NewL")

    return self;
    }

// C++ constructor which initializes the active object with a default priority and adds it to the active scheduler.
// C++ constructor can NOT contain any code, that
// might leave.
CMsgSimOperation::CMsgSimOperation( TRequestStatus& aStatus, TInt aPriority )
: CActive( aPriority ), iClientStatus( &aStatus ), iRetryCount( 0 )
    {
    SMUMLOGGER_ENTERFN("CMsgSimOperation::CMsgSimOperation")
    CActiveScheduler::Add( this );
    SMUMLOGGER_LEAVEFN("CMsgSimOperation::CMsgSimOperation")
    }

// Symbian OS default constructor can leave.
void CMsgSimOperation::ConstructL()
    {
    SMUMLOGGER_ENTERFN("CMsgSimOperation::ConstructL")
    
    // initialise
    iMsvSession = CMsvSession::OpenSyncL( *this ); // have to be sync; actual active object is started right after this, not in HandleSessionEventL
    iClientRegistry = CClientMtmRegistry::NewL( *iMsvSession );   // get client mtm registry
    iSmsClientMtm = STATIC_CAST( CSmsClientMtm*, iClientRegistry->NewMtmL( KUidMsgTypeSMS ));  // get smcm object

    SMUMLOGGER_WRITE("ConstructL - Basic initialising ok, next connecting to Shared Data")
	// Start the System state monitor
	iStartupMonitor = CSmumStartUpMonitor::NewL( this );
	// Client status can be set pending only if we don't leave
    *iClientStatus = KRequestPending;  // activate the active object
    // StartL() is called when iStartupMonitor ActiveObject completes
    //Check is sms servicecenter fetched in every boot
	iCheckSimScAlways = ( SmumUtil::CheckVariationFlagsL( KCRUidMuiuVariation, KMuiuSmsFeatures ) & 
			  KSmsFeatureIdCheckSimAlways );
    SMUMLOGGER_LEAVEFN("CMsgSimOperation::ConstructL")
    }

// C++ virtual destructor
CMsgSimOperation::~CMsgSimOperation()
    {
    SMUMLOGGER_ENTERFN("CMsgSimOperation::~CMsgSimOperation")

    Cancel(); // make sure that this object is not left on the active scheduler.
    delete iSimOperation;  
    delete iSmsClientMtm;
    delete iClientRegistry;
    delete iMsvSession;
	delete iStartupMonitor;
    iStartupMonitor = NULL;
    iClientStatus = NULL;
    
    SMUMLOGGER_LEAVEFN("CMsgSimOperation::~CMsgSimOperation")
    }

// ----------------------------------------------------
// CMsgSimOperation::StartL
// This function issues request and activates the object
//
// ----------------------------------------------------
void CMsgSimOperation::StartL()
    {
    SMUMLOGGER_ENTERFN("CMsgSimOperation::StartL")
    // Retry is used to define the times ReadSimParamsL() is called
    iRetryCount++;
    SMUMLOGGER_WRITE_FORMAT("StartL - iRetryCount :%d", iRetryCount)

    SMUMLOGGER_WRITE("StartL - BootState ok, proceed with SMSC fetch")
    // start the real sim reading operation
    iSimOperation = iSmsClientMtm->ReadSimParamsL( iStatus );
    SetActive();

    SMUMLOGGER_LEAVEFN("CMsgSimOperation::StartL")
    }

// ----------------------------------------------------
// CMsgSimOperation::CompleteRequest
//
// ----------------------------------------------------
void CMsgSimOperation::CompleteRequest( TInt aValue )
    {
    SMUMLOGGER_ENTERFN("CMsgSimOperation::CompleteRequest")
    
    __ASSERT_DEBUG( !IsActive(), Panic( ESimOperationPanicRequestAlreadyActive ));
    TRequestStatus* myRequestStatus = &iStatus;
    User::RequestComplete( myRequestStatus, aValue );
    if( !IsActive() )
        {
        SetActive();
        }

    SMUMLOGGER_LEAVEFN("CMsgSimOperation::CompleteRequest")
    }

// ----------------------------------------------------
// CMsgSimOperation::CompleteClientRequest
//
// ----------------------------------------------------
void CMsgSimOperation::CompleteClientRequest( TInt aValue )
    {
    SMUMLOGGER_ENTERFN("CMsgSimOperation::CompleteClientRequest")
    
    TBool haveClientRequestStatus = HaveClientRequestStatus();
    __ASSERT_DEBUG( haveClientRequestStatus, Panic( ESimOperationPanicClientsRequestAlreadyCompleted ));
    // So that we don't attempt to complete the clients request again in the future
    if ( haveClientRequestStatus )
        {
        User::RequestComplete( iClientStatus, aValue );
        iClientStatus = NULL;
        }

    SMUMLOGGER_LEAVEFN("CMsgSimOperation::CompleteClientRequest")
    }

// ----------------------------------------------------
// CMsgSimOperation::HaveClientRequestStatus
//
// ----------------------------------------------------
TBool CMsgSimOperation::HaveClientRequestStatus() const
    {
    SMUMLOGGER_WRITE("CMsgSimOperation::HaveClientRequestStatus")
    return (iClientStatus != NULL);
    }

// ----------------------------------------------------
// CMsgSimOperation::Panic
//
// ----------------------------------------------------
void CMsgSimOperation::Panic(TSimOperationPanic aPanic)
    {
    SMUMLOGGER_ENTERFN("CMsgSimOperation::Panic")

    _LIT(KSimOpPanicCategory, "SIMOP");
    SMUMLOGGER_WRITE_FORMAT("Panic - Panic :%d", aPanic)
    User::Panic(KSimOpPanicCategory, aPanic);
    
    SMUMLOGGER_LEAVEFN("CMsgSimOperation::Panic")
    }

// ----------------------------------------------------
// CMsgSimOperation::RunL
// Active object RunL method is called when the request (sim params reading operation) has been completed
//
// ----------------------------------------------------
void CMsgSimOperation::RunL()
    {
    SMUMLOGGER_ENTERFN("CMsgSimOperation::RunL")

    TInt error = iStatus.Int();
    SMUMLOGGER_WRITE_FORMAT("RunL - 1st error check :%d", error)
    if ( error == KErrNone ) 
        { // continue if sim reading operation completed ok
        TRAP( error, DoRunL());
        }
    
    SMUMLOGGER_WRITE_FORMAT("RunL - 2nd error check :%d", error)
    // if problems with above; retry 
    TInt maxRetryCount = KSmumRetryCount;
    if ( error == KErrTimedOut )
        {
        maxRetryCount = KSmumRetryCount/10; // no use to retry many times if timed out already
        }
    if ( error != KErrNone /*KErrGsmSMSSimBusy*/ && iRetryCount <= maxRetryCount )
        { 
        SMUMLOGGER_WRITE("RunL - Retrying")
        // first cancel the current simOp if still ongoing
        if ( iSimOperation )
            {
            SMUMLOGGER_WRITE("RunL - Deleting SIM operation")
            iSimOperation->Cancel();
            delete iSimOperation;
            iSimOperation = NULL;
            }
        // wait a bit and actual retry
        User::After( KSmumRetryDelay );
        StartL();
        return;
        }

    // Once the 'real' operation has completed, we inform the
    // client of the result
    if  ( HaveClientRequestStatus() )
        {
        SMUMLOGGER_WRITE("RunL - Completing client request")
        CompleteClientRequest( error );
        }

    SMUMLOGGER_LEAVEFN("CMsgSimOperation::RunL")
    }

// ----------------------------------------------------
// CCMsgSimOperation::DoRunL
//
// ----------------------------------------------------
void CMsgSimOperation::DoRunL()
    {
    SMUMLOGGER_ENTERFN("CMsgSimOperation::DoRunL")

    TIntBuf progressBuf;
    progressBuf.Copy( iSimOperation->ProgressL());
    TInt error = progressBuf();
    SMUMLOGGER_WRITE_FORMAT("DoRunL - Error check :%d", error)

    if ( error == KErrNone )  // check operation status
        {
        CMobilePhoneSmspList* centersList = iSimOperation->ServiceCentersLC();
        TInt count = centersList->Enumerate(); // How many centre numbers are there on the SIM card?
        SMUMLOGGER_WRITE_FORMAT("DoRunL - Amount of SIM SMSCs :%d", count )
        TBool emailFeatureSupported = SmumUtil::CheckEmailOverSmsSupportL();
		TBool emailEntryFound( EFalse );
	
	    // Check the service centre situation even if there are no service centres on SIM!
	    // If there are none, and only usage of service centre SIMs is allowed, all must
	    // be removed and the SMS service will be disabled.
	    	
        TInt numSCAddresses( 0 );
        iSmsClientMtm->SwitchCurrentEntryL( iSmsClientMtm->ServiceId() ); // load SMS settings
        CMsvEntry& service = iSmsClientMtm->Entry();
        // load settings
        CSmsSettings* smsSettings = CSmsSettings::NewLC();
        CSmsAccount* smsAccount = CSmsAccount::NewLC();
    	smsAccount->LoadSettingsL( *smsSettings );
        TBool newDefault = EFalse;
        TInt newDefaultIndex = -1;

		// Check if Only Sim SC's variation is on
		if ( SmumUtil::CheckVariationFlagsL( KCRUidMuiuVariation, KMuiuSmsFeatures ) & 
			 KSmsFeatureIdSimServiceCentresOnly )
			{
			// Remove all old SMSC's
			SMUMLOGGER_WRITE("DoRunL - Only Sim SC's - Deleting old ones->" )			
	        numSCAddresses = smsSettings->ServiceCenterCount();
	        SMUMLOGGER_WRITE_FORMAT("DoRunL - Amount of old SMSC's: #%d", numSCAddresses )
	        for ( TInt j = numSCAddresses; j > 0; j-- )
	            {
	            smsSettings->RemoveServiceCenter( j - 1 );
	            }
	        #ifdef _DEBUG		
			numSCAddresses = smsSettings->ServiceCenterCount();
			SMUMLOGGER_WRITE_FORMAT("DoRunL - Amount of SMSC's now: #%d", numSCAddresses )
			#endif // _DEBUG		
			}
	
        SMUMLOGGER_WRITE("DoRunL - Ready for looping SMSCs")
        for ( TInt i = 0; i < count; i++ )    // add all centres from sim to sms settings
            {
            SMUMLOGGER_WRITE_FORMAT("DoRunL - SMSC #%d", i)
            RMobileSmsMessaging::TMobileSmspEntryV1 entry;
            entry = centersList->GetEntryL( i );
            TBool duplicateFound = EFalse;
            
            SMUMLOGGER_WRITE_FORMAT("DoRunL - SMSC Name: %S", &entry.iText ); 
            SMUMLOGGER_WRITE_FORMAT("DoRunL - SMSC Address: %S", &entry.iServiceCentre.iTelNumber );
            // If empty tel number field, don't add
            if ( entry.iServiceCentre.iTelNumber == KNullDesC )
                {
                SMUMLOGGER_WRITE_FORMAT("DoRunL - No number in SMSC #%d", i)
                continue;
                }
            //Check is number already in cenrep
            //Get default SC from phone settings for later use
            TBuf<TGsmSmsTelNumberMaxLen> defaultSmsCenterNb;;            
            TInt defaultSCIndex = smsSettings->DefaultServiceCenter();
            if ( defaultSCIndex > -1 )
                {
                defaultSmsCenterNb = smsSettings->GetServiceCenter( defaultSCIndex ).Address(); 
                }    
            if ( iCheckSimScAlways && i == 0 )
                {
                //Check is number already in cenrep
                TBuf<TGsmSmsTelNumberMaxLen> previousSmsCenterNb;
                CRepository* cenRepSession = CRepository::NewLC( KCRUidSmum );
                cenRepSession->Get( KSmumPreviousSimSmscNumber, previousSmsCenterNb );
                
                //Check was there different SMSC in cenrep and default SMSC in settings. 
                if ( entry.iServiceCentre.iTelNumber == previousSmsCenterNb &&
                    entry.iServiceCentre.iTelNumber == defaultSmsCenterNb &&
                    smsSettings->ServiceCenterCount() != 0 )
                    {
                    SMUMLOGGER_WRITE_FORMAT("DoRunL - number #%d already in cenrep/settings", i)
                    CleanupStack::PopAndDestroy( cenRepSession );
                    continue;
                    }
                else 
                    {
                    SMUMLOGGER_WRITE_FORMAT("DoRunL - number #%d not in cenrep/settings", i)
                    cenRepSession->Set( KSmumPreviousSimSmscNumber, entry.iServiceCentre.iTelNumber );
                    CleanupStack::PopAndDestroy( cenRepSession );
                    }                
                }
			SMUMLOGGER_WRITE_FORMAT("DoRunL - Entry ProtocolId #%d", entry.iProtocolId)
            // Check for Email settings
            if ( emailFeatureSupported 
                 && KSmumSMSPid == entry.iProtocolId
                 && !emailEntryFound ) // We'll take the first
                {
                SMUMLOGGER_WRITE_FORMAT("DoRunL - Email over sms entry #%d", i)
                SmumUtil::WriteEmailOverSmsSettingsL( 
                    entry.iServiceCentre.iTelNumber,
                    entry.iDestination.iTelNumber,
                    ETrue );
                emailEntryFound = ETrue;
                }
            else
            	{
                // check if this SC is already in Sms settings
                numSCAddresses = smsSettings->ServiceCenterCount();
                for ( TInt j = 0; j < numSCAddresses; j++ )
                    {
                    if ( entry.iServiceCentre.iTelNumber == 
                        smsSettings->GetServiceCenter( j ).Address() )
                        {
                        SMUMLOGGER_WRITE_FORMAT("DoRunL - Duplicate. SMSC #%d", i)
                        SMUMLOGGER_WRITE_FORMAT("DoRunL -    of Sms Settings SMSC #%d", j)

                        duplicateFound = ETrue;
                        if ( i == 0 )   // first smsc in the sim but it has a duplicate already
                            {
                            newDefault = ETrue;
                            newDefaultIndex = j;
                            }
                        break;
                        }
                    }

                SMUMLOGGER_WRITE_FORMAT("DoRunL - After dupe-check : newDefaultIndex :%d", newDefaultIndex)

                // this is not a duplicate, find a name and add it to Sms settings
                if ( !duplicateFound )
                    {
                    
                    if(i==0)    // first smsc in the sim and has no duplicates
                        {
                        newDefault=ETrue;
                        newDefaultIndex = smsSettings->ServiceCenterCount();
                        }
                    
                    SMUMLOGGER_WRITE_FORMAT("DoRunL - Not a dupe : newDefaultIndex :%d", newDefaultIndex)
                    
                    TBuf<KMaxNameLength> name;
                    name = entry.iText;
                    
                    if ( name == KNullDesC )
                        {
                        SMUMLOGGER_WRITE( "DoRunL - SMSC has no name" )
                        // read Service centres from SmsSettings
                        TInt serviceCentres = 0;
                        serviceCentres = smsSettings->ServiceCenterCount();
                        SMUMLOGGER_WRITE_FORMAT(
                            "DoRunL - Amount of Sms Settings SMSCs :%d", 
                            serviceCentres )
                        
                        CDesCArrayFlat* nameArray = 
                            new ( ELeave ) CDesCArrayFlat( KSmumGranularity );
                        CleanupStack::PushL( nameArray );
                        
                        for ( TInt loop = 0 ; loop < serviceCentres; loop++ )
                            {
                            nameArray->AppendL( 
                                smsSettings->GetServiceCenter( loop ).Name() );
                            }
                        SmumUtil::FindDefaultNameForSCL( name, ETrue, nameArray );
                        CleanupStack::PopAndDestroy();// nameArray
                        }
                    smsSettings->AddServiceCenterL( 
                        name, entry.iServiceCentre.iTelNumber );
                    SMUMLOGGER_WRITE("DoRunL - New SMSC added")
                	}
                }
            }
        // set default SC
        if(newDefault)  // the default has changed
            {
            smsSettings->SetDefaultServiceCenter( newDefaultIndex );
            SMUMLOGGER_WRITE_FORMAT("DoRunL - New default SMSC :%d", newDefaultIndex)
            }
        // save settings
        smsAccount->SaveSettingsL( *smsSettings );
        SMUMLOGGER_WRITE("DoRunL - Saved")
    #ifdef _DEBUG
        TMsvEntry entry = service.Entry();
        entry.iDate.HomeTime();
        service.ChangeL( entry );
        SMUMLOGGER_WRITE("DoRunL - Entry changed")
    #endif
        CleanupStack::PopAndDestroy( 2 ); // msvStore, smsSettings
            
        if ( emailFeatureSupported && !emailEntryFound )
            {
            SMUMLOGGER_WRITE("DoRunL - Empty Email settings")
            HBufC* emptyStr = NULL;
            emptyStr = KNullDesC().AllocLC();
            SmumUtil::WriteEmailOverSmsSettingsL( 
                        emptyStr->Des(),
                        emptyStr->Des(),
                        ETrue );
            CleanupStack::PopAndDestroy();  // emptyStr
            }
        CleanupStack::PopAndDestroy();  // centersList
        }
    SMUMLOGGER_LEAVEFN("CMsgSimOperation::DoRunL")
    }

// ----------------------------------------------------
// CMsgSimOperation::RunError
// Called when AO RunL leaves. Handle the leave and return
//
// ----------------------------------------------------
 
TInt CMsgSimOperation::RunError( TInt )
    {
    SMUMLOGGER_ENTERFN("CMsgSimOperation::RunError")
    DoCancel();
    SMUMLOGGER_LEAVEFN("CMsgSimOperation::RunError")
    return KErrNone;
    }

// ----------------------------------------------------
// CMsgSimOperation::DoCancel
// called by active object's Cancel method
//
// ----------------------------------------------------
void CMsgSimOperation::DoCancel()
    {
    SMUMLOGGER_ENTERFN("CMsgSimOperation::DoCancel")

    if ( iSimOperation )  // if the real Sim reading operation was started:
        {
        iSimOperation->Cancel();    // cancel it
        }

    // We don't need to complete our own request status as this function will never
    // be called if our own request status has already been completed previously.
    // 
    // We do need to complete the client's request status however.
    if  (iClientStatus)
        {
        CompleteClientRequest(KErrCancel);
        }

    SMUMLOGGER_LEAVEFN("CMsgSimOperation::DoCancel")
    }


// ----------------------------------------------------
// CMsgSimOperation::HandleSessionEventL
// called when there is a Msv session event. 
//
// ----------------------------------------------------
void CMsgSimOperation::HandleSessionEventL(
    TMsvSessionEvent aEvent, 
    TAny* /*aArg1*/, 
    TAny* /*aArg2*/, 
    TAny* /*aArg3*/) 
    {
    SMUMLOGGER_ENTERFN("CMsgSimOperation::HandleSessionEventL")
    SMUMLOGGER_WRITE_FORMAT("HandleSessionEventL - TMsvSessionEvent :%d", aEvent)

    // problem case handling
    if ( aEvent == EMsvServerFailedToStart )
        {
        // also if we couldn't start up the msv server, we simulate that the request has completed
        CompleteRequest( KErrNotReady ); 
        }
    else if (( aEvent == EMsvServerTerminated ) || ( aEvent == EMsvCloseSession ))
        {
        // be polite to the Messaging Server and close the session
        Cancel();
        CompleteRequest( KErrNotReady ); 

        delete iSimOperation;  // These objects must be deleted first
        iSimOperation = NULL;       // as they can't exist without a MsvSession
        delete iSmsClientMtm;
        iSmsClientMtm = NULL;
        delete iClientRegistry;
        iClientRegistry = NULL;
        delete iMsvSession;
        iMsvSession = NULL;
        iClientStatus = NULL;
        }

    SMUMLOGGER_LEAVEFN("CMsgSimOperation::HandleSessionEventL")
    }

// ----------------------------------------------------
// CMsgSimOperation::HandleStartupReadyL
// Handles events from startup state observer
// 
// ----------------------------------------------------
//
void CMsgSimOperation::HandleStartupReadyL()
    {
	SMUMLOGGER_WRITE( "BootState Ready" )
	// Boot ready, start the real SimOperation
	StartL();
    }
//  End of File  
