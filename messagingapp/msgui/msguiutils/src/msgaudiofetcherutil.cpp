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

 // USER INCLUDES
#include "msgaudiofetcherutil.h"
#include "debugtraces.h"

// INCLUDES
#include <QStringList>
#include <xqappmgr.h>
#include <xqaiwrequest.h>

// Constants

//---------------------------------------------------------------
// MsgAudioFetcherUtil::MsgAudioFetcherUtil
// @see header file
//---------------------------------------------------------------
MsgAudioFetcherUtil::MsgAudioFetcherUtil(QObject* parent):
QObject(parent),
mRequest(NULL)
{
    mAppMgr = new XQApplicationManager;
}

//---------------------------------------------------------------
// MsgAudioFetcherUtil::~MsgAudioFetcherUtil
// @see header file
//---------------------------------------------------------------
MsgAudioFetcherUtil::~MsgAudioFetcherUtil()
{
    delete mAppMgr;
}

//---------------------------------------------------------------
// MsgAudioFetcherUtil::fetchAudio
// @see header file
//---------------------------------------------------------------
void MsgAudioFetcherUtil::fetchAudio()
{
	QString service("Music Fetcher");
    QString interface("com.nokia.services.media.Music");
    QString operation("fetch(QString)");

    mRequest = mAppMgr->create(service, interface, operation, false);
    if(!mRequest)
    {
        QDEBUG_WRITE("AIW-ERROR: NULL request");
        return;
    }

    connect(mRequest, SIGNAL(requestOk(const QVariant&)),
            parent(), SIGNAL(audiosFetched(const QVariant&)));
    connect(mRequest, SIGNAL(requestError(int,const QString&)),
            this, SLOT(onRequestError(int,const QString&)));
    connect(this, SIGNAL(serviceError(const QString&)),
            parent(), SIGNAL(serviceError(const QString&)));

    // Make the request
    if (!mRequest->send())
    {
        QDEBUG_WRITE_FORMAT("AIW-ERROR: Send failed  ",mRequest->lastError());
    }
}

//---------------------------------------------------------------
// MsgAudioFetcherUtil::onRequestError
// @see header file
//---------------------------------------------------------------
void MsgAudioFetcherUtil::onRequestError(int errorCode,
                                     const QString& errorMessage)
{
    Q_UNUSED(errorCode);
    // emit utils mgr signal
    emit serviceError(errorMessage);
}

//EOF
