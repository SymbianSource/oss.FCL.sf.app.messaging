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
 * Description: attachment container class
 *
 */


// INCLUDES
#include <QGraphicsLinearLayout>
#include <QFileInfo>
#include <HbFrameItem>
#include <HbFrameDrawer>

// USER INCLUDES
#include "msgattachmentcontainer.h"
#include "unieditorgenutils.h"
#include "msgmonitor.h"
#include "mmsinsertcheckoperation.h"

// Constants

//---------------------------------------------------------------
// MsgAttachmentContainer::MsgAttachmentContainer
// @see header file
//---------------------------------------------------------------
MsgAttachmentContainer::MsgAttachmentContainer( const QString& pluginPath, QGraphicsItem *parent ) :
HbWidget(parent),
mPluginPath(pluginPath),
mIsMMContent(false)
{
    mLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    mLayout->setContentsMargins(0,0,0,0);
    mLayout->setSpacing(0);
    mMmsInsertCheckOp = new MmsInsertCheckOperation;
}

//---------------------------------------------------------------
// MsgAttachmentContainer::~MsgAttachmentContainer
// @see header file
//---------------------------------------------------------------
MsgAttachmentContainer::~MsgAttachmentContainer()
{
    delete mMmsInsertCheckOp;
}

//---------------------------------------------------------------
// MsgAttachmentContainer::addAttachment
// @see header file
//---------------------------------------------------------------
void MsgAttachmentContainer::addAttachment(const QString& filepath)
{
    //check for insert conformance
    if(EInsertSuccess != mMmsInsertCheckOp->checkModeForInsert(filepath))
        return;

    int msgSize = messageSize();
    QFileInfo fileinfo(filepath);
    int fileSize = fileinfo.size() + KEstimatedMimeHeaderSize;
    
    if( (fileSize + msgSize) <= MsgMonitor::maxMmsSize())
    {
        MsgUnifiedEditorAttachment* att = new MsgUnifiedEditorAttachment(
            mPluginPath, filepath, fileSize, this);
        if( ((mAttachmentList.count() == 0) && att->isMultimediaContent()) ||
                ((mAttachmentList.count() == 1) && !mIsMMContent) )
        {
            mIsMMContent = true;
            emit mmContentAdded(true);
        }
        mAttachmentList << att;
        int index = mLayout->count() - 1;
        mLayout->insertItem(index,att);
        connect(att, SIGNAL(deleteMe(MsgUnifiedEditorAttachment*)),
            this, SLOT(deleteAttachment(MsgUnifiedEditorAttachment*)));

        // emit to signal that container size changed
        emit sizeChanged(containerSize());
    }
    else if(mAttachmentList.count() == 0)
    {
        emit emptyAttachmentContainer();
    }
}

//---------------------------------------------------------------
// MsgAttachmentContainer::deleteAttachment
// @see header file
//---------------------------------------------------------------
void MsgAttachmentContainer::deleteAttachment(MsgUnifiedEditorAttachment* attachment)
{
    mAttachmentList.removeOne(attachment);
    mLayout->removeItem(attachment);
    attachment->setParent(NULL);
    delete attachment;

    if( ((mAttachmentList.count() == 1) && !mAttachmentList.first()->isMultimediaContent()) ||
        ((mAttachmentList.count() == 0) && mIsMMContent) )
    {
        mIsMMContent = false;
        emit mmContentAdded(false);
    }

    // emit to indicate change in container size
    emit sizeChanged(containerSize());

    if(mAttachmentList.count() == 0)
    {
        emit emptyAttachmentContainer();
    }
}

//---------------------------------------------------------------
// MsgAttachmentContainer::count
// @see header file
//---------------------------------------------------------------
int MsgAttachmentContainer::count()
{
    return mAttachmentList.count();
}

//---------------------------------------------------------------
// MsgAttachmentContainer::attachmentList
// @see header file
//---------------------------------------------------------------
MsgUnifiedEditorAttachmentList MsgAttachmentContainer::attachmentList()
{
    return mAttachmentList;
}

//---------------------------------------------------------------
// MsgAttachmentContainer::containerSize
// @see header file
//---------------------------------------------------------------
int MsgAttachmentContainer::containerSize()
{
    int attCount = count();
    int containerSize = 0;
    
    for(int i=0; i<attCount; i++)
    {
        containerSize += mAttachmentList.at(i)->size();
    }
    return containerSize;
}

//---------------------------------------------------------------
// MsgAttachmentContainer::messageSize
// @see header file
//---------------------------------------------------------------
int MsgAttachmentContainer::messageSize()
{
    return containerSize() + MsgMonitor::bodySize() + MsgMonitor::subjectSize();
}

//EOF

