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

#ifndef MSG_CONTACTS_UTIL_H
#define MSG_CONTACTS_UTIL_H

#include <QObject>
#include "convergedmessageaddress.h"

class ConvergedMessage;
class XQServiceRequest;

/**
 * This class provides functionality for fetching contacts from phonebook.
 * 
 */
class MsgContactsUtil : public QObject
    {
    Q_OBJECT

public:
    /**
     * Constructor
     */
    MsgContactsUtil(QObject* parent =0);

    /**
     * Destructor
     */
    ~MsgContactsUtil();

    /**
     * Fectch conatcts
     */
    void fetchContacts();
    
    /*
     * Open Contact Details
     * @param contactId Conatct Id
     */
    void openContactDetails(qint32 contactId);

private:
        
    /**
     * Service Request
     * Owned
     */
    XQServiceRequest* mRequest;
    };

#endif // MSG_CONTACTS_UTIL_H

// EOF

