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
* Description:  This dialog is used for Attachment handling.
*
*/


// INCLUDE FILES
#include "MsgMailViewerAttachmentsDlg.h"
#include "MsgMailViewerAppUi.h"
#include "msgmailviewer.hrh"
#include "MsgMailUIDs.h"    // Application Uid
#include "MsgMailDRMHandler.h"
#include "MailUtils.h"
#include "MailLog.h"

#include <CMailMessage.h>
#include <CMessageData.h>

#include <AiwGenericParam.h>
#include <impicmds.h>
#include <sendui.h>
#include <SenduiMtmUids.h>  // Send UI MTM uid's
#include <StringLoader.h>
#include <eiklbo.h>

#include <MuiuMsvUiServiceUtilities.h> // DiskSpaceBelow
#include <MsgAttachmentModel.h>
#include <MsgAttachmentInfo.h>
#include <MsgEditorAppUi.hrh>   // EditorBase commands
#include <MsgAttachmentModelObserver.h>
#include <MsgEditorAppUi.rsg> // R_MEB_ATTACHMENTS_MENUPANE
#include <MsgMailViewer.rsg>

#include <hlplch.h>
#include <csxhelp/mail.hlp.hrh>
#include <featmgr.h>

// CONSTANTS
_LIT(KMessageMimeType, "message/rfc822");
const TInt KMSKControlID(3);

enum TStatusFlags
	{
    EObserverAdded = KBit0
    };

// ================= MEMBER FUNCTIONS =======================

