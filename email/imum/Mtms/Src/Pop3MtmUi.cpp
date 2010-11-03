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
*       POP3 MTM UI
*
*/


// Standard includes
#include <txtrich.h>                        // CRichText
#include <apacmdln.h>                       // CApaCommandLine
#include <apgcli.h>
#include <apaid.h>
#include <eikdialg.h>                       // CEikDialog
#include <eikon.hrh>
#include <eikenv.h>                         // CEikonEnv
#include <eikrutil.h>                       // EikResourceUtils
#include <aknlistquerydialog.h>
#include <aknPopupHeadingPane.h>            // CAknPopupHeadingPane
#include <ErrorUI.h>                        // CErrorUI
#include <StringLoader.h>
#include <etel.h>                           // KErrEtelBusyDetected
#include <bldvariant.hrh>
#include <etelpckt.h>

// Messaging includes
#include <mtclreg.h>
#include <mtuireg.h>
#include <mtmuidef.hrh>
#include <pops.h>
#include <pop3set.h>
#include <smtpset.h>
#include <miutset.h>
#include <MuiuMsvProgressReporterOperation.h>
#include <avkon.mbg>                        // bitmaps for progress
#include <MuiuOperationWait.h>

// Specific includes
#include "ImumPanic.h"
#include <imum.rsg>
#include "Pop3MtmUi.h"
#include <mtmdef.h>
#include <msvids.h>
#include "PROGTYPE.H"
#include "MsvEmailConnectionProgressProvider.h"
#include "EmailUtils.H"
#include "EmailFeatureUtils.h"
#include <muiuflagger.h>                    // CMuiuFlags
#include "ImumDisconnectOperation.h"
#include "ImumEditOperation.h"
#include "ImumFetchAndEditOperation.h"
#include <MTMStore.h>
#include "ImumOnlineOperation.h"
#include "ImumDeleteMessagesLocally.h"
#include "ImumDeleteMessagesFromServer.h"
#include "MsvPop3ConnectOp.h"
#include "MsvPop3FetchOperation.h"
#include "MsvPop3CopyMoveOperation.h"
#include <MtmExtendedCapabilities.hrh>
#include <Muiumsginfo.h>
#include <Muiumsginfo.hrh>
#include "EmailEditorViewerUids.h"
#include <aknnotewrappers.h>                // CAknNoteDialog
#include "IMSSettingsNoteUi.h"
#include <ImumInternalApi.h>        // CImumInternalApi
#include <ImumInHealthServices.h>
#include "ImumMtmLogging.h"
#include "ImumListQueryDialog.h"

// prevents warning with TRefByValue in WINS REL
#pragma warning (disable : 4710)	// CSI: 47 # Needed for disable warning.

// Constants
const TInt KPop3MtmUiDeleteMessagesLocally = 0;
const TInt KImumEntriesDoneReplaceIndex = 0;
const TInt KImumTotalEntriesReplaceIndex = 1;
// Convenience function IDs for FetchL().
const TInt KFunctionIdFetchSelected = KPOP3MTMCopyMailSelectionWhenAlreadyConnected;
const TInt KFunctionIdFetchAll = KPOP3MTMCopyAllMailWhenAlreadyConnected;
const TInt KFunctionIdFetchNew = KPOP3MTMCopyNewMailWhenAlreadyConnected;
const TInt KImumMessageInfoDateBufferLength = 30;
const TInt KImumMessageInfoSizeBufferLength = 20;
const TInt KImumMessagePriorityBufferLength = 32;
const TInt KImumMaxCharsInLine = 75;
const TInt KImumKB = 1024;
const TInt KErrDndNameNotFound = -5120;

// Defines
_LIT( KPop3uMtmUiResourceFile, "imum.rsc" );

//////////////////////
// Factory function //
//////////////////////

