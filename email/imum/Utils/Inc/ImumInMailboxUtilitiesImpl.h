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
* Description:  ImumInMailboxUtilitiesImpl.h
*
*/


#ifndef C_IMUMINMAILBOXUTILITIESIMPL_H
#define C_IMUMINMAILBOXUTILITIESIMPL_H


#include "ImumInMailboxUtilities.h"

class CMsvSession;
class CMtmUiDataRegistry;
class CImumInternalApiImpl;

/**
 *
 * @since S60 3.2
 */
class CImumInMailboxUtilitiesImpl:  public CBase,
                                    public MImumInMailboxUtilities
    {
    public:
        static CImumInMailboxUtilitiesImpl* NewL( CImumInternalApiImpl& aMailboxApi );
        static CImumInMailboxUtilitiesImpl* NewLC( CImumInternalApiImpl& aMailboxApi );
        virtual ~CImumInMailboxUtilitiesImpl();

// From base class MImumInMailboxUtilities
        TMsvId DefaultMailboxId( const TBool aForceGet = EFalse ) const;

        TBool IsMailMtm( const TUid& aMtm, const TBool& aAllowExtended = EFalse ) const;

        TBool IsMailbox( const TMsvId aMailboxId ) const;

        TBool IsMailbox( const TMsvEntry& aMailbox ) const;

        TMsvEntry GetMailboxEntryL( const TMsvId aMailboxId, const TImumInMboxRequest& aType = ERequestCurrent, const TBool aServiceCheck = ETrue ) const;

        const TUid& GetMailboxEntriesL( const TMsvId aMailboxId, RMsvEntryArray& aEntries, const TBool aResetArray = ETrue ) const;

        TBool IsInbox( const TMsvEntry& aFolderEntry ) const;

        TBool HasWlanConnectionL( const TMsvId aMailboxId ) const;

        TBool HasSubscribedFoldersL( const TMsvId aMailboxId ) const;

        /**
         * Determines whether the Always Online is on or not.
         *
         * @since S60 v3.2
         * @param
         * @return
         * @leave &(leavedesc)s
         */
        virtual void QueryAlwaysOnlineStateL(
            const TMsvId aMailboxId,
            TInt64& aAlwaysOnlineStatus ) const;

        /**
         * Calculates seconds to next interval.
         *
         * @since S60 v3.2
         * @param aMailboxId, Id of the mailbox to be checked
         * @param aOnlineState, State, what should after after seconds have expired
         * @param aSeconds, Seconds to next interval
         * @leave &(leavedesc)s
         */
        virtual void NextAlwaysOnlineIntervalL(
            const TMsvId aMailboxId,
            TInt64& aAlwaysOnlineStatus,
            TTimeIntervalSeconds& aSeconds ) const;

    private:
        void ConstructL();
        CImumInMailboxUtilitiesImpl( CImumInternalApiImpl& aMailboxApi );

        TMsvId DefaultMailboxIdL( const TBool aForceGet = EFalse ) const;

// Data
        // Message Server Session
        CImumInternalApiImpl& iMailboxApi;
        //
        CMtmUiDataRegistry* iMtmUiDataRegistry;
    };

#endif /* C_IMUMINMAILBOXUTILITIESIMPL_H */
