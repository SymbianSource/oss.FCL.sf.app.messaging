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
*       Provides MMS MTM UI methods.
*
*/



// INCLUDE FILES

#include <apgcli.h>
#include <msvuids.h>
#include <mtmuidef.hrh>
#include <mtmdef.h>

#include <msvstore.h>
#include <mmsvattachmentmanager.h>

#include <e32keys.h>
#include <coemain.h>
#include <eikenv.h>
#include <eikdef.h>
#include <eikmfne.h>
#include <eikedwin.h>
#include <eikcapc.h>
#include <gulicon.h>
#include <ErrorUI.h> //CErrorUI

#include <aknPopup.h>
#include <aknlists.h>
#include <aknconsts.h>
#include <akncontext.h>
#include <akntabgrp.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>

#include <centralrepository.h>    // link against centralrepository.lib
#include <messaginginternalcrkeys.h> // for Central Repository keys
#include <cmsvrecipientlist.h>
#include <messagingvariant.hrh>     // additional headers variation flags

#include <StringLoader.h>

#include <Muiumsginfo.h>
#include <MuiuMsvUiServiceUtilities.h>
#include <MuiuOperationWait.h>
#include <muiumsguploadparam.h>

#include <MtmExtendedCapabilities.hrh>
#include <ApUtils.h> //CApUtils, CCommsDatabase, EDatabaseTypeIAP
#include <MmsUi.rsg>
#include <mmsconst.h>
#include <mmscmds.h>    // EMmsDeleteSchedule
#include <mmsmsventry.h>
#include <mmsclient.h>
#include <mmsgenutils.h>
#include <mmssettings.h> // MMS Engine settings

// Features
#include <featmgr.h>    
#include <bldvariant.hrh>

// DRM
#include <fileprotectionresolver.h>

#include "MmsMtmUiPanic.h"
#include "MmsSettingsDialog.h"
#include "MmsMtmConst.h"
#include "MmsMtmUi.h"
// For Delivery Status
#include "CMmsMtmDeliveryDialogLauncher.h"
// CONSTANTS

_LIT( KCharLeftAddressIterator, "<" );
_LIT( KCharRightAddressIterator, ">" );
_LIT( KMmsCharCommaAndSpace, ", " );
_LIT( KMmsCharSpace, " " );

const TInt KMessageSize = 20;
const TInt KMmsAdditionalCharsStringLength = 7;
const TUint KMmsLRMarker = 0x200E;

const TUint KMmsPostcardAppId = 0x10207247;

const TUint KAmsAppId = 0x1020745A;


// MACROS
#define iMmsClient  ( static_cast<CMmsClientMtm&>( iBaseMtm ) )

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ---------------------------------------------------------
// NewMmsMtmUiL
// Factory function
// ---------------------------------------------------------
//
EXPORT_C CBaseMtmUi* NewMmsMtmUiL( CBaseMtm& aMtm, CRegisteredMtmDll& aRegisteredDll )
    {
    return CMmsMtmUi::NewL( aMtm, aRegisteredDll );
    }


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMmsMtmUi::CMmsMtmUi
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMmsMtmUi::CMmsMtmUi( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll )
    :   CBaseMtmUi( aBaseMtm, aRegisteredMtmDll )
    {
    }

// ---------------------------------------------------------
// CMmsMtmUi::ConstructL
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMmsMtmUi::ConstructL()
    {
    LOGTEXT( _L8( "MmsMtmUi::ConstructL, base construct" ) );
    CBaseMtmUi::ConstructL();
    LOGTEXT( _L8( "MmsMtmUi::ConstructL, settings handler" ) );
    iSettingsHandler = CMmsSettingsHandler::NewL( iMmsClient );
    
    // Changes for the cr # 401-1806
    // Get supported features from feature manager.
    // Since mmsui mtm is loaded by mce firsttime it is launched and it is reloaded after an mms operation is performed , 
    // the usage of the flags iSupportAudioMessaging & iSupportPostcard  may not be correct because the feature status might be affected
    // since the feature status can be modified before starting new mms operation
    FeatureManager::InitializeLibL();
    LOGTEXT( _L8( "MmsMtmUi::ConstructL, end" ) );
    }

// ---------------------------------------------------------
// CMmsMtmUi::NewL
// Two-phased constructor.
// ---------------------------------------------------------
// 
CMmsMtmUi* CMmsMtmUi::NewL( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll )
    {
    LOGTEXT( _L8( "MmsMtmUi::NewL, start" ) );
    CMmsMtmUi* self = new ( ELeave ) CMmsMtmUi( aBaseMtm, aRegisteredMtmDll );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    LOGTEXT( _L8( "MmsMtmUi::NewL, end" ) );
    return self;
    }

    
// ---------------------------------------------------------
// CMmsMtmUi::~CMmsMtmUi
// Destructor
// ---------------------------------------------------------
//
CMmsMtmUi::~CMmsMtmUi()
    {
    delete iSettingsHandler;
    
    if (iLauncher)
        {
        delete iLauncher;
        }
    // Changes for the cr # 401-1806
    FeatureManager::UnInitializeLib();        
    }


