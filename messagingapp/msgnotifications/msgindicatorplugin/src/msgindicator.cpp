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
 * Description:
 *
 */
#include "msgindicator.h" 
#include "msgindicator_p.h"   

#include <QThreadPool>
#include <QRunnable>
#include <QByteArray>
#include <QProcess>
#include "debugtraces.h"

#include <xqservicerequest.h>

//Localized Constants
#define LOC_UNREAD_MESSAGES hbTrId("txt_messaging_list_ln")
#define LOC_NEW_MESSAGES hbTrId("txt_common_opt_ln_new_messages")

//----------------------------------------------------------------------
class ServiceRequestSenderTask : public QRunnable
 {
public:
     ServiceRequestSenderTask(qint64 conversationId):mConvId(conversationId)  {}
     
     void run()
     {
     XQServiceRequest snd(
            "com.nokia.services.hbserviceprovider.conversationview",
                "open(qint64)",false);
		  snd << mConvId;
		  bool res=snd.send();    
     }
     
     ~ServiceRequestSenderTask()
     {     
     }
     
 private: 
 	qint64 mConvId;    
 };
 
//----------------------------------------------------------------------


// ----------------------------------------------------------------------------
// MsgIndicator::MsgIndicator
// @see msgindicator.h
// ----------------------------------------------------------------------------
MsgIndicator::MsgIndicator(const QString &indicatorType) :
HbIndicatorInterface(indicatorType,
        HbIndicatorInterface::GroupPriorityHigh,
        InteractionActivated)
    {
    d_ptr = new MsgIndicatorPrivate(this);
    }

// ----------------------------------------------------------------------------
// MsgIndicator::~MsgIndicator
// @see msgindicator.h
// ----------------------------------------------------------------------------
MsgIndicator::~MsgIndicator()
    {
    delete d_ptr;
    }

// ----------------------------------------------------------------------------
// MsgIndicator::handleInteraction
// @see msgindicator.h
// ----------------------------------------------------------------------------
bool MsgIndicator::handleInteraction(InteractionType type)
    {
    bool handled = false;
    if (type == InteractionActivated) 
        {
        QThreadPool::globalInstance()->start(
        	new ServiceRequestSenderTask(mConversationId)); 
        handled = true;   
        }
    return handled;
    }

// ----------------------------------------------------------------------------
// MsgIndicator::indicatorData
// @see msgindicator.h
// ----------------------------------------------------------------------------
QVariant MsgIndicator::indicatorData(int role) const
{
switch(role)
    {
    case TextRole: 
        {
        mConversationId = -100;
        
        IndicatorData data;
        d_ptr->getIndicatorInfo(data);
             
        mConversationFromSingleContact = data.mFromSingle;
        mBodyText = data.mDescription;
        
        if (mConversationFromSingleContact)
            {
            mConversationId = (int)data.mConversationId;
            QString name = prepareDisplayName(data);
            name.append(QString(" (%1)").arg(data.mUnreadMsgCount));
            return name;
            }
        else 
            {
            QString text = QString("%1 new messages").arg(data.mUnreadMsgCount);
            return text;
            }   
        }
    case SecondaryTextRole:
        {
        if (mConversationFromSingleContact)
            {
            return mBodyText;
            }
        else 
            {
            return QString();
            }   
        }
    case IconNameRole:
        {
        QString iconName("z:/resource/messaging/message.svg");
        return iconName;
        }
    default: 
        return QVariant();      
    }
}

// ----------------------------------------------------------------------------
// MsgIndicator::handleClientRequest
// @see msgindicator.h
// ----------------------------------------------------------------------------
bool MsgIndicator::handleClientRequest( RequestType type, 
        const QVariant &parameter)
    {
    Q_UNUSED(parameter);
    bool handled(false);
    switch (type) {
        case RequestActivate:
            {
            emit dataChanged();
            handled =  true;
            }

            break;
        case RequestDeactivate:
            {
            // reset data 
            mConversationId = -100;
            mBodyText =  QString();
            emit deactivate();
            }
            break;
        default:
            break;
    }

    return handled;
    }

// ----------------------------------------------------------------------------
// MsgIndicator::prepareDisplayName
// @see msgindicator.h
// ----------------------------------------------------------------------------
QString MsgIndicator::prepareDisplayName(IndicatorData& indicatorData) const

    {
    //Set the Contact Name/Number
    QString firstName(indicatorData.mFirstName);
    QString lastName(indicatorData.mLastName);
    QString contactAddress(indicatorData.mContactNum);
    QString nickName(indicatorData.mNickName);

    QString displayName;

    if (!nickName.isEmpty())
        {
        displayName.append(nickName);
        }
    else if ( firstName.isEmpty() && lastName.isEmpty())
        {
        displayName.append(contactAddress);
        }
    else if (lastName.isEmpty() && !firstName.isEmpty())
        {
        displayName.append(firstName);
        }
    else if (firstName.isEmpty() && !lastName.isEmpty())
        {
        displayName.append(lastName);
        }
    else
        {
        // If both first Name and last name are present
        displayName.append(firstName);
        displayName.append(" ");
        displayName.append(lastName);
        }
    return displayName;
    }
