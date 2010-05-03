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

// INCLUDES
#include "debugtraces.h"
#include <HbNotificationDialog>
#include <HbWidget>

// USER INCLUDES
#include "msgmonitor.h"
#include "msgunieditorview.h"
#include "msgunieditoraddress.h"
#include "msgunieditorsubject.h"
#include "msgunieditorbody.h"
#include "msgattachmentcontainer.h"
#include "UniEditorGenUtils.h"

// Constants

// Define static
ConvergedMessage::MessageType MsgMonitor::mMessageType;
int MsgMonitor::mBodySize;
int MsgMonitor::mContainerSize;
int MsgMonitor::mSubjectSize;
int MsgMonitor::mMaxMmsSize;
int MsgMonitor::mMaxSmsRecipients;
int MsgMonitor::mMaxMmsRecipients;

//Localized strings
#define LOC_POP_MESSAGE_CHANGE_MUL hbTrId("txt_messaging_dpopinfo_message_type_changed_to_mul")
#define LOC_POP_MESSAGE_CHANGE_TEXT hbTrId("txt_messaging_dpopinfo_message_type_changed_to_tex")

//---------------------------------------------------------------
// MsgMonitor::MsgMonitor
// @see header file
//---------------------------------------------------------------
MsgMonitor::MsgMonitor(QObject* parent) :
QObject(parent),
mSkipNote(false)
{
    init();
    mUniEditorGenUtils = new UniEditorGenUtils;
}

//---------------------------------------------------------------
// MsgMonitor::~MsgMonitor
// @see header file
//---------------------------------------------------------------
MsgMonitor::~MsgMonitor()
{
    delete mUniEditorGenUtils;
}

//---------------------------------------------------------------
// MsgMonitor::init
// @see header file
//---------------------------------------------------------------
void MsgMonitor::init()
{
    mMessageType = ConvergedMessage::Sms;
    mBodySize = 0;
    mContainerSize = 0;
    mSubjectSize = 0;

    UniEditorGenUtils* uniEditorGenUtils = new UniEditorGenUtils;

    mMaxMmsSize = KDefaultMaxSize;
    TRAP_IGNORE(mMaxMmsSize = uniEditorGenUtils->MaxMmsMsgSizeL());

    mMaxSmsRecipients = KDefaultSmsRecipients;
    TRAP_IGNORE(mMaxSmsRecipients = uniEditorGenUtils->MaxSmsRecipientsL());

    mMaxMmsRecipients = KDefaultMmsRecipients;
    TRAP_IGNORE(mMaxMmsRecipients = uniEditorGenUtils->MaxMmsRecipientsL());

    delete uniEditorGenUtils;
}

