/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       CImapConnectionOp declaration file
*
*/

#ifndef __IMAPCONNECTIONOP_H__
#define __IMAPCONNECTIONOP_H__

#include <msvapi.h>
#include <imapset.h>
#include "imapconnectionobserver.h"
#include <mtmuibas.h>
#include <MTMStore.h>
#include <MsvPrgReporter.h>
#include <imapconnectcompletionstates.h>
#include <MuiuMsvSingleOpWatcher.h>
#include <ImumInternalApi.h>

//  Forward declarations
//
class CMsvEmailConnectionProgressProvider;
class CCoeEnv;

/**
* class CImapConnectionOp
*
*/
class CImapConnectionOp :
    public CMsvOperation,
    public MMsvImapConnectionObserver,
    public MMsvProgressDecoder,
    public MMsvSingleOpWatcher
    {
    public:

        enum TImapConnectionType
            {
            ECompleteAfterConnect,
            EWithSyncCompleteAfterConnect,
            EWithSyncCompleteAfterFullSync,
            EWithSyncCompleteAfterDisconnection
            };

        /**
        * NewL()
        * Basic factory function - creates dialog with standard title resource
        * @param CMsvSession& CMsvSession reference
        * @param TInt priority
        * @param TRequestStatus& request status
        * @param TMsvId service (mailbox) id
        * @param MMsvProgressReporter& prog. reporter reference
        * @param TImapConnectionType connection type
        * @return CImapConnectionOp* self pointer
        */
        static CImapConnectionOp* NewL(
            CMsvSession& aMsvSession,
            TInt aPriority,
            TRequestStatus& aStatus,
            TMsvId aService,
            MMsvProgressReporter& aProgressReporter,
            TImapConnectionType aConnectionType);

        /**
        * ~CImapConnectionOp()
        * destructor
        */
        ~CImapConnectionOp();

        /**
        * ProgressL()
        * @return TDesC8&, progress
        */
        const TDesC8& ProgressL();

        /**
        * HandleImapConnectionEvent()
        * From MMsvImapConnectionObserver
        * @param TImapConnectionEvent, imap connection event
        */
        virtual void HandleImapConnectionEvent(
            TImapConnectionEvent aConnectionEvent);

        /**
        * DecodeProgress()
        * From MMsvProgressDecoder
        * @param TDesC8& progress
        * @param TBuf<CBaseMtmUi::EProgressStringMaxLen>&, return string
        * @param TInt&, total entry count
        * @param TInt& entries done
        * @param TInt& current entry size
        * @param TInt& current bytes transferred
        * @param TBool internal flag
        */
        virtual TInt DecodeProgress(
            const TDesC8& aProgress,
            TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans,
            TBool aInternal);

        /**
        * OpCompleted()
        * From MMsvSingleOpWatcher
        * @param CMsvSingleOpWatcher& opwatcher which completed
        * @param TInt, completion code
        */
        virtual void OpCompleted(
            CMsvSingleOpWatcher& aOpWatcher,
            TInt aCompletionCode );

    private:

        enum TImapConnectionState
            {
            EConstructing,
            EEstablishingConnection,
            EUpdatingFolderList,
            EUpdatingInbox,
            EUpdatingFolders,
            EOnline,
            ECompleted
            };

        /**
        * CImapConnectionOp()
        * @param CMsvSession&, CMsvSession reference
        * @param TInt, priority
        * @param TRequestStatus&, request status
        * @param TMsvId, service ( mailbox ) id
        * @param MMsvProgressReporter&, prog. reporter
        * @param TImapConnectionType, connection type
        */
        CImapConnectionOp(
            CMsvSession& aMsvSession,
            TInt aPriority,
            TRequestStatus& aStatus,
            TMsvId aService,
            MMsvProgressReporter& aProgressReporter,
            TImapConnectionType aConnectionType );

        /**
        * ConstructL()
        */
        void ConstructL();


        /**
        * GetConnectionOperationL()
        * Generate the appropriate type of connection operation
        * @return CMsvOperation*, operation
        */
        CMsvOperation* GetConnectionOperationL();

        /**
        * DecodeConnectionProgress()
        * @param TDesC8& progress
        * @param TBuf<CBaseMtmUi::EProgressStringMaxLen>&, return string
        * @param TInt&, total entry count
        * @param TInt& entries done
        * @param TInt& current entry size
        * @param TInt& current bytes transferred
        */
        TInt DecodeConnectionProgress(
            const TDesC8& aProgress,
            TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans);

        /**
        * RunL()
        */
        virtual void RunL();

        /**
        * DoRunL()
        */
        void DoRunL();

        /**
        * FinishConstructionL()
        */
        void FinishConstructionL();

        /**
        * ConnectionCompletionL()
        */
        void ConnectionCompletionL();

        /**
        * DoCancel()
        */
        virtual void DoCancel();

        /**
        * Complete()
        */
        void Complete();
        
        /**
        * RunError()
        */
		virtual TInt RunError(TInt aError);                       

    private: //Data
        //Owned
        CImumInternalApi*                       iMailboxApi;
        CMsvOperation*                          iSubOperation;
        CMsvEmailConnectionProgressProvider*    iConnectionProgressProvider;
        TImapConnectionState                    iState;
        TImapConnectionType                     iConnectionType;
        TPckgBuf<TImap4SyncProgress>            iSyncProgress;
        MMsvProgressReporter&                   iProgressReporter;
        CBaseMtmUi*                             iMtmUi;
        HBufC*                                  iTitleText;
        HBufC*                                  iConnectionText;
        TMsvId                                  iRelatedServiceId;
        CCoeEnv*                                iCoeEnv;
        TInt                                    iLoginRetryCounter;

      //  CMsvSingleOpWatcherArray                iOperations;//for more operations
    };

#endif
