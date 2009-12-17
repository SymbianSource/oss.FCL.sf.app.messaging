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
* Description:  PostcardAppUi implementation
*
*/



// ========== INCLUDE FILES ================================

#include <e32std.h>
#include <w32std.h>

#include <eikapp.h>                 // CEikApplication
#include <eikdialg.h>               // CEikDialog
#include <eikenv.h>                 // Eikon Enviroment
#include <eikmenub.h>               // CEikMenuBar
#include <eikmenup.h>               // CEikMenuPane
#include <bamdesca.h>               // MDesCArray
#include <bautils.h>                // NearestLanguageFile

#include <bldvariant.hrh>
#include <centralrepository.h>
#include <data_caging_path_literals.hrh>
#include <ErrorUI.h>
#include <featmgr.h>
#include <StringLoader.h>

#include <aknenv.h>
#include <aknconsts.h>
#include <AknGlobalMsgQuery.h>      // Global Msg Query
#include <AknIndicatorContainer.h>
#include <akninputblock.h>          // CAknInputBlock
#include <aknnavi.h>                // CAknNavigationControlContainer
#include <aknnotewrappers.h>
#include <akntitle.h>
#include <AknWaitDialog.h>

#include <avkon.hrh>
#include <avkon.mbg>

#include <AknsConstants.h>
#include <AknsUtils.h>

#include <fbs.h>
#include <gulicon.h>
#include <txtetext.h>

#include <hlplch.h>                 // For HlpLauncher 
#include <csxhelp/postcard.hlp.hrh>

#include <cntfield.h>
#include <cntfldst.h>
#include <cntitem.h>				// for ContactItem

#include <mtmdef.h>                 // TMsvPartList
#include <mtmuibas.h>               // MTM UI
#include <msvapi.h>
#include <msvstd.hrh>               // KUidMsvMessageEntryValue

#include <cmsvattachment.h>
#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>

#include <msgoperationwait.h>           // for CMsgOperationWait
#include <MtmExtendedCapabilities.hrh>  // for KMtmUiFunctionMessageInfo
#include <MuiuMsvUiServiceUtilities.h>  // for Diskspace check
#include <muiumsvuiserviceutilitiesinternal.h>  // for Offline mode check

#include <MsgEditorAppUi.rsg>
#include <MsgEditorAppUiExtension.h>// for iMsgEditorAppUiExtension (zoom)
#include <MsgEditorCommon.h>


#include <mmsclient.h>
#include <mmsmsventry.h>

#include <messaginginternalcrkeys.h>
#include <messagingvariant.hrh>     // postcard feature (variation) bits
#include <MsgMediaResolver.h>
#include <MsgMedia.hrh>             // TMmsMediaType
#include <MsgImageInfo.h>
#include <MsgMimeTypes.h>

#include <Postcard.rsg>             // resource identifiers
#include <postcard.mbg>             // icons
#include "Postcard.hrh"             // for EPostcardCmds
#include "PostcardAddressForm.h"    // for editing recipient address
#include "PostcardAppUi.h"          // class header
#include "PostcardCenRep.h"
#include "PostcardController.h"     // Controller class
#include "PostcardLaf.h"
#include "PostcardOperationInsertImage.h" // for inserting images
#include "PostcardOperationOpen.h"	// for opening the application
#include "PostcardOperationSave.h"	// for saving or sending the postcard
#include "PostcardPointerObserver.h"// MPocaPointerEventObserver
#include "PostcardPrivateCRKeys.h"  // cenrep keys
#include "PostcardQueryWaiter.h"    // for PostcardQueryWaiter
#include "PostcardTextDialog.h"		// for editing greeting text
#include "PostcardUtils.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

const TInt KPostcardMaxSendConfirmationLength = 320;
const TInt KPostcardMaxMiniatureCharsPerLine = 100;
const TInt KPostcardLegacyWidth = 208; // the width of legacy display
const TInt KPostcardQvgaWidth = 320; // the width of legacy display
const TInt KPostcardMaxAddressFocusLines = 5; // max number of address focus lines

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CPostcardAppUi::CPostcardAppUi
// ---------------------------------------------------------
CPostcardAppUi::CPostcardAppUi()
    {
    }

// ---------------------------------------------------------
// CPostcardAppUi::ConstructL
// ---------------------------------------------------------
void CPostcardAppUi::ConstructL()
    {
    CMsgEditorAppUi::ConstructL();
    
    // We don't have any app ui if launched from idle
    if ( iEikonEnv->EikAppUi() )
        {
        iAbsorber = CAknInputBlock::NewLC();
        CleanupStack::Pop( iAbsorber );
        }

    // Create central repository interface
    iCenRep = CPostcardCenRep::NewL();

    // initialize feature manager to check supported features
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdCamera ) )
        {
        iPocaFlags |= EPostcardCamcorder;
        }
    if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        iPocaFlags |= EPostcardHelp;
        }
    if( FeatureManager::FeatureSupported( KFeatureIdOfflineMode ) )
        {
        iPocaFlags |= EPostcardFeatureOffline;
        }
    FeatureManager::UnInitializeLib( );

    // In case the postcard application is installed somewhere else than "Z" drive
    // we find out the right drive for postcard resource file
    TParse parseDrive;
    parseDrive.Set( Application( )->AppFullName( ), NULL, NULL );
    TPtrC drive = parseDrive.Drive( );

    TParse parse;
    parse.Set( KPostcardResourceFile, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName fileName( parse.FullName() );
    fileName.Insert( 0, drive );
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
    iResourceFile = iEikonEnv->AddResourceFileL( fileName );
    parse.Set( KMsgEditorAppUiResourceFile, &KDC_RESOURCE_FILES_DIR, NULL );
    fileName.Zero( );
    fileName.Append( parse.FullName() );
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
	iBaseResourceFile = iEikonEnv->AddResourceFileL( fileName );
	
    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    
    iTitlePane = static_cast<CAknTitlePane*>
        ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    
	if( !iEikonEnv->StartedAsServerApp( ) )
		{
	    Document().PrepareToLaunchL( this );
		}

    iIdle = CPeriodic::NewL( EPriorityNormal - 1 );
    }

// ---------------------------------------------------------
// CPostcardAppUi::~CPostcardAppUi
// ---------------------------------------------------------
CPostcardAppUi::~CPostcardAppUi()
    {
    // remove resource file from CONE-maintained resource file list.
    if ( iResourceFile )
        {
        iEikonEnv->DeleteResourceFile( iResourceFile );
        }
    if ( iBaseResourceFile )
        {
        iEikonEnv->DeleteResourceFile( iBaseResourceFile );
        }

    delete iController;

	delete iGreeting;
	delete iContact;

	delete iImageInfo;
	
	delete iIdle;

    delete iAppIcon;
    
	if( iMessageQuery )
		{
        iMessageQuery->CancelMsgQuery();
        delete iMessageQuery;
        iMessageQuery = NULL;		
		}

    delete iSendText;

    delete iCenRep;
    }

// ---------------------------------------------------------
// LaunchViewL
// ---------------------------------------------------------
void CPostcardAppUi::LaunchViewL()
    {
    // First show wait note and start the launch process
    ShowWaitNoteL( R_POSTCARD_WAIT_OPENING, EFalse );

    const TMsvEntry& entry = Document( ).Entry( );
    if ( entry.iType.iUid != KUidMsvMessageEntryValue )
        {
        User::Leave( KErrGeneral );
        }

    TRAPD( error, Document( ).Mtm( ).LoadMessageL( ) );
    if ( error )
        {
        User::Leave( error );
        }
    Document( ).SetMessageType( );

  	if ( !entry.ReadOnly() ) // editor
    	{
   	   	// End key should not close application in editor
	   	CMsgEditorAppUi::SetCloseWithEndKey( EFalse );
    	}

    if( entry.ReadOnly() && !iNaviDecorator )
        {
        CreateViewerNaviPaneL( Document().Entry().iDate, EMsgEditorMsgPriorityNormal, ETrue );    
        iNaviPane->PushL( *iNaviDecorator );  // <- This has nothing to do with cleanup-stack!    
        }

    delete iAbsorber;
    iAbsorber = NULL;

    iController = CPostcardController::NewL( Document()
#ifdef RD_SCALABLE_UI_V2
        , *this
#endif
        );

    CreateAndSetTitleIconL();
    
    RefreshFocusLinesL( );

    iOperation = CPostcardOperationOpen::NewL(
        *this,
        Document(),
        *this,
        FsSession() );
    iOperation->Start( EFalse );
    }


// ---------------------------------------------------------
// CanSaveMessageL
// ---------------------------------------------------------
TBool CPostcardAppUi::CanSaveMessageL( TInt aAmount )
    {
    return !( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL( 
        Document().Session(), 
        aAmount ) );
    }

// ---------------------------------------------------------
// DoMsgSaveExitL - called from options menu
// ---------------------------------------------------------
void CPostcardAppUi::DoMsgSaveExitL()
    {
    if( iMsgType == EPostcardSent )
        { // Msg is a sent one so just close the application
        Exit();
        return;
        }
        
    if ( CAknEnv::AppWithShutterRunning() )
        {
        iPocaFlags |= EPostcardRunAppShutterAtExit;
        }

    if ( IsMessageEmpty() ) 
        {
        if ( Document().Mtm().Entry().Entry().Visible() )
            {
            ShowInformationNoteL( R_POSTCARD_MESSAGE_DELETED, ETrue );
            }
        DeleteCurrentEntryL();
        }
    else if ( Document( ).Modified( ) && CanSaveMessageL() )
        {
        if ( IsForeground() )
            {
            TInt resId = R_POSTCARD_SAVING_NEW;
            if( iMsgType == EPostcardDraft )
                { // We are saving an old postcard
                resId = R_POSTCARD_SAVING;
                }

            iPocaFlags |= EPostcardClosing;
            DoSaveL();
            ShowWaitNoteL( resId, EFalse );
            }
        else
            {
            TRAP_IGNORE( DoSaveL() );
            }
        BeginActiveWait();
        if ( iPocaFlags & EPostcardRunAppShutterAtExit )
            {
            RunAppShutter();
            }
        }
    }

// ---------------------------------------------------------
// CompleteLaunchViewL
// ---------------------------------------------------------
void CPostcardAppUi::CompleteLaunchViewL( )
    {
    iController->ActivateL( );
	RemoveWaitNote( );
	
	iMsgType = Document( ).MessageType( );
    if( iMsgType == EPostcardSent )
        {
	    iController->SetFocused( EPostcardImage );
        }
    else
        {
	    iController->SetFocused( EPostcardRecipient );
            
        // If postcard editor is opened then disable the dialer 
        iAvkonAppUi->SetKeyEventFlags( 
              CAknAppUiBase::EDisableSendKeyShort | 
              CAknAppUiBase::EDisableSendKeyLong );	
        }
    SetMiddleSoftkeyL(); // set msk for this view
    if( iMsgType == EPostcardSendAs )
        {
        CheckLengthsL(  );
        }
    else if( iMsgType == EPostcardForward )
        {
        DoEditAddressL( );
        }
    iController->DrawDeferred( );
    iPocaFlags |= EPostcardLaunchSuccessful;
    }

