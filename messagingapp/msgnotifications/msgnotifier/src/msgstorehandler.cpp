/*
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Message Store Handling for Indications
 *
 */

#include "msgstorehandler.h"
#include <xqconversions.h>
#include "msgnotifier_p.h"
#include <ccsrequesthandler.h>
#include <ccsconversationentry.h>
#include <ccsclientconversation.h>

#include <msvids.h>
#include <mmsconst.h>
#include <SendUiConsts.h>
#include <msvsearchsortquery.h>
#include <msvsearchsortoperation.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Default constructor.
// ---------------------------------------------------------
//
MsgStoreHandler::MsgStoreHandler(MsgNotifierPrivate* notifier, CCSRequestHandler* aCvServer) :
    iMsvSession(NULL), iNotifier(notifier), iRequestHandler(aCvServer)
{
    TRAP_IGNORE(InitL());
}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
//
MsgStoreHandler::~MsgStoreHandler()
{
    if (iMsvEntry) {
        delete iMsvEntry;
        iMsvEntry = NULL;
    }

    if (iMsvSession) {
        delete iMsvSession;
        iMsvSession = NULL;
    }

    if (iFailedMessages) {
        delete iFailedMessages;
        iFailedMessages = NULL;
    }

}

// ---------------------------------------------------------
// InitL( )
// Initialize the Store handler.
// ---------------------------------------------------------
void MsgStoreHandler::InitL()
{
    iMsvSession = CMsvSession::OpenSyncL(*this);
    iMsvEntry = iMsvSession->GetEntryL(KMsvGlobalOutBoxIndexEntryId);
    iMsvEntry->AddObserverL(*this);

    iFailedMessages = new (ELeave) CMsvEntrySelection;
}

// ---------------------------------------------------------
// MsgStoreHandler::HandleSessionEventL()
// ---------------------------------------------------------
//
void MsgStoreHandler::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* /*aArg3*/)
{
    CMsvEntrySelection* selection = NULL;
    TMsvId parent;

    //args
    if (aArg1 == NULL || aArg2 == NULL) {
        return;
    }

    //start, processing the event
    selection = (CMsvEntrySelection*) aArg1;
    parent = *(TMsvId*) aArg2;

    // Return when not (outbox or inbox) and  event not EMsvEntriesChanged
    if (!(parent == KMsvGlobalOutBoxIndexEntryIdValue || parent == KMsvGlobalInBoxIndexEntryIdValue)
        && aEvent != EMsvEntriesChanged) {
        return;
    }

    //Handling for outbox entries
    if (parent == KMsvGlobalOutBoxIndexEntryIdValue) {
        CMsvEntry* rootEntry = iMsvSession->GetEntryL(KMsvGlobalOutBoxIndexEntryId);

        for (TInt i = 0; i < selection->Count(); ++i) {
            TMsvEntry entry = rootEntry->ChildDataL(selection->At(i));

            if ((entry.iMtm == KSenduiMtmSmsUid) || (entry.iMtm == KSenduiMtmMmsUid)) {
                TUint sendingState = entry.SendingState();
                TInt index = iFailedMessages->Find(entry.Id());

                if (sendingState == KMsvSendStateFailed && KErrNotFound == index) {

                    iFailedMessages->AppendL(entry.Id());
                    MsgInfo aInfo;
                    ProcessIndicatorDataL(entry.Id(), aInfo);
                    iNotifier->displayFailedNote(aInfo);
                }
                else if (sendingState != KMsvSendStateFailed && KErrNotFound != index) {
                    iFailedMessages->Delete(index);
                    iFailedMessages->Compress();
                }

            }
        }//end for
    }
    else {
        TMsvEntry entry;
        TMsvId service;
        TInt error = KErrNone;
        for (TInt i = 0; i < selection->Count(); ++i) {
            error = iMsvSession->GetEntry(selection->At(i), service, entry);

            if (error == KErrNone && entry.iMtm == KUidMsgMMSNotification && 
                MmsNotificationStatus(entry) == EMsgStatusFailed) {

                TInt index = iFailedMessages->Find(entry.Id());

                if (KErrNotFound == index) {
                    iFailedMessages->AppendL(entry.Id());
                    MsgInfo aInfo;
                    //Fill aInfo with appropriate data
                    aInfo.mMessageType = ECsMmsNotification;
                    ProcessIndicatorDataL(entry.Id(), aInfo);
                    iNotifier->displayFailedNote(aInfo);

                }// end of if
            }
            else if (error == KErrNone && entry.iMtm == KUidMsgMMSNotification
                && MmsNotificationStatus(entry) == EMsgStatusRetrieving) {
                
                TInt index = iFailedMessages->Find(entry.Id());
                if (KErrNotFound != index) {
                    iFailedMessages->Delete(index);
                    iFailedMessages->Compress();
                }// end of KErrNotFound != index if block

            } // end of 2nd if  
        } // for loop
    }

}

