/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides Uni Sending Settings Dialog methods.
*
*/



// INCLUDE FILES
#include <barsread.h>
//#include <mtclreg.h>

#include <eiktxlbx.h>
#include <aknnavi.h>
#include <akntitle.h>
#include <aknsettingpage.h>
#include <aknradiobuttonsettingpage.h>
#include <aknnotewrappers.h> 

#include <StringLoader.h>

#include <centralrepository.h>    // link against centralrepository.lib
#include <messaginginternalcrkeys.h> // for Central Repository keys

#include <mmsconst.h>

// Features
#include <featmgr.h>    
#include <bldvariant.hrh>
#include <messagingvariant.hrh>
#include <csxhelp/unif.hlp.hrh>
#include <hlplch.h>   // For HlpLauncher

#include <UniEditor.rsg>
#include "UniEditorApp.hrh"
#include "UniSendingSettingsDialog.h"

// CONSTANTS
const TUid  KUniEditorAppId = { 0x102072D8 };

// ---------------------------------------------------------
// CUniSendingSettingsDialog
// C++ constructor
// ---------------------------------------------------------
//
CUniSendingSettingsDialog::CUniSendingSettingsDialog( TInt aSettingsFlags,
                                                      TUniSendingSettings& aSendingSettings,
                                                      TUniExitCode& aExitCode,
                                                      CAknTitlePane& aTitlePane ) :
    iTitlePane( aTitlePane ),
    iExitCode( aExitCode ),
    iSendingSettings( aSendingSettings ),
    iSettingsFlags( aSettingsFlags )
    {
    iExitCode = EUniExternalInterrupt; // reference -> cannot be initialized above
    }

// ---------------------------------------------------------
// ~CUniSendingSettingsDialog()
// Destructor
// ---------------------------------------------------------
//
CUniSendingSettingsDialog::~CUniSendingSettingsDialog()
    {
    delete iPreviousTitleText;

    // remove default navi pane
    if ( iNaviPane && 
         iSettingsDialogFlags & EDefaultNaviPane )
        {
        // if "EDefaultNaviPane" flag is set
        // iNaviPane != 0
        iNaviPane->Pop();
        }
    
    if ( iItems->MdcaCount() == 1 )
        {
		/*
         * If MpMessagingHideMessageTypeId is set, then we must delete
         * iMuiuSettingsItemArray corresponding to message type array.
         * Message type array gets deleted from this dialog during
         * PreLayoutDynInitL, therefore array contained by 
         * message type array requires a clean-up here
         */
        if ( iSettingsFlags & EUniSettingsHideMessageTypeOption )
            {
            delete iMessageTypeSettings.iMuiuSettingsItemArray;
            }
        else
            {
            delete iCharacterSettings.iMuiuSettingsItemArray;
            }
        }
    
    // For lint
    iNaviPane = NULL;
    iItems = NULL;
    iListbox = NULL;
    }

