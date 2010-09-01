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
*       Encapsulates connection validation
*
*/


#include <eikenv.h>

#include "ImumPanic.h"
#include "MsvPop3ConnectOp.h"
#include <ImumUtils.rsg>
#include <miut_err.h>
#include <SendUiConsts.h>
#include <ImumInMailboxUtilities.h>

#include "MsvConnectionValidation.h"
#include "MsvEmailConnectionProgressProvider.h"
#include "EmailUtils.H"
#include "ImumDisconnectOperation.h"
#include "ImumMtmLogging.h"
#include "ImumMboxSettingsUtils.h"

#include <MsvPrgReporter.h>

// Constants and defines
const TInt KConnectOpPriority = CActive::EPriorityStandard;
const TInt KImumMaxLoginRetries = 3;

CMsvPop3ConnectOp* CMsvPop3ConnectOp::NewL(
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aService,
    TInt aPopulateLimit )
    {
    IMUM_STATIC_CONTEXT( CMsvPop3ConnectOp::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();

    // Normally this would be done in ConstructL method, but the base class
    // requires this instance to exist in constructor call. We don't want to
    // use the MTM instance of the emailAPI because this operation can outlive
    // the MTM.
    CImumInternalApi* emailApi = CreateEmailApiLC();

    CMsvPop3ConnectOp* op = new(ELeave) CMsvPop3ConnectOp( emailApi,
        aObserverRequestStatus,
        aReporter,
        aService,
        aPopulateLimit);

    CleanupStack::Pop( emailApi ); // ownership transferred to operation
    CleanupStack::PushL(op);
    op->ConstructL();
    CleanupStack::Pop();
    IMUM_OUT();
    return op;
    }

CMsvPop3ConnectOp::~CMsvPop3ConnectOp()
    {
    IMUM_CONTEXT( CMsvPop3ConnectOp::~CMsvPop3ConnectOp, 0, KImumMtmLog );
    IMUM_IN();

    delete iEntry;
    delete iProgProvider;
    delete iSelection;
    delete iMboxApi;
    IMUM_OUT();
    }

const TDesC8& CMsvPop3ConnectOp::ProgressL()
    {
    IMUM_CONTEXT( CMsvPop3ConnectOp::ProgressL, 0, KImumMtmLog );
    IMUM_IN();

    //
    // If refreshing the inbox, return client op progress, else use connection progress provider.
    //
    if ( iError != KErrNone )
        {
        if ( FlagIsSet( EIapWasInvalid ) )
            {
            iError = KPop3CannotConnect;
            }

        return GetErrorProgressL( iError );
        }
    if ( !iOperation )
        {
        IMUM_OUT();
        return iDummyProg;
        }
    iProgress.Copy( iOperation->ProgressL() );
    if ( iStatus == KRequestPending )
        {
        if ( iProgress().iPop3Progress == TPop3Progress::EPopRefreshing ||
             iProgress().iPop3Progress == TPop3Progress::EPopTopPopulating ||
             iProgress().iPop3Progress == TPop3Progress::EPopPopulating )
            {
            IMUM_OUT();
            // Refreshing inbox or doing populate
            return iProgress;
            }

        // Operation not completed, and not refreshing inbox.
        return iProgProvider->GetConnectionProgressL( iError );
        }
    else if ( FlagIsSet( EMaxNumberOfLoginTries ) )
        {
        // User has entered wrong username or password max number of times, display error note
        iProgress().iErrorCode = KPop3InvalidLogin;
        IMUM_OUT();
        return iProgress;
        }
    else if ( FlagIsSet( EInvalidDetails ) )
        {
        // User details are wrong.
        // This error is handled using a special dialog in DoRunL() which allows retrying of connection.
        // The user must have cancelled that dialog, so patch the error code so we don't display another
        // error notifier in CPop3MtmUi::DisplayProgressSummary().
        // iOperation is a Client MTM connect op, progress is a TPop3Progress.
        iProgress().iErrorCode = KErrCancel;
        IMUM_OUT();
        return iProgress;
        }
    IMUM_OUT();
    // Operation completed.
    return iProgress;
    }

void CMsvPop3ConnectOp::DoCancel()
    {
    IMUM_CONTEXT( CMsvPop3ConnectOp::DoCancel, 0, KImumMtmLog );

    CImumOnlineOperation::DoCancel();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CMsvPop3ConnectOp::DoRunL()
// ----------------------------------------------------------------------------
//
void CMsvPop3ConnectOp::DoRunL()
    {
    IMUM_CONTEXT( CMsvPop3ConnectOp::DoRunL, 0, KImumMtmLog );
    IMUM_IN();

    TInt err  = KErrNone;

    if ( FlagIsSet( EDoConnect ) )
        {
        // To speed up connection establishment, avoid loading settings multiple
        // times. Get all the info needed at once.
        CImumInSettingsData* settings =
            iMailboxApi.MailboxServicesL().LoadMailboxSettingsL( iService );
        CleanupStack::PushL( settings );
        // Get iap name
        TBuf<KImasImailMaxSettingsLength> iapName;
        MsvConnectionValidation::GetServiceIapNameL( *settings,
            KSenduiMtmPop3Uid, iapName);
        TBool ok = ValidateL( *settings );
        CleanupStack::PopAndDestroy(); // settings
        if ( ok )
            {
            // Begin Connect.
            UnsetFlag( EDoConnect );
            iProgProvider = CMsvEmailConnectionProgressProvider::NewL(
                iapName, iService );
            DoConnectL();
            IMUM_OUT();
            return;
            }
        else
            {
            err = KErrCancel;    // User cancelled.
            }
        }
    else
        {
        // Connect completed.
        err = GetOperationCompletionCodeL();
        if ( ( err == KPop3InvalidUser ) ||
             ( err == KPop3InvalidLogin )  ||
             ( err == KPop3InvalidApopLogin ) )
            {
            // Login details are wrong.
            TBool retry = EFalse;
            if ( iLoginRetryCounter < KImumMaxLoginRetries )
                {
                TRAP( err, retry =
                    MsvConnectionValidation::ShowLoginDetailsRejectionDlgL(
                    iMailboxApi, iService ) );
                }
            else
                {
                SetFlag( EMaxNumberOfLoginTries );
                }
            iLoginRetryCounter++;

            if ( retry )
                {
                // Retry connect.
                ReDisplayProgressL(R_EMAIL_CONNECTING_SERVER, iService);
                delete iOperation;
                iOperation = NULL;
                DoConnectL();
                IMUM_OUT();
                return;
                }
            else
                {
                SetFlag( EInvalidDetails );
                }
            }
        else if ( err != KErrNone )
            {
            // We can't do this before, because we want to handle KPop3InvalidUser,
            // KPop3InvalidLogin and KPop3InvalidApopLogin separately.
            User::Leave( err );
            }
        else
            {
            if ( !iPopulated &&
                !FlagIsSet( EDoingPopulate ) &&
                ( iPopulateLimit == -1 || iPopulateLimit > 0 ) )
                {
            	ImumMboxSettingsUtils::SetLastUpdateInfoL(
            	    iMailboxApi,
                	iService,
                	err == KErrNone && !FlagIsSet( EInvalidDetails ) );
                //start populate
                DoPopulateL();
                IMUM_OUT();
                return;
                }
            if ( FlagIsSet( EDoingPopulate ) )
                {
                UnsetFlag( EDoingPopulate );
                }
            }
        /* Background Handling */
        MsvEmailMtmUiUtils::CallNewMessagesL( iService );
        }
    CompleteObserver( err );
    IMUM_OUT();
    }

const TDesC8& CMsvPop3ConnectOp::GetErrorProgressL(TInt aError)
    {
    IMUM_CONTEXT( CMsvPop3ConnectOp::GetErrorProgressL, 0, KImumMtmLog );
    IMUM_IN();

    iError = aError;
    if ( iOperation && iError == KErrNone )
        {
        IMUM_OUT();
        return iOperation->ProgressL();
        }
    TPop3Progress& progress = iProgress();
    progress.iPop3Progress = TPop3Progress::EPopConnecting;
    progress.iTotalMsgs = 0;
    progress.iMsgsToProcess = 0;
    progress.iBytesDone = 0;
    progress.iTotalBytes = 0;
    progress.iErrorCode = iError;
    progress.iPop3SubStateProgress = TPop3Progress::EPopConnecting;
    progress.iServiceId = iService;
    IMUM_OUT();
    return iProgress;
    }

CMsvPop3ConnectOp::CMsvPop3ConnectOp(
    CImumInternalApi* aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aServiceId,
    TInt aPopulateLimit)
    :
    CImumOnlineOperation(
    *aMailboxApi,
    KConnectOpPriority,
    aObserverRequestStatus,
    aReporter)
    {
    IMUM_CONTEXT( CMsvPop3ConnectOp::CMsvPop3ConnectOp, 0, KImumMtmLog );
    IMUM_IN();
    iMboxApi = aMailboxApi;
    iService = aServiceId;
    iPopulateLimit = aPopulateLimit;
    IMUM_OUT();
    }

void CMsvPop3ConnectOp::ConstructL()
    {
    IMUM_CONTEXT( CMsvPop3ConnectOp::ConstructL, 0, KImumMtmLog );
    IMUM_IN();

    iEntry = iMsvSession.GetEntryL(iService);
    iSelection = new(ELeave) CMsvEntrySelection;
    const TMsvEntry& tentry = iEntry->Entry();
    __ASSERT_ALWAYS( tentry.iType.iUid == KUidMsvServiceEntryValue,
        User::Panic(KImumMtmUiPanic,EPop3MtmUiOpConnectNotAService) );
    BaseConstructL(KUidMsgTypePOP3);

    TBool ok = ETrue;
    if(tentry.Connected())
        {
        // We are already connected.
        ok = EFalse;

        //if connected, we must have populated already
        iPopulated = ETrue;
        }
    if(ok)
        {
        // User did not cancel connection, and we are not connected.
        SetFlag(EDoConnect);
        }
    SetActive();
    CompleteThis();
    IMUM_OUT();
    }

void CMsvPop3ConnectOp::DoConnectL()
    {
    IMUM_CONTEXT( CMsvPop3ConnectOp::DoConnectL, 0, KImumMtmLog );
    IMUM_IN();

    // Begin connect.
    iStatus = KRequestPending;
    InvokeClientMtmAsyncFunctionL(KPOP3MTMConnect, iService, iService);
    SetActive();
    }

void CMsvPop3ConnectOp::DoPopulateL()
    {
    IMUM_CONTEXT( CMsvPop3ConnectOp::DoPopulateL, 0, KImumMtmLog );
    IMUM_IN();

    // Populate only if it hasn't done yet
    if( !iPopulated )
        {
        iStatus = KRequestPending;

        // Prepare parameters and include filtering
        TImPop3PopulateOptions pop3GetMailInfo;
        pop3GetMailInfo.SetMaxEmailSize( KMaxTInt32 );
        pop3GetMailInfo.SetPopulationLimit( iPopulateLimit );
        TPckg<TImPop3PopulateOptions> params(pop3GetMailInfo);

        iSelection->InsertL(0, iService);

        // Start the fetch operation
        InvokeClientMtmAsyncFunctionL(
            KPOP3MTMPopulateNew,
            *iSelection,
            iService,
            params);
        SetFlag(EDoingPopulate);

        // Make sure that this won't be done too many times
        iPopulated = ETrue;
        }
    SetActive();
    IMUM_OUT();
    }


TBool CMsvPop3ConnectOp::ValidateL( CImumInSettingsData& aSettings )
    {
    IMUM_CONTEXT( CMsvPop3ConnectOp::ValidateL, 0, KImumMtmLog );
    IMUM_IN();

    iLoginRetryCounter = 0;
    TBool ok = ( aSettings.Validate() == KErrNone );
    if ( !ok )
        {
        SetFlag( EIapWasInvalid );
        }
    else
        {
        UnsetFlag( EIapWasInvalid );
        }

    if(ok)
        {
        iReporter.MakeProgressVisibleL(ETrue);
        TMsvLoginData loginData;
        ImumMboxSettingsUtils::GetLoginInformationL( aSettings,
            KSenduiMtmImap4Uid, loginData.iUsername, loginData.iPassword,
            loginData.iIsPasswordTemporary );
        ok = MsvConnectionValidation::CheckAndPromptForPasswordL(
                iMailboxApi, iService, loginData, iLoginRetryCounter );
        }

    IMUM_OUT();
    return ok;
    }
