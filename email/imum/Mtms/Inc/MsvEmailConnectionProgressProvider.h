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
* Description: MsvEmailConnectionProgressProvider.h
*
*
*/


// Email Connection Related Utilities

#ifndef __MSVEMAILCONNECTIONPROGRESSPROVIDER_H__
#define __MSVEMAILCONNECTIONPROGRESSPROVIDER_H__

#include <msvapi.h>
#include <eikdialg.h>
#include <ImumInternalApi.h>        // CImumInternalApi
#include "Imas.hrh"     // For KImasPasswordLength, KImasImailServiceNameLength


//
// TMsvEmailConnectionProgress
//
// Email connection progress struct.
// Provides additional state information on the connection, beyond that provided
// by messaging GT components.
//
class TMsvEmailConnectionProgress
    {
    public:
        enum TConnectState
            {
            EInitialising,
            EConnectNetwork,
            EConnectMailbox
            };
    public:
        TInt iType;
        TInt iErrorCode;
        TMsvId iService;
        TConnectState iState;
        TBuf<50> iName;
    };


//
// CMsvEmailConnectionProgressProvider
//
// Provides progress for a connection.
// Intended to be owned by a connection operation, such that it is destroyed, when no longer needed.
//
class CMsvEmailConnectionProgressProvider : public CBase
    {
    public:
        static CMsvEmailConnectionProgressProvider* NewL(
            const TDesC& aIapName, TMsvId aService);
        virtual ~CMsvEmailConnectionProgressProvider();
        const TDesC8& GetConnectionProgressL(TInt aErrorCode=KErrNone);
    private:
        CMsvEmailConnectionProgressProvider(const TDesC& aIapName,
            TMsvId aService);
    private:
//      RGenericAgent iGenericAgent;
        TPckgBuf<TMsvEmailConnectionProgress> iProgressBuf;
    };


#endif // __MSVEMAILCONNECTIONPROGRESSPROVIDER_H__
