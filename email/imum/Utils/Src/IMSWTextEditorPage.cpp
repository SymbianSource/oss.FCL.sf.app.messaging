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
* Description:  IMSWTextEditorPage.cpp
*
*/



// INCLUDE FILES
#include <aknnavide.h>                      // CAknNavigationDecorator
#include <aknEditStateIndicator.h>
#include <AknIndicatorContainer.h>
#include <e32base.h>

#include "IMSSettingsWizard.h"
#include "IMSWTextEditorPage.h"
#include "ImumUtilsLogging.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
const TText KIMASCharAt = '@';

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::CIMSSettingsWizardEditTextPage()
// ----------------------------------------------------------------------------
CIMSSettingsWizardEditTextPage::CIMSSettingsWizardEditTextPage(
    CIMSSettingsWizard& aWizard,
    TInt aResourceId,
    TDes& aText,
    TDesC& aPageText,
    TInt aTextSettingPageFlags )
    :
    CAknTextSettingPage( aResourceId, aText, aTextSettingPageFlags ),
    iWizard( aWizard ),
    iText( aText ),
    iPageText( aPageText )
    {
    IMUM_CONTEXT( CIMSSettingsWizardEditTextPage::CIMSSettingsWizardEditTextPage, 0, KLogUi );

    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::ConstructL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizardEditTextPage::ConstructL()
    {
    IMUM_CONTEXT( CIMSSettingsWizardEditTextPage::ConstructL, 0, KLogUi );

    CAknTextSettingPage::ConstructL();
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::PostDisplayCheckL()
// ----------------------------------------------------------------------------
//
TBool CIMSSettingsWizardEditTextPage::PostDisplayCheckL()
    {
    IMUM_CONTEXT( CIMSSettingsWizardEditTextPage::PostDisplayCheckL, 0, KLogUi );

    // Set the text to navipane
    UpdateIndicatorL();

    // Handle text selections
    UpdateTextEditorL();

    // Reset popup
    iWizard.PostponePopup();

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::~CIMSSettingsWizardEditTextPage()
// ----------------------------------------------------------------------------
//
CIMSSettingsWizardEditTextPage::~CIMSSettingsWizardEditTextPage()
    {
    IMUM_CONTEXT( CIMSSettingsWizardEditTextPage::~CIMSSettingsWizardEditTextPage, 0, KLogUi );

    MAknEditingStateIndicator* ptr = iAvkonEnv->EditingStateIndicator();

    if ( ptr )
        {
        CAknIndicatorContainer* iIndicatorContainer =
            ptr->IndicatorContainer();

        // Make sure the pointer to indicator exist
        if ( iIndicatorContainer )
            {
            iIndicatorContainer->SetIndicatorState(
                TUid::Uid( EAknNaviPaneEditorIndicatorMessageLength ),
                EAknIndicatorStateOff );
            }
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::NewL()
// ----------------------------------------------------------------------------
//
CIMSSettingsWizardEditTextPage* CIMSSettingsWizardEditTextPage::NewL(
    CIMSSettingsWizard& aWizard,
    TInt aResourceId,
    TDes& aText,
    TDesC& aPageText,
    TInt aTextSettingPageFlags )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsWizardEditTextPage::NewL, 0, utils, KLogUi );

    CIMSSettingsWizardEditTextPage* self = NewLC(
        aWizard, aResourceId, aText, aPageText, aTextSettingPageFlags );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::NewLC()
// ----------------------------------------------------------------------------
//
CIMSSettingsWizardEditTextPage* CIMSSettingsWizardEditTextPage::NewLC(
    CIMSSettingsWizard& aWizard,
    TInt aResourceId,
    TDes& aText,
    TDesC& aPageText,
    TInt aTextSettingPageFlags )
    {
    IMUM_STATIC_CONTEXT( CIMSSettingsWizardEditTextPage::NewLC, 0, utils, KLogUi );

    CIMSSettingsWizardEditTextPage* self =
        new ( ELeave ) CIMSSettingsWizardEditTextPage(
            aWizard, aResourceId, aText, aPageText, aTextSettingPageFlags );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::ProcessCommandL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizardEditTextPage::ProcessCommandL( TInt aCommandId )
    {
    IMUM_CONTEXT( CIMSSettingsWizardEditTextPage::ProcessCommandL, 0, KLogUi );

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
        default:
            // Process edit commands (eg. number/alpha mode, spec. char table)
            CAknTextSettingPage::ProcessCommandL( aCommandId );
            break;
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::KeyPressedNextL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizardEditTextPage::KeyPressedNextL()
    {
    IMUM_CONTEXT( CIMSSettingsWizardEditTextPage::KeyPressedNextL, 0, KLogUi );

    AttemptExitL( ETrue );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::KeyPressedBackL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizardEditTextPage::KeyPressedBackL()
    {
    IMUM_CONTEXT( CIMSSettingsWizardEditTextPage::KeyPressedBackL, 0, KLogUi );

    AttemptExitL( EFalse );
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::KeyPressedSelectL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizardEditTextPage::KeyPressedSelectL()
    {
    IMUM_CONTEXT( CIMSSettingsWizardEditTextPage::KeyPressedSelectL, 0, KLogUi );

    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::UpdateIndicatorL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizardEditTextPage::UpdateIndicatorL()
    {
    IMUM_CONTEXT( CIMSSettingsWizardEditTextPage::UpdateIndicatorL, 0, KLogUi );

    MAknEditingStateIndicator* ptr = iAvkonEnv->EditingStateIndicator();

    if ( ptr )
        {
        CAknIndicatorContainer* iIndicatorContainer =
            ptr->IndicatorContainer();

        // Make sure the pointer to indicator exist
        if ( iIndicatorContainer )
            {
            iIndicatorContainer->SetIndicatorState(
                TUid::Uid( EAknNaviPaneEditorIndicatorMessageLength ),
                EAknIndicatorStateOn );
            iIndicatorContainer->SetIndicatorValueL(
                TUid::Uid( EAknNaviPaneEditorIndicatorMessageLength ),
                iPageText );
            }
        }
    }

// ----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::UpdateTextEditorL()
// ----------------------------------------------------------------------------
//
void CIMSSettingsWizardEditTextPage::UpdateTextEditorL()
    {
    IMUM_CONTEXT( CIMSSettingsWizardEditTextPage::UpdateTextEditorL, 0, KLogUi );

    // Check if the text contains '@' in the first char
    if ( iText.Size() && iText[0] == KIMASCharAt )
        {
        // Remove selection and move the cursor at the beginning
        CEikEdwin* edwin = TextControl();
        edwin->ClearSelectionL();
        edwin->SetCursorPosL( 0, EFalse );
        edwin = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::OfferKeyEventL()
// This is needed to postpone popup time
// when user is writing. Popup should apper
// only when user has been "idle" for some time.
// -----------------------------------------------------------------------------
//
TKeyResponse CIMSSettingsWizardEditTextPage::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    IMUM_CONTEXT( CIMSSettingsWizardEditTextPage::OfferKeyEventL, 0, KLogUi );

    if ( aType == EEventKeyDown )
        {
        iWizard.PostponePopup();
        }
    return CAknTextSettingPage::OfferKeyEventL( aKeyEvent, aType );
    }

#ifdef RD_SCALABLE_UI_V2
// -----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::HandlePointerEventL()
// This is needed to postpone popup time
// when user is tapping with stylus. Popup should apper
// only when user has been "idle" for some time.
// -----------------------------------------------------------------------------
//
void CIMSSettingsWizardEditTextPage::HandlePointerEventL(
    const TPointerEvent& aPointerEvent)
    {
    IMUM_CONTEXT( CIMSSettingsWizardEditTextPage::HandlePointerEventL, 0, KLogUi );
    IMUM_IN();

    if( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        iWizard.PostponePopup();
        }

    CAknTextSettingPage::HandlePointerEventL( aPointerEvent );
    IMUM_OUT();
    }

// -----------------------------------------------------------------------------
// CIMSSettingsWizardEditTextPage::HandleControlEventL()
// This is needed to postpone popup time
// when user is writing with stylus. Popup should apper
// only when user has been "idle" for some time.
// -----------------------------------------------------------------------------
//
void CIMSSettingsWizardEditTextPage::HandleControlEventL(
    CCoeControl* aControl, TCoeEvent aEventType )
    {
    IMUM_CONTEXT( CIMSSettingsWizardEditTextPage::HandleControlEventL, 0, KLogUi );
    IMUM_IN();

    if( aEventType == EEventStateChanged )
        {
        iWizard.PostponePopup();
        }

    CAknTextSettingPage::HandleControlEventL( aControl, aEventType );
    IMUM_OUT();
    }

#endif  //RD_SCALABLE_UI_V2

//  End of File
