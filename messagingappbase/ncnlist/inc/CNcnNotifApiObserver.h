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
* Description:   Defines class CNcnNotifApiObserver.
*
*/



#ifndef CNCNNOTIFAPIOBSERVER_H
#define CNCNNOTIFAPIOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <msvapi.h>
#include "MNcnNotificationObserver.h"
#include "MNcnMsvSessionHandlerObserver.h"
#include "NcnBaseTimer.h"

// FORWARD DECLARATIONS
class CNcnModelBase;
class CMsvSession;
class CNcnTimer;

// CLASS DECLARATION

/**
* Observes if there are any messages in any of remote email boxes.
* This version supports NCN Notification API. 
*/
class CNcnNotifApiObserver : public CBase,
                             public MNcnNotificationObserver,
                             public MNcnMsvSessionHandlerObserver,
                             public MNcnTimer
    {      
            
    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CNcnNotifApiObserver* NewL( CNcnModelBase& aModel );

        /**
        * Destructor.
        */
        virtual ~CNcnNotifApiObserver();            
        
    public:     // New methods
        
        /**
         * Handles new email entries from msv. Updates the count
         * on appropriate mailbox.
         * @param aEntry EMail msv entry.
         */
        void HandleNewEMailMsvEntryL( const TMsvEntry& aEntry );
        
        /**
         * Handles deleted email entries from msv. Updates the count
         * on appropriate mailbox.
         * @param aEntry EMail msv entry id.
         */
        void HandleDeletedEMailMsvEntryL( const TMsvId& aEntryId );
        
    private:    // From MNcnTimer
    
        void NcnTimerCompleted();

    private:    // From MNcnMsvSessionHandlerObserver
    
        void HandleMsvSessionReadyL( CMsvSession& aMsvSession );
        void HandleMsvSessionClosedL();
        void HandleMsvEntryCreatedL( const TMsvId& aMsvId );
        void HandleMsvEntryDeletedL( const TMsvId& aMsvId );
        void HandleMsvMediaChangedL( const TDriveNumber& aDriveNumber );    
        void HandleMsvMediaAvailableL( const TDriveNumber& aDriveNumber );
        void HandleMsvMediaUnavailableL();
      
    private:    // From MNcnNoficationObserver
    
        void HandleNewInternalMessagesL(
                        const TNcnNotifMessageType aType );

        void HandleNewInternalMessagesL(
                        const TNcnNotifMessageType aType,
                        const TMsvId& aMailBox,
                        const MDesCArray& aInfo );
                        
        void HandleInternalMarkUnreadL(
                        const TNcnUnreadRequestType aRequest );

        void HandleInternalMarkUnreadL(
                        const TNcnUnreadRequestType aRequest,
                        const TMsvId& aMailbox );

        void HandleNewMessagesL(
                        const TMsvId& aMailBox,
                        const MNcnNotification::TIndicationType aIndicationType,
                        const MDesCArray& aInfo );

        void HandleMarkUnreadL( const TMsvId& aMailBox );                        
                        
    private:    // New functions
                
        /**
         * Returns ETrue if entry is an email entry.
         */
        TBool IsEMailEntry( const TMsvEntry& aEntry );
        
        /**
         * Internal helper class for new mail count book-keeping.
         * Used to allow storing of mailbox status in an array.
         */
        class TNcnMailBoxStatus
            { 
            public :
            
                TNcnMailBoxStatus( TMsvId aMailBox );
                
                static TInt Compare( const TNcnMailBoxStatus& aFirst, 
                                     const TNcnMailBoxStatus& aSecond );
                                     
                static TBool Match( const TNcnMailBoxStatus& aFirst, 
                                    const TNcnMailBoxStatus& Second );
                                    
            public:
            
                /**
                 * MsvId of the mailbox.
                 */
                TMsvId iMailBox;                

                /**
                 * MTM of the mailbox. Eg. Pop3, Imap etc.
                 */
                TUid iMTMType;    
 
                 /**
                 * Technology type of the mailbox.
                 */
                TUid iMailBoxTechnologyType;  
                                               
                /**
                 * Amount of new emails at the time of the last NewMessages call
                 * on mailbox.
                 */
                TInt iPublishedNewEmailCount;
                
                /**
                 * Highest email msv id so far in mailbox.
                 */
                TMsvId iHighestEMailMsvId;

                /**
                 * Highest IMAP uid id so far in mailbox.
                 */
                TUint32 iHighestIMAPId;
                
                 /**
                 * Time when the latest message has arrived to this inbox
                 */
                TTime iLatestMessageArrival;
                                                               
                /**
                 * Highest MsvId at the time of the last MarkUnread call
                 * on mailbox.
                 */
                TMsvId iUnreadCheckpointMsvId;
                                
                /**
                 * Highest MsvId at the time of the last NewMessages call
                 * on mailbox.
                 */
                TMsvId iPublishedCheckpointMsvId;
                
                 /**
                 * Highest IMAP uid at the time of the last NewMessages call
                 * on mailbox. This is IMAP specific variable. The IMAP
                 * messages come to the terminal in ID order.
                 * In boot-up this is set to highest IMAP id in the box
                 */
                TUint32 iPublishedCheckpointIMAPId;

                /**
                 * Timestamp at the time of the last NewMessages call
                 * on mailbox. This is used for non IMAP messages (POP, SyncMl etc.)
                 * In boot-up this is set to latest time stamp in the box
                 */
                TTime iPublishedCheckpointTimeStamp;      
                         
                /**
                 * Array containing the id's of messages considered new
                 * in the box.
                 */
                RArray<TMsvId> iNewMessageIds;
                
                /**
	             * Refresh request status
	             */
	            TBool iRefreshRequested;
	                            
                /**
                 * Notification status.
                 */
                TBool iNotified;
                
                /**
                 * Mail boxes can use specific notification parameters
                 * If TRUE notification by icon is required. Default TRUE
                 */
		    	TBool iIcon;

                /**
                 * Mail boxes can use specific notification parameters
                 * If TRUE notification by tone is required. Default TRUE
                 */
		    	TBool iTone;
		    	
                /**
                 * Mail boxes can use specific notification parameters
                 * If TRUE notification by note is required. Default TRUE
                 */
		    	TBool iNote;
		    	
		    	/** Mail box will request for show icon if iShowIcon is true.
		    	 *  Default value will be True
		    	 *  This flag will be set to true if new mail is received.
		    	 *  This flag will be set to flase, if mce sends mailbox oened
		    	 *  event.
		    	 *  If  iShowIcon is true
		    	 *  than  mailbox will request for icon notification
		    	 *  and iShowIcon is False
		    	 *  Than  mailbox will not request for icon notification, 
		    	 *  because mailbox is opened by user in MCE. Mce inform
		    	 *  opening of mailbox event to respective mailbox in ncn list.
		    	 */
		    	TBool iShowIcon;
            };
    
        /**
         * Loads all registered mail boxes to status array.
         * @param aMsvSession Current msv session.
         */
        void LoadMailboxesL( CMsvSession& aMsvSession );
        
        /**
         * Returns ETrue if technology type is a mail technology type.
         * @return ETrue if technology type is a mail technology type.
         */
        TBool IsMailTechnologyType( const TUid& aTechnologyType );
        
        /**
         * Adds mailboxes of given mtm type to iMailBoxStatusArray.
         * @param aMtmType MTM of the mailbox.
         * @param aTechnologyType Technology type of the mailbox.
         * @param aMsvSession Current msv session.
         */
        void AddBoxesToStatusArrayL( const TUid& aMtmType, const TUid& aTechnologyType, CMsvSession& aMsvSession );
        
        /**
         * Adds a mail box to status array.
         * @param aMsvId MsvId of the mail box.
         * @param aMtmType MTM of the mailbox.
         * @param aTechnologyType Technology type of the mail box.
         */
        void AddMailBoxL( const TMsvId& aMsvId, const TUid& aMtmType, const TUid& aTechnologyType );
        
        /**
         * Removes a mail box to status array.
         * @param aMsvId MsvId of the mail box.
         * @param aMtm Mtm of the mail box.
         */
        void RemoveMailBox( const TMsvId& aMsvId );
        
        /**
         * Updates the amount of total new emails based on iMailBoxStatusArray.
         * Note: This method does not query message server.
         */
        void UpdateTotalNewEmails();                            
        
        /**
         * Updates the notification to reflect the current state of
         * new email count. Update is not done if change has not happened
         * @param aForceUpdate Forces the update of the notification
  		 * @param aIcon Notifies with icon
		 * @param aTone Notifies with tone
		 * @param aNote Notifies with note
         */
        void UpdateNotification( 	TBool aForceUpdate = EFalse,
							        TBool aIcon = ETrue,
							        TBool aTone = ETrue,
							        TBool aNote = ETrue);
        
        /**
         * Returns the Mailbox with specified id. Leaves with KErrNotFound if no
         * such mailbox is found (in local structure).
         * @param aId Mailbox id.
         */
        TNcnMailBoxStatus& MailBoxStatusL( const TMsvId& aId );
        
        /**
         * Checks the mailbox's "new email indicator" setting.
         * Setting is only available in IMAP/POP/SyncMl boxes
         * and not in 3rd party boxes.
         * @param aMailboxStatus the queried mail box
         * @Leave KErrNotSupported if the mail box does not support this setting
         * @return ETrue if "new email indicator" setting is on
         */
        TBool NewEmailIndicatorsSetL( const TNcnMailBoxStatus& aMailboxStatus );        
        
        /**
         * Returns message server entry with specified service id.
         *
         * Method leaves with KErrNotSupported if supplied id does not
         * point to a service entry.
         *
         * @param aServiceId Service id.
         * @return Message server entry.
         */
        CMsvEntry* ServiceEntryL( const TMsvId& aServiceId );
        
        /**
         * Returns message server entry with specified service id and
         * puts it into the cleanup stack.
         *
         * Method leaves with KErrNotSupported if supplied id does not
         * point to a service entry.
         *
         * @param aServiceId Service id.
         * @return Message server entry.
         */
        CMsvEntry* ServiceEntryLC( const TMsvId& aServiceId );
        
        /**
         * Marks the mailbox unread. This resets the amount of new
         * emails, both released and actual.
         * @param aId Id of the mailbox.
         */
        void MarkUnreadL( const TMsvId& aId );
        
        /**
         * Marks all mailboxes unread.
         */
        void MarkAllUnread( );
        
        /**
         * Publishes mailboxes new email amount. All new emails that
         * have not been published are added to the amount of
         * new emails. Note that this method does not update the notification,
         * use UpdateNotification.
         * @param aId Id of the mailbox.
         */
        void PublishNewMessagesL( const TMsvId& aId );
        
        /**
         * Publishes new email amounts in all mailboxes.
         * Note that this method does not update the notification,
         * use UpdateNotification.
         */
        void PublishAllNewMessages( );

        /**
         * Browse through the mailbox's messages and find highest
         * MsvId, ImapId and latest time stamp that messages have.
         * Defaults values to 0.
		 * @param aId Id of the mailbox.
	 	 * @param aHighestMsvId Highest message MsvId that box contains
		 * @param aHighestImapId Highest message ImapId that box contains
		 * @param aLatestTimeStamp Latest message time stamp that box contains
         */
        void FindHighest_MsvId_ImapId_LatestTime(	const TMsvId& aId,
													TMsvId& aHighestMsvId,
													TUint32& aHighestImapId,
													TTime& aLatestTimeStamp );
        
        /**
         * Browse through the mailbox's messages and find highest
         * MsvId, ImapId and latest time stamp that messages have.
         * Values should be 0 when the method is used.
		 * @param aId Id of the mailbox.
	 	 * @param aHighestMsvId Highest message MsvId that box contains
		 * @param aHighestImapId Highest message ImapId that box contains
		 * @param aLatestTimeStamp Latest message time stamp that box contains
         */
        void FindHighest_MsvId_ImapId_LatestTimeL(	const TMsvId& aId,
													TMsvId& aHighestMsvId,
													TUint32& aHighestImapId,
													TTime& aLatestTimeStamp );
          
        /**
         * Returns mail boxes MTM and Technolgy type
         * @param aId Id of the entry.
         * @param aMtmType The MTM type
         * @param aTechnologyType The technolgy type
         * @return System wide error code
         */
        TInt GetMailBoxesAttributesL( const TMsvId& aMsvId, TUid& aMtmType, TUid& aTechnologyType );
        
        /**
         * Makes a check for this entry and determines if it requires notification.
         * Phone keeps a certain amount of messages in the terminal and when one of
         * those messages is deleted it retrieves another one from the mail box.
         * Sometimes it retrieves a message that has already been notified so it
         * requires no notification. In this method we try to identify those messages.
         * In IMAP boxes the messages have a special id, this is in increasing order.
         * We know the highest id at the time of notification and do not notify of messages
         * that have lower id. In POP, SyncMl etc. boxes we do this same comparison using
         * time stamps. We only notify messages that have newer timestamp than last notification.
         * Timestamp comparison must be done using UTC time because of the time zones.
         * @param aMailbox The mailbox that contains this message
         * @param aEntry The entry it self.
         * @return True if notification is required
         */        
        TBool IsNotificationNeededForThisMessageL( const TNcnMailBoxStatus& aMailbox, const TMsvEntry& aEntry );

        /**
         * Takes the mailbox and determines the used notifications for it.
         * 3rs party mailboxes give the notifications in the API, but IMAP
         * POP and SynvMl do not. For S60 mailbox's ncnlist will read
         * "new mail indicator" setting and determines the used notification
         * based on the settings value.
         * @param aMailbox updated mailbox status entity
         * @leave KErrNotSupported given mail box is not IMAP/POP/SyncMl
         * @leave System wide error code
         */
        void UpdateS60MailBoxNotificationAttributesL( TNcnMailBoxStatus& aMailbox );
        
        /**
         * A small helper method. Takes the desired indicators as TIndicationType
         * and converts it to separate booleans for tone, note and icon
         * Method updates the values to the mailbox
         * @param aMailbox updated mailbox status entity
         * @param aIndicationType The desired indication type
         */
        void UpdateMailBoxesNotifications( TNcnMailBoxStatus& aMailbox, const TInt& aIndicationType );
             
    private:    // Constructors and destructor

        /**
         * C++ default constructor.
         */
        CNcnNotifApiObserver( CNcnModelBase& aModel );

        /**
         * By default Symbian OS constructor is private.
         */
        void ConstructL();
        
        /**
         * 	Returns ETrue, if aEntry is in SyncML boxes Sent folder
         */
        TBool IsSyncMLEntryInSentFolderL( const TMsvEntry& aEntry  );

    private:    // Data
    
        // Array containing mailbox 1ids and number of new mails in each of them.
        RArray<TNcnMailBoxStatus> iMailBoxStatusArray;
        
        // Sum of all mailboxes' new mail counts
        TInt iTotalNewMailCount;
        
        // Last notified new mail count
        TInt iNotifiedNewMailCount;                        
        
        // Not own, pointer to the event monitor object.
        CNcnModelBase& iModel;
        
        // Timer instance. Owned.
        CNcnTimer* iTimer;        
        
        //Reference to msv session. Not owned
        CMsvSession* iMsvSession;
    };

#endif      // CNCNNOTIFAPIOBSERVER_H

// End of File
