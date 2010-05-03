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
 *
 */

#ifndef CONVERSATIONS_MODEL_H
#define CONVERSATIONS_MODEL_H

// INCLUDES
#include <QObject>
#include <QStandardItemModel>
#include <ccsdefs.h>

// FORWARD DECLARATIONS
class CCsConversationEntry;
class ConversationMsgStoreHandler;
class UniDataModelLoader;
class UniDataModelPluginInterface;

/**
 * This class implements the model of the model/view framework.
 * The model holds data for the conversations fetched from the
 * server.
 *
 * Data from server is mapped to UI via Model
 *
 */

class ConversationsModel: public QStandardItemModel
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit ConversationsModel(ConversationMsgStoreHandler* msgStoreHandler, QObject* parent = 0);

    /**
     * Destructor
     */
    virtual ~ConversationsModel();

    /**
     * Method for mapping the data in the Model to the UI
     * @param index index for which data needs to be fetched
     * @param role defined for the data
     * @return QVariant as the data
     */
    QVariant data(const QModelIndex& index, int role) const;

    /**
     * Add a new row to the conversations model.
     * Overwrite the row if the message already exists.
     * @param entry, CCsConversationEntry
     * @param dynamicUpdate , true if it is a dynamic update event
     */
    void addRow(const CCsConversationEntry& entry, bool dynamicUpdate = false);

    /**
     * Delete a row from conversations model.
     * @param msgId, Message Id
     */
    void deleteRow(int msgId);

private:

    /**
     * Populate an item in model from CCsConversationEntry.
     * @param item, QStandardItem
     * @param entry, CCsConversationEntry
     */
    void populateItem(QStandardItem& item, const CCsConversationEntry& entry);

    /**
     * Populate MMS related data into item.
     * @param item, QStandardItem
     * @param entry, CCsConversationEntry
     */
    void handleMMS(QStandardItem& item, const CCsConversationEntry& entry);
    
    /**
     * Populate MMS Notification related data into item.
     * @param item, QStandardItem
     * @param entry, CCsConversationEntry
     */
    void handleMMSNotification(QStandardItem& item, 
        const CCsConversationEntry& entry);

    /**
     * Populate BT message related data into item.
     * @param item, QStandardItem
     * @param entry, CCsConversationEntry
     */
    void handleBlueToothMessages(QStandardItem& item, 
        const CCsConversationEntry& entry);

    /**
     * Populate Bio message related data into item.
     * @param item, QStandardItem
     * @param entry, CCsConversationEntry
     */
    void handleBioMessages(QStandardItem& item, 
        const CCsConversationEntry& entry);

private:

    /**
     * Msg Store Handler
     * Not Own
     */
    ConversationMsgStoreHandler* mMsgStoreHandler;

    /**
     * UniDataModelLoader object
     * Own
     */
    UniDataModelLoader* iDataModelPluginLoader;

    /**
     * Mms Data model plugin interface
     * Not Own
     */
    UniDataModelPluginInterface* iMmsDataPlugin;

    /**
     * BioMsg Data model plugin interface
     * Not Own
     */
    UniDataModelPluginInterface* iBioMsgPlugin;
};

#endif // CONVERSATIONS_MODEL_H
