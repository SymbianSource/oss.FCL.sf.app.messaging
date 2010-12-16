/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*       CMsgSentItemsObserver declaration file
*
*/



#ifndef MSGSENTITEMSOBSERVER_H
#define MSGSENTITEMSOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <msvapi.h>
#include <cenrepnotifyhandler.h> // link against CenRepNotifHandler.lib

#include "MsgErrorWatcher.h"

// FORWARD DECLARATIONS

class CRepository;

// CLASS DECLARATION

/**
* CMsgSentItemsObserver
*
* @lib msgerrorwatcher.dll
* @since 2.0
*/
class CMsgSentItemsObserver : public CActive,
                              public MMsvEntryObserver,
                              public MCenRepNotifyHandlerCallback
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcher
        * @param aSession A pointer to CMsvSession
        */
        static CMsgSentItemsObserver* NewL( 
            CMsgErrorWatcher* aWatcher,
            CMsvSession* aSession );
        
        /**
        * Destructor.
        */
        ~CMsgSentItemsObserver();
    
    public: // from base classes
    
        /**
        * From MMsvEntryObserver
        *
        * Call back from msg that the sent items folder has changed
        * so check if messages need to be deleted.
        */
        void HandleEntryEventL(
            TMsvEntryEvent aEvent,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3 );

        /**
        * From MCenRepNotifyHandlerCallback
        *
        * Notifications from CenRep
        */
        void HandleNotifyInt(TUint32 aId, TInt aNewValue);
        void HandleNotifyError(TUint32 aId, TInt error, CCenRepNotifyHandler* aHandler);
        void HandleNotifyGeneric(TUint32 aId);

    private:

        enum TCleanupFlags
            {
            EMsvSessionOpened               =0x00000001,
            ESentItemsFolderObserverAdded   =0x00000002,
            EUserSettingsConnected          =0x00000004, 
            EUserSettingsNotifierSet        =0x00000010
            };
    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Private C++ constructor.
        *
        * @param aWatcher A pointer to CMsgErrorWatcher
        * @param aSession A pointer to CMsvSession
        */
        CMsgSentItemsObserver(
            CMsgErrorWatcher* aWatcher,
            CMsvSession* aSession );

        /**
        * Returns the user setting for use of the sent folder. If there
        * is an error then the default value of ON (i.e ETrue) will
        * be returned.
        *
        * @return ETrue, Sent folder is in use
        *         EFalse, Sent foler is not in use
        */
        TBool SentFolderIsBeingUsed();

        /**
        * Returns the user setting for max limit to the number of messages
        * in the sent folder. If there is an error then the default value of
        * 15 will be returned
        *
        * @return Maximum amount of messages in Sent folder
        */
        TInt SentFolderUpperLimit();

        /**
        * If there are more messages in the sent folder than the upper
        * limit set by the user then delete the oldest messages.
        */
        void DeleteOldMessagesFromSentFolderL();

        /**
        * Delete the messages specified by aSelection
        *
        * @param aSelection Enties to be deleted
        */
        void DeleteMessagesFromSentFolderL( CMsvEntrySelection* aSelection );
		
		/**
        * Delete the messages specified by aSelection
        * aSelection is expected to have count 1
        * @param aSelection Enties to be deleted
        */
        void DeleteSingleEntryL(CMsvEntrySelection* aSelection);

    private: //from CActive

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();

    private:    // Data

        CMsgErrorWatcher* iWatcher;
        CMsvSession* iSession;
        CMsvEntry* iSentItemsFolder;
        CMsvEntry* iEntry;
        CCenRepNotifyHandler* iNotifyHandler;
        CRepository* iRepository;
        TInt8 iCleanupFlags;
        CMsvOperation* iOp;
        TInt iMsgStoreDrive;
        
        class CMsvEntrySelection*	iQudUPEntryBeingDeleted;
        class CMsvEntrySelection*	iQueuedUPEntriesToBeDeleted;
        TInt						iRetryCounterIfEntryDelFailed;
        
    };

#endif      // MSGSENTITEMSOBSERVER_H
            
// End of File
