/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <bldvariant.hrh>
#include <csxhelp/mce.hlp.hrh>
// locals
#include "SmumSettingsDialogGSM.h"         // KStringLength
#include "SmumServiceCentreItemDialog.h"// CSmumAddEditServiceCentreDialog
#include "SMSU.HRH"                     // resource header
#include "SmumServiceCentresDialog.h"   // CSmsServiceCentresDialog
#include "SmumUtil.h"                   // SmumUtil
#include "smsui.pan"
#include "smum.loc"                      

//For logging
#include "SmumLogging.h"

// CONSTANTS
const TInt KSCNameLength = 100;
const TInt KSmumGranularity = 4;
const TInt KSmumReplaceCharacterCount = 2;

// ================= MEMBER FUNCTIONS =======================

// C++ constructor can NOT contain any code, that
// might leave.
CSmumAddEditServiceCentreDialog::CSmumAddEditServiceCentreDialog(
    TDes&           aName, 
    TDes&           aNumber,
    TInt&           aExitCode,
    TBool           aHelpFeatureSupported,
    CSmumSCArray*   aSCDlgsAddressesList,
    TBool			aSimSCsOnly ) 
        :iName                  ( aName ), 
        iNumber                 ( aNumber ),
        iExitCode               ( aExitCode ),
        iHelpFeatureSupported   ( aHelpFeatureSupported ),
        iSCDlgsAddressesList    ( aSCDlgsAddressesList ),
        iSimSCsOnly				( aSimSCsOnly )
    {
    }

// Destructor
CSmumAddEditServiceCentreDialog::~CSmumAddEditServiceCentreDialog()
    {
    SMUMLOGGER_ENTERFN(" CSmumAddEditServiceCentreDialog::~CSmumAddEditServiceCentreDialog");
    if ( iListBox )
        {
        iListBox->SetScrollBarFrame( NULL, CEikListBox::EOwnedExternally );
        }
    delete iPreviousTitleText;
    // iSettingsArray, iListBox, iSCDlgsAddressesList, iTitlePane are
    // plain pointers to objects owned by other classes which take
    // care also about deletion.
    SMUMLOGGER_LEAVEFN(" CSmumAddEditServiceCentreDialog::~CSmumAddEditServiceCentreDialog");
    }

// ----------------------------------------------------
// CSmumAddEditServiceCentreDialog::OkToExitL
//
// ----------------------------------------------------
TInt CSmumAddEditServiceCentreDialog::OkToExitL( TInt aButtonId )
    {
    SMUMLOGGER_ENTERFN(" CSmumAddEditServiceCentreDialog::OkToExitL");
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
            TRAP_IGNORE( ClosingSCItemDialogL() ); 
            returnValue = ETrue; // to be sure of not leaving and exiting
            }
            break;
        case EAknSoftkeyBack:
            {
            iExitCode = ESmumSmsSettingsBack;
            returnValue = ClosingSCItemDialogL();
            }
            break;
        default :
            returnValue = CAknDialog::OkToExitL( aButtonId );
            break;
        }

    SMUMLOGGER_LEAVEFN(" CSmumAddEditServiceCentreDialog::OkToExitL");
    return returnValue;
    }

// ----------------------------------------------------
// CSmumAddEditServiceCentreDialog::HandleListBoxEventL
//
// ----------------------------------------------------
void CSmumAddEditServiceCentreDialog::HandleListBoxEventL(
    CEikListBox* /*aListBox*/, 
    TListBoxEvent aEventType )
    {
    SMUMLOGGER_ENTERFN(" CSmumAddEditServiceCentreDialog::HandleListBoxEventL");
    switch( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            //for touch input
            OpenSettingPageL( ETrue );            
            break;
        default :
            // panic
            break;
        }
    SMUMLOGGER_LEAVEFN(" CSmumAddEditServiceCentreDialog::HandleListBoxEventL");
    }

// ----------------------------------------------------
// CSmumAddEditServiceCentreDialog::OfferKeyEventL
//
// ----------------------------------------------------
TKeyResponse CSmumAddEditServiceCentreDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    TKeyResponse returnValue = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
    return returnValue;
    }

// ----------------------------------------------------
// CSmumAddEditServiceCentreDialog::ProcessCommandL
//
// ----------------------------------------------------
void CSmumAddEditServiceCentreDialog::ProcessCommandL(
    TInt aCommandId )
    {
    SMUMLOGGER_ENTERFN(" CSmumAddEditServiceCentreDialog::ProcessCommandL");
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
            // panic
            break ;
        }
    SMUMLOGGER_LEAVEFN(" CSmumAddEditServiceCentreDialog::ProcessCommandL");
    }

