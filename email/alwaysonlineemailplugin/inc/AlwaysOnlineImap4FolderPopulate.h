/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     Populates imap4 folders using current populate settings.
*
*/

#ifndef __ALWAYSONLINEIMAP4FOLDERPOPULATE_H__
#define __ALWAYSONLINEIMAP4FOLDERPOPULATE_H__

#include <msvapi.h> // CMsvOperation
#include <imapset.h> // TImImap4GetPartialMailInfo

class CImap4ClientMtm;
class TImImap4GetPartialMailInfo;

/**
* CAlwaysOnlineImap4FolderPopulate
*
* 
*/
class CAlwaysOnlineImap4FolderPopulate : 
    public CMsvOperation
    {
    public:
        /**
        *
        */
        static CAlwaysOnlineImap4FolderPopulate* NewL(
            CMsvSession& aMsvSession,
            CImap4ClientMtm& aImap4ClientMtm,
            TMsvId aServiceId,
            TMsvId aInboxId,
            TImImap4GetPartialMailInfo& aPartialInfo,
            const CMsvEntrySelection* aSelection, // folder ids
            TRequestStatus& aObserverRequestStatus );

    private:
        /**
        *
        */
	    CAlwaysOnlineImap4FolderPopulate(
            CMsvSession& aMsvSession, 
            CImap4ClientMtm& aImap4ClientMtm,
            TMsvId aServiceId, 
            TMsvId aInboxId,
            TImImap4GetPartialMailInfo& aPartialInfo,
            TRequestStatus& aObserverRequestStatus );
    public:
        /**
        *
        */
	    virtual ~CAlwaysOnlineImap4FolderPopulate();

    public:
        /**
        * From CMsvOperation
        */
        const TDesC8& ProgressL();


    private:
        /**
        * Constructor.
        */
        void ConstructL( const CMsvEntrySelection* aSelection );
        /**
        * From CActive
        */
        void DoCancel();
        /**
        * From CActive
        */
        void RunL();

        /**
        *
        */
        void StartNextFolderPopulateOperation();

        /**
        *
        */
        void DoNewFolderPopulateOperationL();

        /**
        *
        */
        void SelectInbox();
        void DoSelectInboxL();

        TBool HasFolderNewMailL( TMsvId aFolderId );


    private:
        CMsvOperation*          iOperation;
        TMsvId                  iServiceId;
        CMsvEntrySelection*     iFolderSelection;
        TImImap4GetPartialMailInfo iPartialInfo;
        CImap4ClientMtm&        iImap4ClientMtm;

        TMsvId                  iInboxId;

    };



#endif
//EOF
