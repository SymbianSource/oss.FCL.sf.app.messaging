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
*
*/


#include <QtTest/QtTest>
#include <QObject>
#include <qtestcase.h>
#include <t_msgsettingengine.h>
#include <e32const.h>

void TestMsgSettingEngine::initTestCase()
{
mMsgSettingEngine = new MsgSettingEngine();
}

void TestMsgSettingEngine::TestsetDeliveryReport()
{
 mMsgSettingEngine->setDeliveryReport(ETrue);
 mMsgSettingEngine->setDeliveryReport(EFalse);
 
 //CSmsAccount* smsAccount = CSmsAccount::NewLC();
 //smsAccount->LoadSettingsL();
 
 
}

void TestMsgSettingEngine::TestsetCharacterEncoding()
{
mMsgSettingEngine->setCharacterEncoding(MsgSettingEngine::FullSupport);
mMsgSettingEngine->setCharacterEncoding(MsgSettingEngine::ReducedSupport);
}

void TestMsgSettingEngine::TestsettingsDeliverReportAndCharEncoding()
{
bool report=ETrue;
MsgSettingEngine::CharacterEncoding encoding;
encoding = MsgSettingEngine::FullSupport;

mMsgSettingEngine->setDeliveryReport(report);
mMsgSettingEngine->setCharacterEncoding(MsgSettingEngine::FullSupport);
mMsgSettingEngine->settingsDeliverReportAndCharEncoding(report,encoding);

encoding = MsgSettingEngine::ReducedSupport;
mMsgSettingEngine->setCharacterEncoding(MsgSettingEngine::ReducedSupport);
mMsgSettingEngine->settingsDeliverReportAndCharEncoding(report,encoding);

report=EFalse;
mMsgSettingEngine->setDeliveryReport(report);
mMsgSettingEngine->setCharacterEncoding(MsgSettingEngine::ReducedSupport);
mMsgSettingEngine->settingsDeliverReportAndCharEncoding(report,encoding);

encoding = MsgSettingEngine::FullSupport;
mMsgSettingEngine->setCharacterEncoding(MsgSettingEngine::ReducedSupport);
mMsgSettingEngine->settingsDeliverReportAndCharEncoding(report,encoding);

}

void TestMsgSettingEngine::TestsetMMSCreationmode()
{
MsgSettingEngine::MmsCreationMode mode;

mode = MsgSettingEngine::Restricted;
mMsgSettingEngine->setMMSCreationmode(mode);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->setMMSCreationmode(mode);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->setMMSCreationmode(mode);
}

void TestMsgSettingEngine::TestsetMMSRetrieval()
{

MsgSettingEngine::MmsRetrieval retrieval;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->setMMSRetrieval(retrieval);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->setMMSRetrieval(retrieval);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->setMMSRetrieval(retrieval);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->setMMSRetrieval(retrieval);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->setMMSRetrieval(retrieval);
}

void TestMsgSettingEngine::TestsetAnonymousMessages()
{
mMsgSettingEngine->setAnonymousMessages(ETrue);
mMsgSettingEngine->setAnonymousMessages(EFalse);
 
}

void TestMsgSettingEngine::TestsetReceiveMMSAdverts()
{
mMsgSettingEngine->setReceiveMMSAdverts(ETrue);     
mMsgSettingEngine->setReceiveMMSAdverts(EFalse);
}

void TestMsgSettingEngine::TestadvanceMmsSettings()
{

MsgSettingEngine::MmsCreationMode mmsMode;
MsgSettingEngine::MmsCreationMode mode;
MsgSettingEngine::MmsRetrieval retrieval;      

bool anonymous_Status;
bool mmsAdverts_Status;

mmsMode = MsgSettingEngine::Restricted;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mmsMode = MsgSettingEngine::Free;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mmsMode = MsgSettingEngine::Free;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);


MsgSettingEngine::MmsRetrieval mmsRetrieval;
mmsRetrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->setMMSRetrieval(mmsRetrieval);

mmsMode = MsgSettingEngine::Restricted;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);


mmsMode = MsgSettingEngine::Free;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mmsMode = MsgSettingEngine::Free;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);


mmsRetrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->setMMSRetrieval(mmsRetrieval);
mmsMode = MsgSettingEngine::Restricted;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);


