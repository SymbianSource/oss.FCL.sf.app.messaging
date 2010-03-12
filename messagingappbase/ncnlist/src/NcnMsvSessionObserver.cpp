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
* Description:   Methods for CNcnMsvSessionObserver class.
*
*/



// INCLUDE FILES
#include    <AknUtils.h>                // Avkon Utilities
#include    <AknGlobalNote.h>           // Avkon Global Note
#include    <mtclreg.h>                 // Client MTM
#include    <etel3rdparty.h>			// KMaxTelNumberSize
#include    <SendUiConsts.h>            // For MTM Uids
#include    <msvstd.hrh>                // For Message entry types
#include    <centralrepository.h>

#include 	<telremotepartyinformationpskeys.h>
#include 	<telconfigcrkeys.h>
#include    <settingsinternalcrkeys.h>  // For Autolock status

#include    <smsclnt.h>                 // SMS Client MTM
#include    <smuthdr.h>                 // Sms Header
#include    <StringLoader.h>            // String Loader
#include    <stringresourcereader.h>    // String Resource Reader
#include    <mmsclient.h>

#include    "NcnDebug.h"
#include    "NcnHandlerAudio.h"
#include    "CVoiceMailManager.h"
#include    "NcnMsvSessionObserverBase.h"
#include    "NcnMsvSessionObserver.h"
#include    "NcnModel.h"
#include    "CNcnNotifApiObserver.h"
#include    "CNcnMsvSessionHandler.h"
#include    <NcnListInternalCRKeys.h>
#include    <Ncnlist.rsg>               // New Contacts Note resource

#include	"MNcnMsgWaitingManager.h"

// CONSTANTS
_LIT( KEmptyChar, "" );
_LIT( KSpaceChar, " " );

const TInt KNcnStoreIndicator = 0x80;
const TInt KNcnCphsAddressLength = 4;
const TUint KNcnCphsConstantMask = 0x7E;
const TUint KNcnCphsFirstChar = 2;
const TUint KNcnCphsSecondChar = 3;
const TUint KNcnCphsFirstInfo = 0x10;
const TUint KNcnCphsSecondInfo = 0x00;
const TUint KNcnCphsLineMask    = 0xf0;
const TUint KNcnCphsVmiLine1    = 0x10; // bits 00010000
const TUint KNcnCphsVmiLine2    = 0x90; // bits 10010000
const TUint KNcnCphsSet         = 0x01; // bits 00000001

const TInt KNcnMatchMin = 7;
const TInt KNcnMatchMax = 11;
const TInt KNcnMatchDefault = KNcnMatchMin;
const TInt KNcnMaxTries = 10;
const TInt KNcnTryInterval = 500000;

