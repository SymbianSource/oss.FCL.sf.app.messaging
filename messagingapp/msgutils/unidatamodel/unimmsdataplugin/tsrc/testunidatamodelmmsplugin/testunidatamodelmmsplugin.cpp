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

#include <QtTest/QtTest>
#include "serviceinfo.h"
#include <QtDebug>
#include <QTimer>
#include <QSignalSpy>
#include "convergedmessage.h"
#include "s60qconversions.h"
#include "unidatamodelplugininterface.h"
#include "unidatamodelloader.h"
#include <mmsclient.h>
#include <mmsheaders.h>
#include <mmssettings.h>
#include <CMsvMimeHeaders.h>
#include "convergedmessageaddress.h"
#include "testunidatamodelmmsplugin.h"
#include "testunidatamodelmmsplugin.ini"
#include <f32file.h>

_LIT(KUnixEpoch, "19700000:000000.000000");
_LIT( KRootPath, "c:\\" );


void TestUniDataModelMMSPlugin::initTestCase()
    {
    iObserver = new (ELeave) TestObserverMMS();

    // Create a new messaging server session..
    TRAPD(err,iMSession = CMsvSession::OpenSyncL(*iObserver));
    if(err)
        {
        TInt x=err;
        }
    // Create the client mtm registry
    iMtmReg = CClientMtmRegistry::NewL(*iMSession);

    // Get the MMS clientmtm
    iMmsClientMtm = (CMmsClientMtm *) iMtmReg->NewMtmL(KUidMsgTypeMultimedia);

    User::LeaveIfError( iFs.Connect() );
    iFs.SetSessionPath( KRootPath );
    iSettings = CMmsSettings::NewL();
    iMmsHeaders = CMmsHeaders::NewL(iSettings->MmsVersion());
    iSettings->LoadSettingsL();
    iServiceId = iSettings->Service();
    
    iMessageId = 0;
    pluginLoader = new UniDataModelLoader();
    retTimeStamp = QDateTime::currentDateTime();
    }

void TestUniDataModelMMSPlugin::cleanupTestCase()
    {
    for(TInt i=0; i<idArr.Count();i++)
        {
        iMmsClientMtm->Entry().DeleteL(idArr[i]);
        }

    delete iObserver;
    delete iMmsClientMtm;
    delete iMtmReg;
    delete iMSession;
    delete pluginLoader;
    }


//---------------------------------------------------------------
// TestUniDataModelPlugin::init
//---------------------------------------------------------------

void TestUniDataModelMMSPlugin::init()
{
/*    iObserver = new (ELeave) TestObserverMMS();

    // Create a new messaging server session..
    TRAPD(err,iMSession = CMsvSession::OpenSyncL(*iObserver));
    if(err)
        {
        TInt x=err;
        }
    // Create the client mtm registry
    iMtmReg = CClientMtmRegistry::NewL(*iMSession);

    // Get the MMS clientmtm
    iMmsClientMtm = (CMmsClientMtm *) iMtmReg->NewMtmL(KUidMsgTypeMultimedia);

    User::LeaveIfError( iFs.Connect() );
    iFs.SetSessionPath( KRootPath );
    iSettings = CMmsSettings::NewL();
    iMmsHeaders = CMmsHeaders::NewL(iSettings->MmsVersion());
    iSettings->LoadSettingsL();
    iServiceId = iSettings->Service();
    
    iMessageId = 0;
    pluginLoader = new UniDataModelLoader();
    retTimeStamp = QDateTime::currentDateTime();*/
}

//---------------------------------------------------------------
// TestUniDataModelPlugin::cleanup
//---------------------------------------------------------------
void TestUniDataModelMMSPlugin::cleanup()
{

 /*  
    for(TInt i=0; i<idArr.Count();i++)
        {
        iMmsClientMtm->Entry().DeleteL(idArr[i]);
        }

    delete iObserver;
    delete iMmsClientMtm;
    delete iMtmReg;
    delete iMSession;
    delete pluginLoader;*/
}


