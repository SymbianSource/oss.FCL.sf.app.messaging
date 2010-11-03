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
* Description:  Implements UI class for Mail application
*
*/


// Mail includes
#include "MsgMailEditorAppUi.h"
#include "MsgMailEditorDocument.h"
#include "msgmaileditor.hrh"
#include "cmsgmailsendop.h"
#include "cmsgmaillaunchop.h"
#include "cmsgmailfetchfileop.h"
#include <MsgMailUIDs.h>            // Application UIDs
#include "MsgMailViewAttachmentsDlg.h"
#include <MsgEditorAppUi.rsg>       // resource identifiers
#include <MsgMailEditor.rsg>
#include "MailLog.h"
#include "MailUtils.h"
#include "cmsgmaileditorheader.h"
#include "cmsgmailnavipaneutils.h"
#include <hlplch.h>
#include <csxhelp/mail.hlp.hrh>
#include <bldvariant.hrh>

// ui includes
#include <avkon.hrh>
#include <eikmop.h> // for SupplyMopObject
#include <eikrted.h>
#include <aknenv.h>         // AppShutter
#include <barsread.h>       // TResourceReader
#include <aknnotewrappers.h>
#include <AknWaitDialog.h>
#include <akntitle.h>
#include <akntoolbar.h>
#include <akntoolbarextension.h>
#include <aknclearer.h>     // CAknLocalScreenClearer
// Editor base classes:
#include <MsgAttachmentInfo.h>
#include <MsgAttachmentModel.h>
#include <MsgEditorView.h>
#include <MsgBodyControl.h>
#include <MsgExpandableControl.h>
#include <MsgAddressControl.h>
#include <MsgCheckNames.h>

#include <Muiumsginfo.hrh>
#include <mtmuibas.h>                   // CBaseMtmUi
#include <MuiuMsvUiServiceUtilities.h>  // MUIU MTM utils
#include <MtmExtendedCapabilities.hrh>
#include <cemailaccounts.h>				// CEmailAccounts
#include <StringLoader.h>               // for StringLoader
#include "MsgMailOptionsDlg.h"
#include "CMsgAddMailHeadersDlg.h"      // Additional headers dialog

#include <fepbase.h>
#include <aknedsts.h>
#include <akninputblock.h>              // CAknInputBlock

// mail central repository handler
#include "CMailCRHandler.h"
#include <MailInternalCRKeys.h>
#include <ImumInHealthServices.h>
#include <ImumInMailboxServices.h>

#include <SendUiConsts.h>

#include <MsgEditorAppUiExtension.h>// for iMsgEditorAppUiExtension
#include <MsgEditorSettingCacheUpdatePlugin.h>

// LOCAL CONSTANTS
const TInt KMessageSizeMaxLength(15);
const TInt KMSKControlID(3);

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMsgMailEditorAppUi::CMsgMailEditorAppUi()
// A default constructor
// -----------------------------------------------------------------------------
//
CMsgMailEditorAppUi::CMsgMailEditorAppUi()
    {
    }

//
// -----------------------------------------------------------------------------
// CMsgMailEditorAppUi::ConstructL()
// Constructs new mail editor and calls PrepareToLaunchL
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::ConstructL()
    {
    LOG( "ConstructL" );
    CMsgMailAppUi::ConstructL();
    
    // Screen is cleared so that user can't press softkeys before the view is
    // properly constructed. Softkeys become visible again in 
    // DelegateFinalizeLaunch() after the editor is properly constructed. 
    iScreenClearer = CAknLocalScreenClearer::NewLC( EFalse );
    CleanupStack::Pop( iScreenClearer ); 
    
#ifdef RD_SCALABLE_UI_V2    
        if ( AknLayoutUtils::PenEnabled() )
        	{
    		//Set us as an observer for the toolbar
    		if ( PopupToolbar() )
    			{
    			PopupToolbar()->SetToolbarObserver( this );
    			PopupToolbar()->SetToolbarVisibility( ETrue, EFalse );
    			}
        	}        	
#endif // RD_SCALABLE_UI_V2

    // Disable task swapper from options menu during launch
    MenuBar()->SetMenuType( CEikMenuBar::EMenuOptionsNoTaskSwapper );
    
    MenuBar()->SetContextMenuTitleResourceId(R_MSGMAILEDITOR_CONTEXT_MENUBAR);           
    
    iNaviPaneUtils = CMsgMailNaviPaneUtils::NewL();
    
    iSendOp = CMsgMailSendOp::NewL( *Document(), *this );
                                    
    iLaunchOp = CMsgMailLaunchOp::NewL( *Document(), *this );

    // Dialer is always disabled in editor
    SetDialerEnabled( EFalse );
    }


// -----------------------------------------------------------------------------
// CMsgMailEditorAppUi::~CMsgMailEditorAppUi()
// -----------------------------------------------------------------------------
//
CMsgMailEditorAppUi::~CMsgMailEditorAppUi()
    {
    delete iCheckNames;
    delete iWaitNote;
    delete iMailEditorHeader;
    delete iFetchFileOp;
    delete iSendOp;
    delete iLaunchOp;
    delete iNaviPaneUtils;
    delete iScreenClearer;
    }

// -----------------------------------------------------------------------------
//  void CMsgMailEditorAppUi::LaunchViewL
//  Opens a message, either a new or an old one.
//  Sets send options in CMsgMailPreferences object.
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::LaunchViewL()
    {
    LOG( "LaunchViewL" );
    
    if ( !Document()->HasModel() )
        {
        // Format restarts editor without proper context
        // and this extra check is needed with.
        // Leave with KErrGeneral.
        User::Leave( KErrGeneral );
        }    

    CMsgEditorAppUi::SetCloseWithEndKey( EFalse );
     
    iLaunchOp->StartOp( *this );

    LOG( "LaunchViewL end" );
    }


// ----------------------------------------------------------------------------
//  CMsgMailEditorAppUi::DelegateInitLaunchL()
//  Called from CMsgMailLaunchOp
// ----------------------------------------------------------------------------
//
CMsgBodyControl& CMsgMailEditorAppUi::DelegateInitLaunchL()
    {
    LOG("CMsgMailEditorAppUi::DelegateInitLaunchL()");
    CMsgMailEditorDocument& doc = *Document();
    
    iMessageID = doc.Entry().Id();

    DoCreateControlsL();
    doc.SetConfirmation( ETrue );
    doc.ReadSendOptionsL();
    
    // Checks which kind of the message is.
    if( doc.IsForwardMessage() || doc.IsReplyMessage() )
    	{
    	HandleSubjectL();
    	}
    else
    	{
    	SetSubjectL( doc.HeaderL().Subject() );
    	}
    
    LOG("CMsgMailEditorAppUi::DelegateInitLaunchL() end");
    return *( BodyControl() );  
    }

// ----------------------------------------------------------------------------
//  CMsgMailEditorAppUi::HandleSubjectL()
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::HandleSubjectL()
	{
	CMsgMailEditorDocument& doc = *Document();
	// base editor provides a service which removes unneeded prefixes
	HBufC* newSubject = CreateSubjectPrefixStringL( 
			doc.HeaderL().Subject(), doc.IsReplyMessage() );
	CleanupStack::PushL( newSubject );
	SetSubjectL( *newSubject );
	CleanupStack::PopAndDestroy( newSubject );
	}

// ----------------------------------------------------------------------------
//  CMsgMailEditorAppUi::DelegateExecuteViewL()
//  Called from CMsgMailLaunchOp
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DelegateExecuteViewL()
    {
    LOG("CMsgMailEditorAppUi::DelegateExecuteViewL ");
    DoGetRecipientsL();
    LockEntryL();    
    UpdateMessageSizeL();    
    iView->ExecuteL( ClientRect(), EMsgComponentIdNull );    
    LOG("CMsgMailEditorAppUi::DelegateExecuteViewL end");
    }    

// ----------------------------------------------------------------------------
//  CMsgMailEditorAppUi::DelegateFinalizeLaunchL()
//  Called from CMsgMailLaunchOp
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DelegateFinalizeLaunchL()
    {
    LOG("CMsgMailEditorAppUi::DelegateFinalizeLaunchL");
    CMsgMailEditorDocument& doc = *Document();
        
    UpdatePriorityIndicatorL();

    // Check recipients' addresses    
    TBool highlight(EFalse);

    if ( doc.IsNewMessage() && !CheckRecipientsL( highlight ) )
        {
        // Wrong address via Send As, CheckRecipientsL shows error note
        // so don't ShowSelectedMailboxL
        }
    else
        {
        SetFocusToFirstFreeFieldL();
        ShowSelectedMailboxL();
        }

        
	CImHeader& header = doc.HeaderL();
	
	//Check if there is old reply to value saved.
	//We don't want to write over local reply-to setting if there is one.
	//If the reply-to is empty it must be replaced by the value
	//in global smtp settings.
	if(header.ReplyTo().Length() == 0)
		{
		TBool retVal = CheckGlobalReplyToSettingL();        
		LOG1("CheckGlobalReplyToSettingL retVal=%d", retVal);
		}
        
	
	// Enable task swapper to options menu after launch has been completed.
    MenuBar()->SetMenuType( CEikMenuBar::EMenuOptions );

    doc.SetChanged( EFalse );   
    
    // Make softkeys visible again.
    delete iScreenClearer;
    iScreenClearer = NULL;
    
    LOG("CMsgMailEditorAppUi::DelegateFinalizeLaunchL end");
    }

// ----------------------------------------------------------------------------
//  CMsgMailEditorAppUi::SetTitlePaneL()
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::SetTitlePaneL( TInt aResourceId )    
    {
    CAknTitlePane* titlePane =
        static_cast<CAknTitlePane *>(StatusPane()->ControlL(
        TUid::Uid(EEikStatusPaneUidTitle)));

    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, aResourceId );
    titlePane->SetFromResourceL( reader );
    CleanupStack::PopAndDestroy(); // CSI: 12 # CreateResourceReaderLC
    }

