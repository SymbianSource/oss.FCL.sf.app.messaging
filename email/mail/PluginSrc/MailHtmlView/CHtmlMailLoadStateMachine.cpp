/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  internal
*
*/



// INCLUDE FILES
#include    "CHtmlMailLoadStateMachine.h"
#include    "CLoadAttachedMessages.h"
#include	"CLoadHtmlFile.h"
#include 	"MailLog.h"
#include 	"MailUtils.h"
#include    <CMailMessage.h>

// ============================= LOCAL FUNCTIONS ===============================

/// State machine
// -----------------------------------------------------------------------------
// CHtmlLoadStateMachine::NewL
// -----------------------------------------------------------------------------
//

CHtmlLoadStateMachine* CHtmlLoadStateMachine::NewL( 
	MMailAppUiInterface* aAppUI,
	CMailMessage& aMessage,
	CMailHtmlBodyControl& aHtmlControl )
	{
	CHtmlLoadStateMachine* self = new(ELeave) CHtmlLoadStateMachine( 
		aAppUI, aMessage, aHtmlControl ); 
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CHtmlLoadStateMachine::ConstructL
// -----------------------------------------------------------------------------
//	
void CHtmlLoadStateMachine::ConstructL()
	{
	iState = new( ELeave ) CLoadHtmlFile( iAppUI, iHtmlControl );
   /* if ( iMessage.MessageEntry().Attachment() )
        {
        
        }
    else
        {
      //  iState = new( ELeave ) CLoadHtmlFile( iAppUI, *this );
        }*/
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CHtmlLoadStateMachine::StartL
// -----------------------------------------------------------------------------
//	
void CHtmlLoadStateMachine::StartL()
	{
	iState->MakeRequestL( iStatus, iMessage );
	SetActive();
	}
	
// -----------------------------------------------------------------------------
// CHtmlLoadStateMachine::~CHtmlLoadStateMachine
// -----------------------------------------------------------------------------
//
CHtmlLoadStateMachine::~CHtmlLoadStateMachine()
	{
	LOG( "CHtmlLoadStateMachine::~CHtmlLoadStateMachine()" );
	Cancel();
	delete iState;
	}

// -----------------------------------------------------------------------------
// CHtmlLoadStateMachine::DoCancel
// -----------------------------------------------------------------------------
//	
void CHtmlLoadStateMachine::DoCancel()
	{
	TRequestStatus* observer = &iStatus;
	User::RequestComplete( observer, KErrCancel );
		
	LOG( "CHtmlLoadStateMachine::DoCancel()" );
	iMessage.Cancel();
	}
	
// -----------------------------------------------------------------------------
// CHtmlLoadStateMachine::RunL
// -----------------------------------------------------------------------------
//	
void CHtmlLoadStateMachine::RunL()
	{
	// select next state;
	MLoadState* newState = iState->NextStateL();
	delete iState;
	iState = newState;
	if ( iState )
		{
		iState->MakeRequestL( iStatus, iMessage );
		SetActive();
		}
	else
		{
		Deque();
		}
	}
	
// -----------------------------------------------------------------------------
// CHtmlLoadStateMachine::RunError
// -----------------------------------------------------------------------------
//	
TInt CHtmlLoadStateMachine::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CHtmlLoadStateMachine::CHtmlLoadStateMachine
// -----------------------------------------------------------------------------
//
CHtmlLoadStateMachine::CHtmlLoadStateMachine( 
	MMailAppUiInterface* aAppUI, CMailMessage& aMessage,
	CMailHtmlBodyControl& aHtmlControl )
	:CActive( CActive::EPriorityStandard ),
	iAppUI( aAppUI ), 
	iMessage( aMessage ),
	iHtmlControl( aHtmlControl )
	{
	}

//  End of File  
