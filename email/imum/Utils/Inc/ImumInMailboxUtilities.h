/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ImumInMailboxUtilities.h
*
*/
#ifndef M_IMUMINMAILBOXUTILITIES_H
#define M_IMUMINMAILBOXUTILITIES_H

#include <msvstd.h>             // TMsvId

//##ModelId=451CAF630197
class MImumInMailboxUtilities
    {
    public:

        /** */
        enum TImumInMboxRequest
            {
            ERequestCurrent = 0,
            ERequestSending,
            ERequestReceiving
            };

        /** */
        enum TImumInMboxAlwaysOnlineState
            {
            /** Always Online is active */
            EFlagTurnedOn           = 0x01,
            /** Always Online is waiting for connecting */
            EFlagWaitingToConnect   = 0x02
            };

        /** */
        typedef RArray<TMsvEntry> RMsvEntryArray;

    public:
        virtual TMsvId DefaultMailboxId( const TBool aForceGet = EFalse ) const = 0;

        virtual TBool IsMailMtm( const TUid& aMtm, const TBool& aAllowExtended = EFalse ) const = 0;

        virtual TBool IsMailbox( const TMsvId aMailboxId ) const = 0;

        virtual TBool IsMailbox( const TMsvEntry& aMailbox ) const = 0;

        virtual TMsvEntry GetMailboxEntryL( const TMsvId aMailboxId, const TImumInMboxRequest& aType = ERequestCurrent, const TBool aServiceCheck = EFalse ) const = 0;

        virtual const TUid& GetMailboxEntriesL( const TMsvId aMailboxId, RMsvEntryArray& aEntries, const TBool aResetArray = ETrue ) const = 0;

        virtual TBool IsInbox( const TMsvEntry& aFolderEntry ) const = 0;

        virtual TBool HasWlanConnectionL( const TMsvId aMailboxId ) const = 0;

        virtual TBool HasSubscribedFoldersL( const TMsvId aMailboxId ) const = 0;

        virtual void QueryAlwaysOnlineStateL( const TMsvId aMailboxId, TInt64& aAlwaysOnlineStatus ) const = 0;

        virtual void NextAlwaysOnlineIntervalL( const TMsvId aMailboxId, TInt64& aAlwaysOnlineStatus, TTimeIntervalSeconds& aSeconds ) const = 0;
    };



#endif /* M_IMUMINMAILBOXUTILITIES_H */
