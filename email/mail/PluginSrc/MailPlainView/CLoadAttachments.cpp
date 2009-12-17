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
#include "CLoadAttachments.h"
#include "MailLog.h"
#include "MailUtils.h"
#include "MailLoadStateMachine.h"
#include <MailPlainView.rsg>
#include <MMailAppUiInterface.h>
#include <CMailMessage.h>
#include <mmsvattachmentmanager.h>
#include <StringLoader.h>

// ============================ MEMBER FUNCTIONS ===============================

/// messages
// -----------------------------------------------------------------------------
// CLoadAttachments::MessageLoadingL
// -----------------------------------------------------------------------------
//	
void CLoadAttachments::MessageLoadingL(TInt aStatus, CMailMessage& aMessage)
	{
	if( aStatus == CMailMessage::EAttachmentsReady ) 
		{
		//Tell that loading of attachments has started
		iAppUI->StartWaitNoteL();

	    MMsvAttachmentManager& manager = aMessage.AttachmentManager();		
        TRAPD( err, AddAttachmentsToAppUiL( manager ) );
        LOG1("CLoadAttachments::MessageLoadingL AddAttachmentsToAppUiL: err=%d", err );
        
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
// CLoadAttachments::CLoadAttachments
// -----------------------------------------------------------------------------
//	
CLoadAttachments::CLoadAttachments( MMailAppUiInterface* aAppUI )
	{
	iAppUI = aAppUI;
	}

// -----------------------------------------------------------------------------
// CLoadAttachments::~CLoadAttachments
// -----------------------------------------------------------------------------
//	
CLoadAttachments::~CLoadAttachments()
	{
	
	}

// -----------------------------------------------------------------------------
// CLoadAttachments::NextStateL
// -----------------------------------------------------------------------------
//	
MLoadState* CLoadAttachments::NextStateL()
	{
	return NULL;
	}

// -----------------------------------------------------------------------------
// CLoadAttachments::MakeRequestL
// -----------------------------------------------------------------------------
//		
void CLoadAttachments::MakeRequestL( 
	TRequestStatus& aStatus,
	CMailMessage& aMessage )
	{
	iLoadStatus = &aStatus;
	aMessage.LoadAttachmentsL( *this );
	aStatus = KRequestPending;
	}

// -----------------------------------------------------------------------------
// CLoadAttachments::AddAttachmentsToAppUiL
// -----------------------------------------------------------------------------
//		
void CLoadAttachments::AddAttachmentsToAppUiL( MMsvAttachmentManager& aManager )
    {
	LOG1("CLoadAttachments::AddAttachmentsToAppUiL attachmentCount:%d", 
	     aManager.AttachmentCount());

	//It is possible that aManager.AttachmentCount() changes while we are in 
	//this loop. That is why latest count is checked every round.
	for ( TInt index(0); index < aManager.AttachmentCount(); ++index ) 
		{ 
		LOG1("CLoadAttachments::AddAttachmentsToAppUiL iteration: %d", index);
		CMsvAttachment* info = aManager.GetAttachmentInfoL( index );
		CleanupStack::PushL( info );
#ifdef MAIL_ENABLE_LOGGING
        LOG("CLoadAttachments::AddAttachmentsToAppUiL Attachment info:");
		LOG1("CLoadAttachments::AddAttachmentsToAppUiL ID  : 0x%x", info->Id() );
        TPtrC name( info->AttachmentName() );
		LOG1("CLoadAttachments::AddAttachmentsToAppUiL Name: '%S'", &name );
        TPtrC path( info->FilePath() );
        LOG1("CLoadAttachments::AddAttachmentsToAppUiL Path: '%S'", &path );
        HBufC* mime = HBufC::NewLC( info->MimeType().Length() );
        mime->Des().Copy( info->MimeType() );
        LOG1("CLoadAttachments::AddAttachmentsToAppUiL Mime: '%S'", mime );
        CleanupStack::PopAndDestroy( mime );
#endif
		LOG("CLoadAttachments::AddAttachmentsToAppUiL adding atta");
		iAppUI->AddAttachmentL( *info, ETrue );
		LOG("CLoadAttachments::AddAttachmentsToAppUiL atta added");
		
		CleanupStack::PopAndDestroy( info );
		}    
    }

//  End of File  
