/*
* Copyright (c) 2002 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*     Sms Viewer Application UI
*
*/



// INCLUDE FILES

// UIKON/AVKON
#include <akntitle.h>                   // CAknTitlePane
#include <eikmenub.h>                   // CEikMenuBar
#include <aknnotewrappers.h>            // AknNoteWrappers
#include <txtrich.h>                    // Rich text ed
#include <aknnavide.h>              // CAknNavigationDecorator
// messaging editors
#include <MsgBodyControl.h>             // CMsgBodyControl
#include <eikrted.h>                    // CEikRichTextEditor
#include <MsgEditorView.h>              // CMsgEditorView
#include <MsgAddressControl.h>          // CMsgAddressControl
#include <msgbiobodycontrol.h>          // CMsgBioBodyControl
#include <MsgBioUids.h>                 // smart message type uids
#include <msgbiocontrol.h>              // CMsgBioControl
#include <CGmsWrapper.h>                // CGmsWrapper
#include <MsgNaviPaneControl.h>
// messaging
#include <smuthdr.h>                    // CSmsHeader
#include <smsclnt.h>                    // CSmsClientMtm
#include <MsgFolderSelectionDialog.h>   // CMsgFolderSelectionDialog
#include <MuiuMsvProgressReporterOperation.h>  // CMsvProgressReporterOperation
#include <MtmExtendedCapabilities.hrh>	// function id of MessageInfo
#include <MuiuOperationWait.h>              // MuiuOperationWait
#include <mtclreg.h>
#include <mtuireg.h>
#include <mtmdef.h>
#include <akninputblock.h>				// CAknInputBlock
#include <MuiuMsvUiServiceUtilities.h>  // DiskSpaceBelowCriticalLevelL
#include <muiumsvuiserviceutilitiesinternal.h>
#include <messagingvariant.hrh>         // Local variation flags
#include <smumutil.h>                   // SmumUtil
#include <csmsemailfields.h>            // CSmsEmailFields
#include <msgtextutils.h>
// phone
#include <commonphoneparser.h>          // Common phone number validity checker
// resources
#include <SmsViewer.rsg>                // resouce identifiers
#include <msgeditor.mbg>                // bitmaps
// local
#include "MsgSmsViewer.hrh"             // application specific commands
#include "MsgSmsViewerDoc.h"            // CMsgSmsViewerDocument
#include "MsgSmsViewerAppUi.h"          // CMsgSmsViewerAppUi
// Find item
#include <FindItemmenu.rsg>             // find item resources
#include <finditemmenu.h>               // CFindItemMenu
#include <finditemdialog.h>             // CFindItemDialog
#include <finditem.hrh>
#include <ItemFinder.h>
// Creating contact card via AIW
#include <AiwServiceHandler.h>          // CAiwServiceHandler
#include <AiwContactAssignDataTypes.h>  // AiwContactAssign
// general
#include <StringLoader.h>               // StringLoader
#include <featmgr.h>                    // FeatureManager
#include <SenduiMtmUids.h>
#include <sendui.h>
#include <CMessageData.h>               // for SendUi
#include <data_caging_path_literals.hrh>
#include <AknIconUtils.h>
#include <centralrepository.h>
#include <messaginginternalcrkeys.h>
#include <CommonUiInternalCRKeys.h>
#include <coreapplicationuisdomainpskeys.h>


// help
#include <csxhelp/sms.hlp.hrh>          // sms help context ids 
#include <csxhelp/smart.hlp.hrh>
#include <hlplch.h>                     // for HlpLauncher 

#include <msgvoipextension.h>

#include <MsgEditorAppUiExtension.h>// for iMsgEditorAppUiExtension
#include <MsgEditorSettingCacheUpdatePlugin.h>

//Skinnable Title pane icon
#include <AknStatuspaneUtils.h>     // AknStatuspaneUtils
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknsUtils.h>
#include <muiu.mbg>           // title icon
#ifdef RD_MSG_FAST_PREV_NEXT
#include <MuiuMessageIterator.h>
#endif

#ifdef RD_SCALABLE_UI_V2
#include <aknnavide.h>              // pen support on navi pane
#include <AknNaviObserver.h>        // pen support of volume control of navi pane
#include <akntoolbar.h>     
#include <eikcolib.h>		//Added for touchbar support
#endif
#include <aknclearer.h>     // CAknLocalScreenClearer
// CONSTANTS

// Sms Viewer Uid 
const TUid KUidSmsViewer ={0x100058BD};
// GMS Viewer Uid
const TUid KUidGMSViewer ={0x101F4CDA};
// Sms Mtm Service Uid
const TUid KUidSmsMtm = {0x1000102c};
// First free id
const TInt EMsgImageControlId = EMsgComponentIdFirstFreeEditorSpecificId;

// Size approxamations
// TMsvEntry itself
const TInt KApproxEmptyTMsvEntrySize = 200; // atleast when SC included to entry (Outbox)
// Recipients
const TInt KApproxRecipientSize = 80; // manually entered ~50 & fetched ~70
// GMS msg with full 120 chars and whole picture (no recipients)
const TInt KApproxFullGMS = 450; // full of text and whole picture

_LIT( KSmsBitmapFileName, "muiu.mif" );

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
CMsgSmsViewerAppUi::CMsgSmsViewerAppUi()
    {
    // Put constructor code into function InitMembers() as it's
    // also called to prepare the viewer to show a new message.
    InitMembers();
    }

// Initialize data members into a default state. Called from a constructor
// and when a sms is loaded into a viewer to replace a current one.
void CMsgSmsViewerAppUi::InitMembers()
    {
    // Initialize variables that cannot be zeroed or are initialized to a
    // non-zero value.

    // iFullyConstructed is only initialized in construction
    
    iNumber.Zero();
    iRecipientstring.Zero();

    // Initialize flags
    Mem::FillZ( &iFlags, sizeof(iFlags) );

    iControlId = 0;
    iClass0CBA = NULL;
    iFindItemMenu = NULL;
    iAiwServiceHandler = NULL;
    iGmsModel = NULL;
    iSmsHeader = NULL;
    iViewerOperation = NULL;
    iMsgVoIPExtension  = NULL;
    iSendUi = NULL;
    iCenRepSession = NULL;
    iNotifier = NULL;
    }

// Symbian OS default constructor can leave.
void CMsgSmsViewerAppUi::ConstructL()
    {
    CMsgEditorAppUi::ConstructL();

    ConstructMembersL();

	if( !iEikonEnv->StartedAsServerApp( ) )
        { // If the app was not started as server app, we can call PrepareLaunchL
        Document()->PrepareToLaunchL( this );
        }        
    }   

// ConstructL() without calling base class construct
void CMsgSmsViewerAppUi::ConstructMembersL()
    {
    // Prepare FeatureManager, read values to members and uninitialize FeatureManager
    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        iFlags.iHelpFeatureSupported = ETrue;
        }
    // SMSGms variation flags
    if ( FeatureManager::FeatureSupported( KFeatureIdSmartmsgSMSGmsMessageReceiving ) )
        {
        iFlags.iSmartMsgSmsGmsMessageReceiving = ETrue;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdSmartmsgSMSGmsMessageSendingSupported ) )
        {
        iFlags.iSmartMsgSmsGmsMessageSending = ETrue;
        }
    if ( FeatureManager::FeatureSupported( KFeatureIdAudioMessaging ) )
        {
        iFlags.iAudioMsgSupported = ETrue;
        }
    FeatureManager::UnInitializeLib();
    
    //check unieditor restricted reply mode
    TInt features = 0;
    CRepository* repository = CRepository::NewLC( KCRUidMuiuVariation );   
    repository->Get( KMuiuUniEditorFeatures, features );
    CleanupStack::PopAndDestroy( repository );
    iFlags.iSmsRestrictedMode = ( features & KUniEditorFeatureIdRestrictedReplySms );
		repository = NULL;
	
	//check autolock mode
    TInt value( KErrNotFound ); 
    RProperty::Get( KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus, value );
    if ( value > EAutolockOff )
        {
        iFlags.iAutoLockEnabled = ETrue;
        }
    iMsgVoIPExtension = CMsgVoIPExtension::NewL();

    // Local variation flags
    if ( SmumUtil::CheckEmailOverSmsSupportL() )
        { // Email supported
        iFlags.iEmailFeatureSupported = ETrue;
        }
    //Middle softkey
    MenuBar()->SetContextMenuTitleResourceId( R_SMSV_CONTEXTMENUBAR );        
  	// Set path of bitmap file
  	TParse fileParse;
  	fileParse.Set( KSmsBitmapFileName, &KDC_APP_BITMAP_DIR, NULL );
    
  	
    iAppIcon = AknsUtils::CreateGulIconL( 
        AknsUtils::SkinInstance(), 
        KAknsIIDQgnPropMceSmsTitle,
        fileParse.FullName(),
        EMbmMuiuQgn_prop_mce_sms_title,
        EMbmMuiuQgn_prop_mce_sms_title_mask );
        
            
	iScreenClearer = CAknLocalScreenClearer::NewLC( ETrue );
	CleanupStack::Pop( iScreenClearer );    
    iTypeMsg = CSmsPDU::ESmsDeliver; //for successful launch save the PDU Type.


    
    }

// Destructor
CMsgSmsViewerAppUi::~CMsgSmsViewerAppUi()
    {
    // Put destructor code into function DestructMembers() as it's
    // also called to prepare the viewer to show a new message.
    DestructMembers();
    }

// Destruct data members. Called from a destructor and when a sms
// is loaded into a viewer to replace a current one.
void CMsgSmsViewerAppUi::DestructMembers()
    {
    // If operation is still active we must cancel it before exit
    if ( iViewerOperation ) 
        {
        iViewerOperation->Cancel();
        }
    delete iViewerOperation;
    iViewerOperation = NULL;
    delete iFindItemMenu;
    iFindItemMenu = NULL;
    delete iSendUi;
    iSendUi = NULL;    
    delete iAiwServiceHandler;
    iAiwServiceHandler = NULL;    
    delete iGmsModel;
    iGmsModel = NULL;
    delete iSmsHeader;
    iSmsHeader = NULL;

    if( iNotifier )
        {
        iNotifier->StopListening();
        delete iNotifier;
        iNotifier = NULL;
        }
    delete iCenRepSession; // don't delete before notifier
    iCenRepSession = NULL;
    delete iMsgVoIPExtension;
    iMsgVoIPExtension = NULL;
    // AppUi doesn't own iClass0CBA, so it's deleted elsewhere 
	#ifdef RD_SCALABLE_UI_V2
	if(iToolbar)
		{
    	delete iToolbar;
    	iToolbar = NULL;
    	static_cast<CEikAppUiFactory*>( 
                CEikonEnv::Static()->AppUiFactory() )->SetViewFixedToolbar( NULL );
  		}
	#endif	
    delete iAppIcon;
    iAppIcon = NULL;
    delete iScreenClearer;
    iScreenClearer = NULL;
    }

#ifdef RD_MSG_FAST_PREV_NEXT
// ---------------------------------------------------------
// CMsgSmsViewerAppUi::EntryChangedL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::EntryChangedL()
    {
    // First call to EntryChangedL() comes before LaunchViewL() has been called by
    // document.
    if ( !iFullyConstructed )
        return;

    if( iMsgNaviDirection == KErrNotFound )
        {
        // Clear and redraw all controls
        iView->ResetControls();
        iView->DrawNow();
        
        DestructMembers();
        InitMembers();
        delete iView; iView = NULL;
        ConstructMembersL();
        LaunchViewL();
        }
    else
        {
        QuicklaunchViewL();
        }
    
    // Note: In smum (sms mtm UI), file smsu.cpp. Message is marked as read in
    // sim in case it's class 2 sms stored in sim. It is referenced as a fix to
    // an error SPAN-6B7K33. That piece of code could be executed here but is
    // left out because all sim messages are shown as read by default in sim folder
    // anyways.
    }
