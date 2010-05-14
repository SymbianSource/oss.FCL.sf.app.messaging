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
#include <QChar>
#include <HbTextItem>
#include <HbIconItem>
#include <HbIconAnimationManager>
#include <HbIconAnimator>
#include <ccsdefs.h>

// USER INCLUDES
#include "msgconversationwidget.h"
#include "msgviewdefines.h"
#include "msgviewutils.h"
#include "convergedmessage.h"
#include "conversationsenginedefines.h"

// LOCAL CONSTANTS
const QString MSG_OUTBOX_ICON("qtg_small_outbox");
const QString MSG_FAIL_ICON("qtg_small_fail");
const QString ANIMATION_FILE(":/qtg_anim_loading.axml");
const QString ANIMATION_ICON_NAME("qtg_anim_loading");
const QString VCARD_ICON("qtg_large_mycard");

// LOCALIZATION
#define LOC_RINGING_TONE hbTrId("txt_messaging_dpopinfo_ringing_tone")
#define LOC_BUSINESS_CARD hbTrId("txt_messaging_list_business_card")
#define LOC_CALENDAR_EVENT hbTrId("txt_messaging_list_calendar_event")
#define LOC_UNSUPPORTED_MSG_TYPE hbTrId("txt_messaging_list_listview_unsupported_message_type")
#define LOC_RESEND_AT hbTrId("Resend at ")

//---------------------------------------------------------------
// MsgConversationViewItem::MsgConversationViewItem
// Constructor
//---------------------------------------------------------------
MsgConversationViewItem::MsgConversationViewItem(QGraphicsItem* parent) :
    HbListViewItem(parent), mIncoming(false), mConversation(0),
            mMessageStateIconItem(0)
{
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
    return new MsgConversationViewItem(*this);
    }

//---------------------------------------------------------------
// MsgConversationViewItem::updateChildItems
//
//---------------------------------------------------------------
void MsgConversationViewItem::updateChildItems()
    {

    if (!mMessageStateIconItem)
        {
        mMessageStateIconItem = new HbIconItem(this);
        HbStyle::setItemName(mMessageStateIconItem, "msgStateIcon");
        }
    
    QModelIndex index = modelIndex();

#ifdef _DEBUG_TRACES_
    qCritical() << "START MsgConversationViewItem::updateChildItems: "
            << index.data(ConvergedMsgId).toInt();
#endif

    int messageType = index.data(MessageType).toInt();
    int messageSubType = index.data(MessageSubType).toInt();

    
    if (messageType == ConvergedMessage::Sms)
        {
        	updateSmsTypeItem(index);
        }
    else if (messageType == ConvergedMessage::Mms || messageType
            == ConvergedMessage::MmsNotification || messageType
            == ConvergedMessage::BT)
        {
        updateMmsTypeItem(index, messageType, messageSubType);
        }
    else if (messageType == ConvergedMessage::BioMsg)
        {
        if (messageSubType == ConvergedMessage::VCard || messageSubType
                == ConvergedMessage::RingingTone)
            {
            updateMmsTypeItem(index, messageType, messageSubType);
            }
        else
            {
            updateSmsTypeItem(index, messageSubType);
            }
        }

#ifdef _DEBUG_TRACES_
    qCritical() << "END MsgConversationViewItem::updateChildItems: "
            << index.data(ConvergedMsgId).toInt();
#endif

    repolish();
    HbListViewItem::updateChildItems();
    }

