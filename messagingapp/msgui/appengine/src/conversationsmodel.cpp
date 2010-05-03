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

#include "conversationsmodel.h"
#include "conversationsenginedefines.h"
#include "conversationmsgstorehandler.h"
#include "convergedmessage.h"
#include "s60qconversions.h"
#include "conversationsengineutility.h"
#include "unidatamodelloader.h"
#include "unidatamodelplugininterface.h"
#include <ccsconversationentry.h>
#include <QFile>
#include <QFileInfo>

// LOCAL CONSTANTS

//---------------------------------------------------------------
// ConversationsModel::ConversationsModel
// Constructor
//---------------------------------------------------------------
ConversationsModel::ConversationsModel(ConversationMsgStoreHandler* msgStoreHandler,
    QObject* parent) :
    QStandardItemModel(parent), mMsgStoreHandler(msgStoreHandler)
{
    iDataModelPluginLoader = new UniDataModelLoader;
    iMmsDataPlugin = iDataModelPluginLoader->getDataModelPlugin(ConvergedMessage::Mms);
    iBioMsgPlugin = iDataModelPluginLoader->getDataModelPlugin(ConvergedMessage::BioMsg);
}

//---------------------------------------------------------------
// ConversationsModel::~ConversationsModel
// Destructor
//---------------------------------------------------------------
ConversationsModel::~ConversationsModel()
{
    if (iDataModelPluginLoader) {
        delete iDataModelPluginLoader;
        iDataModelPluginLoader = NULL;
    }
}

//---------------------------------------------------------------
// ConversationsModel::data
// @see header
//---------------------------------------------------------------
QVariant ConversationsModel::data(const QModelIndex & index, int role) const
{
    QVariant value;
    QStandardItem* item = itemFromIndex(index);
    switch (role) {
    case ConversationId:
    {
        value = item->data(ConversationId);
        break;
    }
    case UnReadStatus:
    {
        value = item->data(UnReadStatus);
        break;
    }
    case ContactId:
    {
        value = item->data(ContactId);
        break;
    }
    case TimeStamp:
    {
        value = item->data(TimeStamp);
        break;
    }
    case ConvergedMsgId:
    {
        value = item->data(ConvergedMsgId);
        break;
    }
    case MessageProperty:
    {
        value = item->data(MessageProperty);
        break;
    }
    case MessageType:
    {
        value = item->data(MessageType);
        break;
    }
    case MessageSubType:
    {
        value = item->data(MessageSubType);
        break;
    }
    case Subject:
    {
        value = item->data(Subject);
        break;
    }
    case BodyText:
    {
        value = item->data(BodyText);
        break;
    }
    case ConversationAddress:
    {
        value = item->data(ConversationAddress);
        break;
    }
    case Direction:
    {
        value = item->data(Direction);
        break;
    }
    case SendingState:
    {
        value = item->data(SendingState);
        break;
    }
    case MessagePriority:
    {
        value = item->data(MessagePriority);
        break;
    }
    case Attachments:
    {
        value = item->data(Attachments);
        break;
    }
    case MessageLocation:
    {
        value = item->data(MessageLocation);
        break;
    }
    case MessageStore:
    {
        value = item->data(MessageStore);
        break;
    }
    case ConversationAlias:
    {
        value = item->data(ConversationAlias);
        break;
    }
    case UnreadCount:
    {
        value = item->data(UnreadCount);
        break;
    }
    case DisplayName: // Fall through start
        value = item->data(DisplayName);
        break;    
    case Avatar: // Fall througn end
        value = item->data(Avatar);
        break;
    case NotificationStatus:
        value = item->data(NotificationStatus);
        break;
    default:
    {
        //No matching role found, set invalid variant
        value = QVariant();
        break;
    }
    }
    return value;
}

//---------------------------------------------------------------
// ConversationsModel::addRow
// @see header
//---------------------------------------------------------------
void ConversationsModel::addRow(const CCsConversationEntry& entry, bool dynamicUpdate)
{
    int msgId = entry.EntryId();
    //match, if found update else add item
    QModelIndexList indexList = this->match(index(0, 0), ConvergedMsgId, msgId, -1,
        Qt::MatchExactly);

    // if not found, add new item
    if (indexList.count() == 0) {
        QStandardItem* item = new QStandardItem();
        populateItem(*item, entry);
        if (!dynamicUpdate) {
            insertRow(0, item);
        }
        else {
            int i;
            for (i = rowCount() - 1; i >= 0; --i) {
                QStandardItem* modelItem = this->item(i, 0);
                if (modelItem->data(ConvergedMsgId).toInt() < item->data(ConvergedMsgId).toInt()) {
                    if (i == rowCount() - 1) {
                        appendRow(item);
                    }
                    else {
                        insertRow(i + 1, item);
                    }
                    return;
                }
            }
            if (i == 0) {
                insertRow(0, item);
            }
        }
    }
    else {
        // Update an existing item
        QModelIndex index = indexList[0];
        QStandardItem* item = this->item(index.row(), 0);
        populateItem(*item, entry);
    }
}

