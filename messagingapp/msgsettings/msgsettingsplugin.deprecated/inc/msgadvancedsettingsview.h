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

#ifndef MSG_ADVANCEDSETTINGS_VIEW_H_
#define MSG_ADVANCEDSETTINGS_VIEW_H_

#include <cpbasesettingview.h>

class MsgAdvancedSettingsForm;

class MsgAdvancedSettingsView : public CpBaseSettingView
{
Q_OBJECT
public:
    explicit MsgAdvancedSettingsView(QGraphicsItem *parent = 0);
    ~MsgAdvancedSettingsView();
    /**
     * refreshes the view.
     */
    void refreshView();

    /**
     * This is for update Email fields into settings engine
     */
    void commitEmailChanges();
    
signals:
    void newSMSCCenterClicked(int);

private:
    MsgAdvancedSettingsForm* mAdvancedSettingsForm;
};

#endif /* MSG_ADVANCEDSETTINGS_VIEW_H_ */
