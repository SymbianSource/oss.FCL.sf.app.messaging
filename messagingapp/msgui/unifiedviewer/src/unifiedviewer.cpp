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
#include <centralrepository.h>
#include <MmsEngineDomainCRKeys.h>

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
#include "mmsconformancecheck.h"
#include "UniEditorGenUtils.h" // This is needed for KDefaultMaxSize

// LOCAL CONSTANTS
const QString REPLY_ICON("qtg_mono_reply");
const QString REPLY_ALL_ICON("qtg_mono_reply_all");
const QString FORWARD_ICON("qtg_mono_forward_msg");
const QString SEND_ICON("qtg_mono_send");
const QString DELETE_ICON("qtg_mono_delete");

//LOCALIZED CONSTANTS
#define LOC_DELETE_MESSAGE hbTrId("txt_messaging_dialog_delete_message")
#define LOC_BUTTON_DELETE hbTrId("txt_common_button_delete")
#define LOC_BUTTON_CANCEL hbTrId("txt_common_button_cancel")

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

    mScrollArea = new UniScrollArea(this);
    this->setWidget(mScrollArea);

    mContentsWidget = new UniContentsWidget(mViewFeeder,this);

    connect(mContentsWidget,SIGNAL(sendMessage(const QString&,const QString&)),
            this, SLOT(sendMessage(const QString&,const QString&)));

    connect(mScrollArea, SIGNAL(scrolledToNextSlide()),
    mContentsWidget, SLOT(populateNextSlide()));

    mScrollArea->setContentWidget(mContentsWidget);
    mScrollArea->setHorizontalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOff);
    mScrollArea->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAutoHide);

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
    HbToolBar* toolbar = this->toolBar();
    toolbar->setOrientation(Qt::Horizontal);

    int sendingState = mViewFeeder->sendingState();

    if (mViewFeeder->sendingState() == ConvergedMessage::Failed)
    {
        toolbar->addAction(HbIcon(SEND_ICON), "");
    }
    else
    {
        toolbar->addAction(HbIcon(REPLY_ICON), "");
        toolbar->addAction(HbIcon(REPLY_ALL_ICON), "");
    }

    if (validateMsgForForward())
    {
        toolbar->addAction(HbIcon(FORWARD_ICON), "", this, SLOT(handleFwdAction()));
    }

    toolbar->addAction(HbIcon(DELETE_ICON), "", this, SLOT(handleDeleteAction()));
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

    // Dont show the scroll bar.
    mScrollArea->setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOff);

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

    //Creation of toolbar now depends on content
    createToolBar();
    
    QDEBUG_WRITE("UnifiedViewer populateContent END");
}

//---------------------------------------------------------------
// UnifiedViewer::handleFwdAction
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::handleFwdAction()
{
    ConvergedMessage message;
    ConvergedMessageId id(mMessageId);
    message.setMessageId(id);
    if(mViewFeeder->msgType() == KSenduiMtmMmsUidValue)
    {
        message.setMessageType(ConvergedMessage::Mms);
    }
    else
    {
        message.setMessageType(ConvergedMessage::Sms);    
    }

    QByteArray dataArray;
    QDataStream messageStream
    (&dataArray, QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);

    QVariantList params;
    params << MsgBaseView::UNIEDITOR; // target view
    params << MsgBaseView::UNIVIEWER; // source view

    params << dataArray;
    params << MsgBaseView::FORWARD_MSG;
        
    emit switchView(params);
}

//---------------------------------------------------------------
// UnifiedViewer::resizeEvent
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::resizeEvent(QGraphicsSceneResizeEvent * event)
{
    Q_UNUSED(event)
    mContentsWidget->resize(this->rect().width(), this->rect().height()+1);
}

//---------------------------------------------------------------
// UnifiedViewer::handleDeleteAction
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::handleDeleteAction()
{
    HbMessageBox::question(LOC_DELETE_MESSAGE,this,SLOT(onDialogDeleteMsg(HbAction*)),
                                         LOC_BUTTON_DELETE, 
                                         LOC_BUTTON_CANCEL);
}

//---------------------------------------------------------------
// UnifiedViewer::sendMessage
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::sendMessage(const QString& phoneNumber,const QString& alias)
    {
    ConvergedMessage message;
    message.setBodyText(QString());

    ConvergedMessageAddress address;
    address.setAlias(alias);
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

//---------------------------------------------------------------
// UnifiedViewer::validateMsgForForward
// @see header file
//---------------------------------------------------------------
bool UnifiedViewer::validateMsgForForward()
{
    if (mViewFeeder->msgType() == KSenduiMtmMmsUidValue)
    {
        bool retValue = false;

        //Validate if the mms msg can be forwarded or not
        MmsConformanceCheck* mmsConformanceCheck = new MmsConformanceCheck;        
        retValue = mmsConformanceCheck->validateMsgForForward(mMessageId);

        delete mmsConformanceCheck;        
        return retValue;
    }

    return true;
}

//---------------------------------------------------------------
// UnifiedViewer::onDialogDeleteMsg
// @see header file
//---------------------------------------------------------------
void UnifiedViewer::onDialogDeleteMsg(HbAction* action)
{
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0)) {
        QList<int> msgIdList;
        msgIdList << mMessageId;

        ConversationsEngine::instance()->deleteMessages(msgIdList);

        QVariantList param;
        if (mMsgCount > 1) {
            param << MsgBaseView::CV;
            param << MsgBaseView::UNIVIEWER;
        }
        else {
            param << MsgBaseView::CLV;
            param << MsgBaseView::UNIVIEWER;
        }

        QVariant dummy(QVariant::Invalid);
        param << dummy;
        emit switchView(param);
    }
}

// EOF
