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

//SYSTEM INCLUDES
#include <mtclreg.h>
#include <mtclbase.h>
#include <s60qconversions.h>

//USER INCLUDES
#include "univcaldataplugin.h"
#include "univcaldataplugin_p.h"

// UniVCalDataPlugin::UniVCalDataPlugin()
// @see header
//---------------------------------------------------------------
UniVCalDataPlugin::UniVCalDataPlugin(QObject* parent) :
    QObject(parent)
    {
    d_ptr = new UniVCalDataPluginPrivate(this);
    }

// UniVCalDataPlugin::~UniVCalDataPlugin()
// @see header
//---------------------------------------------------------------
UniVCalDataPlugin::~UniVCalDataPlugin()
{
    delete d_ptr;
}

// UniVCalDataPlugin::createInstance()
// @see header
//---------------------------------------------------------------
QObject* UniVCalDataPlugin::createInstance()
{
return new UniVCalDataPlugin();
}	

// UniVCalDataPlugin::reset()
// @see header
//---------------------------------------------------------------
void UniVCalDataPlugin::reset()
{
    d_ptr->reset();
}

// UniVCalDataPlugin::setMessageId()
// @see header
//---------------------------------------------------------------
int UniVCalDataPlugin::setMessageId(int mId)
{
    return d_ptr->setMessageId(mId);
}

// UniVCalDataPlugin::body()
// @see header
//---------------------------------------------------------------
void UniVCalDataPlugin::body(QString& aBodyText)
{
    d_ptr->body(aBodyText);
}

// UniVCalDataPlugin::messageSize()
// @see header
//---------------------------------------------------------------
qint32 UniVCalDataPlugin::messageSize()
{
    return d_ptr->messageSize();
}

// UniVCalDataPlugin::toRecipientList()
// @see header
//---------------------------------------------------------------
void UniVCalDataPlugin::toRecipientList(
    ConvergedMessageAddressList& mAddressList)
{
    d_ptr->toRecipientList(mAddressList);
}

// UniVCalDataPlugin::ccRecipientList()
// @see header
//---------------------------------------------------------------
void UniVCalDataPlugin::ccRecipientList(ConvergedMessageAddressList& /*mAddressList*/)
{

}

// UniVCalDataPlugin::bccRecipientList()
// @see header
//---------------------------------------------------------------

void UniVCalDataPlugin::bccRecipientList(ConvergedMessageAddressList& /*mAddressList*/)
{

}

// UniVCalDataPlugin::fromAddress()
// @see header
//---------------------------------------------------------------
void UniVCalDataPlugin::fromAddress(QString& messageAddress)
{
    d_ptr->fromAddress(messageAddress);
}

// UniVCalDataPlugin::attachmentList()
// @see header
//---------------------------------------------------------------
UniMessageInfoList UniVCalDataPlugin::attachmentList()
{
    RFile file = d_ptr->attachmentL();

    UniMessageInfoList attlist;

    QString path;
    QString mimetype;
    int size;

    TFileName fullName;
    User::LeaveIfError(file.FullName(fullName));
    User::LeaveIfError(file.Size(size));

    path = S60QConversions::s60DescToQString(*fullName.AllocL());

    UniMessageInfo *msgobj = new UniMessageInfo(path, size, mimetype);
    attlist << msgobj;
    file.Close();
    return attlist;
}

// UniVCalDataPlugin::messageType()
// @see header
//---------------------------------------------------------------
QString UniVCalDataPlugin::messageType()
{
    QString str("bio:vcal");
    return str;
}

// UniVCalDataPlugin::messagePriority()
// @see header
//---------------------------------------------------------------
MsgPriority UniVCalDataPlugin::messagePriority()
{
    return d_ptr->messagePriority();
}

// UniVCalDataPlugin::attachmentCount()
// @see header
//---------------------------------------------------------------
int UniVCalDataPlugin::attachmentCount()
{
    return d_ptr->attachmentCount();
}

