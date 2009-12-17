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
*       Provides CNotViewerAppUi class methods. 
*
*/



#include <featmgr.h>                        // FeatureManager
#include <bldvariant.hrh>
#include <messagingvariant.hrh>
#include <txtrich.h>                        // CRichText
#include <eikrted.h>                        // CEikRichTextEditor
#include <StringLoader.h>                   // StringLoader (load and format strings from resources)

#include <centralrepository.h>    // link against centralrepository.lib
#include <CommonUiInternalCRKeys.h>
#include <messaginginternalcrkeys.h> // for Central Repository keys
#include <CoreApplicationUIsSDKCRKeys.h>

#include <AknFepGlobalEnums.h>              // Global Fep enums (ELatin, ENumber)

// Base editor stuff
#include <MsgBodyControl.h>                 // CMsgBodyControl
#include <MsgEditorView.h>                  // CMsgEditorView
#include <MsgAddressControl.h>              // CMsgAddressControl
#include <MsgAttachmentUtils.h>             // CMsgAttachmentUtils
#include <MsgEditorAppUiExtension.h>        // CMsgEditorAppUiExtension

// Help
#include <csxhelp/mms.hlp.hrh>
#include <hlplch.h>                         // For HlpLauncher 

// Phone
#include <commonphoneparser.h>              // Common phone number validity checker

// Contacts
#include <cntdef.h>                         // KNullContactId

// MUIU stuff
#include <MuiuMsvUiServiceUtilities.h>      // Call back to sender
#include <muiumsvuiserviceutilitiesinternal.h>

#include <MuiuOperationWait.h>              // CMuiuOperationWait
#include <akninputblock.h>                  // CAknInputBlock
#include <MuiuMsvProgressReporterOperation.h> // CMsvProgressReporterOperation
#include <MuiuMsgEmbeddedEditorWatchingOperation.h>

// AKN Stuff
#include <AknUtils.h>                       // Apac, layout, etc
#include <AknsUtils.h>						// AknsUtils
#include <aknnotewrappers.h>                // CAknInformationNote
#include <applayout.cdl.h> // LAF
#include <aknlayoutscalable_avkon.cdl.h>

// Client MTM Stuff
#include <mmsnotificationclient.h>          // CMmsClientMtm
#include <mtmuidef.hrh>                     // EMtmUiFlagEditorPreferEmbedded
#include <mmsgenutils.h>                    // TMmsGenUtils
#include <mmsmsventry.h>                    // TMmsMsvEntry
#include <MtmExtendedCapabilities.hrh>      // function id of MessageInfo

// Send As Stuff
#include <sendui.h>                         // SendUI (for Reply via SMS)
#include <SenduiMtmUids.h>                  // MTM Uids
#include <CMessageData.h>

// Find item
#include <FindItemmenu.rsg>                 // Find item stuff
#include <finditemmenu.h>                   //
#include <finditemdialog.h>                 //
#include <finditem.hrh>
#include <ItemFinder.h>

#ifdef RD_SCALABLE_UI_V2
#include <akntoolbar.h>
#endif

#include <aknnavi.h>                        // CAknNavigationControlContainer
#include <aknnavide.h>                      // CAknNavigationDecorator
#include <akntabgrp.h>                      // CAknTabGroup
#include <akntitle.h>
#include <mmserrors.h>
#include <mmsregisteredapplications.h>
#include <mmssettings.h>

#include <mmsui.mbg>                        // Bitmap identifiers
#include <data_caging_path_literals.hrh>    // KDC_APP_BITMAP_DIR
#include "MmsMtmConst.h"
#include "NotMtmUi.h"                       // Notification MTM UI
#include "NotMtmUiData.h"                   // Notification MTM UI Data
#include "NotViewerAppUi.h"
#include "NotViewerDocument.h"
#include "NotViewer.hrh"
#include <NotViewer.rsg>
#include <uniaddresshandler.h>
#include "MmsSettingsHandler.h"
#include <msgvoipextension.h>
#include <textresolver.h>

// CONSTANTS
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
const TUint KNaviGranularity = 2;
const TInt KIconDefaultColor = 0;
#endif
_LIT( KMmsUiMbmFile, "mmsui.mbm" );

const TInt KDateSize            = 30;
const TInt KMaxDetailsLength    = 64;   // Copy max this many chars to TMsvEntry iDetails
const TInt KReplyToSenderSize = 2048;
// Exit time when using CIdle to close the application
const TInt KDelayedExitDelay = 2500000;

// ---------------------------------------------------------
// CNotViewerAppUi
// ---------------------------------------------------------
//
CNotViewerAppUi::CNotViewerAppUi() :
    iIdle( NULL ),
    iAbsorber( NULL ),
    iOperation( NULL ),
    iSenderType( EMuiuAddressTypeNone ), 
    iFlags(0),
    iMsgId( 0 )
    {
    }

// ---------------------------------------------------------
// ~CNotViewerAppUi
// ---------------------------------------------------------
//
CNotViewerAppUi::~CNotViewerAppUi()
    {
    FeatureManager::UnInitializeLib();
    delete iOperation;
    if (    iView
        &&  !( iFlags & ENotViewerIsNotification ) )
        {   // FR mode
        if ( !( iMtm->Entry( ).Entry( ).ReadOnly( ) ) )
            { // FR in edit mode
            CMsgAddressControl* to = 
                static_cast<CMsgAddressControl*>( iView->ControlById( EMsgComponentIdTo ) );
            if ( to )
                { // Lets save the current input mode to shared data
                TInt inputMode = to->Editor().AknEditorCurrentInputMode();
                iMuiuSettRepository->Set( KMuiuToInputMode, inputMode );
                }        
            }
        }
    if ( iNotifyHandler )
        {        
        iNotifyHandler->StopListening();
        }
    delete iNotifyHandler;
    delete iCUiRepository;
    delete iMuiuSettRepository;

    delete iIdle;
    delete iAbsorber;
    delete iSendUi;
    delete iFindItemMenu;
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
    delete iNaviDecorator;
#endif
    delete iMsgVoIPExtension;
	delete iAlias;
    delete iAddressHandler;
    delete iSettingsHandler;
    delete iAppIcon;
    }

// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CNotViewerAppUi::ConstructL()
    {
    LOGTEXT(_L8("NotViewerAppUi: Construct start"));
    CMsgEditorAppUi::ConstructL(); // Constructor of the base class

    // Disable task swapper from options menu during launch
    MenuBar()->SetMenuType( CEikMenuBar::EMenuOptionsNoTaskSwapper );

	FeatureManager::InitializeLibL();

	if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
		{
		iFlags |= ENotViewerFlagsHelp;
		}
		
    // KFeatureIdMmsNonDestructiveForward has been deprecated
    // Check of TMsvEntry::iMtmData2 & KMmsStoredInMMBox replaces it. 
    // MMSEngine sets KMmsStoredInMMBox for received notifications
    
    if ( FeatureManager::FeatureSupported( KFeatureIdOfflineMode ) )
        {
        iFlags |= ENotViewerOffline;
        }    
    if ( FeatureManager::FeatureSupported( KFeatureIdAudioMessaging ) )
        {
        iFlags |= ENotViewerAudioMessaging;
        }

    iMtm = &(Document()->Mtm());
    if ( !iMtm )
        {
        User::Leave( KErrBadHandle );
        }
    
    iSettingsHandler = CMmsSettingsHandler::NewL( *iMtm );

#ifndef RD_SCALABLE_UI_V2
    iView = CMsgEditorView::NewL( *this, CMsgEditorView::EMsgReadOnly );
#endif

    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    iTitlePane = static_cast<CAknTitlePane*>
        ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    iSendUi = CSendUi::NewL();

    TInt highlight = 0;
    iMuiuSettRepository = CRepository::NewL( KCRUidMuiuSettings );

    iCUiRepository = CRepository::NewL( KCRUidCommonUi );
    iCUiRepository->Get( KCuiAutomaticHighlight, highlight  );
    iNotifyHandler = CCenRepNotifyHandler::NewL( *this, 
                                                 *iCUiRepository, 
                                                 CCenRepNotifyHandler::EIntKey, 
                                                 KCuiAutomaticHighlight );
    iNotifyHandler->StartListeningL();
    
    if ( highlight == 0 )
        { // Disable autofind if it's disable in Shared Data 
        iFlags &= ~ENotViewerAutoFind;
        }
    else
        {
        iFlags |= ENotViewerAutoFind;
        }

    // No other reply types as mms is not possible
    TInt features( 0 );
    CRepository* repository = CRepository::NewL( KCRUidMuiuVariation );
    repository->Get( KMuiuUniEditorFeatures, features );
    if ( features & KUniEditorFeatureIdRestrictedReplyMms )
        {
        iFlags |= ENotViewerRestrictedReply;
        }
    delete repository;

    LOGTEXT2(_L("CNotViewerAppUi::ConstructL: iFlags 0x%x"), iFlags );

    iFindItemMenu = CFindItemMenu::NewL( EFindItemMenuPlaceHolder );
    iFindItemMenu->AttachItemFinderMenuL(0);

	if( !iEikonEnv->StartedAsServerApp( ) )
		{
	    Document()->PrepareToLaunchL( this );
		}
		
    MenuBar()->SetContextMenuTitleResourceId( R_NOTVIEWER_SELECTIONMENUBAR );

  	// Set path of bitmap file
  	TParse fileParse;
  	fileParse.Set( KMmsUiMbmFile, &KDC_APP_BITMAP_DIR, NULL );
    
    iAppIcon = AknsUtils::CreateGulIconL( 
        AknsUtils::SkinInstance(), 
        KAknsIIDQgnPropMceMmsTitle,
        fileParse.FullName(),
        EMbmMmsuiQgn_prop_mce_mms_title,
        EMbmMmsuiQgn_prop_mce_mms_title_mask );
    
    ApplyFormat(iLabelFormat, iLabelMask, ETrue);
    ApplyFormat(iValueFormat, iValueMask, EFalse);    
    LOGTEXT(_L8("NotViewerAppUi: Construct end"));
    }

// ---------------------------------------------------------
// LaunchViewL
// ---------------------------------------------------------
//
void CNotViewerAppUi::LaunchViewL()
    {
    LOGTEXT(_L8("NotViewerAppUi: LaunchViewL start"));
    
    TRAPD( err, iMtm->LoadMessageL() );

   	if ( err )
       	{
       	if ( err != KErrNoMemory )
           	{
           	User::Leave( KErrCorrupt );
           	}
       	User::Leave( err );
       	}

   	GetTypeL( ); // Updates the iFlag
    
    SetTitleIconL();   

#ifdef RD_SCALABLE_UI_V2
    if ( AknLayoutUtils::PenEnabled() )
        {
        // iToolbar should have been assigned already in HandleForegroundEventL()
        // If it failed,
        // - catch it here
        // - but it means also that some of functionality cannot be fulfilled
        if ( !iToolbar )
            {
            iToolbar = CurrentFixedToolbar();  
            if ( iToolbar )
                {
                iToolbar->SetToolbarObserver( this );
                }
            }
        }

    if (    iToolbar
        &&  iToolbar->IsToolbarDisabled() )
        {
        iToolbar->DisableToolbarL( EFalse ); 
        iToolbar->SetToolbarVisibility( ETrue, EFalse );
        }
    // Creation of CMsgEditorView must be after toolbar is set visible otherwise layout things can go wrong. 
    // This has been moved here from ConstructL(). For that reason couple of 'if ( iView &&' checks has been added 
    // at locations which could use CMsgEditorView very early at startup
    iView = CMsgEditorView::NewL( *this, CMsgEditorView::EMsgReadOnly );

#endif

    // Editors must not exit due to End key. Flag is set by GetTypeL();
    if ( ! ( iFlags & ENotViewerIsNotification ) )
        {
        SetCloseWithEndKey( EFalse );
        }
        
   	iMsgId = iMtm->Entry( ).Entry( ).Id( );

    TInt focusedControl = 0;

	if( iFlags & ENotViewerIsNotification )
    	{   // We are in the notification mode
        focusedControl = EMsgComponentIdFrom;
        iView->AddControlFromResourceL( 
        	R_NOTVIEWER_FROM, 
            EMsgAddressControl, 
            EMsgAppendControl, 
            EMsgHeader );
    	}
    else
        {   // we are in the forward request mode
        focusedControl = EMsgComponentIdTo;
        iView->AddControlFromResourceL( 
            R_NOTVIEWER_TO, 
            EMsgAddressControl, 
            EMsgAppendControl, 
            EMsgHeader );
        CMsgAddressControl* to = AddressControl( );

        // Remove the auto highlight in FR mode
        iFlags &= ~ENotViewerAutoFind;

        TMsvEntry msvEntry = iMtm->Entry( ).Entry( );

        if( msvEntry.ReadOnly( ) ) // Sent 
            {
            to->SetReadOnly( ETrue );
            }
        else    // Unsent: To-control must not be read only
            {
            to->SetReadOnly( EFalse );
            TInt inputMode = ENumber;

            if ( iMuiuSettRepository->Get( KMuiuToInputMode, inputMode ) )
                {
                inputMode = ENumber;
                }
            to->Editor( ).SetAknEditorCurrentInputMode( inputMode );
            }
        UpdateTitlePaneL( ); // Updates the "Forward Request" to Title pane
        }
        
    // Adds the text in message body here
    if( iFlags & ENotViewerIsNotification )
        { // We are in the Viewer so lets add from field
        const TPtrC sender = iMtm->Sender();
        const TPtrC details = iMtm->Entry().Entry().iDetails;
        if ( sender.Length() )
            {
            iSenderType = MsvUiServiceUtilitiesInternal::ResolveAddressTypeL(    
                TMmsGenUtils::PureAddress( sender ),
                            CommonPhoneParser::ESMSNumber );
            
            if ( iSenderType != EMuiuAddressTypePhoneNumber )
                {
                //We trust on that there won't be illegal addresses
                //in received messages
                iSenderType = EMuiuAddressTypeEmail;
                if ( iFindItemMenu )
                    {
                    iFindItemMenu->SetSenderDescriptorType( CItemFinder::EEmailAddress );
                    }
                }
            CMsgAddressControl* fromControl = AddressControl( );

            //Check if MMS engine has put an alias into iDetails field
            if ( sender.Compare( details ) != 0 )
                {
                TPtrC alias = KNullDesC();
                if ( details.Length() && !iAlias )
                    {
                    iAlias = details.AllocL( );
                    alias.Set( *iAlias );
                    }

                // Resolve whether this is remote alias. Local alias takes precedence
                TPtrC tempAlias( TMmsGenUtils::Alias( sender ) );
                if (    iAlias
                    &&  tempAlias.Length()
                    &&  !tempAlias.CompareF( *iAlias ) )
                    
                    {
                    iRemoteAlias = ETrue;
                    }                    

                MVPbkContactLink* link = NULL;
                fromControl->AddRecipientL( alias,
                    TMmsGenUtils::PureAddress( sender ),
                    ETrue,
                    link );
                }
            else
                {
                MVPbkContactLink* link = NULL;
                fromControl->AddRecipientL( TMmsGenUtils::Alias( sender ),
                    TMmsGenUtils::PureAddress( sender ),
                    ETrue,
                    link );
                }
            }
        else
            {
            // Leave the address control empty
            // This should not happen, though
            }
        }
    else
        {
        const CMsvRecipientList& addresses = iMtm->AddresseeList();

        TInt count = addresses.Count();
        CMsgAddressControl* toControl = AddressControl( );

        if ( count > 0 )
            {
            CMsgRecipientList* recipientList = CMsgRecipientList::NewL();
            CleanupStack::PushL( recipientList );

            for ( TInt i = 0; i < count; i++)
                {
                CMsgRecipientItem* recipient = CMsgRecipientItem::NewLC(
                    TMmsGenUtils::Alias( addresses[i] ),
                    TMmsGenUtils::PureAddress( addresses[i] ) );
                recipient->SetVerified( ETrue );
                recipientList->AppendL( recipient );
                CleanupStack::Pop();
                recipient = NULL;
                }
            toControl->AddRecipientsL( *recipientList );
            CleanupStack::PopAndDestroy();
            }
        if ( !iMtm->Entry().Entry().ReadOnly( ) )
            { // Address handler is created only when we have unsent Forward Request
            iAddressHandler = CUniAddressHandler::NewL( Document()->Mtm(), *toControl, *iCoeEnv );
            }
        }

    // Add the body field stuff
    if( iFlags & ENotViewerIsNotification )
        { // We are in the Viewer so lets add message field, too
        AddMessageFieldL( );
        }
    AddSubjectFieldL( );
    AddTextFieldL( );
    AddSizeFieldL( );
    if( iFlags & ENotViewerIsNotification )
        { // We are in the Viewer so lets add expiry field, too
        AddExpiryFieldL( );
        }
    AddClassFieldL( );
    AddPriorityFieldL( );

    if( iFlags & ENotViewerIsNotification )
        { // We are in the Viewer so Find stuff too
        SetFindModeL( iFlags & ENotViewerAutoFind );

        }
    else
        {
        if ( iView->ItemFinder() )
            {
	        iView->ItemFinder()->SetFindModeL(
                CItemFinder::ENoneSelected );
            }
        }

    iView->ExecuteL( ClientRect(), focusedControl );
    InitNavipaneL( );

    // Enable task swapper to options menu after launch has been completed.
    MenuBar()->SetMenuType( CEikMenuBar::EMenuOptions );


    LOGTEXT(_L8("NotViewerAppUi: LaunchViewL end"));
    }

