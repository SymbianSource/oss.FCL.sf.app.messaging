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
* Description:   Active object for adding an attachment to the entry
*                in the message store (3.x platform).
*
*/




// INCLUDE FILES
#include "MceAttachmentAddition.h"
#include <msvapi.h>
#include <mmsvattachmentmanager.h>
#include <msvstore.h>
#include <e32std.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMceAttachmentAddition::CMceAttachmentAddition
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMceAttachmentAddition::CMceAttachmentAddition()
    : CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this); 
    }

// -----------------------------------------------------------------------------
// CMceAttachmentAddition::NewLC
// Two phase constructor
// -----------------------------------------------------------------------------
//
CMceAttachmentAddition* CMceAttachmentAddition::NewLC()
	{
    CMceAttachmentAddition* self = new(ELeave) CMceAttachmentAddition();
    CleanupStack::PushL(self);

    return self;
    }
	
// -----------------------------------------------------------------------------
// CMceAttachmentAddition::~CMceAttachmentAddition
// Destructor
// -----------------------------------------------------------------------------
CMceAttachmentAddition::~CMceAttachmentAddition()
    {
    delete iStore;
    iStore = NULL;
    Cancel();
    }	

// -----------------------------------------------------------------------------
// CMceAttachmentAddition::AttachFileL
// Assign the asyncronous request for adding the attachment into the entry.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMceAttachmentAddition::AttachFileL( CMsvEntry* aEntry, TFileName aFileName, RFile& /*aFileHandle*/, TInt aSize )
	{
	if( IsActive() )
        {
        User::Leave(KErrInUse);	    
        }
	
	// Get the attachment manager for the entry
	iStore = aEntry->EditStoreL();
	iAttachmentManager = &( iStore->AttachmentManagerL() );

	// Create a new attachment attributes object
	CMsvAttachment* attachmentInfo = CMsvAttachment::NewL( CMsvAttachment::EMsvLinkedFile );
	CleanupStack::PushL( attachmentInfo );
	
	// set the attachment name and size attributes	
	TParse fparse;
	User::LeaveIfError( fparse.Set(aFileName,NULL,NULL) );
	attachmentInfo->SetAttachmentNameL( fparse.NameAndExt() );
	attachmentInfo->SetSize( aSize );

	// Add the attachment, the ownership of the open file aFileHandle and the
	// attachmentInfo are transferred to the message server
	iAttachmentManager->AddLinkedAttachmentL(aFileName, attachmentInfo, iStatus);
	CleanupStack::Pop( attachmentInfo );

	SetActive();

    // Start the nested scheduler loop
    iWait.Start();
	}

// -----------------------------------------------------------------------------
// CMceAttachmentAddition::Result()
// Returns the error code
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
TInt CMceAttachmentAddition::Result() const
	{
	return iError;
	}

// -----------------------------------------------------------------------------
// CMceAttachmentAddition::RunL
// Completes the asyncronous service request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMceAttachmentAddition::RunL()
    {
    iError = iStatus.Int();
    
    if ( iError == KErrNone )
        {
        iStore->CommitL();
        }

    Reset();
    
    if (iWait.IsStarted())
        {
        // Stops the scheduler loop, when all nested scheduler loops have stopped.
        iWait.AsyncStop();
        }
    }

// -----------------------------------------------------------------------------
// CMceAttachmentAddition::DoCancel
// Cancels the asyncronous request, adding the attachment
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
void CMceAttachmentAddition::DoCancel()
    {
    iError = KErrCancel;
    iAttachmentManager->CancelRequest();
    Reset();
    
    if (iWait.IsStarted())
        {
        // Stops the scheduler loop, when all nested scheduler loops have stopped.
        iWait.AsyncStop();
        }
    }
  
 // -----------------------------------------------------------------------------
// CMceAttachmentAddition::RunError
// Called by the framework if RunL leaves
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMceAttachmentAddition::RunError( TInt )
 	{
 	return KErrNone;
 	}
  
// -----------------------------------------------------------------------------
// CMceAttachmentAddition::Reset
// Resets the state of the active object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMceAttachmentAddition::Reset()
    {
    __ASSERT_ALWAYS(IsActive() == EFalse, User::Invariant());

    iAttachmentManager = NULL;
    delete iStore;
    iStore = NULL;
    }


//  End of File
