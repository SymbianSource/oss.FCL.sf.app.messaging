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
*     An active object class which takes care of reading the possible new
*     service centres from SIM and adds them to Sms Settings.
*
*/



#ifndef __MSGSIMSCNUMBERDETECTOR_H__
#define __MSGSIMSCNUMBERDETECTOR_H__

//  INCLUDES
#include <msvapi.h>     // for MsvSession
#include <smsclnt.h>    // for CSmsClientMtm
#include <e32base.h>
#include <smutsimparam.h>   // for CSmsSimParamOperation

// FORWARD DECLARATIONS
class CClientMtmRegistry;
class CSmumStartUpMonitor;
// CLASS DECLARATION

/**
*  CMsgSimOperation
*  Inherited from CActive, MMsvSessionObserver
*/
class CMsgSimOperation: public CActive, public MMsvSessionObserver
    {
    public: // Constructors and destructor

        /**
        * Constructor.
        */
        IMPORT_C static CMsgSimOperation* NewL( TRequestStatus& aClientStatus );

        /**
        * Destructor
        */
        ~CMsgSimOperation();

	public: // new functions
	
		/**
        * Callback function for startup state monitor object
        */
		void HandleStartupReadyL();
		
    private: // new functions
        
        /**
        * C++ constructor which initializes the active object with a 
        * default priority and adds it to the active scheduler.
        * @param TRequestStatus& aStatus, TInt aPriority
        * @return nothing
        */
        CMsgSimOperation( TRequestStatus& aStatus, TInt aPriority = EPriorityStandard );

        /**
        * This function simulates issuing request, activates the object and starts to open Msv session
        * @param none
        * @return nothing
        */
        void StartL();

        /**
        * Complete this active object's request status
        * @param TInt aValue
        * @return nothing
        */
        void CompleteRequest( TInt aValue );

        /**
        * Complete the client's active object request status
        * @param TInt aValue
        * @return nothing
        */
        void CompleteClientRequest( TInt aValue );

        /**
        * Do we have a client request status to complete?
        * @param nothing
        * @return TBool
        */
        TBool HaveClientRequestStatus() const;

    private: // Panic related

        /**
         *
         */
        enum TSimOperationPanic
            {
            ESimOperationPanicRequestAlreadyActive = 0,
            ESimOperationPanicClientsRequestAlreadyCompleted
            };

        /**
        * Panic the thread
        * @param TSimOperationPanic aPanic
        * @return nothing
        */
        static void Panic(TSimOperationPanic aPanic);
        
    private: // from baseclasses

        /**
        * From CActive 
        */
        void DoCancel();
        void RunL();
        void DoRunL();
        TInt RunError( TInt );
        
        /**
        * From MMsvSessionObserver 
        */
         void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3 );

         /**
         * By default Symbian OS constructor is private.
         */        
         void ConstructL();
    
    private: // Data
        TRequestStatus* 		iClientStatus;
        CSmsSimParamOperation*	iSimOperation;
        CMsvSession* 			iMsvSession;
        CSmsClientMtm* 			iSmsClientMtm;
        CClientMtmRegistry* 	iClientRegistry;
        TInt 					iRetryCount;
        CSmumStartUpMonitor*	iStartupMonitor;
        TBool                   iCheckSimScAlways;
        
    };

#endif  // not defined __MSGSIMSCNUMBERDETECTOR_H__

// End of file
