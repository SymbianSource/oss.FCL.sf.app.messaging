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
#include "NcnDebug.h"
#include "NcnOutboxSendOperation.h"   // header
#include "NcnModel.h"

#include    <SenduiMtmUids.h>           // MTM Uids
#include    <msvids.h>                  // Entry Ids
#include    <muiumsvuiserviceutilitiesinternal.h> // Messaging utilites
#include    <gsmerror.h>                // SMS sending failure error codes


// CONSTANTS
const TInt KNcnSendPriority=CActive::EPriorityStandard;
const TInt KNcnSendSelectionGranularity=4;

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CNcnOutboxSendOperation* CNcnOutboxSendOperation::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus )
    {
    // Create the instance of sending operation
    CNcnOutboxSendOperation* self =
        new (ELeave) CNcnOutboxSendOperation(
            aMsvSession, aObserverRequestStatus );

    // Push self into cleanup stack during construction
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    // Return the object
    return self;
    }

// C++ default constructor can NOT contain any code that
// might leave.
//
CNcnOutboxSendOperation::CNcnOutboxSendOperation(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus )
    :
    CMsvOperation( aMsvSession, KNcnSendPriority, aObserverRequestStatus ),
    iSelections( KNcnSendSelectionGranularity ),
    iServices(),
    iSupportedMsgs( CNcnOutboxSendOperation::ENcnSupportsSmsSending )
    {
    // Start scheduler
    CActiveScheduler::Add(this);
    }

//destructor
CNcnOutboxSendOperation::~CNcnOutboxSendOperation()
    {
    // Cancel sending
    Cancel();

    // Delete sending operation
    delete iOperation;
    iOperation = NULL;

    // Delete entry
    delete iEntry;
    iEntry = NULL;

    // Remove services from queue and destroy message selections
    iServices.Reset();
    iSelections.ResetAndDestroy();
    }

// ----------------------------------------------------
// CNcnOutboxSendOperation::ConstructL
// ----------------------------------------------------
//
void CNcnOutboxSendOperation::ConstructL()
    {
    // Get rootindex entry
    iEntry = iMsvSession.GetEntryL( KMsvRootIndexEntryId );
    iEntry->SetSortTypeL(
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );

    // Set sending flags
    iSupportedMsgs |= ENcnSendSms;

    // Start sending
    StartSendingL();
    }

// ----------------------------------------------------
// CNcnOutboxSendOperation::RunL
// ----------------------------------------------------
//
void CNcnOutboxSendOperation::RunL()
    {
    NCN_RDEBUG(_L("CNcnOutboxSendOperation::RunL"));
    User::LeaveIfError( iStatus.Int() );

    // Check and start sending, if needed
    TUid sendMtm;

    // Check if messages needs to be sent
    if( IsSendingNeeded( sendMtm ) )
        {
        StartSendingL();
        }
    // Nothing to send, complete operation
    else
        {
        CompleteObserver( iStatus.Int() );
        }
    }

// ----------------------------------------------------
// CNcnOutboxSendOperation::RunError
// ----------------------------------------------------
//
TInt CNcnOutboxSendOperation::RunError( TInt aError )
    {
    NCN_RDEBUG_INT(_L("CNcnOutboxSendOperation::RunError - Error %d"), aError );
    CompleteObserver( aError );
    return aError;
    }

// ----------------------------------------------------
// CNcnOutboxSendOperation::DoCancel
// ----------------------------------------------------
//
void CNcnOutboxSendOperation::DoCancel()
    {
    // Check if sending operation is running
    if( iOperation )
        {
        // Cancel it
        iOperation->Cancel();
        }

    // Complete operation with current status
    CompleteObserver( iStatus.Int() );
    }

// ----------------------------------------------------
// CNcnOutboxSendOperation::ProgressL
// ----------------------------------------------------
//
const TDesC8& CNcnOutboxSendOperation::ProgressL()
    {
    // Check if operation exists
    if( iOperation )
        {
        // Return the operation
        return iOperation->ProgressL();
        }

    // If no operation, return blank information
    return KNcnBlankBuffer;
    }

// ---------------------------------------------------------
// CNcnOutboxSendOperation::CompleteOperation
// ---------------------------------------------------------
//
void CNcnOutboxSendOperation::CompleteObserver( TInt aStatus )
    {
    // Get the observer status
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, aStatus );
    }

// ----------------------------------------------------
// CNcnOutboxSendOperation::StartSendingL
// ----------------------------------------------------
//
void CNcnOutboxSendOperation::StartSendingL()
    {
    // Remove any sending operation that currently may be running
    delete iOperation;
    iOperation = NULL;

    // Check if selection can be created for selected

    // Check if there was errors with creating selection
    if  ( CheckAndCreateSelectionL() )
        {
        // Create new operation and trap any errors
        SendWaitingSMSMessages();

        // Clear selection after the operation is complete
        RemoveSelection();
        }
    else
        {
    // Set state to pending
        CompleteSelf( KErrNone );
        }
    }

    // if error, then complete this pass with the error code
void CNcnOutboxSendOperation::SendWaitingSMSMessages()
    {
    TRAPD( err, SendWaitingSMSMessagesL() );
    if  ( err != KErrNone )
        {
        ASSERT( !IsActive() );
        CompleteSelf( err );
        }
    }

