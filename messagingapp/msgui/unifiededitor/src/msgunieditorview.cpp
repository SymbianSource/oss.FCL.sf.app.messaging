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

// SYSTEM INCLUDES
#include <e32base.h>
#include <HbMenu>
#include <HbAction>
#include <HbToolBar>
#include <QGraphicsLinearLayout>
#include <HbGroupBox>
#include <HbScrollArea>
#include <QDateTime>
#include <QDir>
#include <QBuffer>
#include <QFile>
#include <QFileInfo>
#include <xqconversions.h>
#include <HbToolBarExtension>
#include <HbListWidget>
#include <HbFrameBackground>
#include <HbListViewItem>
#include <HbListWidgetItem>
#include <HbNotificationDialog>
#include <HbMessageBox>
#include <xqaiwrequest.h>
#include <xqappmgr.h>

// QT Mobility for fetching business card
#include <qversitwriter.h>
#include <qversitdocument.h>
#include <qcontact.h>
#include <qcontactmanager.h>
#include <qversitcontactexporter.h>
#include <cntservicescontact.h>

// USER INCLUDES
#include "debugtraces.h"
#include "msgunieditorview.h"
#include "msgunieditoraddress.h"
#include "msgunieditorsubject.h"
#include "msgunieditorbody.h"
#include "msgmonitor.h"
#include "msgattachmentcontainer.h"
#include "msgsendutil.h"
#include "convergedmessageaddress.h"
#include "unieditorgenutils.h"
#include "unieditorpluginloader.h"
#include "unieditorplugininterface.h"

// Constants
const QString SEND_ICON("qtg_mono_send");
const QString ATTACH_ICON("qtg_mono_attach");
// temporary folder for unieditor
const QString UNIFIED_EDITOR_TEMP_FOLDER("unifiededitor");

const int INVALID_MSGID = -1;
// vcard file extn.
const QString FILE_EXTN(".vcf");
// Max vcards inside a msg. Using a very large number.
// TODO: how we can avoid this?
const int MAX_VCARDS(1000);

// LOCALIZED CONSTANTS
//To,Cc.Bcc
#define LOC_TO  hbTrId("txt_messaging_formlabel_to")
#define LOC_CC hbTrId("txt_messaging_formlabel_cc")
#define LOC_BCC hbTrId("txt_messaging_formlabel_bcc")

//attach options
#define LOC_PHOTO           hbTrId("txt_messaging_button_photo")
#define LOC_SOUND           hbTrId("txt_messaging_button_sound")
#define LOC_BUSINESS_CARD   hbTrId("txt_messaging_button_business_card")

//options menu.
#define LOC_ADD_SUBJECT     hbTrId("txt_messaging_opt_add_subject")
#define LOC_ADD_CC_BCC      hbTrId("txt_messaging_opt_add_cc_bcc")
#define LOC_PRIORITY        hbTrId("txt_messaging_setlabel_priority")
#define LOC_SENDING_OPTIONS hbTrId("txt_messaging_opt_sending_options")
#define LOC_DELETE_MESSAGE  hbTrId("txt_messaging_opt_delete_message")

//priority sub menu
#define LOC_HIGH hbTrId("txt_messaging_setlabel_priority_val_high")
#define LOC_NORMAL hbTrId("txt_messaging_setlabel_priority_val_normal")
#define LOC_LOW hbTrId("txt_messaging_setlabel_priority_val_low")

//group box
#define LOC_OTHER_RECIPIENTS(n) hbTrId("txt_messaging_group_title_ln_other_recipients",n)

//saved to draft note
#define LOC_SAVED_TO_DRAFTS    hbTrId("txt_messaging_dpopinfo_saved_to_drafts")

//delete confermation
#define LOC_NOTE_DELETE_MESSAGE hbTrId("txt_messaging_dialog_delete_message")
#define LOC_BUTTON_DELETE       hbTrId("txt_common_button_delete")
#define LOC_BUTTON_CANCEL       hbTrId("txt_common_button_cancel")
#define LOC_DIALOG_OK           hbTrId("txt_common_button_ok")

//extension list item frame.
const QString POPUP_LIST_FRAME("qtg_fr_popup_list_normal");

// LOCAL FUNCTIONS
QString editorTempPath();

//---------------------------------------------------------------
// MsgUnifiedEditorView::MsgUnifiedEditorView
// @see header file
//---------------------------------------------------------------
MsgUnifiedEditorView::MsgUnifiedEditorView( QGraphicsItem *parent ) :
    MsgBaseView(parent),
    mSubjectAction(0),
    mCcBccAction(0),
    mMainLayout(0),
    mSubjectField(0),
    mToField(0),
    mCcField(0),
    mBccField(0),
    mBody(0),
    mContentWidget(0),
    mMsgMonitor(0),    
    mAttachmentContainer(0),
    mPluginLoader(0),
    mCanSaveToDrafts(true)
    {
    addMenu();
    addToolBar();

    HbScrollArea* scrollArea = new HbScrollArea(this);
    this->setWidget(scrollArea);

    mContentWidget = new HbWidget(this);
    scrollArea->setContentWidget(mContentWidget);

    mPluginPath = pluginPath(); 

    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical, mContentWidget);
    qreal vTopSpacing = 0.0;
    qreal vItemSpacing = 0.0;
    style()->parameter("hb-param-margin-gene-top",vTopSpacing);    
    style()->parameter("hb-param-margin-gene-middle-vertical",vItemSpacing);
    
    mMainLayout->setContentsMargins(0,vTopSpacing,0,0);
    mMainLayout->setSpacing(vItemSpacing);

    mMsgMonitor = new MsgMonitor(this);    

    mToField = new MsgUnifiedEditorAddress( LOC_TO, mPluginPath, mContentWidget );
    mBody = new MsgUnifiedEditorBody(mPluginPath, mContentWidget);

    mMainLayout->addItem(mToField);
    mMainLayout->addItem(mBody);
 
    //Set the invalid msg id
    mOpenedMessageId.setId(-1);

    // create editor's temp folder
    QDir tempDir = QDir(QString());
    QString tempPath(editorTempPath());
    if(tempDir.mkpath(tempPath))
    {
        tempDir.cd(tempPath);
        // remove stale folder content when freshly launched
        QStringList contentList(tempDir.entryList());
        int contentCount = contentList.count();
        for(int i=0; i<contentCount; i++)
        {
            tempDir.remove(contentList.at(i));
        }
    }

    connect(mToField, SIGNAL(sendMessage()), this, SLOT(send()));
    connect(mBody, SIGNAL(sendMessage()), this, SLOT(send()));
    connect(mBody, SIGNAL(contentChanged()),
            mMsgMonitor, SLOT(checkMsgTypeChange()));
    }

