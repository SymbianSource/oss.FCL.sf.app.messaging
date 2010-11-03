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
* Description:   Methods for CNcnCRHandler class.
*
*/



// INCLUDE FILES
#include    "NcnDebug.h"
#include    "NcnModelBase.h"
#include    "NcnModel.h"
#include    "NcnCRHandler.h"
#include    "CNcnCRRepositoryHandler.h"

#include    <messagingvariant.hrh>      // Local variation flags
#include    <centralrepository.h>
#include    <LogsDomainCRKeys.h>
#include    <NcnListInternalCRKeys.h>
#include    <NcnListInternalPSKeys.h>
#include    <messaginginternalcrkeys.h>
#include    <ProfileEngineSDKCRKeys.h>
#include    <telincallvolcntrlcrkeys.h>
#include    <coreapplicationuisdomainpskeys.h>

#include    <e32property.h>
#include    "NcnSubscriber.h"
#include 	"MNcnMsgWaitingManager.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code that
// might leave.
//
CNcnCRHandler::CNcnCRHandler( CNcnModelBase* aModel ) :
    iModel( aModel ),
    iLogsSDAssignedOk( EFalse ), iSysApSDAssignedOk( EFalse )
    {
    }

// Symbian OS default constructor can leave.
void CNcnCRHandler::ConstructL()
    {
    iLogsSession = CRepository::NewL( KCRUidLogs );
    iNcnSession = CRepository::NewL( KCRUidNcnList );
    iMuiuSession = CRepository::NewL( KCRUidMuiuVariation );
    iSmumSession = CRepository::NewL( KCRUidSmum );
    iProfileSession = CRepository::NewL( KCRUidProfileEngine );
    iTelephonySession = CRepository::NewL( KCRUidInCallVolume );

    iLogsSDAssignedOk = ETrue;
    
    TInt missedCalls = 0;
    
    // Get the amount of missed calls. If it is not initialised then initialise it to zero.
    TInt err = iLogsSession->Get( KLogsNewMissedCalls, missedCalls );
    if( err == KErrNotFound || err == KErrUnknown || missedCalls < 0 )
        {
        iLogsSession->Set( KLogsNewMissedCalls, 0 );
        }

    NCN_RDEBUG_INT(_L("CNcnCRHandler::ConstructL : CR Missed Calls:%d" ), missedCalls );

    // Request shared data server to notify on missed call variable changes.
    iLogsNotifyHandler =
        CNcnCRRepositoryHandler::NewL( *this, *iLogsSession, CCenRepNotifyHandler::EIntKey, KLogsNewMissedCalls );
    iLogsNotifyHandler->StartListeningL();        

    // update missed calls notification
    iModel->NcnNotifier().SetNotification(
            MNcnNotifier::ENcnMissedCallsNotification,
            0 );
            
    // For observing outside command to play sound    
    err =  RProperty::Define( KPSUidNcnList,
        KNcnPlayAlertTone,
        RProperty::EInt,
        ECapability_None,
        ECapabilityWriteUserData );
            
    if ( err == KErrNone )
        {
        RProperty::Set( KPSUidNcnList, KNcnPlayAlertTone, 0 );
        }
    iAlertToneSubscriber  = CNcnSubscriber::NewL( *this, KPSUidNcnList, KNcnPlayAlertTone );
    iAlertToneSubscriber->Subscribe();


    // Add listener for smum events
    if( iModel->IsSupported( KNcnIdSimCard ) )
        {
        TInt smsInitialisationPhase = 0; //default value, not informative state
        err = iSmumSession->Get( KSmumSmsStatus, smsInitialisationPhase );

        if( err == KErrNone )
            {
            iModel->SetSmsInitialisationState( smsInitialisationPhase );
            }

        if ( !iSmumNotifyHandler )
            {
            //iSmumNotifyHandler =
            //    CCenRepNotifyHandler::NewL( *this, *iSmumSession, CCenRepNotifyHandler::EIntKey, KSmumSmsStatus );
            //iSmumNotifyHandler->StartListeningL();
            iSmumNotifyHandler =
                CNcnCRRepositoryHandler::NewL( *this, *iSmumSession, CCenRepNotifyHandler::EIntKey, KSmumSmsStatus );
            iSmumNotifyHandler->StartListeningL();
            }
        }

    // Assign tone listener
    iMsgToneSubscriber  = CNcnSubscriber::NewL(
        *this, KPSUidCoreApplicationUIs, KCoreAppUIsMessageToneQuit );
    iMsgToneSubscriber->Subscribe();
    iSysApSDAssignedOk = ETrue;
    }

