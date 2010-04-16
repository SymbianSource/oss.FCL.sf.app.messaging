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
#include "conversationsengine_p.h"

//SYSTEM INCLUDES
#include <ccsconversationentry.h>
#include <ccsclientconversation.h>
//USER INCLUDES
#include "conversationsenginedefines.h"
#include "conversationmsgstorehandler.h"
#include "conversationssummarymodel.h"
#include "conversationsmodel.h"
#include "conversationchangehandler.h"
#include "conversationlistchangehandler.h"

//---------------------------------------------------------------
// ConversationsEnginePrivate::ConversationsEnginePrivate
// @see header
//---------------------------------------------------------------
ConversationsEnginePrivate::ConversationsEnginePrivate(
    ConversationMsgStoreHandler* conversationMsgStoreHandler,
    ConversationsSummaryModel *conversationsSummaryModel,
    ConversationsModel *conversationsModel
    ):mServer(NULL),
    mClientConv(NULL),
    mConversationMsgStoreHandler(conversationMsgStoreHandler),
    mConversationsSummaryModel(conversationsSummaryModel),
    mConversationsModel(conversationsModel)
{
        TRAP_IGNORE(initL());
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::~ConversationsEnginePrivate
// @see header
//---------------------------------------------------------------
ConversationsEnginePrivate::~ConversationsEnginePrivate()
{
    delete mConvChangeHandler;
    
    delete mConvListChangeHandler;
    
    if( mClientConv )
    {        
        if(mServer)
        {    
        mServer->RemoveConversationChangeEventL (mConvChangeHandler,
            mClientConv);
        }
        delete mClientConv;
        mClientConv = NULL;
    }
    if(mServer)
    {
        mServer->RemoveConversationListChangeEventL (mConvListChangeHandler);
        mServer->RemoveResultsEventL ( this );
        mServer->Cancel();
        delete mServer;
        mServer = NULL;
    }
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::initL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::initL()
{
    mServer = CCSRequestHandler::NewL();
    
    //Add results observer to the server
    mServer->RequestResultsEventL(this);
    
    //Fetch the conversation list from server
    mServer->GetConversationListL();    
    
    mConvChangeHandler = new ConversationsChangeHandler(this,
        mConversationsModel);
    
    mConvListChangeHandler = new ConversationsListChangeHandler(this,
        mConversationsSummaryModel);
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::registerForConversationListUpdatesL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::registerForConversationListUpdatesL()
{
    //Add ConversationListChange Observer
    mServer->RequestConversationListChangeEventL (mConvListChangeHandler);
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::getConversationsL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::getConversationsL( TInt aConversationId)
{	
    if(!mClientConv)
    {
        //Clear the model before issueing fetch
        mConversationsModel->clear();
        //create a client conversation
        mClientConv = CCsClientConversation::NewL();
        mClientConv->SetConversationEntryId(aConversationId);
        //set dummy entry
        CCsConversationEntry *entry = CCsConversationEntry::NewL();
		CleanupStack::PushL(entry);
        mClientConv->SetConversationEntryL(entry);
		CleanupStack::PopAndDestroy(entry);
        //Get the conversations for new conversationId 
        mServer->GetConversationsL( mClientConv ); 
    }   
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::deleteConversationL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::deleteConversationL(TInt aConversationId)
{
    mServer->DeleteConversationL(aConversationId);
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::deleteMessages
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::deleteMessages(RArray<TInt>& aIdArray)
{
    mConversationMsgStoreHandler->DeleteMessages(aIdArray);
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::markConversationReadL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::markConversationReadL(TInt aConversationId)
{
    mServer->MarkConversationReadL(aConversationId);
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::markMessagesReadL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::markMessagesReadL(RArray<TInt>& aIdArray)
{
    mConversationMsgStoreHandler->MarkMessagesReadL(aIdArray);
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::getConversationCurrentId
// @see header
//---------------------------------------------------------------
TInt ConversationsEnginePrivate::getConversationCurrentId()
{
    TInt convId = -1;
    if(mClientConv)
    {
        convId = mClientConv->GetConversationEntryId();    
    }
    return convId;
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::getConversationIdFromContactId
// @see header
//---------------------------------------------------------------
TInt ConversationsEnginePrivate::getConversationIdFromContactIdL(TInt contactId)
{
    TInt convId = -1;
    
    convId = mServer->GetConversationIdL(contactId);
    
    return convId;
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::getConversationIdFromAddress
// @see header
//---------------------------------------------------------------
TInt ConversationsEnginePrivate::getConversationIdFromAddressL(TDesC& contactAddress)
{
    TInt convId = -1;
    
    convId = mServer->GetConversationIdFromAddressL(contactAddress);
    
    return convId;
}


//---------------------------------------------------------------
// ConversationsEnginePrivate::clearConversationsL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::clearConversationsL()
{    
    mConvChangeHandler->Cancel();
    //Clear conversations model before populating with new data 
    mConversationsModel->clear();  
    
    // Delete old CCsClientConversation object 
    // Remove the old Conversation change observer
    if(mClientConv)
    {    
        mServer->RemoveConversationChangeEventL (mConvChangeHandler ,mClientConv);
        delete mClientConv;
        mClientConv = NULL;
    }
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::registerForConversationUpdatesL
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::registerForConversationUpdatesL()
{
    //Add the Conversation Change for new  conversationId
    if(mClientConv)
    {    
    mServer->RequestConversationChangeEventL (mConvChangeHandler ,mClientConv);
    }
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::ConversationList
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::ConversationList(
    RPointerArray<CCsClientConversation>& aClientConversationList)
{
    int error;
    TRAP(error,
        mConvListChangeHandler->ConversationListL(aClientConversationList));
}

//---------------------------------------------------------------
// ConversationsEnginePrivate::Conversations
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::Conversations(
    RPointerArray<CCsConversationEntry>& aConversationEntryList)
{
    int error;
    if(mClientConv)
    {    
        TRAP(error,mConvChangeHandler->ConversationsL(aConversationEntryList));
    }
}


//---------------------------------------------------------------
// ConversationsEngine::fetchMoreConversations
// @see header
//---------------------------------------------------------------
void ConversationsEnginePrivate::fetchMoreConversations()
{
    if(mClientConv)
        {    
            mConvChangeHandler->restartHandleConversations();
        }
}

//EOF

