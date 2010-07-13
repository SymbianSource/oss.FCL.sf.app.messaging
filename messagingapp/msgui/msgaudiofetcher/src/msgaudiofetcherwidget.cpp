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
 *     The source file for messaging's audio fetcher widget.
 *
 */

// SYSTEM INCLUDES
#include <HbListView>
#include <QModelIndex>
#include <QGraphicsLinearLayout>
#include <QStandardItemModel>
#include <HbLabel>

// USER INCLUDES
#include "msgaudiofetcherwidget.h"
#include "msgaudiofetchermodel.h"
#include "msgaudiopreview.h"

// LOCALIZED CONSTANTS
#define LOC_SELECT_SOUND  hbTrId("txt_messaging_title_select_a_sound")


MsgAudioFetcherWidget::MsgAudioFetcherWidget()
    : HbWidget(NULL),
      mLabel(0),
      mListView(0),
      mLayout(0),
      mFetcherModel(0),
      mAudioPreview(0),
      mSelected(false)
{
    init();
}

MsgAudioFetcherWidget::~MsgAudioFetcherWidget()
{
}

QString MsgAudioFetcherWidget::getCurrentItemPath()
{
    QModelIndexList modelIndexList = mListView->selectionModel()->selectedIndexes();
    if(modelIndexList.count() > 0)
    {
        QModelIndex index = modelIndexList.at(0);
        QString temppath = 
                mFetcherModel->data(index, Qt::UserRole).toString();
        return QDir::toNativeSeparators(temppath);
    }
    return QString();
}

void MsgAudioFetcherWidget::playOrPause()
{
    mAudioPreview->SetAttr(getCurrentItemPath());
    mAudioPreview->Play();
}

void MsgAudioFetcherWidget::on_list_activated(const QModelIndex &index)
{
    QModelIndexList modelIndexList = mListView->selectionModel()->selectedIndexes();

    //stop previewing when clicking another item.
    if (mAudioPreview->IsPlaying()) {
        mAudioPreview->SetAttr(getCurrentItemPath());
            mAudioPreview->Play();
        
    }

    /*
     * when one item is selected, reselecting it will deselect it. selecting another
     * will also deselect it, while the other is selected.
     */
    if(mSelected)
    {
        if(mOldSeletedItem != index)
        {
            mListView->selectionModel()->select(index, QItemSelectionModel::Select);
            mOldSeletedItem = index;
            emit triggerToolBar(true);
        }
        else
        {
            mListView->selectionModel()->select(index, QItemSelectionModel::Deselect);
            mSelected = false;
            emit triggerToolBar(false);
        }
        return;
    }

    if(modelIndexList.count() > 0)
    {
		QModelIndexList::const_iterator it;
        for(it=modelIndexList.begin(); it!=modelIndexList.end(); ++it)
        {
            if((*it) == index)
            {
                mSelected = true;
                mOldSeletedItem = index;
                emit triggerToolBar(true);
            }
        }
    }
}

void MsgAudioFetcherWidget::init()
{
    mLayout = new QGraphicsLinearLayout(this);
    mLayout->setOrientation(Qt::Vertical);
    setLayout(mLayout);

    mLabel = new HbLabel(this);
    // TODO: need localized string
    mLabel->setPlainText(LOC_SELECT_SOUND);
    mLayout->addItem(mLabel);

    mListView = new HbListView(this);
    mListView->setObjectName("list");
    mLayout->addItem(mListView);
    mListView->setSelectionMode(HbAbstractItemView::SingleSelection);

    mFetcherModel = new MsgAudioFetcherModel(this);
    mListView->setModel(mFetcherModel);
    connect(mListView, SIGNAL(activated(QModelIndex)),
        this, SLOT(on_list_activated(QModelIndex)));
    mAudioPreview = new MsgAudioPreview(this);
}

//End of File
