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
#include <xqappmgr.h>
#include <hbmessagebox.h>
#include <HbView>

#include "conversationsengine.h"
#include "msglistview.h"
#include "msgunieditorview.h"
#include "msgconversationbaseview.h"
#include "unifiedviewer.h"
#include "draftslistview.h"
#include "msgsettingsview.h"
#include "convergedmessageid.h"
#include "ringbc.h"
#include "unidatamodelloader.h"
#include "unidatamodelplugininterface.h"

// LOCALIZATION
#define LOC_BUTTON_DELETE hbTrId("txt_common_button_delete")
#define LOC_BUTTON_CANCEL hbTrId("txt_common_button_cancel")
#define LOC_DELETE_MESSAGE hbTrId("txt_messaging_dialog_delete_message")
#define LOC_DIALOG_SAVE_RINGTONE hbTrId("txt_conversations_dialog_save_ringing_tone")
#define LOC_COMMON_SAVE  hbTrId("txt_common_menu_save")

const qint64 NULL_CONVERSATIONID = -1;

MsgViewManager::MsgViewManager(bool serviceRequest, HbMainWindow* mainWindow, QObject* parent) :
    QObject(parent), mMainWindow(mainWindow), mUniEditor(0), mListView(0), mConversationView(0),
        mUniViewer(0), mDraftsListView(0), mSettingsView(0), mBackAction(0), mServiceRequest(
            serviceRequest), mConversationId(-1), mViewServiceRequest(false),mMessageId(-1)
{
    //creating back action.
    mBackAction = new HbAction(Hb::BackNaviAction, this);
    connect(mBackAction, SIGNAL(triggered()), this, SLOT(onBackAction()));

    //create clv as first view if not a service request.
    if (!mServiceRequest) {
        QVariantList param;
        param << MsgBaseView::CLV;
        param << MsgBaseView::CLV;
        switchView(param);
    }
    else
    {
        mDummyview = new HbView();
        mMainWindow->addView(mDummyview);
        mMainWindow->setCurrentView(mDummyview);
        mViewTobeDeleted << mDummyview;
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
        // if its viewer service then quit the application
        if (mViewServiceRequest) {
            HbApplication::quit();
            break;
        }

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
        mConversationView->saveContentToDrafts();

        //marking messages as red in CV.
        mConversationView->markMessagesAsRead();

        // reset the conversation id published
        mConversationView->setPSCVId(false);
        
        //clearing content of cv.
        mConversationView->clearContent();

        // if service request for veiwer the just switch to univiewer.
        if (mViewServiceRequest) {
            mCurrentView = MsgBaseView::UNIVIEWER;
            mMainWindow->setCurrentView(mUniViewer);
            break;
        }

        if (mServiceRequest) {
            completeServiceRequest();
        }
        else {
            //switch to clv.
            QVariantList param;
            param << MsgBaseView::CLV;
            param << MsgBaseView::CV;
            switchView(param);
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
        mUniEditor->saveContentToDrafts();

        // if service request for veiwer the just switch to univiewer.
        if (mViewServiceRequest) {
            mCurrentView = MsgBaseView::UNIVIEWER;
            mMainWindow->setCurrentView(mUniViewer);
            break;
        }

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
        }

        break;
    }

    case MsgBaseView::UNIVIEWER:
    {
        // if its launched via service request quit the app
        if (mViewServiceRequest) {
            HbApplication::quit();
        }
        else {
            QVariantList param;
            param << MsgBaseView::CV;
            param << MsgBaseView::UNIVIEWER;
            param << mConversationId;
            switchView(param);

        }
        break;
    }
    case MsgBaseView::MSGSETTINGS:
    {
        QVariantList param;
        param << MsgBaseView::DEFAULT;
        param << MsgBaseView::MSGSETTINGS;
        switchView(param);
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

void MsgViewManager::deletePreviousView()
{
    while(mViewTobeDeleted.count())
    {
        HbView* v = mViewTobeDeleted.takeAt(0);
        mMainWindow->removeView(v);
        delete v;
    }
    disconnect(mMainWindow, SIGNAL(viewReady()), this, SLOT(deletePreviousView()));
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

void MsgViewManager::send(const QString phoneNumber, const QString alias, const QString bodyText)
{
    ConvergedMessage message;
    ConvergedMessageAddress address;
    address.setAddress(phoneNumber);
    address.setAlias(alias);
    message.addToRecipient(address);
    message.setBodyText(bodyText);

    QVariantList param;
    QByteArray dataArray;
    QDataStream messageStream(&dataArray, QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);

    param << MsgBaseView::UNIEDITOR;
    param << MsgBaseView::SERVICE;
    param << dataArray;

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
        
        if( mCurrentView == MsgBaseView::CV && mConversationView)
            {
            mConversationView->setPSCVId(false);
            }
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
    // handle multiple files from sendUI
    // e.g. contacts can send multiple vcards
    QStringList receivedFiles = data.toStringList();
    int recFileCount = receivedFiles.count();
    for (int i = 0; i < recFileCount; i++) {
        ConvergedMessageAttachment *attachment =
            new ConvergedMessageAttachment(receivedFiles.at(i));
        attachmentList.append(attachment);
    }
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

    // delete case from viewer service  
    if (mViewServiceRequest && (mPreviousView == MsgBaseView::UNIVIEWER)) {
        // quit the application 
        HbApplication::quit();
    }

    if (mConversationView) {
        mConversationView->saveContentToDrafts();
        //clearing content of cv.
        mConversationView->clearContent();
    }

    if (!mListView) {
        mListView = new MsgListView();
        mListView->setNavigationAction(mBackAction);
        connect(mListView, SIGNAL(switchView(const QVariantList&)), this,
            SLOT(switchView(const QVariantList&)));
        mMainWindow->addView(mListView);
    }

    mMainWindow->setCurrentView(mListView);
    
    //delete UniEditor
    if (mUniEditor)
    {
        appendViewToBeDeleted(mUniEditor);
        mUniEditor = NULL;
    }
    
    //delete UniViewer
    if (mUniViewer)
    {
        appendViewToBeDeleted(mUniViewer);
        mUniViewer = NULL;
    }
}

void MsgViewManager::switchToCv(const QVariantList& data)
{
    //switch to CV.
    mCurrentView = MsgBaseView::CV;
    mPreviousView = data.at(1).toInt();

    // delete case from viewer service
    if (mViewServiceRequest && (mPreviousView == MsgBaseView::UNIVIEWER)) {
        // quit the application 
        HbApplication::quit();
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
            
            if( mCurrentView == MsgBaseView::CV && mConversationView){
                mConversationView->setPSCVId(false);
                }
            
            switchView(param);
            return;
        }
    }
    else if (var.type() == QVariant::Invalid) {
        // this case comes when a message is deleted from 
        // Unified viewer  set curent view as conversation view 
        // and return
        mMainWindow->setCurrentView(mConversationView);
		
		// publish already opened conversation's id
        mConversationView->setPSCVId(true);
		
        if (mUniViewer)
        {
            appendViewToBeDeleted(mUniViewer);
            mUniViewer = NULL;
        }

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
    else if (mConversationView->conversationId() != mConversationId){
  		//Save content to drafts before switching to different CV
        mConversationView->saveContentToDrafts();
        //clearing content of current cv.
        mConversationView->clearContent();
  	}	

    mConversationView->openConversation(conversationId);

    if (mServiceRequest) {
        mMainWindow->setCurrentView(mConversationView);
    }
    else {
        mMainWindow->setCurrentView(mConversationView);
    }
    
    //delete UniEditor
    if (mUniEditor)
    {
        appendViewToBeDeleted(mUniEditor);
        mUniEditor = NULL;
    }
    
    //delete UniViewer
    if (mUniViewer)
    {
        appendViewToBeDeleted(mUniViewer);
        mUniViewer = NULL;
    }
}

void MsgViewManager::switchToDlv(const QVariantList& data)
{
    //switch to DLV.
    mCurrentView = MsgBaseView::DLV;
    mPreviousView = data.at(1).toInt();

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
    /**
     * Editor is tried to open again before exiting the previously
     * opened editor. Multi taping in DLV or Forward.
     */
    if (mUniEditor) {
        return;
    }

    mCurrentView = MsgBaseView::UNIEDITOR;
    mPreviousView = data.at(1).toInt();

    if (mConversationView) {
        //clearing content of cv.
        mConversationView->clearContent();
    }

     // reset conversation id published
     if(mPreviousView == MsgBaseView::CV && mConversationView)
         {
         mConversationView->setPSCVId(false);
         }
     
    //swich to unieditor.
    if (!mUniEditor) {
        mUniEditor = new MsgUnifiedEditorView();
        mMainWindow->addView(mUniEditor);
        mUniEditor->setNavigationAction(mBackAction);
        connect(mUniEditor, SIGNAL(switchView(const QVariantList&)), this,
            SLOT(switchView(const QVariantList&)));
    }

    // check if additional data for unieditor's consumption is available
    if (data.length() > 2) {
        QVariantList editorData;
        // i=2 because view manager consumed first two args
        for (int i = 2; i < data.length(); i++) {
            editorData << data.at(i);
        }

        if (MsgBaseView::DLV == mPreviousView) {
            mUniEditor->openDraftsMessage(editorData);
        }
        else {
            mUniEditor->populateContent(editorData);
        }
    }

    if (mServiceRequest) {
        mMainWindow->setCurrentView(mUniEditor);
    }
    else {
        mMainWindow->setCurrentView(mUniEditor);
    }
    //delete UniViewer
    if (mUniViewer )
    {
        appendViewToBeDeleted(mUniViewer);
        mUniViewer = NULL;
    }
}

void MsgViewManager::switchToUniViewer(const QVariantList& data)
{
    /**
     * Viewer is tried to open again before exiting the previously
     * opened viewer. Multi taping bubbles in CV.
     */
    if (mUniViewer) {
        return;
    }

    mCurrentView = MsgBaseView::UNIVIEWER;
    mPreviousView = data.at(1).toInt();

    //switch to univiewer.
    if (data.length() > 2) {
        qint32 contactId = data.at(2).toLongLong();
        qint32 messageId = data.at(3).toInt();
        int msgCount = data.at(4).toInt();
        int canForwardMessage = data.at(5).toInt();
        
        if (!mUniViewer) {
            mUniViewer = new UnifiedViewer(messageId, canForwardMessage);
            mUniViewer->setNavigationAction(mBackAction);
            mMainWindow->addView(mUniViewer);
            connect(mUniViewer, SIGNAL(switchView(const QVariantList&)), this,
                SLOT(switchView(const QVariantList&)));
        }
        mUniViewer->populateContent(messageId, true, msgCount);        
    }
    
    if(mPreviousView==MsgBaseView::CV && mConversationView)
        {
        mConversationView->setPSCVId(false);
        }
    
    mMainWindow->setCurrentView(mUniViewer);
}
void MsgViewManager::switchToMsgSettings(const QVariantList& data)
{
    mCurrentView = MsgBaseView::MSGSETTINGS;
    mPreviousView = data.at(1).toInt();

    if (!mSettingsView) {
        
        MsgSettingsView::SettingsView view = MsgSettingsView::DefaultView;
        if (mPreviousView == MsgBaseView::UNIEDITOR || mPreviousView
                        == MsgBaseView::CV)
                {
                    view = (MsgSettingsView::SettingsView)data.at(2).toInt();         
                }
        
        mSettingsView = new MsgSettingsView(view);
        mSettingsView->setNavigationAction(mBackAction);
        mMainWindow->addView(mSettingsView);
        mMainWindow->setCurrentView(mSettingsView);
    }
    if(mPreviousView==MsgBaseView::CV && mConversationView){
        mConversationView->setPSCVId(false);
        }
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
        if (mSettingsView)
        {
            appendViewToBeDeleted(mSettingsView);
            mSettingsView = NULL;
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
                mMainWindow->setCurrentView(mConversationView);
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
                mMainWindow->setCurrentView(mUniEditor);
            }
            else {
                mMainWindow->setCurrentView(mUniEditor);
            }
            break;
        }
        }
    }
}

