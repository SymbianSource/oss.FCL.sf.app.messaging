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
*       Base class for operations which perform the same function on
*       all entries of a CMsvEntrySelection
*
*/


#include <ImumInternalApi.h>        // CImumInternalApi

#include "SelectionOperationBase.h"
#include "ImumPanic.h"
#include "ImumMtmLogging.h"

CSelectionOperationBase::CSelectionOperationBase(
    CImumInternalApi& aMailboxApi, 
    TInt aPriority, 
    TRequestStatus& aObserverRequestStatus, 
    CMsvEntrySelection* aEntrySelection )
    :   
    CMsvOperation( aMailboxApi.MsvSession(), aPriority, aObserverRequestStatus ),
    iEntrySelection( aEntrySelection ),
    iSelectionOwnershipExternal( ETrue )
    {
    IMUM_CONTEXT( CSelectionOperationBase::CSelectionOperationBase, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    }

CSelectionOperationBase::~CSelectionOperationBase()
    {
    IMUM_CONTEXT( CSelectionOperationBase::~CSelectionOperationBase, 0, KImumMtmLog );
    IMUM_IN();
    
    if(!iSelectionOwnershipExternal)
        {
        delete iEntrySelection;
        }
    delete iOperation;
    IMUM_OUT();
    }

void CSelectionOperationBase::StartL()
    {
    IMUM_CONTEXT( CSelectionOperationBase::StartL, 0, KImumMtmLog );
    IMUM_IN();
    
    __ASSERT_DEBUG(iCurrentEntryIndex==0,
        User::Panic(KImumMtmUiPanic, EPanicOpStartLCalledWhileStarted));
    __ASSERT_DEBUG(iEntrySelection->Count()!=0,
        User::Panic(KImumMtmUiPanic, EPanicSelectionIsEmpty));

    iOperation=CreateOperationL((*iEntrySelection)[iCurrentEntryIndex]);
    iObserverRequestStatus=KRequestPending;
    IMUM_OUT();
    }

void CSelectionOperationBase::RunL()
    {
    IMUM_CONTEXT( CSelectionOperationBase::RunL, 0, KImumMtmLog );
    IMUM_IN();
    
    DoRunL();
    
    IMUM_OUT();
    }

void CSelectionOperationBase::DoRunL()
    {
    IMUM_CONTEXT( CSelectionOperationBase::DoRunL, 0, KImumMtmLog );
    IMUM_IN();
    
    TInt err=iStatus.Int();
    if(!err)
        {
        err = CheckForError(iOperation->ProgressL());
        }

    if(StopOnError(err))
        {
        Complete(KErrNone);
        return;
        }

    if(++iCurrentEntryIndex == iEntrySelection->Count())
        {
        Complete(KErrNone);
        return;
        }
    else
        {
        delete iOperation;
        iOperation=NULL;
        iOperation=CreateOperationL((*iEntrySelection)[iCurrentEntryIndex]);
        };

    SetActive();
    IMUM_OUT();
    }

TInt CSelectionOperationBase::RunError(TInt aError)
	{
    IMUM_CONTEXT( CSelectionOperationBase::RunError, 0, KImumMtmLog );
    IMUM_IN();
        
    OpRunError(aError);
    Complete( KErrNone );
        	        	
	IMUM_OUT();			
	return KErrNone;
	}

void CSelectionOperationBase::DoCancel()
    {
    IMUM_CONTEXT( CSelectionOperationBase::DoCancel, 0, KImumMtmLog );
    IMUM_IN();
    
    if(iOperation)
        iOperation->Cancel();
    Complete(KErrCancel);
    IMUM_OUT();
    }

void CSelectionOperationBase::Complete(TInt aCompletionCode)
    {
    IMUM_CONTEXT( CSelectionOperationBase::Complete, 0, KImumMtmLog );
    IMUM_IN();
    
    TRequestStatus* completionStatus=&iObserverRequestStatus;
    User::RequestComplete(completionStatus,aCompletionCode);
    IMUM_OUT();
    }


