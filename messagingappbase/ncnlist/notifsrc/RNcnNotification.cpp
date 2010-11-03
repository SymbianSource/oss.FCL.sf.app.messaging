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
* Description:  RNcnNotification implementation
*
*/



// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "RNcnNotification.h"
#include "NcnCommsDefs.h"
#include "CNcnPackDescrArray.h"
#include "NcnNotificationUIDs.hrh"
#include <bamdesca.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

const TInt KNcnNotifMessageSlots = 1; // only synchronous services -> 1 slot
    
}  // namespace

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// RNcnNotification::NewL
//
// Heap constructor for ECOM use.
// Note: this class can be instantiated to stack but we hide
// class type from the ECOM API client -> instantiate to heap.
// ----------------------------------------------------------------------------
//
RNcnNotification* RNcnNotification::NewL()
	{
	RNcnNotification* self = 
	    new( ELeave ) RNcnNotification();
	CleanupDeletePushL( self );
	self->InitializeSessionL();
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// RNcnNotification::RNcnNotification
//
// Constructor.
// ----------------------------------------------------------------------------
//
RNcnNotification::RNcnNotification()
    {
    }
    
// ----------------------------------------------------------------------------
// RNcnNotification::~RNcnNotification
//
// Destructor.
// ----------------------------------------------------------------------------
//        
RNcnNotification::~RNcnNotification()
    {
    Close();
    }
    
// ----------------------------------------------------------------------------
// RNcnNotification::InitializeSessionL
//
// Initializes server session.
// ----------------------------------------------------------------------------
//    
void RNcnNotification::InitializeSessionL()
    {
    TVersion version( KNcnNotificationServerMajor,
                      KNcnNotificationServerMinor,
                      KNcnNotificationServerBuild );    
    User::LeaveIfError( CreateSession( KNcnServerName,
                                       version,
                                       KNcnNotifMessageSlots ) );
    }

// ----------------------------------------------------------------------------
// RNcnNotification::NewMessages
//
// From MNcnNotification interface.
// ----------------------------------------------------------------------------
//
TInt RNcnNotification::NewMessages(
    const TMsvId& aMailBox,
    MNcnNotification::TIndicationType aIndicationType,
    const MDesCArray& aInfo )
	{	
	TInt err = KErrNone;		
	TDesC8* packed = 0;
		    
    TIpcArgs args( aMailBox, aIndicationType );            
    TRAP( err, packed = CNcnPackDescrArray::PackL( aInfo ) );        
    
    // only try to send if packing succeeded
    if( err == KErrNone )
        {
        args.Set( 2, packed );
        err = SendReceive( ENewMessages1, args );
        delete packed;
        }
            
    return err;
	}

// ----------------------------------------------------------------------------
// RNcnNotification::MarkUnread
//
// From MNcnNotification interface.
// ----------------------------------------------------------------------------
//	
TInt RNcnNotification::MarkUnread( const TMsvId& aMailBox )
    {    
	TIpcArgs args( aMailBox );
    return SendReceive( EMarkUnread1, args );
    }	
		
// ----------------------------------------------------------------------------
// RNcnNotification::NewMessages
//
// From MNcnInternalNotification interface.
// ----------------------------------------------------------------------------
//
TInt RNcnNotification::NewMessages(
    const TNcnNotifMessageType aType )
	{	
	TIpcArgs args( aType );
    return SendReceive( ENewInternalMessages1, args );
	}
	
// ----------------------------------------------------------------------------
// RNcnNotification::NewMessages
//
// From MNcnInternalNotification interface.
// ----------------------------------------------------------------------------
//
TInt RNcnNotification::NewMessages(
    const TNcnNotifMessageType aType,
    const TMsvId& aMailBox,
    const MDesCArray& aInfo )
	{	
	TInt err = KErrNone;
	TDesC8* packed = 0;
			
	TIpcArgs args( aType, aMailBox );
    TRAP( err, packed = CNcnPackDescrArray::PackL( aInfo ) );        
	
	// only try to send if packing succeeded
	if( err == KErrNone )
	    {
	    args.Set( 2, packed );
	    err = SendReceive( ENewInternalMessages2, args );
	    delete packed;
	    }
	    
    return err;
	}

// ----------------------------------------------------------------------------
// RNcnNotification::MarkUnread
//
// From MNcnInternalNotification interface.
// ----------------------------------------------------------------------------
//
TInt RNcnNotification::MarkUnread(
    const TNcnUnreadRequestType aRequest )
	{
	TIpcArgs args( aRequest );
    return SendReceive( EInternalMarkUnread1, args );
	}
	
	
// ----------------------------------------------------------------------------
// RNcnNotification::MarkUnread
//
// From MNcnInternalNotification interface.
// ----------------------------------------------------------------------------
//
void RNcnNotification::MarkUnread(
    const TNcnUnreadRequestType aRequest,
    TRequestStatus& aRequestStatus )
	{
	TIpcArgs args( aRequest );
    return SendReceive( EInternalMarkUnread1, args, aRequestStatus );
	}				

// ----------------------------------------------------------------------------
// RNcnNotification::MarkUnread
//
// From MNcnInternalNotification interface.
// ----------------------------------------------------------------------------
//
TInt RNcnNotification::MarkUnread(
    const TNcnUnreadRequestType aRequest,
    const TMsvId& aMailbox )
	{
	TIpcArgs args( aRequest, aMailbox );
    return SendReceive( EInternalMarkUnread2, args );
	}
	
// ----------------------------------------------------------------------------
// RNcnNotification::MarkUnread
//
// From MNcnInternalNotification interface.
// ----------------------------------------------------------------------------
//
void RNcnNotification::MarkUnread(
    const TNcnUnreadRequestType aRequest,
    const TMsvId& aMailbox,
    TRequestStatus& aRequestStatus )
	{
	TIpcArgs args( aRequest, aMailbox );
    return SendReceive( EInternalMarkUnread2, args, aRequestStatus );
	}

// ----------------------------------------------------------------------------
// RNcnNotification::CancelMarkUnread
//
// From MNcnInternalNotification interface.
// ----------------------------------------------------------------------------
//	
void RNcnNotification::CancelMarkUnread()
    {
    SendReceive( EInternalCancelMarkUnread, TIpcArgs() );
    }

// End of file