// ----------------------------------------------------------------------------
// NewPP3UMtmUiL()
// ----------------------------------------------------------------------------
//
EXPORT_C CBaseMtmUi* NewPP3UMtmUiL(CBaseMtm& aMtm, CRegisteredMtmDll& aRegisteredDll)
    {
    return CPop3MtmUi::NewL(aMtm, aRegisteredDll);
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::NewL()
// ----------------------------------------------------------------------------
//
CPop3MtmUi* CPop3MtmUi::NewL(CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll)
    {
    IMUM_STATIC_CONTEXT( CPop3MtmUi::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();
    
    CPop3MtmUi* self=new(ELeave) CPop3MtmUi(aBaseMtm, aRegisteredMtmDll);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    
    IMUM_OUT();
    
    return self;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::CPop3MtmUi(()
// ----------------------------------------------------------------------------
//
CPop3MtmUi::CPop3MtmUi(CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll)
    :
    CImumMtmBaseMtmUi(aBaseMtm, aRegisteredMtmDll)
    {
    IMUM_CONTEXT( CPop3MtmUi::CPop3MtmUi, 0, KImumMtmLog );
    IMUM_IN();
    
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::ConstructL()
// ----------------------------------------------------------------------------
//
void CPop3MtmUi::ConstructL()
    {
    IMUM_CONTEXT( CPop3MtmUi::ConstructL, 0, KImumMtmLog );
    IMUM_IN();
    
    // --- Make sure base class correctly constructed ---
    CImumMtmBaseMtmUi::ConstructL();
    iServerNameCache.iService = KMsvNullIndexEntryId;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::GetEditorFileNameL()
// ----------------------------------------------------------------------------
//
HBufC* CPop3MtmUi::GetEditorFileNameL()
    {
    IMUM_CONTEXT( CPop3MtmUi::GetEditorFileNameL, 0, KImumMtmLog );
    IMUM_IN();
    
    if(!iEditorFilename)
        {
        iEditorFilename = MsvUiEditorUtilities::ResolveAppFileNameL(
            KUidMsgInternetMailEditor);
        }
    IMUM_OUT();
    return iEditorFilename;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::GetViewerFileNameL()
// ----------------------------------------------------------------------------
//
HBufC* CPop3MtmUi::GetViewerFileNameL()
    {
    IMUM_CONTEXT( CPop3MtmUi::GetViewerFileNameL, 0, KImumMtmLog );
    IMUM_IN();
    
    if(!iViewerFilename)
        {
        iViewerFilename = MsvUiEditorUtilities::ResolveAppFileNameL(
            KUidMsgInternetMailViewer);
        }
    IMUM_OUT();
    return iViewerFilename;
    }

// ----------------------------------------------------------------------------
// ::~CPop3MtmUi()()
// ----------------------------------------------------------------------------
//
CPop3MtmUi::~CPop3MtmUi()
    {
    IMUM_CONTEXT( CPop3MtmUi::~CPop3MtmUi, 0, KImumMtmLog );
    IMUM_IN();
    
    delete iEditorFilename;
    delete iViewerFilename;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::CreateL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::CreateL(
    const TMsvEntry& aEntry,
    CMsvEntry& aParent,
    TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CPop3MtmUi::CreateL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Context is irrelevant.
    aStatus=KRequestPending;
    // Make sure the entry is a service, since POP3 doesn't support folders
    // and POP3 messages / attachments are only created by the engine components
    if (aEntry.iType.iUid!=KUidMsvServiceEntryValue)
        User::Leave(KErrNotSupported);
    //
    // Since services can only be created from the root, make sure the requested parent is root
    __ASSERT_ALWAYS(aParent.Entry().Id()==KMsvRootIndexEntryId, User::Panic(KImumMtmUiPanic, EPop3MtmUiParentNotRoot));
    //
    // Create an SMTP service entry
    TMsvEntry smtpService(aEntry);
    smtpService.iMtm=KUidMsgTypeSMTP;
    smtpService.SetVisible(EFalse);
    aParent.CreateL(smtpService);
    IMUM_OUT();
    //
    // POP3 MTMUI defers set-up to the SMTP MTMUI, so create an SMTP mtmui and edit a service with it
    return EditSmtpServiceL(aStatus, smtpService.Id());
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::OpenL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::OpenL(TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CPop3MtmUi::OpenL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Context is the entry to open.
    const TMsvEntry& contextEntry=BaseMtm().Entry().Entry();
    CheckEntryL(contextEntry);

    if(contextEntry.iType.iUid == KUidMsvMessageEntryValue)
        {
        IMUM_OUT();
        return EditL(aStatus);
        }

    // Entry is a service.
    /*Check if partial fetch should be used*/
    /*
    const TMsvId serviceId = contextEntry.Id();
    const CImPop3Settings& settings = GetAccountSettingsL(serviceId);
    TInt limit = settings.PopulationLimit();

    TInt functionID = KPOP3MTMConnect;
    // n lines of message to be fetched. or full message.
    if( limit == EFullBody || ( limit >= EUserDefinedKB && limit < KMaxTInt32 ) )
        {
        functionID = KPOP3MTMPopulateAll;
        }
        */

    TBuf8<1> dummyParams;
    dummyParams.Zero();
    CMsvEntrySelection* sel=new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL(sel);
    sel->AppendL(contextEntry.Id());

    CMsvOperation* op = InvokeAsyncFunctionL(
        KPOP3MTMConnect, *sel, aStatus, dummyParams);
    CleanupStack::PopAndDestroy();//sel
    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::OpenL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::OpenL(
    TRequestStatus& aStatus,
    const CMsvEntrySelection& aSelection)
    {
    IMUM_CONTEXT( CPop3MtmUi::OpenL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Context is irrelevant.
    TMsvId selectionParent=KMsvNullIndexEntryId;
    CheckSelectionL(aSelection, selectionParent);
    // Edit the message selection
    IMUM_OUT();
    return EditL(aStatus, aSelection);
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::CloseL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::CloseL(TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CPop3MtmUi::CloseL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Context is the entry to close.
    const TMsvEntry& contextEntry=BaseMtm().Entry().Entry();
    CheckEntryL(contextEntry);
    // Check that current context is a service, all other entry types
    // cannot be closed
    if(contextEntry.iType != KUidMsvServiceEntry)
        {
        User::Leave(KErrNotSupported);
        }
    // Disconnect the service
    aStatus=KRequestPending;
    if(Session().ServiceActive(contextEntry.Id()))
        {
        IMUM_OUT();
        // Service is busy
        return CMsvCompletedOperation::NewL(
            Session(),
            Type(),
            KNullDesC8,
            KMsvLocalServiceIndexEntryId,
            aStatus,
            KErrNone);// KErrNone because we've reported the fact to the user
        }
    CMsvEntrySelection* sel=SingleEntrySelectionLC(contextEntry.Id());
    TBuf8<1> buf;
    buf.Zero();
    CMsvOperation* op=InvokeAsyncFunctionL(KPOP3MTMDisconnect, *sel, aStatus, buf);
    CleanupStack::PopAndDestroy();//selection
    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::CloseL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::CloseL(
    TRequestStatus& /*aStatus*/,
    const CMsvEntrySelection& /*aSelection*/)
    {
    IMUM_CONTEXT( CPop3MtmUi::CloseL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Multiple close is not supported
    User::Leave(KErrNotSupported);
    IMUM_OUT();
    return NULL;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::EditL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::EditL(TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CPop3MtmUi::EditL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Context is the entry to edit.
    const TMsvEntry& contextEntry=BaseMtm().Entry().Entry();
    CheckEntryL(contextEntry);

    // Edit service (mailbox)
    if(contextEntry.iType.iUid==KUidMsvServiceEntryValue)
        {
        TMsvEntry mailbox = BaseMtm().Entry().Entry();
        if ( !iMailboxApi->HealthServicesL().IsMailboxHealthy( mailbox.Id() ) )
            {
            User::Leave( KErrNotFound );
            }    
        
        return EditSmtpServiceL(aStatus, mailbox.iRelatedId);
        }
    IMUM_OUT();
    // Edit message
    return ViewMessageL(aStatus);
    // POP3 messages are always read only, so actually we are viewing the message.
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::EditL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::EditL(
    TRequestStatus& aStatus,
    const CMsvEntrySelection& aSelection)
    {
    IMUM_CONTEXT( CPop3MtmUi::EditL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Context is irrelevant.
    // Only the first message in the selection is editted.
    __ASSERT_ALWAYS(aSelection.Count(), User::Panic(KImumMtmUiPanic, EPop3MtmUiSelectionIsEmpty));
    BaseMtm().Entry().SetEntryL(aSelection[0]);
    IMUM_OUT();
    return EditL(aStatus);
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::ViewL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::ViewL(TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CPop3MtmUi::ViewL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Context is the message to view.
    const TMsvEntry& context=BaseMtm().Entry().Entry();
    CheckEntryL(context);

    // Can only view message entries
    if(context.iType.iUid != KUidMsvMessageEntryValue)
        {
        User::Leave(KErrNotSupported);
        }
    IMUM_OUT();

    return ViewMessageL(aStatus);
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::ViewL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::ViewL(
    TRequestStatus& aStatus,
    const CMsvEntrySelection& aSelection)
    {
    IMUM_CONTEXT( CPop3MtmUi::ViewL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Context is irrelevant.
    // Only the first message in the selection is viewed.
    __ASSERT_ALWAYS(aSelection.Count(), User::Panic(KImumMtmUiPanic, EPop3MtmUiSelectionIsEmpty));
    BaseMtm().Entry().SetEntryL(aSelection[0]);
    IMUM_OUT();
    return ViewL(aStatus);
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::CancelL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::CancelL(
    TRequestStatus& /*aStatus*/,
    const CMsvEntrySelection& /*aSelection*/)
    {
    IMUM_CONTEXT( CPop3MtmUi::CancelL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Currently this function only used to suspend messages queued for sending.
    // Can't send POP3 messages, so can't cancel them either.
    User::Leave(KErrNotSupported);
    IMUM_OUT();
    return NULL;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::CopyToL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::CopyToL(
    const CMsvEntrySelection& /*aSelection*/,
    TRequestStatus& /*aStatus*/)
    {
    IMUM_CONTEXT( CPop3MtmUi::CopyToL, 0, KImumMtmLog );
    IMUM_IN();
    
    __ASSERT_DEBUG( EFalse, User::Panic(KImumMtmUiPanic, EPop3MtmUiCannotCopyOrMoveToRemote));
    User::Leave(KErrNotSupported);
    IMUM_OUT();
    return NULL;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::MoveToL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::MoveToL(
    const CMsvEntrySelection& /*aSelection*/,
    TRequestStatus& /*aStatus*/)
    {
    IMUM_CONTEXT( CPop3MtmUi::MoveToL, 0, KImumMtmLog );
    IMUM_IN();
    
    __ASSERT_DEBUG( EFalse, User::Panic(KImumMtmUiPanic, EPop3MtmUiCannotCopyOrMoveToRemote));
    User::Leave(KErrNotSupported);
    IMUM_OUT();
    return NULL;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::CopyFromL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::CopyFromL(
    const CMsvEntrySelection& /*aSelection*/,
    TMsvId /*aTargetId*/,
    TRequestStatus& /*aStatus*/ )
    {
    IMUM_CONTEXT( CPop3MtmUi::CopyFromL, 0, KImumMtmLog );
    IMUM_IN();
    
    User::Leave( KErrNotSupported );
    IMUM_OUT();
    return NULL;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::MoveFromL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::MoveFromL(
    const CMsvEntrySelection& /*aSelection*/,
    TMsvId /*aTargetId*/,
    TRequestStatus& /*aStatus*/)
    {
    IMUM_CONTEXT( CPop3MtmUi::MoveFromL, 0, KImumMtmLog );
    IMUM_IN();
    
    User::Leave(KErrNotSupported);
    IMUM_OUT();
    return NULL;
    }

// ----------------------------------------------------------------------------
// ::QueryCapability(()
// ----------------------------------------------------------------------------
//
TInt CPop3MtmUi::QueryCapability(TUid aCapability, TInt& aResponse)
    {
    IMUM_CONTEXT( CPop3MtmUi::QueryCapability, 0, KImumMtmLog );
    IMUM_IN();
    
    switch (aCapability.iUid)
        {
    case KUidMsvMtmUiQueryConnectionOrientedServices:
        IMUM_OUT();
        return KErrNone;

    default:
        break;
        };

    IMUM_OUT();
    return CBaseMtmUi::QueryCapability(aCapability, aResponse);
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::InvokeAsyncFunctionL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::InvokeAsyncFunctionL(
    TInt aFunctionId,
    const CMsvEntrySelection& aSelection,
    TRequestStatus& aStatus,
    TDes8& aParameter )
    {
    IMUM_CONTEXT( CPop3MtmUi::InvokeAsyncFunctionL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Context is dependant upon function ID.
    CMsvOperation* op = NULL;

    TMsvEntry tEntry;
    TMsvId service;
    Session().GetEntry( BaseMtm().Entry().EntryId(), service, tEntry );
    //Offline checks VARIATION START
    if( iFeatureFlags->GF( EMailFeatureOfflineMode ) &&
         aFunctionId != KMtmUiFunctionMessageInfo)
        {
        TBool offline = EFalse;

        TMsvEntry tEntry;
        TMsvId service;
        Session().GetEntry( BaseMtm().Entry().EntryId(), service, tEntry );
        if( !tEntry.Connected() &&
            !iMailboxApi->MailboxUtilitiesL().HasWlanConnectionL( service ) )            
            {
            offline = MsvEmailMtmUiUtils::DoOfflineChecksL( service );
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
        case KMtmUiFunctionFetchAll:
            IMUM_OUT();
            // Client MTM context is service.
            return FetchL( aStatus, aSelection, KFunctionIdFetchAll );

        case KMtmUiFunctionFetchNew:
            IMUM_OUT();
            // Client MTM context is service.
            return FetchL( aStatus, aSelection, KFunctionIdFetchNew );

        case KMtmUiFunctionFetchSelected:
            {
            // Client MTM context is service
            // Check there is something to fetch.
            CMsvEntrySelection* entries =
                MsvEmailMtmUiUtils::StripCompleteEntriesLC(
                BaseMtm().Entry(), aSelection );
            const TInt count = entries->Count();
            CleanupStack::PopAndDestroy();  // entries
            if( !count )
                {
                IMUM_OUT();
                // All messages are complete so nothing to do.
                return CMsvCompletedOperation::NewL(
                    Session(),
                    Type(),
                    KNullDesC8,
                    KMsvLocalServiceIndexEntryId,
                    aStatus,
                    KErrNone);
                }
            }
            return FetchL( aStatus, aSelection, KFunctionIdFetchSelected );

        case KMtmUiFunctionGetNew:
            IMUM_OUT();
            // Client MTM context is service.
            return GetMailL( aStatus );

        case KPOP3MTMConnect:
        case KPOP3MTMDisconnect:
            {

            const CImPop3Settings& settings = GetAccountSettingsL( service );

            //get population limit for connect operations
            TInt limit = settings.PopulationLimit();
            // -1 means body and attachment, pass it to Symbian mail engine
            if ( limit != -1 )
                {
                limit = ( limit * KImumKB ) / KImumMaxCharsInLine;
                }

            // Context is irrelevant.
            // aSelection[0] is service ID.
            CMsvProgressReporterOperation* reporter =
                CMsvProgressReporterOperation::NewL(
                    ETrue, ETrue, Session(),
                    aStatus, EMbmAvkonQgn_note_progress );
            CleanupStack::PushL( reporter );
            if( aFunctionId == KPOP3MTMConnect )
                {
                op = CMsvPop3ConnectOp::NewL(
                    reporter->RequestStatus(),
                    *reporter,
                    aSelection[ 0 ],
                    limit );
                TBuf<EProgressStringMaxLen> buffer;
                FormatConnectingServerProgressTextL( service, buffer );
                reporter->SetTitleL( buffer );
                reporter->SetSeeding( ETrue );
                }
            else
                {
                op = CImumDisconnectOperation::NewL(
                    *iMailboxApi,
                    reporter->RequestStatus(),
                    *reporter,
                    aSelection[ 0 ],
                    KPOP3MTMDisconnect,
                    KUidMsgTypePOP3 );
                }
            reporter->SetOperationL( op ); // Takes immediate ownership
            CleanupStack::Pop(); // reporter
            IMUM_OUT();
            return reporter;

            }
        case KMtmUiFunctionMessageInfo:
            IMUM_OUT();
            return ShowMessageInfoL( aStatus, aParameter );
            /*
        case KPOP3MTMPopulateAll:
            {
            //doing full partial fetch instead of bare connect
            return FetchL(aStatus, aSelection, KPOP3MTMPopulateAll);
            }
            break;
            */
        default:
            break;
        };
    IMUM_OUT();

    return CBaseMtmUi::InvokeAsyncFunctionL(
        aFunctionId, aSelection, aStatus, aParameter );
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::ShowMessageInfoL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::ShowMessageInfoL(
    TRequestStatus& aCompletionStatus,
    TDes8& /*aParameter*/)
    {
    IMUM_CONTEXT( CPop3MtmUi::ShowMessageInfoL, 0, KImumMtmLog );
    IMUM_IN();
    
    TMsgInfoMessageInfoData infoData;
    TBuf<KImumMessageInfoDateBufferLength> dateBuf;
    TBuf<KImumMessageInfoDateBufferLength> timeBuf;
    TBuf<KImumMessageInfoSizeBufferLength> sizeBuf;
    TBuf<KImumMessagePriorityBufferLength> priorityBuf;
    TBool readStoreFound = MsvEmailMtmUiUtils::SetMessageInfoDataLCC(
        infoData,BaseMtm(),*iEikonEnv,dateBuf,timeBuf,sizeBuf,priorityBuf);

    TBuf<KImumMessageInfoDateBufferLength> type;
    iEikonEnv->ReadResourceL(type,R_POP3_MESSAGE_INFO_MAIL_TYPE);
    infoData.iType.Set(type);

    CMsgInfoMessageInfoDialog* infoDialog = CMsgInfoMessageInfoDialog::NewL();
    infoDialog->ExecuteLD(infoData,CMsgInfoMessageInfoDialog::EEmailViewer);
    CleanupStack::PopAndDestroy(2); // CSI: 47 # 2 because SetMessageInfoDataLCC
    if ( readStoreFound )
        {
        CleanupStack::PopAndDestroy(2); // CSI: 47 # 2 because store found
        }
    IMUM_OUT();

    return CMsvCompletedOperation::NewL(
        Session(),
        KUidMsvLocalServiceMtm,
        KNullDesC8,
        KMsvLocalServiceIndexEntryId,
        aCompletionStatus,
        KErrNone);
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::FetchSyncL()
// Offer to fetch messages synchronously if not downloaded.
// ----------------------------------------------------------------------------
//
TInt CPop3MtmUi::FetchSyncL(const CMsvEntrySelection& aSelection)
    {
    IMUM_CONTEXT( CPop3MtmUi::FetchSyncL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Client context is irrelevant.
    TMsvId parent = KMsvNullIndexEntryId;
    CheckSelectionL(aSelection, parent);
    BaseMtm().SwitchCurrentEntryL(parent);
    CMsvEntry* clientContext = &(BaseMtm().Entry());
    CMsvEntrySelection* entries = MsvEmailMtmUiUtils::StripCompleteEntriesLC(*clientContext, aSelection);
    const TInt count = entries->Count();
    CleanupStack::PopAndDestroy();  // entries
    if(!count)
        {
        // Mesages are complete.
        return KErrNone;
        }
    if(clientContext->EntryId() != clientContext->OwningService())
        {
        // Client context must be service for FetchL().
        BaseMtm().SwitchCurrentEntryL(clientContext->OwningService());
        clientContext = &(BaseMtm().Entry());
        }
    // Go get the messages.
    CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
    waiter->iStatus = KRequestPending;
    CMsvOperation* op = FetchL(waiter->iStatus, aSelection, KFunctionIdFetchSelected);
    CleanupStack::PushL(op);
    waiter->Start();
    CActiveScheduler::Start();
    const TInt err = DisplayProgressSummary(op->ProgressL());
    CleanupStack::PopAndDestroy(2); // CSI: 47 # op, waiter
    IMUM_OUT();
    return err;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::GetResourceFileName()
// ----------------------------------------------------------------------------
//
void CPop3MtmUi::GetResourceFileName(TFileName& aFileName) const
    {
    IMUM_CONTEXT( CPop3MtmUi::GetResourceFileName, 0, KImumMtmLog );
    IMUM_IN();
    
    aFileName=KPop3uMtmUiResourceFile;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::GetProgress()
// ----------------------------------------------------------------------------
//
TInt CPop3MtmUi::GetProgress(
    const TDesC8& aProgress,
    TBuf<EProgressStringMaxLen>& aReturnString,
    TInt& aTotalEntryCount,
    TInt& aEntriesDone,
    TInt& aCurrentEntrySize,
    TInt& aCurrentBytesTrans) const
    {
    IMUM_CONTEXT( CPop3MtmUi::GetProgress, 0, KImumMtmLog );
    IMUM_IN();
    
    aReturnString.Zero();
    aTotalEntryCount=0;
    aEntriesDone=0;
    aCurrentEntrySize=0;
    aCurrentBytesTrans=0;
    if( (!aProgress.Length()) ||
        (aProgress.Size() == sizeof(TMsvLocalOperationProgress)) )
        {
        IMUM_OUT();
        return KErrNone;
        }
    const TInt progressType = ProgressType(aProgress);
    if(progressType <= TPop3Progress::EPopMaxProgressValue)
        {
        IMUM_OUT();
        return GetEngineProgress(
            aProgress,
            aReturnString,
            aTotalEntryCount,
            aEntriesDone,
            aCurrentEntrySize,
            aCurrentBytesTrans);
        }
    else
        {
        IMUM_OUT();
        return GetUiProgress(
            aProgress,
            aReturnString,
            aTotalEntryCount,
            aEntriesDone,
            aCurrentEntrySize,
            aCurrentBytesTrans);
        }
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::ProgressType()
// Find the type of the progress. Progress type is identified by the first
// TInt in the buffer.
// ----------------------------------------------------------------------------
//
TInt CPop3MtmUi::ProgressType(const TDesC8& aProgress) const
    {
    IMUM_CONTEXT( CPop3MtmUi::ProgressType, 0, KImumMtmLog );
    IMUM_IN();
    
    TPckgC<TInt> type(0);
    type.Set(aProgress.Left(sizeof(TInt)));
    const TInt progressType = type();
    __ASSERT_DEBUG(
        progressType==TPop3Progress::EPopRefreshing
        || progressType==TPop3Progress::EPopCopying
        || progressType==TPop3Progress::EPopDeleting
        || progressType==TPop3Progress::EPopConnecting
        || progressType==TPop3Progress::EPopTidying
        || progressType==TPop3Progress::EPopConnectedAndIdle
        || progressType==TPop3Progress::EPopDisconnecting
        || progressType==TPop3Progress::EPopDisconnected
        || progressType==TPop3Progress::EPopMoving
        || progressType==TPop3Progress::EPopCopyNewMail
        || progressType==TPop3Progress::EPopMoveNewMail
        || progressType==TPop3Progress::EPopCopyMailSelection
        || progressType==TPop3Progress::EPopMoveMailSelection
        || progressType==TPop3Progress::EPopCopyAllMail
        || progressType==TPop3Progress::EPopMoveAllMail
        || progressType==TPop3Progress::EPopPopulating
        || progressType==TPop3Progress::EPopCancellingOfflineOps
        || progressType==EUiProgTypeEditing
        || progressType==EUiProgTypeConnecting
        || progressType==TPop3Progress::EPopTopPopulating,
        User::Panic(KImumMtmUiPanic, EPop3MtmUiUnknownOperation));
    IMUM_OUT();
    return progressType;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::GetEngineProgress()
// ----------------------------------------------------------------------------
//
TInt CPop3MtmUi::GetEngineProgress(
    const TDesC8& aProgress,
    TBuf<EProgressStringMaxLen>& aReturnString,
    TInt& aTotalEntryCount,
    TInt& aEntriesDone,
    TInt& aCurrentEntrySize,
    TInt& aCurrentBytesTrans) const
    {
    IMUM_CONTEXT( CPop3MtmUi::GetEngineProgress, 0, KImumMtmLog );
    IMUM_IN();
    
    // Unpack the progress buffer in the appropriate POP3 progress object
    __ASSERT_ALWAYS(aProgress.Size() == sizeof(TPop3Progress), User::Panic(KImumMtmUiPanic, EPop3MtmUiNoProgress));
    TPckgBuf<TPop3Progress> paramPack;
    paramPack.Copy(aProgress);
    const TPop3Progress& progress = paramPack();
    // Extract the information on the entries which have been done
    aTotalEntryCount = progress.iTotalMsgs;
    aEntriesDone = Min(aTotalEntryCount, aTotalEntryCount - progress.iMsgsToProcess);
    aCurrentEntrySize = progress.iTotalBytes;
    aCurrentBytesTrans = progress.iBytesDone;

    // Get the progress description
    TInt resourceId = 0;
    switch(progress.iPop3Progress)
        {
    case TPop3Progress::EPopConnectedAndIdle:
    case TPop3Progress::EPopConnecting:
    case TPop3Progress::EPopDisconnecting:
    case TPop3Progress::EPopDisconnected:
        aCurrentEntrySize = 0;
        aCurrentBytesTrans = 0;
        IMUM_OUT();
        return progress.iErrorCode;     // No progress text for these states.

    case TPop3Progress::EPopTidying:
    case TPop3Progress::EPopRefreshing:
        if(progress.iTotalMsgs == 0)
            {
            // Display 'Connecting to <SERVER NAME>', rather than 'Getting header 0 of 0'
            TRAPD(err, FormatConnectingServerProgressTextL(
                progress.iServiceId, aReturnString));
            if(err == KErrNone)
                {
                err = progress.iErrorCode;
                }
            aCurrentEntrySize = 0;
            aCurrentBytesTrans = 0;
            IMUM_OUT();
            return err;
            }
        resourceId = R_POP3_UPDATING_INBOX;
        break;

    case TPop3Progress::EPopMoveNewMail:
    case TPop3Progress::EPopMoveAllMail:
    case TPop3Progress::EPopMoveMailSelection:
    case TPop3Progress::EPopMoving:
        __ASSERT_DEBUG( EFalse, User::Panic(KImumMtmUiPanic,   EPop3MtmUiMoveNotAllowed ));
        break;
    case TPop3Progress::EPopCopyNewMail:
    case TPop3Progress::EPopCopyAllMail:
    case TPop3Progress::EPopCopyMailSelection:
    case TPop3Progress::EPopCopying:
        resourceId = (progress.iTotalMsgs == 1) ?
            (R_POP3_COPYING_IMAIL_PROGRESS) : (R_POP3_COPYING_MULTI_IMAIL_PROGRESS);
        break;
    case TPop3Progress::EPopDeleting:
        resourceId = (progress.iTotalMsgs == 1) ?
            (R_POP3_DELETING_IMAIL_PROGRESS) : (R_POP3_DELETING_IMAIL_PROGRESS_MANY);
        break;
    case TPop3Progress::EPopPopulating:
    case TPop3Progress::EPopTopPopulating:
        resourceId = (progress.iTotalMsgs == 1) ?
            (R_POP3_POPULATING_SINGLE) : (R_POP3_POPULATING_MULTIPLE);
		//We don't want progressbar, but wait animation instead. This is achieved 
		//by denying progress information.
        aTotalEntryCount = 0;
	    aEntriesDone = 0;
	    aCurrentEntrySize = 0;
	    aCurrentBytesTrans = 0;
        break;
    case TPop3Progress::EPopCancellingOfflineOps:
        resourceId = (progress.iTotalMsgs == 1) ?
            (R_POP3_UNDELETING_IMAIL_PROGRESS) : (R_POP3_UNDELETING_MULTI_IMAIL_PROGRESS);
        break;
    default:
        User::Panic(KImumMtmUiPanic, EPop3MtmUiUnknownOperation);
        break;
        };
    //
    // Format the return progress string as appropriate
    if (resourceId!=0)
        {
        StringLoader::Load( aReturnString, resourceId, iCoeEnv);
        if ( resourceId != R_POP3_UPDATING_INBOX &&
             progress.iTotalMsgs>1 )
            {
            TBuf<EProgressStringMaxLen> tempBuffer;
            StringLoader::Format(
                tempBuffer,
                aReturnString,
                KImumEntriesDoneReplaceIndex,
                Min(progress.iTotalMsgs, progress.iTotalMsgs - progress.iMsgsToProcess+1) );
            StringLoader::Format(
                aReturnString,
                tempBuffer,
                KImumTotalEntriesReplaceIndex,
                progress.iTotalMsgs );
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
// CPop3MtmUi::GetUiProgress()
// ----------------------------------------------------------------------------
//
TInt CPop3MtmUi::GetUiProgress(
    const TDesC8& aProgress,
    TBuf<EProgressStringMaxLen>& aReturnString,
    TInt& aTotalEntryCount,
    TInt& aEntriesDone,
    TInt& aCurrentEntrySize,
    TInt& aCurrentBytesTrans) const
    {
    IMUM_CONTEXT( CPop3MtmUi::GetUiProgress, 0, KImumMtmLog );
    IMUM_IN();
    
    switch(ProgressType(aProgress))
        {
        case EUiProgTypeEditing:
            break;
        case EUiProgTypeConnecting:
            IMUM_OUT();
            return GetConnectionProgress(
                aProgress,
                aReturnString,
                aTotalEntryCount,
                aEntriesDone,
                aCurrentEntrySize,
                aCurrentBytesTrans);

        default:
            User::Panic(KImumMtmUiPanic, EPop3MtmUiUnknownOperation);
            break;
        }
    IMUM_OUT();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::GetConnectionProgress()
// ----------------------------------------------------------------------------
//
TInt CPop3MtmUi::GetConnectionProgress(
    const TDesC8& aProgress,
    TBuf<EProgressStringMaxLen>& aReturnString,
    TInt& /*aTotalEntryCount*/,
    TInt& /*aEntriesDone*/,
    TInt& /*aCurrentEntrySize*/,
    TInt& /*aCurrentBytesTrans*/) const
    {
    IMUM_CONTEXT( CPop3MtmUi::GetConnectionProgress, 0, KImumMtmLog );
    IMUM_IN();
    
    // Unpack the progress buffer in the appropriate progress object
    __ASSERT_ALWAYS(aProgress.Size() == sizeof(TMsvEmailConnectionProgress), User::Panic(KImumMtmUiPanic, EPop3MtmUiNoProgress));
    TPckgBuf<TMsvEmailConnectionProgress> paramPack;
    paramPack.Copy(aProgress);
    const TMsvEmailConnectionProgress& progress = paramPack();

    // Get the progress text.
    switch(progress.iState)
        {
        case TMsvEmailConnectionProgress::EInitialising:
        case TMsvEmailConnectionProgress::EConnectNetwork:
        case TMsvEmailConnectionProgress::EConnectMailbox:
            MsvEmailMtmUiUtils::CreateConnectingToText( aReturnString,
                Session(), BaseMtm().Entry().EntryId() );
            break;
        default:
            User::Panic(KImumMtmUiPanic, EPop3MtmUiUnknownProgressState);
            break;
        }
    IMUM_OUT();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::GetDeleteProgress()
// ----------------------------------------------------------------------------
//
TInt CPop3MtmUi::GetDeleteProgress(TInt aTotalMsgs, TInt aError, TBuf<EProgressStringMaxLen>& aReturnString, TInt& aTotalEntryCount, TInt& aEntriesDone,
    TInt& aCurrentEntrySize, TInt& aCurrentBytesTrans) const
    {
    IMUM_CONTEXT( CPop3MtmUi::GetDeleteProgress, 0, KImumMtmLog );
    IMUM_IN();
    
    // Delete progress is something of a special case since it reports only the total
    // number of messages to be deleted, and has no progress bar.
    aEntriesDone = 0;
    aTotalEntryCount = 0;
    aCurrentEntrySize = 0;
    aCurrentBytesTrans = 0;
    if(aTotalMsgs >= 2)
        {
        TBuf<EProgressStringMaxLen> resource;
        TRAP_IGNORE( iCoeEnv->ReadResourceL( resource, 
            R_POP3_DELETING_IMAIL_PROGRESS_MANY ) );
        aReturnString.Format(resource, aTotalMsgs);
        }
    else
        {
        TRAP_IGNORE( iCoeEnv->ReadResourceL( aReturnString, 
            R_POP3_DELETING_IMAIL_PROGRESS ) );
        }
    IMUM_OUT();
    return aError;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::DisplayProgressSummary()
// ----------------------------------------------------------------------------
//
TInt CPop3MtmUi::DisplayProgressSummary(const TDesC8& aProgress) const
    {
    IMUM_CONTEXT( CPop3MtmUi::DisplayProgressSummary, 0, KImumMtmLog );
    IMUM_IN();
    
    if( (!aProgress.Length()) || (aProgress.Size() == sizeof(TMsvLocalOperationProgress)) )
        {
        // Probably a CMsvCompletedOperation
        return KErrCancel;
        }
    const TInt progressType = ProgressType(aProgress);
    TInt err = KErrNone;

    if( (progressType > EUiProgTypeBase) && (progressType < EUiProgTypeLast) )
        {
        // Handle UI types.
        switch(progressType)
            {
            case EUiProgTypeEditing:
                IMUM_OUT();
                return KErrNone;

            case EUiProgTypeConnecting:
                {
                __ASSERT_ALWAYS(aProgress.Size() == sizeof(TMsvEmailConnectionProgress), User::Panic(KImumMtmUiPanic, EPop3MtmUiNoProgress));
                TPckgBuf<TMsvEmailConnectionProgress> uiParamPack;
                uiParamPack.Copy(aProgress);
                const TMsvEmailConnectionProgress& uiProgress = uiParamPack();
                err = uiProgress.iErrorCode;
                if( (err == KErrNone) || (err == KErrCancel) )
                    {
                    IMUM_OUT();
                    // Success.
                    return err;
                    }
                }
                break;
            default:
                User::Panic(KImumMtmUiPanic, EPop3MtmUiUnknownProgressType);
                break;
            }
        }
    else
        {
        // Handle POPS or POPC types.
        __ASSERT_ALWAYS(aProgress.Size() == sizeof(TPop3Progress), User::Panic(KImumMtmUiPanic, EPop3MtmUiNoProgress));
        TPckgBuf<TPop3Progress> paramPack;
        paramPack.Copy(aProgress);
        const TPop3Progress& progress = paramPack();
        err = progress.iErrorCode;
        if( (err == KErrNone) || (err == KErrCancel) )
            {
            IMUM_OUT();
            // Success.
            return err;
            }

        switch((TInt)progress.iPop3Progress)
            {
        case TPop3Progress::EPopConnectedAndIdle:
        case TPop3Progress::EPopConnecting:
        case TPop3Progress::EPopTidying:
        case TPop3Progress::EPopRefreshing:
        case TPop3Progress::EPopCopying:
        case TPop3Progress::EPopCopyNewMail:
        case TPop3Progress::EPopCopyMailSelection:
        case TPop3Progress::EPopCopyAllMail:
        case TPop3Progress::EPopDeleting:
        case TPop3Progress::EPopDisconnecting:
        case TPop3Progress::EPopDisconnected:
        case TPop3Progress::EPopMoving:
        case TPop3Progress::EPopMoveNewMail:
        case TPop3Progress::EPopMoveMailSelection:
        case TPop3Progress::EPopMoveAllMail:
        case TPop3Progress::EPopPopulating:
        case TPop3Progress::EPopTopPopulating:
            break;
        default:
            User::Panic(KImumMtmUiPanic, EPop3MtmUiUnknownProgressType);
            break;
            }
        }
    IMUM_OUT();
    return DisplayProgressErrorAlert( err );
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::DisplayProgressErrorAlert()
// ----------------------------------------------------------------------------
//
TInt CPop3MtmUi::DisplayProgressErrorAlert( TInt aErrCode ) const
    {
    IMUM_CONTEXT( CPop3MtmUi::DisplayProgressErrorAlert, 0, KImumMtmLog );
    IMUM_IN();
    
    if ( aErrCode == KErrDndNameNotFound )
        {
        TRAP_IGNORE( CIMSSettingsNoteUi::ShowQueryL(
            R_MAIL_ERR_NO_IN_SRVR, R_EMAIL_INFORMATION_QUERY ) );
        }
    else if ( aErrCode != KErrGeneral &&
         aErrCode != KErrEtelBusyDetected &&
         aErrCode != KErrAbort )
        // general errors displayed by other components. for ex. phone app...
        {
        //remap error code so we get some sensible error note instead of system
        // error which isn't even displayed.
        if ( aErrCode == KErrDisconnected )
            {
            aErrCode = KPop3ProblemWithRemotePopServer;
            }
        if ( aErrCode == KErrGprsServicesNotAllowed )
            {
            aErrCode = KImskErrorActiveSettingIsDifferent;
            }
        if ( aErrCode == KErrCouldNotConnect )
            {
            aErrCode = KPop3CannotConnect;
            }
        if ( aErrCode == KImskSSLTLSNegotiateFailed )            
			{
			//we use IMAP error code because it has already been mapped in CTextResolver
			//and works for Pop3 too.
			aErrCode = KErrImapServerNoSecurity;
			}

        TRAP_IGNORE( iErrorResolver->ShowGlobalErrorNoteL( aErrCode ) );
        }
    IMUM_OUT();
    return aErrCode;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::FormatConnectingServerProgressTextL()
// ----------------------------------------------------------------------------
//
void CPop3MtmUi::FormatConnectingServerProgressTextL(
    TMsvId aServiceId,
    TDes& aReturnString) const
    {
    IMUM_CONTEXT( CPop3MtmUi::FormatConnectingServerProgressTextL, 0, KImumMtmLog );
    IMUM_IN();
    
    if(iServerNameCache.iService != aServiceId)
        {
        TMsvEntry serviceEntry;
        TMsvId serviceId;
        User::LeaveIfError( Session().GetEntry( aServiceId, serviceId, serviceEntry ) );
        HBufC* text = StringLoader::LoadLC(
            R_EMAIL_CONNECTING_SERVER, serviceEntry.iDetails, iCoeEnv );
        aReturnString.Copy( (*text).Left( EProgressStringMaxLen ) );
        CleanupStack::PopAndDestroy(); // text
        iServerNameCache.iService = aServiceId;
        iServerNameCache.iConnectingServerTxt = aReturnString;
        }
    else
        {
        aReturnString = iServerNameCache.iConnectingServerTxt;
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::LaunchEditorApplicationL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::LaunchEditorApplicationL(
    TRequestStatus& aStatus,
    const TEditorParameters& aParams)
    {
    IMUM_CONTEXT( CPop3MtmUi::LaunchEditorApplicationL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Get the app name
    HBufC* appName = (aParams.iFlags & EMsgReadOnly) ?
        (GetViewerFileNameL()) : (GetEditorFileNameL());
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
    return CImumEditOperation::NewL(
        *iMailboxApi, aStatus, Type(), editParams, ackReceipts);
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::ViewMessageL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::ViewMessageL(TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CPop3MtmUi::ViewMessageL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Context is message to view.
    const TMsvEntry& context=BaseMtm().Entry().Entry();
    TEditorParameters editorParams;
    editorParams.iId=context.Id();
    editorParams.iFlags|=EMsgReadOnly;

    TMsvEmailEntry emailEntry=BaseMtm().Entry().Entry();
    if ( emailEntry.DisconnectedOperation() == EDisconnectedDeleteOperation )
        {
        // remove deleted flag
        TInt error = MsvEmailMtmUiUtils::RemoveOfflineDeleteL(
            BaseMtm(),
            KPOP3MTMCancelOfflineOperations,
            emailEntry.Id() );
        User::LeaveIfError( error );
        }

    if(context.Complete())
        {
        IMUM_OUT();
        return LaunchEditorApplicationL(aStatus, editorParams);
        }
    IMUM_OUT();

    return RetrieveAndEditMessageL(aStatus, editorParams);
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::EditSmtpServiceL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::EditSmtpServiceL(
    TRequestStatus& aStatus,
    TMsvId aServiceEntry) const
    {
    IMUM_CONTEXT( CPop3MtmUi::EditSmtpServiceL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Context is irrelevant.
    CMsvEntry* centry = Session().GetEntryL(aServiceEntry);
    CleanupStack::PushL(centry);
    CMtmStore* mtmStore = CMtmStore::NewL(Session());
    CleanupStack::PushL(mtmStore);
    CBaseMtmUi& smtpUi = mtmStore->GetMtmUiAndSetContextLC(centry->Entry());
    CMsvOperation* op = smtpUi.EditL(aStatus);
    CleanupStack::PopAndDestroy(3); // smtpUi, mtmStore, centry
    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::CheckEntryL()
// ----------------------------------------------------------------------------
//
void CPop3MtmUi::CheckEntryL(const TMsvEntry& aEntry) const
// Checks that that the entry is a POP3 entry, and that it is
// of a type that the POP3 MTM supports; i.e. message or service entries.
    {
    IMUM_CONTEXT( CPop3MtmUi::CheckEntryL, 0, KImumMtmLog );
    IMUM_IN();
    
    if( ((aEntry.iType.iUid!=KUidMsvMessageEntryValue) &&
         (aEntry.iType.iUid!=KUidMsvServiceEntryValue)) ||
        (aEntry.iMtm!=Type()) )
        {
        User::Leave(KErrNotSupported);
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::CheckSelectionL()
// Checks that the entries in the passed selection are of the correct mtm and are all
// children of the same entry.
// Returns the parent of the entries
// ----------------------------------------------------------------------------
void CPop3MtmUi::CheckSelectionL(
    const CMsvEntrySelection& aSelection,
    TMsvId& aParent) const
    {
    IMUM_CONTEXT( CPop3MtmUi::CheckSelectionL, 0, KImumMtmLog );
    IMUM_IN();
    
    __ASSERT_ALWAYS(aSelection.Count(), User::Panic(KImumMtmUiPanic, EPop3MtmUiSelectionIsEmpty));
    // Get the first entry, and then it's parent
    CMsvEntry* entry=Session().GetEntryL(aSelection[0]);
    CleanupStack::PushL(entry);
    CheckEntryL(entry->Entry());
    const TUid mtm=entry->Entry().iMtm;
    __ASSERT_ALWAYS(mtm==Type(), User::Panic(KImumMtmUiPanic, EPop3MtmUiWrongMtm));
    aParent=entry->Entry().Parent();
    entry->SetEntryL(aParent);
    //
    // All entries in the selection should be from the same parent, so make sure this is the case
    for (TInt cc=1; cc < aSelection.Count(); ++cc)
        {
        TMsvEntry tentry;
        TRAPD(err, tentry=entry->ChildDataL(aSelection[cc]));
        if(err == KErrNotFound)
            {
            // All the messages from the selection not found 'under' one parent
            User::Panic(KImumMtmUiPanic, EPop3MtmUiSelectionOfMoreThanOneParent);
            }
        if(tentry.iType.iUid == KUidMsvMessageEntryValue)
            {
            // POP3 MTM UI only supports selections of messages, not services
            User::Leave(KErrNotSupported);
            }
        if(tentry.iMtm != mtm)
            {
            // Given entry of the wrong mtm
            User::Panic(KImumMtmUiPanic, EPop3MtmUiSelectionWithMessageOfWrongMtm);
            }
        }
    CleanupStack::PopAndDestroy();// entry
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::SingleEntrySelectionLC()
// ----------------------------------------------------------------------------
//
CMsvEntrySelection* CPop3MtmUi::SingleEntrySelectionLC(TMsvId aId) const
    {
    IMUM_CONTEXT( CPop3MtmUi::SingleEntrySelectionLC, 0, KImumMtmLog );
    IMUM_IN();
    
    CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
    CleanupStack::PushL(selection);
    selection->AppendL(aId);
    IMUM_OUT();
    return selection;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::StripInvalidEntriesLC()
// ----------------------------------------------------------------------------
//
CMsvEntrySelection* CPop3MtmUi::StripInvalidEntriesLC(
    const CMsvEntrySelection& aSelection) const
    {
    IMUM_CONTEXT( CPop3MtmUi::StripInvalidEntriesLC, 0, KImumMtmLog );
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
// CPop3MtmUi::DeleteFromL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::DeleteFromL(
    const CMsvEntrySelection& aSelection,
    TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CPop3MtmUi::DeleteFromL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Context is parent of entries to be deleted.
    CMsvEntrySelection* validEntries=StripInvalidEntriesLC(aSelection);
    CMsvEntrySelection* entries=
        MsvEmailMtmUiUtils::StripDeletedEntriesLC(
        BaseMtm().Entry(), *validEntries);
        // Current context is parent.

    CMsvOperation* op=NULL;
    const TInt numberOfItems = entries->Count();
    IMUM1(0, "TInt numberOfItems( %d )", numberOfItems);
    
    if( numberOfItems )
        {

        TInt chosedDelMode = KErrNotFound;
        const TInt delMode = GetMailDeletionModeL();
        TBool dlgResult = ETrue;

        IMUM0(0, "Many items");
        if ( delMode == EIMASMailDeletionAlwaysAsk )
            {
            
            CImumListQueryDialog* dlg = new (ELeave) CImumListQueryDialog(&chosedDelMode);
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
            dlgResult = dlg->RunLD( );
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
            chosedDelMode = KPop3MtmUiDeleteMessagesLocally;  
            }
            


        if( dlgResult )
            {
            IMUM0(0, "Starting delete operation");
            CMsvProgressReporterOperation* reporter = NULL;
            
            if ( chosedDelMode == KPop3MtmUiDeleteMessagesLocally )
                {
                // Show a note to make sure that user understands that header will still be seen
                // even if mail is deleted from the phone, only showed if setting is always ask
                if ( delMode == EIMASMailDeletionAlwaysAsk ) 
                    {
                    CIMSSettingsNoteUi::ShowNoteL( R_IMUM_HEADER_WILL_REMAIN_PHONE,
                        EIMSInformationNote, ETrue, CAknNoteDialog::EShortTimeout );
                    }

                reporter = CMsvProgressReporterOperation::NewL(Session(), aStatus, EMbmAvkonQgn_note_erased);
                CleanupStack::PushL(reporter);
                
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
                reporter = CMsvProgressReporterOperation::NewL(Session(), aStatus, EMbmAvkonQgn_note_erased);
                CleanupStack::PushL(reporter);
                
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
                KNullDesC8,
                KMsvLocalServiceIndexEntryId,
                aStatus,
                KErrCancel );
            }
        }

    if(!op)
        {
        op=CMsvCompletedOperation::NewL(
            Session(),
            Type(),
            KNullDesC8,
            KMsvLocalServiceIndexEntryId,
            aStatus,
            KErrNone );
        }
    CleanupStack::PopAndDestroy(2);//entries, validEntries
    
    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::UnDeleteFromL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::UnDeleteFromL(
    const CMsvEntrySelection& aSelection,
    TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CPop3MtmUi::UnDeleteFromL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Context is parent of entries to be undeleted.
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
        TBuf8<1> params;
        params.Zero();
        CMsvProgressReporterOperation* reporter =
            CMsvProgressReporterOperation::NewL(
            Session(), aStatus, EMbmAvkonQgn_note_progress );
        CleanupStack::PushL(reporter);
        reporter->SetTitleL( R_POP3_UNDELETING_IMAIL_PROGRESS );
        reporter->SetSeeding(EFalse);
        reporter->SetProgressVisibilityDelay( EFalse );
        CMsvOperation* op = BaseMtm().InvokeAsyncFunctionL(
            KPOP3MTMCancelOfflineOperations,
            aSelection,
            params,
            reporter->RequestStatus());
        reporter->SetOperationL(op); // Takes immediate ownership
        CleanupStack::Pop(); // reporter
        IMUM_OUT();
        return reporter;
        }
    else
        {
        IMUM_OUT();
        return CMsvCompletedOperation::NewL(
            Session(),
            Type(),
            KNullDesC8,
            KMsvLocalServiceIndexEntryId,
            aStatus,
            KErrCancel);
        }
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::ForwardL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::ForwardL(
    TMsvId aDestination,
    TMsvPartList aPartList,
    TRequestStatus& aCompletionStatus)
    {
    IMUM_CONTEXT( CPop3MtmUi::ForwardL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Context is the message to forward.
    const TMsvEntry& entry = BaseMtm().Entry().Entry();
    CheckEntryL(entry);
    if(entry.iType.iUid != KUidMsvMessageEntryValue)
        {
        User::Leave(KErrNotSupported);
        }
    TEditorParameters editorParams;
    editorParams.iFlags|=EMsgForwardMessage;
    editorParams.iId = entry.Id();
    editorParams.iPartList = aPartList;
    editorParams.iDestinationFolderId = aDestination;
    if(entry.Complete())
        {
        IMUM_OUT();
        return LaunchEditorApplicationL(aCompletionStatus, editorParams);
        }
    IMUM_OUT();
    return RetrieveAndEditMessageL(aCompletionStatus, editorParams);
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::ReplyL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::ReplyL(
    TMsvId aDestination,
    TMsvPartList aPartlist,
    TRequestStatus& aCompletionStatus)
    {
    IMUM_CONTEXT( CPop3MtmUi::ReplyL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Context is the message to reply to.
    TMsvEntry entry = BaseMtm().Entry().Entry();
    CheckEntryL(entry);
    if(entry.iType.iUid != KUidMsvMessageEntryValue)
        {
        User::Leave(KErrNotSupported);
        }
    TEditorParameters editorParams;
    editorParams.iFlags|=(aPartlist&KMsvMessagePartRecipient)?
        EMsgReplyToMessageAll:EMsgReplyToMessageSender;
    editorParams.iId=entry.Id();
    editorParams.iPartList = aPartlist;
    editorParams.iDestinationFolderId = aDestination;

    if ( entry.Unread() )
        {
        entry.SetUnread( EFalse );

        CMuiuOperationWait* wait =
            CMuiuOperationWait::NewLC( EActivePriorityDefault );
        CMsvOperation* op = BaseMtm().Entry().ChangeL( entry, wait->iStatus );
        wait->Start();
        CleanupStack::PopAndDestroy( wait );
        wait = NULL;

        delete op;
        op = NULL;
        }

    if(entry.Complete())
        {
        IMUM_OUT();
        return LaunchEditorApplicationL(aCompletionStatus, editorParams);
        }
    IMUM_OUT();
    return RetrieveAndEditMessageL(aCompletionStatus, editorParams);
}

// ----------------------------------------------------------------------------
// CPop3MtmUi::RetrieveAndEditMessageL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::RetrieveAndEditMessageL(
    TRequestStatus& aStatus,
    const TEditorParameters& aEditorParams)
    {
    IMUM_CONTEXT( CPop3MtmUi::RetrieveAndEditMessageL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Context is irrelevant.
    CMsvEntry* msgEntry = Session().GetEntryL(aEditorParams.iId);
    CleanupStack::PushL(msgEntry);
    const TMsvEntry& tentry = msgEntry->Entry();
    const TMsvId destination = tentry.Parent();
    
    CMsvSession& session = Session();

    // Check available memory.
    if( !MsvEmailMtmUiUtils::CheckAvailableDiskSpaceForDownloadL(
            tentry.iSize, *iEikonEnv, session ) )
        {
        CleanupStack::PopAndDestroy();  // msgEntry
        return CMsvCompletedOperation::NewL(
            Session(),
            Type(),
            KNullDesC8,
            KMsvLocalServiceIndexEntryId,
            aStatus,
            KErrCancel);
        }

    // If not online, ask user.
    msgEntry->SetEntryL(msgEntry->OwningService());
    const TBool connected = msgEntry->Entry().Connected();
    if(!connected)
        {
        // Prompt user.
        if ( !CIMSSettingsNoteUi::ShowQueryL(
            R_EMAIL_RETRIEVE_1_TEXT, R_EMAIL_CONFIRMATION_QUERY ) )
            {
            CleanupStack::PopAndDestroy();  // msgEntry
            return CMsvCompletedOperation::NewL(
                Session(),
                Type(),
                KNullDesC8,
                KMsvLocalServiceIndexEntryId,
                aStatus,
                KErrNone);
            }
        }

    TMsvId id = msgEntry->Entry().Id();
    CleanupStack::PopAndDestroy();  // msgEntry

    //Offline checks VARIATION START
    if ( iFeatureFlags->GF( EMailFeatureOfflineMode ) )
        {
        TBool offline = EFalse;

        if( !connected &&
            !iMailboxApi->MailboxUtilitiesL().HasWlanConnectionL( id ) )
            {
            offline = MsvEmailMtmUiUtils::DoOfflineChecksL( id );
            }

        if( offline )
            {
            return CMsvCompletedOperation::NewL(Session(), Type(), KNullDesC8, KMsvLocalServiceIndexEntryId, aStatus, KErrCancel);
            }
        }//if
    //VARIATION END

    const TBool ackReceipts = AcknowledgeReceiptsL(aEditorParams.iId);

    CMsvProgressReporterOperation* reporter =
        CMsvProgressReporterOperation::NewL(
            ETrue,
            ETrue,
            Session(),
            aStatus,
            EMbmAvkonQgn_note_progress );
    CleanupStack::PushL(reporter);
    
    // If already connected, display the "Retrieving..." note right away.
     
    if ( connected )
    	reporter->SetTitleL( R_POP3_POPULATING_SINGLE );
    
    else
     	{
     	TBuf<EProgressStringMaxLen> title;
     	MsvEmailMtmUiUtils::CreateConnectingToText( title,
             *iMsvSession, destination );
     	reporter->SetTitleL( title );
      	}
    
     HBufC* appName = (aEditorParams.iFlags & EMsgReadOnly) ? (GetViewerFileNameL()) : (GetEditorFileNameL());
    TEmailEditParams fetchAndEditParams;
    fetchAndEditParams.iPreferences = Preferences();
    fetchAndEditParams.iEditorFileName = *appName;
    fetchAndEditParams.iEditorParams = aEditorParams;

    if(fetchAndEditParams.iPreferences & EMtmUiFlagEditorPreferEmbedded)
        {
        fetchAndEditParams.iEditorParams.iFlags |= EMsgLaunchEditorEmbedded;
        }
    if(!(fetchAndEditParams.iPreferences & EMtmUiFlagEditorNoWaitForExit))
        {
        fetchAndEditParams.iEditorParams.iFlags |= EMsgLaunchEditorThenWait;
        }

    CImumFetchAndEditOperation* fetchAndEdit =
        CImumFetchAndEditOperation::NewL(
        *iMailboxApi,
        reporter->RequestStatus(),
        *reporter,
        fetchAndEditParams,
        ackReceipts);
    CleanupStack::PushL(fetchAndEdit);

    CMsvEntrySelection* sel = SingleEntrySelectionLC(aEditorParams.iId);
    TImPop3GetMailInfo info;
    info.iMaxEmailSize = KMaxTInt32;
    info.iDestinationFolder = destination;
    CMsvPop3FetchOperation* fetch =
        CMsvPop3FetchOperation::NewL(
        *iMailboxApi,
        fetchAndEdit->RequestStatus(),
        *reporter,
        KFunctionIdFetchSelected,
        destination,
        info,
        *sel);
    CleanupStack::PopAndDestroy();      // sel

    fetchAndEdit->SetFetchOpL(fetch);
    CleanupStack::Pop();    // fetchAndEdit
    reporter->SetOperationL(fetchAndEdit); // Takes immediate ownership
    CleanupStack::Pop(); // reporter
    IMUM_OUT();
    return reporter;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::GetMailL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::GetMailL(TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CPop3MtmUi::GetMailL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Client MTM context is service.
    const TMsvEntry& entry = BaseMtm().Entry().Entry();
    CheckEntryL(entry);
    if(entry.iType.iUid != KUidMsvServiceEntryValue)
        {
        User::Leave(KErrNotSupported);
        }
    const TMsvId serviceId = entry.Id();
    const CImPop3Settings& settings = GetAccountSettingsL(serviceId);
    const TPop3GetMailOptions opts = settings.GetMailOptions();

    //limit for for connect operation to be used with populate
    TInt limit = settings.PopulationLimit();
    // -1 means body and attachment, pass it to Symbian mail engine
    if ( limit != -1 )
        {
        limit = ( limit * KImumKB ) / KImumMaxCharsInLine;
        }

    CMsvProgressReporterOperation* reporter =
        CMsvProgressReporterOperation::NewL(
        Session(), aStatus, EMbmAvkonQgn_note_progress);
    CleanupStack::PushL(reporter);

    CMsvOperation* op = NULL;
    switch(opts)
        {
        case EGetPop3EmailHeaders:
            {
            op = CMsvPop3ConnectOp::NewL(
                reporter->RequestStatus(),
                *reporter,
                serviceId,
                limit);
            TBuf<EProgressStringMaxLen> buffer;
            FormatConnectingServerProgressTextL( serviceId, buffer );
            reporter->SetTitleL( buffer );
            reporter->SetSeeding( ETrue );
            }
            break;

        case EGetPop3EmailMessages:
            {
            TImPop3GetMailInfo info;
            info.iMaxEmailSize = settings.MaxEmailSize();
            info.iDestinationFolder = serviceId;

            CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
            CleanupStack::PushL(sel);

            op = CMsvPop3FetchOperation::NewL(
                *iMailboxApi,
                reporter->RequestStatus(),
                *reporter,
                KFunctionIdFetchNew,
                serviceId,
                info,
                *sel);
            CleanupStack::PopAndDestroy();      // sel
            }
            break;

        default:
            break;
        }
    reporter->SetOperationL(op); // Takes immediate ownership
    CleanupStack::Pop(); // reporter
    IMUM_OUT();
    return reporter;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::FetchL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CPop3MtmUi::FetchL(
    TRequestStatus& aStatus,
    const CMsvEntrySelection& aSel,
    TInt aFunctionId )
    {
    IMUM_CONTEXT( CPop3MtmUi::FetchL, 0, KImumMtmLog );
    IMUM_IN();
    
    // Client MTM context is service.
    const TMsvEntry& serviceEntry = BaseMtm().Entry().Entry();
    CheckEntryL( serviceEntry );
    if ( serviceEntry.iType.iUid != KUidMsvServiceEntryValue )
        {
        User::Leave(KErrNotSupported);
        }

    // Create reporter instance to keep track of the process
    CMsvProgressReporterOperation* reporter =
        CMsvProgressReporterOperation::NewL( ETrue, ETrue, Session(), aStatus,
            EMbmAvkonQgn_note_progress );
    CleanupStack::PushL( reporter );

    // Prepare the parameters to fetch operation
    TImPop3GetMailInfo info;
    info.iMaxEmailSize = KMaxTInt32;
    info.iDestinationFolder = serviceEntry.Id();

    // Create the actual fetching operation
    CMsvOperation* op = CMsvPop3FetchOperation::NewL( *iMailboxApi,
        reporter->RequestStatus(), *reporter, aFunctionId,
        info.iDestinationFolder, info, aSel );

    reporter->SetOperationL( op ); // Takes immediate ownership
    CleanupStack::Pop(); // reporter
    IMUM_OUT();
    return reporter;
    }

// ----------------------------------------------------------------------------
// CPop3MtmUi::GetAccountSettingsL()
// ----------------------------------------------------------------------------
//
const CImPop3Settings& CPop3MtmUi::GetAccountSettingsL(TMsvId aId) const
    {
    IMUM_CONTEXT( CPop3MtmUi::GetAccountSettingsL, 0, KImumMtmLog );
    IMUM_IN();
    
    CPop3ClientMtm& clientMtm = Pop3ClientMtm();
    if(clientMtm.HasContext())
        {
        if(clientMtm.Entry().EntryId() != aId)
            {
            clientMtm.SwitchCurrentEntryL(aId);
            }
        }
    else
        {
        clientMtm.SetCurrentEntryL(Session().GetEntryL(aId));
        }
    clientMtm.RestoreSettingsL();
    IMUM_OUT();
    return clientMtm.Settings();
    }

