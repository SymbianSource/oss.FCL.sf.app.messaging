/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of Messaging Settings GS Plugin
*
*/



// INCLUDE FILES
#include <aknnotedialog.h>      // Popup note
#include <bautils.h>            // BaflUtils
#include <gsprivatepluginproviderids.h> // KGSPluginProviderInternal
#include <gsplugininterface.h>
#include <gsfwviewuids.h>       // KGSAppsPluginUid
#include <ecom/implementationproxy.h>
#include <featmgr.h>            // Featuremanager
#include <MceSettingsDialog.h>
#include <MceSettingsArrays.h>
#include <hlplch.h>             // HlpLauncher
#include <muiulock.h>           // CMuiuLock to lock settings
#include <ConeResLoader.h>
#include <StringLoader.h>       // StringLoader

#include <mcesettingsgspluginicons.mbg> // own bitmaps
#include <MceSettingsGSPluginRsc.rsg>   // own resources
#include <aknclearer.h>         // CAknLocalScreenClearer
#include "MceSettingsGSPlugin.hrh"
#include "MceSettingsGSPlugin.h"        // own header
#include "MceSettingsGSPluginContainer.h"

// CONSTANTS
_LIT( KMceSettingsGSPluginRscFile, "mcesettingsgspluginrsc.rsc" );
_LIT( KMceSetGSPluginMbmFile, "mcesettingsgspluginicons.mif");



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMceSettingsGSPlugin::CMceSettingsGSPlugin()
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMceSettingsGSPlugin::CMceSettingsGSPlugin()
    {
    }


// ---------------------------------------------------------------------------
// CMceSettingsGSPlugin::~CMceSettingsGSPlugin()
// Destructor
// ---------------------------------------------------------------------------
//
CMceSettingsGSPlugin::~CMceSettingsGSPlugin()
    {
    FeatureManager::UnInitializeLib();
    delete iSettingsLock;
    delete iContainer;
    delete iLocalScreenClearer;
    }


// ---------------------------------------------------------------------------
// CMceSettingsGSPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMceSettingsGSPlugin::ConstructL()
    {
    // preparing resources for use
    TParse parse;
    parse.Set(KMceSettingsGSPluginRscFile, &KDC_RESOURCE_FILES_DIR, NULL);
    TFileName fileName = parse.FullName();
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
    iResourceLoader.OpenL( fileName );
    // base construct (base needs the resources to be loaded 1st)
    BaseConstructL( R_MCEUI_MAIN_VIEW );
    FeatureManager::InitializeLibL();
    }


// ---------------------------------------------------------------------------
// CMceSettingsGSPlugin::NewL()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMceSettingsGSPlugin* CMceSettingsGSPlugin::NewL( TAny* /*aAppUi*/ )
    {
    CMceSettingsGSPlugin* self = new( ELeave ) CMceSettingsGSPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CMceSettingsGSPlugin::Id()
// See header for details.
// ---------------------------------------------------------------------------
//
TUid CMceSettingsGSPlugin::Id() const
    {
    return KGSMceSettingsGSPluginUid;
    }


// ---------------------------------------------------------------------------
// CMceSettingsGSPlugin::NewContainerL
// Creates new iContainer.
// ---------------------------------------------------------------------------
//
void CMceSettingsGSPlugin::NewContainerL()
    {
    iContainer = new( ELeave ) CMceSettingsGSPluginContainer;
    }


// ---------------------------------------------------------------------------
// CMceSettingsGSPlugin::DoActivate
// Activate this view
// ---------------------------------------------------------------------------
//  
void CMceSettingsGSPlugin::DoActivateL( 
    const TVwsViewId& aPrevViewId,
    TUid aCustomMessageId,
    const TDesC8& aCustomMessage )
    {
    iAppUi->AddViewDeactivationObserverL( this );        

    iSettingsLock = CMuiuLock::NewL( KMuiuLockSettings);

    // Reserve settings
    TInt err = iSettingsLock->Reserve();
    if ( !err )
        {
        CGSBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
        }
    else
        {
        // Show information note: MCE has settings opened
        CAknNoteDialog* dlg = new (ELeave) CAknNoteDialog(
            CAknNoteDialog::ENoTone, CAknNoteDialog::ELongTimeout);
        CleanupStack::PushL( dlg );                
        dlg->ExecuteLD(R_MCE_NOTE_SETTINGS_OPEN_MCE);
        CleanupStack::Pop(); // dlg
        iLocalScreenClearer = CAknLocalScreenClearer::NewL( EFalse );
        iAppUi->ActivateLocalViewL( KGSAppsPluginUid );
        }
    }


// ---------------------------------------------------------------------------
// CMceSettingsGSPlugin::DoDeactivate
// Deactivate this view
// ---------------------------------------------------------------------------
//
void CMceSettingsGSPlugin::DoDeactivate()
    {
    // Release repository
    if ( iSettingsLock ) 
    	{
		// Settings lock cleaned up
        iSettingsLock->Release();
    	delete iSettingsLock;
    	iSettingsLock = NULL;	
    	}
		
    // iLocalScreenClearer cleaned up
    if ( iLocalScreenClearer ) 
        {
        delete iLocalScreenClearer;
        iLocalScreenClearer = NULL;
        }
    CGSBaseView::DoDeactivate();
    }

    
// ---------------------------------------------------------------------------
// CMceSettingsGSPlugin::HandleViewDeactivation()
// See header for details.
// ---------------------------------------------------------------------------
//
void CMceSettingsGSPlugin::HandleViewDeactivation(
    const TVwsViewId& /*aViewIdToBeDeactivated*/,
    const TVwsViewId& /*aNewlyActivatedViewId*/)
    {

    }


// ---------------------------------------------------------------------------
// CMceSettingsGSPlugin::HandleCommandL()
// See header for details.
// ---------------------------------------------------------------------------
//
void CMceSettingsGSPlugin::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EMceSettingsCmdEdit:
            {
            static_cast<CMceSettingsGSPluginContainer*>(iContainer)->EditSubSettingsL();
            break;
            }
            
        case EAknSoftkeyBack:
            {
            iAppUi->ActivateLocalViewL( KGSAppsPluginUid );
            break;
            }
        case EMceSettingsGSHelp:
        case EAknCmdHelp:
            {
            if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL(
                    iEikonEnv->WsSession(), iAppUi->AppHelpContextL() );
                }
            break;
            }                       
        default:
            {
            iAppUi->HandleCommandL( aCommand );
            break;
            }
        }
    }


