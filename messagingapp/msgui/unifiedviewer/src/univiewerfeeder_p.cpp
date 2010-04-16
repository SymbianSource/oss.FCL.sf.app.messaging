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
 * Description: Feeder Private class for unified viewer. Fetches data from the
 *              message store for the given message id.
 *
 */
#include "univiewerfeeder_p.h"
// SYSTEM INCLUDES
#include <mtclreg.h>
#include <mtclbase.h>
#include <msvstd.h>
#include <txtetext.h>
#include <txtrich.h>
#include <s60qconversions.h>
#include <msvids.h>
#include <qtcontactsglobal.h>
#include "qtcontacts.h"
#include "qcontactdetailfilter.h"

// USER INCLUDES
#include "nativemessageconsts.h"
#include "univiewerfeeder.h"
#include "unidatamodelloader.h"
#include "debugtraces.h"

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::UniViewerFeederPrivate
// @see header file
// ---------------------------------------------------------------------------
UniViewerFeederPrivate::UniViewerFeederPrivate(qint32 msgId,
                                               UniViewerFeeder* feeder) :
    q_ptr(feeder), mSlideCount(0), mSession(NULL)
{
    TRAP_IGNORE(initL(msgId));
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::~UniViewerFeederPrivate
// @see header file
// ---------------------------------------------------------------------------
UniViewerFeederPrivate::~UniViewerFeederPrivate()
{
    q_ptr = NULL;
    clearContent();
    if(pluginLoader)
    {
        delete pluginLoader;
        pluginLoader = NULL;
    }
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::initL
// Symbian specific constructions
// ---------------------------------------------------------------------------
//
void UniViewerFeederPrivate::initL(qint32 msgId)
{
    QDEBUG_WRITE("UniViewerFeederPrivate::initL start");
    TMsvId serviceId = KMsvNullIndexEntryId;
    pluginInterface = NULL;
    mMsgId = msgId;
    pluginLoader = new UniDataModelLoader();
    pluginLoader->loadPlugins();
    pluginInterface = pluginLoader->getDataModelPlugin("sms");

    mSession = pluginInterface->session();
    mSession->GetEntry(msgId, serviceId, mEntry);
    QDEBUG_WRITE("UniViewerFeederPrivate::initL end");
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::fetchDetails
// Fetches message details from the store
// ---------------------------------------------------------------------------
void UniViewerFeederPrivate::fetchDetails()
{
    TRAP_IGNORE(fetchDetailsL());
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::msgType
// Returns the message type.
// ---------------------------------------------------------------------------
qint32 UniViewerFeederPrivate::msgType()
{
    return mEntry.iMtm.iUid;
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::isIncoming
// Returns if it is a incoming message.
// ---------------------------------------------------------------------------
bool UniViewerFeederPrivate::isIncoming()
{
    if (mEntry.Parent() == KMsvGlobalInBoxIndexEntryId)
    {
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::priority
// Returns the message priority.
// ---------------------------------------------------------------------------
int UniViewerFeederPrivate::priority()
{
    if (mEntry.Priority() == EMsvHighPriority)
    {
        return ConvergedMessage::High;
    }
    else if (mEntry.Priority() == EMsvLowPriority)
    {
        return ConvergedMessage::Low;
    }
    return ConvergedMessage::Normal;
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::subject
// Returns the message subject.
// ---------------------------------------------------------------------------
QString UniViewerFeederPrivate::subject()
{
    return pluginInterface->subject();
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::timeStamp
// Returns the time stamp
// ---------------------------------------------------------------------------
QDateTime UniViewerFeederPrivate::timeStamp()
{

    return pluginInterface->timeStamp();
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::fetchDetailsL
// Fetches message details from the store.
// ---------------------------------------------------------------------------
void UniViewerFeederPrivate::fetchDetailsL()
{
    QDEBUG_WRITE("UniViewerFeederPrivate fetchDetailsL : SMS start");

    if (msgType() == KSenduiMtmSmsUidValue)
    {
        QString body;
        pluginInterface->body(body);
        q_ptr->emitMsgBody(body);
    }

    QDEBUG_WRITE("UniViewerFeederPrivate fetchDetailsL : SMS END");
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::hasAttachments
// @see header file
// ---------------------------------------------------------------------------
bool UniViewerFeederPrivate::hasAttachments()
{
    return pluginInterface->hasAttachment();
}
// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::attachmentsList
// @see header file
// ---------------------------------------------------------------------------
UniMessageInfoList UniViewerFeederPrivate::attachmentsList()
{
    return pluginInterface->attachmentList();
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::attachmentCount
// @see header file
// ---------------------------------------------------------------------------
int UniViewerFeederPrivate::attachmentCount()
{
    return pluginInterface->attachmentCount();
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::objectsList
// @see header file
// ---------------------------------------------------------------------------
UniMessageInfoList UniViewerFeederPrivate::objectsList()
{
    return pluginInterface->objectList();
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::objectCount
// @see header file
// ---------------------------------------------------------------------------
int UniViewerFeederPrivate::objectCount()
{
    return pluginInterface->objectCount();
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::slideCount
// @see header file
// ---------------------------------------------------------------------------
int UniViewerFeederPrivate::slideCount()
{
    return mSlideCount;
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::slideContent
// @see header file
// ---------------------------------------------------------------------------
UniMessageInfoList UniViewerFeederPrivate::slideContent(int slidenum)
{
    return pluginInterface->slideContent(slidenum);
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::updateContent
// @see header file
// ---------------------------------------------------------------------------
void UniViewerFeederPrivate::updateContent(qint32 msgId)
{
  
    if (msgId != mMsgId)
    {
        mMsgId = msgId;
        TMsvId serviceId = KMsvNullIndexEntryId;
        mSession->GetEntry(msgId, serviceId, mEntry);
        clearContent();
    }

    if (msgType() == KSenduiMtmSmsUidValue)
    {
        pluginInterface = pluginLoader->getDataModelPlugin("sms");
        pluginInterface->setMessageId(msgId);
    }

    if (msgType() == KSenduiMtmMmsUidValue)
    {
        pluginInterface = pluginLoader->getDataModelPlugin("mms");
        pluginInterface->setMessageId(msgId);
        mSlideCount = pluginInterface->slideCount();
    }
    pluginInterface->toRecipientList(mToAddressList);
    pluginInterface->ccRecipientList(mCcAddressList);
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::toAddressList
// @see header file
// ---------------------------------------------------------------------------
ConvergedMessageAddressList UniViewerFeederPrivate::toAddressList()
{
    for (int i = 0; i < mToAddressList.count(); ++i)
    {
        if (mToAddressList.at(i)->alias().isEmpty())
        {
            QString alias = QString();
            GetNameFromContacts(mToAddressList.at(i)->address(), alias);
            mToAddressList.at(i)->setAlias(alias);
            alias.clear();
        }
    }
    return mToAddressList;
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::ccAddressList
// @see header file
// ---------------------------------------------------------------------------
ConvergedMessageAddressList UniViewerFeederPrivate::ccAddressList()
{
    for (int i = 0; i < mCcAddressList.count(); ++i)
    {
        if (mCcAddressList.at(i)->alias().isEmpty())
        {
            QString alias = QString();
            GetNameFromContacts(mCcAddressList.at(i)->address(), alias);
            mCcAddressList.at(i)->setAlias(alias);
            alias.clear();
        }
    }
    return mCcAddressList;
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::messageSize
// @see header file
// ---------------------------------------------------------------------------
int UniViewerFeederPrivate::messageSize()
{
    return pluginInterface->messageSize();
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::fromAddressAndAlias
// @see header file
// ---------------------------------------------------------------------------
void UniViewerFeederPrivate::fromAddressAndAlias(QString& from, QString& alias)
{
    pluginInterface->fromAddress(from);
    GetNameFromContacts(from, alias);
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::clearContent
// @see header file
// ---------------------------------------------------------------------------
void UniViewerFeederPrivate::clearContent()
{
    for (int i = 0; i < mToAddressList.count(); ++i)
    {
        delete mToAddressList.at(i);
    }
    mToAddressList.clear();

    for (int i = 0; i < mCcAddressList.count(); ++i)
    {
        delete mCcAddressList.at(i);
    }

    mCcAddressList.clear();
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::GetNameFromContacts
// @see header file
//----------------------------------------------------------------------------
int UniViewerFeederPrivate::GetNameFromContacts(const QString& address,
                                                QString& alias)
{
    QContactManager contactManager("symbian");
    //set filter
    QContactDetailFilter phoneFilter;
    phoneFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName,
                                        QContactPhoneNumber::FieldNumber);
    phoneFilter.setMatchFlags(QContactFilter::MatchEndsWith);

    phoneFilter.setValue(address); // this is the phone number to be resolved

    QList<QContactLocalId> matchingContacts =
            contactManager.contacts(phoneFilter);
    int count = 0;
    if (matchingContacts.count() > 0)
    {
        QContact match = contactManager.contact(matchingContacts.at(0));
        
        QContactName name = match.detail(QContactName::DefinitionName);
        QString nickName(match.detail<QContactNickname> ().nickname());
        
        QString displayLabel = match.displayLabel();
          
        QList<QContactPhoneNumber> numbers =
                match.details<QContactPhoneNumber> ();
        count = numbers.count();
        if(displayLabel != "Unnamed")
        {
            alias.append(displayLabel);
        }
        else if(!nickName.isEmpty())
        {
            alias.append(nickName);
        }
            
    }
    return count;
}

// EOF
