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
*       Common email MTM operation for launching editors
*
*/



#include <eikenv.h>
#include <eikrutil.h>
#include <coehelp.h>

#include <MsvPrgReporter.h>
#include <MTMStore.h>
#include <mtmuibas.h>
#include <mtmuidef.hrh>
#include <msvuids.h>    // KUidMsvLocalServiceMtm
#include <miutmsg.h>    // CImEmailOperation
#include <miutset.h>

#include "ImumEditOperation.h"
#include "EmailUtils.H"
#include "PROGTYPE.H"
#include "ImumMtmLogging.h"

const TInt KEmailEditOpPriority = CActive::EPriorityHigh;

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
CImumEditOperation* CImumEditOperation::NewL(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    TUid aMtmType,
    const TEmailEditParams& aParams,
    TBool aSendReceipts)
    {
    IMUM_STATIC_CONTEXT( CImumEditOperation::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
     // static
    CImumEditOperation* op = new(ELeave) CImumEditOperation(
        aMailboxApi, aObserverRequestStatus, aMtmType, aParams);
    CleanupStack::PushL(op);
    op->ConstructL(aSendReceipts);
    CleanupStack::Pop();
    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
// ~CImumEditOperation
// ----------------------------------------------------------------------------
CImumEditOperation::~CImumEditOperation()
    {
    IMUM_CONTEXT( CImumEditOperation::~CImumEditOperation, 0, KImumMtmLog );
    IMUM_IN();
    
    Cancel();
    delete iOperation;
    delete iMsgEntry;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// ProgressL
// ----------------------------------------------------------------------------
const TDesC8& CImumEditOperation::ProgressL()
    {
    IMUM_CONTEXT( CImumEditOperation::ProgressL, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    return iProgress;
    }

// ----------------------------------------------------------------------------
// DoCancel
// ----------------------------------------------------------------------------
void CImumEditOperation::DoCancel()
    {
    IMUM_CONTEXT( CImumEditOperation::DoCancel, 0, KImumMtmLog );
    IMUM_IN();
    
    if(iOperation)
        {
        iOperation->Cancel();
        }
    iProgress().iError = KErrCancel;
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete(status, KErrNone);
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// RunL
// ----------------------------------------------------------------------------
void CImumEditOperation::RunL()
    {
    IMUM_CONTEXT( CImumEditOperation::RunL, 0, KImumMtmLog );
    IMUM_IN();
    
    TRequestStatus* status = &iObserverRequestStatus;
    switch(iState)
        {
        case EStateLaunching:
            // Completed.
            User::RequestComplete(status, iOperation->iStatus.Int() /*KErrNone*/);
            break;
        case EStateIdle:
        default:
            break;
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumEditOperation
// ----------------------------------------------------------------------------
CImumEditOperation::CImumEditOperation(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    TUid aMtmType,
    const TEmailEditParams& aParams)
    :
    CMsvOperation(aMailboxApi.MsvSession(), KEmailEditOpPriority, aObserverRequestStatus),
    iState(EStateIdle),
    iEditParams(aParams)
    {
    IMUM_CONTEXT( CImumEditOperation::CImumEditOperation, 0, KImumMtmLog );
    IMUM_IN();
    
    CActiveScheduler::Add(this);
    iMtm = aMtmType;

    TEmailEditProgress& prog = iProgress();
    prog.iError = KErrNone;
    prog.iId = iEditParams.iEditorParams.iId;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
void CImumEditOperation::ConstructL(TBool /*aSendReceipts*/)
    {
    IMUM_CONTEXT( CImumEditOperation::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    iMsgEntry = iMsvSession.GetEntryL(iEditParams.iEditorParams.iId);
    TMsvEmailEntry entry(iMsgEntry->Entry());
    iService = entry.iServiceId;

    DoLaunchEditorL();
    IMUM_OUT();

    }

// ----------------------------------------------------------------------------
// DoLaunchEditorL
// ----------------------------------------------------------------------------
void CImumEditOperation::DoLaunchEditorL()
    {
    IMUM_CONTEXT( CImumEditOperation::DoLaunchEditorL, 0, KImumMtmLog );
    IMUM_IN();
    
    if(iOperation)
        {
        delete iOperation;
        iOperation = NULL;
        }

    TEditorParameters launchParams(iEditParams.iEditorParams);
    launchParams.iFlags &= ~(EMtmUiFlagEditorPreferEmbedded | EMtmUiFlagEditorNoWaitForExit);
    launchParams.iFlags |= (iEditParams.iPreferences & EMtmUiFlagEditorPreferEmbedded) ? (EMsgLaunchEditorEmbedded) : (0);
    launchParams.iFlags |= (iEditParams.iPreferences & EMtmUiFlagEditorNoWaitForExit) ? (0) : (EMsgLaunchEditorThenWait);

    iOperation = MsgEditorLauncher::LaunchEditorApplicationL(
        iMsvSession,
        iMtm,
        iStatus,
        launchParams,
        iEditParams.
        iEditorFileName,
        KNullDesC8);

    iObserverRequestStatus = KRequestPending;
    iState = EStateLaunching;
    SetActive();
    IMUM_OUT();
    }

// end of file

