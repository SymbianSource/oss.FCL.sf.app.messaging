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
 * Description:Conversation (chat) view for messaging application.
 *
 */

#include "msgconversationview.h"

// SYSTEM INCLUDES
#include <HbMenu>
#include <HbAction>
#include <HbInstance>
#include <HbApplication>
#include <HbDialog>
#include <HbLabel>
#include <HbWidget>
#include <HbListView>
#include <hbmessagebox.h>
#include <hbnotificationdialog.h>
#include <HbEffect>

#include <QDir>
#include <QLibraryInfo>
#include <QPluginLoader>
#include <QDateTime>
#include <QGraphicsLinearLayout>
#include <QStandardItemModel>

// USER INCLUDES
#include "msguiutilsmanager.h"
#include "msgconversationviewitem.h"
#include "conversationsengine.h"
#include "convergedmessageid.h"
#include "convergedmessage.h"
#include "conversationsenginedefines.h"
#include "msgcontactcardwidget.h"
#include "msgeditorwidget.h"
#include "msgviewdefines.h"
#include "debugtraces.h"
#include "unidatamodelloader.h"
#include "unidatamodelplugininterface.h"
#include "ringbc.h"

// LOCAL CONSTANTS
const QString M_SERVER("mserver");
const QString SMS("sms");

#define LOC_COMMON_OPEN hbTrId("txt_common_menu_open")
#define LOC_COMMON_DELETE hbTrId("txt_common_menu_delete")
#define LOC_COMMON_FORWARD hbTrId("txt_common_menu_forward")
#define LOC_COMMON_SAVE hbTrId("txt_common_menu_save")

#define LOC_DELETE_CONVERSATION hbTrId("txt_messaging_dialog_delete_conversation")
#define LOC_BUTTON_DELETE hbTrId("txt_common_button_delete")
#define LOC_BUTTON_CANCEL hbTrId("txt_common_button_cancel")

#define LOC_DELETE_MESSAGE hbTrId("txt_messaging_dialog_delete_message")
#define LOC_DELETE_POPINFO hbTrId("txt_messaging_dpopinfo_message_deleted")

const QString HELP(QObject::tr("Help"));

#define LOC_ADD hbTrId("txt_messaging_opt_add")
#define LOC_RECIPIENTS hbTrId("txt_messaging_opt_sub_recipients")
#define LOC_SUBJECT hbTrId("txt_messaging_opt_sub_subject")

#define LOC_ATTACH hbTrId("txt_messaging_opt_attach")
#define LOC_PHOTO hbTrId("txt_messaging_opt_attach_sub_photo")
#define LOC_VIDEO hbTrId("txt_messaging_opt_attach_sub_video")
#define LOC_SOUND hbTrId("txt_messaging_opt_attach_sub_sound")
#define LOC_MORE hbTrId("txt_messaging_opt_attach_sub_more")

const QString PLUGINPATH("conversationviewplugin.dll");

const int INVALID_MSGID = -1;
//---------------------------------------------------------------
// MsgConversationView::MsgConversationView
// @see header file
//---------------------------------------------------------------
MsgConversationView::MsgConversationView(MsgContactCardWidget *contactCardWidget,
    QGraphicsItem *parent) :
    MsgBaseView(parent),
    mConversationList(NULL),
    mMessageModel(NULL),
    mEditorWidget(NULL),
    mContactCardWidget(contactCardWidget),
    mUiUtilsManager(NULL)
{
    // Initialize the Utils manager
    mUiUtilsManager = new MsgUiUtilsManager(this);
    // connect the error signal from UtilsManager to the slot
    // in the conversation view.
    connect(mUiUtilsManager,
            SIGNAL(serviceError(const QString&)),
            this,
            SLOT(serviceError(const QString&)));

    connect(mUiUtilsManager, SIGNAL(imagesFetched(const QVariant&)),
                     this, SLOT(imagesFetched(const QVariant&)));
    connect(mUiUtilsManager, SIGNAL(audiosFetched(const QVariant&)),
                     this, SLOT(audiosFetched(const QVariant&)));

    //initialize view
    setupView();
    addMenu();
}