// Two-phased constructor.
CNcnCRHandler* CNcnCRHandler::NewL( CNcnModelBase* aModel )
    {
    CNcnCRHandler* self = new (ELeave) CNcnCRHandler( aModel );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CNcnCRHandler::~CNcnCRHandler()
    {
    if( iLogsNotifyHandler )
        {
        iLogsNotifyHandler->StopListening();
        delete iLogsNotifyHandler;
        }

    delete iLogsSession;
    delete iNcnSession;
    delete iMuiuSession;

    if ( iSmumNotifyHandler )
        {
        iSmumNotifyHandler->StopListening();
        delete iSmumNotifyHandler;
        }

    delete iSmumSession;

    if ( iMuiuSettingsNotifyHandler )
        {
        iMuiuSettingsNotifyHandler->StopListening();
        delete iMuiuSettingsNotifyHandler;
        }

    delete iProfileSession;
    delete iTelephonySession;

    delete iAlertToneSubscriber;
    delete iMsgToneSubscriber;
    }

// ----------------------------------------------------
//  CNcnCRHandler::HandleRepositoryNotifyInt
// ----------------------------------------------------
//
void CNcnCRHandler::HandleRepositoryNotifyInt(
    CRepository& aRepository,
    const TUint32 aID,
    const TInt    aNewValue )
    {        
    // logs repository key changes
    if( &aRepository == iLogsSession )
        {
        NCN_RDEBUG_INT2(_L("CNcnCRHandler::HandleRepositoryNotifyInt - LOGS repository key change (aID = %d, aNewValue = %d)" ), aID, aNewValue );
        
        // if key was missed calls
        if( aID == KLogsNewMissedCalls )
            {
            // update amount of missed calls            
            iModel->NcnNotifier().SetNotification(
                    MNcnNotifier::ENcnMissedCallsNotification,
                    aNewValue );
            }
        }
    // smum repository key changes
    else if( &aRepository == iSmumSession )
        {
        NCN_RDEBUG_INT2(_L("CNcnCRHandler::HandleRepositoryNotifyInt - SMUM repository key change (aID = %d, aNewValue = %d)" ), aID, aNewValue );
        
        // if key was sms status
        if( aID == KSmumSmsStatus && iModel->IsSupported( KNcnIdSimCard ) )
            {
            iModel->SetSmsInitialisationState( aNewValue );
            }    
        }

    else
        {
        NCN_RDEBUG(_L("CNcnCRHandler::HandleRepositoryNotifyInt - Unknown repository" ));
        }
    }

// ---------------------------------------------------------
// CNcnCRHandler::HandlePropertyChangedL()
// Handles the subscribed properties changes.
// ---------------------------------------------------------
//
void CNcnCRHandler::HandlePropertyChangedL( const TUid& aCategory, TInt aKey )
    {
    TInt state = 0;

    if ( aCategory == KPSUidCoreApplicationUIs && aKey == KCoreAppUIsMessageToneQuit )
        {
        RProperty::Get( KPSUidCoreApplicationUIs, KCoreAppUIsMessageToneQuit, state );
        
        NCN_RDEBUG_INT(_L("CNcnCRHandler::HandlePropertyChangedL - KCoreAppUIsMessageToneQuit changed to %d" ), state );
        
        if( state == ECoreAppUIsStopTonePlaying )
            {
            iModel->StopMsgReceivedTonePlaying();
            }
        }
    else if ( aCategory == KPSUidNcnList && aKey == KNcnPlayAlertTone )
        {
        RProperty::Get( KPSUidNcnList, KNcnPlayAlertTone, state );
        // Check if alert tone asked to be played
        if( state )
            {
            iModel->PlayAlertToneL( state );
            }
        }
    }


// ---------------------------------------------------------
// CNcnCRHandler::ResetMissedCalls
// ---------------------------------------------------------
//
void CNcnCRHandler::ResetMissedCalls()
    {
    // Clear all the bits.
    NCN_RDEBUG(_L("CNcnCRHandler::ResetMissedCalls()" ));
    if( iLogsSDAssignedOk )
        {
        iLogsSession->Set( KLogsNewMissedCalls, 0 );
        }
    }
// ---------------------------------------------------------
// CNcnCRHandler::SetSmumFlag( const TInt aNewFlagValue )
// ---------------------------------------------------------
//
void CNcnCRHandler::SetSmumFlag( const TInt aNewFlagValue )
    {
    iSmumSession->Set( KSmumSmsStatus, aNewFlagValue );
    }

// ---------------------------------------------------------
// CNcnCRHandler::MsgReceivedTonePlaying
// ---------------------------------------------------------
//
void CNcnCRHandler::MsgReceivedTonePlaying( const TUint aPlaying )
    {
    // Through this the System Application can detect whether we are playing
    // the message received ringing tone or not.
    NCN_RDEBUG(_L("CNcnCRHandler::MsgReceivedTonePlaying( const TBool aPlaying )"));
    if( iSysApSDAssignedOk )
        {        
        if( aPlaying )
            {
            RProperty::Set( KPSUidCoreApplicationUIs, KCoreAppUIsMessageToneQuit, ECoreAppUIsTonePlaying );
            }
        else
            {
            RProperty::Set( KPSUidCoreApplicationUIs, KCoreAppUIsMessageToneQuit, ECoreAppUIsStopTonePlaying );
            }
        }
    }

// ---------------------------------------------------------
// CNcnCRHandler::GetString
// ---------------------------------------------------------
//
TInt CNcnCRHandler::GetString(
    const TUid&     aUid,
    const TUint32   aKey,
    TDes&           aString ) const
    {
    // Prepare and get client
    CRepository* client = NULL;

    TInt err = GetClientForUid( aUid, client );

    // Check if problems with getting the client
    if( err == KErrNone )
        {
        err = client->Get( aKey, aString );
        }

    // Return result
    return err;
    }

// ---------------------------------------------------------
// CNcnCRHandler::GetPSString
// ---------------------------------------------------------
//
TInt CNcnCRHandler::GetPSString(
    const TUid&     aUid,
    const TUint32   aKey,
    TDes&           aString ) const
    {
    TInt err = RProperty::Get( aUid, aKey, aString );

    // Return result
    return err;
    }

// ---------------------------------------------------------
// CNcnCRHandler::GetCRInt
// ---------------------------------------------------------
//
TInt CNcnCRHandler::GetCRInt(
    const TUid&     aUid,
    const TUint32   aKey,
    TInt&           aValue ) const
    {
    // Prepare and get client
    CRepository*  client = NULL;

    TInt err = GetClientForUid( aUid, client );

    // Check if problems with getting the client
    if( err == KErrNone )
        {
        err = client->Get( aKey, aValue );
        }

    // Return result
    return err;
    }

// ---------------------------------------------------------
// CNcnCRHandler::SetCRInt
// ---------------------------------------------------------
//
TInt CNcnCRHandler::SetCRInt(
    const TUid&     aUid,
    const TUint32   aKey,
    const TInt      aValue,
    const TBool     /*aSave*/ ) const
    {
    // Prepare and get client
    CRepository*  client = NULL;

    TInt err = GetClientForUid( aUid, client );

    // Check if problems with getting the client
    if( err == KErrNone )
        {
        err = client->Set( aKey, aValue );
        }

    // Return result
    return err;
    }

// ---------------------------------------------------------
// CNcnCRHandler::UpdateMissedCallsNotification
// ---------------------------------------------------------
//
void CNcnCRHandler::UpdateMissedCallsNotification()
    {
    TInt missedCalls;
    TInt err = iLogsSession->Get( KLogsNewMissedCalls, missedCalls );
    if( ( err == KErrNone ) && ( missedCalls > 0 ) )
        {
        iModel->NcnNotifier().SetNotification( 
            MNcnNotifier::ENcnMissedCallsNotification, missedCalls );
        }
    }
    
// ---------------------------------------------------------
// CNcnCRHandler::GetClientForUid
// ---------------------------------------------------------
//
TInt CNcnCRHandler::GetClientForUid(
    const TUid&        aUid,
    CRepository*&      aClient ) const
    {
    TInt error = KErrNone;

    if( aUid == KCRUidNcnList )
        {
        // Get ncn session
        aClient = iNcnSession;
        }
    else if( aUid == KCRUidProfileEngine )
        {
        // Get default tone
        aClient = iProfileSession;
        }
    else if( aUid == KCRUidInCallVolume )
        {
        // Get default tone
        aClient = iTelephonySession;
        }
	else if ( aUid == KCRUidMuiuVariation )
		{
		aClient = iMuiuSession;			
		}                
    else
        {
        // Return error
        error = KErrNotFound;
        }

    return error;
    }


//  End of File
