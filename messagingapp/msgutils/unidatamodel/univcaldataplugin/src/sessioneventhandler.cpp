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

#include "sessioneventhandler.h"

SessionEventHandler::SessionEventHandler(QThread* parent):
QThread(parent)
{
}

SessionEventHandler::~SessionEventHandler()
{
}

//---------------------------------------------------------------
// SessionEventHandler::HandleSessionEventL
// @see header
//---------------------------------------------------------------
void SessionEventHandler::HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
                                                  TAny* /*aArg1*/,
                                                  TAny* /*aArg2*/, TAny* /*aArg3*/)
{
// do nothing
}

//---------------------------------------------------------------
// SessionEventHandler::run
// @see header
//---------------------------------------------------------------
void SessionEventHandler::run()
{
    exec();
}

// EOF
