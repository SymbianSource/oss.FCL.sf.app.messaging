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

#ifndef MSGSETTINGSVIEWMANAGER_H
#define MSGSETTINGSVIEWMANAGER_H

#include <QObject>
class MsgAdvancedSettingsView;
class MsgSMSCenterView;
class HbAction;
class MsgBaseView;

/**
 * View manager for the messaging application. This is a singleton
 * class. Creates and activates the specified view.
 */
class MsgSettingsViewManager : public QObject
{
Q_OBJECT

public:
    /**
     * Get an instance of the view manager
     */
    MsgSettingsViewManager(QObject* parent = 0);

    /**
     * Destructor
     */
    virtual ~MsgSettingsViewManager();

    /**
     * open the advanced settings view
     */
    void openAdvancedView();

    /**
     * open messageCenterView
     * @param mode display either in addition or edit mode
     * @example 0 - displays in add mode
     * 			1 - displays in edit mode
     */
    void openMessageCenterView(int mode);

signals:
    void advancedSettingsViewClosed();
    
private slots:
    void closeAdvancedSettingsView();
    void openSmsCenterView(int mode);
    void closeSMSCCenterView();

private:

    /**
     * List View.
     * Own.
     */
    MsgAdvancedSettingsView* mAdvancedSettingsView;

    /**
     * SMS Center view
     */
    MsgSMSCenterView* mSMSCenterView;

    HbAction* mBackAction;    
};

#endif /* MSGSETTINGSVIEWMANAGER_H */
