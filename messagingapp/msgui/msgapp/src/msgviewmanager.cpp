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
 * class to manage differnt messaging views.
 *
 */

#include "msgviewmanager.h"

#include <HbMainWindow>
#include <HbAction>
#include <xqserviceutil.h>
#include <QSqlQuery>
#include <QSqlError>
#include <HbApplication>
#include <HbNotificationDialog>

#include "conversationsengine.h"
#include "msglistview.h"
#include "msgunieditorview.h"
#include "msgconversationbaseview.h"
#include "unifiedviewer.h"
#include "draftslistview.h"
#include "msgsettingsview.h"
#include "convergedmessageid.h"

// LOCALIZATION
#define LOC_SAVED_TO_DRAFTS ("Saved to Drafts")

const qint64 NULL_CONVERSATIONID = -1;

MsgViewManager::MsgViewManager(bool serviceRequest, HbMainWindow* mainWindow, QObject* parent) :
    QObject(parent), mMainWindow(mainWindow), mUniEditor(0), mListView(0), mConversationView(0),
        mUniViewer(0), mDraftsListView(0), mSettingsView(0), mBackAction(0), mServiceRequest(
            serviceRequest), mConversationId(-1)
{
    //creating back action.
    mBackAction = new HbAction(Hb::BackAction, this);
    connect(mBackAction, SIGNAL(triggered()), this, SLOT(onBackAction()));

    //create clv as first view if not a service request.
    if (!mServiceRequest) {
        QVariantList param;
        param << MsgBaseView::CLV;
        param << MsgBaseView::CLV;
        switchView(param);
    }
}

MsgViewManager::~MsgViewManager()
{
    // TODO Auto-generated destructor stub
}

void MsgViewManager::onBackAction()
{
    switch (mCurrentView) {
    case MsgBaseView::CLV:
    {
        if (!mServiceRequest) {
            HbApplication::quit();
        }
        else {
            completeServiceRequest();
        }

        break;
    }

    case MsgBaseView::CV:
    {
        //Save content to drafts before switching to clv
        bool result = mConversationView->saveContentToDrafts();

        //marking messages as red in CV.
        mConversationView->markMessagesAsRead();

        //clearing content of cv.
        mConversationView->clearContent();

        if (mServiceRequest) {
            completeServiceRequest();
        }
        else {
            //switch to clv.
            QVariantList param;
            param << MsgBaseView::CLV;
            param << MsgBaseView::CV;
            switchView(param);
            if (result) {
                HbNotificationDialog::launchDialog(LOC_SAVED_TO_DRAFTS);
            }
        }

        break;
    }

    case MsgBaseView::DLV:
    {
        if (mServiceRequest) {
            completeServiceRequest();
        }
        else {
            //switch to clv.
            QVariantList param;
            param << MsgBaseView::CLV;
            param << MsgBaseView::DLV;
            switchView(param);
        }

        break;
    }

    case MsgBaseView::UNIEDITOR:
    {
        //Save content to drafts before switching to clv
        bool result = mUniEditor->saveContentToDrafts();

        //switch to clv.
        if (mServiceRequest) {
            completeServiceRequest();
        }
        else {
            //switch to clv.
            QVariantList param;
            param << MsgBaseView::CLV;
            param << MsgBaseView::UNIEDITOR;
            switchView(param);
            if (result) {
                HbNotificationDialog::launchDialog(LOC_SAVED_TO_DRAFTS);
            }
        }

        break;
    }

    case MsgBaseView::UNIVIEWER:
    {
        //swich to cv.  
        QVariantList param;
        param << MsgBaseView::CV;
        param << MsgBaseView::UNIVIEWER;
        param << mConversationId;
        switchView(param);

        mUniViewer->clearContent();

        break;
    }

    default:
    {
        break;
    }
    }
}