// ----------------------------------------------------------------------------
//  CMsgMailEditorAppUi::::DynInitMenuPaneL()
//  This function is called by the framework just before it displays
//  a menu pane.
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DynInitMenuPaneL(
	TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    ASSERT( aMenuPane );
    // Confirm app is running properly (needed atleast now)
    if ( !Document()->IsLaunched() )
        {
        // If not, hide everything and return
        TInt amountOfItems = aMenuPane->NumberOfItemsInPane();
        if ( amountOfItems )
            {
            aMenuPane->DeleteBetweenMenuItems( 0, amountOfItems-1 );
            }
        return;
        }
    switch (aResourceId)
        {
        case R_MSGMAILEDITOR_OPTIONS_MENU1:
            // FALLTHROUGH
        case R_MSGMAILEDITOR_CONTEXT_MENUPANE:

            if ( Document()->MailCRHandler()->MailInsertToolBar() )
                {
                aMenuPane->SetItemDimmed(
                    EMsgMailEditorCmdInsertMenu, EFalse );
                }
            else
                {
                aMenuPane->SetItemDimmed(
                    EMsgMailEditorCmdInsertSubMenu, EFalse );
                }
            break;
        case R_MSGMAILEDITOR_OPTIONS_MENU2:
            // Remove attachment option if no attachments
	        aMenuPane->SetItemDimmed(
	        	EMsgMailEditorCmdAttachments,
	        	(Document()->AttachmentModel().NumberOfItems() <= 0));
            break;
        case R_SEND_OPTIONS_MENU:
            // FALLTHROUGH
        case R_MSGMAILEDITOR_OPTIONS_MENU3:
            if (!(iCommonFlags & EHelpSupported))
                {
                aMenuPane->DeleteMenuItem(EAknCmdHelp);
                }
            break;
        case R_MSGMAILEDITOR_INSERT_MENU:
        	aMenuPane->SetItemDimmed(
        		EMsgMailEditorCmdInsertTemplate,
        		EFalse );
        	break;
        case R_MSGMAILEDITOR_ZOOM_SUBMENU:
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
                    ASSERT(0);
                    break;
                }
            }
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
//  CMsgMailEditorAppUi::::EditorObserver
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::EditorObserver(
            TMsgEditorObserverFunc aFunc,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3 )
	{
	TRAP_IGNORE( EditorObserverL(aFunc, aArg1, aArg2, aArg3) );
	}

// -----------------------------------------------------------------------------
// CMsgMailEditorAppUi::HandleKeyEventL()
// Handles received key events
// -----------------------------------------------------------------------------
//
TKeyResponse CMsgMailEditorAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    // still launching application
    if (!Document()->IsLaunched())
        {
        return EKeyWasConsumed;
        }
    TKeyResponse keyResp = CMsgEditorAppUi::HandleKeyEventL(
            aKeyEvent, aType);
    if (iView)
        {
        // By befault the send key is not functional when dialogs or menus
        // are displaying.
        if (!IsDisplayingMenuOrDialog())
        	{
        	if (aKeyEvent.iCode == EKeyPhoneSend)
        		{
        		if ( AreAddressFieldsEmptyL() )
        			{
        			DoAddRecipientL();
        			}
        		else
        			{
        			DoSendMessageL();
        			}
        		keyResp = EKeyWasConsumed;
        		}
        	}
        if ( aKeyEvent.iCode == EKeyEnter )
			{
			HandleEnterKeyL();
			keyResp = EKeyWasConsumed;
			}
        
    	switch (aKeyEvent.iScanCode)
    		{
    		case EStdKeyUpArrow:
    		case EStdKeyDownArrow:
    			{
    			if( aType == EEventKeyUp &&
    					SpecialScrolling( aKeyEvent.iScanCode ) )
    				{
    				keyResp = EKeyWasConsumed;
    				break;
    				}
    			// FALLTHROUGH
    			}
    		case EStdKeyLeftArrow:
    		case EStdKeyRightArrow:
    		case EStdKeyDevice0:        // Options menu
    		case EStdKeyDevice1:        // Done key
    		case EStdKeyDevice2:        // Ok
    		case EStdKeyDevice3:        // Ok
    		case EStdKeyRightShift:     // Shift
    		case EStdKeyApplication0:   // Task switching
    			break;
    		default:
    			{
    			//something is written to header or to body
    			if (aType == EEventKeyUp)
    				{
    				Document()->SetChanged(ETrue);
    				UpdateMessageSizeL();
    				}
    			break;
    			}
    		}
        }
    return keyResp;
    }

// -----------------------------------------------------------------------------
// CMsgMailEditorAppUi::HandleCommandL(TInt aCommand)
// Processes menu selections
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::HandleCommandL(TInt aCommand)
    {
    MAIL_ALLOC_TEST( DoHandleCommandL( aCommand ) );
    } 


// -----------------------------------------------------------------------------
// CMsgMailEditorAppUi::DoMsgSaveExitL()
// Because this method is inherited from MsgEditor method signature is leaving,
// but leaves are really trapped to ensure that exiting succeeds always.
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DoMsgSaveExitL()
	{    
    LOG( "CMsgMailEditorAppUi::DoMsgSaveExit" );
    
#ifdef _DEBUG
    TRAPD( err, PrepereToExitL() );
    LOG1( "CMsgMailEditorAppUi::DoMsgSaveExit @ err:%d", err );
#else
    TRAP_IGNORE( PrepereToExitL() );
#endif
    
    Exit();    
    }
    
// -----------------------------------------------------------------------------
// CMsgMailEditorAppUi::PrepereToExitL()
// Called by DoMsgSaveExit
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::PrepereToExitL()
    {
    LOG( "CMsgMailEditorAppUi::PrepereToExitL" );
    CMsgMailEditorDocument* doc = Document();    
    
    // Make sure that possibly running asynchronous operations are canceled.
    // If their progress note isn't closed before actual destruction, then it
    // will cause an access violation.
    iLaunchOp->Cancel();
    iSendOp->Cancel();
    if ( iFetchFileOp )
        {
        iFetchFileOp->Cancel();
        }
        
    LOG( "PrepereToExitL, operations cancelled" );

    // check that document is launched and view has been created
    if (doc->IsLaunched() && iView )
    	{
    	LOG( "PrepereToExitL, has been fully launched" );
	    if (IsEmptyL())
	        {
	        LOG( "PrepereToExitL, calling DeleteCurrentEntryL" );
	        DeleteCurrentEntryL();
	        LOG( "PrepereToExitL, DeleteCurrentEntryL returned" );
	        return;
	        }

	    // save the document if it has been changed or if it is a new message
	    if (doc->HasChanged() || !doc->CurrentEntry().Entry().Visible() ||
	        iView->IsAnyControlModified())
	        {
	        LOG( "PrepereToExitL, check app shutter" );
	        // Do this some time before starting the active sheduler
	        // Restarting the active scheduler will stop the app shutter,
	        // so record if an app is running a shutter
	        CAknAppUi* shuttingApp = CAknEnv::AppWithShutterRunning();

	        TBool isVisible(Document()->CurrentEntry().Entry().Visible());
	        LOG( "PrepereToExitL, start wait note" );
	        StartWaitNoteLC( isVisible ?
                R_WAIT_SAVE_NOTE :
                R_WAIT_SAVE_NEW_NOTE,
                !isVisible );

            LOG( "PrepereToExitL, saving message" );
	        MsgSaveL( EFalse );
            LOG( "PrepereToExitL, saving finished" );	        
			CleanupStack::PopAndDestroy(); // CSI: 12 # StartWaitNoteLC

	        if (shuttingApp)
	            {
	            shuttingApp->RunAppShutter();
	            }
	        }
    	}        	
    }

// -----------------------------------------------------------------------------
// Checks addresses of all recipients
// -----------------------------------------------------------------------------
TBool CMsgMailEditorAppUi::CheckRecipientsL(TBool aHighlight)
    {
    TBool result(ETrue);
    CMsgAddressControl* addressField = AddressControl(EMsgComponentIdTo);
    if (addressField)
        {
        CheckRecipientsInFieldL(addressField, aHighlight, result);
        }

    if ( result )
        {
        addressField = AddressControl(EMsgComponentIdCc);
        if (addressField)
            {
            CheckRecipientsInFieldL(addressField, aHighlight, result);
            }
        }

    if ( result )
        {
        addressField = AddressControl(EMsgComponentIdBcc);
        if (addressField)
            {
            CheckRecipientsInFieldL(addressField, aHighlight, result);
            }
        }

    return result;
    }

// -----------------------------------------------------------------------------
// Checks addresses of all recipients in given field
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::CheckRecipientsInFieldL(
    CMsgAddressControl* aAddressField, TBool& aHighLight, TBool& aResult)
    {
    ASSERT( aAddressField );
    CAknInputBlock::NewLC(); // block key events
    aResult = ETrue;

    CMsgRecipientArray& recs = *aAddressField->GetRecipientsL();
    TInt count(recs.Count());

    for (TInt i = 0; i < count; i++)
        {
        if (aResult)
            {
            HBufC* address = recs.At(i)->Address();

            if ( IsValidAddress(*address) )
                {
                recs.At(i)->SetValidated(ETrue);
                }

            else // Not valid email address , Search from Phonebook
                {
                InitPhoneBookL();
                //approx 3 recipients
                CMsgRecipientArray* rcptArray =
                    new (ELeave) CArrayPtrFlat<CMsgRecipientItem>(3);		// CSI: 47 # See a comment above.
                CleanupStack::PushL( rcptArray );
		        if ( iCheckNames->FindAndCheckByNameL(
		                *address, CMsgCheckNames::EMsgTypeMail, *rcptArray, -1 ) ) //-1 add
                    {
                    LOG( "CMsgMailEditorAppUi::CheckRecipientsInFieldL" );
                    }

                const TInt contacts(rcptArray->Count());

                if ( contacts > 0)
                    {
                    // Validate addresses found from phonebook
                    TInt index;
                    for (index = 0; index < contacts; ++index)
                        {
                        HBufC* rcptAddress = rcptArray->At(index)->Address();
                        if ( !IsValidAddress(*rcptAddress) )
                            {
                            aResult = EFalse;
                            ShowIncorrectAddressErrorL(
                                *(rcptArray->At(index)->Name()),
                                *rcptAddress);

                            // highlight only first invalid address
                            if (!aHighLight)
                                {
                                SearchUnverifiedStringL(
                                    *address, aAddressField);
                                aAddressField->HighlightUnverifiedStringL();
                                aHighLight = ETrue;
                                }

                            }
                        else
                            {
                            // Valid address from phonebook
                            rcptArray->At(index)->SetValidated(ETrue);
                            rcptArray->At(index)->SetVerified(ETrue);
                            }
                        }

                    // replace array only if all addresses were valid
                    if ( aResult && contacts > 0)
                        {
                        // Replace alias text and set flags
                        recs.At(i)->SetNameL( *rcptArray->At(0)->Name() );
                        recs.At(i)->SetAddressL(
                            *rcptArray->At(0)->Address() );
                        recs.At(i)->SetValidated(ETrue);
                        recs.At(i)->SetVerified(ETrue);

                        // ownership not changed so delete
                        delete rcptArray->At( 0 );
                        rcptArray->Delete( 0 );

                        while ( rcptArray->Count() )
                            {
                            //Add the selected address items
                            i++;
                            count++;
                            // takes ownership
                            recs.InsertL( i, rcptArray->At( 0 ));
                            rcptArray->Delete( 0 );
                            }
                        }

                    }
                else // incorrect address
                    {
                    aResult = EFalse;
                    ShowIncorrectAddressErrorL(*(recs.At(i)->Name()),
                        *address);
                    if (!aHighLight) // highlight only first invalid address
                        {
                        SearchUnverifiedStringL( *address, aAddressField);
                        aAddressField->HighlightUnverifiedStringL();
                        aHighLight = ETrue;
                        }

                    }
                CleanupStack::PopAndDestroy( rcptArray );
                }
            }
        }

    if (aResult)
        {
        // refresh addresscontrol
        User::LeaveIfError( aAddressField->RefreshL( recs ));
        }
    CleanupStack::PopAndDestroy(); // CSI: 12 # CAknInputBlocker
    }

