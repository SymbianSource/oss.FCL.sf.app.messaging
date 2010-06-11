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

// SYSTEM INCLUDES
#include <HbTapGesture>
#include <HbWidget>
#include <HbInstantFeedback>
#include <HbMenu>
#include <QPixmap>
#include <QTimer>

// USER INCLUDES
#include "univiewerutils.h"

// LOCAL CONSTANTS
#define LOC_OPEN    hbTrId("txt_common_menu_open")
#define LOC_SAVE    hbTrId("txt_common_menu_save")

//---------------------------------------------------------------
// UniViewerPixmapWidget::UniViewerPixmapWidget
// @see header file
//---------------------------------------------------------------
UniViewerPixmapWidget::UniViewerPixmapWidget(QGraphicsItem *parent) :
    HbIconItem(parent), mViewerUtils(0)
{
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
void UniViewerPixmapWidget::populate(const QString &mimeType, const QString &pixmapPath)
{
    mMimeType = mimeType;
    mPixmapPath = pixmapPath;
    QPixmap pixmap(mPixmapPath);
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
                // Handle longtap.
                handleLongTap(tapGesture->scenePosition());
            }
            break;
        }
        case Qt::GestureFinished:
        {
            HbInstantFeedback::play(HbFeedback::Basic);
            if (HbTapGesture::Tap == tapGesture->tapStyleHint()) {
                // Handle short tap
                handleShortTap();
            }
            break;
        }
        case Qt::GestureCanceled:
        {
            HbInstantFeedback::play(HbFeedback::Basic);
            break;
        }
        }
    }
    else {
        HbIconItem::gestureEvent(event);
    }
}

//---------------------------------------------------------------
// UniViewerPixmapWidget::handleOpen
// @see header file
//---------------------------------------------------------------
void UniViewerPixmapWidget::handleOpen()
{
    this->ungrabGesture(Qt::TapGesture);
    
    if (!mViewerUtils) {
        mViewerUtils = new UniViewerUtils(this);
    }
    mViewerUtils->launchContentViewer(mMimeType, mPixmapPath);
    
    //fire timer to regrab gesture after some delay.
    QTimer::singleShot(300,this,SLOT(regrabGesture()));
}

//---------------------------------------------------------------
// UniViewerPixmapWidget::handleSave
// @see header file
//---------------------------------------------------------------
void UniViewerPixmapWidget::handleSave()
{
}

//----------------------------------------------------------------------------
// UniViewerPixmapWidget::handleShortTap
// @see header file
//----------------------------------------------------------------------------
void UniViewerPixmapWidget::handleShortTap()
{
    emit shortTap(mPixmapPath);

    // Open the media.
    handleOpen();
}

//---------------------------------------------------------------
// UniViewerPixmapWidget::handleLongTap
// @see header file
//---------------------------------------------------------------
void UniViewerPixmapWidget::handleLongTap(const QPointF &position)
{
    emit longTap(position);

    HbMenu* menu = new HbMenu;
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction(LOC_OPEN, this, SLOT(handleOpen()));
    menu->addAction(LOC_SAVE, this, SLOT(handleSave()));
    menu->setPreferredPos(position);
    menu->show();
}

//---------------------------------------------------------------
// UniViewerPixmapWidget::regrabGesture
// @see header file
//---------------------------------------------------------------
void UniViewerPixmapWidget::regrabGesture()
{
    this->grabGesture(Qt::TapGesture);
}
// EOF
