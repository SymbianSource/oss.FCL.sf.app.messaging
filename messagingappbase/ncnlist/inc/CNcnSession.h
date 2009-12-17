/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CNCNSESSION_H__
#define __CNCNSESSION_H__

//  INCLUDES
#include <e32base.h>

//  ENUMERATIONS
enum TNcnSessionPanic
    {
    EPanicNcnRequestActive
    };
    
// FORWARD DECLARATIONS
class MNcnNotificationObserver;

// CLASS DECLARATION

/**
 * Server session class for handling client messages from
 * NCN Notification API.
 *
 */
class CNcnSession : public CSession2
    {
    public: // Constructor and destructor

       /**
        * Two-phased constructor.
        *
        * @return CNcnSession object
        */
        static CNcnSession* NewL( MNcnNotificationObserver& aObserver );

       /**
        * Destructor.
        */
        ~CNcnSession();

    public: // From CSession2
    
        void ServiceL( const RMessage2 &aMessage );
    
    private:
    
        void ServiceNewInternalMessages1L( const RMessage2& aMessage );
        
        void ServiceNewInternalMessages2L( const RMessage2& aMessage );        
        
        void ServiceInternalMarkUnread1L( const RMessage2& aMessage );
        
        void ServiceInternalMarkUnread2L( const RMessage2& aMessage );
        
        void ServiceNewMessages1L( const RMessage2& aMessage );
        
        void ServiceMarkUnread1L( const RMessage2& aMessage );
        
        void CancelRequest();
        
        /**
         * Completes and resets a request message, if any.
         * After completion message is reset.
         * @param aError Completion error code.
         */
        void CompleteRequestMessage( TInt aError );
        
        /**
         * Sets the current request message.
         * @param aMessage
         */
        void SetRequestMessage( const RMessage2& aMessage );
        
        /**
         * Return ETrue if there is a pending request.
         * @return ETrue if there is a pending request.
         */
        TBool IsRequestPending() const;
        
        /**
         * Service implementation.
         * @param aMessage Request message.
         */
        void HandleServiceL( const RMessage2& aMessage );
        
        /**
         * Internal mark unread service implementation.
         * @param aMessage Request message.
         */
        void HandleMarkUnreadServiceL( const RMessage2& aMessage );

        /**
         * Panics the client with specified panic code.
         * @param aMessage The request message.
         * @param aPanic Panic code.
         */
        void PanicClient( const RMessage2& aMessage, TNcnSessionPanic aPanic );
        
    private:

       /**
        * C++ default constructor.
        */
        CNcnSession( MNcnNotificationObserver& aObserver );

       /**
        *  Constructor, second phase
        */
        void ConstructL();

	private:    // data
	
	    MNcnNotificationObserver& iObserver;
	    
	    /**
	     * Pointer to request message.
	     */
	    RMessagePtr2 iRequestMessage;
    };


#endif // __CNCNSESSION_H__

// End of File
