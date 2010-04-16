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
#include <hbframeitem.h>
#include <hbframedrawer.h>
#include <HbTextItem>
#include <HbFrameItem>

#include "msgcommondefines.h"
#include "conversationsengine.h"
#include "conversationsenginedefines.h"
#include "msgutils.h"
#include "convergedmessage.h"
#include "debugtraces.h"

#define NEW_ITEM_FRAME ":/newitem/qtg_fr_list_new_item"
#define LOC_RINGING_TONE hbTrId("txt_messaging_dpopinfo_ringing_tone")

const QString LIST_ITEM_BG_FRAME("qtg_fr_list_normal");

//---------------------------------------------------------------
// MsgListViewItem::MsgListViewItem
// Constructor
//---------------------------------------------------------------
MsgListViewItem::MsgListViewItem(QGraphicsItem* parent) :
    HbListViewItem(parent),
    mUnReadMsg(false),
    mNewMsgIndicatorItem(NULL),
    mBgFrameItem(NULL),
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
    if (!mBgFrameItem)
    {
        mBgFrameItem = new HbFrameItem(this);
        mBgFrameItem->setZValue(-1.0);
        HbStyle::setItemName(mBgFrameItem, "bgFrame");
        
        mBgFrameItem->frameDrawer().setFrameGraphicsName(LIST_ITEM_BG_FRAME);
        mBgFrameItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
    }

    // Time Stamp.
    QDateTime dateTime;
    dateTime.setTime_t(modelIndex().data(TimeStamp).toUInt());
    QString dateString;
    if (dateTime.date() == QDateTime::currentDateTime().date())
    {
        dateString = MsgUtils::dateTimeToString(dateTime, TIME_FORMAT);
    }
    else
    {
        dateString = MsgUtils::dateTimeToString(dateTime, DATE_FORMAT);
    }

    mTimestampItem->setText(dateString);

    //Sender
    QString firstName = modelIndex().data(FirstName).toString();
    QString lastName = modelIndex().data(LastName).toString();
    QString nickName = modelIndex().data(NickName).toString();
    QString contactAddress = modelIndex().data(ConversationAddress).toString();
    QString contactName;

    if (!nickName.isEmpty())
    {
        contactName.append(nickName);
    }
    else if (firstName.isEmpty() && lastName.isEmpty())
    {
        contactName.append(contactAddress);
    }
    else if (lastName.isEmpty())
    {
        contactName.append(firstName);
    }
    else if (firstName.isEmpty())
    {
        contactName.append(lastName);
    }
    else
    {
        // If both first Name and last name are present
        contactName.append(firstName);
        contactName.append(" ");
        contactName.append(lastName);
    }
    mAddressLabelItem->setText(contactName);

    // Latest Message
    int messageType = modelIndex().data(MessageType).toInt();
    QString previewText;
    // Set the message text depending upon the message type
    if (messageType == ConvergedMessage::Mms)
    {
        QDEBUG_WRITE("MsgListViewItem::updateChildItems Mms block")
        previewText = modelIndex().data(Subject).toString();
    }
    else if(messageType == ConvergedMessage::RingingTone)
    {
        QDEBUG_WRITE("MsgListViewItem::updateChildItems RingingTone block")
        previewText = LOC_RINGING_TONE;
    }
	else if (messageType == ConvergedMessage::VCard)
    {
        previewText = QString("Business Card");
    }
    else if (messageType == ConvergedMessage::VCal)
    {
        previewText = QString("Calender Event");
    }
    else if(messageType == ConvergedMessage::BioMsg)
    {
        previewText = QString("Unsupported Type");
    }
    else
    {
        QDEBUG_WRITE("MsgListViewItem::updateChildItems default block")
        previewText = modelIndex().data(BodyText).toString();
    }
    mPreviewLabelItem->setText(previewText);

    // Unread message count
    int unreadCount = modelIndex().data(UnreadCount).toInt();
    QString unRead;
    if (unreadCount > 0)
    {
        unRead.append(tr("(%n)", "", unreadCount));
        mUnreadCountItem->setText(unRead);
        mUnReadMsg = true;
        mNewMsgIndicatorItem->frameDrawer().setFrameGraphicsName(NEW_ITEM_FRAME);
    }
    else
    {
        mUnreadCountItem->setText("");
        mNewMsgIndicatorItem->frameDrawer().setFrameGraphicsName(QString());
        mUnReadMsg = false;
        repolish();
    }

    HbListViewItem::updateChildItems();
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
