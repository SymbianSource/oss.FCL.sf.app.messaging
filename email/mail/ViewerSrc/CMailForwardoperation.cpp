/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Mail forward operation
*
*/

// INCLUDE FILES
#include    "CMailForwardOperation.h"
#include    "MsgMailViewerDocument.h"
#include    "MailLog.h"
#include    "MailUtils.h"
#include    "MsgMailDRMHandler.h"
#include    "cmailwaitoperation.h"
#include    <CMailMessage.h>
#include    <MsgMailViewer.rsg>
#include    <e32base.h>
#include    <mtmuibas.h>
#include    <mtmuidef.hrh>
#include    <mtmdef.h>
#include    <impicmds.h>
#include    <imapcmds.h>
#include    <MsgAttachmentInfo.h>
#include    <mmsvattachmentmanager.h>
#include    <MsvPrgReporter.h>
#include    <MuiuMsvSingleOpWatcher.h>

// CONSTANTS
enum TOperationState
    {
    ESuspendOperation = 1,
    ELoadAttachments,
    EGetAttachments,
    EConnectToService,
    EUpdateEntryFlags,
    EForwardMessage
    };
    
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMailForwardOperation::CMailForwardOperation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMailForwardOperation::CMailForwardOperation(
    CMsgMailViewerDocument& aDocument,
    TRequestStatus& aObserverRequestStatus ): 
    CMailOperation( aDocument.Session(), 
    CActive::EPriorityStandard, 
    aObserverRequestStatus ),
    iDocument( aDocument ),
    iOperationState( KErrArgument )
    {
    CActiveScheduler::Add( this );   
    }

// -----------------------------------------------------------------------------
// CMailForwardOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
inline void CMailForwardOperation::ConstructL()
    {
    iSelection = new(ELeave) CMsvEntrySelection;
    iMtm = iDocument.Mtm().Type();
    // Issue first step
    NexStateL();    
    }

// -----------------------------------------------------------------------------
// CMailForwardOperation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMailForwardOperation* CMailForwardOperation::NewL(
    MMsvProgressReporter& aReporter,
    CMsgMailViewerDocument& aDocument,
    TRequestStatus& aObserverRequestStatus,
    TBool aSetSuspend )
    {
    CMailForwardOperation* self = new( ELeave ) CMailForwardOperation( 
        aDocument,
        aObserverRequestStatus );
    
    CleanupStack::PushL( self );
    self->iReporter = &aReporter;
    self->iOperationState = aSetSuspend ? ESuspendOperation : ELoadAttachments;
    self->ConstructL();
	CleanupStack::Pop( self );
    return self;
    }

    
// Destructor
CMailForwardOperation::~CMailForwardOperation()
    {
    LOG("CMailForwardOperation::~CMailForwardOperation");       
    Cancel();
    delete iOperation;
    delete iSelection;
    }

// -----------------------------------------------------------------------------
// CMailForwardOperation::SuspendL
// -----------------------------------------------------------------------------
//
void CMailForwardOperation::SuspendL( TBool aSetSuspend )
    {
    LOG1("CMailForwardOperation::SuspendL:%d", aSetSuspend);
    if ( aSetSuspend )
        {
        ASSERT( iOperation == NULL );
        iOperation = CMailWaitOperation::NewL( 
    	    iDocument.Session(), 
    	    iStatus );
        LOG1("CMailForwardOperation::SuspendL::%08x", 
            iOperation);     
    	SetActive();         
        iOperationState = ESuspendOperation;
        }
    else
        {
        if ( iOperationState == ESuspendOperation )
            {
            delete iOperation;
            iOperation = NULL;
            iOperationState = ELoadAttachments;
            }
        }
    }

