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
*       Base class for operations requiring connection
*
*/


#include <eikenv.h>
#include <eikrutil.h>
#include <imapcmds.h>

#include "Imap4ConnectedOp.h"
#include "EmailMsgSizerOperation.h"
#include "EmailUtils.H"
#include "ImumDisconnectOperation.h"
#include "IMAPPRGR.H"
#include "ImumMtmLogging.h"


#include "ImumPanic.h"

// Constants and defines
const TInt KConnectedOpPriority = CActive::EPriorityStandard;
const TInt KConnectedOpConnectPriority = CActive::EPriorityStandard;

CImap4ConnectedOp::~CImap4ConnectedOp()
    {
    IMUM_CONTEXT( CImap4ConnectedOp::~CImap4ConnectedOp, 0, KImumMtmLog );
    IMUM_IN();
    
    delete iConnectedOpErrorProgress;
    IMUM_OUT();
    }

const TDesC8& CImap4ConnectedOp::ProgressL()
    {
    IMUM_CONTEXT( CImap4ConnectedOp::ProgressL, 0, KImumMtmLog );
    IMUM_IN();
    
    if(iConnectedOpErrorProgress && (iState == EStateIdle))
        {
        // Completed, but with an error during connected operation.
        return *iConnectedOpErrorProgress;
        }
    IMUM_OUT();
    return CImumOnlineOperation::ProgressL();
    }

