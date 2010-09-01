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
*     Dialog for handling services of certain mtm (if possible to create services).
*
*/



// INCLUDE FILES

#include <msvids.h>
#include <MsgArrays.h>      // CUidNameArray
#include <eikmenup.h>       // CEikMenuPane, CEikMenuPaneItem
#include <StringLoader.h>   // StringLoader
#include <aknViewAppUi.h>

#include <aknlists.h>       // CAknPopupSingle1StyleListBox
#include <MceSettings.rsg>
#include "MceSettingsMtmServicesDialog.h"
#include "MceSettingsIds.hrh"
#include "MceSettingsAccountManager.h"
#include "MceSettingsArrays.h"

#include "MceSettingsMultipleServicesDialog.h"

#include <featmgr.h>
#include <hlplch.h>             // For HlpLauncher
#include <csxhelp/mbxs.hlp.hrh>

#include <bldvariant.hrh>
#include <SenduiMtmUids.h>

#include <ImumInternalApi.h>
#include <ImumInHealthServices.h>
#include <ImumInMailboxUtilities.h>

#include "MceSettingsUtils.h"
#include <muiuflags.h>

// CONSTANTS

#define KMceApplicationUidValue         0x100058C5
const TUid KMceApplicationUid       = {KMceApplicationUidValue};


const TInt KMceVisibleTextLength = KHumanReadableNameLength;

// The position of the middle soft key
const TInt KMSKPosition = 3;


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
// ----------------------------------------------------
// CMceSettingsMtmServicesDialog::Constructor
// ----------------------------------------------------
CMceSettingsMtmServicesDialog::CMceSettingsMtmServicesDialog(
    TUid                aMtmType,
    MMceSettingsAccountManager& aManager,
    CMsvSession*        aSession,
    CMceSettingsMultipleServicesDialog& aParent )
    :
    CMceSettingsTitlePaneHandlerDialog(),
    iMtmType( aMtmType ),    
    iAccountManager( aManager ),
    iSession( aSession ),
    iParentDialog( aParent ),
    iCurrentMailAccountId( KErrNotFound )
    {
    }

// ----------------------------------------------------
// CMceSettingsMtmServicesDialog::Destructor
// ----------------------------------------------------
CMceSettingsMtmServicesDialog::~CMceSettingsMtmServicesDialog()
    {
    if ( iSessionObserverAdded )
        {
        iAccountManager.Session().RemoveObserver( *this );
        }
    delete iAccountArray;
    delete iEmailApi;
    iEmailApi = NULL;   
    iMailboxArray.Reset();
    }


// ----------------------------------------------------
// CMceSettingsMtmServicesDialog::OkToExitL
// ----------------------------------------------------
TBool CMceSettingsMtmServicesDialog::OkToExitL( TInt aButtonId )
    {
    TBool okToExit = CAknDialog::OkToExitL( aButtonId );
    if ( aButtonId == EMceSettingsCmdSettingsDialogOpen ||
         aButtonId == EMceSettingsCmdSettingsDialogCreateNewEmpty )
        {
        ProcessCommandL( aButtonId );
        okToExit = EFalse;  // Cannot exit, since MSK was pressed
        }
    else if ( aButtonId != EEikBidCancel && okToExit )
        {
        RestoreTitleTextL();
        }
        
    if ( okToExit && aButtonId == EEikBidCancel )
        {
        // Let's inform parent that we are closed
        iParentDialog.SubdialogClosed();
        }

    return okToExit;
    }

// ----------------------------------------------------
// CMceSettingsMtmServicesDialog::HandleListBoxEventL
// ----------------------------------------------------
void CMceSettingsMtmServicesDialog::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType )
    {
    switch(aEventType)
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            ProcessCommandL( EMceSettingsCmdSettingsDialogOpen );
            break;
        default:
            break;
        }
    }

