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

#include "msgeditorwidget.h"

// SYSTEM INCLUDES
#include <HbStyle>
#include <HbPushButton>
#include <qgraphicsscene.h>
#include "debugtraces.h"

// LOCAL CONSTANTS
const QString PLUGINPATH("conversationviewplugin.dll");

const QString SMS_SERVICE("messaging.mserver.sms");
const QString SEND_BUTTON(QObject::tr("Send"));

const QString SEND_ICON(":/icons/qtg_mono_send.png");

QGraphicsItem* FOCUSITEM = 0;

//---------------------------------------------------------------
// MsgEditorWidget::MsgEditorWidget
// @see header
//---------------------------------------------------------------
MsgEditorWidget::MsgEditorWidget(QGraphicsItem *parent) :
    HbWidget(parent), mServiceId(SMS_SERVICE), mMsgEditor(NULL),
    mSendButton(NULL)
{
    int baseId = style()->registerPlugin(PLUGINPATH);
    
    #ifdef _DEBUG_TRACES_
    qDebug() << "MsgEditorWidget BASE ID --->" << baseId;
    #endif
    
    setPluginBaseId(baseId);

    // Initialize the widget.
    init();
}

//---------------------------------------------------------------
// MsgEditorWidget::init
// @see header
//---------------------------------------------------------------
void MsgEditorWidget::init()
{
    // Create mandatory element of mesh layout.
    mMsgEditor = new MsgEditor(this);
    mMsgEditor->setMaxRows(5); // NOTE: Don't remove this line.
    HbStyle::setItemName(mMsgEditor, "msgEditor");

    mSendButton = new HbPushButton(this);
    HbStyle::setItemName(mSendButton, "sendButton");
    mSendButton->setIcon(HbIcon(SEND_ICON));
    mSendButton->setEnabled(false);
    FOCUSITEM = mSendButton;

    connect(mSendButton, SIGNAL(clicked()),this, SLOT(onClicked()));
    connect(mMsgEditor, SIGNAL(replyStarted()),this, SIGNAL(replyStarted()));
    connect(mMsgEditor, SIGNAL(textChanged(QString)),this,SLOT(onTextChanged(QString)));
    
}

//---------------------------------------------------------------
// MsgEditorWidget::~MsgEditorWidget
// @see header
//---------------------------------------------------------------
MsgEditorWidget::~MsgEditorWidget()
{
    style()->unregisterPlugin(PLUGINPATH);
}

//---------------------------------------------------------------
// MsgEditorWidget::serviceId
// @see header
//---------------------------------------------------------------
const QString& MsgEditorWidget::serviceId()
{
    return mServiceId;
}

//---------------------------------------------------------------
// MsgEditorWidget::content
// @see header
//---------------------------------------------------------------
QString MsgEditorWidget::content() const
{
    return mMsgEditor->text();
}

//---------------------------------------------------------------
// MsgEditorWidget::setContent
// @see header
//---------------------------------------------------------------
void MsgEditorWidget::setContent(const QString &text)
{
    mMsgEditor->setText(text);
}

//---------------------------------------------------------------
// MsgEditorWidget::clear
// @see header
//---------------------------------------------------------------
void MsgEditorWidget::clear()
{
    mMsgEditor->setText(QString());
    mMsgEditor->setCursorVisibility(Hb::TextCursorHidden);
}

//---------------------------------------------------------------
// MsgEditorWidget::clear
// @see header
//---------------------------------------------------------------
void MsgEditorWidget::onTextChanged(const QString& str)
{
    if(str.length() > 0 )
        {
        if(!mSendButton->isEnabled())
            {
            mSendButton->setFocusProxy(mMsgEditor);
            mSendButton->setEnabled(true);
            }
        }
    else
        {
        if(mSendButton->isEnabled())
            {
            mSendButton->setFocusProxy(0);
            mSendButton->setEnabled(false);
            }
        }
}

//---------------------------------------------------------------
// MsgEditor::onClicked
// @see header
//---------------------------------------------------------------
void MsgEditorWidget::onClicked()
    {    
    mSendButton->setFocusProxy(0);
    mMsgEditor->setFocusProxy(mSendButton);
    
    this->scene()->clearFocus();
    this->scene()->setFocusItem(mSendButton);
    
    mMsgEditor->setFocusProxy(0);

    mMsgEditor->setCursorVisibility(Hb::TextCursorHidden);
    
    emit sendMessage();
    }


//---------------------------------------------------------------
// MsgEditor::MsgEditor
// @see header
//---------------------------------------------------------------
MsgEditor::MsgEditor(QGraphicsItem *parent)
:HbLineEdit(parent)
    {
    
    }

//---------------------------------------------------------------
// MsgEditor::focusInEvent
// @see header
//---------------------------------------------------------------
void MsgEditor::focusInEvent(QFocusEvent *event)
 {
    if(event->reason() == Qt::MouseFocusReason)
    {
        HbLineEdit::focusInEvent(event);
        FOCUSITEM->setFocusProxy(this);
        setCursorVisibility(Hb::TextCursorVisible);
        emit replyStarted();
    }
    else
    {
        setCursorVisibility(Hb::TextCursorHidden);
    }
 }

//---------------------------------------------------------------
// MsgEditor::focusOutEvent
// @see header
//---------------------------------------------------------------
void MsgEditor::focusOutEvent(QFocusEvent * event)
    {
    FOCUSITEM->setFocusProxy(0);
    setCursorVisibility(Hb::TextCursorHidden);
    HbLineEdit::focusOutEvent(event);  
    }

// EOF
