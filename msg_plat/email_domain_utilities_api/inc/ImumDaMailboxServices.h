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
* Description:  Interface class of settings data collection
 *
*/

 
 
#ifndef M_IMUMDAMAILBOXSERVICES_H
#define M_IMUMDAMAILBOXSERVICES_H 

#include <msvstd.h>                     // TMsvId

class CImumDaSettingsData;

/**
 * Provides services to create mailboxes
 *
 * This class provides a services to control the mailboxes in the system,
 * by allowing to create them and set default mailbox.
 *
 * @lib imumda.lib
 * @since S60 v3.2
 * @see from ImumDaSettingsKeys.h enum TConnectionKeySet for setting keys
 * @see from ImumDaErrorCodes.h enum TImumDaErrorCode for error codes
 * @see from ImumDaSettingsData.h class CImumDaSettingsData for data
 */
NONSHARABLE_CLASS( MImumDaMailboxServices )
    {
    public:
                
        /**
         * Creates and prepares mailbox settings data object.
         * *** NOTE: Client is responsible to destroy the object after usage ***
         *
         * @since S60 v3.2         
         * @param aProtocol, Any receiving protocol: Imap4 or Pop3.
         * @return Pointer to mailbox settings dataobject.
         * @leave EParamNotReceivingProtocol, If protocol receiving one.
         */        
        virtual CImumDaSettingsData* CreateSettingsDataL( 
            const TUid& aProtocol ) const = 0;

        /**
         * Validates the data and creates working mailbox to system.
         *
         * @since S60 v3.2    
         * @param aSettingsData, Settings data object containing mailbox settings.         
         */        
        virtual TMsvId CreateMailboxL( 
            CImumDaSettingsData& aSettingsData ) const = 0;

        /**
         * Sets the system's default mailbox (sending).
         *
         * @since S60 v3.2
         * @param aMailboxId, Id of the mailbox.
         * @leave EEntryNotFound, If the entry for the id can't be found.
         * @leave EEntryNotMailbox, If the id does not belong to mailbox.
         */        
        virtual void SetDefaultMailboxL( const TMsvId aMailboxId ) const = 0;
    };

#endif // M_IMUMDAMAILBOXSERVICES_H
