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

#include "testmmsplugin.h"
#include <QtTest/QtTest>
#include "debugtraces.h"

#include <QTimer>
#include <QSignalSpy>

#include "convergedmessage.h"
#include "TestMsg.h"
#include "convergedmessageid.h"
#include "unieditormmsplugin.h"
#include "messageserviceaction.h"
//---------------------------------------------------------------
// TestMmsPlugin::initTestCase
//---------------------------------------------------------------
void TestMmsPlugin::initTestCase()
{
    msgPlugin = new UniEditorMmsPlugin();
    QVERIFY(msgPlugin != 0);
    smsId = -1;
    mmsId = -1;
}

//---------------------------------------------------------------
// TestMmsPlugin::cleanupTestCase
//---------------------------------------------------------------
void TestMmsPlugin::cleanupTestCase()
{
    delete msgPlugin;
}

//---------------------------------------------------------------
// TestMmsPlugin::init
//---------------------------------------------------------------

void TestMmsPlugin::init()
{
}

//---------------------------------------------------------------
// TestMmsPlugin::cleanup
//---------------------------------------------------------------
void TestMmsPlugin::cleanup()
{
}

//---------------------------------------------------------------
// TestMmsPlugin::testSendMMS
//---------------------------------------------------------------

void TestMmsPlugin::testSendMMS()
{

    //register user defined object to meta system.
    qRegisterMetaType<ConvergedMessage*> ("ConvergedMessage*");
    

    createMMSMessageWithAttachmentAndSend();
     //wait for the response to complete 
     //as sending a message goes through different stages
     
    //Create a smil attachment with image and text
    createMMSMessageWithSmilTextAndImageAttachmentAndSend();
   
    //Create MMS with Normal priority
    createMMSMessageWithAttachmentAndSend(ETrue);
}


void TestMmsPlugin::createMMSMessageWithSmilTextAndImageAttachmentAndSend()
    {
    QString service = TEST_SERVICE_NAME_MMS;
    QString bodyText = TEST_MSG_BODY;
    QString subject  = TEST_MSG_SUBJECT;
    qint64 timeStamp = QDateTime::currentDateTime().toTime_t();
    
    QString sender(TEST_SENDER);
    ConvergedMessageAddress address(sender);
    ConvergedMessageAttachmentList attachmentList;
  
    ConvergedMessageAttachment* attachmentImage = 
        new ConvergedMessageAttachment(TEST_ATTACHMENT4, ConvergedMessageAttachment::EAttachment);
    attachmentList.append(attachmentImage);
    
    ConvergedMessageAttachment* attachmenttext = 
            new ConvergedMessageAttachment(TEST_ATTACHMENT5, ConvergedMessageAttachment::EAttachment);
    attachmentList.append(attachmenttext);
    
    ConvergedMessageAttachment* attachmentSmil = 
                new ConvergedMessageAttachment(TEST_ATTACHMENT3, ConvergedMessageAttachment::ESmil);
    attachmentList.append(attachmentSmil);
     
    
    
    ConvergedMessage msg;
    msg.setMessageType(ConvergedMessage::Mms);
    msg.setSubject(subject);
    msg.setTimeStamp(timeStamp);
    msg.addToRecipient(address);
    msg.addAttachments(attachmentList);
    msg.setPriority(ConvergedMessage::Low);
    
    long int id = msgPlugin->convertTo(&msg);
    msgPlugin->send(id);

    }


//---------------------------------------------------------------
// TestMmsPlugin::testMmsReceived
//---------------------------------------------------------------
void TestMmsPlugin::testMmsReceived()
{
    testReceived(msgPlugin->messageType());  
    QTest::qWait(5000);
}

/*
 * Creates a Smil file with only a text document as attachment
 */
 
void TestMmsPlugin::createMMSMessageWithAttachmentAndSend(TBool priority)
    {
    QString service = TEST_SERVICE_NAME_MMS;
    QString bodyText = TEST_MSG_BODY;
    QString subject  = TEST_MSG_SUBJECT;
    qint64 timeStamp = QDateTime::currentDateTime().toTime_t();
    
    QString sender(TEST_SENDER);
    ConvergedMessageAddress address(sender);
    ConvergedMessageAttachmentList attachmentList;    

   
    ConvergedMessageAttachment* attachment2 = 
            new ConvergedMessageAttachment(TEST_ATTACHMENT2, ConvergedMessageAttachment::ESmil);
    
    ConvergedMessageAttachment* attachment = 
        new ConvergedMessageAttachment(TEST_ATTACHMENT, ConvergedMessageAttachment::EAttachment);
   
    attachmentList.append(attachment);
    
    attachmentList.append(attachment2);
    
    ConvergedMessage msg;
    msg.setMessageType(ConvergedMessage::Mms);
    msg.setSubject(subject);
    msg.setTimeStamp(timeStamp);
    msg.addToRecipient(address);
    msg.addAttachments(attachmentList);
    if(priority == EFalse)
        {
        msg.setPriority(ConvergedMessage::Normal);
        }
    else
        {
        msg.setPriority(ConvergedMessage::High);
        }
    
    //Adding alias
    ConvergedMessageAddress address1;
    address1.setAlias(QString(TEST_MSG_RECIEPIENT));
    msg.addToRecipient(address1);
    
    //Adding CC Address
    QString ccAddress(TEST_CC);
    ConvergedMessageAddress ccAdd(ccAddress);
    msg.addCcRecipient(ccAdd);
    
    //Adding BCC Address
    QString bccAddress(TEST_BCC);
    ConvergedMessageAddress bccAdd(bccAddress);
    msg.addBccRecipient(bccAdd);
    
    long int id = msgPlugin->convertTo(&msg);
    msgPlugin->send(id);
    }
    	
//---------------------------------------------------------------
// TestMmsPlugin::testReceived
//---------------------------------------------------------------
void TestMmsPlugin::testReceived(int type)
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
// TestMmsPlugin::validateMsg
//---------------------------------------------------------------
void TestMmsPlugin::validateMsg(const ConvergedMessage& message,
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
                QCOMPARE(array[0]->address(),QString(TEST_SENDER));
        }

   //     QCOMPARE(serviceId,QString(TEST_SERVICE_ID));

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
