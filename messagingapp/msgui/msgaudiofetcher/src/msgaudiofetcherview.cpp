/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *     The source file for messaging's audio fetcher view.
 *
 */

// SYSTEM INCLUDES
#include <hbaction.h>
#include <hbtoolbar.h>

// USER INCLUDES
#include "msgaudiofetcherview.h"
#include "msgaudiofetcherwidget.h"
#include "convergedmessage.h"

MsgAudioFetcherView::MsgAudioFetcherView()
{
    initToolBar();
    initMainWidget();
    QMetaObject::connectSlotsByName(this);
}

MsgAudioFetcherView::~MsgAudioFetcherView()
{
    removeToolBarAction();
}

void MsgAudioFetcherView::initMainWidget()
{
    mWidget = new MsgAudioFetcherWidget();
    Q_ASSERT(mWidget);
    setWidget(mWidget);
    connect(mWidget, SIGNAL(triggerToolBar(bool)), this, SLOT(enableToolBar(bool)));
}

void MsgAudioFetcherView::initToolBar()
{
    mToolBarLeftAction = new HbAction(this);
    mToolBarLeftAction->setObjectName("leftAction");
    //TODO: need localized string
    mToolBarLeftAction->setText(hbTrId("Play/Pause"));
    toolBar()->addAction(mToolBarLeftAction);
    mToolBarLeftAction->setEnabled(false);

    mToolBarRightAction = new HbAction(this);
    mToolBarRightAction->setObjectName("rightAction");
    //TODO: need localized string
    mToolBarRightAction->setText(hbTrId("Select"));
    mToolBarRightAction->setEnabled(false);
    toolBar()->addAction(mToolBarRightAction);

    toolBar()->setOrientation(Qt::Horizontal);
    toolBar()->setEnabled(false);
}

void MsgAudioFetcherView::removeToolBarAction()
{
    toolBar()->removeAction(mToolBarRightAction);
    toolBar()->removeAction(mToolBarLeftAction);
}

void MsgAudioFetcherView::on_leftAction_triggered()
{
    mWidget->playOrPause();
}

void MsgAudioFetcherView::on_rightAction_triggered()
{    
    // param list for switching to editor view
    QVariantList params;
    QByteArray dataArray;
    QDataStream messageStream
    (&dataArray, QIODevice::WriteOnly | QIODevice::Append);

    ConvergedMessage message;
    QString filepath(mWidget->getCurrentItemPath());
    ConvergedMessageAttachment* attachment =
            new ConvergedMessageAttachment(filepath);
    ConvergedMessageAttachmentList attachmentList;
    attachmentList.append(attachment);
    message.addAttachments(attachmentList);
    
    message.serialize(messageStream);
    params << MsgBaseView::UNIEDITOR;
    params << MsgBaseView::AUDIOFETCHER;
    params << dataArray;
    params << MsgBaseView::ADD_AUDIO;
    emit switchView(params);
}

void MsgAudioFetcherView::enableToolBar(bool enable)
{
    mToolBarRightAction->setEnabled(enable);
    mToolBarLeftAction->setEnabled(enable);
    toolBar()->setEnabled(enable);
}

//End of File
