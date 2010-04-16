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

#include "msgsettingsview.h"
#include "msgsettingsform.h"

#include <cpbasesettingview.h>
#include "msgsettingsdataformcustomitem.h"
#include "debugtraces.h"


MsgSettingsView::MsgSettingsView(QGraphicsItem *parent):
CpBaseSettingView(0,parent)
{
    MsgSettingsForm* settingForm = new MsgSettingsForm(this);
    
    //custom prototype
    MsgSettingsDataFormCustomItem* customPrototype = new MsgSettingsDataFormCustomItem(settingForm);
    settingForm->setItemPrototype(customPrototype);
    
    this->setSettingForm(settingForm);
}
MsgSettingsView::~MsgSettingsView()
{
}