// ---------------------------------------------------------
// CPostcardAppUi::HandleKeyEventL
// ---------------------------------------------------------
TKeyResponse CPostcardAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    
    if ( !Document( ).IsLaunched( ) )
        { // still launching
        return EKeyWasConsumed;
        }

	if( aType != EEventKey )
		{
		return EKeyWasNotConsumed;
		}

    switch ( aKeyEvent.iCode )
        {
        case EKeyOK:
        case EKeyEnter:         //Enter Key
            {
            // Selection key: Show context menus
            DoSelectionKeyL();
            return EKeyWasConsumed;
            }

        case EKeyYes:
            {
            // In view mode there's no function in Yes key
            if( iMsgType != EPostcardSent )
                {
                if( !ContactHasData( *iContact ) )
                    { // If there's no contact data -> open edit address dialog
                    DoEditAddressL( );
                    }
                else
                    { // Else start sending process
                    DoSendL( );                
                    }
                }
            return EKeyWasConsumed;
            }
        case EKeyBackspace:
        case EKeyDelete:
            {
            // Either delete image or the whole postcard entry
            DoBackspaceL( );
            return EKeyWasConsumed;
            }
        default:
            break;
        }
    switch ( aKeyEvent.iScanCode )
        {
        case EStdKeyLeftArrow:  // FALLTHROUGH
        case EStdKeyRightArrow:
            if( iMsgType == EPostcardSent && iController->Focused( ) == EPostcardImage )
                {
                return NavigateBetweenMessagesL(
                    aKeyEvent.iScanCode == EStdKeyRightArrow ?
                    ETrue : EFalse );
                }
		    //lint -fallthrough
        case EStdKeyUpArrow:    // FALLTHROUGH
        case EStdKeyDownArrow:  // FALLTHROUGH
            {
            if( !( iPocaFlags & EPostcardGreetingActive ) && 
            	!( iPocaFlags & EPostcardRecipientActive ) )
            	{
	            iController->Move( aKeyEvent.iScanCode );
	            SetMiddleSoftkeyL(); // set msk for this view
	            
                if ( iMsgType == EPostcardSent )
                    {
                    // In viewer
                    if ( iController->Focused() != EPostcardImage )
                        {
                        // Viewer does not have navigation arrows in text side
                        iNaviPane->Pop( iNaviDecorator );
                        }
                    else
                        {
                        iNaviPane->PushL( *iNaviDecorator );
                        }
                    }
            	}
                
            return EKeyWasConsumed;
            }
        default:
        	{
	        if( iMsgType != EPostcardSent && iController->Focused( ) == EPostcardText )
	            {
	            if( !( iPocaFlags & EPostcardGreetingActive ) )
	            	{
	            	if( aKeyEvent.iCode >= 35 && aKeyEvent.iCode <= 255 )
	            	    {
    		            DoEditGreetingL( aKeyEvent );            		            	    
	            	    }
	            	}
	            }
	        else if( iMsgType != EPostcardSent && iController->Focused( ) == EPostcardRecipient )
	            {
	            if( !( iPocaFlags & EPostcardRecipientActive ) )
	            	{
	            	if( aKeyEvent.iCode >= 35 && aKeyEvent.iCode <= 255 )
	            	    {
    		            DoEditAddressL( aKeyEvent );
	            	    }
	            	}
	            }
            break;
        	}
        }
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------
// HandleCommandL
// ---------------------------------------------------------
void CPostcardAppUi::HandleCommandL( TInt aCommand )
    {
    if ( aCommand != EEikCmdExit &&
         !Document().IsLaunched() )
        {
        return;
        }
    switch (aCommand)
        {
        case EPostcardCmdSend:
            DoSendL( );
            break;
        case EPostcardCmdEditAddress:
            DoEditAddressL( );
            break;
        case EPostcardCmdEditGreeting:
            DoEditGreetingL( );
            break;
        case EPostcardCmdInsertFromGallery:
            DoInsertImageL( MsgAttachmentUtils::EImage );
            break;
        case EPostcardCmdInsertFromCamera:
            DoInsertImageL( MsgAttachmentUtils::ENewImage );
            break;
        case EPostcardCmdReplaceFromGallery:
            DoInsertImageL( MsgAttachmentUtils::EImage );
            break;
        case EPostcardCmdReplaceFromCamera:
            DoInsertImageL( MsgAttachmentUtils::ENewImage );
            break;
        case EPostcardCmdDelete:
            DoBackspaceL( );
            break;
        case EPostcardCmdMessageInfo:
            DoUpdateEntryAndShowMessageInfoL( );
            break;
        case EPostcardCmdForward:
            DoForwardMessageL( );
            break;
        case EAknSoftkeyClose:
        	DoBackSaveL( );
        	break;
        case EAknSoftkeyEdit: // falltrough
        case EAknSoftkeyOpen:
        	DoSelectionKeyL( );
        	break;
        case EAknCmdHelp:
            {            
            if( iMsgType == EPostcardSent )
                {
                LaunchHelpL( KMMS_POSTCARD_HLP_MAIN_VIEW() );
                }
            else
                {
                LaunchHelpL( KMMS_POSTCARD_HLP_MAIN_EDIT() );                
                }
            break;
            }
        case EAknCmdExit:
        case EEikCmdExit:
            RemoveWaitNote();
            ExitAndSaveL();
            break;
        default:
            break;
        }
    return;
    }

// ---------------------------------------------------------
// DynInitMenuPaneL
// ---------------------------------------------------------
void CPostcardAppUi::DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane )
    {
    switch ( aMenuId )
        {
        case R_POSTCARD_MAIN_OPTIONS:
            DynInitMainMenuL( aMenuPane );
            break;

        case R_POSTCARD_INSERT_SUBMENU:
        case R_POSTCARD_REPLACE_SUBMENU:
            DynInitInsertSubMenuL( aMenuPane );
            break;

        case R_POSTCARD_CONTEXT_OPTIONS:
            DynInitContextMenuL( aMenuPane );
            break;
        
        default:
            break;
        }
    }

// ---------------------------------------------------------
// DynInitMainMenuL
// ---------------------------------------------------------
void CPostcardAppUi::DynInitMainMenuL( CEikMenuPane* aMenuPane )
    {
    aMenuPane->SetItemDimmed( EPostcardCmdSend, 
        IsMainMenuOptionInvisible( EPostcardCmdSend ));
    aMenuPane->SetItemDimmed( EPostcardCmdEditAddress, 
        IsMainMenuOptionInvisible( EPostcardCmdEditAddress ));
    aMenuPane->SetItemDimmed( EPostcardCmdEditGreeting, 
        IsMainMenuOptionInvisible( EPostcardCmdEditGreeting ));
    aMenuPane->SetItemDimmed( EPostcardCmdInsertSubmenu, 
        IsMainMenuOptionInvisible( EPostcardCmdInsertSubmenu ));
    aMenuPane->SetItemDimmed( EPostcardCmdReplaceSubmenu, 
        IsMainMenuOptionInvisible( EPostcardCmdReplaceSubmenu ));
    aMenuPane->SetItemDimmed( EPostcardCmdInsertFromGallery, 
        IsMainMenuOptionInvisible( EPostcardCmdInsertFromGallery ));
    aMenuPane->SetItemDimmed( EPostcardCmdReplaceFromGallery, 
        IsMainMenuOptionInvisible( EPostcardCmdReplaceFromGallery ));
    aMenuPane->SetItemDimmed( EPostcardCmdForward, 
        IsMainMenuOptionInvisible( EPostcardCmdForward ));
    aMenuPane->SetItemDimmed( EPostcardCmdDelete, 
        IsMainMenuOptionInvisible( EPostcardCmdDelete ));
    aMenuPane->SetItemDimmed( EPostcardCmdMessageInfo, 
        IsMainMenuOptionInvisible( EPostcardCmdMessageInfo ));
    aMenuPane->SetItemDimmed( EAknCmdHelp, 
        IsMainMenuOptionInvisible( EAknCmdHelp ));
    aMenuPane->SetItemDimmed( EAknCmdExit, 
        IsMainMenuOptionInvisible( EAknCmdExit ));
    }

// ---------------------------------------------------------
// DynInitInsertSubMenuL
// ---------------------------------------------------------
void CPostcardAppUi::DynInitInsertSubMenuL( CEikMenuPane* aMenuPane )
    {
    // If insert/replace submenu is visible -> from Gallery is always visible!
    if ( !iImage )
        {
        aMenuPane->SetItemDimmed( EPostcardCmdInsertFromCamera, 
                IsMainMenuOptionInvisible( EPostcardCmdInsertFromCamera ));
        }
    else
        {
        aMenuPane->SetItemDimmed( EPostcardCmdReplaceFromCamera, 
                IsMainMenuOptionInvisible( EPostcardCmdReplaceFromCamera ));
        }
    
    }

// ---------------------------------------------------------
// DynInitContextMenuL
// ---------------------------------------------------------
void CPostcardAppUi::DynInitContextMenuL( CEikMenuPane* aMenuPane )
    {
    aMenuPane->SetItemDimmed( EPostcardCmdInsertSubmenu, 
        IsMainMenuOptionInvisible( EPostcardCmdInsertSubmenu ));
    aMenuPane->SetItemDimmed( EPostcardCmdReplaceSubmenu, 
        IsMainMenuOptionInvisible( EPostcardCmdReplaceSubmenu ));
    aMenuPane->SetItemDimmed( EPostcardCmdInsertFromGallery, 
        IsMainMenuOptionInvisible( EPostcardCmdInsertFromGallery ));
    aMenuPane->SetItemDimmed( EPostcardCmdReplaceFromGallery, 
        IsMainMenuOptionInvisible( EPostcardCmdReplaceFromGallery ));
    }

