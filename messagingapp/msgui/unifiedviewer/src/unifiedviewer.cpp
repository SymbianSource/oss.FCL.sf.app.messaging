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
 * Description: Main view of unified viewer
 *
 */

#include "unifiedviewer.h"

// SYSTEM INCLUDES
#include <QGraphicsLinearLayout>
#include <HbAction>
#include <HbToolBar>
#include <hbmessagebox.h>
#include <hbnotificationdialog.h>
#include <HbStyleLoader>

// USER INCLUDES
#include "uniscrollarea.h"
#include "unicontentswidget.h"
#include "univiewerfeeder.h"
#include "convergedmessageid.h"
#include "convergedmessage.h"
#include "convergedmessageaddress.h"
#include "conversationsenginedefines.h"
#include "conversationsengine.h"
#include "debugtraces.h"
#include "nativemessageconsts.h"

// LOCAL CONSTANTS
const QString REPLY_ICON("qtg_mono_reply.svg");
const QString REPLY_ALL_ICON("qtg_mono_reply_all.svg");
const QString FORWARD_ICON("qtg_mono_forward.svg");
const QString DELETE_ICON("qtg_mono_delete.svg");

//LOCALIZED CONSTANTS
#define LOC_DELETE_MESSAGE hbTrId("txt_messaging_dialog_delete_message")
#define LOC_BUTTON_DELETE hbTrId("txt_common_button_delete")
#define LOC_BUTTON_CANCEL hbTrId("txt_common_button_cancel")
#define LOC_DELETE_POPINFO hbTrId("txt_messaging_dpopinfo_message_deleted")

//----------------------------------------------------------------------------
// UnifiedViewer::UnifiedViewer
// constructor
//----------------------------------------------------------------------------
UnifiedViewer::UnifiedViewer(const qint32 messageId, QGraphicsItem *parent) :
    MsgBaseView(parent)
{
    QDEBUG_WRITE("UnifiedViewer contruction start");

    if (!HbStyleLoader::registerFilePath(":/layouts"))
    {
        QDEBUG_WRITE("ERROR: UnifiedViewer -> HbStyleLoader::registerFilePath");
    }

    mMessageId = messageId;
    mViewFeeder = new UniViewerFeeder(mMessageId, this);

    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical, this);

    mScrollArea = new UniScrollArea(this);

    mContentsWidget = new UniContentsWidget(mViewFeeder,this);
	
    connect(mContentsWidget,SIGNAL(sendMessage(const QString&)),
            this, SLOT(sendMessage(const QString&)));

    connect(mScrollArea, SIGNAL(scrolledToNextSlide()),
    mContentsWidget, SLOT(populateNextSlide()));

    mScrollArea->setContentWidget(mContentsWidget);
    mScrollArea->setHorizontalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOff);
    mScrollArea->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAutoHide);
    mMainLayout->addItem(mScrollArea);
    mMainLayout->setSpacing(0);
    mMainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mMainLayout);

    createToolBar();

    QDEBUG_WRITE("UnifiedViewer contruction End");
}

//----------------------------------------------------------------------------
// UnifiedViewer::UnifiedViewer
// Destructor
//----------------------------------------------------------------------------
UnifiedViewer::~UnifiedViewer()
{
    HbStyleLoader::unregisterFilePath(":/layouts");

}

//----------------------------------------------------------------------------
// UnifiedViewer::createToolBar
// Creates tool bar actions
//----------------------------------------------------------------------------
void UnifiedViewer::createToolBar()
{
    HbToolBar *toolBar = this->toolBar();
    toolBar->setOrientation(Qt::Horizontal);

    HbAction* replyAction = new HbAction(HbIcon(REPLY_ICON), "", this);
    toolBar->addAction(replyAction);

    HbAction* replyAllAction = new HbAction(HbIcon(REPLY_ALL_ICON), "", this);
    toolBar->addAction(replyAllAction);

    HbAction* forwardAction = new HbAction(HbIcon(FORWARD_ICON), "", this);
    toolBar->addAction(forwardAction);

    HbAction* deleteAction = new HbAction(HbIcon(DELETE_ICON), "", this);
    toolBar->addAction(deleteAction);

    connect(deleteAction, SIGNAL(triggered()),
    this, SLOT(handleDeleteAction()));
}

//---------------------------------------------------------------
// UnifiedViewer::clearContent
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::clearContent()
{
    QDEBUG_WRITE("UnifiedViewer clearContent start");

    mContentsWidget->clearContent();
    mContentsWidget->resize(mContentsWidget->rect().width(), -1);
    mScrollArea->setPosToStart();
    mViewFeeder->clearContent();

    QDEBUG_WRITE("UnifiedViewer clearContent End");
}

//---------------------------------------------------------------
// UnifiedViewer::populateContent
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::populateContent(const qint32 messageId, bool update, int msgCount)
{
    QDEBUG_WRITE("UnifiedViewer populateContent Start");

    mMsgCount = msgCount;
    mMessageId = messageId;

    QDEBUG_WRITE("UnifiedViewer feeder->updateContent START");

    if (update)
    {
        mViewFeeder->updateContent(messageId);
    }
    QDEBUG_WRITE("UnifiedViewer feeder->updateContent END");

    if ( (mViewFeeder->msgType() == KSenduiMtmMmsUidValue) &&
         (mViewFeeder->slideCount() > 0) )
    {
        mScrollArea->setTotalSlides(mViewFeeder->slideCount());
    }
    else
    {
        mScrollArea->setTotalSlides(1);
    }
    mScrollArea->resetCurrentSlide();

    mContentsWidget->populateContent();

    QDEBUG_WRITE("UnifiedViewer populateContent END");
}

//---------------------------------------------------------------
// UnifiedViewer::handleFwdAction
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::handleFwdAction()
{
}

//---------------------------------------------------------------
// UnifiedViewer::resizeEvent
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::resizeEvent(QGraphicsSceneResizeEvent * event)
{
    Q_UNUSED(event)
    mContentsWidget->resize(this->rect().width(), -1);
}

//---------------------------------------------------------------
// UnifiedViewer::handleDeleteAction
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::handleDeleteAction()
{
    bool result = HbMessageBox::question(LOC_DELETE_MESSAGE,
                                         LOC_BUTTON_DELETE, 
                                         LOC_BUTTON_CANCEL);
    if (result)
    {
        QList<int> msgIdList;
        msgIdList << mMessageId;

        ConversationsEngine::instance()->deleteMessages(msgIdList);
        HbNotificationDialog::launchDialog(LOC_DELETE_POPINFO);

        QVariantList param;
        if (mMsgCount > 1)
        {
            param << MsgBaseView::CV;
            param << MsgBaseView::UNIVIEWER;
        }
        else
        {
            param << MsgBaseView::CLV;
            param << MsgBaseView::UNIVIEWER;
        }

        QVariant dummy(QVariant::Invalid);
        param << dummy;
        emit switchView(param);
    }

}

//---------------------------------------------------------------
// UnifiedViewer::sendMessage
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::sendMessage(const QString& phoneNumber)
    {
    ConvergedMessage message;
    message.setBodyText(QString());

    ConvergedMessageAddress address;
    address.setAlias(phoneNumber);
    address.setAddress(phoneNumber);
    message.addToRecipient(address);

    QByteArray dataArray;
    QDataStream messageStream
    (&dataArray, QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);

    QVariantList params;
    params << MsgBaseView::UNIEDITOR;
    params << MsgBaseView::UNIVIEWER;
    params << dataArray;

    emit switchView(params);
    }
// EOF
