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
 * Description: This widget displays the pixmap content in viewer.
 *
 */

#ifndef UNI_VIEWER_PIXMAP_WIDGET_H
#define UNI_VIEWER_PIXMAP_WIDGET_H

#include <HbIconItem>

// FORWARD DECLARATIONS
class UniViewerUtils;

/**
 * This widget displays the pixmap content in viewer.
 */
class UniViewerPixmapWidget: public HbIconItem
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    UniViewerPixmapWidget(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~UniViewerPixmapWidget();

    /**
     * Sets the pixmap content to be displayed.
     * @param mimeType Mime type of the pixmap.
     * @param pixmapPath File path of the pixmap.
     */
    void populate(const QString &mimeType, const QString &pixmapPath);

signals:

    /**
     * Signal emitted when widget is clicked.
     * @param mediaPath File path of the media.
     */
    void shortTap(const QString &mediaPath);

    /**
     * Signal emitted when widget is long tapped.
     * @param position Scene coordinates of tap.
     */
    void longTap(const QPointF &position);

protected:

    /**
     * Event handler for gesture events.
     * Reimplemented from HbWidgetBase.
     * @see HbWidgetBase
     */
    virtual void gestureEvent(QGestureEvent *event);

private slots:

    /**
     *
     */
    void handleOpen();

    /**
     *
     */
    void handleSave();
    
    /**
     * Slot to regrab gesture after some delay (300 ms) to avoid multiple gesture
     * events back to back.  
     */
    void regrabGesture();

private:

    /**
     * Handles short tap event.
     */
    void handleShortTap();

    /**
     * Handles long tap event.
     * @param position Scene coordinates of tap.
     */
    void handleLongTap(const QPointF &position);

private:

    /**
     * UniViewerUtils object.
     * Own
     */
    UniViewerUtils *mViewerUtils;

    /**
     * Mime Type of pixmap.
     */
    QString mMimeType;

    /**
     * Pixmap file path being set.
     */
    QString mPixmapPath;
};

#endif /* UNI_VIEWER_PIXMAP_WIDGET_H */

// EOF
