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
* Description:  internal
*
*/



// INCLUDE FILES
#include    "MailLoadStateMachine.h"
#include    "MailLoadStateMessageAttachment.h"
#include	"MailLoadStateHtml.h"
#include 	"MailLog.h"
#include 	"MailUtils.h"
#include    <CMailMessage.h>

// ============================= LOCAL FUNCTIONS ===============================

/// State machine
// -----------------------------------------------------------------------------
// CLoadStateMachine::NewL
// -----------------------------------------------------------------------------
//

CLoadStateMachine* CLoadStateMachine::NewL( 
	MMailAppUiInterface* aAppUI,
	CMailMessage& aMessage )
	{
	CLoadStateMachine* self = new(ELeave) CLoadStateMachine( 
		aAppUI, aMessage ); 
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CLoadStateMachine::ConstructL
// -----------------------------------------------------------------------------
//	
void CLoadStateMachine::ConstructL()
	{
	if ( iMessage.MessageEntry().MHTMLEmail() )
		{
		iState = new( ELeave ) CLoadHtmlFile( iAppUI, *this );
		}
	else
		{
		iState = new( ELeave ) CLoadAttachedMessages( iAppUI );
		}
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CLoadStateMachine::StartL
// -----------------------------------------------------------------------------
//	
void CLoadStateMachine::StartL()
	{
	iState->MakeRequestL( iStatus, iMessage );
	SetActive();
	}
	
// -----------------------------------------------------------------------------
// CLoadStateMachine::~CLoadStateMachine
// -----------------------------------------------------------------------------
//
CLoadStateMachine::~CLoadStateMachine()
	{
	LOG( "CLoadStateMachine::~CLoadStateMachine()" );
	Cancel();
	delete iState;
	if ( iIdArray )
		{
		iIdArray->ResetAndDestroy();
		delete iIdArray;
		}
	}

// -----------------------------------------------------------------------------
// CLoadStateMachine::DoCancel
// -----------------------------------------------------------------------------
//	
void CLoadStateMachine::DoCancel()
	{
	TRequestStatus* observer = &iStatus;
	User::RequestComplete( observer, KErrCancel );
		
	LOG( "CLoadStateMachine::DoCancel()" );
	iMessage.Cancel();
	}
	
// -----------------------------------------------------------------------------
// CLoadStateMachine::RunL
// -----------------------------------------------------------------------------
//	
void CLoadStateMachine::RunL()
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
// CLoadStateMachine::RunError
// -----------------------------------------------------------------------------
//	
TInt CLoadStateMachine::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CLoadStateMachine::SetIdArray
// -----------------------------------------------------------------------------
//
void CLoadStateMachine::SetIdArray( RPointerArray<HBufC>* aIdArray )
	{
	iIdArray = aIdArray;
	}

// -----------------------------------------------------------------------------
// CLoadStateMachine::CLoadStateMachine
// -----------------------------------------------------------------------------
//
RPointerArray<HBufC>* CLoadStateMachine::GetIdArray()
	{
	return iIdArray;
	}

// -----------------------------------------------------------------------------
// CLoadStateMachine::CLoadStateMachine
// -----------------------------------------------------------------------------
//
CLoadStateMachine::CLoadStateMachine( 
	MMailAppUiInterface* aAppUI, CMailMessage& aMessage )
	:CActive( CActive::EPriorityStandard ),
	iAppUI( aAppUI ), 
	iMessage( aMessage )
	{
	}

//  End of File  
