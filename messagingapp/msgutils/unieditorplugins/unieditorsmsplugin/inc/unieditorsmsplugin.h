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

#ifndef UNIEDITORSMSPLUGIN_H_
#define UNIEDITORSMSPLUGIN_H_


#include <QObject>
#include "unieditorplugininterface.h"

class UniEditorSmsPluginPrivate;

/**
 * Unified editor send SMS plugin.
 */
class UniEditorSmsPlugin : public QObject, public UniEditorPluginInterface
{
Q_OBJECT
    Q_INTERFACES(UniEditorPluginInterface)

public:
    /**
     * Constructor
     */
    UniEditorSmsPlugin(QObject* parent = 0);

    /**
     * Destructor
     */
    ~UniEditorSmsPlugin();

    /**
     *  Message type (SMS/MMS etc..)
     *  @return type of message
     */
    ConvergedMessage::MessageType messageType();

    /**
     * convert from message store entry to convergedmessage type
     * @param aId message entry id
     * @return convergedmessage
     */
    ConvergedMessage* convertFrom( TMsvId aId );
    
    /**
     * delete entry from drafts folder
     * @param aId message entry id to be deleted
     */
    void deleteDraftsEntry( TMsvId aId );

    /**
     * convert from convergedmessage to entry in message store
     * @param aMessage convergedmessage
     * @return message entry id
     */
    TMsvId convertTo( ConvergedMessage *aMessage );

    /**
     * send message
     * @param message entry id
     * @return true, if send is successfull
     */
    bool send(TMsvId aId);

    /**
     * check if service is valid
     * @return true/false
     */
    TBool isServiceValid();

    /**
     * validate settings
     * @param aEmailOverSms true if email over sms is supported, else false
     * @return true/false
     */
    TBool validateService( TBool aEmailOverSms = EFalse );

private:
    UniEditorSmsPluginPrivate* d_ptr;
};

#endif //UNIEDITORSMSPLUGIN_H_
