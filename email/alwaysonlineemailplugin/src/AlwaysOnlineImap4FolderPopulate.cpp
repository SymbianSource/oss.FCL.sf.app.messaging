/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     Populates imap4 folders using current populate settings.
*
*/


#include <impcmtm.h> // CImap4ClientMtm
#include <SenduiMtmUids.h>
#include "AlwaysOnlineEmailPluginLogging.h"
#include "AlwaysOnlineEmailLoggingTools.h"
#include "AlwaysOnlineImap4FolderPopulate.h"



//--------------------------------
// CAlwaysOnlineImap4Agent()
//--------------------------------
CAlwaysOnlineImap4FolderPopulate::CAlwaysOnlineImap4FolderPopulate(
    CMsvSession& aMsvSession, 
    CImap4ClientMtm& aImap4ClientMtm,
    TMsvId aServiceId,
    TMsvId aInboxId,
    TImImap4GetPartialMailInfo& aPartialInfo,
    TRequestStatus& aObserverRequestStatus )
    :
    CMsvOperation(
        aMsvSession, 
        CActive::EPriorityStandard, 
        aObserverRequestStatus), 
    iServiceId( aServiceId ),
    iPartialInfo( aPartialInfo),
    iImap4ClientMtm( aImap4ClientMtm ),
    iInboxId( aInboxId )
    {
    CActiveScheduler::Add(this);
    iMtm = KSenduiMtmImap4Uid;
    }

//--------------------------------
// NewL
//--------------------------------
CAlwaysOnlineImap4FolderPopulate* CAlwaysOnlineImap4FolderPopulate::NewL(
    CMsvSession& aMsvSession, 
    CImap4ClientMtm& aImap4ClientMtm,
    TMsvId aServiceId,
    TMsvId aInboxId,
    TImImap4GetPartialMailInfo& aPartialInfo,
    const CMsvEntrySelection* aSelection, // folder ids
    TRequestStatus& aObserverRequestStatus )
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderPopulate::NewL" );
    CAlwaysOnlineImap4FolderPopulate* self=new (ELeave) CAlwaysOnlineImap4FolderPopulate(
        aMsvSession, 
        aImap4ClientMtm,
        aServiceId,
        aInboxId,
        aPartialInfo,
        aObserverRequestStatus );
    CleanupStack::PushL(self);
    self->ConstructL(aSelection);
    CleanupStack::Pop(self);
    return self;
    }


//--------------------------------
// ConstructL()
//--------------------------------

void CAlwaysOnlineImap4FolderPopulate::ConstructL( const CMsvEntrySelection* aSelection)
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderPopulate::ConstructL" );
    iFolderSelection = aSelection->CopyL();

    // if selection is empty, following just completes 
    StartNextFolderPopulateOperation();
    }


//--------------------------------
// ~CAlwaysOnlineImap4Agent()
//--------------------------------

CAlwaysOnlineImap4FolderPopulate::~CAlwaysOnlineImap4FolderPopulate()
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderPopulate::~CAlwaysOnlineImap4FolderPopulate" );
    Cancel();
    delete iOperation;
    delete iFolderSelection;
    }

// ----------------------------------------------------
// CAlwaysOnlineImap4FolderPopulate::StartNextFolderPopulateOperation
// ----------------------------------------------------
void CAlwaysOnlineImap4FolderPopulate::StartNextFolderPopulateOperation()
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderPopulate::StartNextFolderPopulateOperation" );
    delete iOperation;
    iOperation = NULL;
    TInt err = KErrNotFound; // if folder selection is empty, operation is completed with this error
    if ( iFolderSelection->Count() )
        {
        TRAP(err, DoNewFolderPopulateOperationL());
        // whether error or not, remove the data for this operation
        iFolderSelection->Delete(0);
        }
    // and set active
    iStatus=KRequestPending;
    SetActive();
    // if error, then complete this pass with the error code
    if (err)
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, err);
        }
    }

// ----------------------------------------------------
// CAlwaysOnlineImap4FolderPopulate::DoNewFolderPopulateOperationL
// ----------------------------------------------------
void CAlwaysOnlineImap4FolderPopulate::DoNewFolderPopulateOperationL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderPopulate::DoNewFolderPopulateOperationL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineImap4FolderPopulate::DoNewFolderPopulateOperationL");
    if ( HasFolderNewMailL( (*iFolderSelection)[0] ) )
        {
        // here do folder populate operation
        CMsvEntrySelection* selection=new(ELeave) CMsvEntrySelection;
        CleanupStack::PushL(selection);
        selection->AppendL( iServiceId );
        selection->AppendL( (*iFolderSelection)[0] );

        TPckg<TImImap4GetPartialMailInfo> param( iPartialInfo );

        iOperation = iImap4ClientMtm.InvokeAsyncFunctionL(
            KIMAP4MTMPopulateNewMailWhenAlreadyConnected,
            *selection, 
            param, 
            iStatus);
        KAOEMAIL_LOGGER_WRITE_FORMAT("Started populate folder 0x%x", (*selection)[1]); // CSI: 2 # 2 items added above

        CleanupStack::PopAndDestroy( selection );
        }
    else
        {
        // no new mail found, just complete
        KAOEMAIL_LOGGER_WRITE_FORMAT("No new mail found if folder 0x%x, just complete", (*iFolderSelection)[0]);
        iOperation = CMsvCompletedOperation::NewL(
            iMsvSession, 
            KSenduiMtmImap4Uid, 
            KNullDesC8, 
            KMsvLocalServiceIndexEntryId, 
            iStatus, 
            KErrCancel);
        }
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4FolderPopulate::DoNewFolderPopulateOperationL");
    }