void MsgViewManager::switchView(const QVariantList& data)
{
    int viewId = data.at(0).toInt();
    switch (viewId) {
    case MsgBaseView::DEFAULT:
    {
        handleDefault(data);
        break;
    }
    case MsgBaseView::CLV:
    {
        switchToClv(data);
        break;
    }

    case MsgBaseView::CV:
    {
        switchToCv(data);
        break;
    }

    case MsgBaseView::DLV:
    {
        switchToDlv(data);
        break;
    }

    case MsgBaseView::UNIEDITOR:
    {
        switchToUniEditor(data);
        break;
    }

    case MsgBaseView::UNIVIEWER:
    {
        switchToUniViewer(data);
        break;
    }

    case MsgBaseView::MSGSETTINGS:
    {
        switchToMsgSettings(data);
        break;
    }
    }
}

void MsgViewManager::send(const qint32 contactId, const QString phoneNumber,
    const QString displayName)
{
    qint64 conversationId = ConversationsEngine::instance()-> getConversationIdFromContactId(
        contactId);

    //set mode to viewer, if conversations found else send mode.
    QVariantList param;
    if (conversationId > 0) {
        param << MsgBaseView::CV;
        param << MsgBaseView::SERVICE;
        param << conversationId;
    }
    else {
        ConvergedMessage message;
        ConvergedMessageAddress address;
        address.setAddress(phoneNumber);
        address.setAlias(displayName);
        message.addToRecipient(address);

        QByteArray dataArray;
        QDataStream messageStream(&dataArray, QIODevice::WriteOnly | QIODevice::Append);
        message.serialize(messageStream);

        param << MsgBaseView::UNIEDITOR;
        param << MsgBaseView::SERVICE;
        param << dataArray;
    }
    switchView(param);
    XQServiceUtil::toBackground(false);
}

void MsgViewManager::setServiceRequest(bool request)
{
    mServiceRequest = request;
    mViewAtServiceRequest = mCurrentView;
}

void MsgViewManager::switchToLastSavedView()
{
    QVariantList param;

    switch (mViewAtServiceRequest) {
    case MsgBaseView::CV:
    {
        param << MsgBaseView::CV;
        param << MsgBaseView::SERVICE;
        param << mConversationId;
        break;
    }
    case MsgBaseView::UNIVIEWER:
    {
        param << MsgBaseView::UNIVIEWER;
        param << MsgBaseView::SERVICE;
        break;
    }
    default:
    {
        param << MsgBaseView::CLV;
        param << MsgBaseView::SERVICE;
        break;
    }
    }
    switchView(param);
}

void MsgViewManager::open(qint64 conversationId)
{
    QVariantList param;

    if (conversationId < 0) {
        param << MsgBaseView::CLV;
        param << MsgBaseView::SERVICE;
    }
    else {
        param << MsgBaseView::CV;
        param << MsgBaseView::SERVICE;
        param << conversationId;
    }

    switchView(param);
    XQServiceUtil::toBackground(false);
}

void MsgViewManager::send(QVariant data)
{
    ConvergedMessage message;
    ConvergedMessageAttachmentList attachmentList;
    QString path = data.toString();
    ConvergedMessageAttachment *attachment = new ConvergedMessageAttachment(path);

    attachmentList.append(attachment);
    message.addAttachments(attachmentList);

    QByteArray dataArray;
    QDataStream messageStream(&dataArray, QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);

    QVariantList param;
    param << MsgBaseView::UNIEDITOR;
    param << MsgBaseView::SERVICE;
    param << dataArray;

    switchView(param);
    XQServiceUtil::toBackground(false);
}

qint64 MsgViewManager::findConversationId(const QString phoneNum)
{
    return ConversationsEngine::instance()-> getConversationIdFromAddress(phoneNum);
}

//will be depricated in future.
void MsgViewManager::openEditor(QString phoneNumber, QString name)
{
    ConvergedMessage message;
    ConvergedMessageAddress address;
    address.setAddress(phoneNumber);
    address.setAlias(name);
    message.addToRecipient(address);

    QByteArray dataArray;
    QDataStream messageStream(&dataArray, QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);

    QVariantList param;
    param << MsgBaseView::UNIEDITOR;
    param << MsgBaseView::SERVICE;
    param << dataArray;

    switchView(param);
    XQServiceUtil::toBackground(false);
}

void MsgViewManager::completeServiceRequest()
{
    mServiceRequest = false;
    XQServiceUtil::toBackground(true);
    switchToLastSavedView();
}

