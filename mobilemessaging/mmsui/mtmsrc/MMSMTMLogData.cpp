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
*       Provides methods for retrieving the log db data.
*
*/




// INCLUDE FILES
#include "MMSMTMLogData.h"
#include "CMsgDeliveryItem.h"
#include <logview.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logwrap.h>        // KLogMaxRemotePartyLength
#else
#include <logwrap.h>        // klogmaxremotepartylength
#include <logwraplimits.h>        // klogmaxremotepartylength
#include <logengdurations.h>
#endif
#include <mmsclient.h>
#include <mmsgenutils.h>
#include <cntdb.h>
#include <LogsApiConsts.h>  // klogsmsgreadtext

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================


// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// CMmsMTMLogData::CMmsMTMLogData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsMTMLogData::CMmsMTMLogData() : CActive( EPriorityStandard )
    {
    iEventStatusRead.Copy( KLogsMsgReadText );
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CMmsMTMLogData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmsMTMLogData::ConstructL(MMmsMtmDeliveryDataContainer* aContainer, const TMmsMsvEntry& aEntry )
    {
    iFs = new (ELeave) RFs;
    User::LeaveIfError( iFs->Connect() );

    iLogClient = CLogClient::NewL( *iFs );
    iEventView = CLogViewEvent::NewL( *iLogClient);
    iFilterList = new ( ELeave ) CLogFilterList;
    iParent = aContainer;
    iLoopCounter = 0;
    iEntry = aEntry;
    iState = ENone;
    iDataArray = new(ELeave) CArrayPtrFlat<CMsgDeliveryItem>(64); 
    iLogClient->GetString( iEventStatusPendingTxt, R_LOG_DEL_PENDING );
    iLogClient->GetString( iEventStatusSentTxt, R_LOG_DEL_SENT );
    iLogClient->GetString( iEventStatusFailedTxt, R_LOG_DEL_FAILED );
    iLogClient->GetString( iEventStatusNoDeliveryTxt, R_LOG_DEL_NONE );
    iLogClient->GetString( iEventStatusDeliveredTxt, R_LOG_DEL_DONE );
    iLogClient->GetString( iEventStatusNotSentTxt, R_LOG_DEL_NOT_SENT );
    // See read status creation in construction
    }

// -----------------------------------------------------------------------------
// CMmsMTMLogData::CMmsMTMLogData
// Destructor
// -----------------------------------------------------------------------------
// 
CMmsMTMLogData::~CMmsMTMLogData()
    {
    Cancel();

    if( iFilterList )
        {
        iFilterList->ResetAndDestroy();
        delete iFilterList;
        iFilterList = NULL;
        }
    if (iEventView)
        {
        delete iEventView; 
        iEventView = NULL;
        }
    if (iLogClient)
        {
        delete iLogClient;
        iLogClient = NULL;
        }

    if (iFs)
        {
        iFs->Close();
        delete iFs;
        }
    }

// -----------------------------------------------------------------------------
// CMmsMTMLogData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmsMTMLogData* CMmsMTMLogData::NewL(MMmsMtmDeliveryDataContainer* aContainer, const TMmsMsvEntry& aEntry )
    {
    CMmsMTMLogData* self = new( ELeave ) CMmsMTMLogData( );

    CleanupStack::PushL( self );
    self->ConstructL(aContainer, aEntry);
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// CMmsMTMLogData::DeliveryDataL
// Creates the necessary Log filters and starts the log db query.
// ---------------------------------------------------------
//
TBool CMmsMTMLogData::DeliveryDataL(const CMmsClientMtm* aMmsClient)
    {
    // Check for NULL. If NULL, return false (= no data found)
    if (!aMmsClient)
        {
        return EFalse;
        }
        
    MtmRecipientsToArrayL( *aMmsClient );
        
    iState = EInit;
    const CMsvRecipientList& recipients = aMmsClient->AddresseeList();
    TInt recipientCount = recipients.Count();

    Cancel(); 

    TTime sendingDate = aMmsClient->SendingDate();
    for (TInt i = 0; i < recipientCount ; i++)
        {
        TPtrC number = TMmsGenUtils::PureAddress( recipients[i] );

        CLogFilter* filter = CLogFilter::NewL();
        CleanupStack::PushL( filter );

        filter->SetEventType(KLogMmsEventTypeUid);
        filter->SetStartTime(sendingDate);
        filter->SetDurationType(KLogDurationNone);
        if ( TMmsGenUtils::IsValidMMSPhoneAddress( number, ETrue ) )
            {
            filter->SetNumber( number.Left( Min( number.Length(), KLogMaxRemotePartyLength ) ) );
            }
        else
            {
            filter->SetRemoteParty( number.Left( Min( number.Length(), KLogMaxRemotePartyLength ) ) );
            }

        iFilterList->AppendL(filter);		
        CleanupStack::Pop( filter );
        }

    //Using SetFilterParseFilterByFilterL as it works for combination of RemoteParty and Number
    if ( !iEventView->SetFilterParseFilterByFilterL( *iFilterList, iStatus ) ) // something found with these filters, start the active object
        {
        iState = EFinalize;
        iStatus = KRequestPending;
        TRequestStatus* pStatus = &iStatus;
        User::RequestComplete( pStatus, KErrNone );
        }
    SetActive();
    return ETrue;

    }

// ---------------------------------------------------------
// CMmsMTMLogData::DoCancel
// Cancels the active object, from CActive
// ---------------------------------------------------------
//  
void CMmsMTMLogData::DoCancel()
    {      
    iLogClient->Cancel();
    iEventView->Cancel();
    }

// ---------------------------------------------------------
// CMmsMTMLogData::RunL
// From CActive
// ---------------------------------------------------------
//  
void CMmsMTMLogData::RunL()
    {
    Cancel();
    if (iStatus == KErrNone)
        {
        if (iState == EInit) // init phase finished
            {
            FetchItemL();
            }
        else if (iState == EReading) // reading ongoing
            {
            CopyItemL();
            }
        else if (iState == EFinalize) // reading ongoing
            {
            SendDataL();
            }
        }

    if (iState != EFinished)
        {
        SetActive();
        }
    }

// ---------------------------------------------------------
// CMmsMTMLogData::RunError
// Handles the error, if necessary. From CActive
// ---------------------------------------------------------
//  
 TInt CMmsMTMLogData::RunError(TInt aError)
    {
    return aError;
    }
  
 // ---------------------------------------------------------
// CMmsMTMLogData::SendData
// Callback method. After finished with filling the data array,
// sends it back to the MMmsMtmDeliveryDataContainer (implemented in laucher class)
// ---------------------------------------------------------
//  
 void CMmsMTMLogData::SendDataL()
    {
    iState = EFinished;
    iDataArray->Compress();
    iParent->SetDataArrayL(iDataArray);
    }

 // ---------------------------------------------------------
// CMmsMTMLogData::AddToArray
// Converts the CLogEvent to CMsgDeliveryItem and adds it to the data array
// ---------------------------------------------------------
//  
void CMmsMTMLogData::AddToArrayL(CLogEvent* aEvent)
    {
    // Check for NULL. If null, exit
    if (!aEvent)
    {
        return;
    }
    // copy the data from the CLogEvent to CMsgDeliveryItem,
    // which will eventually be stored into the array
    CMsgDeliveryItem* item = CMsgDeliveryItem::NewL();
    CleanupStack::PushL( item );
    
    item->SetNumberL(aEvent->Number());
    item->SetRecipientL(aEvent->RemoteParty());
    item->SetTime(aEvent->Time());

    if (aEvent->Status().Compare(iEventStatusPendingTxt) == 0) // msg item only supports three statuses, so theres some mapping to do.
        {
        item->SetStatus(CMsgDeliveryItem::EPending);
        }
    else if (aEvent->Status().CompareF(iEventStatusSentTxt) == 0)
        {
        item->SetStatus(CMsgDeliveryItem::EPending);
        }
    else if (aEvent->Status().CompareF(iEventStatusFailedTxt) == 0)
        {
        item->SetStatus(CMsgDeliveryItem::EFailed);
        }
    else if (aEvent->Status().CompareF(iEventStatusNoDeliveryTxt) == 0)
        {
        item->SetStatus(CMsgDeliveryItem::EFailed);
        }
    else if (aEvent->Status().CompareF(iEventStatusDeliveredTxt) == 0)
        {
        item->SetStatus(CMsgDeliveryItem::EDelivered);
        }
    else if (aEvent->Status().CompareF(iEventStatusRead) == 0)
        {
        item->SetStatus(CMsgDeliveryItem::ERead);
        }
    else if (aEvent->Status().CompareF(iEventStatusNotSentTxt) == 0)
        {
        item->SetStatus(CMsgDeliveryItem::EFailed);
        }
    else 
        {
        item->SetStatus(CMsgDeliveryItem::ENone);
        }

    iDataArray->InsertL( iInsertedCount, item, 1 ); // add the item to the array
    iInsertedCount++;
    TPtrC recip = aEvent->RemoteParty();
    DeleteRecipent( recip, EFalse, iInsertedCount ); 
    CleanupStack::Pop( item );
    }

 // ---------------------------------------------------------
// CMmsMTMLogData::FetchItem
// Starts to fetch the next CLogEvent from the CLogViewEvent
// ---------------------------------------------------------
//
void CMmsMTMLogData::FetchItemL()
    {
    iState = EReading;
    if (iLoopCounter == 0) // the first one needs(?) to be fecthed differently
        {
        iEventView->FirstL(iStatus);
        }
    else 
        {
        iEventView->NextL(iStatus);				
        }
    }
 // ---------------------------------------------------------
// CMmsMTMLogData::CopyItem
// Copies the next CLogEvent from the CLogViewEvent
// ---------------------------------------------------------
//  
void CMmsMTMLogData::CopyItemL()	
    {
    CLogEvent* event = CLogEvent::NewL();
    CleanupStack::PushL(event);
    event->CopyL(iEventView->Event());
    iLoopCounter++;

    TLogLink event_id = event->Link(); 
    TMsvId message_id = iEntry.Id();

    if (event_id ==  message_id) // check the message id from the event
        {
        AddToArrayL(event);
        }

    CleanupStack::PopAndDestroy(event);

    if (iLoopCounter >= iEventView->CountL())  // all the log events are now handled
        {
        iState = EFinalize;
        iStatus = KRequestPending;
        TRequestStatus* pStatus = &iStatus;
        User::RequestComplete( pStatus, KErrNone );
        } 

    else  // still some log events left, continue fetching them one by one
        {
        FetchItemL(); 
        }
    }

// ---------------------------------------------------------
// CMmsMTMLogData::MtmRecipientsToArrayL
// CLogViewEvent::SetFilterL() didn't start async operation.
// Just copy recipients into 'destination'.
// Calling this function means showing 'unavailable' status 
// ---------------------------------------------------------
//  
void CMmsMTMLogData::MtmRecipientsToArrayL( const CMmsClientMtm& aMmsClient )
    {
    const CMsvRecipientList& recipients = aMmsClient.AddresseeList();
    TInt recipientCount = recipients.Count();

    TTime sendingDate = aMmsClient.SendingDate();
    
    HBufC* aliasBuf = HBufC::NewLC( KLogMaxRemotePartyLength );
    TPtr aliasPtr = aliasBuf->Des();
    
    for (TInt i = 0; i < recipientCount ; i++)
        {        
        TPtrC recip = TMmsGenUtils::PureAddress( recipients[i] );
        TBool isNumber = TMmsGenUtils::IsValidMMSPhoneAddress( recip, ETrue );

        CMsgDeliveryItem* item = CMsgDeliveryItem::NewL();
        CleanupStack::PushL( item );
        
        item->SetStatus( CMsgDeliveryItem::ENone );
        item->SetTime( sendingDate );

        if ( isNumber )
            {
            item->SetNumberL( recip );
            item->SetRecipientL( recip );
            
            // get alias too
            TInt error = TMmsGenUtils::GetAlias( 
                recip,
                aliasPtr,
                KLogMaxRemotePartyLength,
                *iFs );
            if ( error == KErrNone && aliasPtr.Length() > 0 )
                {            
                item->SetRecipientL( aliasPtr );
                aliasPtr.Zero();
                }
            else
                {
                // recipient must be there
                }
            }
        else
            {
            item->SetRecipientL( recip );
            }
        
        iDataArray->AppendL(item); // add the item to the array
        CleanupStack::Pop( item );
        }
    CleanupStack::PopAndDestroy( aliasBuf );
    }
    
    
// ---------------------------------------------------------
// CMmsMTMLogData::DeleteRecipent
// Checks whether given recipient is already in the iDataArray
// ---------------------------------------------------------
//  
void CMmsMTMLogData::DeleteRecipent(    TDesC&  aRecipient,
                                        TBool   aType,
                                        TInt    aInitialIndex )
    {
    TInt count = iDataArray->Count( );
    for( TInt i = aInitialIndex; i < count ; i++ )
        {
        CMsgDeliveryItem* item = iDataArray->At(i);
        if ( item )
            {
            TPtrC recip;
            if ( aType )
                {
                recip.Set( item->GetNumber() );
                }
            else
                {
                recip.Set( item->GetRecipient() );
                }
            if ( !recip.CompareF( aRecipient ) )
                {
                delete item;
                iDataArray->Delete( i );
                break;
                }
            }
        }
    }                                       