// UniVCalDataPlugin::hasAttachment()
// @see header
//---------------------------------------------------------------
bool UniVCalDataPlugin::hasAttachment()
{
    return true;
}

// UniVCalDataPlugin::objectCount()
// @see header
//---------------------------------------------------------------
int UniVCalDataPlugin::objectCount()
{
    return 0;
}

// UniVCalDataPlugin::objectList()
// @see header
//---------------------------------------------------------------
UniMessageInfoList UniVCalDataPlugin::objectList()
{
    return QList<UniMessageInfo*> ();
}

// UniVCalDataPlugin::slideCount()
// @see header
//---------------------------------------------------------------
int UniVCalDataPlugin::slideCount()
{
    return 0;
}

// UniVCalDataPlugin::slideContent()
// @see header
//---------------------------------------------------------------
UniMessageInfoList UniVCalDataPlugin::slideContent(int /*slidenum*/)
{
    return QList<UniMessageInfo*> ();
}

// UniVCalDataPlugin::timeStamp()
// @see header
//---------------------------------------------------------------
QDateTime UniVCalDataPlugin::timeStamp()
{
    QDateTime retTimeStamp;
    int tStamp = d_ptr->timeStamp();
    retTimeStamp.setTime_t(tStamp);
    return retTimeStamp;
}

//---------------------------------------------------------------
// UniVCalDataPlugin::addObject()
// @see header
//---------------------------------------------------------------
void UniVCalDataPlugin::addObject( int /*aslideNum*/,
    UniMessageInfo* /*aInfo*/ )
{

}

//---------------------------------------------------------------
// UniVCalDataPlugin::addTextObject()
// @see header
//---------------------------------------------------------------
void UniVCalDataPlugin::addTextObject(int /*aSlideNum*/,
    QString /*aText*/ )
{

}

//---------------------------------------------------------------
// UniVCalDataPlugin::addSlide()
// @see header
//---------------------------------------------------------------
void UniVCalDataPlugin::addSlide( int /*aSlideNum*/ )
{

}   

//---------------------------------------------------------------
// UniVCalDataPlugin::composeSmil()
// @see header
//---------------------------------------------------------------
void UniVCalDataPlugin::composeSmil(CMsvStore& /*aEditStore*/)
{

}

//---------------------------------------------------------------
// UniVCalDataPlugin::saveObjects()
// @see header
//---------------------------------------------------------------
void UniVCalDataPlugin::saveObjects()
{

}

//---------------------------------------------------------------
// UniVCalDataPlugin::getSmilAttachmentByIndex()
// @see header
//---------------------------------------------------------------
TMsvAttachmentId UniVCalDataPlugin::getSmilAttachmentByIndex() 
{
    return NULL;
}       

//---------------------------------------------------------------
// UniVCalDataPlugin::restore()
// @see header
//---------------------------------------------------------------
void UniVCalDataPlugin::restore(CBaseMtm& /*mtm*/)
{

}

//---------------------------------------------------------------
// UniVCalDataPlugin::addAttachment()
// @see header
//---------------------------------------------------------------
void UniVCalDataPlugin::addAttachment( UniMessageInfo* /*aInfo*/ )
{

}

//---------------------------------------------------------------
// UniVCalDataPlugin::removeSlide()
// @see header
//---------------------------------------------------------------
void UniVCalDataPlugin::removeSlide(int /*aSlideNumber*/)
{

}

//---------------------------------------------------------------
// UniVCalDataPlugin::subject()
// @see header
//---------------------------------------------------------------
QString UniVCalDataPlugin::subject()
{
  return QString();   
}


//---------------------------------------------------------------
// UniVCalDataPlugin::session()
// @see header
//---------------------------------------------------------------
CMsvSession* UniVCalDataPlugin::session()
{
    return d_ptr->session();
}



Q_EXPORT_PLUGIN2(UniVCalDataPlugin, UniVCalDataPlugin)

