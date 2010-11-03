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
*       CImapPopulateOp implementation file
*
*/


#include "ImapPopulateOperation.h"

//  General includes
#include <eikenv.h>
#include <eikrutil.h>
#include <StringLoader.h>

//  Messaging includes
#include <miut_err.h>
#include <miutset.h>
#include <msvuids.h>
#include <mtmuibas.h>
#include <imapset.h>
#include <Msvprgreporter.H>

#include <imapcmds.h>
#include <imum.rsg>
#include "ImapConnectionOp.h"
#include "MsvConnectionValidation.h"
#include "MsvEmailConnectionProgressProvider.h"
#include "progtype.h"
#include "ImumPanic.h"
#include "ImumMtmLogging.h"
#include "ImapFolderSyncOperation.h"

#include "emailutils.h"

const TInt KImumMaxLoginRetries = 3;
const TInt KImumOperationsArrayGranularity = 2;


// ----------------------------------------------------
// CImapPopulateOp::NewL
// ----------------------------------------------------
CImapPopulateOp* CImapPopulateOp::NewL(
    CMsvSession& aSession,
    TInt aPriority,
    TRequestStatus& aStatus,
    TMsvId aService,
    MMsvProgressReporter& aProgressReporter)
    {
    CImapPopulateOp* me=new (ELeave) CImapPopulateOp(
        aSession, aPriority, aStatus,
        aService, aProgressReporter );
    CleanupStack::PushL(me);
    me->ConstructL();
    CleanupStack::Pop();    //  me
    return me;
    }

// ----------------------------------------------------
// CImapPopulateOp::CImapPopulateOp
// ----------------------------------------------------
CImapPopulateOp::CImapPopulateOp(
    CMsvSession& aSession,
    TInt aPriority,
    TRequestStatus& aStatus,
    TMsvId aService,
    MMsvProgressReporter& aProgressReporter)
    :
    CMsvOperation(aSession, aPriority, aStatus),
    iState(EPopulateConstructing),
    iSyncProgress(),
    iProgressReporter(aProgressReporter)
    {
    iService=aService;
    iMtm=KUidMsgTypeIMAP4;
    iSyncProgress().iType=EImap4SyncProgressType;
    iCoeEnv = CCoeEnv::Static();
    }

    // ----------------------------------------------------
// CImapPopulateOp::~CImapPopulateOp
// ----------------------------------------------------
CImapPopulateOp::~CImapPopulateOp()
    {
    Cancel();
    iProgressReporter.UnsetProgressDecoder();

    //  Release the shared MtmUi
    if(iMtmUi)
        {
        iProgressReporter.MtmStore().ReleaseMtmUi(iMtm);
        }

    delete iConnectionProgressProvider;
    delete iSubOperation;
    delete iTitleText;
    // don't own iCoeEnv so don't delete it
    }

// ----------------------------------------------------
// CImapPopulateOp::ConstructL
// ----------------------------------------------------
void CImapPopulateOp::ConstructL()
    {
    CMsvEntry* serviceEntry=NULL;

    serviceEntry=iMsvSession.GetEntryL(iService);
    CleanupStack::PushL(serviceEntry);

    iSyncProgress().iState=TImap4SyncProgress::EConnecting;
    iSyncProgress().iErrorCode=KErrNone;

    //  Store the related service Id
    iRelatedServiceId=serviceEntry->Entry().iRelatedId;

    iTitleText = StringLoader::LoadL(
        R_EMAIL_CONNECTING_SERVER,
        serviceEntry->Entry().iDetails,
        iCoeEnv );

    CleanupStack::PopAndDestroy( serviceEntry );

    CActiveScheduler::Add(this);

    iObserverRequestStatus=KRequestPending;

    TRequestStatus* myStatus=&iStatus;
    User::RequestComplete(myStatus,KErrNone);

    SetActive();
    //  We complete ourselves so that the dialog gets a chance to redraw
    //  before entering the lengthy process of opening system agent and
    //  connection progress provider
    }


// ----------------------------------------------------
// CImapPopulateOp::RunL
// ----------------------------------------------------
void CImapPopulateOp::RunL()
    {
	DoRunL();
    }

// ----------------------------------------------------
// CImapPopulateOp::DoRunL
// ----------------------------------------------------
void CImapPopulateOp::DoRunL()
    {
    if(iState==EPopulateConstructing)
        {
        FinishConstructionL();
        }
    else
        {
        ConnectionCompletionL();
        }
    }

// ----------------------------------------------------
// CImapPopulateOp::RunError
// ----------------------------------------------------
TInt CImapPopulateOp::RunError(TInt aError)
	{
    iSyncProgress().iErrorCode = aError;
    Complete();		
	}
    
// ----------------------------------------------------
// CImapPopulateOp::FinishConstructionL
// ----------------------------------------------------
void CImapPopulateOp::FinishConstructionL()
    {

    iMtmUi=&(iProgressReporter.MtmStore().ClaimMtmUiL(iMtm));
    iMtmUi->BaseMtm().SwitchCurrentEntryL(iService);


    iConnectionProgressProvider=CMsvEmailConnectionProgressProvider::NewL(
        iMsvSession, iService);

    CImImap4Settings* settings = new(ELeave) CImImap4Settings;
    CleanupStack::PushL( settings );
    CMsvEntry* cEntry = iMsvSession.GetEntryL(iService);
    CleanupStack::PushL( cEntry );
    MsvEmailMtmUiUtils::RestoreSettingsL( *cEntry, *settings );

    TInt bodySizeLimit = settings->BodyTextSizeLimit();

    // -1 for full mail body
    // > 0 for some KB
    // KMaxTInt32 is default value, so no limits set
    if( bodySizeLimit == -1 || bodySizeLimit > 0 || bodySizeLimit < KMaxTInt32 )
        {
        iState = EPopulating;
        iSubOperation = DoPopulateL( *settings );
        CleanupStack::PopAndDestroy(2); // CSI: 47 # settings, cEntry
        }
    else
        {
        CleanupStack::PopAndDestroy(2); // CSI: 47 # settings, cEntry
        }


    SetActive();
    //
    //  We're going to try and connect; tell our MMsvProgressReporter
    //  to come to us for progress.
    //
    iProgressReporter.SetProgressDecoder(*this);

    iObserverRequestStatus=KRequestPending;
    }