// ----------------------------------------------------
// CAlwaysOnlineImap4FolderPopulate::DoCancel
// ----------------------------------------------------
void CAlwaysOnlineImap4FolderPopulate::DoCancel()
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderPopulate::DoCancel" );
    if (iOperation)
        {
        iOperation->Cancel();
        }
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete(status, iStatus.Int());
    }

// ----------------------------------------------------
// CAlwaysOnlineImap4FolderPopulate::RunL
// ----------------------------------------------------
void CAlwaysOnlineImap4FolderPopulate::RunL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderPopulate::RunL" );
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4FolderPopulate::RunL iStatus %d", iStatus.Int() );
    if (iFolderSelection->Count()==0)
        {
        // nothing left to process, so complete the observer
        TRequestStatus* status = &iObserverRequestStatus;
        User::RequestComplete(status, iStatus.Int());
        }
    else
        {
        // start the next pass
        StartNextFolderPopulateOperation();
        }
    }

// ----------------------------------------------------
// CAlwaysOnlineImap4FolderPopulate::ProgressL
// ----------------------------------------------------
const TDesC8& CAlwaysOnlineImap4FolderPopulate::ProgressL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderPopulate::ProgressL" );
    if (iOperation)
        {
        return iOperation->ProgressL();
        }
    return KNullDesC8();
    }

// ----------------------------------------------------
// CAlwaysOnlineImap4FolderPopulate::SelectInbox
// ----------------------------------------------------
void CAlwaysOnlineImap4FolderPopulate::SelectInbox()
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderPopulate::SelectInbox" );
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4FolderPopulate::SelectInbox");
    delete iOperation;
    iOperation = NULL;
    TRAPD(err, DoSelectInboxL());
    // and set active
    iStatus=KRequestPending;
    SetActive();
    // if error, then complete this pass with the error code
    if (err)
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, err);
        }
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4FolderPopulate::SelectInbox");
    }

// ----------------------------------------------------
// CAlwaysOnlineImap4FolderPopulate::DoSelectInboxL
// ----------------------------------------------------
void CAlwaysOnlineImap4FolderPopulate::DoSelectInboxL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderPopulate::DoSelectInboxL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineImap4FolderPopulate::DoSelectInboxL");
    // here do folder populate operation
    CMsvEntrySelection* selection=new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL(selection);
    selection->AppendL( iInboxId );

    TBuf8<1> dummyParams;
    dummyParams.Zero();

    iOperation = iImap4ClientMtm.InvokeAsyncFunctionL(
        //KIMAP4MTMPopulateAllMailWhenAlreadyConnected, 
        KIMAP4MTMInboxNewSync,
        //KIMAP4MTMSelect,
        *selection, 
        dummyParams, 
        iStatus);
    KAOEMAIL_LOGGER_WRITE_FORMAT("Started inbox select 0x%x", iInboxId);

    iInboxId = -1;

    CleanupStack::PopAndDestroy( selection );
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4FolderPopulate::DoSelectInboxL");
    }

// ----------------------------------------------------
// CAlwaysOnlineImap4FolderPopulate::HasFolderNewMailL
// ----------------------------------------------------
TBool CAlwaysOnlineImap4FolderPopulate::HasFolderNewMailL( TMsvId aFolderId )
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderPopulate::HasFolderNewMailL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineImap4FolderPopulate::HasFolderNewMailL");
    TInt count = 0; // number of NEW mails in this folder
    CMsvEntry* folderEntry = iMsvSession.GetEntryL( aFolderId );
    CleanupStack::PushL( folderEntry );
    const TInt entryCount = folderEntry->Count();
    TInt i=0;
    for (i=0; i < entryCount && count == 0 ; i++)
        {
        if ( (*folderEntry)[i].New() )
            {
            count++;
            }
        }
    CleanupStack::PopAndDestroy( folderEntry );
    KAOEMAIL_LOGGER_WRITE_FORMAT("New mail count %d", count);
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4FolderPopulate::HasFolderNewMailL");
    return (count>0);
    }

//EOF