// ---------------------------------------------------------
// IsMainMenuOptionInvisible
// ---------------------------------------------------------
TBool CPostcardAppUi::IsMainMenuOptionInvisible( TInt aMenuItem )
    {
    TBool retVal( ETrue ); // Everything first invisible
    TPostcardPart focused = iController->Focused( );

    switch ( aMenuItem )
        {
        case EPostcardCmdSend:
            if( iMsgType != EPostcardSent )
                {
                if( ContactHasData( *iContact ) )
                    {
                    // If there's contact data -> show "Send"
                    retVal = EFalse;                
                    }
                }
            break;
        case EPostcardCmdEditAddress:
            if( iMsgType != EPostcardSent && focused != EPostcardImage )
                {
                retVal = EFalse;
                }
            break;
        case EPostcardCmdEditGreeting:
            if( iMsgType != EPostcardSent && focused != EPostcardImage )
                {
                retVal = EFalse;
                }
            break;
        case EPostcardCmdInsertSubmenu:
            if( iMsgType != EPostcardSent && !iImage )
                {
                if( iPocaFlags & EPostcardCamcorder )
                	{
	                retVal = EFalse;            		
                	}
                }
            break;
        case EPostcardCmdReplaceSubmenu:
            if( iMsgType != EPostcardSent && iImage )
                {
                if( iPocaFlags & EPostcardCamcorder )
                	{
	                retVal = EFalse;            		
                	}
                }
            break;
        case EPostcardCmdInsertFromGallery:
            if( iMsgType != EPostcardSent && !iImage )
                {
                if( !(iPocaFlags & EPostcardCamcorder ) )
                	{
	                retVal = EFalse;            		
                	}
                }
            break;
        case EPostcardCmdReplaceFromGallery:
            if( iMsgType != EPostcardSent && iImage )
                {
                if( !(iPocaFlags & EPostcardCamcorder ) )
                	{
	                retVal = EFalse;            		
                	}
                }
            break;
        case EPostcardCmdInsertFromCamera:
            if( iMsgType != EPostcardSent )
                {
                if( iPocaFlags & EPostcardCamcorder )
                	{
	                retVal = EFalse;            		
                	}
                }
            break;
        case EPostcardCmdReplaceFromCamera:
            if( iMsgType != EPostcardSent && iImage )
                {
                if( iPocaFlags & EPostcardCamcorder )
                    {
                    retVal = EFalse;                    
                    }
                }
            break;
        case EPostcardCmdForward:
            if( iMsgType == EPostcardSent )
                {
                retVal = EFalse;
                }
            break;
        case EPostcardCmdDelete:
            if( iMsgType == EPostcardSent )
                {
                retVal = EFalse;
                }
            break;
        case EPostcardCmdMessageInfo:
            retVal = EFalse;
            break;
        case EAknCmdHelp:
			if( iPocaFlags & EPostcardHelp )
				{
				retVal = EFalse;				
				}
			break;
        case EAknCmdExit:
            retVal = EFalse;
            break;
        default:
            ;
        }
    return retVal;
    }

// ---------------------------------------------------------
// DoSendL
// ---------------------------------------------------------
void CPostcardAppUi::DoSendL()
    {
	// Let's first check all the fields are present
	if( !iImage )
		{
		ShowInformationNoteL( R_POSTCARD_SEND_INSERT_IMAGE, EFalse );
	    iController->SetFocused( EPostcardImage );
	    SetMiddleSoftkeyL();
	    iController->DrawDeferred( );    
		return;
		}

    // Check that mandatory address fields are present
    TInt mandatories = MandatoryAddressFields();
    CContactItemFieldSet& fields = iContact->CardFields();
    TBool allFilled = ETrue;
    TInt i = EPostcardAddressName;
    for( ; mandatories && allFilled ; i++, mandatories >>= 1 )
        {
        if( mandatories & 1 )
            {
            const CContactItemField* field = TPostcardUtils::FieldOrNull( fields,
                TPostcardUtils::ContactItemNameFromId( i ) );
            allFilled = ( field && field->TextStorage()->IsFull() );
            }
        }
    // If not all filled, open the edit address dialog with the right field
    if( !allFilled )
        {
        DoEditAddressL( TKeyEvent(), i - 1 );
		return;
        }

    // Address is ok, let's still check the lengths..
	if( !CheckLengthsL( ) )
		{
		return;
		}
	
	// Here we are ready to send -> now let's ask the confirmation
    if( iMessageQuery )
        {
        iMessageQuery->CancelMsgQuery();
        delete iMessageQuery;
        iMessageQuery = NULL;
        }

   
    iMessageQuery = CAknGlobalMsgQuery::NewL();

    HBufC* addressString = CreateAddressStringForSendConfirmLC();

    delete iSendText; iSendText = NULL;
    iSendText = StringLoader::LoadL( R_POSTCARD_RECIPIENT_PROMPT_TEXT,
        *addressString );
    CleanupStack::PopAndDestroy( addressString ); // addressString

    CPostcardQueryWaiter* queryWaiter = new (ELeave) CPostcardQueryWaiter( this );
    CleanupStack::PushL( queryWaiter );

    iMessageQuery->ShowMsgQueryL( 
            queryWaiter->iStatus,
            *iSendText,
            R_AVKON_SOFTKEYS_SEND_CANCEL__SEND,
            KNullDesC(),
            KAvkonBitmapFile,
            EMbmAvkonQgn_note_query,
            EMbmAvkonQgn_note_query_mask );

    queryWaiter->SetActiveD(); // Will be deleted when the query is dismissed.
    CleanupStack::Pop( queryWaiter ); // queryWaiter

    // The rest of the sending process is done in QueryWaiterCallback !!
    }

// ---------------------------------------------------------
// DoSaveL
// ---------------------------------------------------------
void CPostcardAppUi::DoSaveL( )
    {
    if ( iOperation && iOperation->IsActive() )
        {
        // Save operation already in progress, don't start another
        return;
        }
    
    delete iOperation;
    iOperation = NULL;
    
    iOperation = CPostcardOperationSave::NewL(
        *this,
        Document(),
        *this,
        FsSession( ),
        *iContact,
        *iGreeting
        );
        
    iOperation->Start( EFalse );    // EFalse means no sending
    
    iAbsorber = CAknInputBlock::NewLC();
    CleanupStack::Pop( iAbsorber );
    }

// ---------------------------------------------------------
// DoBackSaveL
// ---------------------------------------------------------
void CPostcardAppUi::DoBackSaveL()
    {
    if( iMsgType == EPostcardSent )
        {
        // Msg is a sent one so just close the application
        Exit( EAknSoftkeyClose );
        return;
        }
        
    if ( IsMessageEmpty() ) 
        {
        if ( Document().Mtm().Entry().Entry().Visible() )
            {
            ShowInformationNoteL( R_POSTCARD_MESSAGE_DELETED, ETrue );
            }
        DeleteAndExitL();
        }
    else
        {
        TInt closeVal = ShowCloseQueryL( );
        if ( closeVal == EPostcardCloseCancel )
            {
            // User cancels the query
            return;
            }
        else if ( closeVal == EPostcardCloseDelete )
            {
            DeleteAndExitL();
            }
        else
            {
            // Save message
            if ( Document( ).Modified( ) )
                {
                if ( CanSaveMessageL() ) 
                    {
                    TInt resId = R_POSTCARD_SAVING_NEW;
                    if( iMsgType == EPostcardDraft )
                        {
                        // We are saving an old postcard
                        resId = R_POSTCARD_SAVING;
                        }
                        
                    iPocaFlags |= EPostcardClosing;
                    DoSaveL( );
                    ShowWaitNoteL( resId, ETrue );
                    BeginActiveWait();
                    }
                else
                    {
                    if ( ShowConfirmationQueryL( R_POSTCARD_NO_SPACE_TO_SAVE ) )
                        {
                        // Exit without saving.
                        Exit( EAknSoftkeyClose );
                        }
                    //else nothing.
                    }
                }
            else
                {
                // No changes -> just close.
                Exit( EAknSoftkeyClose );
                }
            }
        }        
    }

