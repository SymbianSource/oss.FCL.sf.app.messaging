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
* Description:   Implements the class
*
*/



// INCLUDE FILES
#include    <msgsimscnumberdetector.h>
#include    <featmgr.h>
#include    <bldvariant.hrh>
#include    <messagingvariant.hrh>
#include    <MNcnNotification.h>
#include	<messaginginternalcrkeys.h>
#include    <startupdomainpskeys.h>

#include    "NcnDebug.h"
#include    "NcnSNNotifier.h"
#include    "NcnMsvSessionObserverBase.h"
#include    "NcnCRHandler.h"

#include    "CVoiceMailManager.h"
#include    "NcnOutboxObserver.h"
#include	"MNcnNotificationObserver.h"
#include    "CNcnNotifApiObserver.h"
#include    "NcnHandlerAudio.h"
#include    "NcnModelBase.h"
#include    "CNcnMsvSessionHandler.h"
#include    "CNcnUI.h"
#include    "CNcnNotifier.h"

#include    "NcnPublishAndSubscribeObserver.h"
#include	"CNcnMsgWaitingManager.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
CNcnModelBase::CNcnModelBase()  :
    CActive( EPriorityStandard ), iVoiceMailManager(NULL),
    iMsgSimOperation( NULL ), iSmsInitialisationPhase( 0 ), 
    iNcnStatusBits( 0 ), iNcnFeatureFlags( 0 )
    {
	CActiveScheduler::Add( this );
    }

// Symbian OS default constructor can leave.

void CNcnModelBase::ConstructL()
    {
    // instantiate Msv session handler
    iMsvSessionHandler = CNcnMsvSessionHandler::NewL();
    
    // Set bootstate active
    iNcnStatusBits |= KNcnBootPhase;

    // Get available features from feature manager
    FeatureManager::InitializeLibL();
    
    //Is offline mode supported?
    iNcnFeatureFlags = FeatureManager::FeatureSupported( KFeatureIdOfflineMode );

    //Is automatic opening of the received sms/mms enabled?
    //Received SMS/MMS is opened automatically during active call if the sender
    //is the same as the caller
    FeatureManager::FeatureSupported( KFeatureIdSendFileInCall ) ? iNcnFeatureFlags |= KNcnSendFileInCall : 0;
    
    //Is Audio messaging feature enabled?
    FeatureManager::FeatureSupported( KFeatureIdAudioMessaging ) ? iNcnFeatureFlags |= KNcnAudioMessaging : 0;

	FetchInitialSimSupportStateL();
    
    // instantiate ncn ui    
    iNcnUI = CNcnUI::NewL( *this );
    
    // we're in boot phase, inform UI
    iNcnUI->SetBootPhase( ETrue );
    
    // insantiate ncn notifier with UI
    iNcnNotifier = CNcnNotifier::NewL( *iNcnUI, *this );

	// Create MsgWaitingManager which manages indicators
	// and message counts
	iMsgWaitingManager = CNcnMsgWaitingManager::NewL( *this );

	//Connect to CR keys
	CreateCRConnectionL();

	// Check if SIM service center number should be read 
	// in boot
	CheckIfSimSCShouldBeRead();			

	// Create publish and subscribe observer to receive events
    NCN_RDEBUG( _L("CNcnModelBase::ConstructL - Constructing CNcnPublishAndSubscribeObserver") );
	iNcnPublishAndSubscribeObserver = CNcnPublishAndSubscribeObserver::NewL( this );

	//Create a manager for voice mail messages
	iVoiceMailManager = CVoiceMailManager::NewL( *this );
	
	// Create MsvSession observer to receive message and email events
	// It takes the voice mail manager as a reference so it can
	// notify it directly when new voice mail messages are received
    NCN_RDEBUG( _L("CNcnModelBase::ConstructL - Constructing CNcnMsvSessionObserverBase") ); 
    iNcnMsvSessionObserver = CNcnMsvSessionObserverBase::NewL( this, *iVoiceMailManager );         

    // Check if Service Centres should be fetched
    CheckIfSimSCOperationRequired();
    
    // Create outbox observer
    NCN_RDEBUG( _L("CNcnModelBase::ConstructL - Constructing CNcnOutboxObserver") );
    iNcnOutboxObserver = CNcnOutboxObserver::NewL( this );

    // Create NCN Notification API observer(ECOM API)
    RImplInfoPtrArray implInfoArray;
    CleanupClosePushL( implInfoArray );
    REComSession::ListImplementationsL( KNcnNotificationInterfaceUid,
                                        implInfoArray );
    if ( implInfoArray.Count() > 0 )
        {
        NCN_RDEBUG( _L("CNcnModelBase::ConstructL - Creating CNcnNotifApiObserver") );
        iNcnNotificationObserver = CNcnNotifApiObserver::NewL( *this );
        }
    else
        {
        NCN_RDEBUG( _L("CNcnModelBase::ConstructL - ERROR! CNcnNotifApiObserver ECOM Plug-in not found!") );      
        }
    CleanupStack::PopAndDestroy(); // implInfoArray                                                                           
    
    // start the session handler
    iMsvSessionHandler->StartSessionHandlerL();
    
    // Reset the boot phase bit.
    iNcnStatusBits &= ~KNcnBootPhase;
    
    // we're finished with boot phase, inform UI
    iNcnUI->SetBootPhase( EFalse );
    }

