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
 * Description:This class provides API m/w for msg settings plugin
 *
 */

#include "qstringlist.h"
#include "debugtraces.h"


#include "msgsettingengine.h"
#include "smssettingsprivate.h"
#include "mmssettingprivate.h"
#include "s60qconversions.h"

//Constructor
MsgSettingEngine::MsgSettingEngine()
{
#ifdef _DEBUG_TRACES_
    qDebug() << "Enter MsgSettingEngine::MsgSettingEngine";
#endif


    dptr_smsSettings = SmsSettingsPrivate::NewL();
    dptr_mmsSettings = MmsSettingsPrivate::NewL();

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit MsgSettingEngine::MsgSettingEngine";
#endif

}

//Destructor
MsgSettingEngine::~MsgSettingEngine()
{
#ifdef _DEBUG_TRACES_
    qDebug() << "Enter MsgSettingEngine::~MsgSettingEngine";
#endif


    delete dptr_smsSettings;
    delete dptr_mmsSettings;

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit MsgSettingEngine::~MsgSettingEngine";
#endif

}

/**
 * for basic message settings
 * set the deliver report
 * @param report bool true or false
 */
void MsgSettingEngine::setDeliveryReport(bool report)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setDeliveryReport" << report;
#endif


    dptr_smsSettings->setDeliveryReport(report);
    dptr_mmsSettings->setDeliveryReport(report);

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit MsgSettingEngine::setDeliveryReport";
#endif

}

/**
 * set the character encoding
 * @param encoding specifying encoding type
 */
void MsgSettingEngine::setCharacterEncoding(MsgSettingEngine::CharacterEncoding encoding)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setCharacterEncoding: " << encoding;
#endif


    TBool flag = EFalse;

    if (encoding == MsgSettingEngine::FullSupport)
    {
        flag = ETrue;
    }
    dptr_smsSettings->setCharacterEncoding(flag);
    dptr_mmsSettings->setCharacterEncoding(flag);

#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setCharacterEncoding";
#endif

}

/**
 * returns settings delivery report status
 * and character encoding
 * @param report for getting status report
 * @param encoding for char encoding
 */
void MsgSettingEngine::settingsDeliverReportAndCharEncoding( bool& report,
                                MsgSettingEngine::CharacterEncoding& encoding )
{
#ifdef _DEBUG_TRACES_
    qDebug() << "Enter MsgSettingEngine::settingsDeliverReportAndCharEncoding";
#endif


    TBool encodingStatus1;
    TBool encodingStatus2;
    TBool report1;
    TBool report2;

    dptr_smsSettings->settingsDeliverReportAndCharEncoding(report1,
                                                           encodingStatus1);

    dptr_mmsSettings->settingsDeliverReportAndCharEncoding(report2,
                                                           encodingStatus2);

    report = report1 && report2;

    encoding = MsgSettingEngine::ReducedSupport;
    if (encodingStatus1 && encodingStatus2)
    {
        encoding = MsgSettingEngine::FullSupport;
    }

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit settingsDeliverReportAndCharEncoding: " << report << " "
            << encoding;
#endif

}

/**
 * for advanced settings
 * set the creation mode
 * @mode for creation mode
 */
void MsgSettingEngine::setMMSCreationmode(MsgSettingEngine::MmsCreationMode mode)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setMMSCreationmode " << mode;
#endif


    dptr_mmsSettings->setMMSCreationmode(mode);

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit setMMSCreationmode";
#endif

}

/**
 * set the mms retrieval mode
 * @param retrieval specifying mode
 */
void MsgSettingEngine::setMMSRetrieval(MsgSettingEngine::MmsRetrieval retrieval)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setMMSRetrieval " << retrieval;
#endif


    dptr_mmsSettings->setMMSRetrieval(retrieval);

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit setMMSRetrieval";
#endif

}

/**
 * set the anonymous message
 * @param status true or false
 */
