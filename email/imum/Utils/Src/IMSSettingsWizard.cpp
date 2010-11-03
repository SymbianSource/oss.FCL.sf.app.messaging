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
* Description:  IMSSettingsWizard.cpp
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <badesca.h>                // CDesCArrayFlat
#include <avkon.hrh>                // Softkey enumerations
#include <cemailaccounts.h>         // CEmailAccounts
#include <smtpset.h>                // CImSmtpSettings
#include <imapset.h>                // CImImap4Settings
#include <pop3set.h>                // CImPop3Settings
#include <iapprefs.h>               // CImIAPPreferences
#include <StringLoader.h>           // StringLoader
#include <aknmessagequerydialog.h>  // CAknMessageQueryDialog
#include <MuiuMsvUiServiceUtilities.h> // MsvUiServiceUtilities
#include <eikrted.h>                // CEikRichTextEditor
#include <txtrich.h>                // CRichText
#include <AknInfoPopupNoteController.h>
#include <mtudcbas.h>				// CBaseMtmUiData
#include <mtmuibas.h>		        // CBaseMtmUi
#include <mtsr.h>                   // CMtmRegistryControl

#include <MtmExtendedCapabilities.hrh>
#include <MTMStore.h>				// CMtmStore
#include "MuiuDynamicSettingsItemFinderInterface.h"
#include "MuiuDynamicSettingsDialog.h"
#include <SendUiConsts.h>

#include <ImumUtils.rsg>

#include "ImumInSettingsData.h"
#include "ImumInternalApiImpl.h"
#include "ImumInMailboxServicesImpl.h"
#include "IMSSettingsNoteUi.h"
#include "IMSSettingsWizard.h"
#include "IMSWTextEditorPage.h"
#include "IMSWRadiobuttonPage.h"
#include "IMSSettingsItemApprover.h"
#include "IMASPageIds.hrh"
#include "EmailUtils.H"
#include "EmailFeatureUtils.h"
#include "ComDbUtl.h"               // CMsvCommDbUtilities
#include "ImumUtilsLogging.h"
#include "ImumPanic.h"
#include "IMSWizardControl.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
const TText KIMASCharDot = '.';
const TText KIMASCharAt = '@';
const TInt KMaxPages = 6;
const TInt KMsToWaitBeforePopup = 2000;
const TInt KMsToWaitBeforeRePopup = 6000;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::CIMSSettingsWizard()
// ----------------------------------------------------------------------------
//
CIMSSettingsWizard::CIMSSettingsWizard(
    CIMSSettingsUi& aDialog,
    CImumInternalApiImpl& aMailboxApi,
    CEikFormattedCellListBox& aListBox,
    CIMSWizardControl& aHelpContainer,
    CAknTitlePane& aTitlePane,
    CMuiuFlags& aFlags )
    :
    CIMSSettingsBaseUI( aDialog, aMailboxApi, aListBox,
    					aTitlePane, aFlags ),
    iCurrentItem( KNullUid ),
    iWizState( EWizStateQuerying ),
    iPageEdit( NULL ),
    iPageRadioBtn( NULL ),
    iCurPage( 0 ),
    iPageText( NULL ),
    iHelpContainer( aHelpContainer )
    {
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::~CIMSSettingsWizard()
// ----------------------------------------------------------------------------
//
CIMSSettingsWizard::~CIMSSettingsWizard()
    {
    IMUM_CONTEXT( CIMSSettingsWizard::~CIMSSettingsWizard, 0, KLogUi );
    IMUM_IN();

    iProtocolArray.Close();
    delete iPageText;
    delete iMtmStore;

    iPageEdit = NULL;
    iPageRadioBtn = NULL;

    if ( iWizardLauncher )
        {
        iWizardLauncher->Cancel();
        }

    delete iWizardLauncher;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::ConstructL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::ConstructL( CEikButtonGroupContainer& aSoftkeys,
    const TDesC& aEmailAddress )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::ConstructL, 0, KLogUi );
    IMUM_IN();

    BaseUiConstructL( aSoftkeys, R_IMAS_WIZARD );

	iMtmStore = CMtmStore::NewL(iMailboxApi.MsvSession());

    // Clear the flags
    for ( TUint flag = EWizFlagLastFlag; --flag >= EWizFlagFirstFlag; )
        {
        ClearFlag( flag );
        }

    SettingsStartedL( aEmailAddress );
    
    // Hide the unnecessary scrollbar from the first page of the wizard
    CEikScrollBarFrame* scrollbar = iListBox.ScrollBarFrame();
    if ( scrollbar )
    	{
    	scrollbar->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff );    	
    	}
    
	IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::NewL()
