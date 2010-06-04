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
#include <QtDebug>
#include <QTimer>
#include <QSignalSpy>
#include "s60qconversions.h"
#include <csmsaccount.h>

#include "convergedmessage.h"
#include "unidatamodelplugininterface.h"
#include "unidatamodelloader.h"
#include "serviceinfo.h"
#include "testunidatamodelsmsplugin.h"
#include "testunidatamodelsmsplugin.ini"
_LIT(KUnixEpoch, "19700000:000000.000000");

//---------------------------------------------------------------
// TestUniDataModelSMSPlugin::init
//---------------------------------------------------------------

void TestUniDataModelSMSPlugin::init()
{
    iObserver = new (ELeave) TestObserver();

    // Create a new messaging server session..
    iMSession = CMsvSession::OpenSyncL(*iObserver);
    // Create the client mtm registry
    iMtmReg = CClientMtmRegistry::NewL(*iMSession);

    // Get the SMS clientmtm
    iSmsClientMtm
            = static_cast<CSmsClientMtm*> (iMtmReg->NewMtmL(KUidMsgTypeSMS));
      
    TRAPD(err, iSmsClientMtm->DefaultServiceL()) ;     
            
    if( err == KErrNotFound)
        {
        CSmsAccount* account = CSmsAccount::NewL();
        CleanupStack::PushL(account);
        CSmsSettings* settings = CSmsSettings::NewL();
        CleanupStack::PushL(settings);
        account->InitialiseDefaultSettingsL(*settings);
        CleanupStack::PopAndDestroy(2);
        
        }
    messageId = 0;
    pluginLoader = new UniDataModelLoader();
    retTimeStamp = QDateTime::currentDateTime();
}

//---------------------------------------------------------------
// TestUniDataModelSMSPlugin::cleanup
//---------------------------------------------------------------
void TestUniDataModelSMSPlugin::cleanup()
{

    if (messageId > 0)
        iSmsClientMtm->Entry().DeleteL(messageId);

    delete iObserver;
    iObserver = NULL;
    delete iSmsClientMtm;
    iSmsClientMtm = NULL;
    delete iMtmReg;
    iMtmReg = NULL;
    delete iMSession;
    iMSession = NULL;
    
    delete pluginLoader;
}

//---------------------------------------------------------------
// TestUniDataModelSMSPlugin::validateMsg
//---------------------------------------------------------------
void TestUniDataModelSMSPlugin::createInboxSMS()
{
    TRAPD(err, iSmsClientMtm->SwitchCurrentEntryL(KMsvGlobalInBoxIndexEntryId));

    // Create a SMS message 
    iSmsClientMtm->CreateMessageL(iSmsClientMtm->DefaultServiceL());
    TMsvEntry indexEntry = iSmsClientMtm->Entry().Entry();

    indexEntry.SetInPreparation(ETrue);
    indexEntry.iMtm = KUidMsgTypeSMS;
    indexEntry.iType = KUidMsvMessageEntry;
    indexEntry.iServiceId = iSmsClientMtm->ServiceId();
    indexEntry.iDate.HomeTime();

    // Convert TTime to QDateTime , this will be used for comparing the time of message 
    // when fetched from datamodel
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    TTime timeStamp = indexEntry.iDate;
    timeStamp.SecondsFrom(unixEpoch, seconds);
    retTimeStamp.setTime_t(seconds.Int());

    //AddBody
    QString body(TEST_MSG_BODY);

    HBufC* bodyText = S60QConversions::qStringToS60Desc(body);
    if (bodyText)
    {
        CleanupStack::PushL(bodyText);
        // Set the Body of the message
        CRichText & body = iSmsClientMtm->Body();
        body.Reset();
        body.InsertL(0, *bodyText);

        TBuf<32> buf;
        buf.Copy(bodyText->Left(32));
        indexEntry.iDescription.Set(buf);

        CleanupStack::PopAndDestroy(bodyText);
    }

    QString recepient(TEST_MSG_FROM1);
    QString recepient2(TEST_MSG_FROM2);
    QString alias(TEST_MSG_ALIAS1);

    HBufC* addr = S60QConversions::qStringToS60Desc(recepient);
    HBufC* addr2 = S60QConversions::qStringToS60Desc(recepient2);
    HBufC* alias1 = S60QConversions::qStringToS60Desc(alias);

    if (addr)
    {
        CleanupStack::PushL(addr);
        TBuf<32> name;
        name.Copy(addr->Left(32));
        indexEntry.iDetails.Set(name);
        CSmsHeader& smsHeader = iSmsClientMtm->SmsHeader();
        smsHeader.SetFromAddressL(*addr);
        messageId = indexEntry.Id();

        CleanupStack::PopAndDestroy(addr);
    }

    indexEntry.SetAttachment(EFalse);
    indexEntry.SetVisible(ETrue);
    indexEntry.SetInPreparation(EFalse);

    indexEntry.SetComplete(ETrue);
    iSmsClientMtm->Entry().ChangeL(indexEntry);
    iSmsClientMtm->SaveMessageL();

}