// ---------------------------------------------------------
// DynInitMenuPaneL
// ---------------------------------------------------------
//
void CNotViewerAppUi::DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane )
    {
    if ( aMenuPane )
        {       
        TInt amountOfItems( aMenuPane->NumberOfItemsInPane() );
        if ( !Document( )->IsLaunched( )  )
            {
            if ( amountOfItems )
                {
                aMenuPane->DeleteBetweenMenuItems( 0, amountOfItems - 1 );
                }
            return;
            }
        switch ( aMenuId )
            {
            case R_NOTVIEWER_OPTIONSMENU:
                {
                DynInitOptionsMenuL( aMenuPane );
                break;
                }
            case R_NOTVIEWER_REPLY_SUBMENU:
                {
                DynInitReplyMenuL( aMenuPane );
                break;
                }
            case R_NOTVIEWER_SELECTIONMENU:
                {
                DynInitContextMenuL( aMenuPane );
                break;
                }
            case R_NOTVIEWER_ZOOM_SUBMENU:
                {
                TInt zoomLevel = KErrGeneral;
                iMsgEditorAppUiExtension->
                    iSettingCachePlugin.GetValue( KAknLocalZoomLayoutSwitch, zoomLevel );
                switch ( zoomLevel )
                    {
                    case EAknUiZoomAutomatic:
                        aMenuPane->SetItemButtonState(  EMsgDispSizeAutomatic,
                                                        EEikMenuItemSymbolOn );
                        break;
                    case EAknUiZoomNormal:
                        aMenuPane->SetItemButtonState(  EMsgDispSizeNormal,
                                                        EEikMenuItemSymbolOn );
                        break;
                    case EAknUiZoomSmall:
                        aMenuPane->SetItemButtonState(  EMsgDispSizeSmall,
                                                        EEikMenuItemSymbolOn );
                        break;
                    case EAknUiZoomLarge:
                        aMenuPane->SetItemButtonState(  EMsgDispSizeLarge,
                                                        EEikMenuItemSymbolOn );
                        break;
                    default:
                        break;
                    }
                }
                break;
            default:
                {
                // this does nothing if we're not, so this is safe
                iFindItemMenu->UpdateItemFinderMenuL( aMenuId, aMenuPane );
                break;
                }
            }
        }
    }

// ---------------------------------------------------------
// DynInitOptionsMenuL
// ---------------------------------------------------------
//
void CNotViewerAppUi::DynInitOptionsMenuL( CEikMenuPane* aMenuPane )
    {
    LOGTEXT(_L8("NotViewerAppUi: DynInitOptionsMenuL start"));
    if( !(iFlags & ENotViewerFlagsHelp ) )
        {
        aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
        }
    if (    iFlags & ENotViewerIsNotification )
        {   // Notification mode        
        if ( iView  )
            {
            if( (FocusedControlId( ) == EMsgComponentIdFrom) && ( ( iMtm->Sender( ) ).Length() ) )
                {
                iFindItemMenu->SetSenderHighlightStatus( ETrue );
                iFindItemMenu->SetSenderDisplayText( TMmsGenUtils::PureAddress( iMtm->Sender( ) ) );
                }
	        else
	     	    {
                iFindItemMenu->SetSenderHighlightStatus( EFalse );
		        }
		    iFindItemMenu->AddItemFindMenuL( 
                ( FocusedControlId() == EMsgComponentIdBody ) ? iView->ItemFinder() : 0,
    			aMenuPane, 
                EFindItemMenuPlaceHolder,
                iMtm->Sender(),
                iAlias && !iRemoteAlias ? ETrue : EFalse, //"Is sender known"
    			EFalse );
            }

        aMenuPane->SetItemDimmed( ENotViewerSend, ETrue );
        aMenuPane->SetItemDimmed( ENotViewerAddRecipient, ETrue );
        aMenuPane->SetItemDimmed( ENotViewerCheckNames, ETrue );

        CNotViewerDocument* doc = Document( );

        CNotMtmUiData* uiData = &doc->MtmUiDataL( );
        
        TMsvEntry entry = iMtm->Entry( ).Entry( );

        // Note: OperationSupportedL returns 0 if operation is supported,
        // otherwise it returns some kind of error code

        if( uiData->OperationSupportedL( KMtmUiFunctionFetchMMS, entry ) )
            {
            aMenuPane->SetItemDimmed( ENotViewerRetrieve, ETrue );
            }

        if( uiData->OperationSupportedL( KMtmUiFunctionDeleteMessage, entry ) )
            {
            aMenuPane->SetItemDimmed( ENotViewerDelete, ETrue );
            }

        if( uiData->OperationSupportedL( KMtmUiFunctionForward, entry ) )
            {
            aMenuPane->SetItemDimmed( ENotViewerForward, ETrue ); 
            }

        if( iMtm->Sender().Length()==0 )
            {
            aMenuPane->SetItemDimmed( ENotViewerReplyMenu, ETrue );
            }

        }
    else
        {   // ForwReq mode
        aMenuPane->SetItemDimmed( EFindItemMenuPlaceHolder, ETrue );
        aMenuPane->SetItemDimmed( ENotViewerRetrieve, ETrue );
        aMenuPane->SetItemDimmed( ENotViewerForward, ETrue );
        aMenuPane->SetItemDimmed( ENotViewerReplyMenu, ETrue );
        if ( iMtm->Entry( ).Entry( ).ReadOnly( ) )
            { // Sent ForwReq
            aMenuPane->SetItemDimmed( ENotViewerMessageDetails, ETrue );
            aMenuPane->SetItemDimmed( ENotViewerSend, ETrue );
            aMenuPane->SetItemDimmed( ENotViewerAddRecipient, ETrue );
            aMenuPane->SetItemDimmed( ENotViewerCheckNames, ETrue );
            }
        else
            { 
            // This is not needed when we are in edit mode  
            // Send is always visible
            aMenuPane->SetItemDimmed( ENotViewerDelete, ETrue );
            // Lets check if we need Send or not
            if ( iView  )
                {
                CMsgAddressControl* toCtrl = 
                    static_cast<CMsgAddressControl*>( iView->ControlById( EMsgComponentIdTo ) );
                if ( toCtrl && toCtrl->GetRecipientsL()->Count() < 1 )
                    {
                    // To field empty -> cannot send.
                    aMenuPane->SetItemDimmed( ENotViewerCheckNames, ETrue );
                    }            
                }
            }
        }
    LOGTEXT(_L8("NotViewerAppUi: DynInitOptionsMenuL end"));
    }

// ---------------------------------------------------------
// DynInitReplyMenuL
// ---------------------------------------------------------
//
void CNotViewerAppUi::DynInitReplyMenuL( CEikMenuPane* aMenuPane )
    {
    if( iSenderType == EMuiuAddressTypeNone )
        {   // If sender type is email, disable SMS
        aMenuPane->SetItemDimmed( ENotViewerReplyViaMessage, ETrue );
        }

    if (    ! ( iFlags & ENotViewerAudioMessaging ) 
        ||  iFlags & ENotViewerRestrictedReply
        ||  iSenderType == EMuiuAddressTypeNone )
        {   
        aMenuPane->SetItemDimmed( ENotViewerReplyViaAudio, ETrue );
        }        

    if ( iSenderType != EMuiuAddressTypeEmail   ||
         iFlags & ENotViewerRestrictedReply     ||
         MsvUiServiceUtilitiesInternal::DefaultServiceForMTML(
            Document()->Session(),
            KSenduiMtmSmtpUid,
            ETrue )
            == KMsvUnknownServiceIndexEntryId )
        {
        // "Reply via e-mail" feature
        aMenuPane->SetItemDimmed( ENotViewerReplyViaMail, ETrue );
        }
    }

// ---------------------------------------------------------
// DynInitContextMenuL
// ---------------------------------------------------------
//
void CNotViewerAppUi::DynInitContextMenuL( CEikMenuPane* aMenuPane )
    {
    LOGTEXT(_L8("NotViewerAppUi: DynInitContextMenuL start"));
    if (  iFlags & ENotViewerIsNotification )
        {   // Notification
        if ( iView  )
            {
            if( (FocusedControlId( ) == EMsgComponentIdFrom) && ( ( iMtm->Sender( ) ).Length() ) )
                {
                iFindItemMenu->SetSenderHighlightStatus( ETrue );
           	    iFindItemMenu->SetSenderDisplayText( TMmsGenUtils::PureAddress( iMtm->Sender( ) ) );
           	    }
	        else
	     	    {
                iFindItemMenu->SetSenderHighlightStatus( EFalse );
		        }
		    iFindItemMenu->AddItemFindMenuL( 
                ( FocusedControlId() == EMsgComponentIdBody ) ? iView->ItemFinder() : 0,
    			aMenuPane, 
                EFindItemContextMenuPlaceHolder,
                iMtm->Sender(),
                iAlias && !iRemoteAlias ? ETrue : EFalse, //"Is sender known"
    			ETrue );
            }

        if( iSenderType == EMuiuAddressTypeNone )
            {   // If sender type is none, disable Reply menu
            aMenuPane->SetItemDimmed( ENotViewerReplyMenu, ETrue );
            }
        aMenuPane->SetItemDimmed( ENotViewerSend, ETrue );
        aMenuPane->SetItemDimmed( ENotViewerAddRecipient, ETrue );

        CNotViewerDocument* doc = Document( );

        CNotMtmUiData* uiData = &doc->MtmUiDataL( );

        TMsvEntry entry = iMtm->Entry( ).Entry( );

        // Note: OperationSupportedL returns 0 if operation is supported,
        // otherwise it returns some kind of error code

        if( uiData->OperationSupportedL( KMtmUiFunctionFetchMMS, entry ) )
            {
            aMenuPane->SetItemDimmed( ENotViewerRetrieve, ETrue );
            }
        }
    else
        {   // ForwReq
        aMenuPane->SetItemDimmed( ENotViewerRetrieve, ETrue );
        aMenuPane->SetItemDimmed( ENotViewerReplyMenu, ETrue );
        aMenuPane->SetItemDimmed( EFindItemContextMenuPlaceHolder, ETrue );
        if ( iMtm->Entry( ).Entry( ).ReadOnly( ) )
            {
            aMenuPane->SetItemDimmed( ENotViewerSend, ETrue );
            aMenuPane->SetItemDimmed( ENotViewerAddRecipient, ETrue );
            }
        // else - send key is always available
        }
    LOGTEXT(_L8("NotViewerAppUi: DynInitContextMenuL end"));
    }

