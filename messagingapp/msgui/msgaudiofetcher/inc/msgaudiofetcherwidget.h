/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *     The header file for messaging's audio fetcher widget.
 *
 */
#ifndef MSGAUDIOFETCHERWIDGET_H
#define MSGAUDIOFETCHERWIDGET_H

// SYSTEM INCLUDES
#include <HbWidget>

// USER INCLUDES
class HbListView;
class QGraphicsLinearLayout;
class MsgAudioFetcherView;
class MsgAudioFetcherModel;
class HbLabel;
class MsgAudioPreview;

/**
 * @class MsgAudioFetcherWidget
 * @brief This class is messaging's audio fetcher view's mainwidget
 */
class MsgAudioFetcherWidget : public HbWidget
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    explicit MsgAudioFetcherWidget();

    /**
     * Destructor
     */
    ~MsgAudioFetcherWidget();

    /**
     * Get the path of item selected in the list
     */
    QString getCurrentItemPath();

    /**
     * Play or pause the audio file
     */
    void playOrPause();

signals:
    /**
     * Signal to enable/disable the toolbar
     */
    void triggerToolBar(bool enable);

private slots:
    /**
     * Handles selection of a list item
     */
    void on_list_activated(const QModelIndex &index);
private:
    /**
     * initializes widget
     */
    void init();
    
private:
    /**
     * label of the list view
     * owned
     */
    HbLabel *mLabel;
    
    /**
     * list view
     * owned
     */
    HbListView *mListView;
    
    /**
     * list's layout
     * owned
     */
    QGraphicsLinearLayout *mLayout;
    
    /**
     * populating model
     * owned
     */
    MsgAudioFetcherModel *mFetcherModel;

    /**
     * For previewing audio files
     * owned
     */
    MsgAudioPreview* mAudioPreview;
    
    /**
     * selection state
     */
    bool mSelected;
    
    /**
     * last selected item
     */
    QModelIndex mOldSeletedItem;
};

#endif /* MSGAUDIOFETCHERWIDGET_H_ */
