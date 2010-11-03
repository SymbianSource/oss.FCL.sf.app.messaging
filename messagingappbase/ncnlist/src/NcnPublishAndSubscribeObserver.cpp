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
* Description:   Methods for CNcnPublishAndSubscribeObserver class.
*
*/



// INCLUDE FILES
#include    "NcnDebug.h"
#include    "NcnPublishAndSubscribeObserver.h"
#include    "NcnModelBase.h"
#include    "NcnSubscriber.h"

#include    <featmgr.h>
#include    <bldvariant.hrh>
#include    <startupdomainpskeys.h>
#include    <avkondomainpskeys.h>
#include    <coreapplicationuisdomainpskeys.h>
#include    <ctsydomainpskeys.h>
#include    "CNcnMsgWaitingManager.h"

// ================= LOCAL CONSTANTS =======================
namespace
    {
    }

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
CNcnPublishAndSubscribeObserver::CNcnPublishAndSubscribeObserver( CNcnModelBase* aModel )
    : iModel( aModel )
    {
    }

// Symbian OS default constructor can leave.
void CNcnPublishAndSubscribeObserver::ConstructL()
    {
    TInt value = 0;
    TInt status = KErrNone;
    
	//Subscribe the SIM change status   
	iSimChangedSubscriber = CNcnSubscriber::NewL( *this,
	    KPSUidStartup,
	    KPSSimChanged );
	iSimChangedSubscriber->Subscribe();

	iSimUsableSubscriber = CNcnSubscriber::NewL( *this,
	    KPSUidStartup,
	    KPSSimStatus );
	iSimUsableSubscriber->Subscribe();
	
    status = RProperty::Get ( KPSUidStartup, KPSSimStatus, value );
    if ( status ==KErrNone && value == ESimUsable )
        {
        iModel->MsgWaitingManager().GetFirstIndicatorStatus();
        }

	//And get the initial value
	status = RProperty::Get ( KPSUidStartup, KPSSimChanged, value );
	NCN_RDEBUG_INT2(_L("CNcnPublishAndSubscribeObserver : Requested SIM change! status: %d value: %d"), status, value );
	    	    
    if( status == KErrNone && value != KErrUnknown && iModel->IsSupported( KNcnIdSimCard ) )
        {
        iModel->SetSimChanged(
            ( value == ESimChanged ) ? ETrue : EFalse );
        }

	//Subscribe Idle status. Avkon P&S key is used instead of Active Idle's P&S key.
	//This prevents soft notifications to be deleted when screensaver activates.
	iPhoneIdleSubscriber = CNcnSubscriber::NewL( *this, 
		KPSUidAvkonDomain, // ActiveIdle state
		KAknNotificationsInIdleAllowed );
	iPhoneIdleSubscriber->Subscribe();
	
	//And get the initial value	
	RProperty::Get ( KPSUidAvkonDomain, KAknNotificationsInIdleAllowed, iPhoneIdleState );
	NCN_RDEBUG_INT2(_L("CNcnPublishAndSubscribeObserver : Requested idle! status: %d value: %d"), status, iPhoneIdleState );

	    
	//Subscribe Autolock status 	
	iAutolockSubscriber = CNcnSubscriber::NewL( *this,
	    KPSUidCoreApplicationUIs,
	    KCoreAppUIsAutolockStatus );
	iAutolockSubscriber->Subscribe();
	
	//And get the initial value		
	RProperty::Get ( KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus, iAutolockStatus );
	NCN_RDEBUG_INT2(_L("CNcnPublishAndSubscribeObserver : Requested auto lock! status: %d value: %d"), status, iAutolockStatus );

	
	//Subscribe Calls state	
	iCurrentCallSubscriber = CNcnSubscriber::NewL( *this,
	    KPSUidCtsyCallInformation,
	    KCTsyCallState );
	iCurrentCallSubscriber->Subscribe();		
	
	//And get the initial value	
	RProperty::Get ( KPSUidCtsyCallInformation, KCTsyCallState, iCurrentCallStatus );
	NCN_RDEBUG_INT2(_L("CNcnPublishAndSubscribeObserver : Requested call status! status: %d value: %d"), status, iCurrentCallStatus );

	//Determine idle state
    NCN_RDEBUG(_L("CNcnPublishAndSubscribeObserver Determine idle state in start-up"));
	DetermineIdleState();
	}

