/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "cimapupdateflagoperation.h"
#include "cimapfolder.h"
#include "cimapsyncmanager.h"
#include "cimapsessionconsts.h"
#include "cimaplogger.h"
#include "imappaniccodes.h"

_LIT8(KImapAddFlags, "+FLAGS");
_LIT8(KStoreFlagsClearCommand, "-FLAGS");
_LIT8(KImapFlagSeen, "(\\Seen)");


CImapUpdateFlagOperation::CImapUpdateFlagOperation(CImapSyncManager& aSyncManager,
						 				 CMsvServerEntry& aServerEntry,
						 				 CImapSettings& aImapSettings) :
	CImapCompoundBase(aSyncManager, aServerEntry, aImapSettings)
	
	{
	
	}

CImapUpdateFlagOperation::~CImapUpdateFlagOperation()
	{
	Cancel();
	iMessageUids.Reset();
	iFlagInfoResponseArray.Reset();
	}
	
CImapUpdateFlagOperation* CImapUpdateFlagOperation::NewL(CImapSyncManager& aSyncManager,
						 				  	   CMsvServerEntry& aServerEntry,
						 				  	   CImapSettings& aImapSettings)
	{
	CImapUpdateFlagOperation* self = new (ELeave) CImapUpdateFlagOperation(aSyncManager,
																 aServerEntry,
																 aImapSettings
																 );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CImapUpdateFlagOperation::ConstructL()
	{
	
	// Add to the active scheduler
	CActiveScheduler::Add(this);
	}

void CImapUpdateFlagOperation::StartOperation(TRequestStatus& aStatus, CImapSession& aSession)
//void CImapUpdateFlagOperation::StartOperation(TRequestStatus& aStatus)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapUpdateFlagOperation::StartOperation()");
	iNextStep = EUpdateFlag;
	Queue(aStatus);
	CompleteSelf();
	}

/**
Handles the compound operation state machine

@return ETrue if compound operation is completed, 
		EFalse otherwise.
*/	
TBool CImapUpdateFlagOperation::DoRunLoopL()
	{
	SetCurrentStep();
	switch (iCurrentStep)
		{
	
	case EUpdateFlag:
		{		
		TMsvEmailEntry entry = iServerEntry.Entry();
		TUint id = entry.UID();
		iMessageUids.AppendL(id);
		if(entry.Unread())
			{
			entry.SetSeenIMAP4Flag(EFalse);
			}
		else
			{
			entry.SetSeenIMAP4Flag(ETrue);	
			}
				
				
		User::LeaveIfError(iServerEntry.ChangeEntry(entry));

		HBufC8* uidSeq = CImapSession::CreateSequenceSetLC(iMessageUids);

		// finished with the UID array. Clear it.
		iMessageUids.Reset();
		if(entry.SeenIMAP4Flag())
			{
			// issue the store command
			iSession->StoreL(iStatus, uidSeq->Des(), KImapAddFlags, KImapFlagSeen, ETrue, iFlagInfoResponseArray);
			
			}
		else
			{
			// issue the store command
			iSession->StoreL(iStatus, uidSeq->Des(), KStoreFlagsClearCommand, KImapFlagSeen, ETrue, iFlagInfoResponseArray);
			}
	
		iProgressState = TImap4GenericProgress::EBusy;
		CleanupStack::PopAndDestroy(uidSeq);
		iNextStep = EFinished;
		break;
		
		}
	
	
	case EFinished:
		{
		__LOG_TEXT(iSession->LogId(), "CImapUpdateFlagOperation::Completing OK");
		iProgressState = TImap4GenericProgress::EIdle;
		Complete(KErrNone);
		return ETrue;
		}
		
	default:
		{
		__ASSERT_DEBUG(EFalse, 
					   TImapServerPanic::ImapPanic(TImapServerPanic::ECreateCompoundUnexpectedState));
		// unexpected state - complete the request
		iProgressState = TImap4GenericProgress::EIdle;
		return ETrue;
		}
		}

	if (!IsActive())
		{
		SetActive();
		}
	return EFalse;
	}

void CImapUpdateFlagOperation::DoCancel()
	{
	iProgressErrorCode = KErrCancel;
	
	switch (iCurrentStep)
		{
	case EUpdateFlag:
		{
		// outstanding request is on session
		iSession->Cancel();
		break;
		}
	case EFinished:
		{
		// self-completed or no outstanding request.
		break;
		}

	default:
		{
		__ASSERT_DEBUG(EFalse,
		               TImapServerPanic::ImapPanic(TImapServerPanic::ECreateCompoundCancelUnexpectedState));
		break;
		}
		} // end switch (iCurrentStep)
	
	CMsgActive::DoCancel();
	}




void CImapUpdateFlagOperation::Progress(TImap4CompoundProgress& aCompoundProgress)
	{
	// Create does not set iOperation, it just sets iState to EBusy 
	// when doing the creation
	aCompoundProgress.iGenericProgress.iState = iProgressState;

	
	// Put error into progress buffer
	if( aCompoundProgress.iGenericProgress.iErrorCode == KErrNone )
		{
		aCompoundProgress.iGenericProgress.iErrorCode = iProgressErrorCode;
		}
	}


/**
Handles NO/BAD responses according to current step.
Negative server responses are not fatal - the error is reported in the 
progress report, and the operation completes.

@return KErrNone if the error has been handled
		Completion error code otherwise.
*/
TInt CImapUpdateFlagOperation::ProcessNegativeServerResponse()
	{
	TInt err = iStatus.Int();
	switch (iCurrentStep)
		{
	case EUpdateFlag:
		iNextStep = EFinished;
		break;
	case EFinished:
	default:
		{
		// positive error code not expected,
		// self-completed states or no outstanding request.
		TImapServerPanic::ImapPanic(TImapServerPanic::ECreateCompoundUnexpectedState);
		break;
		}
		
		} // end switch (iCurrentStep)
	iProgressErrorCode = err;
	return KErrNone;
	}

/**
Resume the operation following a bearer migration.
*/
void CImapUpdateFlagOperation::ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSession = &aSession;
	__LOG_TEXT(iSession->LogId(), "CImapUpdateFlagOperation::Resuming");
	__ASSERT_DEBUG(iCurrentStep==ESuspendedForMigrate, TImapServerPanic::ImapPanic(TImapServerPanic::EUpdateCompoundUnexpectedState));
	iStopForMigrate = EFalse;

	switch (iNextStep)
		{
		case EUpdateFlag:
			{
			// just return to the main state machine
			CompleteSelf();
			break;
			}
		case EFinished:
		default:
			{
			__ASSERT_DEBUG(EFalse, TImapServerPanic::ImapPanic(TImapServerPanic::EUpdateCompoundUnexpectedState));
			// abandon the compound operation
			iNextStep=EFinished;
			CompleteSelf();
			break;
			}
		}
	Queue(aStatus);
	CompleteSelf();
	}

