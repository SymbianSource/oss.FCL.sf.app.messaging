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
#include <HbListView>
#include <HbMessageBox>
#include <HbNotificationDialog>
#include <XQServiceRequest.h>
#include <HbStaticVkbHost>
#include <xqaiwrequest.h>
#include <xqappmgr.h>

#include <QDir>
#include <QDateTime>
#include <QGraphicsLinearLayout>
#include <QStandardItemModel>

#include <cntservicescontact.h>
#include <ccsdefs.h>
#include <centralrepository.h>
#include <MmsEngineDomainCRKeys.h>

// USER INCLUDES
#include "msgsendutil.h"
#include "msgconversationviewitem.h"
#include "conversationsengine.h"
#include "convergedmessageid.h"
#include "conversationsenginedefines.h"
#include "msgcontactcardwidget.h"
#include "msgeditorwidget.h"
#include "msgviewdefines.h"
#include "debugtraces.h"
#include "unidatamodelloader.h"
#include "unidatamodelplugininterface.h"
#include "ringbc.h"
#include "mmsconformancecheck.h"

//Item specific menu.

#define LOC_COMMON_OPEN hbTrId("txt_common_menu_open")
#define LOC_COMMON_DELETE hbTrId("txt_common_menu_delete")
#define LOC_COMMON_FORWARD hbTrId("txt_common_menu_forward")
#define LOC_COMMON_DOWNLOAD hbTrId("txt_messaging_menu_download")
#define LOC_COMMON_SEND  hbTrId("txt_common_menu_send") 

#define LOC_BUTTON_DELETE hbTrId("txt_common_button_delete")
#define LOC_BUTTON_CANCEL hbTrId("txt_common_button_cancel")
#define LOC_BUTTON_OK hbTrId("txt_common_button_ok")

#define LOC_DELETE_MESSAGE hbTrId("txt_messaging_dialog_delete_message")

//main menu
#define LOC_ATTACH          hbTrId("txt_messaging_opt_attach")
#define LOC_PHOTO           hbTrId("txt_messaging_opt_attach_sub_photo")
#define LOC_SOUND           hbTrId("txt_messaging_opt_attach_sub_sound")
#define LOC_VCARD           hbTrId("txt_messaging_list_business_card")
#define LOC_ADD_RECIPIENTS  hbTrId("txt_messaging_opt_add_recipients")
#define LOC_ADD_SUBJECT     hbTrId("txt_messaging_opt_add_subject")

#define LOC_MSG_SEND_FAILED hbTrId("txt_messaging_dpopinfo_sending_failed")


const QString PLUGINPATH("conversationviewplugin.dll");

const int INVALID_MSGID = -1;
const int INVALID_CONVID = -1;
const int CONTACT_INSERTION_MODE = 1;
const int VCARD_INSERTION_MODE = 0;

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
    mSendUtil(NULL),
    mItemLongPressed(false),
    mVkbHost(NULL),
    mVkbopened(false)
{
    //create send utils
    mSendUtil = new MsgSendUtil(this);
    //initialize view
    setupView();
    setupMenu();
}

//---------------------------------------------------------------
// MsgConversationView::~MsgConversationView
// @see header file
//---------------------------------------------------------------
MsgConversationView::~MsgConversationView()
{
    
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

    // Create message editor widget, will be displayed based on msg type.
    mEditorWidget = new MsgEditorWidget(this);
    mEditorWidget->hide();

    connect(mEditorWidget, SIGNAL(sendMessage()), this, SLOT(send()));
    connect(mEditorWidget, SIGNAL(smsCharLimitReached()), 
        this, SLOT(handleSmsCharLimitReached()));
    connect(mEditorWidget, SIGNAL(replyStarted()), this, SIGNAL(replyStarted()));

    qreal spacing = HbDeviceProfile::profile(mConversationList).unitValue();
    
    mMainLayout = new QGraphicsLinearLayout(Qt::Vertical,this); 
    
    mMainLayout->setContentsMargins(CONTENT_MARGIN, CONTENT_MARGIN,
                                    CONTENT_MARGIN, CONTENT_MARGIN);   
    mMainLayout->setSpacing(spacing);

    mMainLayout->addItem(mConversationList);

    setLayout(mMainLayout);

    //Create VKB instance and listen to VKB open and close signals for resizing the view.
    mVkbHost = new HbStaticVkbHost(this);
	connect(mVkbHost, SIGNAL(keypadOpened()), this, SLOT(vkbOpened()));
	connect(mVkbHost, SIGNAL(keypadClosed()), this, SLOT(vkbClosed()));

    // Refresh view to show the header details
    refreshView();
}

