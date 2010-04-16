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

#include "msgadvancedsettingsform.h"
#include "msgsettingengine.h"

#include <hbdataformviewitem.h>
#include <hbdataformmodelitem.h>
#include <hbdataformmodel.h>
#include <hbpushbutton.h>
#include <hbcombobox.h>
#include <hbinputeditorinterface.h>
#include <HbLineEdit>
#include <QStandardItemModel>
#include "debugtraces.h"

//Localized Strings
#define LOC_ADVANCED_SETTINGS hbTrId("txt_messaging_title_advanced_settings_messaging")
#define LOC_MMS_SETTINGS hbTrId("txt_messaging_subhead_mms_settings")
#define LOC_SMS_CENTRE_SETTINGS hbTrId("txt_messaging_subhead_sms_message_centre_settings")
#define LOC_MMS_CREATION_MODE hbTrId("txt_messaging_setlabel_mms_creation_mode")
#define LOC_MMS_RETRIEVAL hbTrId("txt_messaging_setlabel_mms_retrieval")
#define LOC_ALLOW_ANONYMOUS_MMS hbTrId("txt_messaging_setlabel_allow_anonymous_mms_message")
#define LOC_RECEIVE_MMS_ADVERTS hbTrId("txt_messaging_setlabel_receive_mms_adverts")
#define LOC_MMS_AP_IN_USE hbTrId("txt_messaging_setlabel_mms_access_point_in_use")
#define LOC_SMS_CENTRE_IN_USE hbTrId("txt_messaging_setlabel_sms_message_centre_in_use")
#define LOC_ADD_NEW hbTrId("txt_messaging_button_add_new")

#define LOC_RESTRICTED hbTrId("txt_messaging_setlabel_val_restricted")
#define LOC_GUIDED hbTrId("txt_messaging_setlabel_val_guided")
#define LOC_FREE hbTrId("txt_messaging_setlabel_val_free")

#define LOC_ALWAYS_AUTOMATIC hbTrId("txt_messaging_setlabel_val_always_automatic")
#define LOC_AUTO_HOME_NETWORK hbTrId("txt_messaging_setlabel_val_auto_home_network")
#define LOC_MANUAL hbTrId("txt_messaging_setlabel_val_mannual")
#define LOC_ON hbTrId("txt_messaging_setlabel_val_on")
#define LOC_OFF hbTrId("txt_messaging_setlabel_val_off")

#define LOC_MMS_YES hbTrId("txt_messaging_setlabel_allow_anonymous_mms_val_yes")
#define LOC_MMS_NO hbTrId("txt_messaging_setlabel_allow_anonymous_mms_val_no")


MsgAdvancedSettingsForm::MsgAdvancedSettingsForm(QGraphicsItem *parent) :
    HbDataForm(parent), mSmsServiceCenterModel(0), mEditEmailGateway(NULL),
        mEditEmailServiceNumber(NULL), mDefaultServiceCenter(0)
{
    this->setHeading(LOC_ADVANCED_SETTINGS);

    mSettingEngine = new MsgSettingEngine();

    initGroupPageName();
    initSettingModel();

    connect(this, SIGNAL(activated(QModelIndex)), this, SLOT(onItemActivated(QModelIndex)));

}

MsgAdvancedSettingsForm::~MsgAdvancedSettingsForm()
{
    delete mSettingEngine;
}

void MsgAdvancedSettingsForm::initGroupPageName()
{
    mGroupPageName << LOC_MMS_SETTINGS 
                   << LOC_SMS_CENTRE_SETTINGS 
                   << tr("SMS E-mail settings");

    mmsSettingsGroup << LOC_MMS_CREATION_MODE 
                     << LOC_MMS_RETRIEVAL 
                     << LOC_ALLOW_ANONYMOUS_MMS 
                     << LOC_RECEIVE_MMS_ADVERTS 
                     << LOC_MMS_AP_IN_USE;

    mSmsMCSettingsGroup << LOC_SMS_CENTRE_IN_USE 
                        << LOC_ADD_NEW;

    mSmsEmailSettingsGroup << tr("E-mail gateway") << tr("E-mail service centre number");
}

