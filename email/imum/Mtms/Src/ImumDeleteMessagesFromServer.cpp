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
* Description:  ImumDeleteMessagesFromServer.cpp
*
*/



// INCLUDE FILES
#include <MTMStore.h>
#include <msvstd.hrh>
#include <mtmuibas.h>
#include <msvuids.h>
#include <imum.rsg>
#include <coemain.h>
#include <StringLoader.h>
#include <imapset.h>
#include <pop3set.h>
#include <miut_err.h> // KErrImapServerBusy
#include <eikenv.h>
#include <f32file.h>

#include "ImumDeleteMessagesFromServer.h"
#include "ImumDeleteMessagesLocally.h"
#include "EmailUtils.H"
#include "ImumMtmLogging.h"


// LOCAL CONSTANTS AND MACROS
const TInt KImumEntriesDoneReplaceIndex = 0;
const TInt KImumTotalEntriesReplaceIndex = 1;
// minimum disk space needed when deleting messages locally
const TInt KImumDiskSpaceForDelete = ( 2*1024 );
const TUid KImumMtmImap4Uid = { 0x1000102A };
const TUid KImumMtmPop3Uid = { 0x10001029 };
_LIT8( KEmptyChar, "" );

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CImumDeleteMessagesFromServer::CImumDeleteMessagesFromServer()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CImumDeleteMessagesFromServer::CImumDeleteMessagesFromServer(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus )
    :
    CMsvOperation(aMailboxApi.MsvSession(), CActive::EPriorityStandard, aObserverRequestStatus),
    iMailboxApi( aMailboxApi ),
    iBlank( KEmptyChar ),
    iDriveNo( KErrNotFound )
    {
    IMUM_CONTEXT( CImumDeleteMessagesFromServer::CImumDeleteMessagesFromServer, 0, KImumMtmLog );
    IMUM_IN();

    CActiveScheduler::Add(this);
    IMUM_OUT();
    }


// -----------------------------------------------------------------------------
// CImumDeleteMessagesFromServer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CImumDeleteMessagesFromServer::ConstructL(
    CMsvEntrySelection& aDeletedEntries )
    {
    IMUM_CONTEXT( CImumDeleteMessagesFromServer::ConstructL, 0, KImumMtmLog );
    IMUM_IN();

    iEntrySelection = aDeletedEntries.CopyL();
    iEntryCount = iEntrySelection->Count();
    if ( iEntryCount == 0 )
        {
        User::Leave( KErrNotSupported );
        }

    // Define the environment
    CEikonEnv* env = CEikonEnv::Static();

    // Create the file session and get access to current drive
    iFileSession = env->FsSession();
    iDriveNo = iMsvSession.CurrentDriveL();

    // Reserve disk space for deletion
    User::LeaveIfError( MsvEmailMtmUiUtils::AcquireDiskSpace(
        iFileSession, iDriveNo, KImumDiskSpaceForDelete ) );

    TMsvId serviceId;
    TMsvEntry entry;
    User::LeaveIfError(
        iMsvSession.GetEntry(
            (*iEntrySelection)[0], serviceId, entry ) );
    iEntry = CMsvEntry::NewL(
        iMsvSession, entry.Parent(), TMsvSelectionOrdering() );
    iMtm = iEntry->Entry().iMtm;

    TInt resourceId = (iEntryCount>1) ?
        R_IMAP4_PROGRESS_DELETING_MANY : R_IMAP4_PROGRESS_DELETING;

    iProgressText = StringLoader::LoadL( resourceId, env );
    iStatus=KRequestPending;
    StartNextDeleteLocally();
    // StartNextDeleteLocally activates this
    IMUM_OUT();
    }