//---------------------------------------------------------------
// MsgConversationView::addMenu
// @see header file
//---------------------------------------------------------------
void MsgConversationView::setupMenu()
{
    // Just create dummy menu.
    // Actual menu will be created in menuAboutToShow()
    HbMenu *mainMenu = this->menu();
    HbAction* clearConversation = mainMenu->addAction(QString());
    connect(mainMenu, SIGNAL(aboutToShow()), this, SLOT(menuAboutToShow()));
}

//---------------------------------------------------------------
// MsgConversationView::refreshView()
// @see header file
//---------------------------------------------------------------
void MsgConversationView::refreshView()
{
    // Hide editor in case of BT conversations.
    qint64 convId = ConversationsEngine::instance()->getCurrentConversationId();
    if (INVALID_CONVID != convId) {
        if (KBluetoothMsgsConversationId == convId) {
            mMainLayout->removeItem(mEditorWidget);
            mEditorWidget->hide();
            mEditorWidget->setParent(this);
        }
        else {
            mMainLayout->addItem(mEditorWidget);
            mEditorWidget->setEncodingSettings();
            mEditorWidget->show();
        }
        mContactCardWidget->updateContents();
    }
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
    mItemLongPressed = true;
    
    MsgConversationViewItem* item = qgraphicsitem_cast<
            MsgConversationViewItem *> (viewItem);

    // Show the item-specific menu
    if (this->isVisible())
    {
        //If message is in Sending state or is Scheduled to be sent later,
        //do not allow any operations on the message
        int sendingState = item->modelIndex().data(SendingState).toInt();
        // Create new menu
        HbMenu* contextMenu = new HbMenu();
        setContextMenu(item, contextMenu, sendingState);
        
        //Before showing menu reset the flag
        mItemLongPressed = false;
        contextMenu->exec(point);
        // Cleanup
        delete contextMenu;

    }
    else
    {
        // For cases where the long tap check failed
        mItemLongPressed = false;
    }
}

//---------------------------------------------------------------
// MsgConversationView::setContextMenu
// @see header
//---------------------------------------------------------------

void MsgConversationView::setContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState)
{
    addOpenItemToContextMenu(item , contextMenu,sendingState);
    addResendItemToContextMenu(item, contextMenu, sendingState);
    addForwardItemToContextMenu(item, contextMenu, sendingState);
    addDownloadItemToContextMenu(item, contextMenu);
    addDeleteItemToContextMenu(item, contextMenu, sendingState);  
}

//---------------------------------------------------------------
// MsgEditorPrivate::addOpenItemToContextMenu
// @see header
//---------------------------------------------------------------

void MsgConversationView::addOpenItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState)
{
    int direction = item->modelIndex().data(Direction).toInt();
    int messageType = item->modelIndex().data(MessageType).toInt();
    int messageSubType = item->modelIndex().data(MessageSubType).toInt();
    
    if((messageSubType == ConvergedMessage::VCal) ||
        (messageSubType == ConvergedMessage::RingingTone) ||
        (messageType == ConvergedMessage::MmsNotification))
        {
        return;
        }
    
    if( (sendingState == ConvergedMessage::SentState ) ||
        (sendingState == ConvergedMessage::Resend ) ||
        (sendingState == ConvergedMessage::Failed ) ||
        (direction == ConvergedMessage::Incoming))
    {
        HbAction *contextItem = contextMenu->addAction(LOC_COMMON_OPEN);
        connect(contextItem, SIGNAL(triggered()),this, SLOT(openItem()));
    }
    
}

