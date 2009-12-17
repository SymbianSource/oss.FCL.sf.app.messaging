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
*       Operation to copy or move messages
*
*/

#include <eikenv.h>
#include <imum.rsg>
#include <MsvPrgReporter.h>
#include <ImumInMailboxUtilities.h>

#include "MsvConnectionValidation.h"
#include "MsvEmailConnectionProgressProvider.h"
#include "EmailUtils.H"
#include "ImumDisconnectOperation.h"
#include "MsvPop3CopyMoveOperation.h"
#include "MsvPop3FetchOperation.h"
#include "IMSSettingsNoteUi.h"

// Constants and defines
const TInt KCopyOrMoveOpPriority = CActive::EPriorityStandard;


// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
CMsvPop3CopyMoveOperation* CMsvPop3CopyMoveOperation::NewL(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aService, TMsvId aTarget,
    const CMsvEntrySelection& aSel,
    TBool aMoveMsgs)
    {
    IMUM_STATIC_CONTEXT( CMsvPop3CopyMoveOperation::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
     // static
    CMsvPop3CopyMoveOperation* op = new(ELeave) CMsvPop3CopyMoveOperation(
        aMailboxApi,
        aObserverRequestStatus,
        aReporter,
        aService,
        aTarget,
        aMoveMsgs);
    CleanupStack::PushL(op);
    op->ConstructL(aSel);
    CleanupStack::Pop();
    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
// ~CMsvPop3CopyMoveOperation
// ----------------------------------------------------------------------------
CMsvPop3CopyMoveOperation::~CMsvPop3CopyMoveOperation()
    {
    IMUM_CONTEXT( CMsvPop3CopyMoveOperation::~CMsvPop3CopyMoveOperation, 0, KImumMtmLog );
    IMUM_IN();
    
    delete iLocalSel;
    delete iRemoteSel;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// DoRunL
// ----------------------------------------------------------------------------
void CMsvPop3CopyMoveOperation::DoRunL()
    {
    IMUM_CONTEXT( CMsvPop3CopyMoveOperation::DoRunL, 0, KImumMtmLog );
    IMUM_IN();
    
    switch(iState)
        {
        case ELocalMsgs:
            if(GetOperationCompletionCodeL() != KErrNone)
                {
                // Failed to copy local messages.
                iState = EIdle;
                CompleteObserver();
                }
            else
                {
                DoRemoteFetchL();
                }
            break;
        case ERemoteMsgs:
            iState = EIdle;
            CompleteObserver();
            break;
        default:
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
const TDesC8& CMsvPop3CopyMoveOperation::GetErrorProgressL(TInt aError)
    {
    IMUM_CONTEXT( CMsvPop3CopyMoveOperation::GetErrorProgressL, 0, KImumMtmLog );
    IMUM_IN();
    
    iError = aError;
    switch(iState)
        {
        case ERemoteMsgs:
            if(iOperation)
                {
                // iOperation is a CMsvPop3FetchOperation.
                return STATIC_CAST(CImumOnlineOperation*,iOperation)->
                    GetErrorProgressL(aError);
                }
            break;
        case ELocalMsgs:
        default:
            break;
        }
    TPop3Progress& progress = iErrorProgress();
    progress.iTotalMsgs = 0;
    progress.iMsgsToProcess = 0;
    progress.iBytesDone = 0;
    progress.iTotalBytes = 0;
    progress.iErrorCode = iError;
    progress.iPop3Progress = (iMoving) ?
        (TPop3Progress::EPopMoving) : (TPop3Progress::EPopCopying);
    progress.iPop3SubStateProgress = progress.iPop3Progress;
    IMUM_OUT();
    return iErrorProgress;
    }

// ----------------------------------------------------------------------------
// SortMessageSelectionL
// ----------------------------------------------------------------------------
void CMsvPop3CopyMoveOperation::SortMessageSelectionL(
    const CMsvEntrySelection& aSel)
    {
    IMUM_CONTEXT( CMsvPop3CopyMoveOperation::SortMessageSelectionL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Sort messages into complete and incomplete selections.
    delete iLocalSel;
    iLocalSel = NULL;
    iLocalSel = new(ELeave) CMsvEntrySelection;
    delete iRemoteSel;
    iRemoteSel = NULL;
    iRemoteSel = new(ELeave) CMsvEntrySelection;

    TInt err;
    TMsvId id;
    TMsvId service;
    TMsvEntry tentry;
    for(TInt count=0; count<aSel.Count(); count++)
        {
        id = aSel[count];
        err = iMsvSession.GetEntry(id, service, tentry);
        if(KErrNone == err)
            {
            if(tentry.Complete())
                {
                iLocalSel->AppendL(id);
                }
            else
                {
                iRemoteSel->AppendL(id);
                }
            }
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// DoLocalCopyMoveL
// ----------------------------------------------------------------------------
void CMsvPop3CopyMoveOperation::DoLocalCopyMoveL()
    {
    IMUM_CONTEXT( CMsvPop3CopyMoveOperation::DoLocalCopyMoveL, 0, KImumMtmLog );
    IMUM_IN();
    
    if(iLocalSel->Count())
        {
        CMsvEntry* centry = iMsvSession.GetEntryL(iService);
        CleanupStack::PushL(centry);
        iStatus = KRequestPending;
        delete iOperation;
        iOperation = NULL;
        iOperation = MsvEmailMtmUiUtils::DoCEntryCopyMoveL(
            *centry,
            *iLocalSel,
            iTarget,
            iStatus,
            !iMoving);
        CleanupStack::PopAndDestroy(); // centry
        }
    else
        {
        CompleteThis();
        }
    iState = ELocalMsgs;
    SetActive();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// DoRemoteFetchL
// ----------------------------------------------------------------------------
void CMsvPop3CopyMoveOperation::DoRemoteFetchL()
    {
    IMUM_CONTEXT( CMsvPop3CopyMoveOperation::DoRemoteFetchL, 0, KImumMtmLog );
    IMUM_IN();
    
    TInt msgCount = iRemoteSel->Count();
    if(msgCount)
        {
        CMsvEntry* centry = iMsvSession.GetEntryL(iService);
        TBool doIt = centry->Entry().Connected();
        delete centry;
        if(!doIt)
            {
            // Prompt user before connecting.
            iReporter.MakeProgressVisibleL(EFalse);
            TInt queryResouce =  msgCount > 1 ?
                R_EMAIL_FETCH_MESSAGES_BEFORE_COPY_QUERY :
                R_EMAIL_FETCH_MESSAGE_BEFORE_COPY_QUERY;
            doIt = CIMSSettingsNoteUi::ShowQueryL(
                queryResouce, R_EMAIL_CONFIRMATION_QUERY );
            }
        if(doIt)
            {
            iReporter.MakeProgressVisibleL(ETrue);
            TImPop3GetMailInfo info;
            info.iMaxEmailSize = KMaxTInt32;
            info.iDestinationFolder = iTarget;
            TInt fnId = (iMoving) ? (KPOP3MTMMoveMailSelectionWhenAlreadyConnected) : (KPOP3MTMCopyMailSelectionWhenAlreadyConnected);
            iStatus = KRequestPending;
            delete iOperation;
            iOperation = NULL;
            iOperation = CMsvPop3FetchOperation::NewL(
                iMailboxApi,
                iStatus,
                iReporter,
                fnId,
                iService,
                info,
                *iRemoteSel);
            }
        }
    if(iStatus != KRequestPending)
        {
        CompleteThis();
        }
    iState = ERemoteMsgs;
    SetActive();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CMsvPop3CopyMoveOperation
// ----------------------------------------------------------------------------
CMsvPop3CopyMoveOperation::CMsvPop3CopyMoveOperation(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aService,
    TMsvId aTarget,
    TBool aMoveMsgs)
    :
    CImumDiskSpaceObserverOperation(
        aMailboxApi,
        KCopyOrMoveOpPriority,
        aObserverRequestStatus,
        aReporter),
    iMoving(aMoveMsgs),
    iTarget(aTarget)
    {
    IMUM_CONTEXT( CMsvPop3CopyMoveOperation::CMsvPop3CopyMoveOperation, 0, KImumMtmLog );
    IMUM_IN();
    
    iService = aService;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
void CMsvPop3CopyMoveOperation::ConstructL(const CMsvEntrySelection& aSel)
    {
    IMUM_CONTEXT( CMsvPop3CopyMoveOperation::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    SortMessageSelectionL(aSel);
    BaseConstructL(KUidMsgTypePOP3);
    DoLocalCopyMoveL();
    IMUM_OUT();
    }