// ----------------------------------------------------
// CMceSettingsMtmServicesDialog::ProcessCommandL
// ----------------------------------------------------
void CMceSettingsMtmServicesDialog::ProcessCommandL( TInt aCommandId )
    {
    CAknDialog::ProcessCommandL( aCommandId ); // hides menu
    if ( aCommandId == EMceSettingsCmdSettingsDialogExit )
        {
        if (iAvkonViewAppUi->ExitHidesInBackground()) 
            { 
            iAvkonViewAppUi->ProcessCommandL( EAknCmdHideInBackground );
            }
        else
            {
            iAvkonViewAppUi->ProcessCommandL( EAknCmdExit );
            }
        return;
        }

    
    CEikTextListBox* list=STATIC_CAST( CEikTextListBox*, 
        Control( EMceSettingsMtmServicesDialogChoiceList ) );
    const TInt choice = list->CurrentItemIndex();
    TInt count = iAccountArray->Count();

    switch ( aCommandId )
        {
        case EMceSettingsCmdSettingsDialogOpen:
            if ( choice >= 0 && choice < count )
                {
                iCurrentMailAccountId = (*iAccountArray)[choice].iUid.iUid;
                iAccountManager.EditAccountL( iCurrentMailAccountId );
                }
            break;

        case EMceSettingsCmdSettingsDialogCreateNewEmpty:
            iMailboxCreationOn = ETrue;
            iAccountManager.CreateNewAccountL( iMtmType, KMsvNullIndexEntryId );
            iMailboxCreationOn = EFalse;
            break;

        case EMceSettingsCmdSettingsDialogDelete:
            if ( choice >= 0 && choice < count )
                {
                iAccountManager.DeleteAccountL( (*iAccountArray)[choice].iUid.iUid );
                }
            break;
       
        case EAknCmdHelp:
            {
            LaunchHelpL();                
            }
            break;            

        default :
            break;
        }
    }

// ----------------------------------------------------
// CMceSettingsMtmServicesDialog::DynInitMenuPaneL
// ----------------------------------------------------
void CMceSettingsMtmServicesDialog::DynInitMenuPaneL( TInt aResourceId, 
                                                       CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_MCE_SETTINGS_MTM_SERVICES_DIALOG_MENUPANE )
        {
        aMenuPane->SetItemDimmed( EMceSettingsCmdSettingsDialogHelp,
            !FeatureManager::FeatureSupported( KFeatureIdHelp ) );  

        if ( iAccountArray->Count() == 0 )
            {
            aMenuPane->DeleteMenuItem( EMceSettingsCmdSettingsDialogOpen );
            aMenuPane->DeleteMenuItem( EMceSettingsCmdSettingsDialogDelete );
            if ( iSelectionKeyPressedToOpenOptionMenu )
                {
                aMenuPane->DeleteMenuItem( EMceSettingsCmdSettingsDialogExit );
                aMenuPane->DeleteMenuItem( EAknCmdHelp );
                iSelectionKeyPressedToOpenOptionMenu = EFalse;
                }
            }
        if ( iNumberOfMailboxes >= KMceMaxNumberOfMailboxes )
            {
            aMenuPane->DeleteMenuItem( EMceSettingsCmdSettingsDialogCreateNewEmpty );
            }
        else
            {
            // no action
            }

        }
    }
  
// ---------------------------------------------------------------------------
// CMceSettingsMtmServicesDialog::UpdateMailboxDataL()
// ---------------------------------------------------------------------------
//
void CMceSettingsMtmServicesDialog::UpdateMailboxDataL()
    {
    const MImumInHealthServices* healthApi = &iEmailApi->HealthServicesL();
    
    // Get the array of the mailboxes for the current view
    MceSettingsUtils::GetHealthyMailboxListL( 
        *healthApi,
        iMailboxArray,
        iMtmType != KSenduiMtmSyncMLEmailUid,
        iMtmType != KSenduiMtmSyncMLEmailUid,
        iMtmType == KSenduiMtmSyncMLEmailUid,
        EFalse );
    iNumberOfMailboxes = iMailboxArray.Count();
    }

