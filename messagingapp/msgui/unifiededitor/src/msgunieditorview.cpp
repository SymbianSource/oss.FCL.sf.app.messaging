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
#include <HbWidget>
#include <HbMenu>
#include <HbAction>
#include <HbToolBar>
#include <QGraphicsLinearLayout>
#include <HbGroupBox>
#include <HbScrollArea>
#include <QDateTime>
#include <QDir>

// USER INCLUDES
#include "debugtraces.h"
#include "msgunieditorview.h"
#include "msgunieditoraddress.h"
#include "msgunieditorsubject.h"
#include "msgunieditorbody.h"
#include "msgtypenotifier.h"
#include "msgmonitor.h"
#include "msgattachmentcontainer.h"
#include "msguiutilsmanager.h"
#include "convergedmessageaddress.h"
#include "matchphnumberutil.h"
#include "unieditorgenutils.h"
#include "unieditorpluginloader.h"
#include "unieditorplugininterface.h"

// Constants
const QString SEND_ICON(":/qtg_mono_send.svg");
const QString ATTACH_ICON(":/qtg_mono_attach.svg");

const int INVALID_MSGID = -1;

// LOCALIZED CONSTANTS
#define LOC_TO hbTrId("txt_messaging_formlabel_to")
#define LOC_HIGH hbTrId("txt_messaging_setlabel_priority_val_high")
#define LOC_NORMAL hbTrId("txt_messaging_setlabel_priority_val_normal")
#define LOC_LOW hbTrId("txt_messaging_setlabel_priority_val_low")
#define LOC_SENDING_OPTIONS hbTrId("txt_messaging_opt_sending_options")

#define LOC_DELETE_MESSAGE hbTrId("txt_messaging_opt_delete_message")
#define LOC_CC hbTrId("txt_messaging_formlabel_cc")
#define LOC_BCC hbTrId("txt_messaging_formlabel_bcc")
#define LOC_OTHER_RECIPIENTS hbTrId("txt_messaging_group_title_ln_other_recipients")

#define LOC_PHOTO hbTrId("txt_messaging_button_photo")
#define LOC_VIDEO hbTrId("txt_messaging_button_video")
#define LOC_SOUND hbTrId("txt_messaging_button_sound")
#define LOC_MORE hbTrId("txt_messaging_button_more")

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
mPrioritySubMenu(0),
mContentWidget(0),
mNotifier(0),
mMsgMonitor(0),
mUtilsManager(0),
mAttachmentContainer(0),
mPluginLoader(0)
{
    addMenu();
    addToolBar();

    HbScrollArea* scrollArea = new HbScrollArea(this);
    this->setWidget(scrollArea);

    mContentWidget = new HbWidget(this);
    scrollArea->setContentWidget(mContentWidget);

    mPluginPath = pluginPath();

    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical, mContentWidget);
    mMainLayout->setContentsMargins(0,0,0,0);

    //Adding suitable spacing between the Custom Widgets
    qreal vItemSpacing = 0.0;
    style()->parameter("hb-param-margin-gene-middle-vertical",vItemSpacing);
    mMainLayout->setSpacing(vItemSpacing);

    // notifier for recording msg type transitions between sms & mms
    mNotifier = new MsgTypeNotifier(this);
    mMsgMonitor = new MsgMonitor(this);
    mUtilsManager = new MsgUiUtilsManager(this);

    mToField = new MsgUnifiedEditorAddress( LOC_TO, mPluginPath, mContentWidget );
    mBody = new MsgUnifiedEditorBody(mPluginPath, mContentWidget);

    mMainLayout->addItem(mToField);
    mMainLayout->addItem(mBody);
 
    //Set the invalid msg id
    mOpenedMessageId.setId(-1);
    
    connect(mToField, SIGNAL(sendMessage()), this, SLOT(send()));
    connect(mBody, SIGNAL(sendMessage()), this, SLOT(send()));
    connect(mBody, SIGNAL(mmContentAdded(bool)),
                     mNotifier, SLOT(onMMContentAdded(bool)));
    connect(mBody, SIGNAL(sizeChanged(int)),
                     mMsgMonitor, SLOT(onSizeChanged(int)));
    connect(mUtilsManager, SIGNAL(imagesFetched(const QVariant&)),
                     this, SLOT(imagesFetched(const QVariant&)));
    connect(mUtilsManager, SIGNAL(audiosFetched(const QVariant&)),
                     this, SLOT(audiosFetched(const QVariant&)));
}

