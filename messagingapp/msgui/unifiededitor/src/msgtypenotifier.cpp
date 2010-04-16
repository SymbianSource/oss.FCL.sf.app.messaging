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

// INCLUDES
#include "debugtraces.h"
//#include <HbDeviceNotificationDialog>
#include <HbNotificationDialog>

// USER INCLUDES
#include "msgtypenotifier.h"

// Constants

//Localized strings
#define LOC_POP_MESSAGE_CHANGE_MUL hbTrId("txt_messaging_dpopinfo_message_type_changed_to_mul")
#define LOC_POP_MESSAGE_CHANGE_TEXT hbTrId("txt_messaging_dpopinfo_message_type_changed_to_tex")

//---------------------------------------------------------------
// MsgTypeNotifier::MsgTypeNotifier
// @see header file
//---------------------------------------------------------------
MsgTypeNotifier::MsgTypeNotifier(QObject* parent) :
QObject(parent),
mmContentCount(0),
mSkipNote(false)
{
    // do nothing
}

//---------------------------------------------------------------
// MsgTypeNotifier::~MsgTypeNotifier
// @see header file
//---------------------------------------------------------------
MsgTypeNotifier::~MsgTypeNotifier()
{
    // do nothing
}

//---------------------------------------------------------------
// MsgTypeNotifier::messageType
// @see header file
//---------------------------------------------------------------
ConvergedMessage::MessageType MsgTypeNotifier::messageType() const
{
    ConvergedMessage::MessageType msgtype = ConvergedMessage::None;
    if(mmContentCount)
    {
        msgtype = ConvergedMessage::Mms;
	}
	else
	{
        msgtype = ConvergedMessage::Sms;
	}

    return msgtype;
}

//---------------------------------------------------------------
// MsgTypeNotifier::onMMContentAdded
// @see header file
//---------------------------------------------------------------
void MsgTypeNotifier::onMMContentAdded(bool isAdded)
{    if(isAdded)
        {
        mmContentCount++;
        if(mmContentCount == 1)
            {
            showPopup(LOC_POP_MESSAGE_CHANGE_MUL);
            }
        }
    else
        {
        mmContentCount--;
        if(mmContentCount == 0)
            {
            showPopup(LOC_POP_MESSAGE_CHANGE_TEXT);
            }
        
        Q_ASSERT(mmContentCount >= 0);
        }
}

void MsgTypeNotifier::showPopup(const QString& text)
{
    if(!mSkipNote)
    {
        HbNotificationDialog* dlg = new HbNotificationDialog();
        dlg->setFocusPolicy(Qt::NoFocus);
        dlg->setAttribute(Qt::WA_DeleteOnClose, true);
        dlg->setText(text);
        dlg->show();
    }
    // reset skip note flag
    mSkipNote = false;
}

void MsgTypeNotifier::setSkipNote()
{
    mSkipNote = true;
}

//EOF
