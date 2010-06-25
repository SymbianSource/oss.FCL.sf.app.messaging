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
 * Description: Message services manages all messaging Qt highway services. 
 *
 */

#include "msgservicewindow.h"

#include "msgviewinterface.h"
#include "msgsendinterface.h"
#include "msgshareuiinterface.h"

#include "msgstorehandler.h"
#include "msgserviceviewmanager.h"

// LOCALIZATION
#define LOC_BUTTON_DELETE hbTrId("txt_common_button_delete")
#define LOC_BUTTON_CANCEL hbTrId("txt_common_button_cancel")
#define LOC_DELETE_MESSAGE hbTrId("txt_messaging_dialog_delete_message")


// ----------------------------------------------------------------------------
// MsgServiceWindow::MsgServiceWindow
// @see header
// ----------------------------------------------------------------------------
MsgServiceWindow::MsgServiceWindow():HbMainWindow(),mSendInterface(NULL),
mViewInterface(NULL),
mShareUiInterface(NULL),
mStoreHandler(NULL),
mViewManager(NULL)
    {
    // create stote handler
    mStoreHandler = new MsgStoreHandler();
    
    // create view manager
    mViewManager = new MsgServiceViewManager(mStoreHandler,this);
    
    // create interfaces 
    mSendInterface = new MsgSendInterface(mViewManager);
    mViewInterface = new MsgViewInterface(mViewManager);
    mShareUiInterface = new MsgShareUiInterface(mViewManager);
    }

// ----------------------------------------------------------------------------
// MsgServiceWindow::~MsgServiceWindow
// @see header
// ----------------------------------------------------------------------------
MsgServiceWindow::~MsgServiceWindow()
    {
    if(mShareUiInterface)
        {
        delete mShareUiInterface;
        }
    if(mViewInterface)
        {
        delete mViewInterface;
        }
    if(mSendInterface)
        {
        delete mSendInterface;
        }
    if(mViewManager)
        {
        delete mViewManager;
        }

    if(mStoreHandler)
        {
        delete mStoreHandler;
        }
    }



    