//---------------------------------------------------------------
// MsgConversationView::~MsgConversationView
// @see header file
//---------------------------------------------------------------
MsgConversationView::~MsgConversationView()
{
mConversationList->setModel(NULL);    
}
//---------------------------------------------------------------
// MsgConversationView::setupView
// @see header file
//---------------------------------------------------------------
void MsgConversationView::setupView()
{
    // Create HbListView and set properties
    mConversationList = new HbListView();
    style()->registerPlugin(PLUGINPATH);
    mConversationList->setLayoutName("custom");
    mConversationList->setItemRecycling(true);
    MsgConversationViewItem *item = new MsgConversationViewItem(this);
    mConversationList->setItemPrototype(item);
    mConversationList->setSelectionMode(HbListView::NoSelection);
    mConversationList->setClampingStyle(HbScrollArea::BounceBackClamping);
    mConversationList->setScrollingStyle(HbScrollArea::PanOrFlick);


    mMessageModel = ConversationsEngine::instance()->getConversationsModel();
    
    connect(ConversationsEngine::instance(), 
        SIGNAL(conversationModelPopulated()), 
        this, 
        SLOT(populateConversationsView()));
    
    connect(mConversationList, SIGNAL(activated(QModelIndex)),
        this, SLOT(openItem(QModelIndex)));

    // Long tap list item
    connect(mConversationList, SIGNAL(longPressed(HbAbstractViewItem*, QPointF)),
        this, SLOT(longPressed(HbAbstractViewItem*, QPointF)));

    // Create message editor widget.
    mEditorWidget = new MsgEditorWidget();

    connect(mEditorWidget, SIGNAL(sendMessage()), this, SLOT(send()));
    connect(mEditorWidget, SIGNAL(replyStarted()), this, SIGNAL(replyStarted()));

    QGraphicsLinearLayout *mainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mainLayout->setContentsMargins(CONTENT_MARGIN, CONTENT_MARGIN,
        CONTENT_MARGIN, CONTENT_MARGIN);
    mainLayout->setSpacing(CONTENT_SPACING);
    mainLayout->addItem(mConversationList);
    mainLayout->addItem(mEditorWidget);
    setLayout(mainLayout);

    // Refresh view to show the header details
    refreshView();
    
    HbEffect::add(QString("ConversationView"), 
                      QString(":/xml/conversationview_effect.fxml"), 
                      QString("show") );
}

//---------------------------------------------------------------
// MsgConversationView::addMenu
// @see header file
//---------------------------------------------------------------
void MsgConversationView::addMenu()
{
    HbMenu *mainMenu = new HbMenu();

    // Clear conversation 
    HbAction* clearConversation = mainMenu->addAction(LOC_COMMON_DELETE);
    connect(clearConversation, SIGNAL(triggered()), this, SLOT(deleteAll()));

    // Attach sub-menu
    HbMenu *attachSubMenu = mainMenu->addMenu(LOC_ATTACH);

    HbAction *addPhoto = attachSubMenu->addAction(LOC_PHOTO);
    connect(addPhoto, SIGNAL(triggered()),mUiUtilsManager, SLOT(fetchImages()));

    HbAction *addVideo = attachSubMenu->addAction(LOC_VIDEO);
    connect(addVideo, SIGNAL(triggered()),mUiUtilsManager, SLOT(fetchVideo()));

    HbAction *addSound = attachSubMenu->addAction(LOC_SOUND);
    connect(addSound, SIGNAL(triggered()),mUiUtilsManager, SLOT(fetchAudio()));

    HbAction *addOther = attachSubMenu->addAction(LOC_MORE);
    connect(addOther, SIGNAL(triggered()),mUiUtilsManager, SLOT(fetchOther()));

    HbMenu *addSubMenu = mainMenu->addMenu(LOC_ADD);
    
    HbAction *addRecipients = addSubMenu->addAction(LOC_RECIPIENTS);
    connect(addRecipients, SIGNAL(triggered()), this, SLOT(addRecipients()));

    HbAction *addSubject = addSubMenu->addAction(LOC_SUBJECT);
    connect(addSubject, SIGNAL(triggered()),this, SLOT(addSubject()));

    // Help
    HbAction* help = mainMenu->addAction(HELP);
    connect(help, SIGNAL(triggered()),this, SLOT(launchHelp()));

    setMenu(mainMenu);
}

