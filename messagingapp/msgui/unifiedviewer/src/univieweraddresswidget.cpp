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
 * Description:Custom widget derived from HbTextEdit which provides rich text
 *              processing
 *
 */

#include "univieweraddresswidget.h"

// SYSTEM INCLUDES
#include <QTextCursor>
#include <HbMenu>
#include <QGraphicsSceneMouseEvent>
#include <HbAction>
#include <HbFrameItem>
#include <QTextBlock>
#include <QApplication>
#include <QClipBoard>
#include <xqservicerequest.h>

#include <xqappmgr.h>
#include <cntservicescontact.h>
#include <qtcontacts.h>
#include <XQServiceRequest.h>
#include <xqaiwrequest.h>
#include "msgcontacthandler.h"





// LOCAL CONSTANTS
const QString ADDRESS_SEPARATOR("; ");
const QString ADDRESS_OPEN(" (");
const QString ADDRESS_CLOSE(")");
const QString SPACE(" ");

//localization
#define LOC_OPEN_CONTACT_INFO hbTrId("txt_messaging_menu_open_contact_info")
#define LOC_CALL              hbTrId("txt_common_menu_call_verb")
#define LOC_SEND_MESSAGE      hbTrId("txt_common_menu_send_message")
#define LOC_SAVE_TO_CONTACTS  hbTrId("txt_common_menu_save_to_contacts")
#define LOC_COPY              hbTrId("txt_common_menu_copy")

const QString BG_FRAME_GRAPHICS("qtg_fr_lineedit_normal");

//---------------------------------------------------------------
// UniViewerAddressWidget::UniViewerAddressWidget
// @see header file
//---------------------------------------------------------------
UniViewerAddressWidget::UniViewerAddressWidget(QGraphicsItem * parent) :
HbTextEdit(parent),
mCursorPos(-1)
{
    this->setReadOnly(true);
    this->setCursorVisibility(Hb::TextCursorHidden);
    this->setScrollable(false);
    HbFrameItem *noBackground = new HbFrameItem(this);
    this->setBackgroundItem(noBackground);

    HbFontSpec fontSpec(HbFontSpec::Secondary);
    QFont font = fontSpec.font();
    this->setFont(font);

    mFormatNormal.setForeground(palette().link());
    mFormatNormal.setBackground(Qt::transparent);

    mFormatHighlight.setBackground(palette().highlight());
    mFormatHighlight.setForeground(palette().highlightedText());

    connect(this, SIGNAL(aboutToShowContextMenu(HbMenu*,QPointF)),
            this, SLOT(aboutToShowContextMenu(HbMenu*,QPointF)));
}

//---------------------------------------------------------------
// UniViewerAddressWidget::~UniViewerAddressWidget
// @see header file
//---------------------------------------------------------------
UniViewerAddressWidget::~UniViewerAddressWidget()
{
}

//---------------------------------------------------------------
//UniViewerAddressWidget :: mousePressEvent
// @see header file
//---------------------------------------------------------------
void UniViewerAddressWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HbTextEdit::mousePressEvent(event);

    QTextDocument* doc = this->document();

    mCursorPos = doc->documentLayout()->hitTest(event->pos(), Qt::ExactHit);

    highlightText(true);
}

//---------------------------------------------------------------
//UniViewerAddressWidget :: mouseReleaseEvent
// @see header file
//---------------------------------------------------------------
void UniViewerAddressWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    HbTextEdit::mouseReleaseEvent(event);

    highlightText(false);

    QString anchor = this->anchorAt(event->pos());

    if(!anchor.isEmpty() && !this->textCursor().hasSelection())
    {
        shortTapAction(anchor);
    }
}

//----------------------------------------------------------------------------
// UniViewerAddressWidget::populate
// @see header file
//----------------------------------------------------------------------------
void UniViewerAddressWidget::populate(const QString &label,
                                      const QString &address,
                                      const QString &alias)
{
    QString labelText = label;
    labelText.trimmed();
    labelText += SPACE;
    
    //Font.
    HbFontSpec fontSpec(HbFontSpec::Secondary);
    qreal fontHeight = 0.0;
    style()->parameter("hb-param-text-height-tiny", fontHeight);
    fontSpec.setTextHeight(fontHeight);    
    QFont font = fontSpec.font();
    
    QTextCharFormat labelFormat;
    labelFormat.setFont(font);
    
    QTextCharFormat addressFormat;
    addressFormat.setForeground(palette().link());
    addressFormat.setFontUnderline(true);

    // Insert the label then the addresses
    QTextCursor cursor(this->textCursor());
    cursor.insertText(labelText,labelFormat);
    
    QString address1 = QString();
    if (!(alias.isEmpty()))
    {
        address1.append(alias);
        QString alias1 = QString();
        
        int totalNumbers = 0;
        MsgContactHandler::resolveContactDisplayName(
                        address,
                        alias1,
                        totalNumbers);
        if (totalNumbers > 1)
        {
            address1.append(ADDRESS_OPEN);
            address1.append(address);
            address1.append(ADDRESS_CLOSE);
        }
    }
    else
    {
        address1.append(address);
    }
    addressFormat.setAnchorHref(address);
    cursor.insertText(address1, addressFormat);
}

