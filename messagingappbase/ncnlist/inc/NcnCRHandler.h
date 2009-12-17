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
* Description:   Defines class CNcnCRHandler.
*
*/



#ifndef NCNSCRHANDLER_H
#define NCNSCRHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <cenrepnotifyhandler.h>
#include "NcnSubscribeHandler.h"
#include "MNcnCRRepositoryHandlerCallback.h"

// CONSTANTS
const TInt KNcnSDReconnectingDelay  = 50000;
const TInt KNcnSDReconnectingTrials = 10;

// CONSTANTS
const TInt KVoiceMailWaiting = 0x0001;
const TInt KVoiceMailWaitingOnLine1 = 0x0002;
const TInt KVoiceMailWaitingOnLine2 = 0x0004;
const TInt KVoiceMailWaitingOnBothLines = 0x0008;

const TInt KEmailMessageWaiting = 0x0010;
const TInt KFaxMessageWaiting = 0x0020;
const TInt KFaxOtherMessageWaiting = 0x0040;


const TInt KCallsFwdActivated = 0x0100;
const TInt KCallsFwdActivatedToVmbx = 0x0200;
const TInt KCallsFwdActivatedOnLine1 = 0x0400;
const TInt KCallsFwdActivatedOnLine2 = 0x0800;
const TInt KCallsFwdActivatedOnBothLines = 0x1000;

const TInt KVoiceMailWaitingBitMask = 0xfff0;
const TInt KCallsFwdStatusBitMask = 0x00ff;

const TInt KAllBitsOn = 0xffff;

// FORWARD DECLARATIONS
class CNcnModelBase;
class CRepository;
class CNcnSubscriber;
class CNcnCRRepositoryHandler;

// CLASS DECLARATION

/**
*  Observes the value of the shared data missed calls variable.
*/
class CNcnCRHandler :
    public CBase,
    //public MCenRepNotifyHandlerCallback,
    public MNcnCRRepositoryHandlerCallback,
    public MNcnSubscribeHandler
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CNcnCRHandler( CNcnModelBase* aModel );

        /**
        * Two-phased constructor.
        */
        static CNcnCRHandler* NewL( CNcnModelBase* aModel );

        /**
        * Destructor.
        */
        virtual ~CNcnCRHandler();


    public: // From MNcnCRRepositoryHandlerCallback
        void HandleRepositoryNotifyInt(
            CRepository& aRepository,
            TUint32 aId,
            TInt aNewValue );
            
    public: // Functions from base classes

        /**
         * Through this method the Property subscribers notify of Property changes
         * @param aCategory The category of the property to be observed
		 * @param aKey The subkey of the property to be observed
		 */
        void HandlePropertyChangedL( const TUid& aCategory, TInt aKey );

    public:  // Other public methods

        /**
        * Resets the missed calls to zero.
        */
        void ResetMissedCalls();

        /**
        * Sets the sms initialisation flag in the shared data.
        */
        void SetSmumFlag( const TInt aNewFlagValue );

        /**
        * Sets the state of message tone playing -flag
        */
        void MsgReceivedTonePlaying( const TUint aPlaying );

        /**
        * Fetches the string from CR with given keys
        * @param aUid Uid to be used
        * @param aKey Which value is to be fetched
        * @param aValue Variable to store fetched value
        * @return KErrNone, when no errors happened
        */
        TInt GetString(
            const TUid&     aUid,
            const TUint32   aKey,
            TDes&           aString ) const;

        /**
        * Fetches the string from P&S with given keys
        * @param aUid Uid to be used
        * @param aKey Which value is to be fetched
        * @param aValue Variable to store fetched value
        * @return KErrNone, when no errors happened
        */
        TInt GetPSString(
            const TUid&     aUid,
            const TUint32   aKey,
            TDes&           aString ) const;

		/**
        * Fetches the integer from CR with given keys
        * @param aUid Uid to be used
        * @param aKey Which value is to be fetched
        * @param aValue Variable to store fetched value
        * @return KErrNone, when no errors happened
        */
        TInt GetCRInt(
            const TUid&     aUid,
            const TUint32   aKey,
            TInt&           aValue ) const;

        /**
        * Fetches the string from CR with given keys
        * @param aUid Uid to be used
        * @param aKey Which value is to be fetched
        * @param aValue Variable that contains value to be stored
        * @return KErrNone, when no errors happened
        */
        TInt SetCRInt(
            const TUid&     aUid,
            const TUint32   aKey,
            const TInt      aValue,
            const TBool     aSave = EFalse ) const;

        /**
        * Checks the count of missed calls and 
        * updates notification if needed.
        */    
	    void UpdateMissedCallsNotification();      
	    
    protected:

        /**
        * By default Symbian OS constructor.
        */
        virtual void ConstructL();

    private:
 
        /**
        * Gets the CR client according to Uid to be used
        * @param aUid Uid to be used
        * @param aClient Variable to client shall be stored
        * @return KErrNone, when succesful
        */
        TInt GetClientForUid(
            const TUid&		aUid,
            CRepository*&	aClient ) const;
	              
    protected:

        /**
        * Stores the current indicator status flags to the shared data file.
        */
        void StoreIndicatorStatusFlags();

    protected: // Data

        // A pointer to the model.
        CNcnModelBase* iModel;
        // A CR session & notifier for logs
        CRepository* iLogsSession;
        
        // Repository handler for logs, owned
		CNcnCRRepositoryHandler* iLogsNotifyHandler;
		
        // A CR session for ncnlist
        CRepository* iNcnSession;
		// A CR session for Muiu
        CRepository* iMuiuSession;
		// A CR session & notifier for Smum
        CRepository* iSmumSession;
        
        // Repository handler for Smum, owned
		CNcnCRRepositoryHandler* iSmumNotifyHandler;
        
        // Repository handler for Muiu, owned
		CNcnCRRepositoryHandler*  iMuiuSettingsNotifyHandler;
		
		// A CR session for profiles
		CRepository* iProfileSession;
		// A CR session for telephony
		CRepository* iTelephonySession;

		// Property subscribers
		CNcnSubscriber* iAlertToneSubscriber;
		CNcnSubscriber* iMsgToneSubscriber;

        // Tells whether the Shared Data logs file is assigned succesfully.
        TBool iLogsSDAssignedOk;

        TBool iSysApSDAssignedOk;

    };

#endif      // NCNCRHANDLER_H

// End of File