//---------------------------------------------------------------
// MsgConversationView::refreshView()
// @see header file
//---------------------------------------------------------------
void MsgConversationView::refreshView()
{
    mContactCardWidget->updateContents();
    mContactCardWidget->resize(mContactCardWidget->boundingRect().size());
    scrollToBottom();
}

//---------------------------------------------------------------
// MsgConversationView::scrollToBottom()
// @see header file
//---------------------------------------------------------------
void MsgConversationView::scrollToBottom()
{
    const int rowCnt = mMessageModel->rowCount();
    mConversationList->scrollTo(
            mMessageModel->index(rowCnt - 1, 0));
}

//---------------------------------------------------------------
// MsgConversationView::longPressed
// @see header file
//---------------------------------------------------------------
void MsgConversationView::longPressed(HbAbstractViewItem* viewItem,
    const QPointF& point)
{
    MsgConversationViewItem* item = qgraphicsitem_cast<
            MsgConversationViewItem *> (viewItem);

    bool inSide = false;

    if (item)
    {
        inSide = item->containsPoint(point);
    }

    //If point is within the item then show the item-specific menu
    if (this->isVisible() && inSide)
    {
        //If message is in Sending state or is Scheduled to be sent later,
        //do not allow any operations on the message
        int sendingState = item->modelIndex().data(SendingState).toInt();
        if(sendingState == ConvergedMessage::Suspended || 
                sendingState == ConvergedMessage::Resend || 
                sendingState == ConvergedMessage::Scheduled ||
                sendingState == ConvergedMessage::Sending ||
                sendingState == ConvergedMessage::Waiting)
        {
            return;
        }
        
        // Create new menu
        HbMenu *contextMenu = new HbMenu();
        
        int messageType = item->modelIndex().data(MessageType).toInt();  
             
        // Add the menu items
        HbAction *contextItem4 = contextMenu->addAction(LOC_COMMON_OPEN);
        connect(contextItem4, SIGNAL(triggered()),
            this, SLOT(openItem()));

        HbAction *contextItem3 = contextMenu->addAction(LOC_COMMON_DELETE);
        connect(contextItem3, SIGNAL(triggered()),
                this, SLOT(deleteItem()));
        
        // for provisioning messages don't add forward option
        if(messageType != ConvergedMessage::Provisioning &&
                messageType != ConvergedMessage::RingingTone)
            {
            HbAction *contextItem2 = contextMenu->addAction(LOC_COMMON_FORWARD);
            connect(contextItem2, SIGNAL(triggered()),
                    this, SLOT(forwardMessage()));
            }

           
        contextMenu->exec(point);

        // Cleanup
        delete contextMenu;
    }
}

//---------------------------------------------------------------
// MsgEditorPrivate::send
// @see header
//---------------------------------------------------------------
void MsgConversationView::send()
{
    mUiUtilsManager->activateInputBlocker(this);
    ConvergedMessageAddressList addresses;
    addresses = mContactCardWidget->address();
    int sendResult = KErrNone;

    // Populate the ConvergedMessage.
    if (!addresses.isEmpty())
    {
        ConvergedMessage msg;
        populateForSending(msg);
        msg.addToRecipients(addresses);

        // Send
        sendResult = mUiUtilsManager->send(msg);
        if( sendResult == KErrNone)
        {
            mEditorWidget->clear();
        }
    }
    mUiUtilsManager->deactivateInputBlocker();
    if( sendResult == KErrNotFound)
    {
        //switch to settings view
        QVariantList param;
        param << MsgBaseView::MSGSETTINGS;
        param << MsgBaseView::CV;
        emit switchView(param);
    }
}

