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
 * Description:
 *
 */

#include "univiewermediawidget.h"

// SYSTEM INCLUDES
#include <HbIconItem>
#include <HbTextItem>
#include <HbPushButton>
#include <HbFrameDrawer>

// USER INCLUDES
#include "debugtraces.h"

// LOCAL CONSTANTS

//----------------------------------------------------------------------------
// UniViewerMediaWidget::UniViewerMediaWidget
// @see header file
//----------------------------------------------------------------------------
UniViewerMediaWidget::UniViewerMediaWidget(QGraphicsItem *parent):
        HbWidget(parent), mMediaIcon(0), mName(0), mInfo(0), mFrameItem(0)
{
    init();
}

//----------------------------------------------------------------------------
// UniViewerMediaWidget::~UniViewerMediaWidget
// @see header file
//----------------------------------------------------------------------------
UniViewerMediaWidget::~UniViewerMediaWidget()
{
}

//----------------------------------------------------------------------------
// UniViewerMediaWidget::init
// @see header file
//----------------------------------------------------------------------------
void UniViewerMediaWidget::init()
{
    mMediaIcon = new HbIconItem(this);
    HbStyle::setItemName(mMediaIcon, "mediaIcon");

    mName = new HbTextItem(this);
    HbStyle::setItemName(mName, "text-1");

    mInfo = new HbTextItem(this);
    HbStyle::setItemName(mInfo, "text-2");

    mFrameItem = new HbPushButton(this);
    HbStyle::setItemName(mFrameItem, "bgFrame");

    connect(mFrameItem, SIGNAL(clicked(bool)), this, SIGNAL(clicked()));
}

//----------------------------------------------------------------------------
// UniViewerMediaWidget::populate
// @see header file
//----------------------------------------------------------------------------
void UniViewerMediaWidget::populate(const QString &icon, const QString &name,
                                    const QString &info, const QString &bgFrame)
{
    HbFrameDrawer *fd = new HbFrameDrawer(bgFrame,
                                          HbFrameDrawer::NinePieces);
    mFrameItem->setFrameBackground(fd);

    mMediaIcon->setIconName(icon);
    mName->setText(name);
    mInfo->setText(info);

}

//----------------------------------------------------------------------------
// UniViewerMediaWidget::clearContent
// @see header file
//----------------------------------------------------------------------------
void UniViewerMediaWidget::clearContent()
{
    mMediaIcon->setIconName("");
    mName->setText("");
    mInfo->setText("");
}

// EOF
