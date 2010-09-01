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
*       CMsgLogsObserver implementation file
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <logcli.h>
#include <logview.h>
#include <mmsconst.h>
#include <LogsApiConsts.h>
#include <AknGlobalNote.h>

#include "MsgErrorWatcher.h"
#include "MsgLogsObserver.h"

#include "MsgErrorWatcherLogging.h"

// CONSTANTS
_LIT(KMmsLogEventTypeName, "MMS");

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMsgLogsObserver::CMsgLogsObserver
//
// C++ constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CMsgLogsObserver::CMsgLogsObserver( CMsgErrorWatcher* aWatcher, RFs& aFs )
    : CActive( CActive::EPriorityStandard ),
    iWatcher( aWatcher ),
    iFs( aFs )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMsgLogsObserver::NewL
//
// Two-phased constructor.
// ---------------------------------------------------------
//
CMsgLogsObserver* CMsgLogsObserver::NewL( CMsgErrorWatcher* aWatcher, RFs& aFs )
    {
    CMsgLogsObserver* self = new ( ELeave ) CMsgLogsObserver( aWatcher, aFs );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CMsgLogsObserver::~CMsgLogsObserver
//
// Destructor
// ---------------------------------------------------------
//
CMsgLogsObserver::~CMsgLogsObserver()
    {
    Cancel();
    if (iFilterList)
        {
        iFilterList->ResetAndDestroy(); // destroy old log filters
        }
    delete iFilterList;
    delete iLogViewEvent;
    delete iLogEvent;
    delete iDummyLogEvent1;
    delete iDummyLogEvent2;
    delete iLogClient;
    delete iLogEventType;
    
    }

// ---------------------------------------------------------
// CMsgLogsObserver::ConstructL()
//
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CMsgLogsObserver::ConstructL()
    {
    iLogClient = CLogClient::NewL( iFs );
    iLogEvent = CLogEvent::NewL();

    iLogViewEvent = CLogViewEvent::NewL( *iLogClient, *this );
    iFilterList = new ( ELeave ) CLogFilterList();
    iLogClient->GetString( iDelivered, R_LOG_DEL_DONE );
    
    
    //AddNewEventType for dummy event
    AddEventTypeL();
    }
    
// ---------------------------------------------------------
// CMsgLogsObserver::AppendFiltersL
// ---------------------------------------------------------
//
void CMsgLogsObserver::AppendFiltersL()
    {
    CLogFilter* filter;

    /// Event Type: MMS
    /// Status: Delivered
    filter = CLogFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetEventType( KLogMmsEventTypeUid );
    filter->SetStatus( iDelivered );
    iFilterList->AppendL( filter );
    CleanupStack::Pop(); //filter

	// Event Type: MMS
	// Status: Read
	filter = CLogFilter::NewL();
	CleanupStack::PushL( filter );
	filter->SetEventType( KLogMmsEventTypeUid );
	filter->SetStatus( KLogsMsgReadText );
	iFilterList->AppendL( filter );
	CleanupStack::Pop(); //filter
    return;
    }

// ---------------------------------------------------------
// CMsgLogsObserver::CreateFiltersL
// ---------------------------------------------------------
//
void CMsgLogsObserver::CreateFiltersL()
    {
MEWLOGGER_WRITE( "CreateFiltersL()" );    
    iState = EMsgLogsCreatingFilters;
    AppendFiltersL();
    if( !IsActive() )
        {	
		if ( iLogViewEvent->SetFilterL( *iFilterList, iStatus ) )
			{
			MEWLOGGER_WRITE( "Setting filter succesfully started" );
		    SetActive();
		    }
		else
		    {
		    MEWLOGGER_WRITE( "Setting filter: CompleteSelf();" ); 
            CompleteSelf();
		    }
        }
    }

// ---------------------------------------------------------
// CMsgLogsObserver::DoCancel
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgLogsObserver::DoCancel()
    {
    if ( iLogViewEvent )
        {
        iLogViewEvent->Cancel();
        }
    if ( iLogClient )
        {
        iLogClient->Cancel();
        }
    }

// ---------------------------------------------------------
// CMsgLogsObserver::RunL
//
// From active object framework
// ---------------------------------------------------------
//
void CMsgLogsObserver::RunL()
    {
    switch ( iState )
        {
        case EMsgLogsIdle:
            MEWLOGGER_WRITE( "Idle state in RunL" );
            break;
        case EMsgLogsGettingEvent:
            {
            MEWLOGGER_WRITE( "call ShowNoteL()  in RunL" );
            TRAP_IGNORE( ShowNoteL() );
            iState = EMsgLogsIdle;
            break;
            }
        case EMsgLogsDeletingDummyEvent1:
            MEWLOGGER_WRITE( "call DeleteDummyEvent2()  in RunL" );
            DeleteDummyEvent2(); 
            break;
        case EMsgLogsDeletingDummyEvent2:
            iState = EMsgLogsIdle;
            break;
        case EMsgLogsCreatingFilters:
            MEWLOGGER_WRITE( "call DeleteDummyEvent1()  in RunL" ); 
            DeleteDummyEvent1(); 
            break;
        case EMsgLogsAddingEventType:
            //Event type already exists
            if ( iStatus.Int() == KErrAlreadyExists)
            	{
            	MEWLOGGER_WRITE( "Event type already exists in RunL" ); 
            	//No need to create any dummy entrys
            	//This not first boot
            	iState = EMsgLogsAddingDummyEvent2;
                CompleteSelf();
            	}
            else
            	{
            	MEWLOGGER_WRITE( "call AddDummyEvent1L()  in RunL" );	
                TRAP_IGNORE( AddDummyEvent1L() );
            	}
            break;
        case EMsgLogsAddingDummyEvent1:
            MEWLOGGER_WRITE( "call AddDummyEvent2L()  in RunL" ); 
            TRAP_IGNORE( AddDummyEvent2L() ); 
            break;   	
        case EMsgLogsAddingDummyEvent2:
            MEWLOGGER_WRITE( "call CreateFiltersL()  in RunL" ); 
            TRAP_IGNORE( CreateFiltersL() );
            break;	
        default:
            // Nothing to do but wait.
            iState = EMsgLogsIdle;
            break;
        }
    }
    
// ---------------------------------------------------------
// CMsgLogsObserver::HandleLogViewChangeEventAddedL
// ---------------------------------------------------------
//
void CMsgLogsObserver::HandleLogViewChangeEventAddedL(TLogId aId, TInt /*aViewIndex*/, TInt /*aChangeIndex*/, TInt /*aTotalChangeCount*/)
    {
    GetLogEventL( aId );
    }

// ---------------------------------------------------------
// CMsgLogsObserver::HandleLogViewChangeEventChangedL
// ---------------------------------------------------------
//
void CMsgLogsObserver::HandleLogViewChangeEventChangedL(TLogId aId, TInt /*aViewIndex*/, TInt /*aChangeIndex*/, TInt /*aTotalChangeCount*/)
    {
    GetLogEventL( aId );
    }

// ---------------------------------------------------------
// CMsgLogsObserver::HandleLogViewChangeEventDeletedL
// ---------------------------------------------------------
//
void CMsgLogsObserver::HandleLogViewChangeEventDeletedL(TLogId /*aId*/, TInt /*aViewIndex*/, TInt /*aChangeIndex*/, TInt /*aTotalChangeCount*/)
    {
    // We're not interested.
    }
 
// ---------------------------------------------------------
// CMsgLogsObserver::GetLogEventL
// ---------------------------------------------------------
//
void CMsgLogsObserver::GetLogEventL( TLogId aId )
    {
    if ( !IsActive() )
        {
        iState = EMsgLogsGettingEvent;
        iLogEvent->SetId( aId );
        iLogClient->GetEvent( *iLogEvent, iStatus );
        SetActive();
        }
    // else we missed this one!
    }

// ---------------------------------------------------------
// CMsgLogsObserver::Delivered
// ---------------------------------------------------------
//
void CMsgLogsObserver::ShowNoteL()
    {
    if ( iDelivered.Compare( iLogEvent->Status() ) == 0 &&
        !( iLogEvent->Flags() & KLogEventRead ) )
        {
        iWatcher->ShowDeliveredNoteL( iLogEvent->RemoteParty() );
        }
    else if ( iLogEvent->Status().Compare( KLogsMsgReadText ) == 0 &&
        !( iLogEvent->Flags() & KLogEventRead ) )
        {
        iWatcher->ShowReadNoteL( iLogEvent->RemoteParty() );
        }
    }
    
    
// ---------------------------------------------------------
// CMsgLogsObserver::AddEventTypeL
//
// ---------------------------------------------------------
//
void CMsgLogsObserver::AddEventTypeL()
    {
    iState = EMsgLogsAddingEventType; 
    if (!IsActive() )
    	{
        iLogEventType = CLogEventType::NewL();
        iLogEventType->SetUid( KLogMmsEventTypeUid );
        iLogEventType->SetDescription( KMmsLogEventTypeName );
        iLogEventType->SetLoggingEnabled( ETrue );
        iLogClient->AddEventType( *iLogEventType, iStatus );
        SetActive();
    	}
    } 
       
// ---------------------------------------------------------
// CMsgLogsObserver::AddDummyEvent1L
//
// ---------------------------------------------------------
//
void CMsgLogsObserver::AddDummyEvent1L() 
	{
	iState = EMsgLogsAddingDummyEvent1;
    if (!IsActive())
    	{	  
        iDummyLogEvent1 = CLogEvent::NewL();
        iDummyLogEvent1->SetEventType( KLogMmsEventTypeUid );
        iDummyLogEvent1->SetStatus( iDelivered );
        iLogClient->AddEvent( *iDummyLogEvent1, iStatus );    
        SetActive();
    	}    
	}
	
	
// ---------------------------------------------------------
// CMsgLogsObserver::AddDummyEvent2L
//
// ---------------------------------------------------------
//
void CMsgLogsObserver::AddDummyEvent2L() 
	{
	iState = EMsgLogsAddingDummyEvent2;
    if (!IsActive())
    	{	  
        iDummyLogEvent2 = CLogEvent::NewL();
        iDummyLogEvent2->SetEventType( KLogMmsEventTypeUid );
        iDummyLogEvent2->SetStatus( KLogsMsgReadText );
        iLogClient->AddEvent( *iDummyLogEvent2, iStatus );    
        SetActive();
    	}    
	}	
	
// ---------------------------------------------------------
// CMsgLogsObserver::DeleteDummyEvent1
//
// ---------------------------------------------------------
//
void CMsgLogsObserver::DeleteDummyEvent1() 
	{
	iState = EMsgLogsDeletingDummyEvent1;
    if (!IsActive() && iDummyLogEvent1 )
    	{	 
        TLogId dummyId = iDummyLogEvent1->Id();
        iLogClient->DeleteEvent( dummyId, iStatus );    
        SetActive();
    	}   
    else
    	{
    	iState = EMsgLogsIdle;
    	CompleteSelf();
    	}	 
	}
	
// ---------------------------------------------------------
// CMsgLogsObserver::DeleteDummyEvent2
//
// ---------------------------------------------------------
//
void CMsgLogsObserver::DeleteDummyEvent2() 
	{
	iState = EMsgLogsDeletingDummyEvent2;
    if (!IsActive() && iDummyLogEvent2 )
    	{	 
        TLogId dummyId = iDummyLogEvent2->Id();
        iLogClient->DeleteEvent( dummyId, iStatus );    
        SetActive();
    	}   
    else
    	{
    	iState = EMsgLogsIdle;
    	CompleteSelf();
    	}	 
	}	
	
void CMsgLogsObserver::CompleteSelf()
    {
	// If no events complete self to get to RunL()
	TRequestStatus* status = &iStatus;
	iStatus = KRequestPending;
	SetActive();	
	User::RequestComplete( status, KErrNone );
	}
		    		
// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
