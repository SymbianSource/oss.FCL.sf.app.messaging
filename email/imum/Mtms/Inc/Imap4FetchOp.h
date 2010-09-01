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
*       Populate a selection of incomplete remote messages
*
*/

#ifndef __IMAP4FETCHOP_H__
#define __IMAP4FETCHOP_H__

#include <imapset.h>
#include <imapcmds.h>
#include "IMAPPRGR.H"
#include "ImumOnlineOperation.h"
#include "Imap4ConnectedOp.h"
#include "Imap4MtmUi.hrh" // KImpuIMAPFolderNameLength


/**
* Populate a selection of incomplete remote messages.
* Uses the IMAP4 Client MTM 'Get Mail API'
*/
class CImap4FetchOp : public CImap4ConnectedOp
    {
    public:
        /**
        *
        * @param aSession, Reference to MsvServer Session
        * @param aStatus, Status for Active Scheduler
        * @param aReporter,
        * @param aServiceId,
        * @param aFunctionId, Function id to be used for mail fetching
        * @param TImImap4GetMailInfo, Fetching settings
        * @param aSel,
        * @return, Constructed object
        */
        static CImap4FetchOp* NewL(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aService,
            TInt aFunctionId,
            const TImImap4GetMailInfo& aGetMailInfo,
            const CMsvEntrySelection& aSel );

        /**
        *
        * @since S60 3.0
        * @param aSession, Reference to MsvServer Session
        * @param aStatus, Status for Active Scheduler
        * @param aReporter,
        * @param aServiceId,
        * @param aFunctionId, Function id to be used for mail fetching
        * @param aPartialGetMailInfo, Fetching settings
        * @param aSel,
        */
        static CImap4FetchOp* NewL(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aService,
            TInt aFunctionId,
            const TImImap4GetPartialMailInfo& aGetMailInfo,
            const CMsvEntrySelection& aSel );

        /**
        *
        */
        virtual ~CImap4FetchOp();
    protected:

        /**
        *
        */
        virtual void DoConnectedOpL();

        /**
        *
        */
        virtual const TDesC8& ConnectedOpErrorProgressL( TInt aError );

    private: // Constructors

        /**
        *
        */
        CImap4FetchOp(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aServiceId,
            TInt aFunctionId,
            const TImImap4GetMailInfo& aGetMailInfo );

        /**
        * Constructor
        * @since S60 3.0
        */
        CImap4FetchOp(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aServiceId,
            TInt aFunctionId,
            const TImImap4GetPartialMailInfo& aPartialGetMailInfo );

        /**
        *
        */
        void ConstructL(const CMsvEntrySelection& aSel);

        private: // New functions

        /**
        * Fetches the bodies to emails
        * @since S60 3.0
        */
        void DoPopulateL();

        /**
        *
        * @since S60 3.0
        * @param aEntry, Entry that contains inbox
        * @return The id of inbox
        */
        TMsvId FindInboxL( CMsvEntry& aEntry );



    private:
        const TInt iFunctionId;
        TImImap4GetMailInfo iGetMailInfo;
        TImImap4GetPartialMailInfo iPartialGetMailInfo;
        CMsvEntrySelection* iSelection;
        TBool   iPopulate;
    };

#endif
