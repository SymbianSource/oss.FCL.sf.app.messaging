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
#include <QSignalMapper>

#include <HbTextItem>
#include <HbIconItem>
#include <HbPushButton>
#include <HbAction>
#include <HbMenu>
#include <HbMainWindow>
#include <HbInstance>
#include <HbGesture>
#include <HbGestureSceneFilter>

#include <xqaiwrequest.h>
#include <xqrequestinfo.h>
#include <xqappmgr.h>

#include "univiewertextitem.h"
#include "msgmediautil.h"
// LOCAL CONSTANTS
const QString AUDIO_ICON("qtg_mono_audio");

// Localization
#define LOC_TITLE   hbTrId("txt_messaging_title_messaging")

//---------------------------------------------------------------
//UniViewerBodyWidget::UniViewerBodyWidget
// @see header file
//---------------------------------------------------------------
UniViewerBodyWidget::UniViewerBodyWidget(QGraphicsItem *parent) :
    HbWidget(parent), mHasText(false), mHasPixmap(false), mTextItem(0), mSlideCounter(0),
        mIconItem(0), mAudioItem(0)
{
    //Gesture filter for the image
    gestureFilter = new HbGestureSceneFilter(Qt::LeftButton, this);
    gestureFilter->setLongpressAnimation(true);
    HbGesture *gesture = new HbGesture(HbGesture::longpress, 20);
    gestureFilter->addGesture(gesture);
    connect(gesture, SIGNAL(longPress(QPointF)), this, SLOT(longPressed(QPointF)));
    // Signal mapper for opening media files
    mSignalMapper = new QSignalMapper(this);
    connect(mSignalMapper, SIGNAL(mapped(const QString &)), this, SLOT(openMedia(const QString &)));
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
    setHasPixmap(true);
    //create image item instance
    if (!mIconItem) {
        mIconItem = new HbIconItem(this);
        HbStyle::setItemName(mIconItem, "pixmap");
    }

    QPixmap pixmap(imagefile);
    mIconItem->setIcon(HbIcon(pixmap));

    // TODO
    // Implementation for short tap action is unclear
    // Connect to signal mapper

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
    MsgMediaUtil mediaUtil;
    mAudioItem->setAdditionalText(mediaUtil.mediaDuration(audiofile));
    mAudioItem->setTextAlignment(Qt::AlignLeft);

    // Connect to signal mapper with file name
    mSignalMapper->setMapping(mAudioItem, audiofile);
    connect(mAudioItem, SIGNAL(clicked()), mSignalMapper, SLOT(map()));

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
//UniViewerBodyWidget::setTextContent
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::setTextContent(QString text)
{
    setHasText(true);

    if (!mTextItem) {
        mTextItem = new UniViewerTextItem(this);
        HbStyle::setItemName(mTextItem, "textItem");
        connect(mTextItem, SIGNAL(sendMessage(const QString&)), this,
            SIGNAL(sendMessage(const QString&)));
    }
    text.replace(QChar::ParagraphSeparator, QChar::LineSeparator);
    text.replace('\r', QChar::LineSeparator);
    mTextItem->setText(text);
    this->repolish();
}

//---------------------------------------------------------------
//UniViewerBodyWidget::setTextContent
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::setSlideCounter(QString &slideCounter)
{
    if (!mSlideCounter) {
        mSlideCounter = new HbTextItem(this);
        HbStyle::setItemName(mSlideCounter, "slideCounter");
    }

    mSlideCounter->setText(slideCounter);
    this->repolish();
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
// UniViewerBodyWidget::hasText
// @see header file
//---------------------------------------------------------------
bool UniViewerBodyWidget::hasText()
{
    return mHasText;
}

//---------------------------------------------------------------
// UniViewerBodyWidget::setHasPixmap
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::setHasPixmap(bool pixmap)
{
    mHasPixmap = pixmap;
}

//---------------------------------------------------------------
// UniViewerBodyWidget::hasPixmap
// @see header file
//---------------------------------------------------------------
bool UniViewerBodyWidget::hasPixmap()
{
    return mHasPixmap;
}

//---------------------------------------------------------------
// UniViewerBodyWidget::setSlideContents
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::setSlideContents(UniMessageInfoList objList, QString slideString)
{
    if (!slideString.isEmpty()) {
        setSlideCounter(slideString);
    }

    int count = objList.count();
    for (int a = 0; a < count; ++a) {
        UniMessageInfo* info = objList.at(a);
        QString type = info->mimetype();

        if (type.contains("text")) {
            QFile file(info->path());
            if (file.open(QIODevice::ReadOnly))
			{
			QString textContent(file.readAll());
            setTextContent(textContent);
			}
            
        }
        else if (type.contains("video")) {
            setVideo(info->path());
        }
        else if (type.contains("audio")) {
            setAudio(info->path());
        }
        else if (type.contains("image")) {
            setImage(info->path());
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

    if (mTextItem) {
        mTextItem->setParent(NULL);
        delete mTextItem;
        mTextItem = NULL;
    }

    setHasText(false);
    setHasPixmap(false);
    repolish();
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
        qreal unitSize = HbDeviceProfile::profile(mIconItem).unitValue();
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
            qreal temp;
            temp = screenWidth;
            screenWidth = screenHeight;
            screenHeight = temp;
            if (mIconItem) {
                if (mHasText) {
                    maxWidth = (screenWidth / 2) - leftMargin - unitSize;
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

            mIconItem->setPreferredWidth(widthToSet);
            mIconItem->setPreferredHeight(heightToSet);
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
}

//---------------------------------------------------------------
//UniViewerBodyWidget::openMedia
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::openMedia(const QString& fileName)
{
    XQSharableFile sf;
    XQAiwRequest* request = 0;

    if (!sf.open(fileName)) {
        return;
    }

    // Get handlers
    XQApplicationManager appManager;
    QList<XQAiwInterfaceDescriptor> fileHandlers = appManager.list(sf);
    if (fileHandlers.count() > 0) {
        XQAiwInterfaceDescriptor d = fileHandlers.first();
        request = appManager.create(sf, d);

        if (!request) {
            sf.close();
            return;
        }
    }
    else {
        sf.close();
        return;
    }

    // Result handlers
    connect(request, SIGNAL(requestOk(const QVariant&)), this, SLOT(handleOk(const QVariant&)));
    connect(request, SIGNAL(requestError(const QVariant&)), this,
        SLOT(handleError(const QVariant&)));

    request->setEmbedded(true);
    request->setSynchronous(true);

    // Fill args
    QList<QVariant> args;
    args << qVariantFromValue(sf);
    request->setArguments(args);

    // Fill headers
    QString key("WindowTitle");
    QVariant value(QString(LOC_TITLE));
    XQRequestInfo info;
    info.setInfo(key, value);
    request->setInfo(info);

    request->send();

    // Cleanup
    sf.close();
    delete request;
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
// UniViewerBodyWidget :: handleOk
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::handleOk(const QVariant& result)
{
    Q_UNUSED(result)
}

//---------------------------------------------------------------
// UniViewerBodyWidget :: handleError
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::handleError(int errorCode, const QString& errorMessage)
{
    Q_UNUSED(errorMessage)
    Q_UNUSED(errorCode)
}
// EOF
