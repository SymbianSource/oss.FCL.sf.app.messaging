/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     A dialog for adding new and editing old service centres.
*
*/



// INCLUDE FILES
#include <SMUM.rsg>                     // resource identifiers
#include <smutset.h>                    // CSmsSettings
#include <aknappui.h>                   // CEikStatusPane
#include <akntitle.h>                   // CAknTitlePane
#include <StringLoader.h>               // StringLoader
#include <commonphoneparser.h>          // CommonPhoneParser
#include <aknnotewrappers.h>
#include <eikmenup.h>                   // CEikMenuPane
#include <hlplch.h>                     // For HlpLauncher
#include <csxhelp/mce.hlp.hrh>
// locals
#include "SmumSettingsDialogGSM.h"         // KStringLength
#include "SmumEmailServiceCentreItemDialog.h"// CSmumEmailServiceCentreDialog 
#include "SMSU.HRH"                     // resource header
#include "SmumServiceCentresDialog.h"   // CSmsServiceCentresDialog
#include "smsui.pan"

// CONSTANTS
const TInt KSCNameLength = 100;
// ================= MEMBER FUNCTIONS =======================
// C++ constructor can NOT contain any code, that
// might leave.
CSmumEmailServiceCentreDialog::CSmumEmailServiceCentreDialog (
    TDes&           aGateway, 
    TDes&           aSC,
    const TBool&    aModifiable,
    TInt&           aExitCode,
    TBool           aHelpFeatureSupported ) 
        :iGateway               ( aGateway ), 
        iSC                     ( aSC ),
        iModifiable             ( aModifiable ),
        iExitCode               ( aExitCode ),
        iHelpFeatureSupported   ( aHelpFeatureSupported ),
        iLaunchDialog           ( EFalse )
    {
    }

// Destructor
CSmumEmailServiceCentreDialog::~CSmumEmailServiceCentreDialog ()
    {
    if ( iListBox )
        {
        iListBox->SetScrollBarFrame( NULL, CEikListBox::EOwnedExternally );
        }
    delete iPreviousTitleText;
    // iSettingsArray, iListBox, iTitlePane are
    // plain pointers to objects owned by other classes which take
    // care also about deletion.
    // Set to NULL to satisfy Lint
    iSettingsArray = NULL;
    iListBox = NULL;
    iTitlePane = NULL;
    }

// ----------------------------------------------------
// CSmumEmailServiceCentreDialog::OkToExitL
//
// ----------------------------------------------------
TInt CSmumEmailServiceCentreDialog::OkToExitL( TInt aButtonId )
    {
    TInt returnValue;

    switch( aButtonId )
        {
        case ESmsSettingsChoiceMenuChange:
	        {
	        ProcessCommandL( aButtonId );
	        returnValue = EFalse; // Cannot exit, since MSK was pressed
	        }     
	        break;
        // menu / system exit
        case EAknSoftkeyCancel: 
            {
            if ( iExitCode != ESmumSmsSettingsMenuExit &&
                iExitCode != ESmumSmsSettingsSystemExit )
                {
                // system exit
                iExitCode = ESmumSmsSettingsSystemExit;
                }
            // Special handling for menu exit
            if ( iExitCode == ESmumSmsSettingsMenuExit )
                {
                returnValue = ClosingEmailSettingsItemDialogL();
                }
            else
                {
                TRAP_IGNORE( ClosingEmailSettingsItemDialogL() ); 
                returnValue = ETrue; // to be sure of not leaving and exiting
                }
            }
            break;
        case EAknSoftkeyBack:
            {
            iExitCode = ESmumSmsSettingsBack;
            returnValue = ClosingEmailSettingsItemDialogL();
            }
            break;
        default :
            returnValue = CAknDialog::OkToExitL( aButtonId );
            break;
        }

    return returnValue;
    }

// ----------------------------------------------------
// CSmumEmailServiceCentreDialog::HandleListBoxEventL
//
// ----------------------------------------------------
void CSmumEmailServiceCentreDialog::HandleListBoxEventL(
    CEikListBox* /*aListBox*/, 
    TListBoxEvent aEventType )
    {
    switch( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            OpenSettingPageL( ETrue );
            break;
        default :
            // panic
            break;
        }
    }