#endif // RD_MSG_FAST_PREV_NEXT

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::Document
// (other items were commented in a header).
// ---------------------------------------------------------
CMsgSmsViewerDocument* CMsgSmsViewerAppUi::Document() const
    {
    return STATIC_CAST( CMsgSmsViewerDocument*, CMsgEditorAppUi::Document());
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::LaunchViewL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::LaunchViewL()
    {
    CAknInputBlock::NewLC();
    // get hands on entry
    TMsvEntry msvEntry1;
    
    TRAPD( error, msvEntry1 = Document()->Entry() );
    
    if( error != KErrNone )
        {
        //Something wrong. Exit here
        DoMsgSaveExitL();
        return;
        }
    
    TBool toolbarVisible = EFalse;
#ifdef RD_SCALABLE_UI_V2
    if ((!iToolbar)&& (AknLayoutUtils::PenEnabled() && !msvEntry1.iBioType && !iFlags.iGms) )
    	{
    	CEikAppUiFactory* appUiFactory = static_cast<CEikAppUiFactory*>( iEikonEnv->AppUiFactory() );
        CAknToolbar* oldFixedToolbar = appUiFactory->CurrentFixedToolbar();

        	iToolbar = CAknToolbar::NewL( R_SMSVIEWER_APP_TOOLBAR );
        	iToolbar->SetToolbarObserver( this );
        

        if ( oldFixedToolbar != iToolbar )
            {
            oldFixedToolbar->SetToolbarVisibility( EFalse );    
            }
        
        appUiFactory->SetViewFixedToolbar( iToolbar );    	
        iToolbar->SetToolbarVisibility( ETrue, EFalse ); 
        toolbarVisible = ETrue;
	    }	    
    //handling the case of iToolbar already created [delete the toolbar incase of biomsgs]
    else if((iToolbar)&&(AknLayoutUtils::PenEnabled() && msvEntry1.iBioType && iFlags.iGms)) 
        {
            delete iToolbar;
            iToolbar = NULL;
        }
        
#endif	
    // Creating view (exits app immediately if fails)
    iView = CMsgEditorView::NewL( *this, CMsgEditorView::EMsgReadOnly );
    // get hands on entry
    const CMsgSmsViewerDocument* doc = Document();
    TMsvEntry msvEntry = doc->Entry();
    
    // Getting sender/receiver to From/To-field
    // (just copies the content of iDetails to To/From-field,
    // should work also with IR-messages...)
    
    //Copy only 50 chars with extremely long names
    iRecipientstring = msvEntry.iDetails.Left( KPhCltNameBufferLength );  

    __ASSERT_DEBUG(msvEntry.Id() != 
         KMsvNullIndexEntryId, Panic( EMsgSmsNoMessage ));

    iFlags.iGms = (msvEntry.iBioType == KMsgBioUidPictureMsg.iUid);

    // Instead of using Sms Mtm load the entry's values
    // using store. This is because Sms Viewer is also used
    // by bio messages and then sms mtm can't be used...
    CPlainText* nullString = CPlainText::NewL();
    CleanupStack::PushL( nullString );
    iSmsHeader = CSmsHeader::NewL( CSmsPDU::ESmsDeliver, *nullString );
    CMsvStore* store = doc->CurrentEntry().ReadStoreL();
    CleanupStack::PushL(store);
  
    // This TRAPping is needed to find out if the entry's store has
    // KUidMsvSMSHeaderStream. If it hasn't, we're most propably dealing
    // with non sms message and we can assume it is received message.
    TRAPD( err, iSmsHeader->RestoreL( *store ));

    // Check if we are dealing with E-mail sms message
    // We use TP-PID as the only validation rule
    CSmsPDU& pdu = iSmsHeader->Message().SmsPDU();
	if( pdu.ProtocolIdentifierPresent() )
		{
		if(	pdu.PIDType() == TSmsProtocolIdentifier::ESmsPIDTelematicInterworking &&
			pdu.TelematicDeviceIndicator() == TSmsProtocolIdentifier::ESmsTelematicDevice &&
			pdu.TelematicDeviceType() == TSmsProtocolIdentifier::ESmsInternetElectronicMail )
			{
			iFlags.iIsEmailMessage = ETrue;
			}
		}

    if (err == KErrNone)
        {// Yes, it is sms based message

        // Original longish LaunchViewL-function is splitted to 3 parts;
        // LaunchViewL, SmsBasedMsgHandlingL & LaunchBioViewL
        SmsBasedMsgHandlingL( msvEntry.iBioType, store );
        }
    else
        {// No, this is not a sms and sms pdu can't be checked. However
        // this is most propably _received_ IR or BT message.
        iFlags.iIrBt = ETrue;
        iControlId = EMsgComponentIdFrom;
        iView->AddControlFromResourceL( 
            R_SMSV_FROM, 
            EMsgAddressControl, 
            EMsgAppendControl, 
            EMsgHeader);
        }
    CleanupStack::PopAndDestroy( 2 ); //store, nullString

    // Tests if Bio message
    if ( msvEntry.iBioType && !iFlags.iGms ) 
        {
		// parse the number if wappush
		if( msvEntry.iBioType == KMsgBioProvisioningMessage.iUid )
		    {
			    TPtrC tempNumber( msvEntry.iDetails );
			    if(CommonPhoneParser::IsValidPhoneNumber( 
			    	tempNumber, CommonPhoneParser::ESMSNumber ))
			    {
			    	iNumber = tempNumber;
			    }	    	
		    }
        // Original longish LaunchViewL-function is split into 3 parts;
        // LaunchViewL, SmsBasedMsgHandlingL & LaunchBioViewL
        LaunchBioViewL( msvEntry );
        }
    
    // Fill address ctrl
    if ( !iFlags.iIsEmailMessage && KErrNotFound != iNumber.Match( iRecipientstring ) )
        {
        iRecipientstring = KNullDesC;
        }
    CMsgBaseControl* addressBase = iView->ControlById( iControlId );
    CMsgAddressControl* addressControl = STATIC_CAST( CMsgAddressControl*, addressBase );

    MVPbkContactLink* link= NULL;
    addressControl->AddRecipientL( 
        iRecipientstring,
        iNumber,
        ETrue,
        link );

    // Message indication ( e.g. 2/32 )
#ifdef RD_MSG_NAVIPANE_IMPROVEMENT
    // Fetch pointer to the default navi pane control
    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( StatusPane( )->ControlL(  TUid::Uid( EEikStatusPaneUidNavi ) ) );
     
    if( !iNaviDecorator )
        {
        CreateViewerNaviPaneL( Document()->Entry().iDate, EMsgEditorMsgPriorityNormal, ETrue );    
        iNaviPane->PushL( *iNaviDecorator );  // <- This has nothing to do with cleanup-stack!    
        }
    else
        {
        //We end up here when fast msg opening is enabled
        CMsgNaviPaneControl* naviControl = static_cast<CMsgNaviPaneControl*>
            ( iNaviDecorator->DecoratedControl() );
        naviControl->SetTimeIndicatorL( Document()->Entry().iDate, ETrue );
        naviControl->SetNavigationIndicatorL( Document()->Session(), Document()->Entry() );
        }
    
#else
	UpdateNaviPaneL();
#endif
	//remove buttons from navipane
	if( iNaviDecorator && iClass0CBA )
	    {
	    iNaviDecorator->SetScrollButtonDimmed( CAknNavigationDecorator::ELeftButton, ETrue );
   	    iNaviDecorator->SetScrollButtonDimmed( CAknNavigationDecorator::ERightButton, ETrue );
	    } 
#ifdef RD_SCALABLE_UI_V2
    if ( AknLayoutUtils::PenEnabled() )
        {
       	iNaviDecorator->SetNaviDecoratorObserver( this );
       	if ( iToolbar )
       		{
       	
       	   if ( iFlags.iBioMsg || iFlags.iGms ) 
       	        {
                //By Default make take touchbar invisible, dimmed and disabled
                iToolbar->SetToolbarVisibility(EFalse, EFalse);
                iToolbar->SetDimmed(ETrue);
                iToolbar->DisableToolbarL( ETrue );
       	        }
       	   
       		//Test that message type is normal SMS
       		if ( !iFlags.iBioMsg && !iFlags.iGms) 
       			{
       		    if(!toolbarVisible)
       		        {
                    //Enable the touchbar, make it visible and undimmed
                    iToolbar->DisableToolbarL( EFalse );
                    iToolbar->SetDimmed(EFalse);
                    iToolbar->SetToolbarVisibility(ETrue, EFalse);
       		        }
               	// default state, nothing dimmed. Uncomment the below lines only if necessary to force set it dim again
                 /*
               		iToolbar->SetItemDimmed(ESmsViewerToolbarReply, EFalse, EFalse);
               		iToolbar->SetItemDimmed(ESmsViewerToolbarForward, EFalse, EFalse);
               		iToolbar->SetItemDimmed(ESmsViewerToolbarDelete, EFalse, ETrue);
                 */

            	//What buttons to dim
           		switch ( iSmsHeader->Type() )
               		{
                	
                		//Received Message
                		case CSmsPDU::ESmsDeliver:
                		//Nothing dimmed, unless sender unknown
                			if (msvEntry.iDetails.Length() == 0 )
                				iToolbar->SetItemDimmed(ESmsViewerToolbarReply, ETrue, ETrue);
                				
                			//Or not valid
                			if (!iFlags.iValidSenderNumber)
                				iToolbar->SetItemDimmed(ESmsViewerToolbarReply, ETrue, ETrue);               					
                			break;
                	
                		//Sent Message
                		case CSmsPDU::ESmsSubmit:
                			//Dim the reply button, we would be replying to ourselves
                			iToolbar->SetItemDimmed(ESmsViewerToolbarReply, ETrue, ETrue);
                			break;
                		
                		//Some other cases we might consider dimming
               			//ESmsSubmitReport SMS-SUBMIT-REPORT, sent from SC to MS
                		//ESmsStatusReport SMS-STATUS-REQUEST, sent from SC to MS
                		//ESmsCommand      SMS-COMMAND, sent from MS to SC	
                		//ESmsDeliverReport SMS-DELIVER-REPORT, sent from MS to SC
                		
                		//TODO Necessary?
                		default:
                			Panic( EMsgSmsViewerUnknownSmsPduType );
                			break;

                		}	// Switch
            		}	// if ( !iFlags.iBioMsg && !iFlags.iGms) 
            	}	// if ( iToolbar )
            }     // if ( AknLayoutUtils::PenEnabled() ) 	
#endif	
    // set focus
    iView->ExecuteL( ClientRect(), iControlId );

    SetTitleIconL();
		iFullyConstructed = ETrue;
		
		if ( !iFlags.iBioMsg && !iFlags.iGms )
    {
        // Read the value of automatic highlight sd-key
        ReadAutoHlSharedDataValueAndSetNotifyL();
        // Set the state of automatic highlighting for sms
        SetAutomaticHighlightL( iFlags.iAutomaticHlValue );
    }
    
    else 
    {
    	if ( iView && iView->ItemFinder() ) 
    		{
    	    iView->ItemFinder()->SetFindModeL( CItemFinder::ENoneSelected );
    		}
    	
    }
		
		delete iScreenClearer;
    iScreenClearer = NULL;
    CleanupStack::PopAndDestroy();// CAknInputBlock
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::SmsBasedMsgHandlingL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::SmsBasedMsgHandlingL( 
    TInt32 aBioType, CMsvStore* aStore )
    {
    // Initialize the richtext object...
    CParaFormatLayer* paraFormat = iEikonEnv->SystemParaFormatLayerL(); // <- pointer only
    CCharFormatLayer* charFormat = iEikonEnv->SystemCharFormatLayerL(); // <- pointer only
    CRichText* textBody = CRichText::NewL( paraFormat, charFormat );
    CleanupStack::PushL( textBody );
    // ...and read bodytext
    aStore->RestoreBodyTextL( *textBody );
    
    if ( !aBioType || iFlags.iGms)
        {
        // message is ordinary sms message or Picture Message
        // so, let's get message body to viewer. 
        CMsgBaseControl* baseControl = iView->ControlById( 
            EMsgComponentIdBody );
        CMsgBodyControl* bodyControl = STATIC_CAST( 
            CMsgBodyControl*, 
            baseControl );
        
        if ( !iFlags.iGms )
            { // normal sms
            //the Title-pane text
            HBufC* text = StringLoader::LoadLC( R_QTN_TITLE_SMS, iCoeEnv );
            CAknTitlePane* title = STATIC_CAST( 
                CAknTitlePane*, 
                StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle )));
            title->SetTextL( *text );
            CleanupStack::PopAndDestroy(); // text
            
            CMsgTextUtils::ConvertLineBreaksL( *textBody, CMsgTextUtils::ECRLFtoLF );
            
            // body text
            bodyControl->SetTextContentL( *textBody );
            
            // Class 0 handling...
            TSmsDataCodingScheme::TSmsClass smsClass;
            if ( iSmsHeader->Message().SmsPDU().Class( smsClass ) 
                && smsClass == TSmsDataCodingScheme::ESmsClass0 )
                {
                // Create new softkeys
                iClass0CBA = Cba(); // <- pointer only
                if (iClass0CBA) 
                    { // iClass0CBA shouldn't be NULL, but if....
                    iClass0CBA->SetCommandSetL( R_SMSVIEWER_CLASS0_SOFTKEYS_OPTIONS_EXIT__CONTEXTOPTIONS );  
                    //Middle softkey
    				MenuBar()->SetContextMenuTitleResourceId( R_SMSV_OPTIONSCONTEXTMENUBAR_CLASS0 );           
                    MenuBar()->SetMenuTitleResourceId( R_SMSV_OPTIONSMENUBAR_CLASS0 );        
                    MenuBar()->SetMenuType(CEikMenuBar::EMenuOptions);     
                    }
                }
            }
        else
            { // picture message
            iGmsModel = CGmsWrapper::NewL(iCoeEnv->FsSession());
            iGmsModel->LoadMsgL(*textBody);
            bodyControl->SetTextContentL(*iGmsModel->GetTextLC());
            CleanupStack::PopAndDestroy(); // CRichText instance
            iGmsModel->AddPictureControlToViewL(*iView, EMsgImageControlId);
            iGmsModel->SetPictureMsgTitleL(*StatusPane(),
                R_QTN_SM_TITLE_PICTURE_MESSAGE);
            }

        bodyControl->SetPlainTextMode( ETrue );
        }
    CleanupStack::PopAndDestroy(); //textBody
    
    // Deciding between To/From text in address control
    const CSmsPDU::TSmsPDUType pduType = iSmsHeader->Type();
    switch ( pduType )
        {
        case CSmsPDU::ESmsDeliver:
            {
            // Setting from-control
            iControlId = EMsgComponentIdFrom;
            iView->AddControlFromResourceL( 
                R_SMSV_FROM, 
                EMsgAddressControl, 
                EMsgAppendControl, 
                EMsgHeader );
            // take the number and verify it
            iNumber = iSmsHeader->FromAddress();
            if ( !iFlags.iIsEmailMessage )
                {
                iFlags.iValidSenderNumber = CommonPhoneParser::IsValidPhoneNumber(
                    iNumber, CommonPhoneParser::ESMSNumber );

                if( !iFlags.iValidSenderNumber && iMsgVoIPExtension->IsVoIPSupported() )
                    {
                    if( MsvUiServiceUtilities::IsValidEmailAddressL( iNumber ) )
                        {
                        iFlags.iValidSenderNumber = ETrue;
                        iFlags.iVoIPNumber = ETrue;
                        }
                    }
                // Replypath indication
                if ( iSmsHeader->Deliver().ReplyPath() )
                    {
                    iView->AddControlFromResourceL( 
                        R_SMSV_REPLYPATH, 
                        EMsgExpandableControl, 
                        EMsgAppendControl, 
                        EMsgHeader );
                
                    HBufC* text = StringLoader::LoadLC( R_QTN_SMS_HEADING_REPLY_PATH, iCoeEnv );
                    CRichText& replyPath = STATIC_CAST(CMsgExpandableControl*, 
                        iView->ControlById(EMsgComponentIdCc))->TextContent();
                    replyPath.Reset();
                    replyPath.InsertL( 0, *text ); 
                    CleanupStack::PopAndDestroy(); // text
                    }
                }
            else
                {
                iFlags.iValidSenderNumber =
                    MsvUiServiceUtilities::IsValidEmailAddressL(
                        iRecipientstring );
                }
            }
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
#if defined ( _DEBUG )
            Panic( EMsgSmsViewerUnknownSmsPduType );
#else
            User::Leave( KErrNotSupported );
#endif
            break;
        }
    // If we are dealing with Email message
    // we add subject control for Email type subject
    if ( iFlags.iIsEmailMessage )
        {
        TPtrC subj( iSmsHeader->EmailFields().Subject() );
        if ( subj.Length() )
            {
            // Add subject control
            AddSubjectControlL();
            // Set the Subject field text accordigly
            SetSubjectL( subj );
            }
        }
    iTypeMsg = pduType; //for successful launch save the PDU Type.
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::LaunchBioViewL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::LaunchBioViewL( const TMsvEntry& aTMsvEntry )
    {
    // Picture Messages are not handled as Bio in this class.
    __ASSERT_DEBUG(!iFlags.iGms, Panic(EMsgSmsNotForGms));

    // Load in BioControl
    const TUid bioType={ aTMsvEntry.iBioType };
    CMsgBioBodyControl* bioBody = NULL;
    bioBody = CMsgBioBodyControl::NewL(
        *this,
        bioType,
        aTMsvEntry.Id(),
        EMsgBioViewerMode,
        Document()->Session()); //since this is the viewer application
    CleanupStack::PushL( bioBody );
    
    // Remove default body control and replace with Bio Control
    iView->DeleteControlL( EMsgComponentIdBody );
    iView->AddControlL(
        bioBody, 
        EMsgComponentIdBody,
        EMsgAppendControl, 
        EMsgBody );
    CleanupStack::Pop(); // bioBody
    
    //the Title-pane text
    HBufC* text = bioBody->HeaderTextL();
    CleanupStack::PushL( text );
    CAknTitlePane* title = STATIC_CAST( 
        CAknTitlePane*, 
        StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle )));
    title->SetTextL( *text );
    CleanupStack::PopAndDestroy(); // text
    
    //current message is bio message
    iFlags.iBioMsg = ETrue;
    
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::AddBioCommandsToMenuL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::AddBioCommandsToMenuL( 
    CEikMenuPane* aMenuPane )
    {
    BioBody().SetMenuCommandSetL( *aMenuPane );
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::OptionsMenuPermissionsBioL
// (other items were commented in a header).
// ---------------------------------------------------------
TUint32 CMsgSmsViewerAppUi::OptionsMenuPermissionsBioL()
    {
    __ASSERT_DEBUG( iFlags.iBioMsg, Panic( EMsgSmsBioNotActive ));
    return BioBody().BioControl().OptionMenuPermissionsL();
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::DynInitMenuPaneL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::DynInitMenuPaneL(
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
    // Init all needed objects
    // Creation delayed to speed up the
    // viewer's opening process
    if( !iSendUi )
        {
        iSendUi = CSendUi::NewL();
        }
    if ( !iFindItemMenu )
        {
        // Tests if normal message
        if ( !iFlags.iBioMsg || !iFlags.iGms ) 
            {
            // Prepare FindItem
            iFindItemMenu = CFindItemMenu::NewL( EFindItemMenuPlaceHolder );
            iFindItemMenu->AttachItemFinderMenuL( R_SMSV_OPTIONSMENUPANE );
            }
        }

    // ...then construct the menu itself. There are two menu-resources,
    // one for contextmenu and one for normal optionsmenu. 
    switch ( aMenuId )
        {
        case R_SMSV_REPLY_MENU:
            {           			            
	            TSendingCapabilities dummy;
	            
				//hide normal short message
				aMenuPane->SetItemDimmed(
	                    ESmsViewerReplyNormal, ETrue);
				//hide mms
	            aMenuPane->SetItemDimmed(
	                    ESmsViewerReplyViaMms, ETrue); 	                    

	            //if Audio msg supported
	            if ( iFlags.iAudioMsgSupported )
	                {
	                aMenuPane->SetItemDimmed(
	                    ESmsViewerReplyViaAudio, EFalse);
	                }
                
            // If this is from email and email settings are defined
            // we allow the user to reply via E-mail
            if ( iFlags.iIsEmailMessage &&
            	 iSendUi->ValidateServiceL( KSenduiMtmSmtpUid, dummy ) )
                {
                aMenuPane->SetItemDimmed(
                    ESmsViewerReplyViaEmail, EFalse);
                }
            // If this is from PS number normal reply is dimmed    
            if ( iFlags.iVoIPNumber )
                {
                aMenuPane->SetItemDimmed(
                    ESmsViewerReplyViaEmail, EFalse);
                aMenuPane->SetItemDimmed(
                    ESmsViewerReplyNormal, ETrue);
                aMenuPane->SetItemDimmed(
                    ESmsViewerCreateCC, ETrue);    
                }    
            }
            break;
        case R_SMSV_OPTIONSMENUPANE: 
            {
            //hide aiw menu item for calender,
            aMenuPane->SetItemDimmed( KAiwCmdSaveToCalendar, ETrue );
            
            if ( !iFlags.iBioMsg )
                { // Normal sms or picture msg
                DynInitNonBioOptionsMenuL( aMenuPane );
                }
            else
                { // Smart (BIO) msg
                DynInitBioOptionsMenuL( aMenuId, aMenuPane );
                }
            if ( iFlags.iBioMsg || iFlags.iGms )                
                {
                aMenuPane->SetItemDimmed( ESmsViewerSubmenuZoomCommands, ETrue );                
                }
            }
            break;
        case R_SMSV_CONTEXTMENUPANE:
            {
            if ( !iFlags.iBioMsg )
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
            // Coverty fix , Forward NULL http://ousrv057/cov.cgi?cid=35945
            if(iFindItemMenu)
               {
               iFindItemMenu->DisplayFindItemCascadeMenuL( *aMenuPane );
         	   }
            }
            break;
        case R_SMSV_OPTIONSMENUPANE_CLASS0:
            {
            TPtrC senderNumber = KNullDesC();
            
            TBool senderHighlighted = EFalse;
            if ( FocusedControlId( ) == EMsgComponentIdFrom )
                {
                CMsgAddressControl* address = static_cast<CMsgAddressControl*>(
                    iView->ControlById( EMsgComponentIdFrom ) );
                           
                if ( address && address->Editor().SelectionLength() )
                    {
                    senderHighlighted = ETrue;
                    }
                }               
            if ( iFlags.iValidSenderNumber 
                 && senderHighlighted )
                {
                senderNumber.Set( iNumber );
                }
            //if autolock enabled, hide finditem-menu
            if ( iFlags.iAutoLockEnabled )
                {
                aMenuPane->DeleteMenuItem( EFindItemMenuPlaceHolder );
                }
            // Coverty fix , Forward NULL http://ousrv057/cov.cgi?cid=35945
            else if(iFindItemMenu)
	            {              
	            iFindItemMenu->AddItemFindMenuL( 
                FindItemHighlighted() ? iView->ItemFinder() : 0,
                aMenuPane, EFindItemMenuPlaceHolder,
                senderNumber, 
                iRecipientstring.Length() != 0 ? ETrue : EFalse,
                EFalse );
                }
            }
            break;
         case R_SMSV_OPTIONSCONTEXTMENUPANE_CLASS0:
            {
            TPtrC senderNumber = KNullDesC();
            if ( iFlags.iValidSenderNumber )
                {
                senderNumber.Set( iNumber );
                }
            //if autolock enabled, hide finditem-menu
            if ( iFlags.iAutoLockEnabled )
                {
                aMenuPane->DeleteMenuItem( EFindItemContextMenuPlaceHolder );
                }
            else
	            {              
	            iFindItemMenu->AddItemFindMenuL( 
                IsBodyFocused() ? iView->ItemFinder() : 0,
                aMenuPane, EFindItemContextMenuPlaceHolder,
                senderNumber, 
                iRecipientstring.Length() != 0 ? ETrue : EFalse,
                ETrue );
                }
            }
            break;
        case R_SMSVIEWER_ZOOM_SUBMENU:
            {
            TInt zoomLevel = KErrGeneral;
            iMsgEditorAppUiExtension->
                iSettingCachePlugin.GetValue( KAknLocalZoomLayoutSwitch, zoomLevel );
            switch ( zoomLevel )
                {
                case EAknUiZoomAutomatic:
                    aMenuPane->SetItemButtonState( EMsgDispSizeAutomatic,
                                     EEikMenuItemSymbolOn );
                    break;
                case EAknUiZoomNormal:
                    aMenuPane->SetItemButtonState( EMsgDispSizeNormal,
                                     EEikMenuItemSymbolOn );
                    break;
                case EAknUiZoomSmall:
                    aMenuPane->SetItemButtonState( EMsgDispSizeSmall,
                                     EEikMenuItemSymbolOn );
                    break;
                case EAknUiZoomLarge:
                    aMenuPane->SetItemButtonState( EMsgDispSizeLarge,
                                     EEikMenuItemSymbolOn );
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            if ( iFlags.iBioMsg || iFlags.iGms )
                {// for BIO & GMS; create contact card menu
                // If it's AIW sub-menu, let AIW initialize it
                InitAiwContactCardSubMenuL( aMenuPane );
                }
            else
                {// for normal SMS; update the finder menu
                if( iFlags.iVoIPNumber )
                    {
                    iFindItemMenu->SetSenderDescriptorType( CItemFinder::EEmailAddress );
                    }
                iFindItemMenu->UpdateItemFinderMenuL( aMenuId, aMenuPane );
                }
            break;
        }
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::DynInitNonBioOptionsMenuL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::DynInitNonBioOptionsMenuL( CEikMenuPane* aMenuPane )
    {
    if ( !iFlags.iGms )
        { // normal sms
    
         //hide submenu if only item is "via message" or restricted mode ON
        TSendingCapabilities dummy; 
        if( iFlags.iSmsRestrictedMode ||
        	( !iFlags.iAudioMsgSupported && 
        	!iFlags.iVoIPNumber &&
        	!(iFlags.iIsEmailMessage &&
	            	 iSendUi->ValidateServiceL( KSenduiMtmSmtpUid, dummy )) ))
        	{       
			aMenuPane->DeleteMenuItem( ESmsViewerReply );
			aMenuPane->SetItemDimmed( ESmsViewerReplyNoSubMenu, EFalse );
        	}        	

        // add automatic highlight menus
        TPtrC senderNumber = KNullDesC();
             
        TBool senderHighlighted = EFalse;
        if ( FocusedControlId( ) == EMsgComponentIdFrom )
            {
            CMsgAddressControl* address = static_cast<CMsgAddressControl*>(
                iView->ControlById( EMsgComponentIdFrom ) );
                       
            if ( address && address->Editor().SelectionLength() )
                {
                senderHighlighted = ETrue;
                }
            }          
        
        if ( iFlags.iIsEmailMessage ) // SMS originating address is email address 
            {  
            if ( senderHighlighted )
                {
                senderNumber.Set( iRecipientstring );
                }
            iFindItemMenu->SetSenderDescriptorType( CItemFinder::EEmailAddress );
            }            
        else 
            {// SMS originating adddress is Phone number
            if ( iFlags.iValidSenderNumber
                 && senderHighlighted )
                {
                senderNumber.Set( iNumber );
                }
            }
        //Added to remove Copy-option when sender number is not valid (for example it has alphabets)    
        if ( iFlags.iValidSenderNumber || IsBodyFocused() )
        	{
	        if( !iFlags.iVoIPNumber )
	            {
	            iFindItemMenu->SetSenderHighlightStatus( 
	                iView->ControlById( EMsgComponentIdFrom ) == iView->FocusedControl() );
	            iFindItemMenu->SetSenderDisplayText( senderNumber );
	            iFindItemMenu->AddItemFindMenuL( 
	                FindItemHighlighted() ? iView->ItemFinder() : 0,
	                aMenuPane, EFindItemMenuPlaceHolder,
	                senderNumber, 
	                iFlags.iIsEmailMessage ? EFalse : ( iRecipientstring.Length() != 0 ) );
	            }
	        else
	            {
	            iFindItemMenu->SetSenderHighlightStatus( 
	                iView->ControlById( EMsgComponentIdFrom ) == iView->FocusedControl() );
	            iFindItemMenu->SetSenderDisplayText( senderNumber );
	            iFindItemMenu->AddItemFindMenuL( 
	                FindItemHighlighted() ? iView->ItemFinder() : 0,
	                aMenuPane, EFindItemMenuPlaceHolder,
	                senderNumber, 
	                ETrue );
	            }
            }
        else //Not valid sender number, dim the find item
        	{
        	aMenuPane->SetItemDimmed( EFindItemMenuPlaceHolder, ETrue);
        	}
            
        // not needed in automatic highlight
        aMenuPane->SetItemDimmed( ESmsViewerCallBack, ETrue );
        aMenuPane->SetItemDimmed( ESmsViewerCreateCC, ETrue );
        }
    else
        { // GMS
        // dim automatic highlight items
        aMenuPane->SetItemDimmed( EFindItemMenuPlaceHolder, ETrue );
        
        // not supported in unified editor
        aMenuPane->SetItemDimmed( ESmsViewerForward, ETrue );
        
        //dim the Add to Contacts menu if the number is already in address book
        if (iRecipientstring.Length() != 0)
        	{
        	//Number is already in address book
        	aMenuPane->SetItemDimmed( ESmsViewerCreateCC, ETrue );
        	}
        
		}

    // Remove "Call back to sender", "reply" and "create cc" from
    // menu, if this is a sent message or sender number is unusable
    if ( iControlId == EMsgComponentIdTo || !iFlags.iValidSenderNumber )
        {
        aMenuPane->SetItemDimmed( ESmsViewerReply, ETrue );
        if ( iFlags.iGms )
            { // "create cc" and "call back to sender" valid only Gms
            aMenuPane->SetItemDimmed( ESmsViewerCreateCC, ETrue );
            aMenuPane->SetItemDimmed( ESmsViewerCallBack, ETrue );
            }
        }
    else if ( iFlags.iValidSenderNumber && iFlags.iGms )
        {
        // Number usable, request Phonebook2 to add menu items (AIW)
        InitAiwContactCardMenuL( R_SMSV_OPTIONSMENUPANE, aMenuPane );
        }
    else
        { // Lint
        }    
    
    if ( iFlags.iVoIPNumber )
        {
        aMenuPane->SetItemDimmed( ESmsViewerCreateCC, ETrue );
        aMenuPane->SetItemDimmed( ESmsViewerReply, ETrue );
        aMenuPane->SetItemDimmed( ESmsViewerForward, ETrue );
        }
    else
        { // Lint
        }

    // Help handling
    aMenuPane->SetItemDimmed( EAknCmdHelp, !iFlags.iHelpFeatureSupported );
    }
    
// ---------------------------------------------------------
// CMsgSmsViewerAppUi::DynInitNonBioContextMenuL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::DynInitNonBioContextMenuL( CEikMenuPane* aMenuPane )
    {
    // Remove "Call back to sender" and "reply" from
    // menu, if this is a sent message or sender number is not usable
    
    // if not GMS, dim following              
    if ( !iFlags.iGms )
        { // Sms
        // add automatic highlight menus
        TPtrC senderNumber = KNullDesC();
        if ( iFlags.iIsEmailMessage ) // SMS originating address is email address 
            {  
            senderNumber.Set( iRecipientstring );
            iFindItemMenu->SetSenderDescriptorType( CItemFinder::EEmailAddress );
            }            
        else 
            {// SMS originating adddress is Phone number
            if ( iFlags.iValidSenderNumber )
                {
                senderNumber.Set( iNumber );
                }
            }
        if( !iFlags.iVoIPNumber )
            {
            iFindItemMenu->SetSenderHighlightStatus( 
                iView->ControlById( EMsgComponentIdFrom ) == iView->FocusedControl() );
            iFindItemMenu->SetSenderDisplayText( senderNumber );
            iFindItemMenu->AddItemFindMenuL( 
                IsBodyFocused() ? iView->ItemFinder() : 0,
                aMenuPane, EFindItemContextMenuPlaceHolder,
                senderNumber, 
                iFlags.iIsEmailMessage ? EFalse : ( iRecipientstring.Length() != 0 ),
                ETrue );
            }
        else
            {
            iFindItemMenu->SetSenderHighlightStatus( 
                iView->ControlById( EMsgComponentIdFrom ) == iView->FocusedControl() );
            iFindItemMenu->SetSenderDisplayText( senderNumber );
            iFindItemMenu->AddItemFindMenuL( 
                IsBodyFocused() ? iView->ItemFinder() : 0,
                aMenuPane, EFindItemContextMenuPlaceHolder,
                senderNumber, 
                ETrue,
                ETrue );
            }
        }
    else 
        { // Gms
        // dim automatic highlight items
        aMenuPane->SetItemDimmed( EFindItemContextMenuPlaceHolder, ETrue );
        }
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::DynInitBioOptionsMenuL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::DynInitBioOptionsMenuL(
    TInt aMenuId,
    CEikMenuPane* aMenuPane )
    {
    // dim automatic highlight items
    aMenuPane->SetItemDimmed( EFindItemMenuPlaceHolder, ETrue );
    //dim the Add to Contacts menu if the number is already in address book
    if (iRecipientstring.Length() != 0)
    	{
    	//Number is already in address book
    	aMenuPane->SetItemDimmed( ESmsViewerCreateCC, ETrue );
    	}
    
    if ( iControlId == EMsgComponentIdTo ) // Sent item
        {
        aMenuPane->SetItemDimmed( ESmsViewerCallBack, ETrue );
        aMenuPane->SetItemDimmed( ESmsViewerReply, ETrue );
        aMenuPane->SetItemDimmed( ESmsViewerCreateCC, ETrue );
        aMenuPane->SetItemDimmed( ESmsViewerForward, ETrue );
        }
    else // Inbox item
        {
        AddBioCommandsToMenuL( aMenuPane );

        TUint32 permBio = OptionsMenuPermissionsBioL();        
        
        if ( !iFlags.iValidSenderNumber )
            {
            aMenuPane->SetItemDimmed( ESmsViewerCallBack, ETrue );
            aMenuPane->SetItemDimmed( ESmsViewerReply, ETrue );
            aMenuPane->SetItemDimmed( ESmsViewerCreateCC, ETrue );
            }
        else // Sender number is valid, check does biocontrol accept options
            {
            if ( !( permBio & EMsgBioReply ) )
                {
                aMenuPane->SetItemDimmed( ESmsViewerReply, ETrue );
                }
        
            if ( !( permBio & EMsgBioCallBack ) || iFlags.iIrBt )
                {
                aMenuPane->SetItemDimmed( ESmsViewerCallBack, ETrue );
                }

            if ( !( permBio & EMsgBioCreateCC) )
                {
                aMenuPane->SetItemDimmed( ESmsViewerCreateCC, ETrue );
                }
            else
                {
                // Request Phonebook2 to add menu items (AIW)
                InitAiwContactCardMenuL( aMenuId, aMenuPane );
                }
            }

        if ( !( permBio & EMsgBioForward ) )
            {
            aMenuPane->SetItemDimmed( ESmsViewerForward, ETrue );
            }

        if ( !( permBio & EMsgBioDelete ))
            {
            aMenuPane->SetItemDimmed( ESmsViewerDelete, ETrue );
            }

        if ( !( permBio & EMsgBioMessInfo ))
            {
            aMenuPane->SetItemDimmed( ESmsViewerMessInfo, ETrue );
            }

        if ( !( permBio & EMsgBioMove ))
            {
            aMenuPane->SetItemDimmed( ESmsViewerMove, ETrue );
            }
        
        // Help-handling
        aMenuPane->SetItemDimmed( EAknCmdHelp, !iFlags.iHelpFeatureSupported || !( permBio & EMsgBioHelp ));
        
        if ( permBio & EMsgBioFindItem ) // This used to be else if. Mistake?
            {
            iFindItemMenu->DisplayFindItemMenuItemL( *aMenuPane, ESmsViewerCreateCC );
            }
        }
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::DynInitBioContextMenuL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::DynInitBioContextMenuL( CEikMenuPane* aMenuPane )
    {
    // dim automatic highlight items
    aMenuPane->SetItemDimmed( EFindItemContextMenuPlaceHolder, ETrue );

    if ( iControlId == EMsgComponentIdTo )
        {
        //aMenuPane->SetItemDimmed( ESmsViewerReply, ETrue );
        }
    else
        {
        AddBioCommandsToMenuL( aMenuPane );

        if ( !(OptionsMenuPermissionsBioL() & EMsgBioDelete )) 
            {
            //aMenuPane->SetItemDimmed( ESmsViewerDelete, ETrue );
            }
        }
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::HandleCommandL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::HandleCommandL(TInt aCommand )
    {

    if ( aCommand != EEikCmdExit && !iFullyConstructed )
    	{
    	return;
    	}
    
        
    // is it find item command
    if ( iFindItemMenu && iFindItemMenu->CommandIsValidL( aCommand ) && !iFlags.iGms && !iFlags.iBioMsg )
        {
        iFindItemMenu->HandleItemFinderCommandL( aCommand );
        return;
        }
    
    // Are we dealing with bio-message?
    if ( iFlags.iBioMsg ) 
        {
        // let the biocontrol handle the command and check
        // whether it was handled.
        if ( aCommand==EAknCmdHelp )
            {
            LaunchHelpL();
            return;
            }

        if ( Document( )->IsLaunched( ) && BioBody().HandleBioCommandL( aCommand ))
            {
            return;// Bio control has handled the command.
            }
        }
    
    // find item
    if ( iFindItemMenu && iFindItemMenu->CommandIsValidL( aCommand ) && ( iFlags.iGms || iFlags.iBioMsg ))
        {
        DoSearchL( aCommand );
        }
    
    switch ( aCommand )
        {
        case ESmsViewerReplyNormal:	//normal sms
            CheckDiskAndReplyForwardL( EFalse );
			if (!(iEditorBaseFeatures & EStayInViewerAfterReply))
                {
                Exit( EAknSoftkeyClose );
                }
            break;
        case ESmsViewerReplyViaMms:
            DoReplyViaL( ESmsViewerReplyViaMms );
			if (!(iEditorBaseFeatures & EStayInViewerAfterReply))
                {
                Exit( EAknSoftkeyClose );
                }
            break;
        case ESmsViewerReplyViaEmail:
            DoReplyViaL( ESmsViewerReplyViaEmail );
            if (!(iEditorBaseFeatures & EStayInViewerAfterReply))
                {
                Exit( EAknSoftkeyClose );
                }
            break;
        case ESmsViewerReplyViaAudio:
            DoReplyViaL( ESmsViewerReplyViaAudio );
            if (!(iEditorBaseFeatures & EStayInViewerAfterReply))
                {
                Exit( EAknSoftkeyClose );
                }
            break;
        case ESmsViewerReplyNoSubMenu:  //unieditor, submenu hidden 
            CheckDiskAndReplyForwardL( EFalse );
            if (!(iEditorBaseFeatures & EStayInViewerAfterReply))
                {
                Exit( EAknSoftkeyClose );
            	}
            break;
        case ESmsViewerReplyViaMessage:  //unieditor
            CheckDiskAndReplyForwardL( EFalse );
			if (!(iEditorBaseFeatures & EStayInViewerAfterReply))
                {
                Exit( EAknSoftkeyClose );
                }
            break;
        case ESmsViewerForward:
            CheckDiskAndReplyForwardL( ETrue );
			if (!(iEditorBaseFeatures & EStayInViewerAfterReply))
				{
				Exit( EAknSoftkeyClose );
				}
            break;
        case ESmsViewerDelete:
            DoFileDeleteL();
            break;
        case ESmsViewerMove:
            DoMoveEntryL();
            break;
        case ESmsViewerSave:
            SaveClass0SmsL( ETrue );
            Exit( EAknSoftkeyClose );
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
        case EAknSoftkeyExit: // called only with class 0 exit
            DeleteAndExitL();
            break;
        case EAknCmdHelp:
            LaunchHelpL();
            break;
        case EMsgDispSizeAutomatic:
        case EMsgDispSizeLarge:
        case EMsgDispSizeNormal:
        case EMsgDispSizeSmall:
            HandleLocalZoomChangeL( (TMsgCommonCommands)aCommand );
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::BodyEditor
// (other items were commented in a header).
// ---------------------------------------------------------
CEikRichTextEditor& CMsgSmsViewerAppUi::BodyEditor() const
    {
     __ASSERT_DEBUG(  !iFlags.iBioMsg, Panic(EMsgSmsNotAllowedForBio));
    return STATIC_CAST( CMsgBodyControl*, iView->ControlById( EMsgComponentIdBody ))->Editor();
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::BioBody
// (other items were commented in a header).
// ---------------------------------------------------------
CMsgBioBodyControl& CMsgSmsViewerAppUi::BioBody() const
    {
    __ASSERT_DEBUG( iFlags.iBioMsg, Panic( EMsgSmsBioBodyOnlyForBio )); 
    return *STATIC_CAST( CMsgBioBodyControl*, iView->ControlById( EMsgComponentIdBody ));
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::FirstFreeCommand
// (other items were commented in a header).
// ---------------------------------------------------------
TInt CMsgSmsViewerAppUi::FirstFreeCommand() const
    {
    return ESmsViewerFirstFreeCmdId;
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::RequestHandleCommandL
// (other items were commented in a header).
// ---------------------------------------------------------
TInt CMsgSmsViewerAppUi::RequestHandleCommandL(
    TMsgCommands /*aCommand*/ )
    {
    //Not used at the moment.
    return KErrNotSupported;
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::IsCommandSupported
// (other items were commented in a header).
// ---------------------------------------------------------
TBool CMsgSmsViewerAppUi::IsCommandSupported(
    TMsgCommands /*aCommand*/ ) const
    {
    //Nothing supported at the moment.
    return EFalse;
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::IsBodyFocused
// (other items were commented in a header).
// ---------------------------------------------------------
TBool CMsgSmsViewerAppUi::IsBodyFocused() const
    {
    return (( iView->ControlById( EMsgComponentIdBody )) 
        == iView->FocusedControl()); 
    }
// ---------------------------------------------------------
// CMsgSmsViewerAppUi::IsSenderFocused
// (other items were commented in a header).
// ---------------------------------------------------------
TBool CMsgSmsViewerAppUi::IsSenderFocused() const
    {
    return (( iView->ControlById( EMsgComponentIdFrom )) 
        == iView->FocusedControl()); 
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::HandleKeyEventL
// (other items were commented in a header).
// ---------------------------------------------------------
TKeyResponse CMsgSmsViewerAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {
    
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
                return EKeyWasConsumed;
                }
            case EKeyDevice3:
            case EKeyEnter:	
                {             
                if( iView->FocusedControl()->ControlType() == EMsgAddressControl )
                    {
                    CMsgAddressControl* address = static_cast<CMsgAddressControl*>(
                        iView->FocusedControl() );
                    if ( address && !address->Editor().SelectionLength() )
                        {
                        // restore highlight to address field
                        return address->Editor().OfferKeyEventL( aKeyEvent, aType );
                        }               
                    }
                else if ( iView->FocusedControl()->ControlType() == EMsgBodyControl )
                    {       
                    CItemFinder* itemFinder = iView->ItemFinder();
                    if ( FocusedControlId() == EMsgComponentIdBody 
                         && itemFinder )
                        {                                                                          
                        if ( !itemFinder->CurrentSelection().Length() )
                            {
                            CMsgBodyControl* body = static_cast<CMsgBodyControl*>(
                                iView->FocusedControl() );
                            if ( body )
                                {                     
                                 // restore highlight to current itemfinder item
                                return body->Editor().OfferKeyEventL( aKeyEvent, aType );
                                }
                            }
                        }
                    }
                // fall through
                }
            case EMsgFindItemKeyEvent:
                {
                // Selection-key checking (Context sensitive menu)
                // (with Class 0 just show the menu)
                if ( !iClass0CBA )
                    {
                    MenuBar()->SetMenuTitleResourceId( R_SMSV_CONTEXTMENUBAR );
                    MenuBar()->SetMenuType(CEikMenuBar::EMenuContext);
                    }
                MenuBar()->TryDisplayMenuBarL();
                if ( !iClass0CBA )
                    {
                    MenuBar()->SetMenuTitleResourceId( R_SMSV_OPTIONSMENUBAR );
                    MenuBar()->SetMenuType(CEikMenuBar::EMenuOptions);
                    }
                return EKeyWasConsumed;
                }
                case EKeyYes:
	                {// Send-key checking
	                if ( !iFlags.iIrBt && !iFlags.iBioMsg && !iFlags.iGms && !iFlags.iIsEmailMessage &&
	                			!iFlags.iAutoLockEnabled )
	                    { // normal sms
	                    DoHandleSendKeyL();
	                    }
	                else if ( iFlags.iValidSenderNumber && !iFlags.iIrBt && !iFlags.iIsEmailMessage 
	                		&& !iFlags.iAutoLockEnabled )
	                    { // bio / gms - sms
	                    TBool okToQuery( ETrue );
	                    // Is it biocontrol?
	                    if ( iFlags.iBioMsg )
	                        {
	                        TUint32 permBio = OptionsMenuPermissionsBioL();
	                        okToQuery = ( permBio & EMsgBioCallBack ) ? ETrue : EFalse ;
	                        }
	                    // Launch confirmation query and start calling
	                    if ( okToQuery )
	                        {
	                        if ( iMsgVoIPExtension && 
	                            iMsgVoIPExtension->IsPreferredTelephonyVoIP() &&
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
	                }
            
            default:
                break;
            }
        
        if ( !iClass0CBA ) // ignore following events if class 0 sms
            {
            switch ( aKeyEvent.iCode )
                {
                case EKeyRightArrow:
                    {// Show next message if possible
                    if ( IsNextMessageAvailableL( ETrue ))
                        {
                        NextMessageL( ETrue );
                        return EKeyWasConsumed;
                        }
                    }
                    break;
                case EKeyLeftArrow:
                    { // Show prev message if possible
                    if ( IsNextMessageAvailableL( EFalse ))
                        {
                        NextMessageL( EFalse );
                        return EKeyWasConsumed;
                        }
                    }
                    break;
                case EKeyBackspace:
                    {
                    DoFileDeleteL();
	                return EKeyWasConsumed;
	                }
                default:
                    break;
                }
            }
        else
            { // class 0
            return EKeyWasConsumed;
            }
        }
    else // App is not running properly yet
        {
        return EKeyWasConsumed;
        }
    
    return EKeyWasNotConsumed; 
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::DoMsgSaveExitL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::DoMsgSaveExitL() 
    {
    // exit in anycase
    Exit();
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::DoFileDeleteL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::DoFileDeleteL()
    {
    // Load the texts from resources
    HBufC* text = StringLoader::LoadLC( R_QTN_QUERY_COMMON_CONF_DELETE_ME, iCoeEnv );
    
    // Confirm from user deletion of message and delete if needed.
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    if ( dlg->ExecuteLD( R_DELETEMSGCONF_QUERY, *text ))
        {
        DeleteAndExitL();        
        }

    CleanupStack::PopAndDestroy(); // text, confText
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::DoMoveEntryL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::DoMoveEntryL()
    {
    CMsvEntry& entry = Document()->CurrentEntry();
    TMsvEntry msvEntry( entry.Entry());
   
    // When going in to SelectFolder-dialog folderId is
    // the folder where message is, but when coming
    // out (if selection done) it has the value of destination 
    // folder.
    TMsvId folderId = msvEntry.Parent();
    HBufC* text = StringLoader::LoadLC( R_QTN_MCE_MOVE_MOVE_TO, iCoeEnv );
    
    // Launch moving-dialog and proceed if ok-returned
    TBool moveIt = CMsgFolderSelectionDialog::SelectFolderL( folderId, *text );
    CleanupStack::PopAndDestroy(); // text
    if ( moveIt )
        {
        MoveMessageEntryL( folderId );
        Exit( EAknSoftkeyClose );
        }
    }

// ----------------------------------------------------
// CMsgSmsViewerAppUi::DoSearchL
// (other items were commented in a header).
// ----------------------------------------------------
void CMsgSmsViewerAppUi::DoSearchL( TInt aCommand )
    {
    __ASSERT_DEBUG(!iFlags.iBioMsg, Panic(EMsgSmsNotAllowedForBio));
    CFindItemDialog* dlg;
    
    CMsgBodyControl* ctrl = STATIC_CAST( CMsgBodyControl*, 
        iView->ControlById(
        EMsgComponentIdBody ));
    
    dlg =  CFindItemDialog::NewL( 
        ctrl->TextContent().Read( 0 ), 
        iFindItemMenu->SearchCase( aCommand) );
    
    dlg->ExecuteLD();
    }

// ----------------------------------------------------
// CMsgSmsViewerAppUi::DoCallBackToSenderL
// (other items were commented in a header).
// ----------------------------------------------------
void CMsgSmsViewerAppUi::DoCallBackToSenderL()
    {
    if ( iFlags.iValidSenderNumber )
        {
        MsvUiServiceUtilitiesInternal::CallToSenderQueryL( iNumber, iRecipientstring );
        }
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::DoCreateContactCardL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::DoCreateContactCardL( TInt aCommand )
    {
    CAiwGenericParamList& inList = iAiwServiceHandler->InParamListL();
    InitAiwContactCardParamL( inList );
    // Callback is not specified as we are not interested in the outcome of the assign
    // operation.
    iAiwServiceHandler->ExecuteMenuCmdL( aCommand, inList,
        iAiwServiceHandler->OutParamListL(), 0, NULL);
    }

// ----------------------------------------------------
// CMsgSmsViewerAppUi::OpCompleted
// ----------------------------------------------------
void CMsgSmsViewerAppUi::OpCompleted(
    CMsvSingleOpWatcher& aOpWatcher,
    TInt aCompletionCode )
    {
    CMsvOperation* op = &aOpWatcher.Operation();
    // Must be trapped because calls Leaving function
    TRAP_IGNORE( DoOperationCompletedL( op, aCompletionCode ) );// Ignored as non critical block
    // The only operation
    delete iViewerOperation;
    iViewerOperation = NULL;
    }

// ----------------------------------------------------
// CMsgSmsViewerAppUi::DoOperationCompletedL
// ----------------------------------------------------
void CMsgSmsViewerAppUi::DoOperationCompletedL(
    CMsvOperation* aOperation,
    TInt aCompletionCode )
    {
    __ASSERT_DEBUG( aOperation, Panic( EMsgSmsNullPointer ) );
    if ( aOperation && aCompletionCode == EEikCmdExit )
        {
        CAknAppUi::ProcessCommandL( EAknCmdExit );
        }
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::DoMessageInfoL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::DoMessageInfoL()
    {
    __ASSERT_DEBUG( !iViewerOperation, Panic( EMsgSmsNullPointer ) );
    CMsvEntrySelection* selection = NULL; // Not used by KMtmUiFunctionMessageInfo
    CAknInputBlock::NewLC();
    CBaseMtmUi& uiMtm = Document()->MtmUiL();
    TBuf8<1> dummy;
    CMsvSingleOpWatcher* singleOpWatcher = CMsvSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( singleOpWatcher );

    CMsvOperation* op = uiMtm.InvokeAsyncFunctionL(
                                KMtmUiFunctionMessageInfo, 
                                *selection, 
                                singleOpWatcher->iStatus, 
                                dummy );
    iViewerOperation = singleOpWatcher;
    CleanupStack::Pop( 1 ); // singleOpWatcher
    singleOpWatcher->SetOperation( op );

    CleanupStack::PopAndDestroy( 1 );// CAknInputBlock
    }

// ----------------------------------------------------
// CMsgSmsViewerAppUi::ForwardGmsL
// (other items were commented in a header).
// ----------------------------------------------------
void CMsgSmsViewerAppUi::ForwardGmsL(TMsvId aTarget)
    {
    DoFwdRepGmsL(aTarget, ETrue);
    }

// ----------------------------------------------------
// CMsgSmsViewerAppUi::ReplyGmsL
// (other items were commented in a header).
// ----------------------------------------------------
void CMsgSmsViewerAppUi::ReplyGmsL(TMsvId aTarget)
    {
    DoFwdRepGmsL(aTarget, EFalse);
    }
    
// ----------------------------------------------------
// CMsgSmsViewerAppUi::DoFwdRepGmsL
// (other items were commented in a header).
// ----------------------------------------------------
void CMsgSmsViewerAppUi::DoFwdRepGmsL(TMsvId aTarget, TBool aForward)
    {
    CMsgEditorDocument& document = *Document();
    TMsvId origMsgId = document.Entry().Id();

    // Change the MTM type of msg temporarily
    CMsvEntry* entry = 
        document.Session().GetEntryL(origMsgId);
    CleanupStack::PushL(entry);

    TMsvEntry tentry(entry->Entry());
    const TUid OrigMtmUid = tentry.iMtm;
    tentry.iMtm = KUidSmsMtm;
        
    // Trap errors, so we can try to restore the original MTM Uid
    TRAPD(err, 
        {
        entry->ChangeL(tentry);
        document.SetEntryWithoutNotificationL(origMsgId);
        
        if (aForward)
            {
            ForwardL(aTarget);
            }
        else
            {
            ReplyL(aTarget);
            }
        
        });
    // Restore original Mtm Uid
    tentry = entry->Entry();
    tentry.iMtm = OrigMtmUid;
    entry->ChangeL(tentry);
    document.SetEntryWithoutNotificationL(origMsgId);
    
    User::LeaveIfError(err);
    CleanupStack::PopAndDestroy(entry);    
    }

// ----------------------------------------------------
// CMsgSmsViewerAppUi::SaveClass0SmsL
// (other items were commented in a header).
// ----------------------------------------------------
void CMsgSmsViewerAppUi::SaveClass0SmsL( TBool aShowNotes )
    {
    if ( aShowNotes )
        { // Show global "Saved"-note
        HBufC* text = StringLoader::LoadLC( R_QTN_MCE_CONF_MESSAGE_SAVED, iCoeEnv );
        CAknGlobalNote* note = CAknGlobalNote::NewLC();
        note->ShowNoteL( EAknGlobalConfirmationNote, *text );
        CleanupStack::PopAndDestroy( 2 ); // note, text
        }
    
    // Get handle to entry
    const CMsgSmsViewerDocument* doc = Document();
    CMsvEntry& entry = doc->CurrentEntry();
    TMsvEntry msvEntry( entry.Entry());
    
    // Read values from TMsvEntry
    HBufC* details = msvEntry.iDetails.AllocLC();
    HBufC* description = msvEntry.iDescription.AllocLC();
    
    // Ensure entry is not read only
    msvEntry.SetReadOnly( EFalse );
    entry.ChangeL( msvEntry );
    
    // Set class from 0 to 1 and save
    CMsvStore* msvstore = entry.EditStoreL();
    CleanupStack::PushL( msvstore );
    iSmsHeader->Message().SmsPDU().SetClass(
        ETrue, TSmsDataCodingScheme::ESmsClass1 );
    iSmsHeader->StoreL( *msvstore );
    msvstore->Commit();
    CleanupStack::PopAndDestroy(); // msvstore
    
    // Put values back to TMsvEntry
    msvEntry.iDetails.Set( *details );
    msvEntry.iDescription.Set( *description );
    
    // Change entry back to read only and ensure it's visible
    msvEntry.SetReadOnly( ETrue );
    msvEntry.SetInPreparation( EFalse );
    msvEntry.SetVisible( ETrue );
    entry.ChangeL( msvEntry );
    CleanupStack::PopAndDestroy( 2 ); // details, description
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::ApproximateReplyForwardSize
// (other items were commented in a header).
// ---------------------------------------------------------
TInt CMsgSmsViewerAppUi::ApproximateReplyForwardSize( TBool aForward )
    {
    //
    // Approximate size
    TInt approximatedSize = 0;
    if ( aForward )
        { // yes, forward
        if ( iFlags.iGms )
            { // picture msg
            approximatedSize = KApproxEmptyTMsvEntrySize + KApproxFullGMS; 
            }
        else
            { // normal sms
            approximatedSize = KApproxEmptyTMsvEntrySize + STATIC_CAST( CMsgBodyControl*, iView->ControlById( EMsgComponentIdBody ))->Editor().TextLength()*2; // multiplied because of unicode saving
            }
        }
    else
        { // no, reply (same for normal sms and gms)
        approximatedSize = KApproxEmptyTMsvEntrySize + KApproxRecipientSize;
        }

    return approximatedSize;
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::CheckDiskAndReplyForwardL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::CheckDiskAndReplyForwardL( TBool aForward )
    {
    //
    // Check disk
    if ( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL( 
    	Document()->Session(),
        ApproximateReplyForwardSize( aForward  ))) 
        {
        User::Leave( KErrDiskFull );
        }

    //to reduce flickering after sending a message.
	if (!(iEditorBaseFeatures & EStayInViewerAfterReply ))
        {
        iToolbar->SetToolbarVisibility( EFalse );
        iView->MakeVisible( EFalse );
        iNaviDecorator->MakeVisible( EFalse );
        }
        
    TRAPD(err, DoReplyFwdL( aForward ));
    if(!(err == KErrNone) )
        {
        iNaviDecorator->MakeVisible( ETrue );
        iView->MakeVisible( ETrue );
        iToolbar->SetToolbarVisibility( ETrue );
        }    
    
    }
    
// ---------------------------------------------------------
// CMsgSmsViewerAppUi::DoReplyFwdL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::DoReplyFwdL( TBool aForward )
    {
    // Actual forward / reply
    // Are we forwarding...
    if ( aForward )
        { //...yes, forwarding
        if ( iFlags.iGms )
            {
            ForwardGmsL( Document()->DefaultMsgFolder());
            }
        else
            {
            ForwardL( Document()->DefaultMsgFolder());
            }
        }
    else
        { //...no, we're replying
        if ( iFlags.iGms )
            {
            ReplyGmsL( Document()->DefaultMsgFolder());
            }
        else
            {
            ReplyL( Document()->DefaultMsgFolder());
            }
        }
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::HandleEntryDeletedL
// (other items were commented in a header).
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::HandleEntryDeletedL()
    {
    Exit( EAknSoftkeyClose );
    }

// ----------------------------------------------------
// CMsgSmsViewerAppUi::LaunchHelpL
// launch help using context
// ----------------------------------------------------
void CMsgSmsViewerAppUi::LaunchHelpL()
    {
    if ( iFlags.iHelpFeatureSupported )
        {
        // activate Help application
        CArrayFix<TCoeHelpContext>* helpContext = AppHelpContextL();
        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
        }
    }


// ----------------------------------------------------
// CMsgSmsViewerAppUi::HelpContextL
// returns help context
// ----------------------------------------------------
CArrayFix<TCoeHelpContext>* CMsgSmsViewerAppUi::HelpContextL() const
    {
    // Check if not bio message or picture message
    if ( !iFlags.iBioMsg && !iFlags.iGms )
        {
        // sms editor and sms viewer use same help file, in which 
        // sms editor id is used
        return CreateHelpContextArrayL(
            TCoeHelpContext( KUidSmsViewer, KSMS_HLP_VIEWER()));
        }
    
    // Help context of picture message
    // GMS xhtml-help is now under smartmessaging
    else if ( iFlags.iGms )
        {
        return CreateHelpContextArrayL(
            TCoeHelpContext( KUidGMSViewer, KSMART_HLP_GMSVIEWER()));
        }
    
    // Help context of bio messages
    else
        {
        TCoeHelpContext bioControlHelpContext;
        BioBody().BioControl().GetHelpContext( bioControlHelpContext );
        if ( !bioControlHelpContext.IsNull())
            {
            return CreateHelpContextArrayL( bioControlHelpContext );
            }
        // if help context not available for bio control
        else
            {
            return NULL;
            }
        }
    }

// ----------------------------------------------------
// CMsgSmsViewerAppUi::CreateHelpContextArrayL
// 
// ----------------------------------------------------
CArrayFix<TCoeHelpContext>* CMsgSmsViewerAppUi::CreateHelpContextArrayL( const TCoeHelpContext& aHelpContext ) const
    {
    CArrayFix<TCoeHelpContext>* r = new(ELeave) CArrayFixFlat<TCoeHelpContext>(1);
    CleanupStack::PushL(r);
    r->AppendL(aHelpContext);
    CleanupStack::Pop(r);
    return r;
    }

// ----------------------------------------------------
// CMsgSmsViewerAppUi::DoHandleSendKeyL
// 
// ----------------------------------------------------
void CMsgSmsViewerAppUi::DoHandleSendKeyL()
    {
    if ( !iMsgVoIPExtension )
        {
        return;
        }
    
    TPtrC number = KNullDesC();
    TPtrC alias = KNullDesC();
    TPtrC focusedAddr = KNullDesC();
	TBool dailerDisabled = EFalse;
    // sender-number
    if ( iFlags.iValidSenderNumber )
        {
        number.Set( *&iNumber );
        if ( iRecipientstring.Length() )
            {
            alias.Set( *&iRecipientstring );
            }
         if(IsSenderFocused())
         {
         	//Disabling the SendKey Event handling by Avkon
    		iAvkonAppUi->SetKeyEventFlags( CAknAppUiBase::EDisableSendKeyShort |  CAknAppUiBase::EDisableSendKeyLong );
    		dailerDisabled = ETrue;	
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
            //Dialer is opened when creating a VoIP call from SMS via Call creation key
            //Disabling the SendKey Event handling by Avkon
            iAvkonAppUi->SetKeyEventFlags( CAknAppUiBase::EDisableSendKeyShort |  CAknAppUiBase::EDisableSendKeyLong );
            dailerDisabled = ETrue;
            focusedAddr.Set( *(item.iItemDescriptor) );
            }
        }

		if(dailerDisabled)
		{
    MsvUiServiceUtilitiesInternal::InternetOrVoiceCallServiceL( 
            *iMsgVoIPExtension,
            number,
            alias,
            focusedAddr,
            ETrue,
            iEikonEnv );    
     }  
    }

// ----------------------------------------------------
// CMsgSmsViewerAppUi::SetAutomaticHighlightL
// 
// ----------------------------------------------------
void CMsgSmsViewerAppUi::SetAutomaticHighlightL( const TBool aSwitchON )
    {
    __ASSERT_DEBUG( !iFlags.iGms, Panic( EMsgSmsNotForGms ) );
    __ASSERT_DEBUG( !iFlags.iBioMsg, Panic( EMsgSmsNotAllowedForBio ) );
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
        if ( iControlId != EMsgComponentIdTo && iFlags.iValidSenderNumber )
            {
             static_cast<CMsgAddressControl*>
                 (iView->ControlById( iControlId ))->SetAddressFieldAutoHighlight( aSwitchON );

            }
        }
    }

// ----------------------------------------------------
// CMsgSmsViewerAppUi::ReadAutoHlSharedDataValueL
// 
// ----------------------------------------------------
void CMsgSmsViewerAppUi::ReadAutoHlSharedDataValueAndSetNotifyL()
    {
    __ASSERT_DEBUG( !iFlags.iGms, Panic( EMsgSmsNotForGms ) );
    __ASSERT_DEBUG( !iFlags.iBioMsg, Panic( EMsgSmsNotAllowedForBio ) );
    if ( iFlags.iAutomaticHlInitialized )
        return;
    // Create the session
    iCenRepSession = CRepository::NewL( KCRUidCommonUi );
    if( iCenRepSession )
        {
        // Get the value of AutomaticHighlight key
        TBool value = ETrue;
        iCenRepSession->Get( KCuiAutomaticHighlight, value );
        iFlags.iAutomaticHlValue = value;
        // Create the notifer
        iNotifier = 
            CCenRepNotifyHandler::NewL( 
                *this, *iCenRepSession, CCenRepNotifyHandler::EIntKey,
                KCuiAutomaticHighlight );
        // Start listening
        iNotifier->StartListeningL();       
        }
    iFlags.iAutomaticHlInitialized = ETrue; // Done once per viewer
    }

// ----------------------------------------------------------------------------
// CMsgSmsViewerAppUi::HandleNotifyInt
// ----------------------------------------------------------------------------    
//
void CMsgSmsViewerAppUi::HandleNotifyInt( 
    TUint32  /*aId*/, 
    TInt aNewValue )
    {   
    // Notifies changes on KCuiAutomaticHighlight
    iFlags.iAutomaticHlValue = aNewValue;
    TRAP_IGNORE( SetAutomaticHighlightL( aNewValue ) ); 
    }

// ----------------------------------------------------------------------------
// CMsgSmsViewerAppUi::HandleNotifyError
// ----------------------------------------------------------------------------    
//
void CMsgSmsViewerAppUi::HandleNotifyError( 
    TUint32 /*aId*/, 
    TInt /* aError */, 
    CCenRepNotifyHandler* /* aHandler */ )
    {
    // Remove session and notifier
    if( iNotifier )
        {
        iNotifier->StopListening();       
        delete iNotifier;
        iNotifier = NULL;
        }
    delete iCenRepSession;    
    iCenRepSession = NULL;
    }
        
// ----------------------------------------------------
// CMsgSmsViewerAppUi::DoReplyViaL
// 
// ----------------------------------------------------
void CMsgSmsViewerAppUi::DoReplyViaL( TInt aCase )
    {
    __ASSERT_DEBUG( iControlId != 
        EMsgComponentIdTo, Panic( EMsgSmsNoSuchComponent ) );

    CMessageData* msgData = CMessageData::NewL();
    CleanupStack::PushL( msgData );    
    CMsgAddressControl* addressControl = 
        static_cast<CMsgAddressControl*> ( iView->ControlById( iControlId ) );
    if ( addressControl )
        {
        CMsgRecipientArray& recs = *addressControl->GetRecipientsL();
        TInt count(recs.Count());
        for (TInt index = 0; index < count; index++)
            {
            if ( iFlags.iIsEmailMessage )
                {
                msgData->AppendToAddressL( 
	                *recs.At( index )->Name(),
	                KNullDesC() );
                //sender->AppendL( *recs.At( index )->Name() );
                //alias->AppendL( KNullDesC() );
                }
            else
                {
                msgData->AppendToAddressL( 
	                *recs.At( index )->Address(),
	                *recs.At( index )->Name() );
                }
            }
        }
	// Via MMS, Email, Audio or to Unieditor
    TUid mtmUid;
    switch( aCase )
		{
		case ESmsViewerReplyViaMms:
			mtmUid = KSenduiMtmMmsUid;
            break;
        case ESmsViewerReplyViaEmail:
			mtmUid = KSenduiMtmSmtpUid;
            break;	
		case ESmsViewerReplyViaAudio:
			mtmUid = KSenduiMtmAudioMessageUid;
            break;
        case ESmsViewerReplyViaMessage:	//unieditor
			mtmUid = KSenduiMtmUniMessageUid;
            break;            
        default:
            __ASSERT_DEBUG( EFalse, Panic( EMsgSmsNoSuchMtmUid ) );
            mtmUid = KSenduiMtmUniMessageUid; // suppress compiler warning
            break;
		}    
    
    
    if ( iFlags.iEmailSubjectControlPresent )
        {
        if ( SubjectControl()->TextContent().DocumentLength() )
            {
            HBufC* buf = SubjectControl()->Editor().GetTextInHBufL();
            CleanupStack::PushL( buf );
            TInt length = buf->Length();
            HBufC* replyFormat = StringLoader::LoadLC( R_QTN_SMS_REPLYFORMAT, iCoeEnv );

            _LIT( KSmsSubjectFormat, "%S" );
            TInt formatPos = replyFormat->Find( KSmsSubjectFormat );
	    	TInt foundInSubj = KErrNotFound;
		    if ( formatPos > 0 )
                {
                // -1 to avoid % in the string
			    foundInSubj = buf->FindF( replyFormat->Left( formatPos-1 ) );
                }
            HBufC* newSubject = NULL;
		    //append only when it is not in the beginning of the subject line or not found.
		    if ( foundInSubj==KErrNotFound || foundInSubj )
			    {
			    // Create a buffer large enough to hold the re-formated subject - need
		        // to subtract two from the prefix length (the %S).
		        length += replyFormat->Length() - 2;
		        newSubject = HBufC::NewLC(length);
		        TPtr ptr( newSubject->Des() );
		        // Format the reply subject
		        ptr.Format( replyFormat->Des(), buf );
			    }
            else
                {
                newSubject = buf->Des().AllocLC();
                }
            msgData->SetSubjectL( newSubject );
            //CleanupStack::Pop( ); // newSubject
            CleanupStack::PopAndDestroy( 3 ); // buf, replyFormat, newSubject
            //CleanupStack::PushL( newSubject );
            }
        }        
    iSendUi->CreateAndSendMessageL( mtmUid, msgData );
    CleanupStack::PopAndDestroy( msgData ); // msgData
    }
    
// ----------------------------------------------------
//  CMsgSmsViewerAppUi::AddSubjectControlL()
//
// ----------------------------------------------------
void CMsgSmsViewerAppUi::AddSubjectControlL()
    {
    // Sanity
    __ASSERT_DEBUG( iView, Panic( EMsgSmsNullPointer ) );
    if ( !iFlags.iEmailSubjectControlPresent )
        {
        iView->AddControlFromResourceL( R_SMS_VIEWER_SUBJECT,
                EMsgExpandableControl, EMsgAppendControl, EMsgHeader );
        // Mark the control creation
        iFlags.iEmailSubjectControlPresent = ETrue;
        }
    }
    
// ----------------------------------------------------
//  CMsgSmsViewerAppUi::SetSubjectL()
//  
// ----------------------------------------------------
void CMsgSmsViewerAppUi::SetSubjectL(const TDesC& aText)
    {
    CMsgExpandableControl* subjectControl = SubjectControl();
    if (subjectControl)
        {
        CRichText& subject = subjectControl->TextContent();
        subject.Reset();
        subject.InsertL( 0, aText );
        }
    }

// ----------------------------------------------------
//  CMsgSmsViewerAppUi::SubjectControl()
//  
// ----------------------------------------------------
CMsgExpandableControl* CMsgSmsViewerAppUi::SubjectControl() const
    {
    __ASSERT_DEBUG( iFlags.iEmailSubjectControlPresent, Panic( EMsgSmsNullPointer ));
    return STATIC_CAST(CMsgExpandableControl*, 
            iView->ControlById(EMsgComponentIdSubject));
    }

// ----------------------------------------------------
//  CMsgSmsViewerAppUi::InitAiwContactCardMenuL
//  
// ----------------------------------------------------
void CMsgSmsViewerAppUi::InitAiwContactCardMenuL( TInt aMenuId,
    CEikMenuPane* aMenuPane )
    {
    if ( !iAiwServiceHandler)
        {
        // Create AIW service handler and attach to contact card menu item
        iAiwServiceHandler = CAiwServiceHandler::NewL();
        iAiwServiceHandler->AttachMenuL( aMenuId,
            R_SMSV_AIW_CONTACT_INTEREST );
        }

    // This menu should contain AIW menu placeholder
    __ASSERT_DEBUG( iAiwServiceHandler->IsAiwMenu( aMenuId ),
        Panic( EMsgSmsAiwMenu ) );

    CAiwGenericParamList& inList = iAiwServiceHandler->InParamListL();
    InitAiwContactCardParamL( inList );
    iAiwServiceHandler->InitializeMenuPaneL( *aMenuPane, aMenuId,
        ESmsViewerCreateCCNew, inList );
    }

// ----------------------------------------------------
//  CMsgSmsViewerAppUi::InitAiwContactCardSubMenuL
//  
// ----------------------------------------------------
void CMsgSmsViewerAppUi::InitAiwContactCardSubMenuL( CEikMenuPane* aMenuPane )
    {
    // If menu is AIW sub-menu, AIW will initialize it
    if ( iAiwServiceHandler &&
         iAiwServiceHandler->HandleSubmenuL( *aMenuPane ) )
        {
        // We assume AIW inserts at most 2 items into the submenu.
        // ESmsViewerCreateCCNew and ESmsViewerCreateCCExisting.
        // Otherwise command codes will overlap
#ifdef _DEBUG
        const TInt KMaxItems = 2;
        __ASSERT_ALWAYS( aMenuPane->NumberOfItemsInPane() <=
            KMaxItems, Panic( EMsgSmsAiwMenu ) );
#endif
        }
    }

// ----------------------------------------------------
//  CMsgSmsViewerAppUi::InitAiwContactCardParamL()
//  
// ----------------------------------------------------
void CMsgSmsViewerAppUi::InitAiwContactCardParamL(
    CAiwGenericParamList& aParamList ) const
    {
    using namespace AiwContactAssign;

    TAiwGenericParam param( EGenericParamContactAssignData );
    TAiwSingleContactAssignDataV1 assignData;
    assignData.SetFlags( 0 );
    param.Value().Set( TAiwSingleContactAssignDataV1Pckg( assignData ) );
    aParamList.AppendL( param );

    // Phone number
    param.Reset();
    param.SetSemanticId( EGenericParamPhoneNumber );
    param.Value().Set( iNumber );
    aParamList.AppendL( param );
    }

#ifdef RD_SCALABLE_UI_V2
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT

// -----------------------------------------------------------------------------
// handles the touch-ui related control events for next/previous message
// -----------------------------------------------------------------------------
//
void CMsgSmsViewerAppUi::HandleNaviDecoratorEventL( TInt aEventID )     
    {
    if( AknLayoutUtils::PenEnabled() &&
        IsNextMessageAvailableL( aEventID == EAknNaviDecoratorEventRightTabArrow ) )
        {
        /* no need for separate checks for right and left arrows
            because IsNextMessageAvailableL() and NextMessageL
            are called with the truth-value of the same comparison */
        NextMessageL( aEventID == EAknNaviDecoratorEventRightTabArrow );
        }
    }
#else
void CMsgSmsViewerAppUi::HandleNaviDecoratorEventL( TInt /* aEventID */)
    {
    }       
#endif // !RD_MSG_NAVIPANE_IMPROVEMENT
#else
void CMsgSmsViewerAppUi::HandleNaviDecoratorEventL( TInt /* aEventID */)
    {
    }
#endif

#ifdef RD_MSG_FAST_PREV_NEXT
// ----------------------------------------------------
//  CMsgSmsViewerAppUi::CanFastOpenL
// ----------------------------------------------------
TBool CMsgSmsViewerAppUi::CanFastOpenL( const CMessageIterator& aIterator )
    {
    // Only sms messages can be fast opened. Bio message open is performed
    // using bio mtm-server which parses the message before viewer is opened.
    // Picture and configuration bio-messages seem to work without parsing by
    // bio mtm-server. Vcards however do not work as there is no vcard attachment
    // expected by the viewer unless message is parsed by the server first.
    // Also, allow fast open only if currently sms is open. This ensures that
    // the viewer is entered through sms mtm ui and before sms messages are viewed
    // (CSmsMtmUi::CheckAndEnsureSmsServiceOkL() is executed).
    TBool retVal = EFalse;
    TMsvEntry msvEntry = Document()->Entry();
    if( msvEntry.iMtm.iUid == KSenduiMtmSmsUidValue &&
        aIterator.CurrentMessage().iMtm.iUid == KSenduiMtmSmsUidValue &&
        !msvEntry.iBioType )
        {
        retVal = ETrue;
        }
    return retVal;
    }
#endif

// ---------------------------------------------------------
// OfferToolbarEventL
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CMsgSmsViewerAppUi::OfferToolbarEventL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case ESmsViewerToolbarReply:
                HandleCommandL( ESmsViewerReplyViaMessage );
            break;
        case ESmsViewerToolbarForward:
            HandleCommandL( ESmsViewerForward );
            break;
        case ESmsViewerToolbarDelete:
            HandleCommandL( ESmsViewerDelete );
            break;
        }
    }

#endif

// ---------------------------------------------------------
// CAudioMessageAppUi::HandleResourceChangeL
// ---------------------------------------------------------
//

void CMsgSmsViewerAppUi::HandleResourceChangeL( TInt aType )
    {
     // Base class call must be first
    CMsgEditorAppUi::HandleResourceChangeL(aType);
    
    if ( aType == KAknsMessageSkinChange )
        {
        if ( iAppIcon )
            {
            delete iAppIcon;
            iAppIcon = NULL;
            }
    
        // Set path of bitmap file
  	    TParse fileParse;
  	    fileParse.Set( KSmsBitmapFileName, &KDC_APP_BITMAP_DIR, NULL );
    
        iAppIcon = AknsUtils::CreateGulIconL( 
            AknsUtils::SkinInstance(), 
            KAknsIIDQgnPropMceSmsTitle,
            fileParse.FullName(),
            EMbmMuiuQgn_prop_mce_sms_title,
            EMbmMuiuQgn_prop_mce_sms_title_mask );
        SetTitleIconL();   
        }
     
    }


// ---------------------------------------------------------
// CAudioMessageAppUi::SetTitleIconL
// ---------------------------------------------------------
//
void CMsgSmsViewerAppUi::SetTitleIconL()
    {
    SetTitleIconSize();
    
    // Create duplicates of the icon to be used
    CFbsBitmap* bitmap = new( ELeave ) CFbsBitmap();
    CleanupStack::PushL( bitmap );
    //SetTitleIconSizeL(bitmap);
    
    
    CFbsBitmap* bitmapMask = new( ELeave ) CFbsBitmap();
    CleanupStack::PushL( bitmapMask );
    
    User::LeaveIfError( bitmap->Duplicate( iAppIcon->Bitmap()->Handle() ) );
    User::LeaveIfError( bitmapMask->Duplicate( iAppIcon->Mask()->Handle() ) );
    
    CAknTitlePane* iTitlePane = STATIC_CAST( 
            CAknTitlePane*, 
            StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle )));

    iTitlePane->SetSmallPicture( bitmap, bitmapMask, ETrue );
    iTitlePane->DrawNow();
    
    CleanupStack::Pop( bitmapMask );
    CleanupStack::Pop( bitmap );
    }


// ---------------------------------------------------------
// CAudioMessageAppUi::SetTitleIconSize
//
// Sets the correct size from LAF for title icon
// ---------------------------------------------------------
//
void CMsgSmsViewerAppUi::SetTitleIconSize()
    {
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::ETitlePane, mainPane );
    TAknLayoutRect titleIconPaneLayoutRect;
    
    if ( AknStatuspaneUtils::StaconPaneActive() )
        {
        titleIconPaneLayoutRect.LayoutRect( 
            mainPane,
            AknLayoutScalable_Avkon::title_pane_stacon_g1( 0 ).LayoutLine() );
        }
    else
        {
        titleIconPaneLayoutRect.LayoutRect( 
            mainPane,
            AknLayoutScalable_Avkon::title_pane_g2( 0 ).LayoutLine() );
        }
    
    TSize iconSize = titleIconPaneLayoutRect.Rect().Size();
    AknIconUtils::SetSize( iAppIcon->Bitmap(), iconSize, EAspectRatioPreserved );
    }    
    
// ---------------------------------------------------------
// CMsgSmsViewerAppUi::EditorObserver
// ---------------------------------------------------------
//
void CMsgSmsViewerAppUi::EditorObserver( TMsgEditorObserverFunc aFunc, TAny* aArg1,
  TAny* aArg2, TAny* aArg3 )
    {
    TRAP_IGNORE( DoEditorObserverL( aFunc,aArg1,aArg2,aArg3 ) );
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::DoEditorObserverL
// ---------------------------------------------------------
//
void CMsgSmsViewerAppUi::DoEditorObserverL( TMsgEditorObserverFunc aFunc, TAny* aArg1,
  TAny* aArg2, TAny*/* aArg3*/ )
    {
    
        switch ( aFunc )
        {
         	    
#ifdef RD_SCALABLE_UI_V2	    
	    case MMsgEditorObserver::EMsgControlPointerEvent:
            {
           	CMsgBaseControl* control = static_cast<CMsgBaseControl*>( aArg1 );
            if(control)
	          	{
	            	TInt id = control->ControlId();	            
								if(id == EMsgComponentIdFrom && iFlags.iValidSenderNumber)
									{     
									//disable dialer
									iAvkonAppUi->SetKeyEventFlags( 
				    					CAknAppUiBase::EDisableSendKeyShort | 
				    					CAknAppUiBase::EDisableSendKeyLong ); 								
									}
								else            
									{
	                	//enable dialer
									iAvkonAppUi->SetKeyEventFlags( 0x00);
									}
	            	}
            }
 		break;
        
#endif //RD_SCALABLE_UI_V2  
			default:
				break;         
        }
 
    }  


// ---------------------------------------------------------
// CMsgSmsViewerAppUi::QuicklaunchViewL
// This is quick launch based on already existing view. Hence this function should  
// be called only for Proper SMS Text message for fast and optimized navigation.
// In case called for any bio-type messages, this will leave with KErrCancel so that 
// caller will TRAP the leave and call LaunchViewL() for complete launch.
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::QuicklaunchViewL()
    {
    CAknInputBlock::NewLC();
    
    /* Reset iFullyConstructed to false to avoid any irrational behaviours during quick launch
     * And set back to true at the end of this function
     */
    iFullyConstructed = EFalse;

    // get hands on entry
    const CMsgSmsViewerDocument* doc = Document();
    TMsvEntry msvEntry = doc->Entry();
    if ( msvEntry.iBioType )
        {
        /* If in case, any bio SMS while fast navigation, then leave immediately, so that MCE will 
         * handle the navigation.
         * Leave with correct code, so that MsgEditorAppUi will perform the appropriate action to 
         * ensure the next/prev entry is launched from MCE.
         */
        User::Leave( KErrCancel );
        }
    
    /* Reset all the necessary flags before proceeding further
     * Other flags are either feature support types or based on bio-SMS types and hence 
     * no need to set/reset here
     */
    iFlags.iIsEmailMessage = EFalse;
    iFlags.iValidSenderNumber = EFalse;
    iFlags.iVoIPNumber = EFalse;
    SetAutomaticHighlightL( EFalse );
    
    // Getting sender/receiver to From/To-field
    // (just copies the content of iDetails to To/From-field,
    // should work also with IR-messages...)

    //Copy only 50 chars with extremely long names
    iRecipientstring = msvEntry.iDetails.Left( KPhCltNameBufferLength );  

    __ASSERT_DEBUG(msvEntry.Id() != 
         KMsvNullIndexEntryId, Panic( EMsgSmsNoMessage ));

    // Instead of using Sms Mtm, load the entry's values
    // using store. This is because Sms Viewer is also used
    // by bio messages and then sms mtm can't be used...
    CPlainText* nullString = CPlainText::NewL();
    CleanupStack::PushL( nullString );
    delete iSmsHeader;
    iSmsHeader = NULL;
    iSmsHeader = CSmsHeader::NewL( CSmsPDU::ESmsDeliver, *nullString );
    CMsvStore* store = doc->CurrentEntry().ReadStoreL();
    CleanupStack::PushL(store);

    // This TRAPping is needed to find out if the entry's store has
    // KUidMsvSMSHeaderStream. If it hasn't, we're most propably dealing
    // with non sms message and we can assume it is received message.
    TRAPD( err, iSmsHeader->RestoreL( *store ));

    // Check if we are dealing with E-mail sms message
    // We use TP-PID as the only validation rule
    CSmsPDU& pdu = iSmsHeader->Message().SmsPDU();
    if( pdu.ProtocolIdentifierPresent() )
        {
        if( pdu.PIDType() == TSmsProtocolIdentifier::ESmsPIDTelematicInterworking &&
            pdu.TelematicDeviceIndicator() == TSmsProtocolIdentifier::ESmsTelematicDevice &&
            pdu.TelematicDeviceType() == TSmsProtocolIdentifier::ESmsInternetElectronicMail )
            {
            iFlags.iIsEmailMessage = ETrue;
            }
        }

    if (err == KErrNone)
        {
        /* Yes, it is sms based message.
         * Quick SMS msg handler shall set the body text and required control field without adding/removing
         *  any controls everytime unless required(if absent/present across different messages).
         */
        QuickSmsMsgNaviHandlingL( store );
        }
    else
        {
        /* Something wrong. Ideally this part of the code should not be hit. 
         * This is not a sms and sms pdu can't be checked. 
         * However this is most propably _received_ IR or BT message.
         */
        iFlags.iIrBt = ETrue;
        iControlId = EMsgComponentIdFrom;
        }
    CleanupStack::PopAndDestroy( 2 ); //store, nullString

    // Fill address ctrl
    if ( !iFlags.iIsEmailMessage && KErrNotFound != iNumber.Match( iRecipientstring ) )
        {
        iRecipientstring = KNullDesC;
        }

    CMsgBaseControl* addressBase = iView->ControlById( iControlId );
    CMsgAddressControl* addressControl = STATIC_CAST( CMsgAddressControl*, addressBase );

    MVPbkContactLink* link= NULL;
    addressControl->Reset();
    addressControl->AddRecipientL( 
        iRecipientstring,
        iNumber,
        ETrue,
        link );

    // Message indication ( e.g. 2/32 )
#ifdef RD_MSG_NAVIPANE_IMPROVEMENT
    // Fetch pointer to the default navi pane control
    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( StatusPane( )->ControlL(  TUid::Uid( EEikStatusPaneUidNavi ) ) );
     
    if( !iNaviDecorator )
        {
        CreateViewerNaviPaneL( Document()->Entry().iDate, EMsgEditorMsgPriorityNormal, ETrue );    
        iNaviPane->PushL( *iNaviDecorator );  // <- This has nothing to do with cleanup-stack!    
        }
    else
        {
        //We end up here when fast msg opening is enabled
        CMsgNaviPaneControl* naviControl = static_cast<CMsgNaviPaneControl*>
            ( iNaviDecorator->DecoratedControl() );
        naviControl->SetTimeIndicatorL( Document()->Entry().iDate, ETrue );
        naviControl->SetNavigationIndicatorL( Document()->Session(), Document()->Entry() );
        }

#else
    UpdateNaviPaneL();
#endif
    //remove buttons from navipane
    if( iNaviDecorator && iClass0CBA )
        {
        iNaviDecorator->SetScrollButtonDimmed( CAknNavigationDecorator::ELeftButton, ETrue );
        iNaviDecorator->SetScrollButtonDimmed( CAknNavigationDecorator::ERightButton, ETrue );
        }

#ifdef RD_SCALABLE_UI_V2
    if ( AknLayoutUtils::PenEnabled() )
        {
        if ( iToolbar )
            {
            //What buttons to dim
            switch ( iSmsHeader->Type() )
                {                    
                //Received Message(Inbox navigation)
                case CSmsPDU::ESmsDeliver:
                    {
                    //Dim reply item if sender is unknown
                    if (msvEntry.iDetails.Length() == 0 || !iFlags.iValidSenderNumber )
                        {
                        iToolbar->SetItemDimmed(ESmsViewerToolbarReply, ETrue, ETrue);
                        }
                    else
                        {
                        iToolbar->SetItemDimmed(ESmsViewerToolbarReply, EFalse, ETrue);
                        }
                    
                    break;
                    }
                
                //Sent Message (Sent folder navigation)
                case CSmsPDU::ESmsSubmit:
                    {
                    //Dim the reply button, we would be replying to ourselves
                    iToolbar->SetItemDimmed(ESmsViewerToolbarReply, ETrue, ETrue);
                    break;
                    }

                default:
                    {
                    Panic( EMsgSmsViewerUnknownSmsPduType );
                    break;
                    }
                }   // Switch
            }   // if ( iToolbar )
        }     // if ( AknLayoutUtils::PenEnabled() )
#endif /* RD_SCALABLE_UI_V2 */
    //Reset the focus to "from address" feild
    iView->SetFocus( iControlId );
    
    // Set the state of automatic highlighting for sms
    SetAutomaticHighlightL( iFlags.iAutomaticHlValue );
    
    iFullyConstructed = ETrue;
    CleanupStack::PopAndDestroy();// CAknInputBlock
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::QuickSmsMsgNaviHandlingL
// This will set the body text and update the control fields if any changes
// as compared to previous SMS message. 
// This function is called only for non-bio text messages for fast preview during navigation
// ---------------------------------------------------------
void CMsgSmsViewerAppUi::QuickSmsMsgNaviHandlingL( CMsvStore* aStore )
    {
    // Initialize the richtext object...
    CParaFormatLayer* paraFormat = iEikonEnv->SystemParaFormatLayerL(); // <- pointer only
    CCharFormatLayer* charFormat = iEikonEnv->SystemCharFormatLayerL(); // <- pointer only
    CRichText* textBody = CRichText::NewL( paraFormat, charFormat );
    CleanupStack::PushL( textBody );
    // ...and read bodytext
    aStore->RestoreBodyTextL( *textBody );
    
    // message is ordinary sms message
    // so, let's get message body to viewer. 
    CMsgBaseControl* baseControl = iView->ControlById( EMsgComponentIdBody );
    CMsgBodyControl* bodyControl = STATIC_CAST( CMsgBodyControl*, baseControl );
    
    CMsgTextUtils::ConvertLineBreaksL( *textBody, CMsgTextUtils::ECRLFtoLF );
	//First resetting the body control content
    bodyControl->Reset();
    // Set body text and update the screen
    bodyControl->SetTextContentL( *textBody );
    bodyControl->SetPlainTextMode( ETrue );
    bodyControl->SetCursorPosL( 0 );
    bodyControl->DrawNow();
    CleanupStack::PopAndDestroy(); //textBody
    
    // Class 0 handling...
    TSmsDataCodingScheme::TSmsClass smsClass;
    if ( iSmsHeader->Message().SmsPDU().Class( smsClass ) 
        && smsClass == TSmsDataCodingScheme::ESmsClass0 )
        {
        // Create new softkeys
        iClass0CBA = Cba(); // <- pointer only
        if (iClass0CBA) 
            { // iClass0CBA shouldn't be NULL, but if....
            iClass0CBA->SetCommandSetL( R_SMSVIEWER_CLASS0_SOFTKEYS_OPTIONS_EXIT__CONTEXTOPTIONS );  
            //Middle softkey
            MenuBar()->SetContextMenuTitleResourceId( R_SMSV_OPTIONSCONTEXTMENUBAR_CLASS0 );           
            MenuBar()->SetMenuTitleResourceId( R_SMSV_OPTIONSMENUBAR_CLASS0 );        
            MenuBar()->SetMenuType(CEikMenuBar::EMenuOptions);     
            }
        }
    else if ( iClass0CBA ) 
        {
        //menubar and CBA is reset to the default resource. 
        //if iClass0CBA is not NULL, then resource id has been changed, hence needs to be reset 
        //setting the CBA to default
        iClass0CBA->SetCommandSetL( R_SMSVIEWER_SOFTKEYS_OPTIONS_BACK__CONTEXTOPTIONS );
        //Since CBA is set to default iClass0CBA is set NULL
        iClass0CBA = NULL; 
        //Setting the menubar to default
        MenuBar()->SetContextMenuTitleResourceId( R_SMSV_CONTEXTMENUBAR  );           
        MenuBar()->SetMenuTitleResourceId( R_SMSV_OPTIONSMENUBAR );        
        MenuBar()->SetMenuType(CEikMenuBar::EMenuOptions);
        }
    
    // Deciding between To/From text in address control
    const CSmsPDU::TSmsPDUType pduType = iSmsHeader->Type();
    switch ( pduType )
        {
        case CSmsPDU::ESmsDeliver:
            {
            // Setting from-control
            iControlId = EMsgComponentIdFrom;
            if(pduType!= iTypeMsg)
                {
                	iView->DeleteControlL(EMsgComponentIdTo);
                	iView->AddControlFromResourceL( 
                	R_SMSV_FROM, 
                	EMsgAddressControl, 
                	EMsgAppendControl, 
                	EMsgHeader );
                }            
            // take the number and verify it
            iNumber = iSmsHeader->FromAddress();
            if ( !iFlags.iIsEmailMessage )
                {
                iFlags.iValidSenderNumber = CommonPhoneParser::IsValidPhoneNumber(
                    iNumber, CommonPhoneParser::ESMSNumber );

                if( !iFlags.iValidSenderNumber && iMsgVoIPExtension->IsVoIPSupported() )
                    {
                    if( MsvUiServiceUtilities::IsValidEmailAddressL( iNumber ) )
                        {
                        iFlags.iValidSenderNumber = ETrue;
                        iFlags.iVoIPNumber = ETrue;
                        }
                    }
                
                // Replypath indication
                CMsgExpandableControl* replyPathControl = STATIC_CAST(CMsgExpandableControl*,
                        iView->ControlById(EMsgComponentIdCc));
                if ( iSmsHeader->Deliver().ReplyPath() )
                    {
                    if( !replyPathControl )
                        {
                        iView->AddControlFromResourceL( 
                            R_SMSV_REPLYPATH, 
                            EMsgExpandableControl, 
                            EMsgAppendControl, 
                            EMsgHeader );
                        }
                
                    HBufC* text = StringLoader::LoadLC( R_QTN_SMS_HEADING_REPLY_PATH, iCoeEnv );
                    CRichText& replyPath = STATIC_CAST(CMsgExpandableControl*, 
                        iView->ControlById(EMsgComponentIdCc))->TextContent();
                    replyPath.Reset();
                    replyPath.InsertL( 0, *text ); 
                    CleanupStack::PopAndDestroy(); // text
                    }
                else if( replyPathControl )
                    {
                    // No reply path in curr SMS, if there was one in prev, remove the control
                    iView->DeleteControlL( EMsgComponentIdCc );
                    }
                }
            else
                {
                iFlags.iValidSenderNumber =
                    MsvUiServiceUtilities::IsValidEmailAddressL(
                        iRecipientstring );
                }
            }
            break;
        case CSmsPDU::ESmsSubmit:
            // Setting To-control
            iControlId = EMsgComponentIdTo;
            if(pduType!= iTypeMsg)
              {
                iView->DeleteControlL(EMsgComponentIdFrom);
                iView->AddControlFromResourceL( 
                R_SMSV_TO, 
                EMsgAddressControl, 
                EMsgAppendControl, 
                EMsgHeader );
              }            
            // take the number
            iNumber = iSmsHeader->FromAddress();
            break;
        default:
#if defined ( _DEBUG )
            Panic( EMsgSmsViewerUnknownSmsPduType );
#else
            User::Leave( KErrNotSupported );
#endif
            break;
        }
    // If we are dealing with Email message
    // we add subject control for Email type subject
    if ( iFlags.iIsEmailMessage )
        {
        TPtrC subj( iSmsHeader->EmailFields().Subject() );
        if ( subj.Length() )
            {
            // Add subject control if not present
            AddSubjectControlL();
            // Set the Subject field text accordigly
            SetSubjectL( subj );
            }
        }
    else if( iFlags.iEmailSubjectControlPresent  && SubjectControl() )
        {
        //remove subject control if in case prev messages is email over SMS message with subject
        iView->DeleteControlL( EMsgComponentIdSubject );
        iFlags.iEmailSubjectControlPresent = EFalse;
        }
    iTypeMsg = pduType; //for successful launch save the PDU Type.
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::FindItemHighlighted
// Checks wheter current find item is highlighted
// ---------------------------------------------------------
//
TBool CMsgSmsViewerAppUi::FindItemHighlighted()
    {
    if ( iView->ItemFinder() && 
         iView->ItemFinder()->CurrentSelection().Length() )
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CMsgSmsViewerAppUi::FocusedControlId
// Returns id of focused control
// ---------------------------------------------------------
//
TInt CMsgSmsViewerAppUi::FocusedControlId( )
    {
    TInt controlType = EMsgComponentIdNull;
    if ( iView && iView->FocusedControl( ) )
        {
        controlType = iView->FocusedControl( )->ControlId( );
        }
    return controlType;
    }

//  End of File  