//---------------------------------------------------------------
// MsgUnifiedEditorView::~MsgUnifiedEditorView
// @see header file
//---------------------------------------------------------------
MsgUnifiedEditorView::~MsgUnifiedEditorView()
{
    // clean editor's temporary contents before exiting
    QDir tempDir = QDir(QString());
    QString tempPath(editorTempPath());
    tempDir.cd(tempPath);
    QStringList contentList(tempDir.entryList());
    int contentCount = contentList.count();
    for(int i=0; i<contentCount; i++)
    {
        tempDir.remove(contentList.at(i));
    }
    tempDir.cdUp();
    tempDir.rmdir(UNIFIED_EDITOR_TEMP_FOLDER);
}

void MsgUnifiedEditorView::addMenu()
{
    //Create Menu Options
    HbMenu* mainMenu = new HbMenu();

    //TODO:These 2 should be submenu option to Add
    mSubjectAction = mainMenu->addAction(LOC_ADD_SUBJECT);
    mCcBccAction = mainMenu->addAction(LOC_ADD_CC_BCC);
    
    HbMenu* prioritySubMenu = mainMenu->addMenu(LOC_PRIORITY);

    HbAction* highPriorityAction = prioritySubMenu->addAction(LOC_HIGH);
    highPriorityAction->setData(ConvergedMessage::High);

    HbAction* normalPriorityAction = prioritySubMenu->addAction(LOC_NORMAL);
    normalPriorityAction->setData(ConvergedMessage::Normal);

    HbAction* lowPriorityAction = prioritySubMenu->addAction(LOC_LOW);
    lowPriorityAction->setData(ConvergedMessage::Low);

    HbAction* sendOptionsAction = mainMenu->addAction(LOC_SENDING_OPTIONS);
    HbAction* deleteMsgAction = mainMenu->addAction(LOC_DELETE_MESSAGE);

    connect(mSubjectAction,SIGNAL(triggered()),this, SLOT(addSubject()));
    connect(mCcBccAction,SIGNAL(triggered()),this, SLOT(addCcBcc()));
    connect(highPriorityAction, SIGNAL(triggered()), this, SLOT(changePriority()));
    connect(normalPriorityAction, SIGNAL(triggered()), this, SLOT(changePriority()));
    connect(lowPriorityAction, SIGNAL(triggered()), this, SLOT(changePriority()));
    connect(sendOptionsAction,SIGNAL(triggered()),this, SLOT(sendingOptions()));
    connect(deleteMsgAction,SIGNAL(triggered()),this, SLOT(deleteMessage()));

    setMenu(mainMenu);
}

void MsgUnifiedEditorView::openDraftsMessage(const QVariantList& editorData)
{
    // unpack editor's data
    // first arg is convergedmessage
    QByteArray dataArray = editorData.at(0).toByteArray();
    ConvergedMessage* messageDetails = new ConvergedMessage;
    QDataStream stream(&dataArray, QIODevice::ReadOnly);
    messageDetails->deserialize(stream);
    ConvergedMessage::MessageType messageType = messageDetails->messageType();
    ConvergedMessageId messageId = *(messageDetails->id());
    delete messageDetails;

    if(!mPluginLoader)
    {
        mPluginLoader = new UniEditorPluginLoader(this);
    }

    UniEditorPluginInterface* pluginInterface =
        mPluginLoader->getUniEditorPlugin(messageType);

    mOpenedMessageId.setId(messageId.getId());
    mmOpenedMessageType = messageType;

    //Fetch the converged message from the msgId
    ConvergedMessage* msg;
    msg = pluginInterface->convertFrom(messageId.getId());

    if( msg != NULL )
    {
        //Populate the content inside editor
        populateContentIntoEditor(*msg);
        delete msg;
    }
}

void MsgUnifiedEditorView::forwardMessage(ConvergedMessageId& messageId,
    ConvergedMessage::MessageType messageType )
{
    if(!mPluginLoader)
    {
        mPluginLoader = new UniEditorPluginLoader(this);
  
    }
    UniEditorPluginInterface* pluginInterface = NULL;
    if( messageType == ConvergedMessage::Mms )
    {
        pluginInterface =
                mPluginLoader->getUniEditorPlugin(ConvergedMessage::Mms);
    }
    else // For sms,vcard,vcal cases
    {
        pluginInterface =
                mPluginLoader->getUniEditorPlugin(ConvergedMessage::Sms);
    }

    //Fetch the converged message from the msgId
    ConvergedMessage* msg;
    msg = pluginInterface->convertFrom(messageId.getId(),
        UniEditorPluginInterface::Forward);

    if( msg != NULL )
    {
        //Populate the content inside editor
        populateContentIntoEditor(*msg);
        delete msg;
    }
}

