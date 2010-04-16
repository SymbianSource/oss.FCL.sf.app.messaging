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
 * Description: Drafts List View for displaying drafts messages.
 *
 */

#include "draftslistview.h"

// SYSTEM INCLUDES
#include <HbMenu>
#include <HbAction>
#include <HbToolBar>
#include <HbToolBarExtension>
#include <HbListView>
#include <HbListViewItem>
#include <HbListWidget>
#include <HbListWidgetItem>
#include <HbStyleLoader>
#include <HbGroupBox>
#include <HbFrameBackground>

#include <QSortFilterProxyModel>
#include <QGraphicsLinearLayout>

// USER INCLUDES
#include "conversationsengine.h"
#include "conversationsenginedefines.h"
#include "convergedmessage.h"
#include "convergedmessageid.h"

// LOCAL CONSTANTS
const QString LIST_ITEM_FRAME("qtg_fr_list_normal");
const QString POPUP_LIST_FRAME("qtg_fr_popup_list_normal");
const QString NEW_MESSAGE_ICON("qtg_mono_create_message");
const QString SORT_ICON("qtg_mono_sort");

// LOCALIZATION CONSTANTS
#define LOC_COMMON_OPEN hbTrId("txt_common_menu_open")
#define LOC_COMMON_DELETE hbTrId("txt_common_menu_delete")
#define VIEW_HEADING "Drafts"
#define MENU_DELETE_ALL "Delete all drafts"
#define TB_VIEW_EXTN "View"
#define TB_NEW_MESSAGE "New message"
#define TB_EXTN_DRAFTS "Drafts"
#define TB_EXTN_CONVERSATIONS "Conversations"

//---------------------------------------------------------------
// DraftsListView::DraftsListView
// @see header
//---------------------------------------------------------------
DraftsListView::DraftsListView(QGraphicsItem *parent) :
    MsgBaseView(parent), mListView(0), mViewExtnList(0)
{
    createView();
}

//---------------------------------------------------------------
// DraftsListView::~DraftsListView
// @see header
//---------------------------------------------------------------
DraftsListView::~DraftsListView()
{
}

//---------------------------------------------------------------
// DraftsListView::createView
// @see header
//---------------------------------------------------------------
void DraftsListView::createView()
{
    setupToolbar();
    setupMenu();
    setupListView();
}

//---------------------------------------------------------------
// DraftsListView::setupMenu
// @see header
//---------------------------------------------------------------
void DraftsListView::setupMenu()
{
}

//---------------------------------------------------------------
// DraftsListView::setupToolbar
// @see header
//---------------------------------------------------------------
void DraftsListView::setupToolbar()
{
    HbToolBar *toolBar = this->toolBar();
    toolBar->setOrientation(Qt::Horizontal);

    // Create & setup ToolBar Extension
    HbToolBarExtension *viewExtn = new HbToolBarExtension();
    HbAction *viewAction = toolBar->addExtension(viewExtn);
    viewAction->setText(TB_VIEW_EXTN);
    viewAction->setIcon(HbIcon(SORT_ICON));

    mViewExtnList = new HbListWidget();
    mViewExtnList->addItem(TB_EXTN_DRAFTS);
    mViewExtnList->addItem(TB_EXTN_CONVERSATIONS);

    HbListViewItem *prototype = mViewExtnList->listItemPrototype();
    HbFrameBackground frame(POPUP_LIST_FRAME, HbFrameDrawer::NinePieces);
    prototype->setDefaultFrame(frame);

    connect(mViewExtnList, SIGNAL(activated(HbListWidgetItem*)), this,
        SLOT(handleViewExtnActivated(HbListWidgetItem*)));
    connect(mViewExtnList, SIGNAL(activated(HbListWidgetItem*)), viewExtn, SLOT(close()));

    viewExtn->setContentWidget(mViewExtnList);

    // Create & setup 2nd ToolBar button.
    HbAction* newMessageAction = new HbAction(HbIcon(NEW_MESSAGE_ICON), TB_NEW_MESSAGE, this);
    connect(newMessageAction, SIGNAL(triggered()), this, SLOT(createNewMessage()));
    toolBar->addAction(newMessageAction);
}

