/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     Sms CDMA Viewer Application UI
*
*/


// UIKON/AVKON
#include <akntitle.h>                   // CAknTitlePane
#include <txtrich.h>                    // Rich text ed
// messaging editors
#include <msgbodycontrol.h>             // CMsgBodyControl
#include <eikrted.h>                    // CEikRichTextEditor
#include <msgeditorview.h>              // CMsgEditorView
#include <msgaddresscontrol.h>          // CMsgAddressControl
#include <msgbiobodycontrol.h>          // CMsgBioBodyControl
// messaging
#include <smuthdr.h>
#include <SmsMessageWrappers.h> 
#include <MuiuMsvUiServiceUtilities.h>  // DiskSpaceBelowCriticalLevelL
#include <MuiuMsvUiServiceUtilitiesInternal.h>
#include <AknInputBlock.h>				// CAknInputBlock
// phone
#include <CommonPhoneParser.h>          // Common phone number validity checker
// resources
#include <smsviewer.rsg>                // resouce identifiers
#include <MsgEditor.mbg>                // bitmaps
// local
#include "msgsmsviewer.hrh"             // application specific commands
#include "msgsmsviewerdoc.h"            // CMsgSmsViewerDocument
#include "msgsmsviewerappui.h"          // CMsgSmsViewerAppUi
// Find item
#include <finditemmenu.rsg>             // find item resources
#include <finditemmenu.h>               // CFindItemMenu
#include <finditem.hrh>
//#include <finditemdialog.h>             // CFindItemDialog
// phonebook
#include <CPbkContactEngine.h>          // Phonebook Engine
#include <RPbkViewResourceFile.h>
#include <CPbkFieldsInfo.h>             // Phonebook field types
#include <CPbkDataSaveAppUi.h>          // CreateContact menu
// general
#include <StringLoader.h>               // StringLoader
#include <FeatMgr.h>                    // FeatureManager
#include <data_caging_path_literals.hrh>	// Data Caging For Platform Security
#include <akniconutils.h>				// Icon utility
#include <MsgVoIPExtension.h>

// variant
#include <bldvariant.hrh>
// local
#include "msgsmsviewerdoc.h"            // CMsgSmsViewerDocument
#include "msgsmsviewerappui.h"          // CMsgSmsViewerCDMAAppUi
#include "msgsmsviewercdmaappui.h"          // CMsgSmsViewerCDMAAppUi

#include <Sendui.h>
#include <CMessageData.h>               // for SendUi

// Mbm-file location
_LIT( KMsgEditorMbm, "MsgEditor.mbm" );

// C++ default constructor can NOT contain any code, that
// might leave.
CMsgSmsViewerCDMAAppUi::CMsgSmsViewerCDMAAppUi()
    : iCallbackNumber( KNullDesC )
    {
    }

// Symbian OS default constructor can leave.
void CMsgSmsViewerCDMAAppUi::ConstructL()
    {
    CMsgEditorAppUi::ConstructL();

    // Prepare FeatureManager, read values to members
    FeatureManager::InitializeLibL();
    iHelpFeatureSupported = FeatureManager::FeatureSupported( KFeatureIdHelp );
    iEmailFeatureSupported =  FeatureManager::FeatureSupported(KFeatureIdEmailOverSms);
    
    // Prepare iSmsHeader
    CPlainText* nullString = CPlainText::NewL();
    CleanupStack::PushL( nullString );
    iSmsHeader = CSmsHeader::NewL( CSmsPDU::ESmsDeliver, *nullString );
    CleanupStack::PopAndDestroy(nullString);

    iSendUi = CSendUi::NewL();
    
    if( !iEikonEnv->StartedAsServerApp( ) )
        { // If the app was not started as server app, we can call PrepareLaunchL
        Document()->PrepareToLaunchL( this );
        }
    iMsgVoIPExtension = CMsgVoIPExtension::NewL();
    
    // Use CDMA Menubar
    MenuBar()->SetMenuTitleResourceId(R_SMSV_OPTIONSMENUBAR_CDMA);
    MenuBar()->SetMenuType(CEikMenuBar::EMenuOptions);
    }   