// ----------------------------------------------------
// CSmumAddEditServiceCentreDialog::OpenSettingPageL
//
// ----------------------------------------------------
void CSmumAddEditServiceCentreDialog::OpenSettingPageL(
    TBool aEnterPressed )
    {
    SMUMLOGGER_ENTERFN(" CSmumAddEditServiceCentreDialog::OpenSettingPageL");
    TInt index = iListBox->CurrentItemIndex();
    __ASSERT_DEBUG( index >= 0 , Panic( ESmsetdlgInvalidIndex ));
	
	if ( iSimSCsOnly ) // Only Sim SC's variation is on
	    {
		SmumUtil::ShowInformationNoteL( R_QTN_SMS_EMAIL_SETT_CANNOT_CHANGE );
		return;
	    }    
    if ( KErrNotFound != index )//check we have something in listbox
        {
        // Launched from CSmsMainSettingsDialog (no centres), 
        // so complex checking of names not needed.
        if ( !iSCDlgsAddressesList && ESmumServiceCentreName == index )
            {
            iSettingsArray->EditItemL( index, aEnterPressed );
            //Clear unwanted characters
            ReplaceCharacters( 
                iSettingsArray->At( ESmumServiceCentreName ).iUserText );
            // Update title to show service centre name
            iTitlePane->SetTextL(
                iSettingsArray->At( ESmumServiceCentreName ).iUserText );
            }
        // ESmumServiceCentreName is to be modified ? 
        else if ( ESmumServiceCentreName == index )
            {
            ModifyScNameL( aEnterPressed, index );
            }
        else{ // No, ESmumServiceCentreNumber is to be modified 
            ModifyScNumberL( aEnterPressed, index );
            }
        iListBox->DrawNow();
        }
    SMUMLOGGER_LEAVEFN(" CSmumAddEditServiceCentreDialog::OpenSettingPageL");
    }

// ---------------------------------------------------------
// CSmumAddEditServiceCentreDialog::DynInitMenuPaneL
//
// ---------------------------------------------------------
void CSmumAddEditServiceCentreDialog::DynInitMenuPaneL(
    TInt aMenuId, 
    CEikMenuPane* aMenuPane)
    {
    SMUMLOGGER_ENTERFN(" CSmumAddEditServiceCentreDialog::DynInitMenuPaneL");
    switch ( aMenuId )
        {
        case R_SMS_SERVICECENTRE_ITEM_CHOICE_MENUPANE:
        	if ( iSimSCsOnly ) // Only Sim SC's variation is on
                {
	            aMenuPane->SetItemDimmed( ESmsSettingsChoiceMenuChange, ETrue );    	
	            }  
            // Help handling
            aMenuPane->SetItemDimmed( EAknCmdHelp, !iHelpFeatureSupported );
            break;
        default :
            // panic
            break;
        }
    SMUMLOGGER_LEAVEFN(" CSmumAddEditServiceCentreDialog::DynInitMenuPaneL");
    }

// ----------------------------------------------------
// CSmumAddEditServiceCentreDialog::ModifyScNameL
//
// ----------------------------------------------------
void CSmumAddEditServiceCentreDialog::ModifyScNameL( TBool aEnterPressed, TInt aIndex )
    {
    SMUMLOGGER_ENTERFN(" CSmumAddEditServiceCentreDialog::ModifyScNameL");
    TBuf<KSmumStringLength> name;
    TBool canExit = EFalse;
    TBool found = EFalse;
    const TInt lBoxCount = iSCDlgsAddressesList->Count();
    // Take the original name safe
    TBuf<KSCNameLength> origName = iSettingsArray->At( ESmumServiceCentreName ).iUserText;
    
    // Check for dublicates
    while ( !canExit )
        {
        // Did user press ok in setting page ?
        if ( iSettingsArray->EditItemL( aIndex, aEnterPressed ))
            {
            // yes, ok pressed
            ReplaceCharacters( iSettingsArray->At( ESmumServiceCentreName ).iUserText );
            iSettingsArray->At( ESmumServiceCentreName ).iUserText.Trim();
            name = iSettingsArray->At( ESmumServiceCentreName ).iUserText;
            found = EFalse;
            // Do we have many service centres defined ?
            if ( lBoxCount )
                {
                // Loop lBoxCount through for duplicates
                for ( TInt loop=0; loop < lBoxCount && !found; loop++ )
                    {
                    if ( name != iSCDlgsAddressesList->SCNameInfo( loop ).Name()
                        || name == iName )
                        {                
                        canExit = ETrue;
                        // Update title to show service centre name
                        iTitlePane->SetTextL(
                            iSettingsArray->At( ESmumServiceCentreName ).iUserText);
                        }
                    else 
                        {
                        // Show "already in use"-query
                        HBufC* errorText = StringLoader::LoadLC(
                            R_QTN_FLDR_RENAME_QUERY, name, iCoeEnv );
                        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
                        if ( !( dlg->ExecuteLD(R_SC_NAME_ALREADY_IN_USE_QUERY, *errorText )))
                            {// Rename? -> No, so cancelling name changing operation
                            CleanupStack::PopAndDestroy(); // errorText
                            iSettingsArray->At( ESmumServiceCentreName ).iUserText = origName;
                            return;
                            }
                        
                        CleanupStack::PopAndDestroy(); // errorText
                        
                        found = ETrue;
                        canExit = EFalse;
                        }
                    }
                }
            else // no service centres, so no need to check for duplicates, can exit 
                {
                canExit = ETrue;
                // Update title to show service centre name
                iTitlePane->SetTextL(
                    iSettingsArray->At( ESmumServiceCentreName ).iUserText);
                }
            }
        else
            {
            // no, cancel pressed
            canExit = ETrue;
            iSettingsArray->At( ESmumServiceCentreName ).iUserText = origName;
            }
        }
    SMUMLOGGER_LEAVEFN(" CSmumAddEditServiceCentreDialog::ModifyScNameL");
    }