// ---------------------------------------------------------
// HandleKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CNotViewerAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    LOGTEXT(_L8("NotViewerAppUi: HandleKeyEventL start"));
    if ( !Document()->IsLaunched() || !iView )
        {
        return EKeyWasNotConsumed;
        }
    if ( aType != EEventKey )
        {
        return EKeyWasNotConsumed;
        }
    TMsvEntry msvEntry = iMtm->Entry( ).Entry( );

    switch ( aKeyEvent.iCode )
        {
        case EKeyBackspace:
            {
            if ( msvEntry.ReadOnly( ) || iFlags & ENotViewerIsNotification )
                { // We have either a notification or sent FR open
                CNotViewerDocument* doc = Document( );
                CNotMtmUiData* uiData = &doc->MtmUiDataL( );
                TMsvEntry entry = iMtm->Entry( ).Entry( );
                if( !uiData->OperationSupportedL( KMtmUiFunctionDeleteMessage, entry ) )
                    {
                    DeleteNotificationL( );
                    }
                // We can consume this because Delete is the only possible function here
                return EKeyWasConsumed;
                }
            // else we need to be able to delete characters in To field
            break;
            }
        case EKeyRightArrow:
            {
            if ( msvEntry.ReadOnly( ) || iFlags & ENotViewerIsNotification )
                { // We have either a notification or sent FR open
                if(     iNaviDecorator
                    &&  IsNextMessageAvailableL( ETrue ) )
                    {
                    NextMessageL( ETrue );
                    }
                // We can consume this because Next msg is the only possible function here
                return EKeyWasConsumed;
                }
            // else we need to be able to move inside To field
            break;
            }
        case EKeyLeftArrow:
            {
            if ( msvEntry.ReadOnly( ) || iFlags & ENotViewerIsNotification )
                { // We have either a notification or sent FR open
                if(     iNaviDecorator
                    &&  IsNextMessageAvailableL( EFalse ) )
                    {
                    NextMessageL( EFalse );
                    }
                // We can consume this because Next msg is the only possible function here
                return EKeyWasConsumed;
                }
            // else we need to be able to move inside To field
            break;
            }
    
        case EKeyDevice3:       //Selection key
        case EKeyEnter:         //Enter Key
            {
            if ( DoSelectionKeyL( ) )
                {
                return EKeyWasConsumed;
                }
            break;  //Lint e527
            }
        case EKeyYes:           //Send key
            {
            DoHandleSendKeyL( );
            return EKeyWasConsumed;           
            }
        default:
            break;
        }
    return iView->OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// HandleCommandL
// ---------------------------------------------------------
//
void CNotViewerAppUi::HandleCommandL( TInt aCommand )
    {
    LOGTEXT(_L8("NotViewerAppUi: HandleCommandL start"));
    if ( aCommand != EEikCmdExit &&
        ! Document()->IsLaunched() )
        {
        return;
        }
    if ( iFindItemMenu && iFindItemMenu->CommandIsValidL( aCommand ) )
        {
        iFindItemMenu->HandleItemFinderCommandL( aCommand );
        return;
        }
    switch ( aCommand )
        {
        case ENotViewerRetrieve:
            {
            FetchL( );
            break;
            }
        case ENotViewerForward:
            {
            DoForwardL( );
            break;
            }
        case ENotViewerSend:
            {
            DoHandleSendKeyL( );
            break;
            }
        case ENotViewerReplyViaMessage:
        case ENotViewerReplyViaAudio:
        case ENotViewerReplyViaMail:
            {
            UniReplyL( aCommand );
            break;
            }
        case ENotViewerAddRecipient:
            {
            AddRecipientL( );
            break;
            }
        case ENotViewerCheckNames:
            {
            CheckNamesL( );
            break;
            }
        case ENotViewerDelete:
            {
            DeleteNotificationL( );
            break;
            }
        case ENotViewerMessageDetails:
            {
            MessageDetailsL( );
            break;
            }
        case EMsgDispSizeAutomatic:
        case EMsgDispSizeLarge:
        case EMsgDispSizeNormal:
        case EMsgDispSizeSmall:
            HandleLocalZoomChangeL( (TMsgCommonCommands)aCommand );
            break;
        case EAknCmdHelp:
            {
            LaunchHelpL( );
            break;
            }
        case EEikCmdExit:
            iExitMode = MApaEmbeddedDocObserver::TExitMode(CMsgEmbeddedEditorWatchingOperation::EMsgExternalExit);        
            // Let it flow thru..
        case EAknSoftkeyBack:
        case EAknSoftkeyClose:
        case EAknSoftkeyCancel:
            // system exit
            Exit( aCommand );
            break;
        default:
            break;
        }
    LOGTEXT(_L8("NotViewerAppUi: HandleCommandL end"));
    }

// ---------------------------------------------------------
// DoSelectionKeyL
// ---------------------------------------------------------
TBool CNotViewerAppUi::DoSelectionKeyL()
    {
    if ( iView )
        {
        CMsgBaseControl* ctrl = iView->FocusedControl();

        if ( ctrl )
            {
            switch ( FocusedControlId() )
                {
                case EMsgComponentIdTo:
                    {
                    if( iFlags & ENotViewerIsNotification ) 
                        {   // We have a notification, we should handle recipients
                        CMsgAddressControl* to = AddressControl( );
                        // Check is there any recipients in address ctrl
                        TBool modified = EFalse;
                        if ( to->GetRecipientsL()->Count() )
                            {
                            // Recipients found. Verify addresses.
                            if ( !VerifyAddressesL( modified ) )
                                {
                                // Illegal address found.
                                modified = ETrue;
                                }
                            }
                        if ( !modified )//else
                            {
                            // Nothing changed on the UI. Open PhoneBook.
                            AddRecipientL();
                            }
                        break; // break only if notification
                        }
                    else // If we are in Forward Request, it's ok to go down to menubar
                        {
                        return EFalse;
                        }
                    }
                case EMsgComponentIdFrom:
                case EMsgComponentIdBody:
                    {
                    MenuBar()->SetMenuTitleResourceId( R_NOTVIEWER_SELECTIONMENUBAR );
                    MenuBar()->SetMenuType(CEikMenuBar::EMenuContext);
                    TRAPD( err, MenuBar()->TryDisplayMenuBarL() );
                    MenuBar()->SetMenuTitleResourceId( R_NOTVIEWER_MENU );
                    MenuBar()->SetMenuType(CEikMenuBar::EMenuOptions);
                    User::LeaveIfError( err );
                    }
                    break;
                            
                default:
                    break;
                }
            }
        }
    return ETrue;
    }

// ---------------------------------------------------------
// DoHandleSendKeyL
// ---------------------------------------------------------
//
void CNotViewerAppUi::DoHandleSendKeyL()
    {
    if( iFlags & ENotViewerIsNotification ) 
        {   // We have a notification, we should call to sender
        CallToNumberL( );
        }
    else if ( !iMtm->Entry( ).Entry( ).ReadOnly( ) )                                 
        {   // We are not-readonly Forward Request -> Send is possible

        // Disable dialer
        iAvkonAppUi->SetKeyEventFlags( 
              CAknAppUiBase::EDisableSendKeyShort | 
              CAknAppUiBase::EDisableSendKeyLong );
              
        // CallCreationKey: Send message if recipients.
        //                  Otherwise fetch recipients
        CMsgAddressControl* to = AddressControl();

        if ( to->GetRecipientsL()->Count() > 0 )
            {
            // has addresses -> Send
            SendL();
            }
        else
            {
            AddRecipientL();
            }
        }
    }

// ---------------------------------------------------------
// FetchL
// ---------------------------------------------------------
//
void CNotViewerAppUi::FetchL( )
    {
    LOGTEXT(_L8("NotViewerAppUi: Fetch start"));
    if( !(iFlags & ENotViewerIsNotification) )
        {   // We are in forward request
        return;
        }

    if(     iFlags & ENotViewerOffline 
        &&  MsvUiServiceUtilitiesInternal::IsPhoneOfflineL() )
        {   // We are in offline at the moment -> Infonote & exit
        ShowInformationNoteL( R_NOTVIEWER_OFFLINE_NOT_POSSIBLE, EFalse );
		DoDelayedExitL( KDelayedExitDelay );
		return;
        }

    if( !CheckAPSettingsL( ) )
        {
        return;
        }
    TTime currentTime;
    currentTime.HomeTime( );
    TTime expiryTime = iMtm->ExpiryDate( );
	TLocale locale;
    expiryTime += locale.UniversalTimeOffset();
	if (locale.QueryHomeHasDaylightSavingOn())          
		{
		TTimeIntervalHours daylightSaving(1);          
		expiryTime += daylightSaving;
		}

    if( currentTime > expiryTime )
        {   // Message is expired
        if ( !ShowConfirmationQueryL( R_NOTVIEWER_QUEST_MSG_EXPIRED ) )
            {   // User did not want to fetch expired message
            return;
            }
        }

    CAknInputBlock* abs = CAknInputBlock::NewLC( );  

    ShowInformationNoteL( R_NOTVIEWER_RETRIEVING_MESSAGE, EFalse );

    TMsvId entryId = iMtm->Entry( ).Entry( ).Id( );
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();

    CMsvEntrySelection* sel = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( sel );
	sel->AppendL( entryId );    
    
    CMsvOperation* op = iMtm->FetchMessagesL( *sel, wait->iStatus );

    if( !op )
        {  
        CleanupStack::PopAndDestroy( sel ); // selection
        CleanupStack::PopAndDestroy( wait ); // wait
        CleanupStack::PopAndDestroy( abs ); // abs
        return; 
        }

    CleanupStack::PushL( op );
    wait->Start();

    // Lets ignore the return value of wait

    CleanupStack::PopAndDestroy( op ); // op 
    CleanupStack::PopAndDestroy( sel ); // selection
    CleanupStack::PopAndDestroy( wait ); // wait
    CleanupStack::PopAndDestroy( abs ); // abs
    LOGTEXT(_L8("NotViewerAppUi: Fetch end"));
    // Let information note time to exit
    DoDelayedExitL( KDelayedExitDelay );
    }

// ---------------------------------------------------------
// DoForwardL
// ---------------------------------------------------------
//
void CNotViewerAppUi::DoForwardL( )
    {
    LOGTEXT(_L8("NotViewerAppUi: ForwardL start"));
    if( !(iFlags & ENotViewerIsNotification) )
        {   // We are in forward request
        return;
        }

    // Lets check the disk space
    if ( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL(
        Document()->Session( ),
        KReplyToSenderSize ) )
        {
        User::Leave( KErrDiskFull );
        }

    if( !CheckAPSettingsL( ) )
        {
        return;
        }

    TTime currentTime;
    currentTime.HomeTime( );
    TTime expiryTime = iMtm->ExpiryDate( );
	TLocale locale;
    expiryTime += locale.UniversalTimeOffset();
	if (locale.QueryHomeHasDaylightSavingOn())          
		{
		TTimeIntervalHours daylightSaving(1);          
		expiryTime += daylightSaving;
		}

    if( currentTime > expiryTime )
        {   // Message is expired
        if ( !ShowConfirmationQueryL( R_NOTVIEWER_QUEST_MSG_EXPIRED ) )
            {   
            // User did not want to forward expired message
            return;
            }
        }
    TMsvEntry entry = iMtm->Entry( ).Entry( );
    if( ! ( entry.iMtmData2 & KMmsStoredInMMBox ) )
        {   // Query, if multiple forward is not possible
        if ( !ShowConfirmationQueryL( R_NOTVIEWER_QUEST_FORWARD_NOTIF ) )
            {   
            // User did not want forward message, as it cannot not be fetched to the phone after that
            return;
            }
        }

    TRAPD ( error,
        {
        ForwardL( Document()->DefaultMsgFolder() );  //Draft
        } ); //end TRAP
    if ( error )
        {
        User::Leave( error );
        }
    iMtm->Entry( ).SetEntryL( iMsgId );
    TRAP_IGNORE( iMtm->LoadMessageL( ) );
    EntryChangedL( );
    LOGTEXT(_L8("NotViewerAppUi: ForwardL end"));
    }

// ---------------------------------------------------------
// SendL
// ---------------------------------------------------------
//
void CNotViewerAppUi::SendL( )
    {
    LOGTEXT(_L8("NotViewerAppUi: SendL start"));
    if( iFlags & ENotViewerIsNotification )
        {   // We are in notification
        return;
        }
    TBool dummy;
    if ( !VerifyAddressesL( dummy ) )
        {
        return;
        }

    // For eating key presses
    CAknInputBlock* abs = CAknInputBlock::NewLC( );

    DoSaveL( );

    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
    CMsvOperation* op = iMtm->SendL( wait->iStatus );

    if( !op )
        {
        CleanupStack::PopAndDestroy( wait );
        CleanupStack::PopAndDestroy( abs ); 
        return;
        }

    CleanupStack::PushL( op );
    wait->Start();
   
    // Let's ignore the return value of wait

    CleanupStack::PopAndDestroy( 2 ); // op, wait

    CleanupStack::PopAndDestroy( abs ); 
    if(     iFlags & ENotViewerOffline 
        &&  MsvUiServiceUtilitiesInternal::IsPhoneOfflineL() )
        {   // We are in offline at the moment -> Infonote
        ShowInformationNoteL( R_NOTVIEWER_SAVED_OUTBOX, EFalse );
        DoDelayedExitL( KDelayedExitDelay  );
        LOGTEXT(_L8("NotViewerAppUi: Offline SendL end"));
        return;
        }

    LOGTEXT(_L8("NotViewerAppUi: SendL end"));
    Exit( EAknSoftkeyBack );
    }