// ---------------------------------------------------------
// CMmsMtmUi::QueryCapability
// Capability check for UI mtm specific functionality. These should
// be minimal, as the default capability checking places are UI data MTM
// (UI) and client MTM (engine).
// ---------------------------------------------------------
TInt CMmsMtmUi::QueryCapability( TUid aCapability, TInt& aResponse )
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
// CMmsMtmUi::CreateL
// Launches MMS editor for creating a new message.
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::CreateL( const TMsvEntry& /*aEntry*/, 
                                  CMsvEntry&        /*aParent*/, 
                                  TRequestStatus&   /*aStatus*/ )
    {
    LOGTEXT( _L8( "MmsMtmUi::CreateL - launching editor" ) );
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CMmsMtmUi::CheckSettingsL() const
// Check MMS service settings
// ---------------------------------------------------------
//
TBool CMmsMtmUi::CheckSettingsL( CMmsSettingsDialog::TMmsExitCode& aExitCode ) const
    {
    iSettingsHandler->RefreshSettingsL( );
    return iSettingsHandler->CheckSettingsL( aExitCode );
    }

// ---------------------------------------------------------
// CMmsMtmUi::DeleteServiceL
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::DeleteServiceL( const TMsvEntry& /*aService*/, 
                                         TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CMmsMtmUi::DeleteFromL
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::DeleteFromL( const CMsvEntrySelection& /*aSelection*/, 
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
CMsvOperation* CMmsMtmUi::CopyToL( const CMsvEntrySelection& aSelection, 
                                  TRequestStatus& aStatus )
    {
    return CopyMoveToL( aSelection, aStatus, ETrue );
    }

// ---------------------------------------------------------
// CMmsMtmUi::MoveToL
// Move local outbox entry to remote server.
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::MoveToL( const CMsvEntrySelection& aSelection, 
                                  TRequestStatus& aStatus )
    {
    return CopyMoveToL( aSelection, aStatus, EFalse );
    }

// ---------------------------------------------------------
// CMmsMtmUi::CopyMoveToL
// Performs either CopyToL or MoveToL operation.
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::CopyMoveToL( const CMsvEntrySelection& aSelection, 
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
// CMmsMtmUi::CopyFromL
// Could be used in fetching messages manually, but client MTM API has simple FetchL()
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::CopyFromL(const CMsvEntrySelection& /*aSelection*/, 
                                    TMsvId /*aTargetId*/, 
                                    TRequestStatus& /*aStatus*/)
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CMmsMtmUi::MoveFromL
// Could be used in fetching messages manually, but client MTM API has simple FetchL()
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::MoveFromL(const CMsvEntrySelection& /*aSelection*/, 
                                    TMsvId /*aTargetId*/, 
                                    TRequestStatus& /*aStatus*/)
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CMmsMtmUi::OpenL
// Handles request to open existing message server entry (message/service)
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::OpenL( TRequestStatus& aStatus )
    {
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
// CMmsMtmUi::OpenL
// Opens the first entry in a selection
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::OpenL( TRequestStatus& aStatus, 
                                const CMsvEntrySelection& aSelection )
    {
    __ASSERT_DEBUG( iBaseMtm.Entry().Entry().iMtm==Type(), Panic( EMmsWrongMtm ) );

    BaseMtm().SwitchCurrentEntryL( aSelection[0] );
    return OpenL( aStatus );
    }

// ---------------------------------------------------------
// CMmsMtmUi::CloseL
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::CloseL( TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CMmsMtmUi::CloseL
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::CloseL( TRequestStatus& /*aStatus*/, 
                                 const CMsvEntrySelection& /*aSelection*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CMmsMtmUi::EditL
// Opens a message (editor) or service (settings dialog) for editing.
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::EditL( TRequestStatus& aStatus )
    {
    LOGTEXT(_L8("MmsMtmUi::EditL"));
    __ASSERT_DEBUG( iBaseMtm.Entry().Entry().iMtm==Type(), Panic( EMmsWrongMtm ) );

    CMsvOperation* msvoperation = NULL;

    switch ( iBaseMtm.Entry().Entry().iType.iUid )
        {
        case KUidMsvMessageEntryValue:
            {
            CMmsSettingsDialog::TMmsExitCode exitCode = 
                CMmsSettingsDialog::EMmsExternalInterrupt;

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
            LOGTEXT( _L8( "MmsMtmUi::EditL launching editor" ) );
            msvoperation = LaunchEditorApplicationL( aStatus, iBaseMtm.Entry().Session() );
            }
            break;

        case KUidMsvServiceEntryValue:
            LOGTEXT( _L8( "MmsMtmUi::EditL - launching settings" ) );
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
// CMmsMtmUi::EditL
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::EditL( TRequestStatus& aStatus, 
                                const CMsvEntrySelection& aSelection )
    {
    BaseMtm().SwitchCurrentEntryL( aSelection[0] );
    return EditL( aStatus );
    }

// ---------------------------------------------------------
// CMmsMtmUi::ViewL
// Opens the message for reading in Viewer.
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::ViewL( TRequestStatus& aStatus )
    {
    LOGTEXT( _L8( "MmsMtmUi::ViewL" ) );
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
            LOGTEXT( _L8( "MmsMtmUi::ViewL - launching Viewer" ) );
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
// CMmsMtmUi::ViewL
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::ViewL( TRequestStatus& aStatus, 
                                const CMsvEntrySelection& aSelection )
    {
    __ASSERT_DEBUG( iBaseMtm.Entry().Entry().iMtm == Type(), Panic( EMmsWrongMtm ) );
    BaseMtm().SwitchCurrentEntryL( aSelection[0] );
    return ViewL( aStatus );
    }

// -----------------------------------------------------------------------------
// CMmsMtmUi::LaunchEditorApplicationL()
// Does the actual opening.
// -----------------------------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::LaunchEditorApplicationL( TRequestStatus& aStatus, 
                                                   CMsvSession& aSession, 
                                                   TEditorType aEditorType )
    {

    TEditorParameters temp;    

    if ( iBaseMtm.HasContext() )
        {
        temp.iId = iBaseMtm.Entry().EntryId();
        // Changes for the cr # 401-1806
        if( FeatureManager::FeatureSupported( KFeatureIdMmsPostcard ) && IsPostcard( iBaseMtm.Entry().Entry() ) )
        	{
            aEditorType = ESpecialEditor;        		
        	}

        else if( FeatureManager::FeatureSupported( KFeatureIdAudioMessaging ) && IsAudioMessage( iBaseMtm.Entry().Entry() ) )
        	{
        	if( aEditorType == EReadOnly )
        		{
        		aEditorType = EReadOnlyAudioMessage;
        		}
        	else
	            {
	            aEditorType = ECreateNewAudioMessage;        		
	        	}
        	}
        }

    switch( aEditorType )
        {
        case EReadOnly:
            temp.iFlags|=EMsgReadOnly;
            break;                             
        case ECreateNewMessage:
            temp.iFlags|=EMsgCreateNewMessage ;
            temp.iId=KMsvDraftEntryIdValue;
            //appName already correct
            break;

        case ESpecialEditor:
            {
            temp.iSpecialAppId = TUid::Uid(KMmsPostcardAppId);
            }
            break;      
        case ECreateNewAudioMessage:
        	{
        	temp.iSpecialAppId = TUid::Uid( KAmsAppId );
        	}        	
        	break;       	
        case EReadOnlyAudioMessage:
        	{
        	temp.iSpecialAppId = TUid::Uid( KAmsAppId );
        	temp.iFlags|=EMsgReadOnly;
            break; 	
        	}
        case EEditExisting:  //fall through
            //appName already correct
        default:
            break;
        };

    temp.iFlags &= ~( EMtmUiFlagEditorPreferEmbedded|EMtmUiFlagEditorNoWaitForExit );

    temp.iFlags |= ( Preferences() & EMtmUiFlagEditorPreferEmbedded?EMsgLaunchEditorEmbedded:0 );

    TPckgC<TEditorParameters> paramPack( temp );
    
    LOGTEXT( _L8( "Just before calling MsgEditorLauncher" ) );
    return MsgEditorLauncher::LaunchEditorApplicationL( aSession, Type(), aStatus, temp, KNullDesC( ), paramPack );
    }

// ---------------------------------------------------------
// CMmsMtmUi::ForwardL
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::ForwardL( TMsvId aDestination, 
                                   TMsvPartList aPartList, 
                                   TRequestStatus& aCompletionStatus )
    {
    TEditorParameters editorParams;
    editorParams.iFlags &= ~( EMtmUiFlagEditorPreferEmbedded | EMtmUiFlagEditorNoWaitForExit );

    editorParams.iFlags |= ( ( Preferences() & EMtmUiFlagEditorPreferEmbedded ) ? EMsgLaunchEditorEmbedded : 0 );
    editorParams.iDestinationFolderId = aDestination; 
    editorParams.iPartList = aPartList;

    editorParams.iFlags |= EMsgForwardMessage;
    editorParams.iId=BaseMtm().Entry().EntryId();
    // Changes for the cr # 401-1806
    if( FeatureManager::FeatureSupported( KFeatureIdMmsPostcard ) && IsPostcard( iBaseMtm.Entry().Entry() ) )
    	{
		editorParams.iSpecialAppId = TUid::Uid(KMmsPostcardAppId);
		}

    //we need to check access points here
    CMmsSettingsDialog::TMmsExitCode exitCode = CMmsSettingsDialog::EMmsExternalInterrupt;
    if ( !CheckSettingsL( exitCode ) )
        {
        TInt32 ap = iSettingsHandler->MmsSettings()->AccessPoint( 0 );
        if( exitCode == CMmsSettingsDialog::EMmsBack && ApExistsL( ap ) )
            {
            LOGTEXT( _L8( "MmsMtmUi::ForwardL launching editor" ) );
            return MsgEditorLauncher::LaunchEditorApplicationL( Session(), 
                Type(), 
                aCompletionStatus, 
                editorParams, 
                KNullDesC( ) );
            }
        else//user selects exit and settings not saved
            {
            return CompleteSettingsOperationL( aCompletionStatus, exitCode );
            }
        }//if

    LOGTEXT( _L8( "MmsMtmUi::ForwardL launching editor" ) );
    return MsgEditorLauncher::LaunchEditorApplicationL( Session(), 
        Type(), 
        aCompletionStatus, 
        editorParams, 
        KNullDesC( ) );
    }


// ---------------------------------------------------------
// CMmsMtmUi::UploadL
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::UploadL(
        const CMsvEntrySelection& aSelection, 
        TRequestStatus& aCompletionStatus,
        TDes8& aParameter )
    {
    // Unpack parameters:
    TPckgBuf<TMsgUploadParameters> inputParamPack;
    inputParamPack.Copy( aParameter.Ptr(), sizeof( TMsgUploadParameters ) );
    TMsgUploadParameters& uploadParam = inputParamPack();

    // Create the message by utilizing ClientMTM's Forward operation:
    TMsvPartList parts =
        KMsvMessagePartBody |
        KMsvMessagePartDescription |
        KMsvMessagePartAttachments;

    if ( aSelection.Count() )
        {
        iMmsClient.SwitchCurrentEntryL( aSelection.At( 0 ) );
        iMmsClient.LoadMessageL();
        }
    
    TMsvId currentEntryId = iMmsClient.Entry().EntryId();
    
    if ( !UploadDRMChecksL() )
        {
        return CMsvCompletedOperation::NewL( Session(), 
            KUidMsvLocalServiceMtm, 
            KNullDesC8, 
            KMsvLocalServiceIndexEntryId, 
            aCompletionStatus );
        }

    CMuiuOperationWait* wait =
        CMuiuOperationWait::NewLC( EActivePriorityWsEvents + 10 );
    CMsvOperation* oper = iMmsClient.ForwardL( KMsvDraftEntryIdValue, parts, wait->iStatus );
    CleanupStack::PushL( oper );
    wait->Start();
    TMsvId temp;
    TPckgC<TMsvId> paramPack( temp );
    const TDesC8& progress = oper->FinalProgress();
    paramPack.Set( progress );
    TMsvId id = paramPack();
    CleanupStack::PopAndDestroy( 2 );  // wait, oper

    // Set the recipient
    CMsvEntry* cEntry = Session().GetEntryL( id );
    CleanupStack::PushL( cEntry );
    iMmsClient.SetCurrentEntryL( cEntry );
    CleanupStack::Pop( cEntry ); // ownership transferred
    iMmsClient.AddAddresseeL( uploadParam.iRealAddress, uploadParam.iAlias );
    iMmsClient.SaveMessageL();

    TMmsMsvEntry tEntry = static_cast<TMmsMsvEntry>( cEntry->Entry() );
    tEntry.iBioType = KUidMsgSubTypeMmsUpload.iUid; 
    tEntry.iDetails.Set( uploadParam.iAlias );
    tEntry.SetInPreparation( !uploadParam.iDirect );
    tEntry.SetForwardedMessage( ETrue );
    cEntry->ChangeL( tEntry );
    
    CMsvOperation* result = uploadParam.iDirect
        ? iMmsClient.SendL( aCompletionStatus )
        : EditL( aCompletionStatus );
    
    iMmsClient.SwitchCurrentEntryL( currentEntryId );
    
    return result;
    }

// ---------------------------------------------------------
// CMmsMtmUi::UploadDRMChecksL
// ---------------------------------------------------------
//
TBool CMmsMtmUi::UploadDRMChecksL()
    {
    CFileProtectionResolver* resolver = CFileProtectionResolver::NewLC( Session().FileSession() );

    CMsvStore* store = iMmsClient.Entry().ReadStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& attaManager = store->AttachmentManagerL();    
    
    TInt attaCount = attaManager.AttachmentCount();
    TInt flCount = 0;
    TInt sdCount = 0;

    for ( TInt i = 0; i < attaCount; i++ )
        {
        CMsvAttachment* info = attaManager.GetAttachmentInfoL( i );
        CleanupStack::PushL( info );
        TDataType dataType( info->MimeType() );
        
        RFile file = attaManager.GetAttachmentFileL( info->Id() );
        CleanupClosePushL( file );
        
        TInt status = resolver->ProtectionStatusL( file, dataType );
        if ( status & EFileProtForwardLocked ||
             status & EFileProtClosedContent )
            {
            flCount++;
            }
        else if ( status & EFileProtSuperDistributable )
            {
            sdCount++;
            }

        CleanupStack::PopAndDestroy( 2, info ); // file, info
        }

    CleanupStack::PopAndDestroy( 2, resolver ); //store, resolver

    if ( flCount )
        {
        CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
        HBufC* string = StringLoader::LoadLC( R_MMS_CANNOT_FW_DRM );
        note->ExecuteLD( *string );
        CleanupStack::PopAndDestroy( string );
        return EFalse;
        }
    if ( sdCount )
        {
        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        HBufC* string = StringLoader::LoadLC( sdCount == attaCount
            ? R_MMS_SEND_WO_RIGHTS
            : R_MMS_SEND_WO_RIGHTS_MIX );
        TInt retVal = dlg->ExecuteLD( R_MMS_CONFIRMATION_QUERY, *string );
        CleanupStack::PopAndDestroy( string );
        return retVal;
        }
    return ETrue;
    }


// ---------------------------------------------------------
// CMmsMtmUi::ReplyL
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::ReplyL( TMsvId aDestination, 
                                 TMsvPartList aPartList, 
                                 TRequestStatus& aCompletionStatus )
    {
    TEditorParameters editorParams;
    editorParams.iFlags |= EMsgReplyToMessageSender;
    editorParams.iId = BaseMtm().Entry().EntryId();
    editorParams.iDestinationFolderId = aDestination; 
    editorParams.iPartList = aPartList;
    editorParams.iFlags &= ~( EMtmUiFlagEditorPreferEmbedded | EMtmUiFlagEditorNoWaitForExit );

    editorParams.iFlags |= ( ( Preferences() & EMtmUiFlagEditorPreferEmbedded ) ? EMsgLaunchEditorEmbedded : 0 );

	  // Changes for the cr # 401-1806
	  if ( FeatureManager::FeatureSupported( KFeatureIdAudioMessaging ) )
    	{
		if( IsAudioMessage( iBaseMtm.Entry().Entry() ) )
	    	{
			editorParams.iSpecialAppId = TUid::Uid( KAmsAppId );
			}
    	}

    //we need to check access points here
    CMmsSettingsDialog::TMmsExitCode exitCode = CMmsSettingsDialog::EMmsExternalInterrupt;
    if ( !CheckSettingsL( exitCode ) )
        {
        TInt32 ap = iSettingsHandler->MmsSettings()->AccessPoint( 0 );
        if( exitCode == CMmsSettingsDialog::EMmsBack && ApExistsL( ap ) ) 
            {
            LOGTEXT( _L8( "MmsMtmUi::ReplyL launching editor" ) );
            return MsgEditorLauncher::LaunchEditorApplicationL( Session(), 
                Type(), 
                aCompletionStatus, 
                editorParams, 
                KNullDesC( ) );
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
  
    LOGTEXT( _L8( "MmsMtmUi::ReplyL launching editor" ) );
    return MsgEditorLauncher::LaunchEditorApplicationL( Session(), 
        Type(), 
        aCompletionStatus, 
        editorParams, 
        KNullDesC( ) );
    }

// ---------------------------------------------------------
// CMmsMtmUi::CancelL
// Suspends sending of the selected messages (if possible)
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::CancelL( TRequestStatus& aStatus, 
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
// CMmsMtmUi::InvokeSyncFunctionL
// ---------------------------------------------------------
//
void CMmsMtmUi::InvokeSyncFunctionL( TInt aFunctionId, 
                                    const CMsvEntrySelection& aSelection, 
                                    TDes8& aParameter )
    {
    switch ( aFunctionId )
        {
    case KMtmUiMessagingInitialisation:
        CreateDefaultSettingsL();
        break;
    case KMtmUiFunctionRestoreFactorySettings:
        HandleFactorySettingsL( aParameter );
        break;
    case KMtmUiFunctionValidateService:
        {
        //TMsvId serviceId = iMmsClient.DefaultSettingsL();
        //TInt error = iMmsClient.ValidateService( serviceId );
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

    }

// ---------------------------------------------------------
// CMmsMtmUi::InvokeAsyncFunctionL
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::InvokeAsyncFunctionL( TInt aFunctionId, 
                                               const CMsvEntrySelection& aSelection, 
                                               TRequestStatus& aCompletionStatus, 
                                               TDes8& aParameter )
    {
    CMsvOperation* msvoperation = NULL;

    switch ( aFunctionId )
        {
    case KMtmUiFunctionDeliveryStatus:
        msvoperation = OpenDeliveryPopupL( aCompletionStatus, aParameter );
        break;
    case KMtmUiFunctionMessageInfo:
        msvoperation = OpenMessageInfoL( aCompletionStatus );
        break;
    case KMtmUiFunctionUpload:
        msvoperation = UploadL( aSelection, aCompletionStatus, aParameter );
        break;
    default:
        msvoperation = CBaseMtmUi::InvokeAsyncFunctionL( aFunctionId, 
            aSelection, 
            aCompletionStatus, 
            aParameter);
        break;
        }
    return msvoperation;
    }

// ---------------------------------------------------------
// CMmsMtmUi::DisplayProgressSummary
// ---------------------------------------------------------
//
TInt CMmsMtmUi::DisplayProgressSummary( const TDesC8& /* aProgress */ ) const
    {
    // This does nothing, because MsgErrorWatcher shows all MMS error messages.
    // Furthermore currently MMS engine does not return any sensible progress to report.
    return KErrNone;
    }

// ---------------------------------------------------------
// CMmsMtmUi::GetResourceFileName
// ---------------------------------------------------------
//
void CMmsMtmUi::GetResourceFileName( TFileName& aFileName ) const
    {
    aFileName = KMmsMtmResourceFile;
    }

// ---------------------------------------------------------
// CMmsMtmUi::CreateDefaultSettingsL
// ---------------------------------------------------------
//
void CMmsMtmUi::CreateDefaultSettingsL()
    {
    // done in engine by FactorySettings
    }

// ---------------------------------------------------------
// CMmsMtmUi::HandleFactorySettingsL
// Reset Factory Settings support
// ---------------------------------------------------------
void CMmsMtmUi::HandleFactorySettingsL( TDes8& aLevel )
    {
    iSettingsHandler->HandleFactorySettingsL( aLevel );
    }


// ---------------------------------------------------------
// CMsvOperation* CMmsMtmUi::OpenServiceSettingsDialogL(TRequestStatus& aCompletionStatus)
// Handles MMS service settings
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::OpenServiceSettingsDialogL( TRequestStatus& aCompletionStatus )
    {
    iSettingsHandler->RefreshSettingsL( );
    return iSettingsHandler->OpenServiceSettingsDialogL( aCompletionStatus );
    }

// ---------------------------------------------------------
// CMsvOperation* CMmsMtmUi::CompleteSettingsOperationL(
//    TRequestStatus& aCompletionStatus, const CMmsSettingsDialog::TMmsExitCode& aExitCode)
// Creates completed operation after settings dialog closing with appropriate parameters.
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::CompleteSettingsOperationL(
    TRequestStatus& aCompletionStatus,
    const CMmsSettingsDialog::TMmsExitCode& aExitCode /* = CMmsSettingsDialog::EMmsExternalInterrupt */ )
    {
    return iSettingsHandler->CompleteSettingsOperationL( 
                                aCompletionStatus,
                                aExitCode );
    }

// ---------------------------------------------------------
// TInt CMmsMtmUi::LaunchSettingsDialogL(
//      CMmsSettingsDialog::TMmsExitCode& aExitCode )
// Opens either MMS Service Settings or MMS Sending Settings dialog.
// ---------------------------------------------------------
//
void CMmsMtmUi::LaunchSettingsDialogL(
    CMmsSettingsDialog::TMmsExitCode& aExitCode ) const
    {
    iSettingsHandler->LaunchSettingsDialogL( aExitCode );
    }

// ---------------------------------------------------------
// CMmsMtmUi::OpenMessageInfoL(TRequestStatus& aCompletionStatus, TDes8& aParameter)
// Opens the Message Info popup.
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::OpenMessageInfoL( TRequestStatus& aCompletionStatus )
    {
    LOGTEXT( _L8( "MmsMtmUi OpenMessageInfo start" ) );
    // Changes for the cr # 401-1806
    if( FeatureManager::FeatureSupported( KFeatureIdMmsPostcard ) && IsPostcard( BaseMtm( ).Entry( ).Entry( ) ) )
    	{
        return OpenPostcardInfoL( aCompletionStatus );	
    	}
    TInt popCount( 0 );

    // get additional headers variation info
    TInt addHeadersBitmask = 0;    

    CRepository* repository = CRepository::NewL( KCRUidMuiuVariation );
    repository->Get( KMuiuMmsFeatures, addHeadersBitmask );
    delete repository;
    
    // MMS Priority feature is enabled
    if ( addHeadersBitmask & KMmsFeatureIdPrioritySupport )
        {
        iPriorityFeatureSupported = ETrue;
        }
        
    // reminder of bit flags:
    // addHeadersBitmask & KMmsFeatureIdEditorSubjectField
    // addHeadersBitmask & KMmsFeatureIdEditorCcField 
    // addHeadersBitmask & KMmsFeatureIdEditorBccField


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
    
    // always shown for MT messages, for MO messages only if variated On
    if (    entry.IsMobileTerminated() 
        ||  addHeadersBitmask & KMmsFeatureIdEditorSubjectField )
        {
        infoData.iSubject.Set( iMmsClient.SubjectL() );
        }
        
    // Supply UTC date-time  to message info
    infoData.iDateTime = entry.iDate;
    // Changes for the cr # 401-1806
    if( FeatureManager::FeatureSupported( KFeatureIdAudioMessaging ) && IsAudioMessage( BaseMtm( ).Entry( ).Entry( ) ) )
    	{
        HBufC* msgType = StringLoader::LoadLC( R_AUDIOMESSAGE_TYPE_TEXT, iCoeEnv );
        popCount++;
    	infoData.iType.Set( msgType->Des() );
    	}
    else
    	{
    	// Type of message -taken directly from resources as this must be a multimedia message
    	HBufC* msgType = StringLoader::LoadLC( R_MMS_TYPE_TEXT, iCoeEnv );	
    	popCount++;
    	infoData.iType.Set( msgType->Des() );
    	}

////// Priority of message /////////////////////////////////////
    // Editor view and Priority feature supported
    if ( !(entry.IsMobileTerminated()) && iPriorityFeatureSupported )
        {
        HBufC* priority = NULL;
        switch ( iMmsClient.MessagePriority() )
    	    {
            case EMmsPriorityHigh:
        	    {
                priority  = StringLoader::LoadLC( R_MMS_MESSAGE_PRIORITY_HIGH, iCoeEnv );
                popCount++;
                break;
        	    }
    	    case EMmsPriorityLow:
    		    {
	            priority  = StringLoader::LoadLC( R_MMS_MESSAGE_PRIORITY_LOW, iCoeEnv );
	            popCount++;
    		    break;
    		    }
            case EMmsPriorityNormal:
            default:
        	    {
                priority  = StringLoader::LoadLC( R_MMS_MESSAGE_PRIORITY_NORMAL, iCoeEnv );
                popCount++;
                break;
                }
            }
        infoData.iPriority.Set( *priority ); 
        }
    else if ( entry.IsMobileTerminated() )   // Viewer View
        {
        HBufC* priority = NULL;
        switch ( iMmsClient.MessagePriority() )
    	    {
            case EMmsPriorityHigh:
        	    {
                priority  = StringLoader::LoadLC( R_MMS_MESSAGE_PRIORITY_HIGH, iCoeEnv );
                popCount++;
                break;
        	    }
    	    case EMmsPriorityLow:
    		    {
	            priority  = StringLoader::LoadLC( R_MMS_MESSAGE_PRIORITY_LOW, iCoeEnv );
	            popCount++;
    		    break;
    		    }
            case EMmsPriorityNormal:
            default:
        	    {
                priority  = StringLoader::LoadLC( R_MMS_MESSAGE_PRIORITY_NORMAL, iCoeEnv );
                popCount++;
                break;
                }
            }
        infoData.iPriority.Set( *priority );
        }
 
    // Size of message.
    TInt sizeOfMessage = MessageSizeL();

    // Finally make the UI string
    TBuf<KMessageSize> sizeString;
    MessageSizeToStringL( sizeString, sizeOfMessage );
    infoData.iSize.Set( sizeString );

    // To
    const CMsvRecipientList& recipients = iMmsClient.AddresseeList();
    HBufC* toList( NULL );
    HBufC* ccList( NULL );
    HBufC* bccList( NULL );
    TurnRecipientsArrayIntoStringsLC(
                            recipients,
                            toList,
                            ccList,
                            bccList, 
                            addHeadersBitmask,
                            entry.IsMobileTerminated() );
    if ( toList )
        {
        infoData.iTo.Set( *toList );
        popCount++;
        }
    if ( ccList )
        {
        infoData.iCC.Set( *ccList );
        popCount++;
        }
    if ( bccList )
        {
        infoData.iBCC.Set( *bccList );
        popCount++;
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
// CMmsMtmUi::TurnRecipientsArrayIntoStringsLC
//
// ---------------------------------------------------------
// every created recipient buffer object increases pushed object count by one
void CMmsMtmUi::TurnRecipientsArrayIntoStringsLC(
                            const   CMsvRecipientList&  aRecip,
                            HBufC*&                     aTo,
                            HBufC*&                     aCc,
                            HBufC*&                     aBcc, 
                            TInt                        aAddHeadersVariation,
                            TBool                       aMobileTerminated ) const
    {
    TInt count = aRecip.Count();
    if ( count == 0 )
        {
        return;
        }
        
    TMmsMsvEntry entry = static_cast<TMmsMsvEntry>( BaseMtm().Entry().Entry() );
    TBool showNumber = CMmsMtmUi::IsUpload( entry )
        ? EFalse
        : ETrue;
    
    // indeces refers To, Cc, Bcc
    TInt recipTypesIndex( 0 );
    const TInt KRecipTypesCount( 3 );
    
    TMsvRecipientTypeValues recipType ( EMsvRecipientTo );
    
    // needs first round to check what is there
    TInt index;
    TInt stringLengthTo = 0;
    TInt stringLengthCc = 0;
    TInt stringLengthBcc = 0;
    TInt* stringLength = &stringLengthTo;    
    for ( recipTypesIndex = 0; recipTypesIndex < KRecipTypesCount; recipTypesIndex++ )
        {
        if ( recipTypesIndex == 1 )
            {
            stringLength = &stringLengthCc;
            recipType = EMsvRecipientCc;
            }
        else if ( recipTypesIndex == 2 )
            {
            stringLength = &stringLengthBcc;
            recipType = EMsvRecipientBcc;
            }
            
        for ( index = 0; index < count; index++ )
            {
            if ( aRecip.Type( index ) == recipType )
                {
                ( *stringLength ) += TMmsGenUtils::Alias( aRecip[index] ).Length();
                ( *stringLength ) += TMmsGenUtils::PureAddress( aRecip[index] ).Length();
                // Additional space for chars: '<' '>' ',' + 2 x KMmsLRMarker
                ( *stringLength ) += KMmsAdditionalCharsStringLength;
                }
            }
        }

    if (    stringLengthTo
        ||  stringLengthCc
        ||  stringLengthBcc )
        {
        // needs second round to copy strigs:
        // if CC not variated On, append CC recipients to To: field
        // if BCC not variated On but CC variated On, append BCC recipients to Cc: field
        // if BCC not variated On and also CC variated Off, append BCC recipients to To: field    
        HBufC** recipients = NULL;
        recipType = EMsvRecipientTo;
        TInt bufLength( stringLengthTo ); 
        for ( recipTypesIndex = 0; recipTypesIndex < KRecipTypesCount; recipTypesIndex++ )
            {       
            recipients = NULL;
            if ( recipTypesIndex == 0 )
                {
                if ( ! ( aAddHeadersVariation & KMmsFeatureIdEditorCcField ) )
                    {
                    // CC not supported on the UI
                    bufLength += stringLengthCc;
                    if ( ! ( aAddHeadersVariation & KMmsFeatureIdEditorBccField ) )
                        {
                        // BCC not supported on the UI
                        bufLength += stringLengthBcc;
                        }
                    }
                if ( bufLength )
                    {
                    aTo = HBufC::NewLC( bufLength );
                    recipients = &aTo;
                    }
                else
                    {
                    continue;
                    }
                }        
            else if ( recipTypesIndex == 1 )
                {
                recipType = EMsvRecipientCc;
                if ( aAddHeadersVariation & KMmsFeatureIdEditorCcField )
                    {
                    // Cc supported on the UI
                    bufLength = stringLengthCc;
                    if ( ! ( aAddHeadersVariation & KMmsFeatureIdEditorBccField ) )
                        {
                        // BCC not supported on the UI
                        bufLength += stringLengthBcc;
                        }
                    if ( bufLength )
                        {
                        aCc = HBufC::NewLC( bufLength );    
                        recipients = &aCc;
                        }
                    else
                        {
                        continue;
                        }
                    }
                else
                    {
                    // Cc not supported on the UI
                    recipients = &aTo;
                    }
                }
            else if ( recipTypesIndex == 2 )
                {
                if ( aMobileTerminated )
                    {
                    // never create bcc list
                    continue;
                    }
                recipType = EMsvRecipientBcc;
                if ( aAddHeadersVariation & KMmsFeatureIdEditorBccField )
                    {
                    // BCC supported on the UI
                    bufLength = stringLengthBcc;
                    if ( bufLength )
                        {
                        aBcc = HBufC::NewLC( bufLength );    
                        recipients = &aBcc;
                        }
                    else
                        {
                        continue;
                        }
                    }
                else if ( aAddHeadersVariation & KMmsFeatureIdEditorCcField )
                    {
                    // CC supported on the UI
                    recipients = &aCc;
                    }
                else 
                    {
                    // BCC not supported on the UI
                    recipients = &aTo;
                    }
                    
                }
            
            if (    recipients
                &&  *recipients )
                {                
                TPtr listPtr = (*recipients)->Des();
                for ( index=0; index < count; index++ )
                    {
                    if ( aRecip.Type( index ) == recipType )
                        {
                        TPtrC name = TMmsGenUtils::Alias( aRecip[index] );
                        TPtrC number = TMmsGenUtils::PureAddress( aRecip[index] );
                        if ( name.Length() )
                            {
                            listPtr.Append( name );
                            if ( showNumber )
                                {
                                listPtr.Append( KMmsCharSpace );
                                listPtr.Append( KMmsLRMarker );
                                listPtr.Append( KCharLeftAddressIterator );
                                listPtr.Append( number );
                                listPtr.Append( KCharRightAddressIterator );
                                listPtr.Append( KMmsLRMarker );
                                }
                            }
                        else
                            {
                            listPtr.Append( number );
                            }
                        
                        //append always and remove last one in the very end
                        listPtr.Append( KMmsCharCommaAndSpace );
                        }
                    }
                }
            }
        
        // third round to strip the ", " off
        for ( recipTypesIndex = 0; recipTypesIndex < KRecipTypesCount; recipTypesIndex++ )
            {
            recipients = NULL;
            if (    recipTypesIndex == 0 
                &&  aTo )
                {
                recipients = &aTo;
                }   
            else if (    recipTypesIndex == 1 
                    &&  aCc )
                {
                recipients = &aCc;
                }   
            else if (    recipTypesIndex == 2 
                    &&  aBcc )
                {
                recipients = &aBcc;
                }
            if (    recipients
                &&  *recipients )
                {
                TPtr listPtr = (*recipients)->Des();
                if ( listPtr.Length() > KMmsCharCommaAndSpace().Length() )
                    {
                    listPtr.SetLength( listPtr.Length() - KMmsCharCommaAndSpace().Length() );
                    }
                }
            }
        }
    }



// ---------------------------------------------------------
// CMmsMtmUi::MessageSizeToStringL(TDes& aTarget, TInt aFileSize)
// Converts message size in bytes to a kilobyte string. Rounds the bytes up to the
// next full kilo. I.e:
// 0 -> 0KB
// 1 -> 1KB
// 1024 -> 1KB
// 1025 -> 2KB
// ---------------------------------------------------------
//
void CMmsMtmUi::MessageSizeToStringL( TDes& aTarget, TInt aFileSize ) const
    {
    TInt fileSize = aFileSize / KKiloByte;
    if ( aFileSize % KKiloByte )
        {
        fileSize++;
        }

    HBufC* buf = StringLoader::LoadLC( R_MMS_MESSAGE_SIZE_KB, fileSize, iEikonEnv );
    TPtr ptr = buf->Des();
    ptr.Trim(); // remove possible white space from beginning
    aTarget.Append( ptr );

    CleanupStack::PopAndDestroy(); // buf
    }

// ---------------------------------------------------------
// CMmsMtmUi::IsMessageEmptyL()
// Checks if the message is effectively empty or not
// ---------------------------------------------------------
//
TBool CMmsMtmUi::IsMessageEmptyL() const
    {
    TBool empty = EFalse;
    //CMsvEntrySelection* ids = iMmsClient.GetAttachmentsL();
    //CleanupStack::PushL( ids );
    
    CMsvStore* store = iMmsClient.Entry().ReadStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& attaManager = store->AttachmentManagerL();    
    TInt attaCount = attaManager.AttachmentCount();
    CleanupStack::PopAndDestroy( store );

    // Only attachments and subject are calculated
    // in message size.
    //if ( ids->Count() == 0 &&
    if ( attaCount == 0 &&
        iMmsClient.SubjectL().Length() == 0 )
        {
        // No attachments nor subject
        empty = ETrue;
        }
    //CleanupStack::PopAndDestroy(); // ids

    return empty;
    }

// ---------------------------------------------------------
// CMmsMtmUi::MessageSizeL()
// Returns size of message
// ---------------------------------------------------------
//
TInt CMmsMtmUi::MessageSizeL() const
    {
    // Only attachments and subject are calculated
    // in message size.
    TInt size( 0 );

    // get size of attachments without headers    
    CMsvStore* store = iMmsClient.Entry().ReadStoreL();
    CleanupStack::PushL( store );
    MMsvAttachmentManager& attaManager = store->AttachmentManagerL();    
    TInt attaCount = attaManager.AttachmentCount();

    for ( TInt i = 0; i < attaCount; i++ )
        {
        RFile attaFile = attaManager.GetAttachmentFileL( i );
        CleanupClosePushL( attaFile );
        TInt fileSize( 0 );
        
        attaFile.Size( fileSize );         
        size += fileSize;
        
        CleanupStack::PopAndDestroy( &attaFile ); // attachmentInfo, mimeHeaders, attaFile
        }
    CleanupStack::PopAndDestroy( store );

    size += iMmsClient.SubjectL().Length();
    return size;
    }

// ---------------------------------------------------------
// CMmsMtmUi::ApExistsL
// Checks the existence of given access point
// according to id.
// ---------------------------------------------------------
//
TBool CMmsMtmUi::ApExistsL( TInt32 aAp )
    {
    return iSettingsHandler->ApExistsL( aAp );
    }

// ---------------------------------------------------------
// CMmsMtmUi::ConfirmSendL
// Gets user confirmation for message sending ni case where application
// does not have required capabilities
//
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::ConfirmSendL( 
			TRequestStatus& aStatus, 
			const CMsvEntrySelection& /*aSelection*/, 
			const TSecurityInfo& /*aClientInfo*/ ) 

	{
	return CMsvCompletedOperation::NewL(
		Session(), 
		Type(), 
		KNullDesC8, 
		BaseMtm().Entry().OwningService(), 
		aStatus, 
		KErrNone );
	}

// ---------------------------------------------------------
// CMmsMtmUi::OpenPostcardInfoL(TRequestStatus& aCompletionStatus )
// Opens the Postcard Info popup.
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::OpenPostcardInfoL( TRequestStatus& aCompletionStatus )
    {
    LOGTEXT( _L8( "MmsMtmUi OpenPostcardInfo start" ) );
    TInt popCount( 0 );

    // The items for Info are read from the message entry.
    // Note: Most of the stuff is in TMsvEntry, but not all -> MMS entry used all the way.
    TMmsMsvEntry entry = static_cast<TMmsMsvEntry>( BaseMtm().Entry().Entry() );
    __ASSERT_DEBUG( entry.iType==KUidMsvMessageEntry, Panic( EMmsNotAMessage ) );
    __ASSERT_DEBUG( entry.iMtm==Type(), Panic( EMmsWrongMtm ) );

    iMmsClient.LoadMessageL();

    TMsgInfoMessageInfoData infoData;

    // Supply UTC date-time  to message info
    infoData.iDateTime = entry.iDate;
    
    // Type of message -taken directly from resources as this must be a multimedia message
    HBufC* msgType = StringLoader::LoadLC( R_POSTCARD_TYPE_TEXT, iCoeEnv );
    popCount++;
    infoData.iType.Set( msgType->Des() );

    // Size of message.
    // The size of recipient/greeting text is always less than 1024 so lets
    // spare that for them
    TInt sizeOfMessage = 1024;
        if ( !IsMessageEmptyL() )
            { 
            sizeOfMessage = iMmsClient.MessageSize();
            }
    // Finally make the UI string
    TBuf<KMessageSize> sizeString;
    MessageSizeToStringL( sizeString, sizeOfMessage );
    infoData.iSize.Set( sizeString );

    infoData.iTo.Set( entry.iDetails );
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
// CMmsMtmUi::OpenDeliveryPopupL
// Uses CMmsMtmDeliveryDialogLauncher to fetch the delivery data for the current message
// and finally to display it in message query dialog.
//
// ---------------------------------------------------------
//
CMsvOperation* CMmsMtmUi::OpenDeliveryPopupL( TRequestStatus& aCompletionStatus, 
                                           TDes8& /*aParameter */)
    {
    // get the current message
    TMmsMsvEntry entry = static_cast<TMmsMsvEntry>( BaseMtm().Entry().Entry() );
    iMmsClient.LoadMessageL();
   
   // reset for reuse, otherwise causes memoryleak
    if (iLauncher)
        {
        delete iLauncher;
        iLauncher = NULL;
        }
    // give the iMmsClient as parameter, it's data is being used when filtering data from Logs DB    
    iLauncher = CMmsMtmDeliveryDialogLauncher::NewL(entry, &iMmsClient);
    TBool success = iLauncher->StartDialogL();
    if (!success)
        {
        delete iLauncher;
        iLauncher = NULL;
        }
    // if succesfull, deleted in destructor   
    
    return CMsvCompletedOperation::NewL( Session(), 
        KUidMsvLocalServiceMtm, 
        KNullDesC8,
        KMsvLocalServiceIndexEntryId, 
        aCompletionStatus );
        
    }
    

// ---------------------------------------------------------
// CMmsMtmUi::ConvertUtcToLocalTime
//
// ---------------------------------------------------------
//
void CMmsMtmUi::ConvertUtcToLocalTime( TTime& aTime )
    {
    TLocale locale;
	// Add time difference
    aTime += locale.UniversalTimeOffset();
	if (locale.QueryHomeHasDaylightSavingOn())          
		{ // and possible daylight saving time
		TTimeIntervalHours daylightSaving(1);          
		aTime += daylightSaving;
		}
    }
//  End of File
