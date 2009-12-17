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
*       Class implementation file
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <msvapi.h>                 // CMsvSession
#include <eikfrlb.h>                // CEikFormattedCellListBox
#include <eikappui.h>
#include <avkon.hrh>                // EAknSoftkeyCancel
#include <aknappui.h>               // CAknAppUi
#include <AknsDrawUtils.h>
#include <eikspane.h>               // CEikStatusPane
#include <akntitle.h>               // CAknTitlePane
#include <hlplch.h>                 // HlpLauncher
#include <msvids.h>                 // KMsvNullIndexEntryId
#include <muiuflagger.h>            // CFlags
#include <eikrted.h>                // CEikRichTextEditor
#include <txtrich.h>                // CRichText
#include <f32file.h>                // TParse
#include <ImumInSettingsData.h>     // CImumInSettingsData
#include <bldvariant.hrh>
#include <csxhelp/mce.hlp.hrh>
#include <ImumUtils.rsg>
#include <AiwServiceHandler.h>      //CAiwServiceHandler
#include <AiwCommon.h>              //CAiwCriteriaItem
#include <AiwVariant.h>             //TAiwVariant
#include <AknIconArray.h>
#include <aknlists.h>
#include <pathinfo.h>
#include <muiu.mbg>
#include <data_caging_path_literals.hrh>
#include <aknlayoutscalable_apps.cdl.h>

#include <aknViewAppUi.h>           //CAknViewAppUi
#include <akninputblock.h>          // CAknInputBlock
#include <eikapp.h>                 // CEikApplication

#include "ImumInternalApiImpl.h"    // CImumInternalApiImpl
#include "IMSSettingsUi.h"
#include "IMSSettingsBaseUi.h"
#include "IMSSettingsWizard.h"
#include "IMSSettingsDialog.h"
#include "ImumMboxManager.h"
#include "ImumPanic.h"
#include "EmailFeatureUtils.h"
#include "EmailUtils.H"
#include "ImumUtilsLogging.h"
#include "IMSWizardControl.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
const TInt KIMSOptionsMenuItemCount = 4;

const TInt KIMSMaxEmailAddressSize = 100;

_LIT( KMuiuBitmapFile, "z:muiu.MBM" );

const TInt KMceUid = 0x100058C5;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIMSSettingsUi::NewAccountL
// ----------------------------------------------------------------------------
//
EXPORT_C TMsvId CIMSSettingsUi::NewAccountL(
    TImumUiExitCodes& aExitCode,
    CImumInSettingsData& aMailboxSettings,
    CMsvSession& aMsvSession )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsUi::NewAccountL, 0, utils, KLogUi );

    return NewAccountL( aExitCode,
			aMailboxSettings,
			aMsvSession,
			EFalse );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsUi::NewAccountL
// ----------------------------------------------------------------------------
//
EXPORT_C TMsvId CIMSSettingsUi::NewAccountL(
    TImumUiExitCodes& aExitCode,
    CImumInSettingsData& aMailboxSettings,
    CMsvSession& aMsvSession,
    TBool aDefaultWizOnly )
    {
    CEikApplication* app = reinterpret_cast<CAknViewAppUi*>(
        CEikonEnv::Static()->EikAppUi())->Application();

    TMsvId emailID = 0;
    TInt err = KErrGeneral;
    RBuf email;
    email.Create( KIMSMaxEmailAddressSize );
    CleanupClosePushL( email );

    if( !aDefaultWizOnly )
    	{
    	// If tried to launch outside of MCE, external-wizard shouldn't be launched
    	if ( app->AppDllUid() == TUid::Uid( KMceUid ) )
    		{
    		TRAP( err, LaunchExtWizardL( email, emailID ) );
    		}
    	}

    // Check how the launch ended, if no external wizard was found
    // err is set to KErrGeneral
    if ( err == KErrCancel )
    	{
    	aExitCode = EImumUiNoChanges;
    	}
    // Red key pressed while in external-wizard
    else if ( err == KLeaveExit )
    	{
    	aExitCode = EImumUiExit;
    	}
    else if ( err == KErrNone )
    	{
    	aExitCode = EImumUiExtWizardCreated;
    	aMailboxSettings.SetAttr( TImumDaSettings::EKeyMailboxId, emailID );
    	}
    else
    	{
    	CAknInputBlock::NewLC();
    	StartL(
    			R_IMAS_SETTINGS_WIZARD_DIALOG,
                aExitCode,
                aMailboxSettings,
                aMsvSession,
                email );
    	CleanupStack::PopAndDestroy(); // CAknInputBlock
    	}

    CleanupStack::PopAndDestroy(); // email

    return emailID;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsUi::EditAccountL
// ----------------------------------------------------------------------------
//
EXPORT_C void CIMSSettingsUi::EditAccountL(
    TImumUiExitCodes& aExitCode,
    CImumInSettingsData& aMailboxSettings,
    CMsvSession& aMsvSession )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsUi::EditAccountL, 0, utils, KLogUi );

    TBufC<1> dummy; // CSI: 47 # Dummy TBuf.

    StartL(
        R_IMAS_SETTINGS_DIALOG,
        aExitCode,
        aMailboxSettings,
        aMsvSession,
        dummy );
    }

