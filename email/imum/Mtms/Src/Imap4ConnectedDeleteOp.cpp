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
*       Folder delete
*
*/


#include <eikenv.h>
#include <eikrutil.h>

#include "Imap4ConnectedDeleteOp.h"
#include "EmailMsgSizerOperation.h"
#include "EmailUtils.H"
#include "ImumDisconnectOperation.h"
#include "IMAPPRGR.H"
#include "ImumMtmLogging.h"

CImap4ConnectedDeleteOp* CImap4ConnectedDeleteOp::NewL(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aServiceId,
    const CMsvEntrySelection& aSel)
    {
    IMUM_STATIC_CONTEXT( CImap4ConnectedDeleteOp::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CImap4ConnectedDeleteOp* op = new(ELeave) CImap4ConnectedDeleteOp(
        aMailboxApi, aStatus, aReporter, aServiceId);
    CleanupStack::PushL(op);
    op->iSelection = aSel.CopyL();
    op->ConstructL();
    CleanupStack::Pop();
    IMUM_OUT();
    return op;
    }

CImap4ConnectedDeleteOp::~CImap4ConnectedDeleteOp()
    {
    IMUM_CONTEXT( CImap4ConnectedDeleteOp::~CImap4ConnectedDeleteOp, 0, KImumMtmLog );
    IMUM_IN();
    
    delete iSelection;
    IMUM_OUT();
    }

void CImap4ConnectedDeleteOp::DoConnectedOpL()
    {
    IMUM_CONTEXT( CImap4ConnectedDeleteOp::DoConnectedOpL, 0, KImumMtmLog );
    IMUM_IN();
    
    iStatus = KRequestPending;
    if(iSelection->Count())
        {
        CMsvEntry* centry = iMsvSession.GetEntryL((*iSelection)[0]);
        CleanupStack::PushL(centry);
        centry->SetEntryL(centry->Entry().Parent());
        delete iOperation;
        iOperation = NULL;
        iOperation = centry->DeleteL(*iSelection, iStatus);
        CleanupStack::PopAndDestroy();
        }
    else
        {
        CompleteThis();
        }
    SetActive();
    IMUM_OUT();
    }

const TDesC8& CImap4ConnectedDeleteOp::ConnectedOpErrorProgressL(TInt aError)
    {
    IMUM_CONTEXT( CImap4ConnectedDeleteOp::ConnectedOpErrorProgressL, 0, KImumMtmLog );
    IMUM_IN();
    
    TImap4UiProgress& prog = iUiProgressBuf();
    prog.iType = KImap4UiProgressType;
    prog.iUiOperation = TImap4UiProgress::EDeletingFolder;
    prog.iError = aError;
    IMUM_OUT();
    return iUiProgressBuf;
    }

CImap4ConnectedDeleteOp::CImap4ConnectedDeleteOp(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aServiceId)
    :
    CImap4ConnectedOp(aMailboxApi, aStatus, aReporter, aServiceId)
    {
    IMUM_CONTEXT( CImap4ConnectedDeleteOp::CImap4ConnectedDeleteOp, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    }


