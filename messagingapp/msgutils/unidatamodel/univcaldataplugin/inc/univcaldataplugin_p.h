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
 * Description: Private implementaion of sms data model plugin.
 *
 */

#ifndef UNIVCALDATAPLUGINPRIVATE_H_
#define UNIVCALDATAPLUGINPRIVATE_H_

#include "unidatamodelplugininterface.h"

//Forward Declarations
class CClientMtmRegistry;
class ConvergedMessage;
class UniVCalDataPlugin;
class CBIOClientMtm;
class CMsvSession;
class SessionEventHandler;

/**
 * Symbian specific implementation of the uni SMS plugin
 */
class UniVCalDataPluginPrivate
{

public:
    /**
     * Constructor
     */
    UniVCalDataPluginPrivate(UniVCalDataPlugin* plugin);

    /**
     * Destructor
     */
    ~UniVCalDataPluginPrivate();

    /**
     *  Resets the datamodel
     */
    void reset();

    /**
     * sets the message id for which details needs to be fetched
     * @param message id
     */
    int setMessageId(int mId);

    /**
     * Get the body of the message
     * @param aBodyText, for message body
     */
    void body(QString& aBodyText);

    /**
     * Size of the message
     * @param message size
     */
    int messageSize();

    /**
     * List of the message TO recipients
     * @param list of the recipients
     */
    void toRecipientList(ConvergedMessageAddressList& mAddressList);

    /**
     * Address from which the message was received
     * @param messageAddress
     */
    void fromAddress(QString& messageAddress);

    /**
     * Time stamp of the message
     * @return time stamp of the created/received message
     */
    int timeStamp();

    /**
     * The File handle of the VCAL attachment
     * @return file handle
     */
    RFile attachmentL();

    /**
     * Priority of the message
     * @return message priority
     */
    MsgPriority messagePriority();

    /**
     * Number of attachments associated with the message..
     * @return  attachment count
     */
    int attachmentCount();

	/**
	 * Session with the Messaging server
	 * @return CMsvSession*
	 */
    CMsvSession* session();

private:
    /**
     *  Extracts the name and address from the CMsvRecipientList
     */
    void extractNameAndAddress(const TDesC& aMsvAddress, TPtrC& aName,
                               TPtrC& aAddress);
    
    /**
     * intitlaises the CMsvSession and mtm
     */
    void initL();
    
    /**
     * setMessageIdL
     * @param int msgId
     * @return void
     */
    void setMessageIdL(int msgId);
private:
    UniVCalDataPlugin* q_ptr;
    CMsvSession* iMSession;
    CClientMtmRegistry* iMtmReg;
    CBIOClientMtm* iBioClientMtm;
    TMsvId iMessageId;
    TBool attachmentProcessed;
    CMsvEntry* iMsvEntry;
    SessionEventHandler *iSessionHandler;
    TBool done;
};
#endif
