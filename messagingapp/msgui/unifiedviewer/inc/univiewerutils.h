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
 * Description:  Utility class for univiewer.
 *
 */

#ifndef UNI_VIEWER_UTILS_H
#define UNI_VIEWER_UTILS_H

// SYSTEM INCLUDES
#include <QObject>

// FORWARD DECLARATIONS

/**
 * @class UniViewerUtils
 * @brief Utility class for univiewer. Contains common functionality required
 * for widgets.
 */

class UniViewerUtils: public QObject
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit UniViewerUtils(QObject *parent = 0);

    /**
     * Destructor
     */
    virtual ~UniViewerUtils();

    /**
     * Utility function to launch different content viewers.
     * @param mimeType Mime type of the content.
     * @param filePath Absolute file path of content.
     */
    void launchContentViewer(const QString &mimeType, const QString &filePath);

signals:

    /**
     * Service launch complete.
     */
    void requestOk(const QVariant& result);

    /**
     * Service launch errors.
     */
    void requestError(int errorCode, const QString& errorMessage);

private slots:

    /**
     * Service launch complete.
     */
    void handleOk(const QVariant& result);

    /**
     * Service launch errors.
     */
    void handleError(int errorCode, const QString& errorMessage);

private:

    /**
     * Launch content viewer using XQSharableFile.
     * @param filePath Absolute path of content.
     */
    void launchViaSharableFile(const QString &filePath);

};

#endif // UNI_VIEWER_UTILS_H
//eof
