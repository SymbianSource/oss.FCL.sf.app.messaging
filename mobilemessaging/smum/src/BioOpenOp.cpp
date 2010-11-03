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
*     Bio message opening operation.
*
*/



// INCLUDE FILES
#include <coemain.h>    // CCoeEnv needed for file session
// Messaging includes
#include <MTMStore.h>   // for CMtmStore
#include <bioscmds.h>   // for KBiosMtmParse
#include <biodb.h>      // CBIODatabase
// Local includes
#include "BioOpenOp.h"
#include "SMSU.H"       // for CSmsMtmUi
#include "smsui.pan"    // SMUM panic codes

// CONSTANTS
const TUid KUidSmsMtm={0x1000102c};
const TUid KUidBioMtm={0x10001262};

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CBioOpenOp::NewL
//
// ---------------------------------------------------------
CBioOpenOp* CBioOpenOp::NewL(CMsvSession& aSession, TRequestStatus& aObserverStatus, TMsvId aId, TUint aFlags)
    {
    CBioOpenOp* self=new(ELeave) CBioOpenOp(aSession,aObserverStatus,aId);
    CleanupStack::PushL(self);
    self->ConstructL(aFlags);
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------
// CBioOpenOp::CBioOpenOp
//
// ---------------------------------------------------------
CBioOpenOp::CBioOpenOp(CMsvSession& aSession,TRequestStatus& aObserverStatus, TMsvId aId)
    : CMsvOperation(aSession,CActive::EPriorityStandard,aObserverStatus),
      iMsvId(aId), iState(EUnknown)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------
// CBioOpenOp::ConstructL
//
// ---------------------------------------------------------
void CBioOpenOp::ConstructL(TUint aFlags)
    {
    iMtmStore=CMtmStore::NewL(iMsvSession);
    
    // Get SMS MTM UI and set its context
    iSmsMtmUi=&(iMtmStore->ClaimMtmUiL(KUidSmsMtm));
    iSmsMtmUiClaimed=ETrue;
    iSmsMtmUi->SetPreferences(aFlags);
    iSmsMtmUi->BaseMtm().SetCurrentEntryL(iMsvSession.GetEntryL(iMsvId));

    // Get Bio MTM UI and set its context
    iBioMtmUi=&(iMtmStore->ClaimMtmUiL(KUidBioMtm));
    iBioMtmUiClaimed=ETrue;
    iBioMtmUi->BaseMtm().SetCurrentEntryL(iMsvSession.GetEntryL(iMsvId));

    //
    // Check if needs parsing
    //
    CBIODatabase* bioDb = CBIODatabase::NewLC(CCoeEnv::Static()->FsSession());
    CMsvEntry* entry = iMsvSession.GetEntryL(iMsvId);
    CleanupStack::PushL(entry);
    TMsvEntry tentry = entry->Entry(); 
    TUid bioUid;
    bioUid.iUid = tentry.iBioType;

    TBool parserAvailable( EFalse );    
    if ( bioDb->GetBioParserNameL( bioUid ).Length() )
        {
        parserAvailable = ETrue;
        }
    // Set the description
    TInt bioDbIndex;
    bioDb->GetBioIndexWithMsgIDL(bioUid, bioDbIndex);
    HBufC* descr = bioDb->BifReader(bioDbIndex).Description().AllocLC();
    tentry.iDescription.Set(*descr);
    // save entry, because description was changed
    entry->ChangeL(tentry);
    CleanupStack::PopAndDestroy(descr);

    CleanupStack::PopAndDestroy(2, bioDb); // entry bioDatabase
    if ( parserAvailable )
        {
        // Start off first operation
        DoParseL(); // (after parsing it calls DoEditL() )
        }
    else
        {
        DoEditL();
        }
    }

// ---------------------------------------------------------
// CBioOpenOp::~CBioOpenOp
//
// ---------------------------------------------------------
CBioOpenOp::~CBioOpenOp()
    {
    Cancel();
    delete iOperation;

    if(iMtmStore)
        {
        if(iSmsMtmUiClaimed)
            {
            iMtmStore->ReleaseMtmUi(KUidSmsMtm);
            }
        if(iBioMtmUiClaimed)
            {
            iMtmStore->ReleaseMtmUi(KUidBioMtm);
            }
        delete iMtmStore;
        }
    }

// ---------------------------------------------------------
// CBioOpenOp::DoParseL
//
// ---------------------------------------------------------
void CBioOpenOp::DoParseL()
    {
    __ASSERT_DEBUG(iOperation==NULL,Panic(EBioOperationNotNull));

    // Set iMtm of message as BIO MTM
    CMsvEntry* entry=iMsvSession.GetEntryL(iMsvId);
    CleanupStack::PushL(entry);
    TMsvEntry tentry;
    tentry=entry->Entry(); 
    tentry.iMtm=KUidBioMtm;
    entry->ChangeL(tentry);
    CleanupStack::PopAndDestroy();      // entry

    // Parse message
    TBuf8<1> blankParams;
    CMsvEntrySelection* sel=new(ELeave) CMsvEntrySelection();
    CleanupStack::PushL(sel);
    sel->AppendL(iMsvId);   // Message Server will use UID of 1st message to load in MTM server.

    SetActive();  
    iOperation=iBioMtmUi->InvokeAsyncFunctionL(KBiosMtmParse,*sel,iStatus,blankParams);
    CleanupStack::PopAndDestroy(); // sel

    iState=EParsing;
    iObserverRequestStatus = KRequestPending;
    }

// ---------------------------------------------------------
// CBioOpenOp::DoEditL
//
// ---------------------------------------------------------
void CBioOpenOp::DoEditL()
    {
    delete iOperation;
    iOperation=NULL;
    
    // Launch SMS editor
    SetActive();
    iOperation=STATIC_CAST(CSmsMtmUi*,iSmsMtmUi)->LaunchEditorApplicationL(iStatus,iMsvSession,CSmsMtmUi::EEditExisting);
    iState=EEditing;
    }

// ---------------------------------------------------------
// CBioOpenOp::RunL
//
// ---------------------------------------------------------    
void CBioOpenOp::RunL()
    {
    __ASSERT_DEBUG(iOperation,Panic(EBioOperationDoesNotExist));

    switch(iState)
        {
    case EParsing:      // The parse has completed.
        {
        // Set iMtm of message back to SMS MTM
        {
        CMsvEntry* entry=iMsvSession.GetEntryL(iMsvId);
        CleanupStack::PushL(entry);
        TMsvEntry tentry(entry->Entry()); 
        tentry.iMtm=KUidSmsMtm;
        tentry.SetReadOnly(EFalse);
        entry->ChangeL(tentry);
        CleanupStack::PopAndDestroy();      // entry
        }

        TBuf<CBaseMtmUi::EProgressStringMaxLen> bufNotUsed1;
        TInt intNotUsed2;
        TInt intNotUsed3;
        TInt intNotUsed4;
        TInt intNotUsed5;
        const TInt err=iBioMtmUi->GetProgress(iOperation->FinalProgress(),bufNotUsed1,intNotUsed2,intNotUsed3,intNotUsed4,intNotUsed5);

        if(KErrNone==err)
            {
            DoEditL();
            }
        else
            {
            CompleteObserver();
            }
        }
        break;
    case EEditing:      // Edit has completed.
        CompleteObserver();
        break;
    case EUnknown:
        break;
    default:
        break;
        }
    }

// ---------------------------------------------------------
// CBioOpenOp::ProgressL
//
// ---------------------------------------------------------
const TDesC8& CBioOpenOp::ProgressL()
    {
    if(iOperation)
        {
        return iOperation->ProgressL();
        }
    return KNullDesC8;
    }

// ---------------------------------------------------------
// CBioOpenOp::DoCancel
//
// ---------------------------------------------------------
void CBioOpenOp::DoCancel()
    {
    if(iOperation)
        {
        iOperation->Cancel();
        }
    CompleteObserver();
    }

// ---------------------------------------------------------
// CBioOpenOp::RunError
//
// ---------------------------------------------------------
TInt CBioOpenOp::RunError(TInt /*aError*/)
    {
    CompleteObserver();
    return KErrNone;
    }

// ---------------------------------------------------------
// CBioOpenOp::CompleteObserver
//
// ---------------------------------------------------------
void CBioOpenOp::CompleteObserver()
    {
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete(status,KErrNone);
    iState=EUnknown;
    }

// end of file
