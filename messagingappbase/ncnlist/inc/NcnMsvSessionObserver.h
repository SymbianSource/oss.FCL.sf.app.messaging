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
* Description:   Defines class CNcnMsvSessionObserver.
*
*/



#ifndef NCNMSVSESSIONOBSERVER_H
#define NCNMSVSESSIONOBSERVER_H

//  INCLUDES
#include "NcnMsvSessionObserverBase.h"

#include    <gsmupdu.h>

// FORWARD DECLARATIONS
class CMsvSession;
class CSmsMessage;
class CNcnNotifApiObserver;
class CVoiceMailManager;

// CLASS DECLARATION

/**
*  Receives and handles In-box events.
*/
class CNcnMsvSessionObserver : public CNcnMsvSessionObserverBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CNcnMsvSessionObserverBase* NewL( 
			CNcnModelBase* aModel, CVoiceMailManager& aVMManager  );

        /**
        * Destructor.
        */
        virtual ~CNcnMsvSessionObserver();

    public: // From MNcnMsvSessionHandlerObserver
    
        virtual void HandleMsvSessionReadyL( CMsvSession& aMsvSession );
        virtual void HandleMsvSessionClosedL();
        virtual void HandleMsvMediaChangedL( const TDriveNumber& aDriveNumber );
        
    public: // Functions from base classes

        /**
        * From MMsvEntryObserver. Callback function. Handles global In-box events.
        * This is a non-CDMA implementation of a pure virtual function declared
        * in base class CNcnMsvSessionObserverBase.
        * @param aEvent An entry event.
        */
        void HandleEntryEventL(
			TMsvEntryEvent	aEvent, 
			TAny*			aArg1, 
			TAny*			aArg2, 
			TAny*			aArg3);			

	protected:
		
		/**
        * By default Symbian OS constructor is private. This is protected because we derive it
        */
        virtual void ConstructL();
        
    private:

        enum TNcnInboxEntryType
            {
            EUnknown,
            ESmsMessage,
            EMMSMessage,
            EClass0SmsMessage,
            EDeliveryReport,
            EDeliveryErrReport,
            ESpecialMessage,
            EMessageWaitingStoreMessage,
            EMessageWaitingDiscardSetMessage,
            EMessageWaitingDiscardClearMessage,
            EDeletedMessage
            };
			
        /**
        * C++ default constructor.
        */
        CNcnMsvSessionObserver( 
            CNcnModelBase* aModel, CVoiceMailManager& aVMManager );
        
        /**
        * Checks the amount of unread messages in the global In-box.
        * This is a non-CDMA implementation of a pure virtual function
        * in base class CNcnMsvSessionObserverBase.
        */
        void CheckAmountOfUnreadMessagesInInboxL( TInt& aMsgCount, TInt& aAudioMsgCount );
        
        /**
         * Checks amount of unread (audio and other) messages in inbox and sets notifiers.
         */
        void UpdateInboxDataNotifiersL();

        /**
        * Checks an entry if it is a Class0SMS and shows it to the user.
        * Method takes ownership for the aEntryToHandlePtr
        * @param aEntryToHandlePtr A pointer to the entry to check
        * @return The type of the message.
        */
        TNcnInboxEntryType CheckAndHandleSmsEntryL( 
            CMsvEntry* aEntryToHandlePtr );

        /**
        * Checks if the sms is a message waiting type and triggers a notifier if it is
        * @param aEntry     The SMS entry to check to.
        * @param aHeader    Received SMS header
        * @return           The (NCN defined) type of the message
        */
        TNcnInboxEntryType CheckForMessageWaitingL( 
            CMsvEntry&      aEntry, 
            CSmsHeader&     aHeader );

        /**
        * Checks if the sms contains cphs message waiting indication information.
        * @param aEntry     The SMS entry to check to.
        * @param aHeader    Received SMS header
        * @return           The (NCN defined) type of the message
        */
        TNcnInboxEntryType CheckForCPHSIndicationBitsL( 
            CMsvEntry&      aEntry, 
            CSmsHeader&     aHeader );

		/**
        * Empties the sender address from a Cphs message.
        * @param aEntry     The SMS entry to check to.
        * @param aHeader    Received SMS header
        */
        void EmptyCphsMessageAddressL( 
            CMsvEntry&      aEntry, 
            CSmsHeader&     aHeader );

        /**
        * Check if message qualifies as CPHS message
        * @param    aOrigAddress    Originator address field (TP-OA)
        * @return   ETrue, if message contains CPHS indicator
        */
        TBool IsCphsMessage( 
            const TGsmSmsTelNumber& aOrigAddress ) const;

        /**
        * Checks the states from CPHS message
        * @param    aFirstOctet    First octet of CPHS message
        * @return   ETrue, if message is set message
        */
        TBool HandleCphsMessageL( const TUint aFirstOctet );

        /**
        * Gets the phone number from the message
        * @param aMessage Message from the number shall be taken
        * @return Phonenumber
        */
        TGsmSmsTelNumber GetTelNumber( const CSmsMessage& aMessage ) const;

        /**
        * Function to be used for launching the Message Centre with
        * given message, so that it's opened for the user.
        * @param aEntry     The SMS entry to check to.
        */
        void CheckAndLaunchMCEL( const TMsvEntry& aEntry );        
        
        /**
         * Handles new children.
         * @param aMsvEntrySelection Entry selection.
         */
        void HandleNewChildrenL( const CMsvEntrySelection& aMsvEntrySelection );
        
        /**
         * Handles changed children.
         * @param aMsvEntrySelection Entry selection.
         */
        void HandleChangedChildrenL( const CMsvEntrySelection& aMsvEntrySelection );
        
        /**
         * Handles deleted children.
         * @param aMsvEntrySelection Entry selection.
         */
        void HandleDeletedChildrenL( const CMsvEntrySelection& aMsvEntrySelection );
        
        /**
         * Displays the correct notification about new child entry.
         * @param aEntry The entry.
         * @param aEntryType The entry type.
         */
        void NotifyAboutNewChildEntryL( const TMsvEntry& aEntry, TNcnInboxEntryType aEntryType );
        
        /**
         * Handles the sms message user data.
         * @param aUserData Message user data.
         * @return Message entry type or EUnknown if type could not be determined.
         */
        TNcnInboxEntryType HandleSmsUserDataL( const CSmsUserData& aUserData );
        
        /**
         * Handles the special SMS message indication.
         * @param aMessageType The message type.
         * @param aMessagesWaiting The number of messages waiting.
         */
        void HandleSpecialSMSMessageIndicationL(
            TUint8 aMessageType,
            TUint8 aMessagesWaiting );
        
        /**
         * Handles the indication found in Data Coding Scheme.
         * @param aIndicationType The indication type.
         * @param aIndicationState The indication state.
         * @return ETrue if indication was set, EFalse otherwise.
         */    
        TBool HandleDCSIndicationL(
            TSmsDataCodingScheme::TSmsIndicationType aIndicationType,
            TSmsDataCodingScheme::TSmsIndicationState aIndicationState
            );           
         
         /**
          * Sets the amount of voice mails waiting.
          * @param aAmount The amount.
          */
         //void SetVoiceMailsWaiting( TUint aAmount );
         
         /**
          * Sets the amount of fax messages waiting.
          * @param aAmount The amount.
          */
         void SetFaxMessagesWaiting( TUint aAmount );
         
         /**
          * Sets the amount of email messages waiting.
          * @param aAmount The amount.
          */
         void SetEMailMessagesWaiting( TUint aAmount );
         
         /**
          * Sets the amount of other messages waiting.
          * @param aAmount The amount.
          */
         void SetOtherMessagesWaiting( TUint aAmount );

         /**
          * Check lock status. Do not open SMS messages automatically
          * if the phone is locked.
          * @return KErrNotFound or KSettingsAutolockStatus key code
          */
         TInt CheckAutoLockStatus();

         /**
          * Check if entry is read or unread. Uses iUnreadMessages array.
          * @return ETrue if entry is unread, else EFalse
          */         
         TBool IsUnread(const TMsvEntry& aEntry);
         
         /**
		  * This method is called only once during phone boot.
		  * Its purpose is to add all unread and visible messages
		  * in Inbox to iUnreadMessages array. Entries in iUnreadMessages
		  * are treated as new and unread eventhough
	 	  * message might have been read and marked again as unread
		  * before the boot.iUnreadMessages is used to determine
		  * which messages in Inbox are actually new unread messages
		  * and which are old unread messages. Soft note and indicator
		  * is not displayed to old unread messages except after the boot.  
		  */ 
         void InitUnreadMessagesArrayL();
                  
