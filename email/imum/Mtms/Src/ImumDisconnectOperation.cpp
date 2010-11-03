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
*       Very simple layer over base class just for setting up progress reporter
*
*/


#include <eikenv.h>
#include <eikrutil.h>
#include <coehelp.h>

#include <ImumInternalApi.h>            // CImumInternalApi
#include <MsvPrgReporter.h>
#include <MTMStore.h>
#include <mtmuibas.h>
#include <msvuids.h>    // KUidMsvLocalServiceMtm
#include <miutmsg.h>    // CImEmailOperation
#include <miutset.h>
#include <SenduiMtmUids.h>

#include <imum.rsg>
#include "ImumDisconnectOperation.h"
#include "EmailUtils.H"
#include "ImumMtmLogging.h"

const TInt KEmailDisconnectOpPriority = CActive::EPriorityStandard;

// ----------------------------------------------------------------------------
// CImumDisconnectOperation::
//
// ----------------------------------------------------------------------------
CImumDisconnectOperation* CImumDisconnectOperation::NewL(CImumInternalApi& aMailboxApi, TRequestStatus& aObserverRequestStatus, MMsvProgressReporter& aReporter, TMsvId aService, TInt aFunctionId, TUid aMtmType)
    {
    IMUM_STATIC_CONTEXT( CImumDisconnectOperation::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CImumDisconnectOperation* op = new(ELeave) CImumDisconnectOperation(aMailboxApi, aObserverRequestStatus, aReporter, aService);
    CleanupStack::PushL(op);
    op->ConstructL(aFunctionId, aMtmType);
    CleanupStack::Pop();
    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
// CImumDisconnectOperation::
//
// ----------------------------------------------------------------------------
CImumDisconnectOperation::~CImumDisconnectOperation()
    {
    IMUM_CONTEXT( CImumDisconnectOperation::~CImumDisconnectOperation, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    }

// ----------------------------------------------------------------------------
// CImumDisconnectOperation::
//
// ----------------------------------------------------------------------------
const TDesC8& CImumDisconnectOperation::GetErrorProgressL(TInt /*aError*/)
    {
    IMUM_CONTEXT( CImumDisconnectOperation::GetErrorProgressL, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    // Never gets called since iError always == KErrNone, because DoRunL() cannot leave.
    return iDummyProg;
    }

// ----------------------------------------------------------------------------
// ProgressL
//
// ----------------------------------------------------------------------------
const TDesC8& CImumDisconnectOperation::ProgressL()
    {
    IMUM_CONTEXT( CImumDisconnectOperation::ProgressL, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    return iDummyProg;
    }

// ----------------------------------------------------------------------------
// CImumDisconnectOperation::
//
// ----------------------------------------------------------------------------
void CImumDisconnectOperation::DoRunL()
    {
    IMUM_CONTEXT( CImumDisconnectOperation::DoRunL, 0, KImumMtmLog );
    IMUM_IN();
    
    CompleteObserver();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumDisconnectOperation::
//
// ----------------------------------------------------------------------------
CImumDisconnectOperation::CImumDisconnectOperation(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aService)
    :
    CImumOnlineOperation(
        aMailboxApi,
        KEmailDisconnectOpPriority,
        aObserverRequestStatus,
        aReporter)
    {
    IMUM_CONTEXT( CImumDisconnectOperation::CImumDisconnectOperation, 0, KImumMtmLog );
    IMUM_IN();
    
    iService = aService;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumDisconnectOperation::
//
// ----------------------------------------------------------------------------
void CImumDisconnectOperation::ConstructL(TInt aFunctionId, TUid aMtmType)
    {
    IMUM_CONTEXT( CImumDisconnectOperation::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    BaseConstructL(aMtmType);
    iReporter.SetTitleL( R_EMAIL_DISCONNECT );
    iReporter.SetProgressDecoder(*this);
    iReporter.SetCanCancelL( EFalse );

    InvokeClientMtmAsyncFunctionL(aFunctionId, iService, iService);
    SetActive();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumDisconnectOperation::
//
// ----------------------------------------------------------------------------
TInt CImumDisconnectOperation::DecodeProgress(
    const TDesC8& /*aProgress*/,
    TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString,
    TInt& aTotalEntryCount,
    TInt& aEntriesDone,
    TInt& aCurrentEntrySize,
    TInt& aCurrentBytesTrans,
    TBool /*aInternal*/)
    {
    IMUM_CONTEXT( CImumDisconnectOperation::DecodeProgress, 0, KImumMtmLog );
    IMUM_IN();
    
    aTotalEntryCount=0;     //  No quantitative progress to return
    aEntriesDone=0;
    aCurrentEntrySize=0;
    aCurrentBytesTrans=0;

    aReturnString.Zero(); // when returning zero then muiu does not change current progress.
    IMUM_OUT();

    return KErrNone;
    }

