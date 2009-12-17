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
*       Used to reset a service's pre-created messages when the service changes
*
*/

#include <MTMStore.h>
#include <mtmuibas.h>

#include "SmtpResetPreCreationOp.h"
#include "EmailPreCreation.h"
#include "ImumMtmLogging.h"

const TInt KResetCreateMsgPriority  = CActive::EPriorityIdle - 10;


CSmtpResetPreCreationOp* CSmtpResetPreCreationOp::NewL(CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus, TMsvId aServiceId)
    {
    IMUM_STATIC_CONTEXT( CSmtpResetPreCreationOp::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
     // static
    CSmtpResetPreCreationOp* op = new(ELeave) CSmtpResetPreCreationOp(aMsvSession, aObserverRequestStatus, aServiceId);
    CleanupStack::PushL(op);
    op->ConstructL();
    CleanupStack::Pop(op);
    IMUM_OUT();
    return op;
    }


void CSmtpResetPreCreationOp::RunL()
    {
    IMUM_CONTEXT( CSmtpResetPreCreationOp::RunL, 0, KImumMtmLog );
    IMUM_IN();
    
    DoRunL();
    
    IMUM_OUT();
    }


TInt CSmtpResetPreCreationOp::RunError(TInt __DEBUG_ONLY( aError ) )
	{
    IMUM_CONTEXT( CSmtpResetPreCreationOp::RunError, 0, KImumMtmLog );
    IMUM_IN();	
	
	IMUM1( 0, "RunL leaved with error code %d", aError );
	
	iMtm = KUidMsvLocalServiceMtm;
	CompleteObserver();
	
	IMUM_OUT();
	
	return KErrNone; 	
	}

void CSmtpResetPreCreationOp::DoRunL()
    {
    IMUM_CONTEXT( CSmtpResetPreCreationOp::DoRunL, 0, KImumMtmLog );
    IMUM_IN();
    
    TInt err = KErrNone;
    TPckgBuf<TMsvId> idbuf;
    if(iStatus.Int() != KErrNone)
        {
        err = iStatus.Int();
        }
    else if(iOperation)
        {
        const TDesC8& prog = iOperation->ProgressL();
        if(prog.Length())
            {
            if(iState == EStateCreate)
                {
                idbuf.Copy(prog.Left(sizeof(TMsvId)));
                if(idbuf() == KMsvNullIndexEntryId)
                    {
                    err = KErrGeneral;
                    }
                }
            else
                {
                const TUid uid(iOperation->Mtm());
                if(uid == KUidMsvLocalServiceMtm)
                    {
                    err = McliUtils::GetLocalProgressL(*iOperation).iError;
                    }
                else
                    {
                    TBuf<CBaseMtmUi::EProgressStringMaxLen> retString;
                    TInt totalEntryCount = 0;
                    TInt entriesDone = 0;
                    TInt currentEntrySize = 0;
                    TInt currentBytesTrans = 0;
                    err = iMtmStore->ClaimMtmUiL(uid).GetProgress(prog, retString, totalEntryCount, entriesDone, currentEntrySize, currentBytesTrans);
                    iMtmStore->ReleaseMtmUi(uid);
                    }
                }
            }
        }
    if(iOperation)
        {
        delete iOperation;
        iOperation = NULL;
        }
    User::LeaveIfError(err); // Caught in RunL()

    switch(iState)
        {
    case EStateIdle:        // should never occur...
        DeleteMessageL();
        break;
    case EStateDelete:
        CreateMessageL();
        break;
    case EStateCreate:
        CompleteObserver();
        break;
    default:
        break;
        }
    iMtm = (iOperation) ? (iOperation->Mtm()) : (KUidMsvLocalServiceMtm);
    IMUM_OUT();
    }


CSmtpResetPreCreationOp::CSmtpResetPreCreationOp(CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus,
    TMsvId aServiceId)
    : CSmtpPreCreationOpBase(aMsvSession, KResetCreateMsgPriority, aObserverRequestStatus)
    {
    IMUM_CONTEXT( CSmtpResetPreCreationOp::CSmtpResetPreCreationOp, 0, KImumMtmLog );
    IMUM_IN();
    
    iService = aServiceId;
    IMUM_OUT();
    }


CSmtpResetPreCreationOp::~CSmtpResetPreCreationOp()
    {
    IMUM_CONTEXT( CSmtpResetPreCreationOp::~CSmtpResetPreCreationOp, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    }


void CSmtpResetPreCreationOp::ConstructL()
    {
    IMUM_CONTEXT( CSmtpResetPreCreationOp::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    CSmtpPreCreationOpBase::ConstructL();

    // DeleteMessage operation must be run synchronously before Control is returned to the Active Scheduler
    // to ensure that an Email editor is not launched with a message which will be deleted by this operation.
    DeleteMessageL();
    IMUM_OUT();
    }


void CSmtpResetPreCreationOp::DeleteMessageL()
    {
    IMUM_CONTEXT( CSmtpResetPreCreationOp::DeleteMessageL, 0, KImumMtmLog );
    IMUM_IN();
    
    iStatus = KRequestPending;
    iOperation = iPreCreate->DeleteAllPreCreatedEmailsL(iService, iStatus);
    SetActive();
    iState = EStateDelete;
    if(!iOperation) // DeleteAllPreCreatedEmailsL() returns NULL if no messages to delete.
        {
        CompleteThis();
        }
    IMUM_OUT();
    }


void CSmtpResetPreCreationOp::CreateMessageL()
    {
    IMUM_CONTEXT( CSmtpResetPreCreationOp::CreateMessageL, 0, KImumMtmLog );
    IMUM_IN();
    
    iStatus = KRequestPending;
    iOperation = iPreCreate->PreCreateBlankEmailL(iService, iStatus);
    SetActive();
    iState = EStateCreate;
    IMUM_OUT();
    }