mmsMode = MsgSettingEngine::Free;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mmsMode = MsgSettingEngine::Free;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);


mmsRetrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->setMMSRetrieval(mmsRetrieval);
mmsMode = MsgSettingEngine::Restricted;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);


mmsMode = MsgSettingEngine::Free;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mmsMode = MsgSettingEngine::Free;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);


mmsRetrieval = MsgSettingEngine::Off;
mMsgSettingEngine->setMMSRetrieval(mmsRetrieval);
mmsMode = MsgSettingEngine::Restricted;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);


mmsMode = MsgSettingEngine::Free;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mmsMode = MsgSettingEngine::Free;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mmsRetrieval = MsgSettingEngine::No;
mMsgSettingEngine->setMMSRetrieval(mmsRetrieval);
mmsMode = MsgSettingEngine::Restricted;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);


mmsMode = MsgSettingEngine::Free;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mmsMode = MsgSettingEngine::Free;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mmsMode = MsgSettingEngine::Free;
mMsgSettingEngine->setMMSCreationmode(mmsMode);

mode = MsgSettingEngine::Restricted;
retrieval = MsgSettingEngine::AlwaysAutomatic;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Guided;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

mode = MsgSettingEngine::Free;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::AutomInHomeNetwork;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Maual;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::Off;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);

retrieval = MsgSettingEngine::No;
mMsgSettingEngine->advanceMmsSettings(mode,retrieval,anonymous_Status,mmsAdverts_Status);



}

void TestMsgSettingEngine::TestallMMsAcessPoints()
{
QStringList list;
TInt index = 0;
mMsgSettingEngine->allMMsAcessPoints(list,index);



}

void TestMsgSettingEngine::TestsetMMSAccesspoint()
{
TInt index = 0;
mMsgSettingEngine->setMMSAccesspoint(index);

index = 1;
mMsgSettingEngine->setMMSAccesspoint(index);
}

void TestMsgSettingEngine::TestallSMSMessageCenter()
{
QStringList list;
TInt index = 0;
mMsgSettingEngine->allSMSMessageCenter(list,index);
}
void TestMsgSettingEngine::TestsetSMSMessageCenter()
{
TInt index = 0;
mMsgSettingEngine->setSMSMessageCenter(index);
 
}

void TestMsgSettingEngine::TesteditSmsMessageCenter()
{
QString list1("text");
QString list2("text");
TInt index = 0;
mMsgSettingEngine->editSmsMessageCenter(list1,list2,index);   

QStringList list;
mMsgSettingEngine->allSMSMessageCenter(list,index);

mMsgSettingEngine->editSmsMessageCenter(list1,list2,index);

index = 1;
QString list3("345464");
QString list4("ServiceCenter");
mMsgSettingEngine->editSmsMessageCenter(list3,list4,index);


}

void TestMsgSettingEngine::TestaddSmsMessageCenter()
{
QString list1("text");
QString list2("text");
mMsgSettingEngine->addSmsMessageCenter(list1,list2);
}

void TestMsgSettingEngine::TestsmsCenterNameAndNumber()
{
QString list1("text");
QString list2("text");
TInt num = 0;
mMsgSettingEngine->smsCenterNameAndNumber(num,list1,list2);
}

void TestMsgSettingEngine::TestsetEmailGateway()
{
QString list;
mMsgSettingEngine->setEmailGateway(list);


}

void TestMsgSettingEngine::TestsetEmailSeriveCenterNumber()
{
QString list;
mMsgSettingEngine->setEmailSeriveCenterNumber(list);
}

void TestMsgSettingEngine::TestemailEmailGatewayAndServiceCenterNumber()
{
QString list1("9239393");
QString list2("93838339");
//mMsgSettingEngine->emailEmailGatewayAndServiceCenterNumber(list1,list2);
}

void TestMsgSettingEngine::TestsetValidityPeriod()
{
/*MsgSettingEngine::ValidityPeriod period;
period = MsgSettingEngine::OneHour;

mMsgSettingEngine->setValidityPeriod(period);*/
}

void TestMsgSettingEngine::TestvalidityPeriod()
{
/*
MsgSettingEngine::ValidityPeriod period;
period=mMsgSettingEngine->validityPeriod();*/
}

void TestMsgSettingEngine::cleanupTestCase()
{
delete mMsgSettingEngine;
}