//---------------------------------------------------------------
// ConversationsModel::deleteRow
// @see header
//---------------------------------------------------------------
void ConversationsModel::deleteRow(int msgId)
{
    //match, if found remove item
    QModelIndexList indexList =
        this->match(index(0, 0), ConvergedMsgId, msgId, 1, Qt::MatchExactly);

    if (indexList.count() == 1) {
        QModelIndex index = indexList[0];
        this->removeRow(index.row());
    }
}

//---------------------------------------------------------------
// ConversationsModel::populateItem
// @see header
//---------------------------------------------------------------
void ConversationsModel::populateItem(QStandardItem& item, const CCsConversationEntry& entry)
{
    int msgId = entry.EntryId();
    // id
    item.setData(msgId, ConvergedMsgId);

    // description
    HBufC* description = entry.Description();
    QString subject("");
    if (description && description->Length()) {
        subject = (S60QConversions::s60DescToQString(*description));     
    }

    // time stamp 
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    TTime timeStamp(entry.TimeStamp());
    timeStamp.SecondsFrom(unixEpoch, seconds);
    item.setData(seconds.Int(), TimeStamp);

    //contact details
    HBufC* contact = entry.Contact();
    if (contact && contact->Length()) {
        item.setData(S60QConversions::s60DescToQString(*contact), ConversationAddress);
    }

    // message type.
    int msgType = ConversationsEngineUtility::messageType(entry.GetType());
    item.setData(msgType, MessageType);

    //message sub-type
    item.setData(ConversationsEngineUtility::messageSubType(entry.GetType()), MessageSubType);

    // unread status
    if (entry.IsAttributeSet(ECsAttributeUnread)) {
        item.setData(true, UnReadStatus);
    }
    else {
        item.setData(false, UnReadStatus);
    }

    //sending state
    item.setData(entry.GetSendState(), SendingState);
    // direction
    item.setData(entry.ConversationDir(), Direction);

    //location
    if (entry.ConversationDir() == ECsDirectionIncoming) {
        item.setData(ConvergedMessage::Inbox, MessageLocation);
    }
    else if (entry.IsAttributeSet(ECsAttributeDraft)) {
        item.setData(ConvergedMessage::Draft, MessageLocation);
    }
    else if (entry.IsAttributeSet(ECsAttributeSent)) {
        item.setData(ConvergedMessage::Sent, MessageLocation);
    }
    else {
        item.setData(ConvergedMessage::Outbox, MessageLocation);
    }

    //message specific handling    
    if (msgType == ConvergedMessage::Mms) {
        handleMMS(item, entry);
    }
    else if(msgType == ConvergedMessage::MmsNotification) {
        item.setData(subject, Subject);
        handleMMSNotification(item, entry);
    }
    else if (msgType == ConvergedMessage::BT) {
        handleBlueToothMessages(item, entry);
    }
    else if (msgType == ConvergedMessage::BioMsg) {
        handleBioMessages(item, entry);
    }
    else {
        // sms bodytext
        item.setData(subject, BodyText);
    }
}

//---------------------------------------------------------------
// ConversationsModel::handleMMS
// @see header
//---------------------------------------------------------------
void ConversationsModel::handleMMS(QStandardItem& item, const CCsConversationEntry& entry)
{
    iMmsDataPlugin->setMessageId(entry.EntryId());
    if (iMmsDataPlugin->attachmentCount() > 0) {
        item.setData(ConvergedMessage::Attachment, MessageProperty);
    }

    int slideCount = iMmsDataPlugin->slideCount();
    bool isBodyTextSet = false;
    QString textContent;
    QStringList attachmentPaths;

    for (int i = 0; i < slideCount; ++i) {
        UniMessageInfoList objectList = iMmsDataPlugin->slideContent(i);
        for (int index = 0; index < objectList.count(); ++index) {
            attachmentPaths.append(objectList[index]->path());
            if (!isBodyTextSet && objectList[index]->mimetype().contains("text")) {
                QFile file(objectList[index]->path());
                if(file.open(QIODevice::ReadOnly))
                {
                    textContent = file.readAll();
                    item.setData(textContent, BodyText);
                    isBodyTextSet = true;
                    file.close();
                }
            }
        }
        foreach(UniMessageInfo* slide,objectList)
            {
                delete slide;
            }
    }
    //populate item  with the attachment list
    item.setData(attachmentPaths.join("|"), Attachments);
    if (entry.IsAttributeSet(ECsAttributeHighPriority)) {
        item.setData(ConvergedMessage::High, MessagePriority);
    }
    else if (entry.IsAttributeSet(ECsAttributeLowPriority)) {
        item.setData(ConvergedMessage::Low, MessagePriority);
    }
    //subject
    item.setData(iMmsDataPlugin->subject(), Subject);
}