// ----------------------------------------------------
// CNcnOutboxSendOperation::SendWaitingSMSMessagesL
// ----------------------------------------------------
//
void CNcnOutboxSendOperation::SendWaitingSMSMessagesL()
    {
    // Get first selection from queue
    CMsvEntrySelection& selection = ( *iSelections[0] );

    // Get count of messages in queue
    TInt count = selection.Count();

    // Go through all messages
    while( count-- )
        {
        // Select message
        iEntry->SetEntryL( selection[count] );
        TMsvEntry entry( iEntry->Entry() );

        // Check if the message is tried to send when in offline
        if( ( entry.SendingState() == KMsvSendStateSuspended ||
              entry.SendingState() == KMsvSendStateFailed ) &&
            ( entry.iError == KErrGsmOfflineOpNotAllowed ||
              entry.iError == KErrGsmOfflineSimOpNotAllowed ) )
            {
            // Set message to wait sending
            NCN_RDEBUG(_L("SendWaitingSMSMessagesL: entry qualified" ) );
            entry.SetSendingState( KMsvSendStateWaiting );
            iEntry->ChangeL( entry );
            }
		else
			{
			NCN_RDEBUG_INT2(_L("SendWaitingSMSMessagesL: entry not qualified: state %d, error %d" ), entry.SendingState(), entry.iError );
			selection.Delete( count );
			}
        }
    selection.Compress();

    // Set entry to outbox
    iMtm = iEntry->Entry().iMtm;
    iEntry->SetEntryL( KMsvGlobalOutBoxIndexEntryId );

	NCN_RDEBUG_INT(_L("SendWaitingSMSMessagesL: %d entries to send" ), selection.Count() );
    // Start sending
	if ( selection.Count() )
		{
		iOperation = iEntry->CopyL( selection, iServices[0], iStatus );
        SetActive();
		}
    else
        {
        // Nothing to send, but we must complete the observer via our RunL callback
        CompleteSelf( KErrNone );
        }
    }


// ----------------------------------------------------
// CNcnOutboxSendOperation::RemoveSelection
// ----------------------------------------------------
//
void CNcnOutboxSendOperation::RemoveSelection()
    {
    // Clear the the current selection.
    iServices.Delete(0);

    // Delete selection object and index
    delete iSelections[0];
    iSelections.Delete(0);
    }

// ----------------------------------------------------
// CNcnOutboxSendOperation::CreateSelectionL
// ----------------------------------------------------
//
void CNcnOutboxSendOperation::CreateSelectionL(
    const TUid &aUidForSel,
    const TMsvId& aServiceId,
    CMsvEntrySelection*& aSelection )
    {
    // Set entry to outbox and get messages from outbox
    iEntry->SetEntryL( KMsvGlobalOutBoxIndexEntryId );
    aSelection = iEntry->ChildrenWithMtmL( *&aUidForSel );

    // Check if there is anything to put into array
    if( aSelection->Count() )
        {
        // Put selection to queue
        CleanupStack::PushL( aSelection );
        iSelections.AppendL( aSelection );
        CleanupStack::Pop( aSelection );

        // Put service to queue
        iServices.AppendL( aServiceId );
        }
    else
        {
        // Remove selection
        delete aSelection;
        aSelection = NULL;
        }
    }

// ----------------------------------------------------
// CNcnOutboxSendOperation::CheckAndCreateSelectionL
// ----------------------------------------------------
//
TBool CNcnOutboxSendOperation::CheckAndCreateSelectionL()
    {
    // Get root index
    iEntry->SetEntryL( KMsvRootIndexEntryId );

    // MTM, for which the selection is collected
    TUid sendMtm;

    // Selection of messages for sending
    CMsvEntrySelection* smsSelection = NULL;

    // While MTM's available for sending
    while( smsSelection == NULL && IsSendingNeeded( sendMtm ) )
        {
        // Find default SMS service
        TMsvId serviceId =
            MsvUiServiceUtilitiesInternal::DefaultServiceForMTML(
                iMsvSession, *&sendMtm, ETrue );

        // Check if the service ID is found
        if( serviceId != KMsvNullIndexEntryId )
            {
            // Create selection of messages of specified MTM
            CreateSelectionL( sendMtm, serviceId, smsSelection );
            }
		else
			{
			NCN_RDEBUG(_L("CNcnOutboxSendOperation::CheckAndCreateSelectionL: default SMS service not found"));
			}

        // Selection has been created, remove the flag
        RemoveSendingFlag( *&sendMtm );
        }

    const TBool selectionAvailable = ( smsSelection != NULL );
    return selectionAvailable;
    }

// ----------------------------------------------------
// CNcnOutboxSendOperation::RemoveSendingFlag
// ----------------------------------------------------
//
void CNcnOutboxSendOperation::RemoveSendingFlag( const TUid& aMtm )
    {
    // Decide actions by mtm
    switch( aMtm.iUid )
        {
        // SMS-messages
        case KSenduiMtmSmsUidValue:
            iSupportedMsgs &= ~ENcnSendSms;
            break;
        // Do nothing
        default:
            break;
        }
    }

// ----------------------------------------------------
// CNcnOutboxSendOperation::IsSendingNeeded
// ----------------------------------------------------
//
TBool CNcnOutboxSendOperation::IsSendingNeeded(
    TUid& aMtm ) const
    {
    // Set starting condition
    TBool needSending = EFalse;

    // Check if sms-sending is supported and messages need to be sent
    if( iSupportedMsgs & ENcnSupportsSmsSending &&
        iSupportedMsgs & ENcnSendSms )
        {
        aMtm = KSenduiMtmSmsUid;
        needSending = ETrue;
        }
    // Otherwise nothing needs to be done
    else
        {
        aMtm.iUid = 0;
        needSending = EFalse;
        }

    // Return the result
    return needSending;
    }

// ----------------------------------------------------
// CNcnOutboxSendOperation::CompleteSelf
// ----------------------------------------------------
//
void CNcnOutboxSendOperation::CompleteSelf( TInt aValue )
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, aValue );
    SetActive();
    }
    
// End of file
