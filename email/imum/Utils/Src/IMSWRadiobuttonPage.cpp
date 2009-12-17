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
* Description:  IMSWRadioButtonPage.cpp
*
*/



// INCLUDE FILES
#include <aknnavide.h>                      // CAknNavigationDecorator
#include <aknEditStateIndicator.h>
#include <AknIndicatorContainer.h>
#include <e32base.h>

#include "IMSSettingsWizard.h"
#include "IMSWRadiobuttonPage.h"
#include "ImumUtilsLogging.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIMSSettingsWizardRadioButtonPage::CIMSSettingsWizardRadioButtonPage()
// ----------------------------------------------------------------------------
//
CIMSSettingsWizardRadioButtonPage::CIMSSettingsWizardRadioButtonPage(
    CIMSSettingsWizard& aWizard,
    TInt aResourceId,
    TInt& aCurrentIndex,
    const MDesCArray* aItemArray,
    TDesC& aPageText )
    :
    CAknRadioButtonSettingPage( aResourceId, aCurrentIndex, aItemArray ),
    iWizard( aWizard ),
    iPageText( aPageText ),
    iNaviPane( NULL ),
    iNaviDecorator( NULL )
    {
    IMUM_CONTEXT( CIMSSettingsWizardRadioButtonPage::CIMSSettingsWizardRadioButtonPage, 0, KLogUi );

    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardRadioButtonPage::ConstructL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizardRadioButtonPage::ConstructL()
    {
    IMUM_CONTEXT( CIMSSettingsWizardRadioButtonPage::ConstructL, 0, KLogUi );

    CAknRadioButtonSettingPage::ConstructL();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardRadioButtonPage::PostDisplayCheckL()
// ----------------------------------------------------------------------------
//
TBool CIMSSettingsWizardRadioButtonPage::PostDisplayCheckL()
    {
    IMUM_CONTEXT( CIMSSettingsWizardRadioButtonPage::PostDisplayCheckL, 0, KLogUi );

    // Set the text to navipane
    UpdateNaviPaneL();

    // Reset popup
    iWizard.PostponePopup();

    return ETrue;
    }


// ----------------------------------------------------------------------------
// CIMSSettingsWizardRadioButtonPage::~CIMSSettingsWizardRadioButtonPage()
// ----------------------------------------------------------------------------
//
CIMSSettingsWizardRadioButtonPage::~CIMSSettingsWizardRadioButtonPage()
    {
    IMUM_CONTEXT( CIMSSettingsWizardRadioButtonPage::~CIMSSettingsWizardRadioButtonPage, 0, KLogUi );
    IMUM_IN();

    // Remove the pages -text from the
    if ( iNaviPane && iNaviDecorator )
        {
        iNaviPane->Pop( iNaviDecorator );
        }

    delete iNaviDecorator;
    iNaviDecorator = NULL;
    iNaviPane = NULL;

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardRadioButtonPage::NewL()
// ----------------------------------------------------------------------------
//
CIMSSettingsWizardRadioButtonPage* CIMSSettingsWizardRadioButtonPage::NewL(
    CIMSSettingsWizard& aWizard,
    TInt aResourceId,
    TInt& aCurrentIndex,
    const MDesCArray* aItemArray,
    TDesC& aPageText )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsWizardRadioButtonPage::NewL, 0, utils, KLogUi );

    CIMSSettingsWizardRadioButtonPage* self = NewLC(
        aWizard, aResourceId, aCurrentIndex, aItemArray, aPageText );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardRadioButtonPage::NewLC()
// ----------------------------------------------------------------------------
//
CIMSSettingsWizardRadioButtonPage* CIMSSettingsWizardRadioButtonPage::NewLC(
    CIMSSettingsWizard& aWizard,
    TInt aResourceId,
    TInt& aCurrentIndex,
    const MDesCArray* aItemArray,
    TDesC& aPageText )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsWizardRadioButtonPage::NewLC, 0, utils, KLogUi );

    CIMSSettingsWizardRadioButtonPage* self =
        new ( ELeave ) CIMSSettingsWizardRadioButtonPage(
            aWizard, aResourceId, aCurrentIndex, aItemArray, aPageText );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardRadioButtonPage::ProcessCommandL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizardRadioButtonPage::ProcessCommandL( TInt aCommandId )
    {
    IMUM_CONTEXT( CIMSSettingsWizardRadioButtonPage::ProcessCommandL, 0, KLogUi );
    IMUM_IN();

    // Reset info popup
    iWizard.PostponePopup();

    switch( aCommandId )
        {
        case EAknSoftkeyCancel: // falltrough
        case EAknCmdExit:
            // Exit setting page (dialog)
            AttemptExitL( EFalse );
            // falltrough
        case EAknSoftkeyBack: // falltrough
        case EAknSoftkeyDone:
            // Move to prev/next wizard page or exit app
            iWizard.ProcessCommandL( aCommandId );
            break;
        case EAknSoftkeyOk:
        case EAknSoftkeySelect:
        	// Selection was made, so select the correct protocol
        	CAknRadioButtonSettingPage::SelectCurrentItemL();
        	break;
        default:
            // Process any other commands. Radio button settings page doesn't
            // have any though.
            CAknRadioButtonSettingPage::ProcessCommandL( aCommandId );
            break;
        }

    IMUM_OUT();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardRadioButtonPage::KeyPressedNextL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizardRadioButtonPage::KeyPressedNextL()
    {
    IMUM_CONTEXT( CIMSSettingsWizardRadioButtonPage::KeyPressedNextL, 0, KLogUi );

    AttemptExitL( ETrue );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardRadioButtonPage::KeyPressedBackL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizardRadioButtonPage::KeyPressedBackL()
    {
    IMUM_CONTEXT( CIMSSettingsWizardRadioButtonPage::KeyPressedBackL, 0, KLogUi );

    AttemptExitL( EFalse );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardRadioButtonPage::KeyPressedSelectL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizardRadioButtonPage::KeyPressedSelectL()
    {
    IMUM_CONTEXT( CIMSSettingsWizardRadioButtonPage::KeyPressedSelectL, 0, KLogUi );

    SelectCurrentItemL();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardRadioButtonPage::UpdateNaviPaneL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizardRadioButtonPage::UpdateNaviPaneL()
    {
    IMUM_CONTEXT( CIMSSettingsWizardRadioButtonPage::UpdateNaviPaneL, 0, KLogUi );

    CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();
    iNaviPane = static_cast<CAknNavigationControlContainer*>(
        sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    iNaviDecorator = iNaviPane->CreateNavigationLabelL();
    iNaviPane->PushL( *iNaviDecorator );

    // Finally catch the label for showing the text
    CAknNaviLabel* naviLabel =
        static_cast<CAknNaviLabel*>( iNaviDecorator->DecoratedControl() );
    naviLabel->SetTextL( iPageText );
    }


// -----------------------------------------------------------------------------
// CIMSSettingsWizardRadioButtonPage::OfferKeyEventL()
// This is needed to postpone popup time
// when user is writing. Popup should apper
// only when user has been "idle" for some time.
// -----------------------------------------------------------------------------
//
TKeyResponse CIMSSettingsWizardRadioButtonPage::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    IMUM_CONTEXT( CIMSSettingsWizardRadioButtonPage::OfferKeyEventL, 0, KLogUi );
    IMUM_IN();

    if ( aType == EEventKeyDown )
        {
        iWizard.PostponePopup();
        }

    IMUM_OUT();
    return CAknRadioButtonSettingPage::OfferKeyEventL( aKeyEvent, aType );
    }


// -----------------------------------------------------------------------------
// CIMSSettingsWizardRadioButtonPage::HandleListBoxEventL
// Handles item selection
// -----------------------------------------------------------------------------
//
void CIMSSettingsWizardRadioButtonPage::HandleListBoxEventL(
		CEikListBox* /*aListBox*/,
        MEikListBoxObserver::TListBoxEvent aEventType )
	{
    IMUM_CONTEXT( CIMSSettingsWizardRadioButtonPage::HandleListBoxEventL, 0, KLogUi );
    IMUM_IN();

	switch( aEventType )
		{
		case EEventEnterKeyPressed:
		case EEventItemClicked:
			// Selection was made, so select the correct protocol
        	CAknRadioButtonSettingPage::SelectCurrentItemL();
			break;
		case EEventItemDoubleClicked:
			{
			// Item was selected with a double click
			// and Wizard moves to next page
			iWizard.ProcessCommandL( EAknSoftkeyDone );
			break;
			}
		default:
			// Do nothing
			break;
		}

	IMUM_OUT();
	}


#ifdef RD_SCALABLE_UI_V2
// -----------------------------------------------------------------------------
// CIMSSettingsWizardRadioButtonPage::HandlePointerEventL()
// This is needed to postpone popup time
// when user is tapping with stylus. Popup should apper
// only when user has been "idle" for some time.
// -----------------------------------------------------------------------------
//
void CIMSSettingsWizardRadioButtonPage::HandlePointerEventL(
    const TPointerEvent& aPointerEvent)
    {
    IMUM_CONTEXT( CIMSSettingsWizardRadioButtonPage::HandlePointerEventL, 0, KLogUi );
    IMUM_IN();

    if( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        iWizard.PostponePopup();
        }

    CAknRadioButtonSettingPage::HandlePointerEventL( aPointerEvent );
    IMUM_OUT();
    }
#endif  //RD_SCALABLE_UI_V2

//  End of File
