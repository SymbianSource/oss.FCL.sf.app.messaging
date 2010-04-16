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
 * Description:  This provides the messaging mw interface for sms 
 *
 */

#include <commdb.h>
#include <commdbconnpref.h>
#include <msvapi.h>
#include <msvstd.h>
#include <msvuids.h>
#include <csmsaccount.h>
#include <smutset.h>
#include "debugtraces.h"


#include "smssettingsprivate.h"
#include "msgsettingsutil.h"


const TInt KUniSmsSCStringLength = 50;

/**
 * Message Server session event handler 
 */
class CObserver : public MMsvSessionObserver
{
public:
    void HandleSessionEvent(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/,
                            TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    }

    void HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/,
                             TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    }
};

//two phase constructor
SmsSettingsPrivate* SmsSettingsPrivate::NewL()
{
    SmsSettingsPrivate* self = new (ELeave) SmsSettingsPrivate();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

//2nd phase constructor
void SmsSettingsPrivate::ConstructL()
{
    createRepositoryL();
}

//---------------------------------------------------------------
// SmsSettingsPrivate::SmsSettingsPrivate
// @see header
//---------------------------------------------------------------
SmsSettingsPrivate::SmsSettingsPrivate()
{
    //do nothing
}

//---------------------------------------------------------------
// SmsSettingsPrivate::~SmsSettingsPrivate
// @see header
//---------------------------------------------------------------
SmsSettingsPrivate::~SmsSettingsPrivate()
{
    //do nothing
}

//---------------------------------------------------------------
// SmsSettingsPrivate::createRepository
// @see header
//---------------------------------------------------------------
void SmsSettingsPrivate::createRepositoryL()
{
    CObserver* pObserver = new (ELeave) CObserver();
    CleanupStack::PushL(pObserver);

    TMsvSelectionOrdering ordering;
    CMsvSession* iMsvSession = CMsvSession::OpenSyncL(*pObserver);
    CleanupStack::PushL(iMsvSession);
    CMsvEntry* root = CMsvEntry::NewL(*iMsvSession,
                                      KMsvRootIndexEntryId,
                                      ordering);
    CleanupStack::PushL(root);

    //then do not create a new entry
    TMsvId firstId;
    TRAPD(err, TSmsUtilities::ServiceIdL(*root,firstId));
    if (err == KErrNotFound)
    {
        TMsvEntry entry;
        entry.iMtm = KUidMsgTypeSMS;
        entry.iType = KUidMsvServiceEntry;
        entry.SetReadOnly(EFalse);
        entry.SetVisible(EFalse);
        entry.iDate.UniversalTime();
        entry.iDetails.Set(_L("Nokiatest"));

        root->SetEntryL(KMsvRootIndexEntryId);
        root->CreateL(entry);
		}
		
        //create a dummy entry
    CSmsAccount* smsAccount = CSmsAccount::NewLC();

    CSmsSettings* smsSettings = CSmsSettings::NewLC();

    smsAccount->LoadSettingsL(*smsSettings);
    smsSettings->SetValidityPeriod(ESmsVPMaximum);
    smsSettings->SetReplyQuoted(ETrue);
    smsSettings->SetRejectDuplicate(ETrue);
    smsSettings->SetDelivery(ESmsDeliveryImmediately);
    smsSettings->SetDeliveryReport(EFalse);
    smsSettings->SetReplyPath(EFalse);
    smsSettings->SetMessageConversion((TSmsPIDConversion) 0);
    smsSettings->SetCanConcatenate(ETrue);
    smsSettings->SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabet7Bit);
    smsSettings->SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFInteger);
    smsSettings->SetStatusReportHandling(CSmsSettings::EMoveReportToInboxVisible);
    smsSettings->SetSpecialMessageHandling(CSmsSettings::EMoveReportToInboxVisible);
    smsSettings->SetSmsBearer((CSmsSettings::TMobileSmsBearer) 3);
    smsSettings->SetCommDbAction(CSmsSettings::EStoreToCommDb);
    smsSettings->SetSmsBearerAction(CSmsSettings::EStoreToCommDb);

    smsAccount->SaveSettingsL(*smsSettings);

    CleanupStack::PopAndDestroy(2);
   	CleanupStack::PopAndDestroy(3);
}

