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
 *
 */

#ifndef MSGEDITORWIDGET_H
#define MSGEDITORWIDGET_H

// SYSTEM INCLUDES
#include <HbWidget>
#include <HbLineEdit>

// FORWARD DECLARATIONS
class HbPushButton;

/**
 * Custom editor class
 */
class MsgEditor : public HbLineEdit
{
Q_OBJECT
    
public:    
    /**
     * Constructor
     */
    MsgEditor(QGraphicsItem *parent = 0);
    
    /**
     * Foucus in Event method from  HbAbstractEdit
     */
    
protected:
    /**
     * reimplemented from base class.
     */
    void focusInEvent(QFocusEvent *event);    
    void focusOutEvent(QFocusEvent * event);
    
signals:  
    /**
     * Signal emitted when the focus in event comes
     * i.e. when editor is tapped for reply
     */ 
     void replyStarted();
};

/**
 * This class is a custom layout widget for Msg Editor.
 */

class MsgEditorWidget : public HbWidget
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    MsgEditorWidget(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    virtual ~MsgEditorWidget();

public:
    /* Setter/Getter methods */

    /**
     * Returns the editor's contents.
     * @return Editor contents
     */
    QString content() const;

    /**
     * Set the editor's content.
     * @param contentText content text to set.
     */
    void setContent(const QString &contentText);

    /**
     * Getter method which returns the current service-id.
     * @return service-id
     */
    const QString& serviceId();

signals:

    /**
     * Emitted when send button from virtual ITUT is pressed.
     */
    void sendMessage();

    /**
     * Signal emitted when the focus in event comes
     * i.e. when editor is tapped for reply
     */ 
    void replyStarted();
    
public slots:

    /**
     * Clears the content.
     */
    void clear();
    
private slots:

    /**
     * Called when editor content is chaged. to enable/disable send button.
     */
    void onTextChanged(const QString& str);
    
    /**
     * called when send button is clicked.
     */
    void onClicked();

private:

    /**
     * Initialization function.
     */
    void init();

private:

    /**
     * Current service-id
     */
    QString mServiceId;

    /**
     * Instance of message edit.
     * Will be deleted automatically by parent.
     * Own.
     */
    MsgEditor *mMsgEditor;

    /**
     * Instance of push button.
     * Will be deleted automatically by parent.
     * Own.
     */
    HbPushButton *mSendButton;
};

#endif // MSGEDITORWIDGET_H
// EOF
