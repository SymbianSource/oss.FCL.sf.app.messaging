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
*       SMTP MTM UI
*
*/


// MODULE - SMTP MTM UI

// Standard includes
#include <apacmdln.h>   //CApaCommandLine
#include <apgcli.h>
#include <apaid.h>
#include <txtrich.h>    //CRichText
#include <eikdialg.h>   //CEikDialog
#include <eikenv.h>     //CEikonEnv
#include <eikrutil.h>   //EikResourceUtils
#include <MtmExtendedCapabilities.hrh>
#include <akninputblock.h> // CAknInputBlock
#include <StringLoader.h>
#include <avkon.mbg> // bitmaps for progress
#include <etelpckt.h>
#include <aknViewAppUi.h> //CAknViewAppUi
#include <eikapp.h>     //CEikApplication
#include <aknclearer.h> // CAknLocalScreenClearer


// SMTP & POP includes
#include <smtcmtm.h>
#include <smtpset.h>
#include <pop3set.h>
#include <imapset.h>
#include <etel.h> // KErrEtelBusyDetected

// Messaging includes
#include <MuiuMsgEditorLauncher.h>
#include <muiumsvuiserviceutilitiesinternal.h>
#include <mtmuidef.hrh>
#include <imum.rsg>
#include <MuiuMsvProgressReporterOperation.h>
#include <iapprefs.h>
#include <ImumInternalApi.h>                        // CImumInternalApi

// Specific includes
#include "SmtpMtmUi.h"
#include "ImumMtmLogging.h"
#include "ImumPanic.h"
#include <imum.rsg>
#include "EmailPreCreation.h"
#include "SmtpCreateNewOp.h"
#include "SmtpCancelSendingOp.h"
#include "SmtpCopyMoveOp.h"
#include "SmtpResetPreCreationOp.h"

#include "EmailFeatureUtils.h"
#include "EmailUtils.H"
#include "MsvEmailConnectionProgressProvider.h"
#include "EmailEditorViewerUids.h"

#include "SenduiMtmUids.h"
#include "ComDbUtl.h"

#include <Muiumsginfo.h>
#include <Muiumsginfo.hrh>
#include <MuiuOperationWait.h> // CMuiuOperationWait
#include <messagingvariant.hrh>
#include "IMSSettingsNoteUi.h"
#include <ImumInSettingsData.h>             // CImumInSettingsData
#include <ImumInSettingsDataCollection.h>   // CImumInSettingsDataCollection
#include <ImumInMailboxServices.h>          // MImumInMailboxServices
#include <ImumInHealthServices.h>           // MImumInHealthServices
#include "ImumMboxSettingsUtils.h"          // ImumMboxSettingsUtils
#include "ImumCreatorPredefinedMailbox.h"   // CImumCreatorPredefinedMailbox

// CONSTANTS
const TInt KImumEntriesDoneReplaceIndex     = 0;
const TInt KImumTotalEntriesReplaceIndex    = 1;
const TInt KImumMessageInfoDateBufferLength = 30;
const TInt KImumMessageInfoSizeBufferLength = 20;
const TInt KImumMessagePriorityBufferLength = 32;
_LIT(KSmtpuMtmUiResourceFile,"IMUM");

const TUid KMceMessageListViewId    = { 0x02 };
const TUid KMceMainViewListViewId   = { 0x01 };
const TInt KMceUid = 0x100058C5;

