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

#ifndef CONVERSATIONS_ENGINE_H
#define CONVERSATIONS_ENGINE_H

// INCLUDES
#include <QObject>
#include <QStandardItemModel>

#ifdef BUILD_DLL
#define CONVERSATIONS_ENGINE_API_EXPORT Q_DECL_EXPORT
#else
#define CONVERSATIONS_ENGINE_API_EXPORT Q_DECL_IMPORT
#endif

class ConversationsSummaryModel;
class ConversationsModel;
class DraftsModel;
class ConversationMsgStoreHandler;
class ConversationsEnginePrivate;

/**
 * This is a singleton class which
 * implements the models for the model/view framework.
 * The models hold data for the conversations fetched from the
 * server.
 *
 */

class CONVERSATIONS_ENGINE_API_EXPORT ConversationsEngine : public QObject
{
    Q_OBJECT

public:

    /**
     * Returns pointer to sole instance.
     * @return ConversationEngine object
     */
    static ConversationsEngine* instance();

    /**
     * Destructor
     */
    virtual ~ConversationsEngine();

public:

    /**
     * Returns Conversations Summary Model instance
     */
    QStandardItemModel* getConversationsSummaryModel();

    /**
     * Returns Conversations Model instance
     */
    QStandardItemModel* getConversationsModel();

    /**
     * Returns drafts Model instance
     */
    QStandardItemModel* getDraftsModel();

    /**
     * Get all conversations.
     * @param conversationId, conversationId
     * @return returns true if call is successful
     */
    bool getConversations(qint64 conversationId);
	
    /**
     * Deregisters the current conversationId and clears the model
     * @return returns true if call is successful
     */
    bool clearConversations();

    /**
     * Delete all conversations.
     * @param conversationId, conversationId
     * @return returns true if call is successful
     */    
    bool deleteConversations(qint64 conversationId);

    /**
     * Delete all messages.
     * @param msgIdList, message list.
     */  
    void deleteMessages(QList<int>& msgIdList);

    /**
     * Marks all the unread conversations to read
     * @param conversationId, conversationId
     * @return returns true if call is successful
     */        
    bool markConversationRead(qint64 conversationId);
    
    /**
     * Mark messages as read.
     * @param msgIdList, message list.
     * @return returns true if call is successful
     */      
    bool markMessagesRead(QList<int>& msgIdList);
    
    /**
     * Get Contact details firstname/lastname.
     * @param conversationId, conversationId
     * @param firstName, First Name
     * @param lastName, Last Name
     */  
    void getContactDetails(qint64 conversationId,
        QString& firstName,
        QString& lastName,
        QString& address);

    /**
     * Get conversationId for a given address.
     * @param address, Phone number
     * @return conversation id
     */  
    qint64 getConversationIdFromAddress(QString address);

    /**
     * Get current conversation id.
     */  
    qint64 getCurrentConversationId();

    /**
     * Get conversationId for a given contactId.
     * @param contactId, Contact Id
     * @return conversation id
     */ 
    qint64 getConversationIdFromContactId(qint32 contactId);

    /**
     * Emits conversationModelPopulated signal
     */ 
    void emitConversationModelPopulated();
    
    /**
     * Emits conversationModelUpdated signal
     */ 
    void emitConversationModelUpdated();
    
    /**
     * Emits conversationListModelPopulated signal
     */ 
    void emitConversationListModelPopulated();

    /**
     *  Starts fetching remaing conversations
     */
    void fetchMoreConversations();
    
private:

    /**
     * Constructor
     */
    explicit ConversationsEngine(QObject* parent = 0);    
    
signals:

    /**
     * ConversationListModelPopulated signal indicating 
     * that the model is ready to be displayed
     */ 
    void conversationModelPopulated();
    
    /**
     * ConversationModelUpdated signal indicating dynamic 
     * updates in the model
     */ 
    void conversationModelUpdated();
    
    /**
     * ConversationListModelPopulated signal indicating 
     * that the model is ready to be displayed
     */
    void conversationListModelPopulated();
    
private:

    /**
     * Conversations Summary Model
     * Own
     */        
    ConversationsSummaryModel* mConversationsSummaryModel;

    /**
     * Conversations Model
     * Own
     */    
    ConversationsModel* mConversationsModel;

    /**
     * Drafts Model
     * Own
     */    
    DraftsModel* mDraftsModel;

    /**
     * MsgStoreHandler
     * Own
     */    
    ConversationMsgStoreHandler* mConversationMsgStoreHandler;
    
    /**
     * ConversationsEnginePrivate
     * Own
     */
    ConversationsEnginePrivate* d_ptr;

};

#endif // CONVERSATIONS_ENGINE_H
