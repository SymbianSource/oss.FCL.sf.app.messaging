/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Defines the CWaitingObserver class
*
*/




#ifndef CWAITINGOBSERVER_H
#define CWAITINGOBSERVER_H

#include 	<etelmm.h> // RMobilePhone

// FORWARD DECLARATIONS
class CMsgIndicatorPluginImplementation;

enum TIndicatorMessageType
    {
    EIndicatorMessageTypeFax,
    EIndicatorMessageTypeVMLine1,
    EIndicatorMessageTypeVMLine2,
    }; // TIndicatorMessageType

class CWaitingObserver: 
	public CActive
	{
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CWaitingObserver* NewL( RMobilePhone &aMobilePhone, CMsgIndicatorPluginImplementation& iIndicatorPlugin );

        /**
         * Destructor.
         */
        virtual ~CWaitingObserver();
        
        TInt GetMsgCount(const TIndicatorMessageType aMsgType);
		/**
         * Check the KMuiuSupressAllNotificationConfiguration value
         */        
        TBool CheckSupressNotificationSettingL();
                
    public: // From CActive
    
        /**
         * RunL
         */
        void RunL();

        /**
         * DoCancel
         */
        void DoCancel();
                                                                
    private:  // Constructors and destructor

        /**
         * C++ default constructor.
         */
        CWaitingObserver( RMobilePhone &aMobilePhone, CMsgIndicatorPluginImplementation& iIndicatorPlugin );
        
        /**
         * 2nd. phase constructor.
         */
        void ConstructL( );
        
	private:
	        
        /**
         * Update indicator status
         */
		void UpdateIndicatorStatusL();        
		        
    private: // Data
            
		TBool iChangeNotifySubscribed;            

        // Needed to initialize iMobilePhone
        RTelServer iTelServer;
        
        // Needed to initialize iMobilePhone
        RTelServer::TPhoneInfo iPhoneInfo;
            
		RMobilePhone &iMobilePhone;
		
        // Struct is a container for indicator flags and message counts
        RMobilePhone::TMobilePhoneMessageWaitingV1      iMsgWaiting;  
                      
		// Package for iMsgWaitingV1                      
        RMobilePhone::TMobilePhoneMessageWaitingV1Pckg  iMsgWaitingPckg;   

        CMsgIndicatorPluginImplementation &iIndicatorPlugin;
        
	}; // CWaitingObserver
	
#endif //CWAITINGOBSERVER_H	
