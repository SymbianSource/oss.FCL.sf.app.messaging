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

#ifndef MSGACTIVITYHANDLER_H_
#define MSGACTIVITYHANDLER_H_

#include <QObject>
#include <QVariant>

class MsgMainWindow;
class MsgViewManager;

class MsgActivityHandler: public QObject
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    MsgActivityHandler( QObject* parent = 0);
                  
    /**
     * Destructor.
     */
   ~MsgActivityHandler();

public slots:

     /**
      * Saves the current running activity.
      */
    void saveActivity();
    
    /**
     * Open the activity which is requested.
     * @param activitry data.
     */
    void handleActivity(const QVariant &activityData);
    
public:   
   /** 
    * Set the message main window pointer.
    * @param mainWindow message main window.
    */
    void setMainWindow(MsgMainWindow* mainWindow);
   
    /**
     * Clears all saved messaging activities. 
     */
    void clearActivities();
    
private:
    /**
     * main window reference not owned.
     */
    MsgMainWindow* mMainWindow;
};

#endif /* MSGACTIVITYHANDLER_H_ */
