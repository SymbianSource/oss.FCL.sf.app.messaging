/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*     Main settings dialog for Sms CDMA
*
*/



// INCLUDE FILES
#include <smum.rsg>                     // resouce identifiers

// New Symbian Includes for CDMA SMS
#include <smutset.h>
#include <smuthdr.h>
#include <tia637.h>
using namespace tia637;

#include <smsMessageSettings.h>
#include <smsmessagewrappers.h>

#include <aknappui.h>                   // CEikStatusPane
#include <akntitle.h>                   // CAknTitlePane
#include <aknnotewrappers.h>            // Avkon note wrappers
#include <aknquerydialog.h>             // CAknQueryDialog
#include <eikmenup.h>                   // CEikMenuPane
#include <smuthdr.h>                    // CSmsHeader
#include <csmsaccount.h>                // CSmsAccount
#include <MsvOffPeakTime.h>             // CMsvOffPeakTimes
#include <MsvScheduleSettings.h>        // Schedule Settings
#include <MsvSysAgentAction.h>          // CMsvSysAgentActions
#include <PsVariables.h>                // PS Variables 
#include <SaCls.h>
#include <StringLoader.h>               // StringLoader
#include <akndlgshut.h>                 // AknDialogShutter
#include <MuiuMsvUiServiceUtilities.h>  // DiskSpaceBelowCriticalLevelL
#include <hlplch.h>                     // HlpLauncher
#include <FeatMgr.h>                    // FeatureManager
#include <bldvariant.hrh>
#include <eikedwin.h>
#include <AknSettingPage.h>
#include <AknTextSettingPage.h>
#include <AknRadioButtonSettingPage.h>
#include <AknPasswordSettingPage.h>
#include <CPbkContactEngine.h>              // Phonebook Engine
#include <CPbkContactItem.h>                // Phonebook Contact
#include <CPbkSingleEntryFetchDlg.h>        // Phonebook Dialog
#include <RPbkViewResourceFile.h>           // Phonebook Resource
#include <CPbkSmsAddressSelect.h>           // Phonebook SMS select number
#include <csxhelp/mce.hlp.hrh>
#include <csxhelp/sms.hlp.hrh>
// locals
#include "smsui.pan"                    // for panics
#include "SmumSettingsDialogCDMA.h"         // CSmumMainSettingsDialog
#include "SmumServiceCentreItemDialog.h"// CSmumAddEditServiceCentreDialog
#include "smsu.hrh"                     // resource header
#include "Smsetdlgcdma.h"
#include <aknsettingpage.h>
#include <aknphonenumbereditor.h>

// for reading the own number from NAM engine for callback number (not applicable in WINS environment)
//#include <TASetCdmaUtility.h>   // RTASettings
//#include <TASetCore.h>      // CTASetCore
// CONSTANTS
const TInt KDefMaxRetries = 1;
const TInt KMsgQError = -31003;

