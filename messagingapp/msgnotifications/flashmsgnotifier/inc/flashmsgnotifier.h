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
 * Description: Implements interface for showing flash messages
 *
 */

#ifndef FLASHMSGNOTIFIER_H
#define FLASHMSGNOTIFIER_H

#include <QObject>
#include <xqserviceprovider.h>
#include <qvariant.h>
#include <qbytearray.h>

class FlashMsgNotifierPrivate;
class XQSystemToneService;

/**
 * @class FlashMsgNotifier
 * Implements interface for showing flash messages
 */
class FlashMsgNotifier: public XQServiceProvider
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    FlashMsgNotifier(QObject *parent = 0);

    /**
     * Destructor
     */
    ~FlashMsgNotifier();

private:
    /**
     *  onDeleteAction
     *  @param msgId message Id
     */
    void onSaveAction(int msgId);  
    
    /**
     *  onDeleteAction
     *  @param msgId message Id
     */
    void onDeleteAction(int msgId);    

public slots:

    /**
     * displayFlashMsg 
     * @param displayParams QVariantList 
     */
    void displayFlashMsg(QByteArray displayParams);

private:

    /**
     * FlashMsgNotifierPrivate obj
     * Owned.
     */
    FlashMsgNotifierPrivate* d_ptr;
    
    /**
     * XQSystemToneService obj
     * Owned.
     */
    XQSystemToneService* mSts;
  
    /**
     * Address string
     */
    QString mAddress;
};

#endif // MSGERRORNOTIFIER_H
