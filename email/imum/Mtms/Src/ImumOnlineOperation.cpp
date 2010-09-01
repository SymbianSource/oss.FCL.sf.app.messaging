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
*       Common base class for email online operations
*
*/


#include <eikenv.h>
#include <eikrutil.h>
#include <MsvPrgReporter.h>
#include <MTMStore.h>
#include <mtmuibas.h>
#include <ImumInternalApi.h>        // CImumInternalApi

#include "ImumOnlineOperation.h"
#include "ImumPanic.h"
#include "ImumMtmLogging.h"


// ----------------------------------------------------------------------------
// CImumOnlineOperation::~CImumOnlineOperation()
// ----------------------------------------------------------------------------
//
CImumOnlineOperation::~CImumOnlineOperation()
    {
    IMUM_CONTEXT( CImumOnlineOperation::~CImumOnlineOperation, 0, KImumMtmLog );
    IMUM_IN();

    Cancel();
    delete iOperation;
    if(iMtmUi)
        {
        iReporter.MtmStore().ReleaseMtmUi(iMtmUi->Type());
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumOnlineOperation::ProgressL()
// ----------------------------------------------------------------------------
//
const TDesC8& CImumOnlineOperation::ProgressL()
    {
    IMUM_CONTEXT( CImumOnlineOperation::ProgressL, 0, KImumMtmLog );
    IMUM_IN();

    IMUM_OUT();
    if(iError != KErrNone)
        {
        return GetErrorProgressL(iError);
        }
    else if(iOperation)
        {
        return iOperation->ProgressL();
        }
    return iDummyProg;
    }

// ----------------------------------------------------------------------------
// CImumOnlineOperation::CImumOnlineOperation()
// ----------------------------------------------------------------------------
//
CImumOnlineOperation::CImumOnlineOperation(
    CImumInternalApi& aMailboxApi,
    TInt aPriority,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter)
    :
    CMsvOperation(
        aMailboxApi.MsvSession(),
        aPriority,
        aObserverRequestStatus),
    iReporter(aReporter),
    iMailboxApi( aMailboxApi ),
    iEikEnv(CEikonEnv::Static())
    {
    IMUM_CONTEXT( CImumOnlineOperation::CImumOnlineOperation, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();

    }

// ----------------------------------------------------------------------------
// CImumOnlineOperation::BaseConstructL()
// ----------------------------------------------------------------------------
//
void CImumOnlineOperation::BaseConstructL(TUid aMtmType)
    {
    IMUM_CONTEXT( CImumOnlineOperation::BaseConstructL, 0, KImumMtmLog );
    IMUM_IN();

    iObserverRequestStatus = KRequestPending;
    CActiveScheduler::Add(this);
    iMtmUi = &(iReporter.MtmStore().ClaimMtmUiL(aMtmType));
    iMtm = aMtmType;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumOnlineOperation::DoCancel()
// ----------------------------------------------------------------------------
//
void CImumOnlineOperation::DoCancel()
    {
    IMUM_CONTEXT( CImumOnlineOperation::DoCancel, 0, KImumMtmLog );
    IMUM_IN();

    if(iOperation)
        {
        iOperation->Cancel();
        }
    CompleteObserver();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumOnlineOperation::RunL()
// ----------------------------------------------------------------------------
//
void CImumOnlineOperation::RunL()
    {
    IMUM_CONTEXT( CImumOnlineOperation::RunL, 0, KImumMtmLog );
    IMUM_IN();

    // First do a disk space check, so the operation can continue
    iError = DoDiskSpaceCheckL();

    // If still free disk space, the actual operation can be continued
    if ( iError == KErrNone )
        {
        DoRunL();
        }
	else
		{
		User::Leave( iError );
		}
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumOnlineOperation::RunError()
// ----------------------------------------------------------------------------
//
TInt CImumOnlineOperation::RunError( TInt aError )
    {
    IMUM_CONTEXT( CImumOnlineOperation::RunError, 0, KImumMtmLog );
    IMUM_IN();

    IMUM1( 0, "RunL error %d", aError );
    CompleteObserver( aError );

    IMUM_OUT();
     // RunError must return KErrNone to active sheduler.
     // We return the error, since we do not fully handle the error
     // We may want the framework to show some notes etc...
    return aError;
    }

// ----------------------------------------------------------------------------
// CImumOnlineOperation::CompleteObserver()
// ----------------------------------------------------------------------------
//
void CImumOnlineOperation::CompleteObserver( TInt aStatus /*= KErrNone*/ )
    {
    IMUM_CONTEXT( CImumOnlineOperation::CompleteObserver, 0, KImumMtmLog );
    IMUM_IN();

    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete(status, aStatus);
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumOnlineOperation::CompleteThis()
// ----------------------------------------------------------------------------
//
void CImumOnlineOperation::CompleteThis()
    {
    IMUM_CONTEXT( CImumOnlineOperation::CompleteThis, 0, KImumMtmLog );
    IMUM_IN();

    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumOnlineOperation::GetOperationCompletionCodeL()
// ----------------------------------------------------------------------------
//
TInt CImumOnlineOperation::GetOperationCompletionCodeL()
    {
    IMUM_CONTEXT( CImumOnlineOperation::GetOperationCompletionCodeL, 0, KImumMtmLog );
    IMUM_IN();

    if(iStatus.Int() != KErrNone)
        {
        IMUM_OUT();
        return iStatus.Int();
        }
    __ASSERT_DEBUG( iMtmUi, User::Panic( KImumMtmUiPanic,EPanicOpNoMtmUi ) );
    TBuf<CBaseMtmUi::EProgressStringMaxLen> retString;
    TInt totalEntryCount = 0;
    TInt entriesDone = 0;
    TInt currentEntrySize = 0;
    TInt currentBytesTrans = 0;
    IMUM_OUT();

    return iMtmUi->GetProgress(
        ProgressL(),
        retString,
        totalEntryCount,
        entriesDone,
        currentEntrySize,
        currentBytesTrans);
    }

// ----------------------------------------------------------------------------
// CImumOnlineOperation::InvokeClientMtmAsyncFunctionL()
// ----------------------------------------------------------------------------
//
void CImumOnlineOperation::InvokeClientMtmAsyncFunctionL(
    TInt aFunctionId,
    TMsvId aEntryId,
    TMsvId aContextId,
    const TDesC8& aParams)
    {
    IMUM_CONTEXT( CImumOnlineOperation::InvokeClientMtmAsyncFunctionL, 0, KImumMtmLog );
    IMUM_IN();

    CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL(sel);
    sel->AppendL(aEntryId);
    InvokeClientMtmAsyncFunctionL(aFunctionId, *sel, aContextId, aParams);
    CleanupStack::PopAndDestroy(); // sel
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumOnlineOperation::InvokeClientMtmAsyncFunctionL()
// ----------------------------------------------------------------------------
//
void CImumOnlineOperation::InvokeClientMtmAsyncFunctionL(
    TInt aFunctionId,
    const CMsvEntrySelection& aSel,
    TMsvId aContextId,
    const TDesC8& aParams)
    {
    IMUM_CONTEXT( CImumOnlineOperation::InvokeClientMtmAsyncFunctionL, 0, KImumMtmLog );
    IMUM_IN();

    __ASSERT_DEBUG(iMtmUi, User::Panic( KImumMtmUiPanic,EPanicOpNoMtmUi));
    CBaseMtm& mtm = iMtmUi->BaseMtm();
    mtm.SwitchCurrentEntryL(aContextId);
    HBufC8* params = aParams.AllocLC();
    TPtr8 ptr(params->Des());
    // Delete previous operation if it exist
    if ( iOperation )
        {
        delete iOperation;
        iOperation = NULL;
        }
    iOperation = mtm.InvokeAsyncFunctionL(aFunctionId, aSel, ptr, iStatus);
    CleanupStack::PopAndDestroy(); // params
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumOnlineOperation::ReDisplayProgressL()
// ----------------------------------------------------------------------------
//
void CImumOnlineOperation::ReDisplayProgressL(
    TInt /*aTitleResourceId*/, TMsvId /*aServiceId*/)
    {
    IMUM_CONTEXT( CImumOnlineOperation::ReDisplayProgressL, 0, KImumMtmLog );
    IMUM_IN();

    iReporter.MakeProgressVisibleL(ETrue);
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumOnlineOperation::DoDiskSpaceCheckL()
// ----------------------------------------------------------------------------
//
TInt CImumOnlineOperation::DoDiskSpaceCheckL()
    {
    IMUM_CONTEXT( CImumOnlineOperation::DoDiskSpaceCheckL, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();

    // As default, return KErrNone
    return KErrNone;
    }


