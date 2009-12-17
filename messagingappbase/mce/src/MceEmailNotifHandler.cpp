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
* Description:   Active object for marking the e-mails as read and resetting
*				 the count of the unread e-mails.
*
*/




// INCLUDE FILES
#include "MceEmailNotifHandler.h"
#include <e32std.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMceEmailNotifHandler::CMceAttachmentAddition
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMceEmailNotifHandler::CMceEmailNotifHandler() : CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CMceEmailNotifHandler::NewLC
// Two phase constructor
// -----------------------------------------------------------------------------
//
CMceEmailNotifHandler* CMceEmailNotifHandler::NewLC()
	{
    CMceEmailNotifHandler* self = new(ELeave) CMceEmailNotifHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CMceEmailNotifHandler::NewLC
// Two phase constructor
// -----------------------------------------------------------------------------
//
CMceEmailNotifHandler* CMceEmailNotifHandler::NewL()
	{
	CMceEmailNotifHandler* self = NewLC();
    CleanupStack::Pop(); // self
    return self;
	}

// -----------------------------------------------------------------------------
// CMceEmailNotifHandler::NewLC
// Two phase constructor
// -----------------------------------------------------------------------------
//	
void CMceEmailNotifHandler::ConstructL()
	{
	iNcnNotification = MNcnInternalNotification::CreateMNcnInternalNotificationL();
	}
	
// -----------------------------------------------------------------------------
// CMceEmailNotifHandler::~CMceAttachmentAddition
// Destructor
// -----------------------------------------------------------------------------
CMceEmailNotifHandler::~CMceEmailNotifHandler()
    {
    Cancel();
    delete iNcnNotification;
    iNcnNotification = NULL;
    }	

// -----------------------------------------------------------------------------
// CMceEmailNotifHandler::AttachFileL
// Assign the asyncronous request for adding the attachment into the entry.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMceEmailNotifHandler::HandleNotif()
	{
	if( IsActive() )
        {
        // Operation is allready running
        return;	    
        }
	
	iNcnNotification->MarkUnread( EIndexMailBox, iStatus );
	SetActive();
	}

// -----------------------------------------------------------------------------
// CMceEmailNotifHandler::HandleNotif
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMceEmailNotifHandler::HandleNotif(const TMsvId& aMailbox)
    {
    if( IsActive() )
        {
        // Operation is allready running
        return;     
        }
    
    iNcnNotification->MarkUnread( EIndexMailBox, aMailbox,iStatus );
    SetActive();
    }
// -----------------------------------------------------------------------------
// CMceEmailNotifHandler::RunL
// Completes the asyncronous service request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMceEmailNotifHandler::RunL()
    {
    ;// nothing
    }

// -----------------------------------------------------------------------------
// CMceEmailNotifHandler::DoCancel
// Cancels the asyncronous request, adding the attachment
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
void CMceEmailNotifHandler::DoCancel()
    {
    // Cancel operation of the async request not called!
    iNcnNotification->CancelMarkUnread();
    }
  
 // -----------------------------------------------------------------------------
// CMceEmailNotifHandler::RunError
// Called by the framework if RunL leaves
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMceEmailNotifHandler::RunError( TInt )
 	{
 	return KErrNone;
 	}

//  End of File
