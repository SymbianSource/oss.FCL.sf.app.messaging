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
* Description:  ImumInternalApiImpl.h
*
*/


#ifndef __CIMBEMAILFACTORY_H__
#define __CIMBEMAILFACTORY_H__

// INCLUDES
#include <e32base.h>
#include <msvapi.h>                     // CMsvSession
#include <ImumInternalApi.h>
#include <cemailaccounts.h>             // CEmailAccounts

#include "ImumInternalApiImpl.h"
#include "ImumInHealthServicesImpl.h"
#include "ImumMboxSettingsCtrl.h"
#include "ImumMboxManager.h"
#include <ConeResLoader.h>      // RConeResourceLoader

// CONSTANTS
// MACROS
// FORWARD DECLARATIONS
class CImumInMailboxUtilitiesImpl;
class CImumInMailboxServicesImpl;
class CMuiuFlags;
class CMsvCommDbUtilities;

// LOCAL FUNCTION PROTOTYPES
// DATA TYPES

/*
* Center point of email API
* @lib ImumUtils.lib
* @since S60 v3.2
*/
class CImumInternalApiImpl : public CImumInternalApi, public MMsvSessionObserver
{
    private: // Enums

        enum TInternalApiFlags
            {
            ESessionOwned = 0,
            EObserverSet,
            EServicesReady,
            EHasUi
            };

    public:  // Constructors and destructor

        /**
         *
         * @since S60 v3.2
         */
        static CImumInternalApiImpl* NewL(
            CMsvSession* aMsvSession = NULL );

        /**
         *
         * @since S60 v3.2
         */
        static CImumInternalApiImpl* NewLC(
            CMsvSession* aMsvSession = NULL );

        /**
         *
         * @since S60 v3.2
         */
        virtual ~CImumInternalApiImpl();

    public: // New functions

        /**
         *
         * @since S60 v3.2
         */
        CEmailAccounts& EmailAccountsL();

        /**
         *
         * @since S60 v3.2
         */
        CImumInHealthServicesImpl& MailboxDoctorL();

        /**
         *
         * @since S60 v3.2
         */
        CImumMboxManager& SettingsManagerL();

        /**
         *
         * @since S60 v3.2
         */
        CImumMboxSettingsCtrl& SettingsStorerL();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         * @leave &(leavedesc)s
         */
        CImumInMailboxServicesImpl& MailboxServicesImplL();

        /**
         *
         *
         * @since S60 v3.2
         * @param
         * @return
         */
        const CMuiuFlags& Flags();

        /**
         *
         *
         * @since S60 v5.0
         * @param
         * @return
         * @leave
         */
        CMsvCommDbUtilities& CommDbUtilsL();


        /**
         * Throws an exception. Leaves in release mode, panic in debug.
         *
         * @since S60 v3.2
         * @param aErrorCode, error code to be used to indicate exception
         * @leave aErrorCode
         */
        void ServiceExceptionL(
            const TInt aErrorCode ) const;

    public: // Functions from base classes

// From CImumInternalApi

        virtual CMsvSession& MsvSession();

        /**
         * Interface to handle information regarding the
         *
         * healthiness of the mailboxes
         * @since S60 v3.2
         * @return Mailbox Doctor Interface
         */
        virtual const MImumInHealthServices& HealthServicesL();
        virtual const MImumInHealthServices& HealthApiL();

        /**
         * Interface to mailbox utilities
         *
         * @since S60 3.2
         * @return Reference to utilities class
         */
        virtual const MImumInMailboxUtilities& MailboxUtilitiesL();

        virtual const MImumInMailboxServices& MailboxServicesL();

        /**
         * Function for checking mailbox flags and show information note.
         * If resource parameters are set blank no note will be displayed.
         *
         * @since S60 v3.2
         * @param Flag from the list TEmailFeatures.
         * @param aFeatureOnResource resource to be displayed if feature is on.
         * @param aFeatureOffResource resource to be displayed if feature is off.
         * @return ETrue if asked flag is active.
         */
        virtual TBool IsEmailFeatureSupportedL(
            const TEmailFeatures aFlag,
            const TUint aFeatureOnResource = 0,
            const TUint aFeatureOffResource = 0 );

// MMsvSessionObserver

        void HandleSessionEventL(
            TMsvSessionEvent aEvent,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3 );

    protected:  // Constructors

        /**
         *
         * @since S60 v3.2
         */
        void ConstructL();

        /**
         *
         * @since S60 v3.2
         */
        CImumInternalApiImpl( CMsvSession* aMsvSession = NULL );

    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes

    private:  // New virtual functions
    private:  // New functions

        /**
         * Resets the API objects and sets the default mailbox.
         *
         * @since S60 v3.2
         */
        void ReloadL();

        /**
         * Destroys objects and Message Server session
         *
         * @since S60 v3.2
         */
        void Reset();

        /**
         * Create session object to Message Server catching the leaves
         *
         * @since S60 v3.2
         * @return Object of Message Server session
         */
        CMsvSession* CreateMsvSession();

        /**
         * Create session object to Message Server
         *
         * @since S60 v3.2
         * @return Object of Message Server session
         * @leave KErrNoMemory when failing to create the object
         */
        CMsvSession* DoCreateMsvSessionL();

    private:  // Functions from base classes

    public:     // Data
    protected:  // Data
    private:    // Data

        // Owned;
        CMuiuFlags* iFlags;
        // Ownership depends on parameter
        CMsvSession* iMsvSession;
        // Owned:
        CImumMboxManager* iMailboxManager;
        // Owned:
        CImumInHealthServicesImpl* iHealthServices;
        // Owned:
        CImumMboxSettingsCtrl* iMboxSettingsCtrl;
        // Owned:
        CEmailAccounts* iAccounts;
        // Owned:
        CImumInMailboxUtilitiesImpl* iMailboxUtilities;
        // Owned:
        CImumInMailboxServicesImpl* iMailboxServices;
        // Owned: Object to load the resources
        RConeResourceLoader*         iResourceLoader;
        // Owned: Pointer to CommDb utils object
        CMsvCommDbUtilities*    iCommDbUtils;
};

#endif /* __CIMBEMAILFACTORY_H__ */