void MsgUnifiedEditorView::populateContent(const QVariantList& editorData)
{
    // unpack editor's data
    // first arg is convergedmessage
    QByteArray dataArray = editorData.at(0).toByteArray();
    ConvergedMessage* messageDetails = new ConvergedMessage;
    QDataStream stream(&dataArray, QIODevice::ReadOnly);
    messageDetails->deserialize(stream);

    // get next arg i.e. editor operation command
    int editorOp = 0;
    if(editorData.length() > 1)
    {
        editorOp = editorData.at(1).toInt();
    }

    // population logic based on editor Operation command
    switch(editorOp)
    {
        case MsgBaseView::ADD_RECIPIENTS:
        {
            addCcBcc();
        }
        break;
        case MsgBaseView::ADD_SUBJECT:
        {
            addSubject();
        }
        break;
        case MsgBaseView::ADD_VCARD:
        {
            contactsFetched(editorData.at(2));
        }
        break;
        case MsgBaseView::FORWARD_MSG:
        {
            forwardMessage(*messageDetails->id(), messageDetails->messageType());
        }
        break;
        default:
        break;
    }

    // additional common operations for non-forwarded messages
    if(editorOp != MsgBaseView::FORWARD_MSG)
    {
        mToField->setAddresses(messageDetails->toAddressList());
        QString bodyTxt = messageDetails->bodyText();
        mBody->setText(bodyTxt);

        int attachmentCount = messageDetails->attachments().count();
        QStringList pendingAttList;
        for(int i=0; i<attachmentCount; i++)
        {
            int imageSize=0;
            QString mimeType;
            TMsgMediaType mediaType = EMsgMediaUnknown;
            QString filePath = messageDetails->attachments().at(i)->filePath();
            UniEditorGenUtils* genUtils = new UniEditorGenUtils;
            TRAP_IGNORE(genUtils->getFileInfoL(filePath,imageSize,
                                           mimeType,mediaType));
            delete genUtils;
            switch(mediaType)
            {
                case EMsgMediaImage:
                {
                    mBody->setImage(filePath);
                    addSubject();
                }
                break;
                case EMsgMediaVideo:
                {
                    mBody->setVideo(filePath);
                    addSubject();
                }
                break;
                case EMsgMediaAudio:
                {
                    mBody->setAudio(filePath);
                    addSubject();
                }
                break;
                default:
                {
                    pendingAttList << filePath;
                }
                break;
            }
        }
        // add pending attachments in bulk
        addAttachments(pendingAttList);
    }
    delete messageDetails;
}

void MsgUnifiedEditorView::populateContentIntoEditor(
    const ConvergedMessage& messageDetails)
{
    // skip first-time MMS type switch note for draft
    mMsgMonitor->setSkipNote(true);
    mToField->setAddresses(messageDetails.toAddressList());
    if(messageDetails.ccAddressList().count() > 0 )
    {
        if(!mCcField)
        {
        addCcBcc();
        }
        mCcField->setAddresses(messageDetails.ccAddressList());
    }
    if(messageDetails.bccAddressList().count() > 0 )
    {
        if(!mBccField)
        {
        addCcBcc();
        }
        mBccField->setAddresses(messageDetails.bccAddressList());
    }
    if(messageDetails.subject().size() > 0)
    {
       if(!mSubjectField)
       {
           addSubject();
       }
       mSubjectField->setText(messageDetails.subject());
    }

    if(messageDetails.priority()==ConvergedMessage::High ||
            messageDetails.priority() == ConvergedMessage::Low)
    {
        if(!mSubjectField)
        {
            addSubject();
        }
        mSubjectField->setPriority(messageDetails.priority());
    }

    QString bodyTxt = messageDetails.bodyText();
    mBody->setText(bodyTxt);

    ConvergedMessageAttachmentList attachmentList =
        messageDetails.attachments();
    int attachmentCount = attachmentList.count();

    UniEditorGenUtils* genUtils = new UniEditorGenUtils;

    QStringList pendingAttList;
    for( int i=0; i < attachmentCount; i++ )
    {
        QString filePath = messageDetails.attachments().at(i)->filePath();

        if(attachmentList.at(i)->attachmentType() ==
            ConvergedMessageAttachment::EInline)
        {
            int imageSize=0;
            QString mimeType;
            TMsgMediaType mediaType = EMsgMediaUnknown;
            TRAP_IGNORE(genUtils->getFileInfoL(filePath,imageSize,
                mimeType,mediaType));

            switch(mediaType)
            {
                case EMsgMediaImage:
                {
                    mBody->setImage(filePath);
                    addSubject();
                    break;
                }
                case EMsgMediaVideo:
                {
                    mBody->setVideo(filePath);
                    addSubject();
                    break;
                }
                case EMsgMediaAudio:
                {
                    mBody->setAudio(filePath);
                    addSubject();
                    break;
                }
                default:
                {
                    pendingAttList << filePath;
                    break;
                }
            }
        }
        else
        {
            pendingAttList << filePath;
        }
    }
    // add pending attachments to editor
    addAttachments(pendingAttList);

    delete genUtils;
    // ensure that any msg-type change after this are shown
    mMsgMonitor->setSkipNote(false);
}

