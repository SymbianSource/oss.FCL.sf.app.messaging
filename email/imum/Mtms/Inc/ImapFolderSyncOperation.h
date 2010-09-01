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
* Description: ImapFolderSyncOperation.h
*
*
*/


#ifndef __IMAPFOLDERSYNCOPERATION_H__
#define __IMAPFOLDERSYNCOPERATION_H__

//INCLUDES
#include <msvapi.h>
#include <ImumInternalApi.h>        // CImumInternalApi

class CBaseMtmUi;

class CImapFolderSyncOperation : public CMsvOperation
    {
    public:
        /**
        * Symbian OS constructor.
        */
        static CImapFolderSyncOperation* NewL( CImumInternalApi& aMailboxApi, TRequestStatus& aStatus, TMsvId aMailbox, CBaseMtmUi& aMtmUi );


        /**
        * destructor
        */
        virtual ~CImapFolderSyncOperation();

    private:
       /**
        * c++ constructor
        */
		CImapFolderSyncOperation(
            CImumInternalApi& aMailboxApi,
            TInt aPriority,
            TRequestStatus& aObserverRequestStatus,
            CBaseMtmUi& aMtmUi );

        /**
        * Symbian OS constructor.
        */
		void ConstructL( TMsvId aMailbox );

        /**
        * HasSubscribedFoldersL()
        * @return ETrue if has subbed folders
        */
        TBool HasSubscribedFoldersL();

    public:

        /**
        *
        */
		virtual const TDesC8& ProgressL();

        /**
        *
        */
		void SetInterval();


	protected:

        /**
        * From CActive
        */
		virtual void DoCancel();

        /**
        * From CActive
        */
		virtual void RunL();


    private:

        /**
        * Counts subscribed folder
        * NOTE: This is recursive
        */
        void CountSubscribedFoldersL(
            TMsvId aFolderId, TInt& aCount );


	protected: // Data
		RTimer iTimer;
		TBuf8<1> iProgress;
        TTimeIntervalMinutes iInterval;
        TMsvId iMailboxId;
        CBaseMtmUi& iMtmUi;
        CImumInternalApi& iMailboxApi;

        CMsvOperation* iOperation;
    };


#endif
// End of File
