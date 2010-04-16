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
#include "msgconversationwidget.h"

#include <HbTextItem>
#include <HbFrameItem>
#include <HbFrameDrawer>
#include <HbIconItem>
#include <QPainter>
#include <QPixmap>
#include <QImageReader>
#include <QDir>
#include "debugtraces.h"

#include "convergedmessage.h"

#define DEBUG_PAINT 0

const int MAX_SIZE(200);

// Icons
const QString MSG_FORWARD_ICON(":/icons/qtg_mono_forward.svg");

const QString MSG_HIGH_PRIORITY_ICON(":/icons/qtg_small_priority_high.svg");
const QString MSG_LOW_PRIORITY_ICON(":/icons/qtg_small_priority_low.svg");

const QString MSG_ATTACH_ICON(":/icons/qtg_small_attachment.svg");

const QString MSG_AUDIO_ICON(":/icons/qtg_large_music_player.png");
const QString MSG_VIDEO_ICON(":/icons/video_icon.png");
const QString MSG_AUDIO_PLAY_ICON(":/icons/qtg_large_music_player.png");


// Frames
const QString IN_CHAT_FRAME(":/bubble/qtg_fr_convlist_received_normal");
const QString IN_CHAT_FRAME_PRESSED(":/bubble/qtg_fr_convlist_received_pressed");
const QString OUT_CHAT_FRAME(":/bubble/qtg_fr_convlist_sent_normal");
const QString OUT_CHAT_FRAME_PRESSED(":/bubble/qtg_fr_convlist_sent_pressed");
const QString IN_CHAT_FRAME_UNREAD(":/bubble/qtg_fr_convlist_received_highlight");
const QString NEW_ITEM_FRAME(":/new_item/qtg_fr_list_new_item");
const QString SENDING_CHAT_FRAME(":/bubble/qtg_fr_convlist_sent_highlight");

#define PLUGINPATH "conversationviewplugin.dll"

//---------------------------------------------------------------
// MsgConversationWidget::MsgConversationWidget
// @see header file
//---------------------------------------------------------------
MsgConversationWidget::MsgConversationWidget(QGraphicsItem *parent) :
        HbWidget(parent),
        mHasAttachment(false),
        mHasImage(false),
        mHasAudio(false),
        mHasVideo(false),
        mIsPlayable(false),
        mIncoming(false),
        mUnread(false),
        mIsMMS(false),
        mPriority(0),
        mSendingState(0),
        mNewFrameItem(NULL),
        mBubbleFrameItem(NULL),
        mSubjectTextItem(NULL),
        mBodyTextItem(NULL),
        mTimeStampTextItem(NULL),
        mPriorityIconItem(NULL),
        mAttachmentIconItem(NULL),
        mAudioIconItem(NULL),
        mVideoIconItem(NULL),
        mPlayIconItem(NULL),
        mPreviewIconItem(NULL)


{
    init();
}

//---------------------------------------------------------------
// MsgConversationWidget::~MsgConversationWidget
// @see header file
//---------------------------------------------------------------
MsgConversationWidget::~MsgConversationWidget()
{

}

//---------------------------------------------------------------
// MsgConversationWidget::init
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::init()
{
    // Common to SMS/MMS

    mBubbleFrameItem = new HbFrameItem(this);
    // ZValue is set to make the bubble to be rendered in behind text items.
    mBubbleFrameItem->setZValue(-1.0);
    HbStyle::setItemName(mBubbleFrameItem, "bubble");

    mBodyTextItem = new HbTextItem(this);
    mBodyTextItem->setTextWrapping(Hb::TextWordWrap);
    HbStyle::setItemName(mBodyTextItem, "bodyText");

    mTimeStampTextItem = new HbTextItem(this);
    HbStyle::setItemName(mTimeStampTextItem, "timeStamp");

    mNewFrameItem = new HbFrameItem(this);
    HbStyle::setItemName(mNewFrameItem, "newItemIcon");

    mAttachmentIconItem = new HbIconItem(this);
    HbStyle::setItemName(mAttachmentIconItem, "attachment");
}

//---------------------------------------------------------------
// MsgConversationWidget::setSubject
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setSubject(const QString &subject)
{
    if (!mSubjectTextItem)
    {
        mSubjectTextItem = new HbTextItem(this);
        HbStyle::setItemName(mSubjectTextItem, "subject");
    }
    mSubjectTextItem->setText(subject);
}

