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
 * Description:Message List View for the messaging application.
 *
 */

#include "msglistview.h"

// SYSTEM INCLUDES
#include <hbmenu.h>
#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbtoolbarextension.h>
#include <hblistview.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <hbgroupbox.h>
#include <hbmessagebox.h>
#include <hbframebackground.h>
#include <QSortFilterProxyModel>
#include <QGraphicsLinearLayout>
#include <hblistviewitem.h>
#include <HbStyleLoader>
#include <HbMainWindow>
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#include <qtcontacts.h>

// USER INCLUDES
#include "msgconversationviewinterface.h"
#include "debugtraces.h"
#include "conversationsengine.h"
#include "conversationsenginedefines.h"
#include "convergedmessage.h"
#include "msglistviewitem.h"

QTM_USE_NAMESPACE

//Icons
const QString POPUP_LIST_FRAME("qtg_fr_popup_list_normal");
const QString NEW_MESSAGE_ICON("qtg_mono_create_message");
const QString SORT_ICON("qtg_mono_sort");

//Localized constants

#define LOC_DIALOG_DELETE_CONVERSATION hbTrId("txt_messaging_dialog_delete_conversation")

//itemspecific menu
#define LOC_OPEN hbTrId("txt_common_menu_open")
#define LOC_SAVETO_CONTACTS hbTrId("txt_messaging_menu_save_to_contacts")
#define LOC_DELETE_CONVERSATION hbTrId("txt_messaging_menu_delete_conversation")
#define LOC_OPEN_CONTACT_INFO hbTrId("txt_messaging_menu_open_contact_info")

#define LOC_BUTTON_DELETE hbTrId("txt_common_button_delete")
#define LOC_BUTTON_CANCEL hbTrId("txt_common_button_cancel")

//main menu
#define LOC_SETTINGS    hbTrId("txt_messaging_opt_settings")

#define LOC_TB_VIEW_EXTN hbTrId("txt_messaging_button_view")
#define LOC_TB_NEW_MESSAGE hbTrId("txt_messaging_button_new_message")
#define LOC_TB_EXTN_DRAFTS hbTrId("txt_messaging_button_drafts")
#define LOC_TB_EXTN_CONVERSATIONS hbTrId("txt_messaging_button_conversations")

#define LOC_VIEW_HEADING hbTrId("txt_messaging_title_conversations")

//---------------------------------------------------------------
// MsgListView::MsgListView
// @see header
//---------------------------------------------------------------
MsgListView::MsgListView(QGraphicsItem *parent) :
    MsgBaseView(parent),
    mItemLongPressed(false)
{
    connect(this->mainWindow(),SIGNAL(viewReady()),this,SLOT(doDelayedConstruction()));
}

//---------------------------------------------------------------
// MsgListView::~MsgListView
// @see header
//---------------------------------------------------------------
MsgListView::~MsgListView()
{
    
}

//---------------------------------------------------------------
// MsgListView::longPressed
// @see header
//---------------------------------------------------------------
void MsgListView::longPressed(HbAbstractViewItem* viewItem, const QPointF& point)
{
    mItemLongPressed = true;
    if (this->isVisible()) {
        // Set the current index as the present Item's index.
        // By default it will not be set.
        mMsgList->setCurrentIndex(viewItem->modelIndex(), QItemSelectionModel::Select);

        // Create new menu
        HbMenu *contextMenu = new HbMenu();
        contextMenu->setAttribute(Qt::WA_DeleteOnClose);
        //open menu option
        contextMenu->addAction(LOC_OPEN,this,SLOT(openConversation()));
        
        //save to contacts for unresolved.
        int msgType = viewItem->modelIndex().data(MessageType).toInt();
        if(msgType == ConvergedMessage::Sms || 
           msgType == ConvergedMessage::Mms || 
           msgType == ConvergedMessage::BioMsg)
            {
            qint64 contactId = mMsgList->currentIndex().data(ContactId).toLongLong();
            if(contactId < 0)
                {
                contextMenu->addAction(LOC_SAVETO_CONTACTS,this,SLOT(saveToContacts()));
                }
            else
                {
                contextMenu->addAction(LOC_OPEN_CONTACT_INFO,this,SLOT(contactInfo()));
                }
            }
        
        //delete conversation
        contextMenu->addAction(LOC_DELETE_CONVERSATION,this,SLOT(deleteItem()));

        contextMenu->setPreferredPos(point);    
        contextMenu->show();
    }
}

