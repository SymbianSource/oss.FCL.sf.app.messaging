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
 * Description:This is the manager class for managing multiple views
 *
 */

#include "msgsettingsviewmanager.h"
#include "msgadvancedsettingsview.h"
#include "msgsmscenterview.h"
#include "msgbaseview.h"

#include <hbinstance.h>
#include <hbaction.h>

MsgSettingsViewManager::MsgSettingsViewManager(QObject* parent) :
    QObject(parent), mAdvancedSettingsView(NULL),
    mSMSCenterView(NULL),
    mBackAction(NULL)
{
    //do nothing
}

MsgSettingsViewManager::~MsgSettingsViewManager()
{
    if (mAdvancedSettingsView)
    {
        mAdvancedSettingsView->setParent(NULL);
        delete mAdvancedSettingsView;
    }

    if (mSMSCenterView)
    {
        mSMSCenterView->setParent(NULL);
        delete mSMSCenterView;
    }
}

void MsgSettingsViewManager::openAdvancedView()
{
    mAdvancedSettingsView = new MsgAdvancedSettingsView();

    HbMainWindow* mainWindow = hbInstance->allMainWindows().at(0);

    mainWindow->addView(mAdvancedSettingsView);
    mainWindow->setCurrentView(mAdvancedSettingsView);

    mBackAction = new HbAction(Hb::BackAction, this);
    mAdvancedSettingsView->setNavigationAction(mBackAction);
    //mBackAction = mainWindow->softKeyAction(Hb::SecondarySoftKey);

    connect(mBackAction, SIGNAL(triggered()), this, SLOT(closeAdvancedSettingsView()));

    connect(mAdvancedSettingsView, SIGNAL(newSMSCCenterClicked(int)), this, SLOT(openSmsCenterView(int)));
}

void MsgSettingsViewManager::closeAdvancedSettingsView()
{
    disconnect(mBackAction, SIGNAL(triggered()), this, SLOT(closeAdvancedSettingsView()));
            
    mAdvancedSettingsView->commitEmailChanges();
    
    HbMainWindow* mainWindow = hbInstance->allMainWindows().at(0);
    mainWindow->removeView(mAdvancedSettingsView);

    mAdvancedSettingsView->setParent(NULL);
    delete mAdvancedSettingsView;
    mAdvancedSettingsView = NULL;
    
    emit advancedSettingsViewClosed();
}

void MsgSettingsViewManager::openSmsCenterView(int mode)
{
    HbMainWindow* mainWindow = hbInstance->allMainWindows().at(0);
    mSMSCenterView = new MsgSMSCenterView(mode);
    mainWindow->addView(mSMSCenterView);
    mainWindow->setCurrentView(mSMSCenterView);
    mSMSCenterView->setNavigationAction(mBackAction);
    disconnect(mBackAction, SIGNAL(triggered()), this, SLOT(closeAdvancedSettingsView()));
    connect(mBackAction, SIGNAL(triggered()), this, SLOT(closeSMSCCenterView()));    
}

void MsgSettingsViewManager::closeSMSCCenterView()
{
    disconnect(mBackAction, SIGNAL(triggered()), this, SLOT(closeSMSCCenterView()));
    connect(mBackAction, SIGNAL(triggered()), this, SLOT(closeAdvancedSettingsView()));
    
    mSMSCenterView->commitChanges();
    //mAdvancedSettingsView->refreshView();
    
    HbMainWindow* mainWindow = hbInstance->allMainWindows().at(0);
    mainWindow->removeView(mSMSCenterView);

    mSMSCenterView->setParent(NULL);
    delete mSMSCenterView;
    mSMSCenterView = NULL;
}
