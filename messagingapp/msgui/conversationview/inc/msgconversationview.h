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
#include "convergedmessage.h"

//Forward declarations
class HbListView;
class HbAbstractViewItem;
class MsgSendUtil;
class QStandardItemModel;
class MsgEditorWidget;
class MsgContactCardWidget;
class MsgConversationViewItem;
class HbStaticVkbHost;
class QGraphicsLinearLayout;

//Defines
#define INVALID_MSG_ID -1

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
     * Clear editors
     * clear message and address editors
     */
    void clearEditors();

    /**
     * Save the content inside editor to drafts
     * @return true if save is success else false.
     */
    bool saveContentToDrafts();

private slots:

    /**
     * Slot is called when menu is about to be shown.
     * Populates the menu with relevant actions.
     */
    void menuAboutToShow();

private:

    /**
     * View initialization function.
     */
    void setupView();

    /**
     * Setup view menu items.
     */
    void setupMenu();

    /**
     * Populates ConvergedMessage for sending.
     * @param ConvergedMessage to be populated
     * @see ConvergedMessage::MessageType
     */
    void populateForSending(ConvergedMessage &message);
    
    /**
     * Adds context menu entries to context menu
     * @param HbMenu context menu
     * @param int sendingstate.
     * @see ConvergedMessage::MessageType
     */
    void setContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState);
	
    /**
     * Adds context menu entry to context menu for Opening items
	 * @param MsgConversationViewItem* item whose information is needed.
     * @param HbMenu context menu
     * @param int sendingstate.
     * @see ConvergedMessage::MessageType
     */
    void addOpenItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState);

    /**
     * Adds context menu entry to context menu for Resending items
     * @param MsgConversationViewItem* item whose information is needed.
     * @param HbMenu context menu
     * @param int sendingstate.
     * @see ConvergedMessage::MessageType
     */  
    void addResendItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState);

    /**
     * Adds context menu entry to context menu for Forwarding items
	 * @param MsgConversationViewItem* item whose information is needed.
     * @param HbMenu context menu
     * @param int sendingstate.
     * @see ConvergedMessage::MessageType
     */
    void addForwardItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState);

    /**
     * Adds context menu entry to context menu for Deleting items
	 * @param MsgConversationViewItem* item whose information is needed.
     * @param HbMenu context menu
     * @param int sendingstate.
     * @see ConvergedMessage::MessageType
     */
    void addDeleteItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState);

    /**
     * Adds context menu entry to context menu for Downloading items
     * @param MsgConversationViewItem* item whose information is needed.
     * @param HbMenu context menu    
     * @see ConvergedMessage::MessageType
     */
    void addDownloadItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu);
    
    /**
     * Validates if message can be forwarded
     * @param messageType
     * @param messageId
     * @return true if message can be forwarded
     *         false if message cant be forwarded
     */
    bool validateMsgForForward(int &messageType,qint32 &messageId);

    /**
     * Launches the BT message display service.
     * @param index Index of the item activated/touched
     */
    void launchBtDisplayService(const QModelIndex & index);

public slots:
    /**
     * Refreshes all widgets in the conversation view according to latest model
     * data
     */
    void refreshView();
    
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

    /*
     * Get the photos and launch editor
     */
    void imagesFetched(const QVariant& result );
    
    /**
     * slot to receive fetched contacts for addition
     */
    void contactsFetched(const QVariant& value);

    /*
     * Get audio files from audio-fetcher and launch editor
     */
    void audiosFetched(const QVariant& result );
    
    /**
     * slot to receive fetched contacts for vcard addition
     */
    void contactsFetchedForVCards(const QVariant& value);
    
    /*
     * Adds subject file to editor.
     */
    void addSubject();


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
     * Resends the item. This is only valid in the failed message case.
     */
    void resendMessage();

    /**
     * Download the specified message
     */
    void downloadMessage();

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
    
    /**
     * Launch Editor since sms character limit reached
     */
    void handleSmsCharLimitReached();   
    
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
    
	/**
	* This signal is emitted when vkb is open.
	*/
    void hideChrome(bool);

private slots:
    /**
     * Resizes the view when VKB is opened.
     * This slot is triggered when user taps on the CV editor
     */
    void vkbOpened();

    /**
     * Resizes the view when VKB is closed.
     * This slot is triggered when VKB focus is lost.
     */
    void vkbClosed();
    
private:
    /**
     * launch editor when attachment is inserted
     * @params data used for launching editor with pre-populated content
     */
    void launchUniEditor(const QVariantList& data);
    
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
     * Send utils instance
     */
    MsgSendUtil *mSendUtil;
 
    /**
     * Instance of the main layout.
     */
    QGraphicsLinearLayout *mMainLayout;

	/**
	 * Flag to track if item has been long pressed.
	 */
    bool mItemLongPressed;
    /*
     * Instance of VKB 
     */
    HbStaticVkbHost* mVkbHost;
    
    /**
     * Flag to check it vkb is open.
     */
    bool mVkbopened;

};

#endif // MSG_CONVERSATION_VIEW_H
// EOF
