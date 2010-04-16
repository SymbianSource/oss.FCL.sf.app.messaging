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
 * Description:  CS Conversation Cache class. 
 *                This cache all the conversation from plugins 
 *                and responds to the client request
 *
 */

// INCLUDE FILES

// SYSTEM INCLUDE FILES
#include <ccsclientconversation.h>
#include <ccsconversationentry.h>
#include <ccsdefs.h>
#include <telconfigcrkeys.h>        // KCRUidTelephonyConfiguration
#include <centralrepository.h>

// USER INCLUDE FILES
#include "ccsconversationcache.h"
#include "ccsconversationcachehelper.h"
#include "ccsconversationcontact.h"
#include "ccsconversation.h"
#include "ccscontactsmanager.h"
#include "ccscontactsresolver.h"
#include "ccsconversationevent.h"
#include "ccsserver.h"
#include "s60qconversions.h"
#include "ccsdebug.h"

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CCsConversationCache::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CCsConversationCache*
CCsConversationCache::NewL(CCsContactsManager* aContactManager,
                           CCsServer* aCsServer)
{
    PRINT ( _L("Enter CCsConversationCache::NewL") );

    CCsConversationCache* self =
            new (ELeave) CCsConversationCache(aContactManager, aCsServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    PRINT ( _L("End CCsConversationCache::NewL") );

    return self;
}

// ----------------------------------------------------------------------------
// CCsConversationCache::CCsConversationCache
// Construtor
// ----------------------------------------------------------------------------
CCsConversationCache::CCsConversationCache(CCsContactsManager* aContactManager,
                                           CCsServer* aCsServer) :
    iCsServer(aCsServer), iContactsManager(aContactManager)
{
}

// ----------------------------------------------------------------------------
// CCsConversationCache::ConstructL
// Second phase constructor
// ----------------------------------------------------------------------------
void CCsConversationCache::ConstructL()
{
    PRINT ( _L("Enter CCsConversationCache::ConstructL") );

    // Register for events from contact manager
    iContactsManager->addObserver(this);

    // initialize the main cache entries
    iConversationList = new (ELeave) RPointerArray<CCsConversation> ();

    // initialize the class helper class
    iConversationCacheHelper = CCsConversationCacheHelper::NewL(this);

    iMatchDigitCount = KDefaultGsmNumberMatchLength;

    // Read the amount of digits to be used in contact matching
    // The key is owned by PhoneApp
    CRepository* repository = CRepository::NewLC(KCRUidTelConfiguration);
    if (repository->Get(KTelMatchDigits, iMatchDigitCount) == KErrNone)
    {
        // Min is 7
        iMatchDigitCount = Max(iMatchDigitCount, KDefaultGsmNumberMatchLength);
    }
    CleanupStack::PopAndDestroy(); // repository

    PRINT ( _L("End CCsConversationCache::ConstructL") );
}

// ----------------------------------------------------------------------------
// CCsConversationCache::CCsConversationCache
// Destructor
// ----------------------------------------------------------------------------
CCsConversationCache::~CCsConversationCache()
{
    PRINT ( _L("Enter CCsConversationCache::~CCsConversationCache") );

    if (iContactsManager)
    {
        iContactsManager->removeObserver(this);
    }

    // delete the set of all conversations
    if (iConversationList)
    {
        iConversationList->ResetAndDestroy();
        iConversationList->Close();
        delete iConversationList;
        iConversationList = NULL;
    }

    if (iConversationCacheHelper)
    {
        delete iConversationCacheHelper;
        iConversationCacheHelper = NULL;
    }

    PRINT ( _L("End CCsConversationCache::~CCsConversationCache") );
}

// ----------------------------------------------------------------------------
// CCsConversationCache::GetConversationListL
// Get Conversation list with contact details
// for all stored conversations
// This API can be used to prepare conversation list
// ----------------------------------------------------------------------------
void CCsConversationCache::GetConversationListL(RPointerArray<
        CCsClientConversation>* aClientConversationList)
{
    TInt totalConversation = iConversationList->Count();

    // Write the contact display names
    for (TInt loop = 0; loop < totalConversation; loop++)
    {
        CCsConversation* conversation =
                static_cast<CCsConversation*> ( (*iConversationList)[loop]);

        // The check is needed to avoid crash while internalizing resulting
        // from the row added for unknown drafts
        if (conversation->GetEntryCount())
        {
            CCsClientConversation* clientConversation =
                    CreateClientConvLC(conversation,
                                       conversation->GetLatestEntryL());
            aClientConversationList->Append(clientConversation);
            CleanupStack::Pop();
        }
    }

    PRINT1 ( _L("CCsConversationCache::GetConversationEntryListL - conversationCount:%d"),
            totalConversation );
}

// ----------------------------------------------------------------------------
// CCsConversationCache::GetConversationUnreadListL
// Get Conversation list with contact details and unread conversation
// for all stored conversations
// This API can be used to prepare conversation list
// ----------------------------------------------------------------------------
void CCsConversationCache::GetConversationUnreadListL(RPointerArray<
        CCsClientConversation>* aClientConversationList)
    {
    TInt conversationCount = iConversationList->Count();

    // Write the contact display names
    for (TInt loop = 0; loop < conversationCount; loop++)
        {
        CCsConversation* conversation =
                static_cast<CCsConversation*> ((*iConversationList)[loop]);

        // The check is needed to avoid crash while internalizing resulting
        // from the row added for unknown drafts
        if (conversation->GetEntryCount() && conversation->GetUnreadMessageCount())
            {
            CCsClientConversation* clientConversation = CreateClientConvLC(
                    conversation, conversation->GetLatestUnreadEntryL());
            aClientConversationList->Append(clientConversation);
            CleanupStack::Pop();
            }
        }

    PRINT1 ( _L("CCsConversationCache::GetConversationUnreadListL - conversationCount:%d"),
            conversationCount );
    }

// ----------------------------------------------------------------------------
// CCsConversationCache::GetConversationsL
// Get All Conversations for a given conversation Id
// the return list is set inside aConversationEntryList
// ----------------------------------------------------------------------------
void CCsConversationCache::GetConversationsL(
                                             const CCsClientConversation* aClientConversation,
                                             RPointerArray<CCsConversationEntry>* aConversationEntryList)
{
    TInt totalConversation = iConversationList->Count();

    // Get the entry id from Client Conversation for which conversations are required
    TCsConversationEntryID conversationEntryID =
            aClientConversation->GetConversationEntryId();

    // Write the contact display names
    for (TInt loop = 0; loop < totalConversation; loop++)
    {
        //match entry ID;
        CCsConversation* conversation =
                static_cast<CCsConversation*> ( (*iConversationList)[loop]);

        if (conversationEntryID == conversation->GetConversationId())
        {
            // search the conversation id and get the list inside
            conversation->GetEntryListL(aConversationEntryList);
            break;
        }
    }
    PRINT1 ( _L("CCsConversationCache::GetConversationsL - conversationCount:%d"),
            totalConversation );
}

// ----------------------------------------------------------------------------
// CCsConversationCache::HandleConversations
// Called from server. This is to handle the event callback from plugin dll
// The idea is to add entries to the cache and at the same time update the clients
// ----------------------------------------------------------------------------
void CCsConversationCache::HandleConversations(const RPointerArray<
        CCsConversationEntry>& aConversationEntryLists,
                                               const TUint32 aConversationEvent)
{
    // get count
    TInt count = aConversationEntryLists.Count();

    // go through each conversation entry
    // add it into queue, event list
    for (TInt loop = 0; loop < count; loop++)
    {
        //get copy of conversation event
        TInt error;
        CCsConversationEvent* conEvent = NULL;
        CCsClientConversation* clientCon = NULL;
        CCsConversationEntry* conEntry = aConversationEntryLists[loop];

        if (aConversationEntryLists[loop])
        {
            TRAP(
                    error,
                    conEvent = CCsConversationEvent::NewL();
                    CleanupStack::PushL(conEvent);
                    //create clientconversation
                    clientCon = CCsClientConversation::NewL();
                    CleanupStack::PushL(clientCon);
                    clientCon->SetConversationEntryL(conEntry);
                    //set conversation entry
                    conEvent->SetClientConversationL(*clientCon);
                    //set event
                    conEvent->SetEvent(aConversationEvent);
                    //add entry
                    iConversationCacheHelper->ConversationEventList()->AppendL(conEvent);
                    CleanupStack::PopAndDestroy(clientCon);
                    CleanupStack::Pop(conEvent));

            if (error != KErrNone)
            {
                PRINT1 ( _L("CCsConversationCache::HandleConversation - Error:%d"),
                        error );
                continue;
            }
        }
    }
    iConversationCacheHelper->StartCacheUpdate();
}

// ----------------------------------------------------------------------------
// CCsConversationCache::GetTotalUnreadCount
// Pending event count
// ----------------------------------------------------------------------------
TUint32 CCsConversationCache::GetTotalUnreadCount()
    {
    TUint32 totalUnread = 0;

    for (TInt loop = 0; loop < iConversationList->Count(); ++loop)
        {
        CCsConversation* conversation =
                static_cast<CCsConversation*> ((*iConversationList)[loop]);
        totalUnread += conversation->GetUnreadMessageCount();
        }
    return totalUnread;
    }

// ----------------------------------------------------------------------------
// CCsConversationCache::NotifyL
// Send the notification client
// ----------------------------------------------------------------------------
void CCsConversationCache::NotifyL(CCsClientConversation* aConversation,
                                   TUint32 aEvent)
{
    iCsServer->NotifySessions(aConversation, aEvent);
}

// ----------------------------------------------------------------------------
// CCsConversationCache::CachingCompletedL
// This is called from cache helper after it resolves all events from plugin
// ----------------------------------------------------------------------------
void CCsConversationCache::CachingCompletedL()
{
    // Not supported
}

// ----------------------------------------------------------------------------
// CCsConversationCache::FindConversation
// Find a conversation based on contact Id
// ----------------------------------------------------------------------------
TInt CCsConversationCache::FindConversation(TInt32 aContactId)
{
    if (aContactId == KErrNotFound)
        return KErrNotFound;

    TInt totalConversations = iConversationList->Count();

    for (TInt index = 0; index < totalConversations; index++)
    {
        CCsConversation* conversation =
                static_cast<CCsConversation*> ( (*iConversationList)[index]);

        CCsConversationContact* inCache = conversation->GetContact();

        TInt32 idInCache = inCache->GetContactId();

        if (idInCache == KErrNotFound)
            continue;

        if (idInCache == aContactId)
        {
            return index;
        }
    }
    return KErrNotFound;
}

// ----------------------------------------------------------------------------
// CCsConversationCache::FindConversation
// Find a conversation based on phone number
// ----------------------------------------------------------------------------
TInt CCsConversationCache::FindConversation(TDesC& aPhoneNumber)
{
    TInt count = iConversationList->Count();

    for (TInt cIndex = 0; cIndex < count; cIndex++)
    {
        CCsConversation* conversation =
                static_cast<CCsConversation*> ( (*iConversationList)[cIndex]);

        CCsConversationContact* inCache = conversation->GetContact();

        if (inCache->MatchPhoneNumber(aPhoneNumber, iMatchDigitCount))
        {
            return cIndex;
        }
    }
    return KErrNotFound;
}

// ----------------------------------------------------------------------------
// CCsConversationCache::RedoResolvingL
// Redo contact resolving for the conversation at this index
// ----------------------------------------------------------------------------
void CCsConversationCache::RedoResolvingL(TInt aIndex)
{
    // Recover the associated conversation object
    CCsConversation* conversation =
            static_cast<CCsConversation*> ( (*iConversationList)[aIndex]);

    // Get the conversation entries
    RPointerArray<CCsConversationEntry> entryList;
    conversation->GetEntryListL(&entryList);

    // Handle as if they are new conversations
    HandleConversations(entryList, KConversationEventNew);

    // Send a notification for delete conversation
    CCsConversationEntry* convEntry = conversation->GetLatestEntryL();

    if (iConversationCacheHelper->IsNotifyRequiredL(conversation,
                                                    convEntry,
                                                    KConversationListEventDelete,
                                                    conversation->GetUnreadMessageCount()))
    {
        //clone conversation and send update to client
        CCsClientConversation* clientConv = CreateClientConvLC(conversation,
                                                               convEntry);
        NotifyL(clientConv, KConversationListEventDelete);
        CleanupStack::PopAndDestroy(clientConv);
    }

    iConversationList->Remove(aIndex);
    entryList.ResetAndDestroy();

    delete conversation;
}

// ----------------------------------------------------------------------------
// CCsConversationCache::CreateClientConvLC
// create CCsConversation from CCsConversation and CCsConversationEntry
// ----------------------------------------------------------------------------
CCsClientConversation* CCsConversationCache::CreateClientConvLC(
                                                                const CCsConversation* aConversation,
                                                                const CCsConversationEntry* aConversationEntry)
{
    //create clientconversation
    CCsClientConversation* clientConversation = CCsClientConversation::NewL();
    CleanupStack::PushL(clientConversation);

    clientConversation->SetFirstNameL(aConversation->GetFirstName());
    clientConversation->SetLastNameL(aConversation->GetLastName());
    clientConversation->SetNickNameL(aConversation->GetNickName());
    clientConversation->SetConversationEntryId(aConversation->GetConversationId());
    clientConversation->SetConversationEntryL(aConversationEntry);
    clientConversation->SetContactId(aConversation->GetContactId());
    clientConversation->SetUnreadMessageCount(aConversation->GetUnreadMessageCount());

    return clientConversation;
}

// ----------------------------------------------------------------------------
// CCsConversationCache::MarkConversationAsDeleted
// ----------------------------------------------------------------------------
void CCsConversationCache::MarkConversationAsDeleted(TInt aConversationId,
                                                     TBool aDeleted)
{
    TInt conversationCount = iConversationList->Count();

    for (TInt loop = 0; loop < conversationCount; loop++)
    {
        CCsConversation* conversation =
                static_cast<CCsConversation*> ( (*iConversationList)[loop]);
        TInt id = conversation->GetConversationId();

        if (id == aConversationId)
        {
            conversation->MarkDeleted(aDeleted);
            break;
        }
    }
}

// ----------------------------------------------------------------------------
// CCsConversationCache::IsDeleted
// ----------------------------------------------------------------------------
TBool CCsConversationCache::IsDeleted(TInt aConversationId)
{
    TInt conversationCount = iConversationList->Count();

    for (TInt loop = 0; loop < conversationCount; loop++)
    {
        CCsConversation* conversation =
                static_cast<CCsConversation*> ( (*iConversationList)[loop]);
        TInt id = conversation->GetConversationId();

        if (id == aConversationId)
        {
            return conversation->IsDeleted();
        }
    }
    return EFalse;
}

// ----------------------------------------------------------------------------
// CCsConversationCache::ConversationList
// Pls refer to .h file
// ----------------------------------------------------------------------------
RPointerArray<CCsConversation>*
CCsConversationCache::ConversationList()
{
    return iConversationList;
}

// ----------------------------------------------------------------------------
// CCsConversationCache::getContactsManager
// Pls refer to .h file
// ----------------------------------------------------------------------------
CCsContactsManager*
CCsConversationCache::ContactsManager() const
{
    return iContactsManager;
}
// ----------------------------------------------------------------------------
// CCsConversationCache::HandleAddContact
// Pls refer to .h file
// ----------------------------------------------------------------------------
void CCsConversationCache::HandleAddContact(CCsContactDetail& aDetail)
{
    //handle addition of new contact

    QStringList phoneNumberList = aDetail.addressList;
    TInt totalPhoneNumber = phoneNumberList.count();

    // Get the conversation indexes for the phone numbers
    RArray<TInt> conversationIndex;

    for (int i = 0; i < totalPhoneNumber; i++)
    {
        QString phoneNumber = phoneNumberList.at(i);
        HBufC* phoneNumber_s60 = 
            S60QConversions::qStringToS60Desc(phoneNumber);
        TInt cIndex = FindConversation(*phoneNumber_s60);
        if (cIndex != KErrNotFound)
        {
            if (conversationIndex.Find(cIndex) == KErrNotFound)
            {
                conversationIndex.Append(cIndex);
            }
        }
        delete phoneNumber_s60;
    }

    // Sort the conversationIndex
    conversationIndex.Sort();
        
    // Redo contact resolving for the affected conversations
    for ( TInt i = conversationIndex.Count() - 1; i >= 0 ; i-- )
    {
        RedoResolvingL(conversationIndex[i]);
    }
    
    conversationIndex.Reset();
}

// ----------------------------------------------------------------------------
// CCsConversationCache::HandleContactChange
// Pls refer to .h file
// ----------------------------------------------------------------------------
void CCsConversationCache::HandleContactChange(CCsContactDetail& aDetail)
{
    //handle contact change 
    QStringList phoneNumberList = aDetail.addressList;
    int countNumberList = phoneNumberList.count();
    
    // Get the conversation indexes for the phone numbers
    RArray<TInt> conversationIndex;
    for (int i=0; i < countNumberList ; i++)
    {
        QString phoneNumber = phoneNumberList.at(i);
        HBufC* phoneNumber_s60 = 
                    S60QConversions::qStringToS60Desc(phoneNumber);
                
        TInt cIndex = FindConversation(*phoneNumber_s60);
        if ( cIndex != KErrNotFound )
        {
           conversationIndex.InsertInOrder(cIndex);
        }
        delete phoneNumber_s60;
    }
    
    // Get the conversation index corresponding to the contact Id
    TInt linkIdIndex = FindConversation(aDetail.contactId);
    if (linkIdIndex != KErrNotFound)
    {
        conversationIndex.InsertInOrder(linkIdIndex);
    }
    
    // Redo contact resolving for the affected conversations
    for ( TInt i = conversationIndex.Count() - 1; i >= 0 ; i-- )
    {
        RedoResolvingL(conversationIndex[i]);
    }

    conversationIndex.Reset();        
}

// ----------------------------------------------------------------------------
// CCsConversationCache::HandleDeleteContact
// Pls refer to .h file
// ----------------------------------------------------------------------------
void CCsConversationCache::HandleDeleteContact(CCsContactDetail& aDetail)
{
    //handle if contact is deleted
    TInt cIndex = FindConversation(aDetail.contactId);
    if (cIndex == KErrNotFound)
    {
        // No need to handle this change
        return;
    }

    // Redo resolving for the affected conversation
    RedoResolvingL(cIndex);
}

// ----------------------------------------------------------------------------
// CCsConversationCache::GetConversationIdL
// ----------------------------------------------------------------------------
TInt CCsConversationCache::GetConversationIdL(TInt aContactId)
    {
    TInt conversationCount = iConversationList->Count();

    for ( TInt loop= 0; loop < conversationCount; loop++ )
       {
       CCsConversation* conversation =
                static_cast<CCsConversation*>((*iConversationList)[loop]);
       TInt contactId = conversation->GetContactId(); 
       
       if ( contactId == aContactId )
           {
           return conversation->GetConversationId();
           }
       }
    
    return -1;
    }

// ----------------------------------------------------------------------------
// CCsConversationCache::GetConversationIdFromAddressL
// ----------------------------------------------------------------------------
TInt CCsConversationCache::GetConversationIdFromAddressL(TDesC& aContactAddress)
    {
    TInt conversationCount = iConversationList->Count();

    for ( TInt loop= 0; loop < conversationCount; loop++ )
    {
        CCsConversation* conversation =
            static_cast<CCsConversation*>((*iConversationList)[loop]);
            CCsConversationContact* contact = conversation->GetContact();
            if(contact->MatchPhoneNumber(aContactAddress,iMatchDigitCount))
            {
                return conversation->GetConversationId();
            }
    }
    return -1;
    }
//end of file
