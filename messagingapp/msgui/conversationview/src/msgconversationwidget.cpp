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
#include <HbEffect>
#include <s32strm.h>
#include <s32mem.h>
#include <fbs.h>
#include <sqldb.h>
#include <QBool>
#include <QCoreApplication>
#include <HbEvent>

#include "debugtraces.h"

#include "convergedmessage.h"
#include "conversationsengine.h"

// Icons
const QString MSG_HIGH_PRIORITY_ICON("qtg_small_priority_high");
const QString MSG_LOW_PRIORITY_ICON("qtg_small_priority_low");
const QString MSG_ATTACH_ICON("qtg_small_attachment");
const QString MSG_AUDIO_ICON("qtg_large_music_player");
const QString MSG_VIDEO_ICON("qtg_large_video_player");
const QString MSG_AUDIO_PLAY_ICON("qtg_large_music_player");


// Frames
const QString CV_RECEIVED_NORMAL_FR("qtg_fr_convlist_received_normal");
const QString CV_RECEIVED_PRESSED_FR("qtg_fr_convlist_received_pressed");
const QString CV_RECEIVED_HIGHLIGHT_FR("qtg_fr_convlist_received_highlight");
const QString CV_SENT_NORMAL_FR("qtg_fr_convlist_sent_normal");
const QString CV_SENT_PRESSED_FR("qtg_fr_convlist_sent_pressed");
const QString CV_SENT_HIGHLIGHT_FR("qtg_fr_convlist_sent_highlight");
const QString NEW_ITEM_FRAME("qtg_fr_list_new_item");

//selecet preview-icon query
_LIT(KSelectPreviewIconStmt,"SELECT  message_id, preview_icon FROM conversation_messages WHERE message_id = :message_id ");

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
        mIsMMSNotification(false),
        mPriority(0),
        mSendingState(Unknown),
        mNotificationState(0),
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
    mBubbleFrameItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
    HbStyle::setItemName(mBubbleFrameItem, "bubble");

    mNewFrameItem = new HbFrameItem(this);
    mNewFrameItem->frameDrawer().setFrameType(HbFrameDrawer::ThreePiecesVertical);
    HbStyle::setItemName(mNewFrameItem, "newItemIcon");
    	
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
        mSubjectTextItem->setTextWrapping(Hb::TextWordWrap);      
    }
    HbStyle::setItemName(mSubjectTextItem, "subject");
    mSubjectTextItem->setText(subject);
    mSubjectTextItem->show();
}

//---------------------------------------------------------------
// MsgConversationWidget::setBodyText
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setBodyText(const QString &body)
{
    if (!mBodyTextItem)
    {
        mBodyTextItem = new HbTextItem(this);
        mBodyTextItem->setTextWrapping(Hb::TextWordWrap);
    }
    HbStyle::setItemName(mBodyTextItem, "bodyText");

    mBodyTextItem->setText(body);
    mBodyTextItem->show();
}