void TestUniDataModelSMSPlugin::createOutboxSMS()
{
    TRAPD(err, iSmsClientMtm->SwitchCurrentEntryL(KMsvGlobalOutBoxIndexEntryId));

    // Create a SMS message 
    iSmsClientMtm->CreateMessageL(iSmsClientMtm->DefaultServiceL());
    TMsvEntry indexEntry = iSmsClientMtm->Entry().Entry();

    indexEntry.SetInPreparation(ETrue);
    indexEntry.iMtm = KUidMsgTypeSMS;
    indexEntry.iType = KUidMsvMessageEntry;
    indexEntry.iServiceId = iSmsClientMtm->ServiceId();
    indexEntry.iDate.HomeTime();

    // Convert TTime to QDateTime , this will be used for comparing the time of message 
    // when fetched from datamodel
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    TTime timeStamp = indexEntry.iDate;
    timeStamp.SecondsFrom(unixEpoch, seconds);
    retTimeStamp.setTime_t(seconds.Int());

    //AddBody
    QString body(TEST_MSG_BODY);

    HBufC* bodyText = S60QConversions::qStringToS60Desc(body);
    if (bodyText)
    {
        CleanupStack::PushL(bodyText);
        // Set the Body of the message
        CRichText & body = iSmsClientMtm->Body();
        body.Reset();
        body.InsertL(0, *bodyText);

        TBuf<32> buf;
        buf.Copy(bodyText->Left(32));
        indexEntry.iDescription.Set(buf);

        CleanupStack::PopAndDestroy(bodyText);
    }

    QString recepient(TEST_MSG_FROM1);
    QString recepient2(TEST_MSG_FROM2);
    QString alias(TEST_MSG_ALIAS1);

    HBufC* addr = S60QConversions::qStringToS60Desc(recepient);
    HBufC* addr2 = S60QConversions::qStringToS60Desc(recepient2);
    HBufC* alias1 = S60QConversions::qStringToS60Desc(alias);

    if (addr)
    {
        CleanupStack::PushL(addr);
        TBuf<32> name;
        name.Copy(addr->Left(32));
        indexEntry.iDetails.Set(name);

        // If drafts/ outbox message , set the recipients 
        iSmsClientMtm->AddAddresseeL(*addr, TPtrC());
        iSmsClientMtm->AddAddresseeL(*addr2, *alias1);
        indexEntry.SetSendingState(KMsvSendStateWaiting);
        messageId = indexEntry.Id();
        CleanupStack::PopAndDestroy(addr);
    }

    indexEntry.SetAttachment(EFalse);
    indexEntry.SetVisible(ETrue);
    indexEntry.SetInPreparation(EFalse);

    indexEntry.SetComplete(ETrue);
    iSmsClientMtm->Entry().ChangeL(indexEntry);
    iSmsClientMtm->SaveMessageL();

}