CMsgMailViewerAttachmentsDlg* CMsgMailViewerAttachmentsDlg::NewL(
	TDesC& aTitle, CMsgAttachmentModel& aAttachmentModel,
    CMsgMailViewerDocument& aDocument,
    MMsgMailAttachmentDialogObserver& aDlgObserver )
	{
    CMsgMailViewerAttachmentsDlg* self =
        new(ELeave) CMsgMailViewerAttachmentsDlg(
        aTitle, aAttachmentModel, aDocument, aDlgObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
    
	CleanupStack::Pop( self );
	return self;
	}
// ----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg
// ----------------------------------------------------------------------------
//
CMsgMailViewerAttachmentsDlg::CMsgMailViewerAttachmentsDlg(
    TDesC& aTitle, CMsgAttachmentModel& aAttachmentModel,
    CMsgMailViewerDocument& aDocument,
    MMsgMailAttachmentDialogObserver& aDlgObserver )
    :CMsgViewAttachmentsDialog(aTitle,
    R_VIEWER_ATTACHMENTS_MENU, aAttachmentModel),
    iDocument(aDocument), iFlags(0), iDlgObserver(aDlgObserver) 
    {
    }

CMsgMailViewerAttachmentsDlg::~CMsgMailViewerAttachmentsDlg()
    {
    LOG("CMsgMailViewerAttachmentsDlg: destructor");
    
    // report to observer
    iDlgObserver.AttachmentDialogExit();
    
    if ((iFlags & EObserverAdded))
    	{
    	iDocument.Session().RemoveObserver(*this);
    	}
    }

// -----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::NotifySizeChanged
// -----------------------------------------------------------------------------
//
void CMsgMailViewerAttachmentsDlg::NotifySizeChanged()
    {
    if(iListBox)
        {
        iListBox->SizeChanged();
        }
    else
        {
        LOG("CMsgMailViewerAttachmentsDlg::NotifySizeChanged: iListBox NULL");
        }

    }

// -----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::HandleListBoxEventL
// -----------------------------------------------------------------------------
//
void CMsgMailViewerAttachmentsDlg::HandleListBoxEventL(
    CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    LOG("CMsgMailViewerAttachmentsDlg::HandleListBoxEventL");
    if ( !MenuShowing() )
        {
		if ( aEventType == EEventEnterKeyPressed ||
		     aEventType == EEventItemDoubleClicked )
		    {
		    const TInt index(CurrentListItemIndex());
		    if ( index != KErrNotFound )
		        {
		        UpdateMiddleSoftKeyL( index );
		        DoFetchOrOpenL( index );
		        }
		    }
		else if ( aEventType == EEventItemClicked )
			{
			const TInt index(CurrentListItemIndex());
			if ( index != KErrNotFound )
				{
				UpdateMiddleSoftKeyL( index );
				}
		    }
		}
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CMsgMailViewerAttachmentsDlg::ConstructL()
    {
    CMsgViewAttachmentsDialog::ConstructL(iDocument.CurrentEntry());

    iDocument.Session().AddObserverL(*this);
    iFlags |= EObserverAdded;
    }

// ----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::DynInitMenuPaneL
// ----------------------------------------------------------------------------
//
void CMsgMailViewerAttachmentsDlg::DynInitMenuPaneL(
    TInt aMenuId, CEikMenuPane* aMenuPane)
    {
    CMsgViewAttachmentsDialog::DynInitMenuPaneL( aMenuId, aMenuPane );
    const TInt index(CurrentListItemIndex());

    if (aMenuId == R_MEB_ATTACHMENTS_MENUPANE)
        {
        // this shouldn't happen...
        if (index == KErrNotFound)
            {
            return;
            }

        CMsgAttachmentInfo& attaInfo =
            iAttachmentModel.AttachmentInfoAt(index);

        if (attaInfo.IsFetched())
            {
            aMenuPane->SetItemDimmed(EMsgViewAttachmentCmdOpen, EFalse);
            aMenuPane->SetItemDimmed(
                EMsgViewAttachmentCmdRemove,
                !CanRemoveAttachment( attaInfo ) );

            //Saving changed so that all files supported or unsupported
            //can be saved.
            //CDocumentHandler can still prevent saving,
            //but shouldn't do so.
            aMenuPane->SetItemDimmed(
            	EMsgViewAttachmentCmdSave,
            	! (iDocument.CanSaveFileL( attaInfo.DataType() )) );

            // attachment is not banned type
            if (!MailUtils::IsClosedMimeL(attaInfo.DataType().Des()))
                {
                // not a message type
                if (!attaInfo.DataType().Des().Compare(KMessageMimeType) == 0)
                    {
                    // -> display send via menu
                    TInt index = 0;
                    aMenuPane->ItemAndPos(EMsgViewAttachmentCmdOpen, index);

                    TSendingCapabilities capabilities(
                        0, 0, TSendingCapabilities::ESupportsAttachments);
                    iDocument.SendUiL().AddSendMenuItemL(
                    	*aMenuPane,
                    	index+3, 								// CSI: 47 # third command below attachments.
                    	EMsgMailViewerFirstFreeCommandId,
                    	capabilities );
                    }
                }

            }
        else
            {
            aMenuPane->SetItemDimmed(EMsgViewAttachmentCmdFetch, EFalse);
            }
        if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ))
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
            }
        }
    }

// ----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::FetchRemoteAttachmentL
// ----------------------------------------------------------------------------
//
void CMsgMailViewerAttachmentsDlg::FetchRemoteAttachmentL( TInt aIndex )
    {
    CMsvEntry& currEntry = iDocument.CurrentEntry();
    CMsgAttachmentInfo& modelInfo = iAttachmentModel.AttachmentInfoAt(aIndex);
	if ( MsvUiServiceUtilities::DiskSpaceBelowCriticalLevelL(
	    currEntry.Session(),
	    modelInfo.Size()) )
        {
        User::Leave( KErrDiskFull );
        }

	TInt status( MailUtils::EQueryAttachmentFetch );
	if ( !iDocument.IsOnLineL() )
		{
		status |= MailUtils::EConnectionOffline;
		}
	if ( MailUtils::LargeAttachmentQueryL(
			iAttachmentModel.AttachmentInfoAt(aIndex).Size(),
			status ) )
		{
	 	iDocument.FetchAttachmentL( modelInfo.AttachmentId() );
        // Now message is fetched?, we'll update datatype information in
        // the HandleSessionEventL
		}
     }

