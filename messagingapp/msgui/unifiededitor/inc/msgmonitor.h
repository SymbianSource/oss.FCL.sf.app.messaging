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
 * Description: Helper class to monitor msg construction in unified editor
 *
 */

#ifndef MSG_MONITOR_H
#define MSG_MONITOR_H

#include <QObject>
#include "convergedmessage.h"
class MsgUnifiedEditorView;
class HbWidget;
class UniEditorGenUtils;

/**
 * Helper class to monitor message's contruction in unified editor.
 * This class is for only reading editor's content to run its logic.
 * This class is NOT a place for content editing inside editor.
 */
class MsgMonitor : public QObject
    {
    Q_OBJECT

public:
    /**
     * Constructor
     */
    MsgMonitor(QObject* parent=0);

    /**
     * Destructor
     */
    ~MsgMonitor();

    /**
     * Set to skip showing note for first time
     * @param skip true/false
     */
    inline void setSkipNote(bool skip = true);
    
    /**
     * Seeker method to access current message type
     */
    static inline ConvergedMessage::MessageType messageType();

    /**
     * seeker function to get current msg size in bytes
     */
    static inline int messageSize();

    /**
     * seeker function to get current msg's body size 
     * in bytes
     */
    static inline int bodySize();

    /**
     * seeker function to get current msg's attachment
     * container size in bytes
     */
    static inline int containerSize();

    /**
     * seeker function to get current msg's subject
     * size in bytes
     */
    static inline int subjectSize();

    /**
     * seeker function to get max possible mms size
     * in bytes
     */
    static inline int maxMmsSize();
    
    /**
     * seeker funtion to get max recipient count for sms
     */
    static inline int maxSmsRecipients();

    /**
     * seeker funtion to get max recipient count for mms
     */
    static inline int maxMmsRecipients();

public slots:
    /**
     * slot to find any msg type change during editor operations
     */
    void checkMsgTypeChange();

private:
    /**
     * setter method to initialize content
     */
    void init();

    /**
     * handle size change of editor components
     */
    void updateSizeInfo(HbWidget* aWidget);
    
    /**
     * show type change discreet note
     */
    void showPopup(const QString& text);
    
    /**
     * accessor for view
     */
    MsgUnifiedEditorView* view();
        
private:
    /**
     * Flag to skip showing the type change popup.
     * Note need to be skipped when an mms is opened from draft.
     */
    bool mSkipNote;

    /**
     * Type of msg under composition in the editor 
     */
    static ConvergedMessage::MessageType mMessageType;
    
    /**
     * msg body size in bytes
     */
    static int mBodySize;
    
    /**
     * msg container size in bytes
     */
    static int mContainerSize;
    
    /**
     * msg subject size in bytes
     */
    static int mSubjectSize;

    /**
     * max mms size in bytes
     */
    static int mMaxMmsSize;
    
    /**
     * max recipient count in an sms
     */
    static int mMaxSmsRecipients;

    /**
     * max recipient count in an sms
     */
    static int mMaxMmsRecipients;

    /**
     * UniEditorGenUtils object
     * Owned
     */
    UniEditorGenUtils* mUniEditorGenUtils;
    };

#include "msgmonitor.inl"

#endif //MSG_MONITOR_H
