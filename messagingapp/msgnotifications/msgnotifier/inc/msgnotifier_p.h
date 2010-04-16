/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Listen to conversation server events and parse the 
 * events. These parsed information passed to notifications and indications.
 *
 */

#ifndef MSGNOTIFIER_PRIVATE_H
#define MSGNOTIFIER_PRIVATE_H

#include <mcsconversationlistchangeobserver.h>

// CLASS DECLARATION
class MsgNotifier;
class CCSRequestHandler;

class MsgNotifierPrivate : public MCsConversationListChangeObserver
    {

public:
    /**
     * Constructor
     */
    MsgNotifierPrivate(MsgNotifier* msgNotifier);

    /**
     * Destructor
     */
    ~MsgNotifierPrivate();

public:
    /**  
     * AddConversationList
     * This is for handling new conversation event
     * asynchronously from the server 
     */
    void AddConversationList(
            const CCsClientConversation& aClientConversation);

    /**  
     * DeleteConversationList
     * This is for handling delete conversation event
     * asynchronously from the server 
     */
    void DeleteConversationList(
            const CCsClientConversation& aClientConversation) ;

    /**  
     * ModifyConversationList
     * This is for handling modify conversation asynchronously
     * from the server 
     */
    void ModifyConversationList(
            const CCsClientConversation& aClientConversation);

    /**
     * RefreshConversationList
     * This is for handling bulk changes. Message store change etc.
     */
    void RefreshConversationList();

private:
    /**
     * Does all the initializations. 
     */
    void initL();

    /**
     * Process conversation entry for showing the notification.
     * @param aClientConversation conversation entry
     */
    void processListEntry(const CCsClientConversation& aClientConversation);

    /**
     * updateIndications
     * Activate/Deactivate message indications based on unread message count
     */
    void updateIndications();

private:

    /**
     * Pointer to msgnotifier
     */
    MsgNotifier* q_ptr;  

    /**
     * Conversation server client 
     */
    CCSRequestHandler* mCvServer;

    };

#endif // MSGNOTIFIER_PRIVATE_H

//EOF
