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
 * Description: This widget holds the message header.
 *
 */

#include "univiewerheadercontainer.h"

// SYSTEM INCLUDES
#include <QGraphicsLinearLayout>
#include <HbGroupBox>
#include <HbFrameItem>

// USER INCLUDES
#include "univieweraddresscontainer.h"
#include "univiewerattachmentcontainer.h"
#include "univiewerdetailswidget.h"
#include "univiewerfeeder.h"

#include "nativemessageconsts.h"

// LOCALIZATION
#define LOC_SEND_FAILED hbTrId("txt_messaging_formlabel_sending_failed")

// LOCAL CONSTANTS
const QString ADDR_LIST_SEPARATOR(", ");
const QString BG_FRAME_GRAPHICS("qtg_fr_form_heading");

//---------------------------------------------------------------
// UniViewerHeaderContainer :: UniViewerHeaderContainer
// @see header file
//---------------------------------------------------------------
UniViewerHeaderContainer::UniViewerHeaderContainer(UniViewerFeeder* feeder, QGraphicsItem *parent) :
    HbWidget(parent), mViewFeeder(feeder), mViewerDetails(0), mHeaderGroupBox(0),
        mAddressContainer(0), mAttachmentContainer(0)
{
    HbFrameItem *bgItem = new HbFrameItem(BG_FRAME_GRAPHICS, HbFrameDrawer::NinePieces, this);
    this->setBackgroundItem(bgItem);

    QGraphicsLinearLayout *mainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Address Group box
    mHeaderGroupBox = new HbGroupBox(this);

    // Address container
    mAddressContainer = new UniViewerAddressContainer(mHeaderGroupBox);

    mHeaderGroupBox->setContentWidget(mAddressContainer);

    // Viewer Details widget
    mViewerDetails = new UniViewerDetailsWidget(this);

    // Attachment Container
    mAttachmentContainer = new UniViewerAttachmentContainer(this);

    //Add address group box and insert into layout
    mainLayout->addItem(mHeaderGroupBox);
    mainLayout->addItem(mViewerDetails);
    mainLayout->addItem(mAttachmentContainer);

    this->setLayout(mainLayout);
}

//---------------------------------------------------------------
// UniViewerHeaderContainer :: ~UniViewerHeaderContainer
// @see header file
//---------------------------------------------------------------
UniViewerHeaderContainer::~UniViewerHeaderContainer()
{

}

//---------------------------------------------------------------
// UniViewerHeaderContainer :: populateContent
// @see header file
//---------------------------------------------------------------
void UniViewerHeaderContainer::populateContent()
{
    setAddrGroupBoxHeading();

    // Address container
    populateAddressContainer();

    // Time stamp
    mViewerDetails->setTimeStamp(mViewFeeder->timeStamp(), mViewFeeder->sendingState());

    // Priority
    mViewerDetails->setPriorityIcon(mViewFeeder->priority());

    // Subject
    populateSubject();

    // Attachment list
    populateAttachments();

    // Expand address group box for outgoing messages.
    if (mViewFeeder->isIncoming()) {
        mHeaderGroupBox->setCollapsed(true);
    }
    else {
        mHeaderGroupBox->setCollapsed(false);
    }
}

//---------------------------------------------------------------
// UniViewerHeaderContainer :: clearContent
// @see header file
//---------------------------------------------------------------
void UniViewerHeaderContainer::clearContent()
{
    if (mViewerDetails) {
        mViewerDetails->clearContent();
    }
    if (mAddressContainer) {
        mAddressContainer->clearContent();
    }
    if (mAttachmentContainer) {
        mAttachmentContainer->clearContent();
    }
}

//---------------------------------------------------------------
// UniViewerHeaderContainer :: populateSubject
// @see header file
//---------------------------------------------------------------
void UniViewerHeaderContainer::populateSubject()
{
    if (KSenduiMtmSmsUidValue == mViewFeeder->msgType()) {
        if ((ConvergedMessage::Failed == mViewFeeder->sendingState()) && !mViewFeeder->isIncoming()) {
            mViewerDetails->setSubject(LOC_SEND_FAILED);
        }
    }
    else if (KSenduiMtmMmsUidValue == mViewFeeder->msgType()) {
        QString subject;
        if (ConvergedMessage::Failed == mViewFeeder->sendingState()) {
            subject.append(LOC_SEND_FAILED);
            subject.append(QChar::LineSeparator);
        }
        subject.append(mViewFeeder->subject());
        if (!subject.isEmpty()) {
            mViewerDetails->setSubject(subject);
        }
    }
}

//---------------------------------------------------------------
// UniViewerHeaderContainer :: populateAttachments
// @see header file
//---------------------------------------------------------------
void UniViewerHeaderContainer::populateAttachments()
{
    UniMessageInfoList attachList = mViewFeeder->attachmentsList();
    for (int a = 0; a < attachList.count(); ++a) {
        UniMessageInfo* info = attachList.at(a);
        mAttachmentContainer->addAttachmentWidget(info->mimetype(), info->path());
        delete info;
    }
}

//---------------------------------------------------------------
// UniViewerHeaderContainer :: populateAttachments
// @see header file
//---------------------------------------------------------------
void UniViewerHeaderContainer::populateAddressContainer()
{
    QString from, alias;
    mViewFeeder->fromAddressAndAlias(from, alias);
    ConvergedMessageAddressList toList = mViewFeeder->toAddressList();

    // From field is added ONLY for incoming messages.
    if (mViewFeeder->isIncoming()) {
        mAddressContainer->setFromField(from, alias);
    }
    // TO field is added ONLY for outgoing messages.
    else if (!toList.isEmpty()) {
        mAddressContainer->setToField(toList);
    }

    // CC field is added ONLY for MMS messages.
    if (mViewFeeder->msgType() == KSenduiMtmMmsUidValue) {
        ConvergedMessageAddressList ccList = mViewFeeder->ccAddressList();
        if (!ccList.isEmpty()) {
            mAddressContainer->setCcField(ccList);
        }
    }

    mAddressContainer->insertDivider();
}
//---------------------------------------------------------------
// UniViewerHeaderContainer :: setAddrGroupBoxHeading
// @see header file
//---------------------------------------------------------------
void UniViewerHeaderContainer::setAddrGroupBoxHeading()
{
    if (mViewFeeder->isIncoming()) {
        QString from, alias;
        mViewFeeder->fromAddressAndAlias(from, alias);
        (alias.isEmpty()) ? mHeaderGroupBox->setHeading(from) : mHeaderGroupBox->setHeading(alias);
    }
    else {
        QString toAddrList = createAddressList(mViewFeeder->toAddressList());
        QString ccAddrList = createAddressList(mViewFeeder->ccAddressList());

        if (!ccAddrList.isEmpty()) {
            toAddrList.append(ADDR_LIST_SEPARATOR);
            toAddrList.append(ccAddrList);
        }
        mHeaderGroupBox->setHeading(toAddrList);
    }
}

//---------------------------------------------------------------
// UniViewerHeaderContainer :: createAddressList
// @see header file
//---------------------------------------------------------------
QString UniViewerHeaderContainer::createAddressList(const ConvergedMessageAddressList &addressList)
{
    QString address;
    for (int i = 0; i < addressList.count(); ++i) {
        address.append((addressList[i]->alias().isEmpty()) ? addressList[i]->address()
            : addressList[i]->alias());
        address.append(ADDR_LIST_SEPARATOR);
    }
    address.chop(ADDR_LIST_SEPARATOR.size());
    return address;
}
