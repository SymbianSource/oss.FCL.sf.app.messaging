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

#include "msgconversationviewitem.h"

// SYSTEM INCLUDES
#include <QDateTime>
#include "debugtraces.h"
#include <QDir>
#include <HbTextItem>
#include <HbIconItem>

// USER INCLUDES
#include "msgconversationwidget.h"
#include "msgviewdefines.h"
#include "msgviewutils.h"
#include "convergedmessage.h"
#include "conversationsenginedefines.h"

// LOCAL CONSTANTS

const QString MSG_FAIL_ICON(":/icons/qtg_small_fail.png");

#define LOC_RINGING_TONE hbTrId("txt_messaging_dpopinfo_ringing_tone")
#define LOC_BUSINESS_CARD hbTrId("txt_messaging_list_business_card")
#define LOC_CALENDER_EVENT hbTrId("txt_messaging_list_calendar_event")
#define LOC_UNSUPPORTED_MESSAGE hbTrId("txt_messaging_list_listview_unsupported_message_type")

//---------------------------------------------------------------
// MsgConversationViewItem::MsgConversationViewItem
// Constructor
//---------------------------------------------------------------
MsgConversationViewItem::MsgConversationViewItem(QGraphicsItem* parent) :
    HbListViewItem(parent), mIncoming(false), mConversation(0),
            mMessageStateIconItem(0)
{
    mConversation = new MsgConversationWidget(this);
    HbStyle::setItemName(mConversation, "msgconvwidget");

    mMessageStateIconItem = new HbIconItem(this);
    HbStyle::setItemName(mMessageStateIconItem, "msgStateIcon");
}

//---------------------------------------------------------------
// MsgConversationViewItem::MsgConversationViewItem
// Destructor
//---------------------------------------------------------------
MsgConversationViewItem::~MsgConversationViewItem()
{

}

//---------------------------------------------------------------
// MsgConversationViewItem::createItem
// Create a new decorator item.
//---------------------------------------------------------------
MsgConversationViewItem* MsgConversationViewItem::createItem()
{
    return new MsgConversationViewItem();
}

