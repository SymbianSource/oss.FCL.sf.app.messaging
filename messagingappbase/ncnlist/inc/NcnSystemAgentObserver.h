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
* Description:   Defines class CNcnSystemAgentObserver.
*
*/



#ifndef NCNSYSTEMAGENTOBSERVER_H
#define NCNSYSTEMAGENTOBSERVER_H

//  INCLUDES
#include    <e32base.h>
#include    <savarset.h>       // RSAVarChangeNotify

// CLASS DECLARATION
class CNcnModelBase;

/**
*  Listens to the Phone idle status through the System Agent session.
*/
class CNcnSystemAgentObserver : public CActive
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CNcnSystemAgentObserver( CNcnModelBase* aModel );

        /**
        * Two-phased constructor.
        */
        static CNcnSystemAgentObserver* NewL( CNcnModelBase* aModel );

        /**
        * Destructor.
        */
        virtual ~CNcnSystemAgentObserver();

    public:

        /*
        * Through this System Agent is notified about state changes.
        */
        void NotifySystemAgent( const TUid& aVariable, const TInt aState );

        /**
        * By this method the user of this class orders the observer to start receiving events.
        */
        void IssueRequest();

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CNcnSystemAgentObserver( const CNcnSystemAgentObserver& );
        // Prohibit assigment operator
        CNcnSystemAgentObserver& operator= ( const CNcnSystemAgentObserver& );

        /**
        * Cancels the request.
        */
        void DoCancel();

        /**
        * Handles the event.
        */
        void RunL();

    private:    // Data

        // For being notified by System Agent
        RSystemAgent iSystemAgent;

        // For notifying System Agent
        RSAVarChangeNotify iSavar;

        // A pointer to the event monitor.
        CNcnModelBase* iModel;

        // The event from System Agent.
        TSysAgentEvent  iSAEvent;

        // Stores the phone idle state status
        TInt iPhoneIdleState;

        // Stores the autolock status
        TInt iAutolockStatus;

        // Stores the current call status
        TInt iCurrentCallStatus;

    };

#endif      // NCNSYSTEMAGENTOBSERVER_H

// End of File
