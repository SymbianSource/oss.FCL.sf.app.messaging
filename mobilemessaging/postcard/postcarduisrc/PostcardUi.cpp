/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides Postcard MTM UI methods.
*
*/



// INCLUDE FILES

#include <apgcli.h>                         // RApaLsSession
#include <MuiuMsvUiServiceUtilities.h>      // DiskSpaceBelow.., MsvUiServiceUtil..
#include <MtmExtendedCapabilities.hrh>      // MtmUiFlags
#include <mmsconst.h>						// KUidMsgTypeMultimedia
#include <mmsclient.h>                      // Client MTM
#include <mmssettings.h> 					// MMS Engine settings
#include <mmssettingsdialog.h>
#include <bautils.h>						// FileExists
#include <data_caging_path_literals.hrh> 
#include <featmgr.h>
#include <bldvariant.hrh>

#include "PostcardMtmConst.h"
#include "PostcardUi.h"
#include "PostcardUiPanic.h"

// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT( KPostcardUiResourceFile,"PostcardUi.rsc");
_LIT( KPostcardDriveZ,"z:"); 

const TInt KZero = 0; //constant for memory check

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ---------------------------------------------------------
// NewPostcardUiL
// Factory function
// ---------------------------------------------------------
//
EXPORT_C CBaseMtmUi* NewPostcardUiL( CBaseMtm& aMtm, CRegisteredMtmDll& aRegisteredDll )
    {
    // Get supported features from feature manager.
    FeatureManager::InitializeLibL();
    if ( !FeatureManager::FeatureSupported( KFeatureIdMmsPostcard ) )
        {
        FeatureManager::UnInitializeLib();
        User::Leave( KErrNotSupported );
        }
    FeatureManager::UnInitializeLib();
    
    return CPostcardUi::NewL( aMtm, aRegisteredDll );
    }

// ---------------------------------------------------------
// Panic
// PostcardUi Panic function
// ---------------------------------------------------------
//
GLDEF_C void Panic( TPostcardUiPanic aPanic )
    {
    User::Panic( _L("PostcardUi"), aPanic );
    }


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPostcardUi::CPostcardUi
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CPostcardUi::CPostcardUi( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll )
    :   CBaseMtmUi( aBaseMtm, aRegisteredMtmDll ), iPocaAppId( KUidPostcardId )
    {
    }

// ---------------------------------------------------------
// CPostcardUi::ConstructL
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CPostcardUi::ConstructL()
    {
    LOGTEXT( _L8( "PostcardUi::ConstructL" ) );
    CBaseMtmUi::ConstructL();
    LOGTEXT( _L8( "PostcardUi::ConstructL Base construct OK" ) );
    iSettingsHandler = CMmsSettingsHandler::NewL( static_cast<CMmsClientMtm&>( iBaseMtm ) );
    LOGTEXT( _L8( "PostcardUi::ConstructL End" ) );
    }

// ---------------------------------------------------------
// CPostcardUi::NewL
// Two-phased constructor.
// ---------------------------------------------------------
// 
CPostcardUi* CPostcardUi::NewL( CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll )
    {
    LOGTEXT( _L8( "PostcardUi::NewL" ) );
    CPostcardUi* self = new ( ELeave ) CPostcardUi( aBaseMtm, aRegisteredMtmDll );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    LOGTEXT( _L8( "PostcardUi::NewL end" ) );
    return self;
    }

    
// ---------------------------------------------------------
// CPostcardUi::~CPostcardUi
// Destructor
// ---------------------------------------------------------
//
CPostcardUi::~CPostcardUi()
    {
    LOGTEXT( _L8( "PostcardUi::~CPostcardUi" ) );
    delete iSettingsHandler;
    }