//---------------------------------------------------------------
// MsgConversationWidget::setBodyText
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setBodyText(const QString &body)
{
    mBodyTextItem->setText(body);
}

//---------------------------------------------------------------
// MsgConversationWidget::setPreviewIconPath
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setPreviewIconPath(const QString &previewPath)
{
    if (!mPreviewIconItem)
    {
        mPreviewIconItem = new HbIconItem(this);
        mPreviewIconItem->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        HbStyle::setItemName(mPreviewIconItem, "preview");
    }

    if (previewPath.isEmpty())
    {
        mPreviewIconItem->setEnabled(false);
    }
    else
    {
        QImageReader iReader(previewPath);
        QSize size(iReader.size());
        
        if(size.height() > MAX_SIZE || size.width() > MAX_SIZE)
        {
        size.scale(MAX_SIZE,MAX_SIZE,Qt::KeepAspectRatio);
        iReader.setScaledSize(size);

        QImage icon = iReader.read();
        QPixmap pixmap = QPixmap::fromImage(icon);
        
        mPreviewIconItem->setIcon(HbIcon(pixmap));
        }
        else
        {
            QPixmap pixmap(previewPath);
            mPreviewIconItem->setIcon(HbIcon(pixmap));
        }
        
        mPreviewIconItem->setEnabled(true);
    }
}

//---------------------------------------------------------------
// MsgConversationWidget::setPriority
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setPriority(int priority)
{
    mPriority = priority;

    if (priority)
    {
        if (!mPriorityIconItem)
        {
            mPriorityIconItem = new HbIconItem(this);
            HbStyle::setItemName(mPriorityIconItem, "priority");
        }

        if (ConvergedMessage::Low == priority)
        {
            mPriorityIconItem->setIcon(HbIcon(MSG_LOW_PRIORITY_ICON));
        }
        else if (ConvergedMessage::High == priority)
        {
            mPriorityIconItem->setIcon(HbIcon(MSG_HIGH_PRIORITY_ICON));
        }
    }
}

//---------------------------------------------------------------
// MsgConversationWidget::priority
// @see header file
//---------------------------------------------------------------
int MsgConversationWidget::priority()
{
    return mPriority;
}

//---------------------------------------------------------------
// MsgConversationWidget::setAttachment
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setAttachment(bool attachment)
{
    mHasAttachment = attachment;

    if (attachment)
    {
        mAttachmentIconItem->setIcon(HbIcon(MSG_ATTACH_ICON));
    }
}

//---------------------------------------------------------------
// MsgConversationWidget::hasAttachment
// @see header file
//---------------------------------------------------------------
bool MsgConversationWidget::hasAttachment()
{
    return mHasAttachment;
}

//---------------------------------------------------------------
// MsgConversationWidget::setImage
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setImage(bool image)
{
    mHasImage = image;
}

//---------------------------------------------------------------
// MsgConversationWidget::hasImage
// @see header file
//---------------------------------------------------------------
bool MsgConversationWidget::hasImage()
{
    return mHasImage;
}

//---------------------------------------------------------------
// MsgConversationWidget::setAudio
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setAudio(bool audio)
{
    mHasAudio = audio;
}

//---------------------------------------------------------------
// MsgConversationWidget::hasAudio
// @see header file
//---------------------------------------------------------------
bool MsgConversationWidget::hasAudio()
{
    return mHasAudio;
}

//---------------------------------------------------------------
// MsgConversationWidget::displayAudioIcon
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::displayAudioIcon()
{
    if (hasAudio())
    {
        if (hasImage())
        {
            if (!mPlayIconItem)
            {
                mPlayIconItem = new HbIconItem(this);
                HbStyle::setItemName(mPlayIconItem, "playIcon");
            }
            mPlayIconItem->setIconName(MSG_AUDIO_PLAY_ICON);
        }
        else
        {
            if (!mAudioIconItem)
            {
                mAudioIconItem = new HbIconItem(this);
                HbStyle::setItemName(mAudioIconItem, "audioIcon");
            }
            mAudioIconItem->setIconName(MSG_AUDIO_ICON);
        }
    }
}