// ----------------------------------------------------
// CMceSettingsMtmServicesDialog::UpdateForArrayChangeL
// ----------------------------------------------------
void CMceSettingsMtmServicesDialog::UpdateServicesArrayL()
    {        
    CEikTextListBox* list=STATIC_CAST( CEikTextListBox*, 
        Control( EMceSettingsMtmServicesDialogChoiceList ) );
    TInt selectedIndex = list->CurrentItemIndex();
    const TInt count = iAccountArray->Count();

    iAccountArray->Reset();

    UpdateMailboxDataL();
    for ( TInt loop=0; loop<iMailboxArray.Count(); loop++ )
        {
        TMsvEntry tentry = iEmailApi->MailboxUtilitiesL().GetMailboxEntryL( 
            iMailboxArray[loop],
            MImumInMailboxUtilities::ERequestSending );

        TUid tmpUid = {iMailboxArray[loop]};
        TUidNameInfo info(
            tmpUid,
            tentry.iDetails.Left( KMceVisibleTextLength ) );
        iAccountArray->AppendL( info );                
        if ( iCurrentMailAccountId == tmpUid.iUid )
            {
            selectedIndex = loop;
            iCurrentMailAccountId = KErrNotFound;
            }
        }

    TInt newCount = iAccountArray->Count();
    if ( newCount > count )
        {
        list->HandleItemAdditionL();
        }
    else if ( newCount < count )
        {
        list->HandleItemRemovalL();
        }
    else
        {
        // no action
        }

    if ( newCount > 0 )
        {
        if ( selectedIndex >= 0 && selectedIndex < newCount )
            {
            list->SetCurrentItemIndex( selectedIndex );
            }
        else if ( selectedIndex >= newCount )
            {
            list->SetCurrentItemIndex( newCount - 1 );
            }
        else
            {
            list->SetCurrentItemIndex( 0 );
            }
        }
    if ( newCount == 0 )
        {
        SetSpecialMSKButtonL(R_MCE_MSK_BUTTON_CREATE_NEW_EMPTY);
        }
    else
    	{
    	SetSpecialMSKButtonL( R_MCE_MSK_BUTTON_OPEN );
    	}        
    list->DrawNow();
    }

// ----------------------------------------------------
// CMceSettingsMtmServicesDialog::PreLayoutDynInitL
// ----------------------------------------------------
void CMceSettingsMtmServicesDialog::PreLayoutDynInitL()
    {    
    CEikTextListBox* list=STATIC_CAST( CEikTextListBox*, 
        Control( EMceSettingsMtmServicesDialogChoiceList ) );

    // Setting scrollbar-indicators on
    list->CreateScrollBarFrameL( ETrue );
    list->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    if ( !iAccountArray )
        {
        iAccountArray = CMceUidNameArray::NewL( CMceUidNameArray::EServicesList );
        }

    if ( !iEmailApi )
        {
        iEmailApi = CreateEmailApiL( iSession );
        }       

    CTextListBoxModel* model= list->Model();
    model->SetItemTextArray( iAccountArray );
    model->SetOwnershipType( ELbmDoesNotOwnItemArray );
    list->SetListBoxObserver(this);    

    THumanReadableName mtmName = iAccountManager.MtmName( iMtmType );    
    if ( iMtmType == KSenduiMtmSmtpUid )
        {
        // email
        HBufC* text = StringLoader::LoadL( R_MCE_SETTINGS_NO_MAILBOXES, iEikonEnv );
        CleanupStack::PushL( text );
        list->View()->SetListEmptyTextL( *text );
        CleanupStack::PopAndDestroy( text ); // text
        HBufC* title = StringLoader::LoadLC( R_MCE_SETTINGS_MTM_SERVICES_DIALOG_TITLE, 
                                                                  iEikonEnv );
        StoreTitleTextAndSetNewL( *title );
        CleanupStack::PopAndDestroy( title ); // title
        }
    else if ( iMtmType == KSenduiMtmSyncMLEmailUid )
        {
        // syncML
        HBufC* text = StringLoader::LoadL( R_MCE_SETTINGS_NO_SYNCBOX, iEikonEnv );
        CleanupStack::PushL( text );
        list->View()->SetListEmptyTextL( *text );
        CleanupStack::PopAndDestroy( text ); // text
        HBufC* title = StringLoader::LoadLC( R_MCE_SETTINGS_SYNCMAIL_TITLE, iEikonEnv );
        StoreTitleTextAndSetNewL( *title );
        CleanupStack::PopAndDestroy( title ); // title
        }
    else
        {
        // not email/syncML
        HBufC* text = StringLoader::LoadLC( 
        R_MCE_MTM_NOT_DEFINED, mtmName, iEikonEnv );
        list->View()->SetListEmptyTextL( *text );
        CleanupStack::PopAndDestroy( text ); // text
        StoreTitleTextAndSetNewL( mtmName );
        }

    iAccountManager.Session().AddObserverL( *this );
    iSessionObserverAdded = ETrue;

    UpdateServicesArrayL();
    }

