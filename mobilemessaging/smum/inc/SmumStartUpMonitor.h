/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*     CSmumStartUpMonitor declaration file
*
*/



#ifndef __SMUMSTARTUPMONITOR_H
#define __SMUMSTARTUPMONITOR_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>

// FORWARD DECLARATIONS
class CMsgSimOperation;

// CLASS DECLARATION

/**
* CSmumStartUpMonitor
*
* @lib smum.dll
* @since 3.0
*/
NONSHARABLE_CLASS( CSmumStartUpMonitor ) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aWatcher a pointer to CMsgSimOperation
        */
        static CSmumStartUpMonitor* NewL( CMsgSimOperation* aWatcher );
        
        /**
        * Destructor.
        */
        virtual ~CSmumStartUpMonitor();

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Private C++ constructor.
        *
        * @param aWatcher a pointer to CMsgSimOperation
        */
        CSmumStartUpMonitor( CMsgSimOperation* aWatcher );

    private:

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        void RunL();

    private:    // Data
        CMsgSimOperation*   iWatcher;
        RProperty iStartupProperty;
    };

#endif      // __SMUMSTARTUPMONITOR_H
            
// End of File