// ---------------------------------------------------------
// UniReplyL
// ---------------------------------------------------------
//
void CNotViewerAppUi::UniReplyL( TInt aCommand )
    {
    LOGTEXT(_L8("NotViewerAppUi: ReplyL start"));
    if( !(iFlags & ENotViewerIsNotification) )
        {   // We are in forward request
        return;
        }

    // Lets check the disk space
    if ( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL(
        Document()->Session( ),
        KReplyToSenderSize ) )
        {
        User::Leave( KErrDiskFull );
        }

    CMessageData* msgData = CMessageData::NewLC();

    const TPtrC sender = iMtm->Sender();
    
    if ( sender != KNullDesC() )
        {
		TMsvEntry entry = iMtm->Entry().Entry();

	    if ( sender.Compare( entry.iDetails ) )
	        {
	        msgData->AppendToAddressL( TMmsGenUtils::PureAddress( sender ), *iAlias );
	        }
	    else
	        {
		    msgData->AppendToAddressL( 	TMmsGenUtils::PureAddress( sender ), 
		        						TMmsGenUtils::Alias( sender ) );
	        }
        }
        
    // audio message has not subject field
    if( aCommand != ENotViewerReplyViaAudio ) 
        {
        HBufC* prefixSubject = CreateSubjectPrefixStringL( iMtm->SubjectL(), ETrue );
        CleanupStack::PushL( prefixSubject ); 
        if ( prefixSubject )
            {
            msgData->SetSubjectL( prefixSubject );
            }
        else
            {
            TPtrC subjectDes = iMtm->SubjectL();
            msgData->SetSubjectL( &subjectDes );
            }
        CleanupStack::PopAndDestroy( prefixSubject ); 
        }


    TUid mtmUid = KSenduiMtmUniMessageUid;
    switch ( aCommand )
        {
        case ENotViewerReplyViaAudio:
            mtmUid = KSenduiMtmAudioMessageUid;
            break;
        case ENotViewerReplyViaMail:
            mtmUid = KSenduiMtmSmtpUid;
            break;
        case ENotViewerReplyViaMessage:
        default:
	     if( iFlags & ENotViewerRestrictedReply  )
	     {
            mtmUid = KSenduiMtmMmsUid;
	     }
            break;
        }
        
    // And do the sending
    TInt error;
    TRAP ( error,
            {
            iSendUi->CreateAndSendMessageL(
            mtmUid,
            msgData);
            } ); //TRAP

    CleanupStack::PopAndDestroy( msgData ); // msgData

    if ( error )
        {
        iMtm->Entry( ).SetEntryL( iMsgId );
        TRAP_IGNORE( iMtm->LoadMessageL() );
        EntryChangedL( );
        LOGTEXT(_L8("NotViewerAppUi: Reply end error"));
        User::Leave( error );
        }
	else
		{
        LOGTEXT(_L8("NotViewerAppUi: Reply end success"));
		if (!(iEditorBaseFeatures & EStayInViewerAfterReply))
            {
            Exit( EAknSoftkeyBack );
            }
		}
    }

// ---------------------------------------------------------
// DeleteNotificationL
// ---------------------------------------------------------
//
void CNotViewerAppUi::DeleteNotificationL()
    {
    LOGTEXT(_L8("NotViewerAppUi: Delete start"));
    __ASSERT_DEBUG( iOperation == NULL, Panic( ENotViewerOperationNotFinished ) );
    CMsgEditorDocument* doc = Document( );
    CNotMtmUi* uiMtm = static_cast<CNotMtmUi*>( &doc->MtmUi( ) );


    if( iFlags & ENotViewerIsNotification )
        { // This is a notification -> lets see if we have options
        LOGTEXT(_L8("NotViewerAppUi: Delete: We r dealing with a notification"));

        TBool inboxNotification = EFalse;   

        if( iMtm->Entry().Entry().Parent() == KMsvGlobalInBoxIndexEntryId )
            {// Are we in Inbox now
            inboxNotification = ETrue;
            LOGTEXT(_L8("NotViewerAppUi: Delete: in Inbox"));
            }
          
        if( inboxNotification && uiMtm->DeleteQueryRequiredByMtm( ) && !AlreadyDeletedFromServer( ) )
            { // We are in Inbox, delete options supported and not deleted from server
            LOGTEXT(_L8("NotViewerAppUi: Delete: Lets show the delete options"));
            TInt index = 0;

            CAknListQueryDialog* dlg = new (ELeave) CAknListQueryDialog( &index );
            dlg->PrepareLC( R_NOTVIEWER_DELETE_FROM_QUERY );

            // Populate list query array
            CDesCArrayFlat *array = new(ELeave) CDesCArrayFlat(2);
            CleanupStack::PushL(array);
            HBufC* buf = StringLoader::LoadLC( R_NOTVIEWER_DELETE_QUERY_REMOTEONLY );
            array->AppendL( *buf );
            CleanupStack::PopAndDestroy(); //buf
            buf = StringLoader::LoadLC( R_NOTVIEWER_DELETE_QUERY_BOTHLOCALANDREMOTE );
            array->AppendL( *buf );
            CleanupStack::PopAndDestroy(); //buf
            dlg->SetItemTextArray( array );
            CleanupStack::Pop(); //array

            LOGTEXT(_L8("NotViewerAppUi: Delete: Next asks the delete options"));
            TInt queryOk = dlg->RunLD();
            if( queryOk )
                {
                /* list query approved, index is usable to see what list item was selected */
                if( index == 0 )
                    {
                    iFlags &= ~ENotViewerDeleteBoth;
                    LOGTEXT(_L8("NotViewerAppUi: Delete: Just the server"));
                    }
                else
                    { // If action withe server was selected, keep it in mind
                    iFlags |= ENotViewerDeleteBoth;
                    LOGTEXT(_L8("NotViewerAppUi: Delete: Both ones"));
                    }
                // The deletion process starts a bit later
                }
            else
                {
                // list query was cancelled -> just return
            LOGTEXT(_L8("NotViewerAppUi: Delete: Query cancelled"));
                return;
                }
            }
        else
            { // In this case we need just a query "Delete message?"
            LOGTEXT(_L8("NotViewerAppUi: Delete: Show just Delete message?"));
            if( ShowConfirmationQueryL( R_NOTVIEWER_QUEST_DELETE_MESSAGE ) )
                { // Delete message confirmed
                LOGTEXT(_L8("NotViewerAppUi: Delete: Answered Yes"));
                if( inboxNotification )
                    {
                    // We are in Inbox, so it means we delete just the local one
                    LOGTEXT(_L8("NotViewerAppUi: Delete: Inbox notification again"));
                    CAknInputBlock* abs = CAknInputBlock::NewLC( );  
    
                    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
                    CleanupStack::PushL( selection );
                    selection->AppendL( iMtm->Entry().EntryId() );
                
                    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();

                    LOGTEXT(_L8("NotViewerAppUi: Delete: Inbox locl deletion starts"));
                    CMsvOperation* deleteop = iMtm->DeleteNotificationL(
                                        *selection,
                                        EMmsDeleteNotificationOnly,
                                        wait->iStatus );

                   LOGTEXT(_L8("NotViewerAppUi: Delete: and goes on.."));
                   if( !deleteop )   // Check this just in case
                        {  
                        CleanupStack::PopAndDestroy( wait ); 
                        CleanupStack::PopAndDestroy( selection  ); 
                        CleanupStack::PopAndDestroy( abs );
                        LOGTEXT(_L8("NotViewerAppUi: Delete: NULL returned"));
                        return; 
                        }

                    CleanupStack::PushL( deleteop );
                    wait->Start();
    
                    if( wait->iStatus != KErrNone )
                        {
                        LOGTEXT(_L8("NotViewerAppUi: Delete: Wait->iStatus != KErrNone "));
                        CleanupStack::PopAndDestroy( 3 ); // op, wait, sel
                        CleanupStack::PopAndDestroy( abs );
                        return;
                        }

                    // -> no error, we can exit the viewer
                    CleanupStack::PopAndDestroy( 3 ); // op, wait, sel
                    CleanupStack::PopAndDestroy( abs );
                    // This was the local deletion so lets just exit from here
                    LOGTEXT(_L8("NotViewerAppUi: Delete: Local delete succesfull - exit"));
                	Exit( EAknSoftkeyBack );
                    }
                else
                    {
                    // else we are in MMBoxFolder and we can continue
                    // there we always need action with server and local
                    LOGTEXT(_L8("NotViewerAppUi: Delete: We have mmbox notification"));
                    iFlags |= ENotViewerDeleteBoth;
                    }
                }
            else
                { // Answer was NO.
                return;
                }
            }
        // OK, now we are either in Inbox and chose some action related to server
        // or we are in MMBoxView and wanted to delete a message in there..
            
        if( !CheckAPSettingsL( ) )
            { // Are the settings OK
            return;
            }
        LOGTEXT(_L8("NotViewerAppUi: Delete: AP Checked"));
        // Just in case to make sure after checking settings
        iMtm->Entry( ).SetEntryL( iMsgId );
        iMtm->LoadMessageL( );

        if(     iFlags & ENotViewerOffline 
            &&  MsvUiServiceUtilitiesInternal::IsPhoneOfflineL() )
            {   // We are in offline at the moment -> Infonote and return
            ShowInformationNoteL( R_NOTVIEWER_OFFLINE_NOT_POSSIBLE, EFalse );
            return;
            }
        LOGTEXT(_L8("NotViewerAppUi: Delete: Not Offline"));

        // OK, lets go on with the deletion process
        CAknInputBlock* abs = CAknInputBlock::NewLC( );  

        CMsvSingleOpWatcher* singleOpWatcher=CMsvSingleOpWatcher::NewL(*this);
        CleanupStack::PushL(singleOpWatcher);

        CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
        CleanupStack::PushL( selection );
        selection->AppendL( iMtm->Entry().EntryId() );

        CMsvProgressReporterOperation* reporter = 
            CMsvProgressReporterOperation::NewL(
                doc->Session(),
                singleOpWatcher->iStatus,
                EMbmAvkonQgn_note_erased );
        CleanupStack::PushL(reporter);

        LOGTEXT(_L8("NotViewerAppUi: Delete: Call unscheduled delete"));
        CMsvOperation* deleteop = iMtm->UnscheduledDeleteNotificationL(
                *selection,
                EMmsDeleteMMBoxOnly,
                reporter->RequestStatus() );
        LOGTEXT(_L8("NotViewerAppUi: Delete: End of call"));
        reporter->SetProgressDecoder( *this );
        reporter->SetOperationL( deleteop ); // Takes immediate ownership
        CleanupStack::Pop( reporter ); //reporter
        CleanupStack::PopAndDestroy( selection); // selection ownership moved to reporter?
        CleanupStack::Pop( singleOpWatcher );

        iOperation = singleOpWatcher;
        LOGTEXT(_L8("NotViewerAppUi: Delete: Set operation"));
        iOperation->SetOperation( reporter );

        CleanupStack::PopAndDestroy( abs ); 
        }       
    else if( !iMtm->Entry( ).Entry( ).ReadOnly( ) )
        {   // New Forward Request: It's not been saved -> just Exit
        if( ShowConfirmationQueryL( R_NOTVIEWER_QUEST_DELETE_MESSAGE ) )
            {
        	Exit( EAknSoftkeyBack );
            }
        }
    else
        {   // We have an open Sent Forward Request open
        if( ShowConfirmationQueryL( R_NOTVIEWER_QUEST_DELETE_MESSAGE ) )
            {
            DeleteAndExitL( );
            }
        }
    LOGTEXT(_L8("NotViewerAppUi: Delete: End"));
    }

// ---------------------------------------------------------
// CallToNumberL
// ---------------------------------------------------------
//
void CNotViewerAppUi::CallToNumberL()
    {
    if( !(iFlags & ENotViewerIsNotification) )
        {   // We are in forward request
        return;
        }
    if ( !iMsgVoIPExtension )
        {
        iMsgVoIPExtension = CMsgVoIPExtension::NewL();
        }
  	
    TPtrC senderAddr = KNullDesC( );
    TPtrC senderAlias = KNullDesC( );
    TPtrC focusedAddr = KNullDesC( );
    TBool dialerDisabled = EFalse;

    senderAddr.Set( TMmsGenUtils::PureAddress( iMtm->Sender() ) );
    if ( iAlias )
        {
        senderAlias.Set( *iAlias );
        }

    if (    FocusedControlId( ) == EMsgComponentIdBody
        &&  iView
        &&  iView->ItemFinder( ) )
        {
        const CItemFinder::CFindItemExt& item =
            iView->ItemFinder( )->CurrentItemExt( );
        if (    item.iItemDescriptor
            &&  (   item.iItemType == CItemFinder::EPhoneNumber
                ||  item.iItemType == CItemFinder::EEmailAddress ) )
            {
            if ( item.iItemType == CItemFinder::EPhoneNumber )
                {
                // Disable dialer
                iAvkonAppUi->SetKeyEventFlags( 
                    CAknAppUiBase::EDisableSendKeyShort | 
                    CAknAppUiBase::EDisableSendKeyLong );		
                dialerDisabled = ETrue;    
                }
            focusedAddr.Set( *(item.iItemDescriptor) );
            }
        }

    if ( FocusedControlId( ) == EMsgComponentIdFrom && 
         senderAddr.Length() &&
         iSenderType == EMuiuAddressTypePhoneNumber )    
        {
        // Disable dialer
        iAvkonAppUi->SetKeyEventFlags( 
            CAknAppUiBase::EDisableSendKeyShort | 
            CAknAppUiBase::EDisableSendKeyLong );	
        dialerDisabled = ETrue;
        }
    // Only when dialer is disabled try placing the call    
    if ( dialerDisabled )
        {
    	MsvUiServiceUtilitiesInternal::InternetOrVoiceCallServiceL( 
            *iMsgVoIPExtension,
            senderAddr,
            senderAlias,
            focusedAddr,
            ETrue,
            iEikonEnv );
        }
    }


