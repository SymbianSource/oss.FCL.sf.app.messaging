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
 * Description: Message Indicator class
 *
 */

#ifndef MSGINDICATOR_H
#define MSGINDICATOR_H

#include <QObject>

#include <hbindicatorinterface.h>

class MsgIndicatorPrivate;

/**
 * Utility class used pass indication data 
 * from private to Qt- class.
 */
class IndicatorData
    {
public:
    /**
     * Constructor
     */
    IndicatorData():mFromSingle(false),
    mConversationId(-1),
    mUnreadMsgCount(0){};
    
    /**
     * Message from single contact or many
     */
    bool mFromSingle; 
    
    /**
     * Conversation id
     */
    int mConversationId; 
    
    /**
     * Unread message count
     */
    int mUnreadMsgCount;
    
    /**
     * First name
     */
    QString mFirstName; 
    
    /**
     * Last name
     */
    QString mLastName; 
    
    /**
     * Nick name
     */
    QString mNickName; 
    
    /**
     * Phone number
     */
    QString mContactNum; 
    
    /**
     * Description
     */
    QString mDescription;
    };

/**
 * Message indicator class. 
 * Handles client request and showing the indications. 
 */
class MsgIndicator : public HbIndicatorInterface
{
public:
    /**
     * Constructor
     */
    MsgIndicator(const QString &indicatorType);
    
    /**
     * Destructor
     */
    ~MsgIndicator();
    
    /**
     * @see HbIndicatorInterface
     */
    bool handleInteraction(InteractionType type);
    
    /**
     * @see HbIndicatorInterface
     */
    QVariant indicatorData(int role) const;
    
protected:
    /**
     * @see HbIndicatorInterface
     */
    bool handleClientRequest(RequestType type, const QVariant &parameter);
    
private: 
    /**
     * Prepares the display name from stream.
     * @param IindicatorData inidcator data.
     */
    QString prepareDisplayName(IndicatorData& indicatorData) const;
    
private:
    
    /**
     * Body text
     */
    mutable QString mBodyText;
    
    /**
     * Conversation id
     */
    mutable qint64 mConversationId;
    
    /**
     * Message from single or multiple contact
     */
    mutable bool mConversationFromSingleContact;
    
    /**
     * Private implementaion object.
     * Owned.
     */
    MsgIndicatorPrivate* d_ptr;
};

#endif // MSGINDICATOR_H