void MsgUnifiedEditorView::addToolBar()
{
    //Create Tool Bar
    HbToolBar *toolBar = new HbToolBar();
    toolBar->setOrientation(Qt::Horizontal);
    
    //tool bar extension for attach action.
    HbToolBarExtension* attachExtension = new HbToolBarExtension();
    HbAction *attachAction = toolBar->addExtension(attachExtension);    
    attachAction->setIcon(HbIcon(ATTACH_ICON));
    
    HbListWidget* extnList = new HbListWidget();
    extnList->addItem(LOC_PHOTO);
    extnList->addItem(LOC_SOUND);
    extnList->addItem(LOC_BUSINESS_CARD);

    HbListViewItem *prototype = extnList->listItemPrototype();
    HbFrameBackground frame(POPUP_LIST_FRAME, HbFrameDrawer::NinePieces);
    prototype->setDefaultFrame(frame);

    connect(extnList, SIGNAL(activated(HbListWidgetItem*)), this,
            SLOT(handleViewExtnActivated(HbListWidgetItem*)));
    connect(extnList, SIGNAL(activated(HbListWidgetItem*)), attachExtension, SLOT(close()));

    attachExtension->setContentWidget(extnList);

    //Add Action to the toolbar and show toolbar
    toolBar->addAction(HbIcon(SEND_ICON),QString(),this,SLOT(send()));


    setToolBar(toolBar);
}

void MsgUnifiedEditorView::addSubject()
{
    if(mSubjectField)
    { // do nothing if already present
        return;
    }
    // remove mainmenu's "Add Subject" action
    HbMenu* mainMenu = this->menu();
    mainMenu->removeAction(mSubjectAction);
    mSubjectAction->setParent(NULL);
    delete mSubjectAction;

    int index =0;
    int offset = 1;
    if(mAttachmentContainer)
    {
        ++offset;
    }
    index = mMainLayout->count() - offset;

    mSubjectField = new MsgUnifiedEditorSubject(mPluginPath, mContentWidget);
    mMainLayout->insertItem(index,mSubjectField);
    connect(mSubjectField, SIGNAL(contentChanged()),
            mMsgMonitor, SLOT(checkMsgTypeChange()));    
}

void MsgUnifiedEditorView::addCcBcc()
{
    if(mCcField && mBccField)
    { // do nothing if already present
        return;
    }

    // remove mainmenu's "Add Cc/Bcc" & "Add Subject" actions
    HbMenu* mainmenu = this->menu();
    mainmenu->removeAction(mCcBccAction);
    mCcBccAction->setParent(NULL);
    delete mCcBccAction;

    mCcField    = new MsgUnifiedEditorAddress( LOC_CC, mPluginPath, mContentWidget );
    mBccField   = new MsgUnifiedEditorAddress( LOC_BCC, mPluginPath, mContentWidget );

    connect(mCcField, SIGNAL(sendMessage()), this, SLOT(send()));
    connect(mCcField, SIGNAL(contentChanged()), mMsgMonitor, SLOT(checkMsgTypeChange()));
    connect(mBccField, SIGNAL(sendMessage()), this, SLOT(send()));
    connect(mBccField, SIGNAL(contentChanged()), mMsgMonitor, SLOT(checkMsgTypeChange()));

    HbWidget* groupWidget = new HbWidget(mContentWidget);
    groupWidget->setContentsMargins(0,0,0,0);
    
    QGraphicsLinearLayout* ccBccLayout = new QGraphicsLinearLayout(Qt::Vertical, groupWidget);
    ccBccLayout->setContentsMargins(0,0,0,0);

    qreal vItemSpacing = 0.0;    
    style()->parameter("hb-param-margin-gene-middle-vertical",vItemSpacing);
    ccBccLayout->setSpacing(vItemSpacing);
    ccBccLayout->addItem(mCcField);
    ccBccLayout->addItem(mBccField);

    HbGroupBox* groupBox = new HbGroupBox(0);    
    groupBox->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);
    
    groupBox->setContentWidget(groupWidget);
    groupBox->setHeading(LOC_OTHER_RECIPIENTS(-1));
    mMainLayout->insertItem(1,groupBox);
    connect(groupBox, SIGNAL(toggled(bool)), this, SLOT(updateOtherRecipientCount(bool)));
    
    // add subject field too
    addSubject();
    
    this->updateGeometry();
}

void MsgUnifiedEditorView::updateOtherRecipientCount(bool state)
{
    HbGroupBox* groupBox = qobject_cast<HbGroupBox*>(sender());
    if(groupBox)
    {
        if(!state)
        {
           groupBox->setHeading(LOC_OTHER_RECIPIENTS(-1));
        }
        else
        {
            int addrCount = mCcField->addressCount();
            addrCount += mBccField->addressCount();
            groupBox->setHeading(LOC_OTHER_RECIPIENTS(addrCount));
        }
    }
}

void MsgUnifiedEditorView::changePriority()
{
    HbAction* action = qobject_cast<HbAction*>(sender());

    ConvergedMessage::Priority priority = ConvergedMessage::Normal;
   if(action)
        {
        int data = action->data().toInt();
        priority = ConvergedMessage::Priority(data);
        }

    addSubject();
    mSubjectField->setPriority(priority);
}

void MsgUnifiedEditorView::sendingOptions()
{
}

void MsgUnifiedEditorView::deleteMessage()
{
    bool ok = HbMessageBox::question(LOC_NOTE_DELETE_MESSAGE,
                                            LOC_BUTTON_DELETE, LOC_BUTTON_CANCEL);
    
    if(ok)
        {
        mCanSaveToDrafts = false;
    
        //delete if draft entry opened
        if( mOpenedMessageId.getId() != -1)
            {    
            if(!mPluginLoader)
                {
                mPluginLoader = new UniEditorPluginLoader(this);
                }
        
            UniEditorPluginInterface* pluginInterface =
                mPluginLoader->getUniEditorPlugin(MsgMonitor::messageType());
        
            pluginInterface->deleteDraftsEntry(mOpenedMessageId.getId());
            }
    
        //trigger back action.
        HbAction* backAction = this->navigationAction();
        if(backAction)
            {
            backAction->trigger();
            }
        }
}

