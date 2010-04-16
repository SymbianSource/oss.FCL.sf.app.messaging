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
class MsgSettingsViewManager;

class MsgSettingsForm : public HbDataForm
{
Q_OBJECT
public:
    explicit MsgSettingsForm(QGraphicsItem *parent = 0);
    ~MsgSettingsForm();
private:
    void initSettingModel();
    // call these function when activated signal is emitted
    void activateAdvanced(HbWidget *widget);
    void activateDeliveryReports(HbWidget *widget);
    void activateCharacterEncoding(HbWidget *widget);

private slots:
    void onItemActivated(const QModelIndex &index);
    void onPressedAdvanced();
    void onPressedDelReports();
    void changeCharEncoding(int index);

private:

    //msg engine reference
    MsgSettingEngine* mSettingEngine;
    
    //DataForm List Items
    HbDataFormModelItem *mDeliverReports;
    HbDataFormModelItem *mCharacterEncoding;
    HbDataFormModelItem *mAdvancedSettings;  

    //Settings View Manager
    MsgSettingsViewManager* mSettingsViewManager;
};
#endif // MSG_SETTINGSFORM_H