void TestUniDataModelSMSPlugin::createSentSMS()
{
    TRAPD(err, iSmsClientMtm->SwitchCurrentEntryL(KMsvSentEntryId));

    // Create a SMS message 
    iSmsClientMtm->CreateMessageL(iSmsClientMtm->DefaultServiceL());
    TMsvEntry indexEntry = iSmsClientMtm->Entry().Entry();

    indexEntry.SetInPreparation(ETrue);
    indexEntry.iMtm = KUidMsgTypeSMS;
    indexEntry.iType = KUidMsvMessageEntry;
    indexEntry.iServiceId = iSmsClientMtm->ServiceId();
    indexEntry.iDate.HomeTime();

    // Convert TTime to QDateTime , this will be used for comparing the time of message 
    // when fetched from datamodel

    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    TTime timeStamp = indexEntry.iDate;
    timeStamp.SecondsFrom(unixEpoch, seconds);
    retTimeStamp.setTime_t(seconds.Int());

    //AddBody
    QString body(TEST_MSG_BODY);

    HBufC* bodyText = S60QConversions::qStringToS60Desc(body);
    if (bodyText)
    {
        CleanupStack::PushL(bodyText);
        // Set the Body of the message
        CRichText & body = iSmsClientMtm->Body();
        body.Reset();
        body.InsertL(0, *bodyText);

        TBuf<32> buf;
        buf.Copy(bodyText->Left(32));
        indexEntry.iDescription.Set(buf);

        CleanupStack::PopAndDestroy(bodyText);
    }

    QString recepient(TEST_MSG_FROM1);
    QString recepient2(TEST_MSG_FROM2);
    QString alias(TEST_MSG_ALIAS1);

    HBufC* addr = S60QConversions::qStringToS60Desc(recepient);
    HBufC* addr2 = S60QConversions::qStringToS60Desc(recepient2);
    HBufC* alias1 = S60QConversions::qStringToS60Desc(alias);

    if (addr)
    {
        CleanupStack::PushL(addr);
        TBuf<32> name;
        name.Copy(addr->Left(32));
        indexEntry.iDetails.Set(name);

        // If inbox/ sent folder  message , set the from address 
        CSmsHeader& smsHeader = iSmsClientMtm->SmsHeader();
        smsHeader.SetFromAddressL(*addr);
        messageId = indexEntry.Id();

        CleanupStack::PopAndDestroy(addr);
    }

    indexEntry.SetAttachment(EFalse);
    indexEntry.SetVisible(ETrue);
    indexEntry.SetInPreparation(EFalse);

    indexEntry.SetComplete(ETrue);
    iSmsClientMtm->Entry().ChangeL(indexEntry);
    iSmsClientMtm->SaveMessageL();

}

void TestUniDataModelSMSPlugin::createDraftsSMS()
{
    TRAPD(err, iSmsClientMtm->SwitchCurrentEntryL(KMsvDraftEntryId));
    // Create a SMS message 
    iSmsClientMtm->CreateMessageL(iSmsClientMtm->DefaultServiceL());
    TMsvEntry indexEntry = iSmsClientMtm->Entry().Entry();

    indexEntry.SetInPreparation(ETrue);
    indexEntry.iMtm = KUidMsgTypeSMS;
    indexEntry.iType = KUidMsvMessageEntry;
    indexEntry.iServiceId = iSmsClientMtm->ServiceId();
    indexEntry.iDate.HomeTime();

    // Convert TTime to QDateTime , this will be used for comparing the time of message 
    // when fetched from datamodel
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    TTime timeStamp = indexEntry.iDate;
    timeStamp.SecondsFrom(unixEpoch, seconds);
    retTimeStamp.setTime_t(seconds.Int());

    //AddBody
    QString body(TEST_MSG_BODY);

    HBufC* bodyText = S60QConversions::qStringToS60Desc(body);
    if (bodyText)
    {
        CleanupStack::PushL(bodyText);
        // Set the Body of the message
        CRichText & body = iSmsClientMtm->Body();
        body.Reset();
        body.InsertL(0, *bodyText);

        TBuf<32> buf;
        buf.Copy(bodyText->Left(32));
        indexEntry.iDescription.Set(buf);

        CleanupStack::PopAndDestroy(bodyText);
    }

    QString recipient(TEST_MSG_FROM1);
    QString recipient2(TEST_MSG_FROM2);
    QString alias(TEST_MSG_ALIAS1);

    HBufC* addr = S60QConversions::qStringToS60Desc(recipient);
    HBufC* addr2 = S60QConversions::qStringToS60Desc(recipient2);
    HBufC* alias1 = S60QConversions::qStringToS60Desc(alias);

    if (addr)
    {
        CleanupStack::PushL(addr);
        TBuf<32> name;
        name.Copy(addr->Left(32));
        indexEntry.iDetails.Set(name);

        // If drafts/ outbox message , set the recipients 
        iSmsClientMtm->AddAddresseeL(*addr, TPtrC());
        indexEntry.SetSendingState(KMsvSendStateWaiting);
        messageId = indexEntry.Id();

        CleanupStack::PopAndDestroy(addr);
    }

    indexEntry.SetAttachment(EFalse);
    indexEntry.SetVisible(ETrue);
    indexEntry.SetInPreparation(EFalse);

    indexEntry.SetComplete(ETrue);
    iSmsClientMtm->Entry().ChangeL(indexEntry);
    iSmsClientMtm->SaveMessageL();

}

