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
 * Description:  Item decorator of the message list view.
 *
 */

// SYSTEM INCLUDES
#include "msglistviewitem.h"

#include <QDateTime>
#include <QFileInfo>
#include <hbframeitem.h>
#include <hbframedrawer.h>
#include <HbTextItem>
#include <HbFrameItem>
#include <HbIconItem>

#include "msgcommondefines.h"
#include "conversationsengine.h"
#include "conversationsenginedefines.h"
#include "msgutils.h"
#include "convergedmessage.h"
#include "debugtraces.h"

#define LOC_RINGING_TONE hbTrId("txt_messaging_dpopinfo_ringing_tone")
#define LOC_MSG_SEND_FAILED hbTrId("txt_messaging_list_message_sending_failed")
#define LOC_MSG_OUTGOING hbTrId("txt_messaging_list_outgoing_message")
#define LOC_MSG_RESEND_AT hbTrId("Resend at ")
#define LOC_BUSINESS_CARD hbTrId("txt_messaging_list_business_card")
#define LOC_CALENDAR_EVENT hbTrId("txt_messaging_list_calendar_event")
#define LOC_UNSUPPORTED_MSG_TYPE hbTrId("txt_messaging_list_unsupported_message_type")
#define LOC_RECEIVED_FILES hbTrId("txt_messaging_list_received_files")

const QString NEW_ITEM_FRAME("qtg_fr_list_new_item");
const QString BT_ICON("qtg_large_bluetooth");
const QString MSG_OUTGOING_ICON("qtg_mono_outbox");
const QString MSG_FAILED_ICON("qtg_mono_failed");

//---------------------------------------------------------------
// MsgListViewItem::MsgListViewItem
// Constructor
//---------------------------------------------------------------
MsgListViewItem::MsgListViewItem(QGraphicsItem* parent) :
    HbListViewItem(parent),
    mUnReadMsg(false),
    mNewMsgIndicatorItem(NULL),
    mAddressLabelItem(NULL),
    mTimestampItem(NULL),
    mPreviewLabelItem(NULL),
    mUnreadCountItem(NULL),
    mPresenceIndicatorItem(NULL)
{
}

//---------------------------------------------------------------
// HbAbstractViewItem::createItem
// @see header
//---------------------------------------------------------------
HbAbstractViewItem* MsgListViewItem::createItem()
{
    return new MsgListViewItem(*this);
}

//---------------------------------------------------------------
// MsgListViewItem::updateChildItems
// @see header
//---------------------------------------------------------------
void MsgListViewItem::updateChildItems()
{
    if (!mAddressLabelItem)
    {
        mAddressLabelItem = new HbTextItem(this);
        HbStyle::setItemName(mAddressLabelItem, "addressLabel");
    }
    if (!mTimestampItem)
    {
        mTimestampItem = new HbTextItem(this);
        HbStyle::setItemName(mTimestampItem, "timeLabel");
    }
    if (!mPreviewLabelItem)
    {
        mPreviewLabelItem = new HbTextItem(this);
        mPreviewLabelItem->setTextWrapping(Hb::TextWordWrap);
        HbStyle::setItemName(mPreviewLabelItem, "previewLabel");
    }
    if (!mUnreadCountItem)
    {
        mUnreadCountItem = new HbTextItem(this);
        HbStyle::setItemName(mUnreadCountItem, "unreadCount");
    }
    if (!mNewMsgIndicatorItem)
    {
        mNewMsgIndicatorItem = new HbFrameItem(this);
        HbStyle::setItemName(mNewMsgIndicatorItem, "newMsgIndicator");

        mNewMsgIndicatorItem->frameDrawer().setFrameType(
            HbFrameDrawer::ThreePiecesVertical);
    }

    // Set the message text depending upon the message type
    int messageType = modelIndex().data(MessageType).toInt();
    int messageSubType = modelIndex().data(MessageSubType).toInt();
    int messageState = modelIndex().data(SendingState).toInt();
    int messageDirection = modelIndex().data(Direction).toInt();
    QString prevText;
    prevText = previewText(messageType, messageSubType, messageState,messageDirection);
    prevText.replace(QChar::ParagraphSeparator, QChar::LineSeparator);
    prevText.replace('\r', QChar::LineSeparator);
    mPreviewLabelItem->setText(prevText);

    // Address label
    QString contactName;
    if(messageType == ConvergedMessage::BT)
    {
        contactName = LOC_RECEIVED_FILES;
    }
    else
    {
        QString displayName = modelIndex().data(DisplayName).toString();
        QString contactAddress = modelIndex().data(ConversationAddress).toString();

        if (displayName.isEmpty())
        {
            contactName.append(contactAddress);
        }
        else
        {
            contactName.append(displayName);
        }        
    }
    mAddressLabelItem->setText(contactName);

    // Unread message count
    int unreadCount = modelIndex().data(UnreadCount).toInt();

    if (unreadCount > 0)
    {
        QString unRead(tr("(%n)", "", unreadCount));
        mUnreadCountItem->setText(unRead);
        mUnReadMsg = true;
        mNewMsgIndicatorItem->frameDrawer().setFrameGraphicsName(NEW_ITEM_FRAME);
    }
    else
    {
        mUnreadCountItem->setText(QString());
        mNewMsgIndicatorItem->frameDrawer().setFrameGraphicsName(QString());
        mUnReadMsg = false;
        repolish();
    }

    HbListViewItem::updateChildItems();
}