#ifdef _DEBUG
         /**
          * Prints the content of the TMsvEntry to logs
          * @param aEntryToHandle The object which info is outputted
          */
         void PrintEntryDebugInformation(const TMsvEntry& aEntryToHandle );
#endif
         
    private:    // Data

        // Senders phonenumber of latest received message
        TGsmSmsTelNumber iTelNumber; 
        
        //Message is notified when it's complete and visible
        //We keep track of unhandled messages here (SMS/MMS perhaps others :)
        RArray<TInt>*	iUnhandledMessages;
        
        //Session observer notifies Voice mail manager
        //when new voice mail messages are received
        CVoiceMailManager& iVMManager;       
        
        // iUnreadMessages contain ids of truly new and unread messages.
        // This array is needed to determine which entries in Inbox
        // are actually new unread messages and which entries are old
        // unread messages. Old unread messages are those that are read
        // at least once but are marked later on as unread in MCE. 
        // Softnote and indicator is not displayed for old unread messages
        // except after the boot. 
        //
        RArray<TInt>*	iUnreadMessages;        
        // After the first initialization only iUnhandledMessages array is cleared
        // when MsvSessionReady is received.
        TBool	iInitialized;
        // If voicemail entry is not deleted then bypass the Notification update so that removeentryl can delete it
        //Set to Invisible mode
        TBool iUpdateNotifier;
    };

#endif      // NCNMSVSESSIONOBSERVER_H

// End of File
