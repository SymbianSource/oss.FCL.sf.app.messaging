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
 * Description: Message Application Launcher interface used for interfacing between
 *              QT highway and other applications
 *
 */

//TODO: to be deprecated
#include <qtcontactsglobal.h>
#include "qcontactdetailfilter.h"
#include "qcontactdetail.h"
#include "qtcontacts.h" 
#include "convergedmessageaddress.h"

#include "msgserviceinterface.h"
#include "msgviewmanager.h"
//INCLUDES

MsgServiceInterface::MsgServiceInterface(QObject* parent, MsgViewManager* manager)
:XQServiceProvider( QLatin1String("com.nokia.services.hbserviceprovider.conversationview"),parent),
mViewManager(manager)
    {
    publishAll();    
    }

MsgServiceInterface::~MsgServiceInterface()
    {
    }

void MsgServiceInterface::send(const QString phoneNumber, const qint32 contactId, const QString displayName)
    {   
    mViewManager->setServiceRequest(true);
    mViewManager->send(contactId, phoneNumber, displayName);    
    }


void MsgServiceInterface::send(const QString phoneNumber, const QString alias, const QString bodyText)
    {   
    mViewManager->setServiceRequest(true);
    mViewManager->send(phoneNumber, alias, bodyText);    
    }


void MsgServiceInterface::open(qint64 conversationId)
    {
    mViewManager->setServiceRequest(false);
    mViewManager->open(conversationId); 
    }

void MsgServiceInterface::send(QVariant data)
    {   
    mViewManager->setServiceRequest(true);
    mViewManager->send(data);    
    }

void MsgServiceInterface::view(int msgId)
    {   
    mViewManager->view(msgId); 
    }
    
void MsgServiceInterface::openConversationView(QString number, QString name)
    {
    mViewManager->setServiceRequest(true);
        
    QString resolvedName;
    if(name.isEmpty())
        {
        ConvergedMessageAddress address;
        address.setAddress(number);
        address.setAlias(name);        
        
        ContactDetail contactDetail;
        bool ret = resolveContact(address, contactDetail);
        if(ret)
            {
            resolvedName = contactDetail.displayName;
            }
        }
    mViewManager->openEditor(number,resolvedName);
    }

bool MsgServiceInterface::resolveContact(const ConvergedMessageAddress &address,
        ContactDetail &contactDetail)
    {
    QContactManager* mPhonebookManager = new QContactManager("symbian");
    QContactDetailFilter phoneFilter;
    phoneFilter.setDetailDefinitionName(
            QContactPhoneNumber::DefinitionName, 
            QContactPhoneNumber::FieldNumber);
    phoneFilter.setValue(address.address());
    phoneFilter.setMatchFlags(QContactFilter::MatchEndsWith);

    QList<QContactSortOrder> sortOrder;
    QList<QContact> matchingContacts = mPhonebookManager->contacts(
            phoneFilter,
            sortOrder,
            QStringList());

    if ( matchingContacts.count() > 0 ) {       
        // Fill the contact details
        QContact match = matchingContacts.at(0);                   
        contactDetail.contactId = match.localId();
        contactDetail.displayName = match.displayLabel();   
        return true;
    }
    
    return false;
    }

