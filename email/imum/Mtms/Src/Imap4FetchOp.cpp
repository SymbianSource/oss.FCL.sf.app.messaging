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
*       Populate a selection of incomplete remote messages
*
*/


// INCLUDES
#include <eikenv.h>
#include <eikrutil.h>
#include <imapcmds.h>
#include <imapset.h>

#include "Imap4FetchOp.h"
#include "EmailMsgSizerOperation.h"
#include "EmailUtils.H"
#include "ImumDisconnectOperation.h"
#include "IMAPPRGR.H"
#include "ImumMtmLogging.h"
#include "ImumPanic.h"
#include "ImapPopulateOperation.h"

// CONSTANTS

// ----------------------------------------------------------------------------
// CImap4FetchOp::NewL()
// ----------------------------------------------------------------------------
//
CImap4FetchOp* CImap4FetchOp::NewL(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aService,
    TInt aFunctionId,
    const TImImap4GetMailInfo& aGetMailInfo,
    const CMsvEntrySelection& aSel )
    {
    IMUM_STATIC_CONTEXT( CImap4FetchOp::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    __ASSERT_DEBUG(aSel.Count(),
        User::Panic(KImumMtmUiPanic,EPanicSelectionEmpty));
    if ( aSel.Count() == 0 )
        {
        User::Leave( KErrNotSupported );
        }
    CImap4FetchOp* op = new(ELeave) CImap4FetchOp(
        aMailboxApi, aObserverRequestStatus, aReporter,
        aService, aFunctionId, aGetMailInfo);
    CleanupStack::PushL(op);
    op->ConstructL(aSel);
    CleanupStack::Pop();
    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
// CImap4FetchOp::NewL()
// ----------------------------------------------------------------------------
//
CImap4FetchOp* CImap4FetchOp::NewL(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aService,
    TInt aFunctionId,
    const TImImap4GetPartialMailInfo& aPartialGetMailInfo,
    const CMsvEntrySelection& aSel )
    {
    IMUM_STATIC_CONTEXT( CImap4FetchOp::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    __ASSERT_DEBUG(aSel.Count(),
        User::Panic(KImumMtmUiPanic,EPanicSelectionEmpty));
    if ( aSel.Count() == 0 )
        {
        User::Leave( KErrNotSupported );
        }
    CImap4FetchOp* op = new(ELeave) CImap4FetchOp(
        aMailboxApi, aObserverRequestStatus, aReporter,
        aService, aFunctionId, aPartialGetMailInfo );
    CleanupStack::PushL(op);
    op->ConstructL(aSel);
    CleanupStack::Pop();
    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
// CImap4FetchOp::~CImap4FetchOp()()
// ----------------------------------------------------------------------------
//
CImap4FetchOp::~CImap4FetchOp()
    {
    IMUM_CONTEXT( CImap4FetchOp::~CImap4FetchOp, 0, KImumMtmLog );
    IMUM_IN();
    
    delete iSelection;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImap4FetchOp::CImap4FetchOp()
// ----------------------------------------------------------------------------
//
CImap4FetchOp::CImap4FetchOp(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aServiceId,
    TInt aFunctionId,
    const TImImap4GetMailInfo& aGetMailInfo )
    :
    CImap4ConnectedOp(aMailboxApi, aStatus, aReporter, aServiceId),
    iFunctionId(aFunctionId),
    iGetMailInfo( aGetMailInfo ),
    iPopulate( EFalse )
    {
    IMUM_CONTEXT( CImap4FetchOp::CImap4FetchOp, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    }

// ----------------------------------------------------------------------------
// CImap4FetchOp::CImap4FetchOp()
// ----------------------------------------------------------------------------
//
CImap4FetchOp::CImap4FetchOp(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aServiceId,
    TInt aFunctionId,
    const TImImap4GetPartialMailInfo& aPartialGetMailInfo )
    :
    CImap4ConnectedOp( aMailboxApi, aStatus, aReporter, aServiceId ),
    iFunctionId(aFunctionId),
    iGetMailInfo( aPartialGetMailInfo ),
    iPartialGetMailInfo( aPartialGetMailInfo ),
    iPopulate( ETrue )
    {
    IMUM_CONTEXT( CImap4FetchOp::CImap4FetchOp, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    }

// ----------------------------------------------------------------------------
// CImap4FetchOp::ConstructL()
// ----------------------------------------------------------------------------
//
void CImap4FetchOp::ConstructL( const CMsvEntrySelection& aSel )
    {
    IMUM_CONTEXT( CImap4FetchOp::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    iSelection = aSel.CopyL();
    iSelection->InsertL(0, iService);
    // For Get Mail API, first selection element must be service.
    CImap4ConnectedOp::ConstructL( iFunctionId );
    if ( iFunctionId != KIMAP4MTMCopyMailSelectionWhenAlreadyConnected &&
         iFunctionId != KIMAP4MTMMoveMailSelectionWhenAlreadyConnected )
        {
        iDisconnect = EFalse;
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImap4FetchOp::DoConnectedOpL()
// ----------------------------------------------------------------------------
//
void CImap4FetchOp::DoConnectedOpL()
    {
    IMUM_CONTEXT( CImap4FetchOp::DoConnectedOpL, 0, KImumMtmLog );
    IMUM_IN();
    
    iStatus = KRequestPending;
    if ( iPopulate )
        {
        delete iOperation;
        iOperation = NULL;
        DoPopulateL();
        }
    else
        {
        TPckg<TImImap4GetMailInfo> param( iGetMailInfo );
        delete iOperation;
        iOperation = NULL;
        InvokeClientMtmAsyncFunctionL(
            iFunctionId, *iSelection, iService, param);
        }
    SetActive();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImap4FetchOp::ConnectedOpErrorProgressL()
// ----------------------------------------------------------------------------
//
const TDesC8& CImap4FetchOp::ConnectedOpErrorProgressL(TInt aError)
    {
    IMUM_CONTEXT( CImap4FetchOp::ConnectedOpErrorProgressL, 0, KImumMtmLog );
    IMUM_IN();
    
    TImap4CompoundProgress& prog = iProgressBuf();
    prog.iGenericProgress.iOperation = TImap4GenericProgress::EPopulate;
    prog.iGenericProgress.iState = TImap4GenericProgress::EFetching;
    prog.iGenericProgress.iErrorCode = aError;
    IMUM_OUT();
    return iProgressBuf;
    }

// ----------------------------------------------------------------------------
// CImap4FetchOp::DoPopulateL()
// ----------------------------------------------------------------------------
//
void CImap4FetchOp::DoPopulateL()
    {
    IMUM_CONTEXT( CImap4FetchOp::DoPopulateL, 0, KImumMtmLog );
    IMUM_IN();
    
    TPckg<TImImap4GetPartialMailInfo> param( iPartialGetMailInfo );

    CMsvEntry* cEntry = iMsvSession.GetEntryL(iService);
    CleanupStack::PushL( cEntry );

    CMsvEntrySelection* selection = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    selection->AppendL( iService );

    TMsvId inbox = FindInboxL( *cEntry );
    selection->AppendL( inbox );

    InvokeClientMtmAsyncFunctionL(
        KIMAP4MTMPopulateNewMailWhenAlreadyConnected,
        //KIMAP4MTMPopulateAllMailWhenAlreadyConnected,
        *selection,
        iService,
        param);

    CleanupStack::PopAndDestroy( 2, cEntry ); // CSI: 47 # selection, centry
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImap4FetchOp::FindInboxL()
// ----------------------------------------------------------------------------
//
TMsvId CImap4FetchOp::FindInboxL( CMsvEntry& aEntry )
    {
    IMUM_CONTEXT( CImap4FetchOp::FindInboxL, 0, KImumMtmLog );
    IMUM_IN();
    
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
    IMUM_OUT();

    return inboxId;
    }


