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
* Description:   Defines class CNcnNotifier.
*
*/



#ifndef CNCNNOTIFIER_H
#define CNCNNOTIFIER_H

//  INCLUDES
#include <e32base.h>
#include "MNcnNotifier.h"

// FORWARD DECLARATIONS
class MNcnUI;
class CNcnModelBase;

// CLASS DECLARATION

/**
 * Ncn Notifier implementation.
 */
class CNcnNotifier : public CBase, public MNcnNotifier
    {
    private:    // Constructors and destructor
    
        /**
         * Constructor.
         * @param aNcnUI Ncn UI reference.
         */
        CNcnNotifier(
            MNcnUI& aNcnUI,
            CNcnModelBase& iModel );
        
        /**
         * 2nd. phase constructor.
         */
        void ConstructL();
        
    public:     // Constructors and destructor
    
        /**
         * Destructor.
         */
        virtual ~CNcnNotifier();
        
        /**
         * Static factory method.  
         * @param aNcnUI Ncn UI reference.
         */
        static CNcnNotifier* NewL(
            MNcnUI& aNcnUI,
            CNcnModelBase& iModel );
        
    public:     // From MNcnNotifier
    
        void SetNotification(
                TNcnNotificationType aNotificationType,
                TUint aAmount,
				TBool aIcon = ETrue,
				TBool aTone = ETrue,
				TBool aNote = ETrue );			
			
        TUint NotificationAmount( TNcnNotificationType aNotificationType );


    private:    // New method
    
        /**
         * Handles notification change for specific notification type.
         * @param aNotifcationType The notification type.
         * @param aPreviousAmount Amount in previous notification.
         * @param aCurrentAmount Current notification amount.
  		 * @param aIcon Notifies with icon
		 * @param aTone Notifies with tone
		 * @param aNote Notifies with note
         */
        void HandleNotificationChange(
            TNcnNotificationType aNotificationType,
            TUint aPreviousAmount,
            TUint aCurrentAmount,
			TBool aIcon,
			TBool aTone,
			TBool aNote );

        /**
         * Handles class0 message notification change.
         * @param aPreviousAmount Amount in previous notification.
         * @param aCurrentAmount Current notification amount.
         */
        void HandleClass0MessageNotificationChange(
            TUint aPreviousAmount,
            TUint aCurrentAmount );
        
        /**
         * Handles message notification change.
         * @param aPreviousAmount Amount in previous notification.
         * @param aCurrentAmount Current notification amount.
         */
        void HandleMessageNotificationChange(
            TUint aPreviousAmount,
            TUint aCurrentAmount );
            
        /**
         * Handles voice mail notification change.
         * @param aPreviousAmount Amount in previous notification.
         * @param aCurrentAmount Current notification amount.
         */
        void HandleVoiceMailNotificationChange(
            TUint aPreviousAmount,
            TUint aCurrentAmount );
            
        /**
         * Handles email notification change.
         * @param aPreviousAmount Amount in previous notification.
         * @param aCurrentAmount Current notification amount.
         */
        void HandleEMailNotificationChange(
            TUint aPreviousAmount,
            TUint aCurrentAmount,
			TBool aIcon,
			TBool aTone  );  
			          
        /**
         * Handles audio message notification change.
         * @param aPreviousAmount Amount in previous notification.
         * @param aCurrentAmount Current notification amount.
         */
        void HandleAudioMessageNotificationChange(
            TUint aPreviousAmount,
            TUint aCurrentAmount ); 
		
        /**
         * Updates the email icon based based on amount of
         * waiting email messages.
         * @param aAmount Amount of waiting email messages.
         */
        void UpdateEmailIcon( TInt aAmount );
        
        /**
         * Updates the message icon based on amount of
         * new messages.
         * @param aAmount Amount of new messages.
         */
        void UpdateMessageIcon( TInt aAmount );

        /**
         * Checks if the soft notification should be displayed or not.
         * @param aNotificationType Type of the notification
         * @return EFalse if soft notification should not be displayed, 
         * default value is ETrue
         */
        TBool CheckSNStatusL( const TNcnNotificationType& aNotificationType ) const;                        
                        
        /**
        * Checks if the Display Lights should be Set or not.
        * @param aNotificationType Type of the notification
        * @return EFalse if soft notification should not be displayed, 
        * default value is ETrue
        */
       TBool CheckDLStatus( const TNcnNotificationType& aNotificationType ) const;                        
      
    private:    // Data
    
        // ncn UI reference
        MNcnUI& iNcnUI;
        
        // ncn model reference
        CNcnModelBase& iModel;
        
        // notification amounts
        TUint iNotificationAmounts[ ENcnNoNotification ];
        //Display Light Configuration
        TInt iDisplayLightstate;
    
    };
    
#endif // CNCNNOTIFIER_H

// End of File
