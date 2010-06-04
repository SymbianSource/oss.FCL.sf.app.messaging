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

// INCLUDES
#include "debugtraces.h"
#include <HbTextItem>
#include <HbIconItem>
#include <QFileInfo>
#include <QFont>
#include <HbFrameDrawer>
#include <HbMenu>
#include <MsgMimeTypes.h>
#include <HbFrameItem>
//#include <HbGestureSceneFilter>
//#include <HbGesture>
#include <QGraphicsSceneMouseEvent>
#include <HbWidgetFeedback>

// USER INCLUDES
#include "msgunieditorattachment.h"
#include "unieditorgenutils.h"
#include "s60qconversions.h"

// Constants
#define BYTES_TO_KBYTES_FACTOR 1024
#define BG_FRAME "qtg_fr_groupbox"

//Localized Constants for item specific menu
#define LOC_OPEN    hbTrId("txt_common_menu_open")
#define LOC_REMOVE  hbTrId("txt_common_menu_remove")
#define LOC_DETAILS hbTrId("txt_common_menu_details")

const QString LIST_ITEM_BG_FRAME_NORMAL ("qtg_fr_list_normal");
const QString LIST_ITEM_BG_FRAME_PRESSED("qtg_fr_list_pressed");

const QString ATTACHMENT_ICON("qtg_small_attachment");

MsgUnifiedEditorAttachment::MsgUnifiedEditorAttachment( const QString& attachmentpath,
                                                        const int filesize,
                                                        QGraphicsItem *parent ) :
HbWidget(parent),
mPath(attachmentpath),
mSize(filesize),
mAttachmentIcon(0),
mAttachmentName(0),
//mGestureFilter(0),
mMaxSmsSize(KFirstNormalSmsLength)
{
        //back ground
        HbFrameItem* backGround = new HbFrameItem(this);
        backGround->frameDrawer().setFrameGraphicsName(LIST_ITEM_BG_FRAME_NORMAL);
        backGround->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
        this->setBackgroundItem(backGround);        

        mAttachmentIcon = new HbIconItem(ATTACHMENT_ICON, this);
        HbStyle::setItemName(mAttachmentIcon,"attachmentIcon");

        int at_size = 0;
        TMsgMediaType mediaType = EMsgMediaUnknown;
        UniEditorGenUtils* genUtils = new UniEditorGenUtils;
        TRAP_IGNORE(genUtils->getFileInfoL(mPath,at_size,
                                           mMimeType,mediaType));
        TRAP_IGNORE(mMaxSmsSize = genUtils->MaxSmsMsgSizeL()); 
        delete genUtils;
        QFileInfo fileinfo(attachmentpath);
        QString filename = fileinfo.fileName();
        mAttachmentName = new HbTextItem(filename,this);
        HbStyle::setItemName(mAttachmentName,"attachmentName");
        mAttachmentName->setElideMode(Qt::ElideRight);
        
        // for sms, pure size should be shown
        // for mms, additional mimeheader size must be included
        qreal displaySize = mSize;
        if(!isMultimediaContent())
        {
            displaySize = fileinfo.size();
        }
        int sizeInKb = displaySize/BYTES_TO_KBYTES_FACTOR;
        QString fileDetails;
        // if size exceeds 1kb, then show kb or else only bytes
        if(sizeInKb >= 1)
        {
            fileDetails = QString().append(QString("(%1 Kb)").arg(sizeInKb));
        }
        else
        {
            fileDetails = QString().append(QString("(%1 B)").arg(displaySize));
        }

        mAttachmentDetails = new HbTextItem(fileDetails, this);
        HbStyle::setItemName(mAttachmentDetails,"attachmentDetails");
        mAttachmentDetails->setElideMode(Qt::ElideNone);
        
        initGesture();
}

MsgUnifiedEditorAttachment::~MsgUnifiedEditorAttachment()
{
  /*  if(mGestureFilter)
        {
        removeSceneEventFilter(mGestureFilter);
        }*/
}

