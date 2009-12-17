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
*     Defines methods for CMceLogPbkMatcher
*
*/



// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logfilterandeventconstants.hrh>
#include <logengevents.h> 
#endif
#include <e32base.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <CPbkFieldsInfo.h>
#include <logview.h>
#include <logcli.h>

#include "MceLogPbkMatcher.h"
#include "MceLogPbkMatcherObserver.h"
#include "MceLogEngine.hrh"

#include "CPhoneNumberMatcher.h"
#include <LogsApiConsts.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================


CMceLogPbkMatcher::CMceLogPbkMatcher(  
    RFs& aFsSession,
                    MMceLogPbkMatcherObserver* aObserver)
                    : CActive( EPriorityHigh ),
                    iFsSession( aFsSession ),
                    iObserver( aObserver )
    {
    }


CMceLogPbkMatcher* CMceLogPbkMatcher::NewL(   
    RFs& aFsSession,
                                        MMceLogPbkMatcherObserver* aObserver )
    {
    CMceLogPbkMatcher* self = new ( ELeave ) CMceLogPbkMatcher( 
        aFsSession,
        aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

void CMceLogPbkMatcher::ConstructL()
    {
    iReaderState = ECreated;
    iLogClient = CLogClient::NewL( iFsSession );
    iLogViewEvent = CLogViewEvent::NewL( *iLogClient, *this );
    iEvent = CLogEvent::NewL();
    SetFiltersL();
    CActiveScheduler::Add( this );
    iPbkS60Matcher = CPhoneNumberMatcher::NewL( iFsSession );
    }


CMceLogPbkMatcher::~CMceLogPbkMatcher()
    {
    Cancel();
    delete iPbkS60Matcher;
    delete iLogViewEvent;
    delete iLogClient;
    delete iEvent;
    if( iFilterList )
        {
        iFilterList->ResetAndDestroy();
        delete iFilterList;
        }
    }

void CMceLogPbkMatcher::StartL()
    {
    Cancel();
    
    if ( iReaderState == ECreated || iReaderState == EOpeningDefaultMatchStores )
        {
        iPbkS60Matcher->OpenDefaultMatchStoresL( iStatus );
        iReaderState = EOpeningDefaultMatchStores;
        SetActive();
        }
    else
        {
        SetFiltersToLogsL();
        }
    }
    
void CMceLogPbkMatcher::SetFiltersToLogsL()
    {
    TBool rc = ( EFalse );
    iReaderState = EFilteringEvent;

    TRAPD( err, ( rc = iLogViewEvent->SetFilterL( *iFilterList, iStatus ) ) );
    if( err )
        {
        if( err == KErrAccessDenied ) // backup active
            {
            return;
            }
        else 
            {
            User::Leave( err );
            }
        }

    if( rc )
        {
        SetActive();
        }
    }


void CMceLogPbkMatcher::SetFiltersL()
    {
    if( iFilterList )
        {
        iFilterList->ResetAndDestroy();
        delete iFilterList;
        iFilterList = NULL;
        }
    iFilterList = new ( ELeave ) CLogFilterList();

    /// Event type: SMS
    iFilterList->AppendL( SetOneFilterLC( KLogShortMessageEventTypeUid ) );
    CleanupStack::Pop();    // filter set in SetOneFilterLC

    /// Event type: MMS
    iFilterList->AppendL( SetOneFilterLC( KLogsEngMmsEventTypeUid ) );
    CleanupStack::Pop();

    // Set filter with empty remote party
    CLogFilter* filter = CLogFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetNullFields( ELogRemotePartyField );
    iFilterList->AppendL( filter );
    CleanupStack::Pop();    // filter
    }


CLogFilter* CMceLogPbkMatcher::SetOneFilterLC( TUid aFilterType )
    {
    CLogFilter* filter = CLogFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetEventType( aFilterType ); // certain type;
    filter->SetFlags( KLogEventContactSearched ); // no flag or remoteparty
    filter->SetFlags( KLogEventRead ); // event has not been cleared
    filter->SetNullFields( ELogFlagsField | ELogRemotePartyField );
    return filter;
    }



void CMceLogPbkMatcher::DoCancel()
    {
    iPbkS60Matcher->CancelOperation();
    iLogClient->Cancel();
    iLogViewEvent->Cancel();
    }


TBool CMceLogPbkMatcher::GetRemotePartyL()
    {
    TBool result (EFalse );
    if( iPbkS60Matcher->MatchCountL( iEvent->Number()) == 1 )
        {
        result = ETrue;
        }
    return result;
    }

void CMceLogPbkMatcher::RunL()
    {

    if( iStatus == KErrNone )
        {
        switch( iReaderState )
            {
            case EOpeningDefaultMatchStores:
                // now stores are open, continue with filtering
                SetFiltersToLogsL();
            break;
            case EFilteringEvent:
                if( iLogViewEvent->FirstL( iStatus ) )
                    {
                    iReaderState = EReadingEvent;
                    SetActive();
                    }
                else if( iObserver )
                    {
//                    iObserver->PbkMatchingDoneL();
                    }

                break;

            case EReadingEvent:

                iEvent->CopyL( iLogViewEvent->Event() );
                iEvent->SetFlags( KLogEventContactSearched ); 
                if( GetRemotePartyL() )
                    {
                    iEvent->SetRemoteParty( iPbkS60Matcher->GetUniqueName() );
                    }
                iReaderState = EUpdatingEvent;
                iLogClient->ChangeEvent( *iEvent, iStatus );
                SetActive();
                break;

            case EUpdatingEvent:

                if( iLogViewEvent->FirstL( iStatus ) )
                    {
                    iReaderState = EReadingEvent;
                    SetActive();
                    }

                else if( iObserver )
                    {
//                    iObserver->PbkMatchingDoneL();
                    }

                break;

            default:
                break;

            }
        }
    else if ( iReaderState == EOpeningDefaultMatchStores )
        {
        iReaderState = ECreated;
        }
    }

TInt CMceLogPbkMatcher::RunError(TInt aError)
    {
    if( aError == KErrAccessDenied ) // backup active
        {
        return KErrNone;
        }
    else
        {
        return aError;
        }
    }

void CMceLogPbkMatcher::HandleLogViewChangeEventAddedL(
                TLogId /*aId*/,   
                TInt /*aViewIndex*/, 
                TInt /*aChangeIndex*/, 
                TInt /*aTotalChangeCount*/)
    {
    StartL();
    }

void CMceLogPbkMatcher::HandleLogViewChangeEventChangedL(
                TLogId /*aId*/,   
                TInt /*aViewIndex*/, 
                TInt /*aChangeIndex*/, 
                TInt /*aTotalChangeCount*/)
    {
    StartL();
    }

void CMceLogPbkMatcher::HandleLogViewChangeEventDeletedL(
                TLogId /*aId*/,   
                TInt /*aViewIndex*/, 
                TInt /*aChangeIndex*/, 
                TInt /*aTotalChangeCount*/)
    {
    StartL();
    }


// End of File

