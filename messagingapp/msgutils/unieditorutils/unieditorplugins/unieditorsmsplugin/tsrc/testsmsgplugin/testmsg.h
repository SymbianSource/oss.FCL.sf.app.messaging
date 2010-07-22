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

#ifndef TEST_MSG_H
#define TEST_MSG_H
//Headers 
#include <e32svr.h>
#include <SMUTHDR.H>
#include <msvapi.h>
#include <smut.h>
#include <msvstd.h>
#include <rsendas.h>
#include <rsendasmessage.h>
#include <SMSCLNT.H>
#include <csmsaccount.h> 
//#include <MTMStore.h>
#include <mtclreg.h>
#include <mtclbase.h>
#include <txtrich.h> 				// CRichText
#include <msvids.h>            // for Message type IDs
#include <mmsclient.h>       // for CMmsClientMtm
#include <CMsvMimeHeaders.h>        //Attachemt mimeheader
#include <MMsvAttachmentManager.h>  //Attachment manager
#include <MMsvAttachmentManagerSync.h>
#include <f32file.h>                //parse
#include <utf.h>

#include <e32base.h>
#include <e32property.h>
#include <simtsy.h>
#include <etelmm.h>
#include <es_ini.h>
#include <commsdattypesv1_1.h>
#include <commsdat.h>

#include <Qtdebug>
#include <xqconversions.h>
#include "testsmsplugin.ini"

using namespace CommsDat;

const TInt KMsvTestUidPhonePwrValue = 0x100052C5;
enum TMsvTestPhoneStatus
    {
    EMsvTestPhoneOff = 0,
    EMsvTestPhoneOn
    };

class TestMsg : public MMsvSessionObserver
{
public:

    TestMsg();
    ~TestMsg();

    void createSCSettings();

    void createSMS();
    void createMMS();

    void deleteMessage(int msgId);

    void initSimTsyL();

public: //MMsvSessionObserver

    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
                             TAny* aArg3);

private:

    void initL();
    void createSMSL(TMsvId aFolderId, TBool aUnReadFlag);
    void createMMSL(TMsvId aFolderId, TBool aUnReadFlag);
    void initializeSimTsyL(TInt aTestNumber, HBufC* aTsyName);

private:

    CMsvSession* iSession;
    CClientMtmRegistry* iMtmRegistry;
    CSmsClientMtm *smsClientMtm;
};

//---------------------------------------------------------------
// TestMsg::TestMsg
//---------------------------------------------------------------
TestMsg::TestMsg()
{
    int err;
    TRAP(err,initL());
    #ifdef _DEBUG_TRACES_
qDebug() << "Error in initialization:" << err;
#endif

}

//---------------------------------------------------------------
// TestMsg::TestMsg
//---------------------------------------------------------------
TestMsg::~TestMsg()
{
    if(smsClientMtm)
    {
        delete smsClientMtm;
        smsClientMtm = NULL;
    }
    
    if (iMtmRegistry)
    {
        delete iMtmRegistry;
        iMtmRegistry = NULL;
    }

    if (iSession)
    {
        delete iSession;
        iSession = NULL;
    }
}

