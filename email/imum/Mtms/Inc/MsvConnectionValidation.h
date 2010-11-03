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
*       Static functions for validating account settings prior to connection
*
*/



#ifndef __MSVCONNECTIONVALIDATION_H__
#define __MSVCONNECTIONVALIDATION_H__

#include <msvapi.h>
#include <eikdialg.h>
#include <iapprefs.h>
#include <ImumInternalApi.h>

#include "Imas.hrh"     // For KImasPasswordLength, KImasImailServiceNameLength
#include "ComDbUtl.h"

// CONSTANTS
enum TIMASPasswordSave
    {
    EIMASPasswordNoSave = 0,
    EIMASPasswordTempSave,
    EIMASPasswordPermanentSave,
    EIMASPasswordCancelSave // User cancelled username/password query
    };

// FORWARD DECLEARATIONS

class CMsvCommDbUtilities;
class CImBaseEmailSettings;
class MMsvProgressReporter;

class TMsvLoginData
    {
public:
    TBuf8<KImasLoginLength> iUsername;
    TBuf8<KImasPasswordLength> iPassword;
    TBool iIsPasswordTemporary;
    };

/**
* MsvConnectionValidation
*
* Static functions for validating account settings prior to connection.
*/
class MsvConnectionValidation
    {
    public:

        /**
        * Queries the login information and stores them to settings
        * @since S60 3.0
        */
        static TIMASPasswordSave RunAndHandleLoginDialogL(
            TDes8& aUserName,
            TDes8& aPassword,
            TBool& aPasswordTemporary );

        /**
        * Check that there is a password specified for aRcvServiceId, and if not, set 'aNoPassword',
        * prompt user to enter one, and store it in the settings. Returns EFalse if user cancels.
        * @param aMailboxApi: Mailbox api
        * @param aServiceId: service id to be connected
        * @param aLoginData: Username and password
        * @param aLoginRetryCounter: incremented by 1 if login dialog is displayed
        */
        static TBool CheckAndPromptForPasswordL(
            CImumInternalApi& aMailboxApi,
            TMsvId aServiceId,
            TMsvLoginData aLoginData,
            TInt& aLoginRetryCounter );


        /**
        * Prompt the user to check the login details for aRcvServiceId, allowing them to change the
        * details. Returns EFalse if user cancels.
        */
        static TBool ShowLoginDetailsRejectionDlgL(
            CImumInternalApi& aMailboxApi, 
            TMsvId aRcvServiceId);

        /**
        * Check that the IAP specified for aSendServiceId is valid
        * If current iap is not valid removes all the iaps from the service and returns EFalse.
        * @return ETrue if services iap found from commdb, EFalse if not.
        */
        static TBool CheckForIapL(
            CMsvSession& aSession,
            TMsvId aServiceId );

        /**
        * Get the name of the service with Id aServiceId.
        */
        static void GetServiceNameL(CMsvSession& aSession, TMsvId aServiceId, TDes& aServiceName);

        /**
        * Get the name of the IAP in use for the aMtmId.
        */
        static void GetServiceIapNameL(CImumInSettingsData& aAccountSettings,
            TUid aMtmUid, TDes& aIapName);

    private:

        /**
        *
        */
        static CImBaseEmailSettings* InstantiateSettingsObjectLC(const TMsvEntry& aEntry, TUid& aMtmType);

        /**
        *
        */
        static TInt RunUsernameAndPasswordDialogL( TDes& aUserName, TDes& aPassword );

        /**
        *
        * @since S60 3.0
        */
        static TBool IsAlwaysAskSet(
            const CImIAPPreferences& iapPref );
    };



#endif // __MSVCONNECTIONVALIDATION_H__
