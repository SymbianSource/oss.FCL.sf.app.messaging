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
*       Simple Timer Operation. Completes observer when time expires
*
*/


#include "SmtpTimerOp.h"
#include "ImumMtmLogging.h"



CSmtpTimerOp::CSmtpTimerOp(CMsvSession& aMsvSession, TInt aPriority, TRequestStatus& aObserverRequestStatus)
: CMsvOperation(aMsvSession, aPriority, aObserverRequestStatus)
    {
    IMUM_CONTEXT( CSmtpTimerOp::CSmtpTimerOp, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    }


CSmtpTimerOp::~CSmtpTimerOp()
    {
    IMUM_CONTEXT( CSmtpTimerOp::~CSmtpTimerOp, 0, KImumMtmLog );
    IMUM_IN();
    
    iTimer.Close();
    IMUM_OUT();
    }


void CSmtpTimerOp::ConstructL()
    {
    IMUM_CONTEXT( CSmtpTimerOp::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add(this);
    IMUM_OUT();
    }


const TDesC8& CSmtpTimerOp::ProgressL()
    {
    IMUM_CONTEXT( CSmtpTimerOp::ProgressL, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    return iProgress;
    }


void CSmtpTimerOp::At(const TTime& aTime)
    {
    IMUM_CONTEXT( CSmtpTimerOp::At, 0, KImumMtmLog );
    IMUM_IN();
    
    iTimer.At(iStatus, aTime);
    SetActive();
    IMUM_OUT();
    }


void CSmtpTimerOp::After(TTimeIntervalMicroSeconds32 aInterval)
    {
    IMUM_CONTEXT( CSmtpTimerOp::After, 0, KImumMtmLog );
    IMUM_IN();
    
    iTimer.After(iStatus, aInterval);
    SetActive();
    IMUM_OUT();
    }


void CSmtpTimerOp::DoCancel()
    {
    IMUM_CONTEXT( CSmtpTimerOp::DoCancel, 0, KImumMtmLog );
    IMUM_IN();
    
    iTimer.Cancel();
    IMUM_OUT();
    }


void CSmtpTimerOp::RunL()
    {
    IMUM_CONTEXT( CSmtpTimerOp::RunL, 0, KImumMtmLog );
    IMUM_IN();
    
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete(status, KErrNone);
    IMUM_OUT();
    }



