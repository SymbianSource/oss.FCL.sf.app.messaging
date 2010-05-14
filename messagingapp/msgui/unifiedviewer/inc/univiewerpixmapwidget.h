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
     * @param pixmapPath File path of the pixmap.
     */
    void setPixmap(const QString &pixmapPath);

signals:

    /**
     * Signal emitted for short tap on pixmap.
     * @param pixmapPath File path of the pixmap being clicked.
     */
    void shortTap(const QString &pixmapPath);

protected:

    /**
     * Event handler for gesture events.
     * Reimplemented from HbWidgetBase.
     * @see HbWidgetBase
     */
    virtual void gestureEvent(QGestureEvent *event);

private:

    /**
     * Pixmap file path being set.
     */
    QString mPixmapFile;
};

#endif /* UNI_VIEWER_PIXMAP_WIDGET_H */

// EOF