// Destructor
CNcnModelBase::~CNcnModelBase()
    {
    FeatureManager::UnInitializeLib();

    Cancel();
    // Free allocated resources.
    
    delete iNcnOutboxObserver;    
	delete iNcnCRHandler;
	delete iNcnPublishAndSubscribeObserver;
    delete iNcnMsvSessionObserver;
    delete iMsgSimOperation;
    delete iNcnNotificationObserver;          
    delete iVoiceMailManager;
        
    delete iNcnNotifier;
    delete iNcnUI;
    
    delete iMsvSessionHandler;
    
    delete iMsgWaitingManager;
    }

// ---------------------------------------------------------
// CNcnModel::SetIdleStateL
// Informs the soft notification about the current phone idle state status.
// ---------------------------------------------------------
//
void CNcnModelBase::SetIdleState( const TBool aCurrentState )
    {
    // delegate to UI
    iNcnUI->SetIdleState( aCurrentState );
    }
// ---------------------------------------------------------
// CNcnModel::NotifyPublishAndSubscribe
// ---------------------------------------------------------
//
void CNcnModelBase::NotifyPublishAndSubscribe(
    const TUid&     aVariable,
    const TInt      aState )
    {
    iNcnPublishAndSubscribeObserver->NotifyPublishAndSubscribe( KUidSystemCategory, aVariable.iUid, aState );
    }
    
// ---------------------------------------------------------
// CNcnModel::NotifyPublishAndSubscribe
// ---------------------------------------------------------
//
void CNcnModelBase::NotifyPublishAndSubscribe(
    const TUid&     aCategory,
    const TUid&     aVariable,
    const TInt      aState )
    {
    iNcnPublishAndSubscribeObserver->NotifyPublishAndSubscribe( aCategory, aVariable.iUid, aState );
    }
    
// ---------------------------------------------------------
// CNcnModel::NotifyPublishAndSubscribe
// ---------------------------------------------------------
//
void CNcnModelBase::NotifyPublishAndSubscribe(
    const TUid&     aCategory,
    const TUint     aVariable,
    const TInt      aState )
    {
    iNcnPublishAndSubscribeObserver->NotifyPublishAndSubscribe( aCategory, aVariable, aState );
    }

// ---------------------------------------------------------
// CNcnModel::SmsServicePresent
// ---------------------------------------------------------
//
void CNcnModelBase::SmsServicePresent( const TBool aSmsServicePresent )
    {
    aSmsServicePresent ?
        ( iNcnStatusBits |= KNcnSmsServicePresent ) :
        ( iNcnStatusBits &= ~KNcnSmsServicePresent );

    // Check here if we should fetch the sms service center number
    // from the sim card.
    CheckIfSimSCOperationRequired();
    }

// ---------------------------------------------------------
// CNcnModelBase::DoCancel
// ---------------------------------------------------------
//
void CNcnModelBase::DoCancel()
    {
    // Destroy the outstanding request (delete the active object).
    delete iMsgSimOperation;
    iMsgSimOperation = NULL;
    }

