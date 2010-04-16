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

// USER INCLUDES
#include "msgmonitor.h"
#include "msgunieditorbody.h"
#include "msgunieditorsubject.h"
#include "msgattachmentcontainer.h"
#include "UniEditorGenUtils.h"

// Constants

// Define static
int MsgMonitor::mBodySize;
int MsgMonitor::mContainerSize;
int MsgMonitor::mSubjectSize;
int MsgMonitor::mMaxMmsSize;

//---------------------------------------------------------------
// MsgMonitor::MsgMonitor
// @see header file
//---------------------------------------------------------------
MsgMonitor::MsgMonitor(QObject* parent) :
QObject(parent)
{
    init();
}

//---------------------------------------------------------------
// MsgMonitor::init
// @see header file
//---------------------------------------------------------------
void MsgMonitor::init()
{
    mBodySize = 0;
    mContainerSize = 0;
    mSubjectSize = 0;

    mMaxMmsSize = 0;
    UniEditorGenUtils* uniEditorGenUtils = new UniEditorGenUtils;
    mMaxMmsSize = KDefaultMaxSize;
    TRAP_IGNORE(mMaxMmsSize = uniEditorGenUtils->MaxMmsMsgSizeL());
}

//---------------------------------------------------------------
// MsgMonitor::~MsgMonitor
// @see header file
//---------------------------------------------------------------
MsgMonitor::~MsgMonitor()
{
}

//---------------------------------------------------------------
// MsgMonitor::messageSize
// @see header file
//---------------------------------------------------------------
int MsgMonitor::messageSize()
{
    return mBodySize + mContainerSize + mSubjectSize;
}

//---------------------------------------------------------------
// MsgMonitor::bodySize
// @see header file
//---------------------------------------------------------------
int MsgMonitor::bodySize()
{
    return mBodySize;
}

//---------------------------------------------------------------
// MsgMonitor::containerSize
// @see header file
//---------------------------------------------------------------
int MsgMonitor::containerSize()
{
    return mContainerSize;
}

//---------------------------------------------------------------
// MsgMonitor::subjectSize
// @see header file
//---------------------------------------------------------------
int MsgMonitor::subjectSize()
{
    return mSubjectSize;
}

//---------------------------------------------------------------
// MsgMonitor::maxMmsSize
// @see header file
//---------------------------------------------------------------
int MsgMonitor::maxMmsSize()
{
    return mMaxMmsSize;
}

//---------------------------------------------------------------
// MsgMonitor::onSizeChanged
// @see header file
//---------------------------------------------------------------
void MsgMonitor::onSizeChanged(int aSize)
{
    // TODO: implement this correctly
    HbWidget* senderWidget = qobject_cast<HbWidget*>(sender());
    
    // if sent by body widget
    MsgUnifiedEditorBody* body = NULL;    
    body = qobject_cast<MsgUnifiedEditorBody*>(senderWidget);
    if(body)
    {
        mBodySize = aSize;
        return;
    }
    
    // if sent by attachment container widget
    MsgAttachmentContainer* container = NULL;
    container = qobject_cast<MsgAttachmentContainer*>(senderWidget);
    if(container)
    {
        mContainerSize = aSize;
        return;
    }

    // if sent by subject widget
    MsgUnifiedEditorSubject* subject = NULL;
    subject = qobject_cast<MsgUnifiedEditorSubject*>(senderWidget);
    if(subject)
    {
        mSubjectSize = aSize;
    }
}

//EOF
