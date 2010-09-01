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
*       Copy or move a selection of messages that may or may not be complete
*
*/


#include <eikenv.h>
#include <eikrutil.h>

#include "Imap4CopyMoveLocalOp.h"
#include "Imap4FetchOp.h"
#include "EmailMsgSizerOperation.h"
#include "EmailUtils.H"
#include "ImumDisconnectOperation.h"
#include <imum.rsg>
#include <imapcmds.h>
#include "IMAPPRGR.H"
#include "ImumMtmLogging.h"
#include "ImumPanic.h"
#include "IMSSettingsNoteUi.h"

const TInt KCopyMoveLocalOpPriority = CActive::EPriorityStandard;


CImap4CopyMoveLocalOp* CImap4CopyMoveLocalOp::NewL(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aStatus,
    MMsvProgressReporter& aReporter,
    const CMsvEntrySelection& aSel,
    TMsvId aTargetId,
    TBool aCopy)
    {
    IMUM_STATIC_CONTEXT( CImap4CopyMoveLocalOp::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CImap4CopyMoveLocalOp* op = new(ELeave) CImap4CopyMoveLocalOp(
        aMailboxApi, aStatus, aReporter, aTargetId, aCopy);
    CleanupStack::PushL(op);
    op->ConstructL(aSel);
    CleanupStack::Pop();
    IMUM_OUT();
    return op;
    }

CImap4CopyMoveLocalOp::~CImap4CopyMoveLocalOp()
    {
    IMUM_CONTEXT( CImap4CopyMoveLocalOp::~CImap4CopyMoveLocalOp, 0, KImumMtmLog );
    IMUM_IN();
    
    delete iLocalSel;
    delete iRemoteSel;
    IMUM_OUT();
    }

const TDesC8& CImap4CopyMoveLocalOp::GetErrorProgressL(TInt aError)
    {
    IMUM_CONTEXT( CImap4CopyMoveLocalOp::GetErrorProgressL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Called to report a leave in DoRunL()
    if(!iProgressBuf().iGenericProgress.iErrorCode)
        {
        TImap4CompoundProgress& prog = iProgressBuf();
        if(iCopying)
            {
            prog.iGenericProgress.iOperation = TImap4GenericProgress::ECopyToLocal;
            prog.iGenericProgress.iState = TImap4GenericProgress::ECopying;
            }
        else
            {
            prog.iGenericProgress.iOperation = TImap4GenericProgress::EMoveToLocal;
            prog.iGenericProgress.iState = TImap4GenericProgress::EMoving;
            }

        prog.iGenericProgress.iErrorCode = aError;
        }
    IMUM_OUT();
    return iProgressBuf;
    }

void CImap4CopyMoveLocalOp::DoRunL()
    {
    IMUM_CONTEXT( CImap4CopyMoveLocalOp::DoRunL, 0, KImumMtmLog );
    IMUM_IN();
    
    TInt returnValue( KErrNone );
    switch(iState)
        {
        case ELocalMsgs:
            // Local copy complete
            returnValue = GetOperationCompletionCodeL();
            if( returnValue != KErrNone )
                {
                // Failed to copy local messages.
                iState = EIdle;
                CompleteObserver( returnValue );
                }
            else
                {
                DoRemoteFetchL();
                }
            break;
        case ERemoteMsgs:
            // Remote fetch complete.
            iState = EIdle;
            CompleteObserver();
            break;
        case EIdle:
        default:
            break;
        }
    IMUM_OUT();
    }

CImap4CopyMoveLocalOp::CImap4CopyMoveLocalOp(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aTargetId,
    TBool aCopy)
    :
    CImumDiskSpaceObserverOperation(aMailboxApi, KCopyMoveLocalOpPriority, aStatus, aReporter),
    iCopying(aCopy),
    iTarget(aTargetId)
    {
    IMUM_CONTEXT( CImap4CopyMoveLocalOp::CImap4CopyMoveLocalOp, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    }

void CImap4CopyMoveLocalOp::ConstructL(const CMsvEntrySelection& aSel)
    {
    IMUM_CONTEXT( CImap4CopyMoveLocalOp::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    __ASSERT_DEBUG(aSel.Count(),
        User::Panic(KImumMtmUiPanic,EPanicSelectionEmpty));
    if ( aSel.Count() == 0 )
        {
        User::Leave( KErrNotSupported );
        }
    BaseConstructL(KUidMsgTypeIMAP4);
    CMsvEntry* centry = iMsvSession.GetEntryL(aSel[0]);
    iService = centry->OwningService();
    delete centry;
    SortMessageSelectionL(aSel);
    DoLocalCopyMoveL();
    IMUM_OUT();
    }

void CImap4CopyMoveLocalOp::SortMessageSelectionL(const CMsvEntrySelection& aSel)
    {
    IMUM_CONTEXT( CImap4CopyMoveLocalOp::SortMessageSelectionL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Sort messages into complete and incomplete selections.
    __ASSERT_DEBUG(!iLocalSel,
        User::Panic(KImumMtmUiPanic,EPanicSelAlreadyCreated));
    iLocalSel = new(ELeave) CMsvEntrySelection;
    __ASSERT_DEBUG(!iRemoteSel,
        User::Panic(KImumMtmUiPanic,EPanicSelAlreadyCreated));
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

void CImap4CopyMoveLocalOp::DoLocalCopyMoveL()
    {
    IMUM_CONTEXT( CImap4CopyMoveLocalOp::DoLocalCopyMoveL, 0, KImumMtmLog );
    IMUM_IN();
    
    iState = ELocalMsgs;
    iStatus = KRequestPending;
    if(iLocalSel->Count())
        {
        CMsvEntry* centry = iMsvSession.GetEntryL((*iLocalSel)[0]);
        CleanupStack::PushL(centry);
        TMsvId parent = centry->Entry().Parent();
        centry->SetEntryL(parent);
        __ASSERT_DEBUG(!iOperation,
            User::Panic(KImumMtmUiPanic,EPanicOpAlreadySet));
        delete iOperation;
        iOperation = NULL;
        iOperation = MsvEmailMtmUiUtils::DoCEntryCopyMoveL(
            *centry, *iLocalSel, iTarget, iStatus, iCopying);
        CleanupStack::PopAndDestroy(); // centry
        }
    else
        {
        CompleteThis();
        }
    SetActive();
    IMUM_OUT();
    }

void CImap4CopyMoveLocalOp::DoRemoteFetchL()
    {
    IMUM_CONTEXT( CImap4CopyMoveLocalOp::DoRemoteFetchL, 0, KImumMtmLog );
    IMUM_IN();
    
    TInt numMsgs = iRemoteSel->Count();
    iState = ERemoteMsgs;
    if(numMsgs)
        {
        CMsvEntry* centry = iMsvSession.GetEntryL(iService);
        TBool doIt = centry->Entry().Connected();
        delete centry;
        if(!doIt)
            {
            numMsgs += iLocalSel->Count();
            iReporter.MakeProgressVisibleL(EFalse);

            // Prompt user before connecting.
            TInt queryResouce =  numMsgs > 1 ?
                R_EMAIL_FETCH_MESSAGES_BEFORE_COPY_QUERY :
                R_EMAIL_FETCH_MESSAGE_BEFORE_COPY_QUERY;
            doIt = CIMSSettingsNoteUi::ShowQueryL(
                queryResouce, R_EMAIL_CONFIRMATION_QUERY );
            }
        if(doIt)
            {
            iReporter.MakeProgressVisibleL(ETrue);
            iStatus = KRequestPending;
            delete iOperation;
            iOperation = NULL;
            TImImap4GetMailInfo getMailInfo;
            getMailInfo.iMaxEmailSize = KMaxTInt32;
            getMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
            getMailInfo.iDestinationFolder = iTarget;

            iOperation = CImap4FetchOp::NewL(
                iMailboxApi,
                iStatus,
                iReporter,
                iService,
                (iCopying?KIMAP4MTMCopyMailSelectionWhenAlreadyConnected:KIMAP4MTMMoveMailSelectionWhenAlreadyConnected),
                getMailInfo,
                *iRemoteSel);
            }
        }
    if(iStatus != KRequestPending)
        {
        CompleteThis();
        }
    SetActive();
    IMUM_OUT();
    }

