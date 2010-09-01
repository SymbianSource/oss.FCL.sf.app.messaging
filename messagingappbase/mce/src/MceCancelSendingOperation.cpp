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
*     CMceCancelSendingOperation.
*     Cancels first sending of the message and then tries to delete or move it to drafts.
*
*/




// INCLUDE FILES
#include    <MTMStore.h> // CMtmStore
#include    <msvids.h>
#include    "MceCancelSendingOperation.h"
#include    "MceLogText.h"

// MODULE DATA STRUCTURES

// Two-phased constructor.
CMceCancelSendingOperation* CMceCancelSendingOperation::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aEntryId,
    TCancelSendingType aCancelType,
    CMtmStore& aMtmStore )
    {
    MCELOGGER_ENTERFN("CMceCancelSendingOperation::NewL");
    CMceCancelSendingOperation* self = new ( ELeave ) CMceCancelSendingOperation(
        aMsvSession, aObserverRequestStatus, aEntryId, aCancelType, aMtmStore );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    MCELOGGER_LEAVEFN("CMceCancelSendingOperation::NewL");
    return self;
    }

// C++ default constructor can NOT contain any code that
// might leave.
//
CMceCancelSendingOperation::CMceCancelSendingOperation(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aEntryId,
    TCancelSendingType aCancelType,
    CMtmStore& aMtmStore )
    :
    CMsvOperation(
        aMsvSession,
        CActive::EPriorityStandard,
        aObserverRequestStatus),
    iMtmStore( aMtmStore ),
    iCancelType( aCancelType ),
    iEntryId( aEntryId ),
    iBlank(_L8(""))
    {
    CActiveScheduler::Add( this );
    }

// Symbian OS default constructor can leave.
void CMceCancelSendingOperation::ConstructL()
    {
    MCELOGGER_ENTERFN("CMceCancelSendingOperation::ConstructL");
    iEntry = CMsvEntry::NewL( iMsvSession, iEntryId, TMsvSelectionOrdering() );
    iEntrySelection = new( ELeave ) CMsvEntrySelection;
    iEntrySelection->AppendL( iEntryId );
    CancelSendingL();
    iCancelState = ECancelling;
    iStatus = KRequestPending;
    SetActive();
    MCELOGGER_LEAVEFN("CMceCancelSendingOperation::ConstructL");
    }


//destructor
CMceCancelSendingOperation::~CMceCancelSendingOperation()
    {
    MCELOGGER_ENTERFN("CMceCancelSendingOperation::~");
    Cancel();
    delete iOperation;
    delete iEntry;
    delete iEntrySelection;
    MCELOGGER_LEAVEFN("CMceCancelSendingOperation::~");
    }



// ----------------------------------------------------
// CMceCancelSendingOperation::CancelSendingL
// ----------------------------------------------------
void CMceCancelSendingOperation::CancelSendingL()
    {
    MCELOGGER_ENTERFN("CMceCancelSendingOperation::CancelSendingL");
    CBaseMtmUi& ui = iMtmStore.GetMtmUiLC( iEntry->Entry().iMtm );
    iMtm = iEntry->Entry().iMtm;
    MCELOGGER_WRITE_FORMAT("Start cancelling message id 0x%x", iEntryId );
    iOperation = ui.CancelL( iStatus, *iEntrySelection);
    CleanupStack::PopAndDestroy( );//release mtmUi
    MCELOGGER_LEAVEFN("CMceCancelSendingOperation::CancelSendingL");
    }

// ----------------------------------------------------
// CMceCancelSendingOperation::MoveOrDeleteEntryL
// ----------------------------------------------------
void CMceCancelSendingOperation::MoveOrDeleteEntryL()
    {
    MCELOGGER_ENTERFN("CMceCancelSendingOperation::MoveOrDeleteEntryL");
    delete iOperation;
    iOperation = NULL;
    iEntry->SetEntryL( KMsvGlobalOutBoxIndexEntryId );
    if ( iCancelType == ECancelAndMoveToDrafts )
        {
        MCELOGGER_WRITE_FORMAT("Start moving message id 0x%x to drafts", iEntryId );
        iOperation = iEntry->MoveL(
            *iEntrySelection, KMsvDraftEntryId, iStatus );
        }
    else
        {
        MCELOGGER_WRITE_FORMAT("Start deleting message id 0x%x", iEntryId );
        iOperation = iEntry->DeleteL(
            *iEntrySelection, iStatus );
        }
    iMtm = iOperation->Mtm();

    MCELOGGER_LEAVEFN("CMceCancelSendingOperation::MoveOrDeleteEntryL");
    }

// ----------------------------------------------------
// CMceCancelSendingOperation::DoCancel
// ----------------------------------------------------
void CMceCancelSendingOperation::DoCancel()
    {
    MCELOGGER_ENTERFN("CMceCancelSendingOperation::DoCancel");
    if ( iOperation )
        {
        iOperation->Cancel();
        }
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete(status, iStatus.Int());
    MCELOGGER_LEAVEFN("CMceCancelSendingOperation::DoCancel");
    }

// ----------------------------------------------------
// CMceCancelSendingOperation::RunL
// ----------------------------------------------------
void CMceCancelSendingOperation::RunL()
    {
    MCELOGGER_ENTERFN("CMceCancelSendingOperation::RunL");
    TInt error = iStatus.Int();
    MCELOGGER_WRITE_FORMAT("error: %d", error );
    if ( error == KErrNone && iCancelType != ECancelOnly && iCancelState == ECancelling )
        {
        MoveOrDeleteEntryL();
        iCancelState = EDeletingOrMoving;
        iStatus = KRequestPending;
        SetActive();
        }
    else
        {
        // everything is done
        MCELOGGER_WRITE("everything done, complete" );
        TRequestStatus* status = &iObserverRequestStatus;
        User::RequestComplete(status, iStatus.Int());
        }
    MCELOGGER_LEAVEFN("CMceCancelSendingOperation::RunL");
    }
// ----------------------------------------------------
// CMceCancelSendingOperation::ProgressL
// ----------------------------------------------------
const TDesC8& CMceCancelSendingOperation::ProgressL()
    {
    if ( iOperation )
        {
        return iOperation->ProgressL();
        }
    return iBlank;
    }

//  End of File
