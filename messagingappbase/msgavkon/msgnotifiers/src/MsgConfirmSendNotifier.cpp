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
* Description:  
*     CMsgConfirmSendNotifier implementation file
*
*/



#include <uikon/eiksrvui.h>
#include <eikenv.h>
#include <eiknotapi.h>

#include "MsgConfirmSendNotifier.h"
#include "MsgConfirmSendQuery.h"
#include "MsgNotifiersConst.h"

// ---------------------------------------------------------
// CMsgConfirmSendNotifier::NewL
// ---------------------------------------------------------
//
CMsgConfirmSendNotifier* CMsgConfirmSendNotifier::NewL( TBool aSendAs )
	{
	CMsgConfirmSendNotifier* self = new ( ELeave ) CMsgConfirmSendNotifier( aSendAs );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}
	
// ---------------------------------------------------------
// CMsgConfirmSendNotifier::ConstructL
// ---------------------------------------------------------
//
void CMsgConfirmSendNotifier::ConstructL()
	{
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
//
CMsgConfirmSendNotifier::~CMsgConfirmSendNotifier()
	{
	delete iQuery;
	}

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
//
CMsgConfirmSendNotifier::CMsgConfirmSendNotifier( TBool aSendAs )
	{
	if ( aSendAs )
	    {
    	iInfo.iUid      = KSendAsNotifierPluginUid;
    	iInfo.iChannel  = KSendAsNotifierPluginUid; // use the plugin uid
    	iInfo.iPriority = MEikSrvNotifierBase2::ENotifierPriorityHigh;
	    }
	else
	    {
    	iInfo.iUid      = KSendUiNotifierPluginUid;
    	iInfo.iChannel  = KSendUiNotifierPluginUid; // use the plugin uid
    	iInfo.iPriority = MEikSrvNotifierBase2::ENotifierPriorityHigh;
	    }
	}

// ---------------------------------------------------------
// CMsgConfirmSendNotifier::Release
// ---------------------------------------------------------
//
void CMsgConfirmSendNotifier::Release()
	{
	delete this;
	}

// ---------------------------------------------------------
// CMsgConfirmSendNotifier::RegisterL
// ---------------------------------------------------------
//
CMsgConfirmSendNotifier::TNotifierInfo CMsgConfirmSendNotifier::RegisterL()
	{
	return iInfo;
	}

// ---------------------------------------------------------
// CMsgConfirmSendNotifier::Info
// ---------------------------------------------------------
//
CMsgConfirmSendNotifier::TNotifierInfo CMsgConfirmSendNotifier::Info() const
	{
	return iInfo;
	}
	
// ---------------------------------------------------------
// CMsgConfirmSendNotifier::Cancel
//
// The notifier has been deactivated so resources can be
// freed and outstanding messages completed.
// ---------------------------------------------------------
//
void CMsgConfirmSendNotifier::Cancel()
	{
	if ( iQuery )
	    {
	    iQuery->Cancel();
	    }
	// Complete iMessage?
	}

// ---------------------------------------------------------
// CMsgConfirmSendNotifier::StartL
//
// Start the Notifier with data aBuffer. 
// Not used for confirm notifiers
// ---------------------------------------------------------
//
TPtrC8 CMsgConfirmSendNotifier::StartL(const TDesC8& /*aBuffer*/)
	{
	User::Leave( KErrNotSupported );
	return KNullDesC8();
	}
	
// ---------------------------------------------------------
// CMsgConfirmSendNotifier::StartL
//
// Start the notifier with data aBuffer. aMessage should be
// completed when the notifier is deactivated.
//
// May be called multiple times if more than one client starts
// the notifier. The notifier is immediately responsible for
// completing aMessage.
// ---------------------------------------------------------
//
void CMsgConfirmSendNotifier::StartL(
        const TDesC8& aBuffer,
        TInt /*aReplySlot*/,
        const RMessagePtr2& aMessage )
	{
	iMessage = aMessage;
	
    if ( !iQuery )
        {
        iQuery = CMsgConfirmSendQuery::NewL( *this );
        }
    iQuery->ShowQueryL( aBuffer, ( iInfo.iUid == KSendAsNotifierPluginUid ) );
	}
	
// ---------------------------------------------------------
// CMsgConfirmSendNotifier::UpdateL
//
// Update a currently active notifier with data aBuffer.
// Not used for confirm notifiers.
// ---------------------------------------------------------
//
TPtrC8 CMsgConfirmSendNotifier::UpdateL( const TDesC8& /*aBuffer*/ )
	{
	User::Leave( KErrNotSupported );
	return KNullDesC8();
	}


// ---------------------------------------------------------
// CMsgConfirmSendNotifier::HandleActiveEvent
// ---------------------------------------------------------
//
void CMsgConfirmSendNotifier::HandleActiveEvent( const TInt aStatus )
	{
    if ( !iMessage.IsNull() )
        {
        // "Cancel" returns "0".
        iMessage.Complete( aStatus == 0
	            ? KErrPermissionDenied
	            : KErrNone );
	    //iMessage.Complete( iMessage.Write(
	    //    0,
	    //    TPckgC<TInt>( aButtonVal == EEikBidCancel
	    //        ? KErrPermissionDenied
	    //        : KErrNone ) ) );
        }
    }
        

// End of File
