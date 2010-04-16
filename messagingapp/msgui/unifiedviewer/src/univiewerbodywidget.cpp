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
 * Description: This widget displays the body of the viewer
 *
 */

#include "univiewerbodywidget.h"

#include <QFile>
#include <QFileInfo>
#include <QPixmap>
#include <QGraphicsLayout>

#include <HbTextItem>
#include <HbIconItem>
#include <HbPushButton>
#include <HbAction>
#include <HbMenu>
#include <HbMainWindow>
#include <HbInstance>
#include <HbGesture>
#include <HbGestureSceneFilter>

#include "debugtraces.h"
#include "univiewertextitem.h"

// LOCAL CONSTANTS
const QString AUDIO_ICON("qtg_mono_audio.svg");

//---------------------------------------------------------------
//UniViewerBodyWidget::UniViewerBodyWidget
// @see header file
//---------------------------------------------------------------
UniViewerBodyWidget::UniViewerBodyWidget(QGraphicsItem *parent) :
    HbWidget(parent), mHasText(false), mTextItem(0), mIconItem(0), mAudioItem(0)
{
    //This is permanent item in the widget
    mTextItem = new UniViewerTextItem(this);
    HbStyle::setItemName(mTextItem, "textItem");

    //Gesture filter for the image
    gestureFilter = new HbGestureSceneFilter(Qt::LeftButton, this);
    gestureFilter->setLongpressAnimation(true);
    HbGesture *gesture = new HbGesture(HbGesture::longpress, 20);
    gestureFilter->addGesture(gesture);
    connect(gesture, SIGNAL(longPress(QPointF)), this, SLOT(longPressed(QPointF)));
    
    connect(mTextItem,SIGNAL(sendMessage(const QString&)),
            this, SIGNAL(sendMessage(const QString&)));
}

//---------------------------------------------------------------
//UniViewerBodyWidget::~UniViewerBodyWidget
// @see header file
//---------------------------------------------------------------
UniViewerBodyWidget::~UniViewerBodyWidget()
{
}

//---------------------------------------------------------------
//UniViewerBodyWidget::setImage
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::setImage(QString imagefile)
{
    //create image item instance
    if (!mIconItem) {
        mIconItem = new HbIconItem(this);
        HbStyle::setItemName(mIconItem, "pixmap");
    }

    QPixmap pixmap(imagefile);
    mIconItem->setIcon(HbIcon(pixmap));
    this->repolish();
}

//---------------------------------------------------------------
//UniViewerBodyWidget::setAudio
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::setAudio(QString audiofile)
{
    if (!mAudioItem) {
        mAudioItem = new HbPushButton(this);
        HbStyle::setItemName(mAudioItem, "audioItem");
    }
    QFileInfo fileInfo(audiofile);
    mAudioItem->setIcon(HbIcon(AUDIO_ICON));
    mAudioItem->setText(fileInfo.baseName());
    mAudioItem->setAdditionalText("00:00");
    mAudioItem->setTextAlignment(Qt::AlignLeft);
    this->repolish();
}

//---------------------------------------------------------------
//UniViewerBodyWidget::setVideo
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::setVideo(QString videofile)
{
    Q_UNUSED(videofile)
}