// Default callback number length
const TInt KDefCallbackNumberLength = 17;

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CSmumMainSettingsDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CSmumMainSettingsDialog* CSmumMainSettingsDialogCDMA::NewL(
    CSmsSettings&   aSettings, 
    TInt            aTypeOfSettings,
    TInt&           aExitCode,
    CSmsHeader*     aSmsHeader )
    {
    CSmumMainSettingsDialogCDMA*  self = new ( ELeave ) CSmumMainSettingsDialogCDMA( 
        aSettings, aTypeOfSettings, aExitCode, aSmsHeader );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CSmumMainSettingsDialogCDMA::CSmumMainSettingsDialogCDMA
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CSmumMainSettingsDialogCDMA::CSmumMainSettingsDialogCDMA(
    CSmsSettings&   aSettings, 
    TInt            aTypeOfSettings,
    TInt&           aExitCode,
    CSmsHeader*     aSmsHeader )
    :   iSettings       ( aSettings ),
        iTypeOfSettings ( aTypeOfSettings ),
        iExitCode       ( aExitCode ),
        iSmsHeader      ( (CSmsHeader*)aSmsHeader ),
        iLaunchDialog   ( EFalse ),
        iHelpFeatureSupported ( EFalse ),
        iPhonebookResource( *iCoeEnv ),
        iMaxCallbackLength(KDefCallbackNumberLength),
        iDeliveryAckSupport(EFalse)
    {
    }
    
// Destructor
CSmumMainSettingsDialogCDMA::~CSmumMainSettingsDialogCDMA()
    {
    if ( iListBox )
        {
        iListBox->SetScrollBarFrame( NULL, CEikListBox::EOwnedExternally );
        }
    delete iPreviousTitleText;
    delete iNoCallbackNumber;
    delete iPbkEngine;
    delete iSmsAccount;
    iPhonebookResource.Close();
    FeatureManager::UnInitializeLib();
    // iSettingsArray, iListBox, iSettings(reference), iSettingsForReadingSCList,
    // iTitlePane, iPreviousTitleText are
    // plain pointers to objects owned by other classes which take
    // care also about deletion.
    }

// -----------------------------------------------------------------------------
// CSmumMainSettingsDialogCDMA::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CSmumMainSettingsDialogCDMA::ConstructL()
    {
    CAknDialog::ConstructL( R_MAIN_SMS_SETTINGS_CHOICE_MENUBAR );

    // Prepare FeatureManager, read values to members and uninitialize FeatureManager
    FeatureManager::InitializeLibL();
    iHelpFeatureSupported = FeatureManager::FeatureSupported( KFeatureIdHelp );
    iNoCallbackNumber = StringLoader::LoadL( R_QTN_MCE_SETTING_SMS_CB_NONE, iCoeEnv );

    // Open contact 
    iPbkEngine = CPbkContactEngine::NewL();
    iPhonebookResource.OpenL();

    // Message Queuing Settings
    //
    iSmsAccount = CSmsAccount::NewL();
    
    // Create the session
    CRepository* cenRepSession = CRepository::NewLC(KCRUidSmum); 
    TInt readSetting;

    // Get max Recipient Phone/Callback number length 
    if ( cenRepSession->Get( KSmumMaxRecipientPhoneNumberLength, readSetting ) != KErrNone )
        {
        readSetting = KDefCallbackNumberLength;
        }
    iMaxCallbackLength = readSetting;
		
    // Get the delivery report flag
    if ( cenRepSession->Get( KSmumDefDelReport, readSetting ) != KErrNone )
        {
        readSetting = EFalse;
        }
    iDeliveryAckSupport = readSetting;
    CleanupStack::PopAndDestroy(cenRepSession);
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogCDMA::OkToExitL
// 
// ----------------------------------------------------
TInt CSmumMainSettingsDialogCDMA::OkToExitL( TInt aButtonId )
    {
    TBool returnValue;

    switch( aButtonId )
        {
        // system / menu exit
        case EAknSoftkeyCancel: 
            {
            if ( iExitCode != ESmumSmsSettingsMenuExit &&
                 iExitCode != ESmumSmsSettingsSystemExit )
                {
                // system exit
                iExitCode = ESmumSmsSettingsSystemExit;
                }
            TRAPD( error, ClosingDialogL()); // to be sure of not leaving and exiting
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
        // For opening next dialog
        case EAknSoftkeyShow:
            {
            CheckOpeningListBoxItemL( ETrue );
            returnValue = EFalse;
            }
            break;
        default :
            returnValue = CAknDialog::OkToExitL( aButtonId );
            break;
        }

    return returnValue;
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogCDMA::HandleListBoxEventL
//
// ----------------------------------------------------
void CSmumMainSettingsDialogCDMA::HandleListBoxEventL(
    CEikListBox* /*aListBox*/, 
    TListBoxEvent aEventType )
    {
    switch( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            // This has to be made this way, if you want to launch new dialogs
            // inside the topmost one.
            // 1. Instead of launching them straightly in HandleListBoxEventL, set a flag.
            // 2. Then in OfferKeyEventL check the flag, if it is on then call
            //    TryExitL with some specific value or flag (I used EAknShowKey).
            // 3. This comes to OkToExitL and here you check that are we launching something
            //    and launch if we are.
            iLaunchDialog = ETrue; 
            break;
        default :
            // nothing
            break;
        }
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogCDMA::ProcessCommandL
//
// ----------------------------------------------------
void CSmumMainSettingsDialogCDMA::ProcessCommandL(
    TInt aCommandId)
    {
    CAknDialog::ProcessCommandL( aCommandId ) ;
    switch ( aCommandId )
        {
        case ESmsSettingsChoiceMenuOpen:
        case ESmsSettingsChoiceMenuChange:
            CheckOpeningListBoxItemL( EFalse );
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
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogCDMA::OfferKeyEventL
//
// ----------------------------------------------------
TKeyResponse CSmumMainSettingsDialogCDMA::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {

    TKeyResponse returnValue = CAknDialog::OfferKeyEventL( aKeyEvent, aType );

    if ( iLaunchDialog && aType == EEventKey )
        {
        TryExitL( EAknSoftkeyShow );
        iLaunchDialog = EFalse;
        }

    return returnValue;
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogCDMA::PreLayoutDynInitL
//
// ----------------------------------------------------
void CSmumMainSettingsDialogCDMA::PreLayoutDynInitL()
    {
    // Creating correct listbox depending Settings vs Sending Options 
    //
    iListBox = static_cast<CEikTextListBox*>(Control( ESmsSettingsListBoxId ));
    
    // Do we support delivery ack or not
    //
    if( iDeliveryAckSupport ) 
        {
        iSettingsArray = CSmumSettingsArray::NewL( iTypeOfSettings ? 
            R_SMS_SENDING_OPTIONS_ITEMS_CDMA : R_SMS_MAIN_SETTINGS_ITEMS_CDMA );
        }
    else 
        {
        iSettingsArray = CSmumSettingsArray::NewL( iTypeOfSettings ? 
            R_SMS_SENDING_OPTIONS_ITEMS_CDMA_NO_DELIVERY : R_SMS_MAIN_SETTINGS_ITEMS_CDMA_NO_DELIVERY );
        }
    iListBox->SetListBoxObserver( this );

    // Arabic-indic conversion not to be done to SMSC Name
    //
    (*iSettingsArray)[1].iLangSpecificNumConv = EFalse;

    // Initialize listboxes to settings values
    //
    OpeningDialogL();

    // Scroll bars
    //
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray( iSettingsArray );
    }

// ---------------------------------------------------------
// CSmumMainSettingsDialogCDMA::DynInitMenuPaneL
//
// ---------------------------------------------------------
void CSmumMainSettingsDialogCDMA::DynInitMenuPaneL(
    TInt aMenuId, 
    CEikMenuPane* aMenuPane )
    {
    switch ( aMenuId )
        {
        case R_MAIN_SMS_SETTINGS_CHOICE_MENUPANE:
            //
            // When we construct SMS settings' options menu (iTypeOfSettings
            // being 0 represents SMS settings), we will display "Change"
            // option, instead of "Open" option for all CDMA SMS settings:
            // Callback, Delivery Report, and Priority.
            //
            aMenuPane->SetItemDimmed( ESmsSettingsChoiceMenuOpen, ETrue );
            // Help handling
            aMenuPane->SetItemDimmed( EAknCmdHelp,!iHelpFeatureSupported );

            break;
        default :
            // panic
            break;
        }
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogCDMA::UpdateSCInUseArrayL
//
// ----------------------------------------------------
void CSmumMainSettingsDialogCDMA::UpdateSCInUseArrayL()
    {
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogCDMA::OpeningDialogL
//
// ----------------------------------------------------
void CSmumMainSettingsDialogCDMA::OpeningDialogL()
    {
    // Fetch pointer to titlepane
    //
    CEikStatusPane *sp = static_cast<CAknAppUi*>(CEikonEnv::Static()->EikAppUi())->StatusPane();
    iTitlePane = static_cast <CAknTitlePane*>(sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle )));
    
    // Take old titlepane text safe
    //
    iPreviousTitleText = ( *iTitlePane->Text() ).Alloc();
        
    // Read new titletext and set it
    //
    HBufC* text = StringLoader::LoadLC( iTypeOfSettings ?
        R_QTN_SMS_TITLE_SENDING : R_QTN_MCE_TITLE_SETTINGS_SMS, iCoeEnv );
    iTitlePane->SetTextL( *text );
    CleanupStack::PopAndDestroy(); // text

    TInt choicelistIndex=0;

    // Callback Number
    //
    TPtrC cbNumber = iSettings.MessageSettings().Cdma().CallBackNumber();
    if ( cbNumber.Size() > 0 )
	    {
	    iSettingsArray->SetCallBackNumberL( cbNumber );
	    }
    else 
	  	{
	    iSettingsArray->SetCallBackNumberL( iNoCallbackNumber->Des() );
	    }
    
    // Priority
    //
    TPriorityIndicator pIndicator;
    pIndicator = iSettings.MessageSettings().Cdma().Priority();
    
    switch( pIndicator )
        {
	    case tia637::KBdNormal:
	        choicelistIndex  = ESmumPriorityNormal;
	        break;
	    case tia637::KBdUrgent:
	        choicelistIndex  = ESmumPriorityUrgent;
	        break;
	    default:
	        choicelistIndex  = ESmumPriorityNormal;
	        break;
        }
    SetItem( iTypeOfSettings ?
             ESmumSendOptPriorityLevelLBICDMA : ESmumPriorityLevelLBICDMA, choicelistIndex);

    // Message Queuing
    // We only have Message queuing in the global options
    // Checks iTypeOfSettings to make sure we only do this in the main menu
    //
    
    // @todo Migrate to central repository
    //
    if( !iTypeOfSettings ) 
        {
        if ( IsMsgQueuingOnL() )
            {
            choicelistIndex = ESmumQueuingOn;
            }
        else
            {
            choicelistIndex = ESmumQueuingOff;
            }
        SetItem( ESmumMessageQueuingLBICDMA , choicelistIndex );
        }

    // Delivery ACK
    //
    if( iDeliveryAckSupport ) 
        {
        TBool deliveryAck = iSettings.MessageSettings().ReplyOptions().DeliveryAck();
        if (!deliveryAck)
            {
            choicelistIndex = ESmumSettingsNo;
            }
        else
            {
            choicelistIndex = ESmumSettingsYes;
            }
        SetItem( iTypeOfSettings ?
                 ESmumCdmaSendOptDeliveryReportLBICDMA : ESmumDeliveryReportLBICDMA, choicelistIndex);
        }
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogCDMA::ClosingDialogL
//
// ----------------------------------------------------
void CSmumMainSettingsDialogCDMA::ClosingDialogL() const
    {    
    // Replace title with old title text
    //
    if ( iExitCode == ESmumSmsSettingsBack )
        {
        // Replace title with old title text
        iTitlePane->SetTextL( *iPreviousTitleText );
        }
    
    // Delivery ACK
    // Transport ACK - enabled, Delivery/User/Read ACK - disabled initially
    TSmsReplyOptions reply(ETrue, EFalse, EFalse, EFalse);
    if( iDeliveryAckSupport ) 
        {
        TInt item = Item( iTypeOfSettings ? ESmumCdmaSendOptDeliveryReportLBICDMA : ESmumDeliveryReportLBICDMA );
        // Set Delivert ACK
        if( item == ESmumSettingsYes )
	        {
		   	// Set delivery ACK ON
		   	reply.SetDeliveryAck(ETrue);
		    }
	    else
	   		{
	    	// Set delivery ACK OFF
	    	reply.SetDeliveryAck(EFalse);
	    	}
	    }
	iSettings.MessageSettings().SetReplyOptions( reply );
	    
	// PRIORITY Account setting or Message setting
	//
	TPriorityIndicator priority;
    if ( !iTypeOfSettings ) // account setting
        {
        switch( Item( ESmumPriorityLevelLBICDMA ))
            {
	        case ESmumPriorityNormal:
	            priority = tia637::KBdNormal;	  
	            break;
	        case ESmumPriorityUrgent:
	            priority = tia637::KBdUrgent;
	            break;
	        default:
	            priority = tia637::KBdNormal;	
	            break;
            }
        }
    else    // sending option
        {
        switch( Item( ESmumSendOptPriorityLevelLBICDMA ))
            {
	        case ESmumPriorityNormal:
	            priority = tia637::KBdNormal;	  
	            break;
	        case ESmumPriorityUrgent:
	            priority = tia637::KBdUrgent;
	            break;
	        default:
	            priority = tia637::KBdNormal;	
	            break;
            }
        } 
	iSettings.MessageSettings().Cdma().SetPriority( priority );   
    
    // Callback number
    //
    HBufC* pNumber = HBufC::NewLC(iMaxCallbackLength);
    TPtr phoneNumber = pNumber->Des();
    phoneNumber.Copy(iSettingsArray->CallBackNumber());

    if (0 == ( phoneNumber.Compare( iNoCallbackNumber->Des() )))
        {
        phoneNumber.Zero();
        }
    iSettings.MessageSettings().Cdma().SetCallBackNumberL( *pNumber );

    CleanupStack::PopAndDestroy( pNumber );
	
    // Message queuing
    //
    if( !iTypeOfSettings )
        {
        TInt item = Item( ESmumMessageQueuingLBICDMA );
        if( item == ESmumSettingsYes )
        	{
        	SetMsgQueuingOnL( ETrue );
        	}
        else
        	{
        	SetMsgQueuingOnL( EFalse );
        	}
        }
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogCDMA::SetItem
//
// ----------------------------------------------------
void CSmumMainSettingsDialogCDMA::SetItem( TInt aSettingLBIndex, TInt aValue )
    {
    iSettingsArray->At( aSettingLBIndex ).iUserText.Copy(
        ( *iSettingsArray->At( aSettingLBIndex ).iMuiuSettingsItemArray )[aValue] );
    ( *iSettingsArray )[ aSettingLBIndex ].iCurrentNumber = aValue;
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogCDMA::Item
//
// ----------------------------------------------------
TInt CSmumMainSettingsDialogCDMA::Item( TInt aSettingLBIndex ) const
    {
    return iSettingsArray->At( aSettingLBIndex ).iCurrentNumber;
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogCDMA::CheckOpeningListBoxItemL
//
// ----------------------------------------------------
void CSmumMainSettingsDialogCDMA::CheckOpeningListBoxItemL( TBool aEnterPressed )
    {
    // Pop up the settings query
    TBool okCancel = iSettingsArray->EditItemL( iListBox->CurrentItemIndex(), aEnterPressed );

    TInt settingIndex = iListBox->CurrentItemIndex();

    // If it was a callback setting
    // Either own number, phonebook number, enter own or none
    //
    if ( settingIndex == ESmumCallbackNumberLBICDMA )
    {
        
        // TBuf containing the number
        //
        HBufC* cbNum = HBufC::NewLC(iMaxCallbackLength);
        TPtr number = cbNum->Des();
        number.Copy(iSettingsArray->CallBackNumber());
        if ( 0 == ( number.Compare( iNoCallbackNumber->Des() )))
            {
            number.Zero();
            }

        // Select the appropriate action based on user selection
        // OK Pressed
        if( okCancel ) 
        {
            switch( Item( settingIndex ) ) 
            {
                case ESmumUseThisPhoneNumber: // Use This Phone Number:
                    ReadOwnNumberL(number);
                    break;
                case ESmumAddFromContact: // Pick a Number from the phonebook
                    DoAddressBookDialogL(number);
                    break;
                case ESmumEnterPhoneNumber: // Input a Callback Number;
                    {
                    HBufC* prompt = StringLoader::LoadLC( R_QTN_MCE_SETTING_ENTER_CALLBACK, iCoeEnv );
                    CAknTextQueryDialog* dlg = CAknTextQueryDialog::NewL(number);
                    CleanupStack::PushL( dlg );
                    dlg->SetPromptL(prompt->Des());
                    dlg->SetMaxLength(iMaxCallbackLength);
                    dlg->ExecuteLD(R_SMS_CALLBACKNUMBER_SETTING_QUERY);
                    CleanupStack::Pop();// dlg

                    // Cleanup
                    if (prompt)
                        {
                        CleanupStack::PopAndDestroy(prompt);// prompt
                        }
                    }
                    break;
                case ESmumNoCallbackNumber: //KNoCallbackNumber:
                    number.Zero();
                    break;
                default:
                    break;
            } // Switch

            // Set the number to the listbox
            //
            if ( 0 != ( number.Size() ) )
            {
            	//The cb number should be DTMF compatible; valid characters are (0..9 * #)
            	//Since dialog is using the EAknEditorStandardNumberModeKeymap flag, invalid characters (w p - +) has to be removed
            	//Meanwhile removing the invalid characters will reduce the original input number length
                HBufC* temp_cbNum = HBufC::NewLC(iMaxCallbackLength);
		        TInt i;
		        for(i=0; i<number.Length(); i++)
		        {
		        	//check if the number has any non DTMF characters to be ignored
		        	if((number[i]>='0' && number[i]<='9') || number[i]=='*' || number[i]=='#')
		        	{
		        	    temp_cbNum->Des().Append(number[i]);
		        	}
		        }
		        
		        if (temp_cbNum->Des().Length()!=0)
		        {
	                iSettingsArray->SetCallBackNumberL(temp_cbNum->Des());
		        }
		        else
		        {
	                iSettingsArray->SetCallBackNumberL(iNoCallbackNumber->Des());
		        }
                
		        CleanupStack::PopAndDestroy(temp_cbNum); // temp_cbNum
            }
            else 
            {
                iSettingsArray->SetCallBackNumberL(iNoCallbackNumber->Des());
            }
        }

        CleanupStack::PopAndDestroy(); // cbNum
    }  // If
    
    iListBox->DrawNow();
    }

// ----------------------------------------------------
// CSmumMainSettingsDialogCDMA::LaunchServiceCentresDialogL
//
// ----------------------------------------------------
TInt CSmumMainSettingsDialogCDMA::LaunchServiceCentresDialogL()
    {
    return KErrNotSupported;
    }
// ----------------------------------------------------
// CSmumMainSettingsDialogCDMA::LaunchHelpL
// launch help using context
// 
// ----------------------------------------------------
void CSmumMainSettingsDialogCDMA::LaunchHelpL()
    {    
    if ( iHelpFeatureSupported )
        {
        CCoeAppUi* editorAppUi = static_cast<CCoeAppUi*> (ControlEnv()->AppUi());
        CArrayFix<TCoeHelpContext>* helpContext = editorAppUi->AppHelpContextL();   
        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
        }
    }


// ----------------------------------------------------
// CSmumMainSettingsDialogCDMA::GetHelpContext
// returns helpcontext as aContext
// 
// ----------------------------------------------------
void CSmumMainSettingsDialogCDMA::GetHelpContext
        (TCoeHelpContext& aContext) const
    {    

    if ( iTypeOfSettings )
        {
        const TUid KUidSmsEditor ={0x100058BC};
        aContext.iMajor = KUidSmsEditor; 
        aContext.iContext = KSMS_HLP_SENDINGOPTIONS;        
        }
    else
        {
        
        const TUid KUidMce ={0x100058C5};      
        aContext.iMajor = KUidMce;
        aContext.iContext = KMCE_HLP_SETTINGS_SMS;
        }    
   
    }

// ---------------------------------------------------------
// CSmumMainSettingsDialogCDMA::ReadOwnNumberL
// ---------------------------------------------------------
void CSmumMainSettingsDialogCDMA::ReadOwnNumberL(TDes& aOwnNumber)
    {
#ifdef __WINS__  
        // assign a dummy number to CCdmaSmsHeader
        TBuf<12> tempNumber( _L("6041234567" ));
        aOwnNumber.Zero();
        aOwnNumber.Append( tempNumber );
#else
/*
        // Read in the own number from NAM settings for callback number
        TChar ch;
        HBufC* aNumber = HBufC::NewLC(iMaxCallbackLength);
        TPtr digitOnlyCallbackNumber = aNumber->Des();

        TSettingsRequestStatus status = ENotProcessed;

        RMobileNamStore::TMobileNamEntryV1* entry = new(ELeave) RMobileNamStore::TMobileNamEntryV1();
        CleanupStack::PushL(entry);

        // assign nam information to entry
        entry->iNamId = ENamOne;
        entry->iParamIdentifier = ENamMdn;  // id for MDN (own number)

        // For Synchronous call
        CTASetCore* settings = CTASetCore::NewL();
        CleanupStack::PushL( settings );
        // get own number from NAM storage (synchronous method)
        status = RTASettings::GetNamProgrammingL( *settings, NULL,  *entry );
        if (status == ERequestComplete)
            {
            TBuf<RMobileNamStore::KMaxNamParamSize> temBuf;
            temBuf.Copy(entry->iData);
            // aviod exceeding the lenght (NAM max len: 64)
            TInt len = Min(temBuf.Length(), iMaxCallbackLength);
            for (TInt i=0; i<len; i++)
                {
                ch = temBuf[i];
                if ( (ch.IsDigit()) || (ch=='*') || (ch=='#') )
                    digitOnlyCallbackNumber.Append( ch );
                }
            }

        // Assign callback number as ownnumber
        aOwnNumber.Zero();
        aOwnNumber.Append( digitOnlyCallbackNumber );

        CleanupStack::PopAndDestroy( settings );
        CleanupStack::PopAndDestroy( entry );
        CleanupStack::PopAndDestroy(aNumber); // Buffer for the number
*/        
#endif

    }

// ---------------------------------------------------------
// CSmumMainSettingsDialogCDMA::DoAddressBookDialogL
// ---------------------------------------------------------
void CSmumMainSettingsDialogCDMA::DoAddressBookDialogL(TDes & aNumber)
    {
    // Check if we already have necessary engines running
    //
    if ( !iPbkEngine )
        {
        iPbkEngine = CPbkContactEngine::NewL();
        }
    if ( !iPhonebookResource.IsOpen())
        {
        iPhonebookResource.OpenL();
        }

    // Add searching array to parameters
    //
    TInt filter = 0;
    filter = (CContactDatabase::EPhonable);
    
    CPbkSingleEntryFetchDlg::TParams params;
    params.iContactView = &iPbkEngine->FilteredContactsViewL( filter ); 
    
    // Launch fetching dialog
    //
    CPbkSingleEntryFetchDlg* fetchDlg = CPbkSingleEntryFetchDlg::NewL( params );
    fetchDlg->SetMopParent( iListBox );
    TInt okPressed = fetchDlg->ExecuteLD();

    // Process results
    if ( okPressed )
        {
            // Get the selected contact
            const TContactItemId cid = params.iSelectedEntry;
            
            // Open the selected contact using Phonebook engine,
            // choose correct number (launch list query if needed)
            //
            CPbkContactItem* pbkItem = iPbkEngine->ReadContactLC( cid );
            CPbkSmsAddressSelect* selectDlg = new (ELeave)  CPbkSmsAddressSelect( );     
            CPbkSmsAddressSelect::TParams contactParam(*pbkItem);

            // Select a phone number if required
            if(selectDlg->ExecuteLD( contactParam )) {
                
                // Get the selected number
                //
                HBufC* temp = HBufC::NewLC(iMaxCallbackLength);
                TPtr tempAddress = temp->Des();
                tempAddress.Copy(contactParam.SelectedField()->Text());

                // Assign the number
                //
                if ( tempAddress.Length() )
                    {
                    aNumber.Zero();
                    aNumber = tempAddress;
                    }
                CleanupStack::PopAndDestroy(temp); 
            
            }

            if ( pbkItem )
                {
                CleanupStack::PopAndDestroy(pbkItem);  // pbkItem
                }
        }
    }
    
// ---------------------------------------------------------
// CSmumMainSettingsDialogCDMA::IsMsgQueuingOnL
// ---------------------------------------------------------
TBool CSmumMainSettingsDialogCDMA::IsMsgQueuingOnL()
	{
	// Get Message Queuing Parameters
    //
    CMsvOffPeakTimes* peakTimes = new (ELeave) CMsvOffPeakTimes();
  	CleanupStack::PushL(peakTimes);
    CMsvSendErrorActions* sendErrActions = CMsvSendErrorActions::NewLC();
    CMsvSysAgentActions* actions = new (ELeave) CMsvSysAgentActions();
    CleanupStack::PushL(actions);
    CMsvScheduleSettings* schSettings = CMsvScheduleSettings::NewL();
    CleanupStack::PushL(schSettings);
    
    // Get default settings from SMS Account
    //
    iSmsAccount->LoadSettingsL( *schSettings, *peakTimes, *sendErrActions, *actions );
    int count = actions->Count();
    // Cleanup  
    //
    CleanupStack::PopAndDestroy(4,peakTimes); // peakTimes, sendErrActions, actions, schSettings
    
    // Did we have any settings?
    return( count ? ETrue : EFalse );
	}
		
// ---------------------------------------------------------
// CSmumMainSettingsDialogCDMA::SetMsgQueuingOnL
// ---------------------------------------------------------
void CSmumMainSettingsDialogCDMA::SetMsgQueuingOnL(TBool aSetting) const
	{
    // Get Message Queuing Parameters
    //
    CMsvOffPeakTimes* peakTimes = new (ELeave) CMsvOffPeakTimes();
    CleanupStack::PushL(peakTimes);
    CMsvSendErrorActions* sendErrActions = CMsvSendErrorActions::NewLC();
    CMsvSysAgentActions* actions = new (ELeave) CMsvSysAgentActions();
    CleanupStack::PushL(actions);
    CMsvScheduleSettings* schSettings = CMsvScheduleSettings::NewL();
    CleanupStack::PushL(schSettings);

    // Get default settings from SMS Account
    //
    iSmsAccount->LoadSettingsL( *schSettings, *peakTimes, *sendErrActions, *actions );
    actions->Reset();

    if( aSetting )
        {
        // Setup System Actent Conditions
        // Message queuing condition to meet in order to send the message
        // Must have network coverage, if we do not have network coverage, we fail with -31003
        // Max retry of 3 days (3430minutes) and maximum of 1 retry
        //
        TMsvSysAgentConditionAction action;
        action.iUseDefaultSysAgentAction = EFalse;
        action.iErrorAction.iAction = ESendActionRetryConditionMet;
        action.iErrorAction.iRetries = ESendRetriesFixed;
        action.iErrorAction.iRetrySpacing = ESendRetrySpacingStatic;
        action.iErrorAction.iError  = KMsgQError;
        action.iErrorAction.SetMaxRetries(KDefMaxRetries);

        action.iCondition.iVariable = KUidNetworkStatus; //KUidNetworkStatus;
        action.iCondition.iState = ESANetworkAvailable;    //ESANetworkAvailable
        action.iCondition.iType = TMsvCondition::EMsvSchSendEquals;
        actions->AppendL(action);	
        }

    // Save Settings
    iSmsAccount->SaveSettingsL( *schSettings, *peakTimes, *sendErrActions, *actions );
    
    // Cleanup  
    CleanupStack::PopAndDestroy(4,peakTimes); // peakTimes, sendErrActions, actions, schSettings	
	}
	
//  End of File  

