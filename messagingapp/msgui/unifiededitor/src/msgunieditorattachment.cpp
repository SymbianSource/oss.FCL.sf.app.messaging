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
#include <HbIconItem>
#include <QFileInfo>
#include <QFont>
#include <HbPushButton>
#include <HbFrameDrawer>
#include <HbMenu>
#include <MsgMimeTypes.h>

// USER INCLUDES
#include "msgunieditorattachment.h"
#include "unieditorgenutils.h"
#include "s60qconversions.h"

// Constants
#define BYTES_TO_KBYTES_FACTOR 1024
#define BG_FRAME "qtg_fr_groupbox"

MsgUnifiedEditorAttachment::MsgUnifiedEditorAttachment( const QString& pluginPath,
                                                        const QString& attachmentpath,
                                                        const int filesize,
                                                        QGraphicsItem *parent ) :
HbWidget(parent),
mPluginPath(pluginPath),
mPath(attachmentpath),
mSize(filesize),
mMimeType(QString()),
mAttachmentIcon(0),
mAttachmentName(0),
mFrameItem(0)
{
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgUnifiedEditorAttachment calling HbStyle::registerPlugin";
#endif

        setPluginBaseId(style()->registerPlugin(mPluginPath));

        mAttachmentIcon = new HbIconItem(":/qtg_small_attachment.svg", this);
        HbStyle::setItemName(mAttachmentIcon,"attachmentIcon");

        // TODO: use utility to get mimetype and size
        int at_size = 0;
        TMsgMediaType mediaType = EMsgMediaUnknown;
        UniEditorGenUtils* genUtils = new UniEditorGenUtils;
        TRAP_IGNORE(genUtils->getFileInfoL(mPath,at_size,
                                           mMimeType,mediaType));
        delete genUtils;
        QFileInfo fileinfo(attachmentpath);
        QString filename = fileinfo.fileName();
        mAttachmentName = new HbTextItem(filename,this);
        HbStyle::setItemName(mAttachmentName,"attachmentName");
        mAttachmentName->setElideMode(Qt::ElideRight);

        int sizeInKb = mSize/BYTES_TO_KBYTES_FACTOR;
        QString fileDetails;
        if(sizeInKb > 1)
        {
            fileDetails = QString().append(QString("(%1 Kb)").arg(sizeInKb));
        }
        else
        {
            fileDetails = QString().append(QString("(%1 B)").arg(mSize));
        }

        mAttachmentDetails = new HbTextItem(fileDetails, this);
        HbStyle::setItemName(mAttachmentDetails,"attachmentDetails");
        mAttachmentDetails->setElideMode(Qt::ElideNone);

        // set underlined font
        QFont underlinedFont(this->font());
        underlinedFont.setUnderline(true);
        mAttachmentName->setFont(underlinedFont);
        mAttachmentDetails->setFont(underlinedFont);

        mFrameItem = new HbPushButton(this);
        HbStyle::setItemName(mFrameItem, "bgFrame");
        HbFrameDrawer *fd = new HbFrameDrawer(BG_FRAME, HbFrameDrawer::NinePieces);
        mFrameItem->setFrameBackground(fd);
        connect(mFrameItem, SIGNAL(clicked(bool)), this, SIGNAL(clicked()));
        connect(mFrameItem, SIGNAL(longPress(QPointF)), this, SLOT(longPressed(QPointF)));
}

MsgUnifiedEditorAttachment::~MsgUnifiedEditorAttachment()
{
    style()->unregisterPlugin(mPluginPath);
}

const QString& MsgUnifiedEditorAttachment::path()
{
    return mPath;
}

qreal MsgUnifiedEditorAttachment::size()
{
    return mSize;
}

const QString& MsgUnifiedEditorAttachment::mimeType()
{
    return mMimeType;
}

void MsgUnifiedEditorAttachment::longPressed(QPointF position)
{
    HbMenu* menu = new HbMenu;
    menu->addAction(tr("Open"), this, SLOT(openAttachment()));
    menu->addAction(tr("Remove"), this, SLOT(removeAttachment()));
    menu->addAction(tr("View details"), this, SLOT(viewDetails()));
    menu->setDismissPolicy(HbPopup::TapAnywhere);
    menu->setAttribute(Qt::WA_DeleteOnClose, true);
    menu->setPreferredPos(position);
    menu->show();
}

void MsgUnifiedEditorAttachment::removeAttachment()
{
    emit deleteMe(this);
}

void MsgUnifiedEditorAttachment::openAttachment()
{
    //open corresponding viewer app.
}

void MsgUnifiedEditorAttachment::viewDetails()
{
    //open details view.
}

bool MsgUnifiedEditorAttachment::isMultimediaContent()
{
    bool ret = true;
    QString vcard = S60QConversions::s60Desc8ToQString(KMsgMimeVCard());
    QString vcal = S60QConversions::s60Desc8ToQString(KMsgMimeVCal());
    QString ical = S60QConversions::s60Desc8ToQString(KMsgMimeICal());
    if( !QString::compare(mMimeType, vcard, Qt::CaseInsensitive) ||
        !QString::compare(mMimeType, vcal, Qt::CaseInsensitive) ||
        !QString::compare(mMimeType, ical, Qt::CaseInsensitive) )
    {
        // vcard, vcal are not mm content
        ret = false;
    }
    return ret;
}

// EOF