// ---------------------------------------------------------
// IsMessageEmpty
// ---------------------------------------------------------
TBool CPostcardAppUi::IsMessageEmpty( )
    {
    if( !iImage && iGreetingLength == 0 &&
        ( !iContact || !ContactHasData( *iContact ) ) )
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// DoUpdateEntryAndShowMessageInfoL
// ---------------------------------------------------------
void CPostcardAppUi::DoUpdateEntryAndShowMessageInfoL( )
    {
    // First update the TMsvEntry
    if( iMsgType != EPostcardSent )
        { 
        // We are dealing with unsent message -> save recipient/text for now
        TMmsMsvEntry tEntry = static_cast<TMmsMsvEntry>( Document().Entry() );

        tEntry.iDate.UniversalTime();

        // Let's find name field
        _LIT( KEmptyName, "");
        TPtrC namePtr( KEmptyName );
        CContactItemFieldSet& fieldSet =  iContact->CardFields();
        for( TInt i = 0; i<fieldSet.Count(); i++ )
            {
            if( !fieldSet[i].Label().Compare( TPostcardUtils::ContactItemNameFromId( EPostcardAddressName ) ) )
                {
                namePtr.Set( fieldSet[i].TextStorage()->Text() );
                break;
                }
            }
        tEntry.iDetails.Set( namePtr ) ;
        // Let's set the subject
        tEntry.iDescription.Set(
            iGreeting->Read( 0, Min( iGreetingLength, 20 ) ) );
        
    	tEntry.iBioType = KUidMsgSubTypeMmsPostcard.iUid;
        
        Document().CurrentEntry().ChangeL( tEntry );
        }
    // And now call message info
    DoShowMessageInfoL();
    }

// ---------------------------------------------------------
// DoShowMessageInfoL
// ---------------------------------------------------------
void CPostcardAppUi::DoShowMessageInfoL()
    {
    // Selection is not really used, just given as a parameter
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    TBuf8<1> param;
    param.Zero();

    CMsvSingleOpWatcher* watch = CMsvSingleOpWatcher::NewLC( Document() );
    
    CMsvOperation* op = Document().MtmUi().InvokeAsyncFunctionL(
        KMtmUiFunctionMessageInfo,
        *selection,
        watch->iStatus,
        param );
    CleanupStack::Pop( watch ); // watch
    Document().AddSingleOperationL( op, watch );

    CleanupStack::PopAndDestroy( selection ); // selection
    }

// ---------------------------------------------------------
// DoBackspaceL
// ---------------------------------------------------------
void CPostcardAppUi::DoBackspaceL( )
    {
    if( iMsgType == EPostcardSent )
        {
        if( ShowConfirmationQueryL( R_POSTCARD_DELETE_MESSAGE ) )
            {
            DeleteAndExitL( );
            }
        }
    else 
        {
        if( iController->Focused( ) == EPostcardImage && iImage )
            {
            if( ShowConfirmationQueryL( R_POSTCARD_REMOVE_IMAGE ) )
            	{
				// Lets go thru the attas to find the right atta
        		CMsvStore* editStore = Document( ).Mtm( ).Entry( ).EditStoreL( );
				MMsvAttachmentManager* manager = &( editStore->AttachmentManagerL( ) );
			    TInt count = manager->AttachmentCount( );
    			CMsvAttachment* atta = NULL;
    			TInt a = 0;
			    for( ; a < count; a++)
        			{
			        atta = manager->GetAttachmentInfoL( a );
        			CleanupStack::PushL( atta );
        			if( Document( ).IsImage( atta->MimeType( ) ) )
        				{
						CleanupStack::PopAndDestroy( atta );
        				break;
        				}
					CleanupStack::PopAndDestroy( atta );
        			}
        		if( a > -1 && a < count )
        			{
        			// and delete atta if found
					editStore->AttachmentManagerExtensionsL( ).RemoveAttachmentL( a );    				
        			}
		    	editStore->CommitL( );
			    delete editStore;
                // Lets nullify the icon in Controller
    	        iController->SetBitmap( EPostcardImage, NULL );
	            iImage = NULL;
	            // And draw the UI again
        		iController->DrawDeferred( );
            	}
            }
        }
    }

// ---------------------------------------------------------
// DoForwardMessageL
// ---------------------------------------------------------
void CPostcardAppUi::DoForwardMessageL( )
    {
    TMsvPartList parts =
        KMsvMessagePartBody |
        KMsvMessagePartDescription |
        KMsvMessagePartAttachments;

    CBaseMtmUi& mtmUi = Document().MtmUi();

    CMsgOperationWait* wait =
        CMsgOperationWait::NewLC( EActivePriorityWsEvents + 10 );

    mtmUi.SetPreferences( mtmUi.Preferences() | EMtmUiFlagEditorPreferEmbedded );
    CMsvOperation* oper = mtmUi.ForwardL( KMsvDraftEntryIdValue, parts, wait->iStatus );
    CleanupStack::PushL( oper );

    wait->Start();

    CleanupStack::PopAndDestroy( 2 );  // wait, oper
    }

// ---------------------------------------------------------
// DoEditAddressL
// ---------------------------------------------------------
void CPostcardAppUi::DoEditAddressL( TKeyEvent aKeyEvent /*=TKeyEvent()*/,
                               				TInt aFirstFocused /*=-1*/)
    { 
	CPostcardAddressForm* form = NULL;
	TInt ret = 0;
    iController->SetFocused( EPostcardRecipient );
    form = CPostcardAddressForm::NewL( *this, Document(), *iContact, ret,
        aFirstFocused, aKeyEvent );
    form->SetMopParent( iController );
    iPocaFlags |= EPostcardRecipientActive;

    // Form might leave with KLeaveExit from CPostcardContact, 
    // so we need to trap it and take proper actions. If it leaves,
    // it means that Messaging is closed from FSW and there has been
    // an active AIW to phonebook.
    TRAPD( err, form->ExecLD() );

	iPocaFlags &= ~EPostcardRecipientActive;
    if ( err )
        {
        // form must be deleted here, since for some reason its destructor is
        // not called, if leave oocurs in ExecLD.
        delete form;
        form = NULL;
        User::Leave( err );
        }

    if( ret == EPostcardDialogExit )
        {
        // If exit -> we need to save if possible
        iPocaFlags |= EPostcardExitCalledFromDialog;
        if ( Document( ).Modified( ) )
            {
            if ( CanSaveMessageL() )
                {
                TInt resId = R_POSTCARD_SAVING;
                ShowWaitNoteL( resId, ETrue );
                DoSaveL( );
                }
            else
                {
                // No space -> exit without saving.
                Exit( EEikCmdExit );
                }
            }
        else
            {
            Exit( EEikCmdExit );         
            }
        }
    else if( iMsgType == EPostcardSent )
        {
        if( ret == EPostcardDialogForward )
            {
            DoForwardMessageL( );
            return;
            }           
        iController->DrawDeferred( );
        }
	else
		{
		Document( ).SetChanged( EPostcardRecipient );
        iController->DrawDeferred( );
		if( ret == EPostcardDialogSend )
		    {
            DoSendL( );		    
		    }
		}
    }

// ---------------------------------------------------------
// DoEditGreetingL
// ---------------------------------------------------------
void CPostcardAppUi::DoEditGreetingL( TKeyEvent aKeyEvent )
    { 
    CPostcardTextDialog* dialog = NULL;
    TInt ret = 0;
    iController->SetFocused( EPostcardText );
    dialog = CPostcardTextDialog::NewL(*this, Document(), *iGreeting, 
        ret, aKeyEvent );
    dialog->SetMopParent( iController );
    iPocaFlags |= EPostcardGreetingActive;
    TRAP_IGNORE( dialog->ExecLD() );

	iPocaFlags &= ~EPostcardGreetingActive;

    if( ret == EPostcardDialogExit )
        {
        // If exit -> we need to save if possible
        iPocaFlags |= EPostcardExitCalledFromDialog;
        if ( Document( ).Modified( ) )
            {
            if ( CanSaveMessageL() )
                {
                TInt resId = R_POSTCARD_SAVING;
                ShowWaitNoteL( resId, ETrue );
                DoSaveL( );
                }
            else
                {
                // No space -> exit without saving.
                Exit( EEikCmdExit );                
                }
            }
        else
            {
            Exit( EEikCmdExit );         
            }
        }
    else if( iMsgType == EPostcardSent )
        {
        if( ret == EPostcardDialogForward )
            {
            DoForwardMessageL( );
            return;
            }           
        iController->DrawDeferred( );
        }
	else
		{
		Document( ).SetChanged( EPostcardText );
        iController->DrawDeferred( );
		if( ret == EPostcardDialogSend )
		    {
		    // Send chosen in greeting dialog
            if( ContactHasData( *iContact ) )
                {
                // If there's contact data -> start sending
                DoSendL( );
                }
            else
                {
                // If no contact data -> open address dialog
                DoEditAddressL( );                
                }
		    }
		}
    }

// ---------------------------------------------------------
// DoInsertImageL
// ---------------------------------------------------------
void CPostcardAppUi::DoInsertImageL( MsgAttachmentUtils::TMsgAttachmentFetchType aFetchType )
    {
    iController->SetFocused( EPostcardImage );
    SetMiddleSoftkeyL(); // set msk for the view
    if ( FetchFileL( aFetchType ) )
        {
        // The file is selected and iImageInfo created

	    ShowWaitNoteL( R_POSTCARD_WAIT_INSERTING, EFalse );

        iOperation = CPostcardOperationInsertImage::NewL(
            *this,
            Document(),
            *this,
            iImageInfo,
            FsSession() );
        iImageInfo = NULL; // ownership moves
        iOperation->Start( EFalse );
        
        iAbsorber = CAknInputBlock::NewLC();
        CleanupStack::Pop( iAbsorber );
        }

    iController->DrawDeferred( );
    }

// ---------------------------------------------------------
// FetchFileL
// ---------------------------------------------------------
TBool CPostcardAppUi::FetchFileL( MsgAttachmentUtils::TMsgAttachmentFetchType aFetchType )
    {
    CDesCArrayFlat* dummy = new ( ELeave ) CDesCArrayFlat( 1 );
    CleanupStack::PushL( dummy );
    TFileName fileName;
    TBool fetchOK = MsgAttachmentUtils::FetchFileL(
        aFetchType,
        fileName,
        *dummy,
        ETrue,
        EFalse,
        this );

    CleanupStack::PopAndDestroy( dummy );

    switch ( aFetchType )
        {
        case MsgAttachmentUtils::ENewImage:
        case MsgAttachmentUtils::ENewAudio:
        case MsgAttachmentUtils::ENewVideo:            
            {
            if ( fetchOK )
                {
                // Something was fetched
                fetchOK = CreateMediaInfoForInsertL( fileName )
                    ? ETrue
                    : EFalse;
                }
            }
            break;
        case MsgAttachmentUtils::EImage:
        case MsgAttachmentUtils::EAudio:
        case MsgAttachmentUtils::EVideo:
        default:
            // CreateMediaInfoForInsertL is called in VeritySelectionL
            break;
        }
    return fetchOK;
    }


// ---------------------------------------------------------
// VerifySelectionL
// ---------------------------------------------------------
TBool CPostcardAppUi::VerifySelectionL( const MDesCArray* aSelectedFiles )
    {
    CAknInputBlock* blocker = CAknInputBlock::NewLC();

    TBool ret = EFalse;
    if ( aSelectedFiles->MdcaCount() == 1 )
        {
        TPtrC ptr = aSelectedFiles->MdcaPoint( 0 );
        if ( CreateMediaInfoForInsertL( ptr ) )
            {
            ret = ETrue;
            }
        }
        
    CleanupStack::PopAndDestroy( blocker );  // Input blocker
    return ret;
    }

// ---------------------------------------------------------
// CreateMediaInfoForInsertL
// ---------------------------------------------------------
TBool CPostcardAppUi::CreateMediaInfoForInsertL( const TDesC& aFileName )
    {
    if( iImageInfo )
    	{
    	delete iImageInfo;
    	iImageInfo = NULL;
    	}

    RFile fileHandle = Document().MediaResolver()->FileHandleL( aFileName );
    CleanupClosePushL( fileHandle );
	iImageInfo = static_cast<CMsgImageInfo*>( Document().MediaResolver()->CreateMediaInfoL( fileHandle ) );
	Document().MediaResolver()->ParseInfoDetailsL( iImageInfo, fileHandle );
	CleanupStack::PopAndDestroy( &fileHandle );

    TInt errResId = 0;

    if ( !CanSaveMessageL( iImageInfo->FileSize() ) )
        {
        User::Leave( KErrDiskFull );
        }

    // Supported type check
    if( iImageInfo->Protection( ) )
        {
        errResId = R_POSTCARD_CANNOT_SEND_PROTECTED;        
        }
    else if ( iImageInfo->Corrupt() )
        {
        // Corrupt is set to only supported media types
        errResId = R_POSTCARD_OBJECT_CORRUPTED;
        }
    else if( !Document( ).IsImage( iImageInfo->MimeType( ) ) )
    	{
        errResId = R_POSTCARD_FORMAT_NOT_SUPPORTED;    		
    	}
    // DRM check
    else if ( iImageInfo->MediaType() != EMsgMediaImage )
        {
        // Keep this after DRM check!
        errResId = R_POSTCARD_FORMAT_NOT_SUPPORTED;
        }
    else if ( iImageInfo->IsAnimation( ) )
        {
        errResId = R_POSTCARD_FORMAT_NOT_SUPPORTED;
        }

    if ( errResId != 0 )
        {
        delete iImageInfo;
        iImageInfo = NULL;
        ShowInformationNoteL( errResId, EFalse );
        return EFalse;
        }
    return ETrue;

    }

// ---------------------------------------------------------
// DoSelectionKeyL
// ---------------------------------------------------------
void CPostcardAppUi::DoSelectionKeyL()
    {
	if( iMsgType != EPostcardSent )
		{
		// We are in editor mode
		TPostcardPart focused = iController->Focused( );
		switch ( focused )
        	{
        	case EPostcardImage:
        		{
		        MenuBar( )->SetMenuTitleResourceId( R_POSTCARD_CONTEXT_MENUBAR );
		        MenuBar()->SetMenuType(CEikMenuBar::EMenuContext);
		        // if display fails set old menu back
		        TRAPD( err, MenuBar( )->TryDisplayMenuBarL( ) ); 
		        MenuBar( )->SetMenuTitleResourceId( R_POSTCARD_MAIN_MENUBAR );
		        MenuBar()->SetMenuType(CEikMenuBar::EMenuOptions);
		        User::LeaveIfError( err );
        		break;
        		}
    		case EPostcardRecipient:
    			{
    			DoEditAddressL( );
    			break;
    			}
    		case EPostcardText:
    			{
    			DoEditGreetingL( );
    			break;
    			}
    		default:
    			break;
	        }
		}
	else
		{
		// We are in viewer mode
		TPostcardPart focused = iController->Focused( );
		switch ( focused )
        	{
        	case EPostcardImage:
        		{
        		break;
        		}
    		case EPostcardRecipient:
    			{
    			DoEditAddressL( );
    			break;
    			}
    		case EPostcardText:
    			{
    			DoEditGreetingL( );
    			break;
    			}
    		default:
    			break;
	        }			
		}
    return;
    }
    
// ---------------------------------------------------------
// LaunchHelpL
// ---------------------------------------------------------
void CPostcardAppUi::LaunchHelpL( const TDesC& aContext )
    {
    CArrayFix<TCoeHelpContext>* 
        helpContext = new(ELeave) CArrayFixFlat<TCoeHelpContext>( 1 );
    TCoeHelpContext context( TUid::Uid(0x10207247), aContext );
    helpContext->AppendL( context );
    HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession( ), helpContext );    
    }

