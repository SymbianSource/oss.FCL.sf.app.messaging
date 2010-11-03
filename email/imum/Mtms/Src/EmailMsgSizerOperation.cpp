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
*       Class implementation file
*
*/


// INCLUDES
#include <miutset.h>
#include <s32file.h>
#include <eikenv.h>
#include <eikrutil.h>
#include <eikbtgpc.h>
#include <iapprefs.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <sendui.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <AknQueryDialog.h>
#include <aknnotedialog.h>
#include <StringLoader.h>
#include <mmsvattachmentmanager.h>          // MMsvAttachmentManager

#include "ImumMtmLogging.h"
#include "EmailMsgSizerOperation.h"

// ----------------------------------------------------------------------------
// TMsvAttachmentNamesArray::TMsvAttachmentNamesArray()
// ----------------------------------------------------------------------------
//
TMsvAttachmentNamesArray::TMsvAttachmentNamesArray(
    const CArrayFixFlat<CMsvAttachment>& aAttachmentList)
    :
    iAttachments(aAttachmentList)
    {
    IMUM_CONTEXT( TMsvAttachmentNamesArray::TMsvAttachmentNamesArray, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    }


// ----------------------------------------------------------------------------
// TMsvAttachmentNamesArray::MdcaCount()
// ----------------------------------------------------------------------------
//
TInt TMsvAttachmentNamesArray::MdcaCount() const
    {
    return iAttachments.Count();
    }


// ----------------------------------------------------------------------------
// TMsvAttachmentNamesArray::MdcaPoint()
// ----------------------------------------------------------------------------
//
TPtrC16 TMsvAttachmentNamesArray::MdcaPoint(TInt aIndex) const
    {
    return TPtrC( iAttachments[aIndex].AttachmentName() );
    }


// ----------------------------------------------------------------------------
// CEmailMsgSizerOperation::NewL()
// ----------------------------------------------------------------------------
//
CEmailMsgSizerOperation* CEmailMsgSizerOperation::NewL(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aMsgId,
    TUid aMtm)
    {
    IMUM_STATIC_CONTEXT( CEmailMsgSizerOperation::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CEmailMsgSizerOperation* sizer = new(ELeave) CEmailMsgSizerOperation(
        aMailboxApi, aObserverRequestStatus, aMtm);
    CleanupStack::PushL(sizer);
    sizer->ConstructL(aMsgId);
    CleanupStack::Pop();
    
    IMUM_OUT();
    
    return sizer;
    }

// ----------------------------------------------------------------------------
// CEmailMsgSizerOperation::~CEmailMsgSizerOperation()
// ----------------------------------------------------------------------------
//
CEmailMsgSizerOperation::~CEmailMsgSizerOperation()
    {
    IMUM_CONTEXT( CEmailMsgSizerOperation::~CEmailMsgSizerOperation, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    delete iEmailMsg;
    delete iMsgEntry;
    }


// ----------------------------------------------------------------------------
// CEmailMsgSizerOperation::ProgressL()
// ----------------------------------------------------------------------------
//
const TDesC8& CEmailMsgSizerOperation::ProgressL()
    {
    return iProgress;
    }


// ----------------------------------------------------------------------------
// CEmailMsgSizerOperation::TotalMessageSize()
// ----------------------------------------------------------------------------
//
TInt32 CEmailMsgSizerOperation::TotalMessageSize() const
    {
    return iMsgEntry->Entry().iSize;
    }


// ----------------------------------------------------------------------------
// CEmailMsgSizerOperation::BodyTextSizeL()
// ----------------------------------------------------------------------------
//
TInt32 CEmailMsgSizerOperation::BodyTextSizeL() const
    {
    return TotalMessageSize() - SizeOfAllAttachmentsL();
    }


// ----------------------------------------------------------------------------
// CEmailMsgSizerOperation::SizeOfAllAttachmentsL()
// ----------------------------------------------------------------------------
//
TInt32 CEmailMsgSizerOperation::SizeOfAllAttachmentsL() const
    {
    IMUM_CONTEXT( CEmailMsgSizerOperation::SizeOfAllAttachmentsL, 0, KImumMtmLog );
    IMUM_IN();
    
    TInt32 size = 0;

    MMsvAttachmentManager& attachments = iEmailMsg->AttachmentManager();
    for( TInt i = attachments.AttachmentCount(); --i >= 0; )
        {
        size += GetSizeL( i, attachments );
        }

    IMUM_OUT();
    return size;
    }


// ----------------------------------------------------------------------------
// CEmailMsgSizerOperation::AttachmentSizeL()
// ----------------------------------------------------------------------------
//
TInt32 CEmailMsgSizerOperation::AttachmentSizeL(
    TMsvId aAttachmentId ) const
    {
    IMUM_CONTEXT( CEmailMsgSizerOperation::AttachmentSizeL, 0, KImumMtmLog );
    IMUM_IN();
    
    TInt32 size = 0;
    MMsvAttachmentManager& attachments = iEmailMsg->AttachmentManager();

    // Search through the whole list of attachments
    TInt i = attachments.AttachmentCount();
    while ( --i >= 0 && !size )
        {
        // Create the object of the info item
        CMsvAttachment* attachment = attachments.GetAttachmentInfoL( i );
        CleanupStack::PushL( attachment );

        // if id of the attachment matches with the id provided,
        // increase the size
        if( attachment->Id() == aAttachmentId )
            {
            size = attachment->Size();
            }

        CleanupStack::PopAndDestroy( attachment );
        attachment = NULL;
        }

    User::LeaveIfError( i );
    // Leaves with KErrNotFound (-1) if attachment not found,
    // or no attachments.
    
    IMUM_OUT();    
    return size;
    }


// ----------------------------------------------------------------------------
// CEmailMsgSizerOperation::HasAttachmentLargerThanL()
// ----------------------------------------------------------------------------
//
TBool CEmailMsgSizerOperation::HasAttachmentLargerThanL( TInt32 aSize ) const
    {
    IMUM_CONTEXT( CEmailMsgSizerOperation::HasAttachmentLargerThanL, 0, KImumMtmLog );
    IMUM_IN();
        
    TBool foundBigger = EFalse;
    MMsvAttachmentManager& attachments = iEmailMsg->AttachmentManager();
    for( TInt i = attachments.AttachmentCount();
         --i >= 0 && !foundBigger;
         foundBigger = GetSizeL( i, attachments ) > aSize ) {};

    IMUM_OUT();
    return foundBigger;
    }


// ----------------------------------------------------------------------------
// CEmailMsgSizerOperation::DoCancel()
// ----------------------------------------------------------------------------
//
void CEmailMsgSizerOperation::DoCancel()
    {
    IMUM_CONTEXT( CEmailMsgSizerOperation::DoCancel, 0, KImumMtmLog );
    IMUM_IN();
    
    IMUM3(0,"Cancelling operation with iStatus (0x%x) with errorcode (%d) and msg object (0x%x)", iStatus.Int(), iProgress().iErrorCode, iEmailMsg);
    
    if(iEmailMsg)
        {
        iEmailMsg->Cancel();
        iProgress().iErrorCode = KErrCancel;
        
        // if not already completed, then 
        TRequestStatus* status = &iObserverRequestStatus;
        User::RequestComplete(status, KErrCancel);        
        }

    IMUM_OUT();        
    }


// ----------------------------------------------------------------------------
// CEmailMsgSizerOperation::RunL()
// ----------------------------------------------------------------------------
//
void CEmailMsgSizerOperation::RunL()
    {
    IMUM_CONTEXT( CEmailMsgSizerOperation::RunL, 0, KImumMtmLog );
    IMUM_IN();
    
    iProgress().iErrorCode = iStatus.Int();
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete(status, KErrNone);

    IMUM_OUT();
    }


// ----------------------------------------------------------------------------
// CEmailMsgSizerOperation::CEmailMsgSizerOperation()
// ----------------------------------------------------------------------------
//
CEmailMsgSizerOperation::CEmailMsgSizerOperation(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    TUid aMtm)
    :
    CMsvOperation(aMailboxApi.MsvSession(), KSizerOpPriority, aObserverRequestStatus)
    {
    IMUM_CONTEXT( CEmailMsgSizerOperation::CEmailMsgSizerOperation, 0, KImumMtmLog );
    IMUM_IN();
    
    CActiveScheduler::Add(this);
    iMtm = aMtm;
    IMUM_OUT();    
    }


// ----------------------------------------------------------------------------
// CEmailMsgSizerOperation::ConstructL()
// ----------------------------------------------------------------------------
//
void CEmailMsgSizerOperation::ConstructL(TMsvId aMsgId)
    {
    IMUM_CONTEXT( CEmailMsgSizerOperation::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Create and start the CImEmailMessage active object.
    iMsgEntry = iMsvSession.GetEntryL(aMsgId);
    iEmailMsg = CImEmailMessage::NewL(*iMsgEntry);
    iStatus = KRequestPending;
    iEmailMsg->GetAttachmentsListL(
        iStatus,
        aMsgId,
        CImEmailMessage::EAllAttachments,
        CImEmailMessage::EThisMessageAndEmbeddedMessages);
    SetActive();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CEmailMsgSizerOperation::GetSizeL()
// ----------------------------------------------------------------------------
//
TInt CEmailMsgSizerOperation::GetSizeL(
     const TInt aIndex,
     MMsvAttachmentManager& aList ) const
    {
    IMUM_CONTEXT( CEmailMsgSizerOperation::GetSizeL, 0, KImumMtmLog );
    IMUM_IN();
    
    TInt size = 0;

    // Create the object of the info item
    CMsvAttachment* attachment = aList.GetAttachmentInfoL( aIndex );
    CleanupStack::PushL( attachment );

    size = attachment->Size();

    CleanupStack::PopAndDestroy( attachment );
    attachment = NULL;

    IMUM_OUT();
    return size;
    }