//---------------------------------------------------------------
// MsgUnifiedEditorView::~MsgUnifiedEditorView
// @see header file
//---------------------------------------------------------------
MsgUnifiedEditorView::~MsgUnifiedEditorView()
{
}

void MsgUnifiedEditorView::addMenu()
{
    //Create Menu Options
    HbMenu* mainMenu = new HbMenu();

    //TODO:These 2 should be submenu option to Add
    mSubjectAction = mainMenu->addAction(tr("Add Subject"));
    mCcBccAction = mainMenu->addAction(tr("Add Cc/Bcc"));
    
    mPrioritySubMenu = mainMenu->addMenu(tr("Add Priority"));

    HbAction* highPriorityAction = mPrioritySubMenu->addAction(LOC_HIGH);
    highPriorityAction->setData(ConvergedMessage::High);

    HbAction* normalPriorityAction = mPrioritySubMenu->addAction(LOC_NORMAL);
    normalPriorityAction->setData(ConvergedMessage::Normal);

    HbAction* lowPriorityAction = mPrioritySubMenu->addAction(LOC_LOW);
    lowPriorityAction->setData(ConvergedMessage::Low);

    HbAction* sendOptionsAction = mainMenu->addAction(LOC_SENDING_OPTIONS);
    HbAction* deleteMsgAction = mainMenu->addAction(LOC_DELETE_MESSAGE);
    HbAction* helpAction = mainMenu->addAction(tr("Help"));

    connect(mSubjectAction,SIGNAL(triggered()),this, SLOT(addSubject()));
    connect(mCcBccAction,SIGNAL(triggered()),this, SLOT(addCcBcc()));
    connect(highPriorityAction, SIGNAL(triggered()), this, SLOT(changePriority()));
    connect(normalPriorityAction, SIGNAL(triggered()), this, SLOT(changePriority()));
    connect(lowPriorityAction, SIGNAL(triggered()), this, SLOT(changePriority()));
    connect(sendOptionsAction,SIGNAL(triggered()),this, SLOT(sendingOptions()));
    connect(deleteMsgAction,SIGNAL(triggered()),this, SLOT(deleteMessage()));
    connect(helpAction,SIGNAL(triggered()),this, SLOT(launchHelp()));

    setMenu(mainMenu);
}

void MsgUnifiedEditorView::openDraftsMessage(ConvergedMessageId& messageId,
    ConvergedMessage::MessageType messageType )
{
    if(!mPluginLoader)
    {
        mPluginLoader = new UniEditorPluginLoader(this);
        mPluginLoader->loadPlugins();
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
        populateDraftsContent(*msg);
        delete msg;
    }
}

void MsgUnifiedEditorView::populateContent(const ConvergedMessage& messageDetails, int editorFields)
{
    mToField->setAddresses(messageDetails.toAddressList());
    QString bodyTxt = messageDetails.bodyText();
    mBody->setText(bodyTxt);

    int attachmentCount = messageDetails.attachments().count();
    if(attachmentCount == 1)
    {
        int imageSize=0;
        QString mimeType;
        TMsgMediaType mediaType = EMsgMediaUnknown;
        QString filePath = messageDetails.attachments().at(0)->filePath();
        UniEditorGenUtils* genUtils = new UniEditorGenUtils;
        TRAP_IGNORE(genUtils->getFileInfoL(filePath,imageSize,
                                           mimeType,mediaType));
        delete genUtils;
        switch(mediaType)
        {
            case EMsgMediaImage:
            {
                mBody->setImage(filePath);
                break;
            }
            case EMsgMediaVideo:
            {
                mBody->setVideo(filePath);
                break;
            }
            case EMsgMediaAudio:
            {
                mBody->setAudio(filePath);
                break;
            }
            case EMsgMediaUnknown:
            case EMsgMediaText:
            default:
            {
                addAttachment(filePath);
                break;
            }
        }
        
        addSubject();
	}
	else if(attachmentCount > 1)
	{
		for(int i=0; i<attachmentCount; i++)
		{
		    QString filePath = messageDetails.attachments().at(i)->filePath();
		    //TODO: put size checks and show size exceeded msg when appropriate
		    addAttachment(filePath);
		}
		
		addSubject();
	}

	if(editorFields & MsgBaseView::ADD_RECIPIENTS)
	{
		addCcBcc();
	}
	else if(editorFields & MsgBaseView::ADD_SUBJECT)
  {
		addSubject();
	}    
}

