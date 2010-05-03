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
 * Description: Utility class for conersationengine.
 */
#include <QFile>
#include <qversitcontactimporter.h>
#include <qversitdocument.h>
#include <qversitreader.h>
#include <qtcontacts.h>
#include <qmobilityglobal.h>
#include <senduiconsts.h>

#include "conversationsengineutility.h"
#include "convergedmessage.h"

QTM_USE_NAMESPACE


//---------------------------------------------------------------
// ConversationsEngineUtility::messageType
// @see header
//---------------------------------------------------------------
int ConversationsEngineUtility::messageType(TCsType value)
{
    int mMessageType;
    switch (value) {
    case ECsSMS:
        mMessageType = ConvergedMessage::Sms;
        break;
    case ECsMMS:
    case ECsAudio:
        mMessageType = ConvergedMessage::Mms;
        break;
    case ECsBioMsg:
    case ECsRingingTone:
    case ECsProvisioning:
    case ECsBioMsg_VCard:
    case ECsBioMsg_VCal:
        mMessageType = ConvergedMessage::BioMsg;
        break;
    case ECsBlueTooth:
        mMessageType = ConvergedMessage::BT;
        break;
    case ECsMmsNotification:
        mMessageType = ConvergedMessage::MmsNotification;
        break;
    default:
        mMessageType = ConvergedMessage::None;
        break;
    }
    return mMessageType;
}

//---------------------------------------------------------------
// ConversationsEngineUtility::mapMsgType
// @see header
//---------------------------------------------------------------
int ConversationsEngineUtility::messageType(TInt32 value)
{
    int type;
    switch (value) {
    case KSenduiMtmSmsUidValue:
        type = ConvergedMessage::Sms;
        break;
    case KSenduiMtmBtUidValue:
        type = ConvergedMessage::BT;
        break;
    case KSenduiMtmMmsUidValue:
        type = ConvergedMessage::Mms;
        break;
    case KSenduiMtmBioUidValue:
        type = ConvergedMessage::BioMsg;
        break;
    default:
        type = ConvergedMessage::None;
        break;
    }
    return type;
}

//---------------------------------------------------------------
// ConversationsEngineUtility::messageSubType
// @see header
//---------------------------------------------------------------
int ConversationsEngineUtility::messageSubType(TCsType value)
{
    int messageSubType;
    switch (value) {
    case ECsRingingTone:
        messageSubType = ConvergedMessage::RingingTone;
        break;
    case ECsProvisioning:
        messageSubType = ConvergedMessage::Provisioning;
        break;
    case ECsBioMsg_VCard:
        messageSubType = ConvergedMessage::VCard;
        break;
    case ECsBioMsg_VCal:
        messageSubType = ConvergedMessage::VCal;
        break;
    case ECsAudio:
        messageSubType = ConvergedMessage::Audio;
        break;
    default:
        messageSubType = ConvergedMessage::None;
        break;
    }
    return messageSubType;
}

//---------------------------------------------------------------
// ConversationsEngineUtility::getVcardDisplayName
// @see header
//---------------------------------------------------------------
QString ConversationsEngineUtility::getVcardDisplayName(QString filePath)
{
    QString displayName;
    //open file for parsing
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return displayName;
    }
    // parse contents
    QVersitReader reader;
    reader.setDevice(&file);
    if (reader.startReading()) {
        if (reader.waitForFinished()) {
            QList<QVersitDocument> versitDocuments = reader.results();
            // Use the resulting document
            if (versitDocuments.count() > 0) {
                QVersitContactImporter importer;
                QList<QContact> contacts = importer.importContacts(versitDocuments);
                // get display-name
                if (contacts.count() > 0) {
                    QContactManager* contactManager = new QContactManager("symbian");
                    displayName = contactManager->synthesizedDisplayLabel(contacts[0]);
                    delete contactManager;
                }
            }
        }
    }
    file.close();

    return displayName;
}

// End of file