void MsgSettingEngine::setAnonymousMessages(bool status)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setAnonymousMessages " << status;
#endif


    dptr_mmsSettings->setAnonymousMessages(status);

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit setAnonymousMessages";
#endif

}

/**
 * set for receiving MMS Adverts
 * @param status for true or false
 */
void MsgSettingEngine::setReceiveMMSAdverts(bool status)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setReceiveMMSAdverts " << status;
#endif


    dptr_mmsSettings->setReceiveMMSAdverts(status);

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit setReceiveMMSAdverts";
#endif

}

/**
 * for view 2 mms advance settings\
* get all the fields of mms advance setting
 * @param mode return for MMsCreation mode
 * @param retrieval return for mms retrieval
 * @param anonymous return status true or false
 * @param mmsadverts return status true or false
 */
void MsgSettingEngine::advanceMmsSettings(MsgSettingEngine::MmsCreationMode& mode,
                                          MsgSettingEngine::MmsRetrieval& retrieval,
                                          bool& anonymousStatus,
                                          bool& mmsAdvertsStatus)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setReceiveMMSAdverts";
#endif


    TBool anonymous_Status;
    TBool mmsAdverts_Status;
    dptr_mmsSettings->advanceMmsSettings(mode,
                                         retrieval,
                                         anonymous_Status,
                                         mmsAdverts_Status);

    anonymousStatus = (bool) anonymous_Status;
    mmsAdvertsStatus = (bool) mmsAdverts_Status;

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit setReceiveMMSAdverts " << mode << " " << retrieval << " "
            << anonymousStatus << " " << mmsAdvertsStatus;
#endif

}

/**
 * get all the MMs access points
 * @param returns all the access point names
 * @attention to be displayed as per the index o to total count
 */
void MsgSettingEngine::allMMsAcessPoints(QStringList& nameList,
                                         int& defaultIndex)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::allMMsAcessPoints";
#endif


    RPointerArray<HBufC> accessPoints;
    //= new(ELeave)RPointerArray<HBufC>();

    dptr_mmsSettings->getAllAccessPoints(accessPoints, defaultIndex);

    for (int i = 0; i < accessPoints.Count(); i++)
        {
        HBufC* name = static_cast<HBufC *> (accessPoints[i]);
        QString qName = S60QConversions::s60DescToQString(name->Des());
        nameList.append(qName);
#ifdef _DEBUG_TRACES_
        qDebug() << "\n " << qName;
#endif

        }
    accessPoints.ResetAndDestroy();

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit allMMsAcessPoints count = " << nameList.count()
            << " Default:" << defaultIndex;
#endif

}

/**
 * set the default access point
 * @param index specifying the index
 */
void MsgSettingEngine::setMMSAccesspoint(int index)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setMMSAccesspoint " << index;
#endif


    dptr_mmsSettings->setMMSAccesspoint(index);

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit setMMSAccesspoint ";
#endif

}

/**
 * get the list of all SMS Message Centres
 * @param list of all names QString
 */
void MsgSettingEngine::allSMSMessageCenter(QStringList& nameList,
                                           int& defaultIndex)
{

#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::allSMSMessageCenter";
#endif


    RPointerArray<HBufC> accessPoints;

    dptr_smsSettings->getAllSMSMessageCenter(accessPoints, defaultIndex);

    for (int i = 0; i < accessPoints.Count(); i++)
        {
        HBufC* name = accessPoints[i];
        QString qName = S60QConversions::s60DescToQString(name->Des());
        nameList.append(qName);
#ifdef _DEBUG_TRACES_
        qDebug() << "\n qName";
#endif

        }
    accessPoints.ResetAndDestroy();

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit allSMSMessageCenter count = " << nameList.count()
            << " Default:" << defaultIndex;
#endif

}

/**
 * set the default SMS message centre
 * @param index specifying default index
 */
