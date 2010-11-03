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
*     Service Centres dialog for Sms.
*
*/



// INCLUDE FILES
#include <eikmenup.h>                   // CEikMenuPane
#include <akntitle.h>                   // CAknTitlePane
#include <SMUM.rsg>                     // resource identifiers
#include <smutset.h>                    // CSmsSettings
#include <AknQueryDialog.h>             // CAknQueryDialog
#include <StringLoader.h>               // StringLoader
#include <aknlists.h>
#include <aknPopup.h>
#include <eiklbx.h>                     // EOwnedExternally
#include <hlplch.h>                     // For HlpLauncher
#include <bldvariant.hrh>
#include <csxhelp/mce.hlp.hrh>
#include <centralrepository.h> 
#include <AvkonInternalCRKeys.h>  		// KAknMiddleSoftkeyEnabled
#include <aknenv.h>                     // AppShutter
// locals
#include "SMSU.HRH"                     // resource header
#include "SMSETDLG.H"                   // TSmumSCNameInfo, TSmsSettingSCNames, CSmumSCArray
#include "SmumServiceCentreItemDialog.h"// CSmumAddEditServiceCentreDialog
#include "SmumSettingsDialogGSM.h"         // KStringLength
#include "SmumServiceCentresDialog.h"   // CSmumServiceCentresDialog
#include "smsui.pan"                    // panics
#include "SmumUtil.h"                   // SmumUtil

//For logging
#include "SmumLogging.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TInt KNameListGranularity = 8;
const TInt KSmumSCArray = 4;

// ================= MEMBER FUNCTIONS =======================

// C++ constructor can NOT contain any code, that
// might leave.
//
CSmumServiceCentresDialog::CSmumServiceCentresDialog(
    CSmsSettings& aSettings, TInt& aExitCode, TBool aHelpFeatureSupported, TBool aSimSCsOnly )
        : iSettings ( aSettings ),
          iExitCode ( aExitCode ),
          iHelpFeatureSupported ( aHelpFeatureSupported ),
          iSimSCsOnly( aSimSCsOnly ),
          iIdle (NULL)
    {
    }
   
// Destructor
CSmumServiceCentresDialog::~CSmumServiceCentresDialog()
    {
    SMUMLOGGER_ENTERFN(" CSmumServiceCentresDialog::~CSmumServiceCentresDialog");
    if ( iListBox )
        {
        iListBox->SetScrollBarFrame( NULL, CEikListBox::EOwnedExternally );
        }
    delete iPreviousTitleText;
    delete iSCAddressesList;
    delete iIdle;
    // iListBox, iSettings(reference), iTitlePane are
    // plain pointers to objects owned by other classes which take
    // care also about deletion.
    SMUMLOGGER_LEAVEFN(" CSmumServiceCentresDialog::~CSmumServiceCentresDialog");
    }

// ----------------------------------------------------
// CSmumServiceCentresDialog::NewL
//
// ----------------------------------------------------
CSmumServiceCentresDialog* CSmumServiceCentresDialog::NewL(
    CSmsSettings& aSettings,
    TInt& aExitCode,
    TInt aMenuTitleResourceId,
    TBool aHelpFeatureSupported,
    TBool aSimSCsOnly )
    {
    SMUMLOGGER_ENTERFN(" CSmumServiceCentresDialog::NewL");
    CSmumServiceCentresDialog* self = 
        new ( ELeave ) CSmumServiceCentresDialog( 
        	aSettings, aExitCode, aHelpFeatureSupported, aSimSCsOnly );
    CleanupStack::PushL( self );
    self->ConstructL( aMenuTitleResourceId );
    CleanupStack::Pop(); // self
    SMUMLOGGER_LEAVEFN(" CSmumServiceCentresDialog::NewL");
    return self;
    }

