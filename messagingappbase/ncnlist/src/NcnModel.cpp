/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Methods for CNcnModel class.
*
*/




// INCLUDE FILES
#include    "NcnDebug.h"
#include    "NcnModel.h"
#include    "NcnCRHandler.h"
#include    "NcnSNNotifier.h"
#include    "CVoiceMailManager.h"

#include    <msgsimscnumberdetector.h>
#include	<rsatrefresh.h>
#include	<rsatsession.h>

// ================= LOCAL CONSTANTS =======================
namespace
    {
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNcnModelBase::NewL
// Protocol specific class factory
// ---------------------------------------------------------
//
CNcnModelBase* CNcnModelBase::NewL()
    {
    CNcnModelBase* self = CNcnModel::NewL();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
CNcnModel::CNcnModel()
:	iSatSession(NULL),
	iSatRefresh(NULL)
    {
	}

// Two-phased constructor.
CNcnModelBase* CNcnModel::NewL()
    {
    CNcnModelBase* self = new (ELeave) CNcnModel();

    return self;
    }

// Symbian OS default constructor can leave.
void CNcnModel::ConstructL()
    {
	// Call base class's constructor
    CNcnModelBase::ConstructL();

    //Subscribe to SAT Refresh notifications
	iSatSession = new ( ELeave ) RSatSession;
	iSatSession->ConnectL();
	iSatRefresh = new ( ELeave ) RSatRefresh( *this );
	iSatRefresh->OpenL( *iSatSession );

	//We subcribe the EF_SMSP file changes
	TSatRefreshFiles file;
	file.Append( KSmspEf );
	iSatRefresh->NotifyFileChangeL( file );
    }


// Destructor
CNcnModel::~CNcnModel()
    {
    //Cleanup for iSatRefresh
    if ( iSatRefresh )
        {
        // Cancel notifications.
        iSatRefresh->Cancel();
        // Close SubSession.
        iSatRefresh->Close();
        }
	delete iSatRefresh;
	iSatRefresh = NULL;

    //Cleanup for iSatSession
	if ( iSatSession )
		{
		iSatSession->Close();
		}
	delete iSatSession;
	iSatSession = NULL;
    }

// ---------------------------------------------------------
// CNcnModel::SetSimChanged
// ---------------------------------------------------------
//
void CNcnModel::SetSimChanged( const TBool aSimChanged )
    {
    NCN_RDEBUG_INT(_L("CNcnModel::SimChanged() %d"), aSimChanged);

    aSimChanged ?
        ( iNcnStatusBits |= KNcnSimChanged ) :
        ( iNcnStatusBits &= ~KNcnSimChanged );

    // If the sim card has been changed we notify some components
    if( aSimChanged )
        {
        NCN_RDEBUG(_L("CNcnModel::SimChanged() ResetMissedCalls") );
        // Reset the stored indicator status variable in the file.
        iNcnCRHandler->ResetMissedCalls();

        // Notify VoiceMailManager if it exists
        // VM Manager does not exist in boot-up but will be
        // available later
        if ( iVoiceMailManager != NULL )
	        {
            NCN_RDEBUG(_L("CNcnModel::SimChanged() iVoiceMailManager->NotifyAboutSIMChange") );
	        iVoiceMailManager->NotifyAboutSIMChange();
	        }
        }
    else
      {
      iNcnCRHandler->UpdateMissedCallsNotification();
      }        

    // Check here if we should fetch the sms service center number
    // from the sim card.
    CheckIfSimSCOperationRequired();
    }

// ---------------------------------------------------------
// CNcnModel::SetSmsInitialisationState
// ---------------------------------------------------------
//
void CNcnModel::SetSmsInitialisationState(
    const TInt aSmsInitialisationPhase )
    {
    iSmsInitialisationPhase = aSmsInitialisationPhase;
    
    if ( iSmsInitialisationPhase != ESMSInitNone )
    	{
    	iNcnStatusBits |= KNcnSmsServicePresent;
    	}

    // Check here if we should fetch the sms service center number
    // from the sim card.
    CheckIfSimSCOperationRequired();
    }

// ---------------------------------------------------------
// CNcnModel::CheckIfSimSCOperationRequired
// ---------------------------------------------------------
//
void CNcnModel::CheckIfSimSCOperationRequired()
    {
    NCN_RDEBUG(_L("CNcnModelBase::CheckIfSimSCOperationRequired()" ));

 	TBool servicePresent = (iNcnStatusBits & KNcnSmsServicePresent);
	TBool simChanged = (iNcnStatusBits & KNcnSimChanged);
#ifdef _DEBUG
	TBool systemRefresh = (iNcnStatusBits & KNcnSystemRefresh);
	TBool readSimSc = (iNcnStatusBits & KNcnReadSimSc);		
#endif	
	NCN_RDEBUG_INT( _L("iSmsInitialisationPhase %d"), iSmsInitialisationPhase );
	NCN_RDEBUG_INT( _L("NCN status bit sms service present %d"), servicePresent );
	NCN_RDEBUG_INT( _L("NCN status bit sim changed %d"), simChanged );
	NCN_RDEBUG_INT( _L("NCN status bit system refresh %d"), systemRefresh );
	NCN_RDEBUG_INT( _L("NCN read sim service center %d"), readSimSc );		

	// If SIM has been changed, we clear KNcnReadSimSc bit so
	// that SIM won't be read twice. 
	if( ( iNcnStatusBits & KNcnReadSimSc ) && 
		( iNcnStatusBits & KNcnSimChanged ) )
		{
		iNcnStatusBits &= ~KNcnReadSimSc;			
		}

	// If SIM service is present and SIM SC should be read 
	// in every boot, we enable KNcnSystemRefresh flag which will
	// force SIM SC to be read. 
	if( ( iNcnStatusBits & KNcnReadSimSc ) && 
		( iNcnStatusBits & KNcnSmsServicePresent ) )
		{
		iNcnStatusBits &= ~KNcnReadSimSc;
		iNcnStatusBits |= KNcnSystemRefresh;
		}

    // Read service centres from SIM
    if( iSmsInitialisationPhase == ESMSInitNone )
        {
        NCN_RDEBUG(_L("CNcnModelBase::CheckIfSimSCOperationRequired() iSmsInitialisationPhase == ESMSInitNone" ));
        if( servicePresent )
            {
            NCN_RDEBUG(_L("CNcnModelBase::CheckIfSimSCOperationRequired() servicePresent" ));
            if( simChanged )
                {
                NCN_RDEBUG(_L("CNcnModelBase::CheckIfSimSCOperationRequired() simChanged" ));
                PerformSimServiceCentreFetching();
                }
            }
        return;
        }
    // Read service centres from SIM
    if ((( iSmsInitialisationPhase == ESMSInitReadServiceCentre ) && servicePresent ) ||
        // Initialisation complete
        (( iSmsInitialisationPhase == ESMSInitComplete ) && 
        ( simChanged  || iNcnStatusBits & KNcnSystemRefresh )))
        {
        NCN_RDEBUG(_L("CNcnModelBase::CheckIfSimSCOperationRequired() second Read service centres from SIM" ));
        PerformSimServiceCentreFetching();
        }
    }

// ---------------------------------------------------------
// CNcnModel::PerformSimServiceCentreFetching
// ---------------------------------------------------------
//
void CNcnModel::PerformSimServiceCentreFetching()
    {
    NCN_RDEBUG(_L("CNcnModelBase::PerformSimServiceCentreFetching() :: Waiting until starting the SC operation" ));

    // If the sms service center is not fetched yet, then we create a
    // CMsgSimOperation class which does the fetching and then,
    // when completed, returns the status as iStatus.
    if( iNcnCRHandler && 
        ( !( iNcnStatusBits & KNcnSimServiceCentreFetched ) 
        || ( iNcnStatusBits & KNcnSystemRefresh ) ) )
        {
        // Read service centres from SIM
		// 3 = read service centres from SIM
        iNcnCRHandler->SetSmumFlag( ESMSInitReadServiceCentre );

        iNcnStatusBits |= KNcnSimServiceCentreFetched;
        iNcnStatusBits &= ~KNcnSystemRefresh;
        if( IsActive() )
            {
            // Cancels the wait for completion of an outstanding request.
    		NCN_RDEBUG(_L("CNcnModelBase::PerformSimServiceCentreFetching() - cancelled outstanding request" ));
            Cancel();
            }

        // Ignore error, but don't let it leave
		TRAPD( err, iMsgSimOperation = CMsgSimOperation::NewL( iStatus ) );
		NCN_RDEBUG_INT2(_L("CNcnModelBase::PerformSimServiceCentreFetching - created CMsgSimOperation with status: %d error code: %d"), iStatus.Int(), err );

		if( err == KErrNone )
		    {
		    SetActive();
		    NCN_RDEBUG_INT(_L("CNcnModelBase::PerformSimServiceCentreFetching() - operation set active: %d"), iStatus.Int() );
		    }
        }
    }

// ---------------------------------------------------------
// CNcnModel::ForceToPerformSimServiceCentreFetching
// ---------------------------------------------------------
//
void CNcnModel::ForceToPerformSimServiceCentreFetching()
    {
    // Reset the sim service centre fetched information.
    iNcnStatusBits |= KNcnSystemRefresh;
    // Get the new sim service centre.
    CheckIfSimSCOperationRequired();
    }

// ---------------------------------------------------------
// CNcnModel::CreateCRConnectionL
// ---------------------------------------------------------
//
void CNcnModel::CreateCRConnectionL()
    {
    // Create a handler for central repository information
    iNcnCRHandler = CNcnCRHandler::NewL( this );
    }

// -----------------------------------------------------------------------------
// CNcnModel::AllowRefresh
// Refresh query. Determines whether we allow the refresh to happen.
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
TBool CNcnModel::AllowRefresh( TSatRefreshType aType, const TSatRefreshFiles& /*aFiles*/ )
#else
TBool CNcnModel::AllowRefresh( TSatRefreshType /*aType*/, const TSatRefreshFiles& /*aFiles*/ )
#endif
	{
 	// Refresh is always allowed
    NCN_RDEBUG( _L("CNcnModel::AllowRefresh()" ) );
    NCN_RDEBUG_INT( _L("CNcnModel::AllowRefresh() Refresh type: %d"), aType );
	return ETrue;
    }

// -----------------------------------------------------------------------------
// CNcnModel::Refresh
// Notification about file refresh. We should launch the fetching of the SMS
// service center in case the changed file is KSmspEf
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
void CNcnModel::Refresh( TSatRefreshType aType, const TSatRefreshFiles& /*aFiles*/ )
#else
void CNcnModel::Refresh( TSatRefreshType /*aType*/, const TSatRefreshFiles& /*aFiles*/ )
#endif
	{
	// The only "file change" that we have subscribed is KSmspEf, so we don't need
	// to examine the argument aFiles
    NCN_RDEBUG( _L("CNcnModel::Refresh()"));
    NCN_RDEBUG_INT( _L("CNcnModel::Refresh Type: %d"), aType );

	//Load service center from SIM
	ForceToPerformSimServiceCentreFetching();

    // Notify SAT Server that refresh initiated file read is done
    iSatRefresh->RefreshEFRead( EFalse );
	}
//  End of File