//---------------------------------------------------------------
// MsgConversationView::deleteAll
// Deletes all the conversations
//---------------------------------------------------------------
void MsgConversationView::deleteAll()
    {
    bool result = HbMessageBox::question(LOC_DELETE_CONVERSATION,
                             LOC_BUTTON_DELETE,
                             LOC_BUTTON_CANCEL);
    if(result)
        {      
        ConversationsEngine* engine = ConversationsEngine::instance();    
        engine->deleteConversations(engine->getCurrentConversationId());
        //show dialog
        HbNotificationDialog::launchDialog("Conversation Deleted");
        //switch to CLV
        QVariantList param;
        param << MsgBaseView::CLV; // target view
        param << MsgBaseView::CV;  // source view
        emit switchView(param);
        }
    }

//---------------------------------------------------------------
// MsgConversationView::addPhoto()
// @see header file
//---------------------------------------------------------------
void MsgConversationView::imagesFetched(const QVariant& result )
{
    if(result.canConvert<QStringList>())
    {
        QStringList fileList = result.value<QStringList>();
        if ( fileList.size()>0 )
        {
            QString filepath(fileList.at(0));
            launchUniEditor(MsgBaseView::ADD_PHOTO, filepath);
        }
    }
}

//---------------------------------------------------------------
// MsgConversationView::audiosFetched()
// @see header file
//---------------------------------------------------------------
void MsgConversationView::audiosFetched(const QVariant& result )
{
    if(result.canConvert<QStringList>())
    {
        QStringList fileList = result.value<QStringList>();
        if ( fileList.size()>0 && !fileList.at(0).isEmpty())
        {
            QString filepath(QDir::toNativeSeparators(fileList.at(0)));
            launchUniEditor(MsgBaseView::ADD_AUDIO, filepath);
        }
    }
}

//---------------------------------------------------------------
// MsgConversationView::addRecipients()
// @see header file
//---------------------------------------------------------------
void MsgConversationView::addRecipients()
{
    QString filepath;
    launchUniEditor(MsgBaseView::ADD_RECIPIENTS, filepath);
}

//---------------------------------------------------------------
// MsgConversationView::addSubject()
// @see header file
//---------------------------------------------------------------
void MsgConversationView::addSubject()
{

    QString filepath;
    launchUniEditor(MsgBaseView::ADD_SUBJECT, filepath);
}

//---------------------------------------------------------------
// MsgConversationView::launchHelp()
// Launches the help content
//---------------------------------------------------------------
void MsgConversationView::launchHelp()
{
}

//---------------------------------------------------------------
// MsgConversationView::exitApp()
// Exits the application
//---------------------------------------------------------------
void MsgConversationView::exitApp()
{
    HbApplication::quit();
}

//---------------------------------------------------------------
// MsgConversationView::forwardMessage()
// Forwards the message
//---------------------------------------------------------------
void MsgConversationView::forwardMessage()
{
}

//---------------------------------------------------------------
// MsgConversationView::deleteItem()
// Deletes the message
//---------------------------------------------------------------
void MsgConversationView::deleteItem()
    {
    QModelIndex index = mConversationList->currentIndex();
    int count = mMessageModel->rowCount();
    //delete message
    qint32 messageId = index.data(ConvergedMsgId).toLongLong();    

    bool result = HbMessageBox::question(LOC_DELETE_MESSAGE,
                             LOC_BUTTON_DELETE,
                             LOC_BUTTON_CANCEL);
    if(result)
        {        
        if (messageId)
            {
            QList<int> msgIdList;
            msgIdList.append(messageId);          
            ConversationsEngine::instance()->deleteMessages(msgIdList);
            HbNotificationDialog::launchDialog(LOC_DELETE_POPINFO);
            //switch view               
            if (count == 1)
                {
                QVariantList param;
                param << MsgBaseView::CLV; // target view
                param << MsgBaseView::CV; // source view
                emit switchView(param);
                }  
            }             
        }
}