//---------------------------------------------------------------
// TestMsg::createSCSettings
//---------------------------------------------------------------
void TestMsg::createSCSettings()
{
    smsClientMtm = static_cast<CSmsClientMtm*> (iMtmRegistry->NewMtmL(KUidMsgTypeSMS));

    TMsvSelectionOrdering ordering;

    CMsvEntry* root = CMsvEntry::NewL(smsClientMtm->Session(),
                                      KMsvRootIndexEntryId,
                                      ordering);

    CSmsAccount* smsAccount = CSmsAccount::NewLC();

    CSmsSettings* settings = CSmsSettings::NewL();
    CleanupStack::PushL(settings);
    smsAccount->LoadSettingsL(*settings);

    TMsvEntry entry;
    entry.iMtm = KUidMsgTypeSMS;
    entry.iType = KUidMsvServiceEntry;
    entry.SetReadOnly(EFalse);
    entry.SetVisible(EFalse);
    entry.iDate.UniversalTime();
    entry.iDetails.Set(_L("Nokiatest"));

    root->SetEntryL(KMsvRootIndexEntryId);
    root->CreateL(entry);

    QString serviceCenter01(SERVICE_CENTER_01);
    QString serviceCenter02(SERVICE_CENTER_02);
    HBufC* sC01 = XQConversions::qStringToS60Desc(serviceCenter01);
    HBufC* sC02 = XQConversions::qStringToS60Desc(serviceCenter02);
    CleanupStack::PushL(sC01);
    CleanupStack::PushL(sC02);
    settings->AddServiceCenterL(_L("Nokia"), *sC01);
    settings->AddServiceCenterL(_L("Nokia"), *sC02);
    CleanupStack::PopAndDestroy(sC02);
    CleanupStack::PopAndDestroy(sC01);
    settings->SetDefaultServiceCenter(1);
    settings->SetValidityPeriod(ESmsVPWeek);
    settings->SetReplyQuoted(ETrue);
    settings->SetRejectDuplicate(ETrue);
    settings->SetDelivery(ESmsDeliveryImmediately);
    settings->SetDeliveryReport(EFalse);
    settings->SetReplyPath(EFalse);
    settings->SetMessageConversion(ESmsConvPIDNone);
    settings->SetCanConcatenate(ETrue);
    settings->SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabet7Bit);
    settings->SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFInteger);
    settings->SetStatusReportHandling(CSmsSettings::EMoveReportToInboxVisible);
    settings->SetSpecialMessageHandling(CSmsSettings::EMoveReportToInboxVisible);
    settings->SetSmsBearer(CSmsSettings::ESmsBearerCircuitPreferred);
    settings->SetCommDbAction(CSmsSettings::ENone);
    settings->SetSmsBearerAction(CSmsSettings::ENone);
    smsAccount->SaveSettingsL(*settings);

    CleanupStack::PopAndDestroy(settings);
    CleanupStack::PopAndDestroy(smsAccount);
}

//---------------------------------------------------------------
// TestMsg::createSMS
//---------------------------------------------------------------
void TestMsg::createSMS()
{
    int err;
    TRAP(err,createSMSL(KMsvGlobalInBoxIndexEntryId,ETrue));
    #ifdef _DEBUG_TRACES_
qDebug() << "Error in creating SMS:" << err;
#endif

}

//---------------------------------------------------------------
// TestMsg::createMMS
//---------------------------------------------------------------
void TestMsg::createMMS()
{
    int err;
    TRAP(err,createMMSL(KMsvGlobalInBoxIndexEntryId,ETrue));
    #ifdef _DEBUG_TRACES_
qDebug() << "Error in creating MMS:" << err;
#endif

}

//---------------------------------------------------------------
// TestMsg::initL
//---------------------------------------------------------------
void TestMsg::initL()
{
    iSession = CMsvSession::OpenSyncL(*this);
    iMtmRegistry = CClientMtmRegistry::NewL(*iSession);
}

