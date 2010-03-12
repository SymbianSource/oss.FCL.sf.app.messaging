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
* Description:   Methods for CNcnNotifier class.
*
*/



// INCLUDE FILES
#include    <centralrepository.h>
#include    <coreapplicationuisdomainpskeys.h>
#include    <sacls.h>
#include    <MessagingDomainCRKeys.h>

#include    "CNcnNotifier.h"
#include    "NcnModelBase.h"
#include    "MNcnUI.h"
#include 	"MNcnMsgWaitingManager.h"


// ================= LOCAL CONSTANTS =======================

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
//  CNcnNotifier::CNcnNotifier
// ----------------------------------------------------
//
CNcnNotifier::CNcnNotifier( MNcnUI& aNcnUI, CNcnModelBase& aModel ) :
    iNcnUI( aNcnUI ),
    iModel( aModel )
    {
    // clear notification amount array
    for( int i = 0; i < ENcnNoNotification; i++ )
        {
        iNotificationAmounts[ i ] = 0;
        }
    }

// ----------------------------------------------------
//  CNcnNotifier::ConstructL
// ----------------------------------------------------
//
void CNcnNotifier::ConstructL()
    {
    
       TInt state; 
       CRepository* repository = NULL;
       iDisplayLightstate = 63;
       TRAPD( err, repository = CRepository::NewL( KCRUidMuiuMessagingConfiguration ) );
       if( err == KErrNone && repository != NULL )
           {
           CleanupStack::PushL( repository ); 
           
           err = repository->Get( KMuiuDisplayLightsConfiguration, state );
           NCN_RDEBUG_INT( _L("CNcnNotifier::ConstructL() -  iDisplayLightstate: %d"), state );
           if(err == KErrNone)
               iDisplayLightstate = state;
           }
       CleanupStack::PopAndDestroy( repository );
    
    }
    