// ---------------------------------------------------------
// FsSession
// ---------------------------------------------------------
RFs& CPostcardAppUi::FsSession() const
    {
    return ( iCoeEnv->FsSession() );
    }

#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------
// CPostcardAppUi::PocaPointerEvent
// ---------------------------------------------------------
void CPostcardAppUi::PocaPointerEventL( TPocaPointerEvent aEvent,
    TPocaPointerEventType aEventType )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        // Handle pointer (pen) events to areas in main pane
        switch( aEvent )
            {
            case EPocaPointerEventUpIcon:
            case EPocaPointerEventDownIcon:
                iController->MoveVertically( aEvent == EPocaPointerEventUpIcon ? ETrue : EFalse );
                
                if ( iMsgType == EPostcardSent )
                    {
                    // In viewer
                    if ( iController->Focused() != EPostcardImage )
                        {
                        // Viewer does not have navigation arrows in text side
                        iNaviPane->Pop( iNaviDecorator );
                        }
                    else
                        {
                        iNaviPane->PushL( *iNaviDecorator );
                        }
                    }
                break;
                
            case EPocaPointerEventGreeting:
                if( iController->Focused() == EPostcardText )
                    {
                    DoSelectionKeyL();
                    }
                else
                    {
                    iController->MoveHorizontally(
                        aEvent == EPocaPointerEventAddress ? ETrue : EFalse );
                    }
                break;
            case EPocaPointerEventAddress:
                if( iController->Focused() == EPostcardRecipient )
                    {
                    DoSelectionKeyL();
                    }
                else
                    {
                    iController->MoveHorizontally(
                        aEvent == EPocaPointerEventAddress ? ETrue : EFalse );
                    }
               break;
            case EPocaPointerEventFrontBg: // front picture area
                DoSelectionKeyL();
                break;
            default:
                ;
            }
        SetMiddleSoftkeyL(); // set msk for the view
        }
    else
        {
        // Suppress warnings
        aEvent = aEvent;
        aEventType = aEventType;
        }
    }

#endif // RD_SCALABLE_UI_V2

// ---------------------------------------------------------
// CPostcardAppUi::MenuBar
// Returns application menubar.
// ---------------------------------------------------------
CEikMenuBar* CPostcardAppUi::MenuBar() const
    {
    return iEikonEnv->AppUiFactory()->MenuBar();
    }

// ---------------------------------------------------------
// CPostcardAppUi::HandleResourceChangeL
// ---------------------------------------------------------
void CPostcardAppUi::HandleResourceChangeL(TInt aType)
    {
    CMsgEditorAppUi::HandleResourceChangeL(aType);
	if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        // The layout is changing -> reread coordinates
        if ( iController )
    	   	{
        	iController->RefreshCoordinates( );
        	}
    	if ( iGreetingLength )
    		{
    		CreateGreetingBitmapL( );
    		}
        if ( ContactHasData( ) )
            {
            RefreshRecipientBitmapL( );
            }
        RefreshFocusLinesL( );
        if ( iController )
            {
            iController->DrawDeferred( );
            }
        
        SetTitleIconL();
        }
    else if ( aType == KAknsMessageSkinChange )
        {
        CreateAndSetTitleIconL();
        // The skin is changing -> recreate icons and refresh coordinates
        if( iController )
            {
            iController->ReLoadIconsL( );
            }
        }
    }

// ---------------------------------------------------------
// CPostcardAppUi::ShowCloseQueryL
// ---------------------------------------------------------
TInt CPostcardAppUi::ShowCloseQueryL()
    {
    TInt selectedIndex( 0 );
    CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( &selectedIndex );
    dlg->PrepareLC( R_POSTCARD_CLOSE_QUERY );
    if ( dlg->RunLD() )
        {
        return ( selectedIndex == 0 )
            ? EPostcardCloseSave
            : EPostcardCloseDelete;
        }
    else
        {
        return EPostcardCloseCancel;
        }
    }

// ---------------------------------------------------------
// CPostcardAppUi::ExitAndSaveL
// ---------------------------------------------------------
void CPostcardAppUi::ExitAndSaveL()
    {
    if ( iPocaFlags & EPostcardClosing )
        {
        Exit( EAknSoftkeyClose );
        return;
        }
    
    if ( CAknEnv::AppWithShutterRunning() )
        {
        iPocaFlags |= EPostcardRunAppShutterAtExit;
        }

    if ( iPocaFlags & EPostcardLaunchSuccessful &&
         Document().MediaAvailable() )
        {
        DoMsgSaveExitL();
        }
    
    Exit();
    }

// ---------------------------------------------------------
// CPostcardAppUi::PostcardOperationEvent
// ---------------------------------------------------------
void CPostcardAppUi::PostcardOperationEvent(
            TPostcardOperationType aOperation,
            TPostcardOperationEvent aEvent )
    {
    // If a large image is inserted, it may take enough time for a screen
    // backlight to go off. It wouldn't reactivate unless inactivity timer
    // is reseted (or user presses a key).
    User::ResetInactivityTime();

    delete iAbsorber;
    iAbsorber = NULL;

    if( aOperation == EPostcardOperationOpen )
        { 
        if( aEvent == EPostcardOperationError )
            {
            RemoveWaitNote( );
            TInt error = iOperation->GetError( );
            if( error == R_POSTCARD_FORMAT_NOT_SUPPORTED ||
                error == R_POSTCARD_MULTIPLE_FILES ||
                error == R_POSTCARD_CANNOT_SEND_PROTECTED ||
                error == R_POSTCARD_PROCESS_NOT_SUCCESSFUL ||
                error == R_POSTCARD_OBJECT_CORRUPTED )
                {
                TRAP_IGNORE( ShowInformationNoteL( error, EFalse ) );
                }
            iOperation->Cancel( );
            delete iOperation;
            iOperation = NULL;
            TRAP_IGNORE( iController->ActivateL( ) ); // This is called to give infonote more time
            DoDelayedExit( 0 );
            }
        else
            {
            // Delete opening operation. CompleteLaunchViewL() may start
            // a save operation.
            delete iOperation;
            iOperation = NULL;
            // Change state only if opening an existing message. State for a changed
            // item is changed when it is really edited. This prevents saving of empty
            // messages to drafts.
            if ( Document().MessageType() != EPostcardNew &&
                 Document().MessageType() != EPostcardSent )
                {
                if ( iImage )
                    {
                    Document().SetChanged( EPostcardImage );
                    }
                if ( iGreeting )
                    {
                    Document().SetChanged( EPostcardText );                
                    }
                if ( iContact )
                    {
                    Document().SetChanged( EPostcardRecipient );
                    }
                }

            TRAP_IGNORE( CompleteLaunchViewL( ) );
            }        
        }
    else if( aOperation == EPostcardOperationSave )
        {
        if( iOperation->GetError( ) )
        	{
        	TRAP_IGNORE( ShowGlobalInformationNoteL( iOperation->GetError( ), EFalse ) );
        	}
        delete iOperation;
        iOperation = NULL;

        RemoveWaitNote();
        if( aEvent == EPostcardOperationError )
            {
            Exit( EAknSoftkeyClose );
            }
        else
            {
            DoDelayedExit( 0 );
            }
        EndActiveWait();
        }
    else
    	{
    	// Insert image operation
    	TInt error = iOperation->GetError( );
        if( error )
        	{
        	if( error == R_POSTCARD_FORMAT_NOT_SUPPORTED ||
                error == R_POSTCARD_PROCESS_NOT_SUCCESSFUL )
        		{
	        	TRAP_IGNORE( ShowInformationNoteL( error, EFalse ) );
        		}
        	}
    	delete iOperation;
    	iOperation = NULL;
    	delete iImageInfo;
    	iImageInfo = NULL;
    	RemoveWaitNote( );
	    iController->DrawDeferred( );	
	    Document( ).SetChanged( EPostcardImage );
    	}
    }

// ---------------------------------------------------------
// DoDelayedExit
// ---------------------------------------------------------
void CPostcardAppUi::DoDelayedExit( TInt aDelayTime )
    {
    if ( !iIdle )
        {
        
        }
    iIdle->Cancel();
    iIdle->Start( aDelayTime,
                  aDelayTime, 
                  TCallBack( DelayedExit, this ));
    }

// ---------------------------------------------------------
// DelayedExit
// ---------------------------------------------------------
TInt CPostcardAppUi::DelayedExit( TAny* aThis )
    {
    CPostcardAppUi* editor = static_cast<CPostcardAppUi*>( aThis );
    editor->DoExternalExit();
	CAknEnv::RunAppShutter( );
    return KErrNone;
    }

// ---------------------------------------------------------
// DoExternalExit
// ---------------------------------------------------------
void CPostcardAppUi::DoExternalExit()
    {
    RemoveWaitNote( );
    if( iOperation )
        {
        delete iOperation;
        iOperation = NULL;
        }
    
    if( iPocaFlags & EPostcardExitCalledFromDialog )
        {
        Exit( EAknCmdExit );
        }
    else
        {
        Exit( EAknSoftkeyClose );
        }
    }

// ---------------------------------------------------------
// CPostcardAppUi::SetImageL
// ---------------------------------------------------------
void CPostcardAppUi::SetImage( CGulIcon* aIcon )
    {
    iImage = aIcon;
    iController->SetBitmap( EPostcardImage, aIcon );
    }

// ---------------------------------------------------------
// CPostcardAppUi::SetTextL
// ---------------------------------------------------------
void CPostcardAppUi::SetTextL( CPlainText& aPlainText )
    {
	delete iGreeting;
	iGreeting = &aPlainText;
	iGreetingLength = iGreeting->DocumentLength();
	if( iGreetingLength )
		{
		CreateGreetingBitmapL( );
		}
	else
		{
		iController->RemoveBitmap( EPostcardText );		
		}
    }

// ---------------------------------------------------------
// CPostcardAppUi::MaxTextLength
// ---------------------------------------------------------
TInt CPostcardAppUi::MaxTextLength( TInt aControlId )
    {
    const TInt KFirstControlId = EPostcardTextEditor;
    __ASSERT_DEBUG( aControlId >= KFirstControlId &&
        aControlId <= EPostcardAddressCountry, Panic( EPostcardPanicCoding ) );

    // Keys has to be in the same order in the table as control IDs in
    // TPostcardControls
    static const TUint32 keys[ENumPostcardControls] = 
        {
        KPocaKeyMaxGreetingLength, KPocaKeyMaxNameLength,
        KPocaKeyMaxInfoLength, KPocaKeyMaxStreetLength,
        KPocaKeyMaxZipLength, KPocaKeyMaxCityLength,
        KPocaKeyMaxStateLength, KPocaKeyMaxCountryLength
        };
    return iCenRep->Get( keys[ aControlId - KFirstControlId ] );
    }

