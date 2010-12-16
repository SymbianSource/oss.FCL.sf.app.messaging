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
* Description:   Defines an abstract class CNcnModelBase.
*
*/


#ifndef NCNMODELBASE_H
#define NCNMODELBASE_H

// INCLUDES
#include    "NcnDebug.h"
#include    "MNcnUI.h"
#include    "MNcnNotifier.h"
#include    <e32base.h>

// CONSTANTS

/*
* The internal status bits of ncn model are defined here.
* The status information is intended to be stored in bits of one unsigned integer
* so that each status is presented by one bit.
*/
const TUint8 KNcnIdleState = 0x04;      // The current status of idle state (yes or no)
const TUint8 KNcnSmsServicePresent = 0x08; // If the sms service is present and operational
const TUint8 KNcnSimChanged = 0x10;     // Has the sim card changed between the last two boots
const TUint8 KNcnSimServiceCentreFetched = 0x20; // Indicates if the sim sc operation was performed
const TUint8 KNcnBootPhase  = 0x40;     // Indicates if the ncn is still in its construction phase
const TUint8 KNcnSystemRefresh = 0x80;  // Indicates if system refresh is needed and
                                        // sim sc operation is performed.
const TUint KNcnLastVmi = 0x100;        // Last line vmi arrived: 0 = line1, 1 = line 2
const TUint KNcnArrayCleared = 0x200;
const TUint KNcnReadSimSc = 0x400;      // If set SIM service center number is refreshed in boot

const TUint KNcnOfflineSupport  = 0x01; // Flag for offline support
const TUint KNcnSendFileInCall  = 0x04; // Flag for SFI support
const TUint KNcnIdSimCard       = 0x08; // Flag for SIM-card support
const TUint KNcnAudioMessaging  = 0x10; // Flag for Audio messaging support

// FORWARD DECLARATIONS
class CNcnMsvSessionObserverBase;
class CNcnCRHandler;
class CNcnSNNotifier;
class CNcnPublishAndSubscribeObserver;
class CNcnOutboxObserver;
class MNcnNotificationObserver;
class CNcnHandlerAudio;
class CMsgSimOperation;
class CNcnMsvSessionHandler;
class CVoiceMailManager;
class MNcnMsgWaitingManager;

// CLASS DECLARATION

