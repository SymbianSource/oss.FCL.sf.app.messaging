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
* Description:   The purpose of this class is to observe if there are messages
*                in the Email.
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <MuiuMsvUiServiceUtilities.h>  // MsvUiServiceUtilities
#include <msvstd.hrh>
#include <mtclreg.h>                    // For CClientMtmRegistry
#include <miuthdr.h>
#include <ImumInternalApi.h>            // CImumInternalApi
#include <ImumInSettingsKeys.h>         // TImumInSettings
#include <ImumInSettingsData.h>         // CImumSettingsData

#include <muiuemailtools.h>
#include <SendUiConsts.h>   // KSenduiMtmSyncMLEmailUid 

#include "CNcnNotifApiObserver.h"
#include "NcnModelBase.h"
#include "CNcnMsvSessionHandler.h"
#include "NcnTimer.h"
#include <NcnListInternalCRKeys.h>
#include <muiumsvuiserviceutilitiesinternal.h> // Messaging utilites
#include <messaginginternalcrkeys.h>


// ================= LOCAL CONSTANTS =======================
namespace
    {
    // Timer iteration delay in microseconds
    const TUint KTimerIterationDelay = 1 * 1000 * 1000;
    
    // Mail technology type
    const TUid KMailTechnologyTypeUid = { 0x10001671 };
    }

// ======================== INTERNAL HELPER CLASS ==============================

CNcnNotifApiObserver::TNcnMailBoxStatus::TNcnMailBoxStatus( 
    TMsvId aMailBox )
    : iMailBox( aMailBox ),
      iMTMType( KNullUid ),
      iMailBoxTechnologyType( KNullUid),
      iPublishedNewEmailCount( 0 ),
      iHighestEMailMsvId( 0 ),
      iHighestIMAPId(0),
      iLatestMessageArrival(0),
      iUnreadCheckpointMsvId( 0 ),
      iPublishedCheckpointMsvId( 0 ),
      iPublishedCheckpointIMAPId(0),
      iPublishedCheckpointTimeStamp(0),
      iNewMessageIds( 5 ),
      iNotified( ETrue ),
      iIcon(ETrue),
      iTone(ETrue),
      iNote(ETrue),
      iShowIcon(ETrue)
    {
    }
                
TInt CNcnNotifApiObserver::TNcnMailBoxStatus::Compare(
    const TNcnMailBoxStatus& aFirst, 
    const TNcnMailBoxStatus& aSecond )
    {
    if ( aFirst.iMailBox < aSecond.iMailBox )
        {
        return -1;
        }        
    else if ( aFirst.iMailBox > aSecond.iMailBox )
        {
        return 1;
        }
    else
        {
        return 0;
        }                        
    }
                                     