// ----------------------------------------------------
// CSmumServiceCentresDialog::ConstructL
//
// ----------------------------------------------------
void CSmumServiceCentresDialog::ConstructL( TInt aMenuTitleResourceId )
    {
    SMUMLOGGER_ENTERFN(" CSmumServiceCentresDialog::ConstructL");
    // Creating the listbox-list for service centres
    iSCAddressesList = CSmumSCArray::NewL( KNameListGranularity );
    TInt serviceCentres = 0;
    // read Service centres from SmsSettings
    serviceCentres = iSettings.ServiceCenterCount();
    for ( TInt loop=0; loop < serviceCentres; loop++ )
        {
        TSmumSCNameInfo scAddress(iSettings.GetServiceCenter( loop ).Name(),
                                   iSettings.GetServiceCenter( loop ).Address());
        iSCAddressesList->AppendL( scAddress );
        }
    CAknDialog::ConstructL( aMenuTitleResourceId );
    CRepository* cenRep = NULL;
	TRAPD(err, cenRep = CRepository::NewL( KCRUidAvkon ));
	if (!err)
		{
		err = cenRep->Get( KAknMiddleSoftkeyEnabled, iMSKEnabledInPlatform );
		delete cenRep;
	    }
    SMUMLOGGER_LEAVEFN(" CSmumServiceCentresDialog::ConstructL");
    }

// ----------------------------------------------------
// CSmumServiceCentresDialog::OkToExitL
//
// ----------------------------------------------------
TInt CSmumServiceCentresDialog::OkToExitL( TInt aButtonId )
    {
    SMUMLOGGER_ENTERFN(" CSmumServiceCentresDialog::OkToExitL");
    TInt returnValue;

    switch( aButtonId )
        {
        case ESmsSettingsChoiceMenuOpen:
	        {
	        ProcessCommandL( aButtonId );
	        returnValue = EFalse; // Cannot exit, since MSK was pressed
	        }     
	        break;
        // system / menu exit
        case EAknSoftkeyCancel:
            {
            if ( iExitCode != ESmumSmsSettingsMenuExit &&
                iExitCode != ESmumSmsSettingsSystemExit )
                {
                // system exit
                iExitCode = ESmumSmsSettingsSystemExit;
                }
            TRAP_IGNORE( ClosingServiceCentresDialogL() );// to be sure of not leaving and exiting
            returnValue = ETrue;
            }
            break;
        case EAknSoftkeyBack:
            {
            iExitCode = ESmumSmsSettingsBack;
            ClosingServiceCentresDialogL();
            returnValue = ETrue;
            }
            break;
        default :
            returnValue = CAknDialog::OkToExitL( aButtonId );
            break;
        }
    SMUMLOGGER_LEAVEFN(" CSmumServiceCentresDialog::OkToExitL");
    return returnValue;
    }

// ----------------------------------------------------
// CSmumServiceCentresDialog::HandleListBoxEventL
//
// ----------------------------------------------------
void CSmumServiceCentresDialog::HandleListBoxEventL(
    CEikListBox* /*aListBox*/, 
    TListBoxEvent aEventType )
    {
    SMUMLOGGER_ENTERFN(" CSmumServiceCentresDialog::HandleListBoxEventL");
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            //for touch input
            if ( iSCAddressesList->Count())
                {
                EditServiceCentreL();
                }
            else
                {
                AddServiceCentreL();
                }            
            break;
        default :
            // panic?
            break;
        }
    SMUMLOGGER_LEAVEFN(" CSmumServiceCentresDialog::HandleListBoxEventL");
    }