// ---------------------------------------------------------
// CUniSendingSettingsDialog::ProcessCommandL
// Processes menu item commands
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::ProcessCommandL( TInt aCommandId ) 
    {
    CAknDialog::ProcessCommandL( aCommandId );

    switch ( aCommandId )
        {
        case EChangeItem:
            {
            HandleEditRequestL( iListbox->CurrentItemIndex(), EFalse );
            break;
            }
        case EAknCmdHelp:
            {
            LaunchHelpL();
            break;
            }
        case EExitItem:
            {
            ExitDialogL();
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CUniSendingSettingsDialog::ExitDialogL
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::ExitDialogL()
    {
    // signal Editor of Exit
    iSendingSettings.iDialogExited = ETrue;

    iExitCode = EUniExit;
    TryExitL( EAknSoftkeyExit );
    }
        
// ---------------------------------------------------------
// CUniSendingSettingsDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CUniSendingSettingsDialog::OkToExitL( TInt aKeyCode )
    {
    switch ( aKeyCode )
        {
        case EUniCmdSettingsChange:
            { 
            // As all the softkey buttons come only to this function, MSK needs to handled here
            HandleEditRequestL( iListbox->CurrentItemIndex(), ETrue );
            return EFalse;
            }
        case EAknSoftkeyCancel:
        case EAknSoftkeyBack:
            {
            // If exit wanted during back operation
            iSettingsDialogFlags &= ~EExitSelected;
            ClosingDialogL();
            
            if( iSettingsDialogFlags & ESwitchToExit )
                {
                iExitCode = EUniExit;
                }
            else//normal back
                {
                iExitCode = EUniBack;
                }
                    
            return ETrue;
            }
        default :
            {
            return CAknDialog::OkToExitL( aKeyCode );
            }
        }
    }

// ---------------------------------------------------------
// CUniSendingSettingsDialog::PreLayoutDynInitL
// Creates a listbox for the settings.
// Values are read from the resource file.
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::PreLayoutDynInitL()
    {
    // Get supported features from feature manager.
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        iSettingsDialogFlags |= EHelpFeatureSupported;
        }
    FeatureManager::UnInitializeLib();

    SetTitleTextL();

    // Get listbox pointer
    iListbox = static_cast<CEikTextListBox*>( Control( EDlgSettingListBox ) );
    iListbox->SetListBoxObserver( this );
    
    // Sets correct size for the listbox    
    iListbox->SetRect( iAvkonAppUi->ClientRect() );
    
    // Read&store the current values to listbox items.
    TInt itemsResId = R_UNI_SENDINGSETTINGS_ITEMS;
    
    iItems = CMuiuSettingsArray::NewL( itemsResId );
    
    iCharacterSettings = iItems->At( EUniSendingSettingsCharSupport );
    /*
     * If MpMessagingHideMessageTypeId is set, we need to
     * delete the settings from message type array.
     * We will need to manually clean up the items contained
     * by message type array in the destructor since we are deleting
     * message type array here */
    iMessageTypeSettings = iItems->At( EUniSendingSettingsMessageType );
    if ( iSettingsFlags & EUniSettingsHideMessageTypeOption )
        {
        iItems->Delete( EUniSendingSettingsMessageType );
        }
    
    if( ( iSettingsFlags & EUniSettingsContentNeedsMms &&
          !( iSettingsFlags & EUniSettingsPermanentSms ) ) ||
        iSettingsFlags & EUniSettingsPermanentMms || 
        iSettingsFlags & EUniSettingsHideCharSet ||
        iSendingSettings.iMessageType == TUniSendingSettings::EUniMessageTypeMultimedia )
        {
        // CharSupport is not shown when we are dealing with MMS messages
        // or when the charSet is locked
        iItems->Delete( EUniSendingSettingsCharSupport );
        }
    
    GetSendingSettingsValues();

    // scroll bars
    iListbox->CreateScrollBarFrameL( ETrue );
    iListbox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, 
                                                         CEikScrollBarFrame::EAuto );

    // replace navi pane with empty one.
    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( iEikonEnv->AppUiFactory()->StatusPane()->ControlL( TUid::Uid(EEikStatusPaneUidNavi) ) );
        
    iNaviPane->PushDefaultL();
    iSettingsDialogFlags |= EDefaultNaviPane;

    CTextListBoxModel* model = iListbox->Model();
    model->SetItemTextArray( iItems );
    model->SetOwnershipType( ELbmOwnsItemArray );
    UpdateMskL();
    }

// ---------------------------------------------------------
// CUniSendingSettingsDialog::ClosingDialogL
// ---------------------------------------------------------
//
TBool CUniSendingSettingsDialog::ClosingDialogL()
    {
    // Replace title with old title text
    if ( !( iSettingsDialogFlags & EGoingToExit ) )
        {
        iTitlePane.SetTextL( *iPreviousTitleText );
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CUniSendingSettingsDialog::GetSendingSettingsValues
// Gets the values for the uni message settings and puts them in
// to the listbox. Text strings are from loc files.
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::GetSendingSettingsValues()
    {
    TInt index = 0;
    /*
     * Set up the Message Type item only if the 
     * MpMessagingHideMessageTypeId feature flag is not set
     * Note: This is the default/existing case */
    if ( ! ( iSettingsFlags & EUniSettingsHideMessageTypeOption ) )
        {
        TInt currentItem = iSendingSettings.iMessageType;
        
        SetArrayItem( index, currentItem );
        }
    if( !( ( iSettingsFlags &  EUniSettingsContentNeedsMms &&
             !( iSettingsFlags & EUniSettingsPermanentSms ) ) || 
            ( iSettingsFlags & EUniSettingsPermanentMms ) ||
            ( iSettingsFlags & EUniSettingsHideCharSet ) ||
              iSendingSettings.iMessageType == TUniSendingSettings::EUniMessageTypeMultimedia ) )
        { 
        // Show CharSupport when not in MMS mode
        /*
         * Change the index only if we are also displaying
         * Message Type.
         */
        if ( ! ( iSettingsFlags & EUniSettingsHideMessageTypeOption ) )
            {
            index = EUniSendingSettingsCharSupport;
            }

        switch ( iSendingSettings.iCharSupport )
            {
            case TUniSendingSettings::EUniCharSupportFull:
                {
                SetArrayItem( index, EUniSettingsCharSupportFull );
                break;
                }
            case TUniSendingSettings::EUniCharSupportReduced:
            default:
                {
                SetArrayItem( index, EUniSettingsCharSupportReduced );
                break;
                }
            }
        }
    }

// ---------------------------------------------------------
// CUniSendingSettingsDialog::SetArrayItem
// Updates indexed item of listbox
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::SetArrayItem( TInt aIndex, TInt aCurrentNumber )
    {
    /*
     * Instead of using hardcoded number to do bounds checking
     * use the number of items in the array
     */
    if( aIndex < 0 || aIndex >= iItems->MdcaCount() )
        { // Item is invisible so just return
        return;
        }
        
    iItems->At( aIndex ).iUserText.Copy(
        ( *iItems->At( aIndex ).iMuiuSettingsItemArray )[aCurrentNumber]);
    ( *iItems )[aIndex].iCurrentNumber = aCurrentNumber;
    }

// ---------------------------------------------------------
// CUniSendingSettingsDialog::SetArrayItem
// Updates indexed item of listbox - user text formatted by the caller
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::SetArrayItem( TInt aIndex, 
                                              TInt aCurrentNumber, 
                                              const TDesC& aUserText)
    {
    if( aIndex < 0 )
        {
        return;
        }
        
    iItems->At( aIndex ).iUserText.Copy( aUserText );
    ( *iItems )[aIndex].iCurrentNumber = aCurrentNumber;
    }

// ---------------------------------------------------------
// CUniSendingSettingsDialog::HandleListBoxEventL
// Handles listbox event
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::HandleListBoxEventL( CEikListBox* /* aListBox */, 
                                                     TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            {
            HandleEditRequestL( iListbox->CurrentItemIndex(), ETrue );
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CUniSendingSettingsDialog::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::DynInitMenuPaneL( TInt aMenuId, 
                                                  CEikMenuPane* aMenuPane )
    {
    switch ( aMenuId )
        {
        case R_UNI_SENDINGSETTINGS_DIALOG_MENUPANE:
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, !( iSettingsDialogFlags & EHelpFeatureSupported ) );
            break;
            }
        default:
            {
            break;
            }
        };
    }

// ---------------------------------------------------------
// CUniSendingSettingsDialog::OfferKeyEventL
// Handles listbox event
// ---------------------------------------------------------
//
TKeyResponse CUniSendingSettingsDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                        TEventCode aType )
    {
    TKeyResponse ret = CAknDialog::OfferKeyEventL( aKeyEvent, aType );

    if( aType == EEventKeyUp )
        {
        UpdateMskL();
        }

    if ( iSettingsDialogFlags & EShouldExit ) 
        {
        iExitCode = EUniExit;
        TryExitL( EAknSoftkeyCancel );
        }

    return ret;
    }

// ---------------------------------------------------------
// CUniSendingSettingsDialog::HandleEditRequestL
// Handles editing of a selected setting
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::HandleEditRequestL( TInt aIndex, TBool aEnterPressed )
    {
    /*
     * Instead of switching on hard coded index values and
     * calling methods based on index values, we now call
     * the function based on the item itself.
     * Search the settings array to find the index of 
     * Message Type and Character Support, and call
     * the appropriate method.
     */
    TKeyArrayFix actNumKey( 0, ECmpTInt );
    TInt characterIndex = -1;
    TInt messagetypeIndex = -1;
    
    iItems->Find( iCharacterSettings, actNumKey, characterIndex );
    iItems->Find( iMessageTypeSettings, actNumKey, messagetypeIndex );
    
    if( messagetypeIndex == aIndex )
        {
        EditMessageTypeL();
        }
    else if( characterIndex == aIndex )
        {
        EditCharacterSupportL( aEnterPressed );
        }
        
    iListbox->DrawItem( aIndex );
    }

// ---------------------------------------------------------
// CUniSendingSettingsDialog::EditMessageTypeL
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::EditMessageTypeL()
    {
    TResourceReader reader;
    iEikonEnv->CreateResourceReaderLC( reader, R_UNI_SETTING_MESSAGE_TYPE_LIST );
    
    CDesCArrayFlat* items = new ( ELeave ) CDesCArrayFlat( KSettingsGranularity );
    CleanupStack::PushL( items );
    
    CArrayFixFlat<TInt>* itemIds = new ( ELeave ) CArrayFixFlat<TInt>( KSettingsGranularity );
    CleanupStack::PushL( itemIds );
    
    // Get the labels from resources
    const TInt count = reader.ReadInt16();
    
    for ( TInt loop = 0; loop < count; loop++ )
        {
        HBufC* label = reader.ReadHBufCL();
        
        TBool addLabel( ETrue );
        switch( loop )
            {
            case EUniSettingsMessageTypeAutomatic:
                {
                if ( iSettingsFlags & EUniSettingsPermanentSms ||
                     iSettingsFlags & EUniSettingsPermanentMms )
                    {
                    // Delete automatic if it's permanent SMS or MMS
                    addLabel = EFalse;
                    }
                break;
                }
            case EUniSettingsMessageTypeText:
                {
                if ( ( iSettingsFlags & EUniSettingsContentNeedsMms &&
                       !( iSettingsFlags & EUniSettingsPermanentSms ) ) ||
                     iSettingsFlags & EUniSettingsPermanentMms )
                    {
                    // Delete text if it's permanent MMS or content is MMS and
                    // we are not on permanently locked SMS mode.
                    addLabel = EFalse;
                    }
                break;
                }
            case EUniSettingsMessageTypeMultimedia:
                {
                if ( iSettingsFlags & EUniSettingsPermanentSms )
                    {
                    // Delete multimedia if it's permanent SMS
                    addLabel = EFalse;
                    }
                break;
                }
            case EUniSettingsMessageTypeFax:
            case EUniSettingsMessageTypePaging:
                {
                if ( ( iSettingsFlags & EUniSettingsContentNeedsMms &&
                       !( iSettingsFlags & EUniSettingsPermanentSms ) ) ||
                     iSettingsFlags & EUniSettingsPermanentMms ||
                     iSettingsFlags & EUniSettingsCSPBitsOn )
                    {
                    // Delete fax/paging if it's permanent MMS or content is MMS
                    // or CSP bits deny conversion
                    addLabel = EFalse;
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        
        if ( addLabel )
            {
            TPtr pLabel = label->Des();
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( pLabel );
            
            CleanupStack::PushL( label );
            items->AppendL( pLabel );
            CleanupStack::PopAndDestroy( label );           
            
            itemIds->AppendL( loop );
            }
        else
            {
            delete label;
            }
        }
    
    // Map the stored message type to current item index.
    TInt currentItem = 0;
    TBool matchFound( EFalse );
    const TBool hideMessageType = iSettingsFlags & EUniSettingsHideMessageTypeOption;
    while( !matchFound )
        {
        if ( currentItem == itemIds->Count() )
            {
            currentItem = 0;
            matchFound = ETrue;
            }
        else if ( hideMessageType && itemIds->At( currentItem ) == iSendingSettings.iCharSupport )
            {
            matchFound = ETrue;
            }
        else if ( !hideMessageType && itemIds->At( currentItem ) == iSendingSettings.iMessageType )
            {
            matchFound = ETrue;
            }
        else
            {
            currentItem++;
            }
        }

    if ( ExecuteSettingsPageDialogL( items, 
                                     currentItem, 
                                     iItems->At( EUniSendingSettingsMessageType ).iLabelText ) )
        {
        switch ( itemIds->At( currentItem ) )
            {
            case EUniSettingsMessageTypeAutomatic:
                {
                SetMessageTypeL( TUniSendingSettings::EUniMessageTypeAutomatic );
                break;
                }
            case EUniSettingsMessageTypeText:
                {
                SetMessageTypeL( TUniSendingSettings::EUniMessageTypeText );
                break;
                }
            case EUniSettingsMessageTypeMultimedia:
                {
                SetMessageTypeL( TUniSendingSettings::EUniMessageTypeMultimedia );
                break;
                }
            case EUniSettingsMessageTypeFax:
                {
                SetMessageTypeL( TUniSendingSettings::EUniMessageTypeFax );
                break;
                }
            case EUniSettingsMessageTypePaging:
                {
                SetMessageTypeL( TUniSendingSettings::EUniMessageTypePaging );
                break;
                }
            default:
                {
                break;
                }
            }
        
        SetArrayItem( 0, itemIds->At( currentItem ) );
        CEikFormattedCellListBox* settingListBox =
            static_cast<CEikFormattedCellListBox*>( Control( EDlgSettingListBox ) );
                    
        if ( iSendingSettings.iMessageType == TUniSendingSettings::EUniMessageTypeMultimedia ||
             ( iSendingSettings.iMessageType == TUniSendingSettings::EUniMessageTypeAutomatic && 
               iSettingsFlags & EUniSettingsContentNeedsMms ) )
            {
            if ( iItems->MdcaCount() == 2 )
                {
                iItems->Delete( EUniSendingSettingsCharSupport );
                settingListBox->HandleItemRemovalL();
                }
            }
        else
            {
            if ( iItems->MdcaCount() == 1 &&
                 !( iSettingsFlags & EUniSettingsHideCharSet ) )
                {
                iItems->InsertL( EUniSendingSettingsCharSupport, iCharacterSettings );
                settingListBox->HandleItemAdditionL();
                
                SetArrayItem( EUniSendingSettingsCharSupport, iSendingSettings.iCharSupport );
                }
            }
            
        settingListBox->DrawNow();
        }
    
    CleanupStack::PopAndDestroy( 3 ); // itemIds, items & reader 
    }
    
// ---------------------------------------------------------
// CUniSendingSettingsDialog::SetMessageTypeL
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::SetMessageTypeL( TInt aMessageType )
    {
    iSendingSettings.iMessageType = (TUniSendingSettings::TUniMessageType)aMessageType;
    }
    
// ---------------------------------------------------------
// CUniSendingSettingsDialog::EditCharacterSupportL
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::EditCharacterSupportL( TBool aEnterPressed )
    {
    TInt currentItem = iSendingSettings.iCharSupport;
    TBool isOk = EFalse;

    if ( aEnterPressed )
        {
        currentItem ^= 1;
        }
    else
        {
        TResourceReader reader;
        iEikonEnv->CreateResourceReaderLC( reader, R_UNI_SETTING_CHARACTER_SUPPORT_LIST );
        
        CDesCArrayFlat* items = new ( ELeave ) CDesCArrayFlat( KSettingsGranularity );
        CleanupStack::PushL( items );
        
        // Get the labels from resources
        const TInt count = reader.ReadInt16();

        for ( TInt loop = 0; loop < count; loop++ )
            {
            HBufC* label = reader.ReadHBufCL();
            TPtr pLabel = label->Des();
            
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( pLabel );
            
            CleanupStack::PushL( label );
            items->AppendL( pLabel );
            CleanupStack::PopAndDestroy( label );           
            }
        
        /*
         * Instead of using hard coded index value, use the current
         * index value
         */ 
        isOk = ExecuteSettingsPageDialogL( items, 
                                           currentItem, 
                                           iItems->At( iListbox->CurrentItemIndex() ).iLabelText );
        
        CleanupStack::PopAndDestroy( 2 ); //items & reader 
        }

    if( isOk || aEnterPressed )
        {
        switch ( currentItem )
            {
            case EUniSettingsCharSupportFull:
                {
                SetCharacterSupportL( TUniSendingSettings::EUniCharSupportFull );
                break;
                }
            case EUniSettingsCharSupportReduced:
            default:
                {
                SetCharacterSupportL( TUniSendingSettings::EUniCharSupportReduced );
                break;
                }
            }
        /*
         * Instead of using hardcoded value for index, use
         * the last index of the array
         */
        SetArrayItem( iItems->MdcaCount()-1, currentItem );
        CEikFormattedCellListBox* settingListBox =
            static_cast<CEikFormattedCellListBox*>( Control( EDlgSettingListBox ) );
        settingListBox->DrawNow();
        }
    }
    
// ---------------------------------------------------------
// CUniSendingSettingsDialog::SetCharacterSupportL
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::SetCharacterSupportL( TInt aCharSetSupport )
    {
    iSendingSettings.iCharSupport = (TUniSendingSettings::TUniCharSupport)aCharSetSupport;    
    }
    
// ---------------------------------------------------------
// CUniSendingSettingsDialog::SetTitleTextL
// Makes the status pane
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::SetTitleTextL()
    {   
    //save old title
    iPreviousTitleText = iTitlePane.Text()->Alloc();
        
    // set new title
    HBufC* text = StringLoader::LoadLC( R_UNI_SETTINGS_TITLE_TEXT );

    iTitlePane.SetTextL( *text );
    CleanupStack::PopAndDestroy(); // text
    }

// ---------------------------------------------------------
// CUniSendingSettingsDialog::LaunchHelpL
// launch help using context
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::LaunchHelpL()
    {
    if ( iSettingsDialogFlags & EHelpFeatureSupported )
        {
        CCoeAppUi* editorAppUi = static_cast<CCoeAppUi*>( ControlEnv()->AppUi() );
      
        CArrayFix<TCoeHelpContext>* helpContext = editorAppUi->AppHelpContextL();   
        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
        }
    }

// ---------------------------------------------------------
// CUniSendingSettingsDialog::GetHelpContext
// returns helpcontext as aContext
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    if ( iSettingsDialogFlags & EHelpFeatureSupported )
        {
        aContext.iMajor = KUniEditorAppId; 
        aContext.iContext = KUNIFIED_HLP_SENDING_OPTIONS;
        }
    }

// ---------------------------------------------------------
// CUniSendingSettingsDialog::ShowInformationNoteL
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::ShowInformationNoteL( TInt aResourceId )
    {
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
    HBufC* string = StringLoader::LoadLC( aResourceId );
    note->ExecuteLD( *string );
    CleanupStack::PopAndDestroy(); //string
    }

// ---------------------------------------------------------
// CUniSendingSettingsDialog::UpdateMskL
// ---------------------------------------------------------
//
void CUniSendingSettingsDialog::UpdateMskL()
    {
    // Leaving this function here in case there might be different MSKs in future
    TInt resId = R_UNI_MSK_SETTINGS_BUTTON_CHANGE;    
    CEikButtonGroupContainer& cba = ButtonGroupContainer();
    const TInt KMskPosition = 3;
    cba.SetCommandL( KMskPosition, resId );
    cba.DrawDeferred();
    }

// ---------------------------------------------------------
// CUniSendingSettingsDialog::ExecuteSettingsPageDialogL
// ---------------------------------------------------------
//
TBool CUniSendingSettingsDialog::ExecuteSettingsPageDialogL( CDesCArrayFlat* aItems, 
                                                             TInt& aCurrentItem,
                                                             const TDesC& aLabelText )
    {
    CAknRadioButtonSettingPage* dlg = new ( ELeave ) CAknRadioButtonSettingPage( R_UNI_SETTING_PAGE, 
                                                                                 aCurrentItem, 
                                                                                 aItems );
    CleanupStack::PushL( dlg );
    
    dlg->SetSettingTextL( aLabelText );
    
    CleanupStack::Pop( dlg );
    
    return dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged );
    }

//  End of File