//----------------------------------------------------------------------------
// UniViewerAddressWidget::populate
// @see header file
//----------------------------------------------------------------------------
void UniViewerAddressWidget::populate(const QString &label,
                                      ConvergedMessageAddressList addressList)
{
    QString labelText = label;
    labelText.trimmed();
    labelText += SPACE;
    
    //Font.
    HbFontSpec fontSpec(HbFontSpec::Secondary);
    qreal fontHeight = 0.0;
    style()->parameter("hb-param-text-height-tiny", fontHeight);
    fontSpec.setTextHeight(fontHeight);    
    QFont font = fontSpec.font();
    
    QTextCharFormat labelFormat;
    labelFormat.setFont(font);
    
    QTextCharFormat defaultFormat;
    defaultFormat.setForeground(palette().link());

    QTextCharFormat addressFormat;
    addressFormat.setForeground(palette().link());
    addressFormat.setFontUnderline(true);

    // Insert the label then the addresses
    QTextCursor cursor(this->document());
    cursor.insertText(labelText,labelFormat);

    int addressCount = addressList.count();

    for (int i = 0; i < addressCount; ++i)
    {

        QString address = QString();
        if (! (addressList[i]->alias().isEmpty()))
        {
            address.append(addressList[i]->alias());
            QString alias = QString();
            
            int totalNumbers = 0;
            MsgContactHandler::resolveContactDisplayName(
                            addressList[i]->address(),
                            alias,
                            totalNumbers);
            if (totalNumbers > 1)
            {
                address.append(ADDRESS_OPEN);
                address.append(addressList[i]->address());
                address.append(ADDRESS_CLOSE);
            }
        }
        else
        {
            address.append(addressList[i]->address());
        }

        addressFormat.setAnchorHref(addressList[i]->address());
        cursor.insertText(address, addressFormat);

        if (addressCount - 1 != i)
        {
            cursor.insertText(ADDRESS_SEPARATOR, defaultFormat);
        }
        
    }
}

//----------------------------------------------------------------------------
// UniViewerAddressWidget::clearContent
// @see header file
//----------------------------------------------------------------------------
void UniViewerAddressWidget::clearContent()
{
    this->document()->clear();
}

//----------------------------------------------------------------------------
// UniViewerAddressWidget::menuClosed
// @see header file
//----------------------------------------------------------------------------
void UniViewerAddressWidget::menuClosed()
{
    highlightText(false);
}

//----------------------------------------------------------------------------
// UniViewerAddressWidget::highlightText
// @see header file
//----------------------------------------------------------------------------
void UniViewerAddressWidget::highlightText(bool highlight)
{
    QTextBlock textBlock = this->document()->findBlock(mCursorPos);

    QTextBlock::iterator it;

    for (it = textBlock.begin(); !(it.atEnd()); ++it)
    {
        QTextFragment currentFragment = it.fragment();
        
        if (currentFragment.isValid() && currentFragment.contains(mCursorPos)
            && currentFragment.charFormat().fontUnderline())
        {
            int start = currentFragment.position();
            int length = currentFragment.length();

            QTextCursor cursor = this->textCursor();
            cursor.clearSelection();
            cursor.setPosition(start);
            cursor.setPosition(start + length,QTextCursor::KeepAnchor);

            if(highlight)
            {
                cursor.mergeCharFormat(mFormatHighlight);
            }
            else
            {
                cursor.mergeCharFormat(mFormatNormal);
            }

            cursor.clearSelection();
            break;
        }
    }
}