void MsgAdvancedSettingsForm::initSettingModel()
{
    HbDataFormModel *advancedSettingsmodel = new HbDataFormModel();

    mMMSGroup = new HbDataFormModelItem(HbDataFormModelItem::GroupItem, mGroupPageName.at(0), 0);

    HbDataFormModelItem* rootItem = advancedSettingsmodel->invisibleRootItem();

    advancedSettingsmodel->appendDataFormItem(mMMSGroup, rootItem);

    addMMSGroupItem(mMMSGroup);

    mSmsMCGroup = new HbDataFormModelItem(HbDataFormModelItem::GroupItem, mGroupPageName.at(1), 0);

    advancedSettingsmodel->appendDataFormItem(mSmsMCGroup, rootItem);

    addSmsMCGroupItem(mSmsMCGroup, advancedSettingsmodel);

    mSmsEmailGroup = new HbDataFormModelItem(HbDataFormModelItem::GroupItem, mGroupPageName.at(2),
        0);

    advancedSettingsmodel->appendDataFormItem(mSmsEmailGroup, rootItem);

    addSmsEmailGroupItem(mSmsEmailGroup);

    this->setModel(advancedSettingsmodel);
}

void MsgAdvancedSettingsForm::addMMSGroupItem(HbDataFormModelItem* parent)
{
    //read settings
    MsgSettingEngine::MmsCreationMode creationMode = MsgSettingEngine::Guided;
    MsgSettingEngine::MmsRetrieval retrieval = MsgSettingEngine::AlwaysAutomatic;
    bool allowAnonymous = true;
    bool allowAdverts = true;

    mSettingEngine->advanceMmsSettings(creationMode, retrieval, allowAnonymous, allowAdverts);
    // add all the mmss group item
    HbDataFormModelItem *mmsCreationMode = new HbDataFormModelItem(
        HbDataFormModelItem::ComboBoxItem, mmsSettingsGroup.at(0), 0);

    QStringList mmsCreationModeList;
    mmsCreationModeList << LOC_RESTRICTED 
                        << LOC_GUIDED 
                        << LOC_FREE;

    mmsCreationMode->setContentWidgetData("items", mmsCreationModeList);
    mmsCreationMode->setContentWidgetData("currentIndex", int(creationMode));

    HbDataFormModelItem *mmsRetrieval = new HbDataFormModelItem(HbDataFormModelItem::ComboBoxItem,
        mmsSettingsGroup.at(1), 0);

    QStringList mmsRetrievalList;
    mmsRetrievalList << LOC_ALWAYS_AUTOMATIC 
                     << LOC_AUTO_HOME_NETWORK 
                     << LOC_MANUAL
                     << LOC_OFF
                     << LOC_ON;

    mmsRetrieval->setContentWidgetData("items", mmsRetrievalList);
    mmsRetrieval->setContentWidgetData("currentIndex", int(retrieval));

    HbDataFormModelItem *anonymousMMS = new HbDataFormModelItem(
        HbDataFormModelItem::ToggleValueItem, mmsSettingsGroup.at(2), 0);

    QStringList anonymousMMSList;
    anonymousMMSList << LOC_MMS_NO 
                     << LOC_MMS_YES;

    int defaultIndex = int(allowAnonymous);
    int alternateIndex = (defaultIndex + 1) % anonymousMMSList.count();

    anonymousMMS->setContentWidgetData("text", anonymousMMSList.at(defaultIndex));
    anonymousMMS->setContentWidgetData("additionalText", anonymousMMSList.at(alternateIndex));

    HbDataFormModelItem *mmsAdverts = new HbDataFormModelItem(HbDataFormModelItem::ToggleValueItem,
        mmsSettingsGroup.at(3), 0);

    QStringList mmsAdvertsList;
    mmsAdvertsList << LOC_MMS_NO 
                   << LOC_MMS_YES;

    defaultIndex = int(allowAdverts);
    alternateIndex = (defaultIndex + 1) % mmsAdvertsList.count();

    mmsAdverts->setContentWidgetData("text", mmsAdvertsList.at(defaultIndex));
    mmsAdverts->setContentWidgetData("additionalText", mmsAdvertsList.at(alternateIndex));

    HbDataFormModelItem *accessPoint = new HbDataFormModelItem(HbDataFormModelItem::ComboBoxItem,
        mmsSettingsGroup.at(4), 0);

    QStandardItemModel* comboModel = new QStandardItemModel();

    QStringList mmsList;
    int defaultAccessPointIndex = -1;

    mSettingEngine->allMMsAcessPoints(mmsList, defaultAccessPointIndex);
    int accessPoints = mmsList.count();
    for (int a = 0; a < accessPoints; ++a) {
        QStandardItem* item = new QStandardItem();
        item->setData(mmsList[a], Qt::DisplayRole);
        comboModel->appendRow(item);
    }
    accessPoint->setModel(comboModel);
    accessPoint->setContentWidgetData("items", mmsList);
    if (defaultAccessPointIndex >= 0) {
        accessPoint->setContentWidgetData("currentIndex", defaultAccessPointIndex);
    }

    parent->appendChild(mmsCreationMode);
    parent->appendChild(mmsRetrieval);
    parent->appendChild(anonymousMMS);
    parent->appendChild(mmsAdverts);
    parent->appendChild(accessPoint);
}

