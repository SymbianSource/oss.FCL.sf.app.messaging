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
* Description:  
*
*/

#include "conversationmsgstorehandler.h"
#include "draftsmodel.h"
// SYSTEM INCLUDES
#include <StringLoader.h>
#include <ccsdefs.h> 
#include <mmsconst.h>
#include <SendUiConsts.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Default constructor.
// ---------------------------------------------------------
//
ConversationMsgStoreHandler::ConversationMsgStoreHandler():
    iMsvSession(NULL),iDraftEntry(NULL),iDraftMessages(NULL),mDraftsModel(NULL)
    {
    TRAP_IGNORE(InitL());
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
//
ConversationMsgStoreHandler::~ConversationMsgStoreHandler()
    {
    if ( iDraftEntry )
        {
        delete iDraftEntry;
        iDraftEntry = NULL;
        }

    if( iMsvSession )
        {
        delete iMsvSession;
        iMsvSession = NULL;
        }

    if ( iDraftMessages )
        {
        iDraftMessages->Reset();
        delete iDraftMessages;
        iDraftMessages = NULL;
        }
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void ConversationMsgStoreHandler::InitL( )
    {
    iMsvSession = CMsvSession::OpenSyncL(*this);
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::HandleSessionEventL()
// ---------------------------------------------------------
//
void ConversationMsgStoreHandler::HandleSessionEventL(TMsvSessionEvent aEvent,
        TAny* aArg1, TAny* aArg2, TAny* aArg3)
    {   
    //args
    if(aArg1 == NULL || aArg2 == NULL)
        {   
        return;
        }

    //start, processing the event
    CMsvEntrySelection* selection=NULL;
    TMsvId target = 0;
    TMsvId source = 0;
    selection= (CMsvEntrySelection*)aArg1;
    target = *(TMsvId*)aArg2; // target (current) parent
    if(aArg3 != NULL)
        {
        source = *(TMsvId*)aArg3; // source parent
        }
    //process for draft messages only
    if ( target == KMsvDraftEntryIdValue)
        {
        switch( aEvent )
            {
            //case EMsvEntriesCreated:
            case EMsvEntriesChanged:
            case EMsvEntriesMoved:
                {
                TMsvEntry entry;
                TMsvId service;
                TInt error= KErrNone;

                for( TInt i=0 ; i < selection->Count() ; ++i )
                    {
                    error = iMsvSession->GetEntry(selection->At(i),service,entry);
                    if (KErrNone == error)
                        {
                        // process only visible entries
                        if (entry.Visible() && IsMtmSupported(entry.iMtm.iUid))
                            {
                                if(mDraftsModel && mDraftsModel->isReady())
                                {
                                    mDraftsModel->addRow(entry);
                                }
                            }
                        }
                    }
                }
                break;

            case EMsvEntriesDeleted:
                {
                for( TInt i=0 ; i < selection->Count() ; ++i )
                    {
                    TMsvId id = selection->At( i );
                    if(mDraftsModel && mDraftsModel->isReady())
                        {
                        mDraftsModel->deleteRow(id);
                        }
                    }
                }
                break;
            }//end switch 
        }
    else if(KMsvDraftEntryIdValue == source)
        {
        //if message is moved from drafts to other folder
        // it needs to be removed.
        if(aEvent == EMsvEntriesMoved)
            {
            for( TInt i=0 ; i < selection->Count() ; ++i )
                {
                TMsvId id = selection->At( i );
                if(mDraftsModel && mDraftsModel->isReady())
                    {
                    mDraftsModel->deleteRow(id);
                    }
                }
            }
        }
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::MsgStatus
// ---------------------------------------------------------
//
TCsMmsNotificationMsgState ConversationMsgStoreHandler::
MmsNotificationStatus( TInt aMsvId )
    {
    TMsvId dummy = 0x0;
    
    TMsvEntry entry;
    iMsvSession->GetEntry(aMsvId, dummy, entry);
    
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
// ConversationMsgStoreHandler::OperationOngoing
// ---------------------------------------------------------
//
TBool ConversationMsgStoreHandler::OperationOngoing( const TMsvEntry& aEntry ) const
    {
    return (    aEntry.iMtmData2 & KMmsOperationOngoing 
            &&  !( aEntry.iMtmData2 & KMmsOperationFinished ) );
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::OperationFinished
// ---------------------------------------------------------
//
TBool ConversationMsgStoreHandler::OperationFinished( 
    const TMsvEntry& aEntry ) const
    {
    return (    aEntry.iMtmData2 & KMmsOperationFinished
            &&  !( aEntry.iMtmData2 & KMmsOperationOngoing ) );
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::MarkMessagesReadL
// ---------------------------------------------------------
//
void ConversationMsgStoreHandler::MarkMessagesReadL(RArray<TInt>& aIdArray)
    {    
    for ( int index = 0; index < aIdArray.Count(); index++ )
        {
        TMsvId id = aIdArray[index];
        CMsvEntry* cEntry = NULL;
        TRAPD(err, cEntry = iMsvSession->GetEntryL(id));
        if ( err == KErrNotFound )
            continue; // Entry is already deleted.
        TMsvEntry entry = cEntry->Entry();
        if ( entry.Unread() ) 
            {
            // Mark the entry as read
            entry.SetUnread( EFalse );
            cEntry->ChangeL( entry );
            }
        delete cEntry;
        }
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::DeleteMessages
// ---------------------------------------------------------
//
void ConversationMsgStoreHandler::DeleteMessages(RArray<TInt>& aIdArray)
{
    for(int index=0;index < aIdArray.Count() ;++index)
    {
        TMsvId id = aIdArray[index];
        iMsvSession->RemoveEntry(id);
    }   
}

// ---------------------------------------------------------
// ConversationMsgStoreHandler::DeleteMessagesL
// ---------------------------------------------------------
//
CMsvSession& ConversationMsgStoreHandler::GetMsvSession()
{
    return *iMsvSession;
}

// ---------------------------------------------------------
// ConversationMsgStoreHandler::FetchDraftsMessages
// ---------------------------------------------------------
//
void ConversationMsgStoreHandler::FetchDraftMessages(DraftsModel* draftsModel)
    {
    mDraftsModel = draftsModel;
    iState = EReadDrafts;
    TCallBack callback = TCallBack(ProcessDraftMessages, (TAny*) this);
    iIdle = CIdle::NewL(CActive::EPriorityStandard);
    iIdle->Start(callback);
    }

// -----------------------------------------------------------------------------
// ConversationMsgStoreHandler::ProcessDraftMessages
// CIdle callback 
// -----------------------------------------------------------------------------
//
TInt ConversationMsgStoreHandler::ProcessDraftMessages(TAny* aArg)
    {
    ConversationMsgStoreHandler* handler = (ConversationMsgStoreHandler*) aArg; 
    TInt ok = 0;
    TRAPD(err, ok = handler->ProcessDraftMessagesL());
    return ((err == KErrNone) && ok);
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::ProcessDraftMessagesL
// ---------------------------------------------------------
//
TInt ConversationMsgStoreHandler::ProcessDraftMessagesL()
    {
    switch(iState)
        {
        case EReadDrafts:
            {
            iDraftEntry = iMsvSession->GetEntryL(KMsvDraftEntryIdValue);
           
            iDraftMessages = iDraftEntry->ChildrenL();    
            iDraftMessageCount = iDraftEntry->Count();

            if ( iDraftMessageCount ) 
                {
                iState = EProcessDrafts;
                }
            else
                {
                iState = EComplete; // no draft messages
                mDraftsModel->setReady(); //ready to handle dynamic events
                CleanupL();
                return 0; //DONE
                }
            return 1;
            }
        case EProcessDrafts:
            {
            if ( iDraftMessageCount ) 
                {
                iDraftMessageCount--;
                TMsvEntry entry = iDraftEntry->ChildDataL(
                        iDraftMessages->At(iDraftMessageCount));
                
                // process only visible entries
                if (entry.Visible() && IsMtmSupported(entry.iMtm.iUid))
                {
                //add message to model
                mDraftsModel->addRow(entry);
                }
                //continue to process other draft messages
                iState = EProcessDrafts;
                }
            else
                {
                iState = EComplete;
                mDraftsModel->setReady(); //ready to handle dynamic events
                CleanupL();
                return 0; // DONE
                }
            return 1; 
            }
        }

    return 0;
    }

// -----------------------------------------------------------------------------
// ConversationMsgStoreHandler::CleanupL()
// Helper function for state machine cleanup
// -----------------------------------------------------------------------------
//
void ConversationMsgStoreHandler::CleanupL()
    {
    if ( iDraftEntry )
        {
        delete iDraftEntry;
        iDraftEntry = NULL;
        }

    if ( iDraftMessages )
        {
        iDraftMessages->Reset();
        delete iDraftMessages;
        iDraftMessages = NULL;
        }

    iDraftMessageCount = 0;
    }

// -----------------------------------------------------------------------------
// ConversationMsgStoreHandler::IsMtmSupported()
// 
// -----------------------------------------------------------------------------
//
TBool ConversationMsgStoreHandler::IsMtmSupported(long uid)	
    {
    if ( KSenduiMtmSmsUidValue == uid || KSenduiMtmMmsUidValue == uid )
        {
        return ETrue;
        }	
    return EFalse;
    }
// End of file
