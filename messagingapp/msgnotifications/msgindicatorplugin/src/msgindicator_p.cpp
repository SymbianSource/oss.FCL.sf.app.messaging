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
 * Description:Message Indicator private class. 
 * Gets the indicaton information from conversation server.
 *
 */
#include <ccsclientconversation.h>
#include <ccsrequesthandler.h>
#include <ccsconversationentry.h>

#include "msgindicator_p.h"  
#include "msgindicator.h"
#include "debugtraces.h"
#include "s60qconversions.h"

// ----------------------------------------------------------------------------
// MsgIndicatorPrivate::MsgIndicatorPrivate
// @see MsgIndicatorPrivate.h
// ----------------------------------------------------------------------------
MsgIndicatorPrivate::MsgIndicatorPrivate(MsgIndicator* inidcator):
	q_ptr(inidcator),mCvServer(NULL)
    {
    TRAP_IGNORE(initL());
    }

// ----------------------------------------------------------------------------
// MsgIndicatorPrivate::~MsgIndicatorPrivate
// @see MsgIndicatorPrivate.h
// ----------------------------------------------------------------------------
MsgIndicatorPrivate::~MsgIndicatorPrivate()
    {
    if(mCvServer)
        {
        delete mCvServer;
        mCvServer = NULL;
        }
    }

// ----------------------------------------------------------------------------
// MsgIndicatorPrivate::initL()
// @see MsgIndicatorPrivate.h
// ----------------------------------------------------------------------------
void MsgIndicatorPrivate::initL()
    {
    mCvServer = CCSRequestHandler::NewL();
    }

// ----------------------------------------------------------------------------
// MsgIndicatorPrivate::getIndicatorInfo
// @see MsgIndicatorPrivate.h
// ----------------------------------------------------------------------------
void MsgIndicatorPrivate::getIndicatorInfo(IndicatorData& indicatorData)
    {
    TRAP_IGNORE(getIndicatorInfoL(indicatorData));
    }
    
// ----------------------------------------------------------------------------
// MsgIndicatorPrivate::getIndicatorInfoL
// @see MsgIndicatorPrivate.h
// ----------------------------------------------------------------------------
void MsgIndicatorPrivate::getIndicatorInfoL(IndicatorData& indicatorData)
    { 
    RPointerArray<CCsClientConversation> *clientConversationList = 
                                    new RPointerArray<CCsClientConversation> ();
    CleanupStack::PushL(clientConversationList);   
    mCvServer->GetConversationUnreadListL(clientConversationList);
    CleanupStack::Pop();
    
    int count = clientConversationList->Count();
    int msgCount = 0;
    if(count== 1)
        {    
        indicatorData.mFromSingle = true;
        
        CCsClientConversation* conversation =
        static_cast<CCsClientConversation*> ((*clientConversationList)[0]);
        
        CCsConversationEntry* convEntry = conversation->GetConversationEntry();
        
        indicatorData.mUnreadMsgCount = conversation->GetUnreadMessageCount();
        indicatorData.mConversationId = conversation->GetConversationEntryId();
        
        HBufC* firstName =conversation->GetFirstName();
        HBufC* lastName = conversation->GetLastName();    
        HBufC* number =  convEntry->Contact();
        HBufC* decription = convEntry->Description();
        
        if(firstName)
            {
            indicatorData.mFirstName = S60QConversions::s60DescToQString(*firstName);
            }
        if(lastName)
            {
            indicatorData.mLastName = S60QConversions::s60DescToQString(*lastName);
            }
        if(number)
            {
            indicatorData.mContactNum  =  S60QConversions::s60DescToQString(*number);
            }
        if(decription)
            {
            indicatorData.mDescription  = S60QConversions::s60DescToQString(*decription);
            }
        
        }
    else
        {
        indicatorData.mFromSingle = false;
        for (int loop = 0; loop < count; ++loop)
            {
            CCsClientConversation* conversation =
            static_cast<CCsClientConversation*> ((*clientConversationList)[loop]);
            msgCount += conversation->GetUnreadMessageCount();
            }
        indicatorData.mUnreadMsgCount = msgCount;
        }
    
    // Clean & destroy clientConversationList
       clientConversationList->ResetAndDestroy();
       delete clientConversationList;
    }
    



