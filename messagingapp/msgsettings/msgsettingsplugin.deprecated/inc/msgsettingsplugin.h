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
 * Description: This is for Message Settings plugin for ControlPanel
 *
 */

#ifndef MSG_SETTINGS_PLUGIN_H
#define MSG_SETTINGS_PLUGIN_H

#include <cpplugininterface.h>
class MsgSettingsPlugin : public QObject, public CpPluginInterface
{
Q_OBJECT
    Q_INTERFACES(CpPluginInterface)
public:
    MsgSettingsPlugin();
    ~MsgSettingsPlugin();
    
    virtual QList<CpSettingFormItemData*> createSettingFormItemData(CpItemDataHelper &itemDataHelper) const;
};
#endif // MSG_SETTINGS_PLUGIN_H
