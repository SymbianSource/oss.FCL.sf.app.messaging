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

#include <qmobilityglobal.h>
#include "msgbaseview.h"
#include "convergedmessage.h"
#include "convergedmessageid.h"

QTM_BEGIN_NAMESPACE
QTM_END_NAMESPACE
QTM_USE_NAMESPACE

class HbWidget;
class HbAction;
class HbGroupBox;
class QGraphicsLinearLayout;
class MsgUnifiedEditorSubject;
class MsgUnifiedEditorAddress;
class MsgUnifiedEditorBody;
class MsgMonitor;
class MsgAttachmentContainer;
class UniEditorPluginLoader;
class HbListWidgetItem;

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
     * @param editorData data for editor's usage
     */
    void populateContent(const QVariantList& editorData);
    
    /**
     * Opens the drafts message into editor
     * @param editorData data for editor's usage
     */
    void openDraftsMessage(const QVariantList& editorData);


    /**
     * Saves the content inside editor to save
     */
    void saveContentToDrafts();
    
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
     * Populate editor with prepopulated msg content
     */
    void populateContentIntoEditor(
        const ConvergedMessage& messageDetails);
    
    /**
     * Populate the editor with the forwarded message's content
     */
    void forwardMessage(ConvergedMessageId& messageId,
        ConvergedMessage::MessageType messageType );
    
    /**
     * Create VCards for all the contacts available in the list
     * @param value list of fetched contacts
     * @param [OUT]filelist list of newly added vcard files
     * @return errId KErrNone if no error
     */
    int createVCards(const QVariant& value, QStringList& filelist);
    
    /**
     * Resolve name conflict when adding attachments to editor
     * @param suggestedName suggested file name to use
     * @return fully qualified path of the file in unieditor's temp folder
     */
    QString generateFileName(QString& suggestedName);
    
    /**
     * Fetch images 
     */
    void fetchImages();

    /**
     * Fectch conatcts
     */
    void fetchContacts();

    /**
     * Fectch audio
     */
    void fetchAudio();

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
     * slot to images fetched.
     */
    void imagesFetched(const QVariant& result );

    /**
     * slot to fetch audio files
     */
    void audiosFetched(const QVariant& result );
    
    /**
     * slot to receive fetched contacts
     */
    void contactsFetched(const QVariant& value);

    /**
     * slot to send message.
     */
    void send();
	
    /**
     * slot to tell view to remove the attachment's container
     */
    void removeAttachmentContainer();

    /**
     * add attachments to the editor
     * @param files list of file paths
     */
    void addAttachments(QStringList files);

    /**
     * add an attachment to the editor
     * @return addition operation status
     */
    int addAttachment(const QString& filepath);

	/**
     * called when extention item is clicked.
     */
    void handleViewExtnActivated(HbListWidgetItem* item);
    
    /**
     * Signal emitted when an error is generated.
     * @param errorCode Error code.
     * @param errorMessage Error description.
     */
    void serviceRequestError(int errorCode, const QString& errorMessage);
   
    /**
     * Activate Input Blocker
     */
    void activateInputBlocker();

    /**
     * Deactivate Input Blocker
     */
    void deactivateInputBlocker();

private:
    HbAction* mSubjectAction;
    HbAction* mCcBccAction;
    QGraphicsLinearLayout* mMainLayout;
    MsgUnifiedEditorSubject* mSubjectField;
    MsgUnifiedEditorAddress* mToField;
    MsgUnifiedEditorAddress* mCcField;
    MsgUnifiedEditorAddress* mBccField;
    MsgUnifiedEditorBody*   mBody;

    HbWidget* mContentWidget;
    QString mPluginPath;

    MsgMonitor* mMsgMonitor;
    MsgAttachmentContainer* mAttachmentContainer;
    UniEditorPluginLoader* mPluginLoader;
    ConvergedMessageId mOpenedMessageId;
    ConvergedMessage::MessageType mmOpenedMessageType;
	bool mCanSaveToDrafts;
	friend class MsgMonitor;
    };

#endif //UNIFIED_EDITOR_VIEW_H