void TestUniDataModelMMSPlugin::createMMS(TMsvId pId, TBool subjectField)
    {
    TMsvEntry indexEntry;
    indexEntry.iType = KUidMsvMessageEntry;
    indexEntry.iMtm = KUidMsgTypeMultimedia;
    indexEntry.iServiceId = iServiceId;
    indexEntry.iDate.HomeTime();
    
    // Convert TTime to QDateTime , this will be used for comparing the time of mesage 
    // when fetched from datamodel
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    TTime timeStamp = indexEntry.iDate;
    timeStamp.SecondsFrom(unixEpoch, seconds);
    retTimeStamp.setTime_t(seconds.Int());


    TMsvId parentId = pId;
    CMsvEntry* entry = CMsvEntry::NewL(*iMSession,parentId,TMsvSelectionOrdering());
    CleanupStack::PushL(entry);

    entry->SetEntryL(parentId);
    entry->CreateL(indexEntry);
    entry->SetEntryL(indexEntry.Id());
    iMmsClientMtm->SwitchCurrentEntryL(entry->EntryId());
    
    //iMmsClientMtm->LoadMessageL();
    
    CMsvStore* store = iMmsClientMtm->Entry().EditStoreL();
    CleanupStack::PushL(store);
      
    iMessageId = indexEntry.Id();
    idArr.Append(iMessageId);
    
    if(subjectField)
        {
        QString subject(TEST_MSG_SUBJECT);
        HBufC* subj = S60QConversions::qStringToS60Desc(subject);
        iMmsClientMtm->SetSubjectL(*subj);
        }

    //Sender
    QString sender(TEST_MSG_FROM1);
    HBufC* addr = S60QConversions::qStringToS60Desc(sender);
    
    
    if (addr)
        {
        CleanupStack::PushL(addr);
        TBuf<32> name;
        name.Copy(addr->Left(32));
        indexEntry.iDetails.Set(name);
        
        iMmsClientMtm->SetSenderL(*addr);
        
        CleanupStack::PopAndDestroy(addr);
        }
    
    //Recipient
    QString recipient(TEST_MSG_RECIEPIENT1);
    HBufC* addr2 = S60QConversions::qStringToS60Desc(recipient);
    if (addr2)
        {
        CleanupStack::PushL(addr2);
        iMmsClientMtm->AddAddresseeL(EMsvRecipientTo,*addr2);
        CleanupStack::PopAndDestroy(addr2);
        }
    
    QString recipient2(TEST_MSG_RECIEPIENT2);
    HBufC* addr3 = S60QConversions::qStringToS60Desc(recipient2);
    if (addr3)
        {
        CleanupStack::PushL(addr3);
        iMmsClientMtm->AddAddresseeL(EMsvRecipientCc,*addr3);
        CleanupStack::PopAndDestroy(addr3);
        }
        
    QString recipient3(TEST_MSG_RECIEPIENT3);
        HBufC* addr4 = S60QConversions::qStringToS60Desc(recipient3);
        if (addr4)
            {
            CleanupStack::PushL(addr4);
            iMmsClientMtm->AddAddresseeL(EMsvRecipientBcc,*addr4);
            CleanupStack::PopAndDestroy(addr4);
            }
        
    //Create Text Attachment
    TMsvAttachmentId attaId = 0;
    TFileName attachmentFile( _L("story.txt") );
    TBufC<12> story = _L( "Hello world!" );
    
    iMmsClientMtm->CreateTextAttachmentL(*store, attaId, story, attachmentFile, ETrue );
    msgSize = iMmsClientMtm->MessageSize();
    
    store->CommitL();
    CleanupStack::PopAndDestroy(); // store
    
    iMmsClientMtm->SaveMessageL();    
    }

