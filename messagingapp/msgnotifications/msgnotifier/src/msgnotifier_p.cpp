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
#include <xqservicerequest.h>
#include <QString>
#include <w32std.h>
#include <apgtask.h> 
#include <XQSettingsManager>
#include <xqpublishandsubscribeutils.h>
#include <xqsystemtoneservice.h>

//USER INCLUDES
#include "msgnotifier.h"
#include "msgnotifier_p.h"
#include <xqconversions.h>
#include "msgstorehandler.h"
#include "msginfodefs.h"
#include "conversationidpsconsts.h"
#include "debugtraces.h"


// ----------------------------------------------------------------------------
// MsgNotifierPrivate::MsgNotifierPrivate
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
MsgNotifierPrivate::MsgNotifierPrivate(MsgNotifier* MsgNotifier) :
    q_ptr(MsgNotifier), mCvServer(NULL), iMsgStoreHandler(NULL),
    mSettingsManager(NULL),mPSUtils(NULL)
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
    if (mCvServer) {
        mCvServer->RemoveConversationListChangeEventL(this);
        delete mCvServer;
        mCvServer = NULL;
    }

    if (iMsgStoreHandler) {
        delete iMsgStoreHandler;
        iMsgStoreHandler = NULL;
    }
    
    if(mPSUtils)
        {
        delete mPSUtils;
        }
    
    if(mSettingsManager)
        {
        delete mSettingsManager;
        }
    
    if(mSts)
        {
        delete mSts;
        mSts = NULL;
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
    iMsgStoreHandler = new MsgStoreHandler(this, mCvServer);
    updateUnreadIndications(true); 
    updateOutboxIndications();

    mSettingsManager = new XQSettingsManager();
    
    // define property
    mPSUtils = new XQPublishAndSubscribeUtils(*mSettingsManager);
    XQPublishAndSubscribeSettingsKey convIdKey(
            KMsgCVIdProperty, KMsgCVIdKey);
    bool success = mPSUtils->defineProperty(convIdKey, 
                            XQSettingsManager::TypeInt);
    
    QDEBUG_WRITE_FORMAT("MsgNotifierPrivate::initL "
                        "property creation ret value",success)
    
    // write -1 initially 
    success = mSettingsManager->writeItemValue(convIdKey,-1);
    
    QDEBUG_WRITE_FORMAT("MsgNotifierPrivate::initL "
                           "writing ret value",success)
    
    mSts = new XQSystemToneService();
    
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
    updateUnreadIndications();

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
    updateUnreadIndications();
    QDEBUG_WRITE("MsgNotifierPrivate::DeleteConversationList : Exit")
}

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::ModifyConversationList
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::ModifyConversationList(const CCsClientConversation& aClientConversation)
{
    QDEBUG_WRITE("MsgNotifierPrivate::ModifyConversationList : Enter")

    processListEntry(aClientConversation);
    updateUnreadIndications();

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
   
    if((convEntry->ConversationDir() == ECsDirectionIncoming)
            && convEntry->IsAttributeSet(ECsAttributeNewEntryAdded) 
            && convEntry->IsAttributeSet(ECsAttributeUnread))
        {
        QDEBUG_WRITE("processListEntry : Processing data for Notification")
        
        NotificationData notifData;
        notifData.msvEntryId = convEntry->EntryId(); 
        notifData.mConversationId = aClientConversation.GetConversationEntryId();
        notifData.mMsgType = static_cast<int>(convEntry->GetType());
        
        HBufC* displayName = aClientConversation.GetDisplayName();
        HBufC* number =  convEntry->Contact();
        HBufC* descrp =  convEntry->Description();
        
        if(displayName)
            {
            notifData.mDisplayName = 
                                XQConversions::s60DescToQString(*displayName);
            }        
        if(number)
            {
            notifData.mContactNum =  XQConversions::s60DescToQString(*number);
            }
        if(descrp)
            {
            notifData.mDescription = XQConversions::s60DescToQString(*descrp);
            }
        
        //Play new message alert tone.
        mSts->playTone(XQSystemToneService::SmsAlertTone);
        
        // check whether opened cv id and received 
        // cv id are same and show notification
        if( showNotification(notifData.mConversationId ))
            {
             q_ptr->displayNewMessageNotification(notifData);
             QDEBUG_WRITE("processListEntry : Notification display called")
            }
       
        }
    
    QDEBUG_WRITE("MsgNotifierPrivate::processListEntry : Exit")
    }

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::updateUnreadIndications
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::updateUnreadIndications(bool bootup)
{
    QDEBUG_WRITE("MsgNotifierPrivate::updateIndicationsL : Enter")

    int unreadCount(0);
   if(bootup)
   {
       TRAP_IGNORE(unreadCount = iMsgStoreHandler->GetUnreadMessageCountL());
   }
   else
   {
       TRAP_IGNORE(unreadCount = mCvServer->GetTotalUnreadCountL());
   }

    QDEBUG_WRITE_FORMAT("MsgNotifierPrivate::"
        "updateIndications unreadCount = ",unreadCount );

    //activate or deactivate indications based on unread count
    q_ptr->updateUnreadIndications(unreadCount);

    QDEBUG_WRITE("MsgNotifierPrivate::updateIndicationsL : Exit")
}

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::updateOutboxIndications
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::updateOutboxIndications()
{

    MsgInfo failedIndicatorData;
    MsgInfo pendingIndicatorData;
    TInt err = KErrNone;
    
    TRAP(err, iMsgStoreHandler->GetOutboxEntriesL(failedIndicatorData,pendingIndicatorData));

    q_ptr->updateOutboxIndications(failedIndicatorData);
    q_ptr->updateOutboxIndications(pendingIndicatorData);
}


