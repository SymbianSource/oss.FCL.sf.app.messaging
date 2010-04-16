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

#include "msgsmscentersettingsform.h"
#include "msgsettingengine.h"

#include <hbdataformviewitem.h>
#include <hbdataformmodelitem.h>
#include <hbdataformmodel.h>
#include <hblineedit.h>
#include <hbinputeditorinterface.h>
#include "debugtraces.h"

//Localized constants
#define LOC_NEW_SMS_CENTRE hbTrId("txt_messaging_title_new_sms_message_centre")
#define LOC_EDIT_SMS_CENTRE hbTrId("txt_messaging_title_edit_sms_message_centre")
#define LOC_SMS_CENTRE_NAME hbTrId("txt_messaging_setlabel_message_centre_name")
#define LOC_SMS_CENTRE_NUMBER hbTrId("txt_messaging_setlabel_message_centre_number")


MsgSMSCenterSettingsForm::MsgSMSCenterSettingsForm(int view, QGraphicsItem *parent) :
    HbDataForm(parent), mEdit1(0), mEdit2(0), mView(view)
{
    QString heading;
    if (mView == -1) {
        heading = LOC_NEW_SMS_CENTRE;
    }
    else {
        heading = LOC_EDIT_SMS_CENTRE;
    }

    this->setHeading(heading);

    bool b = connect(this,
                        SIGNAL(itemShown(const QModelIndex&)),
                        this, SLOT(onItemShown(const QModelIndex&)));
                        
    QDEBUG_WRITE_FORMAT("MsgSMSCenterSettingsForm::MsgSMSCenterSettingsForm initialized.. ", b)
                        
    mSettingEngine = new MsgSettingEngine();

    initSettingModel();    
}

MsgSMSCenterSettingsForm::~MsgSMSCenterSettingsForm()
{
    delete mSettingEngine;
}

void MsgSMSCenterSettingsForm::initSettingModel()
{
	  QDEBUG_WRITE("MsgSMSCenterSettingsForm::initSettingModel");
    HbDataFormModel *settingsmodel = new HbDataFormModel();

    QString centerName = QString();
    QString centerNumber = QString();

    if (mView != -1) {
        mSettingEngine->smsCenterNameAndNumber(mView, centerName, centerNumber);
    }

    messageCenterName = new HbDataFormModelItem(HbDataFormModelItem::TextItem,
            LOC_SMS_CENTRE_NAME, 0);

    messageCenterName->setContentWidgetData("text", centerName);
        
    settingsmodel->appendDataFormItem(messageCenterName, settingsmodel->invisibleRootItem());

    messageCenterNumber = new HbDataFormModelItem(
        HbDataFormModelItem::TextItem, LOC_SMS_CENTRE_NUMBER, 0);

    messageCenterNumber->setContentWidgetData("text", centerNumber);
        
    settingsmodel->appendDataFormItem(messageCenterNumber, settingsmodel->invisibleRootItem());

    this->setModel(settingsmodel);
}


void MsgSMSCenterSettingsForm::commitChanges()
{
	   	   
	   //commit changes
    if(mEdit1)
     mCenterName = mEdit1->text();
    
    if (mEdit2)
     mCenterNumber = mEdit2->text();

    if (mCenterName.isEmpty() || mCenterNumber.isEmpty()) {
        //if mCenterName or mCenterNumber is empty do not commit.
        return;
    }

    QDEBUG_WRITE("MsgSMSCenterSettingsForm::commitChanges");
    if (mView == -1) {
        mSettingEngine->addSmsMessageCenter(mCenterName, mCenterNumber);
    }
    else {
        mSettingEngine->editSmsMessageCenter(mCenterName, mCenterNumber, mView);
    }
    QDEBUG_WRITE("ex MsgSMSCenterSettingsForm::commitChanges");
    
}

void MsgSMSCenterSettingsForm::onItemShown(const QModelIndex& topLeft)
    {
    	
    	QDEBUG_WRITE("DataForm MsgSMSCenterSettingsForm::onItemShown");
    HbDataFormModelItem *itemData =
    static_cast<HbDataFormModel *> (model())->itemFromIndex(topLeft);

    if (itemData && itemData == messageCenterName && !mEdit1)
        {
        	QDEBUG_WRITE("messageCenterName updated..");
    mEdit1 = static_cast<HbLineEdit *>(this->dataFormViewItem(topLeft)->dataItemContentWidget());
         }
    else if (itemData && itemData == messageCenterNumber && !mEdit2)
                {
                	QDEBUG_WRITE("messageCenterNumber updated..");
    mEdit2 = static_cast<HbLineEdit *>(this->dataFormViewItem(topLeft)->dataItemContentWidget());
                HbEditorInterface editorInterface(mEdit2);
                    editorInterface.setUpAsPhoneNumberEditor();

                        }

                    }
