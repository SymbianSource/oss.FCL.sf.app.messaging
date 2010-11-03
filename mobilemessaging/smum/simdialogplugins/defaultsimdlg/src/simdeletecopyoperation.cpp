/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: CSimDeleteCopyOperation header
 *
 */

// INCLUDE FILES
#include <AknProgressDialog.h>  // CAknProgressDialog
#include <eikprogi.h>           // CEikProgressInfo
#include <StringLoader.h>       // StringLoader
#include <msvids.h>             // for msv entry ids
#include <msvapi.h>             // for CMsvSession
#include <smscmds.h>            // for async sms commands
#include <smut.h>               // TSmsUtilities::ServiceIdL

#include "simdeletecopyoperation.h" // for CSimDeleteCopyOperation
#include "SMSU.H"               // for CSmsMtmUi
#include "SMSU.HRH"             // for command ids
#include <defaultsimdialog.rsg>             // for resource ids

//For logging
#include "SmumLogging.h"

// CONSTANTS
const TInt KSmumNoteStringLength = 128;

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSimDeleteCopyOperation::CSimDeleteCopyOperation(
    CMsvSession& aMsvSession,
    CSmsMtmUi& aSmsMtmUi,
    TRequestStatus& aObserverRequestStatus,
    TMsvId aDestinationId,
    CAknProgressDialog& aProgressDialog,
    TBool aCopying,
    CCoeEnv* aCoeEnv )
    :
    CMsvOperation(
        aMsvSession, 
        CActive::EPriorityStandard, 
        aObserverRequestStatus), 
    iDestinationId( aDestinationId ),
    iCopying( aCopying ),
    iSmsMtmUi( aSmsMtmUi ),
    iProgressDialog( aProgressDialog ),
    iCoeEnv( aCoeEnv ),
    iBlank( KNullDesC8 )
    {
    CActiveScheduler::Add(this);
    }


// Symbian OS default constructor can leave.
void CSimDeleteCopyOperation::ConstructL( CMsvEntrySelection* aSelection )
    {
    SMUMLOGGER_ENTERFN("CSimDeleteCopyOperation::ConstructL");
    iEntrySelection = aSelection->CopyL();
    iEntryCount = iEntrySelection->Count();
    if ( iEntryCount == 0 )
        { // no entries in selection -> complete
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }
    else
        { // entries in selection -> proceed
        // Save SMS Service id to member
        CMsvEntry* root = iMsvSession.GetEntryL( KMsvRootIndexEntryId );
        CleanupStack::PushL( root );
        TSmsUtilities::ServiceIdL( *root, iSmsServiceId );
        CleanupStack::PopAndDestroy( root);
        
        // Load correct string to member
        if ( iEntryCount == 1 )
            {// simple string is enough
            iNoteString = StringLoader::LoadL( iCopying 
                ? R_QTN_MCE_WAIT_COPY_SIM_DLG : R_QTN_MCE_WAIT_DELETE_SIM_DLG, iCoeEnv );
            }
        else
            {// prepare string with formatting second number already
             // to string
            TBuf<KSmumNoteStringLength> tbuf;
            HBufC* buf = StringLoader::LoadL( iCopying 
                ? R_QTN_MCE_WAIT_COPY_MANY_SIM_DLG : R_QTN_MCE_WAIT_DELETE_MANY_SIM_DLG, 
                iCoeEnv );
            StringLoader::Format( tbuf, buf->Des(), 1, iEntryCount );
            delete buf;
            iNoteString = tbuf.AllocL();
            }
        
        // Actual operation
        CEikProgressInfo* bar = iProgressDialog.GetProgressInfoL();
        bar->SetFinalValue( iEntryCount );
        
        DeleteCopyNext();
        bar->SetAndDraw( iCurrentEntryIndex + 1 );
        }
    SMUMLOGGER_LEAVEFN("CSimDeleteCopyOperation::ConstructL");
    }


// Two-phased constructor.
CSimDeleteCopyOperation* CSimDeleteCopyOperation::NewL(
    CMsvSession& aMsvSession, 
    CSmsMtmUi& aSmsMtmUi,
    TRequestStatus& aObserverRequestStatus, 
    CMsvEntrySelection* aSelection, 
    TMsvId aDestinationId,
    CAknProgressDialog& aProgressDialog,
    CCoeEnv* aCoeEnv )
    {
    SMUMLOGGER_ENTERFN("CSimDeleteCopyOperation::NewL");
    CSimDeleteCopyOperation* self=new (ELeave) CSimDeleteCopyOperation(
        aMsvSession, 
        aSmsMtmUi, 
        aObserverRequestStatus, 
        aDestinationId, 
        aProgressDialog, 
        ETrue,
        aCoeEnv );
    CleanupStack::PushL(self);
    self->ConstructL( aSelection );
    CleanupStack::Pop(); // self
    SMUMLOGGER_LEAVEFN("CSimDeleteCopyOperation::NewL");
    return self;
    }

// Two-phased constructor.
CSimDeleteCopyOperation* CSimDeleteCopyOperation::NewL(
    CMsvSession& aMsvSession, 
    CSmsMtmUi& aSmsMtmUi,
    TRequestStatus& aObserverRequestStatus, 
    CMsvEntrySelection* aSelection, 
    CAknProgressDialog& aProgressDialog,
    CCoeEnv* aCoeEnv )
    {
    SMUMLOGGER_ENTERFN("CSimDeleteCopyOperation::NewL 2");
    CSimDeleteCopyOperation* self=new (ELeave) CSimDeleteCopyOperation(
        aMsvSession, 
        aSmsMtmUi, 
        aObserverRequestStatus, 
        KMsvNullIndexEntryId, 
        aProgressDialog, 
        EFalse, 
        aCoeEnv );
    CleanupStack::PushL(self);
    self->ConstructL( aSelection );
    CleanupStack::Pop(); // self
    SMUMLOGGER_LEAVEFN("CSimDeleteCopyOperation::NewL 2");
    return self;
    }


    