void MsgUnifiedEditorView::removeAttachmentContainer()
{
    if(mAttachmentContainer)
    {
        mMainLayout->removeItem(mAttachmentContainer);
        mAttachmentContainer->setParent(NULL);
        delete mAttachmentContainer;
        mAttachmentContainer = NULL;
    }
}

void MsgUnifiedEditorView::addAttachments(QStringList files)
{    
    int fcount = files.count();
    int i=0;
    for(i=0; i<fcount; i++)
    {
        if(MsgAttachmentContainer::EAddSizeExceed 
                == addAttachment(files.at(i)))
        {
            // size already exceeds max mms size-limit
            break;
        }
    }
    // check if some files failed to add
    // happens only when size exceeded during addition
    if(i<fcount)
    {
        // TODO: show a note for size exceed
    }
}

int MsgUnifiedEditorView::addAttachment(const QString& filepath)
{
    if(!mAttachmentContainer)
    {
        mAttachmentContainer = new MsgAttachmentContainer(mPluginPath, mContentWidget);
        connect(mAttachmentContainer, SIGNAL(emptyAttachmentContainer()),
                this, SLOT(removeAttachmentContainer()));
        connect(mAttachmentContainer, SIGNAL(contentChanged()),
                mMsgMonitor, SLOT(checkMsgTypeChange()));
        int index = mMainLayout->count() - 1;
        mMainLayout->insertItem(index,mAttachmentContainer);
    }

    int ret = mAttachmentContainer->addAttachment(filepath);
    if(ret != MsgAttachmentContainer::EAddSuccess)
    {
        // delete container, if it is empty
        if(mAttachmentContainer->count() == 0)
        {
            removeAttachmentContainer();
        }
    }
    else if(mAttachmentContainer->hasMMContent())
    {
        // when msg is converted to MMS, subject needs to be auto-inserted
        addSubject();
    }
    return ret;
}

QString MsgUnifiedEditorView::pluginPath()
{
    QString pluginPath;
    #ifdef Q_OS_WIN
    #define PLUGINPATH "../unifiededitorplugin/debug/unifiededitorplugind.dll"
    #endif
    #ifdef Q_OS_SYMBIAN
    #define PLUGINPATH "unifiededitorplugin.dll"
    #endif
    pluginPath.append(PLUGINPATH);
    return pluginPath;
}

void MsgUnifiedEditorView::send()
{
    activateInputBlocker();

    // converged msg for sending
    ConvergedMessage msg;
    ConvergedMessage::MessageType messageType = MsgMonitor::messageType();
    msg.setMessageType(messageType);

    ConvergedMessageAddressList addresses = mToField->addresses();
    if(messageType == ConvergedMessage::Sms &&
       addresses.isEmpty())
    {
        // no recipient specified for sms, do not send msg
        deactivateInputBlocker();
        return;
    }

    ConvergedMessageAddressList ccAddresses;
    ConvergedMessageAddressList bccAddresses;
    if(mCcField)
    {
        ccAddresses = mCcField->addresses();
    }
    if(mBccField)
    {
        bccAddresses = mBccField->addresses();
    }
    if( messageType == ConvergedMessage::Mms &&
            addresses.isEmpty() &&
            ccAddresses.isEmpty() &&
            bccAddresses.isEmpty())
    {
        // no recipient specified for mms, do not send msg
        deactivateInputBlocker();
        return;
    }

    packMessage(msg);
    
    // send message
    MsgSendUtil *sendUtil = new MsgSendUtil(this);
    int sendResult = sendUtil->send(msg);
    delete sendUtil;
    
    // all checks and validations happen before send
    if( sendResult == KErrNone)
    {
        //After sending the new content to drafts chk if the msg
        //was originally opened from drafts and delete the opened entry
        if( mOpenedMessageId.getId() != -1)
        {

            if(!mPluginLoader)
            {
                mPluginLoader = new UniEditorPluginLoader(this);
            }

            UniEditorPluginInterface* pluginInterface =
                mPluginLoader->getUniEditorPlugin(messageType);
            //TODO delete the entry
            pluginInterface->deleteDraftsEntry(mOpenedMessageId.getId());
        }

        ConvergedMessageAddressList addrList = addresses;
        QString receipient;

        int recepientCount = addrList.count();
        if(recepientCount)
        {
            receipient = addrList.at(0)->address();
        }

        if( mCcField && mBccField)
        {
            addrList = ccAddresses;
            int ccCount = addrList.count();
            recepientCount += ccCount;
            if(ccCount)
            {
                receipient = addrList.at(0)->address();
            }

            addrList = bccAddresses;
            int bccCount = addrList.count();
            recepientCount += bccCount;
            if(bccCount)
            {
                receipient = addrList.at(0)->address();
            }
        }


    QVariantList params;

    if(recepientCount == 1 )
        {
        params << MsgBaseView::CV;  // target view
        params << MsgBaseView::UNIEDITOR; // source view

        params << receipient;
        }
    else
        {
        params << MsgBaseView::CLV;// target view
        params << MsgBaseView::UNIEDITOR; // source view
        }
        deactivateInputBlocker();
        emit switchView(params);
    }
    else
    {
        deactivateInputBlocker();
        if(sendResult == KErrNotFound)
        {
        	  bool result = HbMessageBox::question("Settings not defined\nDefine now ?",
                                         LOC_DIALOG_OK,
                                         LOC_BUTTON_CANCEL);
            if (result)
            {
              QVariantList params;
              params << MsgBaseView::MSGSETTINGS;// target view
              params << MsgBaseView::UNIEDITOR; // source view
              emit switchView(params);
            }
        }
    }
}