void MsgViewManager::switchToClv(const QVariantList& data)
{
    //switch to CLV.
    mCurrentView = MsgBaseView::CLV;
    mPreviousView = data.at(1).toInt();

    //delete uni editor.
    if (mUniEditor) {
        mMainWindow->removeView(mUniEditor);
        delete mUniEditor;
        mUniEditor = NULL;
    }
    //remove the settings view from main window
    if (mSettingsView) {
        mMainWindow->removeView(mSettingsView);
        mSettingsView->setNavigationAction(mBackAction);
        connect(mBackAction, SIGNAL(triggered()), this, SLOT(onBackAction()), Qt::UniqueConnection);
    }
    if (mConversationView) {
        //clearing content of cv.
        mConversationView->clearContent();
    }

    if (!mListView) {
        mListView = new MsgListView();
        mListView->setNavigationAction(mBackAction);
        connect(mListView, SIGNAL(switchView(const QVariantList&)), this,
            SLOT(switchView(const QVariantList&)));
        mMainWindow->addView(mListView);
        mListView->activateView();
    }
    mMainWindow->setCurrentView(mListView);
}

void MsgViewManager::switchToCv(const QVariantList& data)
{
    //switch to CV.
    mCurrentView = MsgBaseView::CV;
    mPreviousView = data.at(1).toInt();

    //delete uni editor.
    if (mUniEditor) {
        mMainWindow->removeView(mUniEditor);
        delete mUniEditor;
        mUniEditor = NULL;
    }

    QVariant var = data.at(2);
    qint64 conversationId;
    if (var.type() == QVariant::String) {
        QString phoneNumber = var.toString();
        conversationId = findConversationId(phoneNumber);

        if (conversationId == NULL_CONVERSATIONID) {
            QVariantList param;
            param << MsgBaseView::CLV;
            param << MsgBaseView::CV;
            switchView(param);
            return;
        }
    }
    else if (var.type() == QVariant::Invalid) {
        // this case comes when a message is deleted from 
        // Unified viewer  set curent view as conversation view 
        // and return
        mMainWindow->setCurrentView(mConversationView, false);
        mUniViewer->clearContent();
        return;
    }
    else {
        conversationId = var.toLongLong();
    }
    //switch to CV.
    mConversationId = conversationId;

    if (!mConversationView) {
        mConversationView = new MsgConversationBaseView();
        mConversationView->setNavigationAction(mBackAction);
        connect(mConversationView, SIGNAL(switchView(const QVariantList&)), this,
            SLOT(switchView(const QVariantList&)));

        mMainWindow->addView(mConversationView);
    }

    mConversationView->openConversation(conversationId);

    if (mServiceRequest) {
        mMainWindow->setCurrentView(mConversationView, false);
    }
    else {
        mMainWindow->setCurrentView(mConversationView, false);
    }
}

void MsgViewManager::switchToDlv(const QVariantList& data)
{
    //switch to DLV.
    mCurrentView = MsgBaseView::DLV;
    mPreviousView = data.at(1).toInt();

    //delete uni editor.
    if (mUniEditor) {
        mMainWindow->removeView(mUniEditor);
        delete mUniEditor;
        mUniEditor = NULL;
    }

    if (!mDraftsListView) {
        mDraftsListView = new DraftsListView();
        mDraftsListView->setNavigationAction(mBackAction);
        connect(mDraftsListView, SIGNAL(switchView(const QVariantList&)), this,
            SLOT(switchView(const QVariantList&)));

        mMainWindow->addView(mDraftsListView);
    }
    mMainWindow->setCurrentView(mDraftsListView);
}

