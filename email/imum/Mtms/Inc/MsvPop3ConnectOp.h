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
*       Encapsulates connection validation
*
*/

#ifndef __MSVPOP3CONNECTOP_H__
#define __MSVPOP3CONNECTOP_H__

#include <ImumInternalApi.h>        // CImumInternalApi
#include <pop3set.h>
#include <MsvPrgReporter.h>
#include "ImumOnlineOperation.h"

class CImumInSettingsData;
class CMsvEmailConnectionProgressProvider;

/**
* Connect operation.
* Encapsulates connection validation.
*/
class CMsvPop3ConnectOp :
    public CImumOnlineOperation
    {
    public:

        /**
        *
        */
        static CMsvPop3ConnectOp* NewL(
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aService,
            TInt aPopulateLimit );

        /**
        *
        */
        virtual ~CMsvPop3ConnectOp();

        /**
        *
        */
        virtual const TDesC8& ProgressL();

        /**
        *
        */
        virtual const TDesC8& GetErrorProgressL(TInt aError);

        /**
        *
        */
        inline TBool Connected() const;

    protected:

        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */
        virtual void DoRunL();
    private:

        /**
        *
        */
        CMsvPop3ConnectOp(
            CImumInternalApi* aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter,
            TMsvId aServiceId,
            TInt aPopulateLimit);

        /**
        *
        */
        void ConstructL();

        /**
        *
        */
        void DoConnectL();

        /**
        *
        *
        */
        void DoPopulateL();
        /**
        *
        */
        TBool ValidateL( CImumInSettingsData& aSettings );

        /**
        *
        */
        inline void SetFlag(TUint32 aFlag);

        /**
        *
        */
        inline void UnsetFlag(TUint32 aFlag);

        /**
        *
        */
        inline TBool FlagIsSet(TUint32 aFlag) const;

    private: // Data
        enum TFlags {
            EUnsetPassword = 0x01,
            EInvalidDetails = 0x02,
            EDoConnect = 0x04,
            EMaxNumberOfLoginTries = 0x08,
            EIapWasInvalid = 0x10,
            EDoingPopulate = 0x20 };

        TUint32     iFlags;
        CMsvEntry*  iEntry;
        CMsvEmailConnectionProgressProvider* iProgProvider;
        TPckgBuf<TPop3Progress> iProgress;
        TInt        iLoginRetryCounter;

        TInt        iPopulateLimit;
        CMsvEntrySelection* iSelection;
        TBool iPopulated;
        // Own: Mailbox API
        CImumInternalApi* iMboxApi;
    };

inline TBool CMsvPop3ConnectOp::Connected() const
    {
    return iEntry->Entry().Connected();
    }

inline void CMsvPop3ConnectOp::SetFlag(TUint32 aFlag)
    {
    iFlags |= aFlag;
    }

inline void CMsvPop3ConnectOp::UnsetFlag(TUint32 aFlag)
    {
    iFlags &= (~aFlag);
    }

inline TBool CMsvPop3ConnectOp::FlagIsSet(TUint32 aFlag) const
    {
    return (iFlags & aFlag);
    }


#endif