// ---------------------------------------------------------
// MessageDetailsL
// ---------------------------------------------------------
//
void CNotViewerAppUi::MessageDetailsL( )
    {
    LOGTEXT(_L8("NotViewerAppUi: Details start"));
    //These are not used! They just have to be there.
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    TBuf8<1> param;
    param.Zero();

    CAknInputBlock::NewLC( );  

    CMsvSingleOpWatcher* watch = CMsvSingleOpWatcher::NewLC( *Document() );
    
    CMsgEditorDocument* doc = Document( );

    CMsgAddressControl* cntrl = AddressControl( );

    if ( !cntrl )
        {
        return;
        }

    CNotMtmUi* uiMtm = static_cast<CNotMtmUi*>( &doc->MtmUi( ) );

    if ( !(iFlags & ENotViewerIsNotification) )
        {
        // Need to save only if addresses are changed.
        if ( cntrl && cntrl->IsModified() )
            {
            iAddressHandler->CopyAddressesToMtmL( EMsvRecipientTo );
            iMtm->SaveMessageL();
            }
        }
    
    CMsvOperation* op = uiMtm->InvokeAsyncFunctionL(
        KMtmUiFunctionMessageInfo,
        *selection,
        watch->iStatus,
        param );
    CleanupStack::Pop(); // watch
    Document()->AddSingleOperationL( op, watch );

    CleanupStack::PopAndDestroy( 2 ); // selection, CAknInputBlock
    LOGTEXT(_L8("NotViewerAppUi: Details end"));
    }

// ---------------------------------------------------------
// AddRecipientL
// ---------------------------------------------------------
//
void CNotViewerAppUi::AddRecipientL(  )
    {
    LOGTEXT(_L8("NotViewerAppUi: AddRecipient start"));
    if( iFlags & ENotViewerIsNotification )
        {   // We are in notification
        return;
        }

    TBool invalid = EFalse;

    TBool addressesAdded = iAddressHandler->AddRecipientL( invalid );

    if ( addressesAdded )
        {

        Document()->SetHeaderModified( ETrue );

        CMsgAddressControl* to = NULL; 
        to = AddressControl( ); 

        if ( iView->FocusedControl() != to )
            {
            // Focus was not in address control -> Move it
            iView->SetFocus( EMsgComponentIdTo );
            }
        }
    LOGTEXT(_L8("NotViewerAppUi: AddRecipient end"));
    }


// ---------------------------------------------------------
// CheckNamesL
// ---------------------------------------------------------
//
void CNotViewerAppUi::CheckNamesL(  )
    {
    LOGTEXT(_L8("NotViewerAppUi: CheckNamesL start"));
    if( iFlags & ENotViewerIsNotification )
        {   // We are in notification
        return;
        }
    TBool dummy;
    VerifyAddressesL( dummy );
    }

// ---------------------------------------------------------
// VerifyAddressesL
// ---------------------------------------------------------
//
TBool CNotViewerAppUi::VerifyAddressesL( TBool& aModified )
    {
    LOGTEXT(_L8("NotViewerAppUi: VerifyAddresses start"));
    if( iFlags & ENotViewerIsNotification )
        {   // We are in notification
        return EFalse;
        }
    aModified = EFalse;
    TBool retVal = iAddressHandler->VerifyAddressesL( aModified );
    if ( aModified )
        {
        Document()->SetHeaderModified( ETrue );
        }
    LOGTEXT(_L8("NotViewerAppUi: VerifyAddressesL end"));
    return retVal;
    }


// ---------------------------------------------------------
// RichText
// ---------------------------------------------------------
//
CRichText& CNotViewerAppUi::RichText() const
    {
    CMsgBaseControl* baseControl = iView->ControlById( 
        EMsgComponentIdBody );
    CMsgBodyControl* bodyControl = STATIC_CAST( 
        CMsgBodyControl*, 
        baseControl );
    return bodyControl->TextContent( );
    }

// ---------------------------------------------------------
// DoSaveL
// ---------------------------------------------------------
//
void CNotViewerAppUi::DoSaveL( )
    {
    LOGTEXT(_L8("NotViewerAppUi: DoSaveL start"));
    iMtm = &(Document()->Mtm());
    if( !(iFlags & ENotViewerIsNotification) && !iMtm->Entry( ).Entry( ).ReadOnly( ) )
        {
        iAddressHandler->CopyAddressesToMtmL( EMsvRecipientTo );

        iMtm->SaveMessageL();

        TMmsMsvEntry tEntry = static_cast<TMmsMsvEntry>( Document()->Entry() );

        TBuf<KMaxDetailsLength> detailsBuf; 
        iAddressHandler->MakeDetailsL( detailsBuf );
        tEntry.iDetails.Set( detailsBuf );
        if ( !tEntry.EditorOriented() )
            {
            tEntry.SetEditorOriented( ETrue );
            }
        tEntry.iDate.UniversalTime();

        if ( !tEntry.Visible() )
            {
            // Save from close or exit save.
            // Message should be visible after save
            tEntry.SetVisible( ETrue );
            tEntry.SetInPreparation( EFalse );
            }
        Document()->CurrentEntry().ChangeL( tEntry );

        Document()->SetHeaderModified( EFalse );
        }
    LOGTEXT(_L8("NotViewerAppUi: DoSaveL end"));
    }

// ---------------------------------------------------------
// DoMsgSaveExitL
// ---------------------------------------------------------
//
void CNotViewerAppUi::DoMsgSaveExitL( )
    {
    LOGTEXT(_L8("NotViewerAppUi: DoMsgSaveExitL"));
	Exit( EAknSoftkeyBack );
	}
    
// ---------------------------------------------------------
// HandleEntryDeletedL
// ---------------------------------------------------------
//
void CNotViewerAppUi::HandleEntryDeletedL()
    {
    LOGTEXT(_L8("NotViewerAppUi: HandleEntryDeletedL"));
	Exit( EAknSoftkeyBack );
    }

// ---------------------------------------------------------
// HandleEntryChangeL
// ---------------------------------------------------------
//
void CNotViewerAppUi::HandleEntryChangeL()
    { // Call EntryChangedL to update the status
    LOGTEXT(_L8("NotViewerAppUi: HandleEntryChangeL start"));
    if( !( iFlags & ENotViewerIsNotification ) )
        {   // We are not in notification mode -> entry change can be ignored
        return;
        }
    iMtm->Entry( ).SetEntryL( iMsgId );
    TRAP_IGNORE( iMtm->LoadMessageL() );
    EntryChangedL( );
    LOGTEXT(_L8("NotViewerAppUi: HandleEntryChangeL end"));
    }

// ---------------------------------------------------------
// AddItemL
// ---------------------------------------------------------
//
void CNotViewerAppUi::AddItemL(const TDesC& aLabel, const TDesC& aValue )
    {
    if ( aLabel.Length() > 0 )
        {
        AppendTextL( aLabel , iLabelFormat, iLabelMask );
        }

    if ( aValue.Length() > 0 )
        {
        AppendTextL( aValue , iValueFormat, iValueMask );
        }
    }

// ---------------------------------------------------------
// ApplyFormat
// ---------------------------------------------------------
//
void CNotViewerAppUi::ApplyFormat(TCharFormat& aFormat, TCharFormatMask& aMask, TBool aIsLabel)
    {
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
    TAknLayoutText textLayout;

    if( aIsLabel )
        { // Font for headings
        textLayout.LayoutText(
            mainPane,
            AppLayout::Smart_Messages_Line_1( 0 ) );
        }
    else
        { // Font for data
        textLayout.LayoutText(
            mainPane,
            AppLayout::Smart_Messages_Line_2( 0 ) );
        }

    aFormat.iFontSpec = textLayout.Font( )->FontSpecInTwips();    
    aMask.ClearAll();
    aMask.SetAttrib(EAttFontStrokeWeight);
    // If following line in use, it disables text zooming
    //aMask.SetAttrib(EAttFontHeight);
    aMask.SetAttrib(EAttFontTypeface);
    }

// ---------------------------------------------------------
// AppendTextL
// ---------------------------------------------------------
//
void CNotViewerAppUi::AppendTextL( const TDesC& aText,
    const TCharFormat& aFormat, 
    const TCharFormatMask& aMask )
    {
    // Create the modifiable descriptor
    HBufC* buf = HBufC::NewLC( aText.Length() );
    TPtr modifiedText = buf->Des();
    modifiedText = aText;
    TInt pos(0);

	AknTextUtils::DisplayTextLanguageSpecificNumberConversion( modifiedText );

    /*
    We have three cases, where specific character(s) must be
    converted to the CRichText understandable linebreak character
    (CEditableText::ELineBreak).
    1)  Text contains ascii code linefeed (LF) character 0x0A, which
        is converted to linebreak.
    2)  Text contains ascii code carrier return (CR) character 0x0D
    3)  Text contains both CR and LF characters particularly in this order.
        These two characters must be converted to one linebreak.
    */
    
    TUint LF(0x0A); // Linefeed character in ascii set
    TUint CR(0x0D); // Carriage return character in ascii set

    // Case 2 and 3
    while ( ( pos = modifiedText.Locate( CR ) ) != KErrNotFound ) // Locate CR
        {
        if ( pos + 1 < modifiedText.Length() && LF == modifiedText[pos + 1] ) // Is the next LF?
            {
            modifiedText.Delete( pos + 1, 1 ); // Delete if LF
            }
        modifiedText[pos] = CEditableText::ELineBreak; // Replace CR with linebreak
        }

    // Case 1
    while ( ( pos = modifiedText.Locate( LF ) ) != KErrNotFound ) // Locate LF
        {        
        modifiedText[pos] = CEditableText::ELineBreak; // Replace LF with linebreak
        }

    // Append the modified text to the richtext.
    CRichText& rich = RichText();
    
    TInt documentPos = rich.DocumentLength();

    // Every time when text is added, the cursor is left at the end of the line.
    // When the new text is added we must first add linebreak and then the text
    // linebreak is not added if the text is first item.
    if ( iFlags & ENotViewerIsNotFirstItem )
        {
        //Append the linebreak to the end of the richtext.
        rich.InsertL(documentPos, CEditableText::ELineBreak);

        }
    else
        {
        iFlags |= ENotViewerIsNotFirstItem;
        }

    documentPos = rich.DocumentLength();
    rich.SetInsertCharFormatL(aFormat, aMask, documentPos);
    rich.InsertL( documentPos, *buf );
    rich.CancelInsertCharFormat();
    
    CleanupStack::PopAndDestroy( buf );
    }

// ---------------------------------------------------------
// CNotViewerAppUi::FocusedControlId
// ---------------------------------------------------------
//
TInt CNotViewerAppUi::FocusedControlId()
    {
    TInt ret = EMsgComponentIdNull;
    if ( iView && iView->FocusedControl() )
        {
        ret = iView->FocusedControl()->ControlId();
        }
    return ret;
    }

// ---------------------------------------------------------
// Document
// ---------------------------------------------------------
//
CNotViewerDocument* CNotViewerAppUi::Document() const
    {
    return static_cast<CNotViewerDocument*>( CMsgEditorAppUi::Document() );
    }

// ---------------------------------------------------------
// LaunchHelpL
// ---------------------------------------------------------
//
void CNotViewerAppUi::LaunchHelpL()
    {
    LOGTEXT(_L8("NotViewerAppUi: LaunchHelp"));
	if ( iFlags & ENotViewerFlagsHelp )
		{
	    CArrayFix<TCoeHelpContext>* helpContext = AppHelpContextL();
		HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), helpContext );
		}
    }

// ---------------------------------------------------------
// HelpContextL
// ---------------------------------------------------------
//
CArrayFix<TCoeHelpContext>* CNotViewerAppUi::HelpContextL() const
    {
	if ( iFlags & ENotViewerFlagsHelp )
		{
	    CArrayFix<TCoeHelpContext>* r = new ( ELeave ) CArrayFixFlat<TCoeHelpContext>(1);
		CleanupStack::PushL( r );     

        if ( iFlags & ENotViewerIsNotification )
            {   // Notification mode
    	    r->AppendL( TCoeHelpContext( KUidMmsViewer, KMMS_HLP_NOTIFICATION_VIEWER() ) );
            }
        else
            {
            if ( iMtm->Entry( ).Entry( ).ReadOnly( ) )
                {
        	    r->AppendL( TCoeHelpContext( KUidMmsViewer, KMMS_HLP_NOTIFICATION_VIEWER() ) );
                }
            else
                {
        	    r->AppendL( TCoeHelpContext( KUidMmsViewer, KMMS_HLP_MMS_FWD_REQUEST() ) );
                }
            }
		CleanupStack::Pop( r );
		return r;
		}
	return NULL;
    }

// -------------------------------------------------------------
// AddMessageFieldL
// -------------------------------------------------------------
void CNotViewerAppUi::AddMessageFieldL( )
    {
    LOGTEXT(_L8("NotViewerAppUi: AddMessageField start"));
    TInt resourceId = 0;
    TMsvEntry entry = iMtm->Entry( ).Entry( );

    CTextResolver* resolver = CTextResolver::NewLC();
    const TDesC& error = resolver->ResolveErrorString( entry.iError, CTextResolver::ECtxNoCtxNoSeparator );
    
    switch( iMsgStatus )
        {
        case ENotViewerStatusFailed:
            resourceId = R_NOTVIEWER_INFO_FAILED;
            break;
        case ENotViewerStatusExpired:
            resourceId = R_NOTVIEWER_INFO_EXPIRED;
            break;
        case ENotViewerStatusForwarding:
            resourceId = R_NOTVIEWER_INFO_FORWARDING;
            break;
        case ENotViewerStatusForwarded:
            resourceId = R_NOTVIEWER_INFO_FORWARDED;
            break;
        case ENotViewerStatusNull:
        case ENotViewerStatusWaiting:
        default:
            resourceId = R_NOTVIEWER_INFO_WAITING;
            break;
        }
    HBufC* message = StringLoader::LoadLC( resourceId );
    AddItemL( message->Des( ), error );
    CleanupStack::PopAndDestroy( 2 ); //message, resolver
    LOGTEXT(_L8("NotViewerAppUi: AddMessageField end"));
    }
