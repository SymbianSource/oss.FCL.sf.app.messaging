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
 * Description:
 *
 */

#include "univieweraddresscontainer.h"

// SYSTEM INCLUDES
#include <QString>
#include <QGraphicsLinearLayout>

// USER INCLUDES
#include "univieweraddresswidget.h"

// LOCAL CONSTANTS
#define LOC_FROM hbTrId("txt_messaging_formlabel_from    ")
#define LOC_TO hbTrId("txt_messaging_viewer_formlabel_to        ")
#define LOC_CC hbTrId("txt_messaging_viewer_formlabel_cc        ")

//---------------------------------------------------------------
// UniViewerAddressContainer :: UniViewerAddressContainer
// @see header file
//---------------------------------------------------------------
UniViewerAddressContainer::UniViewerAddressContainer(QGraphicsItem *parent) :
    HbWidget(parent), mMainLayout(NULL),
    mFromWidget(NULL),
    mToWidget(NULL),
    mCcWidget(NULL)
{
    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mMainLayout->setContentsMargins(0, 0, 0, 0);
    mMainLayout->setSpacing(0);
    setLayout(mMainLayout);
}

//---------------------------------------------------------------
// UniViewerAddressContainer :: ~UniViewerAddressContainer
// @see header file
//---------------------------------------------------------------
UniViewerAddressContainer::~UniViewerAddressContainer()
{

}

//---------------------------------------------------------------
// UniViewerAddressContainer :: setFromField
// @see header file
//---------------------------------------------------------------
void UniViewerAddressContainer::setFromField(const QString& fromRecipient,
                                             const QString& alias)
{
    if (!mFromWidget)
    {
        mFromWidget = new UniViewerAddressWidget();
    }

    mMainLayout->addItem(mFromWidget);
    mFromWidget->populate(LOC_FROM, fromRecipient, alias);
}

//---------------------------------------------------------------
// UniViewerAddressContainer :: setToField
// @see header file
//---------------------------------------------------------------
void UniViewerAddressContainer::setToField(
    ConvergedMessageAddressList toRecipients)
{
    if (!mToWidget)
    {
        mToWidget = new UniViewerAddressWidget();
    }

    mMainLayout->addItem(mToWidget);
    mToWidget->populate(LOC_TO, toRecipients);
}

//---------------------------------------------------------------
//UniViewerAddressContainer :: setCcField
// @see header file
//---------------------------------------------------------------
void UniViewerAddressContainer::setCcField(
    ConvergedMessageAddressList ccRecipients)
{
    if (!mCcWidget)
    {
        mCcWidget = new UniViewerAddressWidget();
    }

    mMainLayout->addItem(mCcWidget);
    mCcWidget->populate(LOC_CC, ccRecipients);
}

//---------------------------------------------------------------
// UniViewerAddressContainer :: clearContent
// @see header file
//---------------------------------------------------------------
void UniViewerAddressContainer::clearContent()
{
    if (mFromWidget)
    {
        mMainLayout->removeItem(mFromWidget);
        mFromWidget->setParent(NULL);
        delete mFromWidget;
        mFromWidget = NULL;
    }
    if (mToWidget)
    {
        mMainLayout->removeItem(mToWidget);
        mToWidget->setParent(NULL);
        delete mToWidget;
        mToWidget = NULL;
    }
    if (mCcWidget)
    {
        mMainLayout->removeItem(mCcWidget);
        mCcWidget->setParent(NULL);
        delete mCcWidget;
        mCcWidget = NULL;
    }

    resize(rect().width(), -1);
}

// EOF