//---------------------------------------------------------------
// MsgConversationView::openItem()
// Opens the message
//---------------------------------------------------------------
void MsgConversationView::openItem()
{
    QModelIndex index = mConversationList->currentIndex();
    openItem(index);
}

//---------------------------------------------------------------
// MsgConversationView::setMessageData()
// Set message data
//---------------------------------------------------------------
void MsgConversationView::setMessageData(const ConvergedMessage &message)
{
    mEditorWidget->setContent(message.bodyText());
}

//---------------------------------------------------------------
// MsgConversationView::clearEditors()
// @See header
//---------------------------------------------------------------
void MsgConversationView::clearEditors()
{
    mEditorWidget->clear();
    mConversationList->setModel(NULL);
    mContactCardWidget->clearContent();
}

//---------------------------------------------------------------
// MsgConversationView::saveContentToDrafts()
// @See header
//---------------------------------------------------------------
bool MsgConversationView::saveContentToDrafts()
{
    int msgId = INVALID_MSGID;
    if(!mEditorWidget->content().isEmpty())
    {
        mUiUtilsManager->activateInputBlocker(this);
        ConvergedMessageAddressList addresses;
        addresses = mContactCardWidget->address();

        // Populate the ConvergedMessage.
        if (!addresses.isEmpty())
        {
            ConvergedMessage msg;
            populateForSending(msg);
            msg.addToRecipients(addresses);

            // Send
            msgId = mUiUtilsManager->saveToDrafts(msg);

            mEditorWidget->clear();
        }
        mUiUtilsManager->deactivateInputBlocker();
    }
    return ((msgId > INVALID_MSGID)? true : false);
}

//---------------------------------------------------------------
// MsgConversationView::loadCommandPlugin()
// @See header
//---------------------------------------------------------------
bool MsgConversationView::loadCommandPlugin()
{
    /*
#ifdef _DEBUG
    QDir dir(QLibraryInfo::location(QLibraryInfo::PluginsPath));
    dir.cd("messaging");
#else
    QDir dir("Z:/resource/qt/plugins/messaging");
#endif
#ifdef _DEBUG_TRACES_
    qDebug() << "MsgConversationView::plugins path" << dir.path();
#endif

    // load the plugins
    QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Readable);
    foreach (QFileInfo entry, entries)
        {
            QPluginLoader loader(entry.absoluteFilePath());
#ifdef _DEBUG_TRACES_
            qDebug() << "MsgConversationView::absolute path"
                    << entry.absoluteFilePath();
#endif
            mMsgCmdPlugin = qobject_cast<MsgCommandPluginInterface*> (
                loader.instance());
            if (mMsgCmdPlugin)
            {
                return true;
            }
        }*/
    return false;
}

//---------------------------------------------------------------
//MsgConversationView::parseServiceId()
//@see header
//---------------------------------------------------------------
QString MsgConversationView::parseServiceId(const QString &serviceId)
{
    QStringList services = serviceId.split('.');
    // the service id is in the form "messaging.mserver.mms"
    // split the string by '.' and compare the middle part
    // by the server names.
    return services[1];
}

//---------------------------------------------------------------
//MsgConversationView::currentMessageType()
//@see header
//---------------------------------------------------------------
int MsgConversationView::currentMessageType()
{
    QString serviceId = mEditorWidget->serviceId();
    QString sendClientType = parseServiceId(serviceId);

    ConvergedMessage::MessageType messageType = ConvergedMessage::None;
    if ( (M_SERVER == sendClientType) && serviceId.contains(SMS))
    {
        messageType = ConvergedMessage::Sms;
    }
    return messageType;
}
//---------------------------------------------------------------
//MsgConversationView::populateForSending()
//@see header
//---------------------------------------------------------------
void MsgConversationView::populateForSending(ConvergedMessage &message)
{
    ConvergedMessage::MessageType messageType =
            static_cast<ConvergedMessage::MessageType> (currentMessageType());

    if (ConvergedMessage::Sms == messageType)
    {
        message.setMessageType(messageType);
        message.setBodyText(mEditorWidget->content());
        message.setDirection(ConvergedMessage::Outgoing);
        QDateTime time = QDateTime::currentDateTime();
        message.setTimeStamp(time.toTime_t());
    }
}

