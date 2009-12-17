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
*       Provides Notification MTM UI methods.
*
*/



// INCLUDE FILES

//Features
#include <featmgr.h>    
#include <bldvariant.hrh>
#include <MuiuMsvProgressReporterOperation.h> // CMsvProgressReporterOperation
#include <mtudreg.h>                 // MTM UI data registry
#include <mtudcbas.h>                // MTM UI data base

#include "NotMtmUi.h"

#include <apgcli.h>
#include <msvuids.h>
#include <mtmuidef.hrh>

#include <e32keys.h>
#include <coemain.h>
#include <eikenv.h>
#include <eikdef.h>
#include <eikmfne.h>
#include <eikedwin.h>
#include <eikcapc.h>
#include <gulicon.h>
#include <ErrorUI.h> //CErrorUI

#include <aknnotewrappers.h>                // CAknInformationNote
#include <aknPopup.h>
#include <aknlists.h>
#include <aknconsts.h>
#include <akncontext.h>
#include <akntabgrp.h>
#include <AknQueryDialog.h>
#include <StringLoader.h>               // link against CommonEngine.lib
#include <Muiumsginfo.h>
#include <MuiuMsvUiServiceUtilities.h>
#include <muiumsvuiserviceutilitiesinternal.h>

#include <akninputblock.h>              // CAknInputBlock
#include <MtmExtendedCapabilities.hrh>
#include <messagingvariant.hrh>

#include <centralrepository.h>    // link against centralrepository.lib
#include <messaginginternalcrkeys.h> // for Central Repository keys
#include <CoreApplicationUIsSDKCRKeys.h>

#include <ApUtils.h> //CApUtils, CCommsDatabase, EDatabaseTypeIAP

#include <NotUi.rsg>
#include <mmserrors.h>

#include "NotMtmUiPanic.h"
#include <mmsconst.h>
#include <mmscmds.h>    // EMmsDeleteSchedule

#include <mmsmsventry.h>
#include <mmsnotificationclient.h>
#include <mmsgenutils.h>
#include <mmssettings.h> // MMS Engine settings

#include "MmsMtmConst.h"
#include "MMBoxDialog.h"
#include "NotMtmUiData.h"

// CONSTANTS

_LIT( KAddressSeparator,", " );
_LIT( KCharLeftAddressIterator, "<" );
_LIT( KCharRightAddressIterator, ">" );
_LIT( KMmsCharCommaAndSpace, ", " );
_LIT( KMmsCharSpace, " " );

const TInt KMessageSize = 20;
const TInt KZero = 0; //constant for memory check
const TInt KMmsAdditionalCharsStringLength = 7;
const TUint KMmsLRMarker = 0x200E;
const TUid KUidUniEditor = {0x102072D8};

#define IS_PHONE_OFF_LINEL() MsvUiServiceUtilitiesInternal::IsPhoneOfflineL()

// MACROS
#define iMmsClient  ( static_cast<CMmsNotificationClientMtm&>( iBaseMtm ) )

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ---------------------------------------------------------
// NewNotMtmUiL
// Factory function
// ---------------------------------------------------------
//
EXPORT_C CBaseMtmUi* NewNotMtmUiL( CBaseMtm& aMtm, CRegisteredMtmDll& aRegisteredDll )
    {
    return CNotMtmUi::NewL( aMtm, aRegisteredDll );
    }


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNotMtmUi::CNotMtmUi
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CNotMtmUi::CNotMtmUi( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll )
    :   CBaseMtmUi( aBaseMtm, aRegisteredMtmDll )
    {
    }

// ---------------------------------------------------------
// CNotMtmUi::ConstructL
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CNotMtmUi::ConstructL()
    {
    LOGTEXT(_L8("NotMtmUi: Construct start"));
    CBaseMtmUi::ConstructL();
    iSettingsHandler = CMmsSettingsHandler::NewL( iMmsClient );
    
    TInt featureBitmask = 0;
    
    CRepository* repository = CRepository::NewL( KCRUidMuiuVariation );
    repository->Get( KMuiuMmsFeatures, featureBitmask );
    delete repository;
    repository = NULL;
    
    if ( featureBitmask & KMmsFeatureIdDeleteOptions )
        {
        iNotUiFlags |= EDeleteOptionsSupported;
        }
    featureBitmask = 0;
    
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdOfflineMode ) )
        {
        iNotUiFlags |= EOffline;
        }
    FeatureManager::UnInitializeLib();
    LOGTEXT(_L8("NotMtmUi: Construct end"));
    }

