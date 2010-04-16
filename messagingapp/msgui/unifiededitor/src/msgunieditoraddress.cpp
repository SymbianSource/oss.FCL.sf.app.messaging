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

// INCLUDES
#include "debugtraces.h"
#include <HbTextItem>
#include <HbPushButton>
#include <HbAction>
#include <hbinputeditorinterface.h>
#include <cntservicescontact.h>

// USER INCLUDES
#include "msgunieditoraddress.h"
#include "msgunifiededitorlineedit.h"
#include "msguiutilsmanager.h"
#include "matchphnumberutil.h"

const QString PBK_ICON(":/qtg_mono_contacts.svg");
const QString SEND_ICON(":/qtg_mono_send.svg");

// Constants

MsgUnifiedEditorAddress::MsgUnifiedEditorAddress( const QString& label,
                                                  const QString& pluginPath,
                                                  QGraphicsItem *parent ) :
HbWidget(parent),
mPluginPath(pluginPath),
mUtilsManager(0)
{
    #ifdef _DEBUG_TRACES_
    qDebug() << "MsgUnifiedEditorAddress calling HbStyle::registerPlugin";
    #endif

    setPluginBaseId(style()->registerPlugin(mPluginPath));
    mUtilsManager = new MsgUiUtilsManager(this);

    mLaunchBtn = new HbPushButton(this);
    HbStyle::setItemName(mLaunchBtn,"launchBtn");
    connect(mLaunchBtn,SIGNAL(clicked()),mUtilsManager,SLOT(fetchContacts()));

    mLaunchBtn->setIcon(HbIcon(PBK_ICON));

    mAddressEdit = new MsgUnifiedEditorLineEdit(label,this);
    HbStyle::setItemName(mAddressEdit,"addressField");

    mAddressEdit->setMaxRows(40);
    connect(mAddressEdit, SIGNAL(addressTextChanged(const QString&)),
            this, SLOT(onTextChanged(const QString&)));

    // add "Send" action in VKB
    HbEditorInterface editorInterface(mAddressEdit);
    HbAction *sendAction = new HbAction(HbIcon(SEND_ICON), QString(),this);
    connect(sendAction, SIGNAL(triggered()),this, SIGNAL(sendMessage()));
    editorInterface.addAction(sendAction);
    
    connect(mUtilsManager, SIGNAL(contactsFetched(const QVariant&)),
            this, SLOT(contactsFetched(const QVariant&)));
    }

MsgUnifiedEditorAddress::~MsgUnifiedEditorAddress()
{
    style()->unregisterPlugin(mPluginPath);
}

void MsgUnifiedEditorAddress::contactsFetched(const QVariant& value)
{
   CntServicesContactList contactList;
    contactList = qVariantValue<CntServicesContactList>(value);

    for(int i = 0; i < contactList.count(); i++ )
    {
        mAddressMap.insert(contactList[i].mPhoneNumber, contactList[i].mDisplayName);
        if(!contactList[i].mDisplayName.isEmpty())
        {
            mAddressEdit->setText(contactList[i].mDisplayName);
        }
        else
        {
            mAddressEdit->setText(contactList[i].mPhoneNumber);
        }
    }
}

ConvergedMessageAddressList MsgUnifiedEditorAddress::addresses()
{
    #ifdef _DEBUG_TRACES_
    qCritical() << "MsgUnifiedEditorAddress::address start";
#endif
    // sync-up map to account for user-actions on edit-field
    syncDeletionsToMap();
    syncAdditionsToMap();

    QStringList uniqueAddr;
    uniqueAddr = uniqueAddressList();

    ConvergedMessageAddressList addresses;
    foreach(QString addr, uniqueAddr)
    {
        ConvergedMessageAddress* address = new ConvergedMessageAddress;
        address->setAddress(addr);
        if(!mAddressMap.value(addr).isEmpty())
        {
            address->setAlias(mAddressMap.value(addr));
         }
        addresses.append(address);
    }
    #ifdef _DEBUG_TRACES_
    qCritical() << "MsgUnifiedEditorAddress::address end";
#endif
    return addresses;
}

int MsgUnifiedEditorAddress::addressCount()
{
    return mAddressEdit->addresses().count();
}

void MsgUnifiedEditorAddress::setAddresses(ConvergedMessageAddressList addrlist)
{
	int count = addrlist.count();
    for(int i = 0; i < count; i++ )
    {
        mAddressMap.insert(addrlist[i]->address(), addrlist[i]->alias());
        if(!addrlist[i]->alias().isEmpty())
        {
            mAddressEdit->setText(addrlist[i]->alias());
        }
        else
        {
            mAddressEdit->setText(addrlist[i]->address());
        }
    }
}

void MsgUnifiedEditorAddress::onTextChanged(const QString& text)
{
    if(!text.isEmpty())
    {
        disconnect(mAddressEdit, SIGNAL(addressTextChanged(const QString&)),
                this, SLOT(onTextChanged(const QString&)));
        emit mmContentAdded(true);
        connect(mAddressEdit, SIGNAL(addressTextChanged(const QString&)),
                this, SLOT(onTextRemoved(const QString&)));
    }
}

void MsgUnifiedEditorAddress::onTextRemoved(const QString& text)
{
    if(text.isEmpty())
    {
        disconnect(mAddressEdit, SIGNAL(addressTextChanged(const QString&)),
                this, SLOT(onTextRemoved(const QString&)));
        emit mmContentAdded(false);
        connect(mAddressEdit, SIGNAL(addressTextChanged(const QString&)),
                this, SLOT(onTextChanged(const QString&)));
    }
}

void MsgUnifiedEditorAddress::syncDeletionsToMap()
{
    // get address list from edit-field
    QStringList addrList = mAddressEdit->addresses();

    QMap<QString, QString>::iterator i = mAddressMap.begin();
    while (i != mAddressMap.end())
    {
        // check if either key or value is present in the list
        if( !(addrList.contains(i.value(), Qt::CaseSensitive) ||
              addrList.contains(i.key(), Qt::CaseSensitive)) )
        {// if none are present, then delete entry from map
            i = mAddressMap.erase(i);
        }
        else
        {
            ++i;
        }
    }
}

void MsgUnifiedEditorAddress::syncAdditionsToMap()
{
    QStringList mapAddrList = mAddressMap.values();

    // remove already mapped addresses from edit-field
    QStringList userInputAddrList(mAddressEdit->addresses());
    foreach(QString addr, mapAddrList)
    {
        userInputAddrList.removeAll(addr);
    }

    // add the unmapped addresses to address-map
    foreach(QString addr, userInputAddrList)
    {
        mAddressMap.insertMulti(addr, addr);
    }
}

QStringList MsgUnifiedEditorAddress::uniqueAddressList()
{
    int matchDigitCount = MatchPhNumberUtil::matchDigits();
    QStringList mapAddrList = mAddressMap.keys();
    for(int j = 0;j<mapAddrList.count()-1;j++)
    {
        for(int i =j+1;i<mapAddrList.count();i++)
        {
            if(0 == mapAddrList[j].right(matchDigitCount).compare(mapAddrList[i].right(matchDigitCount)))     
            {
               mapAddrList.removeAt(i);
               i--;
            }
        }
    }
    return mapAddrList;
}
Q_IMPLEMENT_USER_METATYPE(CntServicesContact)
Q_IMPLEMENT_USER_METATYPE_NO_OPERATORS(CntServicesContactList)

//EOF