void MsgAdvancedSettingsForm::addSmsMCGroupItem(HbDataFormModelItem* parent,
    HbDataFormModel* advancedSettingsmodel)
{
    // add all the sms message center group item
    HbDataFormModelItem *smsMessageCenter = new HbDataFormModelItem(
        HbDataFormModelItem::ComboBoxItem, mSmsMCSettingsGroup.at(0), 0);

    mSettingEngine->allSMSMessageCenter(mSmsServiceList, mDefaultServiceCenter);

    //create and fill service center model.
    createServiceCenterModel();

    smsMessageCenter->setContentWidgetData("currentIndex", mDefaultServiceCenter);
    parent->appendChild(smsMessageCenter);

    //CUSTOM SETTING ITEM HbButton
    HbDataFormModelItem::DataItemType customButtonGeneral =
        static_cast<HbDataFormModelItem::DataItemType> (HbDataFormModelItem::CustomItemBase + 1);

    int totalServices = mSmsServiceList.count();
    for (int index = 0; index < totalServices; index++) {
        HbDataFormModelItem *newMessageCenters = advancedSettingsmodel->appendDataFormItem(
            customButtonGeneral, QString(""), parent);

        newMessageCenters->setData(HbDataFormModelItem::KeyRole, mSmsServiceList.at(index));
    }

    HbDataFormModelItem *newMessageCenters = advancedSettingsmodel->appendDataFormItem(
        customButtonGeneral, QString(""), parent);

    newMessageCenters->setData(HbDataFormModelItem::KeyRole, mSmsMCSettingsGroup.at(1));
}

void MsgAdvancedSettingsForm::addSmsEmailGroupItem(HbDataFormModelItem* parent)
{
    QString emailSmsNum = QString();
    QString emailGatewayTxt = QString();

    mSettingEngine->emailEmailGatewayAndServiceCenterNumber(emailGatewayTxt, emailSmsNum);

    // add all the sms email group item
    HbDataFormModelItem *emailGateway = new HbDataFormModelItem(HbDataFormModelItem::TextItem,
        mSmsEmailSettingsGroup.at(0), 0);

    emailGateway->setContentWidgetData("text", emailGatewayTxt);

    HbDataFormModelItem *emailService = new HbDataFormModelItem(HbDataFormModelItem::TextItem,
        mSmsEmailSettingsGroup.at(1), 0);
    emailService->setContentWidgetData("text", emailSmsNum);

    parent->appendChild(emailGateway);
    parent->appendChild(emailService);
}

