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
 * Description:This class is for drawing form for advanced view 
 * settings  
 *
 */

#ifndef MSG_ADVANCEDSETTINGS_FORM_H_
#define MSG_ADVANCEDSETTINGS_FORM_H_

#include <hbdataform.h>

class HbDataFormModelItem;
class HbDataFormModel;
class HbComboBox;
class QStandardItemModel;
class MsgSettingEngine;
class HbLineEdit;

class MsgAdvancedSettingsForm : public HbDataForm
{
Q_OBJECT
public:
    explicit MsgAdvancedSettingsForm(QGraphicsItem *parent = 0);

    ~MsgAdvancedSettingsForm();

signals:
    void newSMSCCenterClicked(int);

private slots:
    
    void onPressedCustomButton();
    void changeAccessPoint(int index);
    void onSMSCurrentIndexChanged(int index);
    void createServiceCenterModel();
    void changeMMSCreationMode(int index);
    void changeMMSRetrievalMode(int index);
    void allowAnonymousMMS();
    void allowMMSAdverts();
    void onLongPress(QPointF point);
    void onDataChanged(const QModelIndex& topLeft,const QModelIndex& bottomRight);

private:
    void initSettingModel();
    void initGroupPageName();
    void addMMSGroupItem(HbDataFormModelItem* parent);
    void addSmsMCGroupItem(HbDataFormModelItem* parent, HbDataFormModel* model);
    void addSmsEmailGroupItem(HbDataFormModelItem* parent);
    
    //sms stuff
    void activateSMSCCenterList(HbWidget* widget);
    void activateCustomButtonListItems(HbWidget* widget, int btnIndex);
    void updateSmsCenterModel();   
    void fillServiceCenterModel();
    
    //email stuff
    void activateEmailGateway(HbWidget* widget, HbLineEdit* &edit);
    void activateEmailServiceCenterNo(HbWidget* widget, HbLineEdit* &edit);
    
public:
    void refresh();
    
    /**
     * This is for update Email fields into settings engine
     */
    void commitEmailChanges();
    
private:
    /**
     * different group items.
     */
    HbDataFormModelItem* mMMSGroup;
    HbDataFormModelItem* mSmsMCGroup;
    HbDataFormModelItem* mSmsEmailGroup;

    /**
     * groupitem string lists.
     */
    QStringList mGroupPageName;
    QStringList mmsSettingsGroup;
    QStringList mSmsMCSettingsGroup;
    QStringList mSmsEmailSettingsGroup;

    /**
     * The list of smsc centers.
     */
    QStringList mSmsServiceList;

    /**
     * model for sms service centers.
     */
    QStandardItemModel* mSmsServiceCenterModel;

    /**
     * engine instance to read/write settings
     */
    MsgSettingEngine* mSettingEngine;
    
    /**
     * sms service center combo box reference.
     */
    HbComboBox* mSmsCenterComboBox;
    
	/**
	 * Line edit for Sms Email gateway
	 */
    HbLineEdit* mEditEmailGateway;
	
	/**
	 * Line edit for Sms Email Service Number
	 */
    HbLineEdit* mEditEmailServiceNumber;
        
    /**
     * default service center index.
     */
    int mDefaultServiceCenter;
    
    HbDataFormModelItem *emailGateway;
    HbDataFormModelItem *emailService;
    
    

};

#endif /* MSG_ADVANCEDSETTINGS_FORM_H_ */