// -----------------------------------------------------------------------------
// Called when UI gets Operation completed notification 
// -----------------------------------------------------------------------------
//
TBool CMailForwardOperation::CompletedL( TInt /*aCompletionCode*/ )
    {
    TBool ret(ETrue);
   /* if ( aCompletionCode == KErrCancel && 
        (iOperationState == ESuspendOperation ||
        iOperationState == EGetAttachments || 
        iOperationState == EConnectToService ) )
        {
        LOG("CMailForwardOperation::CompletedL - continue");
        // Let's forward what we can
        iOperationState = EUpdateEntryFlags;
        NexStateL();
        ret = EFalse;
        }*/
    return ret;    
    }

// -----------------------------------------------------------------------------
// CMailForwardOperation::ProgressL
// -----------------------------------------------------------------------------
//
const TDesC8& CMailForwardOperation::ProgressL()
    {
    LOG("CMailForwardOperation::ProgressL");
    // Used CMsvProgressReporterOperation is set to display only wait bar.
    // --> So there is no need to resolve the progress
    return KNullDesC8();     
    }
// -----------------------------------------------------------------------------
// CMailForwardOperation::DoCancel
// -----------------------------------------------------------------------------
//	    
void CMailForwardOperation::DoCancel()
    {
    LOG1("CMailForwardOperation::DoCancel iOperationState:%d", 
        iOperationState);        
    LOG1("CMailForwardOperation::DoCancel iOperation::%08x", 
        iOperation);        
    if ( iOperation )
        {
        iOperation->Cancel();
        }
        
	if( iOperationState == EGetAttachments ) 
		{ // Actually we are still LOADING attachments in this case, but this
		  // state machine has been implemented so that iOperationState has
		  // already been incremented...
		LOG("CMailForwardOperation::DoCancel, Canceling attachment loading");
		iDocument.MailMessage().Cancel();
		// Note: Canceling CMailMessage::LoadAttachmentsL() does not cancel
		// our own pending request or even notify us, so we must do it manually		
		TRequestStatus* status = &iStatus;
		User::RequestComplete( status, KErrCancel );
		}        
        
    // Complete observer
    CompleteObserver( iStatus.Int() );         
    }
// -----------------------------------------------------------------------------
// CMailForwardOperation::RunL
// -----------------------------------------------------------------------------
//	    
void CMailForwardOperation::RunL()
    {
    LOG1("CMailForwardOperation::RunL iStatus:%d", iStatus.Int());
    
    if ( iStatus.Int() != KErrNone || !NexStateL() )
        {
        CompleteObserver( iStatus.Int() );
       // Deque();
        }
    }
// -----------------------------------------------------------------------------
// CMailForwardOperation::RunError
// -----------------------------------------------------------------------------
//	    
TInt CMailForwardOperation::RunError( TInt aError )
    {
    LOG1("CMailForwardOperation::RunError aError:%d", aError);
    CompleteObserver( aError );
    return KErrNone; // must return KErrNone to active sheduler
    }

// -----------------------------------------------------------------------------
// CMailForwardOperation::MessageLoadingL 
// From 
// -----------------------------------------------------------------------------
// 
void CMailForwardOperation::MessageLoadingL(
    TInt aStatus, CMailMessage& /*aMessage*/)
	{
	if( aStatus == CMailMessage::EAttachmentsReady ) 
		{
		// next state
		LOG("CMailForwardOperation::MessageLoadingL");
		TRequestStatus* status = &iStatus;
		User::RequestComplete( status, KErrNone );
		}
	}      
// -----------------------------------------------------------------------------
// CMailForwardOperation::CompleteObserver
// -----------------------------------------------------------------------------
//
void CMailForwardOperation::CompleteObserver( TInt aStatus )
    {
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, aStatus ); 
    }


