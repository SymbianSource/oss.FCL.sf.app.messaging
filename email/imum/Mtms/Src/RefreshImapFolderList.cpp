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
*       folder refresh operation
*
*/


//  Messaging includes
#include <miutset.h>
#include <imapcmds.h>

//  Specific includes
#include "RefreshImapFolderList.h"
#include "ImapConnectionOp.h"
#include "ImumDisconnectOperation.h"
#include "ImumPanic.h"
#include "ImumMtmLogging.h"


// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
CImapConnectAndRefreshFolderList* CImapConnectAndRefreshFolderList::NewL(
    CImumInternalApi& aMailboxApi, TInt aPriority, TRequestStatus& aObserverRequestStatus,
    TMsvId aService, MMsvProgressReporter& aProgressReporter)
    {
    IMUM_STATIC_CONTEXT( CImapConnectAndRefreshFolderList::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CImapConnectAndRefreshFolderList* self=new (ELeave) CImapConnectAndRefreshFolderList
        (aMailboxApi, aPriority, aObserverRequestStatus, aService, aProgressReporter);
    CleanupStack::PushL(self);
    self->ConstructL(KUidMsgTypeIMAP4);
    CleanupStack::Pop();
    IMUM_OUT();
    return self;
    }

// ----------------------------------------------------------------------------
// CImapConnectAndRefreshFolderList
// ----------------------------------------------------------------------------
CImapConnectAndRefreshFolderList::CImapConnectAndRefreshFolderList(
    CImumInternalApi& aMailboxApi, TInt aPriority, TRequestStatus& aObserverRequestStatus,
    TMsvId aService, MMsvProgressReporter& aProgressReporter)
:   CImumOnlineOperation(aMailboxApi, aPriority, aObserverRequestStatus, aProgressReporter)
    {
    IMUM_CONTEXT( CImapConnectAndRefreshFolderList::CImapConnectAndRefreshFolderList, 0, KImumMtmLog );
    IMUM_IN();
    
    iService=aService;
    IMUM_OUT();
    };

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
void CImapConnectAndRefreshFolderList::ConstructL(TUid aMtm)
    {
    IMUM_CONTEXT( CImapConnectAndRefreshFolderList::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    BaseConstructL(aMtm);   //  Adds us to the scheduler, creates us
                            //  an MtmUi, sets progress reporter width

    iOperation=CImapConnectionOp::NewL(
            iMailboxApi.MsvSession(), CActive::EPriorityStandard,
            iStatus, iService, iReporter,
            CImapConnectionOp::ECompleteAfterConnect );

    iObserverRequestStatus=KRequestPending;
    SetActive();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// ~CImapConnectAndRefreshFolderList
// ----------------------------------------------------------------------------
CImapConnectAndRefreshFolderList::~CImapConnectAndRefreshFolderList()
    {
    IMUM_CONTEXT( CImapConnectAndRefreshFolderList::~CImapConnectAndRefreshFolderList, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    }

// ----------------------------------------------------------------------------
// GetErrorProgressL
// ----------------------------------------------------------------------------
const TDesC8& CImapConnectAndRefreshFolderList::GetErrorProgressL(TInt aError)
    {
    IMUM_CONTEXT( CImapConnectAndRefreshFolderList::GetErrorProgressL, 0, KImumMtmLog );
    IMUM_IN();
    
    TImap4GenericProgress& genProgress=iProgressBuf().iGenericProgress;
    genProgress.iErrorCode=aError;

    switch(iState)
        {
    case EConnecting:
        //  RunL left while trying to initiate a refresh...
        genProgress.iOperation=TImap4GenericProgress::ESync;
        genProgress.iState=TImap4GenericProgress::ESyncing;
        break;
    case EDisconnecting:
        //  If a RunL leave occurred after disconnection, no problem...
        genProgress.iErrorCode=KErrNone;
        //  Fall through
    case ERefreshing:
        //  RunL left while trying to initiate a disconnect...
        genProgress.iOperation=TImap4GenericProgress::EDisconnect;
        genProgress.iState=TImap4GenericProgress::EDisconnecting;
        break;
    default:
        User::Panic(KImumMtmUiPanic,EIMAP4MtmUiBadStateInFolderListOp);
        }
    IMUM_OUT();
    return iProgressBuf;
    }

// ----------------------------------------------------------------------------
// DoRunL
// ----------------------------------------------------------------------------
void CImapConnectAndRefreshFolderList::DoRunL()
    {
    IMUM_CONTEXT( CImapConnectAndRefreshFolderList::DoRunL, 0, KImumMtmLog );
    IMUM_IN();
    
    if(GetOperationCompletionCodeL()!=KErrNone || iState==EDisconnecting)
        {
        iState=ECompleted;
        CompleteObserver();
        return;
        }

    switch(iState)
        {
    case EConnecting:
        //  We have successfully completed connecting
        delete iOperation;
        iOperation=NULL;
        InvokeClientMtmAsyncFunctionL(KIMAP4MTMSyncTree, iService, iService);
        iState=ERefreshing;
        SetActive();
        break;
    case ERefreshing:
        //  We have successfully completed refreshing the folder list
        delete iOperation;
        iOperation=NULL;
        iOperation=CImumDisconnectOperation::NewL(iMailboxApi,
            iStatus, iReporter, iService, KIMAP4MTMDisconnect, iMtm);
        iState=EDisconnecting;
        SetActive();
        break;
    default:
        User::Panic(KImumMtmUiPanic,EIMAP4MtmUiBadStateInFolderListOp);
        };
    IMUM_OUT();
    }

// End of File

