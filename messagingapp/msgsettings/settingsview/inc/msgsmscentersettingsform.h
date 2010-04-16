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
 * Description:This class is for sms message center form view
 *
 */

#ifndef MSGSMSCENTERSETTINGSFORM_H_
#define MSGSMSCENTERSETTINGSFORM_H_

#include <hbdataform.h>

class HbLineEdit;
class MsgSettingEngine;
class HbDataFormModelItem;

class MsgSMSCenterSettingsForm : public HbDataForm
{
Q_OBJECT

public:
    explicit MsgSMSCenterSettingsForm(int view = -1, QGraphicsItem *parent = 0);

    ~MsgSMSCenterSettingsForm();

    void commitChanges();

public slots:
    void onItemShown(const QModelIndex& topLeft);
    
private:
    void initSettingModel();
    
private:
    QString mCenterName;
    QString mCenterNumber;
    HbLineEdit* mEdit1;
    HbLineEdit* mEdit2;
    int mView;
    MsgSettingEngine* mSettingEngine;
    
    HbDataFormModelItem *messageCenterName;
    HbDataFormModelItem *messageCenterNumber;

};

#endif /* MSGSMSCENTERSETTINGSFORM_H_ */
