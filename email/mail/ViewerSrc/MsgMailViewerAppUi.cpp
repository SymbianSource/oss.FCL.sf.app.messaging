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
* Description:  Implements UI class for mail viewer application.
*
*/

#include "MailLog.h"
#include "MsgMailDocument.h"
#include <MsgMailViewer.rsg>
#include "MsgMailViewerAppUi.h"
#include "msgmailviewer.hrh"
#include "MsgMailViewerDocument.h"
#include <MsgMailUIDs.h>                // Application UIDs
#include "MsgMailViewerAttachmentsDlg.h"
#include "MsgMailPreferences.h"
#include "MsgMailViewerCharConv.h"
#include "MailUtils.h"
#include "Msgmailcharactersethandler.h"
#include "MsgMailDRMHandler.h"
#include "CMailCRHandler.h"
#include "CMailViewManager.h"
#include "CMailMessage.h"
#include "mailmessageutils.h"
#include "CMailMessageView.h"
#include "cmailoperation.h"
#include "cmsgmailviewerfinditemhandler.h"
#include "cmsgmailviewerreplytomatcher.h"
#include "cmsgmailviewercontactmatcher.h"
#include "cmsgmailviewercontactcaller.h"
#include <muiu.mbg>
#include <avkon.mbg>
#include <bautils.h>
#include <CMessageData.h>
#include <AknsUtils.h>
#include <eikrted.h>
#include <aknnotewrappers.h>
#include <akncontext.h>
#include <eikmop.h>                     // for SupplyMopObject
#include <MsgEditorView.h>
#include <MsgAddressControl.h>
#include <msvuids.h>                    // KUidMsvFolderEntry
#include <MsgFolderSelectionDialog.h>   // CMsgFolderSelectionDialog
#include <AknWaitDialog.h>
#include <miutpars.h>                   // TImMessageField
#include <mmsvattachmentmanager.h>
#include <muiumsvuiserviceutilitiesinternal.h>
#include <AiwServiceHandler.h>          // aiw
#include <MuiuMsvProgressReporterOperation.h> // progress note
#include <MsgEditorCommon.h>
#include <msgeditor.mbg>
#include <MsgEditorAppUi.rsg>           // resource identifiers
#include <MsgAttachmentModel.h>
#include <MsgEditorCommon.h>
#include <MsgAttachmentInfo.h>
#include <MsgEditorAppUi.hrh>
#include <messagingvariant.hrh>
#include <MsgAttachmentControl.h>
#include <StringLoader.h>
#include <akntitle.h>
#include <akntoolbar.h>
#include <hlplch.h>
#include <csxhelp/mail.hlp.hrh>
#include <SenduiMtmUids.h>
#include <sendui.h>
#include <data_caging_path_literals.hrh>
#include <featmgr.h>
#include <msgvoipextension.h>
#include <CommonUiInternalCRKeys.h>
#include <MsgEditorAppUiExtension.h>// for iMsgEditorAppUiExtension
#include <MsgEditorSettingCacheUpdatePlugin.h>
#include <akninputblock.h>              // CAknInputBlock


// LOCAL CONSTANTS
const TInt KMSKControlID(3);

enum TInternalFlags
	{
    EAttachments = KBit0,
    EUseBaseBodyControl = KBit1
    };