// ----------------------------------------------------------------------------
// MsgNotifierPrivate::displayOutboxIndications
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------

void MsgNotifierPrivate::displayOutboxIndications(MsgInfo data)
	{
		q_ptr->updateOutboxIndications(data);
	}

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::displayFailedNote
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::displayFailedNote(MsgInfo info)
{
    // TODO: use XQAiwRequest
    QDEBUG_WRITE("[MsgNotifierPrivate::handleFailedState] : entered")
    // change to com.nokia.symbian.messaging (servicename), IMsgErrorNotifier
    // as the service name.
    XQServiceRequest snd("messaging.com.nokia.symbian.MsgErrorNotifier",
        "displayErrorNote(QVariantList)", false);

    QVariantList args;
    info.mDisplayName.removeDuplicates();
    info.mDisplayName.sort();
    
    QString nameString;
    
    nameString.append(info.mDisplayName.at(0));
    for(int i = 1; i < info.mDisplayName.count(); ++i){
        nameString.append(", ");
        nameString.append(info.mDisplayName.at(i));
    }
      
    //Even if name string is empty we shall add name into args
    QVariant nameV(nameString);
    args << nameV;

    QDEBUG_WRITE("[MsgNotifierPrivate::handleFailedState] : name and contactnumber")

    args << info.mConversationId;
    args << info.mMessageType;
    snd << args;
    snd.send();
    QDEBUG_WRITE("[MsgNotifierPrivate::handleFailedState] : left")

}

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::showNotification
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
bool MsgNotifierPrivate::showNotification(int receivedMsgConvId)
{
    bool showNotification = true;
    
    RWsSession wsSession ;
    wsSession.Connect();

    TApaTaskList taskList( wsSession );
    TApaTask task = taskList.FindApp(KMsgAppUid); // find msgapp is running

    if(task.Exists())
        {
        TApaTask foregndtask =   taskList.FindByPos(0) ;  // foreground app
        // compare  window group id  
        // if application is in foregorund, then check the currently
        // opened conversation is same as received one.
        if(task.WgId() == foregndtask.WgId() )
            {
            // get the current conversation ID
            XQPublishAndSubscribeSettingsKey convIdKey( KMsgCVIdProperty, 
                    KMsgCVIdKey);
            QVariant value = mSettingsManager->readItemValue(convIdKey, 
                    XQSettingsManager::TypeInt);

            int openedConvId  = value.toInt();
            if( openedConvId == receivedMsgConvId)
                {
                showNotification = false;
                QDEBUG_WRITE("processListEntry : Notification not shown")
                }
            }
        }

    wsSession.Close();
    return showNotification;
}
//EOF