// ----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::UpdateModelInfoL
// ----------------------------------------------------------------------------
//
void CMsgMailViewerAttachmentsDlg::UpdateModelInfoL( TInt aIndex )
    {
    CMsgAttachmentInfo& modelInfo =
        iAttachmentModel.AttachmentInfoAt( aIndex );
    if ( modelInfo.IsFetched() )
        {
	    RFile attachmentHandle = iAttachmentModel.GetAttachmentFileL(
		    aIndex );
	    CleanupClosePushL( attachmentHandle );

		TDataType dataType = iDocument.ResolveMimeType( attachmentHandle );
	    
	    if(dataType.Des().Length())
	        {
	        //Successful recognition.
	        modelInfo.SetDataType(dataType);    
	        }
	    
	    MsgMailDRMHandler* drmHandler = MsgMailDRMHandler::NewL();
	    CleanupStack::PushL( drmHandler );
	    CMsgAttachmentInfo::TDRMDataType  drmType =
	        drmHandler->GetDRMDataTypeL( attachmentHandle );
        modelInfo.SetDRMDataType( drmType );
		CleanupStack::PopAndDestroy(2);							// CSI: 47 # drmHandler, handle
        }
    }


// ----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::UpdateAttachmentsFetchFlags
// ----------------------------------------------------------------------------
//
void CMsgMailViewerAttachmentsDlg::UpdateAttachmentsFetchFlags()
    {
    //Update attachmentmodel to secure model is up to date.
    for(TInt itemIndex=0; itemIndex<iAttachmentModel.NumberOfItems(); itemIndex++)
        {
        CMsgAttachmentInfo& attInfo =
        iAttachmentModel.AttachmentInfoAt(itemIndex);

        TMsvEntry entry;
        TMsvId serviceId;

        if ( iDocument.CurrentEntry().Session().GetEntry(
        attInfo.AttachmentId(), serviceId, entry ) == KErrNone )
            {
            //Update is not done for message entries.
            //This is done because for some reason we
            //can't get file handle for this type of attachment.
            //Also entry.Complete() flag doesn't work with them.
            //This shouldn't be a problem because attached messages (= KUidMsvMessageEntry type)
            //are fetched with original mail and updating of model info 
            //isn't necessary.
            if(entry.iType != KUidMsvMessageEntry)
                {
                LOG( "CMsgMailViewerAttachmentsDlg::EMsvEntriesChanged: FOUND" );
                attInfo.SetFetched( entry.Complete() );
                //Old UpdateModelInfoL call was removed because it caused
                //Panic when exiting dialog too quicly.
                //Reason for Panic was MIME resolving which takes lot of time
                //and can return after dialog object is destroyed.
                }
            else
                {
                LOG("CMsgMailViewerAttachmentsDlg::UpdateAttachmentModelL: KUidMsvMessageEntry");
                }
            }
        }
        
    //If there are attachments update the middle soft key
    //according to the first attachment
    if( iAttachmentModel.NumberOfItems() > 0 )
	    {
	    TRAP_IGNORE( UpdateMiddleSoftKeyL(CurrentListItemIndex()) );	
	    }
    }

// ----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::OfferKeyEventL
// ----------------------------------------------------------------------------
//
TKeyResponse CMsgMailViewerAttachmentsDlg::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    // Consume send keys, so that they are not handled in the appui
    if( aType == EEventKey && aKeyEvent.iCode == EKeyYes )
        {
        return EKeyWasConsumed;
        }  
    
    TInt index( CurrentListItemIndex() );
     if ( index != KErrNotFound && !MenuShowing() )
        {
        if ( aType == EEventKey )
            {
            if( aKeyEvent.iCode == EKeyOK )
                {
                DoFetchOrOpenL( index );
                return EKeyWasConsumed;
                }
            else if( aKeyEvent.iCode == EKeyBackspace )
                {
                CMsgAttachmentInfo& attaInfo =
                    iAttachmentModel.AttachmentInfoAt(index);
                if ( CanRemoveAttachment( attaInfo ) )
                    {
                    ProcessCommandL(EMsgViewAttachmentCmdRemove);
                    }
                return EKeyWasConsumed;
                }
            }
        else if ( aType == EEventKeyUp )
            {
            UpdateMiddleSoftKeyL( index );
            }
        }

    return CMsgViewAttachmentsDialog::OfferKeyEventL(aKeyEvent, aType);
    }


