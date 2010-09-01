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
* Description:  Loads html content 
*
*/



// INCLUDE FILES
#include 	"MailLog.h"
#include 	"MailUtils.h"
#include    "MailLoadStateMessageAttachment.h"
#include 	"MailLoadStateMachine.h"
#include 	"CLoadAttachments.h"
#include 	<MailPlainView.rsg>
#include    <MMailAppUiInterface.h>
#include    <CMailMessage.h>
#include 	<StringLoader.h>

// ============================ MEMBER FUNCTIONS ===============================

/// messages
// -----------------------------------------------------------------------------
// CLoadAttachedMessages::MessageLoadingL
// -----------------------------------------------------------------------------
//	
void CLoadAttachedMessages::MessageLoadingL(TInt aStatus, CMailMessage& aMessage)
	{
	if( aStatus == CMailMessage::EAttachedMessagesReady ) 
		{
        TRAPD( err, AddAttachmentsToAppUiL( aMessage ) );
        
        // Complete observer regardless of result
		TRequestStatus* observer = iLoadStatus;
		User::RequestComplete( observer, err );
		
		// Then leave this method if error occurred
		User::LeaveIfError( err );		
		}
	}

// -----------------------------------------------------------------------------
// CLoadAttachedMessages::CLoadAttachedMessages
// -----------------------------------------------------------------------------
//	
CLoadAttachedMessages::CLoadAttachedMessages( MMailAppUiInterface* aAppUI )
	{
	iAppUI = aAppUI;
	}

// -----------------------------------------------------------------------------
// CLoadAttachedMessages::~CLoadAttachedMessages
// -----------------------------------------------------------------------------
//	
CLoadAttachedMessages::~CLoadAttachedMessages()
	{
	
	}

// -----------------------------------------------------------------------------
// CLoadAttachedMessages::NextStateL
// -----------------------------------------------------------------------------
//	
MLoadState* CLoadAttachedMessages::NextStateL()
	{
	return new( ELeave ) CLoadAttachments( 
		iAppUI );
	}

// -----------------------------------------------------------------------------
// CLoadAttachedMessages::MakeRequestL
// -----------------------------------------------------------------------------
//		
void CLoadAttachedMessages::MakeRequestL( 
	TRequestStatus& aStatus,
	CMailMessage& aMessage )
	{
	iLoadStatus = &aStatus;
	aMessage.LoadAttachedMessagesL( *this );
	aStatus = KRequestPending;
	}


// -----------------------------------------------------------------------------
// CLoadAttachedMessages::AddAttachmentsToAppUiL
// -----------------------------------------------------------------------------
//		
void CLoadAttachedMessages::AddAttachmentsToAppUiL( CMailMessage& aMessage )
	{
    const CMsvEntrySelection& selection = aMessage.Selection();
	for ( TInt i=0; i<selection.Count(); ++i ) 
		{
		TMsvId entryID =  selection[i];
		TInt GetEntry( TMsvId aId, TMsvId &aService, TMsvEntry &aEntry );
		TMsvId service;
		TMsvEntry tEntry;
		TInt retVal = aMessage.Session()->GetEntry( 
			entryID, service, tEntry );
		if ( retVal == KErrNone ) 
			{
			_LIT8(KMessageMimeType, "message/rfc822");	    				
			CMsvAttachment* message = CMsvAttachment::NewL( 
				CMsvAttachment::EMsvMessageEntry ); 
			CleanupStack::PushL( message );
			message->SetSize( tEntry.iSize );
			message->SetMimeTypeL( KMessageMimeType );
			message->SetAttachmentNameL( tEntry.iDescription );
			message->SetComplete( tEntry.Complete() );
			message->SetId( entryID );
			iAppUI->AddAttachmentL( *message, EFalse );
			CleanupStack::PopAndDestroy(); //message
			}
		}
	}

//  End of File  
