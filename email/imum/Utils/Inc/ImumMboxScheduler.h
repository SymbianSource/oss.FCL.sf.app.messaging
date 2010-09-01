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


#ifndef CImumMboxScheduler_H
#define CImumMboxScheduler_H

// INCLUDES
#include <e32base.h>
#include <muiuflags.h>          // TMuiuFlags
#include <ImumInMailboxUtilities.h>

// CONSTANTS   
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CImumInternalApiImpl;

// CLASS DECLARATION

/**
*
*  @lib
*  @since S60 v3.2
*/
NONSHARABLE_CLASS( CImumMboxScheduler ) : public CBase
    {       
    
    private:
    
        /** */                
        enum TValidConnectionDay
            {
            ENoConnection = 0,
            EConnectionAnyDay,
            EConnectionCurrentDay,
            EConnectionPreviousDayOnly
            };
                                  
    public:  // Constructors and destructor

        /**
        * Creates object from CImumMboxScheduler and leaves it to cleanup stack
        * @since S60 v3.2
        * @return, Constructed object
        */
        static CImumMboxScheduler* NewLC( 
            CImumInternalApiImpl& aMailboxApi,
            const TMsvId aMailboxId );

        /**
        * Create object from CImumMboxScheduler
        * @since S60 v3.2
        * @return, Constructed object
        */
        static CImumMboxScheduler* NewL( 
            CImumInternalApiImpl& aMailboxApi,
            const TMsvId aMailboxId );

        /**
        * Destructor
        * @since S60 v3.2
        */
        virtual ~CImumMboxScheduler();

    public: // New functions
            
        /**
         * Check whether the always online is on or off
         *
         * @since S60 v3.2
         * @return Online status (ETrue/EFalse)
         */
        TInt64 QueryAlwaysOnlineStateL();
            
        /**
         *
         *
         * @since S60 v3.2
         */
        TInt64 SecondsToNextMark( TTimeIntervalSeconds& aSeconds );
        
    public: // Functions from base classes

    protected:  // Constructors

        /**
        * Default constructor for classCImumMboxScheduler
        * @since S60 v3.2
        * @return, Constructed object
        */
        CImumMboxScheduler( 
            CImumInternalApiImpl& aMailboxApi,
            const TMsvId aMailboxId );

        /**
        * Symbian 2-phase constructor
        * @since S60 v3.2
        */
        void ConstructL();

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions

        /**
        *
        * @since S60 v3.2
        */
        void PrepareScheduling(
            TTime& aClock,
            TTimeIntervalSeconds& aHome,
            TTimeIntervalSeconds& aStart,
            TTimeIntervalSeconds& aStop );
    
        /**
        *
        * @since S60 v3.2
        */
        TBool AlwaysOnlineOn(
            const TTime& aClock,
            TTimeIntervalSeconds& aHome,
            TTimeIntervalSeconds& aStart,
            TTimeIntervalSeconds& aStop );
                            
        /**
        *
        * @since S60 v3.2
        */
        TValidConnectionDay GetValidConnectionDay( 
            const TTime& aClock );
            
        /**
        *
        * @since S60 v3.2
        */
        TDay GetNextDay( 
            const TDay aToday, 
            const TInt aNth = 1 );
                  
        /**
        *
        * @since S60 v3.2
        */
        TDay GetPrevDay( 
            const TDay aToday, 
            const TInt aNth = 1 );
            
        /**
        *
        * @since S60 v3.2
        */
        TBool IsValidTimeToConnect( 
            const TValidConnectionDay aValidDay,
            TTimeIntervalSeconds& aHome,
            TTimeIntervalSeconds& aStart,
            TTimeIntervalSeconds& aStop );

        /**
        *
        * @since S60 v3.2
        */
        TDateTime AdjustHomeTime( 
            const TTime& aClock );
                
        /**
        *
        * @since S60 v3.2
        */
        void Times2Seconds(
            const TTime& aClock,
            TTimeIntervalSeconds& aHome,
            TTimeIntervalSeconds& aStart,
            TTimeIntervalSeconds& aStop );
                    
        /**
        *
        * @since S60 v3.2
        */
        void OrganizeStartAndStopTimes(
            TTimeIntervalSeconds& aHome,
            TTimeIntervalSeconds& aStart,
            TTimeIntervalSeconds& aStop );
                        
        /**
        *
        * @since S60 v3.2
        */
        void CalculateSecondsToNextMark(
            const TInt64 aConnectionStatus,
            TTimeIntervalSeconds& aSeconds,
            const TTime& aClock,
            const TTimeIntervalSeconds& aHome,
            const TTimeIntervalSeconds& aStart,
            const TTimeIntervalSeconds& aStop );

        /**
        *
        * @since S60 v3.2
        */
        TTimeIntervalSeconds CalcSecsToNextScheduledDay(
            const TTime& aClock, 
            const TTimeIntervalSeconds& aHome,
            const TBool aScheduledDay );

        /**
        *
        * @since S60 v3.2
        */
        void CalcSecsToMark(
            TTimeIntervalSeconds& aSeconds,
            const TTime& aClock,
            const TTimeIntervalSeconds& aHome,
            const TTimeIntervalSeconds& aBonus,
            const TBool aScheduledDay );
            
    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data

        //
        CImumInternalApiImpl&   iMailboxApi;
        // Flags
        TMuiuFlags              iDays;
        // 
        TInt64                  iStartTime;
        //
        TInt64                  iStopTime;      
        // 
        TInt                    iActive;
        //
        TMsvId                  iMailboxId;
    };

#endif //  CImumMboxScheduler_H

// End of File