// ----------------------------------------------------
// CMceSettingsMtmServicesDialog::OfferKeyEventL
// ----------------------------------------------------
TKeyResponse CMceSettingsMtmServicesDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                                          TEventCode aType)
    {
    if ( aType == EEventKey && aKeyEvent.iCode == EKeyBackspace )
        {
        if( !iEmailApi->IsEmailFeatureSupportedL( 
            CImumInternalApi::EMailLockedSettings ) ) 
            {
            ProcessCommandL( EMceSettingsCmdSettingsDialogDelete );                    
            }
        return EKeyWasConsumed;
        }
    return CAknDialog::OfferKeyEventL( aKeyEvent, aType );
    }

// ----------------------------------------------------
// CMceSettingsMtmServicesDialog::HandleSessionEventL
// ----------------------------------------------------
void CMceSettingsMtmServicesDialog::HandleSessionEventL(
    TMsvSessionEvent aEvent, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    switch ( aEvent )
        {
        case EMsvEntriesCreated:
        case EMsvEntriesChanged:
        case EMsvEntriesDeleted:
            if ( !iMailboxCreationOn )
                {
                UpdateServicesArrayL();                    
                }
            break;        
        default:
            // just ignore
            break;
        }
    }

// ----------------------------------------------------
// CMceSettingsMtmServicesDialog::GetHelpContext
// returns helpcontext as aContext
//
// ----------------------------------------------------
void CMceSettingsMtmServicesDialog::GetHelpContext
        (TCoeHelpContext& aContext) const
    {
    aContext.iMajor = KMceApplicationUid;
    if ( iMtmType != KSenduiMtmSyncMLEmailUid )
        { 
        /*aContext.iContext = KMCE_HLP_SETT_SMAILBLIST;
        }
        else
        {*/
        aContext.iContext = KMCE_HLP_SETT_MAILBLIST;
        }
    }

// ----------------------------------------------------
// CMceSettingsMtmServicesDialog::LaunchHelpL
// Launch help using context
// 
// ----------------------------------------------------
void CMceSettingsMtmServicesDialog::LaunchHelpL() const
    { 
    CCoeAppUi* appUi = STATIC_CAST(CCoeAppUi*, ControlEnv()->AppUi());       
    CArrayFix<TCoeHelpContext>* helpContext = appUi->AppHelpContextL();   
    HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
    }

// ----------------------------------------------------
// CMceSettingsMtmServicesDialog::SetSpecialMSKButtonL
// ----------------------------------------------------
void CMceSettingsMtmServicesDialog::SetSpecialMSKButtonL( TInt aResourceId )
    { 
    CEikButtonGroupContainer& cba = ButtonGroupContainer();
    cba.SetCommandL( KMSKPosition, aResourceId );
    cba.DrawNow();
    }

//  End of File
