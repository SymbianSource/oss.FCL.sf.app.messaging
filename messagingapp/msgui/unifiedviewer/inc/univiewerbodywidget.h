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

class UniViewerTextItem;
class HbIconItem;
class HbPushButton;
class HbGestureSceneFilter;

#include "unidatamodelplugininterface.h"

/**
 * This widget displays the body of the viewer
 */
class UniViewerBodyWidget : public HbWidget
{
Q_OBJECT

    Q_PROPERTY(bool hasText READ hasText WRITE setHasText)

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
     * @param medialist list of absolute paths of media.
     */
    void setImage(QString imagefile);

    /**
     * Called to insert audio content in viewer.
     * @param medialist list of absolute paths of media.
     */
    void setAudio(QString audiofile);

    /**
     * Called to insert video content in viewer.
     * @param medialist list of absolute paths of media.
     */
    void setVideo(QString videofile);

    /**
     * Called to insert text content in viewer.
     * @param
     */
    void setTextContent(QString text);

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
    // from HbWidget

    /**
     * reimplemented from base class to provide proper geometry for scrolling.
     * @see HbWidget
     */
    void resizeEvent(QGraphicsSceneResizeEvent* event);

private slots:

    /**
     * called on long press on the media objects
     */
    void longPressed(QPointF position);

    /**
     * called from the media object's item specific menu
     */
    void openMedia();

    /**
     * called from the media object's item specific menu
     */
    void viewDetails();
    
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
     * editor to in put text.
     */
    UniViewerTextItem* mTextItem;

    /**
     * icon item to preview images.
     */
    HbIconItem* mIconItem;

    /**
     * Media widget for embedded audio content.
     */
    HbPushButton *mAudioItem;

    /**
     * To setup longpress gesture on media objects
     */
    HbGestureSceneFilter* gestureFilter;

};

#endif //UNIVIEWER_BODY_WIDGET_H
// EOF
