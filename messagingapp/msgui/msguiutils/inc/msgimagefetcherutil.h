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
 * Description: Image fetcher helper class
 *
 */

#ifndef MSG_IMAGEFETCHER_UTIL_H
#define MSG_IMAGEFETCHER_UTIL_H

#include <QObject>
#include <QVariant>

class XQApplicationManager;
class XQAiwRequest;

class MsgImageFetcherUtil : public QObject
    {
    Q_OBJECT

public:
    /**
     * Constructor
     */
    MsgImageFetcherUtil(QObject* parent=0);

    /**
     * Destructor
     */
    ~MsgImageFetcherUtil();
   
    /**
     * Fetch images 
     */
    void fetchImages();

    signals:
    
    /**
     * emit when result is available
     */
    void resultReady(const QVariant& result);
    
    /**
     * Signal emitted when an error is generated.
     * @param error The error as a string to be displayed on the screen.
     */

    void serviceError(const QString& error);

    private slots:
    /**
     * connects to XQApplication manager failure signal
     */
    void onRequestError(int errorCode, const QString& errorMessage);

private:
   
    /**
     * ApplicationManager
     * Owned
     */
    XQApplicationManager* mAppMgr;
    
    /**
     * AiwRequest
     * Not Owned
     */
    XQAiwRequest* mRequest;

    };

#endif //MSG_IMAGEFETCHER_UTIL_H