//---------------------------------------------------------------
// MsgConversationViewItem::updateChildItems
//
//---------------------------------------------------------------
void MsgConversationViewItem::updateChildItems()
{
    QModelIndex index = modelIndex();

#ifdef _DEBUG_TRACES_
    qCritical() << "START MsgConversationViewItem::updateChildItems: "
            << index.data(ConvergedMsgId).toInt();
#endif

    // Set message properties common to SMS/MMS/IM etc..

    int direction = index.data(Direction).toInt();

    if ( (direction == ConvergedMessage::Incoming) ||
            (direction == ConvergedMessage::Missed))
    {
        setIncoming(true);
        mConversation->setIncoming(true);
    }
    else if (direction == ConvergedMessage::Outgoing)
    {
        setIncoming(false);
        mConversation->setIncoming(false);

        int sendingState = index.data(SendingState).toInt();
        mConversation->setSendingState(sendingState);
        setMessageStateIcon(sendingState);
    }

    bool unreadStatus = index.data(UnReadStatus).toBool();
    mConversation->setUnread(unreadStatus);
    // Create items common to SMS/MMS/IM etc...
    mConversation->drawBubbleFrame();
    mConversation->drawNewItemFrame();

    QDateTime dateTime;
    dateTime.setTime_t(index.data(TimeStamp).toUInt());
    if (dateTime.date() == QDateTime::currentDateTime().date())
    {
        mConversation->setTimeStamp(dateTime.toString(TIME_FORMAT));
    }
    else
    {
        mConversation->setTimeStamp(dateTime.toString(DATE_FORMAT));
    }

    // Create items common to each type of message

    QString bodyText = index.data(BodyText).toString();
    int messageType = index.data(MessageType).toInt();

    QDEBUG_WRITE_FORMAT("MsgConversationViewItem::updateChildItems msg type value",messageType)
    
    if (messageType == ConvergedMessage::Sms ||
            messageType == ConvergedMessage::IM)
    {
    QDEBUG_WRITE("MsgConversationViewItem::updateChildItems SMS block")
        mConversation->setMMS(false);
        mConversation->setBodyText(bodyText);
    }
    else if (messageType == ConvergedMessage::Mms)
    {
    QDEBUG_WRITE("MsgConversationViewItem::updateChildItems MMS block")
    
        mConversation->setMMS(true);
        QString attachments = index.data(Attachments).toString();
        QString subject = index.data(Subject).toString();

        bool hasAttachments = (index.data(MessageProperty).toInt()
                & ConvergedMessage::Attachment) ? true : false;

        if (hasAttachments)
        {
            mConversation->setAttachment();
        }
        else
        {
            mConversation->setAttachment(false);
        }
        QStringList mediaFiles = attachments.split('|',
            QString::SkipEmptyParts);
        //Can be used for preview icon
        QStringList imageFiles;

        MsgViewUtils viewUtils;

        for (int i = 0; i < mediaFiles.size(); ++i)
        {
            QString type = viewUtils.mimeTypeL(QDir::toNativeSeparators(
                mediaFiles.at(i)));

            if (type.contains("image"))
            {
                imageFiles << mediaFiles.at(i);
                mConversation->setImage(true);
            }
            else if (type.contains("video"))
            {
                mConversation->setVideo(false);
            }
            else if (type.contains("audio"))
            {
                mConversation->setAudio(true);
            }
        }
        // Now set the media contents
        if (!imageFiles.isEmpty())
        {
            mConversation->setPreviewIconPath(imageFiles.at(0));
        }
        mConversation->displayAudioIcon();

        int priority = index.data(MessagePriority).toInt();
        mConversation->setPriority(priority);
        mConversation->setSubject(subject);
        mConversation->setBodyText(bodyText);

    } // END of ConvergedMessage::Mms
    else if (messageType == ConvergedMessage::RingingTone)
    {
        QDEBUG_WRITE("MsgConversationViewItem::updateChildItems Ringing Tone block")
        mConversation->setMMS(true);
        mConversation->setImage(false);
        mConversation->setAudio(true);
        mConversation->displayAudioIcon();
        mConversation->setSubject(LOC_RINGING_TONE); 
    }
	/// for all the biotype messages
    else if (messageType == ConvergedMessage::VCard)  
    {
        mConversation->setMMS(false);
        mConversation->setBodyText(LOC_BUSINESS_CARD);
    }
	/// for all the biotype messages
    else if (messageType == ConvergedMessage::VCal)  
    {
        mConversation->setMMS(false);
        mConversation->setBodyText(LOC_CALENDER_EVENT);
    }
	/// for all the biotype messages
    else if (messageType == ConvergedMessage::BioMsg)  
    {
        mConversation->setMMS(false);
        mConversation->setBodyText(LOC_UNSUPPORTED_MESSAGE);
    }
   

#ifdef _DEBUG_TRACES_
    qCritical() << "END MsgConversationViewItem::updateChildItems: "
            << index.data(ConvergedMsgId).toInt();
#endif

    HbListViewItem::updateChildItems();
}

//---------------------------------------------------------------
// MsgConversationViewItem::containsPoint
//
//---------------------------------------------------------------
bool MsgConversationViewItem::containsPoint(const QPointF& point)
{
    return mConversation->boundingRect().
    contains(mConversation->mapFromScene(point));
}

//---------------------------------------------------------------
// MsgConversationViewItem::setIncoming
// @see header file
//---------------------------------------------------------------
void MsgConversationViewItem::setIncoming(bool incoming)
{
    mIncoming = incoming;
}

//---------------------------------------------------------------
// MsgConversationViewItem::isIncoming
// @see header file
//---------------------------------------------------------------
bool MsgConversationViewItem::isIncoming()
{
    return mIncoming;
}

//---------------------------------------------------------------
// MsgConversationViewItem::setIncoming
// @see header file
//---------------------------------------------------------------
void MsgConversationViewItem::setMessageStateIcon(int messageState)
{
    switch (messageState)
    {
        case ConvergedMessage::Suspended:
        case ConvergedMessage::Resend:
        {
            //TODO: Temp icon until official icons are received
            mMessageStateIconItem->setIcon(MSG_FAIL_ICON);
            mMessageStateIconItem->setVisible(true);
            break;
        }
        case ConvergedMessage::Failed:
        {
            //TODO: Temp icon until official icons are received
            mMessageStateIconItem->setIcon(MSG_FAIL_ICON);
            mMessageStateIconItem->setVisible(true);
            break;
        }        
        default:
        {
            mMessageStateIconItem->setVisible(false);
            break;
        }
    }
}

// EOF
