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

#ifndef TESTUNIDATAMODELMMSPLUGIN_H_
#define TESTUNIDATAMODELMMSPLUGIN_H_

#ifdef BUILD_TEST_DLL
#define TEST_EXPORT Q_DECL_EXPORT
#else
#define TEST_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>
#include <txtetext.h>
#include <txtrich.h>
#include <mtclreg.h>
#include <mtclbase.h>
#include <msvids.h>
#include <QDateTime>

class UniDataModelPluginInterface;
class UniDataModelLoader;
class TestObserverMMS;
class CMmsClientMtm;
class CMmsHeaders;
class CMmsSettings;

class ConvergedMessage;
class RFs;

class TEST_EXPORT TestUniDataModelMMSPlugin : public QObject
{
Q_OBJECT

private slots:
    //called by frame work.
    void initTestCase();//called before the first testfunction is executed.
    void cleanupTestCase();//called after the last testfunction was executed.
    void init();//called before each testfunction is executed.
    void cleanup();//called after every testfunction.
    
    void testInboxMessage();
    void testPluginLoading(); 
    void testGetPluginWithoutLoading();
    void testPluginLoadingMultipleTimes();
	void testOutboxMessage();
    void testInboxImageMMSMessage();
//    void testUnusedAPIs();
    
private:
    void createMMS(TMsvId pId, TBool subjectField);
    void createInboxImageMMS();
    
private:
    UniDataModelPluginInterface* pluginInterface;
    TestObserverMMS* iObserver;
    CMsvSession* iMSession;
    CClientMtmRegistry* iMtmReg;
    CMmsClientMtm* iMmsClientMtm;
    TMsvId iMessageId;
    RArray<TMsvId> idArr;
    UniDataModelLoader* pluginLoader;
    QDateTime retTimeStamp ;
    RFs iFs;
    CMmsSettings* iSettings;
    CMmsHeaders* iMmsHeaders;
    TMsvId iServiceId;
    TInt msgSize;
};

class TestObserverMMS : public MMsvSessionObserver
{
public:
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
                             TAny* aArg3);
};

#endif /* TESTUNIDATAMODELMMSPLUGIN_H_ */
