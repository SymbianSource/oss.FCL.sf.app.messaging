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

#ifndef UNIEDITORSMSPLUGIN_SH_H_
#define UNIEDITORSMSPLUGIN_SH_H_

#include <QObject>
#include <QThread>

#include <msvapi.h>


class SessionEventHandler : public QThread,
public MMsvSessionObserver
{
    Q_OBJECT
public:
    SessionEventHandler(QThread* parent=0);
    ~SessionEventHandler();

public:

    /**
     * @see MMsvSessionObserver
     */
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
        TAny* aArg3);

private:
    void run();

};

#endif /* UNIEDITORSMSPLUGIN_SH_H_ */
