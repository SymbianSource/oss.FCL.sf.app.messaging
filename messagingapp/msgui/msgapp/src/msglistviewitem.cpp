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
    initItems();

    setTimestampAndPreviewText();

    // Address label
    QString contactName;
    int messageType = modelIndex().data(MessageType).toInt();
    if(messageType == ConvergedMessage::BT)
    {
        contactName = LOC_RECEIVED_FILES;
    }
    else
    {
        QString displayName = modelIndex().data(DisplayName).toString();
        QString contactAddress = modelIndex().data(ConversationAddress).toString();
        contactName.append(displayName);
    }
    mAddressLabelItem->setText(contactName);

    // Unread message count
    int unreadCount = modelIndex().data(UnreadCount).toInt();

    if (unreadCount > 0)
    {
        QString unRead(tr("(%n)", "", unreadCount));
        mUnreadCountItem->setText(unRead);
        if(!mUnReadMsg)
        {
            mUnReadMsg = true;
            mNewMsgIndicatorItem->frameDrawer().setFrameGraphicsName(NEW_ITEM_FRAME);
            repolish();
        }       
    }
    else
    {
        mUnreadCountItem->setText(QString());        
        if(mUnReadMsg)
        {
            mUnReadMsg = false;  
            mNewMsgIndicatorItem->frameDrawer().setFrameGraphicsName(QString());
            repolish();
        }
    }

    HbListViewItem::updateChildItems();
}

//---------------------------------------------------------------
// MsgListViewItem::initItems
// @see header
//---------------------------------------------------------------
void MsgListViewItem::initItems()
{
    if (!mAddressLabelItem) {
        mAddressLabelItem = new HbTextItem(this);
        HbStyle::setItemName(mAddressLabelItem, "addressLabel");
    }
    if (!mTimestampItem) {
        mTimestampItem = new HbTextItem(this);
        HbStyle::setItemName(mTimestampItem, "timeLabel");
    }
    if (!mPreviewLabelItem) {
        mPreviewLabelItem = new HbTextItem(this);
        mPreviewLabelItem->setTextWrapping(Hb::TextWordWrap);
        HbStyle::setItemName(mPreviewLabelItem, "previewLabel");
    }
    if (!mUnreadCountItem) {
        mUnreadCountItem = new HbTextItem(this);
        HbStyle::setItemName(mUnreadCountItem, "unreadCount");
    }
    if (!mNewMsgIndicatorItem) {
        mNewMsgIndicatorItem = new HbFrameItem(this);
        HbStyle::setItemName(mNewMsgIndicatorItem, "newMsgIndicator");

        mNewMsgIndicatorItem->frameDrawer().setFrameType(HbFrameDrawer::ThreePiecesVertical);
    }
    if (!mPresenceIndicatorItem) {
        mPresenceIndicatorItem = new HbIconItem(this);
        HbStyle::setItemName(mPresenceIndicatorItem, "presenceIndicator");
    }
}
//---------------------------------------------------------------
// MsgListViewItem::defaultPreviewText
// @see header
//---------------------------------------------------------------
QString MsgListViewItem::defaultPreviewText(int msgType, int msgSubType)
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
        else if (msgSubType == ConvergedMessage::NokiaService) {
            previewText = bodyText;
        }
        else {
            previewText = LOC_UNSUPPORTED_MSG_TYPE;
        }

        mPresenceIndicatorItem->setVisible(false);
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

        mPresenceIndicatorItem->setIconName(BT_ICON);
        mPresenceIndicatorItem->setVisible(true);
    }
    else {
        // All message types except BIO & BT.
        previewText = modelIndex().data(BodyText).toString();
        mPresenceIndicatorItem->setVisible(false);
    }
    return previewText;
}


//---------------------------------------------------------------
// MsgListViewItem::setTimestampAndPreviewText
// @see header
//---------------------------------------------------------------
void MsgListViewItem::setTimestampAndPreviewText()
{
    // Get timestamp
    QDateTime dateTime;
    dateTime.setTime_t(modelIndex().data(TimeStamp).toUInt());
    QString dateString;
    if (dateTime.date() == QDateTime::currentDateTime().date()) {
        dateString = MsgUtils::dateTimeToString(dateTime, TIME_FORMAT);
    }
    else {
        dateString = MsgUtils::dateTimeToString(dateTime, DATE_FORMAT);
    }

    // Set preview text & time based on direction
    int msgType = modelIndex().data(MessageType).toInt();
    int msgSubType = modelIndex().data(MessageSubType).toInt();
    int sendState = modelIndex().data(SendingState).toInt();
    int msgDirection = modelIndex().data(Direction).toInt();
    QString previewText;
    if (ConvergedMessage::Incoming == msgDirection) {
        previewText = defaultPreviewText(msgType, msgSubType);
    }
    else if (msgDirection == ConvergedMessage::Outgoing) {

        switch (sendState) {
        case ConvergedMessage::Resend:
        {
            previewText = LOC_MSG_RESEND_AT + dateString;
            dateString = QString();
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
            mPresenceIndicatorItem->setIconName(MSG_OUTGOING_ICON);
            mPresenceIndicatorItem->setVisible(true);
            break;
        }
        case ConvergedMessage::Failed:
        {
            previewText = QString(LOC_MSG_SEND_FAILED);
            mPresenceIndicatorItem->setIconName(MSG_FAILED_ICON);
            mPresenceIndicatorItem->setVisible(true);
            break;
        }
        default:
        {
            // Successful case
            previewText = defaultPreviewText(msgType, msgSubType);
            break;
        }
        }
    }
    previewText.replace(QChar::ParagraphSeparator, QChar::LineSeparator);
    previewText.replace('\r', QChar::LineSeparator);
    mPreviewLabelItem->setText(previewText);
    mTimestampItem->setText(dateString);
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
