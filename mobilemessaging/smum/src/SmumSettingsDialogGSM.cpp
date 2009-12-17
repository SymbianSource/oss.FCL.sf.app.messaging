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
*     Main settings dialog for Sms.
*
*/



// INCLUDE FILES
#include <SMUM.rsg>                     // resouce identifiers
#include <smutset.h>                    // CSmsSettings
#include <aknappui.h>                   // CEikStatusPane
#include <akntitle.h>                   // CAknTitlePane
#include <aknenv.h>                     // AppShutter
#include <aknnotewrappers.h>            // Avkon note wrappers
#include <AknQueryDialog.h>             // CAknQueryDialog
#include <eikmenup.h>                   // CEikMenuPane
#include <smuthdr.h>                    // CSmsHeader
#include <StringLoader.h>               // StringLoader
#include <AknDlgShut.h>                 // AknDialogShutter
#include <hlplch.h>                     // HlpLauncher
#include <featmgr.h>                    // FeatureManager
#include <bldvariant.hrh>
#include <aknnavi.h>                    //CAknNavigationControlContainer
#include <csxhelp/mce.hlp.hrh>
#include <csxhelp/sms.hlp.hrh>
#include <messagingvariant.hrh>
#include <RCustomerServiceProfileCache.h>
#include <centralrepository.h>          // CRepository
#include <messaginginternalcrkeys.h>    // Keys

// locals
#include "smsui.pan"    				// for panics
#include "SmumSettingsDialogGSM.h"         // CSmumMainSettingsDialogGSM
#include "SmumServiceCentreItemDialog.h"// CSmumAddEditServiceCentreDialog
#include "SmumEmailServiceCentreItemDialog.h"// CSmumEmailServiceCentreDialog
#include "SMSU.HRH"                     // resource header
#include "SmumServiceCentresDialog.h"   // CSmumServiceCentresDialog
#include "SmumUtil.h"

#include "SmumLogging.h"

// CONSTANTS
const TInt KMuiuSettingsArrayGranularity    = 4;
const TInt KSmumCharSettingDeduction = 2;
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CSmumMainSettingsDialogGSM::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CSmumMainSettingsDialog* CSmumMainSettingsDialogGSM::NewL(
    CSmsSettings&   aSettings, 
    TInt            aTypeOfSettings,
    TInt&           aExitCode,
    TInt&           aCharSetSupportForSendingOptions,
    CSmsHeader*     aSmsHeader )
    {
    SMUMLOGGER_ENTERFN(" CSmumMainSettingsDialogGSM::NewL");
    CSmumMainSettingsDialogGSM*  self = new ( ELeave ) CSmumMainSettingsDialogGSM( 
        aSettings, aTypeOfSettings, aExitCode, 
        aCharSetSupportForSendingOptions, aSmsHeader );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    SMUMLOGGER_LEAVEFN(" CSmumMainSettingsDialogGSM::NewL");
    return self;
    }
    
// -----------------------------------------------------------------------------
// CSmumMainSettingsDialogGSM::CSmumMainSettingsDialogGSM
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CSmumMainSettingsDialogGSM::CSmumMainSettingsDialogGSM(
    CSmsSettings&   aSettings, 
    TInt            aTypeOfSettings,
    TInt&           aExitCode,
    TInt&           aCharSetSupportForSendingOptions,
    CSmsHeader*     aSmsHeader )
    :   iSettings       ( aSettings ),
        iTypeOfSettings ( aTypeOfSettings ),
        iExitCode       ( aExitCode ),
        iSmsHeader      ( aSmsHeader ),
        iHelpFeatureSupported ( EFalse ),
        iCSPFeatures ( NULL ),
        iCharacterSupport ( aCharSetSupportForSendingOptions ),
        iIdle( NULL ),
        iUseSimSCsOnly( EFalse ),
        iEmailFeatureSupported( EFalse ),
        iMskId( NULL )        
    {
    }

// Destructor
CSmumMainSettingsDialogGSM::~CSmumMainSettingsDialogGSM()
    {
    SMUMLOGGER_ENTERFN(" CSmumMainSettingsDialogGSM::~CSmumMainSettingsDialogGSM");
    if ( iListBox )
        {
        iListBox->SetScrollBarFrame( NULL, CEikListBox::EOwnedExternally );
        }
    delete iPreviousTitleText;
    delete iSettingsArrayIndex;
    delete iCentralRapository;
    delete iIdle;
    // iSettingsArray, iListBox, iSettings(reference), iSettingsForReadingSCList,
    // iTitlePane, iPreviousTitleText, iSmumNaviPane are
    // plain pointers to objects owned by other classes which take
    // care also about deletion.
    // Remove default navi pane
    if ( iSmumNaviPane )
        {
        iSmumNaviPane->Pop();
        }
    // Not owned. For Lint
    iSettingsArray = NULL;
    iListBox = NULL;
    iSmsHeader = NULL;
    iTitlePane = NULL;
    iSmumNaviPane = NULL;
    SMUMLOGGER_LEAVEFN(" CSmumMainSettingsDialogGSM::~CSmumMainSettingsDialogGSM");
    }