// ---------------------------------------------------------
// CNcnModelBase::RunL
// ---------------------------------------------------------
//
void CNcnModelBase::RunL()
    {
    NCN_RDEBUG_INT(_L("CNcnModelBase::RunL(): SC operation completed with status:%d" ), iStatus.Int() );

    if ( IsSupported( KNcnIdSimCard ))
        {
        // Now we have performed the SIM sc fetching so we can close the shared data session...
	    iNcnCRHandler->SetSmumFlag( 2 ); // Initialisation complete
		delete iMsgSimOperation;
        iMsgSimOperation = NULL;
        }
    }

// ---------------------------------------------------------
// CNcnModelBase::StopMsgReceivedTonePlaying
// ---------------------------------------------------------
//
void CNcnModelBase::StopMsgReceivedTonePlaying()
    {
    iNcnUI->AbortAlertTonePlayback();
    }

// ---------------------------------------------------------
// CNcnModelBase::MsgReceivedTonePlaying
// ---------------------------------------------------------
//
void CNcnModelBase::MsgReceivedTonePlaying( TUint aAlertTonePlaying )
    {
    if( iNcnCRHandler )
        {
        iNcnCRHandler->MsgReceivedTonePlaying( aAlertTonePlaying );
        }    
    }

// ---------------------------------------------------------
// CNcnModelBase::IsSupported
// ---------------------------------------------------------
//
TBool CNcnModelBase::IsSupported( TUint aFeature ) const
    {
    return iNcnFeatureFlags & aFeature;
    }

// ---------------------------------------------------------
// CNcnModelBase::AddLocalVariationSupport
// ---------------------------------------------------------
//
void CNcnModelBase::AddLocalVariationSupport( TUint aFeature )
    {
    iNcnFeatureFlags |= aFeature;
    }

// ---------------------------------------------------------
// CNcnModelBase::NotifyOfflineSenderL
// ---------------------------------------------------------
//
void CNcnModelBase::NotifyOfflineSenderL( const TInt& aNetworkBars )
    {
    NCN_RDEBUG_INT(_L("CNcnModelBase::NotifyOfflineSenderL(): aNetworkBars:%d" ), aNetworkBars );
    iNcnOutboxObserver->InformOutboxSenderL( aNetworkBars );
    }

// ---------------------------------------------------------
// CNcnModelBase::GetCRString
// ---------------------------------------------------------
//
TInt CNcnModelBase::GetCRString(
    const TUid&     aUid,
    const TUint32   aKey,
    TDes&           aString ) const
    {
    // Fetch value from shared data
    return iNcnCRHandler->GetString( aUid, aKey, aString );
    }

// ---------------------------------------------------------
// CNcnModelBase::GetPSString
// ---------------------------------------------------------
//
TInt CNcnModelBase::GetPSString(
    const TUid&     aUid,
    const TUint32   aKey,
    TDes&           aString ) const
    {
    // Fetch value from shared data
    return iNcnCRHandler->GetPSString( aUid, aKey, aString );
    }

// ---------------------------------------------------------
// CNcnModelBase::GetCRInteger
// ---------------------------------------------------------
//
TInt CNcnModelBase::GetCRInteger(
    const TUid&     aUid,
    const TUint32   aKey,
    TInt&           aValue) const
    {
    // Fetch value from shared data
    return iNcnCRHandler->GetCRInt( aUid, aKey, aValue );
    }

// ---------------------------------------------------------
// CNcnModelBase::SetCRInt
// ---------------------------------------------------------
//
TInt CNcnModelBase::SetCRInt( const TUid& aUid,
                        const TUint32& aKey, 
                        TInt& aValue ) const
    {
    return iNcnCRHandler->SetCRInt( aUid, aKey, aValue );
    }
                        
// ---------------------------------------------------------
// CNcnModel::PlayAlertTone
// ---------------------------------------------------------
//
TInt CNcnModelBase::PlayAlertToneL( const TUint aAlertTone ) const
    {
    if( aAlertTone == CNcnModelBase::EIndexNewEmails )
        {
        iNcnUI->PlayEMailAlertTone();
        }
    else
        {
        iNcnUI->PlayMessageAlertTone();        
        }
        
    return KErrNone;
    }

// ---------------------------------------------------------
// CNcnModelBase::SetSimSupported
// ---------------------------------------------------------
//
void CNcnModelBase::SetSimSupported( const TBool aSimSupported )
    {
    NCN_RDEBUG_INT(_L("CNcnModelBase::SetSimSupported:%d" ), aSimSupported );

    if ( aSimSupported )
        {
        iNcnFeatureFlags |= KNcnIdSimCard;
        }
    else
        {
        iNcnFeatureFlags &= ~KNcnIdSimCard;
        }
    }