//---------------------------------------------------------------
// MsgConversationView::serviceError()
// @See header
//---------------------------------------------------------------
void MsgConversationView::serviceError(const QString& error)
{
    Q_UNUSED(error)
//    HbDialog popup;
//
//    popup.setDismissPolicy(HbDialog::TapAnywhere);
//    popup.setHeadingWidget(new HbLabel(tr("Error Notification")));
//
//    /// set a label as the content widget in the popup.
//    HbLabel contentLabel(error);
//    popup.setContentWidget(&contentLabel);
//
//    popup.setPrimaryAction(new HbAction(tr("Ok"), &popup));
//    popup.exec();
}

//---------------------------------------------------------------
//MsgConversationView::openItem
//@see header
//---------------------------------------------------------------
void MsgConversationView::openItem(const QModelIndex & index)
    {
    int messageType = index.data(MessageType).toInt();    
    if(messageType == ConvergedMessage::RingingTone )
        {
        if(RingBc::askSaveQuery())
            {
            saveRingingTone();
            }
        return;
        }
	
	/// smart messages hardcoded due to common handling
	if (messageType == ConvergedMessage::BioMsg
	    || messageType >= ConvergedMessage::VCard)  
	{
	    return;
	}
			
    ConvergedMessage message;
    // check whether message is in sending progress, then donot launch viewer.
    int location = index.data(MessageLocation).toInt();
    int sendingState = index.data(SendingState).toInt();
    
    //If message is in Sending state or is Scheduled to be sent later,
    //do not open the message
    if(sendingState == ConvergedMessage::Suspended ||
            sendingState == ConvergedMessage::Resend ||
            sendingState == ConvergedMessage::Scheduled ||
            sendingState == ConvergedMessage::Sending ||
            sendingState == ConvergedMessage::Waiting)
    {
        return;
    }

    //TODO: sendingState check should be removed once msg
    // movement lock issue is resolved from mw
    if( !((location == ConvergedMessage::Inbox) ||
        (location == ConvergedMessage::Sent) ||
        (sendingState == ConvergedMessage::SentState)) )
        {// do not launch viewer, show a note
        HbNotificationDialog* dlg = new HbNotificationDialog();
        dlg->setFocusPolicy(Qt::NoFocus);
        QString text(tr("Message Locked"));
        dlg->setText(text);
        dlg->setDismissPolicy(HbPopup::TapAnywhere);
        dlg->setAttribute(Qt::WA_DeleteOnClose, true);
        dlg->show();
        return;
		}

        // message id
        qint32 messageId = index.data(ConvergedMsgId).toLongLong();
        ConvergedMessageId msgId(messageId);
        message.setMessageId(msgId);

        // contact Id
        qint32 contactId = index.data(ContactId).toLongLong();

        // message type

        message.setMessageType((ConvergedMessage::MessageType) messageType);

        if (messageType == ConvergedMessage::Mms)
        {
            message.setSubject(index.data(Subject).toString());
            message.setPriority((ConvergedMessage::Priority) index.data(
                MessagePriority).toInt());
        }
        else if (messageType == ConvergedMessage::IM)
        {
            // add body text and send to the message for IM case
            message.setBodyText(index.data(BodyText).toString());
        }

        // time stamp
        message.setTimeStamp(index.data(TimeStamp).toLongLong());

        //Message direction
        message.setDirection((ConvergedMessage::Direction)index.data(Direction).toInt());

        // conatct name as address
        ConvergedMessageAddress address;
        address.setAlias(mContactCardWidget->address().at(0)->alias());
        address.setAddress(mContactCardWidget->address().at(0)->address());
        message.addToRecipient(address);

        // Mark this message as read if its unread
        QByteArray dataArray;
        QDataStream messageStream
        (&dataArray, QIODevice::WriteOnly | QIODevice::Append);
        message.serialize(messageStream);
        //if message unread, mark as read before opening view
        QList<int> msgIdList;
        if(index.data(UnReadStatus).toInt())
            {
        msgIdList.clear();
        msgIdList << messageId;
        ConversationsEngine::instance()->markMessagesRead(msgIdList);
        }

        //switch view
        QVariantList param;
        param << MsgBaseView::UNIVIEWER;  // target view
        param << MsgBaseView::CV; // source view

        param << contactId;
        param << dataArray;
        param << mMessageModel->rowCount();

        emit switchView(param);
}