//---------------------------------------------------------------
// TestMsg::createSMSL
//---------------------------------------------------------------
void TestMsg::createSMSL(TMsvId aFolderId, TBool aUnReadFlag)
{
    CSmsClientMtm *smsClientMtm =
            static_cast<CSmsClientMtm*> (iMtmRegistry->NewMtmL(KUidMsgTypeSMS));
    CleanupStack::PushL(smsClientMtm);
    //Create entry from this index entry

    smsClientMtm->SwitchCurrentEntryL(aFolderId);
    smsClientMtm->CreateMessageL(KUidMsgTypeSMS.iUid);
    TMsvEntry indexEntry = smsClientMtm->Entry().Entry();
    indexEntry.SetInPreparation(ETrue);
    indexEntry.iMtm = KUidMsgTypeSMS;
    indexEntry.iType = KUidMsvMessageEntry;
    indexEntry.iServiceId = smsClientMtm->ServiceId();
    indexEntry.iDate.HomeTime();
    //AddBody
    QString body(TEST_MSG_BODY);

    HBufC* bodyText = XQConversions::qStringToS60Desc(body);
    if (bodyText)
    {
        CleanupStack::PushL(bodyText);
        CRichText & body = smsClientMtm->Body();
        body.Reset();
        body.InsertL(0, *bodyText);

        TBuf<32> buf;
        buf.Copy(bodyText->Left(32));
        indexEntry.iDescription.Set(buf);

        CleanupStack::PopAndDestroy(bodyText);
    }

    // Add addressee
    //Add Address
    QString recipient(TEST_MSG_FROM1);
    // convert from QString to HBufC 
    HBufC* addr = XQConversions::qStringToS60Desc(recipient);
    if (addr)
    {
        CleanupStack::PushL(addr);
        TBuf<32> name;
        name.Copy(addr->Left(32));
        indexEntry.iDetails.Set(name);

        if (aFolderId == KMsvDraftEntryId || aFolderId
                == KMsvGlobalOutBoxIndexEntryId)
        {
            smsClientMtm->AddAddresseeL(*addr, TPtrC());
            indexEntry.SetSendingState(KMsvSendStateWaiting);
        }
        else if (aFolderId == KMsvSentEntryId || aFolderId
                == KMsvGlobalInBoxIndexEntryId)
        {
            CSmsHeader& smsHeader = smsClientMtm->SmsHeader();
            smsHeader.SetFromAddressL(*addr);
        }
        CleanupStack::PopAndDestroy(addr);
    }

    // final fine tuning
    indexEntry.SetAttachment(EFalse);
    indexEntry.SetVisible(ETrue);
    indexEntry.SetInPreparation(EFalse);

    if (aFolderId == KMsvGlobalInBoxIndexEntryId)
    {
        if (aUnReadFlag)
        {
            indexEntry.SetUnread(ETrue);
        }
        indexEntry.SetNew(ETrue);
    }
    indexEntry.SetComplete(ETrue);
    smsClientMtm->Entry().ChangeL(indexEntry);
    smsClientMtm->SaveMessageL();

    CleanupStack::PopAndDestroy(smsClientMtm);
}

