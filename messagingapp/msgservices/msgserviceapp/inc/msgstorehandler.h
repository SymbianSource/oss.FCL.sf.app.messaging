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
* Description: Reads message information from message store.
*
*/

#ifndef __MSG_STORE_HANDLER_H__
#define __MSG_STORE_HANDLER_H__

// INCLUDE FILES
#include <msvapi.h>

class MsgStoreHandler : public MMsvSessionObserver
    {
public:
    /**
     * Constructor
     */
    explicit MsgStoreHandler();
    
    /**
     * Destructor
     */
    virtual ~MsgStoreHandler();
    
public :
    /**
     * @see MMsvSessionObserver
     */
    void HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
            TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/);
    
public:
    /**
     * Mark message as read and get the message type
     * @param messageId  message Id 
     * @param msgType message type is written to this.
     * @param msgType message sub type is written to this.
     */
    void markAsReadAndGetType(int msgId,
                              int& msgType,
                              int& msgSubType);
 
    /**
     * Delete message.
     * @param msgId id of the message to be deleted.
     */
    void deleteMessage(int msgId);
    
    /**
     * is draft message.
     * @param msgId id of the message to be checked.
     * @return true if it is a draft message,else false
     */
    bool isDraftMessage(int msgId);
    
private:
    /**
     * Does all initilaizations
     */
    void InitL();
    
    /**
     * Extrcats the message type from TMsvEntry
     * @param aEntry message entry.
     * @param msgType message type is written to this.
     * @param msgType message sub type is written to this.
     */
    void extractMsgType(const TMsvEntry& aEntry,
                        int& msgType,
                        int& msgSubType);
    
private:
    /**
     * Session
     * Owned.
     */
    CMsvSession* iMsvSession;     
    };

#endif // __MSG_STORE_HANDLER_H__

// EOF
