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
*       S60-internal ECom notification API towards NcnList.
*
*/



#ifndef __MNCNINTERNALNOTIFICATION_H__
#define __MNCNINTERNALNOTIFICATION_H__

//  INCLUDES
#include <mncnnotification.h>
#include <ncnnotificationdefs.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * S60-internal ECom notification API towards NcnList.
 * This class extends the public notification API declared by MNcnNotification.
 */
class MNcnInternalNotification: public MNcnNotification
    {
    public: // Constructor and destructor

       /**
        * ECom implementation class factory method.
        * Caller takes the ownership of the created object.
        * If interface needs to be pushed into CleanupStack,
        * remember to use CleanupStackDeletePushL() function!
        * DO NOT USE CleanupStack::PushL()!!        
        * @return ECom implementation instance
        */
        static MNcnInternalNotification* CreateMNcnInternalNotificationL();

       /**
        * Destructor.
        */
        virtual ~MNcnInternalNotification();
            
    public: // Interface

		/**
         * Called by for example Email plugin(s) to inform NcnList that
         * there's new message(s)
         *
         * @param aType    The message type for new message(s)
         *
         * @return         KErrNone for success, or some error code
         */
        virtual TInt NewMessages( const TNcnNotifMessageType aType ) = 0;

		/**
         * Called by for example Email plugin(s) to inform NcnList that
         * there's new message(s). Parameter aInfo is reserved for 
         * future use and is not handled in any way currently.
         *
         * @param aType    The message type for new message(s)
         * @param aMailBox The id for mail message type identifying mailbox or
         *                 individual mail folder that contains new message(s)
         * @param aInfo    Optional info about message (subject, sender etc.)
         *
         * @return         KErrNone for success, or some error code
         */
        virtual TInt NewMessages( const TNcnNotifMessageType aType,
                                  const TMsvId& aMailBox,
                                  const MDesCArray& aInfo ) = 0;

        /**
         * Called by component wanting to make a request about marking certain
         * new messages as unread.
         *
		 * @param aRequest The request maker type (method caller), identifies
		 *                 what type of messages should be marked as unread and
		 *                 whether there has to be made checks for messages.
         *
         * @return         KErrNone for success, or some error code
         */
        virtual TInt MarkUnread( const TNcnUnreadRequestType aRequest ) = 0;
        
        /**
         * Called by component wanting to make a request about marking certain
         * new messages as unread. This is an asynchronic version.
         *
		 * @param aRequest The request maker type (method caller), identifies
		 *                 what type of messages should be marked as unread and
		 *                 whether there has to be made checks for messages.
		 * @param aRequestStatus Request status.
         *
         * @return         KErrNone for success, or some error code
         */
        virtual void MarkUnread( const TNcnUnreadRequestType aRequest,
                                 TRequestStatus& aRequestStatus ) = 0;                                                                          

       /**
         * Called by component wanting to make a request about marking certain
         * new messages as unread.
         *
		 * @param aRequest The request maker type (method caller), identifies
		 *                 what type of messages should be marked as unread and
		 *                 whether there has to be made checks for messages.
		 * @param aMailBox The id for mail msg type identifying mailbox or mail
         *                 folder containing message(s) to be marked as unread.
         *
         * @return         KErrNone for success, or some error code
         */
        virtual TInt MarkUnread( const TNcnUnreadRequestType aRequest,
                                 const TMsvId& aMailbox ) = 0;                                                                      
        
        /**
         * Called by component wanting to make a request about marking certain
         * new messages as unread. This is an asynchronic version.
         *
		 * @param aRequest The request maker type (method caller), identifies
		 *                 what type of messages should be marked as unread and
		 *                 whether there has to be made checks for messages.
		 * @param aMailBox The id for mail msg type identifying mailbox or mail
         *                 folder containing message(s) to be marked as unread.
         * @param aRequestStatus Request status.
         *
         * @return         KErrNone for success, or some error code
         */                         
        virtual void MarkUnread( const TNcnUnreadRequestType aRequest,
                                 const TMsvId& aMailbox,
                                 TRequestStatus& aRequestStatus ) = 0;
        
        /**
         * Cancels an outstanding MarkUnread request.
         */                         
        virtual void CancelMarkUnread( ) = 0;
    };

#include <mncninternalnotification.inl>

#endif // __MNCNINTERNALNOTIFICATION_H__

// End of File