// ---------------------------------------------------------------------------
// CMceSettingsGSPlugin::GetCaptionL
// Caption text for view to be displayed in NaviPane.
// ---------------------------------------------------------------------------
//
void CMceSettingsGSPlugin::GetCaptionL( TDes& aCaption ) const
    {
    // the resource file is already opened.
    HBufC* result = StringLoader::LoadL( R_QTN_SET_FOLDER_MESSAGING, iEikonEnv );
    aCaption.Copy( *result );
    delete result;
    }


// ---------------------------------------------------------------------------
// CPSUIGSPlugin::PluginProviderCategory()
// See header for details.
// ---------------------------------------------------------------------------
//
TInt CMceSettingsGSPlugin::PluginProviderCategory() const
    {
    return KGSPluginProviderInternal;
    }

 
// ---------------------------------------------------------------------------
// CMceSettingsGSPlugin::CreateIconL()
// See header for details.
// ---------------------------------------------------------------------------
//
CGulIcon* CMceSettingsGSPlugin::CreateIconL( const TUid aIconType )
    {
    CGulIcon* icon;

    TParse parse;
    parse.Set(KMceSetGSPluginMbmFile, &KDC_BITMAP_DIR, NULL);
    TPtrC iconFile = parse.FullName();

    if( aIconType == KGSIconTypeLbxItem )
        {
        icon = AknsUtils::CreateGulIconL(
            AknsUtils::SkinInstance(),
            KAknsIIDQgnPropSetAppsMce,
            iconFile,
            EMbmMcesettingsgspluginiconsQgn_prop_set_apps_mce,
            EMbmMcesettingsgspluginiconsQgn_prop_set_apps_mce_mask );

        }
     else
        {
        icon = CGSPluginInterface::CreateIconL( aIconType );
        }

    return icon;
    }


// ---------------------------------------------------------------------------
// CMceSettingsGSPlugin::HandleListBoxSelectionL
// Handle any user actions while in the list view.
// ---------------------------------------------------------------------------
//
void CMceSettingsGSPlugin::HandleListBoxSelectionL()
    {
    }
    
    
// ---------------------------------------------------------------------------
// CMceSettingsGSPlugin::DynInitMenuPaneL()
// See header for details.
// ---------------------------------------------------------------------------
//
void CMceSettingsGSPlugin::DynInitMenuPaneL( TInt /* aResourceId */, CEikMenuPane* /* aMenuPane */)
    {
    }
    
    
// ---------------------------------------------------------------------------
// CMceSettingsGSPlugin::HandleListBoxEventL()
// See header for details.
// ---------------------------------------------------------------------------
//
void CMceSettingsGSPlugin::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {    
    switch(aEventType)
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            HandleCommandL( EMceSettingsCmdEdit );
        break;
        default:
        break;
        }
    }

// End of file