void TestUniDataModelSMSPlugin::createDraftsSMSNoRecipient()
{
    TRAPD(err, iSmsClientMtm->SwitchCurrentEntryL(KMsvDraftEntryId));
    // Create a SMS message 
    iSmsClientMtm->CreateMessageL(iSmsClientMtm->DefaultServiceL());
    TMsvEntry indexEntry = iSmsClientMtm->Entry().Entry();

    indexEntry.SetInPreparation(ETrue);
    indexEntry.iMtm = KUidMsgTypeSMS;
    indexEntry.iType = KUidMsvMessageEntry;
    indexEntry.iServiceId = iSmsClientMtm->ServiceId();
    indexEntry.iDate.HomeTime();

    //AddBody
    QString body(TEST_MSG_BODY);

    HBufC* bodyText = S60QConversions::qStringToS60Desc(body);
    if (bodyText)
    {
        CleanupStack::PushL(bodyText);
        // Set the Body of the message
        CRichText & body = iSmsClientMtm->Body();
        body.Reset();
        body.InsertL(0, *bodyText);

        TBuf<32> buf;
        buf.Copy(bodyText->Left(32));
        indexEntry.iDescription.Set(buf);

        CleanupStack::PopAndDestroy(bodyText);
    }

    indexEntry.SetSendingState(KMsvSendStateWaiting);
    messageId = indexEntry.Id();

    indexEntry.SetAttachment(EFalse);
    indexEntry.SetVisible(ETrue);
    indexEntry.SetInPreparation(EFalse);

    indexEntry.SetComplete(ETrue);
    iSmsClientMtm->Entry().ChangeL(indexEntry);
    iSmsClientMtm->SaveMessageL();

}

void TestUniDataModelSMSPlugin::testInboxMessage()
{
    createInboxSMS();

    // Get the SMS data modelplugin
    pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Sms);

    pluginInterface->setMessageId(messageId);

    int size = pluginInterface->messageSize();

    QVERIFY(pluginInterface->timeStamp() == retTimeStamp);

    QString body;
    pluginInterface->body(body);
    QString origBody(TEST_MSG_BODY);
    QVERIFY(origBody == body);

    ConvergedMessageAddressList recipientList;
    pluginInterface->toRecipientList(recipientList);

    QString recipient(TEST_MSG_FROM1);
    QString recipient2(TEST_MSG_FROM2);
    QString origalias(TEST_MSG_ALIAS1);

    QVERIFY(recipientList.count() == 0);

    int size1 = pluginInterface->messageSize();

    QString fromaddress;
    pluginInterface->fromAddress(fromaddress);
    QVERIFY(recipient == fromaddress);
    TRAPD(err, iSmsClientMtm->SwitchCurrentEntryL(KMsvGlobalInBoxIndexEntryId));
}

void TestUniDataModelSMSPlugin::testOutboxMessage()
{
    createOutboxSMS();
    // Get the SMS data modelplugin
    pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Sms);

    // Get the data for the first message in DRAFTS    
    pluginInterface->setMessageId(messageId);
    int size = pluginInterface->messageSize();

    QVERIFY(pluginInterface->timeStamp() == retTimeStamp);

    QString body;
    pluginInterface->body(body);
    QString origBody(TEST_MSG_BODY);
    QVERIFY(origBody == body);

    ConvergedMessageAddressList recipientList;
    pluginInterface->toRecipientList(recipientList);

    QString recipient(TEST_MSG_FROM1);
    QString recipient2(TEST_MSG_FROM2);
    QString origalias(TEST_MSG_ALIAS1);

    QVERIFY(recipientList.count() == 2);
    QVERIFY(recipient == recipientList[0]->address()
            && recipientList[0]->alias().isEmpty());

    const QString& as = recipientList[1]->address();
    const QString& as1 = recipientList[1]->alias();
    QVERIFY(recipient2 == as);

    // ConvergedMessageAddress::alias return a QSting with a space
    origalias.append(' ');
    QVERIFY(origalias == as1);

    int size1 = pluginInterface->messageSize();

    QString fromaddress;
    pluginInterface->fromAddress(fromaddress);
    QVERIFY(fromaddress.isEmpty());
    TRAPD(err, iSmsClientMtm->SwitchCurrentEntryL(KMsvGlobalOutBoxIndexEntryId));
}