// -----------------------------------------------------------------------------
// Show invalid address error note
// -----------------------------------------------------------------------------
void CMsgMailEditorAppUi::ShowIncorrectAddressErrorL(const TDesC& aTitle,
    const TDesC& aAddress) const
    {
    HBufC* stringholder = NULL;

    if (aTitle.Length())
        {
        // 2 for title & address
        CDesCArrayFlat* strings = new(ELeave) CDesCArrayFlat(2);		// CSI: 47 # See a comment above.
        CleanupStack::PushL( strings );
        strings->AppendL(aAddress);
        strings->AppendL(aTitle);
        stringholder = StringLoader::LoadL(
            R_MAIL_EDITOR_INCORRECT_ADDRESS, *strings, iEikonEnv);
        CleanupStack::PopAndDestroy( strings ); // strings
        CleanupStack::PushL(stringholder);
        }
    else
        {
        stringholder = StringLoader::LoadLC(
            R_MAIL_EDITOR_INCORRECT_ADDRESS_NO_ALIAS,
            aAddress, iEikonEnv );
        }

    // This is a waiting dialog
    CAknInformationNote* note = new(ELeave) CAknInformationNote(ETrue);
    note->ExecuteLD(*stringholder);
    CleanupStack::PopAndDestroy( stringholder ); // stringholder
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::DoSendMessageL()
// Saves message to outbox and tries to send it immediately,
// if Sending schedule in Send options is
// CMsgMailPreferences::EMsgMailSchedulingNow.
// ----------------------------------------------------------------------------
void CMsgMailEditorAppUi::DoSendMessageL()
    {
    CMsgMailEditorDocument* doc = Document();    
    UnlockEntry();

    // Force FEP commit the current word
    BodyControl()->SetFocus( EFalse );
    iCoeEnv->SyncNotifyFocusObserversOfChangeInFocus();

    // check that recipients are valid
    TBool highlight( EFalse );
    if ( CheckRecipientsL( highlight ) )
        {
        DoRemoveDuplicatesL();
        
        iSendOp->StartOp( *this );
        }
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::DelegateSaveMsgL()
// called from send operation
// ----------------------------------------------------------------------------
void CMsgMailEditorAppUi::DelegateSaveMsgL()
    {
    MsgSaveL( EFalse );
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::DelegateMoveMsgToOutboxL()
// called from send operation
// ----------------------------------------------------------------------------
TMsvId CMsgMailEditorAppUi::DelegateMoveMsgToOutboxL()
    {    
    return MoveMessageEntryL( KMsvGlobalOutBoxIndexEntryId );
    }    

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::HandleOpCompleted()
// ----------------------------------------------------------------------------
void CMsgMailEditorAppUi::HandleOpCompleted(
    const CMsgMailBaseOp& aOp,
    TInt aResult )
    {
    LOG( "CMsgMailEditorAppUi::HandleOpCompleted" );
    if ( &aOp == iSendOp )
        {
        LOG1( "HandleOpCompleted, send op @ aResult:%d", aResult );
        if ( aResult == KErrNone || aResult == KErrNoMemory )
            {
            Exit( EAknSoftkeyBack );
            }
        // this is a special error case -  mailbox wasn't found
        else if ( aResult == KErrNotFound )
            {
            //Try to step back
            //Trapped to avoid possible problems in operation side
            //and to keep function as non-leaving.
            TRAPD(err, DoBackstepL() );
            
            //DoBackStep can leave with KLeaveExit and since it call is trapped
            //mail editor isn't closed like it should. That is why we have to make additional
            //call to Exit().
            
            if(err == KLeaveExit)
                {
                Exit( EAknSoftkeyBack );    
                }
            
            
            }
        }
    if ( &aOp == iFetchFileOp )
        {
        LOG1( "HandleOpCompleted, fetch file op @ aResult:%d", aResult );
        // try to update message size and lock the entry
        TRAP_IGNORE( LockEntryL() );
        if ( aResult == KErrNone )
            {
            TRAP_IGNORE( HandleAttaAddedL() );
            }                     
        }    
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::HandleAttaAddedL()
// ----------------------------------------------------------------------------
void CMsgMailEditorAppUi::HandleAttaAddedL()
    {
    if ( iAttachmentDlg )
        {
        iAttachmentDlg->ItemAddedL();
        }
    else
        {
        UpdateMessageSizeL();
        }
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::UpdateHeaderVisibilityL()
// ----------------------------------------------------------------------------
void CMsgMailEditorAppUi::UpdateHeaderVisibilityL(
        	RPointerArray<TAdditionalHeaderStatus>& aHeaders)
	{
	ASSERT( iMailEditorHeader );
	iMailEditorHeader->UpdateHeaderVisibilityL( aHeaders );
	}

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::UpdatePriorityIndicatorL()
// ----------------------------------------------------------------------------
void CMsgMailEditorAppUi::UpdatePriorityIndicatorL()
	{
    TMsvId service;
    TMsvEntry entry;
    User::LeaveIfError(
        Document()->Session().GetEntry( iMessageID, service, entry ) );
    iNaviPaneUtils->SetPriorityIndicator( entry.Priority() );
	}

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::DoSendOptionsL()
// ----------------------------------------------------------------------------
void CMsgMailEditorAppUi::DoSendOptionsL()
    {
    // Enable dialer
    SetDialerEnabled( ETrue );
    UnlockEntry();
    CMsgMailOptionsDlg* sendOptionsDlg
        = CMsgMailOptionsDlg::NewL(
            Document(), (iCommonFlags & EHelpSupported) );
    sendOptionsDlg->SetMopParent(this);

#ifdef RD_SCALABLE_UI_V2    
    //Hide toolbar
    if ( PopupToolbar() )
        {
		CAknToolbarExtension* extension = PopupToolbar()->ToolbarExtension();
    	extension->SetShown( EFalse );
    	DoHideToolbar( ETrue );
        }
    
#endif // RD_SCALABLE_UI_V2
    
    if (sendOptionsDlg->ExecuteLD(R_MAIL_EDITOR_SEND_OPTIONS))
        {
        // exit was chosen in dialog
        CAknAppUi::ProcessCommandL( EAknCmdExit );
        }

#ifdef RD_SCALABLE_UI_V2    
    //Show toolbar
    if ( PopupToolbar() )
        {
        DoHideToolbar( EFalse );
         }
#endif // RD_SCALABLE_UI_V2

	ASSERT( iMailEditorHeader );
    iMailEditorHeader->UpdateReplyToControlL();

    if ( Document()->MailCRHandler()->MailEncodingSetting() )
        {
        DoCheckEncodingL();
        }

    LockEntryL();
    UpdatePriorityIndicatorL();
    // Disable dialer
    SetDialerEnabled( EFalse );
    }

// ----------------------------------------------------------------------------
//  CMsgMailEditorAppUi::SetSubjectL()
//  Sets subject text from CImHeader.
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::SetSubjectL
    (const TDesC& aText) const    //in: subject from CImHeader
    {
    CMsgExpandableControl* subjectControl = SubjectControl();
    if (subjectControl)
    	{
	    CEikRichTextEditor& subject = subjectControl->Editor();
	    subject.SetTextL(&aText);
	    subject.SetCursorPosL(subject.TextLength(), EFalse);
    	}
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::DoHidetoolbar( const TBool aHide )
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DoHideToolbar( const TBool aHide )
	{
    if ( PopupToolbar() )
        {
#ifdef RD_SCALABLE_UI_V2    
        if ( AknLayoutUtils::PenEnabled() )
        	{
    		PopupToolbar()->HideItemsAndDrawOnlyBackground( aHide );
        	}        	
#endif // RD_SCALABLE_UI_V2
		}
	}

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::DoShowSelectionListL()
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DoShowSelectionListL()
	{
	TInt selection( 0 );
	
	CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( &selection );
	dlg->PrepareLC( R_MAILEDITOR_INSERT_SELECTIONLIST );
	
	if ( dlg->RunLD() )
		{
		TInt commandId( EMsgMailFirstFreeCmdId );
		switch( selection )
			{
			case 0:
				commandId = EMsgMailEditorCmdInsertImage;
				break;
			case 1:
				commandId = EMsgMailEditorCmdInsertVideo;
				break;
			case 2:	// CSI: 47 # switch case
				commandId = EMsgMailEditorCmdInsertAudio;
				break;
			case 3:	// CSI: 47 # switch case
				commandId = EMsgMailEditorCmdInsertNote;
				break;
			case 4:	// CSI: 47 # switch case
				commandId = EMsgMailEditorCmdInsertTemplate;
				break;
#ifdef EMAIL_PRESENTATION_SUPPORT
			case 5:	// CSI: 47 # switch case
				commandId = EMsgMailEditorCmdInsertPresentation;
				break;
			case 6:	// CSI: 47 # switch case
				commandId = EMsgMailEditorCmdInsertOther;
				break;
#else
			case 5:	// CSI: 47 # switch case
				commandId = EMsgMailEditorCmdInsertOther;
				break;
#endif //EMAIL_PRESENTATION_SUPPORT
			}
		
		HandleCommandL( commandId );
		}
	}

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::DoShowExtensionToolBar()
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DoShowExtensionToolBar()
	{
    if ( PopupToolbar() )
        {
#ifdef RD_SCALABLE_UI_V2    
        if ( AknLayoutUtils::PenEnabled() )
        	{
        	CAknToolbarExtension* extension = PopupToolbar()->ToolbarExtension();
        	extension->SetShown( ETrue );
        	}        	
#endif // RD_SCALABLE_UI_V2
		}
	}

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::DoShowToolBarOtherL()
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DoShowToolBarOtherL()
	{
    TInt selection( 0 );
    
    CAknListQueryDialog* dlg =
        new ( ELeave ) CAknListQueryDialog( &selection );
    
    if(!AknLayoutUtils::PenEnabled())
        { //For non-touch devices
        dlg->PrepareLC(R_MAILEDITOR_TOOLBAR_CHOICELIST);
        
        if ( dlg->RunLD() )
            {
            TInt command;
            // See r_maileditor_toolbar_choicelist_items
            command = ( selection == 0 ) ?
                EMsgMailEditorCmdInsertNote :
                EMsgMailEditorCmdInsertOther;
            
            DoFetchFileL(command);
            }
        }
#ifdef RD_SCALABLE_UI_V2
    //For touch devices toolbar and submenus are different.
    else 
        {
        dlg->PrepareLC( R_MAILEDITOR_TOOLBAR_TOUCH_CHOICELIST );
        
        if ( dlg->RunLD() )
            {
            // See r_maileditor_toolbar_choicelist_touch_items.
            //Order of this switch-case is based on 
            //that resource array.
            switch(selection)
                {
                case 0: 
                    HandleCommandL( EMsgMailEditorCmdInsertTemplate );
                    break;
                case 1:
                    HandleCommandL( EMsgMailEditorCmdInsertTemplateNote );
                    break;
                case 2:														// CSI: 47 # See a comment above.
                    DoFetchFileL(EMsgMailEditorCmdInsertNote);
                    break;
#ifdef EMAIL_PRESENTATION_SUPPORT
                case 3:														// CSI: 47 # See a comment above.
                    HandleCommandL( EMsgMailEditorCmdInsertPresentation );
                    break;
                case 4:														// CSI: 47 # See a comment above.
                    DoFetchFileL(EMsgMailEditorCmdInsertOther);
                    break;
#else
                case 3:														// CSI: 47 # See a comment above.
                    DoFetchFileL(EMsgMailEditorCmdInsertOther);
                    break;
                    
#endif //EMAIL_PRESENTATION_SUPPORT
                default:
                    //unknown selection
                    ASSERT( 0 );
                    return;
                }
            }
        }
#endif  //RD_SCALABLE_UI_V2
   	}
	

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::DoShowToolBarTextL()
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DoShowToolBarTextL()
    {
    TInt selection( 0 );
    CAknListQueryDialog* dlg =
        new ( ELeave ) CAknListQueryDialog( &selection );
    dlg->PrepareLC( R_MAILEDITOR_TOOLBAR_TEXT_CHOICELIST );
    if ( dlg->RunLD() )
        {
        TInt command;
        // See r_maileditor_toolbar_choicelist_items
        command = ( selection == 0 ) ?
            EMsgMailEditorCmdInsertTemplate :
            EMsgMailEditorCmdInsertTemplateNote;
        HandleCommandL( command );
        }
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::DoFetchFileL()
// Fetch from other application.
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DoFetchFileL( TInt aCommand )
    {
    MsgAttachmentUtils::TMsgAttachmentFetchType fetchType;
    switch( aCommand )
    	{
        case EMsgMailEditorCmdInsertImage:
            fetchType = MsgAttachmentUtils::EImage;
            break;
        case EMsgMailEditorCmdInsertAudio:
            fetchType = MsgAttachmentUtils::EAudio;
            break;
        case EMsgMailEditorCmdInsertVideo:
            fetchType = MsgAttachmentUtils::EVideo;
            break;
        case EMsgMailEditorCmdInsertNote:
        	fetchType = MsgAttachmentUtils::ENote;
        	break;
    	case EMsgMailEditorCmdInsertPresentation:
    		fetchType = MsgAttachmentUtils::ESVG;
    		break;
    		
    		// Falltrough
        case EMsgMailEditorCmdInsertOther:
            // unknown
            fetchType = MsgAttachmentUtils::EUnknown;
            break;
        default:
        	// not our command, just return
    		return;
    	}
    	
    UnlockEntry();
    
    delete iFetchFileOp; // re-instantiate each time with proper fetch type
    iFetchFileOp = NULL;
    iFetchFileOp = CMsgMailFetchFileOp::NewL( fetchType, *Document() );        

    iFetchFileOp->StartOp( *this );
    }

// ----------------------------------------------------------------------------
// TBool CMsgMailEditorAppUi::IsEmptyL()
// Returns ETrue if document contains no data, EFalse otherwise
// ----------------------------------------------------------------------------
//
TBool CMsgMailEditorAppUi::IsEmptyL() const
    {
    if (!AreAddressFieldsEmptyL())
        {
        return EFalse;
        }

    if ( (SubjectControl() && SubjectControl()->TextContent().DocumentLength() > 0) ||
        BodyControl()->TextContent().DocumentLength() > 0 ||
        Document()->AttachmentModel().NumberOfItems())
        {
        return EFalse;
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CMsgEditorAppUi::HelpContextL
// ----------------------------------------------------------------------------
//
CArrayFix<TCoeHelpContext>* CMsgMailEditorAppUi::HelpContextL( ) const
    {
    CArrayFix<TCoeHelpContext>* help =
                    new( ELeave ) CArrayFixFlat<TCoeHelpContext>( 1 );
    CleanupStack::PushL( help );
    help->AppendL( TCoeHelpContext( TUid::Uid( KUidMsgMailEditor ),
        KMAIL_HLP_MAIL_EDITOR() ) );
    CleanupStack::Pop( help );
    return help;
    }
    
// ----------------------------------------------------------------------------
// CMsgEditorAppUi::DynInitToolbarL
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DynInitToolbarL( TInt aResourceId, CAknToolbar* aToolbar )
	{
    if ( aResourceId == R_MAILEDITOR_TOOLBAR )
        {
        //Attachment dialog might have hided this so make sure it is visible
	    aToolbar->HideItem( EMsgMailEditorCmdInsertText, EFalse, EFalse );
	    aToolbar->SetInitialFocusedItem( EMsgMailEditorCmdInsertImage );
	    
#ifdef RD_SCALABLE_UI_V2
	    if(AknLayoutUtils::PenEnabled())
	        {
	        //Less items shown if touch device.
#ifdef RD_SVGT_IN_MESSAGING
	        aToolbar->HideItem( EMsgMailEditorCmdInsertPresentation, ETrue, EFalse );
#endif //RD_SVGT_IN_MESSAGING
	        aToolbar->HideItem( EMsgMailEditorCmdInsertText, ETrue, EFalse );
	        }
#endif //RD_SCALABLE_UI_V2

        }		
	}

// ----------------------------------------------------------------------------
// CMsgEditorAppUi::OfferToolbarEventL
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::OfferToolbarEventL( TInt aCommand )
	{
    if ( PopupToolbar() )
        {
#ifdef RD_SCALABLE_UI_V2    
        // These commands create the list over the toolbar
        // and reason of that list the toolbar must be hidden.
    	if ( aCommand == EMsgMailEditorCmdInsertTemplate ||
    		 aCommand == EMsgMailEditorCmdInsertOther )
    		{
    		CAknToolbarExtension* extension = PopupToolbar()->ToolbarExtension();
    		extension->SetShown( EFalse );
    		}        

    	PopupToolbar()->SetToolbarVisibility( ETrue, EFalse );

#else  // RD_SCALABLE_UI_V2
	PopupToolbar()->SetToolbarVisibility( EFalse );    	
#endif // RD_SCALABLE_UI_V2
        }
		
	if ( aCommand == EMsgMailEditorCmdInsertOther )
	    {
	    // If user chooses Cancel while selecting template text or note
        // DoShowToolBarTextL leaves. This is ok so leave is catched here.
        TRAPD(err, DoShowToolBarOtherL() );
	    if( err != KErrCancel )
            {
            User::LeaveIfError( err );
            }
	    
	    }
    else if ( aCommand == EMsgMailEditorCmdInsertText )
        {
    	// If user chooses Cancel while selecting template text or note
        // DoShowToolBarTextL leaves. This is ok so leave is catched here.
        TRAPD( err, DoShowToolBarTextL() );
        if( err != KErrCancel )
            {
            User::LeaveIfError( err );
            }
        }
    else
        {
	    HandleCommandL( aCommand );
	    
        }
	}

// ----------------------------------------------------------------------------
// CMsgEditorAppUi::DoCreateControlsL
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DoCreateControlsL()
	{
	// Create Base view
	iView = CMsgEditorView::NewL(*this, CMsgEditorView::EMsgEditorModeNormal);
	ASSERT( iView );
	iMailEditorHeader = CMsgMailEditorHeader::NewL( *Document(), *iView );
	}

// ----------------------------------------------------------------------------
// void CMsgMailEditorAppUi::DoAttachmentL()
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DoAttachmentL()
    {
    HBufC* title = iCoeEnv->AllocReadResourceLC(R_TITLE_ATTACHMENTS);

    CMsgAttachmentModel& attmodel = Document()->AttachmentModel();

    iAttachmentDlg = CMsgMailViewAttachmentsDlg::NewL(*title,
        attmodel, *this, (iCommonFlags & EHelpSupported));
    iAttachmentDlg->SetMopParent(this);


#ifdef RD_SCALABLE_UI_V2    
    //Hide toolbar
    if ( PopupToolbar() )
        {
		CAknToolbarExtension* extension = PopupToolbar()->ToolbarExtension();
    	extension->SetShown( EFalse );
        DoHideToolbar( ETrue );
        }
#endif // RD_SCALABLE_UI_V2
    
    // safe to call LD even as a member variable, since
    // framework always takes care of deleting the dialog
    if (iAttachmentDlg->ExecuteLD(R_MEB_VIEW_ATTACHMENT_DIALOG)) // CSI: 50 # see comment above
        {
        iAttachmentDlg = NULL;
        // exit was chosen in dialog
        CAknAppUi::ProcessCommandL( EAknCmdExit );
        }
    else
        {
        //Attachment dialog has exited. Make sure we are the observers for toolbar
        if ( PopupToolbar() )
	        {
			PopupToolbar()->SetToolbarObserver( this );
#ifdef RD_SCALABLE_UI_V2    
			CAknToolbarExtension* extension = PopupToolbar()->ToolbarExtension();
        	extension->SetShown( EFalse );
        	DoHideToolbar( EFalse );
#endif // RD_SCALABLE_UI_V2
			}
	        
        iAttachmentDlg = NULL;
        UpdateMessageSizeL();
        }

    CleanupStack::PopAndDestroy( title ); // title
    }

// ----------------------------------------------------------------------------
// void CMsgMailEditorAppUi::DoCheckEncodingL()
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DoCheckEncodingL()
	{
	CMsgMailPreferences& prefs = Document()->SendOptions();
    TUint charsetId = prefs.SendingCharacterSet().iUid;
    LOG1( "CMsgMailEditorAppUi::DoCheckEncodingL CharsetId:%08x", charsetId );
	if (charsetId)
		{
	    CCnvCharacterSetConverter* charConv =
	        CCnvCharacterSetConverter::NewLC();

	    TInt error = charConv->PrepareToConvertToOrFromL(
	    	charsetId, iEikonEnv->FsSession());
		TInt unconverted(0);
		if (error == CCnvCharacterSetConverter::EAvailable)
			{
			// Try to convert body text
		    CRichText& rtf = BodyControl()->TextContent();
		    HBufC* buf = HBufC::NewLC(rtf.DocumentLength());
		    TPtr bufPtr = buf->Des();
		    rtf.Extract(bufPtr);

		    HBufC8* foreing = HBufC8::NewLC(rtf.DocumentLength());
		    TPtr8 ptr = foreing->Des();
		    error = charConv->ConvertFromUnicode(ptr, *buf, unconverted);
		    CleanupStack::PopAndDestroy(2, buf);							// CSI: 47,12 #  buf, foreing
			}
	    // Show note if can't convert to selected character set.
	    if (error != KErrNone || unconverted > 0)
	    	{
	    	MailUtils::ConfirmationNoteL(R_MAIL_SETTINGS_ENCODE_FALSE_NOTE);
	    	}

	    CleanupStack::PopAndDestroy( charConv );
		}
	}

// ----------------------------------------------------------------------------
// void CMsgMailEditorAppUi::DoBackstepL()
// Implements backstep functionality
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DoBackstepL()
    {
    CMsgMailEditorDocument* doc = Document();

    // if document contains no data, message and entry is deleted
    if ( IsEmptyL() )
        {
        if (doc->CurrentEntry().Entry().Visible())
            {
            HBufC* text = iEikonEnv->AllocReadResourceLC(
                R_MAIL_EDITOR_MESSAGE_DELETED_TEXT);
            // waiting dialog, so it is not left into screen when
            // application is exited
            CAknInformationNote* note = new(ELeave) CAknInformationNote(ETrue);
            note->ExecuteLD(*text);
            CleanupStack::PopAndDestroy( text ); // text
            }
        DeleteAndExitL();
        return;
        }

    switch ( ShowCloseQueryL() )
        {
        case EMsgCloseDelete:
            {
            DeleteAndExitL();
            }
        case EMsgCloseCancel:
            {
            return;
            }
        default:
            {
            // Normal save
            if ( !doc->CurrentEntry().Entry().Visible() ||
                doc->HasChanged() ||
                iView->IsAnyControlModified() )
                {
                const TBool newMessage(
                    !doc->CurrentEntry().Entry().Visible() );
                StartWaitNoteLC( newMessage ?
                    R_WAIT_SAVE_NEW_NOTE :
                    R_WAIT_SAVE_NOTE,
                    newMessage );

			    TRAPD( err, MsgSaveL( EFalse ) );

			    CleanupStack::PopAndDestroy(); // CSI: 12 # StartWaitNoteLC
				User::LeaveIfError(err);
                }
            }
        }
    Exit( EAknSoftkeyBack );
    }


// ----------------------------------------------------------------------------
// void CMsgMailEditorAppUi::DoAddRecipientL()
// Fetch an address from phonebook
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DoAddRecipientL()
    {
    TMsgControlId uiCompId =
    	(TMsgControlId)iView->FocusedControl()->ControlId();

	//Remember original focused control
	TMsgControlId uiOriginalCompId = uiCompId;
	
    // Add recipient into To field if focus is not in address field
    if ( !(uiCompId == EMsgComponentIdTo ||
    	uiCompId == EMsgComponentIdCc ||
    	uiCompId == EMsgComponentIdBcc) )
    	{
    	uiCompId = EMsgComponentIdTo;
    	}

    InitPhoneBookL();
    CMsgRecipientList* recipientList = CMsgRecipientList::NewL();
    CleanupStack::PushL( recipientList );

    CMsgRecipientArray* recipients =
        new (ELeave) CArrayPtrFlat<CMsgRecipientItem>(3);			// CSI: 47 # approx 3
    CleanupStack::PushL( recipients );

#ifdef RD_SCALABLE_UI_V2    
    //Hide toolbar
    if ( PopupToolbar() )
        {
		CAknToolbarExtension* extension = PopupToolbar()->ToolbarExtension();
    	extension->SetShown( EFalse );
    	DoHideToolbar( ETrue );
        }    
#endif // RD_SCALABLE_UI_V2

    //multiple entry fetch to get the contact
    iCheckNames->FetchRecipientsL( *recipients, CMsgCheckNames::EMsgTypeMail );

#ifdef RD_SCALABLE_UI_V2    
    DoHideToolbar( EFalse );
#endif // RD_SCALABLE_UI_V2
    
    //contacts now fetched, verify each contact
    for ( TInt i(0); i < recipients->Count(); i++ )
        {
        CMsgRecipientItem* recipient = (*recipients)[ i ];
        HBufC* name = recipient->Name();
        HBufC* address = recipient->Address();

        // Don't parse away chars here so this is consistent with
        // addresses that user writes "-()" are saved to draft
        // but removed when sending
        if ( IsValidAddress( *address ) )
            {
            //  add it to the list of valid addresses
            recipient->SetVerified( ( name->Length() > 0 ? ETrue : EFalse ) );
            recipientList->AppendL( recipient );
            }
        else
            {
            ShowIncorrectAddressErrorL( *address, *name );
            }
        }
    // if valid recipients were selected, add them to control
    // and set focus to right component
    if ( recipientList->Count() )
        {
        //Add to control
        AddressControl(uiCompId)->AddRecipientsL( *recipientList );
        
        //Set focus
    	SetFocusAfterAddRecipientL( uiOriginalCompId );
        }

    // recipientList, recipients
    CleanupStack::PopAndDestroy( 2, recipientList );				// CSI: 47,12 # See a comment above.
    }
// -----------------------------------------------------------------------------
// void CMsgMailEditorAppUi::AreAddressFieldsEmptyL()
// -----------------------------------------------------------------------------
//
TBool CMsgMailEditorAppUi::AreAddressFieldsEmptyL() const
    {
    TBool result( ETrue );

    CMsgAddressControl* addressField = AddressControl(EMsgComponentIdTo);
    if (addressField)
        {
        CMsgRecipientArray& recs = *addressField->GetRecipientsL();
        if( recs.Count() > 0 )
            {
            result = EFalse;
            }
        }

    addressField = AddressControl(EMsgComponentIdCc);
    if (addressField)
        {
        CMsgRecipientArray& recs = *addressField->GetRecipientsL();
        if( recs.Count() > 0 )
            {
            result = EFalse;
            }
        }

    addressField = AddressControl(EMsgComponentIdBcc);
    if (addressField)
        {
        CMsgRecipientArray& recs = *addressField->GetRecipientsL();
        if( recs.Count() > 0 )
            {
            result = EFalse;
            }
        }

    return result;
    }

// -----------------------------------------------------------------------------
// void CMsgMailEditorAppUi::UpdateMessageSizeL()
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::UpdateMessageSizeL()
    {
    //If attachment dialog is visible we don't want to update navipane,
    //because dialogs navi pane should be empty.
    if(!iAttachmentDlg)
        {
        TInt docSize = MessageSizeL( ETrue ); // Count also attachments
        LOG1( "CMsgMailEditorAppUi::UpdateMessageSizeL:%d", docSize );
        iNaviPaneUtils->SetMessageSizeIndicatorL( docSize );
            
        // attachment indicator may be shown in navi pane only if local variation
        // flag is enabled (by default disabled in common code). Remove this
        // code and SetAttachmentIndicator() method when CR key gets obsolete.
        if ( !Document()->MailCRHandler()->MailAttachmentField() )
            {
            CMsgAttachmentModel& attModel = Document()->AttachmentModel();
            const TInt attCount( attModel.NumberOfItems() );    
            iNaviPaneUtils->SetAttachmentIndicator( attCount > 0 );
            }    
        }
                
    }

// -----------------------------------------------------------------------------
// void CMsgMailEditorAppUi::DoMessageInfoL()
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DoMessageInfoL()
    {
    CMsgMailDocument* doc = Document();
    CMsvEntrySelection* sel = new (ELeave) CMsvEntrySelection;
    CleanupStack::PushL(sel);
    TMsvId id(doc->Entry().Id());
    sel->AppendL(id);

    // save the message so the fields in message info dialog will be up to date
    // this is a "hidden" save, so we preserve values of
    // preparation and visible flags
    const TBool visible(doc->CurrentEntry().Entry().Visible());
    CAknWaitDialog* dlg = new(ELeave) CAknWaitDialog(NULL);
    CleanupStack::PushL(dlg);
    dlg->PrepareLC(R_MEB_WAIT_NOTE);
    HBufC* text = StringLoaderLoadLC( R_WAIT_OPENING );
    dlg->SetTextL(*text);
    CleanupStack::PopAndDestroy( text ); // text
    dlg->RunLD();
    UnlockEntry();
    MsgSaveL(!visible);
    CleanupStack::Pop( dlg ); // dlg
    dlg->ProcessFinishedL();

    TBuf8<KMessageSizeMaxLength> infoParams(KMessageInfoEditor);
    CMsvSingleOpWatcher* watch=CMsvSingleOpWatcher::NewLC(*doc);
    CMsvOperation* op= doc->MtmUi().InvokeAsyncFunctionL(
        KMtmUiFunctionMessageInfo,
        *sel,
        watch->iStatus,
        infoParams
        );
    // watch owns op before leave can occur
    doc->AddSingleOperationL(op, watch);
    CleanupStack::Pop( watch ); // watch
    LockEntryL();

    CleanupStack::PopAndDestroy( sel ); // sel

    // this is done, so message will be saved visible,
    // when editor is exited
    if (!visible)
        {
        doc->SetChanged(ETrue);
        }
    }

// -----------------------------------------------------------------------------
// void CMsgMailEditorAppUi::DoAdditionalHeadersL()
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DoAdditionalHeadersL()
    {
	// Cc field
    TAdditionalHeaderStatus header_status_cc(EMsgComponentIdCc);
    CMsgAddressControl* ccControl =  AddressControl(EMsgComponentIdCc);
    if (ccControl)
    	{
    	header_status_cc.iHasContent = ccControl->Editor().TextLength() > 0;
    	header_status_cc.iStatus = EHeaderVisible;
    	}
    // Bcc field
    TAdditionalHeaderStatus header_status_bcc(EMsgComponentIdBcc);
    // Check if bcc field is allowed
    if ( Document()->MailCRHandler()->MailAdditionalHeaders() )
        {
        CMsgAddressControl* bccControl =  AddressControl(EMsgComponentIdBcc);
        if (bccControl)
        	{
        	header_status_bcc.iHasContent =
        	    bccControl->Editor().TextLength() > 0;
        	header_status_bcc.iStatus = EHeaderVisible;
        	}
        }
	// Subject
    TAdditionalHeaderStatus header_status_subject(EMsgComponentIdSubject);
    CMsgExpandableControl* subjectControl = SubjectControl();
    if (subjectControl)
    	{
    	header_status_subject.iHasContent =
    		subjectControl->Editor().TextLength() > 0;
    	header_status_subject.iStatus = EHeaderVisible;
    	}

    RPointerArray<TAdditionalHeaderStatus> headers;
    CleanupClosePushL(headers);

    headers.Append(&header_status_cc);
    // Check if bcc field is allowed
    if ( Document()->MailCRHandler()->MailAdditionalHeaders() )
        {
        headers.Append(&header_status_bcc);
        }
    headers.Append(&header_status_subject);

    CMsgAddMailHeadersDlg* addHeadersDlg =
    	CMsgAddMailHeadersDlg::NewLC(headers);

    if( addHeadersDlg->ExecuteDialogL() )
        {
        TUint headersvalue(0);
        for (TInt index(0); index < headers.Count(); ++index)
            {
            switch( headers[index]->iHeaderValue )
                {
                case EMsgComponentIdCc:
                    if ( headers[index]->iStatus == EHeaderVisible )
                        {
                        headersvalue += KBit0;
                        }
                    break;
                case EMsgComponentIdBcc:
                    if ( headers[index]->iStatus == EHeaderVisible )
                        {
                        headersvalue += KBit1;
                        }
                    break;
                case EMsgComponentIdSubject:
                    if ( headers[index]->iStatus == EHeaderVisible )
                        {
                        headersvalue += KBit2;
                        }
                    break;
                default:
                    // Unknown header field
                    ASSERT( 0 );
                }
            }
		// remember user selection
        Document()->MailCRHandler()->SetCRInt(
        	KCRUidMail,KMailAdditionalHeaderSettings,headersvalue);

        // set headers
        UpdateHeaderVisibilityL(headers);
        }
    CleanupStack::PopAndDestroy(2);				// CSI: 47,12 # headers, addHeadersDlg
    }

// ----------------------------------------------------------------------------
// void CMsgMailEditorAppUi::DoGetRecipientsL()
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DoGetRecipientsL()
	{
    CImHeader& header = Document()->HeaderL();
    CMsgAddressControl* addressField = NULL;

    addressField = AddressControl(EMsgComponentIdTo);
    AddRecipientArrayL(addressField,
    	header.ToRecipients());

    addressField = AddressControl(EMsgComponentIdCc);
        AddRecipientArrayL(addressField,
            header.CcRecipients());

    addressField = AddressControl(EMsgComponentIdBcc);
    	AddRecipientArrayL(addressField,
            header.BccRecipients());
	}

// ----------------------------------------------------------------------------
// void CMsgMailEditorAppUi::DoRemoveDuplicatesL()
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DoRemoveDuplicatesL()
	{
    // Remove duplicate addresses
    CMsgAddressControl* addressField(NULL);
    addressField = AddressControl(EMsgComponentIdTo);
    RemoveDuplicatesL(addressField);

    addressField = AddressControl(EMsgComponentIdCc);
    RemoveDuplicatesL(addressField);

    addressField = AddressControl(EMsgComponentIdBcc);
    RemoveDuplicatesL(addressField);
    
    //After each control is cleaned remove rest duplicates by comparing different controls
    RemoveDuplicatesL(AddressControl(EMsgComponentIdTo), AddressControl(EMsgComponentIdCc));
    RemoveDuplicatesL(AddressControl(EMsgComponentIdTo), AddressControl(EMsgComponentIdBcc));
    RemoveDuplicatesL(AddressControl(EMsgComponentIdCc), AddressControl(EMsgComponentIdBcc));
	}

// -----------------------------------------------------------------------------
// CMsgEditorAppUi::RemoveDuplicatesL
//
// Compares two address controls and removes duplicate
// address from later one.
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::RemoveDuplicatesL(CMsgAddressControl* aAddressField, 
		CMsgAddressControl* aLaterAddressField)
    {
    
    //Symbian also removes duplicate addresses (CImHeader::IsRecipientPresent), 
    //but it ignores BCC field for some reason.
    
    if(aAddressField && aLaterAddressField)
        {
        CMsgRecipientArray& recs = *aAddressField->GetRecipientsL();
        CMsgRecipientArray& laterRecs = *aLaterAddressField->GetRecipientsL();
    	    
    	TInt laterCount(laterRecs.Count());
    	TInt count(recs.Count());

    	for(TInt i=0; i<count; i++)
    	    {
    	  
        	for(TInt j=0; j<laterCount; j++)
        	    {
    	        if( recs[i]->Address()->FindF(*(laterRecs[j]->Address())) >= 0 )
                    {
                    //Remove duplicate from later control 
                    //and update count of that
                    delete laterRecs.At(j);
                    laterRecs.Delete(j);    
                    
                    laterCount = laterRecs.Count();
                    
                    //Now we can break and move to next item in aAddressField.
                    //Duplicates in the same row should have been removed earlier.
                    break;
                    }
                }
	        }
	
	    aLaterAddressField->RefreshL(laterRecs);    
        }
    
    
    }


// ----------------------------------------------------------------------------
// void CMsgMailEditorAppUi::DoHandleCommandL()
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::DoHandleCommandL( TInt aCommand )
    {
    //Still launching application.
    //IsLaunched() can be true even if iView
    //isn't created yet.
    if( !Document()->IsLaunched() || !iView )
    	{
		if ( aCommand != EEikCmdExit )
			{
			return;
			}
    	}
    
	DoFetchFileL( aCommand );
    switch ( aCommand )
        {
        case EEikCmdExit:
            if( iForceDeleteDocument )
                {
                DeleteAndExitL();
                }
            else
                {
                DoMsgSaveExitL();
                }
            break;
        case EMsgMailEditorCmdSend:
            if (AreAddressFieldsEmptyL())
                {
                DoAddRecipientL();
                }
            else
                {
                DoSendMessageL();
                }
            break;
        case EMsgMailEditorCmdSendingOptions:
            DoSendOptionsL();
            break;
        case EAknSoftkeyClose:
            TRAPD(error, DoBackstepL());
            if ( error == KErrDiskFull )
                {
                // No memory to save changes
                if ( MailUtils::ConfirmationQueryL(
                    R_QTN_MAIL_CLOSE_NOSAVE, R_MAIL_EDITOR_DELETE_MESSAGE) )
                    {
                    Exit();
                    }
                }
            else
                {
                User::LeaveIfError( error );
                }
            break;
        case EMsgMailEditorCmdMskAddRecipient:
            if(!AreAddressFieldsEmptyL() && !AllAddressValidL())
                {
                CheckRecipientsL(EFalse);
                }
            else
                {
                DoAddRecipientL();    
                }
             break;
        case EMsgMailEditorCmdAddRecipient:
            DoAddRecipientL();
            break;
        case EMsgMailEditorCmdAttachments:
            DoAttachmentL();
            break;
        case EMsgMailEditorCmdInsertMenu:
        	DoShowSelectionListL();
           	break;
        case EMsgMailEditorCmdInsertTemplate:
	    // If user chooses Cancel while selecting template text
        // DoShowToolBarTextL leaves. This is ok so leave is catched here.
        	TRAPD(err, InsertTemplateL() );
		    if( err != KErrCancel )
	            {
	            User::LeaveIfError( err );
	            }
            UpdateMessageSizeL();
            break;
        case EMsgMailEditorCmdInsertTemplateNote:
            User::LeaveIfError( InsertTextMemoL() );
            UpdateMessageSizeL();
            break;
        case EMsgMailEditorCmdMessageInfo:
            DoMessageInfoL();
            break;
        case EMsgMailEditorCmdAddHeaders:
            DoAdditionalHeadersL();
            break;
        // general options:
        case EEikCmdEditCut:
            iView->FocusedControl()->ClipboardL(EMsgCut);
            UpdateMessageSizeL();
            break;
        case EEikCmdEditCopy:
            iView->FocusedControl()->ClipboardL(EMsgCopy);
            break;
        case EEikCmdEditPaste:
            iView->FocusedControl()->ClipboardL(EMsgPaste);
            UpdateMessageSizeL();
            break;
        case EMsgMailEditorCmdExpandInsert:
			break;
        case EAknCmdHelp:
            {
            if ((iCommonFlags & EHelpSupported))
                {
                // Launch help
                HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                    iEikonEnv->EikAppUi()->AppHelpContextL() );
                }
            break;
            }
        case EMsgDispSizeAutomatic:
        // FALLTHROUGH
        case EMsgDispSizeLarge:
        // FALLTHROUGH
        case EMsgDispSizeNormal:
        // FALLTHROUGH
        case EMsgDispSizeSmall:
            HandleLocalZoomChangeL( (TMsgCommonCommands)aCommand );
            break;
        default:
            CMsgMailAppUi::HandleCommandL(aCommand);
            break;
        }                       
    }

// ----------------------------------------------------------------------------
// void CMsgMailEditorAppUi::EditorObserverL()
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::EditorObserverL(
    TMsgEditorObserverFunc aFunc,
    TAny* aArg1,
    TAny* aArg2,
    TAny* /*aArg3*/ )
	{
	if ( aFunc == MMsgEditorObserver::EMsgHandleFocusChange )
		{
		ASSERT( aArg1 );
		ASSERT( aArg2 );

		TMsgFocusEvent event = *(static_cast<TMsgFocusEvent*>( aArg1 ) );    		
		TInt controlId = *(static_cast<TInt*>( aArg2 ) );
    		                  
        switch( event )
            {
            case EMsgFocusMovingFrom:
                {
                if ( controlId == EMsgComponentIdAttachment )
                    {                        
                    iNaviPaneUtils->EnableOwnNaviPaneL( EFalse );
                    }
                     
                // Set input mode (e.g. number mode)to other address fields too
				CMsgAddressControl* ctrl = AddressControl( controlId );
                if ( ctrl )
                	{
                	TInt mode( ctrl->Editor().AknEditorCurrentInputMode() );
                    SetInputModeToAddressFields( mode );
                	}
                }// EMsgFocusMovingFrom
                break;
            case EMsgFocusMovedTo:
                {
                CheckFocusForMskL( controlId );
                if ( controlId == EMsgComponentIdAttachment )
                    {     
                    iNaviPaneUtils->EnableOwnNaviPaneL( ETrue );
                    }
                } // EMsgFocusMovingTo
                break;
            } // switch
		}
#ifdef RD_SCALABLE_UI_V2
	else if( aFunc == MMsgEditorObserver::EMsgButtonEvent )
		{
		ASSERT( aArg1 );
		
		//Get the parameter
		CMsgBaseControl* control = static_cast<CMsgBaseControl*>( aArg1 );
        
        //Attachment handling is mostly done in pointer event branch.
		//Here we tackle situation where focus is not in attachment row
		//and the button is pressed. 
		//Also other button events go this way, but attachment handling
		//is a special case.
		if ( control && ( control->ControlId() != EMsgComponentIdAttachment ||
		        iFocusedControl != control ) )
			{
			//Summon touch functionality
			ControlTouchedL( *control );	
        	}
		}
	else if( aFunc == MMsgEditorObserver::EMsgControlPointerEvent )
		{
		if( aArg1 && aArg2 )
			{
            TPointerEvent* event = static_cast<TPointerEvent*>( aArg2 );
            CMsgBaseControl* control = static_cast<CMsgBaseControl*>( aArg1 );

            if ( event->iType == TPointerEvent::EButton1Down )
                {
                //This is done to check if control is already focused.
                //When attachment row is clicked and it has focus
                //attachment dialog is opened. Otherwise row just gains focus.                
                iFocusedControl = control && control->IsFocused() ? control : NULL;
                }
            else if ( event->iType == TPointerEvent::EButton1Up )
                {
				if( control && iFocusedControl == control && 
					control->ControlId() == EMsgComponentIdAttachment )        
					{			
					//Summon touch functionality
					ControlTouchedL( *control );
					}
                }
			}
		}
#endif
	}

// ---------------------------------------------------------------------------
// CMsgMailEditorAppUi::CheckFocusForMSK
// ---------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::CheckFocusForMskL(TInt aControl)
    {

    RemoveMSKCommands();
    switch(aControl)
        {
        case EMsgComponentIdTo:
        // FALLTHROUGH
        case EMsgComponentIdCc:
        // FALLTHROUGH
        case EMsgComponentIdBcc:
             SetMiddleSoftKeyLabelL(R_QTN_MSK_ADD,
                EMsgMailEditorCmdMskAddRecipient);
             break;

        case EMsgComponentIdAttachment:
            SetMiddleSoftKeyLabelL(R_QTN_MSK_ATTACHMENT,
                EMsgMailEditorCmdAttachments);
            break;

        default:
            //Change msk label back to normal (context sensitive menu icon)
            SetMiddleSoftKeyLabelL(R_TEXT_SOFTKEY_OPTION,
                EAknSoftkeyContextOptions);
        }

    }

// ---------------------------------------------------------------------------
// CMsgMailEditorAppUi::SetMiddleSoftKeyLabelL
// ---------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::SetMiddleSoftKeyLabelL(
    const TInt aResourceId, const TInt aCommandId )
    {
    CEikButtonGroupContainer* cbaGroup = Cba();
    if ( cbaGroup )
        {
        HBufC* middleSKText = StringLoader::LoadLC( aResourceId );
        TPtr mskPtr = middleSKText->Des();
        cbaGroup->AddCommandToStackL(
            KMSKControlID,
            aCommandId,
            mskPtr );
        CleanupStack::PopAndDestroy( middleSKText );
        }
    }

// -----------------------------------------------------------------------------
// CMsgMailEditorAppUi::RemoveMSKCommands
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::RemoveMSKCommands()
    {
    if ( Cba() )
        {
        Cba()->RemoveCommandFromStack( KMSKControlID,
            EMsgMailEditorCmdMskAddRecipient);
        Cba()->RemoveCommandFromStack( KMSKControlID,
            EMsgMailEditorCmdAttachments );
        Cba()->RemoveCommandFromStack( KMSKControlID,
            EAknSoftkeyContextOptions );
        }
    }

// -----------------------------------------------------------------------------
// CMsgEditorAppUi::InitPhoneBookL
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::InitPhoneBookL()
    {
    // initialize pb
    if ( !iCheckNames )
        {
        TRAP_IGNORE( iCheckNames = CMsgCheckNames::NewL() );
        }
    ASSERT( iCheckNames );
    }

// -----------------------------------------------------------------------------
// CMsgEditorAppUi::SearchUnverifiedStringL
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::SearchUnverifiedStringL(
    const TDesC& aText, CMsgAddressControl* aControl )
    {
    ASSERT( aControl );
    HBufC* buf = NULL;
    iView->SetFocus(aControl->ControlId());
    aControl->GetFirstUnverifiedStringL(buf);
    while (buf && buf->Compare(aText) != 0)
        {
        delete buf;
        buf = NULL;
        aControl->GetNextUnverifiedStringL(buf);
        }
    delete buf;
    buf = NULL;
    }

// -----------------------------------------------------------------------------
// CMsgEditorAppUi::AllAddressValidL
// -----------------------------------------------------------------------------
//
TBool CMsgMailEditorAppUi::AllAddressValidL()
    {
    TInt i(0);
    TBool ret(ETrue);

    CMsgAddressControl* addressField = AddressControl(EMsgComponentIdTo);
    CMsgRecipientArray* rcpt = addressField->GetRecipientsL();

    while (i < rcpt->Count() && ret)
        {
        ret = IsValidAddress (*rcpt->At(i)->Address());
        i++;
        }

    if (ret)
        {
        addressField = AddressControl(EMsgComponentIdCc);
        if(addressField)
            {
            rcpt = addressField->GetRecipientsL();
            i=0;
            while (i < rcpt->Count() && ret)
                {
                ret = IsValidAddress (*rcpt->At(i)->Address());
                i++;
                }
            }
        }

    if (ret)
        {
        addressField = AddressControl(EMsgComponentIdBcc);

        if(addressField)
            {
            rcpt = addressField->GetRecipientsL();
            i=0;
            while (i < rcpt->Count() && ret)
                {
                ret = IsValidAddress (*rcpt->At(i)->Address());
                i++;
                }
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMsgEditorAppUi::SetFocusToFirstFreeFieldL
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::SetFocusToFirstFreeFieldL()
    {
    // Set focus at begining of the message when replying
    if ( Document()->IsReplyMessage() )
        {
        iView->SetFocus(EMsgComponentIdBody);
        }
    // set focus at first free field
    else
        {
        if(AddressControl(EMsgComponentIdTo))
            {
            if (!AddressControl(EMsgComponentIdTo)->Editor().TextLength())
                {
                iView->SetFocus(EMsgComponentIdTo);
                return;
                }
            }
        if (AddressControl(EMsgComponentIdCc))
            {
            if (!AddressControl(EMsgComponentIdCc)->Editor().TextLength())
                {
                iView->SetFocus(EMsgComponentIdCc);
                return;
                }
            }
        if (AddressControl(EMsgComponentIdBcc))
            {
            if (!AddressControl(EMsgComponentIdBcc)->Editor().TextLength())
                {
                iView->SetFocus(EMsgComponentIdBcc);
                return;
                }
            }
        if (SubjectControl())
            {
            if (!SubjectControl()->Editor().TextLength())
                {
                iView->SetFocus(EMsgComponentIdSubject);
                return;
                }
            }
        iView->SetFocus(EMsgComponentIdBody);
        CEikRichTextEditor& editor = BodyControl()->Editor();
        editor.SetCursorPosL(editor.TextLength(), EFalse);
        }
    }

// -----------------------------------------------------------------------------
// CMsgEditorAppUi::ShowSelectedMailboxL
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::ShowSelectedMailboxL()
    {
    const CMsgMailEditorDocument* doc = Document();
    CMsvEntry* rootEntry = doc->Session().GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(rootEntry);

    TInt noteResource(0);
    TInt64 flags = MImumInHealthServices::EFlagSetHealthyReceiving;
    
	noteResource = R_MAIL_EDITOR_MAILBOX_IN_USE_TEXT;
        

    // if more than one box defined, show its name
    if ( Document()->GetHealthyMailboxListL( flags ).Count() > 1 )
        {
        const TMsvId service(doc->SendOptions().ServiceId());
        TMsvSelectionOrdering ordering(rootEntry->SortType());
        ordering.SetShowInvisibleEntries(ETrue);
        rootEntry->SetSortTypeL(ordering);
        const TMsvEntry child = rootEntry->ChildDataL(service);
        HBufC* stringholder = StringLoader::LoadLC(
            noteResource, child.iDetails, iEikonEnv );

        CAknInformationNote* note = new(ELeave) CAknInformationNote(ETrue);
        note->ExecuteLD(*stringholder);
        CleanupStack::PopAndDestroy( stringholder ); // stringholder
        }
    CleanupStack::PopAndDestroy( rootEntry ); // rootEntry
    }

// -----------------------------------------------------------------------------
// CMsgEditorAppUi::RemoveDuplicatesL
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::RemoveDuplicatesL(CMsgAddressControl* aAddressField)
    {
    if ( aAddressField )
    	{
	    CMsgRecipientArray& recs = *aAddressField->GetRecipientsL();
	    TInt count(recs.Count());
	    for (TInt i=0; i < count; i++)
	        {
	        for (TInt j = i + 1; j < count; )
	            {
	            if (recs[i]->Address()->Compare(*(recs[j]->Address())) == 0)
	                {
	                if (recs[i]->Name()->Length())
	                    {
	                    delete recs.At(j);
	                    recs.Delete(j);
	                    }
	                else
	                    {
	                    delete recs.At(i);
	                    recs.Delete(i);
	                    }
	                count = recs.Count();
	                }
	            else
	                {
	                // increase j if something wasn't deleted. if j is
	                // increased when nothing is deleted, elements are skipped
	                j++;
	                }
	            }
	        }
	    aAddressField->RefreshL(recs);
    	}
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::MopSupplyObject(TTypeUid aId)
// Returns a pointer to popup menu.
// ----------------------------------------------------------------------------
//
TTypeUid::Ptr CMsgMailEditorAppUi::MopSupplyObject(TTypeUid aId)
    {
    return SupplyMopObject(aId);
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::StopWaitNote
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::StopWaitNote()
	{
    delete iWaitNote;
    iWaitNote = NULL;
	}

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::StartWaitNoteLC
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::StartWaitNoteLC(
    TInt aStringResourceId, TBool aDelayOff )
    {
    StartWaitNoteLC( aStringResourceId, aDelayOff, R_MEB_WAIT_NOTE );
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::StartWaitNoteLC
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::StartWaitNoteLC(
    TInt aStringResourceId, TBool aDelayOff, TInt aResouceID )
    {
    StopWaitNote();
    iWaitNote = new(ELeave) CAknWaitDialog(
            reinterpret_cast <CEikDialog**>(&iWaitNote), aDelayOff );
    CleanupStack::PushL(TCleanupItem(CleanupWaitDialog, (TAny**)&iWaitNote));
    iWaitNote->PrepareLC( aResouceID );
    HBufC* text = StringLoaderLoadLC( aStringResourceId );
    iWaitNote->SetTextL(*text);
    CleanupStack::PopAndDestroy( text ); // text
    
	// safe to call LD even as a member variable, since sets itself to NULL when deleting
    iWaitNote->RunLD(); // CSI: 50 # see comment above
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::StringLoaderLoadLC
// ----------------------------------------------------------------------------
//
HBufC* CMsgMailEditorAppUi::StringLoaderLoadLC( TInt aTextResource )
    {
    return StringLoader::LoadLC( aTextResource, iEikonEnv );
    }

// -----------------------------------------------------------------------------
// CMsgMailEditorAppUi::SpecialScrolling
// -----------------------------------------------------------------------------
//
TBool CMsgMailEditorAppUi::SpecialScrolling( TUint aKeyCode )
    {
    TBool keyConsumed( EFalse );
    if ( aKeyCode == EStdKeyUpArrow || aKeyCode == EStdKeyDownArrow )
        {
        CMsgBaseControl* focusedControl = iView->FocusedControl();
        if ( focusedControl )
            {
            TInt focused( focusedControl->ControlId() );
            if ( aKeyCode == EStdKeyDownArrow )
                {
                // Focus on Reply-to field??
                if (  focused == EMailEditorControlReplyTo )
                    {
                    // jump directly to atta control
                    LOG( "CMsgMailEditorAppUi::SpecialScrolling - Down" );
                    RotateFocus( ETrue, EMailEditorControlReplyTo );
                    keyConsumed = ETrue;
                    }
                }
            else
                {
                // scroll up, and focus is top of the body part
                if ( focused == EMailEditorControlReplyTo )
                        {
                        LOG( "CMsgMailEditorAppUi::SpecialScrolling - Up" );
                        RotateFocus( EFalse, EMailEditorControlReplyTo );
                        keyConsumed = ETrue;
                        }
                }
            }
        }

    return keyConsumed;
    }

// -----------------------------------------------------------------------------
// CMsgMailEditorAppUi::RotateFocus
// -----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::RotateFocus( TBool aDirection, TInt aCurrentFocus )
    {
    // Mail editor control order
    const TInt TControls[] =
        {
        EMsgComponentIdTo,
        EMsgComponentIdCc,
        EMsgComponentIdBcc,
        EMailEditorControlReplyTo,
        EMsgComponentIdSubject,
        EMsgComponentIdAttachment,
        EMsgComponentIdBody
        };

    const TInt count( sizeof(TControls) / sizeof(TInt) );
    TInt index(0); // index starts from 0
	for(; index<count; ++index)
	    {
	    __ASSERT_DEBUG( index >= 0 && index < count, User::Invariant() );
	    
	    if ( aCurrentFocus == TControls[ index ] )	// CSI: 2 # This is checked above in the assert macro.
	        {
	        // locate currently focused item
	        break;
	        }
	    }
    CMsgBaseControl* control(NULL);
	while ( !control )
	    {
        // Then find next control for current position
        aDirection ? ++index : --index;
        ASSERT( index >= 0 && index < count );
        control = iView->ControlById( TControls[ index ] );	// CSI: 2 # This is checked above in the assert macro.
	    }

	LOG( "CMsgMailEditorAppUi::RotateFocus" );
    iView->SetFocus( control->ControlId() );
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorAppUi::AddRecipientArrayL
// ----------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::AddRecipientArrayL(CMsgAddressControl* aControl,
    const CDesCArray& aArray)
    {
    const TInt count(aArray.Count());
    if ( aControl && count>0 )
    	{
    	CMsgRecipientList* recipientList = CMsgRecipientList::NewL();
    	CleanupStack::PushL(recipientList);

	    for ( TInt index=0; index<count; ++index )
	    	{
	    	TBool aliasFound( EFalse );
	    	TImMessageField msgField;
		    HBufC* address = msgField.GetValidInternetEmailAddressFromString(
		        aArray[index]).AllocLC();
			address->Des().Trim();

            TInt found(KErrNone);
            TPtrC16 aliasPtr = msgField.GetValidAlias(
		        aArray[index], found );
		    HBufC* alias;
		    if ( found  == KErrNone )
		        {
		        alias = aliasPtr.AllocLC();
		        aliasFound = ETrue;
		        }
		    else
		        {
		        alias = KNullDesC().AllocLC();
		        }

		    CMsgRecipientItem* reci =
		    	CMsgRecipientItem::NewLC(*alias, *address);
		    	
			//verified is set according to alias found status
			//as editorbase uses the alias only if verified == ETrue
		    reci->SetVerified( aliasFound );
			
		    recipientList->AppendL(reci);
		    // Ownership transferred just pop
		    CleanupStack::Pop( reci );  // reci
		    CleanupStack::PopAndDestroy( 2, address ); 			// CSI: 47,12 # alias, address
	    	}

        if ( recipientList->Count() )
            {
            aControl->AddRecipientsL(*recipientList);
            }
	    CleanupStack::PopAndDestroy( recipientList ); // recipientList
    	}
    }

//---------------------------------------------------------------------------
//  CMsgMailEditorAppUi::CheckGlobalReplyToSettingL()
//  Gets mailbox reply to address from smtp settings
//  and adds it to mail header if address is different
//  than address of mailbox. Returns ETrue if new value is
//  set.
//---------------------------------------------------------------------------
TBool CMsgMailEditorAppUi::CheckGlobalReplyToSettingL()
    {
    TBool retVal = EFalse;

    CMsgMailEditorDocument& doc = *Document();

    //Get smtp account and load smtp settings
	CEmailAccounts* smtpAccount = CEmailAccounts::NewLC();
    CImSmtpSettings* smtpSet=new(ELeave)CImSmtpSettings();
    CleanupStack::PushL(smtpSet);
    const TMsvId smtpSetId = doc.CurrentEntry().Entry().iServiceId;

    TSmtpAccount accountParams;
    smtpAccount->GetSmtpAccountL( smtpSetId, accountParams );
	smtpAccount->LoadSmtpSettingsL(accountParams, *smtpSet);

    //Check that if reply to address is same as mailbox address
    //do not add "reply to" field for header. Otherwise "reply to" field
    //would be always visible.
    if(smtpSet->ReplyToAddress().Compare(smtpSet->EmailAddress() ) != 0)
        {
        CImHeader& header = doc.HeaderL();
        
        header.SetReplyToL( smtpSet->ReplyToAddress() );

        ASSERT( iMailEditorHeader );
        iMailEditorHeader->UpdateReplyToControlL();

        retVal = ETrue;
        }
    else
        {
        retVal = EFalse;
        }

    CleanupStack::PopAndDestroy(2, smtpAccount);			// CSI: 47,12 # smtpSet

    return retVal;
    }

// ---------------------------------------------------------
// CMsgMailEditorAppUi::SetInputModeToAddressFields
// ---------------------------------------------------------
void CMsgMailEditorAppUi::SetInputModeToAddressFields( TInt aInputMode )
    {
    CMsgAddressControl* ctrl( NULL );
    for ( TInt ctrlId( EMsgComponentIdTo );
          ctrlId <= EMsgComponentIdBcc; ctrlId++ )
        {
        ctrl = AddressControl( ctrlId );
        if ( ctrl )
            {
            ctrl->Editor().SetAknEditorCurrentInputMode( aInputMode );
            }
        }
    }

// ---------------------------------------------------------
// CMsgMailEditorAppUi::SetFocusAfterAddRecipientL
// ---------------------------------------------------------
void CMsgMailEditorAppUi::SetFocusAfterAddRecipientL( TMsgControlId aFocusBeforeAdd )
    {    
	//If the  cursor is in some other place than the address fields when "Add recipient"
	//is selected the cursor remains in this same address field (do nothing). 
     if ( aFocusBeforeAdd == EMsgComponentIdTo ||
    	aFocusBeforeAdd == EMsgComponentIdCc ||
    	aFocusBeforeAdd == EMsgComponentIdBcc )
    	{
    	//Set the cursor to the next empty address field if there is one. 
    	
    	//From "To" to "CC" if available
        if( aFocusBeforeAdd < EMsgComponentIdCc && 
        	AddressControl(EMsgComponentIdCc) &&
        	!AddressControl(EMsgComponentIdCc)->Editor().TextLength() )
            {
            iView->SetFocus(EMsgComponentIdCc);
            }

    	//From "To" or "CC" to "BCC" if available
        else if( aFocusBeforeAdd < EMsgComponentIdBcc && 
        	AddressControl(EMsgComponentIdBcc) &&
        	!AddressControl(EMsgComponentIdBcc)->Editor().TextLength() )
            {
            iView->SetFocus(EMsgComponentIdBcc);
            }
            
    	//To "Subject" if CC and BCC have not been available           
        else if( SubjectControl() && !SubjectControl()->Editor().TextLength() )
            {
			iView->SetFocus(EMsgComponentIdSubject);
            }            
            
	    //In other case cursor is placed into the text body
	    //1) the beginning of the text body if it is empty, or 
	    //2) the end of text body if there is text already.
	    else
	    	{
			iView->SetFocus(EMsgComponentIdBody);
			CEikRichTextEditor& editor = BodyControl()->Editor();
			editor.SetCursorPosL(editor.TextLength(), EFalse);  	    		
	    	}	
    	}
    }
    
// ---------------------------------------------------------------------------
// CMsgMailEditorAppUi::ControlTouchedL
//
// Control has been touched. Invoke appopriate actions
// ---------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CMsgMailEditorAppUi::ControlTouchedL( const CMsgBaseControl& aControl )
	{
    //Check the control id
    TInt touchedControlId = aControl.ControlId();
	switch( touchedControlId )
		{
		//Touch in address fields
		case EMsgComponentIdTo:
		case EMsgComponentIdCc:
		case EMsgComponentIdBcc:
		    {
			//Set focus to touched control (if needed)
			TInt focusedControlId = iView->FocusedControl()->ControlId(); 
			
			if ( focusedControlId != touchedControlId )
				{
				iView->SetFocus( touchedControlId );	
				}

            if(!AreAddressFieldsEmptyL() && !AllAddressValidL())
                {
                CheckRecipientsL(EFalse);
                }
            else
                {
                DoAddRecipientL();    
                }
			break;
		    }
		//Touch in attachment button
		case EMsgComponentIdAttachment:
		    {
		    //Open attachment view
			DoAttachmentL();
			break;
		    }
		default:
			{
			LOG1( "CMsgMailEditorAppUi::ControlTouchedL - Unhandled control! id %d",  touchedControlId );	
			}
		} // switch   		
	}
#endif

// --------------------------------------------------------------------------
// CMsgMailEditorAppUi::HandleEnterKeyL
// Handles enter key events.
// --------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::HandleEnterKeyL()
    {
    if( iView )
        {
        CMsgBaseControl* focusedCtrl = iView->FocusedControl();
        if( focusedCtrl )
            {
            switch( focusedCtrl->ControlId() )
                {
                case EMsgComponentIdAttachment:
                    {
                    // Open attachment view
                    DoAttachmentL();
                    break;
                    }
                default:
                    {
                    // event ignored for other controls.
                    break;
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CMsgMailEditorAppUi::HandleSystemEventL
// Handles system events
// --------------------------------------------------------------------------
//
void CMsgMailEditorAppUi::HandleSystemEventL( const TWsEvent& aEvent )
    {
    TApaSystemEvent* event = ( TApaSystemEvent* )aEvent.EventData();
    if( *event == EApaSystemEventSecureShutdown )
        {
        // When the user is writing a mail while the used memory drive is 
        // removed from the phone, the mail editor is closed and the mail is
        // not saved.
        iForceDeleteDocument = ETrue;
        }

    CMsgMailAppUi::HandleSystemEventL( aEvent );
    }

// End of File
