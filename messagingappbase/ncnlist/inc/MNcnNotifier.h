/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Defines class MNcnNotifier.
*
*/



#ifndef MNCNNOTIFIER_H
#define MNCNNOTIFIER_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
 * Ncn UI interface.
 */
class MNcnNotifier
    {
    public: // Enumerations
   
        /**
         * Ncn notification types.
         */
        enum TNcnNotificationType
            {
            ENcnMessagesNotification = 0,           // Message notification            
            ENcnAudioMessagesNotification = 1,		// Audio message notification
            ENcnClass0MessageNotification = 2,      // Class0-Message notification
            ENcnEmailNotification = 3,              // EMail notification
            ENcnVoiceMailNotification = 4,          // Voice mail notification
            ENcnVoiceMailOnLine1Notification = 5,   // Voice mail l1 notification
            ENcnVoiceMailOnLine2Notification = 6,   // Voice mail l2 notification
            ENcnMissedCallsNotification = 7,        // Missed calls notification
            ENcnInstantMessagesNotification = 8,    // Instant messages notification
            ENcnNoNotification = 9                  // No notification          
            };
            
    public: // New methods
    
        /**
         * Sets the notification amount of the specified type.
         * @param aNotificationType The notification type.
         * @param aAmount Notification amount.
  		 * @param aIcon Notifies with icon
		 * @param aTone Notifies with tone
		 * @param aNote Notifies with note
         */
        virtual void SetNotification(
            TNcnNotificationType aNotificationType,
            TUint aAmount,
			TBool aIcon = ETrue,
			TBool aTone = ETrue,
			TBool aNote = ETrue ) = 0;
            
        /**
         * Returns the notification amount for specified type.
         * @param aNotificationType The notification type.
         * @return The notification amount.
         */
        virtual TUint NotificationAmount(
            TNcnNotificationType aNotificationType ) = 0;
        
    };
    
#endif // MNCNNOTIFIER_H

// End of File