void MsgUnifiedEditorView::packMessage(ConvergedMessage &msg)
{
    ConvergedMessage::MessageType messageType = MsgMonitor::messageType();
    msg.setMessageType(messageType);

    ConvergedMessageAddressList addresses = mToField->addresses();
    ConvergedMessageAddressList ccAddresses;
    ConvergedMessageAddressList bccAddresses;

    msg.addToRecipients(addresses);
    msg.setBodyText(mBody->text());
    msg.setDirection(ConvergedMessage::Outgoing);
    QDateTime time = QDateTime::currentDateTime();
    msg.setTimeStamp(time.toTime_t());

    if(messageType == ConvergedMessage::Mms)
    {
        if(mCcField)
        {
            ccAddresses = mCcField->addresses();
        }

        if(mBccField)
        {
            bccAddresses = mBccField->addresses();
        }

        int matchDigitsCount = MsgUnifiedEditorAddress::contactMatchDigits();
        //comapre cc and to field,remove duplicate from cc
        foreach(ConvergedMessageAddress *ccAddress,ccAddresses)
        {
          foreach(ConvergedMessageAddress *toAddress,addresses)
          {
             if(0 == ccAddress->address().right(matchDigitsCount).compare(toAddress->address().right(matchDigitsCount)))
             {
                ccAddresses.removeOne(ccAddress);
             }
          }
        }
        //comapre bcc and cc field,remove duplicate from bcc
        foreach(ConvergedMessageAddress *bccAddress,bccAddresses)
        {
          foreach(ConvergedMessageAddress *ccAddress,ccAddresses)
          {
             if(0 == bccAddress->address().right(matchDigitsCount).compare(ccAddress->address().right(matchDigitsCount)))
             {
                bccAddresses.removeOne(bccAddress);
             }
          }
        }
        //comapre bcc and to field,remove duplicate from bcc
        foreach(ConvergedMessageAddress *bccAddress,bccAddresses)
        {
          foreach(ConvergedMessageAddress *toAddress,addresses)
          {
             if(0 == bccAddress->address().right(matchDigitsCount).compare(toAddress->address().right(matchDigitsCount)))
             {
                bccAddresses.removeOne(bccAddress);
             }
          }
        }

        if(ccAddresses.count()>0)
        {
        msg.addCcRecipients(ccAddresses);
        }
        if(bccAddresses.count()>0)
        {
        msg.addBccRecipients(bccAddresses);
        }
        if(mSubjectField)
        {
            msg.setSubject(mSubjectField->text());
            msg.setPriority(mSubjectField->priority());
        }

        ConvergedMessageAttachmentList mediaList;

        QStringList mediafiles(mBody->mediaContent());
        if (!mediafiles.isEmpty())
        {
            for (int i = 0; i < mediafiles.size(); ++i)
            {
                ConvergedMessageAttachment* attachment =
                    new ConvergedMessageAttachment(
                        mediafiles.at(i),
                        ConvergedMessageAttachment::EInline);
                mediaList << attachment;
            }

        }
        if(mediaList.count() > 0)
        {
            msg.addAttachments(mediaList);
        }
    }

    // sms/mms attachment list
    ConvergedMessageAttachmentList attachmentList;
        if(mAttachmentContainer)
        {
            MsgUnifiedEditorAttachmentList editorAttachmentList =
                mAttachmentContainer->attachmentList();
                for (int i = 0; i < editorAttachmentList.count(); ++i)
                {
                    ConvergedMessageAttachment* attachment =
                        new ConvergedMessageAttachment(
                            editorAttachmentList.at(i)->path(),
                            ConvergedMessageAttachment::EAttachment);
                    attachmentList << attachment;
                }
            }
        if(attachmentList.count() > 0)
        {
            msg.addAttachments(attachmentList);
        }
}

void MsgUnifiedEditorView::saveContentToDrafts()
{
    if(!mCanSaveToDrafts)
        {
        return;
        }
    activateInputBlocker();
    ConvergedMessage::MessageType messageType = MsgMonitor::messageType();

    ConvergedMessageAddressList addresses = mToField->addresses();

    UniEditorPluginInterface* pluginInterface = NULL;

    if( mOpenedMessageId.getId() != -1)
    {
        if(!mPluginLoader)
        {
            mPluginLoader = new UniEditorPluginLoader(this);
        }

        pluginInterface =
            mPluginLoader->getUniEditorPlugin(messageType);
    }

    if(messageType == ConvergedMessage::Sms &&
            addresses.isEmpty() &&
            MsgMonitor::bodySize() <= 0 &&
            MsgMonitor::containerSize() <= 0)
    {
        if(mOpenedMessageId.getId() != -1)
        {
        pluginInterface->deleteDraftsEntry(mOpenedMessageId.getId());
        }

        // if empty msg, do not save
        deactivateInputBlocker();
        return;
    }

    ConvergedMessageAddressList ccAddresses;
    ConvergedMessageAddressList bccAddresses;
    if(mCcField)
    {
        ccAddresses = mCcField->addresses();
    }
    if(mBccField)
    {
        bccAddresses = mBccField->addresses();
    }
    int subectSize =0;
    if(mSubjectField)
    {
        subectSize = mSubjectField->text().size();
    }

    if( messageType == ConvergedMessage::Mms &&
            addresses.isEmpty() &&
            ccAddresses.isEmpty() &&
            bccAddresses.isEmpty() &&
            subectSize <= 0 &&
            MsgMonitor::bodySize() <= 0 &&
            MsgMonitor::containerSize() <= 0)
    {
        if(mOpenedMessageId.getId() != -1)
        {
            pluginInterface->deleteDraftsEntry(mOpenedMessageId.getId());
        }
        // if empty msg, do not send
        deactivateInputBlocker();
        return;
    }
    ConvergedMessage msg;
    packMessage(msg);

    // save to drafts
    MsgSendUtil *sendUtil = new MsgSendUtil(this);
    int msgId = sendUtil->saveToDrafts(msg);
    delete sendUtil;

    // If saving new msg to draft succeeded and also if the originally
    // opened msg was from drafts, then delete the original msg entry
    if(msgId != -1 && mOpenedMessageId.getId() != -1)
    {
        pluginInterface->deleteDraftsEntry(mOpenedMessageId.getId());
        mOpenedMessageId.setId(INVALID_MSGID);
    }

    deactivateInputBlocker();

    bool res = ((msgId > INVALID_MSGID)? true : false);
    
    if(res)
        {
        HbNotificationDialog::launchDialog(LOC_SAVED_TO_DRAFTS);
        }
}

