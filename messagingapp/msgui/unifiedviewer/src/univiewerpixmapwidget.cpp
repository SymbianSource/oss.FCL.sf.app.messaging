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

#include "univiewerpixmapwidget.h"

#include <HbTapGesture>
#include <HbWidget>
#include <HbInstantFeedback>
#include <QPixmap>

//---------------------------------------------------------------
// UniViewerPixmapWidget::UniViewerPixmapWidget
// @see header file
//---------------------------------------------------------------
UniViewerPixmapWidget::UniViewerPixmapWidget(QGraphicsItem *parent) :
    HbIconItem(parent)
{
    mPixmapFile = QString("");
    this->grabGesture(Qt::TapGesture);
}

//---------------------------------------------------------------
// UniViewerPixmapWidget::~UniViewerPixmapWidget
// @see header file
//---------------------------------------------------------------
UniViewerPixmapWidget::~UniViewerPixmapWidget()
{
}

//---------------------------------------------------------------
// UniViewerPixmapWidget::setPixmap
// @see header file
//---------------------------------------------------------------
void UniViewerPixmapWidget::setPixmap(const QString &pixmapPath)
{
    mPixmapFile = pixmapPath;
    QPixmap pixmap(pixmapPath);
    this->setIcon(HbIcon(pixmap));
}

//---------------------------------------------------------------
// UniViewerPixmapWidget::gestureEvent
// @see header file
//---------------------------------------------------------------
void UniViewerPixmapWidget::gestureEvent(QGestureEvent *event)
{
    HbTapGesture *tapGesture = qobject_cast<HbTapGesture*> (event->gesture(Qt::TapGesture));
    if (tapGesture) {
        switch (tapGesture->state()) {
        case Qt::GestureStarted:
        {
            // Trigger haptic feedback.
            HbInstantFeedback::play(HbFeedback::Basic);
            break;
        }
        case Qt::GestureUpdated:
        {
            if (HbTapGesture::TapAndHold == tapGesture->tapStyleHint()) {
                // emit longtap
            }
            break;
        }
        case Qt::GestureFinished:
        {
            if (HbTapGesture::Tap == tapGesture->tapStyleHint()) {
                // Emit short tap
                emit shortTap(mPixmapFile);
            }
            break;
        }
        case Qt::GestureCanceled:
        {
            break;
        }
        }
    }
}

// EOF
