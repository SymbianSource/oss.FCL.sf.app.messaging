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



// INCLUDE FILES

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logfilterandeventconstants.hrh>
#endif
#include <coemain.h>            // for CCoeEnv
#include <logview.h>            // CLogViewEvent
#include "DllMain.h"			// panic function
#include "MceLogEngineReader.h"
#include <LogsApiConsts.h>

// ================= MEMBER FUNCTIONS =======================

CMceLogEngineReader::CMceLogEngineReader(
    CMceLogEngine* aObserver)
    : CActive(EPriorityStandard),
      iObserver(aObserver)
    {
    }

CMceLogEngineReader* CMceLogEngineReader::NewL(
    CMceLogEngine* aObserver)
    {
    CMceLogEngineReader* self = new (ELeave) CMceLogEngineReader(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

void CMceLogEngineReader::ConstructL()
    {
    User::LeaveIfError(iFsSession.Connect());
    iLogClient = CLogClient::NewL(iFsSession);
    iLogViewEvent = CLogViewEvent::NewL(*iLogClient, *this);
    SetSpecialFilterL();
    CActiveScheduler::Add(this);
    }

CMceLogEngineReader::~CMceLogEngineReader()
    {
    Cancel();

    delete iLogViewEvent;
    delete iLogClient;

    iFsSession.Close();

    if (iFilterList)
        {
        iFilterList->ResetAndDestroy();
        delete iFilterList;
        }
    }


void CMceLogEngineReader::StartReadingL()
    {
    Cancel();
    iEventCounter = 0;
    iEventsInView = 0;
    iArrayReset = EFalse;

    if( iLogViewEvent->SetFilterL(*iFilterList, iStatus) )
        {
        iReaderState = ESettingFilter;
        SetActive();
        }
    else if( iObserver )
        {
        iObserver->ResetArray();
        iObserver->ReadEventsCompletedL();
        }
    }


void CMceLogEngineReader::DoCancel()
    {
    /// cancel filter setting or event reading
    iLogViewEvent->Cancel();
    }

TInt CMceLogEngineReader::RunError(TInt aError)
    {
    if( aError == KErrAccessDenied )
        {
        return KErrNone;
        }
    else
        {
        return aError;
        }
    }


void CMceLogEngineReader::RunL()
    {
    CLogEvent* event;

    if (iStatus != KErrNone)
        {
        // reading entries failed somehow.
        iObserver->ReadEventsCompletedL( EFalse );
        }
    else
        {
        switch (iReaderState)
            {
            /// filter setting asyncronous request completed
            case ESettingFilter:

                if (iLogViewEvent->CountL() == 0)
                    {
                    iObserver->ResetArray();
                    iObserver->ReadEventsCompletedL();
                    }
                else
                    {
                    /// start first event reading asyncronous request
                    iReaderState = EReadingEntry;
                    iLogViewEvent->FirstL(iStatus);
                    SetActive();
                    }
                break;

            /// event reading asyncronous request completed
            case EReadingEntry:

                if( ! iArrayReset )
                    {
                    iArrayReset = ETrue;
                    iObserver->ResetArray();
                    }

                event = CLogEvent::NewL();
                CleanupStack::PushL( event );
                event->CopyL(iLogViewEvent->Event());

                iEventsInView++;

                if( event->Number().Length() || event->RemoteParty().Length() )
                    {
                    iObserver->AddEventToListL( event );
                    CleanupStack::Pop(); // event
                    iEventCounter++;
                    }
                else
                    {
                    CleanupStack::PopAndDestroy(); // event
                    }

                if ( iEventsInView < iLogViewEvent->CountL() &&
                                        iEventCounter < KMceLogEngFetchLimit )
                    {
                    /// read next event
                    iLogViewEvent->NextL( iStatus );
                    SetActive();
                    }
                else
                    {
                    /// inform observer, that reading is done
                    iObserver->ReadEventsCompletedL();
                    }
                break;

            default:
                break;

                }
            }
    }


void CMceLogEngineReader::SetSpecialFilterL()
    {
    /// create filter array to pass through only
    /// events needed in Delivery Report

    CLogFilter* filter;

    if( iFilterList )
        {
        iFilterList->ResetAndDestroy();
        delete iFilterList;
        iFilterList = NULL;
        }
    iFilterList = new (ELeave) CLogFilterList();

    /// Direction: OUT
    TLogString inOutName;
    iLogClient->GetString(inOutName, R_LOG_DIR_OUT);
    filter = CLogFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetDirection(inOutName);
    filter->SetFlags( KLogEventRead );
    filter->SetNullFields( ELogFlagsField );
    iFilterList->AppendL(filter);
    CleanupStack::Pop(); //filter

    /// Event Type: SMS
    filter = CLogFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetEventType(KLogShortMessageEventTypeUid);
    iFilterList->AppendL(filter);
    CleanupStack::Pop(); //filter

    /// Event Type: MMS
    filter = CLogFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetEventType( KLogsEngMmsEventTypeUid );
    iFilterList->AppendL(filter);
    CleanupStack::Pop(); //filter

    /// Status: Pending
    TLogString pending;
    iLogClient->GetString(pending, R_LOG_DEL_PENDING);
    filter = CLogFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetStatus( pending );
    iFilterList->AppendL(filter);
    CleanupStack::Pop(); //filter

    /// Status: Delivered
    TLogString done;
    iLogClient->GetString(done, R_LOG_DEL_DONE);
    filter = CLogFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetStatus( done );
    iFilterList->AppendL(filter);
    CleanupStack::Pop(); //filter

    /// Status: Failed
    TLogString failed;
    iLogClient->GetString(failed, R_LOG_DEL_FAILED);
    filter = CLogFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetStatus( failed );
    iFilterList->AppendL(filter);
    CleanupStack::Pop(); //filter
      
    /// Event Type: MMS
    /// Status: Read
    filter = CLogFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetEventType( KLogsEngMmsEventTypeUid );
    filter->SetStatus( KLogsMsgReadText );
    iFilterList->AppendL( filter );
    CleanupStack::Pop(); //filter

    }

void CMceLogEngineReader::HandleLogViewChangeEventAddedL(
                TLogId /*aId*/,   
                TInt /*aViewIndex*/, 
                TInt /*aChangeIndex*/, 
                TInt /*aTotalChangeCount*/ )
    {
    StartReadingL();
    }

void CMceLogEngineReader::HandleLogViewChangeEventChangedL(
                TLogId /*aId*/,   
                TInt /*aViewIndex*/, 
                TInt /*aChangeIndex*/, 
                TInt /*aTotalChangeCount*/ )
    {
    StartReadingL();
    }

void CMceLogEngineReader::HandleLogViewChangeEventDeletedL(
                TLogId /*aId*/,   
                TInt /*aViewIndex*/, 
                TInt /*aChangeIndex*/, 
                TInt /*aTotalChangeCount*/ )
    {
    StartReadingL();
    }

void CMceLogEngineReader::ClearDrListL()
    {
    __ASSERT_DEBUG( iLogViewEvent, LogsEnginePanic( ELogsEnginePanicPreCond ) );
    iLogViewEvent->SetFlagsL( KLogEventRead );
    StartReadingL();
    }

// End of file
