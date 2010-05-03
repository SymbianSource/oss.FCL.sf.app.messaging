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

#include "msgcontactcardwidget.h"

// SYSTEM INCLUDES
#include <HbStyle>
#include <HbIconItem>
#include <HbTextItem>
#include <HbFrameDrawer>
#include <HbGesture>
#include <HbGestureSceneFilter>
#include <HbWidgetFeedback>
#include <HbFrameItem>
#include <qmobilityglobal.h>
#include <qcontactphonenumber.h>
#include <qcontactfilter.h>
#include <qcontactmanager.h>
#include <qcontactdetailfilter.h>
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#include <XQServiceRequest.h>
#include <cntservicescontact.h>
#include <QGraphicsSceneMouseEvent>
#include <HbMenu>
#include <HbMainWindow>

#include <ccsdefs.h>

// USER INCLUDES
#include "conversationsenginedefines.h"
#include "debugtraces.h"
#include "conversationsengine.h"
#include "convergedmessage.h"

QTM_USE_NAMESPACE

// LOCAL CONSTANTS
const QString DEFAULT_AVATAR_ICON("qtg_large_avatar");
const QString BT_ICON("qtg_large_bluetooth");
const QString BACKGROUND_FRAME_NORMAL("qtg_fr_groupbox");
const QString GROUPBOX_BG_FRAME_PRESSED("qtg_fr_groupbox_pressed");

const QString PLUGINPATH("conversationviewplugin.dll");

// LOCALIZATION CONSTANTS
#define LOC_RECEIVED_FILES hbTrId("txt_messaging_title_received_files")
#define LOC_MENU_CONTACT_INFO hbTrId("txt_messaging_menu_open_contact_info")
#define LOC_COMMON_MENU_CALL hbTrId("txt_common_menu_call_verb")
#define LOC_SAVETO_CONTACTS hbTrId("txt_messaging_menu_save_to_contacts")

//---------------------------------------------------------------
// MsgContactCardWidget::MsgContactCardWidget
// @see header
//---------------------------------------------------------------
MsgContactCardWidget::MsgContactCardWidget(QGraphicsItem *parent) :
HbWidget(parent), 
mMenuShown(false),
mAvatarIconItem(NULL),
mPresenceIconItem(NULL),
mAddressTextItem(NULL),
mGestureFilter(NULL)

{
    int baseId = style()->registerPlugin(PLUGINPATH);
    QDEBUG_WRITE_FORMAT("MsgContactCardWidget BASE ID --->", baseId);
    setPluginBaseId(baseId);

    init();  
    setBackGround(BACKGROUND_FRAME_NORMAL);
    connectSignals(true);
    
    connect(this->mainWindow(),SIGNAL(viewReady()),this,SLOT(initGesture()));   
}

//---------------------------------------------------------------
// MsgContactCardWidget::~MsgContactCardWidget
// @see header
//---------------------------------------------------------------
MsgContactCardWidget::~MsgContactCardWidget()
{
    style()->unregisterPlugin(PLUGINPATH);
    
    if(mGestureFilter)
        {
        removeSceneEventFilter(mGestureFilter);
        }
}

//---------------------------------------------------------------
// MsgContactCardWidget::init
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::init()
{
    mAvatarIconItem = new HbIconItem(this);
    HbStyle::setItemName(mAvatarIconItem, "avatar");

    mPresenceIconItem = new HbIconItem(this);
    HbStyle::setItemName(mPresenceIconItem, "presence");

    mAddressTextItem = new HbTextItem(this);
    HbStyle::setItemName(mAddressTextItem, "addressText");

}

//---------------------------------------------------------------
// MsgContactCardWidget::setAvatar
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::setAvatar(const HbIcon &avatar)
{
    mAvatarIconItem->setIcon(avatar);
}

//---------------------------------------------------------------
// MsgContactCardWidget::setPresenceIcon
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::setPresenceIcon(const HbIcon &presenceIcon)
{
    mPresenceIconItem->setIcon(presenceIcon);
}

//---------------------------------------------------------------
// MsgContactCardWidget::setAddress
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::setAddress(const QString &address)
{
    mAddress = address;
    mAddressTextItem->setText(address);
}

//---------------------------------------------------------------
// MsgContactCardWidget::address
// @see header
//---------------------------------------------------------------
ConvergedMessageAddressList MsgContactCardWidget::address()
{
    ConvergedMessageAddressList addresses;
    QModelIndex index = ConversationsEngine::instance()->getConversationsModel()->index(0, 0);
    ConvergedMessageAddress* address = new ConvergedMessageAddress(
        index.data(ConversationAddress).toString());
    address->setAlias(mAddress);
    addresses.append(address);
    return addresses;
}

