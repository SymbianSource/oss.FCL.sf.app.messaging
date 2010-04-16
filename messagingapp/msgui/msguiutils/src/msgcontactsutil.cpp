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
#include "msgcontactsutil.h"
#include "convergedmessageaddress.h"

//System Includes
#include <xqservicerequest.h>
#include <cntservicescontact.h>
#include <qtcontactsglobal.h>
#include <qtcontacts.h> 

QTM_USE_NAMESPACE

//---------------------------------------------------------------
// MsgContactsUtil::MsgContactsUtil
// @see header file
//---------------------------------------------------------------
MsgContactsUtil::MsgContactsUtil(QObject* parent):
QObject(parent),
mRequest(NULL)
{    
}

//---------------------------------------------------------------
// MsgContactsUtil::~MsgContactsUtil
// @see header file
//---------------------------------------------------------------
MsgContactsUtil::~MsgContactsUtil()
{
    if(mRequest)
    {
        delete mRequest;
        mRequest = NULL;
    }
}

//---------------------------------------------------------------
// MsgContactsUtil::~onRequestCompleted
// @see header file
//---------------------------------------------------------------
void MsgContactsUtil::fetchContacts()
    {
    if (mRequest)
        {
        delete mRequest;
        }
    QString interface("com.nokia.services.phonebookservices.Fetch");
    QString operation("fetch(QString,QString,QString)");
       
    mRequest = new XQServiceRequest(interface,operation,false);
    connect(mRequest, SIGNAL(requestCompleted(const QVariant&)),
            parent(), SIGNAL(contactsFetched(const QVariant&)));


    *mRequest << QString(tr("Phonebook")); // TODO: Localization
    *mRequest << KCntActionAll;
    *mRequest << KCntFilterDisplayAll;

    QVariant retValue;
    bool res=mRequest->send(retValue);
    }

void MsgContactsUtil::openContactDetails(qint32 contactId)
    {
    XQServiceRequest* mCntOpen;
    
    mCntOpen= new XQServiceRequest("com.nokia.services.phonebookservices.Fetch", "open(int)", false);
    
    // Fetch back the persisted contact
    QContactManager cm("symbian");
    QContact contact = cm.contact(contactId);
    
    QContactLocalId lId;
    lId = contact.localId();
    
    *mCntOpen<<lId;
    bool res=mCntOpen->send();
    delete mCntOpen;
    }
// EOF
