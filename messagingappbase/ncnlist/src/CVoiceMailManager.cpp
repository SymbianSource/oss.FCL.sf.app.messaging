/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Handles the notifications for voice mail messages
*			   : Keeps track about the number of voice mail messages
*			   : Has support for alternative line (ALS)
*
*/



// INCLUDE FILES
#include  <centralrepository.h>
#include  <messagingvariant.hrh>      	// For CR key handling local variation flags
#include  <centralrepository.h>			// For CR key handling
#include  <NcnListInternalCRKeys.h>		// For CR key handling
#include  <RSSSettings.h>				// For ALS detection
#include  <startupdomainpskeys.h>
#include  <MessagingDomainCRKeys.h>

#include "NcnDebug.h"
#include "NcnCRHandler.h"
#include "CVoiceMailManager.h"
#include "CNcnNotifier.h"
#include "NcnModelBase.h"
#include "MNcnMsgWaitingManager.h"


// -----------------------------------------------------------------
// CVoiceMailManager::NewL
// -----------------------------------------------------------------
CVoiceMailManager* CVoiceMailManager::NewL( CNcnModelBase& aModel )
    {
    CVoiceMailManager* self = new (ELeave) CVoiceMailManager( aModel );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------
// CVoiceMailManager::~CVoiceMailManager
// -----------------------------------------------------------------
CVoiceMailManager::~CVoiceMailManager()
    {
    }

// -----------------------------------------------------------------
// CVoiceMailManager::CVoiceMailManager
// -----------------------------------------------------------------
CVoiceMailManager::CVoiceMailManager( CNcnModelBase& aModel ) :
	iIsALSSupported(EFalse),
	iModel( aModel )
    {
	// EMPTY
    }

// -----------------------------------------------------------------
// CVoiceMailManager::ConstructL
// -----------------------------------------------------------------
void CVoiceMailManager::ConstructL( )
    {
	NCN_RDEBUG( _L("CVoiceMailManager : Constructing") );
	
	//Clear soft notifications	that might be drawn automatically by avkon
	//TSW Error TWOK-6PNT26	
	ClearVoiceMailSoftNotes();
	//clear Voicemail indicators only when KMuiuSupressAllNotificationConfiguration(VVM) is enabled.
	if(CheckSupressNotificationSettingL())
		{	
     	iModel.MsgWaitingManager().SetIndicator( MNcnMsgWaitingManager::ENcnIndicatorVMLine1, EFalse );
     	iModel.MsgWaitingManager().SetIndicator( MNcnMsgWaitingManager::ENcnIndicatorVMLine2, EFalse );
		}
 	//Get SIM change status at the startup. If the SIM will change after
 	//boot-up we get notifed by the Model so no need to subscribe for this key
 	//TSW ID EHCN-6NRAZE
 	TInt simValue = 0;
	TInt status = RProperty::Get( KPSUidStartup, KPSSimChanged, simValue );
	NCN_RDEBUG_INT2(_L("CVoiceMailManager : Requested SIM change! status: %d value: %d"), status, simValue );
	    	    
	//Handle SIM change in a separate method
	//no need to continue in constructor after this
    if( status == KErrNone && simValue == ESimChanged )
	    {
	    NotifyAboutSIMChange();
	    return;
	    }

	//Get the ALS status
	iIsALSSupported = UpdateALSStatus();
		        
	///////////////////////////////////////////////////////////////////////////////////////
	// Soft notification must be drawn and the right icon triggered at the boot 
	// if there are active voice mail messages waiting
	///////////////////////////////////////////////////////////////////////////////////////
    }

// ---------------------------------------------------------
// CVoiceMailManager::VMReceived
// Received a voice mail message to line 1 or 2
// ---------------------------------------------------------
//
void CVoiceMailManager::VMMessageReceived( MNcnMsgWaitingManager::TNcnMessageType aLineType, TUint aAmount )
	{
	//Check the value. It must be between 0 - 254 (0xfe) or fuzzy note will be used
	if( aAmount > EVMMaximumNumber )
		{
		aAmount	= EVMExistsButCountNotKnown;
		}
		
	//The amount might be an exact value 0-254 or a special value EVMExistsButCountNotKnown
	iModel.MsgWaitingManager().SetMessageCount( aLineType, aAmount, EFalse );				
	
	//Update notifications
	UpdateVMNotifications();	
	}


// ---------------------------------------------------------
// CVoiceMailManager::NotifyAboutSIMChange()
// SIM has changed so we will need to erase old notes.
// Model uses this method to notify voice mail manager
// We also use this in the ConstructL if we notice that
// the SIM has changed in boot-up
// ---------------------------------------------------------
//
void CVoiceMailManager::NotifyAboutSIMChange()
	{
	NCN_RDEBUG(_L("CVoiceMailManager::NotifyAboutSIMChange - SIM has changed!"));

	//Get the ALS status. ALS status can change if SIM changes
	iIsALSSupported = UpdateALSStatus();
	
	//Update notifications
	UpdateVMNotifications();
	}
	
// ---------------------------------------------------------
// CVoiceMailManager::HandlePropertyChangedL()
// Handles the subscribed PS property changes.
// ---------------------------------------------------------
//
void CVoiceMailManager::HandlePropertyChangedL( const TUid& /*aCategory*/, TInt /*aKey*/ )
    {
    // Empty
    }

// ---------------------------------------------------------
// CVoiceMailManager::ClearVoiceMailSoftNotes
// In boot-up Avkon automatically draws the soft notes that
// were present at the time of the shut down. This is not
// wanted for the voice mails so we manually null them during
// the boot.
// ---------------------------------------------------------
//
void CVoiceMailManager::ClearVoiceMailSoftNotes()
	{
    NCN_RDEBUG(_L("CVoiceMailManager::ClearVoiceMailSoftNotes") );
	iModel.NcnNotifier().SetNotification( MNcnNotifier::ENcnVoiceMailNotification, 0 );
	iModel.NcnNotifier().SetNotification( MNcnNotifier::ENcnVoiceMailOnLine1Notification, 0 );
	iModel.NcnNotifier().SetNotification( MNcnNotifier::ENcnVoiceMailOnLine2Notification, 0 );	
	}


// ---------------------------------------------------------
// Indication message updates status only for one line at a time.
// Both lines need to be updated.
// ---------------------------------------------------------
//
void CVoiceMailManager::UpdateNoteAndIndicationWithALS( TInt aVoiceMailsInLine1,  TInt aVoiceMailsInLine2 )
    {
    //Soft notifications
    NCN_RDEBUG_INT(_L("CVoiceMailManager : %d voice mails in line 1 (ALS supported)"), aVoiceMailsInLine1 );
    NCN_RDEBUG_INT(_L("CVoiceMailManager : %d voice mails in line 2 (ALS supported)"), aVoiceMailsInLine2 );
    
    if(!CheckSupressNotificationSettingL())
        {
        iModel.NcnNotifier().SetNotification( MNcnNotifier::ENcnVoiceMailOnLine1Notification, aVoiceMailsInLine1 );
        iModel.NcnNotifier().SetNotification( MNcnNotifier::ENcnVoiceMailOnLine2Notification, aVoiceMailsInLine2 );
        NCN_RDEBUG( _L("CVoiceMailManager: UpdateNoteAndIndicationWithALS With VVM off") );
        // Indications
        // SysApp checks ALS support and decides whether to use o_o or O_o (left O full), if ALS supported
        iModel.MsgWaitingManager().SetIndicator( MNcnMsgWaitingManager::ENcnIndicatorVMLine1, aVoiceMailsInLine1 ? ETrue : EFalse );
        iModel.MsgWaitingManager().SetIndicator( MNcnMsgWaitingManager::ENcnIndicatorVMLine2, aVoiceMailsInLine2 ? ETrue : EFalse );
        }
    }
            
// ---------------------------------------------------------
// Update soft notification and trigger icon drawing to
// reflect the current state of the lines
// ---------------------------------------------------------
//
void CVoiceMailManager::UpdateVMNotifications()
	{
	//We now need a conversion from our internally held voice mail count number
	//to a format that is used in soft notifications.
	//
	
	if ( !iModel.MsgWaitingManager().ConstructionReady() )
	    {
	    return;
	    }
	
	TUint voiceMailsInLine1 = 0;
	TUint voiceMailsInLine2 = 0;
	
	//Internally we use value EVMExistsButCountNotKnown to specify a case where the
	//exact count of the voice mails is not know. However, the soft notes are designed
	//so that voice mail strings are either exact or "fuzzy", meaning that the expression
	//used in the string does not imply the exact number.
	//The "fuzzy" string is the singular string in the voice mail's soft note. To
	//use the singular form of the string we need to give voice mail count 1 in case the
	//internal value is EVMExistsButCountNotKnown.
	TBool line1(EFalse);
	TBool line2(EFalse);
	
	iModel.MsgWaitingManager().GetIndicator( MNcnMsgWaitingManager::ENcnIndicatorVMLine1, line1 );
	iModel.MsgWaitingManager().GetMessageCount( MNcnMsgWaitingManager::ENcnMessageTypeVMLine1, voiceMailsInLine1 );
	NCN_RDEBUG_INT2(_L("CVoiceMailManager : line 1 indicator %d, count %d"), line1, voiceMailsInLine1 );
	
	if ( line1 && voiceMailsInLine1 == 0)
	    {
	    voiceMailsInLine1 = EVMExistsButCountNotKnown;
	    }
	
	iModel.MsgWaitingManager().GetIndicator( MNcnMsgWaitingManager::ENcnIndicatorVMLine2, line2 );	
	iModel.MsgWaitingManager().GetMessageCount( MNcnMsgWaitingManager::ENcnMessageTypeVMLine2, voiceMailsInLine2 );	
	NCN_RDEBUG_INT2(_L("CVoiceMailManager : line 2 indicator %d, count %d"), line2, voiceMailsInLine2 );
	
	if ( line2 && voiceMailsInLine2 == 0)
	    {
	    voiceMailsInLine2 = EVMExistsButCountNotKnown;
	    }
				
	//The most common case. ALS is not supported in the terminal
	//Notifications show no line information. Only the information in
	//iVoiceMailCountInLine1 matters
	if( iIsALSSupported == EFalse && 
		voiceMailsInLine1 > 0)
		{
		NCN_RDEBUG_INT(_L("CVoiceMailManager : %d voice mails in line 1 (ALS not supported)"), voiceMailsInLine1 );
		
		 if(!CheckSupressNotificationSettingL())
            {
			NCN_RDEBUG( _L("CVoiceMailManager:SetIndicator and Notification ALS not supported With VVM off") );
            //Soft notification. The SN must not contain any reference to line numbers
            iModel.NcnNotifier().SetNotification( MNcnNotifier::ENcnVoiceMailNotification, voiceMailsInLine1 );	
    
            // SysApp checks ALS support and decides whether to use o_o or O_o (left O full), if ALS supported
            // ENcnIndicatorVMLine1 is mapped to KDisplayVoicemailActive in CNcnMsgWaitingManager
            iModel.MsgWaitingManager().SetIndicator( MNcnMsgWaitingManager::ENcnIndicatorVMLine1, ETrue );
            }
		}
	else if( iIsALSSupported == TRUE )		
	    {
	    UpdateNoteAndIndicationWithALS( voiceMailsInLine1, voiceMailsInLine2 );
	    }	

	//No voice mails waiting. Clear the messages
	else
		{
		NCN_RDEBUG( _L("CVoiceMailManager : No voice mails in line 1 nor in line 2") );
		
		//Clear soft notifications		
		ClearVoiceMailSoftNotes();
		
		iModel.MsgWaitingManager().SetIndicator( MNcnMsgWaitingManager::ENcnIndicatorVMLine1, EFalse );
		iModel.MsgWaitingManager().SetIndicator( MNcnMsgWaitingManager::ENcnIndicatorVMLine2, EFalse );				
		}	
	}
	
// -------------------------------------------------------------------
// Find out if alternative line subscription is active in the terminal
// -------------------------------------------------------------------
//
TBool CVoiceMailManager::UpdateALSStatus()
	{
	//Open the information settings
	RSSSettings settings;
	TInt status = settings.Open();
	NCN_RDEBUG_INT( _L("CVoiceMailManager.UpdateALSStatus() : Opened RSSSettings! status %d"), status );
			
	//Get the ALS information, 
	// ALS is supported, if:
	// 1. supported by the device (ALS PP enabled) 
	// 2. CSP enabled and SIM supports alternative service line (line2)
	TInt alsValue = KErrUnknown;
	status = settings.Get( ESSSettingsAls, alsValue );
	NCN_RDEBUG_INT2( _L("CVoiceMailManager.UpdateALSStatus() : Got ALS info Status: %d Value: %d"), status, alsValue);
	
	//Close the settings, they are no longer needed
	settings.Close();
		
	//Meaning of the ALS values
	// ESSSettingsAlsNotSupported - ALS not supported, always primary line.
	// ESSSettingsAlsPrimary - ALS supported, primary line selected.
	// ESSSettingsAlsAlternate - ALS supported, alternate line selected.
	
	//Value read OK and ALS not supported
	if( status == KErrNone && alsValue == ESSSettingsAlsNotSupported )
		{
		NCN_RDEBUG( _L("ALS value read properly! ALS not supported!") );	
		return EFalse;
		}
	//Value read OK and ALS is supported
	else if( status == KErrNone && 
			( alsValue == ESSSettingsAlsPrimary || alsValue == ESSSettingsAlsAlternate ) )
		{
		NCN_RDEBUG( _L("ALS value read properly! ALS is supported!") );	
		return ETrue;
		}
	//Default value. ALS not supported. Returned if ALS status can't be read properly!
	else
		{
		NCN_RDEBUG_INT2( _L("ALS value NOT read properly! Status: %d Value: %d"), status, alsValue );		
		return EFalse;		
		}
	}

// -------------------------------------------------------------------
// Check the KMuiuSupressAllNotificationConfiguration value
// -------------------------------------------------------------------
//
TBool CVoiceMailManager::CheckSupressNotificationSettingL()
{
    TBool result = EFalse; 
    TInt value = 0;
    CRepository* repository = NULL;
   
   TRAPD( err, repository = CRepository::NewL( KCRUidMuiuMessagingConfiguration ) );
   if( err == KErrNone && repository != NULL )
       {
       CleanupStack::PushL( repository ); 
       err = repository->Get( KMuiuSupressAllNotificationConfiguration, value );
       
       if(err == KErrNone && (value & KMuiuNotificationSupressedForVoiceMail ))
           {
           result = ETrue;
           }
        }  
       NCN_RDEBUG_INT( _L("CNcnNotifier::CheckSupressNotificationSetting() -  result: %d"), result );
       CleanupStack::PopAndDestroy( repository );
       return result;
}
//  End of File