// ---------------------------------------------------------------------------
// CIMSSettingsUi::StartL()
// ---------------------------------------------------------------------------
//
void CIMSSettingsUi::StartL(
    TInt aResource,
    TImumUiExitCodes& aExitCode,
    CImumInSettingsData& aMailboxSettings,
    CMsvSession& aMsvSession,
    const TDesC& aEmailAddress )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsUi::StartL, 0, utils, KLogUi );

    // Create the dialog
    CIMSSettingsUi* self =
        new ( ELeave ) CIMSSettingsUi( aExitCode, aMailboxSettings );
    CleanupStack::PushL( self );
    self->SettingsConstructL( aResource, aMsvSession, aEmailAddress );
    CleanupStack::Pop( self );

    self->ExecuteLD( aResource );
    }


// ----------------------------------------------------------------------------
// CIMSSettingsUi::CIMSSettingsUi
// ----------------------------------------------------------------------------
//
CIMSSettingsUi::CIMSSettingsUi(
    TImumUiExitCodes& aExitCode,
    CImumInSettingsData& aMailboxSettings )
    :
    iFlags( NULL ),
    iExitCode( aExitCode ),
    iMailboxSettings( aMailboxSettings ),
    iListBox( NULL ),
    iSettings( NULL ),
    iResourceLoader( *iCoeEnv ),
    iEmailAddress( NULL ),
    iWizardContainer( NULL )
    {
    IMUM_CONTEXT( CIMSSettingsUi::CIMSSettingsUi, 0, KLogUi );

    }