TBool CNcnNotifApiObserver::TNcnMailBoxStatus::Match( 
    const TNcnMailBoxStatus& aFirst, 
    const TNcnMailBoxStatus& aSecond )
    {
    TBool ret = EFalse;
    
    if( aFirst.iMailBox == aSecond.iMailBox )
        {
        ret = ETrue;
        }
    
    return ret;
    }

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CNcnNotifApiObserver default constructor
// ---------------------------------------------------------
//
CNcnNotifApiObserver::CNcnNotifApiObserver( 
    CNcnModelBase& aModel )
    : iModel( aModel ),
      iMsvSession(NULL)
    {    
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::ConstructL
// ---------------------------------------------------------
//
void CNcnNotifApiObserver::ConstructL()
    {
    NCN_RDEBUG( _L("CNcnNotifApiObserver::ConstructL - Entry") );    

    // listen to msv session handler
    iModel.MsvSessionHandler().AddObserverL( this );
    
    NCN_RDEBUG( _L("CNcnNotifApiObserver::ConstructL - Exit") );
    }


// ---------------------------------------------------------
// CNcnNotifApiObserver::NewL
// ---------------------------------------------------------
//
CNcnNotifApiObserver* CNcnNotifApiObserver::NewL( 
    CNcnModelBase& aModel )
    {
    // Create the observer instance
    CNcnNotifApiObserver* self = 
        new (ELeave) CNcnNotifApiObserver( aModel );

    // Call the construct safely
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
        
    return self;
    }


// ---------------------------------------------------------
// CNcnNotifApiObserver::~CNcnNotifApiObserver
// ---------------------------------------------------------
//
CNcnNotifApiObserver::~CNcnNotifApiObserver()
    {
    delete iTimer;
    
    iMailBoxStatusArray.Close();

    // stop listening to msv session handler
    iModel.MsvSessionHandler().RemoveObserver( this );
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::HandleMsvSessionReadyL
// ---------------------------------------------------------
//
void CNcnNotifApiObserver::HandleMsvSessionReadyL( CMsvSession& aMsvSession )
    {
    // Load all mailboxes
    LoadMailboxesL( aMsvSession );
    
    //We take a pointer to msv session. Needed only in special circumstances
    iMsvSession = &aMsvSession;
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::HandleMsvSessionClosedL
// ---------------------------------------------------------
//    
void CNcnNotifApiObserver::HandleMsvSessionClosedL()
    {
    NCN_RDEBUG( _L("CNcnNotifApiObserver::HandleMsvSessionClosedL") );
    iMailBoxStatusArray.Close();
    
    // Reset email counters
    iTotalNewMailCount = 0;
    iNotifiedNewMailCount = 0;    
                                
    // Inform notifier
    iModel.NcnNotifier().SetNotification(
        MNcnNotifier::ENcnEmailNotification,
        iTotalNewMailCount );
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::HandleMsvEntryCreatedL
// ---------------------------------------------------------
//
void CNcnNotifApiObserver::HandleMsvEntryCreatedL( const TMsvId& aMsvId )
    {
    //Get the atrrbiutes
    TUid aMtmType = KNullUid;
    TUid aTechnologyType = KNullUid;
    TInt error = GetMailBoxesAttributesL(aMsvId, aMtmType, aTechnologyType);
    
    // if entry is an email service entry
    if( error == KErrNone && IsMailTechnologyType( aTechnologyType ) )
        {
        // add mail box
        NCN_RDEBUG_INT2( _L("CNcnNotifApiObserver::HandleMsvEntryCreatedL - Box %d created with MTM %d"), aMsvId, aMtmType.iUid );
        AddMailBoxL( aMsvId, aMtmType, aTechnologyType );
        }
    // otherwise inspect the entry more carefully
    else
        {                
        CMsvSession& msvSession = iModel.MsvSessionHandler().MsvSessionL();
    
        // get service id and entry
        TMsvId serviceId;
        TMsvEntry entry;        
        TInt err = msvSession.GetEntry( aMsvId, serviceId, entry );
        
        // if entry was found
        if( err == KErrNone )
            {
            // if entry is an email
            if( IsEMailEntry( entry ) && ( entry.New() && entry.Unread() ) )
                {
                // handle new email
                HandleNewEMailMsvEntryL( entry );
                }    
            }            
        }
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::HandleMsvEntryDeletedL
// ---------------------------------------------------------
//    
void CNcnNotifApiObserver::HandleMsvEntryDeletedL( const TMsvId& aMsvId )
    {
    // just try to remove mail box
    // invalid ids are ignored by the method
    RemoveMailBox( aMsvId );
    
    // Also try to delete email entry
    HandleDeletedEMailMsvEntryL( aMsvId );
    }
    
// ---------------------------------------------------------
// CNcnNotifApiObserver::HandleNewEMailMsvEntryL
// ---------------------------------------------------------
//
void CNcnNotifApiObserver::HandleNewEMailMsvEntryL( const TMsvEntry& aEntry )
    {
    NCN_RDEBUG_INT( _L("[ncnlist] CNcnNotifApiObserver::HandleNewEMailMsvEntryL - entry = %d\n"), aEntry.Id() );    
    
    // find the mailbox
    TNcnMailBoxStatus& mailboxStatus = MailBoxStatusL( aEntry.iServiceId );
    
    // find entry in mailbox new email list
    TInt index = mailboxStatus.iNewMessageIds.Find( aEntry.Id() );
  
    // if there is no such entry in list yet 
    if( index == KErrNotFound )
        {
        //Some messages do not require notifications when they arrive (they are old messages)
        //In this method those messages are spotted
        if ( IsNotificationNeededForThisMessageL( mailboxStatus, aEntry ) == FALSE )
	        {
	        NCN_RDEBUG_INT( _L( "[ncnlist] CNcnNotifApiObserver::HandleNewEMailMsvEntryL - entry %d does not require notification!" ), aEntry.Id() );
			return;
	        }
	        
        //Notification is needed, add it to our list
        NCN_RDEBUG_INT( _L( "[ncnlist] CNcnNotifApiObserver::HandleNewEMailMsvEntryL - entry %d added to list\n" ), aEntry.Id() );
        
        // append entry id to the list of new messages in box
        mailboxStatus.iNewMessageIds.Append( aEntry.Id() );
        
        //Email icon should be shown to indicate new email for this mailbox
        mailboxStatus.iShowIcon = ETrue;
        
        NCN_RDEBUG_INT2(
            _L( "CNcnNotifApiObserver::HandleNewEMailMsvEntryL - new messages in box %d: %d" ),
            mailboxStatus.iMailBox,
            mailboxStatus.iNewMessageIds.Count() );
        
        // update highest msv entry if needed
        if( aEntry.Id() > mailboxStatus.iHighestEMailMsvId )
            {
            mailboxStatus.iHighestEMailMsvId = aEntry.Id();
            }
            
        // update highest IMAP uid if needed
        NCN_RDEBUG(_L( "CNcnNotifApiObserver::HandleNewEMailMsvEntryL - Making an TMsvEmailEntry from TEntry" ) );
        TMsvEmailEntry emailEntry(aEntry);
		NCN_RDEBUG_INT(_L( "CNcnNotifApiObserver::HandleNewEMailMsvEntryL - Messages IMAP UID %d" ), emailEntry.UID() );
        if( emailEntry.UID() > mailboxStatus.iHighestIMAPId  )
            {
            NCN_RDEBUG_INT( _L( "CNcnNotifApiObserver::HandleNewEMailMsvEntryL - Message has highest IMAP UID so far %d" ), emailEntry.UID() );
            mailboxStatus.iHighestIMAPId = emailEntry.UID();            	
            }
            
        // update latest message time if needed
#ifdef _DEBUG
		TBuf<50> 	tempTime;
		_LIT(KBMDebugTimeFormat, "%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B");		

		aEntry.iDate.FormatL(tempTime, KBMDebugTimeFormat);
		NCN_RDEBUG_INT( _L("CNcnNotifApiObserver::HandleNewEMailMsvEntryL messages time stamp: %S"), &tempTime);
#endif
        if( aEntry.iDate > mailboxStatus.iLatestMessageArrival )
            {
            NCN_RDEBUG( _L( "CNcnNotifApiObserver::HandleNewEMailMsvEntryL - Message has latest arrival time" ) );
            mailboxStatus.iLatestMessageArrival = aEntry.iDate;
            }
            
        }
    // entry exists in the list
    else
        {
        NCN_RDEBUG_INT( _L( "[ncnlist] CNcnNotifApiObserver::HandleNewEMailMsvEntryL - entry %d already in list\n" ), aEntry.Id() );
        }
    }
    
// ---------------------------------------------------------
// CNcnNotifApiObserver::HandleDeletedEMailMsvEntryL
// ---------------------------------------------------------
//
void CNcnNotifApiObserver::HandleDeletedEMailMsvEntryL( const TMsvId& aEntryId )
    {
    NCN_RDEBUG( _L( "CNcnNotifApiObserver::HandleDeletedEMailMsvEntryL - deleted email entry" ) );
    
    TInt count( iMailBoxStatusArray.Count() );

    // Run all boxes through
    while( count > 0)
        {
        // decrease count
        count--;
        
        // get reference to mailbox
        TNcnMailBoxStatus& mailboxStatus = iMailBoxStatusArray[count];
        
        // try to find the id in boxes new message id list
        TInt index = mailboxStatus.iNewMessageIds.Find( aEntryId );
        
        // if entry was found
        if( index != KErrNotFound )
            {
            // remove entry id
            mailboxStatus.iNewMessageIds.Remove( index );
            
            NCN_RDEBUG_INT2(
                _L( "CNcnNotifApiObserver::HandleDeletedEMailMsvEntryL - new messages in box %d: %d" ),
                mailboxStatus.iMailBox,
                mailboxStatus.iNewMessageIds.Count() );
            
            // and if id is less than equal to last published it
            // affects the amount of published new emails
            if( aEntryId <= mailboxStatus.iPublishedCheckpointMsvId )
                {
                // decrease amount of published new emails
                mailboxStatus.iPublishedNewEmailCount--;
                
                NCN_RDEBUG_INT2(
                    _L( "CNcnNotifApiObserver::HandleDeletedEMailMsvEntryL - published new messages in box %d: %d" ),
                    mailboxStatus.iMailBox,
                    mailboxStatus.iPublishedNewEmailCount );
                
                // update notification, it might not be valid anymore if
                // published emails have been deleted
                UpdateNotification( EFalse, mailboxStatus.iIcon, mailboxStatus.iTone, mailboxStatus.iNote );
                }

           // and finally we must update the boxes highest values, they might have changed			
			NCN_RDEBUG( _L( "CNcnNotifApiObserver::HandleDeletedEMailMsvEntryL - Updating new 'highest' values" ));                     
            FindHighest_MsvId_ImapId_LatestTime(mailboxStatus.iMailBox,
            									mailboxStatus.iHighestEMailMsvId ,
            									mailboxStatus.iHighestIMAPId,
            									mailboxStatus.iLatestMessageArrival);       
            }
        } 
    }
    
// ---------------------------------------------------------
// CNcnNotifApiObserver::HandleMsvMediaChangedL
// ---------------------------------------------------------
//    
#ifdef _DEBUG
void CNcnNotifApiObserver::HandleMsvMediaChangedL( const TDriveNumber& aDriveNumber )
#else
void CNcnNotifApiObserver::HandleMsvMediaChangedL( const TDriveNumber& /*aDriveNumber */)
#endif
	{
    //The entrys that we have in the iMailBoxStatusArray are no longer valid
    //the array must be rebuild.   
    NCN_RDEBUG_INT( _L("[ncnlist] CNcnNotifApiObserver::HandleMsvMediaChangedL - Media changed to drive: %d"), aDriveNumber );
    
    //Reset the mail box array
    iMailBoxStatusArray.Reset();
    
    // Reset email counters
    iTotalNewMailCount = 0;
    iNotifiedNewMailCount = 0;    
                                
    // Inform notifier
    iModel.NcnNotifier().SetNotification( MNcnNotifier::ENcnEmailNotification, iTotalNewMailCount );
        
    //Load the mail boxes. Old MsvSession is still valid after media changes
    if( iMsvSession != NULL )
	    {
	    LoadMailboxesL( *iMsvSession );	
	    }
	}

// ---------------------------------------------------------
// CNcnNotifApiObserver::HandleMsvMediaAvailableL
// ---------------------------------------------------------
//    
#ifdef _DEBUG
void CNcnNotifApiObserver::HandleMsvMediaAvailableL( const TDriveNumber& aDriveNumber )
#else
void CNcnNotifApiObserver::HandleMsvMediaAvailableL( const TDriveNumber& /*aDriveNumber*/ )
#endif
	{
	//Clean up message store from unhealthy mail boxes
    NCN_RDEBUG_INT( _L("[ncnlist] CNcnNotifApiObserver::HandleMsvMediaAvailableL - Media available in drive: %d"), aDriveNumber );

    // Create interface object
    CIMAEmailApi* api = CreateEmailApiLC( iMsvSession );
    
    // Force message store cleanup
    NCN_RDEBUG( _L("[ncnlist] CNcnNotifApiObserver::HandleMsvMediaAvailableL - Cleanup unhealthy mailboxes") );
    api->HealthServicesL().CleanupUnhealthyMailboxes();
    CleanupStack::PopAndDestroy( api );
    api = NULL;

	}

// ---------------------------------------------------------
// CNcnNotifApiObserver::HandleMsvMediaAvailableL
// Media Unavailable Check if current drive is Not C 
// then set it back to Phone memory.
// ---------------------------------------------------------
//    

void CNcnNotifApiObserver::HandleMsvMediaUnavailableL( )
	{
		
		NCN_RDEBUG( _L("[ncnlist] CNcnNotifApiObserver::HandleMsvMediaUnavailable - Media Unavailable") );

		// get session
		CMsvSession& msvSession = iModel.MsvSessionHandler().MsvSessionL(); 
		NCN_RDEBUG( _L( "CNcnMsvSessionHandler::HandleSessionEventL - EMsvMediaUnavailable, Check Current message store drive." ));
		TInt CurrentDrive = TInt(msvSession.CurrentDriveL());
		RFs fs;	
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		TDriveNumber SystemDrive = fs.GetSystemDrive();	
		CleanupStack::PopAndDestroy();//fs
		if(CurrentDrive != SystemDrive)
		   MsvUiServiceUtilitiesInternal::ChangeMessageStoreToPhoneL(*iMsvSession);
	}

// ---------------------------------------------------------
// CNcnNotifApiObserver::MarkUnread
// ---------------------------------------------------------
//
void CNcnNotifApiObserver::MarkUnreadL( const TMsvId& aId )
    {
    NCN_RDEBUG_INT( _L("[ncnlist] CNcnNotifApiObserver::MarkUnreadL - box %d marked unread."), aId );
    
    // find the mailbox
    TNcnMailBoxStatus& mailboxStatus = MailBoxStatusL( aId );
        
    // reset amounts    
    mailboxStatus.iPublishedNewEmailCount = 0;
    
    // clear new message ids
    mailboxStatus.iNewMessageIds.Reset();
    mailboxStatus.iNewMessageIds.Compress();
    
    // move unread checkpoint to the highest id
    mailboxStatus.iUnreadCheckpointMsvId = mailboxStatus.iHighestEMailMsvId;
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::PublishNewMessages
// ---------------------------------------------------------
//    
void CNcnNotifApiObserver::PublishNewMessagesL( const TMsvId& aId )
    {
    // find the mailbox
    TNcnMailBoxStatus& mailboxStatus = MailBoxStatusL( aId );
    // clear notification flag
    mailboxStatus.iNotified = EFalse;
    mailboxStatus.iRefreshRequested = ETrue;
        
    NCN_RDEBUG_INT( _L("[ncnlist] CNcnNotifApiObserver::PublishNewMessagesL Initializing counter for box %d."),
        mailboxStatus.iMailBox );
    
    // if timer is not already spawned
    if( !iTimer )
        {
        iTimer = CNcnTimer::NewL( *this );
        iTimer->After( KTimerIterationDelay );
        }
    else
    	{
    	iTimer->Cancel();
    	iTimer->After( KTimerIterationDelay );
    	}
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::SpawnTimer
// ---------------------------------------------------------
//    
void CNcnNotifApiObserver::NcnTimerCompleted()
    {
    TInt count( iMailBoxStatusArray.Count() );
    
    NCN_RDEBUG( _L("[ncnlist] CNcnNotifApiObserver::NcnTimerCompleted called." ) );
    
    // Run all boxes through
    while( count-- )
        {        
        // get mailbox status
        TNcnMailBoxStatus& mailboxStatus = iMailBoxStatusArray[count];
        
        if ( mailboxStatus.iRefreshRequested )
        	{
        	NCN_RDEBUG_INT( _L("[ncnlist] CNcnNotifApiObserver::NcnTimerCompleted Refresh requested for box %d "),
        	            mailboxStatus.iMailBox );
        	        
        	// if published count differs from number of new emails
	        if( mailboxStatus.iPublishedNewEmailCount != mailboxStatus.iNewMessageIds.Count() )
	            {
	            // all new messages are now published
	            mailboxStatus.iPublishedNewEmailCount = mailboxStatus.iNewMessageIds.Count();                                            
	            
	            NCN_RDEBUG_INT( _L("[ncnlist] CNcnNotifApiObserver::NcnTimerCompleted published updated to %d."),
	                mailboxStatus.iPublishedNewEmailCount );
	        
	            // move published checkpoints to new values
	           	mailboxStatus.iPublishedCheckpointMsvId = mailboxStatus.iHighestEMailMsvId;
	            mailboxStatus.iPublishedCheckpointIMAPId = mailboxStatus.iHighestIMAPId;
				mailboxStatus.iPublishedCheckpointTimeStamp = mailboxStatus.iLatestMessageArrival;
				NCN_RDEBUG_INT( _L("[ncnlist] CNcnNotifApiObserver::NcnTimerCompleted highest published iHighestEMailMsvId %d"),mailboxStatus.iHighestEMailMsvId);
				NCN_RDEBUG_INT( _L("[ncnlist] CNcnNotifApiObserver::NcnTimerCompleted highest published iHighestIMAPId %d"),mailboxStatus.iHighestIMAPId);
				NCN_RDEBUG_INT( _L("[ncnlist] CNcnNotifApiObserver::NcnTimerCompleted highest published iLatestMessageArrival(Int66) %d"),mailboxStatus.iLatestMessageArrival.Int64() );
								                                
	            // if box has not yet been notified of
	            if( !mailboxStatus.iNotified )
	                {
	                NCN_RDEBUG_INT2( _L( "CNcnNotifApiObserver::NcnTimerCompleted - mailbox %d contains %d published unnotified emails." ),
	                    mailboxStatus.iMailBox,
	                    mailboxStatus.iPublishedNewEmailCount );
	                    
	                // we need to notify
			        NCN_RDEBUG( _L( "CNcnNotifApiObserver::NcnTimerCompleted - notifying about new emails." ) );                
			        UpdateNotification( EFalse, mailboxStatus.iIcon, mailboxStatus.iTone, mailboxStatus.iNote );
	                
	                // mailbox has been notified now
	                mailboxStatus.iNotified = ETrue;
	                }                
	            }
	        mailboxStatus.iRefreshRequested = EFalse;
        	}
        }
    // release timer
    delete iTimer;
    iTimer = NULL;
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::PublishAllNewMessages
// ---------------------------------------------------------
//      
void CNcnNotifApiObserver::PublishAllNewMessages( )
    {
    TInt count( iMailBoxStatusArray.Count() );

    // Run all boxes through
    while( count > 0)
        {
        // decrease count
        count--;
        
	    //S60 boxs need to have notification parameters fetched
	   	TNcnMailBoxStatus& mailboxStatus = iMailBoxStatusArray[count];
	   	
	   	//ignore the error, ncnlist will use existing notification parameters
	   	TRAP_IGNORE( UpdateS60MailBoxNotificationAttributesL(mailboxStatus) );
           	
        // publish message count in box
        TRAP_IGNORE( PublishNewMessagesL( mailboxStatus.iMailBox ) );
        }      
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::MarkAllUnread
// ---------------------------------------------------------
//       
void CNcnNotifApiObserver::MarkAllUnread( )
    {
    TInt count( iMailBoxStatusArray.Count() );

    // Run all boxes through
    while( count > 0)
        {
        // decrease count
        count--;
        
        // mark box unread
        TRAP_IGNORE( MarkUnreadL( iMailBoxStatusArray[count].iMailBox ) );
        }  
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::FindHighest_MsvId_ImapId_LatestTime
// ---------------------------------------------------------
//    
void CNcnNotifApiObserver::FindHighest_MsvId_ImapId_LatestTime(
		const TMsvId& aId,
		TMsvId& aHighestMsvId,
		TUint32& aHighestImapId,
		TTime& aLatestTimeStamp )
    {
   // Return parameters default to 0
    aHighestMsvId = 0;
	aHighestImapId = 0;
    aLatestTimeStamp = 0; 
    
    // get highest values and trap leave
    TRAP_IGNORE( FindHighest_MsvId_ImapId_LatestTimeL(	aId,
    													aHighestMsvId,
    													aHighestImapId,
														aLatestTimeStamp ) );
														
	NCN_RDEBUG( _L("[ncnlist] CNcnNotifApiObserver::FindHighest_MsvId_ImapId_LatestTime") );
	NCN_RDEBUG_INT( _L("[ncnlist] Highest MsvId is: %d"), aHighestMsvId );
	NCN_RDEBUG_INT( _L("[ncnlist] Highest ImapId is: %d"), aHighestImapId );
	NCN_RDEBUG_INT( _L("[ncnlist] Latest time stamp(int64) is: %d"), aLatestTimeStamp.Int64()  );
    }
    
// ---------------------------------------------------------
// CNcnNotifApiObserver::FindHighest_MsvId_ImapId_LatestTimeL
// ---------------------------------------------------------
//    
void CNcnNotifApiObserver::FindHighest_MsvId_ImapId_LatestTimeL( 
		const TMsvId& aId,
		TMsvId& aHighestMsvId,
		TUint32& aHighestImapId,
		TTime& aLatestTimeStamp )
		
    {
    // get msv session handler from model
    CMsvSession& msvSession = iModel.MsvSessionHandler().MsvSessionL();
    
    // get entry pointer
    CMsvEntry* mainEntryPtr = msvSession.GetEntryL( aId );
    CleanupStack::PushL( mainEntryPtr );
        
    // Count the items in selected entry
    TInt items = mainEntryPtr->Count();

    // Search through all items in folder
    while( items > 0)
        {
        // decrement items
        items--;
        
        // Create entry from the item
        const TMsvEntry& entry = (*mainEntryPtr)[items];       
        
        // entry in this iteration
        TMsvId msvId = 0;
    	TUint32 imapId = 0;
    	TTime latestTime = 0;
    	
        // determine entry type    
        switch( entry.iType.iUid )
            {
            // folder
            case KUidMsvFolderEntryValue:
                {
                // find highest entry recursively
                TRAP_IGNORE( FindHighest_MsvId_ImapId_LatestTimeL(	entry.Id(),
                													aHighestMsvId,
                													aHighestImapId,
                													aLatestTimeStamp ) );
                
                break;
                }
            // message
            case KUidMsvMessageEntryValue:
                { 
                //Get entry's MsvId
                msvId = entry.Id();
                
                //Get entry's imapId        
                TMsvEmailEntry emailEntry(entry);                
	            imapId = emailEntry.UID();
	            
	            //Get entry's time
	            latestTime = entry.iDate;
                
		        // Compare this entry's values and set them as highest if they are so.  
		        if(  msvId > aHighestMsvId )
		            {
		            aHighestMsvId = msvId;
		            }
		        if(  imapId > aHighestImapId )
		            {
		            aHighestImapId = imapId;
		            }
		        if(  latestTime > aLatestTimeStamp )
		            {
		            aLatestTimeStamp = latestTime;
		            }
		        break;                  
                }
            // default
            default:
                {
                // do nothing
                break;
                }                
            }
        }
        
    CleanupStack::PopAndDestroy( mainEntryPtr );
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::HandleNewInternalMessagesL
// ---------------------------------------------------------
//
void CNcnNotifApiObserver::HandleNewInternalMessagesL(
    const TNcnNotifMessageType aType )
    {    
    
    // determine message type
    switch( aType )
        {
        // email message
        case EMailMessage:
            {
            PublishAllNewMessages();
            break;
            }
        // inbox message
        case EInboxMessage:
            {
            // Not supported
            User::Leave( KErrNotSupported );
            break;
            }
        // IM message
        case EIMMessage:
            {
            // Not supported
            User::Leave( KErrNotSupported );
            break;
            }
        // default case (unknown)    
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    }
  
// ---------------------------------------------------------
// CNcnNotifApiObserver::HandleNewInternalMessagesL
// ---------------------------------------------------------
//    
void CNcnNotifApiObserver::HandleNewInternalMessagesL(
    const TNcnNotifMessageType aType,
    const TMsvId& aMailBox,
    const MDesCArray& /*aInfo*/ )
    {
        
    // determine message type
    switch( aType )
        {
        // email message
        case EMailMessage:
            {
            NCN_RDEBUG_INT( _L("[ncnlist] CNcnNotifApiObserver::HandleNewInternalMessagesL Publishing box %d."),
                aMailBox );
            
           	// mailboxes need to have notification parameters fetched
           	TNcnMailBoxStatus& mailboxStatus = MailBoxStatusL( aMailBox );
           	
           	//ignore the error, ncnlist will use existing notification parameters
           	TRAP_IGNORE( UpdateS60MailBoxNotificationAttributesL(mailboxStatus) );

            PublishNewMessagesL( aMailBox );
            break;
            }
        // inbox message
        case EInboxMessage:
            {
            // Not supported
            User::Leave( KErrNotSupported );
            break;
            }
        // IM message
        case EIMMessage:
            {
            // Not supported
            User::Leave( KErrNotSupported );
            break;
            }
        // default case (unknown)    
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::HandleInternalMarkUnreadL
// ---------------------------------------------------------
//    
void CNcnNotifApiObserver::HandleInternalMarkUnreadL(
    const TNcnUnreadRequestType aRequest )
    {
    
    // determine request type
    switch( aRequest )
        {
            // email request
            case EIndexMailBox:
                {
                MarkAllUnread();
                UpdateNotification( ETrue ); //Force update
                break;
                }
            // inbox request
            case EIndexInbox:
                {
                // Not supported
                User::Leave( KErrNotSupported );
                break;
                }
            // MCE request
            case EIndexMCE:
                {
                // Not supported
                User::Leave( KErrNotSupported );
                break;
                }
            // default case (unknown)
            default:
                {
                // Not supported
                User::Leave( KErrNotSupported );
                break;
                }
                
        }
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::HandleInternalMarkUnreadL
// ---------------------------------------------------------
//    
void CNcnNotifApiObserver::HandleInternalMarkUnreadL(
    const TNcnUnreadRequestType aRequest,
    const TMsvId& aMailbox )
    {
    // determine request type
    switch( aRequest )
        {
            // email request
            case EIndexMailBox:
                {
            // find the mailbox
            TNcnMailBoxStatus& mailboxStatus = MailBoxStatusL( aMailbox );

            // MailBox is Opened so don't show Icon, even if user has not read any new/unread mails 
            // Publish new email count will be zero.
            mailboxStatus.iShowIcon = EFalse;
            mailboxStatus.iPublishedNewEmailCount = 0;
                MarkUnreadL( aMailbox );
            UpdateNotification( ETrue );
            mailboxStatus.iShowIcon = ETrue;
				break;
                }
            // inbox request
            case EIndexInbox:
                {
                // Not supported
                User::Leave( KErrNotSupported );
                break;
                }
            // MCE request
            case EIndexMCE:
                {
                // Not supported
                User::Leave( KErrNotSupported );
                break;
                }
            // default case (unknown)
            default:
                {
                // Not supported
                User::Leave( KErrNotSupported );
                break;
                }                
        }                   
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::HandleNewMessagesL
// ---------------------------------------------------------
//    
void CNcnNotifApiObserver::HandleNewMessagesL(
    const TMsvId& aMailBox,
    const MNcnNotification::TIndicationType aIndicationType,
    const MDesCArray& /*aInfo*/ )
    {  
    NCN_RDEBUG_INT( _L("[ncnlist] CNcnNotifApiObserver::HandleNewMessagesL Publishing box %d."),
        aMailBox );
    
    //Get the box and set the notification parameters accordingly
    //This will leave if the system does not contain the wanted box
    TNcnMailBoxStatus& mailboxStatus = MailBoxStatusL( aMailBox );
	UpdateMailBoxesNotifications(mailboxStatus, aIndicationType );	  

    PublishNewMessagesL( aMailBox );    
    }
    
// ---------------------------------------------------------
// CNcnNotifApiObserver::HandleMarkUnreadL
// ---------------------------------------------------------
//  
void CNcnNotifApiObserver::HandleMarkUnreadL( const TMsvId& aMailBox )
    {
    // find the mailbox
    TNcnMailBoxStatus& mailboxStatus = MailBoxStatusL( aMailBox );
    
    // MailBox is Opened so don't show Icon, even if user has not read any new/unread mails 
    // Publish new email count will be zero.
    mailboxStatus.iShowIcon = EFalse;
    mailboxStatus.iPublishedNewEmailCount = 0;

    MarkUnreadL( aMailBox );
    
    UpdateNotification( ETrue );
    mailboxStatus.iShowIcon = ETrue;
    
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::LoadMailboxesL
// ---------------------------------------------------------
//
void CNcnNotifApiObserver::LoadMailboxesL( CMsvSession& aMsvSession )
    {
    // load client mtm registry
    CClientMtmRegistry* registry = CClientMtmRegistry::NewL( aMsvSession );
    CleanupStack::PushL( registry );
    
    // get number of registered MTMs
    TInt count = registry->NumRegisteredMtmDlls();
    
    // process each MTM
    while( count-- )
        {
        // get MTM type
        TUid mtmType = registry->MtmTypeUid( count );
        
        // get MTM technology type
        TUid technologyType = registry->TechnologyTypeUid( mtmType );
        
        // and if it is mail type
        if( IsMailTechnologyType( technologyType ) )
            {
            NCN_RDEBUG_INT( _L("CNcnNotifApiObserver::LoadMailboxesL - Loading all boxes with MTM %d"),
                mtmType.iUid );
            
            // Add all boxes with specified MTM to array
            AddBoxesToStatusArrayL( mtmType, technologyType, aMsvSession );                        
            }
        }
    
    // cleanup
    CleanupStack::PopAndDestroy( registry );
    }
    
// ---------------------------------------------------------
// CNcnNotifApiObserver::AddBoxesToStatusArrayL
// ---------------------------------------------------------
//
void CNcnNotifApiObserver::AddBoxesToStatusArrayL(
    const TUid& aMtmType,
    const TUid& aTechnologyType,
    CMsvSession& aMsvSession )
    {
    // Get the list of available email accounts
    CMsvEntrySelection* sel =
        MsvUiServiceUtilities::GetListOfAccountsWithMTML( aMsvSession,
                                                          aMtmType );
    CleanupStack::PushL( sel );

    // Count accounts
    TInt accounts = sel->Count();

    // Go through all accounts
    while( accounts-- )    
        {
        TMsvId msvId = sel->At( accounts );
        
        NCN_RDEBUG_INT2( _L("CNcnNotifApiObserver::AddBoxesToStatusArrayL - Loading box %d with MTM %d"),
                msvId, aMtmType.iUid );
                    
        AddMailBoxL( msvId, aMtmType, aTechnologyType );
        }    
    
    CleanupStack::PopAndDestroy( sel );  // sel
    }
    
// ---------------------------------------------------------
// CNcnNotifApiObserver::AddMailBoxL
// ---------------------------------------------------------
//    
void CNcnNotifApiObserver::AddMailBoxL( const TMsvId& aMsvId, 
										const TUid& aMtmType, 
										const TUid& aTechnologyType)
    {
    TNcnMailBoxStatus mailbox( aMsvId );
    
    // if mail box is not already in status array
    if ( iMailBoxStatusArray.Find( mailbox, 
            TNcnMailBoxStatus::Match ) == KErrNotFound )
        {                        
        // set mailbox fields        
        mailbox.iMTMType = aMtmType;
        mailbox.iMailBoxTechnologyType = aTechnologyType;      
        
        // set the mailboxes 'highest' values
        FindHighest_MsvId_ImapId_LatestTime(mailbox.iMailBox,
        									mailbox.iHighestEMailMsvId ,
        									mailbox.iHighestIMAPId,
        									mailbox.iLatestMessageArrival);
		
		// set the rest of the values
        mailbox.iUnreadCheckpointMsvId = mailbox.iHighestEMailMsvId;
        mailbox.iPublishedCheckpointMsvId = mailbox.iHighestEMailMsvId;
        mailbox.iPublishedCheckpointIMAPId = mailbox.iHighestIMAPId;
        mailbox.iPublishedCheckpointTimeStamp = mailbox.iLatestMessageArrival;
        mailbox.iPublishedNewEmailCount = 0;
        mailbox.iShowIcon = ETrue;
		//In case the mailbox is IMAP/POP/SyncMl update the notifcation
		//parameters. 3rd party box's will give these in API
	    if(	mailbox.iMTMType.iUid == KSenduiMtmImap4UidValue ||
	    	mailbox.iMTMType.iUid == KSenduiMtmPop3UidValue	||  
	    	mailbox.iMTMType.iUid == KSenduiMtmSyncMLEmailUidValue )
			{
			//This should work, but just in case it does not
			//use default parameters
			TRAP_IGNORE(UpdateS60MailBoxNotificationAttributesL(mailbox));
			}
		
        // and finally append mailbox status
        iMailBoxStatusArray.AppendL( mailbox );
        }
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::RemoveMailBoxL
// ---------------------------------------------------------
//
void CNcnNotifApiObserver::RemoveMailBox( const TMsvId& aMsvId )
    {    
    TNcnMailBoxStatus mailbox( aMsvId );
    
    TInt index = iMailBoxStatusArray.Find( mailbox, TNcnMailBoxStatus::Match );
    
    // if mail box is in status array
    if( index != KErrNotFound )
        {
        // update notification
	    TNcnMailBoxStatus& mailboxStatus = iMailBoxStatusArray[index];
	    mailboxStatus.iPublishedNewEmailCount = 0;
	    UpdateNotification( EFalse, mailboxStatus.iIcon, mailboxStatus.iTone, mailboxStatus.iNote );
	    
        // remove entry and compress array
        iMailBoxStatusArray.Remove( index );
        iMailBoxStatusArray.Compress();
        }             
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::UpdateTotalNewEmails
// ---------------------------------------------------------
//
void CNcnNotifApiObserver::UpdateTotalNewEmails()
    {
    TInt count( iMailBoxStatusArray.Count() );
    iTotalNewMailCount = 0;

    // Run all boxes through
    while( count > 0)
        {
        // decrease count
        count--;
        
        // get mailbox status
        const TNcnMailBoxStatus& mailboxStatus = iMailBoxStatusArray[count];       
        NCN_RDEBUG_INT2( _L( "CNcnNotifApiObserver::UpdateTotalNewEmailsL - Mail box %d has %d published new mails!"), mailboxStatus.iMailBox, mailboxStatus.iPublishedNewEmailCount);
       
        // count mailboxes published new emails
        // The published "new" emails are those that have already been notified
        // but which have not yet been viewed by the user. They are marked
        // as "unread" when the user goes to the MCE or to the mailbox. 
        // This might have not yet happened. When this happens the mailbox should
        // call MarkUnread and the iPublishedNewEmailCount will get nulled
        
        // If user has opened the mailbox, IshowIcon for that mail box will be  set false
        //User may have read all mails or may not
        // but for that mail box, mail icon should not be set/shown.
        if(mailboxStatus.iShowIcon)
            iTotalNewMailCount += mailboxStatus.iPublishedNewEmailCount;
        }
        
    NCN_RDEBUG_INT( _L( "CNcnNotifApiObserver::UpdateTotalNewEmailsL - new total %d "),
        iTotalNewMailCount );
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::MailBoxStatusL
// ---------------------------------------------------------
//
CNcnNotifApiObserver::TNcnMailBoxStatus& CNcnNotifApiObserver::MailBoxStatusL( const TMsvId& aId )
    {
    // comparator
    TNcnMailBoxStatus toBeFound( aId );
    
    // Leaves with KErrNotFound if aId is not a mailbox id in status array!
    TInt index = iMailBoxStatusArray.FindL( toBeFound, 
                                            TNcnMailBoxStatus::Match );
    
    // return mailbox
    return iMailBoxStatusArray[index];
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::NewEmailIndicatorsSetL
// ---------------------------------------------------------
//
TBool CNcnNotifApiObserver::NewEmailIndicatorsSetL( const TNcnMailBoxStatus& aMailboxStatus )
    {
    NCN_RDEBUG( _L( "CNcnNotifApiObserver::NewEmailIndicatorsSetL") );  
	NCN_RDEBUG_INT( _L( "Checking indicator from CIMASettingsDataExtension for MTM box %d"), aMailboxStatus.iMTMType.iUid  );
    
    // Method is only valid for Imap, Pop3 and SyncMl boxes
    if(	aMailboxStatus.iMTMType.iUid != KSenduiMtmImap4UidValue &&
    	aMailboxStatus.iMTMType.iUid != KSenduiMtmPop3UidValue	&&   
    	aMailboxStatus.iMTMType.iUid != KSenduiMtmSyncMLEmailUidValue )
        {
        NCN_RDEBUG( _L( "CNcnNotifApiObserver::NewEmailIndicatorsSetL Mail box not supported!") ); 
        User::Leave( KErrNotSupported );
        }
            
     //Get the service id    
    const TMsvId serviceId = aMailboxStatus.iMailBox;
    	
    // get session
    CMsvSession& msvSession = iModel.MsvSessionHandler().MsvSessionL();    

    // Prepare email API for settings loading
    CImumInternalApi* emailApi = CreateEmailApiLC( &msvSession );
    CImumInSettingsData* settings =
        emailApi->MailboxServicesL().LoadMailboxSettingsL( serviceId );
    CleanupStack::PushL( settings );        
    
    // Load settings for the indicator
    TInt indicatorFlags = 0x00;
    settings->GetAttr(
        TImumDaSettings::EKeyEmailAlert,
        indicatorFlags );

    // Cleanup
    CleanupStack::PopAndDestroy( settings );
    settings = NULL;
    CleanupStack::PopAndDestroy( emailApi );
    emailApi = NULL;

    NCN_RDEBUG_INT2( _L( "Indicator for MTM box %d is %d"), aMailboxStatus.iMTMType.iUid, indicatorFlags );

    // return
    return ( indicatorFlags == TImumDaSettings::EFlagAlertSoftNote );
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::ServiceEntryL
// ---------------------------------------------------------
//    
CMsvEntry* CNcnNotifApiObserver::ServiceEntryL( const TMsvId& aServiceId )
    {
    // Get the entry from id
    CMsvEntry* service = ServiceEntryLC( aServiceId );    
    CleanupStack::Pop( service );
    
    return service;
    }
    
// ---------------------------------------------------------
// CNcnNotifApiObserver::ServiceEntryLC
// ---------------------------------------------------------
//    
CMsvEntry* CNcnNotifApiObserver::ServiceEntryLC( const TMsvId& aServiceId )
    {
    // get msv session handler from model
    CMsvSession& msvSession = iModel.MsvSessionHandler().MsvSessionL();
    
    // Get the entry from id
    CMsvEntry* service = msvSession.GetEntryL( aServiceId );        
    CleanupStack::PushL( service );
    
    // get service entry
    const TMsvEntry& serviceEntry = service->Entry();   
    
    // leave with KErrNotSupported if entry does not point to a service
    if( serviceEntry.iType.iUid != KUidMsvServiceEntryValue )
        {
        User::Leave( KErrNotSupported );
        }
    
    // return service
    return service;
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::UpdateNotification
// ---------------------------------------------------------
//
void CNcnNotifApiObserver::UpdateNotification( 	TBool aForceUpdate,
										        TBool aIcon,
										        TBool aTone,
										        TBool aNote )
    {
    // update new email count
    UpdateTotalNewEmails();

    NCN_RDEBUG_INT2( _L( "CNcnNotifApiObserver::UpdateNotificationL - previous total %d, new total %d" ),
            iNotifiedNewMailCount,
            iTotalNewMailCount );
            
    // actions only needed if new count differs from last notified
    // or the action is forced
    if( ((iTotalNewMailCount > 0)&& (iNotifiedNewMailCount != iTotalNewMailCount)) || aForceUpdate )        
        {                
        // count will be notified now
        iNotifiedNewMailCount = iTotalNewMailCount;       
                
        // Inform notifier of any changes
        iModel.NcnNotifier().SetNotification(
            MNcnNotifier::ENcnEmailNotification,
            iTotalNewMailCount,
            aIcon,
            aTone,
            aNote );
        }
    }

// ---------------------------------------------------------
// CNcnNotifApiObserver::IsEMailEntry
// ---------------------------------------------------------
//
TBool CNcnNotifApiObserver::IsEMailEntry( const TMsvEntry& aEntry  )
    {
    TBool ret = EFalse;

    // if it is an entry    
    if( aEntry.iType.iUid == KUidMsvMessageEntryValue )
        {
        // count mailboxes
        TInt count( iMailBoxStatusArray.Count() );
            
        // check each box
        while( count-- )
            {        
            // get mailbox status
            const TNcnMailBoxStatus& mailboxStatus = iMailBoxStatusArray[count];
            
            // if mailboxes message server id matches with entrys service id
            if( mailboxStatus.iMailBox == aEntry.iServiceId )
                {
                ret = ETrue;
                break;
                }
            }
        }
                
    return ret;
    }    
    
// ---------------------------------------------------------
// CNcnNotifApiObserver::GetMailBoxesAttributesL
// ---------------------------------------------------------
//
TInt CNcnNotifApiObserver::GetMailBoxesAttributesL( 	const TMsvId& aMsvId, 
														TUid& aMtmType, 
														TUid& aTechnologyType )
    {   
    // get msv session reference
    CMsvSession& msvSession = iModel.MsvSessionHandler().MsvSessionL();
         
    // service id and entry
    TMsvId serviceId;
    TMsvEntry entry;
    
    TInt error = msvSession.GetEntry( aMsvId, serviceId, entry );
    
    if( error == KErrNone )
        {
        // if entry is a service
        if( entry.iType.iUid == KUidMsvServiceEntryValue )
            {
            // instantiate client mtm registry
            CClientMtmRegistry* registry = CClientMtmRegistry::NewL( msvSession );
            
            // get MTM type
            aMtmType = entry.iMtm;
                        
            // get technology type
            aTechnologyType = registry->TechnologyTypeUid( entry.iMtm );
 
            // delete registry
            delete registry;
            }
        }
        
    return error;
    }
        
// ---------------------------------------------------------
// CNcnNotifApiObserver::IsMailTechnologyType
// ---------------------------------------------------------
//
TBool CNcnNotifApiObserver::IsMailTechnologyType( const TUid& aTechnologyType )
    {
    return ( aTechnologyType == KMailTechnologyTypeUid );
    }
    
// ---------------------------------------------------------
// CNcnNotifApiObserver::IsNotificationNeededForThisMessageL
// ---------------------------------------------------------
//
TBool CNcnNotifApiObserver::IsNotificationNeededForThisMessageL( const TNcnMailBoxStatus& aMailbox, 
																const TMsvEntry& aEntry )
	{
	NCN_RDEBUG_INT2( _L( "CNcnNotifApiObserver::IsNotificationNeeded - Entry: %d in box: %d examined"), 
						aEntry.Id(), aMailbox.iMailBox  );
	
	//Check if this is an IMAP box. If it is we compare message id's
	if( aMailbox.iMTMType.iUid == KSenduiMtmImap4UidValue )
		{
		NCN_RDEBUG( _L( "CNcnNotifApiObserver::NotificationIsNeeded - Mail box is IMAP" ) );
		
		//Checking is done by using the IMAP id     
        TMsvEmailEntry emailEntry(aEntry);   
 	    NCN_RDEBUG_INT2( _L( "IMAP uid in this message is %d, last notified was %d" ), 
 	    					emailEntry.UID(), aMailbox.iPublishedCheckpointIMAPId );

        //Check if the IMAP uid is higher or lower
        if( emailEntry.UID() > aMailbox.iPublishedCheckpointIMAPId )
	        {
	        //This message has higher uid than the last notified had. Notify this.
	        NCN_RDEBUG( _L( "Notification is needed for this message!") );
	        return ETrue;	
	        }
	    else
	    	{
	        NCN_RDEBUG( _L( "Notification is NOT needed for this message!") );
	        return EFalse;		    		
	    	}
		}
	else if( aMailbox.iMTMType.iUid == KSenduiMtmSmtpUidValue )
	    {
	    NCN_RDEBUG( _L( "SMTP Entry, no notification!") );
	    return EFalse;
	    }
	//Not an IMAP box. POP, Syncml etc. Lets compare time stamps
	else
		{
		NCN_RDEBUG( _L( "CNcnNotifApiObserver::NotificationIsNeeded - Mail box is POP, Sync etc." ) );
        
        // Check if the message is new to the device
		NCN_RDEBUG_INT2( _L( "UId in this message is %d, last notified was %d" ), 
				aEntry.Id() , aMailbox.iPublishedCheckpointMsvId );

		
		if ( aEntry.Id() > aMailbox.iPublishedCheckpointMsvId && !IsSyncMLEntryInSentFolderL( aEntry ))
			{
	        // Message is new to device. Notify this.
	        NCN_RDEBUG( _L( "Notification is needed for this message!") );
	        return ETrue;	
	        }
	    else
	    	{
	        NCN_RDEBUG( _L( "Notification is NOT needed for this message!!") );
	        return EFalse;		    		
	    	}
		}	
	}

// ---------------------------------------------------------
// CNcnNotifApiObserver::IsSyncMLEntryInSentFolderL
// ---------------------------------------------------------
//
TBool CNcnNotifApiObserver::IsSyncMLEntryInSentFolderL( const TMsvEntry& aEntry  )
	{
	CMsvSession& msvSession = iModel.MsvSessionHandler().MsvSessionL();
	    
	// get service id and entry
	TMsvId serviceId;
	TMsvEntry entryParent;        
	TInt err = msvSession.GetEntry( aEntry.Parent(), serviceId, entryParent );
	
	if ( entryParent.iMtm == KSenduiMtmSyncMLEmailUid && entryParent.iRelatedId == KMsvSentEntryId )
		{
		return ETrue;
		}
	return EFalse;
	}
// ---------------------------------------------------------
// CNcnNotifApiObserver::UpdateS60MailBoxNotificationAttributesL
// ---------------------------------------------------------
//
void CNcnNotifApiObserver::UpdateS60MailBoxNotificationAttributesL( TNcnMailBoxStatus& aMailbox )
	{
	NCN_RDEBUG( _L( "CNcnNotifApiObserver::GetS60MailBoxNotificationAttributesL" ) );
	
	//Get the "new mail indicator" setting for this box
	TBool setting = NewEmailIndicatorsSetL(aMailbox);
	TInt desiredNotifications = MNcnNotification::EIndicationNormal;
	TInt status = KErrNone;
	
	//Get the notifications from CR key
	if( setting == TRUE )
		{
		status = iModel.GetCRInteger( KCRUidNcnList, KNcnMailNotificationIndicatorOn, desiredNotifications );	
		}
	else
		{	
		status = iModel.GetCRInteger( KCRUidNcnList, KNcnMailNotificationIndicatorOff, desiredNotifications );	
		}
		
	//Look at the value and see if it is OK
	if( status != KErrNone ||
		( desiredNotifications != MNcnNotification::EIndicationIcon &&
		  desiredNotifications != MNcnNotification::EIndicationToneAndIcon &&
		  desiredNotifications != MNcnNotification::EIndicationNormal ) ) 
		{
		NCN_RDEBUG_INT2(_L("Notification is erronous! Setting notifications to default! status: %d value: %d"), status, desiredNotifications );
		desiredNotifications = MNcnNotification::EIndicationNormal;	
		}
				
	//Set the values to mailbox	
	UpdateMailBoxesNotifications( aMailbox, desiredNotifications );		
	}

// ---------------------------------------------------------
// CNcnNotifApiObserver::UpdateMailBoxesNotifications
// ---------------------------------------------------------
//
void CNcnNotifApiObserver::UpdateMailBoxesNotifications( 	
		TNcnMailBoxStatus& aMailbox, 
		const TInt& aIndicationType )
	{
    NCN_RDEBUG( _L("[ncnlist] CNcnNotifApiObserver UpdateMailBoxesNotifications") );

    //Only icon is desired
    if( aIndicationType == MNcnNotification::EIndicationIcon )
	    {
	    aMailbox.iIcon = ETrue;
    	aMailbox.iTone = EFalse;
    	aMailbox.iNote = EFalse;	    	
	    }
	//Icon and tone is desired
	else if( aIndicationType == MNcnNotification::EIndicationToneAndIcon  )	 
		{
	    aMailbox.iIcon = ETrue;
    	aMailbox.iTone = ETrue;
    	aMailbox.iNote = EFalse;	 			
		}
	//Default is all notifications
	else
		{
	    aMailbox.iIcon = ETrue;
    	aMailbox.iTone = ETrue;
    	aMailbox.iNote = ETrue;	 			
		}
		
    NCN_RDEBUG_INT( _L("[ncnlist] CNcnNotifApiObserver Indicators - Icon: %d"), aMailbox.iIcon);
    NCN_RDEBUG_INT( _L("[ncnlist] CNcnNotifApiObserver Indicators - Tone: %d"), aMailbox.iTone);  
    NCN_RDEBUG_INT( _L("[ncnlist] CNcnNotifApiObserver Indicators - Note: %d"), aMailbox.iNote);      
	}
		           
//  End of File
