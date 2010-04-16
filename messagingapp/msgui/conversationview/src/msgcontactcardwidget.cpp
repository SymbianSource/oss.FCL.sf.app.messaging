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

#include "msgcontactcardwidget.h"

// SYSTEM INCLUDES
#include <HbStyle>
#include <HbIconItem>
#include <HbTextItem>
#include <HbPushButton>
#include <HbFrameDrawer>

// USER INCLUDES
#include "conversationsenginedefines.h"
#include "debugtraces.h"
#include "conversationsengine.h"

// LOCAL CONSTANTS
const QString DEFAULT_AVATAR_ICON(":/icons/qtg_large_avatar.svg");
const QString ONLINE_ICON(":/icons/qtg_small_online.svg");
const QString OFFLINE_ICON(":/icons/qtg_small_offline.svg");
const QString BACKGROUND_FRAME("qtg_fr_list_normal");
const QString PLUGINPATH("conversationviewplugin.dll");

//---------------------------------------------------------------
// MsgContactCardWidget::MsgContactCardWidget
// @see header
//---------------------------------------------------------------
MsgContactCardWidget::MsgContactCardWidget(QGraphicsItem *parent) :
    HbAbstractButton(parent), mAvatarIconItem(NULL),
    mPresenceIconItem(NULL),
    mAddressTextItem(NULL)
{
    int baseId = style()->registerPlugin(PLUGINPATH);
    QDEBUG_WRITE_FORMAT("MsgContactCardWidget BASE ID --->", baseId);
    setPluginBaseId(baseId);

    init();
}

//---------------------------------------------------------------
// MsgContactCardWidget::~MsgContactCardWidget
// @see header
//---------------------------------------------------------------
MsgContactCardWidget::~MsgContactCardWidget()
{
    style()->unregisterPlugin(PLUGINPATH);
}

//---------------------------------------------------------------
// MsgContactCardWidget::init
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::init()
{
    mAvatarIconItem = new HbIconItem(this);
    HbStyle::setItemName(mAvatarIconItem, "avatar");

    mPresenceIconItem = new HbIconItem(this);
    HbStyle::setItemName(mPresenceIconItem, "presence");

    mAddressTextItem = new HbTextItem(this);
    HbStyle::setItemName(mAddressTextItem, "addressText");

    mBackgroundItem = new HbPushButton(this);
    HbStyle::setItemName(mBackgroundItem, "backgroundFrame");

    HbFrameDrawer *frameDrawer = new HbFrameDrawer(BACKGROUND_FRAME,
        HbFrameDrawer::NinePieces);

    mBackgroundItem->setFrameBackground(frameDrawer);

    connect(mBackgroundItem, SIGNAL(clicked(bool)),
        this, SIGNAL(clicked()));
}

//---------------------------------------------------------------
// MsgContactCardWidget::setAvatar
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::setAvatar(const HbIcon &avatar)
{
    mAvatarIconItem->setIcon(avatar);
}

//---------------------------------------------------------------
// MsgContactCardWidget::setPresenceIcon
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::setPresenceIcon(const HbIcon &presenceIcon)
{
    mPresenceIconItem->setIcon(presenceIcon);
}

//---------------------------------------------------------------
// MsgContactCardWidget::setAddress
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::setAddress(const QString &address)
{
    mAddress = address;
    mAddressTextItem->setText(address);
}

//---------------------------------------------------------------
// MsgContactCardWidget::address
// @see header
//---------------------------------------------------------------
ConvergedMessageAddressList MsgContactCardWidget::address()
{
    ConvergedMessageAddressList addresses;
    QModelIndex index = 
            ConversationsEngine::instance()->getConversationsModel()->index(0, 0);
    ConvergedMessageAddress* address = new ConvergedMessageAddress(index.data(
        ConversationAddress).toString());
    address->setAlias(mAddress);
    addresses.append(address);
    return addresses;
}

//---------------------------------------------------------------
// MsgContactCardWidget::updateContents
// @see header file
//---------------------------------------------------------------
void MsgContactCardWidget::updateContents()
{
    // Get the Model Index
    QModelIndex index = 
                ConversationsEngine::instance()->getConversationsModel()->index(0, 0);
        
    //Set the Contact Name/Number
    qint64 convId ;
    QString firstName;
    QString lastName;
    QString contactAddress;
    convId = ConversationsEngine::instance()->getCurrentConversationId();
    ConversationsEngine::instance()->getContactDetails(convId,firstName,
        lastName,contactAddress);    
    //QString contactAddress = index.data(ConversationAddress).toString();
    QString nickName;// = index.data(NickName).toString(); TODO
    QString contactName;
    if (!nickName.isEmpty())
    {
        contactName.append(nickName);
    }
    else if (firstName.isEmpty() && lastName.isEmpty())
    {
        contactName.append(contactAddress);
    }
    else if (lastName.isEmpty() && !firstName.isEmpty())
    {
        contactName.append(firstName);
    }
    else if (firstName.isEmpty() && !lastName.isEmpty())
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
    setAddress(contactName);

    QByteArray avatarByteArray = index.data(Avatar).toByteArray();

    HbIcon avatar;
    if (avatarByteArray.isEmpty())
    {
        avatar = HbIcon(DEFAULT_AVATAR_ICON);
    }
    else
    {
        QPixmap pixmap;
        pixmap.loadFromData(avatarByteArray);
        avatar = HbIcon(QIcon(pixmap));
    }
    setAvatar(avatar);
    
}

//---------------------------------------------------------------
// MsgContactCardWidget::clearContent
// @see header file
//---------------------------------------------------------------
void MsgContactCardWidget::clearContent()
{
	  setAddress("");
	  setAvatar(HbIcon());
}
// EOF
