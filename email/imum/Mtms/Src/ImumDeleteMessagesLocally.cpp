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
*     Operation to delete mail messages locally i.e. just remove body and
*     attachments from the flash but leave header. So don't delete message
*     from mail server. After operation user is able to fetch message again

*
*/


// INCLUDE FILES
#include <MTMStore.h>
#include <msvstd.hrh>
#include <mtmuibas.h>
#include <msvuids.h>
#include <imum.rsg>
#include <coemain.h>
#include <StringLoader.h>
#include <eikenv.h>

#include "ImumDeleteMessagesLocally.h"
#include "EmailUtils.H"
#include "ImumMtmLogging.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// minimum disk space needed when deleting messages locally
const TInt KImumDiskSpaceForDelete = (2*1024);

// LOCAL CONSTANTS AND MACROS
const TInt KImumEntriesDoneReplaceIndex = 0;
const TInt KImumTotalEntriesReplaceIndex = 1;

// MODULE DATA STRUCTURES

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CImumDeleteMessagesLocally::CImumDeleteMessagesLocally()
// ----------------------------------------------------------------------------
//
CImumDeleteMessagesLocally::CImumDeleteMessagesLocally(
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    TBool aDiskSpaceRequest )
    :
    CImCacheManager(aMailboxApi.MsvSession(), aObserverRequestStatus),
    iDiskSpaceRequest( aDiskSpaceRequest )
    {
    IMUM_CONTEXT( CImumDeleteMessagesLocally::CImumDeleteMessagesLocally, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    
    }

// ----------------------------------------------------------------------------
// CImumDeleteMessagesLocally::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumDeleteMessagesLocally::ConstructL(
    CMsvEntrySelection& aMessageSelection )
    {
    IMUM_CONTEXT( CImumDeleteMessagesLocally::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    CImCacheManager::ConstructL();
    iMessageSelection = aMessageSelection.CopyL();
    if ( iDiskSpaceRequest )
        {
        CEikonEnv* env = CEikonEnv::Static();

        // Create the file session and get access to current drive
        iFileSession = env->FsSession();
        iDriveNo = iMsvSession.CurrentDriveL();

        // Try to reserve disk space and leave if it fails
        User::LeaveIfError( MsvEmailMtmUiUtils::AcquireDiskSpace(
            iFileSession,
            iDriveNo,
            KImumDiskSpaceForDelete ) );
        }

    TInt resourceId = (iMessageSelection->Count()>1) ?
         R_IMAP4_PROGRESS_DELETING_LOCALLY_MANY : R_IMAP4_PROGRESS_DELETING_LOCALLY;

    iProgressText = StringLoader::LoadL( resourceId );

    StartL( *iMessageSelection, iObserverRequestStatus );
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumDeleteMessagesLocally::NewL()
// ----------------------------------------------------------------------------
//
CImumDeleteMessagesLocally* CImumDeleteMessagesLocally::NewL(
    CMsvEntrySelection& aMessageSelection,
    CImumInternalApi& aMailboxApi,
    TRequestStatus& aObserverRequestStatus,
    TBool aDiskSpaceRequest /* = ETrue */)
    {
    IMUM_STATIC_CONTEXT( CImumDeleteMessagesLocally::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CImumDeleteMessagesLocally* self =
        new ( ELeave ) CImumDeleteMessagesLocally(
            aMailboxApi,
            aObserverRequestStatus,
            aDiskSpaceRequest );

    CleanupStack::PushL(self);
    self->ConstructL( aMessageSelection );
    CleanupStack::Pop();    //  self
    IMUM_OUT();

    return self;
    };


// Destructor
// ----------------------------------------------------------------------------
// CImumDeleteMessagesLocally::~CImumDeleteMessagesLocally()
// ----------------------------------------------------------------------------
//
CImumDeleteMessagesLocally::~CImumDeleteMessagesLocally()
    {
    IMUM_CONTEXT( CImumDeleteMessagesLocally::~CImumDeleteMessagesLocally, 0, KImumMtmLog );
    IMUM_IN();
    
    delete iProgressText;
    delete iMessageSelection;

    if ( iDiskSpaceRequest )
        {
        iFileSession.ReleaseReserveAccess( iDriveNo );
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImumDeleteMessagesLocally::Filter()
// ----------------------------------------------------------------------------
//
TBool CImumDeleteMessagesLocally::Filter() const
    {
    IMUM_CONTEXT( CImumDeleteMessagesLocally::Filter, 0, KImumMtmLog );
    IMUM_IN();
    
    if ( iMessageSelection->Find( iCurrentEntry->Entry().Id() )
        > KErrNotFound )
        {
        IMUM_OUT();
        return ETrue;
        }
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CImumDeleteMessagesLocally::DecodeProgress()
// ----------------------------------------------------------------------------
//
TInt CImumDeleteMessagesLocally::DecodeProgress(const TDesC8& aProgress,
        TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString,
        TInt& aTotalEntryCount, TInt& aEntriesDone,
        TInt& aCurrentEntrySize, TInt& aCurrentBytesTrans, TBool /*aInternal*/)
    {
    IMUM_CONTEXT( CImumDeleteMessagesLocally::DecodeProgress, 0, KImumMtmLog );
    IMUM_IN();
    
    if( !aProgress.Length())
        {
        return KErrNone;
        }

    TPckgBuf<TImCacheManagerProgress> paramPack;
    paramPack.Copy(aProgress);
    const TImCacheManagerProgress& progress=paramPack();

    aEntriesDone=progress.iMessagesProcessed;
    aTotalEntryCount=iMessageSelection->Count();
    aCurrentEntrySize=progress.iTotalMessages;
    aCurrentBytesTrans=progress.iMessagesProcessed;

    aReturnString.Copy( *iProgressText );
    if ( iMessageSelection->Count()>1 )
        {
        TBuf<CBaseMtmUi::EProgressStringMaxLen> tempBuffer;
        StringLoader::Format(
            tempBuffer,
            aReturnString,
            KImumEntriesDoneReplaceIndex,
            aEntriesDone );
        StringLoader::Format(
            aReturnString,
            tempBuffer,
            KImumTotalEntriesReplaceIndex,
            aTotalEntryCount );
        }
    IMUM_OUT();
    return KErrNone;
    }


//  End of File


