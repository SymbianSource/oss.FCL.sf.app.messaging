/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wrapper classes which encapsulates Message store functions
 *
*/

#ifndef CONVERSATION_MSG_STORE_HANDLER_H
#define CONVERSATION_MSG_STORE_HANDLER_H

// INCLUDE FILES
#include <msvapi.h>
#include <ccsdefs.h>

// FORWARD DECLARATIONS
class CMsvSession;
class MMsvSessionObserver;
class DraftsModel;

// CONSTANTS

class ConversationMsgStoreHandler: public MMsvSessionObserver
    {
public:
    
    /*
     * Draft processing states
     */
    enum TDraftHandlerState
        {
        EReadDrafts,
        EProcessDrafts,
        EComplete
        };

    /**
     * Default constructor.
     */
    explicit ConversationMsgStoreHandler();

    /**
     * Destructor
     */
    virtual ~ConversationMsgStoreHandler();

private:
    
    /**
     * Initializes the ConversationMsgStoreHandler class.     
     */
    void InitL();

public:
    /**
     * From MMsvSessionObserver. HandleSessionEventL.
     */
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
            TAny* aArg3);
public: 
    /**
     * Get MMS notification status.
     * @param aMsvId MMS notification status.
     */
    TCsMmsNotificationMsgState MmsNotificationStatus( TInt aMsvId );
    
    /**
     * Mark specified messages as unread.
     * @param aIdArray Array of message ids.
     */
    void MarkMessagesReadL(RArray<TInt>& aIdArray);

    /**
     * Delete specified messages from message store
     * @param aIdArray Array of message ids.
     */
    void DeleteMessages(RArray<TInt>& aIdArray);
    
    /**
     * Returns the msv session object
     * @return CMsvSession object
     */
    CMsvSession& GetMsvSession();

    /**
     * Start fetching all draft messages
     */
    void FetchDraftMessages(DraftsModel* draftsModel);

private:

    /**
     * verifys the current operation status
     *@param aEntry, TMsvEntry
     */
    TBool OperationFinished( const TMsvEntry& aEntry ) const;

    /**
     * verifys the current operation status
     * @param aEntry, TMsvEntry
     */
    TBool OperationOngoing( const TMsvEntry& aEntry ) const;

    /**
     * CIdle callback
     */
    static TInt ProcessDraftMessages(TAny* arg);

    /**
     * State handler to read all draft messages 
     */
    TInt ProcessDraftMessagesL();

    /**
     * Cleanup
     */
    void CleanupL();

    /**
     * Checks if the MTM type supported
     */
    TBool IsMtmSupported(long uid);

private: 
    /**
     * Msv Session
     * Own. 
     */
    CMsvSession* iMsvSession;

    /**
     * Draft entry
     * Own
     */
    CMsvEntry* iDraftEntry;

    /**
     * Active object
     */
    CIdle* iIdle;

    /**
     * Messages under root entry
     * Own
     */
    CMsvEntrySelection* iDraftMessages;

    /**
     * Drafts model
     * Not Own
     */
    DraftsModel *mDraftsModel;

    /**
     * Mesage count
     */
    TInt iDraftMessageCount;

    /**
     * State
     */
    TDraftHandlerState iState;
    
    };

#endif // CONVERSATION_MSG_STORE_HANDLER_H

// EOF
