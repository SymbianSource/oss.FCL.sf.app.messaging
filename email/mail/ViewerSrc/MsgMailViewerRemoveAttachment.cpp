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
* Description:  E-mail attachment remove operation.
*
*/


// INCLUDE FILES
#include <miutset.h> // KUidMsgTypeIMAP4
#include <cacheman.h> // CImPruneMessage
#include <eikenv.h>   // CEikonEnv
#include <mmsvattachmentmanager.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include <cimprunemessage.h>
#endif



#include "MailLog.h"
#include "MsgMailViewerRemoveAttachment.h"
#include "CMailMessage.h"

enum TLoadState
        {
        EChangeStatus = 1,
        EDeleteAttachment,
        EChangeAttachmentEntryStatus,
        EAllDone        
        };
// ================= MEMBER FUNCTIONS =======================

// Constructor
CMsgMailViewerRemoveAttachmentOp::CMsgMailViewerRemoveAttachmentOp(
    CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus,
    TMsvId aMsgID)
    : CMsvOperation(aMsvSession, EPriorityStandard, aObserverRequestStatus)
    {
    CActiveScheduler::Add(this);
    iMtm = KUidMsgTypeIMAP4; // operation possible only for IMAP messages
    iMsgID = aMsgID;
    iState = EDeleteAttachment;
    }

   
// 2nd phase constructor
inline void CMsgMailViewerRemoveAttachmentOp::ConstructL(
    const TMsvId aAttachmentId)
    {
    LOG("CMsgMailViewerRemoveAttachmentOp::ConstructL");
    iCentry = iMsvSession.GetEntryL(iMsgID);
        
    iMsgEntry = iMsvSession.GetEntryL(aAttachmentId);
    
    iPruneMsg = CImPruneMessage::NewL(*iMsgEntry, 
        iMsvSession.FileSession());
    
    iPruneMsg->StartL(aAttachmentId, iStatus);
    iObserverRequestStatus = KRequestPending;
    
    SetActive();
    }

inline void CMsgMailViewerRemoveAttachmentOp::ConstructL(
    CMailMessage& aMailMessage, TMsvAttachmentId aAttachmentId )
    {
    LOG("CMsgMailViewerRemoveAttachmentOp::ConstructL 2");
    aMailMessage.AttachmentManager().RemoveAttachmentL(
        aAttachmentId, iStatus );
    iState = EAllDone;    
    iObserverRequestStatus = KRequestPending;

    SetActive();
    }    


// Symbian OS constructor
CMsgMailViewerRemoveAttachmentOp* 
CMsgMailViewerRemoveAttachmentOp::NewLC(CMsvSession& aMsvSession, 
    TRequestStatus& aObserverRequestStatus, const TMsvId aAttachmentId,
    TMsvId aMsgID)
    {
    CMsgMailViewerRemoveAttachmentOp *op = new(ELeave) 
        CMsgMailViewerRemoveAttachmentOp(
            aMsvSession, aObserverRequestStatus, aMsgID);
    CleanupStack::PushL(op);
    
    op->ConstructL(aAttachmentId);
    return op;
    }
    
     
CMsgMailViewerRemoveAttachmentOp*  CMsgMailViewerRemoveAttachmentOp::NewL(
    CMailMessage& aMailMessage, 
    TRequestStatus& aObserverRequestStatus, 
    TMsvAttachmentId aAttachmentId )
    {
    CMsgMailViewerRemoveAttachmentOp *op = 
        new( ELeave ) CMsgMailViewerRemoveAttachmentOp(
        *aMailMessage.Session(), aObserverRequestStatus, 0 );
    CleanupStack::PushL(op);
    
    op->ConstructL( aMailMessage, aAttachmentId );
    CleanupStack::Pop( op );
        
    return op;
    }    


// Destructor
CMsgMailViewerRemoveAttachmentOp::~CMsgMailViewerRemoveAttachmentOp()
    {
    LOG("CMsgMailViewerRemoveAttachmentOp destructor");
    
    Cancel();
    delete iMsvOp;
    delete iMsgEntry;
    delete iPruneMsg;
    delete iCentry;
    }
    
