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
* Description:  MuiuDynamicSettingsDialog.cpp
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <eiktxlbx.h>               // CEikTextListBox
#include <badesca.h>                // CDesCArray
#include <eiktxlbm.h>               // CTextListBoxModel
#include <AknUtils.h>               // CAknTextUtils
#include <akntitle.h>               // CAknTitlePane
#include <AknQueryDialog.h>         // CAknQueryDialog
#include <aknsettingpage.h>         // CAknSettingPage
#include <aknradiobuttonsettingpage.h> // CAknRadioButtonSettingPage
#include <aknpasswordsettingpage.h> // CAknPasswordSettingPage
#include <akntextsettingpage.h>     // CAknTextSettingPage
#include <eikbtgpc.h>               // CEikButtonGroupContainer
#include <StringLoader.h>           // StringLoader
#include "MuiuDynamicSettingsDialog.h" // CMuiuDynamicSettingsDialog
#include "IMSWCheckBoxPage.h"   // CIMSSettingsWizardCheckBoxPage

#include <muiu_internal.rsg>
#include <ImumUtils.rsg>                 // R_IMAS_MSK_OPEN, should be temporary

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS

// Prevent any looping problems by defining maximum amount of editor openings
const TInt KMuiuMaxEditors = 100;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::CMuiuDynamicSettingsDialog()
// ----------------------------------------------------------------------------
//
CMuiuDynamicSettingsDialog::CMuiuDynamicSettingsDialog(
    CEikFormattedCellListBox& aListBox,
    CAknTitlePane& aTitlePane )
    :
    CMuiuDynamicSettingsArray(),
    iListBox( aListBox ),
    iTitlePane( aTitlePane ),
    iSettingValue( KErrNotFound )
    {
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::~CMuiuDynamicSettingsDialog()
// ----------------------------------------------------------------------------
//
CMuiuDynamicSettingsDialog::~CMuiuDynamicSettingsDialog()
    {
    // Remove the scrollbar from listbox
    iListBox.SetScrollBarFrame( NULL, CEikListBox::EOwnedExternally );

    if ( iSoftkeys )
        {
        iSoftkeys->SetObserver( EFalse, EFalse, EFalse, *this );
        delete iSoftkeys;
        }
    iSoftkeys = NULL;

    delete iSettingPageResources;
    iSettingPageResources = NULL;

    if ( iSettingsLastItemIndex )
        {
        iSettingsLastItemIndex->Reset();
        }

    delete iSettingsLastItemIndex;
    iSettingsLastItemIndex = NULL;

    if ( iCheckboxArray )
        {
        iCheckboxArray->ResetAndDestroy();
        }

    if ( iSettingsPageArray )
        {
        iSettingsPageArray->Reset();
        }

    if ( iBackupArray )
        {
        iBackupArray->ResetAndDestroy();
        }

    delete iBackupArray;
    iBackupArray = NULL;
    delete iSettingsPageArray;
    iSettingsPageArray = NULL;
    delete iCheckboxArray;
    iCheckboxArray = NULL;
    delete iOldTitleText;
    iOldTitleText = NULL;
    delete iQueryDialog;
    iQueryDialog = NULL;
    delete iSettingPage;
    iSettingPage = NULL;
    delete iButtonArray;
    iButtonArray = NULL;
    delete iSettingText;
    iSettingText = NULL;
    delete iNewPassword;
    iNewPassword = NULL;
    delete iOldPassword;
    iOldPassword = NULL;
    iScrollBar = NULL;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingsConstructL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SettingsConstructL(
    CEikButtonGroupContainer& aSoftkeys,
    const TInt aResource,
    const TBool aUpdateMSK,
    const TBool aLockedSettings )
    {
    BaseConstructL( aResource, aLockedSettings );

    // Set as observer
    iListBox.SetListBoxObserver( this );
    iListBox.View()->SetListEmptyTextL( KNullDesC );

    // Get scrollbar
    iScrollBar = iListBox.CreateScrollBarFrameL( ETrue );
    iScrollBar->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    // Create array
    CTextListBoxModel* model = iListBox.Model();
    model->SetItemTextArray( this );
    model->SetOwnershipType( ELbmDoesNotOwnItemArray );

    // Set this dialog for button listening
    iSoftkeys = CIMSSoftkeyControl::NewL( aSoftkeys );
    iSoftkeys->SetObserver( ETrue, ETrue, ETrue, *this );
    iSoftkeys->SetMSKLabelling( aUpdateMSK );
    UpdateMskL();

    // Prepare setting page arrays
    iSettingPageResources = CIMSPageResourceControl::NewL();
    iSettingsLastItemIndex =
        new ( ELeave ) CMuiuDynSetIntArray( KMuiuDynArrayGranularity );

    // Add array to store the backup items
    iBackupArray = new ( ELeave ) CMuiuDynSetItemArray( KMuiuDynArrayGranularity );

    // Define setting pages in enumeration order
    iSettingPageResources->CreateStackL( R_MUIU_SETTINGS_DIALOG_LISTBOX );
    iSettingPageResources->CreateStackL( R_MUIU_SETTINGS_DIALOG_CHECKLISTBOX );
    iSettingPageResources->CreateStackL( R_MUIU_SETTINGS_DIALOG_TEXT );
    iSettingPageResources->CreateStackL( R_MUIU_SETTINGS_DIALOG_NUMBER );
    iSettingPageResources->CreateStackL( R_MUIU_SETTINGS_DIALOG_SC_NUMBER );
    iSettingPageResources->CreateStackL( R_MUIU_SETTINGS_DIALOG_PASSWORD );
    iSettingPageResources->CreateStackL( NULL );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::HandleListBoxEventL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::HandleListBoxEventL(
    CEikListBox* /* aListBox */,
    TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
        // Note for TouchUi. When focused item is tapped, both
        // EEventItemClicked and EEventItemDoubleClicked events
        // are received. When unfocused item is tapped, EEventItemClicked
        // event is received. Double tap to focused item produces
        // the same events as a single tap.
        case EEventEnterKeyPressed:
        case EEventItemDoubleClicked:
        case EEventItemActioned:
            OpenSettingPageL();
            break;

        case EEventItemClicked:
        case EEventItemDraggingActioned:
            UpdateMskL();
            break;
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::OfferKeyEventL
// ----------------------------------------------------------------------------
//
TKeyResponse CMuiuDynamicSettingsDialog::OfferKeyEventL(
    const TKeyEvent& /* aKeyEvent */,
    TEventCode aType )
    {
    if ( aType == EEventKeyUp )
        {
        UpdateMskL();
        }

    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SetTitlePaneTextL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SetTitlePaneTextL(
    const TDesC& aText,
    const TBool aSaveCurrent )
    {
    HBufC* text = aText.Alloc();
    CleanupStack::PushL( text );

    if ( aSaveCurrent )
        {
        delete iOldTitleText;
        iOldTitleText = NULL;
        iOldTitleText = iTitlePane.Text()->Alloc();
        }

    iTitlePane.SetTextL( *text );
    CleanupStack::PopAndDestroy( text );
    text = NULL;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SetPreviousTitlePaneText()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SetPreviousTitlePaneText(
    const TBool aSaveCurrent )
    {
    TRAP_IGNORE( SetTitlePaneTextL( *iOldTitleText, aSaveCurrent ) );
    }

/******************************************************************************

    Settingpage opening

    - Find out the item to be edited
    - Send start editing event with the item
    - Check the item type
    - Check the resources
    - If no resources available, ask them
    - Start editing
    - Send end editing event

******************************************************************************/

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::OpenSettingPageL()
// ----------------------------------------------------------------------------
//
TMuiuPageResult CMuiuDynamicSettingsDialog::OpenSettingPageL(
    const TInt aIndex )
    {
    // To open setting page, check if the Setting Page index is provided,
    // if not get the current item index in array
    TInt index = ( aIndex == KErrNotFound ) ?
        iListBox.CurrentItemIndex() : aIndex;

    // Search for the item and open it
    return SettingPageOpenL( *GetItem( index ) );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::OpenSettingPageL()
// ----------------------------------------------------------------------------
//
TMuiuPageResult CMuiuDynamicSettingsDialog::OpenSettingPageL(
    CMuiuSettingBase& aBaseItem )
    {
    return SettingPageOpenL( aBaseItem );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingPageOpenL()
//
// This function contains the main logic for the setting editor handling
// in the settings dialog. The setting editors are kept opening as long
// as the all the settings are gone through or the editor is dismissed.
// ----------------------------------------------------------------------------
//
TMuiuPageResult CMuiuDynamicSettingsDialog::SettingPageOpenL(
    CMuiuSettingBase& aBaseItem )
    {
    // Don't open the editor for read-only items
    if ( aBaseItem.iItemFlags.Flag( EMuiuDynItemReadOnly ) )
        {
        return EMuiuPageResultReadOnly;
        }
    else if( aBaseItem.iItemFlags.Flag( EMuiuDynItemLockableSetting ) &&
        iArrayFlags.Flag( EMuiuEMailLockedSettings ) )
        {
        return EMuiuPageResultPageLocked;
        }

    // Store current focused item for last item index array
    iCurrentItemIndex = iListBox.CurrentItemIndex();

    // Starting editing of the item, do not allow updating during editing
    iListBox.MakeVisible( EFalse );
    iArrayFlags.ClearFlag( EMuiuDynArrayAllowRefresh );

    // When starting to edit page, send event so that item can be prepared
    // for the editing. In case of error, editing won't start
    TMuiuPageEventResult eventResult = EventItemEditStartsL( aBaseItem );
    TMuiuPageResult result;

    if ( eventResult == EMuiuPageEventResultApproved )
        {
        // Create the array if it doesn't exist yet
        if ( !iSettingsPageArray )
            {
            iSettingsPageArray = new ( ELeave ) CMuiuDynSetItemArray(
                KMuiuDynArrayGranularity );
            }

        // Clear the array and add the editable item to array
        CleanBackupArray();
        iSettingsPageArray->Reset();
        SetCurrentSettingsPage( aBaseItem );
        result = SettingsPageOpenCheckL();

        do
            {
            if ( result == EMuiuPageResultEditing )
                {
                // Open the setting page
                result = SettingPageOpenEditorL();

                // Do open check. Note that the result can be modified!
                SettingsCheckNextEditorOpen( result );
                }

            // Finish the editing of the item
            SettingPageOpenFinishL();
            }
        while ( result == EMuiuPageResultEditing );
        }
    else if ( eventResult == EMuiuPageEventResultDisapproved )
        {
        result = EMuiuPageResultOk;
        }
    else
        {
        result = EMuiuPageResultEventCancel;
        }

    // Send item edit ends
    EventItemEditEndsL( aBaseItem );
    aBaseItem.iItemFlags.ClearFlag( EMuiuDynItemOneTimeForceViewOpen );

    iArrayFlags.SetFlag( EMuiuDynArrayAllowRefresh );
    iListBox.MakeVisible( ETrue );

    // Update the listbox
    Refresh();

    return result;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::EventPrepareSubArrayL()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::EventPrepareSubArrayL(
    CMuiuSettingBase& aBaseItem,
    const TBool aForward )
    {
    // Check direction in array
    if ( aForward )
        {
        // Store the index and set to beginning
        SetSettingsLastItemIndexL( iCurrentItemIndex );
        iListBox.SetCurrentItemIndex( 0 );
        }
    // When going backwards
    else
        {
        // Restore previous index
        iListBox.SetCurrentItemIndex( SettingsLastItemIndex() );
        }

    return EventSubArrayChangeL( aBaseItem );
    }

/******************************************************************************

    Settingpage creation

******************************************************************************/

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingsPageOpenCheckL()
// ----------------------------------------------------------------------------
//
TMuiuPageResult CMuiuDynamicSettingsDialog::SettingsPageOpenCheckL()
    {
    CMuiuSettingBase* base = CurrentSettingsPage();

    // Before starting to open the page, check that item type is not
    // menuarray. In case of menuarray, just open the next level
    if ( HandleStackForwardL( *base ) == EMuiuStackResultSubMenuOpen )
        {
        UpdateMskL();
        return EMuiuPageResultSubMenuOpen;
        }

    // Don't open read only items
    if ( base->iItemFlags.Flag( EMuiuDynItemReadOnly ) )
        {
        return EMuiuPageResultReadOnly;
        }

    // The page is ok is for edit
    return EMuiuPageResultEditing;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingsCheckNextEditorOpen()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SettingsCheckNextEditorOpen(
    TMuiuPageResult& aResult )
    {
    // The settings user have selected, must be available to user.
    // For this reason, items shall restored to main array only, when
    // returning from editors

    switch ( aResult )
        {
        // Handle approved
        case EMuiuPageResultOk:
            aResult = HandleEditorOk();
            break;

        // Handle the cancelled editor page
        case EMuiuPageResultCancelled:
            aResult = HandleEditorCancel();
            break;

        // Continue editing
        case EMuiuPageResultEditing:
            aResult = HandleEditorDisapproved();
            break;

        default:
            __ASSERT_DEBUG( EFalse, User::Panic(
                KMuiuDynSetDialogPanic, KErrUnknown ) );
            break;
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::HandleEditorCancel()
// ----------------------------------------------------------------------------
//
TMuiuPageResult CMuiuDynamicSettingsDialog::HandleEditorCancel()
    {
    // The editor was cancelled, so the first check needed to make
    // is deterimine, if the editor was first in the line.
    TMuiuPageResult result = EMuiuPageResultInvalidResult;

    TInt editorCount = iSettingsPageArray->Count();
    if ( editorCount == 1 )
        {
        // Restore all of the items
        RestoreFromBackup();
        result = EMuiuPageResultCancelled;
        }
    // More than one page in the array, just jump back to the previous
    // and remove the previous value
    else if ( editorCount > 0 )
        {
        RemoveLastBackupItem();
        iSettingsPageArray->Delete( editorCount-1 );
        result = EMuiuPageResultEditing;
        }
    // Array is empty, unexpected situation
    else
        {
        __ASSERT_DEBUG( EFalse, User::Panic(
            KMuiuDynSetDialogPanic, KErrNotFound ) );
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::HandleEditorOk()
// ----------------------------------------------------------------------------
//
TMuiuPageResult CMuiuDynamicSettingsDialog::HandleEditorOk()
    {
    CMuiuSettingBase* nextItem = NULL;

    // Check if current selection contains subeditors
    if ( iSettingValue >= 0 )
        {
        nextItem = SettingFindEditor(
            *CurrentSettingsPage(), iSettingValue );
        }

    TMuiuPageResult result = EMuiuPageResultInvalidResult;

    // In case subeditor is found, set the item to list and continue
    // editing
    if ( nextItem )
        {
        SetCurrentSettingsPage( *nextItem );
        result = EMuiuPageResultEditing;
        }
    // Cleanup the array and stop editing
    else
        {
        CleanBackupArray();
        result = EMuiuPageResultOk;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::HandleEditorDisapproved()
// ----------------------------------------------------------------------------
//
TMuiuPageResult CMuiuDynamicSettingsDialog::HandleEditorDisapproved()
    {
    // Get current setting page
    CurrentSettingsPage()->SetText( iSettingText );

    return EMuiuPageResultEditing;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingPageOpenEditorL()
// ----------------------------------------------------------------------------
//
TMuiuPageResult CMuiuDynamicSettingsDialog::SettingPageOpenEditorL()
    {
    CMuiuSettingBase* currentEditor = CurrentSettingsPage();

    // Prepare setting page. The setting page shall be acquired
    // from the owner class, if required, or the setting page is
    // created with default values
    TMuiuDynSetUpdateMode updateMode;
    TMuiuFlags editorFlags = currentEditor->iItemFlags;

    // If enter pressed is set, the setting page won't be opened,
    // which means creation of page is not needed either
    if ( !editorFlags.Flag( EMuiuDynItemEnterPressed ) ||
         editorFlags.Flag( EMuiuDynItemOneTimeForceViewOpen ) )
        {
        if ( editorFlags.Flag( EMuiuDynItemAcquireCustomPage ) )
            {
            SettingPrepareAcquireL( updateMode, *currentEditor );
            __ASSERT_DEBUG( iSettingPage != NULL,
                User::Panic( KMuiuDynSetDialogPanic, KErrUnknown ) );
            }
        else
            {
            SettingCreateEditorL( *currentEditor, updateMode );
            }
        }

    // Finally launch the editor
    return SettingLaunchPageLD( *currentEditor, updateMode );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingPageOpenFinishL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SettingPageOpenFinishL()
    {
    // Clean up
    iDynSettingsFlags.ClearFlag( EMuiuDynSetRunningCustomPage );
    delete iButtonArray;
    iButtonArray = NULL;
    if ( iCheckboxArray )
        {
        iCheckboxArray->ResetAndDestroy();
        }
    delete iCheckboxArray;
    iCheckboxArray = NULL;
    iSettingPage = NULL;
    iSettingValue = KErrNotFound;
    delete iSettingText;
    iSettingText = NULL;
    delete iNewPassword;
    iNewPassword = NULL;
    delete iOldPassword;
    iOldPassword = NULL;
    }

/******************************************************************************

    Settingpage Open

******************************************************************************/

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::SettingFindEditor()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CMuiuDynamicSettingsDialog::SettingFindEditor(
    const CMuiuSettingBase& aBaseItem,
    const TInt aIndex,
    const TBool aExcludeHidden )
    {
    // The settings structure is constructed in a way, that the radiobutton
    // or the checkbox editor has the array of buttons. To retrieve the editor
    // first the radiobutton has to be retrieved, then inside the radiobutton,
    // the editor can be taken.
    CMuiuSettingBase* volunteerBase =
        GetSubItem( aBaseItem, aIndex, aExcludeHidden );

    // Make sure the item is found and check that it is
    if ( volunteerBase )
        {
        volunteerBase = GetSubItem( *volunteerBase, 0, EFalse );
        }
    else
        {
        volunteerBase = NULL;
        }

    // return the item
    return volunteerBase;
    }

// ----------------------------------------------------------------------------
// CIMSSettingsDialog::SettingFindEditor()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CMuiuDynamicSettingsDialog::SettingFindEditor(
    const CMuiuSettingBase& aBaseItem,
    const TUid& aId,
    const TBool aExcludeHidden )
    {
    // The settings structure is constructed in a way, that the radiobutton
    // or the checkbox editor has the array of buttons. To retrieve the editor
    // first the radiobutton has to be retrieved, then inside the radiobutton,
    // the editor can be taken.
    CMuiuSettingBase* volunteerBase =
        GetSubItem( aBaseItem, aId, aExcludeHidden );

    // Make sure the item is found and check that it is
    if ( volunteerBase )
        {
        volunteerBase = GetSubItem( *volunteerBase, 0, EFalse );
        }
    else
        {
        volunteerBase = NULL;
        }

    // return the item
    return volunteerBase;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingPrepareAcquireRadioButtonPageL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SettingPrepareAcquireRadioButtonPageL(
    CMuiuSettingBase& aBaseItem )
    {
    // Convert the base item to link item and define button array
    CMuiuSettingsLink* radio = static_cast<CMuiuSettingsLink*>( &aBaseItem );

    // Make sure the link exist
    if ( !radio->iItemLinkArray )
        {
        radio->iItemLinkArray =
            new ( ELeave ) CMuiuDynSetItemArray( KMuiuDynArrayGranularity );
        }

    SettingCreateButtonArrayL( *radio->iItemLinkArray );

    iSettingValue = radio->Value();
    radio = NULL;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingPrepareAcquireCheckboxPageL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SettingPrepareAcquireCheckboxPageL(
    CMuiuSettingBase& aBaseItem )
    {
    // Convert the base item to link item and define button array
    CMuiuSettingsLink* link = static_cast<CMuiuSettingsLink*>( &aBaseItem );

    // Make sure the link array exist
    if ( link->iItemLinkArray )
        {
        SettingCreateButtonArrayL( *link->iItemLinkArray );
        }

    link = NULL;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingPrepareAcquireTextEditorPageL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SettingPrepareAcquireTextEditorPageL(
    CMuiuSettingBase& aBaseItem )
    {
    // Create either password or plain texts
    if ( aBaseItem.iItemFlags.Flag( EMuiuDynItemPassword ) )
        {
        iNewPassword = new ( ELeave ) TMuiuPasswordText( *aBaseItem.Text() );
        iOldPassword = new ( ELeave ) TMuiuPasswordText( *aBaseItem.Text() );
        }
    else
        {
        iSettingText = new ( ELeave ) TMuiuSettingsText( *aBaseItem.Text() );
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingPrepareAcquireValueEditorPageL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SettingPrepareAcquireValueEditorPageL(
    CMuiuSettingBase& aBaseItem )
    {
    iSettingValue = aBaseItem.Value();
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingPrepareAcquireL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SettingPrepareAcquireL(
    TMuiuDynSetUpdateMode& aUpdateMode,
    CMuiuSettingBase& aBaseItem )
    {
    // Handle the item, based on the type
    switch ( aBaseItem.Type() )
        {
        // Radio button page
        case EMuiuDynSetRadioButtonArray:
            SettingPrepareAcquireRadioButtonPageL( aBaseItem );
            break;

        // Checkbox page
        case EMuiuDynSetCheckBoxArray:
            SettingPrepareAcquireCheckboxPageL( aBaseItem );
            break;

        // Text editor
        case EMuiuDynSetItemEditText:
            SettingPrepareAcquireTextEditorPageL( aBaseItem );
            break;

        // Number editor
        case EMuiuDynSetItemEditValue:
            SettingPrepareAcquireValueEditorPageL( aBaseItem );
            break;

        // Type is not recognized
        default:
            __ASSERT_DEBUG( EFalse,
                User::Panic( KMuiuDynSetDialogPanic, KErrUnknown ) );
            User::Leave( KErrUnknown );
        }

    // Acquire the custom setting page
    AcquireCustomSettingPageL(
        iSettingPage, aUpdateMode, iSettingValue,
        *iSettingText, *iButtonArray, aBaseItem );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::AcquireCustomSettingPageL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::AcquireCustomSettingPageL(
    CAknSettingPage*& /* aReturnPage */,
    TMuiuDynSetUpdateMode& /* aReturnUpdateMode */,
    TInt& /* aReturnSettingValue */,
    TMuiuSettingsText& /* aReturnSettingText */,
    CDesCArrayFlat& /* aParamRadioButtonArray */,
    CMuiuSettingBase& /* aParamBaseItem */ )
    {
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingCreateEditorL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SettingCreateEditorL(
    CMuiuSettingBase& aBaseItem,
    TMuiuDynSetUpdateMode& aUpdateMode )
    {
    // Create different editors for password settings
    if ( aBaseItem.iItemFlags.Flag( EMuiuDynItemPassword ) )
        {
        switch ( aBaseItem.iItemType )
            {
            case EMuiuDynSetItemEditText:
                SettingCreatePasswordTextEditorPageL(
                    aBaseItem, aUpdateMode );
                break;

            }
        }
    // Normal editors
    else
        {
        switch ( aBaseItem.iItemType )
            {
            case EMuiuDynSetRadioButtonArray:
                SettingCreateRadioButtonPageL( aBaseItem, aUpdateMode );
                break;

            case EMuiuDynSetCheckBoxArray:
                SettingCreateCheckboxPageL( aBaseItem, aUpdateMode );
                break;

            case EMuiuDynSetItemEditText:
                SettingCreatePlainTextEditorPageL(
                    aBaseItem, aUpdateMode );
                break;

            case EMuiuDynSetItemEditValue:
                SettingCreatePlainNumberEditorPageL(
                    aBaseItem, aUpdateMode );
                break;

            case EMuiuDynSetItemEditScValue:
                SettingCreateScNumberEditorPageL(
                    aBaseItem, aUpdateMode );
                break;

            default:
                break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::DefineDefaultSettingPageL()
// ----------------------------------------------------------------------------
//
TMuiuDynSetUpdateMode CMuiuDynamicSettingsDialog::DefineDefaultSettingPageL(
    const CMuiuSettingBase& aBase )
    {
    // Create the aSetting page
    iSettingPage->SetSettingTextL( *aBase.iItemLabel );

    // Determine the update mode
    return aBase.iItemFlags.Flag( EMuiuDynItemUpdateOnChange ) ?
            CAknSettingPage::EUpdateWhenChanged :
            CAknSettingPage::EUpdateWhenAccepted;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingCreateButtonArrayL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SettingCreateButtonArrayL(
    CMuiuDynSetItemArray& aButtonArray )
    {
    // Get the item count in array
    const TInt count = aButtonArray.Count();
    iButtonArray = new ( ELeave ) CDesCArrayFlat( KMuiuDynArrayGranularity );

    // Do number conversion to all items and add them to temporary array
    for ( TInt item = 0; item < count; item++ )
        {
        CMuiuSettingBase* base = aButtonArray[item];

        // Append only visible items on the list
        if ( !IsHidden( *base ) )
            {
            HBufC* label = base->iItemLabel->AllocLC();
            TPtr labelPtr = label->Des();

            // Do number conversion when needed
            if ( base->iItemFlags.Flag( EMuiuDynItemLangSpecificNumConv ) )
                {
                AknTextUtils::DisplayTextLanguageSpecificNumberConversion(
                    labelPtr );
                }

            // Insert to array and delete old item
            iButtonArray->AppendL( *label );
            CleanupStack::PopAndDestroy( label );
            label = NULL;
            }
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingCreateCheckboxArrayL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SettingCreateCheckboxArrayL(
    CMuiuDynSetItemArray& aButtonArray )
    {
    // Get the item count in array
    const TInt count = aButtonArray.Count();
    iCheckboxArray =
        new ( ELeave ) CSelectionItemList( KMuiuDynArrayGranularity );

    // Do number conversion to all items and add them to temporary array
    for ( TInt item = 0; item < count; item++ )
        {
        CMuiuSettingBase* base = aButtonArray[item];

        // Append only visible items on the list
        if ( !IsHidden( *base ) )
            {
            HBufC* label = base->iItemLabel->AllocLC();
            TPtr labelPtr = label->Des();

            // Do number conversion when needed
            if ( base->iItemFlags.Flag( EMuiuDynItemLangSpecificNumConv ) )
                {
                AknTextUtils::DisplayTextLanguageSpecificNumberConversion(
                    labelPtr );
                }

            // Create item and check it
            CSelectableItem* checkbox =
                new ( ELeave ) CSelectableItem(
                    *label, base->iItemFlags.Flag( EMuiuDynItemChecked ) );
            CleanupStack::PushL( checkbox );
            checkbox->ConstructL();

            // Insert to array and delete old item
            iCheckboxArray->AppendL( checkbox );
            CleanupStack::Pop( checkbox );
            CleanupStack::PopAndDestroy( label );
            label = NULL;
            }
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingCreateRadioButtonPageL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SettingCreateRadioButtonPageL(
    CMuiuSettingBase& aBaseItem,
    TMuiuDynSetUpdateMode& aUpdateMode )
    {
    // Convert the base item to radiobutton item
    CMuiuSettingsLinkExtended* radioArray =
        static_cast<CMuiuSettingsLinkExtended*>( &aBaseItem );

    // Create button array for the page and set the selection for page
    SettingCreateButtonArrayL( *radioArray->iItemLinkArray );
    iSettingValue = radioArray->Value();

    // Create the setting page to member
    iSettingPage = ( new ( ELeave ) CAknRadioButtonSettingPage(
        ( *iSettingPageResources )[EIPRRadioButton],
        iSettingValue, iButtonArray ) );

    // Apply defaults
    aUpdateMode = DefineDefaultSettingPageL( aBaseItem );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingCreateCheckboxPageL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SettingCreateCheckboxPageL(
    CMuiuSettingBase& aBaseItem,
    TMuiuDynSetUpdateMode& aUpdateMode )
    {
    // Convert the base item to radiobutton item
    CMuiuSettingsLink* radioArray =
        static_cast<CMuiuSettingsLink*>( &aBaseItem );

    // Create button array for the page
    SettingCreateCheckboxArrayL( *radioArray->iItemLinkArray );

    // Create the setting page to member
    iSettingPage = new ( ELeave ) CIMSSettingsWizardCheckBoxPage(
        ( *iSettingPageResources )[EIPRCheckbox], iCheckboxArray,
        aBaseItem.iItemFlags.Flag( EMuiuDynItemForceMustFill ) );

    // Apply defaults
    aUpdateMode = DefineDefaultSettingPageL( aBaseItem );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingCreatePlainTextEditorPageL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SettingCreatePlainTextEditorPageL(
    CMuiuSettingBase& aBaseItem,
    TMuiuDynSetUpdateMode& aUpdateMode )
    {
    // Get the text to be edited
    CMuiuSettingsEditText* textEditor =
        static_cast<CMuiuSettingsEditText*>( &aBaseItem );
    iSettingText = new ( ELeave ) TMuiuSettingsText( *textEditor->Text() );
    iSettingText->Copy( *textEditor->Text() );

    // Define flags according to item
    TInt flags = GetTextEditorFlags( aBaseItem );

    // Create the setting page using the flags
    TInt pageResource = ( *iSettingPageResources )[EIPRText];
    CAknTextSettingPage* settingPage =
        new ( ELeave ) CAknTextSettingPage(
            pageResource, *iSettingText, flags );
    CleanupStack::PushL( settingPage );
    settingPage->ConstructL();
    CEikEdwin& txtCtrl = *settingPage->TextControl();
    txtCtrl.SetTextLimit( textEditor->iItemMaxLength );

    if ( textEditor->iItemFlags.Flag( EMuiuDynItemLatinOnly ) )
        {
        txtCtrl.SetAknEditorFlags(
            EAknEditorFlagLatinInputModesOnly | EAknEditorFlagNoT9 );
        }

    iSettingPage = settingPage;
    CleanupStack::Pop( settingPage );

    // Apply defaults
    aUpdateMode = DefineDefaultSettingPageL( aBaseItem );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingCreatePasswordTextEditorPageL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SettingCreatePasswordTextEditorPageL(
    CMuiuSettingBase& aBaseItem,
    TMuiuDynSetUpdateMode& aUpdateMode )
    {
    // Prepare password and save the old
    CMuiuSettingsEditText* textEditor =
        static_cast<CMuiuSettingsEditText*>( &aBaseItem );

    iNewPassword = new ( ELeave ) TMuiuPasswordText(
        textEditor->Text()->Left( KMuiuDynMaxPasswordLength ) );
    iOldPassword = new ( ELeave ) TMuiuPasswordText(
        textEditor->Text()->Left( KMuiuDynMaxPasswordLength ) );

    // Create the settings page
    iSettingPage = ( new ( ELeave ) CAknAlphaPasswordSettingPage(
        ( *iSettingPageResources )[EIPRSecret],
        *iNewPassword, *iOldPassword ) );
    iSettingPage->ConstructL();
    CAknAlphaPasswordSettingPage* pwPage =
        static_cast<CAknAlphaPasswordSettingPage*>( iSettingPage );
    pwPage->SetMaxPasswordLength( textEditor->iItemMaxLength );
    if ( pwPage->AlphaPasswordEditor() )
        {
        pwPage->AlphaPasswordEditor()->SetMaxLength(
            textEditor->iItemMaxLength );
        }

    // Apply defaults
    aUpdateMode = DefineDefaultSettingPageL( aBaseItem );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingCreatePlainNumberEditorPageL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SettingCreatePlainNumberEditorPageL(
    CMuiuSettingBase& aBaseItem,
    TMuiuDynSetUpdateMode& aUpdateMode )
    {
    // Conver item and set the value
    CMuiuSettingsEditValue* numberEditor =
        static_cast<CMuiuSettingsEditValue*>( &aBaseItem );
    iSettingValue = numberEditor->Value();

    // Define flags according to item
    TInt flags = GetValueEditorFlags( aBaseItem );

    // Create aSetting page
    iSettingPage = ( new ( ELeave ) CAknIntegerSettingPage(
        ( *iSettingPageResources )[EIPRNumber],
            *&iSettingValue , flags ) );
    iSettingPage->ConstructL();

    CEikEdwin* textControl = static_cast<CAknIntegerSettingPage*>(
                                        iSettingPage )->TextControl();


    textControl->SetTextLimit( numberEditor->iItemMaxLength );
    textControl->SetAknEditorNumericKeymap( EAknEditorPlainNumberModeKeymap );


    // Apply defaults, for number editors, the update mode has to be onchange
    aBaseItem.iItemFlags.SetFlag( EMuiuDynItemUpdateOnChange );
    aUpdateMode = DefineDefaultSettingPageL( aBaseItem );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingCreateScNumberEditorPageL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SettingCreateScNumberEditorPageL(
    CMuiuSettingBase& aBaseItem,
    TMuiuDynSetUpdateMode& aUpdateMode )
    {
    // Conver item and set the value
    CMuiuSettingsEditText* textEditor =
        static_cast<CMuiuSettingsEditText*>( &aBaseItem );
    iSettingText = new ( ELeave ) TMuiuSettingsText( *textEditor->Text() );
    iSettingText->Copy( *textEditor->Text() );

    if ( aBaseItem.iItemFlags.Flag( EMuiuDynItemLangSpecificNumConv ) )
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(
            *iSettingText );
        }

    // Define flags according to item
    TInt flags = CAknIntegerSettingPage::EInvalidValueNotOffered;
    flags |= !aBaseItem.iItemFlags.Flag( EMuiuDynItemForceMustFill ) &
        CAknIntegerSettingPage::EEmptyValueAllowed;
    flags |= aBaseItem.iItemFlags.Flag( EMuiuDynItemNoInitialSelection ) &
        CAknIntegerSettingPage::ENoInitialSelection;
    flags |= aBaseItem.iItemFlags.Flag( EMuiuDynItemEditorCursorAtBeginning ) &
        CAknIntegerSettingPage::EPutCursorAtBeginning;

    // Create aSetting page
    iSettingPage = ( new ( ELeave ) CAknTextSettingPage(
        ( *iSettingPageResources )[EIPRScNumber],
        *iSettingText, flags ) );
    iSettingPage->ConstructL();
    static_cast<CAknTextSettingPage*>( iSettingPage
        )->TextControl()->SetTextLimit( textEditor->iItemMaxLength );
    iSettingPage->SetSettingTextL( *textEditor->iItemLabel );

    // Apply defaults
    aUpdateMode = DefineDefaultSettingPageL( aBaseItem );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingLaunchPageLD()
// ----------------------------------------------------------------------------
//
TMuiuPageResult CMuiuDynamicSettingsDialog::SettingLaunchPageLD(
    CMuiuSettingBase& aBaseItem,
    TMuiuDynSetUpdateMode& aUpdateMode )
    {
    switch ( aBaseItem.Type() )
        {
        case EMuiuDynSetRadioButtonArray:
            return SettingLaunchRadioButtonPageLD(
                aBaseItem, aUpdateMode );

        case EMuiuDynSetCheckBoxArray:
            return SettingLaunchCheckboxPageLD( aBaseItem, aUpdateMode );

        case EMuiuDynSetItemEditText:
        case EMuiuDynSetItemEditScValue:
            return SettingLaunchTextEditorPageLD( aBaseItem, aUpdateMode );

        case EMuiuDynSetItemEditValue:
            return SettingLaunchNumberEditorPageLD( aBaseItem, aUpdateMode );

        // Multiline dialogs are launched from client only
        case EMuiuDynSetItemMultiLine:
            return SettingLaunchMultilinePageL( aBaseItem, aUpdateMode );

        default:
            __ASSERT_DEBUG( EFalse, User::Panic(
                KMuiuDynSetDialogPanic, KErrUnknown ) );
            return EMuiuPageResultCancelled;
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingLaunchRadioButtonPageLD()
// ----------------------------------------------------------------------------
//
TMuiuPageResult CMuiuDynamicSettingsDialog::SettingLaunchRadioButtonPageLD(
    CMuiuSettingBase& aBaseItem,
    TMuiuDynSetUpdateMode& aUpdateMode )
    {
    // Convert the base item to radio item
    CMuiuSettingsLink* linkArray = static_cast<CMuiuSettingsLink*>( &aBaseItem );
    TMuiuPageResult result = EMuiuPageResultOk;

    if ( linkArray->iItemFlags.Flag( EMuiuDynItemEnterPressed ) &&
         !linkArray->iItemFlags.Flag( EMuiuDynItemOneTimeForceViewOpen ) )
        {
        TInt value = !linkArray->Value();
        TMuiuPageEventResult evaluation =
            EventItemEvaluateRadioButton( *linkArray, value );

        // Handle approved item
        if ( evaluation == EMuiuPageEventResultApproved )
            {
            // Change the value and the text
            linkArray->SetValue( value );
            linkArray->iItemSettingText->Copy(
                *( *linkArray->iItemLinkArray
                    )[value]->iItemLabel );
            }

        result = EMuiuPageResultOk;
        }
    else
        {
        // Opening the editor, clear flag immediately
        linkArray->iItemFlags.ClearFlag( EMuiuDynItemOneTimeForceViewOpen );

        // Launch the settings dialog and update the item, if item is valid
        if ( iSettingPage->ExecuteLD( aUpdateMode ) > 0 )
            {
            iSettingPage = NULL;
            TMuiuPageEventResult evaluation =
                EventItemEvaluateRadioButton( *linkArray, iSettingValue );

            // Handle approved item
            if ( evaluation == EMuiuPageEventResultApproved )
                {
                // First store the index value from the visible radiobutton array
                linkArray->SetValue( iSettingValue );
                CMuiuSettingBase* subItem =
                    GetSubItem( *linkArray, iSettingValue );

                // Mark the item as checked and copy the text
                if ( subItem )
                    {
                    CheckRadioButton( *linkArray, subItem->iItemId );
                    linkArray->iItemSettingText->Copy( *subItem->iItemLabel );
                    }

                result = EMuiuPageResultOk;
                }
            // Handle disapproved item
            else if ( evaluation == EMuiuPageEventResultDisapproved )
                {
                result = EMuiuPageResultEditing;
                }
            // Handle cancelled item
            else
                {
                result = EMuiuPageResultCancelled;
                }
            }
        else
            {
            // Editor cancelled
            iSettingPage = NULL;
            result = EMuiuPageResultCancelled;
            }
        }

    linkArray = NULL;

    // Return the result
    return result;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingLaunchCheckboxPageLD()
// ----------------------------------------------------------------------------
//
TMuiuPageResult CMuiuDynamicSettingsDialog::SettingLaunchCheckboxPageLD(
    CMuiuSettingBase& aBaseItem,
    TMuiuDynSetUpdateMode& aUpdateMode )
    {
    // Convert the base item to link item
    CMuiuSettingsLinkExtended* checkboxArray =
        static_cast<CMuiuSettingsLinkExtended*>( &aBaseItem );
    TMuiuPageResult result = EMuiuPageResultOk;

    // Launch the settings dialog and update the item, if item is valid
    if ( iSettingPage->ExecuteLD( aUpdateMode ) )
        {
        iSettingPage = NULL;
        TMuiuPageEventResult evaluation =
            EventItemEvaluateValue( aBaseItem, iSettingValue );

        if ( evaluation == EMuiuPageEventResultApproved )
            {
            // Clean the value
            TInt checkbox = 0;

            // Check all items in the array, and set the flags to item
            for ( TInt item = iCheckboxArray->Count(); --item >= 0; )
                {
                TBool checked = iCheckboxArray->At( item )->SelectionStatus();
                checkboxArray->iItemLinkArray->At( item )->iItemFlags.ChangeFlag(
                    EMuiuDynItemChecked, checked );

                checkbox |= ( checked << item );
                }

            checkboxArray->SetValue( checkbox );

            result = EMuiuPageResultOk;
            }
        // Handle disapproved item
        else if ( evaluation == EMuiuPageEventResultDisapproved )
            {
            result = EMuiuPageResultEditing;
            }
        // Handle cancelled item
        else
            {
            result = EMuiuPageResultCancelled;
            }
        }
    else
        {
        // Editor cancelled
        iSettingPage = NULL;
        result = EMuiuPageResultCancelled;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingLaunchTextEditorPageLD()
// ----------------------------------------------------------------------------
//
TMuiuPageResult CMuiuDynamicSettingsDialog::SettingLaunchTextEditorPageLD(
    CMuiuSettingBase& aBaseItem,
    TMuiuDynSetUpdateMode& aUpdateMode )
    {
    TMuiuPageResult result = EMuiuPageResultOk;

    if ( iSettingPage->ExecuteLD( aUpdateMode ) > 0 )
        {
        iSettingPage = NULL;
        TMuiuSettingsText* settingText = new(ELeave) TMuiuSettingsText;

        // Depending on the text editor, choose the correct text
        if ( aBaseItem.iItemFlags.Flag( EMuiuDynItemPassword ) )
            {
            settingText->Copy( *iNewPassword );
            }
        else
            {
            settingText->Copy( *iSettingText );
            }

        TMuiuPageEventResult evaluation =
            EventItemEvaluateText( aBaseItem, *settingText );

        if ( evaluation == EMuiuPageEventResultApproved )
            {
            // Text is ok, store the text and close editor
            aBaseItem.SetText( settingText );
            }
        // Handle disapproved item
        else if ( evaluation == EMuiuPageEventResultDisapproved )
            {
            result = EMuiuPageResultEditing;
            }
        // Handle cancelled item
        else
            {
            result = EMuiuPageResultCancelled;
            }
        }
    else
        {
        iSettingPage = NULL;
        result = EMuiuPageResultCancelled;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingLaunchNumberEditorPageLD()
// ----------------------------------------------------------------------------
//
TMuiuPageResult CMuiuDynamicSettingsDialog::SettingLaunchNumberEditorPageLD(
    CMuiuSettingBase& aBaseItem,
    TMuiuDynSetUpdateMode& aUpdateMode )
    {
    TMuiuPageResult result = EMuiuPageResultOk;

    // Convert baseitem to number editor
    CMuiuSettingsEditValue* numberEdit =
        static_cast<CMuiuSettingsEditValue*>( &aBaseItem );

    // Launch the editor and fill the settings if approved
    if ( iSettingPage->ExecuteLD( aUpdateMode ) )
        {
        iSettingPage = NULL;
        TMuiuPageEventResult evaluation =
            EventItemEvaluateValue( aBaseItem, iSettingValue );

        // Evaluate item
        if ( evaluation == EMuiuPageEventResultApproved )
            {
            // Item ok
            numberEdit->SetValue( iSettingValue );
            numberEdit->iItemSettingText->Num( iSettingValue );
            result = EMuiuPageResultOk;
            }
        // Handle disapproved item
        else if ( evaluation == EMuiuPageEventResultDisapproved )
            {
            result = EMuiuPageResultEditing;
            }
        // Handle cancelled item
        else
            {
            result = EMuiuPageResultCancelled;
            }
        }
    else
        {
        // Editor cancelled
        iSettingPage = NULL;
        result = EMuiuPageResultCancelled;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingLaunchScNumberEditorPageLD()
// ----------------------------------------------------------------------------
//
TMuiuPageResult CMuiuDynamicSettingsDialog::SettingLaunchScNumberEditorPageLD(
    CMuiuSettingBase& aBaseItem,
    TMuiuDynSetUpdateMode& aUpdateMode )
    {
    // Convert baseitem to number editor
    CMuiuSettingsEditText* textEditor =
        static_cast<CMuiuSettingsEditText*>( &aBaseItem );
    TMuiuPageResult result = EMuiuPageResultOk;

    // Launch the editor and fill the settings if approved
    if ( iSettingPage->ExecuteLD( aUpdateMode ) )
        {
        iSettingPage = NULL;
        TMuiuPageEventResult evaluation =
            EventItemEvaluateText( aBaseItem, *iSettingText );

        if ( evaluation == EMuiuPageEventResultApproved )
            {
            // Item ok
            AknTextUtils::ConvertDigitsTo( *iSettingText, EDigitTypeWestern );
            textEditor->SetText( iSettingText );
            result = EMuiuPageResultOk;
            }
        // Handle disapproved item
        else if ( evaluation == EMuiuPageEventResultDisapproved )
            {
            result = EMuiuPageResultEditing;
            }
        // Handle cancelled item
        else
            {
            result = EMuiuPageResultCancelled;
            }
        }
    else
        {
        // Editor cancelled
        iSettingPage = NULL;
        result = EMuiuPageResultCancelled;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingLaunchMultilinePageL()
// ----------------------------------------------------------------------------
//
TMuiuPageResult CMuiuDynamicSettingsDialog::SettingLaunchMultilinePageL(
    CMuiuSettingBase& aBaseItem,
    TMuiuDynSetUpdateMode& aUpdateMode )
    {
    TMuiuPageEventResult result =
        SettingLaunchMultilineEditorL( aBaseItem, aUpdateMode );

    switch ( result )
        {
        case EMuiuPageEventResultApproved:
            return EMuiuPageResultOk;

        case EMuiuPageEventResultDisapproved:
            return EMuiuPageResultEditing;

        default:
        case EMuiuPageEventResultCancel:
            break;
        }

    return EMuiuPageResultCancelled;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingLaunchMultilineEditorL()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CMuiuDynamicSettingsDialog::SettingLaunchMultilineEditorL(
    CMuiuSettingBase& /* aBaseItem */,
    TMuiuDynSetUpdateMode& /* aUpdateMode */ )
    {
    // This should be called from the client side
    __ASSERT_DEBUG( EFalse,
        User::Panic( KMuiuDynSetDialogPanic, KErrUnknown ) );

    return EMuiuPageEventResultCancel;
    }

/******************************************************************************

    Event handling

******************************************************************************/

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::EventArrayChangedL()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::EventArrayChangedL(
    const TMuiuArrayEvents aEvent )
    {
    switch ( aEvent )
        {
        case EMuiuDynArrayAdded:
            iListBox.HandleItemAdditionL();
            break;

        case EMuiuDynArrayRemoved:
            iListBox.HandleItemRemovalL();
            break;

        // When array has changed, send event before updating listbox
        case EMuiuDynArrayStackForward:
        case EMuiuDynArrayStackBackward:
            EventPrepareSubArrayL(
                *GetItem( 0 ),
                aEvent == EMuiuDynArrayStackForward ? ETrue : EFalse );

        //lint -fallthrough
        case EMuiuDynArrayChanged:
            iListBox.HandleItemRemovalL();
            iListBox.HandleItemAdditionL();
            break;
        }

    return KErrNone;
    }


// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::EventSubArrayChangeL()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::EventSubArrayChangeL(
    CMuiuSettingBase& /* aBaseItem */ )
    {
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::EventItemEditStartsL()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CMuiuDynamicSettingsDialog::EventItemEditStartsL(
    CMuiuSettingBase& /* aBaseItem */ )
    {
    return EMuiuPageEventResultApproved;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::EventItemEditEndsL()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::EventItemEditEndsL(
    CMuiuSettingBase& /* aBaseItem */ )
    {
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::EventItemEvaluateRadioButton()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CMuiuDynamicSettingsDialog::EventItemEvaluateRadioButton(
    const CMuiuSettingBase& /* aBaseItem */,
    TInt& /* aNewValue */ )
    {
    // This check fails always
    return EMuiuPageEventResultApproved;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::EventItemEvaluateValue()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CMuiuDynamicSettingsDialog::EventItemEvaluateValue(
    const CMuiuSettingBase& /* aBaseItem */,
    TInt& /* aNewValue */ )
    {
    // This check always true
    return EMuiuPageEventResultApproved;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::EventItemEvaluateText()
// ----------------------------------------------------------------------------
//
TMuiuPageEventResult CMuiuDynamicSettingsDialog::EventItemEvaluateText(
    const CMuiuSettingBase& /* aBaseItem */,
    TDes& /* aNewText */ )
    {
    // This check fails always
    return EMuiuPageEventResultApproved;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::EventCustomMdcaPoint()
// ----------------------------------------------------------------------------
//
TBool CMuiuDynamicSettingsDialog::EventCustomMdcaPoint(
    const TUid& /* aId */,
    TPtr& /* aString */ ) const
    {
    return EFalse;
    }

/******************************************************************************

    Utilities

******************************************************************************/

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingsStartedL()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::SettingsStartedL()
    {
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingPageResource()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::SettingPageResource(
    const TIMSPageResource aSettingPage ) const
    {
    return ( *iSettingPageResources )[aSettingPage];
    }



// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SetSettingPageResource()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SetSettingPageResource(
    const TIMSPageResource aSettingPage,
    const TInt aResourceId )
    {
    TRAP_IGNORE( iSettingPageResources->PushResourceL( aSettingPage, aResourceId ) );
    }


// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::ShowCustomQueryL()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::ShowCustomQueryL(
    const TInt aResourceStringId )
    {
    iQueryDialog = CAknQueryDialog::NewL();
    TInt result = iQueryDialog->ExecuteLD( aResourceStringId );
    iQueryDialog = NULL;
    return result;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::CurrentItemIndex()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::CurrentItemIndex() const
    {
    return iListBox.CurrentItemIndex();
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::CurrentItem()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CMuiuDynamicSettingsDialog::CurrentItem() const
    {
    return GetItem( iListBox.CurrentItemIndex() );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::GetValueEditorFlags()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::GetValueEditorFlags(
    const CMuiuSettingBase& aBaseItem )
    {
    // Get the flags for editor
    TInt flags =
        CAknIntegerSettingPage::EInvalidValueNotOffered;
    flags |= !aBaseItem.iItemFlags.Flag( EMuiuDynItemForceMustFill ) ?
        CAknIntegerSettingPage::EEmptyValueAllowed : 0;
    flags |= aBaseItem.iItemFlags.Flag( EMuiuDynItemNoInitialSelection ) ?
        CAknIntegerSettingPage::ENoInitialSelection : 0;
    flags |= aBaseItem.iItemFlags.Flag( EMuiuDynItemEditorCursorAtBeginning ) ?
        CAknIntegerSettingPage::EPutCursorAtBeginning : 0;

    return flags;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::GetTextEditorFlags()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::GetTextEditorFlags(
    const CMuiuSettingBase& aBaseItem )
    {
    TInt flags =
        CAknTextSettingPage::EZeroLengthNotOffered;
    flags |= !aBaseItem.iItemFlags.Flag( EMuiuDynItemForceMustFill ) ?
        CAknTextSettingPage::EZeroLengthAllowed : 0;
    flags |= aBaseItem.iItemFlags.Flag( EMuiuDynItemNoInitialSelection ) ?
        CAknTextSettingPage::ENoInitialSelection : 0;
    flags |= aBaseItem.iItemFlags.Flag( EMuiuDynItemEditorCursorAtBeginning ) ?
        CAknTextSettingPage::EPutCursorAtBeginning : 0;

    return flags;
    }

/******************************************************************************

    Item initialization

******************************************************************************/

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::InitAnyItem()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::InitAnyItem(
    CMuiuSettingBase& aBaseItem,
    const TInt aValue,
    const TMuiuSettingsText* aText )
    {
    TInt error = KErrNone;

    // Initialize any item
    switch ( aBaseItem.Type() )
        {
        case EMuiuDynSetMenuArray:
            error = InitArrayLink( aBaseItem, aText );
            break;

        case EMuiuDynSetRadioButtonArray:
            error = InitArrayRadioButton( aBaseItem, aValue, aText );
            break;

        case EMuiuDynSetCheckBoxArray:
            error = InitArrayCheckBox( aBaseItem, aValue, aText );
            break;

        case EMuiuDynSetItemEditText:
            error = InitItemTextEdit( aBaseItem, aText );
            break;

        case EMuiuDynSetItemEditValue:
            error = InitItemNumberEdit( aBaseItem, aValue, aText );
            break;

        // Radiobutton and checkbox initialization is done during
        // the array initialization
        default:
            error = KErrNotFound;
            break;
        }

    // Set the initialized flag on for the item
    aBaseItem.iItemFlags.ChangeFlag(
        EMuiuDynItemIsInitialized, error == KErrNone );
    return error;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::InitAnyMultiItem()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::InitAnyMultiItem(
    const CMuiuDynInitItemIdArray* aStartTree,
    const CMuiuDynInitItemIdArray& aIdArray,
    const CMuiuDynInitItemValueArray* aValueArray,
    const CMuiuDynInitItemTextArray* aDescArray,
    const CMuiuDynInitInfoArray* aInitInfoArray )
    {
    // Make sure there are items in array
    if ( !aIdArray.Count() )
        {
        return KErrNotFound;
        }

    // Count the items in the tree if provided.
    TInt count = aStartTree ? aStartTree->Count() : KErrNotFound;
    CMuiuSettingBase* base = NULL;

    // if the starting point has been provided and there is items,
    // try to find the starting point.
    if ( count > 0 )
        {
        // Get the starting point
        base = GetItem( aStartTree->At( 0 ) );

        // Go as deep as there are items provided
        for ( TInt item = 0; ++item < count; )
            {
            base = GetSubItem( *base, aStartTree->At( item ) );
            }
        }

    // Seek the starting point. If the item has not been found during
    // the tree search, try to find the given item by normal means.
    if ( base )
        {
        base = GetSubItem( *base, aIdArray.At( 0 ) );
        }
    else
        {
        base = GetItem( aIdArray.At( 0 ) );
        }

    // The item has been found initialize item
    if ( base )
        {
        // Prepare
        TInt valueIndex = 0;
        TInt textIndex = 0;
        TInt item = 0;
        count = aIdArray.Count();

        do
            {
            // Intialize the item
            PrepareInit(
                *base, aValueArray, aDescArray,
                aInitInfoArray, valueIndex, textIndex, item );

            // Check if the next item is defined and find it
            base = ++item < count ?
                SettingFindEditor( *base, aIdArray.At( item ) ) : NULL;
            }
        while ( base );
        }

    return base ? KErrNone : KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::PrepareInit()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::PrepareInit(
    CMuiuSettingBase& aBaseItem,
    const CMuiuDynInitItemValueArray* aValueArray,
    const CMuiuDynInitItemTextArray* aDescArray,
    const CMuiuDynInitInfoArray* aInitInfoArray,
    TInt& aValueIndex,
    TInt& aTextIndex,
    TInt aIndex )
    {
    // Prepare temporary array
    CMuiuDynInitItemValueArray* valueArray =
        const_cast<CMuiuDynInitItemValueArray*>( aValueArray );
    CMuiuDynInitItemTextArray* descArray =
        const_cast<CMuiuDynInitItemTextArray*>( aDescArray );

    // If info array is defined, check, which one should be used
    if ( aInitInfoArray && aIndex >= 0 && aIndex < aInitInfoArray->Count() )
        {
        TMuiuDynInitInfo info = aInitInfoArray->At( aIndex );

        valueArray = ( info == EMuiuDynInitInfoValue ) ?
            valueArray : NULL;
        descArray = ( info == EMuiuDynInitInfoText ) ?
            descArray : NULL;
        }

    // Initialize item as value
    if ( valueArray && valueArray->Count() )
        {
        InitAnyItem( aBaseItem, valueArray->At( aValueIndex++ ), NULL );
        }
    // Initialize item as text
    else if ( descArray && descArray->Count() )
        {
        InitAnyItem( aBaseItem, NULL, descArray->At( aTextIndex++ ) );
        }
    else
        {
        // Unidentified combination
        __ASSERT_DEBUG( EFalse,
            User::Panic( KMuiuDynSetDialogPanic, KErrUnknown ) );
        }

    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::InitArrayLink()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::InitArrayLink(
    CMuiuSettingBase& aBaseItem,
    const TMuiuSettingsText* aText )
    {
    // Convert item
    CMuiuSettingsLink* link = static_cast<CMuiuSettingsLink*>( &aBaseItem );

    // Get the item count in array
    TInt items = KErrNotFound;
    if ( link->iItemLinkArray )
        {
        items = link->iItemLinkArray->Count();
        }

    // Clear checked flag from all items
    while ( --items >= 0 )
        {
        link->iItemFlags.ClearFlag( EMuiuDynItemChecked );

        // Uncheck the buttons in the array
        CMuiuSettingBase* base = GetSubItem( aBaseItem, items );
        if ( base )
            {
            base->iItemFlags.ClearFlag( EMuiuDynItemChecked );
            base = NULL;
            }
        }

    // Set the text if provided
    if ( aText && link->iItemSettingText )
        {
        link->iItemSettingText->Copy( *aText );
        }

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::InitArrayRadioButton()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::InitArrayRadioButton(
    CMuiuSettingBase& aBaseItem,
    const TInt aValue,
    const TMuiuSettingsText* aText )
    {
    // Convert item
    CMuiuSettingsLinkExtended* radioArray =
        static_cast<CMuiuSettingsLinkExtended*>( &aBaseItem );
    CMuiuSettingBase* base = NULL;

    // Do normal link array initialization
    InitArrayLink( aBaseItem, NULL );

    TInt result = KErrNotFound;

    // If the item has no value, check if any of the radiobuttons is
    // checked, and get the index from there
    if ( aValue == KErrNotFound && radioArray->iItemLinkArray )
        {
        TUid id = TUid::Uid( 0 );
        TInt index;
        FindCheckedRadiobutton( *radioArray, id, index );
        base = GetSubItem( *radioArray, id );

        radioArray->SetValue( index );
        }
    // For listbox item, simply check one of the buttons
    else if ( aValue >= 0 && radioArray->iItemLinkArray &&
         aValue < radioArray->iItemLinkArray->Count() )
        {
        // In initialization phase, the received index value is from
        // the main resource. Get the visible array's item index
        // and setting the item checked and getting the checked item
        TInt value = GetSubItemIndex( *radioArray, aValue, EFalse );
        CheckRadioButton( *radioArray, value );
        base = GetSubItem( *radioArray, value );

        // Set number of radiobox item to indicate the checked item
        radioArray->SetValue( value );
        result = KErrNone;
        }
    else
        {
        result = KErrNotFound;
        }

    // Make sure the setting text exist and copy text if provided
    if ( result == KErrNone && radioArray->iItemSettingText &&
        ( aText || ( base && base->iItemLabel ) ) )
        {
        radioArray->iItemSettingText->Copy(
            aText != NULL ? *aText : *base->iItemLabel );
        }

    base = NULL;
    radioArray = NULL;

    return result;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::InitArrayCheckBox()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::InitArrayCheckBox(
    CMuiuSettingBase& aBaseItem,
    const TInt aValue,
    const TMuiuSettingsText* aText )
    {
    // Convert item
    CMuiuSettingsLinkExtended* link =
        static_cast<CMuiuSettingsLinkExtended*>( &aBaseItem );
    CMuiuSettingBase* base;

    // Do normal link array initialization
    InitArrayLink( aBaseItem, NULL );
    link->SetValue( aValue );

    // aValue can contain value KErrNotFound, which means 32 flags set.
    // This function trusts the user provides correct value in the field
    if ( link->iItemLinkArray )
        {
        // Get the item count in array
        TInt buttons = link->iItemLinkArray->Count();

        // Check the checkbox buttons
        while ( --buttons >= 0 )
            {
            base = link->iItemLinkArray->At( buttons );

            base->iItemFlags.ChangeFlag(
                EMuiuDynItemChecked, ( aValue >> buttons ) & 0x01 );
            }
        }

    // Make sure the setting text exist and copy text if provided
    if ( aText && link->iItemSettingText )
        {
        link->iItemSettingText->Copy( *aText );
        }

    link = NULL;
    base = NULL;

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::InitItemTextEdit()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::InitItemTextEdit(
    CMuiuSettingBase& aBaseItem,
    const TMuiuSettingsText* aText )
    {
    // Make sure the text fields exists and fill the item if possible
    if ( aText && aBaseItem.Text() )
        {
        CMuiuSettingsEditText* textEditor =
            static_cast<CMuiuSettingsEditText*>( &aBaseItem );
        TMuiuSettingsText tempText;
        tempText.Copy( aText->Left( textEditor->iItemMaxLength ) );
        aBaseItem.SetText( &tempText );
        }

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::InitItemNumberEdit()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::InitItemNumberEdit(
    CMuiuSettingBase& aBaseItem,
    const TInt aValue,
    const TMuiuSettingsText* aText )
    {
    // Convert item
    CMuiuSettingsEditValue* numberEditor =
        static_cast<CMuiuSettingsEditValue*>( &aBaseItem );

    // Fill the existing fields
    numberEditor->SetValue( aValue );

    // Make sure all of the fields exists
    if ( numberEditor->iItemSettingText )
        {
         if ( !aText )
            {
            numberEditor->iItemSettingText->Num( aValue );
            }
         else
            {
            numberEditor->iItemSettingText->Copy( *aText );
            }
        }

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::InitItemUnidentified()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::InitItemUnidentified(
    CMuiuSettingBase& /* aBaseItem */,
    const TInt /* aValue */,
    const TMuiuSettingsText* /* aText */ )
    {
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SettingsLastItemIndex()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::SettingsLastItemIndex(
    const TBool aRemove ) const
    {
    // Store the last index to speed up fetching and removing
    TInt lastIndex = iSettingsLastItemIndex->Count() - 1;

    // Fetch the last item in the list and remove it
    TInt lastItem = iSettingsLastItemIndex->At( lastIndex );

    if ( aRemove )
        {
        iSettingsLastItemIndex->Delete( lastIndex );
        }

    // in case the array has been changed, set the new index in the boundaries
    // of the new array
    SetBetweenValues( lastItem, 0, MdcaCount() );

    return lastItem;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SetSettingsLastItemIndexL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SetSettingsLastItemIndexL(
    const TInt aLastItemIndex )
    {
    iSettingsLastItemIndex->AppendL( aLastItemIndex );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SetListboxEmptyTextL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SetListboxEmptyTextL(
    const TDesC& aText )
    {
    iListBox.View()->SetListEmptyTextL( aText );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::CurrentSettingsPage()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CMuiuDynamicSettingsDialog::CurrentSettingsPage(
    const TBool aRemove )
    {
    // Get the last item in the array
    TInt lastIndex = iSettingsPageArray->Count() - 1;

    // Get item
    CMuiuSettingBase* currentItem = NULL;

    if ( lastIndex >= 0 )
        {
        currentItem = iSettingsPageArray->At( lastIndex );
        }

    // Remove the item when required
    if ( aRemove )
        {
        iSettingsPageArray->Delete( lastIndex );
        }

    return currentItem;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SetCurrentSettingsPage()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SetCurrentSettingsPage(
    CMuiuSettingBase& aBaseItem )
    {
    if ( iSettingsPageArray->Count() < KMuiuMaxEditors )
        {
        // Set the new item as a last item
        TRAP_IGNORE( iSettingsPageArray->AppendL( &aBaseItem ) );

        // Insert the backup item to array
        TRAP_IGNORE( AddBackupItemL( aBaseItem ) );
        }
    else
        {
        // Too many editors, something is wrong
        __ASSERT_DEBUG( EFalse, User::Panic(
            KMuiuDynSetDialogPanic, KErrOverflow ) );
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::CheckRadioButton()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::CheckRadioButton(
    CMuiuSettingsLink& aRadioButtonArray,
    const TInt aRadioButtonIndex )
    {
    // Make sure the array exists
    if ( aRadioButtonArray.iItemLinkArray )
        {
        CMuiuDynSetItemArray& array = *aRadioButtonArray.iItemLinkArray;

        // The loop must return the index of the item from the visible list
        // All items must be searched through
        // Each item has to be checked from their hidden properties
        const TInt buttons = array.Count();
        TInt index = buttons;

        // First, the item flags needs to be cleared
        while ( --index >= 0 )
            {
            array[index]->iItemFlags.ClearFlag( EMuiuDynItemChecked );
            }

        // Get the item from the actual list
        CMuiuSettingBase* subItem =
            GetSubItem( aRadioButtonArray, aRadioButtonIndex );

        if ( subItem )
            {
            subItem->iItemFlags.SetFlag( EMuiuDynItemChecked );
            }
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::CheckRadioButton()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::CheckRadioButton(
    CMuiuSettingsLink& aRadioButtonArray,
    const TUid& aId )
    {
    // Make sure the array exists
    if ( aRadioButtonArray.iItemLinkArray )
        {
        CMuiuSettingBase* radioButton = NULL;

        TInt buttons = aRadioButtonArray.iItemLinkArray->Count();

        // Check the selected radiobutton
        while ( --buttons >= 0 )
            {
            radioButton = ( *aRadioButtonArray.iItemLinkArray )[buttons];
            radioButton->iItemFlags.ChangeFlag(
                EMuiuDynItemChecked, radioButton->iItemId == aId );
            }
        }

    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::FindCheckedRadiobutton()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::FindCheckedRadiobutton(
    const CMuiuSettingsLinkExtended& aArray,
    TUid& aId,
    TInt& aIndex )
    {
    CMuiuSettingBase* radioButton = NULL;
    aId.iUid = KErrNotFound;
    aIndex = KErrNotFound;

    if ( aArray.iItemLinkArray )
        {
        // Find the checked radio button, as there can be only one!
        for ( TInt count = aArray.iItemLinkArray->Count(); --count >= 0; )
            {
            radioButton = ( *aArray.iItemLinkArray )[count];

            // Search for the radiobuttons but ignore hidden ones,
            // even if it is checked
            if ( radioButton && !IsHidden( *radioButton ) )
                {
                ++aIndex;

                // If checked, return the value
                if ( radioButton->iItemFlags.Flag( EMuiuDynItemChecked ) )
                    {
                    aId = radioButton->iItemId;
                    return KErrNone;
                    }
                }
            }
        }

    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::AddBackupItemL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::AddBackupItemL(
    const CMuiuSettingBase& aBaseItem )
    {
    switch ( aBaseItem.iItemType )
        {
        case EMuiuDynSetItemMultiLine:
        case EMuiuDynSetRadioButtonArray:
        case EMuiuDynSetCheckBoxArray:
            BackupItemCreateRadioL( aBaseItem );
            break;

        case EMuiuDynSetItemEditScValue:
        case EMuiuDynSetItemEditText:
            BackupItemCreateTextL( aBaseItem );
            break;

        case EMuiuDynSetItemEditValue:
            BackupItemCreateValueL( aBaseItem );
            break;

        case EMuiuDynSetUndefined:
            BackupItemCreateUndefinedL( aBaseItem );
            break;

        default:
            // No need to backup, or unsupported
            break;
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::RemoveLastBackupItem()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::RemoveLastBackupItem()
    {
    CMuiuSettingBase* base = GetLastBackupItem();
    iBackupArray->Delete( iBackupArray->Count() - 1 );

    if ( base )
        {
        // With link items do not delete array, since the pointer points
        // directly to original array. So set the pointer to null before
        // deleting the item.
        if ( base->HasLinkArray() )
            {
            static_cast<CMuiuSettingsLink*>( &*base )->iItemLinkArray = NULL;
            }

        delete base;
        base = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::GetLastBackupItem()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CMuiuDynamicSettingsDialog::GetLastBackupItem()
    {
    return iBackupArray->At( iBackupArray->Count() - 1 );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::RestoreFromBackup()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::RestoreFromBackup()
    {
    // Restore
    for ( TInt count = iSettingsPageArray->Count(); --count >= 0; )
        {
        TInt lastPageNum = iSettingsPageArray->Count() - 1;
        TInt lastBackupNum = iBackupArray->Count() - 1;
        CMuiuSettingBase* currentPage = iSettingsPageArray->At( lastPageNum );
        CMuiuSettingBase* backupPage = iBackupArray->At( lastBackupNum );

        // Restore values
        const TMuiuSettingsText* text = ItemText( *backupPage );
        if ( text )
            {
            SetItemText( *currentPage, *text );
            }

        currentPage->SetValue( backupPage->Value() );
        currentPage->SetText( backupPage->Text() );

        if ( currentPage->iItemType == EMuiuDynSetRadioButtonArray )
        	{
        	// Convert item and check radiobutton from backup array
	        CMuiuSettingsLinkExtended* radioArray =
    	    	static_cast<CMuiuSettingsLinkExtended*>( currentPage );
        	CheckRadioButton( *radioArray, backupPage->Value() );
        	}

        iSettingsPageArray->Delete( lastPageNum );
        RemoveLastBackupItem();
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::CleanBackupArray()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::CleanBackupArray()
    {
    for ( TInt count = iBackupArray->Count(); --count >= 0; )
        {
        RemoveLastBackupItem();
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::BackupItemCreateLinkL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::BackupItemCreateLinkL(
    const CMuiuSettingBase& aBaseItem )
    {
    CMuiuSettingsLink* linkBackup = CMuiuSettingsLink::NewL();
    const CMuiuSettingsLink* link =
        static_cast<const CMuiuSettingsLink*>( &aBaseItem );
    *linkBackup = *link;

    iBackupArray->AppendL( linkBackup );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::BackupItemCreateRadioL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::BackupItemCreateRadioL(
    const CMuiuSettingBase& aBaseItem )
    {
    CMuiuSettingsLinkExtended* radioBackup = CMuiuSettingsLinkExtended::NewL();
    const CMuiuSettingsLinkExtended* radio =
        static_cast<const CMuiuSettingsLinkExtended*>( &aBaseItem );
    *radioBackup = *radio;

    iBackupArray->AppendL( radioBackup );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::BackupItemCreateValueL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::BackupItemCreateValueL(
    const CMuiuSettingBase& aBaseItem )
    {
    CMuiuSettingsEditValue* valueBackup = CMuiuSettingsEditValue::NewL();
    const CMuiuSettingsEditValue* value =
        static_cast<const CMuiuSettingsEditValue*>( &aBaseItem );
    *valueBackup = *value;

    iBackupArray->AppendL( valueBackup );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::BackupItemCreateTextL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::BackupItemCreateTextL(
    const CMuiuSettingBase& aBaseItem )
    {
    CMuiuSettingsEditText* textBackup = CMuiuSettingsEditText::NewL();
    const CMuiuSettingsEditText* text =
        static_cast<const CMuiuSettingsEditText*>( &aBaseItem );
    *textBackup = *text;

    iBackupArray->AppendL( textBackup );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::BackupItemCreateUndefinedL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::BackupItemCreateUndefinedL(
    const CMuiuSettingBase& aBaseItem )
    {
    CMuiuSettingBase* base = CreateItemForBackupL( aBaseItem );

    iBackupArray->AppendL( base );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::CreateItemForBackupL()
// ----------------------------------------------------------------------------
//
CMuiuSettingBase* CMuiuDynamicSettingsDialog::CreateItemForBackupL(
    const CMuiuSettingBase& aBaseItem )
    {
    CMuiuSettingsLinkExtended* radioBackup = CMuiuSettingsLinkExtended::NewL();
    const CMuiuSettingsLinkExtended* radio =
        static_cast<const CMuiuSettingsLinkExtended*>( &aBaseItem );
    *radioBackup = *radio;

    return radioBackup;
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::UpdateMskL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::UpdateMskL()
    {
    if ( iSoftkeys->MSKLabelling() )
        {
        SetMskL();
        }
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::SetMskL()
// ----------------------------------------------------------------------------
//
void CMuiuDynamicSettingsDialog::SetMskL()
    {
    CMuiuSettingBase* item = CurrentItem();
    __ASSERT_DEBUG( item != NULL, User::Panic(
        KMuiuDynSetDialogPanic, KErrNotFound ) );

    TInt resource = R_IMAS_MSK_OPEN;
    if ( item && !ItemSubArrayCheck( *item ) )
        {
        resource = R_IMAS_MSK_CHANGE;
        }
    iSoftkeys->UpdateMiddleSoftkeyLabelL( resource, MskCommand(), this );
    }

// ----------------------------------------------------------------------------
// CMuiuDynamicSettingsDialog::MskCommand()
// ----------------------------------------------------------------------------
//
TInt CMuiuDynamicSettingsDialog::MskCommand()
    {
    // This function is intended for derived classes to get current msk
    // command value. Though it's a constant now. It could vary in the future.
    return EAknSoftkeyOk;
    }

//  End of File
