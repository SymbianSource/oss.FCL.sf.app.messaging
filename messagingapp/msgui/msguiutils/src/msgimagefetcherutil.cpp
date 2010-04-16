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
#include "msgimagefetcherutil.h"

// INCLUDES
#include <QDebug>
#include <QStringList>
#include <xqappmgr.h>
#include <xqaiwrequest.h>

// Constants

//---------------------------------------------------------------
// MsgImageFetcherUtil::MsgImageFetcherUtil
// @see header file
//---------------------------------------------------------------
MsgImageFetcherUtil::MsgImageFetcherUtil(QObject* parent):
QObject(parent),
mRequest(NULL)
{
    mAppMgr = new XQApplicationManager;
}

//---------------------------------------------------------------
// MsgImageFetcherUtil::~MsgImageFetcherUtil
// @see header file
//---------------------------------------------------------------
MsgImageFetcherUtil::~MsgImageFetcherUtil()
{
    if(mAppMgr)
    {
        delete mAppMgr;
        mAppMgr = NULL;
    }
}

//---------------------------------------------------------------
// MsgImageFetcherUtil::fetchImages
// @see header file
//---------------------------------------------------------------
void MsgImageFetcherUtil::fetchImages()
{

    QString interface("Image");
    QString operation("fetch(QVariantMap,QVariant)");
    
    mRequest = mAppMgr->create(interface, operation, false);
    if(!mRequest)
    {
        qDebug() << "AIW-ERROR: NULL request";
        return;
    }

    connect(mRequest, SIGNAL(requestOk(const QVariant&)),
            parent(), SIGNAL(imagesFetched(const QVariant&)));
    connect(mRequest, SIGNAL(requestError(int,const QString&)),
            this, SLOT(onRequestError(int,const QString&)));
    connect(this, SIGNAL(serviceError(const QString&)),
            parent(), SIGNAL(serviceError(const QString&)));
    
    // Make the request
    if (!mRequest->send())
    {
        qDebug() << "AIW-ERROR: Send failed" << mRequest->lastError();
    }    
}

//---------------------------------------------------------------
// MsgImageFetcherUtil::onRequestError
// @see header file
//---------------------------------------------------------------
void MsgImageFetcherUtil::onRequestError(int errorCode,
                                     const QString& errorMessage)
{
    //TODO: handle error here
    Q_UNUSED(errorCode);
    Q_UNUSED(errorMessage);  
    // emit utils mgr signal
    emit serviceError(errorMessage);
}

//EOF