//---------------------------------------------------------------
// MsgListViewItem::previewText
// @see header
//---------------------------------------------------------------
QString MsgListViewItem::previewText(int msgType, 
    int msgSubType, 
    int msgState,
    int msgDirection)
{
    QString bodyText(modelIndex().data(BodyText).toString());
    QString previewText;
    // Set the message text & icons, depending upon the message type
    if (msgType == ConvergedMessage::BioMsg) {
        if (msgSubType == ConvergedMessage::RingingTone) {
            previewText = LOC_RINGING_TONE;
        }
        else if (msgSubType == ConvergedMessage::VCard) {
            previewText = LOC_BUSINESS_CARD;
            previewText.append(QChar::LineSeparator);
            previewText.append(bodyText);
        }
        else if (msgSubType == ConvergedMessage::Provisioning) {
                previewText = bodyText;
                }
        else {
            previewText = LOC_UNSUPPORTED_MSG_TYPE;
        }
    }
    else if (msgType == ConvergedMessage::BT) {
        
        if (msgSubType == ConvergedMessage::VCard) {
            previewText = LOC_BUSINESS_CARD;
            previewText.append(QChar::LineSeparator);
            previewText.append(bodyText);
        }
        else {
            previewText = bodyText;
        }
        if (!mPresenceIndicatorItem) {
            mPresenceIndicatorItem = new HbIconItem(this);
            HbStyle::setItemName(mPresenceIndicatorItem, "presenceIndicator");
            mPresenceIndicatorItem->setIconName(BT_ICON);
        }
    }
    else {
        previewText = textBySendState(msgState,msgDirection);
    }
    return previewText;
}


//---------------------------------------------------------------
// MsgListViewItem::textBySendState
// @see header
//---------------------------------------------------------------
QString MsgListViewItem::textBySendState(int sendState,int msgDirection)
{
    QString previewText;
    QDateTime dateTime;
    dateTime.setTime_t(modelIndex().data(TimeStamp).toUInt());
    QString dateString;
    if (dateTime.date() == QDateTime::currentDateTime().date()) {
        dateString = MsgUtils::dateTimeToString(dateTime, TIME_FORMAT);
    }
    else {
        dateString = MsgUtils::dateTimeToString(dateTime, DATE_FORMAT);
    }

    if (msgDirection == ConvergedMessage::Outgoing)
    {
        switch (sendState)
        {
            case ConvergedMessage::Resend:
            {

                QString resendString = LOC_MSG_RESEND_AT + dateString;
                previewText = resendString;
                dateString = QString("");
                if (!mPresenceIndicatorItem)
                {
                    mPresenceIndicatorItem = new HbIconItem(this);
                }
                HbStyle::setItemName(mPresenceIndicatorItem,
                    "presenceIndicator");
                mPresenceIndicatorItem->setIconName(MSG_OUTGOING_ICON);
                mPresenceIndicatorItem->setVisible(true);
                break;
            }

            case ConvergedMessage::Sending:
            case ConvergedMessage::Suspended:
            case ConvergedMessage::Scheduled:
            case ConvergedMessage::Waiting:
            {
                previewText = QString(LOC_MSG_OUTGOING);
                if (!mPresenceIndicatorItem)
                {
                    mPresenceIndicatorItem = new HbIconItem(this);
                }
                HbStyle::setItemName(mPresenceIndicatorItem,
                    "presenceIndicator");
                mPresenceIndicatorItem->setIconName(MSG_OUTGOING_ICON);
                mPresenceIndicatorItem->setVisible(true);
                break;
            }
            case ConvergedMessage::Failed:
            {
                previewText = QString(LOC_MSG_SEND_FAILED);
                if (!mPresenceIndicatorItem)
                {
                    mPresenceIndicatorItem = new HbIconItem(this);
                }
                HbStyle::setItemName(mPresenceIndicatorItem,
                    "presenceIndicator");
                mPresenceIndicatorItem->setIconName(MSG_FAILED_ICON);
                mPresenceIndicatorItem->setVisible(true);
                break;
            }
            default:
                QString postFix = QString("");
                previewText = modelIndex().data(BodyText).toString();
                previewText += (QChar(QChar::LineSeparator) + postFix);

                if (!mPresenceIndicatorItem)
                {
                    mPresenceIndicatorItem = new HbIconItem(this);
                }

                HbStyle::setItemName(mPresenceIndicatorItem,
                    "presenceIndicator");
                mPresenceIndicatorItem->setIconName(MSG_FAILED_ICON);
                mPresenceIndicatorItem->setVisible(false);

                break;
        }
    }
    else
    {
        previewText = modelIndex().data(BodyText).toString();
    }
    mTimestampItem->setText(dateString);
    return previewText;
}

//---------------------------------------------------------------
// MsgListViewItem::paint.
// @see header
//---------------------------------------------------------------
/*void MsgListViewItem::paint(QPainter *painter,
 const QStyleOptionGraphicsItem *option,
 QWidget *widget)
 {

 }
 */

//---------------------------------------------------------------
// MsgListViewItem::setWidgetContent
// @see header file
//---------------------------------------------------------------
void MsgListViewItem::setHasUnReadMsg(bool unread)
{
    mUnReadMsg = unread;
}

//---------------------------------------------------------------
// MsgListViewItem::setWidgetContent
// @see header file
//---------------------------------------------------------------
bool MsgListViewItem::hasUnReadMsg()
{
    return mUnReadMsg;
}

//EOF
