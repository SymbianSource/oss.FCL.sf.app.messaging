/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: SmilPlayerVolumeIndicatorController implementation
*
*/



// INCLUDE FILES
#include "SmilPlayerVolumeIndicatorController.h"

#include <AknVolumePopup.h>
#include <avkon.rsg>

#include <centralrepository.h>		// Central Repository
#include <messaginginternalcrkeys.h>// for Central Repository keys
#include <ProfileEngineSDKCRKeys.h> 
#include <Profile.hrh>

// CONSTANTS

const TInt KVolumeControlMinValue = 1;
const TInt KVolumeControlMaxValue = 10;
const TInt KVolumeDefault = 4;

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSmilPlayerVolumeIndicatorController::CSmilPlayerVolumeIndicatorController
// C++ default constructor.
// ----------------------------------------------------------------------------
//
CSmilPlayerVolumeIndicatorController::CSmilPlayerVolumeIndicatorController()
    {
    }

// ----------------------------------------------------------------------------
// CSmilPlayerVolumeIndicatorController::ConstructL
// Symbian 2nd phase constructor. Initializes initial volume level. 
// After this arrow icons are created.
// Then AVKON provided volume control is created and volume timer that updates volume 
// control visibility when needed. 
// ----------------------------------------------------------------------------
//
void CSmilPlayerVolumeIndicatorController::ConstructL( const CCoeControl* aParent )
    {
    SetContainerWindowL( *aParent );
    
    iRepository = CRepository::NewL( KCRUidSmilui ); 
    iInitialVolumeLevel = InitialVolumeLevelL();
    
    iVolumePopup = CAknVolumePopup::NewL( NULL, ETrue );
    iVolumePopup->SetValue( iInitialVolumeLevel );
    iVolumePopup->SetObserver( this );     
    }

// ---------------------------------------------------------
// CSmilPlayerVolumeIndicatorController::NewL
// EPOC two phased constructor
// ---------------------------------------------------------
//
CSmilPlayerVolumeIndicatorController* CSmilPlayerVolumeIndicatorController::NewL( const CCoeControl* aParent )
    {
    CSmilPlayerVolumeIndicatorController* self = new(ELeave) CSmilPlayerVolumeIndicatorController();
    
    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerVolumeIndicatorController::~CSmilPlayerVolumeIndicatorController
// Destructor.
// ----------------------------------------------------------------------------
//
CSmilPlayerVolumeIndicatorController::~CSmilPlayerVolumeIndicatorController()
    {
    StoreAudioVolume();

    delete iRepository;
    
    delete iVolumePopup;
    }
 
// ----------------------------------------------------------------------------
// CSmilPlayerVolumeControl::OfferKeyEventL
// Handles the volume key event. Offers key to volume control which contains
// information about minimum, current and maximum volume levels. If volume control
// consumed volume key event sets volume control visible.
// ----------------------------------------------------------------------------
//
TKeyResponse CSmilPlayerVolumeIndicatorController::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                                   TEventCode aType )
    {
    TKeyResponse response = EKeyWasNotConsumed;
    if ( aKeyEvent.iCode == EKeyRightArrow || aKeyEvent.iCode == EKeyUpArrow )
        {
        iVolumePopup->ShowVolumePopupL();
        TInt maxValue;
        TInt minValue;
        iVolumePopup->GetRange( minValue, maxValue );
        TInt value = iVolumePopup->Value() + 1;
        value = value > maxValue ? maxValue : value;
        iVolumePopup->SetValue( value );
        response = EKeyWasConsumed;
        }
    else if ( aKeyEvent.iCode == EKeyLeftArrow || aKeyEvent.iCode == EKeyDownArrow )
        {
        iVolumePopup->ShowVolumePopupL();
        TInt maxValue;
        TInt minValue;
        iVolumePopup->GetRange( minValue, maxValue );
        TInt value = iVolumePopup->Value() - 1;
        value = value < minValue ? minValue : value;
        iVolumePopup->SetValue( value );
        response = EKeyWasConsumed;
        }
    
    return response;   
    }

// ----------------------------------------------------------------------------
// CSmilPlayerVolumeIndicatorController::HandleControlEventL
//
// Handles control events from volume control. Events are used to control visibility
// of left and right arrow. Volume level changes are also communicated to 
// observer with state change event.
// ----------------------------------------------------------------------------
//
void CSmilPlayerVolumeIndicatorController::HandleControlEventL( CCoeControl* aControl, 
                                                                TCoeEvent aEventType )
    {
    if ( aControl == iVolumePopup &&
         aEventType == MCoeControlObserver::EEventStateChanged )
        {
        ReportEventL( MCoeControlObserver::EEventStateChanged );        
        }
    }

// ----------------------------------------------------------------------------
// CSmilPlayerVolumeControl::VolumeValue
// Returns current volume level.
// ----------------------------------------------------------------------------
//
TInt CSmilPlayerVolumeIndicatorController::VolumeValue() const
    {
    return iVolumePopup->Value();
    }
    

// ----------------------------------------------------------------------------
// CSmilPlayerVolumeIndicatorController::InitialVolumeLevelL
// Returns correct initial volume level. Uses central repository to query used ringing tone 
// type and if it is set to silent then returns zero (=muted). 
// Otherwise queries last used volume level from central repository and returns this.
// ----------------------------------------------------------------------------
//
TInt CSmilPlayerVolumeIndicatorController::InitialVolumeLevelL() const
    {
    TInt result( 0 );
    CRepository* repository = CRepository::NewLC( KCRUidProfileEngine );
    
    TInt ringType( EProfileRingingTypeRinging );
    User::LeaveIfError( repository->Get( KProEngActiveRingingType, ringType ) );

    if ( ringType != EProfileRingingTypeSilent )
        {
        result = KVolumeDefault;
   	    iRepository->Get( KSmilPlayerVolume, result );
        }
        
    CleanupStack::PopAndDestroy( repository );    
    
    return result;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerVolumeIndicatorController::StoreAudioVolume
// Stores the changed volume level to central repository so that 
// volume is always between KVolumeControlMinValue and KVolumeControlMaxValue.
// Volume level is only stored if it has been changed.
// ----------------------------------------------------------------------------
//
void CSmilPlayerVolumeIndicatorController::StoreAudioVolume()
    {
    if ( iRepository )
        {
        TInt currVolume = VolumeValue();
        
        if ( currVolume != iInitialVolumeLevel )
            {
            //calculate the changed value (current + 1) or (currentvalue - 1)
            TInt storedVolume = Max( KVolumeControlMinValue, Min( KVolumeControlMaxValue, currVolume ) );

            iRepository->Set( KSmilPlayerVolume, storedVolume );
            }
        }
    }

//  End of File  
 
