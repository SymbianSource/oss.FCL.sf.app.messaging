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
#include "MsgSimSCNumberDetector.h"
#include "SmumLogging.h"
#include "smsui.pan"    // for panics
#include <FeatMgr.h>                    // FeatureManager
#include <bldvariant.hrh>

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

    SMUMLOGGER_LEAVEFN("CMsgSimOperation::CMsgSimOperation")
    }

// Symbian OS default constructor can leave.
void CMsgSimOperation::ConstructL()
    {
    SMUMLOGGER_ENTERFN("CMsgSimOperation::ConstructL")
/*    
	RSystemAgent sysAgent;
	if( sysAgent.Connect() == KErrNone )
		{
		if ( sysAgent.GetState(KUidSimCStatus) == ESACSimNotSupported )
			{
			// Sim Card not supported
			// Complete the Active Object request returning that this is not supported.
			// Since we cannot insert a sim card when the device is operating, 
			// the Flag cannot change, the request is just completed without doing anything
			//
			CompleteRequest(KErrNotSupported);
			}
		else
			{
			CompleteRequest(KErrNone);
			}
		sysAgent.Close();
		}
	// Default not supported
	// 
	else
		{
		CompleteRequest(KErrNotSupported);
		}
*/
    SMUMLOGGER_LEAVEFN("CMsgSimOperation::ConstructL")
    }

// C++ virtual destructor
CMsgSimOperation::~CMsgSimOperation()
    {
    SMUMLOGGER_ENTERFN("CMsgSimOperation::~CMsgSimOperation")

    Cancel(); // make sure that this object is not left on the active scheduler.
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

	if ( error == KErrNotSupported)
		{
		// Sim Card Not Supported in the platform
		// Should be KErrNotSupported if constructL fails!
		//
		CompleteClientRequest(error);
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
    TMsvSessionEvent /*aEvent*/, 
    TAny* /*aArg1*/, 
    TAny* /*aArg2*/, 
    TAny* /*aArg3*/) 
    {
    SMUMLOGGER_ENTERFN("CMsgSimOperation::HandleSessionEventL")

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