// ---------------------------------------------------------
// CPostcardUi::CreateL
// Launches MMS editor for creating a new message.
// ---------------------------------------------------------
//
CMsvOperation* CPostcardUi::CreateL( const TMsvEntry& aEntry, 
                                  CMsvEntry& aParent, 
                                  TRequestStatus& aStatus )
    {
    LOGTEXT( _L8( "PostcardUi::CreateL" ) );
    __ASSERT_DEBUG( aEntry.iMtm == Type(), Panic( EPostcardWrongMtmType ) );
    __ASSERT_DEBUG( aEntry.iType == KUidMsvMessageEntry, Panic( EPostcardNotAMessage ) );

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

// ---------------------------------------------------------
// CPostcardUi::CheckSettingsL() const
// Check MMS service settings
// ---------------------------------------------------------
//
TBool CPostcardUi::CheckSettingsL( CMmsSettingsDialog::TMmsExitCode& aExitCode ) const
    {
    // reload the settings
    iSettingsHandler->RefreshSettingsL( );
    return iSettingsHandler->CheckSettingsL( aExitCode );
    }

// ---------------------------------------------------------
// CPostcardUi::OpenL
// Handles request to open existing message server entry (message/service)
// ---------------------------------------------------------
//
CMsvOperation* CPostcardUi::OpenL( TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CPostcardUi::OpenL
// Opens the first entry in a selection
// ---------------------------------------------------------
//
CMsvOperation* CPostcardUi::OpenL( TRequestStatus& /*aStatus*/, 
                                const CMsvEntrySelection& /*aSelection*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CPostcardUi::CloseL
// ---------------------------------------------------------
//
CMsvOperation* CPostcardUi::CloseL( TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CPostcardUi::CloseL
// ---------------------------------------------------------
//
CMsvOperation* CPostcardUi::CloseL( TRequestStatus& /*aStatus*/, 
                                 const CMsvEntrySelection& /*aSelection*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CPostcardUi::EditL
// Opens a message (editor) or service (settings dialog) for editing.
// ---------------------------------------------------------
//
CMsvOperation* CPostcardUi::EditL( TRequestStatus& aStatus )
    {
    LOGTEXT(_L8("PostcardUi::EditL"));
    TMsvEntry entry = iBaseMtm.Entry().Entry();
    __ASSERT_DEBUG( entry.iMtm == Type(), Panic( EPostcardWrongMtmType ) );
    __ASSERT_DEBUG( entry.iType == KUidMsvMessageEntry, Panic( EPostcardNotAMessage ) );
    CMsvOperation* msvoperation = NULL;

    switch ( entry.iType.iUid )
        {
        case KUidMsvMessageEntryValue:
            {
            LOGTEXT(_L8("PostcardUi::EditL dealing with message"));

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
            tEntry.iBioType = KUidMsgSubTypeMmsPostcard.iUid;
            cEntry->ChangeL( tEntry );
            CleanupStack::PopAndDestroy( cEntry );

            LOGTEXT( _L8( "PostcardUi::EditL launching editor" ) );
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

    LOGTEXT(_L8("PostcardUi::EditL end"));
    return msvoperation;    
    }

// ---------------------------------------------------------
// CPostcardUi::EditL
// ---------------------------------------------------------
//
CMsvOperation* CPostcardUi::EditL( TRequestStatus& aStatus, 
                                const CMsvEntrySelection& aSelection )
    {
    BaseMtm().SwitchCurrentEntryL( aSelection[0] );
    return EditL( aStatus );
    }

// ---------------------------------------------------------
// CPostcardUi::ViewL
// Opens the message for reading in Viewer.
// ---------------------------------------------------------
//
CMsvOperation* CPostcardUi::ViewL( TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CPostcardUi::ViewL
// ---------------------------------------------------------
//
CMsvOperation* CPostcardUi::ViewL( TRequestStatus& /*aStatus*/, 
                                const CMsvEntrySelection& /*aSelection*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// -----------------------------------------------------------------------------
// CPostcardUi::LaunchEditorApplicationL()
// Does the actual opening.
// -----------------------------------------------------------------------------
//
CMsvOperation* CPostcardUi::LaunchEditorApplicationL( TRequestStatus& aStatus, 
                                                   CMsvSession& aSession, 
                                                   TEditorType aEditorType )
    {
    LOGTEXT(_L8("PostcardUi::LaunchEditorApplicationL"));

    TEditorParameters temp;    
    temp.iSpecialAppId = iPocaAppId;    // We want to use the special Poca application

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
    
    LOGTEXT(_L8("PostcardUi::LaunchEditorApplicationL just before calling EditorLauncher"));
    return MsgEditorLauncher::LaunchEditorApplicationL( aSession, KUidMsgTypeMultimedia, aStatus, temp, _L(""), paramPack );
    }

// ---------------------------------------------------------
// CPostcardUi::ForwardL
// ---------------------------------------------------------
//
CMsvOperation* CPostcardUi::ForwardL( TMsvId /*aDestination*/, 
                                   TMsvPartList /*aPartList*/, 
                                   TRequestStatus& /*aCompletionStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CPostcardUi::ReplyL
// ---------------------------------------------------------
//
CMsvOperation* CPostcardUi::ReplyL( TMsvId /*aDestination*/, 
                                 TMsvPartList /*aPartList*/, 
                                 TRequestStatus& /*aCompletionStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CPostcardUi::CancelL
// Suspends sending of the selected messages (if possible)
// ---------------------------------------------------------
//
CMsvOperation* CPostcardUi::CancelL( TRequestStatus& /*aStatus*/, 
                                  const CMsvEntrySelection& /*aSelection*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }


// ---------------------------------------------------------
// CPostcardUi::GetResourceFileName
// ---------------------------------------------------------
//
void CPostcardUi::GetResourceFileName( TFileName& aFileName ) const
    {
     TFileName tmpName;
          
     // Append the Resource Files Directory
     tmpName.Append( KDC_MTM_RESOURCE_DIR );     
     // Append the Ressource File Name
     tmpName.Append( KPostcardUiResourceFile );
     
     // Obtain the drive where the DLL is installed
     TFileName dllDrive;
     Dll::FileName( dllDrive );
      
     // Obtain the Complete path for the Resource File
     TParse parse;
     parse.Set( dllDrive, NULL, NULL );
     parse.Set( parse.Drive(), &tmpName, NULL );
     TFileName fileName;
     fileName.Append( parse.FullName());
     
     // Get language of resource file
     BaflUtils::NearestLanguageFile( Session().FileSession( ), fileName );

     if( !BaflUtils::FileExists( Session().FileSession( ), fileName) )
         { // If it's not on dll drive -> try Z
         fileName.Replace( 0, 2, KPostcardDriveZ );
         BaflUtils::NearestLanguageFile( Session().FileSession( ), fileName );
         }

     aFileName = fileName;
    
     return;
    }

// ---------------------------------------------------------
// CMsvOperation* CPostcardUi::OpenServiceSettingsDialogL(TRequestStatus& aCompletionStatus)
// Handles MMS service settings
// ---------------------------------------------------------
//
CMsvOperation* CPostcardUi::OpenServiceSettingsDialogL( TRequestStatus& aCompletionStatus )
    {
    return iSettingsHandler->OpenServiceSettingsDialogL( aCompletionStatus );
    }

// ---------------------------------------------------------
// CMsvOperation* CPostcardUi::CompleteSettingsOperationL(
//    TRequestStatus& aCompletionStatus, const CMmsSettingsDialog::TMmsExitCode& aExitCode)
// Creates completed operation after settings dialog closing with appropriate parameters.
// ---------------------------------------------------------
//
CMsvOperation* CPostcardUi::CompleteSettingsOperationL(
    TRequestStatus& aCompletionStatus,
    const CMmsSettingsDialog::TMmsExitCode& aExitCode /* = CMmsSettingsDialog::EMmsExternalInterrupt */ )
    {
    return iSettingsHandler->CompleteSettingsOperationL( 
                                aCompletionStatus,
                                aExitCode );
    }

// ---------------------------------------------------------
// TInt CPostcardUi::LaunchSettingsDialogL(
//      CMmsSettingsDialog::TMmsExitCode& aExitCode )
// Opens either MMS Service Settings or MMS Sending Settings dialog.
// ---------------------------------------------------------
//
void CPostcardUi::LaunchSettingsDialogL(
    CMmsSettingsDialog::TMmsExitCode& aExitCode ) const
    {
    iSettingsHandler->LaunchSettingsDialogL( aExitCode );
    }

// ---------------------------------------------------------
// CPostcardUi::ApExistsL
// ---------------------------------------------------------
//
TBool CPostcardUi::ApExistsL( TUint32 aAp )
    {
    return iSettingsHandler->ApExistsL( aAp );
    }

// ---------------------------------------------------------
// CPostcardUi::CheckDiskSpaceL
// ---------------------------------------------------------
//
void CPostcardUi::CheckDiskSpaceL( )
    {
    if( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL(
        Session( ),
        KZero ) )
        {
        User::Leave( KErrDiskFull );
        }
    }

//  End of File
