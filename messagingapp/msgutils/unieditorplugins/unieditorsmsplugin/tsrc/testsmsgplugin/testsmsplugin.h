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

#ifndef TEST_SMS_PLUGIN_H
#define TEST_SMS_PLUGIN_H

#ifdef BUILD_TEST_DLL
#define TEST_EXPORT Q_DECL_EXPORT
#else
#define TEST_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>
#include <e32const.h>

class UniEditorSmsPlugin;
class ConvergedMessage;
class TestMsg;

class TEST_EXPORT TestSmsPlugin: public QObject
    {
    Q_OBJECT

private slots:
    //called by frame work.
    void initTestCase();//called before the first testfunction is executed.
    void cleanupTestCase();//called after the last testfunction was executed.
    void init();//called before each testfunction is executed.
    void cleanup();//called after every testfunction.

    //test cases.
    void testSendReceiveSMS(); //test sending & Receiveig of sms

private:
    void createAndSend(int type);
    void testReceived(int type);
    void validateMsg(const ConvergedMessage& message, const QString& serviceId);
    
private:
    UniEditorSmsPlugin* msgPlugin;
    TestMsg *testMsg;
    int smsId;
    int mmsId;
    };
#endif //TEST_SMS_PLUGIN_H
