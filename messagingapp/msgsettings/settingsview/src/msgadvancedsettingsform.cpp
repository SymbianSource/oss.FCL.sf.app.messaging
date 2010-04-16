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
    HbDataForm(parent), mSmsServiceCenterModel(0), mEditEmailGateway(0),
            mEditEmailServiceNumber(0), mDefaultServiceCenter(0)
    {
    this->setHeading(LOC_ADVANCED_SETTINGS);

    mSettingEngine = new MsgSettingEngine();

    initGroupPageName();
    initSettingModel();
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
    this->addConnection(mmsCreationMode, SIGNAL(currentIndexChanged(int)),
            this, SLOT(changeMMSCreationMode(int)));

    HbDataFormModelItem *mmsRetrieval = new HbDataFormModelItem(
            HbDataFormModelItem::ComboBoxItem, mmsSettingsGroup.at(1), 0);

    QStringList mmsRetrievalList;
    mmsRetrievalList << LOC_ALWAYS_AUTOMATIC 
                     << LOC_AUTO_HOME_NETWORK 
                     << LOC_MANUAL
                     << LOC_OFF
                     << LOC_ON;

    mmsRetrieval->setContentWidgetData("items", mmsRetrievalList);
    mmsRetrieval->setContentWidgetData("currentIndex", int(retrieval));
    this->addConnection(mmsRetrieval, SIGNAL(currentIndexChanged(int)), this,
            SLOT(changeMMSRetrievalMode(int)));

    HbDataFormModelItem *anonymousMMS = new HbDataFormModelItem(
        HbDataFormModelItem::ToggleValueItem, mmsSettingsGroup.at(2), 0);

    QStringList anonymousMMSList;
    anonymousMMSList << LOC_MMS_NO 
                     << LOC_MMS_YES;

    int defaultIndex = int(allowAnonymous);
    int alternateIndex = (defaultIndex + 1) % anonymousMMSList.count();

    anonymousMMS->setContentWidgetData("text", anonymousMMSList.at(
            defaultIndex));
    anonymousMMS->setContentWidgetData("additionalText", anonymousMMSList.at(
            alternateIndex));
    this->addConnection(anonymousMMS, SIGNAL(clicked()), this,
            SLOT(allowAnonymousMMS()));

    HbDataFormModelItem *mmsAdverts = new HbDataFormModelItem(HbDataFormModelItem::ToggleValueItem,
        mmsSettingsGroup.at(3), 0);

    QStringList mmsAdvertsList;
    mmsAdvertsList << LOC_MMS_NO 
                   << LOC_MMS_YES;

    defaultIndex = int(allowAdverts);
    alternateIndex = (defaultIndex + 1) % mmsAdvertsList.count();

    mmsAdverts->setContentWidgetData("text", mmsAdvertsList.at(defaultIndex));
    mmsAdverts->setContentWidgetData("additionalText", mmsAdvertsList.at(
            alternateIndex));
    this->addConnection(mmsAdverts, SIGNAL(clicked()), this,
            SLOT(allowMMSAdverts()));

    HbDataFormModelItem *accessPoint = new HbDataFormModelItem(HbDataFormModelItem::ComboBoxItem,
        mmsSettingsGroup.at(4), 0);

    QStandardItemModel* comboModel = new QStandardItemModel();

    QStringList mmsList;
    int defaultAccessPointIndex = -1;

    mSettingEngine->allMMsAcessPoints(mmsList, defaultAccessPointIndex);
    int accessPoints = mmsList.count();
    for (int a = 0; a < accessPoints; ++a)
        {
        QStandardItem* item = new QStandardItem();
        item->setData(mmsList[a], Qt::DisplayRole);
        comboModel->appendRow(item);
        }
    accessPoint->setModel(comboModel);
    accessPoint->setContentWidgetData("items", mmsList);
    if (defaultAccessPointIndex >= 0)
        {
        accessPoint->setContentWidgetData("currentIndex",
                defaultAccessPointIndex);
        }
    QDEBUG_WRITE_FORMAT("Settings: adding accesspoints..", defaultAccessPointIndex);
    this->addConnection(accessPoint, SIGNAL(currentIndexChanged(int)), this,
            SLOT(changeAccessPoint(int)));

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

    mSettingEngine->allSMSMessageCenter(mSmsServiceList,
            mDefaultServiceCenter);
            
    QDEBUG_WRITE_FORMAT("MsgAdvancedSettingsForm::addSmsMCGroupItem Default", mDefaultServiceCenter);
    //create and fill service center model.
    createServiceCenterModel();

    smsMessageCenter->setModel(mSmsServiceCenterModel);
    smsMessageCenter->setContentWidgetData("items", mSmsServiceList);
    
    if (mDefaultServiceCenter > 0)
    {
     smsMessageCenter->setContentWidgetData("currentIndex",
             mDefaultServiceCenter);
    }
    
    this->addConnection(smsMessageCenter, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onSMSCurrentIndexChanged(int)));

    parent->appendChild(smsMessageCenter);

    //CUSTOM SETTING ITEM HbButton
    HbDataFormModelItem::DataItemType
            customButtonGeneral =
                    static_cast<HbDataFormModelItem::DataItemType> (HbDataFormModelItem::CustomItemBase
                            + 1);

    int totalServices = mSmsServiceList.count();
    for (int index = 0; index < totalServices; index++)
        {
        HbDataFormModelItem *newMessageCenters =
                advancedSettingsmodel->appendDataFormItem(
                        customButtonGeneral, QString(""), parent);

        newMessageCenters->setContentWidgetData("text", mSmsServiceList.at(
                index));
        this->addConnection(
                newMessageCenters, 
                SIGNAL(clicked()), 
                this,
                SLOT(onPressedCustomButton()));
        this->addConnection(
                newMessageCenters, 
                SIGNAL(longPress(QPointF)),
                this,
                SLOT(onLongPress(QPointF)));
        }

    HbDataFormModelItem *newMessageCenters = advancedSettingsmodel->appendDataFormItem(
            customButtonGeneral, QString(""), parent);

    newMessageCenters->setContentWidgetData("text", mSmsMCSettingsGroup.at(1));
    this->addConnection(newMessageCenters, SIGNAL(clicked()),
            this,SLOT(onPressedCustomButton()));
    this->addConnection(newMessageCenters, SIGNAL(longPress(QPointF)),
    this,SLOT(onLongPress(QPointF)));
    }

