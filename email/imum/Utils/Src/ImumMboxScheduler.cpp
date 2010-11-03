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
* Description: 
*		Alwaysonline email scheduler class.
*
*/


// INCLUDE FILES
#include <e32base.h>

#include "ImumMboxScheduler.h"
#include "ImumInSettingsData.h"
#include "ImumInSettingsKeys.h"
#include "ImumInternalApiImpl.h"
#include "ImumUtilsLogging.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
const TInt KImumSecondsIn24Hours = 86400;
const TInt KImumDaysInWeek = 7;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CImumMboxScheduler::CImumMboxScheduler()
// ----------------------------------------------------------------------------
//
CImumMboxScheduler::CImumMboxScheduler( 
    CImumInternalApiImpl& aMailboxApi,
    const TMsvId aMailboxId ) 
    :
    iMailboxApi( aMailboxApi ),
    iDays( 0 ),
    iStartTime( 0 ),
    iStopTime( 0 ),
    iMailboxId( aMailboxId )
    {
    IMUM_CONTEXT( CImumMboxScheduler::CImumMboxScheduler, 0, KLogInApi );
    
    }

// ----------------------------------------------------------------------------
// CImumMboxScheduler::~CImumMboxScheduler()
// ----------------------------------------------------------------------------
//
CImumMboxScheduler::~CImumMboxScheduler()
    {
    IMUM_CONTEXT( CImumMboxScheduler::~CImumMboxScheduler, 0, KLogInApi );
    
    }

// ----------------------------------------------------------------------------
// CImumMboxScheduler::ConstructL()
// ----------------------------------------------------------------------------
//
void CImumMboxScheduler::ConstructL()
    {
    IMUM_CONTEXT( CImumMboxScheduler::ConstructL, 0, KLogInApi );
    
    // Load the data from the settings for further use
    TInt64 days;
    TMsvEntry mbox = iMailboxApi.MailboxUtilitiesL().GetMailboxEntryL( 
        iMailboxId, MImumInMailboxUtilities::ERequestReceiving );
    iMailboxApi.SettingsStorerL().LoadAlwaysOnlineSettingsL(
        mbox.iMtmData2, mbox.iMtm, days, iStartTime, iStopTime, iActive );

    // First check if all of the days are unchecked, which should not happen
    // at all, but just in case
    iDays = !days ? TImumDaSettings::EFlagSetAllDays : days;
    iActive = ( iActive != EMailAoOff ) ? 
        MImumInMailboxUtilities::EFlagTurnedOn : 0;
    }

