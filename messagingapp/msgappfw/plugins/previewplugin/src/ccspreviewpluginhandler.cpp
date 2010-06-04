/*
 * Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  CS Preview Plugin Handler, This class creates and updates sqlite based db
 *                with the message-preview data.
 *
 */
// USER INCLUDES
#include "ccspreviewpluginhandler.h"
#include "UniObject.h"
// SYSTEM INCLUDES
#include <mmsclient.h>
#include <mtclreg.h>
#include <msvids.h>
#include <e32const.h>
#include <SendUiConsts.h>
#include <utf.h>
//CONSTANTS
//DB-file
_LIT(KDbFileName, "c:[2002A542]conversations.db");
//Encoding
_LIT(KEncodingStmnt,"PRAGMA encoding=\"UTF-8\"");
//Size
_LIT(KCacheSizeStmnt,"PRAGMA default_cache_size = 1024");
// Create table query statement
_LIT(KSqlCreateStmt, "CREATE TABLE IF NOT EXISTS conversation_messages ( message_id  INTEGER PRIMARY KEY, msg_parsed  INTEGER DEFAULT 0, subject TEXT(100), body_text TEXT(160), preview_path TEXT, msg_property INTEGER, preview_icon BLOB DEFAULT NULL ) " );
//Insert without bitmap query
_LIT(KSqlInsertStmt, "INSERT OR REPLACE INTO conversation_messages (message_id, msg_parsed, subject, body_text, preview_path, msg_property ) VALUES( :message_id, :msg_parsed, :subject, :body_text, :preview_path,  :msg_property )");
//update with bitmap query
_LIT(KSqlUpdateBitmapStmt, "UPDATE conversation_messages SET preview_icon=:preview_icon WHERE message_id=:message_id " );
// query to see if msg_parsed is set
_LIT(KSelectMsgParsedStmt, " SELECT message_id, msg_parsed  FROM conversation_messages WHERE message_id=:message_id ");
// Remove record from conversation_messages table.
_LIT(KRemoveMsgStmnt,"DELETE FROM conversation_messages WHERE message_id=:message_id");

// NOTE:- DRAFTS ENTRIES ARE NOT HANDLED IN THE PLUGIN

// ============================== MEMBER FUNCTIONS ============================
// ----------------------------------------------------------------------------
// CCsPreviewPluginHandler::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
//
CCsPreviewPluginHandler* CCsPreviewPluginHandler::NewL(
    CCsPreviewPlugin *aMsgObserver)
{
    PRINT ( _L("Enter CCsMsgHandler::NewL") );

    CCsPreviewPluginHandler* self = new (ELeave) CCsPreviewPluginHandler();
    CleanupStack::PushL(self);
    self->ConstructL(aMsgObserver);
    CleanupStack::Pop(self);

    PRINT ( _L("End CCsPreviewPluginHandler::NewL") );

    return self;
}

// ----------------------------------------------------------------------------
// CCsPreviewPluginHandler::~CCsPreviewPluginHandler
// Destructor
// ----------------------------------------------------------------------------
//
CCsPreviewPluginHandler::~CCsPreviewPluginHandler()
{
    PRINT ( _L("Enter CCsPreviewPluginHandler::~CCsPreviewPluginHandler") );

    iSqlDb.Close();
    iThumbnailRequestArray.Close();
    ifsSession.Close();

    if (iMmsMtm)
    {
        delete iMmsMtm;
        iMmsMtm = NULL;
    }

    if (iMtmRegistry)
    {
        delete iMtmRegistry;
        iMtmRegistry = NULL;
    }

    if (iSession)
    {
        delete iSession;
        iSession = NULL;
    }

    if (iThumbnailManager)
    {
        delete iThumbnailManager;
        iThumbnailManager = NULL;
    }

    PRINT ( _L("End CCsPreviewPluginHandler::~CCsPreviewPluginHandler") );
}