void MsgUnifiedEditorView::resizeEvent( QGraphicsSceneResizeEvent * event )
{
 Q_UNUSED(event)
 #ifdef _DEBUG_TRACES_
 qDebug()<<"resize event";
#endif

 mContentWidget->resize(this->rect().width(),this->rect().height()+1);
}

//---------------------------------------------------------------
// editorTempPath
// @return fullPath of unified editor's temporary dir
//---------------------------------------------------------------
QString editorTempPath()
{
    QDir tempDir = QDir(QString());
    QString tempPath(QDir::toNativeSeparators(tempDir.tempPath()));
    tempPath.append(QDir::separator());
    tempPath.append(UNIFIED_EDITOR_TEMP_FOLDER);
    tempPath.append(QDir::separator());
    return tempPath;
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::createVCards
// @see header file
//---------------------------------------------------------------
int MsgUnifiedEditorView::createVCards(
        const QVariant& value, QStringList& filelist)
{
    // make sure that temp-folder is created for storing vcards
    QDir tempDir = QDir(QString());
    if(!tempDir.mkpath(editorTempPath()))
    {
        return KErrGeneral;
    }
    tempDir.cd(editorTempPath());


    // extract contact-list
    QContactManager* contactManager = new QContactManager("symbian");
    CntServicesContactList cntServicesContacts = qVariantValue<CntServicesContactList>(value);
    int cntCount = cntServicesContacts.count();
    QList<QtMobility::QContact> contactList;
    for(int i = 0; i < cntCount; i++ )
    {
        contactList << contactManager->contact( cntServicesContacts.at(i).mContactId );
    }
    delete contactManager;

    // get list of all versit-documents
    QVersitDocument::VersitType versitType(QVersitDocument::VCard21Type);
    QVersitContactExporter* exporter = new QVersitContactExporter();
    
    bool ret_val = exporter->exportContacts(contactList, versitType);
    QList<QtMobility::QVersitDocument> documentList = exporter->documents();    

    delete exporter;

    // loop though and create a vcard for each contact
    QVersitWriter* writer = new QVersitWriter();
    for(int i = 0; i < cntCount; i++ )
    {
        // buffer to hold vcard data fetched from contacts
        QBuffer contactsbuf;
        contactsbuf.open(QBuffer::ReadWrite);
        writer->setDevice( &contactsbuf );

        //write document data to buffer
        QList<QtMobility::QVersitDocument> currDoc;
        currDoc << documentList.at(i);
        writer->startWriting(currDoc);
        if(writer->waitForFinished())
        {
            // generate file name
            QString displayLabel = contactList.at(i).displayLabel();
            QString filepath = generateFileName(displayLabel);
            QFile file(filepath);
            if(file.open(QIODevice::WriteOnly))
            {
                // trap ignore so that, incase of multiselection, other vcards are still created
                QByteArray bufArr;
                TRAP_IGNORE(
                CBufBase* contactbufbase = CBufFlat::NewL(contactsbuf.size());
                CleanupStack::PushL(contactbufbase);
                contactbufbase->InsertL( contactbufbase->Size(),
                        *XQConversions::qStringToS60Desc8( contactsbuf.data() ) );
                TPtr8 ptrbuf(contactbufbase->Ptr(0));
                bufArr = XQConversions::s60Desc8ToQByteArray(ptrbuf);
                CleanupStack::PopAndDestroy(contactbufbase);
                );
                file.write(bufArr);
                file.close();
                filelist << filepath;
            }
        }
    }

    // cleanup & return
    delete writer;
    return KErrNone;
}


//---------------------------------------------------------------
// MsgUnifiedEditorView::generateFileName
// @param suggestedName suggested name of the file
// @return fullPath of the generated filename
// @algo For multiselected 'Unnamed' contacts, the filename should
// be synthesized as unnamed.vcf, unnamed1.vcf, unnamed2.vcf etc.
//---------------------------------------------------------------
QString MsgUnifiedEditorView::generateFileName(QString& suggestedName)
{
    QDir editorTempDir = QDir(QString());
    editorTempDir.cd(editorTempPath());

    for(int i=0; i<MAX_VCARDS; i++)
    {
        QString searchFileName = suggestedName;
        if(i>0)
        {
            searchFileName.append(QString::number(i));
        }
        searchFileName.append(FILE_EXTN);

        // check if filename is already used by an attachment in container
        bool alreadyExists = false;
        if(mAttachmentContainer)
        {
            MsgUnifiedEditorAttachmentList attList =
                    mAttachmentContainer->attachmentList();
            int attCount = attList.count();
            for(int j=0; j<attCount; j++)
            {
                QFileInfo fileinfo(attList.at(j)->path());
                if(searchFileName == fileinfo.fileName())
                {
                    alreadyExists = true;
                    break;
                }
            }
        }

        if(!alreadyExists)
        {
            if(i>0)
            {
                suggestedName.append(QString::number(i));
            }
            break;
        }
    }
    QString filepath(editorTempPath());
    filepath.append(suggestedName);
    filepath.append(FILE_EXTN);
    return filepath;
}

void MsgUnifiedEditorView::handleViewExtnActivated(HbListWidgetItem* item)
    {
    QString itemText = item->text();
    
    if(itemText == LOC_PHOTO)
        {
        //launch photo picker.
        fetchImages();  
    }
    else if(itemText == LOC_SOUND)
        {
        //launch audio picker
        fetchAudio();
    }
    else if(itemText == LOC_BUSINESS_CARD)
        {
        //launch contact card picker.
        fetchContacts();
    }
   
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::fetchContacts
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::fetchContacts()
{
    QList<QVariant> args;
    QString serviceName("com.nokia.services.phonebookservices");
    QString operation("fetch(QString,QString,QString)");
    XQAiwRequest* request;
    XQApplicationManager appManager;
    request = appManager.create(serviceName, "Fetch", operation, true); //embedded
    if ( request == NULL )
    {
        QCRITICAL_WRITE("AIW-ERROR: NULL request");
        return;       
    }

    // Result handlers
    connect (request, SIGNAL(requestOk(const QVariant&)), 
        this, SLOT(contactsFetched(const QVariant&)));
    connect (request, SIGNAL(requestError(int,const QString&)), 
        this, SLOT(serviceRequestError(int,const QString&)));

    args << QString(tr("Phonebook")); 
    args << KCntActionAll;
    args << KCntFilterDisplayAll;

    request->setArguments(args);
    if(!request->send())
    {
        QDEBUG_WRITE_FORMAT("AIW-ERROR: Request Send failed:",request->lastError());
    }
    delete request;
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::fetchImages
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::fetchImages()
{
    QString interface("Image");
    QString operation("fetch(QVariantMap,QVariant)");
    XQAiwRequest* request = NULL;
    XQApplicationManager appManager;
    request = appManager.create(interface, operation, true);//embedded
    if(!request)
    {     
        QCRITICAL_WRITE("AIW-ERROR: NULL request");
        return;
    }

    connect(request, SIGNAL(requestOk(const QVariant&)),
        this, SLOT(imagesFetched(const QVariant&)));
    connect(request, SIGNAL(requestError(int,const QString&)),
        this, SLOT(serviceRequestError(int,const QString&)));
   
    // Make the request
    if (!request->send())
    {
        QDEBUG_WRITE_FORMAT("AIW-ERROR: Request Send failed:" , request->lastError());
    }  
    delete request;
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::fetchAudio
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::fetchAudio()
{
    QString service("Music Fetcher");
    QString interface("com.nokia.services.media.Music");
    QString operation("fetch(QString)");
    XQAiwRequest* request = NULL;
    XQApplicationManager appManager;
    request = appManager.create(service, interface, operation, true); //embedded
    if(!request)
    {
        QCRITICAL_WRITE("AIW-ERROR: NULL request");
        return;
    }

    connect(request, SIGNAL(requestOk(const QVariant&)),
        this, SLOT(audiosFetched(const QVariant&)));
    connect(request, SIGNAL(requestError(int,const QString&)),
        this, SLOT(serviceRequestError(int,const QString&)));

    // Make the request
    if (!request->send())
    {
        QDEBUG_WRITE_FORMAT("AIW-ERROR: Request Send failed :",request->lastError());
    }
    delete request;
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::contactsFetched
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::contactsFetched(const QVariant& value)
{
    // create a vcard for each contact fetched and add as attachment
    QStringList attachmentList;
    if(createVCards(value, attachmentList) == KErrNone)
    {
        addAttachments(attachmentList);
    }
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::imagesFetched
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::imagesFetched(const QVariant& result )
{
    if(result.canConvert<QStringList>())
    {
        QStringList fileList = result.value<QStringList>();
        if ( fileList.size()>0 )
        {
            QString filepath(QDir::toNativeSeparators(fileList.at(0)));
            mBody->setImage(filepath);
            addSubject();
        }
    }
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::audiosFetched
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::audiosFetched(const QVariant& result )
{
    if(result.canConvert<QStringList>())
    {
        QStringList fileList = result.value<QStringList>();
        if ( fileList.size()>0 && !fileList.at(0).isEmpty())
        {
            QString filepath(QDir::toNativeSeparators(fileList.at(0)));
            QDEBUG_WRITE_FORMAT("Received audio file path = ", fileList.at(0));
            mBody->setAudio(filepath);
            addSubject();
        }
    }
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::serviceRequestError
// @see header file
//---------------------------------------------------------------
void MsgUnifiedEditorView::serviceRequestError(int errorCode, const QString& errorMessage)
{
    QDEBUG_WRITE_FORMAT(errorMessage,errorCode);
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::activateInputBlocker
// @see header file
//--------------------------------------------------------------
void MsgUnifiedEditorView::activateInputBlocker()
    {
        this->grabMouse();
        this->grabKeyboard();
    }

//---------------------------------------------------------------
// MsgUnifiedEditorView::deactivateInputBlocker
// @see header file
//--------------------------------------------------------------
void MsgUnifiedEditorView::deactivateInputBlocker()
    {    
        this->ungrabKeyboard();
        this->ungrabMouse();
    }

//EOF
