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
#include "ringbc.h"
#include "msgcontacthandler.h"
#include <ccsconversationentry.h>

#include "debugtraces.h"

#include <QFile>
#include <QFileInfo>
#include <s32mem.h>
#include <ccsdefs.h>

//CONSTANTS
_LIT(KDbFileName, "c:[2002A542]conversations.db");
// preview sql query
_LIT(KSelectConvMsgsStmt, "SELECT message_id, subject, body_text, preview_path, msg_property FROM conversation_messages WHERE message_id=:message_id ");

//---------------------------------------------------------------
// ConversationsModel::ConversationsModel
// Constructor
//---------------------------------------------------------------
ConversationsModel::ConversationsModel(ConversationMsgStoreHandler* msgStoreHandler,
    QObject* parent) :
    QStandardItemModel(parent), mMsgStoreHandler(msgStoreHandler), iSqlDbOpen(EFalse)
{
    //Open SQL DB
    if (KErrNone == iSqlDb.Open(KDbFileName))
    {
        iSqlDbOpen = ETrue;
    }
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
    //Close SQL-DB
    iSqlDb.Close();

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
    QCRITICAL_WRITE("ConversationsModel::populateItem start.");

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
        QCRITICAL_WRITE("ConversationsModel::populateItem  MMS start.")
        handleMMS(item, entry);
        QCRITICAL_WRITE("ConversationsModel::populateItem MMS end.")
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

    QCRITICAL_WRITE("ConversationsModel::populateItem end.");
}

//---------------------------------------------------------------
// ConversationsModel::handleMMS
// @see header
//---------------------------------------------------------------
void ConversationsModel::handleMMS(QStandardItem& item, const CCsConversationEntry& entry)
{
    //msg_id
    int msgId = entry.EntryId();

    bool isEntryInDb = false;
    TInt err = KErrNone;
    
    //check if db is open and query db
    if (iSqlDbOpen)
    {
        RSqlStatement sqlSelectStmt;
        err = sqlSelectStmt.Prepare(iSqlDb, KSelectConvMsgsStmt);

        // move to fallback option
        if (KErrNone == err)
            {
            TInt msgIdIndex = sqlSelectStmt.ParameterIndex(_L(":message_id"));
            TInt subjectIndex = sqlSelectStmt.ColumnIndex(_L("subject"));
            TInt bodyIndex = sqlSelectStmt.ColumnIndex(_L("body_text"));
            TInt previewPathIndex = sqlSelectStmt.ColumnIndex(
                    _L("preview_path"));
            TInt msgpropertyIndex = sqlSelectStmt.ColumnIndex(
                    _L("msg_property"));

            err = sqlSelectStmt.BindInt(msgIdIndex, msgId);
            
            // populate item
            if ((KErrNone == err) && (sqlSelectStmt.Next() == KSqlAtRow))
                {
                RBuf subjectBuffer;
                subjectBuffer.Create(sqlSelectStmt.ColumnSize(subjectIndex));
                sqlSelectStmt.ColumnText(subjectIndex, subjectBuffer);

                item.setData(
                        S60QConversions::s60DescToQString(subjectBuffer),
                        Subject);
                subjectBuffer.Close();

                RBuf bodyBuffer;
                bodyBuffer.Create(sqlSelectStmt.ColumnSize(bodyIndex));
                sqlSelectStmt.ColumnText(bodyIndex, bodyBuffer);

                item.setData(S60QConversions::s60DescToQString(bodyBuffer),
                        BodyText);
                bodyBuffer.Close();

                RBuf previewPathBuffer;
                previewPathBuffer.Create(sqlSelectStmt.ColumnSize(
                        previewPathIndex));
                sqlSelectStmt.ColumnText(previewPathIndex, previewPathBuffer);

                //Rightnow set inside attachments
                item.setData(S60QConversions::s60DescToQString(
                        previewPathBuffer), Attachments);
                previewPathBuffer.Close();

                int msgProperty = 0;
                msgProperty = sqlSelectStmt.ColumnInt(msgpropertyIndex);
                item.setData(msgProperty, MessageProperty);

                //set flag to disable fallback option
                isEntryInDb = true;
                }
            }
        sqlSelectStmt.Close();
        }

    //fallback option incase of db operation failure or enry not found in DB
    //populate from data plugins
    if (!isEntryInDb || err != KErrNone)
    {
        iMmsDataPlugin->setMessageId(entry.EntryId());
        int msgProperty = 0;

        if (iMmsDataPlugin->attachmentCount() > 0)
        {
            msgProperty |= EPreviewAttachment;
        }

        //subject
        item.setData(iMmsDataPlugin->subject(), Subject);

        int slideCount = iMmsDataPlugin->slideCount();
        bool isBodyTextSet = false;
        bool isAudioSet = false;
        bool isImageSet = false;
        bool isVideoSet = false;
        QString textContent;
        QString videoPath;
        QString imagePath;

        for (int i = 0; i < slideCount; ++i)
        {
            UniMessageInfoList objectList = iMmsDataPlugin->slideContent(i);
            for (int index = 0; index < objectList.count(); ++index)
            {
                if (!isBodyTextSet && objectList[index]->mimetype().contains(
                    "text"))
                {
                    QFile file(objectList[index]->path());
                    file.open(QIODevice::ReadOnly);
                    textContent = file.readAll();
                    item.setData(textContent, BodyText);
                    isBodyTextSet = true;
                    file.close();
                }
                if (!isImageSet && objectList[index]->mimetype().contains(
                    "image"))
                {
                    isImageSet = true;
                    msgProperty |= EPreviewImage;
                    imagePath = objectList[index]->path();
                }
                if (!isAudioSet && objectList[index]->mimetype().contains(
                    "audio"))
                {
                    msgProperty |= EPreviewAudio;
                    isAudioSet = true;
                }
                if (!isVideoSet && objectList[index]->mimetype().contains(
                    "video"))
                {
                    isVideoSet = true;
                    msgProperty |= EPreviewVideo;
                    videoPath = objectList[index]->path();
                }
            }
            foreach(UniMessageInfo* slide,objectList)
                {
                    delete slide;
                }
        }
        //populate item  with the attachment list
        if (isVideoSet)
        {
            item.setData(videoPath, Attachments);
        }
        else if (isImageSet)
        {
            item.setData(imagePath, Attachments);
        }
        //populate msgProperty
        item.setData(msgProperty, MessageProperty);
    }

    // fill other attributes
    if (entry.IsAttributeSet(ECsAttributeHighPriority))
    {
        item.setData(ConvergedMessage::High, MessagePriority);
    }
    else if (entry.IsAttributeSet(ECsAttributeLowPriority))
    {
        item.setData(ConvergedMessage::Low, MessagePriority);
    }
}