//---------------------------------------------------------------
// TestMsg::createMMSL
//---------------------------------------------------------------
void TestMsg::createMMSL(TMsvId aFolderId, TBool aUnReadFlag)
{
    CMmsClientMtm
            *mmsClientMtm =
                    static_cast<CMmsClientMtm*>
    (iMtmRegistry->NewMtmL(KUidMsgTypeMultimedia));
    CleanupStack::PushL(mmsClientMtm);
    //Create entry from this index entry

    mmsClientMtm->SwitchCurrentEntryL(aFolderId);
    mmsClientMtm->CreateMessageL(mmsClientMtm->DefaultServiceL());

    TMsvEntry indexEntry = mmsClientMtm->Entry().Entry();
    indexEntry.SetInPreparation(ETrue);
    indexEntry.iMtm = KUidMsgTypeMultimedia;
    indexEntry.iType = KUidMsvMessageEntry;
    indexEntry.iServiceId = mmsClientMtm->DefaultServiceL();
    indexEntry.iDate.HomeTime();

    //Subject
    QString subject(TEST_MSG_SUBJECT);

    HBufC* sub = XQConversions::qStringToS60Desc(subject);
    if (sub)
    {
        CleanupStack::PushL(sub);

        TBuf<32> buf;
        buf.Copy(sub->Left(32));
        indexEntry.iDescription.Set(buf);

        mmsClientMtm->SetSubjectL(*sub);
        CleanupStack::PopAndDestroy(sub);
    }

    // Add addressee
    //Add Address
    QString recipient(TEST_MSG_FROM1);
    // convert from QString to HBufC 
    HBufC* addr = XQConversions::qStringToS60Desc(recipient);
    if (addr)
    {
        CleanupStack::PushL(addr);
        TBuf<32> name;
        name.Copy(addr->Left(32));
        indexEntry.iDetails.Set(name);

        if (aFolderId == KMsvDraftEntryId || aFolderId
                == KMsvGlobalOutBoxIndexEntryId)
        {
            mmsClientMtm->AddAddresseeL(*addr);
            indexEntry.SetSendingState(KMsvSendStateWaiting);
        }
        else if (aFolderId == KMsvSentEntryId || aFolderId
                == KMsvGlobalInBoxIndexEntryId)
        {
            mmsClientMtm->SetSenderL(*addr);
            mmsClientMtm->AddAddresseeL(*addr);
        }
        CleanupStack::PopAndDestroy(addr);
    }

    // final fine tuning
    indexEntry.SetVisible(ETrue);
    indexEntry.SetInPreparation(EFalse);

    if (aFolderId == KMsvGlobalInBoxIndexEntryId)
    {
        if (aUnReadFlag)
        {
            indexEntry.SetUnread(ETrue);
        }
        indexEntry.SetNew(ETrue);
    }

    //body text
    QString bodyText(TEST_MSG_BODY);
    HBufC* text = XQConversions::qStringToS60Desc(bodyText);
    if (text)
    {
        CleanupStack::PushL(text);

        //we may need some conversion of text here
        const TInt KMmsMaxBytesPerCharacter = 4;
        HBufC8* buffer = HBufC8::NewL( text->Length() * KMmsMaxBytesPerCharacter );
        CleanupStack::PushL( buffer );
        TPtr8 buf8 = buffer->Des();

        // get an access to the message store
        CMsvStore* store = mmsClientMtm->Entry().EditStoreL();
        CleanupStack::PushL(store);

        CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
        CleanupStack::PushL( mimeHeaders );

        CMsvAttachment* attaInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
        CleanupStack::PushL( attaInfo );

        TPtrC8 contentType;
        contentType.Set( KMmsTextPlain );

        TInt position = contentType.Find( KMmsSlash8 );
        mimeHeaders->SetContentTypeL( contentType.Left( position ) );
        mimeHeaders->SetContentSubTypeL( contentType.Mid( position + 1 ) );
        attaInfo->SetMimeTypeL( contentType );
        attaInfo->SetAttachmentNameL( _L("body.txt") );

        mimeHeaders->SetMimeCharset( KMmsUtf8 );
        mimeHeaders->SetSuggestedFilenameL( _L("body.txt") );

        // if conversion fails, something is really seriously wrong
        TInt error = CnvUtfConverter::ConvertFromUnicodeToUtf8( buf8, *text );

        attaInfo->SetSize( buf8.Length() );
        mimeHeaders->StoreL( *attaInfo ); // Mime headers are streamed into atta info

        MMsvAttachmentManagerSync& attaManSync = store->AttachmentManagerExtensionsL();
        RFile textFile;

        attaManSync.CreateAttachmentL( _L("body.txt"), textFile, attaInfo );
        CleanupStack::Pop( attaInfo ); //ownership was transferred.

        CleanupClosePushL( textFile );

        //get attachement id, we need it incase of failure
        TMsvAttachmentId attachmentId = attaInfo->Id();
        // Now our file handle is open for writing
        if ( buf8.Length()> 0 )
        {
            textFile.Write( buf8 );
            error = textFile.Flush();
        }
        // we must always close
        CleanupStack::PopAndDestroy( &textFile ); // close textFile
        CleanupStack::PopAndDestroy( mimeHeaders );

        store->CommitL();
        CleanupStack::PopAndDestroy(store);
        CleanupStack::PopAndDestroy( buffer );
        CleanupStack::PopAndDestroy(text);
    }

    indexEntry.SetComplete(ETrue);
    mmsClientMtm->Entry().ChangeL(indexEntry);
    mmsClientMtm->SaveMessageL();

    // cleanup    
    CleanupStack::PopAndDestroy(mmsClientMtm);
}

void TestMsg::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/,
                                  TAny* /*aArg2*/, TAny* /*aArg3*/)
{
}

void TestMsg::deleteMessage(int msgId)
{
    CMsvEntry* cEntry = iSession->GetEntryL(KMsvGlobalInBoxIndexEntryId);
    CleanupStack::PushL(cEntry);

    CMsvEntry* entry = iSession->GetEntryL(msgId);
    CleanupStack::PushL(entry);

    TMsvEntry tEntry = entry->Entry();
    tEntry.SetReadOnly(EFalse);
    entry->ChangeL(tEntry);

    cEntry->DeleteL(msgId);
    CleanupStack::PopAndDestroy(entry);
    CleanupStack::PopAndDestroy(cEntry);

}

