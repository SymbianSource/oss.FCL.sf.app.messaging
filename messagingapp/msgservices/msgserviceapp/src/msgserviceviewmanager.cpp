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
 * Description:  Manages differnt messaging views.
 *
 */

#include "msgserviceviewmanager.h"

#include <HbMainWindow>
#include <HbAction>
#include <HbApplication>
#include <hbmessagebox.h>

#include <xqserviceutil.h>
#include <xqappmgr.h>

#include "msgunieditorview.h"
#include "unifiedviewer.h"
#include "msgstorehandler.h"

#include "msgsettingsview.h"
#include "convergedmessageid.h"
#include "ringbc.h"
#include "unidatamodelloader.h"
#include "unidatamodelplugininterface.h"

// LOCALIZATION
#define LOC_BUTTON_DELETE hbTrId("txt_common_button_delete")
#define LOC_BUTTON_CANCEL hbTrId("txt_common_button_cancel")
#define LOC_DELETE_MESSAGE hbTrId("txt_messaging_dialog_delete_message")
#define LOC_DLG_SAVE_RINGTONE hbTrId("txt_conversations_dialog_save_ringing_tone")
#define LOC_BUTTON_SAVE hbTrId("txt_common_menu_save")

//----------------------------------------------------------------------------
// MsgViewInterface::MsgViewInterface
// @see header
//----------------------------------------------------------------------------
MsgServiceViewManager::MsgServiceViewManager(MsgStoreHandler* storeHandler,
        HbMainWindow* mainWindow, QObject* parent) :
        QObject(parent), mMainWindow(mainWindow), mUniEditor(NULL),
        mUniViewer(NULL), mSettingsView(NULL), mBackAction(NULL),
        mStoreHandler(storeHandler),mMessageId(-1)
    {
    //creating back action.
    mBackAction = new HbAction(Hb::BackNaviAction, this);
    connect(mBackAction, SIGNAL(triggered()), this, SLOT(onBackAction()));
    
    // create a temp view : which is required for lazy loading of other views
    HbView* tempView = new HbView();
    mMainWindow->addView(tempView);
    mMainWindow->setCurrentView(tempView);
    }

//----------------------------------------------------------------------------
// MsgServiceViewManager::~MsgServiceViewManager
// @see header
//----------------------------------------------------------------------------
MsgServiceViewManager::~MsgServiceViewManager()
    {
  
    }

//----------------------------------------------------------------------------
// MsgServiceViewManager::onBackAction
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::onBackAction()
    {
    switch(mCurrentView)
        {
        case MsgBaseView::UNIEDITOR:
            {
            mUniEditor->saveContentToDrafts();
            HbApplication::quit();
            break;
            }
        case MsgBaseView::MSGSETTINGS:
            {
            //delete the settings instance
            if (mSettingsView)
            {
                mMainWindow->removeView(mSettingsView);
                delete mSettingsView;
                mSettingsView = NULL;
            }
            if(mUniEditor)
                {
                mMainWindow->setCurrentView(mUniEditor);
                mCurrentView = MsgBaseView::UNIEDITOR;
                }
            else 
                {
                ConvergedMessage message;
                QVariantList param;
                QByteArray dataArray;
                QDataStream messageStream(&dataArray, 
                        QIODevice::WriteOnly | QIODevice::Append);
                message.serialize(messageStream);
                param << dataArray;

                // switch to editor
                switchToUniEditor(param);
                }
            break;
            }
        case MsgBaseView::UNIVIEWER: 
        default:
            {
            HbApplication::quit();
            break;
            }
            
        }
    }

//----------------------------------------------------------------------------
// MsgServiceViewManager::switchView
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::switchView(const QVariantList& data)
    {
    int viewId = data.at(0).toInt();
    switch (viewId) {
        case MsgBaseView::UNIEDITOR:
            {
            // except first 2 parameters pass other parameters
            QVariantList editorData;
            for(int a = 2; a < data.length(); ++a)
                {
                editorData << data.at(a);
                }
            switchToUniEditor(editorData);
            break;
            }

        case MsgBaseView::MSGSETTINGS:
            {
            switchToMsgSettings(data);
            break;
            }
        default: 
            {
            HbApplication::quit();
            }
    }
    }

//----------------------------------------------------------------------------
// MsgServiceViewManager::send
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::send(const QString phoneNumber, 
        const qint32 contactId, 
        const QString displayName)
    {
    ConvergedMessage message;
    ConvergedMessageAddress address;
    address.setAddress(phoneNumber);
    address.setAlias(displayName);
    message.addToRecipient(address);

    QVariantList param;
    QByteArray dataArray;
    QDataStream messageStream(&dataArray, 
            QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);
    param << dataArray;
    
    // switch to editor
    switchToUniEditor(param);
    
    XQServiceUtil::toBackground(false);
    }

//----------------------------------------------------------------------------
// MsgServiceViewManager::send
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::send(const QString phoneNumber, 
        const QString alias, 
        const QString bodyText)
    {
    ConvergedMessage message;
    ConvergedMessageAddress address;
    address.setAddress(phoneNumber);
    address.setAlias(alias);
    message.addToRecipient(address);
    message.setBodyText(bodyText);

    QVariantList param;
    QByteArray dataArray;
    QDataStream messageStream(&dataArray, 
            QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);
    param << dataArray;

    // switch to editor
    switchToUniEditor(param);
    
    XQServiceUtil::toBackground(false);
    }

