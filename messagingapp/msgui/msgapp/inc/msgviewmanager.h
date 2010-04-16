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

#ifndef MSGVIEWMANAGER_H_
#define MSGVIEWMANAGER_H_

#include <QObject>
#include <QVariant>

class HbMainWindow;
class MsgUnifiedEditorView;
class MsgListView;
class MsgConversationBaseView;
class UnifiedViewer;
class MsgBaseView;
class DraftsListView;
class MsgSettingsView;
class HbAction;

class MsgViewManager: public QObject
    {
    Q_OBJECT   
   
public:
    /**
     * constructor
     */
    MsgViewManager(bool serviceRequest, HbMainWindow* mainWindow, QObject* parent = 0);
    
    /**
     * Destructor.
     */
    ~MsgViewManager();
    
    /**
     * call back function will be called from MsgServiceInterface.
     */
    void send(const qint32 contactId, const QString phoneNumber, const QString displayName);
    
    /**
     * call back function called from MsgServiceInterface to set service request flag.
     */
    void setServiceRequest(bool request);
    
    /**
     * call back function called MsgServiceInterface.
     */
    void open(qint64 conversationId);
    
    /**
     * call back function called MsgServiceInterface.
     */
    void send(QVariant data);
    
    /**
     * depricated api. will be removed in future.
     */
    void openEditor(QString number, QString name);
    
private:
    /**
     * swiches back to last view after service request is complete.
     */
    void switchToLastSavedView();
    
    /**
     * find conversation id corresponding to given phone no.
     * @param phoneNum
     */
    qint64 findConversationId(const QString phoneNum);
    
    /**
     * helper method to complete back action.
     */
    void completeServiceRequest();

    /*
     * switch to clv.
     */
    void switchToClv(const QVariantList& data);

    /*
     * switch to cv.
     */
    void switchToCv(const QVariantList& data);

    /*
     * switch to dlv.
     */
    void switchToDlv(const QVariantList& data);

    /*
     * switch to unieditor.
     */
    void switchToUniEditor(const QVariantList& data);

    /*
     * switch to univiewer.
     */
    void switchToUniViewer(const QVariantList& data);

    /*
     * switch to settings.
     */
    void switchToMsgSettings(const QVariantList& data);

    /*
     * handle other cases.
     */
    void handleDefault(const QVariantList& data);

private slots:
    /**
     * this slot is called on mainwindows back action.
     */
    void onBackAction();
    
    /**
     * This slot is called when switchView signal is emitted from a view.
     */
    void switchView(const QVariantList& data);
    
private:
    /**
     * main window reference not owned.
     */
    HbMainWindow* mMainWindow;
    
    /**
     * different views.
     */
    MsgUnifiedEditorView* mUniEditor;
    MsgListView* mListView;
    MsgConversationBaseView* mConversationView;
    UnifiedViewer* mUniViewer;
    DraftsListView* mDraftsListView;
    MsgSettingsView* mSettingsView;
    HbAction* mBackAction;

    int mPreviousView;
    int mCurrentView;
    int mViewAtServiceRequest;
    bool mServiceRequest;
    qint64 mConversationId;
    };

#endif /* MSGVIEWMANAGER_H_ */
