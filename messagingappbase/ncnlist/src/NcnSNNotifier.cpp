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
* Description:   Methods for CNcnSNNotifier class.
*
*/



// INCLUDE FILES
#include    "NcnDebug.h"
#include    "NcnSNNotifier.h"
#include    <AknSoftNotifier.h>
#include    <AknNotifyStd.h>
#include    <AknSoftNotificationParameters.h>
#include    <avkon.rsg>
#include    <Ncnlist.rsg>
#include    <aknSDData.h>
#include    <centralrepository.h>
#include    "NcnModel.h"
#include    "CVoiceMailManager.h"
#include "messaginginternalcrkeys.h"

// ================= DEFINITIONS ============================
#define KSpeedDialViewId TVwsViewId( TUid::Uid( 0x1000590A ), TUid::Uid( 0x02 ) )
#define KMceMailViewId TVwsViewId( TUid::Uid( 0x100058C5 ), TUid::Uid( 0x01 ) )
#define KMceAudioMessageViewId TVwsViewId(TUid::Uid(0x100058C5),TUid::Uid(0x01) )
// Conversational messaging UIDs.
#define KConversationApplicationViewUid TVwsViewId(TUid::Uid(0x2002A540),TUid::Uid(0x01))
const TInt KConversationListViewUid  = 0x01 ;

// ================= LOCAL CONSTANTS ========================
namespace
    {
    _LIT( KNcnResourceFile, "z:\\resource\\Ncnlist.rSC" );
    const TInt KNcnNotificationPriority = 2200;
    const TUid KNcnDefaultMessageUid = { 1 };
    const TUid KNcnLine2MessageUid = { 2 };
    const TUid KNcnAudioMessageUid = { 3 };
    _LIT8( KNcnExternalLaunch, "outside" );
    const TUint KNcnExternalizedNumberMaxLength = 8;
    }
    
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNcnSNNotifier default constructor
// ---------------------------------------------------------
//
CNcnSNNotifier::CNcnSNNotifier()
    {
    }


// ---------------------------------------------------------
// CNcnSNNotifier::~CNcnSNNotifier
// ---------------------------------------------------------
//
CNcnSNNotifier::~CNcnSNNotifier()
    {
    // Delete notifier
    delete iNotifier;
    iNotifier = NULL;
    if ( iMuiuSettings )
       {
       delete iMuiuSettings;
       iMuiuSettings = NULL;
       }
    }


// ---------------------------------------------------------
// CNcnSNNotifier::NewL
// ---------------------------------------------------------
//
CNcnSNNotifier* CNcnSNNotifier::NewL()
    {
    // Create the notifier instance
    CNcnSNNotifier* self = new (ELeave) CNcnSNNotifier();

    // Call the construct safely
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
        
    return self;
    }


// ---------------------------------------------------------
// CNcnSNNotifier::ConstructL
// ---------------------------------------------------------
//
void CNcnSNNotifier::ConstructL()
    {
    // Create notifier instance
    iNotifier = CAknSoftNotifier::NewL();
    // Muiu settings repository
    iMuiuSettings = CRepository::NewL(KCRUidMuiuSettings);
    }


// ---------------------------------------------------------
// CNcnSNNotifier::IdleState
// ---------------------------------------------------------
//
void CNcnSNNotifier::IdleState( TBool aCurrentState )
    {
    TRAP_IGNORE( DoInformIdleStateL( aCurrentState ) );
    }


// ---------------------------------------------------------
// CNcnSNNotifier::NewItems
// ---------------------------------------------------------
//
void CNcnSNNotifier::NewItems( const MNcnNotifier::TNcnNotificationType aNotificationType, const TInt aAmount )
    {
    //Convert to avkon type
    TAknSoftNotificationType aknType = ConvertNCNTypeToAvkonType(aNotificationType);

	//Get the secondary display id for this note
	SecondaryDisplay::TSecondaryDisplayNcnListDialogs dialogID = 
		ConvertNCNTypeToSecondaryDisplayType(aNotificationType);
	
    // Amount needs to be stored to "additional parameter"
    TBuf8<KNcnExternalizedNumberMaxLength> additionalData;
    additionalData.Num(aAmount);
    
    // Encapsulate the sent data
    CAknSDData* sdData = NULL;
    TRAPD( status, sdData = CAknSDData::NewL(
		    SecondaryDisplay::KCatNcnList, 
		    dialogID, 
		    additionalData) );
    
    // Send data to cover ui    
    if ( status == KErrNone && sdData != NULL)
	    {
	    iNotifier->SetSecondaryDisplayData( sdData ); //Takes ownership	
	    }
    
    // If there are problems with notifier, just catch the error and 
    // try to finish the other processes.
    TRAP( status, ShowNewItemsL( aknType, aAmount ) );
    
    NCN_RDEBUG_INT( _L("CNcnSNNotifier::NewItems: ShowNewItemsL returned %d"), status );    
    }

