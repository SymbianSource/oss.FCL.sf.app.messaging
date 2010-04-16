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
 * Description:Message chat View for the messaging application.
 *
 */

#ifndef MSG_CONVERSATION_VIEW_H
#define MSG_CONVERSATION_VIEW_H

// INCLUDES
#include "msgbaseview.h"

//Forward declarations
class HbListView;
class HbAbstractViewItem;
class MsgUiUtilsManager;
class ConvergedMessage;
class QStandardItemModel;
class MsgEditorWidget;
class MsgContactCardWidget;

/**
 * This class provides the message chat view for the messaging application.
 * Data source for this view is the conversation model.
 */
class MsgConversationView : public MsgBaseView
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    MsgConversationView(MsgContactCardWidget *contactCardWidget,
        QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~MsgConversationView();

    /**
     * Set message data
     * @param message converged message
     */
    void setMessageData(const ConvergedMessage &message);

    /**
     * Clear editors
     * clear message and address editors
     */
    void clearEditors();

    /**
     * Save the content inside editor to drafts
     * @return true if save is success else false.
     */
    bool saveContentToDrafts();

public:

    /**
     * View initialization function.
     */
    void setupView();

    /**
     * Creates view menu items.
     */
    void addMenu();

public slots:

    /**
     * Slot to receive error sign.
     * @param error The error as a string to be displayed on the screen.
     */

    void serviceError(const QString& error);
   
private:
    /**
     * Load command plugin
     * @return true if plugin is loaded successfully else false
     */
    bool loadCommandPlugin();

    /**
     * This function shall parse the service Id and check for
     * type of client
     * @param serviceId string reference to service id
     * @return String type the server name
     */
    QString parseServiceId(const QString &serviceId);

    /**
     * Returns the current message type selected in editor.
     * @return returns message type
     * @see ConvergedMessage::MessageType
     */
    int currentMessageType();

    /**
     * Populates ConvergedMessage for sending.
     * @param ConvergedMessage to be populated
     * @see ConvergedMessage::MessageType
     */
    void populateForSending(ConvergedMessage &message);

public slots:
    /**
     * Refreshes all widgets in the conversation view according to latest model
     * data
     */
    void refreshView();

    /**
     * Effect finish handler
     * data
     */
    void effectFinished(const HbEffect::EffectStatus &status);
    
private slots:

    /**
     * Utility method to scroll the list to show the bottom most item
     */
    void scrollToBottom();

    /**
     * Handler for long tap of a list item.
     * @param viewItem  Long tapped view item
     * @param point X and Y co-ordinates of long tap
     */
    void longPressed(HbAbstractViewItem* viewItem, const QPointF& point);

    /**
     * Handler for short tap/direct touch of a list item.
     * @param index index of the item activated/touched
     */
    void openItem(const QModelIndex & index);

    /**
     * Sends the message
     */
    void send();

    // ----------------------------- View Specific Menu Slots--------------------//

    /**
     * Deletes all the conversations
     */
    void deleteAll();

    /*
     * Get the photos and launch editor
     */
    void imagesFetched(const QVariant& result );

    /*
     * Get audio files from audio-fetcher and launch editor
     */
    void audiosFetched(const QVariant& result );

    /*
     * Adds other recipients to editor
     */
    void addRecipients();

    /*
     * Adds subject file to editor.
     */
    void addSubject();

    /**
     * Launches help
     */
    void launchHelp();

    /**
     * Exits the application
     */
    void exitApp();

    //----------------------Item Specific menu slots ---------------------//

    /**
     * Forwards the message
     */
    void forwardMessage();

    /**
     * Deletes the item
     */
    void deleteItem();

    /**
     * Open the item
     */
    void openItem();

    /**
     * Set model to the view 
     */    
    void populateConversationsView();

    /**
     * Save ringing tone
     */
    void saveRingingTone();
    
signals:
    /**
     * Signal emitted to inform close the conversation view.
     * When Conversation view has opened in send mode, after sending
     * the message, this signal is emitted.
     */
    void closeConversationView();

    /**
     * Emitted when editor is tapped to reply.
     */
    void replyStarted();

private:
    /**
     * launch editor when attachment is inserted
     * @params eflag Editor content to be shown
     * @params attachment file attachment for editor
     */
    void launchUniEditor(const int editorFields, QString& attachment);

private:

    /**
     * List to hold the conversations
     * Owned
     */
    HbListView *mConversationList;

    /**
     * Model to facilitate filtering and sorting
     * Owned
     */
    QStandardItemModel *mMessageModel;

    /**
     * Instance of messaged editor widget.
     */
    MsgEditorWidget *mEditorWidget;

    /**
     * Instance of Contact Card widget.
     */
    MsgContactCardWidget* mContactCardWidget;

    /**
     * Utils Manager instance
     */
    MsgUiUtilsManager* mUiUtilsManager;
};

#endif // MSG_CONVERSATION_VIEW_H
// EOF
