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
 * Description: This header handles the phonebook manager resolve number static
 *              functions.
 *
 */

#ifndef MSGCONTACTHANDLER_H_
#define MSGCONTACTHANDLER_H_

#include <qmobilityglobal.h>

#include <qtcontacts.h>
#include <qcontactfilter.h>
#include <qcontactdetailfilter.h>
#include <QFile>
#include <qversitcontactimporter.h>
#include <qversitdocument.h>
#include <qversitreader.h>

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class MsgContactHandler
{
    
public:

    /**
     * This shall resolve contact number with display name
     * @param contactNumber number to resolve
     * @param displayName resolved name
     * @param countPhoneNumber specifies number of contacts inside 
     * the resolved contact ex mobile, home, office etc
     * @return contacts unique localId
     */
    static int resolveContactDisplayName(const QString& contactNumber,
                                         QString& displayName,
                                         int& countPhoneNumber)
    {
        QContactManager * phonebookManager;
        QContactDetailFilter phoneFilter;
        phonebookManager = new QContactManager("symbian");
        phoneFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName,
                                            QContactPhoneNumber::FieldNumber);

        QVariant address(contactNumber);
        phoneFilter.setValue(address);
        phoneFilter.setMatchFlags(QContactFilter::MatchEndsWith);

        QList<QContact> matchingContacts =
                phonebookManager->contacts(phoneFilter);

        if (matchingContacts.count() > 0)
        {
            // Fill the contact details
            QContact match = matchingContacts.at(0);

            displayName = match.displayLabel();
            QList<QContactPhoneNumber> numbers = 
                    match.details<QContactPhoneNumber> ();
            countPhoneNumber = numbers.count();
            return match.localId();
        }
        else // no matching contacts
        {
            displayName = contactNumber;
            return -1;
        }
    }
    
    /**
     * This shall resolve contact number with display name
     * @param contactNumber number to resolve
     * @param fieldName specifies filter name
     * @param fieldType specifies filter type
     * @return contact unique local id
     */
    static int resolveContactDisplayName(const QString& contactNumber,
                                         const QString& fieldName,
                                         const QString& fieldType)
    {
        int contactId = -1;

        QContactManager phonebookManager("symbian");

        QContactDetailFilter phoneFilter;
        phoneFilter.setDetailDefinitionName(fieldName, fieldType);
        phoneFilter.setValue(contactNumber);
        phoneFilter.setMatchFlags(QContactFilter::MatchEndsWith);

        QList<QContact>
                matchingContacts = phonebookManager.contacts(phoneFilter);

        if (matchingContacts.count() > 0)
        {
            contactId = matchingContacts.at(0).localId();
        }
        return contactId;
    }

    //---------------------------------------------------------------
    // findContactList
    // Finds the contact matching the phone number.
    // @param phoneNumber Phone number to be matchned.
    // @return List of matching contacts.
    //---------------------------------------------------------------
    static QList<QContact> findContactList(const QString &phoneNumber)
    {
        QContactManager phonebookManager("symbian");

        QContactDetailFilter phoneFilter;
        phoneFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName,
            QContactPhoneNumber::FieldNumber);
        phoneFilter.setValue(phoneNumber);
        phoneFilter.setMatchFlags(QContactFilter::MatchEndsWith);
        
        QList<QContact> matchingContacts = phonebookManager.contacts(phoneFilter);

        return matchingContacts;
    }
    
    /**
     * Get display-name of a contact from VCard.
     * @param filePath, VCard file-path
     * @return display-name
     */

    static QString getVCardDisplayName(QString& filePath)
    {
        QString displayName = QString("");
        //open file for parsing
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly))
        {
            return displayName;
        }
        // parse contents
        QVersitReader reader;
        reader.setDevice(&file);
        if (reader.startReading())
        {
            if (reader.waitForFinished())
            {
                QList<QVersitDocument> versitDocuments = reader.results();
                // Use the resulting document
                if (versitDocuments.count() > 0)
                {
                    QVersitContactImporter importer;
                    QList<QContact> contacts =
                            importer.importContacts(versitDocuments);
                    // get display-name
                    if (contacts.count() > 0)
                    {
                        //resolveSynthesizedDisplayLabel
                        QContactManager* contactManager =
                                new QContactManager("symbian");
                        displayName
                                = contactManager->synthesizedDisplayLabel(contacts[0]);
                        delete contactManager;
                    }
                }
            }
        }
        file.close();
        return displayName;
    }
};

#endif /* MSGCONTACTHANDLER_H_ */