void MsgSettingEngine::setSMSMessageCenter(int index)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setSMSMessageCenter " << index;
#endif


    dptr_smsSettings->setSMSMessageCenter(index);

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit setSMSMessageCenter ";
#endif

}

/**
 * for editing the SMS messafe Centre
 * @param name specifying center name
 * @param number specifying center number
 * @param index specying index of message list
 */
void MsgSettingEngine::editSmsMessageCenter(QString& centreName,
                                            QString& centreNumber, int index)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::editSmsMessageCenter " << index << " "
            << centreName << " " << centreNumber;
#endif

    HBufC* d_addr = S60QConversions::qStringToS60Desc(centreNumber);
    HBufC* d_name = S60QConversions::qStringToS60Desc(centreName);

    dptr_smsSettings->editSMSServiceCentre(d_addr, d_name, index);
    delete d_addr;
    delete d_name;

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit editSmsMessageCenter ";
#endif

}

/**
 * for add new sms message centre
 * @param name specifying center name
 * @param number specifying center number
 */
void MsgSettingEngine::addSmsMessageCenter(QString& centreName,
                                           QString& centreNumber)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::addSmsMessageCenter " << centreName << " "
            << centreNumber;
#endif


    HBufC* d_addr = S60QConversions::qStringToS60Desc(centreNumber);
    HBufC* d_name = S60QConversions::qStringToS60Desc(centreName);

    dptr_smsSettings->addSmsMessageCenter(d_addr, d_name);
    delete d_addr;
    delete d_name;

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit addSmsMessageCenter ";
#endif

}

/**
 * get the sms message centre namd and number
 * @index for which center needs
 * @param return name specifying center name
 * @param return number specifying center number
 */
void MsgSettingEngine::smsCenterNameAndNumber(int index, QString& centreName,
                                              QString& centreNumber)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::smsCenterNameAndNumber " << index;
#endif


    HBufC* d_addr;
    HBufC* d_name;

    dptr_smsSettings->smsCenterNameAndNumber(index, &d_addr, &d_name);

    centreNumber = S60QConversions::s60DescToQString(d_addr->Des());
    centreName = S60QConversions::s60DescToQString(d_name->Des());

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit smsCenterNameAndNumber " << centreNumber << " "
            << centreName;
#endif

}

/**
 * set the Sms Email gateway
 * @param gateway string
 */
void MsgSettingEngine::setEmailGateway(QString& gateway)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setEmailGateway " << gateway;
#endif


    HBufC* d_gateway = S60QConversions::qStringToS60Desc(gateway);

    dptr_smsSettings->setEmailGateway(d_gateway);
    delete d_gateway;

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit setEmailGateway ";
#endif

}

/**
 * set the sms Email Service centre number
 * @param centre number
 */
void MsgSettingEngine::setEmailSeriveCenterNumber(QString& centerNumber)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::setEmailSeriveCenterNumber " << centerNumber;

#endif

    HBufC* d_number = S60QConversions::qStringToS60Desc(centerNumber);

    dptr_smsSettings->setEmailSeriveCenterNumber(d_number);
    delete d_number;

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit setEmailSeriveCenterNumber ";
#endif

}

/**
 * get the sms email gateway and 
 * service centre number
 * @param gateway return email gateway
 * @param centreNumber return service number 
 */
void MsgSettingEngine::emailEmailGatewayAndServiceCenterNumber(QString& gateway,
                                                               QString& serviceNumber)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgSettingEngine::emailEmailGatewayAndServiceCenterNumber";
#endif


    HBufC* d_gateway;
    HBufC* d_number;

    dptr_smsSettings->emailEmailGatewayAndServiceCenterNumber(&d_gateway,
                                                              &d_number);

    gateway = S60QConversions::s60DescToQString(d_gateway->Des());
    serviceNumber = S60QConversions::s60DescToQString(d_number->Des());

    delete d_gateway;
    delete d_number;

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit smsCenterNameAndNumber " << gateway << " "
            << serviceNumber;
#endif

}
//eof

