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
 * Description: Helper class to keep record of msg type changes
 *
 */

#ifndef MSG_TYPE_NOTIFIER_H
#define MSG_TYPE_NOTIFIER_H

#include <QObject>
#include "convergedmessage.h"

class MsgTypeNotifier : public QObject
    {
    Q_OBJECT

public:

    /**
     * Constructor
     */
    MsgTypeNotifier(QObject* parent=0);

    /**
     * Destructor
     */
    ~MsgTypeNotifier();

    /**
     * Seeker method to access current message type
     */
    ConvergedMessage::MessageType messageType() const;

    /**
     * Set to skip showing note for first time
     */
    void setSkipNote();

public slots:
    /**
     * Public slot to keep MMS content count for the under construction
     * message. To be called from content widgets.
     * @param isAdded true/false
     */
    void onMMContentAdded(bool isAdded);
    
private:
    void showPopup(const QString& text);

private:
    int mmContentCount;
    bool mSkipNote;
    };

#endif //MSG_TYPE_NOTIFIER_H