void TestMsg::initSimTsyL()
{
    _LIT(KDefaultTsyName, "SIM");
    TPtrC defaultTsyName(KDefaultTsyName);
    HBufC* defaultTsyNameBuf = defaultTsyName.AllocLC();
    TInt testNumber = 0;

    initializeSimTsyL(testNumber, defaultTsyNameBuf);

    CMDBSession* dbSession = CMDBSession::NewL(CMDBSession::LatestVersion());
    CleanupStack::PushL(dbSession);

    CMDBRecordLink<CCDModemBearerRecord>
            *modemBearerRecordSet = new (ELeave) CMDBRecordLink<
                    CCDModemBearerRecord> (KCDTIdGlobalSettingsRecord
                    | KCDTIdModemPhoneServicesSMS);
    CleanupStack::PushL(modemBearerRecordSet);

    modemBearerRecordSet->SetRecordId(1);
    //Load field container with data from database
    TRAPD(err, modemBearerRecordSet->LoadL(*dbSession));
    CCDModemBearerRecord
            *modemRecord =
                    static_cast<CCDModemBearerRecord*>
    (CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));
    CleanupStack::PushL(modemRecord);

    /*!
     * Set the record ID to that previously read from
     * CCDGlobalSettingsRecord.iModemForPhoneServicesAndSMS
     */
    modemRecord->SetRecordId(*modemBearerRecordSet);

    //Load record container with data from database
    modemRecord->LoadL(*dbSession);

    modemRecord->iTsyName.SetMaxLengthL(defaultTsyNameBuf->Des().Length());
    modemRecord->iTsyName = defaultTsyNameBuf->Des();
    modemRecord->ModifyL(*dbSession);

    CleanupStack::PopAndDestroy(4);
}

void TestMsg::initializeSimTsyL(TInt aTestNumber, HBufC* aTsyName)
{
    //Initialize TSY using the System Agent
    TInt testState;
    if (KErrNone != RProperty::Get(KUidPSSimTsyCategory,
                                   KPSSimTsyTestNumber,
                                   testState))
    {
        User::LeaveIfError(RProperty::Define(KUidPSSimTsyCategory,
                                             KPSSimTsyTestNumber,
                                             RProperty::EInt));
    }

    User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory,
                                      KPSSimTsyTestNumber,
                                      aTestNumber));
    if (KErrNone != RProperty::Get(KUidSystemCategory,
                                   KMsvTestUidPhonePwrValue,
                                   testState))
    {
        User::LeaveIfError(RProperty::Define(KUidSystemCategory,
                                             KMsvTestUidPhonePwrValue,
                                             RProperty::EInt));
    }
    User::LeaveIfError(RProperty::Set(KUidSystemCategory,
                                      KMsvTestUidPhonePwrValue,
                                      EMsvTestPhoneOn));

    User::LeaveIfError(RProperty::Get(KUidPSSimTsyCategory,
                                      KPSSimTsyTestNumber,
                                      aTestNumber));

    RTelServer etelServer;
    TInt err = etelServer.Connect();

    if (err != KErrNone)
    {
        return;
    }
    User::LeaveIfError(etelServer.LoadPhoneModule(aTsyName->Des()));

    /*!
     * Find the phone corresponding to this TSY and open a number of
     * handles on it
     */
    TInt numPhones;
    User::LeaveIfError(etelServer.EnumeratePhones(numPhones));
    TBool found = EFalse;

    RMobilePhone iPhone;
    while (!found && numPhones--)
    {
        TName phoneTsy;
        User::LeaveIfError(etelServer.GetTsyName(numPhones, phoneTsy));
        if (phoneTsy.CompareF(aTsyName->Des()) == KErrNone)
        {
            found = ETrue;
            RTelServer::TPhoneInfo info;
            User::LeaveIfError(etelServer.GetPhoneInfo(numPhones, info));
            CleanupClosePushL(iPhone);
            User::LeaveIfError(iPhone.Open(etelServer, info.iName));
            User::LeaveIfError(iPhone.Initialise());
            CleanupStack::PopAndDestroy(&iPhone);
        }
    }
}

#endif //TEST_MSG_H