//---------------------------------------------------------------
// MsgEditorPrivate::addResendItemToContextMenu
// @see header
//---------------------------------------------------------------

void MsgConversationView::addResendItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState)
{
    Q_UNUSED(item)
    int direction = item->modelIndex().data(Direction).toInt();
    
    if( (direction == ConvergedMessage::Outgoing)&&
        ((sendingState == ConvergedMessage::Resend ) ||
        (sendingState == ConvergedMessage::Suspended )||
        (sendingState == ConvergedMessage::Failed )))
    {
        HbAction *contextItem = contextMenu->addAction(LOC_COMMON_SEND);
        connect(contextItem, SIGNAL(triggered()),this, SLOT(resendMessage()));
    }
}

//---------------------------------------------------------------
// MsgEditorPrivate::addForwardItemToContextMenu
// @see header
//---------------------------------------------------------------
void MsgConversationView::addForwardItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState)
{
    int messageType = item->modelIndex().data(MessageType).toInt();
    int direction = item->modelIndex().data(Direction).toInt();
    int messageSubType = item->modelIndex().data(MessageSubType).toInt();
    
    if( (messageType == ConvergedMessage::BT) ||
        (messageType == ConvergedMessage::MmsNotification) ||
        (messageSubType == ConvergedMessage::Provisioning ) ||
        (messageSubType == ConvergedMessage::RingingTone) ||
        (messageSubType == ConvergedMessage::VCal))
    {
        return;
    }
    
    qint32 messageId = item->modelIndex().data(ConvergedMsgId).toInt();

    if( ((sendingState == ConvergedMessage::SentState) ||
        (sendingState == ConvergedMessage::Resend) ||
        (sendingState == ConvergedMessage::Failed) ||
        (direction == ConvergedMessage::Incoming) ) &&
        (validateMsgForForward(messageType,messageId))	)
    {
        HbAction *contextItem = contextMenu->addAction(LOC_COMMON_FORWARD);
        connect(contextItem, SIGNAL(triggered()),this, SLOT(forwardMessage()));
    }

}

//---------------------------------------------------------------
// MsgEditorPrivate::addDeleteItemToContextMenu
// @see header
//---------------------------------------------------------------
void MsgConversationView::addDeleteItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu, int sendingState)
{
    int direction = item->modelIndex().data(Direction).toInt();
    int messageType = item->modelIndex().data(MessageType).toInt();
    int notificationState = item->modelIndex().data(NotificationStatus).toInt();
    
    if( (messageType == ConvergedMessage::MmsNotification) && 
        ((notificationState == ConvergedMessage::NotifNull) || 
         (notificationState ==  ConvergedMessage::NotifRetrieving) ||
         (notificationState == ConvergedMessage::NotifWaiting)))
    {
        return;
    }
    
    if( (sendingState == ConvergedMessage::SentState) ||
        (sendingState == ConvergedMessage::Resend) ||
        (sendingState == ConvergedMessage::Suspended) ||
        (sendingState == ConvergedMessage::Failed) ||
        (direction == ConvergedMessage::Incoming))
    {
        HbAction *contextItem = contextMenu->addAction(LOC_COMMON_DELETE);
        connect(contextItem, SIGNAL(triggered()),this, SLOT(deleteItem()));
    }  
}

