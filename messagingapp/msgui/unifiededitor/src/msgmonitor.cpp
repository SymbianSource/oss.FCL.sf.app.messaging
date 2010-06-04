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
int MsgMonitor::mMsgCurrAddressCount;

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
    mMsgCurrAddressCount = 0;

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
// MsgMonitor::handleContentChange
// @see header file
//---------------------------------------------------------------
void MsgMonitor::handleContentChange()
{
    // get the projected message type & show the type change note
    ConvergedMessage::MessageType newMsgType = projectedMsgType();    
    if(mMessageType != newMsgType)
    {
        mMessageType = newMsgType;
        QString noteStr;
        if(newMsgType == ConvergedMessage::Sms)
        {
            noteStr = LOC_POP_MESSAGE_CHANGE_TEXT;
        }
        else
        {
            noteStr = LOC_POP_MESSAGE_CHANGE_MUL;
        }
        showPopup(noteStr);
    }

    HbWidget* senderWidget = qobject_cast<HbWidget*>(sender());
    updateMsgInfo(senderWidget);
}

//---------------------------------------------------------------
// MsgMonitor::projectedMsgType
// @see header file
//---------------------------------------------------------------
ConvergedMessage::MessageType MsgMonitor::projectedMsgType()
{
    ConvergedMessage::MessageType newMsgType = ConvergedMessage::Sms;

    // check if MMS content is present in any of the editor component
    if( bodyHasMMSContent() ||
        subjectHasMMSContent() ||
        containerHasMMSContent() ||
        otherMMSCriteriaMet() )
    {
        newMsgType = ConvergedMessage::Mms;
    }
    return newMsgType;
}

//---------------------------------------------------------------
// MsgMonitor::updateMsgInfo
// @see header file
//---------------------------------------------------------------
void MsgMonitor::updateMsgInfo(HbWidget* senderWidget)
{
    if(mMessageType == ConvergedMessage::Mms)
    {
        //Disable char counter & add subject
        view()->mBody->disableCharCounter();
        view()->addSubject();
    }

    // check if sent by body widget
    MsgUnifiedEditorBody* body = NULL;
    body = qobject_cast<MsgUnifiedEditorBody*>(senderWidget);
    if(body)
    {
        mBodySize = view()->mBody->bodySize();
        view()->setAttachOptionEnabled(
                MsgUnifiedEditorView::TBE_PHOTO, !view()->mBody->hasImage());
        view()->setAttachOptionEnabled(
                MsgUnifiedEditorView::TBE_SOUND, !view()->mBody->hasAudio());
        return;
    }
    
    // check if sent by subject widget
    MsgUnifiedEditorSubject* subject = NULL;
    subject = qobject_cast<MsgUnifiedEditorSubject*>(senderWidget);
    if(subject)
    {
        mSubjectSize = view()->mSubjectField->subjectSize();
        return;
    }

    // check if sent by attachment container widget
    MsgAttachmentContainer* container = NULL;
    container = qobject_cast<MsgAttachmentContainer*>(senderWidget);
    if(container)
    {
        mContainerSize = view()->mAttachmentContainer->containerSize();
        return;
    }

    // handle content change from other widgets e.g. To, Cc, Bcc address field
    int totalAddressCount = view()->mToField->addressCount();
    if(view()->mCcField && view()->mBccField)
    {
        totalAddressCount += view()->mCcField->addressCount() +
                view()->mBccField->addressCount();
    }
    mMsgCurrAddressCount = totalAddressCount;
    return;
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

//---------------------------------------------------------------
// MsgMonitor::bodyHasMMSContent
// @see header file
//---------------------------------------------------------------
bool MsgMonitor::bodyHasMMSContent()
{
    MsgUnifiedEditorBody* edBody = view()->mBody;
    // If any media-object is present inside body
    if(!edBody->mediaContent().isEmpty())
    {
        return true;
    }
    
    int bodyTextSize = mUniEditorGenUtils->UTF8Size(edBody->text());
    int maxSmsSize = 0;
    TRAP_IGNORE(maxSmsSize = 
           mUniEditorGenUtils->MaxSmsMsgSizeL(edBody->isUnicode()));
    // If body text size exceeds sms text-size limit
    if(bodyTextSize > maxSmsSize)
    {
        return true;
    }
    return false;
}

//---------------------------------------------------------------
// MsgMonitor::subjectHasMMSContent
// @see header file
//---------------------------------------------------------------
bool MsgMonitor::subjectHasMMSContent()
{
    MsgUnifiedEditorSubject* edSubject = view()->mSubjectField;
    ConvergedMessage::Priority priority = ConvergedMessage::Normal;
    QString subjectText;
    if(edSubject)
    {
        priority = edSubject->priority();
        subjectText = edSubject->text();
    }
    // If priority is set to other than Normal or
    // If subject has some content
    if( (priority != ConvergedMessage::Normal) ||
        !subjectText.isEmpty() )
    {
        return true;
    }
    return false;
}

//---------------------------------------------------------------
// MsgMonitor::containerHasMMSContent
// @see header file
//---------------------------------------------------------------
bool MsgMonitor::containerHasMMSContent()
{
    QString bodyText = view()->mBody->text();
    MsgAttachmentContainer* edContainer = view()->mAttachmentContainer;
    bool hasMMAttachmentContent = false;
    int attachmentCount = 0;
    if(edContainer)
    {
        hasMMAttachmentContent = edContainer->hasMMContent();
        attachmentCount = edContainer->count();
    }
    // If MM attachments are present or
    // If only one non-MM attachment is present e.g. vcf along with body text
    if( hasMMAttachmentContent ||
        ((attachmentCount == 1) && !bodyText.isEmpty()) )
    {
        return true;
    }
    return false;
}

//---------------------------------------------------------------
// MsgMonitor::otherMMSCriteriaMet
// @see header file
//---------------------------------------------------------------
bool MsgMonitor::otherMMSCriteriaMet()
{
    MsgUnifiedEditorAddress* edCc = view()->mCcField;
    MsgUnifiedEditorAddress* edBcc = view()->mBccField;
    int ccCount = 0;
    int bccCount = 0;
    if(edCc && edBcc)
    {
        ccCount = edCc->addressCount();
        bccCount = edBcc->addressCount();
    }
    // If CC/BCC has some content or
    // If to-recipients count exceeds max sms recipient count
    if( ccCount || bccCount ||
        (view()->mToField->addressCount() > mMaxSmsRecipients) )
    {
        return true;
    }
    
    // If to-field contains an email address
    bool isEmailPresent = false;
    ConvergedMessageAddressList addrList = view()->mToField->addresses();
    TRAP_IGNORE(isEmailPresent = mUniEditorGenUtils->VerifyEmailAddressesL(addrList));
    if(isEmailPresent)
    {
        return true;
    }
    return false;
}

//EOF
