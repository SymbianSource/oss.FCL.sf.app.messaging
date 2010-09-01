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
* Description:   Implements class
*
*/



// INCLUDE FILES
#include    "NcnCRHandler.h"
#include    <CoreApplicationUIsSDKCRKeys.h>

#include    <SenduiMtmUids.h>           // mtm uids
#include    <msvuids.h>                 // Entry Uids
#include    <mtmuibas.h>                // CBaseMtmUi
#include    "NcnOutboxSendOperation.h"
#include    "NcnOutboxSender.h"
#include    "NcnModelBase.h"


// CONSTANTS

const TInt KNcnListBar0( 0 );

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
//  CNcnOutboxSender::NewL
// ----------------------------------------------------
//
CNcnOutboxSender* CNcnOutboxSender::NewL(
    CNcnModelBase*  aModel,
    CMsvSession&    aMsvSession )
    {
    // Create instance of the outboxsender object
    CNcnOutboxSender* self =
        new (ELeave) CNcnOutboxSender( aModel, aMsvSession );

    // Push to cleanupstack for constructphase
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    // Return the object
    return self;
    }

// ----------------------------------------------------
//  CNcnOutboxSender::CNcnOutboxSender
// ----------------------------------------------------
//
CNcnOutboxSender::CNcnOutboxSender(
    CNcnModelBase*  aModel,
    CMsvSession&    aMsvSession )
    :
    iNcnModel( aModel ), iMsvSession( aMsvSession )
    {
    }

// ----------------------------------------------------
//  CNcnOutboxSender::~CNcnOutboxSender
// ----------------------------------------------------
//
CNcnOutboxSender::~CNcnOutboxSender()
    {
    // Stop and delete the sending operation
    delete iRunningOperation;

    // Check if notification is set
    if( iFlags & EUserSettingsNotifierSet )
        {
        // Remove the notification from list
        if( iNotifyHandler )
            {            
            iNotifyHandler->StopListening();
		    delete iNotifyHandler;
            }
		
		delete iSession;
        }

    // Remove the address
    iNcnModel = NULL;
    }

// ----------------------------------------------------
//  CNcnOutboxSender::ConstructL
// ----------------------------------------------------
//
void CNcnOutboxSender::ConstructL()
    {
	iNetworkBars = KNcnListBar0;

	iSession = CRepository::NewL( KCRUidCoreApplicationUIs );
	iNotifyHandler = CCenRepNotifyHandler::NewL( *this, *iSession, CCenRepNotifyHandler::EIntKey, KCoreAppUIsNetworkConnectionAllowed );
	iNotifyHandler->StartListeningL();

	// Turn flag on for possible error handling cases
    iFlags |= EUserSettingsNotifierSet;

	CheckBootPhaseL();
    }

// ----------------------------------------------------
//  CNcnOutboxSender::StartSendingL
// ----------------------------------------------------
//
void CNcnOutboxSender::StartSendingL()
    {
    // Check if the sending is already in progress
    if( !IsSending() )
        {
        // Create instance of Single Operation Watcher
        CMsvSingleOpWatcher* singleOpWatcher =
            CMsvSingleOpWatcher::NewL(*this);

        // Push to cleanup stack while creating sending operation
        CleanupStack::PushL( singleOpWatcher );
        CMsvOperation* op =
            CNcnOutboxSendOperation::NewL(
                iMsvSession, singleOpWatcher->iStatus );
        CleanupStack::Pop( singleOpWatcher );

        // Set operation
        singleOpWatcher->SetOperation( op ); // takes immediately ownership
        iRunningOperation = singleOpWatcher;
        }
    }


// ----------------------------------------------------
//  CNcnOutboxSender::CancelSending
// ----------------------------------------------------
//
void CNcnOutboxSender::CancelSending()
    {
    // Remove the running operation
    delete iRunningOperation;
    iRunningOperation = NULL;
    }

// ----------------------------------------------------
//  CNcnOutboxSender::IsSending
// ----------------------------------------------------
//
TBool CNcnOutboxSender::IsSending() const
    {
    return ( iRunningOperation != NULL );
    }

// ----------------------------------------------------
//  CNcnOutboxSender::OpCompleted
// ----------------------------------------------------
//
void CNcnOutboxSender::OpCompleted(
    CMsvSingleOpWatcher& /*aOpWatcher*/,
    TInt /*aCompletionCode*/ )
    {
    // Clear flag
    iFlags &= ~EOffllineSendingNeeded;

    // Errors can be ignored since we try to send only once and
    // if it fails then it is up to the user...
    delete iRunningOperation;
    iRunningOperation = NULL;
    }

