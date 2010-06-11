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
 * Description: This widget is used to display audio content in univiewer.
 *
 */
#include "univieweraudiowidget.h"

// SYSTEM INCLUDES
#include <QFileInfo>
#include <QTimer>
#include <HbMenu>

// USER INCLUDES
#include "msgmediautil.h"
#include "univiewerutils.h"

// LOCAL CONSTANTS
#define LOC_OPEN    hbTrId("txt_common_menu_open")
#define LOC_SAVE    hbTrId("txt_common_menu_save")

const QString AUDIO_ICON("qtg_mono_audio");

//----------------------------------------------------------------------------
// UniViewerAudioWidget::UniViewerAudioWidget
// @see header file
//----------------------------------------------------------------------------
UniViewerAudioWidget::UniViewerAudioWidget(QGraphicsItem *parent) :
    HbPushButton(parent), mViewerUtils(0)
{
    connect(this, SIGNAL(clicked()), this, SLOT(handleShortTap()));
    connect(this, SIGNAL(longPress(QPointF)), this, SLOT(handleLongTap(QPointF)));
}

//----------------------------------------------------------------------------
// UniViewerAudioWidget::~UniViewerAudioWidget
// @see header file
//----------------------------------------------------------------------------
UniViewerAudioWidget::~UniViewerAudioWidget()
{
}

//----------------------------------------------------------------------------
// UniViewerAudioWidget::~UniViewerAudioWidget
// @see header file
//----------------------------------------------------------------------------
void UniViewerAudioWidget::populate(const QString &mimeType, const QString &filePath)
{
    mMimeType = mimeType;
    mMediaPath = filePath;

    this->setIcon(HbIcon(AUDIO_ICON));
    QFileInfo fileInfo(mMediaPath);
    this->setText(fileInfo.baseName());
    this->setTextAlignment(Qt::AlignLeft);
    MsgMediaUtil mediaUtil;
    this->setAdditionalText(mediaUtil.mediaDuration(mMediaPath));
}

//----------------------------------------------------------------------------
// UniViewerAudioWidget::handleShortTap
// @see header file
//----------------------------------------------------------------------------
void UniViewerAudioWidget::handleShortTap()
{
    emit shortTap(mMediaPath);

    // Open the media.
    handleOpen();
}

//----------------------------------------------------------------------------
// UniViewerAudioWidget::handleLongTap
// @see header file
//----------------------------------------------------------------------------
void UniViewerAudioWidget::handleLongTap(const QPointF &position)
{
    emit longTap(position);

    // Display context sensitive menu.
    HbMenu* menu = new HbMenu;
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction(LOC_OPEN, this, SLOT(handleOpen()));
    menu->addAction(LOC_SAVE, this, SLOT(handleSave()));
    menu->setPreferredPos(position);
    menu->show();
}

//----------------------------------------------------------------------------
// UniViewerAudioWidget::handleOpen
// @see header file
//----------------------------------------------------------------------------
void UniViewerAudioWidget::handleOpen()
{
    this->ungrabGesture(Qt::TapGesture);
    
    if (!mViewerUtils) {
        mViewerUtils = new UniViewerUtils(this);
    }
    mViewerUtils->launchContentViewer(mMimeType, mMediaPath);
    
    //fire timer to regrab gesture after some delay.
    QTimer::singleShot(300,this,SLOT(regrabGesture()));
}

//----------------------------------------------------------------------------
// UniViewerAudioWidget::handleSave
// @see header file
//----------------------------------------------------------------------------
void UniViewerAudioWidget::handleSave()
{

}

//---------------------------------------------------------------
// UniViewerAudioWidget::regrabGesture
// @see header file
//---------------------------------------------------------------
void UniViewerAudioWidget::regrabGesture()
{
    this->grabGesture(Qt::TapGesture);
}
// EOF