void MsgAdvancedSettingsForm::addSmsEmailGroupItem(
        HbDataFormModelItem* parent)
    {
    QString emailSmsNum = QString();
    QString emailGatewayTxt = QString();

    mSettingEngine->emailEmailGatewayAndServiceCenterNumber(emailGatewayTxt,
            emailSmsNum);

    // add all the sms email group item
    emailGateway = new HbDataFormModelItem(HbDataFormModelItem::TextItem,
            mSmsEmailSettingsGroup.at(0), 0);

    emailGateway->setContentWidgetData("text", emailGatewayTxt);
    connect(emailGateway->model(),
            SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(onDataChanged(const QModelIndex&, const QModelIndex&)));

    emailService = new HbDataFormModelItem(HbDataFormModelItem::TextItem,
            mSmsEmailSettingsGroup.at(1), 0);
    emailService->setContentWidgetData("text", emailSmsNum);
    connect(emailService->model(),
                SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
                this, SLOT(onDataChanged(const QModelIndex&, const QModelIndex&)));
    
    parent->appendChild(emailGateway);
    parent->appendChild(emailService);

    }

void MsgAdvancedSettingsForm::onSMSCurrentIndexChanged(int index)
    {
#ifdef _DEBUG_TRACES_
    qDebug()
            << "Enter MsgAdvancedSettingsForm::onSMSCurrentIndexChanged index="
            << index << "\n";
#endif

    //set into msg settings
    mSettingEngine->setSMSMessageCenter(index);

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit MsgAdvancedSettingsForm::onSMSCurrentIndexChanged \n";
#endif

    }

// mms settings slots defined here
void MsgAdvancedSettingsForm::changeMMSCreationMode(int index)
    {
    mSettingEngine->setMMSCreationmode(MsgSettingEngine::MmsCreationMode(
            index));
    }

void MsgAdvancedSettingsForm::changeMMSRetrievalMode(int index)
    {
    mSettingEngine->setMMSRetrieval(MsgSettingEngine::MmsRetrieval(index));
    }

void MsgAdvancedSettingsForm::allowAnonymousMMS()
    {
    HbPushButton *btn = qobject_cast<HbPushButton *> (sender());

    if (btn)
        {
        QString btnText = btn->text();

        //check if the button pressed was Yes or NO
        if (LOC_MMS_YES == btnText)
            {
            mSettingEngine->setAnonymousMessages(true);
            }
        else
            {
            mSettingEngine->setAnonymousMessages(false);
            }
        }
    }

void MsgAdvancedSettingsForm::allowMMSAdverts()
    {
    HbPushButton *btn = qobject_cast<HbPushButton *> (sender());

    if (btn)
        {
        QString btnText = btn->text();

        //check if the button pressed was Yes or NO
        if (LOC_MMS_NO == btnText)
            {
            mSettingEngine->setReceiveMMSAdverts(true);
            }
        else
            {
            mSettingEngine->setReceiveMMSAdverts(false);
            }
        }
    }

void MsgAdvancedSettingsForm::changeAccessPoint(int index)
    {
#ifdef _DEBUG_TRACES_
    qDebug() << "Enter MsgAdvancedSettingsForm::changeAccessPoint index="
            << index << "\n";
#endif

    // save to the mms settings
    mSettingEngine->setMMSAccesspoint(index);

#ifdef _DEBUG_TRACES_
    qDebug() << "Exit MsgAdvancedSettingsForm::changeAccessPoint";
#endif
    }

