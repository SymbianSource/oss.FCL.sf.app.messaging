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
 * Description:  ?Description
 *
 */

#include "conversationchangehandler.h"
// SYSTEM INCLUDES
#include <ccsconversationentry.h>

// USER INCLUDES
#include "conversationsengine.h"
#include "conversationsengine_p.h"
#include "conversationsmodel.h"

const TInt WindowSize = 7;

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
ConversationsChangeHandler::ConversationsChangeHandler(
    ConversationsEnginePrivate* convEnginePrivate,
    ConversationsModel* conversationsModel) :
    CActive(EPriorityStandard), 
    mCurrentState(EInit),
    mFirstWindowCached(EFalse),
    mCurrentIndex(-1), 
    mConversationsModel(conversationsModel), 
    mConvEnginePrivate(convEnginePrivate)

{
    CActiveScheduler::Add(this);
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
ConversationsChangeHandler::~ConversationsChangeHandler()
{
    mConversationEntryList.ResetAndDestroy();
    // Cancel the active object
    Cancel();
}

// ---------------------------------------------------------------------------
// This is for handling GetConversation results asynchronusly
// from the server.
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::ConversationsL(RPointerArray<
        CCsConversationEntry>& aConversationEntryList)
{
    mConversationEntryList.ResetAndDestroy();

    for (TInt i = 0; i < aConversationEntryList.Count(); ++i)
    {
        CCsConversationEntry *entry = aConversationEntryList[i]->CloneL();
        mConversationEntryList.AppendL(entry);
    }
    if (aConversationEntryList.Count() > 0)
    {
        mFirstWindowCached = EFalse;
        mCurrentIndex = 0;
        mCurrentState = EInitialCache;
        IssueRequest();
    }
    else
    {
        //This is done for safety
        mConvEnginePrivate->registerForConversationUpdatesL();
        mCurrentState = EListenToEvents;
    }
}

// ---------------------------------------------------------------------------
// Handling addition of new conversation entry from the server
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::AddConversation(
    const CCsConversationEntry& aConvEntry)
{
    mConversationsModel->addRow(aConvEntry, true);
    ConversationsEngine::instance()->emitConversationModelUpdated();
}

// ---------------------------------------------------------------------------
// Handling modification of existing conversation entry from the server
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::ModifyConversation(
    const CCsConversationEntry& aConvEntry)
{
    mConversationsModel->addRow(aConvEntry, true);
}

// ---------------------------------------------------------------------------
// Handling deletion of conversation entry from the server
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::DeleteConversation(
    const CCsConversationEntry& aConvEntry)
{
    mConversationsModel->deleteRow(aConvEntry.EntryId());
}

//-----------------------------------------------------------------------
// This is for handling modify conversation event asynchronusly from the server
//-----------------------------------------------------------------------
//
void ConversationsChangeHandler::RefreshConversation()
{
}

// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::RunL()
{
    if (iStatus != KErrNone)
    {
        return;
    }
    //process
    switch (mCurrentState)
    {
        case EInitialCache:
            HandleConversationsL();
            break;
    }
}

// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::DoCancel()
{
    mCurrentState = EListenToEvents;
}

// ---------------------------------------------------------------------------
// Make the active object alive.
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::IssueRequest()
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
//  Handles Conversations received from server and updates into model
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::HandleConversationsL()
{
    for (int i = 0; i < WindowSize; ++i)
    {
        if (mCurrentIndex < mConversationEntryList.Count())
        {
            mConversationsModel->addRow(
                * (mConversationEntryList[mCurrentIndex]));
            mCurrentIndex++;
        }
        else
        {
            break;
        }
    }
    if (mCurrentIndex < mConversationEntryList.Count())
    {
        if (!mFirstWindowCached)
        {
            ConversationsEngine::instance()->emitConversationModelPopulated();
            mFirstWindowCached = ETrue;
            return;
        }
        IssueRequest();
    }
    else
    {
        if (!mFirstWindowCached)
        {
            ConversationsEngine::instance()->emitConversationModelPopulated();
            mFirstWindowCached = ETrue;
        }
        mConversationEntryList.ResetAndDestroy();
        mConvEnginePrivate->registerForConversationUpdatesL();
        mCurrentState = EListenToEvents;
    }
}

// ---------------------------------------------------------------------------
//  Starts fetching remaining conversations
// ---------------------------------------------------------------------------
//
void ConversationsChangeHandler::restartHandleConversations()
{
    IssueRequest();
}

//EOF
