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

#ifndef UNIVIEWER_BODY_WIDGET_H
#define UNIVIEWER_BODY_WIDGET_H

#include <hbwidget.h>

#include "unidatamodelplugininterface.h"

class UniViewerTextItem;
class UniViewerPixmapWidget;
class UniViewerAudioWidget;
class HbTextItem;

/**
 * @class UniViewerBodyWidget
 * @brief This widget displays the body of the viewer.
 */
class UniViewerBodyWidget: public HbWidget
{
Q_OBJECT

Q_PROPERTY(bool hasText READ hasText WRITE setHasText)
Q_PROPERTY(bool hasPixmap READ hasPixmap WRITE setHasPixmap)

public:

    /**
     * Constructor
     */
    explicit UniViewerBodyWidget(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~UniViewerBodyWidget();

public slots:

    /**
     * Called to insert image content in viewer.
     * @param info Object information.
     */
    void setPixmap(UniMessageInfo *info);

    /**
     * Called to insert audio content in viewer.
     * @param info Object information.
     */
    void setAudio(UniMessageInfo *info);

    /**
     * Called to insert video content in viewer.
     * @param info Object information.
     */
    void setVideo(UniMessageInfo *info);

    /**
     * Called to insert text content in viewer.
     * @param text Body text to be set.
     */
    void setText(QString text);

    /**
     * Sets the slide counter.
     * @param slideCounter Slide counter string to be set.
     */
    void setSlideCounter(QString &slideCounter);

    /**
     * Specify if this slide has text
     * @param bool
     */
    void setHasText(bool text = true);

    /**
     * Get to find if slide has text file
     * @return bool
     */
    bool hasText();

    /**
     * Specify if this slide has pixmap
     * @param bool
     */
    void setHasPixmap(bool text = true);

    /**
     * Returns true if body has pixmap
     * @return bool
     */
    bool hasPixmap();

    /**
     * Sets the side content
     * @param objList object list
     * @param slideString slide string
     */
    void setSlideContents(UniMessageInfoList objList, QString slideString);

    /**
     * Clears the content on the widget
     */
    void clearContent();

protected:

    /**
     * Reimplemented from base class to provide proper geometry for scrolling.
     * @see HbWidget
     */
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF & constraint = QSizeF()) const;

signals:
    /**
     * this signal is emitted when sendMessage is emitted from UniViewerTextItem.
     */
    void sendMessage(const QString& phoneNumber);

private:
    /**
     * Info if slide has text.
     * true if slide has text else false.
     */
    bool mHasText;

    /**
     * Info if slide has pixmap.
     * true if slide has pixmap else false.
     */
    bool mHasPixmap;

    /**
     * Editor to in put text.
     */
    UniViewerTextItem* mTextItem;

    /**
     * Slide counter.
     */
    HbTextItem *mSlideCounter;

    /**
     * Icon item to preview images.
     */
    UniViewerPixmapWidget *mPixmapItem;

    /**
     * Media widget for embedded audio content.
     */
    UniViewerAudioWidget *mAudioItem;
};

#endif //UNIVIEWER_BODY_WIDGET_H
// EOF