// ----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::ProcessCommandL
// ----------------------------------------------------------------------------
//
void CMsgMailViewerAttachmentsDlg::ProcessCommandL(TInt aCommandId)
    {
    MAIL_ALLOC_TEST( DoProcessCommandL( aCommandId ) );
    }


// ----------------------------------------------------------------------------
// CMsgMailViewAttachmentsDlg::GetHelpContext
// This function is called when Help application is launched.
// ----------------------------------------------------------------------------
//
void CMsgMailViewerAttachmentsDlg::GetHelpContext(
    TCoeHelpContext& aContext) const
    {
    // Uses editor's UID because there's only one help file!
    aContext.iMajor = TUid::Uid( KUidMsgMailEditor );
    aContext.iContext = KMAIL_HLP_ATTACHMENTS_VIEWER();
    }

// ----------------------------------------------------------------------------
// CMsgMailViewAttachmentsDlg::OpenAttachmentL
// ----------------------------------------------------------------------------
//
void CMsgMailViewerAttachmentsDlg::OpenAttachmentL()
    {
    const TInt index(CurrentListItemIndex());
    const CMsgAttachmentInfo& info = iAttachmentModel.AttachmentInfoAt(index);
    const TMsvId id(info.AttachmentId());

    if ( !iDocument.OpenMessageEntryL( id ) )
        {
        if ( info.IsSaved() )
            {
            SetOpenParamList( NULL );
            }
        else if( !OpenParamList() )
             {
             TAiwVariant allowSaveVariant( ETrue );
             TAiwGenericParam genericParamAllowSave( EGenericParamAllowSave,
                allowSaveVariant );
             
             CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC();
             paramList->AppendL( genericParamAllowSave );
             SetOpenParamList( paramList );
             CleanupStack::Pop( paramList );
             }
        CMsgViewAttachmentsDialog::ProcessCommandL(EMsgViewAttachmentCmdOpen);
        }
    }

// ----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::DoFetchOrOpenL
// ----------------------------------------------------------------------------
void CMsgMailViewerAttachmentsDlg::DoFetchOrOpenL( TInt aIndex )
    {
    if ( !iAttachmentModel.AttachmentInfoAt(aIndex).IsFetched() )
        {
        FetchRemoteAttachmentL( aIndex );
        }
    else
        {
        OpenAttachmentL();
        }
    }

// ----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::DoSendViaL
// ----------------------------------------------------------------------------
void CMsgMailViewerAttachmentsDlg::DoProcessCommandL( TInt aCommandId )
    {
    HideMenu();
    const TInt index(CurrentListItemIndex());

    switch (aCommandId)
        {
        // handle our own menu command.
        case EMsgViewAttachmentCmdFetch:
            FetchRemoteAttachmentL(index);
            break;
        case EMsgViewAttachmentCmdRemove:
  	        PurgeAttachmentL(index);
            break;
        case EMsgMailViewerCmdAttachmentCmdExit:
            TryExitL(EAknSoftkeyOk);
            break;
        case EMsgViewAttachmentCmdOpen:
            OpenAttachmentL();
            break;
        case EAknCmdHelp:
            // Launch help
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                iEikonEnv->EikAppUi()->AppHelpContextL() );
			break;
        case EMsgMailViewerFirstFreeCommandId:
        	DoSendViaL();
        	break;
        default:
            // call parent class to process other commands.
            CMsgViewAttachmentsDialog::ProcessCommandL(aCommandId);
            break;
        }
    }