void MsgViewManager::view(int msgId)
{
    int msgType;
    int msgSubType;
    mMessageId = msgId;
    qint32 messageId(msgId);
    ConversationsEngine::instance()->markAsReadAndGetType(messageId, msgType, msgSubType);

    switch (msgType) {
    case ConvergedMessage::Sms:
    case ConvergedMessage::Mms:
    case ConvergedMessage::MmsNotification:
    {
        handleSmsMmsMsg(msgId);
        break;
    }
    case ConvergedMessage::BioMsg:
    {
        if (msgSubType == ConvergedMessage::RingingTone) {
            handleRingtoneMsg(msgId);

        }
        else if (msgSubType == ConvergedMessage::Provisioning) {
            handleProvisoningMsg(msgId);
        }
        break;
    }
    case ConvergedMessage::BT:
    {
        break;
    }
    default:
    {
        // for un supported message show delete option
        HbMessageBox::question(LOC_DELETE_MESSAGE,this,SLOT(onDialogDeleteMsg(HbAction*)),
            LOC_BUTTON_DELETE,
            LOC_BUTTON_CANCEL);
        break;
    }
    }
}

// ----------------------------------------------------------------------------
// MsgViewManager::handleSmsMmsMsg
// @see header
// ----------------------------------------------------------------------------
void MsgViewManager::handleSmsMmsMsg(int msgId)
{
    mViewServiceRequest = true;

    mCurrentView = MsgBaseView::UNIVIEWER;

    if (!mUniViewer) {
        mUniViewer = new UnifiedViewer(msgId);
        mUniViewer->setNavigationAction(mBackAction);
        mMainWindow->addView(mUniViewer);
        connect(mUniViewer, SIGNAL(switchView(const QVariantList&)), this,
            SLOT(switchView(const QVariantList&)));
    }
    mUniViewer->populateContent(msgId, true, -1);

    mMainWindow->setCurrentView(mUniViewer);
}