// ----------------------------------------------------
// CSmumServiceCentresDialog::ProcessCommandL
//
// ----------------------------------------------------
void CSmumServiceCentresDialog::ProcessCommandL( TInt aCommandId )
    {
    SMUMLOGGER_ENTERFN(" CSmumServiceCentresDialog::ProcessCommandL");
    CAknDialog::ProcessCommandL( aCommandId ) ;
    switch ( aCommandId )
        {
        case ESmsSettingsChoiceMenuEdit:
        case ESmsSettingsChoiceMenuOpen:
        if ( iSCAddressesList->Count())
                {
                EditServiceCentreL();
                }
            else
                {
                AddServiceCentreL();
                }            
            break;
        case ESmsSettingsChoiceMenuNew:
            AddServiceCentreL();
            break;
        case ESmsSettingsChoiceMenuDelete:
            DeleteServiceCentreL();
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
    SMUMLOGGER_LEAVEFN(" CSmumServiceCentresDialog::ProcessCommandL");
    }

// ----------------------------------------------------
// CSmumServiceCentresDialog::OfferKeyEventL
//
// ----------------------------------------------------
TKeyResponse CSmumServiceCentresDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {

    TKeyResponse returnValue = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
   	if( aType == EEventKeyUp )
		{
		UpdateButtonVisibility();
		}
    if ( aType == EEventKey )
        {
        if ( aKeyEvent.iCode == EKeyBackspace 
            && iListBox->CurrentItemIndex() != KErrNotFound )
            {
            ProcessCommandL( ESmsSettingsChoiceMenuDelete );
            returnValue = EKeyWasConsumed;
            }
        }
    
    return returnValue;
    }

// ----------------------------------------------------
// CSmumServiceCentresDialog::PreLayoutDynInitL
//
// ----------------------------------------------------
void CSmumServiceCentresDialog::PreLayoutDynInitL()
    {
    SMUMLOGGER_ENTERFN(" CSmumServiceCentresDialog::PreLayoutDynInitL");
    // Initialising listbox
    iListBox = STATIC_CAST( CEikTextListBox*, Control( ESmsSCListId ));
    iListBox->SetListBoxObserver( this );
    CTextListBoxModel* model=iListBox->Model();
    model->SetItemTextArray( iSCAddressesList );
    model->SetOwnershipType( ELbmDoesNotOwnItemArray );
    
    // Get "no centres"-text from resources and save it to listbox-model
    HBufC* string = StringLoader::LoadLC( R_QTN_MCE_SETTINGS_SMS_NO_SC, iCoeEnv );
    iListBox->View()->SetListEmptyTextL( *string );

    // Setting scrollbar-indicators on
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    // Fetch pointer to titlepane
    CEikStatusPane *sp = ( STATIC_CAST(
        CAknAppUi*,( CEikonEnv::Static()->EikAppUi())))->StatusPane();
    iTitlePane = STATIC_CAST(
        CAknTitlePane*,sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle )));

    // Take old titlepane text safe
    iPreviousTitleText = ( *iTitlePane->Text()).Alloc();
        
    // Read new titletext and set it
    HBufC* text = StringLoader::LoadLC( R_QTN_MCE_SETTINGS_SMS_TITLE_SC, iCoeEnv );
    iTitlePane->SetTextL( *text );
    CleanupStack::PopAndDestroy(2); // text, string
    
    //Check need for MSK visibility
	UpdateButtonVisibility();
    SMUMLOGGER_LEAVEFN(" CSmumServiceCentresDialog::PreLayoutDynInitL");
    }

// ----------------------------------------------------
// CSmumServiceCentresDialog::ClosingServiceCentresDialogL
//
// ----------------------------------------------------
void CSmumServiceCentresDialog::ClosingServiceCentresDialogL() const
    {
    SMUMLOGGER_ENTERFN(" CSmumServiceCentresDialog::ClosingServiceCentresDialogL");
    // Replace title with old title text
    if ( iExitCode == ESmumSmsSettingsBack )
        {
        // Replace title with old title text
        iTitlePane->SetTextL( *iPreviousTitleText );
        }

    // Create sc list
    TInt loop = 0;
    TInt serviceCentres = 0;
    TInt scInUse = 0;
    serviceCentres = iSettings.ServiceCenterCount();
    scInUse = iSettings.DefaultServiceCenter();
    for ( loop = 0; loop < serviceCentres; loop++ )
        {
        iSettings.RemoveServiceCenter(0);
        }

    // Then add centres from the list to SmsSettings
    TInt count = iSCAddressesList->Count();
    for ( loop = 0; loop < count; loop++ )
        {
        iSettings.AddServiceCenterL(
            iSCAddressesList->SCNameInfo(loop).Name(), 
            iSCAddressesList->SCNameInfo(loop).Address());
        }
    
    // Set default SC
    if ( scInUse >= 0 )
        {
        iSettings.SetDefaultServiceCenter( scInUse );
        }
    SMUMLOGGER_LEAVEFN(" CSmumServiceCentresDialog::ClosingServiceCentresDialogL");
    }