// ----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::DoSendViaL
// ----------------------------------------------------------------------------
void CMsgMailViewerAttachmentsDlg::DoSendViaL()
    {
	CMessageData* messageData = CMessageData::NewL();
	CleanupStack::PushL(messageData);

	RFile attachentHandle = iAttachmentModel.GetAttachmentFileL(
		CurrentListItemIndex() );
	CleanupClosePushL( attachentHandle );
	messageData->AppendAttachmentHandleL( attachentHandle );
    CleanupStack::PopAndDestroy(); // attachentHandle

    TSendingCapabilities capabilities(
    	0, 0, TSendingCapabilities::ESupportsAttachments);

	// Three services to dim
    CArrayFixFlat<TUid>* servicesToDim = new(ELeave) CArrayFixFlat<TUid>( 3 );		// CSI: 47 # See a comment above.
    CleanupStack::PushL( servicesToDim );

    servicesToDim->AppendL( KSenduiMtmMmsUid );
    servicesToDim->AppendL( KSenduiMtmSmtpUid );

    iDocument.SendUiL().ShowQueryAndSendL( messageData, capabilities, servicesToDim );

    CleanupStack::PopAndDestroy(2);							// CSI: 47 # servicesToDim, messageData
    }

// ----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::PurgeAttachmentL
// ----------------------------------------------------------------------------
void CMsgMailViewerAttachmentsDlg::PurgeAttachmentL( TInt aIndex )
    {
   CMsgAttachmentInfo& attInfo =
        iAttachmentModel.AttachmentInfoAt(aIndex);
    TParsePtrC parser(attInfo.FileName());
    HBufC* string = StringLoader::LoadLC(R_MAIL_VIEWER_REMOVE_ATTACHMENT,
        parser.NameAndExt(), iEikonEnv);

    if ( MailUtils::ConfirmationQueryL(*string) )
        {
		iDocument.RemoveAttachmentL( attInfo.AttachmentId() );
        attInfo.SetFetched(EFalse);
        UpdateIcon(aIndex);
        }
    CleanupStack::PopAndDestroy(); // string
    }

// ----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::CanRemoveAttachment
// ----------------------------------------------------------------------------
//
TBool CMsgMailViewerAttachmentsDlg::CanRemoveAttachment(
    CMsgAttachmentInfo& aInfo )
    {
    // POP3 entry or IMAP local entry does not support purge attachment
    const CMsvEntry& messageEntry = iDocument.CurrentEntry();

    return ( messageEntry.Entry().iMtm == KSenduiMtmImap4Uid &&
        messageEntry.OwningService() != KMsvLocalServiceIndexEntryIdValue &&
        aInfo.IsAttachedThisSession() && aInfo.IsFetched() );
    }

// ---------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::SetMiddleSoftKeyLabelL
// ---------------------------------------------------------------------------
//
void CMsgMailViewerAttachmentsDlg::SetMiddleSoftKeyLabelL(
    TInt aResourceId, TInt aCommandId )
    {
    HBufC* middleSKText = StringLoader::LoadLC( aResourceId );

    ButtonGroupContainer().AddCommandToStackL(
        KMSKControlID,
        aCommandId,
        *middleSKText );
    CleanupStack::PopAndDestroy( middleSKText );
    }

