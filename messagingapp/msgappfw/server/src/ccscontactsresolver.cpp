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
 * Description: QT contact resolver interface
 *
 */

// SYSTEM INCLUDES
//#include <QtContacts> 
#include "qtcontacts.h" 
#include "qcontactdetailfilter.h"
#include "qcontactdetail.h"

#include <QList>
// USER INCLUDES
#include "ccscontactsresolver.h"
#include "qcontactnickname.h"
#include "qcontactorganization.h"
// ======================== Member Functions ====================

// ----------------------------------------------------------------------------
// CCsContactsResolver::ContactsResolver
// @see contactsresolver.h
// ----------------------------------------------------------------------------
CCsContactsResolver::CCsContactsResolver(QContactManager* manager):
mPhonebookManager(manager)
        {
        // do nothing
        }
// ----------------------------------------------------------------------------
// CCsContactsResolver::~ContactsResolver
// @see contactsresolver.h
// ----------------------------------------------------------------------------
CCsContactsResolver::~CCsContactsResolver()
    {
    // do nothing
    }

// ----------------------------------------------------------------------------
// CCsContactsResolver::resolveContact
// @see contactsresolver.h
// ----------------------------------------------------------------------------
bool CCsContactsResolver::resolveContact(
        const QString &address,
        CCsContactDetail &contactDetail)
    {
    QContactDetailFilter phoneFilter;
    phoneFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    phoneFilter.setValue(address);
    phoneFilter.setMatchFlags(QContactFilter::MatchEndsWith);

    QList<QContactLocalId> matchingContacts = mPhonebookManager->contacts(phoneFilter);

    if ( matchingContacts.count() > 0 ) {		
    // Fill the contact details
    contactDetail.contactId = matchingContacts.at(0);
    QContact match = mPhonebookManager->contact(matchingContacts.at(0));

    QContactName nameDetails = match.detail(QContactName::DefinitionName);

    QContactName name = match.detail(QContactName::DefinitionName);
    contactDetail.firstName = name.first();
    contactDetail.lastName = name.last();
    
    contactDetail.nickName = match.detail<QContactNickname>().nickname();
    
    //campany name QContactOrganization
    contactDetail.companyName = match.detail<QContactOrganization>().name();
    
    return true;

    }		
    return false;
    }

// ----------------------------------------------------------------------------
// CCsContactsResolver::resolveContactId
// @see contactsresolver.h
// ----------------------------------------------------------------------------
void CCsContactsResolver::resolveContactId(
        const quint32 &contactId,
        CCsContactDetail &contactDetail)	
    {
    // Fetch back the persisted contact
    QContact contact = mPhonebookManager->contact(contactId);
    contactDetail.contactId = contact.localId(); 
    QContactName name = contact.detail(QContactName::DefinitionName);
    contactDetail.firstName = name.first();
    contactDetail.lastName = name.last();
    
    QList<QContactPhoneNumber> numbers = contact.details<QContactPhoneNumber>();
    int numberCount = numbers.count();

    for ( int a=0; a<numberCount ;++a)
        {
        QString phoneNumber= numbers.at(a).number();
        contactDetail.addressList.append(phoneNumber);
        }
    
    QContactNickname nickName = contact.detail(QContactName::DefinitionName);
    contactDetail.nickName = nickName.nickname();
    
    //company name QContactOrganization
    contactDetail.companyName = contact.detail<QContactOrganization>().name();
    }

// EOF
