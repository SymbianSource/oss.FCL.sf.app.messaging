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
 * Description:  This Class handles Add, Delete & Modify conversation list client events.
 *
 */

#include "conversationlistchangehandler.h"

// SYSTEM INCLUDES
#include <ccsconversationentry.h>
#include <ccsclientconversation.h>
// USER INCLUDE
#include "conversationsengine_p.h"
#include "conversationsengine.h"
#include "conversationssummarymodel.h"

TInt ListWindowSize = 7;
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
ConversationsListChangeHandler::ConversationsListChangeHandler(
    ConversationsEnginePrivate* convEnginePrivate,
    ConversationsSummaryModel* conversationsSummaryModel) :
    CActive(EPriorityStandard), 
    mCurrentState(EInit),
    mFirstWindowCached(EFalse),
    mCurrentIndex(-1),     
    mConvSummaryModel(conversationsSummaryModel), 
    mConvEnginePrivate(convEnginePrivate)
{
    CActiveScheduler::Add(this);
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
ConversationsListChangeHandler::~ConversationsListChangeHandler()
{
    mClientConversationList.ResetAndDestroy();
    // Cancel the active object
    Cancel();
}

//-----------------------------------------------------------------------
// From CActive class.
//-----------------------------------------------------------------------
//
void ConversationsListChangeHandler::RunL()
{
    if (iStatus != KErrNone)
    {
        // Reading events has failed
        return;
    }
    else
    {
        switch (mCurrentState)
        {
            case EInitialCache:
            {
                HandleConversationListL();
                break;
            }
        }
    }
}

//-----------------------------------------------------------------------
// From CActive class.
//-----------------------------------------------------------------------
//
void ConversationsListChangeHandler::DoCancel()
{
    mCurrentState = EListenToEvents;
}

//-----------------------------------------------------------------------
// From CActive class.
//-----------------------------------------------------------------------
//
void ConversationsListChangeHandler::ConversationListL(RPointerArray<
        CCsClientConversation>& aClientConversationList)
{
    mClientConversationList.ResetAndDestroy();
    
    for (TInt i = 0; i < aClientConversationList.Count(); ++i)
    {        
        CCsClientConversation *entry = aClientConversationList[i]->CloneL();
        mClientConversationList.AppendL(entry);
    }

    if (aClientConversationList.Count() > 0)
    {
        mCurrentIndex = 0;
        mFirstWindowCached = EFalse;
        mCurrentState = EInitialCache;
        IssueRequest();
    }
    else
    {
        mConvEnginePrivate->registerForConversationListUpdatesL();
        mCurrentState = EListenToEvents;
    }
}

//-----------------------------------------------------------------------
// This is for handling new conversation event asynchronusly from the server
//-----------------------------------------------------------------------
//
void ConversationsListChangeHandler::AddConversationList(
    const CCsClientConversation& aClientConversation)
{
    mConvSummaryModel->addRow(aClientConversation);
}

//-----------------------------------------------------------------------
// This is for handling delete conversation event asynchronusly from the server
//-----------------------------------------------------------------------
//
void ConversationsListChangeHandler::DeleteConversationList(
    const CCsClientConversation& aClientConversation)
{
    mConvSummaryModel->deleteRow(aClientConversation.GetConversationEntryId());
}

//-----------------------------------------------------------------------
// This is for handling modify conversation event asynchronusly from the server
//-----------------------------------------------------------------------
//
void ConversationsListChangeHandler::ModifyConversationList(
    const CCsClientConversation& aClientConversation)
{
    mConvSummaryModel->addRow(aClientConversation);
}

//-----------------------------------------------------------------------
// This is for handling refresh conversation event asynchronusly from the server
//-----------------------------------------------------------------------
//
void ConversationsListChangeHandler::RefreshConversationList()
{
}

// ---------------------------------------------------------------------------
// Make the active object alive.
// ---------------------------------------------------------------------------
//
void ConversationsListChangeHandler::IssueRequest()
{
    if (!IsActive())
    {
        iStatus = KRequestPending;
        TRequestStatus* status = &iStatus;
        SetActive();
        User::RequestComplete(status, KErrNone);
    }
}

// ---------------------------------------------------------------------------
// Handles Conversation List received from server and updates into model
// ---------------------------------------------------------------------------
//
void ConversationsListChangeHandler::HandleConversationListL()
{
    for (int i = 0; i < ListWindowSize; ++i)
    {
        if (mCurrentIndex < mClientConversationList.Count())
        {
            mConvSummaryModel->addRow(
                * (mClientConversationList[mCurrentIndex]));
            mCurrentIndex++;
        }
        else
        {
            break;
        }
    }
    if (mCurrentIndex < mClientConversationList.Count())
    {
        if (!mFirstWindowCached)
        {
            ConversationsEngine::instance()->
                                           emitConversationListModelPopulated();
            mFirstWindowCached = ETrue;
        }
        IssueRequest();
    }
    else
    {
        if (!mFirstWindowCached)
        {
            ConversationsEngine::instance()->
                                           emitConversationListModelPopulated();
            mFirstWindowCached = ETrue;
        }
        mClientConversationList.ResetAndDestroy();
        mConvEnginePrivate->registerForConversationListUpdatesL();
        mCurrentState = EListenToEvents;
    }

}

// EOF