// -------------------------------------------------------------
// AddSubjectFieldL
// -------------------------------------------------------------
void CNotViewerAppUi::AddSubjectFieldL( )
    {
    LOGTEXT(_L8("NotViewerAppUi: AddSubject start"));
    TPtrC subject = iMtm->SubjectL( );
    if( subject.Length( ) > 0 )
        {
        HBufC* label = StringLoader::LoadLC( R_NOTVIEWER_LABEL_SUBJECT );
        AddItemL( label->Des( ), subject );
        CleanupStack::PopAndDestroy( label );
        }
    LOGTEXT(_L8("NotViewerAppUi: AddSubject end"));
    }

// -------------------------------------------------------------
// AddTextFieldL
// -------------------------------------------------------------
//
void CNotViewerAppUi::AddTextFieldL( )
    {
    LOGTEXT(_L8("NotViewerAppUi: AddText start"));
    TPtrC text = iMtm->GetExtendedText( );
    if( text.Length( ) > 0 )
        {
        HBufC* label = StringLoader::LoadLC( R_NOTVIEWER_LABEL_TEXT );
        AddItemL( label->Des( ), text );
        CleanupStack::PopAndDestroy( label );
        }
    LOGTEXT(_L8("NotViewerAppUi: AddText end"));
    }

// -------------------------------------------------------------
// AddExpiryFieldL
// -------------------------------------------------------------
void CNotViewerAppUi::AddExpiryFieldL( )
    {
    LOGTEXT(_L8("NotViewerAppUi: AddExpiry start"));
    HBufC* label = StringLoader::LoadLC( R_NOTVIEWER_LABEL_EXPIRES );
    TTime messageDate = iMtm->ExpiryDate( );

	TLocale locale;
    messageDate += locale.UniversalTimeOffset();
	if (locale.QueryHomeHasDaylightSavingOn())          
		{
		TTimeIntervalHours daylightSaving(1);          
		messageDate += daylightSaving;
		}

    HBufC* dateFormat = StringLoader::LoadLC( R_QTN_DATE_USUAL_WITH_ZERO, iCoeEnv );
    TBuf<KDateSize> dateString;
    messageDate.FormatL( dateString, *dateFormat );
    CleanupStack::PopAndDestroy(); //dateFormat

    dateFormat = StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO, iCoeEnv );
    TBuf<KDateSize> timeString;
    messageDate.FormatL( timeString, *dateFormat );
    CleanupStack::PopAndDestroy(); //dateFormat

    CDesC16ArrayFlat* array = new(ELeave) CDesC16ArrayFlat( 2 );
    CleanupStack::PushL( array );
    array->AppendL( timeString );
    array->AppendL( dateString );
    HBufC* expiryString = 
        StringLoader::LoadLC( R_NOTVIEWER_VALUE_EXPIRES, *array, iCoeEnv );
    AddItemL( label->Des( ), expiryString->Des( ) );
    CleanupStack::PopAndDestroy( expiryString );
    CleanupStack::PopAndDestroy( array );
    CleanupStack::PopAndDestroy( label );        
    LOGTEXT(_L8("NotViewerAppUi: AddExpiry end"));
    }

// -------------------------------------------------------------
// AddSizeFieldL
// -------------------------------------------------------------
void CNotViewerAppUi::AddSizeFieldL( )
    {
    LOGTEXT(_L8("NotViewerAppUi: AddSize start"));
    TInt size = iMtm->MessageTransferSize( );

    TInt maxSize = static_cast<TInt>( iMtm->MmsSettings().MaximumReceiveSize( ) );
    if( maxSize > 0 )
        {
        if( size > maxSize )
            {
            size = maxSize;
            }
        }

    HBufC* label = StringLoader::LoadLC( R_NOTVIEWER_LABEL_SIZE );

    TBuf<20> sizeString;

    MsgAttachmentUtils::FileSizeToStringL( sizeString, size, ETrue );

    sizeString.Trim();

    AddItemL( label->Des( ), sizeString );
    CleanupStack::PopAndDestroy( label );
    LOGTEXT(_L8("NotViewerAppUi: AddSize end"));
    }

// -------------------------------------------------------------
// AddClassFieldL
// -------------------------------------------------------------
void CNotViewerAppUi::AddClassFieldL( )
    {
    LOGTEXT(_L8("NotViewerAppUi: AddClass start"));
    TInt msgClass = iMtm->MessageClass( );
    TInt resourceId;
    switch( msgClass )
        {
        case EMmsClassPersonal:
            {
            resourceId = R_NOTVIEWER_VALUE_CLASS_PERSONAL;
            break;
            }
        case EMmsClassAdvertisement:
            {
            resourceId = R_NOTVIEWER_VALUE_CLASS_ADVERTISEMENT;
            break;
            }
        case EMmsClassInformational:
            {
            resourceId = R_NOTVIEWER_VALUE_CLASS_INFORMATIVE;
            break;
            }
        default:
            {   // In case no class is returned (0), don't add the field
            return;
            }
        }
    HBufC* label = StringLoader::LoadLC( R_NOTVIEWER_LABEL_CLASS );
    HBufC* value = StringLoader::LoadLC( resourceId );
    AddItemL( label->Des( ), value->Des( ) );
    CleanupStack::PopAndDestroy( value );
    CleanupStack::PopAndDestroy( label );
    LOGTEXT(_L8("NotViewerAppUi: AddClass end"));
    }

// -------------------------------------------------------------
// AddPriorityFieldL
// -------------------------------------------------------------
void CNotViewerAppUi::AddPriorityFieldL( )
    {
    LOGTEXT(_L8("NotViewerAppUi: AddPriority start"));
    TInt msgPri = iMtm->MessagePriority( );
    TInt resourceId;
    switch( msgPri )
        {
        case EMmsPriorityLow:
            {
            resourceId = R_NOTVIEWER_VALUE_PRIORITY_LOW;
            break;
            }
        case EMmsPriorityHigh:
            {
            resourceId = R_NOTVIEWER_VALUE_PRIORITY_HIGH;
            break;
            }
        case EMmsPriorityNormal:
            {
            resourceId = R_NOTVIEWER_VALUE_PRIORITY_NORMAL;
            break;
            }
        default:
            {   // In case no priority is returned (0), don't add the field
            return;
            }
        }
    HBufC* label = StringLoader::LoadLC( R_NOTVIEWER_LABEL_PRIORITY );
    HBufC* value = StringLoader::LoadLC( resourceId );
    AddItemL( label->Des( ), value->Des( ) );
    CleanupStack::PopAndDestroy( value );
    CleanupStack::PopAndDestroy( label );
    LOGTEXT(_L8("NotViewerAppUi: AddPriority end"));
    }

// ---------------------------------------------------------
// CNotViewerAppUi::ShowConfirmationQueryL
// ---------------------------------------------------------
//
TInt CNotViewerAppUi::ShowConfirmationQueryL( TInt aResourceId ) const
    {
    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    HBufC* prompt = StringLoader::LoadLC( aResourceId, iCoeEnv );
    TInt retVal = dlg->ExecuteLD( R_NOTVIEWER_CONFIRMATION_QUERY, *prompt );
    CleanupStack::PopAndDestroy( prompt );
    return retVal;
    }

// ---------------------------------------------------------
// CNotViewerAppUi::ShowInformationNoteL
// ---------------------------------------------------------
//
void CNotViewerAppUi::ShowInformationNoteL( TInt aResourceID, TBool aWaiting )
    {
    HBufC* string = NULL;
    string = StringLoader::LoadLC( aResourceID, iCoeEnv );

    CAknInformationNote* note = new ( ELeave )
        CAknInformationNote( aWaiting );
    note->ExecuteLD( *string );

    CleanupStack::PopAndDestroy(); //string
    }

// ---------------------------------------------------------
// CNotViewerAppUi::AddressControl
// ---------------------------------------------------------
//
CMsgAddressControl* CNotViewerAppUi::AddressControl( )
    {
    TInt controlType = 0;
    if( iFlags & ENotViewerIsNotification )
        {   // We are in notification mode
        controlType = EMsgComponentIdFrom;
        }
    else
        {   // we are in the ForwReq mode
        controlType = EMsgComponentIdTo;
        }
    return static_cast<CMsgAddressControl*>( iView->ControlById( controlType ) );
    }

// ---------------------------------------------------------
// CNotViewerAppUi::EntryChangedL
// ---------------------------------------------------------
//
void CNotViewerAppUi::EntryChangedL( )
    {
    LOGTEXT(_L8("NotViewerAppUi: EntryChangedL start"));
    if ( !Document()->IsLaunched() )
        {
        return;
        }

    TNotViewerStatus status = MessageStatus( );
    if( iMsgStatus != status )
        {
        iMsgStatus = status;

        iFlags &= ~ENotViewerIsNotFirstItem;
    
        CMsgBaseControl* control = iView->RemoveControlL( EMsgComponentIdBody );
        delete control;

        CMsgBodyControl* bodyC = CMsgBodyControl::NewL(iView);
        iView->AddControlL(bodyC, EMsgComponentIdBody, EMsgAppendControl, EMsgBody);

        AddMessageFieldL( );
        AddSubjectFieldL( );
        AddTextFieldL( );
        AddSizeFieldL( );
        if( iFlags & ENotViewerIsNotification )
            { // We are in the Viewer so lets add expiry, too
            AddExpiryFieldL( );
            }
        AddClassFieldL( );
        AddPriorityFieldL( );

        // without this editor control does not become properly visible
        bodyC->Editor( ).NotifyNewDocumentL( );
        }

    // update toolbar
    if(  iToolbar )   //check to be provided for non-touch devices.
        {
        CNotViewerDocument* doc = Document( );
        CNotMtmUiData* uiData = &doc->MtmUiDataL( );
        TMsvEntry entry = iMtm->Entry( ).Entry( );

	    if( iFlags & ENotViewerIsNotification )
	        {
	        // return value 0=> OK, !=0 => NOK
            if( uiData->OperationSupportedL( KMtmUiFunctionFetchMMS, entry ) )
                {
                iToolbar->SetItemDimmed(    ENotViewerToolbarRetrieve,
                                            ETrue,
                                            ETrue );
                }
            else
                {                        
                iToolbar->SetItemDimmed(    ENotViewerToolbarRetrieve,
                                            EFalse,
                                            ETrue );
                }
	        }
        }

    LOGTEXT(_L8("NotViewerAppUi: EntryChangedL end"));
    }

// ---------------------------------------------------------
// CNotViewerAppUi::UpdateTitlePaneL
// ---------------------------------------------------------
//
void CNotViewerAppUi::UpdateTitlePaneL( )
    {
    LOGTEXT(_L8("NotViewerAppUi: UpdateTitle"));
    HBufC*  str = StringLoader::LoadLC( R_NOTVIEWER_TITLE_FWD_RQST, iCoeEnv );
    CAknTitlePane* title = static_cast<CAknTitlePane*>
        ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    title->SetTextL( str->Des( ) );
    CleanupStack::PopAndDestroy(); //str
    if( !iMtm->Entry( ).Entry( ).ReadOnly() )
        { // We have a new unsent FR, change CBA too
        Cba()->SetCommandSetL( R_NOTVIEWER_SOFTKEYS_FORWARD_OPTIONS_CANCEL__CONTEXTOPTIONS );
        Cba()->DrawNow();
        }
    }

