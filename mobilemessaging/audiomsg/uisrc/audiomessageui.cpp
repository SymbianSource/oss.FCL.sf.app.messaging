/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides AudioMessage MTM UI methods. 
*
*/




#include <apgcli.h> 
#include <MuiuMsvUiServiceUtilities.h>
#include <MtmExtendedCapabilities.hrh>
#include <mmsconst.h>
#include <mmsclient.h>
#include <mmssettings.h>
#include <mmssettingsdialog.h>
#include <featmgr.h>

#include "AudioMessageLogging.h"
#include "audiomessageui.h"
#include "audiomessageuipanic.h"

#define iMmsClient  ( static_cast<CMmsClientMtm&>( iBaseMtm ) )

_LIT( KAMSUiResourceFile,"AudioMessageMtm"); 
_LIT( KAMSEmpty,"" );
_LIT( KAmsUiMtmPanicText,"AMSUi" );
const TInt KZero = 0; //constant for memory check

const TUid KUidAMSId = {0x1020745A}; 

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewAudioMessageUiL
// Factory function
// ---------------------------------------------------------------------------
//
EXPORT_C CBaseMtmUi* NewAudioMessageUiL( CBaseMtm& aMtm, CRegisteredMtmDll& aRegisteredDll )
    {
    FeatureManager::InitializeLibL();
    if ( !FeatureManager::FeatureSupported( KFeatureIdAudioMessaging ) )
        {
        FeatureManager::UnInitializeLib();
        User::Leave( KErrNotSupported );
        }
    FeatureManager::UnInitializeLib();
    
    return CAudioMessageUi::NewL( aMtm, aRegisteredDll );
    }

