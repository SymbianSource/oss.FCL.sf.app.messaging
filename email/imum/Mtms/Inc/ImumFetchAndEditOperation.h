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
*       Common email MTM wrapper operation
*
*/


#ifndef __IMUMFETCHANDEDITOPERATION_H__
#define __IMUMFETCHANDEDITOPERATION_H__

#include <msvstd.h>
#include "PROGTYPE.H"
#include "ImumDiskSpaceObserverOperation.h"
#include "ImumEditOperation.h"

class CEikonEnv;
class CBaseMtmUi;
class CMsvEntrySelection;
class MMsvProgressReporter;

/**
* class CImumFetchAndEditOperation
*
* Common email MTM wrapper operation that takes an active operation that
* fetches a message, and launches an editor when the fetch completes.
*/
class CImumFetchAndEditOperation :
    public CImumDiskSpaceObserverOperation
    {
    public:
        /**
        *
        */
        static CImumFetchAndEditOperation* NewL(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            const TEmailEditParams& aParams,
            TBool aAckReceipts);

        /**
        *
        */
        virtual ~CImumFetchAndEditOperation();

        /**
        * For reporting DoRunL leaves.
        */
        virtual const TDesC8& GetErrorProgressL(TInt aError);

        /**
        *
        */
        void SetFetchOpL(CMsvOperation* aFetchOp);

        /**
        *
        */
        TRequestStatus& RequestStatus();

    protected:

        /**
        *
        */
        virtual void DoRunL();
    private:
        /**
        *
        */
        CImumFetchAndEditOperation(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            const TEmailEditParams& aParams,
            TBool aAckReceipts);

        /**
        *
        */
        void DoEditL();

    private:
        enum TFetchAndEditState { EStateIdle, EStateFetching, EStateEditing, EStateComplete };
        TFetchAndEditState iState;
        TEmailEditParams iEditParams;
        TUid iActualMtm;
        TPckgBuf<TMsvLocalOperationProgress> iLocalProgress;
        TBool iAckReceipts;
    };

#endif

// End of File
