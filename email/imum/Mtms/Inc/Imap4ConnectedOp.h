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
*       Base class for operations requiring connection
*
*/

#ifndef __IMAP4CONNECTEDOP_H__
#define __IMAP4CONNECTEDOP_H__

#include <imapset.h>
#include <imapcmds.h>
#include <ImumInternalApi.h>        // CImumInternalApi

#include "IMAPPRGR.H"
#include "ImapConnectionOp.h"
#include "ImumDiskSpaceObserverOperation.h"
#include "Imap4MtmUi.hrh" // KImpuIMAPFolderNameLength

/**
* Base class for operations requiring connection.
*/
class CImap4ConnectedOp :
    public CImumDiskSpaceObserverOperation
    {
    public:
        /**
        *
        */
        virtual ~CImap4ConnectedOp();

        /**
        *
        */
        virtual const TDesC8& ProgressL();

        /**
        *
        */
        virtual const TDesC8& GetErrorProgressL(TInt aError);

    protected:

        /**
        *
        */
        CImap4ConnectedOp(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aServiceId );

        /**
        * @param aFuncId, defaulted to 'KErrNotFound'
        */
        void ConstructL(
            TInt aFuncId = KErrNotFound );

        /**
        *
        */
        void StartL();

        /**
        *
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
        virtual void DoConnectedOpL() = 0;

        /**
        *
        */
        virtual const TDesC8& ConnectedOpErrorProgressL(TInt aError) = 0;

        /**
        *
        */
        void DoConnectL();

        /**
        *
        */
        void DoDisconnectL();

    protected:
        TBool                                   iDisconnect;
        TDesC8*                                 iConnectedOpErrorProgress;
        TPckgBuf<TImap4CompoundProgress>        iProgressBuf;
        CImapConnectionOp::TImapConnectionType  iConnectionCompletionState;
        enum TConnectedOpState { EStateIdle, EStateConnecting, EStateDoingOp, EStateDisconnecting };
        TConnectedOpState                       iState;

    };



#endif
