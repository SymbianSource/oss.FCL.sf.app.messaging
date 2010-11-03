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
*       Folder delete
*
*/

#ifndef __IMAP4CONNECTEDDELETEOP_H__
#define __IMAP4CONNECTEDDELETEOP_H__

#include <imapset.h>
#include <imapcmds.h>
#include "Imap4ConnectedOp.h"
#include "IMAPPRGR.H"
#include "ImumOnlineOperation.h"
#include "Imap4MtmUi.hrh" // KImpuIMAPFolderNameLength


/**
* Folder delete
*/
class CImap4ConnectedDeleteOp : public CImap4ConnectedOp
    {
    public:
        /**
        *
        */
        static CImap4ConnectedDeleteOp* NewL(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aServiceId,
            const CMsvEntrySelection& aSel);

        /**
        *
        */
        virtual ~CImap4ConnectedDeleteOp();

    protected:
        /**
        * From CImap4ConnectedOp
        */
        virtual void DoConnectedOpL();

        /**
        * From CImap4ConnectedOp
        */
        virtual const TDesC8& ConnectedOpErrorProgressL(TInt aError);

    private:
        /**
        *
        */
        CImap4ConnectedDeleteOp(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aServiceId);

    private:
        TImap4UiProgressBuf iUiProgressBuf;
        CMsvEntrySelection* iSelection;
    };


#endif
