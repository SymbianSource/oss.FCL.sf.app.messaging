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
 * Description:Conversation Base view.
 *
 */

#include "msgconversationbaseview.h"

// SYSTEM INCLUDES
#include <QGraphicsLinearLayout>
#include <HbMainWindow>
#include <HbNotificationDialog>

#include <xqaiwrequest.h>
#include <xqappmgr.h>

#include <cntservicescontact.h>
#include <qtcontactsglobal.h>
#include <qtcontacts.h>
#include <ccsdefs.h>

// USER INCLUDES
#include "msgconversationview.h"
#include "conversationsengine.h"
#include "msgviewdefines.h"
#include "conversationsenginedefines.h"
#include "msgcontactcardwidget.h"

QTM_USE_NAMESPACE

// LOCALIZATION
#define LOC_SAVED_TO_DRAFTS    hbTrId("txt_messaging_dpopinfo_saved_to_drafts")

//---------------------------------------------------------------
// MsgConversationBaseView::MsgConversationBaseView
// Constructor
//---------------------------------------------------------------
MsgConversationBaseView::MsgConversationBaseView(QGraphicsItem* parent) :
MsgBaseView(parent),
mConversationView(NULL),
mConversationId(-1)
{ 
    connect(this->mainWindow(),SIGNAL(viewReady()),this,SLOT(doDelayedConstruction()));   
    initView();
}

//---------------------------------------------------------------
// MsgConversationBaseView::~MsgConversationBaseView
// Destructor
//---------------------------------------------------------------
MsgConversationBaseView::~MsgConversationBaseView()
{   
}

//---------------------------------------------------------------
// MsgConversationBaseView::openConversationView
// Launches conversation view for entry id and mode
//---------------------------------------------------------------
void MsgConversationBaseView::openConversation(qint64 convId)
{
    ConversationsEngine::instance()->getConversations(convId);
    mConversationId = convId;
    connect(this->mainWindow(),SIGNAL(viewReady()),this,SLOT(doDelayedConstruction()));
    
    if(mConversationView)
        {
        mConversationView->refreshView();
        }
}

//---------------------------------------------------------------
// MsgConversationBaseView::initView
// create and initialise the conversationview
//---------------------------------------------------------------
void MsgConversationBaseView::initView()
    {
    
    // Create header widget
    mContactCard = new MsgContactCardWidget(this);

    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical);

    qreal spacing = HbDeviceProfile::profile(this).unitValue();
    mMainLayout->setSpacing(spacing);
    mMainLayout->setContentsMargins(CONTENT_MARGIN, CONTENT_MARGIN,
                                   CONTENT_MARGIN, CONTENT_MARGIN);

    mMainLayout->addItem(mContactCard);

    /**
     * Create conversation view and connect to proper signals.
     * NOTE: contactCardWidget is NOT parent of MsgConversationView.
     * Just passing reference to MsgConversationView.
     */
    mConversationView = new MsgConversationView(mContactCard);

    mConversationView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(mConversationView, SIGNAL(closeConversationView()),
            this, SLOT(closeConversationView()));

    connect(mConversationView,SIGNAL(replyStarted()),
            this,SLOT(markMessagesAsRead()));

    connect(mConversationView, SIGNAL(switchView(const QVariantList&)),
            this, SIGNAL(switchView(const QVariantList&)));
    
    connect(mConversationView,SIGNAL(hideChrome(bool)),this,SLOT(hideChrome(bool)));

    this->setMenu(mConversationView->menu());

    mMainLayout->addItem(mConversationView);

    this->setLayout(mMainLayout);

}

//---------------------------------------------------------------
// MsgConversationBaseView::closeConversationView
// close conversation view
//---------------------------------------------------------------
void MsgConversationBaseView::closeConversationView()
{
   markMessagesAsRead();
}

//---------------------------------------------------------------
// MsgConversationBaseView::markMessagesAsRead
// Mark unread msgs in cv as read
//---------------------------------------------------------------
void MsgConversationBaseView::markMessagesAsRead()
    {
    if( mConversationId >= 0)
        {
        ConversationsEngine::instance()->markConversationRead(mConversationId);
        }
    }

//---------------------------------------------------------------
// MsgConversationBaseView::saveContentToDrafts
// saves the editors content to drafts
//---------------------------------------------------------------
void MsgConversationBaseView::saveContentToDrafts()
    {
    bool result = false;
    if( mConversationId >= 0)
        {
        result = mConversationView->saveContentToDrafts();
        }
    
    if(result)
        {
        HbNotificationDialog::launchDialog(LOC_SAVED_TO_DRAFTS);
        }
    }

//---------------------------------------------------------------
// MsgConversationBaseView::clearContent
// clears conversation view content
//---------------------------------------------------------------
void MsgConversationBaseView::clearContent()
{
    ConversationsEngine::instance()->clearConversations();
    mConversationView->clearEditors();
}

//---------------------------------------------------------------
// MsgConversationBaseView::handleOk
//
//---------------------------------------------------------------
void MsgConversationBaseView::handleOk(const QVariant& result)
    {
    Q_UNUSED(result)
    }

//---------------------------------------------------------------
// MsgConversationBaseView::handleError
//
//---------------------------------------------------------------
void MsgConversationBaseView::handleError(int errorCode, const QString& errorMessage)
    {
    Q_UNUSED(errorMessage)
    Q_UNUSED(errorCode)
    }

//---------------------------------------------------------------
// MsgConversationBaseView::doDelayedConstruction
//
//---------------------------------------------------------------	
void MsgConversationBaseView::doDelayedConstruction()
{
    disconnect(this->mainWindow(),SIGNAL(viewReady()),this,SLOT(doDelayedConstruction()));
    ConversationsEngine::instance()->fetchMoreConversations();
}

//---------------------------------------------------------------
// MsgConversationBaseView::hideChrome
//
//---------------------------------------------------------------
void MsgConversationBaseView::hideChrome(bool hide)
    {
    if(hide)
        {        
        this->hideItems(Hb::StatusBarItem | Hb::TitleBarItem);
        this->setContentFullScreen(true);
        
        if(this->mainWindow()->orientation() == Qt::Horizontal)
            {
            mMainLayout->removeItem(mContactCard);
            mContactCard->hide();
            }
        }
    else
        {
        this->showItems(Hb::StatusBarItem | Hb::TitleBarItem);
        this->setContentFullScreen(false);
        
        if(!mContactCard->isVisible())
            {
            mMainLayout->insertItem(0,mContactCard);
            mContactCard->show();
            }
        }
    }

// EOF
