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
 * Description: Notification server main class. 
 *  Handles calling notifications and inidcations
 *
 */

// SYSTEM INCLUDES
#include <hbdevicedialog.h>
#include <hbindicator.h>


//USER INCLUDES
#include "msgnotifier.h"
#include "msgnotifier_p.h"
#include "msgsimnumberdetector.h"
#include "msgnotificationdialogpluginkeys.h"
#include "debugtraces.h"

// plugin ids 
const QString InidcationsPluginId
                          ("com.nokia.messaging.indicatorplugin/1.0");
const QString NotificationPluginId
                          ("com.nokia.messaging.newmsgnotificationdialog/1.0");

// ----------------------------------------------------------------------------
// MsgNotifier::MsgNotifier
// @see MsgNotifier.h
// ----------------------------------------------------------------------------
MsgNotifier::MsgNotifier(QObject* parent):QObject(parent)
    {
    QDEBUG_WRITE("MsgNotifier::MsgNotifier : Enter")
    
     d_ptr = new MsgNotifierPrivate(this); 
     
     mSimHandler = new MsgSimNumDetector();
     
    QDEBUG_WRITE("MsgNotifier::MsgNotifier : Exit") 
    }

// ----------------------------------------------------------------------------
// MsgNotifier::~MsgNotifier
// @see MsgNotifier.h
// ----------------------------------------------------------------------------
MsgNotifier::~MsgNotifier()
    {
    QDEBUG_WRITE("MsgNotifier::~MsgNotifier : Enter")
    
    delete d_ptr;
    delete mSimHandler;
    
    QDEBUG_WRITE("MsgNotifier::~MsgNotifier : Enter")
    }



// ----------------------------------------------------------------------------
// MsgNotifier::displayNewMessageNotification
// @see MsgNotifier.h
// ----------------------------------------------------------------------------
void MsgNotifier::displayNewMessageNotification(NotificationData& data)
    {  
    QDEBUG_WRITE("MsgNotifier::displayNewMessageNotification : Enter")
    QDEBUG_WRITE("MsgNotifier::displayNewMessageNotification :"
    																				" Printing notification data")
    
    QDEBUG_WRITE_FORMAT("First Name : ", data.mFirstName);
    QDEBUG_WRITE_FORMAT("Last Name : ", data.mLastName);
    QDEBUG_WRITE_FORMAT("Number : ", data.mContactNum);
    QDEBUG_WRITE_FORMAT("Description : ", data.mDescription);
    QDEBUG_WRITE_FORMAT("Type : ", data.mMsgType);
    QDEBUG_WRITE_FORMAT("Conv Id : ", data.mConversationId);
    
    // Fill data to variant map
    QVariantMap notificationData;
    notificationData[QString(KFirstNameKey)] = data.mFirstName ;
    notificationData[QString(KLastNameKey)] = data.mLastName;
    notificationData[QString(KNickNameKey)] = QString();
    notificationData[QString(KConversationIdKey)] = data.mConversationId;
    notificationData[QString(KMessageTypeKey)] = data.mMsgType;
    notificationData[QString(KMessageBodyKey)] =  data.mDescription;
    notificationData[QString(KMessageSubjectKey)] = QString();
    notificationData[QString(KContactAddressKey)] = data.mContactNum;

    // call device dialog to show notification
    HbDeviceDialog deviceDialog ;
    deviceDialog.show(NotificationPluginId,notificationData);
    
    QDEBUG_WRITE("MsgNotifier::displayNewMessageNotification : Exit")
    }

    
// ----------------------------------------------------------------------------
// MsgNotifier::updateIndications
// @see MsgNotifier.h
// ----------------------------------------------------------------------------
void MsgNotifier::updateIndications(int unreadCount)
    {
    QDEBUG_WRITE("MsgNotifier::updateIndications  Enter")
    
    HbIndicator indicator; 
    if(unreadCount)
        {
        indicator.activate(InidcationsPluginId);      
        QDEBUG_WRITE("MsgNotifier::updateIndications Indications Activated")
        }
    else
        {
        indicator.deactivate(InidcationsPluginId);
        QDEBUG_WRITE("MsgNotifier::updateIndications Indications Deactivated")
        }
    
    QDEBUG_WRITE("MsgNotifier::updateIndications  Exit")
    }


//EOF