//---------------------------------------------------------------
// MsgListView::openConversation
// @see header
//---------------------------------------------------------------
void MsgListView::openConversation(const QModelIndex& index)
{
    if(mItemLongPressed)
     {
     mItemLongPressed = false;
     return;
     }
    //TODO: model populating possibilities.
    if (index.isValid()) {
        QVariant conversationId = index.data(ConversationId);

        // Launch conversation view
        QVariantList param;
        param << MsgBaseView::CV; // target view
        param << MsgBaseView::CLV; // source view

        param << conversationId;

        emit switchView(param);
    }
}

//---------------------------------------------------------------
// MsgListView::sendNewMessage
// @see header
//---------------------------------------------------------------
void MsgListView::sendNewMessage()
{
    QVariantList param;
    param << MsgBaseView::UNIEDITOR; // target view
    param << MsgBaseView::CLV; // source view

    emit switchView(param);
}

//---------------------------------------------------------------
// MsgListView::settings
// @see header
//---------------------------------------------------------------
void MsgListView::settings()
{
    // Launch message settings
    QVariantList param;
    param << MsgBaseView::MSGSETTINGS;
    param << MsgBaseView::CLV;
    emit switchView(param);
}

//---------------------------------------------------------------
// MsgListView::openConversation
// @see header
//---------------------------------------------------------------
void MsgListView::openConversation()
{
    QModelIndex modelIndex = mMsgList->currentIndex();
    openConversation(modelIndex);
}

//---------------------------------------------------------------
// MsgListView::showContact
// @see header
//---------------------------------------------------------------
void MsgListView::showContact()
{
}

//---------------------------------------------------------------
// MsgListView::deleteItem
// @see header
//---------------------------------------------------------------
void MsgListView::deleteItem()
{
#ifdef _DEBUG_TRACES_
    qDebug() << "Inside MsgListView::deleteItem";
#endif
    //confirmation dialog.
    HbMessageBox::question(LOC_DIALOG_DELETE_CONVERSATION,
                                this,SLOT(onDialogDeleteMsg(HbAction*)),
                                LOC_BUTTON_DELETE, LOC_BUTTON_CANCEL);    
#ifdef _DEBUG_TRACES_	
    qDebug() << " Leaving MsgConversationView::deleteItem";
#endif
}

//---------------------------------------------------------------
// MsgListView::setupView
// @see header
//---------------------------------------------------------------
void MsgListView::setupListView()
{
    // Create parent layout.
    QGraphicsLinearLayout *mainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Create view heading.
    HbGroupBox *viewHeading = new HbGroupBox();
    viewHeading->setHeading(LOC_VIEW_HEADING);

    // Register the custorm css path.
    HbStyleLoader::registerFilePath(":/clv");

    mMsgList = new HbListView(this);
    mMsgList->setScrollingStyle(HbScrollArea::PanOrFlick);
    mMsgList->setClampingStyle(HbScrollArea::BounceBackClamping);

    mMsgList->setLayoutName("custom");
    mMsgList->setItemRecycling(true);
    mMsgList->setUniformItemSizes(true);   

    MsgListViewItem *prototype = new MsgListViewItem(this);
    mMsgList->setItemPrototype(prototype);

    // Set proxy model
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSourceModel(ConversationsEngine::instance()->getConversationsSummaryModel());
    proxyModel->setSortRole(TimeStamp);
    proxyModel->sort(0, Qt::DescendingOrder);

    mMsgList->setModel(proxyModel);

    connect(ConversationsEngine::instance(), SIGNAL(conversationListModelPopulated()),
            this, SLOT(populateListView()));

    // Single tap list item
    connect(mMsgList, SIGNAL(activated(const QModelIndex&)),
            this, SLOT(openConversation(const QModelIndex&)));

    // Long tap list item
    connect(mMsgList, SIGNAL(longPressed(HbAbstractViewItem*, const QPointF&)),
            this, SLOT(longPressed(HbAbstractViewItem*, const QPointF&)));

    // Add all widgets to main layout.
    mainLayout->addItem(viewHeading);
    mainLayout->addItem(mMsgList);

    this->setLayout(mainLayout);
}

//---------------------------------------------------------------
// MsgListView::addMenu
// @see header
//---------------------------------------------------------------
void MsgListView::setupMenu()
{
    // Main menu
    HbMenu* mainMenu = this->menu();
    mainMenu->addAction(LOC_SETTINGS,this,SLOT(settings()));
}