void MsgUnifiedEditorView::populateDraftsContent(
    const ConvergedMessage& messageDetails)
{
    // skip first-time MMS type switch note for draft
    mNotifier->setSkipNote();
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
                    break;
                }
                case EMsgMediaVideo:
                {
                    mBody->setVideo(filePath);
                    break;
                }
                case EMsgMediaAudio:
                {
                    mBody->setAudio(filePath);
                    break;
                }
                case EMsgMediaUnknown:
                case EMsgMediaText:
                default:
                {
                    addAttachment(filePath);
                    break;
                }
            }
        }
        else
        {
            addAttachment(filePath);
        }
    }    
    delete genUtils;
}

void MsgUnifiedEditorView::addToolBar()
{
    //Create Tool Bar
    HbToolBar *toolBar = new HbToolBar();
    toolBar->setOrientation(Qt::Horizontal);

    //Add Action to the toolbar and show toolbar
    HbAction* attachToolButton = toolBar->addAction(HbIcon(ATTACH_ICON), QString());
    HbAction* sendToolButton = toolBar->addAction(HbIcon(SEND_ICON), QString());

    connect(attachToolButton, SIGNAL(triggered()), this, SLOT(showAttachmentPopup()));
    connect(sendToolButton, SIGNAL(triggered()), this, SLOT(send()));

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
    connect(mSubjectField, SIGNAL(mmContentAdded(bool)),
                     mNotifier, SLOT(onMMContentAdded(bool)));
    connect(mSubjectField, SIGNAL(sizeChanged(int)),
                     mMsgMonitor, SLOT(onSizeChanged(int)));
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
    connect(mCcField, SIGNAL(mmContentAdded(bool)), mNotifier, SLOT(onMMContentAdded(bool)));
    connect(mBccField, SIGNAL(sendMessage()), this, SLOT(send()));
    connect(mBccField, SIGNAL(mmContentAdded(bool)), mNotifier, SLOT(onMMContentAdded(bool)));

    HbWidget* groupWidget = new HbWidget(mContentWidget);
    groupWidget->setContentsMargins(0,0,0,0);
    QGraphicsLinearLayout* ccBccLayout = new QGraphicsLinearLayout(Qt::Vertical, groupWidget);
    ccBccLayout->addItem(mCcField);
    ccBccLayout->addItem(mBccField);

    HbGroupBox* groupBox = new HbGroupBox(mContentWidget);
    groupBox->setCollapsable(true);
    groupBox->setContentWidget(groupWidget);
    groupBox->setHeading(tr("Other recipients"));    
    connect(groupBox, SIGNAL(toggled(bool)), this, SLOT(updateOtherRecipientCount(bool)));

    mMainLayout->insertItem(1,groupBox);
    // add subject field too
    addSubject();
}

