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
*       Fetch message(s) operation, using client MTM Get Mail API
*
*/

#ifndef __MSVPOP3FETCHOPERATION_H__
#define __MSVPOP3FETCHOPERATION_H__

#include <pop3set.h>
#include <MsvPrgReporter.h>
#include <ImumInternalApi.h>        // CImumInternalApi

#include "ImumDiskSpaceObserverOperation.h"

#include "ImumMtmLogging.h"

class CMsvEmailConnectionProgressProvider;

/**
* CMsvPop3FetchOperation
* Fetch message(s) operation, using client MTM Get Mail API.
* Encapsulates checking for a connection, and if neccessary, connecting beforehand
* and disconnecting afterwards.
*/
class CMsvPop3FetchOperation :
    public CImumDiskSpaceObserverOperation
    {
    public:
        /**
        *
        */
        static CMsvPop3FetchOperation* NewL(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            TInt aFunctionId,
            TMsvId aService,
            const TImPop3GetMailInfo& aGetMailInfo,
            const CMsvEntrySelection& aSel );

        /**
        * for partial fetch
        */
        static CMsvPop3FetchOperation* NewL(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            TInt aFunctionId,
            TMsvId aService,
            const TImPop3GetMailInfo& aGetMailInfo,
            const CMsvEntrySelection& aSel,
            TInt aLimit);



        /**
        *
        */
        virtual ~CMsvPop3FetchOperation();

        /**
        *
        */
        virtual const TDesC8& ProgressL();

        /**
        *
        */
        virtual const TDesC8& GetErrorProgressL( TInt aError );

    protected:

        /**
        *
        */
        CMsvPop3FetchOperation(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            TInt aFunctionId,
            TMsvId aService,
            const TImPop3GetMailInfo& aGetMailInfo);

        /**
        * for partial fetch
        */
        CMsvPop3FetchOperation(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            TInt aFunctionId,
            TMsvId aService,
            const TImPop3GetMailInfo& aGetMailInfo,
            TInt aLimit);

        /**
        *
        */
        void ConstructL( const CMsvEntrySelection& aSel );

        /**
        * From CActive
        */
        virtual void RunL();

        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */
        virtual void DoRunL();

        /**
        *
        */
        void DoConnectL();

        /**
        *
        */
        void DoFetchL();

        /**
        *
        */
        void DoDisconnectL();

    protected:

        enum TFetchState {
            EStateIdle,
            EStateConnecting,
            EStateFetching,
            EStateDisconnecting };
        TFetchState iState;

    protected:
        TBool iDisconnect;
        TDesC8* iFetchErrorProgress;
        TInt iFunctionId;
        TImPop3GetMailInfo iGetMailInfo;
        CMsvEntrySelection* iSelection;
        TInt iPopulateLimit;
    };


#endif

// End of File
