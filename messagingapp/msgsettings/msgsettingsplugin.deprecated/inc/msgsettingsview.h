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

#ifndef MSGSETTINGSVIEW_H
#define MSGSETTINGSVIEW_H

#include <cpbasesettingview.h>

class MsgSettingsView: public CpBaseSettingView
{
    Q_OBJECT
public:
    explicit MsgSettingsView(QGraphicsItem *parent = 0);
    ~MsgSettingsView();
};
#endif // MSGSETTINGSVIEW_H
