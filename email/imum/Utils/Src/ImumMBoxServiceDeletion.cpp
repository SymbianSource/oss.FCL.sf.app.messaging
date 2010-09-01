/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MsvOperation encapsulating deletion of email service
*
*/


#include <StringLoader.h> // StringLoader
#include <mtuireg.h>
#include <muiumsvuiserviceutilitiesinternal.h>
#include <imum.rsg>
#include <MtmExtendedCapabilities.hrh>

#include "ImumMboxServiceDeletion.h"
#include "MsvPrgReporter.h"
#include "EmailUtils.H"
#include "SenduiMtmUids.h"
#include "ImumPanic.h"
#include "IMSSettingsNoteUi.h"
#include "ImumInternalApiImpl.h"

#include "ImumMtmLogging.h"

const TInt KSvcDelOpPriority        = CActive::EPriorityStandard;

// ----------------------------------------------------------------------------
// CImumMBoxServiceDeletion::NewL()
// ----------------------------------------------------------------------------
//
EXPORT_C CImumMBoxServiceDeletion* CImumMBoxServiceDeletion::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aServiceId )
    {
    CImumMBoxServiceDeletion* op = 
        new ( ELeave ) CImumMBoxServiceDeletion( aMsvSession,
                                                 aObserverRequestStatus,
                                                 aReporter,
                                                 aServiceId );
        
    CleanupStack::PushL( op );
    op->ConstructL();
    CleanupStack::Pop( op );
    return op;
    }