// ---------------------------------------------------------
// CNotMtmUi::NewL
// Two-phased constructor.
// ---------------------------------------------------------
// 
CNotMtmUi* CNotMtmUi::NewL( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll )
    {
    LOGTEXT( _L8( "NotMtmUi::NewL" ) );
    CNotMtmUi* self = new ( ELeave ) CNotMtmUi( aBaseMtm, aRegisteredMtmDll );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// ---------------------------------------------------------
// CNotMtmUi::~CNotMtmUi
// Destructor
// ---------------------------------------------------------
//
CNotMtmUi::~CNotMtmUi()
    {
    delete iSettingsHandler;
    delete iWaitDialog;
    }


// ---------------------------------------------------------
// CNotMtmUi::QueryCapability
// Capability check for UI mtm specific functionality. These should
// be minimal, as the default capability checking places are UI data MTM
// (UI) and client MTM (engine).
// ---------------------------------------------------------
TInt CNotMtmUi::QueryCapability( TUid aCapability, TInt& aResponse )
    {
    switch ( aCapability.iUid )
        {
        case KUidMsvMtmUiQueryMessagingInitialisation: // flow through
        case KUidMsvMtmQueryFactorySettings:
        case KUidMsvMtmQuerySupportValidateService:

            aResponse = ETrue;
            return KErrNone;
        default:
            // Actually this routes to client MTM capability check.
            return CBaseMtmUi::QueryCapability( aCapability, aResponse );
        }
    }

// ---------------------------------------------------------
// CNotMtmUi::CreateL
// Not supported.
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::CreateL( const TMsvEntry& /*aEntry*/, 
                                  CMsvEntry& /*aParent*/, 
                                  TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CNotMtmUi::CheckSettingsL() const
// Check MMS service settings
// ---------------------------------------------------------
//
EXPORT_C TBool CNotMtmUi::CheckSettingsL( CMmsSettingsDialog::TMmsExitCode& aExitCode ) const
    {
    // First reload the settings
    iSettingsHandler->RefreshSettingsL( );
    return iSettingsHandler->CheckSettingsL( aExitCode );
    }

// ---------------------------------------------------------
// CNotMtmUi::DeleteServiceL
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::DeleteServiceL( const TMsvEntry& /*aService*/, 
                                         TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CNotMtmUi::DeleteFromL
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::DeleteFromL( const CMsvEntrySelection& /*aSelection*/, 
                                      TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CMmsMtmUi::CopyToL
// Copy local outbox entry to remote server = called when sending the entry from MCE outbox
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::CopyToL( const CMsvEntrySelection& aSelection, 
                                  TRequestStatus& aStatus )
    {
    return CopyMoveToL( aSelection, aStatus, ETrue );
    }

// ---------------------------------------------------------
// CNotMtmUi::MoveToL
// Move local outbox entry to remote server.
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::MoveToL( const CMsvEntrySelection& aSelection, 
                                  TRequestStatus& aStatus )
    {
    return CopyMoveToL( aSelection, aStatus, EFalse );
    }

// ---------------------------------------------------------
// CNotMtmUi::CopyMoveToL
// Performs either CopyToL or MoveToL operation.
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::CopyMoveToL( const CMsvEntrySelection& aSelection, 
                                      TRequestStatus& aStatus, TBool aCopyOnly )
    {
    __ASSERT_DEBUG( iBaseMtm.Entry().Entry().iMtm==Type(), Panic( EMmsWrongMtm ) );
    __ASSERT_DEBUG( iBaseMtm.Entry().Entry().iType==KUidMsvServiceEntry, Panic( EMmsNotAService ) );

    // --- Carry out the operation ---
    TMsvEntry mmsService ( BaseMtm().Entry().Entry() );
    //    Get CMsvEntry of parent of messages to be sent
    CMsvEntry* parentEntry = Session().GetEntryL( aSelection[0] );    

    CleanupStack::PushL( parentEntry );
    parentEntry->SetEntryL( parentEntry->Entry().Parent() );
    
    // Do the copy/move
    CMsvOperation* op = aCopyOnly ?
        parentEntry->CopyL( aSelection, mmsService.Id(), aStatus ) :
        parentEntry->MoveL( aSelection, mmsService.Id(), aStatus );
    
    CleanupStack::PopAndDestroy();    //parentEntry
    return op;
    }

// ---------------------------------------------------------
// CNotMtmUi::CopyFromL
// Could be used in fetching messages manually, but client MTM API has simple FetchL()
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::CopyFromL(const CMsvEntrySelection& /*aSelection*/, 
                                    TMsvId /*aTargetId*/, 
                                    TRequestStatus& /*aStatus*/)
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CNotMtmUi::MoveFromL
// Could be used in fetching messages manually, but client MTM API has simple FetchL()
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::MoveFromL(const CMsvEntrySelection& /*aSelection*/, 
                                    TMsvId /*aTargetId*/, 
                                    TRequestStatus& /*aStatus*/)
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CNotMtmUi::OpenL
// Handles request to open existing message server entry (message/service)
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::OpenL( TRequestStatus& aStatus )
    {
    LOGTEXT(_L8("NotMtmUi: OpenL start"));
    __ASSERT_DEBUG( iBaseMtm.Entry().Entry().iMtm==Type(), Panic( EMmsWrongMtm ) );

    const TMsvEntry& entry = BaseMtm().Entry().Entry();
    if ( entry.ReadOnly() )
        {
        return ViewL( aStatus );
        }
    else
        {
        return EditL( aStatus );
        }
    }

// ---------------------------------------------------------
// CNotMtmUi::OpenL
// Opens the first entry in a selection
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::OpenL( TRequestStatus& aStatus, 
                                const CMsvEntrySelection& aSelection )
    {
    LOGTEXT(_L8("NotMtmUi: OpenL start"));
    __ASSERT_DEBUG( iBaseMtm.Entry().Entry().iMtm==Type(), Panic( EMmsWrongMtm ) );

    BaseMtm().SwitchCurrentEntryL( aSelection[0] );
    return OpenL( aStatus );
    }

// ---------------------------------------------------------
// CNotMtmUi::CloseL
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::CloseL( TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CNotMtmUi::CloseL
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::CloseL( TRequestStatus& /*aStatus*/, 
                                 const CMsvEntrySelection& /*aSelection*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CNotMtmUi::EditL
// Opens a message (editor) or service (settings dialog) for editing.
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::EditL( TRequestStatus& aStatus )
    {
    LOGTEXT(_L8("NotMtmUi::EditL start"));
    __ASSERT_DEBUG( iBaseMtm.Entry().Entry().iMtm==Type(), Panic( EMmsWrongMtm ) );

    CMsvOperation* msvoperation = NULL;

    switch ( iBaseMtm.Entry().Entry().iType.iUid )
        {
        case KUidMsvMessageEntryValue:
            {
            CMmsSettingsDialog::TMmsExitCode exitCode = 
                CMmsSettingsDialog::EMmsExternalInterrupt;

            if( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL(
                Session(),
                KZero ) )
                {
                User::Leave( KErrDiskFull );
                }

            if ( !CheckSettingsL( exitCode ) ) 
                {
                TInt32 ap = iSettingsHandler->MmsSettings()->AccessPoint( 0 );
                //settings saved with valid access point
                if( !( exitCode==CMmsSettingsDialog::EMmsBack && ApExistsL( ap ) ) )
                    {
                    return CompleteSettingsOperationL( aStatus, exitCode );
                    }
                }
 
            if ( iBaseMtm.Entry().Entry().Connected() )
                {
                User::Leave( KErrLocked ); 
                }
            LOGTEXT( _L8( "NotMtmUi::EditL launching editor" ) );
            msvoperation = LaunchEditorApplicationL( aStatus, iBaseMtm.Entry().Session() );
            }
            break;

        case KUidMsvServiceEntryValue:
            LOGTEXT( _L8( "NotMtmUi::EditL - launching settings" ) );
            msvoperation = OpenServiceSettingsDialogL( aStatus );
            break;

        case KUidMsvAttachmentEntryValue:   // flow through
        case KUidMsvFolderEntryValue:       // flow through
        default:
            User::Leave( KErrNotSupported );
            break;
        }

    return msvoperation;
    }

// ---------------------------------------------------------
// CNotMtmUi::EditL
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::EditL( TRequestStatus& aStatus, 
                                const CMsvEntrySelection& aSelection )
    {
    BaseMtm().SwitchCurrentEntryL( aSelection[0] );
    return EditL( aStatus );
    }

// ---------------------------------------------------------
// CNotMtmUi::ViewL
// Opens the message for reading in Viewer.
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::ViewL( TRequestStatus& aStatus )
    {
    LOGTEXT( _L8( "NotMtmUi::ViewL" ) );
    __ASSERT_DEBUG( iBaseMtm.Entry().Entry().iMtm == Type(), Panic( EMmsWrongMtm ) );

    CMsvOperation* msvoperation = NULL;

    switch ( iBaseMtm.Entry().Entry().iType.iUid )
        {
        case KUidMsvMessageEntryValue:
            // It's a multimedia message
            if ( iBaseMtm.Entry().Entry().Connected() )
                {
                User::Leave( KErrLocked );
                }
            // Open Viewer
            LOGTEXT( _L8( "NotMtmUi::ViewL - launching Viewer" ) );
            msvoperation = LaunchEditorApplicationL( aStatus, iBaseMtm.Entry().Session(), EReadOnly );
            break;
        case KUidMsvServiceEntryValue:
        case KUidMsvAttachmentEntryValue:
        case KUidMsvFolderEntryValue:
        default:
            User::Leave( KErrNotSupported );
            break;
        }
    return msvoperation; 
    }

// ---------------------------------------------------------
// CNotMtmUi::ViewL
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::ViewL( TRequestStatus& aStatus, 
                                const CMsvEntrySelection& aSelection )
    {
    __ASSERT_DEBUG( iBaseMtm.Entry().Entry().iMtm == Type(), Panic( EMmsWrongMtm ) );
    BaseMtm().SwitchCurrentEntryL( aSelection[0] );
    return ViewL( aStatus );
    }

// -----------------------------------------------------------------------------
// CNotMtmUi::LaunchEditorApplicationL()
// Does the actual opening.
// -----------------------------------------------------------------------------
//
CMsvOperation* CNotMtmUi::LaunchEditorApplicationL( TRequestStatus& aStatus, 
                                                   CMsvSession& aSession, 
                                                   TEditorType aEditorType )
    {
    LOGTEXT(_L8("NotMtmUi: LaunchEditorApplicationL start"));
    TEditorParameters temp;    

    if ( iBaseMtm.HasContext() )
        {
        temp.iId=iBaseMtm.Entry().EntryId();
        }

    TMsvEntry tempEntry = iBaseMtm.Entry( ).Entry();

    CMtmUiDataRegistry* mtmUiDataReg = CMtmUiDataRegistry::NewL( Session( ) );
    CleanupStack::PushL( mtmUiDataReg );

    CBaseMtmUiData* uiData=mtmUiDataReg->NewMtmUiDataLayerL( this->Type( ) ) ;
    CleanupStack::PushL( uiData );

    TBool supported = ETrue;

    if( uiData->OperationSupportedL( KMtmUiFunctionOpenMessage, tempEntry ) )
        {   // Returns value other than zero if operation is not supported
        supported = EFalse;
        }

    CleanupStack::PopAndDestroy( 2, mtmUiDataReg );

    if( !supported )
        {
        User::Leave( KErrNotSupported );
        }

    switch( aEditorType )
        {
        case EReadOnly:
            temp.iFlags|=EMsgReadOnly;
            break;                             
        case ECreateNewMessage:
            temp.iFlags|=EMsgCreateNewMessage ;
            temp.iId=KMsvDraftEntryIdValue;
            break;
        case EEditExisting: //flow through
        default:
            break;
        };

    temp.iFlags &= ~( EMtmUiFlagEditorPreferEmbedded|EMtmUiFlagEditorNoWaitForExit );

    temp.iFlags |= ( Preferences() & EMtmUiFlagEditorPreferEmbedded?EMsgLaunchEditorEmbedded:0 );

    TPckgC<TEditorParameters> paramPack( temp );
    
    LOGTEXT( _L8( "Just before calling MsgEditorLauncher" ) );
    return MsgEditorLauncher::LaunchEditorApplicationL( aSession, Type(), aStatus, temp, KNullDesC(), paramPack );
    }

// ---------------------------------------------------------
// CNotMtmUi::ForwardL
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::ForwardL( TMsvId aDestination, 
                                   TMsvPartList aPartList, 
                                   TRequestStatus& aCompletionStatus )
    {
    LOGTEXT(_L8("NotMtmUi: ForwardL start"));
    TEditorParameters editorParams;
    editorParams.iFlags &= ~( EMtmUiFlagEditorPreferEmbedded | EMtmUiFlagEditorNoWaitForExit );

    editorParams.iFlags |= ( ( Preferences() & EMtmUiFlagEditorPreferEmbedded ) ? EMsgLaunchEditorEmbedded : 0 );
    editorParams.iDestinationFolderId = aDestination; 
    editorParams.iPartList = aPartList;

    editorParams.iFlags |= EMsgForwardMessage;
    editorParams.iId=BaseMtm().Entry().EntryId();

    //we need to check access points here
    CMmsSettingsDialog::TMmsExitCode exitCode = CMmsSettingsDialog::EMmsExternalInterrupt;
    if ( !CheckSettingsL( exitCode ) )
        {
        TInt32 ap = iSettingsHandler->MmsSettings()->AccessPoint( 0 );
        if( exitCode == CMmsSettingsDialog::EMmsBack && ApExistsL( ap ) )
            {
            LOGTEXT( _L8( "NotMtmUi::ForwardL launching editor" ) );
            return MsgEditorLauncher::LaunchEditorApplicationL( Session(), 
                Type(), 
                aCompletionStatus, 
                editorParams, 
                KNullDesC() );
            }
        else//user selects exit and settings not saved
            {
            return CompleteSettingsOperationL( aCompletionStatus, exitCode );
            }
        }//if

    LOGTEXT( _L8( "NotMtmUi::ForwardL launching editor" ) );
    return MsgEditorLauncher::LaunchEditorApplicationL( Session(), 
        Type(), 
        aCompletionStatus, 
        editorParams, 
        KNullDesC() );
    }

// ---------------------------------------------------------
// CNotMtmUi::ReplyL
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::ReplyL( TMsvId aDestination, 
                                 TMsvPartList aPartList, 
                                 TRequestStatus& aCompletionStatus )
    {
    LOGTEXT(_L8("NotMtmUi: Reply start"));
    TEditorParameters editorParams;
    editorParams.iFlags |= EMsgReplyToMessageSender;
    editorParams.iId = BaseMtm().Entry().EntryId();
    editorParams.iDestinationFolderId = aDestination; 
    editorParams.iPartList = aPartList;
    editorParams.iSpecialAppId = KUidUniEditor;
    editorParams.iFlags &= ~( EMtmUiFlagEditorPreferEmbedded | EMtmUiFlagEditorNoWaitForExit );

    editorParams.iFlags |= ( ( Preferences() & EMtmUiFlagEditorPreferEmbedded ) ? EMsgLaunchEditorEmbedded : 0 );

    //we need to check access points here
    CMmsSettingsDialog::TMmsExitCode exitCode = CMmsSettingsDialog::EMmsExternalInterrupt;
    if ( !CheckSettingsL( exitCode ) )
        {
        TInt32 ap = iSettingsHandler->MmsSettings()->AccessPoint( 0 );
        if( exitCode == CMmsSettingsDialog::EMmsBack && ApExistsL( ap ) ) 
            {
            LOGTEXT( _L8( "NotMtmUi::ReplyL launching editor" ) );
            return MsgEditorLauncher::LaunchEditorApplicationL( Session(), 
                Type(), 
                aCompletionStatus, 
                editorParams, 
                KNullDesC() );
            }
        else//user selects exit and settings not saved
            {
            return CompleteSettingsOperationL( aCompletionStatus, exitCode );
            }
        }
    
    //Setting the entry always to read as done for sms  
    if ( BaseMtm().Entry().Entry().Unread() )
        {
        CMsvEntry* cEntry;
        cEntry = &(BaseMtm().Entry());
        TMsvEntry entry = cEntry->Entry();
        entry.SetUnread( EFalse );
        cEntry->ChangeL( entry );  	
        }
      
    LOGTEXT( _L8( "NotMtmUi::ReplyL launching editor" ) );
    return MsgEditorLauncher::LaunchEditorApplicationL( Session(), 
        Type(), 
        aCompletionStatus, 
        editorParams, 
        KNullDesC() );
    }

// ---------------------------------------------------------
// CNotMtmUi::CancelL
// Suspends sending of the selected messages (if possible)
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::CancelL( TRequestStatus& aStatus, 
                                  const CMsvEntrySelection& aSelection )
    {
    TInt count = aSelection.Count();
    __ASSERT_DEBUG( count, Panic( EMmsWrongParameters ) );

    CMsvEntry* entry = Session().GetEntryL( aSelection[0] );
    CleanupStack::PushL( entry );

    while ( count-- )
        {
        entry->SetEntryL( aSelection[count] );
        TMsvEntry tempEntry = entry->Entry();
        tempEntry.SetSendingState( KMsvSendStateSuspended );

        tempEntry.iDate.UniversalTime();

        entry->ChangeL( tempEntry );
        }

    CleanupStack::PopAndDestroy(); // entry
    
    TBuf8<1> dummyParams;
    dummyParams.Zero();
    return CBaseMtmUi::InvokeAsyncFunctionL( EMmsDeleteSchedule, aSelection, aStatus, dummyParams );
    }

// ---------------------------------------------------------
// CNotMtmUi::InvokeSyncFunctionL
// ---------------------------------------------------------
//
void CNotMtmUi::InvokeSyncFunctionL( TInt aFunctionId, 
                                    const CMsvEntrySelection& aSelection, 
                                    TDes8& aParameter )
    {
    LOGTEXT(_L8("NotMtmUi: InvokeSyncFuntionL start"));
    switch ( aFunctionId )
        {
    case KMtmUiMessagingInitialisation:
        LOGTEXT(_L8("NotMtmUi: InvokeSyncFuntionL MsgIni"));
        CreateDefaultSettingsL();
        break;
    case KMtmUiFunctionRestoreFactorySettings:
        LOGTEXT(_L8("NotMtmUi: InvokeSyncFuntionL FactorySetts"));
        HandleFactorySettingsL( aParameter );
        break;
    case KMtmUiFunctionValidateService:
        {
        LOGTEXT(_L8("NotMtmUi: InvokeSyncFuntionL ValidateService"));
        iSettingsHandler->RefreshSettingsL( );
        TInt error = iSettingsHandler->MmsSettings()->ValidateSettings();
        // return the result via aParameter as a TInt packaged into a TDes8
        TPckgBuf<TInt> resultPackage( error );
        aParameter.Copy( resultPackage );
        }
        break;
    default:
        CBaseMtmUi::InvokeSyncFunctionL( aFunctionId, aSelection, aParameter );
        break;
        }
    LOGTEXT(_L8("NotMtmUi: InvokeSyncFunctionL end"));
    }

// ---------------------------------------------------------
// CNotMtmUi::InvokeAsyncFunctionL
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::InvokeAsyncFunctionL( TInt aFunctionId, 
                                               const CMsvEntrySelection& aSelection, 
                                               TRequestStatus& aCompletionStatus, 
                                               TDes8& aParameter )
    {
    LOGTEXT(_L8("NotMtmUi: InvokeAsyncFuntionL start"));
    CMsvOperation* msvoperation = NULL;
    switch ( aFunctionId )
        {
        case KMtmUiFunctionMessageInfo:
            LOGTEXT(_L8("NotMtmUi: InvokeAsyncFuntionL MsgInfo"));
            msvoperation = OpenMessageInfoL( aCompletionStatus, aParameter );
            break;
        case KMtmUiFunctionMMBox:
            LOGTEXT(_L8("NotMtmUi: InvokeAsyncFuntionL MMBox"));
            msvoperation = OpenMMBoxDialogL( aCompletionStatus );
            break;
        case KMtmUiFunctionFetchMMS:
            LOGTEXT(_L8("NotMtmUi: InvokeAsyncFuntionL Fetch"));
            if(     iNotUiFlags & EOffline
                &&  IS_PHONE_OFF_LINEL() )
                {   // We are in offline at the moment -> Infonote
                ShowInformationNoteL( R_NOTMTM_OFFLINE_NOT_POSSIBLE, EFalse );
                break;
                }
            msvoperation = StartFetchingL( aSelection, aCompletionStatus ); 
            break;
        case KMtmUiFunctionDeleteMessage:
            LOGTEXT(_L8("NotMtmUi: InvokeAsyncFuntionL Delete"));
            msvoperation = StartDeletingL( aSelection, aCompletionStatus );
            break;
        default:
            break;
        }
    LOGTEXT(_L8("NotMtmUi: InvokeAsyncFuntionL end"));
    return msvoperation;
    }

// ---------------------------------------------------------
// CNotMtmUi::DisplayProgressSummary
// ---------------------------------------------------------
//
TInt CNotMtmUi::DisplayProgressSummary( const TDesC8& /* aProgress */ ) const
    {
    // This does nothing, because MsgErrorWatcher shows all MMS error messages.
    // Furthermore currently MMS engine does not return any sensible progress to report.
    return KErrNone;
    }

// ---------------------------------------------------------
// CNotMtmUi::GetResourceFileName
// ---------------------------------------------------------
//
void CNotMtmUi::GetResourceFileName( TFileName& aFileName ) const
    {
    aFileName = KNotMtmResourceFile;
    }

// ---------------------------------------------------------
// CNotMtmUi::CreateDefaultSettingsL
// ---------------------------------------------------------
//
void CNotMtmUi::CreateDefaultSettingsL()
    {
    // done in engine by FactorySettings
    }

// ---------------------------------------------------------
// CNotMtmUi::HandleFactorySettingsL
// Reset Factory Settings support
// ---------------------------------------------------------
EXPORT_C void CNotMtmUi::HandleFactorySettingsL( TDes8& aLevel )
    {
    iSettingsHandler->HandleFactorySettingsL( aLevel );
    }

// ---------------------------------------------------------
// CNotMtmUi::DeleteFromQuery()
// Queries from the user where the notifications
// or messages are being deleted from
// ---------------------------------------------------------
TInt CNotMtmUi::DeleteFromQueryL( TInt& result )
    {
    LOGTEXT(_L8("NotMtmUi: DeleteFromQuery start"));
    TInt index = 0;
    CAknListQueryDialog* dlg = new (ELeave) CAknListQueryDialog( &index );
    dlg->PrepareLC( R_NOTUI_DELETE_FROM_QUERY );

    // Populate list query array
    CDesCArrayFlat *array = new(ELeave) CDesCArrayFlat(3);
    CleanupStack::PushL(array);
    HBufC* buf = StringLoader::LoadLC( R_NOTMTM_DELETE_QUERY_BOTHLOCALANDREMOTE );
    array->AppendL( *buf );
    CleanupStack::PopAndDestroy(); //buf
    buf = StringLoader::LoadLC( R_NOTMTM_DELETE_QUERY_LOCALONLY );
    array->AppendL( *buf );
    CleanupStack::PopAndDestroy(); //buf
    buf = StringLoader::LoadLC( R_NOTMTM_DELETE_QUERY_REMOTEONLY );
    array->AppendL( *buf );
    CleanupStack::PopAndDestroy(); //buf
    dlg->SetItemTextArray( array );
    CleanupStack::Pop(); //array

    TInt queryOk = dlg->RunLD();
    if( queryOk )
            {
            /* list query approved, index is usable to see what list item was selected */
            switch( index )
                {
                case 0:
                    result = EMmsDeleteBoth;
                    break;
                case 1:
                    result = EMmsDeleteNotificationOnly;
                    break;
                case 2:
                default:
                    result = EMmsDeleteMMBoxOnly;
                    break;
                };
            }
    else
            {
            /* list query cancelled, index is unusable */
            }
    LOGTEXT(_L8("NotMtmUi: DeleteFromQueryL end"));
    return queryOk;
    }

// ---------------------------------------------------------
// CMsvOperation* CNotMtmUi::OpenServiceSettingsDialogL(TRequestStatus& aCompletionStatus)
// Handles MMS service settings
// ---------------------------------------------------------
//
EXPORT_C CMsvOperation* CNotMtmUi::OpenServiceSettingsDialogL( TRequestStatus& aCompletionStatus )
    {
    return iSettingsHandler->OpenServiceSettingsDialogL( aCompletionStatus );
    }

// ---------------------------------------------------------
// CheckSettingsL
// ---------------------------------------------------------
//
TBool CNotMtmUi::CheckSettingsL( )
    {
    CMmsSettingsDialog::TMmsExitCode exitCode = CMmsSettingsDialog::EMmsExternalInterrupt;
    if ( !CheckSettingsL( exitCode ) )
        {
        TInt32 ap = iSettingsHandler->MmsSettings()->AccessPoint( 0 );
        if( !( exitCode==CMmsSettingsDialog::EMmsBack && ApExistsL( ap ) ) )
            {
            return EFalse;
            }
        }
    return ETrue;        
    }

// ---------------------------------------------------------
// CMsvOperation* CNotMtmUi::CompleteSettingsOperationL(
//    TRequestStatus& aCompletionStatus, const CMmsSettingsDialog::TMmsExitCode& aExitCode)
// Creates completed operation after settings dialog closing with appropriate parameters.
// ---------------------------------------------------------
//
EXPORT_C CMsvOperation* CNotMtmUi::CompleteSettingsOperationL(
    TRequestStatus& aCompletionStatus,
    const CMmsSettingsDialog::TMmsExitCode& aExitCode /* = CMmsSettingsDialog::EMmsExternalInterrupt */ )
    {
    return iSettingsHandler->CompleteSettingsOperationL( 
                                aCompletionStatus,
                                aExitCode );
    }

// ---------------------------------------------------------
// TInt CNotMtmUi::LaunchSettingsDialogL(
//      CMmsSettingsDialog::TMmsExitCode& aExitCode )
// Opens either MMS Service Settings or MMS Sending Settings dialog.
// ---------------------------------------------------------
//
EXPORT_C void CNotMtmUi::LaunchSettingsDialogL(
    CMmsSettingsDialog::TMmsExitCode& aExitCode ) const
    {
    iSettingsHandler->LaunchSettingsDialogL( aExitCode );
    }
    

// ---------------------------------------------------------
// CNotMtmUi::OpenMessageInfoL(TRequestStatus& aCompletionStatus, TDes8& aParameter)
// Opens the Message Info popup.
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::OpenMessageInfoL( TRequestStatus& aCompletionStatus, 
                                           TDes8& /*aParameter*/ )
    {
    LOGTEXT( _L8( "NotMtmUi OpenMessageInfo start" ) );
    TInt popCount( 0 );

    // The items for Info are read from the message entry.
    // Note: Most of the stuff is in TMsvEntry, but not all -> MMS entry used all the way.
    TMmsMsvEntry entry = static_cast<TMmsMsvEntry>( BaseMtm().Entry().Entry() );
    __ASSERT_DEBUG( entry.iType==KUidMsvMessageEntry, Panic( EMmsNotAMessage ) );
    __ASSERT_DEBUG( entry.iMtm==Type(), Panic( EMmsWrongMtm ) );

    iMmsClient.LoadMessageL();

    TMsgInfoMessageInfoData infoData;

    // From - show only for received messages
    if ( entry.IsMobileTerminated() )
        {
        HBufC* senderNumber = iMmsClient.Sender().AllocLC();
        popCount++;
        // Additional space for chars: '<' '>' ',' + 2 x KMmsLRMarker
        HBufC* fromText = HBufC::NewLC( entry.iDetails.Length() + senderNumber->Length() + KMmsAdditionalCharsStringLength );
        popCount++;
        
        // Do we have alias?
        if ( senderNumber->Compare( entry.iDetails ) )
            { // yes
            TPtr fromTextPtr = fromText->Des();
            fromTextPtr.Append( entry.iDetails );
            fromTextPtr.Append( KMmsCharSpace );
            fromTextPtr.Append( KMmsLRMarker );
            fromTextPtr.Append( KCharLeftAddressIterator );
            fromTextPtr.Append( TMmsGenUtils::PureAddress( senderNumber->Des( ) ) );
            fromTextPtr.Append( KCharRightAddressIterator );
            fromTextPtr.Append( KMmsLRMarker );
            infoData.iFrom.Set( *fromText );
            }
        else
            { // no
            infoData.iFrom.Set( *senderNumber );
            }
        }
    
    // Subject
    infoData.iSubject.Set( iMmsClient.SubjectL() );

    // Supply UTC date-time  to message info
    infoData.iDateTime = entry.iDate;
    
    // Type of message -taken directly from resources as this must be a multimedia message
    HBufC* msgType = StringLoader::LoadLC( R_NOT_TYPE_TEXT, iCoeEnv );
    popCount++;
    infoData.iType.Set( msgType->Des() );
      
    // Priority of message
    HBufC* priority = NULL;
    switch ( iMmsClient.MessagePriority() )
    	{
        case EMmsPriorityHigh:
        	{
            priority  = StringLoader::LoadLC( R_MMS_MESSAGE_PRIORITY_HIGH, iCoeEnv );
            popCount++;
            infoData.iPriority.Set( *priority);
            break;
        	}
    	case EMmsPriorityLow:
    		{
	        priority  = StringLoader::LoadLC( R_MMS_MESSAGE_PRIORITY_LOW, iCoeEnv );
	        popCount++;
            infoData.iPriority.Set( *priority);
    		break;
    		}
        case EMmsPriorityNormal:
        	{
            priority  = StringLoader::LoadLC( R_MMS_MESSAGE_PRIORITY_NORMAL, iCoeEnv );
            popCount++;
            infoData.iPriority.Set( *priority);
            break;
            }
        default: // Do not show the priority if it's not set
            break;
        }

    // Size of message.
    TInt size = iMmsClient.MessageTransferSize( );
    TInt maxSize = static_cast<TInt>(
        iSettingsHandler->MmsSettings()->MaximumReceiveSize() );

    if( maxSize > 0 )
        {
        if( size > maxSize )
            {
            size = maxSize;
            }
        }

    // Finally make the UI string
    TBuf<KMessageSize> sizeString;
    MessageSizeToStringL( sizeString, size );
    infoData.iSize.Set( sizeString );

    // To
    const CMsvRecipientList& recipients = iMmsClient.AddresseeList();
    TInt count = recipients.Count(); 
    TPtrC separatorPtr( KAddressSeparator );

    // Calculate size needed
    TInt index;
    TInt toSize = 0; 

    for ( index = 0; index < count; index++ )
        {
        toSize += recipients[index].Length();
        if ( index < ( count - 1 ) )
            { // not the last address, add separator space
            toSize += separatorPtr.Length();
            }
        }

    if ( toSize )
        {
        // Form the to-string
        HBufC* toList = TurnRecipientsArrayIntoStringLC( recipients );
        popCount++;
        infoData.iTo.Set( *toList );
        }

    CMsgInfoMessageInfoDialog* infoDialog = CMsgInfoMessageInfoDialog::NewL();

    if ( entry.IsMobileTerminated() )
        {
        infoDialog->ExecuteLD( infoData, CMsgInfoMessageInfoDialog::EMmsViewer );
        }
    else
        {
        infoDialog->ExecuteLD( infoData, CMsgInfoMessageInfoDialog::EMmsEditor );
        }

    CleanupStack::PopAndDestroy( popCount );
    return CMsvCompletedOperation::NewL( Session(), 
        KUidMsvLocalServiceMtm, 
        KNullDesC8, 
        KMsvLocalServiceIndexEntryId, 
        aCompletionStatus );
    }


// ---------------------------------------------------------
// CNotMtmUi::OpenMMBoxDialogL
// Opens the MMBox Dialog
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::OpenMMBoxDialogL( TRequestStatus& aCompletionStatus )
    {
    LOGTEXT( _L8( "NotMtmUi OpenMMBoxDialog start" ) );

    TInt exitCode;

    CMtmUiDataRegistry* mtmUiDataReg = CMtmUiDataRegistry::NewL( Session( ) );
    CleanupStack::PushL( mtmUiDataReg );

    CBaseMtmUiData* mtmUiData = mtmUiDataReg->NewMtmUiDataLayerL( this->Type( ) );
    CleanupStack::PushL( mtmUiData );
    CNotMtmUiData* uiData = static_cast<CNotMtmUiData*> ( mtmUiData );

    CMMBoxDialog* dialog = CMMBoxDialog::NewL( *this, *uiData, Session(), exitCode );

    dialog->ExecuteLD( R_NOT_MMBOX_DIALOG );

    CleanupStack::PopAndDestroy( mtmUiData );
    CleanupStack::PopAndDestroy( mtmUiDataReg );

    return CMsvCompletedOperation::NewL( Session(), 
        KUidMsvLocalServiceMtm, 
        KNullDesC8, 
        KMsvLocalServiceIndexEntryId, 
        aCompletionStatus );
    }



// ---------------------------------------------------------
// CNotMtmUi::StartFetchingL
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::StartFetchingL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aCompletionStatus )
    {
    LOGTEXT( _L8( "NotMtmUi StartFetchingL start" ) );
    // Lets check the expiration if only one notification
    if( aSelection.Count( ) == 1 )
        {
        TMsvEntry entry;
        TMsvId service=-1;
        Session().GetEntry( aSelection.At( 0 ), service, entry );
	    if( entry.Parent( ) == KMsvGlobalInBoxIndexEntryId )
            { // We are in Inbox
            TMsvId origId = iMmsClient.Entry().Entry().Id();
            iMmsClient.SwitchCurrentEntryL( aSelection.At( 0 ) );
            iMmsClient.LoadMessageL( );
            TTime currentTime;
            currentTime.HomeTime( );
            TTime expiryTime = iMmsClient.ExpiryDate( );
            iMmsClient.SwitchCurrentEntryL( origId );
            iMmsClient.LoadMessageL( );
            TLocale locale;
            expiryTime += locale.UniversalTimeOffset();
            if (locale.QueryHomeHasDaylightSavingOn())          
                {
                TTimeIntervalHours daylightSaving(1);          
                expiryTime += daylightSaving;
                }

            if( currentTime > expiryTime )
                {   // Message is expired
                TBool res = EFalse;
                TRAPD( err, res=ShowConfirmationQueryL( R_NOT_MMBOX_QUEST_MSG_EXPIRED ) );
                if ( err || !res )
                    {   // User did not want to fetch expired message or the function leaved
                    CMmsSettingsDialog::TMmsExitCode exitCode = CMmsSettingsDialog::EMmsBack;
                    return CompleteSettingsOperationL( aCompletionStatus, exitCode );
                    } // else we continue
                }
            }
        }

    //we need to check access points here
    CMmsSettingsDialog::TMmsExitCode exitCode = CMmsSettingsDialog::EMmsExternalInterrupt;
    if ( !CheckSettingsL( exitCode ) )
        {
        TInt32 ap = iSettingsHandler->MmsSettings()->AccessPoint( 0 );
        //settings saved
        if( exitCode==CMmsSettingsDialog::EMmsBack && ApExistsL( ap ) )
            {
            CMsvOperation* op = NULL;
            CAknInputBlock* abs = CAknInputBlock::NewLC( );
            TRAP_IGNORE( op = iMmsClient.FetchMessagesL( aSelection, aCompletionStatus) );
            CleanupStack::PopAndDestroy( abs ); 
            return op;
            }
        else//user selects exit and settings not saved
            {
            return CompleteSettingsOperationL( aCompletionStatus, exitCode );
            }
        }
    else
        {
        // OK - settings ok and fetch could be done
        CAknInputBlock* abs = CAknInputBlock::NewLC( );
        CMsvOperation* op = NULL;
        TRAP_IGNORE( op = iMmsClient.FetchMessagesL( aSelection, aCompletionStatus) );
        CleanupStack::PopAndDestroy( abs ); 
        return op;
        }
    }

// ---------------------------------------------------------
// CNotMtmUi::StartDeletingL
// ---------------------------------------------------------
//
CMsvOperation* CNotMtmUi::StartDeletingL(
            const CMsvEntrySelection& aSelection,
            TRequestStatus& aCompletionStatus )
    {
    LOGTEXT( _L8( "NotMtmUi StartDeletingL start" ) );

    CMsvOperation* msvoperation = NULL;

    if( aSelection.Count( ) == 0 )
        { // Nothing to delete
        return msvoperation;
        }

    if( aSelection.Count( ) > 1 )
        {
        iFlags |= EDeletingMany;
        }
    else
        {
        iFlags &= ~EDeletingMany;
        }

    iFlags &= ~EDeletingFromMMBox;

    TInt deleteMode(-1);

    CMsvEntry* cEntry = Session().GetEntryL( aSelection[0] );
    CleanupStack::PushL( cEntry );

    TMsvEntry entry = cEntry->Entry( );

    CleanupStack::PopAndDestroy();    //cEntry

    if( entry.Parent( ) == KMsvGlobalInBoxIndexEntryId 
      ||  entry.Parent( ) == iMmsClient.GetMmboxFolderL( ) )
        {
        if(     iNotUiFlags & EOffline
            &&  IS_PHONE_OFF_LINEL() )
            {   // We are in offline at the moment -> Infonote
            ShowInformationNoteL( R_NOTMTM_OFFLINE_NOT_POSSIBLE, EFalse );
            return msvoperation;
            }
        }

    if(     ( entry.Parent( ) == KMsvGlobalInBoxIndexEntryId )
        &&  ( iNotUiFlags & EDeleteOptionsSupported ) 
        && DeleteQueryReallyNeededL( aSelection ) )
        { // Query is really needed (we r in Inbox, DeleteOptions supported
          // and the msg has not been deleted from server yet
        if( !( DeleteFromQueryL( deleteMode ) ) )
            {   // Answered Cancel
            return msvoperation; // Return NULL operation
            }
        if( deleteMode != EMmsDeleteNotificationOnly )
            {
            iFlags |= EDeletingFromMMBox;
            }
        }
    else // Either we have no options or we are deleting FR(s)
        {
        if( entry.Parent( ) == iMmsClient.GetMmboxFolderL( ) )
            {
            iFlags |= EDeletingFromMMBox;
            deleteMode = EMmsDeleteBoth;
            }
        else
            {
            deleteMode = EMmsDeleteNotificationOnly;
            }
        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        HBufC* prompt;
        if( iFlags & EDeletingMany )
            {
            prompt = StringLoader::LoadLC( R_NOTMTM_QUEST_DELETE_MESSAGES, aSelection.Count(), iCoeEnv );
            }
        else
            {
            prompt = StringLoader::LoadLC( R_NOTMTM_QUEST_DELETE_MESSAGE, iCoeEnv );
            }
        // Lets prompt the question
        TInt retVal = dlg->ExecuteLD( R_NOTUI_CONFIRMATION_QUERY, *prompt );
        CleanupStack::PopAndDestroy( prompt );

        if( !retVal )
            { // Answered NO
            return msvoperation; // Return NULL operation
            }
        }
    if( deleteMode == -1 )
        {   // For some reason deleteMode was not set
        return msvoperation; // Return NULL operation
        }
	if( iFlags & EDeletingFromMMBox )
		{
		CMmsSettingsDialog::TMmsExitCode exitCode = CMmsSettingsDialog::EMmsExternalInterrupt;
		if ( !CheckSettingsL( exitCode ) )
			{
			TInt32 ap = iSettingsHandler->MmsSettings()->AccessPoint( 0 );
			// just return NULL if the settings are not all right
			if( !ApExistsL( ap ) )
				{
				return msvoperation;
				}
			}
		}
    // Now we can start the deleting process
    if(     entry.Parent( ) == KMsvGlobalInBoxIndexEntryId 
        ||  entry.Parent( ) == iMmsClient.GetMmboxFolderL( ) )
        {   // We are now in Inbox

        CMsvProgressReporterOperation* reporter = 
            CMsvProgressReporterOperation::NewL(
                Session(),
                aCompletionStatus,
                EMbmAvkonQgn_note_erased );

        CleanupStack::PushL(reporter);

        CMsvOperation* deleteop = iMmsClient.UnscheduledDeleteNotificationL(
                            aSelection,
                            (TMmsDeleteOperationType)deleteMode,
                            reporter->RequestStatus() );
        reporter->SetProgressDecoder( *this );
        reporter->SetOperationL( deleteop ); // Takes immediate ownership
        CleanupStack::Pop(); //reporter
        msvoperation = reporter;
        }
    else 
        { // We are in Outbox/Sent folder -> Synchronously Delete FR
        TInt waitNoteId = 0;
        waitNoteId = aSelection.Count( ) == 1?
            R_NOTMTM_WAIT_DELETING:R_NOTMTM_WAIT_DELETING_MANY;
        ShowWaitNoteLC( waitNoteId, ETrue); // Show deleting note

        iMmsClient.DeleteForwardEntryL( aSelection );

        if( iWaitDialog )
            {
            CleanupStack::PopAndDestroy( ); // iWaitDialog
            iWaitDialog=NULL;
            }          

        return msvoperation; // Return NULL operation as DeleteForwardEntryL is actually synchronous
        }

    return msvoperation;
    }


// ---------------------------------------------------------
// CNotMtmUi::TurnRecipientsArrayIntoStringLC
//
// ---------------------------------------------------------
HBufC* CNotMtmUi::TurnRecipientsArrayIntoStringLC( const CMsvRecipientList& aRecip ) const
    {
    //calc length of string needed
    TInt stringLength = 0;
    TInt index;
    TInt count = aRecip.Count();
    for( index = 0; index < count; index++ )
        {
        stringLength += TMmsGenUtils::Alias( aRecip[index] ).Length();
        stringLength += TMmsGenUtils::PureAddress( aRecip[index] ).Length();
        // Additional space for chars: '<' '>' ',' + 2 x KMmsLRMarker
        stringLength += KMmsAdditionalCharsStringLength;
        }
    //construct string
    HBufC* toList = HBufC::NewLC( stringLength );
    for( index=0; index < count; index++ )
        {
        TPtrC name = TMmsGenUtils::Alias( aRecip[index] );
        TPtrC number = TMmsGenUtils::PureAddress( aRecip[index] );
        TPtr toListPtr = toList->Des();
        if( name.Length())
            {
            toListPtr.Append( name );
            toListPtr.Append( KMmsCharSpace );
            toListPtr.Append( KMmsLRMarker );
            toListPtr.Append( KCharLeftAddressIterator );
            toListPtr.Append( number );
            toListPtr.Append( KCharRightAddressIterator );
            toListPtr.Append( KMmsLRMarker );
            }
        else
            {
            toListPtr.Append( number );
            }
        
        if ( index < ( count - 1 ) )
            {
            toListPtr.Append( KMmsCharCommaAndSpace );
            }
        }

    return toList;
    }       



// ---------------------------------------------------------
// CNotMtmUi::MessageSizeToStringL(TDes& aTarget, TInt aFileSize)
// Converts message size in bytes to a kilobyte string. Rounds the bytes up to the
// next full kilo. I.e:
// 0 -> 0KB
// 1 -> 1KB
// 1024 -> 1KB
// 1025 -> 2KB
// ---------------------------------------------------------
//
void CNotMtmUi::MessageSizeToStringL( TDes& aTarget, TInt aFileSize ) const
    {
    TInt fileSize = aFileSize / KKiloByte;
    if ( aFileSize % KKiloByte )
        {
        fileSize++;
        }

    HBufC* buf = StringLoader::LoadLC( R_NOT_MESSAGE_SIZE_KB, fileSize, iEikonEnv );
    TPtr ptr = buf->Des();
    ptr.Trim(); // remove possible white space from beginning
    aTarget.Append( ptr );

    CleanupStack::PopAndDestroy(); // buf
    }

// ---------------------------------------------------------
// CNotMtmUi::ShowConfirmationQueryL
// ---------------------------------------------------------
//
TInt CNotMtmUi::ShowConfirmationQueryL( TInt aResourceId ) const
    {
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    HBufC* prompt = StringLoader::LoadLC( aResourceId, iCoeEnv );
    TInt retVal = dlg->ExecuteLD( R_NOTUI_CONFIRMATION_QUERY, *prompt );
    CleanupStack::PopAndDestroy( prompt );
    return retVal;
    }

// ---------------------------------------------------------
// CNotMtmUi::ApExistsL
// Checks the existence of given access point
// according to id.
// ---------------------------------------------------------
//
EXPORT_C TBool CNotMtmUi::ApExistsL( TInt32 aAp )
    {
    return iSettingsHandler->ApExistsL( aAp );
    }

// ---------------------------------------------------------
// ConstructWaitNoteLC
// ---------------------------------------------------------

void CNotMtmUi::ConstructWaitNoteLC(
    TBool aVisibilityDelayOff )
    {
    delete iWaitDialog;
    iWaitDialog = NULL;
    iWaitDialog = new (ELeave) CAknWaitDialog( 
        reinterpret_cast<CEikDialog**>( &iWaitDialog ), 
        aVisibilityDelayOff );
    CleanupStack::PushL( TCleanupItem( CleanupWaitDialog, (TAny**)&iWaitDialog ) ); 
    iWaitDialog->PrepareLC( DELETING_WAIT_NOTE );
    }

// ---------------------------------------------------------
// ShowWaitNoteLC
// ---------------------------------------------------------

void CNotMtmUi::ShowWaitNoteLC(
    TInt aNoteTxtResourceId, 
    TBool aVisibilityDelayOff )
    {
    ConstructWaitNoteLC( aVisibilityDelayOff );
    HBufC* text = StringLoader::LoadLC( aNoteTxtResourceId, iCoeEnv );
    iWaitDialog->SetTextL( *text );
    CleanupStack::PopAndDestroy( ); // text
    iWaitDialog->RunLD( );
    }
    
// ---------------------------------------------------------
// CleanupWaitDialog
// ---------------------------------------------------------
//
void CNotMtmUi::CleanupWaitDialog( TAny* aAny ) 
    { 
    CAknWaitDialog** dialog = (CAknWaitDialog**) aAny; 
    if ( dialog && *dialog )
        { 
        delete *dialog;
        } 
    *dialog = NULL;
    dialog = NULL;
    } 

//--------------------------------
// DecodeProgress
//--------------------------------
TInt CNotMtmUi::DecodeProgress(
    const TDesC8& /*aProgress*/, 
    TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString, 
    TInt& aTotalEntryCount, 
    TInt& aEntriesDone,
    TInt& aCurrentEntrySize, 
    TInt& aCurrentBytesTrans, 
    TBool /*aInternal*/ )
    {
    aCurrentEntrySize = 0;
    aCurrentBytesTrans = 0;   
    aEntriesDone = 0;
    aTotalEntryCount = 0;

    if( iFlags & EDeletingFromMMBox )
        {
        StringLoader::Load( aReturnString,
                            R_NOTMTM_WAIT_DELETING_REMOTE,
                            iCoeEnv );
        }
    else if( iFlags & EDeletingMany )
        {
        StringLoader::Load( aReturnString,
                            R_NOTMTM_WAIT_DELETING_MANY,
                            iCoeEnv );
        }
    else
        {
        StringLoader::Load( aReturnString,
                            R_NOTMTM_WAIT_DELETING,
                            iCoeEnv );
        }
    return KErrNone;
    }

//--------------------------------
// DeleteQueryRequiredByMtm
//--------------------------------
EXPORT_C TBool CNotMtmUi::DeleteQueryRequiredByMtm()
    {
    TBool result(EFalse);
    if( iNotUiFlags & EDeleteOptionsSupported )
        {
        return ETrue;
        }
    return result;
    }

//--------------------------------
// DeleteQueryReallyNeeded
//--------------------------------
TBool CNotMtmUi::DeleteQueryReallyNeededL( const CMsvEntrySelection& aSelection )
    {
    TBool result( ETrue );
    if( aSelection.Count( ) == 1 )
        {
        CMsvEntry* cEntry = Session().GetEntryL( aSelection[0] );
        CleanupStack::PushL( cEntry );

        TMsvEntry entry = cEntry->Entry( );

        CleanupStack::PopAndDestroy();    //cEntry

        // Delete from server is not needed if it's been deleted
        if(     ( entry.iMtmData2 & KMmsOperationFetch ) // Fetch+Forward=DELETE!
            &&  ( entry.iMtmData2 & KMmsOperationForward )  // 
            &&  ( entry.iMtmData2 & KMmsOperationFinished )  // Finished
            && !( entry.iMtmData2 & KMmsOperationResult ) ) // Succesfully
            {   // Returns false if only one entry available
            // and it has been deleted from server already
            result = EFalse;
            }
        }
    return result;
    }

// ---------------------------------------------------------
// CNotMtmUi::ShowInformationNoteL
// ---------------------------------------------------------
//
void CNotMtmUi::ShowInformationNoteL( TInt aResourceID, TBool aWaiting )
    {
    HBufC* string = NULL;
    string = StringLoader::LoadLC( aResourceID, iCoeEnv );

    CAknInformationNote* note = new ( ELeave )
        CAknInformationNote( aWaiting );
    note->ExecuteLD( *string );

    CleanupStack::PopAndDestroy(); //string
    }

//  End of File