//---------------------------------------------------------------
// MsgContactCardWidget::updateContents
// @see header file
//---------------------------------------------------------------
void MsgContactCardWidget::updateContents()
{
    // Get the Model Index
    QModelIndex index = ConversationsEngine::instance()->getConversationsModel()->index(0, 0);

    // Index is not valid, return.
    if (!index.isValid()) {
        return;
    }

    qint64 convId = ConversationsEngine::instance()->getCurrentConversationId();

    if (KBluetoothMsgsConversationId == convId) {
        setAddress(LOC_RECEIVED_FILES);
        setAvatar(HbIcon(BT_ICON));
    }
    else {
        //Set the Contact Name/Number
        qint64 convId;
        QString displayName;
        QString contactAddress;
        convId = ConversationsEngine::instance()->getCurrentConversationId();
        ConversationsEngine::instance()->getContactDetails(convId, displayName,
            contactAddress);
        
        mContactNumber = contactAddress;
        QString contactName;
        if (displayName.isEmpty()) {
            contactName.append(contactAddress);
        }
        else {
            contactName.append(displayName);
        }
        
        setAddress(contactName);

        // Set Avatar
        HbIcon avatar;
        QByteArray avatarByteArray = index.data(Avatar).toByteArray();
        if (avatarByteArray.isEmpty()) {
            avatar = HbIcon(DEFAULT_AVATAR_ICON);
        }
        else {
            QPixmap pixmap;
            pixmap.loadFromData(avatarByteArray);
            avatar = HbIcon(QIcon(pixmap));
        }
        setAvatar(avatar);
    }
}

//---------------------------------------------------------------
// MsgContactCardWidget::clearContent
// @see header file
//---------------------------------------------------------------
void MsgContactCardWidget::clearContent()
{
    setAddress("");
    setAvatar(HbIcon());
}

//---------------------------------------------------------------
// MsgContactCardWidget::initGesture
// @see header file
//---------------------------------------------------------------
void MsgContactCardWidget::initGesture()
{
    // Create gesture filter
    QGraphicsScene* sc = this->scene();
    mGestureFilter = new HbGestureSceneFilter( Qt::LeftButton, this );
    
    // Add gestures for longpress
    HbGesture* gestureLongpressed = new HbGesture( HbGesture::longpress,5 );
    
    mGestureFilter->addGesture( gestureLongpressed );
    
    connect( gestureLongpressed, SIGNAL(longPress(QPointF)),
             this, SLOT(handleLongPress(QPointF)));

    //install gesture filter.
    this->installSceneEventFilter(mGestureFilter);
    
    disconnect(this->mainWindow(),SIGNAL(viewReady()),this,SLOT(initGesture()));   
}

//---------------------------------------------------------------
// MsgContactCardWidget::mousePressEvent
// @see header file
//---------------------------------------------------------------
void MsgContactCardWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{    
    mMenuShown = false;
    
    HbWidgetFeedback::triggered(this, Hb::InstantPressed);
    
    setBackGround(GROUPBOX_BG_FRAME_PRESSED);
    
    event->accept();
}


//---------------------------------------------------------------
// MsgContactCardWidget::mouseReleaseEvent
// @see header file
//---------------------------------------------------------------
void MsgContactCardWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setBackGround(BACKGROUND_FRAME_NORMAL);
    
    if(this->rect().contains(event->pos())&& !mMenuShown)
        {
        HbWidgetFeedback::triggered(this, Hb::InstantClicked);
		emit clicked();
        }
    
    event->accept();    
}

//---------------------------------------------------------------
// MsgContactCardWidget::setBackGround
// @see header file
//---------------------------------------------------------------
void MsgContactCardWidget::setBackGround(const QString& bg)
    {
    HbFrameItem* backGround = new HbFrameItem(this);
    backGround->frameDrawer().setFrameGraphicsName(bg);
    backGround->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
    this->setBackgroundItem(backGround); 
    this->repolish();
    }

//---------------------------------------------------------------
// MsgContactCardWidget::handleLongPress
// @see header file
//---------------------------------------------------------------
void MsgContactCardWidget::handleLongPress(QPointF position)
{
    if(KBluetoothMsgsConversationId 
        != ConversationsEngine::instance()->getCurrentConversationId())    
        {
        HbMenu* contextMenu = new HbMenu();
        contextMenu->setDismissPolicy(HbPopup::TapAnywhere);
        contextMenu->setAttribute(Qt::WA_DeleteOnClose, true);
        contextMenu->setPreferredPos(position);
        
        contextMenu->addAction(LOC_MENU_CONTACT_INFO,this, SLOT(openContactInfo()));    
        contextMenu->addAction(LOC_COMMON_MENU_CALL, this, SLOT(call()));
        
        //If contact doesn't exist in phonebook then add another menu item "Save to Contacts"
        int contactId = resolveContactId(mContactNumber);
        if(contactId < 0)
        {
            contextMenu->addAction(LOC_SAVETO_CONTACTS, this, SLOT(addToContacts()));
        }
        
        contextMenu->show();
        
        mMenuShown = true;
        }
}