// ----------------------------------------------------------------------------
//
CIMSSettingsWizard* CIMSSettingsWizard::NewL(
    CIMSSettingsUi& aDialog,
    CImumInternalApiImpl& aMailboxApi,
    CEikFormattedCellListBox& aListBox,
    CIMSWizardControl& aHelpContainer,
    CAknTitlePane& aTitlePane,
    CEikButtonGroupContainer& aSoftkeys,
    CMuiuFlags& aFlags,
    const TDesC& aEmailAddress )
    {
    CIMSSettingsWizard* self = NewLC(
        aDialog, aMailboxApi,
        aListBox, aHelpContainer, aTitlePane, aSoftkeys, aFlags, aEmailAddress );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::NewLC()
// ----------------------------------------------------------------------------
//
CIMSSettingsWizard* CIMSSettingsWizard::NewLC(
    CIMSSettingsUi& aDialog,
    CImumInternalApiImpl& aMailboxApi,
    CEikFormattedCellListBox& aListBox,
    CIMSWizardControl& aHelpContainer,
    CAknTitlePane& aTitlePane,
    CEikButtonGroupContainer& aSoftkeys,
    CMuiuFlags& aFlags,
    const TDesC& aEmailAddress )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsWizard::NewLC, 0, utils, KLogUi );
    IMUM_IN();

    CIMSSettingsWizard* self =
        new ( ELeave ) CIMSSettingsWizard(
            aDialog, aMailboxApi,
            aListBox, aHelpContainer, aTitlePane, aFlags );
    CleanupStack::PushL( self );
    self->ConstructL( aSoftkeys, aEmailAddress );

	IMUM_OUT();
    return self;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::DefinePageNumberStringL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::DefinePageNumberStringL()
    {
    IMUM_CONTEXT( CIMSSettingsWizard::DefinePageNumberStringL, 0, KLogUi );
    IMUM_IN();

    CArrayFix<TInt>* array = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
    CleanupStack::PushL( array );

    // If current page is zero, increase it to 1
    if ( iCurPage == 0 )
    	{
    	iCurPage++;
    	}

    array->AppendL( iCurPage );
    array->AppendL( KMaxPages );

    delete iPageText;
    iPageText = NULL;

    // Load page number to descriptor
	iPageText = StringLoader::LoadL(
	R_MBXS_SETTINGS_MAIL_NEWBOX_PAGES, *array );


    CleanupStack::PopAndDestroy( array );
    array = NULL;
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::SettingsStartedL()
// ----------------------------------------------------------------------------
//
TBool CIMSSettingsWizard::SettingsStartedL( const TDesC& aEmailAddress )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::SettingsStartedL, 0, KLogUi );
	IMUM_IN();

    // Set the titlepane text
    HBufC* text = StringLoader::LoadL( R_MBXS_SETTINGS_NEW_BOX );
    CleanupStack::PushL( text );
    SetTitlePaneTextL( *text );
    CleanupStack::PopAndDestroy( text );

    HidePermanently( ToUid( EIMAWRadioButtonIapAlwaysAsk ), ETrue );

    CMuiuSettingsEditText* email = static_cast<CMuiuSettingsEditText*>(
        GetItem( TUid::Uid( EIMAWEmailAddress ) ) );
    TMuiuSettingsText emailText;

    if ( aEmailAddress.Length() )
        {
        emailText = aEmailAddress;
        email->SetText( &emailText );
        SettingsItemDefineServers( *email->Text() );
        SettingsItemDefineMailboxName( *email->Text() );
        SetFlag( EWizFlagSkipEmail );

        SetFlag( EWizFlagPluginDetected );
        iWizardLauncher = CIMSWizardLauncher::NewL( *this, iDialog );

        //Next prevents the original info screen and buttons from flashing
        //on the screen
        iSoftkeys->SetVisibility( EFalse, this );

        if ( iHelpContainer.CountComponentControls() )
            {
            iHelpContainer.ComponentControl( 0 )->MakeVisible( EFalse );
            }
        }
    else
        {
        // Hide the listbox and the helpscreen
        // The buttons should be hidden here too for the dialog
        SetHideItems( ETrue, R_IMAS_WIZARD, ETrue );
        iDialog.MakeLineVisible( EIMSWizardRichTxtEditorContainer, ETrue);

        //TMuiuSettingsText at;
        emailText.Append( KIMASCharAt );
        email->SetText( &emailText );
        }


	IMUM_OUT();
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::OkToExitL()
// ----------------------------------------------------------------------------
//
TInt CIMSSettingsWizard::OkToExitL( const TInt /* aButtonId */ )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::OkToExitL, 0, KLogUi );
    IMUM_IN();

    // If wizard is running (setting page is on display), let setting page
    // and wizard loop to exit before exiting. Save exit flags to reinstate
    // them when wizard loop exists.
    if ( Flag( EWizFlagStateRunning ) )
        {
        ChangeFlag( EWizFlagShouldExit, Flag( EImumSettingShouldExit ) );
        ChangeFlag( EWizFlagShouldClose, Flag( EImumSettingShouldClose ) );
        IMUM_OUT();
        return EFalse;
        }

    // Show query for exit
    TInt result = 1;

    // Check if user has to be asked about the exit
    if ( Flag( EImumSettingQueryClose ) && !Flag( EWizFlagForcedCancel ))
        {
        if ( !Flag( EWizFlagCompleted ) )
            {
            result = CIMSSettingsNoteUi::ShowQueryL(
	                R_MBXS_SETTINGS_NEW_BOX_EXIT_QUERY,
	                R_EMAIL_CONFIRMATION_QUERY );
            if ( !result && Flag( EWizFlagPluginDetected ) )
                {
                iWizardLauncher->StartLauncher();
                ClearFlag( EWizFlagShouldExit );
                ClearFlag( EWizFlagShouldClose );
                }
            }
        }

	IMUM_OUT();
    return result;
    }


// ----------------------------------------------------------------------------
// CIMSSettingsWizard::KeyPressSoftkey()
// ----------------------------------------------------------------------------
//
TBool CIMSSettingsWizard::KeyPressSoftkey(
    const TInt aButton )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::KeyPressSoftkey, 0, KLogUi );
    IMUM_IN();

    TInt error = KErrNone;

    // Check which of the keys has been pressed
    if ( aButton == EAknSoftkeyBack )
        {
        if ( iWizState == EWizStateQuerying )
            {
            SetFlag( EImumSettingShouldClose );
            }
        else
            {
            TRAP( error, KeyPressBackL() );
            }
        }
    else
        {
        if ( iWizState == EWizStateQuerying )
            {
			WizRunStart();
            TRAP( error,
            		while( Flag( EWizFlagStateRunning ) )
            				 {
            				 WizRunStepL();
            				 } );
            }
        else
            {
            TRAP( error, KeyPressNextL() );
            }
        }

    __ASSERT_DEBUG( error == KErrNone,
        User::Panic( KIMSWizardPanic, error ) );

    IMUM_OUT();
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::KeyPressOKButtonL()
// ----------------------------------------------------------------------------
//
TBool CIMSSettingsWizard::KeyPressOKButtonL( const TInt /* aButton */ )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::KeyPressOKButtonL, 0, KLogUi );
    IMUM_IN();

    // In help screen, ok button opens the first page
    if ( iWizState == EWizStateQuerying )
        {
        KeyPressSoftkey( EAknSoftkeyShow );
        }
    // Text page editing ongoing, call correct page
    else if ( iSettingPage && Flag( EWizFlagTextEditPage ) )
        {
        iPageEdit->KeyPressedSelectL();
        }
    // Radio button page editing ongoing, call correct page
    else if ( iSettingPage && Flag( EWizFlagRadioButtonPage ) )
        {
        iPageRadioBtn->KeyPressedSelectL();
        }
    else
        {
        // Nothing
        }

	IMUM_OUT();
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::KeyPressNextL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::KeyPressNextL()
    {
    IMUM_CONTEXT( CIMSSettingsWizard::KeyPressNextL, 0, KLogUi );
    IMUM_IN();

    SetFlag( EWizFlagNextKeyPressed );

    // Text page editing ongoing, call correct page
    if ( iSettingPage && Flag( EWizFlagTextEditPage ) )
        {
        iPageEdit->KeyPressedNextL();
        }
    // Radio button page editing ongoing, call correct page
    else if ( iSettingPage && Flag( EWizFlagRadioButtonPage ) )
        {
        iPageRadioBtn->KeyPressedNextL();
        }
    else
        {
        // Nothing
        }

	IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::KeyPressBackL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::KeyPressBackL()
    {
    IMUM_CONTEXT( CIMSSettingsWizard::KeyPressBackL, 0, KLogUi );
    IMUM_IN();

    SetFlag( EWizFlagBackKeyPressed );

    // Text page editing ongoing, call correct page
    if ( iSettingPage && Flag( EWizFlagTextEditPage ) )
        {
        iPageEdit->KeyPressedBackL();
        }
    // Radio button page editing ongoing, call correct page
    else if ( iSettingPage && Flag( EWizFlagRadioButtonPage ) )
        {
        iPageRadioBtn->KeyPressedBackL();
        }
    else
        {
        // Nothing
        }
    if ( Flag( EWizFlagPluginDetected ) && iCurrentItem.iUid == EIMAWProtocol )
        {
        SetFlag( EImumSettingShouldClose );
        }

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::EventItemEditStartsL
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsWizard::EventItemEditStartsL(
    CMuiuSettingBase& /* aBaseItem */ )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::EventItemEditStartsL, 0, KLogUi );
    IMUM_IN();

    iNoteDialog = ShowMailSettingsPopupAfterL(
        iCurrentItem, KMsToWaitBeforePopup );

	IMUM_OUT();
    return EMuiuPageEventResultApproved;
    }

