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
* Description:  ImapFolderSyncOperation.cpp
*
*
*/

#include <MuiuOperationWait.h>
#include <mtmuibas.h>
#include <imapcmds.h>
#include <msvuids.h>
#include <imapset.h>
#include "ImapFolderSyncOperation.h"
#include "EmailUtils.H"
#include "ImumMtmLogging.h"

// ----------------------------------------------------------------------------
// CImapFolderSyncOperation::NewL
// ----------------------------------------------------------------------------
CImapFolderSyncOperation* CImapFolderSyncOperation::NewL( CImumInternalApi& aMailboxApi, TRequestStatus& aStatus, TMsvId aMailbox, CBaseMtmUi& aMtmUi )
    {
    IMUM_STATIC_CONTEXT( CImapFolderSyncOperation::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();

    CImapFolderSyncOperation* self = new( ELeave ) CImapFolderSyncOperation( aMailboxApi, CActive::EPriorityIdle, aStatus, aMtmUi );
    CleanupStack::PushL( self );
    self->ConstructL( aMailbox );
    CleanupStack::Pop( self );
    IMUM_OUT();
    return self;
    }

// ----------------------------------------------------------------------------
// CImapFolderSyncOperation::CImapFolderSyncOperation
// ----------------------------------------------------------------------------
CImapFolderSyncOperation::CImapFolderSyncOperation(CImumInternalApi& aMailboxApi, TInt aPriority, TRequestStatus& aObserverRequestStatus, CBaseMtmUi& aMtmUi)
: CMsvOperation(aMailboxApi.MsvSession(), aPriority, aObserverRequestStatus),
iMtmUi( aMtmUi ),
iMailboxApi( aMailboxApi )
    {
    IMUM_CONTEXT( CImapFolderSyncOperation::CImapFolderSyncOperation, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();

    }


// ----------------------------------------------------------------------------
// CImapFolderSyncOperation::~CImapFolderSyncOperation()
// ----------------------------------------------------------------------------
CImapFolderSyncOperation::~CImapFolderSyncOperation()
    {
    IMUM_CONTEXT( CImapFolderSyncOperation::~CImapFolderSyncOperation, 0, KImumMtmLog );
    IMUM_IN();

    iTimer.Close();
    delete iOperation;
    IMUM_OUT();
    }


// ----------------------------------------------------------------------------
// CImapFolderSyncOperation::ConstructL()
// ----------------------------------------------------------------------------
void CImapFolderSyncOperation::ConstructL( TMsvId aMailbox )
    {
    IMUM_CONTEXT( CImapFolderSyncOperation::ConstructL, 0, KImumMtmLog );
    IMUM_IN();

    iMailboxId = aMailbox;

    //get syncRate from imap4 settings. We use that for folder updates
    /*CImumMboxData* accountSettings = CImumMboxData::NewLC();
    accountSettings->iImap4Settings = new(ELeave) CImImap4Settings;
    CMsvEntry* cEntry = iSession.GetEntryL( iMailboxId );
    CleanupStack::PushL( cEntry );
    MsvEmailMtmUiUtils::RestoreSettingsL( *cEntry, *( accountSettings->iImap4Settings ) );
    iInterval = ( accountSettings->iImap4Settings->SyncRate() / KImumSecondsInMinute );//syncRate is in seconds
    CleanupStack::PopAndDestroy(2);//cEntry, accountSettings

    User::LeaveIfError( iTimer.CreateLocal() );*/
    CActiveScheduler::Add(this);
    IMUM_OUT();
    }


// ----------------------------------------------------------------------------
// CImapFolderSyncOperation::ProgressL
// ----------------------------------------------------------------------------
const TDesC8& CImapFolderSyncOperation::ProgressL()
    {
    IMUM_CONTEXT( CImapFolderSyncOperation::ProgressL, 0, KImumMtmLog );
    IMUM_IN();

    IMUM_OUT();
    return iProgress;
    }

// ----------------------------------------------------------------------------
// CImapFolderSyncOperation::SetInterval
// ----------------------------------------------------------------------------
void CImapFolderSyncOperation::SetInterval()
    {
    IMUM_CONTEXT( CImapFolderSyncOperation::SetInterval, 0, KImumMtmLog );
    IMUM_IN();

    TTime home;
    home.HomeTime();
    home+=iInterval;

    iTimer.Cancel();
    iTimer.At(iStatus, home);
    iStatus = KRequestPending;
    SetActive();
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImapFolderSyncOperation::DoCancel
// ----------------------------------------------------------------------------
void CImapFolderSyncOperation::DoCancel()
    {
    IMUM_CONTEXT( CImapFolderSyncOperation::DoCancel, 0, KImumMtmLog );
    IMUM_IN();

    iTimer.Cancel();
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, KErrCancel );
    IMUM_OUT();
    }


// ----------------------------------------------------------------------------
// CImapFolderSyncOperation::RunL
// ----------------------------------------------------------------------------
void CImapFolderSyncOperation::RunL()
    {
    IMUM_CONTEXT( CImapFolderSyncOperation::RunL, 0, KImumMtmLog );
    IMUM_IN();

    if ( iOperation )
        {
        // operation completed, start new timer
        delete iOperation;
        iOperation = NULL;
        SetInterval();
        }
    else
        {
        TBool operationStarted = EFalse;
        // timer completed, start folder sync operation
        TMsvEntry tEntry;
        TMsvId service;
        if ( iMailboxApi.MsvSession().GetEntry( iMailboxId, service, tEntry ) == KErrNone )
            {
            if ( iMailboxApi.MsvSession().ServiceActive( iMailboxId ) )
                {
                // mailbox is doing something else, wait...
                SetInterval();
                return;
                }
            //do sync only if connected.
            if( tEntry.Connected() && HasSubscribedFoldersL() )
                {
                TBuf8<1> dummyParam;

                CMsvEntrySelection* sel = new (ELeave) CMsvEntrySelection;
                CleanupStack::PushL( sel );
                sel->AppendL( iMailboxId );

                //fullsync used to sync folders. Less code and less traffic than
                //if we would sync each folder separately
                iOperation = iMtmUi.InvokeAsyncFunctionL(
                    KIMAP4MTMFullSync,
                    *sel,
                    iStatus,
                    dummyParam);

                CleanupStack::PopAndDestroy( sel );
                iStatus = KRequestPending;
                SetActive();
                operationStarted = ETrue;
                }
            }
        if ( !operationStarted )
            {
            // something wrong, complete observer
            TRequestStatus* status = &iObserverRequestStatus;
            User::RequestComplete(status, KErrNone);
            }
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImapFolderSyncOperation::HasSubscribedFoldersL
// ----------------------------------------------------------------------------
TBool CImapFolderSyncOperation::HasSubscribedFoldersL()
    {
    IMUM_CONTEXT( CImapFolderSyncOperation::HasSubscribedFoldersL, 0, KImumMtmLog );
    IMUM_IN();

    TInt count = 0;
    CountSubscribedFoldersL( iMailboxId, count );
    IMUM_OUT();
    return count > 1; // return ETrue if more than inbox is subscribed
    }

// ----------------------------------------------------
// CAlwaysOnlineImap4Agent::AddSubscribedFoldersL()
// NOTE: This is recursive!
// ----------------------------------------------------
void CImapFolderSyncOperation::CountSubscribedFoldersL(
    TMsvId aFolderId, TInt& aCount )
    {
    IMUM_CONTEXT( CImapFolderSyncOperation::CountSubscribedFoldersL, 0, KImumMtmLog );
    IMUM_IN();

    CMsvEntry* entry = iMailboxApi.MsvSession().GetEntryL( aFolderId );
    CleanupStack::PushL( entry );

    const TInt count = entry->Count();
    for ( TInt loop = 0; loop < count; loop++ )
        {
        TMsvEmailEntry mailEntry = (*entry)[loop];
        if ( mailEntry.iType.iUid == KUidMsvFolderEntryValue )
            {
            if ( mailEntry.LocalSubscription() )
                {
                aCount++;
                }
            CountSubscribedFoldersL( mailEntry.Id(), aCount );
            }
        }

    CleanupStack::PopAndDestroy(); // entry

    IMUM_OUT();
    }

// End of File
