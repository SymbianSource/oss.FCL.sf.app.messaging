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
* Description:  Conversation delete observer interface.
 *
*/

#ifndef MCSCONVERSATIONDELETEOBSERVER_H_
#define MCSCONVERSATIONDELETEOBSERVER_H_

class CCsConversationDeleteHandler;

class MCsConversationDeleteObserver 
    {
public:
    /**
     * Callback that notifies delete is already in progress
     */
    virtual void DeleteInProgress(CCsConversationDeleteHandler* handler) = 0;
        
    /**
     * Callback that notifies delete is complete
     */
    virtual void DeleteComplete(CCsConversationDeleteHandler* handler) = 0;
    };

#endif /* MCSCONVERSATIONDELETEOBSERVER_H_ */