// ----------------------------------------------------
// CSmumAddEditServiceCentreDialog::ModifyScNumberL
//
// ----------------------------------------------------
void CSmumAddEditServiceCentreDialog::ModifyScNumberL( TBool aEnterPressed, TInt aIndex )
    {
    SMUMLOGGER_ENTERFN(" CSmumAddEditServiceCentreDialog::ModifyScNumberL");
    TBool validNumber = EFalse;
    TBool canExit = EFalse;

    // Take the original number safe
    TBuf<KSCNameLength> origNumber = iSettingsArray->At( ESmumServiceCentreNumber ).iUserText;
    
    // Check for dublicates
    while ( !canExit )
        {
        // Did user press ok in setting page ?
        if ( iSettingsArray->EditItemL( aIndex, aEnterPressed ))
            {
            // yes, ok pressed
            
            // Check number with commonphoneparser
            validNumber = CommonPhoneParser::ParsePhoneNumber( 
                iSettingsArray->At(ESmumServiceCentreNumber).iUserText,
                CommonPhoneParser::ESMSNumber );
            
            if ( validNumber )
                {                
                canExit = ETrue;
                }
            else 
                {
                // Show "invalid number"-note
                CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue ); // Wait-flag on 
                HBufC* text = StringLoader::LoadLC( R_QTN_INFO_INVALID_NUMBER, iCoeEnv );
                note->ExecuteLD( *text );
                CleanupStack::PopAndDestroy(); // text
                canExit = EFalse;
                }            
            }
        else
            {
            // no, cancel pressed

            // In case of cancel, restore the value
            iSettingsArray->At( ESmumServiceCentreNumber ).iUserText = origNumber;
            canExit = ETrue;
            }
        }
    SMUMLOGGER_LEAVEFN(" CSmumAddEditServiceCentreDialog::ModifyScNumberL");
    }

// ----------------------------------------------------
// CSmumAddEditServiceCentreDialog::PreLayoutDynInitL
//
// ----------------------------------------------------
void CSmumAddEditServiceCentreDialog::PreLayoutDynInitL()
    {
    SMUMLOGGER_ENTERFN(" CSmumAddEditServiceCentreDialog::PreLayoutDynInitL");
    // Setting arrays in order
    iListBox = STATIC_CAST( CEikTextListBox*, Control( ESmsSCAddEditId ));
    iSettingsArray = CMuiuSettingsArray::NewL( R_SERVICECENTRE_ITEMS );

    // Arabic-indic conversion not to be done to SMSC Name
    (*iSettingsArray)[0].iLangSpecificNumConv = EFalse;

    // Are we adding a new SC? If so let's get a default name for it.
    if ( !iName.Size())
        {
        // Do we have a list of names?
        if ( iSCDlgsAddressesList )
            { // Yes, let's make a nameArray and provide it to FindDefaultNameForSCL
            CDesCArrayFlat* nameArray = new ( ELeave ) CDesCArrayFlat( KSmumGranularity );
            CleanupStack::PushL( nameArray );
            
            TInt amountOfSCs = iSCDlgsAddressesList->Count();
            for ( TInt loop = 0 ; loop < amountOfSCs; loop++ )
                {
                nameArray->AppendL( iSCDlgsAddressesList->SCNameInfo( loop ).Name());
                }
            
            SmumUtil::FindDefaultNameForSCL( iName, EFalse, nameArray );
            CleanupStack::PopAndDestroy();// nameArray
            }
        else
            {// No, so this have to be first SC.
            SmumUtil::FindDefaultNameForSCL( iName, EFalse );
            }
        }

    iSettingsArray->At( ESmumServiceCentreName ).iUserText.Copy( iName );
    iSettingsArray->At( ESmumServiceCentreNumber ).iUserText.Copy( iNumber );

    iListBox->SetListBoxObserver( this );

    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray( iSettingsArray );
    // scroll bars
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    // Fetch pointer to titlepane
    CEikStatusPane *sp =  ( STATIC_CAST(
        CAknAppUi*, ( CEikonEnv::Static()->EikAppUi())))->StatusPane();
    iTitlePane = STATIC_CAST (
        CAknTitlePane*, sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle )));

    // Take old titlepane text safe and replace with current sc name
    iPreviousTitleText = ( *iTitlePane->Text()).Alloc();
    iTitlePane->SetTextL( iName, ETrue );
    SMUMLOGGER_LEAVEFN(" CSmumAddEditServiceCentreDialog::PreLayoutDynInitL");
    }

