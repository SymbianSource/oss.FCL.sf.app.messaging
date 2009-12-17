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
* Description:  State machine for message loading 
*
*/



// INCLUDE FILES
#include 	"MailLog.h"
#include 	"MailUtils.h"
#include    "MailLoadStateInline.h"
#include    "MailLoadStateMessageAttachment.h"
#include 	"MailLoadStateMachine.h"
#include 	"MsgMailViewerHtmlConv.h"
#include 	<MailPlainView.rsg>
#include    <MMailAppUiInterface.h>
#include    <CMailMessage.h>
#include 	<StringLoader.h>

// ============================ MEMBER FUNCTIONS ===============================

/// images
// -----------------------------------------------------------------------------
// CLoadInlineImages::MessageLoadingL
// -----------------------------------------------------------------------------
//	
void CLoadInlineImages::MessageLoadingL( TInt aStatus, CMailMessage& aMessage )
	{
	if( aStatus == CMailMessage::ELinkedFileReady ) 
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
// CLoadInlineImages::CLoadInlineImages
// -----------------------------------------------------------------------------
//
CLoadInlineImages::CLoadInlineImages( 
	MMailAppUiInterface* aAppUI, 
	MStateMachine& aStateMachine )
	{
	iAppUI = aAppUI;
	iStateMachine = &aStateMachine;
	}

// -----------------------------------------------------------------------------
// CLoadInlineImages::~CLoadInlineImages
// -----------------------------------------------------------------------------
//
CLoadInlineImages::~CLoadInlineImages()
	{
	}

// -----------------------------------------------------------------------------
// CLoadInlineImages::NextStateL
// -----------------------------------------------------------------------------
//
MLoadState* CLoadInlineImages::NextStateL()
	{
	RPointerArray<HBufC>* array = iStateMachine->GetIdArray();
	if ( array && array->Count() )
		{
		return new( ELeave ) CLoadInlineImages( iAppUI, *iStateMachine );
		}
	return new( ELeave ) CLoadAttachedMessages( iAppUI );
	}

// -----------------------------------------------------------------------------
// CLoadInlineImages::MakeRequestL
// -----------------------------------------------------------------------------
//
void CLoadInlineImages::MakeRequestL( 
	TRequestStatus& aStatus,
	CMailMessage& aMessage )
	{
	iLoadStatus = &aStatus;
	RPointerArray<HBufC>* array = iStateMachine->GetIdArray();
	if ( array && array->Count() )
		{
        HBufC* item = (*array)[0];
        //LOG1( "CLoadInlineImages::MakeRequestL inlineIdArray: %S", &*item );
        aMessage.LoadLinkedHtmlContentL(
    		KNullDesC(), 
    		*item, 
    		*this );
		delete item;
		array->Remove(0); // remove item pointer from array    		
    	aStatus = KRequestPending;
		}
	else
		{
		// Nothing to do, so complete immediately
		aStatus = KRequestPending;
		TRequestStatus* observer = iLoadStatus;
		User::RequestComplete( observer, KErrNone );
		}	
	}

// -----------------------------------------------------------------------------
// CLoadInlineImages::AddAttachmentsToAppUiL
// -----------------------------------------------------------------------------
//
void CLoadInlineImages::AddAttachmentsToAppUiL( CMailMessage& aMessage )
    {    	
	RFile linkedHtml = aMessage.LinkedHtmlContent();
	if ( linkedHtml.SubSessionHandle() != 0 )
		{
		CleanupClosePushL( linkedHtml );		
		CMsvAttachment* message = CMsvAttachment::NewL( 
					CMsvAttachment::EMsvFile ); 
		CleanupStack::PushL( message );				
		TInt contentSize(0);
		linkedHtml.Size( contentSize );
		message->SetSize( contentSize );
		TFileName fileName;
		linkedHtml.Name( fileName );
		message->SetAttachmentNameL( fileName );
		message->SetComplete( ETrue );
		message->SetId( aMessage.LinkedHtmlId() );
		iAppUI->AddAttachmentL( *message, EFalse );			
        CleanupStack::PopAndDestroy(2);					// CSI: 47 # message, linkedHtml
		}	
    }
	
//  End of File  
