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
 * Description: This widget holds the message details and silde contents.
 *
 */

#include "univiewslidewidget.h"

// SYSTEM INCLUDES
#include <QGraphicsLinearLayout>
#include <HbGroupBox>

// USER INCLUDES
#include "univieweraddresscontainer.h"
#include "univiewerattachmentcontainer.h"
#include "univiewerdetailswidget.h"
#include "univiewerbodywidget.h"
#include "convergedmessage.h"
#include "univiewerfeeder.h"

#include "nativemessageconsts.h"
//---------------------------------------------------------------
//UniViewSlideWidget :: UniViewSlideWidget
// @see header file
//---------------------------------------------------------------
UniViewSlideWidget::UniViewSlideWidget(UniViewerFeeder* feeder, int slideNumber,
    QGraphicsItem * parent) :
    HbWidget(parent), mInsideLayout(false), mSlideNumber(slideNumber), mViewerDetails(NULL),
        mViewFeeder(feeder), mHeaderGroupBox(NULL), mAddressContainer(NULL), mAttachmentContainer(
            NULL)

{

    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mBody = new UniViewerBodyWidget(this);
    
    connect(mBody,SIGNAL(sendMessage(const QString&)),
            this, SIGNAL(sendMessage(const QString&)));

    // If its a first slide create the headers and attach
    // to the main layout
    if (mSlideNumber == 0) {
        // Creation of address group box and adding address container
        mHeaderGroupBox = new HbGroupBox(this);
        mHeaderGroupBox->setCollapsed(true);
        mHeaderGroupBox->setCollapsable(true);

        mAddressContainer = new UniViewerAddressContainer(mHeaderGroupBox);

        mHeaderGroupBox->setContentWidget(mAddressContainer);

        mViewerDetails = new UniViewerDetailsWidget(this);

        mAttachmentContainer = new UniViewerAttachmentContainer(this);

        //Always connect to populate sms content
        connect(mViewFeeder, SIGNAL(msgBody(QString)), mBody, SLOT(setTextContent(QString)));

        //Add address group box and insert into layout
        mViewerDetails->sizePolicy().setVerticalPolicy(QSizePolicy::Fixed);
        mMainLayout->addItem(mHeaderGroupBox);
        mMainLayout->addItem(mViewerDetails);
        mMainLayout->addItem(mAttachmentContainer);
    }

    mMainLayout->addItem(mBody);

    mMainLayout->setSpacing(0);
    mMainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mMainLayout);
}

//---------------------------------------------------------------
//UniViewSlideWidget::~UniViewSlideWidget
// @see header file
//---------------------------------------------------------------
UniViewSlideWidget::~UniViewSlideWidget()
{

}

//---------------------------------------------------------------
//UniViewSlideWidget::clearContent
// @see header file
//---------------------------------------------------------------
void UniViewSlideWidget::clearContent()
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
    if (mBody) {
        mBody->clearContent();
    }
}

//---------------------------------------------------------------
//UniViewSlideWidget::populateContent
// @see header file
//---------------------------------------------------------------
void UniViewSlideWidget::populateContent()
{
    if (mSlideNumber == 0) {
        mViewFeeder->fetchDetails();
        populateFirstSlide();
    }

    if (mViewFeeder->msgType() == KSenduiMtmMmsUidValue
            && mViewFeeder->slideCount() > 0)
    {
        QString slideString;
        slideString = QString("%1 %2/%3").arg(tr("Slide")). arg(mSlideNumber + 1).arg(
            mViewFeeder->slideCount());
        mBody->setSlideContents(mViewFeeder->slideContent(mSlideNumber), slideString);

    }
}

//---------------------------------------------------------------
//UniViewSlideWidget::populateFirstSlide
// @see header file
//---------------------------------------------------------------
void UniViewSlideWidget::populateFirstSlide()
{
    //Populate the name for the groupbox
    ConvergedMessageAddressList toList = mViewFeeder->toAddressList();
    ConvergedMessageAddressList ccList = mViewFeeder->ccAddressList();

    if (!mViewFeeder->isIncoming())
    {
        QString address = QString();
        if (toList.count() > 0)
        {
            if (! (toList[0]->alias().isEmpty()))
            {
                address.append(toList[0]->alias());
            }
            else
            {
                address.append(toList[0]->address());
            }
        }
        for (int i = 1; i < toList.count(); ++i)
        {
            if (! (toList[i]->alias().isEmpty()))
            {
                address.append(", ");
                address.append(toList[i]->alias());
            }
            else
            {
                address.append(", ");
                address.append(toList[i]->address());
            }
        }

        if (ccList.count() > 0)
        {
            if (toList.count() > 0)
            {
                address.append(", ");
            }
            if (! (ccList[0]->alias().isEmpty()))
            {
                address.append(ccList[0]->alias());
            }
            else
            {
                address.append(ccList[0]->address());
            }
        }

        for (int j = 1; j < ccList.count(); ++j)
        {
            if (! (ccList[j]->alias().isEmpty()))
            {
                address.append(", ");
                address.append(ccList[j]->alias());

            }
            else
            {
                address.append(", ");
                address.append(ccList[j]->address());

            }
        }
        mHeaderGroupBox->setHeading(address);
        mHeaderGroupBox->setCollapsed(false);
    }
    else
    {
        QString from = QString();
        QString alias = QString();
        mViewFeeder->fromAddressAndAlias(from, alias);
        if (alias.isEmpty())
        {
            mHeaderGroupBox->setHeading(from);
        }
        else
        {
            mHeaderGroupBox->setHeading(alias);
        }

        mHeaderGroupBox->setCollapsed(true);
    }

    // Set the From fields details 
    if (mViewFeeder->isIncoming())
    {
        QString from = QString();
        QString alias = QString();
        mViewFeeder->fromAddressAndAlias(from, alias);
        mAddressContainer->setFromField(from, alias);
    }

    // Set the To/Cc fields details 
    if (mViewFeeder->msgType() == KSenduiMtmSmsUidValue)
    {
        if (!mViewFeeder->isIncoming())
        {
            mAddressContainer->setToField(toList);
        }
    }
    else if (mViewFeeder->msgType() == KSenduiMtmMmsUidValue)
    {
        mViewerDetails->setSubject(mViewFeeder->subject());
        mViewerDetails->setPriorityIcon(mViewFeeder->priority());

        if (!toList.isEmpty())
        {
            mAddressContainer->setToField(toList);
        }
        if (!ccList.isEmpty())
        {
            mAddressContainer->setCcField(ccList);
        }

        UniMessageInfoList attachList = mViewFeeder->attachmentsList();
        int count = attachList.count();

        for (int a = 0; a < count; ++a)
        {
            UniMessageInfo* info = attachList.at(a);
            QString type = info->mimetype();

            mAttachmentContainer->addAttachmentWidget(type, info->path());
            delete info;
        }
    }
    // Time stamp
 
    mViewerDetails->setTimeStamp(mViewFeeder->timeStamp());
}

//---------------------------------------------------------------
//UniViewSlideWidget::insideLayout
// @see header file
//---------------------------------------------------------------
bool UniViewSlideWidget::insideLayout()
{
    return mInsideLayout;
}

//---------------------------------------------------------------
//UniViewSlideWidget::setInsideLayout
// @see header file
//---------------------------------------------------------------
void UniViewSlideWidget::setInsideLayout(bool inside)
{
    mInsideLayout = inside;
}

// EOF