// ---------------------------------------------------------
// CSmumServiceCentresDialog::DynInitMenuPaneL
//
// ---------------------------------------------------------
void CSmumServiceCentresDialog::DynInitMenuPaneL(
    TInt aMenuId, 
    CEikMenuPane* aMenuPane)
    {
    SMUMLOGGER_ENTERFN(" CSmumServiceCentresDialog::DynInitMenuPaneL");
    switch ( aMenuId )
        {
        case R_SMS_SERVICECENTRES_CHOICE_MENUPANE:
            if ( !iSCAddressesList->Count() )
                {
                aMenuPane->SetItemDimmed( ESmsSettingsChoiceMenuDelete, ETrue );
                aMenuPane->SetItemDimmed( ESmsSettingsChoiceMenuEdit, ETrue );
                }
            if( iMSKEnabledInPlatform) //if MSK enabled
            	{
            	aMenuPane->SetItemDimmed( ESmsSettingsChoiceMenuEdit, ETrue );
            	}
            else
            	{
            	aMenuPane->SetItemDimmed( ESmsSettingsChoiceMenuOpen, ETrue );
                }
			if ( iSimSCsOnly ) // Only Sim SC's variation is on
                {
                // ESmsSettingsChoiceMenuEdit must be preserved so
                // that user can check the SMSC number
	            aMenuPane->SetItemDimmed( ESmsSettingsChoiceMenuDelete, ETrue );    	
                aMenuPane->SetItemDimmed( ESmsSettingsChoiceMenuNew, ETrue );
                }                
            // Help handling
            aMenuPane->SetItemDimmed( EAknCmdHelp, !iHelpFeatureSupported );
            break;
        default :
            // panic
            break;
        }
    SMUMLOGGER_LEAVEFN(" CSmumServiceCentresDialog::DynInitMenuPaneL");
    }

// ----------------------------------------------------
// CSmumServiceCentresDialog::AddServiceCentreL
//
// ----------------------------------------------------
void CSmumServiceCentresDialog::AddServiceCentreL()
    {
    SMUMLOGGER_ENTERFN(" CSmumServiceCentresDialog::AddServiceCentreL");
    TBuf<KSmumStringLength> name;
    TBuf<KSmumStringLength> number;

    TInt originalSCamount = iSCAddressesList->Count();

    // Launch dialog for adding service centre
    TInt exitCode = ESmumSmsSettingsInitialvalue;
    CSmumAddEditServiceCentreDialog* scItemDialog = 
        new ( ELeave ) CSmumAddEditServiceCentreDialog(
            name, number, exitCode, iHelpFeatureSupported, iSCAddressesList );
    CleanupStack::PushL( scItemDialog );
    scItemDialog->ConstructL( R_SMS_SERVICECENTRE_ITEM_CHOICE_MENUBAR );
    CleanupStack::Pop(); // scItemDialog
    scItemDialog->ExecuteLD( R_ADD_EDIT_SERVICECENTRES );
    
    // Update Sc array & listbox if values ok (in anycase)
    if ( name.Size() && number.Size())
        {
        TSmumSCNameInfo scAddress( name, number );
        iSCAddressesList->AppendL( scAddress );
        iSettings.AddServiceCenterL( name, number );
        }

    if ( exitCode == ESmumSmsSettingsMenuExit ||
         exitCode == ESmumSmsSettingsSystemExit )
        {// Exiting...
        iExitCode = exitCode;
        DoDelayedExitL( 0 );
        }
    else
        {// not exiting, let's take care of drawing
        if ( iSCAddressesList->Count() > originalSCamount )
            { // added service centres
            iListBox->HandleItemAdditionL();
            iListBox->SetCurrentItemIndex( iSCAddressesList->Count() - 1 );
            }
        iListBox->DrawNow();
        }
    SMUMLOGGER_LEAVEFN(" CSmumServiceCentresDialog::AddServiceCentreL");
    }

