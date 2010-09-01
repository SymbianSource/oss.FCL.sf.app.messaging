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
* Description:  ImumInMailboxServices.h
*
*/


#ifndef M_IMUMINMAILBOXSERVICES_H
#define M_IMUMINMAILBOXSERVICES_H

// INCLUDES
#include <msvstd.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CImumInSettingsData;
class CMsvOperation;

// CLASS DECLARATION

/**
*
*  @lib
*  @since S60 3.2
*/
class MImumInMailboxServices
    {
    public:
        virtual CImumInSettingsData* CreateSettingsDataL( const TUid& aProtocol ) const = 0;
        virtual CImumInSettingsData* CreateSettingsDataL(
            const TUid& aProtocol,
            const TDesC& aEmailAddress,
            const TDesC& aIncomingServer,
            const TDesC& aOutgoingServer,
            const TInt aAccessPoint,
            const TDesC& aMailboxName ) const = 0;
        virtual CImumInSettingsData* LoadMailboxSettingsL( const TMsvId aMailboxId ) const = 0;
        virtual void SaveMailboxSettingsL( CImumInSettingsData& aSettingsData ) const = 0;
        virtual TMsvId CreateMailboxL( CImumInSettingsData& aSettingsData ) const = 0;
        virtual CMsvOperation* RemoveMailboxL(
        	const TMsvId aMailboxId,
        	TRequestStatus& aStatus ) const = 0;
        virtual void SetDefaultMailboxL( const TMsvId aMailboxId ) const = 0;
        virtual void SetLastUpdateInfoL( TMsvId aMailboxId, TBool aSuccess,
            const TTime& aTime ) const = 0;
    };

#endif //  M_IMUMINMAILBOXSERVICES_H

// End of File
