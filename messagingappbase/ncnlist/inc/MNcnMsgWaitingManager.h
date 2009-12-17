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
* Description:   Defines class MNcnMsgWaitingManager.
*
*/



#ifndef MNCNMSGWAITINGMANAGER_H
#define MNCNMSGWAITINGMANAGER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Class is used to define message waiting manager API.
 * Message waiting manager in turn is responsible of
 * storing message counts for certain message types 
 * and handling indicator status flags. Message count
 * represents number of new messages. Indicator value
 * in turn tells if indicator icon should be displayed
 * or not. This M-class is pure virtual so client needs to 
 * implement following methods to use it:
 * SetIndicator, SetMessageCount, GetMessageCount
 * and GetIndicator.
 */
class MNcnMsgWaitingManager
    {
    public:  // Constructors and destructor

        /**
         * Destructor.
         */
        virtual ~MNcnMsgWaitingManager() { /* EMPTY */ };
                
    public: //

		/**
		 * Defines indicators that can be enabled / disabled. 
		 * Set/GetIndicator method uses this enum. 
		 */
        enum TNcnIndicator
            {
            ENcnIndicatorFax,
            ENcnIndicatorEmail,
            ENcnIndicatorOther,
            ENcnIndicatorVMLine1,
            ENcnIndicatorVMLine2,
            
            ENcnIndicatorLast // This must be last            
            }; // TNcnIndicator

		/**
		 * Defines message types which message count can be 
		 * stored. 
		 * Set/GetMessageCount method uses this enum. 
		 */
        enum TNcnMessageType
            {
            ENcnMessageTypeFax,
            ENcnMessageTypeEmail,
            ENcnMessageTypeOther,
            ENcnMessageTypeVMLine1,
            ENcnMessageTypeVMLine2,
            
            ENcnMessageTypeLast // This must be last            
            }; // TNcnMessageType

        /**
         * SetIndicator
         * Method is used to enable / disable indicator. 
         * @param aIndicator Id of the indicator that should be enabled / disabled. See TNcnIndicator for ids. 
         * @param aEnable ETrue if indicator should be enabled.
         * @return error
         */                      
        virtual TInt SetIndicator( const TNcnIndicator aIndicator, TBool aEnable ) = 0;
        
        /**
         * SetMessageCount
         * Method is used to store new message count for certain message types. 
         * @param aMsgType Message type which count is updated. See TNcnMessageType for ids. 
         * @param aCount Number of new messages.
         * @param aEnableIndicator ETrue if indicator should be enabled for certain message type. 
         * @return error
         */                        
        virtual TInt SetMessageCount( const TNcnMessageType aMsgType, TUint aCount, TBool aEnableIndicator ) = 0;

        /**
         * GetMessageCount
         * Method returns new message count for certain message type
         * @param aMsgType Message type. See MNcnMsgWaitingManager for ids. 
         * @param aCount Number of new messages.
         * @return error
         */                                
        virtual TInt GetMessageCount( const TNcnMessageType aMsgType, TUint& aCount) = 0;                
                
        /**
         * GetIndicator
         * Method returns indicator status (enabled/disabled).  
         * @param aIndicator Indicator id. See MNcnMsgWaitingManager for ids. 
         * @param aEnabled ETrue if indicator is enabled. 
         * @return error
         */                                                
        virtual TInt GetIndicator( const TNcnIndicator aIndicator, TBool& aEnabled ) = 0;      

        virtual TBool ConstructionReady() = 0;
        
        virtual void GetFirstIndicatorStatus() = 0;
                        
    protected:
        
        MNcnMsgWaitingManager() { /* EMPTY */ };
                        
                
    }; // MNcnMsgWaitingManager

#endif    // MNCNMSGWAITINGMANAGER_H

// End of File
