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
 * Description:This class is for sms message center view 
 *
 */

#include "msgsmscenterview.h"
#include "msgsmscentersettingsform.h"

#include <cpbasesettingview.h>
#include "debugtraces.h"

MsgSMSCenterView::MsgSMSCenterView(int view, QGraphicsItem *parent) :
MsgBaseView(parent)
{
    mSMSCenterSettingsForm = new MsgSMSCenterSettingsForm(view);

    setWidget(mSMSCenterSettingsForm);
}

MsgSMSCenterView::~MsgSMSCenterView()
{
}

void MsgSMSCenterView::commitChanges()
{
	QDEBUG_WRITE("MsgSMSCenterView::commitChanges");
    mSMSCenterSettingsForm->commitChanges();
}