// ----------------------------------------------------
//  CNcnOutboxSender::HandleNotifyInt
// ----------------------------------------------------
//
void CNcnOutboxSender::HandleNotifyInt(
    const TUint32 aID,
    const TInt    aNewValue )
    {
    NCN_RDEBUG(_L("CNcnOutboxSender::HandleNotifyInt"));
	// Check if key is for offline-connecton
	if( aID == KCoreAppUIsNetworkConnectionAllowed )
        {
        NCN_RDEBUG(_L("CNcnOutboxSender::HandleNotifyInt KCoreAppUIsNetworkConnectionAllowed"));
        // Check if connection is established
        if( aNewValue == ECoreAppUIsNetworkConnectionAllowed )
            {
			NCN_RDEBUG(_L("CNcnOutboxSender::HandleNotifyInt ECoreAppUIsNetworkConnectionAllowed 1"));
            // Phone switched on again!
            iFlags |= EOffllineSendingNeeded;
            
            NCN_RDEBUG_INT(_L("CNcnOutboxSender::HandleNotifyInt iNetworkBars = %d"), iNetworkBars );
			if ( iNetworkBars > KNcnListBar0 )
				{
				NCN_RDEBUG_INT(_L("CNcnOutboxSender::HandleNotifyInt sending now, iNetworkBars = %d"), iNetworkBars );
				TRAP_IGNORE( StartSendingL() );
				}
            }
        else
            {
			NCN_RDEBUG(_L("CNcnOutboxSender::HandleNotifyInt ECoreAppUIsNetworkConnectionAllowed 0"));
            // Clear flag
            iFlags &= ~EOffllineSendingNeeded;

            // Stop sending
            CancelSending();

			// Set the coverage to 0 in case it didn't come from network in time
			iNetworkBars = KNcnListBar0;
            }
        }
	}

// ----------------------------------------------------
//  CNcnOutboxSender::HandleNotifyGeneric
// ----------------------------------------------------
//
void CNcnOutboxSender::HandleNotifyGeneric( const TUint32 /*aID*/ )
    {
	//NO OPERATION
	}

// ----------------------------------------------------
//  CNcnOutboxSender::HandleNotifyError
// ----------------------------------------------------
//
void CNcnOutboxSender::HandleNotifyError(
    const TUint32 /*aID*/,
    const TInt    /*aError*/,
	CCenRepNotifyHandler* /*aHandler*/ )
    {
	//NO OPERATION
	}

// ----------------------------------------------------
//  CNcnOutboxSender::CheckAndStartSendingL
// ----------------------------------------------------
//
void CNcnOutboxSender::CheckAndStartSendingL( const TInt& aNetworkBars )
    {
	NCN_RDEBUG_INT(_L("CNcnOutboxSender::CheckAndStartSendingL aNetworkBars = %d"), aNetworkBars );
	iNetworkBars = aNetworkBars;
    // Check if sending is needed and network is available
	if ( iFlags & EOffllineSendingNeeded &&
        aNetworkBars > KNcnListBar0 )
        {
		NCN_RDEBUG_INT(_L("CNcnOutboxSender::CheckAndStartSendingL sending now, iNetworkBars = %d"), iNetworkBars );
        // Start sending
        StartSendingL();
		}
	else
		{
		NCN_RDEBUG_INT(_L("CNcnOutboxSender::CheckAndStartSendingL not sending, iNetworkBars = %d"), iNetworkBars );
		}
    }

// ----------------------------------------------------
//  CNcnOutboxSender::CheckBootPhaseL
// ----------------------------------------------------
//
void CNcnOutboxSender::CheckBootPhaseL()
	{
    TInt connection;

	TInt err = iSession->Get( KCoreAppUIsNetworkConnectionAllowed, connection );

	if( err == KErrNone )
        {
        // Check if connection is established
        if( connection )
            {
			NCN_RDEBUG(_L("CNcnOutboxSender::CheckBootPhaseL KGSNetworkConnectionAllowed 1"));
            // Phone switched on again!            
            iFlags |= EOffllineSendingNeeded; 
			
			if ( iNetworkBars > KNcnListBar0 )
				{
				NCN_RDEBUG_INT(_L("CNcnOutboxSender::CheckBootPhaseL sending now, iNetworkBars = %d"), iNetworkBars );
				StartSendingL();
				}
			}
		}
	else
		{
		NCN_RDEBUG(_L("CNcnOutboxSender::CheckBootPhaseL Cannot access shared data"));
		}
	}

// End of file