//destructor
CSimDeleteCopyOperation::~CSimDeleteCopyOperation()
    {
    SMUMLOGGER_ENTERFN("CSimDeleteCopyOperation::~CSimDeleteCopyOperation");
    Cancel();
    delete iOperation;
    delete iEntrySelection;
    delete iNoteString;
    SMUMLOGGER_LEAVEFN("CSimDeleteCopyOperation::~CSimDeleteCopyOperation");
    }

// ---------------------------------------------------------------------------
// CSimDeleteCopyOperation::DeleteCopyNext()
//
// ---------------------------------------------------------------------------
void CSimDeleteCopyOperation::DeleteCopyNext()
    {
    SMUMLOGGER_ENTERFN("CSimDeleteCopyOperation::DeleteCopyNext");
    delete iOperation;
    iOperation = NULL;
    
    TInt err = KErrNone;
    if ( iCanceling )
        {
#ifdef _DEBUG
        RDebug::Print(_L("CSimDeleteCopyOperation::DeleteCopyNext, canceling...") );
#endif
        err = KErrCancel;
        }
    else
        {
#ifdef _DEBUG
        RDebug::Print(_L("CSimDeleteCopyOperation::DeleteCopyNext, do next...") );
#endif
        TRAP(err, MakeNextDeleteCopyL());
        }
    
    // and set active
    iStatus=KRequestPending;
    SetActive();
    // if error then complete this pass with the error code
    if (err)
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, err);
        }
    SMUMLOGGER_LEAVEFN("CSimDeleteCopyOperation::DeleteCopyNext");
    }


// ---------------------------------------------------------------------------
// CSimDeleteCopyOperation::MakeNextDeleteCopyL()
//
// ---------------------------------------------------------------------------
void CSimDeleteCopyOperation::MakeNextDeleteCopyL()
    {
    SMUMLOGGER_ENTERFN("CSimDeleteCopyOperation::MakeNextDeleteCopyL");
    CEikProgressInfo* bar = iProgressDialog.GetProgressInfoL();
    bar->SetAndDraw( iCurrentEntryIndex + 1 );

    // Update (if necessary) the note
    if ( iEntryCount == 1 )
        {
        iProgressDialog.SetTextL( *iNoteString );
        }
    else
        { // update the first number in string
        TBuf<KSmumNoteStringLength> tbuf;
        StringLoader::Format( tbuf, iNoteString->Des(), 0, iCurrentEntryIndex + 1 );
        iProgressDialog.SetTextL( tbuf );
        }

    // Create selection to be given to MTM
    CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection();
    CleanupStack::PushL( selection );
    // Put SMS Service id as first id in array
    selection->AppendL( iSmsServiceId );
    // ...and then current id
    selection->AppendL( iEntrySelection->At( iCurrentEntryIndex ) );

    if ( iCopying )
        {
        TPckgBuf<TMsvId> pkg( iDestinationId );
        iOperation = iSmsMtmUi.BaseMtm().InvokeAsyncFunctionL(
            ESmsMtmCommandCopyFromPhoneStore,
            *selection, pkg, iStatus );
        }
    else
        {
        TBuf8<1> foo (KNullDesC8);
        iOperation = iSmsMtmUi.BaseMtm().InvokeAsyncFunctionL(
            ESmsMtmCommandDeleteFromPhoneStore,
            *selection, foo, iStatus);
        }

    CleanupStack::PopAndDestroy( selection );
    SMUMLOGGER_LEAVEFN("CSimDeleteCopyOperation::MakeNextDeleteCopyL");
    }

// ---------------------------------------------------------------------------
// CSimDeleteCopyOperation::DoCancel()
//
// ---------------------------------------------------------------------------
void CSimDeleteCopyOperation::DoCancel()
    {
#ifdef _DEBUG
    RDebug::Print(_L("CSimDeleteCopyOperation::DoCancel") );
#endif
    if (iOperation)
        {
        iOperation->Cancel();
        }
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete(status, iStatus.Int());
    }

// ---------------------------------------------------------------------------
// CSimDeleteCopyOperation::CancelAfterCurrentCompleted()
//
// ---------------------------------------------------------------------------
void CSimDeleteCopyOperation::CancelAfterCurrentCompleted()
    {
    iCanceling = ETrue;
    }

// ---------------------------------------------------------------------------
// CSimDeleteCopyOperation::RunL()
//
// ---------------------------------------------------------------------------
void CSimDeleteCopyOperation::RunL()
    {
    SMUMLOGGER_ENTERFN("CSimDeleteCopyOperation::RunL");
    // no error handling since we immediately try with
    // the next one

    iCurrentEntryIndex++;
    if ( iCurrentEntryIndex >= iEntrySelection->Count() )
        {
        // nothing left to process, so complete the observer
        TRequestStatus* status = &iObserverRequestStatus;
        User::RequestComplete(status, iStatus.Int());
        }
    else 
        {
        // start the next pass
        DeleteCopyNext();
        }
    SMUMLOGGER_LEAVEFN("CSimDeleteCopyOperation::RunL");
    }

// ---------------------------------------------------------------------------
// CSimDeleteCopyOperation::ProgressL()
//
// ---------------------------------------------------------------------------
const TDesC8& CSimDeleteCopyOperation::ProgressL()
    {
    if (iOperation)
        return iOperation->ProgressL();
    return iBlank;
    }


//  End of File