// ---------------------------------------------------------
// MsgStoreHandler::HandleEntryEventL()
// ---------------------------------------------------------

void MsgStoreHandler::HandleEntryEventL(TMsvEntryEvent aEvent, TAny* /*aArg1*/, TAny* /*aArg2*/,
    TAny* /*aArg3*/)
{
    switch (aEvent) {
    case EMsvChildrenChanged:
    case EMsvDeletedChildren:
    {
        UpdateOutboxIndications();
        break;
    }
    default:
        break;

    }
}

// ---------------------------------------------------------
// UpdateOutboxIndications()
// Outgoing Pending message/messages
// ---------------------------------------------------------
void MsgStoreHandler::UpdateOutboxIndications()
{
    MsgInfo failedIndicatorData;
    MsgInfo pendingIndicatorData;

    TInt err = KErrNone;

    TRAP(err, GetOutboxEntriesL(failedIndicatorData, pendingIndicatorData));

    if (err == KErrNone) {
        iNotifier->displayOutboxIndications(failedIndicatorData);
        iNotifier->displayOutboxIndications(pendingIndicatorData);
    }
}

// ---------------------------------------------------------
// GetOutboxEntries()
// Outgoing Pending message/messages
// ---------------------------------------------------------
TInt MsgStoreHandler::GetOutboxEntriesL(MsgInfo& aFailedIndicatorData,
    MsgInfo& aPendingIndicatorData)
{
    CMsvEntry* rootEntry = iMsvSession->GetEntryL(KMsvGlobalOutBoxIndexEntryId);
    CMsvEntrySelection* messages = rootEntry->ChildrenL();

    TInt failedMessageCount = 0;
    TInt pendingMessageCount = 0;

    for (TInt i = 0; i < messages->Count(); ++i) {
        TMsvEntry entry = rootEntry->ChildDataL(messages->At(i));
        if ((entry.iMtm != KSenduiMtmSmsUid) && (entry.iMtm != KSenduiMtmMmsUid)) {
            continue;
        }

        if (entry.SendingState() == KMsvSendStateFailed) {
            ++failedMessageCount;
        }
        else {
            ++pendingMessageCount;
        }

        if (entry.SendingState() == KMsvSendStateFailed) {
            ProcessIndicatorDataL(entry.Id(), aFailedIndicatorData);
        }
        else {
            ProcessIndicatorDataL(entry.Id(), aPendingIndicatorData);
        }
       
    }

    aFailedIndicatorData.mFromSingle = (failedMessageCount > 1) ? false : true;
    aPendingIndicatorData.mFromSingle = (pendingMessageCount > 1) ? false : true;
    aFailedIndicatorData.mIndicatorType = FailedIndicatorPlugin;
    aPendingIndicatorData.mIndicatorType = PendingIndicatorPlugin;
    aFailedIndicatorData.mMsgCount = failedMessageCount;
    aPendingIndicatorData.mMsgCount = pendingMessageCount;

    return KErrNone;
}

// ---------------------------------------------------------
// ProcessIndicatorData()
// Process the data in the MsgInfo object. 
// ---------------------------------------------------------

void MsgStoreHandler::ProcessIndicatorDataL(TMsvId msgId, MsgInfo& indicatorData)
{
    CCsClientConversation* conversation = iRequestHandler->GetConversationFromMessageIdL(msgId);
    if (conversation == NULL)
        return;

    indicatorData.mConversationId = conversation->GetConversationEntryId();
    CCsConversationEntry* convEntry = conversation->GetConversationEntry();

    //check for valid data
    if ((indicatorData.mConversationId == -1) || (convEntry == NULL)) {
        delete conversation;
        return;
    }
    
    //set indicator data
    HBufC* displayName = conversation->GetDisplayName();

    if (displayName) {
        indicatorData.mDisplayName.append(XQConversions::s60DescToQString(*displayName));
    }
    else {
        HBufC* number = convEntry->Contact();
        if (number)
            indicatorData.mDisplayName.append(XQConversions::s60DescToQString(*number));
    }
    delete conversation;
}