// ================= MEMBER FUNCTIONS =======================
//
CMsgMailViewerAppUi::CMsgMailViewerAppUi()
    : iState(0), iCount(0),
    iResourceLoader(*iCoeEnv),
    iMsgVoIPExtension(NULL),
    iParams(EMailUseDefaultNaviPane),
    iFlags(0),
    iAttachmentLoadingOn(EFalse),
    iDefaultDialerState( EFalse )   // Dialer is disabled by default
    {
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::~CMsgMailViewerAppUi()
// Destructor
// Frees reserved resources
// ------------------------------------------------------------------------------
//
CMsgMailViewerAppUi::~CMsgMailViewerAppUi()
    {
    LOG("~CMsgMailViewerAppUi destructor");
    delete iContactCaller;
    delete iContactMatcher;
    delete iReplyToMatcher;
    delete iFindItemHandler;
    delete iServiceHandler;
    delete iText;
    delete iCharSetHandler;
    iResourceLoader.Close();
    delete iViewManager;
    delete iMsgVoIPExtension;
    delete iTitle;
    // iOperationWatcher can not be deleted if dialog is visible
    if ( iProgressReporter )
    	{
    	TRAP_IGNORE( iProgressReporter->MakeProgressVisibleL( EFalse ) );
    	}
    delete iOperationWatcher;

    if(iOpeningWaitDialog)
	    {
	    TRAP_IGNORE(iOpeningWaitDialog->ProcessFinishedL());
	    }

    LOG("~CMsgMailViewerAppUi destructor end");
    }


// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::ConstructL()
// Symbian OS constructor
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::ConstructL()
    {
    LOGHEAP();
    LOG("CMsgMailViewerAppUi::ConstructL");
    
    // the implementation of this virtual function must call
    // CEikAppUi::ConstructL or CEikAppUi::BaseConstructL
    // before calling any other leaving function
    CMsgMailAppUi::ConstructL();

    //Set us as an observer for the toolbar
    if ( PopupToolbar() )
        {
        PopupToolbar()->SetToolbarObserver( this );
        PopupToolbar()->SetToolbarVisibility( ETrue, EFalse );
        }
    //Create inpu blocker to prevent unwanted exit event when constructing viewer.
    CAknInputBlock::NewLC();
    
    iViewManager = CMailViewManager::NewL();
    iView = CMsgEditorView::NewL(*this, CMsgEditorView::EMsgReadOnly);

    //Open wait dialog
    iOpeningWaitDialog = new(ELeave)CAknWaitDialog(
    		(reinterpret_cast<CEikDialog**>(&iOpeningWaitDialog)), ETrue);

    // safe to call LD even as a member variable, since sets itself to NULL when deleting
    iOpeningWaitDialog->ExecuteLD(R_VIEWER_WAIT_OPEN_NOTE); // CSI: 50 # see comment above

    iMsgVoIPExtension = CMsgVoIPExtension::NewL();

	MenuBar()->SetContextMenuTitleResourceId(R_MSGMAILVIEWER_CONTEXT_MENUBAR);

    iFindItemHandler = CMsgMailViewerFindItemHandler::NewL(
        iView->ItemFinder() );

	iServiceHandler = CAiwServiceHandler::NewL();

    iContactMatcher = CMsgMailViewerContactMatcher::NewL( *iServiceHandler );
    iContactCaller = CMsgMailViewerContactCaller::NewL( *iServiceHandler,
                                                        *iContactMatcher );
    iReplyToMatcher = CMsgMailViewerReplyToMatcher::NewL(*this, *iContactMatcher);

#ifdef RD_MSG_NAVIPANE_IMPROVEMENT
    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( StatusPane( )->ControlL(  TUid::Uid( EEikStatusPaneUidNavi ) ) );
#endif //RD_MSG_NAVIPANE_IMPROVEMENT

    // Reset Dialer's state
    SetDialerEnabled( iDefaultDialerState );

    CleanupStack::PopAndDestroy(); // CAknInputBlock
	LOG("CMsgMailViewerAppUi::ConstructL -> End");
	LOGHEAP();
    }

// ------------------------------------------------------------------------------
// void CMsgMailViewerAppUi::LaunchViewL
// Opens received mail.
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::LaunchViewL()
    {
    LOGHEAP();
    LOG("CMsgMailViewerAppUi::LaunchViewL");
    Document()->MailMessage().LoadMessageL(*this);

    // If mail is in sent folder, then Dialer is enabled
    iDefaultDialerState =
        MailMessageUtils::IsOutgoingL( Document()->MailMessage() );
    SetDialerEnabled( iDefaultDialerState );
    
    // Dims a delete button from the toolbar if the mail is an attachment mail.
    if( IsAttachment() )
    	{
    	if( PopupToolbar() )
    		{
    		PopupToolbar()->SetItemDimmed( EMsgMailViewerCmdDelete, ETrue, ETrue );
    		}
    	}

	LOG("CMsgMailViewerAppUi::LaunchViewL -> End");
	LOGHEAP();
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::::DynInitMenuPaneL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    SetContextBackToMessageL();

	CMsgAddressControl* addrCtrl = FocusedOrFirstAddressControl();
    iFindItemHandler->DynInitMenuPaneL( aResourceId,
                                        aMenuPane,
                                        FirstRecipientAddressL( addrCtrl ),
                                        FirstRecipientNameL( addrCtrl ),
                                        FocusedControlId() );

    iContactCaller->DynInitMenuPaneL( aResourceId,
                                      aMenuPane,
                                      CanMakeCallToContactL() );

    switch ( aResourceId )
        {
        case R_MSGMAILVIEWER_OPTIONS_MENU1:
            {
            DynInitOptionsMenu1L( *aMenuPane );
            break;
            }
        case R_MSGMAILVIEWER_OPTIONS_MENU2:
            {
            DynInitOptionsMenu2L( *aMenuPane );
            break;
            }

        case R_MSGMAILVIEWER_REPLY_MENU:
            {
			// Hide reply to all if only one recipient
			aMenuPane->SetItemDimmed(
				EMsgMailViewerCmdReplyAll,
				DoHideReplyToAll() );

			// Hide reply to sender if from field is empty
			aMenuPane->SetItemDimmed(
				EMsgMailViewerCmdReplySender,
				DoHideReplyToSender() );

			// received mail
            if ( !MailMessageUtils::IsOutgoingL( Document()->MailMessage() ) )
            	{
	             // unieditor
	             aMenuPane->SetItemDimmed(
	                EMsgMailViewerCmdReplyViaUnified,
	                !ValidateServiceL( KSenduiMtmUniMessageUid ) );

	            // Reply via Audio msg
                aMenuPane->SetItemDimmed(
	                EMsgMailViewerCmdReplyViaAudio,
	                !ValidateServiceL( KSenduiMtmAudioMessageUid ) );
            	}
            else
            	{
            	// reply to all is visible in sent items
            	aMenuPane->SetItemDimmed(EMsgMailViewerCmdReplyAll,
            		!HasAnyAddressFieldText());
            	aMenuPane->SetItemDimmed(EMsgMailViewerCmdReplySender, ETrue);
            	}

            break;
            }
        case  R_MSGMAILVIEWER_CHARSET_MENU:
            {
            iCharSetHandler->SetCharacterSetSubMenuItemsL(
                *aMenuPane,
                Document()->Charset());

            break;
            }
        case R_MSGMAILVIEWER_ZOOM_SUBMENU:
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
            {
            break;
            }
        }

    // forwart to Plugin
    MMailMessageView* messageView = iViewManager->ViewPlugin();
    if ( messageView )
    	{
    	messageView->DynInitMenuPaneL(aResourceId, aMenuPane);
    	}
    }


// --------------------------------------------------------------------------------------
// CMsgMailViewerAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
// Handles key press events
// --------------------------------------------------------------------------------------
//
TKeyResponse CMsgMailViewerAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent,
                                                  TEventCode aType)
    {
    // still launching application
    if ( !Document()->IsLaunched() )
        {
        return EKeyWasConsumed;
        }

    if (iView)
        {
        switch (aKeyEvent.iCode)
            {
            case EKeyLeftArrow:
                {
                if ( (iParams & EMailUseDefaultNaviPane) &&
                	IsNextMessageAvailableL(EFalse) )
                    {
                    NextMessageL(EFalse);
                    }
                break;
                }
            case EKeyRightArrow:
                {
                if ( (iParams & EMailUseDefaultNaviPane) &&
                	IsNextMessageAvailableL(ETrue) )
                    {
                    NextMessageL(ETrue);
                    }
                break;
                }
            case EKeyBackspace:
                {
                // Checks if this mail is an attachment mail.
                if( !IsAttachment() )
                	{
                	HandleCommandL(EMsgMailViewerCmdDelete);
                	}
                break;
                }
            case EKeyEnter:
                {
                HandleEnterKeyL();
                break;
                }
            case EKeyYes:
				{
                HandleEKeyYesL();
                // FALLTROUGH
                }
            default:
                {
            	TKeyResponse consumed = SpecialScrolling( aKeyEvent.iCode ) ?
            	    EKeyWasConsumed : iView->OfferKeyEventL(aKeyEvent, aType);
			    if (consumed == EKeyWasNotConsumed)
			    	{
			    	// forwart to Plugin
				    MMailMessageView* plugin = iViewManager->ViewPlugin();
			    	if (plugin && plugin->HandleKeyEventL(
			    		aKeyEvent, aType) == EKeyWasConsumed)
			    		{
			    		consumed = EKeyWasConsumed;
			    		}

			    	}
                return consumed;
                }

            }


        return EKeyWasConsumed;
        }
    else
        {
        // Just in case base class wants to do something
        return CMsgMailAppUi::HandleKeyEventL(aKeyEvent, aType);
        }
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::HandleCommandL(TInt aCommand)
// Command handler
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::HandleCommandL(TInt aCommand)
    {
    // still launching application
    if ( aCommand != EEikCmdExit && !Document()->IsLaunched() )
        {
        return;
        }
    TBool isExternalCmd( EFalse );
    MAIL_ALLOC_TEST( isExternalCmd = HandleExternalCmdL( aCommand ) );
    if ( !isExternalCmd )
        {
        MAIL_ALLOC_TEST( HandleInternalCmdL( aCommand ) );
        }
    }

// -----------------------------------------------------------------------------
// CMsgMailViewerAppUi::HandleNaviDecoratorEventL( TInt aEventID )
// Handles the touch-ui related control events for next/previous arrows
// -----------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CMsgMailViewerAppUi::HandleNaviDecoratorEventL( TInt aEventID )
	{
	if ( AknLayoutUtils::PenEnabled() &&
		 IsNextMessageAvailableL( aEventID == EAknNaviDecoratorEventRightTabArrow ) )
	    {
		NextMessageL( aEventID == EAknNaviDecoratorEventRightTabArrow );
	    }
	}

#endif // RD_SCALABLE_UI_V2

// ----------------------------------------------------------------------------
// CMsgMailViewerAppUi::DynInitToolbarL
// ----------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DynInitToolbarL( TInt /*aResourceId*/, CAknToolbar* /*aToolbar*/ )
	{
	}

// ----------------------------------------------------------------------------
// CMsgMailViewerAppUi::OfferToolbarEventL
// ----------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::OfferToolbarEventL( TInt aCommand )
	{
    HandleCommandL( aCommand );
    }

// ------------------------------------------------------------------------------
// void CMsgMailViewerAppUi::DoMsgSaveExitL.
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DoMsgSaveExitL()
    {
    Exit();
    }

// ------------------------------------------------------------------------------
// void CMsgMailViewerAppUi::DoDeleteL
// Asks confirmation from user and calls DeleteAndExitL
// in MsgEditorAppUi which deletes the message and closes mail viewer.
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DoDeleteL()
    {
    CMsgMailViewerDocument* doc = Document();

    const TMsvId owningService( doc->CurrentEntry().OwningService() );

    if ( owningService == KMsvLocalServiceIndexEntryIdValue )
        { // local entry
        if ( MailUtils::ConfirmationQueryL( R_MAIL_QUEST_DELETE_MSG ) )
            {
            DeleteAndExitL();
            }
        return;
        }
	// check if operation was canceled
    if ( !doc->DeleteMessageL() )
        {
		Exit( EAknSoftkeyBack );
		}
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::HelpContextL
// ------------------------------------------------------------------------------
//
CArrayFix<TCoeHelpContext>* CMsgMailViewerAppUi::HelpContextL( ) const
    {
    CArrayFix<TCoeHelpContext>* helpArray =
                    new( ELeave ) CArrayFixFlat<TCoeHelpContext>( 1 );
    CleanupStack::PushL( helpArray );
    // This uses the editor's UID, because there's only one help file
    helpArray->AppendL( TCoeHelpContext( TUid::Uid( KUidMsgMailEditor ),
        KMAIL_HLP_MAIL_VIEWER() ) );
    CleanupStack::Pop( helpArray );
    return helpArray;
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::AddAttachmentL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::AddAttachmentL(
	CMsvAttachment& aAttachmentInfo,
	TBool aCanBeRemoved )
	{
	if( !iMessageReload )
		{
		CMsgMailViewerDocument* doc = Document();
	    
        // Check if attachment is already added. 
		CMsgAttachmentModel& model = doc->AttachmentModel();
		TBool duplicate = model.IsIdAttached( aAttachmentInfo.Id() );
	    if( duplicate )
	        {
	        // Remove the previous inline attachment. In IMAP email the inline 
	        // image does not always have size information included. So it's
	        // replaced with more informative attachment. Inline attachments are
	        // added first so the duplicate is always a regular attachment.
	        for( TInt i = model.NumberOfItems() - 1; i >= 0; --i )
	            {
	            CMsgAttachmentInfo& info = model.AttachmentInfoAt( i );
	            if( info.AttachmentId() == aAttachmentInfo.Id() )
	                {
	                model.DeleteAttachment( i );
	                break;
	                }
	            }
	        }

	    doc->AddAttachmentL( aAttachmentInfo, aCanBeRemoved );
        if ( doc->MailCRHandler()->MailAttachmentField() && !duplicate )
            {
            DoCreateAttachmentControlL();

	        TParsePtrC fileParser( aAttachmentInfo.AttachmentName() );
	        AttachmentControl()->AppendAttachmentL(
	            fileParser.NameAndExt() );
	        }
	    iFlags |= EAttachments;
	    DoUpdateNaviPaneL();

	    // If attatchment dialog is already opened
	    // update its list also
	    if (iAttachmentdlg)
	    	{
	    	iAttachmentdlg->UpdateAttatchmentListL();
	    	}
		}
	}

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::AddControlL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::AddControlL(TInt aControlType)
	{
    if(! iView->ControlById( aControlType ) )
    	{
	    TInt resourceId(0);
	    TInt controlType(0);

	    switch (aControlType)
	        {
	        case EMsgComponentIdTo:
	            {
	            resourceId = R_MAIL_VIEWER_TO;
	            controlType = EMsgAddressControl;
	            break;
	            }
	        case EMsgComponentIdFrom:
	            {
	            resourceId = R_MAIL_VIEWER_FROM;
	            controlType = EMsgAddressControl;
	            break;
	            }
	        case EMsgComponentIdCc:
	            {
	            resourceId = R_MAIL_VIEWER_CC;
	            controlType = EMsgAddressControl;
	            break;
	            }
	        case EMsgComponentIdBcc:
	            {
	            resourceId = R_MAIL_VIEWER_BCC;
	            controlType = EMsgAddressControl;
	            break;
	            }
	        case EMsgComponentIdSubject:
	            {
	            resourceId = R_MAIL_VIEWER_SUBJECT;
	            controlType = EMsgExpandableControl;
	            break;
	            }
	        default:
	        	User::Leave(KErrNotSupported);
	            break;
	        }
	    iView->AddControlFromResourceL(
	        resourceId, controlType,
	        iState, EMsgHeader);

	    iState = EMsgAppendControl;
    	}
	}
// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::AddControlL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::AddControlL(CCoeControl& aControl)
	{
	iViewManager->AddBodyControlL( iView, aControl );
    iState = EMsgAppendControl;
    iFlags &= ~EUseBaseBodyControl;
	}

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::AddControlL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::AddControlL(MMailMsgBaseControl& aControl)
	{
	iViewManager->AddBodyControlL( iView, aControl );
    iState = EMsgAppendControl;
    iFlags &= ~EUseBaseBodyControl;
	}
// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::ParentControl()
// ------------------------------------------------------------------------------
//
CCoeControl& CMsgMailViewerAppUi::ParentControl()
	{
	return *iView;
	}

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::StatusPane()
// ------------------------------------------------------------------------------
//
CEikStatusPane* CMsgMailViewerAppUi::StatusPane()
	{
	return CAknAppUi::StatusPane();
	}

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::ShowWaitNoteL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::ShowWaitNoteL( TBool aCanCancel )
	{
    if (!iWaitDialog)
        {
        // safe to call LD even as a member variable,
        // since sets itself to NULL when deleting
        iWaitDialog = new(ELeave)CAknWaitDialog(
        		(reinterpret_cast<CEikDialog**>( &iWaitDialog )), ETrue);

        // safe to call LD even as a member variable, since sets itself to NULL when deleting
        iWaitDialog->ExecuteLD(R_VIEWER_WAIT_OPEN_DIALOG_NOTE); // CSI: 50 # see comment above
        iWaitDialog->MakePanelButtonVisible( EAknSoftkeyCancel, aCanCancel );
        }
	}

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::CloseWaitNote()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::CloseWaitNote()
	{
     if (iWaitDialog)
        {
        LOG("CloseWaitNote 1");
        TRAP_IGNORE(iWaitDialog->ProcessFinishedL());
        iWaitDialog = NULL;

        //If attachment dialog exists notify it
        if(iAttachmentdlg)
            {
            iAttachmentdlg->NotifySizeChanged();
            }

        LOG("CloseWaitNote 2");
        }
	}

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::ForwardOperationLL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::ForwardOperationL()
    {
    CMsvSingleOpWatcher* opwatcher = CMsvSingleOpWatcher::NewLC( *this );

    CMsvProgressReporterOperation* reporter =
        CMsvProgressReporterOperation::NewL(
            ( (iFlags & EAttachments) || iAttachmentLoadingOn ), // no delay
            ETrue, // Show wait animation no progress
            Document()->Session(),
            opwatcher->iStatus,
            EMbmAvkonQgn_note_progress );
    opwatcher->SetOperation( reporter ); // Takes ownership
    iProgressReporter = reporter;
    reporter->SetCanCancelL( EFalse ); // there is nothing to cancel
    reporter->SetTitleL( ( (iFlags & EAttachments) || iAttachmentLoadingOn ) ?
        R_QTN_MAIL_WAIT_FETCH_ATT :
        R_VIEWER_WAIT_OPENING );

    iOperation = Document()->ForwardMailMessageL(
        *reporter, reporter->RequestStatus(), iAttachmentLoadingOn );
    reporter->SetOperationL( iOperation ); // Takes ownership
    // don't allow cancel before there is something to cancel
    reporter->SetCanCancelL( ETrue );

    CleanupStack::Pop( opwatcher );
    iOperationWatcher = opwatcher;
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::StartWaitNoteL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::StartWaitNoteL(/*const TDesC& aText*/)
	{
    iAttachmentLoadingOn = ETrue;
    }
// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::StopWaitNote()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::StopWaitNote()
	{
    iAttachmentLoadingOn = EFalse;

    if (iOperation )
        {
        TRAP_IGNORE( iOperation->SuspendL( EFalse ) );
        }
    CloseWaitNote();
	}


// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::SetTitleTextL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::SetTitleTextL(TInt aResourceId)
	{
	CAknTitlePane* titlePane = static_cast<CAknTitlePane *> (
		StatusPane()->ControlL(TUid::Uid(EEikStatusPaneUidTitle)) );

	TResourceReader reader;
	iCoeEnv->CreateResourceReaderLC( reader, aResourceId );
	titlePane->SetFromResourceL(reader);
	CleanupStack::PopAndDestroy(); // CSI: 12 # CreateResourceReaderLC
	}

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::SetAddressFieldTextL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::SetAddressFieldTextL(
	TInt aControlType, CDesCArray& aText)
	{
	LOG("CMsgMailViewerAppUi::SetAddressFieldTextL");
	CMsgAddressControl* addressControl = AddressControl(aControlType);
	if (addressControl)
		{
	    TImMessageField msgField;
	    TInt error = KErrNone;
	    const TInt count(aText.Count());
	    CMsgRecipientList* recips = CMsgRecipientList::NewL();
	    CleanupStack::PushL(recips);
	    for (TInt i=0; i < count; i++)
	        {
	        HBufC* address = msgField.GetValidInternetEmailAddressFromString(
	            aText[i]).AllocLC();
	        HBufC* alias = msgField.GetValidAlias(
	            aText[i], error ).AllocLC();
	        CMsgRecipientItem* reci = CMsgRecipientItem::NewLC(
	            ( error==KErrNone ) ? *alias : KNullDesC(), *address);
	        reci->SetVerified(ETrue);
	        recips->AppendL(reci);
	        CleanupStack::Pop( reci ); // reci, now owned by the array
	        CleanupStack::PopAndDestroy(2,address);	// CSI: 47,12 # alias, address
	        }
	    addressControl->Reset();
	    if (recips->Count())
	        {
	        addressControl->AddRecipientsL(*recips);
	        }
	    CleanupStack::PopAndDestroy( recips );
		}
	}

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::SetAddressFieldTextL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::SetAddressFieldTextL(
	TInt aControlType, const TDesC& aText)
	{
	CMsgAddressControl* addressControl = AddressControl(aControlType);
	if (addressControl)
		{
	    TImMessageField msgField;
	    TInt error = KErrNone;
	    HBufC* address = msgField.GetValidInternetEmailAddressFromString(
	        aText).AllocLC();
	    HBufC* alias = msgField.GetValidAlias(
	        aText, error).AllocLC();
	    addressControl->Reset();

        // restored recipients don't have contact link, so NULL
        MVPbkContactLink* link( NULL );
	    addressControl->AddRecipientL(
	        ( error==KErrNone ) ? *alias : KNullDesC(), *address, ETrue, link );

	    CleanupStack::PopAndDestroy( 2, address );	// CSI: 47,12 # alias, address
		}
	}

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::SetBodyTextL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::SetBodyTextL(CRichText& aText)
	{
	BodyControl()->Reset();
	BodyControl()->SetTextContentL(aText);
	iFlags |= EUseBaseBodyControl;
	DoSetAutomaticHighlightL();
	BodyControl()->Editor().NotifyNewDocumentL();
	}

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::SetSubjectL
// Sets subject text into the subject control.
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::SetSubjectL(const TDesC& aText)
    {
    CMsgExpandableControl* subjectControl = SubjectControl();
    if (subjectControl)
        {
        subjectControl->Reset();
        subjectControl->SetTextContentL(aText);
        }
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::SetContextBackToMessageL
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::SetContextBackToMessageL()
    {
    CMsgMailViewerDocument& doc = *Document();
    CMailMessage* message = &doc.MailMessage();

    //Check that message is created.
    //It might be null if we are just opening the viewer.
    if ( !message )
        {
        User::Leave( KErrNotReady);
        }

    doc.SetEntryWithoutNotificationL(doc.MailMessage().MessageEntry().Id() );

    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::MsgViewRect
// ------------------------------------------------------------------------------
//
TRect CMsgMailViewerAppUi::MsgViewRect(TInt aPane)
	{
	TRect rect(0,0,0,0);

	switch (aPane)
		{
		case 0:
			rect = MsgEditorCommons::MsgBodyPane();
			break;
		case 1:
			rect = MsgEditorCommons::MsgDataPane();
			break;
		case 2:											// CSI: 47 # switch case
			rect = MsgEditorCommons::MsgMainPane();
			break;
		case 3:											// CSI: 47 # switch case
			rect = MsgEditorCommons::MsgHeaderPane();
			break;
		default:
			{
			// return zero size rect
			}
		}
	return rect;
	}

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::AppUiHandleCommandL
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::AppUiHandleCommandL(TInt aCommand)
	{
	HandleCommandL(aCommand);
	}

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::IsNextMessageAvailableL
// ------------------------------------------------------------------------------
//
TBool CMsgMailViewerAppUi::IsNextMessageAvailableL( TBool aForward )
    {
    return CMsgEditorAppUi::IsNextMessageAvailableL( aForward );
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::NextMessageL
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::NextMessageL( TBool aForward )
    {
    CMsgEditorAppUi::NextMessageL( aForward );
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::ViewMessageComplete
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::ViewMessageComplete()
    {
    // Dismiss "opening" waitnote
    if( iOpeningWaitDialog )
        {
        TRAP_IGNORE( iOpeningWaitDialog->ProcessFinishedL() );
        }
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::MessageLoadingL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::MessageLoadingL(TInt aStatus, CMailMessage& aMessage)
	{
	LOG1("CMsgMailViewerAppUi::MessageLoadingL: aStatus:%d", aStatus);
	iViewManager->MessageLoadL(aStatus, aMessage);

	if(aStatus == CMailMessage::ELoadEnd)
		{
		for(TInt index=0; index<iViewManager->PluginCount(); ++index)
			{
			MMailMessageView* viewPlugin = iViewManager->ViewPlugin();
			if (viewPlugin)
				{
				// Load plugin Resource
				LoadResourceFileL( viewPlugin->ResourceFile() );

				TRAPD(error, viewPlugin->ViewMessageL(
					aMessage, *this, iParams));
				if (error == KErrNone)
					{
					break; // success, so break out
					}
				else
					{
					// Try to open with next plugin
					iResourceLoader.Close();
					iViewManager->RemovePlugin();
					if ( !iViewManager->PluginCount() )
					    {
					    ViewMessageComplete();
					    User::LeaveIfError( error );
					    }
					}
				}
			}
        // Context might be somewhere else that in message itself
        SetContextBackToMessageL();

		iView->ExecuteL(ClientRect(),
		        GetFirstAddressControl() != NULL
                ? GetFirstAddressControl()->ControlId() :
                iView->ControlById(EMsgComponentIdSubject) != NULL
                ? EMsgComponentIdSubject : EMsgComponentIdNull);

		if ((iParams & EMailUseDefaultNaviPane))
			{
			DoUpdateNaviPaneL();
			}

	    // observer for Automatic highlight key
	    Document()->MailCRHandler()->SetNotifyHandlerL(this,
				CMailCRHandler::ECommonUiRepository,
				CCenRepNotifyHandler::EIntKey,
	    		KCuiAutomaticHighlight);

		DoSetAutomaticHighlightL();

	    LOGHEAP();
	    }
	}

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::OpCompleted()
// This method is tailor-made for handling completion of forward operation.
// Other CMsvOperations are taken care by the document class.
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::OpCompleted(
    CMsvSingleOpWatcher& aOpWatcher,
	TInt aCompletionCode )
    {
	LOG1("CMsgMailViewerAppUi::OpCompleted:%d", aCompletionCode);
    CMsvOperation* op=&aOpWatcher.Operation();
	const TInt status( op->iStatus.Int() );

    if ( aCompletionCode == EEikCmdExit || aCompletionCode == EAknSoftkeyBack )
        {
        // Exit causes leave with KLeaveExit
        // Leave is trapped in active sheduler thus iOperationWatcher
        // can not be deleted before this call.

        if ( !( iEditorBaseFeatures & EStayInViewerAfterReply ) )
            {
            Exit(aCompletionCode);
            }
        }

    // iOperationWatcher can not be deleted if dialog is visible
    if ( iProgressReporter )
    	{
    	TRAP_IGNORE( iProgressReporter->MakeProgressVisibleL( EFalse ) );
    	iProgressReporter = NULL;
    	}

    if ( iOperationWatcher &&
        iOperationWatcher->Operation().Id()==op->Id() )
        {
        LOG("CMsgMailViewerAppUi::OpCompleted: delete iOperationWatcher");
        delete iOperationWatcher;
        iOperationWatcher = NULL;

        iOperation = NULL;
        }
      }


// ------------------------------------------------------------------------------
// From MProgressDialogCallback called when wait note dismissed
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DialogDismissedL( TInt aButtonId )
   {
   LOG1( "CMsgMailViewerAppUi::DialogDismissedL %d",
       aButtonId );

    if ( aButtonId == EAknSoftkeyCancel )
        {
        delete iAttachmentdlg;
        iAttachmentdlg = NULL;
        iWaitDialog = NULL;
        }
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::BodyTextL()
// ------------------------------------------------------------------------------
//
CRichText* CMsgMailViewerAppUi::BodyTextL()
	{
	return Document()->MailMessage().MessageBody();
	}

// ------------------------------------------------------------------------------
// void CMsgMailViewerAppUi::DoReplyOrForwardLL()
// Replies or forwards a message a message.
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DoReplyOrForwardL(const TInt aOperation)
    {
    CMsgMailViewerDocument& doc = *Document();

    if ( !CheckFFSL( doc.Entry().iSize ) )
        {
        return;
        }
    switch (aOperation)
        {
        case EMsgMailViewerCmdReplySender:
            ReplyL(doc.DefaultMsgFolder(), ETrue);
            break;
        case EMsgMailViewerCmdReplyAll:
            ReplyToAllL(doc.DefaultMsgFolder(), ETrue);
            break;
        case EMsgMailViewerCmdForward:
            {
            ForwardOperationL();
            // calls opcompleted when done
            return;
            }
        default:
            break;
        }

    // check exit setting maintained by base editor (common to all types)
    if ( !( iEditorBaseFeatures & EStayInViewerAfterReply ) )
        {
        Exit(EAknSoftkeyBack);
        }
    }

// ------------------------------------------------------------------------------
// void CMsgMailViewerAppUi::DoAttachmentL()
// Opens an attachment dialog.
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DoAttachmentL()
    {
    //Check if iTitle allready exists
    if(!iTitle)
        {
        iTitle = iCoeEnv->AllocReadResourceLC(
        R_VIEWER_TITLE_ATTACHMENTS);

        CleanupStack::Pop(iTitle);
        }

    CMsgAttachmentModel& attmodel = Document()->AttachmentModel();

    iAttachmentdlg = CMsgMailViewerAttachmentsDlg::NewL(*iTitle,
        attmodel, *Document(), *this );

    iAttachmentdlg->SetMopParent(this);

#ifdef RD_SCALABLE_UI_V2
    //Hide toolbar
    if ( PopupToolbar() )
        {
        DoHideToolbar( ETrue );
        }
#endif // RD_SCALABLE_UI_V2

    // safe to call LD even as a member variable, since sets itself to NULL when deleting
    iAttachmentdlg->ExecuteLD(R_MAIL_VIEWER_ATTACHMENT_DIALOG); // CSI: 50 # see comment above

    LOG("Attachement dialog launchded");

    if(iAttachmentLoadingOn)
        {
        ShowWaitNoteL( ETrue );
        if ( iWaitDialog )
            {
            iWaitDialog->SetCallback( this );
            }
        }
    else
        {
        //Update attachment models fetched-flags
        //if we are not currently loading attachments.
        if(iAttachmentdlg)
            {
            iAttachmentdlg->UpdateAttachmentsFetchFlags();
            }

        }

    // Dialer is always enabled in attachment dialog
    SetDialerEnabled( ETrue );
    }

// ------------------------------------------------------------------------------
// void CMsgMailViewerAppUi::DoMoveL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DoMoveL()
    {
    // Context might be somewhere else that in message itself
    SetContextBackToMessageL();
    CMsgMailViewerDocument& doc = *Document();

    TBool moveMessage(doc.CurrentEntry().OwningService() ==
        KMsvLocalServiceIndexEntryIdValue);

    // Check that file system has enough free space
    TInt size(0);
    if ( !moveMessage )
        {
        size = doc.Entry().iSize;
        }

    if (!CheckFFSL(size))
        {
        return;
        }

    HBufC* title = moveMessage ?
        StringLoader::LoadLC(R_MCE_MOVE_MESSAGES, iCoeEnv) :
        StringLoader::LoadLC(R_MCE_COPY_MESSAGES, iCoeEnv);

    TMsvId selFolder(doc.Entry().Parent());
    const TBool select( CMsgFolderSelectionDialog::SelectFolderL(
        selFolder, *title) );
    CleanupStack::PopAndDestroy( title );

    if (select)
        {
        TMsvId serviceId;
        TMsvEntry destEntry;
        User::LeaveIfError(doc.Session().GetEntry(
            selFolder, serviceId, destEntry));
        const TBool destinationIsRemote(
            KMsvLocalServiceIndexEntryIdValue != serviceId);
        TMsvEntry srcEntry;
        User::LeaveIfError(doc.Session().GetEntry(
            doc.Entry().Parent(), serviceId, srcEntry));
        const TBool sourceIsRemote(
            KMsvLocalServiceIndexEntryIdValue != serviceId);

        if ( !destinationIsRemote && !sourceIsRemote )
            {
            MoveMessageEntryL(selFolder);
            }
        else
            {
            // Handle remote entries
			doc.MoveRemoteEntryL(
			    selFolder, srcEntry.Id(), destinationIsRemote );
            }

        Exit(EAknSoftkeyBack);
        }
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::DoCreateAttachmentControlL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DoCreateAttachmentControlL()
    {
    // first attachment to be added?
    if ( !(iFlags & EAttachments) )
        {
        CMsgAttachmentControl* control =
            CMsgAttachmentControl::NewL( *iView, *iView );
        CleanupStack::PushL( control );
        iView->AddControlL( control,
            EMsgComponentIdAttachment,
            EMsgAppendControl,
            EMsgHeader );
        CleanupStack::Pop( control );
        }
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::DoMessageInfoL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DoMessageInfoL() const
    {
    Document()->ShowMessageInfoL();
    }

// ----------------------------------------------------------------------------
// CMsgMailViewerAppUi::DoHideToolbar()
// ----------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DoHideToolbar( const TBool aHide )
	{
#ifdef RD_SCALABLE_UI_V2
	if ( PopupToolbar() )
        {
        PopupToolbar()->HideItemsAndDrawOnlyBackground( aHide );
        }
#endif // RD_SCALABLE_UI_V
	}

// ----------------------------------------------------------------------------
// CMsgMailViewerAppUi::AttachmentDialogExit()
// ----------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::AttachmentDialogExit()
	{
#ifdef RD_SCALABLE_UI_V2
	if ( PopupToolbar() )
        {
        PopupToolbar()->SetToolbarObserver( this );
        DoHideToolbar( EFalse );
        }
#endif // RD_SCALABLE_UI_V

	// Reset Dialer back to default when Attachment dialog exits
	SetDialerEnabled( iDefaultDialerState );
	// The NaviDecorator has to be moved to a top of NaviPane's stack so that 
	// the navigation arrows refresh correctly. CMsgViewAttachmentsDialog
	// puts an empty NaviDecorator to the top of the stack in constructor and 
	// pops it in destructor so the empty NaviDecorator is moved back to the 
	// top of the NaviPane's stack (PushDefaultL).
	// If the methods leave there is not much possible to do any post-handling 
	// so only TRAP_IGNORE is used to trap the methods if they leave.
	TRAP_IGNORE(
		iNaviPane->PushL( *iNaviDecorator );
		iNaviPane->PushDefaultL());
	}

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::HasAddressFieldText()
// ------------------------------------------------------------------------------
//
TBool CMsgMailViewerAppUi::HasAddressFieldText(
    CMsgAddressControl* aAddressControl) const
    {
    if (aAddressControl)
        {
        CEikRichTextEditor& fromEditor = aAddressControl->Editor();
        if (fromEditor.TextLength() > 0)
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::HasAnyAddressFieldText()
// ------------------------------------------------------------------------------
//
TBool CMsgMailViewerAppUi::HasAnyAddressFieldText() const
	{
    CMsgAddressControl* addressField = AddressControl(EMsgComponentIdTo);

    TBool hasText(HasAddressFieldText(addressField));
    addressField = AddressControl(EMsgComponentIdFrom);
    hasText = hasText || HasAddressFieldText(addressField);
    addressField = AddressControl(EMsgComponentIdCc);
    hasText = hasText || HasAddressFieldText(addressField);
    addressField = AddressControl(EMsgComponentIdBcc);
    hasText = hasText || HasAddressFieldText(addressField);

    return hasText;
	}

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::HasValidFromAddress()
// ------------------------------------------------------------------------------
//
TBool CMsgMailViewerAppUi::HasValidFromAddress() const
    {
    CImHeader* header = Document()->MailMessage().MessageHeader();
    TImMessageField address;
    return address.ValidInternetEmailAddress( header->From() );
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::FromOrToAddressL()
// Returns KNullDesC if address not found
// ------------------------------------------------------------------------------
//
const TDesC& CMsgMailViewerAppUi::FromOrToAddressL() const
    {
    CMsgAddressControl* addressField( NULL );
    if ( !MailMessageUtils::IsOutgoingL( Document()->MailMessage() ) )
        {
        addressField = AddressControl( EMsgComponentIdFrom );
        }
    else
        {
        addressField = AddressControl( EMsgComponentIdTo );
        }
    return FirstRecipientAddressL( addressField );
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::HandleEKeyYesL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::HandleEKeyYesL()
    {
    const TDesC& address( FromOrToAddressL() );


    //Check if call based on email address matching should be done.
    //If VoIP is installed the email address shouldn't be matched.
    if(CanMakeCallToContactL() && !VoipProfileExistL())
        {
        HandleCommandL(EMsgMailViewerCmdCall);
        }
    else
        {
        TBool isBodyFocused( FocusedControlId() == EMsgComponentIdBody );
        CItemFinder::TItemType selectionType(
            iFindItemHandler->SelectionType( isBodyFocused ) );
        // will contain selection text or KNullDesC if nothing selected
        const TDesC& selectionText(
            iFindItemHandler->SelectionText( isBodyFocused ) );

    	if ( selectionType == CItemFinder::EPhoneNumber )
    		{

            //Create VoIP call if pref send key is PS and
            // atleast one VoIP profile.
            if ( IsDefaultSendKeyValueVoIPL() &&
                 VoipProfileExistL() )
                {
                MsvUiServiceUtilitiesInternal::InternetCallToSenderQueryL(
					iView->ItemFinder()->
					CurrentItemExt().iItemDescriptor->Des(),
					KNullDesC, ETrue );
                }
            //otherwise create CS call
            else
                {
                MsvUiServiceUtilitiesInternal::CallToSenderQueryL(
                    iView->ItemFinder()->
                    CurrentItemExt().iItemDescriptor->Des(),
                    KNullDesC,
                    ETrue );
                }
            }
        else if ( VoipProfileExistL() )
            {
            if ( selectionType == CItemFinder::EEmailAddress )
                {
                MsvUiServiceUtilitiesInternal::InternetCallServiceL(
                    address,
                    Document()->MailMessage().MessageHeader()->From(),
                    selectionText,
                    iEikonEnv );
                }
            else if ( address.Length() > 0 )
                { //call to sender
                MsvUiServiceUtilitiesInternal::InternetCallToSenderQueryL(
                    address,
                    KNullDesC,
                    ETrue );
                }
            }
        }


    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::GetRecipientArrayL()
// ------------------------------------------------------------------------------
//
CMsgRecipientArray* CMsgMailViewerAppUi::GetRecipientArrayL(
    CMsgAddressControl* aAddressControl ) const
    {
    CMsgRecipientArray* array(NULL); // prevent compiler warning
    if ( aAddressControl )
        {
        array = aAddressControl->GetRecipientsL();
        }
    return array;
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::DoConvertToCharsetL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DoConvertToCharsetL( TUint aCharset )
    {
    CMsgMailViewerDocument& doc = *Document();
    CMailMessage& message = doc.MailMessage();
    message.Cancel();

	// set override characterset for html message
	message.OverrideHtmlMessageCharset( aCharset );

	// set the wanted character set for plain text message
	message.ConvertToCharsetL( aCharset );

	//Conversion complete, update document character set to current one
    doc.SetCharSet( aCharset );

    //Open wait dialog
    delete iOpeningWaitDialog;
    iOpeningWaitDialog = NULL;
    iOpeningWaitDialog = new(ELeave)CAknWaitDialog(
    		(reinterpret_cast<CEikDialog**>(&iOpeningWaitDialog)), ETrue );
    // safe to call LD even as a member variable,
    // since sets itself to NULL when deleting
    iOpeningWaitDialog->ExecuteLD(R_VIEWER_WAIT_OPEN_NOTE); // CSI: 50 # see comment above

    // HTML mail does not need to add temporary attachment when the mail is
    // being reloaded (hence we have a flag for it).
    // This also prevents the appearance of attachment duplicates when changing
    // characte encoding
    iMessageReload = ETrue;

    message.LoadMessageL(*this);
    }


// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::DoUpdateNaviPaneL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DoUpdateNaviPaneL()
    {
#ifdef RD_MSG_NAVIPANE_IMPROVEMENT
    if ( !iNaviDecorator )
        {
        TMsgEditorMsgPriority priority = EMsgEditorMsgPriorityNormal;

        if ( Document()->MailMessage().MessageEntry().Priority() == EMsvHighPriority )
            {
            priority = EMsgEditorMsgPriorityHigh;
            }
        else if ( Document()->MailMessage().MessageEntry().Priority() == EMsvLowPriority )
            {
            priority = EMsgEditorMsgPriorityLow;
            }

        CreateViewerNaviPaneL( Document()->Entry().iDate, priority, ETrue );
        }

    // if attachment dialog is visible don't update navi pane.
    if( !iAttachmentdlg )
        {
        iNaviPane->PushL( *iNaviDecorator );  // <- This has nothing to do with cleanup-stack!
        }

#else



	CFbsBitmap* bitmap(NULL);
	CFbsBitmap* bitmapMask(NULL);

    // if attachment dialog is visible don't update navi pane.
    if(!iAttachmentdlg)
        {
        if ((iFlags & EAttachments) &&
            !Document()->MailCRHandler()->MailAttachmentField())
            {
    	    TAknsItemID skinID (KAknsIIDQgnPropFolderMessageTab1);
    	    TInt bitmapID(EMbmMsgeditorQgn_prop_folder_message_tab1);
    	    TInt bitmapMaskID(EMbmMsgeditorQgn_prop_folder_message_tab1_mask);

    	    AknsUtils::CreateIconLC( AknsUtils::SkinInstance(),
    	        skinID,
    	        bitmap,
    	        bitmapMask,
    	        KMsgEditorMbm,
    	        bitmapID,
    	        bitmapMaskID );

    	    // navipane takes ownership of the bitmaps
    	    UpdateNaviPaneL(bitmap, bitmapMask);
    	    CleanupStack::Pop( 2, bitmapMask );			// CSI: 47,12 # bitmap, bitmapMask
            }
        else
        	{
       	    TInt bitmapID( 0 );
    	    TInt bitmapMaskID( 0 );
    	    const TMsvPriority messagePriority =
    	        Document()->MailMessage().MessageEntry().Priority();

    	    if ( messagePriority==EMsvHighPriority )
    	        {
      	        bitmapID = EMbmMuiuQgn_indi_mce_priority_high;
                bitmapMaskID = EMbmMuiuQgn_indi_mce_priority_high_mask;
    	        }
            else if ( messagePriority==EMsvLowPriority )
                {
      	        bitmapID = EMbmMuiuQgn_indi_mce_priority_low;
                bitmapMaskID = EMbmMuiuQgn_indi_mce_priority_low_mask;
                }

            if ( bitmapID )
                {
                _LIT(KDirAndMuiuMbmFile,"z:muiu.mbm");
                TParse fileParse;
                fileParse.Set( KDirAndMuiuMbmFile, &KDC_APP_BITMAP_DIR, NULL );
        	    AknsUtils::CreateIconLC( AknsUtils::SkinInstance(),
        	        KAknsIIDQgnPropFolderMessageTab1,
        	        bitmap,
        	        bitmapMask,
        	        fileParse.FullName(),
        	        bitmapID,
        	        bitmapMaskID );

        	    // navipane takes ownership of the bitmaps
        	    UpdateNaviPaneL(bitmap, bitmapMask);
        	    CleanupStack::Pop( 2 bitmapMask );				// CSI: 47,12 # bitmap, bitmapMask
                }
            else
                {
                UpdateNaviPaneL(bitmap, bitmapMask);
                }
        	}
        }



#ifdef RD_SCALABLE_UI_V2
	//Register as observer for decorator events
	if( iNaviDecorator )
		{
		iNaviDecorator->SetNaviDecoratorObserver( this );
		}
#endif


#endif //RD_MSG_NAVIPANE_IMPROVEMENT
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::HandleExternalCmdL()
// Command is offered to external handlers one by one until one of
// them consumes it or no handlers are left.
// ------------------------------------------------------------------------------
//
TBool CMsgMailViewerAppUi::HandleExternalCmdL( TInt aCommand )
    {
    // forwart to Plugin
    MMailMessageView* plugin = iViewManager->ViewPlugin();
    if ( plugin )
    	{
    	if (plugin->HandleCommandL(aCommand))
    		{
    		return ETrue;
    		}
    	}

    if ( iCharSetHandler )
        {
        TUint charSetId;
        if ( iCharSetHandler->GetCharacterSetId( aCommand, charSetId ) )
            {
            DoConvertToCharsetL( charSetId );
            return ETrue;
            }
        }

    if ( iFindItemHandler->CanHandleCommand( aCommand ) )
        {
        iFindItemHandler->HandleFinderCommandL( aCommand );
        return ETrue;
        }

    if ( iContactCaller->CanHandleCommand( aCommand ) )
        {
        iContactCaller->CallToContactL( aCommand, FromOrToAddressL() );
        return ETrue;
        }

    return EFalse;
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::HandleInternalCmdL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::HandleInternalCmdL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EEikCmdExit:
            DoMsgSaveExitL();
            break;
        case EAknSoftkeyClear:
                //FALLTHROGH
        case EMsgMailViewerCmdDelete:
            DoDeleteL();
            break;
        case EAknSoftkeyBack:
            Exit(EAknSoftkeyBack);
            break;
        case EMsgMailViewerCmdReplySender:
            // FALLTHROUGH
        case EMsgMailViewerCmdReplyAll:
            // FALLTHROUGH
        case EMsgMailViewerCmdForward:
            DoReplyOrForwardL(aCommand);
            break;
        case EMsgMailViewerCmdReplyViaUnified:
            iReplyToMatcher->CheckMatchesL(KSenduiMtmUniMessageUid,
            		GetRecipientArrayL(AddressControl( EMsgComponentIdFrom )));
            break;
        case EMsgMailViewerCmdReplyViaAudio:
           iReplyToMatcher->CheckMatchesL(KSenduiMtmAudioMessageUid,
        		   GetRecipientArrayL(AddressControl( EMsgComponentIdFrom )));
            break;
        case EMsgMailViewerCmdAttachments:
            DoAttachmentL();
            break;
        case EMsgMailViewerCmdMove:
            // FALLTHROUGH
        case EMsgMailViewerCmdCopy:
            DoMoveL();
            break;
        case EMsgMailViewerCmdMessageInfo:
            DoMessageInfoL();
            break;
        case EAknCmdHelp:
            // Launch help
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                iEikonEnv->EikAppUi()->AppHelpContextL() );
            break;
		case EMsgMailViewerCmdRetrieve:
			DoRetrieveMessageL();
			break;
        case EMsgDispSizeAutomatic:
        // FALLTROUGH
        case EMsgDispSizeLarge:
        // FALLTROUGH
        case EMsgDispSizeNormal:
        // FALLTROUGH
        case EMsgDispSizeSmall:
            HandleLocalZoomChangeL( (TMsgCommonCommands)aCommand );
            break;
        default:
            CMsgMailAppUi::HandleCommandL( aCommand );
            break;
        }
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::DoHideReplyToAll()
// ------------------------------------------------------------------------------
//
TBool CMsgMailViewerAppUi::DoHideReplyToAll() const
	{
    return !MessageAddresseeCount();
	}

// -----------------------------------------------------------------------------
// CMsgMailViewerAppUi::DoHideReplyToSender()
// -----------------------------------------------------------------------------
//
TBool CMsgMailViewerAppUi::DoHideReplyToSender() const
	{
    // From has text
	CMsgAddressControl* addressField = AddressControl(EMsgComponentIdFrom);
	if ( addressField && HasAddressFieldText( addressField ) )
		{
		return EFalse;
	    }
    return ETrue;
	}

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::MopSupplyObject(TTypeUid aId)
// Returns a pointer to popup menu.
// ------------------------------------------------------------------------------
//
TTypeUid::Ptr CMsgMailViewerAppUi::MopSupplyObject(TTypeUid aId)
    {
    return SupplyMopObject(aId);
    }

// -----------------------------------------------------------------------------
// CMsgMailViewerAppUi::HandleNotifyInt
// -----------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::HandleNotifyInt(
    TUint32  aId,
    TInt aNewValue )
    {
    // Notifies changes on KCuiAutomaticHighlight
    if (aId == KCuiAutomaticHighlight)
    	{
    	TRAP_IGNORE(SetAutomaticHighlightL( aNewValue ));
    	}
    }

// -----------------------------------------------------------------------------
// CMsgMailViewerAppUi::HandleNotifyError
// -----------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::HandleNotifyError(
    TUint32 /* aId */,
    TInt /* aError */,
    CCenRepNotifyHandler* /* aHandler */ )
    {
    // Do nothing
    LOG("CMsgMailViewerAppUi::HandleNotifyError");
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::GetCharsetIdL()
// ------------------------------------------------------------------------------
//
TUint CMsgMailViewerAppUi::GetCharsetIdL() const
    {
    //If this is a HTML mail then return the charset in html part
    if ( Document()->MailMessage().MhtmlPartId() != 0)
    	{
    	return Document()->MailMessage().HtmlCharsetIdL();
    	}
    //else the regular
    else
    	{
    	return Document()->MailMessage().CharsetIdL();
    	}
    }

// -----------------------------------------------------------------------------
// CMsgMailViewerAppUi::GetFirstAddressControl
// -----------------------------------------------------------------------------
//
CMsgAddressControl* CMsgMailViewerAppUi::GetFirstAddressControl(
    TBool aDirection ) const
	{
	CMsgAddressControl* addressField( NULL );
	TInt controlId( aDirection ? EMsgComponentIdTo : EMsgComponentIdBcc );
	TInt count( 1 + ( EMsgComponentIdBcc - EMsgComponentIdTo ) );
	for ( TInt i( 0 ); i < count; i++ )
	    {
	    addressField = AddressControl( controlId );
	    if ( addressField )
	        { // found the first address, stop looping
	        break;
	        }
	    aDirection ? ++controlId : --controlId;
	    }
	return addressField;
	}

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::FirstRecipientAddressL()
// Returns KNullDesC if addresscontrol is NULL or contains no addresses
// ------------------------------------------------------------------------------
//
const TDesC& CMsgMailViewerAppUi::FirstRecipientAddressL(
    CMsgAddressControl* aAddressControl ) const
    {
    CMsgRecipientArray* array = GetRecipientArrayL( aAddressControl );
    if ( array && array->Count() > 0 )
        {
	    return ( *( *array )[0]->Address() ); // first address
        }
    else
    	{
    	return KNullDesC();
    	}
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::FirstRecipientNameL()
// Returns KNullDesC if addresscontrol is NULL or contains no addresses
// ------------------------------------------------------------------------------
//
const TDesC& CMsgMailViewerAppUi::FirstRecipientNameL(
    CMsgAddressControl* aAddressControl ) const
    {
    CMsgRecipientArray* array = GetRecipientArrayL( aAddressControl );
    if ( array && array->Count() > 0 )
        {
	    return ( *( *array )[0]->Name() ); // first name
        }
    else
    	{
    	return KNullDesC();
    	}
    }


// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::DoReplyOptionsMenuL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DoReplyOptionsMenuL(CEikMenuPane& aMenuPane)
    {
	if ( HasValidFromAddress() )
	    {
	    if ( MessageAddresseeCount() ||
	        ValidateServiceL( KSenduiMtmMmsUid ) ||
	        ValidateServiceL( KSenduiMtmAudioMessageUid ) ||
	        ValidateServiceL( KSenduiMtmUniMessageUid ) ||
	        MailMessageUtils::IsOutgoingL( Document()->MailMessage() ) )
	        {
	        // Reply with submenu
            aMenuPane.SetItemDimmed(
    	        EMsgMailViewerCmdReply, EFalse );
	        }
	    else
	        {
	        // Reply to sender
	        aMenuPane.SetItemDimmed(
    	            EMsgMailViewerCmdReplySender, EFalse );
	        }
	    }
    }


// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::DoReplyViaL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DoReplyViaL( TUid aMtmUid, const HBufC* aMatch, const HBufC* aName)
    {
    CMsgAddressControl* from = AddressControl( EMsgComponentIdFrom );
    CMsgRecipientArray* recs = GetRecipientArrayL( from );
    if ( recs )
        {
        CMessageData* messageData = CMessageData::NewL();
	    CleanupStack::PushL(messageData);

        if( aMatch )
            {
            //We have found matching sender address and that should be used

            if( !aName )
                {
                //No alias name found. Just matching address added to messageData.
                messageData->AppendToAddressL(*aMatch);
                }
            else
                {
                //Both address and alias added to messageData.
                messageData->AppendToAddressL(*aMatch, *aName);
                }
            }
        else
            {
            TInt count( recs->Count() );

            for (TInt index = 0; index < count; index++)
                {
                messageData->AppendToAddressL(
                	*recs->At(index)->Address(),
                	*recs->At(index)->Name());
                }
            }
        HBufC* subjectText(NULL);
        CMsgExpandableControl* subjectControl = SubjectControl();
        if ( subjectControl )
            {
            HBufC* buf = subjectControl->Editor().GetTextInHBufL();
            // editor returns null pointer if subject line is empty,
            // we must handle that
            if ( !buf )
                {
                buf = HBufC::NewL( 0 );
                }
            CleanupStack::PushL( buf );
            // base editor provides a service which removes unneeded prefixes
            subjectText = CreateSubjectPrefixStringL( *buf, ETrue );
            CleanupStack::PopAndDestroy( buf );
            CleanupStack::PushL( subjectText );
            }
    	messageData->SetSubjectL( subjectText );
    	CleanupStack::PopAndDestroy( subjectText );

		Document()->SendUiL().CreateAndSendMessageL(
		    aMtmUid, messageData );
    	CleanupStack::PopAndDestroy(messageData); // messageData

        // check exit setting maintained by base editor (common to all types)
        if ( !( iEditorBaseFeatures & EStayInViewerAfterReply ) )
            {
            Exit(EAknSoftkeyBack);
            }
        }
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::DoRetrieveMessageL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DoRetrieveMessageL()
    {
    CMsgMailViewerDocument* doc = Document();
	doc->RetrieveMessageL();

    // Refresh body text. HTML or plain text
    doc->MailMessage().LoadMessageL(*this);
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::SetAutomaticHighlightL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::SetAutomaticHighlightL( TInt aValue )
    {
    if ( iFlags & EUseBaseBodyControl )
        {
        // content highlight
        if ( aValue )
            { // switch ON
            iFindItemHandler->SetFindModeL( CItemFinder::EPhoneNumber |
                                            CItemFinder::EUrlAddress |
                                            CItemFinder::EEmailAddress );
            }
        else
            { // switch OFF
            iFindItemHandler->SetFindModeL( CItemFinder::ENoneSelected );
            }
        // header highlight
        SetAddressControlHighlight(
            AddressControl(EMsgComponentIdBcc), aValue );
        SetAddressControlHighlight(
            AddressControl(EMsgComponentIdCc), aValue );
        SetAddressControlHighlight(
            AddressControl(EMsgComponentIdFrom), aValue );
        SetAddressControlHighlight(
            AddressControl(EMsgComponentIdTo), aValue );
        }
    }

// ------------------------------------------------------------------------------
// CMsgMailViewerAppUi::DoSetAutomaticHighlightL()
// ------------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DoSetAutomaticHighlightL()
    {
	TInt value(0);
	TInt crError = Document()->MailCRHandler()->GetCRInt(
			KCRUidCommonUi,
			KCuiAutomaticHighlight,
			value );
	SetAutomaticHighlightL(value);
    }

// -----------------------------------------------------------------------------
// CMsgMailViewerAppUi::SetAddressControlHighlight
// -----------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::SetAddressControlHighlight(
            CMsgAddressControl* aControl, TInt aValue )
    {
    if ( aControl )
        {
        aControl->SetAddressFieldAutoHighlight( aValue );
        }
    }


// -----------------------------------------------------------------------------
// CMsgMailViewerAppUi::DynInitOptionsMenu1L
// -----------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DynInitOptionsMenu1L(CEikMenuPane& aMenuPane)
    {
	// Reply option
    DoReplyOptionsMenuL( aMenuPane );
    
    // Hides a Delete option from options menu if opened mail
    // is an attachment mail.
    if( IsAttachment() )
    	{
    	aMenuPane.SetItemDimmed( EMsgMailViewerCmdDelete, ETrue );
    	}
    }

// -----------------------------------------------------------------------------
// CMsgMailViewerAppUi::DynInitOptionsMenu2L
// -----------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::DynInitOptionsMenu2L(CEikMenuPane& aMenuPane)
    {
    CMsgMailViewerDocument* doc = Document();

    if ( !(iCommonFlags & EWestern) )
        {
        if ( !iCharSetHandler )
            {
            //Character set handler is now created, it is possible
            //that character set changes if the user selects to do so
            //Current character set must now be checked
		    if( Document()->Charset() == 0 )
			    {
			    Document()->SetCharSet( GetCharsetIdL() );
			    }

            //Create the character set handler
            iCharSetHandler = CMsgMailCharacterSetHandler::NewL(
                EMsgMailViewerFirstCharacterSetCmd);
            }
        iCharSetHandler->SetCharacterSetMenuItemL( aMenuPane );
        }

    // check whether the mail is filtered
    if (!Document()->MailMessage().MessageEntry().PartialDownloaded())
        {
        aMenuPane.DeleteMenuItem( EMsgMailViewerCmdRetrieve );
        }

    // hide Attachments, if message does not have any
    aMenuPane.SetItemDimmed(
    	EMsgMailViewerCmdAttachments,
    	!( (iFlags & EAttachments) || iAttachmentLoadingOn ) );

    if ( doc->MailCRHandler()->MailCopyMoveSupport() )
        {
        // show copy or move depending on if message is local or not
        if ( doc->CurrentEntry().OwningService() ==
            KMsvLocalServiceIndexEntryIdValue )
            {
            aMenuPane.SetItemDimmed( EMsgMailViewerCmdMove, EFalse );
            }
        else
            {
            aMenuPane.SetItemDimmed( EMsgMailViewerCmdCopy, EFalse );
            }
        }
    if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ))
        {
        aMenuPane.SetItemDimmed( EAknCmdHelp, ETrue );
        }
    }


// -----------------------------------------------------------------------------
// CMsgMailViewerAppUi::IsDefaultSendKeyValueVoIPL
// -----------------------------------------------------------------------------
//
TBool CMsgMailViewerAppUi::IsDefaultSendKeyValueVoIPL() const
    {
    return iMsgVoIPExtension->IsPreferredTelephonyVoIP();
    }

// -----------------------------------------------------------------------------
// CMsgMailViewerAppUi::LoadResourceFileL
// -----------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::LoadResourceFileL(const TDesC& aResourceName)
	{
	if ( aResourceName.Length() )
		{
		TParse* fp = new(ELeave) TParse();
    	fp->Set(aResourceName, &KDC_RESOURCE_FILES_DIR, NULL);
    	TFileName fileName(fp->FullName());
    	BaflUtils::NearestLanguageFile(
    		iEikonEnv->FsSession() ,fileName);
    	LOG1("CMsgMailViewerAppUi iResourceLoader.Open:%S",
    		&fileName);
    	delete fp;
    	// only one resource can be opened, so close previous
    	iResourceLoader.Close();
		TInt err = iResourceLoader.Open(fileName);
		LOG1("iResourceLoader.Open:%d", err);
		}
	}

// -----------------------------------------------------------------------------
// CMsgMailViewerAppUi::FocusedControlId
// returns EMsgComponentIdNull if nothing focused
// -----------------------------------------------------------------------------
//
TMsgControlId CMsgMailViewerAppUi::FocusedControlId() const
    {
    TMsgControlId retVal = EMsgComponentIdNull;
    CMsgBaseControl* ctrl = iView->FocusedControl();
    if ( ctrl )
        {
        retVal = static_cast<TMsgControlId>( ctrl->ControlId() );
        }
	return retVal;
    }

// -----------------------------------------------------------------------------
// CMsgMailViewerAppUi::ValidateServiceL
// -----------------------------------------------------------------------------
//
TBool CMsgMailViewerAppUi::ValidateServiceL( TUid aUid )
    {
    // return ETrue also when service settings are not set
    TSendingCapabilities dummy;
    TInt error = Document()->SendUiL().ServiceCapabilitiesL( aUid, dummy );
    return ( error == KErrNone );
    }

// -----------------------------------------------------------------------------
// CMsgMailViewerAppUi::VoipProfileExistL
// -----------------------------------------------------------------------------
//
TBool CMsgMailViewerAppUi::VoipProfileExistL() const
    {
    return iMsgVoIPExtension->VoIPProfilesExistL();
    }

// -----------------------------------------------------------------------------
// CMsgMailViewerAppUi::MessageAddresseeCount
// -----------------------------------------------------------------------------
//
TInt CMsgMailViewerAppUi::MessageAddresseeCount() const
    {
    TInt count(0);
    CImHeader* header = Document()->MailMessage().MessageHeader();
	ASSERT( header );
    count = header->CcRecipients().Count();
    // Add count only if several to recipients.
    count += header->ToRecipients().Count() > 1 ?
        header->ToRecipients().Count() : 0;
    return count;
    }

// ---------------------------------------------------------------------------
// CMsgMailViewerAppUi::SetMiddleSoftKeyLabelL
// ---------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::SetMiddleSoftKeyLabelL(
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
// CMsgMailViewerAppUi::RemoveMSKCommands
// -----------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::RemoveMSKCommands()
    {
    if ( Cba() )
        {
        Cba()->RemoveCommandFromStack( KMSKControlID, EMsgMailViewerCmdAttachments );
        Cba()->RemoveCommandFromStack( KMSKControlID, EAknSoftkeyContextOptions );
        }
    }

// -----------------------------------------------------------------------------
// CMsgMailViewerAppUi::SpecialScrolling
// -----------------------------------------------------------------------------
//
TBool CMsgMailViewerAppUi::SpecialScrolling( TUint aKeyCode )
    {
    TBool keyConsumed( EFalse );
    // last address is the first when direction is EFalse, may also be NULL
    CMsgAddressControl* lastAddressControl( GetFirstAddressControl( EFalse ) );
    if ( aKeyCode == EKeyDownArrow || aKeyCode == EKeyUpArrow )
        {
        CMsgBaseControl* focusedControl = iView->FocusedControl();
        if ( focusedControl )
            {
            TInt focused( focusedControl->ControlId() );
            if ( aKeyCode == EKeyDownArrow )
                {
                // Focus in on last address field??
                if (  lastAddressControl &&
                      focused == lastAddressControl->ControlId() )
                    {
                    // jump directly to atta control
                    if ( iView->ControlById( EMsgComponentIdAttachment ) )
                        {
                        iView->SetFocus( EMsgComponentIdAttachment );
                        keyConsumed = ETrue;
                        }
                    // or directly to body
                    else if ( BodyControl() )
                        {
                        iView->SetFocus( BodyControl()->ControlId() );
                        keyConsumed = ETrue;
                        }
                    }
                }
            else
                {
                // scroll up, and focus is top of the body part
                if ( focused == EMsgComponentIdBody &&
                        BodyControl()->IsFocusChangePossible( EMsgFocusUp ) )
                        {
                        // Move focus to atta control
                        if ( iView->ControlById( EMsgComponentIdAttachment ) )
                            {
                            iView->SetFocus( EMsgComponentIdAttachment );
                            keyConsumed = ETrue;
                            }
                        // or move to the last address field if one exists
                        else if ( lastAddressControl )
                            {
                            iView->SetFocus( lastAddressControl->ControlId() );
                            keyConsumed = ETrue;
                            }
                        }
                // scroll up when focus is on the attachment control and
                // one address control exists
                else if ( focused == EMsgComponentIdAttachment &&
                          lastAddressControl )
                    {
                    // Move focus to last address control
                    iView->SetFocus( lastAddressControl->ControlId() );
                    keyConsumed = ETrue;
                    }
                }
            }
        }

    return keyConsumed;
    }
// ----------------------------------------------------------------------------
// void CMsgMailViewerAppUi::EditorObserver()
// ----------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::EditorObserver( TMsgEditorObserverFunc aFunc,
                                            TAny* aArg1,
                                            TAny* aArg2,
                                            TAny* aArg3)
    {
	if ( aFunc == MMsgEditorObserver::EMsgHandleFocusChange )
		{
    	ASSERT( aArg1 );
        TMsgControlId controlId = FocusedControlId();
    	CMsgBaseControl* focusedControl = iView->FocusedControl();

        TInt err(0);

        if ( Document()->MailCRHandler()->MailAttachmentField() &&
             controlId == EMsgComponentIdAttachment &&
             iLastControlId != EMsgComponentIdAttachment )
            {
            RemoveMSKCommands();

            //Change msk label to "attachments"
            TRAP(err, SetMiddleSoftKeyLabelL(R_QTN_MSK_ATTACHMENT, EMsgMailViewerCmdAttachments));
            }
         //If last control was atta we must change back to context options menu.
         else if( iLastControlId == EMsgComponentIdAttachment &&
                  controlId != EMsgComponentIdAttachment )
            {
            RemoveMSKCommands();

            //Change msk label back to normal (context sensitive menu icon)
            TRAP(err, SetMiddleSoftKeyLabelL(R_TEXT_SOFTKEY_OPTION, EAknSoftkeyContextOptions));
            }


		if(err == KErrNone)
		    {
		    //Collect current id, may be EMsgComponentIdNull
		    iLastControlId = controlId;
		    }

		}
#ifdef RD_SCALABLE_UI_V2
	else if( aFunc == MMsgEditorObserver::EMsgControlPointerEvent )
		{
		if( aArg1 && aArg2 )
			{
            TPointerEvent* event = static_cast<TPointerEvent*>( aArg2 );
            CMsgBaseControl* control = static_cast<CMsgBaseControl*>( aArg1 );

            if ( event->iType == TPointerEvent::EButton1Down )
                {
                iFocusedControl = control;
                }
            else if ( event->iType == TPointerEvent::EButton1Up )
                {
				if( control && iFocusedControl == control &&
					control->ControlId() == EMsgComponentIdAttachment )
					{
					//Summon touch functionality
					TRAP_IGNORE( DoAttachmentL() );
					}
                iFocusedControl = NULL;
                }
			}
		}
#endif
    CMsgEditorAppUi::EditorObserver(aFunc, aArg1, aArg2, aArg3);
    }

// --------------------------------------------------------------------------
// CMsgMailViewerAppUi::CanMakeCallToContactL
// --------------------------------------------------------------------------
//
TBool CMsgMailViewerAppUi::CanMakeCallToContactL() const
    {
    TBool retVal( EFalse );
    // We can show call option in following situations.
    //From Mail Ui Specification:
    //Send key initiates a voice call
    //creation functionality for the sender in case
    //the focus is in From field, when focus is not in
    //a phonenumber or in case the auto-highlight
    //functionality is set off.
    //Send key is inactive is inactive,
    //when message is opened from Sent items
    //folder .

    TBool isBodyFocused( FocusedControlId() == EMsgComponentIdBody );
    TBool isFromFocused( FocusedControlId() == EMsgComponentIdFrom );

#if defined(__VOIP) && defined(RD_VOIP_REL_2_2)
    if ( !MailMessageUtils::IsOutgoingL( Document()->MailMessage() ) &&
         ( !isBodyFocused ) )
        {
        retVal = ETrue;
        }
#else
    CItemFinder::TItemType selectionType(
            iFindItemHandler->SelectionType( isBodyFocused ) );

    if ( !MailMessageUtils::IsOutgoingL( Document()->MailMessage() ) &&
         ( isFromFocused || selectionType != CItemFinder::EPhoneNumber ) )
        {
        retVal = ETrue;
        }
#endif // __VOIP && RD_VOIP_REL_2_2

    return retVal;
    }

// --------------------------------------------------------------------------
// CMsgMailViewerAppUi::FocusedOrFirstAddressControl
// This method returns empty string if GetFirstAddressControl() returns NULL
// --------------------------------------------------------------------------
//
CMsgAddressControl* CMsgMailViewerAppUi::FocusedOrFirstAddressControl() const
    {
    CMsgAddressControl* addressControl( NULL );
    TInt controlId( FocusedControlId() );
	if ( controlId == EMsgComponentIdTo ||
		 controlId ==  EMsgComponentIdFrom ||
		 controlId ==  EMsgComponentIdCc ||
		 controlId ==  EMsgComponentIdBcc )
	    {
        addressControl = AddressControl( controlId );
        }
    else
        {
        addressControl = GetFirstAddressControl();
        }
    return addressControl;
    }

// --------------------------------------------------------------------------
// CMsgMailViewerAppUi::IsAttachment
// --------------------------------------------------------------------------
//
TBool CMsgMailViewerAppUi::IsAttachment() const
	{
	CMsgMailViewerDocument* doc = Document();
	
	TMsvId parentId = doc->Entry().Parent();
    TMsvId serviceId;
    TMsvEntry entry;
    
    // Have to get parent's parent.
    doc->Session().GetEntry(parentId, serviceId, entry);
    parentId = entry.Parent();
    doc->Session().GetEntry(parentId, serviceId, entry);
    
    if( entry.Attachment() )
    	{
    	return ETrue;
    	}
    else
    	{
    	return EFalse;
    	}
	}

// --------------------------------------------------------------------------
// CMsgMailViewerAppUi::HandleEnterKeyL
// Handles enter key events.
// --------------------------------------------------------------------------
//
void CMsgMailViewerAppUi::HandleEnterKeyL()
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
                    // Display context menu
                    CEikMenuBar* menu = MenuBar();
                    if( menu )
                        {
                        menu->TryDisplayContextMenuBarL();
                        }
                    break;
                    }
                }
            }
        }
    }

// ---------------------------------------------------------
// CMsgMailViewerAppUi::HandleEntryDeletedL
//
// HandleEntryDeletedL is called when the currently open message
// has been deleted by some other messaging client.
// ---------------------------------------------------------
//
void CMsgMailViewerAppUi::HandleEntryDeletedL()
    {
    //If message has been deleted while same entry is open on viewer
    //we need to close viewer and go back to mce.
    Exit(EAknSoftkeyBack);
    }

// End of File
