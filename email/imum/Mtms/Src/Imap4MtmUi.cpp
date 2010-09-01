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
*       CImap4MtmUi - IMAP4 MTM UI
*
*/


// Standard includes
//
#include <eikenv.h>//CEikonEnv
#include <StringLoader.h>
#include <avkon.mbg>

//
// Messaging includes
//
#include <mtmuidef.hrh>
#include <mtclreg.h>
#include <mtuireg.h>
#include <eikproc.h>
#include <MuiuMsgEditorLauncher.h>
#include <etel.h> // KErrEtelBusyDetected
#include <bldvariant.hrh>
#include <etelpckt.h>
#include <muiuflagger.h>            // CMuiuFlags

#include <ErrorUI.h> // CErrorUI
#include <mtmdef.h>
#include <smtcmtm.h>
#include <miuthdr.h>
#include <MuiuMsvProgressReporterOperation.h>
#include "PROGTYPE.H"
#include "MsvEmailConnectionProgressProvider.h"

// Specific includes
//
#include "impicmds.h"
#include "ImumPanic.h"
#include <imum.rsg>
#include "IMAPPRGR.H"
#include "Imap4MtmUi.h"
#include <imapset.h>
#include <impcmtm.h>
#include "EmailUtils.H"
#include "EmailFeatureUtils.h"
#include "Imap4ConnectedOp.h"
#include "Imap4FetchOp.h"
#include "Imap4SizeAndFetchOp.h"
#include "Imap4FetchAttachmentOp.h"
#include "Imap4CopyMoveLocalOp.h"
#include "Imap4ConnectedDeleteOp.h"
#include "ImumDeleteMessagesLocally.h"
#include "ImumDeleteMessagesFromServer.h"
#include "RefreshImapFolderList.h"
#include <Muiumsginfo.h>
#include <Muiumsginfo.hrh>
#include "ImumMtmLogging.h"
#include "ImumDisconnectOperation.h"
#include "ImumEditOperation.h"
#include "ImumFetchAndEditOperation.h"
#include "EmailEditorViewerUids.h"
#include "MsvConnectionValidation.h"
#include "IMSSettingsNoteUi.h"
#include <ImumInternalApi.h>        // CImumInternalApi
#include <ImumInHealthServices.h>
#include "ImumListQueryDialog.h"

#include <aknnotewrappers.h>  // CAknNoteDialog
#include <MuiuOperationWait.h>
// Constants
const TInt KImumEntriesDoneReplaceIndex = 0;
const TInt KImumTotalEntriesReplaceIndex = 1;
const TInt KImumProgressBufferLengthSafe = CBaseMtmUi::EProgressStringMaxLen - 10; // 10 is enough to fill message numbers
_LIT(KImpuMtmUiResourceFile, "imum.rsc");
const TUint KForwardSlash = '/';
const TUint KBackSlash = '\\';
const TInt KImumDeleteMessagesLocally = 0;
const TInt KImumMessageInfoBuffer = 32;
const TInt KImumMessagePriorityBufferLength = 32;
const TInt KImumApproxMailFetchSize = 1000;
const TInt KErrDndNameNotFound = -5120;
_LIT( KEmptySpace, " " );

enum TImap4MtmUiOperations
    {
    EImap4MtmUiEditing=EUiProgTypeEditing
    };

