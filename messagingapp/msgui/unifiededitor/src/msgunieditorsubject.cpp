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

// INCLUDES
#include "debugtraces.h"
#include <HbTextItem>
#include <HbLineEdit>
#include <HbIconItem>

// USER INCLUDES
#include "msgunieditorsubject.h"
#include "UniEditorGenUtils.h"

// Localized Constants
#define LOC_SUBJECT hbTrId("txt_messaging_formlabel_subject")

//---------------------------------------------------------------
// MsgUnifiedEditorSubject::MsgUnifiedEditorSubject
// @see header file
//---------------------------------------------------------------
MsgUnifiedEditorSubject::MsgUnifiedEditorSubject( const QString& pluginPath, QGraphicsItem *parent ) :
HbWidget(parent),
mPluginPath(pluginPath),
mPriorityIcon(NULL),
mGenUtils(0)
{
#ifdef _DEBUG_TRACES_
				qDebug() << "MsgUnifiedEditorSubject calling HbStyle::registerPlugin";
#endif

        setPluginBaseId(style()->registerPlugin(mPluginPath));

        mSubjectLabel = new HbTextItem(LOC_SUBJECT,this);
        HbStyle::setItemName(mSubjectLabel,"subjectLabel");

        mSubjectEdit = new HbLineEdit(this);
        HbStyle::setItemName(mSubjectEdit,"subjectEdit");
        mSubjectEdit->setMinRows(1);
        mSubjectEdit->setMaxRows(10);
        
        mGenUtils = new UniEditorGenUtils();
        
        connect(mSubjectEdit, SIGNAL(textChanged(const QString&)),
                this, SLOT(onTextChanged(const QString&)));
}

//---------------------------------------------------------------
// MsgUnifiedEditorSubject::~MsgUnifiedEditorSubject
// @see header file
//---------------------------------------------------------------
MsgUnifiedEditorSubject::~MsgUnifiedEditorSubject()
{
    style()->unregisterPlugin(mPluginPath);
   
    if(mGenUtils)
    {
        delete mGenUtils;
        mGenUtils = NULL;
    }
}

void MsgUnifiedEditorSubject::setPriority(ConvergedMessage::Priority priority)
{
	mPriority = priority;
    if(mPriorityIcon)
    {// Transition from low/high => low/normal/high
        mPriorityIcon->setParent(NULL);
        delete mPriorityIcon;
        mPriorityIcon = NULL;
    }
    else
    {// Transition from normal
        if(mPriority == ConvergedMessage::Normal)
        {// to normal
            return;
        }
        else
        {// to high/low
            emit mmContentAdded(true);
        }
    }

    switch(priority)
    {
        case ConvergedMessage::High :
        {
        mPriorityIcon = new HbIconItem(":/qtg_small_priority_high.svg", this);
        HbStyle::setItemName(mPriorityIcon,"priorityIcon");
        }
        break;
        case ConvergedMessage::Low :
        {
        mPriorityIcon = new HbIconItem(":/qtg_small_priority_low.svg", this);
        HbStyle::setItemName(mPriorityIcon,"priorityIcon");
        }
        break;
        case ConvergedMessage::Normal :
        {
            emit mmContentAdded(false);
        }
        break;
        default:
        break;
    }

    this->repolish();
}

QString MsgUnifiedEditorSubject::text()
{
    return mSubjectEdit->text();
}

ConvergedMessage::Priority MsgUnifiedEditorSubject::priority()
{
	return mPriority;
}

void MsgUnifiedEditorSubject::onTextChanged(const QString& text)
{
    if(!text.isEmpty())
    {
        disconnect(mSubjectEdit, SIGNAL(textChanged(const QString&)),
                this, SLOT(onTextChanged(const QString&)));
        if(!subjectOkInSms())
        {
            emit mmContentAdded(true);
        }
        connect(mSubjectEdit, SIGNAL(textChanged(const QString&)),
                this, SLOT(onTextRemoved(const QString&)));
    }
}

void MsgUnifiedEditorSubject::onTextRemoved(const QString& text)
{
    if(text.isEmpty())
    {
        disconnect(mSubjectEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(onTextRemoved(const QString&)));
        if(!subjectOkInSms())
        {
            emit mmContentAdded(false);
        }
        connect(mSubjectEdit, SIGNAL(textChanged(const QString&)),
                this, SLOT(onTextChanged(const QString&)));
    }
}

bool MsgUnifiedEditorSubject::subjectOkInSms()
{
    bool result = false;
    int error;
    TRAP(error, result = mGenUtils->AcceptEmailAddressesL());
    return result;    
}

int MsgUnifiedEditorSubject::subjectSize()
{
    return mGenUtils->UTF8Size(mSubjectEdit->text());
}

void MsgUnifiedEditorSubject::setText(const QString& text)
{
    mSubjectEdit->setText(text);
}

//EOF
