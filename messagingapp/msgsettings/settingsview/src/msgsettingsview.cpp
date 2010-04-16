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

#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbinstance.h>

#include "msgsettingsview.h"
#include "msgsettingsform.h"
#include "msgsettingsdataformcustomitem.h"
#include "msgsettingsviewmanager.h"
#include "debugtraces.h"

MsgSettingsView::MsgSettingsView(QGraphicsItem *parent) :
    MsgBaseView(parent)
{
    mSettingsViewManager = new MsgSettingsViewManager(this);
    
    MsgSettingsForm* settingForm = new MsgSettingsForm(
                                                       mSettingsViewManager,
                                                       this);

    //custom prototype
    MsgSettingsDataFormCustomItem* customPrototype =
            new MsgSettingsDataFormCustomItem(settingForm);
    //settingForm->setItemPrototype(customPrototype);
    
    QList<HbAbstractViewItem*> protos = settingForm->itemPrototypes();
    protos.append(customPrototype);
    settingForm->setItemPrototypes(protos);

    setWidget(settingForm);

    mMainWindow = hbInstance->allMainWindows().at(0);

    mBackAction = new HbAction(Hb::BackAction, this);
    
    connectCloseAction();    
    
    connect (mSettingsViewManager,
             SIGNAL(advancedSettingsViewClosed()),
             this,
             SLOT(connectCloseAction()));
}

MsgSettingsView::~MsgSettingsView()
{
   //do nothing 
}

void MsgSettingsView::connectCloseAction()
{
    this->setNavigationAction(mBackAction);
    connect(mBackAction, SIGNAL(triggered()), this, SLOT(closeSettings()), Qt::UniqueConnection);
}

void MsgSettingsView::closeSettings()
{
    QVariantList param;
    param << MsgBaseView::DEFAULT;
    param << MsgBaseView::MSGSETTINGS;
    emit switchView(param);
}
