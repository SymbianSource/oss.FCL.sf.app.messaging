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
 * Description: Main view of unified viewer
 *
 */

#ifndef UNIFIED_VIEWER_H
#define UNIFIED_VIEWER_H

#ifdef BUILD_UNI_VIEWER_DLL
#define UNI_VIEWER_DLL Q_DECL_EXPORT
#else
#define UNI_VIEWER_DLL Q_DECL_IMPORT
#endif

#include "msgbaseview.h"

#include "convergedmessage.h"

class UniViewerFeeder;
class ConvergedMessage;
class HbScrollArea;
class UniScrollArea;
class UniContentsWidget;
class MessageDeleteAction;
class HbAction;
/**
 * Main view of unified viewer.
 */
class UNI_VIEWER_DLL UnifiedViewer : public MsgBaseView
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    UnifiedViewer(const qint32 messageId, 
                  int canForwardMessage = 0,
                  QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~UnifiedViewer();

    /**
     * Populates the content on the widget
     */
    void populateContent(const qint32 messageId, bool update, int msgCount);

protected:

    /**
     * reimplemented from base class to provide proper geometry for scrolling.
     * @see HbWidget
     */
    void resizeEvent(QGraphicsSceneResizeEvent * event);

private:

    /**
     * Creates tool bar actions
     * @see HbView
     */
    void createToolBar();
   

public slots:

    /**
     * Clears the content on the widget
     */
    void clearContent();

    // Used for internal testing will be removed
    void handleFwdAction();
    
    /**
     * Handle delete 
     */     
    void handleDeleteAction();
    
private slots:
    /**
     * This slot is called when sendMessage signal is emitted for a highlighted
     * phone number, from body widget.
     */
    void sendMessage(const QString& phoneNumber, const QString& alias = QString());

    /**
     * This slot is called when delete message dialog is launched.
     * @param action selected action (yes or no).
     */
    void onDialogDeleteMsg(HbAction* action);
    
private:

    /**
     * Feeder object
     * Owned
     */
    UniViewerFeeder *mViewFeeder;

    /**
     * HbScrollArea object
     * Owned
     */
    UniScrollArea* mScrollArea;

    /**
     * UniContentsWidget object
     * Owned
     */
    UniContentsWidget* mContentsWidget;

    /**
     * Contact id
     */
    qint32 mContactId;

    /**
     * Message id
     */
    qint32 mMessageId;
	
    /**
     * Message count
     */
    int mMsgCount;
    
	/**
	 * Forward status of message
	 */
    bool mForwardMessage;
};

#endif
// EOF
