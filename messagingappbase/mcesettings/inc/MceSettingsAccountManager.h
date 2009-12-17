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
* Description:  
*     Mixed class to offer different account functions.
*
*/



#ifndef __MCESETTINGSACCOUNTMANAGER_H__
#define __MCESETTINGSACCOUNTMANAGER_H__

//  INCLUDES

#include <msvreg.h>     // THumanReadableName


// FORWARD DECLARATIONS
class CUidNameArray;
class CMsvSession;

// CLASS DECLARATION

/**
*  Mixed class to offer different account functions.
*  Implemented by Mce application or some other class...
*/
class MMceSettingsAccountManager
    {
    public:
        /**
        * Create new account.
        * @param aType type of the account.
        * @param aOldServiceId id of the service where old settings are copied (this is for 'create from existing')
        * @return ETrue if account created, EFalse otherwise
        */
        virtual TBool CreateNewAccountL( TUid aType, TMsvId aOldServiceId ) = 0;

        /**
        * Edit account.
        * @param aId id of the account to be edited.
        */
        virtual void EditAccountL( TMsvId aId ) = 0;

        /**
        * Returns array of accounts of desired type.
        * Callers responsibility to destroy array!
        * @param aType type of the account.
        */
        virtual CUidNameArray* MtmAccountsL( TUid aType ) = 0;

        /**
        * Can create create new account of aMtm.
        * @param aMtm Mtm uid.
        * @return EFalse if new accounts cannot be created for mtm aMtm.
        */
        virtual TBool CanCreateNewAccountL( TUid aMtm ) = 0;

        /**
        * Delete account.
        * @param aId id of the account to be deleted.
        */
        virtual void DeleteAccountL( TMsvId aId ) = 0;

        /**
         * Returns session used.
         * @return reference to object of CMsvSession class.
         */
        virtual CMsvSession& Session() = 0;

        /**
         * Returns ETrue if aMtm is installed in the device.
         * @param aMtm: Mtm uid to be checked.
         * @return EFalse if aMtm is not installed, ETrue if it is installed.
         */
        virtual TBool IsPresent( TUid aMtm ) const = 0;

        /**
         * Returns human readable name of mtm.
         * @param aMtm: Mtm uid
         * @return human readable name of mtm.
         */
        virtual THumanReadableName MtmName( TUid aMtm ) const = 0;
        
        
        /**
        * Called by General settings dialog when message store
        *   change is started / ended. 
        * @param aChangeEnded: EFalse when change started
        *                      ETrue when change ended
        * @since S60 3.2
        */
        virtual void SetChangeMessageStore( TBool aChangeEnded ) = 0;
    };


#endif

// End of file
