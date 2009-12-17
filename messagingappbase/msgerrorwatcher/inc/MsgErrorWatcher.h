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
*     CMsgErrorWatcher declaration file
*
*/



#ifndef MSGERRORWATCHER_H
#define MSGERRORWATCHER_H

// INCLUDES
#include <f32file.h>
#include <msvstd.h>
#include <msvapi.h>
#include <systemwarninglevels.hrh>    // Critical level
#include <mmsconst.h>          // KMmsDiskSafetyMargin

#include "MsgErrorWatcher.hrh"

const TUint KCriticalDiskLevel = KDRIVECCRITICALTHRESHOLD;
const TUint KCriticalLevel = KCriticalDiskLevel + KMmsDiskSafetyMargin;
const TUint32 KDefaultTriggerLevel = 300 * 1024; // 100 kilobytes
const TUint KTriggerMargin = 20 * 1024; // 20 kilobytes

// FUNCTION PROTOTYPES
GLREF_C void Panic( TInt aPanic ); 

// FORWARD DECLARATIONS
class CWatcherLog;
class CMmsClientMtm;
class CClientMtmRegistry;
class CStringResourceReader;
class CMsgSentItemsObserver;
class CMsgErrorCommDbObserver;
class CMsgErrorConnectionObserver;
class CMsgErrorDiskSpaceObserver;
class CMsgErrorSmsDiskSpaceObserver;
class CMsgErrorExtSmsDiskSpaceObserver;
class CMsgErrorRoamingObserver;
class CMsgErrorStartupObserver;
class CMsgErrorGlobalQuery;
class CMsgErrorDisconnectDlg;
class CMsgCenRepObserver;
class CMsgLogsObserver;
class CMsgGarbageCollection;

// CLASS DECLARATION

