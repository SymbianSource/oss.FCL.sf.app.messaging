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
#include <hbinstance.h>
#include <hbapplication.h>
#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbtoolbarextension.h>
#include <hblistview.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <hbnotificationdialog.h>
#include <hbgroupbox.h>
#include <hbmessagebox.h>
#include <hbframebackground.h>
#include <QProcess>
#include <QSortFilterProxyModel>
#include <QGraphicsLinearLayout>
//cp settings launcher.
#include <cppluginlauncher.h>
#include <hblistviewitem.h>
#include <HbStyleLoader>
#include <hbeffect.h>

// USER INCLUDES
#include "msgconversationviewinterface.h"
#include "debugtraces.h"
#include "conversationsengine.h"
#include "conversationsenginedefines.h"

#include "msglistviewitem.h"

//Icons
const QString POPUP_LIST_FRAME("qtg_fr_popup_list_normal");
const QString NEW_MESSAGE_ICON("qtg_mono_create_message");
const QString SORT_ICON("qtg_mono_sort");

//Localized constants
#define LOC_MENU_DELETE_CONVERSATION hbTrId("txt_messaging_menu_delete_conversation")
#define LOC_DIALOG_DELETE_CONVERSATION hbTrId("txt_messaging_dialog_delete_conversation")
#define LOC_POPINFO_CONVERSATION_DELETED hbTrId("txt_messaging_dpopinfo_conversation_deleted")

#define LOC_COMMON_OPEN hbTrId("txt_common_menu_open")
#define LOC_BUTTON_DELETE hbTrId("txt_common_button_delete")
#define LOC_BUTTON_CANCEL hbTrId("txt_common_button_cancel")

#define LOC_SETTINGS hbTrId("txt_messaging_opt_settings")

#define LOC_TB_VIEW_EXTN hbTrId("txt_messaging_button_view")
#define LOC_TB_NEW_MESSAGE hbTrId("txt_messaging_button_new_message")
#define LOC_TB_EXTN_DRAFTS hbTrId("txt_messaging_button_drafts")
#define LOC_TB_EXTN_CONVERSATIONS hbTrId("txt_messaging_button_conversations")

#define LOC_VIEW_HEADING hbTrId("txt_messaging_title_conversations")

//const int MSGSETTINGSPLUGIN_UID = 0x2001FE74;
//---------------------------------------------------------------
// MsgListView::MsgListView
// @see header
//---------------------------------------------------------------
MsgListView::MsgListView(QGraphicsItem *parent) :
    MsgBaseView(parent)
{
    // Create the MsgListView.
    createView();
}

//---------------------------------------------------------------
// MsgListView::~MsgListView
// @see header
//---------------------------------------------------------------
MsgListView::~MsgListView()
{
    mMsgList->setModel(NULL);
}

//---------------------------------------------------------------
// MsgListView::longPressed
// @see header
//---------------------------------------------------------------
void MsgListView::longPressed(HbAbstractViewItem* viewItem, const QPointF& point)
{
    if (this->isVisible()) {
        // Set the current index as the present Item's index.
        // By default it will not be set.
        mMsgList->setCurrentIndex(viewItem->modelIndex(), QItemSelectionModel::Select);

        // Create new menu
        HbMenu *contextMenu = new HbMenu();

        // Add the menu items
        HbAction *contextItem1 = contextMenu->addAction(LOC_COMMON_OPEN);
        connect(contextItem1, SIGNAL(triggered()), this, SLOT(
            openConversation()));

        //This Item specific option is not mentioned in the UI spec
        HbAction *contextItem2 = contextMenu->addAction(tr("Show contact info"));
        connect(contextItem2, SIGNAL(triggered()), this, SLOT(showContact()));

        qint64 conversationId = mMsgList->currentIndex().data(ConversationId).toLongLong();

        bool isNotSent = false; //TODO implement new api 
        //MessageModel::instance()->isMessageSendingOrScheduled(conversationId);
        // Display Delete option only if conversation does not contain 
        // any message which is yet to be sent
        if (isNotSent == false) {
            HbAction *contextItem3 = contextMenu->addAction(LOC_MENU_DELETE_CONVERSATION);
            connect(contextItem3, SIGNAL(triggered()), this, SLOT(deleteItem()));
        }

        contextMenu->exec(point);

        // Cleanup
        delete contextMenu;
    }
}

