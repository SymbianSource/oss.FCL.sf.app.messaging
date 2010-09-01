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
* Description:   Defines class CNcnMsgWaitingManager.
*
*/



#ifndef CNCNMSGWAITINGMANAGER_H
#define CNCNMSGWAITINGMANAGER_H

//  INCLUDES
#include "MNcnMsgWaitingManager.h"

#include <e32base.h>
#include <etelmm.h>

// FORWARD DECLARATIONS
class CNcnModelBase;
#ifdef _DEBUG
class CNcnMsgWaitingManagerObserver;
#endif

// CLASS DECLARATION

/**
 * CNcnMsgWaitingManager is responsible of handling 
 * message related indicators and counts. It uses
 * ETEL MM API to store indicator and message count values. 
 */
class CNcnMsgWaitingManager :
    public CActive,
    public MNcnMsgWaitingManager
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aModel Reference to ncn model
         */
        static CNcnMsgWaitingManager* NewL( CNcnModelBase &aModel );

        /**
         * Destructor.
         */
        virtual ~CNcnMsgWaitingManager();
                
    public: // From CActive
    
        /**
         * RunL
         */
        void RunL();

        /**
         * RunError
         * @param aError the error returned
         * @return error
         */
        TInt RunError( TInt aError );

        /**
         * DoCancel
         */
        void DoCancel();
        
    public: // From MNcnMsgWaitingManager

        /**
         * SetIndicator
         * Method is used to enable / disable indicator. 
         * @param aIndicator Id of the indicator that should be enabled / disabled. See MNcnMsgWaitingManager for ids. 
         * @param aEnable ETrue if indicator should be enabled.
         * @return error
         */        
        TInt SetIndicator( const TNcnIndicator aIndicator, TBool aEnable );

        /**
         * SetMessageCount
         * Method is used to store new message count for certain message types. 
         * @param aMsgType Message type which count is updated. See MNcnMsgWaitingManager for ids. 
         * @param aCount Number of new messages.
         * @param aEnableIndicator ETrue if indicator should be enabled for certain message type. 
         * @return error
         */                
        TInt SetMessageCount( const TNcnMessageType aMsgType, TUint aCount, TBool aEnableIndicator );

        /**
         * GetIndicator
         * Method returns indicator status (enabled/disabled).  
         * @param aIndicator Indicator id. See MNcnMsgWaitingManager for ids. 
         * @param aEnabled ETrue if indicator is enabled. 
         * @return error
         */                                
        TInt GetIndicator( const TNcnIndicator aIndicator, TBool& aEnabled );
        
        /**
         * GetMessageCount
         * Method returns new message count for certain message type
         * @param aMsgType Message type. See MNcnMsgWaitingManager for ids. 
         * @param aCount Number of new messages.
         * @return error
         */                        
        TInt GetMessageCount( const TNcnMessageType aMsgType, TUint& aCount );
        
        /**
         * ConstructionReady
         * Returns ETrue, if initial settings have been read from RMobilePhone. Otherwise returns EFalse.
         */                                                        
        TBool ConstructionReady();

        /**
         * GetFirstIndicatorStatus
         * Indicator statuses are fetched only after SIM has been verified to be in the ESimUsable state.
         */          
        void GetFirstIndicatorStatus();
        
                                                        
    private:  // Constructors and destructor

        /**
         * C++ default constructor.
         * @param aMsvSessionHandler Msv session handler reference.
         */
        CNcnMsgWaitingManager( CNcnModelBase &aModel );
        
        /**
         * 2nd. phase constructor.
         */
        void ConstructL( );
        
	private: // Type definitions
	
		/**
         * Current state indicating what kind of operation is on-going. 
         */
		enum TNcnWaitingManagerState
			{
			ENcnIdle,
			ENcnGetIndicator,
			ENcnSetIndicator,
			ENcnSetCount,
			ENcnGetCount,
			ENcnNotifyMsgWaiting
			}; // TNcnWaitingManagerState   
			
		/**
		 * Work Queue element. 
		 */			
		struct TNcnReqQueueElement
			{
			TNcnReqQueueElement(TNcnWaitingManagerState aCmd, TInt aField, TBool aParam1, TUint aParam2 = 0 )
				{
				iCmd = aCmd;
				iField = aField;
				iParam1 = aParam1;
				iParam2 = aParam2;
				}
			
			TNcnWaitingManagerState	iCmd; 
			TInt	iField;			// See TNcnIndicator or TNcnMessageType in MNcnMsgWaitingManager
			TBool	iParam1;		// ETrue/EFalse if indicator should be enabled
			TUint	iParam2;		// Used only for count operations. Tells if indicator should be enabled/disabled.					
			}; // TNcnReqQueueElement			   
        
	private: // Internal methods. 
	
        /**
         * Enable / disable indicator
         */
		void DoSetIndicator( TInt aId, TBool aEnable );
		        
    private: // Data
            
		// Current state
        TNcnWaitingManagerState iState;
        
        // Reference to ncn model
        CNcnModelBase &iModel;
        
        // Needed to initialize iMobilePhone
        RTelServer iTelServer;
        
        // Needed to initialize iMobilePhone
        RTelServer::TPhoneInfo iPhoneInfo;
            
		// Used to store indicator flags and message counts. 
        RMobilePhone iMobilePhone;
                        
        // Struct is a container for indicator flags and message counts
        RMobilePhone::TMobilePhoneMessageWaitingV1      iMsgWaiting;  
                      
		// Package for iMsgWaitingV1                      
        RMobilePhone::TMobilePhoneMessageWaitingV1Pckg  iMsgWaitingPckg;   
        
        // Request Queue
        RArray<TNcnReqQueueElement> iReqQueue;       
        
#ifdef _DEBUG

		CNcnMsgWaitingManagerObserver *iObserver;
#endif        
        
        TBool iConstructionReady;

    }; // CNcnMsgWaitingManager

#endif    // CNCNMSGWAITINGMANAGER_H

// End of File
