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


#include <e32base.h>
#include <miut_err.h>
#include "SmtpCopyMoveOp.h"
#include "MsvEmailConnectionProgressProvider.h"
#include "ImumPanic.h"
#include "ImumMtmLogging.h"
#include "EmailUtils.H"
#include "MsvConnectionValidation.h"
#include "ImumMboxSettingsUtils.h"
#include "ImumInMailboxUtilities.h"
#include "SendUiConsts.h"

const TInt KCopyMoveOpPriority = CActive::EPriorityStandard;
const TInt KImumMaxLoginRetries = 3;


// ----------------------------------------------------------------------------
// CSmtpCopyMoveOp::NewL()
// ----------------------------------------------------------------------------
//
CSmtpCopyMoveOp* CSmtpCopyMoveOp::NewL(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aTarget,
    const CMsvEntrySelection& aSel,
    TBool aMoving )
    {
    IMUM_STATIC_CONTEXT( CSmtpCopyMoveOp::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();

    CSmtpCopyMoveOp* op = new ( ELeave ) CSmtpCopyMoveOp(
        aMailboxApi, aObserverRequestStatus, aReporter, aTarget, aMoving );
    CleanupStack::PushL( op );
    op->ConstructL( aSel );
    CleanupStack::Pop( op );
    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
// CSmtpCopyMoveOp::~CSmtpCopyMoveOp()
// ----------------------------------------------------------------------------
//
CSmtpCopyMoveOp::~CSmtpCopyMoveOp()
    {
    IMUM_CONTEXT( CSmtpCopyMoveOp::~CSmtpCopyMoveOp, 0, KImumMtmLog );
    IMUM_IN();

    delete iEntry;
    delete iSelection;
    delete iProgProvider;
    IMUM_OUT();
    }


// ----------------------------------------------------------------------------
// CSmtpCopyMoveOp::ProgressL()
// ----------------------------------------------------------------------------
//
const TDesC8& CSmtpCopyMoveOp::ProgressL()
    {
    IMUM_CONTEXT( CSmtpCopyMoveOp::ProgressL, 0, KImumMtmLog );
    IMUM_IN();

    if(!iOperation)
        {
        if ( iValidIap )
            {
            IMUM_OUT();
            return iDummyProg;
            }
        else
            {
            TImSmtpProgress& progress = iProgress();
            progress.SetError( KSmtpUnknownErr );
            IMUM_OUT();
            return iProgress;
            }
        }
    TRAPD( err, iProgress.Copy( iOperation->ProgressL() ) );
    // If message is already being sent, ProgressL will leave with KErrNotReady
    // Ignore this error, otherwise progress bar of the second sending
    // will be missing 'Connecting to mailbox...' texts.
    if ( err != KErrNotReady && err != KErrNone )
    	{
		User::Leave( err );
		}
    if ( ( iStatus == KRequestPending ) &&
         ( iProgress().Status() != EMsgOutboxProgressSending ) )
        {
        IMUM_OUT();
        return iProgProvider->GetConnectionProgressL(iError);
        }

    IMUM_OUT();
    return iProgress;
    }


// ----------------------------------------------------------------------------
// CSmtpCopyMoveOp::GetErrorProgressL()
// ----------------------------------------------------------------------------
//
const TDesC8& CSmtpCopyMoveOp::GetErrorProgressL(TInt /*aError*/)
    {
    IMUM_CONTEXT( CSmtpCopyMoveOp::GetErrorProgressL, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();

    // Never called.
    return iProgress;
    }


// ----------------------------------------------------------------------------
// CSmtpCopyMoveOp::DoRunL()
// ----------------------------------------------------------------------------
//
void CSmtpCopyMoveOp::DoRunL()
    {
    IMUM_CONTEXT( CSmtpCopyMoveOp::DoRunL, 0, KImumMtmLog );
    IMUM_IN();

    if ( ContinueSendingL() )
        {
        InitiateSendingOperationL();
        }
    else
        {
        CompleteObserver( iValidIap ? KErrNone : KSmtpUnknownErr );
        }
    IMUM_OUT();
    }


// ----------------------------------------------------------------------------
// CSmtpCopyMoveOp::CSmtpCopyMoveOp()
// ----------------------------------------------------------------------------
//
CSmtpCopyMoveOp::CSmtpCopyMoveOp(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aTarget,
    TBool aMoving)
    :
    CImumOnlineOperation(
        aMailboxApi, KCopyMoveOpPriority, aObserverRequestStatus, aReporter ),
    iTarget(aTarget),
    iMoving(aMoving),
    iValidIap( ETrue ), // valid until otherwise stated
    iLoginRetryCounter( 0 )
    {
    IMUM_CONTEXT( CSmtpCopyMoveOp::CSmtpCopyMoveOp, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();

    }


// ----------------------------------------------------------------------------
// CSmtpCopyMoveOp::ConstructL()
// ----------------------------------------------------------------------------
//
void CSmtpCopyMoveOp::ConstructL(
    const CMsvEntrySelection& aSel )
    {
    IMUM_CONTEXT( CSmtpCopyMoveOp::ConstructL, 0, KImumMtmLog );
    IMUM_IN();

    __ASSERT_ALWAYS( aSel.Count(),
        User::Panic( KImumMtmUiPanic,ESmtpMtmUiEmptySelection ) );
    BaseConstructL( KUidMsgTypeSMTP );

    iEntry = iMsvSession.GetEntryL(aSel[0]);
    iService = iEntry->Entry().iServiceId;
    const TMsvId parent = iEntry->Entry().Parent();

    // To speed up connection establishment, avoid loading settings multiple
    // times. Get all the info needed at once.
    CImumInSettingsData* settings =
        iMailboxApi.MailboxServicesL().LoadMailboxSettingsL( iService );
    CleanupStack::PushL( settings );

    // Get iap name
    TBuf<KImasImailMaxSettingsLength> iapName;
    MsvConnectionValidation::GetServiceIapNameL( *settings,
        KSenduiMtmSmtpUid, iapName);

    TBool ok = ValidateL( *settings );
    CleanupStack::PopAndDestroy(); // settings

    if ( !ok )
        {
        // Will be unable to connect. Complete now.
        SetActive();
        CompleteThis();
        IMUM_OUT();
        return;
        }

    iSelection = aSel.CopyL();

    iProgProvider = CMsvEmailConnectionProgressProvider::NewL( iapName,
        iService );

    iEntry->SetEntryL(parent);
    InitiateSendingOperationL();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CSmtpCopyMoveOp::ValidateL()
// ----------------------------------------------------------------------------
//
TBool CSmtpCopyMoveOp::ValidateL( CImumInSettingsData& aSettings )
    {
    IMUM_CONTEXT( CSmtpCopyMoveOp::ValidateL, 0, KImumMtmLog );
    IMUM_IN();

    TBool ok = ETrue;
    iEntry->SetEntryL(iService);
    if(!iEntry->Entry().Connected())
        {
        ok = iValidIap = ( aSettings.Validate() == KErrNone );
        }
    IMUM_OUT();
    return ok;
    }

// ----------------------------------------------------------------------------
// CSmtpCopyMoveOp::InitiateSendingOperationL()
// ----------------------------------------------------------------------------
//
void CSmtpCopyMoveOp::InitiateSendingOperationL()
    {
    IMUM_CONTEXT( CSmtpCopyMoveOp::InitiateSendingOperationL, 0, KImumMtmLog );
    IMUM_IN();

    if ( iOperation )
        {
        delete iOperation;
        iOperation = NULL;
        }

    iOperation = MsvEmailMtmUiUtils::DoCEntryCopyMoveL(
        *iEntry, *iSelection, iTarget, iStatus, !iMoving );

    iStatus = KRequestPending;
    SetActive();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CSmtpCopyMoveOp::GetProgressErrorL()
// ----------------------------------------------------------------------------
//
TInt CSmtpCopyMoveOp::GetProgressErrorL()
    {
    IMUM_CONTEXT( CSmtpCopyMoveOp::GetProgressErrorL, 0, KImumMtmLog );
    IMUM_IN();

    const TPckgBuf<TImSmtpProgress>& progress =
        static_cast<const TPckgBuf<TImSmtpProgress>&>( ProgressL() );
    IMUM_OUT();
    return progress().Error();
    }

// ----------------------------------------------------------------------------
// CSmtpCopyMoveOp::ContinueSendingL()
// ----------------------------------------------------------------------------
//
TBool CSmtpCopyMoveOp::ContinueSendingL()
    {
    IMUM_CONTEXT( CSmtpCopyMoveOp::ContinueSendingL, 0, KImumMtmLog );
    IMUM_IN();

    TInt error = GetProgressErrorL();

    TBool continueSending =
        !( error != KSmtpLoginRefused && error != KSmtpUnknownErr );

    // If login is refused we can try again
    if ( continueSending )
        {
        continueSending = ( iLoginRetryCounter++ < KImumMaxLoginRetries );
        }

    if ( continueSending )
        {
        continueSending =
            MsvConnectionValidation::ShowLoginDetailsRejectionDlgL(
                iMailboxApi, iService );
        }
    IMUM_OUT();

    return continueSending;
    }