void MsgAdvancedSettingsForm::onItemActivated(const QModelIndex &index)
{
    HbDataFormModelItem *itemData = static_cast<HbDataFormModelItem *> (index.internalPointer());
    // to deal with orbit change temparialy
    if (itemData->type() > HbDataFormModelItem::GroupPageItem) {
        HbDataFormModelItem *pageItemData = const_cast<HbDataFormModelItem *> (itemData->parent());
        //get the widget of setting item
        HbWidget* widget = this->dataFormViewItem(index)->dataItemContentWidget();
        if (pageItemData) {
            //the first one is for mms group items
            if (pageItemData == mMMSGroup) {
                switch (pageItemData->indexOf(itemData)) {
                case 0: //mms creation mode
                    activateMMSCreationMode(widget);
                    break;
                case 1: //mms retrieval
                    activateMMSRetrievalMode(widget);
                    break;
                case 2: //anonymous msgs
                    activateMMSAnonymousMode(widget);
                    break;
                case 3: //mms adverts
                    activateMMSAdverts(widget);
                    break;
                case 4: //MMS Accesspoint
                    activateMMSAccesspoint(widget);
                    break;
                default:
                    break;
                }
            }
            else if (pageItemData == mSmsMCGroup) {
                //the second one is for sms message center
                if (pageItemData->indexOf(itemData) == 0) {
                    activateSMSCCenterList(widget);
                }
                else {
                    int btnIndex = pageItemData->indexOf(itemData);
                    activateCustomButtonListItems(widget, btnIndex);
                }
            }
            else if (pageItemData == mSmsEmailGroup) {
                switch (pageItemData->indexOf(itemData)) {
                case 0:
                    activateEmailGateway(widget, mEditEmailGateway);
                    break;
                case 1:
                    activateEmailServiceCenterNo(widget, mEditEmailServiceNumber);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void MsgAdvancedSettingsForm::activateSMSCCenterList(HbWidget* widget)
{
    mSmsCenterComboBox = qobject_cast<HbComboBox *> (widget);

    if (mSmsCenterComboBox) {
        mSmsCenterComboBox->setModel(mSmsServiceCenterModel);

        //to avoid multiple connects.
        disconnect(mSmsCenterComboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(onSMSCurrentIndexChanged(int)));

        mSmsCenterComboBox->setCurrentIndex(mDefaultServiceCenter);

        //map the sms message center selected
        connect(mSmsCenterComboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(onSMSCurrentIndexChanged(int)));
    }
}

void MsgAdvancedSettingsForm::onSMSCurrentIndexChanged(int index)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "Enter MsgAdvancedSettingsForm::onSMSCurrentIndexChanged index=" << index << "\n";
#endif

    //set into msg settings
    mSettingEngine->setSMSMessageCenter(index);

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit MsgAdvancedSettingsForm::onSMSCurrentIndexChanged \n";
#endif

}

void MsgAdvancedSettingsForm::activateCustomButtonListItems(HbWidget* widget, int btnIndex)
{
    HbPushButton *btn = qobject_cast<HbPushButton *> (widget);

    if (btnIndex == mSmsServiceList.count() + 1) {
        btn->setText(mSmsMCSettingsGroup.at(1));
    }
    else {
        btn->setText(mSmsServiceList.at(btnIndex - 1));
    }

    if (btn) {
        //to avoid multiple connects.
        disconnect(btn, SIGNAL(clicked()), this, SLOT(onPressedCustomButton()));

        //activate the custom buttons
        connect(btn, SIGNAL(clicked()), this, SLOT(onPressedCustomButton()));

        disconnect(btn, SIGNAL(longPress(QPointF)), this, SLOT(onLongPress(QPointF)));

        connect(btn, SIGNAL(longPress(QPointF)), this, SLOT(onLongPress(QPointF)));
    }
}

void MsgAdvancedSettingsForm::activateMMSCreationMode(HbWidget* widget)
{
    HbComboBox* combo = qobject_cast<HbComboBox *> (widget);

    if (combo) {
        //to avoid multiple connects.
        disconnect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeMMSCreationMode(int)));

        //map the mms access points selected
        connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeMMSCreationMode(int)));
    }
}

void MsgAdvancedSettingsForm::changeMMSCreationMode(int index)
{
    mSettingEngine->setMMSCreationmode(MsgSettingEngine::MmsCreationMode(index));
}