// ----------------------------------------------------------------------------
// CImap4MtmUi::NewIMPUMtmUiL()
// ----------------------------------------------------------------------------
//
EXPORT_C CBaseMtmUi* NewIMPUMtmUiL(
    CBaseMtm& aMtm,
    CRegisteredMtmDll& aRegisteredDll )
    {
    return CImap4MtmUi::NewL(aMtm, aRegisteredDll);
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::NewL()
// ----------------------------------------------------------------------------
//
CImap4MtmUi* CImap4MtmUi::NewL(
    CBaseMtm& aBaseMtm,
    CRegisteredMtmDll& aRegisteredMtmDll )
    {
    IMUM_STATIC_CONTEXT( CImap4MtmUi::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();

    CImap4MtmUi* self=new(ELeave) CImap4MtmUi(aBaseMtm, aRegisteredMtmDll);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    IMUM_OUT();
    return self;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::CImap4MtmUi()
// ----------------------------------------------------------------------------
//
CImap4MtmUi::CImap4MtmUi(CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll)
    :
    CImumMtmBaseMtmUi( aBaseMtm, aRegisteredMtmDll )
    {
    IMUM_CONTEXT( CImap4MtmUi::CImap4MtmUi, 0, KImumMtmLog );
    IMUM_IN();

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::ConstructL()
// ----------------------------------------------------------------------------
//
void CImap4MtmUi::ConstructL()
    {
    IMUM_CONTEXT( CImap4MtmUi::ConstructL, 0, KImumMtmLog );
    IMUM_IN();

    CImumMtmBaseMtmUi::ConstructL();
    iEntry = CMsvEntry::NewL(
        Session(), KMsvRootIndexEntryId,
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByNone, ETrue ) );
    iEikonEnv->ReadResourceL( iInboxName, R_INBOX_NAME );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::~CImap4MtmUi()
// ----------------------------------------------------------------------------
//
CImap4MtmUi::~CImap4MtmUi()
    {
    IMUM_CONTEXT( CImap4MtmUi::~CImap4MtmUi, 0, KImumMtmLog );
    IMUM_IN();

    delete iEditorFilename;
    delete iViewerFilename;
    delete iEntry;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::GetEditorFileNameL()
// ----------------------------------------------------------------------------
//
HBufC* CImap4MtmUi::GetEditorFileNameL()
    {
    IMUM_CONTEXT( CImap4MtmUi::GetEditorFileNameL, 0, KImumMtmLog );
    IMUM_IN();

    if(!iEditorFilename)
        {
        iEditorFilename = MsvUiEditorUtilities::ResolveAppFileNameL(KUidMsgInternetMailEditor);
        }
    IMUM_OUT();
    return iEditorFilename;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::GetViewerFileNameL()
// ----------------------------------------------------------------------------
//
HBufC* CImap4MtmUi::GetViewerFileNameL()
    {
    IMUM_CONTEXT( CImap4MtmUi::GetViewerFileNameL, 0, KImumMtmLog );
    IMUM_IN();

    if(!iViewerFilename)
        {
        iViewerFilename = MsvUiEditorUtilities::ResolveAppFileNameL(KUidMsgInternetMailViewer);
        }
    IMUM_OUT();
    return iViewerFilename;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::CreateL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::CreateL(
    const TMsvEntry& /*aEntry*/,
    CMsvEntry& /*aParent*/,
    TRequestStatus& /*aStatus*/ )
    {
    IMUM_CONTEXT( CImap4MtmUi::CreateL, 0, KImumMtmLog );
    IMUM_IN();

    User::Leave(KErrNotSupported);
    IMUM_OUT();
    return NULL;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::OpenL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::OpenL( TRequestStatus& aStatus )
    {
    IMUM_CONTEXT( CImap4MtmUi::OpenL, 0, KImumMtmLog );
    IMUM_IN();

    const TMsvEntry& contextEntry=BaseMtm().Entry().Entry();
    CheckEntryL(contextEntry);
    if (contextEntry.iType == KUidMsvMessageEntry || contextEntry.iType == KUidMsvFolderEntry)
        return EditL(aStatus);
    else if (contextEntry.iType != KUidMsvServiceEntry)
        User::Leave(KErrNotSupported);
    //
    // --- Trying to open a service ---

    const TMsvId owningService=BaseMtm().Entry().OwningService();
    const TInt serviceState=ServiceState(owningService);
    if(!ServiceIsLoaded(serviceState) || ServiceIsDisconnected(serviceState))
        {
        CMsvEntrySelection* sel=new(ELeave) CMsvEntrySelection;
        CleanupStack::PushL(sel);
        sel->AppendL(contextEntry.Id());
        //  Use the UI side connection wrapping operation
        TBuf8<1> dummyParams;

        CMsvOperation* op=InvokeAsyncFunctionL(
            KImpiMTMConnectAndSyncCompleteAfterDisconnection,
            *sel, aStatus, dummyParams);
        CleanupStack::PopAndDestroy();//selection
        IMUM_OUT();
        return op;
        }
    else
        {
        IMUM_OUT();
        //  Can't connect if we're not disconnected
        return CompletedImap4OperationL(owningService, aStatus);
        }
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::OpenL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::OpenL(
    TRequestStatus& aStatus,
    const CMsvEntrySelection& aSelection )
    {
    IMUM_CONTEXT( CImap4MtmUi::OpenL, 0, KImumMtmLog );
    IMUM_IN();

    CheckSelectionL(aSelection);
    IMUM_OUT();
    //
    // --- Edit the message selection ---
    return EditL(aStatus, aSelection);
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::CloseL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::CloseL( TRequestStatus& aStatus )
    {
    IMUM_CONTEXT( CImap4MtmUi::CloseL, 0, KImumMtmLog );
    IMUM_IN();

    const TMsvEntry& contextEntry=BaseMtm().Entry().Entry();
    CheckEntryL(contextEntry);
    // --- Check that current context is a service, all other entry types cannot be closed ---
    if (contextEntry.iType!=KUidMsvServiceEntry)
        User::Leave(KErrNotSupported);

    const TMsvId owningServiceId=BaseMtm().Entry().OwningService();
    TInt serviceState=ServiceState(owningServiceId);
    CMsvOperation* op=NULL;
    if ( !ServiceIsDisconnected(serviceState) && !ServiceIsDisconnecting(serviceState) )
        {
        //  Not disconnected or disconnecting, so try to close service
        CMsvProgressReporterOperation* reporter =
            CMsvProgressReporterOperation::NewL(
                ETrue,
                ETrue,
                Session(),
                aStatus,
                EMbmAvkonQgn_note_progress );
        CleanupStack::PushL(reporter);
        op = CImumDisconnectOperation::NewL(*iMailboxApi, reporter->RequestStatus(), *reporter, contextEntry.Id(), KIMAP4MTMDisconnect, KUidMsgTypeIMAP4);
        reporter->SetOperationL(op); // Takes immediate ownership
        CleanupStack::Pop();    // reporter
        op = reporter;
        }
    IMUM_OUT();
    return op?op:CompletedImap4OperationL(owningServiceId, aStatus);
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::CloseL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::CloseL(
    TRequestStatus& /*aStatus*/,
    const CMsvEntrySelection& /*aSelection*/ )
    {
    IMUM_CONTEXT( CImap4MtmUi::CloseL, 0, KImumMtmLog );
    IMUM_IN();

    // --- Multiple close is not supported ---
    User::Leave(KErrNotSupported);
    IMUM_OUT();
    return NULL;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::WithinIMAPInboxL()
// ----------------------------------------------------------------------------
//
TBool CImap4MtmUi::WithinIMAPInboxL( TMsvId aEntryId )
    {
    IMUM_CONTEXT( CImap4MtmUi::WithinIMAPInboxL, 0, KImumMtmLog );
    IMUM_IN();

    if (aEntryId==KMsvRootIndexEntryId)
        return EFalse;
    TBool withinInbox=EFalse;

    CMsvEntry* myEntry=Session().GetEntryL(aEntryId);
    CleanupStack::PushL(myEntry);

    if (!(myEntry->OwningService()==KMsvLocalServiceIndexEntryId))
        //  We're not under the local service
        while (!withinInbox && myEntry->Entry().iType!=KUidMsvServiceEntry)
            {
            withinInbox = ( ( myEntry->Entry().iType==KUidMsvFolderEntry ) &&
                ( myEntry->Entry().iDetails.CompareF(iInboxName) == 0 ) );
            myEntry->SetEntryL(myEntry->Entry().Parent());
            }

    CleanupStack::PopAndDestroy();  //  myEntry
    IMUM_OUT();
    return withinInbox;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::CompletedImap4OperationL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::CompletedImap4OperationL(
    TMsvId aServiceId,
    TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CImap4MtmUi::CompletedImap4OperationL, 0, KImumMtmLog );
    IMUM_IN();

    TPckgBuf<TImap4UiProgress> newProgressBuf;
    IMUM_OUT();
    return CMsvCompletedOperation::NewL(Imap4BaseMtm()->Session(), Type(), newProgressBuf, aServiceId, aStatus, KErrNone);// KErrNone because we've reported the fact to the user
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::EditL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::EditL( TRequestStatus& aStatus )
    {
    IMUM_CONTEXT( CImap4MtmUi::EditL, 0, KImumMtmLog );
    IMUM_IN();

    CMsvOperation* op=NULL;
    const TMsvEntry& contextEntry=BaseMtm().Entry().Entry();
    CheckEntryL(contextEntry);

    switch (contextEntry.iType.iUid)
        {
    case KUidMsvServiceEntryValue:
        if ( !iMailboxApi->HealthServicesL().IsMailboxHealthy(
            contextEntry.Id() ) )
            {
            User::Leave( KErrNotFound );
            }
        op=EditImap4ServiceL( aStatus, contextEntry.iRelatedId );
        break;
    case KUidMsvMessageEntryValue:
    case KUidMsvFolderEntryValue:
        op=EditL(aStatus, *SingleEntrySelectionLC(contextEntry.Id()));
        CleanupStack::PopAndDestroy();
        break;
    default:
        User::Leave(KErrNotSupported);
        }
    IMUM_OUT();

    return op?op:CompletedImap4OperationL(BaseMtm().Entry().OwningService(), aStatus);
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::EditL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::EditL(
    TRequestStatus& aStatus,
    const CMsvEntrySelection& aSelection )
    {
    IMUM_CONTEXT( CImap4MtmUi::EditL, 0, KImumMtmLog );
    IMUM_IN();

    CMsvOperation* op=NULL;

    iEntry->SetEntryL(aSelection[0]);
    const TMsvId owningService=iEntry->OwningService();

    TMsvEmailEntry tentry=iEntry->Entry();

    switch(iEntry->Entry().iType.iUid)
        {
    case KUidMsvServiceEntryValue:
        BaseMtm().SwitchCurrentEntryL(tentry.Id());
        op=EditL(aStatus);
        break;
    case KUidMsvMessageEntryValue:
        {
        if ( tentry.DisconnectedOperation() == EDisconnectedDeleteOperation )
            {
            // remove deleted flag
            TInt error = MsvEmailMtmUiUtils::RemoveOfflineDeleteL(
                BaseMtm(),
                KIMAP4MTMUndeleteAll,
                tentry.Id() );
            User::LeaveIfError( error );
            }

        if(tentry.iRelatedId==KMsvNullIndexEntryId)
            //  The entry is not a ghost - operate on it directly
            //
            if(tentry.BodyTextComplete())
                {
                op=LaunchMultipleEditorApplicationsL(aSelection, aStatus);
                }
            else
                {
                TEditorParameters editorParams;
                editorParams.iId = aSelection[0];
                editorParams.iFlags &= ~(EMsgLaunchEditorEmbedded | EMsgLaunchEditorThenWait);
                editorParams.iFlags |= EMsgReadOnly;
                const TUint preferences = Preferences();
                if(preferences & EMtmUiFlagEditorPreferEmbedded)
                    {
                    editorParams.iFlags |= EMsgLaunchEditorEmbedded;
                    }
                if(!(preferences & EMtmUiFlagEditorNoWaitForExit))
                    {
                    editorParams.iFlags |= EMsgLaunchEditorThenWait;
                    }

                op = OpenMessageL(aStatus, editorParams);
                }
        else
            {
            //  The entry is a ghost entry - we need to follow the
            //  iRelatedId back to the originator to determine what
            //  we need to do.
            iEntry->SetEntryL(tentry.iRelatedId);
            tentry=iEntry->Entry();
            if(tentry.BodyTextComplete())
                {
                CMsvEntrySelection* mySelection=SingleEntrySelectionLC(tentry.Id());
                op=LaunchMultipleEditorApplicationsL(*mySelection, aStatus);
                CleanupStack::PopAndDestroy();  //  mySelection
                }
            else
                {
                op=OfferSynchroniseL(owningService,aStatus);
                }
            }
        }
        break;
    case KUidMsvFolderEntryValue:
    default:
        User::Leave(KErrNotSupported);
        }
    IMUM_OUT();
    return op?op:CompletedImap4OperationL(owningService, aStatus);
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::ViewL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::ViewL( TRequestStatus& aStatus )
    {
    IMUM_CONTEXT( CImap4MtmUi::ViewL, 0, KImumMtmLog );
    IMUM_IN();

    CMsvOperation* op = NULL;
    TMsvEmailEntry context=BaseMtm().Entry().Entry();
    if (context.iType != KUidMsvMessageEntry)
        User::Leave(KErrNotSupported);

    if(context.iRelatedId==KMsvNullIndexEntryId)
        //  This entry is not a ghost entry - operate on it directly.
        if(context.Complete())
            {
            op=LaunchEditorApplicationL(aStatus);
            }
        else
            {
            TEditorParameters editorParams;
            editorParams.iId = context.Id();
            editorParams.iFlags &= ~(EMsgLaunchEditorEmbedded | EMsgLaunchEditorThenWait);
            editorParams.iFlags |= EMsgReadOnly;
            const TUint preferences = Preferences();
            if(preferences & EMtmUiFlagEditorPreferEmbedded)
                {
                editorParams.iFlags |= EMsgLaunchEditorEmbedded;
                }
            if(!(preferences & EMtmUiFlagEditorNoWaitForExit))
                {
                editorParams.iFlags |= EMsgLaunchEditorThenWait;
                }
            op = OpenMessageL(aStatus, editorParams);
            }
    else
        {
        //  The context is a ghost entry - follow the iRelatedId
        //  back to the originating entry
        BaseMtm().SwitchCurrentEntryL(context.iRelatedId);
        if(BaseMtm().Entry().Entry().Complete())
            op=LaunchEditorApplicationL(aStatus);
        else
            op=OfferSynchroniseL(BaseMtm().Entry().OwningService(),aStatus);
        }
    IMUM_OUT();

    return op;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::ViewL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::ViewL(
    TRequestStatus& aStatus,
    const CMsvEntrySelection& aSelection)
    {
    IMUM_CONTEXT( CImap4MtmUi::ViewL, 0, KImumMtmLog );
    IMUM_IN();

    __ASSERT_DEBUG(aSelection.Count()>0,
        User::Panic(KImumMtmUiPanic,EIMAP4MtmUiSelectionIsEmpty));
    BaseMtm().Entry().SetEntryL(aSelection[0]);
    IMUM_OUT();
    return ViewL(aStatus);
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::CopyToL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::CopyToL(
    const CMsvEntrySelection& /*aSelection*/,
    TRequestStatus& /*aStatus*/)
    {
    IMUM_CONTEXT( CImap4MtmUi::CopyToL, 0, KImumMtmLog );

    __ASSERT_DEBUG( EFalse,
        User::Panic(KImumMtmUiPanic, EIMAP4MtmUiCopyToNotSupported ) );
    User::Leave(KErrNotSupported);
    IMUM_OUT();
    return NULL;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::MoveToL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::MoveToL(
    const CMsvEntrySelection& /*aSelection*/,
    TRequestStatus& /*aStatus*/)
    {
    IMUM_CONTEXT( CImap4MtmUi::MoveToL, 0, KImumMtmLog );
    IMUM_IN();

    __ASSERT_DEBUG( EFalse,
        User::Panic(KImumMtmUiPanic,EIMAP4MtmUiMoveToNotSupported ));
    User::Leave(KErrNotSupported);
    IMUM_OUT();
    return NULL;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::CopyFromL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::CopyFromL(
    const CMsvEntrySelection& /*aSelection*/,
    TMsvId /*aTargetId*/,
    TRequestStatus& /*aStatus*/ )
    {
    IMUM_CONTEXT( CImap4MtmUi::CopyFromL, 0, KImumMtmLog );
    IMUM_IN();

    __ASSERT_DEBUG( EFalse,
        User::Panic( KImumMtmUiPanic,EIMAP4MtmUiCopyFromNotSupported ) );
    User::Leave( KErrNotSupported );
    IMUM_OUT();
    return NULL;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::MoveFromL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::MoveFromL(
    const CMsvEntrySelection& /*aSelection*/,
    TMsvId /*aTargetId*/,
    TRequestStatus& /*aStatus*/)
    {
    IMUM_CONTEXT( CImap4MtmUi::MoveFromL, 0, KImumMtmLog );
    IMUM_IN();

    // Context should be MTM folder/service to move from
    __ASSERT_DEBUG( EFalse,
        User::Panic(KImumMtmUiPanic,EIMAP4MtmUiMoveFromNotSupported ) );
    User::Leave(KErrNotSupported);
    IMUM_OUT();
    return NULL;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::CancelL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::CancelL(
    TRequestStatus& /*aStatus*/,
    const CMsvEntrySelection& /*aSelection*/)
    {
    IMUM_CONTEXT( CImap4MtmUi::CancelL, 0, KImumMtmLog );
    IMUM_IN();

    // Currently this function only used to suspend messages queued for sending.
    // Can't send IMAP4 messages, so can't cancel them either.
    User::Leave(KErrNotSupported);
    IMUM_OUT();
    return NULL;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::QueryCapability()
// Reporting on which commands are supported
// ----------------------------------------------------------------------------
//
TInt CImap4MtmUi::QueryCapability( TUid aCapability, TInt& aResponse )
    {
    IMUM_CONTEXT( CImap4MtmUi::QueryCapability, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();

    return CBaseMtmUi::QueryCapability(aCapability, aResponse);
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::ServiceState()
// ----------------------------------------------------------------------------
//
TInt CImap4MtmUi::ServiceState( TMsvId aServiceId ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::ServiceState, 0, KImumMtmLog );
    IMUM_IN();

    //  Inline functions are provided to test the returned status flags
    //
    TPckgBuf<TImap4CompoundProgress> myProgressBuf;
    const TInt error=BaseMtm().Session().ServiceProgress(aServiceId, myProgressBuf);

    TInt statusFlags=0;
    if(!error)  //  NB. If error, all the flags should be returned unset
        {
        TImap4GenericProgress::TImap4GenericProgressState genericProgressState=myProgressBuf().iGenericProgress.iState;
        TImap4SyncProgress::TImap4SyncProgressState syncProgressState=myProgressBuf().iSyncProgress.iState;

        statusFlags|=
            (genericProgressState==TImap4GenericProgress::EIdle &&
            syncProgressState==TImap4SyncProgress::EIdle)?EIsIdle:0;

        statusFlags|=
            (genericProgressState==TImap4GenericProgress::EDisconnected)?EIsDisconnected:0;

        statusFlags|=
            (genericProgressState==TImap4GenericProgress::EConnecting)?EIsConnecting:0;

        statusFlags|=
            (genericProgressState==TImap4GenericProgress::EDisconnecting)?EIsDisconnecting:0;

        statusFlags|=EIsLoaded; //  Must be loaded to have been able to get the progress
        }
    else
        //  Service is not loaded, we can set the disconnected flag accordingly
        statusFlags|=EIsDisconnected;
    IMUM_OUT();
    return statusFlags;
    }


// ----------------------------------------------------------------------------
// CImap4MtmUi::IsConnected()
// ----------------------------------------------------------------------------
//
TBool CImap4MtmUi::IsConnected( const TMsvId aService ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::IsConnected, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    return(!ServiceIsDisconnected(ServiceState(aService)));
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::InvokeAsyncFunctionL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::InvokeAsyncFunctionL(
                TInt aFunctionId,
                const CMsvEntrySelection& aSelection,
                TRequestStatus& aStatus,
                TDes8& aParameter )
    {
    IMUM_CONTEXT( CImap4MtmUi::InvokeAsyncFunctionL, 0, KImumMtmLog );
    IMUM_IN();

    CMsvOperation* op = NULL;

    //Offline checks VARIATION START
    if( iFeatureFlags->GF( EMailFeatureOfflineMode ) &&
        aFunctionId != KMtmUiFunctionMessageInfo )
        {
        TMsvId mailboxId = BaseMtm().Entry().EntryId();
        //No need to do this if we're online...
        TBool offline = EFalse;
        if( !IsConnected( mailboxId ) &&
            !iMailboxApi->MailboxUtilitiesL().HasWlanConnectionL( mailboxId ) )
            {
            offline = MsvEmailMtmUiUtils::DoOfflineChecksL( mailboxId );
            }

        if( offline )
            {
            op = CMsvCompletedOperation::NewL(
                Session(), Type(), KNullDesC8,
                KMsvLocalServiceIndexEntryId, aStatus, KErrCancel );
            return op;
            }
        }//if
    //VARIATION END

    switch( aFunctionId )
        {
    case KMtmUiFunctionFetchAll:    // 'Fetch All'
    case KMtmUiFunctionFetchNew:    // 'Fetch New'
        {
        // aSelection is empty.
        TImImap4GetMailInfo getMailInfo;
        CMsvEntrySelection* sel = new ( ELeave ) CMsvEntrySelection;
        CleanupStack::PushL( sel );
        InitParametersForFetchCmdL( sel, getMailInfo );
        if( aFunctionId == KMtmUiFunctionFetchAll )
            {
            aFunctionId = KIMAP4MTMPopulateAllMailWhenAlreadyConnected;
            }
        else
            {
            aFunctionId = KIMAP4MTMPopulateNewMailWhenAlreadyConnected;
            }
        op = FetchMessagesL( aStatus, *sel, aFunctionId, getMailInfo );
        CleanupStack::PopAndDestroy(); // sel
        }
        break;

    case KMtmUiFunctionFetchSelected:   // 'Fetch Selected'
        // aSelection contains the messages to populate.
        op = PopulateMessagesL( aStatus, aSelection );
        break;

    case KImpiMTMConnect:
    case KImpiMTMConnectAndSyncCompleteAfterConnection:
    case KImpiMTMConnectAndSyncCompleteAfterFolderSync:
    case KImpiMTMConnectAndSyncCompleteAfterDisconnection:
    case KImpiMTMRefreshFolderList:
        {
        if( aFunctionId != KImpiMTMRefreshFolderList )
            {
            TImImap4GetPartialMailInfo partialGetMailInfo;
            CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
            CleanupStack::PushL( sel );

            InitParametersForFilteredPopulateL( *sel, partialGetMailInfo );
            if( partialGetMailInfo.iPartialMailOptions != ENoSizeLimits )
                {
                //if partial fetch is used, we connect this way
                op = DoFilteredPopulateL(aStatus, *sel, aFunctionId, partialGetMailInfo );
                CleanupStack::PopAndDestroy(); //sel
                break;
                }
            else
                {
                CleanupStack::PopAndDestroy(); //sel
                }
            }//if

        //  All function IDs requiring a progress reporter op
        CMsvProgressReporterOperation* myProgressReporterOp=
            CMsvProgressReporterOperation::NewL(
                ETrue, ETrue, Session(),
                aStatus, EMbmAvkonQgn_note_progress );

        CleanupStack::PushL( myProgressReporterOp );
        CMsvOperation* wrappedOp = NULL;

        if( aFunctionId != KImpiMTMRefreshFolderList )
            {
            //  Must be some flavour of connection op

            __ASSERT_DEBUG( aSelection.Count(),
                User::Panic(KImumMtmUiPanic, EIMAP4MtmUiNoService ) );
            if( aSelection.Count() == 0 )
                {
                op = CMsvCompletedOperation::NewL(
                        Session(), Type(), KNullDesC8,
                        KMsvLocalServiceIndexEntryId, aStatus, KErrCancel );
                break;
                }

            iEntry->SetEntryL( aSelection[ 0 ] );
            const TMsvId service = iEntry->OwningService();

            CImapConnectionOp::TImapConnectionType connectionCase =
                CImapConnectionOp::EWithSyncCompleteAfterDisconnection;
            switch( aFunctionId )
                {
            case KImpiMTMConnect:
                connectionCase = CImapConnectionOp::ECompleteAfterConnect;
                break;
            case KImpiMTMConnectAndSyncCompleteAfterConnection:
                connectionCase = CImapConnectionOp::EWithSyncCompleteAfterConnect;
                break;
            case KImpiMTMConnectAndSyncCompleteAfterFolderSync:
                connectionCase = CImapConnectionOp::EWithSyncCompleteAfterFullSync;
                break;
            default:
                break;
                }

            wrappedOp = CImapConnectionOp::NewL(
                iMailboxApi->MsvSession(), CActive::EPriorityStandard,
                myProgressReporterOp->RequestStatus(),      // ie. complete the reporter when finished
                service, *myProgressReporterOp, // the reporter owns the wrapped op
                connectionCase );

            myProgressReporterOp->SetOperationL( wrappedOp ); // Takes immediate ownership

            TBuf<EProgressStringMaxLen> title;
            MsvEmailMtmUiUtils::CreateConnectingToText( title,
                Session(), BaseMtm().Entry().EntryId() );

            myProgressReporterOp->SetTitleL( title );
            }

        else
            {
            //note text for imap folderlist update
            HBufC* title = StringLoader::LoadLC(
                R_IMAS_SETTINGS_DIALOG_UPDATING_FOLDERLIST, iCoeEnv );

            myProgressReporterOp->SetTitleL( *title );
            wrappedOp = CImapConnectAndRefreshFolderList::NewL(
                *iMailboxApi, CActive::EPriorityStandard,
                myProgressReporterOp->RequestStatus(), aSelection[ 0 ],
                *myProgressReporterOp );
            myProgressReporterOp->SetOperationL( wrappedOp ); //  Takes immediate ownership
            CleanupStack::PopAndDestroy( title );
            }
        CleanupStack::Pop();    //  myProgressReporterOp
        op = myProgressReporterOp;
        }
        break;
    case KImpiMTMFetchAttachments:
        {
        __ASSERT_DEBUG( aSelection.Count(),
            User::Panic(KImumMtmUiPanic, EIMAP4MtmUiSelectionIsEmpty ) );
        if ( aSelection.Count() > 0 )
            {
            iEntry->SetEntryL( aSelection[ 0 ] );
            const TMsvId service = iEntry->OwningService();
            CMsvProgressReporterOperation* reporter =
                CMsvProgressReporterOperation::NewL(
                    Session(), aStatus, EMbmAvkonQgn_note_progress );
            CleanupStack::PushL( reporter );
            op = CImap4FetchAttachmentOp::NewL(
                *iMailboxApi, reporter->RequestStatus(),
                *reporter, service, aSelection );
            reporter->SetOperationL( op );    //  Takes immediate ownership
            CleanupStack::Pop();    //  reporter
            op = reporter;
            }
        else
            {
            op = CMsvCompletedOperation::NewL(
                Session(), Type(), KNullDesC8,
                KMsvLocalServiceIndexEntryId, aStatus, KErrCancel );
            }
        }
        break;

    case KMtmUiFunctionMessageInfo:
        IMUM_OUT();
        return ShowMessageInfoL( aStatus, aParameter );

    case KIMAP4MTMConnectAndSyncCompleteAfterFullSync:
    default:
        op=BaseMtm().InvokeAsyncFunctionL(
            aFunctionId, aSelection,
            aParameter, aStatus );
        break;
        }
    IMUM_OUT();
    return op;
    }


// ----------------------------------------------------------------------------
// InitParametersForFilteredPopulateL
// ----------------------------------------------------------------------------
//
void CImap4MtmUi::InitParametersForFilteredPopulateL(
    CMsvEntrySelection& aSelection,
    TImImap4GetPartialMailInfo& aPartialGetMailInfo)
        {
        CMsvEntrySelection* serviceChildren = BaseMtm().Entry().ChildrenL();
        CleanupStack::PushL(serviceChildren);

        aSelection.AppendL( BaseMtm().Entry().Entry().Id() );

        //add inbox to selection

        const TMsvId inboxId = SelectionContainsInboxL(BaseMtm().Entry(),*serviceChildren);
        CleanupStack::PopAndDestroy();

        aSelection.AppendL( inboxId );

        //read settings
        Imap4BaseMtm()->RestoreSettingsL();
        const CImImap4Settings& settings = Imap4BaseMtm()->Imap4Settings();

        aPartialGetMailInfo.iPartialMailOptions = settings.PartialMailOptions();
        aPartialGetMailInfo.iBodyTextSizeLimit = settings.BodyTextSizeLimit();
        aPartialGetMailInfo.iAttachmentSizeLimit = settings.AttachmentSizeLimit();
        aPartialGetMailInfo.iTotalSizeLimit = aPartialGetMailInfo.iBodyTextSizeLimit +
                                            aPartialGetMailInfo.iAttachmentSizeLimit;
        }

// ----------------------------------------------------------------------------
// DoFilteredPopulateL
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::DoFilteredPopulateL(
    TRequestStatus& aStatus,
    const CMsvEntrySelection& aSelection,
    TInt aFunctionId,
    TImImap4GetPartialMailInfo& aPartialGetMailInfo)
    {
    IMUM_CONTEXT( CImap4MtmUi::InitParametersForFilteredPopulateL, 0, KImumMtmLog );
    IMUM_IN();

    CMsvProgressReporterOperation* reporter =
        CMsvProgressReporterOperation::NewL(
            Session(), aStatus, EMbmAvkonQgn_note_progress );
    CleanupStack::PushL(reporter);
    const TMsvId service = BaseMtm().Entry().OwningService();

    // Set func id as null, because we're only connecting, not
    // fetching anything. ( fetching is done internally
    // by fetchop, because we're populating )
    CMsvOperation* fetch = CImap4FetchOp::NewL(
        *iMailboxApi, reporter->RequestStatus(), *reporter,
        service, aFunctionId, aPartialGetMailInfo, aSelection );
    reporter->SetOperationL(fetch); // Takes immediate ownership
    CleanupStack::Pop(); // reporter
    IMUM_OUT();
    return reporter;
    }

// ----------------------------------------------------------------------------
// InvokeSyncFunctionL
// ----------------------------------------------------------------------------
//
void CImap4MtmUi::InvokeSyncFunctionL(
    TInt aFunctionId,
    const CMsvEntrySelection& aSelection,
    TDes8& aParameter )
    {
    IMUM_CONTEXT( CImap4MtmUi::InvokeSyncFunctionL, 0, KImumMtmLog );
    IMUM_IN();

    CBaseMtmUi::InvokeSyncFunctionL(aFunctionId, aSelection, aParameter);
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::PopulateMessagesSyncL()
// ----------------------------------------------------------------------------
//
TInt CImap4MtmUi::PopulateMessagesSyncL( const CMsvEntrySelection& aSelection )
    {
    IMUM_CONTEXT( CImap4MtmUi::PopulateMessagesSyncL, 0, KImumMtmLog );
    IMUM_IN();

    CMsvEntrySelection* sel = StripBodyTextCompleteEntriesLC(aSelection);
    const TInt count = sel->Count();
    CleanupStack::PopAndDestroy(); // sel
    if(!count)
        {
        IMUM_OUT();
        // Have body text.
        return KErrNone;
        }

    CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
    waiter->iStatus = KRequestPending;
    CMsvOperation* op = PopulateMessagesL(waiter->iStatus, aSelection);
    CleanupStack::PushL(op);

    waiter->Start();
    CActiveScheduler::Start();
    const TInt err = DisplayProgressSummary(op->ProgressL());
    CleanupStack::PopAndDestroy(2); // CSI: 47 # op, waiter
    IMUM_OUT();
    return err;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::GetResourceFileName()
// ----------------------------------------------------------------------------
//
void CImap4MtmUi::GetResourceFileName( TFileName& aFileName ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::GetResourceFileName, 0, KImumMtmLog );
    IMUM_IN();

    aFileName=KImpuMtmUiResourceFile;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::GetProgress()
// ----------------------------------------------------------------------------
//
TInt CImap4MtmUi::GetProgress(
    const TDesC8& aProgress,
    TBuf<EProgressStringMaxLen>& aReturnString,
    TInt& aTotalEntryCount,
    TInt& aEntriesDone,
    TInt& aCurrentEntrySize,
    TInt& aCurrentBytesTrans ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::GetProgress, 0, KImumMtmLog );
    IMUM_IN();

    if( (!aProgress.Length()) || (aProgress.Size() == sizeof(TMsvLocalOperationProgress)) )
        return KErrNone;

    TPckgC<TInt> type(0);
    type.Set(aProgress.Left(sizeof(TInt)));
    switch (type())
        {
    case KImap4UiProgressType:
        //  IMAP specific UI buffer
        return GetUiProgress(aProgress, aReturnString, aTotalEntryCount, aEntriesDone, aCurrentEntrySize, aCurrentBytesTrans);

    case EUiProgTypeSizingMsg:
        // Email message sizer operation progress - we don't decode this
        // Fall-through
    case EUiProgTypeEditing:
        //  Email editing progress - we don't decode this
        //  Fall-through
    case EUiProgTypeConnecting:
        //  Email connection progress - we don't decode this
        return KErrNone;

    case EImap4SyncProgressType:
        return GetSyncProgress(aProgress, aReturnString, aTotalEntryCount, aEntriesDone, aCurrentEntrySize, aCurrentBytesTrans);
        //  Fall-through
    default:
        break;
        }

    //  If none of the above are true, then we should be getting a TImap4CompoundProgress,
    //  of which we should just decode the GenericProgress part (unless we are in the middle of
    //  a sync operation)
    __ASSERT_DEBUG(type() == EImap4GenericProgressType,
        User::Panic(KImumMtmUiPanic,EIMAP4MtmUiInvalidProgress));

    TPckgBuf<TImap4CompoundProgress> paramPack;
    paramPack.Copy(aProgress);
    const TImap4GenericProgress& progress=paramPack().iGenericProgress;
    TInt sliderTotal=0;
    TInt sliderNumber=0;
    TInt itemTotal=progress.iMsgsToDo;
    TInt itemNumber=Min(itemTotal, progress.iMsgsDone+1);

    TInt resourceId=0;
    switch(progress.iOperation)
        {
        case TImap4GenericProgress::ECopyWithinService:
        case TImap4GenericProgress::EMoveWithinService:
            __ASSERT_DEBUG(EFalse,
                User::Panic(KImumMtmUiPanic,EIMAP4MtmUiProgressMoveCopyWithinServiceNotSupported));
        break;
        case TImap4GenericProgress::ECopyToLocal:
        case TImap4GenericProgress::EOffLineCopyToLocal:
        case TImap4GenericProgress::EOffLineCopyFromLocal:
        case TImap4GenericProgress::EOffLineCopyWithinService:
            resourceId = (itemTotal==1) ? (R_IMAP4_PROGRESS_COPYING) : (R_IMAP4_PROGRESS_COPYING_MULTI);
            if(itemTotal==1)
                {
                sliderTotal=progress.iBytesToDo;
                sliderNumber=progress.iBytesDone;
                }
            else
                {
                sliderTotal=progress.iMsgsToDo;
                sliderNumber=progress.iMsgsDone;
                }
            break;

        case TImap4GenericProgress::EPopulate:
        case TImap4GenericProgress::EOffLinePopulate:
            {
            resourceId = (itemTotal==1) ? (R_IMAP4_PROGRESS_FETCHING) : (R_IMAP4_PROGRESS_FETCHING_MULTI);
            //We don't want slider to be displayed. When no progress info, wait animation is shown instead.
            sliderTotal=0;
			sliderNumber=0;
            }
            break;
        case TImap4GenericProgress::EOffLineUndelete:
            {
            resourceId = (itemTotal==1) ? (R_IMAP4_PROGRESS_UNDELETING) : (R_IMAP4_PROGRESS_UNDELETING_MANY);
            }
            break;
        case TImap4GenericProgress::ESync:
            {
            TPckg<TImap4SyncProgress> syncProgress(paramPack().iSyncProgress);
            return GetSyncProgress(syncProgress, aReturnString, aTotalEntryCount, aEntriesDone, aCurrentEntrySize, aCurrentBytesTrans);
            }

        default:
            {
            switch(STATIC_CAST(TInt,progress.iState))
                {
                case TImap4GenericProgress::EConnecting:
                case TImap4GenericProgress::EDisconnecting:
                case TImap4GenericProgress::EDisconnected:
                case TImap4GenericProgress::EIdle:
                case TImap4GenericProgress::ESelecting:
                    //  No progress string to be returned for these,
                    //  inc. no slider
                    break;
                case TImap4GenericProgress::EBusy:
                case TImap4GenericProgress::ESyncing:
                    sliderTotal=itemTotal;
                    sliderNumber=progress.iMsgsDone;
                    resourceId=R_IMAP4_PROGRESS_INBOXSYNC;
                    break;
                case TImap4GenericProgress::EDeleting:
                    if( (itemTotal == 0) && (itemNumber == 0) )
                        {
                        // IMPS does not report counts for folder deletions, but does for messages.
                        resourceId=R_IMAP4_PROGRESS_DELETING;
                        }
                    else
                        {
                        resourceId = (itemTotal==1) ? (R_IMAP4_PROGRESS_DELETING) : (R_IMAP4_PROGRESS_DELETING_MANY);
                        }
                    break;
                case TImap4GenericProgress::EFetching:
                case TImap4GenericProgress::EPopulateNewMail:
                case TImap4GenericProgress::EPopulateAllMail:
                case TImap4GenericProgress::EPopulateMailSelection:
                    {
                    resourceId = (itemTotal==1) ? (R_IMAP4_PROGRESS_FETCHING) : (R_IMAP4_PROGRESS_FETCHING_MULTI);
                    sliderTotal=progress.iBytesToDo;
                    sliderNumber=progress.iBytesDone;
                    }
                    break;
                case TImap4GenericProgress::EAppending:
                    __ASSERT_DEBUG(EFalse,
                        User::Panic(KImumMtmUiPanic,EIMAP4MtmUiProgressAppendingNotSupported));
                    sliderTotal=progress.iBytesToDo;
                    sliderNumber=progress.iBytesDone;
                    break;
                case TImap4GenericProgress::EMoveNewMail:
                case TImap4GenericProgress::EMoving:
                case TImap4GenericProgress::EMoveMailSelection:
                case TImap4GenericProgress::EMoveAllMail:
                    __ASSERT_DEBUG(EFalse,
                        User::Panic(KImumMtmUiPanic,EIMAP4MtmUiProgressMoveNotSupported));
                    // drop through
                case TImap4GenericProgress::ECopying:
                case TImap4GenericProgress::ECopyNewMail:
                case TImap4GenericProgress::ECopyMailSelection:
                case TImap4GenericProgress::ECopyAllMail:
                    resourceId = (itemTotal==1) ? (R_IMAP4_PROGRESS_COPYING) : (R_IMAP4_PROGRESS_COPYING_MULTI);
                    sliderTotal=progress.iBytesToDo;
                    sliderNumber=progress.iBytesDone;
                    break;
                case EImap4MtmUiEditing:
                    break;
                default:
                    __ASSERT_DEBUG(EFalse,
                        User::Panic(KImumMtmUiPanic,EIMAP4MtmUiInvalidProgress));
                    break;
                };  // end ofinner switch case
            };  //  end of outer switch's default
        };  // end of outer switch


	if ( progress.iOperation == TImap4GenericProgress::EPopulate )
		{
		//We don't want slider to be displayed. When no progress info, wait animation is shown instead.
		aEntriesDone=0;
	    aTotalEntryCount=0;
	    aCurrentEntrySize=0;
	    aCurrentBytesTrans=0;
		}
	else
		{
		aEntriesDone=itemNumber;
	    aTotalEntryCount=itemTotal;
	    aCurrentEntrySize=sliderTotal;
	    aCurrentBytesTrans=sliderNumber;
		}

    if (resourceId!=0)
        {
        StringLoader::Load( aReturnString, resourceId, iCoeEnv);
        if ( itemTotal>1 )
            {
            aReturnString = aReturnString.Left( KImumProgressBufferLengthSafe );
            TBuf<EProgressStringMaxLen> tempBuffer;
            StringLoader::Format(
                tempBuffer,
                aReturnString,
                KImumEntriesDoneReplaceIndex,
                itemNumber );
            StringLoader::Format(
                aReturnString,
                tempBuffer,
                KImumTotalEntriesReplaceIndex,
                itemTotal );
            }
        }
    else
        {
        aReturnString.Zero();
        }
    IMUM_OUT();

    return progress.iErrorCode;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::GetSyncProgress()
// ----------------------------------------------------------------------------
//
TInt CImap4MtmUi::GetSyncProgress(
    const TDesC8& aProgress,
    TBuf<EProgressStringMaxLen>& aReturnString,
    TInt& aTotalEntryCount,
    TInt& aEntriesDone,
    TInt& aCurrentEntrySize,
    TInt& aCurrentBytesTrans ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::GetSyncProgress, 0, KImumMtmLog );
    IMUM_IN();

    TPckgBuf<TImap4SyncProgress> progressPack;
    progressPack.Copy(aProgress);
    const TImap4SyncProgress& progress=progressPack();

    TInt sliderTotal=0;
    TInt sliderNumber=0;
    TInt itemTotal=progress.iFoldersToDo;
    TInt itemNumber=Min(itemTotal, progress.iFoldersDone);

    TInt resourceId=0;

    switch(progress.iState)
        {
    case TImap4SyncProgress::EIdle:
    case TImap4SyncProgress::EConnecting:
    case TImap4SyncProgress::EDisconnecting:
    case TImap4SyncProgress::ESyncFolderTree:
        //  No progress string to be returned for these states.
        break;
    case TImap4SyncProgress::ESyncInbox:
        resourceId=R_IMAP4_PROGRESS_INBOXSYNC;
        sliderTotal=progress.iMsgsToDo;
        sliderNumber=progress.iMsgsDone;
        break;
    case TImap4SyncProgress::ECheckRemoteSubscription:
    case TImap4SyncProgress::EUpdateRemoteSubscription:
        __ASSERT_DEBUG(EFalse,
            User::Panic(KImumMtmUiPanic,EIMAP4MtmUiSyncProgressFolder));
        sliderTotal=0;
        sliderNumber=0;
        itemTotal=progress.iFoldersToDo;
        itemNumber=progress.iFoldersDone;
        break;
    case TImap4SyncProgress::ESyncOther:
    case TImap4SyncProgress::EBusy:
    case TImap4SyncProgress::EDeleting:
    case TImap4SyncProgress::EProcessingPendingOps:
        resourceId=R_IMAP4_PROGRESS_INBOXSYNC;
        sliderTotal=progress.iFoldersToDo;
        sliderNumber=progress.iFoldersDone;
        itemTotal=progress.iFoldersToDo;
        itemNumber=progress.iFoldersDone;
        break;
    default:
        break;
        }

    aEntriesDone=sliderNumber;
    aTotalEntryCount=sliderTotal;
    aCurrentEntrySize=sliderTotal;
    aCurrentBytesTrans=sliderNumber;

    if (resourceId!=0)
        {
        StringLoader::Load( aReturnString, resourceId, iCoeEnv);
        }
    else
        {
        aReturnString.Zero();
        }
    IMUM_OUT();

    return progress.iErrorCode;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::GetUiProgress()
// ----------------------------------------------------------------------------
//
TInt CImap4MtmUi::GetUiProgress(
    const TDesC8& aProgress,
    TBuf<EProgressStringMaxLen>& aReturnString,
    TInt& aTotalEntryCount,
    TInt& aEntriesDone,
    TInt& aCurrentEntrySize,
    TInt& aCurrentBytesTrans ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::GetUiProgress, 0, KImumMtmLog );
    IMUM_IN();

    TPckgBuf<TImap4UiProgress> paramPack;
    paramPack.Copy(aProgress);
    const TImap4UiProgress& progress=paramPack();

    TInt resourceId=0;

    switch(progress.iUiOperation)
        {
        case TImap4UiProgress::ESynchronising:
            __ASSERT_DEBUG(EFalse,
                User::Panic(KImumMtmUiPanic,EIMAP4MtmUiUiProgressSync));
            break;
        case TImap4UiProgress::ECreatingFolder:
            __ASSERT_DEBUG(EFalse,
                User::Panic(KImumMtmUiPanic,EIMAP4MtmUiUiProgressCreateFolder));
            break;
        case TImap4UiProgress::EFetchingAttachment:
            resourceId=R_IMAP4_GETTING_ATTACHMENT;
        default:
            //  NB. the "default" category includes purging
            //  attachments which should happen very quickly and
            //  not require any progress during it's activity
            break;
        }

    aEntriesDone=progress.iFoldersDone;
    aTotalEntryCount=progress.iFoldersToDo;
    aCurrentEntrySize=progress.iBytesToDo;
    aCurrentBytesTrans=progress.iBytesDone;
    if (resourceId==0)
        {
        aReturnString.Zero();
        }
    else
        {
        StringLoader::Load( aReturnString, resourceId, iCoeEnv);
        };
    IMUM_OUT();

    return progress.iError;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::DisplayProgressSummary()
// ----------------------------------------------------------------------------
//
TInt CImap4MtmUi::DisplayProgressSummary( const TDesC8& aProgress ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::DisplayProgressSummary, 0, KImumMtmLog );
    IMUM_IN();

    if ( ( !aProgress.Length() ) ||
         ( aProgress.Size() == sizeof(TMsvLocalOperationProgress) ) )
        return KErrCancel; // Probably a CMsvCompletedOperation

    TPckgC<TInt> type(0);
    type.Set( aProgress.Left( sizeof(TInt) ) );
    switch( type() )
        {
    case KImap4UiProgressType:
        IMUM_OUT();
        return DisplayUiProgressSummary( aProgress );

    case EUiProgTypeEditing:
        // Email message editing/viewing progress, ignore errors.
        //  Fall through...
    case EUiProgTypeSizingMsg:
        // Email message sizer operation progress, ignore errors.
        IMUM_OUT();
        return KErrNone;
    case EUiProgTypeConnecting:
        //  Email connection progress
        IMUM_OUT();
        return DisplayConnectionProgressSummary( aProgress );

    case EImap4SyncProgressType:
        IMUM_OUT();
        return DisplaySyncProgressSummary( aProgress );

    default:
        break;
        }

    //  Should be a TImap4CompoundProgress
    __ASSERT_DEBUG( type() == EImap4GenericProgressType,
        User::Panic(KImumMtmUiPanic, EIMAP4MtmUiInvalidProgress ) );

    TPckgBuf<TImap4CompoundProgress> paramPack;
    paramPack.Copy( aProgress );
    const TImap4CompoundProgress& progress = paramPack();
    const TImap4GenericProgress& genericProgress = progress.iGenericProgress;
    const TImap4SyncProgress& syncProgress = progress.iSyncProgress;

    TBool displayErrorNote = EFalse;
    TInt messageRID = 0;
    TInt errorCode = KErrNone;
    if ( genericProgress.iOperation == TImap4GenericProgress::EConnect ||
         genericProgress.iOperation == TImap4GenericProgress::EDisconnect ||
         genericProgress.iOperation == TImap4GenericProgress::ESync )
        {
        //  We take the progress from the sync progress part
        errorCode = syncProgress.iErrorCode;
        if ( errorCode == KErrNone )
            {
            // Belt and braces approach; check generic progress for errors too.
            errorCode = genericProgress.iErrorCode;
            }
        if ( errorCode != KErrNone )
            {
            // There was an error.
            switch ( genericProgress.iOperation )
                {
            case TImap4GenericProgress::EConnect:
                //  If we are connecting, we should discount KErrCancels
                //  Also KErrNotSupported
                if( ( errorCode != KErrCancel ) &&
                    ( errorCode != KErrNotSupported ) )
                    {
                    displayErrorNote = ETrue;
                    }
                break;
            case TImap4GenericProgress::EDisconnect:
                if ( errorCode == KErrTimedOut )
                    {
                    displayErrorNote = ETrue;
                    messageRID = R_IMAP4_TIMEOUT_DIALOG_TEXT;
                    }
                else
                    {
                    displayErrorNote = ETrue;
                    }
                break;
            case TImap4GenericProgress::ESync:
                displayErrorNote = ETrue;
                //  Fall through
            default:
                break;
                };
            }
        }
    else    //  We take the progress from the generic progress part
        {
        errorCode=genericProgress.iErrorCode;
        if ( errorCode == KErrNone )
            {
            // Belt and braces approach; check sync progress for errors too.
            errorCode=syncProgress.iErrorCode;
            }
        if ( errorCode != KErrNone && errorCode != KErrCancel )
            {
            // There was an error.
            switch ( genericProgress.iOperation )
                {
            case TImap4GenericProgress::EPopulate:
            case TImap4GenericProgress::ECopyToLocal:
            case TImap4GenericProgress::ECopyWithinService:
            case TImap4GenericProgress::ECopyFromLocal:
            case TImap4GenericProgress::EMoveToLocal:
            case TImap4GenericProgress::EMoveWithinService:
            case TImap4GenericProgress::EMoveFromLocal:
                displayErrorNote = ETrue;
                break;
            default:
                break;
                }
            }
        }
    if ( displayErrorNote )
        {
        IMUM_OUT();
        // messageRID can be 0 and then error text is received from error resolver
        return ShowErrorDialog( errorCode, messageRID );
        }
    IMUM_OUT();
    return errorCode;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::DisplaySyncProgressSummary()
// ----------------------------------------------------------------------------
//
TInt CImap4MtmUi::DisplaySyncProgressSummary( const TDesC8& aProgress ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::DisplaySyncProgressSummary, 0, KImumMtmLog );
    IMUM_IN();

    //  Sync progress is only returned by a connection operation
    TPckgBuf<TImap4SyncProgress> paramPack;
    paramPack.Copy( aProgress );
    const TImap4SyncProgress& syncProgress = paramPack();

    TBool displayErrorNote = EFalse;
    TInt messageRID = 0;

    if ( syncProgress.iErrorCode == KErrNone )
        {
        IMUM_OUT();
        return KErrNone;
        }
    else
        {
        switch ( syncProgress.iState )
            {
            case TImap4SyncProgress::EConnecting:
                if ( syncProgress.iErrorCode == KErrTimedOut )
                    {
                    displayErrorNote = ETrue;
                    messageRID=R_IMAP4_TIMEOUT_DIALOG_TEXT;
                    }
                else if ( syncProgress.iErrorCode == KErrDndNameNotFound )
                    {
                    displayErrorNote = ETrue;
                    messageRID = R_MAIL_ERR_NO_IN_SRVR;
                    }
                else if ( (syncProgress.iErrorCode != KErrCancel ) &&
                         ( syncProgress.iErrorCode != KErrNotSupported ) )
                    //  If we are connecting, we should discount KErrCancels
                    //  Also KErrNotSupported
                    {
                    displayErrorNote = ETrue;
                    }
                break;
            case TImap4SyncProgress::EDisconnecting:
                displayErrorNote = ETrue;
                if ( syncProgress.iErrorCode == KErrTimedOut )
                    {
                    messageRID = R_IMAP4_TIMEOUT_DIALOG_TEXT;
                    }
                break;
            case TImap4SyncProgress::EIdle:
                displayErrorNote = ETrue;
                messageRID=R_IMAP4_TIMEOUT_DIALOG_TEXT;
                break;
            default:
                //  All the rest of the sync states are related to syncing
                displayErrorNote = ETrue;
            break;
            };
        }
    IMUM_OUT();

    return ( displayErrorNote ) ?
        ShowErrorDialog( syncProgress.iErrorCode, messageRID ) :
        syncProgress.iErrorCode;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::ShowErrorDialog()
// ----------------------------------------------------------------------------
//
TInt CImap4MtmUi::ShowErrorDialog(
    TInt aErrorCode,
    TInt aErrorMessageRID ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::ShowErrorDialog, 0, KImumMtmLog );
    IMUM_IN();

    TInt ignore = 0;
    TInt ret = KErrNone;
    TBuf<EProgressStringMaxLen> errorBufferText;
    const TDesC* line = NULL;
    if(!aErrorMessageRID)
        {
        if ( aErrorCode == KErrGeneral ||
             aErrorCode == KErrEtelBusyDetected ||
             aErrorCode == KErrAbort )
            // general errors displayed by other components. for ex. phone app...
            {
            return KErrGeneral;
            }

        if ( aErrorCode == KErrCouldNotConnect )
            {
            aErrorCode = KErrImapConnectFail;
            }
        if( aErrorCode == KErrGprsServicesNotAllowed )
            {
            aErrorCode = KImskErrorActiveSettingIsDifferent;
            }
        ret = aErrorCode;
        TRAP( ignore, iErrorResolver->ShowGlobalErrorNoteL( aErrorCode ) );
        }
    else if ( aErrorMessageRID == R_IMAP4_TIMEOUT_DIALOG_TEXT )
        {
        TBuf<EProgressStringMaxLen> tempString;
        StringLoader::Load( tempString, aErrorMessageRID, iEikonEnv);
        TMsvId serviceId = 0;
        TMsvId id = BaseMtm().Entry().OwningService();
        TMsvEntry serviceEntry;
        if ( Session().GetEntry(
            id,
            serviceId,
            serviceEntry ) == KErrNone )
            {
            TInt descriptionLength = EProgressStringMaxLen - tempString.Length();
            if ( descriptionLength < 1 )
                {
                descriptionLength = EProgressStringMaxLen - KImumProgressBufferLengthSafe;
                tempString = tempString.Left( KImumProgressBufferLengthSafe );
                }
            StringLoader::Format(
                errorBufferText,
                tempString,
                -1,
                serviceEntry.iDetails.Left(descriptionLength) );
            }
        else
            {
            StringLoader::Format( errorBufferText, tempString, -1, KEmptySpace );
            }
        line = &errorBufferText;
        ret = KErrTimedOut;
        TRAP( ignore, CIMSSettingsNoteUi::ShowNoteL(
            *line, EIMSErrorNote, ETrue ) );
        }
    else
        {
        StringLoader::Load( errorBufferText, aErrorMessageRID, iEikonEnv);
        line = &errorBufferText;
        ret = KErrCancel; // some other error, just return != KErrNone
        TRAP_IGNORE( CIMSSettingsNoteUi::ShowQueryL(
            *line, R_EMAIL_INFORMATION_QUERY ) );
        }
    IMUM_OUT();

    return ret;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::DisplayUiProgressSummary()
// ----------------------------------------------------------------------------
//
TInt CImap4MtmUi::DisplayUiProgressSummary( const TDesC8& aProgress ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::DisplayUiProgressSummary, 0, KImumMtmLog );
    IMUM_IN();

    TPckgBuf<TImap4UiProgress> paramPack;
    paramPack.Copy(aProgress);
    const TImap4UiProgress& progress=paramPack();

    if( (progress.iError != KErrNone) && (progress.iError != KErrCancel) )
        {
        //  Only failed creation or renaming of folders requires summary
        // and you cannot do those so panic
        switch(progress.iUiOperation)
            {
            case TImap4UiProgress::ECreatingFolder:
            case TImap4UiProgress::ERenamingFolder:
                __ASSERT_DEBUG(EFalse,
                    User::Panic(KImumMtmUiPanic,EIMAP4MtmUiUiProgressSummaryCreateRenameFolder));
                break;

            case TImap4UiProgress::EDeletingMessages:
                return ShowErrorDialog(progress.iError);

            default:
                break;
            }
        }
    IMUM_OUT();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::DisplayConnectionProgressSummary()
// ----------------------------------------------------------------------------
//
TInt CImap4MtmUi::DisplayConnectionProgressSummary(
    const TDesC8& aProgress) const
    {
    IMUM_CONTEXT( CImap4MtmUi::DisplayConnectionProgressSummary, 0, KImumMtmLog );
    IMUM_IN();

    TPckgBuf<TMsvEmailConnectionProgress> progressBuf;
    progressBuf.Copy(aProgress);
    const TMsvEmailConnectionProgress& connectionProgress=progressBuf();
    IMUM_OUT();

    if(connectionProgress.iErrorCode!=KErrNone && connectionProgress.iErrorCode!=KErrCancel)
        {
        return ShowErrorDialog(connectionProgress.iErrorCode);
        }
    else
        {
        //  If we are connecting, we should discount KErrCancels
        return KErrNone;
        }
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::LaunchMultipleEditorApplicationsL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::LaunchMultipleEditorApplicationsL(
    const CMsvEntrySelection& aSelection,
    TRequestStatus& aStatus )
    {
    IMUM_CONTEXT( CImap4MtmUi::LaunchMultipleEditorApplicationsL, 0, KImumMtmLog );
    IMUM_IN();

    BaseMtm().Entry().SetEntryL(aSelection.At(0));
    IMUM_OUT();
    return LaunchEditorApplicationL(aStatus);
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::LaunchEditorApplicationL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::LaunchEditorApplicationL(
    TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CImap4MtmUi::LaunchEditorApplicationL, 0, KImumMtmLog );
    IMUM_IN();

    // --- Create the parameters necessary for the editor ---
    TEditorParameters editorParams;
    editorParams.iId=BaseMtm().Entry().EntryId();
    editorParams.iFlags|=EMsgReadOnly;
    IMUM_OUT();
    return LaunchEditorApplicationL(aStatus, editorParams);
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::LaunchEditorApplicationL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::LaunchEditorApplicationL(
    TRequestStatus& aStatus,
    const TEditorParameters& aParams)
    {
    IMUM_CONTEXT( CImap4MtmUi::LaunchEditorApplicationL, 0, KImumMtmLog );
    IMUM_IN();

    // Get the app name, appName not owed so must not delete it at the end
    HBufC* appName = (aParams.iFlags & EMsgReadOnly) ? (GetViewerFileNameL()) : (GetEditorFileNameL());
    const TUint preferences = Preferences();

    TEmailEditParams editParams;
    editParams.iPreferences = preferences;
    editParams.iEditorFileName = *appName;
    editParams.iEditorParams = aParams;

    editParams.iEditorParams.iFlags &= ~(EMsgLaunchEditorEmbedded | EMsgLaunchEditorThenWait);
    if(preferences & EMtmUiFlagEditorPreferEmbedded)
        {
        editParams.iEditorParams.iFlags |= EMsgLaunchEditorEmbedded;
        }
    if(!(preferences & EMtmUiFlagEditorNoWaitForExit))
        {
        editParams.iEditorParams.iFlags |= EMsgLaunchEditorThenWait;
        }
    const TBool ackReceipts = AcknowledgeReceiptsL(aParams.iId);
    IMUM_OUT();
    return CImumEditOperation::NewL(*iMailboxApi, aStatus, Type(), editParams, ackReceipts);
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::EditImap4ServiceL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::EditImap4ServiceL(
    TRequestStatus& aStatus,
    TMsvId aServiceEntry )
    {
    IMUM_CONTEXT( CImap4MtmUi::EditImap4ServiceL, 0, KImumMtmLog );
    IMUM_IN();

    // Get SMTP client and ui dlls
    CClientMtmRegistry* clientRegistry=CClientMtmRegistry::NewL(Session());
    CleanupStack::PushL(clientRegistry);

    CBaseMtm* smtpmtm=clientRegistry->NewMtmL(KUidMsgTypeSMTP);
    CleanupStack::PushL(smtpmtm);

    CMtmUiRegistry* mtmuireg=CMtmUiRegistry::NewL(Session());
    CleanupStack::PushL(mtmuireg);

    CBaseMtmUi* smtpUi=mtmuireg->NewMtmUiL(*smtpmtm);
    CleanupStack::PushL(smtpUi);

    smtpUi->BaseMtm().SetCurrentEntryL(Session().GetEntryL(aServiceEntry));

    TRAPD(ret, STATIC_CAST(CSmtpClientMtm&,smtpUi->BaseMtm()).RestoreSettingsL());
    if (ret==KErrNotFound)
        STATIC_CAST(CSmtpClientMtm&,smtpUi->BaseMtm()).StoreSettingsL();
    else
        User::LeaveIfError(ret);

    CMsvOperation* op=smtpUi->EditL(aStatus);
    CleanupStack::PopAndDestroy(4); // CSI: 47 # smtpui, mtmuireg, smtpmtm, clientregistry
    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::CheckEntryL()
// ----------------------------------------------------------------------------
//
void CImap4MtmUi::CheckEntryL( const TMsvEntry& aEntry ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::CheckEntryL, 0, KImumMtmLog );
    IMUM_IN();

    if ((aEntry.iType!=KUidMsvMessageEntry && aEntry.iType!=KUidMsvServiceEntry && aEntry.iType!=KUidMsvFolderEntry) || aEntry.iMtm!=Type())
        User::Leave(KErrNotSupported);
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::CheckSelectionL()
// ----------------------------------------------------------------------------
//
void CImap4MtmUi::CheckSelectionL(
    const CMsvEntrySelection& aSelection ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::CheckSelectionL, 0, KImumMtmLog );
    IMUM_IN();

    __ASSERT_DEBUG(aSelection.Count(),
        User::Panic(KImumMtmUiPanic,EIMAP4MtmUiSelectionIsEmpty));
    if ( aSelection.Count() == 0 )
        {
        User::Leave(KErrNotSupported);
        }
    iEntry->SetEntryL(aSelection[0]);
    iEntry->SetEntryL(iEntry->Entry().Parent());
    TMsvEntry tentry;
    for (TInt cc=aSelection.Count(); --cc>=0;)
        {
        tentry=iEntry->ChildDataL(aSelection[cc]);
        if (tentry.iType!=KUidMsvMessageEntry || tentry.iMtm!=Type())
            {
            User::Leave(KErrNotSupported);
            }
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::SingleEntrySelectionLC()
// ----------------------------------------------------------------------------
//
CMsvEntrySelection* CImap4MtmUi::SingleEntrySelectionLC(
    TMsvId aId ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::SingleEntrySelectionLC, 0, KImumMtmLog );
    IMUM_IN();

    CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
    CleanupStack::PushL(selection);
    selection->AppendL(aId);
    IMUM_OUT();
    return selection;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::StripInvalidEntriesLC()
// ----------------------------------------------------------------------------
//
CMsvEntrySelection* CImap4MtmUi::StripInvalidEntriesLC(
    const CMsvEntrySelection& aSelection ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::StripInvalidEntriesLC, 0, KImumMtmLog );
    IMUM_IN();

    CMsvEntrySelection* entries=aSelection.CopyLC();
    CMsvEntrySelection* contextChildren=BaseMtm().Entry().ChildrenL();
    CleanupStack::PushL(contextChildren);
    TInt cc=entries->Count();
    while (cc--)
        {
        if (contextChildren->Find((*entries)[cc]) == KErrNotFound)
            entries->Delete(cc);
        }
    CleanupStack::PopAndDestroy();
    IMUM_OUT();
    return entries;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::StripBodyTextCompleteEntriesLC()
// ----------------------------------------------------------------------------
//
CMsvEntrySelection* CImap4MtmUi::StripBodyTextCompleteEntriesLC(
    const CMsvEntrySelection& aSelection ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::StripBodyTextCompleteEntriesLC, 0, KImumMtmLog );
    IMUM_IN();

    CMsvEntrySelection* entries=aSelection.CopyLC();
    iEntry->SetEntryL((*entries)[0]);
    iEntry->SetEntryL(iEntry->Entry().Parent());
    TInt cc=entries->Count();
    while (cc--)
        {
        TMsvEmailEntry emailEntry(iEntry->ChildDataL((*entries)[cc]));
        if(emailEntry.BodyTextComplete())
            {
            entries->Delete(cc);
            }
        }
    IMUM_OUT();
    return entries;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::StripCompleteEntriesLC()
// ----------------------------------------------------------------------------
//
CMsvEntrySelection* CImap4MtmUi::StripCompleteEntriesLC(
    const CMsvEntrySelection& aSelection ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::StripCompleteEntriesLC, 0, KImumMtmLog );
    IMUM_IN();

    iEntry->SetEntryL(aSelection[0]);
    iEntry->SetEntryL(iEntry->Entry().Parent());
    IMUM_OUT();
    return MsvEmailMtmUiUtils::StripCompleteEntriesLC(*iEntry, aSelection);
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::ValidateFolderNameL()
// ----------------------------------------------------------------------------
//
void CImap4MtmUi::ValidateFolderNameL(
    const TDesC& aName,
    TDes8& aRetProgress )
    {
    IMUM_CONTEXT( CImap4MtmUi::ValidateFolderNameL, 0, KImumMtmLog );
    IMUM_IN();

    Imap4BaseMtm()->RestoreSettingsL(); //  To recover the imap settings
    const TChar separatorChar = Imap4BaseMtm()->Imap4Settings().PathSeparator();

    TImap4UiProgressBuf& retProgress = STATIC_CAST(TImap4UiProgressBuf&, aRetProgress);
    retProgress().iError = KErrNone;
    const TChar forwardSlash(KForwardSlash);
    const TChar backSlash(KBackSlash);
    if(!aName.Length())
        {
        retProgress().iError = KErrPathNotFound;
        }
    else if(aName.Length() > KImpuIMAPFolderNameLength)
        {
        retProgress().iError = KErrTooBig;
        }
    else if( ((separatorChar == forwardSlash || separatorChar == backSlash) &&
                        ((aName.Locate(forwardSlash) >= 0) || (aName.Locate(backSlash) >= 0))) ||
                    (aName.Locate(separatorChar) >= 0) )
        {
        retProgress().iSeparatorChar = separatorChar;
        retProgress().iError = KErrBadName;
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::DeleteFoldersFromL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::DeleteFoldersFromL(
    const CMsvEntrySelection& /*aSelection*/,
    TRequestStatus& /*aStatus*/,
    TMsvId /*aService*/,
    TBool /*aOnline*/)
    {
    IMUM_CONTEXT( CImap4MtmUi::DeleteFoldersFromL, 0, KImumMtmLog );
    IMUM_IN();

    // not supported
    User::Leave(KErrNotSupported);
    IMUM_OUT();
    return NULL;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::SelectionContainsInboxL()
// ----------------------------------------------------------------------------
//
TMsvId CImap4MtmUi::SelectionContainsInboxL(
    const CMsvEntry& aParentEntry,
    const CMsvEntrySelection& aSelection ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::SelectionContainsInboxL, 0, KImumMtmLog );
    IMUM_IN();

    if (aParentEntry.Entry().iType.iUid!=KUidMsvServiceEntryValue)
        return KMsvNullIndexEntryId;
    for (TInt cc=aSelection.Count(); --cc>=0;)
        {
        const TMsvEntry& tentry = aParentEntry.ChildDataL(aSelection[cc]);
        if (tentry.iDetails.CompareF(iInboxName)==0)
            return tentry.Id();
        }
    IMUM_OUT();
    return KMsvNullIndexEntryId;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::SelectionContainsGhostEntryL()
// ----------------------------------------------------------------------------
//
TBool CImap4MtmUi::SelectionContainsGhostEntryL(
    const CMsvEntry& aParentEntry,
    const CMsvEntrySelection& aSelection )
    {
    IMUM_CONTEXT( CImap4MtmUi::SelectionContainsGhostEntryL, 0, KImumMtmLog );
    IMUM_IN();

    if(aSelection.Count()==0)
        {
        return EFalse;
        }

    for(TInt cc=aSelection.Count(); --cc>=0;)
        {
        if(aParentEntry.ChildDataL(aSelection[cc]).iRelatedId!=KMsvNullIndexEntryId)
            {
            IMUM_OUT();
            return ETrue;
            }
        }
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::SelectionContainsOnlyMessagesL()
// ----------------------------------------------------------------------------
//
TBool CImap4MtmUi::SelectionContainsOnlyMessagesL(
    const CMsvEntry& aParentEntry,
    const CMsvEntrySelection& aSelection ) const
    {
    IMUM_CONTEXT( CImap4MtmUi::SelectionContainsOnlyMessagesL, 0, KImumMtmLog );
    IMUM_IN();

    for(TInt cc=aSelection.Count() ; --cc>=0;)
        {
        if(aParentEntry.ChildDataL(aSelection[cc]).iType!=KUidMsvMessageEntry)
            {
            IMUM_OUT();
            return EFalse;
            }
        }
    IMUM_OUT();
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::DeleteFromL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::DeleteFromL(
    const CMsvEntrySelection& aSelection,
    TRequestStatus& aStatus )
    {
    IMUM_CONTEXT( CImap4MtmUi::DeleteFromL, 0, KImumMtmLog );
    IMUM_IN();

    TBuf8<1> buf;
    CMsvOperation* op=NULL;

    // Validate selection.
    __ASSERT_DEBUG(aSelection.Count(),
        User::Panic(KImumMtmUiPanic,EIMAP4MtmUiSelectionIsEmpty));
    CMsvEntrySelection* validEntries=StripInvalidEntriesLC(aSelection);
    CMsvEntrySelection* entries=MsvEmailMtmUiUtils::StripDeletedEntriesLC(BaseMtm().Entry(), *validEntries);    // Current context is parent.
    const TBool hasInbox = (SelectionContainsInboxL(BaseMtm().Entry(),*entries) != KMsvNullIndexEntryId);
    if( (!entries->Count()) || (hasInbox) )
        {
        IMUM2(0, "Cancelling delete for reasons: %d or %d", entries->Count(), hasInbox );
        CleanupStack::PopAndDestroy(2); // CSI: 47 # entries,validEntries
        IMUM_OUT();
        return CMsvCompletedOperation::NewL(Session(), Type(), buf, KMsvLocalServiceIndexEntryId, aStatus, KErrNone);
        }

    // Remove folders from the selection.
    iEntry->SetEntryL((*entries)[0]);
    const TMsvId parent(iEntry->Entry().Parent());
    iEntry->SetEntryL(parent);
    TInt numberOfItems = entries->Count();
    while (numberOfItems--)
        {
        if(iEntry->ChildDataL((*entries)[numberOfItems]).iType == KUidMsvFolderEntry)
            {
            // remove folders from selection, we can only delete messages
            entries->Delete(numberOfItems);
            }
        }
    numberOfItems = entries->Count();

    TInt chosedDelMode = KErrNotFound;
    const TInt delMode = GetMailDeletionModeL();
    IMUM1(0, "Selected deletion mode: %d", delMode );

    TBool dlgResult = ETrue;

    if ( delMode == EIMASMailDeletionAlwaysAsk )
        {
        IMUM0(0, "Deleting: Always Ask" );
        CImumListQueryDialog* dlg = new (ELeave) CImumListQueryDialog( &chosedDelMode );
        dlg->PrepareLC( R_IMUM_DELETE_LIST_QUERY );

        if ( numberOfItems > 1 )
            {
            // change list query title
            HBufC* manyMailsText = StringLoader::LoadLC(
                R_IMUM_DELETE_LIST_QUERY_TITLE_MANY_MAILS, numberOfItems, iEikonEnv );

            CAknPopupHeadingPane* heading = dlg->QueryHeading();
            heading->SetTextL( *manyMailsText );
            CleanupStack::PopAndDestroy( manyMailsText );
            }

        IMUM0(0, "Running dialog" );
        dlgResult = dlg->RunLD();
        }
    else if ( numberOfItems > 1 )
        {

        HBufC* queryText = NULL;

        if ( delMode == EIMASMailDeletionPhone )
            {
            queryText = StringLoader::LoadLC(
                    R_IMUM_DELETE_MANY_MAILS_PHONE_QUERY, numberOfItems, iEikonEnv );
            }
        else
            {
            queryText = StringLoader::LoadLC(
                    R_IMUM_DELETE_MANY_MAILS_SERVER_QUERY, numberOfItems, iEikonEnv );
            }

        dlgResult = CIMSSettingsNoteUi::ShowQueryL(
               *queryText, R_EMAIL_CONFIRMATION_QUERY );

        CleanupStack::PopAndDestroy( queryText );
        }

    if ( delMode == EIMASMailDeletionPhone )
        {
        chosedDelMode = KImumDeleteMessagesLocally;
        }


    if( dlgResult )
        {
        CMsvProgressReporterOperation* reporter =
                CMsvProgressReporterOperation::NewL(
                    Session(), aStatus, EMbmAvkonQgn_note_erased );
        CleanupStack::PushL(reporter);


        if ( chosedDelMode == KImumDeleteMessagesLocally )
            {
            // Show a note to make sure that user understands that header will still be seen
            // even if mail is deleted from the phone, only showed if setting is always ask
            if ( delMode == EIMASMailDeletionAlwaysAsk )
                {
                // Set info note timeout to 1 second. Otherwise progress
                // note will come on top of info note. CAknProgressDialog
                // lead time before displaying the dialog is hardcoded to
                // 1 second and cannot be adjusted.
                const TInt KTimeout = 1000000;
                CIMSSettingsNoteUi::ShowNoteL( R_IMUM_HEADER_WILL_REMAIN_PHONE,
                    EIMSInformationNote, EFalse, KTimeout );
                }

            CImumDeleteMessagesLocally* deleteop =
                CImumDeleteMessagesLocally::NewL(
                *entries,
                *iMailboxApi,
                reporter->iStatus );
            reporter->SetProgressDecoder( *deleteop );
            reporter->SetOperationL( deleteop ); // Takes immediate ownership
            }
        else
            {
            CImumDeleteMessagesFromServer* deleteop =
                CImumDeleteMessagesFromServer::NewL(
                *iMailboxApi,
                reporter->iStatus,
                *entries );
            reporter->SetProgressDecoder( *deleteop );
            reporter->SetOperationL( deleteop ); // Takes immediate ownership
            }

        CleanupStack::Pop(); //reporter
        op = reporter;
        }
    else
        {
        op=CMsvCompletedOperation::NewL(
            Session(),
            Type(),
            buf,
            KMsvLocalServiceIndexEntryId,
            aStatus,
            KErrCancel);
        }

    CleanupStack::PopAndDestroy(2); // CSI: 47 # entries,validEntries

    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::UnDeleteFromL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::UnDeleteFromL(
    const CMsvEntrySelection& aSelection,
    TRequestStatus& aStatus )
    {
    IMUM_CONTEXT( CImap4MtmUi::UnDeleteFromL, 0, KImumMtmLog );
    IMUM_IN();

    //Un-deletes selection from current context

    //  We shouldn't need to check for folders - they are always deleted
    //  immediately on-line, so should never be in a position to be
    //  un-deleted.
    TBool undelete = EFalse;
    if ( aSelection.Count() > 1 )
        {
        HBufC* promptText = StringLoader::LoadLC(
            R_EMAIL_UNDELETE_MANY_MESSAGES_QUERY, aSelection.Count(), iCoeEnv );
        undelete = CIMSSettingsNoteUi::ShowQueryL(
            *promptText, R_EMAIL_CONFIRMATION_QUERY );
        CleanupStack::PopAndDestroy( promptText );
        }
    else
        {
        undelete = CIMSSettingsNoteUi::ShowQueryL(
            R_EMAIL_UNDELETE_MESSAGE_QUERY, R_EMAIL_CONFIRMATION_QUERY );
        }
    if ( undelete )
        {
        TBuf8<1> dummyParam;
        CMsvProgressReporterOperation* reporter = CMsvProgressReporterOperation::NewL(Session(), aStatus, EMbmAvkonQgn_note_progress);
        CleanupStack::PushL(reporter);
        CMsvOperation* op = BaseMtm().InvokeAsyncFunctionL(KIMAP4MTMUndeleteAll,aSelection,dummyParam,reporter->RequestStatus());
        reporter->SetOperationL(op); // Takes immediate ownership
        CleanupStack::Pop(); // reporter
        IMUM_OUT();
        return reporter;
        }
    else
        {
        TBuf8<1> buf;
        IMUM_OUT();
        return CMsvCompletedOperation::NewL(
            Session(),
            Type(),
            buf,
            KMsvLocalServiceIndexEntryId,
            aStatus,
            KErrCancel);
        }
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::ForwardL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::ForwardL(
    TMsvId aDestination,
    TMsvPartList aPartList,
    TRequestStatus& aCompletionStatus )
    {
    IMUM_CONTEXT( CImap4MtmUi::ForwardL, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();

    return ForwardOrReplyL(aPartList, aDestination, aCompletionStatus, ETrue);
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::ReplyL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::ReplyL(
    TMsvId aDestination,
    TMsvPartList aPartList,
    TRequestStatus& aCompletionStatus )
    {
    IMUM_CONTEXT( CImap4MtmUi::ReplyL, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();

    return ForwardOrReplyL(aPartList, aDestination, aCompletionStatus, EFalse);
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::ForwardOrReplyL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::ForwardOrReplyL(
    TMsvPartList aPartlist,
    TMsvId aDestination,
    TRequestStatus& aCompletionStatus,
    TBool aForward)
    {
    IMUM_CONTEXT( CImap4MtmUi::ForwardOrReplyL, 0, KImumMtmLog );
    IMUM_IN();

    //  We need to determine whether we're working with a ghost entry or not
    TMsvId relatedId=BaseMtm().Entry().Entry().iRelatedId;

    if(relatedId!=KMsvNullIndexEntryId)
        //  We're dealing with a ghost entry - trace back to originator
        BaseMtm().SwitchCurrentEntryL(relatedId);

    TEditorParameters editorParams;

    editorParams.iFlags &= ~(EMsgLaunchEditorEmbedded | EMsgLaunchEditorThenWait);
    const TUint preferences = Preferences();
    if(preferences & EMtmUiFlagEditorPreferEmbedded)
        {
        editorParams.iFlags |= EMsgLaunchEditorEmbedded;
        }
    if(!(preferences & EMtmUiFlagEditorNoWaitForExit))
        {
        editorParams.iFlags |= EMsgLaunchEditorThenWait;
        }

    if(aForward)
        editorParams.iFlags|=EMsgForwardMessage;
    else
        editorParams.iFlags|=(aPartlist&KMsvMessagePartRecipient)?EMsgReplyToMessageAll:EMsgReplyToMessageSender;

    editorParams.iId=BaseMtm().Entry().EntryId();
    editorParams.iPartList = aPartlist;
    editorParams.iDestinationFolderId = aDestination;

    CMsvOperation* op=NULL;

    if ( BaseMtm().Entry().Entry().Unread() )
        {
        // mark read before reply...

        CMsvEntry* msgEntry =
            Session().GetEntryL( BaseMtm().Entry().EntryId() );
        CleanupStack::PushL( msgEntry );
        TMsvEntry email = msgEntry->Entry();

        // Set the unread flag
        email.SetUnread( EFalse );

        CMuiuOperationWait* wait =
            CMuiuOperationWait::NewLC( EActivePriorityDefault );
        CMsvOperation* op=msgEntry->ChangeL( email, wait->iStatus );
        wait->Start();
        CleanupStack::PopAndDestroy( wait );
        wait = NULL;

        delete op;
        op = NULL;

        CleanupStack::PopAndDestroy( msgEntry );
        msgEntry = NULL;
        }

    if(BaseMtm().Entry().Entry().Complete())
        op=LaunchEditorApplicationL(aCompletionStatus, editorParams);
    else if(relatedId!=KMsvNullIndexEntryId)
        //  We were originally dealing with ghost entry
        op=OfferSynchroniseL(BaseMtm().Entry().OwningService(), aCompletionStatus);
    else
        //  We are dealing with an incomplete, non-ghost entry
        op=OpenMessageL(aCompletionStatus, editorParams);
    IMUM_OUT();

    return op;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::FetchMessagesL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::FetchMessagesL(
    TRequestStatus& aStatus,
    const CMsvEntrySelection& aSelection,
    TInt aFunctionId,
    TImImap4GetMailInfo& aGetMailInfo )
    {
    IMUM_CONTEXT( CImap4MtmUi::FetchMessagesL, 0, KImumMtmLog );
    IMUM_IN();

    // Fetch messages, no size checking.
    // Used for 'Fetch All', 'Fetch New', and 'Get Mail'.
    CMsvProgressReporterOperation* reporter =
        CMsvProgressReporterOperation::NewL( ETrue, ETrue, Session(), aStatus,
        EMbmAvkonQgn_note_progress );
    CleanupStack::PushL( reporter );

        HBufC* title = StringLoader::LoadLC(
            R_IMAP4_PROGRESS_FETCHING, iCoeEnv );
    reporter->SetTitleL( *title );
    CleanupStack::PopAndDestroy( title );
    title=NULL;


    const TMsvId service = BaseMtm().Entry().OwningService();
    CMsvOperation* fetch = CImap4FetchOp::NewL( *iMailboxApi,
        reporter->RequestStatus(), *reporter, service, aFunctionId,
        aGetMailInfo, aSelection );
    reporter->SetOperationL( fetch ); // Takes immediate ownership
    CleanupStack::Pop(); // reporter
    IMUM_OUT();
    return reporter;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::InitParametersForFetchCmdL()
// ----------------------------------------------------------------------------
//
void CImap4MtmUi::InitParametersForFetchCmdL(
    CMsvEntrySelection* aSelection,
    TImImap4GetMailInfo& aGetMailInfo,
    TBool aInsertParentFolder /* = ETrue */) const
    {
    IMUM_CONTEXT( CImap4MtmUi::InitParametersForFetchCmdL, 0, KImumMtmLog );
    IMUM_IN();

    // Initialise the parameters for Fetch New, Fetch All or Fetch selected.
    // Client MTM context is folder.
    // For populate new or populate all, we must add the parent folder to the selection.
    if ( aInsertParentFolder && aSelection)
        {
        aSelection->InsertL(0, BaseMtm().Entry().EntryId());
        }
    aGetMailInfo.iMaxEmailSize = KMaxTInt32;
    const TMsvId currentEntryId = BaseMtm().Entry().EntryId();
    BaseMtm().SwitchCurrentEntryL(BaseMtm().Entry().OwningService());
    Imap4BaseMtm()->RestoreSettingsL();
    BaseMtm().SwitchCurrentEntryL( currentEntryId );

    if ( iFeatureFlags->GF( EMailFeatureMeetingRequestSupport ) )
        {
        aGetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyAlternativeText;
        }
    else
        {
        aGetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyText;
        }

    // No destination for populate.
    aGetMailInfo.iDestinationFolder = KMsvNullIndexEntryId;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::InitParametersForGetMailCmdL()
// ----------------------------------------------------------------------------
//
void CImap4MtmUi::InitParametersForGetMailCmdL(
    CMsvEntrySelection& aSelection,
    TImImap4GetMailInfo& aGetMailInfo)
    {
    IMUM_CONTEXT( CImap4MtmUi::InitParametersForGetMailCmdL, 0, KImumMtmLog );
    IMUM_IN();

    // Initialise the parameters for Get Mail.
    // Client MTM context is service.
    // We must add the inbox id to the selection.
    CMsvEntrySelection* serviceChildren = BaseMtm().Entry().ChildrenL();
    CleanupStack::PushL(serviceChildren);
    const TMsvId inboxId = SelectionContainsInboxL(BaseMtm().Entry(),*serviceChildren);
    CleanupStack::PopAndDestroy(); // serviceChildren
    aSelection.InsertL(0, inboxId);
    Imap4BaseMtm()->RestoreSettingsL();
    const CImImap4Settings& settings = Imap4BaseMtm()->Imap4Settings();
    aGetMailInfo.iMaxEmailSize = (TInt32)(settings.MaxEmailSize());
    aGetMailInfo.iGetMailBodyParts = settings.GetMailOptions();
    aGetMailInfo.iDestinationFolder = KMsvNullIndexEntryId;     // No destination for populate.
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::OpenMessageL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::OpenMessageL(
    TRequestStatus& aCompletionStatus,
    const TEditorParameters& aEditorParams )
    {
    IMUM_CONTEXT( CImap4MtmUi::OpenMessageL, 0, KImumMtmLog );
    IMUM_IN();

    CMsvOperation* op = DoPopulateIncompleteMessageL(aCompletionStatus, *SingleEntrySelectionLC(aEditorParams.iId), aEditorParams);
    CleanupStack::PopAndDestroy(); // selection

    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::PopulateMessagesL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::PopulateMessagesL(
    TRequestStatus& aCompletionStatus,
    const CMsvEntrySelection& aSel )
    {
    IMUM_CONTEXT( CImap4MtmUi::PopulateMessagesL, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();
    return DoPopulateIncompleteMessageL(
        aCompletionStatus, aSel, TEditorParameters() );
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::DoPopulateIncompleteMessageL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::DoPopulateIncompleteMessageL(
    TRequestStatus& aStatus,
    const CMsvEntrySelection& aSel,
    const TEditorParameters& aEditorParams )
    {
    IMUM_CONTEXT( CImap4MtmUi::DoPopulateIncompleteMessageL, 0, KImumMtmLog );
    IMUM_IN();

    TBuf8<1> dummyParams;
    CMsvSession& session = Session();

    // this is just to make sure fetching is not even started if no disk space left.
    if ( !MsvEmailMtmUiUtils::CheckAvailableDiskSpaceForDownloadL(
         KImumApproxMailFetchSize, *iEikonEnv, session ) )
        {
        return CMsvCompletedOperation::NewL(Session(), Type(), dummyParams, KMsvLocalServiceIndexEntryId, aStatus, KErrNone);
        }
    // Determine how much of the message to fetch, and prompt if neccessary.
    // Return the operation to be returned to the application.
    // Copes with simply populating, and launching an editor after populating. If launching an editor,
    // 'aSel' should contain only one message id.


    // Check the whole selection for ghost entries. If such an entry exists,
    // and we're offline, we need to synchronise.
    CMsvEntry* msgEntry = Session().GetEntryL(aSel[0]);
    CleanupStack::PushL(msgEntry);
    TMsvId owningService=msgEntry->OwningService();
    TMsvEmailEntry emailEntry(msgEntry->Entry());

    TBool currentBodyComplete = emailEntry.BodyTextComplete();

    const TBool offline = !IsConnected(owningService);
    const TMsvId parentId = msgEntry->Entry().Parent();
    msgEntry->SetEntryL(parentId);
    if(SelectionContainsGhostEntryL(*msgEntry,aSel) && offline)
        {
        CleanupStack::PopAndDestroy();  //  msgEntry;
        return OfferSynchroniseL(owningService, aStatus);
        }

    CMsvEntrySelection* entries = StripCompleteEntriesLC(aSel);
    TInt numMsgs = entries->Count();
    CleanupStack::PopAndDestroy();  //  entries
    if(!numMsgs)
        {
        // no entries to fetch
        CleanupStack::PopAndDestroy();  //  msgEntry;
        return CMsvCompletedOperation::NewL(Session(), Type(), dummyParams, KMsvLocalServiceIndexEntryId, aStatus, KErrNone);
        }
    ////
    //// --- Get info ---
    ////
    const TBool forwarding = (aEditorParams.iFlags & EMsgForwardMessage);
    TImImap4GetMailInfo getMailInfo;
    InitParametersForFetchCmdL(NULL, getMailInfo, EFalse);
    enum TFetchType { EFetchCompleted, EFetchBodyText, EFetchAttachments };
    TFetchType fetchType = EFetchBodyText;

    numMsgs = aSel.Count();
    TBool hasAttachments = EFalse;      // Do *any* of the messages have attachments?
    TInt msgCount = 0;
    for(; msgCount < numMsgs; msgCount++)
        {
        if(msgEntry->ChildDataL(aSel[msgCount]).Attachment())
            {
            hasAttachments = ETrue;     // Can we tell if we already have all attachments?
            break;
            }
        }

    if ( hasAttachments &&
         getMailInfo.iGetMailBodyParts == EGetImap4EmailBodyTextAndAttachments )
        {
        fetchType = EFetchAttachments;
        }

    for(msgCount = 0; msgCount < numMsgs; msgCount++)
        {
        emailEntry = msgEntry->ChildDataL(aSel[msgCount]);
        if(!emailEntry.BodyTextComplete())
            {
            break;
            }
        }
    CleanupStack::PopAndDestroy(); //msgEntry

    if(hasAttachments && forwarding)
        {
        // Always get attachments if forwarding.
        fetchType = EFetchAttachments;
        }

    IMUM0(0, "Prompt for the fetching");

    //Do the prompting only if opening unfetched message and offline and message body is incomplete
    if( !currentBodyComplete && offline && aEditorParams.iFlags)
        {
        if ( CIMSSettingsNoteUi::ShowQueryL(
            R_EMAIL_RETRIEVE_1_TEXT, R_EMAIL_CONFIRMATION_QUERY ) )
            {
            //VARIATION START
            if ( iFeatureFlags->GF( EMailFeatureOfflineMode ) )
                {
                TMsvId mailboxId = BaseMtm().Entry().EntryId();
                //No need to do this if we're online...
                TBool offline = EFalse;
                if( !IsConnected( mailboxId ) &&
                    !iMailboxApi->MailboxUtilitiesL().HasWlanConnectionL( mailboxId ) )
                    {
                    offline = MsvEmailMtmUiUtils::DoOfflineChecksL( mailboxId );
                    }
                if( offline )
                    {
                    return CMsvCompletedOperation::NewL(Session(), Type(), KNullDesC8, KMsvLocalServiceIndexEntryId, aStatus, KErrCancel);
                    }
                }//if
            //VARIATION END
            if( hasAttachments )
                {
                fetchType = EFetchAttachments;
                }
            }
        else
            {
            return CMsvCompletedOperation::NewL(Session(), Type(), dummyParams, KMsvLocalServiceIndexEntryId, aStatus, KErrCancel);
            }
        }

    ////
    //// --- Check there is work to do ---
    ////
    // We know none of the entries are complete, (since we check at the top of the fn).
    // If just fetching body text, check we don't have the body text for all the messages.
    if(fetchType == EFetchBodyText)
        {
        entries = StripBodyTextCompleteEntriesLC(aSel);
        if(entries->Count() == 0)
            {
            // No messages to fetch.
            fetchType = EFetchCompleted;
            }
        CleanupStack::PopAndDestroy(); // entries
        }

    if(fetchType == EFetchCompleted)
        {
        if(aEditorParams.iFlags)
            {
            IMUM_OUT();
            // Editing, but no fetch to do.
            return LaunchEditorApplicationL(aStatus, aEditorParams);
            }
        }

    ////
    //// --- Create the operation(s) ---
    ////

    // Create a progress reporting dialog.
    CMsvProgressReporterOperation* reporter =
        CMsvProgressReporterOperation::NewL(
            ETrue,
            ETrue,
            Session(),
            aStatus,
            EMbmAvkonQgn_note_progress );
    CleanupStack::PushL(reporter);

    // If already connected, display the "Retrieving..." note right away.
    if ( IsConnected( owningService ))
    	reporter->SetTitleL( R_IMAP4_PROGRESS_FETCHING );
    	
    else
    	{
    	TBuf<EProgressStringMaxLen> title;
    	MsvEmailMtmUiUtils::CreateConnectingToText( title,
            *iMsvSession, owningService );
    	reporter->SetTitleL( title );
    	}


    CMsvOperation* fetch = NULL;
    getMailInfo.iDestinationFolder = parentId;
    if(aEditorParams.iFlags)
        {
        // Create a fetch and edit operation.
        HBufC* appName = (aEditorParams.iFlags & EMsgReadOnly) ? (GetViewerFileNameL()) : (GetEditorFileNameL());
        TEmailEditParams fetchAndEditParams;
        fetchAndEditParams.iPreferences = Preferences();
        fetchAndEditParams.iEditorFileName = *appName;
        fetchAndEditParams.iEditorParams = aEditorParams;
        const TBool ackReceipts = AcknowledgeReceiptsL(aEditorParams.iId);
        CImumFetchAndEditOperation* fetchAndEdit = CImumFetchAndEditOperation::NewL(
            *iMailboxApi, reporter->RequestStatus(), *reporter, fetchAndEditParams, ackReceipts);
        CleanupStack::PushL(fetchAndEdit);
        // Create a fetch operation.
        fetch = InvokeRemoteFetchOpL(*reporter, fetchAndEdit->RequestStatus(), aSel,
            KIMAP4MTMPopulateMailSelectionWhenAlreadyConnected, getMailInfo);
        // Set up the wrapping.
        fetchAndEdit->SetFetchOpL(fetch);
        CleanupStack::Pop(); // fetchAndEdit
        reporter->SetOperationL(fetchAndEdit); // Takes immediate ownership
        }
    else
        {
        // Create a fetch operation.
        fetch = InvokeRemoteFetchOpL(*reporter, reporter->RequestStatus(), aSel,
            KIMAP4MTMPopulateMailSelectionWhenAlreadyConnected, getMailInfo);
        reporter->SetOperationL(fetch); // Takes immediate ownership
        }
    CleanupStack::Pop(); // reporter

    IMUM_OUT();
    return reporter;
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::InvokeRemoteFetchOpL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::InvokeRemoteFetchOpL(
    CMsvProgressReporterOperation& aReporter,
    TRequestStatus& aCompletionStatus,
    const CMsvEntrySelection& aSel,
    TInt aFunctionId,
    const TImImap4GetMailInfo& aGetMailInfo)
    {
    IMUM_CONTEXT( CImap4MtmUi::InvokeRemoteFetchOpL, 0, KImumMtmLog );
    IMUM_IN();

    CMsvEntry* centry = Session().GetEntryL(aSel[0]);
    TMsvId service = centry->OwningService();
    delete centry;
    IMUM_OUT();
    return CImap4SizeAndFetchOp::NewL(*iMailboxApi,aCompletionStatus, aReporter, service, aFunctionId, aGetMailInfo, aSel);
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::InvokeCopyMoveOpL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::InvokeCopyMoveOpL(
    CMsvProgressReporterOperation& aReporter,
    TRequestStatus& aCompletionStatus,
    const CMsvEntrySelection& aSel,
    TMsvId aTargetId,
    TBool aCopy )
    {
    IMUM_CONTEXT( CImap4MtmUi::InvokeCopyMoveOpL, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();

    return CImap4CopyMoveLocalOp::NewL(*iMailboxApi, aCompletionStatus, aReporter, aSel, aTargetId, aCopy);
    }

// ----------------------------------------------------------------------------
// CImap4MtmUi::OfferSynchroniseL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::OfferSynchroniseL(
    TMsvId aService,
    TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CImap4MtmUi::OfferSynchroniseL, 0, KImumMtmLog );
    IMUM_IN();

    CMsvOperation* op=NULL;
    TInt state=ServiceState(aService);
    if(ServiceIsDisconnected(state) && !ServiceIsConnecting(state))
    // no query to user, just synchronise...
        {
        //  User wishes to connect and sync...
        CMsvEntrySelection* mySelection=SingleEntrySelectionLC(aService);
        TBuf8<1> dummyParam;
        op=InvokeAsyncFunctionL(KImpiMTMConnectAndSyncCompleteAfterDisconnection, *mySelection, aStatus, dummyParam);
        CleanupStack::PopAndDestroy();  //  mySelection
        }
    else
        op=CompletedImap4OperationL(aService, aStatus);
    IMUM_OUT();
    return op;
    }

// ---------------------------------------------------------------------------
// CImap4MtmUi::ShowMessageInfoL()
// ---------------------------------------------------------------------------
//
CMsvOperation* CImap4MtmUi::ShowMessageInfoL(
    TRequestStatus& aCompletionStatus,
    TDes8& /*aParameter*/ )
    {
    IMUM_CONTEXT( CImap4MtmUi::ShowMessageInfoL, 0, KImumMtmLog );
    IMUM_IN();

    TMsgInfoMessageInfoData infoData;
    TBuf<KImumMessageInfoBuffer> dateBuf;
    TBuf<KImumMessageInfoBuffer> timeBuf;
    TBuf<KImumMessageInfoBuffer> sizeBuf;
    TBuf<KImumMessagePriorityBufferLength> priorityBuf;
    TBool readStoreFound = MsvEmailMtmUiUtils::SetMessageInfoDataLCC(
        infoData,*Imap4BaseMtm(),*iEikonEnv,dateBuf,timeBuf,sizeBuf,priorityBuf );

    TBuf<KImumMessageInfoBuffer> type;
    iEikonEnv->ReadResourceL( type,R_IMAP_MESSAGE_INFO_MAIL_TYPE );
    infoData.iType.Set(type);

    CMsgInfoMessageInfoDialog* infoDialog = CMsgInfoMessageInfoDialog::NewL();
    infoDialog->ExecuteLD( infoData, CMsgInfoMessageInfoDialog::EEmailViewer );
    CleanupStack::PopAndDestroy( 2 ); // CSI: 47 # 2 because SetMessageInfoDataLCC
    if ( readStoreFound )
        {
        CleanupStack::PopAndDestroy( 2 ); // CSI: 47 # 2 because store found
        }
    IMUM_OUT();

    return CMsvCompletedOperation::NewL(
        Session(), KUidMsvLocalServiceMtm, KNullDesC8,
        KMsvLocalServiceIndexEntryId, aCompletionStatus, KErrNone );
    }

// End of file

