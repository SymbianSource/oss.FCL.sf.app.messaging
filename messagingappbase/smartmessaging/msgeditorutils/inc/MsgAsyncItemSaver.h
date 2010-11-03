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
*     This is a base class for implementing asyncronous item saving.
*
*/



#ifndef CMSGASYNCITEMSAVER_H
#define CMSGASYNCITEMSAVER_H

// INCLUDES

#include <e32base.h>
 
// CLASS DECLARATION

/**
* Interface for implementing the asyncronous saving operations.
*/
class MMsgAsyncItemSaver
    {

    public:
        
        /**
        * Checks the status of the operation.
        * @return ETrue if the operation can be completed, EFalse
        * if the operation should continue.
        */
        virtual TBool IsComplete() const = 0;
        
        /**
        * Perform some operations and set the inner state.
        * If this leaves then RunError will be called with the
        * leave code.
        */
        virtual void ContinueOperationL() = 0;

        /**
        * This method is called after IsComplete() has return ETrue.
        * This method should result deleting the saver object.
        */
        virtual void CompleteOperationL() = 0;

        /**
        * Error handling method. If a leave occurs in ContinueOperationL
        * then this method will be called with the leave code.
        * @param aError Error code from the leave.
        * @return Error code or KErrNone if error doesn't need to be
        * handled. Returning KErrNone will continue the operation.
        */
        virtual TInt HandleError( TInt aError ) = 0;
    };

/**
* This class implements the asyncronous framework for item saving.
*/
class CMsgAsyncItemSaver : public CActive
    {
    public: 

        /**
        * Constructor.
        */
        IMPORT_C CMsgAsyncItemSaver();
        
        /**
        * Destructor.
        */  
        IMPORT_C ~CMsgAsyncItemSaver();

        /**
         * Call this when the operation is ready to start executing.
         * Ownership of aItemSaver is not taken.
         * @param aItemSaver asyncronous item saver
         */
        IMPORT_C void StartOperation( MMsgAsyncItemSaver* aItemSaver );

		/**
		* @param aMicroseconds time to pause. By default pauses indefinitely.
		*/
		IMPORT_C void Pause( TInt aMicroSeconds = 0 );

		IMPORT_C void Continue();

    protected: // from CActive

        void RunL();
        void DoCancel();
	    TInt RunError(TInt aError);

    private: // Own
        
        /**
        * Execute the next loop.
        */
        void NextStep();
    
    private:

        /// Reference to the operation specific item saver.
        MMsgAsyncItemSaver* iItemSaver;

		/// Saver activity state.
		TBool iPaused;
		RTimer iTimer;
		TInt iPauseDelay;
    };

#endif // CMSGASYNCITEMSAVER_H

// End of file