//---------------------------------------------------------------
// ConversationsModel::handleMMSNotification
// @see header
//---------------------------------------------------------------
void ConversationsModel::handleMMSNotification(QStandardItem& item,
    const CCsConversationEntry& entry)
{
    // set context to current entry
    TRAPD(err, mMsgStoreHandler->setNotificationMessageIdL(entry.EntryId()));
    if(err != KErrNone)
    {
        return;
    }
    
    // fetch relevent info to show in CV
    // msg size
    QString estimatedMsgSizeStr = QString("%1").arg(0);
    estimatedMsgSizeStr.append(" Kb");
    TRAP_IGNORE(estimatedMsgSizeStr = 
            mMsgStoreHandler->NotificationMsgSizeL());
    
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
        QString displayName = MsgContactHandler::getVCardDisplayName(
                description);
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
            QString displayName = 
                    MsgContactHandler::getVCardDisplayName(
                            attachmentPath);
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
    else if (ConvergedMessage::RingingTone == msgSubType) {
        if (iBioMsgPlugin->attachmentCount() > 0) {
            UniMessageInfoList attList = iBioMsgPlugin->attachmentList();
            QString attachmentPath = attList[0]->path();

            //get tone title, and set as bodytext
            RingBc ringBc;
            item.setData(ringBc.toneTitle(attachmentPath), BodyText);
            while (!attList.isEmpty()) {
                delete attList.takeFirst();
            }
        }

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

//---------------------------------------------------------------
// ConversationsModel::getDBHandle()
// @see header
//---------------------------------------------------------------
RSqlDatabase& ConversationsModel::getDBHandle(TBool& isOpen)
{
    isOpen = iSqlDbOpen;
    return iSqlDb;
}
//EOF
