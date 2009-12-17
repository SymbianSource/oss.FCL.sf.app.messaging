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
*       Common email MTM wrapper operation
*
*/


#include <eikenv.h>
#include <eikrutil.h>
#include <coehelp.h>
#include <MsvPrgReporter.h>
#include <MTMStore.h>
#include <mtmuibas.h>
#include <msvuids.h>    // KUidMsvLocalServiceMtm
#include <miutmsg.h>    // CImEmailOperation
#include <miutset.h>

#include "ImumFetchAndEditOperation.h"
#include "ImumEditOperation.h"
#include "EmailUtils.H"
#include "ImumPanic.h"
#include "ImumMtmLogging.h"

const TInt KEmailFetchAndEditOpPriority = CActive::EPriorityStandard;

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CImumFetchAndEditOperation
// ----------------------------------------------------------------------------
CImumFetchAndEditOperation::CImumFetchAndEditOperation(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter,
    const TEmailEditParams& aParams,
    TBool aAckReceipts)
    :
    CImumDiskSpaceObserverOperation(
        aMailboxApi,
        KEmailFetchAndEditOpPriority,
        aObserverRequestStatus,
        aReporter),
    iState(EStateIdle),
    iEditParams(aParams),
    iAckReceipts(aAckReceipts)
    {
    IMUM_CONTEXT( CImumFetchAndEditOperation::CImumFetchAndEditOperation, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    }

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
CImumFetchAndEditOperation* CImumFetchAndEditOperation::NewL(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter,
    const TEmailEditParams& aParams,
    TBool aAckReceipts )
    {
    IMUM_STATIC_CONTEXT( CImumFetchAndEditOperation::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    return new ( ELeave ) CImumFetchAndEditOperation(
        aMailboxApi, aObserverRequestStatus,
        aReporter, aParams, aAckReceipts );
    }

// ----------------------------------------------------------------------------
// ~CImumFetchAndEditOperation
// ----------------------------------------------------------------------------
CImumFetchAndEditOperation::~CImumFetchAndEditOperation()
    {
    IMUM_CONTEXT( CImumFetchAndEditOperation::~CImumFetchAndEditOperation, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    }

// ----------------------------------------------------------------------------
// GetErrorProgressL
// ----------------------------------------------------------------------------
const TDesC8& CImumFetchAndEditOperation::GetErrorProgressL( TInt aError )
    {
    IMUM_CONTEXT( CImumFetchAndEditOperation::GetErrorProgressL, 0, KImumMtmLog );
    IMUM_IN();
    
    // DoRunL() left, failed to switch to launch editor state.
    iLocalProgress().iType = TMsvLocalOperationProgress::ELocalNone;
    iLocalProgress().iTotalNumberOfEntries = 0;
    iLocalProgress().iNumberCompleted = 0;
    iLocalProgress().iNumberFailed = 0;
    iLocalProgress().iNumberRemaining = 0;
    iLocalProgress().iError = aError;
    iLocalProgress().iId = iEditParams.iEditorParams.iId;
    IMUM_OUT();
    return iLocalProgress;
    }

// ----------------------------------------------------------------------------
// SetFetchOpL
// ----------------------------------------------------------------------------
void CImumFetchAndEditOperation::SetFetchOpL( CMsvOperation* aFetchOp )
    {
    IMUM_CONTEXT( CImumFetchAndEditOperation::SetFetchOpL, 0, KImumMtmLog );
    IMUM_IN();
    
    __ASSERT_ALWAYS( aFetchOp, User::Panic( KImumMtmUiPanic, EPanicOpBadFetchOp ) );
    __ASSERT_ALWAYS( !iOperation, User::Panic( KImumMtmUiPanic, EPanicAlreadySetFetchOp ) );
    iOperation = aFetchOp;
    iService = iOperation->Service();
    BaseConstructL( iOperation->Mtm() );
    iState = EStateFetching;
    iStatus = KRequestPending;
    SetActive();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// RequestStatus
// ----------------------------------------------------------------------------
TRequestStatus& CImumFetchAndEditOperation::RequestStatus()
    {
    IMUM_CONTEXT( CImumFetchAndEditOperation::RequestStatus, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    return iStatus;
    }

// ----------------------------------------------------------------------------
// DoRunL
// ----------------------------------------------------------------------------
void CImumFetchAndEditOperation::DoRunL()
    {
    IMUM_CONTEXT( CImumFetchAndEditOperation::DoRunL, 0, KImumMtmLog );
    IMUM_IN();
    
    iActualMtm = iMtm;
    iMtm = KUidMsvLocalServiceMtm;  // Error progress is local MTM.
    switch(iState)
        {
        case EStateFetching:
            // The fetch has completed.
            {
            if(KErrNone == GetOperationCompletionCodeL())
                {
                DoEditL();
                }
            else
                {
                CompleteObserver();
                }
            }
            break;
        case EStateEditing:
            // Edit has completed.
            CompleteObserver( iOperation->iStatus.Int() );
            iState = EStateComplete;
            break;

        case EStateIdle:
        case EStateComplete:
        default:
            break;
        }
    iMtm = iOperation->Mtm();   // Completed without error, reset MTM.
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// DoEditL
// ----------------------------------------------------------------------------
void CImumFetchAndEditOperation::DoEditL()
    {
    IMUM_CONTEXT( CImumFetchAndEditOperation::DoEditL, 0, KImumMtmLog );
    IMUM_IN();
    
    iReporter.MakeProgressVisibleL( EFalse );     // Get rid of the dialog.
    iMtmUi->BaseMtm().SwitchCurrentEntryL( iEditParams.iEditorParams.iId );
    delete iOperation;
    iOperation = NULL;
    iStatus = KRequestPending;

    iOperation = CImumEditOperation::NewL(
        iMailboxApi, iStatus, iActualMtm, iEditParams, iAckReceipts );

    iState = EStateEditing;
    SetActive();
    IMUM_OUT();
    }

// End of File
