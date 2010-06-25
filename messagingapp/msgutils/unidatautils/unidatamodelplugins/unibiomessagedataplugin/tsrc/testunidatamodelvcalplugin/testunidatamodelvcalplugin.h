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

#ifndef TEST_UNI_DATA_MODEL_VCAL_PLUGIN_H
#define TEST_UNI_DATA_MODEL_VCAL_PLUGIN_H

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
#include <smsclnt.h>
#include <smuthdr.h>
#include <QDateTime>
#include <e32math.h> 


#include <bsp.h>

const TInt32	KUidBIOVCalenderMsg			=  {0x10005533};    // VCalender diary

class UniDataModelPluginInterface;
class UniDataModelLoader;
class MsgObserver;

class ConvergedMessage;
class CBIODatabase;


typedef enum
	{							//	GENERATE A MESSAGE USING....
    ENoMessage,                 //  .. error cases
    EBioIapSettingsMessage,     //  .. IAP grammar, includes Internet settings 
								//		Email settings, and logon scripts, 
	EBioEnpMessage,				//	.. Email Notification grammar.
	EBioRingTonesMessage,		//	.. Ringing Tones binary data
	EBioOpLogoMessage,			//	.. Logo for Operator service
	EBioCompBusCardMessage,		//	.. Compact Business Card data
	EBiovCardMessage,			//	.. virtual business card data
	EBiovCalenderMessage,		//	.. vCalender data
	EBioWAPSettingsMessage,		//  .. wap settings
	EBioWPRVSettingsMessage		//	.. wprv settings
	} TBIOMessageType;


class TEST_EXPORT TestUniDataModelVCalPlugin : public QObject
{
Q_OBJECT

private slots:
    //called by frame work.
    void initTestCase();//called before the first testfunction is executed.
    void cleanupTestCase();//called after the last testfunction was executed.
    void init();//called before each testfunction is executed.
    void cleanup();//called after every testfunction.
    //test cases.
    void testInBoxVCalMessage();
    void testDraftsVCalMessage();
    void testUnusedAPIs();

private:
    TMsvId CreateBIOEntryL(TDesC& aText, TBIOMessageType aMessageType);
    void CreateBioEntryClientSideL(TMsvEntry& aEntry, CRichText& aBody);
    HBufC* ReadFromFileL(const TDesC& aFile);
    void SetSessionPath(const TDesC& aSessionPath);
    TMsvId CreateBIOEntryFromFileL(const TDesC& aFilename,
                                   TBIOMessageType aMessageType);
    TBIOMessageType SetMessageType(const TDesC& aFileName);
    CMsvEntrySelection* GenerateMessagesL();
    TMsvId SetBIOServiceIdL();
    void SetForMtmTypeL(TMsvEntry& aEntry, TBIOMessageType aType);

private:
    UniDataModelPluginInterface* pluginInterface;
    MsgObserver* iObserver;
    CMsvSession* iMSession;
    CClientMtmRegistry* iMtmReg;
    TMsvId messageId;
    UniDataModelLoader* pluginLoader;
    QDateTime retTimeStamp;
    RFs iFs;
    CDir* iDir;
    TInt iFilesProcessed;
    CMsvEntry* iMsvEntry;
    CBIODatabase* iBioDb;
    TMsvId newEntryId;
    HBufC* tempNumber;
    TBufC<KMaxFileName> currentFile;
    TBool inbox;
    TBool drafts;
};

class MsgObserver : public MMsvSessionObserver
{
public:
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
                             TAny* aArg3);
};

#endif //TEST_UNI_DATA_MODEL_VCAL_PLUGIN_H