//----------------------------------------------------------------------------
// MsgServiceViewManager::send
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::send(QVariant data)
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

    QVariantList param;
    QByteArray dataArray;
    QDataStream messageStream(&dataArray, 
            QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);
    param << dataArray;

    // switch to editor
    switchToUniEditor(param);
    
    XQServiceUtil::toBackground(false);
    }

//----------------------------------------------------------------------------
// MsgServiceViewManager::switchToUniEditor
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::switchToUniEditor(const QVariantList& editorData)
    {
    // construct
    if (!mUniEditor) {
    mUniEditor = new MsgUnifiedEditorView();
    mMainWindow->addView(mUniEditor);
    mUniEditor->setNavigationAction(mBackAction);
    connect(mUniEditor, SIGNAL(switchView(const QVariantList&)), this,
            SLOT(switchView(const QVariantList&)));
    // construct completion : viewReady() signal was not called when 
    // editor is constructed first time.
   // mUniEditor->doDelayedConstruction();
    }
    
    // populate
    mUniEditor->populateContent(editorData);
    
    // set current view as editor
    mMainWindow->setCurrentView(mUniEditor);
    mCurrentView = MsgBaseView::UNIEDITOR;
    }

//----------------------------------------------------------------------------
// MsgServiceViewManager::switchToMsgSettings
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::switchToMsgSettings(const QVariantList& data)
    {
    MsgSettingsView::SettingsView view = MsgSettingsView::DefaultView;
    
    if (mCurrentView == MsgBaseView::UNIEDITOR)
    {
        view = (MsgSettingsView::SettingsView)data.at(2).toInt();
    }
    
    mCurrentView = MsgBaseView::MSGSETTINGS;

    if (!mSettingsView) {        
    mSettingsView = new MsgSettingsView(view);
    mSettingsView->setNavigationAction(mBackAction);
    mMainWindow->addView(mSettingsView);
    }
    mMainWindow->setCurrentView(mSettingsView);
    }

//----------------------------------------------------------------------------
// MsgServiceViewManager::view
// @see header
//----------------------------------------------------------------------------
void MsgServiceViewManager::view(int msgId)
    {
    int msgType;
    int msgSubType;
   
    mMessageId = msgId;
    // Mark as read and get message type
    mStoreHandler->markAsReadAndGetType(msgId,msgType,msgSubType);
    
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
            handleBTMessage(msgId);
            break;
            }
        default:
            {
            // for un supported message show delete option
            HbMessageBox::question(LOC_DELETE_MESSAGE, 
                this,SLOT(onDialogDeleteMsg(HbAction*)),    
                LOC_BUTTON_DELETE,
                LOC_BUTTON_CANCEL);
            break;
            }
    }
    }

// ----------------------------------------------------------------------------
// MsgServiceViewManager::handleSmsMmsMsg
// @see header
// ----------------------------------------------------------------------------
void MsgServiceViewManager::handleSmsMmsMsg(int msgId)
    {
    if (!mUniViewer) {
    mUniViewer = new UnifiedViewer(msgId);
    mUniViewer->setNavigationAction(mBackAction);
    mMainWindow->addView(mUniViewer);
    connect(mUniViewer, SIGNAL(switchView(const QVariantList&)), this,
            SLOT(switchView(const QVariantList&)));
    }
    mUniViewer->populateContent(msgId, true, 1);

    mMainWindow->setCurrentView(mUniViewer);
    
    // set current view as viewer
    mCurrentView = MsgBaseView::UNIVIEWER;
    }

// ----------------------------------------------------------------------------
// MsgServiceViewManager::handleRingtoneMsg
// @see header
// ----------------------------------------------------------------------------
void MsgServiceViewManager::handleRingtoneMsg(int msgId)
    {
    mMessageId = msgId;
    HbMessageBox::question(LOC_DLG_SAVE_RINGTONE, this,
        SLOT(onDialogSaveTone(HbAction*)), LOC_BUTTON_SAVE, LOC_BUTTON_CANCEL);
    }

// ----------------------------------------------------------------------------
// MsgServiceViewManager::handleProvisoningMsg
// @see header
// ----------------------------------------------------------------------------
void MsgServiceViewManager::handleProvisoningMsg(int msgId)
    {
    QString messageId;
    messageId.setNum(msgId);

    XQApplicationManager* aiwMgr = new XQApplicationManager();

    XQAiwRequest* request = aiwMgr->create("com.nokia.services.MDM", 
            "Provisioning",
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

//-----------------------------------------------------------------------------
//MsgServiceViewManager::handleBTMessage()
//@see header
//-----------------------------------------------------------------------------
void MsgServiceViewManager::handleBTMessage(int msgId)
    {
    XQApplicationManager* aiwMgr = new XQApplicationManager();
    XQAiwRequest* request = 
    aiwMgr->create("com.nokia.services.btmsgdispservices", "displaymsg",
            "displaymsg(int)", true); // embedded

    if (request) {
    QList<QVariant> args;
    args << QVariant(msgId);
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

//-----------------------------------------------------------------------------
//MsgServiceViewManager::onDialogDeleteMsg()
//@see header
//-----------------------------------------------------------------------------
void MsgServiceViewManager::onDialogDeleteMsg(HbAction* action)
{
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0)) {
        mStoreHandler->deleteMessage(mMessageId);
    }
    HbApplication::quit(); // exit after handling
}

//-----------------------------------------------------------------------------
//MsgServiceViewManager::onDialogSaveTone()
//@see header
//-----------------------------------------------------------------------------

void MsgServiceViewManager::onDialogSaveTone(HbAction* action)
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