//---------------------------------------------------------------
// ConversationsModel::handleMMSNotification
// @see header
//---------------------------------------------------------------
void ConversationsModel::handleMMSNotification(QStandardItem& item,
    const CCsConversationEntry& entry)
{
    // set context to current entry
    mMsgStoreHandler->setNotificationMessageId(entry.EntryId());
    
    // fetch relevent info to show in CV
    // msg size
    QString estimatedMsgSizeStr = 
            mMsgStoreHandler->NotificationMsgSize();
    
    // msg class type
    QString classInfoStr = mMsgStoreHandler->NotificationClass();
    
    // notification expiry date
    //TODO: Need to do localization of digits used to show expiry time
    TTime expiryTime;
    QString expiryTimeStr;
    mMsgStoreHandler->NotificationExpiryDate(expiryTime, expiryTimeStr);
    
    // notification state e.g. waiting, retrieving etc
    QString statusStr;
    int status;
    mMsgStoreHandler->NotificationStatus(status, statusStr);
    
    // create data for bodytext role
    QString dataText;
    dataText.append("Size: "); // TODO: use logical str name
    dataText.append(estimatedMsgSizeStr);
    dataText.append(QChar::LineSeparator);
    dataText.append("Class: "); // TODO: use logical str name
    dataText.append(classInfoStr);
    dataText.append(QChar::LineSeparator);
    dataText.append("Expiry date: "); //TODO: use logical str name
    dataText.append(expiryTimeStr);
    if(!statusStr.isEmpty())
    {
        dataText.append(QChar::LineSeparator);
        dataText.append(statusStr);
        dataText.append(QChar::LineSeparator); //Temp fix to be removed
    }

    // set fetched data to roles
    item.setData(status, NotificationStatus);
    item.setData(dataText, BodyText);

    if (entry.IsAttributeSet(ECsAttributeHighPriority)) {
        item.setData(ConvergedMessage::High, MessagePriority);
    }
    else if (entry.IsAttributeSet(ECsAttributeLowPriority)) {
        item.setData(ConvergedMessage::Low, MessagePriority);
    }
}

//---------------------------------------------------------------
// ConversationsModel::handleBlueToothMessages
// @see header
//---------------------------------------------------------------
void ConversationsModel::handleBlueToothMessages(QStandardItem& item,
    const CCsConversationEntry& entry)
{
    //TODO, needs to be revisited again, once BT team provides the solution for
    //BT received as Biomsg issue.
    QString description = S60QConversions::s60DescToQString(*(entry.Description()));

    if (description.contains(".vcf") || description.contains(".ics")) // "vCard"
    {
        //message sub-type
        item.setData(ConvergedMessage::VCard, MessageSubType);

        //parse vcf file to get the details
        QString displayName = ConversationsEngineUtility::getVcardDisplayName(description);
        item.setData(displayName, BodyText);
    }    
    else 
    {
        if (description.contains(".vcs")) // "vCalendar"
        {
            //message sub-type
            item.setData(ConvergedMessage::VCal, MessageSubType);
        }
        else
        {
            //message sub-type
            item.setData(ConvergedMessage::None, MessageSubType);
        }
        //for BT messages we show filenames for all other (except vcard) messages
        //get filename and set as body
        QFileInfo fileinfo(description);
        QString filename = fileinfo.fileName();
        item.setData(filename, BodyText);
    }
}

//---------------------------------------------------------------
// ConversationsModel::handleBioMessages
// @see header
//---------------------------------------------------------------
void ConversationsModel::handleBioMessages(QStandardItem& item, const CCsConversationEntry& entry)
{
    iBioMsgPlugin->setMessageId(entry.EntryId());
    int msgSubType = ConversationsEngineUtility::messageSubType(entry.GetType());
    if (ConvergedMessage::VCard == msgSubType) {
        if (iBioMsgPlugin->attachmentCount() > 0) {
            UniMessageInfoList attList = iBioMsgPlugin->attachmentList();
            QString attachmentPath = attList[0]->path();

            //get display-name and set as bodytext
            QString displayName = ConversationsEngineUtility::getVcardDisplayName(attachmentPath);
            item.setData(displayName, BodyText);

            // clear attachement list : its allocated at data model
            while (!attList.isEmpty()) {
                delete attList.takeFirst();
            }
        }
    }
    else if (ConvergedMessage::VCal == msgSubType) {
        //not supported
    }
    else {
        // description
        HBufC* description = entry.Description();
        QString subject("");
        if (description && description->Length()) {
            subject = (S60QConversions::s60DescToQString(*description));
            item.setData(subject, BodyText);
        }
    }
}

//EOF
