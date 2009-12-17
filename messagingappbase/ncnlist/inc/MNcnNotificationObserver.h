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
* Description:  
*       Interface for handling received and unpacked IPC notifications.
*
*/



#ifndef __MNCNNOTIFICATIONOBSERVER_H__
#define __MNCNNOTIFICATIONOBSERVER_H__

//  INCLUDES
#include <NcnNotificationDefs.h>
#include <MNcnNotification.h>
#include <msvstd.h>

// FORWARD DECLARATIONS
class MDesCArray;

// CLASS DECLARATION

/**
 * Interface for handling received and unpacked IPC notifications.
 *
 */
class MNcnNotificationObserver
    {
    public: // Interface

		/**
         * Callback method for handling NewInternalMessages request.
         * @param aType    The message type for new message(s)
         * @return         KErrNone or some system wide error code
         */
        virtual void HandleNewInternalMessagesL(
                        const TNcnNotifMessageType aType ) = 0;
                                                
		/**
         * Callback method for handling NewInternalMessages request.
         * @param aType    The message type for new message(s)
         * @param aMailBox The id for mail message type identifying mailbox or
         *                 individual mail folder that contains new message(s)
         * @param aInfo    Optional extra info about message (subject, sender etc.)
         * @return         KErrNone or some system wide error code         
         */
        virtual void HandleNewInternalMessagesL(
                        const TNcnNotifMessageType aType,
                        const TMsvId& aMailBox,
                        const MDesCArray& aInfo ) = 0;                            

		/**
         * Callback method for handling InternalMarkUnread request.
		 * @param aRequest The request maker type (method caller), identifies
		 *                 what type of messages should be marked as unread and
		 *                 whether there has to be made checks for messages.
		 * @param sStatus  Request status.
         * @return         KErrNone or some system wide error code         		 
         */
        virtual void HandleInternalMarkUnreadL(
                        const TNcnUnreadRequestType aRequest ) = 0;

		/**
         * Callback method for handling InternalMarkUnread request.
		 * @param aRequest The request maker type (method caller), identifies
		 *                 what type of messages should be marked as unread and
		 *                 whether there has to be made checks for messages.
		 * @param aMailBox The id for mail message type identifying mailbox or
         *                 mail folder containing message(s) to be marked as unread.
         * @return         KErrNone or some system wide error code                  
         */
        virtual void HandleInternalMarkUnreadL(
                        const TNcnUnreadRequestType aRequest,
                        const TMsvId& aMailbox ) = 0;

		/**
		 * Callback method for handling NewMessages request.
         * @param aMailBox The id for mail message type identifying mailbox or
         *                 individual mail folder that contains new message(s)
         * @param aIndicationType Indication type for new messages.
         * @param aInfo    Optional extra info about message (subject, sender etc.)
         * @return         KErrNone or some system wide error code                  
         */
        virtual void HandleNewMessagesL(
                        const TMsvId& aMailBox,
                        const MNcnNotification::TIndicationType aIndicationType,
                        const MDesCArray& aInfo ) = 0;

        /**
         * Callback method for handling MarkUnread request.
         * @param aMailBox The id for mail message type identifying mailbox or
         *                 mail folder containing message(s) to be marked as unread.
         */
        virtual void HandleMarkUnreadL(
                        const TMsvId& aMailBox ) = 0;
    };


#endif // __MNCNNOTIFICATIONOBSERVER_H__

// End of File
