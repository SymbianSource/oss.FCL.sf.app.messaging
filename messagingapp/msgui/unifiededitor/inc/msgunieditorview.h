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
 * Description:uni editor view class.
 *
 */

#ifndef UNIFIED_EDITOR_VIEW_H
#define UNIFIED_EDITOR_VIEW_H

#ifdef UNIFIEDEDITOR_DLL
#define UNIFIEDEDITOR_EXPORT Q_DECL_EXPORT
#else
#define UNIFIEDEDITOR_EXPORT Q_DECL_IMPORT
#endif

#include "msgbaseview.h"
#include "convergedmessage.h"
#include "convergedmessageid.h"

class HbWidget;
class HbAction;
class HbGroupBox;
class QGraphicsLinearLayout;
class MsgUnifiedEditorSubject;
class MsgUnifiedEditorAddress;
class MsgUnifiedEditorBody;
class MsgTypeNotifier;
class MsgMonitor;
class MsgUiUtilsManager;
class MsgAttachmentContainer;
class UniEditorPluginLoader;

class UNIFIEDEDITOR_EXPORT MsgUnifiedEditorView : public MsgBaseView
    {
    Q_OBJECT

public:

    /**
     * Constructor
     */
    MsgUnifiedEditorView(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~MsgUnifiedEditorView();

    /**
     * Populate editor with passed information
     */
    void populateContent(const ConvergedMessage& messageDetails, 
        int editorFields = 0);
    
    /**
     * Opens the drafts message into editor
     */
    void openDraftsMessage(ConvergedMessageId& messageId,
        ConvergedMessage::MessageType messageType );


    /**
     * Saves the content inside editor to save
     */
    bool saveContentToDrafts();

protected:
    /**
     * reimplemented from base class to provide proper geometry for scrolling.
     */
    void resizeEvent( QGraphicsSceneResizeEvent * event );

private:

    /**
     * helper method to add menu.
     */
    void addMenu();

    /**
     * helper method to add tool bar.
     */
    void addToolBar();

    /**
     * helper method to get style plugin path.
     */
    QString pluginPath();

    /**
     * Packs the content inside editor into converged message
     */
    void packMessage(ConvergedMessage &msg);

    /**
     * Populate editor with Drafts msg content
     */
    void populateDraftsContent(
        const ConvergedMessage& messageDetails);

private slots:

    /**
     * slot to add subject field.
     */
    void addSubject();

    /**
     * slot to add Cc,Bcc field.
     */
    void addCcBcc();

    /**
     * slot to handle Cc, Bcc group-box's toggle action
     */
    void updateOtherRecipientCount(bool state);

    /**
     * slot to change priority.
     */
    void changePriority();

    /**
     * slot for different sending options.
     */
    void sendingOptions();

    /**
     * slot to current delete message.
     */
    void deleteMessage();

    /**
     * slot to launch help.
     */
    void launchHelp();

    /**
     * slot to show attachment popup.
     */
    void showAttachmentPopup();

    /**
     * slot to images fetched.
     */
    void imagesFetched(const QVariant& result );

    /**
     * slot to fetch audio files
     */
    void audiosFetched(const QVariant& result );

    /**
     * slot to send message.
     */
    void send();
	
    /**
     * slot to tell view to remove the attachment's container
     */
    void removeAttachmentContainer();
    
    /**
     * add an attachment to editor
     */
    void addAttachment(const QString& filepath);

private:
    HbAction* mSubjectAction;
    HbAction* mCcBccAction;
    QGraphicsLinearLayout* mMainLayout;
    MsgUnifiedEditorSubject* mSubjectField;
    MsgUnifiedEditorAddress* mToField;
    MsgUnifiedEditorAddress* mCcField;
    MsgUnifiedEditorAddress* mBccField;
    MsgUnifiedEditorBody*   mBody;

    HbMenu *mPrioritySubMenu;
    HbWidget* mContentWidget;
    QString mPluginPath;

    MsgTypeNotifier* mNotifier;
    MsgMonitor* mMsgMonitor;
    MsgUiUtilsManager* mUtilsManager;
    MsgAttachmentContainer* mAttachmentContainer;
    UniEditorPluginLoader* mPluginLoader;
    ConvergedMessageId mOpenedMessageId;
    ConvergedMessage::MessageType mmOpenedMessageType;
    };

#endif //UNIFIED_EDITOR_VIEW_H