// ---------------------------------------------------------
// CNcnModelBase::IsSIMPresent
// ---------------------------------------------------------
//
TBool CNcnModelBase::IsSIMPresent()
	{
	TInt status = KErrNone;
	TInt value = 0;
	
	status = RProperty::Get ( KPSUidStartup, KPSSimStatus, value );
    NCN_RDEBUG_INT2(_L("CNcnModelBase::IsSIMPresent! status %d, value %d" ), status, value );
    
	if ( status == KErrNone && value != ESimNotPresent )
        {
        NCN_RDEBUG(_L("SIM present") );
		return TRUE;
        }
    else
    	{
    	NCN_RDEBUG(_L("SIM not present") );
    	return FALSE;
    	}
	}
		
// ---------------------------------------------------------
// CNcnModelBase::NotificationObserver
// ---------------------------------------------------------
//
MNcnNotificationObserver& CNcnModelBase::NotificationObserver()
    {
    __ASSERT_DEBUG( iNcnNotificationObserver, User::Invariant() );
    return *iNcnNotificationObserver;
    }
    
// ---------------------------------------------------------
// CNcnModelBase::FetchInitialSimSupportStateL
// ---------------------------------------------------------
//
void CNcnModelBase::FetchInitialSimSupportStateL()
	{
	TInt state = 0;

    RProperty::Get ( KPSUidStartup, KPSSimStatus, state );
    
	if ( state != KErrUnknown )
        {
		SetSimSupported( ETrue );
        SetSimSupported(
            ( state == ESimNotSupported ) ? EFalse : ETrue );
        }
	}

// ---------------------------------------------------------
// CNcnModelBase::MsvSessionHandler
// ---------------------------------------------------------
//
CNcnMsvSessionHandler& CNcnModelBase::MsvSessionHandler()
    {
    __ASSERT_DEBUG( iMsvSessionHandler, User::Invariant() );
    return *iMsvSessionHandler;
    }
    
// ---------------------------------------------------------
// CNcnModelBase::NcnUI
// ---------------------------------------------------------
//
MNcnUI& CNcnModelBase::NcnUI()
    {
    __ASSERT_DEBUG( iNcnUI, User::Invariant() );
    return *iNcnUI;
    }

// ---------------------------------------------------------
// CNcnModelBase::NcnNotifier
// ---------------------------------------------------------
//    
MNcnNotifier& CNcnModelBase::NcnNotifier()
    {
    __ASSERT_DEBUG( iNcnNotifier, User::Invariant() );
    return *iNcnNotifier;
    }

// ---------------------------------------------------------
// CNcnModelBase::NcnNotifier
// ---------------------------------------------------------
//    
MNcnMsgWaitingManager& CNcnModelBase::MsgWaitingManager()
	{
    __ASSERT_DEBUG( iMsgWaitingManager, User::Invariant() );
	return *iMsgWaitingManager;		
	}

// ---------------------------------------------------------
// CNcnModelBase::VoiceMailManager
// ---------------------------------------------------------
//    
CVoiceMailManager& CNcnModelBase::VoiceMailManager()
	{
    __ASSERT_DEBUG( iVoiceMailManager, User::Invariant() );
	return *iVoiceMailManager;		
	}

// ---------------------------------------------------------
// CNcnModelBase::CheckIfSimSCShouldBeRead
// ---------------------------------------------------------
//    
void CNcnModelBase::CheckIfSimSCShouldBeRead()
	{
	TInt value = 0;
	TInt ret = GetCRInteger( KCRUidMuiuVariation, KMuiuSmsFeatures, value );
	NCN_RDEBUG_INT(_L("CNcnModelBase::CheckIfSimSCShouldBeRead: GetCRInteger returned %d" ), ret );			
	if( ret == KErrNone && ( value & KSmsFeatureIdCheckSimAlways ) )
		{
		iNcnStatusBits |= KNcnReadSimSc;
		}
		
	NCN_RDEBUG_INT(_L("CNcnModelBase::CheckIfSimSCShouldBeRead: reading SMS SC from SIM %d" ), ( iNcnStatusBits & KNcnReadSimSc ) ? ETrue : EFalse );		
	}
    
//  End of File