/**
*  The main Ncn subsystem class
*/
class CNcnModelBase : public CActive
    {
    public:  // Constructors and destructor

		// Types of messages to keep count
        enum
            {
            EIndexMissedCalls = 0,
            EIndexNewEmails,
            EIndexUnreadMessages,            
            EIndexNewAudioMessages,
            EIndexLast
            };
    public:  // Constructors and destructor

        /**
        * Class factory.
        */
        static CNcnModelBase* NewL();

        /**
        * Destructor.
        */
        virtual ~CNcnModelBase();

    public: // New functions

        /**
        * The status of idle state is passed to the model through this method.
        * @param aCurrentState The current status of idle state (on/off).
        */
        void SetIdleState( const TBool aCurrentState );

        /**
        * Stores indicator status to file
        * @param aVariable
        * @param aState
        */
        void StoreIndicatorStatus( const TUid& aVariable, const TInt aState );

        /**
        * Notifies Publish and Subscribe.
        * @param aVariable
        * @param aState
        */
        void NotifyPublishAndSubscribe( const TUid& aVariable, const TInt aState );
        
        /**
        * Notifies Publish and Subscribe.
        * @param aCategory
        * @param aVariable
        * @param aState
        */
        void NotifyPublishAndSubscribe( const TUid& aCategory, const TUid& aVariable, const TInt aState );
        
        /**
        * Notifies Publish and Subscribe.
        * @param aCategory
        * @param aVariable
        * @param aState
        */
        void NotifyPublishAndSubscribe( const TUid& aCategory, const TUint aVariable, const TInt aState );

        /**
        * Stores indicator status to file and notifies Publish and Subscribe
        * @param aVariable
        * @param aState
        */
        void StoreIndicatorStatusAndNotifyPublishAndSubscribe( const TUid& aVariable, const TInt aState );

        /**
        * By this method the model is being told that the SMS service is present.
        */
        void SmsServicePresent( const TBool aSmsServicePresent );

        /**
        * Checks if the offered feature is supported.
        */
        TBool IsSupported( TUint aFeature ) const;

        /**
        * Stops playing of soundfile
        */
        void StopMsgReceivedTonePlaying();

        /**
        * Inform playing state to Shared Data
        */
        void MsgReceivedTonePlaying( TUint aAlertTonePlaying );

        /**
        * Turns on specified feature flag
        */
        void AddLocalVariationSupport( TUint aFeature );

        /**
        * Sends message to offline sender of new network status
        * @since Series60 2.6
        */
        void NotifyOfflineSenderL( const TInt& aNetworkBars );

        /**
        * Fetches given string for Uid
        * @since Series60 3.0
        */
        TInt GetCRString(
            const TUid&     aUid,
            const TUint32   aKey,
            TDes&           aValue ) const;

        /**
        * Fetches given P&S string for Uid
        * @since Series60 3.0
        */
        TInt GetPSString(
            const TUid&     aUid,
            const TUint32   aKey,
            TDes&           aValue ) const;
		
        /**
        * Fetches given integer value for Uid
        * @since Series60 3.0
        */
        TInt GetCRInteger(
            const TUid&     aUid,
            const TUint32   aKey,
            TInt&           aValue) const;

         /**
         * Sets new value for CR key.
         * @param aUid Uid to be used
         * @param aKey Which value is to be stored
         * @param aValue New value to be stored
         * @return System wide error. KErrNone when no errors.
         */
         TInt SetCRInt( const TUid& aUid,
                        const TUint32& aKey, 
                        TInt& aValue ) const;
                        
        /**
        * Plays the alert tone provided in parameter
        * @since Series60 2.6
        * @param aAlertTone Sound to be played
        * @return result of playing. KErrNone when no errors.
        */
        TInt PlayAlertToneL( const TUint aAlertTone ) const;

		/**
        * This method turns the SIM card support on/off. If this method
        * is never called, model will assume sim card is not supported.
        * @since Series60 2.8
        * @param aSimSupported Information regarding current SIM card
        * support status
        */
        void SetSimSupported( const TBool aSimSupported );

		/**
        * Get SIM status. Is SIM present or not
        * @since Series60 3.1
        * @return True if SIM is present
        */
		TBool IsSIMPresent();
		        
        /**
        * This method returns a reference to the notification API observer.
        * @return observer reference
        */
        MNcnNotificationObserver& NotificationObserver();
        
        /**
         * Returns a reference to Msv session handler.
         * @return A reference to Msv session handler.
         */
        CNcnMsvSessionHandler& MsvSessionHandler();
        
        /**
         * Returns a reference to Ncn UI.
         * @return A reference to Ncn UI.
         */
        MNcnUI& NcnUI();
        
        /**
         * Returns a reference to Ncn notifier.
         * @return A reference to Ncn notifier.
         */
         MNcnNotifier& NcnNotifier();
         
        /**
         * Returns a reference to Message Waiting Manager
         * @return A reference to Message Waiting Manager.
         */
         MNcnMsgWaitingManager& MsgWaitingManager();
     
        /**
         * Returns a reference to Voicemail Manager
         * @return A reference to Voicemail Manager.
         */   
         CVoiceMailManager& VoiceMailManager();
		 
        /**
         * Returns true if we are in boot phase
         * @return ETrue OR EFalse
         */ 
         TBool CheckIfBootPhase();
    public: // Virtual functions

        /**
        * Sim changed status is given by this method
        * @since Series60 2.6
        * @param aSimChanged Information of current SIM state
        */
        virtual void SetSimChanged( const TBool aSimChanged ) = 0;

        /**
        * By this method the model is being told about the current
        * SMS initialisation phase.
        * @since Series60 2.6
        * @param aSmsInitialisationPhase Current initialisation state
        */
        virtual void SetSmsInitialisationState(
            const TInt aSmsInitialisationPhase ) = 0;

        /**
        * This method performs the sim sms service centre fetching operation.
        * @since Series60 2.6
        */
        virtual void PerformSimServiceCentreFetching() = 0;

        /**
        * This method forces to call sms service centre fetching operation.
        * @since Series60 2.6
        */
        virtual void ForceToPerformSimServiceCentreFetching() = 0;

        /**
        * This method checks if the SMSC fetching is needed
        * @since Series60 2.6
        */
        virtual void CheckIfSimSCOperationRequired() = 0;

    protected:
        /**
        * C++ default constructor.
        */
        CNcnModelBase();

        /**
        * By default Symbian OS constructor is private.
        */
        virtual void ConstructL();

        /**
        * Cancels the request.
        */
        virtual void DoCancel();

        /**
        * Handles the event.
        */
        virtual void RunL();

		/**
		* Fetches the initial Sim support state from System Agent/
		* Publish and Subscribe before the actual SA/PS observer
		* is created
		*/
		virtual void FetchInitialSimSupportStateL();

    protected: // Virtual functions

        /**
        * Create Central repository connection
        * @since Series60 3.0
        */
        virtual void CreateCRConnectionL() = 0;        

    protected:

        // Pointer to Msv session handler. Owned.
        CNcnMsvSessionHandler* iMsvSessionHandler;
        
        // Pointer to Ncn ui instance. Owned.
        MNcnUI* iNcnUI;
        
        // Pointer to Ncn notifier instance. Owned.
        MNcnNotifier* iNcnNotifier;
   
   		//Manager for voice mail message handling
   		CVoiceMailManager* iVoiceMailManager;
   		
   		//Manager for handling message indicators and counts. 
   		MNcnMsgWaitingManager* iMsgWaitingManager;
        
        // A pointer to the CNcnMsvSessionObserver instance. Owned.
        CNcnMsvSessionObserverBase* iNcnMsvSessionObserver;
        // A pointer to the CNcnCRHandler instance. Owned.
		CNcnCRHandler* iNcnCRHandler;
        // A pointer to the CNcnOutboxObserver instance. Owned.
        CNcnOutboxObserver* iNcnOutboxObserver;
        // A pointer to the MNcnNotificationObserver instance. Owned.
        MNcnNotificationObserver* iNcnNotificationObserver;
		// A pointer to the CNcnPublishAndSubscribeObserver instance. Owned.
		CNcnPublishAndSubscribeObserver* iNcnPublishAndSubscribeObserver;
        // By this class we perform the sim service centre fetching. Owmed.
        CMsgSimOperation* iMsgSimOperation;
        // Tells the sms initialisation phase.
        TInt iSmsInitialisationPhase;
        // The Ncn status bits are stored into this unsigned integer.
        TUint iNcnStatusBits;
        // Featurelist for ncnlist
        TUint iNcnFeatureFlags;
        
	private:
	
		/**
        * Checks if SIM service center number should be refreshed in boot. 
        * @since Series60 3.0
        */
		void CheckIfSimSCShouldBeRead();        
        
    };

#endif      // NCNMODELBASE_H

// End of File
