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

//Localized constants
#define LOC_NEW_SMS_CENTRE hbTrId("txt_messaging_title_new_sms_message_centre")
#define LOC_EDIT_SMS_CENTRE hbTrId("txt_messaging_title_edit_sms_message_centre")
#define LOC_SMS_CENTRE_NAME hbTrId("txt_messaging_setlabel_message_centre_name")
#define LOC_SMS_CENTRE_NUMBER hbTrId("txt_messaging_setlabel_message_centre_number")


MsgSMSCenterSettingsForm::MsgSMSCenterSettingsForm(int view, QGraphicsItem *parent) :
    HbDataForm(parent), mEdit1(NULL), mEdit2(NULL), mView(view)
{
    QString heading;
    if (mView == -1) {
        heading = LOC_NEW_SMS_CENTRE;
    }
    else {
        heading = LOC_EDIT_SMS_CENTRE;
    }

    this->setHeading(heading);

    mSettingEngine = new MsgSettingEngine();

    initSettingModel();

    connect(this, SIGNAL(activated(QModelIndex)), this, SLOT(onItemActivated(QModelIndex)));
}

MsgSMSCenterSettingsForm::~MsgSMSCenterSettingsForm()
{
    delete mSettingEngine;
}

void MsgSMSCenterSettingsForm::initSettingModel()
{
    HbDataFormModel *settingsmodel = new HbDataFormModel();

    QString centerName = QString();
    QString centerNumber = QString();

    if (mView != -1) {
        mSettingEngine->smsCenterNameAndNumber(mView, centerName, centerNumber);
    }

    HbDataFormModelItem *messageCenterName = new HbDataFormModelItem(HbDataFormModelItem::TextItem,
            LOC_SMS_CENTRE_NAME, 0);

    messageCenterName->setContentWidgetData("text", centerName);

    settingsmodel->appendDataFormItem(messageCenterName, settingsmodel->invisibleRootItem());

    HbDataFormModelItem *messageCenterNumber = new HbDataFormModelItem(
        HbDataFormModelItem::TextItem, LOC_SMS_CENTRE_NUMBER, 0);

    messageCenterNumber->setContentWidgetData("text", centerNumber);

    settingsmodel->appendDataFormItem(messageCenterNumber, settingsmodel->invisibleRootItem());

    this->setModel(settingsmodel);
}

void MsgSMSCenterSettingsForm::onItemActivated(const QModelIndex &index)
{
    HbDataFormModelItem *itemData = static_cast<HbDataFormModelItem *> (index.internalPointer());
    // to deal with orbit change temparialy
    if (itemData->type() > HbDataFormModelItem::GroupPageItem) {
        HbDataFormModelItem *pageItemData = const_cast<HbDataFormModelItem *> (itemData->parent());
        //get the widget of setting item
        HbWidget* widget = this->dataFormViewItem(index)->dataItemContentWidget();
        if (pageItemData) {
            switch (pageItemData->indexOf(itemData)) {
            case 0: //name
                activateTextItem(widget, mEdit1);
                break;
            case 1: //number
                activateTextItem(widget, mEdit2);
                break;

            default:
                break;
            }
        }
    }
}

void MsgSMSCenterSettingsForm::activateTextItem(HbWidget* widget, HbLineEdit* &edit)
{
    HbLineEdit* editWidget = qobject_cast<HbLineEdit *> (widget);

    if (editWidget) {
        edit = editWidget;
        // If Service center number
        if (edit == mEdit2) {
            HbEditorInterface editorInterface(edit);
            editorInterface.setUpAsPhoneNumberEditor();
        }
        connect(edit, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
    }
}

void MsgSMSCenterSettingsForm::onEditingFinished()
{
    HbLineEdit *edit1 = qobject_cast<HbLineEdit *> (sender());

    if (edit1) {
        if (edit1 == mEdit1) {
            mCenterName = edit1->text();
        }
        else if (edit1 == mEdit2) {
            mCenterNumber = edit1->text();
        }
    }
}

void MsgSMSCenterSettingsForm::commitChanges()
{
    //commit changes
    mCenterName = mEdit1->text();
    mCenterNumber = mEdit2->text();

    if (mCenterName.isEmpty() || mCenterNumber.isEmpty()) {
        //if mCenterName or mCenterNumber is empty do not commit.
        return;
    }

    if (mView == -1) {
        mSettingEngine->addSmsMessageCenter(mCenterName, mCenterNumber);
    }
    else {
        mSettingEngine->editSmsMessageCenter(mCenterName, mCenterNumber, mView);
    }
}
