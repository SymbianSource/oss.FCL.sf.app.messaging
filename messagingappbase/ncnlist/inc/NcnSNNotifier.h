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
* Description:   Defines class CNcnSNNotifier.
*
*/



#ifndef NCNSNNOTIFIER_H
#define NCNSNNOTIFIER_H

//  INCLUDES
#include <e32base.h>
#include <AknNotifyStd.h>
#include <secondarydisplay/NcnSecondaryDisplayAPI.h>
#include "MNcnNotifier.h"

// CLASS DECLARATION
class CAknSoftNotifier;
class CAknSoftNotificationParameters;
class TVwsViewId;
class CRepository;
/**
*  Notifies Avkon notifier about new items and idle state status.
*/
class CNcnSNNotifier : public CBase
    {
    public:  // Constructor and destructor

        /**
        * Two-phased constructor.
        */
        static CNcnSNNotifier* NewL();

        /**
        * Destructor.
        */
        virtual ~CNcnSNNotifier();

    public:

        /**
        * Informs Avkon soft notifier about received items.
        */
        void NewItems( 
            const MNcnNotifier::TNcnNotificationType	aNotificationType, 
            const TInt                      			aAmount );

        /**
        * Informs the notifier subsystem about the current idle state status.
        */
        void IdleState( TBool aCurrentState );

    private:

        /**
        * C++ default constructor.
        */
        CNcnSNNotifier();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    private:
        /**
         * Shows new items.
         */
        void ShowNewItemsL( 
            const TAknSoftNotificationType  aNotificationType, 
            const TInt                      aAmount );
        
        /**
         * Custom notification.
         */    
        void ShowCustomNotificationL(
            const TAknSoftNotificationType aNotificationType, 
            const TInt aAmount );
            
        /**
         * Normal notification.
         */    
        void ShowDefaultNotificationL(
            const TAknSoftNotificationType aNotificationType, 
            const TInt aAmount );
        
        CAknSoftNotificationParameters* VoicemailNoteParametersL( 
            const TAknSoftNotificationType aNotificationType, 
            const TBool aAmountKnown );
        /**
         * Creates grouped notification parameters.
         */    
        CAknSoftNotificationParameters* CreateNotificationParametersLC(
            const TAknSoftNotificationType aNotificationType,
            TBool aAmountKnown );

        /**
         * Hides the note.
         */
        void DoInformIdleStateL( TBool aCurrentState );
        
        /**
         * Converts NCN soft note type to avkon type
         */
        TAknSoftNotificationType ConvertNCNTypeToAvkonType( 
        	const MNcnNotifier::TNcnNotificationType aNotification ) const;

        /**
         * Converts NCN soft note type to secondary display type
         */
        SecondaryDisplay::TSecondaryDisplayNcnListDialogs ConvertNCNTypeToSecondaryDisplayType( 
        	const MNcnNotifier::TNcnNotificationType aNotification ) const;
        	        
    private: // data
        CAknSoftNotifier* iNotifier;
        
        friend class CNcnlist_SNNotifier;

        CRepository* iMuiuSettings;
    };

#endif      // NCNSNNOTIFIER_H

// End of File