void MsgAdvancedSettingsForm::activateMMSRetrievalMode(HbWidget* widget)
{
    HbComboBox* combo = qobject_cast<HbComboBox *> (widget);

    if (combo) {
        //to avoid multiple connects.
        disconnect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeMMSRetrievalMode(int)));

        //map the mms access points selected
        connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeMMSRetrievalMode(int)));
    }
}

void MsgAdvancedSettingsForm::changeMMSRetrievalMode(int index)
{
    mSettingEngine->setMMSRetrieval(MsgSettingEngine::MmsRetrieval(index));
}

void MsgAdvancedSettingsForm::activateMMSAnonymousMode(HbWidget* widget)
{
    HbPushButton* toggleItem = qobject_cast<HbPushButton *> (widget);

    if (toggleItem) {
        //TODO Issue in DataForm Calling the itemActivated twice
        disconnect(toggleItem, SIGNAL(clicked()), this, SLOT(allowAnonymousMMS()));

        connect(toggleItem, SIGNAL(clicked()), this, SLOT(allowAnonymousMMS()));
    }
}

void MsgAdvancedSettingsForm::allowAnonymousMMS()
{
    HbPushButton *btn = qobject_cast<HbPushButton *> (sender());

    if (btn) {
        QString btnText = btn->text();

        //check if the button pressed was Yes or NO
        if (LOC_MMS_YES == btnText) {
            mSettingEngine->setAnonymousMessages(true);
        }
        else {
            mSettingEngine->setAnonymousMessages(false);
        }
    }
}

void MsgAdvancedSettingsForm::activateMMSAdverts(HbWidget* widget)
{
    HbPushButton* toggleItem = qobject_cast<HbPushButton *> (widget);

    if (toggleItem) {
        //TODO Issue in DataForm Calling the itemActivated twice
        disconnect(toggleItem, SIGNAL(clicked()), this, SLOT(allowMMSAdverts()));

        connect(toggleItem, SIGNAL(clicked()), this, SLOT(allowMMSAdverts()));
    }
}

void MsgAdvancedSettingsForm::allowMMSAdverts()
{
    HbPushButton *btn = qobject_cast<HbPushButton *> (sender());

    if (btn) {
        QString btnText = btn->text();

        //check if the button pressed was Yes or NO
        if (LOC_MMS_NO == btnText) {
            mSettingEngine->setReceiveMMSAdverts(true);
        }
        else {
            mSettingEngine->setReceiveMMSAdverts(false);
        }
    }
}

void MsgAdvancedSettingsForm::activateMMSAccesspoint(HbWidget* widget)
{
    HbComboBox* accessPointCombo = qobject_cast<HbComboBox *> (widget);

    //to avoid multiple connects.
    disconnect(accessPointCombo, SIGNAL(currentIndexChanged(int)), this,
        SLOT(changeAccessPoint(int)));

    //map the mms access points selected
    connect(accessPointCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeAccessPoint(int)));
}

void MsgAdvancedSettingsForm::onPressedCustomButton()
{
    HbPushButton *btn = qobject_cast<HbPushButton *> (sender());

    if (btn) {
        QString btnText = btn->text();
        if (btnText == mSmsMCSettingsGroup.at(1)) {
            // emit the signal to open add view 
            emit newSMSCCenterClicked(-1);
        }
        else {
            int index = mSmsServiceList.indexOf(btnText);
            emit newSMSCCenterClicked(index);
        }
    }
}

void MsgAdvancedSettingsForm::changeAccessPoint(int index)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "Enter MsgAdvancedSettingsForm::changeAccessPoint index=" << index << "\n";
#endif

    // save to the mms settings
    mSettingEngine->setMMSAccesspoint(index);

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit MsgAdvancedSettingsForm::changeAccessPoint";
#endif

}

void MsgAdvancedSettingsForm::refresh()
{
    int childCount = mSmsMCGroup->childCount();
    //remove items after mSmsCenterComboBox till new center button.
    mSmsMCGroup->removeChildren(1, childCount - 2);
    updateSmsCenterModel();
}