// ----------------------------------------------------------------------------
// CCsMsgHandler::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::ConstructL(CCsPreviewPlugin *aMsgObserver)
{
    PRINT ( _L("Enter CCsPreviewPluginHandler::ConstructL") );

    iMsgObserver = aMsgObserver;

    //file session connect
    User::LeaveIfError(ifsSession.Connect());

    //create msv session
    iSession = CMsvSession::OpenSyncL(*this);

    //create mtm registry
    iMtmRegistry = CClientMtmRegistry::NewL(*iSession);

    //create mms client mtm
    iMmsMtm = static_cast<CMmsClientMtm*> (iMtmRegistry-> NewMtmL(
        KSenduiMtmMmsUid));

    //create thumbnail manager
    iThumbnailManager = CThumbnailManager::NewL(*this);

    // open DB
    TInt error = iSqlDb.Open(KDbFileName);

    PRINT1 ( _L("End CCsPreviewPluginHandler::ConstructL open DB file error=%d"), error );

    // if not found, create DB
    if (error == KErrNotFound)
    {
        //create sqlite-DB
        TSecurityPolicy defaultPolicy(TSecurityPolicy::EAlwaysPass);
        RSqlSecurityPolicy securityPolicy;
        securityPolicy.Create(defaultPolicy);

        // TODO, setting UID security policy
        //TSecurityPolicy readPolicy(ECapabilityReadUserData);  
        //securityPolicy.SetDbPolicy(RSqlSecurityPolicy::EReadPolicy, readPolicy);

        iSqlDb.Create(KDbFileName, securityPolicy);

        //Create the table inside DB
        iSqlDb.Exec(KSqlCreateStmt);
        iSqlDb.Exec(KEncodingStmnt);
        iSqlDb.Exec(KCacheSizeStmnt);
    }
    else
    {
        User::LeaveIfError(error);
    }

    PRINT ( _L("End CCsPreviewPluginHandler::ConstructL") );
}

// ----------------------------------------------------------------------------
// CCsPreviewPluginHandler::CCsPreviewPluginHandler
// Two Phase Construction
// ----------------------------------------------------------------------------
//
CCsPreviewPluginHandler::CCsPreviewPluginHandler()
{
}

// ----------------------------------------------------------------------------
// CCsPreviewPluginHandler::HandleSessionEventL
// Implemented for MMsvSessionObserver
// ----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::HandleSessionEventL(TMsvSessionEvent aEvent,
    TAny* aArg1, TAny* aArg2, TAny* /*aArg3*/)
{
    PRINT1 ( _L("Enter CCsPreviewPluginHandler::HandleSessionEventL aEvent=%d"),aEvent );

    CMsvEntrySelection* selection = NULL;
    TMsvId parent;

    //args
    if (aArg1 == NULL || aArg2 == NULL)
    {
        PRINT ( _L("Enter CCsPreviewPluginHandler::HandleSessionEventL arguments invalid"));
        return;
    }

    //start, processing the event
    selection = (CMsvEntrySelection*) aArg1;
    parent = *(TMsvId*) aArg2;

    //Drafts not handled
    if (KMsvDraftEntryIdValue == parent)
    {
        return;
    }

    switch (aEvent)
    {
        case EMsvEntriesChanged:
        case EMsvEntriesMoved:
        {
            HandleEventL(selection);
        }
        break;

        case EMsvEntriesDeleted:
        {
            for (TInt i = 0; i < selection->Count(); i++)
            {
                RSqlStatement sqlDeleteStmt;
                CleanupClosePushL(sqlDeleteStmt);
                sqlDeleteStmt.PrepareL(iSqlDb, KRemoveMsgStmnt);

                TInt messageIdIndex = sqlDeleteStmt.ParameterIndex(_L(
                    ":message_id"));
                User::LeaveIfError(sqlDeleteStmt.BindInt(messageIdIndex, selection->At(i)));

                User::LeaveIfError(sqlDeleteStmt.Exec());
                CleanupStack::PopAndDestroy(&sqlDeleteStmt);
            }
        }
        break;
    }

    PRINT ( _L("Exit CCsPreviewPluginHandler::HandleSessionEventL") );
}

