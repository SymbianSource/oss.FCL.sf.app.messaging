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
* Description:   Server session class.
*
*/



// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "CNcnSession.h"
#include "NcnCommsDefs.h"
#include "CNcnUnpackDescrArray.h"
#include "MNcnNotificationObserver.h"
#include <MNcnNotification.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {


}  // namespace


// ----------------------------------------------------------------------------
// CNcnSession::CNcnSession
//
// Constructor.
// ----------------------------------------------------------------------------
//
CNcnSession::CNcnSession( MNcnNotificationObserver& aObserver )
    : iObserver( aObserver )
    {
    }
    
// ----------------------------------------------------------------------------
// CNcnSession::ConstructL
//
// Symbian 2nd phase constructor.
// ----------------------------------------------------------------------------
//    
void CNcnSession::ConstructL()
    {
    }
    
// ----------------------------------------------------------------------------
// CNcnSession::~CNcnSession
//
// Destructor.
// ----------------------------------------------------------------------------
//        
CNcnSession::~CNcnSession()
    {
    }
    
// ----------------------------------------------------------------------------
// CNcnSession::NewL
//
// Symbian two-phased construction.
// ----------------------------------------------------------------------------
//
CNcnSession* CNcnSession::NewL( MNcnNotificationObserver& aObserver )
	{
	CNcnSession* self = 
	    new( ELeave ) CNcnSession( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// CNcnSession::ServiceL
//
// ServiceL method from CSession2.
// ----------------------------------------------------------------------------
//
void CNcnSession::ServiceL( const RMessage2& aMessage )
    {
    // If this method leaves default ServiceError implementation will
    // complete the message using the leave value.
    
    // However, we have to reset our member variable in any case so
    // we can't rely on this functionality, hence the trap below    
    
    if( IsRequestPending() )
        {
        PanicClient( aMessage, EPanicNcnRequestActive );
        }
    
    // set current request message
    SetRequestMessage( aMessage );    

    // handle service inside a trap so we can
    // reset our message member if service leaves
    TRAPD( error, HandleServiceL( aMessage ) );
        
    // complete & reset current message
    CompleteRequestMessage( error );
    }    

// ----------------------------------------------------------------------------
// CNcnSession::HandleServiceL
// ----------------------------------------------------------------------------
//    
void CNcnSession::HandleServiceL( const RMessage2& aMessage )
    {
    switch ( aMessage.Function() )
        {
        case ENewInternalMessages1:
            {
            ServiceNewInternalMessages1L( aMessage );            
            break;
            }
        case ENewInternalMessages2:
            {
            ServiceNewInternalMessages2L( aMessage );
            break;
            }
        case EInternalMarkUnread1:
            {
            ServiceInternalMarkUnread1L( aMessage );
            break;
            }
        case EInternalMarkUnread2:
            {
            ServiceInternalMarkUnread2L( aMessage );
            break;
            }
        case EInternalCancelMarkUnread:
            {
            CancelRequest();
            break;            
            }
        case ENewMessages1:
            {
            ServiceNewMessages1L( aMessage );
            break;
            }
        case EMarkUnread1:
            {
            ServiceMarkUnread1L( aMessage );
            break;
            }             
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CNcnSession::CompleteRequestMessage
// ----------------------------------------------------------------------------
//    
void CNcnSession::CompleteRequestMessage( TInt aError )
    {
    // if request message is set complete it with error code
    if( !iRequestMessage.IsNull() )
        {
        iRequestMessage.Complete( aError );
        }
    }

// ----------------------------------------------------------------------------
// CNcnSession::SetRequestMessage
// ----------------------------------------------------------------------------
//        
void CNcnSession::SetRequestMessage( const RMessage2& aMessage )
    {
    iRequestMessage = aMessage;
    }

// ----------------------------------------------------------------------------
// CNcnSession::IsRequestPending
// ----------------------------------------------------------------------------
//
TBool CNcnSession::IsRequestPending() const
    {
    return !iRequestMessage.IsNull();
    }
    
// ----------------------------------------------------------------------------
// CNcnSession::CancelRequest
// ----------------------------------------------------------------------------
//
void CNcnSession::CancelRequest()
    {
    // complete pending request with KErrCancel
    CompleteRequestMessage( KErrCancel );
    }

// ----------------------------------------------------------------------------
// CNcnSession::PanicClient
// ----------------------------------------------------------------------------
//    
void CNcnSession::PanicClient( const RMessage2& aMessage, TNcnSessionPanic aPanic )
    {
    _LIT( KPanicCategory, "ncnsession" );
    aMessage.Panic( KPanicCategory, aPanic );
    }
    
void CNcnSession::ServiceNewInternalMessages1L( const RMessage2& aMessage )
    {    
    TNcnNotifMessageType type =
        static_cast <TNcnNotifMessageType> ( aMessage.Int0() );
    iObserver.HandleNewInternalMessagesL( type );
    }
    
void CNcnSession::ServiceNewInternalMessages2L( const RMessage2& aMessage )
    {     
    TNcnNotifMessageType type =
        static_cast <TNcnNotifMessageType> ( aMessage.Int0() );
    TMsvId mailBox = static_cast <TMsvId> ( aMessage.Int1() );            
    CNcnUnpackDescrArray* packedInfo = CNcnUnpackDescrArray::NewLC();
    packedInfo->UnpackL( aMessage, 2 );
    iObserver.HandleNewInternalMessagesL( type, mailBox, *packedInfo );
    CleanupStack::PopAndDestroy( packedInfo );
    }       
    
void CNcnSession::ServiceInternalMarkUnread1L( const RMessage2& aMessage )
    {    
    TNcnUnreadRequestType type =
        static_cast <TNcnUnreadRequestType> ( aMessage.Int0() );
    iObserver.HandleInternalMarkUnreadL( type );
    }    
    
void CNcnSession::ServiceInternalMarkUnread2L( const RMessage2& aMessage )
    {    
    TNcnUnreadRequestType type =
        static_cast <TNcnUnreadRequestType> ( aMessage.Int0() );
        
    TMsvId mailBox = static_cast <TMsvId> ( aMessage.Int1() );        
    iObserver.HandleInternalMarkUnreadL( type, mailBox );
    }
    
void CNcnSession::ServiceNewMessages1L( const RMessage2& aMessage )
    {    
    TMsvId mailBox = static_cast <TMsvId> ( aMessage.Int0() );
    MNcnNotification::TIndicationType indicationType =
        static_cast <MNcnNotification::TIndicationType> ( aMessage.Int1() );
    CNcnUnpackDescrArray* packedInfo = CNcnUnpackDescrArray::NewLC();
    packedInfo->UnpackL( aMessage, 2 );
    iObserver.HandleNewMessagesL( mailBox, indicationType, *packedInfo );
    CleanupStack::PopAndDestroy( packedInfo );
    }
    
void CNcnSession::ServiceMarkUnread1L( const RMessage2& aMessage )
    {
    TMsvId mailBox = static_cast <TMsvId> ( aMessage.Int0() );        
    iObserver.HandleMarkUnreadL( mailBox );    
    }
        
// End of file
