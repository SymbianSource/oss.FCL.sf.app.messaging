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
* Description:   Defines class MNcnMsvSessionHandlerObserver.
*
*/



#ifndef NCNMSVSESSIONHANDLEROBSERVER_H
#define NCNMSVSESSIONHANDLEROBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <msvstd.h>

// FORWARD DECLARATIONS
class CMsvSession;

// CLASS DECLARATION

/**
 * Observer interface for Msv session handling.
 */
class MNcnMsvSessionHandlerObserver
    {
    public:
        /**
         * Notifies that Msv session is ready to use.
         * @param aMsvSession The Msv session.
         */
        virtual void HandleMsvSessionReadyL( CMsvSession& aMsvSession );

        /**
         * Notifies that Msv session is closed.
         */
        virtual void HandleMsvSessionClosedL();
        
        /**
         * Notifies that an entry has been created.
         * @apram aMsvId Id of the created entry.
         */
        virtual void HandleMsvEntryCreatedL( const TMsvId& aMsvId );
        
        /**
         * Notifies that an entry has been created.
         * @apram aMsvId Id of the deleted entry.
         */
        virtual void HandleMsvEntryDeletedL( const TMsvId& aMsvId );
        
         /**
         * Notifies that the Message Server has automatically changed the index 
         * location to use the internal disk.
         * @param aDriveNumber The drive number where the message store moves
         */
         virtual void HandleMsvMediaChangedL( const TDriveNumber& aDriveNumber );
         
         /**
         * The disk containing the Message Store is Unavailable again.
         * Change the message store to default
         */
         virtual void HandleMsvMediaUnavailableL();
         
         
		/** 
		* The disk containing the Message Store is available again.
		* The Message Server can now operate as normal
		* @param aDriveNumber is a TDriveNumber value that identifies the drive that is being used.
		*/
        virtual void HandleMsvMediaAvailableL( const TDriveNumber& aDriveNumber );
                  
    };
    
#endif    // NCNMSVSESSIONHANDLEROBSERVER_H

// End of File
