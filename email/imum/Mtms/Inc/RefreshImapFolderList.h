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
*       folder refresh operation
*
*/


#if !defined(__REFRESHIMAPFOLDERLIST_H__)
#define __REFRESHIMAPFOLDERLIST_H__

//  Messaging includes
#include <imapset.h>
#include <ImumInternalApi.h>        // CImumInternalApi

//  Specific includes
#include "ImumOnlineOperation.h"

//  Forward declarations
class MMsvProgressReporter;

/**
* Superoperation which wraps a UI around going online, updating
* an IMAP service's folder list and disconnecting again.
*/
class CImapConnectAndRefreshFolderList : public CImumOnlineOperation
    {
public:

    /**
    * NewL()
    * @param CMsvSession&
    * @param TInt
    * @param TRequestStatus&
    * @param TMsvId
    * @param MMsvProgressReporter&
    * @return CImapConnectAndRefreshFolderList*
    */
    static CImapConnectAndRefreshFolderList* NewL(
        CImumInternalApi& aMailboxApi, TInt aPriority, TRequestStatus& aObserverRequestStatus,
        TMsvId aService, MMsvProgressReporter& aProgressReporter);

    /**
    * Destructor
    * ~CImapConnectAndRefreshFolderList()
    */
    virtual ~CImapConnectAndRefreshFolderList();

    /**
    * From CImumOnlineOperation
    * GetErrorProgressL()
    * @param TInt error code
    * @return TDesC8&, progress
    */
    virtual const TDesC8& GetErrorProgressL(TInt aError);       // For reporting DoRunL leaves.

protected:


    /**
    * From CImumOnlineOperation
    * DoRunL()
    */
    virtual void DoRunL();

private:

    enum TImumFolderRefreshStates
        {
        EConnecting,
        ERefreshing,
        EDisconnecting,
        ECompleted
        };

    /**
    * C++ constructor
    * CImapConnectAndRefreshFolderList()
    * @param CMsvSession&
    * @param TInt
    * @param TRequestStatus&
    * @param TMsvId
    * @param MMsvProgressReporter&
    */
    CImapConnectAndRefreshFolderList(CImumInternalApi& aMailboxApi, TInt aPriority,
        TRequestStatus& aObserverRequestStatus, TMsvId aService,
        MMsvProgressReporter& aProgressReporter);

    /**
    * ConstructL()
    * @param TUid
    */
    void ConstructL(TUid aMtm);

    //data
private:
    TPckgBuf<TImap4CompoundProgress>    iProgressBuf;
    TInt                                iState;
    };

#endif