void UniViewerAddressWidget::aboutToShowContextMenu(HbMenu *contextMenu, const QPointF &pos)
{
    //remove default actions.
    contextMenu->clearActions();
    
    // Check if there is an anchor at this pos
    QString  anchor = this->anchorAt(pos);

    if(!anchor.isEmpty() && !this->textCursor().hasSelection())
    {

        HbAction* action = NULL;

        action = contextMenu->addAction(LOC_OPEN_CONTACT_INFO, this, SLOT(openContactInfo()));
        action->setData(anchor);

        action = contextMenu->addAction(LOC_CALL, this, SLOT(call()));
        action->setData(anchor);

        action = contextMenu->addAction(LOC_SEND_MESSAGE, this, SLOT(sendMessage()));
        action->setData(anchor);

        action = contextMenu->addAction(LOC_SAVE_TO_CONTACTS, this, SLOT(saveToContacts()));
        action->setData(anchor);

        action = contextMenu->addAction(LOC_COPY, this, SLOT(copyToClipboard()));
        action->setData(anchor);

    }

    connect(contextMenu,SIGNAL(aboutToClose()),this,SLOT(menuClosed()));    
}

void UniViewerAddressWidget::shortTapAction(QString anchor)
{
    HbAction action;
    action.setData(anchor);
    connect(&action,SIGNAL(triggered()),this,SLOT(openContactInfo()));
    action.trigger();
}

void UniViewerAddressWidget::copyToClipboard()
{
    HbAction* action = qobject_cast<HbAction*>(sender());

    if(action)
    {
        QMimeData* data = new QMimeData();
        QString str = action->data().toString();
        data->setText(str);
        QApplication::clipboard()->setMimeData(data);
    }
}

void UniViewerAddressWidget::call()
{
    HbAction* action = qobject_cast<HbAction*>(sender());

    if(action)
    {
        QString phoneNumber = action->data().toString();
        
        //invoke dialer service and pass phoneNumber.        
        QString serviceName("com.nokia.services.telephony");
        QString operation("dial(QString)");
        
        XQServiceRequest* serviceRequest = new XQServiceRequest(serviceName,operation,false);
        
        connect(serviceRequest, SIGNAL(requestCompleted(QVariant)),
                this, SLOT(onServiceRequestCompleted()));
        
        connect(serviceRequest, SIGNAL(requestError(int)),
                this, SLOT(onServiceRequestCompleted()));
        
        *serviceRequest << phoneNumber;
        serviceRequest->send();
    }
}

void UniViewerAddressWidget::onServiceRequestCompleted()
    {
    //service request is now complete. delete it.    
    XQServiceRequest* request = qobject_cast<XQServiceRequest*>(sender());
    
    if(request)
        {
        delete request;
        }
    }


void UniViewerAddressWidget::openContactInfo()
{
    HbAction* action = qobject_cast<HbAction*>(sender());
    
    if(action)
    {
        QList<QVariant> args;
        QString operation;
        
        QString data = action->data().toString();        
    
        int contactId = MsgContactHandler::resolveContactDisplayName(
                data,
                QContactPhoneNumber::DefinitionName,
                QContactPhoneNumber::FieldNumber);

        if(contactId > 0)
        {
            //open contact card
            operation = QString("open(int)");
            args << contactId;
        }
        else
        {
            //save to contacts with phone number field prefilled.

            operation = QString("editCreateNew(QString,QString)");
            QString type = QContactPhoneNumber::DefinitionName;

            args << type;
            args << data;
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
        connect (request, SIGNAL(requestOk(const QVariant&)), 
            this, SLOT(handleOk(const QVariant&)));
        connect (request, SIGNAL(requestError(const QVariant&)), 
            this, SLOT(handleError(const QVariant&)));
        
        request->setArguments(args);
        request->send();
        delete request;
    }
}

void UniViewerAddressWidget::handleOk(const QVariant& result)
    {
    Q_UNUSED(result)
    }

void UniViewerAddressWidget::handleError(int errorCode, const QString& errorMessage)
    {
    Q_UNUSED(errorMessage)
    Q_UNUSED(errorCode)
    }

void UniViewerAddressWidget::saveToContacts()
{
    //handler for save to contacts.
}

void UniViewerAddressWidget::sendMessage()
{
    HbAction* action = qobject_cast<HbAction*>(sender());

    if(action)
    {
        QString phoneNumber = action->data().toString();
        QString alias;

        QTextBlock textBlock = this->document()->findBlock(mCursorPos);

        QTextBlock::iterator it;

        for (it = textBlock.begin(); !(it.atEnd()); ++it)
        {
            QTextFragment currentFragment = it.fragment();

            if (currentFragment.isValid() && currentFragment.contains(mCursorPos)
                && currentFragment.charFormat().fontUnderline())
            {
                QString txt = currentFragment.text();
                if(txt != phoneNumber)
                {
                    alias = txt;  
                }
                break;
            }
        }
        

        //invoke editor & pass phoneNumber.
        emit sendMessage(phoneNumber,alias);
    }
}

// EOF