// ---------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::UpdateMiddleSoftKeyL
// ---------------------------------------------------------------------------
//
void CMsgMailViewerAttachmentsDlg::UpdateMiddleSoftKeyL( TInt aIndex )
    {
    ASSERT( aIndex >= 0 && aIndex < iAttachmentModel.NumberOfItems() );
    CMsgAttachmentInfo& attInfo =
        iAttachmentModel.AttachmentInfoAt( aIndex );

    if ( attInfo.IsFetched() )
        {
        // switch command from "fetch" to "open"
        ButtonGroupContainer().RemoveCommandFromStack(
            KMSKControlID, EMsgViewAttachmentCmdFetch );
        SetMiddleSoftKeyLabelL(
            R_QTN_MSK_OPEN, EAknSoftkeyOpen );
        }
    else
        {
        // switch command from "open" to "fetch"
        ButtonGroupContainer().RemoveCommandFromStack(
            KMSKControlID, EAknSoftkeyOpen );
        SetMiddleSoftKeyLabelL(
            R_QTN_MSK_FETCH, EMsgViewAttachmentCmdFetch );
        }
    }

// -----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::HandleSessionEventL
// -----------------------------------------------------------------------------
//
void CMsgMailViewerAttachmentsDlg::HandleSessionEventL(TMsvSessionEvent aEvent,
            TAny* aArg1, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    if ( aEvent == EMsvEntriesChanged && aArg1 )
        {
        CMsvEntrySelection* changedEntries =
            static_cast<CMsvEntrySelection*>( aArg1 );
        UpdateChangedAttachmentsL( *changedEntries );
        }
    }

// -----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::UpdateChangedAttachmentsL
// Update model and UI e.g. due to attachment fetching. We cannot update
// attached messages but that is not a problem since they are automatically
// fetched along with the parent message.
// -----------------------------------------------------------------------------
//
void CMsgMailViewerAttachmentsDlg::UpdateChangedAttachmentsL(
    const CMsvEntrySelection& aChangedEntries )
    {
    TInt changedCount( aChangedEntries.Count() );
    for ( TInt changedIndex( 0 ); changedIndex < changedCount; ++changedIndex )
        {
        TMsvId changedId( aChangedEntries[changedIndex] );
        TInt attachmentCount( iAttachmentModel.NumberOfItems() );
        for ( TInt attaIndex( 0 ); attaIndex < attachmentCount; ++attaIndex )
            {
            CMsgAttachmentInfo& attaInfo =
                iAttachmentModel.AttachmentInfoAt( attaIndex );
            if ( attaInfo.AttachmentId() == changedId )
                {
                TMsvEntry entry;
                TMsvId serviceId;
                if ( iDocument.CurrentEntry().Session().GetEntry(
                    changedId, serviceId, entry ) == KErrNone &&
                    entry.iType != KUidMsvMessageEntry )
                    {
                    attaInfo.SetFetched( entry.Complete() );
                    UpdateModelInfoL( attaIndex );
                    UpdateMiddleSoftKeyL( attaIndex );
                    UpdateIcon( attaIndex );
                    }
                break; // continue with next changed entry
                }
            }
        }    
    }

 // -----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CMsgMailViewerAttachmentsDlg::OkToExitL(TInt aButtonId)
    {
    //Check if user has selected exit from menu
    // -> whole application shall be closed.
    if ( aButtonId == EAknSoftkeyOk )
        {
        if ( CMsgViewAttachmentsDialog::OkToExitL(aButtonId) )
           {
           CEikAppUi* appUi = iEikonEnv->EikAppUi();
           static_cast<MEikCommandObserver*>(appUi)->ProcessCommandL(
            EAknCmdExit );
           return ETrue;
           }
        else
           {
           return EFalse;
           }
        }

     else if ( aButtonId == EMsgViewAttachmentCmdFetch )
        {
        DoProcessCommandL( aButtonId );
        return EFalse;
        }

    //Let base class decide if it ok to exit
    return CMsgViewAttachmentsDialog::OkToExitL(aButtonId);
    }


// -----------------------------------------------------------------------------
// CMsgMailViewerAttachmentsDlg::UpdateAttatchmentListL
// Update attatchmentlist
// -----------------------------------------------------------------------------
//
void CMsgMailViewerAttachmentsDlg::UpdateAttatchmentListL()
	{
	// Handles item addition
	ItemAddedL();
	}

//  End of File



