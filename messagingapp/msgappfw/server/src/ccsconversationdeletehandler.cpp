/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CS Server Delete Handler
*
*/
// INCLUDES
#include <ccsclientconversation.h>
#include <ccsconversationentry.h>
#include "ccsconversationcache.h"
#include "ccsconversationdeletehandler.h"

// ----------------------------------------------------------------------------
// CCsConversationDeleteHandler::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CCsConversationDeleteHandler* CCsConversationDeleteHandler::
NewL(CCsConversationCache* aCache)
    {
    CCsConversationDeleteHandler* self = 
            new (ELeave) CCsConversationDeleteHandler();
    CleanupStack::PushL(self);
    self->ConstructL(aCache);
    CleanupStack::Pop(self); // self
    return self;
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
CCsConversationDeleteHandler::CCsConversationDeleteHandler():
        CActive(CActive::EPriorityLow)
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
void CCsConversationDeleteHandler::ConstructL(CCsConversationCache* aCache)
    {
    iCache = aCache;
    iState = EIdle;
     
    iConversationEntryList = new (ELeave)RPointerArray<CCsConversationEntry> ();  
    iSession = CMsvSession::OpenSyncL(*this);
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
CCsConversationDeleteHandler::~CCsConversationDeleteHandler()
{
    if (iConversationEntryList)
    {
        iConversationEntryList->ResetAndDestroy();
        iConversationEntryList->Close();
        delete iConversationEntryList;
        iConversationEntryList = NULL;
    }

    if (iSession)
    {
        delete iSession;
        iSession = NULL;
        }
    }

// ----------------------------------------------------------------------------
// Delete set of messages
// ----------------------------------------------------------------------------
void CCsConversationDeleteHandler::DeleteL(TInt aConversationId)
    {
    // Check if delete in progress
    if ( iCache->IsDeleted(aConversationId) )
        {
        // Deletion is in progress for this conversation, so clean up this AO
        delete this;
        return;
        }
    
    // Store conversationId currently being deleted.
    iConversationId = aConversationId;

    // Temp client conversation object
    CCsClientConversation* clientConversation = CCsClientConversation::NewL();
    clientConversation->SetConversationEntryId(iConversationId);
    CleanupStack::PushL(clientConversation);

    // Get conversationlist for given client conversation
    iCache->GetConversationsL (clientConversation, iConversationEntryList);
    iCache->MarkConversationAsDeleted(iConversationId, ETrue);
    
    iDeletedCount = 0;
    
    // Cleanup  
    CleanupStack::PopAndDestroy(clientConversation);
        
    iState = EDeleteStart;
    IssueRequest();
    }

// ----------------------------------------------------------------------------
// Delete a message
// ----------------------------------------------------------------------------
void CCsConversationDeleteHandler::DeleteOneMessage()
    {
    CCsConversationEntry* entry = 
            static_cast<CCsConversationEntry*>((*iConversationEntryList)[iDeletedCount]);
            
    TInt id = entry->EntryId();
    
    if ( ECsSendStateSending != entry->GetSendState() )
        {
        iSession->RemoveEntry(id);
        }
    }

// ----------------------------------------------------------------------------
// RunL
// ----------------------------------------------------------------------------
void CCsConversationDeleteHandler::RunL()
    {
    switch ( iState )
        {
        case EDeleteStart:
            if ( iConversationEntryList->Count() > 0 )
                {
                iDeletedCount = iConversationEntryList->Count();
                iState = EDeleteNext;
                IssueRequest();
                }
            else
                {
                iState = EDeleteComplete;
                IssueRequest();
                }
            break;
        
        case EDeleteNext:
            iDeletedCount--;
            DeleteOneMessage();
            if ( iDeletedCount > 0 )
                {
                iState = EDeleteNext;
                IssueRequest();
                }
            else
                {
                iState = EDeleteComplete;
                IssueRequest();
                }
            break;
            
        case EDeleteComplete:
            // Mark delete complete.
            iCache->MarkConversationAsDeleted(iConversationId, EFalse);
            // Done with the processing , cleanup the AO since this is the last 
            //call to the delete handler.
            delete this;
            break;
        }
    }

// ----------------------------------------------------------------------------
// DoCancel
// ----------------------------------------------------------------------------
void CCsConversationDeleteHandler::DoCancel()
    {
    // Not supported
    }

// ---------------------------------------------------------------------------
// Move to next state
// ---------------------------------------------------------------------------
//
void CCsConversationDeleteHandler::IssueRequest()
    {
    if( !IsActive() )
        {
        iStatus = KRequestPending;
        TRequestStatus* status = &iStatus;
        SetActive();
        User::RequestComplete(status, KErrNone);
        }
    }

// EOF
