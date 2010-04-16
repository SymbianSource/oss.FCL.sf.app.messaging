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
#include "msgsettingsviewmanager.h"

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


MsgSettingsForm::MsgSettingsForm(QGraphicsItem *parent) :
    HbDataForm(parent)
{
    mSettingsViewManager = new MsgSettingsViewManager(0);

    this->setHeading(tr("Application settings: Messaging"));

    mSettingEngine = new MsgSettingEngine();

    //initialize the form model
    initSettingModel();

    connect(this, SIGNAL(activated(QModelIndex)), this, SLOT(onItemActivated(QModelIndex)));

}

MsgSettingsForm::~MsgSettingsForm()
{
    delete mSettingsViewManager;
    delete mSettingEngine;
}

void MsgSettingsForm::initSettingModel()
{
    HbDataFormModel *model = new HbDataFormModel(0);

    mDeliverReports = new HbDataFormModelItem(HbDataFormModelItem::ToggleValueItem, 
            LOC_DELIVERY_REPORTS, 0);

    bool deliveryReoprt = false;
    MsgSettingEngine::CharacterEncoding charEncoding = MsgSettingEngine::ReducedSupport;

    mSettingEngine->settingsDeliverReportAndCharEncoding(deliveryReoprt, charEncoding);

    QStringList deliveryReoprtList;
    deliveryReoprtList << LOC_OFF << LOC_ON;

    int index = int(deliveryReoprt);
    int otherIndex = (deliveryReoprt + 1) % deliveryReoprtList.count();

    mDeliverReports->setContentWidgetData("text", deliveryReoprtList.at(index));
    mDeliverReports->setContentWidgetData("additionalText", deliveryReoprtList.at(otherIndex));

    model->appendDataFormItem(mDeliverReports, model->invisibleRootItem());

    // Character encoding
    mCharacterEncoding = new HbDataFormModelItem(HbDataFormModelItem::ComboBoxItem, 
            LOC_CHAR_ENCODING, 0);
    QStringList charEncodingList;
    charEncodingList << LOC_REDUCED_SUPPORT << LOC_FULL_SUPPORT;

    index = int(charEncoding);

    mCharacterEncoding->setContentWidgetData("items", charEncodingList);
    mCharacterEncoding->setContentWidgetData("currentIndex", index);

    model->appendDataFormItem(mCharacterEncoding, model->invisibleRootItem());

    //CUSTOM SETTING ITEM HbButton
    HbDataFormModelItem::DataItemType customItem =
        static_cast<HbDataFormModelItem::DataItemType> (HbDataFormModelItem::CustomItemBase + 1);

    mAdvancedSettings
        = model->appendDataFormItem(customItem, QString(), model->invisibleRootItem());
    this->setModel(model);
}

// On Item activated
void MsgSettingsForm::onItemActivated(const QModelIndex &index)
{
    HbDataFormModelItem *itemData = static_cast<HbDataFormModelItem *> (index.internalPointer());
    // to deal with orbit change temparialy
    if (itemData->type() > HbDataFormModelItem::GroupPageItem) {
        HbDataFormModelItem *pageItemData = const_cast<HbDataFormModelItem *> (itemData->parent());
        //get the widget of setting item
        HbWidget* widget = this->dataFormViewItem(index)->dataItemContentWidget();

        if (itemData == mDeliverReports) {
            activateDeliveryReports(widget);
        }
        else if (itemData == mCharacterEncoding) {
            activateCharacterEncoding(widget);
        }
        else if (itemData == mAdvancedSettings) {
            activateAdvanced(widget);
        }
    }
}

void MsgSettingsForm::activateAdvanced(HbWidget* widget)
{
    HbPushButton *advanced = qobject_cast<HbPushButton *> (widget);

    if (advanced) {
        advanced->setText(LOC_ADVANCED);
        //TODO Issue in DataForm Calling the itemActivated twice
        disconnect(advanced, SIGNAL(clicked()), this, SLOT(onPressedAdvanced()));

        connect(advanced, SIGNAL(clicked()), this, SLOT(onPressedAdvanced()));
    }
}

void MsgSettingsForm::activateDeliveryReports(HbWidget* widget)
{
    HbPushButton *delReports = qobject_cast<HbPushButton *> (widget);

    if (delReports) {
        //TODO Issue in DataForm Calling the itemActivated twice
        disconnect(delReports, SIGNAL(clicked()), this, SLOT(onPressedDelReports()));

        connect(delReports, SIGNAL(clicked()), this, SLOT(onPressedDelReports()));
    }
}

void MsgSettingsForm::activateCharacterEncoding(HbWidget* widget)
{
    HbComboBox *charEncoding = qobject_cast<HbComboBox *> (widget);

    if (charEncoding) {
        //TODO Issue in DataForm Calling the itemActivated twice
        disconnect(charEncoding, SIGNAL(currentIndexChanged(int)), this,
            SLOT(changeCharEncoding(int)));

        connect(charEncoding, SIGNAL(currentIndexChanged(int)), this, SLOT(changeCharEncoding(int)));
    }
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
