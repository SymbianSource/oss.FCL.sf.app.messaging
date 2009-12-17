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
* Description:  Implements document class of Mail Editor
*
*/


// INCLUDE FILES
#include "MsgMailEditorDocument.h"
#include "MsgMailEditorAppUi.h"
#include "MsgMailPreferences.h"
#include "MsgMailDRMHandler.h"
#include "MailUtils.h"
#include "CMailCRHandler.h"

#include <cntdef.h>
#include <mtclreg.h>                 // for CClientMtmRegistry
#include <mtmuibas.h>                // for CBaseMtmUi
#include <miutmsg.h>
#include <akninputblock.h>           // CAknInputBlock
#include <MuiuOperationWait.h> // CMuiuOperationWait
#include <MuiuMsgEditorLauncher.h>
#include <MuiuMsvUiServiceUtilities.h>
#include <mmsvattachmentmanager.h>
#include <coeutils.h>
#include <featmgr.h>
#include <bldvariant.hrh>
#include <MsgEditor.hrh>
#include <MsgAttachmentInfo.h>
#include <MsgAttachmentModel.h>
#include <MsgMailEditor.rsg>
#include <mtmuibas.h>
#include <MTMStore.h>
#include <SendUiConsts.h>

// ================= MEMBER FUNCTIONS =======================

// Constructor
CMsgMailEditorDocument::CMsgMailEditorDocument( CEikApplication& aApp )
    : CMsgMailDocument( aApp )
    { 
    }

// destructor
CMsgMailEditorDocument::~CMsgMailEditorDocument()
    {
    delete iPrefs;
    iPrefs = NULL; // owned by CMsgMailDocument
    delete iMailCRHandler;
    delete iEmailApi;
    iEmailApi = NULL;
    iMailboxArray.Close();    
    }
