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
*     Messaging Centre application's engine class that reads events from
*     log DB.
*
*/



#ifndef __Mce_Engine_CMceLogEngineReader_H__
#define __Mce_Engine_CMceLogEngineReader_H__

// INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <logviewchangeobserver.h>
#include "MceLogEngine.h"
#include "MceLogEngine.hrh"


// FORWARD DECLARATIONS
class CLogClient;
class CLogViewEvent;
class CMceLogEngine;

// CLASS DECLARATION

/**
 * Implements the functionality to read log events from DB.
 */
class CMceLogEngineReader : public CActive, public MLogViewChangeObserver
    {
    public:
        /**
         * Standard creation function.
         *
         * @param aObserver     Pointer to observer.
         * @return pointer to CMceLogEngineReader instance.
         */
        static CMceLogEngineReader* NewL(CMceLogEngine* aObserver);

        /**
         * Destructor. 
         */
        ~CMceLogEngineReader();

        /**
         * Starts reading the entries from log database.
         *
         * @param aFirstime If ETrue reading is started from setting the filter.
         */
        void StartReadingL();

        /**
         * Clear DR list
         *
         */
        void ClearDrListL();

    private:
        /**
         * Second phase of construction.
         */
        void ConstructL();

        /**
         * Default constructor.
         */
        CMceLogEngineReader();

        /**
         * Constructor.
         *
         * @param aObserver     Pointer to observer.
         */
        CMceLogEngineReader( CMceLogEngine* aObserver );

        /**
         * Set filters to fetch only events of certain type.
         */
        void SetSpecialFilterL();

        
    private: // From CActive
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);

    public: // from MLogViewChangeObserver
        void HandleLogViewChangeEventAddedL(TLogId aId,   TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);
        void HandleLogViewChangeEventChangedL(TLogId aId,   TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);
        void HandleLogViewChangeEventDeletedL(TLogId aId,   TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);
        
    private: // data
        /// Ref: Logs's engine
        CMceLogEngine* iObserver;
        /// Own: File server session
        RFs iFsSession;
        /// Own: Log engine
        CLogClient* iLogClient;
        /// Own: View to filtered events.
        CLogViewEvent* iLogViewEvent; 
        /// Own: Reader's state
        TMceLogEngineReaderState iReaderState;
        /// Own: Filterlist
        CLogFilterList* iFilterList;
        /// Own: How many events has been read.
        TInt iEventCounter;
        /// Own: Indicator if observer's array was reset
        TBool   iArrayReset;
        /// Own: Amount of event in view
        TInt iEventsInView;
    };

#endif // __Mce_Engine_CMceLogEngineReader_H__

// End of file