// ----------------------------------------------------------------------------
// CImumMBoxServiceDeletion::~CImumMBoxServiceDeletion()
// ----------------------------------------------------------------------------
//
CImumMBoxServiceDeletion::~CImumMBoxServiceDeletion()
    {
    IMUM_CONTEXT( CImumMBoxServiceDeletion::CImumMBoxServiceDeletion, 0, KImumMtmLog );
    IMUM_IN();
       
    if ( IsActive() )
        {        
        Cancel();
        }
    
    delete iEmailApi;
    delete iSubOperation;
    delete iProgressString;
    
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMBoxServiceDeletion::ProgressL()
// ----------------------------------------------------------------------------
//
const TDesC8& CImumMBoxServiceDeletion::ProgressL()
    {
    if ( iState == EDeleteService )
        {
        iProgressBuf.Copy( iSubOperation->ProgressL() );
        }
    return iProgressBuf;
    }

// ----------------------------------------------------------------------------
// CImumMBoxServiceDeletion::RunL()
// ----------------------------------------------------------------------------
//
void CImumMBoxServiceDeletion::RunL()
    {
    IMUM_CONTEXT( CImumMBoxServiceDeletion::RunL, 0, KImumMtmLog );
    IMUM_IN();
    
    DoRunL(); 

    IMUM_OUT();        
    }

// ----------------------------------------------------------------------------
// CImumMBoxServiceDeletion::DoCancel()
// ----------------------------------------------------------------------------
//
void CImumMBoxServiceDeletion::DoCancel()
    {
    IMUM_CONTEXT( CImumMBoxServiceDeletion::DoCancel, 0, KImumMtmLog );
    IMUM_IN();
    
    if ( iSubOperation )
        {
        iSubOperation->Cancel();
        }
    CompleteObserver();
    IMUM_OUT();
    }
    
// ----------------------------------------------------------------------------
// CImumMBoxServiceDeletion::RunError()
// ----------------------------------------------------------------------------
//    
TInt CImumMBoxServiceDeletion::RunError(TInt __DEBUG_ONLY( aError ) )
	{
    IMUM_CONTEXT( CImumMBoxServiceDeletion::RunError, 0, KImumMtmLog );
    IMUM_IN();
    
    IMUM1( 0, "RunL error %d", aError);
    
    CompleteObserver();
    
    IMUM_OUT();		
    return KErrNone; // RunError must return KErrNone to active sheduler.
	}

// ----------------------------------------------------------------------------
// CImumMBoxServiceDeletion::CImumMBoxServiceDeletion()
// ----------------------------------------------------------------------------
//
CImumMBoxServiceDeletion::CImumMBoxServiceDeletion(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    MMsvProgressReporter& aReporter,
    TMsvId aServiceId )
    :
    CMsvOperation( aMsvSession, KSvcDelOpPriority, aObserverRequestStatus ),
    iReporter( aReporter ),
    iServiceId( aServiceId )
    {
    IMUM_CONTEXT( CImumMBoxServiceDeletion::CImumMBoxServiceDeletion, 0, KImumMtmLog );
    IMUM_IN();    
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMBoxServiceDeletion::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumMBoxServiceDeletion::ConstructL()
    {
    IMUM_CONTEXT( CImumMBoxServiceDeletion::ConstructL, 0, KImumMtmLog );
    IMUM_IN();

    CActiveScheduler::Add( this );

    iEmailApi = CImumInternalApiImpl::NewL( &iMsvSession );

    TMsvEntry servEntry;
    TMsvId serviceId;
    User::LeaveIfError(
        iMsvSession.GetEntry( iServiceId, serviceId, servEntry ) );

    iSMTPServiceId = servEntry.iRelatedId;
    iMtm = servEntry.iMtm;

	TMsvEntry smtpEntry;
	TMsvId smtpServiceId;
	
    User::LeaveIfError( iMsvSession.GetEntry(
        iSMTPServiceId, smtpServiceId, smtpEntry ) );
  
    // serviceEntry must: be a service entry and have the root as parent.
    __ASSERT_DEBUG(servEntry.iType.iUid == KUidMsvServiceEntryValue,
        User::Panic(KImumMtmUiPanic,ESmtpMtmUiSvcDelOpNotAService) );
    __ASSERT_DEBUG(servEntry.Parent() == KMsvRootIndexEntryIdValue,
        User::Panic(KImumMtmUiPanic,ESmtpMtmUiSvcDelOpParentIsNotRoot) );
    
    SetProgressStringL( smtpEntry.Id() );
    iReporter.SetProgressDecoder( *this );
    
    // Remove mailbox from ao agent list.
    // if fails, just ignore, doesn't effect us
    TRAP_IGNORE( MsvEmailMtmUiUtils::SendAOCommandL( EServerAPIEmailAgentRemove, 
        iServiceId ) );
                                        
    BeginDeleteOperationL();
    iState = EDoPrompts;
    ProgressL();//Update initial progress.
        
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMBoxServiceDeletion::BeginDeleteOperationL()
// ----------------------------------------------------------------------------
//
void CImumMBoxServiceDeletion::BeginDeleteOperationL()
    {
    IMUM_CONTEXT( CImumMBoxServiceDeletion::BeginDeleteOperationL, 0, KImumMtmLog );
    IMUM_IN();
    
    iSubOperation = CMsvCompletedOperation::NewL(
            iMsvSession, KUidMsvLocalServiceMtm, KNullDesC8,
            KMsvLocalServiceIndexEntryId, iStatus );
            
    SetActive();
    IMUM_OUT();
    }
    
// ----------------------------------------------------------------------------
// CImumMBoxServiceDeletion::RemovePreCreatedMessagesL()
// ----------------------------------------------------------------------------
//
void CImumMBoxServiceDeletion::RemovePreCreatedMessagesL()
    {
    IMUM_CONTEXT( CImumMBoxServiceDeletion::RemovePreCreatedMessagesL, 0, KImumMtmLog );
    IMUM_IN();
    
    iStatus = KRequestPending;
    iSubOperation = GetPreCreatedMailDeletionOpL( iStatus );
    SetActive();
    
    if( !iSubOperation )
        {
        // If this is the case, complete this stage.
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);
        }
    IMUM_OUT();        
    }

// ----------------------------------------------------------------------------
// CImumMBoxServiceDeletion::DoRunL()
// ----------------------------------------------------------------------------
//
void CImumMBoxServiceDeletion::DoRunL()
    {
    IMUM_CONTEXT( CImumMBoxServiceDeletion::DoRunL, 0, KImumMtmLog );
    IMUM_IN();
    
    if( iSubOperation )
        {
        if ( iSubOperation->IsActive() )
            {
            iSubOperation->Cancel();
            }
            
        delete iSubOperation;
        iSubOperation = NULL;
        }

    switch(iState)
        {
    case EDoPrompts:
        DoRemoveMailboxL();
        iState = EDeleteService;
        break;        
    case EDeleteService:
        RemovePreCreatedMessagesL();
        iState = EDeletePreCreatedMsgs;
        break;
    case EDeletePreCreatedMsgs:
        CompleteObserver();
        break;
    case EStateComplete:
    default:
        break;
        }
    IMUM_OUT();            
    }

// ----------------------------------------------------------------------------
// CImumMBoxServiceDeletion::CompleteObserver()
// ----------------------------------------------------------------------------
//    
void CImumMBoxServiceDeletion::CompleteObserver()
    {
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete(status, iStatus.Int());
    }


// ----------------------------------------------------------------------------
// CImumMBoxServiceDeletion::DecodeProgress()
// ----------------------------------------------------------------------------
//
TInt CImumMBoxServiceDeletion::DecodeProgress(
    const TDesC8& /* aProgress */,
    TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString,
    TInt& aTotalEntryCount,
    TInt& aEntriesDone,
    TInt& aCurrentEntrySize,
    TInt& aCurrentBytesTrans,
    TBool /* aInternal */)
    {
    aTotalEntryCount = 0;
    aEntriesDone = 0;
    aCurrentEntrySize = 0;
    aCurrentBytesTrans = 0;
    aReturnString.Copy( *iProgressString );
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CImumMBoxServiceDeletion::BeginRemoveAccountDeleteOpL()
// ----------------------------------------------------------------------------
//
void CImumMBoxServiceDeletion::BeginRemoveAccountDeleteOpL()
    {
    IMUM_CONTEXT( CImumMBoxServiceDeletion::BeginRemoveAccountDeleteOpL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Complete right after the deletion
    iSubOperation = CMsvCompletedOperation::NewL( iMsvSession,
                                                  KUidMsvLocalServiceMtm,
                                                  KNullDesC8,
                                                  KMsvLocalServiceIndexEntryId,
                                                  iStatus );
    iStatus = KRequestPending;
    SetActive();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumMBoxServiceDeletion::GetPreCreatedMailDeletionOpL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImumMBoxServiceDeletion::GetPreCreatedMailDeletionOpL( TRequestStatus& aStatus )
    {
    IMUM_CONTEXT( CImumMBoxServiceDeletion::GetPreCreatedMailDeletionOpL, 0, KImumMtmLog );
    IMUM_IN();
    
    //Get SMTP UI mtm
    CClientMtmRegistry* clientRegistry = CClientMtmRegistry::NewL( iMsvSession );
    CleanupStack::PushL( clientRegistry );

    CBaseMtm* smtpmtm = clientRegistry->NewMtmL( KUidMsgTypeSMTP );
    CleanupStack::PushL( smtpmtm );

    CMtmUiRegistry* mtmuireg = CMtmUiRegistry::NewL( iMsvSession );
    CleanupStack::PushL( mtmuireg );

    CBaseMtmUi* smtpUi = mtmuireg->NewMtmUiL( *smtpmtm );
    CleanupStack::PushL( smtpUi );
    
    CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL(sel); 
    sel->AppendL( iSMTPServiceId );
    
    TBuf8<1> dummyParam;
    CMsvOperation* op = smtpUi->InvokeAsyncFunctionL( KMtmUiFunctionPreCreateMessage, *sel, aStatus, dummyParam );
    
    CleanupStack::PopAndDestroy( 5, clientRegistry ); // CSI: 47 # sel, smtpUi, mtmuireg, smtpmtm, clientRegistry
    
    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
// CImumMBoxServiceDeletion::DoRemoveMailboxL()
// ----------------------------------------------------------------------------
//    
void CImumMBoxServiceDeletion::DoRemoveMailboxL()
    {
    IMUM_CONTEXT( CImumMBoxServiceDeletion::DoRemoveMailboxL, 0, KImumMtmLog );
    IMUM_IN();
    
    iStatus = KRequestPending;
    
    iEmailApi->SettingsManagerL().RemoveAccount( iServiceId, ETrue );
    
    iSubOperation = CMsvCompletedOperation::NewL( iMsvSession,
                                                  KUidMsvLocalServiceMtm,
                                                  KNullDesC8,
                                                  KMsvLocalServiceIndexEntryId,
                                                  iStatus );
                                                  
            
    SetActive();
    IMUM_OUT();
    }
    
// ----------------------------------------------------------------------------
// CImumMBoxServiceDeletion::SetProgressStringL()
// ----------------------------------------------------------------------------
//      
void CImumMBoxServiceDeletion::SetProgressStringL( TMsvId aServiceId )
    {
    TMsvId serv;
    TMsvEntry tEntry;
    
    TInt err = iMsvSession.GetEntry( aServiceId, serv, tEntry );
    
    if ( iProgressString )
        {
        delete iProgressString;
        iProgressString = NULL;
        }
        
    iProgressString = StringLoader::LoadL(
        R_EMAIL_SVCDELOP_DIALOG_TITLE, tEntry.iDetails, CEikonEnv::Static() );

    iReporter.SetTitleL(*iProgressString);
    iReporter.SetProgressDecoder( *this );
    }

// End of File