// ----------------------------------------------------------------------------
// CMsgMailEditorDocument::ConstructL()
// Symbian OS constructor can leave.
// ----------------------------------------------------------------------------
//
void CMsgMailEditorDocument::ConstructL()
    {
    CMsgEditorDocument::ConstructL();
    iMailCRHandler = CMailCRHandler::NewL( this );
    iEmailApi = CreateEmailApiL( &(Session()) );

    iPrefs = new (ELeave) CMsgMailPreferences();
    iPrefs->SetEncodingSupport( iMailCRHandler->MailEncodingSetting() );
    
    PrepareMtmL( KSenduiMtmSmtpUid );
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorDocument::NewL()
// ----------------------------------------------------------------------------
//
CMsgMailEditorDocument* CMsgMailEditorDocument::NewL( 
        CEikApplication& aApp )     // CMsgMailEditorApp reference
    {
    CMsgMailEditorDocument* self = 
        new (ELeave) CMsgMailEditorDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
   
// ----------------------------------------------------------------------------
// CMsgMailEditorDocument::CreateAppUiL()
// ----------------------------------------------------------------------------
//
CEikAppUi* CMsgMailEditorDocument::CreateAppUiL()
    {
    return new(ELeave) CMsgMailEditorAppUi;
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorDocument::NotifyChanges()
// ----------------------------------------------------------------------------
//
void CMsgMailEditorDocument::NotifyChanges(TMsgAttachmentCommand aCommand, 
                                           CMsgAttachmentInfo* aAttachmentInfo)
    { 
    TRAP_IGNORE(NotifyChangesL(aCommand, aAttachmentInfo));
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorDocument::GetAttachmentFileL()
// ----------------------------------------------------------------------------
//
RFile CMsgMailEditorDocument::GetAttachmentFileL( TMsvAttachmentId aId )
	{
	return GetAttachmentManagerL().GetAttachmentFileL( aId );
	}
// ----------------------------------------------------------------------------
// CMsgMailEditorDocument::NotifyChangesL()
// ----------------------------------------------------------------------------
//
void CMsgMailEditorDocument::NotifyChangesL(TMsgAttachmentCommand aCommand, 
                                           CMsgAttachmentInfo* aAttachmentInfo)
    {
    // block key events while handling with attachments.
    CAknInputBlock::NewLC();
    switch (aCommand)
        {
        case EMsgAttachmentAdded:
            {
            // don't show confirmation when restoring message
            if (ShowConfirmation())
                {             
                SetChanged(ETrue);
                }            
            break;
            }
        case EMsgAttachmentRemoved:
            {  
            CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
			
			TMsvAttachmentId attachmentId = aAttachmentInfo->AttachmentId();
			MMsvAttachmentManager& manager = MessageL().AttachmentManager();
			manager.RemoveAttachmentL(attachmentId, wait->iStatus);
            
            wait->Start();
            CleanupStack::PopAndDestroy( wait );
            SetChanged(ETrue);
            break;
            }
        default:

        break;
        }   
    CleanupStack::PopAndDestroy();  // CSI: 12 # CAknInputBlock    
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorDocument::IsNewMessage()
// ----------------------------------------------------------------------------
//    
TBool CMsgMailEditorDocument::IsNewMessage() const
    {
    return (!CurrentEntry().Entry().Visible());
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorDocument::IsReplyMessage()
// ----------------------------------------------------------------------------
//
TBool CMsgMailEditorDocument::IsReplyMessage() const
    {
    return ( (LaunchFlags() & EMsgReplyToMessageSender) ||
        (LaunchFlags() & EMsgReplyToMessageAll));
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorDocument::IsForwardMessage()
// ----------------------------------------------------------------------------
//
TBool CMsgMailEditorDocument::IsForwardMessage() const
	{
	return ( LaunchFlags() & EMsgForwardMessage );
	}

void CMsgMailEditorDocument::ReadSendOptionsL()
    {
    if (!CurrentEntry().Entry().Visible())
        {
        // get default send options from default account
        SendOptions().DefaultAccountSettingsL(
            CurrentEntry().Entry().iServiceId,
            Session());
        }
    else
        {
        // opening old message
        SendOptions().ExportSendOptionsL(Entry());
        }        
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorDocument::IsOnlineL()
// ----------------------------------------------------------------------------
//
TBool CMsgMailEditorDocument::IsOnlineL()
    {
    CMsvEntrySelection* accounts = MsvUiServiceUtilities::GetListOfAccountsL(
        Session() );
    CleanupStack::PushL(accounts);

    CMsvEntry* rootEntry = Session().GetEntryL(KMsvRootIndexEntryId);
    CleanupStack::PushL(rootEntry);

    TBool active = EFalse;
    for (TInt cc=0; cc<accounts->Count(); cc++)
        {
        TMsvEntry tentry;
        TRAPD( err, tentry=rootEntry->ChildDataL(accounts->At(cc)) );

        if ( err==KErrNone && tentry.Connected() )
            {
            active = ETrue;
            break;
            }
        }
    CleanupStack::PopAndDestroy( 2, accounts );				// CSI: 47,12 # rootEntry, accounts
    return active;
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorDocument::OpenMailMessageEntryL()
// ----------------------------------------------------------------------------
//
void CMsgMailEditorDocument::OpenMailMessageEntryL( TMsvEntry aEntry )
    {
    CMtmStore* store = CMtmStore::NewL( Session() );
    CleanupStack::PushL(store);
    CBaseMtmUi& mtmUi=store->GetMtmUiAndSetContextLC( aEntry );
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
    CMsvOperation* op=mtmUi.EditL( wait->iStatus );
    wait->Start();
    CleanupStack::PopAndDestroy( 3, store );				// CSI: 47,12 # store, ContextLC, wait
    delete op;    
    }

// ----------------------------------------------------------------------------
// CMsgMailEditorDocument::SendImmediatelyL()
// ----------------------------------------------------------------------------
//
void CMsgMailEditorDocument::SendImmediatelyL( const TMsvId aMsgId )
    {
    CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
    CleanupStack::PushL(selection);
    selection->AppendL( aMsgId );

    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();

    // Switch MTM context to the service to send to.
    const TMsvEntry entry( Entry() );
    Mtm().SwitchCurrentEntryL( entry.iServiceId );

    CBaseMtmUi& ui= MtmUi();
    CMsvOperation* op=ui.CopyToL(*selection, wait->iStatus);

    CleanupStack::PushL( op );
    wait->Start(); // wait for asynch operation

    const TDesC8& finalBuf = op->ProgressL();

    ui.DisplayProgressSummary(finalBuf);

    CleanupStack::PopAndDestroy(3, selection);				// CSI: 47,12 # op, wait, selection
    }
    
// ----------------------------------------------------------------------------
// CMsgMailEditorDocument::GetHealthyMailboxListL()
// ----------------------------------------------------------------------------
//  
MImumInHealthServices::RMailboxIdArray 
        CMsgMailEditorDocument::GetHealthyMailboxListL(
        const TInt64& aFlags )
    {    
    // The list of mailboxes must be updated. 
    User::LeaveIfError( 
        iEmailApi->HealthServicesL().GetMailboxList( 
            iMailboxArray, aFlags ) );
    
    return iMailboxArray;
    }
        
// ----------------------------------------------------------------------------
// CMsgMailEditorDocument::GetAttachmentManagerL()
// ----------------------------------------------------------------------------
//
MMsvAttachmentManager& CMsgMailEditorDocument::GetAttachmentManagerL()
    {
	// Load Attachment list
	MessageL().GetAttachmentsListL(iMessageID, 
		CImEmailMessage::EAllAttachments, 
		CImEmailMessage::EThisMessageAndEmbeddedMessages);
	
	return MessageL().AttachmentManager();       
    }
    
// End of File  
