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
* Description:  Dialog used for attachment handling
*
*/


// INCLUDE FILES

#include "MsgMailViewAttachmentsDlg.h"
#include "msgmaileditor.hrh"
#include "MsgMailEditorAppUi.h"
#include <MsgMailUIDs.h>    // Application Uid
#include "MsgMailEditorDocument.h"
#include "MailUtils.h"
#include "CMailCRHandler.h"

#include <akntoolbar.h>
#include <akntoolbarextension.h>
#include <StringLoader.h>

#include <MsgMailEditor.rsg>
#include <MsgEditorAppUi.hrh>
#include <MsgAttachmentModelObserver.h>
#include <MsgAttachmentModel.h>
#include <MsgAttachmentInfo.h>
#include <MsgEditorAppUi.rsg>

#include <hlplch.h>
#include <csxhelp/mail.hlp.hrh>

#include <MsgMailEditor.rsg>
#include <aknlistquerydialog.h>


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMsgMailViewAttachmentsDlg::NewL
// -----------------------------------------------------------------------------
//
CMsgMailViewAttachmentsDlg* CMsgMailViewAttachmentsDlg::NewL(TDesC& aTitle,		// CSI: 61 # aTitle must be a non-const
																				// because of CMsgViewAttachmentsDialog
																				// takes a non-const parameter.
            CMsgAttachmentModel& aAttachmentModel, CMsgMailEditorAppUi& aAppUi,
            TBool aHelpSupported)
    {
    CMsgMailViewAttachmentsDlg* self = new (ELeave) CMsgMailViewAttachmentsDlg(
        aTitle,
        R_ATTACHMENTS_MENU,
        aAttachmentModel,
        aAppUi );
    
    CleanupStack::PushL(self);
    self->ConstructL(self->iAppUi.Document()->CurrentEntry());
    CleanupStack::Pop( self );
    self->iHelpSupported = aHelpSupported;
    return self;
    }

// -----------------------------------------------------------------------------
// CMsgMailViewAttachmentsDlg::CMsgMailViewAttachmentsDlg
// -----------------------------------------------------------------------------
//
CMsgMailViewAttachmentsDlg::CMsgMailViewAttachmentsDlg(
    TDesC& aTitle, TInt aMenuId,
    CMsgAttachmentModel& aAttachmentModel, CMsgMailEditorAppUi& aAppUi)
    :CMsgViewAttachmentsDialog(aTitle, aMenuId, aAttachmentModel),
    iAppUi(aAppUi)
    {
    // Dialer is enabled in attachment dialog
    iAppUi.SetDialerEnabled( ETrue );    
    }

// -----------------------------------------------------------------------------
// CMsgMailViewAttachmentsDlg::CMsgMailViewAttachmentsDlg
// -----------------------------------------------------------------------------
//
CMsgMailViewAttachmentsDlg::~CMsgMailViewAttachmentsDlg()
    {
    // Dialer is disabled by default
    iAppUi.SetDialerEnabled( EFalse );
    }

// -----------------------------------------------------------------------------
// CMsgMailViewAttachmentsDlg::DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CMsgMailViewAttachmentsDlg::DynInitMenuPaneL(
    TInt aMenuId, CEikMenuPane* aMenuPane)
    {
    CMsgViewAttachmentsDialog::DynInitMenuPaneL( aMenuId, aMenuPane );

    if (aMenuId == R_MEB_ATTACHMENTS_MENUPANE)
        {
        if (!iAttachmentModel.NumberOfItems())
            {
            aMenuPane->DeleteMenuItem(EMsgViewAttachmentCmdRemove);
            aMenuPane->DeleteMenuItem(EMsgViewAttachmentCmdOpen);
            }
        if ( iAppUi.Document()->MailCRHandler()->MailInsertToolBar() )
            {
            aMenuPane->SetItemDimmed(
                EMsgMailEditorCmdInsertMenu, EFalse );
            }
        else
            {
            aMenuPane->SetItemDimmed(
                EMsgMailEditorCmdInsertSubMenu, EFalse );
            }
        aMenuPane->SetItemDimmed( EAknCmdHelp, !iHelpSupported);
        }
    }

// -----------------------------------------------------------------------------
// CMsgMailViewAttachmentsDlg::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CMsgMailViewAttachmentsDlg::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    LOG1("CMsgMailViewAttachmentsDlg::OfferKeyEventL %d ", aKeyEvent.iCode );
    
    // Consume send keys, so that they are not handled in the appui
    if( aType == EEventKey && aKeyEvent.iCode == EKeyYes )
        {
        return EKeyWasConsumed;
        }
    
    TKeyResponse consumed( EKeyWasNotConsumed );
    if ( iAppUi.PopupToolbar() && iAppUi.PopupToolbar()->IsShown() )
    	{
        LOG1("CMsgMailViewAttachmentsDlg::OfferKeyEventL - TB %d ",
    	    aKeyEvent.iCode );

    	consumed = iAppUi.PopupToolbar()->OfferKeyEventL( aKeyEvent, aType );
    	}

    if ( consumed == EKeyWasNotConsumed )
        {
        if ( !MenuShowing() && aType == EEventKey )
            {
            if (aKeyEvent.iCode == EKeyOK)
                {
                ProcessCommandL(EMsgViewAttachmentCmdOpen);
                return EKeyWasConsumed;
                }
            if (aKeyEvent.iCode == EKeyBackspace &&
                iAttachmentModel.NumberOfItems())
                {
                ProcessCommandL(EMsgViewAttachmentCmdRemove);
                return EKeyWasConsumed;
                }
            }
        consumed = CMsgViewAttachmentsDialog::OfferKeyEventL(aKeyEvent, aType);
        }
    return consumed;
    }