// ----------------------------------------------------
// CSmumEmailServiceCentreDialog::OfferKeyEventL
//
// ----------------------------------------------------
TKeyResponse CSmumEmailServiceCentreDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    TKeyResponse returnValue = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
    return returnValue;
    }

// ----------------------------------------------------
// CSmumEmailServiceCentreDialog::ProcessCommandL
//
// ----------------------------------------------------
void CSmumEmailServiceCentreDialog::ProcessCommandL(
    TInt aCommandId )
    {
    CAknDialog::ProcessCommandL( aCommandId ) ;
    switch ( aCommandId )
        {
        case ESmsSettingsChoiceMenuChange:
            OpenSettingPageL( EFalse );
        break;
        case EAknCmdHelp:           
            LaunchHelpL();            
            break;
        case ESmsSettingsChoiceMenuExit:
            { 
            iExitCode = ESmumSmsSettingsMenuExit;
            TryExitL( EAknSoftkeyCancel );
            }
            break;
        default :
            break;
        }
    }

// ----------------------------------------------------
// CSmumEmailServiceCentreDialog::OpenSettingPageL
//
// ----------------------------------------------------
void CSmumEmailServiceCentreDialog::OpenSettingPageL(
    TBool aEnterPressed )
    {
    TInt index = iListBox->CurrentItemIndex();
    __ASSERT_DEBUG( index >= 0 , Panic( ESmsetdlgInvalidIndex ));
    
    if ( KErrNotFound != index )//check we have something in listbox
        {
        if ( iModifiable ) // Permisson to edit
            { // Edit the selected setting
            ModifySettingNumberL( aEnterPressed, index );
            }
        else // Show note
            {
            ShowInformationNoteL( R_QTN_SMS_EMAIL_SETT_CANNOT_CHANGE );
            }
        }
    }

// ---------------------------------------------------------
// CSmumEmailServiceCentreDialog::DynInitMenuPaneL
//
// ---------------------------------------------------------
void CSmumEmailServiceCentreDialog::DynInitMenuPaneL(
    TInt aMenuId, 
    CEikMenuPane* aMenuPane)
    {
    if ( aMenuPane )
        {
        switch ( aMenuId )
            {
            case R_SMS_SERVICECENTRE_ITEM_CHOICE_MENUPANE:
                // Help handling
                aMenuPane->SetItemDimmed( EAknCmdHelp, !iHelpFeatureSupported );
                break;
            default :
                // panic
                break;
            }
        }
    }

// ----------------------------------------------------
// CSmumEmailServiceCentreDialog::ModifySettingNumberL
//
// ----------------------------------------------------
void CSmumEmailServiceCentreDialog::ModifySettingNumberL( TBool aEnterPressed, TInt aIndex )
    {
    TBool validNumber = EFalse;
    TBool canExit = EFalse;
    // Take the original number safe
    TBuf<KSCNameLength> origNumber = iSettingsArray->At( aIndex ).iUserText;
    // Check validity
    while ( !canExit )
        {
        // Did user press ok in setting page ?
        if ( iSettingsArray->EditItemL( aIndex, aEnterPressed ))
            {
            // yes, ok pressed
            // Check number with commonphoneparser
            validNumber = CommonPhoneParser::ParsePhoneNumber( 
                iSettingsArray->At( aIndex ).iUserText,
                CommonPhoneParser::ESMSNumber );
            if ( validNumber )
                {                
                canExit = ETrue;
                // Update the ListView accordingly
		    	iListBox->DrawNow();    
                }
            else 
                {
                // Show "invalid number"-note
                ShowInformationNoteL( R_QTN_INFO_INVALID_NUMBER );
                canExit = EFalse;
                }            
            }
        else
            {
            // no, cancel pressed
            // In case of cancel, restore the value
            iSettingsArray->At( aIndex ).iUserText = origNumber;
            canExit = ETrue;
            }
        }
    }

