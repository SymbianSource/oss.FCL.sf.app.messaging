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
    //does nothing
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

void SmsSettingsPrivate::setReceiveSerivceMessages(
        TBool serviceMessages)
{
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    CSmsSettings* smsSettings = CSmsSettings::NewLC();

    smsAccount->LoadSettingsL(*smsSettings);

    //TODO set the receive message part

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

void SmsSettingsPrivate::settingsServiceMessagesAndCharEncoding(
                                                              TBool& report,
                                                              TBool& statusEncoding)
{
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    CSmsSettings* smsSettings = CSmsSettings::NewLC();

    smsAccount->LoadSettingsL(*smsSettings);

    //TODO: service messages read

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
    //smsSettings->RemoveServiceCenter(index);
    //add a new service center
    smsSettings->AddServiceCenterL(name->Des(), address->Des());
    
    if (flag == true)
    {
       smsSettings->SetDefaultServiceCenter(
               smsSettings->ServiceCenterCount()- 1);
    }
    
    smsAccount->SaveSettingsL(*smsSettings);

    smsAccount->LoadSettingsL(*smsSettings);
    if (flag == true)
    {
        smsSettings->RemoveServiceCenter(index);
    }
    
    smsAccount->SaveSettingsL(*smsSettings);
    
    CleanupStack::PopAndDestroy(2);
}

void SmsSettingsPrivate::addSmsMessageCenter(HBufC* address, HBufC* name)
{
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    CSmsSettings* smsSettings = CSmsSettings::NewLC();

    smsAccount->LoadSettingsL(*smsSettings);
    
    int total_count = smsSettings->ServiceCenterCount();
    smsSettings->AddServiceCenterL(name->Des(), address->Des());
    
    if (total_count == 0) // no service center
    {
        smsSettings->SetDefaultServiceCenter(0);
    }
    smsSettings->SetCommDbAction(CSmsSettings::ENone);
    smsAccount->SaveSettingsL(*smsSettings);

    CleanupStack::PopAndDestroy(2);
}

void SmsSettingsPrivate::deleteSmsMessageCenter(TInt aDeleteIndex)
{
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    CSmsSettings* smsSettings = CSmsSettings::NewLC();

    smsAccount->LoadSettingsL(*smsSettings);
    
    TInt indexDefault = smsSettings->DefaultServiceCenter();
    
    //remove from settings
    smsSettings->RemoveServiceCenter(aDeleteIndex);
    
    if (indexDefault == aDeleteIndex 
            && smsSettings->ServiceCenterCount() > 0)
    {
        smsSettings->SetDefaultServiceCenter(0);
    }
    smsSettings->SetCommDbAction(CSmsSettings::ENone);
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

//eof
