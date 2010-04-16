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

#include "univiewerattachmentcontainer.h"

// SYSTEM INCLUDES
#include <QGraphicsLinearLayout>
#include <QFileInfo>

// USER INCLUDES
#include "univiewermediawidget.h"
#include "debugtraces.h"

// LOCAL CONSTANTS
const QString ATTACHMENT_FRAME("qtg_fr_groupbox");
const QString ATTACHMENT_ICON("qtg_small_attachment.svg");

const int KILOBYTE = 1024;

//---------------------------------------------------------------
// UniViewerAttachmentContainer :: UniViewerAttachmentContainer
// @see header file
//---------------------------------------------------------------
UniViewerAttachmentContainer::UniViewerAttachmentContainer(QGraphicsItem *parent) :
    HbWidget(parent), mTotalAttachment(0), mMainLayout(NULL)
{
    QDEBUG_WRITE("AttachmentContainer Initialized..");
    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mMainLayout->setContentsMargins(0, 0, 0, 0);
    mMainLayout->setSpacing(0);
    setLayout(mMainLayout);
}

//---------------------------------------------------------------
// UniViewerAttachmentContainer :: ~UniViewerAttachmentContainer
// @see header file
//---------------------------------------------------------------
UniViewerAttachmentContainer::~UniViewerAttachmentContainer()
{

}

//---------------------------------------------------------------
// UniViewerAttachmentContainer :: addAttachmentWidget
// @see header file
//---------------------------------------------------------------
void UniViewerAttachmentContainer::addAttachmentWidget(QString type, QString fileName)
{
    Q_UNUSED(type)

    UniViewerMediaWidget *attachmentWidget = new UniViewerMediaWidget(this);
    mUniViewerMediaWidgetList.append(attachmentWidget);
    QFileInfo fileInfo(fileName);

    int fileSize = fileInfo.size();
    QString sizeString('B');
    if (fileSize > KILOBYTE) {
        // Convert to Kilobytes.
        fileSize /= KILOBYTE;
        sizeString = "Kb";
    }

    QString fileDetails = "(" + QString::number(fileSize) + sizeString + ")";

    attachmentWidget->populate(ATTACHMENT_ICON, fileInfo.fileName(), fileDetails, ATTACHMENT_FRAME);

    mMainLayout->addItem(attachmentWidget);

    mTotalAttachment++;
}

//---------------------------------------------------------------
// UniViewerAttachmentContainer :: clearContent
// @see header file
//---------------------------------------------------------------
void UniViewerAttachmentContainer::clearContent()
{
    for (int i = 0; i < mUniViewerMediaWidgetList.count(); ++i) {
        --mTotalAttachment;
        mMainLayout->removeItem(mUniViewerMediaWidgetList[i]);
        mUniViewerMediaWidgetList[i]->setParent(NULL);
        delete mUniViewerMediaWidgetList[i];
        mUniViewerMediaWidgetList[i] = NULL;
    }
    mUniViewerMediaWidgetList.clear();
    resize(rect().width(), -1);
}

//eof
