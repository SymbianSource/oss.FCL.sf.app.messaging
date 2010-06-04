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
 * Description:  Message Preview Handler
 *
 */

#ifndef _C_CS_PREVIEW_PLUGIN_HANDLER_H_
#define _C_CS_PREVIEW_PLUGIN_HANDLER_H_

//SYSTEM INCLUDES
#include <msvapi.h>
#include <f32file.h>
#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>
#include <thumbnaildata.h>
#include <sqldb.h>

//USER INCLUDES
#include "ccspreviewplugin.h"
#include "UniDataModel.h"

// FORWARD DECLARATION
class CCsPlugin;
class CMsvSession;
class MCsPluginEventObserver;
class MMsvSessionObserver;
class CCsConversationEntry;
class CCsPreviewPlugin;
class CClientMtmRegistry;
class CMmsClientMtm;

/*
 * Thumbnail Request Data
 */
struct ThumbnailRequestData
    {
    /*
     * Request Id
     */
    TThumbnailRequestId iRequestId;

    /*
     * Message Id
     */
    TMsvId iMsgId;
    };

/**
 *  This class interacts with MsvServer to fetch message data
 */
class CCsPreviewPluginHandler : public CBase,
        public MMsvSessionObserver,
        public MUniDataModelObserver,
        public MThumbnailManagerObserver
    {

public:

    /**
     * Two phase construction
     */
    static CCsPreviewPluginHandler* NewL(CCsPreviewPlugin *aMsgObserver);

    /**
     * Destructor
     */
    virtual ~CCsPreviewPluginHandler();

public:
    //MMsvSessionObserver
    /**
     * Handles the MsvServer updates. Implemented for MMsvSessionObserver
     */
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1,
            TAny* aArg2, TAny* aArg3);

public:
    //MUniDataModelObserver
    /**
     */
    void RestoreReady(TInt aParseResult, TInt aError);

public:
    // Callbacks from MThumbnailManagerObserver for getting thumbnails
    void ThumbnailPreviewReady(MThumbnailData& aThumbnail,
            TThumbnailRequestId aId);

    void ThumbnailReady(TInt aError, MThumbnailData& aThumbnail,
            TThumbnailRequestId aId);

public:

    /*
     * Compare by request Id.
     */
    static TBool CompareByRequestId(const ThumbnailRequestData& aFirst,
            const ThumbnailRequestData& aSecond);

private:

    /**
     * Constructor   
     */
    CCsPreviewPluginHandler();

    /**
     * 2nd phase construtor
     */
    void ConstructL(CCsPreviewPlugin *aMsgPlugin);

    /**
     * Process each entry
     *@param aSelection, CMsvEntrySelections
     */
    void HandleEventL(CMsvEntrySelection* aSelection);

    /**
     * Bind bodytext to sql statement.
     *@param sqlStmt, RSqlStatement
     *@param attachmentId, TMsvAttachmentId
     */
    void BindBodyText(RSqlStatement& sqlStmt, TMsvAttachmentId attachmentId);

    /**
     * Get Thumbnail data from attachment
     *@param attachmentId, TMsvAttachmentId
     *@param mimeType, 
     *@param msgId, message id
     */
    void GetThumbNailL(TMsvAttachmentId attachmentId, TDesC8& mimeType,
            TMsvId msgId);

    /*
     * Process Thumbnail data
     * @param aThumbnail MThumbnailData
     * @param aId TThumbnailRequestId
     */
    void HandleThumbnailReadyL(MThumbnailData& aThumbnail,
            TThumbnailRequestId aId);

private:
    //Data

    /**
     * MsvServer session object
     * Own
     */
    CMsvSession* iSession;

    /**
     * iMsgObserver
     * The Observer interface
     * Not Own.
     */
    CCsPreviewPlugin *iMsgObserver;

    /**
     * Mtm registry object
     * Own
     */
    CClientMtmRegistry* iMtmRegistry;

    /**
     * Mms client mtm object
     * Own.
     */
    CMmsClientMtm* iMmsMtm;

    /*
     * File session
     */
    RFs ifsSession;

    /*
     * Thumbnail request array.
     * Own
     */
    RArray<ThumbnailRequestData> iThumbnailRequestArray;

    /*
     * Thumbnail manager.
     * Own
     */
    CThumbnailManager* iThumbnailManager;

    /*
     * Sqlite DB Handle
     */
    RSqlDatabase iSqlDb;
    };

#endif // _C_CS_PREVIEW_PLUGIN_HANDLER_H_