/******************************************************************************

    Wizard state machine

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::WizRunStart()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::WizRunStart()
    {
    // This function is called, when user starts the wizard by
    // pressing Start-key or by choosing options->Start
    SetFlag( EWizFlagStateRunning );
    iWizState = EWizStateStarted;
    }


// ----------------------------------------------------------------------------
// CIMSSettingsWizard::WizRunStepL()
// ----------------------------------------------------------------------------
//
TBool CIMSSettingsWizard::WizRunStepL()
	{
	IMUM_CONTEXT( CIMSSettingsWizard::WizRunStepL, 0, KLogUi );
	IMUM_IN();

	// Run wizard step and change state
    WizStateMachineL();
    WizStateChangeStateL();

	IMUM_OUT();
    return Flag( EWizFlagStateRunning );
	}


// ----------------------------------------------------------------------------
// CIMSSettingsWizard::WizStateChangeStateL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::WizStateChangeStateL()
    {
    IMUM_CONTEXT( CIMSSettingsWizard::WizStateChangeStateL, 0, KLogUi );
    IMUM_IN();

    // if EWizFlagShouldExit or EWizFlagShouldClose are on, OkToExitL()
    // inquiry was received while wizard is running. Should exit wizard
    // loop immediately.
    if ( Flag( EWizFlagShouldExit ) || Flag( EWizFlagShouldClose ) )
        {
        // Reinstate exit flags as they were reset after we declined
        // exit request in OkToExitL().
        ChangeFlag( EImumSettingShouldExit, Flag( EWizFlagShouldExit ) );
        ChangeFlag( EImumSettingShouldClose, Flag( EWizFlagShouldClose ) );

        ClearFlag( EWizFlagStateRunning ); // exit wizard loop
        IMUM_OUT();
        return;
        }

    // This seperate function is meant only to chage the state of the
    // state machine.
    switch ( iWizState )
        {
        // After wizard has been initialized, go straight to page open state
        case EWizStateStarted:
            iWizState = EWizStateEditPage;
            break;

        // After new item has been chosen, Change to editing mode
        case EWizStateDeterminePage:
            iWizState = EWizStateEditPage;
            break;

        // After the item has been edited, check that the changes are ok
        case EWizStateEditPage:
            iWizState = EWizFinishEditing;
            break;

        // After validating item, the wizard can be exited by succesfully
        // finishing the wizard, or by returning back to help screen
        case EWizFinishEditing:
            iWizState = ( Flag( EWizFlagExiting ) ||
                          Flag( EImumSettingShouldExit ) ) ?
                 EWizStateExit : EWizStateDeterminePage;
            break;

        // As default, try to leave the wizard
        default:
        case EWizStateExit:
            iWizState = ( !Flag( EWizFlagCompleted ) && !Flag( EWizFlagForcedCancel )) ?
                 EWizStateQuerying : EWizStateStarted;
            break;

        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::WizStateMachineL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::WizStateMachineL()
    {
    IMUM_CONTEXT( CIMSSettingsWizard::WizStateMachineL, 0, KLogUi );
    IMUM_IN();

    switch ( iWizState )
        {
        // This is executed, when wizard is started
        case EWizStateStarted:
            WizHandleStateInitializeL();
            break;

        // In this state, the wizard chooses the next item to be edited
        case EWizStateDeterminePage:
            WizHandleStateDeterminePage();
            break;

        // Start editing the actual item
        case EWizStateEditPage:
            WizHandleStateEditL();
            break;

        // Check that the item is ok, so that the editing can be continued
        case EWizFinishEditing:
            WizHandleStateFinishEditingL();
            break;

        // Handle the exiting from the wizard
        default:
        case EWizStateExit:
            WizHandleStateExitL();
            break;
        }
    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::WizHandleStateInitializeL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::WizHandleStateInitializeL()
    {
    IMUM_CONTEXT( CIMSSettingsWizard::WizHandleStateInitializeL, 0, KLogUi );
    IMUM_IN();

    SetHideItems( ETrue, R_IMAS_WIZARD, ETrue );

    iDialog.MakeLineVisible( EIMSWizardRichTxtEditorContainer, EFalse);

    iCurrentItem.iUid = EIMAWProtocol;

    // if protocoll for mailbox has not yet been selected the iCurPage is set to 0 so that
    // wizard page number text is only updated after the number of pages is known.
    if ( iProtocol == KNullUid )
    	iCurPage = 0;

    // Clear all flags except EWizFlagStateRunning
    for ( TInt flag = EWizFlagExiting; flag <= EWizFlagTextEditPage; flag++ )
        {
        ClearFlag( flag );
        }

    iPageEdit = NULL;
    iPageRadioBtn = NULL;
	IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::WizHandlePageForward()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::WizHandlePageForward()
    {
    IMUM_CONTEXT( CIMSSettingsWizard::WizHandlePageForward, 0, KLogUi );
    IMUM_IN();

    iCurrentItem.iUid++;
    iCurPage++;

    ClearFlag( EWizFlagItemApproved );
    ClearFlag( EWizFlagNextKeyPressed );

	IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::WizHandlePageBackward()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::WizHandlePageBackward()
    {
    IMUM_CONTEXT( CIMSSettingsWizard::WizHandlePageBackward, 0, KLogUi );
	IMUM_IN();

    iCurrentItem.iUid--;
    iCurPage--;

    ClearFlag( EWizFlagBackKeyPressed );

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::WizHandleStateDeterminePage()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::WizHandleStateDeterminePage()
    {
    IMUM_CONTEXT( CIMSSettingsWizard::WizHandleStateDeterminePage, 0, KLogUi );
    IMUM_IN();

    // When next key is pressed and the item is approved, change to next page
    if ( Flag( EWizFlagNextKeyPressed ) &&
         Flag( EWizFlagItemApproved ) )
        {
        WizHandlePageForward();
        }
    // When back key is pressed, change to previous page
    else if ( Flag( EWizFlagBackKeyPressed ) )
        {
        WizHandlePageBackward();
        }
    else
        {
        // Just open the same page again, as it has not been filled correctly
        ClearFlag( EWizFlagNextKeyPressed );
        }

	IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::WizHandleStateEditL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::WizHandleStateEditL()
    {
    IMUM_CONTEXT( CIMSSettingsWizard::WizHandleStateEditL, 0, KLogUi );
    IMUM_IN();

    // Get the item from the list
    CMuiuSettingBase* setting = GetItem( iCurrentItem );
    User::LeaveIfNull( setting );

    // When editing other edit settings, create page with next and back
    if ( setting->iItemType == EMuiuDynSetItemEditText )
        {
        SetFlag( EWizFlagTextEditPage );
        }
    // For radiobutton pages, show next and back
    else if ( setting->iItemType == EMuiuDynSetRadioButtonArray )
        {
        SetFlag( EWizFlagRadioButtonPage );
        }
    // Iap is radiobutton page
    else if ( iCurrentItem.iUid == EIMAWIap )
        {
        SetFlag( EWizFlagRadioButtonPage );
        }
    else
        {
        // do nothing
        }

    DefinePageNumberStringL();

    //Checks if email editor should be skipped
    if ( !CheckEmailSkip() )
        {
        // Begin editing the page
        OpenSettingPageL( *setting );
        }

    delete iNoteDialog;
    iNoteDialog = NULL;

	IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::WizHandleStateFinishEditingL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::WizHandleStateFinishEditingL()
    {
    IMUM_CONTEXT( CIMSSettingsWizard::WizHandleStateFinishEditingL, 0, KLogUi );
    IMUM_IN();

    // Should exit when:
    // Back key pressed and first page  OR
    if ( Flag( EWizFlagBackKeyPressed ) &&
         iCurrentItem.iUid == EIMAWProtocol )
        {
        SetFlag( EWizFlagExiting );
        }

    // Next key pressed and item validated and last item
    if ( Flag( EWizFlagNextKeyPressed ) )
        {
       	TBool isInternal = iMailboxApi.MailboxUtilitiesL().IsMailMtm( iProtocol, ETrue );

		if ( !isInternal && iCurrentItem.iUid == EIMAWProtocol )
			{
            TBool completed = ETrue;
			//This should not make the utils code leave
			TRAP_IGNORE( CallExternalMtmWizardL( iProtocol, completed ) );
            if (!completed)
                {
                ClearFlag( EWizFlagNextKeyPressed );
                }
			}
		else
			{
            // Get the item to be edited
            CMuiuSettingBase* base = GetItem( iCurrentItem );

            if ( iCurrentItem.iUid == EIMAWMailboxName )
                {
                ChangeFlag( EWizFlagExiting, Flag( EWizFlagItemApproved ) );
            	}
            }
        }

    if ( Flag ( EWizFlagNextKeyPressed ) || Flag ( EWizFlagBackKeyPressed ) )
        {
        // Clear the flags while exiting
        ClearFlag( EWizFlagRadioButtonPage );
        }
    ClearFlag( EWizFlagTextEditPage );
    iPageEdit = NULL;
    iPageRadioBtn = NULL;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::WizHandleStateExitL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::WizHandleStateExitL()
    {
    IMUM_CONTEXT( CIMSSettingsWizard::WizHandleStateExitL, 0, KLogUi );
    IMUM_IN();

    // Stop the wizard
    ClearFlag( EWizFlagStateRunning );

    // If the last item is approved, complete the wizard
    if ( Flag( EWizFlagNextKeyPressed ) &&
        iCurrentItem.iUid == EIMAWMailboxName )
        {
        ChangeFlag( EWizFlagCompleted, CheckCompleted() );
        }

    // In case wizard is not completed, but just returned back to help screen
   	if ( !Flag( EWizFlagCompleted ) && !Flag( EWizFlagForcedCancel ))
        {
        iDialog.MakeLineVisible( EIMSWizardRichTxtEditorContainer, ETrue);
        }
    else
        {
        if( Flag( EWizFlagCompleted ) )
        	{
   			// Show the information query to the user
        	// before leaving the wizard.
   			CIMSSettingsNoteUi::ShowQueryL(
                R_MBXS_SETTINGS_MAIL_NEWBOX_FINAL, R_EMAIL_INFORMATION_QUERY, ETrue );
        	}
        SetFlag( EImumSettingShouldClose );
        }

	IMUM_OUT();
    }


/******************************************************************************

    Item handling

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::EventItemEditEndsL()
// ----------------------------------------------------------------------------
//
TInt CIMSSettingsWizard::EventItemEditEndsL(
    CMuiuSettingBase& aBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::EventItemEditEndsL, 0, KLogUi );
    IMUM_IN();

    if ( Flag( EWizFlagItemApproved ) )
        {
        // Set item as approved accoding to the settings
        aBaseItem.iItemFlags.SetFlag( EMuiuDynItemValidated );

        SettingsItemProcessItemL( aBaseItem );
        }

	IMUM_OUT();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::EventItemEvaluateRadioButton()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsWizard::EventItemEvaluateRadioButton(
    const CMuiuSettingBase& aBaseItem,
    TInt& aNewValue )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::EventItemEvaluateRadioButton, 0, KLogUi );
	IMUM_IN();

    // Test the item with the approver
    TIMSApproverEvent event;
    TMuiuPageEventResult result = iItemApprover->EvaluateValue(
        event, aBaseItem, aNewValue );

    // The EWizFlagItemApproved has to be updated according to result of the
    // validation. When flag is ETrue, next page can be opened
    TBool approved = ( result == EMuiuPageEventResultApproved );
    ChangeFlag( EWizFlagItemApproved, approved );
    if ( !approved )
        {
        // Needs to clear this as CMuiuDynamicSettingsDialog opens the
        // same page again without returning to wizard.
        ClearFlag( EWizFlagNextKeyPressed );
        }

    IMUM_OUT();
    return result;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::EventItemEvaluateText()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CIMSSettingsWizard::EventItemEvaluateText(
    const CMuiuSettingBase& aBaseItem,
    TDes& aNewText )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::EventItemEvaluateText, 0, KLogUi );
    IMUM_IN();

    // Test the item with the approver
    TIMSApproverEvent event;
    TMuiuPageEventResult result = iItemApprover->EvaluateText(
        event, aBaseItem, aNewText );

    // The EWizFlagItemApproved has to be updated according to result of the
    // validation. When flag is ETrue, next page can be opened
    TBool approved = ( result == EMuiuPageEventResultApproved );
    ChangeFlag( EWizFlagItemApproved, approved );
    if ( !approved )
        {
        // Needs to clear this as CMuiuDynamicSettingsDialog opens the
        // same page again without returning to wizard.
        ClearFlag( EWizFlagNextKeyPressed );
        }

    IMUM_OUT();
    return result;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::SettingsItemProcessItemL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::SettingsItemProcessItemL(
    CMuiuSettingBase& aBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::SettingsItemProcessItemL, 0, KLogUi );
    IMUM_IN();

    switch ( aBaseItem.iItemId.iUid )
        {
        case EIMAWProtocol:
            SettingsItemProcessProtocol( aBaseItem );
            break;

        case EIMAWEmailAddress:
            SettingsItemProcessEmailL( aBaseItem );
            break;

        case EIMAWIap:
            SettingsItemProcessIapL( aBaseItem );
            break;

        default:
            break;
        }

	IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::SettingsItemProcessProtocol()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::SettingsItemProcessProtocol(
    CMuiuSettingBase& aBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::SettingsItemProcessProtocol, 0, KLogUi );

   	iProtocol.iUid = iProtocolArray[aBaseItem.Value()];
   	if ( iProtocol == KSenduiMtmImap4Uid )
   	    {
   	    SettingsItemDefineNormalWizardArray();
   	    }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::SettingsItemDefineNormalWizardArray()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::SettingsItemDefineNormalWizardArray()
    {
    IMUM_CONTEXT( CIMSSettingsWizard::SettingsItemDefineNormalWizardArray, 0, KLogUi );

    // Unhide all the items
    HidePermanently( ToUid( EIMAWProtocol ), EFalse );
    HidePermanently( ToUid( EIMAWEmailAddress ), EFalse );
    HidePermanently( ToUid( EIMAWReceivingServer ), EFalse );
    HidePermanently( ToUid( EIMAWSendingServer ), EFalse );
    HidePermanently( ToUid( EIMAWIap ), EFalse );
    HidePermanently( ToUid( EIMAWMailboxName ), EFalse );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::SettingsItemProcessEmailL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::SettingsItemProcessEmailL(
    CMuiuSettingBase& aBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::SettingsItemProcessEmailL, 0, KLogUi );

    CMuiuSettingsEditText* email =
        static_cast<CMuiuSettingsEditText*>( &aBaseItem );

    // Define new server address and mailbox name, if the email address changes
    if ( email->Text() &&
         email->Text()->CompareC( iTemporaryText ) )
        {
        // Fill rest of the items
        SettingsItemDefineServers( *email->Text() );
        SettingsItemDefineMailboxName( *email->Text() );
        }

    email = NULL;
    iTemporaryText.Zero();

    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::SettingsItemProcessIapL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::SettingsItemProcessIapL(
    CMuiuSettingBase& aBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::SettingsItemProcessIapL, 0, KLogUi );

    CIMSSettingsAccessPointItem* iapItem =
        static_cast<CIMSSettingsAccessPointItem*>( &aBaseItem );

    // Update the iap item, but use the radio button as reference point
    SetItemIapItemL( *iapItem );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::SettingsItemDefineServerL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::SettingsItemDefineServers(
    const TDesC& aEmailAddress )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::SettingsItemDefineServers, 0, KLogUi );

    // Fetch the item
    CMuiuSettingsEditText* outgoing = static_cast<CMuiuSettingsEditText*>(
        GetItem( TUid::Uid( EIMAWSendingServer ) ) );
    CMuiuSettingsEditText* incoming = static_cast<CMuiuSettingsEditText*>(
        GetItem( TUid::Uid( EIMAWReceivingServer ) ) );

    if ( outgoing && incoming )
        {
        TMuiuSettingsText newAddress;
        TInt len = aEmailAddress.Length();

        // Find index after '@' character
        TInt index = aEmailAddress.Locate( KIMASCharAt ) + 1;
        newAddress.Zero();
        newAddress.Append( KIMASCharDot );
        newAddress.Append( aEmailAddress.Right( len - index ) );

        // Finally copy the address
        outgoing->SetText( &newAddress );
        incoming->SetText( &newAddress );
        }

    outgoing = NULL;
    incoming = NULL;

    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::SettingsItemDefineMailboxName()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::SettingsItemDefineMailboxName(
    const TDesC& aEmailAddress )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::SettingsItemDefineMailboxName, 0, KLogUi );

    // Fetch the item
    CMuiuSettingsEditText* name = static_cast<CMuiuSettingsEditText*>(
        GetItem( TUid::Uid( EIMAWMailboxName ) ) );
    TInt len = aEmailAddress.Length();

    if ( name && len > 0 )
        {
        // Clear the old name and prepare the new
        TMuiuSettingsText newAddress;

        // Find the first letter after '@' character and set the string
        // to name field
        TInt firstLetterIndex = aEmailAddress.Locate( KIMASCharAt ) + 1;
        newAddress.Append( aEmailAddress.Right( len - firstLetterIndex ) );

        // Find the first '.' character and remove it and
        // all the text after it
        TInt pointCharIndex = newAddress.Locate( KIMASCharDot );
        if ( pointCharIndex > 0 )
            {
            newAddress.Delete( pointCharIndex, newAddress.Length() - 1 );
            }

        // Validate the generated account name
        TIMSApproverEvent event;
        name->iItemFlags.SetFlag( EIMSApproverGenerate );
        iItemApprover->EvaluateText( event, *name, newAddress );
        name->iItemFlags.ClearFlag( EIMSApproverGenerate );

        // Finally copy the address
        name->SetText( &newAddress );
        }

    // Clean up
    name = NULL;

    }


// ----------------------------------------------------------------------------
// CIMSSettingsWizard::CheckEmailSkip()
// ----------------------------------------------------------------------------
//
TBool CIMSSettingsWizard::CheckEmailSkip()
    {
    TBool ok = EFalse;
    if ( Flag( EWizFlagSkipEmail ) && iCurrentItem.iUid == EIMAWEmailAddress )
        {
        ClearFlag( EWizFlagSkipEmail );
        CMuiuSettingsEditText* email = static_cast<CMuiuSettingsEditText*>(
            GetItem( TUid::Uid( EIMAWEmailAddress ) ) );
        TRAPD( error, ok =
            MsvUiServiceUtilities::IsValidEmailAddressL( *email->Text() ) );
        if ( !error && ok )
            {
            CMuiuSettingBase* emailItem = GetItem( TUid::Uid( EIMAWEmailAddress ) );
            emailItem->iItemFlags.SetFlag( EMuiuDynItemValidated );
            SetFlag( EWizFlagItemApproved );
            SetFlag( EWizFlagNextKeyPressed );
            }
        else
            {
            // Should not leave just show the email address
            TRAP_IGNORE( CIMSSettingsNoteUi::ShowNoteL(
                R_IMUM_EDIT_OWN_EMAIL_ADDRESS_ERROR_TEXT,
                EIMSInformationNote, ETrue ) );
            }
        }
    return ok;
    }

/******************************************************************************

    Custom settings

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::AcquireCustomSettingPageL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::AcquireCustomSettingPageL(
    CAknSettingPage*& aReturnPage,
    TMuiuDynSetUpdateMode& aReturnUpdateMode,
    TInt& aReturnSettingValue,
    TMuiuSettingsText& aReturnSettingText,
    CDesCArrayFlat& aReturnRadioButtonArray,
    CMuiuSettingBase& aParamBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::AcquireCustomSettingPageL, 0, KLogUi );

    switch ( aParamBaseItem.iItemId.iUid )
        {
        // Radiobutton pages
        case EIMAWProtocol:
            aReturnPage = AcquireProtocolPageL(
                aReturnSettingValue, aReturnUpdateMode,
                aReturnRadioButtonArray, aParamBaseItem );
            break;

        // Text editors
        case EIMAWEmailAddress:
        case EIMAWSendingServer:
        case EIMAWReceivingServer:
        case EIMAWMailboxName:
            aReturnPage = AcquireCustomTextEditorL(
                aReturnSettingText, aReturnUpdateMode,
                aParamBaseItem );
            break;

        case EIMAWIap:
            aReturnPage = AcquireCustomIapPageL(
                aReturnSettingValue, aReturnUpdateMode,
                aReturnRadioButtonArray, aParamBaseItem );
            break;

        default:
            break;
        }

    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::AcquireCustomRadioButtonPageL()
// ----------------------------------------------------------------------------
//
CAknSettingPage* CIMSSettingsWizard::AcquireCustomRadioButtonPageL(
    TInt& aReturnSettingValue,
    TMuiuDynSetUpdateMode& aReturnUpdateMode,
    MDesCArray& aReturnRadioButtonArray,
    CMuiuSettingBase& aParamBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::AcquireCustomRadioButtonPageL, 0, KLogUi );

    iPageRadioBtn =
        ( CIMSSettingsWizardRadioButtonPage::NewL(
            *this, R_IMAS_WIZARD_DIALOG_LISTBOX_NEXT_BACK,
            aReturnSettingValue, &aReturnRadioButtonArray, *iPageText ) );

    // Define the page properties
    iPageRadioBtn->SetSettingTextL( *aParamBaseItem.iItemLabel );

    // Set updatemode to accept setting only when the setting is
    // accepted
    aReturnUpdateMode = CAknSettingPage::EUpdateWhenAccepted;

    return iPageRadioBtn;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::AcquireCustomTextEditorL()
// ----------------------------------------------------------------------------
//
CAknSettingPage* CIMSSettingsWizard::AcquireCustomTextEditorL(
    TMuiuSettingsText& aReturnSettingText,
    TMuiuDynSetUpdateMode& aReturnUpdateMode,
    CMuiuSettingBase& aParamBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::AcquireCustomTextEditorL, 0, KLogUi );

    // Allow zero length with wizard, the setting won't be accepted
    // anyway, when empty
    TInt settingPageFlags = GetTextEditorFlags( aParamBaseItem );

    // Determine buttons
    TInt page = ( aParamBaseItem.iItemId.iUid == EIMAWMailboxName ) ?
        R_IMAS_WIZARD_DIALOG_EDWIN_FINISH_BACK :
        R_IMAS_WIZARD_DIALOG_EDWIN_NEXT_BACK;

    // Create the text editir page
    iPageEdit =
        ( CIMSSettingsWizardEditTextPage::NewL(
            *this, page, *&aReturnSettingText, *iPageText,
            settingPageFlags ) );

    // Define the page properties
    iPageEdit->SetSettingTextL( *aParamBaseItem.iItemLabel );
    CMuiuSettingsEditText* textEditor =
        static_cast<CMuiuSettingsEditText*>( &aParamBaseItem );
    iPageEdit->TextControl()->SetTextLimit(
        textEditor->iItemMaxLength );
    aReturnSettingText.Copy( *textEditor->Text() );
    iTemporaryText.Zero();
    iTemporaryText.Copy( *textEditor->Text() );

    // Set updatemode to accept setting only when the setting is
    // accepted
    aReturnUpdateMode = CAknSettingPage::EUpdateWhenAccepted;

    return iPageEdit;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::AcquireCustomIapPageL()
// ----------------------------------------------------------------------------
//
CAknSettingPage* CIMSSettingsWizard::AcquireCustomIapPageL(
    TInt& aReturnSettingValue,
    TMuiuDynSetUpdateMode& aReturnUpdateMode,
    CDesCArrayFlat& aReturnRadioButtonArray,
    CMuiuSettingBase& aParamBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::AcquireCustomIapPageL, 0, KLogUi );

    CIMSSettingsAccessPointItem* iapItem =
        static_cast<CIMSSettingsAccessPointItem*>( &aParamBaseItem );

    // Initialize item in the first round
    if ( !aParamBaseItem.iItemFlags.Flag( EMuiuDynItemChecked ) )
        {
        aParamBaseItem.iItemFlags.SetFlag( EMuiuDynItemChecked );
        iMailboxApi.CommDbUtilsL().InitItemAccessPointL( *iapItem, ETrue );
        }

    // Create the button array
    CDesCArrayFlat* buttons = &aReturnRadioButtonArray;

    TRAPD( error, iMailboxApi.CommDbUtilsL().FillCustomIapArrayL( *buttons ) );
    if ( error != KErrNone )
        {
        __ASSERT_DEBUG( EFalse,
            User::Panic( KIMSWizardPanic, error ) );
        }

    // The correct radiobutton index is fetched from the item
    aReturnSettingValue = iMailboxApi.CommDbUtilsL().GetWizardRadioButton( *iapItem );

    AcquireCustomRadioButtonPageL(
        aReturnSettingValue, aReturnUpdateMode, *buttons, aParamBaseItem );

    // Focus the first access point, when no items selected
    if ( aParamBaseItem.Value() == KErrNotFound )
        {
        TInt staticCount = iMailboxApi.CommDbUtilsL().StaticIAPCount();
        TInt highlight = buttons->Count() > staticCount ? staticCount : 0;

        iPageRadioBtn->ListBoxControl()->SetCurrentItemIndexAndDraw( highlight );
        }

    return iPageRadioBtn;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::AcquireProtocolPageL()
// ----------------------------------------------------------------------------
//
CAknSettingPage* CIMSSettingsWizard::AcquireProtocolPageL(
    TInt& aReturnSettingValue,
    TMuiuDynSetUpdateMode& aReturnUpdateMode,
    CDesCArrayFlat& aReturnRadioButtonArray,
    CMuiuSettingBase& aParamBaseItem )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::AcquireProtocolPageL, 0, KLogUi );
	IMUM_IN();

    CDesCArrayFlat* buttons = &aReturnRadioButtonArray;

	CMtmUiDataRegistry* uiDataMtmRegistry = CMtmUiDataRegistry::NewL( iMailboxApi.MsvSession() );
	CleanupStack::PushL( uiDataMtmRegistry );
	CBaseMtmUiData* uiData = NULL;

    // Initialize the button/protocol ID mapping table
    iProtocolArray.Reset();
	iProtocolArray.AppendL( KSenduiMtmImap4UidValue );
	iProtocolArray.AppendL( KSenduiMtmPop3UidValue );

	for ( TInt i = uiDataMtmRegistry->NumRegisteredMtmDlls(); --i >= 0; )
		{
        TUid mtmType = uiDataMtmRegistry->MtmTypeUid(i);

        if ( uiDataMtmRegistry->TechnologyTypeUid( mtmType ) == KSenduiTechnologyMailUid )
            {
            TRAPD( err, uiData = &( iMtmStore->MtmUiDataL( mtmType ) ) );

            if ( err == KErrNone )
                {
                CheckMtmWizardSupportL( *uiData, *buttons );
                iMtmStore->ReleaseMtmUiData( mtmType );
                }
            }
		}

	CleanupStack::PopAndDestroy( uiDataMtmRegistry );
	uiDataMtmRegistry = NULL;

    AcquireCustomRadioButtonPageL(
        aReturnSettingValue, aReturnUpdateMode, *buttons, aParamBaseItem );

    IMUM_OUT();

    return iPageRadioBtn;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::CheckMtmWizardSupportL()
// Checks if spesified MTM has ability to launch added-on wizard from email
// creation wizard
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::CheckMtmWizardSupportL(
    CBaseMtmUiData& aMtmUiData,
    CDesCArrayFlat& aRadioButtons
    )
    {
	IMUM_CONTEXT( CIMSSettingsWizard::CheckMtmWizardSupportL, 0, KLogUi );
	IMUM_IN();

    const CArrayFix<CBaseMtmUiData::TMtmUiFunction>& funcs =
        aMtmUiData.MtmSpecificFunctions();
    TInt  funcIndex = -1;
    TBool found = EFalse;

    while ( ( ++funcIndex < funcs.Count() ) && found == EFalse )
        {
        CBaseMtmUiData::TMtmUiFunction func = funcs.At( funcIndex );

        if ( func.iFunctionId == KMtmUiFunctionSettingsWizard )
            {
            aRadioButtons.AppendL( func.iCaption );
            iProtocolArray.AppendL( aMtmUiData.Type().iUid );
            found = ETrue;
            }
        }
    IMUM_OUT();
    }


/******************************************************************************

    Account creating

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::StoreSettingsToAccountL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizard::StoreSettingsToAccountL(
    CImumInSettingsData& aSettings )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::StoreSettingsToAccountL, 0, KLogUi );


    SetPreviousTitlePaneText();

    // Make sure that all settings really are ok
    if ( !Flag( EWizFlagCompleted ) )
        {
        // Settings not ok, don't try to create them
        aSettings.ResetAll();

        return;
        }

    TUid protocol;
    if ( iProtocol == KSenduiMtmImap4Uid )
        {
        protocol = KSenduiMtmImap4Uid;
        }
    else
        {
        protocol = KSenduiMtmPop3Uid;
        }

    // First get the items and check they can be used
    CMuiuSettingsEditText* emailAddress =
        static_cast<CMuiuSettingsEditText*>(
            GetItem( TUid::Uid( EIMAWEmailAddress ) ) );
    User::LeaveIfNull( emailAddress );

    CMuiuSettingsEditText* outgoingAddress =
        static_cast<CMuiuSettingsEditText*>(
            GetItem( TUid::Uid( EIMAWSendingServer ) ) );
    User::LeaveIfNull( outgoingAddress );


    CMuiuSettingsEditText* incomingAddress =
        static_cast<CMuiuSettingsEditText*>(
            GetItem( TUid::Uid( EIMAWReceivingServer ) ) );
    User::LeaveIfNull( incomingAddress );

    CIMSSettingsAccessPointItem* iapPreference =
        static_cast<CIMSSettingsAccessPointItem*>(
            GetItem( TUid::Uid( EIMAWIap ) ) );
    User::LeaveIfNull( iapPreference );
    // Resolve Access Point type, and actual ID; it is assumed
    // that Default Connection is always first in the list
    if( iapPreference->iIapRadioButton <= 0 )
    	{
    	iapPreference->iIap.iResult = CMManager::EDefaultConnection;
    	iapPreference->iIap.iId = 0; // default conn id
    	}
    else
    	{
    	iapPreference->iIap.iResult = CMManager::EConnectionMethod;
    	iapPreference->iIap.iId = iMailboxApi.CommDbUtilsL().GetIapIdL(
    			iapPreference->iIapRadioButton );

    	}

    CMuiuSettingsEditText* mailboxName =
        static_cast<CMuiuSettingsEditText*>(
            GetItem( TUid::Uid( EIMAWMailboxName ) ) );
    User::LeaveIfNull( mailboxName );

    // Create settings data object through the interface
    iMailboxApi.MailboxServicesImplL().FillCompulsorySettingsL(
        aSettings,
        protocol,
        *emailAddress->Text(),
        *incomingAddress->Text(),
        *outgoingAddress->Text(),
        iapPreference->iIap.iId,
        *mailboxName->Text() );

    // Store the exit code, so the settings will be saved
    iBaseUiFlags.SetFlag( EImumSettingSave );

    // Cleanup
    emailAddress = NULL;
    incomingAddress = NULL;
    outgoingAddress = NULL;
    iapPreference = NULL;
    mailboxName = NULL;

    }

/******************************************************************************

    Utilitites

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsWizard::CheckCompleted()
// ----------------------------------------------------------------------------
//
TBool CIMSSettingsWizard::CheckCompleted()
    {
    IMUM_CONTEXT( CIMSSettingsWizard::CheckCompleted, 0, KLogUi );

    // Get the array of main resource
    CMuiuDynFinderItemArray* array = GetResource( R_IMAS_WIZARD, EFalse );

    // Search through the array, and if all items are ready, return true
    TInt item = array->Count();
    TBool ok = item > 0;
    while ( --item >= 0 )
        {
        TMuiuFlags flags = ( *array )[item].iItem->iItemFlags;

        // All visible items are checked and one failure is enough to
        // make the array unfinished
        ok = flags.Flag( EMuiuDynItemPermanentlyHidden ) ||
             flags.Flag( EMuiuDynItemValidated ) && ok;
        }

    return ok;
    }

// -----------------------------------------------------------------------------
// CIMSSettingsWizard::PostponePopup()
// PostponePopup is used to move time forward
// when popup will be displayed. This is needed
// to prevent situation when user is typing
// e.g. email address and popup will be displayed
// after user is typed a half of address but
// original time has experied.
// -----------------------------------------------------------------------------
//
void CIMSSettingsWizard::PostponePopup()
    {
    IMUM_CONTEXT( CIMSSettingsWizard::PostponePopup, 0, KLogUi );
    IMUM_IN();

    if( iNoteDialog )
        {
        iNoteDialog->SetTimeDelayBeforeShow( KMsToWaitBeforeRePopup );
        iNoteDialog->ShowInfoPopupNote();
        }

    IMUM_OUT();
    }

// -----------------------------------------------------------------------------
// CIMSSettingsWizard::ShowMailSettingsPopupAfter()
// -----------------------------------------------------------------------------
//
CAknInfoPopupNoteController* CIMSSettingsWizard::ShowMailSettingsPopupAfterL(
    const TUid& aId,
    TInt aAfterMs )
    {
    IMUM_CONTEXT( CIMSSettingsWizard::ShowMailSettingsPopupAfterL, 0, KLogUi );

    CAknInfoPopupNoteController* noteDialog = NULL;
    switch ( aId.iUid )
        {
        case EIMAWProtocol:
            noteDialog = CIMSSettingsNoteUi::ShowPopupL( 
                R_IMUM_POPUP_MAILBOX_TYPE, aAfterMs );
            break;

        case EIMAWEmailAddress:
            noteDialog = CIMSSettingsNoteUi::ShowPopupL(
                R_IMUM_POPUP_MY_EMAIL_ADDRESS, aAfterMs );
            break;

        case EIMAWSendingServer:
            noteDialog = CIMSSettingsNoteUi::ShowPopupL(
                R_IMUM_POPUP_OUTGOING_SERVER, aAfterMs );
            break;

        case EIMAWReceivingServer:
            noteDialog = CIMSSettingsNoteUi::ShowPopupL(
                R_IMUM_POPUP_INCOMING_SERVER, aAfterMs );
            break;

        case EIMAWIap:
            noteDialog = CIMSSettingsNoteUi::ShowPopupL(
                R_IMUM_POPUP_DEFINING_ACCESS_POINT, aAfterMs );
            break;

        case EIMAWMailboxName:
            noteDialog = CIMSSettingsNoteUi::ShowPopupL(
                R_IMUM_POPUP_MAILBOX_NAME, aAfterMs );
            break;

        default:
            __ASSERT_DEBUG( EFalse,
                User::Panic( KIMSWizardPanic, KErrUnknown ) );
            break;
        }

    return noteDialog;
    }

// -----------------------------------------------------------------------------
// CIMSSettingsWizard::CallExternalMtmWizardL()
// Calls added-on MTM-wizard
// -----------------------------------------------------------------------------
//
void CIMSSettingsWizard::CallExternalMtmWizardL( const TUid& aMtmUid, TBool& aCompleted )
	{
	IMUM_CONTEXT( CIMSSettingsWizard::CallExternalMtmWizardL, 0, KLogUi );
	IMUM_IN();

	TInt completed( 0 );
    TPckgBuf<TInt> buf( completed );

	CBaseMtmUi& mtmUi = iMtmStore->ClaimMtmUiL( aMtmUid );

    CMsvEntrySelection* emptySelection = new (ELeave) CMsvEntrySelection;
    CleanupStack::PushL( emptySelection );

	TRAP_IGNORE( mtmUi.InvokeSyncFunctionL(
		KMtmUiFunctionSettingsWizard, *emptySelection, buf) );

	iMtmStore->ReleaseMtmUi( aMtmUid );

	CleanupStack::PopAndDestroy( emptySelection );
	emptySelection = NULL;

    aCompleted = (buf() != 1);

    // Makes the wizard exit
    if ( aCompleted )
        {
       	SetFlag( EWizFlagExiting );
    	SetFlag( EWizFlagForcedCancel );
    	IMUM0(0, "MTM wizard has returned an exit code!");
        }

    IMUM_OUT();
	}

//  End of File