void TestUniDataModelMMSPlugin::testInboxMessage()
    {
    createMMS(KMsvGlobalInBoxIndexEntryIdValue,ETrue);
    
    pluginLoader->loadPlugins();
    // Get the MMS data modelplugin
    pluginInterface = pluginLoader->getDataModelPlugin("mms");
    
    pluginInterface->setMessageId(iMessageId);

    int size = pluginInterface->messageSize();
    QVERIFY(size == msgSize);
    
    QVERIFY(pluginInterface->timeStamp() == retTimeStamp);
    
   
    //Fetch the To List
    ConvergedMessageAddressList recipientList;
    pluginInterface->toRecipientList(recipientList);
    QVERIFY(recipientList.count() != 0);
    
    ConvergedMessageAddressList recipientList2;
    pluginInterface->ccRecipientList(recipientList2);
    QVERIFY(recipientList2.count() != 0);
    
    ConvergedMessageAddressList recipientList3;
    pluginInterface->bccRecipientList(recipientList3);
    QVERIFY(recipientList3.count() != 0);
    
    QString recepient(TEST_MSG_FROM1);
    QString fromaddress;
    pluginInterface->fromAddress(fromaddress);
    QVERIFY(recepient == fromaddress);
    
    UniMessageInfoList attList = pluginInterface->attachmentList();
    QVERIFY(attList.count() != 0);
    
    TInt attCount =0;
    attCount = pluginInterface->attachmentCount();
    QVERIFY(attCount != 0);
    
    MsgPriority priority = pluginInterface->messagePriority();
    // Normal priority
    qDebug() << "prority" << priority;
    QVERIFY(priority == 1);

    QVERIFY(pluginInterface->objectCount() == 1);
    
    UniMessageInfoList objectlist = pluginInterface->objectList();
    QVERIFY(!objectlist.isEmpty());
    
    QVERIFY(pluginInterface->slideCount() == 0);
    
    TRAPD(err, iMmsClientMtm->SwitchCurrentEntryL(KMsvGlobalInBoxIndexEntryId));
    }

void TestUniDataModelMMSPlugin::testOutboxMessage()
    {
    createMMS(KMsvGlobalOutBoxIndexEntryIdValue,EFalse);
    
    pluginLoader->loadPlugins();
    // Get the MMS data modelplugin
    pluginInterface = pluginLoader->getDataModelPlugin("mms");
    QVERIFY(pluginInterface->messageType()=="mms");
    
    pluginInterface->setMessageId(iMessageId);

    int size = pluginInterface->messageSize();
    QVERIFY(size == msgSize);
    
    QVERIFY(pluginInterface->timeStamp() == retTimeStamp);
    
        
    //Fetch the To List
    ConvergedMessageAddressList recipientList;
    pluginInterface->toRecipientList(recipientList);
    QVERIFY(recipientList.count() != 0);
    
    ConvergedMessageAddressList recipientList2;
    pluginInterface->ccRecipientList(recipientList2);
    QVERIFY(recipientList2.count() != 0);
    
    ConvergedMessageAddressList recipientList3;
    pluginInterface->bccRecipientList(recipientList3);
    QVERIFY(recipientList3.count() != 0);
    
    QString recepient(TEST_MSG_FROM1);
    QString fromaddress;
    pluginInterface->fromAddress(fromaddress);
    QVERIFY(recepient == fromaddress);
    
    UniMessageInfoList attList = pluginInterface->attachmentList();
    QVERIFY(attList.count() != 0);
    
    TInt attCount =0;
    attCount = pluginInterface->attachmentCount();
    QVERIFY(attCount != 0);
    
    MsgPriority priority = pluginInterface->messagePriority();
    // Normal priority
    qDebug() << "prority" << priority;
    QVERIFY(priority == 1);
    
    QVERIFY(pluginInterface->objectCount() == 1);
    
    UniMessageInfoList objectlist = pluginInterface->objectList();
    QVERIFY(!objectlist.isEmpty());

    TRAPD(err, iMmsClientMtm->SwitchCurrentEntryL(KMsvGlobalInBoxIndexEntryId));    
    }