// ----------------------------------------------------------------------------
// MsgViewManager::handleRingtoneMsg
// @see header
// ----------------------------------------------------------------------------
void MsgViewManager::handleRingtoneMsg(int msgId)
{
    mMessageId = msgId;
    HbMessageBox::question(LOC_DIALOG_SAVE_RINGTONE, this,
        SLOT(onDialogSaveTone(HbAction*)), LOC_COMMON_SAVE, LOC_BUTTON_CANCEL);
}

// ----------------------------------------------------------------------------
// MsgViewManager::handleProvisoningMsg
// @see header
// ----------------------------------------------------------------------------
void MsgViewManager::handleProvisoningMsg(int msgId)
{
    QString messageId;
    messageId.setNum(msgId);

    XQApplicationManager* aiwMgr = new XQApplicationManager();

    XQAiwRequest* request = aiwMgr->create("com.nokia.services.MDM", "Provisioning",
        "ProcessMessage(QString)", true); // embedded

    if (request) {
        QList<QVariant> args;
        args << QVariant(messageId);
        request->setArguments(args);

        // Send the request
        bool res = request->send();

        // Cleanup
        delete request;
    }

    delete aiwMgr;

    // close the application once its handled
    HbApplication::quit();
}

void MsgViewManager::setViewInteractive()
{
    if(!mMainWindow->isInteractive())
    {
        mMainWindow->setInteractive(true);
    }
    
    disconnect(mMainWindow, SIGNAL(viewReady()),this,SLOT(setViewInteractive()));
}

