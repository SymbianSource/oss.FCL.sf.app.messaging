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
 * events. These parse infomation passed to notifications and indications.
 *
 */

// SYSTEM INCLUDES
#include <msvapi.h>
#include <ccsclientconversation.h>
#include <ccsrequesthandler.h>
#include <ccsconversationentry.h>

//USER INCLUDES
#include "msgnotifier.h"
#include "msgnotifier_p.h"
#include "s60qconversions.h"

#include <QtDebug>
  
#define QDEBUG_WRITE(str) {qDebug() << str;}
#define QDEBUG_WRITE_FORMAT(str, val) {qDebug() << str << val;}
#define QCRITICAL_WRITE(str) {qCritical() << str;}
#define QCRITICAL_WRITE_FORMAT(str, val) {qCritical() << str << val;}

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::MsgNotifierPrivate
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
MsgNotifierPrivate::MsgNotifierPrivate(MsgNotifier* MsgNotifier):
q_ptr(MsgNotifier),
mCvServer(NULL)
{
QDEBUG_WRITE("MsgNotifierPrivate::MsgNotifierPrivate : Enter")
TRAP_IGNORE(initL());
QDEBUG_WRITE("MsgNotifierPrivate::MsgNotifierPrivate : Exit")
}

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::~MsgNotifierPrivate
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
MsgNotifierPrivate::~MsgNotifierPrivate()
{
QDEBUG_WRITE("MsgNotifierPrivate::~MsgNotifierPrivate : Enter")	
if(mCvServer)
    {
    mCvServer->RemoveConversationListChangeEventL (this);
    delete mCvServer;
    mCvServer = NULL;
    }
QDEBUG_WRITE("MsgNotifierPrivate::~MsgNotifierPrivate : Exit")    
}

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::initL
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::initL()
    {
    QDEBUG_WRITE("MsgNotifierPrivate::initL : Enter")
    
    mCvServer = CCSRequestHandler::NewL();
    mCvServer->RequestConversationListChangeEventL(this);
    updateIndications();
    
    QDEBUG_WRITE("MsgNotifierPrivate::initL : Exit")
    }

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::AddConversationList
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::AddConversationList(
           const CCsClientConversation& aClientConversation)
    {
    QDEBUG_WRITE("MsgNotifierPrivate::AddConversationList : Enter")
    
    processListEntry(aClientConversation);
    updateIndications();
    
    QDEBUG_WRITE("MsgNotifierPrivate::AddConversationList : Exit")
    }

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::DeleteConversationList
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::DeleteConversationList(
           const CCsClientConversation& /*aClientConversation*/) 
    {
    QDEBUG_WRITE("MsgNotifierPrivate::DeleteConversationList : Enter")	
    updateIndications();
    QDEBUG_WRITE("MsgNotifierPrivate::DeleteConversationList : Exit")
    }

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::ModifyConversationList
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate:: ModifyConversationList(
           const CCsClientConversation& aClientConversation)
    {
    QDEBUG_WRITE("MsgNotifierPrivate::ModifyConversationList : Enter")
    
    processListEntry(aClientConversation);
    updateIndications();
    
    QDEBUG_WRITE("MsgNotifierPrivate::ModifyConversationList : Exit")
    }
   
// ----------------------------------------------------------------------------
// MsgNotifierPrivate::RefreshConversationList
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::RefreshConversationList()
    {
    
    }

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::processListEntry
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::processListEntry(
        const CCsClientConversation& aClientConversation)
    {
    QDEBUG_WRITE("MsgNotifierPrivate::processListEntry : Enter")
    
    CCsConversationEntry* convEntry = 
                                 aClientConversation.GetConversationEntry();
    
    QDEBUG_WRITE_FORMAT("MsgNotifierPrivate::processListEntry Direction",
                                                convEntry->ConversationDir())
    QDEBUG_WRITE_FORMAT("MsgNotifierPrivate::processListEntry New Entry Attribute",
                                                convEntry->IsAttributeSet(ECsAttributeNewEntryAdded))
    QDEBUG_WRITE_FORMAT("MsgNotifierPrivate::processListEntry Unread Attribute",
                                                convEntry->IsAttributeSet(ECsAttributeUnread))

    bool newFlag=convEntry->IsAttributeSet(ECsAttributeNewEntryAdded);
    
    if((convEntry->ConversationDir() == ECsDirectionIncoming)
            && convEntry->IsAttributeSet(ECsAttributeNewEntryAdded) 
            && convEntry->IsAttributeSet(ECsAttributeUnread))
        {
        QDEBUG_WRITE("processListEntry : Processing data for Notification")
        
        NotificationData notifData;
        
        notifData.mConversationId = aClientConversation.GetConversationEntryId();
        notifData.mMsgType = convEntry->GetType();
        
        HBufC* firstName = aClientConversation.GetFirstName();
        HBufC* lastName =  aClientConversation.GetLastName();
        HBufC* number =  convEntry->Contact();
        HBufC* descrp =  convEntry->Description();
        
        if(firstName)
            {
            notifData.mFirstName = 
                                S60QConversions::s60DescToQString(*firstName);
            }
        if(lastName)
            {
            notifData.mLastName = S60QConversions::s60DescToQString(*lastName);
            }
        if(number)
            {
            notifData.mContactNum =  S60QConversions::s60DescToQString(*number);
            }
        if(descrp)
            {
            notifData.mDescription = S60QConversions::s60DescToQString(*descrp);
            }
        
        q_ptr->displayNewMessageNotification(notifData);
        
        QDEBUG_WRITE("processListEntry : Notification display called")
        }
    
    QDEBUG_WRITE("MsgNotifierPrivate::processListEntry : Exit")
    }

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::updateIndications
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::updateIndications()
    {
    QDEBUG_WRITE("MsgNotifierPrivate::updateIndicationsL : Enter")  

    int unreadCount(0); 
    TRAP_IGNORE(unreadCount = mCvServer->GetTotalUnreadCountL());

    QDEBUG_WRITE_FORMAT("MsgNotifierPrivate::"
            "updateIndications unreadCount = ",unreadCount );

    //activate or deactivate indications based on unread count
    q_ptr->updateIndications(unreadCount);

    QDEBUG_WRITE("MsgNotifierPrivate::updateIndicationsL : Exit")
    }
//EOF
