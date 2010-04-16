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
 * Description: This widget displays subject,timestamp & priority info
 *
 */

#include "univiewerdetailswidget.h"

// SYSTEM INCLUDES
#include <HbTextItem>
#include <HbIconItem>
#include <QDateTime>

// USER INCLUDES
#include "convergedmessage.h"
#include "debugtraces.h"

// LOCAL CONSTANTS
const QString DATE_TIME_FORMAT("dd/MM/yy hh:mm ap"); //Date format.

const QString MSG_HIGH_PRIORITY_ICON("qtg_small_priority_high.svg");
const QString MSG_LOW_PRIORITY_ICON("qtg_small_priority_low.svg");

//---------------------------------------------------------------
// UniViewerDetailsWidget::UniViewerDetailsWidget
// @see header file
//---------------------------------------------------------------
UniViewerDetailsWidget::UniViewerDetailsWidget(QGraphicsItem *parent) :
    HbWidget(parent), mSubjectLabel(NULL),
    mPriorityIcon(NULL)
{
    // This is the only permanent item & will not be removed
    mTime = new HbTextItem(this);
    HbStyle::setItemName(mTime, "timeLabel");
}

//---------------------------------------------------------------
// UniViewerDetailsWidget::~UniViewerDetailsWidget
// @see header file
//---------------------------------------------------------------
UniViewerDetailsWidget::~UniViewerDetailsWidget()
{
}

//---------------------------------------------------------------
//UniViewerDetailsWidget :: setSubject
// @see header file
//---------------------------------------------------------------
void UniViewerDetailsWidget::setSubject(const QString &subject)
{
    if (!mSubjectLabel)
    {
        mSubjectLabel = new HbTextItem(this);
        HbStyle::setItemName(mSubjectLabel, "subjectLabel");
        mSubjectLabel->setTextWrapping(Hb::TextWrapAnywhere);
    }
    mSubjectLabel->setText(subject);
    this->repolish();
}

//---------------------------------------------------------------
//UniViewerDetailsWidget :: setTimeStamp
// @see header file
//---------------------------------------------------------------
void UniViewerDetailsWidget::setTimeStamp(const QDateTime &aTimeStamp)
{
    mTime->setText(aTimeStamp.toString(DATE_TIME_FORMAT));
}

//---------------------------------------------------------------
// UniViewerDetailsWidget::setPriorityIcon
// @see header file
//---------------------------------------------------------------
void UniViewerDetailsWidget::setPriorityIcon(int priority)
{
    if (priority)
    {
        if (!mPriorityIcon)
        {
            mPriorityIcon = new HbIconItem(this);
            HbStyle::setItemName(mPriorityIcon, "priorityIcon");
        }
        if (ConvergedMessage::Low == priority)
        {
            mPriorityIcon->setIcon(HbIcon(MSG_LOW_PRIORITY_ICON));
        }
        else if (ConvergedMessage::High == priority)
        {
            mPriorityIcon->setIcon(HbIcon(MSG_HIGH_PRIORITY_ICON));
        }
        this->repolish();
    }
}

//---------------------------------------------------------------
//UniViewerDetailsWidget :: clearContent
// @see header file
//---------------------------------------------------------------
void UniViewerDetailsWidget::clearContent()
{
    //Delete the temporary items(subject & priority) &
    //content on the permanent item

    if (mSubjectLabel)
    {
        mSubjectLabel->setParent(NULL);
        delete mSubjectLabel;
        mSubjectLabel = NULL;
    }

    if (mPriorityIcon)
    {
        mPriorityIcon->setParent(NULL);
        delete mPriorityIcon;
        mPriorityIcon = NULL;
    }

    mTime->setText(QString());
}

// EOF