// ----------------------------------------------------
// CSmumServiceCentresDialog::EditServiceCentreL
//
// ----------------------------------------------------
void CSmumServiceCentresDialog::EditServiceCentreL()
    {
    SMUMLOGGER_ENTERFN(" CSmumServiceCentresDialog::EditServiceCentreL");
    const TInt index = iListBox->CurrentItemIndex();
    __ASSERT_DEBUG( index >= 0 , Panic( ESmsetdlgInvalidIndex ));
    
    TSmumSCNameInfo& tempSCName = iSCAddressesList->SCNameInfo( index );
    
    TBuf<KSmumStringLength> name = tempSCName.Name();
    TBuf<KSmumStringLength> number = tempSCName.Address();
    
    // Launch dialog for editing sc values
    TInt exitCode = ESmumSmsSettingsInitialvalue;
    CSmumAddEditServiceCentreDialog* scItemDialog = 
        new ( ELeave ) CSmumAddEditServiceCentreDialog(
        name, number, exitCode, iHelpFeatureSupported, iSCAddressesList, iSimSCsOnly );
    CleanupStack::PushL( scItemDialog );
    scItemDialog->ConstructL( R_SMS_SERVICECENTRE_ITEM_CHOICE_MENUBAR );
    CleanupStack::Pop(); // scItemDialog
    scItemDialog->ExecuteLD( R_ADD_EDIT_SERVICECENTRES );
    
    // Update scaddresseslist
    if ( name.Size() && number.Size()) 
        {
        // A valid entry
        TSmumSCNameInfo scAddress( name, number );
        iSCAddressesList->InsertL( index, scAddress );
        iSCAddressesList->Delete( index + 1 ); // delete old one            
        }
    else
        {
        // An invalid entry, delete
        iSCAddressesList->Delete( index );
        iListBox->HandleItemRemovalL();
        iSettings.RemoveServiceCenter( index );
        // Handle focus
        TInt numberOfItems = iListBox->Model()->NumberOfItems();
        if ( index + 1 > numberOfItems && index )
            {
            iListBox->SetCurrentItemIndex( index - 1 );
            }
        else if ( index + 1 <= numberOfItems && index )
            {
            iListBox->SetCurrentItemIndex( index );
            }
        }
    
    if ( exitCode == ESmumSmsSettingsMenuExit ||
         exitCode == ESmumSmsSettingsSystemExit )
        {// Exiting...
        iExitCode = exitCode;
        DoDelayedExitL( 0 );
        }
    else
        {// not exiting, let's take care of drawing
        iListBox->SetCurrentItemIndex(index);
        iListBox->HandleItemAdditionL();
        iListBox->DrawNow();
        }
    SMUMLOGGER_LEAVEFN(" CSmumServiceCentresDialog::EditServiceCentreL");
    }