void TestUniDataModelSMSPlugin::testSentMessage()
{
    createSentSMS();
    // Get the SMS data modelplugin
    pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Sms);

    pluginInterface->setMessageId(messageId);

    int size = pluginInterface->messageSize();

    QVERIFY(pluginInterface->timeStamp() == retTimeStamp);

    QString body;
    pluginInterface->body(body);
    QString origBody(TEST_MSG_BODY);
    QVERIFY(origBody == body);

    ConvergedMessageAddressList recipientList;
    pluginInterface->toRecipientList(recipientList);

    QString recipient(TEST_MSG_FROM1);
    QString recipient2(TEST_MSG_FROM2);
    QString origalias(TEST_MSG_ALIAS1);

    QVERIFY(recipientList.count() == 0);

    QString fromaddress;
    pluginInterface->fromAddress(fromaddress);
    QVERIFY(recipient == fromaddress);
    TRAPD(err, iSmsClientMtm->SwitchCurrentEntryL(KMsvSentEntryId));
}

void TestUniDataModelSMSPlugin::testDraftsMessage()
{
    createDraftsSMS();
    // Get the SMS data modelplugin
    pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Sms);

    // Get the data for the first message in DRAFTS    
    pluginInterface->setMessageId(messageId);
    int size = pluginInterface->messageSize();

    QVERIFY(pluginInterface->timeStamp() == retTimeStamp);

    QString body;
    pluginInterface->body(body);
    QString origBody(TEST_MSG_BODY);
    QVERIFY(origBody == body);

    ConvergedMessageAddressList recipientList;
    pluginInterface->toRecipientList(recipientList);

    QString recipient(TEST_MSG_FROM1);

    QVERIFY(recipientList.count() == 1);
    QVERIFY(recipient == recipientList[0]->address()
            && recipientList[0]->alias().isEmpty());

    QString fromaddress;
    pluginInterface->fromAddress(fromaddress);
    QVERIFY(fromaddress.isEmpty());
    TRAPD(err, iSmsClientMtm->SwitchCurrentEntryL(KMsvDraftEntryId));
}

void TestUniDataModelSMSPlugin::testDraftsMessageNoRecipient()
{
    createDraftsSMSNoRecipient();
    // Get the SMS data modelplugin
    pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Sms);

    // Get the data for the first message in DRAFTS    
    pluginInterface->setMessageId(messageId);
    int size = pluginInterface->messageSize();

    QString body;
    pluginInterface->body(body);
    QString origBody(TEST_MSG_BODY);
    QVERIFY(origBody == body);

    ConvergedMessageAddressList recipientList;
    pluginInterface->toRecipientList(recipientList);

    QVERIFY(recipientList.count() == 0);

    QString fromaddress;
    pluginInterface->fromAddress(fromaddress);
    QVERIFY(fromaddress.isEmpty());
    TRAPD(err, iSmsClientMtm->SwitchCurrentEntryL(KMsvDraftEntryId));
}

void TestUniDataModelSMSPlugin::testGetPluginWithoutLoading()
{
    pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Sms);
    QVERIFY(pluginInterface == NULL);

    // Try to geta plugin for a differnt message type
    pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Mms);
    QVERIFY(pluginInterface == NULL);
}

void TestUniDataModelSMSPlugin::testUnusedAPIs()
{
	  pluginInterface = pluginLoader->getDataModelPlugin(ConvergedMessage::Sms);

    QVERIFY(pluginInterface->hasAttachment() == false);

    UniMessageInfoList attachmentlist = pluginInterface->attachmentList();
    QVERIFY(attachmentlist.isEmpty());

    QVERIFY(pluginInterface->objectCount() == 0);

    UniMessageInfoList objectlist = pluginInterface->objectList();
    QVERIFY(objectlist.isEmpty());

    QVERIFY(pluginInterface->slideCount() == 0);

    UniMessageInfoList slidelist = pluginInterface->slideContent(0);
    QVERIFY(slidelist.isEmpty());
    
    ConvergedMessageAddressList recipientList;
    
	pluginInterface->ccRecipientList(recipientList);
	QVERIFY(recipientList.count() == 0);
		
	pluginInterface->bccRecipientList(recipientList);
	QVERIFY(recipientList.count() == 0);
		
	 MsgPriority priority = pluginInterface->messagePriority();
    // Normal priority
    qDebug() << "prority" << priority;
    QVERIFY(priority == 1);
	int count = pluginInterface->attachmentCount();
    QVERIFY(count == 0);
		
}

void TestObserver::HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
                                       TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
}