// ----------------------------------------------------------------------------
// CImumMboxScheduler::NewL()
// ----------------------------------------------------------------------------
//
CImumMboxScheduler* CImumMboxScheduler::NewL( 
    CImumInternalApiImpl& aMailboxApi,
    const TMsvId aMailboxId )
    {
    IMUM_STATIC_CONTEXT( CImumMboxScheduler::NewL, 0, utils, KLogInApi );
    
    CImumMboxScheduler* self = NewLC( aMailboxApi, aMailboxId );
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CImumMboxScheduler::NewLC()
// ----------------------------------------------------------------------------
//
CImumMboxScheduler* CImumMboxScheduler::NewLC( 
    CImumInternalApiImpl& aMailboxApi,
    const TMsvId aMailboxId )
    {
    IMUM_STATIC_CONTEXT( CImumMboxScheduler::NewLC, 0, utils, KLogInApi );
    
    CImumMboxScheduler* self =
        new ( ELeave ) CImumMboxScheduler( aMailboxApi, aMailboxId );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

/******************************************************************************

    Public scheduling functions

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxScheduler::AlwaysOnlineOn()
// ----------------------------------------------------------------------------
//
TInt64 CImumMboxScheduler::QueryAlwaysOnlineStateL()
    {
    IMUM_CONTEXT( CImumMboxScheduler::QueryAlwaysOnlineStateL, 0, KLogInApi );
    
    TBool ok = EFalse;
    TInt64 flags = 0;

    if ( iActive )
        {
        // Get hometime
        TTime clock;
        TTimeIntervalSeconds homeTime;
        TTimeIntervalSeconds startTime;
        TTimeIntervalSeconds stopTime;
        PrepareScheduling( clock, homeTime, startTime, stopTime );
        
        flags = MImumInMailboxUtilities::EFlagTurnedOn;

        ok = AlwaysOnlineOn( clock, homeTime, startTime, stopTime );
        
        if ( ok )
            {
            flags |= MImumInMailboxUtilities::EFlagWaitingToConnect;
            }                       
        }
    
    return flags;
    }


// ----------------------------------------------------------------------------
// CImumMboxScheduler::SecondsToNextMark()
// ----------------------------------------------------------------------------
//
TInt64 CImumMboxScheduler::SecondsToNextMark(
    TTimeIntervalSeconds& aSeconds )
    {
    IMUM_CONTEXT( CImumMboxScheduler::SecondsToNextMark, 0, KLogInApi );
    
    // Get hometime
    aSeconds = 0;   
    TTime clock;
    TTimeIntervalSeconds homeTime;
    TTimeIntervalSeconds startTime;
    TTimeIntervalSeconds stopTime;
    PrepareScheduling( clock, homeTime, startTime, stopTime );          
        
    // At first it needs to be checked, that either previous or current day
    // is selected.
    TValidConnectionDay validDay = GetValidConnectionDay( clock );
    TBool timeToConnect = ( validDay != ENoConnection );
    
    // Next step is to check, is it time for connection or disconnection
    if ( timeToConnect )
        {        
        timeToConnect = IsValidTimeToConnect( 
            validDay, homeTime, startTime, stopTime );
        }
        
    TInt connectStatus = !timeToConnect ? 
        MImumInMailboxUtilities::EFlagWaitingToConnect : 0;

    // Finally calculate the time to next mark       
    CalculateSecondsToNextMark(
        connectStatus, aSeconds, clock, homeTime, startTime, stopTime );      
            
    return connectStatus | iActive;
    }

/******************************************************************************

    Private scheduling tools

******************************************************************************/

// ----------------------------------------------------------------------------
// CImumMboxScheduler::PrepareScheduling()
// ----------------------------------------------------------------------------
//
void CImumMboxScheduler::PrepareScheduling(
    TTime& aClock,
    TTimeIntervalSeconds& aHome,
    TTimeIntervalSeconds& aStart,
    TTimeIntervalSeconds& aStop )
    {
    IMUM_CONTEXT( CImumMboxScheduler::PrepareScheduling, 0, KLogInApi );
    
    // Get the hometime and set the day flags    
    aClock.HomeTime();              
        
    // First, all the times has to be converted to seconds, to guarrantee
    // proper comparisions for the times
    Times2Seconds( aClock, aHome, aStart, aStop );
    
    // Start and stop times needs to be reordered so, that the
    // start time is before stop time in proportion to hometime
    OrganizeStartAndStopTimes( aHome, aStart, aStop );
    }
    
// ----------------------------------------------------------------------------
// CImumMboxScheduler::AlwaysOnlineOn()
// ----------------------------------------------------------------------------
//
TBool CImumMboxScheduler::AlwaysOnlineOn(
    const TTime& aClock,
    TTimeIntervalSeconds& aHome,
    TTimeIntervalSeconds& aStart,
    TTimeIntervalSeconds& aStop )
    {
    IMUM_CONTEXT( CImumMboxScheduler::AlwaysOnlineOn, 0, KLogInApi );
    
    TValidConnectionDay validDay = GetValidConnectionDay( aClock );
    TBool result = EFalse;
    
    // Connecting can be ok, if either previous or current day are checked
    if ( validDay != ENoConnection )
        {
        
        result = IsValidTimeToConnect(
            validDay, aHome, aStart, aStop );
        }

    
    return result;    
    }

// ----------------------------------------------------------------------------
// CImumMboxScheduler::GetValidConnectionDay()
//
// The connection is allowed in following cases:
// Day:     Mo Tu We Th Fr Sa Su
// Checked: 0  0  0  1  1  0  0
// Allowed: F  F  F  T  T  T  F
// ----------------------------------------------------------------------------
//
CImumMboxScheduler::TValidConnectionDay 
    CImumMboxScheduler::GetValidConnectionDay(
        const TTime& aClock )
    {
    IMUM_CONTEXT( CImumMboxScheduler::TValidConnectionDay, 0, KLogInApi );
    
    // Get current and previous day and make sure it is selected
    TDay currentDay = aClock.DayNoInWeek();
    TDay previousDay = GetPrevDay( currentDay );
    TValidConnectionDay result;
    
    
    // Connection can be made anyday
    if ( iDays.Flag( currentDay ) && iDays.Flag( previousDay ) )
        {
        
        result = EConnectionAnyDay;        
        }
    // Connection can be established when start time is at current day
    else if ( iDays.Flag( currentDay ) )
        {
        
        result = EConnectionCurrentDay;
        }
    // Connection can be established, if the connection begins from
    // checked day
    else if ( iDays.Flag( previousDay ) )
        {
        
        result = EConnectionPreviousDayOnly;
        }
    // Connection is not allowed
    else
        {
        
        result = ENoConnection;
        } 
        
    return result;           
    }

// ----------------------------------------------------------------------------
// CImumMboxScheduler::GetNextDay()
// ----------------------------------------------------------------------------
//
TDay CImumMboxScheduler::GetNextDay(
    const TDay aToday,
    const TInt aNth )
    {
    IMUM_CONTEXT( CImumMboxScheduler::GetNextDay, 0, KLogInApi );
    
    return static_cast<TDay>( ( aToday + aNth ) % KImumDaysInWeek );    
    }

// ----------------------------------------------------------------------------
// CImumMboxScheduler::GetPrevDay()
// ----------------------------------------------------------------------------
//
TDay CImumMboxScheduler::GetPrevDay(
    const TDay aToday,
    const TInt aNth )
    {
    IMUM_CONTEXT( CImumMboxScheduler::GetPrevDay, 0, KLogInApi );
    
    TInt weekIncrease = ( ( aNth / KImumDaysInWeek ) + 1 ) * KImumDaysInWeek;
    return static_cast<TDay>( ( weekIncrease + aToday - aNth ) % KImumDaysInWeek );
    }

// ----------------------------------------------------------------------------
// CImumMboxScheduler::IsValidTimeToConnect()
// ----------------------------------------------------------------------------
//
TBool CImumMboxScheduler::IsValidTimeToConnect(
    const TValidConnectionDay aValidDay,
    TTimeIntervalSeconds& aHome,
    TTimeIntervalSeconds& aStart,
    TTimeIntervalSeconds& aStop )
    {
    IMUM_CONTEXT( CImumMboxScheduler::IsValidTimeToConnect, 0, KLogInApi );
    
    TBool result = EFalse;
    
    // Combine the possible combinations.
    // Connection in current day is possible, only if the current day or 
    // all days are selected
    TBool currentDay = ( aValidDay == EConnectionCurrentDay ||
                         aValidDay == EConnectionAnyDay );
    // Connection can extended from previous day, only if previous day is
    // set or all days are set                         
    TBool previousDay = ( aValidDay == EConnectionPreviousDayOnly ||
                          aValidDay == EConnectionAnyDay );
                         
    // First it is needed to check that are the times even set
    // If the times are equal, the times can be considered to be valid for
    // all the day
    if ( currentDay && aStart == aStop )
        {
        result = ETrue;
        }
    // It's allowed to make the connection in following situations homeTime 
    // is between startTime and stopTime, and the connection is allowed
    // for the day. The connection is not allowed to be open during 
    // the last minute (aHome < aStop), but connection should be 
    // made at the start of the scheduling (aStart <= aHome).
    else if ( aStart <= aHome && aHome < aStop )
        {
        // If connection extends from previous day and only extended connection
        // is allowed
        if ( previousDay && aStart.Int() < 0 )
            {
            
            result = ETrue;
            }
        // If previous day is not checked and connection extends over day
        // make sure that the correct time is used
        else if ( currentDay && aStart.Int() >= 0 )
            {
            
            result = ETrue;
            }
        else
            {
            
            result = EFalse;
            }
        }
    // Neither of the statements were fulfilled, do not allow connection
    else
        {
        
        result = EFalse;
        }

    return result;    
    }

// ----------------------------------------------------------------------------
// CImumMboxScheduler::AdjustHomeTime()
// ----------------------------------------------------------------------------
//
TDateTime CImumMboxScheduler::AdjustHomeTime(
    const TTime& aClock )
    {
    IMUM_CONTEXT( CImumMboxScheduler::AdjustHomeTime, 0, KLogInApi );
    
    // Zero time is the beginning of the day
    TTime zeroTime( 0 );

    TDateTime dtHome = aClock.DateTime();
    TDateTime dtZero = zeroTime.DateTime();

    dtZero.SetHour( dtHome.Hour() );
    dtZero.SetMinute( dtHome.Minute() );
    dtZero.SetSecond( dtHome.Second() );
    
    return dtZero;    
    }

// ----------------------------------------------------------------------------
// CImumMboxScheduler::Times2Seconds()
//
// This function converts home-, start- and stop times to secondes from the
// beginning of the day
// ----------------------------------------------------------------------------
//
void CImumMboxScheduler::Times2Seconds(
    const TTime& aClock,
    TTimeIntervalSeconds& aHome,
    TTimeIntervalSeconds& aStart,
    TTimeIntervalSeconds& aStop )
    {
    IMUM_CONTEXT( CImumMboxScheduler::Times2Seconds, 0, KLogInApi );
    
    
    TTime zeroTime( 0 );
    TTime adjustedHomeTime = AdjustHomeTime( aClock );
    TTime selectedTimeStart( iStartTime );
    TTime selectedTimeStop( iStopTime );
    adjustedHomeTime.SecondsFrom( zeroTime, aHome );
    selectedTimeStart.SecondsFrom( zeroTime, aStart );
    selectedTimeStop.SecondsFrom( zeroTime, aStop );
    
    }

// ----------------------------------------------------------------------------
// CImumMboxScheduler::OrganizeStartAndStopTimes()
// ----------------------------------------------------------------------------
//
void CImumMboxScheduler::OrganizeStartAndStopTimes(
    TTimeIntervalSeconds& aHome,
    TTimeIntervalSeconds& aStart,
    TTimeIntervalSeconds& aStop )
    {
    IMUM_CONTEXT( CImumMboxScheduler::OrganizeStartAndStopTimes, 0, KLogInApi );
    
    // The start-, stop- and home times needs to be reordered so that the
    // start is always before stop, in other words, start time is smaller
    // than stop time.
    //
    // Following cases need to be considered:
    //     aStart < aStop  (OK) -       aStart > aStop (NOK)
    // aHome < aStart < aStop       ( aHome < aStop ) < aStart  (start-24h)
    // aStart < aHome < aStop       aStop < aHome < aStart  (end+24h)
    // aStart < aStop < aHome       aStop < aStart < aHome  (end+24h)
    if ( aStart > aStop )
        {       
        // If we're between the the start and stop times,
        // move this
        if ( aHome < aStop )
            {
            // Set starting time to previous day
            aStart = ( aStart.Int() - KImumSecondsIn24Hours );
            }
        else
            {           
            // Set ending time to next day
            aStop = ( aStop.Int() + KImumSecondsIn24Hours );
            }
        }

        
    }

// ----------------------------------------------------------------------------
// CImumMboxScheduler::CalculateSecondsToNextMark()
// ----------------------------------------------------------------------------
//
void CImumMboxScheduler::CalculateSecondsToNextMark(
    TInt64 aConnectionStatus,
    TTimeIntervalSeconds& aSeconds,
    const TTime& aClock,
    const TTimeIntervalSeconds& aHome,
    const TTimeIntervalSeconds& aStart,
    const TTimeIntervalSeconds& aStop )
    {
    IMUM_CONTEXT( CImumMboxScheduler::CalculateSecondsToNextMark, 0, KLogInApi );
    
    // If all day is selected, next stop should be the midnight of the next
    // (un)scheduled day depending on should the connection be on or off
    if ( aStart == aStop )
        {
                
        aSeconds = CalcSecsToNextScheduledDay(
            aClock, aHome, 
            aConnectionStatus == MImumInMailboxUtilities::EFlagWaitingToConnect );
        
        }
    // To calculate seconds, it needs to checked, should the connection be now
    // on or off. If between, the connection should be on and seconds should
    // be calculated to next disconnect time
    else if ( aConnectionStatus != MImumInMailboxUtilities::EFlagWaitingToConnect )
        {
        
        CalcSecsToMark( aSeconds, aClock, aHome, aStop, EFalse );
        
        }
    //
    else
        {
        
        CalcSecsToMark( aSeconds, aClock, aHome, aStart, ETrue );

        }

    }

// ----------------------------------------------------------------------------
// CImumMboxScheduler::CalcSecsToMark()
// ----------------------------------------------------------------------------
//
void CImumMboxScheduler::CalcSecsToMark(
    TTimeIntervalSeconds& aSeconds,
    const TTime& aClock,
    const TTimeIntervalSeconds& aHome,
    const TTimeIntervalSeconds& aBonus,
    const TBool aScheduledDay )
    {
    IMUM_CONTEXT( CImumMboxScheduler::CalcSecsToMark, 0, KLogInApi );
      
    // If extending to next day, calculate seconds to next day
    if ( aHome > aBonus )
        {            
        aSeconds = CalcSecsToNextScheduledDay( aClock, aHome, aScheduledDay );
        aSeconds = aSeconds.Int() + aBonus.Int();
        }
    else
        {
        aSeconds = aBonus.Int() - aHome.Int();            
        }
    }
    
// ----------------------------------------------------------------------------
// CImumMboxScheduler::CalcSecsToNextScheduledDay()
// ----------------------------------------------------------------------------
//
TTimeIntervalSeconds CImumMboxScheduler::CalcSecsToNextScheduledDay(
    const TTime& aClock,
    const TTimeIntervalSeconds& aHome,
    const TBool aScheduledDay )
    {
    IMUM_CONTEXT( CImumMboxScheduler::CalcSecsToNextScheduledDay, 0, KLogInApi );
    
    TTimeIntervalSeconds seconds = 0;
    TBool hit = EFalse;
    TDay today = aClock.DayNoInWeek();
    TDay dayAfter = GetNextDay( today );
    TInt dayCount = KErrNotFound;
    
    while ( dayAfter != today && !hit )
        {
        dayCount++;
        hit = ( iDays.Flag( dayAfter ) == aScheduledDay );
        dayAfter = GetNextDay( dayAfter );
        }

    
    seconds = KImumSecondsIn24Hours - aHome.Int() +
        dayCount * KImumSecondsIn24Hours;

    
    return seconds;       
    }

// End of File

