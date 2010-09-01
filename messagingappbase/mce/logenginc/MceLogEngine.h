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
*     This class communicates with the log db. This also handles all the
*     filtering options and registers Mce to LogEng to get notidied about
*     changes in log DB.
*
*/



#ifndef __Mce_Engine_CMceLogEngine_H__
#define __Mce_Engine_CMceLogEngine_H__

// INCLUDES
#include <logcli.h>         // for TLogConfig
#include <cntdef.h>
#include <logwrap.h>        // for LogEvent, TLogRecentList, TLogId


#include "MceLogEngine.hrh"
#include "MceLogNotify.h"  // Log DB observer class.
#include "MceLogPbkMatcherObserver.h"
#include <LogsApiConsts.h>

// FORWARD DECLARATIONS
class CMceLogEngineReader;
class CMceLogPbkMatcher;
class MMceLogEngObserver;
class CMceLogEngineReader;

// CLASS DECLARATION

/**
 * This class communicates with the log db.  This also handles all the
 * filtering options and registers Mce to LogEng to get notified about
 * changes in log DB.
 */
class CMceLogEngine : public CBase,
                      public MMceLogPbkMatcherObserver
    {
    public: // Construction/destruction
        /**
         * Standard creation function. Creates and returns a new object of this
         * class.
         *
         * @return pointer to CMceLogEngine instance.
         */
        IMPORT_C static CMceLogEngine* NewL();


        /**
         *  Destructor.
         */
        ~CMceLogEngine();

        /**
         * Starts reading events from log DB.
         *
         */
        void UpdateEventsL();

        /**
         * Returns const pointer to an event from iEventArray
         *
         * @param aIndex        Index of the event to return.
         * @return CLogEvent*   Pointer to returned event.
         */
        IMPORT_C const CLogEvent* Event(TInt aIndex);

        /**
         * Confirmation that reading entries from log engine is completed.
         *
         * @param aStatus       ETrue if reading succeeded, otherwise EFalse
         */
        void ReadEventsCompletedL( TBool aStatus = ETrue );

        /**
         * Informs model when log db state has changed.
         */
        void NotifyDbChangedL();

        /**
         * Add event to the list of events (iEventArray).
         *
         * @param aEvent The event to be added into iEventArray.
         */
        void AddEventToListL( CLogEvent* aEvent );

        /**
         * Resets event array
         */
        void ResetArray();

        /**
         * Set model observer for MLogsModelObserver class.
         *
         * @param aContainerObserver Pointer to container.
         */
        IMPORT_C void SetContainerObserver( MMceLogEngObserver* aContainerObserver );

        /**
         * Remove model observer to "Delivery Report" view container class.
         */
        IMPORT_C void RemoveContainerObserver();

        /**
         * Purge data to iLogSmsPduData class.
         *
         * @param aEvent The event which data field has to be purged.
         * @param aData pointer to array to put results in
         * @return total count of SMS messages in case of pending
         */
        IMPORT_C TInt PurgeData( const CLogEvent* aEvent, CArrayFixFlat<TInt>* aData );

        /**
         * Count of events
         *
         * @return count of log events
         */
        IMPORT_C TInt Count() const;

        /**
         * Gets standard strings from logwrap.dll
         *
         * @param aFailed reference to buffer
         * @param aPending reference to buffer
         */
        IMPORT_C void GetStandardStrings( TDes& aFailed, TDes& aPending );

        /**
         * Used to find out if phone number is available for a call
         *
         * @param aIndex index of currently selected item
         * @return ETrue if number is available -> call is possible
         */
        IMPORT_C TBool IsNumberAvailable( TInt aIndex );

        /**
         * Called by UI when foreground is gained
         */
        IMPORT_C void ForegroundGainedL();

        /**
         * Called by UI when foreground is lost
         */
        IMPORT_C void ForegroundLost();

        /**
         * Clear dr list.
         */
        IMPORT_C void ClearDrListL();

    public:

        void PbkMatchingDoneL(); // from MMceLogPbkMatcherObserver

    private:
        /*
         * Constructor.
         */
        CMceLogEngine();

        /**
         * Constructor, second phase.
         */
        void ConstructL();

    private:    // data
        /// Own: engine reader
        CMceLogEngineReader* iEngineReader;

        /// Own: array of events
        CArrayPtrFlat<CLogEvent>* iEventArray;

        /// Own: LogEng client object.
        CLogClient* iLogClient;

        /// Own: File System session
        RFs iFsSession;

        /// Own: Log db observer class.
        CMceLogNotify* iLogNotify;

        /// Ref: Engine observer.
        MMceLogEngObserver* iContainerObserver;

        /// Own: Foreground / Background indicator
        TBool iForeground;

        /// Own: DB changed / Not changed indicator
        TBool iDbDirty;

        /// Own: event matcher with phonebook
        CMceLogPbkMatcher* iPbkMatcher;

    };


#endif // __Mce_Engine_CMceLogEngine_H__


// End of file
