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
*       Very simple layer over base class just for setting up progress reporter
*
*
*/

#ifndef __IMUMDISCONNECTOPERATION_H__
#define __IMUMDISCONNECTOPERATION_H__

#include <pop3set.h>
#include <msvstd.h>
#include <MsvPrgReporter.h>
#include <ImumInternalApi.h>        // CImumInternalApi
#include "PROGTYPE.H"
#include "ImumOnlineOperation.h"
#include <imapset.h>

class CEikonEnv;
class CBaseMtmUi;
class CMsvEntrySelection;
class MMsvProgressReporter;


/**
* class CImumDisconnectOperation
*
* Very simple layer over base class just for setting up progress reporter.
*/
class CImumDisconnectOperation :
    public CImumOnlineOperation,
    public MMsvProgressDecoder
    {
    public:

        /**
        * NewL
        * @param CMsvSession&, session reference
        * @param TRequestStatus&, request status
        * @param MMsvProgressReporter&, progress reporter
        * @param TMsvId, entry id
        * @param TInt, function id
        * @param TUid, mtm type. Pop or Imap
        * @return CImumDisconnectOperation*, self pointer
        */
        static CImumDisconnectOperation* NewL(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aService,
            TInt aFunctionId,
            TUid aMtmType);

        /**
        * ~CImumDisconnectOperation
        * Destructor
        */
        virtual ~CImumDisconnectOperation();

        /**
        * GetErrorProgressL
        * @param TInt, error
        */
        virtual const TDesC8& GetErrorProgressL(TInt aError);

        /**
        * ProgressL
        * @return TDesC8&, progress
        */
        virtual const TDesC8& ProgressL();

        /**
        * DecodeProgress
        * From MMsvProgressDecoder
        * @param const TDesC8&, progress
        * @param TBuf<CBaseMtmUi::EProgressStringMaxLen>&, return string
        * @param TInt& total entry count
        * @param TInt& entries done
        * @param TInt& current entry size
        * @param TInt& current bytes trasferred
        * @param TBool, aInternal
        * @return TInt
        */
        virtual TInt DecodeProgress(
            const TDesC8& aProgress,
            TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans,
            TBool aInternal );

    protected:
        /**
        * DoRunL
        * From CImumOnlineOperation
        */
        virtual void DoRunL();

    private:
        /**
        * C++ constructor
        * @param CMsvSession&, session reference
        * @param TRequestStatus&, request status
        * @param MMsvProgressReporter&, progress reporter
        * @param TMsvId, entry id
        */
        CImumDisconnectOperation(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aService);

        /**
        * Symbian OS constructor
        * @param TInt, function id
        * @param TUid, Mtm type. Pop or Imap
        */
        void ConstructL(TInt aFunctionId, TUid aMtmType);

    };

#endif