void  MsgViewManager::appendViewToBeDeleted(HbView* view)
{
    if (view)
    {
        mViewTobeDeleted << view;
        connect(mMainWindow, SIGNAL(viewReady()), this, SLOT(deletePreviousView()));
    }
}

// ----------------------------------------------------------------------------
// MsgViewManager::onDialogDeleteMsg
// @see header
// ----------------------------------------------------------------------------
void MsgViewManager::onDialogDeleteMsg(HbAction* action)
{
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0)) {
        QList<int> msgIdList;
        msgIdList << mMessageId;
        ConversationsEngine::instance()->deleteMessages(msgIdList);
    }
    HbApplication::quit(); // exit after handling
}

// ----------------------------------------------------------------------------
// MsgViewManager::onDialogSaveTone
// @see header
// ----------------------------------------------------------------------------
void MsgViewManager::onDialogSaveTone(HbAction* action)
    {
        HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
        if (action == dlg->actions().at(0)) {
            UniDataModelLoader* pluginLoader = new UniDataModelLoader();
            UniDataModelPluginInterface* pluginInterface = pluginLoader->getDataModelPlugin(
                ConvergedMessage::BioMsg);
            pluginInterface->setMessageId(mMessageId);
            UniMessageInfoList attachments = pluginInterface->attachmentList();

            QString attachmentPath = attachments.at(0)->path();

            RingBc* ringBc = new RingBc();
            ringBc->saveTone(attachmentPath);

            // clear attachement list : its allocated at data model
            while (!attachments.isEmpty()) {
                delete attachments.takeFirst();
            }

            delete ringBc;
            delete pluginLoader;
        }

        // close the application once its handled
        HbApplication::quit();
}

// ----------------------------------------------------------------------------
// MsgViewManager::currentView
// @see header
// ----------------------------------------------------------------------------
int MsgViewManager::currentView()
    {
    return mCurrentView;
    }
