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
 * Description: Message send service interface used for interfacing between
 *              QT highway and other applications.
 *
 */

#ifndef MSGSENDINTERFACE_H_
#define MSGSENDINTERFACE_H_

// INCLUDES
#include <xqserviceprovider.h>

// FORWARD DECLARATIONS
class MsgServiceViewManager;

class MsgSendInterface : public XQServiceProvider
    {
    Q_OBJECT
    
public:
    /*
     * Constructor
     */
    MsgSendInterface(MsgServiceViewManager* viewManager);
    
    /*
     * Destructor
     */
    ~MsgSendInterface();
    
public slots:
    
    /**
     * Send message.
     * @param phoneNumber phone number.
     * @param contactId contactId.
     * @param displayName display name.
     */
    void send(const QString phoneNumber, const qint32 contactId, 
            const QString displayName);
    
    /**
     * Send message.
     * @param phoneNumber phone number.
     * @param alias alias name.
     * @param bodyText body text.
     */
    void send(const QString phoneNumber, const QString alias, 
                            const QString bodyText);

private:
    
    /**
     * Pointer to view manager
     * Not owned.
     */
    MsgServiceViewManager* mViewManager;
    };


#endif /* MSGSENDINTERFACE_H_ */