//---------------------------------------------------------------
// MsgConversationWidget::setVideo
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setVideo(bool video)
{
    mHasVideo = video;

    if (video)
    {
        if (!mVideoIconItem)
        {
            mVideoIconItem = new HbIconItem(this);
            HbStyle::setItemName(mVideoIconItem, "video");
        }
        mVideoIconItem->setIcon(HbIcon(MSG_VIDEO_ICON));
    }
}

//---------------------------------------------------------------
// MsgConversationWidget::hasVideo
// @see header file
//---------------------------------------------------------------
bool MsgConversationWidget::hasVideo()
{
    return mHasVideo;
}

//---------------------------------------------------------------
// MsgConversationWidget::setPlayable
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setPlayable(bool playable)
{
    mIsPlayable = playable;
}

//---------------------------------------------------------------
// MsgConversationWidget::isPlayable
// @see header file
//---------------------------------------------------------------
bool MsgConversationWidget::isPlayable()
{
    return mIsPlayable;
}

//---------------------------------------------------------------
// MsgConversationWidget::setIncoming
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setIncoming(bool incoming)
{
    mIncoming = incoming;
}

//---------------------------------------------------------------
// MsgConversationWidget::isIncoming
// @see header file
//---------------------------------------------------------------
bool MsgConversationWidget::isIncoming()
{
    return mIncoming;
}

//---------------------------------------------------------------
// MsgConversationWidget::setUnread
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setUnread(bool unread)
{
    mUnread = unread;
}

//---------------------------------------------------------------
// MsgConversationWidget::isUnread
// @see header file
//---------------------------------------------------------------
bool MsgConversationWidget::isUnread()
{
    return mUnread;
}

//---------------------------------------------------------------
// MsgConversationWidget::setMMS
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setMMS(bool isMMS)
{
    mIsMMS = isMMS;
}

//---------------------------------------------------------------
// MsgConversationWidget::isMMS
// @see header file
//---------------------------------------------------------------
bool MsgConversationWidget::isMMS()
{
    return mIsMMS;
}

//---------------------------------------------------------------
// MsgConversationWidget::setSendingState
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setSendingState(int state)
{
    switch (state)
    {
        case ConvergedMessage::SentState:
        {
            mSendingState = Sent;
            repolish();
            break;
        }
        case ConvergedMessage::Sending:
        {
            mSendingState = Sending;
            break;
        }
        case ConvergedMessage::Suspended:
        case ConvergedMessage::Resend:
        {
            mSendingState = Pending;
            break;
        }
        case ConvergedMessage::Failed:
        {
            mSendingState = Failed;
            break;
        }
        default:
        {
            mSendingState = Unknown;
            break;
        }
    }
}

//---------------------------------------------------------------
// MsgConversationWidget::sendingState
// @see header file
//---------------------------------------------------------------
int MsgConversationWidget::sendingState()
{
    return mSendingState;
}

//---------------------------------------------------------------
// MsgConversationWidget::setTimeStamp
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setTimeStamp(const QString &timeStamp)
{
    mTimeStampTextItem->setText(timeStamp);
}

//---------------------------------------------------------------
// MsgConversationWidget::drawNewItemFrame
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::drawNewItemFrame()
{
    if (mUnread)
    {
        mNewFrameItem->frameDrawer().setFrameType(HbFrameDrawer::ThreePiecesVertical);
        mNewFrameItem->frameDrawer().setFrameGraphicsName(NEW_ITEM_FRAME);
    }
    else
    {
        mNewFrameItem->frameDrawer().setFrameGraphicsName(QString());
    }
}

//---------------------------------------------------------------
// MsgConversationWidget::drawBubbleFrame
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::drawBubbleFrame()
{
    mBubbleFrameItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
    mBubbleFrameItem->frameDrawer().setFillWholeRect(true);

    if(isIncoming())
    {
        mBubbleFrameItem->frameDrawer().setFrameGraphicsName(IN_CHAT_FRAME);
    }
    else
    {
        if(mSendingState == Sending || mSendingState == Pending || mSendingState == Unknown)
        {
            mBubbleFrameItem->frameDrawer().setFrameGraphicsName(SENDING_CHAT_FRAME);
        }
        else
        {
            mBubbleFrameItem->frameDrawer().setFrameGraphicsName(OUT_CHAT_FRAME);
        }
    }
}

// EOF