// -----------------------------------------------------------------------------
// CSmumMainSettingsDialogGSM::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CSmumMainSettingsDialogGSM::ConstructL()
    {
    SMUMLOGGER_ENTERFN(" CSmumMainSettingsDialogGSM::ConstructL");
    CAknDialog::ConstructL( R_MAIN_SMS_SETTINGS_CHOICE_MENUBAR );

    // Prepare FeatureManager, read values to members and uninitialize FeatureManager
    FeatureManager::InitializeLibL();
    iHelpFeatureSupported = FeatureManager::FeatureSupported( KFeatureIdHelp );
    FeatureManager::UnInitializeLib();
    // Read the supported settings
    iEmailFeatureSupported = SmumUtil::CheckEmailOverSmsSupportL();
    ReadCspSupportBitsL();
    iCentralRapository = CRepository::NewL( KCRUidSmum );
    iCentralRapository->Get( KSmumRemoveReplyViaSameCentre, iRemoveReplyScSetting );
    // Check if Only Sim SC's variation is on
	if ( SmumUtil::CheckVariationFlagsL( KCRUidMuiuVariation, KMuiuSmsFeatures ) & 
		 KSmsFeatureIdSimServiceCentresOnly )
		{
		iUseSimSCsOnly = ETrue;
		}
    SMUMLOGGER_LEAVEFN(" CSmumMainSettingsDialogGSM::ConstructL");
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogGSM::OkToExitL
// 
// ----------------------------------------------------
TInt CSmumMainSettingsDialogGSM::OkToExitL( TInt aButtonId )
    {
    SMUMLOGGER_ENTERFN(" CSmumMainSettingsDialogGSM::OkToExitL");
    TInt returnValue;

    switch( aButtonId )
        {
        case ESmsSettingsChoiceMenuOpen:
        case ESmsSettingsChoiceMenuChange:
        case ESmsSettingsChoiceMenuChangeMSK:
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
            TRAP_IGNORE( ClosingDialogL() ); // to be sure of not leaving and exiting
            returnValue = ETrue;
            }
            break;
        // back-key
        case EAknSoftkeyBack:
            {
            iExitCode = ESmumSmsSettingsBack;
            ClosingDialogL();
            returnValue = ETrue;
            }
            break;
        default :
            returnValue = CAknDialog::OkToExitL( aButtonId );
            break;
        }

    SMUMLOGGER_LEAVEFN(" CSmumMainSettingsDialogGSM::OkToExitL");
    return returnValue;
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogGSM::HandleListBoxEventL
//
// ----------------------------------------------------
void CSmumMainSettingsDialogGSM::HandleListBoxEventL(
    CEikListBox* /*aListBox*/, 
    TListBoxEvent aEventType )
    {
    SMUMLOGGER_ENTERFN(" CSmumMainSettingsDialogGSM::HandleListBoxEventL");
    switch( aEventType )
        {
        case EEventItemDraggingActioned:
            UpdateMskL();
            break;            
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            CheckOpeningListBoxItemL( ETrue );
            break;
        default :
            // nothing
            break;
        }
    SMUMLOGGER_LEAVEFN(" CSmumMainSettingsDialogGSM::HandleListBoxEventL");
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogGSM::ProcessCommandL
//
// ----------------------------------------------------
void CSmumMainSettingsDialogGSM::ProcessCommandL(
    TInt aCommandId)
    {
    SMUMLOGGER_ENTERFN(" CSmumMainSettingsDialogGSM::ProcessCommandL");
    CAknDialog::ProcessCommandL( aCommandId ) ;
    switch ( aCommandId )
        {
        case ESmsSettingsChoiceMenuOpen:
        case ESmsSettingsChoiceMenuChange:
            CheckOpeningListBoxItemL( EFalse );
            break;
        case ESmsSettingsChoiceMenuChangeMSK:
            CheckOpeningListBoxItemL( ETrue );
            break;
        case EAknCmdHelp:
            {
            LaunchHelpL();                
            }
            break;
        case ESmsSettingsChoiceMenuExit:
            { 
            iExitCode = ESmumSmsSettingsMenuExit;
            TryExitL( EAknSoftkeyCancel );
            }
            break;
        default :
            // nothing
            break ;
        }
    SMUMLOGGER_LEAVEFN(" CSmumMainSettingsDialogGSM::ProcessCommandL");
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogGSM::OfferKeyEventL
//
// ----------------------------------------------------
TKeyResponse CSmumMainSettingsDialogGSM::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {

    TKeyResponse returnValue = CAknDialog::OfferKeyEventL( aKeyEvent, aType );
	if( aType == EEventKeyUp )
	{
	UpdateMskL( );
        }

    return returnValue;
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogGSM::PreLayoutDynInitL
//
// ----------------------------------------------------
void CSmumMainSettingsDialogGSM::PreLayoutDynInitL()
    {
    SMUMLOGGER_ENTERFN(" CSmumMainSettingsDialogGSM::PreLayoutDynInitL");
    // Creating correct listbox depending Settings vs Sending Options 
    iListBox = STATIC_CAST( CEikTextListBox*, Control( ESmsSettingsListBoxId ));
    iSettingsArray = CSmumSettingsArray::NewL( iTypeOfSettings ? 
        R_SMS_SENDING_OPTIONS_ITEMS : R_SMS_MAIN_SETTINGS_ITEMS );
    
    // The count of setting items depends of setting type
    TInt numberOfSettingItems = iTypeOfSettings ? 
        ESmumSendOptReplyViaSameCentreLBI + 1 : ESmumReplyViaSameCentreLBI + 1;
    iSettingsArrayIndex = new(ELeave) CArrayFixFlat<TInt>( numberOfSettingItems );
	// Iterate through settings
	for (TInt count = 0; count < numberOfSettingItems; count++)
		{
        iSettingsArrayIndex->AppendL( count ); // Indicates that setting is present
		}
    DeleteVariatedSettings();

    iListBox->SetListBoxObserver( this );

    // Arabic-indic conversion not to be done to SMSC Name
    if ( !iTypeOfSettings ) // For main settings
        {
        (*iSettingsArray)[ESmumServiceCentreInUseLBI].iLangSpecificNumConv = EFalse;
        }
    else
        {
        (*iSettingsArray)[ESmumSendOptServiceCentreInUseLBI].iLangSpecificNumConv = EFalse;
        }

    // Initialize listboxes to settings values
    OpeningDialogL();

    // scroll bars
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray( iSettingsArray );

	UpdateMskL();
    SMUMLOGGER_LEAVEFN(" CSmumMainSettingsDialogGSM::PreLayoutDynInitL");
    }

// ---------------------------------------------------------
// CSmumMainSettingsDialogGSM::DynInitMenuPaneL
//
// ---------------------------------------------------------
void CSmumMainSettingsDialogGSM::DynInitMenuPaneL(
    TInt aMenuId, 
    CEikMenuPane* aMenuPane )
    {
    SMUMLOGGER_ENTERFN(" CSmumMainSettingsDialogGSM::DynInitMenuPaneL");
    switch ( aMenuId )
        {
        case R_MAIN_SMS_SETTINGS_CHOICE_MENUPANE:
            // Check which one to show in Options-menu
            if ( ESmumServiceCentresLBI == iListBox->CurrentItemIndex() 
                && !iTypeOfSettings )
                {
                // Open option is left in the menu even if no SIM service
                // centres available and use only SIM service centres is in use.
                // If user selects it, he gets note:
                // No service centres, SMS service has been disabled
                /*
                if ( iUseSimSCsOnly && iSettings.ServiceCenterCount() == 0 )
                	{
                	aMenuPane->DeleteMenuItem( ESmsSettingsChoiceMenuOpen );
                	}
                */	
                aMenuPane->DeleteMenuItem( ESmsSettingsChoiceMenuChange );
                }
            else
                {
                aMenuPane->DeleteMenuItem( ESmsSettingsChoiceMenuOpen );
                }

            // Help handling
            aMenuPane->SetItemDimmed( EAknCmdHelp,!iHelpFeatureSupported );

            break;
        default :
            // panic
            break;
        }
    SMUMLOGGER_LEAVEFN(" CSmumMainSettingsDialogGSM::DynInitMenuPaneL");
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogGSM::UpdateSCInUseArrayL
//
// ----------------------------------------------------
void CSmumMainSettingsDialogGSM::UpdateSCInUseArrayL()
    {
    SMUMLOGGER_ENTERFN(" CSmumMainSettingsDialogGSM::UpdateSCInUseArrayL");
    // Create an sc array
    CDesCArrayFlat* array = 
        new ( ELeave ) CDesCArrayFlat( KMuiuSettingsArrayGranularity );
    TBool arrayOwnershipChanged( EFalse );
    CleanupStack::PushL( array );
    TInt serviceCentres = 0;
    serviceCentres = iSettings.ServiceCenterCount();
    const TInt listboxIndex = iTypeOfSettings ? 
        ESmumSendOptServiceCentreInUseLBI : ESmumServiceCentreInUseLBI;
    
    if( serviceCentres )
        {
        // We have service centres
        for ( TInt loop = 0; loop < serviceCentres; loop++ )
            {
            array->AppendL( iSettings.GetServiceCenter( loop ).Name());
            }
        // Delete old and replace array
        delete iSettingsArray->At( listboxIndex ).iMuiuSettingsItemArray;
        iSettingsArray->At( listboxIndex ).iMuiuSettingsItemArray = array;
        // Set the one in use visible
        SetItem( listboxIndex, iSettings.DefaultServiceCenter() );
        arrayOwnershipChanged = ETrue;
        }
    else
        {
        if ( !iUseSimSCsOnly )
        	{
	        // No service centres defined
	        // Read value from resources
	        HBufC* txt = StringLoader::LoadLC( R_QTN_SELEC_SETT_VAL_FIELD_NONE, iCoeEnv );
	        array->AppendL( *txt );
	        // Delete old and replace array
	        delete iSettingsArray->At( listboxIndex ).iMuiuSettingsItemArray;
	        iSettingsArray->At( listboxIndex ).iMuiuSettingsItemArray = array;
	        // Confirm listbox value 
	        SetItem( listboxIndex, serviceCentres );
	        CleanupStack::PopAndDestroy(); // txt	
	        arrayOwnershipChanged = ETrue;
        	}
        }
    if ( arrayOwnershipChanged )
    	{
	    CleanupStack::Pop(); //array
    	}
    else
    	{
    	CleanupStack::PopAndDestroy(); //array
    	}
    SMUMLOGGER_LEAVEFN(" CSmumMainSettingsDialogGSM::UpdateSCInUseArrayL");
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogGSM::OpeningDialogL
//
// ----------------------------------------------------
void CSmumMainSettingsDialogGSM::OpeningDialogL()
    {
    SMUMLOGGER_ENTERFN(" CSmumMainSettingsDialogGSM::OpeningDialogL");
    // Fetch pointer to titlepane
    CEikStatusPane *sp =  ( STATIC_CAST(
        CAknAppUi*, ( CEikonEnv::Static()->EikAppUi())))->StatusPane();
    iTitlePane = STATIC_CAST (
        CAknTitlePane*, sp->ControlL( TUid::Uid(EEikStatusPaneUidTitle)));
    
    // Take old titlepane text safe
    iPreviousTitleText = ( *iTitlePane->Text()).Alloc();
        
    // Read new titletext and set it
    HBufC* text = StringLoader::LoadLC( iTypeOfSettings ?
        R_QTN_SMS_TITLE_SENDING : R_QTN_MCE_TITLE_SETTINGS_SMS, iCoeEnv );
    iTitlePane->SetTextL( *text );
    CleanupStack::PopAndDestroy(); // text

    // replace navi pane with empty one.
    iSmumNaviPane = static_cast<CAknNavigationControlContainer*>
        ( iEikonEnv->AppUiFactory()->StatusPane()->ControlL(
            TUid::Uid(EEikStatusPaneUidNavi) ) );
    if ( iSmumNaviPane )
        {
        iSmumNaviPane->PushDefaultL();
        }

    // Settings :
    // Service Centre in Use
    // Sending options?
    TBool isReplyMessage = EFalse;
    
    if(iTypeOfSettings)
    	{
    	isReplyMessage = iSmsHeader->Submit().ReplyPath();
    	}
    
    if ( iTypeOfSettings == 1 && iCSPFeatures & EReplyPathSupport ) // 1 = sending options
        {
        // Replypath provided?
        if ( isReplyMessage && !iRemoveReplyScSetting)
            { // yes, it's provided
            iSettingsArray->SetReplyPath( ETrue );
            }
        }
    // Update arrays
    if(!(iRemoveReplyScSetting && isReplyMessage))
    	{	
    UpdateSCInUseArrayL();
    	}
	
	TInt index;
    TKeyArrayFix key(0, ECmpTInt);    
    // Character Support 3.0 onwards
    if ( iTypeOfSettings ) // Sending options
        {
        if ( !iSettingsArrayIndex->Find ( ESmumSendOptCharSupportInUseLBI, key, index ) )
        	{
	        TInt temp = iCharacterSupport;
	        // Check if the value is modifiable
	        // and modify setting value if needed
	        if ( ECharSetFullLocked==temp || 
	             ECharSetReducedLocked==temp )
	            {
	            temp -= KSmumCharSettingDeduction;
	            }
	        SetItem( index, temp );
        	}
        }
    else
        {
        if ( !iSettingsArrayIndex->Find ( ESmumCharSupportInUseLBI, key, index ) )
        	{
        	SetItem( index, ReadCharSetSupport() );
        	}
        }

    if ( iCSPFeatures & EDeliveryReportSupport ) // Set value only if setting is supported
        {
        if ( !iSettingsArrayIndex->Find (iTypeOfSettings ? 
                ESmumSendOptDeliveryReportLBI : ESmumDeliveryReportLBI, key, index ) )
            {
            // Delivery Report
            SetItem( index, iSettings.DeliveryReport() ? 
                ESmumSettingsYes : ESmumSettingsNo );
            }
        }
    // Validity Period
    TInt choicelistIndex = KErrNotFound;
    if ( iCSPFeatures & EValidityPeriodSupport )
        {
        if ( !iSettingsArrayIndex->Find ( iTypeOfSettings ? 
                ESmumSendOptValidityPeriodLBI : ESmumValidityPeriodLBI, key, index ) )
            {
            switch( iSettings.ValidityPeriod().Int())
                {
                case ESmsVPHour:
                    choicelistIndex = ESmumValidityPeriodHour;
                    break;
                case ESmsVPSixHours:
                    choicelistIndex = ESmumValidityPeriodSixHours;
                    break;
                case ESmsVP24Hours:
                    choicelistIndex = ESmumValidityPeriod24Hours;
                    break;
                case (3 * (TInt) ESmsVP24Hours): 
                    choicelistIndex = ESmumValidityPeriod3Days;
                    break;
                case ESmsVPWeek:
                    choicelistIndex = ESmumValidityPeriodWeek;
                    break;
                case ESmsVPMaximum:
                    choicelistIndex = ESmumValidityPeriodMaximum;
                    break;
                default:
        #if defined (_DEBUG)
                    Panic( ESmsetdlgUnknownValidity );
        #else
                // drop through in release version
                case ESmsVPNotSupported:
                    choicelistIndex = ESmumValidityPeriodMaximum;
        #endif
                    break;
                }
            SetItem( index, choicelistIndex);
            }
        }
    
    // Message conversion
    if ( iCSPFeatures & EProtocolIDSupport )
        {
        if ( !iSettingsArrayIndex->Find ( iTypeOfSettings ? 
                ESmumSendOptMessageConversionLBI : ESmumMessageConversionLBI, key, index ) )
            {
            // Message Conversion
            switch(iSettings.MessageConversion())
                {
                case ESmsConvPIDNone:
                    choicelistIndex = ESmumConversionNone;
                    break;
                case ESmsConvFax:
                    choicelistIndex = ESmumConversionFax;
                    break;        
                case ESmsConvPaging:
                    choicelistIndex = ESmumConversionPaging;
                    break;
                case ESmsConvX400:
                case ESmsConvErmes:
                case ESmsConvSpeech:
                default:
        #if defined (_DEBUG)
                Panic( ESmsetdlgUnknownConversion );
        #else
                // drop through in release version
                choicelistIndex = ESmumConversionNone;
        #endif
                    break;            
                }
            SetItem( index, choicelistIndex);
            }
        }

    // Preferred Connection
    if ( !iTypeOfSettings )
        {
        if ( !iSettingsArrayIndex->Find ( ESmumPreferredConnectionLBI, key, index ) )
            {
            const TInt preferredConnection = iSettings.SmsBearer();
            switch( preferredConnection )
                {// "prefer GPRS"
                case CSmsSettings::ESmsBearerPacketPreferred :
                    SetItem( index, ESmumPreferGPRSConnection );
                    break;
                // In all other cases "prefer GSM"
                //case RMobileSmsMessaging::ESmsBearerCircuitPreferred :
                default: 
                //case RMobileSmsMessaging::ESmsBearerCircuitOnly :
                //case RMobileSmsMessaging::ESmsBearerPacketOnly :
                    SetItem( index, ESmumPreferGSMConnection );
                    break;
                }
            }
        }
    
    // Reply Path
    if ( iCSPFeatures & EReplyPathSupport && !iRemoveReplyScSetting)
        {
        // Reply Via Same Centre
        if ( !iSettingsArrayIndex->Find ( iTypeOfSettings ? 
                ESmumSendOptReplyViaSameCentreLBI : ESmumReplyViaSameCentreLBI, key, index ) )
            {
            SetItem( index, iSettings.ReplyPath() ? ESmumSettingsYes : ESmumSettingsNo );
            }
        }
    SMUMLOGGER_LEAVEFN(" CSmumMainSettingsDialogGSM::OpeningDialogL");
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogGSM::ClosingDialogL
//
// ----------------------------------------------------
void CSmumMainSettingsDialogGSM::ClosingDialogL()
    {
    SMUMLOGGER_ENTERFN(" CSmumMainSettingsDialogGSM::ClosingDialogL");
    TBool replyMessage = EFalse;
    
    if ( iTypeOfSettings == 1 && iCSPFeatures & EReplyPathSupport ) // 1 = sending options
        {
        // Replypath provided?
        
        if ( iSmsHeader->Submit().ReplyPath() && iRemoveReplyScSetting)
            { // yes, it's provided
            replyMessage = ETrue;
            iSettingsArray->SetReplyPath( ETrue );
            }
        }
    // Replace title with old title text
    if ( iExitCode == ESmumSmsSettingsBack )
        {
        // Replace title with old title text
        iTitlePane->SetTextL( *iPreviousTitleText );
        }
    TInt index;
    TKeyArrayFix key(0, ECmpTInt);
    // Settings :
    // Service Centre in Use
    if ( iSettings.ServiceCenterCount())
        {
        if(!replyMessage && !iRemoveReplyScSetting)
        	{
        	//Make sure array is updated if user presses end-key
        	UpdateSCInUseArrayL();
        	
            iSettings.SetDefaultServiceCenter( Item(
                iTypeOfSettings ? 
                ESmumSendOptServiceCentreInUseLBI : ESmumServiceCentreInUseLBI ));
        	}
        }
    // Character Support 3.0 onwards
    if ( !iSettingsArrayIndex->Find ( iTypeOfSettings ? 
            ESmumSendOptCharSupportInUseLBI : ESmumCharSupportInUseLBI, key, index ) )
    	{
    	TInt setVal = Item( index );
	    // Take actions only if setting is not locked,
	    // not ECharSetFullLocked or ECharSetReducedLocked  
	    if ( setVal != iCharacterSupport &&
	         iCharacterSupport < ECharSetFullLocked )
	        {
	        if ( iTypeOfSettings ) // Sending option
	            {
	            iCharacterSupport = setVal;
	            }
	        else // Main setting, save to CenRep
	            {
	            iCentralRapository->Set( KSmumCharSupport, setVal );
	            }
	        }
    	}
    // Delivery Report
    if ( iCSPFeatures & EDeliveryReportSupport )
        {
        if ( !iSettingsArrayIndex->Find ( iTypeOfSettings ? 
            ESmumSendOptDeliveryReportLBI : ESmumDeliveryReportLBI, key, index ) )
            {
            iSettings.SetDeliveryReport( Item( index ) == ESmumSettingsYes );
            }
        }
    
    // Validity Period    
    if ( iCSPFeatures & EValidityPeriodSupport )
        {
        if ( !iSettingsArrayIndex->Find ( iTypeOfSettings ? 
            ESmumSendOptValidityPeriodLBI : ESmumValidityPeriodLBI, key, index ) )
            {
            TTimeIntervalMinutes validityPeriod = iSettings.ValidityPeriod().Int();
            switch( Item( index ) )
                {
                case ESmumValidityPeriodHour:
                    validityPeriod = ( TInt ) ESmsVPHour;
                    break;
                case ESmumValidityPeriodSixHours:
                    validityPeriod = ( TInt ) ESmsVPSixHours;
                    break;
                case ESmumValidityPeriod24Hours:
                    validityPeriod = ( TInt ) ESmsVP24Hours;
                    break;
                case ESmumValidityPeriod3Days:   
                    validityPeriod = ( 3 * ( TInt ) ESmsVP24Hours );// Instead of modifying smutset.h
                    break;
                case ESmumValidityPeriodWeek:
                    validityPeriod = ( TInt ) ESmsVPWeek;
                    break;
                case ESmumValidityPeriodMaximum:
                    validityPeriod = ( TInt ) ESmsVPMaximum;
                    break;
                default:
        #if defined ( _DEBUG )
                    Panic( ESmsetdlgUnknownValidityChoice );
        #else
                    validityPeriod = ( TInt ) ESmsVP24Hours;
        #endif
                    break;
                }
            iSettings.SetValidityPeriod( validityPeriod );
            }
        }

    // Message Conversion
    if ( iCSPFeatures & EProtocolIDSupport )
        {
        if ( !iSettingsArrayIndex->Find ( iTypeOfSettings ? 
            ESmumSendOptMessageConversionLBI : ESmumMessageConversionLBI, key, index ) )
            {
            TInt choicelistIndex = KErrNotFound;
            switch( Item( index ))
                {
                case ESmumConversionNone:
                    choicelistIndex = ESmsConvPIDNone;
                    break;
                case ESmumConversionFax:
                    choicelistIndex = ESmsConvFax;
                    break;        
                case ESmumConversionPaging:
                    choicelistIndex = ESmsConvPaging;
                    break;
                default:
        #if defined ( _DEBUG )
                Panic( ESmsetdlgUnknownConversion );
        #else
                // drop through in release version
                choicelistIndex = ESmsConvPIDNone;
        #endif
                    break;            
                }
            iSettings.SetMessageConversion(STATIC_CAST(
                TSmsPIDConversion, 
                choicelistIndex ));
            }
        }

    // Preferred Connection
    if ( !iTypeOfSettings )
        {
        if ( !iSettingsArrayIndex->Find ( ESmumPreferredConnectionLBI, key, index ) )
            {
            switch( Item( index ))
                {
                // "prefer GPRS"
                case ESmumPreferGPRSConnection :
                    iSettings.SetSmsBearer( CSmsSettings::ESmsBearerPacketPreferred );
                    break;
                // "prefer GSM"
                // case ESmumPreferGSMConnection :
                default :
                    iSettings.SetSmsBearer( CSmsSettings::ESmsBearerCircuitPreferred );
                    break;
                }
            }
        }

    // Reply Via Same Centre
    if ( iCSPFeatures & EReplyPathSupport && !iRemoveReplyScSetting )
        {
        if ( !iSettingsArrayIndex->Find ( iTypeOfSettings ? 
            ESmumSendOptReplyViaSameCentreLBI : ESmumReplyViaSameCentreLBI, key, index ) )
            {
            iSettings.SetReplyPath( Item( index ) == ESmumSettingsYes );
            }
        }
    SMUMLOGGER_LEAVEFN(" CSmumMainSettingsDialogGSM::ClosingDialogL");
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogGSM::SetItem
//
// ----------------------------------------------------
void CSmumMainSettingsDialogGSM::SetItem( TInt aSettingLBIndex, TInt aValue )
    {
    iSettingsArray->At( aSettingLBIndex ).iUserText.Copy(
        ( *iSettingsArray->At( aSettingLBIndex ).iMuiuSettingsItemArray )[aValue]);
    ( *iSettingsArray )[ aSettingLBIndex ].iCurrentNumber = aValue;
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogGSM::Item
//
// ----------------------------------------------------
TInt CSmumMainSettingsDialogGSM::Item( TInt aSettingLBIndex ) const
    {
    return iSettingsArray->At( aSettingLBIndex ).iCurrentNumber;
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogGSM::CheckOpeningListBoxItemL
//
// ----------------------------------------------------
void CSmumMainSettingsDialogGSM::CheckOpeningListBoxItemL( TBool aEnterPressed )
    {
    SMUMLOGGER_ENTERFN(" CSmumMainSettingsDialogGSM::CheckOpeningListBoxItemL");
    // First get the selected index
    TInt variatedSelectionIndex = GetVariatedSelectionIndex( iListBox->CurrentItemIndex() );
    __ASSERT_DEBUG( variatedSelectionIndex >= 0 , Panic( ESmsetdlgInvalidIndex ));
    // Sending options
    if ( iTypeOfSettings )
        {
        // Focus in "Service centre in use & no SC'es defined
        if ( ESmumSendOptServiceCentreInUseLBI == variatedSelectionIndex && 
            iSettings.ServiceCenterCount() == 0 )
            {
            if ( iUseSimSCsOnly ) // Only Sim SC's variation is on
	            { // No centres -> No sms sending
	            SmumUtil::ShowInformationNoteL( R_QTN_MCE_SETTINGS_SMS_SERVICE_DISABLED );	
	            }
	        else
		        {
		        SmumUtil::ShowInformationNoteL( R_QTN_SMS_SETTINGS_INFO_NO_SC );
		        }
            }
        // Focus in "Service centre in use & replypath provided
        else if ( ESmumSendOptServiceCentreInUseLBI == variatedSelectionIndex
                && iSettingsArray->ReplyPathProvided())
            {
            CAknQueryDialog* dlg = CAknQueryDialog::NewL();
            if ( dlg->ExecuteLD( R_OVERRIDE_PROVIDED_REPLYPATH_QUERY ))
                {
                if (iSettingsArray->EditItemL( variatedSelectionIndex, EFalse ))// EFalse forcing opening
                    {
                    iSettingsArray->SetReplyPath( EFalse );
                    iSmsHeader->Message().SetServiceCenterAddressL( KNullDesC );
                    iListBox->DrawNow();
                    }
                }
            }
        // Focus in "Service centre in use
        else if ( ESmumSendOptServiceCentreInUseLBI == variatedSelectionIndex )
            {
            iSettingsArray->EditItemL(
                iListBox->CurrentItemIndex(), 
                EFalse );// EFalse forcing opening
            iListBox->DrawNow();
            }
        // Simple listbox item
        else if ( ESmumSendOptCharSupportInUseLBI <= variatedSelectionIndex )
            {
            if ( variatedSelectionIndex == ESmumSendOptCharSupportInUseLBI )
                {
                // Check is the editing allowed
                if ( iCharacterSupport == ECharSetFullLocked || 
                     iCharacterSupport == ECharSetReducedLocked )
                    {
                    // Display note and exit
                    SmumUtil::ShowInformationNoteL( R_QTN_MCE_INFO_CANNOT_CHANGE_SMS_CHARS );
                    // return immediatelly
                    return;
                    }
                }
            iSettingsArray->EditItemL(
                iListBox->CurrentItemIndex(), 
                aEnterPressed );
            iListBox->DrawNow();
            }
        else
            {// Lint
            }
        }
    // Sms Settings 
    else
        {
        TBool goingToExit = EFalse;

        // Check if focus in two first ones and non sc defined
        if ( ESmumServiceCentreInUseLBI >= variatedSelectionIndex && 
            iSettings.ServiceCenterCount() == 0 )
            {
            //Save the used menu item to later use
            iStoreSettingsIndex = iListBox->CurrentItemIndex();
            if ( iUseSimSCsOnly ) // Only Sim SC's variation is on
	            { // No centres -> No sms sending
	            SmumUtil::ShowInformationNoteL( R_QTN_MCE_SETTINGS_SMS_SERVICE_DISABLED );	
	            }
	        else
		        {
	            // Run "create new sc?" -query
	            CAknQueryDialog* dlg = CAknQueryDialog::NewL();
	            if ( dlg->ExecuteLD( R_CREATE_NEW_SC_QUERY ))
	                {
	                TBuf<KSmumStringLength> name;
	                TBuf<KSmumStringLength> number;
	               
	                // Add SC dialog
	                TInt addSCDlgExitCode = ESmumSmsSettingsInitialvalue;
	                CSmumAddEditServiceCentreDialog* scItemDialog = 
	                    new ( ELeave ) CSmumAddEditServiceCentreDialog(
	                        name, number, addSCDlgExitCode, iHelpFeatureSupported );
	                CleanupStack::PushL( scItemDialog );
	                scItemDialog->ConstructL( R_SMS_SERVICECENTRE_ITEM_CHOICE_MENUBAR );
	                CleanupStack::Pop();// scItemDialog
	                scItemDialog->ExecuteLD( R_ADD_EDIT_SERVICECENTRES );
	                if ( addSCDlgExitCode == ESmumSmsSettingsMenuExit ||
	                     addSCDlgExitCode == ESmumSmsSettingsSystemExit )
	                    { // Exiting
	                    goingToExit = ETrue;
	                    }
	                                    
	                // Update sc array and launch service centres-dialog
	                // if values ok, otherwise reset the listbox.
	                if ( name.Size() && number.Size()) 
	                    {
	                    iSettings.AddServiceCenterL( name, number );
	                    if ( goingToExit )
	                        { // Exiting
	                        iExitCode = addSCDlgExitCode;
	                        DoDelayedExitL( 0 );
	                        return;
	                        }
	                    TInt centresDlgExitCode = LaunchServiceCentresDialogL();
	                    if ( centresDlgExitCode == ESmumSmsSettingsMenuExit ||
	                         centresDlgExitCode == ESmumSmsSettingsSystemExit )
	                        { // Exiting
	                        iExitCode = centresDlgExitCode;
	                        DoDelayedExitL( 0 );
	                        return;
	                        }
	                    if( !iRemoveReplyScSetting )   
	                    	{
	                    	UpdateSCInUseArrayL();
	                    	}
	                    iListBox->DrawNow();
	                    }
	                else
	                    {
	                    if ( goingToExit )
	                        {// Exiting
	                        iExitCode = addSCDlgExitCode;
	                        DoDelayedExitL( 0 );
	                        return;
	                        }
	                    else
	                        {
	                        iListBox->Reset();
	                        //Set the correct item in previous menu
	                        iListBox->SetCurrentItemIndexAndDraw(iStoreSettingsIndex);
	                        }
	                    }
	                }
		        }
            }
        // Check if focus in "service centres"
        else if ( ESmumServiceCentresLBI == variatedSelectionIndex )
            {
            TInt centresDlgExitCode = LaunchServiceCentresDialogL();
            if ( centresDlgExitCode == ESmumSmsSettingsMenuExit ||
                 centresDlgExitCode == ESmumSmsSettingsSystemExit )
                { // Exiting
                iExitCode = centresDlgExitCode;
                DoDelayedExitL( 0 );
                return;
                }
            if( !iRemoveReplyScSetting )   
            	{
            	UpdateSCInUseArrayL();
            	}
            iListBox->DrawNow();
            }
        // Check if focus in "Service centre in use"
        else if ( ESmumServiceCentreInUseLBI == variatedSelectionIndex )
            {
            iSettingsArray->EditItemL(
                iListBox->CurrentItemIndex(), 
                EFalse ); // EFalse forcing opening
            // Save default Sc
            iSettings.SetDefaultServiceCenter( Item(
                iTypeOfSettings ? 
                ESmumSendOptServiceCentreInUseLBI : ESmumServiceCentreInUseLBI ));
            iListBox->DrawNow();
            }
		// EmailOverSms
        else if ( iEmailFeatureSupported &&
                    ESmumEmailServiceCentreLBI == variatedSelectionIndex )
        	{
            DisplayEmailOverSmsSettingsL();
            }
        // Focus must be then elsewhere
        else if ( ESmumCharSupportInUseLBI <= variatedSelectionIndex )
            {
            if ( variatedSelectionIndex == ESmumCharSupportInUseLBI )
                {
                // Check is the editing allowed
                if ( iCharacterSupport == ECharSetFullLocked || 
                     iCharacterSupport == ECharSetReducedLocked )
                    {
                    // Display note and exit
                    SmumUtil::ShowInformationNoteL( R_QTN_MCE_INFO_CANNOT_CHANGE_SMS_CHARS );
                    return;
                    }
                }
            // Simple listbox item
            iSettingsArray->EditItemL(
                iListBox->CurrentItemIndex(), 
                aEnterPressed);
            iListBox->DrawNow();
            }
        else
            {// Lint
            }
        }
    SMUMLOGGER_LEAVEFN(" CSmumMainSettingsDialogGSM::CheckOpeningListBoxItemL - LAST");
    // panic
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogGSM::LaunchServiceCentresDialogL
//
// ----------------------------------------------------
TInt CSmumMainSettingsDialogGSM::LaunchServiceCentresDialogL()
    {
    SMUMLOGGER_ENTERFN(" CSmumMainSettingsDialogGSM::LaunchServiceCentresDialogL");
    TInt exitCode = ESmumSmsSettingsInitialvalue;
    // Launch Service Centres dialog
    CSmumServiceCentresDialog* servicecentreDialog = 
        CSmumServiceCentresDialog::NewL(
        iSettings, exitCode, R_SMS_SERVICECENTRES_CHOICE_MENUBAR, iHelpFeatureSupported, iUseSimSCsOnly );
    servicecentreDialog->ExecuteLD( R_SERVICECENTRELIST_DIALOG );
    SMUMLOGGER_LEAVEFN(" CSmumMainSettingsDialogGSM::LaunchServiceCentresDialogL");
    return exitCode;
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogGSM::LaunchHelpL
// launch help using context
// 
// ----------------------------------------------------
void CSmumMainSettingsDialogGSM::LaunchHelpL()
    {    
    if ( iHelpFeatureSupported )
        {
        CCoeAppUi* editorAppUi = STATIC_CAST(CCoeAppUi*, ControlEnv()->AppUi());
        CArrayFix<TCoeHelpContext>* helpContext = editorAppUi->AppHelpContextL();   
        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
        }
    }


// ----------------------------------------------------
// CSmumMainSettingsDialogGSM::GetHelpContext
// returns helpcontext as aContext
// 
// ----------------------------------------------------
void CSmumMainSettingsDialogGSM::GetHelpContext
        (TCoeHelpContext& aContext) const
    {    
    const TUid KUidMce ={0x100058C5};      
    aContext.iMajor = KUidMce;
    aContext.iContext = KMCE_HLP_SETTINGS_SMS;
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogGSM::GetVariatedSelectionIndex
//
// ---------------------------------------------------------------------------
TInt CSmumMainSettingsDialogGSM::GetVariatedSelectionIndex( 
        const TInt aListSelection ) const
    {
	TInt variatedValue ( aListSelection );
	TBool isScInUseRemoved( EFalse );
	if ( iUseSimSCsOnly && iSettings.ServiceCenterCount() == 0 )
        {
        if ( iTypeOfSettings ? 
        	 aListSelection >= ESmumSendOptServiceCentreInUseLBI : 
        	 aListSelection >= ESmumServiceCentreInUseLBI )
            {
            variatedValue += 1; // removed
            isScInUseRemoved = ETrue;
            }
        }
    if ( !iEmailFeatureSupported ) // EmailFeature on
        {
        // Modifies only main settings
        if ( !iTypeOfSettings && isScInUseRemoved ?
        	 aListSelection >= ( ESmumEmailServiceCentreLBI - 1 ) : 
        	 aListSelection >= ESmumEmailServiceCentreLBI )
            {
            variatedValue += 1; // ESmumEmailServiceCentreLBI is removed
            }
        else if ( iRemoveReplyScSetting )
        	{
        	if ( iTypeOfSettings ? 
        	 aListSelection >= ESmumSendOptServiceCentreInUseLBI : 
        	 aListSelection >= ESmumServiceCentreInUseLBI )
	            {
	            variatedValue += 2; // ESmumEmailServiceCentreLBI
	            }					// ServiceCentreInUseLBI are removed
            }
        }
    return variatedValue;    
    }

// ---------------------------------------------------------------------------
// CSmumMainSettingsDialogGSM::DeleteVariatedSettings
// 
// ---------------------------------------------------------------------------
void CSmumMainSettingsDialogGSM::DeleteVariatedSettings()
    {
    SMUMLOGGER_ENTERFN("CSmumMainSettingsDialogGSM::DeleteVariatedSettings")
    SMUMLOGGER_WRITE_FORMAT(
        "DeleteVariatedSettings - Type of Settings :%d",
        iTypeOfSettings )
    TBool needToBeCompressed = EFalse;
        
    if ( !( iCSPFeatures & EReplyPathSupport ) || iRemoveReplyScSetting )
        {
        SMUMLOGGER_WRITE(
            "DeleteVariatedSettings - EReplyPathSupport")
        DeleteSettingItemFromArrays( iTypeOfSettings ? 
            ESmumSendOptReplyViaSameCentreLBI : ESmumReplyViaSameCentreLBI );
        needToBeCompressed = ETrue;
        }
    if ( !( iCSPFeatures & EProtocolIDSupport ))
        {
        SMUMLOGGER_WRITE(
            "DeleteVariatedSettings - EProtocolIDSupport")
        DeleteSettingItemFromArrays( iTypeOfSettings ? 
                ESmumSendOptMessageConversionLBI : ESmumMessageConversionLBI );
        needToBeCompressed = ETrue;
        }
    if ( !( iCSPFeatures & EValidityPeriodSupport ))
        {
        SMUMLOGGER_WRITE(
            "DeleteVariatedSettings - EValidityPeriodSupport")
        DeleteSettingItemFromArrays( iTypeOfSettings ? 
                ESmumSendOptValidityPeriodLBI : ESmumValidityPeriodLBI );
        needToBeCompressed = ETrue;
        }
    if ( !( iCSPFeatures & EDeliveryReportSupport ))
        {
        SMUMLOGGER_WRITE(
            "DeleteVariatedSettings - EDeliveryReportSupport")
        DeleteSettingItemFromArrays( iTypeOfSettings ? 
                ESmumSendOptDeliveryReportLBI : ESmumDeliveryReportLBI );
        needToBeCompressed = ETrue;
        }
	// Main settings and no email support
    if ( !iEmailFeatureSupported && !iTypeOfSettings )
        {
        SMUMLOGGER_WRITE(
            "DeleteVariatedSettings - EmailFeatureSupport")
        DeleteSettingItemFromArrays( ESmumEmailServiceCentreLBI );
        needToBeCompressed = ETrue;
        }        
    TBool isReplyMessage = EFalse; 
    if(iTypeOfSettings)
    	{
    	isReplyMessage = iSmsHeader->Submit().ReplyPath();
    	}
	if ( iUseSimSCsOnly && iSettings.ServiceCenterCount() == 0 || 
		(iRemoveReplyScSetting && isReplyMessage ))
        {
        DeleteSettingItemFromArrays(iTypeOfSettings ? 
        	 ESmumSendOptServiceCentreInUseLBI : ESmumServiceCentreInUseLBI );
        needToBeCompressed = ETrue;
        }
    if ( needToBeCompressed )
        {
        iSettingsArray->Compress();
        iSettingsArrayIndex->Compress();
        }
    SMUMLOGGER_LEAVEFN("CSmumMainSettingsDialogGSM::DeleteVariatedSettings")
    }

// ---------------------------------------------------------------------------
// CSmumMainSettingsDialogGSM::DeleteSettingItemFromArrays
// 
// ---------------------------------------------------------------------------
void CSmumMainSettingsDialogGSM::DeleteSettingItemFromArrays( TInt aIndex )
    {
    SMUMLOGGER_ENTERFN("CSmumMainSettingsDialogGSM::DeleteSettingItemFromArrays")
    // Delete from CMuiuSettingsArray
    delete iSettingsArray->At( aIndex ).iMuiuSettingsItemArray;
    iSettingsArray->Delete( aIndex );
    // Delete form index array
    iSettingsArrayIndex->Delete( aIndex );
    SMUMLOGGER_WRITE_FORMAT(
        "DeleteSettingItemFromArrays - Delete Setting :%d",
        aIndex )
    SMUMLOGGER_LEAVEFN("CSmumMainSettingsDialogGSM::DeleteSettingItemFromArrays")
    }

// ---------------------------------------------------------------------------
// CSmumMainSettingsDialogGSM::ReadCspSupportBitsL
// 
// ---------------------------------------------------------------------------
void CSmumMainSettingsDialogGSM::ReadCspSupportBitsL()
    {
    SMUMLOGGER_ENTERFN("CSmumMainSettingsDialogGSM::ReadCspSupportBitsL")
    // Initialize as all features supported
    iCSPFeatures =  EValidityPeriodSupport
                    |EProtocolIDSupport
                    |EDeliveryReportSupport
                    |EReplyPathSupport;
    // Bit values changed only if feature is supported
    TInt tmpInt( KErrNotFound );

    tmpInt = SmumUtil::CheckVariationFlagsL( KCRUidMuiuVariation, KMuiuMceFeatures );
    // Read from bits from sim cache
    if ( tmpInt & KMceFeatureIdCSPSupport ) 
        {
        SMUMLOGGER_WRITE(
            "ReadCspSupportBitsL - KMceFeatureIdCSPSupport supported" )
        RCustomerServiceProfileCache* cspProfile = 
            new (ELeave) RCustomerServiceProfileCache;    
        __ASSERT_DEBUG( cspProfile, Panic( ESmsuNullPointer ) );
        TInt error = cspProfile->Open();
        if( KErrNone == error )
            {
            // Get tele services flags from CSP
            RMobilePhone::TCspTeleservices params;
            // If not supported (-5) or any other error
            // we assume all settings are supported
            error = cspProfile->CspTeleServices( params );
            if ( KErrNone == error ) 
                {
                SMUMLOGGER_WRITE_FORMAT(
                    "ReadCspSupportBitsL - RMobilePhone::TCspTeleservices #%d", params )
                if( 0 == ( params&RMobilePhone::KCspValidityPeriod ) ) 
                    {
                    SMUMLOGGER_WRITE(
                        "ReadCspSupportBitsL - KCspValidityPeriod not supported" )
                    iCSPFeatures &= ~EValidityPeriodSupport;
                    }
                if( 0 == ( params&RMobilePhone::KCspProtocolID ) ) 
                    {
                    SMUMLOGGER_WRITE(
                        "ReadCspSupportBitsL - KCspProtocolID not supported" )
                    iCSPFeatures &= ~EProtocolIDSupport;
                    }
                if( 0 == ( params&RMobilePhone::KCspDelConf ) ) 
                    {
                    SMUMLOGGER_WRITE(
                        "ReadCspSupportBitsL - KCspDelConf not supported" )
                    iCSPFeatures &= ~EDeliveryReportSupport;
                    }
                if( 0 == ( params&RMobilePhone::KCspReplyPath ) ) 
                    {
                    SMUMLOGGER_WRITE(
                        "ReadCspSupportBitsL - KCspReplyPath not supported" )
                    iCSPFeatures &= ~EReplyPathSupport;
                    }
                }
            else
                {
                SMUMLOGGER_WRITE_FORMAT(
                    "ReadCspSupportBitsL - cspProfile->CspTeleServices() #%d", error )
                }
            cspProfile->Close();
            }
        else
            {
            SMUMLOGGER_WRITE_FORMAT(
                    "ReadCspSupportBitsL - cspProfile->Open() #%d", error )
            }
        delete cspProfile;    
        }
    SMUMLOGGER_LEAVEFN("CSmumMainSettingsDialogGSM::ReadCspSupportBitsL")
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogGSM::ReadCharSetSupport
// reads the setting value and sets it to member variable
// 
// ----------------------------------------------------
TInt CSmumMainSettingsDialogGSM::ReadCharSetSupport()
    {
    SMUMLOGGER_ENTERFN(" CSmumMainSettingsDialogGSM::ReadCharSetSupport");
    __ASSERT_DEBUG( iCentralRapository, Panic( ESmsuNullPointer ));
    TInt tempCharSupport( ECharSetReduced );
    if ( iCentralRapository )
        {
        if ( KErrNone == iCentralRapository->Get( 
                KSmumCharSupport, tempCharSupport ) )
            {
            // Sanity check for the read value ( accepted values between 0-3 )
            if ( ECharSetReducedLocked<tempCharSupport || ECharSetFull>tempCharSupport )
                {
                // Set the member and the return value to Reduced mode
                tempCharSupport = iCharacterSupport = ECharSetReduced;
                }
            else
                {
                // Use in member as it was read
                iCharacterSupport = tempCharSupport;
                // But we may have to modify the return value ...
                // The return value is used as setting item index
                // and it must be 0(ECharSetFull) or 1(ECharSetReduced)
                // 2 -> 0, 3 -> 1
                if ( ECharSetFullLocked==tempCharSupport || 
                     ECharSetReducedLocked==tempCharSupport )
                    {
                    tempCharSupport -= KSmumCharSettingDeduction;
                    }
                }
            __ASSERT_DEBUG( tempCharSupport==ECharSetFull ||
                            tempCharSupport==ECharSetReduced, 
                            Panic( ESmsetdlgInvalidIndex ));
            }
        }
    SMUMLOGGER_WRITE_FORMAT(" CSmumMainSettingsDialogGSM::ReadCharSetSupport - %d", tempCharSupport);
    SMUMLOGGER_LEAVEFN(" CSmumMainSettingsDialogGSM::ReadCharSetSupport");
    return tempCharSupport;
    }

// ---------------------------------------------------------------------------
// CSmumMainSettingsDialogGSM::DisplayEmailOverSmsSettingsL
// 
// ---------------------------------------------------------------------------
void CSmumMainSettingsDialogGSM::DisplayEmailOverSmsSettingsL()
    {
    SMUMLOGGER_ENTERFN(" CSmumMainSettingsDialogGSM::DisplayEmailOverSmsSettingsL");
    // Add SC dialog
    TBuf<KSmumStringLength> gateway;
    TBuf<KSmumStringLength> sc;
    TBool modifiable = ETrue;
    // Read current values
    TRAP_IGNORE( SmumUtil::ReadEmailOverSmsSettingsL( 
        sc, 
        gateway, 
        modifiable ) );

    TInt addEmailSettingsDlgExitCode = ESmumSmsSettingsInitialvalue;
    CSmumEmailServiceCentreDialog* scItemDialog = 
        new ( ELeave ) CSmumEmailServiceCentreDialog(
            gateway, 
            sc,
            modifiable, 
            addEmailSettingsDlgExitCode, 
            iHelpFeatureSupported );

    CleanupStack::PushL( scItemDialog );
    scItemDialog->ConstructL( R_SMS_SERVICECENTRE_ITEM_CHOICE_MENUBAR );
    CleanupStack::Pop();// scItemDialog
    scItemDialog->ExecuteLD( R_ADD_EDIT_SERVICECENTRES );
    if ( addEmailSettingsDlgExitCode == ESmumSmsSettingsSystemExit )
        { // Exiting
        iExitCode = ESmumSmsSettingsMenuExit;
        DoDelayedExitL( 0 );
        return;
        }
    else // ESmumSmsSettingsBack, ESmumSmsSettingsMenuExit
        {
        // Save settings if modifiable
        if ( modifiable && gateway.Length() && sc.Length() )
            {
            TRAP_IGNORE( SmumUtil::WriteEmailOverSmsSettingsL( 
                sc, 
                gateway, 
                modifiable ) );
            }
        // Settings ok and saved - Exit messaging
		if ( addEmailSettingsDlgExitCode == ESmumSmsSettingsMenuExit )
            {
            iExitCode = ESmumSmsSettingsMenuExit;
            DoDelayedExitL( 0 );
            return;
            }
        }
    }

// ---------------------------------------------------------
// CSmumMainSettingsDialogGSM::HandleResourceChange
//
// ---------------------------------------------------------
void CSmumMainSettingsDialogGSM::HandleResourceChange( TInt aType )
    {
    CEikDialog::HandleResourceChange( aType );
    }
// ---------------------------------------------------------
// CSmumMainSettingsDialogGSM::UpdateMskL
//
// ---------------------------------------------------------
void CSmumMainSettingsDialogGSM::UpdateMskL( )
	{
	TInt resId = 0;
	TInt index = iListBox->CurrentItemIndex( );
	
	if ( index == KErrNotFound )
	    return;
	
	if( index == 0 )
		{ // Message type
		resId = R_SMUM_MSK_BUTTON_OPEN;
		}
	else
		{ // Char set
		resId = R_SMUM_MSK_BUTTON_CHANGE;
		}
		
    if (resId != iMskId )
        {
        CEikButtonGroupContainer& cba = ButtonGroupContainer( );
        const TInt KMskPosition = 3;
        cba.SetCommandL( KMskPosition, resId );
        cba.DrawDeferred( );    
        iMskId = resId;        
        }
    
	}
	
// ---------------------------------------------------------
// CSmumMainSettingsDialogGSM::DoDelayedExitL
// ---------------------------------------------------------
void CSmumMainSettingsDialogGSM::DoDelayedExitL( const TInt aDelay )
    {
    SMUMLOGGER_ENTERFN(" CSmumMainSettingsDialogGSM::DoDelayedExitL");
    delete iIdle;
    iIdle = 0;
    iIdle = CPeriodic::NewL( EPriorityNormal - 1 );
    iIdle->Start(   aDelay,
                    aDelay,
                    TCallBack( DelayedExit, this ) );
    }

// ---------------------------------------------------------
// CSmumMainSettingsDialogGSM::DelayedExit
// ---------------------------------------------------------
TInt CSmumMainSettingsDialogGSM::DelayedExit( TAny* aThis )
    {
    SMUMLOGGER_ENTERFN(" CSmumMainSettingsDialogGSM::DelayedExit");
    CSmumMainSettingsDialogGSM* settings = static_cast<CSmumMainSettingsDialogGSM*>( aThis );
    TRAP_IGNORE( settings->ClosingDialogL() ); // to be sure of not leaving and exiting
    CAknEnv::RunAppShutter( );
    return KErrNone;
    }
//  End of File  

