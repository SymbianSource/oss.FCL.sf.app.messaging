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
*       SMTP Cancel Sending operation
*
*/

#ifndef __SMTPCANCELSENDINGOP_H__
#define __SMTPCANCELSENDINGOP_H__

#include <ImumInternalApi.h>        // CImumInternalApi

#include "SelectionOperationBase.h"


class CMsvEmailConnectionProgressProvider;
class CMtmStore;

/**
* SMTP Cancel Sending operation
*/
class CSmtpCancelSendingOp : public CSelectionOperationBase
    {
    public:
        /**
        *
        */
        static CSmtpCancelSendingOp* NewL(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            CMsvEntrySelection* aEntrySelection );

        /**
        * Destructor
        */
        virtual ~CSmtpCancelSendingOp();

        /**
        *
        */
        virtual const TDesC8& ProgressL();
    private:

        /**
        * C++ constructor
        */
        CSmtpCancelSendingOp(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            CMsvEntrySelection* aEntrySelection );

        /**
        * Symbian OS constructor
        */
        void ConstructL();

        /**
        *
        */
        virtual CMsvOperation* CreateOperationL(TMsvId aMsvId);

        /**
        *
        */
        virtual TBool StopOnError(TInt aErrorCode);

        /**
        *
        */
        virtual void OpRunError(TInt aErrorCode);

        /**
        *
        */
        virtual TInt CheckForError(const TDesC8& aProgress);

    private: // Data
        TPckgBuf<TMsvLocalOperationProgress> iProgress;
        CMsvEntry* iEntry;
    };


#endif