// -----------------------------------------------------------------------------
// CImumDeleteMessagesFromServer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CImumDeleteMessagesFromServer* CImumDeleteMessagesFromServer::NewL(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    CMsvEntrySelection& aDeletedEntries )
    {
    IMUM_STATIC_CONTEXT( CImumDeleteMessagesFromServer::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();

    CImumDeleteMessagesFromServer* self=new (ELeave) CImumDeleteMessagesFromServer(
        aMailboxApi, aObserverRequestStatus );
    CleanupStack::PushL(self);
    self->ConstructL( aDeletedEntries );
    CleanupStack::Pop(); // self
    IMUM_OUT();
    return self;
    }

// -----------------------------------------------------------------------------
// CImumDeleteMessagesFromServer::~CImumDeleteMessagesFromServer()
// -----------------------------------------------------------------------------
//
CImumDeleteMessagesFromServer::~CImumDeleteMessagesFromServer()
    {
    IMUM_CONTEXT( CImumDeleteMessagesFromServer::~CImumDeleteMessagesFromServer, 0, KImumMtmLog );
    IMUM_IN();

    Cancel();

    // Release the disk space
    iFileSession.ReleaseReserveAccess( iDriveNo );

    delete iOperation;
    delete iEntry;
    delete iEntrySelection;
    delete iProgressText;
    IMUM_OUT();
    }

// -----------------------------------------------------------------------------
// CImumDeleteMessagesFromServer::DoCancel()
// -----------------------------------------------------------------------------
//
void CImumDeleteMessagesFromServer::DoCancel()
    {
    IMUM_CONTEXT( CImumDeleteMessagesFromServer::DoCancel, 0, KImumMtmLog );
    IMUM_IN();

    if (iOperation)
        {
        iOperation->Cancel();
        }
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete(status, iStatus.Int());
    IMUM_OUT();
    }

// -----------------------------------------------------------------------------
// CImumDeleteMessagesFromServer::RunL()
// -----------------------------------------------------------------------------
//
void CImumDeleteMessagesFromServer::RunL()
    {
    IMUM_CONTEXT( CImumDeleteMessagesFromServer::RunL, 0, KImumMtmLog );
    IMUM_IN();

    // Make first a check, that nothing has failed
    if ( iStatus.Int() != KErrNone )
        {
        // something failed, just complete
        TRequestStatus* status = &iObserverRequestStatus;
        User::RequestComplete(status, iStatus.Int());
        return;
        }

    if ( iState == EImumDeletingMessagesStateLocally )
        {
        // local delete completed, start deleting from server
        StartDeleteFromServer();
        }
    else
        {
        // nothing left to process, so complete the observer
        TRequestStatus* status = &iObserverRequestStatus;
        User::RequestComplete(status, iStatus.Int());
        }
    IMUM_OUT();
    }

// -----------------------------------------------------------------------------
// CImumDeleteMessagesFromServer::ProgressL()
// -----------------------------------------------------------------------------
//
const TDesC8& CImumDeleteMessagesFromServer::ProgressL()
    {
    IMUM_CONTEXT( CImumDeleteMessagesFromServer::ProgressL, 0, KImumMtmLog );
    IMUM_IN();

    // Make sure that operation is active
    if ( IsActive() )
        {
        // Get progress
        if ( iOperation )
            {
            return iOperation->ProgressL();
            }
        }
    // Check that we're using IMAP4 protocol
    else if ( iMtm == KImumMtmImap4Uid )
        {
        // Get the progress bar only with IMAP4
        TImap4UiProgress& uiProgress=iUiProgress();
        uiProgress.iBytesToDo=0;
        uiProgress.iBytesDone=0;
        uiProgress.iError= ( iStatus.Int() == KErrServerBusy ? KErrImapServerBusy : iStatus.Int() );
        uiProgress.iUiOperation = TImap4UiProgress::EDeletingMessages;
        IMUM_OUT();
        return iUiProgress;
        }
    IMUM_OUT();

    return iBlank;
    }

// -----------------------------------------------------------------------------
// CImumDeleteMessagesFromServer::DecodeProgress()
// -----------------------------------------------------------------------------
//
TInt CImumDeleteMessagesFromServer::DecodeProgress(
    const TDesC8& aProgress,
    TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString,
    TInt& aTotalEntryCount,
    TInt& aEntriesDone,
    TInt& aCurrentEntrySize,
    TInt& aCurrentBytesTrans,
    TBool /*aInternal*/ )
    {
    IMUM_CONTEXT( CImumDeleteMessagesFromServer::DecodeProgress, 0, KImumMtmLog );
    IMUM_IN();

    aCurrentEntrySize = aTotalEntryCount = iEntryCount;
    if ( iState == EImumDeletingMessagesStateLocally )
        {
        TPckgBuf<TImCacheManagerProgress> paramPack;
        paramPack.Copy(aProgress);
        const TImCacheManagerProgress& progress=paramPack();

        aEntriesDone=progress.iMessagesProcessed;
        aCurrentBytesTrans=progress.iMessagesProcessed;
        }
    else if ( iState == EImumDeletingMessagesStateFromServer )
        {
        if ( iMtm == KImumMtmImap4Uid )
            {
            TPckgBuf<TImap4CompoundProgress> paramPack;
            paramPack.Copy(aProgress);
            const TImap4GenericProgress& progress=paramPack().iGenericProgress;
            aCurrentBytesTrans = aEntriesDone = progress.iMsgsDone == 0 ? 1 :
                progress.iMsgsDone;
            if ( progress.iState == TImap4GenericProgress::EDeleting )
                {
                iDeleteProgress = ETrue;
                }
            else
                {
                if ( iDeleteProgress )
                    {
                    aCurrentBytesTrans = aEntriesDone = aTotalEntryCount;
                    }
                }
            }
        else if ( iMtm == KImumMtmPop3Uid )
            {
            TPckgBuf<TPop3Progress> paramPack;
            paramPack.Copy( aProgress );
            const TPop3Progress& progress = paramPack();
            aCurrentBytesTrans = aEntriesDone =
                ( progress.iMsgsToProcess == progress.iTotalMsgs ) ?
                1 : progress.iTotalMsgs-progress.iMsgsToProcess;
            if ( ( progress.iPop3Progress == TPop3Progress::EPopDeleting )
                && ( progress.iTotalMsgs != progress.iMsgsToProcess ) )
                {
                iDeleteProgress = ETrue;
                }
            else
                {
                if ( iDeleteProgress )
                    {
                    aCurrentBytesTrans = aEntriesDone = aTotalEntryCount;
                    }
                }
            }
        }
    aReturnString.Copy( *iProgressText );
    if ( iEntryCount > 1 )
        {
        TBuf<CBaseMtmUi::EProgressStringMaxLen> tempBuffer;
        StringLoader::Format(
            tempBuffer,
            aReturnString,
            KImumEntriesDoneReplaceIndex,
            aEntriesDone );
        StringLoader::Format(
            aReturnString,
            tempBuffer,
            KImumTotalEntriesReplaceIndex,
            aTotalEntryCount );
        }
    IMUM_OUT();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CImumDeleteMessagesFromServer::StartNextDeleteLocally()
// -----------------------------------------------------------------------------
//
void CImumDeleteMessagesFromServer::StartNextDeleteLocally()
    {
    IMUM_CONTEXT( CImumDeleteMessagesFromServer::StartNextDeleteLocally, 0, KImumMtmLog );
    IMUM_IN();

    if(iOperation)
        {
        delete iOperation;
        iOperation = NULL;
        }
    TRAPD(err, MakeNextDeleteLocallyL());

    // Set active
    iState = EImumDeletingMessagesStateLocally;
    iStatus=KRequestPending;
    SetActive();
    // if error then complete this pass with the error code
    if (err)
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, err);
        }
    IMUM_OUT();
    }

