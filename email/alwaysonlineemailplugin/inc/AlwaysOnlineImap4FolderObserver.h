/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     Observer of the given folder. If new messages are created to the folder, 
*     calls HandleFolderEntryL.
*
*/

#ifndef __ALWAYSONLINEIMAP4FOLDEROBSERVER_H__
#define __ALWAYSONLINEIMAP4FOLDEROBSERVER_H__

#include <msvstd.h> // general messaging
#include <msvapi.h> // MMsvEntryObserver


/**
* MAlwaysOnlineImap4FolderObserver 
*
* 
*/

class MAlwaysOnlineImap4FolderObserver
    {
    public:
    	virtual void HandleFolderEntryL( const CMsvEntrySelection& aFolderArray ) = 0;
    };


/**
* CAlwaysOnlineImap4FolderObserver 
*
* 
*/
class CAlwaysOnlineImap4FolderObserver : 
    public CActive,
    public MMsvSessionObserver
    {
    public:
       /**
        * 2-phase constructor.
        */
        static CAlwaysOnlineImap4FolderObserver* NewL(
            CMsvSession& aMsvSession,
            MAlwaysOnlineImap4FolderObserver& aObserver );

        /**
        *
        */
        static CAlwaysOnlineImap4FolderObserver* NewL(
            CMsvSession& aMsvSession,
            CMsvEntrySelection* aFoldersToWatch,
            MAlwaysOnlineImap4FolderObserver& aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CAlwaysOnlineImap4FolderObserver();

    public:
        /**
        *
        */
        void Start();
        void Stop();

        /**
        *
        */
        void AppendFolderL( TMsvId aFolderId );

        /**
        *
        */
        void RemoveFolder( TMsvId aFolderId );

        /**
        *
        */
        void ResetFoldersL( CMsvEntrySelection* aFoldersToWatch );

        /**
        *
        */
        void SetMailbox( const TMsvId aMailboxId );




    public:
        void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);


    private:
        /**
        * From CActive
        */
        virtual void DoCancel();


        /**
        * From CActive ?member_description
        */
        virtual void RunL();

       /**
        * C++ constructor.
        */
        CAlwaysOnlineImap4FolderObserver( 
            CMsvSession& aMsvSession,
            MAlwaysOnlineImap4FolderObserver& aObserver );

        /**
        * constructor.
        */
        void ConstructL( CMsvEntrySelection* aFoldersToWatch );
        
    private:
        RTimer          iTimer;
        CMsvSession&    iMsvSession;
        CMsvEntrySelection* iFolders;
        CMsvEntrySelection* iChangedFolders;
        TBool           iRunning;
        MAlwaysOnlineImap4FolderObserver& iObserver;
        TBool           iSendNotification;
        TMsvId          iMailboxId;
    };



#endif
//EOF