// ----------------------------------------------------
// CSmumServiceCentresDialog::DeleteServiceCentreL
//
// ----------------------------------------------------
void CSmumServiceCentresDialog::DeleteServiceCentreL()
    {
    SMUMLOGGER_ENTERFN(" CSmumServiceCentresDialog::DeleteServiceCentreL");
    TInt index = iListBox->CurrentItemIndex();
    __ASSERT_DEBUG( index >= 0 , Panic( ESmsetdlgInvalidIndex ));

	if ( iSimSCsOnly ) // Only Sim SC's variation is on
	    {
	    // No delete/edit operations are allowed
		SmumUtil::ShowInformationNoteL( R_QTN_SMS_EMAIL_SETT_CANNOT_CHANGE );
		return;
	    }
    // Text for deletion queries
    HBufC* scName = iSCAddressesList->SCNameInfo( index ).Name().AllocLC();
    TInt scDeletionValue;
    HBufC* wholeConfTextInQuery = NULL;
    TInt scInUseIndex = 0; 
    TInt amountOfSCs = 0;
    scInUseIndex = iSettings.DefaultServiceCenter();
    amountOfSCs = iSettings.ServiceCenterCount();
    // Last Service Centre?
    if ( ESmumScLastOne == amountOfSCs )
        {
        wholeConfTextInQuery = StringLoader::LoadLC( R_QTN_MCE_SETTINGS_SMS_QUEST_DEL, iCoeEnv );
        scDeletionValue = ESmumScLastOne;
        }
    // Service Centre in use?
    else if (scInUseIndex == index)
        {
        wholeConfTextInQuery = StringLoader::LoadLC( R_QTN_MCE_SETTINGS_SMS_QUEST_D, iCoeEnv );
        scDeletionValue = ESmumScInUse;
        }
    // Are you sure to delete?
    else
        {
        wholeConfTextInQuery = StringLoader::LoadLC( R_SMUM_DELETE_CONFIRMATION_COMMON, *scName, iCoeEnv );
        scDeletionValue = ESmumScNormalDelete;
        }

    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    // delete sc?
    if ( dlg->ExecuteLD( R_DELETE_SC_QUERY, *wholeConfTextInQuery ))
        { // yes
        // Delete sc from list & sc array
        iSCAddressesList->Delete( index );
        iSettings.RemoveServiceCenter( index );
        amountOfSCs = amountOfSCs - 1;
        iListBox->HandleItemRemovalL();

        switch ( scDeletionValue )
            {
            case ESmumScNormalDelete :
                { // Normal deletion, handle Default SC if necessary
                if ( index < scInUseIndex )
                    {
                    iSettings.SetDefaultServiceCenter( scInUseIndex - 1 );
                    } 
                }
                break;
            case ESmumScInUse :
                { // Sc in use, launch list query for the user to select new one
                if ( amountOfSCs > 1 )
                    {
                    LaunchScInUseListQueryL();
                    }
                }
                break;
            case ESmumScLastOne : 
            default :
                break;
            }

        TInt indexToHighlight = index < iListBox->Model()->NumberOfItems() ?
                                   index : iListBox->Model()->NumberOfItems() - 1;                
        // Set focus after deletion if we have Service Centres
        if ( amountOfSCs >= 1 )
            {
            iListBox->SetCurrentItemIndex(
                  indexToHighlight >= 1 ? indexToHighlight : 0 );
            }
        }
    CleanupStack::PopAndDestroy( 2 ); // scName, wholeConfTextInQuery
    UpdateButtonVisibility();
    iListBox->DrawNow();
    SMUMLOGGER_LEAVEFN(" CSmumServiceCentresDialog::DeleteServiceCentreL - LAST");
    }

