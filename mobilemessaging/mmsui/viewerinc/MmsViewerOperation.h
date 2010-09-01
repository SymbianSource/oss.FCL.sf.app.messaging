/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       CMmsViewerOperation
*
*/




#ifndef __MMSVIEWEROPERATION_H
#define __MMSVIEWEROPERATION_H

// INCLUDES

#include <e32base.h>
#include <f32file.h>

#include "UniMmsViewerDocument.h"

// CONSTANTS
//#define CANCEL_TEST_CODE 1  

// MACROS

// FORWARD DECLARATIONS
// Cancel test code
#ifdef CANCEL_TEST_CODE
class CHighOperation;
#endif

// DATA TYPES

enum TMmsViewerOperationType
    {
    EMmsViewerOperationChangeSlide,
    EMmsViewerOperationLaunch,
    EMmsViewerOperationReadReport
    };

enum TMmsViewerOperationEvent
	{
	EMmsViewerOperationComplete,
    EMmsViewerOperationError,
    EMmsViewerOperationCancel
	};


// FUNCTION PROTOTYPES

// CLASS DECLARATION

class MMmsViewerOperationObserver
    {
    public:
        virtual void ViewerOperationEvent(
            TMmsViewerOperationType aOperation,
            TMmsViewerOperationEvent aEvent,
            TInt aError ) = 0;
    };

/**
* CMmsViewerOperation
*
* @lib mmsViewer.app
* @since 3.0
*/
class CMmsViewerOperation : public CActive
    {
    public:  // New methods

        /**
        * Destructor
        *
        * @since    3.0
        */
        virtual ~CMmsViewerOperation();

    protected:

        /**
        * Constructor.
        *
        * @since    3.0
        */
        CMmsViewerOperation(
            MMmsViewerOperationObserver& aObserver,
            CMmsViewerDocument& aDocument,
            RFs& aFs );

        /**
        * CompleteSelf
        * Sets active and completes
        *
        * @since 3.0
        */
        void CompleteSelf( TInt aError );

        /**
        * SetActiveNoCompleteSelf
        * Sets active and but does not complete
        *
        * @since 3.1
        */
        void SetActiveNoCompleteSelf( );

        /**
        * SetActiveNoCompleteSelf
        * Completes earlier activated operation
        *
        * @since 3.1
        */
        void CompleteActiveSelf( TInt aError );
        /**
        * IsCompleted
        * Checks whether active operation is completed
        *
        * @since 3.1
        */
        TBool IsCompleted( );
        
#ifdef CANCEL_TEST_CODE
        /**
        * 
        * Usually active objects runs with priority EPriorityStandard
        * @param aUseRandomCancel indicates how often Cancel is called. Use value 0...100:
        * 0: never
        * 1: with probability of 1%
        * ...100:  with probability of 100%.
        * >100: runs with priority EPriorityStandard+1 and Cancels immediately.
        * if aUseRandomCancel <= 100, active objects runs until it is cancelled
        */
        void LaunchCancelTest( TInt aUseRandomCancel = 100);        
        void CancelCancelTest( );        
#endif    

    protected: // data

        MMmsViewerOperationObserver&    iObserver;
        CMmsViewerDocument&             iDocument;
        RFs&                            iFs;
        TInt                            iNextState;
#ifdef CANCEL_TEST_CODE
        // Cancel test code
        CHighOperation*                 iHighOp;
#endif

    };

#endif // __MMSVIEWEROPERATION_H