//---------------------------------------------------------------
// MsgConversationView::launchUniEditor
// @see header file
//---------------------------------------------------------------
void MsgConversationView::launchUniEditor(const int editorFields, QString& filepath)
{
    ConvergedMessage message;
    message.setBodyText(mEditorWidget->content());

    ConvergedMessageAddress address;
    address.setAlias(mContactCardWidget->address().at(0)->alias());
    address.setAddress(mContactCardWidget->address().at(0)->address());
    message.addToRecipient(address);

    if(!filepath.isEmpty())
    {
        ConvergedMessageAttachment* attachment = new ConvergedMessageAttachment(filepath);
        ConvergedMessageAttachmentList attachmentList;
        attachmentList.append(attachment);
        message.addAttachments(attachmentList);
    }

    QByteArray dataArray;
    QDataStream messageStream
    (&dataArray, QIODevice::WriteOnly | QIODevice::Append);
    message.serialize(messageStream);

    QVariantList params;
    params << MsgBaseView::UNIEDITOR; // target view
    params << MsgBaseView::CV; // source view

    params << dataArray;
    params << editorFields;

    clearEditors();
    emit switchView(params);
}

//---------------------------------------------------------------
// MsgConversationView::populateConversationsView
// @see header file
//---------------------------------------------------------------
void MsgConversationView::populateConversationsView()
{
    connect(mMessageModel, SIGNAL(conversationModelUpdated()),
        this, SLOT(refreshView()));

    mConversationList->setModel(mMessageModel);
    
    refreshView();
    
    HbEffect::start(this,QString("ConversationView"),
        QString("show"),this,"effectFinished");
}

//---------------------------------------------------------------
// MsgConversationView::effectFinished
// @see header file
//---------------------------------------------------------------
void MsgConversationView::effectFinished(
    const HbEffect::EffectStatus& /*status*/)
  {
  // Effect is finished. Start fetching remaing conversations
    ConversationsEngine::instance()->fetchMoreConversations();
  }

//---------------------------------------------------------------
// MsgConversationView::saveRingingTone
// @see header file
//---------------------------------------------------------------
void MsgConversationView::saveRingingTone()
    {
    QModelIndex index = mConversationList->currentIndex();
    int messageId = index.data(ConvergedMsgId).toInt();
    
    UniDataModelLoader* pluginLoader = new UniDataModelLoader();
    pluginLoader->loadPlugins();
    UniDataModelPluginInterface* pluginInterface = 
                        pluginLoader->getDataModelPlugin("bio:vcal");
    pluginInterface->setMessageId(messageId);
    UniMessageInfoList attachments = pluginInterface->attachmentList();
    
    QString attachmentPath = attachments[0]->path();
    
    RingBc* ringBc = new RingBc();
    
    ringBc->saveTone(attachmentPath);
      
    // clear attachement list : its allocated at data model
    while(!attachments.isEmpty())
        {
        delete attachments.takeFirst();
        }

    delete ringBc;
    delete pluginLoader;
    }

// EOF
