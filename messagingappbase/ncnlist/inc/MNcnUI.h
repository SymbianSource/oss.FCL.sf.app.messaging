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
* Description:   Defines class MNcnUI.
*
*/



#ifndef MNCNUI_H
#define MNCNUI_H

//  INCLUDES
#include <e32base.h>
#include <msvapi.h>
#include "MNcnNotifier.h"

// CLASS DECLARATION

/**
 * Ncn UI interface.
 */
class MNcnUI
    {         
    public: // New methods
        /**
         * Show the message to the user.
         * @param aEntry The Msv entry containing the message.
         */
        virtual void ShowMessageL( const TMsvEntry& aEntry ) = 0;
        
        /**
         * Opens the specified message in message centre.
         * @param aEntry The Msv entry to open.
         */
        virtual void OpenMessageL( const TMsvEntry& aEntry ) = 0;        
        
        /**
         * Shows the delivery note for an msv entry.
         * @param aEntry The Msv entry.
         */
        virtual void ShowDeliveryNoteL( const TMsvEntry& aEntry ) = 0;
        
        /**
         * Plays the message alert tone.
         */
        virtual void PlayMessageAlertToneL() = 0;
        
        /**
         * Plays the message alert tone. This method does not leave.
         */
        virtual void PlayMessageAlertTone() = 0;
        
        /**
         * Plays the email alert tone.
         */
        virtual void PlayEMailAlertToneL() = 0;
        
        /**
         * Plays the email alert tone. This method does not leave.
         */
        virtual void PlayEMailAlertTone() = 0;
        
        /**
         * Aborts current alert tone playback, if any.
         */
        virtual void AbortAlertTonePlayback() = 0;
        
        /**
         * Flashes the display.
         */
        virtual void FlashDisplay() = 0;
        
        /**
         * Sets UIs idle state. If UI is in idle state no notifications are shown.
         * @param aIdleState Idle state status.
         */
        virtual void SetIdleState( TBool aIdleState ) = 0;
        
        /**
         * Returns the current idle state status.
         */
        virtual TBool IdleState() = 0;
        
        /**
         * Sets UIs boot phase state. If UI is in boot phase audio notifications
         * are not played.
         * @param aBootPhase Boot phase status.
         */
        virtual void SetBootPhase( TBool aBootPhase ) = 0;
        
        /**
         * Returns the current boot phase status.
         */
        virtual TBool BootPhase() = 0;
        
        /**
         * Updates the specified soft notification.
         * @param aNotification The notification type.
         * @param aAmount The amount of messages for notification.
         */
        virtual void UpdateSoftNotification(
            MNcnNotifier::TNcnNotificationType aNotification,
            TUint aAmount ) = 0;

        /**
         * Returns ETrue if visual notifications are allowed.
         * EFalse when visual notifications are not allowed (when in SAP-profile
         * for example.
         * @return ETrue if visual notifications are allowed.
         */
        virtual TBool VisualNotificationsAllowed() = 0;
        
        /**
         * Returns ETrue if audio notifications are allowed.
         * EFalse when audio notifications are not allowed (when in boot phase
         * for example.
         * @return ETrue if audio notifications are allowed.
         */
        virtual TBool AudioNotificationsAllowed() = 0;
    };
    
#endif // MNCNUI_H

// End of File