// ---------------------------------------------------------
// CPostcardAppUi::CenRep
// ---------------------------------------------------------
const CPostcardCenRep& CPostcardAppUi::CenRep()
    {
    return *iCenRep;
    }

// ---------------------------------------------------------
// CPostcardAppUi::SetRecipientL
// ---------------------------------------------------------
void CPostcardAppUi::SetRecipientL( CContactCard& aRecipient )
    {
    delete iContact;
    iContact = &aRecipient;

    if( !ContactHasData( aRecipient ) )
        {
        iController->RemoveBitmap( EPostcardRecipient );
        return;			
        }

    RefreshRecipientBitmapL( );
    }

// ---------------------------------------------------------
// CPostcardAppUi::RefreshRecipientBitmapL
// ---------------------------------------------------------
void CPostcardAppUi::RefreshRecipientBitmapL( )
    {
    // Get the contact set
    CContactItemFieldSet& set = iContact->CardFields( );
	
    CDesCArrayFlat* strings = new (ELeave) CDesCArrayFlat( CPostcardAddressForm::ENumAddrFields );
    CleanupStack::PushL( strings );

    CArrayFixFlat<TInt>* indexes =
        new ( ELeave ) CArrayFixFlat<TInt>(CPostcardAddressForm::ENumAddrFields);
    CleanupStack::PushL( indexes );
    for( TInt a=0; a<CPostcardAddressForm::ENumAddrFields; a++ )
        {
        indexes->AppendL(-1);
        }

    // Lets first find the indexes of different fields from the ContactItemFieldSet
	for( TInt i = 0; i<set.Count( ); i++ )
		{
		CContactItemField& field = set[i];
    	for( TInt a = EPostcardAddressName; a <= EPostcardAddressCountry; a++ )
    		{
    		if( !field.Label( ).Compare( TPostcardUtils::ContactItemNameFromId( a ) ) )
    		    { // Label found so store the index of ContactItemField
    	        indexes->Delete( a-EPostcardAddressName ); // remove -1
    	        indexes->InsertL( a-EPostcardAddressName, i ); // add right one
    			continue;
    		    }
    		}
		}			
	// Lets put them into right order
	for( TInt a = EPostcardAddressName; a<=EPostcardAddressCountry; a++ )
		{
		TInt id = indexes->At( a - EPostcardAddressName );
		if( id != -1 )
			{
			CContactItemField& field = set[ id ];
    		if( field.TextStorage( )->Text( ).Length( ) > 0 )
	    		{
            	strings->AppendL( field.TextStorage()->Text() );
	    		}
			}
		}
    CleanupStack::PopAndDestroy( indexes );
    // And create a transparent bitmap of the address strings
	CGulIcon* icon = CreateTransparentIconL( *strings );
	iController->SetBitmap( EPostcardRecipient, icon );

	CleanupStack::PopAndDestroy( strings );
    }

// ---------------------------------------------------------
// CreateGreetingBitmapL
// ---------------------------------------------------------
void CPostcardAppUi::CreateGreetingBitmapL( )
	{
    TPtrC16 text( iGreeting->Read( 0, iGreetingLength ) );
    
    CGulIcon* icon = CreateTransparentIconL( text );
   	iController->SetBitmap( EPostcardText, icon );
	}

// ---------------------------------------------------------
// CreateTransparentIconL
// ---------------------------------------------------------
CGulIcon* CPostcardAppUi::CreateTransparentIconL( const TDesC& aText )	
	{
	TSize iconSize = PostcardLaf::GreetingText( ).Size( );

	// The bitmap
	CFbsBitmap* bitmap = CreateEmptyBitmapLC( iconSize ); // pushes to stack

	// The mask bitmap
	CFbsBitmap* bitmapMask = new (ELeave) CFbsBitmap( );
	CleanupStack::PushL( bitmapMask );
	bitmapMask->Create( iconSize, EGray2 );
	
	CFbsBitmapDevice* deviceMask = CFbsBitmapDevice::NewL( bitmapMask );
	CleanupStack::PushL( deviceMask );
	
	CBitmapContext* contextMask = NULL;
	deviceMask->CreateBitmapContext( contextMask );	
	CleanupStack::PushL( contextMask );

    contextMask->SetPenStyle(CGraphicsContext::ENullPen);
    contextMask->SetBrushStyle(CGraphicsContext::ESolidBrush);
    contextMask->SetBrushColor( KRgbBlack );
    contextMask->DrawRect(TRect(TPoint(0,0), iconSize));

    contextMask->SetPenStyle(CGraphicsContext::ESolidPen);
    contextMask->SetBrushStyle(CGraphicsContext::ENullBrush);
    contextMask->SetPenColor(KRgbWhite);

	CFont* font = NULL;
    CParaFormat paraFormat;
    TParaFormatMask paraFormatMask;
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;

	PostcardLaf::MiniatureFont( charFormat, charFormatMask, paraFormat, paraFormatMask  );

	deviceMask->GetNearestFontToDesignHeightInTwips( font, charFormat.iFontSpec );
	
	contextMask->UseFont( font );

   	TInt top = PostcardLaf::BaselineTop( );
   	TInt delta = PostcardLaf::BaselineDelta( );
    TInt leftMargin = PostcardLaf::LeftMargin( );
    HBufC* buf = HBufC::NewLC( KPostcardMaxMiniatureCharsPerLine );
    TPtr strPtr = buf->Des( );

	TBool linesLeft = ETrue;
	TInt lineIndex = 0;
	TInt charTotalIndex = 0;
    TBool mirrored = AknLayoutUtils::LayoutMirrored();
    TInt lineWidth = iconSize.iWidth - leftMargin; // We use the same margin on right side, too
    const TInt greetingLineCount = PostcardLaf::GreetingFocusLineCount();

    // Insert lines until there is something to add or
    // maximum amount of greeting focus lines is reached.
    // This prevents drawing of text without a focus line,
    // which can happen for the last row of the greeting text.
	while( linesLeft && lineIndex < greetingLineCount )
		{
		strPtr.Zero( );
    	TPtrC origPtr = aText.Mid( charTotalIndex ); 
    	TInt index = 0;
    	while( index < origPtr.Length( ) 
    	    && font->TextWidthInPixels( origPtr.Mid( 0, index ) ) + leftMargin < lineWidth )
    		{
    		index++;
    		charTotalIndex++;
            TChar ch = origPtr[index-1];
            if( ch == 0x2029 ) // If enter, break from the while
                {
                charTotalIndex++; // increase now as it will be decreased next
                break;
                }
    		}
    	if( index < origPtr.Length( ) - 1 )
    	    { // This is the last character in the text so lets still include that
    	    index--;
    	    charTotalIndex--;
    	    }
    	if( index > 0 )
    		{ // There's actually something to draw on this line
    		strPtr.Append( origPtr.Mid( 0, index ) );
    		if( strPtr[index-1] == 0x2029 )
    		    { // Remove the last character if it's CR
                strPtr.Delete( index-1, 1 );
                index--;
                charTotalIndex--;
    		    }

            if( index == 0 )
                { // There are probably many empty lines so thats why we got here.. just increase line
			    lineIndex++;
    		    }
    		else
    		    { // Now we can draw it
        		TInt deltaX = leftMargin;
        		if( mirrored )
        		    {
        		    deltaX = iconSize.iWidth - leftMargin - font->TextWidthInPixels( strPtr );
        		    }
        		TPoint point = TPoint( deltaX, top + ( lineIndex * delta ) );
        		lineIndex++; // lets change the line
        		if( mirrored )
        		    {
        		    CGraphicsContext::TDrawTextExtendedParam params;
        		    params.iParRightToLeft = ETrue; // change the direction of text
            		contextMask->DrawTextExtended( strPtr, point, params );
        		    }
        		else
        		    {
            		contextMask->DrawText( strPtr, point );
        		    }
    		    }    
    		if( index >= origPtr.Length( ) )
    			{
    			linesLeft = EFalse;
    			}
    		}
		else  // Nothing left -> return
			{
			if( origPtr.Length( ) == 0)
			    { // Nothing left -> return
    			linesLeft = EFalse;	
			    }
			else
			    { // Just an empty line -> increase line counter
			    lineIndex++;
			    }
			}
		}

	CleanupStack::PopAndDestroy( 3, deviceMask );   // deviceMask, contextMask, buf
	
    CGulIcon* icon = CGulIcon::NewL( bitmap, bitmapMask );

    // Ownership moves, so just pop.
    CleanupStack::Pop( 2, bitmap ); // bitmapMask and bitmap

	return icon;
	}