// Two-phased constructor.
CNcnPublishAndSubscribeObserver* CNcnPublishAndSubscribeObserver::NewL( CNcnModelBase* aModel )
    {
    CNcnPublishAndSubscribeObserver* self =
        new (ELeave) CNcnPublishAndSubscribeObserver( aModel );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CNcnPublishAndSubscribeObserver::~CNcnPublishAndSubscribeObserver()
    {
    delete iSimUsableSubscriber;
	delete iSimChangedSubscriber;
	delete iPhoneIdleSubscriber;
	delete iAutolockSubscriber;
	delete iCurrentCallSubscriber;
	
    iModel = NULL;
    }

// ---------------------------------------------------------
// CNcnPublishAndSubscribeObserver::HandlePropertyChangedL()
// Handles the subscribed properties changes.
// ---------------------------------------------------------
//
void CNcnPublishAndSubscribeObserver::HandlePropertyChangedL( const TUid& aCategory, TInt aKey )
    {
 	TInt state = 0;

    if( iModel->IsSupported( KNcnIdSimCard ) )
        {
        if ( aKey == KPSSimChanged )
            {
			RProperty::Get( KPSUidStartup, KPSSimChanged, state );
            iModel->SetSimChanged(
                ( state == ESimChanged ) ? ETrue : EFalse  );
            return;
            }
        }

    TBool ncnIdleRelatedEventReceived = EFalse;
        
    // idle status changed
    if ( aCategory == KPSUidAvkonDomain && aKey == KAknNotificationsInIdleAllowed )
        {
        RProperty::Get( KPSUidAvkonDomain, KAknNotificationsInIdleAllowed, iPhoneIdleState );
        ncnIdleRelatedEventReceived = ETrue;
        }
        
    // autolock status changed
	else if ( aCategory == KPSUidCoreApplicationUIs && aKey == KCoreAppUIsAutolockStatus )
        {
 		RProperty::Get( KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus, iAutolockStatus );
        ncnIdleRelatedEventReceived = ETrue;
        }
        
    // call state status changed
	else if ( aCategory == KPSUidCtsyCallInformation && aKey == KCTsyCallState )
        {
 		RProperty::Get( KPSUidCtsyCallInformation, KCTsyCallState, iCurrentCallStatus );
		ncnIdleRelatedEventReceived = ETrue;
        }
    else if ( aCategory == KPSUidStartup && aKey == KPSSimStatus )
        {
        RProperty::Get( KPSUidStartup, KPSSimStatus , state );
        NCN_RDEBUG_INT(_L("CNcnPublishAndSubscribeObserver::HandlePropertyChangedL RProperty::KPSSimStatus %d"), state );
        if ( state == ESimUsable )
            {
            iModel->MsgWaitingManager().GetFirstIndicatorStatus();
            }
        }
          
	//Determine idle state
    if(  ncnIdleRelatedEventReceived  )
        {
		DetermineIdleState();
        }
    }
    
// ---------------------------------------------------------
// CNcnPublishAndSubscribeObserver::NotifyPublishAndSubscribe
// ---------------------------------------------------------
//
void CNcnPublishAndSubscribeObserver::NotifyPublishAndSubscribe(
    const TUid&     aCategory,
    const TUint     aVariable,
    const TInt      aState )
    {
    NCN_RDEBUG_INT2(_L("CNcnPublishAndSubscribeObserver::NotifyPublishAndSubscribe : aCategory %d, aVariable: %d"), aCategory, aVariable );
    NCN_RDEBUG_INT(_L("CNcnPublishAndSubscribeObserver::NotifyPublishAndSubscribe : aState: %d"), aState );
    
	int err = RProperty::Set( aCategory, aVariable, aState );
	
	if( err != KErrNone )
	    {
	    NCN_RDEBUG_INT(_L("CNcnPublishAndSubscribeObserver::NotifyPublishAndSubscribe RProperty::Set failed with %d"), err );
	    }
    }
// ---------------------------------------------------------
// CNcnPublishAndSubscribeObserver::DetermineIdleState
// ---------------------------------------------------------
//
void CNcnPublishAndSubscribeObserver::DetermineIdleState()
	{
	//Determine idle state based on these three variables
	TBool idleState = EFalse; // default
    NCN_RDEBUG_INT(_L("CNcnPublishAndSubscribeObserver::DetermineIdleState AutoLockState::%d"), iAutolockStatus );
    if( iAutolockStatus > EAutolockOff  )
     {
     NCN_RDEBUG( _L("Autolock is ON and. Setting idle to false") );
     }
	else
	    {
	    if( iPhoneIdleState )
	        {
	        NCN_RDEBUG( _L("Autolock is OFF and phone is in IDLE. Setting idle to true") );
	        idleState = ETrue;
	        }
	    else
	        {
	        NCN_RDEBUG( _L("Autolock is OFF and phone is NOT in IDLE. Setting idle to false") );
	        }
	    }
	iModel->SetIdleState( idleState );
	}

// End of File
