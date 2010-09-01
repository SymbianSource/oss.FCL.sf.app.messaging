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
* Description:   Defines class CNcnPublishAndSubscribeObserver.
*
*/



#ifndef NCNPUBLISHANDSUBSCRIBEOBSERVER_H
#define NCNPUBLISHANDSUBSCRIBEOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>
#include "NcnSubscribeHandler.h"

// CLASS DECLARATION
class CNcnModelBase;
class CNcnSubscriber;

/**
* Listens to the Phone idle status and other Properties
* through the Publish and Subscribe session.
*/
class CNcnPublishAndSubscribeObserver : public CBase, public MNcnSubscribeHandler
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CNcnPublishAndSubscribeObserver( CNcnModelBase* aModel );

        /**
        * Two-phased constructor.
        */
        static CNcnPublishAndSubscribeObserver* NewL( CNcnModelBase* aModel );

        /**
        * Destructor.
        */
        virtual ~CNcnPublishAndSubscribeObserver();

    public:
        
        /*
        * Through this Publish and Subscribe is notified about state changes.
        * @param aCategory Property category.
        * @param aVariable The Property to be set
		* @param aState The value to be set for the aVariable Property
        */
        void NotifyPublishAndSubscribe( const TUid& aCategory, TUint aVariable, const TInt aState );

	public: //from base classes

		 /**
        * Through this method the Property subscribers notify of Property changes
        * @param aCategory The category of the property to be observed
		* @param aKey The subkey of the property to be observed
		*/
        void HandlePropertyChangedL( const TUid& aCategory, TInt aKey );

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CNcnPublishAndSubscribeObserver( const CNcnPublishAndSubscribeObserver& );
        // Prohibit assigment operator
        CNcnPublishAndSubscribeObserver& operator= ( const CNcnPublishAndSubscribeObserver& );

		 /**
        *  Determine idle state.
        * Reads iAutolockStatus, iCurrentCallStatus and iPhoneIdleState and informs 
        * model about the idle status
		*/
        void DetermineIdleState();
        
    private:    // Data

        // Property subscribers
		CNcnSubscriber* iSimChangedSubscriber;
		CNcnSubscriber* iPhoneIdleSubscriber;
		CNcnSubscriber* iAutolockSubscriber;
		CNcnSubscriber* iCurrentCallSubscriber;
		CNcnSubscriber* iSimUsableSubscriber;
		
         // A pointer to the event monitor.
        CNcnModelBase* iModel;

        // Stores the phone idle state status
        TInt iPhoneIdleState;

        // Stores the autolock status
        TInt iAutolockStatus;

        // Stores the current call status
        TInt iCurrentCallStatus;
    };

#endif      // NCNPUBLISHANDSUBSCRIBEOBSERVER_H

// End of File
