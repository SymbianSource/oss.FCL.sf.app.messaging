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

class HbTextItem;
class HbLineEdit;
class HbIconItem;
class UniEditorGenUtils;

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
     * Emitted when MMS content is added or removed
     */
    void mmContentAdded(bool isAdded);
    
    /**
     * Emitted when subject size changes
     */
    void sizeChanged(int aSize);
    
private slots:
    /**
     * called when textChanged signal is emitted by the line edit
     */
    void onTextChanged(const QString&);
    
    /**
     * Called when textChanged signal is emitted by the line edit
     * Checks for empty text content 
     */
    void onTextRemoved(const QString& text);
    
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
     * label to show sub: string.
     */
    HbTextItem *mSubjectLabel;
    
    /**
     * line edit to input subject.
     */
    HbLineEdit* mSubjectEdit;
    
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