//---------------------------------------------------------------
// MsgEditorPrivate::addDownloadItemToContextMenu
// @see header
//---------------------------------------------------------------
void MsgConversationView::addDownloadItemToContextMenu(MsgConversationViewItem* item, HbMenu* contextMenu)
{
    int notificationState = item->modelIndex().data(NotificationStatus).toInt();
    int messageType = item->modelIndex().data(MessageType).toInt();
    qint32 messageId = item->modelIndex().data(ConvergedMsgId).toLongLong();
        
    if( messageType == ConvergedMessage::MmsNotification &&
        ConversationsEngine::instance()->downloadOperationSupported(messageId))
    {            
        HbAction *contextItem = contextMenu->addAction(LOC_COMMON_DOWNLOAD);
        connect(contextItem, SIGNAL(triggered()),this, SLOT(downloadMessage()));
    }
}
//---------------------------------------------------------------
// MsgEditorPrivate::send
// @see header
//---------------------------------------------------------------
void MsgConversationView::send()
{
    activateInputBlocker();
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
        sendResult = mSendUtil->send(msg);
                   
        if( sendResult == KErrNone)
        {
            mEditorWidget->clear();
        }
    }
    deactivateInputBlocker();
    if( sendResult == KErrNotFound)
    {
    bool result = HbMessageBox::question("SMS Settings not defined\nDefine now ?",
            LOC_BUTTON_OK,
            LOC_BUTTON_CANCEL);
        if (result)
        {
            //switch to settings view
            QVariantList param;
            param << MsgBaseView::MSGSETTINGS;
            param << MsgBaseView::CV;
            emit switchView(param);
        }
    }
}

//---------------------------------------------------------------
// MsgConversationView::contactsFetchedForVCards
// @see header file
//---------------------------------------------------------------
void MsgConversationView::contactsFetchedForVCards(const QVariant& value)
{
    // get received contact-list and launch unieditor
    CntServicesContactList contactList = 
            qVariantValue<CntServicesContactList>(value);
    int cntCount = contactList.count();
    if(cntCount > 0)
    {
        QVariantList params;
        params << MsgBaseView::ADD_VCARD;
        params << value;
        launchUniEditor(params);        
    }
}

