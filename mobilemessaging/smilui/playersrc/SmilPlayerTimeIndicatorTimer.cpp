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
* Description: SmilPlayerTimeIndicatorTimer implementation
*
*/



// INCLUDE FILES
#include <AknUtils.h>       // AknTextUtils
#include <StringLoader.h>   // StringLoader
#include <avkon.rsg>        // R_QTN_TIME_DURAT_MIN_SEC
#include <SmilPlayer.rsg>   // R_SMILPLAYER_PRESENTATION_DURATION

#include "SmilPlayerTimeIndicatorTimer.h"
#include "SmilPlayerTimeObserver.h"

// CONSTANTS
const TInt KTimeControlIntToMicroSec            = 1000000;
const TInt KMaxTimeInSeconds                    = 3599;
const TInt KTimeControlStartDelay               = 500000; //0.5 sec
const TInt KTimeControlProceedingTime           = 200000; //0.2 sec


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSmilPlayerTimeIndicatorTimer::CSmilPlayerTimeIndicatorTimer
// C++ constructor.
// ----------------------------------------------------------------------------
// 
CSmilPlayerTimeIndicatorTimer::CSmilPlayerTimeIndicatorTimer( MSmilPlayerTimeObserver* aObserver, 
                                                              CCoeEnv* aCoeEnv ) :
    CTimer( EPriorityStandard ),
    iTimeObserver( aObserver ),
    iCurrentValue( 0 ),
    iState( EReadyForAction ),
    iCoeEnv( aCoeEnv )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerTimeIndicatorTimer::ConstructL
