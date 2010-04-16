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
#include "msguiutilsmanager.h"

// SYSTEM INCLUDES
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>
#include <QGraphicsLinearLayout>

// USER INCLUDES
#include "msgconversationview.h"
//#include "convergedmessage.h"
#include "conversationsengine.h"
#include "msgviewdefines.h"
#include "conversationsenginedefines.h"
#include "msgcontactcardwidget.h"

//---------------------------------------------------------------
// MsgConversationBaseView::MsgConversationBaseView
// Constructor
//---------------------------------------------------------------
MsgConversationBaseView::MsgConversationBaseView(QGraphicsItem* parent) :
MsgBaseView(parent),
mConversationView(NULL),
mConversationId(-1)
{   
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
    mConversationView->refreshView();
}

//---------------------------------------------------------------
// MsgConversationBaseView::initView
// create and initialise the conversationview
//---------------------------------------------------------------
void MsgConversationBaseView::initView()
    {
    // Create header widget
    MsgContactCardWidget *contactCardWidget = new MsgContactCardWidget();

    connect(contactCardWidget, SIGNAL(clicked()), this, SLOT(openContactDetails()));

    QGraphicsLinearLayout *mainLayout = new QGraphicsLinearLayout(Qt::Vertical);

    mainLayout->setContentsMargins(CONTENT_MARGIN, CONTENT_MARGIN,
                                   CONTENT_MARGIN, CONTENT_MARGIN);

    mainLayout->setSpacing(CONTENT_SPACING);

    mainLayout->addItem(contactCardWidget);

    /**
     * Create conversation view and connect to proper signals.
     * NOTE: contactCardWidget is NOT parent of MsgConversationView.
     * Just passing reference to MsgConversationView.
     */
    mConversationView = new MsgConversationView(contactCardWidget);

    connect(mConversationView, SIGNAL(closeConversationView()),
            this, SLOT(closeConversationView()));

    connect(mConversationView,SIGNAL(replyStarted()),
            this,SLOT(markMessagesAsRead()));

    connect(mConversationView, SIGNAL(switchView(const QVariantList&)),
            this, SIGNAL(switchView(const QVariantList&)));

    this->setMenu(mConversationView->menu());

    mainLayout->addItem(mConversationView);

    this->setLayout(mainLayout);

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
bool MsgConversationBaseView::saveContentToDrafts()
    {
    bool result = false;
    if( mConversationId >= 0)
        {
        result = mConversationView->saveContentToDrafts();
        }
    return result;
    }

//---------------------------------------------------------------
// MsgConversationBaseView::openContactDetails
// close conversation view
//---------------------------------------------------------------
void MsgConversationBaseView::openContactDetails()
{
    MsgUiUtilsManager uiUtilsManager(this);
    QModelIndex aIndex = ConversationsEngine::instance()->getConversationsModel()->index(0, 0);
    qint32 contactId = aIndex.data(ContactId).toLongLong();
    uiUtilsManager.openContactDetails(contactId);
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
// EOF
