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
*     Defines the CMceLogPbkMatcher class
*
*/



#ifndef __Logs_Engine_CMceLogPbkMatcher_H__
#define __Logs_Engine_CMceLogPbkMatcher_H__

// INCLUDES
#include <e32base.h>
#include <logviewchangeobserver.h>

// DATA TYPES

enum TMceLogPbkMatcherState
    {
    ECreated = 0,           // when created, no operation in progress
    EOpeningDefaultMatchStores, // opening match stores from the phonebook
    EFilteringEvent,        // filter setting
    EReadingEvent ,         // log reading is in progress
    EUpdatingEvent          // log updating is in progress
    };

// FORWARD DECLARATIONS
class CLogViewEvent;
class CLogClient;
class CLogEvent;
class CLogFilter;
class CLogFilterList;
class MMceLogPbkMatcherObserver;
class CPhoneNumberMatcher;

// CLASS DECLARATION

class CMceLogPbkMatcher : public CActive, public MLogViewChangeObserver

    {
    public:     // Constructors.
        /**
         * Standard creation function. Creates and returns a new object of this
         * class.
         *
         * @param aFsSession    reference to file server session
         * @param aObserver     pointer to observer
         *
         * @return Pointer to the new instance of this class.
         */
        static CMceLogPbkMatcher* NewL( 
            RFs& aFsSession,
                                MMceLogPbkMatcherObserver* aObserver = NULL );


        /**
         * Destructor.
         */
        ~CMceLogPbkMatcher();

        /**
         * Start the process of matching and updating log entries
         */
        void StartL();


    private:    // Constructions.
    
        /**
         * Sets filters to logs engine
         */
        void SetFiltersToLogsL();

        /**
         * Configure filters to read certain log events
         */
        void SetFiltersL();

        /**
         * Configure to read log events of particular type
         *
         * @param aFilterType   filter type
         * @return Pointer to created filter
         */
        CLogFilter* SetOneFilterLC( TUid aFilterType );

        /**
         * Gets name from the phonebook
         *
         * @return ETrue if found one match, otherwise EFalse
         */
        TBool GetRemotePartyL();

        /**
         * Add correctly matching contacts to array
         *
         * @param aIdArray, parameters array
         * @param aNoDuplicateArray, result array
         * @param aNumber, matching number
         */
        void FillNoDuplicateArrayL( CContactIdArray& aIdArray,
                    CArrayFixFlat<TContactItemId>& aNoDuplicateArray,
                    const TDesC& aNumber );

    protected: // Constructions, for test access used in derived test class

        /**
         * Constructor, second phase.
         */
        void ConstructL();

        /**
         * Constructor
         *
         * @param aFsSession    reference to file server session
         * @param aObserver     pointer to observer
         */
        CMceLogPbkMatcher( 
            RFs& aFsSession, 
                    MMceLogPbkMatcherObserver* aObserver = NULL );


    private:     // Operations from CActive

        /**
         * Cancels active request.
         */
        void DoCancel();

    protected:     // Operations from CActive, derived test class

        /**
         * This is called when a request to a log engine is completed.
         */
        void RunL();

        /**
         * RunL leave handler.
         *
         * @param aError, leave code
         * @return error code or KErrNone
         */
        TInt RunError( TInt aError );

    public: // from MLogViewChangeObserver
        void HandleLogViewChangeEventAddedL(TLogId aId,   TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);
        void HandleLogViewChangeEventChangedL(TLogId aId,   TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);
        void HandleLogViewChangeEventDeletedL(TLogId aId,   TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);



    private: // data

        /// Ref: Reference to file server session
        RFs& iFsSession;

        /// Ref: pointer to observer
        MMceLogPbkMatcherObserver* iObserver;

        /// Own: Log engine
        CLogClient* iLogClient;

        /// Own: Event
        CLogEvent* iEvent;

        /// Own: Filterlist
        CLogFilterList* iFilterList;

        /// Own: View to filtered events.
        CLogViewEvent* iLogViewEvent;

        /// Own: Reader's state
        TMceLogPbkMatcherState iReaderState;

        /// Own: matcher utility class
        CPhoneNumberMatcher* iPbkS60Matcher;
    };

#endif // __Logs_Engine_CMceLogPbkMatcher_H__

// End of File
