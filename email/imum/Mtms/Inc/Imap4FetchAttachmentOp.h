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
*       Fetch attachments
*
*/

#ifndef __IMAP4FETCHATTACHMENTOP_H__
#define __IMAP4FETCHATTACHMENTOP_H__

#include <imapset.h>
#include <imapcmds.h>
#include "IMAPPRGR.H"
#include "ImumOnlineOperation.h"
#include "Imap4ConnectedOp.h"
#include "Imap4MtmUi.hrh" // KImpuIMAPFolderNameLength


/**
* Fetch attachments after checking there is enough memory and disk space to
* fetch the first attachment in the selection.
* Uses the KIMAP4MTMPopulate function Id directly.
*/
class CImap4FetchAttachmentOp : public CImap4ConnectedOp
    {
    public:
        /**
        *
        */
        static CImap4FetchAttachmentOp* NewL(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aService,
            const CMsvEntrySelection& aSel);

        /**
        *
        */
        virtual ~CImap4FetchAttachmentOp();

        /**
        *
        */
        virtual const TDesC8& ProgressL();

    protected:
        /**
        * Do the fetch
        */
        virtual void DoConnectedOpL();

        /**
        *
        */
        virtual const TDesC8& ConnectedOpErrorProgressL(TInt aError);

    private:

        /**
        *
        */
        CImap4FetchAttachmentOp(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aService);

        /**
        *
        */
        void ConstructL(const CMsvEntrySelection& aSel);

    private:
        CMsvEntrySelection* iSelection;
        TImap4UiProgressBuf iUiProgress;
    };


#endif