/**
* CMsgErrorWatcher
*
* @lib msgerrorwatcher.dll
* @since 2.0
*/
class CMsgErrorWatcher :
    public CActive,
    public MMsvSessionObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * The parameters must be exactly these. Defined by
        * Symbian watcher framework
        *
        * @param aFs A reference to file session
        * @param aLog A reference to watcher log
        */
        static CMsgErrorWatcher* NewL( TAny* aWatcherParams );
        
        /**
        * Destructor.
        */
        virtual ~CMsgErrorWatcher();

    public: // New functions

        /**
        * Callback function for roaming observer.
        *
        * @param aRoaming ETrue if roaming, EFalse otherwise
        */
        void HandleRoamingEventL( TBool aRoaming );

        /**
        * Callback function for CommDB observer.
        */
        void HandleCommDbEventL();

        /**
        * Callback function for connection observer.
        */
        void HandleConnectionEvent();

        /**
        * Callback function for disk space observer.
        */
        void HandleDiskSpaceEventL();
        void HandleDiskSpaceEvent2L();
        /**
        * Callback function for global query observer.
        */
        void HandleGlobalQueryEventL( TInt aQueryId, TInt aStatus );

        /**
        * Callback function for startup state observer (currently
        * sent items observer).
        */
        void HandleStartupReadyL();
        
        /**
        * HandleCenRepNotificationL
        * Handles events from central repository observer
        */
        void HandleCenRepNotificationL();
        
        /*
        * Hadles events from logs observer
        */
        void ShowDeliveredNoteL( const TDesC& aRemoteParty );
        
        /*
        * Hadles events from logs observer
        */
        void ShowReadNoteL( const TDesC& aRemoteParty );
        
        /*
        * Showns read or delivered note
        */
        void ShowReadOrDeliveredNoteL( const TDesC& aRemoteParty, TInt aResourceId );
        
        
    public: // Functions from base classes

        /**
        * From MMsvSessionObserver
        */
        void HandleSessionEventL(
            TMsvSessionEvent aEvent,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3 );

    protected: // Functions from base classes

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Private C++ constructor.
        *
        * @param aFs A reference to file session
        */
        CMsgErrorWatcher( RFs& aFs );

        /**
        * Start watcher.
        */
        void StartWatcherL();

        /**
        * Stop watcher.
        */
        void StopWatcher();

        /**
        * Start restart timer. Timer is used when trying to reconnect
        * to message server after watcher has been stopped due
        * to certain message server events.
        */
        void StartRestartTimer();

        /**
        * Initialise iErrorMessages array
        */
        void InitMessageArraysL();

        /**
        * Kills all observers and resets counters.
        */
        void ResetWatcher();

        /**
        * Checks whether MMS Service is already found. If not
        * gets a pointer to MMS service from MsgStore and if
        * found updates the context of MmsClientMtm.
        *
        * @return ETrue if MMS service was found, EFalse otherwise
        */
        TBool GetMmsServiceL();

        /**
        * Initiates MMS fetch by sending a request to MMS engine.
        */
        void StartMmsFetchL();

        /**
        * Initiates MMS send by sending a request to MMS engine.
        */
        void StartMmsSendL();

        /**
        * Checks whether MMS receiving mode is "Enabled in home network"
        * and "On". If so, activates roaming observer.
        */
        void CheckMmsReceivingModeL();

        /**
        * A wrapper for CMmsClient::ValidateService().
        *
        * @return ETrue if service is valid, EFalse otherwise.
        */
        TBool ValidateMmsServiceL();

        /**
        * Shows a global query when message sending, (manual)
        * retrieving or forwarding has failed.
        */
        void ShowErrorMessageQueryL();

        /**
        * Shows a global note
        *
        * @param aNoteId The identifier of the note
        */
        void ShowGlobalNoteL( TMsgErrorNoteIds aNoteId );

        /**
        * Cancels all global notes
        */
        void CancelNotesL();

        /**
        * Finds the next message entry for which to
        * to show an error message.
        * @param aEntry OUT  The possibly found message entry
        * @return Standard Symbian error code.
        *         KErrNone if message was found.
        */
        TInt GetNextErrorMsg( TMsvEntry& aEntry );

        /**
        * Resolves the first text for
        * send/(manual)retrieve/forward failure
        * @param aEntry IN   Valid message entry
        */
        void ResolveErrorTextL( TMsvEntry& aEntry );

        /**
        * Resolves the possible details text
        * for send/(manual)retrieve/forward failure
        * @param aEntry IN   Valid message entry
        */
        void ResolveErrorDetailsL( TMsvEntry& aEntry );

        /**
        * Called when MMS reception fails due to insufficient
        * disk space.
        *
        * @param aEntry The failed entry
        */
        void HandleDiskSpaceErrorL( TMsvEntry& aEntry );

        /**
        * Called when MMS reception fails due to insufficient
        * memory.
        *
        * @param aEntry The failed entry
        */
        void HandleMemoryErrorL( TMsvEntry& aEntry );

        /**
        * Called when MMS reception fails because there is
        * another data connection open.
        *
        * @param aEntry The failed entry
        * @param aReceive, ETrue for incoming messages, EFalse for outgoing
        */
        void HandleConnectionErrorL( TMsvEntry& aEntry, TBool aReceive );

        /**
        * Called when MMS reception fails because there is no
        * access point defined.
        *
        * @param aEntry The failed entry
        */
        void HandleNoAPErrorL( TMsvEntry& aEntry );

        /**
        * Called when MMS reception fails because the current
        * access point is invalid.
        *
        * @param aEntry The failed entry
        */
        void HandleInvalidAPErrorL( TMsvEntry& aEntry, TBool aStartObserver );

        /**
        * Subfunction of HandleSessionEventL
        * Handles root events from MsgServer observer
        *
        * @param aEvent     Message server event
        * @param aEntries   Affected entries
        */
        void HandleRootEventL(
            TMsvSessionEvent aEvent,
            CMsvEntrySelection* aEntries );

        /**
        * Subfunction of HandleSessionEventL
        * Handles local service events from MsgServer observer
        *
        * @param aEvent     Message server event
        * @param aEntries   Affected entries
        */
        void HandleLocalServiceEventL(
            TMsvSessionEvent aEvent,
            CMsvEntrySelection* aEntries );

        /**
        * Subfunction of HandleSessionEventL
        * Handles inbox events from MsgServer observer
        *
        * @param aEvent     Message server event
        * @param aEntries   Affected entries
        */
        void HandleInboxEventL(
            TMsvSessionEvent aEvent,
            CMsvEntrySelection* aEntries );

        /**
        * Subfunction of HandleSessionEventL
        * Handles outbox events from MsgServer observer
        *
        * @param aEvent     Message server event
        * @param aEntries   Affected entries
        */
        void HandleOutboxEventL(
            TMsvSessionEvent aEvent,
            CMsvEntrySelection* aEntries );

        /**
        * Subfunction of HandleSessionEventL
        * Handles MMS service events from MsgServer observer
        *
        * @param aEvent     Message server event
        * @param aEntries   Affected entries
        */
        void HandleMmsServiceEventL(
            TMsvSessionEvent aEvent,
            CMsvEntrySelection* aEntries );


        /**
        * Resets TMsvEntry::iError of an entry saved in iCurrentEntry
        */
        void ResetErrorFieldL( );
        
        /**
        * Resets TMsvEntry::iError of an entry given as parameter
        */
        void ResetErrorFieldL( TMsvEntry& aEntry );
         
    private:    // Data

        enum TMsgErrorWatcherFlags
            {
            EStartupReady               = 0x0001,
            EWatcherRunning             = 0x0002,
            ENoAPErrorPending           = 0x0004,
            EShowRoamingNote            = 0x0008,
            ERoamingNoteShown           = 0x0010,
            EReceivingDisconnectDelay   = 0x0020
            };

        enum TMsgRequestTypes
            {
            EMsgRequestNone = 0,
            EMsgRequestStartingUp,
            EMsgRequestFetching,
            EMsgRequestFetchingAll,
            EMsgRequestSending,
            EMsgRequestWaitingErrorNote,
            EMsgRequestWaitingDisconnection
            };


        TMsvId iMmsServiceId;
        TMsvId iNotificationFolderId;
        CMsvSession* iSession;
        CMsvOperation* iOperation;
        CClientMtmRegistry* iClientMtmRegistry;
        CMmsClientMtm* iMmsClient;
        CMsvEntrySelection* iMmsReceiveErrorMessages;
        CMsvEntrySelection* iMmsSendErrorMessages;
        CMsvEntrySelection* iErrorMessages;
        CArrayFixFlat<TInt>* iNoteIds;
        CStringResourceReader* iResourceReader;
        TMsvId iCurrentEntryId;

        CMsgSentItemsObserver* iSentItemsObserver;
        CMsgErrorCommDbObserver* iCommDbObserver;
        CMsgErrorConnectionObserver* iConnectionObserver;
        CMsgErrorDiskSpaceObserver* iDiskSpaceObserver;
        CMsgErrorSmsDiskSpaceObserver* iSmsDiskSpaceObserver;
        CMsgErrorExtSmsDiskSpaceObserver* iSmsExtDiskSpaceObserver;
        CMsgErrorRoamingObserver* iRoamingObserver;
        CMsgErrorStartupObserver* iStartupObserver;
        CMsgErrorDisconnectDlg* iDisconnectDlg;
        CMsgCenRepObserver* iCenRepObserver;
        CMsgLogsObserver* iLogsObserver;
        CMsgGarbageCollection* iGarbageCollection;

        HBufC* iErrorMsgText;
        HBufC* iErrorMsgDetails;
        CMsgErrorGlobalQuery* iErrorQuery;
        CMsgErrorGlobalQuery* iDisconnectQuery;
        CMsvEntrySelection* iErrorMsgNotes;

        RFs& iFs;
        RTimer iTimer;

        TMsgRequestTypes iRequestType;
        TUint iWatcherFlags;
        TUint32 iMaxReceiveSize;
        TUint iDiskSpaceErrors;
        TUint iTimerRetries;
    };

#endif      // MSGERRORWATCHER_H
            
// End of File
