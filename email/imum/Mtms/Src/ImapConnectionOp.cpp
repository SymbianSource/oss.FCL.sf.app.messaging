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
*       CImapConnectionOp implementation file
*
*/



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
#include <MsvPrgReporter.h>
#include <imapcmds.h>
#include <ImumUtils.rsg>
#include <AlwaysOnlineManagerClient.h>
#include <SendUiConsts.h>
#include <ImumInMailboxUtilities.h>

#include "ImapConnectionOp.h"
#include "MsvConnectionValidation.h"
#include "MsvEmailConnectionProgressProvider.h"
#include "PROGTYPE.H"
#include "ImumPanic.h"
#include "ImumMtmLogging.h"
#include "ImapFolderSyncOperation.h"
#include "EmailUtils.H"
#include "ImumMboxSettingsUtils.h"


const TInt KImumMaxLoginRetries = 3;

// ----------------------------------------------------------------------------
// CImapConnectionOp::NewL()
// ----------------------------------------------------------------------------
CImapConnectionOp* CImapConnectionOp::NewL(
    CMsvSession& aMsvSession,
    TInt aPriority,
    TRequestStatus& aStatus,
    TMsvId aService,
    MMsvProgressReporter& aProgressReporter,
    TImapConnectionType aConnectionType)
    {
    IMUM_STATIC_CONTEXT( CImapConnectionOp::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CImapConnectionOp* me=new (ELeave) CImapConnectionOp(
        aMsvSession, aPriority, aStatus,
        aService, aProgressReporter, aConnectionType );
    CleanupStack::PushL(me);
    me->ConstructL();
    CleanupStack::Pop();    //  me
    IMUM_OUT();
    return me;
    }

// ----------------------------------------------------------------------------
// CImapConnectionOp::CImapConnectionOp()
// ----------------------------------------------------------------------------
CImapConnectionOp::CImapConnectionOp(
    CMsvSession& aMsvSession,
    TInt aPriority,
    TRequestStatus& aStatus,
    TMsvId aService,
    MMsvProgressReporter& aProgressReporter,
    TImapConnectionType aConnectionType )
    :
    CMsvOperation(aMsvSession, aPriority, aStatus),
    iMailboxApi( NULL ),
    iState(EConstructing),
    iConnectionType(aConnectionType),
    iSyncProgress(),
    iProgressReporter(aProgressReporter)/*,
    iOperations( KImumOperationsArrayGranularity )*/
    {
    IMUM_CONTEXT( CImapConnectionOp::CImapConnectionOp, 0, KImumMtmLog );
    IMUM_IN();
    
    iService=aService;
    iMtm=KUidMsgTypeIMAP4;
    iSyncProgress().iType=EImap4SyncProgressType;
    iCoeEnv = CCoeEnv::Static();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImapConnectionOp::~CImapConnectionOp()
// ----------------------------------------------------------------------------
CImapConnectionOp::~CImapConnectionOp()
    {
    IMUM_CONTEXT( CImapConnectionOp::~CImapConnectionOp, 0, KImumMtmLog );
    IMUM_IN();
    
    //iOperations.ResetAndDestroy();

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
    delete iConnectionText;
    
    delete iMailboxApi;
    iMailboxApi = NULL;
    // don't own iCoeEnv so don't delete it
    iMtmUi = NULL;
    iCoeEnv = NULL;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImapConnectionOp::ConstructL()
// ----------------------------------------------------------------------------
void CImapConnectionOp::ConstructL()
    {
    IMUM_CONTEXT( CImapConnectionOp::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    CMsvEntry* serviceEntry=NULL;

    serviceEntry=iMsvSession.GetEntryL(iService);
    CleanupStack::PushL(serviceEntry);

    iSyncProgress().iState=TImap4SyncProgress::EConnecting;
    iSyncProgress().iErrorCode=KErrNone;

    //  Store the related service Id
    iRelatedServiceId=serviceEntry->Entry().iRelatedId;

    iConnectionText = NULL;
    iTitleText = StringLoader::LoadL(
        R_EMAIL_CONNECTING_SERVER,
        serviceEntry->Entry().iDetails,
        iCoeEnv );

    CleanupStack::PopAndDestroy( serviceEntry );
    
    iMailboxApi = CreateEmailApiL( &iMsvSession );

    CActiveScheduler::Add(this);

    iObserverRequestStatus=KRequestPending;

    TRequestStatus* myStatus=&iStatus;
    User::RequestComplete(myStatus,KErrNone);

    SetActive();
    //  We complete ourselves so that the dialog gets a chance to redraw
    //  before entering the lengthy process of opening system agent and
    //  connection progress provider
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImapConnectionOp::GetConnectionOperationL()
// ----------------------------------------------------------------------------
CMsvOperation* CImapConnectionOp::GetConnectionOperationL()
    {
    IMUM_CONTEXT( CImapConnectionOp::GetConnectionOperationL, 0, KImumMtmLog );
    IMUM_IN();
    
    //  Invoke the appropriate client connection operation
    //  InvokeAsyncFunctionL in the MtmUi.
    //
    TPckg<MMsvImapConnectionObserver*> param(this);

    CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
    CleanupStack::PushL(selection);
    selection->AppendL(iService);

    CMsvOperation* connectOp=NULL;

    switch(iConnectionType)
        {
    case ECompleteAfterConnect:
        connectOp=iMtmUi->InvokeAsyncFunctionL(
            KIMAP4MTMConnect, *selection, iStatus, param);
        break;
    case EWithSyncCompleteAfterConnect:
        connectOp=iMtmUi->InvokeAsyncFunctionL(
            KIMAP4MTMConnectAndSyncCompleteAfterConnect, *selection, iStatus, param);
        break;
    case EWithSyncCompleteAfterFullSync:
        connectOp=iMtmUi->InvokeAsyncFunctionL(
            KIMAP4MTMConnectAndSyncCompleteAfterFullSync, *selection, iStatus, param);
        break;
    case EWithSyncCompleteAfterDisconnection:
        connectOp=iMtmUi->InvokeAsyncFunctionL(
            KIMAP4MTMConnectAndSyncCompleteAfterDisconnect, *selection, iStatus, param);
        break;
    default:
        __ASSERT_DEBUG( 0,
            User::Panic(KImumMtmUiPanic, EIMAP4MtmUiUnknownOperation) );
        }

    CleanupStack::PopAndDestroy();  //  selection
    IMUM_OUT();

    return connectOp;
    }

// ----------------------------------------------------------------------------
// CImapConnectionOp::RunL()
// ----------------------------------------------------------------------------
void CImapConnectionOp::RunL()
    {
    IMUM_CONTEXT( CImapConnectionOp::RunL, 0, KImumMtmLog );
    IMUM_IN();
    
    DoRunL();

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImapConnectionOp::DoRunL()
// ----------------------------------------------------------------------------
void CImapConnectionOp::DoRunL()
    {
    IMUM_CONTEXT( CImapConnectionOp::DoRunL, 0, KImumMtmLog );
    IMUM_IN();
    
    if(iState==EConstructing)
        {
        FinishConstructionL();
        }
    else
        {
        ConnectionCompletionL();
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImapConnectionOp::RunError()
// ----------------------------------------------------------------------------
TInt CImapConnectionOp::RunError(TInt aError)
	{
    IMUM_CONTEXT( CImapConnectionOp::RunError, 0, KImumMtmLog );
    IMUM_IN();

	IMUM1( 0, "RunL leaved with error code %d", aError );

    iSyncProgress().iErrorCode = aError;
    Complete();

	IMUM_OUT();	
	return KErrNone;		
	}

// ----------------------------------------------------------------------------
// CImapConnectionOp::FinishConstructionL()
// ----------------------------------------------------------------------------
void CImapConnectionOp::FinishConstructionL()
    {
    IMUM_CONTEXT( CImapConnectionOp::FinishConstructionL, 0, KImumMtmLog );
    IMUM_IN();
    
    //  Called by RunL when in EConstructing state
    TBool check = ETrue;

    // To speed up connection establishment, avoid loading settings multiple
    // times. Get all the info needed at once.
    CImumInSettingsData* settings =
        iMailboxApi->MailboxServicesL().LoadMailboxSettingsL( iService );
    CleanupStack::PushL( settings );

    // Check if the service has a password and valid IAP
    // Get login data from settings
    TMsvLoginData loginData;
    ImumMboxSettingsUtils::GetLoginInformationL( *settings,
        KSenduiMtmImap4Uid, loginData.iUsername, loginData.iPassword,
        loginData.iIsPasswordTemporary );
    // Get iap name
    TBuf<KImasImailMaxSettingsLength> iapName;
    MsvConnectionValidation::GetServiceIapNameL( *settings,
        KSenduiMtmImap4Uid, iapName);
    TBool iapOk = ( settings->Validate() == KErrNone );
    CleanupStack::PopAndDestroy(); // settings

    iLoginRetryCounter = 0;
    if( !check ||
        !iapOk ||
        !MsvConnectionValidation::CheckAndPromptForPasswordL(
            *iMailboxApi, iService, loginData, iLoginRetryCounter ) )
        {
        //  User has declined to provide a password or a IAP. Complete
        //  _ourselves_ immediately.
        iStatus=KRequestPending;
        SetActive();

        TRequestStatus* myStatus=&iStatus;
        User::RequestComplete(myStatus, KErrNone);
        iSyncProgress().iErrorCode= ( iapOk ? KErrCancel : KErrImapConnectFail );
        iState=ECompleted;
        }
    else
        {
        //  Service has password in it and a valid IAP.
        //
        iMtmUi=&(iProgressReporter.MtmStore().ClaimMtmUiL(iMtm));
        iMtmUi->BaseMtm().SwitchCurrentEntryL(iService);

        iConnectionProgressProvider = CMsvEmailConnectionProgressProvider::NewL(
            iapName, iService);
        iSubOperation=GetConnectionOperationL();
        iState=EEstablishingConnection;

        SetActive();
        //
        //  We're going to try and connect; tell our MMsvProgressReporter
        //  to come to us for progress.
        //
        iProgressReporter.SetProgressDecoder(*this);
        }
    iObserverRequestStatus=KRequestPending;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImapConnectionOp::ConnectionCompletionL()
// ----------------------------------------------------------------------------
void CImapConnectionOp::ConnectionCompletionL()
    {
    IMUM_CONTEXT( CImapConnectionOp::ConnectionCompletionL, 0, KImumMtmLog );
    IMUM_IN();
    
    //  Called by RunL when past EConstructing state
    TInt err=KErrNone;
    if(iSubOperation)
        {
        //  The sub connection operation has completed.

        //  We need to detect a failure to connect due to invalid login details,
        //  and prompt the user accordingly.
        //
        TBuf<CBaseMtmUi::EProgressStringMaxLen> dummyString;
        TInt dummyInt[4]; // CSI: 47 # For dummyInt.

        err=DecodeProgress(iSubOperation->ProgressL(),dummyString,dummyInt[0],dummyInt[1],dummyInt[2],dummyInt[3],ETrue); // CSI: 47 # dummyInts.
        if(err == KErrImapBadLogon)
            {
            // Login details are wrong.
            TBool retry = EFalse;
            if ( iLoginRetryCounter < KImumMaxLoginRetries )
                {
                retry = MsvConnectionValidation::ShowLoginDetailsRejectionDlgL(
                    *iMailboxApi, iService);
                }
            else
                {
                // User has entered wrong username or password max number of times,
                // sets error code for imap mtm ui to display correct note...
                iSyncProgress().iErrorCode = KErrImapBadLogon;
                iState=ECompleted;
                }
            iLoginRetryCounter++;

            if(retry)
                {
                // Retry connect.
                iProgressReporter.MakeProgressVisibleL(ETrue);
                iProgressReporter.SetTitleL(*iTitleText);
                delete iSubOperation;
                iSubOperation=NULL;
                iSubOperation=GetConnectionOperationL();
                SetActive();
                iProgressReporter.SetProgressDecoder(*this);
                IMUM_OUT();
                return;
                }
            else
                {
                Complete();
                IMUM_OUT();
                return;
                }
            }
        }

    User::LeaveIfError(err);    //  Any error apart from KErrImapBadLogon

    Complete();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImapConnectionOp::DoCancel()
// ----------------------------------------------------------------------------
void CImapConnectionOp::DoCancel()
    {
    IMUM_CONTEXT( CImapConnectionOp::DoCancel, 0, KImumMtmLog );
    IMUM_IN();
    
    if( iSubOperation )
        {
        iSubOperation->Cancel();
        }

    iSyncProgress().iErrorCode = KErrCancel;
    Complete();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImapConnectionOp::ProgressL()
// ----------------------------------------------------------------------------
const TDesC8& CImapConnectionOp::ProgressL()
    {
    IMUM_CONTEXT( CImapConnectionOp::ProgressL, 0, KImumMtmLog );
    IMUM_IN();
    
    if(iSubOperation && (iState!=ECompleted))
        {
        //  Need to make the distinction between connection states.
        //
        //  EEstablishingConnection ->  CMsvEMailConnectionProgressProvider
        //  EUpdatingFolderList     ->  iOperation->ProgressL
        //  EUpdatingInbox,         ->  iOperation->ProgressL
        //  EUpdatingFolders,       ->  iOperation->ProgressL
        //  EOnline,                ->  iOperation->ProgressL
        //  ECompleted              ->  iSyncProgress
        //
        if(iState==EEstablishingConnection)
            {
            IMUM_OUT();
            return iConnectionProgressProvider->GetConnectionProgressL();
            }
        else
            {
            IMUM_OUT();
            return iSubOperation->ProgressL();
            }
        }
    else
        {
        IMUM_OUT();
        return iSyncProgress;
        }
    }

// ----------------------------------------------------------------------------
// CImapConnectionOp::DecodeProgress()
// ----------------------------------------------------------------------------
TInt CImapConnectionOp::DecodeProgress(const TDesC8& aProgress,
        TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString,
        TInt& aTotalEntryCount, TInt& aEntriesDone,
        TInt& aCurrentEntrySize, TInt& aCurrentBytesTrans, TBool aInternal)
    {
    IMUM_CONTEXT( CImapConnectionOp::DecodeProgress, 0, KImumMtmLog );
    IMUM_IN();
    
    if(aProgress.Length()==0)
        return KErrNone;

    TPckgC<TInt> type(0);
    type.Set(aProgress.Left(sizeof(TInt)));

    IMUM_OUT();
    if(!aInternal || type()!=EUiProgTypeConnecting)
        return iMtmUi->GetProgress(aProgress, aReturnString, aTotalEntryCount,
            aEntriesDone, aCurrentEntrySize, aCurrentBytesTrans);
    else
        //  We want the internal representation of connection progress...
        return DecodeConnectionProgress(aProgress, aReturnString,aTotalEntryCount, aEntriesDone, aCurrentEntrySize, aCurrentBytesTrans);
    }

// ----------------------------------------------------------------------------
// CImapConnectionOp::DecodeConnectionProgress()
// ----------------------------------------------------------------------------
TInt CImapConnectionOp::DecodeConnectionProgress(
    const TDesC8& aProgress,
    TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString,
    TInt& aTotalEntryCount,
    TInt& aEntriesDone,
    TInt& aCurrentEntrySize,
    TInt& aCurrentBytesTrans)
    {
    IMUM_CONTEXT( CImapConnectionOp::DecodeConnectionProgress, 0, KImumMtmLog );
    IMUM_IN();
    
    aTotalEntryCount=0;     //  No quantitative progress to return
    aEntriesDone=0;
    aCurrentEntrySize=0;
    aCurrentBytesTrans=0;

    TPckgBuf<TMsvEmailConnectionProgress> progressBuf;
    progressBuf.Copy(aProgress);
    const TMsvEmailConnectionProgress& connectionProgress=progressBuf();

    switch(connectionProgress.iState)
        {
    case TMsvEmailConnectionProgress::EInitialising:
    case TMsvEmailConnectionProgress::EConnectNetwork:
        {
        // Check if the name is already prepared
        if( iConnectionText )
            {
            aReturnString.Copy( *iConnectionText );
            }
        else
            {
            MsvEmailMtmUiUtils::CreateConnectingToText( aReturnString,
                iMsvSession, iService );
            iConnectionText = aReturnString.Alloc();
            }

        }
        break;
    case TMsvEmailConnectionProgress::EConnectMailbox:
        aReturnString.Copy( *iTitleText );
        break;
        }
    IMUM_OUT();

    return connectionProgress.iErrorCode;
    }

// ----------------------------------------------------------------------------
// CImapConnectionOp::Complete()
// ----------------------------------------------------------------------------
void CImapConnectionOp::Complete()
    {
    IMUM_CONTEXT( CImapConnectionOp::Complete, 0, KImumMtmLog );
    IMUM_IN();
    
    TRequestStatus* observer=&iObserverRequestStatus;
    User::RequestComplete(observer, KErrNone);
    iState=ECompleted;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImapConnectionOp::HandleImapConnectionEvent()
// ----------------------------------------------------------------------------
void CImapConnectionOp::HandleImapConnectionEvent(
    TImapConnectionEvent aConnectionEvent )
    {
    IMUM_CONTEXT( CImapConnectionOp::HandleImapConnectionEvent, 0, KImumMtmLog );
    IMUM_IN();
    
    switch ( aConnectionEvent )
        {
    case ESynchronisingInbox:
        //  Means that the connection has finished
        iState=EUpdatingFolderList;
        delete iConnectionProgressProvider;
        iConnectionProgressProvider=NULL;

        //  Get progress decoded by the default decoder
        iProgressReporter.UnsetProgressDecoder();

        // Try to update successful connection time. If failed
        // just forget it.
        TRAP_IGNORE( ImumMboxSettingsUtils::SetLastUpdateInfoL(
            *iMailboxApi, iService, ETrue ) );

        break;

    case ESynchronisingFolderList:
        break;

    case ESynchronisingFolders:
    case ESynchronisationComplete:
        {
        if ( iConnectionType == EWithSyncCompleteAfterDisconnection )
            {
            // Only hide dialog if we're here 'til the end
            TRAP_IGNORE( iProgressReporter.MakeProgressVisibleL( EFalse ) );
            }

        // Send the 'Turn ON' message to always online.
        TRAP_IGNORE( MsvEmailMtmUiUtils::SendAOCommandL(
            EServerAPIEmailUpdateMailWhileConnected, iService ) );
        }

        break;

    default:
        break;
        }

    /* We can always call NewMessage function, because Ncnlist is responsible of
    checking whether there really is new messages or not. We only tell Ncnlist
    that now there might be new messages */
    TRAP_IGNORE( MsvEmailMtmUiUtils::CallNewMessagesL( iService ) );
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImapConnectionOp::OpCompleted()
// ----------------------------------------------------------------------------
void CImapConnectionOp::OpCompleted(
    CMsvSingleOpWatcher& /*aOpWatcher*/,
    TInt /*aCompletionCode*/ )
    {
    IMUM_CONTEXT( CImapConnectionOp::OpCompleted, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    /*
    TMsvOp opId = aOpWatcher.Operation().Id();
    const TInt count = iOperations.Count();

    for ( TInt i=0; i < count; i++ )
        {
        CMsvOperation& op = iOperations[i]->Operation();

        if ( op.Id() == opId )
            {
            delete iOperations[i];
            iOperations.Delete(i);
            break;
            }
        }
        */
    }

// End of File
