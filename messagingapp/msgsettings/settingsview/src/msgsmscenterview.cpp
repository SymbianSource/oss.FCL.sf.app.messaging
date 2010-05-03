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
#include <hbaction.h>

#include "msgsmscenterview.h"
#include "msgsmscentersettingsform.h"
#include "debugtraces.h"

MsgSMSCenterView::MsgSMSCenterView(int view, QGraphicsItem *parent) :
    MsgBaseView(parent)
{
    mSMSCenterSettingsForm = new MsgSMSCenterSettingsForm(view);

    setWidget(mSMSCenterSettingsForm);

    HbAction* backAction = new HbAction(Hb::BackAction, this);
    setNavigationAction(backAction);

    connect(mSMSCenterSettingsForm,
            SIGNAL(deleteMessageCentreAndClose()),
            this,
            SLOT(onCloseMessageCenterView()));
    
    connect(backAction, SIGNAL(triggered()), this, SLOT(onBackAction()));
}

MsgSMSCenterView::~MsgSMSCenterView()
{
    setParent(NULL);
}

void MsgSMSCenterView::onBackAction()
{
    QDEBUG_WRITE("MsgSMSCenterView::onBackAction");

    //commit the change
    mSMSCenterSettingsForm->commitChanges();
    setNavigationAction(NULL);
    emit smsCenterEditViewClosed();
}

void MsgSMSCenterView::onCloseMessageCenterView()
{
    QDEBUG_WRITE("MsgSMSCenterView::onCloseMessageCenterView");

    setNavigationAction(NULL);
    emit smsCenterEditViewClosed();
}

//eof