// ---------------------------------------------------------
// CNcnSNNotifier::DoInformIdleStateL
// ---------------------------------------------------------
//
void CNcnSNNotifier::DoInformIdleStateL( TBool aCurrentState )
    {
    NCN_RDEBUG_INT(_L("CNcnSNNotifier::DoInformIdleStateL aCurrentState = %d" ), aCurrentState );

    // Inform the notifier component about new idle state status.
    iNotifier->SetIdleStateL( aCurrentState );
    }


// ---------------------------------------------------------
// CNcnSNNotifier::ShowNewItemsL
// ---------------------------------------------------------
//
void CNcnSNNotifier::ShowNewItemsL( const TAknSoftNotificationType aNotificationType, const TInt aAmount )
    {
    NCN_RDEBUG_INT2(_L("CNcnSNNotifier::ShowNewItemsL: aNotificationType %d, aAmount: %d" ), aNotificationType, aAmount );

    // Separate handling for voice mail notifications for now
    switch( aNotificationType )
        {
        case ECustomSoftNotification:
        	// Fallthrough
        case ENewMailNotification:
            // Fallthrough
        case EVoiceMailNotification:
            // Fallthrough
        case ESeparateVoiceMailOnLine1Notification:
            // Fallthrough
        case ESeparateVoiceMailOnLine2Notification:
            {
            ShowCustomNotificationL( aNotificationType, aAmount );
            break;
            }            
        default:
            {
            ShowDefaultNotificationL( aNotificationType, aAmount );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CNcnSNNotifier::ShowVoiceMailNotificationL
// ---------------------------------------------------------
//
void CNcnSNNotifier::ShowCustomNotificationL( const TAknSoftNotificationType aNotificationType, const TInt aAmount )
    {
    TBool amountKnown = ( aAmount <= CVoiceMailManager::EVMMaximumNumber );

    NCN_RDEBUG_INT2(_L("CNcnSNNotifier::ShowCustomNotificationL: aNotificationType %d, amount known: %d" ), aNotificationType, amountKnown );

    // fuzzy voicemail note and normal voice mail note should never exist at the same time. 
    // Always cancel the other before adding new note.
    if ( aNotificationType == EVoiceMailNotification
        || aNotificationType == ESeparateVoiceMailOnLine1Notification
        || aNotificationType == ESeparateVoiceMailOnLine2Notification )
        {
        CAknSoftNotificationParameters* params =
            CreateNotificationParametersLC( aNotificationType, !amountKnown );
        iNotifier->CancelCustomSoftNotificationL( *params );
        CleanupStack::PopAndDestroy( params );
        }

    // create notification parameters
    CAknSoftNotificationParameters* params =
        CreateNotificationParametersLC( aNotificationType, amountKnown );
        
    if( aAmount <= 0 )
        {
        iNotifier->CancelCustomSoftNotificationL( *params );
        }
    else
        {
        iNotifier->SetCustomNotificationCountL( *params, aAmount );
        }

    // destroy parameters
    CleanupStack::PopAndDestroy( params );
    }
     
// ---------------------------------------------------------
// CNcnSNNotifier::ShowDefaultNotificationL
// ---------------------------------------------------------
//
void CNcnSNNotifier::ShowDefaultNotificationL( const TAknSoftNotificationType aNotificationType, const TInt aAmount )
    {
    NCN_RDEBUG_INT2(_L("CNcnSNNotifier::ShowDefaultNotificationL aNotificationType %d, aAmount: %d" ), aNotificationType, aAmount );
    
    if( aAmount <= 0 )
        {
        iNotifier->CancelSoftNotificationL( aNotificationType );
        }
    else
        {
        TRAPD( err, iNotifier->SetNotificationCountL( aNotificationType, aAmount ) );
    	NCN_RDEBUG_INT(_L("CNcnSNNotifier::ShowDefaultNotificationL: SetNotificationCountL returned %d"  ), err );
    	User::LeaveIfError( err );
//      iNotifier->SetNotificationCountL( aNotificationType, aAmount );
        }
    }
// ---------------------------------------------------------
// CNcnSNNotifier::VoicemailNoteId
// ---------------------------------------------------------
//        
CAknSoftNotificationParameters* CNcnSNNotifier::VoicemailNoteParametersL( 
    const TAknSoftNotificationType aNotificationType, 
    const TBool aAmountKnown )
    {
    NCN_RDEBUG_INT2(_L("CNcnSNNotifier::VoicemailNoteParametersL: aNotificationType %d, aAmountKnown: %d" ), aNotificationType, aAmountKnown );
    
    // note is different only for voice mails, if amount is known or not known.
    TUid customMessageId = KNcnDefaultMessageUid; // default
    TInt groupId(0);
    TInt noteId(0);
    switch( aNotificationType )
        {
        case EVoiceMailNotification:
            {
            noteId = aAmountKnown ? R_NCN_VOICE_MAIL_NOTE : R_NCN_VOICE_MAILS_UNK_AMOUNT;
            groupId = aAmountKnown ? R_NCN_VOICE_MAIL_GROUPED : R_NCN_VOICE_MAILS_UNK_AMOUNT_GROUPED;
            break;
            }
        case ESeparateVoiceMailOnLine1Notification:
            {
            noteId = aAmountKnown ? R_NCN_VOICE_MAIL_ON_LINE1_NOTE : R_NCN_VOICE_MAILS_UNK_AMOUNT_ON_LINE1;
            groupId = aAmountKnown ? R_NCN_VOICE_MAIL_ON_LINE1_GROUPED : R_NCN_VOICE_MAILS_UNK_AMOUNT_ON_LINE1_GROUPED;
            break;
            }
        case ESeparateVoiceMailOnLine2Notification:
            {
            noteId = aAmountKnown ? R_NCN_VOICE_MAIL_ON_LINE2_NOTE : R_NCN_VOICE_MAILS_UNK_AMOUNT_ON_LINE2;
            groupId = aAmountKnown ? R_NCN_VOICE_MAIL_ON_LINE2_GROUPED : R_NCN_VOICE_MAILS_UNK_AMOUNT_ON_LINE2_GROUPED;
            customMessageId = KNcnLine2MessageUid;
            break;
            }
        default:
            {
            ASSERT( EFalse );
            break;
            }
        }
        
        CAknSoftNotificationParameters* ret = CAknSoftNotificationParameters::NewL(
                KNcnResourceFile,
                noteId,
                KNcnNotificationPriority,
                R_AVKON_SOFTKEYS_LISTEN_EXIT,
                CAknNoteDialog::ENoTone,
                KSpeedDialViewId,
                customMessageId,
                EAknSoftkeyListen,
                KNcnExternalLaunch );
        ret->SetGroupedTexts( groupId );
        return ret;
    }
    
// ---------------------------------------------------------
// CNcnSNNotifier::CreateNotificationParametersLC
// ---------------------------------------------------------
//    
CAknSoftNotificationParameters* CNcnSNNotifier::CreateNotificationParametersLC(
    const TAknSoftNotificationType aNotificationType,
    TBool aAmountKnown )
    {
    NCN_RDEBUG_INT(_L("CNcnSNNotifier::CreateNotificationParametersLC: aNotificationType %d" ), aNotificationType );
    // instantiate parameters    
    CAknSoftNotificationParameters* ret = 0;
        
    // determine resource ids based on notification type
    switch( aNotificationType )
        {
        // Notification for audio message
        case ECustomSoftNotification:
        	{
       		TInt viewtype = 0; // Default is traditional Inbox
		    if ( iMuiuSettings )
		        {
			    // Read the messaging settings
		        iMuiuSettings->Get(KMuiuMceDefaultView,viewtype);
		        } 	 
        	 if ( viewtype == 1 ) // Launch conversations 
    	        {        	        
    	        ret = CAknSoftNotificationParameters::NewL(
                        KNcnResourceFile,
                        R_NCN_AUDIO_MESSAGE_NOTE,
                        KNcnNotificationPriority,
                        R_AVKON_SOFTKEYS_SHOW_EXIT,
                        CAknNoteDialog::ENoTone,
                        KConversationApplicationViewUid,
                        TUid::Uid(KConversationListViewUid),   
                        EAknSoftkeyShow,
                        KNullDesC8() );    	        
    	        }  
        	 else
        	    {
        	ret = CAknSoftNotificationParameters::NewL(
                KNcnResourceFile,
                R_NCN_AUDIO_MESSAGE_NOTE,
                KNcnNotificationPriority,
                R_AVKON_SOFTKEYS_SHOW_EXIT,
                CAknNoteDialog::ENoTone,
                KMceAudioMessageViewId,
                KNcnAudioMessageUid,
                EAknSoftkeyShow,
                KNullDesC8() );
        	    }        	
            ret->SetGroupedTexts( R_NCN_AUDIO_MESSAGE_GROUPED );
            break;
        	}
        case ENewMailNotification:
            {
            ret = CAknSoftNotificationParameters::NewL(
                KNcnResourceFile,
                R_NCN_EMAIL_NOTE,
                KNcnNotificationPriority,
                R_AVKON_SOFTKEYS_SHOW_EXIT,
                CAknNoteDialog::ENoTone,
                KMceMailViewId,
                KNcnDefaultMessageUid,
                EAknSoftkeyShow,
                KNullDesC8() );
            ret->SetGroupedTexts( R_NCN_EMAIL_GROUPED );
            break;
            }
        case EVoiceMailNotification:
        case ESeparateVoiceMailOnLine1Notification:
        case ESeparateVoiceMailOnLine2Notification:
            {
            ret = VoicemailNoteParametersL( aNotificationType, aAmountKnown );
            break;
            }
        default:
            {
            // Not found.
            User::Leave( KErrNotFound );
            break;
            }
        }
        
    CleanupStack::PushL( ret );
    return ret;
    }
    

// ---------------------------------------------------------
// CNcnSNNotifier::ConvertNCNTypeToAvkonType
// ---------------------------------------------------------
//   
TAknSoftNotificationType CNcnSNNotifier::ConvertNCNTypeToAvkonType( 
        	const MNcnNotifier::TNcnNotificationType aNotification ) const
	{
    // default to no notification
    TAknSoftNotificationType aknNotificationType = ENoSoftNotification;
            
    // Map ncn specific notification to Akn notification type and update
    // notification.
    // Note that only the messages with valid Akn notification need to be
    // mapped (ie. class0 needs no mapping)
    switch( aNotification )
        {
        case MNcnNotifier::ENcnMessagesNotification:
            {
            aknNotificationType = ENewMessagesNotification;
            break;
            }
	    case MNcnNotifier::ENcnAudioMessagesNotification:
	        {
	        aknNotificationType = ECustomSoftNotification;
	        break;
	        }
        case MNcnNotifier::ENcnEmailNotification:
            {
            aknNotificationType = ENewMailNotification;
            break;
            }
        case MNcnNotifier::ENcnVoiceMailNotification:
            {
            aknNotificationType = EVoiceMailNotification;
            break;
            }
        case MNcnNotifier::ENcnVoiceMailOnLine1Notification:
            {
            aknNotificationType = ESeparateVoiceMailOnLine1Notification;
            break;
            }
        case MNcnNotifier::ENcnVoiceMailOnLine2Notification:
            {
            aknNotificationType = ESeparateVoiceMailOnLine2Notification;
            break;
            }
        case MNcnNotifier::ENcnMissedCallsNotification:
            {
            aknNotificationType = EMissedCallsNotification;
            break;
            }
        default:
            {
            break;
            }
        }
        
    return aknNotificationType;
	}    
	
// ---------------------------------------------------------
// CNcnSNNotifier::ConvertNCNTypeToSecondaryDisplayType
// ---------------------------------------------------------
//   
SecondaryDisplay::TSecondaryDisplayNcnListDialogs CNcnSNNotifier::ConvertNCNTypeToSecondaryDisplayType( 
	const MNcnNotifier::TNcnNotificationType aNotification ) const
	{
    // default to no notification
    SecondaryDisplay::TSecondaryDisplayNcnListDialogs secDisplayDialog = 
    	SecondaryDisplay::ECmdNoNotification;
            
    //Get the corresbonding secondary display dialog id
    switch( aNotification )
        {
        case MNcnNotifier::ENcnMessagesNotification:
            {
            secDisplayDialog = SecondaryDisplay::ECmdMessagesNotification;
            break;
            }
        case MNcnNotifier::ENcnClass0MessageNotification:
            {
            secDisplayDialog = SecondaryDisplay::ECmdClass0MessageNotification;
            break;
            }
	    case MNcnNotifier::ENcnAudioMessagesNotification:
	        {
	        secDisplayDialog = SecondaryDisplay::ECmdAudioMessagesNotification;
	        break;
	        }
        case MNcnNotifier::ENcnEmailNotification:
            {
            secDisplayDialog = SecondaryDisplay::ECmdEmailNotification;
            break;
            }
        case MNcnNotifier::ENcnVoiceMailNotification:
            {
            secDisplayDialog = SecondaryDisplay::ECmdVoiceMailNotification;
            break;
            }
        case MNcnNotifier::ENcnVoiceMailOnLine1Notification:
            {
            secDisplayDialog = SecondaryDisplay::ECmdVoiceMailOnLine1Notification;
            break;
            }
        case MNcnNotifier::ENcnVoiceMailOnLine2Notification:
            {
            secDisplayDialog = SecondaryDisplay::ECmdVoiceMailOnLine2Notification;
            break;
            }
        case MNcnNotifier::ENcnMissedCallsNotification:
            {
            secDisplayDialog = SecondaryDisplay::ECmdMissedCallsNotification;
            break;
            }
        default:
            {
            break;
            }
        }
        
    return secDisplayDialog;		
	}
//  End of File