// ----------------------------------------------------
// CImapPopulateOp::ConnectionCompletionL
// ----------------------------------------------------
void CImapPopulateOp::ConnectionCompletionL()
    {
    //  Called by RunL when past EPopulateConstructing state
    TInt err=KErrNone;
    if(iSubOperation)
        {
        //  The sub connection operation has completed.

        //  We need to detect a failure to connect due to invalid login details,
        //  and prompt the user accordingly.
        //
        TBuf<CBaseMtmUi::EProgressStringMaxLen> dummyString;
        TInt dummyInt[4]; // CSI: 47 # For dummyInt

        err=DecodeProgress(iSubOperation->ProgressL(),dummyString,dummyInt[0],dummyInt[1],dummyInt[2],dummyInt[3],ETrue); // CSI: 47 # For dummyInts and dummyStrings.
        }


    User::LeaveIfError(err);

    Complete();
    }

// ----------------------------------------------------
// CImapPopulateOp::DoCancel
// ----------------------------------------------------
void CImapPopulateOp::DoCancel()
    {
    if(iSubOperation)
        iSubOperation->Cancel();

    // Unset password.
    if(iResetPassword)
        {
        TRAPD(ignore, MsvConnectionValidation::UnsetPasswordL(iMsvSession,iService));
        }

    iSyncProgress().iErrorCode=KErrCancel;
    Complete();
    }

// ----------------------------------------------------
// CImapPopulateOp::ProgressL
// ----------------------------------------------------
const TDesC8& CImapPopulateOp::ProgressL()
    {
    if(iSubOperation)
        {
        return iSubOperation->ProgressL();
        }
    else
        {
        return iSyncProgress;
        }
    }

// ----------------------------------------------------
// CImapPopulateOp::DecodeProgress
// ----------------------------------------------------
TInt CImapPopulateOp::DecodeProgress(const TDesC8& aProgress,
        TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString,
        TInt& aTotalEntryCount, TInt& aEntriesDone,
        TInt& aCurrentEntrySize, TInt& aCurrentBytesTrans, TBool aInternal)
    {
    if(aProgress.Length()==0)
        return KErrNone;

    TPckgC<TInt> type(0);
    type.Set(aProgress.Left(sizeof(TInt)));

    if(!aInternal || type()!=EUiProgTypeConnecting)
        return iMtmUi->GetProgress(aProgress, aReturnString, aTotalEntryCount,
            aEntriesDone, aCurrentEntrySize, aCurrentBytesTrans);
    return KErrNone;
    }


// ----------------------------------------------------
// CImapPopulateOp::Complete
// ----------------------------------------------------
void CImapPopulateOp::Complete()
    {
    TRequestStatus* observer=&iObserverRequestStatus;
    User::RequestComplete(observer, KErrNone);
    }


CMsvOperation* CImapPopulateOp::DoPopulateL( CImImap4Settings& aSettings )
    {
    iPopulated = ETrue;
    //iStatus = KRequestPending;

    TImImap4GetPartialMailInfo info;

    CMsvEntry* cEntry = iMsvSession.GetEntryL(iService);
    CleanupStack::PushL( cEntry );


    TInt bodySizeLimit = aSettings.BodyTextSizeLimit();
    TInt attaSizeLimit = aSettings.AttachmentSizeLimit();

    info.iBodyTextSizeLimit = bodySizeLimit;
    info.iAttachmentSizeLimit = attaSizeLimit;
    info.iTotalSizeLimit = ( bodySizeLimit + attaSizeLimit );//??
    info.iPartialMailOptions = aSettings.PartialMailOptions();

    TPckg<TImImap4GetPartialMailInfo> param(info);

    CMsvEntrySelection* selection = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    selection->AppendL( iService );

    TMsvId inbox = FindInboxL( *cEntry );
    selection->AppendL( inbox );

    CMsvOperation* op = iMtmUi->InvokeAsyncFunctionL(
        KIMAP4MTMPopulateNewMailWhenAlreadyConnected,
        //KIMAP4MTMPopulateAllMailWhenAlreadyConnected,
        *selection,
        iStatus,
        param);


    CleanupStack::PopAndDestroy(2); // CSI: 47 # selection, centry

    return op;
    }

TMsvId CImapPopulateOp::FindInboxL( CMsvEntry& aEntry )
    {
    TMsvId inboxId = KErrNotFound;
    TMsvEntry child;
    const TInt count = aEntry.Count();
    _LIT( KTMceUiInboxName, "INBOX");
    for (TInt loop = 0; loop < count && inboxId == KErrNotFound; loop++)
        {
        child = (aEntry)[loop];
        if ( child.iType == KUidMsvFolderEntry  &&
             child.iDetails.CompareF( KTMceUiInboxName ) == 0 )
            {
            inboxId = child.Id();
            }
        }

    return inboxId;
    }

// End of File
