/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*		E-mail attachment remove operation
*
*/


// INCLUDE FILES
#include <miutset.h> // KUidMsgTypeSMTP
#include <miutmsg.h> // CImEmailMessage

#include "MsgMailEditorRemoveAttachment.h"


// ================= MEMBER FUNCTIONS =======================

// Constructor
CMsgMailEditorRemoveAttachmentOp::CMsgMailEditorRemoveAttachmentOp(CMsvSession& aMsvSession, 
    TRequestStatus& aObserverRequestStatus)
    : CMsvOperation(aMsvSession, EPriorityStandard, aObserverRequestStatus)
    {
    CActiveScheduler::Add(this);
    iMtm = KUidMsgTypeSMTP;
    }

// Symbian OS constructor
CMsgMailEditorRemoveAttachmentOp* 
CMsgMailEditorRemoveAttachmentOp::NewLC(CMsvSession& aMsvSession, 
    TRequestStatus& aObserverRequestStatus, const TMsvId aMsgId, 
    const TMsvId aAttachmentId)
    {
    CMsgMailEditorRemoveAttachmentOp *op = new(ELeave) 
        CMsgMailEditorRemoveAttachmentOp(aMsvSession, aObserverRequestStatus);
    CleanupStack::PushL(op);
    op->ConstructL(aMsgId, aAttachmentId);
    return op;
    }

// Destructor
CMsgMailEditorRemoveAttachmentOp::~CMsgMailEditorRemoveAttachmentOp()
    {
    Cancel();
    delete iMsgEntry;
    delete iEmailMsg;
    }

// 2nd phase constructor
void CMsgMailEditorRemoveAttachmentOp::ConstructL(const TMsvId aMsgId, 
                                                  const TMsvId aAttachmentId)
    {
    iMsgEntry = iMsvSession.GetEntryL(aMsgId);
    iEmailMsg = CImEmailMessage::NewL(*iMsgEntry);
    iStatus = KRequestPending;
    iEmailMsg->DeleteAttachmentL(aMsgId, aAttachmentId, iStatus);
    SetActive();
    }

// Returns progress of the operation using TMsvLocalOperationProgress
const TDesC8& CMsgMailEditorRemoveAttachmentOp::ProgressL()
    {
    return iProgress;
    }

// Cancels outstanding operation
void CMsgMailEditorRemoveAttachmentOp::DoCancel()
    {
    if(iEmailMsg)
        {
        iEmailMsg->Cancel();
        iProgress().iError = KErrCancel;
        }
    }

// Handles an active object’s request completion event
void CMsgMailEditorRemoveAttachmentOp::RunL()
    {
    iProgress().iError = iStatus.Int();
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete(status, KErrNone);
    }



// End of File  
