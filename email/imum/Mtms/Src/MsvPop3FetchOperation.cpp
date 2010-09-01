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
*       Fetch message(s) operation, using client MTM Get Mail API
*
*/

#include <eikenv.h>
#include <StringLoader.h>
#include <MsvPrgReporter.h>
#include <ImumUtils.rsg>
#include "ImumPanic.h"
#include "MsvPop3FetchOperation.h"
#include "MsvPop3ConnectOp.h"
#include "MsvConnectionValidation.h"
#include "MsvEmailConnectionProgressProvider.h"
#include "EmailUtils.H"
#include "ImumDisconnectOperation.h"

// Constants and defines
const TInt KFetchOpPriority = CActive::EPriorityStandard;


// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
CMsvPop3FetchOperation* CMsvPop3FetchOperation::NewL(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter,
    TInt aFunctionId,
    TMsvId aService,
    const TImPop3GetMailInfo& aGetMailInfo,
    const CMsvEntrySelection& aSel)
    {
    IMUM_STATIC_CONTEXT( CMsvPop3FetchOperation::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CMsvPop3FetchOperation* op = new(ELeave) CMsvPop3FetchOperation(
        aMailboxApi,
        aObserverRequestStatus,
        aReporter,
        aFunctionId,
        aService,
        aGetMailInfo);
    CleanupStack::PushL(op);
    op->ConstructL(aSel);
    CleanupStack::Pop(); // op
    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
//Overloaded NewL for partial fetch operation
// ----------------------------------------------------------------------------
CMsvPop3FetchOperation* CMsvPop3FetchOperation::NewL(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter,
    TInt aFunctionId,
    TMsvId aService,
    const TImPop3GetMailInfo& aGetMailInfo,
    const CMsvEntrySelection& aSel,
    TInt aLimit)
    {
    IMUM_STATIC_CONTEXT( CMsvPop3FetchOperation::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CMsvPop3FetchOperation* op = new(ELeave) CMsvPop3FetchOperation(
        aMailboxApi,
        aObserverRequestStatus,
        aReporter,
        aFunctionId,
        aService,
        aGetMailInfo,
        aLimit);
    CleanupStack::PushL(op);
    op->ConstructL(aSel);
    CleanupStack::Pop(); // op
    IMUM_OUT();
    return op;
    }


// ----------------------------------------------------------------------------
// ~CMsvPop3FetchOperation
// ----------------------------------------------------------------------------
CMsvPop3FetchOperation::~CMsvPop3FetchOperation()
    {
    IMUM_CONTEXT( CMsvPop3FetchOperation::~CMsvPop3FetchOperation, 0, KImumMtmLog );
    IMUM_IN();
    
    delete iFetchErrorProgress;
    delete iSelection;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// ProgressL
// ----------------------------------------------------------------------------
const TDesC8& CMsvPop3FetchOperation::ProgressL()
    {
    IMUM_CONTEXT( CMsvPop3FetchOperation::ProgressL, 0, KImumMtmLog );
    IMUM_IN();
    
    if(iFetchErrorProgress && (iState == EStateIdle))
        {
        IMUM_OUT();
        // Completed, but with an error during fetch.
        return *iFetchErrorProgress;
        }
    IMUM_OUT();
    return CImumOnlineOperation::ProgressL();
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
void CMsvPop3FetchOperation::ConstructL(const CMsvEntrySelection& aSel)
    {
    IMUM_CONTEXT( CMsvPop3FetchOperation::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    BaseConstructL(KUidMsgTypePOP3);
    iSelection = aSel.CopyL();
    iSelection->InsertL(0, iService);
    // For Get Mail API, first selection element must be service.
    DoConnectL();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// DoConnectL
// ----------------------------------------------------------------------------
void CMsvPop3FetchOperation::DoConnectL()
    {
    IMUM_CONTEXT( CMsvPop3FetchOperation::DoConnectL, 0, KImumMtmLog );
    IMUM_IN();
    
    iState = EStateConnecting;
    iStatus = KRequestPending;
    CMsvPop3ConnectOp* connOp = CMsvPop3ConnectOp::NewL(
        iStatus,
        iReporter,
        iService,
        0 );
    // when connecting for the fetch operation, don't let connect operation to do fetch,
    // because we do it by ourself. That's why give 0 to connect operation.
    delete iOperation;
    iOperation = connOp;

    //We always leave connection on after fetching.
    //If fetching from off line state and want to disconnect afterwards,
    //checks are needed here.
    iDisconnect = EFalse;

    TMsvEntry serviceEntry;
    TMsvId serviceId;
    
    if ( iMsvSession.GetEntry( Service(), serviceId, serviceEntry ) == KErrNone )
        {
        
        if ( !serviceEntry.Connected() )
        	{
        	HBufC* text = StringLoader::LoadLC(
        			R_EMAIL_CONNECTING_SERVER, serviceEntry.iDetails, iEikEnv );
        	iReporter.SetTitleL( *text );
        	CleanupStack::PopAndDestroy( text );
        	}
        }   	    	
    // Go active ourselves.
    SetActive();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// DoFetchL
// ----------------------------------------------------------------------------
void CMsvPop3FetchOperation::DoFetchL()
    {
    IMUM_CONTEXT( CMsvPop3FetchOperation::DoFetchL, 0, KImumMtmLog );
    IMUM_IN();
    
    iState = EStateFetching;

    // Switch operations.
    delete iOperation;
    iOperation = NULL;
    iStatus = KRequestPending;

    // Filters are not used when performing 'fetch' operation, use normal getmail info instead
    TPckg<TImPop3GetMailInfo> param(iGetMailInfo);
    InvokeClientMtmAsyncFunctionL(iFunctionId, *iSelection, iService, param );
    SetActive();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// DoDisconnectL
// ----------------------------------------------------------------------------
void CMsvPop3FetchOperation::DoDisconnectL()
    {
    IMUM_CONTEXT( CMsvPop3FetchOperation::DoDisconnectL, 0, KImumMtmLog );
    IMUM_IN();
    
    iState = EStateDisconnecting;
    TMsvId temp;
    TMsvEntry entry;
    const TInt err = iMsvSession.GetEntry(iService, temp, entry);
    if( (iDisconnect) && (err==KErrNone) && (entry.Connected()) )
        {
        delete iOperation;
        iOperation = NULL;
        iStatus = KRequestPending;
        iOperation = CImumDisconnectOperation::NewL(
            iMailboxApi,
            iStatus,
            iReporter,
            iService,
            KPOP3MTMDisconnect,
            KUidMsgTypePOP3);
        }
    else
        {
        CompleteThis();
        }
    SetActive();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CMsvPop3FetchOperation
// ----------------------------------------------------------------------------
CMsvPop3FetchOperation::CMsvPop3FetchOperation(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter,
    TInt aFunctionId,
    TMsvId aService,
    const TImPop3GetMailInfo& aGetMailInfo)
    :
    CImumDiskSpaceObserverOperation(
    aMailboxApi,
    KFetchOpPriority,
    aObserverRequestStatus,
    aReporter),
    iFunctionId(aFunctionId),
    iGetMailInfo(aGetMailInfo)
    {
    IMUM_CONTEXT( CMsvPop3FetchOperation::CMsvPop3FetchOperation, 0, KImumMtmLog );
    IMUM_IN();
    
    iService = aService;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CMsvPop3FetchOperation. Overloaded constructor for
// partial fetch operation
// ----------------------------------------------------------------------------
CMsvPop3FetchOperation::CMsvPop3FetchOperation(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter,
    TInt aFunctionId,
    TMsvId aService,
    const TImPop3GetMailInfo& aGetMailInfo,
    TInt aLimit)
    :
    CImumDiskSpaceObserverOperation(
    aMailboxApi,
    KFetchOpPriority,
    aObserverRequestStatus,
    aReporter),
    iFunctionId(aFunctionId),
    iGetMailInfo(aGetMailInfo),
    iPopulateLimit( aLimit )
    {
    IMUM_CONTEXT( CMsvPop3FetchOperation::CMsvPop3FetchOperation, 0, KImumMtmLog );
    IMUM_IN();
    
    iService = aService;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// RunL
// ----------------------------------------------------------------------------
void CMsvPop3FetchOperation::RunL()
    {
    IMUM_CONTEXT( CMsvPop3FetchOperation::RunL, 0, KImumMtmLog );
    IMUM_IN();
    
    TRAP(iError, DoRunL()); // CSI: 86 # Needed here to properly handle error situations
    if(iError != KErrNone)
        {
        // Need to ensure we disconnect if we have connected.
        if( (iError == KErrNoMemory) || (iState == EStateDisconnecting) )
            {
            // OOM and/or failed to create disconnect operation. Call StopService()
            iMsvSession.StopService(iService);
            // Ignore return, nothing we can do.
            }
        else if( (iState == EStateFetching) && (iDisconnect) )
            {
            // Failed to start fetch operation. Need to disconnect.
            __ASSERT_DEBUG(!iFetchErrorProgress,
                User::Panic(KImumMtmUiPanic,EPop3MtmUiOpAlreadyHaveProgErr));
            TPckgBuf<TPop3Progress>* progPkg = new TPckgBuf<TPop3Progress>; // CSI: 62 # We want to avoid leaves.
            iFetchErrorProgress = progPkg;
            if(iFetchErrorProgress)
                {
                TPop3Progress& errProg = (*progPkg)();
                errProg.iPop3Progress = TPop3Progress::EPopPopulating;
                errProg.iTotalMsgs = 0;
                errProg.iMsgsToProcess = 0;
                errProg.iBytesDone = 0;
                errProg.iTotalBytes = 0;
                errProg.iErrorCode = iError;
                errProg.iPop3SubStateProgress = TPop3Progress::EPopPopulating;
                errProg.iServiceId = iService;
                }
            SetActive();
            CompleteThis();     // Next DoRunL() will attempt to disconnect.
            IMUM_OUT();
            return;
            }
        // Notify observer we have finished.
        CompleteObserver();
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// DoCancel
// ----------------------------------------------------------------------------
void CMsvPop3FetchOperation::DoCancel()
    {
    IMUM_CONTEXT( CMsvPop3FetchOperation::DoCancel, 0, KImumMtmLog );
    IMUM_IN();
    
    CImumOnlineOperation::DoCancel();
    if(iState == EStateFetching)
        {
        // Cancelled while fetching. Need to disconnect.
        iMsvSession.StopService(iService);
        // Ignore return, nothing we can do.
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// DoRunL
// ----------------------------------------------------------------------------
void CMsvPop3FetchOperation::DoRunL()
    {
    IMUM_CONTEXT( CMsvPop3FetchOperation::DoRunL, 0, KImumMtmLog );
    IMUM_IN();
    
    switch(iState)
        {
        case EStateConnecting:
            {
            // Connect complete.
            TBool connected = STATIC_CAST(CMsvPop3ConnectOp*,iOperation)->Connected();
            if(!connected)
                {
                // Connect failed.
                CompleteObserver( KErrCouldNotConnect );
                IMUM_OUT();
                return;
                }
            DoFetchL();
            }
            break;
        case EStateFetching:
            // Fetch complete.
            {
            TInt err = GetOperationCompletionCodeL();
            if(err != KErrNone)
                {
                __ASSERT_DEBUG(!iFetchErrorProgress,
                    User::Panic(KImumMtmUiPanic,EPop3MtmUiOpAlreadyHaveProgErr));
                TPckgBuf<TPop3Progress> paramPack;
                paramPack.Copy( iOperation->ProgressL() );
                TPop3Progress& progress = paramPack();
                progress.iErrorCode = err;
                iFetchErrorProgress = paramPack.AllocL();
                }
            DoDisconnectL();
            }
            break;
        case EStateDisconnecting:
            // Disconnect complete.
            iState = EStateIdle;
            CompleteObserver();
            break;
        default:
            User::Panic(KImumMtmUiPanic,EPop3MtmUiOpUnknownState);
            break;
        }
    if(iOperation)
        iMtm = iOperation->Mtm();
    else
        iMtm = KUidMsgTypePOP3;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// GetErrorProgressL
// ----------------------------------------------------------------------------
const TDesC8& CMsvPop3FetchOperation::GetErrorProgressL(TInt aError)
// Called when DoRunL() leaves.
    {
    IMUM_CONTEXT( CMsvPop3FetchOperation::GetErrorProgressL, 0, KImumMtmLog );
    IMUM_IN();
    
    iError = aError;
    if(iFetchErrorProgress)
        {
        delete iFetchErrorProgress;
        iFetchErrorProgress = NULL;
        }
    TPckgBuf<TPop3Progress> paramPack;
    TPop3Progress& progress = paramPack();
    progress.iPop3Progress = TPop3Progress::EPopConnecting;
    progress.iTotalMsgs = 0;
    progress.iMsgsToProcess = 0;
    progress.iBytesDone = 0;
    progress.iTotalBytes = 0;
    progress.iErrorCode = iError;
    switch(iState)
        {
        case EStateConnecting:  // Should never get this, so treat as next state.
        case EStateFetching:
            if(iOperation)
                {
                paramPack.Copy( iOperation->ProgressL() );
                }
            else
                {
                progress.iPop3Progress = TPop3Progress::EPopPopulating;
                }
            break;
        case EStateDisconnecting:
            progress.iPop3Progress = TPop3Progress::EPopDisconnecting;
            break;
        default:
            break;
        }
    iFetchErrorProgress = paramPack.AllocL();
    IMUM_OUT();
    return *iFetchErrorProgress;
    }

// End of File