// -----------------------------------------------------------------------------
// CImumDeleteMessagesFromServer::MakeNextDeleteLocallyL()
// -----------------------------------------------------------------------------
//
void CImumDeleteMessagesFromServer::MakeNextDeleteLocallyL()
    {
    IMUM_CONTEXT( CImumDeleteMessagesFromServer::MakeNextDeleteLocallyL, 0, KImumMtmLog );
    IMUM_IN();

    iOperation = CImumDeleteMessagesLocally::NewL(
        *iEntrySelection, iMailboxApi, iStatus, EFalse );
    IMUM_OUT();
    }

// -----------------------------------------------------------------------------
// CImumDeleteMessagesFromServer::StartDeleteFromServer()
// -----------------------------------------------------------------------------
//
void CImumDeleteMessagesFromServer::StartDeleteFromServer()
    {
    IMUM_CONTEXT( CImumDeleteMessagesFromServer::StartDeleteFromServer, 0, KImumMtmLog );
    IMUM_IN();

    if(iOperation)
        {
        delete iOperation;
        iOperation = NULL;
        }
    TRAPD(err, MakeDeleteFromServerL());

    // and set active
    iState = EImumDeletingMessagesStateFromServer;
    iStatus=KRequestPending;
    SetActive();
    // if error then complete this pass with the error code
    if (err)
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, err);
        }
    IMUM_OUT();
    }

// -----------------------------------------------------------------------------
// CImumDeleteMessagesFromServer::MakeDeleteFromServerL()
// -----------------------------------------------------------------------------
//
void CImumDeleteMessagesFromServer::MakeDeleteFromServerL()
    {
    IMUM_CONTEXT( CImumDeleteMessagesFromServer::MakeDeleteFromServerL, 0, KImumMtmLog );
    IMUM_IN();

    iOperation = iEntry->DeleteL( *iEntrySelection, iStatus );
    IMUM_OUT();
    }

//  End of File


