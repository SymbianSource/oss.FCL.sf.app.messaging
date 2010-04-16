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
 * Description: Message Application service interface used for interfacing between
 *              QT highway and other applications
 *
 */

#ifndef MSGSERVICEINTERFACE_H_
#define MSGSERVICEINTERFACE_H_

// INCLUDES
#include <xqserviceprovider.h>
#include <qvariant.h>
#include <qmobilityglobal.h>

//TODO: to be deprecated
#include <QStringList>
class ConvergedMessageAddress;

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

struct ContactDetail
{
public:
    /**
     * Constructor
     */
    ContactDetail():contactId(-1),firstName(QString()),
                    lastName(QString()){};
public:
    int contactId;
    QString firstName;
    QString lastName;
    QStringList addressList;
};

class MsgViewManager;
class MsgServiceInterface : public XQServiceProvider
    {
    Q_OBJECT
    
public:
    /*
     * Constructor
     */
    MsgServiceInterface(QObject* parent=0, MsgViewManager* manager =0);
    
    /*
     * Destructor
     */
    ~MsgServiceInterface();
    
public slots:
    
    /**
     * send called from phone book.
     */
    void send(const QString phoneNumber, const qint32 contactId, const QString displayName);
    
    /**
     * called from indications.
     */
    void open(qint64 conversationId);
    
    /**
     * called from send UI.
     */
    void send(QVariant data);
    
    /**
     * depricated api. will be removed in future.
     */
    void openConversationView(QString number, QString name);
    
signals:

private:
    //TODO: to be deprecated
    bool resolveContact(const ConvergedMessageAddress &address,
            ContactDetail &contactDetail);
 
private:
MsgViewManager* mViewManager;
    
    };


#endif /* MsgServiceInterface_H_ */
