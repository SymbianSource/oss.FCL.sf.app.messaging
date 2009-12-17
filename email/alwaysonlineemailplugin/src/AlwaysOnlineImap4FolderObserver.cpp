/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     Observer of the given folder. If new messages are created to the folder,
*     calls HandleFolderEntryL.
*
*/


#include "AlwaysOnlineEmailPluginLogging.h"
#include "AlwaysOnlineImap4FolderObserver.h"
#include "AlwaysOnlineEmailLoggingTools.h"

const TInt KAlwaysOnlineImap4FolderObserverTimeout = 2000000; // 2 seconds

// ----------------------------------------------------------------------------
// CAlwaysOnlineImap4FolderObserver()
// ----------------------------------------------------------------------------
CAlwaysOnlineImap4FolderObserver::CAlwaysOnlineImap4FolderObserver(
    CMsvSession& aMsvSession,
    MAlwaysOnlineImap4FolderObserver& aObserver )
    :
    CActive( EPriorityStandard ),
    iMsvSession( aMsvSession ),
    iObserver( aObserver )
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderObserver::CAlwaysOnlineImap4FolderObserver" );
    CActiveScheduler::Add( this );
    }


// ----------------------------------------------------------------------------
// NewL()
// ----------------------------------------------------------------------------
CAlwaysOnlineImap4FolderObserver* CAlwaysOnlineImap4FolderObserver::NewL(
    CMsvSession& aMsvSession,
    MAlwaysOnlineImap4FolderObserver& aObserver )
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderObserver::NewL" );
    return CAlwaysOnlineImap4FolderObserver::NewL( aMsvSession, NULL, aObserver );
    }

// ----------------------------------------------------------------------------
// NewL()
// ----------------------------------------------------------------------------
CAlwaysOnlineImap4FolderObserver* CAlwaysOnlineImap4FolderObserver::NewL(
    CMsvSession& aMsvSession,
    CMsvEntrySelection* aFoldersToWatch,
    MAlwaysOnlineImap4FolderObserver& aObserver )
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderObserver::NewL" );
    CAlwaysOnlineImap4FolderObserver* self =
        new (ELeave) CAlwaysOnlineImap4FolderObserver(
        aMsvSession, aObserver );

    CleanupStack::PushL( self );
    self->ConstructL( aFoldersToWatch );
    CleanupStack::Pop( self );

    return self;
    }


// ----------------------------------------------------------------------------
// ConstructL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4FolderObserver::ConstructL(
    CMsvEntrySelection* aFoldersToWatch )
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderObserver::ConstructL" );
    User::LeaveIfError( iTimer.CreateLocal() );
    iRunning = EFalse;
    iSendNotification = EFalse;
    iMsvSession.AddObserverL( *this );

    if ( aFoldersToWatch )
        {
        iFolders = aFoldersToWatch->CopyL();
        }
    else
        {
        iFolders = new (ELeave) CMsvEntrySelection;
        }
    iChangedFolders = new (ELeave) CMsvEntrySelection;


    }

// ----------------------------------------------------------------------------
// ~CAlwaysOnlineImap4Agent()
// ----------------------------------------------------------------------------
CAlwaysOnlineImap4FolderObserver::~CAlwaysOnlineImap4FolderObserver()
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderObserver::~CAlwaysOnlineImap4FolderObserver" );
    iMsvSession.RemoveObserver( *this );
    delete iFolders;
    delete iChangedFolders;
    Cancel();
    iTimer.Close();
    }

// ----------------------------------------------------------------------------
// Start()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4FolderObserver::Start()
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderObserver::Start" );
    KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4FolderObserver::Start");
    iRunning = ETrue;
    iSendNotification = EFalse;
    }

void CAlwaysOnlineImap4FolderObserver::Stop()
    {
    KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4FolderObserver::Stop");
    Cancel();
    iRunning = EFalse;
    iSendNotification = EFalse;
    }


// ----------------------------------------------------------------------------
// DoCancel()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4FolderObserver::DoCancel()
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderObserver::DoCancel" );
    iTimer.Cancel();
    }

