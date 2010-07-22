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
 *     The header file for messaging's audio fetcher view.
 *
 */

#ifndef MSGAUDIOFETCHERVIEW_H
#define MSGAUDIOFETCHERVIEW_H

#ifdef BUILD_MSGAUDIOFETCHER_DLL
#define MSGAUDIOFETCHER_DLL_EXPORT Q_DECL_EXPORT
#else
#define MSGAUDIOFETCHER_DLL_EXPORT Q_DECL_IMPORT
#endif

// SYSTEM INCLUDES
#include <QObject>
#include <hblistwidget.h>

// USER INCLUDES
#include "msgbaseview.h"
#include "convergedmessage.h"

// FORWARD DECLARATIONS
class MsgAudioFetcherWidget;
class HbAction;

/**
 * @class MsgAudioFetcherView
 * @brief This class is messaging's audio fetcher view
 */
class MSGAUDIOFETCHER_DLL_EXPORT MsgAudioFetcherView : public MsgBaseView
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    explicit MsgAudioFetcherView(const QVariantList& data);

    /**
     * Destructor
     */
    ~MsgAudioFetcherView();
private:
    /**
     * Creates a widget and sets it to the view
     */
    void initMainWidget();

    /**
     * Initializes view's toolbar actions
     */
    void initToolBar();

    /**
     * tool bar actions must to be removed in case the app crashes.
     */
    void removeToolBarAction();

private slots:
    /**
     * Handles toolbar's leftaction (play/pause)
     */
    void on_leftAction_triggered();

    /**
     * Handles toolbar's rightaction (select)
     */
    void on_rightAction_triggered();

    /**
     * enable toolbar actions, only when an item is selected
     */
    void enableToolBar(bool enable);

private:
    /**
     * view's main widget
     */
    MsgAudioFetcherWidget *mWidget;
    
    /**
     * Toolbar's left action
     */
    HbAction *mToolBarLeftAction;

    /**
     * Toolbar's right action
     */
    HbAction *mToolBarRightAction;
    
    /**
     * Converged Message object
     * owned
     */
    ConvergedMessage* message;
};

#endif /* MSGAUDIOFETCHERVIEW_H */