void SmsSettingsPrivate::setDeliveryReport(TBool report)
{
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    CSmsSettings* smsSettings = CSmsSettings::NewLC();

    smsAccount->LoadSettingsL(*smsSettings);

    smsSettings->SetDeliveryReport(report);

    smsAccount->SaveSettingsL(*smsSettings);

    CleanupStack::PopAndDestroy(2);
}

void SmsSettingsPrivate::setCharacterEncoding(TBool status)
{
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    CSmsSettings* smsSettings = CSmsSettings::NewLC();

    smsAccount->LoadSettingsL(*smsSettings);

    if (status == EFalse)
    {
        smsSettings->SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabet7Bit);
    }
    else
    {
        smsSettings->SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabetUCS2);
    }

    smsAccount->SaveSettingsL(*smsSettings);

    CleanupStack::PopAndDestroy(2);
}

void SmsSettingsPrivate::settingsDeliverReportAndCharEncoding(
                                                              TBool& report,
                                                              TBool& statusEncoding)
{
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    CSmsSettings* smsSettings = CSmsSettings::NewLC();

    smsAccount->LoadSettingsL(*smsSettings);

    report = smsSettings->DeliveryReport();

    TSmsDataCodingScheme::TSmsAlphabet charSet = smsSettings->CharacterSet();

    statusEncoding = ETrue;
    if (charSet == TSmsDataCodingScheme::ESmsAlphabet7Bit)
    {
        statusEncoding = EFalse;
    }
    CleanupStack::PopAndDestroy(2);
}

void SmsSettingsPrivate::getAllSMSMessageCenter(
                                                RPointerArray<HBufC>& accessPoints,
                                                TInt &defaultIndex)
{
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    CSmsSettings* settings = CSmsSettings::NewLC();

    smsAccount->LoadSettingsL(*settings);

    //save the default index
    defaultIndex = settings->DefaultServiceCenter();

    int totalSMSc = settings->ServiceCenterCount();
    for (int index = 0; index < totalSMSc; index++)
    {
        TPtrC16 name = settings->GetServiceCenter(index).Name();
        HBufC* accessName = name.AllocL();
        accessPoints.AppendL(accessName);
    }

    CleanupStack::PopAndDestroy(2);

    return;
}

void SmsSettingsPrivate::setSMSMessageCenter(int index)
{
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    CSmsSettings* smsSettings = CSmsSettings::NewLC();

    smsAccount->LoadSettingsL(*smsSettings);
    smsSettings->SetDefaultServiceCenter(index);
    smsAccount->SaveSettingsL(*smsSettings);

    CleanupStack::PopAndDestroy(2);
}

void SmsSettingsPrivate::editSMSServiceCentre(HBufC* address, HBufC* name,
                                              TInt index)
{
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    CSmsSettings* smsSettings = CSmsSettings::NewLC();

    smsAccount->LoadSettingsL(*smsSettings);

    TInt indexDefault = smsSettings->DefaultServiceCenter();
    bool flag = false;
    if (index == indexDefault)
    {
        flag = true;
    }

    //remove the service center
    smsSettings->RemoveServiceCenter(index);
    //add a new service center
    smsSettings->AddServiceCenterL(name->Des(), address->Des());

    if (flag == true)
    {
        smsSettings->SetDefaultServiceCenter(smsSettings->ServiceCenterCount()
                - 1);
    }
    smsAccount->SaveSettingsL(*smsSettings);

    CleanupStack::PopAndDestroy(2);
}