//---------------------------------------------------------------
// MsgConversationWidget::setPreviewIconPath
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setPreviewIconPath(const QString &filePath,int msgId)
{
    if (!mPreviewIconItem)
    {
        mPreviewIconItem = new HbIconItem(this);
        mPreviewIconItem->setAlignment(Qt::AlignHCenter | Qt::AlignTop);        
    }

    HbStyle::setItemName(mPreviewIconItem, "preview");
    //sql query to get preview-icon from DB
    TBool isOpen = EFalse;
    bool imagePreviewed = false;

    //get DB handle and check if it is open
    RSqlDatabase& sqlDb = ConversationsEngine::instance()->getDBHandle(isOpen);
    if (isOpen)
    {
        RSqlStatement sqlSelectPreviewIconStmt;
        TInt err = sqlSelectPreviewIconStmt.Prepare(sqlDb,
            KSelectPreviewIconStmt);

        QCRITICAL_WRITE_FORMAT("Error from Prepare()", err)

        if (err == KErrNone)
        {
            //msg_id
            TInt msgIdIndex = sqlSelectPreviewIconStmt.ParameterIndex(
                _L(":message_id"));
            sqlSelectPreviewIconStmt.BindInt(msgIdIndex, msgId);

            // get preview-icon from DB
            err = sqlSelectPreviewIconStmt.Next();
            QCRITICAL_WRITE_FORMAT("Error from Next()", err)

            if (err == KSqlAtRow)
            {
                TInt previewIconIndex = sqlSelectPreviewIconStmt.ColumnIndex(
                    _L("preview_icon"));

                RSqlColumnReadStream stream;

                //Get data from binary column BLOB
                err = stream.ColumnBinary(sqlSelectPreviewIconStmt,
                    previewIconIndex);

                QCRITICAL_WRITE_FORMAT("Error from ColumnBinary()", err)

                if (err == KErrNone)
                {
                    CFbsBitmap *bitmap = new CFbsBitmap;
                    TRAPD(err,bitmap->InternalizeL(stream));
                    QCRITICAL_WRITE_FORMAT("Error from bitmap InternalizeL()", err)

                    //convert bitmap to pixmap
                    if (err == KErrNone)
                    {
                        TSize size = bitmap->SizeInPixels();
                        int bytesPerLine = bitmap->ScanLineLength(size.iWidth,
                            bitmap->DisplayMode());
                        const uchar* dataPtr =
                                (const uchar*) bitmap->DataAddress();

                        QPixmap pixmap = QPixmap::fromImage(QImage(dataPtr,
                            size.iWidth, size.iHeight, bytesPerLine,
                            QImage::Format_RGB16));

                        mPreviewIconItem->setIcon(HbIcon(pixmap));
                        mPreviewIconItem->setPreferredSize(pixmap.size());
                        mPreviewIconItem->setEnabled(true);
                        mPreviewIconItem->show();
                        imagePreviewed = true;
                                                
                        QCRITICAL_WRITE("Bitmap Conversion completed")
                    }
					//remove bitmap
                    delete bitmap;                    
                }
                //close stream
                stream.Close();
            }
        }
        sqlSelectPreviewIconStmt.Close();
    }

    // if not found in db, set from file path
    if(!imagePreviewed)
    {
        QPixmap pixmap(filePath);
        QPixmap scaledPixmap =pixmap.scaled(100,100,Qt::IgnoreAspectRatio);
        mPreviewIconItem->setIcon(HbIcon(scaledPixmap));
        mPreviewIconItem->setPreferredSize(scaledPixmap.size());
	    mPreviewIconItem->setEnabled(true);
        mPreviewIconItem->show();
    }
    
    QCRITICAL_WRITE("MsgConversationWidget::setPreviewIconPath end.")  

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
        }
        HbStyle::setItemName(mPriorityIconItem, "priority");
        if (ConvergedMessage::Low == priority)
        {
            mPriorityIconItem->setIcon(HbIcon(MSG_LOW_PRIORITY_ICON));
        }
        else if (ConvergedMessage::High == priority)
        {
            mPriorityIconItem->setIcon(HbIcon(MSG_HIGH_PRIORITY_ICON));
        }
        mPriorityIconItem->show();
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
        if (!mAttachmentIconItem)
        {
            mAttachmentIconItem = new HbIconItem(this);
        }
        HbStyle::setItemName(mAttachmentIconItem, "attachment");
        mAttachmentIconItem->setIcon(HbIcon(MSG_ATTACH_ICON));
        mAttachmentIconItem->show();
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
void MsgConversationWidget::displayAudioIcon(const QString &iconPath)
{
    if (hasAudio())
    {
        if (hasImage())
        {
            if (!mPlayIconItem)
            {
                mPlayIconItem = new HbIconItem(this);                
            }
            HbStyle::setItemName(mPlayIconItem, "playIcon");
            mPlayIconItem->setIconName(iconPath.isEmpty() ? MSG_AUDIO_PLAY_ICON : iconPath);
            mPlayIconItem->show();
        }
        else
        {
            if (!mAudioIconItem)
            {
                mAudioIconItem = new HbIconItem(this);    
            }
            HbStyle::setItemName(mAudioIconItem, "audioIcon");
            mAudioIconItem->setIconName(iconPath.isEmpty() ? MSG_AUDIO_PLAY_ICON : iconPath);
            mAudioIconItem->show();
           
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
        }
        HbStyle::setItemName(mVideoIconItem, "video");
        mVideoIconItem->setIcon(HbIcon(MSG_VIDEO_ICON));
        mVideoIconItem->show();
        
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
// MsgConversationWidget::setMMSNotification
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setMMSNotification(bool isMMSNotification)
{
    mIsMMSNotification = isMMSNotification;
}

//---------------------------------------------------------------
// MsgConversationWidget::isMMS
// @see header file
//---------------------------------------------------------------
bool MsgConversationWidget::isMMSNotification()
{
    return mIsMMSNotification;
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
MsgConversationWidget::SendingState MsgConversationWidget::sendingState()
{
    return mSendingState;
}

//---------------------------------------------------------------
// MsgConversationWidget::setNotificationState
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setNotificationState(int state)
{
    switch (state)
    {
        case ConvergedMessage::NotifFailed:
        {
            mNotificationState = NotifFailed;
            break;
        }
        case ConvergedMessage::NotifReadyForFetching:
        {
            mNotificationState = NotifReadyForFetching;
            break;
        }
        case ConvergedMessage::NotifRetrieving:
        case ConvergedMessage::NotifWaiting:
        {
            mNotificationState = NotifRetrieving;
            break;
        }
        case ConvergedMessage::NotifExpired:
        {
            mNotificationState = NotifExpired;
            break;
        }
        default:
        {
            mNotificationState = NotifUnknown;
            break;
        }
    }
}

//---------------------------------------------------------------
// MsgConversationWidget::notificationState
// @see header file
//---------------------------------------------------------------
int MsgConversationWidget::notificationState()
{
    return mNotificationState;
}

//---------------------------------------------------------------
// MsgConversationWidget::setTimeStamp
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::setTimeStamp(const QString &timeStamp)
{
    if(!mTimeStampTextItem)
    {
	  mTimeStampTextItem = new HbTextItem(this);
	  }
    HbStyle::setItemName(mTimeStampTextItem, "timeStamp");

    mTimeStampTextItem->setText(timeStamp);
    mTimeStampTextItem->show();
 
}

//---------------------------------------------------------------
// MsgConversationWidget::drawNewItemFrame
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::drawNewItemFrame()
{
    if (mUnread)
    {
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
    if(isIncoming())
    {
        if (mIsMMSNotification && (mNotificationState == NotifUnknown
                || mNotificationState == NotifRetrieving ))
        {
            mBubbleFrameItem->frameDrawer().setFrameGraphicsName(
                CV_RECEIVED_HIGHLIGHT_FR);
            //Inactive state bubble  
        }
        else
        {
            mBubbleFrameItem->frameDrawer().setFrameGraphicsName(CV_RECEIVED_NORMAL_FR);
        }
    }
    else
    {
        if(mSendingState == Sending || mSendingState == Pending || 
           mSendingState == Unknown || mSendingState == Failed)
        {
            mBubbleFrameItem->frameDrawer().setFrameGraphicsName(CV_SENT_HIGHLIGHT_FR);
        }
        else
        {
            mBubbleFrameItem->frameDrawer().setFrameGraphicsName(CV_SENT_NORMAL_FR);
        }
    }
}

//---------------------------------------------------------------
// MsgConversationWidget::drawPressedBubbleFrame
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::drawPressedBubbleFrame()
{
    if(isIncoming())
    {
        mBubbleFrameItem->frameDrawer().setFrameGraphicsName(CV_RECEIVED_PRESSED_FR);
    }
    else
    {
        mBubbleFrameItem->frameDrawer().setFrameGraphicsName(CV_SENT_PRESSED_FR);
    }
}

//---------------------------------------------------------------
// MsgConversationWidget::pressStateChanged
// @see header file
//---------------------------------------------------------------
void MsgConversationWidget::pressStateChanged(bool pressed, bool animate)
{
    Q_UNUSED(animate)

    if (mBubbleFrameItem) {
        (pressed) ? drawPressedBubbleFrame() : drawBubbleFrame();
    }
}

    void MsgConversationWidget::resetProperties()
    	{
        mHasAttachment = false;
        mHasImage = false;
        mHasAudio = false;
        mHasVideo = false;
        mIsPlayable =false;
        mIncoming =false;
        mUnread =false;
        mIsMMS = false;
        mIsMMSNotification= false;
        mPriority = 0;
        mSendingState =Unknown;
        mNotificationState =0;
        
		if(mBodyTextItem){
			  mBodyTextItem->setText(QString());
        mBodyTextItem->hide();
        HbStyle::setItemName(mBodyTextItem, "");
	    }
    
	    if(mSubjectTextItem){
	    	  mSubjectTextItem->setText(QString());
	        mSubjectTextItem->hide();
	        HbStyle::setItemName(mSubjectTextItem, "");
	    }
    
	    if(mTimeStampTextItem){
	    	  mTimeStampTextItem->setText(QString());
	        mTimeStampTextItem->hide();
	        HbStyle::setItemName(mTimeStampTextItem, "");        
	    }   
    
	    if(mAttachmentIconItem){
	        mAttachmentIconItem->hide();
	        HbStyle::setItemName(mAttachmentIconItem, "");
	    }
    
	   if(mPriorityIconItem){
	       HbStyle::setItemName(mPriorityIconItem, "");
	        mPriorityIconItem->hide();
	    }
   
	    if(mAudioIconItem){
	        HbStyle::setItemName(mAudioIconItem, "");
	        mAudioIconItem->hide();              
	       }
    
	    if(mVideoIconItem){
	        HbStyle::setItemName(mVideoIconItem, "");
	        mVideoIconItem->hide();
	      }
    
	    if(mPlayIconItem){
	        HbStyle::setItemName(mPlayIconItem, "");
	        mPlayIconItem->hide();
	       }
    
	    if(mPreviewIconItem){
	        HbStyle::setItemName(mPreviewIconItem, "");
	        mPreviewIconItem->hide();               
	       }    
    	}
    
    void MsgConversationWidget::repolishWidget()
    	{
    	QCoreApplication::postEvent(this, new HbEvent(HbEvent::ThemeChanged));	
    	repolish();
    	}
    
// EOF