void MsgViewManager::switchToUniEditor(const QVariantList& data)
{
    mCurrentView = MsgBaseView::UNIEDITOR;
    mPreviousView = data.at(1).toInt();

    if (mConversationView) {
        //clearing content of cv.
        mConversationView->clearContent();
    }

    //swich to unieditor.
    if (!mUniEditor) {
        mUniEditor = new MsgUnifiedEditorView();
        mMainWindow->addView(mUniEditor);
        mUniEditor->setNavigationAction(mBackAction);
        connect(mUniEditor, SIGNAL(switchView(const QVariantList&)), this,
            SLOT(switchView(const QVariantList&)));
    }

    if (data.length() > 2) {
        QByteArray dataArray = data.at(2).toByteArray();
        ConvergedMessage *message = new ConvergedMessage;
        QDataStream stream(&dataArray, QIODevice::ReadOnly);
        message->deserialize(stream);

        if (data.length() > 3) {
            mUniEditor->populateContent(*message, data.at(3).toInt());
        }
        else {
            if (MsgBaseView::DLV == mPreviousView) {
                mUniEditor->openDraftsMessage(*(message->id()), message->messageType());
            }
            else {
                mUniEditor->populateContent(*message);
            }
        }
        delete message;
    }

    if (mServiceRequest) {
        mMainWindow->setCurrentView(mUniEditor, false);
    }
    else {
        mMainWindow->setCurrentView(mUniEditor);
    }
}

void MsgViewManager::switchToUniViewer(const QVariantList& data)
{
    mCurrentView = MsgBaseView::UNIVIEWER;
    mPreviousView = data.at(1).toInt();

    //switch to univiewer.
    if (data.length() > 2) {
        qint32 contactId = data.at(2).toLongLong();
        QByteArray dataArray = data.at(3).toByteArray();
        int msgCount = data.at(4).toInt();

        ConvergedMessage *message = new ConvergedMessage;
        QDataStream stream(&dataArray, QIODevice::ReadOnly);
        message->deserialize(stream);
		qint32 messageId = message->id()->getId();
        if (!mUniViewer) {
            mUniViewer = new UnifiedViewer(messageId);
            mUniViewer->setNavigationAction(mBackAction);
            mMainWindow->addView(mUniViewer);
            connect(mUniViewer, SIGNAL(switchView(const QVariantList&)), this,
                SLOT(switchView(const QVariantList&)));
        }
        mUniViewer->populateContent(messageId, true, msgCount);
        delete message;
    }
    mMainWindow->setCurrentView(mUniViewer);
}
void MsgViewManager::switchToMsgSettings(const QVariantList& data)
{
    mCurrentView = MsgBaseView::MSGSETTINGS;
    mPreviousView = data.at(1).toInt();

    if (!mSettingsView) {
        mSettingsView = new MsgSettingsView();
        connect(mSettingsView, SIGNAL(switchView(const QVariantList&)), this,
            SLOT(switchView(const QVariantList&)));
    }
    else {
        //re-connect the back action
        mSettingsView->connectCloseAction();
    }

    mMainWindow->addView(mSettingsView);
    disconnect(mBackAction, SIGNAL(triggered()), this, SLOT(onBackAction()));
    mMainWindow->setCurrentView(mSettingsView);
}

void MsgViewManager::handleDefault(const QVariantList& data)
{
    //special handling incase we switch from settings-view
    int previousViewId = data.at(1).toInt();
    if (MsgBaseView::MSGSETTINGS == previousViewId) {
        //switch to view, where from settings view was launched
        mCurrentView = mPreviousView;
        mPreviousView = previousViewId;
        //remove the settings view from main window
        if (mSettingsView) {
            mMainWindow->removeView(mSettingsView);
            mSettingsView->setNavigationAction(mBackAction);
            connect(mBackAction, SIGNAL(triggered()), this, SLOT(onBackAction()));
        }
        switch (mCurrentView) {
        case MsgBaseView::CLV:
        {
            if (mListView)
                mMainWindow->setCurrentView(mListView);
            break;
        }
        case MsgBaseView::CV:
        {
            if (mConversationView)
                mMainWindow->setCurrentView(mConversationView, false);
            break;
        }
        case MsgBaseView::DLV:
        {
            if (mDraftsListView)
                mMainWindow->setCurrentView(mDraftsListView);
            break;
        }
        case MsgBaseView::UNIEDITOR:
        {
            if (mServiceRequest) {
                mMainWindow->setCurrentView(mUniEditor, false);
            }
            else {
                mMainWindow->setCurrentView(mUniEditor);
            }
            break;
        }
        }
    }
}