// ---------------------------------------------------------
// GetUnreadMessageCountL()
// Get Unread message count.
// ---------------------------------------------------------

int MsgStoreHandler::GetUnreadMessageCountL()
{
    //Create  the query/operation object
    CMsvSearchSortOperation *operation = CMsvSearchSortOperation::NewL(*iMsvSession);
    CleanupStack::PushL(operation);
    CMsvSearchSortQuery *query = CMsvSearchSortQuery::NewL();
    CleanupStack::PushL(query);

    //set the query options
    query->SetParentId(KMsvGlobalInBoxIndexEntryId);
    query->SetResultType(EMsvResultAsTMsvEntry);
    query->AddSearchOptionL(EMsvUnreadMessages, ETrue);
    CleanupStack::Pop(query);

    CMsvOperationWait* wait = CMsvOperationWait::NewLC();
    //ownership of Query transferred to Operation  
    operation->RequestL(query, EFalse, wait->iStatus);
    wait->Start();
    CActiveScheduler::Start();

    //Get No of entries
    int count = operation->GetResultCountL();
    CleanupStack::PopAndDestroy(2, operation);

    return count;
}

// ---------------------------------------------------------
// MsgStoreHandler::MmsNotificationStatus
// ---------------------------------------------------------
//
TCsMmsNotificationMsgState MsgStoreHandler::
MmsNotificationStatus( TMsvEntry entry )
    {   
    TCsMmsNotificationMsgState status = EMsgStatusNull;

    // operationMask includes operation type. It is not bitmap but ordinal number. 
    // It does not include operation status and result
    TInt operationMask = (entry.iMtmData2 & KMmsOperationIdentifier) ;

    // Note! Ongoing operation resets every bit of operation type, operation status
    // and result. E.g. If message has been forwarded and then fetching starts, 
    // information about forwarding is lost

    if( ( entry.iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageMNotificationInd )
        {
        if(     operationMask == KMmsOperationFetch 
                &&  OperationOngoing( entry ) )
            { 
            // It's in retrieving state
            status = EMsgStatusRetrieving;
            }
        else if(    operationMask == KMmsOperationForward
                &&  OperationOngoing( entry ) )
            { 
            // It's in forwarding state
            status = EMsgStatusForwarding;
            }
        else if(    operationMask == KMmsOperationForward
                &&  OperationFinished( entry )
                &&  !( entry.iMtmData2 & KMmsOperationResult ) )
            { 
            // It's been forwarded succesfully
            status = EMsgStatusForwarded;
            }
        else if(    operationMask == KMmsOperationFetch 
                &&  OperationFinished( entry )
                &&   (  entry.iMtmData2 & KMmsOperationResult 
                ||  entry.iError ) )
            { 
            // Fetch has been failed
            status = EMsgStatusFailed;
            }
        else if(    operationMask == KMmsOperationDelete
                &&  OperationFinished( entry )
                &&  !( entry.iMtmData2 & KMmsOperationResult ) )
            { 
            // It's been deleted succesfully
            status = EMsgStatusDeleted;
            }
        else 
            {   // Normal waiting state
            status = EMsgStatusReadyForFetching;
            }
        }

    return status;
    }

// ---------------------------------------------------------
// MsgStoreHandler::OperationOngoing
// ---------------------------------------------------------
//
TBool MsgStoreHandler::OperationOngoing( const TMsvEntry& aEntry ) const
    {
    return (    aEntry.iMtmData2 & KMmsOperationOngoing 
            &&  !( aEntry.iMtmData2 & KMmsOperationFinished ) );
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::OperationFinished
// ---------------------------------------------------------
//
TBool MsgStoreHandler::OperationFinished( 
    const TMsvEntry& aEntry ) const
    {
    return (    aEntry.iMtmData2 & KMmsOperationFinished
            &&  !( aEntry.iMtmData2 & KMmsOperationOngoing ) );
    }

// End of file