//---------------------------------------------------------------
// MsgMonitor::checkMsgTypeChange
// @see header file
//---------------------------------------------------------------
void MsgMonitor::checkMsgTypeChange()
{
    // fetch editor's content 
    MsgUnifiedEditorBody* edBody = view()->mBody;
    QStringList objList = edBody->mediaContent();
    QString bodyText = edBody->text();
    
    MsgUnifiedEditorSubject* edSubject = view()->mSubjectField;
    ConvergedMessage::Priority priority = ConvergedMessage::Normal;
    QString subjectText;
    if(edSubject)
    {
        priority = edSubject->priority();
        subjectText = edSubject->text();
    }

    MsgUnifiedEditorAddress* edCc = view()->mCcField;
    MsgUnifiedEditorAddress* edBcc = view()->mBccField;
    int ccCount = 0;
    int bccCount = 0;
    if(edCc && edBcc)
    {
        ccCount = edCc->addressCount();
        bccCount = edBcc->addressCount();
    }
    
    MsgAttachmentContainer* edContainer = view()->mAttachmentContainer;
    bool hasMMAttachmentContent = false;
    int attachmentCount = 0;
    if(edContainer)
    {
        hasMMAttachmentContent = edContainer->hasMMContent();
        attachmentCount = edContainer->count();
    }

    // find out the msgtype based on content
    ConvergedMessage::MessageType projectedMsgType = ConvergedMessage::Sms;

    // check for presence of MMS content
    // 1. If any media-object is present inside body
    // 2. If priority is set to other than Normal
    // 3. If subject has some content
    // 4. If CC/BCC has some content
    // 5. If MM attachments are present
    // 6. If only one non-MM attachment is present e.g. vcf 
    //    and body text is also present
    // 7. If body text size exceeds sms text-size limit
    if( !objList.isEmpty() || 
        (priority != ConvergedMessage::Normal) || 
        !subjectText.isEmpty() ||
        (ccCount || bccCount) ||
        hasMMAttachmentContent ||
        ((attachmentCount == 1) && !bodyText.isEmpty())
      )
    {
        projectedMsgType = ConvergedMessage::Mms;
    }
    else
    {
        projectedMsgType = ConvergedMessage::Sms;
    }

    // optimization 1: if projected type is still sms means
    // the message under composition has only plain text
    if(projectedMsgType == ConvergedMessage::Sms)
    {
        bool hasUnicodeText = edBody->isUnicode();
        int bodyTextSize = mUniEditorGenUtils->UTF8Size(bodyText);
        int maxSmsSize = mUniEditorGenUtils->MaxSmsMsgSizeL(hasUnicodeText);
        if(bodyTextSize > maxSmsSize)
        {
            projectedMsgType = ConvergedMessage::Mms;
        }
    }
        
    // show type change note, if needed
    if(mMessageType != projectedMsgType)
    {
        mMessageType = projectedMsgType;
        QString noteStr;
        if(projectedMsgType == ConvergedMessage::Sms)
        {
            noteStr = LOC_POP_MESSAGE_CHANGE_TEXT;
        }
        else
        {
            noteStr = LOC_POP_MESSAGE_CHANGE_MUL;
            
            //Disable char counter
            edBody->disableCharCounter();
        }
        showPopup(noteStr);
    }
    
    // update size of editor component
    HbWidget* senderWidget = qobject_cast<HbWidget*>(sender());
    updateSizeInfo(senderWidget);
}

//---------------------------------------------------------------
// MsgMonitor::updateSizeInfo
// @see header file
//---------------------------------------------------------------
void MsgMonitor::updateSizeInfo(HbWidget* aWidget)
{
    // if sent by body widget
    MsgUnifiedEditorBody* body = NULL;    
    body = qobject_cast<MsgUnifiedEditorBody*>(aWidget);
    if(body)
    {
        mBodySize = view()->mBody->bodySize();
        return;
    }
    
    // if sent by attachment container widget
    MsgAttachmentContainer* container = NULL;
    container = qobject_cast<MsgAttachmentContainer*>(aWidget);
    if(container)
    {
        mContainerSize = view()->mAttachmentContainer->containerSize();
        return;
    }

    // if sent by subject widget
    MsgUnifiedEditorSubject* subject = NULL;
    subject = qobject_cast<MsgUnifiedEditorSubject*>(aWidget);
    if(subject)
    {
        mSubjectSize = view()->mSubjectField->subjectSize();
    }
}

//---------------------------------------------------------------
// MsgMonitor::showPopup
// @see header file
//---------------------------------------------------------------
void MsgMonitor::showPopup(const QString& text)
{
    if(!mSkipNote)
    {
        HbNotificationDialog* dlg = new HbNotificationDialog();
        dlg->setFocusPolicy(Qt::NoFocus);
        dlg->setDismissPolicy(HbPopup::TapAnywhere);
        dlg->setAttribute(Qt::WA_DeleteOnClose, true);
        dlg->setText(text);
        dlg->show();
    }
    // reset skip note flag
    mSkipNote = false;
}

//---------------------------------------------------------------
// MsgMonitor::view
// @see header file
//---------------------------------------------------------------
MsgUnifiedEditorView* MsgMonitor::view()
{
    return static_cast<MsgUnifiedEditorView*>(this->parent());
}

//EOF
