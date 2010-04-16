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

#include "msgsettingsform.h"
#include "msgsettingsdataformcustomitem.h"

#include <hbdataformmodelitem.h>
#include <hbdataformmodel.h>
#include <hbcombobox.h>
#include <hbpushbutton.h>

#include "debugtraces.h"

//Localized constants
#define LOC_DELIVERY_REPORTS hbTrId("txt_messaging_setlabel_delivery_reports")
#define LOC_ON hbTrId("txt_messaging_setlabel_val_on")
#define LOC_OFF hbTrId("txt_messaging_setlabel_val_off")
#define LOC_REDUCED_SUPPORT hbTrId("txt_messaging_setlabel_val_reduced_support")
#define LOC_FULL_SUPPORT hbTrId("txt_messaging_setlabel_val_full_support")
#define LOC_ADVANCED hbTrId("txt_messaging_button_advanced")
#define LOC_CHAR_ENCODING hbTrId("txt_messaging_setlabel_character_encoding")


MsgSettingsForm::MsgSettingsForm(MsgSettingsViewManager* settingsViewManager,
	                               QGraphicsItem *parent)
:HbDataForm(parent), mSettingsViewManager(settingsViewManager)
{
    this->setHeading(tr("Application settings: Messaging"));

    mSettingEngine = new MsgSettingEngine();

    //initialize the form model
    initSettingModel();
}

MsgSettingsForm::~MsgSettingsForm()
{
    delete mSettingEngine;
}

void MsgSettingsForm::initSettingModel()
{
    HbDataFormModel *model = new HbDataFormModel(0);

    bool deliveryReoprt = false;
    MsgSettingEngine::CharacterEncoding charEncoding = MsgSettingEngine::ReducedSupport;

    mSettingEngine->settingsDeliverReportAndCharEncoding(deliveryReoprt, charEncoding);

    QStringList deliveryReoprtList;
    deliveryReoprtList << LOC_OFF << LOC_ON;

    int index = int(deliveryReoprt);
    int otherIndex = (deliveryReoprt + 1) % deliveryReoprtList.count();

    // 1. create the first item delivery reports
    HbDataFormModelItem *deliverReports = new HbDataFormModelItem(
                HbDataFormModelItem::ToggleValueItem, 
                LOC_DELIVERY_REPORTS, 0);
    deliverReports->setContentWidgetData("text", deliveryReoprtList.at(index));
    deliverReports->setContentWidgetData("additionalText", deliveryReoprtList.at(otherIndex));

    model->appendDataFormItem(deliverReports, model->invisibleRootItem());
    this->addConnection(deliverReports, SIGNAL(clicked()),
                this,SLOT(onPressedDelReports()));
    
    // 2. Character encoding
    HbDataFormModelItem *characterEncoding = new HbDataFormModelItem(
            HbDataFormModelItem::ComboBoxItem, 
            LOC_CHAR_ENCODING, 0);
    QStringList charEncodingList;
    charEncodingList << LOC_REDUCED_SUPPORT << LOC_FULL_SUPPORT;

    index = int(charEncoding);

    characterEncoding->setContentWidgetData("items", charEncodingList);
    characterEncoding->setContentWidgetData("currentIndex", index);

    model->appendDataFormItem(characterEncoding, model->invisibleRootItem());
    this->addConnection(characterEncoding, SIGNAL(currentIndexChanged(int)),
                    this,SLOT(changeCharEncoding(int)));
    
    // 3. CUSTOM SETTING ITEM HbButton
    HbDataFormModelItem::DataItemType customItem =
        static_cast<HbDataFormModelItem::DataItemType> (HbDataFormModelItem::CustomItemBase + 1);

    HbDataFormModelItem *advancedSettings
        = model->appendDataFormItem(customItem, QString(), model->invisibleRootItem());
    
    advancedSettings->setContentWidgetData("text", LOC_ADVANCED);
    this->addConnection(advancedSettings, SIGNAL(clicked()),
            this,SLOT(onPressedAdvanced())); 
            
    this->setModel(model);
}

void MsgSettingsForm::onPressedAdvanced()
{
    //tell view manager to open advanced view
    mSettingsViewManager->openAdvancedView();
}

void MsgSettingsForm::onPressedDelReports()
{
    HbPushButton *btn = qobject_cast<HbPushButton *> (sender());

    if (btn) {
        QString btnText = btn->text();

        //check if the button pressed was On or Off
        if (LOC_OFF == btnText) {
            mSettingEngine->setDeliveryReport(false);
        }
        else {
            mSettingEngine->setDeliveryReport(true);
        }
    }
}

void MsgSettingsForm::changeCharEncoding(int index)
{
    //set the character encoding
    mSettingEngine->setCharacterEncoding(MsgSettingEngine::CharacterEncoding(index));
}


//EOF