// ----------------------------------------------------
//  CNcnNotifier::NewL
// ----------------------------------------------------
//
CNcnNotifier* CNcnNotifier::NewL( MNcnUI& aNcnUI, CNcnModelBase& aModel )
    {
    CNcnNotifier* self = new (ELeave) CNcnNotifier( aNcnUI, aModel );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ----------------------------------------------------
//  CNcnNotifier::~CNcnNotifier
// ----------------------------------------------------
//
CNcnNotifier::~CNcnNotifier()
    {
    }

// ----------------------------------------------------
//  CNcnNotifier::SetNotification
// ----------------------------------------------------
//
void CNcnNotifier::SetNotification(
                TNcnNotificationType aNotificationType,
                TUint aAmount,
				TBool aIcon,
				TBool aTone,
				TBool aNote )
    {
    NCN_RDEBUG_INT( _L("CNcnNotifier::SetNotification() -  aNotificationType: %d"), aNotificationType );
    
    // if notification is one of the specified notifications
    // (ENcnNoNotification is a special case with no amount)
    if( aNotificationType != ENcnNoNotification )
        {        
        NCN_RDEBUG_INT( _L("CNcnNotifier::SetNotification() -  prev: %d"), iNotificationAmounts[ aNotificationType ] );
        NCN_RDEBUG_INT( _L("CNcnNotifier::SetNotification() -  curr: %d"), aAmount );

        // update notification only if amount changes
        if( iNotificationAmounts[ aNotificationType ] != aAmount || aAmount == 0 ||
            aNotificationType == MNcnNotifier::ENcnVoiceMailNotification || 
            aNotificationType == MNcnNotifier::ENcnVoiceMailOnLine1Notification || 
            aNotificationType == MNcnNotifier::ENcnVoiceMailOnLine2Notification )
            {
            // store previous amount
            TUint previousAmount = iNotificationAmounts[ aNotificationType ];
            
            // change notification amount
            iNotificationAmounts[ aNotificationType ] = aAmount;
            
            // update notification
            HandleNotificationChange(
                aNotificationType,
                previousAmount,
                aAmount,
                aIcon,
                aTone,
                aNote );
            }        
        }
    }

// ----------------------------------------------------
//  CNcnNotifier::NotificationAmount
// ----------------------------------------------------
//
TUint CNcnNotifier::NotificationAmount(
    TNcnNotificationType aNotificationType )
    {
    TUint amount = 0;
    
    // if notification is one of the specified notifications
    // (ENcnNoNotification is a special case with no amount)
    if( aNotificationType != ENcnNoNotification )
        {
        amount = iNotificationAmounts[ aNotificationType ];        
        }
        
    return amount;
    }

      									
// ----------------------------------------------------
//  CNcnNotifier::HandleNotificationChange
// ----------------------------------------------------
//    
void CNcnNotifier::HandleNotificationChange(
    TNcnNotificationType aNotificationType,
    TUint aPreviousAmount,
    TUint aCurrentAmount,
	TBool aIcon,
	TBool aTone,
	TBool aNote )
    {
    TBool displaySN = EFalse;
    NCN_RDEBUG_INT( _L("CNcnNotifier::HandleNotificationChange() -  notifType: %d"), aNotificationType );
    
    
    
    // handles voice mail note as the correct amount of new
    // voice mail messages isn't known
    TBool amountUnknown( EFalse );
        
    // determine notification type
    switch( aNotificationType )
        {
        case MNcnNotifier::ENcnClass0MessageNotification:
            {
            HandleClass0MessageNotificationChange(
                aPreviousAmount,
                aCurrentAmount );            
            break;
            }
        case MNcnNotifier::ENcnMessagesNotification:
            {
            HandleMessageNotificationChange(
                aPreviousAmount,
                aCurrentAmount );
            break;
            }
        case MNcnNotifier::ENcnAudioMessagesNotification:
        	{
        	HandleAudioMessageNotificationChange(
                aPreviousAmount,
                aCurrentAmount );                
            break;
            }
        case MNcnNotifier::ENcnVoiceMailNotification:
        case MNcnNotifier::ENcnVoiceMailOnLine1Notification:
        case MNcnNotifier::ENcnVoiceMailOnLine2Notification:
            {
        	amountUnknown = ETrue;
        	
            HandleVoiceMailNotificationChange(
                aPreviousAmount,
                aCurrentAmount );
            break;
            }

        case MNcnNotifier::ENcnEmailNotification:
        	{
        	//Email define which notes they want
        	//others don't define it ATM.
            HandleEMailNotificationChange(
                aPreviousAmount,
                aCurrentAmount,
				aIcon,
				aTone );
            break;
            }    
        default:
            {
            break;
            }
        }
    TRAP_IGNORE( displaySN = CheckSNStatusL( aNotificationType ));
    TBool DisplayLight = CheckDLStatus( aNotificationType );
    NCN_RDEBUG_INT( _L("CNcnNotifier::HandleNotificationChange() -  displaySN: %d"), displaySN );		

    // note is updated only if the count increases
    // or if count remains the same as is the case with voice mail messages,
    // where we don't know the actual message amount
    if( ( aCurrentAmount > aPreviousAmount ) 
            && displaySN 
            && aNote )
        {
            if(DisplayLight)
            {
            NCN_RDEBUG( _L( "CNcnNotifier::HandleNotificationChange - calling Flash Display" ) );
            iNcnUI.FlashDisplay();
            }
            iNcnUI.UpdateSoftNotification( aNotificationType, aCurrentAmount );	
        }
    else 
        if( amountUnknown // This is ETrue only with voicemails
            && displaySN
            && aCurrentAmount > 0 
        )
        {
            if(DisplayLight)
            {
            NCN_RDEBUG( _L( "CNcnNotifier::HandleNotificationChange - calling Flash Display" ) );
            iNcnUI.FlashDisplay();
            }
            iNcnUI.UpdateSoftNotification( aNotificationType, aCurrentAmount );	
        }
	//If the count decreases we must wipe it out
	else
		{
		iNcnUI.UpdateSoftNotification( aNotificationType, 0 );				
		}
    }

// ----------------------------------------------------
//  CNcnNotifier::HandleClass0MessageNotificationChange
// ----------------------------------------------------
//    
void CNcnNotifier::HandleClass0MessageNotificationChange(
            TUint /*aPreviousAmount*/,
            TUint /*aCurrentAmount*/ )
    {
    // alert tone is always played for class0
    iNcnUI.PlayMessageAlertTone();
    
    // Special: since we don't need to keep track of the amount of
    // class0 messages reset in now.
    iNotificationAmounts[ MNcnNotifier::ENcnClass0MessageNotification ] = 0;
    }
    
// ----------------------------------------------------
//  CNcnNotifier::HandleMessageNotificationChange
// ----------------------------------------------------
//        
void CNcnNotifier::HandleMessageNotificationChange(
            TUint aPreviousAmount,
            TUint aCurrentAmount )
    {
    // update message icon
    UpdateMessageIcon( aCurrentAmount );
    
    // play message alert tone if there are new messages
    if( aCurrentAmount > aPreviousAmount )
        {
        iNcnUI.PlayMessageAlertTone();
        }
    }

// ----------------------------------------------------
//  CNcnNotifier::HandleAudioMessageNotificationChange
// ----------------------------------------------------
//        
void CNcnNotifier::HandleAudioMessageNotificationChange(
            TUint aPreviousAmount,
            TUint aCurrentAmount )
    {    
    // update message icon
    UpdateMessageIcon( aCurrentAmount );
    
    // play message alert tone if there are new audio messages
    if( aCurrentAmount > aPreviousAmount )
        {
        iNcnUI.PlayMessageAlertTone();
        }
    }

// ----------------------------------------------------
//  CNcnNotifier::HandleVoiceMailNotificationChange
// ----------------------------------------------------
//            
void CNcnNotifier::HandleVoiceMailNotificationChange(
            TUint aPreviousAmount,
            TUint aCurrentAmount )
    {
    // play message alert tone always when messages are
    // received and the count of them is more than 0
    if( aCurrentAmount > 0 && aCurrentAmount > aPreviousAmount )
        {
        iNcnUI.PlayMessageAlertTone();
        }
    }

// ----------------------------------------------------
//  CNcnNotifier::HandleEMailNotificationChange
// ----------------------------------------------------
//            
void CNcnNotifier::HandleEMailNotificationChange(
            TUint aPreviousAmount,
            TUint aCurrentAmount,
			TBool aIcon,
			TBool aTone  )
    {        
    // update icon
    if ( aIcon == TRUE )
	    {
	    UpdateEmailIcon( aCurrentAmount );   	
	    }
        
    // play email alert tone if there are new messages
    if( aCurrentAmount > aPreviousAmount && aTone == TRUE )
        {
        iNcnUI.PlayEMailAlertTone();
        }
    }

// ----------------------------------------------------
//  CNcnNotifier::UpdateEmailIcon
//  Updates email message indicator. It does not update
//  remote mailbox email message indicator. 
// ----------------------------------------------------
//    
void CNcnNotifier::UpdateEmailIcon( TInt aAmount )
    {
    // default to messages waiting    
    TUint value = ECoreAppUIsNewEmail;
                
    // change the default if there are now messages waiting
    if( !aAmount )
        {
        value = ECoreAppUIsNoNewEmail;
        }

    // update key value. SysApp will show @ icon based on this value
	iModel.NotifyPublishAndSubscribe(
            KPSUidCoreApplicationUIs, KCoreAppUIsNewEmailStatus, value );   
			
    }

// ----------------------------------------------------
//  CNcnNotifier::UpdateMessageIcon
// ----------------------------------------------------
//
void CNcnNotifier::UpdateMessageIcon( TInt aAmount )
    {
    // default to inbox empty
    TInt state = ESAInboxEmpty;
    
    // if there are any messages to be notified of
	if( ( iModel.IsSupported( KNcnAudioMessaging ) &&
		( iNotificationAmounts[ ENcnMessagesNotification ] > 0 ||
    	  iNotificationAmounts[ ENcnAudioMessagesNotification ] > 0 ))
    	  || aAmount )
		{
		NCN_RDEBUG( _L( "CNcnNotifier::UpdateMessageIcon - setting message icon on " ) );
		state = ESADocumentsInInbox;
	    }
    else
        {
        NCN_RDEBUG( _L( "CNcnNotifier::UpdateMessageIcon - setting UpdateMessageIcon icon off " ) );
        }
        
    // set icon state through P&S variable
    iModel.NotifyPublishAndSubscribe(
        KUidSystemCategory,
        KUidInboxStatusValue,
        state );        
    }
    
TBool CNcnNotifier::CheckSNStatusL( const TNcnNotificationType& aNotificationType ) const
    {
    NCN_RDEBUG_INT( _L("CNcnNotifier::CheckSNStatusL() -  aNotificationType: %d"), aNotificationType );
    TUint32 flag;
    
    switch ( aNotificationType )
        {
        case ENcnMessagesNotification:
            {
            flag = KMuiuNewMessageSNFlag;
            break;
            }
        case ENcnEmailNotification:
            {
            flag = KMuiuNewEmailSNFlag;
            break;
            }
        case ENcnVoiceMailNotification:
        	// Fallthrough
        case ENcnVoiceMailOnLine1Notification:
        	// Fallthrough
        case ENcnVoiceMailOnLine2Notification:
            {
            flag = KMuiuNewVoiceMailSNFlag;
            break;
            }
        case ENcnMissedCallsNotification:
            {
            flag = KMuiuMissedCallSNFlag;
            break;
            }
        case ENcnInstantMessagesNotification:
            {
            flag = KMuiuInstantMessageSNFlag;
            break;
            }
        case ENcnAudioMessagesNotification:
            {
            flag = KMuiuAudioMessageSNFlag;
            break;
            }        
        case ENcnClass0MessageNotification:
        	// Fallthrough
        case ENcnNoNotification:
        	// Fallthrough
        default:
            return ETrue; // This is already default value for displaying SN  
        }
        
    TInt state; 
    CRepository* repository = NULL;
    
    TRAPD( err, repository = CRepository::NewL( KCRUidMuiuMessagingConfiguration ) );
    if( err == KErrNone && repository != NULL )
	    {
	    CleanupStack::PushL( repository ); // CSI: 42 # Should not leave
	    
		err = repository->Get( KMuiuSoftNotificationConfiguration, state );
		NCN_RDEBUG_INT( _L("CNcnNotifier::CheckSNStatusL() -  state: %d"), state );
		
		TBool result( EFalse );
		if( err == KErrNone && ( state & flag ) == flag )		
			{
			result = ETrue;				
			}
		else if( err != KErrNone )				
			{
			// soft note is displayed if we fail to get SN config state
			result = ETrue;				
			}
							
		CleanupStack::PopAndDestroy( repository );
		return result;
	    }
	else
		{
		return ETrue;			
		}	    
    }
    


TBool CNcnNotifier::CheckDLStatus( const TNcnNotificationType& aNotificationType ) const
    {
    NCN_RDEBUG_INT( _L("CNcnNotifier::CheckDLStatus() -  aNotificationType: %d"), aNotificationType );
    TUint32 flag;
    
    switch ( aNotificationType )
        {
        case ENcnMessagesNotification:
            {
            flag = KMuiuDLNewMessageFlag;
            break;
            }
        case ENcnEmailNotification:
            {
            flag = KMuiuDLNewEmailFlag;
            break;
            }
        case ENcnVoiceMailNotification:
            // Fallthrough
        case ENcnVoiceMailOnLine1Notification:
            // Fallthrough
        case ENcnVoiceMailOnLine2Notification:
            {
            flag = KMuiuDLNewVoiceMailFlag;
            break;
            }
        case ENcnMissedCallsNotification:
            {
            flag = KMuiuDLMissedCallFlag;
            break;
            }
        case ENcnInstantMessagesNotification:
            {
            flag = KMuiuDLInstantMessageFlag;
            break;
            }
        case ENcnAudioMessagesNotification:
            {
            flag = KMuiuDLAudioMessageFlag;
            break;
            }        
        case ENcnClass0MessageNotification:
            // Fallthrough
        case ENcnNoNotification:
            // Fallthrough
        default:
            return ETrue; // This is already default value for displaying SN  
        }
  
    if((iDisplayLightstate & flag ) == flag)
        return ETrue;
    else
        return EFalse;
            
          
    }
    
//  End of File