// ---------------------------------------------------------------------
// CCsPreviewPluginHandler::HandleEvent
// Handle events
// ---------------------------------------------------------------------
//
void CCsPreviewPluginHandler::HandleEventL(CMsvEntrySelection* aSelection)
{
    PRINT ( _L("Enter CCsPreviewPluginHandler::HandleEvent") );

    TMsvEntry entry;
    TMsvId service;
    TInt error = KErrNone;

    for (TInt i = 0; i < aSelection->Count(); i++)
    {
        error = iSession->GetEntry(aSelection->At(i), service, entry);

        if ( (KErrNone == error) && !entry.InPreparation() && entry.Visible()
                && (KSenduiMtmMmsUidValue == entry.iMtm.iUid))
        {
            PRINT ( _L("Enter CCsPreviewPluginHandler::HandleEvent for loop started.") );

            TInt msgId = entry.Id();

            //check if the message is already parsed
            RSqlStatement sqlSelectStmt;
            CleanupClosePushL(sqlSelectStmt);
            sqlSelectStmt.PrepareL(iSqlDb,KSelectMsgParsedStmt);
            TInt messageIdIndex = sqlSelectStmt.ParameterIndex(
                _L(":message_id"));

            User::LeaveIfError(sqlSelectStmt.BindInt(messageIdIndex, msgId));

            if (sqlSelectStmt.Next() == KSqlAtRow)
            {
                TInt parsedColIndex = sqlSelectStmt.ColumnIndex(
                    _L("msg_parsed"));
                TInt msgParsed = sqlSelectStmt.ColumnInt(parsedColIndex);
                //if message alresdy parsed, move to next message.
                if (msgParsed)
                {
                    CleanupStack::PopAndDestroy(&sqlSelectStmt);
                    continue;
                }
            }
            CleanupStack::PopAndDestroy(&sqlSelectStmt);

            // update db with message preview data
            RSqlStatement sqlInsertStmt;
            CleanupClosePushL(sqlInsertStmt);
            sqlInsertStmt.PrepareL(iSqlDb, KSqlInsertStmt);
            
            // parse message
            iMmsMtm->SwitchCurrentEntryL(msgId);
            iMmsMtm->LoadMessageL();

            CUniDataModel* iUniDataModel = CUniDataModel::NewL(ifsSession,
                *iMmsMtm);
            CleanupStack::PushL(iUniDataModel);
            iUniDataModel->RestoreL(*this, ETrue);

            //msg property
            TInt msgProperty = 0;
            if (iUniDataModel->AttachmentList().Count() > 0)
            {
                msgProperty |= EPreviewAttachment;
            }

            TPtrC videoPath;
            TPtrC imagePath;
           
            // preview parsing
            TInt slideCount = iUniDataModel->SmilModel().SlideCount();
            TBool isBodyTextSet = EFalse;
            TBool isImageSet = EFalse;
            TBool isAudioSet = EFalse;
            TBool isVideoSet = EFalse;

            for (int i = 0; i < slideCount; i++)
            {
                int slideobjcount =
                        iUniDataModel->SmilModel().SlideObjectCount(i);
                for (int j = 0; j < slideobjcount; j++)
                {
                    CUniObject *obj =
                            iUniDataModel->SmilModel(). GetObjectByIndex(i, j);

                    TPtrC8 mimetype = obj->MimeType();
                    TMsvAttachmentId attachId = obj->AttachmentId();

                    //bodytext
                    if (!isBodyTextSet && (mimetype.Find(_L8("text"))
                            != KErrNotFound))
                    {
                        //bind bodytext into statement
                        BindBodyText(sqlInsertStmt, attachId);
                        isBodyTextSet = ETrue;
                    }

                    //image parsing
                    if (!isImageSet && (mimetype.Find(_L8("image"))
                            != KErrNotFound))
                    {
                        //get thumbnail for this image
                        GetThumbNailL(attachId, mimetype, msgId);
                        isImageSet = ETrue;
                        imagePath.Set(obj->MediaInfo()->FullFilePath());
                        msgProperty |= EPreviewImage;
                    }

                    //audio content
                    if (!isAudioSet && (mimetype.Find(_L8("audio"))
                            != KErrNotFound))
                    {
                        isAudioSet = ETrue;
                        msgProperty |= EPreviewAudio;
                    }

                    //video content
                    if (!isVideoSet && (mimetype.Find(_L8("video"))
                            != KErrNotFound))
                    {
                        videoPath.Set(obj->MediaInfo()->FullFilePath());
                        isVideoSet = ETrue;
                        msgProperty |= EPreviewVideo;
                    }
                }
            }

            //set preview path
            TInt previewPathIndex = sqlInsertStmt.ParameterIndex(_L(
                ":preview_path"));
            if (isVideoSet)
            {
                User::LeaveIfError(sqlInsertStmt.BindText(previewPathIndex,
                    videoPath));
            }
            else if (isImageSet)
            {
                User::LeaveIfError(sqlInsertStmt.BindText(previewPathIndex,
                    imagePath));
            }

            //msg_id
            TInt msgIdIndex = sqlInsertStmt.ParameterIndex(_L(":message_id"));
            User::LeaveIfError(sqlInsertStmt.BindInt(msgIdIndex, msgId));

            //subjext
            TInt subjectIndex = sqlInsertStmt.ParameterIndex(_L(":subject"));
            User::LeaveIfError(sqlInsertStmt.BindText(subjectIndex,
                iMmsMtm->SubjectL()));

            //msg_property
            TInt msgPropertyIndex = sqlInsertStmt.ParameterIndex(_L(
                ":msg_property"));
            User::LeaveIfError(sqlInsertStmt.BindInt(msgPropertyIndex,
                msgProperty));

            //msg-parsed
            TInt msgParsedIndex = sqlInsertStmt.ParameterIndex(
                _L(":msg_parsed"));
            User::LeaveIfError(sqlInsertStmt.BindInt(msgParsedIndex, 1)); // 1 as true

            //execute sql stament
            User::LeaveIfError(sqlInsertStmt.Exec());

            //cleanup
            CleanupStack::PopAndDestroy(2, &sqlInsertStmt);
        }
}//end for loop

PRINT ( _L("Exit CCsPreviewPluginHandler::HandleEvent") );
}

