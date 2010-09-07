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
 * Main window of the messaging application. Activates the default view.
 *
 */

#include "msgmainwindow.h"

#include "msgviewmanager.h"
#include "msgserviceinterface.h"
#include "msgsendserviceinterface.h"
#include "conversationsengine.h"

#include <QKeyEvent>


//---------------------------------------------------------------
// MsgMainWindow::MsgMainWindow
// Constructor
//---------------------------------------------------------------
MsgMainWindow::MsgMainWindow(bool serviceRequest,int activityMsgId,QWidget *parent) :
HbMainWindow(parent), mMsgSI(0), mMsgSendSI(0)
{
    mViewManager = new MsgViewManager(serviceRequest,this,this,activityMsgId);
    mMsgSI = new MsgServiceInterface(NULL,mViewManager);
    mMsgSendSI = new MsgSendServiceInterface(NULL,mViewManager);
        
    //Model creation
    ConversationsEngine::instance();
    
}

//---------------------------------------------------------------
// MsgMainWindow::~MsgMainWindow
// Constructor
//---------------------------------------------------------------
MsgMainWindow::~MsgMainWindow()
{   
    if (mMsgSI)
    {
        delete mMsgSI;
        mMsgSI = NULL;
    }
    if (mMsgSendSI)
    {
        delete mMsgSendSI;
        mMsgSendSI = NULL;
    }
    
    delete ConversationsEngine::instance();
}

//---------------------------------------------------------------
// MsgMainWindow::viewManager
// Constructor
//---------------------------------------------------------------
MsgViewManager* MsgMainWindow::viewManager()
{   
     return mViewManager; 
}

//---------------------------------------------------------------
// MsgMainWindow::keyPressEvent
// @see header
//---------------------------------------------------------------
void MsgMainWindow::keyPressEvent(QKeyEvent *event)
{
    bool eventHandled = false;
    if (Qt::Key_Yes == event->key()) {
        eventHandled = mViewManager->handleKeyEvent(event->key());
    }

    if (eventHandled) {
        event->accept();
    }
    else {
        HbMainWindow::keyPressEvent(event);
    }

}
// End of file