// ----------------------------------------------------
// CSmumEmailServiceCentreDialog::PreLayoutDynInitL
//
// ----------------------------------------------------
void CSmumEmailServiceCentreDialog::PreLayoutDynInitL()
    {
    // Setting arrays in order
    iListBox = static_cast<CEikTextListBox*> ( Control( ESmsSCAddEditId ) );
    iSettingsArray = CMuiuSettingsArray::NewL( R_EMAIL_SERVICECENTRE_ITEMS );

    iSettingsArray->At( ESmumEmailServiceCentreNumber ).iUserText.Copy( iSC );
    iSettingsArray->At( ESmumEmailGateway ).iUserText.Copy( iGateway );

    iListBox->SetListBoxObserver( this );

    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray( iSettingsArray );

    // Fetch pointer to titlepane
    CEikStatusPane *sp =  static_cast<CAknAppUi*> 
        ( CEikonEnv::Static()->EikAppUi())->StatusPane();
    iTitlePane = static_cast<CAknTitlePane*> 
        ( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle )) );
    // Take old titlepane text safe
    iPreviousTitleText = ( *iTitlePane->Text() ).Alloc();
    HBufC* text = StringLoader::LoadLC( R_QTN_MCE_SETT_TITLE_SMS_MAIL );
    iTitlePane->SetTextL( *text );
    CleanupStack::PopAndDestroy(); // text
    }

// ----------------------------------------------------
// CSmumEmailServiceCentreDialog::ClosingEmailSettingsItemDialogL
//
// ----------------------------------------------------
TBool CSmumEmailServiceCentreDialog::ClosingEmailSettingsItemDialogL()
    {
    TBool retValue = EFalse;
    TInt scSize( 
        iSettingsArray->At( ESmumEmailServiceCentreNumber ).iUserText.Size() );
    TInt gatewaySize( 
        iSettingsArray->At( ESmumEmailGateway ).iUserText.Size() );
    // Did user left empty fields?
    if ( !scSize || !gatewaySize )
        { // Yes, we have empty fields
        if ( iExitCode == ESmumSmsSettingsSystemExit ) // Are we exiting?
            { // Exiting fast without any queries or titlepane updates
            iSC.Zero();
            iGateway.Zero();
            retValue = ETrue;
            }
        else 
            {
            // do we exit without valid settings?
            CAknQueryDialog* dlg = CAknQueryDialog::NewL();
            if ( dlg->ExecuteLD( R_EMAIL_SETTINGS_INCOMPLETE_QUERY ) )
                {// Yes
                // reset values
                iSC.Zero();
                iGateway.Zero();
                retValue = ETrue;
                }
            else
                {// No
                // Set the focus accordingly
                if ( !gatewaySize )
                    {
                    iListBox->SetCurrentItemIndexAndDraw( 
                        ESmumEmailGateway );
                    }
                else
                    {
                    iListBox->SetCurrentItemIndexAndDraw( 
                        ESmumEmailServiceCentreNumber );
                    }
                }
            }
        }
    else{ // no empty fields
        iSC.Copy( iSettingsArray->At( 
            ESmumEmailServiceCentreNumber ).iUserText );
        iGateway.Copy( 
            iSettingsArray->At( ESmumEmailGateway ).iUserText );
        retValue = ETrue;
        }
    
    if ( retValue && iExitCode == ESmumSmsSettingsBack )
        {
        // Replace title with old title text
        iTitlePane->SetTextL( *iPreviousTitleText );
        }

    return retValue;
    }

// ----------------------------------------------------
// CSmumEmailServiceCentreDialog::LaunchHelpL
// launch help using context
//
// ----------------------------------------------------
void CSmumEmailServiceCentreDialog::LaunchHelpL() const
    {
    if ( iHelpFeatureSupported )
        {
        CCoeAppUi* editorAppUi = static_cast<CCoeAppUi*>
            ( ControlEnv()->AppUi() );
        CArrayFix<TCoeHelpContext>* helpContext = editorAppUi->AppHelpContextL();
        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
        }
    }

// ----------------------------------------------------
// CSmumEmailServiceCentreDialog::GetHelpContext
// returns helpcontext as aContext
//
// ----------------------------------------------------
void CSmumEmailServiceCentreDialog::GetHelpContext
        (TCoeHelpContext& aContext) const
    {    
    const TUid KUidMce ={0x100058C5};      
    aContext.iMajor = KUidMce;
    aContext.iContext = KSMS_HLP_SETTINGS_SMS_EMAIL;
    }

// ----------------------------------------------------
// CSmumEmailServiceCentreDialog::ShowInformationNoteL
//
// ----------------------------------------------------
void CSmumEmailServiceCentreDialog::ShowInformationNoteL( TInt aResource )
    {
    HBufC* text = StringLoader::LoadLC( aResource, iCoeEnv );
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy(); // text
    }
//  End of File  

