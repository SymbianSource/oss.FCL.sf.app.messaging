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
*       The purpose of this class is to observe if there are messages 
*       in the outbox.
*
*/




#ifndef NCNOUTBOXOBSERVER_H
#define NCNOUTBOXOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <msvapi.h>         // for MMsvSessionObserver
#include "MNcnMsvSessionHandlerObserver.h"
#include "MNcnSignalStrengthObserver.h"


// FORWARD DECLARATIONS
class CNcnModelBase;
class CMsvSession;
class CNcnOutboxSender;
class MNcnSignalStrengthHandler;

// CLASS DECLARATION

/**
*  Observes if there are messages in the OutBox..
*/
class CNcnOutboxObserver : 
    public CBase, public MMsvEntryObserver, public MNcnMsvSessionHandlerObserver,
    public MNcnSignalStrengthObserver
    {
    public:  // Constructors and destructor

        /**
        *   A Two-phased constructor.
        */
        static CNcnOutboxObserver* NewL( CNcnModelBase* aModel );

        /**
        *   Destructor.
        */
        virtual ~CNcnOutboxObserver();

    private:    // From MNcnSignalStrengthObserver
        
        void SignalStrengthAndBarUpdatedL( TInt aNewSignalValue, TInt aNewBarValue );
                                             
    public: // From MNcnMsvSessionHandlerObserver
    
        void HandleMsvSessionReadyL( CMsvSession& aMsvSession );
        void HandleMsvSessionClosedL();
        
    public: // Functions from base classes

        /**
        *   The messaging server sends notifications of changes in the
        *   observed entry to CNcnOutboxObserver by calling this method.
        */
        void HandleEntryEventL( 
            TMsvEntryEvent aEvent, TAny* /*aArg1*/, 
            TAny* /*aArg2*/, TAny* /*aArg3*/ );
            
            

    public: // New functions
        
        /**
        *  Informs the networkstatus to Outbox Sender
        */
        void InformOutboxSenderL( const TInt& aNetworkBars );

    private:

        /**
        *   A C++ constructor.
        */
        CNcnOutboxObserver( CNcnModelBase* aModel );

        /**
        *   By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        *   By default, prohibit copy constructor
        */
        CNcnOutboxObserver( const CNcnOutboxObserver& );

        /**
        *   Prohibit assigment operator
        */
        CNcnOutboxObserver& operator= ( const CNcnOutboxObserver& );

    private:    // New functions

        /**
        *   Checks if any items found from outbox and turns the indicator
        *   ON/OFF accordingly.
        */
        void CheckOutboxAndNotifyL();

        /**
         * Creates the Msv Session and sets the outbox 
         * observer and outbox sender.
         * @param aMsvSession The Msv session to use.
         */
        void StartSessionsL( CMsvSession& aMsvSession );

        /**
        *   Delete session, outbox observer and outbox sender.
        */
        void EndSessions();       

    private:    // Data

        // A pointer to the event monitor object.
        CNcnModelBase*  iModel;
        // In-box folder entry. Note that the entry is not owned by this class.
        CMsvEntry* iOutboxFolder;
        // Pointer to Outbox Sender.
        CNcnOutboxSender* iOutboxSender;
        
        // Signal strength handler. Owned.
        MNcnSignalStrengthHandler* iSignalStrengthHandler;
    };

#endif      // NCNOUTBOXOBSERVER_H

// End of File
