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
 * Description:This is the first view for msgsettings plugin 
 *
 */
#include <hbmainwindow.h>

#include "msgsettingsview.h"
#include "msgsettingsform.h"
#include "msgsmscenterview.h"
#include "debugtraces.h"

MsgSettingsView::MsgSettingsView(QGraphicsItem *parent) :
    MsgBaseView(parent), mSMSCenterView(0)
{
    mSettingsForm = new MsgSettingsForm();

    setWidget(mSettingsForm);
    mMainWindow = this->mainWindow();

    connect(mSettingsForm,
            SIGNAL(newSMSCCenterClicked(int)),
            this,
            SLOT(onNewSMSCCenterClicked(int)));
}

MsgSettingsView::~MsgSettingsView()
{
    delete mSMSCenterView;
}

void MsgSettingsView::onNewSMSCCenterClicked(int index)
{
    //open the sms center views
    if (mSMSCenterView)
    {
        delete mSMSCenterView;
        mSMSCenterView = NULL;
    }

    mSMSCenterView = new MsgSMSCenterView(index);
    
    connect(mSMSCenterView,
            SIGNAL(smsCenterEditViewClosed()),
            this,
            SLOT(onSmsCenterEditViewClosed()));
    
    mMainWindow->addView(mSMSCenterView);        
    mMainWindow->setCurrentView(mSMSCenterView);
}

void MsgSettingsView::onSmsCenterEditViewClosed()
{
    //remove the view 
    mMainWindow->removeView(mSMSCenterView);

    //refresh the form
    mSettingsForm->refreshViewForm();

    //add the current view on top
    mMainWindow->setCurrentView(this);
}

//eof