// ----------------------------------------------------
// CSmumAddEditServiceCentreDialog::ClosingSCItemDialogL
//
// ----------------------------------------------------
TBool CSmumAddEditServiceCentreDialog::ClosingSCItemDialogL()
    {
    SMUMLOGGER_ENTERFN(" CSmumAddEditServiceCentreDialog::ClosingSCItemDialogL");
    TBool retValue = EFalse;

    // Did user left empty fields?
    if ( !iSettingsArray->At( ESmumServiceCentreNumber ).iUserText.Size() ||
        !iSettingsArray->At( ESmumServiceCentreName ).iUserText.Size())
        { // Yes, we have empty fields
        if ( iExitCode == ESmumSmsSettingsMenuExit ||
             iExitCode == ESmumSmsSettingsSystemExit ) // Are we exiting?
            { // Exiting fast without any queries or titlepane updates
            iNumber.Zero();
            iName.Zero();
            retValue = ETrue;
            }
        else 
            {
            // do we delete it?
            CAknQueryDialog* dlg = CAknQueryDialog::NewL();
            if ( dlg->ExecuteLD( R_SC_WITHOUT_ALL_FIELDS_DELETE_QUERY ))
                {// Yes
                
                // reset values
                iNumber.Zero();
                iName.Zero();
                retValue = ETrue;
                }
            }
        
        }
    else{ // no empty fields
        iNumber.Copy( iSettingsArray->At( ESmumServiceCentreNumber ).iUserText);
        iName.Copy( iSettingsArray->At( ESmumServiceCentreName ).iUserText);
        retValue = ETrue;
        }
    
    if ( retValue && iExitCode == ESmumSmsSettingsBack )
        {
        // Replace title with old title text
        iTitlePane->SetTextL( *iPreviousTitleText );
        }

    SMUMLOGGER_LEAVEFN(" CSmumAddEditServiceCentreDialog::ClosingSCItemDialogL");
    return retValue;
    }

// ----------------------------------------------------
// CSmumAddEditServiceCentreDialog::ReplaceCharacters
//
// ----------------------------------------------------
void CSmumAddEditServiceCentreDialog::ReplaceCharacters( TDes& aText )
    {
    TBuf<KSmumReplaceCharacterCount> replaceChars;
    replaceChars.Zero();
    replaceChars.Append( CEditableText::EParagraphDelimiter );
    replaceChars.Append( CEditableText::ETabCharacter );
    AknTextUtils::ReplaceCharacters(
        aText, 
        replaceChars, 
        CEditableText::ESpace );
    }

// ----------------------------------------------------
// CSmumAddEditServiceCentreDialog::LaunchHelpL
// launch help using context
//
// ----------------------------------------------------
void CSmumAddEditServiceCentreDialog::LaunchHelpL()
    {
    if ( iHelpFeatureSupported )
        {
        CCoeAppUi* editorAppUi = STATIC_CAST(CCoeAppUi*, ControlEnv()->AppUi());
        CArrayFix<TCoeHelpContext>* helpContext = editorAppUi->AppHelpContextL();
        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
        }
    }

// ----------------------------------------------------
// CSmumAddEditServiceCentreDialog::GetHelpContext
// returns helpcontext as aContext
//
// ----------------------------------------------------
void CSmumAddEditServiceCentreDialog::GetHelpContext
        (TCoeHelpContext& aContext) const
    {    
    const TUid KUidMce ={0x100058C5};      
    aContext.iMajor = KUidMce;
    aContext.iContext = KSMS_HLP_SETTINGS_SCENTRES_EDIT;
    }

//  End of File  