void TestUniDataModelMMSPlugin::createInboxImageMMS()
    {
    TMsvEntry indexEntry;
    indexEntry.iType = KUidMsvMessageEntry;
    indexEntry.iMtm = KUidMsgTypeMultimedia;
    indexEntry.iServiceId = iServiceId;
    indexEntry.iDate.HomeTime();
    
    // Convert TTime to QDateTime , this will be used for comparing the time of mesage 
    // when fetched from datamodel
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    TTime timeStamp = indexEntry.iDate;
    timeStamp.SecondsFrom(unixEpoch, seconds);
    retTimeStamp.setTime_t(seconds.Int());


    TMsvId parentId = KMsvGlobalInBoxIndexEntryIdValue;
    CMsvEntry* entry = CMsvEntry::NewL(*iMSession,parentId,TMsvSelectionOrdering());
    CleanupStack::PushL(entry);

    entry->SetEntryL(parentId);
    entry->CreateL(indexEntry);
    entry->SetEntryL(indexEntry.Id());
    iMmsClientMtm->SwitchCurrentEntryL(entry->EntryId());
    
    //iMmsClientMtm->LoadMessageL();
    
    CMsvStore* store = iMmsClientMtm->Entry().EditStoreL();
    CleanupStack::PushL(store);
      
    iMessageId = indexEntry.Id();
    idArr.Append(iMessageId);
    
    //Sender
    QString sender(TEST_MSG_FROM1);
    HBufC* addr = S60QConversions::qStringToS60Desc(sender);
    
    if (addr)
        {
        CleanupStack::PushL(addr);
        TBuf<32> name;
        name.Copy(addr->Left(32));
        indexEntry.iDetails.Set(name);
        
        iMmsClientMtm->SetSenderL(*addr);
        
        CleanupStack::PopAndDestroy(addr);
        }
    
    //Recipient
    QString recipient(TEST_MSG_RECIEPIENT1);
    HBufC* addr2 = S60QConversions::qStringToS60Desc(recipient);
    if (addr2)
        {
        CleanupStack::PushL(addr2);
        iMmsClientMtm->AddAddresseeL(EMsvRecipientTo,*addr2);    
        CleanupStack::PopAndDestroy(addr2);
        }
    
    
    // Attachments are added to the message entry one by one
   
    TMsvAttachmentId attaId = 0;
   
    RFile attaFile;
    // Set filename of attachment
    TFileName name( _L("C:\\pictures\\photo.jpg") );
   
    CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
    CleanupStack::PushL( mimeHeaders );
   
    // Set values to mime headers
    mimeHeaders->SetContentTypeL( _L8( "image") );
    mimeHeaders->SetContentSubTypeL( _L8( "jpeg" ) );
   
    _LIT8(KMimeType, "image/jpeg");
    TBufC8<10> mimeType(KMimeType);
    // CreateAttachment2L will set the content type to attachment Info
   
    // Open the attachment file for reading
    attaFile.Open( iFs, name, EFileShareReadersOnly | EFileRead );
    CleanupClosePushL(attaFile);
   
    CMsvAttachment* attaInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
    // attaInfo ownerhip will be transferred to Attachment Manager.
    // It must not be pushed onto the cleanupStack before calling 
    // CreateAttachment2L.
   
       
    iMmsClientMtm->CreateAttachment2L(*store, attaFile,mimeType,*mimeHeaders,attaInfo,attaId);
    // Now Attachment Manager owns the attaInfo
    attaInfo = NULL;
   
    CleanupStack::PopAndDestroy(); // attaFile.Close()
    CleanupStack::PopAndDestroy(); // mimeHeaders
   
    // Several attachments can be added before committing the store
   
    // Store must be committed before it is destroyed
    store->CommitL();
    CleanupStack::PopAndDestroy(); // store
    
    iMmsClientMtm->SaveMessageL();    
    }

void TestUniDataModelMMSPlugin::testInboxImageMMSMessage()
    {
    createInboxImageMMS();
    
    pluginLoader->loadPlugins();
    // Get the MMS data modelplugin
    pluginInterface = pluginLoader->getDataModelPlugin("mms");
    
    pluginInterface->setMessageId(iMessageId);

    int size = pluginInterface->messageSize();
    
    QVERIFY(pluginInterface->timeStamp() == retTimeStamp);
    
        
    //Fetch the To List
    ConvergedMessageAddressList recipientList;
    pluginInterface->toRecipientList(recipientList);
    QVERIFY(recipientList.count() != 0);
    
    QString recepient(TEST_MSG_FROM1);
    QString fromaddress;
    pluginInterface->fromAddress(fromaddress);
    QVERIFY(recepient == fromaddress);
    
    TRAPD(err, iMmsClientMtm->SwitchCurrentEntryL(KMsvGlobalInBoxIndexEntryId));
    }


void TestUniDataModelMMSPlugin::testPluginLoading()
{
    pluginLoader->loadPlugins();

}


void TestUniDataModelMMSPlugin::testGetPluginWithoutLoading()
{
    pluginInterface = pluginLoader->getDataModelPlugin("mms");
    QVERIFY(pluginInterface == NULL);

    // Try to geta plugin for a differnt message type
    pluginInterface = pluginLoader->getDataModelPlugin("vcal");
    QVERIFY(pluginInterface == NULL);
}

void TestUniDataModelMMSPlugin::testPluginLoadingMultipleTimes()
{
    pluginLoader->loadPlugins();
    pluginLoader->loadPlugins();
    pluginLoader->loadPlugins();
    pluginLoader->loadPlugins();
}


void TestObserverMMS::HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
                                       TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
}