//---------------------------------------------------------------
// MsgContactCardWidget::overrideFeedback
// @see header file
//---------------------------------------------------------------
HbFeedback::InstantEffect MsgContactCardWidget::overrideFeedback(Hb::InstantInteraction interaction) const
        {
        switch(interaction)
            {
            case Hb::InstantPressed:
            case Hb::InstantClicked:
                return HbFeedback::Basic;
            default:
                return HbFeedback::NoOverride;
            }
        }
		
//---------------------------------------------------------------
// MsgContactCardWidget::openContactInfo
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::openContactInfo()
{
    QString operation;
    QList<QVariant> args;
    if(KBluetoothMsgsConversationId 
            != ConversationsEngine::instance()->getCurrentConversationId())
    {
        int contactId = resolveContactId(mContactNumber);                
        if(contactId > 0)
        {
            //open existing contact card
            operation = QString("open(int)");
            args << contactId;
        }
        else
        {
            //populate data and open unknown contact template
            operation = QString("editCreateNew(QString,QString)");
            QString type = QContactPhoneNumber::DefinitionName;
    
            args << type;
            args << mAddress;
        }
        //service stuff.
        QString serviceName("com.nokia.services.phonebookservices");
     
        XQAiwRequest* request;
        XQApplicationManager appManager;
        request = appManager.create(serviceName, "Fetch", operation, true); // embedded
        if ( request == NULL )
        {
        	return;       
        }
        
        // Result handlers
        connect (request, SIGNAL(requestOk(const QVariant&)), this, SLOT(handleOk(const QVariant&)));
        connect (request, SIGNAL(requestError(const QVariant&)), this, SLOT(handleError(const QVariant&)));
        
        request->setArguments(args);
        request->send();
        delete request;
    }
}

//---------------------------------------------------------------
// MsgContactCardWidget::resolveContactId
// @see header
//---------------------------------------------------------------
int MsgContactCardWidget::resolveContactId(const QString& value)
{
    int contactId = -1;
    
    QContactManager phonebookManager("symbian");
    
    QContactDetailFilter phoneFilter;
    phoneFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName,
                                        QContactPhoneNumber::FieldNumber);
    phoneFilter.setValue(value);
    phoneFilter.setMatchFlags(QContactFilter::MatchEndsWith);

    QList<QContactSortOrder> sortOrder;
    QList<QContact> matchingContacts = phonebookManager.contacts(
            phoneFilter,
            sortOrder,
            QStringList());

    if ( matchingContacts.count() > 0 ) 
        {       
        contactId = matchingContacts.at(0).localId();   
        }
    
    return contactId;
}

//---------------------------------------------------------------
// MsgContactCardWidget::call
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::call()
{
    //Launch dialer service     
    QString serviceName("com.nokia.services.telephony");
    QString operation("dial(QString)");
    
    XQServiceRequest* serviceRequest = new XQServiceRequest(serviceName,operation,false);
    
    connect(serviceRequest, SIGNAL(requestCompleted(QVariant)),
            this, SLOT(onServiceRequestCompleted()));
    
    connect(serviceRequest, SIGNAL(requestError(int)),
            this, SLOT(onServiceRequestCompleted()));
    
    *serviceRequest << mContactNumber;
    serviceRequest->send();
}

//---------------------------------------------------------------
// MsgContactCardWidget::addToContacts
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::addToContacts()
{
}

//---------------------------------------------------------------
// MsgContactCardWidget::handleOk
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::handleOk(const QVariant& result)
{
    Q_UNUSED(result)
    
    QContactManager phonebookManager("symbian");
        
    QContactDetailFilter phoneFilter;
    phoneFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, 
                                            QContactPhoneNumber::FieldNumber);
    phoneFilter.setValue(mContactNumber);
    phoneFilter.setMatchFlags(QContactFilter::MatchEndsWith);

    QList<QContactSortOrder> sortOrder;
    QList<QContact> matchingContacts = phonebookManager.contacts(phoneFilter,
                                       sortOrder, QStringList());
    
    if(matchingContacts.count())
    {
        setAddress(matchingContacts.at(0).displayLabel());
    }
}

//---------------------------------------------------------------
// MsgContactCardWidget::handleError
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::handleError(int errorCode, const QString& errorMessage)
    {
    Q_UNUSED(errorMessage)
    Q_UNUSED(errorCode)
    }
	
//---------------------------------------------------------------
// MsgContactCardWidget::onServiceRequestCompleted
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::onServiceRequestCompleted()
{
    //service request is now complete. delete it.
    XQServiceRequest* request = qobject_cast<XQServiceRequest*>(sender());
    if(request)
    {
        delete request;
    }
}

//---------------------------------------------------------------
// MsgContactCardWidget::connectSignals
// @see header
//---------------------------------------------------------------
void MsgContactCardWidget::connectSignals(bool yes)
{
    if(yes)
    {
        connect(this,SIGNAL(clicked()),this,SLOT(openContactInfo()));
    }
    else
    {
        disconnect(this,SIGNAL(clicked()),this,SLOT(openContactInfo()));
    }
}
// EOF