//---------------------------------------------------------------
// MsgListView::setupToolBar
// @see header
//---------------------------------------------------------------
void MsgListView::setupToolBar()
{
    // ToolBar
    HbToolBar *toolBar = this->toolBar();
    toolBar->setOrientation(Qt::Horizontal);

    // Create & setup ToolBar Extension
    HbToolBarExtension *viewExtn = new HbToolBarExtension();
    HbAction *viewAction = toolBar->addExtension(viewExtn);
    viewAction->setIcon(HbIcon(SORT_ICON));

    mViewExtnList = new HbListWidget();
    mViewExtnList->addItem(LOC_TB_EXTN_DRAFTS);
    mViewExtnList->addItem(LOC_TB_EXTN_CONVERSATIONS);

    HbListViewItem *prototype = mViewExtnList->listItemPrototype();
    HbFrameBackground frame(POPUP_LIST_FRAME, HbFrameDrawer::NinePieces);
    prototype->setDefaultFrame(frame);

    connect(mViewExtnList, SIGNAL(activated(HbListWidgetItem*)),
            this,SLOT(handleViewExtnActivated(HbListWidgetItem*)));
    connect(mViewExtnList, SIGNAL(released(HbListWidgetItem*)), viewExtn, SLOT(close()));

    viewExtn->setContentWidget(mViewExtnList);
    
    // Create & setup 2nd ToolBar button.
    toolBar->addAction(HbIcon(NEW_MESSAGE_ICON),"",this,SLOT(sendNewMessage()));
}

//---------------------------------------------------------------
// MsgListView::handleViewExtnActivated
// @see header
//---------------------------------------------------------------
void MsgListView::handleViewExtnActivated(HbListWidgetItem *item)
{
    int row = mViewExtnList->row(item);
    if (DRAFTS_EXTN == row) {
        QVariantList param;
        param << MsgBaseView::DLV; // target view
        param << MsgBaseView::CLV; // source view

        emit switchView(param);
    }
}

//---------------------------------------------------------------
// MsgListView::doDelayedConstruction
// @see header
//---------------------------------------------------------------
void MsgListView::doDelayedConstruction()
    {
    setupToolBar();    
    setupListView();
    setupMenu();
    
    disconnect(this->mainWindow(),SIGNAL(viewReady()),this,SLOT(doDelayedConstruction()));
    }

//---------------------------------------------------------------
// MsgListView::saveToContacts
// @see header
//---------------------------------------------------------------
void MsgListView::saveToContacts()
    {
    //save to contacts with phone number field prefilled.
    QList<QVariant> args;  

    QString data = mMsgList->currentIndex().data(DisplayName).toString();

    QString type = QContactPhoneNumber::DefinitionName;

    args << type;
    args << data;

    //service stuff.
    QString serviceName("com.nokia.services.phonebookservices");
    QString operation("editCreateNew(QString,QString)");

    XQAiwRequest* request;
    XQApplicationManager appManager;
    request = appManager.create(serviceName, "Fetch", operation, true); // embedded
    if ( request == NULL )
        {
        return;       
        }

    request->setArguments(args);
    request->send();
    
    delete request;
    }

//---------------------------------------------------------------
// MsgListView::contactInfo
// @see header
//---------------------------------------------------------------
void MsgListView::contactInfo()
    {
    //open contact info.
    QList<QVariant> args;  

    int contactId = mMsgList->currentIndex().data(ContactId).toInt();

    args << contactId;

    //service stuff.
    QString serviceName("com.nokia.services.phonebookservices");
    QString operation("open(int)");

    XQAiwRequest* request;
    XQApplicationManager appManager;
    request = appManager.create(serviceName, "Fetch", operation, true); // embedded
    if ( request == NULL )
        {
        return;       
        }

    request->setArguments(args);
    request->send();
    
    delete request;
    }

//---------------------------------------------------------------
// MsgListView::onDialogDeleteMsg
// @see header
//---------------------------------------------------------------
void MsgListView::onDialogDeleteMsg(HbAction* action)
{
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(0)) {
        QModelIndex index = mMsgList->currentIndex();
        if(index.isValid())
        {
            qint64 conversationId = index.data(ConversationId).toLongLong();
                    ConversationsEngine::instance()->deleteConversations(conversationId);    
        }
        
    }
}

//EOF
