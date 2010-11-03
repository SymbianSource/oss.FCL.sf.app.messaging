/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ImumInMailboxServicesImpl.h
*
*/


#ifndef CIMUMINMAILBOXSERVICESIMPL_H
#define CIMUMINMAILBOXSERVICESIMPL_H

// INCLUDES
#include <msvstd.h>

#include "ImumInMailboxServices.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CImumInternalApiImpl;
class CImumMboxDefaultDataCtrl;
class CImumMboxDefaultData;

// CLASS DECLARATION

/**
*
*  @lib
*  @since S60 v3.2
*/
class CImumInMailboxServicesImpl : public CBase, public MImumInMailboxServices
    {
    public:  // Constructors and destructor

        /**
         * Creates object from CImumInMailboxServicesImpl and leaves it to cleanup stack
         *
         * @since S60 v3.2
         * @return, Constructed object
         */
        static CImumInMailboxServicesImpl* NewLC( CImumInternalApiImpl& aInternalApi );

        /**
         * Create object from CImumInMailboxServicesImpl
         *
         * @since S60 v3.2
         * @return, Constructed object
         */
        static CImumInMailboxServicesImpl* NewL( CImumInternalApiImpl& aInternalApi );

        /**
         * Destructor
         *
         * @since S60 v3.2
         */
        virtual ~CImumInMailboxServicesImpl();

    public: // New functions

        /**
         * Fills settings data object and fills the mandatory data.
         *
         * @since S60 v3.2
         * @param aSettingsData Settings data object to be filled
         * @param aProtocol Receiving protocol of the mailbox (Imap4/Pop3)
         * @param aEmailAddress The email address
         * @param aIncomingServer Server to be used to fetch the mail
         * @param aOutgoingServer Server to be used to send the mail
         * @param aAccessPoint Used accesspoint id or 0 for default connection
         * @param aMailboxName The name of the mailbox in UI
         */
        void FillCompulsorySettingsL(
            CImumInSettingsData& aSettingsData,
            const TUid& aProtocol,
            const TDesC& aEmailAddress,
            const TDesC& aIncomingServer,
            const TDesC& aOutgoingServer,
            const TInt aAccessPoint,
            const TDesC& aMailboxName ) const;

        /**
         * Returns the default data object.
         *
         * @since S60 v3.2
         * @return Pointer to default data object
         */
        const CImumMboxDefaultData* DefaultDataObjectL();

    public: // Functions from base classes

        /**
         * Creates an empty settings data object.
         *
         * @since S60 v3.2
         * @param aProtocol Protocol for initialization (Imap4/Pop3)
         * @return Settings data object and responsibility to destroy it.
         */
        CImumInSettingsData* CreateSettingsDataL( const TUid& aProtocol ) const;

        /**
         * Creates and fills settings data object and fills the mandatory data.
         *
         * @since S60 v3.2
         * @param aSettingsData Settings data object to be filled
         * @param aProtocol Receiving protocol of the mailbox (Imap4/Pop3)
         * @param aEmailAddress The email address
         * @param aIncomingServer Server to be used to fetch the mail
         * @param aOutgoingServer Server to be used to send the mail
         * @param aAccessPoint Used accesspoint id or 0 for default connection
         * @param aMailboxName The name of the mailbox in UI
         * @return Settings data object and responsibility to destroy it.
         */
        CImumInSettingsData* CreateSettingsDataL(
            const TUid& aProtocol,
            const TDesC& aEmailAddress,
            const TDesC& aIncomingServer,
            const TDesC& aOutgoingServer,
            const TInt aAccessPoint,
            const TDesC& aMailboxName ) const;

        /**
         * Load the settings data of requested mailbox.
         *
         * @since S60 v3.2
         * @param aMailboxId Id of the mailbox entry
         * @return Settings data object and responsibility to destroy it.
         */
        CImumInSettingsData* LoadMailboxSettingsL( const TMsvId aMailboxId ) const;

        /**
         * Saves the settings data.
         *
         * @since S60 v3.2
         * @param aSettingsData Settings data to be stored.
         */
        void SaveMailboxSettingsL( CImumInSettingsData& aSettingsData ) const;

        /**
         * Verifies and creates the mailbox based on the settings data.
         *
         * @since S60 v3.2
         * @param aSettingsData Settings data to be used.
         * @return Id of the new mailbox.
         */
        TMsvId CreateMailboxL( CImumInSettingsData& aSettingsData ) const;

        /**
         * Deletes the mailbox from the system.
         *
         * @since S60 v3.2
         * @param Id of the mailbox entry.
         */
        CMsvOperation* RemoveMailboxL(
        	const TMsvId aMailboxId,
        	TRequestStatus& aStatus ) const;

        /**
         * Sets the default sending mailbox.
         *
         * Provided mailbox id is used to set the default sending mailbox. In
         * case the mailbox id is not correct, valid mailbox is searched from the
         * system.
         *
         * @since S60 v3.2
         * @param aMailboxId Id of the mailbox entry.
         */
        void SetDefaultMailboxL( const TMsvId aMailboxId ) const;

        /**
         * Set last mailbox update time
         *
         * @since S60 v3.2
         * @param aMailboxId Mailbox ID
         * @param aSuccess Flag if mailbox update was successfull
         * @param aTime Mailbox update time
         */
        void SetLastUpdateInfoL( TMsvId aMailboxId, TBool aSuccess,
            const TTime& aTime ) const;

    protected:  // Constructors

        /**
         * Default constructor for classCImumInMailboxServicesImpl
         *
         * @since S60 v3.2
         * @return, Constructed object
         */
        CImumInMailboxServicesImpl( CImumInternalApiImpl& aInternalApi );

        /**
         * Symbian 2-phase constructor
         *
         * @since S60 v3.2
         */
        void ConstructL();

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions

        /**
         * Raises leave with error in release mode and panic in debug mode.
         *
         * @since S60 v3.2
         * @param aErrorCode The actual error code
         */
        void ServiceExceptionL( const TInt aErrorCode ) const;

        /**
         * Validates the settings data object.
         *
         * @since S60 v3.2
         * @param aSettingsData The object to be verified.
         * @leave Any error code defined in ImumDaErrorCodes.h
         * @see ImumDaErrorCodes.h
         */
        void ValidateSettingsL(
            CImumInSettingsData& aSettingsData ) const;

    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data

        // Reference to internal api
        CImumInternalApiImpl&   iInternalApi;
        // Owned: Default data
        CImumMboxDefaultData*   iDefaultData;
    };

#endif //  CIMUMINMAILBOXSERVICESIMPL_H