//---------------------------------------------------------------
// MsgListView::openConversation
// @see header
//---------------------------------------------------------------
void MsgListView::openConversation(const QModelIndex& index)
{
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
// MsgListView::sortBySubject
// @see header
//---------------------------------------------------------------
void MsgListView::sortBySubject()
{
    //	mProxyModel->setSortRole(LatestMsg);
    //	mProxyModel->sort(0, Qt::AscendingOrder);
}

//---------------------------------------------------------------
// MsgListView::sortByDate
// @see header
//---------------------------------------------------------------
void MsgListView::sortByDate()
{
    //	mProxyModel->setSortRole(LatestMsgTimeStamp);
    //	mProxyModel->sort(0, Qt::DescendingOrder);
}

//---------------------------------------------------------------
// MsgListView::sortBySender
// @see header
//---------------------------------------------------------------
void MsgListView::sortBySender()
{
    //	mProxyModel->setSortRole(ContactName);
    //	mProxyModel->sort(0, Qt::AscendingOrder);
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
// MsgListView::closeActiveClats
// @see header
//---------------------------------------------------------------
void MsgListView::closeActiveChats()
{
    // Implementation for closing all active chats
}

//---------------------------------------------------------------
// MsgListView::deleteAll
// @see header
//---------------------------------------------------------------
void MsgListView::deleteAll()
{
}

//---------------------------------------------------------------
// MsgListView::viewImMessagesSeperately
// @see header
//---------------------------------------------------------------
void MsgListView::viewImMessagesSeparately()
{
    //Lists all the Im messages separately
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

    QModelIndex index = mMsgList->currentIndex();
    qint64 conversationId = 0;
    conversationId = index.data(ConversationId).toLongLong();

    bool result = HbMessageBox::question(LOC_DIALOG_DELETE_CONVERSATION,
        LOC_BUTTON_DELETE, LOC_BUTTON_CANCEL);
    if (result) {
        ConversationsEngine::instance()->deleteConversations(conversationId);
        HbNotificationDialog::launchDialog(LOC_POPINFO_CONVERSATION_DELETED);
    }
#ifdef _DEBUG_TRACES_	
    qDebug() << " Leaving MsgConversationView::deleteItem";
#endif
}

//---------------------------------------------------------------
// MsgListView::createView
// @see header
//---------------------------------------------------------------
void MsgListView::createView()
{
    setupMenu();
    setupToolBar();
    setupListView();
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
    viewHeading->setTitleText(LOC_VIEW_HEADING);

    // Register the custorm css path.
    HbStyleLoader::registerFilePath(":/msglistviewitem.css");
    HbStyleLoader::registerFilePath(":/msglistviewitem.widgetml");

    mMsgList = new HbListView(this);

    HbEffect::add(QString("ListView"), QString(":/slide_in_from_screen_bottom.fxml"), QString(
        "show"));

    mMsgList->setLayoutName("custom");
    mMsgList->setItemRecycling(false);
    mMsgList->setUniformItemSizes(true);
   
    mMsgList->setScrollingStyle(HbScrollArea::PanOrFlick);
    MsgListViewItem *prototype = new MsgListViewItem(this);
    mMsgList->setItemPrototype(prototype);

    // Set proxy model
    mProxyModel = new QSortFilterProxyModel(this);
    mProxyModel->setDynamicSortFilter(true);
    mProxyModel->setSourceModel(ConversationsEngine::instance()->getConversationsSummaryModel());
    mProxyModel->setSortRole(TimeStamp);
    mProxyModel->sort(0, Qt::DescendingOrder);

    mMsgList->setModel(mProxyModel);

    connect(ConversationsEngine::instance(), SIGNAL(conversationListModelPopulated()), this,
        SLOT(populateListView()));

    // Single tap list item
    connect(mMsgList, SIGNAL(activated(const QModelIndex&)), this,
        SLOT(openConversation(const QModelIndex&)));

    // Long tap list item
    connect(mMsgList, SIGNAL(longPressed(HbAbstractViewItem*, const QPointF&)), this,
        SLOT(longPressed(HbAbstractViewItem*, const QPointF&)));

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
    // Get the menu pointer for the view

    HbMenu* mainMenu = this->menu();

    // Delete (This option is not mentioned in the UI spec 1.04)
    HbAction* deleteItem = mainMenu->addAction(tr("Delete"));
    connect(deleteItem, SIGNAL(triggered()), this, SLOT(deleteAll()));

    // Settings
    HbAction* settings = mainMenu->addAction(LOC_SETTINGS);
    connect(settings, SIGNAL(triggered()), this, SLOT(settings()));
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
    viewAction->setText(LOC_TB_VIEW_EXTN);
    viewAction->setIcon(HbIcon(SORT_ICON));

    mViewExtnList = new HbListWidget();
    mViewExtnList->addItem(LOC_TB_EXTN_DRAFTS);
    mViewExtnList->addItem(LOC_TB_EXTN_CONVERSATIONS);

    HbListViewItem *prototype = mViewExtnList->listItemPrototype();
    HbFrameBackground frame(POPUP_LIST_FRAME, HbFrameDrawer::NinePieces);
    prototype->setDefaultFrame(frame);

    connect(mViewExtnList, SIGNAL(activated(HbListWidgetItem*)), this,
        SLOT(handleViewExtnActivated(HbListWidgetItem*)));
    connect(mViewExtnList, SIGNAL(activated(HbListWidgetItem*)), viewExtn, SLOT(close()));

    viewExtn->setContentWidget(mViewExtnList);

    // Create & setup 2nd ToolBar button.
    HbAction* newMessageAction = new HbAction(HbIcon(NEW_MESSAGE_ICON), LOC_TB_NEW_MESSAGE, this);
    connect(newMessageAction, SIGNAL(triggered()), this, SLOT(sendNewMessage()));
    toolBar->addAction(newMessageAction);
}

//---------------------------------------------------------------
// MsgListView::activateView
// @see header
//---------------------------------------------------------------
void MsgListView::activateView()
{
    HbEffect::start(mMsgList, QString("ListView"), QString("show"), this, NULL);
}

//---------------------------------------------------------------
// MsgListView::populateListView
// @see header
//---------------------------------------------------------------
void MsgListView::populateListView()
{

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

//EOF