// ---------------------------------------------------------
// CNotViewerAppUi::MessageStatus
// ---------------------------------------------------------
//
TNotViewerStatus CNotViewerAppUi::MessageStatus( ) const
    {
    TNotViewerStatus status = ENotViewerStatusNull; 
    TTime currentTime;
    currentTime.HomeTime( );
    TTime expiryTime = iMtm->ExpiryDate( );
	TLocale locale;
    expiryTime += locale.UniversalTimeOffset();
	if (locale.QueryHomeHasDaylightSavingOn())          
		{
		TTimeIntervalHours daylightSaving(1);          
		expiryTime += daylightSaving;
		}

    TMsvEntry entry = iMtm->Entry( ).Entry( );

    if( ( entry.iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageMNotificationInd ) // Notification
        {
        if(     !( entry.iMtmData2 & KMmsOperationForward ) 
            &&   ( entry.iMtmData2 & KMmsOperationFetch )
            &&  !( entry.iMtmData2 & KMmsOperationOngoing )  // It's !ongoing
            &&   ( entry.iMtmData2 & KMmsOperationFinished ) // It's finished
            &&   ( entry.iMtmData2 & KMmsOperationResult || 
                   entry.iError ) ) // There's error code
            { // Fetch has been failed
            status = ENotViewerStatusFailed;
            }
        else if( currentTime > expiryTime )
            {
            status = ENotViewerStatusExpired;
            }
        else if( ( entry.iMtmData2 & KMmsOperationForward )     // It's forward
            &&  !( entry.iMtmData2 & KMmsOperationFetch )   // It's not delete
            &&   ( entry.iMtmData2 & KMmsOperationOngoing )     // It's ongoing
            &&  !( entry.iMtmData2 & KMmsOperationFinished ) )    // It's not finished
            { // Message is being forwarded
            status = ENotViewerStatusForwarding;
            }
        else if( ( entry.iMtmData2 & KMmsOperationForward )     // It's forward
            &&  !( entry.iMtmData2 & KMmsOperationFetch )   // It's not delete
            &&  !( entry.iMtmData2 & KMmsOperationOngoing )     // It's !ongoing
            &&   ( entry.iMtmData2 & KMmsOperationFinished )    // It's finished
            &&  !( entry.iMtmData2 & KMmsOperationResult ) )    // There was no error
            { // Message has been forwarded
            status = ENotViewerStatusForwarded;
            }
        else 
            {   // Normal waiting state
            status = ENotViewerStatusWaiting;
            }
        }
    
    return status;
    }

// ---------------------------------------------------------
// CNotViewerAppUi::GetTypeL
// ---------------------------------------------------------
//
void CNotViewerAppUi::GetTypeL( )
    {
    TMsvEntry entry = iMtm->Entry( ).Entry( );

    if( ( entry.iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageMNotificationInd ) // Notification
        {
        iFlags |= ENotViewerIsNotification;
        }
    else if( ( entry.iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageForwardReq ) // Forward Request
        {
        iFlags &= ~ENotViewerIsNotification;
        }
    else
        {
        User::Leave( KErrNotSupported); // Unsupported msg type
        }
    iMsgStatus = MessageStatus( );
    }

// ---------------------------------------------------------
// CNotViewerAppUi::HandleNotifyInt
//
// Notification from central repository
//
// There is no need to check aId as we are listening events 
// only from one key
// ---------------------------------------------------------
//
void CNotViewerAppUi::HandleNotifyInt( TUint32 /*aId*/, TInt aNewValue )
    {    
    LOGTEXT(_L8("NotViewerAppUi: HandleNotifyInt"));
    if( iFlags & ENotViewerIsNotification )
        {
        if ( aNewValue == 0 )
            {
            iFlags &= ~ENotViewerAutoFind;
            }
        else
            {
            iFlags |= ENotViewerAutoFind;
            }
        TRAP_IGNORE( SetFindModeL( iFlags & ENotViewerAutoFind ) );
        }
    }

// ---------------------------------------------------------
// CNotViewerAppUi::HandleNotifyGeneric
// ---------------------------------------------------------
//
void CNotViewerAppUi::HandleNotifyGeneric( TUint32 /*aId*/ )
    {
    //Nothing.
    }
    
// ---------------------------------------------------------
// CNotViewerAppUi::HandleNotifyError
// ---------------------------------------------------------
//
void CNotViewerAppUi::HandleNotifyError( TUint32 /*aId*/, TInt /*error*/, CCenRepNotifyHandler* /*aHandler*/ )
    {
    //Nothing.
    }
  
//  
// NOTICE:
//
// Startup observer moved to own class because Publish & Subscribe
// requires an active object for event handling.
//

// ---------------------------------------------------------
// CNotViewerAppUi::SetFindModeL
// ---------------------------------------------------------
//
void CNotViewerAppUi::SetFindModeL( TBool aEnable )
    {
    if ( iView )
        {
        if ( iView->ItemFinder() )
            {
            TInt findMode = aEnable ?
                CItemFinder::EPhoneNumber |
                CItemFinder::EUrlAddress |
                CItemFinder::EEmailAddress :
                CItemFinder::ENoneSelected;
            iView->ItemFinder()->SetFindModeL( findMode );
            }
        CMsgBaseControl* fromControl = iView->ControlById( EMsgComponentIdFrom );
        if ( fromControl && iMtm->Sender().Length() )
            {
            static_cast<CMsgAddressControl*>( fromControl )
                ->SetAddressFieldAutoHighlight( aEnable );
            }
        }
    }

// ----------------------------------------------------
// CNotViewerAppUi::OpCompleted
// ----------------------------------------------------
//
void CNotViewerAppUi::OpCompleted(
    CMsvSingleOpWatcher& aOpWatcher,
    TInt aCompletionCode )
    {
    LOGTEXT(_L8("NotViewerAppUi: OpCompleted start"));
    CMsvOperation* op=&aOpWatcher.Operation();

    if( iOperation )
        {
        if( iOperation->Operation( ).Id( ) == op->Id( ) )
            {
            delete iOperation;
            iOperation = NULL;
            }
        else
            {
            // This should never happen ie. unrecognised operation completed
            return;
            }
        }
    else
        {
        // This should never happen ie. unrecognised operation completed
        return;
        }

    if ( aCompletionCode == KErrCancel )
        {
        LOGTEXT(_L8("NotViewerAppUi: OpCompleted end3"));
        return;
        }

    TMsvEntry entry = iMtm->Entry( ).Entry( );

    if(      ( entry.iMtmData2 & KMmsOperationFetch )   // Fetch + forward
        &&   ( entry.iMtmData2 & KMmsOperationForward ) // == DELETE!
        &&   ( entry.iMtmData2 & KMmsOperationFinished ) // It's finished
        &&   ( entry.iMtmData2 & KMmsOperationResult ) ) // Error code
        {
        // Error note here!
        TRAP_IGNORE( ShowInformationNoteL( R_NOTVIEWER_REMOTE_DELETE_FAILED, EFalse ) );
        // Ignore err
        }
    else
        {
        LOGTEXT(_L8("NotViewerAppUi: OpCompleted: First delete successful"));
        // If also local chosen, call that and exit
        // else show completed note
        if( iFlags & ENotViewerDeleteBoth )
            { // Deletion was succesfull, but also local delete still needed
            TRAPD( err, DoDeleteOpCompletedL( ) );
            if( err )
                { // We may not leave as we are in non-leaving function
                delete iAbsorber;
                iAbsorber = NULL;
                }
            LOGTEXT(_L8("NotViewerAppUi: OpCompleted end2"));
            return;
            }
        else
            {
            // MMBox Deletion was succesfull
            TRAP_IGNORE( ShowInformationNoteL( R_NOTVIEWER_REMOTE_DELETED, EFalse ) );
            // Ignore err 
            }
        }
    LOGTEXT(_L8("NotViewerAppUi: OpCompleted end"));
    }

// ----------------------------------------------------
// CNotViewerAppUi::DoDeleteOpCompletedL
// ----------------------------------------------------
//
void CNotViewerAppUi::DoDeleteOpCompletedL( )
    {
    LOGTEXT(_L8("NotViewerAppUi: DoDeleteOpCompletedL start"));
    iAbsorber = CAknInputBlock::NewLC( );
    CleanupStack::Pop(); // Pop the absorber

    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    selection->AppendL( iMtm->Entry().EntryId() );

    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();

    CMsvOperation* deleteop = iMtm->DeleteNotificationL(
                        *selection,
                        EMmsDeleteNotificationOnly,
                        wait->iStatus );

    if( !deleteop )   // Check this just in case
        {  
        CleanupStack::PopAndDestroy( 2 ); // wait, sel
        return; 
        }

    CleanupStack::PushL( deleteop );
    wait->Start();

    if( wait->iStatus != KErrNone )
        {
        CleanupStack::PopAndDestroy( 3 ); // op, wait, sel
        return;
        }

    // -> no error, we can exit the viewer
    CleanupStack::PopAndDestroy( 3 ); // op, wait, sel

	DoDelayedExitL( 0 );
    LOGTEXT(_L8("NotViewerAppUi: DoDeleteOpCompleted end"));
    return;
    }

//--------------------------------
// DecodeProgress
//--------------------------------
TInt CNotViewerAppUi::DecodeProgress(
    const TDesC8& /*aProgress*/, 
    TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString, 
    TInt& aTotalEntryCount, 
    TInt& aEntriesDone,
    TInt& aCurrentEntrySize, 
    TInt& aCurrentBytesTrans, 
    TBool /*aInternal*/ )
    {
    aCurrentEntrySize = 0;
    aCurrentBytesTrans = 0;   
    aEntriesDone = 0;
    aTotalEntryCount = 0;
    StringLoader::Load( aReturnString,
                        R_NOTVIEWER_WAIT_DELETING_REMOTE,
                        iCoeEnv );
    return KErrNone;
    }

// ---------------------------------------------------------
// CNotViewerAppUi::HandleIteratorEventL
// ---------------------------------------------------------
//
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
void CNotViewerAppUi::HandleIteratorEventL( TMessageIteratorEvent aEvent )
    {
    LOGTEXT(_L8("NotViewerAppUi: HandleIteratorEventL"));
    if (aEvent == EFolderCountChanged)
        {
        InitNavipaneL();
        }
    }
#endif

// ---------------------------------------------------------
// CNotViewerAppUi::InitNavipaneL
// ---------------------------------------------------------
//
void CNotViewerAppUi::InitNavipaneL()
    {
    LOGTEXT(_L8("NotViewerAppUi: InitNavipaneL start"));
#ifdef RD_MSG_NAVIPANE_IMPROVEMENT
    if ( !iNaviDecorator )
        {
        TMsgEditorMsgPriority priority = EMsgEditorMsgPriorityNormal;
    
        if ( iMtm->MessagePriority() == EMmsPriorityHigh )
            {
            priority = EMsgEditorMsgPriorityHigh;
            }
        else if ( iMtm->MessagePriority() == EMmsPriorityLow )
            {
            priority = EMsgEditorMsgPriorityLow;
            }        
        CreateViewerNaviPaneL( Document()->Entry().iDate, priority, ETrue );
        }
        
    iNaviPane->PushL( *iNaviDecorator );
#else
    TMsvEntry msvEntry = iMtm->Entry( ).Entry( );

    if( msvEntry.ReadOnly( ) || iFlags & ENotViewerIsNotification )
        // If message is readonly or it is a notification ->
        // we need msg ordinal number and scroll arrows visible
        {
        TInt indexInFolder = 0;
        TInt msgsInFolder = 0;
        MessageIndexInFolderL( indexInFolder, msgsInFolder );

        if (    indexInFolder < 0
            ||  msgsInFolder == 0 )
            {
            // this may occur when viewer is used in cover ui. Viewer is started as 
            // stand alone without current folder and '1/0' would be visible. 
            iNaviPane->PushDefaultL( );
            return;
            }

        CArrayFixFlat<TInt>* array = new ( ELeave ) CArrayFixFlat<TInt>(KNaviGranularity);
        CleanupStack::PushL( array );
        array->AppendL( indexInFolder + 1 );
        array->AppendL( msgsInFolder );

        HBufC* buf = StringLoader::LoadLC( R_NOTVIEWER_ORDINAL_NUMBER, *array, iCoeEnv );
        CreateTabGroupL( *buf );
        CleanupStack::PopAndDestroy( 2 ); //buf, array

        iNaviDecorator->MakeScrollButtonVisible( ETrue );

        //NaviButtons are dimmed by default
        if ( IsNextMessageAvailableL( ETrue ) )
            {
            iNaviDecorator->SetScrollButtonDimmed( CAknNavigationDecorator::ERightButton, EFalse );
            }
        if ( IsNextMessageAvailableL( EFalse ) )
            {
            iNaviDecorator->SetScrollButtonDimmed( CAknNavigationDecorator::ELeftButton, EFalse );
            }        
        }
    else    // Unsent FR: Make navipane null
        {
        CreateTabGroupL( KNullDesC );
        }
    iNaviPane->PushL( *iNaviDecorator );  // Make it visible
#endif // RD_MSG_NAVIPANE_IMPROVEMENT
    LOGTEXT(_L8("NotViewerAppUi: InitNavipaneL end"));
    }

#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
// ---------------------------------------------------------
// CNotViewerAppUi::CreateTabGroupL
// ---------------------------------------------------------
//
void CNotViewerAppUi::CreateTabGroupL( const TDesC& aText )
    {
    //Delete (possibly) existing navi decorator
    delete iNaviDecorator;
    iNaviDecorator = NULL;

    //Create TabGroup
    iNaviDecorator = iNaviPane->CreateTabGroupL();
    CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>
        ( iNaviDecorator->DecoratedControl() );

    
    TRect naviPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::ENaviPane, naviPane );
    
    TAknLayoutRect naviTabsPane;
    naviTabsPane.LayoutRect( naviPane,
                             AknLayoutScalable_Avkon::navi_navi_tabs_pane().LayoutLine() );
    
    TAknLayoutRect navi2TabsPane;
    navi2TabsPane.LayoutRect( naviTabsPane.Rect(),
                              AknLayoutScalable_Avkon::navi_tabs_2_pane().LayoutLine() );

    TAknLayoutRect tabs2ActivePane;
    tabs2ActivePane.LayoutRect( navi2TabsPane.Rect(),
                                AknLayoutScalable_Avkon::tabs_2_active_pane( 0 ).LayoutLine() );

    TAknLayoutRect tabs2ActivePaneIcon;
    tabs2ActivePaneIcon.LayoutRect( tabs2ActivePane.Rect(),
                                    AknLayoutScalable_Avkon::tabs_2_active_pane_g1().LayoutLine() );

    TSize indicatorIconSize( tabs2ActivePaneIcon.Rect().Size() );
    
    CFbsBitmap* bitmapPriority = NULL;
	CFbsBitmap* maskPriority = NULL;
    
    // Create message priority bitmap
    if ( iMtm->MessagePriority() == EMmsPriorityHigh || 
         iMtm->MessagePriority() == EMmsPriorityLow )
        {
        // Set path of bitmap file
        TParse fileParse;
        fileParse.Set( KMmsUiMbmFile, &KDC_APP_BITMAP_DIR, NULL );
        
        if ( iMtm->MessagePriority() == EMmsPriorityHigh )
            {
            // Create high priority bitmap
            AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
                                         KAknsIIDQgnIndiMcePriorityHigh,
                                         KAknsIIDQsnIconColors,
                                         EAknsCIQsnIconColorsCG7,
                                         bitmapPriority,
                                         maskPriority,
                                         fileParse.FullName(),
                                         EMbmMmsuiQgn_indi_mce_priority_high,
                                         EMbmMmsuiQgn_indi_mce_priority_high_mask,
                                         AKN_LAF_COLOR( KIconDefaultColor ),
                                         indicatorIconSize,
                                         EAspectRatioPreserved );    
            }
        else
            {
            // Create low priority bitmap
            AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
                                         KAknsIIDQgnIndiMcePriorityLow,
                                         KAknsIIDQsnIconColors,
                                         EAknsCIQsnIconColorsCG7,
                                         bitmapPriority,
                                         maskPriority,
                                         fileParse.FullName(),
                                         EMbmMmsuiQgn_indi_mce_priority_low,
                                         EMbmMmsuiQgn_indi_mce_priority_low_mask,
                                         AKN_LAF_COLOR( KIconDefaultColor ),
                                         indicatorIconSize,                                         
                                         EAspectRatioPreserved ); 
            }
            
        if ( !bitmapPriority )            
            {
            User::Leave( KErrNotFound );
            }
            
        CleanupStack::PushL( bitmapPriority );
        CleanupStack::PushL( maskPriority );

        if ( tabGroup->TabCount() == 0 )
            {
            tabGroup->AddTabL( 0, aText, bitmapPriority, maskPriority );
            }
        else
            {
            tabGroup->ReplaceTabL( 0, aText, bitmapPriority, maskPriority );
            }
	      
    	CleanupStack::Pop( maskPriority );
        CleanupStack::Pop( bitmapPriority );
        }
    else
	    {
        if ( tabGroup->TabCount() == 0 )
            {
	        tabGroup->AddTabL( 0, aText );	
            }
        else
            {
	        tabGroup->ReplaceTabL( 0, aText );	
            }
	    }
	    
	tabGroup->SetActiveTabById( 0 );
    tabGroup->SetTabFixedWidthL( KTabWidthWithOneTab );
    }