// ----------------------------------------------------
// CSmumServiceCentresDialog::LaunchScInUseListQuery
//
// ----------------------------------------------------
void CSmumServiceCentresDialog::LaunchScInUseListQueryL()
    {    
    SMUMLOGGER_ENTERFN(" CSmumServiceCentresDialog::LaunchScInUseListQueryL");
    // Initialise listbox
    CAknSinglePopupMenuStyleListBox* list = 
        new(ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL( list );
    CAknPopupList* popupList = CAknPopupList::NewL( 
        list, R_AVKON_SOFTKEYS_OK_EMPTY );
    CleanupStack::PushL( popupList );	
    list->ConstructL( popupList, CEikListBox::ELeftDownInViewRect );
    // Add scroll indicators
    list->CreateScrollBarFrameL( ETrue );
    list->ScrollBarFrame()->SetScrollBarVisibilityL( 
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    
    // Create array and add recipient's numbers to it
    CDesCArrayFlat* items = new ( ELeave ) CDesCArrayFlat( KSmumSCArray );
    CleanupStack::PushL( items );
    
    TInt amountOfSCs = iSCAddressesList->Count();
    for ( TInt loop = 0 ; loop < amountOfSCs; loop++ )
        {
        items->AppendL( iSCAddressesList->SCNameInfo( loop ).Name());
        }
    
    // Give array to listbox model
    CTextListBoxModel* model = list->Model();
    model->SetItemTextArray( items );
    model->SetOwnershipType(ELbmOwnsItemArray);
    CleanupStack::Pop();// items - listbox model now owns this
    
    // Load txt to show in title
    HBufC* text = StringLoader::LoadLC( R_QTN_MCE_SETTINGS_SMS_LIST_SC, iCoeEnv );
    popupList->SetTitleL( *text );
    CleanupStack::PopAndDestroy(); // text
    
    // Launch popuplist and update choice
    TInt choice = KErrNotFound;
    CleanupStack::Pop();			//popupList
    if ( popupList->ExecuteLD() )
        {
        choice = list->CurrentItemIndex();
        }
    
    CleanupStack::PopAndDestroy();	// list
    
    // Take user's choice to use
    if ( choice != KErrNotFound )
        {
        iSettings.SetDefaultServiceCenter( choice );
        }
    SMUMLOGGER_LEAVEFN(" CSmumServiceCentresDialog::LaunchScInUseListQueryL");
    }

// ----------------------------------------------------
// CSmumServiceCentresDialog::LaunchHelpL
// launch help using context
//
// ----------------------------------------------------
void CSmumServiceCentresDialog::LaunchHelpL()
    {
    if ( iHelpFeatureSupported )
        {
        CCoeAppUi* editorAppUi = STATIC_CAST(CCoeAppUi*, ControlEnv()->AppUi());
        CArrayFix<TCoeHelpContext>* helpContext = editorAppUi->AppHelpContextL();
        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
        }
    }


// ----------------------------------------------------
// CSmumServiceCentresDialog::GetHelpContext
// returns helpcontext as aContext
//
// ----------------------------------------------------
void CSmumServiceCentresDialog::GetHelpContext
        (TCoeHelpContext& aContext) const
    {    
    const TUid KUidMce ={0x100058C5};      
    aContext.iMajor = KUidMce;
    aContext.iContext = KSMS_HLP_SETTINGS_SCENTRES; 
    }
    
// ---------------------------------------------------------
// CSmumServiceCentresDialog::UpdateButtonVisibility
// ---------------------------------------------------------
//
void CSmumServiceCentresDialog::UpdateButtonVisibility()
    {
    if ( ButtonGroupContainer().ControlOrNull( ESmsSettingsChoiceMenuOpen ) )
        {
        //Check need for MSK visibility
	    if( iListBox->CurrentItemIndex() > -1 )
	    	{
	    	ButtonGroupContainer().MakeCommandVisible( ESmsSettingsChoiceMenuOpen, ETrue );
	    	}
	    else
	    	{//no need to show
	    	ButtonGroupContainer().MakeCommandVisible( ESmsSettingsChoiceMenuOpen, EFalse );
	       	}
        }
    }
    
// ---------------------------------------------------------
// CSmumServiceCentresDialog::DoDelayedExitL
// ---------------------------------------------------------
void CSmumServiceCentresDialog::DoDelayedExitL( const TInt aDelay )
    {
    SMUMLOGGER_ENTERFN(" CSmumServiceCentresDialog::DoDelayedExitL");
    delete iIdle;
    iIdle = 0;
    iIdle = CPeriodic::NewL( EPriorityNormal - 1 );
    iIdle->Start(   aDelay,
                    aDelay,
                    TCallBack( DelayedExit, this ) );
    SMUMLOGGER_LEAVEFN(" CSmumServiceCentresDialog::DoDelayedExitL");
    }

// ---------------------------------------------------------
// CSmumServiceCentresDialog::DelayedExit
// ---------------------------------------------------------
TInt CSmumServiceCentresDialog::DelayedExit( TAny* aThis )
    {
    SMUMLOGGER_ENTERFN(" CSmumServiceCentresDialog::DelayedExit");
    CSmumServiceCentresDialog* settings = static_cast<CSmumServiceCentresDialog*>( aThis );
    TRAP_IGNORE( settings->ClosingServiceCentresDialogL() ); // to be sure of not leaving and exiting
    CAknEnv::RunAppShutter( );
    SMUMLOGGER_LEAVEFN(" CSmumServiceCentresDialog::DelayedExit");
    return KErrNone;
    }

    
//  End of File  

