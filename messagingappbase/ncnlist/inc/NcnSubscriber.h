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
* Description:   Defines class CNcnSubscriber.
*
*/



#ifndef NCNSUBSCRIBER_H
#define NCNSUBSCRIBER_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>
#include "NcnPublishAndSubscribeObserver.h"

// forward declarations
class MNcnSubscribeHandler;

// CLASS DECLARATION
/**
*  CNcnSubscriber is an acitve object that subscribes to a single
*  Publish and Subscribe Property and notifies about the Property changes.
*/

class CNcnSubscriber : public CActive
{
public:
    /**
    * Two-phased constructor.
	*
	* @param aPropertyResponder The object to be notified about Property changes
	* @param aCategory The category of the Property to be observed
	* @param aKey The subkey of the Property to be observed
    */
    static CNcnSubscriber* NewL( MNcnSubscribeHandler& aPropertyResponder, const TUid& aCategory, TUint aKey );

    /**
    * Destructor.
    */
     ~CNcnSubscriber();

	 /**
	  * Subscribes to the Property given in constructor
	  */
     void Subscribe();

private:
	/**
     * C++ default constructor.
	 *
  	 * @param aPropertyResponder The object to be notified about Property changes
	 * @param aCategory The category of the Property to be observed
	 * @param aKey The subkey of the Property to be observed
     */
     CNcnSubscriber( MNcnSubscribeHandler& aPropertyResponder, const TUid& aCategory, TUint aKey );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
	 void ConstructL();

public: // from CActive
    /**
	*   RunL
    */
    void RunL();

    /**
	*   RunError
    *	@param aError the error returned
	*
    *	@return error
    */
    TInt RunError( TInt aError );

    /**
    *	DoCancel
    */
    void DoCancel();

private: // Methods not implemented
    CNcnSubscriber( const CNcnSubscriber& );           // Copy constructor
    CNcnSubscriber& operator=( const CNcnSubscriber& );// Assigment operator

private: // data

	// The object to be notified about Property changes
    MNcnSubscribeHandler&  iPropertyResponder;

	// The property object that subscribes to the Property to be observed
    RProperty                 iProperty;

	// The category of the Property to be observed
    TUid                      iCategory;

	// The subkey of the Property to be observed
	TUint                     iKey;
};


#endif // NCNSUBSCRIBER_H

// End of File
