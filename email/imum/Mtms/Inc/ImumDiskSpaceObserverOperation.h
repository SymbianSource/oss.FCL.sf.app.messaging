/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     Class for observing free disk space and if disk space goes under critical
*     level this cancels itself.
*
*
*/


#ifndef __IMUMDISKSPACEOBSERVEROPERATION_H__
#define __IMUMDISKSPACEOBSERVEROPERATION_H__

//  INCLUDES
#include <e32base.h>                // Basic Symbian Include
#include <f32file.h>                // RFs

#include "ImumOnlineOperation.h"
#include "ImumDiskSpaceObserver.h"  // CImumDiskSpaceObserver

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
* CImumDiskSpaceObserverOperation
* Class for observing free disk space and if disk space goes under critical
* level this cancels itself.
* note: Derived class MUST call BaseConstructL in its ConstuctL
*/
class CImumDiskSpaceObserverOperation :
    public CImumOnlineOperation,
    public MImumDiskSpaceObserver
    {
    public:

        /**
        * Destructor
        */
        virtual ~CImumDiskSpaceObserverOperation();

    protected: // From base classes
    protected: // Constructors

        /**
        * C++ default constructor.
        * Parameters like in CImumOnlineOperation
        */
        CImumDiskSpaceObserverOperation(
            CImumInternalApi& aMailboxApi,
            TInt aPriority,
            TRequestStatus& aObserverRequestStatus,
            MMsvProgressReporter& aReporter);

        /**
        * Opens connection to shared data and calls CImumOnlineOperation::BaseConstructL
        * note: Derived class MUST call BaseConstructL in its ConstuctL
        * @param aMtmType: mtm uid of the current operation
        */
        void BaseConstructL( TUid aMtmType );

    private: // From Baseclasses

        /**
        * Function to execute the disk space check from current drive
        * @since S60 3.0
        * @KErrNone, when everything is ok
        * @KErrDiskFull, when no disk space for operation
        */
        TInt DoDiskSpaceCheckL();

        /**
        *
        * @since S60 3.0
        */
        void HandleDiskSpaceEvent();

        /**
        *
        * @since S60 3.0
        */
        void DoCancel();

    private: // New Functions
    private: // Data

        // Disk Space Oserver
        CImumDiskSpaceObserver* iDiskObserver;
        
        // Create own instance of API because ImumDiskSpaceObserver outlives
        // the owner of iMailboxApi reference in ImumOnlineOperation
        CImumInternalApi* iEmailApi;
    };


#endif      // __IMUMDISKSPACEOBSERVEROPERATION_H__

// End of File