// -----------------------------------------------------------------------------
// CMailForwardOperation::ConnectToServiceL
// -----------------------------------------------------------------------------
//
void CMailForwardOperation::ConnectToServiceL()
    {
    LOG("CMailForwardOperation::ConnectToServiceL");      
	// connect to service
	TBuf8<1> param;

	// set context to imap service
	TMsvId imapservice = iDocument.Mtm().Entry().OwningService();
	iDocument.Mtm().SwitchCurrentEntryL( imapservice );
	
	if ( !iDocument.Mtm().Entry().Entry().Connected() )
	    {	    
    	CMsvEntrySelection* imapbox = new(ELeave) CMsvEntrySelection;
        CleanupStack::PushL(imapbox);
        imapbox->AppendL( imapservice );
        ASSERT( iOperation == NULL );
    
    	iOperation = 
    	    iDocument.MtmUi().InvokeAsyncFunctionL( 
    	        KIMAP4MTMConnect, *imapbox, iStatus, param );
        LOG1("CMailForwardOperation::ConnectToServiceL::%08x", 
            iOperation);        
    	CleanupStack::PopAndDestroy( imapbox );        
    	SetActive();
	    }
	 else
	    {
	    // We are online, start fetch
	    ASSERT( iSelection );
	    FetchAttachmentsL( *iSelection );
	    }
    }

// -----------------------------------------------------------------------------
// CMailForwardOperation::CheckAttachmentsL
// -----------------------------------------------------------------------------
//
void CMailForwardOperation::CheckAttachmentsL()
    {
    MMsvAttachmentManager& attachmentManager = 
        iDocument.MailMessage().AttachmentManager();
    LOG1("CMailForwardOperation::CheckAttachmentsL attachmentCount:%d", 
         attachmentManager.AttachmentCount());	
    
    TInt maxSize(0);    
    // attachment count not static
    for( TInt index(0); index<attachmentManager.AttachmentCount(); ++index )    
        {
        CMsvAttachment* info = attachmentManager.GetAttachmentInfoL( index );
        CleanupStack::PushL( info );
        
        if( !info->Complete() )
            {
			// if not fetched, append to the list
			iSelection->AppendL( info->Id() );
			// check if this is 1st or largest unfetched attachment
			if ( info->Size() > maxSize )
				{
				maxSize = info->Size();
				}			                
            }
        CleanupStack::PopAndDestroy( info ); 
        }      

    FetchAttachmentsL( *iSelection, maxSize );           
    }

// -----------------------------------------------------------------------------
// CMailForwardOperation::LoadAttachmentsL
// -----------------------------------------------------------------------------
//
void CMailForwardOperation::LoadAttachmentsL()
    {
    iDocument.MailMessage().LoadAttachmentsL( *this );    
    iStatus = KRequestPending;
    SetActive();
    }
// -----------------------------------------------------------------------------
// CMailForwardOperation::ForwardMessageL
// -----------------------------------------------------------------------------
//
void CMailForwardOperation::ForwardMessageL()
    {
    LOG("CMailForwardOperation::ForwardMessageL");    
        
    iOperationState = EForwardMessage;  
    TMsvId msgId = iDocument.MailMessage().MessageEntry().Id();    
    TMsvPartList parts =
        KMsvMessagePartBody |
        KMsvMessagePartDescription |
        KMsvMessagePartAttachments;
    iDocument.SetEntryWithoutNotificationL( msgId );
    CBaseMtmUi& mtmUi = iDocument.MtmUi();

    mtmUi.SetPreferences( 
        mtmUi.Preferences() | 
        EMtmUiFlagEditorPreferEmbedded );
    // Set context to message
    iDocument.Mtm().SwitchCurrentEntryL( msgId );
    iDocument.Mtm().Entry().Entry();

    // hide the progress note
    iReporter->MakeProgressVisibleL( EFalse );

    ASSERT( iOperation == NULL );    
    iOperation = mtmUi.ForwardL( 
        iDocument.DefaultMsgFolder(), 
        parts, 
        iStatus );
    LOG1("CMailForwardOperation::ForwardMessageL::%08x", 
        iOperation); 
    SetActive();    
    }