// ----------------------------------------------------------------------------
// RunL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4FolderObserver::RunL()
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderObserver::RunL" );
    KAOEMAIL_LOGGER_FN1("CAlwaysOnlineImap4FolderObserver::RunL");
    iRunning = EFalse;
    if ( iSendNotification )
        {
        iObserver.HandleFolderEntryL( *iChangedFolders );
        iChangedFolders->Reset();
        }
    KAOEMAIL_LOGGER_FN2("CAlwaysOnlineImap4FolderObserver::RunL");
    }

// ----------------------------------------------------------------------------
// HandleSessionEventL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4FolderObserver::HandleSessionEventL(
    TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* /*aArg3*/)
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderObserver::HandleSessionEventL" );
    KAOEMAIL_LOGGER_WRITE_FORMAT("CAlwaysOnlineImap4FolderObserver::HandleSessionEventL aEvent %d", aEvent);
    if ( !iRunning )
        {
        // just ignore everything, we are not running
        KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4FolderObserver::HandleSessionEventL: ignore everything");
        return;
        }
    const TTimeIntervalMicroSeconds32 timeInterval = KAlwaysOnlineImap4FolderObserverTimeout;

    switch ( aEvent )
        {
        case EMsvEntriesCreated:
        case EMsvEntriesChanged: // Handle this somehow
        case EMsvEntriesDeleted:
            {
            TMsvId parentId = (*(TMsvId*) (aArg2));
            if ( iFolders->Find( parentId ) != KErrNotFound )
                {
                KAOEMAIL_LOGGER_WRITE_FORMAT("parent 0x%x found", parentId);
                iSendNotification = ETrue;
                if ( iChangedFolders->Find( parentId ) == KErrNotFound )
                    {
                    iChangedFolders->AppendL( parentId );
                    }
                KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4FolderObserver::HandleSessionEventL: restart timer case 2");
                Cancel();
                // cancel called right before, no need to check if still active
                iTimer.After( iStatus, timeInterval ); // CSI: 10 # see comment above
                SetActive();
                }
            else
                {
                const CMsvEntrySelection* selection = static_cast<CMsvEntrySelection*>(aArg1);
                if ( iMailboxId > 0 &&
                    selection->Find( iMailboxId ) > KErrNotFound )
                    {
                    iSendNotification = ETrue;
                    iChangedFolders->AppendL( iMailboxId );
                    KAOEMAIL_LOGGER_WRITE("CAlwaysOnlineImap4FolderObserver::HandleSessionEventL: restart timer case 3");
                    Cancel();
                    // cancel called right before, no need to check if still active
                    iTimer.After( iStatus, timeInterval ); // CSI: 10 # see comment above
                    SetActive();
                    }
                }
            }
            break;
        default:
        break;
        }

    }

// ----------------------------------------------------------------------------
// AppendFolderL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4FolderObserver::AppendFolderL( TMsvId aFolderId )
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderObserver::AppendFolderL" );
    if ( iFolders->Find( aFolderId ) == KErrNotFound )
        {
        iFolders->AppendL( aFolderId );
        }
    }

// ----------------------------------------------------------------------------
// RemoveFolder()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4FolderObserver::RemoveFolder( TMsvId aFolderId )
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderObserver::RemoveFolder" );
    TInt index = iFolders->Find( aFolderId );
    if ( index != KErrNotFound )
        {
        iFolders->Delete( index );
        }
    }

// ----------------------------------------------------------------------------
// ResetFoldersL()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4FolderObserver::ResetFoldersL( CMsvEntrySelection* aFoldersToWatch )
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderObserver::ResetFoldersL" );
    delete iFolders;
    iFolders = NULL;
    iFolders = aFoldersToWatch->CopyL();
    }

// ----------------------------------------------------------------------------
// SetMailbox()
// ----------------------------------------------------------------------------
void CAlwaysOnlineImap4FolderObserver::SetMailbox( const TMsvId aMailboxId )
    {
    AOLOG_IN( "CAlwaysOnlineImap4FolderObserver::SetMailbox" );
    iMailboxId = aMailboxId;
    }
//EOF
