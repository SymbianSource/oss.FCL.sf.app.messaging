/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*  Settings observer   
*
*/




#ifndef __MCESETTINGSSESSIONOBSERVER_H__
#define __MCESETTINGSSESSIONOBSERVER_H__

//  INCLUDES

#include <MuiuMsvSingleOpWatcher.h>
#include "MceSettingsAccountManager.h"

// CLASS DECLARATION


// CLASS DECLARATION

/**
*  Settings observer
*  
*/

class CMceSettingsSessionObserver : 
    public CBase, 
    public MMsvSessionObserver,
    public MMsvSingleOpWatcher,
    public MMceSettingsAccountManager
    {
    public:

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMceSettingsSessionObserver* NewL( CMsvSession* aSession );

        /**
         * Destructor
         */
        IMPORT_C virtual ~CMceSettingsSessionObserver();

    public:
        
        /**
        * from MMsvSessionObserver
        */
	    virtual void HandleSessionEventL( TMsvSessionEvent aEvent, 
	                                                        TAny* aArg1, 
	                                                        TAny* aArg2, 
	                                                        TAny* aArg3 );

        /**
        * from MMsvSingleOpWatcher
        */
        virtual void OpCompleted( CMsvSingleOpWatcher& aOpWatcher, TInt aCompletionCode );

        /**
        * From MMceSettingsAccountManager
        */
        IMPORT_C TBool CreateNewAccountL(
            TUid aMessageType,
            TMsvId aOldServiceId );

        /**
        * From MMceSettingsAccountManager
        */
        IMPORT_C void EditAccountL( TMsvId aId );

        /**
        * From MMceSettingsAccountManager
        */
        IMPORT_C CUidNameArray* MtmAccountsL( TUid aType );

        /**
        * From MMceSettingsAccountManager
        */
        IMPORT_C TBool CanCreateNewAccountL( TUid aMtm );

        /**
        * From MMceSettingsAccountManager
        */
        IMPORT_C void DeleteAccountL( TMsvId aId );

        /**
        * From MMceSettingsAccountManager
        */
        IMPORT_C CMsvSession& Session();

        /**
        * From MMceSettingsAccountManager
        */
        IMPORT_C TBool IsPresent( TUid aMtm ) const;
       
        /**
        * From MMceAccountManager
        */
        IMPORT_C THumanReadableName MtmName( TUid aType ) const;

        /**
        * From MMceAccountManager
        */
        IMPORT_C void SetChangeMessageStore( TBool aChangeEnded );

    public:

        /**
        * Returns mtm store pointer
        */
        IMPORT_C CMtmStore* MtmStore() const;

        /**
        * Returns ui data registry pointer
        */
        IMPORT_C CMtmUiDataRegistry* UiRegistry() const;

    private:

        /**
         * Default constructor
         */
        CMceSettingsSessionObserver( CMsvSession* aSession );

        /**
         * Symbian OS constructor
         */
        void ConstructL();

        /**
        * Check if free disk space goes under critical level, and leaves if it does
        */
        void LeaveIfDiskSpaceUnderCriticalLevelL( TInt aBytesToWrite = 0 ) const;

        /**
        * Display possible error when operation is compeleted
        */
        void DoOperationCompletedL(
            CMsvOperation* aOperation, 
            TInt aCompletionCode );

        /**
        * Cancels RequestFreeDiskSpace if leave happens
        * on DeleteAccountL
        * @param aAny: pointer to object.
        */
        static void CancelFreeDiskSpaceRequest( TAny* aAny );


    private: // Data
        CMsvSession*    iSession;
        CMtmUiDataRegistry* iUiRegistry;
        CMtmStore*      iMtmStore;
        CMsvEntry*      iRootEntry;
        CMsvSingleOpWatcher* iRunningOperation;
        TBool           iDeleteSession;
        
    };

#endif // MCESETTINGSSESSIONOBSERVER_H