void MsgAdvancedSettingsForm::onPressedCustomButton()
    {
    HbPushButton *btn = qobject_cast<HbPushButton *> (sender());

    if (btn)
        {
        QString btnText = btn->text();
        if (btnText == mSmsMCSettingsGroup.at(1))
            {
            // emit the signal to open add view 
            emit newSMSCCenterClicked(-1);
            }
        else
            {
            int index = mSmsServiceList.indexOf(btnText);
            emit newSMSCCenterClicked(index);
            }
        }
    }

void MsgAdvancedSettingsForm::refresh()
    {
    int childCount = mSmsMCGroup->childCount();
    //remove items after mSmsCenterComboBox till new center button.
    if (childCount > 0){
        mSmsMCGroup->removeChildren(1, childCount - 2);
      }
    updateSmsCenterModel();
    }

void MsgAdvancedSettingsForm::commitEmailChanges()
    {
    if (mEditEmailGateway == NULL || mEditEmailServiceNumber == NULL)
        {
        return;
        }

    QString emailGateway = mEditEmailGateway->text();
    QString emailServiceCenter = mEditEmailServiceNumber->text();

#ifdef _DEBUG_TRACES_
    qDebug() << "Enter MsgAdvancedSettingsForm::commitEmailChanges "
            << emailGateway << " " << emailServiceCenter;
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
    mSettingEngine->allSMSMessageCenter(mSmsServiceList,
            mDefaultServiceCenter);

    HbDataFormModelItem* smsCenterCombo = mSmsMCGroup->childAt(0);
    QAbstractItemModel* mod = const_cast<QAbstractItemModel*> (this->model());
    HbDataFormModel* advancedSettingsmodel =
            static_cast<HbDataFormModel*> (mod);
    int msgCenterComboIndex = advancedSettingsmodel->indexFromItem(
            smsCenterCombo).row();

    //clear model before refilling.
    mSmsServiceCenterModel->clear();
    fillServiceCenterModel();

    //CUSTOM SETTING ITEM HbButton
    HbDataFormModelItem::DataItemType
            customButtonGeneral =
                    static_cast<HbDataFormModelItem::DataItemType> (HbDataFormModelItem::CustomItemBase
                            + 1);

    int itemCount = mSmsServiceList.count();
    for (int i = 0; i < itemCount; ++i)
        {
        //add custom buttons.
        HbDataFormModelItem *messageCenters = new HbDataFormModelItem(
                customButtonGeneral, QString(), mSmsMCGroup);

        messageCenters->setData(HbDataFormModelItem::KeyRole,
                mSmsServiceList.at(i));

        advancedSettingsmodel->insertDataFormItem(
                msgCenterComboIndex + 1 + i, messageCenters, mSmsMCGroup);
        }
    }

void MsgAdvancedSettingsForm::createServiceCenterModel()
    {
    mSmsServiceCenterModel = new QStandardItemModel();

    //to know when is model destroyed.
    connect(mSmsServiceCenterModel, SIGNAL(destroyed()), this,
            SLOT(createServiceCenterModel()));

    fillServiceCenterModel();
    }

void MsgAdvancedSettingsForm::fillServiceCenterModel()
    {
    QStringList serviceCenters;
    mSettingEngine->allSMSMessageCenter(serviceCenters, mDefaultServiceCenter);

    int itemCount = serviceCenters.count();
    
    QDEBUG_WRITE_FORMAT("MsgAdvancedSettingsForm::fillServiceCenterModel..", itemCount);
    for (int i = 0; i < itemCount; ++i)
        {
        QStandardItem *item = new QStandardItem;
        item->setData(serviceCenters.at(i), Qt::DisplayRole);
        QDEBUG_WRITE_FORMAT("MsgAdvancedSettingsForm::fillServiceCenterModel Setdata", serviceCenters.at(i));
        mSmsServiceCenterModel->appendRow(item);
        }
    }

void MsgAdvancedSettingsForm::activateEmailServiceCenterNo(HbWidget* widget,
        HbLineEdit* &edit)
    {
    // editing finished SIGNAL is still not coming in week48MCL
    HbLineEdit* editWidget = qobject_cast<HbLineEdit *> (widget);

    if (editWidget)
        {
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

void MsgAdvancedSettingsForm::onDataChanged(const QModelIndex& topLeft,
        const QModelIndex& bottomRight)
    {
    HbDataFormModelItem *itemData =
    static_cast<HbDataFormModel *> (model())->itemFromIndex(topLeft);

    if (itemData && itemData == emailGateway && !mEditEmailGateway)
        {
             mEditEmailGateway = 
                     static_cast<HbLineEdit *>(this->dataFormViewItem(topLeft)->dataItemContentWidget());
         }
    else if (itemData && itemData == emailService && !mEditEmailServiceNumber)
                {
                mEditEmailServiceNumber = 
                        static_cast<HbLineEdit *>(this->dataFormViewItem(topLeft)->dataItemContentWidget());
                HbEditorInterface editorInterface(mEditEmailServiceNumber);
                    editorInterface.setUpAsPhoneNumberEditor();

                        }

                    }
                //eof
