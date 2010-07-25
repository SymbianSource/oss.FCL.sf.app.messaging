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
 * Description: Utility class for univiewer.
 *
 */

#include "univiewerutils.h"

// SYSTEM INCLUDES
#include <xqaiwrequest.h>
#include <xqrequestinfo.h>
#include <xqappmgr.h>
#include <hbglobal.h>

// USER INCLUDES
#include "msgcontactsutil.h"

// LOCAL CONSTANTS
#define LOC_TITLE   hbTrId("txt_messaging_title_messaging")

const QString IMAGE_MIMETYPE("image");
const QString AUDIO_MIMETYPE("audio");
const QString VCARD_MIMETYPE("text/X-vCard");
const QString VIDEO_MIMETYPE("video");

//---------------------------------------------------------------
// UniViewerUtils::UniViewerUtils
// @see header file
//---------------------------------------------------------------
UniViewerUtils::UniViewerUtils(QObject *parent) :
    QObject(parent)
{
}

//---------------------------------------------------------------
// UniViewerUtils::~UniViewerUtils
// @see header file
//---------------------------------------------------------------
UniViewerUtils::~UniViewerUtils()
{
}

//---------------------------------------------------------------
// UniViewerUtils::addAttachmentWidget
// @see header file
//---------------------------------------------------------------
void UniViewerUtils::launchContentViewer(const QString &mimeType, const QString &filePath)
{
    if (mimeType.contains(VCARD_MIMETYPE, Qt::CaseInsensitive)) {
        MsgContactsUtil::launchVCardViewer(filePath);
    }
    else {
        launchViaSharableFile(filePath);
    }
}

//---------------------------------------------------------------
// UniViewerUtils::handleOk
// @see header file
//---------------------------------------------------------------
void UniViewerUtils::handleOk(const QVariant& result)
{
    emit requestOk(result);
}

//---------------------------------------------------------------
// UniViewerUtils::handleError
// @see header file
//---------------------------------------------------------------
void UniViewerUtils::handleError(int errorCode, const QString& errorMessage)
{
    emit requestError(errorCode, errorMessage);
}

//---------------------------------------------------------------
// UniViewerUtils::launchViaSharableFile
// @see header file
//---------------------------------------------------------------
void UniViewerUtils::launchViaSharableFile(const QString &filePath)
{
    XQSharableFile sf;
    XQAiwRequest* request = 0;

    if (!sf.open(filePath)) {
        return;
    }

    // Get handlers
    XQApplicationManager appManager;
    QList<XQAiwInterfaceDescriptor> fileHandlers = appManager.list(sf);
    if (fileHandlers.count() > 0) {
        XQAiwInterfaceDescriptor d = fileHandlers.first();
        request = appManager.create(sf, d);

        if (!request) {
            sf.close();
            return;
        }
    }
    else {
        sf.close();
        return;
    }

    // Result handlers
    connect(request, SIGNAL(requestOk(const QVariant&)), this, SLOT(handleOk(const QVariant&)),
        Qt::UniqueConnection);
    connect(request, SIGNAL(requestError(int,const QString&)), this,
        SLOT(handleError(int,const QString&)), Qt::UniqueConnection);

    request->setEmbedded(true);
    request->setSynchronous(true);

    // Fill args
    QList<QVariant> args;
    args << qVariantFromValue(sf);
    request->setArguments(args);

    // Fill headers
    QString key("WindowTitle");
    QVariant value(QString(LOC_TITLE));
    XQRequestInfo info;
    info.setInfo(key, value);
    request->setInfo(info);

    request->send();

    // Cleanup
    sf.close();
    delete request;
}

//EOF