// -----------------------------------------------------------------------------
// CCsPreviewPluginHandler::RestoreReady()
// 
// -----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::RestoreReady(TInt /*aParseResult*/, TInt /*aError*/)
{

}

// -----------------------------------------------------------------------------
// CCsPreviewPluginHandler::ThumbnailReady()
// 
// -----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::ThumbnailReady(TInt aError,
    MThumbnailData& aThumbnail, TThumbnailRequestId aId)
{
    // This function must not leave.
    if (!aError)
    {
        PRINT ( _L("CCsPreviewPluginHandler::ThumbnailReady received.") );
        TInt err;
        TRAP(err, HandleThumbnailReadyL(aThumbnail, aId));
        PRINT1 ( _L("CCsPreviewPluginHandler::ThumbnailReady handling error= %d."), err );
    }
    else
    {
        // An error occurred while getting the thumbnail.
        PRINT1 ( _L("End CCsPreviewPluginHandler::ThumbnailReady error= %d."), aError );
    }
}

// -----------------------------------------------------------------------------
// CCsPreviewPluginHandler::ThumbnailPreviewReady()
// callback
// -----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::ThumbnailPreviewReady(
    MThumbnailData& /*aThumbnail*/, TThumbnailRequestId /*aId*/)
{

}

// -----------------------------------------------------------------------------
// CCsPreviewPluginHandler::HandleThumbnailReadyL()
// 
// -----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::HandleThumbnailReadyL(MThumbnailData& aThumbnail,
    TThumbnailRequestId aId)
{
    //match response to request
    ThumbnailRequestData tempObj;
    tempObj.iRequestId = aId;

    TInt index = iThumbnailRequestArray.Find(tempObj,
        CCsPreviewPluginHandler::CompareByRequestId);
    if (index < 0)
    {
        PRINT ( _L("End CCsPreviewPluginHandler::HandleThumbnailReady request match not found.") );
        return;
    }

    // get msg-id corresponding to the request-id
    TInt msgId = iThumbnailRequestArray[index].iMsgId;
    //remove the request from requestarray
    iThumbnailRequestArray.Remove(index);

    // get bitmap
    CFbsBitmap* bitmap = aThumbnail.Bitmap();

    // sql-statment to set preview-icon
    RSqlStatement sqlInsertStmt;
    CleanupClosePushL(sqlInsertStmt);
    sqlInsertStmt.PrepareL(iSqlDb, KSqlUpdateBitmapStmt);

    TInt msgIdIndex = sqlInsertStmt.ParameterIndex(_L(":message_id"));
    TInt previewIconIndex = sqlInsertStmt.ParameterIndex(_L(":preview_icon"));

    User::LeaveIfError(sqlInsertStmt.BindInt(msgIdIndex, msgId));

    RSqlParamWriteStream previewIconStream;
    CleanupClosePushL(previewIconStream);

    //bind data
    User::LeaveIfError(previewIconStream.BindBinary(sqlInsertStmt, previewIconIndex));
    bitmap->ExternalizeL(previewIconStream);
    previewIconStream.CommitL();

    //execute the statent
    User::LeaveIfError(sqlInsertStmt.Exec());

    CleanupStack::PopAndDestroy(2,&sqlInsertStmt);//sqlInsertStmt,previewIconStream
}

