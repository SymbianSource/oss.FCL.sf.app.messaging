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
*       Fetch operation with checking of available memory and disk space
*
*/

#ifndef __IMAP4SIZEANDFETCHOP_H__
#define __IMAP4SIZEANDFETCHOP_H__

#include <ImumInternalApi.h>        // CImumInternalApi
#include <imapset.h>
#include <imapcmds.h>
#include "IMAPPRGR.H"
#include "ImumOnlineOperation.h"
#include "Imap4MtmUi.hrh" // KImpuIMAPFolderNameLength

/**
* Fetch operation with checking of available memory and disk space.
*/
class CImap4SizeAndFetchOp : public CImumOnlineOperation
    {
    public:
        /**
        *
        */
        static CImap4SizeAndFetchOp* NewL(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aService,
            TInt aFunctionId,
            const TImImap4GetMailInfo& aGetMailInfo,
            const CMsvEntrySelection& aSel);

        /**
        *
        */
        virtual ~CImap4SizeAndFetchOp();

        /**
        *
        */
        virtual const TDesC8& GetErrorProgressL(TInt aError);

    protected:

        /**
        *
        */
        CImap4SizeAndFetchOp(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aService,
            TInt aFunctionId,
            const TImImap4GetMailInfo& aGetMailInfo);

        /**
        *
        */
        void ConstructL(const CMsvEntrySelection& aSel);

        /**
        *
        */
        virtual void DoRunL();

        /**
        *
        */
        void DoSizeMessageL();

        /**
        *
        */
        void DoSizeNextMessageL();

        /**
        *
        */
        void DoFetchMessageL();

        /**
        *
        */
        TBool CheckMessageSizeL();

    private:
        enum TSizeAndFetchState { EStateIdle, EStateSizeMsg, EStateFetchMsg };
        TSizeAndFetchState iState;
        TPckgBuf<TImap4CompoundProgress> iProgressBuf;
        CMsvEntrySelection* iSelection;
        const TInt iFunctionId;
        const TImImap4GetMailInfo iGetMailInfo;
        TInt32   iTotalSizeToFetch;
        TInt    iCurrentMsgToSize;
    };

#endif
