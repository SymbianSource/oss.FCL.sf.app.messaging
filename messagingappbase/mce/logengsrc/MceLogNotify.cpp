/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     Reqisters Mce to LogEng to get notification about log DB changes.
*
*/



// INCLUDE FILES

#include <coemain.h>
#include <logview.h>
#include <logcli.h>
#include "MceLogNotify.h"
#include "MceLogEngine.h"

// ================= MEMBER FUNCTIONS =======================

CMceLogNotify::CMceLogNotify(
    CMceLogEngine* aObserver,
    TTimeIntervalMicroSeconds32 aDelay )
    : CActive(EPriorityStandard),
      iObserver(aObserver),
      iDelay(aDelay)
    {
    }

CMceLogNotify* CMceLogNotify::NewL(
    CMceLogEngine* aObserver,    // Pointer to model
    TTimeIntervalMicroSeconds32 aDelay)
    {
    CMceLogNotify* self = new (ELeave) CMceLogNotify( aObserver, aDelay );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( );  // self
    return self;
    }

void CMceLogNotify::ConstructL()
    {
    User::LeaveIfError(iFsSession.Connect());
    iLogClient = CLogClient::NewL(iFsSession);

    CActiveScheduler::Add(this);
    }

CMceLogNotify::~CMceLogNotify()
    {
    if( iLogClient )
        {
        iLogClient->NotifyChangeCancel();
        }
    Cancel();

    delete iLogClient;
    iFsSession.Close();

    iObserver = NULL;
    }

void CMceLogNotify::IssueNotifyRequest()
    {
    iLogClient->NotifyChange( iDelay, iStatus);
    SetActive();
    }

void CMceLogNotify::DoCancel()
    {
    iLogClient->NotifyChangeCancel();
    }

void CMceLogNotify::RunL()
    {
    iObserver->NotifyDbChangedL();
    }

// End of file
