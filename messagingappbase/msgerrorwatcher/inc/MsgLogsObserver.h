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
* Description:  
*     CMsgLogsObserver declaration file
*
*/



#ifndef __MSGLOGSOBSERVER_H
#define __MSGLOGSOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <logviewchangeobserver.h>

// FORWARD DECLARATIONS
class CMsgErrorWatcher;
class CLogClient;
class CLogViewEvent;
class CLogFilterList;
class CLogEventType;

// CLASS DECLARATION

/**
* CMsgLogsObserver
*
* @lib msgerrorwatcher.dll
* @since 3.0
*/
class CMsgLogsObserver : public CActive, public MLogViewChangeObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcher
        */
        static CMsgLogsObserver* NewL( CMsgErrorWatcher* aWatcher, RFs& aFs );
        
        /**
        * Destructor.
        */
        virtual ~CMsgLogsObserver();

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Private C++ constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcher
        */
        CMsgLogsObserver( CMsgErrorWatcher* aWatcher, RFs& aFs );

        /**
        *
        */
        void CreateFiltersL();
        
        /**
        *
        */
        void AppendFiltersL();
        
        /**
        *
        */
        void GetLogEventL( TLogId aId );
        
        /**
        *
        */
        void ShowNoteL();
        
        /**
        *
        */
        void AddEventTypeL();
        
         /**
        *
        */
        void AddDummyEvent1L();
        
        /**
        *
        */
        void AddDummyEvent2L();
        
        /**
        *
        */
        void DeleteDummyEvent1();
        
        /**
        *
        */
        void DeleteDummyEvent2();
        
        /**
        *
        */
        void CompleteSelf();
        
    private:

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        void RunL();

    private:
    
        /**
        * From MLogViewChangeObserver
        */
        void HandleLogViewChangeEventAddedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);
        void HandleLogViewChangeEventChangedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);
        void HandleLogViewChangeEventDeletedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);

    private:    // Data

        enum TMsgLogsObserverState
            {
            EMsgLogsIdle = 0,
            EMsgLogsAddingEventType,
            EMsgLogsAddingDummyEvent1,
            EMsgLogsAddingDummyEvent2,
            EMsgLogsCreatingFilters,
            EMsgLogsDeletingDummyEvent1,
            EMsgLogsDeletingDummyEvent2,
            EMsgLogsGettingEvent
            };
            
        CMsgErrorWatcher* iWatcher;
        RFs& iFs;
        
        TMsgLogsObserverState iState;
        
        CLogClient* iLogClient;
        CLogEvent* iLogEvent;
        CLogEvent* iDummyLogEvent1;
        CLogEvent* iDummyLogEvent2;
        CLogViewEvent* iLogViewEvent;
        CLogFilterList* iFilterList;
        CLogEventType*  iLogEventType;  // to add mms event type

        TLogString iDelivered;
        TLogId iDummyLogId;
    };

#endif      // __MSGLOGSOBSERVER_H
            
// End of File
