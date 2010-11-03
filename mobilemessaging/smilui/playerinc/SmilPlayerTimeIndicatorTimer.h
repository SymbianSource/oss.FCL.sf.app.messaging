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
* Description: SmilPlayerTimeIndicatorTimer  declaration
*
*/



#ifndef SMILPLAYERTIMEINDICATORTIMER_H
#define SMILPLAYERTIMEINDICATORTIMER_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
const TInt KTimeControlMaxLength = 32;

// FORWARD DECLARATIONS
class CCoeEnv;
class MSmilPlayerTimeObserver;

// DATA TYPES

// CLASS DECLARATION

/**
* Time indicator timer class. 
*
* @lib smilplayer.lib
* @since 3.0
*/
NONSHARABLE_CLASS(CSmilPlayerTimeIndicatorTimer) : public CTimer
    {

    public: // Constructor and Destructor

        /**
        * Two-phased constructor.
        *
        * @param aObserver        Time indicator timer observer.
        * @param aCoeEnv          Control environment.
        *
        * @return Pointer to created CSmilPlayerTimeIndicatorTimer object.
        */
        static CSmilPlayerTimeIndicatorTimer* NewL( MSmilPlayerTimeObserver* aObserver,
                                                    CCoeEnv* aCoeEnv );
        
        /**
        * Destructor.
        */
        virtual ~CSmilPlayerTimeIndicatorTimer();

    public: // New functions
    
        /**
        * Starts the timer.
        *
        * @since 3.0
        */     
        void Start();

        /**
        * Pauses the timer.
        *
        * @since 3.0
        */     
        void Pause();

        /**
        * Resumes the timer.
        *
        * @since 3.0
        */     
        void Resume();

        /**
        * Stops the timer.
        *
        * @since 3.0
        */
        void Stop();

        /**
        * Notifies timer that presentation end has been reached.
        *
        * @since 3.0
        */
        void EndReached();
    
    public: // Functions from base classes
            
        /**
        * From CTimer Called when timer is triggered.
        *
        * @since 3.0
        */
        void RunL();
        
    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Constructor.
        *
        * @param aObserver        Time indicator timer observer.
        * @param aCoeEnv          Control environment.
        */
        CSmilPlayerTimeIndicatorTimer( MSmilPlayerTimeObserver* aObserver, 
                                       CCoeEnv* aCoeEnv );

        /**
        * C++ default constructor.
        */
        CSmilPlayerTimeIndicatorTimer();
        
        /**
        * Creates time string.
        */
        void CreateNewTimeStringL();
        
    private: // Data
    
        // States of the time control
        // Used for handling time value in ReportL()
        // See error EMLI-5Y9APL.
        enum TTimeControlState
            {
            // initialized state
            // set: Stop()
            EReadyForAction = 1,
            // from start to ( iDuration - 1 second)
            // set: Start(), Resume()
            ERunning,
            // paused state
            // set in Pause()
            EPaused,
            // last second before resetting to zero.
            // set internally
            EPreparingForEnd 
            };

        // Reference to the parent window
        MSmilPlayerTimeObserver* iTimeObserver;
        
        // Stores the current time value of the presentation
        TInt iCurrentValue;

        // Stores the total time of the presentation
        TInt iDurationValue;

        // Format string placeholder.
        // R_QTN_TIME_DURAT_MIN_SEC_WITH_ZERO is loaded to it in construction.
        TBuf< KTimeControlMaxLength > iTimeDuratMinSecWithZero;
        
        // Internal state.
        TInt iState;
        
        // Pointer to control environment.
        CCoeEnv* iCoeEnv;
    };

#endif  // SMILPLAYERTIMEINDICATORTIMER_H
            
// End of File  