//---------------------------------------------------------------
// MsgConversationViewItem::updateSmsTypeItem
// @see header file
//---------------------------------------------------------------
void MsgConversationViewItem::updateSmsTypeItem(const QModelIndex& index, int messageSubType)
    {
    
    if (!mConversation)
        {
        mConversation = new MsgConversationWidget(this);
        HbStyle::setItemName(mConversation, "msgconvwidget");
        }
    mIncoming = false;
    mConversation->resetProperties();
    
    int direction = index.data(Direction).toInt();
    
    if (direction == ConvergedMessage::Incoming)
        {
        setIncoming(true);
        mConversation->setIncoming(true);

        mMessageStateIconItem->setVisible(false);
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

    mConversation->drawBubbleFrame();
    mConversation->drawNewItemFrame();

    QDateTime dateTime;
    dateTime.setTime_t(index.data(TimeStamp).toUInt());
    QString resendStateNote((index.data(SendingState).toInt()
                    == ConvergedMessage::Resend) ? LOC_RESEND_AT : "");
    if (dateTime.date() == QDateTime::currentDateTime().date())
        {

        mConversation->setTimeStamp(resendStateNote + dateTime.toString(
                        TIME_FORMAT));
        }
    else
        {
        mConversation->setTimeStamp(resendStateNote + dateTime.toString(
                        DATE_FORMAT));
        }

    if (messageSubType == ConvergedMessage::VCal)
        {
         mConversation->setSubject(LOC_UNSUPPORTED_MSG_TYPE);
        }
    else
        {
        QString bodyText = index.data(BodyText).toString();
        bodyText.replace(QChar::ParagraphSeparator, QChar::LineSeparator);
        bodyText.replace('\r', QChar::LineSeparator);
        mConversation->setSubject(bodyText);
        }
    
    //repolish
    mConversation->repolishWidget();
    }

//---------------------------------------------------------------
// MsgConversationViewItem::updateMmsTypeItem
// @see header file
//---------------------------------------------------------------
void MsgConversationViewItem::updateMmsTypeItem(const QModelIndex& index,
        int messageType, int messageSubType)
    {
    // create widget
    if (!mConversation)
        {
        mConversation = new MsgConversationWidget(this);
        HbStyle::setItemName(mConversation, "msgconvwidget");
        }
    mIncoming = false;
    mConversation->resetProperties();
    
    mConversation->setMMS(true);
    int direction = index.data(Direction).toInt();
    QString bodyText = index.data(BodyText).toString();

    if (direction == ConvergedMessage::Incoming)
        {
        setIncoming(true);
        mConversation->setIncoming(true);

        if (messageType == ConvergedMessage::MmsNotification)
            {
            mConversation->setMMSNotification(true);
            int notificationState = index.data(NotificationStatus).toInt();
            mConversation->setNotificationState(notificationState);
            setNotificationStateIcon(notificationState);
            }
        else
            {
            mMessageStateIconItem->setVisible(false);
            }
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

    mConversation->drawBubbleFrame();
    mConversation->drawNewItemFrame();

    QDateTime dateTime;
    dateTime.setTime_t(index.data(TimeStamp).toUInt());
    QString resendStateNote((index.data(SendingState).toInt()
            == ConvergedMessage::Resend) ? LOC_RESEND_AT : "");
    if (dateTime.date() == QDateTime::currentDateTime().date())
        {

        mConversation->setTimeStamp(resendStateNote + dateTime.toString(
                TIME_FORMAT));
        }
    else
        {
        mConversation->setTimeStamp(resendStateNote + dateTime.toString(
                DATE_FORMAT));
        }

    if (messageType == ConvergedMessage::Mms)
        {
        //preview path
        QString previewPath = index.data(Attachments).toString();
        QString subject = index.data(Subject).toString();

        int msgProperty = index.data(MessageProperty).toInt();
        bool hasAttachments = (msgProperty & EPreviewAttachment) ? true : false;

        if (hasAttachments)
            {
            mConversation->setAttachment();
            }
        else
            {
            mConversation->setAttachment(false);
            }
        
        // Now set the media contents
        
        //preview image
        bool hasImage = (msgProperty & EPreviewImage) ? true : false;

        if (hasImage)
            {
            int msgId = index.data(ConvergedMsgId).toInt();
            mConversation->setPreviewIconPath(previewPath, msgId);
            mConversation->setImage(true);
            }

        bool hasVideo = (msgProperty & EPreviewVideo) ? true : false;
        
        if (hasVideo)
            {
            mConversation->setVideo(true);
            }

        bool hasAudio = (msgProperty & EPreviewAudio) ? true : false;              
        if (hasAudio)
            {
            mConversation->setAudio(true);
            }

        mConversation->displayAudioIcon();

        int priority = index.data(MessagePriority).toInt();
        mConversation->setPriority(priority);
        mConversation->setSubject(subject);
        mConversation->setBodyText(bodyText);
        }
    else if (messageType == ConvergedMessage::BioMsg)
        {
        if (messageSubType == ConvergedMessage::RingingTone)
            {
            mConversation->setImage(false);
            mConversation->setAudio(true);
            mConversation->displayAudioIcon();
            mConversation->setSubject(LOC_RINGING_TONE);
            }
        else if (messageSubType == ConvergedMessage::VCard)
            {
            mConversation->setImage(false);
            mConversation->setAudio(true);
            mConversation->displayAudioIcon(VCARD_ICON);
            mConversation->setSubject(LOC_BUSINESS_CARD);
            mConversation->setBodyText(bodyText);
            }
        }
    else if (messageType == ConvergedMessage::BT)
        {
        QString deviceName = index.data(ConversationAddress).toString();
        mConversation->setSubject(deviceName);
        QString blueToothBody;
        if (messageSubType == ConvergedMessage::VCard)
            {
            mConversation->setImage(false);
            mConversation->setAudio(true);
            mConversation->displayAudioIcon(VCARD_ICON);
            blueToothBody.append(LOC_BUSINESS_CARD);
            blueToothBody.append(QChar::LineSeparator);
            blueToothBody.append(bodyText);
            }
        else
            {
            blueToothBody.append(bodyText);
            }
        mConversation->setBodyText(blueToothBody);
        }
    else if (messageType == ConvergedMessage::MmsNotification)
        {
        QString subject = index.data(Subject).toString();
        int priority = index.data(MessagePriority).toInt();
        mConversation->setPriority(priority);
        mConversation->setSubject(subject);
        mConversation->setBodyText(bodyText);
        }
    
    //repolish widget
    mConversation->repolishWidget();
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
// MsgConversationViewItem::setMessageStateIcon
// @see header file
//---------------------------------------------------------------
void MsgConversationViewItem::setMessageStateIcon(int messageState)
{
    HbIconAnimator& iconAnimator = mMessageStateIconItem->animator();
    HbIconAnimationManager* iconAnimationManager = HbIconAnimationManager::global();
    switch (messageState)
    {
        case ConvergedMessage::Waiting:
        case ConvergedMessage::Scheduled:
        case ConvergedMessage::Sending:
        { 
 	        bool defined = iconAnimationManager->addDefinitionFile(ANIMATION_FILE);
     	    HbIcon animIcon;
            animIcon.setIconName(ANIMATION_ICON_NAME);
            QSizeF size = mMessageStateIconItem->size();
            mMessageStateIconItem->setIcon(animIcon);
            mMessageStateIconItem->setVisible(true);
            iconAnimator.startAnimation();
            repolish();
            break;
        }     
        case ConvergedMessage::Suspended:
        {
           iconAnimator.stopAnimation();
           mMessageStateIconItem->setIcon(MSG_OUTBOX_ICON);
           mMessageStateIconItem->setVisible(true);
           break;
        
        }
        case ConvergedMessage::Resend:
        {
            iconAnimator.stopAnimation();
            mMessageStateIconItem->setIcon(MSG_OUTBOX_ICON);
            mMessageStateIconItem->setVisible(true);
            break;
        }
        case ConvergedMessage::Failed:
        {
            iconAnimator.stopAnimation();
            mMessageStateIconItem->setIcon(MSG_FAIL_ICON);
            mMessageStateIconItem->setVisible(true);
            break;
        }   
        case ConvergedMessage::Unknown:
        default:
        {
            iconAnimator.stopAnimation();
            mMessageStateIconItem->setVisible(false);
            break;
        }
    }
}

//---------------------------------------------------------------
// MsgConversationViewItem::setNotificationStateIcon
// @see header file
//---------------------------------------------------------------

void MsgConversationViewItem::setNotificationStateIcon(int notificationState)
{
    HbIconAnimator& iconAnimator = mMessageStateIconItem->animator();
    HbIconAnimationManager* iconAnimationManager = HbIconAnimationManager::global();
    switch (notificationState)
    {
        case ConvergedMessage::NotifRetrieving:
        case ConvergedMessage::NotifWaiting:
        { 
            //TODO: Temp icon until official icons are received
            bool defined = iconAnimationManager->addDefinitionFile(ANIMATION_FILE);
            HbIcon animIcon;
            animIcon.setIconName(ANIMATION_ICON_NAME);
            QSizeF size = mMessageStateIconItem->size();
            mMessageStateIconItem->setIcon(animIcon);
            mMessageStateIconItem->setVisible(true);
            iconAnimator.startAnimation();
            repolish();
            break;
        }     
        default:
        {
            iconAnimator.stopAnimation();
            mMessageStateIconItem->setVisible(false);
            break;
        }
    }
}

//---------------------------------------------------------------
// MsgConversationViewItem::pressStateChanged
// @see header file
//---------------------------------------------------------------
void MsgConversationViewItem::pressStateChanged (bool pressed, bool animate)
{
    mConversation->pressStateChanged(pressed, animate);
}

// EOF