// Destructor
CMsgSmsViewerCDMAAppUi::~CMsgSmsViewerCDMAAppUi()
    {
    FeatureManager::UnInitializeLib();
    delete iMsgVoIPExtension;
    }


// ---------------------------------------------------------
// CMsgSmsViewerCDMAAppUi::LaunchViewL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerCDMAAppUi::LaunchViewL()
    {
    CAknInputBlock::NewLC();

    // Creating view (exits app immediately if fails)
    iView = CMsgEditorView::NewL( *this, CMsgEditorView::EMsgReadOnly );
    // get hands on entry
    const CMsgSmsViewerDocument* doc = Document();
    TMsvEntry msvEntry = doc->Entry();
    // Getting sender/receiver to From/To-field
    // (just copies the content of iDetails to To/From-field,
    // should work also with IR-messages...)
    iRecipientstring = msvEntry.iDetails;  

    __ASSERT_DEBUG(msvEntry.Id() != 
         KMsvNullIndexEntryId, Panic( EMsgSmsNoMessage ));


    // Instead of using Sms Mtm load the entry's values
    // using store. This is because Sms Viewer is also used
    // by bio messages and then sms mtm can't be used...

    CMsvStore* store = doc->CurrentEntry().ReadStoreL();
    CleanupStack::PushL(store);
  
    // This TRAPping is needed to find out if the entry's store has
    // KUidMsvSMSHeaderStream. If it hasn't, we're most propably dealing
    // with non sms message and we can assume it is received message.
    TRAPD( err, iSmsHeader->RestoreL( *store ));
    if (err == KErrNone)
        {// Yes, it is sms based message
        iIrBt = EFalse;
        
        // Original longish LaunchViewL-function is splitted to 3 parts;
        // LaunchViewL, SmsBasedMsgHandlingL & LaunchBioViewL
        SmsBasedMsgHandlingL( msvEntry.iBioType, store );
        }
    else
        {// No, this is not a sms and sms pdu can't be checked. However
        // this is most propably _received_ IR or BT message.
        iIrBt = ETrue;
        iControlId = EMsgComponentIdFrom;
        iView->AddControlFromResourceL( 
            R_SMSV_FROM, 
            EMsgAddressControl, 
            EMsgAppendControl, 
            EMsgHeader);
        }
    CleanupStack::PopAndDestroy( store ); //store

    // Tests if Bio message
    if ( msvEntry.iBioType ) 
        {
        // Original longish LaunchViewL-function is split into 3 parts;
        // LaunchViewL, SmsBasedMsgHandlingL & LaunchBioViewL
        iFindItemMenu = CFindItemMenu::NewL( ESmsViewerFindItem );
        LaunchBioViewL( msvEntry );
        }

    if ( iIsFromEmail )
        { // Email over SMS. Use email address
        if (KErrNotFound != iEmailAddress.Match( iRecipientstring ))
            {
            iRecipientstring = KNullDesC;
            }
        }
    else // General SMS. Use sender number
        {
        if (KErrNotFound != iNumber.Match( iRecipientstring ))
            {
            iRecipientstring = KNullDesC;
            }
        }
    CMsgBaseControl* addressBase = iView->ControlById( iControlId );
    CMsgAddressControl* addressControl = static_cast<CMsgAddressControl*>( addressBase );
    if ( iIsFromEmail )
        { // Email over SMS. Use email address          
        addressControl->AddRecipientL( iRecipientstring, iEmailAddress, KNullContactId, ETrue );
        }
    else
        { // General SMS. Use sender number
        addressControl->AddRecipientL( iRecipientstring, iNumber, KNullContactId, ETrue );
        }

    if ( !iBioMsg )
        {
        // Read the value of automatic highlight sd-key
        ReadAutoHlSharedDataValueAndSetNotifyL();
        // Set the state of automatic highlighting for sms
        SetAutomaticHighlightL( iAutomaticHlValue );
        }

    // Parse the filename
    TParse tp;
    tp.Set( KMsgEditorMbm, &KDC_APP_BITMAP_DIR, NULL );
    // Load the bitmaps
    CFbsBitmap *bitmap;
    CFbsBitmap *bitmapMask;
    AknIconUtils::CreateIconLC(
        bitmap, bitmapMask, tp.FullName(), 
        EMbmMsgeditorQgn_prop_folder_sms_tab1, 
        EMbmMsgeditorQgn_prop_folder_sms_tab1_mask);

    // navipane tabgroup takes ownership of the bitmaps.
    UpdateNaviPaneL( bitmap, bitmapMask );
    CleanupStack::Pop(2); // bitmap, bitmapMask

    // set focus
    iView->ExecuteL( ClientRect(), iControlId );

    CleanupStack::PopAndDestroy();// CAknInputBlock
    }