// ----------------------------------------------------------------------------
// CSmtpMtmUi::NewSMTUMtmUiL()
// ----------------------------------------------------------------------------
EXPORT_C CBaseMtmUi* NewSMTUMtmUiL(CBaseMtm& aMtm, CRegisteredMtmDll& aRegisteredDll)
    {
    return CSmtpMtmUi::NewL(aMtm, aRegisteredDll);
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::NewL()
// ----------------------------------------------------------------------------
CSmtpMtmUi* CSmtpMtmUi::NewL(CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll)
    {
    IMUM_STATIC_CONTEXT( CSmtpMtmUi::NewL, 0, mtm, KImumMtmLog );
    IMUM_IN();

    CSmtpMtmUi* self=new(ELeave) CSmtpMtmUi(aBaseMtm, aRegisteredMtmDll);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    IMUM_OUT();
    return self;
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::CSmtpMtmUi()
// ----------------------------------------------------------------------------
CSmtpMtmUi::CSmtpMtmUi(CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll)
    :
    CImumMtmBaseMtmUi( aBaseMtm, aRegisteredMtmDll )
    {
    IMUM_CONTEXT( CSmtpMtmUi::CSmtpMtmUi, 0, KImumMtmLog );
    IMUM_IN();

    //don't wait for msg editors to exit.
    iFlags = EMtmUiFlagEditorNoWaitForExit;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::ConstructL()
// ----------------------------------------------------------------------------
void CSmtpMtmUi::ConstructL()
    {
    IMUM_CONTEXT( CSmtpMtmUi::ConstructL, 0, KImumMtmLog );
    IMUM_IN();

    // Construct own base MTM UI
    CImumMtmBaseMtmUi::ConstructL();

    // Create a 'message pre-creator'.
    iPreCreate = CEmailPreCreation::NewL( *iMailboxApi );

    //VARIATION START
    iNewSecureSettingOn =
        MsvEmailMtmUiFeatureUtils::LocalFeatureL(
            KCRUidMuiuVariation, KMuiuEmailConfigFlags,
            KEmailFeatureIdSecureSettings );
    //VARIATION END
    IMUM_OUT();

    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::ResolveEditorFileNameL()
// ----------------------------------------------------------------------------
void CSmtpMtmUi::ResolveEditorFileNameL()
    {
    IMUM_CONTEXT( CSmtpMtmUi::ResolveEditorFileNameL, 0, KImumMtmLog );
    IMUM_IN();

    __ASSERT_DEBUG(
        iEditorFilename == NULL,
            User::Panic(KImumMtmUiPanic, ESmtpMtmUiEditorFilenameDuplicate ) );
    iEditorFilename = MsvUiEditorUtilities::ResolveAppFileNameL(
        KUidMsgInternetMailEditor );
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::ResolveViewerFileNameL()
// ----------------------------------------------------------------------------
void CSmtpMtmUi::ResolveViewerFileNameL()
    {
    IMUM_CONTEXT( CSmtpMtmUi::ResolveViewerFileNameL, 0, KImumMtmLog );
    IMUM_IN();

    __ASSERT_DEBUG(
        iViewerFilename == NULL,
            User::Panic(KImumMtmUiPanic, ESmtpMtmUiEditorFilenameDuplicate ) );
    iViewerFilename = MsvUiEditorUtilities::ResolveAppFileNameL(
        KUidMsgInternetMailViewer );
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::GetResourceFileName()
// ----------------------------------------------------------------------------
void CSmtpMtmUi::GetResourceFileName( TFileName& aFileName ) const
    {
    IMUM_CONTEXT( CSmtpMtmUi::GetResourceFileName, 0, KImumMtmLog );
    IMUM_IN();

    aFileName=KSmtpuMtmUiResourceFile;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::~CSmtpMtmUi()
// ----------------------------------------------------------------------------
CSmtpMtmUi::~CSmtpMtmUi()
    {
    IMUM_CONTEXT( CSmtpMtmUi::~CSmtpMtmUi, 0, KImumMtmLog );
    IMUM_IN();

    delete iPreCreate;
    iPreCreate = NULL;
    delete iEditorFilename;
    iEditorFilename = NULL;
    delete iViewerFilename;
    iViewerFilename = NULL;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::QueryCapability()
// ----------------------------------------------------------------------------
TInt CSmtpMtmUi::QueryCapability(TUid aCapability, TInt& aResponse)
    {
    IMUM_CONTEXT( CSmtpMtmUi::QueryCapability, 0, KImumMtmLog );
    IMUM_IN();

    if ( aCapability.iUid == KUidMsvMtmUiQueryMessagingInitialisation )
        {
        aResponse=ETrue;
        IMUM_OUT();
        return KErrNone;
        }
    else if( aCapability.iUid == KUidMsvMtmQuerySupportValidateService )
        {
        aResponse=ETrue;
        IMUM_OUT();
        return KErrNone;
        }
    else if ( aCapability.iUid == KUidMsvMtmQuerySupportValidateSelection )
        {
        aResponse=ETrue;
        IMUM_OUT();
        return KErrNone;
        }
    else
        {
        IMUM_OUT();
        return CBaseMtmUi::QueryCapability( aCapability, aResponse );
        }
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::InvokeAsyncFunctionL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::InvokeAsyncFunctionL(
    TInt aFunctionId,
    const CMsvEntrySelection& aSelection,
    TRequestStatus& aStatus,
    TDes8& aParameter )
    {
    IMUM_CONTEXT( CSmtpMtmUi::InvokeAsyncFunctionL, 0, KImumMtmLog );
    IMUM_IN();

    if(aFunctionId == KMtmUiFunctionMessageInfo)
        {
        return ShowMessageInfoL(aStatus, aParameter);
        }
    if ( aFunctionId == KMtmUiFunctionPreCreateMessage )
        {
        CEmailPreCreation* preCreate = CEmailPreCreation::NewL( *iMailboxApi );
        CleanupStack::PushL( preCreate );
        CMsvOperation* op =
            preCreate->DeleteAllPreCreatedEmailsL( aSelection.At(0), aStatus );
        CleanupStack::PopAndDestroy( preCreate );
        IMUM_OUT();

        return op;
        }
    IMUM_OUT();
    return CBaseMtmUi::InvokeAsyncFunctionL( aFunctionId,
                                             aSelection,
                                             aStatus,
                                             aParameter);
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::ShowMessageInfoL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::ShowMessageInfoL(
    TRequestStatus& aCompletionStatus,
    TDes8& /*aParameter*/ )
    {
    IMUM_CONTEXT( CSmtpMtmUi::ShowMessageInfoL, 0, KImumMtmLog );
    IMUM_IN();

    TMsgInfoMessageInfoData infoData;
    TBuf<KImumMessageInfoDateBufferLength> dateBuf;
    TBuf<KImumMessageInfoDateBufferLength> timeBuf;
    TBuf<KImumMessageInfoSizeBufferLength> sizeBuf;
    TBuf<KImumMessagePriorityBufferLength> priorityBuf;
    MsvEmailMtmUiUtils::SetMessageInfoDataLCCCCC(
        infoData,BaseMtm(),
        *iEikonEnv,
        dateBuf,
        timeBuf,
        sizeBuf,
        priorityBuf
        );

    TBuf<KImumMessageInfoDateBufferLength> type;
    StringLoader::Load( type, R_SMTP_MESSAGE_INFO_MAIL_TYPE, iCoeEnv );
    infoData.iType.Set( type );

    CMsgInfoMessageInfoDialog* infoDialog = CMsgInfoMessageInfoDialog::NewL();
    infoDialog->ExecuteLD( infoData, CMsgInfoMessageInfoDialog::EEmailEditor );
    CleanupStack::PopAndDestroy( 5 ); // CSI: 47 # 5 C's on end of SetMessageInfoDataLCCCCC
    IMUM_OUT();

    return CMsvCompletedOperation::NewL(
        Session(),
        KUidMsvLocalServiceMtm,
        KNullDesC8,
        KMsvLocalServiceIndexEntryId,
        aCompletionStatus,
        KErrNone
        );
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::InvokeSyncFunctionL()
// ----------------------------------------------------------------------------
void CSmtpMtmUi::InvokeSyncFunctionL(
    TInt aFunctionId,
    const CMsvEntrySelection& aSelection,
    TDes8& aParameter)
    {
    IMUM_CONTEXT( CSmtpMtmUi::InvokeSyncFunctionL, 0, KImumMtmLog );
    IMUM_IN();

    // Context is dependant on function ID.

    switch (aFunctionId)
        {
        case KMtmUiMessagingInitialisation:
            HandlePredefinedMailboxCreationL();
            break;

        case KMtmUiFunctionValidateService://servicevalidation
            ValidateServiceL( aParameter );
            break;

        case KUidMsvMtmQuerySupportValidateSelection:
            ValidateServiceL( aSelection, aParameter );
            break;

        default:
            CBaseMtmUi::InvokeSyncFunctionL(aFunctionId, aSelection, aParameter);
            break;
        };
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::CreateExecutePreCreationL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CSmtpMtmUi::CreateExecutePreCreationL(
    const TMsvId aSmtpService, TRequestStatus& aStatus ) const
    {
    IMUM_CONTEXT( CSmtpMtmUi::CreateExecutePreCreationL, 0, KImumMtmLog );
    IMUM_IN();


    if ( aSmtpService != KMsvNullIndexEntryId )
        {
        TMsvEntry mailbox = iUtils->GetMailboxEntryL( aSmtpService );

        // Precreate email only for regular mailboxes
        if ( iUtils->IsMailMtm( mailbox.iMtm ) )
            {
            // Create waiter operation and precreation operation
            CMsvOperation* op = CSmtpResetPreCreationOp::NewL(
                iMailboxApi->MsvSession(), aStatus, aSmtpService );
            IMUM_OUT();

            return op;
            }
        }
    IMUM_OUT();
    return NULL;
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::CreateEmailServiceL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CSmtpMtmUi::CreateEmailServiceL(
    const TMsvEntry& aEntry,
    TRequestStatus& aStatus )
    {
    IMUM_CONTEXT( CSmtpMtmUi::CreateEmailServiceL, 0, KImumMtmLog );
    IMUM_IN();



    // Prepare creation of the email
    TInt exitCode = KErrCancel; // JYKA: Changed from EEikCmdExit to allow proper error handling
    TSmtpMtmUiCreateNewServiceParameters saveParameters = ESmtpMtmUiCreateNewServiceExit;
    TMsvId smtpService = CreateNewServiceL( aEntry, saveParameters );
    CMsvOperation* preCreationOperation = NULL;

    // Return value decided the continuation
    switch ( saveParameters )
        {
        default:
        case ESmtpMtmUiCreateNewServiceNotCreated:
            exitCode = KErrCancel;
            break;

        case ESmtpMtmUiCreateNewServiceSave:
            preCreationOperation = CreateExecutePreCreationL( smtpService, aStatus );
            exitCode = KErrNone;
            break;

        case ESmtpMtmUiCreateNewServiceSaveAndExit:
            preCreationOperation = CreateExecutePreCreationL( smtpService, aStatus );

        //lint -fallthrough
        case ESmtpMtmUiCreateNewServiceExit:
            exitCode = EEikCmdExit;
            break;
        }

    IMUM_OUT();

    if ( preCreationOperation )
        {
        return preCreationOperation;
        }

    // Finally return the completed operation
    return CMsvCompletedOperation::NewL(
        Session(), Type(), KNullDesC8,
        KMsvLocalServiceIndexEntryId,
        aStatus, exitCode );
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::CreateL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::CreateL(
    const TMsvEntry& aEntry,
    CMsvEntry& /* aParent */,
    TRequestStatus& aStatus )
    {
    IMUM_CONTEXT( CSmtpMtmUi::CreateL, 0, KImumMtmLog );
    IMUM_IN();


    // Context is irrelevant.
    __ASSERT_ALWAYS( aEntry.iMtm==Type(),
        User::Panic(KImumMtmUiPanic, ESmtpMtmUiWrongMtm ) );

    TMsvId smtpService = KMsvNullIndexEntryId;

    // If service value provided, create the new mailbox
    if ( aEntry.iType.iUid == KUidMsvServiceEntryValue )
        {

        CMsvOperation* op = CreateEmailServiceL( aEntry, aStatus );


        IMUM_OUT();

        return op;
        }
    else if ( aEntry.iType.iUid != KUidMsvMessageEntryValue )
        {
        User::Leave( KErrNotSupported );
        }

    smtpService = aEntry.iServiceId;
    if( smtpService == KMsvNullIndexEntryId ||
        smtpService == KMsvUnknownServiceIndexEntryId )
        {
        // No accounts, offer to create one.
        TSmtpMtmUiCreateNewServiceParameters saveParameters;
        smtpService = HandleNoAccountsL( saveParameters );
        if(  saveParameters == ESmtpMtmUiCreateNewServiceNotCreated )
            {

            IMUM_OUT();

            // User cancelled.
            return CMsvCompletedOperation::NewL(
                Session(), Type(), KNullDesC8,
                KMsvLocalServiceIndexEntryId,
                aStatus, KErrCancel);
            }
        else if(  saveParameters == ESmtpMtmUiCreateNewServiceExit )
            {

            IMUM_OUT();

            // User pressed Exit
            return CMsvCompletedOperation::NewL(
                Session(), Type(),
                KNullDesC8, KMsvLocalServiceIndexEntryId,
                aStatus, EEikCmdExit);
            }
        }
    else
        {

        // Pass to the editor the service Id of the SMTP service to use to send the new message.
        // The message centre will pass a POP3 or IMAP4 service id in aEntry.iServiceId,
        // if so find the SMTP service associated with it.
        TMsvEntry entry;
        TInt findService = Session().GetEntry(aEntry.iServiceId, smtpService, entry);
        if ( findService == KErrNone )
            {
            if ( entry.iMtm != KUidMsgTypeSMTP )
                {
                // mce passed pop3 or imap4 service, so we need to find related smtp service
                findService = Session().GetEntry(entry.iRelatedId, smtpService, entry);
                }
            }

        if ( findService == KErrNotFound )
            {

            // this will dispay 'check email settings' note
            User::Leave( KSmtpUnknownErr );
            }
        else
            {
            User::LeaveIfError( findService );
            }


        //smtp auth fix 23092002
        CheckAndRestoreSmtpUserAndPasswordL( smtpService );
        //fix end
        }


    TEditorParameters editorParams;
    editorParams.iFlags |= EMsgCreateMessageToService;
    editorParams.iId = smtpService;
    TPckgC<TInt> paramPack(ESmtpMtmUiEditing);
    if(!iEditorFilename)
        ResolveEditorFileNameL();
    const TUint preferences = Preferences();
    editorParams.iFlags &= ~(EMsgLaunchEditorEmbedded | EMsgLaunchEditorThenWait);
    if(preferences & EMtmUiFlagEditorPreferEmbedded)
        {
        editorParams.iFlags |= EMsgLaunchEditorEmbedded;
        }
    if(!(preferences & EMtmUiFlagEditorNoWaitForExit))
        {
        editorParams.iFlags |= EMsgLaunchEditorThenWait;
        }

    IMUM_OUT();

    return CSmtpCreateNewOp::NewL(*iMailboxApi, aStatus, editorParams, *iEditorFilename, paramPack);
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::OpenL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::OpenL(TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CSmtpMtmUi::OpenL, 0, KImumMtmLog );
    IMUM_IN();

    // Context is entry to open.
    const TMsvEntry& context=iBaseMtm.Entry().Entry();
    __ASSERT_ALWAYS(context.iMtm==Type(), User::Panic(KImumMtmUiPanic, ESmtpMtmUiWrongMtm));

    const TUid type(context.iType);
    __ASSERT_ALWAYS(type!=KUidMsvFolderEntry, User::Panic(KImumMtmUiPanic, ESmtpMtmUiFoldersNotSupported));

    //check that message is not corrupted and open
    CMsvEntry& tmpEntry = iBaseMtm.Entry();
    TBool hasStore = tmpEntry.HasStoreL();

    if( hasStore )
        {
        IMUM_OUT();
        return EditL(aStatus);
        }
    else//if corrupt
        {
        User::Leave( KErrCorrupt );
        IMUM_OUT();
        return NULL;//execution never gets here...but complier will
        }
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::OpenL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::OpenL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection)
    {
    IMUM_CONTEXT( CSmtpMtmUi::OpenL, 0, KImumMtmLog );
    IMUM_IN();

    // Context is irrelevant.
    __ASSERT_ALWAYS(aSelection.Count(), User::Panic(KImumMtmUiPanic, ESmtpMtmUiEmptySelection));
    iBaseMtm.Entry().SetEntryL(aSelection[0]);
    IMUM_OUT();
    return OpenL(aStatus);
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::CloseL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::CloseL(TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CSmtpMtmUi::CloseL, 0, KImumMtmLog );
    IMUM_IN();

    // Context is entry to close.
    __ASSERT_DEBUG(iBaseMtm.Entry().Entry().iMtm==Type(), User::Panic(KImumMtmUiPanic, ESmtpMtmUiWrongMtm));
    __ASSERT_DEBUG(iBaseMtm.Entry().Entry().iType!=KUidMsvFolderEntry, User::Panic(KImumMtmUiPanic, ESmtpMtmUiFoldersNotSupported));
    // No explicit close of SMTP entries.
    CMsvCompletedOperation* op=CMsvCompletedOperation::NewL( Session(), Type(), KNullDesC8, KMsvLocalServiceIndexEntryId, aStatus);
    IMUM_OUT();
    return op;
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::CloseL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::CloseL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection)
    {
    IMUM_CONTEXT( CSmtpMtmUi::CloseL, 0, KImumMtmLog );
    IMUM_IN();

    // Context is irrelevant.
    __ASSERT_ALWAYS(aSelection.Count(), User::Panic(KImumMtmUiPanic, ESmtpMtmUiEmptySelection));
    iBaseMtm.Entry().SetEntryL(aSelection.At(0));
    IMUM_OUT();
    return CloseL(aStatus);
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::EditL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::EditL(TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CSmtpMtmUi::EditL, 0, KImumMtmLog );
    IMUM_IN();

    // Context is entry to edit.
    __ASSERT_ALWAYS(iBaseMtm.Entry().Entry().iMtm==Type(), User::Panic(KImumMtmUiPanic, ESmtpMtmUiWrongMtm));
    __ASSERT_DEBUG(iBaseMtm.Entry().Entry().iType!=KUidMsvFolderEntry, User::Panic(KImumMtmUiPanic, ESmtpMtmUiFoldersNotSupported));

    switch (iBaseMtm.Entry().Entry().iType.iUid)
        {
    case KUidMsvMessageEntryValue:
        IMUM_OUT();
        return LaunchEditorApplicationL(aStatus, EFalse, EFalse, iBaseMtm.Entry().Session() );
        //break;    // Unreachable
    case KUidMsvServiceEntryValue:
        IMUM_OUT();
        return LaunchSettingsDialogL(aStatus);
        //break;    // Unreachable
    default:
        User::Leave(KErrNotSupported);
        break;
        };
    IMUM_OUT();
    return NULL;    // Unreachable in practice.
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::EditL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::EditL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection)
    {
    IMUM_CONTEXT( CSmtpMtmUi::EditL, 0, KImumMtmLog );
    IMUM_IN();

    // Context is irrelevant.
    __ASSERT_ALWAYS(aSelection.Count(), User::Panic(KImumMtmUiPanic, ESmtpMtmUiEmptySelection));
    iBaseMtm.Entry().SetEntryL(aSelection[0]);
    IMUM_OUT();
    return EditL(aStatus);
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::ViewL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::ViewL(TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CSmtpMtmUi::ViewL, 0, KImumMtmLog );
    IMUM_IN();

    // Context is message to view.
    __ASSERT_ALWAYS(iBaseMtm.Entry().Entry().iMtm==Type(), User::Panic(KImumMtmUiPanic, ESmtpMtmUiWrongMtm));
    __ASSERT_ALWAYS(iBaseMtm.Entry().Entry().iType!=KUidMsvFolderEntry, User::Panic(KImumMtmUiPanic, ESmtpMtmUiFoldersNotSupported));

    IMUM_OUT();
    return LaunchEditorApplicationL(aStatus, ETrue, EFalse, Session());
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::ViewL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::ViewL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection)
    {
    IMUM_CONTEXT( CSmtpMtmUi::ViewL, 0, KImumMtmLog );
    IMUM_IN();

    // Context is irrelevant.
    __ASSERT_ALWAYS(aSelection.Count(), User::Panic(KImumMtmUiPanic, ESmtpMtmUiEmptySelection));
    iBaseMtm.Entry().SetEntryL(aSelection[0]);
    IMUM_OUT();
    return ViewL(aStatus);
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::CancelL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::CancelL(TRequestStatus& aStatus, const CMsvEntrySelection& aSelection)
    {
    IMUM_CONTEXT( CSmtpMtmUi::CancelL, 0, KImumMtmLog );
    IMUM_IN();

    // Context is irrelevant.
    __ASSERT_ALWAYS(aSelection.Count(), User::Panic(KImumMtmUiPanic, ESmtpMtmUiEmptySelection));
    IMUM_OUT();
    return CSmtpCancelSendingOp::NewL( *iMailboxApi, aStatus, aSelection.CopyL());
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::CopyFromL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::CopyFromL(const CMsvEntrySelection& /*aSelection*/, TMsvId /*aTargetId*/, TRequestStatus& /*aStatus*/)
    {
    IMUM_CONTEXT( CSmtpMtmUi::CopyFromL, 0, KImumMtmLog );
    IMUM_IN();

    User::Panic(KImumMtmUiPanic, ESmtpMtmUiCannotCopyOrMoveFromSmtpServiceOrFolder);
    IMUM_OUT();
    return NULL;
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::MoveFromL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::MoveFromL(const CMsvEntrySelection& /*aSelection*/, TMsvId /*aTargetId*/, TRequestStatus& /*aStatus*/)
    {
    IMUM_CONTEXT( CSmtpMtmUi::MoveFromL, 0, KImumMtmLog );
    IMUM_IN();

    User::Panic(KImumMtmUiPanic, ESmtpMtmUiCannotCopyOrMoveFromSmtpServiceOrFolder);
    IMUM_OUT();
    return NULL;
    }

// ----------------------------------------------------------------------------
// MoveToL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::MoveToL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CSmtpMtmUi::MoveToL, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();

    return DoCopyMoveToL(aSelection, aStatus, ETrue);
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::CopyToL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::CopyToL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus)
    {
    IMUM_CONTEXT( CSmtpMtmUi::CopyToL, 0, KImumMtmLog );
    IMUM_IN();
    IMUM_OUT();

    return DoCopyMoveToL(aSelection, aStatus, EFalse);
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::DoCopyMoveToL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::DoCopyMoveToL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus, TBool aMoving)
    {
    IMUM_CONTEXT( CSmtpMtmUi::DoCopyMoveToL, 0, KImumMtmLog );
    IMUM_IN();

    // Context is service to send to.
    const TMsvEntry& context=BaseMtm().Entry().Entry();
    __ASSERT_ALWAYS(context.iMtm==Type(), User::Panic(KImumMtmUiPanic, ESmtpMtmUiWrongMtm));
    __ASSERT_ALWAYS(context.iType==KUidMsvServiceEntry, User::Panic(KImumMtmUiPanic, ESmtpMtmUiNotAService));

    //smtp auth fix 23092002
    CheckAndRestoreSmtpUserAndPasswordL( BaseMtm().Entry().EntryId() );
    //fix end

    TInt cc = aSelection.Count();
    if(!cc)
        {
        // Nothing to do.
        return CMsvCompletedOperation::NewL(Session(), Type(), KNullDesC8, KMsvLocalServiceIndexEntryId, aStatus);
        }
    // Iterate entries in selection and check them.
    TBool changed = EFalse;
    TMsvEntry entry;
    TMsvId id = KMsvNullIndexEntryId;
    User::LeaveIfError(Session().GetEntry(aSelection[0], id, entry));
    CMsvEntry* parent = Session().GetEntryL(entry.Parent());
    CleanupStack::PushL(parent);
    CMsvEntry* centry = Session().GetEntryL(entry.Id());
    CleanupStack::PushL(centry);
    while(cc--)
        {
        id = aSelection[cc];
        entry = parent->ChildDataL(id);
        if(entry.iServiceId != context.Id())
            {
            // If service ID does not match context, set to context ID.
            entry.iServiceId = context.Id();
            changed = ETrue;
            }
        if(entry.SendingState() == KMsvSendStateSuspended)
            {
            // If the entry is suspended, set it to scheduled, or it will not be sent.
            entry.SetSendingState(KMsvSendStateWaiting);
            changed = ETrue;
            }
        if(changed)
            {
            if(centry->EntryId() != entry.Id())
                {
                centry->SetEntryL(entry.Id());
                }
            centry->ChangeL(entry);
            changed = EFalse;
            }
        }
    CleanupStack::PopAndDestroy(2, parent); // CSI: 47 # centry, parent.

    // Create send operation.
    CMsvProgressReporterOperation* reporter =
        CMsvProgressReporterOperation::NewL(
            EFalse,
            ETrue,
            Session(),
            aStatus,
            EMbmAvkonQgn_note_mail );
    CleanupStack::PushL(reporter);
    CMsvOperation* op = CSmtpCopyMoveOp::NewL(*iMailboxApi, reporter->RequestStatus(),
        *reporter, context.Id(), aSelection, aMoving);
    reporter->SetOperationL(op); // Takes immediate ownership
    CleanupStack::Pop();    // reporter
    IMUM_OUT();
    return reporter;
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::DisplayProgressSummary()
// ----------------------------------------------------------------------------
TInt CSmtpMtmUi::DisplayProgressSummary(const TDesC8& aProgress) const
    {
    IMUM_CONTEXT( CSmtpMtmUi::DisplayProgressSummary, 0, KImumMtmLog );
    IMUM_IN();

    // Context is irrelevent.
    if( (!aProgress.Length()) || (aProgress.Size() == sizeof(TMsvLocalOperationProgress)) )
        {
        IMUM_OUT();
        return KErrCancel;
        }

    if( aProgress.Size() == sizeof(TMsvId) )
        {
        // reason for this is that for CImEmailOperation did not fill iMtm correctly (it was always 0)
        // and that caused that in mce app's CMceUi::DoOperationCompletedL leaves
        // when trying to create mtm for displaying progress summary and execution
        // never came here. Now CImEmailOperation sets its iMtm correctly and
        // now CMceUi::DoOperationCompletedL works correcly and execution comes here.
        // problem now is that how to make difference between imum's internal progress
        // and CImEmailOperation progress since CImEmailOperation's progress is just
        // TMsvId of the created mail message. This needs to be solved.
        IMUM_OUT();
        return KErrNone;
        }


    // First element of progress should be identifier of the operation type
    TPckgC<TInt> progressType(0);
    progressType.Set(aProgress.Left(sizeof(TInt)));
    if(progressType()==ESmtpMtmUiEditing)
        {
        IMUM_OUT();
        return KErrNone;
        }

    // SMTP MTM operations
    __ASSERT_ALWAYS(aProgress.Size() == sizeof(TImSmtpProgress), User::Panic(KImumMtmUiPanic, ESmtpMtmUiBadProgress));
    TPckgBuf<TImSmtpProgress> paramPack;
    paramPack.Copy(aProgress);
    const TImSmtpProgress& progress = paramPack();
    TInt err = progress.Error();
    if( err != KErrNone && err != KErrCancel && err != KErrGeneral &&
        err != KErrEtelBusyDetected && err != KErrAbort )
        {
        if( err == KErrGprsServicesNotAllowed )
            {
            err = KImskErrorActiveSettingIsDifferent;
            }
        TRAP_IGNORE( iErrorResolver->ShowGlobalErrorNoteL( err ) );
        }
    IMUM_OUT();
    return err;
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::GetProgress()
// ----------------------------------------------------------------------------
TInt CSmtpMtmUi::GetProgress(
    const TDesC8& aProgress,
    TBuf<EProgressStringMaxLen>& aReturnString,
    TInt& aTotalEntryCount,
    TInt& aEntriesDone,
    TInt& aCurrentEntrySize,
    TInt& aCurrentBytesTrans) const
    {
    IMUM_CONTEXT( CSmtpMtmUi::GetProgress, 0, KImumMtmLog );
    IMUM_IN();

    aEntriesDone=0;
    aTotalEntryCount=0;
    aCurrentBytesTrans=0;
    aCurrentEntrySize=0;
    aReturnString.Zero();

    if( (!aProgress.Length()) ||
        (aProgress.Size() == sizeof(TMsvLocalOperationProgress)) )
        {
        IMUM_OUT();
        return KErrNone;
        }

    // First element of progress should be identifier of the operation type
    TPckgC<TInt> type( 0 );
    type.Set( aProgress.Left( sizeof( TInt ) ) );
    switch ( type() )
        {
        case ESmtpMtmUiEditing:
            IMUM_OUT();
            // Editor operation
            return KErrNone;

        case EUiProgTypeConnecting:
            IMUM_OUT();
            // Connecting progress
            return GetConnectionProgress(aProgress, aReturnString);

        default:
            break;
        }

    // SMTP MTM operations
    __ASSERT_ALWAYS(aProgress.Size() == sizeof(TImSmtpProgress), User::Panic(KImumMtmUiPanic, ESmtpMtmUiBadProgress));
    TPckgBuf<TImSmtpProgress> paramPack;
    paramPack.Copy(aProgress);
    const TImSmtpProgress& progress = paramPack();

    aTotalEntryCount = progress.SendTotal();
    aEntriesDone = Min(aTotalEntryCount, progress.MsgNo()+1);
    aCurrentEntrySize = progress.iSendFileProgress.iBytesToSend;
    aCurrentBytesTrans = progress.iSendFileProgress.iBytesSent;

    TInt err = progress.Error();
    switch(progress.Status())
        {
        case EMsgOutboxProgressSending:
            {
            TInt resId = (aTotalEntryCount == 1) ? (R_SMTP_SENDING_PROGRESS) : (R_SMTP_SENDING_PROGRESS_MULTIPLE);
            StringLoader::Load( aReturnString, resId, iCoeEnv);
            if ( aTotalEntryCount>1 )
                {
                TBuf<EProgressStringMaxLen> tempBuffer;
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
            err = progress.Error();
            }
            break;
        case EMsgOutboxProgressWaiting:
        case EMsgOutboxProgressConnecting:
        default:
            // Should not get any other statuses, get UI connection progress instead.
            aReturnString.Zero();
            break;
        }
    IMUM_OUT();
    return err;
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::GetConnectionProgress()
// ----------------------------------------------------------------------------
TInt CSmtpMtmUi::GetConnectionProgress(const TDesC8& aProgress, TBuf<EProgressStringMaxLen>& aReturnString) const
    {
    IMUM_CONTEXT( CSmtpMtmUi::GetConnectionProgress, 0, KImumMtmLog );
    IMUM_IN();

    __ASSERT_ALWAYS(aProgress.Size() == sizeof(TMsvEmailConnectionProgress), User::Panic(KImumMtmUiPanic, ESmtpMtmUiBadProgress));
    TPckgBuf<TMsvEmailConnectionProgress> paramPack;
    paramPack.Copy(aProgress);
    const TMsvEmailConnectionProgress& connectProgress = paramPack();

    MsvEmailMtmUiUtils::CreateConnectingToText( aReturnString,
        Session(), connectProgress.iService );
    IMUM_OUT();

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::DoExitL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::DoExitL(
    TRequestStatus& aStatus,
    const TBool aIssueQuit )
    {
    IMUM_CONTEXT( CSmtpMtmUi::DoExitL, 0, KImumMtmLog );
    IMUM_IN();

    TInt err = aIssueQuit ? EEikCmdExit : EEikCmdCanceled;
    IMUM_OUT();

    // Nothing to do, so return a completed operation.
    return CMsvCompletedOperation::NewL(
        Session(), Type(), KNullDesC8,
        KMsvLocalServiceIndexEntryId, aStatus, err );
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::LaunchEditorApplicationL()
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::LaunchEditorApplicationL(
    TRequestStatus& aStatus,
    TBool aReadOnly,
    TBool aNewMsg,
    CMsvSession& aSession,
    TMsvId aNewMsgServiceId)
    {
    IMUM_CONTEXT( CSmtpMtmUi::LaunchEditorApplicationL, 0, KImumMtmLog );
    IMUM_IN();

    // Context is irrelevant.

    // Create the parameters necessary for the editor
    TEditorParameters editorParams;
    TMsvEntry entry = BaseMtm().Entry().Entry();
    if ( entry.Parent() != KMsvDraftEntryId )
        {
        aReadOnly = ETrue;
        }
    if (aReadOnly)
        {
        editorParams.iFlags|=EMsgReadOnly;
        }

    if ( !aReadOnly )
        {
        // we are opening the editor, check that there is at least one mail service defined
        TMsvId serviceId = entry.iServiceId;
        TSmtpMtmUiCreateNewServiceParameters aSaveParameters;

        if ( !CheckMailboxesBeforeEditL( serviceId, aSaveParameters ) )
            {
            if ( aSaveParameters == ESmtpMtmUiCreateNewServiceExit )
                {
                return CMsvCompletedOperation::NewL(Session(), Type(), KNullDesC8, KMsvLocalServiceIndexEntryId, aStatus, EEikCmdExit );
                }
            else
                {
                return CMsvCompletedOperation::NewL(Session(), Type(), KNullDesC8, KMsvLocalServiceIndexEntryId, aStatus);
                }
            }
        if ( serviceId != entry.iServiceId )
            {
            // service has been changed, change it to the entry too...
            entry.iServiceId = serviceId;
            BaseMtm().Entry().ChangeL( entry );
            }
        }

    if (aNewMsg)
        {
        // Creating new message. The EMsgCreateMessageToService flag specifies that
        // editorParams.iId is the SMTP service to use for the message.
        __ASSERT_DEBUG(aNewMsgServiceId != KMsvNullIndexEntryIdValue, User::Panic(KImumMtmUiPanic, ESmtpMtmUiNullNewMsgService));
        editorParams.iFlags |= EMsgCreateMessageToService;
        editorParams.iId = aNewMsgServiceId;
        }
    else
        {
        editorParams.iId=BaseMtm().Entry().EntryId();
        }

    //
    // --- This is a MTM UI operation only, so make an appropriate progress package ---
    // --- Editor progress is blank, except for the opreation identifier ---
    TPckgC<TInt> paramPack(ESmtpMtmUiEditing);
    HBufC* appName = NULL;
    if(aReadOnly)
        {
        if(!iViewerFilename)
            ResolveViewerFileNameL();
        appName = iViewerFilename;
        }
    else
        {
        if(!iEditorFilename)
            ResolveEditorFileNameL();
        appName = iEditorFilename;
        }

    const TUint preferences = Preferences();
    editorParams.iFlags &= ~(EMsgLaunchEditorEmbedded | EMsgLaunchEditorThenWait);
    if(preferences & EMtmUiFlagEditorPreferEmbedded)
        {
        editorParams.iFlags |= EMsgLaunchEditorEmbedded;
        }
    if(!(preferences & EMtmUiFlagEditorNoWaitForExit))
        {
        editorParams.iFlags |= EMsgLaunchEditorThenWait;
        }
    IMUM_OUT();

    return MsgEditorLauncher::LaunchEditorApplicationL(
        aSession, Type(), aStatus, editorParams, *appName, paramPack );
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::HandleNoAccountsL()
// ----------------------------------------------------------------------------
TMsvId CSmtpMtmUi::HandleNoAccountsL(TSmtpMtmUiCreateNewServiceParameters& aSaveParameters )
    {
    IMUM_CONTEXT( CSmtpMtmUi::HandleNoAccountsL, 0, KImumMtmLog );
    IMUM_IN();

    // Context is irrelevant.
    if ( !CIMSSettingsNoteUi::ShowQueryL(
        R_SMTP_CREATE_NEED_MAILBOX_TITLE, R_EMAIL_CONFIRMATION_QUERY ) )
        {
        // User cancelled.
        aSaveParameters = ESmtpMtmUiCreateNewServiceNotCreated;
        return KMsvNullIndexEntryId;
        }

    //Show empty navipane
    CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();
    CAknNavigationControlContainer* naviPane = static_cast<CAknNavigationControlContainer*>(
        sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    CAknNavigationDecorator* naviDecorator = naviPane->CreateNavigationLabelL();
    CleanupStack::PushL(naviDecorator);
    naviPane->PushL( *naviDecorator );

    TMsvEntry nentry;
    nentry.iMtm = Type();
    nentry.iType = KUidMsvServiceEntry;
    nentry.iDate.HomeTime();
    nentry.iServiceId = KMsvNullIndexEntryId;

    // Note that we don't pre-create a message for this account here since we will
    // be launching an editor immediately afterwards.
    IMUM_OUT();

    // Newly created service will be the default.
    TMsvId serviceId = CreateNewServiceL(nentry, aSaveParameters);
    //Remove empty navipane
    naviPane->Pop( naviDecorator );
    CleanupStack::PopAndDestroy( naviDecorator );
    return serviceId;
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::PrepareCommandL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CSmtpMtmUi::PrepareCommandL(
    TEditorParameters& aParameters,
    TMsvId aDestination,
    TMsvPartList aPartlist,
    TRequestStatus& aCompletionStatus )
    {
    IMUM_CONTEXT( CSmtpMtmUi::PrepareCommandL, 0, KImumMtmLog );
    IMUM_IN();

    const TMsvEntry& entry = BaseMtm().Entry().Entry();

    if ( entry.iType.iUid != KUidMsvMessageEntryValue )
        {
        User::Leave( KErrNotSupported );
        }

    // Set editor name
    HBufC* appName = NULL;
    if(!iEditorFilename)
        {
        ResolveEditorFileNameL();
        }
    appName = iEditorFilename;

    // Set params
    aParameters.iId=entry.Id();
    aParameters.iPartList = aPartlist;
    aParameters.iDestinationFolderId = aDestination;

    // Mark as read
    ChangeUnreadFlagL( entry.Id(), EFalse );

    // Handle preferences
    const TUint preferences = Preferences();
    aParameters.iFlags &=
        ~( EMsgLaunchEditorEmbedded | EMsgLaunchEditorThenWait );

    if ( preferences & EMtmUiFlagEditorPreferEmbedded )
        {
        aParameters.iFlags |= EMsgLaunchEditorEmbedded;
        }
    if ( !( preferences & EMtmUiFlagEditorNoWaitForExit ) )
        {
        aParameters.iFlags |= EMsgLaunchEditorThenWait;
        }

    // Launch editor
    TPckgC<TInt> paramPack( ESmtpMtmUiEditing );
    IMUM_OUT();
    return MsgEditorLauncher::LaunchEditorApplicationL(
        Session(), Type(), aCompletionStatus, aParameters,
        *appName, paramPack);
    }


// ----------------------------------------------------------------------------
// CSmtpMtmUi::ReplyL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CSmtpMtmUi::ReplyL(
    TMsvId aDestination,
    TMsvPartList aPartlist,
    TRequestStatus& aCompletionStatus )
    {
    IMUM_CONTEXT( CSmtpMtmUi::ReplyL, 0, KImumMtmLog );
    IMUM_IN();

    TEditorParameters editorParams;
    editorParams.iFlags |= EMsgReplyToMessageAll;
    IMUM_OUT();

    return PrepareCommandL(
        editorParams, aDestination, aPartlist, aCompletionStatus );
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::ForwardL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CSmtpMtmUi::ForwardL(
    TMsvId aDestination,
    TMsvPartList aPartlist,
    TRequestStatus& aCompletionStatus)
    {
    IMUM_CONTEXT( CSmtpMtmUi::ForwardL, 0, KImumMtmLog );
    IMUM_IN();

    TEditorParameters editorParams;
    editorParams.iFlags |= EMsgForwardMessage;
    IMUM_OUT();

    return PrepareCommandL(
        editorParams, aDestination, aPartlist, aCompletionStatus );
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::GetMailboxNameL()
// ----------------------------------------------------------------------------
HBufC* CSmtpMtmUi::GetMailboxNameL(TMsvId aServiceId) const
    {
    IMUM_CONTEXT( CSmtpMtmUi::GetMailboxNameL, 0, KImumMtmLog );
    IMUM_IN();

    TMsvEntry serviceEntry =
        iMailboxApi->MailboxUtilitiesL().GetMailboxEntryL( aServiceId );
    IMUM_OUT();
    return serviceEntry.iDetails.AllocL();
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::CheckMailboxesBeforeEditL()
// ----------------------------------------------------------------------------
TBool CSmtpMtmUi::CheckMailboxesBeforeEditL( TMsvId& aServiceId, TSmtpMtmUiCreateNewServiceParameters& aSaveParameters )
    {
    IMUM_CONTEXT( CSmtpMtmUi::CheckMailboxesBeforeEditL, 0, KImumMtmLog );
    IMUM_IN();

    // checks if aServiceId exists and if not checks that there is default service and
    // if no services, then asks to create one
    TMsvEntry serviceEntry;
    TMsvId serviceId;
    aSaveParameters = ESmtpMtmUiCreateNewServiceNotCreated;
    if ( Session().GetEntry( aServiceId, serviceId, serviceEntry ) == KErrNone )
        {
        IMUM_OUT();
        // given service exists
        return ETrue;
        }

    // given service does not exists, check if there is default one:
    serviceId = MsvUiServiceUtilitiesInternal::DefaultServiceForMTML(
        Session(),
        Type(),
        ETrue );

    if ( serviceId != KMsvUnknownServiceIndexEntryId )
        {
        // default exists!
        aServiceId = serviceId;
        IMUM_OUT();
        return ETrue;
        }

    // no mailboxes, ask to create one
    serviceId = HandleNoAccountsL( aSaveParameters );

    if ( serviceId != KMsvUnknownServiceIndexEntryId &&
         serviceId != KMsvNullIndexEntryId &&
         aSaveParameters == ESmtpMtmUiCreateNewServiceSave )
        {
        aServiceId = serviceId;
        IMUM_OUT();
        return ETrue;
        }
    IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::ChangeUnreadFlagL()
// ----------------------------------------------------------------------------
//
void CSmtpMtmUi::ChangeUnreadFlagL(
    const TMsvId aEmailId,
    const TBool aNewState )
    {
    IMUM_CONTEXT( CSmtpMtmUi::ChangeUnreadFlagL, 0, KImumMtmLog );
    IMUM_IN();

    // Prepare the entry
    CMsvEntry* entry = ( CMsvEntry::NewL(
        *iMsvSession, aEmailId, TMsvSelectionOrdering() ) );
    CleanupStack::PushL( entry );
    TMsvEntry email = entry->Entry();

    // Set the unread flag
    email.SetUnread( aNewState );
    entry->ChangeL( email );

    CleanupStack::PopAndDestroy( entry );
    entry = NULL;
    IMUM_OUT();
    }


// ----------------------------------------------------------------------------
// CSmtpMtmUi::LaunchSettingsDialogL
// ----------------------------------------------------------------------------
CMsvOperation* CSmtpMtmUi::LaunchSettingsDialogL(
    TRequestStatus& aStatus )
    {
    IMUM_CONTEXT( CSmtpMtmUi::LaunchSettingsDialogL, 0, KImumMtmLog );
    IMUM_IN();

    // Get the TMsvEntry for the service.
    TMsvEntry msvEntry( BaseMtm().Entry().Entry() );
    TMsvEntry rcvEntry = iUtils->GetMailboxEntryL( msvEntry.iRelatedId );

    // Do this earlies phase possible, to guarrentee there won't be any
    // useless mailboxes around
    iMailboxApi->HealthServicesL().CleanupUnhealthyMailboxes();

    TBool quit( EFalse );

    // Mailbox should not be connected when settings are edited
    if ( !rcvEntry.Connected() ||
        MsvEmailMtmUiUtils::DisconnectMailboxL( rcvEntry, Session() ) )
        {
        // Create and load email account
        CImumInSettingsData* accountSettings =
            iMailboxApi->MailboxServicesL().LoadMailboxSettingsL(
                msvEntry.Id() );
        CleanupStack::PushL( accountSettings );

        // Create message for receiving protocol and send it to Always Online
        TPckgBuf<TMsvId> param = msvEntry.iRelatedId;
        TRAP_IGNORE( iAOClient.SendSinglePacketL(
            EServerAPIEmailTurnOff, param ) );

        // Bring the settings to user
        TImumUiExitCodes exitcode = EImumUiNoChanges;
        CIMSSettingsUi::EditAccountL( exitcode, *accountSettings, *iMsvSession );

        // Create completed operation to finish the settings
        quit = SettingsDialogExitL( *accountSettings, exitcode );

        // Get the emn setting
        TInt emn = TImumDaSettings::EValueNotificationsOff;
        accountSettings->GetAttr(
            TImumDaSettings::EKeyAutoNotifications, emn );

        // Get the always online setting
        TInt alwaysOnline = TImumDaSettings::EValueAutoOff;
        accountSettings->GetAttr(
            TImumDaSettings::EKeyAutoRetrieval, alwaysOnline );

        // Send turn on message to plugin, if either AO or EMN is turned on.
        TAlwaysOnlineServerAPICommands command =
            ( alwaysOnline != EMailAoOff || emn != EMailEmnOff ) ?
            EServerAPIEmailTurnOn : EServerAPIEmailAgentRemove;

        CleanupStack::PopAndDestroy( accountSettings );
        accountSettings = NULL;

        // Turn on the Always Online to current mailbox
        iAOClient.SendSinglePacketL( command, param );
        }
    if ( quit )
        {
        // Delete precreated emails because they might contain old signature.
        // Since we're in a hurry to exit, we don't recreate them.
        iPreCreate->DeleteAllPreCreatedEmailsL( msvEntry.Id() );
        
        IMUM_OUT();
        return DoExitL( aStatus, quit );
        }
    else
        {
        IMUM_OUT();
        //Let's start the empty email creation
        return CSmtpResetPreCreationOp::NewL(
            Session(), aStatus, msvEntry.Id() );
        }
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::SettingsDialogExitL()
// ----------------------------------------------------------------------------
//
TBool CSmtpMtmUi::SettingsDialogExitL(
    CImumInSettingsData& aSettings,
    const TImumUiExitCodes& aExitCode )
    {
    IMUM_CONTEXT( CSmtpMtmUi::SettingsDialogExitL, 0, KImumMtmLog );
    IMUM_IN();

    TBool quit = EFalse;
    // Check the exit code
    switch( aExitCode )
        {
        // Save the settings
        case EImumUiSaveAndClose:
            SettingsDialogExitSaveAndExitL( aSettings );
            break;

        // Save the settings and quit
        case EImumUiSaveAndExit:
            SettingsDialogExitSaveAndExitL( aSettings );

        //lint -fallthrough
        case EImumUiExit:
            quit = ETrue;
            break;

        default:
            // Nothing to do
            break;
        }
    IMUM_OUT();

    return quit;
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::SettingsDialogExitSaveAndExitL()
// ----------------------------------------------------------------------------
//
void CSmtpMtmUi::SettingsDialogExitSaveAndExitL(
    CImumInSettingsData& aSettings )
    {
    IMUM_CONTEXT( CSmtpMtmUi::SettingsDialogExitSaveAndExitL, 0, KImumMtmLog );
    IMUM_IN();

    TInt ao = ImumMboxSettingsUtils::QuickGetL<TInt>(
        aSettings, TImumDaSettings::EKeyAutoRetrieval );

    //restore original alwaysonline setting
    if ( ImumMboxSettingsUtils::IsImap4( aSettings ) &&
         ao == TImumDaSettings::EValueAutoOff )
        {
        //switching off, set default syncrate
        aSettings.GetInSetL( 0 ).Reset( TImumInSettings::EKeySyncRate );
        }

    // Save the settings
    iMailboxApi->MailboxServicesL().SaveMailboxSettingsL( aSettings );
    TMsvId id = ImumMboxSettingsUtils::QuickGetL<TMsvId>(
        aSettings, TImumDaSettings::EKeyMailboxId, 0 );
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::CreateNewServiceL
// ----------------------------------------------------------------------------
TMsvId CSmtpMtmUi::CreateNewServiceL(
    const TMsvEntry& aEntry,
    TSmtpMtmUiCreateNewServiceParameters& aSaveParameters )
    {
    IMUM_CONTEXT( CSmtpMtmUi::CreateNewServiceL, 0, KImumMtmLog );
    IMUM_IN();

    // This function is called, when new account is to be created. The account
    // is created, after user has succesfully finished the mailbox settings
    // wizard.
    // Do this earlies phase possible, to guarrentee there won't be any
    // useless mailboxes around
    iMailboxApi->HealthServicesL().CleanupUnhealthyMailboxes();

    // Make sure empty index has been given
    if ( aEntry.iServiceId != KMsvNullIndexEntryId &&
         aEntry.iServiceId != KMsvUnknownServiceIndexEntryId )
        {
        User::Leave( KErrUnknown );
        }

    // Run the mailbox settings
    TMsvId id = 0;
    TImumUiExitCodes exitcode = EImumUiNoChanges;

    //Check if mail settings are locked
    if( iMailboxApi->IsEmailFeatureSupportedL(
        CImumInternalApi::EMailLockedSettings, R_QTN_SELEC_PROTECTED_SETTING ) )
        {
        aSaveParameters = ESmtpMtmUiCreateNewServiceNotCreated;
        }
    else
        {
        // Settings wizard fills in the settings object
        CImumInSettingsData* accountSettings =
            iMailboxApi->MailboxServicesL().CreateSettingsDataL( TUid::Uid( 0 ) );
        CleanupStack::PushL( accountSettings );

        // lets clear the screen so that MCE is not seen on the background after wizard is
        // finished and new view is activated
        CAknLocalScreenClearer* localScreenClearer = 
        	CAknLocalScreenClearer::NewLC( EFalse );
        
        // Add ScreenClearer to control stack
        CEikonEnv::Static()->EikAppUi()->AddToStackL( 
        	localScreenClearer, ECoeStackPriorityDefault, 
        	ECoeStackFlagRefusesAllKeys|ECoeStackFlagRefusesFocus );
        
        // If application leaves make sure the screen clearer is removed
        // from control stack.
        CleanupStack::PushL( TCleanupItem( &ScreenClearerCleanupOperation, 
        	localScreenClearer ) );
        
        
         // Run wizard, run!
        id = CIMSSettingsUi::NewAccountL(
            exitcode, *accountSettings, *iMsvSession );
        aSaveParameters = SaveParams( exitcode );

        TMsvEntry tentry;

        if( exitcode != EImumUiExtWizardCreated )
        	{
            if ( aSaveParameters == ESmtpMtmUiCreateNewServiceSave ||
                aSaveParameters == ESmtpMtmUiCreateNewServiceSaveAndExit )
                {
                id = iMailboxApi->MailboxServicesL().CreateMailboxL(
                     *accountSettings );
                }
        	}

        if ( exitcode == EImumUiExtWizardCreated ||
            aSaveParameters == ESmtpMtmUiCreateNewServiceSave ||
            aSaveParameters == ESmtpMtmUiCreateNewServiceSaveAndExit )
            {
            // check that there is actually a mailbox generated
            if( iMailboxApi->MailboxUtilitiesL().IsMailbox( id ) )
            	{
                tentry = iMailboxApi->MailboxUtilitiesL().GetMailboxEntryL( id );
            	}
            }

        // If view is activated when EImumUiExit, MCE will be reopened which
        // is not desirable.
        if ( exitcode != EImumUiExit )
        	{
	        // check which view should be activated after wizard is run
	        if( tentry.iMtm.iUid == KErrNone )
	        	{
	        	/*
	        	 * No actual mailbox was defined, so activate MCE
	        	 * main view.
	        	 *
	        	 * This could be the case for example with webmail
	        	 * accounts that don't actually create an actual
	        	 * mailbox entry.
	        	 */
	            ActivateViewL( EImumUiNoChanges, id );
	        	}
			else
				{
				// POP/IMAP mailbox entry created, open correct view
				ActivateViewL( exitcode, tentry.iRelatedId );
				}
        	}
        
        // Pop and destroy TCleanupItem, localScreenClearer and accountSettings
        // TCleanupItem calls ScreenClearerCleanupOperation that removes 
        // ScreenClearer from control stack.
        CleanupStack::PopAndDestroy( 3, accountSettings );
        localScreenClearer = NULL;
        accountSettings = NULL;
        }
    IMUM_OUT();

    // Return the mailbox id
    return id;
    }

// ---------------------------------------------------------------------------.
// CSmtpMtmUi::SaveParams()
// ---------------------------------------------------------------------------
//
CSmtpMtmUi::TSmtpMtmUiCreateNewServiceParameters CSmtpMtmUi::SaveParams(
    const TImumUiExitCodes& aUiExitCode )
    {
    IMUM_CONTEXT( CSmtpMtmUi::TSmtpMtmUiCreateNewServiceParameters, 0, KImumMtmLog );
    IMUM_IN();

    TSmtpMtmUiCreateNewServiceParameters params =
        ESmtpMtmUiCreateNewServiceNotCreated;

    // Map account settings exit values to save parameters
    switch ( aUiExitCode )
        {
        case EImumUiNoChanges: // Nothing has changed, no need for save
        case EImumUiClose: // After back key has been pressed, exit settings
        default:
            params = ESmtpMtmUiCreateNewServiceNotCreated;
            break;

        // After back key has been pressed, save and exit settings
        case EImumUiSaveAndClose:
        // External wizard created a new mailbox. ESmtpMtmUiCreateNewServiceSave
        // must be set to param because we have to inform that the mailbox is
        // created. So in this case we must not save the mailbox because
        // External Wizard is already saved it.
        case EImumUiExtWizardCreated:
            params = ESmtpMtmUiCreateNewServiceSave;
            break;

        // After options->exit has been chose, exit settings
        case EImumUiExit:
            params = ESmtpMtmUiCreateNewServiceExit;
            break;

        // After options->exit has been chose, save and exit settings
        case EImumUiSaveAndExit:
            params = ESmtpMtmUiCreateNewServiceSaveAndExit;
            break;

        }
    IMUM_OUT();

    return params;
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::HandlePredefinedMailboxCreationL
// ----------------------------------------------------------------------------
void CSmtpMtmUi::HandlePredefinedMailboxCreationL()
    {
    IMUM_CONTEXT( CSmtpMtmUi::HandlePredefinedMailboxCreationL, 0, KImumMtmLog );
    IMUM_IN();

    // First, it needs to be check, does the system contain previously defined
    // mailboxes. This is to prevent failure cases, when mtm attempts to
    // initialise email mtm's several times in row.
    MImumInHealthServices::RMailboxIdArray mailboxes;
    iMailboxApi->HealthServicesL().GetMailboxList( mailboxes );
    TBool allowCreation = ETrue;

    // Check if there's any predefined mailboxes available
    for ( TInt mailbox = mailboxes.Count();
        allowCreation && --mailbox >= 0; )
        {
        // If the key is not set, the mailbox is propably created by the
        // user
        TRAP_IGNORE( allowCreation = !ImumMboxSettingsUtils::QuickLoadL<TInt>( // CSI: 59 # codescanner gives critical error for some reason
            *iMailboxApi,
            mailboxes[mailbox],
            TImumInSettings::EKeyIsPredefinedMailbox ) );
        }
    mailboxes.Reset();

    // The mailbox creation can start
    if ( allowCreation )
        {
        // Create the mailbox creator unit
        CImumCreatorPredefinedMailbox* creator =
            CImumCreatorPredefinedMailbox::NewLC( *iMailboxApi );
        TUid protocol = TUid::Uid( 0 );

        // Create all possible mailboxes
        for ( TInt i=0; i<KImumMaxMailboxes; i++)
            {
            // Define
            CImumInSettingsData* account =
                iMailboxApi->MailboxServicesL().CreateSettingsDataL(
                    protocol );
            CleanupStack::PushL( account );

            if ( creator->DefinePredefinedMailboxL( i, *account ) )
                {
                iMailboxApi->MailboxServicesL().CreateMailboxL( *account );
                }

            // Mailbox has been saved, remove the object
            CleanupStack::PopAndDestroy( account );
            account = NULL;
            }

        CleanupStack::PopAndDestroy( creator );
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::CheckAndRestoreSmtpUserAndPasswordL
// ----------------------------------------------------------------------------
void CSmtpMtmUi::CheckAndRestoreSmtpUserAndPasswordL( TMsvId aSmtpServiceId )
    {
    IMUM_CONTEXT( CSmtpMtmUi::CheckAndRestoreSmtpUserAndPasswordL, 0, KImumMtmLog );
    IMUM_IN();

    // Get settings.
    TBuf8<KImasLoginLength> username8;
    TBuf8<KImasPasswordLength> password8;
    TBool isPasswordTemporary = EFalse;

    // Load login information
    ImumMboxSettingsUtils::GetLoginInformationL(
        *iMailboxApi,
        username8,
        password8,
        isPasswordTemporary,
        aSmtpServiceId );

        // Temporary information is not accepted as valid data
    if ( isPasswordTemporary )
        {
        // Have a valid password.
        password8.Zero();

        // Save login information
        ImumMboxSettingsUtils::SetLoginInformationL(
            *iMailboxApi,
            username8,
            password8,
            isPasswordTemporary,
            aSmtpServiceId );
        }
    IMUM_OUT();
    }


// ----------------------------------------------------------------------------
// CSmtpMtmUi::ValidateServiceL
// ----------------------------------------------------------------------------
void CSmtpMtmUi::ValidateServiceL( TDes8& aParameter )
    {
    IMUM_CONTEXT( CSmtpMtmUi::ValidateServiceL, 0, KImumMtmLog );
    IMUM_IN();

    DoValidateServiceL( iUtils->DefaultMailboxId(), aParameter );
    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CSmtpMtmUi::ValidateServiceL()
// ---------------------------------------------------------------------------
//
void CSmtpMtmUi::ValidateServiceL(
    const CMsvEntrySelection& aSelection,
    TDes8& aParameter )
    {
    IMUM_CONTEXT( CSmtpMtmUi::ValidateServiceL, 0, KImumMtmLog );
    IMUM_IN();

    if ( aSelection.Count() )
        {
        DoValidateServiceL( aSelection[0], aParameter );
        }
    IMUM_OUT();
    }

// ---------------------------------------------------------------------------
// CSmtpMtmUi::DoValidateServiceL()
// ---------------------------------------------------------------------------
//
void CSmtpMtmUi::DoValidateServiceL(
    const TMsvId aMailboxId,
    TDes8& aParameter )
    {
    IMUM_CONTEXT( CSmtpMtmUi::DoValidateServiceL, 0, KImumMtmLog );
    IMUM_IN();

    TInt result =
        iMailboxApi->HealthServicesL().IsMailboxHealthy( aMailboxId );
    // 0 = KErrNone, other values indicate error
    TPckgBuf<TInt> resultPackage( !result );
    aParameter.Copy( resultPackage );
    IMUM_OUT();
    }
// ---------------------------------------------------------------------------
// CSmtpMtmUi::ActivateViewL()
// Activates right view after email wizard
// ---------------------------------------------------------------------------
//
void CSmtpMtmUi::ActivateViewL( TImumUiExitCodes aExitCode, TMsvId aId )
    {
    IMUM_CONTEXT( CSmtpMtmUi::DoValidateServiceL, 0, KImumMtmLog );
    IMUM_IN();
    CAknViewAppUi* appUi = reinterpret_cast<CAknViewAppUi*>
        (CEikonEnv::Static()->EikAppUi());

    // If launched oustide of MCE, mce-view shouldn't be activated
    if ( appUi->Application()->AppDllUid() == TUid::Uid( KMceUid ) )
       {
        switch( aExitCode )
            {
            // If no mailbox created, let's return to main view
            case EImumUiNoChanges:
            case EImumUiClose:
            case EImumUiExit:
            	appUi->ActivateLocalViewL( KMceMainViewListViewId );
                break;
            // If mailbox created, let's open it
            case EImumUiSaveAndExit:
            case EImumUiSaveAndClose:
            case EImumUiExtWizardCreated:
                appUi->ActivateLocalViewL( KMceMessageListViewId,
                    TUid::Uid( aId ), KNullDesC8 );
                break;
            default:
                break;
            }
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CSmtpMtmUi::ScreenClearerCleanupOperation
// ----------------------------------------------------------------------------
//
void CSmtpMtmUi::ScreenClearerCleanupOperation( TAny* aScreenClearer )
	{
	CEikonEnv::Static()->EikAppUi()->RemoveFromStack( 
		static_cast<CAknLocalScreenClearer*>( aScreenClearer ) );
	}

// End of File



