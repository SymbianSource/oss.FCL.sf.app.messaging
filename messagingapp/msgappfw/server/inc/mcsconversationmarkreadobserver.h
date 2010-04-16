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
* Description:  Conversation mark read observer interface.
 *
*/

#ifndef MCSCONVERSATIONREADOBSERVER_H_
#define MCSCONVERSATIONREADOBSERVER_H_

class CCsConversationMarkReadHandler;

class MCsConversationMarkReadObserver 
    {
public:
    /**
     * Callback that notifies mark read is complete
     */
    virtual void MarkReadComplete(CCsConversationMarkReadHandler* handler) = 0;
    };

#endif /* MCSCONVERSATIONREADOBSERVER_H_ */