// ---------------------------------------------------------
// CMsgSmsViewerAppUi::SmsBasedMsgHandlingL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerCDMAAppUi::SmsBasedMsgHandlingL( 
    TInt32 aBioType, CMsvStore* aStore )
    {
    // Initialize the richtext object...
    CParaFormatLayer* paraFormat = iEikonEnv->SystemParaFormatLayerL(); // <- pointer only
    CCharFormatLayer* charFormat = iEikonEnv->SystemCharFormatLayerL(); // <- pointer only
    CRichText* textBody = CRichText::NewL( paraFormat, charFormat );
    CleanupStack::PushL( textBody );
    // ...and read bodytext
    if ( aStore != NULL )
        {
        aStore->RestoreBodyTextL( *textBody );
        }
    if ( !aBioType )
        {
        // message is ordinary sms message or Picture Message
        // so, let's get message body to viewer. 
        CMsgBaseControl* baseControl = iView->ControlById( 
            EMsgComponentIdBody );
        CMsgBodyControl* bodyControl = static_cast<CMsgBodyControl*>(baseControl);
        
        iFindItemMenu = CFindItemMenu::NewL( EFindItemMenuPlaceHolder );
            //the Title-pane text
            HBufC* text = StringLoader::LoadLC( R_QTN_TITLE_SMS, iCoeEnv );
            CAknTitlePane* title = static_cast<CAknTitlePane*> 
                (StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle )));

            title->SetTextL( *text );
            CleanupStack::PopAndDestroy(); // text

            // body text
            bodyControl->SetTextContentL( *textBody );
            if ( iView->ItemFinder() )
                {
                iView->ItemFinder()->SetFindModeL( 
                    CItemFinder::EPhoneNumber |
                    CItemFinder::EUrlAddress |
                    CItemFinder::EEmailAddress );
                }
            bodyControl->SetPlainTextMode( ETrue );
        }
    CleanupStack::PopAndDestroy(); //textBody
    
    // Deciding between To/From text in address control

    const CSmsPDU::TSmsPDUType pduType = iSmsHeader->Type();

    switch ( pduType )
        {

        case CSmsPDU::ESmsDeliver:
           
            // Setting from-control
            iControlId = EMsgComponentIdFrom;
            iView->AddControlFromResourceL( 
                    R_SMSV_FROM, 
                    EMsgAddressControl, 
                    EMsgAppendControl, 
                    EMsgHeader );
            ExtractCallingInfoL();
            
            break;

        case CSmsPDU::ESmsSubmit:
            // Setting To-control
            iControlId = EMsgComponentIdTo;
            iView->AddControlFromResourceL( 
                R_SMSV_TO, 
                EMsgAddressControl, 
                EMsgAppendControl, 
                EMsgHeader );
            // take the number
            iNumber = iSmsHeader->FromAddress();

            break;

        default:
            Panic( EMsgSmsViewerUnknownSmsPduType );
            break;
        }
    }