// ---------------------------------------------------------------------------
// Panic
// CAudioMessageUi Panic function
// ---------------------------------------------------------------------------
//
GLDEF_C void Panic( TAMSUiPanic aPanic )
    {
    User::Panic( KAmsUiMtmPanicText, aPanic );
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CAudioMessageUi::CAudioMessageUi( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll )
    :   CBaseMtmUi( aBaseMtm, aRegisteredMtmDll ), iAmsAppId( KUidAMSId )
    {
    }

void CAudioMessageUi::ConstructL()
    {
    AMSLOGGER_WRITE( "CAudioMessageUi::ConstructL" );
    CBaseMtmUi::ConstructL();
    AMSLOGGER_WRITE( "CAudioMessageUi::ConstructL Base construct OK" ) ;
  	iSettingsHandler = CMmsSettingsHandler::NewL( iMmsClient );
    AMSLOGGER_WRITE( "CAudioMessageUi::ConstructL End" );
    }

CAudioMessageUi* CAudioMessageUi::NewL( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll )
    {
    AMSLOGGER_WRITE( "CAudioMessageUi::NewL" );
    CAudioMessageUi* self = new ( ELeave ) CAudioMessageUi( aBaseMtm, aRegisteredMtmDll );
  
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    AMSLOGGER_WRITE( "CAudioMessageUi::NewL end" );
    return self;
    }

    
CAudioMessageUi::~CAudioMessageUi()
    {
    AMSLOGGER_WRITE( "AMSUi::~CAudioMessageUi" );
   	delete iSettingsHandler;
    }

// ---------------------------------------------------------------------------
// Launches Ams for creating a new message.
// ---------------------------------------------------------------------------
//
CMsvOperation* CAudioMessageUi::CreateL( const TMsvEntry& aEntry, 
                                  CMsvEntry& aParent, 
                                  TRequestStatus& aStatus )
    {
    AMSLOGGER_WRITE( "CAudioMessageUi::CreateL" );
    
    __ASSERT_DEBUG( aEntry.iMtm == Type(), Panic( EAMSWrongMtmType ) );
    __ASSERT_DEBUG( aEntry.iType == KUidMsvMessageEntry, Panic( EAMSNotAMessage ) );

    CheckDiskSpaceL( );

    CMmsSettingsDialog::TMmsExitCode exitCode =
        CMmsSettingsDialog::EMmsExternalInterrupt;

    if ( !CheckSettingsL( exitCode ) )
        {
        TUint32 ap = iSettingsHandler->MmsSettings()->AccessPoint( 0 );
        if( exitCode==CMmsSettingsDialog::EMmsBack && ApExistsL( ap ) )
            { //settings saved
            return LaunchEditorApplicationL( aStatus, aParent.Session(), ECreateNewMessage );
            }
        else
            { //user selects exit and settings not saved
            return CompleteSettingsOperationL( aStatus, exitCode );
            }
        }
    else
        {
        // OK - open the editor
        return LaunchEditorApplicationL( aStatus, aParent.Session(), ECreateNewMessage );
        }
    }

TBool CAudioMessageUi::CheckSettingsL( CMmsSettingsDialog::TMmsExitCode& aExitCode ) const
    {
    // reload the settings
    iSettingsHandler->RefreshSettingsL( );
    return iSettingsHandler->CheckSettingsL( aExitCode );
    }

// ---------------------------------------------------------------------------
// Handles request to open existing message server entry (message/service)
// ---------------------------------------------------------------------------
//
CMsvOperation* CAudioMessageUi::OpenL( TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------------------------
// Opens the first entry in a selection
// ---------------------------------------------------------------------------
//
CMsvOperation* CAudioMessageUi::OpenL( TRequestStatus& /*aStatus*/, 
                                const CMsvEntrySelection& /*aSelection*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

CMsvOperation* CAudioMessageUi::CloseL( TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

CMsvOperation* CAudioMessageUi::CloseL( TRequestStatus& /*aStatus*/, 
                                 const CMsvEntrySelection& /*aSelection*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------------------------
// Opens a message (editor) or service (settings dialog) for editing.
// ---------------------------------------------------------------------------
//
CMsvOperation* CAudioMessageUi::EditL( TRequestStatus& aStatus )
    {
    AMSLOGGER_WRITE( "CAudioMessageUi::EditL" );
    
    TMsvEntry entry = iBaseMtm.Entry().Entry();
    __ASSERT_DEBUG( entry.iMtm == Type(), Panic( EAMSWrongMtmType ) );
    __ASSERT_DEBUG( entry.iType == KUidMsvMessageEntry, Panic( EAMSNotAMessage ) );
    CMsvOperation* msvoperation = NULL;

    switch ( entry.iType.iUid )
        {
        case KUidMsvMessageEntryValue:
            {
            AMSLOGGER_WRITE( "CAudioMessageUi::EditL dealing with message" );

            CheckDiskSpaceL( );

            CMmsSettingsDialog::TMmsExitCode exitCode = 
                CMmsSettingsDialog::EMmsExternalInterrupt;

            if ( !CheckSettingsL( exitCode ) ) 
                {
		        TUint32 ap = iSettingsHandler->MmsSettings()->AccessPoint( 0 );
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

            CMsvEntry* cEntry = Session().GetEntryL( iBaseMtm.Entry().Entry().Id() );
            CleanupStack::PushL( cEntry );

            TMsvEntry tEntry = cEntry->Entry();
            tEntry.iMtm = KUidMsgTypeMultimedia;
            tEntry.iBioType = KUidMsgSubTypeMmsAudioMsg.iUid;

            cEntry->ChangeL( tEntry );
            CleanupStack::PopAndDestroy( cEntry );

            AMSLOGGER_WRITE( "CAudioMessageUi::EditL launching editor" );
            msvoperation = LaunchEditorApplicationL( aStatus, iBaseMtm.Entry().Session() );
            }
            break;

        case KUidMsvServiceEntryValue:
        case KUidMsvAttachmentEntryValue:   // flow through
        case KUidMsvFolderEntryValue:       // flow through
        default:
            User::Leave( KErrNotSupported );
            break;
        }

    AMSLOGGER_WRITE( "AMSUi::EditL end" );
    return msvoperation;    
    }

CMsvOperation* CAudioMessageUi::EditL( TRequestStatus& aStatus, 
                                const CMsvEntrySelection& aSelection )
    {
    BaseMtm().SwitchCurrentEntryL( aSelection[0] );
    return EditL( aStatus );
    }

// ---------------------------------------------------------------------------
// Opens the message for reading in Viewer.
// ---------------------------------------------------------------------------
//
CMsvOperation* CAudioMessageUi::ViewL( TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

CMsvOperation* CAudioMessageUi::ViewL( TRequestStatus& /*aStatus*/, 
                                const CMsvEntrySelection& /*aSelection*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------------------------
// Does the actual opening.
// ---------------------------------------------------------------------------
//
CMsvOperation* CAudioMessageUi::LaunchEditorApplicationL( TRequestStatus& aStatus, 
                                                   CMsvSession& aSession, 
                                                   TEditorType aEditorType )
    {
    AMSLOGGER_WRITE( "AMSUi::LaunchEditorApplicationL" );
   
    TEditorParameters temp; 
     
    temp.iSpecialAppId =iAmsAppId;
    if ( iBaseMtm.HasContext() )
        {
        temp.iId = iBaseMtm.Entry().EntryId();
        }

    switch( aEditorType )
        {
        case ECreateNewMessage:
            temp.iFlags|=EMsgCreateNewMessage ;
            temp.iId=KMsvDraftEntryIdValue;
            break;
        case EEditExisting: // No need for action
        case EReadOnly: // Never happens
        default:
            break;
        };

    temp.iFlags &= ~( EMtmUiFlagEditorPreferEmbedded|EMtmUiFlagEditorNoWaitForExit );

    temp.iFlags |= ( Preferences() & EMtmUiFlagEditorPreferEmbedded?EMsgLaunchEditorEmbedded:0 );

    TPckgC<TEditorParameters> paramPack( temp );
    
    AMSLOGGER_WRITE( "CAudioMessageUi::LaunchEditorApplicationL just before calling EditorLauncher" );  
    return MsgEditorLauncher::LaunchEditorApplicationL( aSession, 
    	KUidMsgTypeMultimedia, aStatus, temp, KAMSEmpty, paramPack );
    }

CMsvOperation* CAudioMessageUi::ForwardL( TMsvId /*aDestination*/, 
                                   TMsvPartList /*aPartList*/, 
                                   TRequestStatus& /*aCompletionStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

CMsvOperation* CAudioMessageUi::ReplyL( TMsvId /*aDestination*/, 
                                 TMsvPartList /*aPartList*/, 
                                 TRequestStatus& /*aCompletionStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------------------------
// Suspends sending of the selected messages (if possible)
// ---------------------------------------------------------------------------
//
CMsvOperation* CAudioMessageUi::CancelL( TRequestStatus& /*aStatus*/, 
                                  const CMsvEntrySelection& /*aSelection*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

void CAudioMessageUi::GetResourceFileName( TFileName& aFileName ) const
    {
    aFileName = KAMSUiResourceFile;
    }

// ---------------------------------------------------------------------------
// Handles MMS service settings
// ---------------------------------------------------------------------------
//
CMsvOperation* CAudioMessageUi::OpenServiceSettingsDialogL( TRequestStatus& 
		aCompletionStatus )
    {
    return iSettingsHandler->OpenServiceSettingsDialogL( aCompletionStatus );
    }

// ---------------------------------------------------------------------------
// Creates completed operation after settings dialog closing with appropriate 
// parameters.
// ---------------------------------------------------------------------------
//
CMsvOperation* CAudioMessageUi::CompleteSettingsOperationL(
    TRequestStatus& aCompletionStatus,
    const CMmsSettingsDialog::TMmsExitCode& aExitCode )
    {
    return iSettingsHandler->CompleteSettingsOperationL( 
                                aCompletionStatus,
                                aExitCode );
    }

// ---------------------------------------------------------------------------
// Opens either MMS Service Settings or MMS Sending Settings dialog.
// ---------------------------------------------------------------------------
//
void CAudioMessageUi::LaunchSettingsDialogL(
    CMmsSettingsDialog::TMmsExitCode& aExitCode ) const
    {
    iSettingsHandler->LaunchSettingsDialogL( aExitCode );
    }

// ---------------------------------------------------------------------------
// CAudioMessageUi::ApExistsL
// ---------------------------------------------------------------------------
//
TBool CAudioMessageUi::ApExistsL( TUint32 aAp )
    {
  	return iSettingsHandler->ApExistsL( aAp );
    }

void CAudioMessageUi::CheckDiskSpaceL( )
    {
    if( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL(
        Session( ),
        KZero ) )
        {
        User::Leave( KErrDiskFull );
        }
    }

