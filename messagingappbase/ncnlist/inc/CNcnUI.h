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
* Description:   Defines class CNcnUI.
*
*/



#ifndef CNCNUI_H
#define CNCNUI_H

//  INCLUDES
#include <e32base.h>
#include "MNcnUI.h"
#include "NcnSubscribeHandler.h"

// FORWARD DECLARATIONS
class CNcnModelBase;
class CNcnHandlerAudio;
class CNcnClass0Viewer;
class CNcnSNNotifier;
class CNcnSubscriber;

// CLASS DECLARATION

/**
 * Ncn UI implementation.
 */
class CNcnUI : public CBase, public MNcnUI, public MNcnSubscribeHandler
    {
    private:    // Constructors and destructor
    
        /**
         * Constructor.
         * @param aModel Ncn model reference.
         */
        CNcnUI( CNcnModelBase& aModel );
        
        /**
         * 2nd. phase constructor.
         */
        void ConstructL();
        
    public:     // Constructors and destructor
    
        /**
         * Destructor.
         */
        virtual ~CNcnUI();
        
        /**
         * Static factory method.  
         * @param aModel Ncn model reference.
         */
        static CNcnUI* NewL( CNcnModelBase& aModel );
        
    public:     // From MNcnUI
    
        void ShowMessageL( const TMsvEntry& aEntry );
        void OpenMessageL( const TMsvEntry& aEntry );        
        void ShowDeliveryNoteL( const TMsvEntry& aEntry );
        void PlayMessageAlertToneL();
        void PlayMessageAlertTone();
        void PlayEMailAlertToneL();        
        void PlayEMailAlertTone();
        void AbortAlertTonePlayback();
        void FlashDisplay();
        void SetIdleState( TBool aIdleState );
        TBool IdleState();
        void SetBootPhase( TBool aBootPhase );
        TBool BootPhase();
        void UpdateSoftNotification(
            MNcnNotifier::TNcnNotificationType aNotification,
            TUint aAmount );
        TBool VisualNotificationsAllowed();
        TBool AudioNotificationsAllowed();
        
    private:    // From MNcnSubscribeHandler
    
        void HandlePropertyChangedL( const TUid& aCategory, TInt aKey );
        
    private:    // New methods
    
        /**
         * Returns ETrue if SAP is connected.
         * @return ETrue if SAP is connected.
         */
        static TBool SAPConnected();
        
    private:    // Data
    
        // ncn model referece
        CNcnModelBase& iNcnModel;
                
        // pointer to audio handler, owned.
        CNcnHandlerAudio* iAudioHandler;
        
        // pointer to class0 viewer, owned.
        CNcnClass0Viewer* iClass0Viewer;
        
        // soft note notifier, owned.
        CNcnSNNotifier* iNotifier;
        
        // idle state status
        TBool iIdleState;
        
        // boot phase status
        TBool iBootPhase;
        
        // SAP connection status
        TBool iSAPConnected;        
        
        // subscriber for SAP connection
        CNcnSubscriber* iSAPSubscriber;        
    };
    
#endif // CNCNUI_H

// End of File
