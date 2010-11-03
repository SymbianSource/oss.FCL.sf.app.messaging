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

#include <MTMStore.h>

#include "SmtpPreCreationOpBase.h"
#include "EmailPreCreation.h"
#include "ImumMtmLogging.h"



CSmtpPreCreationOpBase::~CSmtpPreCreationOpBase()
    {
    IMUM_CONTEXT( CSmtpPreCreationOpBase::~CSmtpPreCreationOpBase, 0, KImumMtmLog );
    IMUM_IN();
    
    Cancel();
    delete iOperation;
    delete iPreCreate;
    delete iMtmStore;
    delete iMailboxApi;
    iMailboxApi = NULL;
    IMUM_OUT();
    }


const TDesC8& CSmtpPreCreationOpBase::ProgressL()
    {
    IMUM_CONTEXT( CSmtpPreCreationOpBase::ProgressL, 0, KImumMtmLog );
    IMUM_IN();
    
    if(iOperation)
        {
        IMUM_OUT();
        return iOperation->ProgressL();
        }
    IMUM_OUT();
    return iProgress;
    }


void CSmtpPreCreationOpBase::DoCancel()
    {
    IMUM_CONTEXT( CSmtpPreCreationOpBase::DoCancel, 0, KImumMtmLog );
    IMUM_IN();
    
    if(iOperation)
        {
        iOperation->Cancel();
        }
    // complete observer and itself
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete(status, KErrCancel);
    status = &iStatus;
    if( ( *status ) == KRequestPending )
        {
        User::RequestComplete(status, KErrCancel);
        }
    IMUM_OUT();

    }


void CSmtpPreCreationOpBase::CompleteObserver()
    {
    IMUM_CONTEXT( CSmtpPreCreationOpBase::CompleteObserver, 0, KImumMtmLog );
    IMUM_IN();
    
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete(status, KErrNone);
    IMUM_OUT();
    }


void CSmtpPreCreationOpBase::CompleteThis()
    {
    IMUM_CONTEXT( CSmtpPreCreationOpBase::CompleteThis, 0, KImumMtmLog );
    IMUM_IN();
    
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    IMUM_OUT();
    }


CSmtpPreCreationOpBase::CSmtpPreCreationOpBase(CMsvSession& aMsvSession, TInt aPriority, TRequestStatus& aObserverRequestStatus)
: CMsvOperation(aMsvSession, aPriority, aObserverRequestStatus)
    {
    IMUM_CONTEXT( CSmtpPreCreationOpBase::CSmtpPreCreationOpBase, 0, KImumMtmLog );
    IMUM_IN();
    
    CActiveScheduler::Add(this);
    IMUM_OUT();
    }


void CSmtpPreCreationOpBase::ConstructL()
    {
    IMUM_CONTEXT( CSmtpPreCreationOpBase::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    iMailboxApi = CreateEmailApiL( &iMsvSession );
    iPreCreate = CEmailPreCreation::NewL( *iMailboxApi );
    iMtmStore = CMtmStore::NewL(iMsvSession);
    iObserverRequestStatus = KRequestPending;    
    IMUM_OUT();
    }



