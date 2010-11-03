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
*   Active object to count messages background for mainview.
*
*/



#ifndef MCEUTILSIDLECLASS_H
#define MCEUTILSIDLECLASS_H

//  INCLUDES
#include <e32base.h>

#include <bldvariant.hrh>


// FORWARD DECLARATIONS

// CLASS DECLARATION

class MMceUtilsIdleClassObserver
    {
    public:
    /**
    * return ETrue if wanted to be called again
    * EFalse if operation is ended.
    */
    virtual TBool MMceUtilsIdleClassStepL() = 0;
    };





/**
*  CMceUtilsIdleClass
*
*
*/
class CMceUtilsIdleClass : public CActive
    {
    public:  // Constructors and destructor
    
        /**
        *
        */
        static CMceUtilsIdleClass* NewL(
            MMceUtilsIdleClassObserver& aObserver );

        /**
        * Destructor.
        */
        virtual ~CMceUtilsIdleClass();

    public: // New functions

        /**
        * Starts
        */
        void StartL();

        /**
        * Complete synchronously. 
        */
        void ForceFinishL();


    private:  // Functions from CActive
        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */
        virtual void RunL();

        /**
        * From CActive
        */
	    virtual TInt RunError(TInt aError);
	    
    private: // New functions
    
        /**
        * C++ constructor.
        */
        CMceUtilsIdleClass( MMceUtilsIdleClassObserver& aObserver );

        /**
        * Requests next operation.
        */
        void QueueNextL();

    private: //Data
        MMceUtilsIdleClassObserver& iObserver;
    };

#endif

// End of file