void MsgUnifiedEditorView::updateOtherRecipientCount(bool state)
{
    HbGroupBox* groupBox = qobject_cast<HbGroupBox*>(sender());
    if(groupBox)
    {
        if(!state)
        {
            groupBox->setHeading(tr("Other recipients"));
        }
        else
        {
            int addrCount = mCcField->addressCount();
            addrCount += mBccField->addressCount();
            QString str;
            str = QString(tr("%1 Other recipients")).arg(addrCount);
            groupBox->setHeading(str);
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
    if(priority != ConvergedMessage::Normal)
    {
		mPrioritySubMenu->setTitle("Change Priority");
	}
   else
   {
       mPrioritySubMenu->setTitle("Add Priority");
   }
}

void MsgUnifiedEditorView::sendingOptions()
{
}

void MsgUnifiedEditorView::deleteMessage()
{
}

void MsgUnifiedEditorView::launchHelp()
{
}

void MsgUnifiedEditorView::showAttachmentPopup()
{
    HbMenu* attachMenu = new HbMenu();
    attachMenu->deleteLater();
    attachMenu->setFrameType(HbPopup::Strong);

    HbAction* attachPhoto = attachMenu->addAction(LOC_PHOTO,mUtilsManager,SLOT(fetchImages()));
    HbAction* attachVideo = attachMenu->addAction(LOC_VIDEO,mUtilsManager,SLOT(fetchVideo()));
    HbAction* attachAudio = attachMenu->addAction(LOC_SOUND,mUtilsManager,SLOT(fetchAudio()));
    HbAction* attachOther = attachMenu->addAction(LOC_MORE,mUtilsManager,SLOT(fetchOther()));
    attachMenu->exec(QPoint());
    delete attachMenu;
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

void MsgUnifiedEditorView::addAttachment(const QString& filepath)
{
    if(!mAttachmentContainer)
    {
        mAttachmentContainer = new MsgAttachmentContainer(mPluginPath, mContentWidget);
        connect(mAttachmentContainer, SIGNAL(emptyAttachmentContainer()),
                this, SLOT(removeAttachmentContainer()));
        connect(mAttachmentContainer, SIGNAL(mmContentAdded(bool)),
                mNotifier, SLOT(onMMContentAdded(bool)));
        connect(mAttachmentContainer, SIGNAL(sizeChanged(int)),
                         mMsgMonitor, SLOT(onSizeChanged(int)));
        int index = mMainLayout->count() - 1;
        mMainLayout->insertItem(index,mAttachmentContainer);
    }
    mAttachmentContainer->addAttachment(filepath);
}

void MsgUnifiedEditorView::imagesFetched(const QVariant& result )
    {
    if(result.canConvert<QStringList>())
        {
        QStringList fileList = result.value<QStringList>();
        if ( fileList.size()>0 )
            {
            QString filepath(fileList.at(0));
            mBody->setImage(filepath);
            addSubject();
            }
        }
    }

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
    mUtilsManager->activateInputBlocker(this);

    // converged msg for sending
    ConvergedMessage msg;
    ConvergedMessage::MessageType messageType = mNotifier->messageType();
    msg.setMessageType(messageType);

    ConvergedMessageAddressList addresses = mToField->addresses();
    if(messageType == ConvergedMessage::Sms &&
       addresses.isEmpty())
    {
        // no recipient specified for sms, do not send msg
        mUtilsManager->deactivateInputBlocker();
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
        mUtilsManager->deactivateInputBlocker();
        return;
    }

    packMessage(msg);

    int sendResult = mUtilsManager->send(msg);
    
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
                mPluginLoader->loadPlugins();
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
        mUtilsManager->deactivateInputBlocker();
    emit switchView(params);
    }
    else
    {
        mUtilsManager->deactivateInputBlocker();
        if(sendResult == KErrNotFound)
        {
            QVariantList params;
            params << MsgBaseView::MSGSETTINGS;// target view
            params << MsgBaseView::UNIEDITOR; // source view
            emit switchView(params);
        }
    }
}

void MsgUnifiedEditorView::packMessage(ConvergedMessage &msg)
{
    ConvergedMessage::MessageType messageType = mNotifier->messageType();
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

        int matchDigitsCount = MatchPhNumberUtil::matchDigits();
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

bool MsgUnifiedEditorView::saveContentToDrafts()
{
    mUtilsManager->activateInputBlocker(this);
    ConvergedMessage::MessageType messageType = mNotifier->messageType();

    ConvergedMessageAddressList addresses = mToField->addresses();

    UniEditorPluginInterface* pluginInterface = NULL;
    
    if( mOpenedMessageId.getId() != -1)
    {    
        if(!mPluginLoader)
        {
            mPluginLoader = new UniEditorPluginLoader(this);
            mPluginLoader->loadPlugins();
        }

        pluginInterface =
            mPluginLoader->getUniEditorPlugin(messageType);
    }

    if(messageType == ConvergedMessage::Sms &&
            addresses.isEmpty() && MsgMonitor::bodySize() <= 0)
    {
        if(mOpenedMessageId.getId() != -1)
        {
        pluginInterface->deleteDraftsEntry(mOpenedMessageId.getId());
        }

        // no recipient specified for sms, do not send msg
        mUtilsManager->deactivateInputBlocker();
        return false;
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
        // no recipient specified for mms, do not send msg
        mUtilsManager->deactivateInputBlocker();
        return false;
    }
    ConvergedMessage msg;
    packMessage(msg);
    
    int msgId = mUtilsManager->saveToDrafts(msg);

    //After saving the new content to drafts chk if the msg 
    //was originally opened from drafts and delete the opened entry
    if(mOpenedMessageId.getId() != -1)
    {
        pluginInterface->deleteDraftsEntry(mOpenedMessageId.getId());
        mOpenedMessageId.setId(INVALID_MSGID);
    }
    
    mUtilsManager->deactivateInputBlocker();

    return ((msgId > INVALID_MSGID)? true : false);
}

void MsgUnifiedEditorView::resizeEvent( QGraphicsSceneResizeEvent * event )
{
 Q_UNUSED(event)
 #ifdef _DEBUG_TRACES_
 qDebug()<<"resize event";
#endif

 mContentWidget->resize(this->rect().width(),this->rect().height()+1);
}

//EOF
