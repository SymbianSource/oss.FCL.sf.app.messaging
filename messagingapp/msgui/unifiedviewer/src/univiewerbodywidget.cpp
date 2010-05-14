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
#include <QSignalMapper>

#include <HbTextItem>
#include <HbPushButton>
#include <HbMenu>
#include <HbMainWindow>

#include <xqaiwrequest.h>
#include <xqrequestinfo.h>
#include <xqappmgr.h>

#include "univiewertextitem.h"
#include "univiewerpixmapwidget.h"
#include "msgmediautil.h"
// LOCAL CONSTANTS
const QString AUDIO_ICON("qtg_mono_audio");

// Localization
#define LOC_TITLE   hbTrId("txt_messaging_title_messaging")
#define LOC_OPEN    hbTrId("txt_common_menu_open")
#define LOC_SAVE    hbTrId("txt_common_menu_save")

//---------------------------------------------------------------
//UniViewerBodyWidget::UniViewerBodyWidget
// @see header file
//---------------------------------------------------------------
UniViewerBodyWidget::UniViewerBodyWidget(QGraphicsItem *parent) :
    HbWidget(parent), mHasText(false), mHasPixmap(false), mTextItem(0), mSlideCounter(0),
        mPixmapItem(0), mAudioItem(0)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
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
void UniViewerBodyWidget::setPixmap(QString pixmapFile)
{
    setHasPixmap(true);
    //create image item instance
    if (!mPixmapItem) {
        mPixmapItem = new UniViewerPixmapWidget(this);
        HbStyle::setItemName(mPixmapItem, "pixmap");
        connect(mPixmapItem, SIGNAL(shortTap(QString)), this, SLOT(openMedia(QString)));
    }

    mPixmapItem->hide();
    mPixmapItem->setPixmap(pixmapFile);

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
    mAudioItem->hide();
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
    mTextItem->hide();
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

    mSlideCounter->hide();
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
            if (file.open(QIODevice::ReadOnly)) {
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
            setPixmap(info->path());
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
    if (mPixmapItem) {
        mPixmapItem->setParent(NULL);
        delete mPixmapItem;
        mPixmapItem = NULL;
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
//UniViewerBodyWidget::sizeHint
// @see header file
//---------------------------------------------------------------
QSizeF UniViewerBodyWidget::sizeHint(Qt::SizeHint which, const QSizeF & constraint) const
{
    QSizeF szHint = HbWidget::sizeHint(which, constraint);

    HbMainWindow *mainWindow = this->mainWindow();
    if (!mainWindow) {
        return szHint;
    }

    qreal screenWidth = 0.0;
    qreal screenHeight = 0.0;
    qreal leftMargin = 0.0;
    qreal rightMargin = 0.0;
    qreal chromeHeight = 0.0;
    qreal toolbarHeight = 0.0;
    qreal iconSize = 0.0;
    qreal spacing = 0.0;
    qreal unitSize = HbDeviceProfile::profile(mPixmapItem).unitValue();
    style()->parameter("hb-param-screen-width", screenWidth);
    style()->parameter("hb-param-screen-height", screenHeight);
    style()->parameter("hb-param-margin-gene-left", leftMargin);
    style()->parameter("hb-param-margin-gene-right", rightMargin);
    style()->parameter("hb-param-widget-chrome-height", chromeHeight);
    style()->parameter("hb-param-widget-toolbar-height", toolbarHeight);
    style()->parameter("hb-param-graphic-size-primary-large", iconSize);
    style()->parameter("hb-param-margin-gene-middle-vertical", spacing);

    qreal maxWidth = 0.0;
    qreal maxHeight = 0.0;

    // Calculate max height & max width.
    if (mainWindow->orientation() == Qt::Horizontal) {
        qreal temp;
        temp = screenWidth;
        screenWidth = screenHeight;
        screenHeight = temp;

        if (mPixmapItem && mHasText) {
            maxWidth = (screenWidth / 2) - leftMargin - unitSize;
        }
        else {
            maxWidth = screenWidth - leftMargin - rightMargin;
        }
        maxHeight = screenHeight - chromeHeight - toolbarHeight;

        if (mAudioItem) {
            mAudioItem->setStretched(true);
        }
    }
    else if (mainWindow->orientation() == Qt::Vertical) {
        maxWidth = screenWidth - leftMargin - rightMargin;
        maxHeight = screenHeight - chromeHeight - toolbarHeight;

        if (mAudioItem) {
            mAudioItem->setStretched(false);
        }
    }

    // Slide Counter
    QSizeF slideCounterSize(0, 0);
    if (mSlideCounter) {
        slideCounterSize = mSlideCounter->effectiveSizeHint(which, constraint);
        mSlideCounter->show();
    }
    // Audio Item
    QSizeF audioSize(0, 0);
    if (mAudioItem) {
        audioSize = mAudioItem->effectiveSizeHint(which, constraint);
        mAudioItem->show();
    }

    // Text Item
    QSizeF textSize(0, 0);
    if (mTextItem) {
        textSize = mTextItem->effectiveSizeHint(which, constraint);
        mTextItem->show();
    }

    // Pixmap Item
    QSizeF pixmapSize(0, 0);
    if (mPixmapItem) {
        qreal imageWidth = mPixmapItem->icon().defaultSize().width();
        qreal imageHeight = mPixmapItem->icon().defaultSize().height();

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

        pixmapSize.setHeight(heightToSet);
        pixmapSize.setWidth(widthToSet);
        mPixmapItem->setPreferredSize(pixmapSize);
        mPixmapItem->show();
    }

    // Calculate the size hint to be returned.
    szHint.setHeight(0);

    if (!slideCounterSize.isNull()) {
        szHint.rheight() += (slideCounterSize.height() + spacing);
    }
    if (!audioSize.isNull()) {
        szHint.rheight() += (audioSize.height() + spacing);
    }

    if (mainWindow->orientation() == Qt::Horizontal) {
        qreal remainingHeight = qMax(pixmapSize.height(), textSize.height());
        if (remainingHeight > 0.0) {
            szHint.rheight() += (remainingHeight + spacing);
        }
    }
    else if (mainWindow->orientation() == Qt::Vertical) {
        if (!pixmapSize.isNull()) {
            szHint.rheight() += (pixmapSize.height() + spacing);
        }
        if (!textSize.isNull()) {
            szHint.rheight() += (textSize.height() + spacing);
        }
    }
    szHint.rheight() = qMax(maxHeight, szHint.height());

    return szHint;
}

//---------------------------------------------------------------
//UniViewerBodyWidget::longPressed
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::longPressed(QPointF position)
{

    HbMenu* menu = new HbMenu;
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction(LOC_OPEN, this, SLOT(openMedia()));
    menu->addAction(LOC_SAVE, this, SLOT(saveMedia()));
    menu->setPreferredPos(position);
    menu->show();
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
//UniViewerBodyWidget::saveMedia
// @see header file
//---------------------------------------------------------------
void UniViewerBodyWidget::saveMedia()
{
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