//-----------------------------------------------------------------------------
// CCsPreviewPluginHandler::CompareByRequestId
// Compare to conversation entry object based on Entry Ids
//----------------------------------------------------------------------------
TBool CCsPreviewPluginHandler::CompareByRequestId(
    const ThumbnailRequestData& aFirst, const ThumbnailRequestData& aSecond)
{
    if (aFirst.iRequestId == aSecond.iRequestId)
        return ETrue;

    return EFalse;
}

// -----------------------------------------------------------------------------
// CCsPreviewPluginHandler::BindBodyText()
// 
// -----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::BindBodyText(RSqlStatement& sqlStmt,
    TMsvAttachmentId attachmentId)
{
    //get file handle from attachmnet manager.
    CMsvStore * store = iMmsMtm->Entry().ReadStoreL();
    CleanupStack::PushL(store);
    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    RFile file = attachMan.GetAttachmentFileL(attachmentId);
    CleanupClosePushL(file);

    //read file contents to buffer
    TInt length;
    file.Size(length);
    HBufC8* bodyText = HBufC8::NewLC(length);
    TPtr8 textBuffer = bodyText->Des();
    file.Read(textBuffer);

    // convert from HBufC8 to HBufC16
    HBufC16 *text16 = HBufC16::NewLC(textBuffer.Length());
    TPtr16 textPtr16 = text16->Des();
    CnvUtfConverter::ConvertToUnicodeFromUtf8(textPtr16, textBuffer);

    //set bodytext in the sql statement
    TInt bodyTextIndex = sqlStmt.ParameterIndex(_L(":body_text"));
    sqlStmt.BindText(bodyTextIndex, textPtr16);

    CleanupStack::PopAndDestroy(4, store); //store,file, bodyText, text16
}

// -----------------------------------------------------------------------------
// CCsPreviewPluginHandler::GetThumbNailL()
// 
// -----------------------------------------------------------------------------
//
void CCsPreviewPluginHandler::GetThumbNailL(TMsvAttachmentId attachmentId,
    TDesC8& mimeType, TMsvId msgId)
{
    //Scale the image
    iThumbnailManager->SetFlagsL(CThumbnailManager::ECropToAspectRatio);
    // Preferred size is 100x100 (or less)
    iThumbnailManager->SetThumbnailSizeL(TSize(100, 100));
    //optimize for performace
    iThumbnailManager->SetQualityPreferenceL(
        CThumbnailManager::EOptimizeForPerformance);

    // Create Thumbnail object source representing a path to a file
    HBufC* mimeInfo = HBufC::NewLC(mimeType.Length());
    mimeInfo->Des().Copy(mimeType);

    CMsvStore * store = iMmsMtm->Entry().ReadStoreL();
    CleanupStack::PushL(store);

    //get file handle from attachment manager.
    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    RFile file = attachMan.GetAttachmentFileL(attachmentId);
    CleanupClosePushL(file);

    CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(
        (RFile64&) file, mimeInfo->Des());

    // Issue the request for thumbnail creation
    ThumbnailRequestData reqObject;
    reqObject.iMsgId = msgId;
    reqObject.iRequestId = iThumbnailManager->GetThumbnailL(*source);
    iThumbnailRequestArray.Append(reqObject);

    CleanupStack::PopAndDestroy(4, mimeInfo);//mimeInfo,store,file,source
}

// End of file

