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
* Description:  Implements document for application.
*
*/


// INCLUDE FILES
#include "MailLog.h"
#include "MailUtils.h"
#include "MsgMailViewerDocument.h"
#include "MsgMailViewerAppUi.h"
#include <MsgMailViewer.rsg>
#include "MsgMailDRMHandler.h"
#include "CMailCRHandler.h"
#include "CMailForwardOperation.h"
#include "MsgMailViewerRemoveAttachment.h"
#include <CMailMessage.h>
#include <Muiumsginfo.hrh>
#include <MuiuMsgEditorLauncher.h>      // for TEditorParameters
#include <miutmsg.h>
#include <mtmuibas.h>
#include <mmsvattachmentmanager.h>
#include <MsgAttachmentModel.h>
#include <MsgAttachmentInfo.h>
#include <DocumentHandler.h>
#include <MuiuOperationWait.h>          // CMuiuOperationWait
#include <impicmds.h>                   // TIMAP4Cmds
#include <sendui.h>
#include <mtmuidef.hrh>

// LOCAL CONSTANTS
// Max param length for msg info.
const TInt KMessageSizeMaxLength(10);

// ================= MEMBER FUNCTIONS =======================

// Default constructor
CMsgMailViewerDocument::CMsgMailViewerDocument(CEikApplication& aApp)
    : CMsgMailDocument(aApp), iCharSet(0)
    { 
    }

// destructor
CMsgMailViewerDocument::~CMsgMailViewerDocument()
    {
    delete iViewedMessage;    
    delete iPrefs;
    iPrefs = NULL; // Owned by a superclass
    delete iDRMHandler;
    delete iSendUi;
    delete iMailCRHandler;        
    }

// Symbian OS default constructor can leave.
void CMsgMailViewerDocument::ConstructL()
    {
    CMsgEditorDocument::ConstructL();
    iMailCRHandler = CMailCRHandler::NewL( this );
    }

// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::DoInitDRMHandlerL()
// ----------------------------------------------------------------------------
//
void CMsgMailViewerDocument::DoInitDRMHandlerL()
	{
	if (!iDRMHandler)
		{
        LOG("CMsgMailViewerAppUi::DoInitDRMHandlerL initializing DRM");
		iDRMHandler = MsgMailDRMHandler::NewL();
		}
	}	
// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMsgMailViewerDocument* CMsgMailViewerDocument::NewL(
        CEikApplication& aApp)     // CMsgMailViewerApp reference
    {
    CMsgMailViewerDocument* self = new (ELeave) CMsgMailViewerDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }    
// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::CreateAppUiL()
// constructs CMsgMailViewerAppUi
// ----------------------------------------------------------------------------
//
CEikAppUi* CMsgMailViewerDocument::CreateAppUiL()
    {
    return new (ELeave) CMsgMailViewerAppUi;
    }

// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::NotifyChanges()
// ----------------------------------------------------------------------------
//
void CMsgMailViewerDocument::NotifyChanges(TMsgAttachmentCommand aCommand, 
    CMsgAttachmentInfo* /*aAttachmentInfo*/ )
    {
    switch (aCommand)
        {
        case EMsgAttachmentAdded:
            // FALLTHROUGH
        case EMsgAttachmentRemoved:
            SetChanged(ETrue);
            break;
        default:
            break;
        }   
    }
// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::GetAttachmentFileL()
// ----------------------------------------------------------------------------
//
RFile CMsgMailViewerDocument::GetAttachmentFileL( TMsvAttachmentId aId )
	{
    LOG1("CMsgMailViewerDocument::GetAttachmentFileL %08x", aId);	
    return MailMessage().GetFileHandleL( aId );
	}

// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::EntryChangedL()
// ----------------------------------------------------------------------------
//
void CMsgMailViewerDocument::EntryChangedL()
	{
    delete iViewedMessage;
    iViewedMessage = NULL;
    iViewedMessage = CMailMessage::NewL(CurrentEntry());
    iViewedMessage->SetNew( (LaunchFlags() & EMsgUnreadMessage) );  
    LOG2( "CMsgMailViewerDocument::EntryChangedL %08x is new:%d" , 
    	MailMessage().MessageEntry().Id(), iViewedMessage->IsNew() );
	}
	
// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::AddAttachmentL()
// ----------------------------------------------------------------------------
//
void CMsgMailViewerDocument::AddAttachmentL( 
	CMsvAttachment& aAttachmentInfo,
	TBool aCanBeRemoved )
	{
    // add attachment to model
	CMsgAttachmentModel& attmodel = AttachmentModel();
	TDataType dataType(aAttachmentInfo.MimeType());     

    if ( aAttachmentInfo.Type() != CMsvAttachment::EMsvMessageEntry ) 
    	{	    		    
	    RFile attachment = GetAttachmentFileL( aAttachmentInfo.Id() );
	    CleanupClosePushL(attachment);	
	    
	    dataType = ResolveMimeType( attachment );	   
	    if ( !dataType.Des().Length() && aAttachmentInfo.MimeType().Length() )
	    	{
	    	dataType = aAttachmentInfo.MimeType();
	    	}
	    
	    CMsgAttachmentInfo* info = attmodel.AddAttachmentL(
	        aAttachmentInfo.AttachmentName(),
	        aAttachmentInfo.Size(),
	        aAttachmentInfo.Id(),
	        aAttachmentInfo.Complete(),
	        dataType); 	    

    	TMsvId serviceId;
    	TMsvEmailEntry attachmentEntry;
    	User::LeaveIfError(Session().GetEntry(
        aAttachmentInfo.Id(), serviceId, attachmentEntry));
    	info->SetAttachedThisSession( aCanBeRemoved );
    	LOG1("CMsgMailViewerAppUi::AddAttachmentL attachment complete:%d",
    		attachmentEntry.Complete());
    	LOG1("CMsgMailViewerAppUi::AddAttachmentL attachment BodyComplete:%d",
    		attachmentEntry.BodyTextComplete());
	    
	    DoInitDRMHandlerL();
	    iDRMHandler->SetDRMDataTypeL( *info, attachment );
    	CleanupStack::PopAndDestroy(); // attachment
    	}
    else  // EMsvMessageEntry 
    	{
    	// completed flag is not set correctly to message entry info
	    CMsgAttachmentInfo* info = attmodel.AddAttachmentL(
	        aAttachmentInfo.AttachmentName(),
    	    aAttachmentInfo.Size(),
        	aAttachmentInfo.Id(),
        	ETrue /*aAttachmentInfo.Complete()*/,
        	dataType);
        info->SetAttachedThisSession( aCanBeRemoved );	 
    	}  	
	
	}
    	

// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::CanSaveFileL()
// ----------------------------------------------------------------------------
//
TBool CMsgMailViewerDocument::CanSaveFileL( TDataType aDataType )
	{
	// remove save if system doesn't support saving
	CDocumentHandler* doch = CDocumentHandler::NewLC( 
		CEikonEnv::Static()->Process() );					// CSI: 27 # Must be used because of 
															// iEikEnv is not accessible.
     
    TBool canSave(EFalse);
    TRAP_IGNORE( canSave = doch->CanSaveL( aDataType ) );
    // if error hide save
    CleanupStack::PopAndDestroy(); //doch	
   	return canSave;
	}
	

// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::DeleteMessageL()
// ----------------------------------------------------------------------------
//
TBool CMsgMailViewerDocument::DeleteMessageL()
	{
    CMsvEntrySelection* sel = new (ELeave) CMsvEntrySelection;
    CleanupStack::PushL(sel);
    TMsvId msgID = MailMessage().MessageEntry().Id();
    sel->AppendL( msgID );
    Mtm().SwitchCurrentEntryL( MailMessage().MessageEntry().Parent() );
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
    CMsvOperation* op = MtmUi().DeleteFromL( *sel, wait->iStatus );
    CleanupStack::PushL(op);
    
    //Check if delete is cancelled from IMUM menu
    TBool cancel(op->iStatus.Int() == KErrCancel);
   
    //When deleting is really started we proceed
    //as it was done complitely (= close viewer).
    //Reason for this is that we don't
    //know if IMUM has deleted message completely
    //or not.
        
    wait->Start();
	
    CleanupStack::PopAndDestroy(3);			// CSI: 47 # op, wait, sel

	return cancel;        	
	}
	
// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::FetchAttachmentL()
// ----------------------------------------------------------------------------
//
CMsvOperation* CMsgMailViewerDocument::FetchAttachmentL( 
    CMsvEntrySelection& aSelection, TRequestStatus& aCompletionStatus )
	{
	TBuf8<1> param;
	// Set context back to message if not allready set
	SetEntryWithoutNotificationL( MailMessage().MessageEntry().Id() ); 
	return MtmUi().InvokeAsyncFunctionL(
		KImpiMTMFetchAttachments, aSelection, aCompletionStatus, param );
	}

// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::FetchAttachmentL()
// ----------------------------------------------------------------------------
//
void CMsgMailViewerDocument::FetchAttachmentL( CMsvEntrySelection& aSelection )
	{
	CMsvSingleOpWatcher* watch=CMsvSingleOpWatcher::NewLC( *this );
	CMsvOperation* op = FetchAttachmentL( aSelection, watch->iStatus );
    // watch owns op before leave can occur
    AddSingleOperationL(op, watch);
    CleanupStack::Pop( watch ); // watch    
	}
// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::FetchAttachmentL()
// ----------------------------------------------------------------------------
//
void CMsgMailViewerDocument::FetchAttachmentL( TMsvAttachmentId aAttachmentId )
	{
    CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
	CleanupStack::PushL(sel);
	sel->AppendL( aAttachmentId );
	FetchAttachmentL( *sel );
	CleanupStack::PopAndDestroy( sel );
	}	

// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::RemoveAttachmentL()
// ----------------------------------------------------------------------------
//		
void CMsgMailViewerDocument::RemoveAttachmentL( 
	TMsvAttachmentId aAttachmentId )
	{
    CMsvSingleOpWatcher* watch=CMsvSingleOpWatcher::NewLC( *this );
    LOG1("CMsgMailViewerDocument::RemoveAttachmentL %08x", 
        	aAttachmentId);       	
    TMsvId msgID = MailMessage().MessageEntry().Id();
    
    CMsvOperation* op = CMsgMailViewerRemoveAttachmentOp::NewLC(
    	Session(), watch->iStatus, aAttachmentId, msgID );

    CleanupStack::Pop(); // op
    // watch owns op before leave can occur
    AddSingleOperationL(op, watch);
    CleanupStack::Pop( watch ); // watch	
	}
	
// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::IsOnLineL()
// ----------------------------------------------------------------------------
//
TBool CMsgMailViewerDocument::IsOnLineL() const
    {
    const TMsvId entryId(CurrentEntry().OwningService());
    CMsvEntry *entry = CMsvEntry::NewL(Session(),
        entryId, TMsvSelectionOrdering());
    const TBool result(entry->Entry().Connected());
    delete entry;
    return result;
    }


// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::RetrieveMessageL()
// ----------------------------------------------------------------------------
//
void CMsgMailViewerDocument::RetrieveMessageL()
	{
    CMsvEntrySelection* sel= new (ELeave) CMsvEntrySelection();
    CleanupStack::PushL( sel );
    TMsvId messageId = Entry().Id();
    sel->AppendL( messageId );

    SetEntryL(Entry().Parent());

    TBuf8<1> blankParams;
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();

    CMsvOperation* op=MtmUi().InvokeAsyncFunctionL(
        KMtmUiFunctionFetchSelected,
        *sel,
        wait->iStatus,
        blankParams);

    CleanupStack::PushL(op);
    wait->Start();

    // Set content back to message itself
    SetEntryL(messageId);
    
    // ignore return value.
    MtmUi().DisplayProgressSummary( op->FinalProgress() );
    CleanupStack::PopAndDestroy(3);				// CSI: 47 # op, wait, sel	
	}

// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::SendUiL()
// ----------------------------------------------------------------------------
//
CSendUi& CMsgMailViewerDocument::SendUiL()
    {
    if ( !iSendUi )
        iSendUi = CSendUi::NewL();
    
    return *iSendUi;
    }
    
// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::ShowMessageInfoL()
// ----------------------------------------------------------------------------
//
void CMsgMailViewerDocument::ShowMessageInfoL()    
	{
    CMsvEntrySelection* sel = new (ELeave) CMsvEntrySelection;
    CleanupStack::PushL(sel);
    TMsvId id( Entry().Id() );
    sel->AppendL(id);

    TBuf8<KMessageSizeMaxLength> infoParams(KMessageInfoViewer);
    CMsvSingleOpWatcher* watch=CMsvSingleOpWatcher::NewLC(*this);
    CMsvOperation* op= MtmUi().InvokeAsyncFunctionL(
        KMtmUiFunctionMessageInfo,
        *sel,
        watch->iStatus,
        infoParams
        );
    // watch owns op before leave can occur
    AddSingleOperationL(op, watch);
    CleanupStack::Pop(); // watch
    CleanupStack::PopAndDestroy(); // sel	
	}
	
// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::OpenMessageEntryL()
// ----------------------------------------------------------------------------
//
TBool CMsgMailViewerDocument::OpenMessageEntryL(TMsvId aEntryId )
	{
   	TMsvEntry entry;
   	TMsvId service;
   	User::LeaveIfError( Session().GetEntry(aEntryId, service, entry) );
     
    if (entry.iType==KUidMsvMessageEntry)
        {
        // dealing with attached message 
        const TMsvId prevEntry = CurrentEntry().EntryId();
        SetEntryL( aEntryId );

        // create watcher
        CMsvSingleOpWatcher* watch=CMsvSingleOpWatcher::NewLC(
            *this );
        // open attached message
        CMsvOperation* op = MtmUi().ViewL(watch->iStatus);
        // add to operation queue, watch owns op before leave can occur
        AddSingleOperationL(op, watch);
        CleanupStack::Pop(); // watch
        // set context back to original entry
        SetEntryL(prevEntry); 
        return ETrue;
        }
    return EFalse;
	}

// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::MailMessage()
// ----------------------------------------------------------------------------
//
CMailMessage& CMsgMailViewerDocument::MailMessage()
	{
	__ASSERT_DEBUG( iViewedMessage, User::Invariant() );
	return *iViewedMessage;
	}
	
// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::MoveRemoteEntryL()
// ----------------------------------------------------------------------------
//
void CMsgMailViewerDocument::MoveRemoteEntryL( 
	TMsvId aTo, TMsvId aService, TBool aDestinationIsRemote )
	{
    CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection();
    CleanupStack::PushL(sel);
    sel->AppendL( MailMessage().MessageEntry().Id() );

    CMsvOperation* op=NULL;
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
    
    if ( aDestinationIsRemote )
        {
        Mtm().SwitchCurrentEntryL( aTo );
        op=MtmUi().CopyToL( *sel, wait->iStatus);
        }
    else
        {
        Mtm().SwitchCurrentEntryL( aService );
        op=MtmUi().CopyFromL( *sel, aTo, wait->iStatus);
        }

    CleanupStack::PushL(op);
    wait->Start();  // wait for completion of asynch operation
    LOG1("CMsgMailViewerDocument::MoveRemoteEntryL iStatus:%d",
        wait->iStatus.Int() );
    User::LeaveIfError( wait->iStatus.Int() );    

    CleanupStack::PopAndDestroy(3);			// CSI: 47 # sel, op, wait	
	}
	
// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::UpdateAttachmentModelFromMessageL()
// ----------------------------------------------------------------------------
//
void CMsgMailViewerDocument::UpdateAttachmentModelFromMessageL()
    {
    // Now message is fetched, we'll update datatype information
    //MailMessage().LoadAttachmentsL( *this );    
    MMsvAttachmentManager& manager = MailMessage().AttachmentManager();
    LOG1("CMsgMailViewerDocument::UpdateAttachmentModelFromMessageL count:%d",
        manager.AttachmentCount() );    
    }

// ----------------------------------------------------------------------------
// CMsgMailViewerDocument::ForwardMailMessageL()
// ----------------------------------------------------------------------------
//
CMailOperation* CMsgMailViewerDocument::ForwardMailMessageL( 
    MMsvProgressReporter& aReporter,
    TRequestStatus& aStatus, 
    TBool aSetSuspend )
    {
    return CMailForwardOperation::NewL( 
    	aReporter, *this, aStatus, aSetSuspend );
    }


// End of File  
