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
*       Fetch operation with checking of available disk space
*
*/


#include <eikenv.h>
#include <eikrutil.h>

#include "Imap4SizeAndFetchOp.h"
#include "Imap4FetchOp.h"
#include "EmailMsgSizerOperation.h"
#include "EmailUtils.H"
#include "ImumDisconnectOperation.h"
#include "IMAPPRGR.H"
#include "ImumMtmLogging.h"
#include "ImumPanic.h"

// Constants
const TInt KSizeAndFetchPriority = CActive::EPriorityStandard;

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
CImap4SizeAndFetchOp* CImap4SizeAndFetchOp::NewL(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aService,
    TInt aFunctionId,
    const TImImap4GetMailInfo& aGetMailInfo,
    const CMsvEntrySelection& aSel)
    {
    IMUM_STATIC_CONTEXT( CImap4SizeAndFetchOp::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CImap4SizeAndFetchOp* op = new(ELeave) CImap4SizeAndFetchOp(
        aMailboxApi, aStatus, aReporter, aService, aFunctionId, aGetMailInfo);
    CleanupStack::PushL(op);
    op->ConstructL(aSel);
    CleanupStack::Pop();
    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
// ~CImap4SizeAndFetchOp
// ----------------------------------------------------------------------------
CImap4SizeAndFetchOp::~CImap4SizeAndFetchOp()
    {
    IMUM_CONTEXT( CImap4SizeAndFetchOp::~CImap4SizeAndFetchOp, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    delete iSelection;
    }

// ----------------------------------------------------------------------------
// GetErrorProgressL
// ----------------------------------------------------------------------------
const TDesC8& CImap4SizeAndFetchOp::GetErrorProgressL(TInt aError)
    {
    IMUM_CONTEXT( CImap4SizeAndFetchOp::GetErrorProgressL, 0, KImumMtmLog );
    IMUM_IN();
    
    TImap4CompoundProgress& progress = iProgressBuf();
    progress.iGenericProgress.iOperation = TImap4GenericProgress::EPopulate;
    progress.iGenericProgress.iState = TImap4GenericProgress::EFetching;
    progress.iGenericProgress.iErrorCode = aError;
    IMUM_OUT();
    return iProgressBuf;
    }

// ----------------------------------------------------------------------------
// CImap4SizeAndFetchOp
// ----------------------------------------------------------------------------
CImap4SizeAndFetchOp::CImap4SizeAndFetchOp(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aService,
    TInt aFunctionId,
    const TImImap4GetMailInfo& aGetMailInfo )
    :
    CImumOnlineOperation(aMailboxApi, KSizeAndFetchPriority, aStatus, aReporter),
    iState(EStateIdle),
    iFunctionId(aFunctionId),
    iGetMailInfo(aGetMailInfo)
    {
    IMUM_CONTEXT( CImap4SizeAndFetchOp::CImap4SizeAndFetchOp, 0, KImumMtmLog );
    IMUM_IN();
    
    iService = aService;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
void CImap4SizeAndFetchOp::ConstructL(const CMsvEntrySelection& aSel)
    {
    IMUM_CONTEXT( CImap4SizeAndFetchOp::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    __ASSERT_DEBUG(aSel.Count(),
        User::Panic(KImumMtmUiPanic,EPanicSelectionEmpty));
    if ( aSel.Count() == 0 )
        {
        User::Leave( KErrNotSupported );
        }
    iSelection = aSel.CopyL();
#ifdef _DEBUG
    const TInt count = aSel.Count();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        }
#endif

    BaseConstructL(KUidMsgTypeIMAP4);
    DoSizeMessageL();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// DoRunL
// ----------------------------------------------------------------------------
void CImap4SizeAndFetchOp::DoRunL()
    {
    IMUM_CONTEXT( CImap4SizeAndFetchOp::DoRunL, 0, KImumMtmLog );
    IMUM_IN();
    
    switch(iState)
        {
        case EStateSizeMsg:
            // Finished determining message size.
            if(!CheckMessageSizeL())
                {
                // Not enough storage, user already informed.
                CompleteObserver();
                iState = EStateIdle;
                }
            else
                {
                DoSizeNextMessageL();
                }
            break;
        case EStateFetchMsg:
            // Finished fetching message.
            CompleteObserver();
            iState = EStateIdle;
            break;
        case EStateIdle:
        default:
            break;
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// DoSizeMessageL
// ----------------------------------------------------------------------------
void CImap4SizeAndFetchOp::DoSizeMessageL()
    {
    IMUM_CONTEXT( CImap4SizeAndFetchOp::DoSizeMessageL, 0, KImumMtmLog );
    IMUM_IN();
    
    iState = EStateSizeMsg;
    iStatus = KRequestPending;
    delete iOperation;
    iOperation = NULL;
    iOperation = CEmailMsgSizerOperation::NewL(
        iMailboxApi, iStatus, (*iSelection)[ iCurrentMsgToSize ], iMtm);
    SetActive();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// DoSizeNextMessageL
// ----------------------------------------------------------------------------
void CImap4SizeAndFetchOp::DoSizeNextMessageL()
    {
    IMUM_CONTEXT( CImap4SizeAndFetchOp::DoSizeNextMessageL, 0, KImumMtmLog );
    IMUM_IN();
    
    iCurrentMsgToSize++;
    if ( iCurrentMsgToSize < iSelection->Count() )
        {
        DoSizeMessageL();
        }
    else
        {
        DoFetchMessageL();
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// DoFetchMessageL
// ----------------------------------------------------------------------------
void CImap4SizeAndFetchOp::DoFetchMessageL()
    {
    IMUM_CONTEXT( CImap4SizeAndFetchOp::DoFetchMessageL, 0, KImumMtmLog );
    IMUM_IN();
    
    iState = EStateFetchMsg;
    iStatus = KRequestPending;
    delete iOperation;
    iOperation = NULL;
    iOperation = CImap4FetchOp::NewL(
        iMailboxApi, iStatus, iReporter, iService,
        iFunctionId, iGetMailInfo, *iSelection);
    SetActive();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CheckMessageSizeL
// ----------------------------------------------------------------------------
TBool CImap4SizeAndFetchOp::CheckMessageSizeL()
    {
    IMUM_CONTEXT( CImap4SizeAndFetchOp::CheckMessageSizeL, 0, KImumMtmLog );
    IMUM_IN();
    
    CEmailMsgSizerOperation& sizer = *STATIC_CAST(CEmailMsgSizerOperation*, iOperation);
    TInt32 msgSize = 0;

    switch(iGetMailInfo.iGetMailBodyParts)
        {
        case EGetImap4EmailBodyText:
            msgSize = sizer.BodyTextSizeL();
            break;
        case EGetImap4EmailBodyAlternativeText:
        case EGetImap4EmailBodyTextAndAttachments:
            msgSize = sizer.TotalMessageSize();
            break;
        case EGetImap4EmailAttachments:
            msgSize = sizer.SizeOfAllAttachmentsL();
            break;
        case EGetImap4EmailHeaders:
        default:
            break;
        }
    iTotalSizeToFetch += msgSize;
    
    IMUM_OUT();
    
    return ETrue;
    }

// End of File