// ---------------------------------------------------------
// CMsgSmsViewerCDMAAppUi::DynInitNonBioOptionsMenuL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerCDMAAppUi::DynInitNonBioOptionsMenuL( CEikMenuPane* aMenuPane )
    {
    if ( aMenuPane != NULL ) 
        {
        TPtrC senderNumber = KNullDesC();

        if ( iIsFromEmail ) // case of Email over SMS 
            {  
            senderNumber.Set( iEmailAddress );
            iFindItemMenu->SetSenderDescriptorType( CItemFinder::EEmailAddress );
            if ( iValidCallbackNumber )
                {
                iFindItemMenu->SetCallbackNumber ( iCallbackNumber);
                }
            }
        else 
            {
            if ( iValidSenderNumber ) // if sender's number is valid, choose it first
                {
                senderNumber.Set( iNumber );
                }
            if ( iValidCallbackNumber && ( ComparePhoneNumberL( iCallbackNumber, iNumber ) != 0 ) )
                {
                iFindItemMenu->SetCallbackNumber ( iCallbackNumber);
                }
            }

        iFindItemMenu->AddItemFinderMenuL( 
                IsBodyFocused() ? iView->ItemFinder() : 0,
                aMenuPane, EFindItemMenuPlaceHolder,
                senderNumber, 
                ( iRecipientstring.Length() != 0 ));
        // Remove "Call back to sender" and "create cc" from menu
        // Find UI can handle it now.
        aMenuPane->SetItemDimmed( ESmsViewerCallBack, ETrue );
        aMenuPane->SetItemDimmed( ESmsViewerCreateCC, ETrue );

        // Remove "reply"  from menu, 
        // if this is a sent message
        if ( iControlId == EMsgComponentIdTo )
            {
            aMenuPane->SetItemDimmed( ESmsViewerReply, ETrue );
            }

        // Help handling
        aMenuPane->SetItemDimmed( EAknCmdHelp, !iHelpFeatureSupported );
        }
    }