// -----------------------------------------------------------------------------
// CMailForwardOperation::UpdateEntryFlagsL
// -----------------------------------------------------------------------------
//
void CMailForwardOperation::UpdateEntryFlagsL()
    {
    LOG("CMailForwardOperation::UpdateEntryFlagsL");
    TMsvId msgID = iDocument.MailMessage().MessageEntry().Id();
    iDocument.Mtm().SwitchCurrentEntryL( msgID );
    
    TMsvEntry msgEntry = iDocument.Mtm().Entry().Entry();
    ASSERT( msgEntry.iType == KUidMsvMessageEntry );
    msgEntry.SetComplete( ETrue );
  
    ASSERT( iOperation == NULL );   
    iOperation =  iDocument.Mtm().Entry().ChangeL( 
        msgEntry, 
        iStatus );
    LOG1("CMailForwardOperation::UpdateEntryFlagsL::%08x", 
        iOperation);     
    iOperationState = EUpdateEntryFlags;
    SetActive();
    }
    
// ------------------------------------------------------------------------------
// CMailForwardOperation::FetchAttachmentsL()
// ------------------------------------------------------------------------------
//
void CMailForwardOperation::FetchAttachmentsL( 
    CMsvEntrySelection& aSelection, TInt aMaxSize)
	{
    LOG1("CMailForwardOperation::FetchAttachmentsL aMaxSize:%d", aMaxSize);	
	// if some files are not on device, fetch them
    if ( aSelection.Count() )
        {
        TInt status( MailUtils::EQueryForward );
        if ( !iDocument.IsOnLineL() )
        	{
        	status |= MailUtils::EConnectionOffline;
        	}
        // check if large attachment is not fetched and if user
		// wants to fetch it
		if ( MailUtils::LargeAttachmentQueryL( aMaxSize, status ) )
			{
			ConnectToServiceL();
			}
		else
			{
			// User does not want to fetch, let's just forward the message.
			// After this entry completed flag is updated incorrectly, but 
			// that should not cause any problems. If completed flag is not set
			// forward operation tries to make connection to mailbox.
			UpdateEntryFlagsL();
			}
		}
	else
	    {
	    // Attachments are fetched, make sure entry flag is updated
	    UpdateEntryFlagsL();
	    }
        	
	}    

// ------------------------------------------------------------------------------
// CMailForwardOperation::FetchAttachmentsL()
// ------------------------------------------------------------------------------
//
void CMailForwardOperation::FetchAttachmentsL( 
    CMsvEntrySelection& aSelection )
    {
    TBuf8<1> param;    
	// Do actual fetch operation
    ASSERT( iOperation == NULL );
    iOperation = iDocument.Mtm().InvokeAsyncFunctionL( 
	    KIMAP4MTMPopulate, aSelection, param, iStatus );
    LOG1("CMailForwardOperation::FetchAttachmentsL::%08x", 
        iOperation); 	    
	SetActive();    
    }

// ------------------------------------------------------------------------------
// void CMailForwardOperation::NexStateL()
// ------------------------------------------------------------------------------
//
TBool CMailForwardOperation::NexStateL()
    {
    // Reset previous operation
    delete iOperation;
    iOperation = NULL;
        
    TBool valid( ETrue );
    switch( iOperationState )
        {
        case ESuspendOperation:
            SuspendL( ETrue );
            break;
            
        case ELoadAttachments:
            LoadAttachmentsL();
            iOperationState++;
            break;
            
        case EGetAttachments:
            CheckAttachmentsL();
            iOperationState++;
            break;
        
        case EConnectToService:
            // Now we are online let's fetch 'em
            FetchAttachmentsL( *iSelection );
            iOperationState++;
            break;            
            
        case EUpdateEntryFlags:
            // Now we are ready to forward
            // Attachment model is not updated, but that's not a problem?
            UpdateEntryFlagsL();
            iOperationState++;
            break;     
                        
        case EForwardMessage:
            ForwardMessageL();
            iOperationState++;
            break;
        default:
            LOG("CMailForwardOperation::NexStateL - all done");
            valid = EFalse;
            break;
        };
    
    return valid;    
    }

//  End of File  
