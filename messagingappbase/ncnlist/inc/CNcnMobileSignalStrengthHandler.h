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
* Description:   Signal strength handler implementation over telephony API
*
*/



#ifndef NCNMOBILESIGNALSTRENGTHHANDLER_H
#define NCNMOBILESIGNALSTRENGTHHANDLER_H

// INCLUDES
#include <e32base.h>
#include <etelmm.h>
#include "MNcnSignalStrengthHandler.h"

// FORWARD DECLARATIONS
class MNcnSignalStrengthObserver;

// CLASS DECLARATION

/**
* Class implementing a signal strength handler over the telephony API
*/

class CNcnMobileSignalStrengthHandler : public CActive, public MNcnSignalStrengthHandler
{
    public:
        /**
         * Two-phased constructor.
         */
        static CNcnMobileSignalStrengthHandler* NewL( );

        /**
         * Destructor.
         */
        ~CNcnMobileSignalStrengthHandler();

    private:
        /**
         * C++ default constructor.
         */
        CNcnMobileSignalStrengthHandler();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    public: // from CActive
        /**
         * RunL
         */
        void RunL();

        /**
         * RunError
         * @param aError the error returned
         *  @return error
         */
        TInt RunError( TInt aError );

        /**
         * DoCancel
         */
        void DoCancel();

    public: // from MNcnSignalStrengthHandler
        /**
         * Returns ETrue if signal observing is supported, EFalse otherwise.
         * @return ETrue if signal observing is supported, EFalse otherwise.
         */
        TBool ObservingSupported();
        
        /**
         * Returns ETrue if signal strength requests are supported, EFalse otherwise.
         * @return ETrue if signal strength requests are supported, EFalse otherwise.
         */
        TBool RequestsSupported();

        /**
         * Sets the signal strength observer. May leave with KErrNotSupported
         * if signal strength observing is not supported.
         * @param aObserver Observer for signal strength. Ownership is not passed.
         */
        void SetSignalStrengthObserverL( MNcnSignalStrengthObserver* aObserver );

        /**
         * Starts observing the signal strength. May leave with KErrNotSupported
         * if signal strength observing is not supported.
         */
        void StartObservingL();
 
        /**
         * Stops the signal strength observing.
         */
        void StopObservingL();

        /**
         * Returns the current signal strength. May leave with KErrNotSupported
         * if signal strength requests are not supported.
         * @return The current signal strength.
         */
        TInt SignalStrengthL();
        
        /**
         * Returns the current bar value. May leave with KErrNotSupported
         * if bar value requests are not supported.
         * @return The current signal strength.
         */
        TInt BarValueL();

    private: // New methods

        /**
         * Initializes server sessions.
         */
        void InitializeSessionsL();
        
        /**
         * Updates the current value of the signal strength.
         */
        void UpdateSignalStrengthL();
        
        /**
         * Notifiers observer about signal strength.
         */
        void NotifyObserver();

    private: // data
    	/**
         * Pointer to signal strength observer, may be NULL. Also not owned by the instance.
         */
        MNcnSignalStrengthObserver* iObserver;
        
        /**
         * State variable for observation.
         */
        TBool iObserving;

        /**
         * Current value of the signal strength.
         */
        TInt32 iSignalStrength;
        
        /**
         * Network bars value.
         */
        TInt8 iBars;
        
        /**
         * Mobile phone.
         */
        RMobilePhone iMobilePhone;

        /**
         * Telephony server session.
         */
        RTelServer iTelServer;
};


#endif // NCNMOBILESIGNALSTRENGTHHANDLER_H

// End of File
