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
 * Description:  This class is for advanced settings view
 *
 */

#include "msgadvancedsettingsview.h"
#include "msgadvancedsettingsform.h"
#include "msgsettingsdataformcustomitem.h"

MsgAdvancedSettingsView::MsgAdvancedSettingsView(QGraphicsItem *parent) :
    MsgBaseView(parent), mAdvancedSettingsForm(0)
{

    mAdvancedSettingsForm = new MsgAdvancedSettingsForm(this);

    //custom prototype
    MsgSettingsDataFormCustomItem* customPrototype = 
            new MsgSettingsDataFormCustomItem(mAdvancedSettingsForm);
    
    QList<HbAbstractViewItem*> protos =
            mAdvancedSettingsForm->itemPrototypes();
    protos.append(customPrototype);
    mAdvancedSettingsForm->setItemPrototypes(protos);

    setWidget(mAdvancedSettingsForm);

    connect(mAdvancedSettingsForm, SIGNAL(newSMSCCenterClicked(int)), this,
            SIGNAL(newSMSCCenterClicked(int)));
    }

MsgAdvancedSettingsView::~MsgAdvancedSettingsView()
{
}

void MsgAdvancedSettingsView::refreshView()
    {
    mAdvancedSettingsForm->refresh();
    }

void MsgAdvancedSettingsView::commitEmailChanges()
{
    mAdvancedSettingsForm->commitEmailChanges();
}
