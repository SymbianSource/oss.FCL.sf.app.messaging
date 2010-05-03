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
 * Description:  This class is for first form view for msg settings  
 *
 */

#ifndef MSG_SETTINGSFORM_H
#define MSG_SETTINGSFORM_H

#include <hbdataform.h>
#include "msgsettingengine.h"

class HbDataFormModelItem;
class HbDataFormModel;
class QStandardItemModel;
class QStandardItemModel;

class MsgSettingsForm : public HbDataForm
{
Q_OBJECT
public:
    explicit MsgSettingsForm(QGraphicsItem *parent = 0);
    ~MsgSettingsForm();
    void refreshViewForm();
    void expandSMSSettings();
    
signals:
    void newSMSCCenterClicked(int);
        
private:
    void initSettingModel();
    void addMMSGroupItem(HbDataFormModelItem* parent);
    void addSmsMCGroupItem(HbDataFormModelItem* parent);
    void updateSmsMCGroupItem(HbDataFormModelItem* parent);
    
private slots:
    void onPressedServiceMessages();
    void onPressedCustomButton();
    void changeCharEncoding(int index);
    void changeAccessPoint(int index);
    void onSMSCurrentIndexChanged(int index);
    void changeMMSRetrievalMode(int index);
    void allowAnonymousMMS();
    void allowMMSAdverts();

private:
    
    HbDataFormModel *settingsModel;
    
    /**
     * SmsGroup model item
     */
    HbDataFormModelItem* mSmsMCGroup;

    /**
     * SMS Center gropu list
     */
    QStringList mSmsMCSettingsGroup;

    /**
     * The list of smsc centers.
     */
    QStringList mSmsServiceList;

    //msg engine reference
    MsgSettingEngine* mSettingEngine;
    
    QStandardItemModel* mSmsServiceCenterModel;
    HbDataFormModelItem *smsMessageCenter;

};
#endif // MSG_SETTINGSFORM_H