// ---------------------------------------------------------
// CreateTransparentIconL
// ---------------------------------------------------------
CGulIcon* CPostcardAppUi::CreateTransparentIconL( const MDesCArray& aStrings )
	{
	TSize iconSize = PostcardLaf::Address( ).Size( );

	CFbsBitmap* bitmap = CreateEmptyBitmapLC( iconSize ); // pushes to stack
	
	CFbsBitmap* bitmapMask = new (ELeave) CFbsBitmap( );
	CleanupStack::PushL( bitmapMask );
	bitmapMask->Create( iconSize, EGray2 );
	
	CFbsBitmapDevice* deviceMask = CFbsBitmapDevice::NewL( bitmapMask );
	CleanupStack::PushL( deviceMask );
	
	CBitmapContext* contextMask = NULL;
	deviceMask->CreateBitmapContext( contextMask );	
	CleanupStack::PushL( contextMask );

    contextMask->SetPenStyle(CGraphicsContext::ENullPen);
    contextMask->SetBrushStyle(CGraphicsContext::ESolidBrush);
    contextMask->SetBrushColor( KRgbBlack );
    contextMask->DrawRect(TRect(TPoint(0,0), iconSize));

    contextMask->SetPenStyle(CGraphicsContext::ESolidPen);
    contextMask->SetBrushStyle(CGraphicsContext::ENullBrush);
    contextMask->SetPenColor(KRgbWhite);

	CFont* font = NULL;
    CParaFormat paraFormat;
    TParaFormatMask paraFormatMask;
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;

	PostcardLaf::MiniatureFont( charFormat, charFormatMask, paraFormat, paraFormatMask  );

	deviceMask->GetNearestFontToDesignHeightInTwips( font, charFormat.iFontSpec );
	
	contextMask->UseFont( font );

   	TInt top = PostcardLaf::BaselineTop( );
   	TInt delta = PostcardLaf::BaselineDelta( );
    TInt leftMargin = PostcardLaf::LeftMargin( );
    HBufC* buf = HBufC::NewLC( KPostcardMaxMiniatureCharsPerLine );
    TPtr strPtr = buf->Des( );
    TBool mirrored = AknLayoutUtils::LayoutMirrored();
    TInt ellipsisWidth = font->CharWidthInPixels( CEditableText::EEllipsis );
    TInt lineWidth = iconSize.iWidth - leftMargin - ellipsisWidth; // We use the same margin on right side, too
    TInt lines = aStrings.MdcaCount() > KPostcardMaxAddressFocusLines ? 
        KPostcardMaxAddressFocusLines : 
        aStrings.MdcaCount();
    // Following characters are converted to spaces, before showing them on text side.
    TBuf<1> replaceChars;
    replaceChars.Zero();
    replaceChars.Append( CEditableText::EParagraphDelimiter );
    
    for( TInt a = 0; a < lines; a++ )
    	{
    	strPtr.Zero( );
    	TPtrC origPtr = aStrings.MdcaPoint( a );
    	if( origPtr.Length( ) < 1 )
    		{ // There's no text so skip this..
    		continue;
    		}
    	TInt index = 0;
    	while( index < origPtr.Length( ) && font->TextWidthInPixels( origPtr.Mid( 0, index ) ) + leftMargin < ( lineWidth ) )
    		{
    		index++;
    		}
    	strPtr.Append( origPtr.Mid( 0, index ) );
    	if( index < origPtr.Length( ) )
    		{
    		strPtr.Append( CEditableText::EEllipsis );
    		index++;
    		}
    	TInt deltaX = leftMargin;
    	if( mirrored )
    	    {
            deltaX = iconSize.iWidth - font->TextWidthInPixels( strPtr ) - leftMargin;
    	    }
    	TPoint point = TPoint( deltaX, top + ( a * delta ) );
        
        // Convert all line changes to spaces
    	AknTextUtils::ReplaceCharacters(
            strPtr,
            replaceChars,
            CEditableText::ESpace );
    	
		if( mirrored )
		    {
		    CGraphicsContext::TDrawTextExtendedParam params;
		    params.iParRightToLeft = ETrue; // change the direction of text
    		contextMask->DrawTextExtended( strPtr, point, params );
		    }
		else
		    {
        	contextMask->DrawText( strPtr, point );
		    }
    	}
	
	CleanupStack::PopAndDestroy( 3, deviceMask );   // deviceMask, contextMask, buf
	
    CGulIcon* icon = CGulIcon::NewL( bitmap, bitmapMask );

    // Ownership moves, so just pop.
    CleanupStack::Pop( 2, bitmap ); // bitmapMask and bitmap

	return icon;

	}

// ---------------------------------------------------------
// RefreshFocusLinesL
// ---------------------------------------------------------
void CPostcardAppUi::RefreshFocusLinesL( )
    {
    // Lets refresh the location of lines in greeting and recipients rects
    if( !iController )
        {
        // If there's no controller -> just return
        return; 
        }
        
	CFbsBitmap* dummyBmp = new (ELeave) CFbsBitmap( );
	CleanupStack::PushL( dummyBmp );
	dummyBmp->Create( TSize( 1, 1), EGray2 );
	
	CFbsBitmapDevice* dummyDevice = CFbsBitmapDevice::NewL( dummyBmp );
	CleanupStack::PushL( dummyDevice );

	CFont* font = NULL;
    CParaFormat paraFormat;
    TParaFormatMask paraFormatMask;
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;

	PostcardLaf::MiniatureFont( charFormat, charFormatMask, paraFormat, paraFormatMask  );

	dummyDevice->GetNearestFontToDesignHeightInTwips( font, charFormat.iFontSpec );

    TInt descent = font->DescentInPixels( );
    
    CleanupStack::PopAndDestroy( 2, dummyBmp ); // dummyBmp, dummyDevice
    
 	TInt delta = PostcardLaf::BaselineDelta( );
	TInt top = PostcardLaf::BaselineTop( );
    TInt leftMargin = PostcardLaf::LeftMargin( );
    TInt extraHeight = MsgEditorCommons::ScreenWidth( )>KPostcardLegacyWidth?1:0;
    if( MsgEditorCommons::ScreenWidth( ) > KPostcardQvgaWidth )
        {
        extraHeight++;
        }
    
    TInt rectLoop = EPostcardFocusAreaGreeting;
    for ( rectLoop = 0; rectLoop < EPostcardFocusAreaLast; rectLoop++ )
        {
        TRect focusRect = rectLoop == EPostcardFocusAreaGreeting ? 
            PostcardLaf::GreetingText() :
            PostcardLaf::Address(); // Indicates the are where focus lines are drawn

    	// Indicates number of focus lines to draw. There is always 5 focus line in address side.
    	// Focus line count in greeting text side depends on the used layout.
        TInt rectLineCount = rectLoop == EPostcardFocusAreaGreeting ? 
            PostcardLaf::GreetingFocusLineCount() : 
            KPostcardMaxAddressFocusLines;
        CArrayPtrFlat<TRect>* lineRects = new (ELeave) CArrayPtrFlat<TRect>( rectLineCount );
        CleanupStack::PushL( lineRects );

        TRect* rect = NULL;
        for (TInt i = 0; i < rectLineCount; i++)
            {
            rect = new (ELeave) TRect;
            CleanupStack::PushL( rect );
            lineRects->InsertL( i, rect );
            CleanupStack::Pop( rect ); // rect -> just pop as it will be in lineRects
            }

        TInt maxHeight = focusRect.Height( );
        TInt lineNumber = 1;
        
        while( ETrue )
            {
            TInt y = focusRect.iTl.iY + top + ( lineNumber - 1 ) * delta + descent;
            
            lineRects->At( lineNumber - 1 )->SetRect( 
                                focusRect.iTl.iX + leftMargin,
                                y,
                                focusRect.iBr.iX - leftMargin,
                                y + extraHeight );
            if( lineNumber >= rectLineCount || top + lineNumber * delta > maxHeight )
                {
                break;
                }
            lineNumber++;
            } 
        // ownership moves to controller
        CleanupStack::Pop( lineRects );

        if( rectLoop == 0 )
            {
            iController->SetGreetingLines( *lineRects );
            }
        else
            {
            iController->SetRecipientLines( *lineRects );
            }
        }
    }

// ---------------------------------------------------------
// CreateEmptyBitmapLC
// ---------------------------------------------------------
CFbsBitmap* CPostcardAppUi::CreateEmptyBitmapLC( const TSize& aSize )
	{
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap( );
	CleanupStack::PushL( bitmap );
	bitmap->Create( aSize, EGray2 );
	
	CFbsBitmapDevice* device = CFbsBitmapDevice::NewL( bitmap );
	CleanupStack::PushL( device );
	
	CBitmapContext* context = NULL;
	device->CreateBitmapContext( context );	
	CleanupStack::PushL( context );

    context->SetPenStyle( CGraphicsContext::ENullPen );
    context->SetBrushStyle( CGraphicsContext::ESolidBrush );
    context->SetBrushColor( KRgbRed );
    context->DrawRect( TRect( TPoint( 0, 0 ), aSize ) );

    CleanupStack::PopAndDestroy( 2, device );   // context, device

	return bitmap;
	}


// ---------------------------------------------------------
// ShowGlobalInformationNoteL
// ---------------------------------------------------------
void CPostcardAppUi::ShowGlobalInformationNoteL( TInt aResourceId, TBool aWaiting )
    {
    CErrorUI* errorUi = CErrorUI::NewLC(*iCoeEnv);
    aWaiting ? errorUi->ShowGlobalErrorQueryL( aResourceId ) :
               errorUi->ShowGlobalErrorNoteL( aResourceId );
    CleanupStack::PopAndDestroy( errorUi );
    }

// ---------------------------------------------------------
// ShowInformationNoteL
// ---------------------------------------------------------
void CPostcardAppUi::ShowInformationNoteL( TInt aResourceId, TBool aWaiting )
    {
    HBufC* prompt = StringLoader::LoadLC( aResourceId, iCoeEnv );
    ShowInformationNoteL( *prompt, aWaiting );
    CleanupStack::PopAndDestroy( prompt );
    }

// ---------------------------------------------------------
// ShowInformationNoteL
// ---------------------------------------------------------
void CPostcardAppUi::ShowInformationNoteL( const TDesC& aText, TBool aWaiting )
    {
    CAknInformationNote* note = new ( ELeave )
        CAknInformationNote( aWaiting );
    note->ExecuteLD( aText );
    }

// ---------------------------------------------------------
// ShowConfirmationQueryL
// ---------------------------------------------------------
TInt CPostcardAppUi::ShowConfirmationQueryL( TInt aResourceId, TBool aOnlyOK/*=EFalse*/ ) const
    {
    HBufC* prompt = StringLoader::LoadLC( aResourceId, iCoeEnv );
    TInt retVal = ShowConfirmationQueryL( *prompt, aOnlyOK );
    CleanupStack::PopAndDestroy( prompt );
    return retVal;
    }

// ---------------------------------------------------------
// ShowConfirmationQueryL
// ---------------------------------------------------------
TInt CPostcardAppUi::ShowConfirmationQueryL( const TDesC& aText, TBool aOnlyOK/*=EFalse*/ ) const
    {
    TInt dialogType=aOnlyOK?R_POSTCARD_CONFIRMABLE_QUERY:R_POSTCARD_CONFIRMATION_QUERY;
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    TInt retVal = dlg->ExecuteLD( dialogType, aText );
    return retVal;    
    }

// ---------------------------------------------------------
// ShowWaitNoteL
// ---------------------------------------------------------
TBool CPostcardAppUi::ShowWaitNoteL( TInt aResourceId, TBool aNoTimeout )
    {
    TInt waitNoteResource = R_POSTCARD_WAIT_NOTE;

    HBufC* string = StringLoader::LoadLC( aResourceId, iCoeEnv );
    
    iWaitDialog = new( ELeave )CAknWaitDialog(
        reinterpret_cast<CEikDialog**>( &iWaitDialog ), aNoTimeout ); // ETrue means no delay in showing the note.
    iWaitDialog->PrepareLC( waitNoteResource );
    iWaitDialog->SetTextL( *string );
    TInt success = iWaitDialog->RunLD();
    CleanupStack::PopAndDestroy( string );
    return success;
    }

// ---------------------------------------------------------
// RemoveWaitNote
// ---------------------------------------------------------
void CPostcardAppUi::RemoveWaitNote()
    {
    delete iWaitDialog;
    iWaitDialog = NULL;
    }

// ---------------------------------------------------------
// Document
// ---------------------------------------------------------
CPostcardDocument& CPostcardAppUi::Document()
    {
    CMsgEditorDocument& doc = static_cast <CMsgEditorDocument&> ( *CEikAppUi::Document() );
    return static_cast <CPostcardDocument&> ( doc );
    }

// ---------------------------------------------------------
// ContactHasData
// ---------------------------------------------------------
TBool CPostcardAppUi::ContactHasData( const CContactCard& aContact )
	{
	CContactItemFieldSet& fields = aContact.CardFields( );
	for( TInt i = 0; i < fields.Count( ); i++ )
		{
		CContactItemField& field = fields[i];
		if( field.TextStorage( )->Text( ).Length( ) > 0 )
			{
			return ETrue;
			}
		}
	return EFalse;	
	}

