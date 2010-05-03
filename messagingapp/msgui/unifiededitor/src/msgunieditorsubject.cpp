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
#include <HbIconItem>

// USER INCLUDES
#include "msgunieditorsubject.h"
#include "UniEditorGenUtils.h"
#include "msgunifiededitorlineedit.h"

// Localized Constants
#define LOC_SUBJECT hbTrId("txt_messaging_formlabel_subject")

//priority icon
const QString HIGH_PRIORITY("qtg_small_priority_high");
const QString LOW_PRIORITY("qtg_small_priority_low");

//---------------------------------------------------------------
// MsgUnifiedEditorSubject::MsgUnifiedEditorSubject
// @see header file
//---------------------------------------------------------------
MsgUnifiedEditorSubject::MsgUnifiedEditorSubject( const QString& pluginPath, QGraphicsItem *parent ) :
HbWidget(parent),
mPluginPath(pluginPath),
mPriorityIcon(NULL),
mPriority(ConvergedMessage::Normal),
mGenUtils(0)
{
#ifdef _DEBUG_TRACES_
				qDebug() << "MsgUnifiedEditorSubject calling HbStyle::registerPlugin";
#endif

        setPluginBaseId(style()->registerPlugin(mPluginPath));

        mSubjectEdit = new MsgUnifiedEditorLineEdit(LOC_SUBJECT,this);
        mSubjectEdit->setDefaultBehaviour(true);        
        HbStyle::setItemName(mSubjectEdit,"subjectEdit");
        mSubjectEdit->setMinRows(1);
        mSubjectEdit->setMaxRows(10);
        
        mGenUtils = new UniEditorGenUtils();
        
        connect(mSubjectEdit, SIGNAL(contentsChanged(const QString&)),
                this, SLOT(onContentsAdded(const QString&)));
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

    switch(priority)
    {
        case ConvergedMessage::High :
        {
        mPriorityIcon = new HbIconItem(HIGH_PRIORITY, this);
        HbStyle::setItemName(mPriorityIcon,"priorityIcon");
        }
        break;
        case ConvergedMessage::Low :
        {
        mPriorityIcon = new HbIconItem(LOW_PRIORITY, this);
        HbStyle::setItemName(mPriorityIcon,"priorityIcon");
        }
        break;
        default:
        break;
    }

    emit contentChanged();
    this->repolish();
}

QString MsgUnifiedEditorSubject::text()
{
    return mSubjectEdit->content();
}

ConvergedMessage::Priority MsgUnifiedEditorSubject::priority()
{
	return mPriority;
}

void MsgUnifiedEditorSubject::onContentsAdded(const QString& text)
{
    if(!text.isEmpty())
    {
        disconnect(mSubjectEdit, SIGNAL(contentsChanged(const QString&)),
                this, SLOT(onContentsAdded(const QString&)));
        if(!subjectOkInSms())
        {
            emit contentChanged();
        }
        connect(mSubjectEdit, SIGNAL(contentsChanged(const QString&)),
                this, SLOT(onContentsRemoved(const QString&)));
    }
}

void MsgUnifiedEditorSubject::onContentsRemoved(const QString& text)
{
    if(text.isEmpty())
    {
        disconnect(mSubjectEdit, SIGNAL(contentsChanged(const QString&)),
            this, SLOT(onContentsRemoved(const QString&)));
        if(!subjectOkInSms())
        {
            emit contentChanged();
        }
        connect(mSubjectEdit, SIGNAL(contentsChanged(const QString&)),
                this, SLOT(onContentsAdded(const QString&)));
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
    return mGenUtils->UTF8Size(mSubjectEdit->content());
}

void MsgUnifiedEditorSubject::setText(const QString& text)
{
    mSubjectEdit->setText(text);
}

//EOF