void MsgAdvancedSettingsForm::commitEmailChanges()
{
    if (mEditEmailGateway == NULL || mEditEmailServiceNumber == NULL) {
        return;
    }

    QString emailGateway = mEditEmailGateway->text();
    QString emailServiceCenter = mEditEmailServiceNumber->text();

#ifdef _DEBUG_TRACES_
    qDebug() << "Enter MsgAdvancedSettingsForm::commitEmailChanges " << emailGateway << " "
        << emailServiceCenter;
#endif

    mSettingEngine->setEmailGateway(emailGateway);

    //set the smsc center number
    mSettingEngine->setEmailSeriveCenterNumber(emailServiceCenter);

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit MsgAdvancedSettingsForm::commitEmailChanges\n";
#endif

}

void MsgAdvancedSettingsForm::updateSmsCenterModel()
{
    mSmsServiceList.clear();
    mSettingEngine->allSMSMessageCenter(mSmsServiceList, mDefaultServiceCenter);

    HbDataFormModelItem* smsCenterCombo = mSmsMCGroup->childAt(0);
    QAbstractItemModel* mod = const_cast<QAbstractItemModel*> (this->model());
    HbDataFormModel* advancedSettingsmodel = static_cast<HbDataFormModel*> (mod);
    int msgCenterComboIndex = advancedSettingsmodel->indexFromItem(smsCenterCombo).row();

    //clear model before refilling.
    mSmsServiceCenterModel->clear();
    fillServiceCenterModel();

    //CUSTOM SETTING ITEM HbButton
    HbDataFormModelItem::DataItemType customButtonGeneral =
        static_cast<HbDataFormModelItem::DataItemType> (HbDataFormModelItem::CustomItemBase + 1);

    int itemCount = mSmsServiceList.count();
    for (int i = 0; i < itemCount; ++i) {
        //add custom buttons.
        HbDataFormModelItem *messageCenters = new HbDataFormModelItem(customButtonGeneral,
            QString(), mSmsMCGroup);

        messageCenters->setData(HbDataFormModelItem::KeyRole, mSmsServiceList.at(i));

        advancedSettingsmodel->insertDataFormItem(msgCenterComboIndex + 1 + i, messageCenters,
            mSmsMCGroup);
    }
}

void MsgAdvancedSettingsForm::createServiceCenterModel()
{
    mSmsServiceCenterModel = new QStandardItemModel();

    //to know when is model destroyed.
    connect(mSmsServiceCenterModel, SIGNAL(destroyed()), this, SLOT(createServiceCenterModel()));

    fillServiceCenterModel();
}

void MsgAdvancedSettingsForm::fillServiceCenterModel()
{
    QStringList serviceCenters;
    mSettingEngine->allSMSMessageCenter(serviceCenters, mDefaultServiceCenter);

    int itemCount = serviceCenters.count();
    for (int i = 0; i < itemCount; ++i) {
        QStandardItem *item = new QStandardItem;
        item->setData(serviceCenters.at(i), Qt::DisplayRole);
        mSmsServiceCenterModel->appendRow(item);
    }
}

void MsgAdvancedSettingsForm::activateEmailGateway(HbWidget* widget, HbLineEdit* &edit)
{
    // editing finished SIGNAL is still not coming in week48MCL
    HbLineEdit* editWidget = qobject_cast<HbLineEdit *> (widget);

    if (editWidget) {
        edit = editWidget;
        //connect(edit, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
    }
}

void MsgAdvancedSettingsForm::activateEmailServiceCenterNo(HbWidget* widget, HbLineEdit* &edit)
{
    // editing finished SIGNAL is still not coming in week48MCL
    HbLineEdit* editWidget = qobject_cast<HbLineEdit *> (widget);

    if (editWidget) {
        edit = editWidget;
        HbEditorInterface editorInterface(edit);
        editorInterface.setUpAsPhoneNumberEditor();
        //connect(edit, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
    }
}

void MsgAdvancedSettingsForm::onLongPress(QPointF point)
{
    Q_UNUSED(point);
}
//eof
