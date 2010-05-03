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

#ifndef SMSSETTINS_H
#define SMSSETTINS_H

#include <e32base.h>

/**
 * SMS setting class implements the SMSC settings
 * The settings will be stored in the central repository.
 */
class SmsSettingsPrivate : public CBase
{
public:

    //2phase constructor
    static SmsSettingsPrivate* NewL();

    /*
     * Destructor
     */
    ~SmsSettingsPrivate();

    /**
     * sets the status of service messages
     * @param serviceMessages status true or false
     */
    void setReceiveSerivceMessages(TBool serviceMessages);

    /**
     * set the character encoding
     * @param status true or false
     */
    void setCharacterEncoding(TBool status);

    /**
     * get the status of deliver report and
     * character encoding
     * @param report status of delivery report
     * @param statusEncoding charcter encoding
     */
    void settingsServiceMessagesAndCharEncoding(TBool& report,
                                              TBool& statusEncoding);

    /**
     * get all the sms service names and 
     * the default selected index
     * @param accessPoints array of access names
     * @param defaultIndex, default set index
     */
    void getAllSMSMessageCenter(RPointerArray<HBufC>& accessPoints,
                                int &defaultIndex);

    /**
     * set the default index
     * @param index default index
     */
    void setSMSMessageCenter(int index);

    /**
     * edit the sms service address
     * @param address sms address number
     * @param name sms service name
     * @param index value to modify
     */
    void editSMSServiceCentre(HBufC* address, HBufC* name, TInt index);

    /**
     * add a new sms service address
     * @param address service address number
     * @param name service name
     */
    void addSmsMessageCenter(HBufC* address, HBufC* name);
    
    /**
     * delete sms service address
     * @param aDeleteIndex refers to the index to delete
     */
    void deleteSmsMessageCenter(TInt aDeleteIndex);
    

    /**
     * get the service name and number by the index value
     * @param index for the service need
     * @param centerNumber address of the service
     * @param centerName name of the service
     */
    void smsCenterNameAndNumber(int index, HBufC** centerNumber,
                                HBufC** centerName);

private:

    /*
     * 1st phase Constructor 
     */
    SmsSettingsPrivate();

    /**
     * 2nd phase constructor
     */
    void ConstructL();

};

#endif // SMSSETTINS_H

