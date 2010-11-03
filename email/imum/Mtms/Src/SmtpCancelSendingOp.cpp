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
*       SMTP Cancel Sending operation
*
*/

#include <msvids.h>
#include <msvuids.h>

#include "SmtpCancelSendingOp.h"
#include "ImumMtmLogging.h"

// CONSTANTS
const TInt KCancelSendOpPriority = CActive::EPriorityStandard;


CSmtpCancelSendingOp* CSmtpCancelSendingOp::NewL(CImumInternalApi& aMailboxApi, TRequestStatus& aObserverRequestStatus, CMsvEntrySelection* aEntrySelection)
    {
    IMUM_STATIC_CONTEXT( CSmtpCancelSendingOp::NewL, 0, mtm, KImumMtmLog );
    
    CSmtpCancelSendingOp* op = new(ELeave) CSmtpCancelSendingOp(aMailboxApi, aObserverRequestStatus, aEntrySelection);
    CleanupStack::PushL(op);
    op->ConstructL();
    CleanupStack::Pop();
    IMUM_OUT();
    return op;
    }

CSmtpCancelSendingOp::~CSmtpCancelSendingOp()
    {
    IMUM_CONTEXT( CSmtpCancelSendingOp::~CSmtpCancelSendingOp, 0, KImumMtmLog );
    IMUM_IN();
    
    delete iEntry;
    IMUM_OUT();
    }

const TDesC8& CSmtpCancelSendingOp::ProgressL()
    {
    IMUM_CONTEXT( CSmtpCancelSendingOp::ProgressL, 0, KImumMtmLog );
    IMUM_IN();
    
    if( (iOperation) && (iProgress().iError == KErrNone) )
        {
        IMUM_OUT();
        return iOperation->ProgressL();
        }
    IMUM_OUT();
    return iProgress;
    }

CSmtpCancelSendingOp::CSmtpCancelSendingOp(CImumInternalApi& aMailboxApi, TRequestStatus& aObserverRequestStatus, CMsvEntrySelection* aEntrySelection)
: CSelectionOperationBase(aMailboxApi, KCancelSendOpPriority, aObserverRequestStatus, aEntrySelection)
    {
    IMUM_CONTEXT( CSmtpCancelSendingOp::CSmtpCancelSendingOp, 0, KImumMtmLog );
    IMUM_IN();
    
    CActiveScheduler::Add(this);
    SetSelectionOwnershipExternal(EFalse);
    IMUM_OUT();
    }

void CSmtpCancelSendingOp::ConstructL()
    {
    IMUM_CONTEXT( CSmtpCancelSendingOp::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    iEntry = iMsvSession.GetEntryL(KMsvGlobalOutBoxIndexEntryId);
    iStatus = KRequestPending;
    CSelectionOperationBase::StartL();
    SetActive();
    IMUM_OUT();
    }

CMsvOperation* CSmtpCancelSendingOp::CreateOperationL(TMsvId aMsvId)
    {
    IMUM_CONTEXT( CSmtpCancelSendingOp::CreateOperationL, 0, KImumMtmLog );
    IMUM_IN();
    
    CMsvOperation* op = NULL;
    iEntry->SetEntryL(aMsvId);
    TMsvEntry tentry(iEntry->Entry());
    if(tentry.SendingState() != KMsvSendStateSuspended)
        {
        tentry.SetSendingState(KMsvSendStateSuspended);
        op = iEntry->ChangeL(tentry, iStatus);;
        }
    else
        {
        op = CMsvCompletedOperation::NewL(iMsvSession, KUidMsvLocalServiceMtm, iProgress, KMsvLocalServiceIndexEntryId, iStatus, KErrNone);
        }
    iMtm = op->Mtm();
    IMUM_OUT();
    return op;
    }

TBool CSmtpCancelSendingOp::StopOnError(TInt /*aErrorCode*/)
    {
    IMUM_CONTEXT( CSmtpCancelSendingOp::StopOnError, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    return EFalse;
    }

void CSmtpCancelSendingOp::OpRunError(TInt aErrorCode)
    {
    IMUM_CONTEXT( CSmtpCancelSendingOp::OpRunError, 0, KImumMtmLog );
    IMUM_IN();
    
    TMsvLocalOperationProgress& prog = iProgress();
    prog.iType = TMsvLocalOperationProgress::ELocalChanged;
    prog.iTotalNumberOfEntries = NumItems();
    prog.iNumberCompleted = CurrentItemIndex();
    prog.iNumberFailed++;
    prog.iNumberRemaining = (prog.iTotalNumberOfEntries) - (prog.iNumberCompleted);
    prog.iError = aErrorCode;
    IMUM_OUT();
    }

TInt CSmtpCancelSendingOp::CheckForError(const TDesC8& aProgress)
    {
    IMUM_CONTEXT( CSmtpCancelSendingOp::CheckForError, 0, KImumMtmLog );
    IMUM_IN();
    
    iProgress.Copy(aProgress);
    IMUM_OUT();
    return iProgress().iError;
    }

// End of file

