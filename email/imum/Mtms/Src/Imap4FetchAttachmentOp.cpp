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
*       Fetch attachments
*
*/


#include <eikenv.h>
#include <eikrutil.h>

#include "Imap4FetchAttachmentOp.h"
#include "EmailMsgSizerOperation.h"
#include "EmailUtils.H"
#include "ImumDisconnectOperation.h"
#include <imapcmds.h>
#include "IMAPPRGR.H"
#include "ImumMtmLogging.h"
#include "ImumPanic.h"


CImap4FetchAttachmentOp* CImap4FetchAttachmentOp::NewL(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aService,
    const CMsvEntrySelection& aSel)
    {
    IMUM_STATIC_CONTEXT( CImap4FetchAttachmentOp::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CImap4FetchAttachmentOp* op = new(ELeave) CImap4FetchAttachmentOp(
        aMailboxApi, aStatus, aReporter, aService);
    CleanupStack::PushL(op);
    op->ConstructL(aSel);
    CleanupStack::Pop();
    IMUM_OUT();
    return op;
    }

CImap4FetchAttachmentOp::~CImap4FetchAttachmentOp()
    {
    IMUM_CONTEXT( CImap4FetchAttachmentOp::~CImap4FetchAttachmentOp, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    delete iSelection;
    }

void CImap4FetchAttachmentOp::DoConnectedOpL()
    {
    IMUM_CONTEXT( CImap4FetchAttachmentOp::DoConnectedOpL, 0, KImumMtmLog );
    IMUM_IN();
    
    delete iOperation;
    iOperation = NULL;
    // iOperation is set in base class function InvokeClientMtmAsyncFunctionL
    TPckgBuf<TImImap4GetMailInfo> optionsBuf;
    TImImap4GetMailInfo& options = optionsBuf();
    options.iMaxEmailSize = KMaxTInt32;
    options.iGetMailBodyParts = EGetImap4EmailAttachments;
    options.iDestinationFolder = 0; // not used with fetch attachment operation
    InvokeClientMtmAsyncFunctionL(
        KIMAP4MTMPopulate, *iSelection, iService, optionsBuf);
    SetActive();
    IMUM_OUT();
    }

const TDesC8& CImap4FetchAttachmentOp::ConnectedOpErrorProgressL(TInt aError)
    {
    IMUM_CONTEXT( CImap4FetchAttachmentOp::ConnectedOpErrorProgressL, 0, KImumMtmLog );
    IMUM_IN();
    
    __ASSERT_DEBUG(iState==EStateDoingOp || iState==EStateIdle,
        User::Panic(KImumMtmUiPanic,EPanicWrongStateForOpErrorProgress));
    iUiProgress().iError=aError;
    IMUM_OUT();
    return iUiProgress;
    }

const TDesC8& CImap4FetchAttachmentOp::ProgressL()
    {
    IMUM_CONTEXT( CImap4FetchAttachmentOp::ProgressL, 0, KImumMtmLog );
    IMUM_IN();
    
    if(iState!=EStateDoingOp)
        {
        IMUM_OUT();
        return CImap4ConnectedOp::ProgressL();
        }
    else
        {
        TPckgBuf<TImap4CompoundProgress> fetchProgressBuf;
        fetchProgressBuf.Copy(iOperation->ProgressL());
        const TImap4CompoundProgress& opProgress=fetchProgressBuf();
        TImap4UiProgress& uiProgress=iUiProgress();
        uiProgress.iBytesToDo=opProgress.iGenericProgress.iBytesToDo;
        uiProgress.iBytesDone=opProgress.iGenericProgress.iBytesDone;
        uiProgress.iError=fetchProgressBuf().iGenericProgress.iErrorCode;
        IMUM_OUT();
        return iUiProgress;
        }
    }

CImap4FetchAttachmentOp::CImap4FetchAttachmentOp(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aService)
    :
    CImap4ConnectedOp(aMailboxApi, aStatus, aReporter, aService)
    {
    IMUM_CONTEXT( CImap4FetchAttachmentOp::CImap4FetchAttachmentOp, 0, KImumMtmLog );
    IMUM_IN();
    
    iConnectionCompletionState = CImapConnectionOp::ECompleteAfterConnect;
    // Don't do a sync.
    TImap4UiProgress uiProgress;
    uiProgress.iUiOperation=TImap4UiProgress::EFetchingAttachment;
    //  Used during fetch stage to differentiate from
    iUiProgress()=uiProgress;
    //  standard message fetch.
    IMUM_OUT();
    }

void CImap4FetchAttachmentOp::ConstructL(const CMsvEntrySelection& aSel)
    {
    IMUM_CONTEXT( CImap4FetchAttachmentOp::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    __ASSERT_DEBUG(aSel.Count(),
        User::Panic(KImumMtmUiPanic,EPanicSelectionEmpty));
    if ( aSel.Count() == 0 )
        {
        User::Leave( KErrNotSupported );
        }
    iSelection = aSel.CopyL();
    BaseConstructL(KUidMsgTypeIMAP4);

    // Get message size.
    CMsvEntry* centry = iMsvSession.GetEntryL(aSel[0]);
    TInt32 msgSize = centry->Entry().iSize;
    delete centry;

    // Check message size.
    if ( MsvEmailMtmUiUtils::CheckAvailableDiskSpaceForDownloadL(
        msgSize, *iEikEnv, iMsvSession ) )
        {
        StartL();
        }
    else
        {
        // Not enough disk space or memory.
        // User already notified, so 'this' op completes without error.           
        CompleteThis();
        iState = EStateDisconnecting;
        
        SetActive();
        }
    IMUM_OUT();
    }