// Returns progress of the operation using TMsvLocalOperationProgress
const TDesC8& CMsgMailViewerRemoveAttachmentOp::ProgressL()
    {
    return iProgress;
    }

// Cancels outstanding operation
void CMsgMailViewerRemoveAttachmentOp::DoCancel()
    {
    LOG("CMsgMailViewerRemoveAttachmentOp::DoCancel");
    if(iPruneMsg)
        {
        iPruneMsg->Cancel();
        iProgress().iError = KErrCancel;
        }
    
    if(iMsvOp)
        {
        iMsvOp->Cancel();
        iProgress().iError = KErrCancel;
        }
    
    }

// Handles an active object’s request completion event
void CMsgMailViewerRemoveAttachmentOp::RunL()
    {
    LOG("CMsgMailViewerRemoveAttachmentOp::RunL");
    delete iMsvOp;
    iMsvOp = NULL;
    
    switch(iState)
        {
        case EDeleteAttachment:
            LOG("RunL: EDeleteAttachment");
            iState = EChangeStatus;
            break;
        
        case EChangeStatus:
            LOG("RunL: EChangeStatus");
            iState = EChangeAttachmentEntryStatus;
            break;
        
        case EChangeAttachmentEntryStatus:
            LOG("RunL: EChangeAttachmentEntryStatus");
            iState = EAllDone;
            break;            
        
        default:
            LOG("RunL: default");
            break;
        }
    
    DoNextStateL();        
    }
    

// ----------------------------------------------------------------------------
// CMsgMailViewerRemoveAttachmentOp::DoNextStateL()
// ----------------------------------------------------------------------------
//
void CMsgMailViewerRemoveAttachmentOp::DoNextStateL()
    {
    LOG("CMsgMailViewerRemoveAttachmentOp::DoNextState");
    TRequestStatus* status;
    
    switch(iState)
        {
        case EChangeStatus:
            LOG("EChangeStatus");
            UpdateStatusL();
            SetActive();
            break;
        case EChangeAttachmentEntryStatus:
            LOG("EChangeAttachmentEntryStatus");
            UpdateAttachmentStatusL();
            SetActive();
            break;              
        case EAllDone:
            LOG("EAllDone");
            iProgress().iError = iStatus.Int();
            status = &iObserverRequestStatus;
            User::RequestComplete(status, KErrNone);
            break;
        default:
            LOG1("CMsgMailViewerRemoveAttachmentOp::DoNextState:False State%d",
                iState);
            break;
        
        }
    
    }
// ----------------------------------------------------------------------------
// CMsgMailViewerRemoveAttachmentOp::UpdateAttachmentStatusL()
// ----------------------------------------------------------------------------
//
void CMsgMailViewerRemoveAttachmentOp::UpdateAttachmentStatusL()
    {
    TMsvEntry attachment = iMsgEntry->Entry();
    
    //Set attachment entry as not comleted
    attachment.SetComplete( EFalse );
    
    ASSERT( iMsvOp == NULL );         
    iMsvOp = iMsgEntry->ChangeL(attachment, iStatus);
    }


// ----------------------------------------------------------------------------
// CMsgMailViewerRemoveAttachmentOp::UpdateStatusL()
// ----------------------------------------------------------------------------
//
void CMsgMailViewerRemoveAttachmentOp::UpdateStatusL()
    {
    TMsvEntry tentry = iCentry->Entry();
    ASSERT( tentry.iType == KUidMsvMessageEntry );    
    //Set entry as not comleted
    tentry.SetComplete(EFalse);
    ASSERT( iMsvOp == NULL );    
    iMsvOp = iCentry->ChangeL(tentry, iStatus);
    }

// -----------------------------------------------------------------------------
// CMsgMailViewerRemoveAttachmentOp::RunError
// -----------------------------------------------------------------------------
//
TInt CMsgMailViewerRemoveAttachmentOp::RunError(TInt aError)
    {
    Cancel();    
    
    iProgress().iError = aError;
    
    // ActiveSheduler Panics if this returns != KErrNone
    return KErrNone;
    }

// End of File  