const TDesC8& CImap4ConnectedOp::GetErrorProgressL(TInt aError)
    {
    IMUM_CONTEXT( CImap4ConnectedOp::GetErrorProgressL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Called if DoRunL leaves.
    switch(iState)
        {
        case EStateConnecting:
        case EStateDisconnecting:
            if(!iProgressBuf().iGenericProgress.iErrorCode)
                {
                TImap4CompoundProgress& prog = iProgressBuf();
                prog.iGenericProgress.iState =
                    (iState == EStateConnecting) ?
                    (TImap4GenericProgress::EConnecting) :
                    (TImap4GenericProgress::EDisconnecting);
                prog.iGenericProgress.iErrorCode = aError;
                }
            IMUM_OUT();
            return iProgressBuf;

        case EStateDoingOp:
        case EStateIdle:
        default:
            IMUM_OUT();
            // Call pure virtual function to get progress for the connected operation.
            return ConnectedOpErrorProgressL(aError);

        }
    }

CImap4ConnectedOp::CImap4ConnectedOp(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aServiceId )
    :
    CImumDiskSpaceObserverOperation(aMailboxApi, KConnectedOpPriority, aStatus, aReporter),
    iConnectionCompletionState(CImapConnectionOp::EWithSyncCompleteAfterFullSync)
    {
    IMUM_CONTEXT( CImap4ConnectedOp::CImap4ConnectedOp, 0, KImumMtmLog );
    IMUM_IN();
    
    iService = aServiceId;
    IMUM_OUT();
    }

void CImap4ConnectedOp::ConstructL( TInt /* aFuncId */ )
    {
    IMUM_CONTEXT( CImap4ConnectedOp::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    BaseConstructL(KUidMsgTypeIMAP4);

    StartL();
    IMUM_OUT();
    }

void CImap4ConnectedOp::StartL()
    {
    IMUM_CONTEXT( CImap4ConnectedOp::StartL, 0, KImumMtmLog );
    IMUM_IN();
    
    CMsvEntry* centry = iMsvSession.GetEntryL(iService);
    iDisconnect = !(centry->Entry().Connected());
    // Only disconnect afterwards if not currently connected.
    delete centry;

    if(iDisconnect)
        {
        iState = EStateConnecting;
        DoConnectL();
        }
    else
        {
        // Already connected, get on with it.
        iState = EStateDoingOp;
        DoConnectedOpL();
        }
    IMUM_OUT();
    }

void CImap4ConnectedOp::RunL()
    {
    IMUM_CONTEXT( CImap4ConnectedOp::RunL, 0, KImumMtmLog );
    IMUM_IN();
    
    const TInt previousError = iError;
    TRAP(iError, DoRunL());	// CSI: 86 # Needed here to properly handle error situations
    if(iError != KErrNone)
        {
        // Need to ensure we disconnect if we have connected.
        if( (iError == KErrNoMemory) || (iState == EStateDisconnecting) )
            {
            // OOM and/or failed to create disconnect operation. Call StopService()
            iMsvSession.StopService(iService);
            // Ignore return, nothing we can do.
            }
        else if( (iState == EStateDoingOp) && (iDisconnect) )
            {
            // Failed to start connected operation. Need to disconnect.
            SetActive();
            CompleteThis();
            IMUM_OUT();
            // Next DoRunL() will attempt to disconnect.
            return;
            }
        CompleteObserver();
        }
    else if(previousError != KErrNone)
        {
        iError = previousError;
        }
    IMUM_OUT();
    }
    
void CImap4ConnectedOp::DoCancel()
    {
    IMUM_CONTEXT( CImap4ConnectedOp::DoCancel, 0, KImumMtmLog );
    IMUM_IN();
    
    CImumOnlineOperation::DoCancel();
    if ( iState == EStateConnecting )
        {
        // Cancelled while doing connected op. Need to disconnect.
        iMsvSession.StopService( iService );
        // Ignore return, nothing we can do.
        }
    IMUM_OUT();
    }

void CImap4ConnectedOp::DoRunL()
    {
    IMUM_CONTEXT( CImap4ConnectedOp::DoRunL, 0, KImumMtmLog );
    IMUM_IN();
    
    switch(iState)
        {
        case EStateConnecting:
            // Connection completed.
            if(GetOperationCompletionCodeL() != KErrNone)
                {
                iState = EStateIdle;
                CompleteObserver();
                }
            else
                {
                iState = EStateDoingOp;
                DoConnectedOpL();
                }
            break;
        case EStateDoingOp:
            // Connected operation completed.
            if(GetOperationCompletionCodeL() != KErrNone)
                {
                // Store the failure progress.
                __ASSERT_DEBUG(!iConnectedOpErrorProgress,
                    User::Panic(KImumMtmUiPanic,EPanicAlreadyHaveErrProg));
                iConnectedOpErrorProgress = iOperation->ProgressL().AllocL();
                }

            /* Background Handling */
            MsvEmailMtmUiUtils::CallNewMessagesL( iService );
            iState = EStateDisconnecting;
            DoDisconnectL();
            break;
        case EStateDisconnecting:
            // Disonnection completed.
            iState = EStateIdle;
            CompleteObserver();
            break;
        case EStateIdle:
        default:
            break;
        }
    if(iOperation)
        {
        iMtm = iOperation->Mtm();
        }
    IMUM_OUT();
    }

void CImap4ConnectedOp::DoConnectL()
    {
    IMUM_CONTEXT( CImap4ConnectedOp::DoConnectL, 0, KImumMtmLog );
    IMUM_IN();
    
    __ASSERT_DEBUG(!iOperation, User::Panic(KImumMtmUiPanic,EPanicOpAlreadySet));
    delete iOperation;
    iOperation = NULL;
    iStatus = KRequestPending;
    iOperation = CImapConnectionOp::NewL(iMailboxApi.MsvSession(), KConnectedOpConnectPriority,
        iStatus, iService, iReporter, iConnectionCompletionState);
    SetActive();
    IMUM_OUT();
    }

void CImap4ConnectedOp::DoDisconnectL()
    {
    IMUM_CONTEXT( CImap4ConnectedOp::DoDisconnectL, 0, KImumMtmLog );
    IMUM_IN();
    
    iStatus = KRequestPending;
    if(iDisconnect)
        {
        delete iOperation;
        iOperation = NULL;
        iOperation = CImumDisconnectOperation::NewL(
            iMailboxApi,
            iStatus,
            iReporter,
            iService,
            KIMAP4MTMDisconnect,
            KUidMsgTypeIMAP4);
        }
    else
        {
        CompleteThis();
        }
    SetActive();
    IMUM_OUT();
    }


