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
*       CImapPopulateOp declaration file
*
*/

#ifndef __IMAPPOPULATEOP_H__
#define __IMAPPOPULATEOP_H__


#include <msvapi.h>
#include <imapset.h>
#include "imapconnectionobserver.h"
#include <mtmuibas.h>
#include <MTMStore.h>
#include <MsvPrgReporter.h>
#include <imapconnectcompletionstates.h>
#include <MuiuMsvSingleOpWatcher.h>

//  Forward declarations
//
class CMsvEmailConnectionProgressProvider;
class CCoeEnv;

/**
* class CImapPopulateOp
*
*/


class CImapPopulateOp :
    public CMsvOperation,
    public MMsvProgressDecoder
    {
    public:

        /**
        * NewL()
        * Basic factory function - creates dialog with standard title resource
        * @param CMsvSession& CMsvSession reference
        * @param TInt priority
        * @param TRequestStatus& request status
        * @param TMsvId service (mailbox) id
        * @param MMsvProgressReporter& prog. reporter reference
        * @param TImapConnectionType connection type
        * @return CImapPopulateOp* self pointer
        */

        static CImapPopulateOp* NewL(
            CMsvSession& aSession,
            TInt aPriority,
            TRequestStatus& aStatus,
            TMsvId aService,
            MMsvProgressReporter& aProgressReporter);

        /**
        * ~CImapPopulateOp()
        * destructor
        */

        ~CImapPopulateOp();

        /**
        * ProgressL()
        * @return TDesC8&, progress
        */

        const TDesC8& ProgressL();


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

    private:


        /**
        * CImapPopulateOp()
        * @param CMsvSession&, CMsvSession reference
        * @param TInt, priority
        * @param TRequestStatus&, request status
        * @param TMsvId, service ( mailbox ) id
        * @param MMsvProgressReporter&, prog. reporter
        */

        CImapPopulateOp(
            CMsvSession& aSession,
            TInt aPriority,
            TRequestStatus& aStatus,
            TMsvId aService,
            MMsvProgressReporter& aProgressReporter);

        /**
        * ConstructL()
        */

        void ConstructL();


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
        * FindInboxL()
        */
        TMsvId  FindInboxL( CMsvEntry& aEntry );

        /**
        * RunError()
        */        
		virtual TInt RunError(TInt aError);                       

    protected:
        /**
        *
        */

        CMsvOperation* DoPopulateL( CImImap4Settings& aSettings );

    private: //Data
        //
        CMsvOperation*                          iSubOperation;
        CMsvEmailConnectionProgressProvider*    iConnectionProgressProvider;

        TInt                                    iState;
        enum{EPopulateConstructing, EPopulating};

        TPckgBuf<TImap4SyncProgress>            iSyncProgress;
        TBool                                   iResetPassword;
        MMsvProgressReporter&                   iProgressReporter;
        CBaseMtmUi*                             iMtmUi;
        HBufC*                                  iTitleText;
        TMsvId                                  iRelatedServiceId;
        CCoeEnv*                                iCoeEnv;

        TBool                                   iPopulated;

      //  CMsvSingleOpWatcherArray                iOperations;//for more operations
    };

#endif //__IMAPPOPULATEOP_H__
