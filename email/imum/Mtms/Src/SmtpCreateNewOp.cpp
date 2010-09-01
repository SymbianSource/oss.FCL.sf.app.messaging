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
*       SMTP Create New Email operation
*
*/

#include <eikenv.h>
#include <miuthdr.h>
#include <smtpset.h>
#include <MTMStore.h>
#include <muiumsvuiserviceutilitiesinternal.h>
#include <MsvPrgReporter.h>
#include <SendUiConsts.h>
#include "ImumMtmLogging.h"
#include "SmtpCreateNewOp.h"
#include "SmtpTimerOp.h"
#include "EmailPreCreation.h"
#include "MsvConnectionValidation.h"
#include "MsvEmailConnectionProgressProvider.h"
#include "EmailUtils.H"


const TInt KNewMsgOpPriority = CActive::EPriorityStandard;
const TInt KIntervalAfterEditorLaunch = 100000; // 0.1 seconds

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
CSmtpCreateNewOp* CSmtpCreateNewOp::NewL(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    const TEditorParameters& aParams,
    const TDesC& aAppName,
    const TDesC8& aEditorProgress)
    {
    IMUM_STATIC_CONTEXT( CSmtpCreateNewOp::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CSmtpCreateNewOp* op = new(ELeave) CSmtpCreateNewOp(
        aMailboxApi, aObserverRequestStatus, aParams, aAppName);
    CleanupStack::PushL(op);
    op->ConstructL(aEditorProgress);
    CleanupStack::Pop();
    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
// ~CSmtpCreateNewOp
// ----------------------------------------------------------------------------
CSmtpCreateNewOp::~CSmtpCreateNewOp()
    {
    IMUM_CONTEXT( CSmtpCreateNewOp::~CSmtpCreateNewOp, 0, KImumMtmLog );
    IMUM_IN();
    
    delete iEditorProgress;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// RunL
// ----------------------------------------------------------------------------
void CSmtpCreateNewOp::RunL()
    {
    IMUM_CONTEXT( CSmtpCreateNewOp::RunL, 0, KImumMtmLog );
    IMUM_IN();
    
    DoRunL();
    
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// RunL
// ----------------------------------------------------------------------------
TInt CSmtpCreateNewOp::RunError(TInt aError)
	{
    IMUM_CONTEXT( CSmtpCreateNewOp::RunError, 0, KImumMtmLog );
    IMUM_IN();
	
	IMUM1( 0, "RunL leaved with error code %d", aError );
	
    if( iState == EStateLaunch || iState == EStateDeleteAll || iState == EStateCreateNew )
        {
        iProgress().iError = aError;
        iProgress().iType = TMsvLocalOperationProgress::ELocalNew;
        iProgress().iTotalNumberOfEntries = 1;
        iProgress().iNumberFailed = 1;
        }
    iMtm = KUidMsvLocalServiceMtm;
    CompleteObserver();	
    
    IMUM_OUT();	
    
    return KErrNone;
	}
 
// ----------------------------------------------------------------------------
// DoRunL
// ----------------------------------------------------------------------------
void CSmtpCreateNewOp::DoRunL()
    {
    IMUM_CONTEXT( CSmtpCreateNewOp::DoRunL, 0, KImumMtmLog );
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
            if(iState == EStateCreate || iState == EStateCreateNew)
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
                    err = iMtmStore->ClaimMtmUiL(uid).GetProgress(
                        prog,
                        retString,
                        totalEntryCount,
                        entriesDone,
                        currentEntrySize,
                        currentBytesTrans);
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
    case EStateIdle:
    case EStateCreateNew:
        LaunchEditorL();
        break;
    case EStateDeleteAll:
        CreateMessageL( ETrue );
        break;
    case EStateLaunch:
    case EStateWait:
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

// ----------------------------------------------------------------------------
// CSmtpCreateNewOp
// ----------------------------------------------------------------------------
CSmtpCreateNewOp::CSmtpCreateNewOp(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    const TEditorParameters& aParams,
    const TDesC& aAppName)
    :   
    CSmtpPreCreationOpBase(
        aMailboxApi.MsvSession(), KNewMsgOpPriority, aObserverRequestStatus ),    
    iParams(aParams),
    iAppName(aAppName)
    {
    IMUM_CONTEXT( CSmtpCreateNewOp::CSmtpCreateNewOp, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
void CSmtpCreateNewOp::ConstructL(const TDesC8& aEditorProgress)
    {
    IMUM_CONTEXT( CSmtpCreateNewOp::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    CSmtpPreCreationOpBase::ConstructL();
    iEditorProgress = aEditorProgress.AllocL();
    if(iParams.iFlags & EMsgCreateMessageToService)
        {
        // Context sensitive service.
        iService = iParams.iId;
        }
    else
        {
        iService = MsvUiServiceUtilitiesInternal::DefaultServiceForMTML(
            iMsvSession, KUidMsgTypeSMTP, ETrue);
        }
    iStatus = KRequestPending;
    SetActive();
    CompleteThis();
    iState = EStateIdle;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// LaunchEditorL
// ----------------------------------------------------------------------------
void CSmtpCreateNewOp::LaunchEditorL()
    {
    IMUM_CONTEXT( CSmtpCreateNewOp::LaunchEditorL, 0, KImumMtmLog );
    IMUM_IN();
    
    TMsvId id = iPreCreate->FindPreCreatedEmailL(iService, KMsvNullIndexEntryId);
    if(id != KMsvNullIndexEntryId)
        {
        // Adjust editor parameters for editing existing message.
        iParams.iFlags &= ~(EMsgCreateNewMessage | EMsgCreateMessageToService);
        iParams.iFlags |= EMsgEditNewMessage;
        iParams.iId = id;
        }
    iStatus = KRequestPending;
    iOperation = NULL;

    TRAPD( error, iOperation = MsgEditorLauncher::LaunchEditorApplicationL(
        iMsvSession,
        TUid::Uid( KSenduiMtmSmtpUidValue ),
        iStatus,
        iParams,
        iAppName,
        *iEditorProgress) );

    if ( error == KErrNotFound && iState == EStateIdle )
        {
        // try to recover from the situation where precreated mail is corrupted.
        iOperation = iPreCreate->DeleteAllPreCreatedEmailsL( iService, iStatus );
        iState = EStateDeleteAll;
        }
    else
        {
        if ( error != KErrNone )
            {
            User::Leave( KErrCancel );    
            }
        iState = EStateLaunch;
        }

    SetActive();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// WaitL
// ----------------------------------------------------------------------------
void CSmtpCreateNewOp::WaitL()
    {
    IMUM_CONTEXT( CSmtpCreateNewOp::WaitL, 0, KImumMtmLog );
    IMUM_IN();
    
    iStatus = KRequestPending;
    CSmtpTimerOp* op = new(ELeave) CSmtpTimerOp(
        iMailboxApi->MsvSession(), CActive::EPriorityIdle, iStatus);
    CleanupStack::PushL(op);
    op->ConstructL();
    op->After(TTimeIntervalMicroSeconds32(KIntervalAfterEditorLaunch));
    CleanupStack::Pop(op);
    iOperation = op;
    SetActive();
    iState = EStateWait;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CreateMessageL
// ----------------------------------------------------------------------------
void CSmtpCreateNewOp::CreateMessageL( TBool aLaunchEditorAfterCreate /*= EFalse*/ )
    {
    IMUM_CONTEXT( CSmtpCreateNewOp::CreateMessageL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Only create if there isn't a message there already.
    iStatus = KRequestPending;
    if(iPreCreate->FindPreCreatedEmailL(iService, iParams.iId) == KMsvNullIndexEntryId)
        {
        iOperation = iPreCreate->PreCreateBlankEmailL(iService, iStatus);
        }
    else
        {
        CompleteThis();
        }
    SetActive();
    iState = aLaunchEditorAfterCreate ? EStateCreateNew: EStateCreate;
    IMUM_OUT();
    }

// End of File