// ---------------------------------------------------------
// ContactHasData
// ---------------------------------------------------------
TBool CPostcardAppUi::ContactHasData( )
    {
    // Call the overriden function with iContact
    if(!iContact)
    return EFalse;
    return ContactHasData( *iContact );    
    }


// ---------------------------------------------------------
// MandatoryAddressFields
// ---------------------------------------------------------
TInt CPostcardAppUi::MandatoryAddressFields( )
	{
    return iCenRep->Get( KPocaKeyMandatoryFields );
	}

// ---------------------------------------------------------
// ReadFromResourceLC
// ---------------------------------------------------------
HBufC* CPostcardAppUi::ReadFromResourceLC( TInt& aResourceId )
	{
    return StringLoader::LoadLC( aResourceId, iCoeEnv );	
	}

// ---------------------------------------------------------
// NavigateBetweenMessagesL
// ---------------------------------------------------------
TKeyResponse CPostcardAppUi::NavigateBetweenMessagesL( TBool aNextMessage )
	{
    if( IsNextMessageAvailableL( aNextMessage ) )
        {
        NextMessageL( aNextMessage );
        }
    return EKeyWasConsumed;
    }

// ---------------------------------------------------------
// CheckLengthsL
// ---------------------------------------------------------
TBool CPostcardAppUi::CheckLengthsL( )
	{
    if( iGreetingLength > MaxTextLength( EPostcardTextEditor ) )
        {
        DoEditGreetingL();
        return EFalse;    	    
        }

	if( !ContactHasData( *iContact ) )
		{
		// No contact -> no need to show anything..
    	return ETrue;
		}

    // Get length of address fields into a table. Table is initialized to
    // zero because contact may be missing some address fields. Lengths are
    // first fetched into a table and checked after that in order to do the
    // check in correct order. If a some fields are too long they are presented
    // to user in correct order for fixing.
    TInt lengths[CPostcardAddressForm::ENumAddrFields];
    Mem::FillZ( lengths, sizeof( lengths ) );
    CContactItemFieldSet& set = iContact->CardFields();
    for( TInt i = 0; i < set.Count(); i++ )
        {
        CContactItemField& field = set[i];
        CContactTextField* textField = set[i].TextStorage();
        if (textField)
            {
            TInt id = TPostcardUtils::IdFromContactItemName( field.Label() );
            __ASSERT_DEBUG( id >= EPostcardAddressName &&
                id <= EPostcardAddressCountry, Panic( EPostcardPanicCoding ) );
            lengths[id - EPostcardAddressName] = textField->Text().Length();
            }
        }
    // Check length of fields
    for( TInt i = EPostcardAddressName; i <= EPostcardAddressCountry; i++ )
        {
        if (lengths[i - EPostcardAddressName] > MaxTextLength(i))
            {
            // A field is too long, open the address dialog..
            DoEditAddressL( TKeyEvent(), i );
            return EFalse;
            }
        }
    return ETrue;
	}

// ---------------------------------------------------------
// CreateAddressStringForSendConfirmLC
// ---------------------------------------------------------
HBufC* CPostcardAppUi::CreateAddressStringForSendConfirmLC()
    {
    HBufC* buf = HBufC::NewLC( KPostcardMaxSendConfirmationLength );
    TPtr bufPtr = buf->Des();

    _LIT( KNewline, "\n" );
    CContactItemFieldSet& set = iContact->CardFields( );
    for(TInt i = EPostcardAddressName; i <= EPostcardAddressCountry; i++ )
        {
        const CContactItemField* field = TPostcardUtils::FieldOrNull( set,
            TPostcardUtils::ContactItemNameFromId( i ) );
        if ( field )
            {
            TPtrC text = field->TextStorage()->Text();
            TInt len = text.Length();
            if( len && len < ( bufPtr.MaxLength() - bufPtr.Length() ) )
                {
                bufPtr.Append( text );
                bufPtr.Append( KNewline );
                }
            }
        }

    // Remove the last "\n" in case there's question mark in the end of the loc string
    bufPtr.Delete( bufPtr.Length() - 1, 1 );
    return buf;
	}

// ---------------------------------------------------------
// QueryWaiterCallback
// ---------------------------------------------------------
void CPostcardAppUi::QueryWaiterCallbackL( TInt aResult )
	{
	if( aResult == EAknSoftkeySend )
		{
		// Proceed if send query result is send
		
		// Displaying extra charge query is optional
        aResult = EAknSoftkeyOk;
		if( iCenRep->FeatureBits() & KPostcardFeatureIdExtraChargeNotification)
		    {
            CAknQueryDialog* dlg = CAknQueryDialog::NewL();
            aResult = dlg->ExecuteLD( R_POSTCARD_EXTRA_CHARGE_QUERY );
            }
        if( aResult == EAknSoftkeyOk )
            {
            // OK, start sending
            
            TInt resourceId = R_POSTCARD_SENDING_POSTCARD;
            if( IsPhoneOfflineL() )
                {
                resourceId = R_POSTCARD_SAVED_OUTBOX;
                }            
            
            ShowWaitNoteL( resourceId, ETrue );
            iOperation = CPostcardOperationSave::NewL(
                *this,
                Document(),
                *this,
                FsSession(),
                *iContact,
                *iGreeting
    	        );
            iOperation->Start( ETrue );
            
            iAbsorber = CAknInputBlock::NewLC();
            CleanupStack::Pop( iAbsorber );
            iPocaFlags |= EPostcardClosing;
            }
		}

	if( iMessageQuery )
		{
        iMessageQuery->CancelMsgQuery();
        delete iMessageQuery;
        iMessageQuery = NULL;		
		}

    delete iSendText;
    iSendText = NULL;
	}

// ---------------------------------------------------------
// QueryWaiterCallbackError
// ---------------------------------------------------------
void CPostcardAppUi::QueryWaiterCallbackError()
    {
    // Cleanup after QueryWaiterCallbackL() leaves

	if( iMessageQuery )
		{
        iMessageQuery->CancelMsgQuery();
        delete iMessageQuery;
        iMessageQuery = NULL;		
		}

    delete iSendText;
    iSendText = NULL;

    RemoveWaitNote();

    delete iOperation;
    iOperation = NULL;
    }

// ---------------------------------------------------------
// SetMiddleSoftkeyL
// ---------------------------------------------------------
void CPostcardAppUi::SetMiddleSoftkeyL()
    {
    TInt resourceId;
    if ( iController->Frontpage() )
        {
        // Frontside is focused
        if ( iMsgType == EPostcardSent )
            {
            resourceId = R_POSTCARD_MSK_BUTTON_EMPTY;
            }
        else
            {
            resourceId = R_POSTCARD_MSK_BUTTON_OPTIONS;
            MenuBar()->SetContextMenuTitleResourceId(
                R_POSTCARD_CONTEXT_MENUBAR );
            }
        }
    else
        {
        // Backside is focused
        resourceId = iMsgType == EPostcardSent ?
            R_POSTCARD_MSK_BUTTON_OPEN : R_POSTCARD_MSK_BUTTON_EDIT;
        }
    if ( resourceId != iMskResource )
        {
        const TInt KMskPosition = 3;
        CEikButtonGroupContainer *cba = Cba();
        cba->SetCommandL( KMskPosition, resourceId );
        cba->DrawDeferred();
        iMskResource = resourceId;
        }
    }

// ---------------------------------------------------------
// GetZoomLevelL
// ---------------------------------------------------------
void CPostcardAppUi::GetZoomLevelL( TInt& aZoomLevel )
    {
    TInt err = iMsgEditorAppUiExtension->iSettingCachePlugin.GetValue( KAknLocalZoomLayoutSwitch, 
                                                            aZoomLevel );
                
    if ( err != KErrNone )
        {
        User::Leave( err );
        }
    }

// ---------------------------------------------------------
// HandleLocalZoomChangeL
// ---------------------------------------------------------
//
void CPostcardAppUi::HandleLocalZoomChangeL( TMsgCommonCommands aNewZoom )
    {
    // Forward to base, which sets new zoom level to cenrep.
    CMsgEditorAppUi::HandleLocalZoomChangeL( aNewZoom );
    }

// ---------------------------------------------------------
// CPostcardAppUi::BeginActiveWait
// ---------------------------------------------------------
//
void CPostcardAppUi::BeginActiveWait()
    {
    if ( iWait.IsStarted() )
        {
        return;
        }
    iWait.Start();
    }

// ---------------------------------------------------------
// CPostcardAppUi::EndActiveWait
// ---------------------------------------------------------
//
void CPostcardAppUi::EndActiveWait()
    {
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
    }

// ---------------------------------------------------------
// CPostcardAppUi::SetTitleIconL
// ---------------------------------------------------------
//
void CPostcardAppUi::SetTitleIconL()
    {
    SetTitleIconSizeL( iAppIcon->Bitmap() );
    
    // Create duplicates of the icon to be used
    CFbsBitmap* bitmap = new( ELeave ) CFbsBitmap();
    CleanupStack::PushL( bitmap );
    
    CFbsBitmap* bitmapMask = new( ELeave ) CFbsBitmap();
    CleanupStack::PushL( bitmapMask );
    
    User::LeaveIfError( bitmap->Duplicate( iAppIcon->Bitmap()->Handle() ) );
    User::LeaveIfError( bitmapMask->Duplicate( iAppIcon->Mask()->Handle() ) );

    iTitlePane->SetSmallPicture( bitmap, bitmapMask, ETrue );
    iTitlePane->DrawNow();
    
    CleanupStack::Pop( bitmapMask );
    CleanupStack::Pop( bitmap );
    }

// ---------------------------------------------------------
// CPostcardAppUi::CreateAndSetTitleIconL
// ---------------------------------------------------------
//
void CPostcardAppUi::CreateAndSetTitleIconL()
    {
    if ( iAppIcon )
        {
        delete iAppIcon;
        iAppIcon = NULL;
        }
    
    TParse parseDrive;
    parseDrive.Set( Document().AppFullName( ), NULL, NULL );
    TPtrC drive = parseDrive.Drive( );
    
    TParse fileParse;
    fileParse.Set( KPostcardMifFile, &KDC_APP_BITMAP_DIR, NULL );
    
    TFileName fileName( fileParse.FullName( ) );
    fileName.Insert( 0, drive );
    
    iAppIcon = AknsUtils::CreateGulIconL( 
        AknsUtils::SkinInstance(), 
        KAknsIIDQgnPropMcePostcardTitle,
        fileParse.FullName(),
        EMbmPostcardQgn_prop_mce_postcard_title,
        EMbmPostcardQgn_prop_mce_postcard_title_mask );
    
    SetTitleIconL();
    }

// ---------------------------------------------------------
// CPostcardAppUi::IsPhoneOfflineL
// ---------------------------------------------------------
//
TBool CPostcardAppUi::IsPhoneOfflineL() const
    {
    if ( iPocaFlags & EPostcardFeatureOffline )
        {    
        return MsvUiServiceUtilitiesInternal::IsPhoneOfflineL();
        }
    else
        {
        return EFalse;
        }       
    }

//  End of File