// ================= LOCAL CONSTANTS =======================
namespace
    {
    // default number of messages waiting to set, if any
    const TUint KNcnDefaultMessagesWaiting = 1;
        
    //Granularity for the unhandled message que
    const TUint KNcnUnhandledMessageQueGranularity = 5;
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNcnMsvSessionObserverBase::NewL
// Protocol specific class factory
// ---------------------------------------------------------
//
CNcnMsvSessionObserverBase* CNcnMsvSessionObserverBase::NewL(
    CNcnModelBase* aModel, CVoiceMailManager& aVMManager)
    {
    CNcnMsvSessionObserverBase* self =
        CNcnMsvSessionObserver::NewL( aModel, aVMManager );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
CNcnMsvSessionObserver::CNcnMsvSessionObserver(
    CNcnModelBase* aModel, CVoiceMailManager& aVMManager ) : 
    CNcnMsvSessionObserverBase( aModel ),
    iUnhandledMessages(NULL),
    iVMManager(aVMManager),
	iUnreadMessages(NULL)
    {
    }

// Two-phased constructor. NewL
CNcnMsvSessionObserverBase* CNcnMsvSessionObserver::NewL(
    CNcnModelBase* aModel, CVoiceMailManager& aVMManager )
    {
    CNcnMsvSessionObserverBase* self = new (ELeave)
		CNcnMsvSessionObserver ( aModel, aVMManager );
       
    return self;
    }

// Two-phased constructor. ConstructL
void CNcnMsvSessionObserver::ConstructL()
    {
    //Base classes constructor
    CNcnMsvSessionObserverBase::ConstructL();
   
    //Own constructions
	iUnhandledMessages = new (ELeave)RArray<TInt>(KNcnUnhandledMessageQueGranularity);
	iUnreadMessages = new (ELeave)RArray<TInt>(KNcnUnhandledMessageQueGranularity);
	iUpdateNotifier = ETrue;
    }
    
// Destructor
CNcnMsvSessionObserver::~CNcnMsvSessionObserver()
    {
    if( iUnhandledMessages != NULL)
	    {
	    delete iUnhandledMessages;
	    iUnhandledMessages = NULL;	
	    }
    if( iUnreadMessages != NULL)
	    {
	    delete iUnreadMessages;
	    iUnreadMessages = NULL;	
	    }
    }         
    
// ---------------------------------------------------------
// CNcnMsvSessionObserver::HandleMsvSessionReadyL
// ---------------------------------------------------------
//
void CNcnMsvSessionObserver::HandleMsvSessionReadyL( CMsvSession& aMsvSession )
	{
	NCN_RDEBUG_INT( _L( "CNcnMsvSessionObserver::HandleMsvSessionReadyL %d >> " ), iInitialized );
	if ( !iInitialized )
		{
		//Most of this is in the base class
		CNcnMsvSessionObserverBase::HandleMsvSessionReadyL(aMsvSession);
			
		// Add unread and visible entries in Inbox to iUnreadMessages
		// Needed later on to determine which entries are truly new and unread
		// and which are just old unread messages. 
		InitUnreadMessagesArrayL();
		
		//Additional task is to update the message icon TSW ID: EXTW-6JDBWN
	    UpdateInboxDataNotifiersL();
	    iInitialized = ETrue;
		}
	else
		{
		iUnhandledMessages->Reset();
		}
    NCN_RDEBUG( _L( "CNcnMsvSessionObserver::HandleMsvSessionReadyL <<" ) );
	}
	
// ---------------------------------------------------------
// CNcnMsvSessionObserver::HandleMsvSessionClosedL
// ---------------------------------------------------------
//
void CNcnMsvSessionObserver::HandleMsvSessionClosedL()
	{
	NCN_RDEBUG(_L("CNcnMsvSessionObserver::HandleMsvSessionClosedL >>"));        

	CNcnMsvSessionObserverBase::HandleMsvSessionClosedL();
    NCN_RDEBUG(_L("CNcnMsvSessionObserver::HandleMsvSessionClosedL >>"));        
	}
	
// ---------------------------------------------------------
// CNcnMsvSessionObserver::HandleEntryEventL
// ---------------------------------------------------------
//
void CNcnMsvSessionObserver::HandleEntryEventL(
    TMsvEntryEvent  aEvent,
    TAny*           aArg1,
    TAny*           /*aArg2*/,
    TAny*           /*aArg3*/)
    {
    NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver::HandleEntryEventL: aEvent: %d"), aEvent );        

    // handle event accordingly
    switch( aEvent )
        {
        case EMsvNewChildren:
            {
            NCN_RDEBUG( _L("CNcnMsvSessionObserver::HandleEntryEventL: EMsvNewChildren") );
            const CMsvEntrySelection* msvEntrySelection =
                static_cast<CMsvEntrySelection*>( aArg1 );
            HandleNewChildrenL( *msvEntrySelection );
            break;
            }
        case EMsvChildrenChanged:
            {
            NCN_RDEBUG( _L("CNcnMsvSessionObserver::HandleEntryEventL: EMsvChildrenChanged") );
            const CMsvEntrySelection* msvEntrySelection =
                static_cast<CMsvEntrySelection*>( aArg1 );
            HandleChangedChildrenL( *msvEntrySelection );
            break;
            }
        case EMsvDeletedChildren:
            {
            NCN_RDEBUG( _L("CNcnMsvSessionObserver::HandleEntryEventL: EMsvDeletedChildren") );
            const CMsvEntrySelection* msvEntrySelection =
                static_cast<CMsvEntrySelection*>( aArg1 );
            HandleDeletedChildrenL( *msvEntrySelection );
            break;
            }
        default:
            {
            NCN_RDEBUG_INT( _L("CNcnMsvSessionObserver::HandleEntryEventL: default (%d)"), aEvent );
            break;
            }
        }        
    NCN_RDEBUG( _L( "CNcnMsvSessionObserver::HandleEntryEventL <<" ) );
    }

// ---------------------------------------------------------
// CNcnMsvSessionObserver::HandleNewChildrenL
// ---------------------------------------------------------
//    
void CNcnMsvSessionObserver::HandleNewChildrenL(
    const CMsvEntrySelection& aMsvEntrySelection )
    {
    NCN_RDEBUG( _L( "CNcnMsvSessionObserver::HandleNewChildrenL" ) );

    // get session from session handler
    CMsvSession& msvSession = iModel->MsvSessionHandler().MsvSessionL();
    
	//The received entrys type in ncn list's own definition
    TNcnInboxEntryType receivedEntryType = EUnknown;
    
    //Zero the phone number that is updated later
    iTelNumber.iTelNumber.Zero();

	//The amount of new children
    const TInt newEntries = aMsvEntrySelection.Count();
    NCN_RDEBUG_INT( _L( "HandleNewChildrenL Count is %d" ), newEntries );
        
    // Go through all the new children
    for ( TInt index = 0; index < newEntries; index++ )
        {
        // Get the id of new entry and check if it is a sms message.
        // If so, then we have to do a detailed check.
        TMsvId id = aMsvEntrySelection.At( index );
        CMsvEntry* entryToHandlePtr = msvSession.GetEntryL( id );
        CleanupStack::PushL(entryToHandlePtr);
        
        TMsvEntry entryToHandle = entryToHandlePtr->Entry();
		//Extensive debug information about the new entry
		#ifdef _DEBUG
		PrintEntryDebugInformation(entryToHandle);
		#endif
		
		//Message is in preparation
        if( entryToHandle.InPreparation() && 
        	( entryToHandle.iMtm.iUid == KSenduiMtmSmsUidValue || 
        	  entryToHandle.iMtm.iUid == KSenduiMtmMmsUidValue ||
        	  entryToHandle.iMtm.iUid == KSenduiMtmBtUidValue ||
        	  entryToHandle.iMtm.iUid == KSenduiMtmIrUidValue ) )
            {
            switch( entryToHandle.iMtm.iUid )
	            {
	            case KSenduiMtmSmsUidValue:
	            	{
	            	NCN_RDEBUG_INT(_L("MsvEntry id: %d is an SMS that is still in preparation!"), entryToHandle.Id() );
	            	break;
	            	}
	            case KSenduiMtmMmsUidValue:
	            	{
	            	NCN_RDEBUG_INT(_L("MsvEntry id: %d is an MMS that is still in preparation!"), entryToHandle.Id() );
	            	break;
	            	}
	            default:
	            	{
	            	NCN_RDEBUG_INT(_L("MsvEntry id: %d is an IR/Bt message that is still in preparation!"), entryToHandle.Id() );
	            	break;
	            	}
	            }
	
			//Append the message to unhandled que. We handle it when it changes to completed
			iUnhandledMessages->AppendL( entryToHandle.Id() );
            }
        else if( entryToHandle.InPreparation() == EFalse && 
                 entryToHandle.Unread() )
            {
            switch ( entryToHandle.iMtm.iUid )
                {
                //Message is unread and complete SMS
                case KSenduiMtmSmsUidValue:
                    {
                    NCN_RDEBUG_INT(_L("MsvEntry id: %d is an SMS that is complete!"), entryToHandle.Id() );            

        			// CheckAndHandleSmsEntryL takes ownership of the entryToHandlePtr
        			// and deletes it after completion
    		        CleanupStack::Pop(entryToHandlePtr);
                    // If the entry is an unread sms then call handling method...
                    // The method returns the entry type.		
                    TRAPD( err, receivedEntryType = CheckAndHandleSmsEntryL( entryToHandlePtr ) );
                    entryToHandlePtr = NULL;
        	        NCN_RDEBUG_INT(_L("New entry is of Ncn entry type: %d"), receivedEntryType );
        
                    // Check if message is deleted or message couldn't be
        			// recognized at all because of error
                    if( err != KErrNone )
                        {
                        // Don't react on messages that doesn't exist
        	        	NCN_RDEBUG(_L("Handling of the SMS failed. Changing it as NcnList entry type 'deleted'!") );
                        receivedEntryType = EDeletedMessage;
                        }
        			else                           
                        {
                        TInt err = iUnreadMessages->Append( entryToHandle.Id() );                   
                        if ( err )
                            {
                            NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver::HandleNewChildrenL - APPEND ERROR %d!"), err );
                            }
                        }
                    break;
                    }
                //Message is unread and complete MMS
                case KSenduiMtmMmsUidValue:
                    {
        			NCN_RDEBUG_INT(_L("MsvEntry id: %d is an MMS that is complete!"), entryToHandle.Id() );
        
        			//This is the MMS entry's type. Notify about its arrival
        			receivedEntryType = EMMSMessage;
			
                    TInt err = iUnreadMessages->Append( entryToHandle.Id() );
                    if ( err )
                        {
                        NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver::HandleNewChildrenL - APPEND ERROR %d!"), err );
                        }
                    break;
                    }
                 
                 default:
                    {
                    NCN_RDEBUG_INT2(_L("MsvEntry id: %d with unknown MTM: 0x%x is complete!"), entryToHandle.Id(), entryToHandle.iMtm );
                    
                    //The exact type is not known
                   receivedEntryType = EUnknown;
                    
                    TInt err = iUnreadMessages->Append( entryToHandle.Id() );
                    if ( err )
                        {
                        NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver::HandleNewChildrenL 2- APPEND ERROR %d!"), err );
                        }
                    break;
                    }
                }
            // Common for all
            // notify about new child entry
            NotifyAboutNewChildEntryL( entryToHandle, receivedEntryType );
            }
         //Not an SMS/MMS, we don't do anything with it
         else
            {
            //Still in preparation
            
            // Do not add entry, if it is read. Message server sends all messages from
            // the store as changes messages, when changing message store.
            if ( entryToHandle.Unread() )
                {
                #ifdef _DEBUG
                PrintEntryDebugInformation(entryToHandle);
                #endif
                NCN_RDEBUG_INT2(_L("MsvEntry id: %d with unknown MTM: 0x%x is still in preparation!"), entryToHandle.Id(), entryToHandle.iMtm );    
                    
                TInt err = iUnreadMessages->Append( entryToHandle.Id() );
                if ( err )
                    {
                    NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver::HandleNewChildrenL 3 - APPEND ERROR %d!"), err );
                    }
                }
            }

        if (entryToHandlePtr)
            {
            CleanupStack::PopAndDestroy(entryToHandlePtr);   
            entryToHandlePtr = NULL;
            }
        }
    }
    
// ---------------------------------------------------------
// CNcnMsvSessionObserver::HandleChangedChildrenL
// ---------------------------------------------------------
//        
void CNcnMsvSessionObserver::HandleChangedChildrenL(
    const CMsvEntrySelection& aMsvEntrySelection )
    {
    NCN_RDEBUG( _L( "CNcnMsvSessionObserver::HandleChangedChildrenL >>" ) );
    
	//Get session from session handler
    CMsvSession& msvSession = iModel->MsvSessionHandler().MsvSessionL();
        
  	//The amount of changed children
  	const TInt newEntries = aMsvEntrySelection.Count();
    NCN_RDEBUG_INT( _L( "HandleChangedChildrenL Count is %d" ), newEntries );
         	
  	 // Go through the changed children
    TBool completeHandling(EFalse);

    for ( TInt index = 0; index < newEntries; index++ )
	    {
	    //Get the id of the new entry and check if it has been processed
	    //and also check if it can be notifed now
		TMsvId id = aMsvEntrySelection.At( index );
		CMsvEntry* entryToHandlePtr = msvSession.GetEntryL( id );
        CleanupStack::PushL(entryToHandlePtr);
        
        TMsvEntry entryToHandle = entryToHandlePtr->Entry();
        		
		#ifdef _DEBUG
		PrintEntryDebugInformation( entryToHandle );
		#endif
		
		//Was this entry handled when it arrived or was it in preparation?
		TInt indexInUnhandledArray = iUnhandledMessages->Find( entryToHandle.Id() );				
		NCN_RDEBUG_INT(_L("HandleChangedChildrenL indexInUnhandledArray [%d]"), indexInUnhandledArray );
				
        // Did the entry change to unread during preparation?
        if ( indexInUnhandledArray != KErrNotFound 
            && entryToHandle.Unread() ) 
            {
            TInt indexInUnreadArray = iUnreadMessages->Find( entryToHandle.Id() );				
            if ( indexInUnreadArray == KErrNotFound )
                {
                // This entry has changed to Unread(0->1) during preparation. Add it to unread messages cache.
                TInt err = iUnreadMessages->Append( entryToHandle.Id() );
                if ( err )
                    {
                    NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver::HandleNewChildrenL - APPEND ERROR %d!"), err );
                    }
                }
            }

			
		//Check if the entry is unhandled SMS and it can now be handled
	    TBool entryUnhandled = 
	        indexInUnhandledArray != KErrNotFound &&
	    	entryToHandle.InPreparation() == EFalse &&
	    	entryToHandle.Unread();

        TNcnInboxEntryType handledEntryType = EUnknown;  // default type
        completeHandling = EFalse;
	    if( entryUnhandled )
            {
            switch( entryToHandle.iMtm.iUid )
            	{
           	
	            case KSenduiMtmSmsUidValue:
	    		    {
	      			NCN_RDEBUG(_L("HandleChangedChildrenL Unhandled SMS found! Handling it now!") );
	    			// CheckAndHandleSmsEntryL takes ownership of the entryToHandlePtr
	    			// and deletes it after completion
	    			CleanupStack::Pop(entryToHandlePtr);

	      			// If the entry is an unread sms then call handling method...
	    			// The method returns the entry type.
	    			iUpdateNotifier = ETrue;
	    			TRAPD( err, handledEntryType = CheckAndHandleSmsEntryL( entryToHandlePtr ) );
	                entryToHandlePtr = NULL;
	    	        NCN_RDEBUG_INT(_L("New entry is of Ncn entry type: %d"), handledEntryType );
	    
	    			// Check if message is deleted or message couldn't be
	    			// recognized at all because of error
	    			if( err != KErrNone )
	    			    {
	                    // Don't react on messages that doesn't exist
	    	        	NCN_RDEBUG(_L("Handling of the SMS failed. Changing it as NcnList entry type 'deleted'!") );
	                    handledEntryType = EDeletedMessage;
	    			    }
					break;
	    		    }
            	case KSenduiMtmMmsUidValue: 	            	
	    		    {
	      			NCN_RDEBUG( _L("HandleChangedChildrenL Unhandled MMS found! Handling it now!") );
	      			//This is the unhandled entry's type
	    			handledEntryType = EMMSMessage;
	    			break;
	    		    }
            	
            	default:
            		{
            		NCN_RDEBUG( _L( "CNcnMsvSessionObserver::HandleChangedChildrenL: Completed something else than SMS or MMS. Smart message?" ) );
            		 // Eg. smart messages
            		break;
            		}
            	}
            // common for all messages
            completeHandling = ETrue; 
            }
		 // It is required to update new message notifications after a message entry has been handled (by ncnlist) and
		 // read (by user e.g. in MCE)
		 
		 // 2. If a read message is marked unread (e.g. in MCE), this condition is not true and notification is not displayed
		 // about it. However if a new message arrives, all unread message (despite how they got that way) are treated as a new message.
		 else if( indexInUnhandledArray == KErrNotFound && 
		 		 !entryToHandle.Unread() &&
                 !entryToHandle.InPreparation() )
			{
			NCN_RDEBUG_INT( _L("HandleChangedChildrenL TMsvId %d changed - update message counts!"), entryToHandle.Id() );
			TInt indexInUnreadArray = iUnreadMessages->Find( entryToHandle.Id() );				
			NCN_RDEBUG_INT(_L("HandleChangedChildrenL indexInUnreadArray [%d]"), indexInUnreadArray );		
			if ( indexInUnreadArray != KErrNotFound )
				{
				iUnreadMessages->Remove( indexInUnreadArray );
				}
			}
		 //We dont do anything special with this message		 		  
		 else
		 	{
  			NCN_RDEBUG_INT( _L("HandleChangedChildrenL Unknown children has changed: TMsvId %d"), entryToHandle.Id() );
		 	}
		 	
        if ( completeHandling )
            {
            // notify about new child entry
            NotifyAboutNewChildEntryL( entryToHandle, handledEntryType );
            //Remove the notification from our unhandled list
            iUnhandledMessages->Remove( indexInUnhandledArray );
            }
        if ( entryToHandlePtr )
            {
            CleanupStack::PopAndDestroy(entryToHandlePtr);
            }
	    }
	    
	 if(!iUpdateNotifier) 
	 {
	 	NCN_RDEBUG( _L( "CNcnMsvSessionObserver::HandleChangedChildrenL: iUpdateNotifier is set to False so No update of Notifiers" ) );
	 	iUpdateNotifier = ETrue;
	 	return;
	 }

    UpdateInboxDataNotifiersL();
    NCN_RDEBUG( _L( "CNcnMsvSessionObserver::HandleChangedChildrenL <<" ) );
    }

// ---------------------------------------------------------
// CNcnMsvSessionObserver::HandleDeletedChildrenL
// ---------------------------------------------------------
//        
void CNcnMsvSessionObserver::HandleDeletedChildrenL(
    const CMsvEntrySelection& aMsvEntrySelection )
    {
    NCN_RDEBUG( _L( "CNcnMsvSessionObserver::HandleDeletedChildrenL" ) );
    
    // get session from session handler
    CMsvSession& msvSession = iModel->MsvSessionHandler().MsvSessionL();
    const TInt deletedEntries = aMsvEntrySelection.Count();            

    NCN_RDEBUG_INT( _L( "CNcnMsvSessionObserver::HandleDeletedChildrenL: deleting %d messages" ), deletedEntries );
    NCN_RDEBUG_INT( _L( "CNcnMsvSessionObserver::HandleDeletedChildrenL: %d entries in iUnreadMessages" ), iUnreadMessages->Count() );
         		         	
  	 // Go through the deleted children
    for ( TInt index = 0; index < deletedEntries && iUnreadMessages->Count(); index++ )
	    {	   
	    NCN_RDEBUG( _L( "CNcnMsvSessionObserver::HandleDeletedChildrenL: for loop" ) ); 
	    //Get the id of the new entry and check if it has been processed
	    //and also check if it can be notifed now
		TMsvId id = aMsvEntrySelection.At( index );

		NCN_RDEBUG_INT( _L( "CNcnMsvSessionObserver::HandleDeletedChildrenL: Checking if msvid %d can found from unread array" ), id );

		TInt idx = iUnreadMessages->Find( id );
		if( idx != KErrNotFound )
			{
			NCN_RDEBUG_INT( _L( "CNcnMsvSessionObserver::HandleDeletedChildrenL: Removing msvid %d from unread array unread array" ), id );	
			iUnreadMessages->Remove( idx );
			}			
	    }
	    
	NCN_RDEBUG( _L( "CNcnMsvSessionObserver::HandleDeletedChildrenL: Updating unread message count" ) );	    
    
    UpdateInboxDataNotifiersL();
     NCN_RDEBUG( _L( "CNcnMsvSessionObserver::HandleDeletedChildrenL <<" ) );

    }

// ---------------------------------------------------------
// CNcnMsvSessionObserver::NotifyAboutNewChildEntryL
// ---------------------------------------------------------
//    
void CNcnMsvSessionObserver::NotifyAboutNewChildEntryL(
    const TMsvEntry& aEntry,
    TNcnInboxEntryType aEntryType )    
    {
    NCN_RDEBUG(_L("CNcnMsvSessionObserver::NotifyAboutNewChildEntryL >>"));    
    // Now we check what is the entry type and continue as required
    switch ( aEntryType )
        {
        // If the entry was a class 0 sms, then we set the lights on
        // and play the message received tone.
        case EClass0SmsMessage:
            {
            // set class0 notification
            iModel->NcnNotifier().SetNotification(
                    MNcnNotifier::ENcnClass0MessageNotification,
                    1 );
            }
            break;

        // If the new entry was a message of which the user
        // should be informed by the note then we set the lights on,
        // play the message received ringing tone and count the number
        // of unread messages in in-box. Then the model class is
        // informed about the new number of unread messages.
        case ESmsMessage:
        case EMMSMessage:
        case EMessageWaitingStoreMessage:
        case ESpecialMessage:
        case EUnknown:
            {
            // If the message is unread then the tone is played...
            if( IsUnread( aEntry ) )
                {
                UpdateInboxDataNotifiersL();
                // Check if the message should be shown to user
                if( iModel->IsSupported( KNcnSendFileInCall ) )
                    {
                    CheckAndLaunchMCEL( aEntry );
                    }
                }
            break;
            }
        // If the new entry was a message waiting discard set message
        // then we set the lights on and play the tone.
        // Because it is a discard message (not containing text),
        // no note is required
        case EMessageWaitingDiscardSetMessage:
            {
            NCN_RDEBUG(_L("EMessageWaitingDiscardSetMessage:"));
            break;
            }
        // If the new entry was a delivery report or message waiting
        // discard clear message, just switch the lights on.
        case EDeliveryReport:
        case EMessageWaitingDiscardClearMessage:
            {
            NCN_RDEBUG(_L("EDeliveryReport:"));
            }
            break;
        // Delivery err report or deleted message, do nothing
        case EDeletedMessage:
        case EDeliveryErrReport:
            {
            // do nothing
            }
            break;
        default:
            break;
        }
    NCN_RDEBUG(_L("CNcnMsvSessionObserver::NotifyAboutNewChildEntryL <<")); 
    }

// ---------------------------------------------------------
// CNcnMsvSessionObserver::CheckAmountOfUnreadMessagesInInboxL
// ---------------------------------------------------------
//
void CNcnMsvSessionObserver::CheckAmountOfUnreadMessagesInInboxL( TInt& aMsgCount, TInt& aAudioMsgCount )
    {
    aMsgCount = 0;
    aAudioMsgCount = 0;
    TBool audioMessagingSupported = iModel->IsSupported( KNcnAudioMessaging );
    // get session from session handler
    CMsvSession& msvSession = iModel->MsvSessionHandler().MsvSessionL();
        
    // Check how many unread messages there are in the In-box.
    const TInt itemCount = iInboxFolder->Count();
    NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver::CheckAmountOfUnreadMessagesInInbox [%d]"), itemCount );
    
    for( TInt index = 0; index < itemCount; index++ )
        {
        const TMsvEntry entry = (*iInboxFolder)[index];

        if( entry.iType.iUid == KUidMsvMessageEntryValue && 
        	entry.Visible() &&
        	IsUnread( entry ) )
            {            
			if( audioMessagingSupported && entry.iBioType == KUidMsgSubTypeMmsAudioMsg.iUid )
			    {
                aAudioMsgCount++;
                }
            else 
                {
                aMsgCount++;
                }
            }
        }
        
    NCN_RDEBUG_INT( _L( "CNcnMsvSessionObserver::CheckAmountOfUnreadMessagesInInbox \
        SetCRInt(KNcnNewMessageCountInInbox): %d" ), aMsgCount );
    
    TInt totalNewMsgCount = aMsgCount + aAudioMsgCount;
    User::LeaveIfError( iModel->SetCRInt( 
        KCRUidNcnList, KNcnNewMessageCountInInbox, totalNewMsgCount) );
        
    NCN_RDEBUG_INT( _L( "CNcnMsvSessionObserver::CheckAmountOfUnreadMessagesInInbox other\
        :%d" ), aMsgCount );

    if ( audioMessagingSupported )
        {
        NCN_RDEBUG_INT( _L("CNcnMsvSessionObserver::\
        CheckAmountOfNotListenedAudioMessagesInInboxL SetCRInt(KNcnNewAudioMessageCountInInbox) %d" ), aAudioMsgCount );
            
        User::LeaveIfError( iModel->SetCRInt( 
            KCRUidNcnList, KNcnNewAudioMessageCountInInbox, aAudioMsgCount ) );
        }
    }

// ---------------------------------------------------------
// CNcnMsvSessionObserver::CheckAndHandleSmsEntryL
// ---------------------------------------------------------
//
CNcnMsvSessionObserver::TNcnInboxEntryType
    CNcnMsvSessionObserver::CheckAndHandleSmsEntryL(
        CMsvEntry* aEntryToHandlePtr )
    {
    NCN_RDEBUG(_L("CNcnMsvSessionObserver::CheckAndHandleSmsEntryL"));
    
    // Check that the pointer is valid
    TNcnInboxEntryType ncnEntryType = EUnknown;
    if( aEntryToHandlePtr == NULL )
        {
        return ncnEntryType;
        }

    //We take the ownership of the aEntryToHandlePtr
    CleanupStack::PushL(aEntryToHandlePtr);
    
    // get session from session handler
    CMsvSession& msvSession = iModel->MsvSessionHandler().MsvSessionL();
    
    // And get a copy of the TMsvEntry through the CMsvEntry
    TMsvEntry entryToHandle = aEntryToHandlePtr->Entry();
			
    //This registry holds details of the all the Client-side MTMs
    CClientMtmRegistry* mtmReg = CClientMtmRegistry::NewL( msvSession );
	CleanupStack::PushL(mtmReg);

    //Client-side MTM object for the MTM specified by aMtmTypeUid
    CBaseMtm* clientMtm = NULL;

	// Wait for messaging objects to become available
	TBool clientSet = EFalse;
	TBool entrySet = EFalse;
    for (TInt tries = 0; tries < KNcnMaxTries; tries++ )
		{
		if ( !clientSet )
			{
			TRAPD( err0, clientMtm = mtmReg->NewMtmL( KUidMsgTypeSMS ));
			if ( err0 == KErrNone )
				{
				CleanupStack::PushL( clientMtm );
				clientSet = ETrue;
				}
			else if (( err0 == KErrInUse ) && ( tries < KNcnMaxTries - 1 ))
				{
				NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver: CheckAndHandleSmsEntryL mtmReg->NewMtmL() leave %d"), err0 );
				User::After( KNcnTryInterval ); // CSI: 92 # We must use sleep here before we next try
				}
			else
				{
				NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver: CheckAndHandleSmsEntryL << mtmReg->NewMtmL() leave %d"), err0 );
				User::Leave( err0 );
				}
			}
		if ( clientSet && !entrySet )
			{
			// in the stack we now have clientMtm, mtmReg and aEntryToHandlePtr
			// we need to remove aEntryToHandlePtr
			CleanupStack::Pop(clientMtm);
			CleanupStack::Pop(mtmReg);		
			CleanupStack::Pop(aEntryToHandlePtr);
				
			//Transfer ownership of the aEntryToHandlePtr to the clientMtm
			TRAPD( err1, clientMtm->SetCurrentEntryL( aEntryToHandlePtr ));
			
			//Push the mtmReg and clientMtm back to stack
			CleanupStack::PushL(mtmReg);
			CleanupStack::PushL(clientMtm); //clientMtm is now responsible for the aEntryToHandlePtr
				
			if ( err1 == KErrNone )
				{
				entrySet = ETrue;
				}
			else if ( err1 == KErrInUse )
				{
				NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver: CheckAndHandleSmsEntryL clientMtm->SetCurrentEntryL() leave %d"), err1 );
				User::After( KNcnTryInterval ); // CSI: 92 # We must use sleep here before we next try
				}
			else
				{
				NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver: CheckAndHandleSmsEntryL clientMtm->SetCurrentEntryL() leave %d"), err1 );
				delete aEntryToHandlePtr; //Ownership was not transferred
				User::Leave( err1 );
				}
			}
		if ( clientSet && entrySet )
			{
			TRAPD( err2, clientMtm->LoadMessageL() );

			if ( err2 == KErrNone )
				{
				break;
				}
			else if ( err2 == KErrInUse )
				{
				NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver: CheckAndHandleSmsEntryL clientMtm->LoadMessageL() leave %d"), err2 );
				User::After( KNcnTryInterval ); // CSI: 92 # We must use sleep here before we next try
			    }
			else
				{
				NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver: CheckAndHandleSmsEntryL clientMtm->LoadMessageL() leave %d"), err2 );
				User::Leave( err2 );
				}
			}
		}
	//Always at this point the cleanupstack contains:
	//1) clientMtm
	//2) mtmReg
	//clientMtm also has ownership to aEntryToHandlePtr
	
    // check for special types of message
    CSmsHeader& header = static_cast<CSmsClientMtm*>(clientMtm)->SmsHeader();
    CSmsPDU& pdu = header.Message().SmsPDU();

    TSmsDataCodingScheme::TSmsClass smsClass;

    NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver:CheckAndHandleSmsEntryL Message's PDU type: %d"), pdu.Type());

    // First we check if the sms is a delivery report and if so, handle it.
    if( pdu.Type() == CSmsPDU::ESmsStatusReport ) //test for delivery note
        {
        CSmsStatusReport* statusReport = static_cast<CSmsStatusReport*>(&pdu);

        if( statusReport->Status() ==
                TSmsStatus::ESmsShortMessageReceivedBySME ||
            statusReport->Status() ==
                TSmsStatus::ESmsShortMessageForwardedBySCToSMEButUnconfirmedBySC ||
            statusReport->Status() ==
                TSmsStatus::ESmsShortMessageReplacedBySC )
            {
            
            // Message is reached the recipient
            iModel->NcnUI().ShowDeliveryNoteL( entryToHandle );
            ncnEntryType = EDeliveryReport;
            }
        else
            {
            // This means that the SMS has not yet reached the recipient.
            // Some problems in delivering.
            ncnEntryType = EDeliveryErrReport;
            }
        }
    // Then we check if the sms is a class 0 sms. If so, then we
    // launch the viewer
    else if( pdu.Class( smsClass ) ) //test for class 0 sms
        {
        if( smsClass == TSmsDataCodingScheme::ESmsClass0 )
            {
            // Class 0 SMS should be set invisible at this point
            entryToHandle.SetVisible( EFalse );
            aEntryToHandlePtr->ChangeL( entryToHandle );

            // Class 0 message can contain CHPS type indication bits,
            // so they must be checked
            ncnEntryType =
                CheckForMessageWaitingL( *aEntryToHandlePtr, header );

            if( ncnEntryType != EMessageWaitingDiscardSetMessage &&
                ncnEntryType != EMessageWaitingDiscardClearMessage )
                {
                // show message
                iModel->NcnUI().ShowMessageL( entryToHandle );
                ncnEntryType = EClass0SmsMessage;
                }
            }
        else
            {
            ncnEntryType =
                CheckForMessageWaitingL( *aEntryToHandlePtr, header );
            }
        }
    // Then we check the status bits of the message
    // (message waiting, cphs etc...)
    else
        {
        ncnEntryType =
                CheckForMessageWaitingL( *aEntryToHandlePtr, header );
        }

    // If the message is to be deleted, then we do it here.
    if( ncnEntryType == EMessageWaitingDiscardSetMessage ||
        ncnEntryType == EMessageWaitingDiscardClearMessage ||
        ncnEntryType == EDeliveryReport ||
        ncnEntryType == EDeliveryErrReport )
        {
        // Delete entry        
        TRAPD( err, iInboxFolder->DeleteL( entryToHandle.Id() ) );
        NCN_RDEBUG_INT(_L("HandleNewChildrenL:CheckAndHandleSmsEntryL Trap error Id is: %d "), err );
		if(err == KErrInUse)
		{
			entryToHandle.SetVisible( EFalse );
			entryToHandle.SetInPreparation(ETrue);
#ifdef _DEBUG
			TRAPD(TrapError,aEntryToHandlePtr->ChangeL( entryToHandle ));
			NCN_RDEBUG_INT(_L("HandleNewChildrenL:CheckAndHandleSmsEntryL ChangeL Trap error1 Id is: %d "), TrapError );
#else
			TRAP_IGNORE(aEntryToHandlePtr->ChangeL( entryToHandle ));
#endif			
			iUpdateNotifier = EFalse;
			NCN_RDEBUG(_L("CNcnMsvSessionObserver::CheckAndHandleSmsEntryL Discard message: Try deleting again "));
			//Entry is not yet released.. try Remove Entry
			msvSession.RemoveEntry(entryToHandle.Id());
		}  

        }
    else
        {
        iTelNumber = GetTelNumber( header.Message() );
        }

    // Now we have got the SMS header of the message. Next delete mtmReq
    // and clientMtm. Destroying clientMtm destroys
    // also the current entryFocused to so we must not
    // delete the entryToHandlePtr after this...
    CleanupStack::PopAndDestroy(2); // mtmReq, clientMtm
    return ncnEntryType;
    }
    
// ---------------------------------------------------------
// CNcnMsvSessionObserver::CheckForMessageWaitingL
// Check if the sms is a message waiting indicator and
// notify the system agent if it is.
// ---------------------------------------------------------
//
CNcnMsvSessionObserver::TNcnInboxEntryType
    CNcnMsvSessionObserver::CheckForMessageWaitingL(
        CMsvEntry&      aEntry,
        CSmsHeader&    aHeader )
    {
    NCN_RDEBUG(_L("CNcnMsvSessionObserver::CheckForMessageWaitingL"));

    CSmsPDU &pdu = aHeader.Message().SmsPDU();

    // First check if the User Data contains Special message
    // indications coded as specified in GSM 03.40 V7.4.0
    if( pdu.UserDataPresent() )
        {
        // handle user data
        TNcnInboxEntryType entryType = HandleSmsUserDataL( pdu.UserData() );
        
        // use type could be identified from user data
        // return it now
        if( entryType != EUnknown )
            {
            NCN_RDEBUG(_L("CNcnMsvSessionObserver::CheckForMessageWaitingL, not EUnknown 1 <<"));
            return entryType;
            }
        }

    // if IndicationType() is called on a normal sms there is a panic,
    // so have to filter out wrong type of smses with the following tests

    if(!pdu.DataCodingSchemePresent())
        {
        NCN_RDEBUG(_L("CNcnMsvSessionObserver::CheckForMessageWaitingL, ESmsMessage 1<<"));
        return ESmsMessage;
        }

    TInt bits7to4 = pdu.Bits7To4();
    TSmsDataCodingScheme::TSmsIndicationState indicationState =
        TSmsDataCodingScheme::ESmsIndicationInactive;

    switch (bits7to4)
        {
        case TSmsDataCodingScheme::ESmsDCSMessageWaitingIndication7Bit:
        case TSmsDataCodingScheme::ESmsDCSMessageWaitingIndicationUCS2:
        case TSmsDataCodingScheme::ESmsDCSMessageWaitingIndicationDiscardMessage:

           indicationState = pdu.IndicationState();

           if( indicationState != TSmsDataCodingScheme::ESmsIndicationActive &&
               indicationState != TSmsDataCodingScheme::ESmsIndicationInactive )
                {
                NCN_RDEBUG(_L("CNcnMsvSessionObserver::CheckForMessageWaitingL, ESmsMessage 2<<"));
                return ESmsMessage;
                }
           break;

        default:
            // Check for CPHS message waiting bits. If they are not
            // present, we are dealing with normal sms message
            CNcnMsvSessionObserver::TNcnInboxEntryType type =
                CheckForCPHSIndicationBitsL( aEntry, aHeader );

            if( type != EUnknown )
                {
                NCN_RDEBUG(_L("CNcnMsvSessionObserver::CheckForMessageWaitingL, not EUnknown 2<<"));
                return type;
                }
            NCN_RDEBUG(_L("CNcnMsvSessionObserver::CheckForMessageWaitingL, ESmsMessage 3<<"));

            return ESmsMessage;
        }

    // handle dcs indication
    TBool indicatorSetMessage = HandleDCSIndicationL(
        pdu.IndicationType(),
        pdu.IndicationState() );

    if( bits7to4 == TSmsDataCodingScheme::
            ESmsDCSMessageWaitingIndicationDiscardMessage &&
            aHeader.Message().Buffer().Length() < 2 )
        {
        if( indicatorSetMessage )
            {
            NCN_RDEBUG(_L("CNcnMsvSessionObserver::CheckForMessageWaitingL, EMessageWaitingDiscardSetMessage <<"));
            return EMessageWaitingDiscardSetMessage;
            }
        else
            {
            NCN_RDEBUG(_L("CNcnMsvSessionObserver::CheckForMessageWaitingL, EMessageWaitingDiscardClearMessage <<"));
            return EMessageWaitingDiscardClearMessage;
            }
        }
    NCN_RDEBUG(_L("CNcnMsvSessionObserver::CheckForMessageWaitingL, EMessageWaitingStoreMessage <<"));
    return EMessageWaitingStoreMessage;
    }

// ---------------------------------------------------------
// CNcnMsvSessionObserver::HandleDCSIndicationL
// ---------------------------------------------------------
//    
TBool CNcnMsvSessionObserver::HandleDCSIndicationL(
    TSmsDataCodingScheme::TSmsIndicationType aIndicationType,
    TSmsDataCodingScheme::TSmsIndicationState aIndicationState )
    {
    NCN_RDEBUG( _L("CNcnMsvSessionObserver::HandleDCSIndicationL >>") );
    // amount of messages waiting set
    TUint amount = 0;
    
    // determine indication type
    switch( aIndicationType )
        {
        case TSmsDataCodingScheme::ESmsVoicemailMessageWaiting:
            {            
            if( aIndicationState == TSmsDataCodingScheme::ESmsIndicationActive )
                {
                NCN_RDEBUG( _L("CNcnMsvSessionObserver::HandleDCSIndicationL: voice mail active") );
                amount = CVoiceMailManager::EVMExistsButCountNotKnown;
                }
            else
            	{
                NCN_RDEBUG( _L("CNcnMsvSessionObserver::HandleDCSIndicationL: voice mail inactive") );            	
            	amount = CVoiceMailManager::ENoVoiceMails;	
            	}    
                
            //DCS indications don't seem to have alternative line support
            iVMManager.VMMessageReceived( MNcnMsgWaitingManager::ENcnMessageTypeVMLine1 , amount );
            break;
            }
        case TSmsDataCodingScheme::ESmsFaxMessageWaiting:
            {            
            if( aIndicationState == TSmsDataCodingScheme::ESmsIndicationActive )
                {
                NCN_RDEBUG( _L("CNcnMsvSessionObserver::HandleDCSIndicationL: fax message active") );                
                amount = KNcnDefaultMessagesWaiting;
                }
			else
				{
				NCN_RDEBUG( _L("CNcnMsvSessionObserver::HandleDCSIndicationL: fax message inactive") );						
				}                
                
            SetFaxMessagesWaiting( amount );
            break;
            }
        case TSmsDataCodingScheme::ESmsElectronicMailMessageWaiting:
            {
            if( aIndicationState == TSmsDataCodingScheme::ESmsIndicationActive )
                {
				NCN_RDEBUG( _L("CNcnMsvSessionObserver::HandleDCSIndicationL: email message active") );                
                amount = KNcnDefaultMessagesWaiting;
                }
			else
				{
				NCN_RDEBUG( _L("CNcnMsvSessionObserver::HandleDCSIndicationL: email message inactive") );	
				}                
                
            SetEMailMessagesWaiting( amount );
            break;
            }
        case TSmsDataCodingScheme::ESmsFaxOtherMessageWaiting:
            {            
            if( aIndicationState == TSmsDataCodingScheme::ESmsIndicationActive )
                {
				NCN_RDEBUG( _L("CNcnMsvSessionObserver::HandleDCSIndicationL: other message active") );                
                amount = KNcnDefaultMessagesWaiting;
                }
			else
				{
				NCN_RDEBUG( _L("CNcnMsvSessionObserver::HandleDCSIndicationL: other message inactive") ); 					
				}                
                
            SetOtherMessagesWaiting( amount );
            break;            
            }
        default:
            break;
        }
        
    // return the indication set status
    // baes on number of messages waiting
    NCN_RDEBUG( _L("CNcnMsvSessionObserver::HandleDCSIndicationL <<") );
    return ( amount != 0 );
    }

// ---------------------------------------------------------
// CNcnMsvSessionObserver::HandleSmsUserDataL
// ---------------------------------------------------------
//
CNcnMsvSessionObserver::TNcnInboxEntryType CNcnMsvSessionObserver::HandleSmsUserDataL(
    const CSmsUserData& aUserData )
    {
    NCN_RDEBUG(_L("CNcnMsvSessionObserver::HandleSmsUserDataL <<"));    
    // default to unknown
    TNcnInboxEntryType entryType = EUnknown;
    
    // count data elements
    TInt count = aUserData.NumInformationElements();
               
    // process each element
    while( count-- )
        {
        // extract information element
        CSmsInformationElement& informationElement =
            aUserData.InformationElement(count);
            
        // if element is message indication
        if ( informationElement.Identifier() ==
            CSmsInformationElement::ESmsIEISpecialSMSMessageIndication )
            {
            //Determine which resource ID to use
            const TDesC8& data = informationElement.Data();
            
            // extract message type
            TUint8 messageType = data[0];
            
            // extract number of messages waiting
            TUint8 messagesWaiting = data[1];

            // handle the element
            HandleSpecialSMSMessageIndicationL( messageType, messagesWaiting );
            
            // Check the store/discard -information from IE, so that we
            // know, what to do to message itself
            if( count == 0 )
                {
                if ( messageType & KNcnStoreIndicator )
                    {
                    entryType = EMessageWaitingStoreMessage;
                    }
                else
                    {
                    if( messagesWaiting )
                        {
                        entryType = EMessageWaitingDiscardSetMessage;
                        }
                    else
                        {
                        entryType = EMessageWaitingDiscardClearMessage;
                        }
                    }
                }
            }
        }
        
    // return the entry type        
    NCN_RDEBUG(_L("CNcnMsvSessionObserver::HandleSmsUserDataL <<"));    
    return entryType;
    }
    
// ---------------------------------------------------------
// CNcnMsvSessionObserver::HandleSpecialSMSMessageIndicationL
// ---------------------------------------------------------
//
void CNcnMsvSessionObserver::HandleSpecialSMSMessageIndicationL(
    TUint8 aMessageType,
    TUint8 aMessagesWaiting )
    {
    NCN_RDEBUG( _L("CNcnMsvSessionObserver::HandleSpecialSMSMessageIndicationL") );
    // Check from current message IEI, which type of indicator is provided
    // message type is masked with 01111111
    switch ( aMessageType & TSmsUtilities::ESpecialMessageTypeMask )
        {
        case TSmsUtilities::EVoiceMessageWaiting:
            //Special SMS indications don't seem to have alternative line support
            NCN_RDEBUG_INT( _L("CNcnMsvSessionObserver::HandleSpecialSMSMessageIndicationL: voice mails on line: %d"), aMessagesWaiting );
       		iVMManager.VMMessageReceived( MNcnMsgWaitingManager::ENcnMessageTypeVMLine1, aMessagesWaiting );
            break;

        case TSmsUtilities::EFaxMessageWaiting:
            NCN_RDEBUG_INT( _L("CNcnMsvSessionObserver::HandleSpecialSMSMessageIndicationL: fax messages waiting: %d"), aMessagesWaiting );        
            SetFaxMessagesWaiting( aMessagesWaiting );
            break;
            
        case TSmsUtilities::EEmailMessageWaiting:
            NCN_RDEBUG_INT( _L("CNcnMsvSessionObserver::HandleSpecialSMSMessageIndicationL: emails waiting: %d"), aMessagesWaiting );        
            SetEMailMessagesWaiting( aMessagesWaiting );
            break;
            
        default: // EOtherMessageWaiting
            NCN_RDEBUG_INT( _L("CNcnMsvSessionObserver::HandleSpecialSMSMessageIndicationL: other messages waiting: %d"), aMessagesWaiting );        
            SetOtherMessagesWaiting( aMessagesWaiting );
            break;
        }        
    NCN_RDEBUG(_L("CNcnMsvSessionObserver::HandleSpecialSMSMessageIndicationL <<"));    
    }
    
// ---------------------------------------------------------
// CNcnMsvSessionObserver::CheckForCPHSIndicationBitsL
// Check if the sms is a CPHS message waiting indicator and
// notify the system agent if it is.
// ---------------------------------------------------------
//
CNcnMsvSessionObserver::TNcnInboxEntryType
    CNcnMsvSessionObserver::CheckForCPHSIndicationBitsL(
        CMsvEntry&      aEntry,
        CSmsHeader&     aHeader )
    {
    NCN_RDEBUG(_L("CNcnMsvSessionObserver::CheckForCPHSIndicationBitsL >>"));    
    // CPHS Implementation of message waiting indicator (CPHS Phase 2)
    CSmsPDU &pdu = aHeader.Message().SmsPDU();
    TGsmSmsTelNumber address;
    pdu.ParsedToFromAddress(address);

    // we can't get the npi from the address.
    if( IsCphsMessage( address ) )// x000 000x constant value
        {
		EmptyCphsMessageAddressL( aEntry, aHeader );

        // Check the bits on first character of telephone number
        TInt indicatorSetMessage =
            HandleCphsMessageL( address.iTelNumber[KNcnCphsFirstChar] );

        // Now check if the SMS is a store message or a
        // discard message. The SMS is a store message
        // if there is only one byte present and the byte
        // has a value 01000000. Fetch the user data...
        if( pdu.UserDataPresent() )
            {
            CSmsBufferBase &buffer = aHeader.Message().Buffer();
            TBuf<1> oneChar(KEmptyChar);

            buffer.Extract(oneChar, 0, 1);
            if( buffer.Length() == 1 && oneChar == KSpaceChar )
                {
                if( indicatorSetMessage )
                    {
                    return EMessageWaitingDiscardSetMessage;
                    }
                else
                    {
                    return EMessageWaitingDiscardClearMessage;
                    }
                }
            else
                {
                return EMessageWaitingStoreMessage;
                }
            }
        else if( indicatorSetMessage )
            {
            return EMessageWaitingDiscardSetMessage;
            }
        else
            {
            return EMessageWaitingDiscardClearMessage;
            }
        }
    NCN_RDEBUG(_L("CNcnMsvSessionObserver::CheckForCPHSIndicationBitsL <<"));    
    return EUnknown;
    }

// ---------------------------------------------------------
// CNcnMsvSessionObserver::EmptyCphsMessageAddress
// Empties the sender address from a Cphs message.
// ---------------------------------------------------------
//
void CNcnMsvSessionObserver::EmptyCphsMessageAddressL(
        CMsvEntry&      aEntry,
        CSmsHeader&     aHeader )
	{
    NCN_RDEBUG(_L("CNcnMsvSessionObserver::EmptyCphsMessageAddressL >>"));
    TMsvEntry tentry = aEntry.Entry();
    TInt wasLocked = tentry.ReadOnly();

	if( wasLocked )
		{
		tentry.SetReadOnly( EFalse ); // unlock the entry for writing
		}

    // Empty the iDetails data
    tentry.iDetails.Set( KNullDesC );
    // Commit the change
    aEntry.ChangeL( tentry );
    // Empty the real adress
    aHeader.SetFromAddressL( KNullDesC );
    // Store the header
    CMsvStore* store = aEntry.EditStoreL();
    CleanupStack::PushL(store);
    aHeader.StoreL( *store );
    store->CommitL();
    CleanupStack::PopAndDestroy(store);

    // Setting the old state back
    tentry.SetReadOnly( wasLocked );
    NCN_RDEBUG(_L("CNcnMsvSessionObserver::EmptyCphsMessageAddressL <<"));
    }

// ---------------------------------------------------------
//  CNcnMsvSessionObserver::IsCphsMessage
//  Runs CPHS-checks for originator address field to determine
//  if message contains voicemail indication
// ---------------------------------------------------------
//
TBool CNcnMsvSessionObserver::IsCphsMessage(
    const TGsmSmsTelNumber& aOrigAddress ) const
    {
    // Determine if the length of TP-OA is qualifies for CPHS indication
    if( aOrigAddress.iTelNumber.Length() != KNcnCphsAddressLength )
        {
        // Address length is wrong, return immediately
        return EFalse;
        }

    // Get the Type of Address
    TGsmSmsTypeOfNumber oaTON = aOrigAddress.iTypeOfAddress.TON();

    // Get first two digits of phonenumber
    TUint oaDigits1 = aOrigAddress.iTelNumber[KNcnCphsFirstChar];

    // Get second two digits of phonenumber
    TUint oaDigits2 = aOrigAddress.iTelNumber[KNcnCphsSecondChar];

    // Determine if Type of Address has the constant CPHS values (11010000)
    TBool cphsTon = ( oaTON == EGsmSmsTONAlphaNumeric );

    // Determine if first two digits contains the voicemail
    // indicator (bits 4..2) and in last two digits,
    // bits 7 to 2 are all zeros
    TBool cphsType = ( oaDigits1 & KNcnCphsConstantMask ) == KNcnCphsFirstInfo;
    TBool cphsInd = ( oaDigits2 & KNcnCphsConstantMask ) == KNcnCphsSecondInfo;

    // Return the combined result of tests for final decision
    return ( cphsTon && cphsType && cphsInd );
    }

// ---------------------------------------------------------
//  CNcnMsvSessionObserver::HandleCphsMessage
//  Handles CPHS-voicemail notifications
// ---------------------------------------------------------
//
TInt CNcnMsvSessionObserver::HandleCphsMessageL( const TUint aFirstOctet )
    {
    NCN_RDEBUG(_L("CNcnMsvSessionObserver::HandleCphsMessageL >>"));
    // Get if set or clear message
    TBool typeSetMessage = aFirstOctet & KNcnCphsSet;
    TUint lineInfo = aFirstOctet & KNcnCphsLineMask;

    // Make sure the fields are correctly set
    if( lineInfo == KNcnCphsVmiLine1 ||
        lineInfo == KNcnCphsVmiLine2 )
        {                  
    	//The message is a SET message in line 1
        if( typeSetMessage == 1 && lineInfo == KNcnCphsVmiLine1 )
            {
            iVMManager.VMMessageReceived( MNcnMsgWaitingManager::ENcnMessageTypeVMLine1, CVoiceMailManager::EVMExistsButCountNotKnown );
            }
            
      	//The message is a CLEAR message in line 1
        else if( typeSetMessage != 1 && lineInfo == KNcnCphsVmiLine1 )
            {
            iVMManager.VMMessageReceived( MNcnMsgWaitingManager::ENcnMessageTypeVMLine1, CVoiceMailManager::ENoVoiceMails );
            }
                       
     	//The message is a SET message in line 2       
        else if( typeSetMessage == 1 && lineInfo == KNcnCphsVmiLine2 )
            {
            iVMManager.VMMessageReceived( MNcnMsgWaitingManager::ENcnMessageTypeVMLine2, CVoiceMailManager::EVMExistsButCountNotKnown );
            }
            
     	//The message is a CLEAR message in line 2       
        else if( typeSetMessage != 1 && lineInfo == KNcnCphsVmiLine2 )
            {
            iVMManager.VMMessageReceived( MNcnMsgWaitingManager::ENcnMessageTypeVMLine2, CVoiceMailManager::ENoVoiceMails );
            }   
        }

    // Return EFalse, when clear message
    NCN_RDEBUG(_L("CNcnMsvSessionObserver::HandleCphsMessageL <<"));
    return typeSetMessage;
    }

// ---------------------------------------------------------
//  CNcnMsvSessionObserver::GetTelNumber
// ---------------------------------------------------------
//
TGsmSmsTelNumber CNcnMsvSessionObserver::GetTelNumber(
    const CSmsMessage& aMessage ) const
    {
    // Get SMS PDU
    const CSmsPDU &pdu = aMessage.SmsPDU();

    // Prepare address and get address from header
    TGsmSmsTelNumber address;
    pdu.ParsedToFromAddress( address );

    return address;
    }

// ---------------------------------------------------------
//  CNcnMsvSessionObserver::CheckAndLaunchMCEL
// ---------------------------------------------------------
//
void CNcnMsvSessionObserver::CheckAndLaunchMCEL(
    const TMsvEntry& aEntry )
    {
    NCN_RDEBUG(_L("CNcnMsvSessionObserver::CheckAndLaunchMCEL >>"));
    // get session from session handler
    CMsvSession& msvSession = iModel->MsvSessionHandler().MsvSessionL();
    
    TBuf<CTelephony::KMaxTelNumberSize> number;
	TInt err = iModel->GetPSString(
		KPSUidTelRemotePartyInformation, KTelCLINumber, number );

    if( err == KErrNone )
        {
        NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver::CheckAndLaunchMCEL: CLI number access successful, telnumber: %s"), number.PtrZ() );

        // Get match value from Shared Data
        TInt sdMatchValue = KNcnMatchDefault;
		err = iModel->GetCRInteger(
			KCRUidTelConfiguration, KTelMatchDigits, sdMatchValue );

        if( err == KErrNone )
            {
            if ( sdMatchValue < KNcnMatchMin || sdMatchValue > KNcnMatchMax )
                {
                sdMatchValue = KNcnMatchDefault;
                }
            }

        // Compare arrived message number/ender to the possible active phonenumber.
        // Notice, that numbers can be on formats like 35840xxx and 040xxx,
		// so rightmost numbers have to be compared.
		if( number.Length() != 0 )
			{
			TBool match = EFalse;
			if( aEntry.iMtm == KUidMsgTypeMultimedia )
				{
				// Get MMS sender information
				CClientMtmRegistry* clientMtmRegistry =
				    CClientMtmRegistry::NewL( msvSession );
				CleanupStack::PushL( clientMtmRegistry );
				CMmsClientMtm* mMsClient = static_cast< CMmsClientMtm* >
				    ( clientMtmRegistry->NewMtmL( KUidMsgTypeMultimedia ) );
				CleanupStack::PushL( mMsClient );
				mMsClient->SwitchCurrentEntryL( aEntry.Id() );
				mMsClient->LoadMessageL();
				const TPtrC sender = mMsClient->Sender();
				NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver::CheckAndLaunchMCEL: MMS sender: %s"), sender.Ptr() );
				match = ( sender.Right( sdMatchValue ) == number.Right( sdMatchValue ) );
				CleanupStack::PopAndDestroy( 2, clientMtmRegistry );    // mMsClient, clientMtmRegistry
				}
			else
				{
				// Get SMS sender information
				const TPtrC sender = iTelNumber.iTelNumber;
				NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver::CheckAndLaunchMCEL: SMS number: %s"), iTelNumber.iTelNumber.PtrZ() );
				match = ( sender.Right( sdMatchValue ) == number.Right( sdMatchValue ) );
				}
			
			//Get the autolock status
			TInt autoLockStatus = CheckAutoLockStatus();
			
			NCN_RDEBUG_INT(_L("CheckAndLaunchMCEL: number match result = %d"), match );
        	NCN_RDEBUG_INT(_L("CheckAndLaunchMCEL: autolockStatus = %d"), autoLockStatus );
        
			if ( match && autoLockStatus != 1)
			    {
			    // open message
			    iModel->NcnUI().OpenMessageL( aEntry );
				}
            }
        }
      NCN_RDEBUG(_L("CNcnMsvSessionObserver::CheckAndLaunchMCEL <<"));
    }

// ---------------------------------------------------------
//  CNcnMsvSessionObserver::SetFaxMessagesWaiting
// ---------------------------------------------------------
//         
void CNcnMsvSessionObserver::SetFaxMessagesWaiting( TUint aAmount )
    {
	TBool enable = ( aAmount > 0 ) ? ETrue : EFalse;
	iModel->MsgWaitingManager().SetMessageCount( MNcnMsgWaitingManager::ENcnMessageTypeFax, aAmount, enable );				
    }
         
// ---------------------------------------------------------
//  CNcnMsvSessionObserver::SetEMailMessagesWaiting
// ---------------------------------------------------------
//
void CNcnMsvSessionObserver::SetEMailMessagesWaiting( TUint aAmount )
    {
/*    
    // default to messages waiting    
    TUint value = KNcnNewEmail;
                
    // change the default if there are now messages waiting
    if( !aAmount )
        {
        value = KNcnNoNewEmail;
        }

    // update key value
	iModel->NotifyPublishAndSubscribe(
			KNcnNewEmailCategory, KNcnNewEmailStatus, value );   
			
	// Store it as indicator
	iModel->StoreIndicatorStatus( KNcnNewEmailStatus, value );
*/

	TBool enable = ( aAmount > 0 ) ? ETrue : EFalse;
	iModel->MsgWaitingManager().SetMessageCount( MNcnMsgWaitingManager::ENcnMessageTypeEmail, aAmount, enable );	
    }

// ---------------------------------------------------------
//  CNcnMsvSessionObserver::SetOtherMessagesWaiting
// ---------------------------------------------------------
//         
void CNcnMsvSessionObserver::SetOtherMessagesWaiting( TUint aAmount )
    {
	TBool enable = ( aAmount > 0 ) ? ETrue : EFalse;
	iModel->MsgWaitingManager().SetMessageCount( MNcnMsgWaitingManager::ENcnMessageTypeOther, aAmount, enable );		
    }

// ---------------------------------------------------------
//  CNcnMsvSessionObserver::CheckAutoLockStatus
// ---------------------------------------------------------
//               
TInt CNcnMsvSessionObserver::CheckAutoLockStatus()
	{
	//Get the lock status from Central Repository
    TInt lockStatus(KErrNotFound);
    NCN_RDEBUG(_L("CNcnMsvSessionObserver: CheckAutoLockStatus >>"));
    CRepository* settingsRepository(NULL);
    TRAPD(err, settingsRepository = CRepository::NewL(KCRUidSecuritySettings));
    NCN_RDEBUG_INT(_L("CheckAndLaunchMCEL: CRepository::NewL err = %d"), err );
    
    if (err == KErrNone)
        {
        err = settingsRepository->Get(KSettingsAutolockStatus, lockStatus);
        NCN_RDEBUG_INT(_L("CheckAndLaunchMCEL: lockStatus = %d"), lockStatus );
        }
                    
    //Cleanup   
    delete settingsRepository;
    NCN_RDEBUG(_L("CNcnMsvSessionObserver: CheckAutoLockStatus <<"));
    return lockStatus;		
	}

// ---------------------------------------------------------
// CNcnMsvSessionObserver::IsUnread
// Check if entry is read or unread. Uses iUnreadMessages array.
// ---------------------------------------------------------
//	
TBool CNcnMsvSessionObserver::IsUnread(const TMsvEntry& aEntry)
	{
	if( iUnreadMessages->Find( aEntry.Id() ) != KErrNotFound )
		{
		//NCN_RDEBUG_INT( _L("CNcnMsvSessionObserver::CheckIfUnread: message (TMsvId %d) is unread"), aEntry.Id() );
		return ETrue;				
		}
	else
		{
		//NCN_RDEBUG_INT( _L("CNcnMsvSessionObserver::CheckIfUnread: message (TMsvId %d) is read"), aEntry.Id() );
		return EFalse;
		}			

	}
	
// ---------------------------------------------------------
// CNcnMsvSessionObserver::InitUnreadMessagesArrayL
// This method is called only once during phone boot.
// Its purpose is to add all unread and visible messages
// in Inbox to iUnreadMessages array. This means that
// those entries are treated as new and unread eventhough
// message might have been read and marked again as unread
// before the boot. 
// ---------------------------------------------------------
//	
void CNcnMsvSessionObserver::InitUnreadMessagesArrayL()
    {
    // get session from session handler
    CMsvSession& msvSession = iModel->MsvSessionHandler().MsvSessionL();
    
    NCN_RDEBUG( _L("CNcnMsvSessionObserver::InitUnreadMessagesArrayL") );
    
    // Check how many unread messages there are in the In-box.
    const TInt itemCount = iInboxFolder->Count();
    NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver::InitUnreadMessagesArrayL [%d]"), itemCount );
    TInt numberOfUnreadMessages = 0; // Note, This value only for logging purpose
    
    for( TInt index = 0; index < itemCount; index++ )
        {
        const TMsvEntry entry = (*iInboxFolder)[index];

        if( entry.iType.iUid == KUidMsvMessageEntryValue && 
        	entry.Visible() &&
        	entry.Unread() )
            {
            TInt err = iUnreadMessages->Append( entry.Id() );
            if ( err )
         	    {
         	    NCN_RDEBUG_INT(_L("CNcnMsvSessionObserver::InitUnreadMessagesArrayL - APPEND ERROR %d!"), err );
         	    }
            ++numberOfUnreadMessages;
            }
        }
    
    NCN_RDEBUG_INT( _L( "CNcnMsvSessionObserver::InitUnreadMessagesArrayL \
        :%d" ), numberOfUnreadMessages );
        
    NCN_RDEBUG( _L("CNcnMsvSessionObserver::InitUnreadMessagesArrayL <<") );
    }

// ---------------------------------------------------------
//  CNcnMsvSessionObserver::HandleMsvMediaChangedL
// ---------------------------------------------------------
//
void CNcnMsvSessionObserver::HandleMsvMediaChangedL( const TDriveNumber& /*aDriveNumber*/ )
    {
	NCN_RDEBUG(_L("CNcnMsvSessionObserver::HandleMsvMediaChangedL >> <<" ));
    }

// ---------------------------------------------------------
//  CNcnMsvSessionObserver::UpdateInboxDataNotifiersL
// ---------------------------------------------------------
//
void CNcnMsvSessionObserver::UpdateInboxDataNotifiersL()
    {
    TInt unreadMessages = 0;
    TInt notListenedAudioMessages = 0;
    CheckAmountOfUnreadMessagesInInboxL( unreadMessages, notListenedAudioMessages);
    
    iModel->NcnNotifier().SetNotification(
		        MNcnNotifier::ENcnMessagesNotification,
		        unreadMessages );
		        
	if( iModel->IsSupported( KNcnAudioMessaging ) )
		{
		iModel->NcnNotifier().SetNotification( 
    		MNcnNotifier::ENcnAudioMessagesNotification,
    		notListenedAudioMessages );
		}
    }
             
// ---------------------------------------------------------
//  CNcnMsvSessionObserver::PrintEntryDebugInformation
// ---------------------------------------------------------
//
#ifdef _DEBUG
void CNcnMsvSessionObserver::PrintEntryDebugInformation(const TMsvEntry&  aEntryToHandle)
	{
	NCN_RDEBUG(_L(" *** MESSAGE ANALYZE START ***" ));

	NCN_RDEBUG(_L(" * DATA *" ));
    NCN_RDEBUG_INT(_L(" MSG: Service ID = %d"), aEntryToHandle.iServiceId );
    NCN_RDEBUG_INT(_L(" MSG: Related ID = %d"), aEntryToHandle.iRelatedId );
    NCN_RDEBUG_INT(_L(" MSG: Type = %d"), aEntryToHandle.iType );
    NCN_RDEBUG_INT(_L(" MSG: MTM = %d"), aEntryToHandle.iMtm );
    NCN_RDEBUG_INT(_L(" MSG: Date = %d"), aEntryToHandle.iDate.Int64() );
    NCN_RDEBUG_INT(_L(" MSG: Size = %d"), aEntryToHandle.iSize );
    NCN_RDEBUG_INT(_L(" MSG: Error = %d"), aEntryToHandle.iError );
	NCN_RDEBUG_INT(_L(" MSG: BioType = %d"), aEntryToHandle.iBioType );
	NCN_RDEBUG_INT(_L(" MSG: Data 1 = %d"), aEntryToHandle.iMtmData1 );
	NCN_RDEBUG_INT(_L(" MSG: Data 2 = %d"), aEntryToHandle.iMtmData2 );
	NCN_RDEBUG_INT(_L(" MSG: Data 3 = %d"), aEntryToHandle.iMtmData3 );

	NCN_RDEBUG(_L(" * STATES *" ));

	NCN_RDEBUG_INT(_L(" MSG: Owner( %d )"), aEntryToHandle.Owner() );
	NCN_RDEBUG_INT(_L(" MSG: Deleted( %d )"), aEntryToHandle.Deleted() );
	NCN_RDEBUG_INT(_L(" MSG: Complete( %d )"), aEntryToHandle.Complete() );
	NCN_RDEBUG_INT(_L(" MSG: New( %d )"), aEntryToHandle.New() );
	NCN_RDEBUG_INT(_L(" MSG: Unread( %d )"), aEntryToHandle.Unread() );
	NCN_RDEBUG_INT(_L(" MSG: Failed( %d )"), aEntryToHandle.Failed() );
	NCN_RDEBUG_INT(_L(" MSG: Operation( %d )"), aEntryToHandle.Operation() );
	NCN_RDEBUG_INT(_L(" MSG: Visible( %d )"), aEntryToHandle.Visible() );
	NCN_RDEBUG_INT(_L(" MSG: Multiple Recipients( %d )"), aEntryToHandle.MultipleRecipients() );
	NCN_RDEBUG_INT(_L(" MSG: ReadOnly( %d )"), aEntryToHandle.ReadOnly() );
	NCN_RDEBUG_INT(_L(" MSG: Standard Folder( %d )"), aEntryToHandle.StandardFolder() );
	NCN_RDEBUG_INT(_L(" MSG: Attachment( %d )"), aEntryToHandle.Attachment() );
	NCN_RDEBUG_INT(_L(" MSG: Connected( %d )"), aEntryToHandle.Connected() );
	NCN_RDEBUG_INT(_L(" MSG: In Preparation( %d )"), aEntryToHandle.InPreparation() );
	NCN_RDEBUG_INT(_L(" MSG: Off peak( %d )"), aEntryToHandle.OffPeak() );
	NCN_RDEBUG_INT(_L(" MSG: Scheduled( %d )"), aEntryToHandle.Scheduled() );
	NCN_RDEBUG_INT(_L(" MSG: Pending Delete( %d )"), aEntryToHandle.PendingDelete() );

	NCN_RDEBUG(_L(" *** MESSAGE ANALYZE END ***" ));		
	}
#endif // _DEBUG

// End of file
