/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Loads message attachments 
*
*/



// INCLUDE FILES
#include    "CHtmlLoadAttachments.h"
#include 	"MailLog.h"
#include 	"MailUtils.h"
#include 	"CLoadAttachedMessages.h"
#include    <MMailAppUiInterface.h>
#include    <CMailMessage.h>
#include    <MMsvAttachmentManager.h>
#include 	<stringloader.h>

// ============================ MEMBER FUNCTIONS ===============================

/// messages
// -----------------------------------------------------------------------------
// CHtmlLoadAttachments::MessageLoadingL
// -----------------------------------------------------------------------------
//	
void CHtmlLoadAttachments::MessageLoadingL(TInt aStatus, CMailMessage& aMessage)
	{
	if( aStatus == CMailMessage::EAttachmentsReady ) 
		{
		//Tell that loading of attachments has started
		iAppUI->StartWaitNoteL();

	    MMsvAttachmentManager& manager = aMessage.AttachmentManager();		
        TRAPD( err, AddAttachmentsToAppUiL( manager ) );
        
		//Tell that loading of attachments has ended
		iAppUI->StopWaitNote();
		
		// Complete observer regardless of result
		TRequestStatus* observer = iLoadStatus;
		User::RequestComplete( observer, err );
		
		// Then leave this method if error occurred
		User::LeaveIfError( err );
		}
	}

// -----------------------------------------------------------------------------
// CHtmlLoadAttachments::CHtmlLoadAttachments
// -----------------------------------------------------------------------------
//	
CHtmlLoadAttachments::CHtmlLoadAttachments( MMailAppUiInterface* aAppUI )
	{
	iAppUI = aAppUI;
	}

// -----------------------------------------------------------------------------
// CHtmlLoadAttachments::~CHtmlLoadAttachments
// -----------------------------------------------------------------------------
//	
CHtmlLoadAttachments::~CHtmlLoadAttachments()
	{
	
	}

// -----------------------------------------------------------------------------
// CHtmlLoadAttachments::NextStateL
// -----------------------------------------------------------------------------
//	
MLoadState* CHtmlLoadAttachments::NextStateL()
	{
	return new(ELeave) CLoadAttachedMessages( iAppUI );
	}

// -----------------------------------------------------------------------------
// CHtmlLoadAttachments::MakeRequestL
// -----------------------------------------------------------------------------
//		
void CHtmlLoadAttachments::MakeRequestL( 
	TRequestStatus& aStatus,
	CMailMessage& aMessage )
	{
	iLoadStatus = &aStatus;
	aMessage.LoadAttachmentsL( *this );
	aStatus = KRequestPending;
	}


// -----------------------------------------------------------------------------
// CHtmlLoadAttachments::AddAttachmentsToAppUiL
// -----------------------------------------------------------------------------
//		
void CHtmlLoadAttachments::AddAttachmentsToAppUiL(
    MMsvAttachmentManager& aManager )
    {
	LOG1("CHtmlLoadAttachments::AddAttachmentsToAppUiL attachmentCount:%d", 
	     aManager.AttachmentCount());

	//It is possible that aManager.AttachmentCount() changes while we are in 
	//this loop. That is why latest count is checked every round.
	for ( TInt index(0); index < aManager.AttachmentCount(); ++index ) 
		{ 
		LOG1("CHtmlLoadAttachments::AddAttachmentsToAppUiL iteration: %d",
		     index);
		CMsvAttachment* info = aManager.GetAttachmentInfoL( index );
		CleanupStack::PushL( info );
		
		LOG("CHtmlLoadAttachments::AddAttachmentsToAppUiL adding atta");
		iAppUI->AddAttachmentL( *info, ETrue );
		LOG("CHtmlLoadAttachments::AddAttachmentsToAppUiL atta added");
		
		CleanupStack::PopAndDestroy( info );
		}    
    }
    
//  End of File  