//---------------------------------------------------------------
//UniViewerBodyWidget::resizeEvent
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::resizeEvent(QGraphicsSceneResizeEvent* event)
{
    Q_UNUSED(event)

    HbMainWindow *mainWindow = this->mainWindow();
    if (mainWindow) {
        qreal screenWidth = 0.0;
        qreal screenHeight = 0.0;
        qreal leftMargin = 0.0;
        qreal rightMargin = 0.0;
        qreal chromeHeight = 0.0;
        qreal toolbarHeight = 0.0;
        qreal iconSize = 0.0;
        style()->parameter("hb-param-screen-width", screenWidth);
        style()->parameter("hb-param-screen-height", screenHeight);
        style()->parameter("hb-param-margin-gene-left", leftMargin);
        style()->parameter("hb-param-margin-gene-right", rightMargin);
        style()->parameter("hb-param-widget-chrome-height", chromeHeight);
        style()->parameter("hb-param-widget-toolbar-height", toolbarHeight);
        style()->parameter("hb-param-graphic-size-primary-large", iconSize);

        qreal maxWidth = 0.0;
        qreal maxHeight = 0.0;

        if (mainWindow->orientation() == Qt::Horizontal) {
            if (mIconItem) {
                if (mHasText) {
                    maxWidth = (screenWidth / 2) - leftMargin
                        - HbDeviceProfile::profile(mIconItem).unitValue();
                }
                else {
                    maxWidth = screenWidth - leftMargin - rightMargin;
                }
                maxHeight = screenHeight - chromeHeight - toolbarHeight;
            }
            if (mAudioItem) {
                mAudioItem->setStretched(true);
            }
        }
        else if (mainWindow->orientation() == Qt::Vertical) {
            if (mIconItem) {
                maxWidth = screenWidth - leftMargin - rightMargin;
                maxHeight = screenHeight - chromeHeight - toolbarHeight;
            }
            if (mAudioItem) {
                mAudioItem->setStretched(false);
            }
        }

        if (mIconItem) {
            qreal imageWidth = mIconItem->icon().defaultSize().width();
            qreal imageHeight = mIconItem->icon().defaultSize().height();

            qreal widthToSet = 0.0;
            qreal heightToSet = 0.0;

            if (imageWidth < iconSize) {
                widthToSet = iconSize;
                heightToSet = iconSize;
            }
            else if (imageWidth <= maxWidth) {
                // resize not needed
                widthToSet = imageWidth;
                heightToSet = qMin(imageHeight, maxHeight);
            }
            else {
                // resize needed, keep aspect-ratio and resize
                widthToSet = maxWidth;
                heightToSet = maxWidth * (imageHeight / imageWidth);
                heightToSet = qMin(heightToSet, maxHeight);

            }
            if (heightToSet == maxHeight) {
                widthToSet = heightToSet * (imageWidth / imageHeight);
            }

            mIconItem->setMinimumWidth(widthToSet);
            mIconItem->setMinimumHeight(heightToSet);
            mIconItem->setMaximumWidth(widthToSet);
            mIconItem->setMaximumHeight(heightToSet);
        }
    }
}

//---------------------------------------------------------------
//UniViewerBodyWidget::longPressed
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::longPressed(QPointF position)
{
    HbMenu* menu = new HbMenu;
    menu->addAction(tr("Open"), this, SLOT(openMedia()));
    menu->addAction(tr("View details"), this, SLOT(viewDetails()));
    menu->exec(position);
    delete menu;
}

//---------------------------------------------------------------
//UniViewerBodyWidget::openMedia
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::openMedia()
{
    //open corresponding viewer app.
}

//---------------------------------------------------------------
//UniViewerBodyWidget::viewDetails
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::viewDetails()
{
    //open details view.
}

//---------------------------------------------------------------
//UniViewerBodyWidget::setTextContent
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::setTextContent(QString text)
{
    text.replace('\n', QChar::LineSeparator);
    mTextItem->setText(text);
    setHasText(true);
}

//---------------------------------------------------------------
// UniViewerBodyWidget::hasText
// @see header file
//---------------------------------------------------------------
bool UniViewerBodyWidget::hasText()
{
    return mHasText;
}

//---------------------------------------------------------------
// UniViewerBodyWidget::setHasText
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::setHasText(bool text)
{
    mHasText = text;
}

//---------------------------------------------------------------
// UniViewerBodyWidget::setSlideContents
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::setSlideContents(UniMessageInfoList objList, QString slideString)
{
    Q_UNUSED(slideString)

    int count = objList.count();

    for (int a = 0; a < count; ++a) {
        UniMessageInfo* info = objList.at(a);
        QString type = info->mimetype();

        if (type.contains("video")) {
            setVideo(info->path());
        }
        else if (type.contains("audio")) {
            setAudio(info->path());
        }
        if (type.contains("image")) {
            setImage(info->path());
        }
        else if (type.contains("text")) {
            QFile file(info->path());
            file.open(QIODevice::ReadOnly);
            QString textContent(file.readAll());
            setTextContent(textContent);
        }
        delete info;
    }
}

//---------------------------------------------------------------
//UniViewerBodyWidget :: clearContent
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::clearContent()
{
    // delete the temp items(pixmap) & clear permanent items(text)
    if (mIconItem) {
        mIconItem->setParent(NULL);
        delete mIconItem;
        mIconItem = NULL;
    }

    if (mAudioItem) {
        mAudioItem->setParent(NULL);
        delete mAudioItem;
        mAudioItem = NULL;
    }

    mTextItem->setText(QString());
    setHasText(false);
}

// EOF