// ----------------------------------------------------------------------------
// CIMSSettingsUi::~CIMSSettingsUi()
// ----------------------------------------------------------------------------
//
CIMSSettingsUi::~CIMSSettingsUi()
    {
    IMUM_CONTEXT( CIMSSettingsUi::~CIMSSettingsUi, 0, KLogUi );

    iResourceLoader.Close();
    delete iSettings;
    iSettings = NULL;

    // Prevents CONE 8 panics.
    if ( iFlags->Flag( EImumSettingsWizard ) )
        {
        // We created the listbox --> we delete it.
        delete iListBox;
        iListBox = NULL;
        }
    else
        {
        // Control function created the listbox
        // --> we don't own it --> we don't delete it
        iListBox = NULL;
        }


    delete iFlags;
    iFlags = NULL;

    delete iMailboxApi;
    iMailboxApi = NULL;
    delete iEmailAddress;
    iEmailAddress = NULL;

    // not owned:
    iWizardContainer = NULL;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsUi::SettingsConstructL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsUi::SettingsConstructL(
    const TInt aResource,
    CMsvSession& aMsvSession,
    const TDesC& aEmailAddress )
    {
    IMUM_CONTEXT( CIMSSettingsUi::SettingsConstructL, 0, KLogUi );

    iEmailAddress = aEmailAddress.AllocL();

    // Create connection to mailbox interface
    iMailboxApi = CImumInternalApiImpl::NewL( &aMsvSession );

    // Create flags object
    iFlags = MsvEmailMtmUiFeatureUtils::EmailFeaturesLC( ETrue, ETrue );
    CleanupStack::Pop( iFlags );

    if ( aResource == R_IMAS_SETTINGS_WIZARD_DIALOG )
        {
        iFlags->SetFlag( EImumSettingsWizard );
        iResourceMenuBar = R_IMAS_SETTINGS_WIZARD_MENUBAR;
        }
    else
        {
        iResourceMenuBar = R_IMAS_MENUBAR;
        }

    // Call the base ConstructL
    ConstructL( iResourceMenuBar );

    MsvEmailMtmUiUtils::LoadResourceFileL( iResourceLoader );
    }

// ---------------------------------------------------------------------------
// From class .
// CIMSSettingsUi::ExitCode()
// ---------------------------------------------------------------------------
//
TImumUiExitCodes CIMSSettingsUi::ExitCode()
    {
    IMUM_CONTEXT( CIMSSettingsUi::ExitCode, 0, KLogUi );

    if ( iFlags->Flag( EImumSettingShouldClose ) )
        {
        return iFlags->Flag( EImumSettingSave ) ?
            EImumUiSaveAndClose : EImumUiClose;
        }
    else if ( iFlags->Flag( EImumSettingShouldExit ) )
        {
        return iFlags->Flag( EImumSettingSave ) ?
            EImumUiSaveAndExit : EImumUiExit;
        }
    else
        {
        __ASSERT_DEBUG(
            EFalse, User::Panic( KIMSSettingsDialogPanic, KErrUnknown ) );
        return EImumUiNoChanges;
        }
    }


// ----------------------------------------------------------------------------
// CIMSSettingsUi::OkToExitL
// ----------------------------------------------------------------------------
//
TBool CIMSSettingsUi::OkToExitL( TInt aButtonId )
    {
    IMUM_CONTEXT( CIMSSettingsUi::OkToExitL, 0, KLogUi );

    TBool allowQuit = EFalse;

    // Exit not issued, forward the call to command processor
    if ( !iFlags->Flag( EImumSettingShouldExit ) &&
         !iFlags->Flag( EImumSettingShouldClose ) )
        {
        iSettings->ProcessCommandL( aButtonId );
        }
    else
        {
        // Should quit
        allowQuit = iSettings->OkToExitL( aButtonId );

        // Clear the flag after usage
        iFlags->ClearFlag( EImumSettingQueryClose );

        // If quit is allowed, store the settings, otherwise clear the flag
        if ( allowQuit )
            {
            // Failing to store the settings should cause panic, to prevent
            // the settings to stop into unwanted errors
            TRAP_IGNORE( iSettings->StoreSettingsToAccountL(
                iMailboxSettings ) );

            // Set the exitcode
            iExitCode = ExitCode();
            }
        else
            {
            // Clear the flags
            iFlags->ClearFlag( EImumSettingShouldExit );
            iFlags->ClearFlag( EImumSettingShouldClose );
            }

        // Exit, now
        if ( allowQuit )
            {
            allowQuit = CAknDialog::OkToExitL( EAknSoftkeyCancel );
            }
        }

    return allowQuit;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsUi::HandleResourceChange
// ----------------------------------------------------------------------------
//
void CIMSSettingsUi::HandleResourceChange( TInt aType )
    {
    IMUM_CONTEXT( CIMSSettingsUi::HandleResourceChange, 0, KLogUi );

    CAknDialog::HandleResourceChange( aType );

    if ( iListBox && aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect rect = Rect();
        iListBox->SetRect( rect );
        iListBox->HandleResourceChange( aType );
        }
    }

// ----------------------------------------------------------------------------
//  CIMSSettingsUi::SetSizeAndPosition()
// ----------------------------------------------------------------------------
//
void CIMSSettingsUi::SetSizeAndPosition( const TSize& aSize )
    {
    CAknDialog::SetSizeAndPosition( aSize );

    // this is fail safe check because iWizardContainer is created
    // in by framework not in constuctors
    if ( iWizardContainer )
        {
        iWizardContainer->SetRect(Rect());
        }

    }

// ----------------------------------------------------------------------------
// CIMSSettingsUi::PreLayoutDynInitL
// ----------------------------------------------------------------------------
//
void CIMSSettingsUi::PreLayoutDynInitL()
    {
    IMUM_CONTEXT( CIMSSettingsUi::PreLayoutDynInitL, 0, KLogUi );


    // Get the pointer to titlepane
    CEikStatusPane* sp =
        static_cast<CAknAppUi*>(
            iEikonEnv->EikAppUi() )->StatusPane();
    CAknTitlePane* title = static_cast<CAknTitlePane*>(
        sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    CEikButtonGroupContainer& cba = ButtonGroupContainer();

    // In case no email account created, start the wizard, otherwise
    // launch the normal settings menu
    if ( iFlags->Flag( EImumSettingsWizard ) )
        {
        // Prepare the dummy listbox, needed by CIMSSettingsWizard and its
        // super classes.
        iListBox = new (ELeave) CEikFormattedCellListBox;
        iListBox->ConstructL( this, EAknListBoxSelectionList );

        iSettings = CIMSSettingsWizard::NewL(
            *this, *iMailboxApi,
            *iListBox, *iWizardContainer, *title, cba, *iFlags, *iEmailAddress );

        MAknsControlContext* context =
            AknsDrawUtils::ControlContext( iListBox );

        // iWizardContainer is created by framework not in constructors
        // leave if it not created
        if ( iWizardContainer == NULL )
            {
            User::Leave(0);
            }
        iWizardContainer->SetBackgroundContext( context );
        }
    else
        {
        // Prepare the listbox
        iListBox = static_cast<CEikFormattedCellListBox*>(
            Control( EImasCIDListBox ) );

        //Check if mail settings are locked
        if( iFlags->GF( EMailFeatureIdSapPolicyManagement ) )
            {
            LoadListboxIconArrayL();
            }

        iSettings = CIMSSettingsDialog::NewL(
            *this, *iMailboxApi,
            *iListBox, *title, cba, *iFlags, iMailboxSettings );
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsUi::LaunchExtWizardL
// ----------------------------------------------------------------------------
//
void CIMSSettingsUi::LaunchExtWizardL( TDes& aEmailAddress, TMsvId& aMailboxId )
    {
    CAiwGenericParamList* outParamList = CAiwGenericParamList::NewLC();

    // Create AIW service handler
    CAiwServiceHandler* serviceHandler = CAiwServiceHandler::NewLC();

    // Attach the interest to the AIW framework.
    serviceHandler->AttachL( R_AIW_EMAIL_CRITERIA );

    serviceHandler->ExecuteServiceCmdL(
        KAiwCmdSettingWizardEmailView,
        serviceHandler->InParamListL(),
        *outParamList );

    TInt error = KErrGeneral;
    if ( outParamList->Count() )
        {
        TInt paramPos = 0; //Just to send something
        const TAiwGenericParam* errorParam =
            outParamList->FindFirst( paramPos, EGenericParamError );

        error = errorParam->Value().AsTInt32();
        if ( error == KErrGeneral )
            {
            const TAiwGenericParam* emailParam = outParamList->FindFirst(
                paramPos,
                EGenericParamEmailAddress );
			if ( emailParam )
				{
	            aEmailAddress = emailParam->Value().AsDes();
				}
            }
        else if ( error == KErrNone )
            {
            const TAiwGenericParam* idParam = outParamList->FindNext( paramPos,
                EGenericParamMessageItemMbox );
            aMailboxId = idParam->Value().AsTInt32();
            }
        }

    CleanupStack::PopAndDestroy( 2, outParamList ); // CSI: 47 # serviceHandler, outParamList.
    serviceHandler = NULL;
    outParamList = NULL;

    User::LeaveIfError( error );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsUi::ProcessCommandL
// ----------------------------------------------------------------------------
//
void CIMSSettingsUi::ProcessCommandL( TInt aCommandId )
    {
    IMUM_CONTEXT( CIMSSettingsUi::ProcessCommandL, 0, KLogUi );

    // Forward the commands
    CAknDialog::ProcessCommandL( aCommandId );

    // Forward command either to wizard or setting dialog
    iSettings->ProcessCommandL( aCommandId );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsUi::OfferKeyEventL
// ----------------------------------------------------------------------------
//
TKeyResponse CIMSSettingsUi::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    IMUM_CONTEXT( CIMSSettingsUi::OfferKeyEventL, 0, KLogUi );


    // Offer the key event to existing mailbox setting handler
    TKeyResponse ret = iSettings->OfferKeyEventL( aKeyEvent, aType );

    if ( ret != EKeyWasConsumed )
        {
        ret = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
        }

    return ret;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsUi::DynInitMenuPaneL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsUi::DynInitMenuPaneL(
    TInt /* aResourceId */,
    CEikMenuPane* aMenuPane )
    {
    IMUM_CONTEXT( CIMSSettingsUi::DynInitMenuPaneL, 0, KLogUi );

    if ( aMenuPane->NumberOfItemsInPane() == KIMSOptionsMenuItemCount )
        {
        aMenuPane->SetItemDimmed(
            EAknSoftkeyOpen, !iFlags->Flag( EImumSettingPositionSubMenu ) );
        aMenuPane->SetItemDimmed(
            EAknSoftkeySelect, iFlags->Flag( EImumSettingPositionSubMenu ) );
        aMenuPane->SetItemDimmed(
            EAknCmdHelp, !iFlags->GF( EMailFeatureHelp ) );
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsUi::DoQuitL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsUi::DoQuitL()
    {
    IMUM_CONTEXT( CIMSSettingsUi::DoQuitL, 0, KLogUi );

    // Start the exit process, the query is shown during TryExitL call
    if ( iFlags->Flag( EImumSettingShouldExit ) )
        {
        TryExitL( EAknSoftkeyExit );
        }
    else
        {
        TryExitL( EAknSoftkeyCancel );
        }
    }


// ----------------------------------------------------------------------------
// CIMSSettingsUi::GetHelpContext()
// ----------------------------------------------------------------------------
//
void CIMSSettingsUi::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    IMUM_CONTEXT( CIMSSettingsUi::GetHelpContext, 0, KLogUi );

    iSettings->GetHelpContext( aContext );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsUi::LaunchHelp()
// ----------------------------------------------------------------------------
//
void CIMSSettingsUi::LaunchHelp()
    {
    IMUM_CONTEXT( CIMSSettingsUi::LaunchHelp, 0, KLogUi );

    CCoeAppUi* appUi = static_cast<CCoeAppUi*>( ControlEnv()->AppUi() );
    // codescanner gives false positive here for leaving function called in
    // non-leaving function as it does not detect the trap in previous line
    TRAP_IGNORE( HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
    		appUi->AppHelpContextL() ) ); // CSI: 42 # see comment above
    }

// ----------------------------------------------------------------------------
// CFindItemDialog::CreateCustomControlL
// from MEikDialogPageObserver
// ----------------------------------------------------------------------------
//
SEikControlInfo CIMSSettingsUi::CreateCustomControlL( TInt aControlType )
    {
    SEikControlInfo info = { 0, 0, 0 };
    if ( aControlType == EIMSWizardRichTxtEditorContainer )
        {
        iWizardContainer = new (ELeave) CIMSWizardControl;
        info.iControl = iWizardContainer;
        }
    return info;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsUi::ExecFlags()
// ----------------------------------------------------------------------------
//
void CIMSSettingsUi::ExecFlags()
    {
    IMUM_CONTEXT( CIMSSettingsUi::ExecFlags, 0, KLogUi );
    IMUM_IN();

    // Execute commands requested by flags

    // Show the menu, if asked
    if ( iFlags->Flag( EImumSettingOpenOptionsMenu ) )
        {
        iFlags->ClearFlag( EImumSettingOpenOptionsMenu );
        TMuiuDynSettingsType type = iSettings->CurrentItem()->iItemType;
        iFlags->ChangeFlag(
            EImumSettingPositionSubMenu, ( type == EMuiuDynSetMenuArray ) );

        TRAP_IGNORE( DisplayMenuL() );
        }
    else if ( iFlags->Flag( EImumSettingShouldExit ) ||
        iFlags->Flag( EImumSettingShouldClose ) )
        {
        // Exit has been issued
        TRAP_IGNORE( iSettings->DoQuitL() );
        }
    else
        {
        // lint
        }
    IMUM_OUT();
    }

// -----------------------------------------------------------------------------
// CIMSSettingsUi::LoadListboxIconArrayL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIMSSettingsUi::LoadListboxIconArrayL()
	{
	IMUM_CONTEXT( CIMSSettingsUi::LoadListboxIconArrayL, 0, KLogUi );
	IMUM_IN();

	// Create array for listbox graphics
	CAknIconArray* icons = new ( ELeave ) CAknIconArray( 1 ); // one icon in array
	CleanupStack::PushL( icons );

    TFileName bitmapfilepath;
    TParse tp;
    tp.Set( KMuiuBitmapFile, &KDC_APP_BITMAP_DIR, NULL );
    bitmapfilepath.Copy( tp.FullName() );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    TAknsItemID aid;

    aid.Set( KAknsIIDQgnIndiSettProtectedAdd );
    AknsUtils::CreateIconLC( AknsUtils::SkinInstance(),
        aid, bitmap, mask, bitmapfilepath,
        EMbmMuiuQgn_indi_sett_protected_add,
        EMbmMuiuQgn_indi_sett_protected_add_mask );

    icons->AppendL( CGulIcon::NewL( bitmap, mask ) );
    CleanupStack::Pop( 2, bitmap ); // CSI: 47 # mask, bitmap
    bitmap = NULL;
    mask = NULL;

    iListBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( icons );

    CleanupStack::Pop( icons );
    icons = NULL;

    IMUM_OUT();
	}

//  End of File