void SmsSettingsPrivate::addSmsMessageCenter(HBufC* address, HBufC* name)
{
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    CSmsSettings* smsSettings = CSmsSettings::NewLC();

    smsAccount->LoadSettingsL(*smsSettings);
    smsSettings->AddServiceCenterL(name->Des(), address->Des());
    smsAccount->SaveSettingsL(*smsSettings);

    CleanupStack::PopAndDestroy(2);
}

void SmsSettingsPrivate::smsCenterNameAndNumber(int index,
                                                HBufC** centerNumber,
                                                HBufC** centerName)
{
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    CSmsSettings* smsSettings = CSmsSettings::NewLC();

    smsAccount->LoadSettingsL(*smsSettings);

    TPtrC16 addr = smsSettings->GetServiceCenter(index).Address();
    TPtrC16 name = smsSettings->GetServiceCenter(index).Name();

    (*centerNumber) = addr.AllocL();
    (*centerName) = name.AllocL();

    CleanupStack::PopAndDestroy(2);
}

void SmsSettingsPrivate::setEmailGateway(HBufC* emailGateway)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "Enter SmsSettingsPrivate::setEmailGateway" << emailGateway;
#endif

    
    TBuf<KUniSmsSCStringLength> emailGateWayNumber;
    
    MsgSettingsUtil::WriteEmailGatewayOverSmsSettingsL( 
                             emailGateway->Des(),
                             ETrue );
    
#ifdef _DEBUG_TRACES_
    qDebug() << "Exit SmsSettingsPrivate::setEmailGateway";
#endif

    return;
}

void SmsSettingsPrivate::setEmailSeriveCenterNumber(HBufC* emailNumber)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "Enter SmsSettingsPrivate::setEmailSeriveCenterNumber"
            << emailNumber;
#endif

    
    MsgSettingsUtil::WriteEmailServiceNumberOverSmsSettingsL(emailNumber->Des(),
                                                             ETrue );
    
#ifdef _DEBUG_TRACES_
    qDebug() << "Enter SmsSettingsPrivate::setEmailSeriveCenterNumber";
#endif

    return;
}

void SmsSettingsPrivate::emailEmailGatewayAndServiceCenterNumber(
                                                  HBufC** emailGateway,
                                                  HBufC** serviceNumber)
{
#ifdef _DEBUG_TRACES_
    qDebug()
            << "Enter SmsSettingsPrivate::emailEmailGatewayAndServiceCenterNumber";
#endif

    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    CSmsSettings* smsSettings = CSmsSettings::NewLC();

    smsAccount->LoadSettingsL(*smsSettings);

    TBuf<KUniSmsSCStringLength> emailSmscNumber;
    TBuf<KUniSmsSCStringLength> emailGateWayNumber;

    TInt readResult = MsgSettingsUtil::ReadEmailOverSmsSettingsL(emailSmscNumber,
                                               emailGateWayNumber);
    if (KErrNone == readResult)
    {
        // In any otther case we need to show the conf pop-up window
        if (emailSmscNumber == KNullDesC)
        {
            if (smsSettings->DefaultServiceCenter() > 0)
            {
                emailSmscNumber = smsSettings->GetServiceCenter(
                             smsSettings->DefaultServiceCenter()).Address();
            }
            else
            {
                int totalSMSc = smsSettings->ServiceCenterCount();
                if (totalSMSc > 0)
                {
                    emailSmscNumber
                        = smsSettings->GetServiceCenter(0).Address();
                }
            }
        }
    }
    
    MsgSettingsUtil::WriteEmailServiceNumberOverSmsSettingsL(
                                                             emailSmscNumber,
                                                             ETrue);
    
    *emailGateway = emailGateWayNumber.AllocL();
    *serviceNumber = emailSmscNumber.AllocL();
    
    CleanupStack::PopAndDestroy(2); //smssetting and smsaccount
    
#ifdef _DEBUG_TRACES_
    qDebug()
            << "Enter SmsSettingsPrivate::emailEmailGatewayAndServiceCenterNumber"
            << serviceNumber << " " << emailGateway;
#endif

    
}

//eof