// -----------------------------------------------------------------------------
// CMsgMailViewAttachmentsDlg::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CMsgMailViewAttachmentsDlg::ProcessCommandL(TInt aCommandId)
    {
    LOG1("CMsgMailViewAttachmentsDlg::ProcessCommandL %d ",
    	aCommandId);
    HideMenu();
    iAppUi.UnlockEntry();

    switch (aCommandId)
        {
        case EMsgMailEditorCmdInsertMenu:
        	DoShowSelectionListL();
           	break;
        case EMsgMailEditorCmdInsertImage:
        	// Falltrought
        case EMsgMailEditorCmdInsertAudio:
        	// Falltrought
		case EMsgMailEditorCmdInsertVideo:
			// Falltrought
        case EMsgMailEditorCmdInsertNote:
	        // Falltrought
        case EMsgMailEditorCmdInsertOther:
        	{
            //Fetch selected file from file system.
            iAppUi.DoFetchFileL(aCommandId);
            break;
        	}
        case EMsgViewAttachmentCmdRemove:
            {
            TParsePtrC parser(iAttachmentModel.AttachmentInfoAt(
                iListBox->CurrentItemIndex()).FileName());
            HBufC* string = StringLoader::LoadLC(R_MAIL_REMOVE_ATTACHMENT,
                parser.NameAndExt(),
                iEikonEnv);

            // Using delete query resource to save ROM
            if ( MailUtils::ConfirmationQueryL(
                *string, R_MAIL_EDITOR_DELETE_MESSAGE ) )
		        {
                CMsgViewAttachmentsDialog::ProcessCommandL( aCommandId );
		        }
            CleanupStack::PopAndDestroy( string );
            break;
            }
        case EMsgMailEditorAttachmentsCmdExit:
        	iAppUi.LockEntryL();
        	TryExitL(EAknSoftkeyOk);
            return;

        case EMsgViewAttachmentCmdOpen:
            OpenAttachmentL();
            break;

		case EAknCmdHelp:

            if (iHelpSupported)
                {
                // Launch help
                HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                    iEikonEnv->EikAppUi()->AppHelpContextL() );
                }
			break;

        default:
            // call parent class to process other commands.
            CMsgViewAttachmentsDialog::ProcessCommandL(aCommandId);
            break;
        }
    }

// ----------------------------------------------------------------------------
// CMsgMailViewAttachmentsDlg::GetHelpContext
// This function is called when Help application is launched.
// ----------------------------------------------------------------------------
//
void CMsgMailViewAttachmentsDlg::GetHelpContext(
	TCoeHelpContext& aContext) const
    {
    if (iHelpSupported)
        {
        aContext.iMajor = TUid::Uid( KUidMsgMailEditor );
        aContext.iContext = KMAIL_HLP_ATTACHMENTS_EDITOR();
        }
    }

// ----------------------------------------------------------------------------
// CMsgMailViewAttachmentsDlg::HandleListBoxEventL
// ----------------------------------------------------------------------------
//
void CMsgMailViewAttachmentsDlg::HandleListBoxEventL( 
    CEikListBox* aListBox, 
    TListBoxEvent aEventType )
    {
    LOG1("CMsgMailViewAttachmentsDlg::HandleListBoxEventL %d ", aEventType );
    switch( aEventType )
        {
        case EEventEnterKeyPressed:
            {
            ProcessCommandL( EMsgViewAttachmentCmdOpen );
            break;
            }
        default:
            {
            CMsgViewAttachmentsDialog::HandleListBoxEventL( 
                aListBox, aEventType );
            }
        }
    }

// ----------------------------------------------------------------------------
// CMsgMailViewAttachmentsDlg::OpenAttachmentL
// ----------------------------------------------------------------------------
//
void CMsgMailViewAttachmentsDlg::OpenAttachmentL()
    {
    const TInt index(CurrentListItemIndex());
    if (index == KErrNotFound)
        {
        return; // no attachments
        }

    const TMsvId id(iAttachmentModel.AttachmentInfoAt(index).AttachmentId());
    TMsvId service;
    TMsvEntry entry;
    User::LeaveIfError(
    	iAppUi.Document()->CurrentEntry().Session().GetEntry(
    		id, service, entry) );

    if (entry.iType==KUidMsvMessageEntry)
        {
        iAppUi.Document()->OpenMailMessageEntryL( entry );
        }
    else
        {
        CMsgViewAttachmentsDialog::ProcessCommandL(EMsgViewAttachmentCmdOpen);
        }
    }

void CMsgMailViewAttachmentsDlg::DoShowSelectionListL()
	{
	TInt selection( 0 );
	
	CAknListQueryDialog* dlg = new ( ELeave ) CAknListQueryDialog( &selection );
	
	dlg->PrepareLC( R_MAILEDITOR_ATTACHMENTVIEW_INSERT_SELECTIONLIST );
	
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
#ifdef EMAIL_PRESENTATION_SUPPORT
			case 3:	// CSI: 47 # switch case
				commandId = EMsgMailEditorCmdInsertPresentation;
				break;
			case 4:	// CSI: 47 # switch case
				commandId = EMsgMailEditorCmdInsertOther;
				break;
#else
			case 3:	// CSI: 47 # switch case
				commandId = EMsgMailEditorCmdInsertOther;
				break;				
#endif //EMAIL_PRESENTATION SUPPORT	
			}
		
		iAppUi.LockEntryL();
		// Fetch selected file from file system.
		iAppUi.DoFetchFileL( commandId );
		}
	}

//  End of File