#endif // !RD_MSG_NAVIPANE_IMPROVEMENT

// ---------------------------------------------------------
// CNotViewerAppUi::AlreadyDeletedFromServer
// ---------------------------------------------------------
//
TBool CNotViewerAppUi::AlreadyDeletedFromServer( )
    {
    TBool resp = EFalse;
    TMsvEntry entry = iMtm->Entry( ).Entry( );
    if(      ( entry.iMtmData2 & KMmsOperationFetch )   // Fetch + forward
        &&   ( entry.iMtmData2 & KMmsOperationForward ) // == DELETE!
        &&   ( entry.iMtmData2 & KMmsOperationFinished ) // It's finished
        &&  !( entry.iMtmData2 & KMmsOperationResult ) ) // No error code
        { // Message fetch has been failed
        resp = ETrue;
        }
    return resp;
    }

// ---------------------------------------------------------
// CNotViewerAppUi::CheckAPSettingsL
// ---------------------------------------------------------
//
TBool CNotViewerAppUi::CheckAPSettingsL( )
    {
    LOGTEXT(_L8("NotViewerAppUi: CheckAPSettingsL start"));
    CMsgEditorDocument* doc = Document( );

    CNotMtmUi* uiMtm = static_cast<CNotMtmUi*>( &doc->MtmUi( ) );

    CMmsSettingsDialog::TMmsExitCode exitCode = CMmsSettingsDialog::EMmsExternalInterrupt;

    if ( iToolbar ) //check to be provided for non-touch devices.
        {    
    iToolbar->HideItemsAndDrawOnlyBackground( ETrue ); // Hide the toolbar before settings dialog is launched
        }
    // Enable Dialer as MMS Settings dialog may be launched
    iAvkonAppUi->SetKeyEventFlags( 0x00 );	
    if( !uiMtm->CheckSettingsL( exitCode ) )
        {

        // Refresh the settings because they were changed
        iSettingsHandler->RefreshSettingsL( );
        TInt32 ap = iSettingsHandler->MmsSettings( )->AccessPoint( 0 );

        //settings saved
        if( !( exitCode==CMmsSettingsDialog::EMmsBack && uiMtm->ApExistsL( ap ) ) )
            { // Settings NOK
             if ( iToolbar ) //check to be provided for non-touch devices.
                {            
            iToolbar->HideItemsAndDrawOnlyBackground( EFalse ); // Show the toolbar 
                }  
            return EFalse;
            }
        //Settings now OK
        }
    if ( iToolbar ) //check to be provided for non-touch devices.
        {
    iToolbar->HideItemsAndDrawOnlyBackground( EFalse ); // Show the toolbar 
        }
    if( exitCode==CMmsSettingsDialog::EMmsExit )
        {
        // Exit was called
        Exit( );
        }

    LOGTEXT(_L8("NotViewerAppUi: CheckAPSettingsL end"));
    return ETrue;
    }

// ---------------------------------------------------------
// DoDelayedExitL
// ---------------------------------------------------------
void CNotViewerAppUi::DoDelayedExitL( const TInt aDelay )
    {
    delete iIdle;
    iIdle = 0;
    iIdle = CPeriodic::NewL( EPriorityNormal - 1 );
    iIdle->Start(   aDelay,
                    aDelay,
                    TCallBack( DelayedExit, this ) );
    }

// ---------------------------------------------------------
// DelayedExit
// ---------------------------------------------------------
TInt CNotViewerAppUi::DelayedExit( TAny* aThis )
    {
    CNotViewerAppUi* editor = static_cast<CNotViewerAppUi*>( aThis );
    TRAP_IGNORE( editor->DoInternalExitL() );
	CAknEnv::RunAppShutter( );
    return KErrNone;
    }

// ---------------------------------------------------------
// DoExternalExitL
// ---------------------------------------------------------
void CNotViewerAppUi::DoInternalExitL()
    {
    delete iAbsorber; // Remove absorber now just in case
    iAbsorber = NULL;
    Exit( EAknSoftkeyBack );
    }

// ---------------------------------------------------------
// EditorObserver
// ---------------------------------------------------------
void CNotViewerAppUi::EditorObserver(TMsgEditorObserverFunc aFunc, TAny* aArg1, TAny* aArg2, TAny* aArg3)
    {
    TRAP_IGNORE(DoEditorObserverL(aFunc,aArg1,aArg2,aArg3 ) );
    }

// ---------------------------------------------------------
// EditorObserver
// ---------------------------------------------------------
void CNotViewerAppUi::DoEditorObserverL(TMsgEditorObserverFunc aFunc, TAny* aArg1, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    switch ( aFunc )
        {
        case EMsgButtonEvent:
            {
            if( !( iFlags & ENotViewerIsNotification ) &&  !iMtm->Entry( ).Entry( ).ReadOnly( ) )
            	{
            	CMsgAddressControl* addrControl = AddressControl( );
				if( addrControl == static_cast<CMsgAddressControl*>( aArg1 ) )
					{
                    TBool modified = EFalse;
                    if ( addrControl->GetRecipientsL()->Count() )
                        {
                        // Recipients found. Verify addresses.
                        if ( !VerifyAddressesL( modified ) )
                            {
                            // Illegal address found.
                            modified = ETrue;
                            }
                        }
                    if ( !modified )//else
                        {
                        // Nothing changed on the UI. Open PhoneBook.
                        AddRecipientL();
                        }
					}
            	}
            }
            break;
         case EMsgHandleFocusChange:
         case EMsgControlPointerEvent:
            {
            // Enable Dialer 
            iAvkonAppUi->SetKeyEventFlags( 0x00 );
            break; 
            }
		default:
			break;
        }
    }

// -----------------------------------------------------------------------------
// handles the touch-ui related control events for next/previous message
// -----------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
void CNotViewerAppUi::HandleNaviDecoratorEventL( TInt aEventID )     
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        if (    iNaviDecorator
            &&  iNaviPane->Top( ) == iNaviDecorator )
            {
            if( IsNextMessageAvailableL( aEventID == EAknNaviDecoratorEventRightTabArrow ) )
                {
                /* no need for separate checks for right and left arrows
                    because IsNextMessageAvailableL() and NextMessageL
                    are called with the truth-value of the same comparison */
                NextMessageL( aEventID == EAknNaviDecoratorEventRightTabArrow );
                }
            }
        }
    }
#endif
#endif

// ---------------------------------------------------------
// DynInitToolbarL
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CNotViewerAppUi::DynInitToolbarL(  TInt            /*aResourceId*/, 
                                        CAknToolbar*    aToolbar )
    {
    if (    iToolbar
        &&  iToolbar  == aToolbar )
        {
        // Dimming buttons follow     
        CNotViewerDocument* doc = Document( );
        CNotMtmUiData* uiData = &doc->MtmUiDataL( );
        TMsvEntry entry = iMtm->Entry( ).Entry( );

	    if( iFlags & ENotViewerIsNotification )
	        {
            // Hide always Send and Add
            iToolbar->HideItem( ENotViewerToolbarSend,
                                ETrue,
                                EFalse );
            iToolbar->HideItem( ENotViewerToolbarAdd,
                                ETrue,
                                EFalse );
            
            if( uiData->OperationSupportedL( KMtmUiFunctionFetchMMS, entry ) )
                {
                iToolbar->SetItemDimmed(    ENotViewerToolbarRetrieve,
                                            ETrue,
                                            EFalse );
                }
            if( iMtm->Sender().Length()==0 )
                {
                iToolbar->SetItemDimmed(    ENotViewerToolbarReply,
                                            ETrue,
                                            EFalse );
                }
            if( uiData->OperationSupportedL( KMtmUiFunctionDeleteMessage, entry ) )
                {
                iToolbar->SetItemDimmed(    ENotViewerToolbarDelete,
                                            ETrue,
                                            EFalse );
                }
	        }
	    else 
            {            
            // Hide always Retrieve and Reply
            iToolbar->HideItem( ENotViewerToolbarRetrieve,
                                ETrue,
                                EFalse );
            iToolbar->HideItem( ENotViewerToolbarReply,
                                ETrue,
                                EFalse );
            
	        if( !iMtm->Entry( ).Entry( ).ReadOnly( ) )
	            {
                iToolbar->SetItemDimmed(    ENotViewerToolbarDelete,
                                            ETrue,
                                            EFalse );
	            }
	        if( iMtm->Entry( ).Entry( ).ReadOnly( ) )
	            {
                iToolbar->SetItemDimmed(    ENotViewerToolbarSend,
                                            ETrue,
                                            EFalse );	            
                iToolbar->SetItemDimmed(    ENotViewerToolbarAdd,
                                            ETrue,
                                            EFalse );	            
	            }
            }
	                
        }
    }
#endif        


// ---------------------------------------------------------
// OfferToolbarEventL
// ---------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CNotViewerAppUi::OfferToolbarEventL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case ENotViewerToolbarRetrieve:
            HandleCommandL( ENotViewerRetrieve );
            break;
            
        case ENotViewerToolbarReply:
            HandleCommandL( ENotViewerReplyViaMessage );            
            break;
            
        case ENotViewerToolbarSend:
            HandleCommandL( ENotViewerSend );
            break;
            
        case ENotViewerToolbarAdd:
            HandleCommandL( ENotViewerAddRecipient );            
            break;

        case ENotViewerToolbarDelete:
            HandleCommandL( ENotViewerDelete );
            break;

        default:
            break;
        }
    }

// ---------------------------------------------------------
// OperationOngoing
// ---------------------------------------------------------
//
TBool CNotViewerAppUi::OperationOngoing( const TMsvEntry& aEntry ) const
    {
    return (    aEntry.iMtmData2 & KMmsOperationOngoing 
            &&  !( aEntry.iMtmData2 & KMmsOperationFinished ) );
    }

// ---------------------------------------------------------
// OperationFinished
// ---------------------------------------------------------
//
TBool CNotViewerAppUi::OperationFinished( const TMsvEntry& aEntry ) const
    {
    return (    aEntry.iMtmData2 & KMmsOperationFinished
            &&  !( aEntry.iMtmData2 & KMmsOperationOngoing ) );
    }
    
// ---------------------------------------------------------
// HandleForegroundEventL
// ---------------------------------------------------------
//
void CNotViewerAppUi::HandleForegroundEventL(TBool aForeground)
    {
    if ( AknLayoutUtils::PenEnabled() )
        {                
        if ( !iToolbar )
            {
            iToolbar = CurrentFixedToolbar();  
            if ( iToolbar )
                {
                iToolbar->SetToolbarObserver( this );
                iToolbar->DisableToolbarL( ETrue ); 
                }
            }
        }
    CMsgEditorAppUi::HandleForegroundEventL( aForeground );
    }
#endif

// ---------------------------------------------------------
// HandleResourceChangeL
// ---------------------------------------------------------
//
void CNotViewerAppUi::HandleResourceChangeL(TInt aType)
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
      	fileParse.Set( KMmsUiMbmFile, &KDC_APP_BITMAP_DIR, NULL );
        
        iAppIcon = AknsUtils::CreateGulIconL( 
            AknsUtils::SkinInstance(), 
            KAknsIIDQgnPropMceMmsTitle,
            fileParse.FullName(),
            EMbmMmsuiQgn_prop_mce_mms_title,
            EMbmMmsuiQgn_prop_mce_mms_title_mask );
        SetTitleIconL();   
        }
    else if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetTitleIconL();   
        }
    }

// ---------------------------------------------------------
// SetTitleIconL
// ---------------------------------------------------------
//
void CNotViewerAppUi::SetTitleIconL()
    {
    SetTitleIconSizeL( iAppIcon->Bitmap() );
    // Create duplicates of the icon to be used
    CFbsBitmap* bitmap = new( ELeave ) CFbsBitmap();
    CleanupStack::PushL( bitmap );
    
    CFbsBitmap* bitmapMask = new( ELeave ) CFbsBitmap();
    CleanupStack::PushL( bitmapMask );
    
    User::LeaveIfError( bitmap->Duplicate( iAppIcon->Bitmap()->Handle() ) );
    User::LeaveIfError( bitmapMask->Duplicate( iAppIcon->Mask()->Handle() ) );

    iTitlePane->SetSmallPicture( bitmap, bitmapMask, ETrue );
    iTitlePane->DrawNow();
    
    CleanupStack::Pop( bitmapMask );
    CleanupStack::Pop( bitmap );
    }
