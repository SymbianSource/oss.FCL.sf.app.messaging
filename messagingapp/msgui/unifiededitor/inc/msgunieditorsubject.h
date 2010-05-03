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
 */

#ifndef UNIFIED_EDITOR_SUBJECT_H
#define UNIFIED_EDITOR_SUBJECT_H

#include <hbwidget.h>

#include "convergedmessage.h"

class HbIconItem;
class UniEditorGenUtils;
class MsgUnifiedEditorLineEdit;

class MsgUnifiedEditorSubject : public HbWidget
    {
    Q_OBJECT

public:

    /**
     * Constructor
     */
    MsgUnifiedEditorSubject(const QString& pluginPath, QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~MsgUnifiedEditorSubject();

    /**
     * to set priority.
     * @param priority MsgPriority {High,Normal,Low}
     */
    void setPriority(ConvergedMessage::Priority priority);
    
    /**
     * returns content of subject field.
     */
    QString text();

    /**
     * return the priority of this message
     */
    ConvergedMessage::Priority priority();
    
    /**
     * get size of content in bytes
     */
    int subjectSize();

    /**
     * set the text on the subject field
     * @param text
     */
    void setText(const QString& text);

signals:
    /**
     * Emitted when subject/priority content is added or removed
     */
    void contentChanged();
    
private slots:
    /**
     * called when contentsChanged signal is emitted by the line edit
     */
    void onContentsAdded(const QString&);
    
    /**
     * Called when contentsChanged signal is emitted by the line edit
     * Checks for empty text content 
     */
    void onContentsRemoved(const QString& text);
    
private:
    /**
     * Check if email over sms is supported. If yes, subject is a valid sms field
     * @return true, if email over sms is supported.
     */
    bool subjectOkInSms();

private:
    
    /**
     * plug in path.
     */
    QString mPluginPath;
    
    /**
     * line edit to input subject.
     */
    MsgUnifiedEditorLineEdit* mSubjectEdit;
    
    /**
     * icon item to display priority.
     */
    HbIconItem* mPriorityIcon;

    /**
     * priority of the message
     */
    ConvergedMessage::Priority mPriority;
    
    /**
     * general utilities
     */
    UniEditorGenUtils* mGenUtils;
    };

#endif //UNIFIED_EDITOR_SUBJECT_H
