/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef T_MESSAGESETTINGENGINE_H
#define T_MESSAGESETTINGENGINE_H


#ifdef BUILD_TEST_DLL
#define TEST_EXPORT Q_DECL_EXPORT
#else
#define TEST_EXPORT Q_DECL_IMPORT
#endif


#include <QObject>
#include "msgsettingengine.h"


class TEST_EXPORT TestMsgSettingEngine: public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();
    
    void TestsetDeliveryReport();
    
    void TestsetCharacterEncoding();
   
    void TestsettingsDeliverReportAndCharEncoding();
   
    void TestsetMMSCreationmode();
    
    void TestsetMMSRetrieval();
    
    void TestsetAnonymousMessages();
   
    void TestsetReceiveMMSAdverts();
    
    void TestadvanceMmsSettings();
   
    void TestallMMsAcessPoints();
    
    void TestsetMMSAccesspoint();
    
    void TestallSMSMessageCenter();
    
    void TestsetSMSMessageCenter();
    
    void TesteditSmsMessageCenter();
    
    void TestaddSmsMessageCenter();
    
    void TestsmsCenterNameAndNumber();
    
    void TestsetEmailGateway();
    
    void TestsetEmailSeriveCenterNumber();
    
    void TestemailEmailGatewayAndServiceCenterNumber();
    
    void TestsetValidityPeriod();
    
    void TestvalidityPeriod();    
    
	void cleanupTestCase();

private:

    MsgSettingEngine* mMsgSettingEngine;
};

#endif //T_MESSAGESETTINGENGINE_H