// Symbian 2nd phase constructor. Initializes timer and loads corret resource string
// for localizable time string.
// ----------------------------------------------------------------------------
// 
void CSmilPlayerTimeIndicatorTimer::ConstructL()
    {
    CTimer::ConstructL();

    StringLoader::Load( iTimeDuratMinSecWithZero, R_QTN_TIME_DURAT_MIN_SEC, iCoeEnv );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerTimeIndicatorTimer::NewL
// Symbian two phased constructor
// ----------------------------------------------------------------------------
// 
CSmilPlayerTimeIndicatorTimer* CSmilPlayerTimeIndicatorTimer::NewL( MSmilPlayerTimeObserver* aObserver,
                                                                    CCoeEnv* aCoeEnv )
    {
    CSmilPlayerTimeIndicatorTimer* self = new(ELeave) CSmilPlayerTimeIndicatorTimer( aObserver, 
                                                                                     aCoeEnv );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerTimeIndicatorTimer::~CSmilPlayerTimeIndicatorTimer
// Destructor
// ----------------------------------------------------------------------------
// 
CSmilPlayerTimeIndicatorTimer::~CSmilPlayerTimeIndicatorTimer()
    {    
    iTimeObserver = NULL; //For LINT
    iCoeEnv = NULL; // For LINT
    }

// ----------------------------------------------------------------------------
// CSmilPlayerTimeIndicatorTimer::RunL
// Called when timer triggers to update time indicator string if approriate.
// Also resets inactivity timer so that sceensaver is not triggered during
// playback.
// ----------------------------------------------------------------------------
// 
void CSmilPlayerTimeIndicatorTimer::RunL()
    {
    TInt previousTotalDuration( iDurationValue );
    TInt previousTime( iCurrentValue );

    // Updating current time
    TInt newTime( iTimeObserver->CurrentTime() );

    // Check to see if we are preparing to end even
    // if state is not updated correctly.
    if ( iState == ERunning && 
         iCurrentValue == (iDurationValue - 1) &&
         iCurrentValue != newTime )
        {
        iState = EPreparingForEnd;
        }

    if ( iState == EPreparingForEnd )
        {
        newTime = iDurationValue;
        previousTime = 0; // Force update
        }
    
    // Updating duration
    if ( !iTimeObserver->IsDurationFinite() )
        {
        iCurrentValue = newTime;
        iDurationValue = 0;
        }
    else
        {
        //store current time to the member
        iCurrentValue = newTime;
    
        //store current duration to the member 
        iDurationValue = iTimeObserver->PresentationDuration();
        }
        
    //check here is the current time or duration has changed
    if ( previousTime != iCurrentValue ||
         previousTotalDuration != iDurationValue )
        {
        // Reset inactivity timer.
        User::ResetInactivityTime();
        
        CreateNewTimeStringL();
        }
    
    // start the timer again
    After( KTimeControlProceedingTime );//0.2sec
    }

// ----------------------------------------------------------------------------
// CSmilPlayerTimeIndicatorTimer::Start
// Cancels the old time indicator refreshing is ongoing.
// Sets current time to zero and tries to create new time string so that new time value is 
// refreshed immediatelly to indicator. Error on creating time string are ignored
// since time string update is not critical task and allowing leaves from function
// would create multiple TRAPs to other places.
// After this timer is started to update the time indicator and state is set to running. 
// ----------------------------------------------------------------------------
// 
void CSmilPlayerTimeIndicatorTimer::Start()
    {
    Cancel();
    
    iCurrentValue = 0;
    TRAP_IGNORE( CreateNewTimeStringL() );
    
    After( KTimeControlStartDelay );
    iState = ERunning;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerTimeIndicatorTimer::Pause
// Pauses time string update.
// ----------------------------------------------------------------------------
// 
void CSmilPlayerTimeIndicatorTimer::Pause()
    {
    iState = EPaused;
    Cancel();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerTimeIndicatorTimer::Resume
// Continues time string update.
// ----------------------------------------------------------------------------
// 
void CSmilPlayerTimeIndicatorTimer::Resume()
    {
    iState = ERunning;
    After( KTimeControlProceedingTime );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerTimeIndicatorTimer::Stop
// Stops time string update. Tries to update current value if possible.
// ----------------------------------------------------------------------------
// 
void CSmilPlayerTimeIndicatorTimer::Stop()
    {
    // Updating current time
    iCurrentValue = iTimeObserver->CurrentTime();
    
    TRAP_IGNORE( CreateNewTimeStringL() );
    iState = EReadyForAction;
    Cancel();
    }

// ----------------------------------------------------------------------------
// CSmilPlayerTimeIndicatorTimer::EndReached
// Indicates that presentation end has been reached. Sets timer state to preparing
// for end. 
// ----------------------------------------------------------------------------
// 
void CSmilPlayerTimeIndicatorTimer::EndReached()
    {
    iState = EPreparingForEnd;
    iCurrentValue = iDurationValue;
    }

// ----------------------------------------------------------------------------
// CSmilPlayerTimeIndicatorTimer::CreateNewTimeStringL
// Constructs new time string and updates it to observer.
// ----------------------------------------------------------------------------
//
void CSmilPlayerTimeIndicatorTimer::CreateNewTimeStringL()
    {
    // do not draw any time ( for example interactive presentation )
    if ( iCurrentValue != 0 || iDurationValue != 0 )
        {
        HBufC* timeBuffer = NULL;

        TBuf<KTimeControlMaxLength> timeString;
        
        TInt64 timeValue = MAKE_TINT64( 0, Min( KMaxTimeInSeconds, iCurrentValue ) );
        timeValue *= KTimeControlIntToMicroSec;
        
        TTime time( timeValue );
        time.FormatL( timeString, iTimeDuratMinSecWithZero );
        
        // checks if needed to draw total duration
        // and load localised string from resources
        if( iDurationValue != 0 )
            {
            TBuf<KTimeControlMaxLength> durationString;
            TInt64 durationValue = MAKE_TINT64( 0, Min( KMaxTimeInSeconds, iDurationValue ) );
            durationValue *= KTimeControlIntToMicroSec;
            TTime duration( durationValue );
            duration.FormatL( durationString,
                              iTimeDuratMinSecWithZero );
            
            //load localised string for presentation duration
            CDesCArrayFlat* array = new(ELeave) CDesCArrayFlat( 2 );
            CleanupStack::PushL( array );
            
            AknTextUtils::LanguageSpecificNumberConversion( timeString );
            array->AppendL( timeString );
            
            AknTextUtils::LanguageSpecificNumberConversion( durationString );
            array->AppendL( durationString );
            
            timeBuffer = StringLoader::LoadL( R_SMILPLAYER_PRESENTATION_DURATION,
                                              *array,
                                              iCoeEnv );
                                              
            CleanupStack::PopAndDestroy( array );
            }
        else
            {
            //only current time is drawn (no duration)
            //and localisation string is not needed to be used
            AknTextUtils::LanguageSpecificNumberConversion( timeString );
            timeBuffer = timeString.AllocL();
            }
        
        CleanupStack::PushL( timeBuffer );    
        iTimeObserver->TimeChangedL( *timeBuffer );
        CleanupStack::PopAndDestroy( timeBuffer );
        }
    }
        
// End of file