// ---------------------------------------------------------
// CMsgSmsViewerCDMAAppUi::HandleCommandL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerCDMAAppUi::HandleCommandL(TInt aCommand )
    {
    // is it find item command
    if ( iFindItemMenu && iFindItemMenu->CommandIsValidL( aCommand ) && !iGms && !iBioMsg )
        {
        iFindItemMenu->HandleItemFinderCommandL( aCommand );
        return;
        }
    
    // Are we dealing with bio-message?
    if ( iBioMsg ) 
        {
        // let the biocontrol handle the command and check
        // whether it was handled.
        if ( aCommand==EAknCmdHelp )
            {
            LaunchHelpL();
            return;
            }

        if ( BioBody().HandleBioCommandL( aCommand ))
            {
            return;// Bio control has handled the command.
            }
        }
    
    // find item
    if ( iFindItemMenu && iFindItemMenu->CommandIsValidL( aCommand ) && ( iGms || iBioMsg ))
        {
        DoSearchL( aCommand );
        }
    
    switch ( aCommand )
        {
        case ESmsViewerReply:
            CheckDiskAndReplyForwardL( EFalse );
            Exit( EAknSoftkeyClose );
            break;
        case ESmsViewerForward:
            CheckDiskAndReplyForwardL( ETrue );
            Exit( EAknSoftkeyClose );
            break;
        case ESmsViewerDelete:
            DoFileDeleteL();
            break;
        case ESmsViewerMove:
            DoMoveEntryL();
            break;
        case ESmsViewerCallBack:
            DoCallBackToSenderL();
            break;
        case ESmsViewerCreateCCNew:
            // FALLTHROUGH
        case ESmsViewerCreateCCExisting:
            DoCreateContactCardL( aCommand );
            break;
        case ESmsViewerMessInfo:
            DoMessageInfoL();
            break;
        case EAknSoftkeyBack:
            Exit( EAknSoftkeyClose );
            break;
        case EEikCmdExit:
            // system exit
            DoMsgSaveExitL();
            break;
        case EAknCmdHelp:
            LaunchHelpL();
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CMsgSmsViewerCDMAAppUi::DynInitMenuPaneL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerCDMAAppUi::DynInitMenuPaneL(
    TInt aMenuId, 
    CEikMenuPane* aMenuPane )
    {
    // Confirm app is running properly
    if ( !Document()->IsLaunched())
        {
        // If not, hide everything and return
        TInt amountOfItems = aMenuPane->NumberOfItemsInPane();
        if ( amountOfItems )
            {
            aMenuPane->DeleteBetweenMenuItems( 0, amountOfItems-1 );
            return;
            }
        }

    // ...then construct the menu itself. There is two menu-resources,
    // one for contextmenu and one for normal optionsmenu. 
    switch ( aMenuId )
        {
        case R_SMSV_OPTIONSMENUPANE_CDMA: 
            {
            if ( !iBioMsg )
                { // Normal sms or picture msg
                DynInitNonBioOptionsMenuL( aMenuPane );
                }
            else
                { // Smart (BIO) msg
                DynInitBioOptionsMenuL( aMenuId, aMenuPane );
                }
            }
            break;
        case R_SMSV_CONTEXTMENUPANE_CDMA:
            {
            if ( !iBioMsg )
                { // Normal sms or picture msg
                DynInitNonBioContextMenuL( aMenuPane );
                }
            else
                { // Smart (BIO) msg
                DynInitBioContextMenuL( aMenuPane );
                }
            }                
            break;
        case R_FINDITEMMENU_MENU:
            {
            iFindItemMenu->DisplayFindItemCascadeMenuL( *aMenuPane );
            }
            break;
        default:
            if ( iBioMsg || iGms )
                {// for BIO & GMS; create contact card menu
                // If it's AIW sub-menu, let AIW initialize it
                InitAiwContactCardSubMenuL( aMenuPane );
                }
            else
                {// for normal SMS; update the finder menu
                if( iVoIPNumber && iMsgVoIPExtension->IsVoIPSupported() )
                    {
                    iFindItemMenu->SetSenderDescriptorType( CItemFinder::EEmailAddress );
                    }
                iFindItemMenu->UpdateItemFinderMenuL( aMenuId, aMenuPane );
                }
            break;
        }
    }


// ---------------------------------------------------------
// CMsgSmsViewerCDMAAppUi::DynInitNonBioContextMenuL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerCDMAAppUi::DynInitNonBioContextMenuL( CEikMenuPane* aMenuPane )
    {
    if ( aMenuPane != NULL )
        {

        TPtrC senderNumber = KNullDesC();
        if ( iIsFromEmail ) // SMS originating address is email address 
            {  
            senderNumber.Set( iEmailAddress );
            iFindItemMenu->SetSenderDescriptorType( CItemFinder::EEmailAddress );
            if ( iValidCallbackNumber )
                {// set callback number when it is attached on the email over SMS
                iFindItemMenu->SetCallbackNumber ( iCallbackNumber);
                }
            }            
        else 
            {// SMS originating adddress is Phone number
            if ( iValidSenderNumber ) // if sender's number is valid, choose it first
                {
                senderNumber.Set( iNumber );
                }
            if ( iValidCallbackNumber && ( ComparePhoneNumberL( iCallbackNumber, iNumber ) != 0 ) )
                {// set callback number when it is valid and different from SMS originating address.
                iFindItemMenu->SetCallbackNumber ( iCallbackNumber);
                }
            }

        iFindItemMenu->AddItemFinderMenuL( 
                IsBodyFocused() ? iView->ItemFinder() : 0,
                aMenuPane, EFindItemContextMenuPlaceHolder,
                senderNumber, 
                ETrue, ETrue );

        }
    }

// ---------------------------------------------------------
// CMsgSmsViewerCDMAAppUi::HandleKeyEventL
// (other items were commented in a header).
// ---------------------------------------------------------
TKeyResponse CMsgSmsViewerCDMAAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {
    TKeyResponse aResponse = EKeyWasNotConsumed;
   
    // Confirm app is running properly
    if ( iView && ( aType == EEventKey ) && Document()->IsLaunched())
        { 
        // react with all sms classes
        switch ( aKeyEvent.iCode )
            {
            case EKeyUpArrow:
            case EKeyDownArrow:
                {
                // Base-editor handles scrolling
                iView->OfferKeyEventL( aKeyEvent, aType );
                aResponse = EKeyWasConsumed;
                }
                break;
            case EKeyDevice3:
                {// Selection-key checking (Context sensitive menu)
                // (with Class 0 just show the menu)
                MenuBar()->SetMenuTitleResourceId( R_SMSV_CONTEXTMENUBAR_CDMA);
                MenuBar()->SetMenuType(CEikMenuBar::EMenuContext);
                MenuBar()->TryDisplayMenuBarL();
                MenuBar()->SetMenuTitleResourceId( R_SMSV_OPTIONSMENUBAR_CDMA );
                MenuBar()->SetMenuType(CEikMenuBar::EMenuOptions);
                aResponse = EKeyWasConsumed;
                }
                break;
            case EKeyYes:
                {// Send-key checking. Either valid sender's phone number or callback number is valid. 
                if ( !iIrBt && !iBioMsg )
                    { // normal sms
                    DoHandleSendKeyL();
                    }
                else if ( iValidSenderNumber && !iIrBt )
                    { // smart message
                    TBool okToQuery( ETrue );
                    // Is it biocontrol?
                    TUint32 permBio = OptionsMenuPermissionsBioL();
                    okToQuery = ( permBio & EMsgBioCallBack ) ? ETrue : EFalse ;
                        
                    
                  // Launch confirmation query and start calling
                    if ( okToQuery )
                        {
                        if ( iMsgVoIPExtension->IsPreferredTelephonyVoIP() &&
                             iMsgVoIPExtension->VoIPProfilesExistL() )
                            {
                            MsvUiServiceUtilitiesInternal::InternetCallToSenderQueryL( iNumber, iRecipientstring, ETrue );  
                            }
                        else
                            {
                            MsvUiServiceUtilitiesInternal::CallToSenderQueryL( iNumber, iRecipientstring, ETrue );
                            }
                        }
                    }
                else
                    {
                    //do nothing
                    }
                aResponse = EKeyWasConsumed;
                }
                break;
            default:
                break;
            }
        
            switch ( aKeyEvent.iCode )
                {
                case EKeyRightArrow:
                    {// Show next message if possible
                    if ( IsNextMessageAvailableL( ETrue ))
                        {
                        NextMessageL( ETrue );
                        aResponse = EKeyWasConsumed;
                        }
                    }
                    break;
                case EKeyLeftArrow:
                    { // Show prev message if possible
                    if ( IsNextMessageAvailableL( EFalse ))
                        {
                        NextMessageL( EFalse );
                        aResponse = EKeyWasConsumed;
                        }
                    }
                    break;
                case EKeyBackspace:
                    {
                    DoFileDeleteL();
                    aResponse = EKeyWasConsumed;
                    }
                    break;
                default:
                    break;
                }
        }
    else // App is not running properly yet
        {
        aResponse = EKeyWasConsumed;

        }
    
    return aResponse; 
    }


// ----------------------------------------------------
// CMsgSmsViewerCDMAAppUi::DoHandleSendKeyL
// 
// ----------------------------------------------------

void CMsgSmsViewerCDMAAppUi::DoHandleSendKeyL()
    {
    if ( !iMsgVoIPExtension )
        {
        return;
        }

    TPtrC senderNumber = KNullDesC();
    TPtrC alias = KNullDesC();
    TPtrC focusedNumber = KNullDesC();
        
    if ( iIsFromEmail ) // case of Email over SMS 
        {  
        senderNumber.Set( iEmailAddress );
        }
    else // SMS originating address is phone number
        {
        if ( iValidSenderNumber ) // if sender's number is valid, choose it first
            {
            senderNumber.Set( iNumber );
            }
        if ( iRecipientstring.Length() )
            {
            alias.Set( *&iRecipientstring );
            }
        }

    if ( IsBodyFocused() && iView->ItemFinder( ) )
        {
        const CItemFinder::CFindItemExt& item =
            iView->ItemFinder( )->CurrentItemExt( );
        if (    item.iItemDescriptor
            &&  (   item.iItemType == CItemFinder::EPhoneNumber
                ||  item.iItemType == CItemFinder::EEmailAddress ) )
            {
            focusedNumber.Set( *(item.iItemDescriptor) );
            }
        }

    MsvUiServiceUtilitiesInternal::InternetOrVoiceCallServiceL( 
            *iMsgVoIPExtension,
            senderNumber,
            alias,
            focusedNumber,
            ETrue,
            iEikonEnv );    
    }


// ----------------------------------------------------
// CMsgSmsViewerCDMAAppUi::SetAutomaticHighlightL
// 
// ----------------------------------------------------
void CMsgSmsViewerCDMAAppUi::SetAutomaticHighlightL( const TBool aSwitchON )
    {
    __ASSERT_DEBUG( !iGms, Panic( EMsgSmsNotForGms ) );
    __ASSERT_DEBUG( !iBioMsg, Panic( EMsgSmsNotAllowedForBio ) );
    if ( iView->ItemFinder() )
        {
        // content highlight
        if ( aSwitchON )
            { // switch ON
            iView->ItemFinder()->SetFindModeL( 
                CItemFinder::EPhoneNumber |
                CItemFinder::EUrlAddress |
                CItemFinder::EEmailAddress );
            }
        else
            { // switch OFF
            iView->ItemFinder()->SetFindModeL( 
                CItemFinder::ENoneSelected );
            }
        // header highlight     
        if ( iControlId != EMsgComponentIdTo && ( iValidSenderNumber || iValidEmailAddress) )
            {
             static_cast<CMsgAddressControl*>
                 (iView->ControlById( iControlId ))->SetAddressFieldAutoHighlight( aSwitchON );

            }
        }
    }

// ----------------------------------------------------
// CMsgSmsViewerCDMAAppUi::ExtractCallingInfoL
// 
// ----------------------------------------------------
void CMsgSmsViewerCDMAAppUi::ExtractCallingInfoL()
{


	HBufC* callback = NULL;	

	callback = iSmsHeader->CdmaMessage().GetCallbackNumberL();
	if (callback != NULL)
	  {
		CleanupStack::PushL(callback);
		iCallbackNumber = callback->Des();
	    iValidCallbackNumber = CommonPhoneParser::IsValidPhoneNumber( 
            iCallbackNumber, CommonPhoneParser::EPhoneClientNumber );
		CleanupStack::PopAndDestroy(callback);
		
	  }
	// Currently only consider general SMS situation.

    if ( MsvUiServiceUtilities::IsValidEmailAddressL(iSmsHeader->FromAddress()) 
    				&& iEmailFeatureSupported )
      { 
	  iIsFromEmail = ETrue;
	  iValidEmailAddress = ETrue;
	  iVoIPNumber = ETrue;
	  iEmailAddress = iSmsHeader->FromAddress();
      }
    else
      {
      iNumber = iSmsHeader->FromAddress();
      iValidSenderNumber = CommonPhoneParser::IsValidPhoneNumber( iNumber, 
            CommonPhoneParser::ESMSNumber );		
      }
	
}

