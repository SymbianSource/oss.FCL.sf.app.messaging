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
* Description:   Defines class CNcnMsvSessionHandler.
*
*/



#ifndef NCNMSVSESSIONHANDLER_H
#define NCNMSVSESSIONHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <msvapi.h>     // For MMsvSessionObserver

// FORWARD DECLARATIONS
class MNcnMsvSessionHandlerObserver;
class CMsvSession;

// CLASS DECLARATION

/**
 * Handles the Msv-session instance and notifier observers about
 * changes in session state.
 */
class CNcnMsvSessionHandler :
    public CBase,
    public MMsvSessionObserver
    {
    private:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CNcnMsvSessionHandler( );

        /**
         * 2nd. phase constructor.
         */ 
        void ConstructL();

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CNcnMsvSessionHandler* NewL( );

        /**
        * Destructor.
        */
        virtual ~CNcnMsvSessionHandler();
        
    public:     // From MMsvSessionObserver
    
        void HandleSessionEventL(
            TMsvSessionEvent aEvent,
            TAny* aArg1,
            TAny* aArg2,
            TAny* aArg3 );

    public:  // New methods

        /**
         * Adds an observer.
         * @param aObserver The observer to add. Ownership is not passed.
         */
        void AddObserverL( MNcnMsvSessionHandlerObserver* aObserver );

        /**
         * Removes the specified observer.
         * @param aObserver The observer to remove. Ownership is not passed.
         */
        void RemoveObserver( MNcnMsvSessionHandlerObserver* aObserver );

        /**
         * Returns ETrue if Msv session is ready.
         * @return ETrue if Msv session is ready.
         */
        TBool IsMsvSessionReady() const;

        /**
         * Returns Msv session reference. May leave with KErrGeneral
         * if Msv session is not ready.
         * @return The Msv session reference.
         */
        CMsvSession& MsvSessionL();
        
        /**
         * Starts the session handler. Method will connect the session
         * and handler notifies observers about the progress.
         * Method may leave if Msv session could not be instantiated.
         */
        void StartSessionHandlerL();
        
        /**
         * Stops the session handler.
         */
        void StopSessionHandler();
        
    private:    // New methods
    
        /**
         * Handles Msv terminated event.
         */
        void HandleMsvTerminatedL();        
        
        /**
         * Handles Msv closed event.
         */
        void HandleMsvClosed();
        
        /**
         * Handles created entries.
         */
        void HandleMsvEntriesCreatedL( const CMsvEntrySelection& aSelection );
        
        /**
         * Handles deleted entries.
         */
        void HandleMsvEntriesDeletedL( const CMsvEntrySelection& aSelection );                
        
        /**
         * Notifies that Msv session is ready.
         */
        void NotifyMsvSessionReady();
        
        /**
         * Notifies that Msv session is closed.
         */
        void NotifyMsvSessionClosed();
        
        /**
         * Notifies about created msv entry.
         * @param aMsvId The id of the created entry.
         */
        void NotifyMsvEntryCreated( const TMsvId& aMsvId );
        
        /**
         * Notifies about deleted msv entry.
         * @param aMsvId The id of the deleted entry.
         */
        void NotifyMsvEntryDeleted( const TMsvId& aMsvId );
        
         /**
         * Notifies that the Message Server has automatically changed the index 
         * location to use the internal disk.
         * @param aDriveNumber The drive number where the message store moves
         */
        void HandleMsvMediaChangedL( const TDriveNumber& aDriveNumber );

		/**
		* Notifies that the disk containing the Message Store is available again.
		* The Message Server can now operate as normal
		* @param aDriveNumber is a TDriveNumber value that identifies the drive that is being used.
		*/
        void HandleMsvMediaAvailableL( const TDriveNumber& aDriveNumber );
        
        /**
		* Notifies that the disk containing the Message Store is Unavailable .
		*/
        void HandleMsvMediaUnAvailableL( );
        
    private:    // Data

        // Msv session pointer, owned
        CMsvSession* iMsvSession;        
        
        // Observer array, pointers are not owned
        CArrayPtrSeg<MNcnMsvSessionHandlerObserver> iObservers;
    };

#endif    // NCNMSVSESSIONHANDLER_H

// End of File
