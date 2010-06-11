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
 * Description:
 * Handles saving and opening activites.
 *
 */

#include "msgactivityhandler.h"

#include "msgviewmanager.h"
#include "msgbaseview.h"
#include "msgmainwindow.h"
#include <hbapplication.h>
#include <hbactivitymanager.h>
#include <QVariantHash>

// Activity Names 
const QString ListViewActivityName("MsgConversationsList");

//-----------------------------------------------------------------------------
// MsgActivityHandler::MsgActivityHandler
// Constructor
//-----------------------------------------------------------------------------
MsgActivityHandler::MsgActivityHandler(QObject* parent):
    QObject(parent)
{
  
}

//-----------------------------------------------------------------------------
// MsgActivityHandler::MsgActivityHandler
// Destructor
//-----------------------------------------------------------------------------
MsgActivityHandler::~MsgActivityHandler()
{
   
}

//-----------------------------------------------------------------------------
// MsgActivityHandler::saveActivity
// @see header
//-----------------------------------------------------------------------------
void MsgActivityHandler::saveActivity()
{
    HbActivityManager* activityManager = 
                        qobject_cast<HbApplication*>(qApp)->activityManager();
    
    if( mMainWindow->viewManager()->currentView()== MsgBaseView::CLV)
        {
        // get a screenshot for saving to the activity manager
        QVariantHash metadata;
        metadata.insert("screenshot", 
                QPixmap::grabWidget(mMainWindow, mMainWindow->rect()));

        // save any data necessary to save the state
        QByteArray serializedActivity;
        QDataStream stream(&serializedActivity, 
                QIODevice::WriteOnly | QIODevice::Append);
        stream << ListViewActivityName;

        // add the activity to the activity manager
        bool ok = activityManager->addActivity(ListViewActivityName, 
                serializedActivity, metadata);
        }
}

//-----------------------------------------------------------------------------
// MsgActivityHandler::handleActivity
// @see header
//-----------------------------------------------------------------------------
void MsgActivityHandler::handleActivity(const QVariant &activityData)
{
  // To be handled later 
   Q_UNUSED(activityData)
}

//-----------------------------------------------------------------------------
// MsgActivityHandler::clearActivities
// @see header
//-----------------------------------------------------------------------------
void MsgActivityHandler::clearActivities()
{
    HbActivityManager* activityManager = 
                        qobject_cast<HbApplication*>(qApp)->activityManager();
    activityManager->removeActivity(ListViewActivityName);   
}

//-----------------------------------------------------------------------------
// MsgActivityHandler::setMainWindow
// @see header
//-----------------------------------------------------------------------------
void  MsgActivityHandler::setMainWindow(MsgMainWindow* mainWindow)
{
    mMainWindow = mainWindow;
}