//---------------------------------------------------------------
// DraftsListView::setupListView
// @see header
//---------------------------------------------------------------
void DraftsListView::setupListView()
{
    // Create parent layout.
    QGraphicsLinearLayout *mainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Create view heading.
    HbGroupBox *viewHeading = new HbGroupBox();
    viewHeading->setTitleText(VIEW_HEADING);

    // Create List View.
    mListView = new HbListView(this);
    mListView->setScrollingStyle(HbScrollArea::PanOrFlick);
    mListView->setItemRecycling(true);
    mListView->setUniformItemSizes(true);
    mListView->setClampingStyle(HbScrollArea::BounceBackClamping);

    // Register the custorm css path.
    HbStyleLoader::registerFilePath(":/hblistviewitem.css");
    //    mListView->setLayoutName("custom");

    // Set list item properties.
    HbListViewItem *prototype = mListView->listItemPrototype();
    prototype->setGraphicsSize(HbListViewItem::SmallIcon);
    prototype->setStretchingStyle(HbListViewItem::StretchLandscape);
    prototype->setSecondaryTextRowCount(1, 1);
    HbFrameBackground frame(LIST_ITEM_FRAME, HbFrameDrawer::NinePieces);
    prototype->setDefaultFrame(frame);

    // Create and set model
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSourceModel(ConversationsEngine::instance()->getDraftsModel());
    proxyModel->setSortRole(TimeStamp);
    proxyModel->sort(0, Qt::DescendingOrder);
    mListView->setModel(proxyModel);

    // Short & Long Taps
    connect(mListView, SIGNAL(activated(QModelIndex)), this, SLOT(openDraftMessage(QModelIndex)));
    connect(mListView, SIGNAL(longPressed(HbAbstractViewItem*,QPointF)), this,
        SLOT(handleLongPressed(HbAbstractViewItem*,QPointF)));

    // Add all widgets to main layout.
    mainLayout->addItem(viewHeading);
    mainLayout->addItem(mListView);

    this->setLayout(mainLayout);
}

//------------------------------------------------------------------------------
// DraftsListView::openDraftMessage
// @see header
//------------------------------------------------------------------------------
void DraftsListView::openDraftMessage()
{
    openDraftMessage(mListView->currentIndex());
}

//------------------------------------------------------------------------------
// DraftsListView::deleteDraftMessage
// @see header
//------------------------------------------------------------------------------
void DraftsListView::deleteDraftMessage()
{
    int msgId = mListView->currentIndex().data(ConvergedMsgId).toInt();
    QList<int> msgIdList;
    msgIdList.append(msgId);
    ConversationsEngine::instance()->deleteMessages(msgIdList);
}

//------------------------------------------------------------------------------
// DraftsListView::createNewMessage
// @see header
//------------------------------------------------------------------------------
void DraftsListView::createNewMessage()
{
    QVariantList param;
    param << MsgBaseView::UNIEDITOR; // target view
    param << MsgBaseView::DLV; // source view

    emit switchView(param);
}

//------------------------------------------------------------------------------
// DraftsListView::openDraftMessage
// @see header
//------------------------------------------------------------------------------
void DraftsListView::openDraftMessage(const QModelIndex &index)
{
    QVariant msgId = index.data(ConvergedMsgId);
    QVariant msgType = index.data(MessageType);
    ConvergedMessageId convergedMsgId = ConvergedMessageId(msgId.toInt());
    ConvergedMessage message;
    message.setMessageType((ConvergedMessage::MessageType) msgType.toInt());
    message.setMessageId(convergedMsgId);

    // Launch uni-editor view
    QByteArray dataArray;
    QDataStream messageStream(&dataArray, QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);

    QVariantList params;
    params << MsgBaseView::UNIEDITOR; // target view
    params << MsgBaseView::DLV; // source view

    params << dataArray;
    emit switchView(params);
}

//------------------------------------------------------------------------------
// DraftsListView::handleLongPressed
// @see header
//------------------------------------------------------------------------------
void DraftsListView::handleLongPressed(HbAbstractViewItem *item, const QPointF &coords)
{
    if (this->isVisible()) {

        // Set the current index as tapped items index.
        mListView->setCurrentIndex(item->modelIndex(), QItemSelectionModel::Select);

        HbMenu *contextMenu = new HbMenu();

        // Open
        HbAction* openAction = contextMenu->addAction(LOC_COMMON_OPEN);
        connect(openAction, SIGNAL(triggered()), this, SLOT(openDraftMessage()));

        // Delete
        HbAction *deletAction = contextMenu->addAction(LOC_COMMON_DELETE);
        connect(deletAction, SIGNAL(triggered()), this, SLOT(deleteDraftMessage()));

        contextMenu->exec(coords);
        delete contextMenu;
    }
}

//------------------------------------------------------------------------------
// DraftsListView::handleViewExtnActivated
// @see header
//------------------------------------------------------------------------------
void DraftsListView::handleViewExtnActivated(HbListWidgetItem *item)
{
    int row = mViewExtnList->row(item);
    if (CONVERSATIONS_EXTN == row) {
        QVariantList param;
        param << MsgBaseView::CLV; // target view
        param << MsgBaseView::DLV; // source view

        emit switchView(param);
    }
}

// EOF
