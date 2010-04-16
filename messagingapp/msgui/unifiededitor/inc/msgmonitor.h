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
 * Description: Helper class to monitor msg construction in unified editor
 *
 */

#ifndef MSG_MONITOR_H
#define MSG_MONITOR_H

#include <QObject>

class MsgMonitor : public QObject
    {
    Q_OBJECT

public:
    /**
     * Constructor
     */
    MsgMonitor(QObject* parent=0);

    /**
     * Destructor
     */
    ~MsgMonitor();

    /**
     * setter method to initialize content
     */
    void init();

    /**
     * seeker function to get current msg size in bytes
     */
    static int messageSize();

    /**
     * seeker function to get current msg's body size 
     * in bytes
     */
    static int bodySize();

    /**
     * seeker function to get current msg's attachment
     * container size in bytes
     */
    static int containerSize();

    /**
     * seeker function to get current msg's subject
     * size in bytes
     */
    static int subjectSize();

    /**
     * seeker function to get max possible mms size
     * in bytes
     */
    static int maxMmsSize();

public slots:
    /**
     * handle size change signal emitted by msg components
     */
    void onSizeChanged(int aSize);
        
private:
    /**
     * msg body size in bytes
     */
    static int mBodySize;
    
    /**
     * msg container size in bytes
     */
    static int mContainerSize;
    
    /**
     * msg subject size in bytes
     */
    static int mSubjectSize;

    /**
     * max mms size in bytes
     */
    static int mMaxMmsSize;
    };

#endif //MSG_MONITOR_H
