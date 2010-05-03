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
 */

#include "testsmsplugin.h"
#include <QtTest/QtTest>
#include "debugtraces.h"

#include <QTimer>
#include <QSignalSpy>

#include "convergedmessage.h"
#include "TestMsg.h"
#include "convergedmessageid.h"
#include "unieditorsmsplugin.h"
#include "messageserviceaction.h"
//---------------------------------------------------------------
// TestSmsPlugin::initTestCase
//---------------------------------------------------------------
void TestSmsPlugin::initTestCase()
{
    msgPlugin = new UniEditorSmsPlugin();
    testMsg = new TestMsg();
  //  QVERIFY(msgPlugin != 0);
    smsId = -1;
    mmsId = -1;
}

//---------------------------------------------------------------
// TestSmsPlugin::cleanupTestCase
//---------------------------------------------------------------
void TestSmsPlugin::cleanupTestCase()
{
    delete msgPlugin;
    delete testMsg;
}

//---------------------------------------------------------------
// TestSmsPlugin::init
//---------------------------------------------------------------

void TestSmsPlugin::init()
{
}

//---------------------------------------------------------------
// TestSmsPlugin::cleanup
//---------------------------------------------------------------
void TestSmsPlugin::cleanup()
{
}

//---------------------------------------------------------------
// TestSmsPlugin::testSendSMS
//---------------------------------------------------------------
void TestSmsPlugin::testSendReceiveSMS()
{ 
    testMsg->createSCSettings();
    
    testMsg->initSimTsyL();
    //TODO: Should uncomment the watcher code below while executing the test case in text shell mode. 
    //Also the watcher code should be commented while executing in UI mode.
  /*  RProcess watcherProcess;
    _LIT(KWatcherExe, "z:\\system\\libs\\watcher.exe");
    User::LeaveIfError(watcherProcess.Create(KWatcherExe, KNullDesC));
    watcherProcess.Resume();
    QTest::qWait(5000);*/
    TBool result = msgPlugin->validateService(ETrue);
    result = msgPlugin->validateService(EFalse);
    result = msgPlugin->isServiceValid();
    createAndSend(msgPlugin->messageType());
    testReceived(msgPlugin->messageType());
}

//---------------------------------------------------------------
// TestSmsPlugin::createAndSend
//---------------------------------------------------------------
void TestSmsPlugin::createAndSend(int type)
{

    ConvergedMessage message;
    message.setMessageType((ConvergedMessage::MessageType) type);
    message.setBodyText(QString(TEST_MSG_BODY));
    ConvergedMessageAddress address1;
    address1.setAddress(QString(TEST_MSG_TO3));
    
    message.addToRecipient(address1);
    message.setPriority(ConvergedMessage::Normal);
    if (type == ConvergedMessage::Mms)
    {
        ConvergedMessageAddress address2;
        address2.setAddress(QString(TEST_MSG_FROM2));
        address2.setAlias(QString(TEST_MSG_ALIAS2));
        message.addCcRecipient(address2);
        message.addBccRecipient(address2);

        message.setSubject(QString(TEST_MSG_SUBJECT));
        message.setPriority(ConvergedMessage::Normal);
    }

    int ret = -1;
    QBENCHMARK 
    {
    long int id = msgPlugin->convertTo(&message);
    msgPlugin->send(id);
    }
}
	
//---------------------------------------------------------------
// TestSmsPlugin::testReceived
//---------------------------------------------------------------
void TestSmsPlugin::testReceived(int type)
{
MessageServiceAction* messageAction = new MessageServiceAction(this);    
    QVERIFY(messageAction != 0);

    //register user defined object to meta system.
    qRegisterMetaType<ConvergedMessage*>("ConvergedMessage*");

    //signal spy.
    //listen to added event
    QSignalSpy spyAdd( messageAction,
            SIGNAL(messageAdded(ConvergedMessage*, QString)));
    //signal spy.
    //listen to updated event
        QSignalSpy spyUpdate( messageAction,
                SIGNAL(messageUpdated(ConvergedMessage*, QString)));
    //will force stub_clientmanager to emit messageUpdated.
    messageAction->registerForNotification();
    //register user defined object to meta system.

    // wait for the response to come
    QTest::qWait(5000);

    //check signals
    int countAdd = -1;
    countAdd = spyAdd.count();

    ConvergedMessage* message = 0;
    QString serviceId("");
    
    if (countAdd)
    {
        //verify added event
        void * temp = const_cast<void*> (spyAdd.at(0).at(0).data());
        message = *reinterpret_cast<ConvergedMessage*(*)> (temp);
        serviceId = spyAdd.at(0).at(1).toString();
        QVERIFY(message != 0);
        validateMsg(*message, serviceId);
    }

    int countUpdate = -1;
    countUpdate = spyUpdate.count();
    message = 0;

    for (int i = countUpdate; i > 0; --i)
    {
        //verify updated event
        void * temp = const_cast<void*> (spyUpdate.at(i-1).at(0).data());
        message = *reinterpret_cast<ConvergedMessage*(*)> (temp);
        if (ConvergedMessage::Inbox == message->location())
        {
            serviceId = spyUpdate.at(i-1).at(1).toString();
            break;
        }
    }
    if (message)
    {
        validateMsg(*message, serviceId);
    }
    delete message;
}

//---------------------------------------------------------------
// TestSmsPlugin::validateMsg
//---------------------------------------------------------------
void TestSmsPlugin::validateMsg(const ConvergedMessage& message,
                                const QString& serviceId)
{
#ifdef _DEBUG_TRACES_
		qDebug() << "....message received.....";
#endif

    //process incoming messages
    if (message.location() == ConvergedMessage::Inbox)
    {
        int msgId;
        if (message.id())
        {
            msgId = message.id()->getId();
        }
        ConvergedMessageAddressList array = message.toAddressList();
        if (array.count() > 0)
        {
            if (array[0])
                QCOMPARE(array[0]->address(),QString(TEST_MSG_FROM1));
        }
        ConvergedMessageAddress *fromAddr = message.fromAddress();
        if (fromAddr)
        {
            QCOMPARE(fromAddr->address(),QString(TEST_MSG_FROM1));
        }

        QCOMPARE(serviceId,QString(TEST_SERVICE_ID));

        if (message.messageType() == ConvergedMessage::Mms)
        {
            mmsId = msgId;
            QCOMPARE(message.subject(), QString(TEST_MSG_SUBJECT));
        }
        else if (message.messageType() == ConvergedMessage::Sms)
        {
            smsId = msgId;
            QCOMPARE(message.bodyText(), QString(TEST_MSG_BODY));
        }
    }
    else if (message.location() == ConvergedMessage::Sent)
    {
        ConvergedMessageAddressList array = message.toAddressList();
        if (array.count() > 0)
        {
            if (array[0])
                QCOMPARE(array[0]->address(),QString(TEST_MSG_FROM1));
        }

        QCOMPARE(serviceId,QString(TEST_SERVICE_ID));

        if (message.messageType() == ConvergedMessage::Mms)
        {
            QCOMPARE(message.subject(), QString(TEST_MSG_SUBJECT));
        }
        else if (message.messageType() == ConvergedMessage::Sms)
        {
            QCOMPARE(message.bodyText(), QString(TEST_MSG_BODY));
        }
    }
}