//---------------------------------------------------------------
// MsgConversationView::fetchContacts
// @see header file
//---------------------------------------------------------------
void MsgConversationView::fetchContacts()
{
    HbAction* action = qobject_cast<HbAction*>(sender());

    if(!action)
        return;

    QList<QVariant> args;
    QString serviceName("com.nokia.services.phonebookservices");
    QString operation("fetch(QString,QString,QString)");
    XQAiwRequest* request;
    XQApplicationManager appManager;
    request = appManager.create(serviceName, "Fetch", operation, true); //embedded
    if ( request == NULL )
    {
        return;       
    }

    int mode = action->data().toInt();
    
    if( VCARD_INSERTION_MODE == mode) //vcard-insert mode
    {
        connect(request, SIGNAL(requestOk(const QVariant&)),
            this, SLOT(contactsFetchedForVCards(const QVariant&)));      
    }
    else  //contact-insert mode
    {
        connect(request, SIGNAL(requestOk(const QVariant&)),
            this, SLOT(contactsFetched(const QVariant&)));
    }
    connect (request, SIGNAL(requestError(int,const QString&)), 
        this, SLOT(serviceRequestError(int,const QString&)));

    args << QString(tr("Phonebook")); 
    args << KCntActionAll;
    args << KCntFilterDisplayAll;

    request->setArguments(args);
    request->send();
    delete request;
}
//---------------------------------------------------------------
// MsgConversationView::fetchImages
// @see header file
//---------------------------------------------------------------
void MsgConversationView::fetchImages()
{
    QString interface("Image");
    QString operation("fetch(QVariantMap,QVariant)");
    XQAiwRequest* request = NULL;
    XQApplicationManager appManager;
    request = appManager.create(interface, operation, true);//embedded
    if(!request)
    {
        QDEBUG_WRITE("AIW-ERROR: NULL request");
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
// MsgConversationView::fetchAudio
// @see header file
//---------------------------------------------------------------
void MsgConversationView::fetchAudio()
{
    QString service("Music Fetcher");
    QString interface("com.nokia.services.media.Music");
    QString operation("fetch(QString)");
    XQAiwRequest* request = NULL;
    XQApplicationManager appManager;
    request = appManager.create(service, interface, operation, true); //embedded
    if(!request)
    {
        QDEBUG_WRITE("AIW-ERROR: NULL request");
        return;
    }

    connect(request, SIGNAL(requestOk(const QVariant&)),
        this, SLOT(audiosFetched(const QVariant&)));
    connect(request, SIGNAL(requestError(int,const QString&)),
        this, SLOT(serviceRequestError(int,const QString&)));

    // Make the request
    if (!request->send())
    {
        QDEBUG_WRITE_FORMAT("AIW-ERROR: Request Send failed  ",request->lastError());
    }
    delete request;
}

//---------------------------------------------------------------
// MsgConversationView::contactsFetched
// @see header file
//---------------------------------------------------------------
void MsgConversationView::contactsFetched(const QVariant& value)
{
    CntServicesContactList contactList;
    contactList = qVariantValue<CntServicesContactList>(value);
    ConvergedMessageAddressList addresses;
	int count = contactList.count();
	if(count > 0)
      {
	  ConvergedMessageAddress* address = new ConvergedMessageAddress;
      address->setAlias(mContactCardWidget->address().at(0)->alias());
      address->setAddress(mContactCardWidget->address().at(0)->address());
      addresses.append(address);
    	for(int i = 0; i < contactList.count(); i++ )
	    {
	    ConvergedMessageAddress* address = new ConvergedMessageAddress;
	    address->setAlias(contactList[i].mDisplayName);
	    address->setAddress(contactList[i].mPhoneNumber);
	    addresses.append(address);
	    }

      ConvergedMessage message;
      message.setBodyText(mEditorWidget->content());
      message.addToRecipients(addresses);//takes the ownership of list elements
      QByteArray dataArray;
      QDataStream messageStream
      (&dataArray, QIODevice::WriteOnly | QIODevice::Append);
      message.serialize(messageStream);
      QVariantList params;
      params << MsgBaseView::UNIEDITOR; // target view
      params << MsgBaseView::CV; // source view
      params << dataArray;
      clearEditors();
      emit switchView(params);
     }    
}

//---------------------------------------------------------------
// MsgConversationView::imagesFetched()
// @see header file
//---------------------------------------------------------------
void MsgConversationView::imagesFetched(const QVariant& result )
{
    if(result.canConvert<QStringList>())
    {
        QStringList fileList = result.value<QStringList>();
        if ( fileList.size()>0 )
        {
            QString filepath(QDir::toNativeSeparators(fileList.at(0)));
            QVariantList params;
            params << MsgBaseView::ADD_PHOTO;
            params << filepath;
            launchUniEditor(params);
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
            QVariantList params;
            params << MsgBaseView::ADD_AUDIO;
            params << filepath;
            launchUniEditor(params);
        }
    }
}

//---------------------------------------------------------------
// MsgConversationView::addSubject()
// @see header file
//---------------------------------------------------------------
void MsgConversationView::addSubject()
{

    QString filepath;
    QVariantList params;
    params << MsgBaseView::ADD_SUBJECT;
    launchUniEditor(params);
}

//---------------------------------------------------------------
// MsgConversationView::forwardMessage()
// Forwards the message
//---------------------------------------------------------------
void MsgConversationView::forwardMessage()
{
    QModelIndex index = mConversationList->currentIndex();
    //messageId & messageType to be forwarded
    qint32 messageId = index.data(ConvergedMsgId).toLongLong();
    int messageType = index.data(MessageType).toInt();
 
    //Mark the message to read before forwarding.
    if(index.data(UnReadStatus).toBool())
    {
        QList<int> msgIdList;
        msgIdList.append(messageId);
        ConversationsEngine::instance()->markMessagesRead(msgIdList);
    }
    // populate params and launch editor 
    QVariantList params;
    params << MsgBaseView::FORWARD_MSG;
    params << messageId;
    params << messageType;
    launchUniEditor(params);
}

//---------------------------------------------------------------
// MsgConversationView::resendMessage()
// Resends the message in the failed messages case
//---------------------------------------------------------------
void MsgConversationView::resendMessage()
{
    QModelIndex index = mConversationList->currentIndex();
    if(index.isValid())
    {
        qint32 messageId = index.data(ConvergedMsgId).toLongLong();    
        if(!(ConversationsEngine::instance()->resendMessage(messageId)))
        {
            HbMessageBox::warning(LOC_MSG_SEND_FAILED);
        }
    }
    
}

//---------------------------------------------------------------
// MsgConversationView::downloadMessage()
// @see header
//---------------------------------------------------------------
void MsgConversationView::downloadMessage()
{
    QModelIndex index = mConversationList->currentIndex();
    if(index.isValid())
    {
        qint32 messageId = index.data(ConvergedMsgId).toLongLong();    
        if(ConversationsEngine::instance()->downloadMessage(messageId)!=KErrNone)
        {
            HbMessageBox::warning("Message Retrieval Failed!"); //TODO: use logical str name
        }
    }
    
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
        activateInputBlocker();
        ConvergedMessageAddressList addresses;
        addresses = mContactCardWidget->address();

        // Populate the ConvergedMessage.
        if (!addresses.isEmpty())
        {
            ConvergedMessage msg;
            populateForSending(msg);
            msg.addToRecipients(addresses);
            
            // save to drafts
            msgId = mSendUtil->saveToDrafts(msg);
                     
            mEditorWidget->clear();
        }
        deactivateInputBlocker();
    }
    return ((msgId > INVALID_MSGID)? true : false);
}

//---------------------------------------------------------------
//MsgConversationView::populateForSending()
//@see header
//---------------------------------------------------------------
void MsgConversationView::populateForSending(ConvergedMessage &message)
{
    message.setMessageType(ConvergedMessage::Sms);
    message.setBodyText(mEditorWidget->content());
    message.setDirection(ConvergedMessage::Outgoing);
    QDateTime time = QDateTime::currentDateTime();
    message.setTimeStamp(time.toTime_t());
}

//---------------------------------------------------------------
//MsgConversationView::launchBtDisplayService()
//@see header
//---------------------------------------------------------------
void MsgConversationView::launchBtDisplayService(const QModelIndex & index)
{
    qint32 messageId = index.data(ConvergedMsgId).toLongLong();

    QString interface("com.nokia.services.btmsgdispservices.displaymsg");
    QString operation("displaymsg(int)");

    XQServiceRequest request(interface, operation, false);

    request << messageId;

    bool result = request.send();
}

//---------------------------------------------------------------
// MsgConversationView::menuAboutToShow()
// @See header
//---------------------------------------------------------------
void MsgConversationView::menuAboutToShow()
{
    // Clear all the previously added actions.
    HbMenu *mainMenu = this->menu();
    mainMenu->clearActions();

    // Message type specific menu items
    QModelIndex index = ConversationsEngine::instance()->getConversationsModel()->index(0, 0);
    if (ConvergedMessage::BT != index.data(MessageType).toInt())
    {
        // Attach sub-menu
        HbMenu *attachSubMenu = mainMenu->addMenu(LOC_ATTACH);
        
        attachSubMenu->addAction(LOC_PHOTO,this, SLOT(fetchImages()));
        attachSubMenu->addAction(LOC_SOUND,this, SLOT(fetchAudio()));
		
        HbAction* addVCard = attachSubMenu->addAction(LOC_VCARD);
        addVCard->setData(VCARD_INSERTION_MODE);        
        connect(addVCard, SIGNAL(triggered()),this,SLOT(fetchContacts()));

        HbAction *addRecipients = mainMenu->addAction(LOC_ADD_RECIPIENTS);
        addRecipients->setData(CONTACT_INSERTION_MODE);        
        connect(addRecipients, SIGNAL(triggered()), this, SLOT(fetchContacts()));

        mainMenu->addAction(LOC_ADD_SUBJECT,this, SLOT(addSubject()));
    }
}

//---------------------------------------------------------------
//MsgConversationView::openItem
//@see header
//---------------------------------------------------------------
void MsgConversationView::openItem(const QModelIndex & index)
    {
    // Return if invalid index.
    if (!index.isValid() || mVkbopened)
        {
        return;
        }

    if(mItemLongPressed)
        return;
    
    int messageType = index.data(MessageType).toInt();
    int messageSubType = index.data(MessageSubType).toInt();
    
    if (ConvergedMessage::BioMsg == messageType) {
        if (ConvergedMessage::RingingTone == messageSubType) {
            if (RingBc::askSaveQuery()) {
                saveRingingTone();
            }
            return;
        }
        // Unsupported messages
        else if (ConvergedMessage::VCard == messageSubType
            || ConvergedMessage::VCal == messageSubType
            || ConvergedMessage::Provisioning == messageSubType) {
            return;
        }
    }
    else if (ConvergedMessage::BT == messageType) {
        launchBtDisplayService(index);
        return;
    }
    else if(ConvergedMessage::MmsNotification == messageType)
    {
        qint32 messageId = index.data(ConvergedMsgId).toLongLong();    
        if(!ConversationsEngine::instance()->downloadOperationSupported(messageId))           
        {
           int notificationState = index.data(NotificationStatus).toInt();
           if( notificationState == ConvergedMessage::NotifExpired)
           {
               deleteItem();
           }
           return;
        }
        else
        {
            //TODO: use logical str name
            if(HbMessageBox::question("Download Message?",
                LOC_COMMON_DOWNLOAD,
                LOC_BUTTON_CANCEL))
            {
                downloadMessage();
            }
            return;
        }
    }

    ConvergedMessage message;
    // check whether message is in sending progress, then donot launch viewer.
    int location = index.data(MessageLocation).toInt();
    int sendingState = index.data(SendingState).toInt();
    
    //If message is in Sending state or is Scheduled to be sent later,
    //do not open the message
    if(sendingState == ConvergedMessage::Suspended ||
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
        (sendingState == ConvergedMessage::SentState)||
        (sendingState == ConvergedMessage::Failed) ||
        (sendingState == ConvergedMessage::Resend)))
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
        message.setMessageSubType((ConvergedMessage::MessageSubType) messageSubType);

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
void MsgConversationView::launchUniEditor(const QVariantList& data)
{
    // param list for switching to editor view
    QVariantList params;
    QByteArray dataArray;
    QDataStream messageStream
    (&dataArray, QIODevice::WriteOnly | QIODevice::Append);

    // first arg is always the editor operation
    int editorOperation = data.at(0).toInt();
    
    ConvergedMessage message;
    QVariant data2;
    if( editorOperation != MsgBaseView::FORWARD_MSG )
    {
        message.setBodyText(mEditorWidget->content());

        ConvergedMessageAddress address;
        address.setAlias(mContactCardWidget->address().at(0)->alias());
        address.setAddress(mContactCardWidget->address().at(0)->address());
        message.addToRecipient(address);

        if(editorOperation == MsgBaseView::ADD_PHOTO ||
           editorOperation == MsgBaseView::ADD_AUDIO ||
           editorOperation == MsgBaseView::ADD_VIDEO )
        {
            // filepath is sent in cases like ADD_PHOTO, ADD_AUDIO etc.
            QString filepath;
            filepath = data.at(1).toString();
            if(!filepath.isEmpty())
            {
                ConvergedMessageAttachment* attachment = 
                        new ConvergedMessageAttachment(filepath);
                ConvergedMessageAttachmentList attachmentList;
                attachmentList.append(attachment);
                message.addAttachments(attachmentList);
            }            
        }
        else if(editorOperation == MsgBaseView::ADD_VCARD)
        {
            // filepath is not sent in cases like VCards & recipients
            // instead, we will get a list of contacts. Pass it as it is.
            data2 = data.at(1);
        }
    }
    else
    {
        qint32 msgId = (qint32)data.at(1).toInt();
        int msgType = data.at(2).toInt();
        ConvergedMessageId id(msgId);
        message.setMessageId(id);
        message.setMessageType((ConvergedMessage::MessageType)msgType);
    }

    message.serialize(messageStream);
    params << MsgBaseView::UNIEDITOR;
    params << MsgBaseView::CV;
    params << dataArray;
    params << editorOperation;
    if(!data2.isNull())
        params << data2;

    clearEditors();
    emit switchView(params);
}

//---------------------------------------------------------------
// MsgConversationView::populateConversationsView
// @see header file
//---------------------------------------------------------------
void MsgConversationView::populateConversationsView()
{
    bool b = connect(ConversationsEngine::instance(), 
                     SIGNAL(conversationModelUpdated()),
                     this, 
                     SLOT(scrollToBottom()));

    mConversationList->setModel(mMessageModel);
    
    refreshView();
    scrollToBottom();
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
    UniDataModelPluginInterface* pluginInterface = 
    pluginLoader->getDataModelPlugin(ConvergedMessage::BioMsg);
    pluginInterface->setMessageId(messageId);
    UniMessageInfoList attachments = pluginInterface->attachmentList();
    if(attachments.count() > 0)
    {
        QString attachmentPath = attachments[0]->path();

        RingBc* ringBc = new RingBc();

        ringBc->saveTone(attachmentPath);

        // clear attachement list : its allocated at data model
        while(!attachments.isEmpty())
        {
            delete attachments.takeFirst();
        }

        delete ringBc;
    }
    delete pluginLoader;
}

//---------------------------------------------------------------
// MsgConversationView::handleSmsCharLimitReached
// @see header file
//---------------------------------------------------------------
void MsgConversationView::handleSmsCharLimitReached()   
{
    QString filepath;
    QVariantList params;
    params << MsgBaseView::ADD_OTHERS;
    launchUniEditor(params);   
}

//---------------------------------------------------------------
// MsgConversationView::validateMsgForForward
// @see header file
//---------------------------------------------------------------
bool MsgConversationView::validateMsgForForward(int &messageType,
    qint32 &messageId)
{
    bool retValue = true;
    if (messageType == ConvergedMessage::Mms)
    {
        //Validate if the mms msg can be forwarded or not
        MmsConformanceCheck* mmsConformanceCheck = new MmsConformanceCheck;
        retValue = mmsConformanceCheck->validateMsgForForward(messageId);
        
        delete mmsConformanceCheck;
    }

    return retValue;
}

//---------------------------------------------------------------
// MsgConversationView::vkbOpened
// @see header file
//---------------------------------------------------------------
void MsgConversationView::vkbOpened()
{
    mVkbopened = true;
    
    mContactCardWidget->connectSignals(false);
    
    emit hideChrome(true);
    
    QRectF appRect = mVkbHost->applicationArea();    
    qreal spacing = 0.0;
    qreal cardHeight = 0.0;
    if(mContactCardWidget->isVisible())
        {
        cardHeight = mContactCardWidget->rect().height();
        spacing = HbDeviceProfile::profile(this).unitValue();
        }
    
    this->setMaximumHeight(appRect.height()- cardHeight - spacing);
    
    disconnect(mVkbHost,SIGNAL(keypadOpened()),this,SLOT(vkbOpened()));
    
    scrollToBottom();
}
	  
//---------------------------------------------------------------
// MsgConversationView::vkbClosed
// @see header file
//---------------------------------------------------------------
void MsgConversationView::vkbClosed()
{
    mVkbopened = false;
    
    mContactCardWidget->connectSignals(true);
    
    emit hideChrome(false);
    
    this->setMaximumHeight(-1);
    connect(mVkbHost,SIGNAL(keypadOpened()),this,SLOT(vkbOpened()));
}

//---------------------------------------------------------------
// MsgConversationView::serviceRequestError
// @see header file
//---------------------------------------------------------------
void MsgConversationView::serviceRequestError(int errorCode, const QString& errorMessage)
{
    QDEBUG_WRITE_FORMAT(errorMessage,errorCode);
}

//---------------------------------------------------------------
// MsgConversationView::activateInputBlocker
// @see header file
//--------------------------------------------------------------
void MsgConversationView::activateInputBlocker()
    {
        this->grabMouse();
        this->grabKeyboard();
    }

//---------------------------------------------------------------
// MsgConversationView::deactivateInputBlocker
// @see header file
//--------------------------------------------------------------
void MsgConversationView::deactivateInputBlocker()
    {    
        this->ungrabKeyboard();
        this->ungrabMouse();
    }


// EOF