const QString& MsgUnifiedEditorAttachment::path()
{
    return mPath;
}

qreal MsgUnifiedEditorAttachment::size()
{
    return mSize;
}

const QString& MsgUnifiedEditorAttachment::mimeType()
{
    return mMimeType;
}

void MsgUnifiedEditorAttachment::longPressed(QPointF position)
{
    HbMenu* menu = new HbMenu;
    menu->addAction(LOC_OPEN, this, SLOT(openAttachment()));
    menu->addAction(LOC_REMOVE, this, SLOT(removeAttachment()));
    menu->addAction(LOC_DETAILS, this, SLOT(viewDetails()));
    menu->setDismissPolicy(HbPopup::TapAnywhere);
    menu->setAttribute(Qt::WA_DeleteOnClose, true);
    menu->setPreferredPos(position);
    menu->show();
}

void MsgUnifiedEditorAttachment::removeAttachment()
{
    emit deleteMe(this);
}

void MsgUnifiedEditorAttachment::openAttachment()
{
    //open corresponding viewer app.
}

void MsgUnifiedEditorAttachment::viewDetails()
{
    //open details view.
}

bool MsgUnifiedEditorAttachment::isMultimediaContent()
{
    bool ret = true;
    QString vcard = S60QConversions::s60Desc8ToQString(KMsgMimeVCard());
    QString vcal = S60QConversions::s60Desc8ToQString(KMsgMimeVCal());
    QString ical = S60QConversions::s60Desc8ToQString(KMsgMimeICal());
    if( !QString::compare(mMimeType, vcard, Qt::CaseInsensitive) ||
        !QString::compare(mMimeType, vcal, Qt::CaseInsensitive) ||
        !QString::compare(mMimeType, ical, Qt::CaseInsensitive) )
    {
        QFileInfo fileinfo(mPath);
        int fSize = fileinfo.size();
        
        // if filesize is within sms size-limit, then
        // it is not mm content, else it is mm attachment
        if(fSize <= mMaxSmsSize)
        {
            ret = false;
        }
    }
    return ret;
}

void MsgUnifiedEditorAttachment::mousePressEvent(QGraphicsSceneMouseEvent *event)
{    
    HbWidgetFeedback::triggered(this, Hb::InstantPressed);
    
    HbFrameItem* backGround = new HbFrameItem(this);
    backGround->frameDrawer().setFrameGraphicsName(LIST_ITEM_BG_FRAME_PRESSED);
    backGround->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
    this->setBackgroundItem(backGround); 
    
    event->accept();
}

void MsgUnifiedEditorAttachment::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    HbFrameItem* backGround = new HbFrameItem(this);
    backGround->frameDrawer().setFrameGraphicsName(LIST_ITEM_BG_FRAME_NORMAL);
    backGround->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
    this->setBackgroundItem(backGround);
    
    if(this->rect().contains(event->pos()))
        {
        HbWidgetFeedback::triggered(this, Hb::InstantClicked);
        emit clicked();
        }
    
    event->accept();    
}

void MsgUnifiedEditorAttachment::initGesture()
{
    // Create gesture filter
 /*   mGestureFilter = new HbGestureSceneFilter( Qt::LeftButton, this );
    
    // Add gestures for longpress
    HbGesture* gestureLongpressed = new HbGesture( HbGesture::longpress,5 );
    
    mGestureFilter->addGesture( gestureLongpressed );
    
    connect( gestureLongpressed, SIGNAL(longPress(QPointF)),
             this, SLOT(longPressed(QPointF)) );

    //install gesture filter.
    this->installSceneEventFilter(mGestureFilter);*/
}

/*HbFeedback::InstantEffect MsgUnifiedEditorAttachment::overrideFeedback(Hb::InstantInteraction interaction) const
        {
        switch(interaction)
            {
            case Hb::InstantPressed:
            case Hb::InstantClicked:
                return HbFeedback::Basic;
            default:
                return HbFeedback::None;
            }
        }*/

// EOF
